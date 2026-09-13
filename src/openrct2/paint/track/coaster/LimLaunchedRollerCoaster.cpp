/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../../SpriteIds.h"
#include "../../../drawing/Drawing.h"
#include "../../../ride/RideData.h"
#include "../../../ride/TrackPaint.h"
#include "../../../world/tile_element/TrackElement.h"
#include "../../Paint.h"
#include "../../support/MetalSupports.h"
#include "../../tile_element/Paint.TileElement.h"
#include "../../tile_element/Segment.h"
#include "../../track/Segment.h"
#include "../../track/Support.h"
#include "../../track_pieces/QuarterHelix.h"

using namespace OpenRCT2;

static constexpr TunnelGroup kTunnelGroup = TunnelGroup::Standard;

static constexpr uint32_t LIM_LAUNCHED_RC_BLOCK_BRAKE_SW_NE_OPEN = 15020;
static constexpr uint32_t LIM_LAUNCHED_RC_BLOCK_BRAKE_NW_SE_OPEN = 15021;
static constexpr uint32_t LIM_LAUNCHED_RC_BLOCK_BRAKE_SW_NE_CLOSED = 15022;
static constexpr uint32_t LIM_LAUNCHED_RC_BLOCK_BRAKE_NW_SE_CLOSED = 15023;

static constexpr uint32_t kLimLauncherBlockBrakeImages[kNumOrthogonalDirections][2] = {
    { LIM_LAUNCHED_RC_BLOCK_BRAKE_SW_NE_OPEN, LIM_LAUNCHED_RC_BLOCK_BRAKE_SW_NE_CLOSED },
    { LIM_LAUNCHED_RC_BLOCK_BRAKE_NW_SE_OPEN, LIM_LAUNCHED_RC_BLOCK_BRAKE_NW_SE_CLOSED },
    { LIM_LAUNCHED_RC_BLOCK_BRAKE_SW_NE_OPEN, LIM_LAUNCHED_RC_BLOCK_BRAKE_SW_NE_CLOSED },
    { LIM_LAUNCHED_RC_BLOCK_BRAKE_NW_SE_OPEN, LIM_LAUNCHED_RC_BLOCK_BRAKE_NW_SE_CLOSED },
};

static constexpr std::array<std::array<int8_t, kNumOrthogonalDirections>, kQuarterHelixSequenceCount>
    kLeftQuarterHelixSupportHeights = { { { 4, 4, 2, 0 }, {}, {}, {}, {}, {}, { 4, 8, 8, 4 } } };
static constexpr std::array<std::array<int8_t, kNumOrthogonalDirections>, kQuarterHelixSequenceCount>
    kRightQuarterHelixSupportHeights = { { { 2, 0, 4, 4 }, {}, {}, {}, {}, {}, { 6, 8, 8, 4 } } };
static constexpr std::array<std::array<int8_t, kNumOrthogonalDirections>, kQuarterHelixSequenceCount>
    kLeftQuarterBankedHelixSupportHeights = { { { 2, 2, 5, 4 }, {}, {}, {}, {}, {}, { 7, 8, 6, 6 } } };
static constexpr std::array<std::array<int8_t, kNumOrthogonalDirections>, kQuarterHelixSequenceCount>
    kRightQuarterBankedHelixSupportHeights = { { { 4, 4, 4, 2 }, {}, {}, {}, {}, {}, { 6, 6, 6, 8 } } };

/** RCt2: 0x008A6D50, 0x008A6D60, 0x008A6D70 */
static void LimLaunchedRCTrackStation(
    PaintSession& session, const Ride& ride, [[maybe_unused]] uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    static constexpr ImageIndex imageIds[4] = {
        15018,
        15019,
        15018,
        15019,
    };

    if (trackElement.GetTrackType() == TrackElemType::endStation)
    {
        bool isClosed = trackElement.IsBrakeClosed();
        PaintAddImageAsParentRotated(
            session, direction, session.TrackColours.WithIndex(kLimLauncherBlockBrakeImages[direction][isClosed]),
            { 0, 0, height }, { { 0, 6, height + 3 }, { 32, 20, 1 } });
    }
    else
    {
        PaintAddImageAsParentRotated(
            session, direction, session.TrackColours.WithIndex(imageIds[direction]), { 0, 0, height },
            { { 0, 6, height + 3 }, { 32, 20, 1 } });
    }
    if (TrackPaintUtilDrawStation(session, ride, direction, height, trackElement, StationBaseType::b, 0))
    {
        DrawSupportsSideBySide(session, direction, height, session.SupportColours, supportType.metal);
    }
    else if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::centre, direction, 0, height, session.SupportColours);
    }
    TrackPaintUtilDrawStationTunnel(session, direction, height);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** RCt2: 0x008A65E0 */
static void LimLaunchedRCTrackLeftVerticalLoop(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15388), { 0, 6, height }, { 32, 20, 3 });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::centre, 8, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15396), { 0, 6, height }, { 32, 20, 3 });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::centre, 8, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15395), { 0, 6, height }, { 32, 20, 3 });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::centre, 8, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15403), { 0, 6, height }, { 32, 20, 7 });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::centre, 6, height, session.SupportColours);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft,
                        PaintSegment::topRight, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15389), { 0, 0, height }, { 32, 26, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15397), { 0, 14, height }, { 32, 2, 63 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15394), { 0, 6, height }, { 32, 26, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15402), { 0, 6, height }, { 32, 26, 3 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft,
                        PaintSegment::topRight, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15390), { 16, 0, height },
                        { { 16, 0, height }, { 3, 16, 119 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15398), { 12, 0, height },
                        { { 12, 0, height }, { 3, 16, 119 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15393), { 10, 16, height },
                        { { 10, 16, height }, { 4, 16, 119 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15401), { 16, 16, height },
                        { { 16, 16, height }, { 2, 16, 119 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 168);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15391), { 0, 0, height + 32 }, { 32, 16, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15399), { 0, 0, height + 32 }, { 32, 16, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15392), { 0, 16, height + 32 }, { 32, 16, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15400), { 0, 16, height + 32 }, { 32, 16, 3 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft,
                        PaintSegment::topRight, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 4:
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 5:
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15392), { 0, 16, height + 32 }, { 32, 16, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15400), { 0, 16, height + 32 }, { 32, 16, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15391), { 0, 0, height + 32 }, { 32, 16, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15399), { 0, 0, height + 32 }, { 32, 16, 3 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight,
                        PaintSegment::bottomLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 7:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15393), { 10, 16, height },
                        { { 10, 16, height }, { 4, 16, 119 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15401), { 16, 16, height },
                        { { 16, 16, height }, { 2, 16, 119 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15390), { 16, 0, height },
                        { { 16, 0, height }, { 3, 16, 119 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15398), { 12, 0, height },
                        { { 12, 0, height }, { 3, 16, 119 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 168);
            break;
        case 8:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15394), { 0, 6, height }, { 32, 26, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15402), { 0, 6, height }, { 32, 26, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15389), { 0, 0, height }, { 32, 26, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15397), { 0, 14, height }, { 32, 2, 63 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight,
                        PaintSegment::bottomLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 9:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15395), { 0, 6, height }, { 32, 20, 3 });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::centre, 8, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15403), { 0, 6, height }, { 32, 20, 7 });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::centre, 6, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15388), { 0, 6, height }, { 32, 20, 3 });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::centre, 8, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15396), { 0, 6, height }, { 32, 20, 3 });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::centre, 8, height, session.SupportColours);
                    break;
            }
            switch (direction)
            {
                case 1:
                    PaintUtilPushTunnelRight(session, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
                    break;
                case 2:
                    PaintUtilPushTunnelLeft(session, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight,
                        PaintSegment::bottomLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
    }
}

/** RCt2: 0x008A65F0 */
static void LimLaunchedRCTrackRightVerticalLoop(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15419), { 0, 6, height }, { 32, 20, 7 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15411), { 0, 6, height }, { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15412), { 0, 6, height }, { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15404), { 0, 6, height }, { 32, 20, 3 });
                    break;
            }
            MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::centre, 8, height, session.SupportColours);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15418), { 0, 6, height }, { 32, 26, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15410), { 0, 6, height }, { 32, 26, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15413), { 0, 14, height }, { 32, 2, 63 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15405), { 0, 0, height }, { 32, 26, 3 });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15417), { 16, 16, height },
                        { { 16, 16, height }, { 2, 16, 119 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15409), { 10, 16, height },
                        { { 10, 16, height }, { 4, 16, 119 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15414), { 12, 0, height },
                        { { 12, 0, height }, { 3, 16, 119 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15406), { 16, 0, height },
                        { { 16, 0, height }, { 2, 16, 119 } });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 168);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15416), { 0, 16, height + 32 }, { 32, 16, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15408), { 0, 16, height + 32 }, { 32, 16, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15415), { 0, 0, height + 32 }, { 32, 16, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15407), { 0, 0, height + 32 }, { 32, 16, 3 });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 4:
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 5:
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15415), { 0, 0, height + 32 }, { 32, 16, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15407), { 0, 0, height + 32 }, { 32, 16, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15416), { 0, 16, height + 32 }, { 32, 16, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15408), { 0, 16, height + 32 }, { 32, 16, 3 });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 7:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15414), { 12, 0, height },
                        { { 12, 0, height }, { 3, 16, 119 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15406), { 16, 0, height },
                        { { 16, 0, height }, { 2, 16, 119 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15417), { 16, 16, height },
                        { { 16, 16, height }, { 2, 16, 119 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15409), { 10, 16, height },
                        { { 10, 16, height }, { 4, 16, 119 } });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 168);
            break;
        case 8:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15413), { 0, 14, height }, { 32, 2, 63 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15405), { 0, 0, height }, { 32, 26, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15418), { 0, 6, height }, { 32, 26, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15410), { 0, 6, height }, { 32, 26, 3 });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 9:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15412), { 0, 6, height }, { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15404), { 0, 6, height }, { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15419), { 0, 6, height }, { 32, 20, 7 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15411), { 0, 6, height }, { 32, 20, 3 });
                    break;
            }
            MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::centre, 8, height, session.SupportColours);
            switch (direction)
            {
                case 1:
                    PaintUtilPushTunnelRight(session, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
                    break;
                case 2:
                    PaintUtilPushTunnelLeft(session, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
    }

    TrackPaintUtilRightVerticalLoopSegments(session, direction, trackSequence);
}

/** RCt2: 0x008A6D10 */
static void LimLaunchedRCTrackLeftTwistDownToUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15758), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15782), { 0, 0, height },
                        { { 0, 6, height + 28 }, { 32, 20, 0 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15761), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15785), { 0, 0, height },
                        { { 0, 6, height + 28 }, { 32, 20, 0 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15764), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15788), { 0, 0, height },
                        { { 0, 6, height + 28 }, { 32, 20, 0 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15767), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15791), { 0, 0, height },
                        { { 0, 6, height + 28 }, { 32, 20, 0 } });
                    break;
            }
            MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::centre, 0, height, session.SupportColours);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft,
                        PaintSegment::topRight, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15759), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15783), { 0, 0, height },
                        { { 0, 6, height + 28 }, { 32, 20, 0 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15762), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15786), { 0, 0, height },
                        { { 0, 6, height + 28 }, { 32, 20, 0 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15765), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15789), { 0, 0, height },
                        { { 0, 6, height + 28 }, { 32, 20, 0 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15768), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15792), { 0, 0, height },
                        { { 0, 6, height + 28 }, { 32, 20, 0 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft,
                        PaintSegment::topRight, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15760), { 0, 0, height },
                        { { 0, 6, height - 16 }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15784), { 0, 0, height },
                        { { 0, 6, height + 12 }, { 32, 20, 0 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15763), { 0, 0, height },
                        { { 0, 6, height - 16 }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15787), { 0, 0, height },
                        { { 0, 6, height + 12 }, { 32, 20, 0 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15766), { 0, 0, height },
                        { { 0, 6, height - 16 }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15790), { 0, 0, height },
                        { { 0, 6, height + 12 }, { 32, 20, 0 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15769), { 0, 0, height },
                        { { 0, 6, height - 16 }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15793), { 0, 0, height },
                        { { 0, 6, height + 12 }, { 32, 20, 0 } });
                    break;
            }
            switch (direction)
            {
                case 1:
                    PaintUtilPushTunnelRight(session, height - 32, kTunnelGroup, TunnelSubType::Tall);
                    break;
                case 2:
                    PaintUtilPushTunnelLeft(session, height - 32, kTunnelGroup, TunnelSubType::Tall);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft,
                        PaintSegment::topRight, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 16);
            break;
    }
}

/** RCt2: 0x008A6D20 */
static void LimLaunchedRCTrackRightTwistDownToUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15770), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15794), { 0, 0, height },
                        { { 0, 6, height + 28 }, { 32, 20, 0 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15773), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15797), { 0, 0, height },
                        { { 0, 6, height + 28 }, { 32, 20, 0 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15776), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15800), { 0, 0, height },
                        { { 0, 6, height + 28 }, { 32, 20, 0 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15779), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15803), { 0, 0, height },
                        { { 0, 6, height + 28 }, { 32, 20, 0 } });
                    break;
            }
            MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::centre, 0, height, session.SupportColours);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight,
                        PaintSegment::bottomLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15771), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15795), { 0, 0, height },
                        { { 0, 6, height + 28 }, { 32, 20, 0 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15774), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15798), { 0, 0, height },
                        { { 0, 6, height + 28 }, { 32, 20, 0 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15777), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15801), { 0, 0, height },
                        { { 0, 6, height + 28 }, { 32, 20, 0 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15780), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15804), { 0, 0, height },
                        { { 0, 6, height + 28 }, { 32, 20, 0 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight,
                        PaintSegment::bottomLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15772), { 0, 0, height },
                        { { 0, 6, height - 16 }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15796), { 0, 0, height },
                        { { 0, 6, height + 12 }, { 32, 20, 0 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15775), { 0, 0, height },
                        { { 0, 6, height - 16 }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15799), { 0, 0, height },
                        { { 0, 6, height + 12 }, { 32, 20, 0 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15778), { 0, 0, height },
                        { { 0, 6, height - 16 }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15802), { 0, 0, height },
                        { { 0, 6, height + 12 }, { 32, 20, 0 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15781), { 0, 0, height },
                        { { 0, 6, height - 16 }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15805), { 0, 0, height },
                        { { 0, 6, height + 12 }, { 32, 20, 0 } });
                    break;
            }
            switch (direction)
            {
                case 1:
                    PaintUtilPushTunnelRight(session, height - 32, kTunnelGroup, TunnelSubType::Tall);
                    break;
                case 2:
                    PaintUtilPushTunnelLeft(session, height - 32, kTunnelGroup, TunnelSubType::Tall);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight,
                        PaintSegment::bottomLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 16);
            break;
    }
}

/** RCt2: 0x008A6D30 */
static void LimLaunchedRCTrackLeftTwistUpToDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15766), { 0, 0, height },
                        { { 0, 6, height - 16 }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15790), { 0, 0, height },
                        { { 0, 6, height + 12 }, { 32, 20, 0 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15769), { 0, 0, height },
                        { { 0, 6, height - 16 }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15793), { 0, 0, height },
                        { { 0, 6, height + 12 }, { 32, 20, 0 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15760), { 0, 0, height },
                        { { 0, 6, height - 16 }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15784), { 0, 0, height },
                        { { 0, 6, height + 12 }, { 32, 20, 0 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15763), { 0, 0, height },
                        { { 0, 6, height - 16 }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15787), { 0, 0, height },
                        { { 0, 6, height + 12 }, { 32, 20, 0 } });
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 32, kTunnelGroup, TunnelSubType::Tall);
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight,
                        PaintSegment::bottomLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 16);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15765), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15789), { 0, 0, height },
                        { { 0, 6, height + 28 }, { 32, 20, 0 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15768), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15792), { 0, 0, height },
                        { { 0, 6, height + 28 }, { 32, 20, 0 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15759), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15783), { 0, 0, height },
                        { { 0, 6, height + 28 }, { 32, 20, 0 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15762), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15786), { 0, 0, height },
                        { { 0, 6, height + 28 }, { 32, 20, 0 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight,
                        PaintSegment::bottomLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15764), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15788), { 0, 0, height },
                        { { 0, 6, height + 28 }, { 32, 20, 0 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15767), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15791), { 0, 0, height },
                        { { 0, 6, height + 28 }, { 32, 20, 0 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15758), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15782), { 0, 0, height },
                        { { 0, 6, height + 28 }, { 32, 20, 0 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15761), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15785), { 0, 0, height },
                        { { 0, 6, height + 28 }, { 32, 20, 0 } });
                    break;
            }
            MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::centre, 0, height, session.SupportColours);
            switch (direction)
            {
                case 1:
                    PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
                    break;
                case 2:
                    PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight,
                        PaintSegment::bottomLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
    }
}

/** RCt2: 0x008A6D40 */
static void LimLaunchedRCTrackRightTwistUpToDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15778), { 0, 0, height },
                        { { 0, 6, height - 16 }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15802), { 0, 0, height },
                        { { 0, 6, height + 12 }, { 32, 20, 0 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15781), { 0, 0, height },
                        { { 0, 6, height - 16 }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15805), { 0, 0, height },
                        { { 0, 6, height + 12 }, { 32, 20, 0 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15772), { 0, 0, height },
                        { { 0, 6, height - 16 }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15796), { 0, 0, height },
                        { { 0, 6, height + 12 }, { 32, 20, 0 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15775), { 0, 0, height },
                        { { 0, 6, height - 16 }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15799), { 0, 0, height },
                        { { 0, 6, height + 12 }, { 32, 20, 0 } });
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 32, kTunnelGroup, TunnelSubType::Tall);
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft,
                        PaintSegment::topRight, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 16);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15777), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15801), { 0, 0, height },
                        { { 0, 6, height + 28 }, { 32, 20, 0 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15780), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15804), { 0, 0, height },
                        { { 0, 6, height + 28 }, { 32, 20, 0 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15771), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15795), { 0, 0, height },
                        { { 0, 6, height + 28 }, { 32, 20, 0 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15774), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15798), { 0, 0, height },
                        { { 0, 6, height + 28 }, { 32, 20, 0 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft,
                        PaintSegment::topRight, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15776), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15800), { 0, 0, height },
                        { { 0, 6, height + 28 }, { 32, 20, 0 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15779), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15803), { 0, 0, height },
                        { { 0, 6, height + 28 }, { 32, 20, 0 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15770), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15794), { 0, 0, height },
                        { { 0, 6, height + 28 }, { 32, 20, 0 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15773), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15797), { 0, 0, height },
                        { { 0, 6, height + 28 }, { 32, 20, 0 } });
                    break;
            }
            MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::centre, 0, height, session.SupportColours);
            switch (direction)
            {
                case 1:
                    PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
                    break;
                case 2:
                    PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft,
                        PaintSegment::topRight, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
    }
}

/** RCt2: 0x008A6CD0 */
static void LimLaunchedRCTrackLeftCorkscrewUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15734), { 0, 0, height },
                        { { 0, 6, height + 4 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15737), { 0, 0, height },
                        { { 0, 6, height + 4 }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15740), { 0, 0, height },
                        { { 0, 6, height + 4 }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15743), { 0, 0, height },
                        { { 0, 6, height + 4 }, { 32, 20, 3 } });
                    break;
            }

            TrackPaintUtilLeftCorkscrewUpSupports(session, direction, height);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15735), { 0, 0, height },
                        { { 6, 6, height + 10 }, { 20, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15738), { 0, 0, height },
                        { { 6, 6, height + 10 }, { 20, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15741), { 0, 0, height },
                        { { 6, 6, height + 10 }, { 20, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15744), { 0, 0, height },
                        { { 6, 6, height + 10 }, { 20, 20, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15736), { 0, 0, height },
                        { { 6, 0, height + 24 }, { 20, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15739), { 0, 0, height },
                        { { 6, 0, height + 24 }, { 20, 32, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15742), { 0, 0, height },
                        { { 6, 0, height + 24 }, { 20, 32, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15745), { 0, 0, height },
                        { { 6, 0, height + 24 }, { 20, 32, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft,
                        PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, supportType.metal, MetalSupportPlace::centre, 0, height + 35, session.SupportColours);

            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height + 8, kTunnelGroup, TunnelSubType::Flat);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height + 8, kTunnelGroup, TunnelSubType::Flat);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
    }
}

/** RCt2: 0x008A6CE0 */
static void LimLaunchedRCTrackRightCorkscrewUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15746), { 0, 0, height },
                        { { 0, 6, height + 4 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15749), { 0, 0, height },
                        { { 0, 6, height + 4 }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15752), { 0, 0, height },
                        { { 0, 6, height + 4 }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15755), { 0, 0, height },
                        { { 0, 6, height + 4 }, { 32, 20, 3 } });
                    break;
            }
            MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::centre, 0, height, session.SupportColours);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft,
                        PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15747), { 0, 0, height },
                        { { 6, 6, height + 10 }, { 20, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15750), { 0, 0, height },
                        { { 6, 6, height + 10 }, { 20, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15753), { 0, 0, height },
                        { { 6, 6, height + 10 }, { 20, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15756), { 0, 0, height },
                        { { 6, 6, height + 10 }, { 20, 20, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15748), { 0, 0, height },
                        { { 6, 0, height + 24 }, { 20, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15751), { 0, 0, height },
                        { { 6, 0, height + 24 }, { 20, 32, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15754), { 0, 0, height },
                        { { 6, 0, height + 24 }, { 20, 32, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15757), { 0, 0, height },
                        { { 6, 0, height + 24 }, { 20, 32, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft,
                        PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, supportType.metal, MetalSupportPlace::centre, 0, height + 35, session.SupportColours);

            switch (direction)
            {
                case 0:
                    PaintUtilPushTunnelRight(session, height + 8, kTunnelGroup, TunnelSubType::Flat);
                    break;
                case 1:
                    PaintUtilPushTunnelLeft(session, height + 8, kTunnelGroup, TunnelSubType::Flat);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
    }
}

/** RCt2: 0x008A6CF0 */
static void LimLaunchedRCTrackLeftCorkscrewDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    LimLaunchedRCTrackRightCorkscrewUp(
        session, ride, 2 - trackSequence, (direction + 1) & 3, height, trackElement, supportType);
}

/** RCt2: 0x008A6D00 */
static void LimLaunchedRCTrackRightCorkscrewDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    LimLaunchedRCTrackLeftCorkscrewUp(session, ride, 2 - trackSequence, (direction - 1) & 3, height, trackElement, supportType);
}

/** RCt2: 0x008A6D80 */
static void LimLaunchedRCTrackBrakes(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(15018), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
        case 1:
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(15019), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
    }
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::centre, 0, height, session.SupportColours);
    }
    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** RCt2: 0x008A6C10 */
static void LimLaunchedRCTrack90DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15702), { 0, 0, height },
                        { { 4, 6, height + 8 }, { 2, 20, 31 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15703), { 0, 0, height },
                        { { 24, 6, height + 8 }, { 2, 20, 31 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15704), { 0, 0, height },
                        { { 24, 6, height + 8 }, { 2, 20, 31 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15705), { 0, 0, height },
                        { { 4, 6, height + 8 }, { 2, 20, 31 } });
                    break;
            }
            PaintUtilSetVerticalTunnel(session, height + 32);
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 1:
            break;
    }
}

/** RCt2: 0x008A6C20 */
static void LimLaunchedRCTrack90DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    LimLaunchedRCTrack90DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** RCt2: 0x008A6C30 */
static void LimLaunchedRCTrack60DegUpTo90DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15694), { 0, 0, height },
                        { { 4, 6, height + 8 }, { 2, 20, 55 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15695), { 0, 0, height },
                        { { 24, 6, height }, { 2, 20, 55 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15696), { 0, 0, height },
                        { { 24, 6, height }, { 2, 20, 55 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15697), { 0, 0, height },
                        { { 4, 6, height + 8 }, { 2, 20, 55 } });
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
            }
            PaintUtilSetVerticalTunnel(session, height + 56);
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 1:
            break;
    }
}

/** RCt2: 0x008A6C40 */
static void LimLaunchedRCTrack90DegDownTo60DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    LimLaunchedRCTrack60DegUpTo90DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** RCt2: 0x008A6C50 */
static void LimLaunchedRCTrack90DegUpTo60DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(15698), { 0, 0, height },
                { { 4, 6, height + 8 }, { 2, 20, 48 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(15699), { 0, 0, height },
                { { 39, 6, height + 8 }, { 2, 20, 31 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(15700), { 0, 0, height },
                { { 39, 6, height + 8 }, { 2, 20, 31 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(15701), { 0, 0, height },
                { { 4, 6, height + 8 }, { 2, 20, 48 } });
            break;
    }
    switch (direction)
    {
        case 1:
            PaintUtilPushTunnelRight(session, height + 48, kTunnelGroup, TunnelSubType::SlopeEnd);
            break;
        case 2:
            PaintUtilPushTunnelLeft(session, height + 48, kTunnelGroup, TunnelSubType::SlopeEnd);
            break;
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 80);
}

/** RCt2: 0x008A6C60 */
static void LimLaunchedRCTrack60DegDownTo90DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15700), { 0, 0, height },
                        { { 39, 6, height + 8 }, { 2, 20, 31 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15701), { 0, 0, height },
                        { { 0, 6, height + 2 }, { 6, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15698), { 0, 0, height },
                        { { 0, 6, height + 2 }, { 6, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15699), { 0, 0, height },
                        { { 39, 6, height + 8 }, { 2, 20, 31 } });
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height + 48, kTunnelGroup, TunnelSubType::SlopeEnd);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 80);
            break;
        case 1:
            break;
    }
}

/** RCt2: 0x008A6C70 */
static void LimLaunchedRCTrack90DegToInvertedFlatQuarterLoopUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15722), { 0, 0, height },
                        { { 4, 6, height + 8 }, { 2, 20, 31 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15725), { 0, 0, height },
                        { { 24, 6, height + 8 }, { 2, 20, 31 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15728), { 0, 0, height },
                        { { 24, 6, height + 8 }, { 2, 20, 31 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15731), { 0, 0, height },
                        { { 4, 6, height + 8 }, { 2, 20, 31 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 88);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15723), { 0, 0, height },
                        { { -8, 6, height }, { 2, 20, 31 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15726), { 0, 0, height },
                        { { 24, 6, height + 8 }, { 2, 20, 63 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15729), { 0, 0, height },
                        { { 24, 6, height + 8 }, { 2, 20, 63 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15732), { 0, 0, height },
                        { { -8, 6, height }, { 2, 20, 31 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15724), { 0, 0, height },
                        { { 0, 6, height + 24 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15727), { 0, 0, height },
                        { { 24, 6, height + 8 }, { 2, 20, 31 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15730), { 0, 0, height },
                        { { 24, 6, height + 8 }, { 2, 20, 31 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15733), { 0, 0, height },
                        { { 0, 6, height + 24 }, { 32, 20, 3 } });
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
    }
}

/** RCt2: 0x008A6C80 */
static void LimLaunchedRCTrackInvertedFlatTo90DegQuarterLoopDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    LimLaunchedRCTrack90DegToInvertedFlatQuarterLoopUp(
        session, ride, 2 - trackSequence, direction, height, trackElement, supportType);
}

/** RCt2: 0x008A6D90 */
static void LimLaunchedRCTrackBlockBrakes(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    bool isClosed = trackElement.IsBrakeClosed();
    PaintAddImageAsParentRotated(
        session, direction, session.TrackColours.WithIndex(kLimLauncherBlockBrakeImages[direction][isClosed]), { 0, 0, height },
        { { 0, 6, height }, { 32, 20, 3 } });

    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::centre, 0, height, session.SupportColours);
    }
    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

static void LimLaunchedRCTrackLeftQuarterTurn190DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15706), { 0, 0, height },
                        { { 4, 6, height + 8 }, { 2, 20, 63 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15707), { 0, 0, height },
                        { { 4, 6, height + 8 }, { 2, 20, 63 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15715), { 0, 0, height },
                        { { 24, 6, height + 8 }, { 2, 20, 63 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15716), { 0, 0, height },
                        { { 24, 24, height + 8 }, { 2, 2, 63 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15709), { 0, 0, height },
                        { { 4, 6, height + 8 }, { 2, 20, 63 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15717), { 0, 0, height },
                        { { 24, 6, height + 8 }, { 2, 20, 63 } });
                    break;
            }
            PaintUtilSetVerticalTunnel(session, height + 96);
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 96);
            break;
        case 1:
            break;
    }
}

/** RCt2: 0x008A6CA0 */
static void LimLaunchedRCTrackRightQuarterTurn190DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15710), { 0, 0, height },
                        { { 4, 6, height + 8 }, { 2, 20, 63 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15718), { 0, 0, height },
                        { { 24, 6, height + 8 }, { 2, 20, 63 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15719), { 0, 0, height },
                        { { 24, 24, height + 8 }, { 2, 2, 63 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15712), { 0, 0, height },
                        { { 4, 6, height + 8 }, { 2, 20, 63 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15720), { 0, 0, height },
                        { { 24, 6, height + 8 }, { 2, 20, 63 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(15713), { 0, 0, height },
                        { { 4, 6, height + 8 }, { 2, 20, 63 } });
                    break;
            }
            PaintUtilSetVerticalTunnel(session, height + 96);
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 96);
            break;
        case 1:
            break;
    }
}

/** RCt2: 0x008A6CB0 */
static void LimLaunchedRCTrackLeftQuarterTurn190DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    LimLaunchedRCTrackRightQuarterTurn190DegUp(
        session, ride, trackSequence, (direction + 1) & 3, height, trackElement, supportType);
}

/** RCt2: 0x008A6CC0 */
static void LimLaunchedRCTrackRightQuarterTurn190DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    LimLaunchedRCTrackLeftQuarterTurn190DegUp(
        session, ride, trackSequence, (direction - 1) & 3, height, trackElement, supportType);
}

static void LimLaunchedRCTrackLeftBarrelRollUpToDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_BARREL_ROLL + 0),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_BARREL_ROLL + 1),
                        { 0, 0, height }, { { 0, 6, height + 28 }, { 32, 20, 0 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::rightCorner, 4, height + 1, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_BARREL_ROLL + 6),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_BARREL_ROLL + 7),
                        { 0, 0, height }, { { 0, 6, height + 28 }, { 32, 20, 0 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomCorner, 4, height + 1, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_BARREL_ROLL + 12),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_BARREL_ROLL + 13),
                        { 0, 0, height }, { { 0, 6, height + 28 }, { 32, 20, 0 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::leftCorner, 0, height + 1, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_BARREL_ROLL + 18),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_BARREL_ROLL + 19),
                        { 0, 0, height }, { { 0, 6, height + 28 }, { 32, 20, 0 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topCorner, 0, height, session.SupportColours);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft,
                        PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_BARREL_ROLL + 2),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_BARREL_ROLL + 3),
                        { 0, 0, height }, { { 0, 6, height + 28 }, { 32, 20, 0 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_BARREL_ROLL + 8),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_BARREL_ROLL + 9),
                        { 0, 0, height }, { { 0, 6, height + 28 }, { 32, 20, 0 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_BARREL_ROLL + 14),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_BARREL_ROLL + 15),
                        { 0, 0, height }, { { 0, 6, height + 28 }, { 32, 20, 0 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_BARREL_ROLL + 20),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_BARREL_ROLL + 21),
                        { 0, 0, height }, { { 0, 6, height + 28 }, { 32, 20, 0 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight,
                        PaintSegment::bottomLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_BARREL_ROLL + 4),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_BARREL_ROLL + 5),
                        { 0, 0, height }, { { 0, 6, height + 44 }, { 32, 20, 0 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_BARREL_ROLL + 10),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_BARREL_ROLL + 11),
                        { 0, 0, height }, { { 0, 6, height + 44 }, { 32, 20, 0 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_BARREL_ROLL + 16),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_BARREL_ROLL + 17),
                        { 0, 0, height }, { { 0, 6, height + 44 }, { 32, 20, 0 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_BARREL_ROLL + 22),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_BARREL_ROLL + 23),
                        { 0, 0, height }, { { 0, 6, height + 44 }, { 32, 20, 0 } });
                    break;
            }
            switch (direction)
            {
                case 1:
                    PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Tall);
                    break;
                case 2:
                    PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Tall);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft,
                        PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
    }
}

static void LimLaunchedRCTrackRightBarrelRollUpToDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_BARREL_ROLL + 24),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_BARREL_ROLL + 25),
                        { 0, 0, height }, { { 0, 6, height + 28 }, { 32, 20, 0 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topCorner, 0, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_BARREL_ROLL + 30),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_BARREL_ROLL + 31),
                        { 0, 0, height }, { { 0, 6, height + 28 }, { 32, 20, 0 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::rightCorner, 0, height + 1, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_BARREL_ROLL + 36),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_BARREL_ROLL + 37),
                        { 0, 0, height }, { { 0, 6, height + 28 }, { 32, 20, 0 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomCorner, 4, height + 1, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_BARREL_ROLL + 42),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_BARREL_ROLL + 43),
                        { 0, 0, height }, { { 0, 6, height + 28 }, { 32, 20, 0 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::leftCorner, 4, height + 1, session.SupportColours);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight,
                        PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_BARREL_ROLL + 26),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_BARREL_ROLL + 27),
                        { 0, 0, height }, { { 0, 6, height + 28 }, { 32, 20, 0 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_BARREL_ROLL + 32),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_BARREL_ROLL + 33),
                        { 0, 0, height }, { { 0, 6, height + 28 }, { 32, 20, 0 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_BARREL_ROLL + 38),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_BARREL_ROLL + 39),
                        { 0, 0, height }, { { 0, 6, height + 28 }, { 32, 20, 0 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_BARREL_ROLL + 44),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_BARREL_ROLL + 45),
                        { 0, 0, height }, { { 0, 6, height + 28 }, { 32, 20, 0 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft,
                        PaintSegment::topRight, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_BARREL_ROLL + 28),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_BARREL_ROLL + 29),
                        { 0, 0, height }, { { 0, 6, height + 44 }, { 32, 20, 0 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_BARREL_ROLL + 34),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_BARREL_ROLL + 35),
                        { 0, 0, height }, { { 0, 6, height + 44 }, { 32, 20, 0 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_BARREL_ROLL + 40),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_BARREL_ROLL + 41),
                        { 0, 0, height }, { { 0, 6, height + 44 }, { 32, 20, 0 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_BARREL_ROLL + 46),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_BARREL_ROLL + 47),
                        { 0, 0, height }, { { 0, 6, height + 44 }, { 32, 20, 0 } });
                    break;
            }
            switch (direction)
            {
                case 1:
                    PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Tall);
                    break;
                case 2:
                    PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Tall);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight,
                        PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
    }
}

static void LimLaunchedRCTrackLeftBarrelRollDownToUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    LimLaunchedRCTrackLeftBarrelRollUpToDown(
        session, ride, 2 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

static void LimLaunchedRCTrackRightBarrelRollDownToUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    LimLaunchedRCTrackRightBarrelRollUpToDown(
        session, ride, 2 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

static void LimLaunchedRCTrackHalfLoopUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_HALF_LOOP + 0),
                        { 0, 6, height }, { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_HALF_LOOP + 4),
                        { 0, 6, height }, { 32, 20, 11 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_HALF_LOOP + 8),
                        { 0, 6, height }, { 32, 20, 9 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_HALF_LOOP + 12),
                        { 0, 6, height }, { 32, 20, 7 });
                    break;
            }
            MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::centre, 8, height, session.SupportColours);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_HALF_LOOP + 1),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::centre, 20, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_HALF_LOOP + 5),
                        { 0, 14, height }, { { 28, 6, height }, { 3, 20, 63 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::centre, 15, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_HALF_LOOP + 9),
                        { 0, 6, height }, { { 28, 6, height }, { 3, 20, 63 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::centre, 16, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_HALF_LOOP + 13),
                        { 0, 6, height }, { 32, 20, 3 });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::centre, 16, height, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_HALF_LOOP + 2),
                        { 16, 0, height }, { 2, 16, 119 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_HALF_LOOP + 6),
                        { 12, 0, height }, { { 12, 0, height }, { 3, 16, 119 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_HALF_LOOP + 10),
                        { 10, 16, height }, { 4, 12, 119 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_HALF_LOOP + 14),
                        { 16, 16, height }, { { 15, 6, height }, { 2, 16, 119 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::left, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft,
                        PaintSegment::bottomLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 168);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_HALF_LOOP + 3),
                        { 0, 0, height + 32 }, { 32, 16, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_HALF_LOOP + 7),
                        { 0, 0, height + 32 }, { 32, 16, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_HALF_LOOP + 11),
                        { 0, 16, height + 32 }, { 32, 12, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_HALF_LOOP + 15),
                        { 0, 16, height + 32 }, { 32, 12, 3 });
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
    }
}

static void LimLaunchedRCTrackHalfLoopDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    LimLaunchedRCTrackHalfLoopUp(session, ride, 3 - trackSequence, direction, height, trackElement, supportType);
}

static void LimLaunchedRCTrackFlatTo60DegUpLongBase(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP + 0),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP + 4),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP + 8),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP + 12),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    break;
            }
            if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
            {
                MetalASupportsPaintSetup(
                    session, supportType.metal, MetalSupportPlace::centre, 3, height, session.SupportColours);
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP + 1),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP + 5),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP + 9),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP + 13),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    break;
            }
            if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
            {
                MetalASupportsPaintSetup(
                    session, supportType.metal, MetalSupportPlace::centre, 9, height, session.SupportColours);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP + 2),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP + 6),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP + 10),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP + 14),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    break;
            }
            if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
            {
                MetalASupportsPaintSetup(
                    session, supportType.metal, MetalSupportPlace::centre, 10, height, session.SupportColours);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP + 3),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP + 7),
                        { 0, 0, height }, { { 0, 27, height }, { 32, 1, 98 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP + 11),
                        { 0, 0, height }, { { 0, 27, height }, { 32, 1, 98 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP + 15),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    break;
            }
            if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
            {
                MetalASupportsPaintSetup(
                    session, supportType.metal, MetalSupportPlace::centre, 19, height, session.SupportColours);
            }
            switch (direction)
            {
                case 1:
                    PaintUtilPushTunnelRight(session, height + 24, kTunnelGroup, TunnelSubType::SlopeEnd);
                    break;
                case 2:
                    PaintUtilPushTunnelLeft(session, height + 24, kTunnelGroup, TunnelSubType::SlopeEnd);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 80);
            break;
    }
}
static void LimLaunchedRCTrack60DegUpToFlatLongBase(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP + 16),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP + 20),
                        { 0, 0, height }, { { 0, 27, height }, { 32, 1, 98 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP + 24),
                        { 0, 0, height }, { { 0, 27, height }, { 32, 1, 98 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP + 28),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    break;
            }
            if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
            {
                MetalASupportsPaintSetup(
                    session, supportType.metal, MetalSupportPlace::centre, 24, height, session.SupportColours);
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::SlopeStart);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 80);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP + 17),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP + 21),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP + 25),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP + 29),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    break;
            }
            if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
            {
                MetalASupportsPaintSetup(
                    session, supportType.metal, MetalSupportPlace::centre, 18, height, session.SupportColours);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 80);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP + 18),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP + 22),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP + 26),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP + 30),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    break;
            }
            if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
            {
                MetalASupportsPaintSetup(
                    session, supportType.metal, MetalSupportPlace::centre, 13, height, session.SupportColours);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP + 19),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP + 23),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP + 27),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP + 31),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    break;
            }
            if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
            {
                MetalASupportsPaintSetup(
                    session, supportType.metal, MetalSupportPlace::centre, 5, height, session.SupportColours);
            }
            switch (direction)
            {
                case 1:
                    PaintUtilPushTunnelRight(session, height + 8, kTunnelGroup, TunnelSubType::FlatTo25Deg);
                    break;
                case 2:
                    PaintUtilPushTunnelLeft(session, height + 8, kTunnelGroup, TunnelSubType::FlatTo25Deg);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 40);
            break;
    }
}

static void LimLaunchedRCTrackFlatTo60DegDownLongBase(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    LimLaunchedRCTrack60DegUpToFlatLongBase(
        session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

static void LimLaunchedRCTrack60DegDownToFlatLongBase(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    LimLaunchedRCTrackFlatTo60DegUpLongBase(
        session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

static void LimLaunchedRCTrackLeftLargeCorkscrewUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 0),
                        { 0, 0, height }, { { 0, 6, height + 4 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 5),
                        { 0, 0, height }, { { 0, 6, height + 4 }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 10),
                        { 0, 0, height }, { { 0, 6, height + 4 }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 15),
                        { 0, 0, height }, { { 0, 6, height + 4 }, { 32, 20, 3 } });
                    break;
            }

            MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::centre, 2, height, session.SupportColours);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight,
                        PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 40);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 1),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topRightSide, 34, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 6),
                        { 0, 0, height }, { { 0, 29, height }, { 26, 1, 32 } });
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomRightSide, 22, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 11),
                        { 0, 0, height }, { { 0, 6, height }, { 20, 20, 3 } });
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomLeftSide, 24, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 16),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topLeftSide, 16, height, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft,
                        PaintSegment::topRight, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 2),
                        { 0, 0, height }, { { 0, 31, height }, { 48, 1, 64 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 7),
                        { 0, 0, height }, { { 2, 2, height + 40 }, { 28, 28, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 12),
                        { 0, 0, height }, { { 2, 2, height + 48 }, { 28, 28, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 17),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 3:
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 4:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::left, PaintSegment::bottomLeft, PaintSegment::bottom, PaintSegment::topLeft,
                        PaintSegment::centre, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 3),
                        { 0, 0, height }, { { 2, 2, height + 50 }, { 28, 28, 0 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topLeftSide, 0, height + 56, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 8),
                        { 0, 0, height }, { { 2, 2, height + 50 }, { 28, 28, 0 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topRightSide, 0, height + 56, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 13),
                        { 0, 0, height }, { { 2, 2, height + 50 }, { 24, 28, 0 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomRightSide, 0, height + 56, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 18),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomLeftSide, 0, height + 56, session.SupportColours);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
        case 5:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 4),
                        { 0, 0, height }, { { 2, 2, height + 40 }, { 28, 28, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 9),
                        { 0, 0, height }, { { 2, 2, height + 40 }, { 28, 28, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 14),
                        { 0, 0, height }, { { 2, 2, height + 40 }, { 26, 28, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 19),
                        { 0, 0, height }, { { 2, 2, height + 40 }, { 26, 28, 1 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::left, PaintSegment::bottomLeft, PaintSegment::bottom, PaintSegment::topLeft,
                        PaintSegment::centre, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height + 8, kTunnelGroup, TunnelSubType::Flat);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height + 8, kTunnelGroup, TunnelSubType::Flat);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
    }
}

static void LimLaunchedRCTrackRightLargeCorkscrewUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 20),
                        { 0, 0, height }, { { 0, 6, height + 4 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 25),
                        { 0, 0, height }, { { 0, 6, height + 4 }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 30),
                        { 0, 0, height }, { { 0, 6, height + 4 }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 35),
                        { 0, 0, height }, { { 0, 6, height + 4 }, { 32, 20, 3 } });
                    break;
            }

            MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::centre, 2, height, session.SupportColours);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottomRight, PaintSegment::centre, PaintSegment::right, PaintSegment::topRight,
                        PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 40);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 21),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topRightSide, 16, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 26),
                        { 0, 0, height }, { { 0, 6, height }, { 20, 20, 3 } });
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomRightSide, 24, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 31),
                        { 0, 0, height }, { { 0, 29, height }, { 26, 1, 32 } });
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomLeftSide, 22, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 36),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topLeftSide, 34, height, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomRight,
                        PaintSegment::topRight, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 22),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 27),
                        { 0, 0, height }, { { 2, 2, height + 48 }, { 28, 28, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 32),
                        { 0, 0, height }, { { 2, 2, height + 48 }, { 28, 28, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 37),
                        { 0, 0, height }, { { 0, 31, height }, { 48, 1, 64 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomRight, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 3:
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 4:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::left, PaintSegment::bottomLeft, PaintSegment::bottom, PaintSegment::topLeft,
                        PaintSegment::centre, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 23),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomRightSide, 0, height + 56, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 28),
                        { 0, 0, height }, { { 2, 2, height + 50 }, { 24, 28, 0 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomLeftSide, 0, height + 56, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 33),
                        { 0, 0, height }, { { 2, 2, height + 50 }, { 28, 28, 0 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topLeftSide, 0, height + 56, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 38),
                        { 0, 0, height }, { { 2, 2, height + 50 }, { 28, 28, 0 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topRightSide, 0, height + 56, session.SupportColours);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
        case 5:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 24),
                        { 0, 0, height }, { { 2, 2, height + 40 }, { 26, 28, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 29),
                        { 0, 0, height }, { { 2, 2, height + 40 }, { 26, 28, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 34),
                        { 0, 0, height }, { { 2, 2, height + 40 }, { 28, 28, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 39),
                        { 0, 0, height }, { { 2, 2, height + 40 }, { 28, 28, 1 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::left, PaintSegment::bottomLeft, PaintSegment::bottom, PaintSegment::topLeft,
                        PaintSegment::centre, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    PaintUtilPushTunnelRight(session, height + 8, kTunnelGroup, TunnelSubType::Flat);
                    break;
                case 1:
                    PaintUtilPushTunnelLeft(session, height + 8, kTunnelGroup, TunnelSubType::Flat);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
    }
}

static void LimLaunchedRCTrackLeftLargeCorkscrewDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    LimLaunchedRCTrackRightLargeCorkscrewUp(
        session, ride, 5 - trackSequence, (direction + 1) & 3, height, trackElement, supportType);
}

static void LimLaunchedRCTrackRightLargeCorkscrewDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    LimLaunchedRCTrackLeftLargeCorkscrewUp(
        session, ride, 5 - trackSequence, (direction - 1) & 3, height, trackElement, supportType);
}

static void LimLaunchedRCTrackLeftMediumHalfLoopUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 0),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 5),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 10),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 15),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    break;
            }

            MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::centre, 7, height, session.SupportColours);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 1),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 6),
                        { 0, 0, height }, { { 0, 31, height }, { 0, 32, 64 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 11),
                        { 0, 0, height }, { { 0, 32, height }, { 40, 0, 64 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 16),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::left, PaintSegment::topLeft, PaintSegment::centre, PaintSegment::topRight,
                        PaintSegment::bottomLeft, PaintSegment::top),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 2),
                        { 0, 0, height }, { { 0, 0, height + 2 }, { 32, 32, 0 } });
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topLeftSide, 14, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 7),
                        { 0, 0, height }, { { 29, 0, height }, { 1, 32, 96 } });
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topRightSide, 16, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 12),
                        { 0, 0, height }, { { 31, 0, height }, { 1, 32, 96 } });
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomRightSide, 18, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 17),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 0 } });
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomLeftSide, 14, height, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::left, PaintSegment::topLeft, PaintSegment::centre, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 144);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 3),
                        { 0, 0, height }, { { 2, 0, height }, { 1, 32, 160 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 8),
                        { 0, 0, height }, { { 0, 0, height + 140 }, { 32, 32, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 13),
                        { 0, 0, height }, { { 29, 0, height }, { 1, 32, 160 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 18),
                        { 0, 0, height }, { { 0, 0, height }, { 1, 32, 160 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottom, PaintSegment::bottomRight, PaintSegment::centre, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 144);
            break;
        case 4:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 4),
                        { 0, 0, height }, { { 0, 2, height + 48 }, { 32, 32, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 9),
                        { 0, 0, height }, { { 0, 2, height + 48 }, { 32, 32, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 14),
                        { 0, 0, height }, { { 0, 0, height + 48 }, { 32, 32, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 19),
                        { 0, 0, height }, { { 0, 0, height + 48 }, { 32, 32, 1 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::topRight, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottom,
                        PaintSegment::bottomRight, PaintSegment::right),
                    direction),
                0xFFFF, 0);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height + 16, kTunnelGroup, TunnelSubType::Flat);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
    }
}

static void LimLaunchedRCTrackRightMediumHalfLoopUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 20),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 25),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 30),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 35),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    break;
            }

            MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::centre, 7, height, session.SupportColours);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 21),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 26),
                        { 0, 0, height }, { { 0, 32, height }, { 40, 0, 64 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 31),
                        { 0, 0, height }, { { 0, 31, height }, { 0, 32, 64 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 36),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::topRight, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottom,
                        PaintSegment::bottomRight, PaintSegment::right),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 22),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 0 } });
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomRightSide, 14, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 27),
                        { 0, 0, height }, { { 30, 16, height }, { 0, 32, 96 } });
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomLeftSide, 18, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 32),
                        { 0, 0, height }, { { 29, 0, height }, { 0, 32, 96 } });
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topLeftSide, 16, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 37),
                        { 0, 0, height }, { { 0, 0, height + 2 }, { 32, 32, 0 } });
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topRightSide, 14, height, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottom, PaintSegment::bottomRight, PaintSegment::centre, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 144);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 23),
                        { 0, 0, height }, { { 0, 0, height }, { 1, 32, 160 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 28),
                        { 0, 0, height }, { { 29, 16, height }, { 0, 16, 160 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 33),
                        { 0, 0, height }, { { 0, 0, height + 140 }, { 32, 32, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 38),
                        { 0, 0, height }, { { 2, 0, height }, { 1, 32, 160 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 144);
            break;
        case 4:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 24),
                        { 0, 0, height }, { { 0, 0, height + 48 }, { 32, 32, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 29),
                        { 0, 0, height }, { { 0, 0, height + 48 }, { 32, 32, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 34),
                        { 0, 0, height }, { { 0, 0, height + 48 }, { 32, 32, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 39),
                        { 0, 0, height }, { { 0, 0, height + 48 }, { 32, 32, 1 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::left, PaintSegment::topLeft, PaintSegment::centre, PaintSegment::topRight,
                        PaintSegment::bottomLeft, PaintSegment::top),
                    direction),
                0xFFFF, 0);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height + 16, kTunnelGroup, TunnelSubType::Flat);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
    }
}

static void LimLaunchedRCTrackLeftMediumHalfLoopDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    LimLaunchedRCTrackRightMediumHalfLoopUp(session, ride, 4 - trackSequence, direction, height, trackElement, supportType);
}

static void LimLaunchedRCTrackRightMediumHalfLoopDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    LimLaunchedRCTrackLeftMediumHalfLoopUp(session, ride, 4 - trackSequence, direction, height, trackElement, supportType);
}

static void LimLaunchedRCTrackLeftZeroGRollUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_ZERO_G_ROLL + 0),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_ZERO_G_ROLL + 1),
                        { 0, 0, height }, { { 0, 6, height + 28 }, { 32, 20, 1 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::centre, 3, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_ZERO_G_ROLL + 4),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_ZERO_G_ROLL + 5),
                        { 0, 0, height }, { { 0, 31, height }, { 32, 1, 32 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::centre, 6, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_ZERO_G_ROLL + 8),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::centre, 8, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_ZERO_G_ROLL + 12),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::centre, 14, height, session.SupportColours);
                    break;
            }

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 40);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_ZERO_G_ROLL + 2),
                        { 0, 0, height }, { { 0, 6, height + 28 }, { 32, 20, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_ZERO_G_ROLL + 6),
                        { 0, 0, height }, { { 0, 6, height + 28 }, { 32, 20, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_ZERO_G_ROLL + 9),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_ZERO_G_ROLL + 13),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::left, PaintSegment::topLeft, PaintSegment::top, PaintSegment::bottomLeft,
                        PaintSegment::centre, PaintSegment::topRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_ZERO_G_ROLL + 3),
                        { 0, 0, height }, { { 0, 6, height + 28 }, { 32, 20, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_ZERO_G_ROLL + 7),
                        { 0, 0, height }, { { 0, 6, height + 28 }, { 32, 20, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_ZERO_G_ROLL + 10),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_ZERO_G_ROLL + 11),
                        { 0, 0, height }, { { 0, 6, height + 33 }, { 32, 20, 0 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_ZERO_G_ROLL + 14),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_ZERO_G_ROLL + 15),
                        { 0, 0, height }, { { 0, 6, height + 33 }, { 32, 20, 0 } });
                    break;
            }
            switch (direction)
            {
                case 1:
                    PaintUtilPushTunnelRight(session, height + 8, kTunnelGroup, TunnelSubType::Flat);
                    break;
                case 2:
                    PaintUtilPushTunnelLeft(session, height + 8, kTunnelGroup, TunnelSubType::Flat);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::left, PaintSegment::topLeft, PaintSegment::top, PaintSegment::bottomLeft,
                        PaintSegment::centre, PaintSegment::topRight),
                    direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, supportType.metal, MetalSupportPlace::centre, 0, height + 38, session.SupportColours);
            PaintUtilSetGeneralSupportHeight(session, height + 40);
            break;
    }
}

static void LimLaunchedRCTrackRightZeroGRollUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_ZERO_G_ROLL + 16),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::centre, 14, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_ZERO_G_ROLL + 20),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::centre, 8, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_ZERO_G_ROLL + 24),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_ZERO_G_ROLL + 25),
                        { 0, 0, height }, { { 0, 31, height }, { 32, 1, 32 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::centre, 6, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_ZERO_G_ROLL + 28),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_ZERO_G_ROLL + 29),
                        { 0, 0, height }, { { 0, 6, height + 28 }, { 32, 20, 1 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::centre, 3, height, session.SupportColours);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 40);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_ZERO_G_ROLL + 17),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_ZERO_G_ROLL + 21),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_ZERO_G_ROLL + 26),
                        { 0, 0, height }, { { 0, 6, height + 28 }, { 32, 20, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_ZERO_G_ROLL + 30),
                        { 0, 0, height }, { { 0, 6, height + 28 }, { 32, 20, 1 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottom, PaintSegment::bottomRight, PaintSegment::right, PaintSegment::bottomLeft,
                        PaintSegment::centre, PaintSegment::topRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_ZERO_G_ROLL + 18),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_ZERO_G_ROLL + 19),
                        { 0, 0, height }, { { 0, 6, height + 33 }, { 32, 20, 0 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_ZERO_G_ROLL + 22),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_ZERO_G_ROLL + 23),
                        { 0, 0, height }, { { 0, 6, height + 33 }, { 32, 20, 0 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_ZERO_G_ROLL + 27),
                        { 0, 0, height }, { { 0, 6, height + 28 }, { 32, 20, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_ZERO_G_ROLL + 31),
                        { 0, 0, height }, { { 0, 6, height + 28 }, { 32, 20, 1 } });
                    break;
            }
            switch (direction)
            {
                case 1:
                    PaintUtilPushTunnelRight(session, height + 8, kTunnelGroup, TunnelSubType::Flat);
                    break;
                case 2:
                    PaintUtilPushTunnelLeft(session, height + 8, kTunnelGroup, TunnelSubType::Flat);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottom, PaintSegment::bottomRight, PaintSegment::right, PaintSegment::bottomLeft,
                        PaintSegment::centre, PaintSegment::topRight),
                    direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, supportType.metal, MetalSupportPlace::centre, 0, height + 38, session.SupportColours);
            PaintUtilSetGeneralSupportHeight(session, height + 40);
            break;
    }
}

static void LimLaunchedRCTrackLeftZeroGRollDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    LimLaunchedRCTrackLeftZeroGRollUp(session, ride, 2 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

static void LimLaunchedRCTrackRightZeroGRollDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    LimLaunchedRCTrackRightZeroGRollUp(
        session, ride, 2 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

static void LimLaunchedRCTrackLeftLargeZeroGRollUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 0),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 5),
                        { 0, 0, height }, { { 0, 30, height }, { 32, 0, 96 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 9),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 10), { 0, 0, height },
                        { { 0, 30, height }, { 32, 0, 96 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 15), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
            }
            MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::centre, 25, height, session.SupportColours);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 88);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 1),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 2),
                        { 0, 0, height }, { { 0, 30, height }, { 32, 0, 96 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 6),
                        { 0, 0, height }, { { 0, 30, height }, { 32, 0, 64 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 11), { 0, 0, height },
                        { { 0, 2, height }, { 32, 0, 64 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 16), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 3),
                        { 0, 0, height }, { { 0, 30, height }, { 32, 0, 64 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 7),
                        { 0, 0, height }, { { 0, 30, height }, { 32, 0, 48 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 12), { 0, 0, height },
                        { { 0, 2, height }, { 32, 0, 48 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 17), { 0, 0, height },
                        { { 0, 2, height }, { 32, 0, 32 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 18), { 0, 0, height },
                        { { 0, 6, height + 40 }, { 32, 20, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::left, PaintSegment::topLeft, PaintSegment::top, PaintSegment::bottomLeft,
                        PaintSegment::centre, PaintSegment::topRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::left, PaintSegment::topLeft, PaintSegment::top, PaintSegment::bottomLeft,
                        PaintSegment::centre, PaintSegment::topRight),
                    direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 4),
                        { 0, 0, height }, { { 0, 26, height }, { 26, 0, 20 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomLeftSide, 0, height + 28, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 8),
                        { 0, 0, height }, { { 0, 26, height }, { 26, 0, 20 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topLeftSide, 0, height + 28, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 13), { 0, 0, height },
                        { { 0, 2, height }, { 32, 0, 32 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 14), { 0, 0, height },
                        { { 0, 6, height + 40 }, { 32, 20, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topRightSide, 0, height + 28, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 19), { 0, 0, height },
                        { { 0, 18, height }, { 32, 10, 20 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomRightSide, 0, height + 28, session.SupportColours);
                    break;
            }
            switch (direction)
            {
                case 1:
                    PaintUtilPushTunnelRight(session, height + 8, kTunnelGroup, TunnelSubType::Flat);
                    break;
                case 2:
                    PaintUtilPushTunnelLeft(session, height + 8, kTunnelGroup, TunnelSubType::Flat);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 40);
            break;
    }
}

static void LimLaunchedRCTrackRightLargeZeroGRollUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 20), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 25), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 26), { 0, 0, height },
                        { { 0, 30, height }, { 32, 0, 96 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 31), { 0, 0, height },
                        { { 0, 30, height }, { 32, 0, 96 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 35), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
            }
            MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::centre, 25, height, session.SupportColours);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 88);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 21), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 27), { 0, 0, height },
                        { { 0, 2, height }, { 32, 0, 64 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 32), { 0, 0, height },
                        { { 0, 30, height }, { 32, 0, 64 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 36), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 37), { 0, 0, height },
                        { { 0, 30, height }, { 32, 0, 96 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 22), { 0, 0, height },
                        { { 0, 2, height }, { 32, 0, 32 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 23), { 0, 0, height },
                        { { 0, 6, height + 40 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 28), { 0, 0, height },
                        { { 0, 2, height }, { 32, 0, 48 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 33), { 0, 0, height },
                        { { 0, 30, height }, { 32, 0, 48 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 38), { 0, 0, height },
                        { { 0, 30, height }, { 32, 0, 64 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight,
                        PaintSegment::bottomLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight,
                        PaintSegment::bottomLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 24), { 0, 0, height },
                        { { 0, 18, height }, { 32, 10, 20 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomLeftSide, 0, height + 28, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 29), { 0, 0, height },
                        { { 0, 2, height }, { 32, 0, 32 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 30), { 0, 0, height },
                        { { 0, 6, height + 40 }, { 32, 20, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topLeftSide, 0, height + 28, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 34), { 0, 0, height },
                        { { 0, 26, height }, { 26, 0, 20 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topRightSide, 0, height + 28, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 39), { 0, 0, height },
                        { { 0, 26, height }, { 26, 0, 20 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomRightSide, 0, height + 28, session.SupportColours);
                    break;
            }
            switch (direction)
            {
                case 1:
                    PaintUtilPushTunnelRight(session, height + 8, kTunnelGroup, TunnelSubType::Flat);
                    break;
                case 2:
                    PaintUtilPushTunnelLeft(session, height + 8, kTunnelGroup, TunnelSubType::Flat);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 40);
            break;
    }
}

static void LimLaunchedRCTrackLeftLargeZeroGRollDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    LimLaunchedRCTrackLeftLargeZeroGRollUp(
        session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

static void LimLaunchedRCTrackRightLargeZeroGRollDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    LimLaunchedRCTrackRightLargeZeroGRollUp(
        session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

static void LimLaunchedRCTrackLeftBankToLeftQuarterTurn325DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_TURN_BANK_TRANSITION + 0),
                        { 0, 6, height }, { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_TURN_BANK_TRANSITION + 2),
                        { 0, 6, height }, { 32, 20, 3 });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_TURN_BANK_TRANSITION + 3),
                        { 0, 6, height }, { { 0, 27, height }, { 32, 1, 26 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_TURN_BANK_TRANSITION + 5),
                        { 0, 6, height }, { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_TURN_BANK_TRANSITION + 7),
                        { 0, 6, height }, { 32, 20, 3 });
                    break;
            }
            MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::centre, 3, height, session.SupportColours);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
        case 1:
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 2:
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_TURN_BANK_TRANSITION + 1),
                        { 6, 0, height }, { { 0, 6, height - 6 }, { 20, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_TURN_BANK_TRANSITION + 4),
                        { 6, 0, height }, { { 0, 6, height - 6 }, { 20, 32, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_TURN_BANK_TRANSITION + 6),
                        { 6, 0, height }, { { 0, 6, height - 6 }, { 20, 32, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_TURN_BANK_TRANSITION + 8),
                        { 6, 0, height }, { { 0, 6, height - 6 }, { 20, 32, 3 } });
                    break;
            }
            MetalASupportsPaintSetup(
                session, supportType.metal, MetalSupportPlace::centre, 8, height - 6, session.SupportColours);
            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::SlopeEnd);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::SlopeEnd);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
    }
}

static void LimLaunchedRCTrackRightBankToRightQuarterTurn325DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_TURN_BANK_TRANSITION + 9),
                        { 0, 6, height }, { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_TURN_BANK_TRANSITION + 11),
                        { 0, 6, height }, { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_TURN_BANK_TRANSITION + 13),
                        { 0, 6, height }, { 32, 20, 3 });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_TURN_BANK_TRANSITION + 14),
                        { 0, 6, height }, { { 0, 27, height }, { 32, 1, 26 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_TURN_BANK_TRANSITION + 16),
                        { 0, 6, height }, { 32, 20, 3 });
                    break;
            }
            MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::centre, 3, height, session.SupportColours);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
        case 1:
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 2:
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_TURN_BANK_TRANSITION + 10),
                        { 6, 0, height }, { { 0, 6, height - 6 }, { 20, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_TURN_BANK_TRANSITION + 12),
                        { 6, 0, height }, { { 0, 6, height - 6 }, { 20, 32, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_TURN_BANK_TRANSITION + 15),
                        { 6, 0, height }, { { 0, 6, height - 6 }, { 20, 32, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_TURN_BANK_TRANSITION + 17),
                        { 6, 0, height }, { { 0, 6, height - 6 }, { 20, 32, 3 } });
                    break;
            }
            MetalASupportsPaintSetup(
                session, supportType.metal, MetalSupportPlace::centre, 8, height - 6, session.SupportColours);
            switch (direction)
            {
                case 0:
                    PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::SlopeEnd);
                    break;
                case 1:
                    PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::SlopeEnd);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
    }
}

static void LimLaunchedRCTrackLeftQuarterTurn325DegDownToLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_TURN_BANK_TRANSITION + 12),
                        { 0, 6, height }, { { 0, 6, height - 6 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_TURN_BANK_TRANSITION + 15),
                        { 0, 6, height }, { { 0, 6, height - 6 }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_TURN_BANK_TRANSITION + 17),
                        { 0, 6, height }, { { 0, 6, height - 6 }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_TURN_BANK_TRANSITION + 10),
                        { 0, 6, height }, { { 0, 6, height - 6 }, { 32, 20, 3 } });
                    break;
            }
            MetalASupportsPaintSetup(
                session, supportType.metal, MetalSupportPlace::centre, 8, height - 6, session.SupportColours);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::SlopeEnd);
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
        case 1:
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 2:
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_TURN_BANK_TRANSITION + 11),
                        { 6, 0, height }, { 20, 32, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_TURN_BANK_TRANSITION + 13),
                        { 6, 0, height }, { 20, 32, 3 });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_TURN_BANK_TRANSITION + 14),
                        { 6, 0, height }, { { 27, 0, height }, { 1, 32, 26 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_TURN_BANK_TRANSITION + 16),
                        { 6, 0, height }, { 20, 32, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_TURN_BANK_TRANSITION + 9),
                        { 6, 0, height }, { 20, 32, 3 });
                    break;
            }
            MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::centre, 3, height, session.SupportColours);
            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
    }
}

static void LimLaunchedRCTrackRightQuarterTurn325DegDownToRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_TURN_BANK_TRANSITION + 8),
                        { 0, 6, height }, { { 0, 6, height - 6 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_TURN_BANK_TRANSITION + 1),
                        { 0, 6, height }, { { 0, 6, height - 6 }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_TURN_BANK_TRANSITION + 4),
                        { 0, 6, height }, { { 0, 6, height - 6 }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_TURN_BANK_TRANSITION + 6),
                        { 0, 6, height }, { { 0, 6, height - 6 }, { 32, 20, 3 } });
                    break;
            }
            MetalASupportsPaintSetup(
                session, supportType.metal, MetalSupportPlace::centre, 8, height - 6, session.SupportColours);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::SlopeEnd);
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
        case 1:
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 2:
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_TURN_BANK_TRANSITION + 7),
                        { 6, 0, height }, { 20, 32, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_TURN_BANK_TRANSITION + 0),
                        { 6, 0, height }, { 20, 32, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_TURN_BANK_TRANSITION + 2),
                        { 6, 0, height }, { 20, 32, 3 });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_TURN_BANK_TRANSITION + 3),
                        { 6, 0, height }, { { 27, 0, height }, { 1, 32, 26 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_TURN_BANK_TRANSITION + 5),
                        { 6, 0, height }, { 20, 32, 3 });
                    break;
            }
            MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::centre, 3, height, session.SupportColours);
            switch (direction)
            {
                case 0:
                    PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
                    break;
                case 1:
                    PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
    }
}

static void LimLaunchedRCTrackLeftLargeHalfLoopUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 0),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 7),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 14),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 21),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    break;
            }
            MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::centre, 8, height, session.SupportColours);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 1),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::centre, 15, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 8),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 9 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::centre, 9, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 15),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::centre, 9, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 22),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::centre, 3, height, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 2),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 9),
                        { 0, 0, height }, { { 0, 0, height + 70 }, { 32, 16, 0 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 16),
                        { 0, 0, height }, { { 0, 16, height + 70 }, { 32, 16, 0 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 23),
                        { 0, 0, height }, { { 0, 16, height }, { 32, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft,
                        PaintSegment::topRight, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 88);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 3),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 16, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topLeftSide, 28, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 10),
                        { 0, 0, height }, { { 0, 0, height + 200 }, { 32, 16, 0 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topRightSide, 28, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 17),
                        { 0, 0, height }, { { 0, 16, height + 200 }, { 32, 16, 0 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomRightSide, 0, height + 28, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 24),
                        { 0, 0, height }, { { 0, 16, height }, { 32, 16, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomLeftSide, 28, height, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft,
                        PaintSegment::topRight, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 224);
            break;
        case 4:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 4),
                        { 0, 0, height }, { { 16, 16, height }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 11),
                        { 0, 0, height }, { { 0, 16, height + 110 }, { 16, 16, 0 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 18),
                        { 0, 0, height }, { { 0, 0, height + 100 }, { 16, 16, 0 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 25),
                        { 0, 0, height }, { { 16, 0, height }, { 16, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 128);
            break;
        case 5:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 5),
                        { 0, 0, height }, { { 0, 16, height }, { 32, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 12),
                        { 0, 0, height }, { { 0, 16, height + 200 }, { 32, 16, 0 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 19),
                        { 0, 0, height }, { { 0, 0, height + 200 }, { 32, 16, 0 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 26),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight,
                        PaintSegment::bottomLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 224);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 6),
                        { 0, 0, height }, { { 0, 16, height + 32 }, { 32, 16, 0 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 13),
                        { 0, 0, height }, { { 0, 16, height + 32 }, { 32, 16, 0 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 20),
                        { 0, 0, height }, { { 0, 0, height + 32 }, { 32, 16, 0 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 27),
                        { 0, 0, height }, { { 0, 0, height + 32 }, { 32, 16, 0 } });
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight,
                        PaintSegment::bottomLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 40);
            break;
    }
}

static void LimLaunchedRCTrackRightLargeHalfLoopUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 28),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 35),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 42),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 49),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    break;
            }
            MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::centre, 8, height, session.SupportColours);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 29),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::centre, 3, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 36),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::centre, 9, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 43),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 9 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::centre, 9, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 50),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::centre, 15, height, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 30),
                        { 0, 0, height }, { { 0, 16, height }, { 32, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 37),
                        { 0, 0, height }, { { 0, 16, height + 70 }, { 32, 16, 0 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 44),
                        { 0, 0, height }, { { 0, 0, height + 70 }, { 32, 16, 0 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 51),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight,
                        PaintSegment::bottomLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 88);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 31),
                        { 0, 0, height }, { { 0, 16, height }, { 32, 16, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomRightSide, 28, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 38),
                        { 0, 0, height }, { { 0, 16, height + 200 }, { 32, 16, 0 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomLeftSide, 0, height + 28, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 45),
                        { 0, 0, height }, { { 0, 0, height + 200 }, { 32, 16, 0 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topLeftSide, 28, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 52),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 16, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topRightSide, 28, height, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight,
                        PaintSegment::bottomLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 224);
            break;
        case 4:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 32),
                        { 0, 0, height }, { { 16, 0, height }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 39),
                        { 0, 0, height }, { { 0, 0, height + 100 }, { 16, 16, 0 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 46),
                        { 0, 0, height }, { { 0, 16, height + 110 }, { 16, 16, 0 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 53),
                        { 0, 0, height }, { { 16, 16, height }, { 16, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 128);
            break;
        case 5:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 33),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 40),
                        { 0, 0, height }, { { 0, 0, height + 200 }, { 32, 16, 0 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 47),
                        { 0, 0, height }, { { 0, 16, height + 200 }, { 32, 16, 0 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 54),
                        { 0, 0, height }, { { 0, 16, height }, { 32, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft,
                        PaintSegment::topRight, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 224);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 34),
                        { 0, 0, height }, { { 0, 0, height + 32 }, { 32, 16, 0 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 41),
                        { 0, 0, height }, { { 0, 0, height + 32 }, { 32, 16, 0 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 48),
                        { 0, 0, height }, { { 0, 16, height + 32 }, { 32, 16, 0 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 55),
                        { 0, 0, height }, { { 0, 16, height + 32 }, { 32, 16, 0 } });
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft,
                        PaintSegment::topRight, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 40);
            break;
    }
}

static void LimLaunchedRCTrackLeftLargeHalfLoopDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    LimLaunchedRCTrackRightLargeHalfLoopUp(session, ride, 6 - trackSequence, direction, height, trackElement, supportType);
}

static void LimLaunchedRCTrackRightLargeHalfLoopDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    LimLaunchedRCTrackLeftLargeHalfLoopUp(session, ride, 6 - trackSequence, direction, height, trackElement, supportType);
}

static void LimLaunchedRCTrackFlatTo60DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_SMALL_FLAT_TO_STEEP + 0),
                { 0, 0, height }, { { 0, 2, height }, { 32, 27, 4 } });
            MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::centre, 3, height, session.SupportColours);
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_SMALL_FLAT_TO_STEEP + 2),
                { 0, 0, height }, { { 29, 4, height + 2 }, { 1, 24, 43 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_SMALL_FLAT_TO_STEEP + 1),
                { 0, 0, height }, { { 0, 4, height }, { 32, 2, 43 } });
            MetalASupportsPaintSetup(
                session, supportType.metal, MetalSupportPlace::centre, 0, height + 4, session.SupportColours);
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_SMALL_FLAT_TO_STEEP + 4),
                { 0, 0, height }, { { 29, 4, height + 2 }, { 1, 24, 43 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_SMALL_FLAT_TO_STEEP + 3),
                { 0, 0, height }, { { 0, 4, height }, { 32, 2, 43 } });
            MetalASupportsPaintSetup(
                session, supportType.metal, MetalSupportPlace::centre, 0, height + 4, session.SupportColours);
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_SMALL_FLAT_TO_STEEP + 5),
                { 0, 0, height }, { { 0, 2, height }, { 32, 27, 4 } });
            MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::centre, 3, height, session.SupportColours);
            break;
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 24, kTunnelGroup, TunnelSubType::SlopeEnd);
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 64);
}

static void LimLaunchedRCTrack60DegUpToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_SMALL_FLAT_TO_STEEP + 6),
                { 0, 0, height }, { { 0, 2, height }, { 32, 27, 4 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_SMALL_FLAT_TO_STEEP + 7),
                { 0, 0, height }, { { 0, 4, height }, { 32, 2, 43 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_SMALL_FLAT_TO_STEEP + 8),
                { 0, 0, height }, { { 29, 4, height + 2 }, { 1, 24, 43 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_SMALL_FLAT_TO_STEEP + 10),
                { 0, 0, height }, { { 29, 4, height + 2 }, { 1, 24, 43 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_SMALL_FLAT_TO_STEEP + 9),
                { 0, 0, height }, { { 0, 4, height }, { 32, 2, 43 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_SMALL_FLAT_TO_STEEP + 11),
                { 0, 0, height }, { { 0, 2, height }, { 32, 27, 4 } });
            break;
    }
    MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::centre, 16, height, session.SupportColours);
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 24, kTunnelGroup, TunnelSubType::Flat);
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 72);
}

static void LimLaunchedRCTrackFlatTo60DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    LimLaunchedRCTrack60DegUpToFlat(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

static void LimLaunchedRCTrack60DegDownToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    LimLaunchedRCTrackFlatTo60DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

static void LimLaunchedRCTrackDiagFlatTo60DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_SMALL_FLAT_TO_STEEP + 15),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 4 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_SMALL_FLAT_TO_STEEP + 12),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 4 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_SMALL_FLAT_TO_STEEP + 14),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 4 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::leftCorner, 7, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_SMALL_FLAT_TO_STEEP + 13),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 4 } });
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topCorner, 7, height, session.SupportColours);
                    break;
                case 2:
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::rightCorner, 7, height, session.SupportColours);
                    break;
                case 3:
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomCorner, 7, height, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
    }
}

static void LimLaunchedRCTrackDiag60DegUpToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_SMALL_FLAT_TO_STEEP + 19),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 4 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_SMALL_FLAT_TO_STEEP + 16),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 4 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_SMALL_FLAT_TO_STEEP + 18),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 4 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::leftCorner, 20, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_SMALL_FLAT_TO_STEEP + 17),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 4 } });
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topCorner, 20, height, session.SupportColours);
                    break;
                case 2:
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::rightCorner, 20, height, session.SupportColours);
                    break;
                case 3:
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomCorner, 20, height, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
    }
}

static void LimLaunchedRCTrackDiagFlatTo60DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_SMALL_FLAT_TO_STEEP + 17),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 4 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_SMALL_FLAT_TO_STEEP + 18),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 4 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_SMALL_FLAT_TO_STEEP + 16),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 4 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::leftCorner, 16, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_SMALL_FLAT_TO_STEEP + 19),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 4 } });
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topCorner, 16, height, session.SupportColours);
                    break;
                case 2:
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::rightCorner, 16, height, session.SupportColours);
                    break;
                case 3:
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomCorner, 16, height, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
    }
}

static void LimLaunchedRCTrackDiag60DegDownToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_SMALL_FLAT_TO_STEEP + 13),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 4 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_SMALL_FLAT_TO_STEEP + 14),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 4 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_SMALL_FLAT_TO_STEEP + 12),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 4 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::leftCorner, 5, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex(SPR_TRACKS_LIM_LAUNCHED_TRACK_SMALL_FLAT_TO_STEEP + 15),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 4 } });
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topCorner, 5, height, session.SupportColours);
                    break;
                case 2:
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::rightCorner, 5, height, session.SupportColours);
                    break;
                case 3:
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomCorner, 5, height, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
    }
}

static void LimLaunchedRCTrackBooster(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(15012), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(15014), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 26 } });
            break;
        case 1:
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(15013), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(15015), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 26 } });
            break;
    }
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::centre, 0, height, session.SupportColours);
    }
    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

// wide sloped turns
static void LimLaunchedRCTrackLeftEighthToDiagUp25(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE + 0)),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE + 4)),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE + 8)),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE + 12)),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    break;
            }
            MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::centre, 6, height, session.SupportColours);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE + 1)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE + 5)),
                        { 0, 0, height }, { { 0, 0, height }, { 34, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE + 9)),
                        { 0, 0, height }, { { 0, 16, height }, { 32, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE + 13)),
                        { 0, 0, height }, { { 0, 16, height }, { 32, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft,
                        PaintSegment::topRight, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE + 2)),
                        { 0, 0, height }, { { 0, 16, height }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE + 6)),
                        { 0, 0, height }, { { 16, 16, height }, { 16, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE + 10)),
                        { 0, 0, height }, { { 16, 0, height }, { 16, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE + 14)),
                        { 0, 0, height }, { { 0, 0, height }, { 16, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight,
                        PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 4:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE + 3)),
                        { 0, 0, height }, { { 16, 16, height }, { 16, 16, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomCorner, 6, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE + 7)),
                        { 0, 0, height }, { { 0, 16, height }, { 16, 18, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::leftCorner, 8, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE + 11)),
                        { 0, 0, height }, { { 0, 0, height }, { 16, 16, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topCorner, 7, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE + 15)),
                        { 0, 0, height }, { { 16, 0, height }, { 16, 16, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::rightCorner, 8, height, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight,
                        PaintSegment::bottomLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
    }
}

static void LimLaunchedRCTrackRightEighthToDiagUp25(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE + 16)),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE + 20)),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE + 24)),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE + 28)),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    break;
            }
            MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::centre, 6, height, session.SupportColours);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE + 17)),
                        { 0, 0, height }, { { 0, 16, height }, { 32, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE + 21)),
                        { 0, 0, height }, { { 0, 16, height }, { 32, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE + 25)),
                        { 0, 0, height }, { { 0, 0, height }, { 34, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE + 29)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight,
                        PaintSegment::bottomLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE + 18)),
                        { 0, 0, height }, { { 0, 0, height }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE + 22)),
                        { 0, 0, height }, { { 16, 0, height }, { 16, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE + 26)),
                        { 0, 0, height }, { { 4, 4, height }, { 28, 28, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE + 30)),
                        { 0, 0, height }, { { 0, 16, height }, { 16, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft,
                        PaintSegment::topRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 4:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE + 19)),
                        { 0, 0, height }, { { 16, 0, height }, { 16, 16, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::leftCorner, 6, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE + 23)),
                        { 0, 0, height }, { { 0, 0, height }, { 16, 16, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topCorner, 6, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE + 27)),
                        { 0, 0, height }, { { 0, 16, height }, { 16, 18, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::rightCorner, 8, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE + 31)),
                        { 0, 0, height }, { { 16, 16, height }, { 16, 16, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomCorner, 10, height, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight,
                        PaintSegment::bottomLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
    }
}

static void LimLaunchedRCTrackLeftEighthToOrthogonalUp25(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE + 32)),
                        { 0, 0, height }, { { 0, 16, height }, { 16, 16, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::rightCorner, 11, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE + 36)),
                        { 0, 0, height }, { { 16, 16, height }, { 16, 18, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomCorner, 10, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE + 40)),
                        { 0, 0, height }, { { 16, 0, height }, { 16, 16, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::leftCorner, 8, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE + 44)),
                        { 0, 0, height }, { { 0, 0, height }, { 16, 16, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topCorner, 5, height, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::right, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight,
                        PaintSegment::bottomLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE + 33)),
                        { 0, 0, height }, { { 16, 16, height }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE + 37)),
                        { 0, 0, height }, { { 0, 16, height }, { 16, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE + 41)),
                        { 0, 0, height }, { { 0, 0, height }, { 16, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE + 45)),
                        { 0, 0, height }, { { 16, 0, height }, { 16, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft,
                        PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 2:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE + 34)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE + 38)),
                        { 0, 0, height }, { { 0, 0, height }, { 34, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE + 42)),
                        { 0, 0, height }, { { 0, 16, height }, { 32, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE + 46)),
                        { 0, 0, height }, { { 0, 16, height }, { 32, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft,
                        PaintSegment::topRight, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 4:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE + 35)),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE + 39)),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE + 43)),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 10 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE + 47)),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    break;
            }
            MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::centre, 4, height, session.SupportColours);

            if (direction == 1 || direction == 2)
            {
                PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
    }
}

static void LimLaunchedRCTrackRightEighthToOrthogonalUp25(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE + 48)),
                        { 0, 0, height }, { { 0, 16, height }, { 16, 16, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::rightCorner, 10, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE + 52)),
                        { 0, 0, height }, { { 16, 16, height }, { 16, 16, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomCorner, 11, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE + 56)),
                        { 0, 0, height }, { { 16, 0, height }, { 16, 18, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::leftCorner, 12, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE + 60)),
                        { 0, 0, height }, { { 0, 0, height }, { 16, 16, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topCorner, 6, height, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::right, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight,
                        PaintSegment::bottomLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE + 49)),
                        { 0, 0, height }, { { 0, 0, height }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE + 53)),
                        { 0, 0, height }, { { 16, 0, height }, { 16, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE + 57)),
                        { 0, 0, height }, { { 16, 16, height }, { 16, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE + 61)),
                        { 0, 0, height }, { { 0, 16, height }, { 16, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::top, PaintSegment::right, PaintSegment::centre, PaintSegment::topLeft,
                        PaintSegment::topRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 2:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE + 50)),
                        { 0, 0, height }, { { 16, 0, height }, { 16, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE + 54)),
                        { 0, 0, height }, { { 0, 0, height }, { 16, 32, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE + 58)),
                        { 0, 0, height }, { { 0, 0, height }, { 16, 32, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE + 62)),
                        { 0, 0, height }, { { 16, 0, height }, { 16, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::left, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft,
                        PaintSegment::bottomLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 4:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE + 51)),
                        { 0, 0, height }, { { 6, 0, height }, { 20, 32, 10 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE + 55)),
                        { 0, 0, height }, { { 6, 0, height }, { 20, 32, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE + 59)),
                        { 0, 0, height }, { { 6, 0, height }, { 20, 32, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE + 63)),
                        { 0, 0, height }, { { 6, 0, height }, { 20, 32, 3 } });
                    break;
            }
            MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::centre, 4, height, session.SupportColours);
            if (direction == 0 || direction == 1)
            {
                PaintUtilPushTunnelRotated(session, direction + 1, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomRight), direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
    }
}

static void LimLaunchedRCTrackLeftEighthToDiagDown25(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    uint8_t map[5] = { 4, 3, 1, 2, 0 };
    trackSequence = map[trackSequence];
    LimLaunchedRCTrackRightEighthToOrthogonalUp25(
        session, ride, trackSequence, (direction + 1) & 3, height, trackElement, supportType);
}

static void LimLaunchedRCTrackRightEighthToDiagDown25(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    uint8_t map[5] = { 4, 3, 1, 2, 0 };
    trackSequence = map[trackSequence];
    LimLaunchedRCTrackLeftEighthToOrthogonalUp25(
        session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

static void LimLaunchedRCTrackLeftEighthToOrthogonalDown25(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    LimLaunchedRCTrackRightEighthToDiagUp25(
        session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

static void LimLaunchedRCTrackRightEighthToOrthogonalDown25(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    LimLaunchedRCTrackLeftEighthToDiagUp25(
        session, ride, trackSequence, (direction + 3) & 3, height, trackElement, supportType);
}

// banked
static void LimLaunchedRCTrackLeftEighthBankToDiagUp25(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 64)),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::centre, 10, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 68)),
                        { 0, 0, height }, { { 0, 31, height }, { 32, 1, 32 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::centre, 8, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 72)),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::centre, 10, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 76)),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::centre, 12, height, session.SupportColours);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 65)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 69)),
                        { 0, 0, height }, { { 0, 31, height }, { 32, 1, 32 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 73)),
                        { 0, 0, height }, { { 0, 16, height }, { 32, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 77)),
                        { 0, 0, height }, { { 0, 16, height }, { 32, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft,
                        PaintSegment::topRight, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 66)),
                        { 0, 0, height }, { { 0, 16, height }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 70)),
                        { 0, 0, height }, { { 0, 0, height + 32 }, { 32, 40, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 74)),
                        { 0, 0, height }, { { 16, 0, height }, { 16, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 78)),
                        { 0, 0, height }, { { 0, 0, height }, { 16, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight,
                        PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 4:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 67)),
                        { 0, 0, height }, { { 16, 16, height }, { 16, 16, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomCorner, 11, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 71)),
                        { 0, 0, height }, { { 0, 0, height + 48 }, { 32, 32, 1 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::leftCorner, 7, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 75)),
                        { 0, 0, height }, { { 0, 0, height }, { 16, 16, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topCorner, 4, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 79)),
                        { 0, 0, height }, { { 16, 0, height }, { 16, 16, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::rightCorner, 9, height, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight,
                        PaintSegment::bottomLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
    }
}

static void LimLaunchedRCTrackRightEighthBankToDiagUp25(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 80)),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::centre, 9, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 84)),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::centre, 9, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 88)),
                        { 0, 0, height }, { { 0, 31, height }, { 32, 1, 32 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::centre, 9, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 92)),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::centre, 11, height, session.SupportColours);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 81)),
                        { 0, 0, height }, { { 0, 16, height }, { 32, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 85)),
                        { 0, 0, height }, { { 0, 16, height }, { 32, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 89)),
                        { 0, 0, height }, { { 0, 31, height }, { 32, 1, 32 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 93)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight,
                        PaintSegment::bottomLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 82)),
                        { 0, 0, height }, { { 0, 0, height }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 86)),
                        { 0, 0, height }, { { 16, 0, height }, { 16, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 90)),
                        { 0, 0, height }, { { 0, 0, height + 32 }, { 32, 40, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 94)),
                        { 0, 0, height }, { { 0, 16, height }, { 16, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft,
                        PaintSegment::topRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 4:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 83)),
                        { 0, 0, height }, { { 16, 0, height }, { 16, 16, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::leftCorner, 8, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 87)),
                        { 0, 0, height }, { { 0, 0, height }, { 16, 16, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topCorner, 7, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 91)),
                        { 0, 0, height }, { { 0, 0, height + 48 }, { 32, 32, 1 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::rightCorner, 9, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 95)),
                        { 0, 0, height }, { { 16, 16, height }, { 16, 16, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomCorner, 11, height, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight,
                        PaintSegment::bottomLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
    }
}

static void LimLaunchedRCTrackLeftEighthBankToOrthogonalUp25(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 96)),
                        { 0, 0, height }, { { 0, 0, height + 32 }, { 32, 32, 1 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::rightCorner, 11, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 100)),
                        { 0, 0, height }, { { 0, 0, height + 32 }, { 32, 32, 1 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomCorner, 12, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 104)),
                        { 0, 0, height }, { { 16, 0, height }, { 16, 16, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::leftCorner, 10, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 108)),
                        { 0, 0, height }, { { 0, 0, height }, { 16, 16, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topCorner, 0, height, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::right, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight,
                        PaintSegment::bottomLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 97)),
                        { 0, 0, height }, { { 0, 0, height + 32 }, { 32, 32, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 101)),
                        { 0, 0, height }, { { 0, 0, height + 32 }, { 32, 32, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 105)),
                        { 0, 0, height }, { { 0, 0, height }, { 16, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 109)),
                        { 0, 0, height }, { { 16, 0, height }, { 16, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft,
                        PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 2:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 98)),
                        { 0, 0, height }, { { 0, 31, height }, { 32, 1, 32 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 102)),
                        { 0, 0, height }, { { 0, 31, height }, { 32, 1, 32 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 106)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 16, 10 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 110)),
                        { 0, 0, height }, { { 0, 16, height }, { 32, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft,
                        PaintSegment::topRight, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 4:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 99)),
                        { 0, 0, height }, { { 0, 31, height }, { 32, 1, 32 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::centre, 10, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 103)),
                        { 0, 0, height }, { { 0, 31, height }, { 32, 1, 32 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::centre, 10, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 107)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 20, 10 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::centre, 13, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 111)),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::centre, 11, height, session.SupportColours);
                    break;
            }
            if (direction == 1 || direction == 2)
            {
                PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
    }
}

static void LimLaunchedRCTrackRightEighthBankToOrthogonalUp25(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 112)),
                        { 0, 0, height }, { { 0, 16, height }, { 16, 16, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::rightCorner, 9, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 116)),
                        { 0, 0, height }, { { 0, 0, height + 32 }, { 32, 32, 1 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomCorner, 11, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 120)),
                        { 0, 0, height }, { { 0, 0, height + 32 }, { 32, 32, 1 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::leftCorner, 9, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 124)),
                        { 0, 0, height }, { { 0, 0, height }, { 16, 16, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topCorner, 2, height, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::right, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight,
                        PaintSegment::bottomLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 113)),
                        { 0, 0, height }, { { 0, 0, height }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 117)),
                        { 0, 0, height }, { { 0, 0, height + 32 }, { 32, 32, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 121)),
                        { 0, 0, height }, { { 0, 0, height + 32 }, { 32, 32, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 125)),
                        { 0, 0, height }, { { 0, 16, height }, { 16, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::top, PaintSegment::right, PaintSegment::centre, PaintSegment::topLeft,
                        PaintSegment::topRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 2:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 114)),
                        { 0, 0, height }, { { 0, 0, height }, { 16, 32, 10 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 118)),
                        { 0, 0, height }, { { 31, 0, height }, { 1, 32, 32 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 122)),
                        { 0, 0, height }, { { 31, 0, height }, { 1, 32, 32 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 126)),
                        { 0, 0, height }, { { 16, 0, height }, { 16, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::left, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft,
                        PaintSegment::bottomLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 4:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 115)),
                        { 0, 0, height }, { { 6, 0, height }, { 20, 32, 10 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::centre, 8, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 119)),
                        { 0, 0, height }, { { 31, 0, height }, { 1, 32, 32 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::centre, 8, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 123)),
                        { 0, 0, height }, { { 31, 0, height }, { 1, 32, 32 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::centre, 9, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 127)),
                        { 0, 0, height }, { { 6, 0, height }, { 20, 32, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::centre, 11, height, session.SupportColours);
                    break;
            }
            if (direction == 0 || direction == 1)
            {
                PaintUtilPushTunnelRotated(session, direction + 1, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomRight), direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
    }
}

static void LimLaunchedRCTrackLeftEighthBankToDiagDown25(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    uint8_t map[5] = { 4, 3, 1, 2, 0 };
    trackSequence = map[trackSequence];
    LimLaunchedRCTrackRightEighthBankToOrthogonalUp25(
        session, ride, trackSequence, (direction + 1) & 3, height, trackElement, supportType);
}

static void LimLaunchedRCTrackRightEighthBankToDiagDown25(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    uint8_t map[5] = { 4, 3, 1, 2, 0 };
    trackSequence = map[trackSequence];
    LimLaunchedRCTrackLeftEighthBankToOrthogonalUp25(
        session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

static void LimLaunchedRCTrackLeftEighthBankToOrthogonalDown25(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    LimLaunchedRCTrackRightEighthBankToDiagUp25(
        session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

static void LimLaunchedRCTrackRightEighthBankToOrthogonalDown25(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    LimLaunchedRCTrackLeftEighthBankToDiagUp25(
        session, ride, trackSequence, (direction + 3) & 3, height, trackElement, supportType);
}

// banked tranisitons
static void LimLaunchedRCTrackDiagUp25ToLeftBankedUp25(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 4)),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 0)),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 1)),
                        { -16, -16, height }, { { -16, -16, height + 48 }, { 32, 32, 0 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 3)),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::leftCorner, 8, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 2)),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 3 } });
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topCorner, 10, height, session.SupportColours);
                    break;
                case 2:
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::rightCorner, 10, height, session.SupportColours);
                    break;
                case 3:
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomCorner, 10, height, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
    }
}

static void LimLaunchedRCTrackDiagUp25ToRightBankedUp25(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 9)),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 5)),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 7)),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 8)),
                        { -16, -16, height }, { { -16, -16, height + 48 }, { 32, 32, 0 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::leftCorner, 10, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 6)),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 3 } });
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topCorner, 10, height, session.SupportColours);
                    break;
                case 2:
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::rightCorner, 5, height, session.SupportColours);
                    break;
                case 3:
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomCorner, 10, height, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
    }
}

static void LimLaunchedRCTrackDiagLeftBankedUp25ToUp25(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 14)),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 10)),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 11)),
                        { -16, -16, height }, { { -16, -16, height + 48 }, { 32, 32, 0 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 13)),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::leftCorner, 5, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 12)),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 3 } });
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topCorner, 9, height, session.SupportColours);
                    break;
                case 2:
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::rightCorner, 9, height, session.SupportColours);
                    break;
                case 3:
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomCorner, 9, height, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
    }
}

static void LimLaunchedRCTrackDiagRightBankedUp25ToUp25(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 19)),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 15)),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 17)),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 18)),
                        { -16, -16, height }, { { -16, -16, height + 48 }, { 32, 32, 0 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::leftCorner, 9, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 16)),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 3 } });
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topCorner, 9, height, session.SupportColours);
                    break;
                case 2:
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::rightCorner, 5, height, session.SupportColours);
                    break;
                case 3:
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomCorner, 9, height, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
    }
}

static void LimLaunchedRCTrackDiagDown25ToLeftBankedDown25(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    LimLaunchedRCTrackDiagRightBankedUp25ToUp25(
        session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

static void LimLaunchedRCTrackDiagDown25ToRightBankedDown25(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    LimLaunchedRCTrackDiagLeftBankedUp25ToUp25(
        session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

static void LimLaunchedRCTrackDiagLeftBankedDown25ToDown25(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    LimLaunchedRCTrackDiagUp25ToRightBankedUp25(
        session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

static void LimLaunchedRCTrackDiagRightBankedDown25ToDown25(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    LimLaunchedRCTrackDiagUp25ToLeftBankedUp25(
        session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

static void LimLaunchedRCTrackDiagLeftBankedFlatToLeftBankedUp25(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 23)),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 20)),
                        { -16, -16, height }, { { -16, -16, height + 34 }, { 32, 32, 0 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 22)),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::leftCorner, 0, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 21)),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 3 } });
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topCorner, 4, height, session.SupportColours);
                    break;
                case 2:
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::rightCorner, 4, height, session.SupportColours);
                    break;
                case 3:
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomCorner, 4, height, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
    }
}

static void LimLaunchedRCTrackDiagRightBankedFlatToRightBankedUp25(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 27)),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 24)),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 26)),
                        { -16, -16, height }, { { -16, -16, height + 34 }, { 32, 32, 0 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::leftCorner, 4, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 25)),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 3 } });
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topCorner, 4, height, session.SupportColours);
                    break;
                case 2:
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::rightCorner, 0, height, session.SupportColours);
                    break;
                case 3:
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomCorner, 4, height, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
    }
}

static void LimLaunchedRCTrackDiagLeftBankedUp25ToLeftBankedFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 31)),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 28)),
                        { -16, -16, height }, { { -16, -16, height + 32 }, { 32, 32, 0 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 30)),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::leftCorner, 4, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 29)),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 3 } });
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topCorner, 8, height, session.SupportColours);
                    break;
                case 2:
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::rightCorner, 8, height, session.SupportColours);
                    break;
                case 3:
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomCorner, 8, height, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
    }
}

static void LimLaunchedRCTrackDiagRightBankedUp25ToRightBankedFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 35)),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 32)),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 34)),
                        { -16, -16, height }, { { -16, -16, height + 32 }, { 32, 32, 0 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::leftCorner, 8, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 33)),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 3 } });
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topCorner, 8, height, session.SupportColours);
                    break;
                case 2:
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::rightCorner, 4, height, session.SupportColours);
                    break;
                case 3:
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomCorner, 8, height, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
    }
}

static void LimLaunchedRCTrackDiagLeftBankedFlatToLeftBankedDown25(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    LimLaunchedRCTrackDiagRightBankedUp25ToRightBankedFlat(
        session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

static void LimLaunchedRCTrackDiagRightBankedFlatToRightBankedDown25(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    LimLaunchedRCTrackDiagLeftBankedUp25ToLeftBankedFlat(
        session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

static void LimLaunchedRCTrackDiagLeftBankedDown25ToLeftBankedFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    LimLaunchedRCTrackDiagRightBankedFlatToRightBankedUp25(
        session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

static void LimLaunchedRCTrackDiagRightBankedDown25ToRightBankedFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    LimLaunchedRCTrackDiagLeftBankedFlatToLeftBankedUp25(
        session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

static void LimLaunchedRCTrackDiagUp25LeftBanked(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 39)),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 36)),
                        { -16, -16, height }, { { -16, -16, height + 42 }, { 32, 32, 0 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 38)),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::leftCorner, 6, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 37)),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 3 } });
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topCorner, 11, height, session.SupportColours);
                    break;
                case 2:
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::rightCorner, 11, height, session.SupportColours);
                    break;
                case 3:
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomCorner, 11, height, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
    }
}

static void LimLaunchedRCTrackDiagUp25RightBanked(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 43)),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 40)),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 42)),
                        { -16, -16, height }, { { -16, -16, height + 42 }, { 32, 32, 0 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::leftCorner, 11, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 41)),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 3 } });
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topCorner, 11, height, session.SupportColours);
                    break;
                case 2:
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::rightCorner, 6, height, session.SupportColours);
                    break;
                case 3:
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomCorner, 11, height, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
    }
}

static void LimLaunchedRCTrackDiagDown25LeftBanked(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    LimLaunchedRCTrackDiagUp25RightBanked(
        session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

static void LimLaunchedRCTrackDiagDown25RightBanked(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    LimLaunchedRCTrackDiagUp25LeftBanked(
        session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

static void LimLaunchedRCTrackDiagFlatToLeftBankedUp25(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 48)),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 44)),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 45)),
                        { -16, -16, height }, { { -16, -16, height + 34 }, { 32, 32, 0 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 47)),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::leftCorner, 0, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 46)),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 3 } });
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topCorner, 4, height, session.SupportColours);
                    break;
                case 2:
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::rightCorner, 4, height, session.SupportColours);
                    break;
                case 3:
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomCorner, 4, height, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
    }
}

static void LimLaunchedRCTrackDiagFlatToRightBankedUp25(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 53)),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 49)),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 51)),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 52)),
                        { -16, -16, height }, { { -16, -16, height + 34 }, { 32, 32, 0 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::leftCorner, 4, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 50)),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 3 } });
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topCorner, 4, height, session.SupportColours);
                    break;
                case 2:
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::rightCorner, 0, height, session.SupportColours);
                    break;
                case 3:
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomCorner, 4, height, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
    }
}

static void LimLaunchedRCTrackDiagLeftBankedUp25ToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 58)),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 54)),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 55)),
                        { -16, -16, height }, { { -16, -16, height + 27 }, { 32, 32, 0 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 57)),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::leftCorner, 4, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 56)),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 3 } });
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topCorner, 6, height, session.SupportColours);
                    break;
                case 2:
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::rightCorner, 6, height, session.SupportColours);
                    break;
                case 3:
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomCorner, 6, height, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
    }
}

static void LimLaunchedRCTrackDiagRightBankedUp25ToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 63)),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 59)),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 61)),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 62)),
                        { -16, -16, height }, { { -16, -16, height + 27 }, { 32, 32, 0 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::leftCorner, 6, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_GENTLE_LARGE_CURVE_BANKED + 60)),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 3 } });
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topCorner, 6, height, session.SupportColours);
                    break;
                case 2:
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::rightCorner, 4, height, session.SupportColours);
                    break;
                case 3:
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomCorner, 6, height, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
    }
}

static void LimLaunchedRCTrackDiagFlatToLeftBankedDown25(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    LimLaunchedRCTrackDiagRightBankedUp25ToFlat(
        session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

static void LimLaunchedRCTrackDiagFlatToRightBankedDown25(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    LimLaunchedRCTrackDiagLeftBankedUp25ToFlat(
        session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

static void LimLaunchedRCTrackDiagLeftBankedDown25ToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    LimLaunchedRCTrackDiagFlatToRightBankedUp25(
        session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

static void LimLaunchedRCTrackDiagRightBankedDown25ToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    LimLaunchedRCTrackDiagFlatToLeftBankedUp25(
        session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

// dive loops
static void LimLaunchedRCTrackLeftEighthDiveLoopUpToOrthogonal(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_DIVE_LOOP + 4)),
                        { -16, -16, height }, { { 0, 0, height + 48 }, { 32, 32, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_DIVE_LOOP + 12)),
                        { -16, -16, height }, { { 0, 0, height + -8 }, { 32, 32, 1 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight, PaintSegment::right),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 96);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_DIVE_LOOP + 0)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::topLeft, PaintSegment::top, PaintSegment::centre, PaintSegment::topRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 120);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_DIVE_LOOP + 8)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 120);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_DIVE_LOOP + 1)),
                        { -16, -16, height }, { { 0, 31, height + 0 }, { 32, 1, 64 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::leftCorner, 0, height + 9, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_DIVE_LOOP + 5)),
                        { -16, -16, height }, { { 0, 31, height + 0 }, { 32, 1, 64 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topCorner, 0, height + 4, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_DIVE_LOOP + 9)),
                        { -16, -16, height }, { { 2, 2, height + 4 }, { 28, 28, 1 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::rightCorner, 0, height + 4, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_DIVE_LOOP + 13)),
                        { -16, -16, height }, { { 2, 2, height + 12 }, { 28, 28, 1 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomCorner, 0, height + 4, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::left, PaintSegment::topLeft, PaintSegment::top, PaintSegment::bottomLeft,
                        PaintSegment::centre, PaintSegment::topRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 112);
            break;
        case 4:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_DIVE_LOOP + 2)),
                        { -16, -16, height }, { { 0, 31, height + 0 }, { 32, 1, 64 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_DIVE_LOOP + 6)),
                        { -16, -16, height }, { { 0, 31, height + 0 }, { 32, 1, 64 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_DIVE_LOOP + 10)),
                        { -16, -16, height }, { { 0, 0, height + 0 }, { 32, 1, 64 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_DIVE_LOOP + 14)),
                        { -16, -16, height }, { { 0, 6, height + 58 }, { 32, 20, 1 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::left, PaintSegment::topLeft, PaintSegment::top, PaintSegment::bottomLeft,
                        PaintSegment::centre, PaintSegment::topRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 80);
            break;
        case 5:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::left, PaintSegment::topLeft, PaintSegment::top, PaintSegment::bottomLeft,
                        PaintSegment::centre, PaintSegment::topRight),
                    direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_DIVE_LOOP + 3)),
                        { -16, -16, height }, { { 0, 6, height + 36 }, { 32, 20, 1 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomLeftSide, 0, height + 44, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_DIVE_LOOP + 7)),
                        { -16, -16, height }, { { 0, 6, height + 36 }, { 32, 20, 1 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topLeftSide, 0, height + 36, session.SupportColours);
                    PaintUtilPushTunnelRight(session, height + 8, kTunnelGroup, TunnelSubType::Tall);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_DIVE_LOOP + 11)),
                        { -16, -16, height }, { { 0, 6, height + 36 }, { 32, 20, 1 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topRightSide, 0, height + 34, session.SupportColours);
                    PaintUtilPushTunnelLeft(session, height + 8, kTunnelGroup, TunnelSubType::Tall);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_DIVE_LOOP + 15)),
                        { -16, -16, height }, { { 0, 6, height + 36 }, { 32, 20, 1 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomRightSide, 0, height + 39, session.SupportColours);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
    }
}

static void LimLaunchedRCTrackRightEighthDiveLoopUpToOrthogonal(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_DIVE_LOOP + 20)),
                        { -16, -16, height }, { { 0, 0, height + 48 }, { 32, 32, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_DIVE_LOOP + 28)),
                        { -16, -16, height }, { { 0, 0, height + -8 }, { 32, 32, 1 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight, PaintSegment::right),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 96);
            break;
        case 1:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_DIVE_LOOP + 24)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 120);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_DIVE_LOOP + 16)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::topLeft, PaintSegment::top, PaintSegment::centre, PaintSegment::topRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 120);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_DIVE_LOOP + 17)),
                        { -16, -16, height }, { { 2, 2, height + 4 }, { 28, 28, 1 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::leftCorner, 0, height + 4, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_DIVE_LOOP + 21)),
                        { -16, -16, height }, { { 31, 0, height + 0 }, { 1, 32, 64 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topCorner, 0, height + 4, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_DIVE_LOOP + 25)),
                        { -16, -16, height }, { { 31, 0, height + 0 }, { 1, 32, 64 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::rightCorner, 0, height + 9, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_DIVE_LOOP + 29)),
                        { -16, -16, height }, { { 2, 2, height + 12 }, { 28, 28, 1 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomCorner, 0, height + 4, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre,
                        PaintSegment::bottom, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 112);
            break;
        case 4:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_DIVE_LOOP + 18)),
                        { -16, -16, height }, { { 0, 0, height + 0 }, { 1, 32, 64 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_DIVE_LOOP + 22)),
                        { -16, -16, height }, { { 31, 0, height + 0 }, { 1, 32, 64 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_DIVE_LOOP + 26)),
                        { -16, -16, height }, { { 31, 0, height + 0 }, { 1, 32, 64 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_DIVE_LOOP + 30)),
                        { -16, -16, height }, { { 6, 0, height + 58 }, { 20, 32, 1 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre,
                        PaintSegment::bottom, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 80);
            break;
        case 5:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre,
                        PaintSegment::bottom, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_DIVE_LOOP + 19)),
                        { -16, -16, height }, { { 6, 0, height + 36 }, { 20, 32, 1 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topLeftSide, 0, height + 34, session.SupportColours);
                    PaintUtilPushTunnelRight(session, height + 8, kTunnelGroup, TunnelSubType::Tall);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_DIVE_LOOP + 23)),
                        { -16, -16, height }, { { 6, 0, height + 36 }, { 20, 32, 1 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topRightSide, 0, height + 36, session.SupportColours);
                    PaintUtilPushTunnelLeft(session, height + 8, kTunnelGroup, TunnelSubType::Tall);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_DIVE_LOOP + 27)),
                        { -16, -16, height }, { { 6, 0, height + 36 }, { 20, 32, 1 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomRightSide, 0, height + 44, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_DIVE_LOOP + 31)),
                        { -16, -16, height }, { { 6, 0, height + 36 }, { 20, 32, 1 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomLeftSide, 0, height + 39, session.SupportColours);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
    }
}

static void LimLaunchedRCTrackLeftEighthDiveLoopDownToDiag(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    LimLaunchedRCTrackRightEighthDiveLoopUpToOrthogonal(
        session, ride, 5 - trackSequence, (direction + 1) & 3, height, trackElement, supportType);
}

static void LimLaunchedRCTrackRightEighthDiveLoopToDownOrthogonal(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    LimLaunchedRCTrackLeftEighthDiveLoopUpToOrthogonal(
        session, ride, 5 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

// diag large transition
static void LimLaunchedRCTrackDiagFlatTo60DegUpLongBase(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP_DIAGONAL + 9)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight, PaintSegment::right),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP_DIAGONAL + 0)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::topLeft, PaintSegment::top, PaintSegment::centre, PaintSegment::topRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP_DIAGONAL + 6)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::leftCorner, 0, height + 0, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP_DIAGONAL + 3)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topCorner, 0, height + 0, session.SupportColours);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::rightCorner, 0, height + 0, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP_DIAGONAL + 10)),
                        { -16, -16, height }, { { -20, -20, height + 8 }, { 24, 24, 1 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomCorner, 0, height + 0, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre,
                        PaintSegment::topRight, PaintSegment::bottomRight, PaintSegment::right),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
        case 4:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP_DIAGONAL + 1)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::topLeft, PaintSegment::top, PaintSegment::centre, PaintSegment::topRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 5:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP_DIAGONAL + 7)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::leftCorner, 13, height + 0, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP_DIAGONAL + 4)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topCorner, 13, height + 0, session.SupportColours);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::rightCorner, 13, height + 0, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP_DIAGONAL + 11)),
                        { -16, -16, height }, { { -16, -16, height + 32 }, { 32, 32, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomCorner, 13, height + 0, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre,
                        PaintSegment::topRight, PaintSegment::bottomRight, PaintSegment::right),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 96);
            break;
        case 7:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP_DIAGONAL + 2)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::topLeft, PaintSegment::top, PaintSegment::centre, PaintSegment::topRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 120);
            break;
        case 8:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP_DIAGONAL + 8)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 120);
            break;
        case 9:
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::leftCorner, 21, height + 1, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP_DIAGONAL + 5)),
                        { -16, -16, height }, { { -16, -16, height + 80 }, { 32, 32, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topCorner, 21, height + 0, session.SupportColours);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::rightCorner, 21, height + 0, session.SupportColours);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomCorner, 21, height + 0, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 136);
            break;
    }
}

static void LimLaunchedRCTrackDiag60DegUpToFlatLongBase(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP_DIAGONAL + 21)),
                        { -16, -16, height }, { { -8, -8, height + 0 }, { 16, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight, PaintSegment::right),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 96);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP_DIAGONAL + 12)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::topLeft, PaintSegment::top, PaintSegment::centre, PaintSegment::topRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 104);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP_DIAGONAL + 18)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 104);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::leftCorner, 30, height + 2, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP_DIAGONAL + 15)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topCorner, 30, height + 0, session.SupportColours);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::rightCorner, 30, height + 0, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP_DIAGONAL + 22)),
                        { -16, -16, height }, { { -8, -8, height + 56 }, { 16, 16, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomCorner, 30, height + 0, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre,
                        PaintSegment::topRight, PaintSegment::bottomRight, PaintSegment::right),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 112);
            break;
        case 4:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP_DIAGONAL + 13)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::topLeft, PaintSegment::top, PaintSegment::centre, PaintSegment::topRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
        case 5:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP_DIAGONAL + 19)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::leftCorner, 12, height + 0, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP_DIAGONAL + 16)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topCorner, 12, height + 0, session.SupportColours);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::rightCorner, 12, height + 0, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP_DIAGONAL + 23)),
                        { -16, -16, height }, { { -16, -16, height + 24 }, { 24, 24, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomCorner, 12, height + 0, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre,
                        PaintSegment::topRight, PaintSegment::bottomRight, PaintSegment::right),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 40);
            break;
        case 7:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP_DIAGONAL + 14)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::topLeft, PaintSegment::top, PaintSegment::centre, PaintSegment::topRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 40);
            break;
        case 8:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP_DIAGONAL + 20)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 40);
            break;
        case 9:
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::leftCorner, 5, height + 0, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP_DIAGONAL + 17)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topCorner, 5, height + 0, session.SupportColours);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::rightCorner, 5, height + 0, session.SupportColours);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomCorner, 5, height + 0, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 40);
            break;
    }
}

static void LimLaunchedRCTrackDiagFlatTo60DegDownLongBase(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP_DIAGONAL + 17)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight, PaintSegment::right),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 40);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP_DIAGONAL + 20)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::topLeft, PaintSegment::top, PaintSegment::centre, PaintSegment::topRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 40);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP_DIAGONAL + 14)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 40);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::leftCorner, 26, height + 0, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP_DIAGONAL + 23)),
                        { -16, -16, height }, { { -16, -16, height + 24 }, { 24, 24, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topCorner, 26, height + 0, session.SupportColours);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::rightCorner, 26, height + 0, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP_DIAGONAL + 16)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomCorner, 26, height + 0, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre,
                        PaintSegment::topRight, PaintSegment::bottomRight, PaintSegment::right),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 40);
            break;
        case 4:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP_DIAGONAL + 19)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::topLeft, PaintSegment::top, PaintSegment::centre, PaintSegment::topRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
        case 5:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP_DIAGONAL + 13)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::leftCorner, 60, height + 2, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP_DIAGONAL + 22)),
                        { -16, -16, height }, { { -8, -8, height + 56 }, { 16, 16, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topCorner, 60, height + 0, session.SupportColours);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::rightCorner, 60, height + 0, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP_DIAGONAL + 15)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomCorner, 60, height + 0, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre,
                        PaintSegment::topRight, PaintSegment::bottomRight, PaintSegment::right),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 112);
            break;
        case 7:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP_DIAGONAL + 18)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::topLeft, PaintSegment::top, PaintSegment::centre, PaintSegment::topRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 104);
            break;
        case 8:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP_DIAGONAL + 12)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 104);
            break;
        case 9:
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::leftCorner, 16, height + 3, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP_DIAGONAL + 21)),
                        { -16, -16, height }, { { -8, -8, height + 0 }, { 16, 16, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topCorner, 16, height + 0, session.SupportColours);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::rightCorner, 16, height + 0, session.SupportColours);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomCorner, 16, height + 0, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 96);
            break;
    }
}

static void LimLaunchedRCTrackDiag60DegDownToFlatLongBase(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP_DIAGONAL + 5)),
                        { -16, -16, height }, { { -16, -16, height + 80 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight, PaintSegment::right),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 136);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP_DIAGONAL + 8)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::topLeft, PaintSegment::top, PaintSegment::centre, PaintSegment::topRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 120);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP_DIAGONAL + 2)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 120);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::leftCorner, 40, height + 2, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP_DIAGONAL + 11)),
                        { -16, -16, height }, { { -16, -16, height + 32 }, { 32, 32, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topCorner, 40, height + 0, session.SupportColours);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::rightCorner, 40, height + 0, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP_DIAGONAL + 4)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomCorner, 40, height + 0, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre,
                        PaintSegment::topRight, PaintSegment::bottomRight, PaintSegment::right),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 96);
            break;
        case 4:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP_DIAGONAL + 7)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::topLeft, PaintSegment::top, PaintSegment::centre, PaintSegment::topRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 5:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP_DIAGONAL + 1)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::leftCorner, 14, height + 0, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP_DIAGONAL + 10)),
                        { -16, -16, height }, { { -8, -8, height + 8 }, { 24, 24, 1 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topCorner, 14, height + 0, session.SupportColours);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::rightCorner, 14, height + 0, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP_DIAGONAL + 3)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomCorner, 14, height + 0, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre,
                        PaintSegment::topRight, PaintSegment::bottomRight, PaintSegment::right),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
        case 7:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP_DIAGONAL + 6)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::topLeft, PaintSegment::top, PaintSegment::centre, PaintSegment::topRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 8:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP_DIAGONAL + 0)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 9:
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::leftCorner, 0, height + 0, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP_DIAGONAL + 9)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topCorner, 0, height + 0, session.SupportColours);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::rightCorner, 0, height + 0, session.SupportColours);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomCorner, 0, height + 0, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32);
            break;
    }
}

TrackPaintFunction GetTrackPaintFunctionLimLaunchedRC(TrackElemType trackType)
{
    switch (trackType)
    {
        // Use Looping Roller Coaster
        case TrackElemType::flat:
        case TrackElemType::up25:
        case TrackElemType::up60:
        case TrackElemType::flatToUp25:
        case TrackElemType::up25ToUp60:
        case TrackElemType::up60ToUp25:
        case TrackElemType::up25ToFlat:
        case TrackElemType::down25:
        case TrackElemType::down60:
        case TrackElemType::flatToDown25:
        case TrackElemType::down25ToDown60:
        case TrackElemType::down60ToDown25:
        case TrackElemType::down25ToFlat:
        case TrackElemType::leftQuarterTurn5Tiles:
        case TrackElemType::rightQuarterTurn5Tiles:
        case TrackElemType::flatToLeftBank:
        case TrackElemType::flatToRightBank:
        case TrackElemType::leftBankToFlat:
        case TrackElemType::rightBankToFlat:
        case TrackElemType::bankedLeftQuarterTurn5Tiles:
        case TrackElemType::bankedRightQuarterTurn5Tiles:
        case TrackElemType::leftBankToUp25:
        case TrackElemType::rightBankToUp25:
        case TrackElemType::up25ToLeftBank:
        case TrackElemType::up25ToRightBank:
        case TrackElemType::leftBankToDown25:
        case TrackElemType::rightBankToDown25:
        case TrackElemType::down25ToLeftBank:
        case TrackElemType::down25ToRightBank:
        case TrackElemType::leftBank:
        case TrackElemType::rightBank:
        case TrackElemType::leftQuarterTurn5TilesUp25:
        case TrackElemType::rightQuarterTurn5TilesUp25:
        case TrackElemType::leftQuarterTurn5TilesDown25:
        case TrackElemType::rightQuarterTurn5TilesDown25:
        case TrackElemType::sBendLeft:
        case TrackElemType::sBendRight:
        case TrackElemType::leftQuarterTurn3Tiles:
        case TrackElemType::rightQuarterTurn3Tiles:
        case TrackElemType::leftBankedQuarterTurn3Tiles:
        case TrackElemType::rightBankedQuarterTurn3Tiles:
        case TrackElemType::leftQuarterTurn3TilesUp25:
        case TrackElemType::rightQuarterTurn3TilesUp25:
        case TrackElemType::leftQuarterTurn3TilesDown25:
        case TrackElemType::rightQuarterTurn3TilesDown25:
        case TrackElemType::leftHalfBankedHelixUpSmall:
        case TrackElemType::rightHalfBankedHelixUpSmall:
        case TrackElemType::leftHalfBankedHelixDownSmall:
        case TrackElemType::rightHalfBankedHelixDownSmall:
        case TrackElemType::leftHalfBankedHelixUpLarge:
        case TrackElemType::rightHalfBankedHelixUpLarge:
        case TrackElemType::leftHalfBankedHelixDownLarge:
        case TrackElemType::rightHalfBankedHelixDownLarge:
        case TrackElemType::leftQuarterTurn1TileUp60:
        case TrackElemType::rightQuarterTurn1TileUp60:
        case TrackElemType::leftQuarterTurn1TileDown60:
        case TrackElemType::rightQuarterTurn1TileDown60:
        case TrackElemType::up25LeftBanked:
        case TrackElemType::up25RightBanked:
        case TrackElemType::onRidePhoto:
        case TrackElemType::down25LeftBanked:
        case TrackElemType::down25RightBanked:
        case TrackElemType::leftEighthToDiag:
        case TrackElemType::rightEighthToDiag:
        case TrackElemType::leftEighthToOrthogonal:
        case TrackElemType::rightEighthToOrthogonal:
        case TrackElemType::leftEighthBankToDiag:
        case TrackElemType::rightEighthBankToDiag:
        case TrackElemType::leftEighthBankToOrthogonal:
        case TrackElemType::rightEighthBankToOrthogonal:
        case TrackElemType::diagFlat:
        case TrackElemType::diagUp25:
        case TrackElemType::diagUp60:
        case TrackElemType::diagFlatToUp25:
        case TrackElemType::diagUp25ToUp60:
        case TrackElemType::diagUp60ToUp25:
        case TrackElemType::diagUp25ToFlat:
        case TrackElemType::diagDown25:
        case TrackElemType::diagDown60:
        case TrackElemType::diagFlatToDown25:
        case TrackElemType::diagDown25ToDown60:
        case TrackElemType::diagDown60ToDown25:
        case TrackElemType::diagDown25ToFlat:
        case TrackElemType::diagFlatToLeftBank:
        case TrackElemType::diagFlatToRightBank:
        case TrackElemType::diagLeftBankToFlat:
        case TrackElemType::diagRightBankToFlat:
        case TrackElemType::diagLeftBankToUp25:
        case TrackElemType::diagRightBankToUp25:
        case TrackElemType::diagUp25ToLeftBank:
        case TrackElemType::diagUp25ToRightBank:
        case TrackElemType::diagLeftBankToDown25:
        case TrackElemType::diagRightBankToDown25:
        case TrackElemType::diagDown25ToLeftBank:
        case TrackElemType::diagDown25ToRightBank:
        case TrackElemType::diagLeftBank:
        case TrackElemType::diagRightBank:
        case TrackElemType::leftBankedQuarterTurn3TileUp25:
        case TrackElemType::rightBankedQuarterTurn3TileUp25:
        case TrackElemType::leftBankedQuarterTurn3TileDown25:
        case TrackElemType::rightBankedQuarterTurn3TileDown25:
        case TrackElemType::leftBankedQuarterTurn5TileUp25:
        case TrackElemType::rightBankedQuarterTurn5TileUp25:
        case TrackElemType::leftBankedQuarterTurn5TileDown25:
        case TrackElemType::rightBankedQuarterTurn5TileDown25:
        case TrackElemType::up25ToLeftBankedUp25:
        case TrackElemType::up25ToRightBankedUp25:
        case TrackElemType::leftBankedUp25ToUp25:
        case TrackElemType::rightBankedUp25ToUp25:
        case TrackElemType::down25ToLeftBankedDown25:
        case TrackElemType::down25ToRightBankedDown25:
        case TrackElemType::leftBankedDown25ToDown25:
        case TrackElemType::rightBankedDown25ToDown25:
        case TrackElemType::leftBankedFlatToLeftBankedUp25:
        case TrackElemType::rightBankedFlatToRightBankedUp25:
        case TrackElemType::leftBankedUp25ToLeftBankedFlat:
        case TrackElemType::rightBankedUp25ToRightBankedFlat:
        case TrackElemType::leftBankedFlatToLeftBankedDown25:
        case TrackElemType::rightBankedFlatToRightBankedDown25:
        case TrackElemType::leftBankedDown25ToLeftBankedFlat:
        case TrackElemType::rightBankedDown25ToRightBankedFlat:
        case TrackElemType::flatToLeftBankedUp25:
        case TrackElemType::flatToRightBankedUp25:
        case TrackElemType::leftBankedUp25ToFlat:
        case TrackElemType::rightBankedUp25ToFlat:
        case TrackElemType::flatToLeftBankedDown25:
        case TrackElemType::flatToRightBankedDown25:
        case TrackElemType::leftBankedDown25ToFlat:
        case TrackElemType::rightBankedDown25ToFlat:
            return GetTrackPaintFunctionLoopingRC(trackType);
        case TrackElemType::endStation:
        case TrackElemType::beginStation:
        case TrackElemType::middleStation:
            return LimLaunchedRCTrackStation;
        case TrackElemType::leftVerticalLoop:
            return LimLaunchedRCTrackLeftVerticalLoop;
        case TrackElemType::rightVerticalLoop:
            return LimLaunchedRCTrackRightVerticalLoop;
        case TrackElemType::leftTwistDownToUp:
            return LimLaunchedRCTrackLeftTwistDownToUp;
        case TrackElemType::rightTwistDownToUp:
            return LimLaunchedRCTrackRightTwistDownToUp;
        case TrackElemType::leftTwistUpToDown:
            return LimLaunchedRCTrackLeftTwistUpToDown;
        case TrackElemType::rightTwistUpToDown:
            return LimLaunchedRCTrackRightTwistUpToDown;
        case TrackElemType::leftCorkscrewUp:
            return LimLaunchedRCTrackLeftCorkscrewUp;
        case TrackElemType::rightCorkscrewUp:
            return LimLaunchedRCTrackRightCorkscrewUp;
        case TrackElemType::leftCorkscrewDown:
            return LimLaunchedRCTrackLeftCorkscrewDown;
        case TrackElemType::rightCorkscrewDown:
            return LimLaunchedRCTrackRightCorkscrewDown;
        case TrackElemType::brakes:
            return LimLaunchedRCTrackBrakes;
        case TrackElemType::up90:
            return LimLaunchedRCTrack90DegUp;
        case TrackElemType::down90:
            return LimLaunchedRCTrack90DegDown;
        case TrackElemType::up60ToUp90:
            return LimLaunchedRCTrack60DegUpTo90DegUp;
        case TrackElemType::down90ToDown60:
            return LimLaunchedRCTrack90DegDownTo60DegDown;
        case TrackElemType::up90ToUp60:
            return LimLaunchedRCTrack90DegUpTo60DegUp;
        case TrackElemType::down60ToDown90:
            return LimLaunchedRCTrack60DegDownTo90DegDown;
        case TrackElemType::up90ToInvertedFlatQuarterLoop:
            return LimLaunchedRCTrack90DegToInvertedFlatQuarterLoopUp;
        case TrackElemType::invertedFlatToDown90QuarterLoop:
            return LimLaunchedRCTrackInvertedFlatTo90DegQuarterLoopDown;
        case TrackElemType::blockBrakes:
            return LimLaunchedRCTrackBlockBrakes;
        case TrackElemType::leftQuarterTurn1TileUp90:
            return LimLaunchedRCTrackLeftQuarterTurn190DegUp;
        case TrackElemType::rightQuarterTurn1TileUp90:
            return LimLaunchedRCTrackRightQuarterTurn190DegUp;
        case TrackElemType::leftQuarterTurn1TileDown90:
            return LimLaunchedRCTrackLeftQuarterTurn190DegDown;
        case TrackElemType::rightQuarterTurn1TileDown90:
            return LimLaunchedRCTrackRightQuarterTurn190DegDown;
        case TrackElemType::flatToUp60LongBase:
            return LimLaunchedRCTrackFlatTo60DegUpLongBase;
        case TrackElemType::up60ToFlatLongBase:
            return LimLaunchedRCTrack60DegUpToFlatLongBase;
        case TrackElemType::flatToDown60LongBase:
            return LimLaunchedRCTrackFlatTo60DegDownLongBase;
        case TrackElemType::down60ToFlatLongBase:
            return LimLaunchedRCTrack60DegDownToFlatLongBase;
        case TrackElemType::flatToUp60:
            return LimLaunchedRCTrackFlatTo60DegUp;
        case TrackElemType::up60ToFlat:
            return LimLaunchedRCTrack60DegUpToFlat;
        case TrackElemType::flatToDown60:
            return LimLaunchedRCTrackFlatTo60DegDown;
        case TrackElemType::down60ToFlat:
            return LimLaunchedRCTrack60DegDownToFlat;
        case TrackElemType::diagFlatToUp60:
            return LimLaunchedRCTrackDiagFlatTo60DegUp;
        case TrackElemType::diagUp60ToFlat:
            return LimLaunchedRCTrackDiag60DegUpToFlat;
        case TrackElemType::diagFlatToDown60:
            return LimLaunchedRCTrackDiagFlatTo60DegDown;
        case TrackElemType::diagDown60ToFlat:
            return LimLaunchedRCTrackDiag60DegDownToFlat;
        case TrackElemType::leftBarrelRollUpToDown:
            return LimLaunchedRCTrackLeftBarrelRollUpToDown;
        case TrackElemType::rightBarrelRollUpToDown:
            return LimLaunchedRCTrackRightBarrelRollUpToDown;
        case TrackElemType::leftBarrelRollDownToUp:
            return LimLaunchedRCTrackLeftBarrelRollDownToUp;
        case TrackElemType::rightBarrelRollDownToUp:
            return LimLaunchedRCTrackRightBarrelRollDownToUp;
        case TrackElemType::leftBankToLeftQuarterTurn3TilesUp25:
            return LimLaunchedRCTrackLeftBankToLeftQuarterTurn325DegUp;
        case TrackElemType::rightBankToRightQuarterTurn3TilesUp25:
            return LimLaunchedRCTrackRightBankToRightQuarterTurn325DegUp;
        case TrackElemType::leftQuarterTurn3TilesDown25ToLeftBank:
            return LimLaunchedRCTrackLeftQuarterTurn325DegDownToLeftBank;
        case TrackElemType::rightQuarterTurn3TilesDown25ToRightBank:
            return LimLaunchedRCTrackRightQuarterTurn325DegDownToRightBank;
        case TrackElemType::halfLoopUp:
            return LimLaunchedRCTrackHalfLoopUp;
        case TrackElemType::halfLoopDown:
            return LimLaunchedRCTrackHalfLoopDown;
        case TrackElemType::leftMediumHalfLoopUp:
            return LimLaunchedRCTrackLeftMediumHalfLoopUp;
        case TrackElemType::rightMediumHalfLoopUp:
            return LimLaunchedRCTrackRightMediumHalfLoopUp;
        case TrackElemType::leftMediumHalfLoopDown:
            return LimLaunchedRCTrackLeftMediumHalfLoopDown;
        case TrackElemType::rightMediumHalfLoopDown:
            return LimLaunchedRCTrackRightMediumHalfLoopDown;
        case TrackElemType::leftLargeHalfLoopUp:
            return LimLaunchedRCTrackLeftLargeHalfLoopUp;
        case TrackElemType::rightLargeHalfLoopUp:
            return LimLaunchedRCTrackRightLargeHalfLoopUp;
        case TrackElemType::leftLargeHalfLoopDown:
            return LimLaunchedRCTrackLeftLargeHalfLoopDown;
        case TrackElemType::rightLargeHalfLoopDown:
            return LimLaunchedRCTrackRightLargeHalfLoopDown;
        case TrackElemType::leftLargeCorkscrewUp:
            return LimLaunchedRCTrackLeftLargeCorkscrewUp;
        case TrackElemType::rightLargeCorkscrewUp:
            return LimLaunchedRCTrackRightLargeCorkscrewUp;
        case TrackElemType::leftLargeCorkscrewDown:
            return LimLaunchedRCTrackLeftLargeCorkscrewDown;
        case TrackElemType::rightLargeCorkscrewDown:
            return LimLaunchedRCTrackRightLargeCorkscrewDown;
        case TrackElemType::leftZeroGRollUp:
            return LimLaunchedRCTrackLeftZeroGRollUp;
        case TrackElemType::rightZeroGRollUp:
            return LimLaunchedRCTrackRightZeroGRollUp;
        case TrackElemType::leftZeroGRollDown:
            return LimLaunchedRCTrackLeftZeroGRollDown;
        case TrackElemType::rightZeroGRollDown:
            return LimLaunchedRCTrackRightZeroGRollDown;
        case TrackElemType::leftLargeZeroGRollUp:
            return LimLaunchedRCTrackLeftLargeZeroGRollUp;
        case TrackElemType::rightLargeZeroGRollUp:
            return LimLaunchedRCTrackRightLargeZeroGRollUp;
        case TrackElemType::leftLargeZeroGRollDown:
            return LimLaunchedRCTrackLeftLargeZeroGRollDown;
        case TrackElemType::rightLargeZeroGRollDown:
            return LimLaunchedRCTrackRightLargeZeroGRollDown;
        case TrackElemType::booster:
            return LimLaunchedRCTrackBooster;
        case TrackElemType::leftQuarterHelixLargeUp:
            return OpenRCT2::trackPaintLeftQuarterHelixLargeUp<
                SPR_TRACKS_LIM_LAUNCHED_TRACK_QUARTER_HELIX_LEFT, OpenRCT2::kLeftQuarterHelixLargeUpSpriteMap, false,
                kLeftQuarterHelixSupportHeights, OpenRCT2::BlockedSegmentsType::narrow, kTunnelGroup, false>;
        case TrackElemType::rightQuarterHelixLargeUp:
            return OpenRCT2::trackPaintRightQuarterHelixLargeUp<
                SPR_TRACKS_LIM_LAUNCHED_TRACK_QUARTER_HELIX_RIGHT, OpenRCT2::kRightQuarterHelixLargeUpSpriteMap, false,
                kRightQuarterHelixSupportHeights, OpenRCT2::BlockedSegmentsType::narrow, kTunnelGroup, false>;
        case TrackElemType::leftQuarterHelixLargeDown:
            return OpenRCT2::trackPaintLeftQuarterHelixLargeDown<
                SPR_TRACKS_LIM_LAUNCHED_TRACK_QUARTER_HELIX_RIGHT, OpenRCT2::kRightQuarterHelixLargeUpSpriteMap, false,
                kRightQuarterHelixSupportHeights, OpenRCT2::BlockedSegmentsType::narrow, kTunnelGroup, false>;
        case TrackElemType::rightQuarterHelixLargeDown:
            return OpenRCT2::trackPaintRightQuarterHelixLargeDown<
                SPR_TRACKS_LIM_LAUNCHED_TRACK_QUARTER_HELIX_LEFT, OpenRCT2::kLeftQuarterHelixLargeUpSpriteMap, false,
                kLeftQuarterHelixSupportHeights, OpenRCT2::BlockedSegmentsType::narrow, kTunnelGroup, false>;
        case TrackElemType::leftQuarterBankedHelixLargeUp:
            return OpenRCT2::trackPaintLeftQuarterBankedHelixLargeUp<
                SPR_TRACKS_LIM_LAUNCHED_TRACK_QUARTER_HELIX_LEFT_BANKED, OpenRCT2::kLeftQuarterBankedHelixLargeUpSpriteMap,
                false, kLeftQuarterBankedHelixSupportHeights, OpenRCT2::BlockedSegmentsType::narrow, kTunnelGroup, false>;
        case TrackElemType::rightQuarterBankedHelixLargeUp:
            return OpenRCT2::trackPaintRightQuarterBankedHelixLargeUp<
                SPR_TRACKS_LIM_LAUNCHED_TRACK_QUARTER_HELIX_RIGHT_BANKED, OpenRCT2::kRightQuarterBankedHelixLargeUpSpriteMap,
                false, kRightQuarterBankedHelixSupportHeights, OpenRCT2::BlockedSegmentsType::narrow, kTunnelGroup, false>;
        case TrackElemType::leftQuarterBankedHelixLargeDown:
            return OpenRCT2::trackPaintLeftQuarterBankedHelixLargeDown<
                SPR_TRACKS_LIM_LAUNCHED_TRACK_QUARTER_HELIX_RIGHT_BANKED, OpenRCT2::kRightQuarterBankedHelixLargeUpSpriteMap,
                false, kRightQuarterBankedHelixSupportHeights, OpenRCT2::BlockedSegmentsType::narrow, kTunnelGroup, false>;
        case TrackElemType::rightQuarterBankedHelixLargeDown:
            return OpenRCT2::trackPaintRightQuarterBankedHelixLargeDown<
                SPR_TRACKS_LIM_LAUNCHED_TRACK_QUARTER_HELIX_LEFT_BANKED, OpenRCT2::kLeftQuarterBankedHelixLargeUpSpriteMap,
                false, kLeftQuarterBankedHelixSupportHeights, OpenRCT2::BlockedSegmentsType::narrow, kTunnelGroup, false>;
            // Large banked curved slopes
        case TrackElemType::leftEighthBankToDiagUp25:
            return LimLaunchedRCTrackLeftEighthBankToDiagUp25;
        case TrackElemType::rightEighthBankToDiagUp25:
            return LimLaunchedRCTrackRightEighthBankToDiagUp25;
        case TrackElemType::leftEighthBankToDiagDown25:
            return LimLaunchedRCTrackLeftEighthBankToDiagDown25;
        case TrackElemType::rightEighthBankToDiagDown25:
            return LimLaunchedRCTrackRightEighthBankToDiagDown25;
        case TrackElemType::leftEighthBankToOrthogonalUp25:
            return LimLaunchedRCTrackLeftEighthBankToOrthogonalUp25;
        case TrackElemType::rightEighthBankToOrthogonalUp25:
            return LimLaunchedRCTrackRightEighthBankToOrthogonalUp25;
        case TrackElemType::leftEighthBankToOrthogonalDown25:
            return LimLaunchedRCTrackLeftEighthBankToOrthogonalDown25;
        case TrackElemType::rightEighthBankToOrthogonalDown25:
            return LimLaunchedRCTrackRightEighthBankToOrthogonalDown25;

            // Large Unbanked Curved Slopes
            // Large curved slopes
        case TrackElemType::leftEighthToDiagUp25:
            return LimLaunchedRCTrackLeftEighthToDiagUp25;
        case TrackElemType::rightEighthToDiagUp25:
            return LimLaunchedRCTrackRightEighthToDiagUp25;
        case TrackElemType::leftEighthToDiagDown25:
            return LimLaunchedRCTrackLeftEighthToDiagDown25;
        case TrackElemType::rightEighthToDiagDown25:
            return LimLaunchedRCTrackRightEighthToDiagDown25;
        case TrackElemType::leftEighthToOrthogonalUp25:
            return LimLaunchedRCTrackLeftEighthToOrthogonalUp25;
        case TrackElemType::rightEighthToOrthogonalUp25:
            return LimLaunchedRCTrackRightEighthToOrthogonalUp25;
        case TrackElemType::leftEighthToOrthogonalDown25:
            return LimLaunchedRCTrackLeftEighthToOrthogonalDown25;
        case TrackElemType::rightEighthToOrthogonalDown25:
            return LimLaunchedRCTrackRightEighthToOrthogonalDown25;

        case TrackElemType::leftEighthDiveLoopUpToOrthogonal:
            return LimLaunchedRCTrackLeftEighthDiveLoopUpToOrthogonal;
        case TrackElemType::rightEighthDiveLoopUpToOrthogonal:
            return LimLaunchedRCTrackRightEighthDiveLoopUpToOrthogonal;
        case TrackElemType::leftEighthDiveLoopDownToDiag:
            return LimLaunchedRCTrackLeftEighthDiveLoopDownToDiag;
        case TrackElemType::rightEighthDiveLoopDownToDiag:
            return LimLaunchedRCTrackRightEighthDiveLoopToDownOrthogonal;

        // diag banked tranisitons
        case TrackElemType::diagUp25ToLeftBankedUp25:
            return LimLaunchedRCTrackDiagUp25ToLeftBankedUp25;
        case TrackElemType::diagUp25ToRightBankedUp25:
            return LimLaunchedRCTrackDiagUp25ToRightBankedUp25;
        case TrackElemType::diagLeftBankedUp25ToUp25:
            return LimLaunchedRCTrackDiagLeftBankedUp25ToUp25;
        case TrackElemType::diagRightBankedUp25ToUp25:
            return LimLaunchedRCTrackDiagRightBankedUp25ToUp25;
        case TrackElemType::diagDown25ToLeftBankedDown25:
            return LimLaunchedRCTrackDiagDown25ToLeftBankedDown25;
        case TrackElemType::diagDown25ToRightBankedDown25:
            return LimLaunchedRCTrackDiagDown25ToRightBankedDown25;
        case TrackElemType::diagLeftBankedDown25ToDown25:
            return LimLaunchedRCTrackDiagLeftBankedDown25ToDown25;
        case TrackElemType::diagRightBankedDown25ToDown25:
            return LimLaunchedRCTrackDiagRightBankedDown25ToDown25;
        case TrackElemType::diagLeftBankedFlatToLeftBankedUp25:
            return LimLaunchedRCTrackDiagLeftBankedFlatToLeftBankedUp25;
        case TrackElemType::diagRightBankedFlatToRightBankedUp25:
            return LimLaunchedRCTrackDiagRightBankedFlatToRightBankedUp25;
        case TrackElemType::diagLeftBankedUp25ToLeftBankedFlat:
            return LimLaunchedRCTrackDiagLeftBankedUp25ToLeftBankedFlat;
        case TrackElemType::diagRightBankedUp25ToRightBankedFlat:
            return LimLaunchedRCTrackDiagRightBankedUp25ToRightBankedFlat;
        case TrackElemType::diagLeftBankedFlatToLeftBankedDown25:
            return LimLaunchedRCTrackDiagLeftBankedFlatToLeftBankedDown25;
        case TrackElemType::diagRightBankedFlatToRightBankedDown25:
            return LimLaunchedRCTrackDiagRightBankedFlatToRightBankedDown25;
        case TrackElemType::diagLeftBankedDown25ToLeftBankedFlat:
            return LimLaunchedRCTrackDiagLeftBankedDown25ToLeftBankedFlat;
        case TrackElemType::diagRightBankedDown25ToRightBankedFlat:
            return LimLaunchedRCTrackDiagRightBankedDown25ToRightBankedFlat;
        case TrackElemType::diagUp25LeftBanked:
            return LimLaunchedRCTrackDiagUp25LeftBanked;
        case TrackElemType::diagUp25RightBanked:
            return LimLaunchedRCTrackDiagUp25RightBanked;
        case TrackElemType::diagDown25LeftBanked:
            return LimLaunchedRCTrackDiagDown25LeftBanked;
        case TrackElemType::diagDown25RightBanked:
            return LimLaunchedRCTrackDiagDown25RightBanked;
        case TrackElemType::diagFlatToLeftBankedUp25:
            return LimLaunchedRCTrackDiagFlatToLeftBankedUp25;
        case TrackElemType::diagFlatToRightBankedUp25:
            return LimLaunchedRCTrackDiagFlatToRightBankedUp25;
        case TrackElemType::diagLeftBankedUp25ToFlat:
            return LimLaunchedRCTrackDiagLeftBankedUp25ToFlat;
        case TrackElemType::diagRightBankedUp25ToFlat:
            return LimLaunchedRCTrackDiagRightBankedUp25ToFlat;
        case TrackElemType::diagFlatToLeftBankedDown25:
            return LimLaunchedRCTrackDiagFlatToLeftBankedDown25;
        case TrackElemType::diagFlatToRightBankedDown25:
            return LimLaunchedRCTrackDiagFlatToRightBankedDown25;
        case TrackElemType::diagLeftBankedDown25ToFlat:
            return LimLaunchedRCTrackDiagLeftBankedDown25ToFlat;
        case TrackElemType::diagRightBankedDown25ToFlat:
            return LimLaunchedRCTrackDiagRightBankedDown25ToFlat;

        // diag flat to steep long
        case TrackElemType::diagFlatToUp60LongBase:
            return LimLaunchedRCTrackDiagFlatTo60DegUpLongBase;
        case TrackElemType::diagUp60ToFlatLongBase:
            return LimLaunchedRCTrackDiag60DegUpToFlatLongBase;
        case TrackElemType::diagFlatToDown60LongBase:
            return LimLaunchedRCTrackDiagFlatTo60DegDownLongBase;
        case TrackElemType::diagDown60ToFlatLongBase:
            return LimLaunchedRCTrackDiag60DegDownToFlatLongBase;

        default:
            return TrackPaintFunctionDummy;
    }
}
