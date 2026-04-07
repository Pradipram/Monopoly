#ifndef SPACE_H
#define SPACE_H

#include <QString>

enum class SpaceType {
    Go,
    Property,
    Railroad,
    Utility,
    Chance,
    CommunityChest,
    Tax,
    Jail,         // Just Visiting
    FreeParking,
    GoToJail
};

class Player; // Forward declaration

class Space {
public:
    Space(const QString& name, SpaceType type, int price = 0, int baseRent = 0);

    QString getName() const;
    SpaceType getType() const;
    
    int getPrice() const;
    int getRent() const; // Will eventually need to handle houses/hotels logic

    Player* getOwner() const;
    void setOwner(Player* owner);

private:
    QString m_name;
    SpaceType m_type;
    int m_price;
    int m_baseRent;
    Player* m_owner;
};

#endif // SPACE_H