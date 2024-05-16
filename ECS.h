#ifndef ECS_H
#define ECS_H

#include <iostream>
#include <QVector>
#include <QString>
#include <memory>
#include <QDebug>
#include <cmath>

#include "FloorRequest.h"
#include "ElevatorRequest.h"

using namespace std;

class Floor;
class Elevator;
class Passenger;

/*
    ECS Class

    The elevator control system manages all the floors and elevators, but mostly the elevators.

    It tells the elevators how to move up and down floors, when to stop to pick up floor requests,
    and ordering elevators to move to them.

    Elevators handle their own in elevator requests.

    Notes for more confusing functions are listed.
*/

class ECS {
    public:
        ECS();

        //Public Functions, more notes for individual functions in .cpp

        //Manages floor request and actions that initiate when a new request comes in.
        bool addFloorReq(int targetFloor, QString direction);
        void previousFloorReq();

        //Managing and updating floors and their requests.
        void confirmedFloorRequest(int targetFloor, QString direction);
        void servicedFloor(int floorNumber, QString direction);
        shared_ptr<Floor>& newFloor(QString direction, int floorNumber, int elevatorNumber);

        //ECS Logic, determines elevator behavior
        bool checkElevatorStates();
        Elevator* nearestElevator(int targetFloor);
        Elevator* elevatorPresent(int floor);
        void makeDecision(int floorNumber, int elevatorNumber);

        //Functions to recognize when a floor request matches
        bool matchingFloorRequest(int floorNumber, QString direction);
        bool matchingFloorRequestAtEdge(int floorNumber);

        //Data Strucutre Management
        bool addElevator(int elevatorNumber, int floorNumber);
        bool addFloor(int floorNumber);
        bool addPassenger(QString id, int weight, int floorNumber);


        //Emergency Testing:
        void interruptElevators();
        bool buildingSafetyAnswer();
        void broadcastFire();
        void broadcastPowerout();
        void powerOff();


        //Getters and setters:
        shared_ptr<Elevator> getElevator(int elevatorNumber);
        shared_ptr<Floor> getFloor(int floorNumber);
        shared_ptr<Passenger> getPassenger(QString id);

    private:
        //Private Attributes (Just data structures for the simulation entities that the ECS manages)
        QVector<shared_ptr<Floor>> floors;
        QVector<shared_ptr<Elevator>> elevators;
        QVector<shared_ptr<Passenger>> passengers;
        QVector<shared_ptr<FloorRequest>> floorRequests;
};

#endif

