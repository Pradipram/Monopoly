#ifndef SPACE_H
#define SPACE_H

#include <QList>
#include <QObject>
#include <QString>

class SpaceConstants : public QObject
{
    Q_OBJECT
   public:
    enum SpaceType {
        InvalidType = -1,
        Property = 0,
        Chance,
        Railroad,
        Jail,
        PowerCompany,
        Rest,
        WaterCompany,
        Tax,
        Go,
        Parking,
        GoToJail
    };
    Q_ENUM(SpaceType)  // This macro creates the magic

    enum SpaceColorGroup {
        InvalidColor = -1,
        None = 0xFFFFFF,
        YellowGreen = 0x9ACD32,
        Red = 0xFF0000,
        DeepSkyBlue = 0x00BFFF,
        Orange = 0xFFA500,
        DarkGreen = 0x006400,
        Indigo = 0x4B0082,
        Brown = 0xA52A2A,
        DarkJungleGreen = 0x1A2421
    };
    Q_ENUM(SpaceColorGroup)
};

class Space
{
   public:
    Space(uint8_t index, const QString& name, SpaceConstants::SpaceType type, int price,
          const QList<int>& rent, int houseBuildCost, SpaceConstants::SpaceColorGroup colorGroup);
    ~Space();

    uint8_t index() const
    {
        return m_index;
    }
    SpaceConstants::SpaceType type() const
    {
        return m_type;
    }
    SpaceConstants::SpaceColorGroup colorGroup() const
    {
        return m_colorGroup;
    }
    const QString& name() const
    {
        return m_name;
    }
    int price() const
    {
        return m_price;
    }
    const QList<int>& rent() const
    {
        return m_rent;
    }
    int houseBuildCost() const
    {
        return m_houseBuildCost;
    }

    uint8_t m_index;
    QString m_name;
    SpaceConstants::SpaceType m_type;
    int m_price;
    QList<int> m_rent;
    int m_houseBuildCost;
    SpaceConstants::SpaceColorGroup m_colorGroup;
    int ownerId;
    int m_houseCount;

   private:
};

#endif  // SPACE_H