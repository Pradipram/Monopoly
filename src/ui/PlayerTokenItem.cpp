#include "PlayerTokenItem.h"

// 1. Removed "= nullptr" (Default arguments only go in the .h file!)
PlayerTokenItem::PlayerTokenItem(int playerID, QGraphicsItem* parent)
    // 2. Changed to QGraphicsEllipseItem and gave it a 24x24 pixel size
    : QGraphicsEllipseItem(0, 0, 24, 24, parent), m_playerID(playerID)
{
    QColor playerColor;
    switch (playerID) {
        case 0:
            playerColor = Qt::red;
            break;
        case 1:
            playerColor = Qt::blue;
            break;
        case 2:
            playerColor = Qt::green;
            break;
        case 3:
            playerColor = Qt::yellow;
            break;
        case 4:
            playerColor = Qt::cyan;
            break;
        case 5:
            playerColor = Qt::magenta;
            break;
        case 6:
            playerColor = QColor(255, 165, 0);  // Orange
            break;
        case 7:
            playerColor = QColor(128, 0, 128);  // Purple
            break;
        default:
            playerColor = Qt::black;
            break;
    }

    // Now IntelliSense knows exactly what a Brush and Pen are!
    setBrush(QBrush(playerColor));
    setPen(QPen(Qt::black, 2));
    setZValue(10);
}

int PlayerTokenItem::getPlayerID() const
{
    return m_playerID;
}