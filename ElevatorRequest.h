#ifndef ELEVATORREQUEST_H
#define ELEVATORREQUEST_H

#include <QString>
#include <iostream>

using namespace std;

/*
    ElevatorRequest Class

    Represents in elevator requests.
*/

class ElevatorRequest{

    public:
        ElevatorRequest(int targetFloor, int elevatorNumber);
        //using default destructor

        int getFloor() const ;
        int getElevatorNumber() const ;

    private:
        const int targetFloor;
        const int elevatorNumber;
};

#endif
