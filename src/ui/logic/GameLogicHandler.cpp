#include "GameLogicHandler.h"

#include <QDialog>
#include <QEventLoop>
#include <QMessageBox>
#include <QTimer>
#include <QtGlobal>

#include "BoardWidget.h"
#include "BuyPropertyDialog.h"
#include "EndTurnDialog.h"
#include "GameEngine.h"
#include "PayRentDialog.h"
#include "Space.h"
#include "SpaceItem.h"
#include "ToastNotification.h"
#include "const.h"

GameLogicHandler::GameLogicHandler(GameEngine* gameEngine, QWidget* parentWidget)
    : m_gameEngine(gameEngine), m_parentWidget(parentWidget)
{
}

bool GameLogicHandler::handlePostMoveActions(const QMap<int, SpaceItem*>& tileUIMap,
                                             int dice_outcome)
{
    bool shouldChangeTurn = true;
    bool animationTriggered = false;

    // 1. Get the current state from your Game Engine
    int currPlayerIdx = m_gameEngine->m_currentPlayerTurn;
    Player* currPlayer = m_gameEngine->m_players[currPlayerIdx];

    // 2. Get the space the player is currently sitting on
    Space* landedSpace = m_gameEngine->m_spaces[currPlayer->m_landedSpaceIndex];

    switch (landedSpace->type()) {
        case SpaceConstants::SpaceType::Property:
            if (landedSpace->ownerId == Constants::UNOWNED) {
                BuyPropertyDialog dialog(currPlayer, landedSpace, m_parentWidget);
                if (dialog.exec() == QDialog::Accepted) {
                    m_gameEngine->buyLandedProperty();

                    // Force custom redraw of that tile specifically so new owner color appears!
                    SpaceItem* visualTile = tileUIMap.value(landedSpace->index());
                    if (visualTile) {
                        visualTile->update();
                    }
                }
            } else if (landedSpace->ownerId != currPlayer->m_id) {
                Player* owner = m_gameEngine->m_players[landedSpace->ownerId];
                PayRentDialog dialog(currPlayer, landedSpace, owner, m_parentWidget);
                dialog.exec();
                m_gameEngine->payRent();
            }
            break;

        case SpaceConstants::SpaceType::Chance: {
            animationTriggered = chanceLogic(dice_outcome);
            if (animationTriggered) {
                shouldChangeTurn = false;
            }
            break;
        }

        default:
            break;
    }

    if (shouldChangeTurn) {
        // bool turnEnded = false;
        if (landedSpace->type() == SpaceConstants::SpaceType::Chance ||
            landedSpace->type() == SpaceConstants::SpaceType::Go) {
            QEventLoop loop;
            QTimer::singleShot(ToastNotification::totalDurationMs(), &loop, &QEventLoop::quit);
            loop.exec();
        }

        EndTurnDialog actionMenu(currPlayer, m_gameEngine, m_parentWidget);
        int choice = actionMenu.exec();

        if (choice == EndTurnDialog::Trade) {
            // TODO: Trigger your custom Trade Dialog here later
            QMessageBox::information(m_parentWidget, "Trade", "Trade Dialog coming soon!");
        } else if (choice == EndTurnDialog::Build) {
            // TODO: Trigger your custom Build Dialog here later
            QMessageBox::information(m_parentWidget, "Build", "Build Dialog coming soon!");
        }
        m_gameEngine->changePlayerTurn();
    }
    return animationTriggered;
}

bool GameLogicHandler::chanceLogic(int dice_outcome)
{
    ToastNotification* toast =
        new ToastNotification(m_chanceOutcomes[dice_outcome],
                              PlayerColor[m_gameEngine->m_currentPlayerTurn], m_parentWidget);
    toast->showToast();
    switch (dice_outcome) {
        case 2:  // Advance to GO
        {
            Player* currPlayer = m_gameEngine->m_players[m_gameEngine->m_currentPlayerTurn];
            const int currentSpace = currPlayer->m_landedSpaceIndex;
            const int boardSize = m_gameEngine->m_spaces.size();
            const int stepsToGo =
                (Constants::START_SPACE_INDEX - currentSpace + boardSize) % boardSize;
            return triggerDelayedMovement(stepsToGo, dice_outcome, currPlayer, currentSpace);
        }
        case 3: {
            m_gameEngine->increasePlayerCash(200);
            return false;
        }
        case 4: {
            m_gameEngine->increasePlayerCash(50);
            return false;
        }
        case 5: {
            Player* currPlayer = m_gameEngine->m_players[m_gameEngine->m_currentPlayerTurn];
            int currentSpace = currPlayer->m_landedSpaceIndex;
            int stepsToMove =
                m_gameEngine->m_spaces.size();  // Start with max possible, then find nearest
            for (int idx : m_railroadIndices) {
                int delta =
                    (idx - currentSpace + m_gameEngine->m_spaces.size()) % m_gameEngine->m_spaces.size();
                if (delta == 0) {
                    delta = m_gameEngine->m_spaces.size();
                }
                stepsToMove = qMin(delta, stepsToMove);
            }
            qDebug() << "Chance card requires moving" << stepsToMove
                     << "steps to nearest railroad.";
            return triggerDelayedMovement(stepsToMove, dice_outcome, currPlayer, currentSpace);
        }
        case 6: {
            m_gameEngine->increasePlayerCash(-15);
            return false;
        }
        case 7: {
            m_gameEngine->increasePlayerCash(50);
            return false;
        }
        case 8: {
            Player* currPlayer = m_gameEngine->m_players[m_gameEngine->m_currentPlayerTurn];
            int houseCount = 0;
            for (Space* space : currPlayer->m_ownedSpaces) {
                if (space->type() == SpaceConstants::SpaceType::Property) {
                    houseCount += space->m_houseCount;
                }
            }
            m_gameEngine->increasePlayerCash(-25 * houseCount);
            return false;
        }
        case 9: {
            Player* currPlayer = m_gameEngine->m_players[m_gameEngine->m_currentPlayerTurn];
            int currentSpace = currPlayer->m_landedSpaceIndex;
            int stepsToMove =
                (m_waterCooperationIndex - currentSpace + m_gameEngine->m_spaces.size()) %
                m_gameEngine->m_spaces.size();
            qDebug() << "Chance card requires moving" << stepsToMove
                     << "steps to nearest Water Cooperation.";
            return triggerDelayedMovement(stepsToMove, dice_outcome, currPlayer, currentSpace);
        }
        case 10: {
            int totalPlayers = m_gameEngine->m_localPlayerCount + m_gameEngine->m_botCount;
            int currentPlayerIdx = m_gameEngine->m_currentPlayerTurn;
            for (int i = 0; i < totalPlayers; ++i) {
                if (i != currentPlayerIdx) {
                    m_gameEngine->increasePlayerCash(-50);
                }
            }
            return false;
        }
        case 11: {
            Player* currPlayer = m_gameEngine->m_players[m_gameEngine->m_currentPlayerTurn];
            int propertiesOwned = currPlayer->m_ownedSpaces.size();
            m_gameEngine->increasePlayerCash(25 * propertiesOwned);
            return false;
        }
        case 12: {
            Player* currPlayer = m_gameEngine->m_players[m_gameEngine->m_currentPlayerTurn];
            const int currentSpace = currPlayer->m_landedSpaceIndex;
            const int boardSize = m_gameEngine->m_spaces.size();
            const int jailIndex = m_jailIndex;
            const int stepsToGo = (jailIndex - currentSpace + boardSize) % boardSize;
            return triggerDelayedMovement(stepsToGo, dice_outcome, currPlayer, currentSpace);
        }
        default:
            return false;
    }
    return false;
}

bool GameLogicHandler::triggerDelayedMovement(int stepsToGo, int dice_outcome, Player* currPlayer,
                                              int currentSpace)
{
    BoardWidget* boardWidget = dynamic_cast<BoardWidget*>(m_parentWidget);
    if (boardWidget && stepsToGo > 0) {
        QTimer::singleShot(
            ToastNotification::totalDurationMs(), boardWidget,
            [this, boardWidget, currPlayer, currentSpace, stepsToGo, dice_outcome]() {
                m_gameEngine->advanceTurn(stepsToGo);
                boardWidget->animateTokenSteps(currPlayer->m_id, currentSpace, stepsToGo,
                                               dice_outcome);
            });
        return true;
    }

    return false;
}