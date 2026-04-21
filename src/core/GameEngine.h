#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include <QList>

#include "Player.h"
#include "Space.h"

class GameEngine
{
   public:
    GameEngine(int localPlayerCount, int botCount);
    ~GameEngine();

    const QList<Space*>& getSpacesList();
    int m_currentPlayerTurn;

    int m_localPlayerCount;
    int m_botCount;
    QVector<Player*> m_players;
    QList<Space*> m_spaces;

    void loadBoardData();
    void advanceTurn(int steps);

    void buyLandedProperty();
    void changePlayerTurn();
    void payRent();
    void increasePlayerCash(int amount);

    void updatePlayerNetWorth();
    void updatePlayerNetWorth(Player* player);
};

#endif  // GAMEENGINE_H