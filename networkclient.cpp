#include "networkclient.h"

NetworkClient::NetworkClient(QObject* parent)
    : NetworkBase(parent)
    , _socket(new QTcpSocket(this))
    , _timeoutTimer(new QTimer(this))
{
    // Set up network timeout timer
    _timeoutTimer->setSingleShot(true);
    _timeoutTimer->setInterval(NETWORK_TIMEOUT);

    // Connect network timeout timer
    connect(_timeoutTimer, &QTimer::timeout, [=] { onError(QAbstractSocket::SocketError::SocketTimeoutError); });

    // Connect socket [dis]connections
    connect(_socket, &QTcpSocket::connected, this, &NetworkClient::onConnected);
    connect(_socket, &QTcpSocket::disconnected, this, &NetworkClient::onDisconnected);

    // Connect socket reads/errors
    connect(_socket, &QTcpSocket::readyRead, [=] { onReadyRead(_socket); });
    connect(_socket, &QAbstractSocket::errorOccurred, this, &NetworkClient::onError);
}

void NetworkClient::tryJoinGame(QHostAddress const& hostAddress, PlayerColor color, QString const& username, quint16 port)
{
    _color = color;
    _username = username;

    _timeoutTimer->start();
    _socket->connectToHost(hostAddress, port);
}

void NetworkClient::leaveGame()
{
    _socket->disconnectFromHost();
    onDisconnected();
}

void NetworkClient::sendPositionUpdate(QPointF position)
{
    sendMessage(_socket, positionMessage(_color, position));
}

void NetworkClient::sendBulletUpdate(QPointF source, qreal angle)
{
    sendMessage(_socket, bulletMessage(_color, source, angle));
}

void NetworkClient::sendHealthUpdate(PlayerColor color, int health, bool hasCrown)
{
    sendMessage(_socket, healthMessage(color, health, hasCrown));
}

void NetworkClient::sendChatMessage(QString const& body)
{
    sendMessage(_socket, chatMessage(_color, _username, body));
}

void NetworkClient::onConnected()
{
    _hasJoinedGame = false;
    _hasGameStarted = false;
    _usernames.clear();

    _timeoutTimer->start();
    sendMessage(_socket, joinRequest(_color, _username));
}

void NetworkClient::onDisconnected()
{
    _timeoutTimer->stop();

    _hasJoinedGame = false;
    _hasGameStarted = false;
    _usernames.clear();

    emit leftGame();
}

void NetworkClient::onError(QAbstractSocket::SocketError socketError)
{
    leaveGame();
    emit error(socketError);
}

void NetworkClient::receivedFrom(QAbstractSocket* socket)
{
    if (socket == _socket)
    {
        // Refresh network timeout timer upon receiving message
        _timeoutTimer->stop();

        if (_hasGameStarted)
        {
            _timeoutTimer->start();
        }
    }
}

void NetworkClient::onParsedJoinResponse(bool succeeded, PlayerColor color, QString const& username, JoinError error)
{
    _hasJoinedGame = succeeded;

    if (succeeded)
    {
        _color = color;
        _username = username;

        emit joinedGame(color, username);
    }
    else
    {
        emit joinGameFailed(error);
    }
}

void NetworkClient::onParsedPositionMessage(PlayerColor color, QPointF position)
{
    emit positionUpdated(color, position);
}

void NetworkClient::onParsedBulletMessage(PlayerColor color, QPointF source, qreal angle)
{
    emit bulletUpdated(color, source, angle);
}

void NetworkClient::onParsedHealthMessage(PlayerColor color, int health, bool hasCrown)
{
    emit healthUpdated(color, health, hasCrown);
}

void NetworkClient::onParsedGameStartMessage(int gameTime)
{
    _hasGameStarted = true;
    emit gameStarted(gameTime);
}

void NetworkClient::onParsedGameEndMessage(PlayerColor winner, QString const& username)
{
    _hasGameStarted = false;
    emit gameEnded(winner, username);
}

void NetworkClient::onParsedPlayerJoinedMessage(PlayerColor color, QString const& username)
{
    _usernames[color] = username;
    emit playerJoined(color, username);
}

void NetworkClient::onParsedPlayerLeftMessage(PlayerColor color, QString const& username)
{
    _usernames.remove(color);
    emit playerLeft(color, username);
}

void NetworkClient::onParsedChatMessage(PlayerColor color, QString const& username, QString const& body)
{
    emit receivedChatMessage(color, username, body);
}
