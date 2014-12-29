#ifndef TEAM_H
#define TEAM_H

#include "playerpiece.h"


class Team
{
public:
    Team(QColor color, int numPieces, int travelDirection);
    ~Team();

    QList<PlayerPiece*> playerPieces; //turn this into a private member
    void calculateNextMove(QList<PlayerPiece *> opponents, QRectF sceneRectangle);
    void initializeTeam(QList<int> xcoords, QList<int> travelVectorIndices);
    bool getVictoryStatus();
    void setPlayerPiecesStartX(QList<int> xcoords);
    void setPlayerPieceSecondaryGoal(int secGoal);
    void checkCollisions(Team *opposingTeam);

private:

    int numPlayerPieces;
    int direction; //direction of travel for team
    int target; //y coord of destination
    bool won;

};

#endif // TEAM_H
