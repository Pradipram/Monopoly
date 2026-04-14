#ifndef SPACE_H
#define SPACE_H

#include <QString>
#include <QList>

enum class SpaceType {
    Property,
    Chance,
    Tax,
    Railroad,
    Utility,
    Go,
    Jail,
    Parking,
    GoToJail
};

class Space {
    public:
        Space(uint8_t index, const QString &name, SpaceType type, int price, const QList<int> &rent, int houseBuildCost, const QString &colorGroup);
        ~Space();

        uint8_t index() const { return m_index; }
        SpaceType type() const { return m_type; }
        QString colorGroup() const { return m_colorGroup; }
        QString name() const { return m_name; }
        int price() const { return m_price; }
        QList<int> rent() const { return m_rent; }
        int houseBuildCost() const { return m_houseBuildCost; }

    private:
        uint8_t m_index;
        QString m_name;
        SpaceType m_type;
        int m_price;
        QList<int> m_rent;
        int m_houseBuildCost;
        QString m_colorGroup;
};

#endif // SPACE_H