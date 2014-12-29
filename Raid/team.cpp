#include "team.h"
#include "mainwindow.h"

#include <QDebug>


Team::Team(QColor color, int numPieces, int travelDirection)
{
    qDebug()<<"In Team::Team()";
    for (int i = 0; i<numPieces; i++) {
        playerPieces.append(new PlayerPiece(color,i));
    }
    numPlayerPieces = numPieces;
    direction = travelDirection;

}

void Team::initializeTeam(QList<int> xcoords, QList<int> travelVectorIndices) {
    won = false;
    target = PlayerPiece::blockSize*MainWindow::halfBoardNumBlocks*direction;

    //set starting y position, which is always the far side of the board
    int startingYCoord = target*(-1);    //starting pos is always flipped across the x-axis from target

    //set up each piece
    QList<PlayerPiece*>::iterator i;
    int j=0;
    for (i = playerPieces.begin(); i != playerPieces.end(); i++) {
        (*i)->initialize(xcoords.at(j),startingYCoord,direction, travelVectorIndices.at(j)); //initialize pieces
        j++;
    }
}

Team::~Team()
{

}

void Team::calculateNextMove(QList<PlayerPiece*> opponents,QRectF sceneRectangle) {
    //this function serves only to move the pieces

    if (!won) {
        QList<PlayerPiece*>::iterator i;
        for (i = playerPieces.begin(); i != playerPieces.end(); ++i) {
            if ((*i)->getStatus()==1) { //this piece is alive, so it can be moved
                QPoint travelVector = (*i)->getTravelVector(playerPieces, opponents,sceneRectangle);
                (*i)->setPos(travelVector.x(),travelVector.y());
            }
        }
    }
    QList<PlayerPiece*>::iterator i;
    for (i = playerPieces.begin(); i != playerPieces.end(); ++i) {
        if ((*i)->y()==target) won = 1;
    }
}

void Team::checkCollisions(Team *opposingTeam) {
    //this function changes statuses of this team or of the opposing team (i.e., revive or kill)
    QList<PlayerPiece*>::iterator thisTeamPiece;

    //check kills first by checking each of this team's pieces against opposing team's pieces
    for (thisTeamPiece = playerPieces.begin(); thisTeamPiece != playerPieces.end(); ++thisTeamPiece) {
        if ((*thisTeamPiece)->getStatus()) { //this team piece can do something
            QList<PlayerPiece*>::iterator oppTeamPiece;
            for (oppTeamPiece = opposingTeam->playerPieces.begin(); oppTeamPiece != opposingTeam->playerPieces.end(); ++oppTeamPiece) {
                if ((*thisTeamPiece)->x()==(*oppTeamPiece)->x() && (*thisTeamPiece)->y()==(*oppTeamPiece)->y()) {
                    qDebug() <<"Opp team piece killed";
                    (*oppTeamPiece)->setStatus(0);
                }
            }
        }
    }
}

bool Team::getVictoryStatus() {
    return won;
}


