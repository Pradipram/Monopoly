#include "BoardWidget.h"
#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QColor>
#include <QFontMetrics>

BoardWidget::BoardWidget(GameEngine* engine, QWidget *parent)
    : QWidget(parent), m_engine(engine)
{
    setMinimumSize(600, 600);
}

void BoardWidget::updateBoard() {
    update(); // Schedule a repaint
}

QRectF BoardWidget::getSpaceRect(int index, qreal w, qreal h) const {
    qreal cellW = w / 11.0;
    qreal cellH = h / 11.0;
    int row = 0, col = 0;

    index = index % 40;

    if (index >= 0 && index < 10) {
        row = 10;
        col = 10 - index;
    } else if (index >= 10 && index < 20) {
        col = 0;
        row = 10 - (index - 10);
    } else if (index >= 20 && index < 30) {
        row = 0;
        col = index - 20;
    } else if (index >= 30 && index < 40) {
        col = 10;
        row = index - 30;
    }

    return QRectF(col * cellW, row * cellH, cellW, cellH);
}

void BoardWidget::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Fill background
    painter.fillRect(rect(), QColor("#DDE5D6")); // classic light green ish background

    qreal w = width();
    qreal h = height();

    const auto& board = m_engine->getBoard();
    if (board.empty()) return;

    // Draw the 40 spaces
    for (int i = 0; i < 40; ++i) {
        QRectF r = getSpaceRect(i, w, h);
        drawSpace(painter, i, r);
    }

    // Draw the inner center box outline
    qreal cellW = w / 11.0;
    qreal cellH = h / 11.0;
    painter.setPen(QPen(Qt::black, 2));
    painter.drawRect(cellW, cellH, w - 2 * cellW, h - 2 * cellH);
    
    // Draw Monoply title in center
    QFont titleFont = painter.font();
    titleFont.setPointSize(24);
    titleFont.setBold(true);
    painter.setFont(titleFont);
    painter.setPen(Qt::black);
    painter.drawText(QRectF(cellW, cellH, w - 2 * cellW, h - 2 * cellH), Qt::AlignCenter, "QT MONOPOLY");

    drawPlayers(painter, w, h);
}

void BoardWidget::drawSpace(QPainter& painter, int index, const QRectF& rect) {
    const auto& board = m_engine->getBoard();
    if (index >= static_cast<int>(board.size())) return;
    
    const auto& space = board[index];

    painter.setPen(QPen(Qt::black, 1));
    
    // Draw owner color block for properties
    if (space->getOwner() != nullptr) {
        painter.setBrush(QColor("#FFCCCC")); // Light red to indicate owned
    } else {
        painter.setBrush(Qt::white);
    }
    
    painter.drawRect(rect);

    painter.setPen(Qt::black);
    QFont f = painter.font();
    f.setPointSize(7);
    painter.setFont(f);

    QString text = space->getName();
    if (space->getType() == SpaceType::Property || space->getType() == SpaceType::Railroad) {
        text += QString("\n$%1").arg(space->getPrice());
        if (space->getOwner()) {
            text += QString("\nOwner: %1").arg(space->getOwner()->getName());
        }
    }

    painter.drawText(rect.adjusted(2, 2, -2, -2), Qt::AlignCenter | Qt::TextWordWrap, text);
}

void BoardWidget::drawPlayers(QPainter& painter, qreal w, qreal h) {
    const QColor colors[] = { Qt::red, Qt::blue, Qt::green, Qt::yellow, Qt::magenta, Qt::cyan };

    // To prevent overlap, keep track of how many players are on each space
    int offsets[40] = {0};

    const auto& players = m_engine->getPlayers();
    int pIndex = 0;
    for (const auto& player : players) {
        if (!player) continue;

        int pos = player->getPosition();
        if (pos >= 0 && pos < 40) {
            QRectF r = getSpaceRect(pos, w, h);
            
            // Draw a small circle for the player token
            qreal radius = qMin(r.width(), r.height()) / 6.0;
            
            // Calculate a staggered position based on how many players are already here
            int offset = offsets[pos]++;
            qreal dx = (offset % 2) * (radius * 2.2);
            qreal dy = (offset / 2) * (radius * 2.2);

            QRectF tokenRect(r.x() + 5 + dx, r.y() + 5 + dy, radius * 2, radius * 2);

            painter.setBrush(colors[pIndex % 6]);
            
            if (player.get() == m_engine->getCurrentPlayer()) {
                painter.setPen(QPen(Qt::white, 2));
            } else {
                painter.setPen(QPen(Qt::black, 1));
            }
            
            painter.drawEllipse(tokenRect);
        }
        pIndex++;
    }
}