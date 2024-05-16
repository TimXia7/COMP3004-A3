#include "ElevatorController.h"
#include "Elevator.h"
#include "Floor.h"
#include "Passenger.h"

ElevatorController::ElevatorController(){
}


void ElevatorController::runSimulation(){

    shared_ptr<ECS> ecs = make_shared<ECS>();


    ecs->addFloor(1);
    ecs->addFloor(2);
    ecs->addFloor(3);
    ecs->addFloor(4);
    ecs->addFloor(5);
    ecs->addFloor(6);
    ecs->addFloor(7);

    ecs->addElevator(1);
    ecs->addElevator(2);
    ecs->addElevator(3);

    ecs->printElevatorStates();

    ecs->getElevator(1).move("up");
    ecs->getElevator(1).move("up");
    ecs->getElevator(1).move("up");
    ecs->getElevator(1).move("up");
    ecs->getElevator(1).move("up");
    ecs->getElevator(1).move("up");
    ecs->getElevator(1).move("up");

    ecs->getElevator(2).move("up");
    ecs->getElevator(2).move("up");
    ecs->getElevator(2).move("up");
    ecs->getElevator(2).move("up");

    ecs->getElevator(3).move("up");
    ecs->getElevator(3).move("down");
    ecs->getElevator(3).move("up");

    ecs->printElevatorStates();
}
