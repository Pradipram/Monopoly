#ifndef CONST_H
#define CONST_H

namespace Constants
{
// Property states
constexpr int UNOWNED = -1;

// Board UI
constexpr int MAX_BOARD_POS = 1000;
constexpr int CORNER_TILE_SIZE = 140;
constexpr int REGULAR_TILE_SIZE = 80;

// Token UI
constexpr double TOKEN_GRID_SPACING = 25.0;
constexpr double TOKEN_OFFSET_X = 12.0;
constexpr double TOKEN_OFFSET_Y = 12.0;

// Dice UI
constexpr int DICE_UI_SIZE = 72;

constexpr int START_SPACE_INDEX = 0;
}  // namespace Constants

#endif  // CONST_H