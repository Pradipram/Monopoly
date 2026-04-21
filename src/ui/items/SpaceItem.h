#ifndef SPACEITEM_H
#define SPACEITEM_H

#include <QGraphicsItem>
#include <QPainter>

#include "Space.h"  // Your logic class

class SpaceItem : public QGraphicsItem
{
   public:
    explicit SpaceItem(Space* spaceData, QGraphicsItem* parent = nullptr);
    void setShowOwnerIndicator(bool showOwnerIndicator);
    void setShowInvertedText(bool showInvertedText);

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

    void drawIcon(QPainter* painter, QPixmap* iconToDraw);
    void drawInvertedTextwithPrice(QPainter* painter);
    void drawInvertedText(QPainter* painter);

    // Helper functions for painting different spaces
    void drawGoSpace(QPainter* painter);
    void drawPropertySpace(QPainter* painter, const QFont& font);
    void drawPricedIconSpace(QPainter* painter, const QFont& font);
    void drawIconSpace(QPainter* painter, const QFont& font);
    void drawCornerIconSpace(QPainter* painter, const QFont& font, const QString& imagePath);

    void drawOwnerIndicator(QPainter* painter);

    bool m_showOwnerIndicator;
    bool m_showInvertedText;
};

#endif  // SPACEITEM_H