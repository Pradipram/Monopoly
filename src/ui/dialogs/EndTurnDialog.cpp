#include "EndTurnDialog.h"

EndTurnDialog::EndTurnDialog(Player* player, GameEngine* engine, QWidget* parent) : QDialog(parent)
{
    setWindowTitle("End of Turn Actions");
    setFixedSize(300, 250);
    setStyleSheet("QDialog { background-color: white; }");

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(15);

    QLabel* titleLabel =
        new QLabel(QString("<b>%1</b>, your moves are done.<br>What would you like to do?")
                       .arg(player->m_name));
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setFont(QFont("Arial", 12));

    // --- CHECK FOR FULL COLOR SET (MONOPOLY) ---
    bool hasMonopoly = false;
    QMap<int, int> totalInGroup;
    QMap<int, int> playerOwnedInGroup;

    for (Space* space : engine->m_spaces) {
        if (space->type() == SpaceConstants::SpaceType::Property) {
            if (space->colorGroup() != SpaceConstants::SpaceColorGroup::None) {
                int cGroup = static_cast<int>(space->colorGroup());
                totalInGroup[cGroup]++;

                if (space->ownerId == player->m_id) {
                    playerOwnedInGroup[cGroup]++;
                }
            }
        }
    }

    // Compare total properties in each color group to the player's owned count
    for (int cGroup : totalInGroup.keys()) {
        if (totalInGroup[cGroup] > 0 && totalInGroup[cGroup] == playerOwnedInGroup[cGroup]) {
            hasMonopoly = true;
            break;  // They have at least one Monopoly, so unlock the Build button!
        }
    }

    // --- BUTTON CREATION ---
    QPushButton* tradeBtn = new QPushButton("Trade Properties");
    QPushButton* buildBtn = new QPushButton("Build Houses");
    QPushButton* endTurnBtn = new QPushButton("End Turn (Do Nothing)");

    // Styling
    tradeBtn->setStyleSheet(
        "background-color: #2b5c80; color: white; padding: 10px; font-weight: bold; border-radius: "
        "4px;");

    if (hasMonopoly) {
        buildBtn->setStyleSheet(
            "background-color: #2b803b; color: white; padding: 10px; font-weight: bold; "
            "border-radius: 4px;");
    } else {
        buildBtn->setStyleSheet(
            "background-color: #cccccc; color: #666666; padding: 10px; font-weight: bold; "
            "border-radius: 4px;");
        buildBtn->setEnabled(false);
        buildBtn->setToolTip("You must own a complete color set to build houses.");
    }

    endTurnBtn->setStyleSheet(
        "background-color: #a83232; color: white; padding: 10px; font-weight: bold; border-radius: "
        "4px;");

    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(tradeBtn);
    mainLayout->addWidget(buildBtn);
    mainLayout->addStretch();
    mainLayout->addWidget(endTurnBtn);

    // --- CONNECT BUTTONS TO CUSTOM RETURN CODES ---
    // Instead of accept/reject, we return our custom Enums!
    connect(tradeBtn, &QPushButton::clicked, this, [this]() { done(Trade); });
    connect(buildBtn, &QPushButton::clicked, this, [this]() { done(Build); });
    connect(endTurnBtn, &QPushButton::clicked, this, [this]() { done(EndTurn); });
}