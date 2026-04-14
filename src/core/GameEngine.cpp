#include <QFile>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

#include "GameEngine.h"

GameEngine::GameEngine()
{
    qDebug() << "Initializing Game Engine...";
    loadBoardData();
}

GameEngine::~GameEngine()
{
    qDeleteAll(m_spaces);
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

    // 3. Loop through every item in the JSON file
    for (const QJsonValue &value : jsonArray) {
        QJsonObject obj = value.toObject();

        int index = obj["index"].toInt();
        QString name = obj["name"].toString();
        // SpaceType type = Space::stringToType(obj["type"].toString());
        // SpaceType type = static_cast<SpaceType>(obj["type"].toInt());
        SpaceType type = static_cast<SpaceType>(obj["type"].toInt());
        int price = obj["price"].toInt();
        QString colorGroup = obj["colorGroup"].toString();
        int houseCost = obj["houseBuildCost"].toInt();

        // Extract the rent array
        QList<int> rentList;
        QJsonArray rentArray = obj["rent"].toArray();
        for (const QJsonValue &rentVal : rentArray) {
            rentList.append(rentVal.toInt());
        }

        // 4. Create the Logic Object and add it to our list
        Space *newSpace = new Space(index, name, type, price, rentList, houseCost, colorGroup);
        m_spaces.append(newSpace);
    }
    
    qDebug() << "Successfully loaded" << m_spaces.size() << "spaces into memory!";
}

const QList<Space*>& GameEngine::getSpacesList()
{
    return m_spaces;
}