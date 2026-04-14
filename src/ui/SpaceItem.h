#ifndef SPACEITEM_H
#define SPACEITEM_H

#include <QGraphicsItem>
#include <QPainter>

#include "Space.h"  // Your logic class

class SpaceItem : public QGraphicsItem
{
   public:
    explicit SpaceItem(Space* spaceData, QGraphicsItem* parent = nullptr);

    // Required by Qt to know the exact clickable/drawable bounds
    QRectF boundingRect() const override;

    // Required by Qt to actually draw the shapes and text
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

    // Sets up the pivot point and rotation based on the board side
    void setupPositionAndRotation();

   protected:
    // Captures mouse clicks
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;

   private:
    Space* m_spaceData;

    // These are no longer const so we can change them for corner tiles
    int TILE_WIDTH;
    int TILE_HEIGHT;
};

#endif  // SPACEITEM_H