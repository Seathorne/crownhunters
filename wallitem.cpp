#include "wallitem.h"
#include "playeritem.h"


WallItem::WallItem(QPointF points[], int numPoints, QPen pen, QBrush brush, QGraphicsItem* parent) :
    QGraphicsPolygonItem (parent)
{
    // create polygon that will be set as WallItem
    QPolygonF *wall = new QPolygonF;
    // create QPolygonF from QPointF's
    for (int i = 0; i < numPoints; i++){
        wall->append(points[i]);
    }
    // set the pen and brush to draw the wall with
    this->setPen(pen);
    this->setBrush(brush);
    // set the polygon as the QGraphicsItem/wall
    this->setPolygon(*wall);
}

WallItem::~WallItem()
{
    delete this;
}
