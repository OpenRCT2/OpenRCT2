/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../tile_element/Segment.h"

#include <cstdint>

namespace OpenRCT2::BlockedSegments
{
    constexpr uint16_t kStraightFlat = EnumsToFlags(
        PaintSegment::centre, PaintSegment::topRightSide, PaintSegment::bottomLeftSide);
    constexpr uint16_t kDiagStraightFlat[] = {
        EnumsToFlags(
            PaintSegment::centre, PaintSegment::topRightSide, PaintSegment::bottomRightSide, PaintSegment::rightCorner),
        EnumsToFlags(PaintSegment::centre, PaintSegment::topRightSide, PaintSegment::topLeftSide, PaintSegment::topCorner),
        EnumsToFlags(
            PaintSegment::bottomLeftSide, PaintSegment::centre, PaintSegment::bottomCorner, PaintSegment::bottomRightSide),
        EnumsToFlags(PaintSegment::bottomLeftSide, PaintSegment::centre, PaintSegment::leftCorner, PaintSegment::topLeftSide),
    };
} // namespace OpenRCT2::BlockedSegments
