#include "FloorRequest.h"

FloorRequest::FloorRequest(int targetFloor, QString direction) : targetFloor(targetFloor), direction(direction){}

int FloorRequest::getFloor() const {
    return targetFloor;
}

QString FloorRequest::getDirection() const {
    return direction;
}
