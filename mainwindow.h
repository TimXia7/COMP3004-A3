#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QTextBrowser>
#include <QPushButton>
#include <QObject>

#include "ECS.h"
#include "Elevator.h"
#include "Floor.h"
#include "Passenger.h"

// This is the view part of the MVC model, only it also runs the simualation to easily do this job.
// So it also fulfills parts of the controller in MVC.
// It is in charge of updating the GUI.


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    //Runs simulation and udpates GUI on a polling timer
    void runSimulation();
    void updateGUI();

    //Helpers to update
    void updateElevatorLocation(int eN, QWidget* w); //elevatorNumber and widget
    void updateTextBrowser(QTextBrowser *textBrowser, QString text);

    //action events:
    void passengerSelected();
    void updateLocationDisplay(Passenger* passenger);

    void disableAllButtons();
    void enableAllButtons();
    void enableFloorButtons();
    void enableElevatorButtons();

    void forceOpen();
    void forceClose();

    void floorButtonUp();
    void floorButtonDown();

    void operateElevatorButtons(int number);
    void elevatorButton1();
    void elevatorButton2();
    void elevatorButton3();
    void elevatorButton4();
    void elevatorButton5();
    void elevatorButton6();
    void elevatorButton7();

    // Helpers for other functions, names and codeshould be pretty self explanatory
    Passenger* determinePassenger();
    Elevator* determineElevator();
    // NOTE: I did not change colours of buttons, just made them unclickable to represent illumination
    void illuminateButton(QPushButton* button);
    void deIlluminateButton(QPushButton* button);

    //Emergency events:
    void helpButton();
    void fireAlarm();
    void powerOutage();
    void startShutDownTimer();
    void checkShutDown();
    void setObstacle();

    //override close event:
    void closeEvent(QCloseEvent *event);


private:
    Ui::MainWindow *ui;
    QTimer *simulationTimer;
    QTimer *shutDownTimer;
    // Holds ECS as it fulfills some controller function
    shared_ptr<ECS> ecs;

    bool emergency;
};
#endif // MAINWINDOW_H

