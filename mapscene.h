#ifndef MAPSCENE_H
#define MAPSCENE_H

#include "playeritem.h"
#include "wallitem.h"
#include "crownitem.h"
#include "healthitem.h"
#include "configdialog.h"
#include "hostconfigdialog.h"
#include "ui_configdialog.h"
#include "settings.h"
#include "respawnoverlayitem.h"
#include "gamestartoverlayitem.h"

#include <QTimer>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QMouseEvent>
#include <QObject>
#include <QAbstractSocket>


/*!
 * \brief MapScene is a QGraphicsScene created to hold the walls, players
 * and items in the map.
 */
class MapScene : public QGraphicsScene
{
    Q_OBJECT

public:
    /*!
     * \brief Constructs the MapScene object.
     */
    explicit MapScene(QObject* parent = nullptr);

    /*!
     * \brief Deletes the MapScene.
     */
    ~MapScene();

    /*!
     * \brief Handles mouse presses within the MapScene.
     */
    void mousePressEvent(QGraphicsSceneMouseEvent* event);

    /*!
     * \brief Handles key presses within the MapScene.
     */
    void keyPressEvent(QKeyEvent* event);

    /*!
     * \brief Handles key releases within the MapScene
     */
    void keyReleaseEvent(QKeyEvent* event);

    /*!
     * \brief Removes the specified healthkit item from the scene.
     * \param item the healthkit item remove
     */
    void removeHealthKit(HealthItem* item);

    inline QMap<PlayerColor, PlayerItem*> players() const
    {
        // TODO return immutable/cloned collection
        return _players;
    }

    inline PlayerItem* player(PlayerColor color) const
    {
        return _players[color];
    }

    inline PlayerItem* myPlayer() const
    {
        return _players[_myPlayerColor];
    }

    PlayerItem* winner;

    /*!
     * \brief Function called to initialize the MapScene at the beginning of a new game
     */

    void mapSetup();

    void runGameStartOverlay();

public slots:
    /*!
     * \brief Function that randomly spawns health kits.
     */
    virtual void healthSpawner();

    /*!
     * \brief Checks if player is dead, and handles respawn overlay.
     */
    virtual void checkIfPlayerDead();

    void setMyColor(PlayerColor value);
    void onPositionUpdated(PlayerColor color, QPointF position);
    void onBulletUpdated(PlayerColor color, QPointF source, qreal angle);
    void onHealthUpdated(PlayerColor color, int health, bool hasCrown);

private:
    /*!
     * \brief Creates and adds players to the MapScene.
     */
    void createPlayers(int count);

    PlayerColor _myPlayerColor = PlayerColor::Cyan;

    QMap<PlayerColor, PlayerItem*> _players;

    QPointF const spawns[DEFAULT_MAX_PLAYERS] =
    {
        QPointF(500,400),
        QPointF(500,200),
        QPointF(650,350),
        QPointF(600,200),
        QPointF(600,400),
        QPointF(450,250),
        QPointF(450,350),
        QPointF(650,250),
    };

    QTimer *healthTimer;

    QTimer *advanceTimer;

    QTimer* respawnTimer;

    /*!
     * \brief Test crown for demonstration purposes.
     */
    CrownItem* crown;

    /*!
     * \brief Test healthkit for demonstration purposes.
     */
    HealthItem* healthKit;

    /*!
     * \brief Test healthkit for demonstration purposes.
     */
    HealthItem* healthKit2;

    /*!
     * \brief Test healthkit for demonstration purposes.
     */
    HealthItem* healthKit3;

    /*!
     * \brief Test respawn overlay.
     */
    RespawnOverlayItem* respawnOverlay;

    GameStartOverlayItem* gameStartOverlay;

};

#endif // MAPSCENE_H
