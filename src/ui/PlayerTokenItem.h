#ifndef PLAYERTOKENITEM_H
#define PLAYERTOKENITEM_H

#include <QBrush>
#include <QGraphicsEllipseItem>  // <--- Must be EllipseItem
#include <QPen>

// Must inherit from QGraphicsEllipseItem
class PlayerTokenItem : public QGraphicsEllipseItem
{
   public:
    // The = nullptr belongs here!
    explicit PlayerTokenItem(int playerID, QGraphicsItem* parent = nullptr);

    int getPlayerID() const;

   private:
    int m_playerID;
};

#endif  // PLAYERTOKENITEM_H