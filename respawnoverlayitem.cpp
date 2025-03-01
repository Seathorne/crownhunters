#include "respawnoverlayitem.h"

#include <QTimer>
#include <QDebug>

RespawnOverlayItem::RespawnOverlayItem()
{
    x = 0;
    y = 0;
    w = 1200;
    h = 600;
    respawnindex = -1;

    images[0] = QImage(":/images/respawnOverlay-1.png");
    images[1] = QImage(":/images/respawnOverlay-2.png");
    images[2] = QImage(":/images/respawnOverlay-3.png");
    images[3] = QImage(":/images/respawnOverlay-4.png");
    images[4] = QImage(":/images/respawnOverlay-5.png");
    images[5] = QImage(":/images/respawnOverlay-blank.png");

    setRect(x,y,w,h);
    this->mapToScene(0,0);

    // Set up and configure timer to display images every second
    rateTimer = new QTimer;
    connect(rateTimer, SIGNAL(timeout()), this, SLOT(updateImage()));
    rateTimer->setInterval(1000);
}

void RespawnOverlayItem::updateImage()
{
    respawnindex = (respawnindex + 1) % 6;  // The remainder of index+1 divided by 5 is the index of the image to use
    //qDebug()<<"the index is " << respawnindex;
    imageBrush.setTextureImage(images[respawnindex]);
    this->setPen(Qt::NoPen);
    this->setBrush(imageBrush);
}

void RespawnOverlayItem::ResetIndex()
{
    respawnindex = -1;  // Reset respawn index for proper iteration through image array
}
