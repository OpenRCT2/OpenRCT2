/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/FlagHolder.hpp"

namespace OpenRCT2
{
    enum class PathAdditionFlag : uint16_t
    {
        PATH_ADDITION_FLAG_IS_BIN = 1 << 0,
        PATH_ADDITION_FLAG_IS_BENCH = 1 << 1,
        PATH_ADDITION_FLAG_BREAKABLE = 1 << 2,
        PATH_ADDITION_FLAG_LAMP = 1 << 3,
        PATH_ADDITION_FLAG_JUMPING_FOUNTAIN_WATER = 1 << 4,
        PATH_ADDITION_FLAG_JUMPING_FOUNTAIN_SNOW = 1 << 5,
        PATH_ADDITION_FLAG_DONT_ALLOW_ON_QUEUE = 1 << 6,
        PATH_ADDITION_FLAG_DONT_ALLOW_ON_SLOPE = 1 << 7,
        PATH_ADDITION_FLAG_IS_QUEUE_SCREEN = 1 << 8
    };
    using PathAdditionFlags = FlagHolder<uint16_t, PathAdditionFlag>;
}