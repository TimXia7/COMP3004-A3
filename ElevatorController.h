#ifndef ELEVATORCONTROLLER_H
#define ELEVATORCONTROLLER_H

#include <QString>
#include <QDebug>
#include <memory>

#include "ECS.h"

class Elevator;
class Floor;
class Passenger;

using namespace std;

class ElevatorController {
    public:
        ElevatorController();

        void runSimulation();
};

#endif
