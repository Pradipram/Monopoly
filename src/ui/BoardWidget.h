#ifndef BOARDWIDGET_H
#define BOARDWIDGET_H

#include <QWidget>

#include "GameEngine.h"

class BoardWidget : public QWidget {
    Q_OBJECT
public:
    explicit BoardWidget(int botCount, QWidget *parent = nullptr);

signals:

public slots:

protected:

private:
    int m_botCount;
    void init_UI();
    void init_board_UI();
    void init_info_UI();

    GameEngine *m_gameEngine;
};
#endif // BOARDWIDGET_H
