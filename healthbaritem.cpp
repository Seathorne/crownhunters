#include "healthbaritem.h"


HealthbarItem::HealthbarItem():
    QGraphicsRectItem ()
{
    // set the pen, brush and size of the healthbar outline
    this->setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::FlatCap, Qt::RoundJoin));
    this->setBrush(QBrush(Qt::red));

    this->setRect(x, y, width, height);

    // set the green visible health of the healthbar
    visibleHealth = new QGraphicsRectItem();
    visibleHealth->setBrush(QBrush(Qt::green));
    visibleHealth->setRect(x,y,vhWidth,vhHeight);
}

HealthbarItem::~HealthbarItem()
{
    delete this;
}

void HealthbarItem::advance(int phase)
{
    if (phase == 0){
        return;
    }
    // constantly set size of visible health so that it updates
    // when a player loses health
    visibleHealth->setRect(x,y,vhWidth, vhHeight);
}



