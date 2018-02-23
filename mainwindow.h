#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStateMachine>
#include <QState>
#include <QTimer>
#include <QElapsedTimer>


enum Mode {WAVE, GRILL, WAVE_GRILL};

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    void initStateMachine(QStateMachine * stateM);
    void init();
    void reset();
    void changeMode();
    ~MainWindow();
    QState * stopGroup = new QState();
    QState * idleState = new QState(stopGroup);
    QState * clockHourState = new QState(stopGroup);
    QState * clockMinState = new QState(stopGroup);
    QState * powerSetState = new QState(stopGroup);
    QState * modeSetState = new QState(stopGroup);
    QState * timeSetState = new QState(stopGroup);
    QState * defrostState = new QState(stopGroup);
    QState * cookingState = new QState(stopGroup);


private:
    Ui::MainWindow *ui;
    int power;
    int duration;
    int hours;
    int minutes;
    int ratio;
    QTimer timer,secondTimer;
    Mode mode;
    QElapsedTimer elapsedTimer;


private slots:
    //enter states
    void idle();
    void clockMin();
    void clockHour();
    void timeSet();
    void powerSet();
    void modeSet();
    void cookingStart();
    void defrost();
    //change dial
    void onDialChanged(int value);
    //exit states
    void clockMinExit();
    void clockHourExit();
    void powerSetExit();
    void modeSetExit();
    void timeSetExit();
    void defrostExit();
    void cookingExit();

    void currentTime();
    void addTime();




};

#endif // MAINWINDOW_H
