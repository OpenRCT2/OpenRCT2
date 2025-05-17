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
#include "../../../ride/TrackData.h"
#include "../../../ride/TrackPaint.h"
#include "../../../ride/Vehicle.h"
#include "../../../world/Map.h"
#include "../../../world/tile_element/Slope.h"
#include "../../../world/tile_element/SurfaceElement.h"
#include "../../../world/tile_element/TrackElement.h"
#include "../../Paint.SessionFlags.h"
#include "../../Paint.h"
#include "../../support/MetalSupports.h"
#include "../../support/WoodenSupports.h"
#include "../../tile_element/Segment.h"
#include "../../track/Segment.h"
#include "../../track/Support.h"

#include <iterator>

using namespace OpenRCT2;

static constexpr TunnelGroup kTunnelGroup = TunnelGroup::Standard;

/** rct2: 0x0087F10C */
static void PaintMiniGolfTrackFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSpriteFence2(session, ride, trackSequence, direction, height, trackElement, supportType);
    MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::Centre, 0, height, session.SupportColours);
    PaintUtilSetSegmentSupportHeight(
        session,
        PaintUtilRotateSegments(
            EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight), direction),
        0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x0087F11C */
static void PaintMiniGolfTrack25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites3(session, ride, trackSequence, direction, height, trackElement, supportType);

    MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::Centre, 8, height, session.SupportColours);
    PaintUtilSetSegmentSupportHeight(
        session,
        PaintUtilRotateSegments(
            EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight), direction),
        0xFFFF, 0);

    switch (direction)
    {
        case 0:
            PaintUtilPushTunnelLeft(session, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
            break;
        case 1:
            PaintUtilPushTunnelRight(session, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
            break;
        case 2:
            PaintUtilPushTunnelLeft(session, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
            break;
        case 3:
            PaintUtilPushTunnelRight(session, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
            break;
    }

    PaintUtilSetGeneralSupportHeight(session, height + 56);
}

/** rct2: 0x0087F12C */
static void PaintMiniGolfTrackFlatTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites3(session, ride, trackSequence, direction, height, trackElement, supportType);

    MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::Centre, 0, height, session.SupportColours);
    PaintUtilSetSegmentSupportHeight(
        session,
        PaintUtilRotateSegments(
            EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight), direction),
        0xFFFF, 0);

    switch (direction)
    {
        case 0:
            PaintUtilPushTunnelLeft(session, height, TunnelType::PathAndMiniGolf);
            break;
        case 1:
            PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::SlopeEnd);
            break;
        case 2:
            PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::SlopeEnd);
            break;
        case 3:
            PaintUtilPushTunnelRight(session, height, TunnelType::PathAndMiniGolf);
            break;
    }

    PaintUtilSetGeneralSupportHeight(session, height + 48);
}

/** rct2: 0x0087F13C */
static void PaintMiniGolfTrack25DegUpToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites3(session, ride, trackSequence, direction, height, trackElement, supportType);

    MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::Centre, 8, height, session.SupportColours);
    PaintUtilSetSegmentSupportHeight(
        session,
        PaintUtilRotateSegments(
            EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight), direction),
        0xFFFF, 0);

    switch (direction)
    {
        case 0:
            PaintUtilPushTunnelLeft(session, height - 8, kTunnelGroup, TunnelSubType::Flat);
            break;
        case 1:
            PaintUtilPushTunnelRight(session, height + 8, TunnelType::PathAndMiniGolf);
            break;
        case 2:
            PaintUtilPushTunnelLeft(session, height + 8, TunnelType::PathAndMiniGolf);
            break;
        case 3:
            PaintUtilPushTunnelRight(session, height - 8, kTunnelGroup, TunnelSubType::Flat);
            break;
    }

    PaintUtilSetGeneralSupportHeight(session, height + 40);
}

/** rct2: 0x0087F14C */
static void PaintMiniGolfTrack25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    PaintMiniGolfTrack25DegUp(session, ride, trackSequence, (direction + 2) % 4, height, trackElement, supportType);
}

/** rct2: 0x0087F15C */
static void PaintMiniGolfTrackFlatTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    PaintMiniGolfTrack25DegUpToFlat(session, ride, trackSequence, (direction + 2) % 4, height, trackElement, supportType);
}

/** rct2: 0x0087F16C */
static void PaintMiniGolfTrack25DegDownToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    PaintMiniGolfTrackFlatTo25DegUp(session, ride, trackSequence, (direction + 2) % 4, height, trackElement, supportType);
}

/** rct2: 0x0087F17C, 0x0087F18C, 0x0087F19C */
static void PaintMiniGolfStation(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintStation1SpriteFences(session, ride, trackSequence, direction, height, trackElement, supportType);
    TrackPaintUtilDrawStationTunnel(session, direction, height);

    WoodenASupportsPaintSetupRotated(
        session, WoodenSupportType::Truss, WoodenSupportSubType::NeSw, direction, height, session.SupportColours);

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x0087F1AC */
static void PaintMiniGolfTrackLeftQuarterTurn1Tile(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSpriteFence3(session, ride, trackSequence, direction, height, trackElement, supportType);

    MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::Centre, 0, height, session.SupportColours);

    PaintUtilSetSegmentSupportHeight(
        session,
        PaintUtilRotateSegments(
            EnumsToFlags(PaintSegment::left, PaintSegment::topLeft, PaintSegment::centre, PaintSegment::bottomLeft), direction),
        0xFFFF, 0);

    switch (direction)
    {
        case 0:
            PaintUtilPushTunnelLeft(session, height, TunnelType::PathAndMiniGolf);
            break;
        case 2:
            PaintUtilPushTunnelRight(session, height, TunnelType::PathAndMiniGolf);
            break;

        case 3:
            PaintUtilPushTunnelLeft(session, height, TunnelType::PathAndMiniGolf);
            PaintUtilPushTunnelRight(session, height, TunnelType::PathAndMiniGolf);
            break;
    }

    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x0087F1BC */
static void PaintMiniGolfTrackRightQuarterTurn1Tile(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    PaintMiniGolfTrackLeftQuarterTurn1Tile(
        session, ride, trackSequence, (direction + 3) % 4, height, trackElement, supportType);
}

/** rct2: 0x0087F1CC */
static void PaintMiniGolfHoleA(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);
    WoodenASupportsPaintSetupRotated(
        session, WoodenSupportType::Truss, WoodenSupportSubType::NeSw, direction, height, session.SupportColours);

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);

    if ((direction == 0 && trackSequence == 0) || (direction == 2 && trackSequence == 1))
    {
        PaintUtilPushTunnelLeft(session, height, TunnelType::PathAndMiniGolf);
    }
    else if ((direction == 3 && trackSequence == 0) || (direction == 1 && trackSequence == 1))
    {
        PaintUtilPushTunnelRight(session, height, TunnelType::PathAndMiniGolf);
    }
}

/** rct2: 0x0087F1DC */
static void PaintMiniGolfHoleB(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);
    WoodenASupportsPaintSetupRotated(
        session, WoodenSupportType::Truss, WoodenSupportSubType::NeSw, direction, height, session.SupportColours);

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);

    if ((direction == 0 && trackSequence == 0) || (direction == 2 && trackSequence == 1))
    {
        PaintUtilPushTunnelLeft(session, height, TunnelType::PathAndMiniGolf);
    }
    else if ((direction == 3 && trackSequence == 0) || (direction == 1 && trackSequence == 1))
    {
        PaintUtilPushTunnelRight(session, height, TunnelType::PathAndMiniGolf);
    }
}

/** rct2: 0x0087F1EC */
static void PaintMiniGolfHoleC(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);

    WoodenASupportsPaintSetupRotated(
        session, WoodenSupportType::Truss, WoodenSupportSubType::NeSw, direction, height, session.SupportColours);

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);

    if ((direction == 0 && trackSequence == 0) || (direction == 2 && trackSequence == 1))
    {
        PaintUtilPushTunnelLeft(session, height, TunnelType::PathAndMiniGolf);
    }
    else if ((direction == 3 && trackSequence == 0) || (direction == 1 && trackSequence == 1))
    {
        PaintUtilPushTunnelRight(session, height, TunnelType::PathAndMiniGolf);
    }
}

/** rct2: 0x0087F1FC */
static void PaintMiniGolfHoleD(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);

    auto supportSubType = (direction & 1) ? WoodenSupportSubType::NwSe : WoodenSupportSubType::NeSw;
    if (trackSequence == 2)
        supportSubType = (direction & 1) ? WoodenSupportSubType::NeSw : WoodenSupportSubType::NwSe;
    WoodenASupportsPaintSetup(session, WoodenSupportType::Truss, supportSubType, height, session.SupportColours);

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);

    switch ((direction << 4) | trackSequence)
    {
        case 0x00:
        case 0x12:
            PaintUtilPushTunnelLeft(session, height, TunnelType::PathAndMiniGolf);
            break;

        case 0x02:
        case 0x30:
            PaintUtilPushTunnelRight(session, height, TunnelType::PathAndMiniGolf);
            break;
    }
}

/** rct2: 0x0087F1FC */
static void PaintMiniGolfHoleE(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);

    auto supportSubType = (direction & 1) ? WoodenSupportSubType::NwSe : WoodenSupportSubType::NeSw;
    if (trackSequence == 2)
        supportSubType = (direction & 1) ? WoodenSupportSubType::NeSw : WoodenSupportSubType::NwSe;
    WoodenASupportsPaintSetup(session, WoodenSupportType::Truss, supportSubType, height, session.SupportColours);

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);

    switch ((direction << 4) | trackSequence)
    {
        case (0 << 4 | 0):
        case (3 << 4 | 2):
            PaintUtilPushTunnelLeft(session, height, TunnelType::PathAndMiniGolf);
            break;

        case (2 << 4 | 2):
        case (3 << 4 | 0):
            PaintUtilPushTunnelRight(session, height, TunnelType::PathAndMiniGolf);
            break;
    }
}

/**
 * rct2: 0x0087EDC4
 */
TrackPaintFunction GetTrackPaintFunctionMiniGolf(OpenRCT2::TrackElemType trackType)
{
    switch (trackType)
    {
        case TrackElemType::Flat:
            return PaintMiniGolfTrackFlat;

        case TrackElemType::BeginStation:
        case TrackElemType::MiddleStation:
        case TrackElemType::EndStation:
            return PaintMiniGolfStation;

        case TrackElemType::Up25:
            return PaintMiniGolfTrack25DegUp;
        case TrackElemType::FlatToUp25:
            return PaintMiniGolfTrackFlatTo25DegUp;
        case TrackElemType::Up25ToFlat:
            return PaintMiniGolfTrack25DegUpToFlat;

        case TrackElemType::Down25:
            return PaintMiniGolfTrack25DegDown;
        case TrackElemType::FlatToDown25:
            return PaintMiniGolfTrackFlatTo25DegDown;
        case TrackElemType::Down25ToFlat:
            return PaintMiniGolfTrack25DegDownToFlat;

        case TrackElemType::LeftQuarterTurn1Tile:
            return PaintMiniGolfTrackLeftQuarterTurn1Tile;
        case TrackElemType::RightQuarterTurn1Tile:
            return PaintMiniGolfTrackRightQuarterTurn1Tile;

        case TrackElemType::MinigolfHoleA:
            return PaintMiniGolfHoleA;
        case TrackElemType::MinigolfHoleB:
            return PaintMiniGolfHoleB;
        case TrackElemType::MinigolfHoleC:
            return PaintMiniGolfHoleC;
        case TrackElemType::MinigolfHoleD:
            return PaintMiniGolfHoleD;
        case TrackElemType::MinigolfHoleE:
            return PaintMiniGolfHoleE;
        default:
            return TrackPaintFunctionDummy;
    }
}
