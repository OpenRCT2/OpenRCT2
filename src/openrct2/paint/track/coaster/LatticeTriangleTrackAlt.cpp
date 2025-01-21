/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../../drawing/Drawing.h"
#include "../../../interface/Viewport.h"
#include "../../../ride/RideData.h"
#include "../../../ride/TrackData.h"
#include "../../../ride/TrackPaint.h"
#include "../../../sprites.h"
#include "../../../world/Map.h"
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
    { SPR_G2_LATTICE_TRIANGLE_TRACK_BRAKE_ALT_OPEN_NE_SW, SPR_G2_LATTICE_TRIANGLE_TRACK_BRAKE_ALT_CLOSED_NE_SW },
    { SPR_G2_LATTICE_TRIANGLE_TRACK_BRAKE_ALT_OPEN_NW_SE, SPR_G2_LATTICE_TRIANGLE_TRACK_BRAKE_ALT_CLOSED_NW_SE },
    { SPR_G2_LATTICE_TRIANGLE_TRACK_BRAKE_ALT_OPEN_SW_NE, SPR_G2_LATTICE_TRIANGLE_TRACK_BRAKE_ALT_CLOSED_SW_NE },
    { SPR_G2_LATTICE_TRIANGLE_TRACK_BRAKE_ALT_OPEN_SE_NW, SPR_G2_LATTICE_TRIANGLE_TRACK_BRAKE_ALT_CLOSED_SE_NW },
};

static constexpr uint32_t kLatticeTriangleAltBlockBrakeImages[kNumOrthogonalDirections][2] = {
    { SPR_G2_LATTICE_TRIANGLE_TRACK_BLOCK_BRAKE_ALT_OPEN_NE_SW, SPR_G2_LATTICE_TRIANGLE_TRACK_BLOCK_BRAKE_ALT_CLOSED_NE_SW },
    { SPR_G2_LATTICE_TRIANGLE_TRACK_BLOCK_BRAKE_ALT_OPEN_NW_SE, SPR_G2_LATTICE_TRIANGLE_TRACK_BLOCK_BRAKE_ALT_CLOSED_NW_SE },
    { SPR_G2_LATTICE_TRIANGLE_TRACK_BLOCK_BRAKE_ALT_OPEN_SW_NE, SPR_G2_LATTICE_TRIANGLE_TRACK_BLOCK_BRAKE_ALT_CLOSED_SW_NE },
    { SPR_G2_LATTICE_TRIANGLE_TRACK_BLOCK_BRAKE_ALT_OPEN_SE_NW, SPR_G2_LATTICE_TRIANGLE_TRACK_BLOCK_BRAKE_ALT_CLOSED_SE_NW },
};

static void LatticeTriangleTrackAltStation(
    PaintSession& session, const Ride& ride, [[maybe_unused]] uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    if (ride.mode == RideMode::PoweredLaunch || ride.mode == RideMode::PoweredLaunchBlockSectioned
        || ride.mode == RideMode::PoweredLaunchPasstrough)
    {
        static constexpr uint32_t imageIds[4][2] = {
            { SPR_G2_LATTICE_TRIANGLE_TRACK_BOOSTER_ALT_NE_SW, SPR_STATION_BASE_A_SW_NE },
            { SPR_G2_LATTICE_TRIANGLE_TRACK_BOOSTER_ALT_NW_SE, SPR_STATION_BASE_A_NW_SE },
            { SPR_G2_LATTICE_TRIANGLE_TRACK_BOOSTER_ALT_SW_NE, SPR_STATION_BASE_A_SW_NE },
            { SPR_G2_LATTICE_TRIANGLE_TRACK_BOOSTER_ALT_SE_NW, SPR_STATION_BASE_A_NW_SE },
        };

        PaintAddImageAsParentRotated(
            session, direction, session.TrackColours.WithIndex(imageIds[direction][0]), { 0, 0, height },
            { { 0, 6, height + 3 }, { 32, 20, 1 } });

        PaintAddImageAsParentRotated(
            session, direction, GetStationColourScheme(session, trackElement).WithIndex(imageIds[direction][1]),
            { 0, 0, height }, { 32, 32, 1 });
    }
    else
    {
        static constexpr uint32_t imageIds[4][2] = {
            { SPR_G2_LATTICE_TRIANGLE_TRACK_DRIVE_TYRE_NE_SW, SPR_STATION_BASE_A_SW_NE },
            { SPR_G2_LATTICE_TRIANGLE_TRACK_DRIVE_TYRE_NW_SE, SPR_STATION_BASE_A_NW_SE },
            { SPR_G2_LATTICE_TRIANGLE_TRACK_DRIVE_TYRE_SW_NE, SPR_STATION_BASE_A_SW_NE },
            { SPR_G2_LATTICE_TRIANGLE_TRACK_DRIVE_TYRE_SE_NW, SPR_STATION_BASE_A_NW_SE },
        };
        if (trackElement.GetTrackType() == TrackElemType::EndStation)
        {
            bool isClosed = trackElement.IsBrakeClosed();
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(kLatticeTriangleAltBlockBrakeImages[direction][isClosed]),
                { 0, 0, height }, { { 0, 6, height + 3 }, { 32, 20, 1 } });
        }
        else
        {
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(imageIds[direction][0]), { 0, 0, height },
                { { 0, 6, height + 3 }, { 32, 20, 1 } });
        }

        PaintAddImageAsParentRotated(
            session, direction, GetStationColourScheme(session, trackElement).WithIndex(imageIds[direction][1]),
            { 0, 0, height }, { 32, 32, 1 });
    }
    DrawSupportsSideBySide(session, direction, height, session.SupportColours, supportType.metal);
    TrackPaintUtilDrawStation2(session, ride, direction, height, trackElement, 9, 11);
    TrackPaintUtilDrawStationTunnel(session, direction, height);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

static void LatticeTriangleTrackAltBrakes(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    bool isClosed = trackElement.IsBrakeClosed();
    PaintAddImageAsParentRotated(
        session, direction, session.TrackColours.WithIndex(kLatticeTriangleAltBrakeImages[direction][isClosed]),
        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });

    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(
            session, MetalSupportType::Tubes, MetalSupportPlace::Centre, 0, height, session.SupportColours);
    }
    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

static void LatticeTriangleTrackAltBlockBrakes(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    bool isClosed = trackElement.IsBrakeClosed();
    PaintAddImageAsParentRotated(
        session, direction, session.TrackColours.WithIndex(kLatticeTriangleAltBlockBrakeImages[direction][isClosed]),
        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });

    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(
            session, MetalSupportType::Tubes, MetalSupportPlace::Centre, 0, height, session.SupportColours);
    }
    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

static void LatticeTriangleTrackAltBooster(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(SPR_G2_LATTICE_TRIANGLE_TRACK_BOOSTER_ALT_NE_SW),
                { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(SPR_G2_LATTICE_TRIANGLE_TRACK_BOOSTER_ALT_NW_SE),
                { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(SPR_G2_LATTICE_TRIANGLE_TRACK_BOOSTER_ALT_SW_NE),
                { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(SPR_G2_LATTICE_TRIANGLE_TRACK_BOOSTER_ALT_SE_NW),
                { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
            break;
    }
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(
            session, MetalSupportType::Tubes, MetalSupportPlace::Centre, 0, height, session.SupportColours);
    }
    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

static void LatticeTriangleTrackAltPoweredLift(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    PaintAddImageAsParentRotated(
        session, direction, session.TrackColours.WithIndex(SPR_G2_LATTICE_TRIANGLE_TRACK_POWERED_LIFT_ALT_1 + direction),
        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });

    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::Centre, 8, height, session.SupportColours);
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56);
}

static void LatticeTriangleTrackAltDiagBooster(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    constexpr ImageIndex images[kNumOrthogonalDirections] = { SPR_G2_LATTICE_TRIANGLE_TRACK_DIAG_BOOSTER_ALT_1,
                                                              SPR_G2_LATTICE_TRIANGLE_TRACK_DIAG_BOOSTER_ALT_2,
                                                              SPR_G2_LATTICE_TRIANGLE_TRACK_DIAG_BOOSTER_ALT_3,
                                                              SPR_G2_LATTICE_TRIANGLE_TRACK_DIAG_BOOSTER_ALT_4 };

    TrackPaintUtilDiagTilesPaintExtra(session, 3, height, direction, trackSequence, images, supportType.metal);
}

static void LatticeTriangleTrackAltDiagBlockBrake(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    constexpr ImageIndex images[2][kNumOrthogonalDirections] = {
        {
            SPR_G2_LATTICE_TRIANGLE_TRACK_DIAG_BLOCK_BRAKE_OPEN_ALT_1,
            SPR_G2_LATTICE_TRIANGLE_TRACK_DIAG_BLOCK_BRAKE_OPEN_ALT_2,
            SPR_G2_LATTICE_TRIANGLE_TRACK_DIAG_BLOCK_BRAKE_OPEN_ALT_3,
            SPR_G2_LATTICE_TRIANGLE_TRACK_DIAG_BLOCK_BRAKE_OPEN_ALT_4,
        },
        {
            SPR_G2_LATTICE_TRIANGLE_TRACK_DIAG_BLOCK_BRAKE_CLOSED_ALT_1,
            SPR_G2_LATTICE_TRIANGLE_TRACK_DIAG_BLOCK_BRAKE_CLOSED_ALT_2,
            SPR_G2_LATTICE_TRIANGLE_TRACK_DIAG_BLOCK_BRAKE_CLOSED_ALT_3,
            SPR_G2_LATTICE_TRIANGLE_TRACK_DIAG_BLOCK_BRAKE_CLOSED_ALT_4,
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
            SPR_G2_LATTICE_TRIANGLE_TRACK_DIAG_BRAKE_OPEN_ALT_1,
            SPR_G2_LATTICE_TRIANGLE_TRACK_DIAG_BRAKE_OPEN_ALT_2,
            SPR_G2_LATTICE_TRIANGLE_TRACK_DIAG_BRAKE_OPEN_ALT_3,
            SPR_G2_LATTICE_TRIANGLE_TRACK_DIAG_BRAKE_OPEN_ALT_4,
        },
        {
            SPR_G2_LATTICE_TRIANGLE_TRACK_DIAG_BRAKE_CLOSED_ALT_1,
            SPR_G2_LATTICE_TRIANGLE_TRACK_DIAG_BRAKE_CLOSED_ALT_2,
            SPR_G2_LATTICE_TRIANGLE_TRACK_DIAG_BRAKE_CLOSED_ALT_3,
            SPR_G2_LATTICE_TRIANGLE_TRACK_DIAG_BRAKE_CLOSED_ALT_4,
        },
    };

    TrackPaintUtilDiagTilesPaintExtra(
        session, 3, height, direction, trackSequence, images[trackElement.IsBrakeClosed()], supportType.metal);
}

TrackPaintFunction GetTrackPaintFunctionLatticeTriangleTrackAlt(TrackElemType trackType)
{
    switch (trackType)
    {
        case TrackElemType::EndStation:
        case TrackElemType::BeginStation:
        case TrackElemType::MiddleStation:
            return LatticeTriangleTrackAltStation;
        case TrackElemType::Brakes:
            return LatticeTriangleTrackAltBrakes;
        case TrackElemType::BlockBrakes:
            return LatticeTriangleTrackAltBlockBrakes;
        case TrackElemType::Booster:
            return LatticeTriangleTrackAltBooster;
        case TrackElemType::PoweredLift:
            return LatticeTriangleTrackAltPoweredLift;
        case TrackElemType::DiagBooster:
            return LatticeTriangleTrackAltDiagBooster;
        case TrackElemType::DiagBlockBrakes:
            return LatticeTriangleTrackAltDiagBlockBrake;
        case TrackElemType::DiagBrakes:
            return LatticeTriangleTrackAltDiagBrake;

        default:
            return GetTrackPaintFunctionLatticeTriangleTrack(trackType);
    }
}
