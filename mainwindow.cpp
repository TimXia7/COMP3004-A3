#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QString>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //UI, Signals, Slots, elc.
    connect(ui->selectP1, &QRadioButton::clicked, this, &MainWindow::passengerSelected);
    connect(ui->selectP2, &QRadioButton::clicked, this, &MainWindow::passengerSelected);
    connect(ui->selectP3, &QRadioButton::clicked, this, &MainWindow::passengerSelected);

    connect(ui->eFloor1, &QPushButton::pressed, this, &MainWindow::elevatorButton1);
    connect(ui->eFloor2, &QPushButton::pressed, this, &MainWindow::elevatorButton2);
    connect(ui->eFloor3, &QPushButton::pressed, this, &MainWindow::elevatorButton3);
    connect(ui->eFloor4, &QPushButton::pressed, this, &MainWindow::elevatorButton4);
    connect(ui->eFloor5, &QPushButton::pressed, this, &MainWindow::elevatorButton5);
    connect(ui->eFloor6, &QPushButton::pressed, this, &MainWindow::elevatorButton6);
    connect(ui->eFloor7, &QPushButton::pressed, this, &MainWindow::elevatorButton7);

    connect(ui->fButtonUp, &QPushButton::pressed, this, &MainWindow::floorButtonUp);
    connect(ui->fButtonDown, &QPushButton::pressed, this, &MainWindow::floorButtonDown);

    connect(ui->forceOpen, &QPushButton::pressed, this, &MainWindow::forceOpen);
    connect(ui->forceClose, &QPushButton::pressed, this, &MainWindow::forceClose);

    connect(ui->fireAlarm, &QPushButton::pressed, this, &MainWindow::fireAlarm);
    connect(ui->powerOutage, &QPushButton::pressed, this, &MainWindow::powerOutage);
    connect(ui->setObstacle, &QPushButton::pressed, this, &MainWindow::setObstacle);
    connect(ui->helpButton, &QPushButton::pressed, this, &MainWindow::helpButton);

    //Run the simulation!
    emergency = false;
    runSimulation();
}


MainWindow::~MainWindow()
{
    delete ui;
    delete simulationTimer;
    delete shutDownTimer;
}


void MainWindow::runSimulation(){

    // Make the ECS, floors, and elevators.
    // You could easily add more floors and elevators, but for this project, the GUI is hard coded to work with
    // the required amount. The actual back end can handle more though.
    // An example is that the front end recognizes specific passenger names.
    ecs = make_shared<ECS>();

    ecs->addFloor(1);
    ecs->addFloor(2);
    ecs->addFloor(3);
    ecs->addFloor(4);
    ecs->addFloor(5);
    ecs->addFloor(6);
    ecs->addFloor(7);

    ecs->addElevator(1,1);
    ecs->addElevator(2,2);
    ecs->addElevator(3,3);

    ecs->addPassenger("P1", 60, 1);
    ecs->addPassenger("P2", 60, 2);
    ecs->addPassenger("P3", 60, 3);

    //Run the simulation polling timer!
    simulationTimer = new QTimer(this);
    connect(simulationTimer, &QTimer::timeout, this, &MainWindow::updateGUI);
    simulationTimer->setInterval(25);
    simulationTimer->start();

}

// Updates the GUI
void MainWindow::updateGUI(){

    //Elevator movement:
    updateElevatorLocation(1, ui->elevator1);
    updateElevatorLocation(2, ui->elevator2);
    updateElevatorLocation(3, ui->elevator3);

    //Passenger Buttons: changes depending on what passengers are selected, unless an emergency is in effect.
    if (!(emergency)){
        passengerSelected();
    }
}

// General function to update text
void MainWindow::updateTextBrowser(QTextBrowser *textBrowser, QString text){
    if (textBrowser){
        QFont font = textBrowser->currentFont();
        textBrowser->setText(text);
        textBrowser->setCurrentFont(font);
    }
}

// Exactly what the name says it does
void MainWindow::updateElevatorLocation(int elevatorNumber, QWidget* widget){
    if (widget){
        int floor = ecs->getElevator(elevatorNumber)->sensor();

        int newY = 390 - (40 * floor);

        if (!(emergency)){
            QTextBrowser *textBrowser;
            if (ecs->getElevator(elevatorNumber)->getElevatorNumber() == 1){
                textBrowser = ui->e1Display;
            } else if (ecs->getElevator(elevatorNumber)->getElevatorNumber() == 2){
                textBrowser = ui->e2Display;
            } else {
                textBrowser = ui->e3Display;
            }

            updateTextBrowser(textBrowser, QString::number(floor));
        }

        widget->move(widget->x(), newY);
    }
}



//Action Events:

void MainWindow::disableAllButtons(){
    QList<QPushButton*> allButtons = findChildren<QPushButton*>();
    for (int i = 0; i < allButtons.size(); ++i){
        allButtons[i]->setEnabled(false);
    }
}

void MainWindow::enableAllButtons(){
    QList<QPushButton*> allButtons = findChildren<QPushButton*>();
    for (int i = 0; i < allButtons.size(); ++i){
        allButtons[i]->setEnabled(true);
    }
}

// If the passenger is in an elevator, enable those buttons
void MainWindow::enableElevatorButtons(){
    this->enableAllButtons();

    ui->fButtonDown->setEnabled(false);
    ui->fButtonUp->setEnabled(false);
}

// If the passenger is in a floor, enable those buttons
void MainWindow::enableFloorButtons(){
    this->enableAllButtons();

    ui->eFloor1->setEnabled(false);
    ui->eFloor2->setEnabled(false);
    ui->eFloor3->setEnabled(false);
    ui->eFloor4->setEnabled(false);
    ui->eFloor5->setEnabled(false);
    ui->eFloor6->setEnabled(false);
    ui->eFloor7->setEnabled(false);

    ui->helpButton->setEnabled(false);
    ui->forceClose->setEnabled(false);
    ui->forceOpen->setEnabled(false);

}

// What passenger is selected in the GUI?
Passenger* MainWindow::determinePassenger(){
    if (ui->selectP1->isChecked()){
        return (ecs->getPassenger("P1")).get();
    }
    if (ui->selectP2->isChecked()){
        return (ecs->getPassenger("P2")).get();
    }
    if (ui->selectP3->isChecked()){
        return (ecs->getPassenger("P3")).get();
    }
    return nullptr;
}

// What elevator is selected in the GUI?
Elevator* MainWindow::determineElevator(){
    if (ui->selectE1->isChecked()){
        return (ecs->getElevator(1)).get();
    }
    if (ui->selectE2->isChecked()){
        return (ecs->getElevator(2)).get();
    }
    if (ui->selectE3->isChecked()){
        return (ecs->getElevator(3)).get();
    }
    return nullptr;
}

// When a passenger is selected, update the GUI for its state.
// Looks bloated, but its because it needs to be individual for the GUI
void MainWindow::passengerSelected(){
    Passenger* modelPassenger = this->determinePassenger();
    this->updateLocationDisplay(modelPassenger);

    if (modelPassenger == nullptr){
        this->enableAllButtons();
    } else if (modelPassenger->getElevator() == nullptr){
        this->enableFloorButtons();

        //Illuminated Floor Button Code: (illuminated states)
        if (modelPassenger->getFloor()->getDownState()){
            illuminateButton(ui->fButtonDown);
        } else {
            deIlluminateButton(ui->fButtonDown);
        }
        if (modelPassenger->getFloor()->getUpState()){
            illuminateButton(ui->fButtonUp);
        } else {
            deIlluminateButton(ui->fButtonUp);
        }
    } else if (modelPassenger->getElevator() != nullptr){
        if (modelPassenger->getElevator()->getState() == "emergency"){
            this->disableAllButtons();
        } else {
            this->enableElevatorButtons();
            //Illuminated Elevator Button Code: (illuminated states)
            if (modelPassenger->getElevator()->isButtonSelected(1) || modelPassenger->getElevator()->sensor() == 1){
                illuminateButton(ui->eFloor1);
            } else {
                deIlluminateButton(ui->eFloor1);
            }
            if (modelPassenger->getElevator()->isButtonSelected(2) || modelPassenger->getElevator()->sensor() == 2){
                illuminateButton(ui->eFloor2);
            } else {
                deIlluminateButton(ui->eFloor2);
            }
            if (modelPassenger->getElevator()->isButtonSelected(3) || modelPassenger->getElevator()->sensor() == 3){
                illuminateButton(ui->eFloor3);
            } else {
                deIlluminateButton(ui->eFloor3);
            }
            if (modelPassenger->getElevator()->isButtonSelected(4) || modelPassenger->getElevator()->sensor() == 4){
                illuminateButton(ui->eFloor4);
            } else {
                deIlluminateButton(ui->eFloor4);
            }
            if (modelPassenger->getElevator()->isButtonSelected(5) || modelPassenger->getElevator()->sensor() == 5){
                illuminateButton(ui->eFloor5);
            } else {
                deIlluminateButton(ui->eFloor5);
            }
            if (modelPassenger->getElevator()->isButtonSelected(6) || modelPassenger->getElevator()->sensor() == 6){
                illuminateButton(ui->eFloor6);
            } else {
                deIlluminateButton(ui->eFloor6);
            }
            if (modelPassenger->getElevator()->isButtonSelected(7) || modelPassenger->getElevator()->sensor() == 7){
                illuminateButton(ui->eFloor7);
            } else {
                deIlluminateButton(ui->eFloor7);
            }
        }
    }
}

// Updates the location display for the current passenger
void MainWindow::updateLocationDisplay(Passenger* passenger){
    if (passenger){
        if (passenger->getElevator() == nullptr){
            QString qStr = "F" + QString::number(passenger->getFloor()->getFloorNumber());
            ui->locationDisplay->setText(qStr);
        } else if (passenger->getElevator() != nullptr){
            QString qStr = "E" + QString::number(passenger->getElevator()->getElevatorNumber());
            ui->locationDisplay->setText(qStr);
        } else {
            ui->locationDisplay->setText("");
        }
    } else {
        ui->locationDisplay->setText("");
    }
    ui->locationDisplay->setStyleSheet("background-color: black; color: Green; font-size: 18pt;");
}

// Note for the 2 functions below, I know how to change colours of buttons, but Qt sometimes has trouble changing colours of
// widgets in quick succession, resulting in some colours not changing consistently. Apparently this is a Qt bug, that or
// I don't know how to do it.
void MainWindow::illuminateButton(QPushButton* button){
    if (button){
        //button->setStyleSheet("background-color: yellow;  font-size: 20pt;");
        button->setEnabled(false);
    }
}

void MainWindow::deIlluminateButton(QPushButton* button){
    if (button){
        //button->setStyleSheet("background-color: white;  font-size: 20pt;");
        button->setEnabled(true);
    }
}


void MainWindow::floorButtonUp(){
    Passenger* passenger = determinePassenger();
    if (passenger){
        passenger->operateFloorButtons("up");
    }
}

void MainWindow::floorButtonDown(){
    Passenger* passenger = determinePassenger();
    if (passenger){
        passenger->operateFloorButtons("down");
    }
}

void MainWindow::forceOpen(){
    Passenger* passenger = determinePassenger();
    if (passenger){
        passenger->keepOpen();
    }

}

void MainWindow::forceClose(){
    Passenger* passenger = determinePassenger();
    if (passenger){
        passenger->keepClosed();
    }
}



void MainWindow::operateElevatorButtons(int number){
    Passenger* passenger = determinePassenger();
    if (passenger){
        passenger->operateElevatorButtons(number);
    }
}

void MainWindow::elevatorButton1(){ operateElevatorButtons(1); }
void MainWindow::elevatorButton2(){ operateElevatorButtons(2); }
void MainWindow::elevatorButton3(){ operateElevatorButtons(3); }
void MainWindow::elevatorButton4(){ operateElevatorButtons(4); }
void MainWindow::elevatorButton5(){ operateElevatorButtons(5); }
void MainWindow::elevatorButton6(){ operateElevatorButtons(6); }
void MainWindow::elevatorButton7(){ operateElevatorButtons(7); }


//Emergency Action Events:
// These should be pretty self explanatory
void MainWindow::helpButton(){
    if (this->determinePassenger()){
        Elevator* elevator = determinePassenger()->getElevator();
        if (elevator){
            this->disableAllButtons();
            elevator->helpButton();
        }
    }
}

void MainWindow::fireAlarm(){
    emergency = true;

    QString qStr = "Fire Alarm, Remain Clam.";
    updateTextBrowser(ui->e1Display, qStr);
    updateTextBrowser(ui->e2Display, qStr);
    updateTextBrowser(ui->e3Display, qStr);

    ecs->broadcastFire();

    this->disableAllButtons();

    this->startShutDownTimer();
}

void MainWindow::powerOutage(){
    emergency = true;

    QString qStr = "Power Outage, Remain Clam.";
    updateTextBrowser(ui->e1Display, qStr);
    updateTextBrowser(ui->e2Display, qStr);
    updateTextBrowser(ui->e3Display, qStr);

    ecs->broadcastPowerout();

    this->disableAllButtons();

    this->startShutDownTimer();
}

void MainWindow::setObstacle(){
    Elevator* elevator = this->determineElevator();
    if (elevator){
        elevator->addObstacle();
        qInfo() << "Added an obstacle.";
    }
}

void MainWindow::startShutDownTimer(){
    shutDownTimer = new QTimer(this);
    connect(shutDownTimer, &QTimer::timeout, this, &MainWindow::checkShutDown);
    shutDownTimer->setInterval(3000);
    shutDownTimer->start();
}

void MainWindow::checkShutDown(){
    qInfo("System shut off imminent.");

    if (ecs->getElevator(1)->sensor() == 1 && ecs->getElevator(2)->sensor() == 1 && ecs->getElevator(3)->sensor() == 1){
        ecs->powerOff();

        // NOTE: The system should close automatically, its not a crash!
        this->close();
    }
}

void MainWindow::closeEvent(QCloseEvent *event) {
    simulationTimer->stop();
    shutDownTimer->stop();
    ecs->interruptElevators();
    QMainWindow::closeEvent(event);
}









