/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../../SpriteIds.h"
#include "../../../interface/Viewport.h"
#include "../../../ride/RideData.h"
#include "../../../ride/Track.h"
#include "../../../ride/TrackPaint.h"
#include "../../../world/Map.h"
#include "../../../world/tile_element/PathElement.h"
#include "../../../world/tile_element/TileElement.h"
#include "../../../world/tile_element/TrackElement.h"
#include "../../Paint.h"
#include "../../support/WoodenSupports.h"
#include "../../tile_element/Segment.h"
#include "../../track/Segment.h"
#include "../../track/Support.h"

using namespace OpenRCT2;

static constexpr TunnelGroup kTunnelGroup = TunnelGroup::Square;

/** rct2: 0x008AD0C0 */
static void PaintMiniatureRailwayTrackFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintFlatRailwayCrossing(session, ride, trackSequence, direction, height, trackElement, supportType);

    WoodenASupportsPaintSetupRotated(
        session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours);

    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x008AD170, 0x008AD180, 0x008AD190 */
static void PaintMiniatureRailwayStation(
    PaintSession& session, const Ride& ride, [[maybe_unused]] uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);

    TrackPaintUtilDrawStationTunnel(session, direction, height);

    TrackPaintUtilDrawStation2(session, ride, direction, height, trackElement, StationBaseType::a, -2, 7);

    WoodenASupportsPaintSetupRotated(
        session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours);

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 30);
}

/** rct2: 0x008AD0D0 */
static void PaintMiniatureRailwayTrack25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);

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

    WoodenASupportsPaintSetupRotated(
        session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
        WoodenSupportTransitionType::Up25DegRailway);

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56);
}

/** rct2: 0x008AD0E0 */
static void PaintMiniatureRailwayTrackFlatTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);

    switch (direction)
    {
        case 0:
            PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
            break;
        case 1:
            PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::SlopeEnd);
            break;
        case 2:
            PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::SlopeEnd);
            break;
        case 3:
            PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
            break;
    }

    WoodenASupportsPaintSetupRotated(
        session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
        WoodenSupportTransitionType::FlatToUp25DegRailway);

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48);
}

/** rct2: 0x008AD0F0 */
static void PaintMiniatureRailwayTrack25DegUpToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);

    switch (direction)
    {
        case 0:
            PaintUtilPushTunnelLeft(session, height - 8, kTunnelGroup, TunnelSubType::Flat);
            break;
        case 1:
            PaintUtilPushTunnelRight(session, height + 8, kTunnelGroup, TunnelSubType::FlatTo25Deg);
            break;
        case 2:
            PaintUtilPushTunnelLeft(session, height + 8, kTunnelGroup, TunnelSubType::FlatTo25Deg);
            break;
        case 3:
            PaintUtilPushTunnelRight(session, height - 8, kTunnelGroup, TunnelSubType::Flat);
            break;
    }

    WoodenASupportsPaintSetupRotated(
        session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
        WoodenSupportTransitionType::Up25DegToFlatRailway);

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 40);
}

/** rct2: 0x008AD100 */
static void PaintMiniatureRailwayTrack25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    PaintMiniatureRailwayTrack25DegUp(session, ride, trackSequence, (direction + 2) % 4, height, trackElement, supportType);
}

/** rct2: 0x008AD110 */
static void PaintMiniatureRailwayTrackFlatTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    PaintMiniatureRailwayTrack25DegUpToFlat(
        session, ride, trackSequence, (direction + 2) % 4, height, trackElement, supportType);
}

/** rct2: 0x008AD120 */
static void PaintMiniatureRailwayTrack25DegDownToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    PaintMiniatureRailwayTrackFlatTo25DegUp(
        session, ride, trackSequence, (direction + 2) % 4, height, trackElement, supportType);
}

static constexpr WoodenSupportSubType right_quarter_turn_5_supports_type[4][7] = {
    { WoodenSupportSubType::NeSw, WoodenSupportSubType::Null, WoodenSupportSubType::Corner2, WoodenSupportSubType::Corner0,
      WoodenSupportSubType::Null, WoodenSupportSubType::Corner2, WoodenSupportSubType::NwSe },
    { WoodenSupportSubType::NwSe, WoodenSupportSubType::Null, WoodenSupportSubType::Corner3, WoodenSupportSubType::Corner1,
      WoodenSupportSubType::Null, WoodenSupportSubType::Corner3, WoodenSupportSubType::NeSw },
    { WoodenSupportSubType::NeSw, WoodenSupportSubType::Null, WoodenSupportSubType::Corner0, WoodenSupportSubType::Corner2,
      WoodenSupportSubType::Null, WoodenSupportSubType::Corner0, WoodenSupportSubType::NwSe },
    { WoodenSupportSubType::NwSe, WoodenSupportSubType::Null, WoodenSupportSubType::Corner1, WoodenSupportSubType::Corner3,
      WoodenSupportSubType::Null, WoodenSupportSubType::Corner1, WoodenSupportSubType::NeSw },
};

/** rct2: 0x008AD140 */
static void PaintMiniatureRailwayTrackRightQuarterTurn5Tiles(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);

    if (right_quarter_turn_5_supports_type[direction][trackSequence] != WoodenSupportSubType::Null)
    {
        WoodenASupportsPaintSetup(
            session, supportType.wooden, right_quarter_turn_5_supports_type[direction][trackSequence], height,
            session.SupportColours);
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

    int32_t blockedSegments = 0;
    switch (trackSequence)
    {
        case 0:
            blockedSegments = kSegmentsAll;
            break;
        case 1:
            blockedSegments = EnumsToFlags(PaintSegment::top, PaintSegment::topLeft, PaintSegment::topRight);
            break;
        case 2:
            blockedSegments = EnumsToFlags(
                PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottomRight, PaintSegment::right,
                PaintSegment::bottom, PaintSegment::topRight);
            break;
        case 3:
            blockedSegments = EnumsToFlags(
                PaintSegment::top, PaintSegment::left, PaintSegment::right, PaintSegment::centre, PaintSegment::topLeft,
                PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
            break;
        case 4:
            blockedSegments = EnumsToFlags(PaintSegment::top, PaintSegment::topLeft, PaintSegment::topRight);
            break;
        case 5:
            blockedSegments = EnumsToFlags(
                PaintSegment::bottomRight, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::left,
                PaintSegment::bottom, PaintSegment::topLeft);
            break;
        case 6:
            blockedSegments = kSegmentsAll;
            break;
    }

    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments, direction), 0xFFFF, 0);

    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x008AD130 */
static void PaintMiniatureRailwayTrackLeftQuarterTurn5Tiles(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    PaintMiniatureRailwayTrackRightQuarterTurn5Tiles(
        session, ride, trackSequence, (direction + 1) % 4, height, trackElement, supportType);
}

static constexpr WoodenSupportSubType s_bend_left_supports_type[kNumOrthogonalDirections][4] = {
    { WoodenSupportSubType::NeSw, WoodenSupportSubType::Corner3, WoodenSupportSubType::Corner1, WoodenSupportSubType::NeSw },
    { WoodenSupportSubType::NwSe, WoodenSupportSubType::Corner0, WoodenSupportSubType::Corner2, WoodenSupportSubType::NwSe },
    { WoodenSupportSubType::NeSw, WoodenSupportSubType::Corner3, WoodenSupportSubType::Corner1, WoodenSupportSubType::NeSw },
    { WoodenSupportSubType::NwSe, WoodenSupportSubType::Corner0, WoodenSupportSubType::Corner2, WoodenSupportSubType::NwSe },
};

/** rct2: 0x8AD150 */
static void PaintMiniatureRailwayTrackSBendLeft(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);

    if (direction == 2 || direction == 3)
    {
        trackSequence = 3 - trackSequence;
    }

    WoodenASupportsPaintSetup(
        session, supportType.wooden, s_bend_left_supports_type[direction][trackSequence], height, session.SupportColours);

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

    int32_t blockedSegments = 0;
    switch (trackSequence)
    {
        case 0:
            blockedSegments = kSegmentsAll;
            break;
        case 1:
            blockedSegments = EnumsToFlags(
                PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight, PaintSegment::left,
                PaintSegment::topLeft, PaintSegment::top);
            break;
        case 2:
            blockedSegments = EnumsToFlags(
                PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottom,
                PaintSegment::bottomRight, PaintSegment::right);
            break;
        case 3:
            blockedSegments = kSegmentsAll;
            break;
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments, direction & 1), 0xFFFF, 0);

    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

static constexpr WoodenSupportSubType s_bend_right_supports_type[kNumOrthogonalDirections][4] = {
    { WoodenSupportSubType::NeSw, WoodenSupportSubType::Corner2, WoodenSupportSubType::Corner0, WoodenSupportSubType::NeSw },
    { WoodenSupportSubType::NwSe, WoodenSupportSubType::Corner3, WoodenSupportSubType::Corner1, WoodenSupportSubType::NwSe },
    { WoodenSupportSubType::NeSw, WoodenSupportSubType::Corner2, WoodenSupportSubType::Corner0, WoodenSupportSubType::NeSw },
    { WoodenSupportSubType::NwSe, WoodenSupportSubType::Corner3, WoodenSupportSubType::Corner1, WoodenSupportSubType::NwSe },
};

/** rct2: 0x008AD160 */
static void PaintMiniatureRailwayTrackSBendRight(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);

    if (direction == 2 || direction == 3)
    {
        trackSequence = 3 - trackSequence;
    }

    WoodenASupportsPaintSetup(
        session, supportType.wooden, s_bend_right_supports_type[direction][trackSequence], height, session.SupportColours);

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

    int32_t blockedSegments = 0;
    switch (trackSequence)
    {
        case 0:
            blockedSegments = kSegmentsAll;
            break;
        case 1:
            blockedSegments = EnumsToFlags(
                PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottom,
                PaintSegment::bottomRight, PaintSegment::right);
            break;
        case 2:
            blockedSegments = EnumsToFlags(
                PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight, PaintSegment::left,
                PaintSegment::topLeft, PaintSegment::top);
            break;
        case 3:
            blockedSegments = kSegmentsAll;
            break;
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments, direction & 1), 0xFFFF, 0);

    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x008AD1B0 */
static void PaintMiniatureRailwayTrackRightQuarterTurn3Tiles(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);

    if (trackSequence != 1 && trackSequence != 2)
    {
        WoodenASupportsPaintSetupRotated(
            session, supportType.wooden, WoodenSupportSubType::Corner2, direction, height, session.SupportColours);
    }
    TrackPaintUtilRightQuarterTurn3TilesTunnel(session, kTunnelGroup, TunnelSubType::Flat, height, direction, trackSequence);

    int32_t blockedSegments = 0;
    switch (trackSequence)
    {
        case 0:
            blockedSegments = kSegmentsAll;
            break;
        case 2:
            blockedSegments = EnumsToFlags(
                PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottomRight, PaintSegment::bottom);
            break;
        case 3:
            blockedSegments = kSegmentsAll;
            break;
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments, direction), 0xFFFF, 0);

    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x008AD1A0 */
static void PaintMiniatureRailwayTrackLeftQuarterTurn3Tiles(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    PaintMiniatureRailwayTrackRightQuarterTurn3Tiles(
        session, ride, trackSequence, (direction + 1) % 4, height, trackElement, supportType);
}

/** rct2: 0x008AD1C0 */
static void PaintMiniatureRailwayTrackLeftEighthToDiag(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);

    static constexpr WoodenSupportSubType supportSubType[kNumOrthogonalDirections][5] = {
        { WoodenSupportSubType::NeSw, WoodenSupportSubType::NeSw, WoodenSupportSubType::Corner1, WoodenSupportSubType::Corner3,
          WoodenSupportSubType::NwSe },
        { WoodenSupportSubType::NwSe, WoodenSupportSubType::NwSe, WoodenSupportSubType::Corner2, WoodenSupportSubType::Corner0,
          WoodenSupportSubType::NeSw },
        { WoodenSupportSubType::NeSw, WoodenSupportSubType::NeSw, WoodenSupportSubType::Corner3, WoodenSupportSubType::Corner1,
          WoodenSupportSubType::NwSe },
        { WoodenSupportSubType::NwSe, WoodenSupportSubType::NwSe, WoodenSupportSubType::Corner0, WoodenSupportSubType::Corner2,
          WoodenSupportSubType::NeSw },
    };

    bool isRightEighthToOrthog = trackElement.GetTrackType() == TrackElemType::RightEighthToOrthogonal;
    // Right eighth to orthogonal calls this function but we do not want to have a support call for it
    // for track sequence 4
    if (trackSequence != 4 || !isRightEighthToOrthog)
    {
        WoodenASupportsPaintSetup(
            session, supportType.wooden, supportSubType[direction][trackSequence], height, session.SupportColours);
    }

    if (direction == 0 && trackSequence == 0)
    {
        PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
    }
    if (direction == 3 && trackSequence == 0)
    {
        PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
    }

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);

    if (trackSequence != 4 || !isRightEighthToOrthog)
        PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x008AD1D0 */
static void PaintMiniatureRailwayTrackRightEighthToDiag(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);

    static constexpr WoodenSupportSubType supportSubType[kNumOrthogonalDirections][5] = {
        { WoodenSupportSubType::NeSw, WoodenSupportSubType::NeSw, WoodenSupportSubType::Corner0, WoodenSupportSubType::Corner2,
          WoodenSupportSubType::NwSe },
        { WoodenSupportSubType::NwSe, WoodenSupportSubType::NwSe, WoodenSupportSubType::Corner1, WoodenSupportSubType::Corner3,
          WoodenSupportSubType::NeSw },
        { WoodenSupportSubType::NeSw, WoodenSupportSubType::NeSw, WoodenSupportSubType::Corner2, WoodenSupportSubType::Corner0,
          WoodenSupportSubType::NwSe },
        { WoodenSupportSubType::NwSe, WoodenSupportSubType::NwSe, WoodenSupportSubType::Corner3, WoodenSupportSubType::Corner1,
          WoodenSupportSubType::NeSw },
    };

    bool isLeftEighthToOrthog = trackElement.GetTrackType() == TrackElemType::LeftEighthToOrthogonal;
    // Right eighth to orthogonal calls this function but we do not want to have a support call for it
    // for track sequence 4
    if (trackSequence != 4 || !isLeftEighthToOrthog)
    {
        WoodenASupportsPaintSetup(
            session, supportType.wooden, supportSubType[direction][trackSequence], height, session.SupportColours);
    }

    if (direction == 0 && trackSequence == 0)
    {
        PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
    }
    if (direction == 3 && trackSequence == 0)
    {
        PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
    }

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);

    // Fixes #5281.
    if (trackSequence != 4 || !isLeftEighthToOrthog)
        PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x008AD1E0 */
static void PaintMiniatureRailwayTrackLeftEighthToOrthogonal(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    PaintMiniatureRailwayTrackRightEighthToDiag(
        session, ride, trackSequence, (direction + 2) % 4, height, trackElement, supportType);
}

/** rct2: 0x008AD1F0 */
static void PaintMiniatureRailwayTrackRightEighthToOrthogonal(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    PaintMiniatureRailwayTrackLeftEighthToDiag(
        session, ride, trackSequence, (direction + 3) % 4, height, trackElement, supportType);
}

static constexpr WoodenSupportSubType kDiagSupportTypes[kNumOrthogonalDirections][4] = {
    { WoodenSupportSubType::Null, WoodenSupportSubType::Corner0, WoodenSupportSubType::Corner2, WoodenSupportSubType::NeSw },
    { WoodenSupportSubType::Null, WoodenSupportSubType::Corner1, WoodenSupportSubType::Corner3, WoodenSupportSubType::NwSe },
    { WoodenSupportSubType::Null, WoodenSupportSubType::Corner2, WoodenSupportSubType::Corner0, WoodenSupportSubType::NeSw },
    { WoodenSupportSubType::Null, WoodenSupportSubType::Corner3, WoodenSupportSubType::Corner1, WoodenSupportSubType::NwSe },
};

/** rct2: 0x008AD200 */
static void MiniatureRailwayTrackDiagFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);

    auto supportSubType = kDiagSupportTypes[direction][trackSequence];

    if (supportSubType != WoodenSupportSubType::Null)
    {
        WoodenASupportsPaintSetup(session, supportType.wooden, supportSubType, height, session.SupportColours);
    }

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);

    // Fixes #5281.
    if (supportSubType != WoodenSupportSubType::Null)
        PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x008AD230 */
static void MiniatureRailwayTrackDiag25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);

    static constexpr int8_t heightDiffs[] = { +8, -8, +8, -8 };
    int8_t heightDiff = heightDiffs[direction];
    if (trackSequence == 3)
    {
        heightDiff = 8;
    }

    auto supportSubType = kDiagSupportTypes[direction][trackSequence];
    if (supportSubType != WoodenSupportSubType::Null)
    {
        if (trackSequence == 3)
        {
            WoodenASupportsPaintSetup(session, supportType.wooden, supportSubType, height + heightDiff, session.SupportColours);
        }
        else
        {
            WoodenBSupportsPaintSetup(session, supportType.wooden, supportSubType, height + heightDiff, session.SupportColours);
        }
    }

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);

    // Fixes #5281
    if (supportSubType != WoodenSupportSubType::Null)
        PaintUtilSetGeneralSupportHeight(session, height + 56);
}

/** rct2: 0x008AD210 */
static void MiniatureRailwayTrackDiagFlatTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);

    auto supportSubType = kDiagSupportTypes[direction][trackSequence];
    if (supportSubType != WoodenSupportSubType::Null)
    {
        WoodenASupportsPaintSetup(session, supportType.wooden, supportSubType, height, session.SupportColours);
    }

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);

    if (supportSubType != WoodenSupportSubType::Null)
        PaintUtilSetGeneralSupportHeight(session, height + 48);
}

/** rct2: 0x008AD220 */
static void MiniatureRailwayTrackDiag25DegUpToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);

    static constexpr int8_t supportOffsets[][4] = {
        { 0, +8, +8, +8 },
        { 0, -8, -8, 0 },
        { 0, +8, +8, +8 },
        { 0, -8, -8, +8 },
    };

    auto supportSubType = kDiagSupportTypes[direction][trackSequence];

    if (supportSubType != WoodenSupportSubType::Null)
    {
        if (trackSequence == 3)
        {
            WoodenASupportsPaintSetup(
                session, supportType.wooden, supportSubType, height + supportOffsets[direction][trackSequence],
                session.SupportColours);
        }
        else
        {
            WoodenBSupportsPaintSetup(
                session, supportType.wooden, supportSubType, height + supportOffsets[direction][trackSequence],
                session.SupportColours);
        }
    }

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);

    if (supportSubType != WoodenSupportSubType::Null)
        PaintUtilSetGeneralSupportHeight(session, height + 56);
}

/** rct2: 0x008AD260 */
static void MiniatureRailwayTrackDiag25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    MiniatureRailwayTrackDiag25DegUp(
        session, ride, 3 - trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}

/** rct2: 0x008AD240 */
static void MiniatureRailwayTrackDiagFlatTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    MiniatureRailwayTrackDiag25DegUpToFlat(
        session, ride, 3 - trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}

static void MiniatureRailwayTrackDiag25DegDownToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    MiniatureRailwayTrackDiagFlatTo25DegUp(
        session, ride, 3 - trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}

/**
 * rct2: 0x008ACE48
 */
TrackPaintFunction GetTrackPaintFunctionMiniatureRailway(OpenRCT2::TrackElemType trackType)
{
    switch (trackType)
    {
        case TrackElemType::Flat:
            return PaintMiniatureRailwayTrackFlat;

        case TrackElemType::EndStation:
        case TrackElemType::BeginStation:
        case TrackElemType::MiddleStation:
            return PaintMiniatureRailwayStation;

        case TrackElemType::Up25:
            return PaintMiniatureRailwayTrack25DegUp;
        case TrackElemType::FlatToUp25:
            return PaintMiniatureRailwayTrackFlatTo25DegUp;
        case TrackElemType::Up25ToFlat:
            return PaintMiniatureRailwayTrack25DegUpToFlat;

        case TrackElemType::Down25:
            return PaintMiniatureRailwayTrack25DegDown;
        case TrackElemType::FlatToDown25:
            return PaintMiniatureRailwayTrackFlatTo25DegDown;
        case TrackElemType::Down25ToFlat:
            return PaintMiniatureRailwayTrack25DegDownToFlat;

        case TrackElemType::LeftQuarterTurn5Tiles:
            return PaintMiniatureRailwayTrackLeftQuarterTurn5Tiles;
        case TrackElemType::RightQuarterTurn5Tiles:
            return PaintMiniatureRailwayTrackRightQuarterTurn5Tiles;

        case TrackElemType::SBendLeft:
            return PaintMiniatureRailwayTrackSBendLeft;
        case TrackElemType::SBendRight:
            return PaintMiniatureRailwayTrackSBendRight;

        case TrackElemType::LeftQuarterTurn3Tiles:
            return PaintMiniatureRailwayTrackLeftQuarterTurn3Tiles;
        case TrackElemType::RightQuarterTurn3Tiles:
            return PaintMiniatureRailwayTrackRightQuarterTurn3Tiles;

        case TrackElemType::LeftEighthToDiag:
            return PaintMiniatureRailwayTrackLeftEighthToDiag;
        case TrackElemType::RightEighthToDiag:
            return PaintMiniatureRailwayTrackRightEighthToDiag;
        case TrackElemType::LeftEighthToOrthogonal:
            return PaintMiniatureRailwayTrackLeftEighthToOrthogonal;
        case TrackElemType::RightEighthToOrthogonal:
            return PaintMiniatureRailwayTrackRightEighthToOrthogonal;

        case TrackElemType::DiagFlat:
            return MiniatureRailwayTrackDiagFlat;
        case TrackElemType::DiagUp25:
            return MiniatureRailwayTrackDiag25DegUp;
        case TrackElemType::DiagFlatToUp25:
            return MiniatureRailwayTrackDiagFlatTo25DegUp;
        case TrackElemType::DiagUp25ToFlat:
            return MiniatureRailwayTrackDiag25DegUpToFlat;
        case TrackElemType::DiagDown25:
            return MiniatureRailwayTrackDiag25DegDown;
        case TrackElemType::DiagFlatToDown25:
            return MiniatureRailwayTrackDiagFlatTo25DegDown;
        case TrackElemType::DiagDown25ToFlat:
            return MiniatureRailwayTrackDiag25DegDownToFlat;
        default:
            return TrackPaintFunctionDummy;
    }
}
