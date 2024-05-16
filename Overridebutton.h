#ifndef OVERRIDEBUTTON_H
#define OVERRIDEBUTTON_H

#include <iostream>
#include <QString>
#include <chrono>
#include <thread>

using namespace std;

class OverrideButton {
    public:
        OverrideButton();

        bool getPressedState() const ;
        void hold();
        void release();

    private:
        bool pressed;
};

#endif
