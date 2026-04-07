#include "GameEngine.h"
#include <QRandomGenerator>

GameEngine::GameEngine(QObject *parent)
    : QObject(parent), m_currentPlayerIndex(0)
{
    createBoard();
}

void GameEngine::initializeGame(int humanPlayers, int botPlayers) {
    m_players.clear();
    for (int i = 0; i < humanPlayers; ++i) {
        m_players.push_back(std::make_unique<Player>(QString("Player %1").arg(i + 1), false));
    }
    for (int i = 0; i < botPlayers; ++i) {
        m_players.push_back(std::make_unique<Player>(QString("Bot %1").arg(i + 1), true));
    }
    m_currentPlayerIndex = 0;
    emit messageCreated("Game initialized.");
    emit gameStateChanged();
}

void GameEngine::createBoard() {
    m_board.clear();
    // Simplified board setup for now
    m_board.push_back(std::make_unique<Space>("GO", SpaceType::Go));
    m_board.push_back(std::make_unique<Space>("Mediterranean Avenue", SpaceType::Property, 60, 2));
    m_board.push_back(std::make_unique<Space>("Community Chest", SpaceType::CommunityChest));
    m_board.push_back(std::make_unique<Space>("Baltic Avenue", SpaceType::Property, 60, 4));
    m_board.push_back(std::make_unique<Space>("Income Tax", SpaceType::Tax));
    m_board.push_back(std::make_unique<Space>("Reading Railroad", SpaceType::Railroad, 200, 25));
    // Pad the rest of the board out for 40 total spaces...
    for (int i = 6; i < 40; ++i) {
         m_board.push_back(std::make_unique<Space>(QString("Space %1").arg(i), SpaceType::Property, 100, 10));
    }
}

Player* GameEngine::getCurrentPlayer() const {
    if (m_players.empty()) return nullptr;
    return m_players[m_currentPlayerIndex].get();
}

const std::vector<std::unique_ptr<Player>>& GameEngine::getPlayers() const {
    return m_players;
}

const std::vector<std::unique_ptr<Space>>& GameEngine::getBoard() const {
    return m_board;
}

void GameEngine::rollDiceAndMove() {
    if (m_players.empty()) return;

    int d1 = QRandomGenerator::global()->bounded(1, 7);
    int d2 = QRandomGenerator::global()->bounded(1, 7);
    int total = d1 + d2;

    Player* p = getCurrentPlayer();
    
    emit messageCreated(QString("%1 rolled a %2 and a %3 (Total: %4)").arg(p->getName()).arg(d1).arg(d2).arg(total));

    int oldPosition = p->getPosition();
    p->move(total, static_cast<int>(m_board.size()));
    int newPosition = p->getPosition();

    // Check passing GO
    if (newPosition < oldPosition) {
        p->addBalance(200);
        emit messageCreated(QString("%1 passed GO and collected $200!").arg(p->getName()));
    }

    emit messageCreated(QString("%1 landed on %2").arg(p->getName()).arg(m_board[newPosition]->getName()));
    
    handleSpaceLanded(p, m_board[newPosition].get());

    emit gameStateChanged();
    
    // Simplification: We ignore doubles for now (no second turn, no going to jail for 3 doubles)
}

void GameEngine::handleSpaceLanded(Player* player, Space* space) {
    if (space->getType() == SpaceType::Property || space->getType() == SpaceType::Railroad) {
        if (space->getOwner() == nullptr) {
            // Simplified: Unowned property handling.
            if (player->getIsBot()) {
                // Basic Bot AI: Buy if they have the cash AND leave themselves a $150 buffer
                if (player->getBalance() >= (space->getPrice() + 150)) {
                    player->payAmount(space->getPrice());
                    space->setOwner(player);
                    emit messageCreated(QString("%1 bought %2 for $%3!").arg(player->getName()).arg(space->getName()).arg(space->getPrice()));
                } else {
                    emit messageCreated(QString("%1 declined to buy %2 to save money.").arg(player->getName()).arg(space->getName()));
                }
            } else {
                emit messageCreated(QString("Prompt: Would you like to buy %1 for $%2? (Implementation pending)").arg(space->getName()).arg(space->getPrice()));
            }
        } else if (space->getOwner() != player) {
            int rent = space->getRent();
            if (player->payAmount(rent)) {
                space->getOwner()->addBalance(rent);
                emit messageCreated(QString("%1 paid $%2 rent to %3.").arg(player->getName()).arg(rent).arg(space->getOwner()->getName()));
            } else {
                emit messageCreated(QString("%1 cannot afford the rent and is Bankrupt!").arg(player->getName()));
                // Further logic to handle removing player from the game is needed later.
            }
        }
    } else if (space->getType() == SpaceType::Tax) {
        // Simplified Tax logic ($100 flat rate for now)
        int taxAmount = 100;
        if (player->payAmount(taxAmount)) {
            emit messageCreated(QString("%1 paid $%2 in taxes.").arg(player->getName()).arg(taxAmount));
        } else {
            emit messageCreated(QString("%1 cannot afford taxes and is Bankrupt!").arg(player->getName()));
        }
    }
}

void GameEngine::endTurn() {
    if (m_players.empty()) return;

    m_currentPlayerIndex = (m_currentPlayerIndex + 1) % m_players.size();
    emit messageCreated(QString("It is now %1's turn.").arg(getCurrentPlayer()->getName()));
    emit gameStateChanged();
}