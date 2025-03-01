#ifndef HEALTHBARITEM_H
#define HEALTHBARITEM_H

#include <QPainter>
#include <QGraphicsItem>
#include <QGraphicsRectItem>
#include <QGraphicsScene>

/*!
 * \brief Healthbar item class that contains a red background to display damage
 *        as well as a green rectangle that shows actual amount of current health.
 */
class HealthbarItem: public QGraphicsRectItem
{
public:
    /*!
     * \brief Constructs a healthbar item.
     */
    HealthbarItem();

    /*!
     * \brief Deletes a healthbar item.
     */
    ~HealthbarItem() override;

    /*!
     * \brief Green rect item that displays the visible health on a healthbar.
     */
    QGraphicsRectItem* visibleHealth;

    /*!
     * \brief Max width in pixels of the healthbar
     */
    qreal vhMaxWidth = 50;

    /*!
     * \brief Width of visible health.
     */
    qreal vhWidth = 50;

    /*!
     * \brief Height of visible health.
     */
    qreal vhHeight = 5;

    /*!
     * \brief Starting x position to map to scene.
     */
    qreal x = 0;

    /*!
     * \brief Starting y position to map to scene.
     */
    qreal y = 0;

protected:
    /*!
     * \brief Advance method that draws the bullet moving.
     */
    void advance(int phase) override;

private:
    /*!
     * \brief Healthbar item width.
     */
    qreal width = 50;

    /*!
     * \brief Healthbar item height.
     */
    qreal height = 5;
};

#endif // HEALTHBARITEM_H
