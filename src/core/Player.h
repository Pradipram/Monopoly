#ifndef PLAYER_H
#define PLAYER_H

#include <QString>

class Player {
public:
    Player(const QString& name, bool isBot = false);

    QString getName() const;
    int getBalance() const;
    void addBalance(int amount);
    bool payAmount(int amount);

    int getPosition() const;
    void setPosition(int position);
    void move(int spaces, int boardSize);

    bool getIsBot() const;
    bool isInJail() const;
    void setInJail(bool inJail);

private:
    QString m_name;
    int m_balance;
    int m_position;
    bool m_isBot;
    bool m_inJail;
};

#endif // PLAYER_H