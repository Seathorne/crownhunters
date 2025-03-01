#ifndef CROWNITEM_H
#define CROWNITEM_H

#include "playeritem.h"

#include <QtDebug>
#include <QGraphicsItem>
#include <QObject>
#include <QGraphicsRectItem>
#include <QBrush>
/*!
 * \brief CrownItem class is a collectable item the players can pick up
 */
class CrownItem : public QObject, public QGraphicsRectItem
{
    Q_OBJECT
public:
    /*!
     * \brief Constructor for CrownItem class
     * \param pointer to parent graphicsitem
     */
    CrownItem(QGraphicsItem * parent = nullptr);
    /*!
     * \brief Variable for the width of the crown
     */
    int width = 25;
    /*!
     * \brief Variable for the height of the crown
     */
    int height = 25;

protected:
    void advance(int phase);

private:
    /*!
     * \brief Function to determine collision with player
     * \return True or false of player collision
     */
    bool collidesWithPlayer();
};

#endif // CROWNITEM_H
