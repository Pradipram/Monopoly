#include "PayRentDialog.h"

#include <QTimer>

PayRentDialog::PayRentDialog(Player* player, Space* space, Player* owner, QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle("Rent Due!");
    setFixedSize(300, 180);
    setStyleSheet("QDialog { background-color: white; }");

    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // Note: You will need to calculate actual rent based on houses/monopoly later.
    // For now, let's assume space->price() / 10 as a placeholder for rent.
    int rentAmount = space->price() / 10;

    QLabel* infoLabel = new QLabel(QString("You landed on <b>%1</b>!<br>It is owned by <b>%2</b>.")
                                       .arg(space->name())
                                       .arg(owner->m_name));
    infoLabel->setAlignment(Qt::AlignCenter);

    QLabel* rentLabel = new QLabel(QString("You must pay: $%1").arg(rentAmount));
    rentLabel->setAlignment(Qt::AlignCenter);
    rentLabel->setStyleSheet("color: #a83232; font-size: 16px; font-weight: bold;");

    QPushButton* payBtn = new QPushButton("Pay Rent");
    payBtn->setCursor(Qt::PointingHandCursor);
    if (player->m_isBot) {
        payBtn->setEnabled(false);
        QTimer::singleShot(1000, this, [this]() { this->accept(); });
    }
    payBtn->setStyleSheet(
        "background-color: #a83232; color: white; padding: 8px; font-weight: bold; border-radius: "
        "4px;");

    mainLayout->addWidget(infoLabel);
    mainLayout->addWidget(rentLabel);
    mainLayout->addWidget(payBtn);

    connect(payBtn, &QPushButton::clicked, this, &QDialog::accept);
}