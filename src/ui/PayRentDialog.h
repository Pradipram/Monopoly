#ifndef PAYRENTDIALOG_H
#define PAYRENTDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

#include "Player.h"
#include "Space.h"

class PayRentDialog : public QDialog
{
    Q_OBJECT
   public:
    explicit PayRentDialog(Player* player, Space* space, Player* owner, QWidget* parent = nullptr);
};

#endif  // PAYRENTDIALOG_H