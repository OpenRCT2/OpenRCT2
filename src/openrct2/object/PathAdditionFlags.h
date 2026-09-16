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
        isBin = 1 << 0,
        isBench = 1 << 1,
        breakable = 1 << 2,
        lamp = 1 << 3,
        jumpingFountainWater = 1 << 4,
        jumpingFountainSnow = 1 << 5,
        dontAllowOnQueue = 1 << 6,
        dontAllowOnSlope = 1 << 7,
        isQueueScreen = 1 << 8
    };
    using PathAdditionFlags = FlagHolder<uint16_t, PathAdditionFlag>;
}