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

#include "../../common.h"
#include "../track_paint.h"
#include "../track.h"
#include "../vehicle_paint.h"
#include "../../interface/viewport.h"
#include "../../paint/paint.h"
#include "../../paint/supports.h"
#include "../ride_data.h"
#include "../../world/map.h"

enum {
    SPR_CAR_RIDE_FLAT_SW_NE = 28773,
    SPR_CAR_RIDE_FLAT_NW_SE = 28774,
    SPR_CAR_RIDE_LOG_BUMPS_SW_NE = 28775,
    SPR_CAR_RIDE_LOG_BUMPS_NW_SE = 28776,
    SPR_CAR_RIDE_25_DEG_UP_SW_NE = 28777,
    SPR_CAR_RIDE_FLAT_TO_25_DEG_UP_SW_NE = 28778,
    SPR_CAR_RIDE_25_DEG_UP_NE_SW = 28779,
    SPR_CAR_RIDE_FLAT_TO_25_DEG_UP_NE_SW = 28780,
    SPR_CAR_RIDE_DEG_UP_TO_FLAT_NE_SW = 28781,
    SPR_CAR_RIDE_DEG_UP_TO_FLAT_SW_NE = 28782,
    SPR_CAR_RIDE_25_DEG_UP_NW_SE = 28783,
    SPR_CAR_RIDE_25_DEG_UP_SE_NW = 28784,
    SPR_CAR_RIDE_FLAT_TO_25_DEG_UP_NW_SE = 28785,
    SPR_CAR_RIDE_FLAT_TO_25_DEG_UP_SE_NW = 28786,
    SPR_CAR_RIDE_DEG_UP_TO_FLAT_SE_NW = 28787,
    SPR_CAR_RIDE_DEG_UP_TO_FLAT_NW_SE = 28788,
    SPR_CAR_RIDE_25_DEG_UP_TO_60_DEG_UP_SW_NE = 28789,
    SPR_CAR_RIDE_25_DEG_UP_TO_60_DEG_UP_NW_SE = 28790,
    SPR_CAR_RIDE_25_DEG_UP_TO_60_DEG_UP_NE_SW = 28791,
    SPR_CAR_RIDE_25_DEG_UP_TO_60_DEG_UP_SE_NW = 28792,
    SPR_CAR_RIDE_25_DEG_UP_TO_60_DEG_UP_FRONT_NW_SE = 28793,
    SPR_CAR_RIDE_25_DEG_UP_TO_60_DEG_UP_FRONT_NE_SW = 28794,
    SPR_CAR_RIDE_60_DEG_UP_TO_25_DEG_UP_SW_NE = 28795,
    SPR_CAR_RIDE_60_DEG_UP_TO_25_DEG_UP_NW_SE = 28796,
    SPR_CAR_RIDE_60_DEG_UP_TO_25_DEG_UP_NE_SW = 28797,
    SPR_CAR_RIDE_60_DEG_UP_TO_25_DEG_UP_SE_NW = 28798,
    SPR_CAR_RIDE_60_DEG_UP_TO_25_DEG_UP_FRONT_NW_SE = 28799,
    SPR_CAR_RIDE_60_DEG_UP_TO_25_DEG_UP_FRONT_NE_SW = 28800,
    SPR_CAR_RIDE_60_DEG_UP_SW_NE = 28801,
    SPR_CAR_RIDE_60_DEG_UP_NW_SE = 28802,
    SPR_CAR_RIDE_60_DEG_UP_NE_SW = 28803,
    SPR_CAR_RIDE_60_DEG_UP_SE_NW = 28804,
    SPR_CAR_RIDE_QUARTER_TURN_1_TILE_SW_NW = 28805,
    SPR_CAR_RIDE_QUARTER_TURN_1_TILE_NW_NE = 28806,
    SPR_CAR_RIDE_QUARTER_TURN_1_TILE_NE_SE = 28807,
    SPR_CAR_RIDE_QUARTER_TURN_1_TILE_SE_SW = 28808,
    SPR_CAR_RIDE_QUARTER_TURN_3_TILES_SW_SE_PART_0 = 28809,
    SPR_CAR_RIDE_QUARTER_TURN_3_TILES_SW_SE_PART_1 = 28810,
    SPR_CAR_RIDE_QUARTER_TURN_3_TILES_SW_SE_PART_2 = 28811,
    SPR_CAR_RIDE_QUARTER_TURN_3_TILES_NW_SW_PART_0 = 28812,
    SPR_CAR_RIDE_QUARTER_TURN_3_TILES_NW_SW_PART_1 = 28813,
    SPR_CAR_RIDE_QUARTER_TURN_3_TILES_NW_SW_PART_2 = 28814,
    SPR_CAR_RIDE_QUARTER_TURN_3_TILES_NE_NW_PART_0 = 28815,
    SPR_CAR_RIDE_QUARTER_TURN_3_TILES_NE_NW_PART_1 = 28816,
    SPR_CAR_RIDE_QUARTER_TURN_3_TILES_NE_NW_PART_2 = 28817,
    SPR_CAR_RIDE_QUARTER_TURN_3_TILES_SE_NE_PART_0 = 28818,
    SPR_CAR_RIDE_QUARTER_TURN_3_TILES_SE_NE_PART_1 = 28819,
    SPR_CAR_RIDE_QUARTER_TURN_3_TILES_SE_NE_PART_2 = 28820,
};

static const uint32 car_ride_track_pieces_flat[4] = {
    SPR_CAR_RIDE_FLAT_SW_NE,
    SPR_CAR_RIDE_FLAT_NW_SE,
    SPR_CAR_RIDE_FLAT_SW_NE,
    SPR_CAR_RIDE_FLAT_NW_SE,
};

static const uint32 car_ride_track_pieces_log_bumps[4] = {
    SPR_CAR_RIDE_LOG_BUMPS_SW_NE,
    SPR_CAR_RIDE_LOG_BUMPS_NW_SE,
    SPR_CAR_RIDE_LOG_BUMPS_SW_NE,
    SPR_CAR_RIDE_LOG_BUMPS_NW_SE,
};

static const uint32 car_ride_track_pieces_25_deg_up[4] = {
    SPR_CAR_RIDE_25_DEG_UP_SW_NE,
    SPR_CAR_RIDE_25_DEG_UP_NW_SE,
    SPR_CAR_RIDE_25_DEG_UP_NE_SW,
    SPR_CAR_RIDE_25_DEG_UP_SE_NW,
};

static const uint32 car_ride_track_pieces_flat_to_25_deg_up[4] = {
    SPR_CAR_RIDE_FLAT_TO_25_DEG_UP_SW_NE,
    SPR_CAR_RIDE_FLAT_TO_25_DEG_UP_NW_SE,
    SPR_CAR_RIDE_FLAT_TO_25_DEG_UP_NE_SW,
    SPR_CAR_RIDE_FLAT_TO_25_DEG_UP_SE_NW,
};

static const uint32 car_ride_track_pieces_25_deg_up_to_flat[4] = {
    SPR_CAR_RIDE_DEG_UP_TO_FLAT_SW_NE,
    SPR_CAR_RIDE_DEG_UP_TO_FLAT_NW_SE,
    SPR_CAR_RIDE_DEG_UP_TO_FLAT_NE_SW,
    SPR_CAR_RIDE_DEG_UP_TO_FLAT_SE_NW,
};

static const uint32 car_ride_track_pieces_60_deg_up[4] = {
    SPR_CAR_RIDE_60_DEG_UP_SW_NE,
    SPR_CAR_RIDE_60_DEG_UP_NW_SE,
    SPR_CAR_RIDE_60_DEG_UP_NE_SW,
    SPR_CAR_RIDE_60_DEG_UP_SE_NW,
};

static const uint32 car_ride_track_pieces_25_deg_up_to_60_deg_up[4][2] = {
    {SPR_CAR_RIDE_25_DEG_UP_TO_60_DEG_UP_SW_NE, 0},
    {SPR_CAR_RIDE_25_DEG_UP_TO_60_DEG_UP_NW_SE, SPR_CAR_RIDE_25_DEG_UP_TO_60_DEG_UP_FRONT_NW_SE},
    {SPR_CAR_RIDE_25_DEG_UP_TO_60_DEG_UP_NE_SW, SPR_CAR_RIDE_25_DEG_UP_TO_60_DEG_UP_FRONT_NE_SW},
    {SPR_CAR_RIDE_25_DEG_UP_TO_60_DEG_UP_SE_NW, 0},
};

static const uint32 car_ride_track_pieces_60_deg_up_to_25_deg_up[4][2] = {
    {SPR_CAR_RIDE_60_DEG_UP_TO_25_DEG_UP_SW_NE, 0},
    {SPR_CAR_RIDE_60_DEG_UP_TO_25_DEG_UP_NW_SE, SPR_CAR_RIDE_60_DEG_UP_TO_25_DEG_UP_FRONT_NW_SE},
    {SPR_CAR_RIDE_60_DEG_UP_TO_25_DEG_UP_NE_SW, SPR_CAR_RIDE_60_DEG_UP_TO_25_DEG_UP_FRONT_NE_SW},
    {SPR_CAR_RIDE_60_DEG_UP_TO_25_DEG_UP_SE_NW, 0},
};

static const uint32 car_ride_track_pieces_left_quarter_turn_1_tile[4] = {
    SPR_CAR_RIDE_QUARTER_TURN_1_TILE_SW_NW,
    SPR_CAR_RIDE_QUARTER_TURN_1_TILE_NW_NE,
    SPR_CAR_RIDE_QUARTER_TURN_1_TILE_NE_SE,
    SPR_CAR_RIDE_QUARTER_TURN_1_TILE_SE_SW,
};

static const uint32 car_ride_track_pieces_quarter_turn_3_tiles[4][3] = {
    {
        SPR_CAR_RIDE_QUARTER_TURN_3_TILES_SW_SE_PART_0,
        SPR_CAR_RIDE_QUARTER_TURN_3_TILES_SW_SE_PART_1,
        SPR_CAR_RIDE_QUARTER_TURN_3_TILES_SW_SE_PART_2
    },
    {
        SPR_CAR_RIDE_QUARTER_TURN_3_TILES_NW_SW_PART_0,
        SPR_CAR_RIDE_QUARTER_TURN_3_TILES_NW_SW_PART_1,
        SPR_CAR_RIDE_QUARTER_TURN_3_TILES_NW_SW_PART_2
    },
    {
        SPR_CAR_RIDE_QUARTER_TURN_3_TILES_NE_NW_PART_0,
        SPR_CAR_RIDE_QUARTER_TURN_3_TILES_NE_NW_PART_1,
        SPR_CAR_RIDE_QUARTER_TURN_3_TILES_NE_NW_PART_2
    },
    {
        SPR_CAR_RIDE_QUARTER_TURN_3_TILES_SE_NE_PART_0,
        SPR_CAR_RIDE_QUARTER_TURN_3_TILES_SE_NE_PART_1,
        SPR_CAR_RIDE_QUARTER_TURN_3_TILES_SE_NE_PART_2
    }
};

/** rct2: 0x006F72C8 */
static void paint_car_ride_track_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    uint32 imageId = car_ride_track_pieces_flat[direction] | gTrackColours[SCHEME_TRACK];

    if (direction == 0 || direction == 2) {
        sub_98196C(imageId, 0, 6, 32, 20, 1, height, get_current_rotation());
    } else {
        sub_98196C(imageId, 6, 0, 20, 32, 1, height, get_current_rotation());
    }

    if (direction == 0 || direction == 2) {
        paint_util_push_tunnel_left(height, TUNNEL_0);
    } else {
        paint_util_push_tunnel_right(height, TUNNEL_0);
    }

    metal_a_supports_paint_setup(METAL_SUPPORTS_BOXED, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
    paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(height + 32, 0x20);
}

/** rct2: 0x006F72D8 */
static void paint_car_ride_track_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    uint32 imageId = car_ride_track_pieces_25_deg_up[direction] | gTrackColours[SCHEME_TRACK];

    if (direction == 0 || direction == 2) {
        sub_98197C(imageId, 0, 2, 32, 20, 1, height, 0, 6, height, get_current_rotation());
    } else {
        sub_98197C(imageId, 2, 0, 20, 32, 1, height, 6, 0, height, get_current_rotation());
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

    metal_a_supports_paint_setup(METAL_SUPPORTS_BOXED, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
    paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(height + 56, 0x20);
}

/** rct2: 0x006F72E8 */
static void paint_car_ride_track_flat_to_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    uint32 imageId = car_ride_track_pieces_flat_to_25_deg_up[direction] | gTrackColours[SCHEME_TRACK];

    if (direction == 0 || direction == 2) {
        sub_98197C(imageId, 0, 2, 32, 20, 1, height, 0, 6, height, get_current_rotation());
    } else {
        sub_98197C(imageId, 2, 0, 20, 32, 1, height, 6, 0, height, get_current_rotation());
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

    metal_a_supports_paint_setup(METAL_SUPPORTS_BOXED, 4, 3, height, gTrackColours[SCHEME_SUPPORTS]);
    paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(height + 48, 0x20);
}

/** rct2: 0x006F72F8 */
static void paint_car_ride_track_25_deg_up_to_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    uint32 imageId = car_ride_track_pieces_25_deg_up_to_flat[direction] | gTrackColours[SCHEME_TRACK];

    if (direction == 0 || direction == 2) {
        sub_98197C(imageId, 0, 2, 32, 20, 1, height, 0, 6, height, get_current_rotation());
    } else {
        sub_98197C(imageId, 2, 0, 20, 32, 1, height, 6, 0, height, get_current_rotation());
    }

    switch (direction) {
        case 0:
            paint_util_push_tunnel_left(height - 8, TUNNEL_0);
            break;
        case 1:
            paint_util_push_tunnel_right(height + 8, TUNNEL_12);
            break;
        case 2:
            paint_util_push_tunnel_left(height + 8, TUNNEL_12);
            break;
        case 3:
            paint_util_push_tunnel_right(height - 8, TUNNEL_0);
            break;
    }

    metal_a_supports_paint_setup(METAL_SUPPORTS_BOXED, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
    paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(height + 40, 0x20);
}

/** rct2: 0x006F7308 */
static void paint_car_ride_track_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    paint_car_ride_track_25_deg_up(rideIndex, trackSequence, (direction + 2) % 4, height, mapElement);
}

/** rct2: 0x006F7318 */
static void paint_car_ride_track_flat_to_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    paint_car_ride_track_25_deg_up_to_flat(rideIndex, trackSequence, (direction + 2) % 4, height, mapElement);
}

/** rct2: 0x006F7328 */
static void paint_car_ride_track_25_deg_down_to_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    paint_car_ride_track_flat_to_25_deg_up(rideIndex, trackSequence, (direction + 2) % 4, height, mapElement);
}

/** rct2: 0x006F7338, 0x006F7348, 0x006F7358 */
static void paint_car_ride_station(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    uint32 imageId;

    if (direction == 0 || direction == 2) {
        imageId = SPR_STATION_BASE_B_SW_NE | gTrackColours[SCHEME_MISC];
        sub_98197C(imageId, 0, 0, 32, 28, 1, height - 2, 0, 2, height, get_current_rotation());
    } else if (direction == 1 || direction == 3) {
        imageId = SPR_STATION_BASE_B_NW_SE | gTrackColours[SCHEME_MISC];
        sub_98197C(imageId, 0, 0, 28, 32, 1, height - 2, 2, 0, height, get_current_rotation());
    }

    imageId = car_ride_track_pieces_flat[direction] | gTrackColours[SCHEME_TRACK];
    if (direction == 0 || direction == 2) {
        sub_98199C(imageId, 0, 6, 32, 20, 1, height, 0, 0, height, get_current_rotation());
    } else {
        sub_98199C(imageId, 6, 0, 20, 32, 1, height, 0, 0, height, get_current_rotation());
    }

    if (direction == 0 || direction == 2) {
        paint_util_push_tunnel_left(height, TUNNEL_6);
    } else {
        paint_util_push_tunnel_right(height, TUNNEL_6);
    }

    if (direction == 0 || direction == 2) {
        metal_a_supports_paint_setup(METAL_SUPPORTS_BOXED, 5, 0, height, gTrackColours[SCHEME_SUPPORTS]);
        metal_a_supports_paint_setup(METAL_SUPPORTS_BOXED, 8, 0, height, gTrackColours[SCHEME_SUPPORTS]);
    } else {
        metal_a_supports_paint_setup(METAL_SUPPORTS_BOXED, 6, 0, height, gTrackColours[SCHEME_SUPPORTS]);
        metal_a_supports_paint_setup(METAL_SUPPORTS_BOXED, 7, 0, height, gTrackColours[SCHEME_SUPPORTS]);
    }

    track_paint_util_draw_station(rideIndex, trackSequence, direction, height, mapElement);

    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 32, 0x20);
}

/** rct2: 0x006F7378 */
static void paint_car_ride_track_right_quarter_turn_3_tiles(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    track_paint_util_right_quarter_turn_3_tiles_paint(3, height, direction, trackSequence, gTrackColours[SCHEME_TRACK], car_ride_track_pieces_quarter_turn_3_tiles, defaultRightQuarterTurn3TilesOffsets, defaultRightQuarterTurn3TilesBoundLengths, NULL, get_current_rotation());
    track_paint_util_right_quarter_turn_3_tiles_tunnel(height, direction, trackSequence, TUNNEL_0);

    switch (trackSequence) {
        case 0:
        case 3:
            metal_a_supports_paint_setup(METAL_SUPPORTS_BOXED, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
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

/** rct2: 0x006F7368 */
static void paint_car_ride_track_left_quarter_turn_3_tiles(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    paint_car_ride_track_right_quarter_turn_3_tiles(rideIndex, trackSequence, (direction + 1) % 4, height, mapElement);
}

/** rct2: 0x006F7388 */
static void paint_car_ride_track_left_quarter_turn_1_tile(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    uint32 imageId = car_ride_track_pieces_left_quarter_turn_1_tile[direction] | gTrackColours[SCHEME_TRACK];

    switch(direction) {
        case 0: sub_98197C(imageId, 6, 0, 26, 24, 1, height, 6, 2, height, get_current_rotation()); break;
        case 1: sub_98196C(imageId, 0, 0, 26, 26, 1, height, get_current_rotation()); break;
        case 2: sub_98197C(imageId, 0, 6, 24, 26, 1, height, 2, 6, height, get_current_rotation()); break;
        case 3: sub_98196C(imageId, 6, 6, 24, 24, 1, height, get_current_rotation()); break;
    }

    metal_a_supports_paint_setup(METAL_SUPPORTS_BOXED, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);

    track_paint_util_left_quarter_turn_1_tile_tunnel(direction, height, 0, TUNNEL_0, 0, TUNNEL_0);

    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 32, 0x20);
}

/** rct2: 0x006F7398 */
static void paint_car_ride_track_right_quarter_turn_1_tile(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    paint_car_ride_track_left_quarter_turn_1_tile(rideIndex, trackSequence, (direction + 3) % 4, height, mapElement);
}

/** rct2: 0x006F73A8 */
static void paint_car_ride_track_spinning_tunnel(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    uint32 imageId = car_ride_track_pieces_flat[direction] | gTrackColours[SCHEME_TRACK];

    if (direction == 0 || direction == 2) {
        sub_98196C(imageId, 0, 6, 32, 20, 1, height, get_current_rotation());
    } else {
        sub_98196C(imageId, 6, 0, 20, 32, 1, height, get_current_rotation());
    }

    track_paint_util_spinning_tunnel_paint(1, height, direction, get_current_rotation());

    if (direction == 0 || direction == 2) {
        paint_util_push_tunnel_left(height, TUNNEL_0);
    } else {
        paint_util_push_tunnel_right(height, TUNNEL_0);
    }

    wooden_a_supports_paint_setup((direction & 1), 0, height, gTrackColours[SCHEME_MISC], NULL);

    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 32, 0x20);
}

/** rct2: 0x006F73B8 */
static void paint_car_ride_track_60_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    rct_xy16 position = gPaintMapPosition;

    uint32 imageId = car_ride_track_pieces_60_deg_up[direction] | gTrackColours[SCHEME_TRACK];

    switch (direction) {
        case 0:
            sub_98197C(imageId, 0, 0, 32, 20, 1, height, 0, 6, height, get_current_rotation());
            break;
        case 1:
            sub_98197C(imageId, 0, 0, 1, 32, 98, height, 27, 0, height, get_current_rotation());
            break;
        case 2:
            sub_98197C(imageId, 0, 0, 32, 1, 98, height, 0, 27, height, get_current_rotation());
            break;
        case 3:
            sub_98197C(imageId, 0, 0, 20, 32, 1, height, 6, 0, height, get_current_rotation());
            break;
    }

    switch (direction) {
        case 0:
            paint_util_push_tunnel_left(height - 8, TUNNEL_1);
            break;
        case 1:
            paint_util_push_tunnel_right(height + 56, TUNNEL_2);
            break;
        case 2:
            paint_util_push_tunnel_left(height + 56, TUNNEL_2);
            break;
        case 3:
            paint_util_push_tunnel_right(height - 8, TUNNEL_1);
            break;
    }


    if (track_paint_util_should_paint_supports(position)) {
        metal_a_supports_paint_setup(METAL_SUPPORTS_BOXED, 4, 32, height, gTrackColours[SCHEME_SUPPORTS]);
    }

    paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(height + 104, 0x20);
}

/** rct2: 0x006F73C8 */
static void paint_car_ride_track_25_deg_up_to_60_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    rct_xy16 position = gPaintMapPosition;

    uint32 imageId = car_ride_track_pieces_25_deg_up_to_60_deg_up[direction][0] | gTrackColours[SCHEME_TRACK];

    if (direction == 0 || direction == 2) {
        sub_98197C(imageId, 0, 0, 32, 20, 1, height, 0, 6, height, get_current_rotation());
    } else {
        sub_98197C(imageId, 0, 0, 20, 32, 1, height, 6, 0, height, get_current_rotation());
    }

    if (car_ride_track_pieces_25_deg_up_to_60_deg_up[direction][1] != 0) {
        imageId = car_ride_track_pieces_25_deg_up_to_60_deg_up[direction][1] | gTrackColours[SCHEME_TRACK];

        if (direction == 0 || direction == 2) {
            sub_98197C(imageId, 0, 0, 32, 1, 66, height, 0, 27, height, get_current_rotation());
        } else {
            sub_98197C(imageId, 0, 0, 1, 32, 66, height, 27, 0, height, get_current_rotation());
        }
    }

    switch (direction) {
        case 0:
            paint_util_push_tunnel_left(height - 8, TUNNEL_1);
            break;
        case 1:
            paint_util_push_tunnel_right(height + 24, TUNNEL_2);
            break;
        case 2:
            paint_util_push_tunnel_left(height + 24, TUNNEL_2);
            break;
        case 3:
            paint_util_push_tunnel_right(height - 8, TUNNEL_1);
            break;
    }

    if (track_paint_util_should_paint_supports(position)) {
        metal_a_supports_paint_setup(METAL_SUPPORTS_BOXED, 4, 12, height, gTrackColours[SCHEME_SUPPORTS]);
    }

    paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(height + 72, 0x20);
}

/** rct2: 0x006F73D8 */
static void paint_car_ride_track_60_deg_up_to_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    rct_xy16 position = gPaintMapPosition;

    uint32 imageId = car_ride_track_pieces_60_deg_up_to_25_deg_up[direction][0] | gTrackColours[SCHEME_TRACK];

    if (direction == 0 || direction == 2) {
        sub_98197C(imageId, 0, 0, 32, 20, 1, height, 0, 6, height, get_current_rotation());
    } else {
        sub_98197C(imageId, 0, 0, 20, 32, 1, height, 6, 0, height, get_current_rotation());
    }

    if (car_ride_track_pieces_60_deg_up_to_25_deg_up[direction][1] != 0) {
        imageId = car_ride_track_pieces_60_deg_up_to_25_deg_up[direction][1] | gTrackColours[SCHEME_TRACK];

        if (direction == 0 || direction == 2) {
            sub_98197C(imageId, 0, 0, 32, 1, 66, height, 0, 27, height, get_current_rotation());
        } else {
            sub_98197C(imageId, 0, 0, 1, 32, 66, height, 27, 0, height, get_current_rotation());
        }
    }

    switch (direction) {
        case 0:
            paint_util_push_tunnel_left(height - 8, TUNNEL_1);
            break;
        case 1:
            paint_util_push_tunnel_right(height + 24, TUNNEL_2);
            break;
        case 2:
            paint_util_push_tunnel_left(height + 24, TUNNEL_2);
            break;
        case 3:
            paint_util_push_tunnel_right(height - 8, TUNNEL_1);
            break;
    }

    if (track_paint_util_should_paint_supports(position)) {
        metal_a_supports_paint_setup(METAL_SUPPORTS_BOXED, 4, 20, height, gTrackColours[SCHEME_SUPPORTS]);
    }

    paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(height + 72, 0x20);
}

/** rct2: 0x006F73E8 */
static void paint_car_ride_track_60_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    paint_car_ride_track_60_deg_up(rideIndex, trackSequence, (direction + 2) % 4, height, mapElement);
}

/** rct2: 0x006F73F8 */
static void paint_car_ride_track_25_deg_down_to_60_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    paint_car_ride_track_60_deg_up_to_25_deg_up(rideIndex, trackSequence, (direction + 2) % 4, height, mapElement);
}

/** rct2: 0x006F7408 */
static void paint_car_ride_track_60_deg_down_to_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    paint_car_ride_track_25_deg_up_to_60_deg_up(rideIndex, trackSequence, (direction + 2) % 4, height, mapElement);
}

/** rct2: 0x006F7418 */
static void paint_car_ride_track_log_bumps(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    uint32 imageId = car_ride_track_pieces_log_bumps[direction] | gTrackColours[SCHEME_TRACK];

    if (direction == 0 || direction == 2) {
        sub_98196C(imageId, 0, 6, 32, 20, 1, height, get_current_rotation());
    } else {
        sub_98196C(imageId, 6, 0, 20, 32, 1, height, get_current_rotation());
    }

    if (direction == 0 || direction == 2) {
        paint_util_push_tunnel_left(height, TUNNEL_0);
    } else {
        paint_util_push_tunnel_right(height, TUNNEL_0);
    }

    metal_a_supports_paint_setup(METAL_SUPPORTS_BOXED, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
    paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(height + 32, 0x20);
}

/**
 * rct2: 0x006F7000
 */
TRACK_PAINT_FUNCTION get_track_paint_function_car_ride(sint32 trackType, sint32 direction)
{
    switch (trackType) {
        case TRACK_ELEM_FLAT:
            return paint_car_ride_track_flat;

        case TRACK_ELEM_END_STATION:
        case TRACK_ELEM_BEGIN_STATION:
        case TRACK_ELEM_MIDDLE_STATION:
            return paint_car_ride_station;

        case TRACK_ELEM_25_DEG_UP:
            return paint_car_ride_track_25_deg_up;
        case TRACK_ELEM_60_DEG_UP:
            return paint_car_ride_track_60_deg_up;
        case TRACK_ELEM_FLAT_TO_25_DEG_UP:
            return paint_car_ride_track_flat_to_25_deg_up;
        case TRACK_ELEM_25_DEG_UP_TO_60_DEG_UP:
            return paint_car_ride_track_25_deg_up_to_60_deg_up;
        case TRACK_ELEM_60_DEG_UP_TO_25_DEG_UP:
            return paint_car_ride_track_60_deg_up_to_25_deg_up;
        case TRACK_ELEM_25_DEG_UP_TO_FLAT:
            return paint_car_ride_track_25_deg_up_to_flat;

        case TRACK_ELEM_25_DEG_DOWN:
            return paint_car_ride_track_25_deg_down;
        case TRACK_ELEM_60_DEG_DOWN:
            return paint_car_ride_track_60_deg_down;
        case TRACK_ELEM_FLAT_TO_25_DEG_DOWN:
            return paint_car_ride_track_flat_to_25_deg_down;
        case TRACK_ELEM_25_DEG_DOWN_TO_60_DEG_DOWN:
            return paint_car_ride_track_25_deg_down_to_60_deg_down;
        case TRACK_ELEM_60_DEG_DOWN_TO_25_DEG_DOWN:
            return paint_car_ride_track_60_deg_down_to_25_deg_down;
        case TRACK_ELEM_25_DEG_DOWN_TO_FLAT:
            return paint_car_ride_track_25_deg_down_to_flat;

        case TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES:
            return paint_car_ride_track_left_quarter_turn_3_tiles;
        case TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES:
            return paint_car_ride_track_right_quarter_turn_3_tiles;

        case TRACK_ELEM_LEFT_QUARTER_TURN_1_TILE:
            return paint_car_ride_track_left_quarter_turn_1_tile;
        case TRACK_ELEM_RIGHT_QUARTER_TURN_1_TILE:
            return paint_car_ride_track_right_quarter_turn_1_tile;

        case TRACK_ELEM_RAPIDS:
            return paint_car_ride_track_log_bumps;

        case TRACK_ELEM_SPINNING_TUNNEL:
            return paint_car_ride_track_spinning_tunnel;
    }

    return NULL;
}
