#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStateMachine>
#include <QTimer>


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

private:
    Ui::MainWindow *ui;
    int power;
    int duration;
    int hours;
    int minutes;
    int ratio;
    QTimer timer;
    Mode mode;

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





};

#endif // MAINWINDOW_H
