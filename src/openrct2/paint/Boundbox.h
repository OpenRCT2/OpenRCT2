/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../world/Location.hpp"

#include <cstdint>

struct BoundBoxXY
{
    CoordsXY offset{};
    CoordsXY length{};

    constexpr BoundBoxXY() = default;
    constexpr BoundBoxXY(CoordsXY _offset, CoordsXY _length)
        : offset(_offset)
        , length(_length)
    {
    }
};

struct BoundBoxXYZ
{
    CoordsXYZ offset{};
    CoordsXYZ length{};

    constexpr BoundBoxXYZ() = default;
    constexpr BoundBoxXYZ(CoordsXYZ _offset, CoordsXYZ _length)
        : offset(_offset)
        , length(_length)
    {
    }
};
