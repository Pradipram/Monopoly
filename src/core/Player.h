#ifndef PLAYER_H
#define PLAYER_H

#include <QColor>
#include <QList>
#include <QString>

#include "Space.h"

inline const QList<QColor> PlayerColor = {Qt::red,
                                          Qt::blue,
                                          Qt::green,
                                          Qt::yellow,
                                          Qt::cyan,
                                          Qt::magenta,
                                          QColor(255, 165, 0),
                                          QColor(128, 0, 128)};

class Player
{
   public:
    Player(int id, const QString& name, bool isBot = true);

    QString m_name;

    int m_id;
    bool m_isBot;
    int m_cash;
    int m_landedSpaceIndex;
    int m_netWorth;

    QVector<Space*> m_ownedSpaces;

   private:
};

#endif  // PLAYER_H