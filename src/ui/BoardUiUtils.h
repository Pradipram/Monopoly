#ifndef BOARDUIUTILS_H
#define BOARDUIUTILS_H

#include <QLabel>
#include <QPixmap>
#include <QPointF>

#include "const.h"

QPointF tileCenterForIndex(int index);
QPointF getTokenTargetPos(int playerId, QPointF tileCenter);
QPixmap createDiePixmap(int dieValue, int size);
void setDieFace(QLabel* dieLabel, int dieValue);

#endif  // BOARDUIUTILS_H
