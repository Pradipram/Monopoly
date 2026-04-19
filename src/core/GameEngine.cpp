#include "GameEngine.h"

#include <QDebug>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMetaEnum>

GameEngine::GameEngine(int localPlayerCount, int botCount)
    : m_localPlayerCount(localPlayerCount), m_botCount(botCount)
{
    qDebug() << "Initializing Game Engine...";
    loadBoardData();
    for (int i = 0; i < m_localPlayerCount; i++) {
        Player* newPlayer = new Player(i, QString("Player %1").arg(i + 1), false);
        m_players.append(newPlayer);
    }
    for (int i = 0; i < m_botCount; i++) {
        Player* newPlayer = new Player(i + m_localPlayerCount, QString("Bot %1").arg(i + 1), true);
        m_players.append(newPlayer);
    }
    m_currentPlayerTurn = 0;  // Start with the first player
}

GameEngine::~GameEngine()
{
    qDeleteAll(m_spaces);
    qDeleteAll(m_players);
}

void GameEngine::loadBoardData()
{
    // 1. Open the JSON file from the Qt Resource system
    // QFile file(":/board_spaces.json");
    QFile file(":/assets/board_spaces.json");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Failed to open board_spaces.json!";
        return;
    }

    // 2. Parse the JSON
    QByteArray rawData = file.readAll();
    QJsonDocument doc(QJsonDocument::fromJson(rawData));
    QJsonArray jsonArray = doc.array();

    QMetaEnum metaEnumSpaceType = QMetaEnum::fromType<SpaceConstants::SpaceType>();
    QMetaEnum metaEnumColorGroup = QMetaEnum::fromType<SpaceConstants::SpaceColorGroup>();

    // 3. Loop through every item in the JSON file
    for (const QJsonValue& value : jsonArray) {
        QJsonObject obj = value.toObject();

        int index = obj["index"].toInt();
        QString name = obj["name"].toString();

        // Parse "type" from a String into our SpaceType Enum
        SpaceConstants::SpaceType type = static_cast<SpaceConstants::SpaceType>(
            metaEnumSpaceType.keyToValue(obj["type"].toString().toUtf8().constData()));

        int price = obj["price"].toInt();
        // QString colorGroup = obj["colorGroup"].toString();
        SpaceConstants::SpaceColorGroup colorGroup = static_cast<SpaceConstants::SpaceColorGroup>(
            metaEnumColorGroup.keyToValue(obj["colorGroup"].toString().toUtf8().constData()));
        int houseCost = obj["houseBuildCost"].toInt();

        // Extract the rent array
        QList<int> rentList;
        QJsonArray rentArray = obj["rent"].toArray();
        for (const QJsonValue& rentVal : rentArray) {
            rentList.append(rentVal.toInt());
        }

        // 4. Create the Logic Object and add it to our list
        Space* newSpace = new Space(index, name, type, price, rentList, houseCost, colorGroup);
        m_spaces.append(newSpace);
    }

    qDebug() << "Successfully loaded" << m_spaces.size() << "spaces into memory!";
}

void GameEngine::advanceTurn(int steps)
{
    m_players[m_currentPlayerTurn]->m_landedSpaceIndex =
        (m_players[m_currentPlayerTurn]->m_landedSpaceIndex + steps) % m_spaces.size();
    // m_currentPlayerTurn = (m_currentPlayerTurn + 1) % m_players.size();
}

void GameEngine::buyLandedProperty()
{
    Player* currentPlayer = m_players[m_currentPlayerTurn];
    Space* landedSpace = m_spaces[currentPlayer->m_landedSpaceIndex];

    currentPlayer->m_cash -= landedSpace->price();
    currentPlayer->m_ownedSpaces.append(landedSpace);
    landedSpace->ownerId = currentPlayer->m_id;
}

const QList<Space*>& GameEngine::getSpacesList()
{
    return m_spaces;
}

void GameEngine::changePlayerTurn()
{
    m_currentPlayerTurn = (m_currentPlayerTurn + 1) % m_players.size();
}

void GameEngine::payRent()
{
    Player* currentPlayer = m_players[m_currentPlayerTurn];
    Space* landedSpace = m_spaces[currentPlayer->m_landedSpaceIndex];

    Player* ownerPlayer = m_players[landedSpace->ownerId];
    int houseCnt = landedSpace->m_houseCount;
    int rentAmount = landedSpace->rent().at(houseCnt);
    currentPlayer->m_cash -= rentAmount;
    ownerPlayer->m_cash += rentAmount;
}