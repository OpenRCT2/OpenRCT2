/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"

enum CURSOR_ID
{
    CURSOR_UNDEFINED = -1,
    CURSOR_ARROW,
    CURSOR_BLANK,
    CURSOR_UP_ARROW,
    CURSOR_UP_DOWN_ARROW,
    CURSOR_HAND_POINT,
    CURSOR_ZZZ,
    CURSOR_DIAGONAL_ARROWS,
    CURSOR_PICKER,
    CURSOR_TREE_DOWN,
    CURSOR_FOUNTAIN_DOWN,
    CURSOR_STATUE_DOWN,
    CURSOR_BENCH_DOWN,
    CURSOR_CROSS_HAIR,
    CURSOR_BIN_DOWN,
    CURSOR_LAMPPOST_DOWN,
    CURSOR_FENCE_DOWN,
    CURSOR_FLOWER_DOWN,
    CURSOR_PATH_DOWN,
    CURSOR_DIG_DOWN,
    CURSOR_WATER_DOWN,
    CURSOR_HOUSE_DOWN,
    CURSOR_VOLCANO_DOWN,
    CURSOR_WALK_DOWN,
    CURSOR_PAINT_DOWN,
    CURSOR_ENTRANCE_DOWN,
    CURSOR_HAND_OPEN,
    CURSOR_HAND_CLOSED,
    CURSOR_COUNT,
};

namespace OpenRCT2::Ui
{
    constexpr size_t CURSOR_BIT_WIDTH = 32;
    constexpr size_t CURSOR_HEIGHT    = 4;
    struct CursorData
    {
        struct HotSpot
        {
            int16_t X;
            int16_t Y;
        } HotSpot;
        uint8_t Data[CURSOR_BIT_WIDTH * CURSOR_HEIGHT];
        uint8_t Mask[CURSOR_BIT_WIDTH * CURSOR_HEIGHT];
    };
} // namespace OpenRCT2::Ui
