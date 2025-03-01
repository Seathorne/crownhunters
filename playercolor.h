#ifndef PLAYERCOLOR_H
#define PLAYERCOLOR_H

#include <QDataStream>

enum class PlayerColor
{
    Red,
    Blue,
    Green,
    Magenta,
    White,
    Black,
    Cyan,
    Gray,
};

QDataStream& operator<<(QDataStream& ds, PlayerColor const value);

QDataStream& operator>>(QDataStream& ds, PlayerColor& value);

QString const playerColorToString(PlayerColor value);

PlayerColor parsePlayerColor(QString const& value);

#endif // PLAYERCOLOR_H
