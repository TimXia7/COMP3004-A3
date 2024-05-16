#include "Passenger.h"
#include "Floor.h"
#include "Elevator.h"
#include "ECS.h"

Passenger::Passenger(QString id, int weight) : id(id), weight(weight), myElevator(nullptr), myFloor(nullptr), ecs(nullptr){
    this->direction = "NULL";
    this->targetFloor = -1;
}

Passenger::Passenger(QString id, int weight, Floor* myFloor, ECS* ecs) : id(id), weight(weight), myElevator(nullptr), myFloor(myFloor), ecs(ecs){
    this->direction = "NULL";
    this->targetFloor = -1;
}

Passenger::Passenger(QString id, int weight, Elevator* myElevator, ECS* ecs) : id(id), weight(weight), myElevator(myElevator), myFloor(nullptr), ecs(ecs){
    this->direction = "NULL";
    this->targetFloor = -1;
}

bool Passenger::operateFloorButtons(QString direction){

    int floorNumber = this->getFloorNumber();
    if (floorNumber == -1){
        return false;
    }

    this->ecs->addFloorReq(this->getFloorNumber(), direction);
    //call the elevator down from the ECS class.

    this->setDirection(direction);

    return true;
}

bool Passenger::operateElevatorButtons(int targetFloor){
    if (this->getElevator() != nullptr){
        this->setTargetFloor(targetFloor);
        this->getElevator()->elevatorButtons(targetFloor);
        return true;
    }
    return false;
}

void Passenger::enterElevator(Elevator* elevator){
    this->myElevator = elevator;

    qInfo() << "Passenger " + this->getId() + " has entered the elevator.";

    //myFloor is not cleared to let the passenger leave the elevator. Should not be able to be ran when owns an elevator.
}

void Passenger::exitElevator(Floor* floor){
    this->setDirection("NULL");
    this->targetFloor = -1;
    this->myElevator = nullptr;
    this->myFloor = floor;

    qInfo() << "Passenger " + this->getId() + " has left the elevator.";
}

void Passenger::keepOpen(){
    if (this->myElevator != nullptr){
        myElevator->keepOpen();
    }
}

void Passenger::keepClosed(){
    if (this->myElevator != nullptr){
        myElevator->keepClosed();
    }
}

void Passenger::removeObstacle(){
    if (this->myElevator != nullptr){
        myElevator->removeObstacle();
        qInfo() << "Passenger " + this->getId() + " removed the obstacle!";
    }
}


int Passenger::getFloorNumber() const {
    //if floor exists, shouldn't be called when passenger is in an elevator
    if (myFloor) {
        return (this->myFloor->getFloorNumber());
    }
    return -1;
}

void Passenger::setDirection(QString direction){
    this->direction = direction;
}

void Passenger::setTargetFloor(int targetFloor){
    this->targetFloor = targetFloor;
}

