#include "crownitem.h"

#include <QPainter>


CrownItem::CrownItem(QGraphicsItem * parent) : QGraphicsRectItem(parent)
{
    //creating the graphics object and setting it into the scene.
    QBrush q;
    q.setTextureImage(QImage(":/images/startingcrown.png"));
    this->setBrush(q);
    qreal x = 550;
    qreal y = 300;
    setRect(x, y, width, height);
}

void CrownItem::advance(int phase)
{
    if(phase == 0)
    {
        return;
    }
    if(this->collidesWithPlayer())
    {
        //since the crown will only be touched once,
        //we have no use for the object afterwards
        this->~CrownItem();
    }
}

bool CrownItem::collidesWithPlayer()
{
    QList<QGraphicsItem*> collidingItems = this->collidingItems();

    foreach (QGraphicsItem * item, collidingItems)
    {
        if (PlayerItem* player = dynamic_cast<PlayerItem*>(item))
        {
            //if there is a player inside of the collision vector
            //we set the crown to the player that touched it.
            player->setHasCrown(true);
            return true;
        }
    }
    return false;
}
