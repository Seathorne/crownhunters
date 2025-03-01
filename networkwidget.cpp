#include "networkwidget.h"
#include "ui_networkwidget.h"

#include <QMessageBox>

NetworkWidget::NetworkWidget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::NetworkWidget)
    , _client(new NetworkClient(this))
    , _host(new NetworkHost(this))
{
    ui->setupUi(this);

    for (int i = 0; i < DEFAULT_MAX_PLAYERS; i++)
    {
        QString const& color = playerColorToString(static_cast<PlayerColor>(i));
        ui->colorComboBox->addItem(color);
    }

    QRegExpValidator* ipValidator = new QRegExpValidator(QRegExp("^((25[0-5]|(2[0-4]|1[0-9]|[1-9]|)[0-9])(\\.(?!$)|$)){4}$"), this);
    connect(ui->hostAddressLineEdit, &QLineEdit::textChanged, this, &NetworkWidget::validateUserSettings);
    ui->hostAddressLineEdit->setValidator(ipValidator);

    QRegExpValidator* usernameValidator = new QRegExpValidator(QRegExp("^[a-zA-Z0-9]{1,16}$"), this);
    connect(ui->usernameLineEdit, &QLineEdit::textChanged, this, &NetworkWidget::validateUserSettings);
    ui->usernameLineEdit->setValidator(usernameValidator);

    QRegExpValidator* chatValidator = new QRegExpValidator(QRegExp("^[ \\S]{0,100}$"), this);
    connect(ui->chatLineEdit, &QLineEdit::textChanged, this, &NetworkWidget::validateChat);
    ui->chatLineEdit->setValidator(chatValidator);

    // Assign my address to address field
    ui->hostAddressLineEdit->setText(NetworkWidget::myAddress().toString());

    connect(ui->joinGameButton, &QAbstractButton::clicked, this, &NetworkWidget::tryJoinGame);
    connect(ui->leaveGameButton, &QAbstractButton::clicked, this, &NetworkWidget::leaveGame);
    connect(ui->chatButton, &QAbstractButton::clicked, this, &NetworkWidget::sendChatMessage);
    connect(ui->chatLineEdit, &QLineEdit::returnPressed, this, &NetworkWidget::sendChatMessage);

    connect(_client, &NetworkClient::error, [=](QAbstractSocket::SocketError socketError) { onSocketError(_client->color(), _client->username(), socketError); });
    connect(_client, &NetworkClient::joinedGame, this, &NetworkWidget::onJoinedGame);
    connect(_client, &NetworkClient::joinGameFailed, this, &NetworkWidget::onJoinGameFailed);
    connect(_client, &NetworkClient::leftGame, this, &NetworkWidget::onLeftGame);
    connect(_client, &NetworkClient::positionUpdated, this, &NetworkWidget::onPositionUpdated);
    connect(_client, &NetworkClient::bulletUpdated, this, &NetworkWidget::onBulletUpdated);
    connect(_client, &NetworkClient::healthUpdated, this, &NetworkWidget::onHealthUpdated);
    connect(_client, &NetworkClient::gameStarted, this, &NetworkWidget::onGameStarted);
    connect(_client, &NetworkClient::gameEnded, this, &NetworkWidget::onGameEnded);
    connect(_client, &NetworkClient::playerJoined, this, &NetworkWidget::onPlayerJoined);
    connect(_client, &NetworkClient::playerLeft, this, &NetworkWidget::onPlayerLeft);
    connect(_client, &NetworkClient::receivedChatMessage, this, &NetworkWidget::onReceivedChatMessage);

    // HOST
    connect(ui->usernameLineEdit, &QLineEdit::textChanged, this, &NetworkWidget::validateHostSettings);

    ui->myAddressLineEdit->setText(NetworkWidget::myAddress().toString());

    connect(ui->hostGameButton, &QAbstractButton::clicked, this, &NetworkWidget::startHosting);
    connect(ui->stopHostingButton, &QAbstractButton::clicked, this, &NetworkWidget::stopHosting);

    connect(ui->startGameButton, &QAbstractButton::clicked, this, &NetworkWidget::startGame);

    connect(_host, &NetworkHost::error, this, &NetworkWidget::onSocketError);
    connect(_host, &NetworkHost::startedHosting, this, &NetworkWidget::onStartedHosting);
    connect(_host, &NetworkHost::stoppedHosting, this, &NetworkWidget::onStoppedHosting);
    connect(_host, &NetworkHost::positionUpdated, this, &NetworkWidget::onPositionUpdated);
    connect(_host, &NetworkHost::bulletUpdated, this, &NetworkWidget::onBulletUpdated);
    connect(_host, &NetworkHost::healthUpdated, this, &NetworkWidget::onHealthUpdated);
    connect(_host, &NetworkHost::gameStarted, this, &NetworkWidget::onGameStarted);
    connect(_host, &NetworkHost::gameEnded, this, &NetworkWidget::onGameEnded);
    connect(_host, &NetworkHost::playerJoined, this, &NetworkWidget::onPlayerJoined);
    connect(_host, &NetworkHost::playerLeft, this, &NetworkWidget::onPlayerLeft);
    connect(_host, &NetworkHost::receivedChatMessage, this, &NetworkWidget::onReceivedChatMessage);

    setMode(Mode::None);
}

NetworkWidget::~NetworkWidget()
{
    delete ui;
}

void NetworkWidget::setUsername(QString const& newValue)
{
    ui->usernameLineEdit->setText(newValue);
}

void NetworkWidget::setColor(PlayerColor newValue)
{
    ui->colorComboBox->setCurrentIndex(static_cast<int>(newValue));
}

void NetworkWidget::setMaxPlayers(int newValue)
{
    ui->maxPlayersSpinBox->setValue(newValue);
}

void NetworkWidget::setGameTime(int newValue)
{
    ui->gameLengthSpinBox->setValue(newValue);
}

void NetworkWidget::setMode(Mode mode)
{
    _mode = mode;

    bool none = mode == Mode::None;

    ui->hostAddressLineEdit->setEnabled(none);
    ui->usernameLineEdit->setEnabled(none);
    ui->colorComboBox->setEnabled(none);

    ui->maxPlayersSpinBox->setEnabled(none);
    ui->gameLengthSpinBox->setEnabled(none);

    ui->chatLineEdit->setEnabled(!none);

    switch (mode)
    {
    case Mode::None:
        validateUserSettings();
        ui->leaveGameButton->setEnabled(false);

        validateHostSettings();
        ui->stopHostingButton->setEnabled(false);
        ui->startGameButton->setEnabled(false);

        ui->chatLineEdit->clear();
        ui->chatButton->setEnabled(false);
        break;
    case Mode::Client:
        validateUserSettings();
        ui->leaveGameButton->setEnabled(true);

        ui->hostGameButton->setEnabled(false);
        ui->stopHostingButton->setEnabled(false);
        ui->startGameButton->setEnabled(false);

        validateChat();
        break;
    case Mode::Host:
        ui->joinGameButton->setEnabled(false);
        ui->leaveGameButton->setEnabled(false);

        validateHostSettings();
        ui->stopHostingButton->setEnabled(true);
        ui->startGameButton->setEnabled(true);

        validateChat();
        break;
    }

    // Override since username/color are set via config dialog
    ui->usernameLineEdit->setEnabled(false);
    ui->colorComboBox->setEnabled(false);
    ui->maxPlayersSpinBox->setEnabled(false);
    ui->gameLengthSpinBox->setEnabled(false);

    emit modeChanged(_mode);
}

// HOST

void NetworkWidget::startHosting()
{
    setMode(Mode::Host);

    QHostAddress const& address = QHostAddress(ui->hostAddressLineEdit->text());
    QString const& username = ui->usernameLineEdit->text();
    PlayerColor color = parsePlayerColor(ui->colorComboBox->currentText());

    ui->chatTextBrowser->append(QString("* Hosting game at %1.").arg(address.toString()));

    _host->startHosting(color, username, ui->maxPlayersSpinBox->value());
}

void NetworkWidget::stopHosting()
{
    setMode(Mode::None);

    _host->stopHosting();
}

void NetworkWidget::startGame()
{
    _host->startGame(ui->gameLengthSpinBox->value());
}

void NetworkWidget::validateHostSettings()
{
    bool isUsernameAcceptable = ui->usernameLineEdit->hasAcceptableInput();

    ui->hostGameButton->setEnabled(isUsernameAcceptable && !_host->isHosting());

    this->showError(ui->usernameLineEdit, !isUsernameAcceptable);
}

void NetworkWidget::onStartedHosting(PlayerColor color, QString const& username)
{
    setMode(Mode::Host);

    ui->usernameLineEdit->setText(username);
    ui->colorComboBox->setCurrentIndex(static_cast<int>(color));

    ui->startGameButton->setEnabled(true);
}

void NetworkWidget::onStoppedHosting()
{
    setMode(Mode::None);

    ui->chatTextBrowser->append(QStringLiteral("* Stopped hosting."));
}

// END HOST

void NetworkWidget::tryJoinGame()
{
    setMode(Mode::Client);

    QHostAddress const& address = QHostAddress(ui->hostAddressLineEdit->text());
    QString const& username = ui->usernameLineEdit->text();
    PlayerColor color = parsePlayerColor(ui->colorComboBox->currentText());

    ui->chatTextBrowser->append(QString("* Trying to join game at %1...").arg(address.toString()));

    _client->tryJoinGame(address, color, username);
}

void NetworkWidget::leaveGame()
{
    setMode(Mode::None);

    _client->leaveGame();
}

void NetworkWidget::sendChatMessage()
{
    QString const& message = ui->chatLineEdit->text();
    if (!(message.isNull() || message.isEmpty()))
    {
        if (_mode == Mode::Client)
        {
            _client->sendChatMessage(message);
            ui->chatTextBrowser->append(QString("%1: %2").arg(_client->username()).arg(message));
        }
        else if (_mode == Mode::Host)
        {
            _host->sendChatMessage(message);
            ui->chatTextBrowser->append(QString("%1: %2").arg(_host->username()).arg(message));
        }

        ui->chatLineEdit->clear();
    }
}

void NetworkWidget::validateUserSettings()
{
    bool isUsernameAcceptable = ui->usernameLineEdit->hasAcceptableInput();
    bool isAddressAcceptable = ui->hostAddressLineEdit->hasAcceptableInput();

    ui->joinGameButton->setEnabled(isUsernameAcceptable && isAddressAcceptable
        && !(_client->hasJoinedGame() || _client->hasGameStarted()));

    this->showError(ui->usernameLineEdit, !isUsernameAcceptable);
    this->showError(ui->hostAddressLineEdit, !isAddressAcceptable);
}

void NetworkWidget::validateChat()
{
    bool isChatAcceptable = ui->chatLineEdit->hasAcceptableInput();
    ui->chatButton->setEnabled(isChatAcceptable);
    this->showError(ui->chatLineEdit, !isChatAcceptable);
}

void NetworkWidget::onSocketError(PlayerColor color, QString const& username, QAbstractSocket::SocketError error)
{
    if (color == _client->color() || color == _host->color())
    {
        setMode(Mode::None);
    }

    QString explanation;

    // What kind of error occurred
    switch (error)
    {
    case QAbstractSocket::RemoteHostClosedError:
    case QAbstractSocket::ProxyConnectionClosedError:
        // Handled by disconnectedFromServer
        return;
    case QAbstractSocket::ConnectionRefusedError:
        explanation = QStringLiteral("The host refused the connection");
        break;
    case QAbstractSocket::ProxyConnectionRefusedError:
        explanation = QStringLiteral("The proxy refused the connection");
        break;
    case QAbstractSocket::ProxyNotFoundError:
        explanation = QStringLiteral("Could not find the proxy");
        break;
    case QAbstractSocket::HostNotFoundError:
        explanation = QStringLiteral("Could not find the server");
        break;
    case QAbstractSocket::SocketAccessError:
        explanation = QStringLiteral("You don't have permissioSns to execute this operation");
        break;
    case QAbstractSocket::SocketResourceError:
        explanation = QStringLiteral("Too many connections opened");
        break;
    case QAbstractSocket::SocketTimeoutError:
        explanation = QStringLiteral("Operation timed out");
        return;
    case QAbstractSocket::ProxyConnectionTimeoutError:
        explanation = QStringLiteral("Proxy timed out");
        break;
    case QAbstractSocket::NetworkError:
        explanation = QStringLiteral("Unable to reach the network");
        break;
    case QAbstractSocket::UnknownSocketError:
        explanation = QStringLiteral("An unknown error occurred");
        break;
    case QAbstractSocket::UnsupportedSocketOperationError:
        explanation = QStringLiteral("Operation not supported");
        break;
    case QAbstractSocket::ProxyAuthenticationRequiredError:
        explanation = QStringLiteral("Your proxy requires authentication");
        break;
    case QAbstractSocket::ProxyProtocolError:
        explanation = QStringLiteral("Proxy communication failed");
        break;
    case QAbstractSocket::TemporaryError:
    case QAbstractSocket::OperationError:
        explanation = QStringLiteral("Operation failed, please try again");
        return;
    default:
        Q_UNREACHABLE();
    }

    ui->chatTextBrowser->append(QString("* Socket error (%1) from [%2] %3: %4.").arg(error).arg(playerColorToString(color)).arg(username).arg(explanation));
}

void NetworkWidget::onJoinedGame(PlayerColor color, QString const& username)
{
    setMode(Mode::Client);

    ui->usernameLineEdit->setText(username);
    ui->colorComboBox->setCurrentIndex(static_cast<int>(color));

    ui->chatTextBrowser->append(QString("* Joined game."));
}

void NetworkWidget::onJoinGameFailed(NetworkBase::JoinError error)
{
    setMode(Mode::None);

    ui->chatTextBrowser->append(QString("* Failed to join game: %1.").arg(NetworkBase::toString(error)));
}

void NetworkWidget::onLeftGame()
{
    setMode(Mode::None);

    ui->chatTextBrowser->append(QStringLiteral("* Disconnected."));
}

void NetworkWidget::onPlayerJoined(PlayerColor color, QString const& username)
{
    ui->chatTextBrowser->append(QString("* [%1] %2 has joined the game.").arg(playerColorToString(color)).arg(username));
}

void NetworkWidget::onPlayerLeft(PlayerColor color, QString const& username)
{
    ui->chatTextBrowser->append(QString("* [%1] %2 has left the game.").arg(playerColorToString(color)).arg(username));
}

void NetworkWidget::onPositionUpdated(PlayerColor, QPointF)
{
//    ui->chatTextBrowser->append((QString("%1 position (%2, %3)")).arg(playerColorToString(color)).arg(position.x()).arg(position.y()));
}

void NetworkWidget::onHealthUpdated(PlayerColor, int)
{
//    ui->chatTextBrowser->append((QString("%1 health %2")).arg(playerColorToString(color)).arg(health));
}

void NetworkWidget::onBulletUpdated(PlayerColor, QPointF, qreal)
{
//    ui->chatTextBrowser->append((QString("%1 bullet (%2, %3) %4")).arg(playerColorToString(color)).arg(source.x()).arg(source.y()).arg(angle));
}

void NetworkWidget::onReceivedChatMessage(PlayerColor color, QString const& username, QString const& message)
{
    ui->chatTextBrowser->append(QString("[%1] %2: %3").arg(playerColorToString(color)).arg(username).arg(message));
}

void NetworkWidget::onGameStarted(int gameTime)
{
    ui->chatTextBrowser->append(QString("The game (%1) is about to start!").arg(gameTime));
}

void NetworkWidget::onGameEnded(PlayerColor winner, QString const& username)
{
    ui->chatTextBrowser->append(QString("Game over! [%1] %2 wins!").arg(playerColorToString(winner)).arg(username));
}

QHostAddress const NetworkWidget::myAddress()
{
    for (QHostAddress const& address : QNetworkInterface::allAddresses())
    {
        if (address.isLoopback() == false
            && address.isGlobal()
            && address.protocol() == QAbstractSocket::IPv4Protocol)
        {
            return address;
        }
    }

    // If no suitable address found, assign 0.0.0.0
    return QHostAddress::Null;
}

void NetworkWidget::showError(QWidget* thing, bool showError)
{
    static QPalette original = thing->palette();

    QPalette palette = original;
    if (showError)
    {
        palette.setColor(QPalette::Window, Qt::red);
        palette.setColor(QPalette::Highlight, Qt::red);
    }

    thing->setPalette(palette);
}
