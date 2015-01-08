#include "playerpiece.h"

#include <QGradient>
#include <QGraphicsLinearLayout>
#include <QPainter>
#include <QDebug>
#include <QTableWidget>

PlayerPiece::PlayerPiece(QColor col, int index, QGraphicsItem *parent/* = 0*/)
    : QGraphicsLayoutItem(), QGraphicsItem(parent), color(col), displayIndex(index)
{
    //m_pix = new QPixmap(QLatin1String(":/images/block.png"));
    setGraphicsItem(this);
    status = 1;
    ability = 1;      //hard code this to attack for now. TODO: add healing
    abilitySearchRange = 2;

}

PlayerPiece::~PlayerPiece()
{
    //delete m_pix;
}

void PlayerPiece::paint(QPainter *painter,
    const QStyleOptionGraphicsItem *option, QWidget *widget /*= 0*/)
{
    Q_UNUSED(widget);
    Q_UNUSED(option);

    QString label = QString::number(this->displayIndex)+"\n"+marker;
    painter->setBrush(QBrush(color));
    painter->drawRoundedRect(-1*blockSize/2,-1*blockSize/2,blockSize,blockSize,2.0,2.0);
    painter->drawText(-1*blockSize/2,-1*blockSize/2,20,blockSize,Qt::AlignCenter,label);
}

QRectF PlayerPiece::boundingRect() const
{
    return QRectF(QPointF(-1*blockSize/2,-1*blockSize/2), geometry().size());
}

void PlayerPiece::setGeometry(const QRectF &geom)
{
    prepareGeometryChange();
    QGraphicsLayoutItem::setGeometry(geom);
    setPos(geom.topLeft());
}

QSizeF PlayerPiece::sizeHint(Qt::SizeHint which, const QSizeF &constraint) const
{
    switch (which) {
    case Qt::MinimumSize:
        return QSizeF(blockSize,blockSize);
    case Qt::PreferredSize:
        // Do not allow a size smaller than the pixmap with two frames around it.
        //return m_pix->size() + QSize(12, 12);
        return QSizeF(blockSize,blockSize);
    case Qt::MaximumSize:
        return QSizeF(blockSize,blockSize);
    default:
        break;
    }
    return constraint;
}

QPoint PlayerPiece::getTravelVector(QList<PlayerPiece*> teammates, QList<PlayerPiece*> opponents, QRectF sceneRectangle) {
    // Generally trying to achieve:
    // 1. If the next position calculated from (travel vector i) is not in the the scene, remove travel vector i from possibleTravelVectors
    // 2. if the next position calculated from (travel vector i) overlaps own team pieces, remove travel vector i from possibleTravelVectors
    // 3. If the next position calculated from (travel vector i) lands in a possible next position for any piece on the opponent team, remove travel vector i from possibleTravelVectors
    // TODO: 3. may jam the game so haven't tried it yet

    // Algorithm to achieve this:
    //  1. filter out possible unit travel vectors based on criteria:
    //  - cannot go into a place where the next move the opponent will kill you
    //  - cannot overlap own non-dead piece
    //  - cannot go off the board
    //  2. rank order the travel vectors remaining based on priorities:
    //  - move towards secondary goal
    //  - move towards target
    //  - TODO: move towards centroid (not sure about this one)

    qDebug()<<"PlayerPiece::getTravelVector for piece at"<<pos();
    QList<QPoint> possibleTravelVectors = travelVectors;

    for (int i = 0; i< travelVectors.size(); i++) {
        QPoint thisVector = travelVectors.at(i);
        thisVector = thisVector;
        QPointF nextPosition = this->pos() + thisVector*blockSize;

        //check if outside of board
        if (!sceneRectangle.contains(nextPosition)) {
            possibleTravelVectors.removeAt(possibleTravelVectors.indexOf(thisVector));
            qDebug() <<"   REMOVED:"<<thisVector<<"pos()="<<nextPosition << "marker="<<marker<<thisVector;
        }

        //check if overlapping own live pieces
        for (int j = 0; j < teammates.size(); j++) {
            if (this!=teammates.at(j)) {
                if (teammates.at(j)->pos().x() == nextPosition.x() && teammates.at(j)->pos().y() == nextPosition.y()) {
                    if (teammates.at(j)->getStatus()==1) { //if the teammate is alive
                        possibleTravelVectors.removeAt(possibleTravelVectors.indexOf(thisVector));
                        qDebug() <<"   REMOVED:"<<thisVector<<"pos()="<<nextPosition << "marker="<<marker<<thisVector;
                    }
                }
            }
        }
    }

    qDebug() << "   Piece" << QString::number(displayIndex)+"-"+marker<<"original vectors" << travelVectors;
    qDebug() << "   Piece" << QString::number(displayIndex)+"-"+marker<<"filtered vectors before sorting" << possibleTravelVectors;

    possibleTravelVectors = sortTravelVectors(possibleTravelVectors, opponents);

    QPoint returnVal = possibleTravelVectors.at(0) * blockSize;
    returnVal.setX(returnVal.x() + pos().x());
    returnVal.setY(returnVal.y() + pos().y());

    qDebug() << "   Piece" <<QString::number(displayIndex)+"-"+marker<<"filtered vectors after sorting" << possibleTravelVectors;

    return returnVal;

}

int PlayerPiece::getStatus() {
    return status;
}

void PlayerPiece::setStatus(int stat) {
    color.setAlpha(255);
    if (stat==0) color.setAlpha(50);
    status = stat;
}

void PlayerPiece::initialize(int startingXCoord, int startingYCoord, int direction, int index) {
    setStatus(1);
    setPrimaryGoal(direction);
    setPos(startingXCoord,startingYCoord);
    setTravelVectors(index);
}

void PlayerPiece::setPrimaryGoal(int direction) {
    directionOfTravel = direction;
}

QString PlayerPiece::getMarker() {
    return marker;
}

void PlayerPiece::setTravelVectors(int index) { //possible travel vectors are hard coded for now. May not want to random gen these
    travelVectors.clear();  //empty before doing this
    travelVectors.append(QPoint(0,0));  //not moving is always an option
    switch(index) {
        case 0: //move straight forward only, 1 space
            travelVectors.append(QPoint(0,1));
            marker = "Sf1";
            break;
        case 1: //move diagonal forward only, 1 space
            travelVectors.append(QPoint(1,1));
            travelVectors.append(QPoint(-1,1));
            marker = "Df1";
            break;
        case 2: //move diagonal any direction, 1 space
            travelVectors.append(QPoint(1,1));
            travelVectors.append(QPoint(-1,1));
            travelVectors.append(QPoint(1,-1));
            travelVectors.append(QPoint(-1,-1));
            marker = "Da1";
            break;
        case 3: //move straight forward only, 2 spaces
            travelVectors.append(QPoint(0,2));
            marker = "Sf2";
            break;
        case 4: //move diagonal forward only, 2 spaces
            travelVectors.append(QPoint(2,2));
            travelVectors.append(QPoint(-2,2));
            marker = "Df2";
            break;
        case 5: //move any direction, 1 space
            travelVectors.append(QPoint(0,1));
            travelVectors.append(QPoint(1,1));
            travelVectors.append(QPoint(0,1));
            travelVectors.append(QPoint(-1,1));
            travelVectors.append(QPoint(-1,0));
            travelVectors.append(QPoint(-1,-1));
            travelVectors.append(QPoint(0,-1));
            travelVectors.append(QPoint(1,-1));
            marker = "A1";
        default:
            break;
    }

    //flipping vertical (if required) happens here!
    for (int i = 0; i< travelVectors.size(); i++) {
        QPoint temp = travelVectors.at(i);
        temp.setY(temp.y()*directionOfTravel);
        travelVectors.replace(i,temp);
    }
}

QList<QPoint> PlayerPiece::sortTravelVectors(QList<QPoint> possibleTravelVectors, QList<PlayerPiece*> opponents) {
    // this is called only when someone wants to get a ranked list of travel vectors
    // this function takes a filtered list of possible travel vectors and sorts them in order of priority of objective:
    // 1) can act on ability
    // 2) sign of "y" in travel vector is the same as "travelDirection"
    // 3) TODO: use searchRange... right now pieces only try to kill if they can do it in the next move

    // General Algorithm:
    // Implement a table "sortVectorsTable" where each field is 0 or 1, and each column represents an objective
    // [possibleTraveVectorsIndex], [can act on ability], [can travel in directionOfTravel], [TODO: other objectives like defence]
    // sort sortVectorsTable in order of col(1) to col(n), where columns are in ascending order of priority
    // create return QList where each item is added from possibleTravelVectors in the order that they appear in sortVectorsTable

    int numEvalCriteria = 2;    //check only if 1) can act on ability or 2) if can travel in travelDirection. TODO: don't hardcode
    QTableWidgetItem *item;
    QList<QPoint> returnListOfPoints;


    QTableWidget sortVectorsTable(possibleTravelVectors.count(),numEvalCriteria+1); //num cols = index column + # of evaluation criteri
    for (int i =0; i<possibleTravelVectors.count(); i++) { //loop through each point
        QPoint possibleTravelCoord(possibleTravelVectors.at(i).x()*blockSize+pos().x(), possibleTravelVectors.at(i).y()*blockSize+pos().y());

        //set the first column to be the index in possibleTravelVector
        item = new QTableWidgetItem();
        item->setData(Qt::DisplayRole,i);   //default 0
        sortVectorsTable.setItem(i,0,item); //assuming column indexing starts at 0...

        //figure out column 1 = is the sign of the possibleTravelVector the SAME as this pieces "directionOfTravel"?
        //in the case of zero, set the field to 0, not 1
        item = new QTableWidgetItem();
        item->setData(Qt::DisplayRole,0);   //default 0
        if (possibleTravelVectors.at(i).y() * directionOfTravel > 0)
            item->setData(Qt::DisplayRole,1);
        sortVectorsTable.setItem(i,1,item); //assuming column indexing starts at 0... TODO: this really shouldn't be hardcoded.

        //figure out column 2 = can I kill any enemy pieces?
        //this logic searches until one is found, sets it to 1
        int j = 0;
        bool found = false;
        item = new QTableWidgetItem();
        item->setData(Qt::DisplayRole,0); //default 0
        while (j < opponents.count() && !found) {
            QPoint opponentCoord(opponents.at(j)->x(),opponents.at(j)->y());
            if (opponentCoord == possibleTravelCoord && opponents.at(j)->getStatus()==1) {
                item->setData(Qt::DisplayRole,1);
                found = true;
                qDebug()<<"   In PlayerPiece::sortTravelVectors: Found killing move";
            }
            j++;
        }
        sortVectorsTable.setItem(i,2,item);
    }

    sortVectorsTable.sortByColumn(1,Qt::DescendingOrder); //sort by moving forward
    sortVectorsTable.sortByColumn(2,Qt::DescendingOrder); //sort by killing intent

    for (int i =0; i<possibleTravelVectors.count(); i++) { //loop through each point
        int index = sortVectorsTable.item(i,0)->data(0).toInt();
        returnListOfPoints.append(possibleTravelVectors.at(index));
        //qDebug()<<"sortVectorsTable:"<< sortVectorsTable.item(i,0)->data(0).toInt() << sortVectorsTable.item(i,1)->data(0).toInt() <<sortVectorsTable.item(i,2)->data(0).toInt();
    }
    return returnListOfPoints;
}
