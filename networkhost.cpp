#include "networkhost.h"

NetworkHost::NetworkHost(QObject* parent)
    : NetworkBase(parent)
    , _server(new QTcpServer(this))
{
    // Connect server signals
    connect(_server, &QTcpServer::newConnection, this, &NetworkHost::onNewConnection);
}

void NetworkHost::onNewConnection()
{
    while (QTcpSocket* clientSocket = _server->nextPendingConnection())
    {
        onConnected(clientSocket);
    }
}

void NetworkHost::onConnected(QAbstractSocket* socket)
{
    socket->setProxy(QNetworkProxy::NoProxy);
    socket->open(QIODevice::ReadWrite);

    _tempConnected.append(socket);

    // Connect client [dis]connect events
    connect(socket, &QAbstractSocket::disconnected, [=] { onDisconnected(socket); });
    connect(socket, &QAbstractSocket::readyRead, [=] { onReadyRead(socket); });

    // Set up client timeout timer
    QTimer* timeoutTimer = new QTimer(this);
    timeoutTimer->setSingleShot(true);
    timeoutTimer->setInterval(NETWORK_TIMEOUT);
    _timeoutTimers[socket] = timeoutTimer;

    // Connect client timeout timer
    connect(timeoutTimer, &QTimer::timeout, [=] { onError(socket, QAbstractSocket::SocketError::SocketTimeoutError); });
}

void NetworkHost::onDisconnected(QAbstractSocket* socket)
{
    // If socket is a player who has joined the game
    if (_sockets.values().contains(socket))
    {
        PlayerColor color = _sockets.key(socket);
        QString username = _usernames[color];

        _sockets.remove(color);
        _usernames.remove(color);

        qDebug() << "player left" << username;

        sendMessageToClients(playerLeftMessage(color, username));
        emit playerLeft(color, username);
    }

    // Stop timeout timer
    QTimer* timer = _timeoutTimers[socket];
    _timeoutTimers.remove(socket);
    timer->stop();
    timer->deleteLater();

    // Delete socket
    _tempConnected.removeOne(socket);
    socket->deleteLater();
}

void NetworkHost::onError(QAbstractSocket* socket, QAbstractSocket::SocketError socketError)
{
    // If socket is a player who has joined the game
    if (_sockets.values().contains(socket))
    {
        PlayerColor color = _sockets.key(socket);
        QString username = _usernames[color];

        emit error(color, username, socketError);
    }

    socket->disconnectFromHost();
    onDisconnected(socket);
}

void NetworkHost::startHosting(PlayerColor color, QString const& username, int maxPlayers, QHostAddress const& hostAddress, quint16 port)
{
    _sockets.clear();
    _usernames.clear();
    _tempConnected.clear();

    _color = color;
    _username = username;
    _usernames[color] = username;

    _maxPlayers = maxPlayers;
    _hosting = true;
    _hasGameStarted = false;

    _server->listen(hostAddress, port);
    emit startedHosting(color, username);
}

void NetworkHost::stopHosting()
{
    _hosting = false;
    _hasGameStarted = false;

    sendMessageToClients(gameEndMessage(_color, _username));

    for (QAbstractSocket* clientSocket : _sockets)
    {
        clientSocket->disconnectFromHost();
        clientSocket->deleteLater();
    }

    _sockets.clear();
    _usernames.clear();
    _tempConnected.clear();

    _server->close();
    emit stoppedHosting();
}

void NetworkHost::startGame(int gameTime)
{
    if (!_hosting)
    {
        return;
    }

    _hasGameStarted = true;

    sendMessageToClients(gameStartMessage(gameTime));
    emit gameStarted(gameTime);
}

void NetworkHost::endGame(PlayerColor winner, QString const& username)
{
    _hasGameStarted = false;

    sendMessageToClients(gameEndMessage(winner, username));
    emit gameEnded(winner, username);
}

void NetworkHost::sendPositionUpdate(QPointF position)
{
    sendMessageToClients(positionMessage(_color, position));
}

void NetworkHost::sendBulletUpdate(QPointF source, qreal angle)
{
    sendMessageToClients(bulletMessage(_color, source, angle));
}

void NetworkHost::sendHealthUpdate(PlayerColor color, int health, bool hasCrown)
{
    sendMessageToClients(healthMessage(color, health, hasCrown));
}

void NetworkHost::sendChatMessage(QString const& body)
{
    sendMessageToClients(chatMessage(_color, _username, body));
}

void NetworkHost::receivedFrom(QAbstractSocket* socket)
{
    if (_timeoutTimers.contains(socket))
    {
        QTimer* timeoutTimer = _timeoutTimers[socket];

        // Refresh network timeout timer upon receiving message
        timeoutTimer->stop();

        if (_hasGameStarted)
        {
            timeoutTimer->start();
        }
    }
}

void NetworkHost::onParsedJoinRequest(QAbstractSocket* socket, PlayerColor color, QString const& username)
{
    JoinError error = JoinError::NO_ERROR;

    if (_usernames.size() >= _maxPlayers)
    {
        error = static_cast<JoinError>(error | JoinError::GAME_FULL);
    }

    if (_usernames.keys().contains(color))
    {
        error = static_cast<JoinError>(error | JoinError::COLOR_TAKEN);
    }

    if (_usernames.values().contains(username))
    {
        error = static_cast<JoinError>(error | JoinError::USERNAME_TAKEN);
    }

    bool succeeded = (error == JoinError::NO_ERROR);

    // Inform user whether join is accepted or rejected
    sendMessage(socket, joinResponse(succeeded, color, username, error));

    if (succeeded)
    {
        _usernames[color] = username;
        _sockets[color] = socket;

        // Notify other clients of new player
        sendMessageToClients(playerJoinedMessage(color, username));
        emit playerJoined(color, username);
    }
}

void NetworkHost::onParsedPositionMessage(PlayerColor color, QPointF position)
{
    // Forward position to all players except sender
    sendMessageToClients(positionMessage(color, position), /* except */ _sockets[color]);
    emit positionUpdated(color, position);
}

void NetworkHost::onParsedBulletMessage(PlayerColor color, QPointF source, qreal angle)
{
    // Forward bullet to all players except sender
    sendMessageToClients(bulletMessage(color, source, angle), /* except */ _sockets[color]);
    emit bulletUpdated(color, source, angle);
}

void NetworkHost::onParsedHealthMessage(PlayerColor color, int health, bool hasCrown)
{
    // Forward health update to all players except sender
    sendMessageToClients(healthMessage(color, health, hasCrown), /* except */ _sockets[color]);
    emit healthUpdated(color, health, hasCrown);
}

void NetworkHost::onParsedChatMessage(PlayerColor color, QString const& username, QString const& body)
{
    // Forward chat message to all clients
    sendMessageToClients(chatMessage(color, username, body), /* except */ _sockets[color]);
    emit receivedChatMessage(color, username, body);
}

void NetworkHost::sendMessageToClients(QJsonObject const& message, QAbstractSocket* except)
{
    for (QAbstractSocket* clientSocket : _sockets.values())
    {
        if (clientSocket != except)
        {
            sendMessage(clientSocket, message);
        }
    }
}
