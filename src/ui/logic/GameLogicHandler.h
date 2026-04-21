#ifndef GAMELOGICHANDLER_H
#define GAMELOGICHANDLER_H

#include <QWidget>

class GameEngine;
class Player;
class Space;
class SpaceItem;

/**
 * @brief Handles post-move game logic (property interactions, rent payments, etc.)
 * Separates game logic from UI rendering concerns.
 */
class GameLogicHandler : public QObject
{
    Q_OBJECT
   public:
    explicit GameLogicHandler(GameEngine* gameEngine, QWidget* parentWidget = nullptr);

    /**
     * @brief Processes all game logic after a player lands on a space
     * Handles property purchases, rent payments, and turn advancement
     * @param tileUIMap Map of space indices to their visual representations
     * @param dice_outcome The outcome of the dice roll
     */
    bool handlePostMoveActions(const QMap<int, SpaceItem*>& tileUIMap, int dice_outcome);

   private:
    GameEngine* m_gameEngine;
    QWidget* m_parentWidget;  // For dialog parent reference

    QVector<QString> m_chanceOutcomes = {
        "",
        "",
        "Advance to Go (Collect $200)",                                                   // Roll 2
        "Bank error in your favor. Collect $200",                                         // Roll 3
        "Bank pays you a dividend of $50.",                                               // Roll 4
        "Advance to the nearest Railroad.",                                               // Roll 5
        "Speeding fine! Pay $15",                                                         // Roll 6
        "Dividend Paid. Collect $50 from the Bank",                                       // Roll 7
        "Make general repairs on your properties (Pay $25 per house).",                   // Roll 8
        "Advance to the nearest Water Cooperation.",                                      // Roll 9
        "You have been elected Chairman of the Board. Pay each player $50.",              // Roll 10
        "Real Estate Boom! The Bank pays you $25 for every property you currently own.",  // Roll 11
        " Go directly to Jail. Do not pass GO.",                                          // Roll 12
    };

    QList<int> m_railroadIndices = {5, 15, 25, 35};
    int m_waterCooperationIndex = 28;
    int m_jailIndex = 10;

    bool chanceLogic(int dice_outcome);
    bool moveCurrentPlayerToJail(int dice_outcome);
    bool triggerDelayedMovement(int stepsToGo, int dice_outcome, Player* currPlayer,
                                int currentSpace);

   public slots:
    void showEndTurnMenu();
};

#endif  // GAMELOGICHANDLER_H
