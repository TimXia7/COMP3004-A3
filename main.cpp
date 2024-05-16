#include "mainwindow.h"

//TESTING TESTT :
#include "Floor.h"
#include "Elevator.h"
#include "ECS.h"
#include "Passenger.h"


#include <QApplication>
#include <memory>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
