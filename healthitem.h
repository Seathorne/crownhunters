#ifndef HEALTHITEM_H
#define HEALTHITEM_H

#include <QGraphicsRectItem>
#include <QPainter>
#include <QObject>


/*!
 * \brief HealthItem class is a collectable item that regains player health.
 */
class HealthItem : public QObject, public QGraphicsRectItem
{
    Q_OBJECT
public:
    /*!
     * \brief Constructs a health item.
     */
    explicit HealthItem(qreal xPos, qreal yPos);

public slots:
    /*!
     * \brief Slot that updates on a timer to update the image and
     * animate the health item.
     */
    void updateImage();

private:
    /*!
     * \brief The brush used to draw the health item.
     */
    QBrush healthBrush;

    /*!
     * \brief An array of images used to animate the health item.
     */
    QImage images[15];

    /*!
     * \brief Starting x position to map to scene.
     */
    qreal x;

    /*!
     * \brief Starting y position to map to scene.
     */
    qreal y;

    /*!
     * \brief Width of the health item.
     */
    qreal w;

    /*!
     * \brief Height of the health item.
     */
    qreal h;

    /*!
     * \brief Index used to swap images.
     */
    int index;
};

#endif // HEALTHITEM_H
