#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}


//PlayerPiece manages:
//  - shape and size of piece itself
//  - status (dead or alive) and alpha (the two are connected)
//  - allowable travelVectors (a list of points, depends on type of piece)
//  - abilities
//  - figuring out which travelVector to take on a given turn, given all teams in the field
//  - it's responsible for mapping player piece level coords to scene coordinates

//Team manages:
//  - container for playerPieces
//  - initializing logic for team given params
//  - team-level parameters (victory, centroid, team's target, number of team pieces)
//  - it's responsible for mapping any team level coordinates scene coordinates
//  - it implements logic to take in a "turn"

//MainWindow's job is to manage the high level logic
//  - calls initilize teams with a bunch of params, handles things at the "match" level
//  - draws scene, view, buttons
//  - maps buttons to initialize and step
//  - handles what can be done each turn:
//      - move
//      - check for collisions and act accordingly
//  - timer
//  - logging summary level things

//TODOS:
//  - TODO notes all throughout project
//  - logging details at the per-piece movement level
//  - try larger list of possivle movement vectors (should in theory work...)
//  - handle healing ability:
//      need to right this into the recommendation logic in PlayerPiece (right now it only tries to kill or to advance towards target)
//      if overlapped, need to change the status of the revived piece back to "alive"
//  - check what happens if we make the board larger or smaller
//  - check what happens if there are more or less than 3 pieces
//  - change order of drawing for the team that is moving (right now red is always drawn below green)
//  - what happens if a piece "jumps" over another piece?
