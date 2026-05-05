/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "Footpath.h"
#include "Location.hpp"
#include "Map.h"

#include <vector>

namespace OpenRCT2
{
    struct FootpathDragPlacement
    {
        CoordsXYZ position;
        FootpathSlope slope;

        constexpr bool operator==(const FootpathDragPlacement& rhs) const
        {
            return position == rhs.position && slope == rhs.slope;
        }
    };

    /**
     * Build path placements that follow terrain with appropriate slopes.
     * Returns empty if no valid terrain (caller handles fallback).
     */
    std::vector<FootpathDragPlacement> calculateConnectedPathSlopes(MapRange range, CoordsXY dragStart);

} // namespace OpenRCT2
