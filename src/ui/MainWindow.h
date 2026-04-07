#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>

class GameEngine;
class BoardWidget;
class QLabel;
class QTextEdit;
class QPushButton;
class QStackedWidget;
class QSpinBox;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void logMessage(const QString& msg);
    void updateStatus();
    void startGame();
    void addBot();
    void removeBot();

private:
    QStackedWidget *m_stackedWidget;
    QLabel *m_botCountLabel;
    int m_botCount;
    
    GameEngine *m_engine;
    BoardWidget *m_boardWidget;
    QLabel *m_statusLabel;
    QTextEdit *m_logArea;
    QPushButton *m_rollButton;
    QPushButton *m_endTurnButton;
    bool m_botTurnInProgress;
};

#endif // MAINWINDOW_H