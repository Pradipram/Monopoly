
#include "BuyPropertyDialog.h"

#include <QTimer>

BuyPropertyDialog::BuyPropertyDialog(Player* player, Space* space, QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle("Unowned Property");
    setFixedSize(300, 200);
    setStyleSheet("QDialog { background-color: white; }");

    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // 1. Title Label
    QLabel* titleLabel = new QLabel(
        QString("<b>%1</b> landed on <b>%2</b>!").arg(player->m_name).arg(space->name()));
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setFont(QFont("Arial", 12));

    // 2. Price Label
    QLabel* priceLabel = new QLabel(QString("Price: $%1").arg(space->price()));
    priceLabel->setAlignment(Qt::AlignCenter);
    priceLabel->setFont(QFont("Arial", 14, QFont::Bold));

    // 3. Balance Warning (If player is too poor)
    QLabel* balanceLabel = new QLabel(QString("Your Cash: $%1").arg(player->m_cash));
    balanceLabel->setAlignment(Qt::AlignCenter);

    // 4. Buttons
    QHBoxLayout* btnLayout = new QHBoxLayout();
    QPushButton* buyBtn = new QPushButton("Buy");
    QPushButton* passBtn = new QPushButton("Pass");

    buyBtn->setCursor(Qt::PointingHandCursor);
    passBtn->setCursor(Qt::PointingHandCursor);

    if (player->m_isBot) {
        buyBtn->setEnabled(false);
        passBtn->setEnabled(false);
        QTimer::singleShot(1000, this, [this]() { this->accept(); });
    }

    // Style buttons
    buyBtn->setStyleSheet(
        "QPushButton {"
        "  background-color: #2b5c40;"
        "  color: white;"
        "  padding: 8px;"
        "  font-weight: bold;"
        "  border-radius: 4px;"
        "}"
        "QPushButton:hover:enabled { background-color: #245136; }"
        "QPushButton:disabled { background-color: #b9c3bd; color: #eef2ef; }");
    passBtn->setStyleSheet(
        "QPushButton {"
        "  background-color: #a83232;"
        "  color: white;"
        "  padding: 8px;"
        "  font-weight: bold;"
        "  border-radius: 4px;"
        "}"
        "QPushButton:hover:enabled { background-color: #8e2a2a; }"
        "QPushButton:disabled { background-color: #cfb4b4; color: #f3ecec; }");

    // If they don't have enough money, disable the Buy button
    if (player->m_cash < space->price()) {
        buyBtn->setEnabled(false);
        balanceLabel->setStyleSheet("color: red; font-weight: bold;");  // Highlight they are broke
    }

    btnLayout->addWidget(buyBtn);
    btnLayout->addWidget(passBtn);

    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(priceLabel);
    mainLayout->addWidget(balanceLabel);
    mainLayout->addLayout(btnLayout);

    // 5. Connect buttons to QDialog's built-in accept/reject slots
    connect(buyBtn, &QPushButton::clicked, this, &QDialog::accept);
    connect(passBtn, &QPushButton::clicked, this, &QDialog::reject);
}