#ifndef GAMESTARTOVERLAYITEM_H
#define GAMESTARTOVERLAYITEM_H

#include <QGraphicsRectItem>
#include <QPainter>
#include <QObject>


class GameStartOverlayItem: public QObject,public QGraphicsRectItem
{
    Q_OBJECT
public:
    GameStartOverlayItem();

    QTimer* rateTimer;

    int gameStartIndex;

    void ResetIndex();

public slots:
    void updateImage();

private:
    QBrush imageBrush;

    QImage images[11];
    qreal x;
    qreal y;
    qreal w;
    qreal h;
};

#endif // GAMESTARTOVERLAYITEM_H
