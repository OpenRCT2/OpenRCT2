#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#include "../track_paint.h"
#include "../track.h"
#include "../vehicle_paint.h"
#include "../../interface/viewport.h"
#include "../../paint/paint.h"
#include "../../paint/supports.h"
#include "../ride_data.h"
#include "../../world/map.h"

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

static const uint32 ghost_train_track_pieces_flat[4] = {
    SPR_GHOST_TRAIN_TRACK_FLAT_SW_NE,
    SPR_GHOST_TRAIN_TRACK_FLAT_NW_SE,
    SPR_GHOST_TRAIN_TRACK_FLAT_SW_NE,
    SPR_GHOST_TRAIN_TRACK_FLAT_NW_SE,
};

static const uint32 ghost_train_track_pieces_flat_to_25_deg_up[4][2] = {
    {SPR_GHOST_TRAIN_TRACK_FLAT_TO_25_DEG_UP_SW_NE, SPR_GHOST_TRAIN_TRACK_FLAT_TO_25_DEG_UP_FRONT_SW_NE},
    {SPR_GHOST_TRAIN_TRACK_FLAT_TO_25_DEG_UP_NW_SE, SPR_GHOST_TRAIN_TRACK_FLAT_TO_25_DEG_UP_FRONT_NW_SE},
    {SPR_GHOST_TRAIN_TRACK_FLAT_TO_25_DEG_UP_NE_SW, SPR_GHOST_TRAIN_TRACK_FLAT_TO_25_DEG_UP_FRONT_NE_SW},
    {SPR_GHOST_TRAIN_TRACK_FLAT_TO_25_DEG_UP_SE_NW, SPR_GHOST_TRAIN_TRACK_FLAT_TO_25_DEG_UP_FRONT_SE_NW},
};

static const uint32 ghost_train_track_pieces_25_deg_up_to_flat[4][2] = {
    {SPR_GHOST_TRAIN_TRACK_25_DEG_UP_TO_FLAT_SW_NE, SPR_GHOST_TRAIN_TRACK_25_DEG_UP_TO_FLAT_FRONT_SW_NE},
    {SPR_GHOST_TRAIN_TRACK_25_DEG_UP_TO_FLAT_NW_SE, SPR_GHOST_TRAIN_TRACK_25_DEG_UP_TO_FLAT_FRONT_NW_SE},
    {SPR_GHOST_TRAIN_TRACK_25_DEG_UP_TO_FLAT_NE_SW, SPR_GHOST_TRAIN_TRACK_25_DEG_UP_TO_FLAT_FRONT_NE_SW},
    {SPR_GHOST_TRAIN_TRACK_25_DEG_UP_TO_FLAT_SE_NW, SPR_GHOST_TRAIN_TRACK_25_DEG_UP_TO_FLAT_FRONT_SE_NW},
};

static const uint32 ghost_train_track_pieces_25_deg_up[4][2] = {
    {SPR_GHOST_TRAIN_TRACK_25_DEG_UP_SW_NE, SPR_GHOST_TRAIN_TRACK_25_DEG_UP_FRONT_SW_NE},
    {SPR_GHOST_TRAIN_TRACK_25_DEG_UP_NW_SE, SPR_GHOST_TRAIN_TRACK_25_DEG_UP_FRONT_NW_SE},
    {SPR_GHOST_TRAIN_TRACK_25_DEG_UP_NE_SW, SPR_GHOST_TRAIN_TRACK_25_DEG_UP_FRONT_NE_SW},
    {SPR_GHOST_TRAIN_TRACK_25_DEG_UP_SE_NW, SPR_GHOST_TRAIN_TRACK_25_DEG_UP_FRONT_SE_NW},
};

static const uint32 ghost_train_track_pieces_quarter_turn_1_tile[4] = {
    SPR_GHOST_TRAIN_QUARTER_TURN_1_TILE_SW_NW,
    SPR_GHOST_TRAIN_QUARTER_TURN_1_TILE_NW_NE,
    SPR_GHOST_TRAIN_QUARTER_TURN_1_TILE_NE_SE,
    SPR_GHOST_TRAIN_QUARTER_TURN_1_TILE_SE_SW,
};

static const uint32 ghost_train_track_pieces_quarter_turn_3_tiles[4][3] = {
    {
        SPR_GHOST_TRAIN_QUARTER_TURN_3_TILES_SW_SE_PART_0,
        SPR_GHOST_TRAIN_QUARTER_TURN_3_TILES_SW_SE_PART_1,
        SPR_GHOST_TRAIN_QUARTER_TURN_3_TILES_SW_SE_PART_2
    },
    {
        SPR_GHOST_TRAIN_QUARTER_TURN_3_TILES_NW_SW_PART_0,
        SPR_GHOST_TRAIN_QUARTER_TURN_3_TILES_NW_SW_PART_1,
        SPR_GHOST_TRAIN_QUARTER_TURN_3_TILES_NW_SW_PART_2
    },
    {
        SPR_GHOST_TRAIN_QUARTER_TURN_3_TILES_NE_NW_PART_0,
        SPR_GHOST_TRAIN_QUARTER_TURN_3_TILES_NE_NW_PART_1,
        SPR_GHOST_TRAIN_QUARTER_TURN_3_TILES_NE_NW_PART_2
    },
    {
        SPR_GHOST_TRAIN_QUARTER_TURN_3_TILES_SE_NE_PART_0,
        SPR_GHOST_TRAIN_QUARTER_TURN_3_TILES_SE_NE_PART_1,
        SPR_GHOST_TRAIN_QUARTER_TURN_3_TILES_SE_NE_PART_2
    }
};

static const uint32 ghost_train_track_pieces_spinning_tunnel_track[4] = {
    SPR_GHOST_TRAIN_SPINNING_TUNNEL_TRACK_SW_NE,
    SPR_GHOST_TRAIN_SPINNING_TUNNEL_TRACK_NW_SE,
    SPR_GHOST_TRAIN_SPINNING_TUNNEL_TRACK_SW_NE,
    SPR_GHOST_TRAIN_SPINNING_TUNNEL_TRACK_NW_SE,
};

static const uint32 ghost_train_track_pieces_brakes[4] = {
    SPR_GHOST_TRAIN_TRACK_BRAKES_SW_NE,
    SPR_GHOST_TRAIN_TRACK_BRAKES_NW_SE,
    SPR_GHOST_TRAIN_TRACK_BRAKES_SW_NE,
    SPR_GHOST_TRAIN_TRACK_BRAKES_NW_SE,
};

/** rct2: 0x00770BEC */
static void paint_ghost_train_track_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    rct_xy16 position = session->MapPosition;

    uint32 imageId = ghost_train_track_pieces_flat[direction] | gTrackColours[SCHEME_TRACK];

    if (direction == 0 || direction == 2) {
        sub_98197C(imageId, 0, 0, 32, 20, 3, height, 0, 6, height, get_current_rotation());
    } else {
        sub_98197C(imageId, 0, 0, 20, 32, 3, height, 6, 0, height, get_current_rotation());
    }

    if (direction == 0 || direction == 2) {
        paint_util_push_tunnel_left(height, TUNNEL_0);
    } else {
        paint_util_push_tunnel_right(height, TUNNEL_0);
    }

    if (track_paint_util_should_paint_supports(position)) {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
    }

    paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(height + 32, 0x20);
}

/** rct2: 0x00770BFC */
static void paint_ghost_train_track_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    rct_xy16 position = session->MapPosition;

    uint32 imageId = ghost_train_track_pieces_25_deg_up[direction][0] | gTrackColours[SCHEME_TRACK];
    if (direction == 0 || direction == 2) {
        sub_98197C(imageId, 0, 0, 32, 20, 3, height, 0, 6, height, get_current_rotation());
    } else {
        sub_98197C(imageId, 0, 0, 20, 32, 3, height, 6, 0, height, get_current_rotation());
    }

    imageId = ghost_train_track_pieces_25_deg_up[direction][1] | gTrackColours[SCHEME_TRACK];
    if (direction == 0 || direction == 2) {
        sub_98197C(imageId, 0, 0, 32, 1, 23, height, 0, 27, height, get_current_rotation());
    } else {
        sub_98197C(imageId, 0, 0, 1, 32, 23, height, 27, 0, height, get_current_rotation());
    }

    if (track_paint_util_should_paint_supports(position)) {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
    }


    switch (direction) {
        case 0:
            paint_util_push_tunnel_left(height - 8, TUNNEL_1);
            break;
        case 1:
            paint_util_push_tunnel_right(height + 8, TUNNEL_2);
            break;
        case 2:
            paint_util_push_tunnel_left(height + 8, TUNNEL_2);
            break;
        case 3:
            paint_util_push_tunnel_right(height - 8, TUNNEL_1);
            break;
    }

    paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(height + 56, 0x20);
}

/** rct2: 0x00770C0C */
static void paint_ghost_train_track_flat_to_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    rct_xy16 position = session->MapPosition;

    uint32 imageId = ghost_train_track_pieces_flat_to_25_deg_up[direction][0] | gTrackColours[SCHEME_TRACK];
    if (direction == 0 || direction == 2) {
        sub_98197C(imageId, 0, 0, 32, 20, 3, height, 0, 6, height, get_current_rotation());
    } else {
        sub_98197C(imageId, 0, 0, 20, 32, 3, height, 6, 0, height, get_current_rotation());
    }

    imageId = ghost_train_track_pieces_flat_to_25_deg_up[direction][1] | gTrackColours[SCHEME_TRACK];
    if (direction == 0 || direction == 2) {
        sub_98197C(imageId, 0, 0, 32, 1, 15, height, 0, 27, height, get_current_rotation());
    } else {
        sub_98197C(imageId, 0, 0, 1, 32, 15, height, 27, 0, height, get_current_rotation());
    }

    if (track_paint_util_should_paint_supports(position)) {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 4, 3, height, gTrackColours[SCHEME_SUPPORTS]);
    }

    switch (direction) {
        case 0:
            paint_util_push_tunnel_left(height, TUNNEL_0);
            break;
        case 1:
            paint_util_push_tunnel_right(height, TUNNEL_2);
            break;
        case 2:
            paint_util_push_tunnel_left(height, TUNNEL_2);
            break;
        case 3:
            paint_util_push_tunnel_right(height, TUNNEL_0);
            break;
    }

    paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(height + 48, 0x20);
}

static void paint_ghost_train_track_25_deg_up_to_flat_shared(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    rct_xy16 position = session->MapPosition;

    uint32 imageId = ghost_train_track_pieces_25_deg_up_to_flat[direction][0] | gTrackColours[SCHEME_TRACK];
    if (direction == 0 || direction == 2) {
        sub_98197C(imageId, 0, 0, 32, 20, 3, height, 0, 6, height, get_current_rotation());
    } else {
        sub_98197C(imageId, 0, 0, 20, 32, 3, height, 6, 0, height, get_current_rotation());
    }

    imageId = ghost_train_track_pieces_25_deg_up_to_flat[direction][1] | gTrackColours[SCHEME_TRACK];
    if (direction == 0 || direction == 2) {
        sub_98197C(imageId, 0, 0, 32, 1, 15, height, 0, 27, height, get_current_rotation());
    } else {
        sub_98197C(imageId, 0, 0, 1, 32, 15, height, 27, 0, height, get_current_rotation());
    }

    if (track_paint_util_should_paint_supports(position)) {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
    }

    paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(height + 40, 0x20);
}

/** rct2: 0x00770C1C */
static void paint_ghost_train_track_25_deg_up_to_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    paint_ghost_train_track_25_deg_up_to_flat_shared(session, rideIndex, trackSequence, direction, height, mapElement);

    switch (direction) {
        case 0: paint_util_push_tunnel_left(height - 8, TUNNEL_0); break;
        case 1: paint_util_push_tunnel_right(height + 8, TUNNEL_12); break;
        case 2: paint_util_push_tunnel_left(height + 8, TUNNEL_12); break;
        case 3: paint_util_push_tunnel_right(height - 8, TUNNEL_0); break;
    }
}

/** rct2: 0x00770C2C */
static void paint_ghost_train_track_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    paint_ghost_train_track_25_deg_up(session, rideIndex, trackSequence, (direction + 2) % 4, height, mapElement);
}

/** rct2: 0x00770C3C */
static void paint_ghost_train_track_flat_to_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    paint_ghost_train_track_25_deg_up_to_flat_shared(session, rideIndex, trackSequence, (direction + 2) % 4, height, mapElement);

    switch ((direction + 2) % 4) {
        case 0: paint_util_push_tunnel_left(height - 8, TUNNEL_0); break;
        case 1: paint_util_push_tunnel_right(height + 8, TUNNEL_0); break;
        case 2: paint_util_push_tunnel_left(height + 8, TUNNEL_0); break;
        case 3: paint_util_push_tunnel_right(height - 8, TUNNEL_0); break;
    }
}

/** rct2: 0x00770C4C */
static void paint_ghost_train_track_25_deg_down_to_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    paint_ghost_train_track_flat_to_25_deg_up(session, rideIndex, trackSequence, (direction + 2) % 4, height, mapElement);
}

/** rct2: 0x00770C5C, 0x00770C6C, 0x00770C7C */
static void paint_ghost_train_station(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    uint32 imageId;

    if (direction == 0 || direction == 2) {
        imageId = SPR_STATION_BASE_B_SW_NE | gTrackColours[SCHEME_MISC];
        sub_98197C(imageId, 0, 0, 32, 28, 3, height - 2, 0, 2, height, get_current_rotation());
    } else if (direction == 1 || direction == 3) {
        imageId = SPR_STATION_BASE_B_NW_SE | gTrackColours[SCHEME_MISC];
        sub_98197C(imageId, 0, 0, 28, 32, 3, height - 2, 2, 0, height, get_current_rotation());
    }

    imageId = ghost_train_track_pieces_flat[direction] | gTrackColours[SCHEME_TRACK];
    if (direction == 0 || direction == 2) {
        sub_98199C(imageId, 0, 0, 32, 20, 3, height, 0, 0, height, get_current_rotation());
    } else {
        sub_98199C(imageId, 0, 0, 20, 32, 3, height, 0, 0, height, get_current_rotation());
    }

    if (direction == 0 || direction == 2) {
        paint_util_push_tunnel_left(height, TUNNEL_6);
    } else {
        paint_util_push_tunnel_right(height, TUNNEL_6);
    }

    if (direction == 0 || direction == 2) {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 5, 0, height, gTrackColours[SCHEME_SUPPORTS]);
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 8, 0, height, gTrackColours[SCHEME_SUPPORTS]);
    } else {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 6, 0, height, gTrackColours[SCHEME_SUPPORTS]);
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 7, 0, height, gTrackColours[SCHEME_SUPPORTS]);
    }

    track_paint_util_draw_station(session, rideIndex, trackSequence, direction, height, mapElement);

    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 32, 0x20);
}

/** rct2: 0x00770C9C */
static void paint_ghost_train_track_right_quarter_turn_3_tiles(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    track_paint_util_right_quarter_turn_3_tiles_paint(session, 3, height, direction, trackSequence, gTrackColours[SCHEME_TRACK], ghost_train_track_pieces_quarter_turn_3_tiles, NULL, defaultRightQuarterTurn3TilesBoundLengths, defaultRightQuarterTurn3TilesBoundOffsets, get_current_rotation());
    track_paint_util_right_quarter_turn_3_tiles_tunnel(session, height, direction, trackSequence, TUNNEL_0);

    switch (trackSequence) {
        case 0:
        case 3:
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
    }

    sint32 blockedSegments = 0;
    switch (trackSequence) {
        case 0: blockedSegments = SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_BC; break;
        case 2: blockedSegments = SEGMENT_D0 | SEGMENT_C4 | SEGMENT_D4 | SEGMENT_C0; break;
        case 3: blockedSegments = SEGMENT_D4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_B8; break;
    }
    paint_util_set_segment_support_height(paint_util_rotate_segments(blockedSegments, direction), 0xFFFF, 0);

    paint_util_set_general_support_height(height + 32, 0x20);
}

/** rct2: 0x00770CAC */
static void paint_ghost_train_track_left_quarter_turn_3_tiles(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    paint_ghost_train_track_right_quarter_turn_3_tiles(session, rideIndex, trackSequence, (direction + 1) % 4, height, mapElement);
}

/** rct2: 0x00770CAC */
static void paint_ghost_train_track_left_quarter_turn_1_tile(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    track_paint_util_left_quarter_turn_1_tile_paint(session, 3, height, 0, direction, gTrackColours[SCHEME_TRACK], ghost_train_track_pieces_quarter_turn_1_tile, get_current_rotation());
    track_paint_util_left_quarter_turn_1_tile_tunnel(session, direction, height, 0, TUNNEL_0, 0, TUNNEL_0);

    metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 32, 0x20);
}

/** rct2: 0x00770CBC */
static void paint_ghost_train_track_right_quarter_turn_1_tile(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    paint_ghost_train_track_left_quarter_turn_1_tile(session, rideIndex, trackSequence, (direction + 3) % 4, height, mapElement);
}

/** rct2: 0x00770CCC */
static void paint_ghost_train_track_spinning_tunnel(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    uint32 imageId = ghost_train_track_pieces_spinning_tunnel_track[direction] | gTrackColours[SCHEME_TRACK];

    if (direction == 0 || direction == 2) {
        sub_98197C(imageId, 0, 0, 28, 20, 3, height, 2, 6, height, get_current_rotation());
    } else {
        sub_98197C(imageId, 0, 0, 20, 28, 3, height, 6, 2, height, get_current_rotation());
    }

    track_paint_util_spinning_tunnel_paint(session, 3, height, direction, get_current_rotation());

    if (direction == 0 || direction == 2) {
        paint_util_push_tunnel_left(height, TUNNEL_0);
    } else {
        paint_util_push_tunnel_right(height, TUNNEL_0);
    }

    wooden_a_supports_paint_setup(session, (direction & 1), 0, height, gTrackColours[SCHEME_MISC], NULL);

    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 32, 0x20);
}

/** rct2: 0x00770CDC */
static void paint_ghost_train_track_brakes(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    rct_xy16 position = session->MapPosition;

    uint32 imageId = ghost_train_track_pieces_brakes[direction] | gTrackColours[SCHEME_TRACK];

    if (direction == 0 || direction == 2) {
        sub_98197C(imageId, 0, 0, 32, 20, 3, height, 0, 6, height, get_current_rotation());
    } else {
        sub_98197C(imageId, 0, 0, 20, 32, 3, height, 6, 0, height, get_current_rotation());
    }

    if (direction == 0 || direction == 2) {
        paint_util_push_tunnel_left(height, TUNNEL_0);
    } else {
        paint_util_push_tunnel_right(height, TUNNEL_0);
    }

    if (track_paint_util_should_paint_supports(position)) {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
    }

    paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(height + 32, 0x20);
}

/**
 * rct2: 0x00770924
 */
TRACK_PAINT_FUNCTION get_track_paint_function_ghost_train(sint32 trackType, sint32 direction) {
    switch(trackType) {
        case TRACK_ELEM_FLAT:
            return paint_ghost_train_track_flat;

        case TRACK_ELEM_END_STATION:
        case TRACK_ELEM_BEGIN_STATION:
        case TRACK_ELEM_MIDDLE_STATION:
            return paint_ghost_train_station;

        case TRACK_ELEM_25_DEG_UP:
            return paint_ghost_train_track_25_deg_up;
        case TRACK_ELEM_FLAT_TO_25_DEG_UP:
            return paint_ghost_train_track_flat_to_25_deg_up;
        case TRACK_ELEM_25_DEG_UP_TO_FLAT:
            return paint_ghost_train_track_25_deg_up_to_flat;

        case TRACK_ELEM_25_DEG_DOWN:
            return paint_ghost_train_track_25_deg_down;
        case TRACK_ELEM_FLAT_TO_25_DEG_DOWN:
            return paint_ghost_train_track_flat_to_25_deg_down;
        case TRACK_ELEM_25_DEG_DOWN_TO_FLAT:
            return paint_ghost_train_track_25_deg_down_to_flat;

        case TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES:
            return paint_ghost_train_track_left_quarter_turn_3_tiles;
        case TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES:
            return paint_ghost_train_track_right_quarter_turn_3_tiles;

        case TRACK_ELEM_LEFT_QUARTER_TURN_1_TILE:
            return paint_ghost_train_track_left_quarter_turn_1_tile;
        case TRACK_ELEM_RIGHT_QUARTER_TURN_1_TILE:
            return paint_ghost_train_track_right_quarter_turn_1_tile;

        case TRACK_ELEM_BRAKES:
            return paint_ghost_train_track_brakes;

        case TRACK_ELEM_SPINNING_TUNNEL:
            return paint_ghost_train_track_spinning_tunnel;
    }

    return NULL;
}
