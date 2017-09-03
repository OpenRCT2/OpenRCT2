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
#include "../../world/map.h"

enum
{
    SPR_GO_KARTS_FLAT_SW_NE = 20752,
    SPR_GO_KARTS_FLAT_NW_SE = 20753,
    SPR_GO_KARTS_FLAT_FRONT_SW_NE = 20754,
    SPR_GO_KARTS_FLAT_FRONT_NW_SE = 20755,
    SPR_GO_KARTS_STARTING_GRID_END_SW_NE = 20756,
    SPR_GO_KARTS_STARTING_GRID_END_NW_SE = 20757,
    SPR_GO_KARTS_STARTING_GRID_END_NE_SW = 20758,
    SPR_GO_KARTS_STARTING_GRID_END_SE_NW = 20759,
    SPR_GO_KARTS_STARTING_GRID_END_FRONT_SW_NE = 20760,
    SPR_GO_KARTS_STARTING_GRID_END_FRONT_NW_SE = 20761,
    SPR_GO_KARTS_STARTING_GRID_END_FRONT_NE_SW = 20762,
    SPR_GO_KARTS_STARTING_GRID_END_FRONT_SE_NW = 20763,
    SPR_GO_KARTS_STARTING_GRID_SW_NE = 20764,
    SPR_GO_KARTS_STARTING_GRID_NW_SE = 20765,
    SPR_GO_KARTS_STARTING_GRID_NE_SW = 20766,
    SPR_GO_KARTS_STARTING_GRID_SE_NW = 20767,
    SPR_GO_KARTS_STARTING_GRID_FRONT_SW_NE = 20768,
    SPR_GO_KARTS_STARTING_GRID_FRONT_NW_SE = 20769,
    SPR_GO_KARTS_STARTING_GRID_FRONT_NE_SW = 20770,
    SPR_GO_KARTS_STARTING_GRID_FRONT_SE_NW = 20771,
    SPR_GO_KARTS_FLAT_TO_25_DEG_UP_SW_NE = 20772,
    SPR_GO_KARTS_FLAT_TO_25_DEG_UP_NW_SE = 20773,
    SPR_GO_KARTS_FLAT_TO_25_DEG_UP_NE_SW = 20774,
    SPR_GO_KARTS_FLAT_TO_25_DEG_UP_SE_NW = 20775,
    SPR_GO_KARTS_25_DEG_UP_TO_FLAT_SW_NE = 20776,
    SPR_GO_KARTS_25_DEG_UP_TO_FLAT_NW_SE = 20777,
    SPR_GO_KARTS_25_DEG_UP_TO_FLAT_NE_SW = 20778,
    SPR_GO_KARTS_25_DEG_UP_TO_FLAT_SE_NW = 20779,
    SPR_GO_KARTS_25_DEG_UP_SW_NE = 20780,
    SPR_GO_KARTS_25_DEG_UP_NW_SE = 20781,
    SPR_GO_KARTS_25_DEG_UP_NE_SW = 20782,
    SPR_GO_KARTS_25_DEG_UP_SE_NW = 20783,
    SPR_GO_KARTS_FLAT_TO_25_DEG_UP_FRONT_SW_NE = 20784,
    SPR_GO_KARTS_FLAT_TO_25_DEG_UP_FRONT_NW_SE = 20785,
    SPR_GO_KARTS_FLAT_TO_25_DEG_UP_FRONT_NE_SW = 20786,
    SPR_GO_KARTS_FLAT_TO_25_DEG_UP_FRONT_SE_NW = 20787,
    SPR_GO_KARTS_25_DEG_UP_TO_FLAT_FRONT_SW_NE = 20788,
    SPR_GO_KARTS_25_DEG_UP_TO_FLAT_FRONT_NW_SE = 20789,
    SPR_GO_KARTS_25_DEG_UP_TO_FLAT_FRONT_NE_SW = 20790,
    SPR_GO_KARTS_25_DEG_UP_TO_FLAT_FRONT_SE_NW = 20791,
    SPR_GO_KARTS_25_DEG_UP_FRONT_SW_NE = 20792,
    SPR_GO_KARTS_25_DEG_UP_FRONT_NW_SE = 20793,
    SPR_GO_KARTS_25_DEG_UP_FRONT_NE_SW = 20794,
    SPR_GO_KARTS_25_DEG_UP_FRONT_SE_NW = 20795,
    SPR_GO_KARTS_FLAT_QUARTER_TURN_1_TILE_SW_NW = 20796,
    SPR_GO_KARTS_FLAT_QUARTER_TURN_1_TILE_NW_NE = 20797,
    SPR_GO_KARTS_FLAT_QUARTER_TURN_1_TILE_NE_SE = 20798,
    SPR_GO_KARTS_FLAT_QUARTER_TURN_1_TILE_SE_SW = 20799,
    SPR_GO_KARTS_FLAT_QUARTER_TURN_1_TILE_EDGE_A_SW_NW = 20800,
    SPR_GO_KARTS_FLAT_QUARTER_TURN_1_TILE_EDGE_A_NW_NE = 20801,
    SPR_GO_KARTS_FLAT_QUARTER_TURN_1_TILE_EDGE_A_NE_SE = 20802,
    SPR_GO_KARTS_FLAT_QUARTER_TURN_1_TILE_EDGE_A_SE_SW = 20803,
    SPR_GO_KARTS_FLAT_QUARTER_TURN_1_TILE_EDGE_B_SW_NW = 20804, // Empty
    SPR_GO_KARTS_FLAT_QUARTER_TURN_1_TILE_EDGE_B_NW_NE = 20805,
    SPR_GO_KARTS_FLAT_QUARTER_TURN_1_TILE_EDGE_B_NE_SE = 20806,
    SPR_GO_KARTS_FLAT_QUARTER_TURN_1_TILE_EDGE_B_SE_SW = 20807,
    SPR_GO_KARTS_START_POLE_RED_SW_NE = 20808,
    SPR_GO_KARTS_START_POLE_GREEN_SW_NE = 20809,
    SPR_GO_KARTS_START_POLE_NW_SE = 20810,
    SPR_GO_KARTS_START_POLE_NE_SW = 20811,
    SPR_GO_KARTS_START_POLE_RED_SE_NW = 20812,
    SPR_GO_KARTS_START_POLE_GREEN_SE_NW = 20813,
    SPR_GO_KARTS_START_LIGHTS_RED_SW_NE = 20814,
    SPR_GO_KARTS_START_LIGHTS_GREEN_SW_NE = 20815,
    SPR_GO_KARTS_START_LIGHTS_NW_SE = 20816,
    SPR_GO_KARTS_START_LIGHTS_NE_SW = 20817,
    SPR_GO_KARTS_START_LIGHTS_RED_SE_NW = 20818,
    SPR_GO_KARTS_START_LIGHTS_GREEN_SE_NW = 20819,
};

static const uint32 go_karts_track_pieces_starting_grid_end[4][2] = {
    {SPR_GO_KARTS_STARTING_GRID_END_SW_NE, SPR_GO_KARTS_STARTING_GRID_END_FRONT_SW_NE},
    {SPR_GO_KARTS_STARTING_GRID_END_NW_SE, SPR_GO_KARTS_STARTING_GRID_END_FRONT_NW_SE},
    {SPR_GO_KARTS_STARTING_GRID_END_NE_SW, SPR_GO_KARTS_STARTING_GRID_END_FRONT_NE_SW},
    {SPR_GO_KARTS_STARTING_GRID_END_SE_NW, SPR_GO_KARTS_STARTING_GRID_END_FRONT_SE_NW},
};

static const uint32 go_karts_track_pieces_starting_grid[4][2] = {
    {SPR_GO_KARTS_STARTING_GRID_SW_NE, SPR_GO_KARTS_STARTING_GRID_FRONT_SW_NE},
    {SPR_GO_KARTS_STARTING_GRID_NW_SE, SPR_GO_KARTS_STARTING_GRID_FRONT_NW_SE},
    {SPR_GO_KARTS_STARTING_GRID_NE_SW, SPR_GO_KARTS_STARTING_GRID_FRONT_NE_SW},
    {SPR_GO_KARTS_STARTING_GRID_SE_NW, SPR_GO_KARTS_STARTING_GRID_FRONT_SE_NW},
};

static const uint32 go_karts_track_pieces_25_deg_up[4][2] = {
    {SPR_GO_KARTS_25_DEG_UP_SW_NE, SPR_GO_KARTS_25_DEG_UP_FRONT_SW_NE},
    {SPR_GO_KARTS_25_DEG_UP_NW_SE, SPR_GO_KARTS_25_DEG_UP_FRONT_NW_SE},
    {SPR_GO_KARTS_25_DEG_UP_NE_SW, SPR_GO_KARTS_25_DEG_UP_FRONT_NE_SW},
    {SPR_GO_KARTS_25_DEG_UP_SE_NW, SPR_GO_KARTS_25_DEG_UP_FRONT_SE_NW},
};

static const uint32 go_karts_track_pieces_flat_to_25_deg_up[4][2] = {
    {SPR_GO_KARTS_FLAT_TO_25_DEG_UP_SW_NE, SPR_GO_KARTS_FLAT_TO_25_DEG_UP_FRONT_SW_NE},
    {SPR_GO_KARTS_FLAT_TO_25_DEG_UP_NW_SE, SPR_GO_KARTS_FLAT_TO_25_DEG_UP_FRONT_NW_SE},
    {SPR_GO_KARTS_FLAT_TO_25_DEG_UP_NE_SW, SPR_GO_KARTS_FLAT_TO_25_DEG_UP_FRONT_NE_SW},
    {SPR_GO_KARTS_FLAT_TO_25_DEG_UP_SE_NW, SPR_GO_KARTS_FLAT_TO_25_DEG_UP_FRONT_SE_NW},
};

static const uint32 go_karts_track_pieces_25_deg_up_to_flat[4][2] = {
    {SPR_GO_KARTS_25_DEG_UP_TO_FLAT_SW_NE, SPR_GO_KARTS_25_DEG_UP_TO_FLAT_FRONT_SW_NE},
    {SPR_GO_KARTS_25_DEG_UP_TO_FLAT_NW_SE, SPR_GO_KARTS_25_DEG_UP_TO_FLAT_FRONT_NW_SE},
    {SPR_GO_KARTS_25_DEG_UP_TO_FLAT_NE_SW, SPR_GO_KARTS_25_DEG_UP_TO_FLAT_FRONT_NE_SW},
    {SPR_GO_KARTS_25_DEG_UP_TO_FLAT_SE_NW, SPR_GO_KARTS_25_DEG_UP_TO_FLAT_FRONT_SE_NW},
};

/** rct2: 0x0074A748 */
static void paint_go_karts_track_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    uint32 imageId;
    if (direction == 0 || direction == 2) {
        imageId = SPR_GO_KARTS_FLAT_SW_NE | gTrackColours[SCHEME_TRACK];
        sub_98197C(session, imageId, 0, 0, 32, 28, 1, height, 0, 2, height, get_current_rotation());

        imageId = SPR_GO_KARTS_FLAT_FRONT_SW_NE | gTrackColours[SCHEME_TRACK];
        sub_98197C(session, imageId, 0, 0, 32, 1, 3, height, 0, 29, height + 2, get_current_rotation());

        paint_util_push_tunnel_left(session, height, TUNNEL_6);
    } else {
        imageId = SPR_GO_KARTS_FLAT_NW_SE | gTrackColours[SCHEME_TRACK];
        sub_98197C(session, imageId, 0, 0, 28, 32, 1, height, 2, 0, height, get_current_rotation());

        imageId = SPR_GO_KARTS_FLAT_FRONT_NW_SE | gTrackColours[SCHEME_TRACK];
        sub_98197C(session, imageId, 0, 0, 1, 32, 3, height, 29, 0, height + 2, get_current_rotation());

        paint_util_push_tunnel_right(session, height, TUNNEL_6);
    }

    wooden_a_supports_paint_setup(session, (direction & 1), 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);

    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x0074A758 */
static void paint_go_karts_track_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    uint32 imageId;
    paint_struct * ps;

    imageId = go_karts_track_pieces_25_deg_up[direction][0] | gTrackColours[SCHEME_TRACK];
    if (direction == 0 || direction == 2) {
        ps = sub_98197C(session, imageId, 0, 0, 32, 28, 1, height, 0, 2, height, get_current_rotation());
    } else {
        ps = sub_98197C(session, imageId, 0, 0, 28, 32, 1, height, 2, 0, height, get_current_rotation());
    }

    imageId = go_karts_track_pieces_25_deg_up[direction][1] | gTrackColours[SCHEME_TRACK];
    if (direction == 0 || direction == 2) {
        sub_98197C(session, imageId, 0, 0, 32, 1, 11, height, 0, 29, height + 2, get_current_rotation());
    } else {
        sub_98197C(session, imageId, 0, 0, 1, 32, 11, height, 29, 0, height + 2, get_current_rotation());
    }

    session->WoodenSupportsPrependTo = ps;

    switch (direction) {
        case 0:
            wooden_a_supports_paint_setup(session, 0, 9, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            paint_util_push_tunnel_left(session, height - 8, TUNNEL_7);
            break;
        case 1:
            wooden_a_supports_paint_setup(session, 1, 10, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            paint_util_push_tunnel_right(session, height + 8, TUNNEL_8);
            break;
        case 2:
            wooden_a_supports_paint_setup(session, 0, 11, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            paint_util_push_tunnel_left(session, height + 8, TUNNEL_8);
            break;
        case 3:
            wooden_a_supports_paint_setup(session, 1, 12, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            paint_util_push_tunnel_right(session, height - 8, TUNNEL_7);
            break;
    }

    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 56, 0x20);
}

/** rct2: 0x0074A768 */
static void paint_go_karts_track_flat_to_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    uint32 imageId;
    paint_struct * ps;

    imageId = go_karts_track_pieces_flat_to_25_deg_up[direction][0] | gTrackColours[SCHEME_TRACK];
    if (direction == 0 || direction == 2) {
        ps = sub_98197C(session, imageId, 0, 0, 32, 28, 1, height, 0, 2, height, get_current_rotation());
    } else {
        ps = sub_98197C(session, imageId, 0, 0, 28, 32, 1, height, 2, 0, height, get_current_rotation());
    }

    imageId = go_karts_track_pieces_flat_to_25_deg_up[direction][1] | gTrackColours[SCHEME_TRACK];
    if (direction == 0 || direction == 2) {
        sub_98197C(session, imageId, 0, 0, 32, 1, 11, height, 0, 29, height + 2, get_current_rotation());
    } else {
        sub_98197C(session, imageId, 0, 0, 1, 32, 11, height, 29, 0, height + 2, get_current_rotation());
    }

    session->WoodenSupportsPrependTo = ps;

    switch (direction) {
        case 0:
            wooden_a_supports_paint_setup(session, 0, 1, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            paint_util_push_tunnel_left(session, height, TUNNEL_6);
            break;
        case 1:
            wooden_a_supports_paint_setup(session, 1, 2, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            paint_util_push_tunnel_right(session, height, TUNNEL_8);
            break;
        case 2:
            wooden_a_supports_paint_setup(session, 0, 3, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            paint_util_push_tunnel_left(session, height, TUNNEL_8);
            break;
        case 3:
            wooden_a_supports_paint_setup(session, 1, 4, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            paint_util_push_tunnel_right(session, height, TUNNEL_6);
            break;
    }

    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 48, 0x20);
}

/** rct2: 0x */
static void paint_go_karts_track_25_deg_up_to_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    uint32 imageId;
    paint_struct * ps;

    imageId = go_karts_track_pieces_25_deg_up_to_flat[direction][0] | gTrackColours[SCHEME_TRACK];
    if (direction == 0 || direction == 2) {
        ps = sub_98197C(session, imageId, 0, 0, 32, 28, 1, height, 0, 2, height, get_current_rotation());
    } else {
        ps = sub_98197C(session, imageId, 0, 0, 28, 32, 1, height, 2, 0, height, get_current_rotation());
    }

    imageId = go_karts_track_pieces_25_deg_up_to_flat[direction][1] | gTrackColours[SCHEME_TRACK];
    if (direction == 0 || direction == 2) {
        sub_98197C(session, imageId, 0, 0, 32, 1, 11, height, 0, 29, height + 2, get_current_rotation());
    } else {
        sub_98197C(session, imageId, 0, 0, 1, 32, 11, height, 29, 0, height + 2, get_current_rotation());
    }

    session->WoodenSupportsPrependTo = ps;

    switch (direction) {
        case 0:
            wooden_a_supports_paint_setup(session, 0, 5, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            paint_util_push_tunnel_left(session, height - 8, TUNNEL_6);
            break;
        case 1:
            wooden_a_supports_paint_setup(session, 1, 6, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            paint_util_push_tunnel_right(session, height + 8, TUNNEL_14);
            break;
        case 2:
            wooden_a_supports_paint_setup(session, 0, 7, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            paint_util_push_tunnel_left(session, height + 8, TUNNEL_14);
            break;
        case 3:
            wooden_a_supports_paint_setup(session, 1, 8, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            paint_util_push_tunnel_right(session, height - 8, TUNNEL_6);
            break;
    }

    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 40, 0x20);
}

/** rct2: 0x0074A788 */
static void paint_go_karts_track_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    paint_go_karts_track_25_deg_up(session, rideIndex, trackSequence, (direction + 2) % 4, height, mapElement);
}

/** rct2: 0x0074A798 */
static void paint_go_karts_track_flat_to_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    paint_go_karts_track_25_deg_up_to_flat(session, rideIndex, trackSequence, (direction + 2) % 4, height, mapElement);
}

/** rct2: 0x0074A7A8 */
static void paint_go_karts_track_25_deg_down_to_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    paint_go_karts_track_flat_to_25_deg_up(session, rideIndex, trackSequence, (direction + 2) % 4, height, mapElement);
}

/** rct2: 0x */
static void paint_go_karts_station(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    rct_xy16 position = session->MapPosition;
    rct_ride * ride = get_ride(rideIndex);
    const rct_ride_entrance_definition * entranceStyle = &RideEntranceDefinitions[ride->entrance_style];

    bool hasFence;
    uint32 imageId;
    const uint32 (* sprites)[2] = go_karts_track_pieces_starting_grid;

    if (mapElement->properties.track.type == TRACK_ELEM_END_STATION) {
        sprites = go_karts_track_pieces_starting_grid_end;
    }

    imageId = sprites[direction][0] | gTrackColours[SCHEME_TRACK];
    if (direction == 0 || direction == 2) {
        sub_98197C(session, imageId, 0, 0, 32, 28, 1, height, 0, 2, height, get_current_rotation());
    } else {
        sub_98197C(session, imageId, 0, 0, 28, 32, 1, height, 2, 0, height, get_current_rotation());
    }

    if (direction == 0 || direction == 2) {
        hasFence = track_paint_util_has_fence(EDGE_NW, position, mapElement, ride, get_current_rotation());
        track_paint_util_draw_station_covers(session, EDGE_NW, hasFence, entranceStyle, direction, height);
    } else {
        hasFence = track_paint_util_has_fence(EDGE_NE, position, mapElement, ride, get_current_rotation());
        track_paint_util_draw_station_covers(session, EDGE_NE, hasFence, entranceStyle, direction, height);
    }

    imageId = sprites[direction][1] | gTrackColours[SCHEME_TRACK];
    if (direction == 0 || direction == 2) {
        sub_98197C(session, imageId, 0, 0, 32, 1, 3, height, 0, 29, height + 2, get_current_rotation());

        paint_util_push_tunnel_left(session, height, TUNNEL_6);
    } else {
        sub_98197C(session, imageId, 0, 0, 1, 32, 3, height, 29, 0, height + 2, get_current_rotation());

        paint_util_push_tunnel_right(session, height, TUNNEL_6);
    }

    if (direction == 0 || direction == 2) {
        hasFence = track_paint_util_has_fence(EDGE_SE, position, mapElement, ride, get_current_rotation());
        track_paint_util_draw_station_covers(session, EDGE_SE, hasFence, entranceStyle, direction, height);
    } else {
        hasFence = track_paint_util_has_fence(EDGE_SW, position, mapElement, ride, get_current_rotation());
        track_paint_util_draw_station_covers(session, EDGE_SW, hasFence, entranceStyle, direction, height);
    }

    if (mapElement->properties.track.type == TRACK_ELEM_END_STATION) {
        const bool hasGreenLight = map_element_get_green_light(mapElement);

        switch (direction) {
            case 0:
                imageId = (hasGreenLight ? SPR_GO_KARTS_START_POLE_GREEN_SW_NE : SPR_GO_KARTS_START_POLE_RED_SW_NE) | gTrackColours[SCHEME_TRACK];
                sub_98197C(session, imageId, 0, 0, 3, 3, 13, height, 1, 1, height + 4, get_current_rotation());

                imageId = (hasGreenLight ? SPR_GO_KARTS_START_LIGHTS_GREEN_SW_NE : SPR_GO_KARTS_START_LIGHTS_RED_SW_NE) | gTrackColours[SCHEME_TRACK];
                sub_98197C(session, imageId, 0, 0, 3, 3, 13, height, 1, 28, height + 4, get_current_rotation());
                break;
            case 1:
                imageId = SPR_GO_KARTS_START_POLE_NW_SE | gTrackColours[SCHEME_TRACK];
                sub_98197C(session, imageId, 0, 0, 3, 3, 13, height, 1, 28, height + 4, get_current_rotation());

                imageId = SPR_GO_KARTS_START_LIGHTS_NW_SE | gTrackColours[SCHEME_TRACK];
                sub_98197C(session, imageId, 0, 0, 3, 3, 13, height, 28, 28, height + 4, get_current_rotation());
                break;
            case 2:
                imageId = SPR_GO_KARTS_START_POLE_NE_SW | gTrackColours[SCHEME_TRACK];
                sub_98197C(session, imageId, 0, 0, 3, 3, 13, height, 28, 1, height + 4, get_current_rotation());

                imageId = SPR_GO_KARTS_START_LIGHTS_NE_SW | gTrackColours[SCHEME_TRACK];
                sub_98197C(session, imageId, 0, 0, 3, 3, 13, height, 28, 28, height + 4, get_current_rotation());
                break;
            case 3:
                imageId = (hasGreenLight ? SPR_GO_KARTS_START_POLE_GREEN_SE_NW : SPR_GO_KARTS_START_POLE_RED_SE_NW) | gTrackColours[SCHEME_TRACK];
                sub_98197C(session, imageId, 0, 0, 3, 3, 13, height, 1, 1, height + 4, get_current_rotation());

                imageId = (hasGreenLight ? SPR_GO_KARTS_START_LIGHTS_GREEN_SE_NW : SPR_GO_KARTS_START_LIGHTS_RED_SE_NW) | gTrackColours[SCHEME_TRACK];
                sub_98197C(session, imageId, 0, 0, 3, 3, 13, height, 28, 1, height + 4, get_current_rotation());
                break;
        }
    }

    wooden_a_supports_paint_setup(session, (direction & 1), 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);

    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x0074A7E8 */
static void paint_go_karts_track_left_quarter_turn_1_tile(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    uint32 imageId;

    switch (direction) {
        case 0:
            imageId = SPR_GO_KARTS_FLAT_QUARTER_TURN_1_TILE_NW_NE | gTrackColours[SCHEME_TRACK];
            sub_98197C(session, imageId, 0, 0, 32, 28, 1, height, 0, 2, height, get_current_rotation());

            imageId = SPR_GO_KARTS_FLAT_QUARTER_TURN_1_TILE_EDGE_A_NW_NE | gTrackColours[SCHEME_TRACK];
            sub_98197C(session, imageId, 0, 0, 1, 1, 3, height, 29, 2, height + 2, get_current_rotation());

            imageId = SPR_GO_KARTS_FLAT_QUARTER_TURN_1_TILE_EDGE_B_NW_NE | gTrackColours[SCHEME_TRACK];
            sub_98197C(session, imageId, 0, 0, 16, 1, 3, height, 14, 29, height + 2, get_current_rotation());
            break;
        case 1:
            imageId = SPR_GO_KARTS_FLAT_QUARTER_TURN_1_TILE_NE_SE | gTrackColours[SCHEME_TRACK];
            sub_98197C(session, imageId, 0, 0, 30, 30, 1, height, 0, 0, height, get_current_rotation());

            imageId = SPR_GO_KARTS_FLAT_QUARTER_TURN_1_TILE_EDGE_A_NE_SE | gTrackColours[SCHEME_TRACK];
            sub_98197C(session, imageId, 0, 0, 16, 1, 3, height, 2, 29, height + 2, get_current_rotation());

            imageId = SPR_GO_KARTS_FLAT_QUARTER_TURN_1_TILE_EDGE_B_NE_SE | gTrackColours[SCHEME_TRACK];
            sub_98197C(session, imageId, 0, 0, 1, 16, 3, height, 29, 2, height + 2, get_current_rotation());
            break;
        case 2:
            imageId = SPR_GO_KARTS_FLAT_QUARTER_TURN_1_TILE_SE_SW | gTrackColours[SCHEME_TRACK];
            sub_98197C(session, imageId, 0, 0, 28, 32, 1, height, 2, 0, height, get_current_rotation());

            imageId = SPR_GO_KARTS_FLAT_QUARTER_TURN_1_TILE_EDGE_A_SE_SW | gTrackColours[SCHEME_TRACK];
            sub_98197C(session, imageId, 0, 0, 1, 1, 3, height, 2, 2, height + 2, get_current_rotation());

            imageId = SPR_GO_KARTS_FLAT_QUARTER_TURN_1_TILE_EDGE_B_SE_SW | gTrackColours[SCHEME_TRACK];
            sub_98197C(session, imageId, 0, 0, 1, 16, 3, height, 29, 14, height + 2, get_current_rotation());
            break;
        case 3:
            imageId = SPR_GO_KARTS_FLAT_QUARTER_TURN_1_TILE_SW_NW | gTrackColours[SCHEME_TRACK];
            sub_98197C(session, imageId, 0, 0, 32, 32, 1, height, 0, 0, height, get_current_rotation());

            imageId = SPR_GO_KARTS_FLAT_QUARTER_TURN_1_TILE_EDGE_A_SW_NW | gTrackColours[SCHEME_TRACK];
            sub_98197C(session, imageId, 0, 0, 1, 1, 3, height, 29, 29, height + 2, get_current_rotation());

            // The empty sprite isn't drawn
            break;
    }

    switch (direction) {
        case 0:
            paint_util_push_tunnel_left(session, height, TUNNEL_6);
            break;
        case 2:
            paint_util_push_tunnel_right(session, height, TUNNEL_6);
            break;
        case 3:
            paint_util_push_tunnel_left(session, height, TUNNEL_6);
            paint_util_push_tunnel_right(session, height, TUNNEL_6);
            break;
    }

    wooden_a_supports_paint_setup(session, (direction & 1), 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);

    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x0074A7F8 */
static void paint_go_karts_track_right_quarter_turn_1_tile(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    paint_go_karts_track_left_quarter_turn_1_tile(session, rideIndex, trackSequence, (direction + 3) % 4, height, mapElement);
}

/**
 * rct2: 0x0074A668
 */
TRACK_PAINT_FUNCTION get_track_paint_function_go_karts(sint32 trackType, sint32 direction)
{
    switch (trackType) {
        case TRACK_ELEM_FLAT:
            return paint_go_karts_track_flat;

        case TRACK_ELEM_END_STATION:
        case TRACK_ELEM_BEGIN_STATION:
        case TRACK_ELEM_MIDDLE_STATION:
            return paint_go_karts_station;

        case TRACK_ELEM_25_DEG_UP:
            return paint_go_karts_track_25_deg_up;
        case TRACK_ELEM_FLAT_TO_25_DEG_UP:
            return paint_go_karts_track_flat_to_25_deg_up;
        case TRACK_ELEM_25_DEG_UP_TO_FLAT:
            return paint_go_karts_track_25_deg_up_to_flat;

        case TRACK_ELEM_25_DEG_DOWN:
            return paint_go_karts_track_25_deg_down;
        case TRACK_ELEM_FLAT_TO_25_DEG_DOWN:
            return paint_go_karts_track_flat_to_25_deg_down;
        case TRACK_ELEM_25_DEG_DOWN_TO_FLAT:
            return paint_go_karts_track_25_deg_down_to_flat;

        case TRACK_ELEM_LEFT_QUARTER_TURN_1_TILE:
            return paint_go_karts_track_left_quarter_turn_1_tile;
        case TRACK_ELEM_RIGHT_QUARTER_TURN_1_TILE:
            return paint_go_karts_track_right_quarter_turn_1_tile;
    }

    return NULL;
}
