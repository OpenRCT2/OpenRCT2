/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "QuarterTile.h"

#include "Location.hpp"

TileCornersZ QuarterTile::GetQuarterHeights(const int32_t height) const
{
    const uint8_t raisedQuarters = GetZQuarterOccupied();
    const int32_t northZ = height + (raisedQuarters & 0b0001 ? kLandHeightStep : 0);
    const int32_t eastZ = height + (raisedQuarters & 0b0010 ? kLandHeightStep : 0);
    const int32_t southZ = height + (raisedQuarters & 0b0100 ? kLandHeightStep : 0);
    const int32_t westZ = height + (raisedQuarters & 0b1000 ? kLandHeightStep : 0);
    return { northZ, eastZ, southZ, westZ };
}
