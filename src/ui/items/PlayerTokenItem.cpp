#include "PlayerTokenItem.h"

#include "Player.h"

// 1. Removed "= nullptr" (Default arguments only go in the .h file!)
PlayerTokenItem::PlayerTokenItem(int playerID, QGraphicsItem* parent)
    // 2. Changed to QGraphicsEllipseItem and gave it a 24x24 pixel size
    : QGraphicsEllipseItem(0, 0, 24, 24, parent), m_playerID(playerID)
{
    QColor playerColor = Qt::black;
    if (playerID >= 0 && !PlayerColor.isEmpty()) {
        playerColor = PlayerColor[playerID % PlayerColor.size()];
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