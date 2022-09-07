/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
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
    CoordsXY length{};
    CoordsXY offset{};

    constexpr BoundBoxXY() = default;
    constexpr BoundBoxXY(CoordsXY _length, CoordsXY _offset)
        : length(_length)
        , offset(_offset)
    {
    }
};

struct BoundBoxXYZ
{
    CoordsXYZ length{};
    CoordsXYZ offset{};

    constexpr BoundBoxXYZ() = default;
    constexpr BoundBoxXYZ(CoordsXYZ _length, CoordsXYZ _offset)
        : length(_length)
        , offset(_offset)
    {
    }
};
