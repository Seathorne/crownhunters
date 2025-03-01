#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDataStream>
#include <QVector2D>
#include <QtMath>

/*!
 * \brief The port number on which hosts should listen and clients should try to connect.
 */
const int PORT_NUMBER = 7400;

/*!
 * \brief The Qt compression algorithm to use when transferring messages between host and clients.
 */
const QDataStream::Version COMPRESSION_VERSION = QDataStream::Qt_4_1;

/*!
 * \brief The number of milliseconds to wait for a message to be received before considering
 * the connection to be timed out.
 */
const int NETWORK_TIMEOUT = 15 * 1000;

const int NETWORK_UPDATE_RATE = 1 * 50;

const int DEFAULT_MAX_PLAYERS = 8;

const int DEFAULT_GAME_LENGTH = 3;

/*!
 * \brief The amount of health players begin with upon starting a game and upon each respawn.
 */
const int PLAYER_MAX_HEALTH = 10;

/*!
 * \brief The amount of health players regenerate per advance frame.
 */
const int PLAYER_REGENERATE_HEALTH = 1;

/*!
 * \brief The amount of damage 1 bullet does to a player.
 */
const int BULLET_DAMAGE = PLAYER_MAX_HEALTH / 10;

/*!
 * \brief The acceleration magnitude applied to players as they move.
 * Lower values result in slipperier controls.
 */
const double PLAYER_ACCELERATION = 0.5;

/*!
 * \brief The acceleration magnitude applied to players as they stop moving.
 * Lower values result in slipperier floors.
 */
const double PLAYER_DECELERATION = 0.5;

/*!
 * \brief The maximum velocity magnitude of players.
 */
const double PLAYER_MAX_VELOCITY = 4.0;

/*!
 * \brief The cooldown time (in milliseconds) between a player dying and respawning.
 */
const qint64 PLAYER_RESPAWN_TIME = 5 * 1000;

/*!
 * \brief The width of the map.
 */
const qreal MAP_WIDTH = 1200;

/*!
 * \brief The height of the map.
 */
const qreal MAP_HEIGHT = 600;

#endif // SETTINGS_H
