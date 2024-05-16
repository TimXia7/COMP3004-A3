#include "Floor.h"
#include "Elevator.h"
#include "Passenger.h"
#include "ECS.h"

Floor::Floor(int floorNumber, ECS* ecs) : floorButtons(false, false), ecs(ecs){
    this->floorNumber = floorNumber;
}

// Managing data structures
bool Floor::addPassenger(Passenger* passenger){
    passengers.push_back(passenger);

    return true;
}

bool Floor::deletePassenger(QString id){

    for (int i = 0; i < passengers.size(); ++i){
        if (passengers[i]->getId() == id){
            passengers.remove(i);
            return true;
        }
    }

    return false;
}

bool Floor::deletePassenger(int index){
    if (passengers.size()-1 > index){
        return false;
    } else {
        passengers.remove(index);
    }
    return true;
}

bool Floor::addElevator(Elevator* newElevator){
    elevators.push_back(newElevator);

    return true;
}

bool Floor::deleteElevator(int elevatorNumber){

    for (int i = 0; i < elevators.size(); ++i){
        if (elevators[i]->getElevatorNumber() == elevatorNumber){
            elevators.remove(i);
            return true;
        }
    }

    return false;
}


// helps sensor detect the floor
int Floor::detectMe() const {
    return floorNumber;
}

//turns off floor buttons
void Floor::serviced(QString direction) {
    if (direction == "up"){
        floorButtons.removeUp();
    } else if (direction  == "down"){
        floorButtons.removeDown();
    }

}

// manages floorButton status
void Floor::request(QString direction) {
    if (direction == "up"){
        floorButtons.pressUp();
    } else if (direction  == "down"){
        floorButtons.pressDown();
    }
}

//Determines if an elevator is on the floor for the ECS logic.
Elevator* Floor::elevatorPresent() const {
    Elevator* present = nullptr;
    //Get first idle elevator
    for (int start = 0; start < elevators.size(); ++start){
        if (elevators[start] != nullptr && elevators[start]->getState() == "idle"){
            present = (elevators[start]);
            return present;
        }
    }

    return nullptr;
}



//getters and setters:
int Floor::getFloorNumber() {
    return this->floorNumber;
}

Passenger* Floor::getPassenger(int index){
    if (passengers.empty()){
        return nullptr;
    } else {
        return passengers[index];
    }
}

bool Floor::getDownState() const {
    return this->floorButtons.getDownState();
}

bool Floor::getUpState() const {
    return this->floorButtons.getUpState();
}










