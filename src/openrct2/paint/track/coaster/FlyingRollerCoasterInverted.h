/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
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

using Direction = uint8_t;

namespace OpenRCT2
{
    struct TrackElement;
} // namespace OpenRCT2

namespace OpenRCT2::FlyingRCInverted
{
    void trackPaintHalfLoopInvertedUp(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
        const OpenRCT2::TrackElement& trackElement, SupportType supportType);
    void trackPaintLeftFlyingLargeHalfLoopInvertedUp(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
        const OpenRCT2::TrackElement& trackElement, SupportType supportType);
    void trackPaintRightFlyingLargeHalfLoopInvertedUp(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
        const OpenRCT2::TrackElement& trackElement, SupportType supportType);
} // namespace OpenRCT2::FlyingRCInverted
