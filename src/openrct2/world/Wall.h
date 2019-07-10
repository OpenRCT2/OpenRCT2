/*****************************************************************************
 * Copyright (c) 2014-2019 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "TileElement.h"

enum
{
    WALL_ANIMATION_FLAG_ACROSS_TRACK = (1 << 2),
    // 3 - 6 animation frame number
    WALL_ANIMATION_FLAG_DIRECTION_BACKWARD = (1 << 7),
    WALL_ANIMATION_FLAG_ALL_FLAGS = WALL_ANIMATION_FLAG_ACROSS_TRACK | WALL_ANIMATION_FLAG_DIRECTION_BACKWARD
};

money32 wall_remove(int16_t x, int16_t y, uint8_t baseHeight, uint8_t direction, uint8_t flags);
