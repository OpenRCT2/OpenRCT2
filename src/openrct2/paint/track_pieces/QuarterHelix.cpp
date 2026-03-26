/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "QuarterHelix.h"

namespace OpenRCT2
{
    const QuarterHelixBoundingBoxes kLeftQuarterHelixLargeUpBoundingBoxes = { {
        { {
            { { { { 0, 6, 1 }, { 32, 20, 1 } }, { { 0, 25, 3 }, { 32, 1, 27 } } } },
            { { kBoundingBoxUnimplemented, kBoundingBoxUnimplemented } },
            { { { { 0, 0, 1 }, { 32, 16, 1 } }, { { 0, 15, 1 }, { 32, 1, 29 } } } },
            { { { { 0, 16, 1 }, { 16, 16, 1 } }, { { 0, 16, 37 }, { 16, 16, 1 } } } },
            { { kBoundingBoxUnimplemented, kBoundingBoxUnimplemented } },
            { { { { 16, 0, 1 }, { 16, 32, 1 } }, { { 16, 0, 37 }, { 16, 32, 1 } } } },
            { { { { 6, 0, kCoordsZStep + 1 }, { 20, 32, 1 } }, { { 6, 0, kCoordsZStep + 29 }, { 20, 32, 1 } } } },
        } },
        { {
            { { { { 6, 0, 1 }, { 20, 32, 1 } }, { { 25, 0, 3 }, { 1, 32, 27 } } } },
            { { kBoundingBoxUnimplemented, kBoundingBoxUnimplemented } },
            { { { { 0, 0, 1 }, { 16, 32, 1 } }, { { 15, 16, 3 }, { 1, 12, 27 } } } },
            { { { { 16, 16, 1 }, { 16, 16, 1 } }, { { 16, 16, 37 }, { 16, 16, 1 } } } },
            { { kBoundingBoxUnimplemented, kBoundingBoxUnimplemented } },
            { { { { 0, 0, 1 }, { 32, 16, 1 } }, { { 16, 15, 3 }, { 12, 1, 35 } } } },
            { { { { 0, 6, kCoordsZStep + 1 }, { 32, 20, 1 } }, { { 0, 25, kCoordsZStep + 3 }, { 32, 1, 27 } } } },
        } },
        { {
            { { { { 0, 6, 1 }, { 32, 20, 1 } }, { { 0, 6, 29 }, { 32, 20, 1 } } } },
            { { kBoundingBoxUnimplemented, kBoundingBoxUnimplemented } },
            { { { { 0, 16, 1 }, { 32, 16, 1 } }, { { 0, 16, 29 }, { 32, 16, 1 } } } },
            { { { { 16, 0, 1 }, { 16, 16, 1 } }, { { 16, 0, 37 }, { 16, 16, 1 } } } },
            { { kBoundingBoxUnimplemented, kBoundingBoxUnimplemented } },
            { { { { 0, 0, 1 }, { 16, 32, 1 } }, { { 15, 0, 3 }, { 1, 32, 27 } } } },
            { { { { 6, 0, kCoordsZStep + 1 }, { 20, 32, 1 } }, { { 25, 0, kCoordsZStep + 3 }, { 1, 32, 27 } } } },
        } },
        { {
            { { { { 6, 0, 1 }, { 20, 32, 1 } }, { { 6, 0, 29 }, { 20, 32, 1 } } } },
            { { { { 0, 0, 1 }, { 16, 16, 1 } }, kBoundingBoxUnimplemented } },
            { { { { 16, 0, 1 }, { 16, 32, 1 } }, { { 16, 0, 29 }, { 16, 32, 1 } } } },
            { { { { 0, 0, 1 }, { 16, 16, 1 } }, { { 0, 0, 37 }, { 16, 16, 1 } } } },
            { { kBoundingBoxUnimplemented, kBoundingBoxUnimplemented } },
            { { { { 0, 16, 1 }, { 32, 16, 1 } }, { { 0, 16, 37 }, { 32, 16, 1 } } } },
            { { { { 0, 6, kCoordsZStep + 1 }, { 32, 20, 1 } }, { { 0, 6, kCoordsZStep + 29 }, { 32, 20, 1 } } } },
        } },
    } };
    const QuarterHelixBoundingBoxes kRightQuarterHelixLargeUpBoundingBoxes = flipTrackSequenceBoundBoxesXAxis(
        kLeftQuarterHelixLargeUpBoundingBoxes);

    const std::array<int8_t, kQuarterHelixSequenceCount> kQuarterHelixGeneralSupportHeights = { 32, 32, 32, 48, 48, 48, 48 };

    void trackPaintLeftQuarterHelixTunnels(
        PaintSession& session, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const TunnelGroup tunnelGroup)
    {
        if (trackSequence == 0 && direction == 0)
        {
            PaintUtilPushTunnelLeft(session, height, tunnelGroup, TunnelSubType::Flat);
        }
        else if (trackSequence == 0 && direction == 3)
        {
            PaintUtilPushTunnelRight(session, height, tunnelGroup, TunnelSubType::Flat);
        }
        else if (trackSequence == 6 && direction == 2)
        {
            PaintUtilPushTunnelRight(session, height + kLandHeightStep, tunnelGroup, TunnelSubType::FlatTo25Deg);
        }
        else if (trackSequence == 6 && direction == 3)
        {
            PaintUtilPushTunnelLeft(session, height + kLandHeightStep, tunnelGroup, TunnelSubType::FlatTo25Deg);
        }
    }

    void trackPaintRightQuarterHelixTunnels(
        PaintSession& session, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const TunnelGroup tunnelGroup)
    {
        if (trackSequence == 0 && direction == 0)
        {
            PaintUtilPushTunnelLeft(session, height, tunnelGroup, TunnelSubType::Flat);
        }
        else if (trackSequence == 0 && direction == 3)
        {
            PaintUtilPushTunnelRight(session, height, tunnelGroup, TunnelSubType::Flat);
        }
        else if (trackSequence == 6 && direction == 0)
        {
            PaintUtilPushTunnelRight(session, height + kLandHeightStep, tunnelGroup, TunnelSubType::FlatTo25Deg);
        }
        else if (trackSequence == 6 && direction == 1)
        {
            PaintUtilPushTunnelLeft(session, height + kLandHeightStep, tunnelGroup, TunnelSubType::FlatTo25Deg);
        }
    }
} // namespace OpenRCT2
