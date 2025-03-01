#ifndef PLAYERITEM_H
#define PLAYERITEM_H

#include "wallitem.h"
#include "healthbaritem.h"
#include "bulletitem.h"
#include "healthitem.h"
#include "settings.h"
#include "respawnoverlayitem.h"
#include "playercolor.h"

#include <QKeyEvent>
#include <QMouseEvent>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QDebug>
#include <QGraphicsItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsScene>
#include <QObject>
#include <QString>
#include <QPointF>
/*!
 * \brief PlayerItem class controls player creation, movement, shooting, death, respawn and map interactions
 */
class PlayerItem : public QObject, public QGraphicsEllipseItem
{
    Q_OBJECT

public:
    /*!
     * \brief PlayerItem constructor
     * \param takes the players color as a parameter
     */
    PlayerItem(PlayerColor color);
    /*!
     * \brief Funciton to get the players color
     * \return returns the variable _color which stores the players color
     */
    inline PlayerColor color() const
    {
        return _color;
    }
    /*!
     * \brief Function to set the players color
     * \param Color to set players color to
     */
    void setColor(PlayerColor value);

    inline BulletItem const* const* bullets() const
    {
        return _bullets.data();
    }

    /*!
     * \brief Sets the player starting health
     */
    int maxHealth = PLAYER_MAX_HEALTH;
    int _health = PLAYER_MAX_HEALTH;
    /*!
     * \brief Handles keyPressEvents for the playeritem
     */
    void keyPressEvent(QKeyEvent * event) override;
    /*!
     * \brief Handles keyReleaseEvents for the playeritem
     */
    void keyReleaseEvent(QKeyEvent * event) override;
    /*!
     * \brief Variable that holds the respawn point for the player
     */
    QPointF respawnPoint;
    /*!
     * \brief Brush to paint player images
     */
    QBrush playerBrush;
    /*!
     * \brief Players healthbar
     */
    HealthbarItem* myHealthbar;
    /*!
     * \brief Shoots a bullet from the player's current location
     * to the specified attack location.
     * \param the scene position at which to aim and shoot
     */
    void shoot(QPointF attackDestination);
    void shoot(QPointF source, qreal angle, bool fromNetwork = false);
    /*!
     * \brief Function to reduce the health of the player
     */
    void reduceHealth();
    /*!
     * \brief Function to increase the health of the player
     */
    void gainHealth();
    /*!
     * \brief Variable that says if the player is dead or not
     */
    bool isDead = false;
    /*!
     * \brief Function to determine if the player has the crown
     * \return returns variable _hascrown which is a boolean of if the player has the crown or not
     */
    inline bool hasCrown() const
    {
        return _hasCrown;
    }
    /*!
     * \brief Sets the value of whether or not the player has the crown
     * \param True or false to does the player have the crown
     */
    void setHasCrown(bool value);
    /*!
     * \brief Gets the health of the player
     * \return returns variable _health which is players health
     */
    inline int health() const
    {
        return _health;
    }
    /*!
     * \brief Function to set the health value of the player
     * \param new health value for player
     */
    void setHealth(int value);

    void reset();

signals:
    void shotBullet(PlayerColor color, QPointF source, qreal angle);
    void hasCrownChanged(bool newValue);

public slots:
    /*!
     * \brief Function for when player respawns
     */
    void respawn();

protected:
    void advance(int phase) override;

private:
    /*!
     * \brief Variable that stores player color
     */
    PlayerColor _color;
    /*!
     * \brief Variable that stores width of player ellipse
     */
    qreal width = 25;
    /*!
     * \brief Variable that stores height of player ellipse
     */
    qreal height = 25;

    QVector<BulletItem*> _bullets;

    // speed/acceleration variables
    double velocityX = 0.0,
           velocityY = 0.0,
           acceleration = PLAYER_ACCELERATION,
           max_speed = PLAYER_MAX_VELOCITY;
    /*!
     * \brief Variable for if the player has the crown. On player creation initialized to false
     */
    bool _hasCrown = false;
    /*!
     * \brief Variable to control if the player is allowed to move. Set to false when dead
     */
    bool allowedToMove = true;
    /*!
     * \brief Variable to control is the player is allowed to shoot. Set to false when dead
     */
    bool allowedToShoot = true;
    // bool values for key press
    bool leftKeyPressed=false,
         rightKeyPressed=false,
         upKeyPressed=false,
         downKeyPressed=false;
    // bool values for key releases
    bool leftKeyReleased=true,
         rightKeyReleased=true,
         upKeyReleased=true,
         downKeyReleased=true;
    /*!
     * \brief Function to determine if the player is colliding with a wall
     * \return true or false
     */
    bool collidesWithWall();
    /*!
     * \brief Function to determine if the player is colliding with another player
     * \return true or false
     */
    bool collidesWithPlayer();
    /*!
     * \brief Function to determine if the player is colliding with a health item
     * \return true or false
     */
    bool collidesWithHealth();
    /*!
     * \brief Variable to store previous position for help in collision detection
     */
    QPointF previousPos;
    /*!
     * \brief Timer for death countdown
     */
    QTimer deathTimer;
    /*!
     * \brief Function for when player dies
     */
    void die();
};

#endif // PLAYERITEM_H
