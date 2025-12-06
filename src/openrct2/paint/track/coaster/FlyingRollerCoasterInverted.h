/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <cstdint>

struct PaintSession;
struct Ride;
struct SupportType;

namespace OpenRCT2
{
    struct TrackElement;

    void FlyingRCTrackHalfLoopInvertedUp(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const OpenRCT2::TrackElement& trackElement, SupportType supportType);
    void FlyingRCTrackLeftFlyingLargeHalfLoopInvertedUp(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const OpenRCT2::TrackElement& trackElement, SupportType supportType);
    void FlyingRCTrackRightFlyingLargeHalfLoopInvertedUp(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const OpenRCT2::TrackElement& trackElement, SupportType supportType);
} // namespace OpenRCT2
