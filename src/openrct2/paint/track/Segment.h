/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../../ride/TrackData.h"
#include "../../world/tile_element/TrackElement.h"
#include "../Paint.h"
#include "../tile_element/Segment.h"

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

    enum class TrackType
    {
        Narrow,
        Inverted,
        Wide,
    };

    enum class TrainType
    {
        Narrow,
        SuspendedSwinging,
        Wide,
    };

    bool GetShouldInvertPrePostCall(const TrackElemType trackElemType, const uint8_t trackSequence, const TrackType trackType);

    void BlockSegmentsForTrackSequence(
        PaintSession& session, const uint8_t trackSequence, const Direction direction, const uint16_t height,
        const TrackElemType trackElemType, const TrackType trackType, const TrainType trainType);
} // namespace OpenRCT2::BlockedSegments
