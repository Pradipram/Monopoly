#include "Player.h"

Player::Player(const QString& name, bool isBot)
    : m_name(name)
    , m_balance(1500) // Starting Monopoly money
    , m_position(0)   // Start at GO
    , m_isBot(isBot)
    , m_inJail(false)
{
}

QString Player::getName() const { return m_name; }
int Player::getBalance() const { return m_balance; }

void Player::addBalance(int amount) {
    m_balance += amount;
}

bool Player::payAmount(int amount) {
    if (m_balance >= amount) {
        m_balance -= amount;
        return true;
    }
    return false; // Cannot afford (trigger bankruptcy handling later)
}

int Player::getPosition() const { return m_position; }

void Player::setPosition(int position) {
    m_position = position;
}

void Player::move(int spaces, int boardSize) {
    m_position = (m_position + spaces) % boardSize;
}

bool Player::getIsBot() const { return m_isBot; }
bool Player::isInJail() const { return m_inJail; }
void Player::setInJail(bool inJail) { m_inJail = inJail; }