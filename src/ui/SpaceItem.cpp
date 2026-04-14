#include "SpaceItem.h"

#include <QDebug>
#include <QFont>
#include <QGraphicsSceneMouseEvent>

SpaceItem::SpaceItem(Space* spaceData, QGraphicsItem* parent)
    : QGraphicsItem(parent), m_spaceData(spaceData)
{
    // Update the sizes to fill the 1000x1000 board
    if (m_spaceData->index() % 10 == 0) {
        TILE_WIDTH = 140;
        TILE_HEIGHT = 140;
    } else {
        TILE_WIDTH = 80;
        TILE_HEIGHT = 140;
    }

    setupPositionAndRotation();
}

QRectF SpaceItem::boundingRect() const
{
    return QRectF(0, 0, TILE_WIDTH, TILE_HEIGHT);
}

void SpaceItem::drawIcon(QPainter* painter, QPixmap* iconToDraw)
{
    if (iconToDraw != nullptr && !iconToDraw->isNull()) {
        QPixmap scaledIcon =
            iconToDraw->scaled(50, 50, Qt::KeepAspectRatio, Qt::SmoothTransformation);

        // Calculate center for 50x50 icon
        double imgX = (TILE_WIDTH - scaledIcon.width()) / 2.0;
        double imgY = (TILE_HEIGHT - scaledIcon.height()) / 2.0;

        painter->drawPixmap(imgX, imgY, scaledIcon);
    } else {
        qDebug() << "Warning: Could not load icon for" << m_spaceData->name();
    }
}

void SpaceItem::drawInvertedText(QPainter* painter)
{
    painter->save();
    painter->translate(TILE_WIDTH / 2.0, TILE_HEIGHT / 2.0);
    painter->rotate(180);
    painter->translate(-TILE_WIDTH / 2.0, -TILE_HEIGHT / 2.0);

    QRectF invertedTextRect(4, 4, TILE_WIDTH - 8, 40);
    painter->drawText(invertedTextRect, Qt::AlignTop | Qt::AlignHCenter | Qt::TextWordWrap,
                      m_spaceData->name());

    painter->restore();
}

void SpaceItem::drawInvertedTextwithPrice(QPainter* painter)
{
    painter->save();
    painter->translate(TILE_WIDTH / 2.0, TILE_HEIGHT / 2.0);
    painter->rotate(180);
    painter->translate(-TILE_WIDTH / 2.0, -TILE_HEIGHT / 2.0);

    QRectF invertedTextRect(4, 4, TILE_WIDTH - 8, 30);
    painter->drawText(invertedTextRect, Qt::AlignTop | Qt::AlignHCenter | Qt::TextWordWrap,
                      m_spaceData->name());

    QString priceText = QString("$%1").arg(m_spaceData->price());
    painter->drawText(0, 26, TILE_WIDTH, 15, Qt::AlignCenter, priceText);

    painter->restore();
}

void SpaceItem::drawGoSpace(QPainter* painter)
{
    painter->save();
    painter->translate(TILE_WIDTH / 2, TILE_HEIGHT / 2);
    painter->rotate(315);

    QFont goFont("Arial", 16, QFont::ExtraBold);
    painter->setFont(goFont);
    painter->drawText(QRectF(-TILE_WIDTH / 2.0, -40, TILE_WIDTH, 30), Qt::AlignCenter, "START");
    painter->drawText(QRectF(-TILE_WIDTH / 2.0, -10, TILE_WIDTH, 20), Qt::AlignCenter, "+$400");

    painter->rotate(45);

    // Draw thick filled red arrow pointing left
    QPen arrowPen(Qt::red, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    painter->setPen(arrowPen);
    painter->setBrush(Qt::red);

    int x1 = -TILE_WIDTH / 2 + 30;  // Tip of the arrow
    int y1 = TILE_HEIGHT / 2 - 20;  // Y center
    int ArrowLength = 80;
    int shaftThickness = 12;
    int headLength = 20;
    int headWidth = 26;

    // Shaft
    painter->drawRect(x1 + headLength, y1 - (shaftThickness / 2), ArrowLength - headLength,
                      shaftThickness);

    // Arrowhead
    QPolygonF arrowHead;
    arrowHead << QPointF(x1, y1) << QPointF(x1 + headLength, y1 - (headWidth / 2))
              << QPointF(x1 + headLength, y1 + (headWidth / 2));
    painter->drawPolygon(arrowHead);

    painter->restore();
}

void SpaceItem::drawPropertySpace(QPainter* painter, const QFont& font)
{
    SpaceConstants::SpaceColorGroup cGroup = m_spaceData->colorGroup();
    QColor groupColor((cGroup == SpaceConstants::SpaceColorGroup::None)
                          ? 0x808080
                          : static_cast<uint32_t>(cGroup));

    painter->setBrush(groupColor);
    painter->drawRect(0, 0, TILE_WIDTH, 25);

    painter->setFont(font);
    QRectF normalTextRect(4, 26, TILE_WIDTH - 8, 30);
    painter->drawText(normalTextRect, Qt::AlignTop | Qt::AlignHCenter | Qt::TextWordWrap,
                      m_spaceData->name());

    QString priceText = QString("$%1").arg(m_spaceData->price());
    painter->drawText(0, 48, TILE_WIDTH, 15, Qt::AlignCenter, priceText);

    drawInvertedTextwithPrice(painter);
}

void SpaceItem::drawPricedIconSpace(QPainter* painter, const QFont& font)
{
    QRectF textRect(4, 4, TILE_WIDTH - 8, 30);
    painter->setFont(font);
    painter->drawText(textRect, Qt::AlignTop | Qt::AlignHCenter | Qt::TextWordWrap,
                      m_spaceData->name());

    QString priceText = QString("$%1").arg(m_spaceData->price());
    painter->drawText(0, 25, TILE_WIDTH, 15, Qt::AlignCenter, priceText);

    QPixmap* iconToDraw = nullptr;
    switch (m_spaceData->type()) {
        case SpaceConstants::SpaceType::Tax: {
            static QPixmap icon(":/assets/images/tax.png");
            iconToDraw = &icon;
            break;
        }
        case SpaceConstants::SpaceType::WaterCompany: {
            static QPixmap icon(":/assets/images/water.png");
            iconToDraw = &icon;
            break;
        }
        case SpaceConstants::SpaceType::PowerCompany: {
            static QPixmap icon(":/assets/images/powerco.png");
            iconToDraw = &icon;
            break;
        }
        case SpaceConstants::SpaceType::Railroad: {
            static QPixmap icon(":/assets/images/railroad.png");
            iconToDraw = &icon;
            break;
        }
        default:
            break;
    }

    drawIcon(painter, iconToDraw);
    drawInvertedTextwithPrice(painter);
}

void SpaceItem::drawIconSpace(QPainter* painter, const QFont& font)
{
    QRectF textRect(4, 4, TILE_WIDTH - 8, 40);
    painter->setFont(font);
    painter->drawText(textRect, Qt::AlignTop | Qt::AlignHCenter | Qt::TextWordWrap,
                      m_spaceData->name());

    QPixmap* iconToDraw = nullptr;
    switch (m_spaceData->type()) {
        case SpaceConstants::SpaceType::Chance: {
            static QPixmap icon(":/assets/images/chance.png");
            iconToDraw = &icon;
            break;
        }
        case SpaceConstants::SpaceType::Parking: {
            static QPixmap icon(":/assets/images/parking.png");
            iconToDraw = &icon;
            break;
        }
        default:
            break;
    }

    drawIcon(painter, iconToDraw);
    drawInvertedText(painter);
}

void SpaceItem::drawCornerIconSpace(QPainter* painter, const QFont& font, const QString& imagePath)
{
    painter->save();
    painter->translate(TILE_WIDTH / 2.0, TILE_HEIGHT / 2.0);

    // Apply rotation based on the corner type so it faces inwards accurately
    if (m_spaceData->type() == SpaceConstants::SpaceType::Jail) {
        painter->rotate(45);
    } else if (m_spaceData->type() == SpaceConstants::SpaceType::Rest) {
        painter->rotate(135);
    } else if (m_spaceData->type() == SpaceConstants::SpaceType::GoToJail) {
        painter->rotate(225);
    }

    // Draw text rotated around the center pivot, moved towards the "top" (inside of board)
    QRectF textRect(-TILE_WIDTH / 2.0, -TILE_HEIGHT / 2.0 + 30, TILE_WIDTH, 40);
    painter->setFont(font);
    painter->drawText(textRect, Qt::AlignTop | Qt::AlignHCenter | Qt::TextWordWrap,
                      m_spaceData->name());

    // Draw icon centered below the text
    QPixmap icon(imagePath);
    if (!icon.isNull()) {
        QPixmap scaledIcon = icon.scaled(50, 50, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        double imgX = -scaledIcon.width() / 2.0;
        double imgY = -scaledIcon.height() / 2.0 + 20;  // Shift down slightly from true center
        painter->drawPixmap(imgX, imgY, scaledIcon);
    }

    painter->restore();
}

void SpaceItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    // 1. Draw the tile background and border
    painter->setBrush(Qt::white);
    painter->setPen(QPen(Qt::black, 2));
    painter->drawRect(boundingRect());

    QFont defaultFont("Arial", 10, QFont::Bold);

    // 2. Delegate rendering based on the space logic type
    switch (m_spaceData->type()) {
        case SpaceConstants::SpaceType::Go:
            drawGoSpace(painter);
            break;

        case SpaceConstants::SpaceType::Property:
            drawPropertySpace(painter, defaultFont);
            break;

        case SpaceConstants::SpaceType::Tax:
        case SpaceConstants::SpaceType::WaterCompany:
        case SpaceConstants::SpaceType::PowerCompany:
        case SpaceConstants::SpaceType::Railroad:
            drawPricedIconSpace(painter, defaultFont);
            break;

        case SpaceConstants::SpaceType::Chance:
        case SpaceConstants::SpaceType::Parking:
            drawIconSpace(painter, defaultFont);
            break;

        case SpaceConstants::SpaceType::GoToJail:
            drawCornerIconSpace(painter, defaultFont, ":/assets/images/push.png");
            break;

        case SpaceConstants::SpaceType::Rest:
            drawCornerIconSpace(painter, defaultFont, ":/assets/images/sleep.png");
            break;

        case SpaceConstants::SpaceType::Jail:
            drawCornerIconSpace(painter, defaultFont, ":/assets/images/prison.png");
            break;

        default:
            qDebug("Error: Processing Space '%s' with index %d falling back...",
                   qPrintable(m_spaceData->name()), m_spaceData->index());
            break;
    }
}

void SpaceItem::setupPositionAndRotation()
{
    int index = m_spaceData->index();

    // Set the pivot point to the exact center of whatever size the tile is
    setTransformOriginPoint(TILE_WIDTH / 2.0, TILE_HEIGHT / 2.0);

    if (index > 0 && index < 10) {
        setRotation(0);  // Bottom row faces up
    } else if (index > 10 && index < 20) {
        setRotation(90);  // Left row faces right
    } else if (index > 20 && index < 30) {
        setRotation(180);  // Top row faces down
    } else if (index > 30 && index < 40) {
        setRotation(270);  // Right row faces left
    }
}

void SpaceItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    qDebug() << "You clicked on:" << m_spaceData->name();
    event->accept();
}
