
#include "Player.h"

Player::Player(int id, const QString& name, bool isBot)
    : m_name(name), m_id(id), m_isBot(isBot), m_cash(3000), m_landedSpaceIndex(0), m_netWorth(3000)
{
}