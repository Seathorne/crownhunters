#include "healthitem.h"

#include <QTimer>

HealthItem::HealthItem(qreal xPos, qreal yPos)
{
    x = xPos;  // passed x position set
    y = yPos;  // passed y position set
    w = 25;  // width of item
    h = 25;  // height of item
    index = 0;  // index for the animation images

    images[0] = QImage(":/images/health-1.png");
    images[1] = QImage(":/images/health-1.png");
    images[2] = QImage(":/images/health-1.png");
    images[3] = QImage(":/images/health-1.png");
    images[4] = QImage(":/images/health-1.png");
    images[5] = QImage(":/images/health-1.png");
    images[6] = QImage(":/images/health-1.png");
    images[7] = QImage(":/images/health-1.png");
    images[8] = QImage(":/images/health-1.png");
    images[9] = QImage(":/images/health-1.png");
    images[10] = QImage(":/images/health-2.png");
    images[11] = QImage(":/images/health-3.png");
    images[12] = QImage(":/images/health-4.png");
    images[13] = QImage(":/images/health-5.png");
    images[14] = QImage(":/images/health-6.png");

    // set position relative to scene
    setRect(x,y,w,h);
    this->mapToScene(0,0);

    // create a timer for redrawing the animation images
    QTimer* rateTimer = new QTimer;
    connect(rateTimer, SIGNAL(timeout()), this, SLOT(updateImage()));
    rateTimer->setInterval(80);
    rateTimer->start();
}


void HealthItem::updateImage()
{
    index = (index + 1) % 15;
    healthBrush.setTextureImage(images[index]);
    this->setPen(Qt::NoPen);
    this->setBrush(healthBrush);
}
