/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../../core/EnumUtils.hpp"

#include <cstdint>

// This controls in which segment of a tile something is drawn.
// This is from a screen perspective, e.g. topCorner will always represent the part of top of the screen.
enum class PaintSegment : uint16_t
{
    topCorner = 0,
    topRightSide = 1,
    rightCorner = 2,
    bottomRightSide = 3,
    bottomCorner = 4,
    bottomLeftSide = 5,
    leftCorner = 6,
    topLeftSide = 7,
    centre = 8,
};
constexpr int32_t kSegmentsAll = EnumsToFlags(
    PaintSegment::topCorner, PaintSegment::leftCorner, PaintSegment::rightCorner, PaintSegment::bottomCorner,
    PaintSegment::centre, PaintSegment::topLeftSide, PaintSegment::topRightSide, PaintSegment::bottomLeftSide,
    PaintSegment::bottomRightSide);
