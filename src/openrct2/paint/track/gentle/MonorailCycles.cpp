/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../../interface/Viewport.h"
#include "../../../ride/RideData.h"
#include "../../../ride/Track.h"
#include "../../../ride/TrackPaint.h"
#include "../../../world/Map.h"
#include "../../Paint.h"
#include "../../support/MetalSupports.h"
#include "../../tile_element/Segment.h"
#include "../../track/Segment.h"
#include "../../track/Support.h"

using namespace OpenRCT2;

static constexpr TunnelGroup kTunnelGroup = TunnelGroup::Standard;

/** rct2: 0x0088AD48 */
static void PaintMonorailCyclesTrackFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);

    if (direction & 1)
    {
        PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
    }
    else
    {
        PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
    }

    MetalASupportsPaintSetupRotated(
        session, supportType.metal, MetalSupportPlace::Centre, direction, -1, height, session.SupportColours);

    PaintUtilSetSegmentSupportHeight(
        session,
        PaintUtilRotateSegments(
            EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight), direction),
        0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x0088ADD8 */
static void PaintMonorailCyclesStation(
    PaintSession& session, const Ride& ride, [[maybe_unused]] uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSpriteStation(session, ride, trackSequence, direction, height, trackElement, supportType);

    TrackPaintUtilDrawStationTunnel(session, direction, height);

    DrawSupportsSideBySide(session, direction, height, session.SupportColours, MetalSupportType::Boxed);

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x0088AD88 */
static void PaintMonorailCyclesTrackLeftQuarterTurn3Tiles(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);
    TrackPaintUtilLeftQuarterTurn3TilesTunnel(session, kTunnelGroup, TunnelSubType::Flat, height, direction, trackSequence);

    switch (trackSequence)
    {
        case 0:
            MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::Centre, -1, height, session.SupportColours);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight, PaintSegment::top),
                    direction),
                0xFFFF, 0);
            break;
        case 2:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::topLeft, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::left),
                    direction),
                0xFFFF, 0);
            break;
        case 3:
            MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::Centre, -1, height, session.SupportColours);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::topLeft, PaintSegment::centre, PaintSegment::bottomRight, PaintSegment::bottom),
                    direction),
                0xFFFF, 0);
            break;
    }

    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

static constexpr uint8_t monorail_cycles_right_quarter_turn_3_tiles_to_left_turn_map[] = {
    3,
    1,
    2,
    0,
};

/** rct2: 0x0088AD98 */
static void PaintMonorailCyclesTrackRightQuarterTurn3Tiles(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = monorail_cycles_right_quarter_turn_3_tiles_to_left_turn_map[trackSequence];
    PaintMonorailCyclesTrackLeftQuarterTurn3Tiles(
        session, ride, trackSequence, (direction + 3) % 4, height, trackElement, supportType);
}

static constexpr int8_t kMonorailCyclesTrackRightQuarterTurn5TilesSupportHeightOffset[][7] = {
    { -2, 0, -2, 0, 0, -3, -1 },
    { -3, 0, 0, 0, 0, 0, 0 },
    { 0 },
    { 0, 0, 0, 0, 0, -2, -3 },
};

static constexpr int8_t kMonorailCyclesTrackRightQuarterTurn5TilesSupportSpecial[][7] = {
    { 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 1 },
    { 0, 0, 1, 0, 0, 1, 1 },
    { 1, 0, 0, 0, 0, 0, 0 },
};

/** rct2: 0x0088ADB8 */
static void PaintMonorailCyclesTrackRightQuarterTurn5Tiles(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);

    int32_t supportHeight = height + kMonorailCyclesTrackRightQuarterTurn5TilesSupportHeightOffset[direction][trackSequence];
    int32_t supportSpecial = kMonorailCyclesTrackRightQuarterTurn5TilesSupportSpecial[direction][trackSequence];
    switch (trackSequence)
    {
        case 0:
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::Centre, direction, supportSpecial, supportHeight,
                session.SupportColours);
            break;
        case 2:
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::BottomRightSide, direction, supportSpecial, supportHeight,
                session.SupportColours);
            break;
        case 5:
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::BottomRightSide, DirectionNext(direction), supportSpecial,
                supportHeight, session.SupportColours);
            break;
        case 6:
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::Centre, DirectionNext(direction), supportSpecial, supportHeight,
                session.SupportColours);
            break;
    }

    if (direction == 0 && trackSequence == 0)
    {
        PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
    }

    if (direction == 0 && trackSequence == 6)
    {
        PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
    }

    if (direction == 1 && trackSequence == 6)
    {
        PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
    }

    if (direction == 3 && trackSequence == 0)
    {
        PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
    }

    switch (trackSequence)
    {
        case 0:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight, PaintSegment::right),
                    direction),
                0xFFFF, 0);
            break;
        case 2:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottomRight, PaintSegment::right,
                        PaintSegment::bottom, PaintSegment::topRight),
                    direction),
                0xFFFF, 0);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::top, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::centre),
                    direction),
                0xFFFF, 0);
            break;
        case 5:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottomRight, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::left,
                        PaintSegment::bottom, PaintSegment::topLeft),
                    direction),
                0xFFFF, 0);
            break;
        case 6:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::bottomRight, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::left),
                    direction),
                0xFFFF, 0);
            break;
    }

    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x0088ADA8 */
static void PaintMonorailCyclesTrackLeftQuarterTurn5Tiles(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    PaintMonorailCyclesTrackRightQuarterTurn5Tiles(
        session, ride, trackSequence, (direction + 1) % 4, height, trackElement, supportType);
}

/** rct2: 0x0088ADC8 */
static void PaintMonorailCyclesTrackSBendLeft(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);

    uint8_t originalTrackSequence = trackSequence;
    if (direction == 2 || direction == 3)
    {
        trackSequence = 3 - trackSequence;
    }

    if (direction == 0 || direction == 2)
    {
        if (trackSequence == 0)
        {
            PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
        }
    }
    else
    {
        if (trackSequence == 3)
        {
            PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
        }
    }

    DrawSBendLeftSupports(session, supportType.metal, originalTrackSequence, direction, height, 0, 0);

    switch (trackSequence)
    {
        case 0:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight, PaintSegment::top),
                    direction & 1),
                0xFFFF, 0);
            break;
        case 1:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight, PaintSegment::left,
                        PaintSegment::topLeft, PaintSegment::top),
                    direction & 1),
                0xFFFF, 0);
            break;
        case 2:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottom,
                        PaintSegment::bottomRight, PaintSegment::right),
                    direction & 1),
                0xFFFF, 0);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottom),
                    direction & 1),
                0xFFFF, 0);
            break;
    }
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x*/
static void PaintMonorailCyclesTrackSBendRight(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);

    uint8_t originalTrackSequence = trackSequence;
    if (direction == 2 || direction == 3)
    {
        trackSequence = 3 - trackSequence;
    }

    if (direction == 0 || direction == 2)
    {
        if (trackSequence == 0)
        {
            PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
        }
    }
    else
    {
        if (trackSequence == 3)
        {
            PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
        }
    }
    DrawSBendRightSupports(
        session, supportType.metal, originalTrackSequence, direction, trackSequence == 1 ? height - 2 : height, 0, 0);

    switch (trackSequence)
    {
        case 0:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight, PaintSegment::right),
                    direction & 1),
                0xFFFF, 0);
            break;
        case 1:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottom,
                        PaintSegment::bottomRight, PaintSegment::right),
                    direction & 1),
                0xFFFF, 0);
            break;
        case 2:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight, PaintSegment::left,
                        PaintSegment::topLeft, PaintSegment::top),
                    direction & 1),
                0xFFFF, 0);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight, PaintSegment::left),
                    direction & 1),
                0xFFFF, 0);
            break;
    }
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/**
 * rct2: 0x0088ac88
 */
TrackPaintFunction GetTrackPaintFunctionMonorailCycles(OpenRCT2::TrackElemType trackType)
{
    switch (trackType)
    {
        case TrackElemType::Flat:
            return PaintMonorailCyclesTrackFlat;

        case TrackElemType::EndStation:
        case TrackElemType::BeginStation:
        case TrackElemType::MiddleStation:
            return PaintMonorailCyclesStation;

        case TrackElemType::LeftQuarterTurn5Tiles:
            return PaintMonorailCyclesTrackLeftQuarterTurn5Tiles;
        case TrackElemType::RightQuarterTurn5Tiles:
            return PaintMonorailCyclesTrackRightQuarterTurn5Tiles;

        case TrackElemType::SBendLeft:
            return PaintMonorailCyclesTrackSBendLeft;
        case TrackElemType::SBendRight:
            return PaintMonorailCyclesTrackSBendRight;

        case TrackElemType::LeftQuarterTurn3Tiles:
            return PaintMonorailCyclesTrackLeftQuarterTurn3Tiles;
        case TrackElemType::RightQuarterTurn3Tiles:
            return PaintMonorailCyclesTrackRightQuarterTurn3Tiles;
        default:
            return TrackPaintFunctionDummy;
    }
}
