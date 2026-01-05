/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <cstdint>

namespace OpenRCT2
{
    enum class ObjectSourceGame : uint8_t
    {
        custom,
        wackyWorlds,
        timeTwister,
        openRCT2Official,
        rct1,
        addedAttractions,
        loopyLandscapes,
        rct2 = 8,
    };
}
