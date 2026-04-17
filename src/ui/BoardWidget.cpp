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

QFrame* createDicePanel(QWidget* parent)
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

    QLabel* dieOneLabel = new QLabel(dicePanel);
    QLabel* dieTwoLabel = new QLabel(dicePanel);
    dieOneLabel->setAlignment(Qt::AlignCenter);
    dieTwoLabel->setAlignment(Qt::AlignCenter);
    dieOneLabel->setFixedSize(72, 72);
    dieTwoLabel->setFixedSize(72, 72);
    dieOneLabel->setStyleSheet(
        "background-color: transparent; border: none; margin: 0px; padding: 0px;");
    dieTwoLabel->setStyleSheet(
        "background-color: transparent; border: none; margin: 0px; padding: 0px;");

    QPushButton* rollButton = new QPushButton("Roll", dicePanel);
    rollButton->setMinimumHeight(40);
    rollButton->setCursor(Qt::PointingHandCursor);
    rollButton->setStyleSheet(
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

    diceLayout->addWidget(dieOneLabel);
    diceLayout->addWidget(dieTwoLabel);
    diceLayout->addStretch();
    diceLayout->addWidget(rollButton);

    setDieFace(dieOneLabel, 1);
    setDieFace(dieTwoLabel, 1);

    QObject::connect(
        rollButton, &QPushButton::clicked, dicePanel, [dieOneLabel, dieTwoLabel, rollButton]() {
            rollButton->setEnabled(false);

            QTimer* animationTimer = new QTimer(rollButton);
            animationTimer->setInterval(80);

            QObject::connect(
                animationTimer, &QTimer::timeout, animationTimer,
                [dieOneLabel, dieTwoLabel, rollButton, animationTimer, frame = 0]() mutable {
                    setDieFace(dieOneLabel, QRandomGenerator::global()->bounded(1, 7));
                    setDieFace(dieTwoLabel, QRandomGenerator::global()->bounded(1, 7));

                    frame++;
                    if (frame >= 10) {
                        animationTimer->stop();

                        setDieFace(dieOneLabel, QRandomGenerator::global()->bounded(1, 7));
                        setDieFace(dieTwoLabel, QRandomGenerator::global()->bounded(1, 7));

                        rollButton->setEnabled(true);
                        animationTimer->deleteLater();
                    }
                });

            animationTimer->start();
        });

    return dicePanel;
}

void setDieFace(QLabel* dieLabel, int dieValue)
{
    dieLabel->setPixmap(createDiePixmap(dieValue, dieLabel->width()));
}

}  // namespace

BoardWidget::BoardWidget(int botCount, QWidget* parent) : QWidget(parent), m_botCount(botCount)
{
    m_gameEngine = new GameEngine();
    init_UI();
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
    for (int i = 0; i <= m_botCount; i++) {
        QString pName = QString("Player %1").arg(i + 1);

        // Instantiate the custom widget we discussed earlier
        PlayerInfoWidget* pCard = new PlayerInfoWidget(i, colors[i], pName, playerListContainer);

        // Add it to the internal scroll layout
        playerListLayout->addWidget(pCard);
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
    // --- NEW ACTION PANEL ---
    QFrame* actionPanel = new QFrame(parent);
    actionPanel->setObjectName("actionPanel");
    actionPanel->setStyleSheet(
        "#actionPanel {"
        "  background-color: white;"
        "  border: 2px solid rgb(66, 120, 87);"
        "  border-radius: 8px;"
        "}");

    QVBoxLayout* actionLayout = new QVBoxLayout(actionPanel);
    QLabel* turnStatusLabel =
        new QLabel("<b>Player 1's Turn</b><br>Landed on: Boardwalk", actionPanel);
    turnStatusLabel->setAlignment(Qt::AlignCenter);

    QHBoxLayout* actionButtonsLayout = new QHBoxLayout();
    QPushButton* buyBtn = new QPushButton("Buy ($400)", actionPanel);
    QPushButton* endTurnBtn = new QPushButton("End Turn", actionPanel);

    // Style the buttons
    QString btnStyle =
        "QPushButton { background-color: #2b5c40; color: white; border-radius: 4px; padding: 6px; "
        "font-weight: bold; }";
    buyBtn->setStyleSheet(btnStyle);
    endTurnBtn->setStyleSheet(btnStyle);

    actionButtonsLayout->addWidget(buyBtn);
    actionButtonsLayout->addWidget(endTurnBtn);

    actionLayout->addWidget(turnStatusLabel);
    actionLayout->addLayout(actionButtonsLayout);

    // Add them to the main side layout
    layout->addWidget(actionPanel);
}