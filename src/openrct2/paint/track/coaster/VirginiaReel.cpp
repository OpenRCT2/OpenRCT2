/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../../interface/Viewport.h"
#include "../../../ride/Track.h"
#include "../../../ride/TrackPaint.h"
#include "../../../ride/Vehicle.h"
#include "../../../world/tile_element/TrackElement.h"
#include "../../Paint.h"
#include "../../support/WoodenSupports.h"
#include "../../support/WoodenSupports.hpp"
#include "../../tile_element/Segment.h"
#include "../../track/Segment.h"
#include "../../track/Support.h"

#include <cassert>

using namespace OpenRCT2;

static constexpr TunnelGroup kTunnelGroup = TunnelGroup::Square;

enum
{
    SPR_VIRGINIA_REEL_FLAT_SW_NE = 21458,
    SPR_VIRGINIA_REEL_FLAT_NW_SE = 21459,
    SPR_VIRGINIA_REEL_FLAT_TO_25_DEG_UP_SW_NE = 21460,
    SPR_VIRGINIA_REEL_FLAT_TO_25_DEG_UP_NW_SE = 21461,
    SPR_VIRGINIA_REEL_FLAT_TO_25_DEG_UP_NE_SW = 21462,
    SPR_VIRGINIA_REEL_FLAT_TO_25_DEG_UP_SE_NW = 21463,
    SPR_VIRGINIA_REEL_25_DEG_UP_TO_FLAT_SW_NE = 21464,
    SPR_VIRGINIA_REEL_25_DEG_UP_TO_FLAT_NW_SE = 21465,
    SPR_VIRGINIA_REEL_25_DEG_UP_TO_FLAT_NE_SW = 21466,
    SPR_VIRGINIA_REEL_25_DEG_UP_TO_FLAT_SE_NW = 21467,
    SPR_VIRGINIA_REEL_25_DEG_UP_SW_NE = 21468,
    SPR_VIRGINIA_REEL_25_DEG_UP_NW_SE = 21469,
    SPR_VIRGINIA_REEL_25_DEG_UP_NE_SW = 21470,
    SPR_VIRGINIA_REEL_25_DEG_UP_SE_NW = 21471,
    SPR_VIRGINIA_REEL_FLAT_QUARTER_TURN_3_TILES_SW_SE_PART_0 = 21472,
    SPR_VIRGINIA_REEL_FLAT_QUARTER_TURN_3_TILES_SW_SE_PART_1 = 21473,
    SPR_VIRGINIA_REEL_FLAT_QUARTER_TURN_3_TILES_SW_SE_PART_2 = 21474,
    SPR_VIRGINIA_REEL_FLAT_QUARTER_TURN_3_TILES_NW_SW_PART_0 = 21475,
    SPR_VIRGINIA_REEL_FLAT_QUARTER_TURN_3_TILES_NW_SW_PART_1 = 21476,
    SPR_VIRGINIA_REEL_FLAT_QUARTER_TURN_3_TILES_NW_SW_PART_2 = 21477,
    SPR_VIRGINIA_REEL_FLAT_QUARTER_TURN_3_TILES_NE_NW_PART_0 = 21478,
    SPR_VIRGINIA_REEL_FLAT_QUARTER_TURN_3_TILES_NE_NW_PART_1 = 21479,
    SPR_VIRGINIA_REEL_FLAT_QUARTER_TURN_3_TILES_NE_NW_PART_2 = 21480,
    SPR_VIRGINIA_REEL_FLAT_QUARTER_TURN_3_TILES_SE_NE_PART_0 = 21481,
    SPR_VIRGINIA_REEL_FLAT_QUARTER_TURN_3_TILES_SE_NE_PART_1 = 21482,
    SPR_VIRGINIA_REEL_FLAT_QUARTER_TURN_3_TILES_SE_NE_PART_2 = 21483,
    SPR_VIRGINIA_REEL_FLAT_QUARTER_TURN_1_TILE_SW_NW = 21484,
    SPR_VIRGINIA_REEL_FLAT_QUARTER_TURN_1_TILE_NW_NE = 21485,
    SPR_VIRGINIA_REEL_FLAT_QUARTER_TURN_1_TILE_NE_SE = 21486,
    SPR_VIRGINIA_REEL_FLAT_QUARTER_TURN_1_TILE_SE_SW = 21487,
    SPR_VIRGINIA_REEL_FLAT_LIFT_HILL_SW_NE = 21488,
    SPR_VIRGINIA_REEL_FLAT_LIFT_HILL_NW_SE = 21489,
    SPR_VIRGINIA_REEL_FLAT_LIFT_HILL_NE_SW = 21490,
    SPR_VIRGINIA_REEL_FLAT_LIFT_HILL_SE_NW = 21491,
    SPR_VIRGINIA_REEL_FLAT_TO_25_DEG_UP_LIFT_HILL_SW_NE = 21492,
    SPR_VIRGINIA_REEL_FLAT_TO_25_DEG_UP_LIFT_HILL_NW_SE = 21493,
    SPR_VIRGINIA_REEL_FLAT_TO_25_DEG_UP_LIFT_HILL_NE_SW = 21494,
    SPR_VIRGINIA_REEL_FLAT_TO_25_DEG_UP_LIFT_HILL_SE_NW = 21495,
    SPR_VIRGINIA_REEL_25_DEG_UP_TO_FLAT_LIFT_HILL_SW_NE = 21496,
    SPR_VIRGINIA_REEL_25_DEG_UP_TO_FLAT_LIFT_HILL_NW_SE = 21497,
    SPR_VIRGINIA_REEL_25_DEG_UP_TO_FLAT_LIFT_HILL_NE_SW = 21498,
    SPR_VIRGINIA_REEL_25_DEG_UP_TO_FLAT_LIFT_HILL_SE_NW = 21499,
    SPR_VIRGINIA_REEL_25_DEG_UP_LIFT_HILL_SW_NE = 21500,
    SPR_VIRGINIA_REEL_25_DEG_UP_LIFT_HILL_NW_SE = 21501,
    SPR_VIRGINIA_REEL_25_DEG_UP_LIFT_HILL_NE_SW = 21502,
    SPR_VIRGINIA_REEL_25_DEG_UP_LIFT_HILL_SE_NW = 21503,
};

static constexpr uint32_t kPiecesFlat[4] = {
    SPR_VIRGINIA_REEL_FLAT_SW_NE,
    SPR_VIRGINIA_REEL_FLAT_NW_SE,
    SPR_VIRGINIA_REEL_FLAT_SW_NE,
    SPR_VIRGINIA_REEL_FLAT_NW_SE,
};

static constexpr uint32_t kPiecesFlatLiftHill[4] = {
    SPR_VIRGINIA_REEL_FLAT_LIFT_HILL_SW_NE,
    SPR_VIRGINIA_REEL_FLAT_LIFT_HILL_NW_SE,
    SPR_VIRGINIA_REEL_FLAT_LIFT_HILL_NE_SW,
    SPR_VIRGINIA_REEL_FLAT_LIFT_HILL_SE_NW,
};

static constexpr uint32_t kPiecesFlatTo25DegUp[4] = {
    SPR_VIRGINIA_REEL_FLAT_TO_25_DEG_UP_SW_NE,
    SPR_VIRGINIA_REEL_FLAT_TO_25_DEG_UP_NW_SE,
    SPR_VIRGINIA_REEL_FLAT_TO_25_DEG_UP_NE_SW,
    SPR_VIRGINIA_REEL_FLAT_TO_25_DEG_UP_SE_NW,
};

static constexpr uint32_t kPiecesFlatTo25DegUpLiftHill[4] = {
    SPR_VIRGINIA_REEL_FLAT_TO_25_DEG_UP_LIFT_HILL_SW_NE,
    SPR_VIRGINIA_REEL_FLAT_TO_25_DEG_UP_LIFT_HILL_NW_SE,
    SPR_VIRGINIA_REEL_FLAT_TO_25_DEG_UP_LIFT_HILL_NE_SW,
    SPR_VIRGINIA_REEL_FLAT_TO_25_DEG_UP_LIFT_HILL_SE_NW,
};

static constexpr uint32_t kPieces25DegUpToFlat[4] = {
    SPR_VIRGINIA_REEL_25_DEG_UP_TO_FLAT_SW_NE,
    SPR_VIRGINIA_REEL_25_DEG_UP_TO_FLAT_NW_SE,
    SPR_VIRGINIA_REEL_25_DEG_UP_TO_FLAT_NE_SW,
    SPR_VIRGINIA_REEL_25_DEG_UP_TO_FLAT_SE_NW,
};

static constexpr uint32_t kPieces25DegUpToFlatLiftHill[4] = {
    SPR_VIRGINIA_REEL_25_DEG_UP_TO_FLAT_LIFT_HILL_SW_NE,
    SPR_VIRGINIA_REEL_25_DEG_UP_TO_FLAT_LIFT_HILL_NW_SE,
    SPR_VIRGINIA_REEL_25_DEG_UP_TO_FLAT_LIFT_HILL_NE_SW,
    SPR_VIRGINIA_REEL_25_DEG_UP_TO_FLAT_LIFT_HILL_SE_NW,
};

static constexpr uint32_t kPieces25DegUp[4] = {
    SPR_VIRGINIA_REEL_25_DEG_UP_SW_NE,
    SPR_VIRGINIA_REEL_25_DEG_UP_NW_SE,
    SPR_VIRGINIA_REEL_25_DEG_UP_NE_SW,
    SPR_VIRGINIA_REEL_25_DEG_UP_SE_NW,
};

static constexpr uint32_t kPieces25DegUpLiftHill[4] = {
    SPR_VIRGINIA_REEL_25_DEG_UP_LIFT_HILL_SW_NE,
    SPR_VIRGINIA_REEL_25_DEG_UP_LIFT_HILL_NW_SE,
    SPR_VIRGINIA_REEL_25_DEG_UP_LIFT_HILL_NE_SW,
    SPR_VIRGINIA_REEL_25_DEG_UP_LIFT_HILL_SE_NW,
};

static constexpr uint32_t kPiecesFlatQuarterTurn3Tiles[4][3] = {
    {
        SPR_VIRGINIA_REEL_FLAT_QUARTER_TURN_3_TILES_SW_SE_PART_0,
        SPR_VIRGINIA_REEL_FLAT_QUARTER_TURN_3_TILES_SW_SE_PART_1,
        SPR_VIRGINIA_REEL_FLAT_QUARTER_TURN_3_TILES_SW_SE_PART_2,
    },
    {
        SPR_VIRGINIA_REEL_FLAT_QUARTER_TURN_3_TILES_NW_SW_PART_0,
        SPR_VIRGINIA_REEL_FLAT_QUARTER_TURN_3_TILES_NW_SW_PART_1,
        SPR_VIRGINIA_REEL_FLAT_QUARTER_TURN_3_TILES_NW_SW_PART_2,
    },
    {
        SPR_VIRGINIA_REEL_FLAT_QUARTER_TURN_3_TILES_NE_NW_PART_0,
        SPR_VIRGINIA_REEL_FLAT_QUARTER_TURN_3_TILES_NE_NW_PART_1,
        SPR_VIRGINIA_REEL_FLAT_QUARTER_TURN_3_TILES_NE_NW_PART_2,
    },
    {
        SPR_VIRGINIA_REEL_FLAT_QUARTER_TURN_3_TILES_SE_NE_PART_0,
        SPR_VIRGINIA_REEL_FLAT_QUARTER_TURN_3_TILES_SE_NE_PART_1,
        SPR_VIRGINIA_REEL_FLAT_QUARTER_TURN_3_TILES_SE_NE_PART_2,
    },
};

static constexpr uint32_t kPiecesFlatQuarterTurn1Tile[4] = {
    SPR_VIRGINIA_REEL_FLAT_QUARTER_TURN_1_TILE_SW_NW,
    SPR_VIRGINIA_REEL_FLAT_QUARTER_TURN_1_TILE_NW_NE,
    SPR_VIRGINIA_REEL_FLAT_QUARTER_TURN_1_TILE_NE_SE,
    SPR_VIRGINIA_REEL_FLAT_QUARTER_TURN_1_TILE_SE_SW,
};

/** rct2: 0x00811264 */
static void PaintVirginiaReelTrackFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    const uint32_t* sprites = kPiecesFlat;
    if (trackElement.HasChain())
    {
        sprites = kPiecesFlatLiftHill;
    }

    auto imageId = session.TrackColours.WithIndex(sprites[direction]);
    if (direction & 1)
    {
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 2, 0, height }, { 27, 32, 2 } });
        PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
    }
    else
    {
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 2, height }, { 32, 27, 2 } });
        PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
    }

    DrawSupportForSequenceA<TrackElemType::Flat>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x00811274 */
static void PaintVirginiaReelTrack25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    const uint32_t* sprites = kPieces25DegUp;
    if (trackElement.HasChain())
    {
        sprites = kPieces25DegUpLiftHill;
    }

    auto imageId = session.TrackColours.WithIndex(sprites[direction]);
    PaintStruct* ps;

    if (direction & 1)
    {
        ps = PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 2, 0, height }, { 27, 32, 2 } });
    }
    else
    {
        ps = PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 2, height }, { 32, 27, 2 } });
    }

    if (direction == 1 || direction == 2)
    {
        session.WoodenSupportsPrependTo = ps;
    }

    DrawSupportForSequenceA<TrackElemType::Up25>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
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

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56);
}

/** rct2: 0x00811294 */
static void PaintVirginiaReelTrackFlatTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    const uint32_t* sprites = kPiecesFlatTo25DegUp;
    if (trackElement.HasChain())
    {
        sprites = kPiecesFlatTo25DegUpLiftHill;
    }

    auto imageId = session.TrackColours.WithIndex(sprites[direction]);
    PaintStruct* ps;
    switch (direction)
    {
        case 0:
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 2, height }, { 32, 27, 2 } });

            break;
        case 1:
            ps = PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 2, 0, height }, { 27, 32, 2 } });
            session.WoodenSupportsPrependTo = ps;
            break;
        case 2:
            ps = PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 2, height }, { 32, 27, 2 } });
            session.WoodenSupportsPrependTo = ps;
            break;
        case 3:
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 2, 0, height }, { 27, 32, 2 } });
            break;
    }

    DrawSupportForSequenceA<TrackElemType::FlatToUp25>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48);
}

/** rct2: 0x00811294 */
static void PaintVirginiaReelTrack25DegUpToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    const uint32_t* sprites = kPieces25DegUpToFlat;
    if (trackElement.HasChain())
    {
        sprites = kPieces25DegUpToFlatLiftHill;
    }

    auto imageId = session.TrackColours.WithIndex(sprites[direction]);
    PaintStruct* ps;

    if (direction & 1)
    {
        ps = PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 2, 0, height }, { 27, 32, 2 } });
    }
    else
    {
        ps = PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 2, height }, { 32, 27, 2 } });
    }

    if (direction == 1 || direction == 2)
    {
        session.WoodenSupportsPrependTo = ps;
    }

    DrawSupportForSequenceA<TrackElemType::Up25ToFlat>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
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

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 40);
}

/** rct2: 0x008112A4 */
static void PaintVirginiaReelTrack25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    PaintVirginiaReelTrack25DegUp(session, ride, trackSequence, (direction + 2) % 4, height, trackElement, supportType);
}

/** rct2: 0x008112B4 */
static void PaintVirginiaReelTrackFlatTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    PaintVirginiaReelTrack25DegUpToFlat(session, ride, trackSequence, (direction + 2) % 4, height, trackElement, supportType);
}

/** rct2: 0x008112C4 */
static void PaintVirginiaReelTrack25DegDownToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    PaintVirginiaReelTrackFlatTo25DegUp(session, ride, trackSequence, (direction + 2) % 4, height, trackElement, supportType);
}

/** rct2: 0x008112D4, 0x008112E4, 0x008112F4 */
static void PaintVirginiaReelStation(
    PaintSession& session, const Ride& ride, [[maybe_unused]] uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    ImageId imageId;

    if (direction == 0 || direction == 2)
    {
        imageId = GetStationColourScheme(session, trackElement).WithIndex(SPR_STATION_BASE_B_SW_NE);
        PaintAddImageAsParent(session, imageId, { 0, 0, height - 2 }, { { 0, 2, height }, { 32, 28, 2 } });

        imageId = session.TrackColours.WithIndex(SPR_VIRGINIA_REEL_FLAT_SW_NE);
        PaintAddImageAsChild(session, imageId, { 0, 0, height }, { { 0, 0, height }, { 32, 20, 2 } });

        PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
    }
    else if (direction == 1 || direction == 3)
    {
        imageId = GetStationColourScheme(session, trackElement).WithIndex(SPR_STATION_BASE_B_NW_SE);
        PaintAddImageAsParent(session, imageId, { 0, 0, height - 2 }, { { 2, 0, height }, { 28, 32, 2 } });

        imageId = session.TrackColours.WithIndex(SPR_VIRGINIA_REEL_FLAT_NW_SE);
        PaintAddImageAsChild(session, imageId, { 0, 0, height }, { { 0, 0, height }, { 20, 32, 2 } });

        PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
    }

    DrawSupportForSequenceA<TrackElemType::EndStation>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    TrackPaintUtilDrawStation(session, ride, direction, height, trackElement);

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x00811304 */
static void PaintVirginiaReelTrackLeftQuarterTurn3Tiles(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    TrackPaintUtilLeftQuarterTurn3TilesPaint(
        session, 2, height, direction, trackSequence, session.TrackColours, kPiecesFlatQuarterTurn3Tiles);
    TrackPaintUtilLeftQuarterTurn3TilesTunnel(session, kTunnelGroup, TunnelSubType::Flat, height, direction, trackSequence);

    switch (trackSequence)
    {
        case 2:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::topLeftSide, PaintSegment::centre, PaintSegment::bottomLeftSide,
                        PaintSegment::leftCorner),
                    direction),
                0xFFFF, 0);
            break;
        case 0:
        case 3:
            DrawSupportForSequenceA<TrackElemType::LeftQuarterTurn3Tiles>(
                session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            break;
    }

    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

static constexpr uint8_t virginia_reel_right_quarter_turn_3_tiles_to_left_turn_map[] = {
    3,
    1,
    2,
    0,
};

/** rct2: 0x00811314 */
static void PaintVirginiaReelTrackRightQuarterTurn3Tiles(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = virginia_reel_right_quarter_turn_3_tiles_to_left_turn_map[trackSequence];
    PaintVirginiaReelTrackLeftQuarterTurn3Tiles(
        session, ride, trackSequence, (direction + 3) % 4, height, trackElement, supportType);
}

/** rct2: 0x00811324 */
static void PaintVirginiaReelTrackLeftQuarterTurn1Tile(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    TrackPaintUtilLeftQuarterTurn1TilePaint(
        session, 2, height, 0, direction, session.TrackColours, kPiecesFlatQuarterTurn1Tile);

    DrawSupportForSequenceA<TrackElemType::LeftQuarterTurn1Tile>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    switch (direction)
    {
        case 0:
            PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
            break;
        case 2:
            PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
            break;
        case 3:
            PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
            PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
            break;
    }

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x00811334 */
static void PaintVirginiaReelTrackRightQuarterTurn1Tile(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    PaintVirginiaReelTrackLeftQuarterTurn1Tile(
        session, ride, trackSequence, (direction + 3) % 4, height, trackElement, supportType);
}

/**
 * rct2: 0x00811184
 */
TrackPaintFunction GetTrackPaintFunctionVirginiaReel(OpenRCT2::TrackElemType trackType)
{
    switch (trackType)
    {
        case TrackElemType::Flat:
            return PaintVirginiaReelTrackFlat;

        case TrackElemType::EndStation:
        case TrackElemType::BeginStation:
        case TrackElemType::MiddleStation:
            return PaintVirginiaReelStation;

        case TrackElemType::Up25:
            return PaintVirginiaReelTrack25DegUp;
        case TrackElemType::FlatToUp25:
            return PaintVirginiaReelTrackFlatTo25DegUp;
        case TrackElemType::Up25ToFlat:
            return PaintVirginiaReelTrack25DegUpToFlat;

        case TrackElemType::Down25:
            return PaintVirginiaReelTrack25DegDown;
        case TrackElemType::FlatToDown25:
            return PaintVirginiaReelTrackFlatTo25DegDown;
        case TrackElemType::Down25ToFlat:
            return PaintVirginiaReelTrack25DegDownToFlat;

        case TrackElemType::LeftQuarterTurn3Tiles:
            return PaintVirginiaReelTrackLeftQuarterTurn3Tiles;
        case TrackElemType::RightQuarterTurn3Tiles:
            return PaintVirginiaReelTrackRightQuarterTurn3Tiles;

        case TrackElemType::LeftQuarterTurn1Tile:
            return PaintVirginiaReelTrackLeftQuarterTurn1Tile;
        case TrackElemType::RightQuarterTurn1Tile:
            return PaintVirginiaReelTrackRightQuarterTurn1Tile;

        default:
            return TrackPaintFunctionDummy;
    }
}
