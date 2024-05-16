#include "FloorButtons.h"


FloorButtons::FloorButtons(bool upState, bool downState){
    this->upState = upState;
    this->downState = downState;
}

//default destructor is being used


void FloorButtons::pressUp(){
    upState = true;
}

void FloorButtons::pressDown(){
    downState = true;
}

void FloorButtons::removeUp(){
    upState = false;
}

void FloorButtons::removeDown(){
    downState = false;
}

bool FloorButtons::getUpState() const {
    return upState;
}

bool FloorButtons::getDownState() const {
    return downState;
}

