#include "SpaceItem.h"

#include <QDebug>
#include <QFont>
#include <QGraphicsSceneMouseEvent>

SpaceItem::SpaceItem(Space* spaceData, QGraphicsItem* parent)
    : QGraphicsItem(parent), m_spaceData(spaceData)
{
    // Update the sizes to fill the 1000x1000 board
    if (m_spaceData->index() % 10 == 0) {
        TILE_WIDTH = 140;
        TILE_HEIGHT = 140;
    } else {
        TILE_WIDTH = 80;
        TILE_HEIGHT = 140;
    }

    setupPositionAndRotation();
}

QRectF SpaceItem::boundingRect() const
{
    return QRectF(0, 0, TILE_WIDTH, TILE_HEIGHT);
}

void SpaceItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    // 1. Draw the tile background and border
    painter->setBrush(Qt::white);
    painter->setPen(QPen(Qt::black, 2));
    painter->drawRect(boundingRect());

    // 2. Draw the color header (Only for regular properties)
    if (m_spaceData->type() == SpaceType::Property) {
        QColor groupColor = Qt::gray;
        QString cGroup = m_spaceData->colorGroup();

        // Map your JSON strings to actual Qt colors
        if (cGroup == "Light Blue")
            groupColor = QColor(173, 216, 230);
        else if (cGroup == "Pink")
            groupColor = QColor(255, 105, 180);
        else if (cGroup == "Orange")
            groupColor = QColor(255, 165, 0);
        else if (cGroup == "Red")
            groupColor = QColor(255, 0, 0);
        else if (cGroup == "Yellow")
            groupColor = QColor(255, 255, 0);
        else if (cGroup == "Green")
            groupColor = QColor(0, 128, 0);
        else if (cGroup == "DarkBlue")
            groupColor = QColor(0, 0, 139);

        painter->setBrush(groupColor);
        painter->setPen(Qt::black);               // Keep a border around the color bar
        painter->drawRect(0, 0, TILE_WIDTH, 25);  // Top 25 pixels
    }

    // 3. Draw the Text
    painter->setPen(Qt::black);
    QFont font("Arial", 10, QFont::Bold);
    painter->setFont(font);

    // Give the text a slight margin so it doesn't hit the edges
    QRectF textRect(4, 28, TILE_WIDTH - 8, 45);

    // If it's a corner tile, push the text down a bit to center it
    if (m_spaceData->index() % 10 == 0) {
        textRect.setRect(4, 30, TILE_WIDTH - 8, 60);
        painter->save();
        painter->translate(TILE_WIDTH / 2.0, TILE_HEIGHT / 2.0);

        int index = m_spaceData->index();
        if (index == 0) {
            painter->rotate(45);
        } else if (index == 10) {
            painter->rotate(135);
        } else if (index == 20) {
            painter->rotate(225);  // assuming 125 was a typo for 225
        } else if (index == 30) {
            painter->rotate(315);
        }

        QRectF rotatedTextRect(-textRect.width() / 2.0, -textRect.height() / 2.0, textRect.width(),
                               textRect.height());
        painter->drawText(rotatedTextRect, Qt::AlignCenter | Qt::TextWordWrap, m_spaceData->name());
        painter->restore();
    } else {
        painter->drawText(textRect, Qt::AlignCenter | Qt::TextWordWrap, m_spaceData->name());
    }

    // 4. Draw the Price (if it has one)
    if (m_spaceData->price() > 0) {
        QString priceText = QString("$%1").arg(m_spaceData->price());
        // Draw the price near the bottom edge
        painter->drawText(0, TILE_HEIGHT - 18, TILE_WIDTH, 15, Qt::AlignCenter, priceText);
    }
}

void SpaceItem::setupPositionAndRotation()
{
    int index = m_spaceData->index();

    // Set the pivot point to the exact center of whatever size the tile is
    setTransformOriginPoint(TILE_WIDTH / 2.0, TILE_HEIGHT / 2.0);

    // Apply rotation based on which side of the board it lives on
    if (index > 0 && index < 10) {
        setRotation(0);  // Bottom row faces up
    } else if (index > 10 && index < 20) {
        setRotation(90);  // Left row faces right
    } else if (index > 20 && index < 30) {
        setRotation(180);  // Top row faces down
    } else if (index > 30 && index < 40) {
        setRotation(270);  // Right row faces left
    }
}

void SpaceItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    qDebug() << "You clicked on:" << m_spaceData->name();
    event->accept();
}