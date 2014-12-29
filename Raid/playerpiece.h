#ifndef LAYOUTITEM_H
#define LAYOUTITEM_H

#include <QGraphicsLayoutItem>
#include <QGraphicsItem>
#include <QPoint>

class PlayerPiece : public QGraphicsLayoutItem, public QGraphicsItem
{
public:
    PlayerPiece(QColor col, int index, QGraphicsItem *parent = 0);
    ~PlayerPiece();
    // Inherited from QGraphicsLayoutItem
    void setGeometry(const QRectF &geom) Q_DECL_OVERRIDE;
    QSizeF sizeHint(Qt::SizeHint which, const QSizeF &constraint = QSizeF()) const Q_DECL_OVERRIDE;

    // Inherited from QGraphicsItem
    QRectF boundingRect() const Q_DECL_OVERRIDE;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0) Q_DECL_OVERRIDE;

    QPoint getTravelVector(QList<PlayerPiece*> teammates, QList<PlayerPiece *> opponents, QRectF sceneRectangle);
    int getStatus();
    QString getMarker();
    void setStatus(int status);
    void setTravelVectors(int travelVectorIndex);
    void setPrimaryGoal(int direction);
    static const int blockSize = 30;
    void initialize(int startingXCoord, int startingYCoord, int direction, int index);      //for each piece sets status, possible vectors, and starting pos given starting line
    QList<QPoint> sortTravelVectors(QList<QPoint> possibleTravelVectors, QList<PlayerPiece*> opponents);   //sorts given priorities: ability, direction of travel

private:
    QColor color;
    QString marker;

    int directionOfTravel;    //defined as 1 or -1 for now, i.e., a direction of travel
    int abilitySearchRange;
    int ability;        //0 = attack; 1 = heal. TODO: these should really be enums
    int status;         //0 = dead; 1 = alive.  TODO: these should really be enums
    QList<QPoint> travelVectors;
    int displayIndex;     //purely for UI purposes, not used for any logic
};

#endif // LAYOUTITEM_H
