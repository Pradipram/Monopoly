#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include <QList>

#include "Space.h"

class GameEngine
{
    public:
        GameEngine();
        ~GameEngine();

        void loadBoardData();
        const QList<Space*>& getSpacesList();
    private:
    QList<Space*> m_spaces;
};

#endif // GAMEENGINE_H