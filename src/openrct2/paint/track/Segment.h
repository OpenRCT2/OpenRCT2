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

#include <array>
#include <cstdint>

namespace OpenRCT2::BlockedSegments
{
    constexpr uint16_t kStraightFlat = EnumsToFlags(PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft);
    constexpr uint16_t kDiagStraightFlat[] = {
        EnumsToFlags(PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight, PaintSegment::right),
        EnumsToFlags(PaintSegment::centre, PaintSegment::topRight, PaintSegment::topLeft, PaintSegment::top),
        EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight),
        EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::left, PaintSegment::topLeft),
    };

    static constexpr std::array<uint16_t, 7> kLeftQuarterHelixLargeUp = {
        EnumsToFlags(
            PaintSegment::top, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::topLeft),
        0,
        EnumsToFlags(
            PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight,
            PaintSegment::bottomLeft),
        EnumsToFlags(
            PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight, PaintSegment::bottom,
            PaintSegment::top),
        0,
        EnumsToFlags(
            PaintSegment::left, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft,
            PaintSegment::bottomRight),
        EnumsToFlags(
            PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomRight,
            PaintSegment::bottomLeft),
    };

    static constexpr std::array<uint16_t, 7> kRightQuarterHelixLargeUp = {
        EnumsToFlags(
            PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft,
            PaintSegment::bottomRight),
        0,
        EnumsToFlags(
            PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft,
            PaintSegment::bottomRight),
        EnumsToFlags(
            PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::left,
            PaintSegment::right),
        0,
        EnumsToFlags(
            PaintSegment::left, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft,
            PaintSegment::bottomRight),
        EnumsToFlags(
            PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomRight,
            PaintSegment::bottomLeft),
    };

    static constexpr std::array<uint16_t, 7> kLeftQuarterTurn5TilesWide = {
        kSegmentsAll,
        EnumsToFlags(PaintSegment::right, PaintSegment::topRight, PaintSegment::bottomRight),
        EnumsToFlags(
            PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight,
            PaintSegment::bottomLeft),
        EnumsToFlags(
            PaintSegment::top, PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft,
            PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight),
        EnumsToFlags(PaintSegment::right, PaintSegment::topRight, PaintSegment::bottomRight),
        EnumsToFlags(
            PaintSegment::left, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft,
            PaintSegment::bottomRight),
        kSegmentsAll,
    };

    static constexpr std::array<uint16_t, 7> kRightQuarterTurn5TilesWide = {
        kSegmentsAll,
        EnumsToFlags(PaintSegment::top, PaintSegment::topLeft, PaintSegment::topRight),
        EnumsToFlags(
            PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft,
            PaintSegment::bottomRight),
        EnumsToFlags(
            PaintSegment::top, PaintSegment::left, PaintSegment::right, PaintSegment::centre, PaintSegment::topLeft,
            PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight),
        EnumsToFlags(PaintSegment::top, PaintSegment::topLeft, PaintSegment::topRight),
        EnumsToFlags(
            PaintSegment::left, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft,
            PaintSegment::bottomRight),
        kSegmentsAll,
    };
} // namespace OpenRCT2::BlockedSegments
