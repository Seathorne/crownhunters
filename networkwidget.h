#ifndef NETWORKWIDGET_H
#define NETWORKWIDGET_H

#include "networkclient.h"
#include "networkhost.h"
#include "settings.h"

#include <QNetworkInterface>
#include <QWidget>

namespace Ui {
class NetworkWidget;
}

class NetworkWidget : public QWidget
{
    Q_OBJECT

public:
    enum Mode
    {
        None,
        Host,
        Client,
    };

    explicit NetworkWidget(QWidget *parent = nullptr);
    ~NetworkWidget();

    inline NetworkClient* client() const
    {
        return _client;
    }

    inline NetworkHost* host() const
    {
        return _host;
    }

    inline Mode mode() const
    {
        return _mode;
    }

signals:
    void modeChanged(Mode newValue);

public slots:
    void setUsername(QString const& newValue);
    void setColor(PlayerColor newValue);
    void setMaxPlayers(int newValue);
    void setGameTime(int newValue);

private slots:
    void tryJoinGame();
    void leaveGame();
    void sendChatMessage();

    void validateUserSettings();
    void validateChat();

    void onSocketError(PlayerColor color, QString const& username, QAbstractSocket::SocketError error);
    void onJoinedGame(PlayerColor color, QString const& username);
    void onJoinGameFailed(NetworkBase::JoinError error);
    void onLeftGame();

    void onPositionUpdated(PlayerColor color, QPointF position);
    void onBulletUpdated(PlayerColor color, QPointF source, qreal angle);
    void onHealthUpdated(PlayerColor color, int health);
    void onGameStarted(int gameTime);
    void onGameEnded(PlayerColor winner, QString const& username);
    void onPlayerJoined(PlayerColor color, QString const& username);
    void onPlayerLeft(PlayerColor color, QString const& username);
    void onReceivedChatMessage(PlayerColor color, QString const& username, QString const& body);

    void setMode(Mode _mode);

    // HOST
    void startHosting();
    void stopHosting();
    void startGame();

    void validateHostSettings();

    void onStartedHosting(PlayerColor color, QString const& username);
    void onStoppedHosting();

private:
    Ui::NetworkWidget *ui;

    NetworkClient* _client;
    NetworkHost* _host;
    Mode _mode = Mode::None;

    static QHostAddress const myAddress();
    static void showError(QWidget* thing, bool showError);

};

#endif // NETWORKWIDGET_H
