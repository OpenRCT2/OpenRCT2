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

/** rct2: 0x */
static void paint_mini_helicopters_track_station(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    uint32 imageId;

    if (direction == 0 || direction == 2) {
        imageId = SPR_STATION_BASE_B_SW_NE | gTrackColours[SCHEME_MISC];
        sub_98197C(session, imageId, 0, 0, 32, 28, 1, height - 2, 0, 2, height, get_current_rotation());

        imageId = SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_NE_SW | gTrackColours[SCHEME_TRACK];
        sub_98199C(session, imageId, 0, 0, 32, 20, 1, height, 0, 0, height, get_current_rotation());

        metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 5, 0, height, gTrackColours[SCHEME_SUPPORTS]);
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 8, 0, height, gTrackColours[SCHEME_SUPPORTS]);
        paint_util_push_tunnel_left(session, height, TUNNEL_6);
    } else if (direction == 1 || direction == 3) {
        imageId = SPR_STATION_BASE_B_NW_SE | gTrackColours[SCHEME_MISC];
        sub_98197C(session, imageId, 0, 0, 28, 32, 1, height - 2, 2, 0, height, get_current_rotation());

        imageId = SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_SE_NW | gTrackColours[SCHEME_TRACK];
        sub_98199C(session, imageId, 0, 0, 20, 32, 1, height, 0, 0, height, get_current_rotation());

        metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 6, 0, height, gTrackColours[SCHEME_SUPPORTS]);
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 7, 0, height, gTrackColours[SCHEME_SUPPORTS]);
        paint_util_push_tunnel_right(session, height, TUNNEL_6);
    }

    track_paint_util_draw_station(session, rideIndex, trackSequence, direction, height, mapElement);

    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x0081F348 */
static void paint_mini_helicopters_track_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    rct_xy16 position = session->MapPosition;
    uint32 imageId;

    if (direction & 1) {
        imageId = SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_SE_NW | gTrackColours[SCHEME_TRACK];
        sub_98197C(session, imageId, 0, 0, 20, 32, 3, height, 6, 0, height, get_current_rotation());
        paint_util_push_tunnel_right(session, height, TUNNEL_0);
    } else {
        imageId = SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_NE_SW | gTrackColours[SCHEME_TRACK];
        sub_98197C(session, imageId, 0, 0, 32, 20, 3, height, 0, 6, height, get_current_rotation());
        paint_util_push_tunnel_left(session, height, TUNNEL_0);
    }

    if (track_paint_util_should_paint_supports(position)) {
        metal_a_supports_paint_setup(session, (direction & 1) ? METAL_SUPPORTS_STICK_ALT : METAL_SUPPORTS_STICK, 4, -1, height, gTrackColours[SCHEME_SUPPORTS]);
    }

    paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x0081F368 */
static void paint_mini_helicopters_track_flat_to_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    rct_xy16 position = session->MapPosition;
    uint32 imageId;

    switch (direction) {
        case 0:
            imageId = SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_TO_25_DEG_UP_SW_NE | gTrackColours[SCHEME_TRACK];
            sub_98197C(session, imageId, 0, 0, 32, 20, 3, height, 0, 6, height, get_current_rotation());
            paint_util_push_tunnel_left(session, height, TUNNEL_0);
            break;
        case 1:
            imageId = SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_TO_25_DEG_UP_NW_SE | gTrackColours[SCHEME_TRACK];
            sub_98197C(session, imageId, 0, 0, 20, 32, 3, height, 6, 0, height, get_current_rotation());
            paint_util_push_tunnel_right(session, height, TUNNEL_2);
            break;
        case 2:
            imageId = SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_TO_25_DEG_UP_NE_SW | gTrackColours[SCHEME_TRACK];
            sub_98197C(session, imageId, 0, 0, 32, 20, 3, height, 0, 6, height, get_current_rotation());
            paint_util_push_tunnel_left(session, height, TUNNEL_2);
            break;
        case 3:
            imageId = SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_TO_25_DEG_UP_SE_NW | gTrackColours[SCHEME_TRACK];
            sub_98197C(session, imageId, 0, 0, 20, 32, 3, height, 6, 0, height, get_current_rotation());
            paint_util_push_tunnel_right(session, height, TUNNEL_0);
            break;
    }

    if (track_paint_util_should_paint_supports(position)) {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK, 4, -4, height, gTrackColours[SCHEME_SUPPORTS]);
    }

    paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 48, 0x20);
}

/** rct2: 0x0081F358 */
static void paint_mini_helicopters_track_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    rct_xy16 position = session->MapPosition;
    uint32 imageId;

    switch (direction) {
        case 0:
            imageId = SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_25_DEG_UP_SW_NE | gTrackColours[SCHEME_TRACK];
            sub_98197C(session, imageId, 0, 0, 32, 20, 3, height, 0, 6, height, get_current_rotation());
            paint_util_push_tunnel_left(session, height - 8, TUNNEL_1);
            break;
        case 1:
            imageId = SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_25_DEG_UP_NW_SE | gTrackColours[SCHEME_TRACK];
            sub_98197C(session, imageId, 0, 0, 20, 32, 3, height, 6, 0, height, get_current_rotation());
            paint_util_push_tunnel_right(session, height + 8, TUNNEL_2);
            break;
        case 2:
            imageId = SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_25_DEG_UP_NE_SW | gTrackColours[SCHEME_TRACK];
            sub_98197C(session, imageId, 0, 0, 32, 20, 3, height, 0, 6, height, get_current_rotation());
            paint_util_push_tunnel_left(session, height + 8, TUNNEL_2);
            break;
        case 3:
            imageId = SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_25_DEG_UP_SE_NW | gTrackColours[SCHEME_TRACK];
            sub_98197C(session, imageId, 0, 0, 20, 32, 3, height, 6, 0, height, get_current_rotation());
            paint_util_push_tunnel_right(session, height - 8, TUNNEL_1);
            break;
    }

    if (track_paint_util_should_paint_supports(position)) {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK, 4, -9, height, gTrackColours[SCHEME_SUPPORTS]);
    }

    paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 56, 0x20);
}

/** rct2: 0x0081F378 */
static void paint_mini_helicopters_track_25_deg_up_to_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    rct_xy16 position = session->MapPosition;
    uint32 imageId;

    switch (direction) {
        case 0:
            imageId = SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_25_DEG_UP_TO_FLAT_SW_NE | gTrackColours[SCHEME_TRACK];
            sub_98197C(session, imageId, 0, 0, 32, 20, 3, height, 0, 6, height, get_current_rotation());
            paint_util_push_tunnel_left(session, height - 8, TUNNEL_0);
            break;
        case 1:
            imageId = SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_25_DEG_UP_TO_FLAT_NW_SE | gTrackColours[SCHEME_TRACK];
            sub_98197C(session, imageId, 0, 0, 20, 32, 3, height, 6, 0, height, get_current_rotation());
            paint_util_push_tunnel_right(session, height + 8, TUNNEL_12);
            break;
        case 2:
            imageId = SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_25_DEG_UP_TO_FLAT_NE_SW | gTrackColours[SCHEME_TRACK];
            sub_98197C(session, imageId, 0, 0, 32, 20, 3, height, 0, 6, height, get_current_rotation());
            paint_util_push_tunnel_left(session, height + 8, TUNNEL_12);
            break;
        case 3:
            imageId = SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_25_DEG_UP_TO_FLAT_SE_NW | gTrackColours[SCHEME_TRACK];
            sub_98197C(session, imageId, 0, 0, 20, 32, 3, height, 6, 0, height, get_current_rotation());
            paint_util_push_tunnel_right(session, height - 8, TUNNEL_0);
            break;
    }

    if (track_paint_util_should_paint_supports(position)) {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK, 4, -7, height, gTrackColours[SCHEME_SUPPORTS]);
    }

    paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 40, 0x20);
}

/** rct2: 0x */
static void paint_mini_helicopters_track_flat_to_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    paint_mini_helicopters_track_25_deg_up_to_flat(session, rideIndex, trackSequence, (direction + 2) % 4, height, mapElement);
}

/** rct2: 0x0081F388 */
static void paint_mini_helicopters_track_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    paint_mini_helicopters_track_25_deg_up(session, rideIndex, trackSequence, (direction + 2) % 4, height, mapElement);
}

/** rct2: 0x0081F3A8 */
static void paint_mini_helicopters_track_25_deg_down_to_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    paint_mini_helicopters_track_flat_to_25_deg_up(session, rideIndex, trackSequence, (direction + 2) % 4, height, mapElement);
}

/** rct2: 0x0081F3E8 */
static void paint_mini_helicopters_track_left_quarter_turn_3_tiles(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    track_paint_util_left_quarter_turn_3_tiles_paint(session, 3, height, direction, trackSequence, gTrackColours[SCHEME_TRACK], trackSpritesSubmarineRideMiniHelicoptersQuarterTurn3Tiles, get_current_rotation());
    track_paint_util_left_quarter_turn_3_tiles_tunnel(session, height, TUNNEL_0, direction, trackSequence);

    switch (trackSequence) {
        case 0:
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK, 4, -1, height, gTrackColours[SCHEME_SUPPORTS]);
            paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_B4, direction), 0xFFFF, 0);
            break;
        case 2:
            paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_C8 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_B8, direction), 0xFFFF, 0);
            break;
        case 3:
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK, 4, -1, height, gTrackColours[SCHEME_SUPPORTS]);
            paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_C8 | SEGMENT_C4 | SEGMENT_D4 | SEGMENT_C0, direction), 0xFFFF, 0);
            break;
    }

    paint_util_set_general_support_height(session, height + 32, 0x20);
}

static const uint8 mini_helicopters_right_quarter_turn_3_tiles_to_left_turn_map[] = {3, 1, 2, 0};

/** rct2: 0x0081F3F8 */
static void paint_mini_helicopters_track_right_quarter_turn_3_tiles(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    trackSequence = mini_helicopters_right_quarter_turn_3_tiles_to_left_turn_map[trackSequence];
    paint_mini_helicopters_track_left_quarter_turn_3_tiles(session, rideIndex, trackSequence, (direction + 3) % 4, height, mapElement);
}

/** rct2: 0x0081F408 */
static void paint_mini_helicopters_track_left_quarter_turn_1_tile(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    track_paint_util_left_quarter_turn_1_tile_paint(session, 1, height, 0, direction, gTrackColours[SCHEME_TRACK], trackSpritesSubmarineRideMiniHelicoptersQuarterTurn1Tile, get_current_rotation());
    track_paint_util_left_quarter_turn_1_tile_tunnel(session, direction, height, 0, TUNNEL_0, 0, TUNNEL_0);

    paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C8 | SEGMENT_C4 | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x0081F418 */
static void paint_mini_helicopters_track_right_quarter_turn_1_tile(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    paint_mini_helicopters_track_left_quarter_turn_1_tile(session, rideIndex, trackSequence, (direction + 3) % 4, height, mapElement);
}

/**
 * rct2: 0x0081F268
 */
TRACK_PAINT_FUNCTION get_track_paint_function_mini_helicopters(sint32 trackType, sint32 direction)
{
    switch (trackType) {
        case TRACK_ELEM_FLAT:
            return paint_mini_helicopters_track_flat;

        case TRACK_ELEM_END_STATION:
        case TRACK_ELEM_BEGIN_STATION:
        case TRACK_ELEM_MIDDLE_STATION:
            return paint_mini_helicopters_track_station;

        case TRACK_ELEM_25_DEG_UP:
            return paint_mini_helicopters_track_25_deg_up;
        case TRACK_ELEM_FLAT_TO_25_DEG_UP:
            return paint_mini_helicopters_track_flat_to_25_deg_up;
        case TRACK_ELEM_25_DEG_UP_TO_FLAT:
            return paint_mini_helicopters_track_25_deg_up_to_flat;

        case TRACK_ELEM_25_DEG_DOWN:
            return paint_mini_helicopters_track_25_deg_down;
        case TRACK_ELEM_FLAT_TO_25_DEG_DOWN:
            return paint_mini_helicopters_track_flat_to_25_deg_down;
        case TRACK_ELEM_25_DEG_DOWN_TO_FLAT:
            return paint_mini_helicopters_track_25_deg_down_to_flat;

        case TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES:
            return paint_mini_helicopters_track_left_quarter_turn_3_tiles;
        case TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES:
            return paint_mini_helicopters_track_right_quarter_turn_3_tiles;

        case TRACK_ELEM_LEFT_QUARTER_TURN_1_TILE:
            return paint_mini_helicopters_track_left_quarter_turn_1_tile;
        case TRACK_ELEM_RIGHT_QUARTER_TURN_1_TILE:
            return paint_mini_helicopters_track_right_quarter_turn_1_tile;
    }

    return NULL;
}
