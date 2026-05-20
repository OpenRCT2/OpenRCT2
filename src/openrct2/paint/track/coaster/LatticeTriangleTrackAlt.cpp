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

using namespace OpenRCT2;

static constexpr TunnelGroup kTunnelGroup = TunnelGroup::Square;

static constexpr uint32_t kLatticeTriangleAltBrakeImages[kNumOrthogonalDirections][2] = {
    { SPR_TRACKS_LATTICE_TRIANGLE_TRACK_BRAKE_ALT_OPEN_NE_SW, SPR_TRACKS_LATTICE_TRIANGLE_TRACK_BRAKE_ALT_CLOSED_NE_SW },
    { SPR_TRACKS_LATTICE_TRIANGLE_TRACK_BRAKE_ALT_OPEN_NW_SE, SPR_TRACKS_LATTICE_TRIANGLE_TRACK_BRAKE_ALT_CLOSED_NW_SE },
    { SPR_TRACKS_LATTICE_TRIANGLE_TRACK_BRAKE_ALT_OPEN_SW_NE, SPR_TRACKS_LATTICE_TRIANGLE_TRACK_BRAKE_ALT_CLOSED_SW_NE },
    { SPR_TRACKS_LATTICE_TRIANGLE_TRACK_BRAKE_ALT_OPEN_SE_NW, SPR_TRACKS_LATTICE_TRIANGLE_TRACK_BRAKE_ALT_CLOSED_SE_NW },
};

static constexpr uint32_t kLatticeTriangleAltBlockBrakeImages[kNumOrthogonalDirections][2] = {
    { SPR_TRACKS_LATTICE_TRIANGLE_TRACK_BLOCK_BRAKE_ALT_OPEN_NE_SW,
      SPR_TRACKS_LATTICE_TRIANGLE_TRACK_BLOCK_BRAKE_ALT_CLOSED_NE_SW },
    { SPR_TRACKS_LATTICE_TRIANGLE_TRACK_BLOCK_BRAKE_ALT_OPEN_NW_SE,
      SPR_TRACKS_LATTICE_TRIANGLE_TRACK_BLOCK_BRAKE_ALT_CLOSED_NW_SE },
    { SPR_TRACKS_LATTICE_TRIANGLE_TRACK_BLOCK_BRAKE_ALT_OPEN_SW_NE,
      SPR_TRACKS_LATTICE_TRIANGLE_TRACK_BLOCK_BRAKE_ALT_CLOSED_SW_NE },
    { SPR_TRACKS_LATTICE_TRIANGLE_TRACK_BLOCK_BRAKE_ALT_OPEN_SE_NW,
      SPR_TRACKS_LATTICE_TRIANGLE_TRACK_BLOCK_BRAKE_ALT_CLOSED_SE_NW },
};

static void LatticeTriangleTrackAltStation(
    PaintSession& session, const Ride& ride, [[maybe_unused]] uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    if (ride.mode == RideMode::poweredLaunch || ride.mode == RideMode::poweredLaunchBlockSectioned
        || ride.mode == RideMode::poweredLaunchPasstrough)
    {
        static constexpr ImageIndex imageIds[4] = {
            SPR_TRACKS_LATTICE_TRIANGLE_TRACK_BOOSTER_ALT_NE_SW,
            SPR_TRACKS_LATTICE_TRIANGLE_TRACK_BOOSTER_ALT_NW_SE,
            SPR_TRACKS_LATTICE_TRIANGLE_TRACK_BOOSTER_ALT_SW_NE,
            SPR_TRACKS_LATTICE_TRIANGLE_TRACK_BOOSTER_ALT_SE_NW,
        };

        paintAddImageAsParentRotated(
            session, direction, session.trackColours.WithIndex(imageIds[direction]), { 0, 0, height },
            { { 0, 6, height + 3 }, { 32, 20, 1 } });
    }
    else
    {
        static constexpr uint32_t imageIds[4] = {
            SPR_TRACKS_LATTICE_TRIANGLE_TRACK_DRIVE_TYRE_NE_SW,
            SPR_TRACKS_LATTICE_TRIANGLE_TRACK_DRIVE_TYRE_NW_SE,
            SPR_TRACKS_LATTICE_TRIANGLE_TRACK_DRIVE_TYRE_SW_NE,
            SPR_TRACKS_LATTICE_TRIANGLE_TRACK_DRIVE_TYRE_SE_NW,
        };
        if (trackElement.GetTrackType() == TrackElemType::endStation)
        {
            bool isClosed = trackElement.IsBrakeClosed();
            paintAddImageAsParentRotated(
                session, direction, session.trackColours.WithIndex(kLatticeTriangleAltBlockBrakeImages[direction][isClosed]),
                { 0, 0, height }, { { 0, 6, height + 3 }, { 32, 20, 1 } });
        }
        else
        {
            paintAddImageAsParentRotated(
                session, direction, session.trackColours.WithIndex(imageIds[direction]), { 0, 0, height },
                { { 0, 6, height + 3 }, { 32, 20, 1 } });
        }
    }
    if (TrackPaintUtilDrawStation2(session, ride, direction, height, trackElement, StationBaseType::a, 0, 9, 11))
    {
        DrawSupportsSideBySide(session, direction, height, session.supportColours, supportType.metal);
    }
    else if (TrackPaintUtilShouldPaintSupports(session.mapPosition))
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::centre, direction, 0, height, session.supportColours);
    }
    TrackPaintUtilDrawStationTunnel(session, direction, height);
    paintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    paintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

static void LatticeTriangleTrackAltBrakes(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    bool isClosed = trackElement.IsBrakeClosed();
    paintAddImageAsParentRotated(
        session, direction, session.trackColours.WithIndex(kLatticeTriangleAltBrakeImages[direction][isClosed]),
        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });

    if (TrackPaintUtilShouldPaintSupports(session.mapPosition))
    {
        MetalASupportsPaintSetup(
            session, MetalSupportType::tubes, MetalSupportPlace::centre, 0, height, session.supportColours);
    }
    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    paintUtilSetSegmentSupportHeight(session, paintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    paintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

static void LatticeTriangleTrackAltBlockBrakes(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    bool isClosed = trackElement.IsBrakeClosed();
    paintAddImageAsParentRotated(
        session, direction, session.trackColours.WithIndex(kLatticeTriangleAltBlockBrakeImages[direction][isClosed]),
        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });

    if (TrackPaintUtilShouldPaintSupports(session.mapPosition))
    {
        MetalASupportsPaintSetup(
            session, MetalSupportType::tubes, MetalSupportPlace::centre, 0, height, session.supportColours);
    }
    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    paintUtilSetSegmentSupportHeight(session, paintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    paintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

static void LatticeTriangleTrackAltBooster(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
            paintAddImageAsParentRotated(
                session, direction, session.trackColours.WithIndex(SPR_TRACKS_LATTICE_TRIANGLE_TRACK_BOOSTER_ALT_NE_SW),
                { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
            break;
        case 1:
            paintAddImageAsParentRotated(
                session, direction, session.trackColours.WithIndex(SPR_TRACKS_LATTICE_TRIANGLE_TRACK_BOOSTER_ALT_NW_SE),
                { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
            break;
        case 2:
            paintAddImageAsParentRotated(
                session, direction, session.trackColours.WithIndex(SPR_TRACKS_LATTICE_TRIANGLE_TRACK_BOOSTER_ALT_SW_NE),
                { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
            break;
        case 3:
            paintAddImageAsParentRotated(
                session, direction, session.trackColours.WithIndex(SPR_TRACKS_LATTICE_TRIANGLE_TRACK_BOOSTER_ALT_SE_NW),
                { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
            break;
    }
    if (TrackPaintUtilShouldPaintSupports(session.mapPosition))
    {
        MetalASupportsPaintSetup(
            session, MetalSupportType::tubes, MetalSupportPlace::centre, 0, height, session.supportColours);
    }
    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    paintUtilSetSegmentSupportHeight(session, paintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    paintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

static void LatticeTriangleTrackAltPoweredLift(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    paintAddImageAsParentRotated(
        session, direction, session.trackColours.WithIndex(SPR_TRACKS_LATTICE_TRIANGLE_TRACK_POWERED_LIFT_ALT_1 + direction),
        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });

    if (TrackPaintUtilShouldPaintSupports(session.mapPosition))
    {
        MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::centre, 8, height, session.supportColours);
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
    }
    paintUtilSetSegmentSupportHeight(session, paintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    paintUtilSetGeneralSupportHeight(session, height + 56);
}

static void LatticeTriangleTrackAltDiagBooster(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    constexpr ImageIndex images[kNumOrthogonalDirections] = { SPR_TRACKS_LATTICE_TRIANGLE_TRACK_DIAG_BOOSTER_ALT_1,
                                                              SPR_TRACKS_LATTICE_TRIANGLE_TRACK_DIAG_BOOSTER_ALT_2,
                                                              SPR_TRACKS_LATTICE_TRIANGLE_TRACK_DIAG_BOOSTER_ALT_3,
                                                              SPR_TRACKS_LATTICE_TRIANGLE_TRACK_DIAG_BOOSTER_ALT_4 };

    TrackPaintUtilDiagTilesPaintExtra(session, 3, height, direction, trackSequence, images, supportType.metal);
}

static void LatticeTriangleTrackAltDiagBlockBrake(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    constexpr ImageIndex images[2][kNumOrthogonalDirections] = {
        {
            SPR_TRACKS_LATTICE_TRIANGLE_TRACK_DIAG_BLOCK_BRAKE_OPEN_ALT_1,
            SPR_TRACKS_LATTICE_TRIANGLE_TRACK_DIAG_BLOCK_BRAKE_OPEN_ALT_2,
            SPR_TRACKS_LATTICE_TRIANGLE_TRACK_DIAG_BLOCK_BRAKE_OPEN_ALT_3,
            SPR_TRACKS_LATTICE_TRIANGLE_TRACK_DIAG_BLOCK_BRAKE_OPEN_ALT_4,
        },
        {
            SPR_TRACKS_LATTICE_TRIANGLE_TRACK_DIAG_BLOCK_BRAKE_CLOSED_ALT_1,
            SPR_TRACKS_LATTICE_TRIANGLE_TRACK_DIAG_BLOCK_BRAKE_CLOSED_ALT_2,
            SPR_TRACKS_LATTICE_TRIANGLE_TRACK_DIAG_BLOCK_BRAKE_CLOSED_ALT_3,
            SPR_TRACKS_LATTICE_TRIANGLE_TRACK_DIAG_BLOCK_BRAKE_CLOSED_ALT_4,
        },
    };

    TrackPaintUtilDiagTilesPaintExtra(
        session, 3, height, direction, trackSequence, images[trackElement.IsBrakeClosed()], supportType.metal);
}

static void LatticeTriangleTrackAltDiagBrake(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    constexpr ImageIndex images[2][kNumOrthogonalDirections] = {
        {
            SPR_TRACKS_LATTICE_TRIANGLE_TRACK_DIAG_BRAKE_OPEN_ALT_1,
            SPR_TRACKS_LATTICE_TRIANGLE_TRACK_DIAG_BRAKE_OPEN_ALT_2,
            SPR_TRACKS_LATTICE_TRIANGLE_TRACK_DIAG_BRAKE_OPEN_ALT_3,
            SPR_TRACKS_LATTICE_TRIANGLE_TRACK_DIAG_BRAKE_OPEN_ALT_4,
        },
        {
            SPR_TRACKS_LATTICE_TRIANGLE_TRACK_DIAG_BRAKE_CLOSED_ALT_1,
            SPR_TRACKS_LATTICE_TRIANGLE_TRACK_DIAG_BRAKE_CLOSED_ALT_2,
            SPR_TRACKS_LATTICE_TRIANGLE_TRACK_DIAG_BRAKE_CLOSED_ALT_3,
            SPR_TRACKS_LATTICE_TRIANGLE_TRACK_DIAG_BRAKE_CLOSED_ALT_4,
        },
    };

    TrackPaintUtilDiagTilesPaintExtra(
        session, 3, height, direction, trackSequence, images[trackElement.IsBrakeClosed()], supportType.metal);
}

TrackPaintFunction GetTrackPaintFunctionLatticeTriangleTrackAlt(TrackElemType trackType)
{
    switch (trackType)
    {
        case TrackElemType::endStation:
        case TrackElemType::beginStation:
        case TrackElemType::middleStation:
            return LatticeTriangleTrackAltStation;
        case TrackElemType::brakes:
            return LatticeTriangleTrackAltBrakes;
        case TrackElemType::blockBrakes:
            return LatticeTriangleTrackAltBlockBrakes;
        case TrackElemType::booster:
            return LatticeTriangleTrackAltBooster;
        case TrackElemType::poweredLift:
            return LatticeTriangleTrackAltPoweredLift;
        case TrackElemType::diagBooster:
            return LatticeTriangleTrackAltDiagBooster;
        case TrackElemType::diagBlockBrakes:
            return LatticeTriangleTrackAltDiagBlockBrake;
        case TrackElemType::diagBrakes:
            return LatticeTriangleTrackAltDiagBrake;

        default:
            return GetTrackPaintFunctionLatticeTriangleTrack(trackType);
    }
}
