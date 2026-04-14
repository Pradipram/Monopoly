
#include "Space.h"

#include <QDebug>

Space::Space(uint8_t index, const QString& name, SpaceConstants::SpaceType type, int price,
             const QList<int>& rent, int houseBuildCost,
             const SpaceConstants::SpaceColorGroup colorGroup)
    : m_index(index),
      m_name(name),
      m_type(type),
      m_price(price),
      m_rent(rent),
      m_houseBuildCost(houseBuildCost),
      m_colorGroup(colorGroup)
{
    if (m_type == SpaceConstants::InvalidType || m_colorGroup == SpaceConstants::InvalidColor) {
        qFatal(
            "Error: Space '%s' with index %d has an invalid type or color group! Exiting game...",
            qPrintable(name), m_index);
    }
}

Space::~Space() {}