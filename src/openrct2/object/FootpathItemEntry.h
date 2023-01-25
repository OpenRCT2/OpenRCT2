/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "../interface/Cursors.h"

using ObjectEntryIndex = uint16_t;

enum class PathBitDrawType : uint8_t
{
    Light,
    Bin,
    Bench,
    JumpingFountain,
};

enum
{
    PATH_BIT_FLAG_IS_BIN = 1 << 0,
    PATH_BIT_FLAG_IS_BENCH = 1 << 1,
    PATH_BIT_FLAG_BREAKABLE = 1 << 2,
    PATH_BIT_FLAG_LAMP = 1 << 3,
    PATH_BIT_FLAG_JUMPING_FOUNTAIN_WATER = 1 << 4,
    PATH_BIT_FLAG_JUMPING_FOUNTAIN_SNOW = 1 << 5,
    PATH_BIT_FLAG_DONT_ALLOW_ON_QUEUE = 1 << 6,
    PATH_BIT_FLAG_DONT_ALLOW_ON_SLOPE = 1 << 7,
    PATH_BIT_FLAG_IS_QUEUE_SCREEN = 1 << 8
};

struct PathBitEntry
{
    StringId name;
    uint32_t image;
    uint16_t flags;
    PathBitDrawType draw_type;
    CursorID tool_id;
    money16 price;
    ObjectEntryIndex scenery_tab_id;
};
