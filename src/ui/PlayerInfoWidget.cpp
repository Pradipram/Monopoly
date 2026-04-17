#include "PlayerInfoWidget.h"

PlayerInfoWidget::PlayerInfoWidget(int playerId, QColor playerColor, QString playerName,
                                   QWidget* parent)
    : QWidget(parent)
{
    // Make this widget look like a nice card
    this->setAttribute(Qt::WA_StyledBackground, true);
    this->setObjectName("PlayerCard");
    this->setStyleSheet(
        "#PlayerCard {"
        "  background-color: white;"
        "  border-radius: 8px;"
        "  border-left: 10px solid " +
        playerColor.name() +
        ";"  // Left stripe with player's color
        "}");

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(15, 10, 10, 10);  // Left, Top, Right, Bottom margins

    // Header: Name and ID
    m_nameLabel =
        new QLabel("<b>" + playerName + "</b> (Player " + QString::number(playerId + 1) + ")");
    m_nameLabel->setFont(QFont("Arial", 12));

    // Stats: Cash and Properties
    m_cashLabel = new QLabel("Cash: $3000");
    m_cashLabel->setStyleSheet("color: green; font-weight: bold; font-size: 14px;");

    m_propertiesLabel = new QLabel("Properties Owned: 0");

    mainLayout->addWidget(m_nameLabel);
    mainLayout->addWidget(m_cashLabel);
    mainLayout->addWidget(m_propertiesLabel);
}

void PlayerInfoWidget::updateCash(int newAmount)
{
    m_cashLabel->setText("Cash: $" + QString::number(newAmount));
}

void PlayerInfoWidget::updatePropertiesInfo(int propertyCount)
{
    m_propertiesLabel->setText("Properties Owned: " + QString::number(propertyCount));
}