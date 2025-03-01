#ifndef NETWORKCLIENT_H
#define NETWORKCLIENT_H

#include "networkbase.h"

#include <QTcpSocket>
#include <QTimer>

class NetworkClient : public NetworkBase
{
    Q_OBJECT

public:
    explicit NetworkClient(QObject* parent = nullptr);

    inline PlayerColor color() const
    {
        return _color;
    }

    inline QString const& username() const
    {
        return _username;
    }

    inline bool hasJoinedGame() const
    {
        return _hasJoinedGame;
    }

    inline bool hasGameStarted() const
    {
        return _hasGameStarted;
    }

public slots:
    void tryJoinGame(QHostAddress const& hostAddress, PlayerColor color, QString const& username, quint16 port = PORT_NUMBER);
    void leaveGame();

    void sendPositionUpdate(QPointF position);
    void sendBulletUpdate(QPointF source, qreal angle);
    void sendHealthUpdate(PlayerColor color, int health, bool hasCrown);
    void sendChatMessage(QString const& body);

signals:
    void error(QAbstractSocket::SocketError error);
    void joinedGame(PlayerColor color, QString const& username);
    void joinGameFailed(JoinError error);
    void leftGame();

protected:
    void receivedFrom(QAbstractSocket* socket);

    void onParsedJoinResponse(bool succeeded, PlayerColor color, QString const& username, JoinError error = NO_ERROR);
    void onParsedPositionMessage(PlayerColor color, QPointF position);
    void onParsedBulletMessage(PlayerColor color, QPointF source, qreal angle);
    void onParsedHealthMessage(PlayerColor color, int health, bool hasCrown);
    void onParsedGameStartMessage(int gameTime);
    void onParsedGameEndMessage(PlayerColor winner, QString const& username);
    void onParsedPlayerJoinedMessage(PlayerColor color, QString const& username);
    void onParsedPlayerLeftMessage(PlayerColor color, QString const& username);
    void onParsedChatMessage(PlayerColor color, QString const& username, QString const& body);

private slots:
    void onConnected();
    void onDisconnected();
    void onError(QAbstractSocket::SocketError error);

private:
    QAbstractSocket* _socket;
    QTimer* _timeoutTimer;

    bool _hasJoinedGame = false;
    bool _hasGameStarted = false;
    PlayerColor _color;
    QString _username = QStringLiteral("NULL");

    QMap<PlayerColor, QString> _usernames;
};

#endif // NETWORKCLIENT_H
