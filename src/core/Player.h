#ifndef PLAYER_H
#define PLAYER_H

#include <QString>

#include "Space.h"

class Player
{
   public:
    Player(int id, const QString& name, bool isBot = true);

    QString m_name;

    int m_id;
    bool m_isBot;
    int m_cash;
    int m_landedSpaceIndex;

    QVector<Space*> m_ownedSpaces;

   private:
};

#endif  // PLAYER_H