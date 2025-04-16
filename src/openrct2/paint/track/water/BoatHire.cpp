/*****************************************************************************
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
#include "../../../ride/Ride.h"
#include "../../../ride/Track.h"
#include "../../../ride/TrackPaint.h"
#include "../../Paint.h"
#include "../../tile_element/Segment.h"
#include "../../track/Segment.h"
#include "../../track/Support.h"

using namespace OpenRCT2;

/** rct2: 0x008B0E40 */
static void PaintBoatHireTrackFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);
    PaintUtilSetSegmentSupportHeight(
        session,
        PaintUtilRotateSegments(
            EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight), direction),
        0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 16);
}

/** rct2: 0x008B0E50 */
static void PaintBoatHireStation(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    const auto* stationObj = ride.getStationObject();

    TrackPaintUtilDrawStationTunnel(session, direction, height);
    TrackPaintUtilDrawPier(
        session, ride, stationObj, session.MapPosition, direction, height, trackElement, session.CurrentRotation);

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x008B0E80 */
static void PaintBoatHireTrackLeftQuarterTurn1Tile(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);
    PaintUtilSetSegmentSupportHeight(
        session,
        PaintUtilRotateSegments(EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topLeft), direction),
        0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 16);
}

/** rct2: 0x008B0E90 */
static void PaintBoatHireTrackRightQuarterTurn1Tile(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    PaintBoatHireTrackLeftQuarterTurn1Tile(
        session, ride, trackSequence, (direction + 3) % 4, height, trackElement, supportType);
}

static void PaintBoatHireTrackLeftQuarterTurn3Tiles(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 16);
}

static void PaintBoatHireTrackRightQuarterTurn3Tiles(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    PaintBoatHireTrackLeftQuarterTurn3Tiles(
        session, ride, trackSequence, DirectionPrev(direction), height, trackElement, supportType);
}

static void PaintBoatHireTrackLeftQuarterTurn5Tiles(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 16);
}

static void PaintBoatHireTrackRightQuarterTurn5Tiles(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    PaintBoatHireTrackLeftQuarterTurn5Tiles(
        session, ride, trackSequence, DirectionPrev(direction), height, trackElement, supportType);
}

static void PaintBoatHireTrackLeftEighthToDiag(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 16);
}

static void PaintBoatHireTrackRightEighthToDiag(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 16);
}

static void PaintBoatHireTrackLeftEighthToOrthogonal(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    PaintBoatHireTrackRightEighthToDiag(
        session, ride, trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}

static void PaintBoatHireTrackRightEighthToOrthogonal(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    PaintBoatHireTrackLeftEighthToDiag(
        session, ride, trackSequence, DirectionPrev(direction), height, trackElement, supportType);
}

static void PaintBoatHireTrackDiagFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 16);
}

static void PaintBoatHireTrackSBendLeft(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 16);
}

static void PaintBoatHireTrackSBendRight(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 16);
}

/**
 * rct2: 0x008B0D60
 */
TrackPaintFunction GetTrackPaintFunctionBoatHire(OpenRCT2::TrackElemType trackType)
{
    switch (trackType)
    {
        case TrackElemType::Flat:
            return PaintBoatHireTrackFlat;

        case TrackElemType::EndStation:
        case TrackElemType::BeginStation:
        case TrackElemType::MiddleStation:
            return PaintBoatHireStation;

        case TrackElemType::LeftQuarterTurn1Tile:
            return PaintBoatHireTrackLeftQuarterTurn1Tile;
        case TrackElemType::RightQuarterTurn1Tile:
            return PaintBoatHireTrackRightQuarterTurn1Tile;

        // Added by OpenRCT2

        // Small turns
        case TrackElemType::LeftQuarterTurn3Tiles:
            return PaintBoatHireTrackLeftQuarterTurn3Tiles;
        case TrackElemType::RightQuarterTurn3Tiles:
            return PaintBoatHireTrackRightQuarterTurn3Tiles;

        // Medium turns
        case TrackElemType::LeftQuarterTurn5Tiles:
            return PaintBoatHireTrackLeftQuarterTurn5Tiles;
        case TrackElemType::RightQuarterTurn5Tiles:
            return PaintBoatHireTrackRightQuarterTurn5Tiles;

        // Large turns
        case TrackElemType::LeftEighthToDiag:
            return PaintBoatHireTrackLeftEighthToDiag;
        case TrackElemType::RightEighthToDiag:
            return PaintBoatHireTrackRightEighthToDiag;
        case TrackElemType::LeftEighthToOrthogonal:
            return PaintBoatHireTrackLeftEighthToOrthogonal;
        case TrackElemType::RightEighthToOrthogonal:
            return PaintBoatHireTrackRightEighthToOrthogonal;

        // Diagonal
        case TrackElemType::DiagFlat:
            return PaintBoatHireTrackDiagFlat;

        // S bends
        case TrackElemType::SBendLeft:
            return PaintBoatHireTrackSBendLeft;
        case TrackElemType::SBendRight:
            return PaintBoatHireTrackSBendRight;

        default:
            return TrackPaintFunctionDummy;
    }
}
