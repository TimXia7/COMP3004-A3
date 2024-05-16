#include "Elevator.h"
#include "Floor.h"
#include "ECS.h"
#include "Door.h"

Elevator::Elevator(int elevatorNumber, ECS* ecs, Floor* floor) : openInterval(10000) ,elevatorNumber(elevatorNumber), weightLimit(150), floor(floor), door(), ecs(ecs){
    state = "idle";
    direction = "NULL";
    obstacle = false;
    targetFloor = -1;

    //Initiate Timers:
    moveTimer = new QTimer(this);
    moveTimer->setSingleShot(true);
    connect(moveTimer, &QTimer::timeout, this, &Elevator::move);
    moveTimer->setInterval(2000);

    emergencyTimer = new QTimer(this);
    emergencyTimer->setSingleShot(true);
    connect(emergencyTimer, &QTimer::timeout, this, &Elevator::moveToFirstFloor);
    emergencyTimer->setInterval(2000);

    enterTimer = new QTimer(this);
    enterTimer->setSingleShot(true);
    connect(enterTimer, &QTimer::timeout, this, &Elevator::passengerMovement);
    enterTimer->setInterval(1500);

    doorOpenTimer = new QTimer(this);
    doorOpenTimer->setSingleShot(true);
    connect(doorOpenTimer, &QTimer::timeout, this, &Elevator::close);
    doorOpenTimer->setInterval(openInterval);

    helpTimer = new QTimer(this);
    helpTimer->setSingleShot(true);
    connect(helpTimer, &QTimer::timeout, this, &Elevator::buildingSafetyService);
    helpTimer->setInterval(5000);

}

Elevator::~Elevator(){
    //delete timers
    delete moveTimer;
    delete emergencyTimer;
    delete enterTimer;
    delete doorOpenTimer;
    delete helpTimer;
}

// When the elevator moves from idle state, it needs to set all nessasary conditions.
void Elevator::start(QString direction){
    this->state = "active";
    this->direction = direction;
    this->readyToMove();

    //does not set target floor, that's for in elevator requests only!
}

// Handles the actual movement of the elevator by asking the ECS and sensor where it is.
// needs function readyToMove() below to handle any logic before movement
void Elevator::move(){
    this->floor = (this->ecs->newFloor(direction, this->sensor(), this->getElevatorNumber())).get();

    QString qMsg = "moved to floor " + QString::number(this->sensor());
    this->display(qMsg);

    this->newFloor(this->sensor());
}

// Before actually moving, it processes with closed doors, to determine if it should go up or down, based on requests.
// See state diagram for that.
// Has the move timer to simulate time between floors.
void Elevator::readyToMove(){
    this->door.close();
    this->setState("active");

    // If there is an in elevator request, prioritize that.
    // If this block doesn't run, then the ECS called it to a floor request.
    if (!(elevatorRequests.empty())){
        // Find the closest elevator request and fulfill it first
        ElevatorRequest* closestRequest = (elevatorRequests[0]).get();
        int smallestDifference = abs(this->sensor() - closestRequest->getFloor());
        int newDifference;
        for (int i = 1; i < elevatorRequests.size(); ++i){
            newDifference = abs(this->sensor() - elevatorRequests[i]->getFloor());
            if (smallestDifference > newDifference){
                closestRequest = elevatorRequests[i].get();
                smallestDifference = newDifference;
            }
        }
        if (closestRequest->getFloor() > this->sensor()){
            this->direction = "up";
        } else if (closestRequest->getFloor() < this->sensor()) {
            this->direction = "down";
        } else {
            // If the elevator request is at the current floor, stop now.
            this->stop();
        }
        this->setTargetFloor(closestRequest->getFloor());

    }

    moveTimer->start();
}

int Elevator::sensor(){
    return (this->floor->detectMe());
}

// Ask the ECS for the next floor, with the help of the sensor.
void Elevator::newFloor(int floorNumber){
    ecs->makeDecision(floorNumber, this->getElevatorNumber());
}


// Managing passengers
bool Elevator::addPassenger(Passenger* passenger){
    this->passengers.push_back(passenger);
    return true;
}

bool Elevator::deletePassenger(QString id){
    for (int i = 0; i < passengers.size(); ++i){
        if (passengers[i]->getId() == id){
            passengers.remove(i);
            return true;
        }
    }

    return false;
}

// Add in elevator request when the buttons are pressed
void Elevator::elevatorButtons(int targetFloor){
    shared_ptr<ElevatorRequest> newElevatorRequest = make_shared<ElevatorRequest>(targetFloor, this->elevatorNumber);
    elevatorRequests.push_back(newElevatorRequest);

    //If the doors are already closed, then move the elevator right away
    if (this->door.getState() == false){
        this->readyToMove();
    }
}

// Stop the elevator with the purpose of passenger movement.
// enterTimer is the 10 second wait period.
void Elevator::stop(){

    QString qStr = "Elevator: " + QString::number(elevatorNumber) + " has stopped to let the passengers movve.";
    qInfo() << qStr;

    // Tell the ECS what floor it has serviced.
    if (this->targetFloor == -1 || this->elevatorRequests.empty()){
        this->ecs->servicedFloor(this->sensor(),"up");
        this->ecs->servicedFloor(this->sensor(),"down");
    } else {
        this->ecs->servicedFloor(this->sensor(), this->getDirection());
    }

    enterTimer->start();
}

// This function seems quite bloated, but it does fulfill only one function,
// the logic behind passengers moving between floors.

// Hopefully with the brief comments below, it will actually become easily understandable.
void Elevator::passengerMovement(){
    this->open();

    // Remove elevator requests of the passengers who are moving out of the elevator
    for (int i = elevatorRequests.size()-1; i >= 0; --i){
        if (elevatorRequests[i]->getFloor() == this->sensor()){
            elevatorRequests.remove(i);
        }
    }
    // Next, remove those passengers from the elevator
    Floor* floor = this->ecs->getFloor(this->sensor()).get();
    for (int i = passengers.size()-1; i >= 0; --i){
        if (passengers[i]->getTargetFloor() == this->sensor()){
            passengers[i]->exitElevator(floor);
            floor->addPassenger(passengers[i]);
            this->deletePassenger(passengers[i]->getId());
        }
    }

    Passenger* nextPassenger;
    bool getOn = false;

    // Now, move the elevators into the elevator, using the the bool getOn to determine if a given passenger should enter.
    for (int i = floor->getPassengerSize()-1; i >= 0; --i){
        nextPassenger = floor->getPassenger(i);
        if (nextPassenger != nullptr){
            // If the passenger is moving the direction of the elevator, get on.
            if (nextPassenger->getDirection() != "NULL"){
                getOn = nextPassenger->getDirection() == this->getDirection();
            }
            // If the elevator was called by the ECS to pick up passengers, then direction doesn't matter (no in elevator requests)
            // Therefore, everyone get on.
            if ((this->targetFloor == -1 || this->elevatorRequests.size() <= 0) && nextPassenger->getDirection() != "NULL") {
                getOn = true;
            }
        } else {
            getOn = false;
        }
        if (getOn){
            //Passenger enters:
            this->addPassenger(nextPassenger);
            floor->deletePassenger(nextPassenger->getId());
            QString qStr = "Passenger, " + nextPassenger->getId() + " has entered elevator " + QString::number(this->getElevatorNumber());
            qInfo() << qStr;

            nextPassenger->enterElevator(this);
        }
    }

    //Service floors and return to idle if that request was the last.
    lastRequest();

    //close after a delay!
    doorOpenTimer->setInterval(openInterval);
    doorOpenTimer->start();
}

// If the last elevator request was satisfied, go idle
void Elevator::lastRequest(){
    this->ecs->matchingFloorRequestAtEdge(this->sensor());
    if (this->targetFloor == -1 || this->elevatorRequests.empty()){
        this->ecs->servicedFloor(this->sensor(),"up");
        this->ecs->servicedFloor(this->sensor(),"down");
    } else {
        this->ecs->servicedFloor(this->sensor(), this->getDirection());
    }

    // Nobody left? go idle.
    if (this->getWeight() == 0 && elevatorRequests.size() <= 0){
        this->idle();
    }
}

void Elevator::idle(){
    QString qStr = "Elevator " + QString::number(elevatorNumber) + ": has went idle!";

    qInfo() << qStr;
    this->setState("idle");
    this->setTargetFloor(-1);
    this->direction = "NULL";

    this->ecs->previousFloorReq();
}

void Elevator::open(){
    this->door.open();
    this->door.ringBell();
}

// After checking for obstacles and weight limit, close the doors.
void Elevator::close(){

    if (checkObstacle() && weightSensor()){
        qInfo() << "Elevator " + QString::number(elevatorNumber) + ": is closing!";
    }

    this->door.close();
    this->door.ringBell();

    if (elevatorRequests.empty() && this->getWeight() == 0 && (this->getState() != "idle")){
        this->idle();
    } else if (!(elevatorRequests.empty())){
        this->readyToMove();
    }

}

// The keep open button is pressed, so it restarts the open timer.
void Elevator::keepOpen(){
    // The elevator is either, moving or in emergency, so don't open the doors!
    if (moveTimer->isActive() || emergencyTimer->isActive() || helpTimer->isActive()){
        qInfo() << "Can't open the door right now!";
        return;
    }

    // If not, then keep those doors open.
    QString qStr = "Elevator " + QString::number(elevatorNumber) + ": is forced to open longer!";
    qInfo() << qStr;

    if (door.getState() == false){
        this->open();
    }

    doorOpenTimer->stop();
    doorOpenTimer->setInterval(openInterval);
    doorOpenTimer->start();
}

// The keep closed button is pressed, so it ends the timer early.
void Elevator::keepClosed(){
    if (this->door.getState() == true){
        doorOpenTimer->stop();
        doorOpenTimer->setInterval(1);
        doorOpenTimer->start();
    } else {
        qInfo("(The door is already closed!)");
    }
}



//Emergency Testing:

void Elevator::helpButton(){
    this->state = "emergency";
    for (int i = elevatorRequests.size()-1; i >= 0; --i){
        elevatorRequests.remove(i);
    }
    if (this->door.getState()){
        qInfo("the door is already open, go get help elsewhere!");
    } else {
        this->interrupt();
        qInfo() << "Elevator " + QString::number(elevatorNumber) + ": is contacting the building safety service!";;
        helpTimer->start();
    }

}

bool Elevator::buildingSafetyService(){
    if (this->ecs->buildingSafetyAnswer()){
        qInfo("Building safety service here!");
        return true;
    } else {
        this->emergencyCall();
        return false;
    }
}

void Elevator::emergencyCall(){
    qInfo("911 call issued! But it turns out everything is fine, continue on your trip.");
    this->state = "idle";
}

void Elevator::fireAlarm(){
    this->direction = "down";

    QString qMsg = "Fire Alarm, Remain Calm";
    this->display(qMsg);
    this->audio(qMsg);
    this->emergencyTimer->start();

}

void Elevator::powerOutage(){
    this->direction = "down";

    QString qMsg = "Power Outage, Remain Calm";
    this->display(qMsg);
    this->audio(qMsg);
    this->emergencyTimer->start();
}

bool Elevator::checkObstacle(){
    if (this->obstacle){
        display("Please remove the obstacle blocking the door");
        audio("Please remove the obstacle blocking the door");

        if (!(passengers.empty())){
            passengers[0]->removeObstacle();
        } else {
            qInfo("The elevator doors just pushed the obstacle out of the way...");
            this->removeObstacle();
        }
        return false;
    }
    return true;
}

// Despite the larger function, it just checks the weight of all passengers, and sees if it is over limit
bool Elevator::weightSensor(){
    if (this->getWeight() > this->weightLimit){
        display("passenger or cargo load exceeds carrying capacity, please remove weight");
        audio("passenger or cargo load exceeds carrying capacity, please remove weight");

        Passenger* passenger = passengers[0];
        if (passenger){
            int targetFloor = passenger->getTargetFloor();
            passenger->exitElevator(floor);
            ecs->getFloor(this->sensor())->addPassenger(passenger);
            this->deletePassenger(passenger->getId());

            if (this->getWeight() <= 0){
                this->interrupt();

                for (int i = elevatorRequests.size()-1; i >= 0; --i){
                    if (elevatorRequests[i]->getFloor() == targetFloor){
                        elevatorRequests.remove(i);
                    }
                }

            } else {
                qInfo() << "Elevator " + QString::number(elevatorNumber) + " continues after the weight sensor is satisfied.";
            }
            return false;
        }
    }
    return true;
}

void Elevator::interrupt(){
    moveTimer->stop();
    emergencyTimer->stop();
    enterTimer->stop();
    doorOpenTimer->stop();
}

// Move to first floor in an event of an emergency.
void Elevator::moveToFirstFloor(){
    this->direction = "down";
    this->floor = (this->ecs->newFloor(direction, this->sensor(), this->getElevatorNumber())).get();

    if (this->sensor() == 1){
        this->idle();
    } else {
        emergencyTimer->start();
    }
}

void Elevator::powerOff() {
    setTargetFloor(-1);
    state = "off";
    targetFloor = -1;

    QString qMsg = "Elevator Number " + QString::number(this->elevatorNumber) + " is powering off.";
    qInfo() << qMsg;
}


//Getters and setters
int Elevator::getElevatorNumber() const {
    return (this->elevatorNumber);
}

void Elevator::setTargetFloor(int targetFloor){
    this->targetFloor = targetFloor;
}

QString Elevator::getState() const {
    return this->state;
}

int Elevator::getWeight() const {
    int weight = 0;
    for (int i = 0; i < passengers.size(); ++i){
        weight += passengers[i]->getWeight();
    }
    return weight;
}

void Elevator::setState(QString state) {
    this->state = state;
}

QString Elevator::getDirection() const {
    return this->direction;
}

bool Elevator::isButtonSelected(int number) const {
    //if number has an elevator request, respond yes so the buttons can be illuminated.
    for (int i = 0; i < elevatorRequests.size(); ++i){
        if (elevatorRequests[i]->getFloor() == number){
            return true;
        }
    }
    return false;
}


// Audio and display functions.
void Elevator::display(QString message) const {
    QString qMsg = "E" + QString::number(this->elevatorNumber) + " DISPLAY: " + message;
    qInfo() << qMsg;
}

void Elevator::audio(QString message) const {
    QString qMsg = "E" + QString::number(this->elevatorNumber) + " AUDIO:   " + message;
    qInfo() << qMsg;
}

