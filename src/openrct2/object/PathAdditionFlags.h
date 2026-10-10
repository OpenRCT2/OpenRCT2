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
    enum class PathAdditionFlag
    {
        isBin = 0,
        isBench = 1,
        breakable = 2,
        lamp = 3,
        jumpingFountainWater = 4,
        jumpingFountainSnow = 5,
        dontAllowOnQueue = 6,
        dontAllowOnSlope = 7,
        isQueueScreen = 8
    };
    using PathAdditionFlags = FlagHolder<uint16_t, PathAdditionFlag>;
} // namespace OpenRCT2
