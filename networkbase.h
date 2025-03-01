#ifndef NETWORKBASE_H
#define NETWORKBASE_H

#include "playercolor.h"
#include "settings.h"

#include <QAbstractSocket>
#include <QHostAddress>
#include <QJsonDocument>
#include <QJsonObject>
#include <QObject>

/*!
 * \brief NetworkBase is an abstract class that provides common functionality for hosts and clients
 * to transmit and receive messages via TCP.
 * \author Scott
 */
class NetworkBase : public QObject
{
    Q_OBJECT

public:
    /*!
     * \brief The MessageType enum specifies the type of a message.
     */
    enum MessageType
    {
        JOIN_REQUEST,
        JOIN_RESPONSE,
        POSITION_UPDATE,
        BULLET_SHOT,
        HEALTH_UPDATE,
        GAME_START,
        GAME_END,
        PLAYER_JOINED,
        PLAYER_LEFT,
        CHAT_MESSAGE,
    };

    /*!
     * \brief The MessageParam enum specifies the name of a
     * specific parameter that is present in a message.
     */
    enum MessageParam
    {
        MESSAGE_TYPE,
        USERNAME,
        COLOR,
        POSITION_X,
        POSITION_Y,
        BULLET_ANGLE,
        HEALTH,
        CROWN,
        GAME_TIME,
        JOIN_SUCCEEDED,
        JOIN_ERROR,
        CHAT_BODY,
    };

    /*!
     * \brief The JoinError enum is a set of bitflags that indicate which
     * error is the reason a client was rejected from joining a game.
     */
    enum JoinError
    {
        NO_ERROR = 0,
        INVALID_USERNAME = 1,
        INVALID_COLOR = 1 << 1,
        GAME_FULL = 1 << 2,
        USERNAME_TAKEN = 1 << 3,
        COLOR_TAKEN = 1 << 4,
    };

    /*!
     * \brief Returns a string representation of the provided MessageType value.
     * \param messageType the message type to return as a string
     * \return a string representation of the message type
     */
    static QString toString(MessageType messageType);

    /*!
     * \brief Returns a string representation of the provided MessageParam value.
     * \param messageParam the parameter value to return as a string
     * \return a string representation of the parameter name
     */
    static QString toString(MessageParam messageParam);

    /*!
     * \brief Returns a string representation of the provided JoinError value.
     * \param joinError the join error reason to return as a string
     * \return a string representation of the error
     */
    static QString toString(JoinError joinError);

signals:

    /*!
     * \brief This signal is emitted when a position update message has been received.
     * \param color the color of the player whose position this message concerns
     * \param position the new position of the player
     */
    void positionUpdated(PlayerColor color, QPointF position);

    /*!
     * \brief This signal is emitted when a message has been received indicating that
     * a player has just shot a bullet.
     * \param color the color of the player who shot a bullet
     * \param source the source location of the bullet
     * \param angle the angle at which the bullet was fired (0 degrees = horizontal, facing right)
     */
    void bulletUpdated(PlayerColor color, QPointF source, qreal angle);

    /*!
     * \brief This signal is emitted when a health update message has been received.
     * \param color the color of the player whose health this message concerns
     * \param health the new health value of the player
     * \param hasCrown whether or not the player has the crown
     */
    void healthUpdated(PlayerColor color, int health, bool hasCrown);

    /*!
     * \brief This signal is emitted when the host has started the game.
     * \param gameTime the number of minutes specified as the length of the game
     */
    void gameStarted(int gameTime);

    /*!
     * \brief This signal is emitted by the host when the game has ended.
     * \param winner the color of the player who won the game
     * \param username the username of the player who won the game
     */
    void gameEnded(PlayerColor winner, QString const& username);

    /*!
     * \brief This signal is emitted when a new player has joined the game.
     * \param color the color of the new player
     * \param username the username of the new player
     */
    void playerJoined(PlayerColor color, QString const& username);

    /*!
     * \brief This signal is emitted when a player has left the game.
     * \param color the color of the player who left the game
     * \param username the username of the player who left the game
     */
    void playerLeft(PlayerColor color, QString const& username);

    /*!
     * \brief This signal is emitted when a chat message has been received.
     * \param color the color of the player sending the chat message
     * \param username the username of the player sending the chat message
     * \param body the contents of the chat message
     */
    void receivedChatMessage(PlayerColor color, QString const& username, QString const& body);

protected:
    /*!
     * \brief Creates a new instance of the NetworkBase class.
     * \param parent the parent who will handle disposal of this object
     */
    explicit NetworkBase(QObject* parent = nullptr);

    /*!
     * \brief Sends the provided message via the specified socket.
     * \param socket the socket via which to send the message
     * \param message the message to send
     */
    static void sendMessage(QAbstractSocket* socket, QJsonObject const& message);

    /*!
     * \brief Constructs a message for a client to request entry to a host's game.
     * \param color color that the client is requesting to use
     * \param username the username that the client is requesting to use
     * \return a message for a client to request entry to a host's game
     */
    static QJsonObject const joinRequest(PlayerColor color, QString const& username);

    /*!
     * \brief Constructs a message for a host to accept or reject a client's game entry request.
     * \param succeeded whether the client's request is accepted or rejected
     * \param color if the request was accepted, the color that was approved for the client
     * \param username if the request was accepted, the username that was approved for the client
     * \param error if the request was rejected, the reason for the rejection
     * \return a message for a host to accept or reject a client's game entry request
     */
    static QJsonObject const joinResponse(bool succeeded, PlayerColor color, QString const& username, JoinError error = JoinError::NO_ERROR);

    /*!
     * \brief Constructs a message that indicates a player's position has been updated.
     * \param color the color of the player whose position this message concerns
     * \param position the new position of the player
     * \return a message that indicates a player's position has been updated
     */
    static QJsonObject const positionMessage(PlayerColor color, QPointF position);

    /*!
     * \brief Constructs a message that indicates a player has shot a bullet.
     * \param color the color of the player who shot the bullet
     * \param source the source position at which the bullet was shot
     * \param angle the angle at which the bullet was shot
     * \return a message that indicates a player has shot a bullet
     */
    static QJsonObject const bulletMessage(PlayerColor color, QPointF source, qreal angle);

    /*!
     * \brief Constructs a message that indicates a player's health has changed.
     * \param color the color of the player whose health this message concerns
     * \param health the new health value of the player
     * \param hasCrown whether or not the player currently has the crown
     * \return a message that indicates a player's health has changed
     */
    static QJsonObject const healthMessage(PlayerColor color, int health, bool hasCrown);

    /*!
     * \brief Constructs a message that indicates the game is starting.
     * \param gameTime the length of the game, in minutes
     * \return a message that indicates the game is starting
     */
    static QJsonObject const gameStartMessage(int gameTime);

    /*!
     * \brief Constructs a message that indicates the game is ending.
     * \param winner the color of the player who won the game
     * \param username the username of the player who won the game
     * \return a message that indicates the game is ending
     */
    static QJsonObject const gameEndMessage(PlayerColor winner, QString const& username);

    /*!
     * \brief Constructs a message that indicates a player has joined the game.
     * \param color the color of the player who joined the game
     * \param username the username of the player who joined the game
     * \return a message that indicates a player has joined the game
     */
    static QJsonObject const playerJoinedMessage(PlayerColor color, QString const& username);

    /*!
     * \brief Constructs a message that indicates a player has left the game.
     * \param color the color of the player who left the game
     * \param username the username of the player has left the game
     * \return a message that indicates a player has left the game
     */
    static QJsonObject const playerLeftMessage(PlayerColor color, QString const& username);

    /*!
     * \brief Constructs a message that indicates a player is sending a chat message.
     * \param color the color of the player who is sending the chat message
     * \param username the username of the player who is sending the chat message
     * \param body the contents of the chat message
     * \return a message that indicates a player is sending a chat message
     */
    static QJsonObject const chatMessage(PlayerColor color, QString const& username, QString const& body);

    /*!
     * \brief A pure virtual function defining the behavior of a client or host upon receiving
     * a message from the specified socket. The defined behavior may include refreshing a timer
     * to indicate the player is still connected.
     * \param socket the socket on which some message was received
     */
    virtual void receivedFrom(QAbstractSocket* socket) = 0;

    /*!
     * \brief A host may define the behavior to be taken upon successfully parsing a join request message.
     * \param socket the socket on which the join request message was received
     * \param color the requested color of the client sending the message
     * \param username the requested username of the client sending the message
     */
    virtual void onParsedJoinRequest(QAbstractSocket* socket, PlayerColor color, QString const& username);

    /*!
     * \brief A client may define the behavior to be taken upon successfully parsing a join response message.
     * \param succeeded whether the client's join request has been accepted or rejected
     * \param color if the join request was accepted, the approved color for the client
     * \param username if the join request was accepted, the approved username for the client
     * \param error if the join request was rejected, the reason for the rejection
     */
    virtual void onParsedJoinResponse(bool succeeded, PlayerColor color, QString const& username, JoinError error = JoinError::NO_ERROR);

    /*!
     * \brief A host or client may define the behavior to be taken upon successfully parsing a position update message.
     * \param color the color of the player whose position the message concerns
     * \param position the new position of the player
     */
    virtual void onParsedPositionMessage(PlayerColor color, QPointF position);

    /*!
     * \brief A host or client may define the behavior to be taken upon successfully parsing a bullet shot message.
     * \param player the color of the player who shot a bullet
     * \param source the source position at which the bullet was shot
     * \param angle the angle at which the bullet was shot
     */
    virtual void onParsedBulletMessage(PlayerColor player, QPointF source, qreal angle);

    /*!
     * \brief A host or client may define the behavior to be taken upon successfully parsing a health update message.
     * \param color the color of the player whose health the message concerns
     * \param health the new health value of the player
     * \param hasCrown whether or not the player has the crown
     */
    virtual void onParsedHealthMessage(PlayerColor color, int health, bool hasCrown);

    /*!
     * \brief A client may define the behavior to be taken upon successfully parsing a game start message.
     * \param gameTime the length of the game, in minutes
     */
    virtual void onParsedGameStartMessage(int gameTime);

    /*!
     * \brief A client may define the behavior to be taken upon successfully parsing a game end message.
     * \param winner the color of the player who won the game
     * \param username the username of the player who won the game
     */
    virtual void onParsedGameEndMessage(PlayerColor winner, QString const& username);

    /*!
     * \brief A client may define the behavior to be taken upon successfully parsing a player joined message.
     * \param color the color of the player who joined the game
     * \param username the username of the player who joined the game
     */
    virtual void onParsedPlayerJoinedMessage(PlayerColor color, QString const& username);

    /*!
     * \brief A client may define the behavior to be taken upon successfully parsing a player left message.
     * \param color the color of the player who left the game
     * \param username the username of the player who left the game
     */
    virtual void onParsedPlayerLeftMessage(PlayerColor color, QString const& username);

    /*!
     * \brief A client or host may define the behavior to be taken upon successfully parsing a chat message.
     * \param color the color of the player who sent the chat message
     * \param username the username of the player who sent the chat message
     * \param body the contents of the chat message
     */
    virtual void onParsedChatMessage(PlayerColor color, QString const& username, QString const& body);

    /*!
     * \brief Tries to parse a message type from a string.
     * \param string the string to try to parse into a message type
     * \param messageType the resulting value of the parsing, if it was successful
     * \return whether or not the string was able to be parsed successfully
     */
    static bool tryParse(QString const& string, MessageType& messageType);

    /*!
     * \brief Tries to parse a message parameter name from a string.
     * \param string the string to try to parse into a message parameter name
     * \param messageParam the resulting value of the parsing, if it was successful
     * \return whether or not the string was able to be parsed successfully
     */
    static bool tryParse(QString const& string, MessageParam& messageParam);

    /*!
     * \brief Tries to parse a join error value from a string.
     * \param string the string to try to parse into a join error value
     * \param joinError the resulting value of the parsing, if it was successful
     * \return whether or not the string was abled to be parsed successfully
     */
    static bool tryParse(QString const& string, JoinError& joinError);

protected slots:
    /*!
     * \brief This slot should be called when a socket has pending data
     * that is ready to be read. The data will be attempted to be parsed
     * into any of the possible message types.
     * \param socket
     */
    void onReadyRead(QAbstractSocket* socket);

private:
    /*!
     * \brief Tries to parse a received data object into any of the possible message
     * types. This method determines the type of the message and passes it to
     * one of the other parse methods for further parsing, if applicable.
     * \param socket the socket on which the data was received
     * \param message the data object that was received
     * \return whether or not the data object was able to be parsed into a message
     */
    bool parseMessage(QAbstractSocket* socket, QJsonObject const& message);

    /*!
     * \brief Tries to extract the relevant information from a join request message.
     * If the message is able to be parsed, the corresponding signal is emitted.
     * \param socket the socket on which the data was received
     * \param message the data object that was received
     * \return whether or not the data object was able to be parsed into a message
     */
    bool parseJoinRequest(QAbstractSocket* socket, QJsonObject const& message);

    /*!
     * \brief Tries to extract the relevant information from a join response message.
     * If the message is able to be parsed, the corresponding signal is emitted.
     * \param message the data object that was received
     * \return whether or not the data object was able to be parsed into a message
     */
    bool parseJoinResponse(QJsonObject const& message);

    /*!
     * \brief Tries to extract the relevant information from a position update message.
     * If the message is able to be parsed, the corresponding signal is emitted.
     * \param message the data object that was received
     * \return whether or not the data object was able to be parsed into a message
     */
    bool parsePositionMessage(QJsonObject const& message);

    /*!
     * \brief Tries to extract the relevant information from a bullet shot message.
     * If the message is able to be parsed, the corresponding signal is emitted.
     * \param message the data object that was received
     * \return whether or not the data object was able to be parsed into a message
     */
    bool parseBulletMessage(QJsonObject const& message);

    /*!
     * \brief Tries to extract the relevant information from a health update message.
     * If the message is able to be parsed, the corresponding signal is emitted.
     * \param message the data object that was received
     * \return whether or not the data object was able to be parsed into a message
     */
    bool parseHealthMessage(QJsonObject const& message);

    /*!
     * \brief Tries to extract the relevant information from a game start message.
     * If the message is able to be parsed, the corresponding signal is emitted.
     * \param message the data object that was received
     * \return whether or not the data object was able to be parsed into a message
     */
    bool parseGameStartMessage(QJsonObject const& message);

    /*!
     * \brief Tries to extract the relevant information from a game end message.
     * If the message is able to be parsed, the corresponding signal is emitted.
     * \param message the data object that was received
     * \return whether or not the data object was able to be parsed into a message
     */
    bool parseGameEndMessage(QJsonObject const& message);

    /*!
     * \brief Tries to extract the relevant information from a player joined message.
     * If the message is able to be parsed, the corresponding signal is emitted.
     * \param message the data object that was received
     * \return whether or not the data object was able to be parsed into a message
     */
    bool parsePlayerJoinedMessage(QJsonObject const& message);

    /*!
     * \brief Tries to extract the relevant information from a player left message.
     * If the message is able to be parsed, the corresponding signal is emitted.
     * \param message the data object that was received
     * \return whether or not the data object was able to be parsed into a message
     */
    bool parsePlayerLeftMessage(QJsonObject const& message);

    /*!
     * \brief Tries to extract the relevant information from a received chat message.
     * If the message is able to be parsed, the corresponding signal is emitted.
     * \param message the data object that was received
     * \return whether or not the data object was able to be parsed into a message
     */
    bool parseChatMessage(QJsonObject const& message);
};

#endif // NETWORKBASE_H
