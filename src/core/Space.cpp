#include "Space.h"

Space::Space(const QString& name, SpaceType type, int price, int baseRent)
    : m_name(name)
    , m_type(type)
    , m_price(price)
    , m_baseRent(baseRent)
    , m_owner(nullptr)
{
}

QString Space::getName() const { return m_name; }
SpaceType Space::getType() const { return m_type; }

int Space::getPrice() const { return m_price; }
int Space::getRent() const { return m_baseRent; }

Player* Space::getOwner() const { return m_owner; }
void Space::setOwner(Player* owner) { m_owner = owner; }