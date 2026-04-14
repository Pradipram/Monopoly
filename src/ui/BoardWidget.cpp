#include "BoardWidget.h"

#include <QGraphicsDropShadowEffect>
#include <QGraphicsScene>
#include <QGraphicsView>

#include "SpaceItem.h"

BoardWidget::BoardWidget(int botCount, QWidget* parent) : QWidget(parent), m_botCount(botCount)
{
    m_gameEngine = new GameEngine();
    init_UI();
}

void BoardWidget::init_UI()
{
    this->setAttribute(Qt::WA_StyledBackground, true);
    this->setStyleSheet("background-color: rgb(205, 230, 208);");
    init_board_UI();
    init_info_UI();
}

void BoardWidget::init_board_UI()
{
    QGraphicsScene* scene = new QGraphicsScene(this);
    scene->setSceneRect(-5, -5, 1010, 1010);

    QGraphicsView* view = new QGraphicsView(scene, this);
    view->setGeometry(10, 10, 1010, 1010);
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

    int maxPos = 1000;
    int cornerSize = 140;
    int regSize = 80;

    // Pre-calculate the halves to easily find the exact center of any tile
    double cornerHalf = cornerSize / 2.0;
    double regHalf = regSize / 2.0;

    for (Space* spaceLogic : allSpaces) {
        SpaceItem* visualTile = new SpaceItem(spaceLogic);
        int index = spaceLogic->index();

        double centerX = 0;
        double centerY = 0;

        // Calculate the exact Center X/Y for the tile
        if (index == 0) {  // START (Bottom Right)
            centerX = maxPos - cornerHalf;
            centerY = maxPos - cornerHalf;
        } else if (index > 0 && index < 10) {  // Bottom Row (Moving Left)
            centerX = maxPos - cornerSize - regHalf - ((index - 1) * regSize);
            centerY = maxPos - cornerHalf;
        } else if (index == 10) {  // JAIL (Bottom Left)
            centerX = cornerHalf;
            centerY = maxPos - cornerHalf;
        } else if (index > 10 && index < 20) {  // Left Row (Moving Up)
            centerX = cornerHalf;
            centerY = maxPos - cornerSize - regHalf - ((index - 11) * regSize);
        } else if (index == 20) {  // FREE PARKING (Top Left)
            centerX = cornerHalf;
            centerY = cornerHalf;
        } else if (index > 20 && index < 30) {  // Top Row (Moving Right)
            centerX = cornerSize + regHalf + ((index - 21) * regSize);
            centerY = cornerHalf;
        } else if (index == 30) {  // GO TO JAIL (Top Right)
            centerX = maxPos - cornerHalf;
            centerY = cornerHalf;
        } else if (index > 30 && index < 40) {  // Right Row (Moving Down)
            centerX = maxPos - cornerHalf;
            centerY = cornerSize + regHalf + ((index - 31) * regSize);
        }

        // To set the item's exact position, we subtract half its width/height from the center
        int tileWidth = (index % 10 == 0) ? cornerSize : regSize;
        int tileHeight = cornerSize;  // Height is always 140 facing inward

        visualTile->setPos(centerX - (tileWidth / 2.0), centerY - (tileHeight / 2.0));

        // Add it to the canvas
        scene->addItem(visualTile);
    }
}

void BoardWidget::init_info_UI() {}