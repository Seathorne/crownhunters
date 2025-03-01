#ifndef BULLETITEM_H
#define BULLETITEM_H

#include "wallitem.h"
#include "settings.h"

class PlayerItem;

#include <QGraphicsItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsScene>
#include <QPainter>
#include <QObject>
#include <QPixmap>
#include <QTimer>
#include <qmath.h> // qSin, qCos, qTan
#include <QDebug>

/*!
 * \brief Bullet item class that handles collisions with walls and players,
 *        as well as moves the bullet across the screen when a player shoots.
 */
class BulletItem: public QObject, public QGraphicsEllipseItem
{
    Q_OBJECT
public:
    /*!
     * \brief Constructs a bullet item.
     */
    BulletItem(PlayerItem* PlayerShooter, QGraphicsItem* parent=nullptr);

    /*!
     * \brief Deletes a bullet item.
     */
    ~BulletItem();

public slots:
    /*!
     * \brief Public slot function used to move the bullet.
     */
    void move();

signals:
    void collided(PlayerItem const* player);

private:
    PlayerItem* shooter;
    /*!
     * \brief Width of the bullet item.
     */
    qreal width = 13;

    /*!
     * \brief Height of the bullet item.
     */
    qreal height = 4;

    /*!
     * \brief Function to check if a bullet collides with a wall.
     */
    bool collidesWithWall();

    /*!
     * \brief Function to check if a bullet collides with a player.
     */
    bool collidesWithPlayer();

};

#endif // BULLETITEM_H
