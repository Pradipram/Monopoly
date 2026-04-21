#include "BoardWidget.h"

#include <QEasingCurve>
#include <QFrame>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsRectItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QHBoxLayout>
#include <QLabel>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QRandomGenerator>
#include <QScrollArea>
#include <QTimer>
#include <QVariantAnimation>
#include <algorithm>

#include "BoardUiUtils.h"
#include "PlayerInfoWidget.h"
#include "SpaceItem.h"
#include "ToastNotification.h"
#include "const.h"

QFrame* BoardWidget::createDicePanel(QWidget* parent)
{
    QFrame* dicePanel = new QFrame(parent);
    dicePanel->setObjectName("dicePanel");
    dicePanel->setStyleSheet(
        "#dicePanel {"
        "  background-color: rgb(255, 255, 255);"
        "  border: 2px solid rgb(66, 120, 87);"
        "  border-radius: 8px;"
        "}");

    QHBoxLayout* diceLayout = new QHBoxLayout(dicePanel);
    diceLayout->setContentsMargins(10, 8, 10, 8);
    diceLayout->setSpacing(10);

    m_dieOneLabel = new QLabel(dicePanel);
    m_dieTwoLabel = new QLabel(dicePanel);
    m_dieOneLabel->setAlignment(Qt::AlignCenter);
    m_dieTwoLabel->setAlignment(Qt::AlignCenter);
    m_dieOneLabel->setFixedSize(Constants::DICE_UI_SIZE, Constants::DICE_UI_SIZE);
    m_dieTwoLabel->setFixedSize(Constants::DICE_UI_SIZE, Constants::DICE_UI_SIZE);
    m_dieOneLabel->setStyleSheet(
        "background-color: transparent; border: none; margin: 0px; padding: 0px;");
    m_dieTwoLabel->setStyleSheet(
        "background-color: transparent; border: none; margin: 0px; padding: 0px;");

    m_rollButton = new QPushButton("Roll", dicePanel);
    m_rollButton->setMinimumHeight(40);
    m_rollButton->setCursor(Qt::PointingHandCursor);
    m_rollButton->setStyleSheet(
        "QPushButton {"
        "  background-color: rgb(66, 120, 87);"
        "  color: white;"
        "  border: none;"
        "  border-radius: 6px;"
        "  padding: 6px 14px;"
        "  font-weight: bold;"
        "}"
        "QPushButton:hover { background-color: rgb(54, 102, 74); }"
        "QPushButton:pressed { background-color: rgb(43, 84, 61); }"
        "QPushButton:disabled {"
        "  background-color: rgb(158, 172, 163);"
        "  color: rgb(230, 235, 232);"
        "}");

    diceLayout->addWidget(m_dieOneLabel);
    diceLayout->addWidget(m_dieTwoLabel);
    diceLayout->addStretch();
    diceLayout->addWidget(m_rollButton);

    setDieFace(m_dieOneLabel, 1);
    setDieFace(m_dieTwoLabel, 1);

    QObject::connect(m_rollButton, &QPushButton::clicked, this, &BoardWidget::rollDice);

    return dicePanel;
}

void BoardWidget::rollDice()
{
    m_rollButton->setEnabled(false);

    QTimer* animationTimer = new QTimer(this);
    animationTimer->setInterval(80);

    QObject::connect(
        animationTimer, &QTimer::timeout, this, [this, animationTimer, frame = 0]() mutable {
            setDieFace(m_dieOneLabel, QRandomGenerator::global()->bounded(1, 7));
            setDieFace(m_dieTwoLabel, QRandomGenerator::global()->bounded(1, 7));

            frame++;
            if (frame >= 10) {
                animationTimer->stop();

                int dieOneNumber = QRandomGenerator::global()->bounded(1, 7);
                int dieTwoNumber = QRandomGenerator::global()->bounded(1, 7);
                setDieFace(m_dieOneLabel, dieOneNumber);
                setDieFace(m_dieTwoLabel, dieTwoNumber);

                int steps = dieOneNumber + dieTwoNumber;
                int currPlayerIdx = m_gameEngine->m_currentPlayerTurn;
                int startSpace = m_gameEngine->m_players[currPlayerIdx]->m_landedSpaceIndex;

                // Let the engine update state immediately
                m_gameEngine->advanceTurn(steps);

                // Start the stepping animation instead of calling updateTokens() directly
                animateTokenSteps(currPlayerIdx, startSpace, steps, steps);

                animationTimer->deleteLater();
            }
        });

    animationTimer->start();
}

BoardWidget::BoardWidget(int botCount, QWidget* parent) : QWidget(parent), m_botCount(botCount)
{
    m_gameEngine = new GameEngine(1, botCount);
    m_gameLogicHandler = new GameLogicHandler(m_gameEngine, this);
    init_UI();
}

BoardWidget::~BoardWidget()
{
    delete m_gameEngine;
}

void BoardWidget::init_UI()
{
    this->setAttribute(Qt::WA_StyledBackground, true);
    this->setObjectName("BoardWidgetContainer");
    this->setStyleSheet("#BoardWidgetContainer { background-color: rgb(205, 230, 208); }");
    init_board_UI();
    init_info_UI();
}

void BoardWidget::init_board_UI()
{
    QGraphicsScene* scene = new QGraphicsScene(this);
    scene->setSceneRect(-5, -5, 1010, 1010);

    QGraphicsView* view = new QGraphicsView(scene, this);
    // view->setGeometry(10, 10, 1010, 1010);
    // Remove view->setGeometry(...) and replace it with:
    view->setFixedSize(1020, 1020);
    view->setObjectName("gameBoard");

    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // 1. Change "border: 2px solid gray;" to "border: none;"
    view->setStyleSheet(
        "#gameBoard {"
        "  background-color: rgb(245, 249, 246);"
        "  border-radius: 10px;"
        "  border: none;"
        "}");

    view->setRenderHint(QPainter::Antialiasing);

    // 2. Create and configure the raised shadow effect
    QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(25);               // How soft and feathered the shadow is
    shadow->setXOffset(0);                   // 0 keeps the shadow centered directly underneath
    shadow->setYOffset(10);                  // Pushes the shadow slightly down to look "raised"
    shadow->setColor(QColor(0, 0, 0, 120));  // Black, but semi-transparent

    // 3. Apply the effect to the game board
    view->setGraphicsEffect(shadow);
    QList<Space*> allSpaces = m_gameEngine->getSpacesList();

    for (Space* spaceLogic : allSpaces) {
        SpaceItem* visualTile = new SpaceItem(spaceLogic);
        int index = spaceLogic->index();

        const QPointF center = tileCenterForIndex(index);
        const qreal centerX = center.x();
        const qreal centerY = center.y();

        // To set the item's exact position, we subtract half its width/height from the center
        int tileWidth =
            (index % 10 == 0) ? Constants::CORNER_TILE_SIZE : Constants::REGULAR_TILE_SIZE;
        int tileHeight = Constants::CORNER_TILE_SIZE;  // Height is always 140 facing inward

        visualTile->setPos(centerX - (tileWidth / 2.0), centerY - (tileHeight / 2.0));

        // Add it to the canvas
        scene->addItem(visualTile);

        m_tileUIMap.insert(index, visualTile);
    }

    // Add rules to the center of the board
    QString rulesString =
        "RULES (Double Dice Rolls):\n"
        "Roll 2: Advance to GO! (Collect $400)\n"
        "Roll 3: Bank error in your favor. Collect $200.\n"
        "Roll 4: Bank pays you a dividend of $50.\n"
        "Roll 5: Advance to the nearest Railroad.\n"
        "Roll 6: Speeding fine! Pay $15.\n"
        "Roll 7: Dividend Paid. Collect $50 from the Bank.\n"
        "Roll 8: Make general repairs on properties (Pay $25/house).\n"
        "Roll 9: Advance to the nearest Water Cooperation.\n"
        "Roll 10: Elected Chairman of the Board. Pay each player $50.\n"
        "Roll 11: Real Estate Boom! Bank pays you $25/property.\n"
        "Roll 12: Go directly to Jail. Do not pass GO.";

    QFont rulesFont("Arial", 10, QFont::Bold);

    // Draw one clean rules panel in the top-left of the inner board area.
    QGraphicsTextItem* rulesText = scene->addText(rulesString, rulesFont);
    rulesText->setDefaultTextColor(QColor(43, 84, 61));

    const QRectF textRect = rulesText->boundingRect();
    const qreal panelPadding = 12.0;
    const qreal panelWidth = textRect.width() + (panelPadding * 2);
    const qreal panelHeight = textRect.height() + (panelPadding * 2);
    const QRectF panelRect((1000 - panelWidth) / 2.0, (1000 - panelHeight) / 2.0, panelWidth,
                           panelHeight);

    QGraphicsRectItem* rulesPanel =
        scene->addRect(panelRect, QPen(QColor(66, 120, 87), 2), QBrush(QColor(245, 249, 246, 235)));
    rulesPanel->setZValue(10);

    rulesText->setPos(panelRect.left() + panelPadding, panelRect.top() + panelPadding);
    rulesText->setZValue(11);

    init_players_UI(scene);
}

void BoardWidget::init_players_UI(QGraphicsScene* scene)
{
    // 1. Get the exact center of Tile 0 (Start)
    QPointF startCenter = m_tileUIMap[0]->sceneBoundingRect().center();

    // 2. Loop to create players (supports up to 8)
    int num_players = m_botCount + 1;
    for (int i = 0; i < num_players; i++) {
        // Create the token - No image needed, just pass the ID!
        PlayerTokenItem* token = new PlayerTokenItem(i);

        // 3. Grid-Based Position Offset Math
        QPointF targetPos = getTokenTargetPos(i, startCenter);
        token->setPos(targetPos);

        // 5. Add to scene and save to our list
        scene->addItem(token);
        m_playerTokens.append(token);
    }
}

void BoardWidget::init_info_UI()
{
    // 1. Create the container for the Right Side
    QWidget* sidePanel = new QWidget(this);
    sidePanel->setFixedWidth(
        400);  // Give the dashboard a fixed width so it doesn't stretch infinitely
    sidePanel->setObjectName("sidePanel");
    sidePanel->setStyleSheet(
        "#sidePanel {"
        "  background-color: rgb(242, 247, 243);"
        "  border: 2px solid rgb(66, 120, 87);"
        "  border-radius: 10px;"
        "}");

    // 2. Create a Vertical Layout to stack the UI elements
    QVBoxLayout* sideLayout = new QVBoxLayout(sidePanel);
    sideLayout->setAlignment(Qt::AlignTop);  // Push everything to the top
    sideLayout->setSpacing(15);              // Gap between each player card
    sideLayout->setContentsMargins(12, 12, 12, 12);

    // 3. Add a Title
    QLabel* dashboardTitle = new QLabel("Game Dashboard", sidePanel);
    dashboardTitle->setFont(QFont("Arial", 22, QFont::Bold));
    dashboardTitle->setAlignment(Qt::AlignCenter);
    sideLayout->addWidget(dashboardTitle);

    QFrame* dicePanel = createDicePanel(sidePanel);

    // Create a scroll area for the player cards
    QScrollArea* playerScrollArea = new QScrollArea(sidePanel);
    playerScrollArea->setWidgetResizable(true);
    playerScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    playerScrollArea->setFrameShape(QFrame::NoFrame);
    playerScrollArea->setStyleSheet("QScrollArea { background: transparent; }");

    QWidget* playerListContainer = new QWidget(playerScrollArea);
    playerListContainer->setStyleSheet("background: transparent;");
    m_playerListLayout = new QVBoxLayout(playerListContainer);
    m_playerListLayout->setAlignment(Qt::AlignTop);
    m_playerListLayout->setSpacing(15);
    m_playerListLayout->setContentsMargins(0, 0, 0, 0);

    // 5. Generate the 8 Player Cards
    for (int i = 0; i < m_gameEngine->m_players.size(); i++) {
        // QString pName = QString("Player %1").arg(i + 1);
        QString pName = m_gameEngine->m_players[i]->m_name;  // Get player name from GameEngine

        // Instantiate the custom widget we discussed earlier
        PlayerInfoWidget* pCard = new PlayerInfoWidget(
            m_gameEngine->m_players[i]->m_id, m_gameEngine->m_players[i]->m_cash,
            PlayerColor[i % PlayerColor.size()], pName, playerListContainer);

        // Add it to the internal scroll layout
        m_playerListLayout->addWidget(pCard);
        m_playerCards.append(pCard);
    }

    m_playerListLayout->addStretch();
    playerScrollArea->setWidget(playerListContainer);

    // Add the scroll area to the main side layout so it takes up leftover space
    sideLayout->addWidget(playerScrollArea, 1);

    action_layout(sideLayout, sidePanel);

    sideLayout->addWidget(dicePanel);

    // ---------------------------------------------------------
    // 6. THE MASTER LAYOUT (Binding Left and Right together)
    // ---------------------------------------------------------

    // Check if the BoardWidget already has a layout to avoid crashing
    if (!this->layout()) {
        QHBoxLayout* mainLayout = new QHBoxLayout(this);
        mainLayout->setContentsMargins(10, 10, 10, 10);
        mainLayout->setSpacing(20);

        // Find the board we created in init_board_UI()
        // (Because we set its ObjectName to "gameBoard" earlier, we can safely grab it!)
        QGraphicsView* boardView = this->findChild<QGraphicsView*>("gameBoard");

        if (boardView) {
            mainLayout->addWidget(boardView);  // Put board on the Left
        }

        mainLayout->addWidget(sidePanel);  // Put dashboard on the Right
    }
}

void BoardWidget::action_layout(QVBoxLayout* layout, QWidget* parent)
{
    QFrame* actionPanel = new QFrame(parent);
    actionPanel->setObjectName("actionPanel");
    actionPanel->setStyleSheet(
        "#actionPanel {"
        "  background-color: white;"
        "  border: 2px solid rgb(66, 120, 87);"
        "  border-radius: 8px;"
        "  padding: 8px;"  // Added padding for a cleaner look
        "}");

    QVBoxLayout* actionLayout = new QVBoxLayout(actionPanel);

    // Initialize the Label and save to member variable
    m_turnStatusLabel = new QLabel("", actionPanel);
    m_turnStatusLabel->setAlignment(Qt::AlignCenter);
    m_turnStatusLabel->setFont(QFont("Arial", 11));  // Make it slightly larger

    // Add them to the panel
    actionLayout->addWidget(m_turnStatusLabel);

    layout->addWidget(actionPanel);

    // Immediately trigger an update to load Player 1's starting info
    updateActionPanel();
    updatePlayerInfo();
}

void BoardWidget::updateActionPanel()
{
    // 1. Get the current state from your Game Engine
    int currPlayerIdx = m_gameEngine->m_currentPlayerTurn;
    Player* currPlayer = m_gameEngine->m_players[currPlayerIdx];

    // 2. Get the space the player is currently sitting on
    Space* landedSpace = m_gameEngine->m_spaces[currPlayer->m_landedSpaceIndex];

    // 3. Update the Text
    QString statusText = QString("<b>%1's Turn</b><br>Landed on: %2")
                             .arg(currPlayer->m_name)
                             .arg(landedSpace->name());
    m_turnStatusLabel->setText(statusText);
}

void BoardWidget::animateTokenSteps(int playerId, int currentSpace, int stepsRemaining,
                                    int dice_outcome)
{
    if (stepsRemaining <= 0) {
        // Once stepping is done, do final UI updates
        bool animationOngoing =
            m_gameLogicHandler->handlePostMoveActions(m_tileUIMap, dice_outcome);
        if (animationOngoing) {
            return;
        }
        updateActionPanel();
        updatePlayerInfo();
        int nextPlayerIdx = m_gameEngine->m_currentPlayerTurn;
        Player* nextPlayer = m_gameEngine->m_players[nextPlayerIdx];

        if (nextPlayer->m_isBot) {
            m_rollButton->setEnabled(false);
            QTimer::singleShot(1500, this, &BoardWidget::rollDice);
        } else {
            m_rollButton->setEnabled(true);
        }
        return;
    }

    // Determine next space index (wrap around the board)
    int nextSpace = (currentSpace + 1) % m_gameEngine->m_spaces.size();

    if (nextSpace == Constants::START_SPACE_INDEX) {
        m_gameEngine->increasePlayerCash(400);
        Player* movingPlayer = m_gameEngine->m_players[playerId];
        QString msg = QString("%1 passed GO!\nCollected $400").arg(movingPlayer->m_name);
        QColor playerColor = PlayerColor[playerId % PlayerColor.size()];
        ToastNotification* toast = new ToastNotification(msg, playerColor, this);
        toast->showToast();
    }
    // Get the player token items
    PlayerTokenItem* token = m_playerTokens[playerId];
    SpaceItem* visualTile = m_tileUIMap[nextSpace];

    if (!visualTile || !token) {
        // Fallback safety
        updateTokens();
        return;
    }

    QPointF tileCenter = visualTile->sceneBoundingRect().center();

    QPointF targetPos = getTokenTargetPos(playerId, tileCenter);

    QVariantAnimation* anim = new QVariantAnimation(this);
    anim->setDuration(250);  // fast transition per tile
    anim->setStartValue(token->pos());
    anim->setEndValue(targetPos);
    anim->setEasingCurve(QEasingCurve::InOutQuad);

    QObject::connect(anim, &QVariantAnimation::valueChanged,
                     [token](const QVariant& value) { token->setPos(value.toPointF()); });

    // When this tile animation finishes, trigger the next step
    QObject::connect(anim, &QVariantAnimation::finished, this,
                     [this, anim, playerId, nextSpace, stepsRemaining, dice_outcome]() {
                         anim->deleteLater();
                         animateTokenSteps(playerId, nextSpace, stepsRemaining - 1, dice_outcome);
                     });

    anim->start();
}

void BoardWidget::updateTokens()
{
    // Iterate through all tokens and update their position
    for (PlayerTokenItem* token : m_playerTokens) {
        int playerId = token->getPlayerID();
        Player* player = m_gameEngine->m_players[playerId];
        int spaceIndex = player->m_landedSpaceIndex;

        // Get the space visually
        SpaceItem* visualTile = m_tileUIMap[spaceIndex];
        if (visualTile) {
            QPointF tileCenter = visualTile->sceneBoundingRect().center();
            QPointF targetPos = getTokenTargetPos(playerId, tileCenter);

            if (token->pos() != targetPos) {
                QVariantAnimation* anim = new QVariantAnimation(this);
                anim->setDuration(600);  // Smooth 600ms transition
                anim->setStartValue(token->pos());
                anim->setEndValue(targetPos);
                anim->setEasingCurve(QEasingCurve::InOutQuad);

                QObject::connect(
                    anim, &QVariantAnimation::valueChanged,
                    [token](const QVariant& value) { token->setPos(value.toPointF()); });
                QObject::connect(anim, &QVariantAnimation::finished, anim, &QObject::deleteLater);
                anim->start();
            } else {
                token->setPos(targetPos);
            }
        }
    }
}

void BoardWidget::updatePlayerInfo()
{
    int currPlayerIdx = m_gameEngine->m_currentPlayerTurn;

    QVector<int> orderedPlayerIds;
    orderedPlayerIds.reserve(m_gameEngine->m_players.size());

    for (int i = 0; i < m_playerCards.size(); i++) {
        Player* player = m_gameEngine->m_players[i];
        orderedPlayerIds.append(i);

        m_playerCards[i]->setActive(i == currPlayerIdx);
        m_playerCards[i]->updateCash(player->m_cash);
        m_playerCards[i]->updateNetWorth(player->m_netWorth);
        m_playerCards[i]->setOwnedProperties(player->m_ownedSpaces);
    }

    std::sort(orderedPlayerIds.begin(), orderedPlayerIds.end(), [this](int lhsId, int rhsId) {
        const Player* lhs = m_gameEngine->m_players[lhsId];
        const Player* rhs = m_gameEngine->m_players[rhsId];

        if (lhs->m_netWorth != rhs->m_netWorth) {
            return lhs->m_netWorth > rhs->m_netWorth;
        }

        return lhsId < rhsId;
    });

    if (m_playerListLayout) {
        for (int displayPos = 0; displayPos < orderedPlayerIds.size(); ++displayPos) {
            m_playerListLayout->insertWidget(displayPos,
                                             m_playerCards[orderedPlayerIds[displayPos]]);
        }
    }
}
