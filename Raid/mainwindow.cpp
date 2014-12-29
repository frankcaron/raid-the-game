#include "mainwindow.h"
#include "playerpiece.h"

#include "QGraphicsView"
#include "QVBoxLayout"
#include "QHBoxLayout"
#include "QPushButton"
#include "QDebug"
#include <QTime>
#include <QDir>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    //setup some things that are meta to teams, i.e., to do with a match
    int halfDim = PlayerPiece::blockSize*halfBoardNumBlocks;
    numPlayerPieces = 3; //cannot be zero.
    team1 = new Team(QColor(255,0,0,255),numPlayerPieces, 1);
    team2 = new Team(QColor(0,255,0,255),numPlayerPieces, -1);
    turn = 0;
    matchesPlayed = 0;

    //misc things to setup
    timer = new QTimer(this); //this has to do with automating the matches
    //qsrand(QTime::currentTime().msec()); //new rand seed


    //setup the playfield scene
    scene = new QGraphicsScene();
    scene->setSceneRect(-1*halfDim-15,-1*halfDim-15,(halfDim+15)*2,(halfDim+15)*2);
    addTeamToScene(team1);
    addTeamToScene(team2);

    //Pure UI Widget setup
    QWidget *main = new QWidget();
    QVBoxLayout *myverticallayout = new QVBoxLayout();
    QHBoxLayout *toprow = new QHBoxLayout();
    QHBoxLayout *secondRow = new QHBoxLayout();
    matchLabel = new QLabel("Match:");
    matchLabel->setGeometry(0,0,50,50);
    endGameText = scene->addText("");
    endGameText->setPos(-100.0,0.0);
    endGameText->setFont(QFont("Arial",36,10));
    QPushButton *buttonRestart = new QPushButton("restart");
    QPushButton *buttonStep = new QPushButton("step");
    QPushButton *buttonStartTimer = new QPushButton("startTimer");
    QPushButton *buttonEndTimer   = new QPushButton("endTimer");
    connect(buttonRestart, SIGNAL(clicked()), this, SLOT(onRestartButtonPress()));
    connect(buttonStep, SIGNAL(clicked()), this, SLOT(onStepButtonPress()));
    connect(buttonStartTimer, SIGNAL(clicked()), this, SLOT(onStartTimerButtonPress()));
    connect(buttonEndTimer, SIGNAL(clicked()), this, SLOT(onEndTimerButtonPress()));
    toprow->addWidget(buttonRestart);
    toprow->addWidget(buttonStep);
    secondRow->addWidget(buttonStartTimer);
    secondRow->addWidget(buttonEndTimer);
    secondRow->addWidget(matchLabel);
    QGraphicsView *view = new QGraphicsView(scene);
    view->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    view->setGeometry(0,0,1,1);
    myverticallayout->addLayout(toprow);
    myverticallayout->addLayout(secondRow);
    myverticallayout->addWidget(view);
    main->setLayout(myverticallayout);
    setCentralWidget(main);

    //logfile setup
    QTime time = time.currentTime();
    QDate date = date.currentDate();
    filename = "raid_logfile_"+QString::number(date.year())+makePrettyDateTime(date.month())+makePrettyDateTime(date.day());
    filename = filename + "_" + makePrettyDateTime(time.hour())+makePrettyDateTime(time.minute())+makePrettyDateTime(time.second());

    onRestartButtonPress(); //this will initialize everything to do with an actual match
}

MainWindow::~MainWindow()
{

}

void MainWindow::onRestartButtonPress() {
    if(matchesPlayed!=0) logStuff();
    matchesPlayed++;
    qDebug() <<"######## NEW MATCH #######";
    updateMatchLabel();
    team1->initializeTeam(getRandomXCoords(),getRandomTravelVectorIndices());
    team2->initializeTeam(getRandomXCoords(),getRandomTravelVectorIndices());

    endGameText->setPlainText("");
    turn = 0;
}

void MainWindow::onStepButtonPress() {
    turn = turn + 1;
    if (!team1->getVictoryStatus() && !team2->getVictoryStatus()) { //TODO: shouldn't have to check if match is over twice like this,
        if (turn % 2 == 1) {
            qDebug() <<"@@@ RED's TURN @@@";
            team1->calculateNextMove(team2->playerPieces,scene->sceneRect());
            team1->checkCollisions(team2);
        }
        else {
            qDebug() <<"@@@ GREEN's TURN @@@";
            team2->calculateNextMove(team1->playerPieces,scene->sceneRect());
            team2->checkCollisions(team1);
        }
    }
    if (team1->getVictoryStatus() || team2->getVictoryStatus()) { //and if match now over, do stuff
        QString temp = "";
        if (team1->getVictoryStatus()) {
            temp = "RED WINS";
            endGameText->setDefaultTextColor(QColor(255,0,0,255));
        }
        else {
            temp = "GREEN WINS";
            endGameText->setDefaultTextColor(QColor(0,255,0,255));
        }
        endGameText->setPlainText(temp);
    }
}




void MainWindow::addTeamToScene(Team *team) {
    QList<PlayerPiece*>::iterator i;
    for (i=team->playerPieces.begin(); i!=team->playerPieces.end(); i++) {
        scene->addItem(*i);
    }
}

QList<int> MainWindow::getRandomXCoords() {
    //This function gets random unique x values, one for each player piece regardless of status of the player piece

    QList<int> xcoords;
    int numUniqueRands = 0;
    for (int i = 0; i < numPlayerPieces; i++) {
        while(numUniqueRands != numPlayerPieces) {
            int randNum = qrand();
            int xcoord = (double)randNum / (double)RAND_MAX * (halfBoardNumBlocks+1)*2 - (halfBoardNumBlocks+1); //+1 because casting double as int floors
            xcoord = xcoord*PlayerPiece::blockSize; //put in blocksize steps
            if (xcoords.indexOf(xcoord)!=false && xcoords.indexOf(xcoord)<0) {
                xcoords.append(xcoord);
                numUniqueRands ++;
            }
        }
    }
    return xcoords;
}

QList<int> MainWindow::getRandomTravelVectorIndices() {
    //this function returns the index of valid travel vectors, 0-5 supported right now
    QList<int> travelVectorIndices;
    for (int i = 0; i < numPlayerPieces; i++) {
        int randNum = qrand();
        int index = (double)randNum / (double)RAND_MAX * 6;
        travelVectorIndices.append(index);
    }
    return travelVectorIndices; //put in blocksize steps
}

void MainWindow::onStartTimerButtonPress() {
    connect(timer, SIGNAL(timeout()), this, SLOT(timerUpdate()));
    timer->start(200);
}

void MainWindow::onEndTimerButtonPress() {
    timer->stop();
}

void MainWindow::timerUpdate() {
    if (team1->getVictoryStatus()==false && team2->getVictoryStatus()==false) {
        onStepButtonPress();
    }
    else onRestartButtonPress();
}

void MainWindow::logStuff() {
    QString summaryFileName = filename+"_summary.txt";
    qDebug() <<"logging summary to"<<filename;

    summaryFileName = "logfile.txt";    //delete when done debugging

    QFile file(summaryFileName);
    if (!file.open(QIODevice::Append))
        return;

    QTextStream out(&file);
    if (this->matchesPlayed==1) { //print header row
        out<<"matchNo,winner,turns,redPieces,greenPieces,redDead,greedDead\n";
    }

    QString winner;
    if (team1->getVictoryStatus()==true) winner = "red";
    else winner = "green";

    QString team1Pieces= ":";
    QString team1DeadPieces =":";
    for (int i =0; i < team1->playerPieces.count(); i++) {
        team1Pieces = team1Pieces+team1->playerPieces.at(i)->getMarker()+":";
        if (team1->playerPieces.at(i)->getStatus()==0)
            team1DeadPieces = team1DeadPieces + team1->playerPieces.at(i)->getMarker()+":";
    }

    QString team2Pieces = ":";
    QString team2DeadPieces = ":";
    for (int i =1; i < team2->playerPieces.count(); i++) {
        team2Pieces = team2Pieces+team2->playerPieces.at(i)->getMarker()+":";
        if (team2->playerPieces.at(i)->getStatus()==0)
            team2DeadPieces = team2DeadPieces + team2->playerPieces.at(i)->getMarker()+":";
    }

    QString outString = QString::number(matchesPlayed)+","+winner+","+QString::number(turn)+","+team1Pieces+","+team2Pieces+","+team1DeadPieces+","+team2DeadPieces+"\n";
    out << outString;

    file.close();
}

QString MainWindow::makePrettyDateTime(int arg) {
    if (arg>=10)
        return QString::number(arg);
    return "0"+QString::number(arg);
}

void MainWindow::updateMatchLabel() {
    matchLabel->setText("Match: "+QString::number(matchesPlayed));
}
