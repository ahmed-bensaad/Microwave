#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QState>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
  {
      init();
      ui->setupUi(this);
      QStateMachine * stateM = new QStateMachine();
      initStateMachine(stateM);
      stateM->start();
  }

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::init()
{
    hours = 0;
    minutes = 0;
    ratio = 0;
    mode = WAVE;
    reset();
}

void MainWindow::reset()
{
    power = 100;
    duration = 60;
}

void MainWindow::modeSet()
{
    switch(mode)
    {
    case WAVE: mode = GRILL; break;
    case GRILL: mode = WAVE_GRILL; break;
    case WAVE_GRILL: mode = WAVE; break;
    default: mode = WAVE;
    }
}


void MainWindow::initStateMachine(QStateMachine * stateM){

    QState * stopGroup = new QState();
    QState * idle = new QState(stopGroup);
    QState * clockHour = new QState(stopGroup);
    QState * clockMin = new QState(stopGroup);
    QState * powerSet = new QState(stopGroup);
    QState * modeSet = new QState(stopGroup);
    QState * timeSet = new QState(stopGroup);
    QState * defrost = new QState(stopGroup);
    QState * cooking = new QState(stopGroup);

    idle->addTransition(ui->clock,       SIGNAL(clicked()),  clockHour);
    idle->addTransition(ui->power,       SIGNAL(clicked()),  powerSet);
    idle->addTransition(ui->mode,        SIGNAL(clicked()),  modeSet);
    idle->addTransition(ui->defrost,     SIGNAL(clicked()),  defrost);
    idle->addTransition(ui->start,       SIGNAL(clicked()),  cooking);
    clockHour->addTransition(ui->clock,  SIGNAL(clicked()),  clockMin);
    clockMin->addTransition(ui->clock,   SIGNAL(clicked()),  idle);
    powerSet->addTransition(ui->power,   SIGNAL(clicked()),  timeSet);
    modeSet->addTransition(ui->mode,     SIGNAL(clicked()),  timeSet);
    timeSet->addTransition(ui->start,    SIGNAL(clicked()),  cooking);
    defrost->addTransition(ui->start,    SIGNAL(clicked()),  cooking);
    cooking->addTransition(&timer,       SIGNAL(timeout()),  idle);
    stopGroup->addTransition(ui->stop,   SIGNAL(clicked()),  idle);

    stateM->addState(stopGroup);
    stopGroup->setInitialState(idle);
    stateM->setInitialState(stopGroup);

    connect(idle,       SIGNAL(entered()), this, SLOT(idle()));
    connect(clockHour,  SIGNAL(entered()), this, SLOT(clockHour()));
    connect(clockMin,   SIGNAL(entered()), this, SLOT(clockMin()));
    connect(powerSet,   SIGNAL(entered()), this, SLOT(powerSet()));
    connect(modeSet,    SIGNAL(entered()), this, SLOT(modeSet()));
    connect(timeSet,    SIGNAL(entered()), this, SLOT(timeSet()));
    connect(defrost,    SIGNAL(entered()), this, SLOT(defrost()));
    connect(cooking,    SIGNAL(entered()), this, SLOT(cookingStart()));
    connect(clockHour,  SIGNAL(exited()),  this, SLOT(clockHourExit()));
    connect(clockMin,   SIGNAL(exited()),  this, SLOT(clockMinExit()));
    connect(powerSet,   SIGNAL(exited()),  this, SLOT(powerSetExit()));
    connect(modeSet,    SIGNAL(exited()),  this, SLOT(modeSetExit()));
    connect(timeSet,    SIGNAL(exited()),  this, SLOT(timeSetExit()));
    connect(defrost,    SIGNAL(exited()),  this, SLOT(defrostExit()));
    connect(&timer,     SIGNAL(timeout()), this, SLOT(cookingExit()));

    connect(ui->dial,SIGNAL(valueChanged(int)),SLOT(onDialChanged(int)));

}

void MainWindow::clockHourExit(){
    hours = ui->dial->value();
}

void MainWindow::clockMinExit(){
    minutes = ui->dial->value();
}

void MainWindow::powerSetExit(){
    power = ui->dial->value();
}

void MainWindow::modeSetExit(){
    modeSet();
}

void MainWindow::timeSetExit(){
    duration = ui->dial->value();
}

void MainWindow::defrostExit(){
    duration = ratio * ui->dial->value();
}

void MainWindow::onDialChanged(int value){
    ui->screen->setText(QString::number(value));
}

void MainWindow::idle(){
    ui->screen->setText(QString::number(hours) + ":" + QString::number(minutes));
}

void MainWindow::clockHour(){
    int newVal = 24 * ui->dial->value() / ui->dial->maximum();
    ui->dial->setRange(0, 24);
    ui->dial->setValue(newVal);
    ui->screen->setText(QString::number(ui->dial->value()));
}

void MainWindow::clockMin(){
    int newVal = 60 * ui->dial->value() / ui->dial->maximum();
    ui->dial->setRange(0, 60);
    ui->dial->setValue(newVal);
    ui->screen->setText(QString::number(ui->dial->value()));
}

void MainWindow::powerSet(){
    int newVal = 100 * ui->dial->value() / ui->dial->maximum();
    ui->dial->setRange(0, 100);
    ui->dial->setValue(newVal);
    ui->screen->setText(QString::number(ui->dial->value()));
}

void MainWindow::timeSet(){
    int newVal = 360 * ui->dial->value() / ui->dial->maximum();
    ui->dial->setRange(0, 360);
    ui->dial->setValue(newVal);
    ui->screen->setText(QString::number(ui->dial->value()));
}

void MainWindow::defrost(){
    int newVal = 100 * ui->dial->value() / ui->dial->maximum();
    ui->dial->setRange(0, 100);
    ui->dial->setValue(newVal);
    ui->screen->setText(QString::number(ratio * ui->dial->value()));
}
void MainWindow::cookingStart(){
    timer.start(duration);
    while(timer.isActive()){
        duration-=1;
        ui->screen->setText((QString::number(duration)));
    }

}
void MainWindow::cookingExit(){
    timer.stop();
}
