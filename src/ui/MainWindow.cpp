#include "MainWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTextEdit>
#include <QLabel>
#include <QTimer>
#include <QScrollBar>
#include <QStackedWidget>
#include <QSpinBox>
#include <QFont>
#include <QPixmap>
#include "../core/GameEngine.h"
#include "BoardWidget.h"
#include <QPainter>
#include <QPaintEvent>

class MenuBackgroundWidget : public QWidget {
public:
    explicit MenuBackgroundWidget(QWidget *parent = nullptr) : QWidget(parent) {
        m_bg.load("../assets/images/background.jpg");
    }
protected:
    void paintEvent(QPaintEvent *event) override {
        QPainter painter(this);
        if (!m_bg.isNull()) {
            QPixmap scaledBg = m_bg.scaled(size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
            int x = (width() - scaledBg.width()) / 2;
            int y = (height() - scaledBg.height()) / 2;
            painter.drawPixmap(x, y, scaledBg);
        } else {
            painter.fillRect(rect(), QColor("#222222"));
        }
    }
private:
    QPixmap m_bg;
};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), m_botTurnInProgress(false), m_botCount(1)
{
    setWindowTitle("Qt Monopoly");
    resize(1200, 800);

    m_stackedWidget = new QStackedWidget(this);
    setCentralWidget(m_stackedWidget);

    // ==========================================
    // Screen 1: Start Menu Setup
    // ==========================================
    MenuBackgroundWidget *menuWidget = new MenuBackgroundWidget(this);
    QVBoxLayout *menuLayout = new QVBoxLayout(menuWidget);
    menuLayout->setAlignment(Qt::AlignCenter);

    QLabel *titleLabel = new QLabel("QT MONOPOLY", this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(48);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("color: white; background-color: rgba(0,0,0,0.5); padding: 20px; border-radius: 10px;");
    menuLayout->addWidget(titleLabel);

    QHBoxLayout *botSelectionLayout = new QHBoxLayout();
    botSelectionLayout->setAlignment(Qt::AlignCenter);
    
    QPushButton *removeBotButton = new QPushButton("-", this);
    removeBotButton->setFixedSize(50, 50);
    removeBotButton->setStyleSheet("QPushButton { font-size: 30px; font-weight: bold; background-color: #f44336; color: white; border: none; border-radius: 25px; } QPushButton:hover { background-color: #d32f2f; }");
    connect(removeBotButton, &QPushButton::clicked, this, &MainWindow::removeBot);
    botSelectionLayout->addWidget(removeBotButton);

    m_botCountLabel = new QLabel("1 Bot", this);
    m_botCountLabel->setAlignment(Qt::AlignCenter);
    m_botCountLabel->setMinimumWidth(150);
    m_botCountLabel->setStyleSheet("font-size: 24px; color: white; font-weight: bold; background-color: rgba(0,0,0,0.5); padding: 10px; border-radius: 10px;");
    botSelectionLayout->addWidget(m_botCountLabel);

    QPushButton *addBotButton = new QPushButton("+", this);
    addBotButton->setFixedSize(50, 50);
    addBotButton->setStyleSheet("QPushButton { font-size: 30px; font-weight: bold; background-color: #2196F3; color: white; border: none; border-radius: 25px; } QPushButton:hover { background-color: #0b7dda; }");
    connect(addBotButton, &QPushButton::clicked, this, &MainWindow::addBot);
    botSelectionLayout->addWidget(addBotButton);
    
    menuLayout->addLayout(botSelectionLayout);

    QPushButton *startButton = new QPushButton("Start Game", this);
    startButton->setFixedSize(250, 60);
    startButton->setStyleSheet("QPushButton { font-size: 20px; padding: 15px; background-color: #4CAF50; color: white; border: none; border-radius: 15px; } QPushButton:hover { background-color: #45a049; } QPushButton:pressed { background-color: #3e8e41; }");
    connect(startButton, &QPushButton::clicked, this, &MainWindow::startGame);
    menuLayout->addWidget(startButton, 0, Qt::AlignCenter);

    m_stackedWidget->addWidget(menuWidget); // Index 0

    // ==========================================
    // Screen 2: Game Board UI
    // ==========================================
    QWidget *gameWidget = new QWidget(this);
    QHBoxLayout *mainLayout = new QHBoxLayout(gameWidget);

    // Left Panel: Board
    m_engine = new GameEngine(this);
    m_boardWidget = new BoardWidget(m_engine, this);
    mainLayout->addWidget(m_boardWidget, 2); // Board gets 2/3 of space

    // Right Panel: Logs and Controls
    QVBoxLayout *rightLayout = new QVBoxLayout();
    mainLayout->addLayout(rightLayout, 1); // Right layout gets 1/3

    m_statusLabel = new QLabel("Initializing Game...", this);
    m_statusLabel->setWordWrap(true);
    rightLayout->addWidget(m_statusLabel);

    m_logArea = new QTextEdit(this);
    m_logArea->setReadOnly(true);
    rightLayout->addWidget(m_logArea);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    m_rollButton = new QPushButton("Roll Dice", this);
    buttonLayout->addWidget(m_rollButton);

    m_endTurnButton = new QPushButton("End Turn", this);
    buttonLayout->addWidget(m_endTurnButton);
    rightLayout->addLayout(buttonLayout);

    connect(m_engine, &GameEngine::messageCreated, this, &MainWindow::logMessage);
    connect(m_engine, &GameEngine::gameStateChanged, this, &MainWindow::updateStatus);

    connect(m_rollButton, &QPushButton::clicked, m_engine, &GameEngine::rollDiceAndMove);
    connect(m_endTurnButton, &QPushButton::clicked, m_engine, &GameEngine::endTurn);

    m_stackedWidget->addWidget(gameWidget); // Index 1

    // Show menu initially
    m_stackedWidget->setCurrentIndex(0);
}

void MainWindow::startGame() {
    m_engine->initializeGame(1, m_botCount);
    
    m_stackedWidget->setCurrentIndex(1); // Switch to Game Board view
    m_boardWidget->updateBoard();        // Force an initial draw
}

void MainWindow::addBot() {
    if (m_botCount < 7) {
        m_botCount++;
        m_botCountLabel->setText(QString("%1 Bots").arg(m_botCount));
    }
}

void MainWindow::removeBot() {
    if (m_botCount > 1) {
        m_botCount--;
        m_botCountLabel->setText(m_botCount == 1 ? "1 Bot" : QString("%1 Bots").arg(m_botCount));
    }
}

void MainWindow::logMessage(const QString& msg) {
    m_logArea->append(msg);
    m_logArea->verticalScrollBar()->setValue(m_logArea->verticalScrollBar()->maximum());
}

void MainWindow::updateStatus() {
    m_boardWidget->updateBoard(); // Redraw board any time game updates 

    if (auto player = m_engine->getCurrentPlayer()) {
        m_statusLabel->setText(QString("Current Turn: <b style='color:blue;'>%1</b><br> Balance: $%2<br> Position: %3<br> Is Bot: %4")
            .arg(player->getName())
            .arg(player->getBalance())
            .arg(player->getPosition())
            .arg(player->getIsBot() ? "Yes" : "No"));

        bool isBot = player->getIsBot();
        m_rollButton->setEnabled(!isBot);
        m_endTurnButton->setEnabled(!isBot);

        if (isBot && !m_botTurnInProgress) {
            m_botTurnInProgress = true;
            
            QTimer::singleShot(800, this, [this]() {
                m_engine->rollDiceAndMove();
                
                QTimer::singleShot(800, this, [this]() {
                    m_botTurnInProgress = false;
                    m_engine->endTurn();
                });
            });
        }
    }
}