#include "Overridebutton.h"

OverrideButton::OverrideButton() {
    this->pressed = false;
}


bool OverrideButton::getPressedState() const {
    return pressed;
}

void OverrideButton::hold(){
    pressed = true;
}

void OverrideButton::release(){
    pressed = false;
}
