#include "playercolor.h"
#include "qobject.h"

#include <QStringLiteral>

QDataStream& operator<<(QDataStream& ds, PlayerColor const value)
{
    return ds << static_cast<int>(value);
}

QDataStream& operator>>(QDataStream& ds, PlayerColor& value)
{
    int temp;
    ds >> temp;
    value = static_cast<PlayerColor>(temp);

    return ds;
}

QString const playerColorToString(PlayerColor value)
{
    switch (value)
    {
    case PlayerColor::Red:
        return QStringLiteral("Red");
    case PlayerColor::Blue:
        return QStringLiteral("Blue");
    case PlayerColor::Green:
        return QStringLiteral("Green");
    case PlayerColor::Magenta:
        return QStringLiteral("Magenta");
    case PlayerColor::White:
        return QStringLiteral("White");
    case PlayerColor::Black:
        return QStringLiteral("Black");
    case PlayerColor::Cyan:
        return QStringLiteral("Cyan");
    case PlayerColor::Gray:
        return QStringLiteral("Gray");
    }

    return QString();
}

PlayerColor parsePlayerColor(QString const& value)
{
    QString const& treated = value.trimmed().toLower();

    if (treated == QStringLiteral("blue"))
    {
        return PlayerColor::Blue;
    }

    if (treated == QStringLiteral("green"))
    {
        return PlayerColor::Green;
    }

    if (treated == QStringLiteral("magenta"))
    {
        return PlayerColor::Magenta;
    }

    if (treated == QStringLiteral("white"))
    {
        return PlayerColor::White;
    }

    if (treated == QStringLiteral("black"))
    {
        return PlayerColor::Black;
    }

    if (treated == QStringLiteral("cyan"))
    {
        return PlayerColor::Cyan;
    }

    if (treated == QStringLiteral("gray"))
    {
        return PlayerColor::Gray;
    }

    // Return red (value 0) by default
    return PlayerColor::Red;
}
