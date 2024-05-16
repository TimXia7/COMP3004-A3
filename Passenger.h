#ifndef PASSENGER_H
#define PASSENGER_H

#include <iostream>
#include <QString>
#include <memory>

class Elevator;
class Floor;
class ECS;

using namespace std;

/*
    Passenger Class

    Represents the passengers and their abilities.
*/

class Passenger {
    public:
        Passenger(QString id, int weight);
        Passenger(QString id, int weight, Floor* myFloor, ECS* ecs);
        Passenger(QString id, int weight, Elevator* myElevator, ECS* ecs);

        //Public Functions
        bool operateFloorButtons(QString direction);
        bool operateElevatorButtons(int targetFloor);
        void enterElevator(Elevator* elevator);
        void exitElevator(Floor* floor);
        void keepOpen();
        void keepClosed();
        void removeObstacle();

        QString getId() const { return (this->id); }
        int getWeight() const { return (this->weight); }
        int getFloorNumber() const;
        Elevator* getElevator(){ return myElevator; }
        Floor* getFloor(){ return myFloor; }
        QString getDirection() const { return this->direction; }
        int getTargetFloor() const { return this->targetFloor; }
        void setDirection(QString direction);
        void setTargetFloor(int targetFloor);

    private:
        //Private Attributes
        const QString id;
        const int weight;
        QString direction;
        int targetFloor;
        Elevator* myElevator;
        Floor* myFloor;
        ECS* ecs;
};

#endif
