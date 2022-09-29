/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
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
    SPR_GHOST_TRAIN_TRACK_FLAT_SW_NE = 28821,
    SPR_GHOST_TRAIN_TRACK_FLAT_NW_SE = 28822,
    SPR_GHOST_TRAIN_TRACK_FLAT_TO_25_DEG_UP_SW_NE = 28823,
    SPR_GHOST_TRAIN_TRACK_FLAT_TO_25_DEG_UP_NW_SE = 28824,
    SPR_GHOST_TRAIN_TRACK_FLAT_TO_25_DEG_UP_NE_SW = 28825,
    SPR_GHOST_TRAIN_TRACK_FLAT_TO_25_DEG_UP_SE_NW = 28826,
    SPR_GHOST_TRAIN_TRACK_25_DEG_UP_TO_FLAT_SW_NE = 28827,
    SPR_GHOST_TRAIN_TRACK_25_DEG_UP_TO_FLAT_NW_SE = 28828,
    SPR_GHOST_TRAIN_TRACK_25_DEG_UP_TO_FLAT_NE_SW = 28829,
    SPR_GHOST_TRAIN_TRACK_25_DEG_UP_TO_FLAT_SE_NW = 28830,
    SPR_GHOST_TRAIN_TRACK_25_DEG_UP_SW_NE = 28831,
    SPR_GHOST_TRAIN_TRACK_25_DEG_UP_NW_SE = 28832,
    SPR_GHOST_TRAIN_TRACK_25_DEG_UP_NE_SW = 28833,
    SPR_GHOST_TRAIN_TRACK_25_DEG_UP_SE_NW = 28834,
    SPR_GHOST_TRAIN_TRACK_FLAT_TO_25_DEG_UP_FRONT_SW_NE = 28835,
    SPR_GHOST_TRAIN_TRACK_FLAT_TO_25_DEG_UP_FRONT_NW_SE = 28836,
    SPR_GHOST_TRAIN_TRACK_FLAT_TO_25_DEG_UP_FRONT_NE_SW = 28837,
    SPR_GHOST_TRAIN_TRACK_FLAT_TO_25_DEG_UP_FRONT_SE_NW = 28838,
    SPR_GHOST_TRAIN_TRACK_25_DEG_UP_TO_FLAT_FRONT_SW_NE = 28839,
    SPR_GHOST_TRAIN_TRACK_25_DEG_UP_TO_FLAT_FRONT_NW_SE = 28840,
    SPR_GHOST_TRAIN_TRACK_25_DEG_UP_TO_FLAT_FRONT_NE_SW = 28841,
    SPR_GHOST_TRAIN_TRACK_25_DEG_UP_TO_FLAT_FRONT_SE_NW = 28842,
    SPR_GHOST_TRAIN_TRACK_25_DEG_UP_FRONT_SW_NE = 28843,
    SPR_GHOST_TRAIN_TRACK_25_DEG_UP_FRONT_NW_SE = 28844,
    SPR_GHOST_TRAIN_TRACK_25_DEG_UP_FRONT_NE_SW = 28845,
    SPR_GHOST_TRAIN_TRACK_25_DEG_UP_FRONT_SE_NW = 28846,
    SPR_GHOST_TRAIN_QUARTER_TURN_1_TILE_SW_NW = 28847,
    SPR_GHOST_TRAIN_QUARTER_TURN_1_TILE_NW_NE = 28848,
    SPR_GHOST_TRAIN_QUARTER_TURN_1_TILE_NE_SE = 28849,
    SPR_GHOST_TRAIN_QUARTER_TURN_1_TILE_SE_SW = 28850,
    SPR_GHOST_TRAIN_QUARTER_TURN_3_TILES_SW_SE_PART_0 = 28851,
    SPR_GHOST_TRAIN_QUARTER_TURN_3_TILES_SW_SE_PART_1 = 28852,
    SPR_GHOST_TRAIN_QUARTER_TURN_3_TILES_SW_SE_PART_2 = 28853,
    SPR_GHOST_TRAIN_QUARTER_TURN_3_TILES_NW_SW_PART_0 = 28854,
    SPR_GHOST_TRAIN_QUARTER_TURN_3_TILES_NW_SW_PART_1 = 28855,
    SPR_GHOST_TRAIN_QUARTER_TURN_3_TILES_NW_SW_PART_2 = 28856,
    SPR_GHOST_TRAIN_QUARTER_TURN_3_TILES_NE_NW_PART_0 = 28857,
    SPR_GHOST_TRAIN_QUARTER_TURN_3_TILES_NE_NW_PART_1 = 28858,
    SPR_GHOST_TRAIN_QUARTER_TURN_3_TILES_NE_NW_PART_2 = 28859,
    SPR_GHOST_TRAIN_QUARTER_TURN_3_TILES_SE_NE_PART_0 = 28860,
    SPR_GHOST_TRAIN_QUARTER_TURN_3_TILES_SE_NE_PART_1 = 28861,
    SPR_GHOST_TRAIN_QUARTER_TURN_3_TILES_SE_NE_PART_2 = 28862,
    SPR_GHOST_TRAIN_SPINNING_TUNNEL_TRACK_SW_NE = 28863,
    SPR_GHOST_TRAIN_SPINNING_TUNNEL_TRACK_NW_SE = 28864,

    SPR_GHOST_TRAIN_TRACK_BRAKES_SW_NE = 28881,
    SPR_GHOST_TRAIN_TRACK_BRAKES_NW_SE = 28882
};

static constexpr const uint32_t ghost_train_track_pieces_flat[4] = {
    SPR_GHOST_TRAIN_TRACK_FLAT_SW_NE,
    SPR_GHOST_TRAIN_TRACK_FLAT_NW_SE,
    SPR_GHOST_TRAIN_TRACK_FLAT_SW_NE,
    SPR_GHOST_TRAIN_TRACK_FLAT_NW_SE,
};

static constexpr const uint32_t ghost_train_track_pieces_flat_to_25_deg_up[4][2] = {
    { SPR_GHOST_TRAIN_TRACK_FLAT_TO_25_DEG_UP_SW_NE, SPR_GHOST_TRAIN_TRACK_FLAT_TO_25_DEG_UP_FRONT_SW_NE },
    { SPR_GHOST_TRAIN_TRACK_FLAT_TO_25_DEG_UP_NW_SE, SPR_GHOST_TRAIN_TRACK_FLAT_TO_25_DEG_UP_FRONT_NW_SE },
    { SPR_GHOST_TRAIN_TRACK_FLAT_TO_25_DEG_UP_NE_SW, SPR_GHOST_TRAIN_TRACK_FLAT_TO_25_DEG_UP_FRONT_NE_SW },
    { SPR_GHOST_TRAIN_TRACK_FLAT_TO_25_DEG_UP_SE_NW, SPR_GHOST_TRAIN_TRACK_FLAT_TO_25_DEG_UP_FRONT_SE_NW },
};

static constexpr const uint32_t ghost_train_track_pieces_25_deg_up_to_flat[4][2] = {
    { SPR_GHOST_TRAIN_TRACK_25_DEG_UP_TO_FLAT_SW_NE, SPR_GHOST_TRAIN_TRACK_25_DEG_UP_TO_FLAT_FRONT_SW_NE },
    { SPR_GHOST_TRAIN_TRACK_25_DEG_UP_TO_FLAT_NW_SE, SPR_GHOST_TRAIN_TRACK_25_DEG_UP_TO_FLAT_FRONT_NW_SE },
    { SPR_GHOST_TRAIN_TRACK_25_DEG_UP_TO_FLAT_NE_SW, SPR_GHOST_TRAIN_TRACK_25_DEG_UP_TO_FLAT_FRONT_NE_SW },
    { SPR_GHOST_TRAIN_TRACK_25_DEG_UP_TO_FLAT_SE_NW, SPR_GHOST_TRAIN_TRACK_25_DEG_UP_TO_FLAT_FRONT_SE_NW },
};

static constexpr const uint32_t ghost_train_track_pieces_25_deg_up[4][2] = {
    { SPR_GHOST_TRAIN_TRACK_25_DEG_UP_SW_NE, SPR_GHOST_TRAIN_TRACK_25_DEG_UP_FRONT_SW_NE },
    { SPR_GHOST_TRAIN_TRACK_25_DEG_UP_NW_SE, SPR_GHOST_TRAIN_TRACK_25_DEG_UP_FRONT_NW_SE },
    { SPR_GHOST_TRAIN_TRACK_25_DEG_UP_NE_SW, SPR_GHOST_TRAIN_TRACK_25_DEG_UP_FRONT_NE_SW },
    { SPR_GHOST_TRAIN_TRACK_25_DEG_UP_SE_NW, SPR_GHOST_TRAIN_TRACK_25_DEG_UP_FRONT_SE_NW },
};

static constexpr const uint32_t ghost_train_track_pieces_quarter_turn_1_tile[4] = {
    SPR_GHOST_TRAIN_QUARTER_TURN_1_TILE_SW_NW,
    SPR_GHOST_TRAIN_QUARTER_TURN_1_TILE_NW_NE,
    SPR_GHOST_TRAIN_QUARTER_TURN_1_TILE_NE_SE,
    SPR_GHOST_TRAIN_QUARTER_TURN_1_TILE_SE_SW,
};

static constexpr const uint32_t ghost_train_track_pieces_quarter_turn_3_tiles[4][3] = {
    {
        SPR_GHOST_TRAIN_QUARTER_TURN_3_TILES_SW_SE_PART_0,
        SPR_GHOST_TRAIN_QUARTER_TURN_3_TILES_SW_SE_PART_1,
        SPR_GHOST_TRAIN_QUARTER_TURN_3_TILES_SW_SE_PART_2,
    },
    {
        SPR_GHOST_TRAIN_QUARTER_TURN_3_TILES_NW_SW_PART_0,
        SPR_GHOST_TRAIN_QUARTER_TURN_3_TILES_NW_SW_PART_1,
        SPR_GHOST_TRAIN_QUARTER_TURN_3_TILES_NW_SW_PART_2,
    },
    {
        SPR_GHOST_TRAIN_QUARTER_TURN_3_TILES_NE_NW_PART_0,
        SPR_GHOST_TRAIN_QUARTER_TURN_3_TILES_NE_NW_PART_1,
        SPR_GHOST_TRAIN_QUARTER_TURN_3_TILES_NE_NW_PART_2,
    },
    {
        SPR_GHOST_TRAIN_QUARTER_TURN_3_TILES_SE_NE_PART_0,
        SPR_GHOST_TRAIN_QUARTER_TURN_3_TILES_SE_NE_PART_1,
        SPR_GHOST_TRAIN_QUARTER_TURN_3_TILES_SE_NE_PART_2,
    },
};

static constexpr const uint32_t ghost_train_track_pieces_spinning_tunnel_track[4] = {
    SPR_GHOST_TRAIN_SPINNING_TUNNEL_TRACK_SW_NE,
    SPR_GHOST_TRAIN_SPINNING_TUNNEL_TRACK_NW_SE,
    SPR_GHOST_TRAIN_SPINNING_TUNNEL_TRACK_SW_NE,
    SPR_GHOST_TRAIN_SPINNING_TUNNEL_TRACK_NW_SE,
};

static constexpr const uint32_t ghost_train_track_pieces_brakes[4] = {
    SPR_GHOST_TRAIN_TRACK_BRAKES_SW_NE,
    SPR_GHOST_TRAIN_TRACK_BRAKES_NW_SE,
    SPR_GHOST_TRAIN_TRACK_BRAKES_SW_NE,
    SPR_GHOST_TRAIN_TRACK_BRAKES_NW_SE,
};

static constexpr const uint8_t doorOpeningOutwardsToImage[] = {
    TUNNEL_DOORS_2, // Closed
    TUNNEL_DOORS_2, // Unused?
    TUNNEL_DOORS_3, // Half open
    TUNNEL_DOORS_4, // Fully open
    TUNNEL_DOORS_2, // Unused?
    TUNNEL_DOORS_2, // Unused?
    TUNNEL_DOORS_2, // Unused?
};

static constexpr const uint8_t doorOpeningInwardsToImage[] = {
    TUNNEL_DOORS_2, // Closed
    TUNNEL_DOORS_2, // Unused?
    TUNNEL_DOORS_5, // Half open
    TUNNEL_DOORS_6, // Fully open
    TUNNEL_DOORS_2, // Unused?
    TUNNEL_DOORS_2, // Unused?
    TUNNEL_DOORS_2, // Unused?
};

static uint8_t get_tunnel_doors_image_straight_flat(const TrackElement& trackElement, uint8_t direction)
{
    switch (direction)
    {
        case 0:
            return doorOpeningInwardsToImage[trackElement.GetDoorAState()];
        case 1:
            return doorOpeningOutwardsToImage[trackElement.GetDoorBState()];
        case 2:
            return doorOpeningOutwardsToImage[trackElement.GetDoorBState()];
        case 3:
            return doorOpeningInwardsToImage[trackElement.GetDoorAState()];
    }
    return TUNNEL_DOORS_2;
}

/** rct2: 0x00770BEC */
static void paint_ghost_train_track_flat(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex(ghost_train_track_pieces_flat[direction]);

    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });

    auto tunnelImage = get_tunnel_doors_image_straight_flat(trackElement, direction);
    paint_util_push_tunnel_rotated(session, direction, height, tunnelImage);

    if (track_paint_util_should_paint_supports(session.MapPosition))
    {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
    }

    paint_util_set_segment_support_height(
        session, paint_util_rotate_segments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x00770BFC */
static void paint_ghost_train_track_25_deg_up(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex(ghost_train_track_pieces_25_deg_up[direction][0]);
    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });

    imageId = session.TrackColours[SCHEME_TRACK].WithIndex(ghost_train_track_pieces_25_deg_up[direction][1]);
    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { 32, 1, 23 }, { 0, 27, height });

    if (track_paint_util_should_paint_supports(session.MapPosition))
    {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
    }

    switch (direction)
    {
        case 0:
            paint_util_push_tunnel_left(session, height - 8, TUNNEL_1);
            break;
        case 1:
            paint_util_push_tunnel_right(session, height + 8, TUNNEL_2);
            break;
        case 2:
            paint_util_push_tunnel_left(session, height + 8, TUNNEL_2);
            break;
        case 3:
            paint_util_push_tunnel_right(session, height - 8, TUNNEL_1);
            break;
    }

    paint_util_set_segment_support_height(
        session, paint_util_rotate_segments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 56, 0x20);
}

/** rct2: 0x00770C0C */
static void paint_ghost_train_track_flat_to_25_deg_up(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    bool isBackwards = trackElement.GetTrackType() == TrackElemType::Down25ToFlat;
    uint8_t doorImage;
    if (!isBackwards)
    {
        doorImage = doorOpeningInwardsToImage[trackElement.GetDoorAState()];
    }
    else
    {
        doorImage = doorOpeningOutwardsToImage[trackElement.GetDoorBState()];
    }

    auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex(ghost_train_track_pieces_flat_to_25_deg_up[direction][0]);
    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });

    imageId = session.TrackColours[SCHEME_TRACK].WithIndex(ghost_train_track_pieces_flat_to_25_deg_up[direction][1]);
    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { 32, 1, 15 }, { 0, 27, height });

    if (track_paint_util_should_paint_supports(session.MapPosition))
    {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 4, 3, height, session.TrackColours[SCHEME_SUPPORTS]);
    }

    switch (direction)
    {
        case 0:
            paint_util_push_tunnel_left(session, height, doorImage);
            break;
        case 1:
            paint_util_push_tunnel_right(session, height, TUNNEL_2);
            break;
        case 2:
            paint_util_push_tunnel_left(session, height, TUNNEL_2);
            break;
        case 3:
            paint_util_push_tunnel_right(session, height, doorImage);
            break;
    }

    paint_util_set_segment_support_height(
        session, paint_util_rotate_segments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 48, 0x20);
}

static void paint_ghost_train_track_25_deg_up_to_flat_shared(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex(ghost_train_track_pieces_25_deg_up_to_flat[direction][0]);
    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });

    imageId = session.TrackColours[SCHEME_TRACK].WithIndex(ghost_train_track_pieces_25_deg_up_to_flat[direction][1]);
    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { 32, 1, 15 }, { 0, 27, height });

    if (track_paint_util_should_paint_supports(session.MapPosition))
    {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
    }

    paint_util_set_segment_support_height(
        session, paint_util_rotate_segments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 40, 0x20);
}

/** rct2: 0x00770C1C */
static void paint_ghost_train_track_25_deg_up_to_flat(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    paint_ghost_train_track_25_deg_up_to_flat_shared(session, ride, trackSequence, direction, height, trackElement);

    switch (direction)
    {
        case 0:
            paint_util_push_tunnel_left(session, height - 8, TUNNEL_0);
            break;
        case 1:
            paint_util_push_tunnel_right(session, height + 8, doorOpeningOutwardsToImage[trackElement.GetDoorBState()]);
            break;
        case 2:
            paint_util_push_tunnel_left(session, height + 8, doorOpeningOutwardsToImage[trackElement.GetDoorBState()]);
            break;
        case 3:
            paint_util_push_tunnel_right(session, height - 8, TUNNEL_0);
            break;
    }
}

/** rct2: 0x00770C2C */
static void paint_ghost_train_track_25_deg_down(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    paint_ghost_train_track_25_deg_up(session, ride, trackSequence, (direction + 2) % 4, height, trackElement);
}

/** rct2: 0x00770C3C */
static void paint_ghost_train_track_flat_to_25_deg_down(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    paint_ghost_train_track_25_deg_up_to_flat_shared(session, ride, trackSequence, (direction + 2) % 4, height, trackElement);

    switch ((direction + 2) % 4)
    {
        case 0:
            paint_util_push_tunnel_left(session, height - 8, TUNNEL_0);
            break;
        case 1:
            paint_util_push_tunnel_right(session, height + 8, doorOpeningInwardsToImage[trackElement.GetDoorAState()]);
            break;
        case 2:
            paint_util_push_tunnel_left(session, height + 8, doorOpeningInwardsToImage[trackElement.GetDoorAState()]);
            break;
        case 3:
            paint_util_push_tunnel_right(session, height - 8, TUNNEL_0);
            break;
    }
}

/** rct2: 0x00770C4C */
static void paint_ghost_train_track_25_deg_down_to_flat(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    paint_ghost_train_track_flat_to_25_deg_up(session, ride, trackSequence, (direction + 2) % 4, height, trackElement);
}

/** rct2: 0x00770C5C, 0x00770C6C, 0x00770C7C */
static void paint_ghost_train_station(
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

    imageId = session.TrackColours[SCHEME_TRACK].WithIndex(ghost_train_track_pieces_flat[direction]);
    PaintAddImageAsChildRotated(session, direction, imageId, { 0, 0, height }, { 32, 20, 3 }, { 0, 0, height });

    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);

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

    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x00770C9C */
static void paint_ghost_train_track_right_quarter_turn_3_tiles(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    track_paint_util_right_quarter_turn_3_tiles_paint(
        session, 3, height, direction, trackSequence, session.TrackColours[SCHEME_TRACK],
        ghost_train_track_pieces_quarter_turn_3_tiles, nullptr, defaultRightQuarterTurn3TilesBoundLengths,
        defaultRightQuarterTurn3TilesBoundOffsets);
    bool isBackwards = trackElement.GetTrackType() == TrackElemType::LeftQuarterTurn3Tiles;
    bool isDoorA = (!isBackwards && trackSequence == 0) || (isBackwards && trackSequence == 3);
    auto tunnelType = isDoorA ? doorOpeningInwardsToImage[trackElement.GetDoorAState()]
                              : doorOpeningOutwardsToImage[trackElement.GetDoorBState()];
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
    paint_util_set_segment_support_height(session, paint_util_rotate_segments(blockedSegments, direction), 0xFFFF, 0);

    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x00770CAC */
static void paint_ghost_train_track_left_quarter_turn_3_tiles(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    paint_ghost_train_track_right_quarter_turn_3_tiles(session, ride, trackSequence, (direction + 1) % 4, height, trackElement);
}

/** rct2: 0x00770CAC */
static void paint_ghost_train_track_left_quarter_turn_1_tile(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    bool isBackwards = trackElement.GetTrackType() == TrackElemType::RightQuarterTurn1Tile;
    uint8_t tunnelStartImage, tunnelEndImage;
    if (!isBackwards)
    {
        tunnelStartImage = doorOpeningInwardsToImage[trackElement.GetDoorAState()];
        tunnelEndImage = doorOpeningOutwardsToImage[trackElement.GetDoorBState()];
    }
    else
    {
        tunnelStartImage = doorOpeningOutwardsToImage[trackElement.GetDoorBState()];
        tunnelEndImage = doorOpeningInwardsToImage[trackElement.GetDoorAState()];
    }

    track_paint_util_left_quarter_turn_1_tile_paint(
        session, 3, height, 0, direction, session.TrackColours[SCHEME_TRACK], ghost_train_track_pieces_quarter_turn_1_tile);
    track_paint_util_left_quarter_turn_1_tile_tunnel(session, direction, height, 0, tunnelStartImage, 0, tunnelEndImage);

    metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x00770CBC */
static void paint_ghost_train_track_right_quarter_turn_1_tile(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    paint_ghost_train_track_left_quarter_turn_1_tile(session, ride, trackSequence, (direction + 3) % 4, height, trackElement);
}

/** rct2: 0x00770CCC */
static void paint_ghost_train_track_spinning_tunnel(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex(ghost_train_track_pieces_spinning_tunnel_track[direction]);

    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { 28, 20, 3 }, { 2, 6, height });

    track_paint_util_spinning_tunnel_paint(session, 3, height, direction);

    auto tunnelImage = get_tunnel_doors_image_straight_flat(trackElement, direction);
    paint_util_push_tunnel_rotated(session, direction, height, tunnelImage);

    wooden_a_supports_paint_setup(session, (direction & 1), 0, height, session.TrackColours[SCHEME_MISC]);

    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x00770CDC */
static void paint_ghost_train_track_brakes(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex(ghost_train_track_pieces_brakes[direction]);

    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });

    auto tunnelImage = get_tunnel_doors_image_straight_flat(trackElement, direction);
    paint_util_push_tunnel_rotated(session, direction, height, tunnelImage);

    if (track_paint_util_should_paint_supports(session.MapPosition))
    {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
    }

    paint_util_set_segment_support_height(
        session, paint_util_rotate_segments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/**
 * rct2: 0x00770924
 */
TRACK_PAINT_FUNCTION get_track_paint_function_ghost_train(int32_t trackType)
{
    switch (trackType)
    {
        case TrackElemType::Flat:
            return paint_ghost_train_track_flat;

        case TrackElemType::EndStation:
        case TrackElemType::BeginStation:
        case TrackElemType::MiddleStation:
            return paint_ghost_train_station;

        case TrackElemType::Up25:
            return paint_ghost_train_track_25_deg_up;
        case TrackElemType::FlatToUp25:
            return paint_ghost_train_track_flat_to_25_deg_up;
        case TrackElemType::Up25ToFlat:
            return paint_ghost_train_track_25_deg_up_to_flat;

        case TrackElemType::Down25:
            return paint_ghost_train_track_25_deg_down;
        case TrackElemType::FlatToDown25:
            return paint_ghost_train_track_flat_to_25_deg_down;
        case TrackElemType::Down25ToFlat:
            return paint_ghost_train_track_25_deg_down_to_flat;

        case TrackElemType::LeftQuarterTurn3Tiles:
            return paint_ghost_train_track_left_quarter_turn_3_tiles;
        case TrackElemType::RightQuarterTurn3Tiles:
            return paint_ghost_train_track_right_quarter_turn_3_tiles;

        case TrackElemType::LeftQuarterTurn1Tile:
            return paint_ghost_train_track_left_quarter_turn_1_tile;
        case TrackElemType::RightQuarterTurn1Tile:
            return paint_ghost_train_track_right_quarter_turn_1_tile;

        case TrackElemType::Brakes:
            return paint_ghost_train_track_brakes;

        case TrackElemType::SpinningTunnel:
            return paint_ghost_train_track_spinning_tunnel;
    }

    return nullptr;
}
