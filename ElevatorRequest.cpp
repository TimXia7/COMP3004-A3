#include "ElevatorRequest.h"

ElevatorRequest::ElevatorRequest(int targetFloor, int elevatorNumber) : targetFloor(targetFloor), elevatorNumber(elevatorNumber){}

int ElevatorRequest::getFloor() const {
    return targetFloor;
}

int ElevatorRequest::getElevatorNumber() const {
    return elevatorNumber;
}
