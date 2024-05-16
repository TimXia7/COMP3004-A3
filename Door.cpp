#include "Door.h"

Door::Door(){
    isOpen = false;
}

//using default deconstructor

void Door::open(){
    this->isOpen = true;
}

void Door::close(){
    this->isOpen = false;
}

bool Door::getState() const {
    return (this->isOpen);
}

void Door::ringBell() const {
    qInfo("*ring ring!*");
}
