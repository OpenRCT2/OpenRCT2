/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../interface/Viewport.h"
#include "../../paint/Paint.h"
#include "../../paint/Supports.h"
#include "../../world/Map.h"
#include "../RideData.h"
#include "../Track.h"
#include "../TrackPaint.h"
#include "../VehiclePaint.h"

enum
{
    SprGhostTrainTrackFlatSwNe = 28821,
    SprGhostTrainTrackFlatNwSe = 28822,
    SprGhostTrainTrackFlatTo25DegUpSwNe = 28823,
    SprGhostTrainTrackFlatTo25DegUpNwSe = 28824,
    SprGhostTrainTrackFlatTo25DegUpNeSw = 28825,
    SprGhostTrainTrackFlatTo25DegUpSeNw = 28826,
    SprGhostTrainTrack25DegUpToFlatSwNe = 28827,
    SprGhostTrainTrack25DegUpToFlatNwSe = 28828,
    SprGhostTrainTrack25DegUpToFlatNeSw = 28829,
    SprGhostTrainTrack25DegUpToFlatSeNw = 28830,
    SprGhostTrainTrack25DegUpSwNe = 28831,
    SprGhostTrainTrack25DegUpNwSe = 28832,
    SprGhostTrainTrack25DegUpNeSw = 28833,
    SprGhostTrainTrack25DegUpSeNw = 28834,
    SprGhostTrainTrackFlatTo25DegUpFrontSwNe = 28835,
    SprGhostTrainTrackFlatTo25DegUpFrontNwSe = 28836,
    SprGhostTrainTrackFlatTo25DegUpFrontNeSw = 28837,
    SprGhostTrainTrackFlatTo25DegUpFrontSeNw = 28838,
    SprGhostTrainTrack25DegUpToFlatFrontSwNe = 28839,
    SprGhostTrainTrack25DegUpToFlatFrontNwSe = 28840,
    SprGhostTrainTrack25DegUpToFlatFrontNeSw = 28841,
    SprGhostTrainTrack25DegUpToFlatFrontSeNw = 28842,
    SprGhostTrainTrack25DegUpFrontSwNe = 28843,
    SprGhostTrainTrack25DegUpFrontNwSe = 28844,
    SprGhostTrainTrack25DegUpFrontNeSw = 28845,
    SprGhostTrainTrack25DegUpFrontSeNw = 28846,
    SprGhostTrainQuarterTurn1TileSwNw = 28847,
    SprGhostTrainQuarterTurn1TileNwNe = 28848,
    SprGhostTrainQuarterTurn1TileNeSe = 28849,
    SprGhostTrainQuarterTurn1TileSeSw = 28850,
    SprGhostTrainQuarterTurn3TilesSwSePart0 = 28851,
    SprGhostTrainQuarterTurn3TilesSwSePart1 = 28852,
    SprGhostTrainQuarterTurn3TilesSwSePart2 = 28853,
    SprGhostTrainQuarterTurn3TilesNwSwPart0 = 28854,
    SprGhostTrainQuarterTurn3TilesNwSwPart1 = 28855,
    SprGhostTrainQuarterTurn3TilesNwSwPart2 = 28856,
    SprGhostTrainQuarterTurn3TilesNeNwPart0 = 28857,
    SprGhostTrainQuarterTurn3TilesNeNwPart1 = 28858,
    SprGhostTrainQuarterTurn3TilesNeNwPart2 = 28859,
    SprGhostTrainQuarterTurn3TilesSeNePart0 = 28860,
    SprGhostTrainQuarterTurn3TilesSeNePart1 = 28861,
    SprGhostTrainQuarterTurn3TilesSeNePart2 = 28862,
    SprGhostTrainSpinningTunnelTrackSwNe = 28863,
    SprGhostTrainSpinningTunnelTrackNwSe = 28864,

    SprGhostTrainTrackBrakesSwNe = 28881,
    SprGhostTrainTrackBrakesNwSe = 28882
};

static constexpr const uint32_t GhostTrainTrackPiecesFlat[4] = {
    SprGhostTrainTrackFlatSwNe,
    SprGhostTrainTrackFlatNwSe,
    SprGhostTrainTrackFlatSwNe,
    SprGhostTrainTrackFlatNwSe,
};

static constexpr const uint32_t GhostTrainTrackPiecesFlatTo25DegUp[4][2] = {
    { SprGhostTrainTrackFlatTo25DegUpSwNe, SprGhostTrainTrackFlatTo25DegUpFrontSwNe },
    { SprGhostTrainTrackFlatTo25DegUpNwSe, SprGhostTrainTrackFlatTo25DegUpFrontNwSe },
    { SprGhostTrainTrackFlatTo25DegUpNeSw, SprGhostTrainTrackFlatTo25DegUpFrontNeSw },
    { SprGhostTrainTrackFlatTo25DegUpSeNw, SprGhostTrainTrackFlatTo25DegUpFrontSeNw },
};

static constexpr const uint32_t GhostTrainTrackPieces25DegUpToFlat[4][2] = {
    { SprGhostTrainTrack25DegUpToFlatSwNe, SprGhostTrainTrack25DegUpToFlatFrontSwNe },
    { SprGhostTrainTrack25DegUpToFlatNwSe, SprGhostTrainTrack25DegUpToFlatFrontNwSe },
    { SprGhostTrainTrack25DegUpToFlatNeSw, SprGhostTrainTrack25DegUpToFlatFrontNeSw },
    { SprGhostTrainTrack25DegUpToFlatSeNw, SprGhostTrainTrack25DegUpToFlatFrontSeNw },
};

static constexpr const uint32_t GhostTrainTrackPieces25DegUp[4][2] = {
    { SprGhostTrainTrack25DegUpSwNe, SprGhostTrainTrack25DegUpFrontSwNe },
    { SprGhostTrainTrack25DegUpNwSe, SprGhostTrainTrack25DegUpFrontNwSe },
    { SprGhostTrainTrack25DegUpNeSw, SprGhostTrainTrack25DegUpFrontNeSw },
    { SprGhostTrainTrack25DegUpSeNw, SprGhostTrainTrack25DegUpFrontSeNw },
};

static constexpr const uint32_t GhostTrainTrackPiecesQuarterTurn1Tile[4] = {
    SprGhostTrainQuarterTurn1TileSwNw,
    SprGhostTrainQuarterTurn1TileNwNe,
    SprGhostTrainQuarterTurn1TileNeSe,
    SprGhostTrainQuarterTurn1TileSeSw,
};

static constexpr const uint32_t GhostTrainTrackPiecesQuarterTurn3Tiles[4][3] = {
    {
        SprGhostTrainQuarterTurn3TilesSwSePart0,
        SprGhostTrainQuarterTurn3TilesSwSePart1,
        SprGhostTrainQuarterTurn3TilesSwSePart2,
    },
    {
        SprGhostTrainQuarterTurn3TilesNwSwPart0,
        SprGhostTrainQuarterTurn3TilesNwSwPart1,
        SprGhostTrainQuarterTurn3TilesNwSwPart2,
    },
    {
        SprGhostTrainQuarterTurn3TilesNeNwPart0,
        SprGhostTrainQuarterTurn3TilesNeNwPart1,
        SprGhostTrainQuarterTurn3TilesNeNwPart2,
    },
    {
        SprGhostTrainQuarterTurn3TilesSeNePart0,
        SprGhostTrainQuarterTurn3TilesSeNePart1,
        SprGhostTrainQuarterTurn3TilesSeNePart2,
    },
};

static constexpr const uint32_t ghost_train_track_pieces_spinning_tunnel_track[4] = {
    SprGhostTrainSpinningTunnelTrackSwNe,
    SprGhostTrainSpinningTunnelTrackNwSe,
    SprGhostTrainSpinningTunnelTrackSwNe,
    SprGhostTrainSpinningTunnelTrackNwSe,
};

static constexpr const uint32_t GhostTrainTrackPiecesBrakes[4] = {
    SprGhostTrainTrackBrakesSwNe,
    SprGhostTrainTrackBrakesNwSe,
    SprGhostTrainTrackBrakesSwNe,
    SprGhostTrainTrackBrakesNwSe,
};

static constexpr const uint8_t DoorOpeningOutwardsToImage[] = {
    TUNNEL_DOORS_2, // Closed
    TUNNEL_DOORS_2, // Unused?
    TUNNEL_DOORS_3, // Half open
    TUNNEL_DOORS_4, // Fully open
    TUNNEL_DOORS_2, // Unused?
    TUNNEL_DOORS_2, // Unused?
    TUNNEL_DOORS_2, // Unused?
};

static constexpr const uint8_t DoorOpeningInwardsToImage[] = {
    TUNNEL_DOORS_2, // Closed
    TUNNEL_DOORS_2, // Unused?
    TUNNEL_DOORS_5, // Half open
    TUNNEL_DOORS_6, // Fully open
    TUNNEL_DOORS_2, // Unused?
    TUNNEL_DOORS_2, // Unused?
    TUNNEL_DOORS_2, // Unused?
};

static uint8_t GetTunnelDoorsImageStraightFlat(const TrackElement& trackElement, uint8_t direction)
{
    switch (direction)
    {
        case 0:
            return DoorOpeningInwardsToImage[trackElement.GetDoorAState()];
        case 1:
            return DoorOpeningOutwardsToImage[trackElement.GetDoorBState()];
        case 2:
            return DoorOpeningOutwardsToImage[trackElement.GetDoorBState()];
        case 3:
            return DoorOpeningInwardsToImage[trackElement.GetDoorAState()];
    }
    return TUNNEL_DOORS_2;
}

/** rct2: 0x00770BEC */
static void PaintGhostTrainTrackFlat(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex(GhostTrainTrackPiecesFlat[direction]);

    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });

    auto tunnelImage = GetTunnelDoorsImageStraightFlat(trackElement, direction);
    PaintUtilPushTunnelRotated(session, direction, height, tunnelImage);

    if (track_paint_util_should_paint_supports(session.MapPosition))
    {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
    }

    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

/** rct2: 0x00770BFC */
static void PaintGhostTrainTrack25DegUp(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex(GhostTrainTrackPieces25DegUp[direction][0]);
    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });

    imageId = session.TrackColours[SCHEME_TRACK].WithIndex(GhostTrainTrackPieces25DegUp[direction][1]);
    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { 32, 1, 23 }, { 0, 27, height });

    if (track_paint_util_should_paint_supports(session.MapPosition))
    {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
    }

    switch (direction)
    {
        case 0:
            PaintUtilPushTunnelLeft(session, height - 8, TUNNEL_1);
            break;
        case 1:
            PaintUtilPushTunnelRight(session, height + 8, TUNNEL_2);
            break;
        case 2:
            PaintUtilPushTunnelLeft(session, height + 8, TUNNEL_2);
            break;
        case 3:
            PaintUtilPushTunnelRight(session, height - 8, TUNNEL_1);
            break;
    }

    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
}

/** rct2: 0x00770C0C */
static void PaintGhostTrainTrackFlatTo25DegUp(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    bool isBackwards = trackElement.GetTrackType() == TrackElemType::Down25ToFlat;
    uint8_t doorImage;
    if (!isBackwards)
    {
        doorImage = DoorOpeningInwardsToImage[trackElement.GetDoorAState()];
    }
    else
    {
        doorImage = DoorOpeningOutwardsToImage[trackElement.GetDoorBState()];
    }

    auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex(GhostTrainTrackPiecesFlatTo25DegUp[direction][0]);
    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });

    imageId = session.TrackColours[SCHEME_TRACK].WithIndex(GhostTrainTrackPiecesFlatTo25DegUp[direction][1]);
    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { 32, 1, 15 }, { 0, 27, height });

    if (track_paint_util_should_paint_supports(session.MapPosition))
    {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 4, 3, height, session.TrackColours[SCHEME_SUPPORTS]);
    }

    switch (direction)
    {
        case 0:
            PaintUtilPushTunnelLeft(session, height, doorImage);
            break;
        case 1:
            PaintUtilPushTunnelRight(session, height, TUNNEL_2);
            break;
        case 2:
            PaintUtilPushTunnelLeft(session, height, TUNNEL_2);
            break;
        case 3:
            PaintUtilPushTunnelRight(session, height, doorImage);
            break;
    }

    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
}

static void PaintGhostTrainTrack25DegUpToFlatShared(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex(GhostTrainTrackPieces25DegUpToFlat[direction][0]);
    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });

    imageId = session.TrackColours[SCHEME_TRACK].WithIndex(GhostTrainTrackPieces25DegUpToFlat[direction][1]);
    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { 32, 1, 15 }, { 0, 27, height });

    if (track_paint_util_should_paint_supports(session.MapPosition))
    {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
    }

    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 40, 0x20);
}

/** rct2: 0x00770C1C */
static void PaintGhostTrainTrack25DegUpToFlat(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    PaintGhostTrainTrack25DegUpToFlatShared(session, ride, trackSequence, direction, height, trackElement);

    switch (direction)
    {
        case 0:
            PaintUtilPushTunnelLeft(session, height - 8, TUNNEL_0);
            break;
        case 1:
            PaintUtilPushTunnelRight(session, height + 8, DoorOpeningOutwardsToImage[trackElement.GetDoorBState()]);
            break;
        case 2:
            PaintUtilPushTunnelLeft(session, height + 8, DoorOpeningOutwardsToImage[trackElement.GetDoorBState()]);
            break;
        case 3:
            PaintUtilPushTunnelRight(session, height - 8, TUNNEL_0);
            break;
    }
}

/** rct2: 0x00770C2C */
static void PaintGhostTrainTrack25DegDown(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    PaintGhostTrainTrack25DegUp(session, ride, trackSequence, (direction + 2) % 4, height, trackElement);
}

/** rct2: 0x00770C3C */
static void PaintGhostTrainTrackFlatTo25DegDown(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    PaintGhostTrainTrack25DegUpToFlatShared(session, ride, trackSequence, (direction + 2) % 4, height, trackElement);

    switch ((direction + 2) % 4)
    {
        case 0:
            PaintUtilPushTunnelLeft(session, height - 8, TUNNEL_0);
            break;
        case 1:
            PaintUtilPushTunnelRight(session, height + 8, DoorOpeningInwardsToImage[trackElement.GetDoorAState()]);
            break;
        case 2:
            PaintUtilPushTunnelLeft(session, height + 8, DoorOpeningInwardsToImage[trackElement.GetDoorAState()]);
            break;
        case 3:
            PaintUtilPushTunnelRight(session, height - 8, TUNNEL_0);
            break;
    }
}

/** rct2: 0x00770C4C */
static void PaintGhostTrainTrack25DegDownToFlat(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    PaintGhostTrainTrackFlatTo25DegUp(session, ride, trackSequence, (direction + 2) % 4, height, trackElement);
}

/** rct2: 0x00770C5C, 0x00770C6C, 0x00770C7C */
static void PaintGhostTrainStation(
    paint_session& session, const Ride& ride, [[maybe_unused]] uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    ImageId imageId;

    static constexpr const std::array imageIds = {
        SPR_STATION_BASE_B_SW_NE,
        SPR_STATION_BASE_B_NW_SE,
        SPR_STATION_BASE_B_SW_NE,
        SPR_STATION_BASE_B_NW_SE,
    };

    imageId = session.TrackColours[SCHEME_MISC].WithIndex(imageIds[direction]);
    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height - 2 }, { 32, 28, 3 }, { 0, 2, height });

    imageId = session.TrackColours[SCHEME_TRACK].WithIndex(GhostTrainTrackPiecesFlat[direction]);
    PaintAddImageAsChildRotated(session, direction, imageId, { 0, 0, height }, { 32, 20, 3 }, { 0, 0, height });

    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);

    if (direction == 0 || direction == 2)
    {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 5, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
    }
    else
    {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 6, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
    }

    track_paint_util_draw_station(session, ride, direction, height, trackElement);

    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

/** rct2: 0x00770C9C */
static void PaintGhostTrainTrackRightQuarterTurn3Tiles(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    track_paint_util_right_quarter_turn_3_tiles_paint(
        session, 3, height, direction, trackSequence, session.TrackColours[SCHEME_TRACK],
        GhostTrainTrackPiecesQuarterTurn3Tiles, nullptr, defaultRightQuarterTurn3TilesBoundLengths,
        defaultRightQuarterTurn3TilesBoundOffsets);
    bool isBackwards = trackElement.GetTrackType() == TrackElemType::LeftQuarterTurn3Tiles;
    bool isDoorA = (!isBackwards && trackSequence == 0) || (isBackwards && trackSequence == 3);
    auto tunnelType = isDoorA ? DoorOpeningInwardsToImage[trackElement.GetDoorAState()]
                              : DoorOpeningOutwardsToImage[trackElement.GetDoorBState()];
    track_paint_util_right_quarter_turn_3_tiles_tunnel(session, height, direction, trackSequence, tunnelType);

    switch (trackSequence)
    {
        case 0:
        case 3:
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            break;
    }

    int32_t blockedSegments = 0;
    switch (trackSequence)
    {
        case 0:
            blockedSegments = SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_BC;
            break;
        case 2:
            blockedSegments = SEGMENT_D0 | SEGMENT_C4 | SEGMENT_D4 | SEGMENT_C0;
            break;
        case 3:
            blockedSegments = SEGMENT_D4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_B8;
            break;
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments, direction), 0xFFFF, 0);

    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

/** rct2: 0x00770CAC */
static void PaintGhostTrainTrackLeftQuarterTurn3Tiles(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    PaintGhostTrainTrackRightQuarterTurn3Tiles(session, ride, trackSequence, (direction + 1) % 4, height, trackElement);
}

/** rct2: 0x00770CAC */
static void PaintGhostTrainTrackLeftQuarterTurn1Tile(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    bool isBackwards = trackElement.GetTrackType() == TrackElemType::RightQuarterTurn1Tile;
    uint8_t tunnelStartImage, tunnelEndImage;
    if (!isBackwards)
    {
        tunnelStartImage = DoorOpeningInwardsToImage[trackElement.GetDoorAState()];
        tunnelEndImage = DoorOpeningOutwardsToImage[trackElement.GetDoorBState()];
    }
    else
    {
        tunnelStartImage = DoorOpeningOutwardsToImage[trackElement.GetDoorBState()];
        tunnelEndImage = DoorOpeningInwardsToImage[trackElement.GetDoorAState()];
    }

    track_paint_util_left_quarter_turn_1_tile_paint(
        session, 3, height, 0, direction, session.TrackColours[SCHEME_TRACK], GhostTrainTrackPiecesQuarterTurn1Tile);
    track_paint_util_left_quarter_turn_1_tile_tunnel(session, direction, height, 0, tunnelStartImage, 0, tunnelEndImage);

    metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

/** rct2: 0x00770CBC */
static void PaintGhostTrainTrackRightQuarterTurn1Tile(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    PaintGhostTrainTrackLeftQuarterTurn1Tile(session, ride, trackSequence, (direction + 3) % 4, height, trackElement);
}

/** rct2: 0x00770CCC */
static void PaintGhostTrainTrackSpinningTunnel(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex(ghost_train_track_pieces_spinning_tunnel_track[direction]);

    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { 28, 20, 3 }, { 2, 6, height });

    track_paint_util_spinning_tunnel_paint(session, 3, height, direction);

    auto tunnelImage = GetTunnelDoorsImageStraightFlat(trackElement, direction);
    PaintUtilPushTunnelRotated(session, direction, height, tunnelImage);

    WoodenASupportsPaintSetup(session, (direction & 1), 0, height, session.TrackColours[SCHEME_MISC]);

    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

/** rct2: 0x00770CDC */
static void PaintGhostTrainTrackBrakes(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex(GhostTrainTrackPiecesBrakes[direction]);

    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });

    auto tunnelImage = GetTunnelDoorsImageStraightFlat(trackElement, direction);
    PaintUtilPushTunnelRotated(session, direction, height, tunnelImage);

    if (track_paint_util_should_paint_supports(session.MapPosition))
    {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
    }

    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

/**
 * rct2: 0x00770924
 */
TRACK_PAINT_FUNCTION GetTrackPaintFunctionGhostTrain(int32_t trackType)
{
    switch (trackType)
    {
        case TrackElemType::Flat:
            return PaintGhostTrainTrackFlat;

        case TrackElemType::EndStation:
        case TrackElemType::BeginStation:
        case TrackElemType::MiddleStation:
            return PaintGhostTrainStation;

        case TrackElemType::Up25:
            return PaintGhostTrainTrack25DegUp;
        case TrackElemType::FlatToUp25:
            return PaintGhostTrainTrackFlatTo25DegUp;
        case TrackElemType::Up25ToFlat:
            return PaintGhostTrainTrack25DegUpToFlat;

        case TrackElemType::Down25:
            return PaintGhostTrainTrack25DegDown;
        case TrackElemType::FlatToDown25:
            return PaintGhostTrainTrackFlatTo25DegDown;
        case TrackElemType::Down25ToFlat:
            return PaintGhostTrainTrack25DegDownToFlat;

        case TrackElemType::LeftQuarterTurn3Tiles:
            return PaintGhostTrainTrackLeftQuarterTurn3Tiles;
        case TrackElemType::RightQuarterTurn3Tiles:
            return PaintGhostTrainTrackRightQuarterTurn3Tiles;

        case TrackElemType::LeftQuarterTurn1Tile:
            return PaintGhostTrainTrackLeftQuarterTurn1Tile;
        case TrackElemType::RightQuarterTurn1Tile:
            return PaintGhostTrainTrackRightQuarterTurn1Tile;

        case TrackElemType::Brakes:
            return PaintGhostTrainTrackBrakes;

        case TrackElemType::SpinningTunnel:
            return PaintGhostTrainTrackSpinningTunnel;
    }

    return nullptr;
}
