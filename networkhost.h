#ifndef NETWORKHOST_H
#define NETWORKHOST_H

#include "networkbase.h"

#include <QNetworkProxy>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTimer>

class NetworkHost : public NetworkBase
{
    Q_OBJECT

public:
    explicit NetworkHost(QObject* parent = nullptr);

    inline PlayerColor color() const
    {
        return _color;
    }

    inline QString const& username() const
    {
        return _username;
    }

    inline bool isHosting() const
    {
        return _hosting;
    }

    inline bool hasGameStarted() const
    {
        return _hasGameStarted;
    }

public slots:
    void startHosting(PlayerColor color, QString const& username, int maxPlayers = DEFAULT_MAX_PLAYERS, QHostAddress const& hostAddress = QHostAddress::Any, quint16 port = PORT_NUMBER);
    void stopHosting();

    void startGame(int gameTime);
    void endGame(PlayerColor winner, QString const& username);

    void sendPositionUpdate(QPointF position);
    void sendBulletUpdate(QPointF source, qreal angle);
    void sendHealthUpdate(PlayerColor color, int health, bool hasCrown);
    void sendChatMessage(QString const& body);

signals:
    void connected(QAbstractSocket* socket);
    void disconnected(QAbstractSocket* socket);
    void error(PlayerColor color, QString const& username, QAbstractSocket::SocketError socketError);

    void startedHosting(PlayerColor color, QString const& username);
    void stoppedHosting();

protected slots:
    void receivedFrom(QAbstractSocket* socket);

    void onParsedJoinRequest(QAbstractSocket* socket, PlayerColor color, QString const& username);
    void onParsedPositionMessage(PlayerColor color, QPointF position);
    void onParsedBulletMessage(PlayerColor color, QPointF source, qreal angle);
    void onParsedHealthMessage(PlayerColor color, int health, bool hasCrown);
    void onParsedChatMessage(PlayerColor color, QString const& username, QString const& body);

private slots:
    void onNewConnection();
    void onConnected(QAbstractSocket* socket);
    void onDisconnected(QAbstractSocket* socket);
    void onError(QAbstractSocket* socket, QAbstractSocket::SocketError socketError);

private:
    void sendMessageToClients(QJsonObject const& message, QAbstractSocket* except = nullptr);

    QTcpServer* _server;
    QMap<PlayerColor, QAbstractSocket*> _sockets;
    QMap<PlayerColor, QString> _usernames;
    QMap<QAbstractSocket*, QTimer*> _timeoutTimers;
    QList<QAbstractSocket*> _tempConnected;

    int _maxPlayers = DEFAULT_MAX_PLAYERS;
    bool _hosting = false;
    bool _hasGameStarted = false;
    QString _username = QStringLiteral("NULL");
    PlayerColor _color;
};

#endif // NETWORKHOST_H
