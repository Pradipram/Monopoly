#include "BoardWidget.h"

#include <QFrame>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QPushButton>
#include <QRandomGenerator>
#include <QScrollArea>
#include <QTimer>
#include <QVariantAnimation>
#include <QEasingCurve>

#include "PlayerInfoWidget.h"
#include "SpaceItem.h"

namespace
{

QPointF tileCenterForIndex(int index, int maxPos, int cornerSize, int regSize)
{
    const qreal cornerHalf = cornerSize / 2.0;
    const qreal regHalf = regSize / 2.0;

    if (index == 0) {
        return {maxPos - cornerHalf, maxPos - cornerHalf};
    }
    if (index > 0 && index < 10) {
        return {maxPos - cornerSize - regHalf - ((index - 1) * regSize), maxPos - cornerHalf};
    }
    if (index == 10) {
        return {cornerHalf, maxPos - cornerHalf};
    }
    if (index > 10 && index < 20) {
        return {cornerHalf, maxPos - cornerSize - regHalf - ((index - 11) * regSize)};
    }
    if (index == 20) {
        return {cornerHalf, cornerHalf};
    }
    if (index > 20 && index < 30) {
        return {cornerSize + regHalf + ((index - 21) * regSize), cornerHalf};
    }
    if (index == 30) {
        return {maxPos - cornerHalf, cornerHalf};
    }
    if (index > 30 && index < 40) {
        return {maxPos - cornerHalf, cornerSize + regHalf + ((index - 31) * regSize)};
    }

    return {};
}

QPixmap createDiePixmap(int dieValue, int size)
{
    QPixmap pixmap(size, size);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing, true);

    QRectF dieRect(1.0, 1.0, size - 2.0, size - 2.0);
    painter.setBrush(Qt::white);
    painter.setPen(QPen(QColor(30, 30, 30), 2));
    painter.drawRoundedRect(dieRect, 10, 10);

    const qreal left = size * 0.23;
    const qreal center = size * 0.50;
    const qreal right = size * 0.77;
    const qreal top = size * 0.23;
    const qreal middle = size * 0.50;
    const qreal bottom = size * 0.77;
    const qreal pipRadius = size * 0.09;

    auto drawPip = [&painter, pipRadius](qreal x, qreal y) {
        painter.setBrush(QColor(30, 30, 30));
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(QPointF(x, y), pipRadius, pipRadius);
    };

    switch (dieValue) {
        case 1:
            drawPip(center, middle);
            break;
        case 2:
            drawPip(left, top);
            drawPip(right, bottom);
            break;
        case 3:
            drawPip(left, top);
            drawPip(center, middle);
            drawPip(right, bottom);
            break;
        case 4:
            drawPip(left, top);
            drawPip(right, top);
            drawPip(left, bottom);
            drawPip(right, bottom);
            break;
        case 5:
            drawPip(left, top);
            drawPip(right, top);
            drawPip(center, middle);
            drawPip(left, bottom);
            drawPip(right, bottom);
            break;
        case 6:
            drawPip(left, top);
            drawPip(right, top);
            drawPip(left, middle);
            drawPip(right, middle);
            drawPip(left, bottom);
            drawPip(right, bottom);
            break;
        default:
            drawPip(center, middle);
            break;
    }

    return pixmap;
}

void setDieFace(QLabel* dieLabel, int dieValue);

}  // namespace

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
    m_dieOneLabel->setFixedSize(72, 72);
    m_dieTwoLabel->setFixedSize(72, 72);
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
        "QPushButton:pressed { background-color: rgb(43, 84, 61); }");

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

    QObject::connect(animationTimer, &QTimer::timeout, this,
                     [this, animationTimer, frame = 0]() mutable {
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
                             animateTokenSteps(currPlayerIdx, startSpace, steps);

                             animationTimer->deleteLater();
                         }
                     });

    animationTimer->start();
}

namespace
{
void setDieFace(QLabel* dieLabel, int dieValue)
{
    dieLabel->setPixmap(createDiePixmap(dieValue, dieLabel->width()));
}

}  // namespace

BoardWidget::BoardWidget(int botCount, QWidget* parent) : QWidget(parent), m_botCount(botCount)
{
    m_gameEngine = new GameEngine(1, botCount);
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
        "  background-color: rgb(255, 255, 255);"
        "  border-radius: 10px;"
        "  border: none;"
        "  border-image: url(:/background.jpg) 0 0 0 0 stretch stretch;"
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

    const int maxPos = 1000;
    const int cornerSize = 140;
    const int regSize = 80;

    for (Space* spaceLogic : allSpaces) {
        SpaceItem* visualTile = new SpaceItem(spaceLogic);
        int index = spaceLogic->index();

        const QPointF center = tileCenterForIndex(index, maxPos, cornerSize, regSize);
        const qreal centerX = center.x();
        const qreal centerY = center.y();

        // To set the item's exact position, we subtract half its width/height from the center
        int tileWidth = (index % 10 == 0) ? cornerSize : regSize;
        int tileHeight = cornerSize;  // Height is always 140 facing inward

        visualTile->setPos(centerX - (tileWidth / 2.0), centerY - (tileHeight / 2.0));

        // Add it to the canvas
        scene->addItem(visualTile);

        m_tileUIMap.insert(index, visualTile);
    }
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
        // Arranges up to 9 tokens in a 3x3 grid
        int col = i % 3;
        int row = i / 3;

        double dx = (col - 1) * 25;  // Spacing of 25 pixels between columns
        double dy = (row - 1) * 25;  // Spacing of 25 pixels between rows

        // 4. Center the token's own bounding box on that new shifted coordinate
        // Since we hardcoded the circle to be 24x24, half of that is 12
        double tokenOffsetX = 12.0;
        double tokenOffsetY = 12.0;

        token->setPos(startCenter.x() + dx - tokenOffsetX, startCenter.y() + dy - tokenOffsetY);

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

    // 4. Define your 8 player colors (matching your tokens!)
    QList<QColor> colors = {Qt::red,  Qt::blue,    Qt::green,           Qt::yellow,
                            Qt::cyan, Qt::magenta, QColor(255, 165, 0), QColor(128, 0, 128)};

    // Create a scroll area for the player cards
    QScrollArea* playerScrollArea = new QScrollArea(sidePanel);
    playerScrollArea->setWidgetResizable(true);
    playerScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    playerScrollArea->setFrameShape(QFrame::NoFrame);
    playerScrollArea->setStyleSheet("QScrollArea { background: transparent; }");

    QWidget* playerListContainer = new QWidget(playerScrollArea);
    playerListContainer->setStyleSheet("background: transparent;");
    QVBoxLayout* playerListLayout = new QVBoxLayout(playerListContainer);
    playerListLayout->setAlignment(Qt::AlignTop);
    playerListLayout->setSpacing(15);
    playerListLayout->setContentsMargins(0, 0, 0, 0);

    // 5. Generate the 8 Player Cards
    for (int i = 0; i < m_gameEngine->m_players.size(); i++) {
        // QString pName = QString("Player %1").arg(i + 1);
        QString pName = m_gameEngine->m_players[i]->m_name;  // Get player name from GameEngine

        // Instantiate the custom widget we discussed earlier
        PlayerInfoWidget* pCard = new PlayerInfoWidget(m_gameEngine->m_players[i]->m_id,
                                                       m_gameEngine->m_players[i]->m_cash,
                                                       colors[i], pName, playerListContainer);

        // Add it to the internal scroll layout
        playerListLayout->addWidget(pCard);
        m_playerCards.append(pCard);
    }

    playerListLayout->addStretch();
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
        "}");

    QVBoxLayout* actionLayout = new QVBoxLayout(actionPanel);

    // 1. Initialize the Label (Empty for now), save to member variable
    m_turnStatusLabel = new QLabel("", actionPanel);
    m_turnStatusLabel->setAlignment(Qt::AlignCenter);

    // 2. Create a sub-widget just to hold our dynamic buttons
    QWidget* buttonContainer = new QWidget(actionPanel);
    m_actionButtonsLayout = new QHBoxLayout(buttonContainer);
    m_actionButtonsLayout->setContentsMargins(0, 0, 0, 0);

    // Add them to the panel
    actionLayout->addWidget(m_turnStatusLabel);
    actionLayout->addWidget(buttonContainer);

    layout->addWidget(actionPanel);

    // 3. Immediately trigger an update to load Player 1's starting info
    updateActionPanel();
    updatePlayerInfo();
}

void BoardWidget::updateActionPanel()
{
    // 1. Get the current state from your Game Engine
    int currPlayerIdx = m_gameEngine->m_currentPlayerTurn;
    Player* currPlayer = m_gameEngine->m_players[currPlayerIdx];

    // Assuming you have a way to get the space the player is currently sitting on
    // Space* landedSpace = m_gameEngine->getSpacesList()[currPlayer->m_position];
    Space* landedSpace = m_gameEngine->m_spaces[currPlayer->m_landedSpaceIndex];

    // 2. Update the Text
    QString statusText = QString("<b>%1's Turn</b><br>Landed on: %2")
                             .arg(currPlayer->m_name)
                             .arg(landedSpace->name());
    m_turnStatusLabel->setText(statusText);

    // 3. CLEAR OLD BUTTONS (Crucial step in Qt!)
    // If you don't do this, you'll get 50 buttons stacked up after a few turns.
    QLayoutItem* child;
    while ((child = m_actionButtonsLayout->takeAt(0)) != nullptr) {
        delete child->widget();  // Delete the physical button
        delete child;            // Delete the layout reference
    }

    // 4. GENERATE NEW BUTTONS based on game logic
    QString btnStyle =
        "QPushButton { background-color: #2b5c40; color: white; border-radius: 4px; padding: 6px; "
        "font-weight: bold; }";

    // Example Logic: Unowned Property -> Show "Buy" Button
    if (landedSpace->type() == SpaceConstants::SpaceType::Property && landedSpace->ownerId == -1) {
        QPushButton* buyBtn = new QPushButton(QString("Buy ($%1)").arg(landedSpace->price()));
        buyBtn->setStyleSheet(btnStyle);
        m_actionButtonsLayout->addWidget(buyBtn);

        // Connect the button to your buy logic (you will write this later)
        // connect(buyBtn, &QPushButton::clicked, this, &BoardWidget::handleBuyProperty);
    }

    // Example Logic: Owned by someone else -> Show "Pay Rent" Button
    else if (landedSpace->type() == SpaceConstants::SpaceType::Property &&
             landedSpace->ownerId != currPlayer->m_id) {
        QPushButton* payBtn = new QPushButton("Pay Rent");
        payBtn->setStyleSheet(
            "QPushButton { background-color: #a83232; color: white; border-radius: 4px; padding: "
            "6px; font-weight: bold; }");  // Red for paying money
        m_actionButtonsLayout->addWidget(payBtn);
    }
}

void BoardWidget::animateTokenSteps(int playerId, int currentSpace, int stepsRemaining)
{
    if (stepsRemaining <= 0) {
        // Once stepping is done, do final UI updates
        updateActionPanel();
        updatePlayerInfo();
        m_rollButton->setEnabled(true);
        return;
    }

    // Determine next space index (wrap around the board)
    int nextSpace = (currentSpace + 1) % m_gameEngine->m_spaces.size();

    // Get the player token items
    PlayerTokenItem* token = m_playerTokens[playerId];
    SpaceItem* visualTile = m_tileUIMap[nextSpace];

    if (!visualTile || !token) {
        // Fallback safety
        updateTokens();
        return;
    }

    QPointF tileCenter = visualTile->sceneBoundingRect().center();
            
    // Grid spacing logic
    int col = playerId % 3;
    int row = playerId / 3;

    double dx = (col - 1) * 25;  
    double dy = (row - 1) * 25;  

    double tokenOffsetX = 12.0;
    double tokenOffsetY = 12.0;

    QPointF targetPos(tileCenter.x() + dx - tokenOffsetX, tileCenter.y() + dy - tokenOffsetY);

    QVariantAnimation* anim = new QVariantAnimation(this);
    anim->setDuration(250); // fast transition per tile
    anim->setStartValue(token->pos());
    anim->setEndValue(targetPos);
    anim->setEasingCurve(QEasingCurve::InOutQuad);
    
    QObject::connect(anim, &QVariantAnimation::valueChanged, [token](const QVariant& value) {
        token->setPos(value.toPointF());
    });
    
    // When this tile animation finishes, trigger the next step
    QObject::connect(anim, &QVariantAnimation::finished, this, [this, anim, playerId, nextSpace, stepsRemaining]() {
        anim->deleteLater();
        animateTokenSteps(playerId, nextSpace, stepsRemaining - 1);
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
            
            // Reapply grid spacing logic
            int col = playerId % 3;
            int row = playerId / 3;

            double dx = (col - 1) * 25;  
            double dy = (row - 1) * 25;  

            double tokenOffsetX = 12.0;
            double tokenOffsetY = 12.0;

            QPointF targetPos(tileCenter.x() + dx - tokenOffsetX, tileCenter.y() + dy - tokenOffsetY);

            if (token->pos() != targetPos) {
                QVariantAnimation* anim = new QVariantAnimation(this);
                anim->setDuration(600); // Smooth 600ms transition
                anim->setStartValue(token->pos());
                anim->setEndValue(targetPos);
                anim->setEasingCurve(QEasingCurve::InOutQuad);
                
                QObject::connect(anim, &QVariantAnimation::valueChanged, [token](const QVariant& value) {
                    token->setPos(value.toPointF());
                });
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
    for (int i = 0; i < m_playerCards.size(); i++) {
        m_playerCards[i]->setActive(i == currPlayerIdx);
        m_playerCards[i]->updateCash(m_gameEngine->m_players[i]->m_cash);
        // You could also update properties if that changes.
    }
}