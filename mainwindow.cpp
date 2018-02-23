#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QElapsedTimer>
#include <QTime>
#include <math.h>

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
    QTime * time= new QTime();
    *time=time->currentTime();
    hours=time->hour();
    minutes=time->minute();
    ratio = 1;
    mode = WAVE;
    reset();
}

void MainWindow::reset()
{
    power = 100;
    duration = 60;
    timer.stop();
    secondTimer.stop();

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


    idleState->addTransition(ui->clock,       SIGNAL(clicked()),  clockHourState);
    idleState->addTransition(ui->power,       SIGNAL(clicked()),  powerSetState);
    idleState->addTransition(ui->mode,        SIGNAL(clicked()),  modeSetState);
    idleState->addTransition(ui->defrost,     SIGNAL(clicked()),  defrostState);
    idleState->addTransition(ui->start,       SIGNAL(clicked()),  cookingState);
    clockHourState->addTransition(ui->clock,  SIGNAL(clicked()),  clockMinState);
    clockMinState->addTransition(ui->clock,   SIGNAL(clicked()),  idleState);
    powerSetState->addTransition(ui->power,   SIGNAL(clicked()),  timeSetState);
    modeSetState->addTransition(ui->mode,     SIGNAL(clicked()),  timeSetState);
    timeSetState->addTransition(ui->start,    SIGNAL(clicked()),  cookingState);
    defrostState->addTransition(ui->start,    SIGNAL(clicked()),  cookingState);
    cookingState->addTransition(&timer,       SIGNAL(timeout()),  idleState);
    stopGroup->addTransition(ui->stop,        SIGNAL(clicked()),  idleState);

    stateM->addState(stopGroup);
    stopGroup->setInitialState(idleState);
    stateM->setInitialState(stopGroup);

    connect(idleState,       SIGNAL(entered()), this, SLOT(idle()));
    connect(clockHourState,  SIGNAL(entered()), this, SLOT(clockHour()));
    connect(clockMinState,   SIGNAL(entered()), this, SLOT(clockMin()));
    connect(powerSetState,   SIGNAL(entered()), this, SLOT(powerSet()));
    connect(modeSetState,    SIGNAL(entered()), this, SLOT(modeSet()));
    connect(timeSetState,    SIGNAL(entered()), this, SLOT(timeSet()));
    connect(defrostState,    SIGNAL(entered()), this, SLOT(defrost()));
    connect(cookingState,    SIGNAL(entered()), this, SLOT(cookingStart()));
    connect(clockHourState,  SIGNAL(exited()),  this, SLOT(clockHourExit()));
    connect(clockMinState,   SIGNAL(exited()),  this, SLOT(clockMinExit()));
    connect(powerSetState,   SIGNAL(exited()),  this, SLOT(powerSetExit()));
    connect(modeSetState,    SIGNAL(exited()),  this, SLOT(modeSetExit()));
    connect(timeSetState,    SIGNAL(exited()),  this, SLOT(timeSetExit()));
    connect(defrostState,    SIGNAL(exited()),  this, SLOT(defrostExit()));
    connect(&timer,          SIGNAL(timeout()), this, SLOT(cookingExit()));
    connect(&secondTimer,    SIGNAL(timeout()), this, SLOT(currentTime()));


    connect(ui->dial,SIGNAL(valueChanged(int)),SLOT(onDialChanged(int)));
    connect(ui->start,SIGNAL(clicked()),this,SLOT(addTime()));

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
    reset();
    ui->dial->setDisabled(false);
    ui->screen->setText(QString::number(hours) + ":" + QString::number(minutes));
}

void MainWindow::clockHour(){
    int newVal = 23 * ui->dial->value() / ui->dial->maximum();
    ui->dial->setRange(0, 23);
    ui->dial->setValue(newVal);
    ui->screen->setText(QString::number(ui->dial->value()));
}

void MainWindow::clockMin(){
    int newVal = 59 * ui->dial->value() / ui->dial->maximum();
    ui->dial->setRange(0, 59);
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
    timer.start(duration*1000);
    timer.setInterval(duration*1000);
    elapsedTimer.restart();
    secondTimer.start(duration*1000);
    secondTimer.setInterval(1000);
    ui->dial->setDisabled(true);

}

void MainWindow::cookingExit(){
    reset();

}

void MainWindow::currentTime(){
    ui->screen->setText(QString::number(duration-elapsedTimer.elapsed()/1000));
}

void MainWindow::addTime(){
    if (cookingState->active()){
        duration=duration-elapsedTimer.elapsed()/1000 + 60;
        timer.stop();
        timer.start(duration*1000);
        elapsedTimer.restart();
    }
}
