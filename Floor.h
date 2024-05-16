#ifndef FLOOR_H
#define FLOOR_H

#include <iostream>
#include <QVector>
#include <QString>
#include <QDebug>
#include <memory>

#include "FloorButtons.h"
#include "Door.h"

class ECS;
class Elevator;
class Passenger;

using namespace std;

/*
    Floor Class

    Represents floors of the simulation.

    Can take floor requests and talk to the ECS.

    Cannot directly communicate to Elevators, only for managing passenger locations and similar.
*/

class Floor {
    public:
        Floor(int floorNumber, ECS* ecs);

        // Managing Data Structures
        bool addPassenger(Passenger* passenger);
        bool deletePassenger(QString id);
        bool deletePassenger(int index);
        bool addElevator(Elevator* newElevator);
        bool deleteElevator(int elevatorNumber);

        int detectMe() const ;
        void serviced(QString direction);
        void request(QString direction);
        Elevator* elevatorPresent() const ;


        //getters and setters:
        int getFloorNumber();
        Passenger* getPassenger(int index);
        int getPassengerSize(){ return passengers.size(); }
        bool getUpState() const ;
        bool getDownState() const ;

    private:
        int floorNumber;
        FloorButtons floorButtons;
        QVector<Passenger*> passengers;
        QVector<Elevator*> elevators;
        ECS* ecs;
};

#endif
