#ifndef BOARDWIDGET_H
#define BOARDWIDGET_H

#include <QWidget>
#include <QPaintEvent>
#include "../core/GameEngine.h"

class BoardWidget : public QWidget {
    Q_OBJECT
public:
    explicit BoardWidget(GameEngine* engine, QWidget *parent = nullptr);

public slots:
    void updateBoard();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    GameEngine* m_engine;
    QRectF getSpaceRect(int index, qreal w, qreal h) const;
    void drawSpace(QPainter& painter, int index, const QRectF& rect);
    void drawPlayers(QPainter& painter, qreal w, qreal h);
};

#endif // BOARDWIDGET_H