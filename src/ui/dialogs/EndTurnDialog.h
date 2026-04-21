#ifndef ENDTURNDIALOG_H
#define ENDTURNDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QMap>
#include <QPushButton>
#include <QVBoxLayout>

#include "GameEngine.h"
#include "Player.h"

class EndTurnDialog : public QDialog
{
    Q_OBJECT
   public:
    // Custom return values for the dialog
    enum Action { EndTurn = 1, Trade = 2, Build = 3 };

    explicit EndTurnDialog(Player* player, GameEngine* engine, QWidget* parent = nullptr);
};

#endif  // ENDTURNDIALOG_H