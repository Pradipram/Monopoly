#include "BoardUiUtils.h"

#include <QPainter>

QPointF tileCenterForIndex(int index)
{
    const qreal cornerHalf = Constants::CORNER_TILE_SIZE / 2.0;
    const qreal regHalf = Constants::REGULAR_TILE_SIZE / 2.0;

    if (index == 0) {
        return {Constants::MAX_BOARD_POS - cornerHalf, Constants::MAX_BOARD_POS - cornerHalf};
    }
    if (index > 0 && index < 10) {
        return {Constants::MAX_BOARD_POS - Constants::CORNER_TILE_SIZE - regHalf - ((index - 1) * Constants::REGULAR_TILE_SIZE), Constants::MAX_BOARD_POS - cornerHalf};
    }
    if (index == 10) {
        return {cornerHalf, Constants::MAX_BOARD_POS - cornerHalf};
    }
    if (index > 10 && index < 20) {
        return {cornerHalf, Constants::MAX_BOARD_POS - Constants::CORNER_TILE_SIZE - regHalf - ((index - 11) * Constants::REGULAR_TILE_SIZE)};
    }
    if (index == 20) {
        return {cornerHalf, cornerHalf};
    }
    if (index > 20 && index < 30) {
        return {Constants::CORNER_TILE_SIZE + regHalf + ((index - 21) * Constants::REGULAR_TILE_SIZE), cornerHalf};
    }
    if (index == 30) {
        return {Constants::MAX_BOARD_POS - cornerHalf, cornerHalf};
    }
    if (index > 30 && index < 40) {
        return {Constants::MAX_BOARD_POS - cornerHalf, Constants::CORNER_TILE_SIZE + regHalf + ((index - 31) * Constants::REGULAR_TILE_SIZE)};
    }

    return {};
}

QPointF getTokenTargetPos(int playerId, QPointF tileCenter)
{
    // Grid spacing logic: Arranges up to 9 tokens in a 3x3 grid
    int col = playerId % 3;
    int row = playerId / 3;

    double dx = (col - 1) * Constants::TOKEN_GRID_SPACING;  // Spacing of 25 pixels between columns
    double dy = (row - 1) * Constants::TOKEN_GRID_SPACING;  // Spacing of 25 pixels between rows

    return QPointF(tileCenter.x() + dx - Constants::TOKEN_OFFSET_X, 
                   tileCenter.y() + dy - Constants::TOKEN_OFFSET_Y);
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

void setDieFace(QLabel* dieLabel, int dieValue)
{
    dieLabel->setPixmap(createDiePixmap(dieValue, dieLabel->width()));
}
