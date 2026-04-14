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
    // This function can be used to draw icons for different space types if needed
    if (iconToDraw != NULL && !iconToDraw->isNull()) {
        QPixmap scaledIcon =
            iconToDraw->scaled(50, 50, Qt::KeepAspectRatio, Qt::SmoothTransformation);

        // Math to calculate the exact center pixel for the image
        double imgX = (TILE_WIDTH - scaledIcon.width()) / 2.0;
        double imgY = (TILE_HEIGHT - scaledIcon.height()) / 2.0;

        // Draw the image
        painter->drawPixmap(imgX, imgY, scaledIcon);
    } else {
        qDebug() << "Warning: Could not load Chance image!";
    }
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

void SpaceItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    // 1. Draw the tile background and border
    painter->setBrush(Qt::white);
    painter->setPen(QPen(Qt::black, 2));
    painter->drawRect(boundingRect());

    QFont font("Arial", 10, QFont::Bold);

    switch (m_spaceData->type()) {
        case SpaceConstants::SpaceType::Go: {
            painter->save();
            painter->translate(TILE_WIDTH / 2, TILE_HEIGHT / 2);
            painter->rotate(315);
            QFont goFont("Arial", 16, QFont::ExtraBold);
            painter->setFont(goFont);
            // painter->drawText(boundingRect(), Qt::AlignCenter, "GO");
            painter->drawText(QRectF(-TILE_WIDTH / 2.0, -40, TILE_WIDTH, 30), Qt::AlignCenter,
                              "START");
            painter->drawText(QRectF(-TILE_WIDTH / 2.0, -10, TILE_WIDTH, 20), Qt::AlignCenter,
                              "+$400");

            painter->rotate(45);

            // Draw a thick filled red arrow pointing left
            QPen arrowPen(Qt::red, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
            painter->setPen(arrowPen);
            painter->setBrush(Qt::red);

            // Calculate arrow geometry
            int x1 = -TILE_WIDTH / 2 + 30;  // Tip of the arrow (left side)
            int y1 = TILE_HEIGHT / 2 - 20;  // Y center of arrow
            int ArrowLength = 80;
            int shaftThickness = 12;
            int headLength = 20;
            int headWidth = 26;

            // Draw shaft (body of arrow)
            painter->drawRect(x1 + headLength, y1 - (shaftThickness / 2), ArrowLength - headLength,
                              shaftThickness);

            // Draw filled arrowhead (pointing left)
            QPolygonF arrowHead;
            arrowHead << QPointF(x1, y1)                                  // pointy tip
                      << QPointF(x1 + headLength, y1 - (headWidth / 2))   // top back of head
                      << QPointF(x1 + headLength, y1 + (headWidth / 2));  // bottom back of head
            painter->drawPolygon(arrowHead);

            painter->restore();
            break;
        }

        case SpaceConstants::SpaceType::Property: {
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
            break;
        }

        case SpaceConstants::SpaceType::Tax:
        case SpaceConstants::SpaceType::WaterCompany:
        case SpaceConstants::SpaceType::PowerCompany:
        case SpaceConstants::SpaceType::Railroad: {
            QRectF textRect(4, 4, TILE_WIDTH - 8, 30);
            painter->setFont(font);
            painter->drawText(textRect, Qt::AlignTop | Qt::AlignHCenter | Qt::TextWordWrap,
                              m_spaceData->name());
            QString priceText = QString("$%1").arg(m_spaceData->price());
            painter->drawText(0, 25, TILE_WIDTH, 15, Qt::AlignCenter, priceText);

            QPixmap* iconToDraw = NULL;
            switch (m_spaceData->type()) {
                case SpaceConstants::SpaceType::Tax: {
                    static QPixmap taxIcon(":/assets/images/tax.png");
                    iconToDraw = &taxIcon;
                    break;
                }
                case SpaceConstants::SpaceType::WaterCompany: {
                    static QPixmap waterIcon(":/assets/images/water.png");
                    iconToDraw = &waterIcon;
                    break;
                }
                case SpaceConstants::SpaceType::PowerCompany: {
                    static QPixmap powerIcon(":/assets/images/powerco.png");
                    iconToDraw = &powerIcon;
                    break;
                }
                case SpaceConstants::SpaceType::Railroad: {
                    static QPixmap railroadIcon(":/assets/images/railroad.png");
                    iconToDraw = &railroadIcon;
                    break;
                }
                default: {
                    break;
                }
            }

            drawIcon(painter, iconToDraw);
            drawInvertedTextwithPrice(painter);
            break;
        }

        case SpaceConstants::SpaceType::Chance:
        case SpaceConstants::SpaceType::Parking: {
            QRectF textRect(4, 4, TILE_WIDTH - 8, 40);
            painter->setFont(font);
            painter->drawText(textRect, Qt::AlignTop | Qt::AlignHCenter | Qt::TextWordWrap,
                              m_spaceData->name());
            QPixmap* iconToDraw = NULL;
            switch (m_spaceData->type()) {
                case SpaceConstants::SpaceType::Chance: {
                    static QPixmap chanceIcon(":/assets/images/chance.png");
                    iconToDraw = &chanceIcon;
                    break;
                }
                case SpaceConstants::SpaceType::Parking: {
                    static QPixmap parkingIcon(":/assets/images/parking.png");
                    iconToDraw = &parkingIcon;
                    break;
                }
                default: {
                    break;
                }
            }

            drawIcon(painter, iconToDraw);

            painter->save();
            painter->translate(TILE_WIDTH / 2.0, TILE_HEIGHT / 2.0);
            painter->rotate(180);
            painter->translate(-TILE_WIDTH / 2.0, -TILE_HEIGHT / 2.0);
            QRectF invertedTextRect(4, 4, TILE_WIDTH - 8, 40);
            painter->drawText(invertedTextRect, Qt::AlignTop | Qt::AlignHCenter | Qt::TextWordWrap,
                              m_spaceData->name());
            painter->restore();
            break;
        }

        case SpaceConstants::SpaceType::GoToJail:
        case SpaceConstants::SpaceType::Rest:
        case SpaceConstants::SpaceType::Jail: {
            painter->save();
            painter->translate(TILE_WIDTH / 2.0, TILE_HEIGHT / 2.0);

            QPixmap* iconToDraw = NULL;
            switch (m_spaceData->type()) {
                case SpaceConstants::SpaceType::GoToJail: {
                    painter->rotate(225);
                    static QPixmap goToJailIcon(":/assets/images/push.png");
                    iconToDraw = &goToJailIcon;
                    break;
                }
                case SpaceConstants::SpaceType::Rest: {
                    painter->rotate(135);
                    static QPixmap restIcon(":/assets/images/sleep.png");
                    iconToDraw = &restIcon;
                    break;
                }
                case SpaceConstants::SpaceType::Jail: {
                    painter->rotate(45);
                    static QPixmap jailIcon(":/assets/images/prison.png");
                    iconToDraw = &jailIcon;
                    break;
                }
                default: {
                    break;
                }
            }
            painter->translate(-TILE_WIDTH / 2.0, -TILE_HEIGHT / 2.0);
            QRectF textRect(0, 25, TILE_WIDTH, 40);
            painter->setFont(font);
            painter->drawText(textRect, Qt::AlignTop | Qt::AlignHCenter | Qt::TextWordWrap,
                              m_spaceData->name());
            drawIcon(painter, iconToDraw);

            painter->restore();
            break;
        }

        default: {
            qDebug("Error: Space '%s' with index %d has an invalid type! Exiting game...",
                   qPrintable(m_spaceData->name()), m_spaceData->index());
            break;
        }
    }
}

void SpaceItem::setupPositionAndRotation()
{
    int index = m_spaceData->index();
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