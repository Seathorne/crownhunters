#include "networkbase.h"

NetworkBase::NetworkBase(QObject* parent)
    : QObject(parent)
{

}

void NetworkBase::sendMessage(QAbstractSocket* socket, QJsonObject const& message)
{
    QDataStream ds(socket);
    ds.setVersion(COMPRESSION_VERSION);
    ds << QJsonDocument(message).toJson(QJsonDocument::Compact);
}

QJsonObject const NetworkBase::joinRequest(PlayerColor color, QString const& username)
{
    QJsonObject message;
    message[toString(MessageParam::MESSAGE_TYPE)] = toString(MessageType::JOIN_REQUEST);
    message[toString(MessageParam::COLOR)] = static_cast<int>(color);
    message[toString(MessageParam::USERNAME)] = username;
    return message;
}

QJsonObject const NetworkBase::joinResponse(bool succeeded, PlayerColor color, QString const& username, JoinError error)
{
    QJsonObject message;
    message[toString(MessageParam::MESSAGE_TYPE)] = toString(MessageType::JOIN_RESPONSE);
    message[toString(MessageParam::JOIN_SUCCEEDED)] = succeeded;
    message[toString(MessageParam::COLOR)] = static_cast<int>(color);
    message[toString(MessageParam::USERNAME)] = username;
    message[toString(MessageParam::JOIN_ERROR)] = error;
    return message;
}

QJsonObject const NetworkBase::positionMessage(PlayerColor color, QPointF position)
{
    QJsonObject message;
    message[toString(MessageParam::MESSAGE_TYPE)] = toString(MessageType::POSITION_UPDATE);
    message[toString(MessageParam::COLOR)] = static_cast<int>(color);
    message[toString(MessageParam::POSITION_X)] = position.x();
    message[toString(MessageParam::POSITION_Y)] = position.y();
    return message;
}

QJsonObject const NetworkBase::bulletMessage(PlayerColor color, QPointF source, qreal angle)
{
    QJsonObject message;
    message[toString(MessageParam::MESSAGE_TYPE)] = toString(MessageType::BULLET_SHOT);
    message[toString(MessageParam::COLOR)] = static_cast<int>(color);
    message[toString(MessageParam::POSITION_X)] = source.x();
    message[toString(MessageParam::POSITION_Y)] = source.y();
    message[toString(MessageParam::BULLET_ANGLE)] = angle;
    return message;
}

QJsonObject const NetworkBase::healthMessage(PlayerColor color, int health, bool hasCrown)
{
    QJsonObject message;
    message[toString(MessageParam::MESSAGE_TYPE)] = toString(MessageType::HEALTH_UPDATE);
    message[toString(MessageParam::COLOR)] = static_cast<int>(color);
    message[toString(MessageParam::HEALTH)] = health;
    message[toString(MessageParam::CROWN)] = hasCrown;
    return message;
}

QJsonObject const NetworkBase::gameStartMessage(int gameTime)
{
    QJsonObject message;
    message[toString(MessageParam::MESSAGE_TYPE)] = toString(MessageType::GAME_START);
    message[toString(MessageParam::GAME_TIME)] = gameTime;
    return message;
}

QJsonObject const NetworkBase::gameEndMessage(PlayerColor winner, QString const& username)
{
    QJsonObject message;
    message[toString(MessageParam::MESSAGE_TYPE)] = toString(MessageType::GAME_END);
    message[toString(MessageParam::COLOR)] = static_cast<int>(winner);
    message[toString(MessageParam::USERNAME)] = username;
    return message;
}

QJsonObject const NetworkBase::playerJoinedMessage(PlayerColor color, QString const& username)
{
    QJsonObject message;
    message[toString(MessageParam::MESSAGE_TYPE)] = toString(MessageType::PLAYER_JOINED);
    message[toString(MessageParam::COLOR)] = static_cast<int>(color);
    message[toString(MessageParam::USERNAME)] = username;
    return message;
}

QJsonObject const NetworkBase::playerLeftMessage(PlayerColor color, QString const& username)
{
    QJsonObject message;
    message[toString(MessageParam::MESSAGE_TYPE)] = toString(MessageType::PLAYER_LEFT);
    message[toString(MessageParam::COLOR)] = static_cast<int>(color);
    message[toString(MessageParam::USERNAME)] = username;
    return message;
}

QJsonObject const NetworkBase::chatMessage(PlayerColor color, QString const& username, QString const& body)
{
    QJsonObject message;
    message[toString(MessageParam::MESSAGE_TYPE)] = toString(MessageType::CHAT_MESSAGE);
    message[toString(MessageParam::COLOR)] = static_cast<int>(color);
    message[toString(MessageParam::USERNAME)] = username;
    message[toString(MessageParam::CHAT_BODY)] = body;
    return message;
}

bool NetworkBase::parseJoinRequest(QAbstractSocket* socket, QJsonObject const& message)
{
    QJsonValue colorValue = message.value(toString(MessageParam::COLOR));
    if (colorValue.isNull())
    {
        return false;
    }

    QJsonValue usernameValue = message.value(toString(MessageParam::USERNAME));
    if (usernameValue.isNull() || !usernameValue.isString())
    {
        return false;
    }

    PlayerColor color = static_cast<PlayerColor>(colorValue.toInt());
    QString const& username = usernameValue.toString();

    onParsedJoinRequest(socket, color, username);
    return true;
}

bool NetworkBase::parseJoinResponse(QJsonObject const& message)
{
    QJsonValue succeededValue = message.value(toString(MessageParam::JOIN_SUCCEEDED));
    if (succeededValue.isNull() || !succeededValue.isBool())
    {
        return false;
    }

    QJsonValue colorValue = message.value(toString(MessageParam::COLOR));
    if (colorValue.isNull())
    {
        return false;
    }

    QJsonValue usernameValue = message.value(toString(MessageParam::USERNAME));
    if (usernameValue.isNull() || !usernameValue.isString())
    {
        return false;
    }

    QJsonValue errorValue = message.value(toString(MessageParam::JOIN_ERROR));
    if (errorValue.isNull())
    {
        return false;
    }

    bool succeeded = succeededValue.toBool();
    PlayerColor color = static_cast<PlayerColor>(colorValue.toInt());
    QString const& username = usernameValue.toString();
    JoinError error = static_cast<JoinError>(errorValue.toInt());

    onParsedJoinResponse(succeeded, color, username, error);
    return true;
}

bool NetworkBase::parsePositionMessage(QJsonObject const& message)
{
    QJsonValue colorValue = message.value(toString(MessageParam::COLOR));
    if (colorValue.isNull())
    {
        return false;
    }

    QJsonValue xValue = message.value(toString(MessageParam::POSITION_X));
    if (xValue.isNull() || !xValue.isDouble())
    {
        return false;
    }

    QJsonValue yValue = message.value(toString(MessageParam::POSITION_Y));
    if (yValue.isNull() || !yValue.isDouble())
    {
        return false;
    }

    PlayerColor color = static_cast<PlayerColor>(colorValue.toInt());
    qreal x = xValue.toDouble();
    qreal y = yValue.toDouble();
    QPointF position(x, y);

    onParsedPositionMessage(color, position);
    return true;
}

bool NetworkBase::parseHealthMessage(QJsonObject const& message)
{
    QJsonValue colorValue = message.value(toString(MessageParam::COLOR));
    if (colorValue.isNull())
    {
        return false;
    }

    QJsonValue healthValue = message.value(toString(MessageParam::HEALTH));
    if (healthValue.isNull())
    {
        return false;
    }

    QJsonValue hasCrownValue = message.value(toString(MessageParam::CROWN));
    if (hasCrownValue.isNull() || !hasCrownValue.isBool())
    {
        return false;
    }

    PlayerColor color = static_cast<PlayerColor>(colorValue.toInt());
    int health = healthValue.toInt();
    bool hasCrown = hasCrownValue.toBool();

    onParsedHealthMessage(color, health, hasCrown);
    return true;
}

bool NetworkBase::parseBulletMessage(QJsonObject const& message)
{
    QJsonValue colorValue = message.value(toString(MessageParam::COLOR));
    if (colorValue.isNull())
    {
        return false;
    }

    QJsonValue sourceXValue = message.value(toString(MessageParam::POSITION_X));
    if (sourceXValue.isNull() || !sourceXValue.isDouble())
    {
        return false;
    }

    QJsonValue sourceYValue = message.value(toString(MessageParam::POSITION_Y));
    if (sourceYValue.isNull() || !sourceYValue.isDouble())
    {
        return false;
    }

    QJsonValue angleValue = message.value(toString(MessageParam::BULLET_ANGLE));
    if (angleValue.isNull() || !angleValue.isDouble())
    {
        return false;
    }

    PlayerColor color = static_cast<PlayerColor>(colorValue.toInt());
    qreal sourceX = sourceXValue.toDouble();
    qreal sourceY = sourceYValue.toDouble();
    qreal angle = angleValue.toDouble();
    QPointF source(sourceX, sourceY);

    onParsedBulletMessage(color, source, angle);
    return true;
}

bool NetworkBase::parseGameStartMessage(QJsonObject const& message)
{
    QJsonValue gameTimeValue = message.value(toString(MessageParam::GAME_TIME));
    if (gameTimeValue.isNull())
    {
        return false;
    }

    int gameTime = gameTimeValue.toInt();

    onParsedGameStartMessage(gameTime);
    return true;
}

bool NetworkBase::parseGameEndMessage(QJsonObject const& message)
{
    QJsonValue winnerValue = message.value(toString(MessageParam::COLOR));
    if (winnerValue.isNull())
    {
        return false;
    }

    QJsonValue const usernameValue = message.value(toString(MessageParam::USERNAME));
    if (usernameValue.isNull() || !usernameValue.isString())
    {
        return false;
    }

    PlayerColor winner = static_cast<PlayerColor>(winnerValue.toInt());
    QString const& username = usernameValue.toString();

    onParsedGameEndMessage(winner, username);
    return true;
}

bool NetworkBase::parsePlayerJoinedMessage(QJsonObject const& message)
{
    QJsonValue const colorValue = message.value(toString(MessageParam::COLOR));
    if (colorValue.isNull())
    {
        return false;
    }

    QJsonValue const usernameValue = message.value(toString(MessageParam::USERNAME));
    if (usernameValue.isNull() || !usernameValue.isString())
    {
        return false;
    }

    PlayerColor color = static_cast<PlayerColor>(colorValue.toInt());
    QString const& username = usernameValue.toString();

    onParsedPlayerJoinedMessage(color, username);
    return true;
}

bool NetworkBase::parsePlayerLeftMessage(QJsonObject const& message)
{
    QJsonValue const colorValue = message.value(toString(MessageParam::COLOR));
    if (colorValue.isNull())
    {
        return false;
    }

    QJsonValue const usernameValue = message.value(toString(MessageParam::USERNAME));
    if (usernameValue.isNull() || !usernameValue.isString())
    {
        return false;
    }

    PlayerColor color = static_cast<PlayerColor>(colorValue.toInt());
    QString const& username = usernameValue.toString();

    onParsedPlayerLeftMessage(color, username);
    return true;
}

bool NetworkBase::parseChatMessage(QJsonObject const& message)
{
    QJsonValue const colorValue = message.value(toString(MessageParam::COLOR));
    if (colorValue.isNull())
    {
        return false;
    }

    QJsonValue const usernameValue = message.value(toString(MessageParam::USERNAME));
    if (usernameValue.isNull() || !usernameValue.isString())
    {
        return false;
    }

    QJsonValue const bodyValue = message.value(toString(MessageParam::CHAT_BODY));
    if (bodyValue.isNull() || !bodyValue.isString())
    {
        return false;
    }

    PlayerColor color = static_cast<PlayerColor>(colorValue.toInt());
    QString const& username = usernameValue.toString();
    QString body = bodyValue.toString();

    onParsedChatMessage(color, username, body);
    return true;
}

bool NetworkBase::parseMessage(QAbstractSocket* socket, QJsonObject const& message)
{
    QJsonValue typeValue = message.value(toString(MessageParam::MESSAGE_TYPE));
    if (typeValue.isNull())
    {
        return false;
    }

    MessageType type;
    if (tryParse(typeValue.toString(), type))
    {
        switch (type)
        {
        case MessageType::JOIN_REQUEST:
            return parseJoinRequest(socket, message);
        case MessageType::JOIN_RESPONSE:
            return parseJoinResponse(message);
        case MessageType::POSITION_UPDATE:
            return parsePositionMessage(message);
        case MessageType::BULLET_SHOT:
            return parseBulletMessage(message);
        case MessageType::HEALTH_UPDATE:
            return parseHealthMessage(message);
        case MessageType::GAME_START:
            return parseGameStartMessage(message);
        case MessageType::GAME_END:
            return parseGameEndMessage(message);
        case MessageType::PLAYER_JOINED:
            return parsePlayerJoinedMessage(message);
        case MessageType::PLAYER_LEFT:
            return parsePlayerLeftMessage(message);
        case MessageType::CHAT_MESSAGE:
            return parseChatMessage(message);
        }
    }

    return false;
}

void NetworkBase::onReadyRead(QAbstractSocket* socket)
{
    receivedFrom(socket);

    QByteArray jsonData;

    QDataStream in(socket);
    in.setVersion(COMPRESSION_VERSION);

    while (true)
    {
        // Start a transaction that may be rolled back if a full message has
        //  not yet been received

        in.startTransaction();
        in >> jsonData;

        if (in.commitTransaction())
        {
            QJsonParseError parseError;
            QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData, &parseError);

            if (parseError.error == QJsonParseError::NoError && jsonDoc.isObject())
            {
                parseMessage(socket, jsonDoc.object());
            }
        }
        else
        {
            break;
        }
    }
}

QString NetworkBase::toString(MessageType messageType)
{
    switch (messageType)
    {
    case MessageType::JOIN_REQUEST:
        return QStringLiteral("join_request");
    case MessageType::JOIN_RESPONSE:
        return QStringLiteral("join_response");
    case MessageType::POSITION_UPDATE:
        return QStringLiteral("position_message");
    case MessageType::BULLET_SHOT:
        return QStringLiteral("bullet_message");
    case MessageType::HEALTH_UPDATE:
        return QStringLiteral("health_message");
    case MessageType::GAME_START:
        return QStringLiteral("game_start");
    case MessageType::GAME_END:
        return QStringLiteral("game_end");
    case MessageType::PLAYER_JOINED:
        return QStringLiteral("player_joined");
    case MessageType::PLAYER_LEFT:
        return QStringLiteral("player_left");
    case MessageType::CHAT_MESSAGE:
        return QStringLiteral("chat_message");
    }

    return QStringLiteral("INVALID");
}

QString NetworkBase::toString(MessageParam messageParam)
{
    switch (messageParam)
    {
    case MessageParam::MESSAGE_TYPE:
        return QStringLiteral("message_type");
    case MessageParam::USERNAME:
        return QStringLiteral("username");
    case MessageParam::COLOR:
        return QStringLiteral("color");
    case MessageParam::POSITION_X:
        return QStringLiteral("x");
    case MessageParam::POSITION_Y:
        return QStringLiteral("y");
    case MessageParam::BULLET_ANGLE:
        return QStringLiteral("bullet_angle");
    case MessageParam::HEALTH:
        return QStringLiteral("health");
    case MessageParam::CROWN:
        return QStringLiteral("crown");
    case MessageParam::GAME_TIME:
        return QStringLiteral("game_time");
    case MessageParam::JOIN_SUCCEEDED:
        return QStringLiteral("join_succeeded");
    case MessageParam::JOIN_ERROR:
        return QStringLiteral("join_error");
    case MessageParam::CHAT_BODY:
        return QStringLiteral("chat_body");
    }

    return QStringLiteral("INVALID");
}

QString NetworkBase::toString(JoinError joinError)
{
    QString string = QStringLiteral("");

    if (joinError == JoinError::NO_ERROR)
    {
        string += QStringLiteral("[no_error]");
        return string;
    }

    if (joinError & JoinError::INVALID_USERNAME)
    {
        string += QStringLiteral("[invalid_username]");
    }

    if (joinError & JoinError::INVALID_COLOR)
    {
        string += QStringLiteral("[invalid_color]");
    }

    if (joinError & JoinError::GAME_FULL)
    {
        string += QStringLiteral("[game_full]");
    }

    if (joinError & JoinError::USERNAME_TAKEN)
    {
        string += QStringLiteral("[username_taken]");
    }

    if (joinError & JoinError::COLOR_TAKEN)
    {
        string += QStringLiteral("[color_taken]");
    }

    // If error is in invalid format, assume invalid username and color
    if (string == QStringLiteral(""))
    {
        return QStringLiteral("[invalid_username][invalid_color]");
    }

    return string;
}

bool NetworkBase::tryParse(QString const& string, JoinError& result)
{
    int temp = JoinError::NO_ERROR;

    if (string == QStringLiteral("[no_error]"))
    {
        result = JoinError::NO_ERROR;
        return true;
    }

    bool parsed = false;

    if (string.contains(QStringLiteral("[invalid_username]")))
    {
        temp |= JoinError::INVALID_USERNAME;
        parsed = true;
    }

    if (string.contains(QStringLiteral("[invalid_color]")))
    {
        temp |= JoinError::INVALID_COLOR;
        parsed = true;
    }

    if (string.contains(QStringLiteral("[game_full]")))
    {
        temp |= JoinError::GAME_FULL;
        parsed = true;
    }

    if (string.contains(QStringLiteral("[username_taken]")))
    {
        temp |= JoinError::USERNAME_TAKEN;
        parsed = true;
    }

    if (string.contains(QStringLiteral("[color_taken]")))
    {
        temp |= JoinError::COLOR_TAKEN;
        parsed = true;
    }

    result = static_cast<JoinError>(temp);
    return parsed;
}

bool NetworkBase::tryParse(QString const& string, MessageType& result)
{
    static QMap<QString, MessageType> messageTypes
    {
        { QStringLiteral("join_request"), MessageType::JOIN_REQUEST },
        { QStringLiteral("join_response"), MessageType::JOIN_RESPONSE },
        { QStringLiteral("position_message"), MessageType::POSITION_UPDATE },
        { QStringLiteral("bullet_message"), MessageType::BULLET_SHOT },
        { QStringLiteral("health_message"), MessageType::HEALTH_UPDATE },
        { QStringLiteral("game_start"), MessageType::GAME_START },
        { QStringLiteral("game_end"), MessageType::GAME_END },
        { QStringLiteral("player_joined"), MessageType::PLAYER_JOINED },
        { QStringLiteral("player_left"), MessageType::PLAYER_LEFT },
        { QStringLiteral("chat_message"), MessageType::CHAT_MESSAGE },
    };

    if (messageTypes.contains(string))
    {
        result = messageTypes[string];
        return true;
    }

    return false;
}

bool NetworkBase::tryParse(QString const& string, MessageParam& result)
{
    static QMap<QString, MessageParam> messageParams
    {
        { QStringLiteral("message_type"), MessageParam::MESSAGE_TYPE },
        { QStringLiteral("username"), MessageParam::USERNAME },
        { QStringLiteral("color"), MessageParam::COLOR },
        { QStringLiteral("x"), MessageParam::POSITION_X },
        { QStringLiteral("y"), MessageParam::POSITION_Y },
        { QStringLiteral("bullet_angle"), MessageParam::BULLET_ANGLE },
        { QStringLiteral("health"), MessageParam::HEALTH },
        { QStringLiteral("crown"), MessageParam::CROWN },
        { QStringLiteral("game_time"), MessageParam::GAME_TIME },
        { QStringLiteral("join_succeeded"), MessageParam::JOIN_SUCCEEDED },
        { QStringLiteral("join_error"), MessageParam::JOIN_ERROR },
        { QStringLiteral("chat_body"), MessageParam::CHAT_BODY },
    };

    if (messageParams.contains(string))
    {
        result = messageParams[string];
        return true;
    }

    return false;
}

// These methods are left empty so that any number of them may be overridden by a base
//  class host or client, but none have to be overridden (as they would if they were pure virtual).
void NetworkBase::onParsedJoinRequest(QAbstractSocket*, PlayerColor, QString const&) { }
void NetworkBase::onParsedJoinResponse(bool, PlayerColor, QString const&, JoinError) { }
void NetworkBase::onParsedPositionMessage(PlayerColor, QPointF) { }
void NetworkBase::onParsedBulletMessage(PlayerColor, QPointF, qreal) { }
void NetworkBase::onParsedHealthMessage(PlayerColor, int, bool) { }
void NetworkBase::onParsedGameStartMessage(int) { }
void NetworkBase::onParsedGameEndMessage(PlayerColor, QString const&) { }
void NetworkBase::onParsedPlayerJoinedMessage(PlayerColor, QString const&) { }
void NetworkBase::onParsedPlayerLeftMessage(PlayerColor, QString const&) { }
void NetworkBase::onParsedChatMessage(PlayerColor, QString const&, QString const&) { }
