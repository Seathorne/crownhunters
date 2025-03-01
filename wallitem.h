#ifndef WALLITEM_H
#define WALLITEM_H

#include <QPainter>
#include <QPolygonF>
#include <QGraphicsItem>

/*!
 * \brief WallItem class will create wall items within mapscene
 */
class WallItem : public QObject, public QGraphicsPolygonItem
{
    Q_OBJECT
public:
    /*!
     * \brief Constructor that creates the wall for the scene
     * \param Array of points for the vertices, the number of vertices for the polygon,
     * a QPen and QBrush item to help draw the wall, and a pointer to the parent item
     */
    WallItem(QPointF points[], int numPoints, QPen pen, QBrush brush, QGraphicsItem* parent=nullptr);
    ~WallItem();
};

#endif // WALLITEM_H
