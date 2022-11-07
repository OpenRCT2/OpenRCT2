/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <cstdint>

enum class TileElementType : uint8_t
{
    Surface = 0,
    Path = 1,
    Track = 2,
    SmallScenery = 3,
    Entrance = 4,
    Wall = 5,
    LargeScenery = 6,
    Banner = 7,
};
