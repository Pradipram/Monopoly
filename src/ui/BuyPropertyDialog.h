#ifndef BUYPROPERTYDIALOG_H
#define BUYPROPERTYDIALOG_H

#include <QDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

#include "Player.h"
#include "Space.h"

class BuyPropertyDialog : public QDialog
{
    Q_OBJECT
   public:
    explicit BuyPropertyDialog(Player* player, Space* space, QWidget* parent = nullptr);
};

#endif  // BUYPROPERTYDIALOG_H