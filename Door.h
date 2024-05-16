#ifndef DOOR_H
#define DOOR_H

#include <QString>

using namespace std;

/*
    Door Class

    This is a class to help the Elevator class represent the door.

    It should be pretty straightforward, so no other header file notes are added.
*/

class Door {
    public:
        Door();

        void open();
        void close();
        bool getState() const ;
        void ringBell() const ;

    private:
        //Determines the open status of the elevator door.
        bool isOpen;
};

#endif
