#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "team.h"

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QTimer>
#include <QLabel>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    static const int halfBoardNumBlocks = 4;


public slots:
    void onStepButtonPress();
    void onRestartButtonPress();
    void onStartTimerButtonPress();
    void onEndTimerButtonPress();
    void timerUpdate();


private:
    QGraphicsScene *scene;
    Team *team1;
    Team *team2;
    QGraphicsTextItem *endGameText;
    int turn;
    int numPlayerPieces;
    int matchesPlayed;
    QTimer *timer;

    void addTeamToScene(Team *team);
    QList<int> getRandomXCoords();
    QList<int> getRandomTravelVectorIndices();
    void logStuff();
    QString makePrettyDateTime(int arg);
    QString filename;
    QLabel *matchLabel;
    void updateMatchLabel();
};

#endif // MAINWINDOW_H
