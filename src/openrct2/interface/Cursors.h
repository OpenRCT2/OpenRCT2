#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

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

#ifdef __cplusplus

namespace OpenRCT2 { namespace Ui
{
    struct CursorData
    {
        struct HotSpot
        {
            sint16 X;
            sint16 Y;
        } HotSpot;
        uint8 Data[32 * 4];
        uint8 Mask[32 * 4];
    };
} }

#endif
