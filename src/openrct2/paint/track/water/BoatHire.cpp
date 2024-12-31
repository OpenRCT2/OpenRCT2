/*****************************************************************************
/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../../interface/Viewport.h"
#include "../../../ride/Ride.h"
#include "../../../ride/Track.h"
#include "../../../ride/TrackPaint.h"
#include "../../../sprites.h"
#include "../../Paint.h"
#include "../../tile_element/Segment.h"
#include "../../track/Segment.h"
#include "../../track/Support.h"

using namespace OpenRCT2;

enum
{
    SPR_BOAT_HIRE_FLAT_BACK_SW_NE = 28523,
    SPR_BOAT_HIRE_FLAT_FRONT_SW_NE = 28524,
    SPR_BOAT_HIRE_FLAT_BACK_NW_SE = 28525,
    SPR_BOAT_HIRE_FLAT_FRONT_NW_SE = 28526,
    SPR_BOAT_HIRE_FLAT_QUARTER_TURN_1_TILE_BACK_SW_NW = 28527,
    SPR_BOAT_HIRE_FLAT_QUARTER_TURN_1_TILE_FRONT_SW_NW = 28528,
    SPR_BOAT_HIRE_FLAT_QUARTER_TURN_1_TILE_BACK_NW_NE = 28529,
    SPR_BOAT_HIRE_FLAT_QUARTER_TURN_1_TILE_FRONT_NW_NE = 28530,
    SPR_BOAT_HIRE_FLAT_QUARTER_TURN_1_TILE_BACK_NE_SE = 28531,
    SPR_BOAT_HIRE_FLAT_QUARTER_TURN_1_TILE_FRONT_NE_SE = 28532,
    SPR_BOAT_HIRE_FLAT_QUARTER_TURN_1_TILE_FRONT_SE_SW = 28533,
    SPR_BOAT_HIRE_FLAT_QUARTER_TURN_1_TILE_BACK_SE_SW = 28534,
};

/** rct2: 0x008B0E40 */
static void PaintBoatHireTrackFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    ImageId imageId;

    const auto offset = CoordsXYZ{ 0, 0, height };
    if (direction & 1)
    {
        imageId = session.TrackColours.WithIndex(SPR_BOAT_HIRE_FLAT_BACK_NW_SE);
        PaintAddImageAsParent(session, imageId, offset, { { 4, 0, height }, { 1, 32, 3 } });

        imageId = session.TrackColours.WithIndex(SPR_BOAT_HIRE_FLAT_FRONT_NW_SE);
        PaintAddImageAsParent(session, imageId, offset, { { 28, 0, height }, { 1, 32, 3 } });
    }
    else
    {
        imageId = session.TrackColours.WithIndex(SPR_BOAT_HIRE_FLAT_BACK_SW_NE);
        PaintAddImageAsParent(session, imageId, offset, { { 0, 4, height }, { 32, 1, 3 } });

        imageId = session.TrackColours.WithIndex(SPR_BOAT_HIRE_FLAT_FRONT_SW_NE);
        PaintAddImageAsParent(session, imageId, offset, { { 0, 28, height }, { 32, 1, 3 } });
    }

    PaintUtilSetSegmentSupportHeight(
        session,
        PaintUtilRotateSegments(
            EnumsToFlags(PaintSegment::bottomLeftSide, PaintSegment::centre, PaintSegment::topRightSide), direction),
        0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 16);
}

/** rct2: 0x008B0E50 */
static void PaintBoatHireStation(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    const auto* stationObj = ride.GetStationObject();

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
    ImageId imageId;
    const auto offset = CoordsXYZ{ 0, 0, height };
    switch (direction)
    {
        case 0:
            imageId = session.TrackColours.WithIndex(SPR_BOAT_HIRE_FLAT_QUARTER_TURN_1_TILE_BACK_SW_NW);
            PaintAddImageAsParent(session, imageId, offset, { { 0, 0, height }, { 32, 32, 1 } });

            imageId = session.TrackColours.WithIndex(SPR_BOAT_HIRE_FLAT_QUARTER_TURN_1_TILE_FRONT_SW_NW);
            PaintAddImageAsParent(session, imageId, offset, { { 28, 28, height + 2 }, { 3, 3, 3 } });
            break;
        case 1:
            imageId = session.TrackColours.WithIndex(SPR_BOAT_HIRE_FLAT_QUARTER_TURN_1_TILE_BACK_NW_NE);
            PaintAddImageAsParent(session, imageId, offset, { { 0, 0, height }, { 32, 32, 1 } });

            imageId = session.TrackColours.WithIndex(SPR_BOAT_HIRE_FLAT_QUARTER_TURN_1_TILE_FRONT_NW_NE);
            PaintAddImageAsParent(session, imageId, offset, { { 28, 28, height + 2 }, { 3, 3, 3 } });
            break;
        case 2:
            imageId = session.TrackColours.WithIndex(SPR_BOAT_HIRE_FLAT_QUARTER_TURN_1_TILE_BACK_NE_SE);
            PaintAddImageAsParent(session, imageId, offset, { { 0, 0, height }, { 32, 32, 1 } });

            imageId = session.TrackColours.WithIndex(SPR_BOAT_HIRE_FLAT_QUARTER_TURN_1_TILE_FRONT_NE_SE);
            PaintAddImageAsParent(session, imageId, offset, { { 28, 28, height + 2 }, { 3, 3, 3 } });
            break;
        case 3:
            imageId = session.TrackColours.WithIndex(SPR_BOAT_HIRE_FLAT_QUARTER_TURN_1_TILE_FRONT_SE_SW);
            PaintAddImageAsParent(session, imageId, offset, { { 28, 28, height + 2 }, { 3, 3, 3 } });

            imageId = session.TrackColours.WithIndex(SPR_BOAT_HIRE_FLAT_QUARTER_TURN_1_TILE_BACK_SE_SW);
            PaintAddImageAsParent(session, imageId, offset, { { 0, 0, height }, { 32, 32, 1 } });
            break;
    }

    PaintUtilSetSegmentSupportHeight(
        session,
        PaintUtilRotateSegments(
            EnumsToFlags(PaintSegment::bottomLeftSide, PaintSegment::centre, PaintSegment::topLeftSide), direction),
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
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_SMALL_CURVE + 0)),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_SMALL_CURVE + 1)),
                        { 0, 0, height }, { { 0, 6, height + 15 }, { 32, 20, 0 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_SMALL_CURVE + 6)),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_SMALL_CURVE + 7)),
                        { 0, 0, height }, { { 0, 6, height + 15 }, { 32, 20, 0 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_SMALL_CURVE + 12)),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_SMALL_CURVE + 13)),
                        { 0, 0, height }, { { 0, 6, height + 15 }, { 32, 20, 0 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_SMALL_CURVE + 18)),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 26, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_SMALL_CURVE + 19)),
                        { 0, 0, height }, { { 0, 6, height + 15 }, { 32, 26, 0 } });
                    break;
            }
            break;
        case 1:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_SMALL_CURVE + 8)),
                        { 0, 0, height }, { { 16, 16, height }, { 1, 1, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_SMALL_CURVE + 21)),
                        { 0, 0, height }, { { 0, 0, height + 15 }, { 16, 16, 0 } });
                    break;
            }
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_SMALL_CURVE + 2)),
                        { 0, 0, height }, { { 16, 0, height }, { 16, 16, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_SMALL_CURVE + 3)),
                        { 0, 0, height }, { { 16, 0, height + 15 }, { 16, 16, 0 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_SMALL_CURVE + 9)),
                        { 0, 0, height }, { { 0, 0, height + 15 }, { 16, 16, 0 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_SMALL_CURVE + 14)),
                        { 0, 0, height }, { { 0, 16, height }, { 16, 16, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_SMALL_CURVE + 15)),
                        { 0, 0, height }, { { 0, 16, height + 15 }, { 16, 16, 0 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_SMALL_CURVE + 20)),
                        { 0, 0, height }, { { 16, 16, height }, { 16, 16, 1 } });
                    break;
            }
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_SMALL_CURVE + 4)),
                        { 0, 0, height }, { { 6, 0, height }, { 20, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_SMALL_CURVE + 5)),
                        { 0, 0, height }, { { 6, 0, height + 15 }, { 20, 32, 0 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_SMALL_CURVE + 10)),
                        { 0, 0, height }, { { 6, 0, height }, { 20, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_SMALL_CURVE + 11)),
                        { 0, 0, height }, { { 6, 0, height + 15 }, { 20, 32, 0 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_SMALL_CURVE + 16)),
                        { 0, 0, height }, { { 6, 0, height }, { 20, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_SMALL_CURVE + 17)),
                        { 0, 0, height }, { { 6, 0, height + 15 }, { 20, 32, 0 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_SMALL_CURVE + 22)),
                        { 0, 0, height }, { { 6, 0, height }, { 26, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_SMALL_CURVE + 23)),
                        { 0, 0, height }, { { 6, 0, height + 15 }, { 26, 32, 0 } });
                    break;
            }
            break;
    }

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
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_MEDIUM_CURVE + 0)),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_MEDIUM_CURVE + 1)),
                        { 0, 0, height }, { { 0, 6, height + 15 }, { 32, 20, 0 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_MEDIUM_CURVE + 10)),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_MEDIUM_CURVE + 11)),
                        { 0, 0, height }, { { 0, 6, height + 15 }, { 32, 20, 0 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_MEDIUM_CURVE + 20)),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_MEDIUM_CURVE + 21)),
                        { 0, 0, height }, { { 0, 6, height + 15 }, { 32, 20, 0 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_MEDIUM_CURVE + 30)),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_MEDIUM_CURVE + 31)),
                        { 0, 0, height }, { { 0, 6, height + 15 }, { 32, 20, 0 } });
                    break;
            }
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_MEDIUM_CURVE + 2)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 16, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_MEDIUM_CURVE + 3)),
                        { 0, 0, height }, { { 0, 0, height + 15 }, { 32, 16, 0 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_MEDIUM_CURVE + 12)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 16, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_MEDIUM_CURVE + 13)),
                        { 0, 0, height }, { { 0, 0, height + 15 }, { 32, 16, 0 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_MEDIUM_CURVE + 22)),
                        { 0, 0, height }, { { 0, 16, height }, { 32, 16, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_MEDIUM_CURVE + 23)),
                        { 0, 0, height }, { { 0, 16, height + 15 }, { 32, 16, 0 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_MEDIUM_CURVE + 32)),
                        { 0, 0, height }, { { 0, 16, height }, { 32, 16, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_MEDIUM_CURVE + 33)),
                        { 0, 0, height }, { { 0, 16, height + 15 }, { 32, 16, 0 } });
                    break;
            }
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_MEDIUM_CURVE + 4)),
                        { 0, 0, height }, { { 0, 16, height }, { 16, 16, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_MEDIUM_CURVE + 5)),
                        { 0, 0, height }, { { 0, 16, height + 15 }, { 16, 16, 0 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_MEDIUM_CURVE + 14)),
                        { 0, 0, height }, { { 16, 16, height }, { 16, 16, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_MEDIUM_CURVE + 15)),
                        { 0, 0, height }, { { 16, 16, height + 15 }, { 16, 16, 0 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_MEDIUM_CURVE + 24)),
                        { 0, 0, height }, { { 16, 0, height }, { 16, 16, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_MEDIUM_CURVE + 25)),
                        { 0, 0, height }, { { 16, 0, height + 15 }, { 16, 16, 0 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_MEDIUM_CURVE + 34)),
                        { 0, 0, height }, { { 0, 0, height }, { 16, 16, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_MEDIUM_CURVE + 35)),
                        { 0, 0, height }, { { 0, 0, height + 15 }, { 32, 32, 0 } });
                    break;
            }
            break;
        case 5:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_MEDIUM_CURVE + 6)),
                        { 0, 0, height }, { { 16, 0, height }, { 16, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_MEDIUM_CURVE + 7)),
                        { 0, 0, height }, { { 16, 0, height + 15 }, { 16, 32, 0 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_MEDIUM_CURVE + 16)),
                        { 0, 0, height }, { { 0, 0, height }, { 16, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_MEDIUM_CURVE + 17)),
                        { 0, 0, height }, { { 0, 0, height + 15 }, { 16, 32, 0 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_MEDIUM_CURVE + 26)),
                        { 0, 0, height }, { { 0, 0, height }, { 16, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_MEDIUM_CURVE + 27)),
                        { 0, 0, height }, { { 0, 0, height + 15 }, { 16, 32, 0 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_MEDIUM_CURVE + 36)),
                        { 0, 0, height }, { { 16, 0, height }, { 16, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_MEDIUM_CURVE + 37)),
                        { 0, 0, height }, { { 16, 0, height + 15 }, { 16, 32, 0 } });
                    break;
            }
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_MEDIUM_CURVE + 8)),
                        { 0, 0, height }, { { 6, 0, height }, { 20, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_MEDIUM_CURVE + 9)),
                        { 0, 0, height }, { { 6, 0, height + 15 }, { 20, 32, 0 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_MEDIUM_CURVE + 18)),
                        { 0, 0, height }, { { 6, 0, height }, { 20, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_MEDIUM_CURVE + 19)),
                        { 0, 0, height }, { { 6, 0, height + 15 }, { 20, 32, 0 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_MEDIUM_CURVE + 28)),
                        { 0, 0, height }, { { 6, 0, height }, { 20, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_MEDIUM_CURVE + 29)),
                        { 0, 0, height }, { { 6, 0, height + 15 }, { 20, 32, 0 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_MEDIUM_CURVE + 38)),
                        { 0, 0, height }, { { 6, 0, height }, { 20, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_MEDIUM_CURVE + 39)),
                        { 0, 0, height }, { { 6, 0, height + 15 }, { 20, 32, 0 } });
                    break;
            }
            break;
    }

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
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_LARGE_CURVE + 0)),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_LARGE_CURVE + 1)),
                        { 0, 0, height }, { { 0, 6, height + 15 }, { 32, 20, 0 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_LARGE_CURVE + 8)),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_LARGE_CURVE + 9)),
                        { 0, 0, height }, { { 0, 6, height + 15 }, { 32, 20, 0 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_LARGE_CURVE + 18)),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_LARGE_CURVE + 19)),
                        { 0, 0, height }, { { 0, 6, height + 15 }, { 32, 20, 0 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_LARGE_CURVE + 26)),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_LARGE_CURVE + 27)),
                        { 0, 0, height }, { { 0, 6, height + 15 }, { 32, 20, 0 } });
                    break;
            }
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_LARGE_CURVE + 2)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 16, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_LARGE_CURVE + 3)),
                        { 0, 0, height }, { { 0, 0, height + 15 }, { 32, 16, 0 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_LARGE_CURVE + 10)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 16, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_LARGE_CURVE + 11)),
                        { 0, 0, height }, { { 0, 0, height + 15 }, { 32, 16, 0 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_LARGE_CURVE + 20)),
                        { 0, 0, height }, { { 0, 16, height }, { 32, 16, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_LARGE_CURVE + 21)),
                        { 0, 0, height }, { { 0, 16, height + 15 }, { 32, 16, 0 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_LARGE_CURVE + 28)),
                        { 0, 0, height }, { { 0, 16, height }, { 32, 16, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_LARGE_CURVE + 29)),
                        { 0, 0, height }, { { 0, 16, height + 15 }, { 32, 16, 0 } });
                    break;
            }
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_LARGE_CURVE + 4)),
                        { 0, 0, height }, { { 0, 16, height }, { 16, 16, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_LARGE_CURVE + 5)),
                        { 0, 0, height }, { { 0, 16, height + 15 }, { 16, 16, 0 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_LARGE_CURVE + 12)),
                        { 0, 0, height }, { { 16, 16, height }, { 16, 16, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_LARGE_CURVE + 13)),
                        { 0, 0, height }, { { 16, 16, height + 15 }, { 16, 16, 0 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_LARGE_CURVE + 22)),
                        { 0, 0, height }, { { 16, 0, height }, { 16, 16, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_LARGE_CURVE + 23)),
                        { 0, 0, height }, { { 16, 0, height + 15 }, { 16, 16, 0 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_LARGE_CURVE + 30)),
                        { 0, 0, height }, { { 0, 0, height }, { 16, 16, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_LARGE_CURVE + 31)),
                        { 0, 0, height }, { { 0, 0, height + 15 }, { 16, 16, 0 } });
                    break;
            }
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_LARGE_CURVE + 14)),
                        { 0, 0, height }, { { 0, 0, height }, { 16, 16, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_LARGE_CURVE + 15)),
                        { 0, 0, height }, { { 0, 0, height + 15 }, { 16, 16, 0 } });
                    break;
            }
            break;
        case 4:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_LARGE_CURVE + 6)),
                        { 0, 0, height }, { { 16, 16, height }, { 16, 16, 1 } });
                    PaintAddImageAsChildRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_LARGE_CURVE + 7)),
                        { 0, 0, height }, { { 16, 16, height }, { 16, 16, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_LARGE_CURVE + 16)),
                        { 0, 0, height }, { { 0, 16, height }, { 16, 18, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_LARGE_CURVE + 17)),
                        { 0, 0, height }, { { 0, 16, height + 15 }, { 16, 16, 0 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_LARGE_CURVE + 24)),
                        { 0, 0, height }, { { 0, 0, height }, { 16, 16, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_LARGE_CURVE + 25)),
                        { 0, 0, height }, { { 0, 0, height + 15 }, { 16, 16, 0 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_LARGE_CURVE + 32)),
                        { 0, 0, height }, { { 16, 0, height }, { 16, 16, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_LARGE_CURVE + 33)),
                        { 0, 0, height }, { { 16, 0, height + 15 }, { 16, 16, 0 } });
                    break;
            }
            break;
    }

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 16);
}

static void PaintBoatHireTrackRightEighthToDiag(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_LARGE_CURVE + 34)),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_LARGE_CURVE + 35)),
                        { 0, 0, height }, { { 0, 6, height + 15 }, { 32, 20, 0 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_LARGE_CURVE + 42)),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_LARGE_CURVE + 43)),
                        { 0, 0, height }, { { 0, 6, height + 15 }, { 32, 20, 0 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_LARGE_CURVE + 50)),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_LARGE_CURVE + 51)),
                        { 0, 0, height }, { { 0, 6, height + 15 }, { 32, 20, 0 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_LARGE_CURVE + 60)),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_LARGE_CURVE + 61)),
                        { 0, 0, height }, { { 0, 6, height + 15 }, { 32, 20, 0 } });
                    break;
            }
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_LARGE_CURVE + 36)),
                        { 0, 0, height }, { { 0, 16, height }, { 32, 16, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_LARGE_CURVE + 37)),
                        { 0, 0, height }, { { 0, 16, height + 15 }, { 32, 16, 0 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_LARGE_CURVE + 44)),
                        { 0, 0, height }, { { 0, 16, height }, { 32, 16, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_LARGE_CURVE + 45)),
                        { 0, 0, height }, { { 0, 16, height + 15 }, { 32, 16, 0 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_LARGE_CURVE + 52)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 16, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_LARGE_CURVE + 53)),
                        { 0, 0, height }, { { 0, 0, height + 15 }, { 32, 16, 0 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_LARGE_CURVE + 62)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 16, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_LARGE_CURVE + 63)),
                        { 0, 0, height }, { { 0, 0, height + 15 }, { 32, 16, 0 } });
                    break;
            }
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_LARGE_CURVE + 38)),
                        { 0, 0, height }, { { 0, 0, height }, { 16, 16, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_LARGE_CURVE + 39)),
                        { 0, 0, height }, { { 0, 0, height + 15 }, { 16, 16, 0 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_LARGE_CURVE + 46)),
                        { 0, 0, height }, { { 16, 0, height }, { 16, 16, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_LARGE_CURVE + 47)),
                        { 0, 0, height }, { { 16, 0, height + 15 }, { 16, 16, 0 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_LARGE_CURVE + 54)),
                        { 0, 0, height }, { { 16, 16, height }, { 16, 16, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_LARGE_CURVE + 55)),
                        { 0, 0, height }, { { 16, 16, height + 15 }, { 16, 16, 0 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_LARGE_CURVE + 64)),
                        { 0, 0, height }, { { 0, 16, height }, { 16, 16, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_LARGE_CURVE + 65)),
                        { 0, 0, height }, { { 0, 16, height + 15 }, { 16, 16, 0 } });
                    break;
            }
            break;
        case 3:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_LARGE_CURVE + 56)),
                        { 0, 0, height }, { { 0, 0, height }, { 16, 16, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_LARGE_CURVE + 57)),
                        { 0, 0, height }, { { 0, 0, height + 15 }, { 16, 16, 0 } });
                    break;
            }
            break;
        case 4:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_LARGE_CURVE + 40)),
                        { 0, 0, height }, { { 16, 0, height }, { 16, 16, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_LARGE_CURVE + 41)),
                        { 0, 0, height }, { { 16, 0, height + 15 }, { 16, 16, 0 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_LARGE_CURVE + 48)),
                        { 0, 0, height }, { { 0, 0, height }, { 16, 16, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_LARGE_CURVE + 49)),
                        { 0, 0, height }, { { 0, 0, height + 15 }, { 16, 16, 0 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_LARGE_CURVE + 58)),
                        { 0, 0, height }, { { 0, 16, height }, { 16, 16, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_LARGE_CURVE + 59)),
                        { 0, 0, height }, { { 0, 16, height + 15 }, { 16, 16, 0 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_LARGE_CURVE + 66)),
                        { 0, 0, height }, { { 16, 16, height }, { 16, 16, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_LARGE_CURVE + 67)),
                        { 0, 0, height }, { { 16, 16, height + 15 }, { 16, 16, 0 } });
                    break;
            }
            break;
    }

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
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_G2_BOAT_HIRE_TRACK_FLAT_DIAGONAL + 2),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 1 } });
                    break;
            }
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_G2_BOAT_HIRE_TRACK_FLAT_DIAGONAL + 0),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_G2_BOAT_HIRE_TRACK_FLAT_DIAGONAL + 1),
                        { -16, -16, height }, { { -16, -16, height + 15 }, { 32, 32, 0 } });
                    break;
            }
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_G2_BOAT_HIRE_TRACK_FLAT_DIAGONAL + 0),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_G2_BOAT_HIRE_TRACK_FLAT_DIAGONAL + 1),
                        { -16, -16, height }, { { -16, -16, height + 15 }, { 32, 32, 0 } });
                    break;
            }
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_G2_BOAT_HIRE_TRACK_FLAT_DIAGONAL + 2),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 1 } });
                    break;
            }
            break;
    }

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 16);
}

static void PaintBoatHireTrackSBendLeft(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_S_BEND + 0)),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_S_BEND + 1)),
                        { 0, 0, height }, { { 0, 6, height + 15 }, { 32, 20, 0 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_S_BEND + 8)),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_S_BEND + 9)),
                        { 0, 0, height }, { { 0, 6, height + 15 }, { 32, 20, 0 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_S_BEND + 6)),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_S_BEND + 7)),
                        { 0, 0, height }, { { 0, 6, height + 15 }, { 32, 20, 0 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_S_BEND + 14)),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_S_BEND + 15)),
                        { 0, 0, height }, { { 0, 6, height + 15 }, { 32, 20, 0 } });
                    break;
            }
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_S_BEND + 2)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 26, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_S_BEND + 3)),
                        { 0, 0, height }, { { 0, 0, height + 15 }, { 32, 26, 0 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_S_BEND + 10)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 26, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_S_BEND + 11)),
                        { 0, 0, height }, { { 0, 0, height + 15 }, { 32, 26, 0 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_S_BEND + 4)),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 26, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_S_BEND + 5)),
                        { 0, 0, height }, { { 0, 6, height + 15 }, { 32, 26, 0 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_S_BEND + 12)),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 26, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_S_BEND + 13)),
                        { 0, 0, height }, { { 0, 6, height + 15 }, { 32, 26, 0 } });
                    break;
            }
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_S_BEND + 4)),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 26, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_S_BEND + 5)),
                        { 0, 0, height }, { { 0, 6, height + 15 }, { 32, 26, 0 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_S_BEND + 12)),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 26, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_S_BEND + 13)),
                        { 0, 0, height }, { { 0, 6, height + 15 }, { 32, 26, 0 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_S_BEND + 2)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 26, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_S_BEND + 3)),
                        { 0, 0, height }, { { 0, 0, height + 15 }, { 32, 26, 0 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_S_BEND + 10)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 26, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_S_BEND + 11)),
                        { 0, 0, height }, { { 0, 0, height + 15 }, { 32, 26, 0 } });
                    break;
            }
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_S_BEND + 6)),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_S_BEND + 7)),
                        { 0, 0, height }, { { 0, 6, height + 15 }, { 32, 20, 0 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_S_BEND + 14)),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_S_BEND + 15)),
                        { 0, 0, height }, { { 0, 6, height + 15 }, { 32, 20, 0 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_S_BEND + 0)),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_S_BEND + 1)),
                        { 0, 0, height }, { { 0, 6, height + 15 }, { 32, 20, 0 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_S_BEND + 8)),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_S_BEND + 9)),
                        { 0, 0, height }, { { 0, 6, height + 15 }, { 32, 20, 0 } });
                    break;
            }
            break;
    }

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 16);
}

static void PaintBoatHireTrackSBendRight(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_S_BEND + 16)),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_S_BEND + 17)),
                        { 0, 0, height }, { { 0, 6, height + 15 }, { 32, 20, 0 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_S_BEND + 24)),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_S_BEND + 25)),
                        { 0, 0, height }, { { 0, 6, height + 15 }, { 32, 20, 0 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_S_BEND + 22)),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 01 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_S_BEND + 23)),
                        { 0, 0, height }, { { 0, 6, height + 15 }, { 32, 20, 0 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_S_BEND + 30)),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_S_BEND + 31)),
                        { 0, 0, height }, { { 0, 6, height + 15 }, { 32, 20, 0 } });
                    break;
            }
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_S_BEND + 18)),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 26, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_S_BEND + 19)),
                        { 0, 0, height }, { { 0, 6, height + 15 }, { 32, 26, 0 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_S_BEND + 26)),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 26, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_S_BEND + 27)),
                        { 0, 0, height }, { { 0, 6, height + 15 }, { 32, 26, 0 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_S_BEND + 20)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 26, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_S_BEND + 21)),
                        { 0, 0, height }, { { 0, 0, height + 15 }, { 32, 26, 0 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_S_BEND + 28)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 26, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_S_BEND + 29)),
                        { 0, 0, height }, { { 0, 0, height + 15 }, { 32, 26, 0 } });
                    break;
            }
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_S_BEND + 20)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 26, 11 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_S_BEND + 21)),
                        { 0, 0, height }, { { 0, 0, height + 15 }, { 32, 26, 0 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_S_BEND + 28)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 26, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_S_BEND + 29)),
                        { 0, 0, height }, { { 0, 0, height + 15 }, { 32, 26, 0 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_S_BEND + 18)),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 26, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_S_BEND + 19)),
                        { 0, 0, height }, { { 0, 6, height + 15 }, { 32, 26, 0 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_S_BEND + 26)),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 26, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_S_BEND + 27)),
                        { 0, 0, height }, { { 0, 6, height + 15 }, { 32, 26, 0 } });
                    break;
            }
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_S_BEND + 22)),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_S_BEND + 23)),
                        { 0, 0, height }, { { 0, 6, height + 15 }, { 32, 20, 0 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_S_BEND + 30)),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_S_BEND + 31)),
                        { 0, 0, height }, { { 0, 6, height + 15 }, { 32, 20, 0 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_S_BEND + 16)),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_S_BEND + 17)),
                        { 0, 0, height }, { { 0, 6, height + 15 }, { 32, 20, 0 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_S_BEND + 24)),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_BOAT_HIRE_TRACK_S_BEND + 25)),
                        { 0, 0, height }, { { 0, 6, height + 15 }, { 32, 20, 0 } });
                    break;
            }
            break;
    }

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
