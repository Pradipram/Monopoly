#ifndef BOARDWIDGET_H
#define BOARDWIDGET_H

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

#include "GameEngine.h"
#include "GameLogicHandler.h"
#include "PlayerInfoWidget.h"
#include "PlayerTokenItem.h"
#include "SpaceItem.h"

class BoardWidget : public QWidget
{
    Q_OBJECT
   public:
    explicit BoardWidget(int botCount, QWidget* parent = nullptr);
    ~BoardWidget();
    void updateActionPanel();
    void animateTokenSteps(int playerId, int currentSpace, int stepsRemaining, int dice_outcome);
   signals:

   public slots:

   protected:
   private:
    int m_botCount;
    GameEngine* m_gameEngine;
    GameLogicHandler* m_gameLogicHandler;
    QMap<int, SpaceItem*> m_tileUIMap;
    QList<PlayerTokenItem*> m_playerTokens;
    QList<PlayerInfoWidget*> m_playerCards;
    QVBoxLayout* m_playerListLayout = nullptr;

    QLabel* m_turnStatusLabel;
    // QHBoxLayout* m_actionButtonsLayout;

    QLabel* m_dieOneLabel;
    QLabel* m_dieTwoLabel;
    QPushButton* m_rollButton;

    void init_UI();
    void init_board_UI();
    void init_info_UI();
    void init_players_UI(QGraphicsScene* scene);
    void action_layout(QVBoxLayout* layout, QWidget* parent = nullptr);

    QFrame* createDicePanel(QWidget* parent = nullptr);

    void updateTokens();
    void updatePlayerInfo();

   public slots:
    void rollDice();
};
#endif  // BOARDWIDGET_H
