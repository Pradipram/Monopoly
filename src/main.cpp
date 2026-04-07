#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTextEdit>
#include <QLabel>
#include <QTimer>
#include <QScrollBar>
#include "core/GameEngine.h"
#include "ui/BoardWidget.h"

class MonopolyWindow : public QMainWindow {
    Q_OBJECT
public:
    MonopolyWindow(QWidget *parent = nullptr) : QMainWindow(parent), m_botTurnInProgress(false) {
        setWindowTitle("Qt Monopoly - Dev Build");
        resize(1200, 800);

        QWidget *centralWidget = new QWidget(this);
        setCentralWidget(centralWidget);

        QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);

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

        connect(m_engine, &GameEngine::messageCreated, this, &MonopolyWindow::logMessage);
        connect(m_engine, &GameEngine::gameStateChanged, this, &MonopolyWindow::updateStatus);

        connect(m_rollButton, &QPushButton::clicked, m_engine, &GameEngine::rollDiceAndMove);
        connect(m_endTurnButton, &QPushButton::clicked, m_engine, &GameEngine::endTurn);

        // Start with 1 human, 3 bots
        m_engine->initializeGame(1, 3);
    }

private slots:
    void logMessage(const QString& msg) {
        m_logArea->append(msg);
        m_logArea->verticalScrollBar()->setValue(m_logArea->verticalScrollBar()->maximum());
    }

    void updateStatus() {
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

private:
    GameEngine *m_engine;
    BoardWidget *m_boardWidget;
    QLabel *m_statusLabel;
    QTextEdit *m_logArea;
    QPushButton *m_rollButton;
    QPushButton *m_endTurnButton;
    bool m_botTurnInProgress;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    MonopolyWindow window;
    window.showMaximized();

    return app.exec();
}

#include "main.moc"