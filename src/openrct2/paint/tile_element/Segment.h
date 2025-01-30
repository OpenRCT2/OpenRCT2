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
// This is from a screen perspective, e.g. top will always represent the part of top of the screen.
enum class PaintSegment : uint16_t
{
    top = 0,
    topRight = 1,
    right = 2,
    bottomRight = 3,
    bottom = 4,
    bottomLeft = 5,
    left = 6,
    topLeft = 7,
    centre = 8,
};
constexpr uint16_t kSegmentsNone = 0;
constexpr int32_t kSegmentsAll = EnumsToFlags(
    PaintSegment::top, PaintSegment::left, PaintSegment::right, PaintSegment::bottom, PaintSegment::centre,
    PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
constexpr uint16_t kSegmentsUnimplemented = kSegmentsAll;
