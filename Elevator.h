#ifndef ELEVATOR_H
#define ELEVATOR_H

#include <iostream>
#include <QVector>
#include <QString>
#include <QDebug>
#include <memory>

#include <QObject>
#include <QTimer>

#include "ElevatorRequest.h"
#include "Door.h"
#include "Passenger.h"

class Passneger;
class Floor;
class ECS;

using namespace std;

/*
    Elevator Class

    The elevator is in charge of fulfilling the elevator role in the simulation.

    It requires the ECS to tell it what to do, but handles its own in elevator requests.

    Notes for more confusing functions are listed.
*/

class Elevator : public QObject {
    Q_OBJECT //inherit qobj stuff for timers

    public:
        Elevator(int elevatorNumber, ECS* ecs, Floor* floor);
        ~Elevator();

        //Public Functions

        // With the help if the ECS, passengerMovement(), and slots, moves the elevators
        void start(QString direction);
        void newFloor(int floorNumber);
        int sensor();

        // Manages passenger and requests
        bool addPassenger(Passenger* passenger);
        bool deletePassenger(QString id);
        void newElevatorRequest(int targetFloor);
        void elevatorButtons(int targetFloor);

        // Manages the state of the elevators
        void stop();
        void idle();
        void open();
        void close();
        void keepOpen();
        void keepClosed();

        //Emergency Testing
        void helpButton();
        void fireAlarm();
        bool buildingSafetyService();
        void emergencyCall();
        bool checkObstacle();
        bool weightSensor();
        void powerOutage();
        void interrupt();
        void powerOff();


        //Getters and Setters
        int getElevatorNumber() const ;
        void setTargetFloor(int targetFloor);
        int getTargetFloor(){ return targetFloor; }
        QString getState() const ;
        int getWeight() const ;
        void setState(QString state);
        QString getDirection() const;
        void addObstacle() { this->obstacle = true; }
        void removeObstacle() { this->obstacle = false; }
        bool isButtonSelected(int number) const ;

    public slots:
        //Responsible for moving elevators with delay.
        void readyToMove();
        void move();
        void moveToFirstFloor();

    private:
        //Private Attributes
        const int openInterval;
        QString state;
        QString direction;
        bool obstacle;
        int targetFloor;
        const int elevatorNumber;
        const int weightLimit;
        Floor* floor;
        ECS* ecs;
        Door door;
        QVector<shared_ptr<ElevatorRequest>> elevatorRequests;
        QVector<Passenger*> passengers;

        QTimer *moveTimer;
        QTimer *emergencyTimer;
        QTimer *enterTimer;
        QTimer *doorOpenTimer;
        QTimer *helpTimer;

        //Private Functions
        void display(QString message) const ;
        void audio(QString message) const ;

        //Private functions to move passengers in and our of the elevator
        void passengerMovement(); //helper to create a delay on passenger movement
        void lastRequest(); //helper for passengerMovement


};

#endif
