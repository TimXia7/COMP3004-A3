#include "ECS.h"
#include "Floor.h"
#include "Elevator.h"
#include "Passenger.h"

ECS::ECS(){}

// When a floor button is pressed on a floor, this function is called.
// it determines what to do when a new request is added (3 scenarios).
// it seems rather large, but it is simple with added commments and does fulfill only one function
bool ECS::addFloorReq(int targetFloor, QString direction){
    Elevator* presentElevator = this->elevatorPresent(targetFloor);

    // If there is already an elevator at the floor, then it should open right away for the passenger..
    if (presentElevator != nullptr){
        confirmedFloorRequest(targetFloor, direction);

        presentElevator->setState("active");
        presentElevator->newFloor(presentElevator->sensor());
        return true;
    }

    // If there is no elevator readily present, then check to see if all elevators are idle.
    // If they are, then call the closest elevator over and make a new floor request.
    if (!(checkElevatorStates())){
        Elevator* nearest = this->nearestElevator(targetFloor);

        if (nearest->sensor() == targetFloor && nearest->getDirection() == direction){
            nearest->stop();
        } else {
            confirmedFloorRequest(targetFloor, direction);
            nearest->setTargetFloor(targetFloor);
            if (nearest->sensor() > targetFloor){
                nearest->start("down");
            } else {
                nearest->start("up");
            }
            return true; //if an elevator is automatically called
        }
    }

    //If there is already an elevator active, wait and see an elevator goes by later
    // (opportunistic)
    confirmedFloorRequest(targetFloor, direction);

    return false;
}

// Similar to the function before, but is called when the whole system sleeps,
// This is to ensure there are no unanswered requests
// e.g. if a floor request is created the moment elevators sleep
void ECS::previousFloorReq(){
    if (!(floorRequests.empty())){

        FloorRequest* floorRequest = this->floorRequests[0].get();
        int targetFloor = floorRequest->getFloor();
        QString direction = floorRequest->getDirection();
        Elevator* presentElevator = this->elevatorPresent(targetFloor);

        // If there is an elevator present, call it right away.
        if (presentElevator != nullptr){
            presentElevator->setState("active");
            presentElevator->newFloor(presentElevator->sensor());
        }

        // Call an elevator right away if there is not present elevator
        if (!(checkElevatorStates())){
            Elevator* nearest = this->nearestElevator(targetFloor);

            if (nearest->sensor() == targetFloor && nearest->getDirection() == direction){
                nearest->stop();
            } else {
                confirmedFloorRequest(targetFloor, direction);
                nearest->setTargetFloor(targetFloor);
                if (nearest->sensor() > targetFloor){
                    nearest->start("down");
                } else {
                    nearest->start("up");
                }
            }
        } else {
            qInfo("prevFloorReq toss!");
        }
    }
}

//Add a floor request when nessasary
void ECS::confirmedFloorRequest(int targetFloor, QString direction){
    shared_ptr<FloorRequest> newFloorRequest = make_shared<FloorRequest>(targetFloor, direction);
    floorRequests.push_back(newFloorRequest);
    this->getFloor(targetFloor)->request(direction);
}

void ECS::servicedFloor(int floorNumber, QString direction){
    this->getFloor(floorNumber)->serviced(direction);
}

// Despite its length, this function is quite simple.
// It is called when the elevator wants to move in a certain direction.
// The ECS returns the new floor to the elevator.
// Its length is due to edge cases.
shared_ptr<Floor>& ECS::newFloor(QString direction, int floorNumber, int elevatorNumber){
    //Update to update Floor's arrays of elevators
    Elevator* newElevator = (this->getElevator(elevatorNumber)).get();

    if (direction == "up"){
        if (floorNumber >= (floors.size())){
            //Elevator is going up and has reached its destination
            return floors[floorNumber-1];
        } else {
            //Elevator is going up to the next floor:
            if (floors[floorNumber] == nullptr){
                throw runtime_error("newFloor accessed a floor out of bounds. (While going up)");
            }
            floors[floorNumber-1]->deleteElevator(elevatorNumber);
            floors[floorNumber]->addElevator(newElevator);
            return floors[floorNumber];
        }
    } else {
        if (floorNumber <= 1){
            //Elevator is going down and has reached its destination
            return floors[floorNumber-1];
        } else {
            //Elevator is going down to the next floor
            if (floors[floorNumber-2] == nullptr){
                throw runtime_error("newFloor accessed a floor out of bounds. (While going down)");
            }
            floors[floorNumber-1]->deleteElevator(elevatorNumber);
            floors[floorNumber-2]->addElevator(newElevator);
            return floors[floorNumber-2];
        }
    }
}

// False for all elevators idle, True of at least one active
// Helps other functions
bool ECS::checkElevatorStates(){
    for (int i = 0; i < elevators.size(); ++i){
        if (elevators[i]->getState() == "active"){
            return true;
        }
    }
   return false;
}

// Finds and returns the nearest IDLE elevator.
Elevator* ECS::nearestElevator(int targetFloor){

    if (elevators.empty()){
        return nullptr;
    }

    // First, get an idle elevator to compare
    Elevator* nearest = nullptr;
    for (int start = 0; start < elevators.size(); ++start){
        if (elevators[start] != nullptr && elevators[start]->getState() == "idle"){
            nearest = (elevators[start]).get();

            int smallestDifference = abs(nearest->sensor() - targetFloor);
            int currDifference;

            // Now, compare it to all other idle elevators
            for (int i = start; i < elevators.size(); ++i){
                currDifference = abs(elevators[i]->sensor() - targetFloor);
                if (currDifference < smallestDifference || (currDifference == smallestDifference && (elevators[i]->sensor() > nearest->sensor()))){
                    nearest = (elevators[i]).get();
                    smallestDifference = currDifference;
                }
            }
            //This can be nullptr if there are no valid elevators.
            return nearest;
        }
    }

    return nullptr;
}

Elevator* ECS::elevatorPresent(int floor) {
    return this->getFloor(floor)->elevatorPresent();
}

// This function determines what elevators should do when they reach a new floor.
void ECS::makeDecision(int floorNumber, int elevatorNumber){
    Elevator* elevator = (this->getElevator(elevatorNumber)).get();

    bool isIdle = false;
    if (this->getElevator(elevatorNumber)->getWeight() <= 0){
        isIdle = true;
    }

    // Checks if the elevator is at the top most and bottom most floor (for edge cases!), or should go idle.
    if (floorNumber == floors[floors.size()-1]->getFloorNumber() || floorNumber == floors[0]->getFloorNumber() || isIdle){
        // If there is a floor request to stop for or the elevator is at its target floor for in elevator requests, stop.
        if (matchingFloorRequestAtEdge(floorNumber) || elevator->getTargetFloor() == floorNumber){
            elevator->stop();
        } else {
            elevator->readyToMove();
        }
    } else {
        // Same as above, but the elevator is not at the top or bottom most floor.
        if (matchingFloorRequest(floorNumber, elevator->getDirection()) || elevator->getTargetFloor() == floorNumber){
            elevator->stop();
        } else {
            elevator->readyToMove();
        }
    }
}

bool ECS::matchingFloorRequest(int floorNumber, QString direction){
    bool foundRequest = false;
    for (int i = floorRequests.size() - 1; i >= 0; --i){
        if (floorNumber == floorRequests[i]->getFloor() && direction == floorRequests[i]->getDirection()){
            floorRequests.remove(i);
            foundRequest = true;
        }
    }
    return foundRequest;
}

bool ECS::matchingFloorRequestAtEdge(int floorNumber){
    bool foundRequest = false;
    for (int i = floorRequests.size() - 1; i >= 0; --i){
        if (floorNumber == floorRequests[i]->getFloor()){
            floorRequests.remove(i);
            foundRequest = true;
        }
    }
    return foundRequest;
}

// Below are just functions to manage the data structures
bool ECS::addElevator(int elevatorNumber, int floorNumber){

    if (floors.size() <= 0) {
        return false;
    }

    shared_ptr<Elevator> newElevator = make_shared<Elevator>(elevatorNumber, this, (floors[floorNumber-1].get()));

    elevators.push_back(newElevator);
    floors[floorNumber-1]->addElevator((newElevator).get());

    return true;
}

bool ECS::addFloor(int floorNumber){

    shared_ptr<Floor> newFloor = make_shared<Floor>(floorNumber, this);

    floors.push_back(newFloor);

    return true;
}

bool ECS::addPassenger(QString id, int weight, int floorNumber){
    shared_ptr<Passenger> newPassenger = make_shared<Passenger>(id, weight, (this->getFloor(floorNumber).get()), this);

    this->getFloor(floorNumber)->addPassenger(newPassenger.get());

    passengers.push_back(newPassenger);

    return true;
}


//Emergency Testing functions:

bool ECS::buildingSafetyAnswer(){
    return false;
}

// Tells all functions to stop what they're doing
void ECS::interruptElevators(){
    for (int i = 0; i < elevators.size(); ++i){
        elevators[i]->interrupt();
    }
}

void ECS::broadcastFire(){
    for (int i = 0; i < elevators.size(); ++i){
        elevators[i]->interrupt();
        elevators[i]->fireAlarm();
    }
}

void ECS::broadcastPowerout(){
    for (int i = 0; i < elevators.size(); ++i){
        elevators[i]->interrupt();
        elevators[i]->powerOutage();
    }
}

// Called after fire and power outages.
void ECS::powerOff(){

    qInfo("ECS is powering off.");

    for (int i = 0; i < elevators.size(); ++i){
        elevators[i]->powerOff();
    }

    //doesn't need to delete self as it assumes the one using it will manage its memory.
}






//getters and setters:

shared_ptr<Elevator> ECS::getElevator(int elevatorNumber){
    for (int i = 0; i < elevators.size(); ++i){
        if (elevators[i]->getElevatorNumber() == elevatorNumber){
            return elevators[i];
        }
    }
    throw runtime_error("getElevator could not return a valid elevator.");
}

shared_ptr<Floor> ECS::getFloor(int floorNumber){
    for (int i = 0; i < floors.size(); ++i){
        if (floors[i]->getFloorNumber() == floorNumber){
            return floors[i];
        }
    }
    throw runtime_error("getFloor could not return a valid floor.");
}

shared_ptr<Passenger> ECS::getPassenger(QString id){
    for (int i = 0; i < floors.size(); ++i){
        if (passengers[i]->getId() == id){
            return passengers[i];
        }
    }
    throw runtime_error("getPassenger could not return a valid passenger.");
}







