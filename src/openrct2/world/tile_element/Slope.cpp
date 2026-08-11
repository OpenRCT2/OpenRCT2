/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Slope.h"

#include "../../world/MapLimits.h"

#include <array>

namespace OpenRCT2
{
    // clang-format off
    static constexpr std::array<SlopeRelativeCornerHeights, kTileSlopeMask + 1> kSlopeRelativeCornerHeights = {{
        { 0, 0, 0, 0 },
        { 0, 0, 1, 0 },
        { 0, 0, 0, 1 },
        { 0, 0, 1, 1 },
        { 1, 0, 0, 0 },
        { 1, 0, 1, 0 },
        { 1, 0, 0, 1 },
        { 1, 0, 1, 1 },
        { 0, 1, 0, 0 },
        { 0, 1, 1, 0 },
        { 0, 1, 0, 1 },
        { 0, 1, 1, 1 },
        { 1, 1, 0, 0 },
        { 1, 1, 1, 0 },
        { 1, 1, 0, 1 },
        { 1, 1, 1, 1 },
        { 0, 0, 0, 0 },
        { 0, 0, 1, 0 },
        { 0, 0, 0, 1 },
        { 0, 0, 1, 1 },
        { 1, 0, 0, 0 },
        { 1, 0, 1, 0 },
        { 1, 0, 0, 1 },
        { 1, 0, 1, 2 },
        { 0, 1, 0, 0 },
        { 0, 1, 1, 0 },
        { 0, 1, 0, 1 },
        { 0, 1, 2, 1 },
        { 1, 1, 0, 0 },
        { 1, 2, 1, 0 },
        { 2, 1, 0, 1 },
        { 1, 1, 1, 1 },
    }};
    // clang-format on

    SlopeRelativeCornerHeights GetSlopeRelativeCornerHeights(const uint8_t slope)
    {
        return kSlopeRelativeCornerHeights[slope & kTileSlopeMask];
    }

    TileCornersZ GetSlopeCornerHeights(const int32_t height, const uint8_t slope)
    {
        const auto cornerHeights = GetSlopeRelativeCornerHeights(slope);
        const int32_t northZ = height + (cornerHeights.bottom * kLandHeightStep);
        const int32_t eastZ = height + (cornerHeights.left * kLandHeightStep);
        const int32_t southZ = height + (cornerHeights.top * kLandHeightStep);
        const int32_t westZ = height + (cornerHeights.right * kLandHeightStep);
        return { northZ, eastZ, southZ, westZ };
    }
} // namespace OpenRCT2
