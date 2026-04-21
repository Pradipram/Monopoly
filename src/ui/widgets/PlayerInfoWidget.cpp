#include "PlayerInfoWidget.h"

#include <QDialog>
#include <QDialogButtonBox>
#include <QFrame>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QListWidget>
#include <QTimer>
#include <QVBoxLayout>

#include "Space.h"
#include "SpaceItem.h"

PlayerInfoWidget::PlayerInfoWidget(int playerId, int cash, QColor playerColor,
                                   const QString& playerName, QWidget* parent)
    : QWidget(parent), m_playerColor(playerColor)
{
    m_currentCash = cash;

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
    m_cashLabel = new QLabel("Cash: $" + QString::number(cash));
    m_cashLabel->setStyleSheet("color: green; font-weight: bold; font-size: 14px;");

    m_netWorthLabel = new QLabel("Net Worth: $" + QString::number(cash));
    m_netWorthLabel->setStyleSheet("color: #1f2937; font-weight: bold; font-size: 13px;");

    m_propertiesButton = new QPushButton("Owned Spaces: 0");
    m_propertiesButton->setCursor(Qt::PointingHandCursor);
    m_propertiesButton->setStyleSheet(
        "QPushButton {"
        "  background-color: rgb(66, 120, 87);"
        "  color: white;"
        "  border-radius: 4px;"
        "  padding: 5px;"
        "  font-weight: bold;"
        "}"
        "QPushButton:hover:enabled { background-color: rgb(54, 102, 74); }"
        "QPushButton:disabled { background-color: rgb(158, 172, 163); color: rgb(230, 235, 232); "
        "}");
    connect(m_propertiesButton, &QPushButton::clicked, this,
            &PlayerInfoWidget::showOwnedPropertiesDialog);

    mainLayout->addWidget(m_nameLabel);
    mainLayout->addWidget(m_cashLabel);
    mainLayout->addWidget(m_netWorthLabel);
    mainLayout->addWidget(m_propertiesButton);
}

void PlayerInfoWidget::updateCash(int newAmount)
{
    if (newAmount == m_currentCash) {
        return;  // No change, skip animation
    }

    int oldAmount = m_currentCash;
    m_currentCash = newAmount;

    // Determine if cash increased or decreased for color feedback
    bool isIncrease = newAmount > oldAmount;
    QString changeText = isIncrease ? "+" : "";
    changeText += QString::number(newAmount - oldAmount);

    // Highlight the label temporarily
    QString originalStyle = m_cashLabel->styleSheet();
    QString highlightColor = isIncrease ? "green" : "red";
    m_cashLabel->setStyleSheet(
        QString("color: %1; font-weight: bold; font-size: 16px; padding: 4px; border-radius: 4px; "
                "background-color: rgba(0, 0, 0, 20);")
            .arg(highlightColor));

    // Update the text immediately
    m_cashLabel->setText(QString("Cash: $%1 (%2)").arg(newAmount).arg(changeText));

    // Animate back to normal after 800ms
    QTimer::singleShot(800, this, [this, originalStyle]() {
        m_cashLabel->setStyleSheet("color: green; font-weight: bold; font-size: 14px;");
        m_cashLabel->setText("Cash: $" + QString::number(m_currentCash));
    });
}

void PlayerInfoWidget::setOwnedProperties(const QVector<Space*>& spaces)
{
    m_ownedSpaces = spaces;
    m_propertiesButton->setText("Owned Spaces: " + QString::number(m_ownedSpaces.size()));
}

void PlayerInfoWidget::updateNetWorth(int netWorth)
{
    m_netWorthLabel->setText("Net Worth: $" + QString::number(netWorth));
}

void PlayerInfoWidget::showOwnedPropertiesDialog()
{
    QDialog dialog(this);
    dialog.setWindowTitle("Owned Spaces");
    dialog.setMinimumWidth(500);
    dialog.setMinimumHeight(400);
    dialog.setStyleSheet(
        "QDialog { background-color: white; } "
        "QLabel { color: black; } "
        "QGraphicsView { background-color: #fafafa; border: 1px solid #cfcfcf; outline: none; } "
        "QPushButton { color: black; background-color: #f0f0f0; padding: 5px; border: 1px solid "
        "#aaa; border-radius: 3px; }"
        "QPushButton:hover:enabled { background-color: #e4e4e4; }"
        "QPushButton:disabled { background-color: #eeeeee; color: #9a9a9a; border-color: #d5d5d5; "
        "}");

    QVBoxLayout* layout = new QVBoxLayout(&dialog);
    QLabel* titleLabel = new QLabel("Properties owned by this player:", &dialog);
    titleLabel->setStyleSheet("font-weight: bold; margin-bottom: 5px;");
    layout->addWidget(titleLabel);

    if (m_ownedSpaces.isEmpty()) {
        QFrame* emptyCard = new QFrame(&dialog);
        emptyCard->setObjectName("emptyOwnedCard");
        emptyCard->setStyleSheet(
            "#emptyOwnedCard { background-color: #f7f7f7; border: 1px dashed #c7c7c7; "
            "border-radius: 8px; padding: 18px; }");
        QVBoxLayout* emptyLayout = new QVBoxLayout(emptyCard);
        QLabel* emptyTitle = new QLabel("No properties owned yet.", emptyCard);
        emptyTitle->setAlignment(Qt::AlignCenter);
        emptyTitle->setStyleSheet("font-weight: bold; font-size: 14px; color: #444;");
        QLabel* emptyHint =
            new QLabel("Owned spaces will appear here as detailed board tiles.", emptyCard);
        emptyHint->setAlignment(Qt::AlignCenter);
        emptyHint->setWordWrap(true);
        emptyHint->setStyleSheet("color: #666;");
        emptyLayout->addWidget(emptyTitle);
        emptyLayout->addWidget(emptyHint);
        layout->addWidget(emptyCard);
    } else {
        QGraphicsView* view = new QGraphicsView(&dialog);
        QGraphicsScene* scene = new QGraphicsScene(view);
        view->setScene(scene);
        view->setRenderHint(QPainter::Antialiasing);
        view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        view->setFrameShape(QFrame::NoFrame);

        int viewWidthLimit = 450;
        int xPos = 0;
        int yPos = 0;
        int rowHeight = 0;

        for (Space* space : m_ownedSpaces) {
            SpaceItem* item = new SpaceItem(space);
            item->setShowOwnerIndicator(false);
            item->setShowInvertedText(false);
            // Reset rotation to ensure all tiles render upright in the dialog
            item->setRotation(0);

            int itemW = item->boundingRect().width();
            int itemH = item->boundingRect().height();

            // Wrap to next line if necessary
            if (xPos + itemW > viewWidthLimit && xPos > 0) {
                xPos = 0;
                yPos += rowHeight + 15;
                rowHeight = 0;
            }

            item->setPos(xPos, yPos);
            scene->addItem(item);

            xPos += itemW + 15;
            if (itemH > rowHeight) {
                rowHeight = itemH;
            }
        }

        // Compute scene rect automatically based on items
        scene->setSceneRect(scene->itemsBoundingRect());
        view->setSceneRect(scene->sceneRect());
        layout->addWidget(view);
    }

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok, &dialog);
    for (QPushButton* button : buttonBox->findChildren<QPushButton*>()) {
        button->setCursor(Qt::PointingHandCursor);
    }
    QObject::connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    layout->addWidget(buttonBox);

    dialog.exec();
}

void PlayerInfoWidget::setActive(bool isActive)
{
    if (isActive) {
        this->setStyleSheet(
            "#PlayerCard {"
            "  background-color: #e6f7ff;"
            "  border-radius: 8px;"
            "  border: 2px solid #0055ff;"
            "  border-left: 10px solid " +
            m_playerColor.name() +
            ";"
            "}");
    } else {
        this->setStyleSheet(
            "#PlayerCard {"
            "  background-color: white;"
            "  border-radius: 8px;"
            "  border-left: 10px solid " +
            m_playerColor.name() +
            ";"
            "}");
    }
}
