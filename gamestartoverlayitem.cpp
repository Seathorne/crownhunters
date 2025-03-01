#include "gamestartoverlayitem.h"

#include <QTimer>
#include <QDebug>

GameStartOverlayItem::GameStartOverlayItem()
{
    x = 0;
    y = 0;
    w = 1200;
    h = 600;
    gameStartIndex = -1;

    //rateTimer->isSingleShot();

    images[0] = QImage(":/images/gameStartOverlay-10.png");
    images[1] = QImage(":/images/gameStartOverlay-9.png");
    images[2] = QImage(":/images/gameStartOverlay-8.png");
    images[3] = QImage(":/images/gameStartOverlay-7.png");
    images[4] = QImage(":/images/gameStartOverlay-6.png");
    images[5] = QImage(":/images/gameStartOverlay-5.png");
    images[6] = QImage(":/images/gameStartOverlay-4.png");
    images[7] = QImage(":/images/gameStartOverlay-3.png");
    images[8] = QImage(":/images/gameStartOverlay-2.png");
    images[9] = QImage(":/images/gameStartOverlay-1.png");
    images[10] = QImage(":/images/gameStartOverlay-blank.png");

    setRect(x,y,w,h);
    this->mapToScene(0,0);

    // Set up and configure timer to display images every second
    rateTimer = new QTimer;
    connect(rateTimer, SIGNAL(timeout()), this, SLOT(updateImage()));
    rateTimer->setInterval(1000);
}

void GameStartOverlayItem::updateImage()
{
    gameStartIndex = (gameStartIndex + 1) % 11;  // The remainder of index+1 divided by 11 is the index of the image to use
    imageBrush.setTextureImage(images[gameStartIndex]);
    this->setPen(Qt::NoPen);
    this->setBrush(imageBrush);

    //qDebug() << gameStartIndex;
}

void GameStartOverlayItem::ResetIndex()
{
    gameStartIndex = -1;
}
