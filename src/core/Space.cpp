
#include "Space.h"

Space::Space(uint8_t index, const QString &name, SpaceType type, int price, const QList<int> &rent, int houseBuildCost, const QString &colorGroup)
    : m_index(index), m_name(name), m_type(type), m_price(price), m_rent(rent), m_houseBuildCost(houseBuildCost), m_colorGroup(colorGroup)
{

}

Space::~Space()
{

}