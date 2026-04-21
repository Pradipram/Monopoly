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
#include "BoardWidget.h"
#include <QPainter>
#include <QPaintEvent>

#include <QCoreApplication>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), m_botCount(1)
{
    init_UI();
}

void MainWindow::init_UI(){
    setWindowTitle("Qt Monopoly");
    resize(1200, 800);

    QWidget *centralWidget = new QWidget(this);
    centralWidget->setObjectName("gameBoard");
    centralWidget->setAttribute(Qt::WA_StyledBackground, true);
    setCentralWidget(centralWidget);
    
    // Load absolute path to fix broken relative URL handling
    centralWidget->setStyleSheet(QString("#gameBoard { border-image: url(:/assets/images/background.jpg) 0 0 0 0 stretch stretch; }"));
    QVBoxLayout *menuLayout = new QVBoxLayout(centralWidget);
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
}

void MainWindow::startGame() {
    m_boardWidget = new BoardWidget(m_botCount, this);
    setCentralWidget(m_boardWidget);
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