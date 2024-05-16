#ifndef FLOORBUTTONS_H
#define FLOORBUTTONS_H

#include <iostream>
#include <QString>

using namespace std;

/*
    FloorButtons Class

    Represents Floor Buttons of the simulation.

    Buttons can be pressed to add in floor requests.

    Also holds the state of the buttons.
*/

class FloorButtons {
    public:
        FloorButtons(bool upState, bool downState);
        // using default destructor

        void pressUp();
        void pressDown();
        void removeUp();
        void removeDown();
        bool getUpState() const ;
        bool getDownState() const ;

    private:
        bool downState;
        bool upState;
};

#endif
