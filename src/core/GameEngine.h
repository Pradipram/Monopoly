#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include <QObject>
#include <vector>
#include <memory>
#include "Player.h"
#include "Space.h"

class GameEngine : public QObject {
    Q_OBJECT
public:
    explicit GameEngine(QObject *parent = nullptr);

    void initializeGame(int humanPlayers, int botPlayers);
    void rollDiceAndMove();
    void endTurn();

    Player* getCurrentPlayer() const;
    const std::vector<std::unique_ptr<Player>>& getPlayers() const;
    const std::vector<std::unique_ptr<Space>>& getBoard() const;

signals:
    void gameStateChanged();
    void messageCreated(const QString& message);

private:
    void createBoard();
    void handleSpaceLanded(Player* player, Space* space);

    std::vector<std::unique_ptr<Space>> m_board;
    std::vector<std::unique_ptr<Player>> m_players;
    int m_currentPlayerIndex;
};

#endif // GAMEENGINE_H