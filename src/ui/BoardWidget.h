#ifndef BOARDWIDGET_H
#define BOARDWIDGET_H

#include <QWidget>

#include "GameEngine.h"
#include "PlayerTokenItem.h"
#include "SpaceItem.h"

class QVBoxLayout;

class BoardWidget : public QWidget
{
    Q_OBJECT
   public:
    explicit BoardWidget(int botCount, QWidget* parent = nullptr);

   signals:

   public slots:

   protected:
   private:
    int m_botCount;
    GameEngine* m_gameEngine;
    QMap<int, SpaceItem*> m_tileUIMap;
    QList<PlayerTokenItem*> m_playerTokens;

    void init_UI();
    void init_board_UI();
    void init_info_UI();
    void init_players_UI(QGraphicsScene* scene);
    void action_layout(QVBoxLayout* layout, QWidget* parent = nullptr);
};
#endif  // BOARDWIDGET_H
