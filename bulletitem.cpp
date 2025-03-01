#include "bulletitem.h"
#include "playeritem.h"

BulletItem::BulletItem(PlayerItem* PlayerShooter, QGraphicsItem* parent):
    QGraphicsEllipseItem (parent)
{
    this->shooter = PlayerShooter;
    // set bullet relative to scene
    setRect(0,0,width, height);
    // set graphics
    QBrush q;
    q.setTextureImage(QImage(":/images/bullet.png"));
    this->setBrush(q);

    // connect a timer to move
    QTimer * move_timer = new QTimer();
    connect(move_timer, SIGNAL(timeout()), this, SLOT(move()));
    move_timer->start(50);
}

BulletItem::~BulletItem()
{
    delete this;
}

void BulletItem::move()
{
    int STEP_SIZE = 40; // how many pixels the bullet moves per timer tick
    double theta = rotation(); // angle in degrees

    double dy = STEP_SIZE * qSin(qDegreesToRadians(theta)); // set delta y in radians
    double dx = STEP_SIZE * qCos(qDegreesToRadians(theta)); // set delta x in radians

    setPos(x() + dx, y() + dy); // sets the position of the bullet according to dx and dy
    // if the bullet hits a wall, remove bullet from scene
    if (collidesWithWall())
    {
        scene()->removeItem(this);
    }
    // if the bullet hits a player, remove bullet from scene
    if (collidesWithPlayer())
    {
        scene()->removeItem(this);
    }
}

bool BulletItem::collidesWithWall()
{
    QList<QGraphicsItem*> collidingItems = this->collidingItems();

    foreach (QGraphicsItem* item, collidingItems)
    {
        if (WallItem * wall = dynamic_cast<WallItem*>(item))
        {
            emit collided(nullptr);
            return true;
        }
    }
    return false;
}

bool BulletItem::collidesWithPlayer()
{
    QList<QGraphicsItem*> collidingItems = this->collidingItems();

    foreach (QGraphicsItem* item, collidingItems)
    {
        if (PlayerItem * player = dynamic_cast<PlayerItem*>(item))
        {
            for (int i = 0; i < BULLET_DAMAGE; i++)
            {
                player->reduceHealth();
            }
            if(player->isDead && player->hasCrown())
            {
                player->setHasCrown(false);
                this->shooter->setHasCrown(true);
            }

            emit collided(player);
            return true;
        }
    }
    return false;
}
