/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
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
#include "../../Paint.h"
#include "../../support/MetalSupports.h"
#include "../../support/WoodenSupports.h"
#include "../../tile_element/Segment.h"
#include "../../track/Segment.h"
#include "../../track/Support.h"

using namespace OpenRCT2;

static constexpr TunnelGroup kTunnelGroup = TunnelGroup::Standard;

/** rct2: 0x */
static void PaintMiniHelicoptersTrackStation(
    PaintSession& session, const Ride& ride, [[maybe_unused]] uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    ImageId imageId;

    if (direction == 0 || direction == 2)
    {
        imageId = session.TrackColours.WithIndex(SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_NE_SW);
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 6, height + 1 }, { 32, 20, 1 } });
    }
    else if (direction == 1 || direction == 3)
    {
        imageId = session.TrackColours.WithIndex(SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_SE_NW);
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 6, 0, height + 1 }, { 20, 32, 1 } });
    }

    TrackPaintUtilDrawStationTunnel(session, direction, height);

    if (TrackPaintUtilDrawStation(session, ride, direction, height, trackElement, StationBaseType::b, -2))
    {
        DrawSupportsSideBySide(session, direction, height, session.SupportColours, MetalSupportType::boxed);
    }
    else if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::centre, direction, -1, height, session.SupportColours);
    }

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x0081F348 */
static void PaintMiniHelicoptersTrackFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    ImageId imageId;

    if (direction & 1)
    {
        imageId = session.TrackColours.WithIndex(SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_SE_NW);
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 6, 0, height }, { 20, 32, 3 } });
        PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
    }
    else
    {
        imageId = session.TrackColours.WithIndex(SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_NE_SW);
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
        PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
    }

    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::centre, direction, -1, height, session.SupportColours);
    }

    PaintUtilSetSegmentSupportHeight(
        session,
        PaintUtilRotateSegments(
            EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight), direction),
        0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x0081F368 */
static void PaintMiniHelicoptersTrackFlatTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    ImageId imageId;

    switch (direction)
    {
        case 0:
            imageId = session.TrackColours.WithIndex(SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_TO_25_DEG_UP_SW_NE);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
            PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
            break;
        case 1:
            imageId = session.TrackColours.WithIndex(SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_TO_25_DEG_UP_NW_SE);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 6, 0, height }, { 20, 32, 3 } });
            PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::SlopeEnd);
            break;
        case 2:
            imageId = session.TrackColours.WithIndex(SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_TO_25_DEG_UP_NE_SW);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
            PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::SlopeEnd);
            break;
        case 3:
            imageId = session.TrackColours.WithIndex(SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_TO_25_DEG_UP_SE_NW);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 6, 0, height }, { 20, 32, 3 } });
            PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
            break;
    }

    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::centre, -4, height, session.SupportColours);
    }

    PaintUtilSetSegmentSupportHeight(
        session,
        PaintUtilRotateSegments(
            EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight), direction),
        0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48);
}

/** rct2: 0x0081F358 */
static void PaintMiniHelicoptersTrack25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    ImageId imageId;

    switch (direction)
    {
        case 0:
            imageId = session.TrackColours.WithIndex(SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_25_DEG_UP_SW_NE);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
            PaintUtilPushTunnelLeft(session, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
            break;
        case 1:
            imageId = session.TrackColours.WithIndex(SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_25_DEG_UP_NW_SE);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 6, 0, height }, { 20, 32, 3 } });
            PaintUtilPushTunnelRight(session, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
            break;
        case 2:
            imageId = session.TrackColours.WithIndex(SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_25_DEG_UP_NE_SW);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
            PaintUtilPushTunnelLeft(session, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
            break;
        case 3:
            imageId = session.TrackColours.WithIndex(SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_25_DEG_UP_SE_NW);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 6, 0, height }, { 20, 32, 3 } });
            PaintUtilPushTunnelRight(session, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
            break;
    }

    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::centre, -9, height, session.SupportColours);
    }

    PaintUtilSetSegmentSupportHeight(
        session,
        PaintUtilRotateSegments(
            EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight), direction),
        0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56);
}

/** rct2: 0x0081F378 */
static void PaintMiniHelicoptersTrack25DegUpToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    ImageId imageId;

    switch (direction)
    {
        case 0:
            imageId = session.TrackColours.WithIndex(SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_25_DEG_UP_TO_FLAT_SW_NE);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
            PaintUtilPushTunnelLeft(session, height - 8, kTunnelGroup, TunnelSubType::Flat);
            break;
        case 1:
            imageId = session.TrackColours.WithIndex(SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_25_DEG_UP_TO_FLAT_NW_SE);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 6, 0, height }, { 20, 32, 3 } });
            PaintUtilPushTunnelRight(session, height + 8, kTunnelGroup, TunnelSubType::FlatTo25Deg);
            break;
        case 2:
            imageId = session.TrackColours.WithIndex(SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_25_DEG_UP_TO_FLAT_NE_SW);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
            PaintUtilPushTunnelLeft(session, height + 8, kTunnelGroup, TunnelSubType::FlatTo25Deg);
            break;
        case 3:
            imageId = session.TrackColours.WithIndex(SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_25_DEG_UP_TO_FLAT_SE_NW);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 6, 0, height }, { 20, 32, 3 } });
            PaintUtilPushTunnelRight(session, height - 8, kTunnelGroup, TunnelSubType::Flat);
            break;
    }

    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::centre, -7, height, session.SupportColours);
    }

    PaintUtilSetSegmentSupportHeight(
        session,
        PaintUtilRotateSegments(
            EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight), direction),
        0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 40);
}

/** rct2: 0x */
static void PaintMiniHelicoptersTrackFlatTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    PaintMiniHelicoptersTrack25DegUpToFlat(
        session, ride, trackSequence, (direction + 2) % 4, height, trackElement, supportType);
}

/** rct2: 0x0081F388 */
static void PaintMiniHelicoptersTrack25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    PaintMiniHelicoptersTrack25DegUp(session, ride, trackSequence, (direction + 2) % 4, height, trackElement, supportType);
}

/** rct2: 0x0081F3A8 */
static void PaintMiniHelicoptersTrack25DegDownToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    PaintMiniHelicoptersTrackFlatTo25DegUp(
        session, ride, trackSequence, (direction + 2) % 4, height, trackElement, supportType);
}

/** rct2: 0x0081F3E8 */
static void PaintMiniHelicoptersTrackLeftQuarterTurn3Tiles(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    TrackPaintUtilLeftQuarterTurn3TilesPaint(
        session, 3, height, direction, trackSequence, session.TrackColours,
        kTrackSpritesSubmarineRideMiniHelicoptersQuarterTurn3Tiles);
    TrackPaintUtilLeftQuarterTurn3TilesTunnel(session, kTunnelGroup, TunnelSubType::Flat, height, direction, trackSequence);

    switch (trackSequence)
    {
        case 0:
            MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::centre, -1, height, session.SupportColours);
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
            MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::centre, -1, height, session.SupportColours);
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

static constexpr uint8_t kMiniHelicoptersRightQuarterTurn3TilesToLeftTurnMap[] = {
    3,
    1,
    2,
    0,
};

/** rct2: 0x0081F3F8 */
static void PaintMiniHelicoptersTrackRightQuarterTurn3Tiles(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMiniHelicoptersRightQuarterTurn3TilesToLeftTurnMap[trackSequence];
    PaintMiniHelicoptersTrackLeftQuarterTurn3Tiles(
        session, ride, trackSequence, (direction + 3) % 4, height, trackElement, supportType);
}

/** rct2: 0x0081F408 */
static void PaintMiniHelicoptersTrackLeftQuarterTurn1Tile(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    TrackPaintUtilLeftQuarterTurn1TilePaint(
        session, 1, height, 0, direction, session.TrackColours, kTrackSpritesSubmarineRideMiniHelicoptersQuarterTurn1Tile);
    TrackPaintUtilLeftQuarterTurn1TileTunnel(
        session, kTunnelGroup, direction, height, 0, TunnelSubType::Flat, 0, TunnelSubType::Flat);

    PaintUtilSetSegmentSupportHeight(
        session,
        PaintUtilRotateSegments(
            EnumsToFlags(PaintSegment::left, PaintSegment::topLeft, PaintSegment::centre, PaintSegment::bottomLeft), direction),
        0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x0081F418 */
static void PaintMiniHelicoptersTrackRightQuarterTurn1Tile(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    PaintMiniHelicoptersTrackLeftQuarterTurn1Tile(
        session, ride, trackSequence, (direction + 3) % 4, height, trackElement, supportType);
}

static void PaintMiniHelicoptersTrackSpinningTunnel(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    const uint32_t sprites[kNumOrthogonalDirections][2] = {
        { SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_NE_SW, 28773 },
        { SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_SE_NW, 28774 },
        { SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_NE_SW, 28773 },
        { SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_SE_NW, 28774 },
    };

    ImageId imageId = session.TrackColours.WithIndex(sprites[direction][0]);
    ImageId underlay = session.TrackColours.WithIndex(sprites[direction][1]);

    PaintAddImageAsParentRotated(session, direction, underlay, { 0, 6, height - 2 }, { { 0, 6, height }, { 32, 20, 1 } });
    PaintAddImageAsChildRotated(session, direction, imageId, { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
    TrackPaintUtilSpinningTunnelPaint(session, 1, height, direction);
    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);

    WoodenASupportsPaintSetupRotated(
        session, WoodenSupportType::truss, WoodenSupportSubType::neSw, direction, height,
        GetStationColourScheme(session, trackElement));

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/**
 * rct2: 0x0081F268
 */
TrackPaintFunction GetTrackPaintFunctionMiniHelicopters(TrackElemType trackType)
{
    switch (trackType)
    {
        case TrackElemType::flat:
            return PaintMiniHelicoptersTrackFlat;

        case TrackElemType::endStation:
        case TrackElemType::beginStation:
        case TrackElemType::middleStation:
            return PaintMiniHelicoptersTrackStation;

        case TrackElemType::up25:
            return PaintMiniHelicoptersTrack25DegUp;
        case TrackElemType::flatToUp25:
            return PaintMiniHelicoptersTrackFlatTo25DegUp;
        case TrackElemType::up25ToFlat:
            return PaintMiniHelicoptersTrack25DegUpToFlat;

        case TrackElemType::down25:
            return PaintMiniHelicoptersTrack25DegDown;
        case TrackElemType::flatToDown25:
            return PaintMiniHelicoptersTrackFlatTo25DegDown;
        case TrackElemType::down25ToFlat:
            return PaintMiniHelicoptersTrack25DegDownToFlat;

        case TrackElemType::leftQuarterTurn3Tiles:
            return PaintMiniHelicoptersTrackLeftQuarterTurn3Tiles;
        case TrackElemType::rightQuarterTurn3Tiles:
            return PaintMiniHelicoptersTrackRightQuarterTurn3Tiles;

        case TrackElemType::leftQuarterTurn1Tile:
            return PaintMiniHelicoptersTrackLeftQuarterTurn1Tile;
        case TrackElemType::rightQuarterTurn1Tile:
            return PaintMiniHelicoptersTrackRightQuarterTurn1Tile;

        case TrackElemType::spinningTunnel:
            return PaintMiniHelicoptersTrackSpinningTunnel;
        default:
            return TrackPaintFunctionDummy;
    }
}
