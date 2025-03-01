#ifndef RESPAWNOVERLAYITEM_H
#define RESPAWNOVERLAYITEM_H

#include <QGraphicsRectItem>
#include <QPainter>
#include <QObject>

/*!
 * \brief RespawnOverlayItem class controls the display of a respawn counter when the player dies
 */
class RespawnOverlayItem: public QObject, public QGraphicsRectItem
{
    Q_OBJECT
public:
    explicit RespawnOverlayItem();
    /*!
     * \brief Controls the speed at which images appear on the screen
     */
    QTimer* rateTimer;
    /*!
     * \brief Current index of image array
     */
    int respawnindex;
    /*!
     * \brief Resets the index for the image array
     */
    void ResetIndex();

public slots:
    /*!
     * \brief Finds the next index of image to use and displays it on the screen
     */
    void updateImage();

private:
    QBrush imageBrush;
    /*!
     * \brief Array of images to be displayed
     */
    QImage images[6];
    qreal x;
    qreal y;
    qreal w;
    qreal h;   
};

#endif // RESPAWNOVERLAYITEM_H
