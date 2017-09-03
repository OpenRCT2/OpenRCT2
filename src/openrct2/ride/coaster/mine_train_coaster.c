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

#include "../../drawing/drawing.h"
#include "../../paint/supports.h"
#include "../../interface/viewport.h"
#include "../../paint/map_element/map_element.h"
#include "../../paint/paint.h"
#include "../../sprites.h"
#include "../../world/map.h"
#include "../../world/sprite.h"
#include "../ride_data.h"
#include "../track_data.h"
#include "../track_paint.h"

/** rct2: 0x0071BFA4 */
static void mine_train_rc_track_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    if (track_element_is_lift_hill(mapElement)) {
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20054, 0, 0, 32, 20, 1, height, 0, 6, height);
            wooden_a_supports_paint_setup(session, 6, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20055, 0, 0, 32, 20, 1, height, 0, 6, height);
            wooden_a_supports_paint_setup(session, 7, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20056, 0, 0, 32, 20, 1, height, 0, 6, height);
            wooden_a_supports_paint_setup(session, 6, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20057, 0, 0, 32, 20, 1, height, 0, 6, height);
            wooden_a_supports_paint_setup(session, 7, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
    } else {
        switch (direction) {
        case 0:
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20052, 0, 0, 32, 20, 1, height, 0, 6, height);
            wooden_a_supports_paint_setup(session, 6, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20053, 0, 0, 32, 20, 1, height, 0, 6, height);
            wooden_a_supports_paint_setup(session, 7, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
    }
    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_6);
    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x0071C154, 0x0071C164, 0x0071C174 */
static void mine_train_rc_track_station(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    static const uint32 imageIds[4][3] = {
        { 20064, 20060, SPR_STATION_BASE_B_SW_NE },
        { 20065, 20061, SPR_STATION_BASE_B_NW_SE },
        { 20064, 20060, SPR_STATION_BASE_B_SW_NE },
        { 20065, 20061, SPR_STATION_BASE_B_NW_SE },
    };

    sub_98197C_rotated(session, direction, imageIds[direction][2] | gTrackColours[SCHEME_MISC], 0, 0, 32, 28, 1, height - 2, 0, 2, height);
    if (mapElement->properties.track.type == TRACK_ELEM_END_STATION) {
        sub_98199C_rotated(session, direction, imageIds[direction][1] | gTrackColours[SCHEME_TRACK], 0, 0, 32, 20, 1, height, 0, 0, height);
    } else {
        sub_98199C_rotated(session, direction, imageIds[direction][0] | gTrackColours[SCHEME_TRACK], 0, 0, 32, 20, 1, height, 0, 0, height);
    }
    track_paint_util_draw_station_metal_supports_2(session, direction, height, gTrackColours[SCHEME_SUPPORTS], 3);
    track_paint_util_draw_station(session, rideIndex, trackSequence, direction, height, mapElement);
    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_6);
    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x0071BFB4 */
static void mine_train_rc_track_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    if (track_element_is_lift_hill(mapElement)) {
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20102, 0, 0, 32, 20, 1, height, 0, 6, height);
            wooden_a_supports_paint_setup(session, 6, 9, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20103, 0, 0, 32, 20, 1, height, 0, 6, height);
            wooden_a_supports_paint_setup(session, 7, 10, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20104, 0, 0, 32, 20, 1, height, 0, 6, height);
            wooden_a_supports_paint_setup(session, 6, 11, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20105, 0, 0, 32, 20, 1, height, 0, 6, height);
            wooden_a_supports_paint_setup(session, 7, 12, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
    } else {
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20074, 0, 0, 32, 20, 1, height, 0, 6, height);
            wooden_a_supports_paint_setup(session, 6, 9, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20075, 0, 0, 32, 20, 1, height, 0, 6, height);
            wooden_a_supports_paint_setup(session, 7, 10, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20076, 0, 0, 32, 20, 1, height, 0, 6, height);
            wooden_a_supports_paint_setup(session, 6, 11, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20077, 0, 0, 32, 20, 1, height, 0, 6, height);
            wooden_a_supports_paint_setup(session, 7, 12, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
    }
    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_7);
    } else {
        paint_util_push_tunnel_rotated(session, direction, height + 8, TUNNEL_8);
    }
    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 56, 0x20);
}

/** rct2: 0x0071BFC4 */
static void mine_train_rc_track_60_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20090, 0, 0, 32, 20, 1, height, 0, 6, height);
        wooden_a_supports_paint_setup(session, 6, 21, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    case 1:
        session->WoodenSupportsPrependTo = sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20091, 0, 0, 1, 32, 98, height, 27, 0, height);
        wooden_a_supports_paint_setup(session, 7, 22, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    case 2:
        session->WoodenSupportsPrependTo = sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20092, 0, 0, 1, 32, 98, height, 27, 0, height);
        wooden_a_supports_paint_setup(session, 6, 23, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    case 3:
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20093, 0, 0, 32, 20, 1, height, 0, 6, height);
        wooden_a_supports_paint_setup(session, 7, 24, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    }
    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_7);
    } else {
        paint_util_push_tunnel_rotated(session, direction, height + 56, TUNNEL_8);
    }
    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 104, 0x20);
}

/** rct2: 0x0071BFD4 */
static void mine_train_rc_track_flat_to_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    if (track_element_is_lift_hill(mapElement)) {
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20094, 0, 0, 32, 20, 1, height, 0, 6, height);
            wooden_a_supports_paint_setup(session, 6, 1, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20095, 0, 0, 32, 20, 1, height, 0, 6, height);
            wooden_a_supports_paint_setup(session, 7, 2, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20096, 0, 0, 32, 20, 1, height, 0, 6, height);
            wooden_a_supports_paint_setup(session, 6, 3, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20097, 0, 0, 32, 20, 1, height, 0, 6, height);
            wooden_a_supports_paint_setup(session, 7, 4, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
    } else {
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20066, 0, 0, 32, 20, 1, height, 0, 6, height);
            wooden_a_supports_paint_setup(session, 6, 1, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20067, 0, 0, 32, 20, 1, height, 0, 6, height);
            wooden_a_supports_paint_setup(session, 7, 2, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20068, 0, 0, 32, 20, 1, height, 0, 6, height);
            wooden_a_supports_paint_setup(session, 6, 3, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20069, 0, 0, 32, 20, 1, height, 0, 6, height);
            wooden_a_supports_paint_setup(session, 7, 4, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
    }
    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_6);
    } else {
        paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_8);
    }
    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 48, 0x20);
}

/** rct2: 0x0071BFE4 */
static void mine_train_rc_track_25_deg_up_to_60_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20078, 0, 0, 32, 20, 1, height, 0, 6, height);
        wooden_a_supports_paint_setup(session, 6, 13, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    case 1:
        session->WoodenSupportsPrependTo = sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20079, 0, 0, 32, 20, 1, height, 0, 6, height);
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20082, 0, 0, 32, 1, 66, height, 0, 27, height);
        wooden_a_supports_paint_setup(session, 7, 14, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    case 2:
        session->WoodenSupportsPrependTo = sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20080, 0, 0, 32, 20, 1, height, 0, 6, height);
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20083, 0, 0, 32, 1, 66, height, 0, 27, height);
        wooden_a_supports_paint_setup(session, 6, 15, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    case 3:
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20081, 0, 0, 32, 20, 1, height, 0, 6, height);
        wooden_a_supports_paint_setup(session, 7, 16, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    }
    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_7);
    } else {
        paint_util_push_tunnel_rotated(session, direction, height + 24, TUNNEL_8);
    }
    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 72, 0x20);
}

/** rct2: 0x0071BFF4 */
static void mine_train_rc_track_60_deg_up_to_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20084, 0, 0, 32, 20, 1, height, 0, 6, height);
        wooden_a_supports_paint_setup(session, 6, 17, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    case 1:
        session->WoodenSupportsPrependTo = sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20085, 0, 0, 24, 1, 61, height, 4, 29, height - 16);
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20088, 0, 0, 32, 2, 66, height, 0, 4, height);
        wooden_a_supports_paint_setup(session, 7, 18, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    case 2:
        session->WoodenSupportsPrependTo = sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20086, 0, 0, 24, 1, 61, height, 4, 29, height - 16);
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20089, 0, 0, 32, 2, 66, height, 0, 4, height);
        wooden_a_supports_paint_setup(session, 6, 19, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    case 3:
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20087, 0, 0, 32, 20, 1, height, 0, 6, height);
        wooden_a_supports_paint_setup(session, 7, 20, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    }
    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_7);
    } else {
        paint_util_push_tunnel_rotated(session, direction, height + 24, TUNNEL_8);
    }
    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 72, 0x20);
}

/** rct2: 0x0071C004 */
static void mine_train_rc_track_25_deg_up_to_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    if (track_element_is_lift_hill(mapElement)) {
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20098, 0, 0, 32, 20, 1, height, 0, 6, height);
            wooden_a_supports_paint_setup(session, 6, 5, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20099, 0, 0, 32, 20, 1, height, 0, 6, height);
            wooden_a_supports_paint_setup(session, 7, 6, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20100, 0, 0, 32, 20, 1, height, 0, 6, height);
            wooden_a_supports_paint_setup(session, 6, 7, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20101, 0, 0, 32, 20, 1, height, 0, 6, height);
            wooden_a_supports_paint_setup(session, 7, 8, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
    } else {
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20070, 0, 0, 32, 20, 1, height, 0, 6, height);
            wooden_a_supports_paint_setup(session, 6, 5, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20071, 0, 0, 32, 20, 1, height, 0, 6, height);
            wooden_a_supports_paint_setup(session, 7, 6, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20072, 0, 0, 32, 20, 1, height, 0, 6, height);
            wooden_a_supports_paint_setup(session, 6, 7, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20073, 0, 0, 32, 20, 1, height, 0, 6, height);
            wooden_a_supports_paint_setup(session, 7, 8, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
    }
    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_6);
    } else {
        paint_util_push_tunnel_rotated(session, direction, height + 8, TUNNEL_14);
    }
    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 40, 0x20);
}

/** rct2: 0x0071C014 */
static void mine_train_rc_track_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    mine_train_rc_track_25_deg_up(session, rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x0071C024 */
static void mine_train_rc_track_60_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    mine_train_rc_track_60_deg_up(session, rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x0071C034 */
static void mine_train_rc_track_flat_to_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    mine_train_rc_track_25_deg_up_to_flat(session, rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x0071C044 */
static void mine_train_rc_track_25_deg_down_to_60_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    mine_train_rc_track_60_deg_up_to_25_deg_up(session, rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x0071C054 */
static void mine_train_rc_track_60_deg_down_to_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    mine_train_rc_track_25_deg_up_to_60_deg_up(session, rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x0071C064 */
static void mine_train_rc_track_25_deg_down_to_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    mine_train_rc_track_flat_to_25_deg_up(session, rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x0071C0B4 */
static void mine_train_rc_track_left_quarter_turn_5(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20155, 0, 0, 32, 20, 1, height, 0, 6, height);
            wooden_a_supports_paint_setup(session, 6, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20160, 0, 0, 32, 20, 1, height, 0, 6, height);
            wooden_a_supports_paint_setup(session, 7, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20165, 0, 0, 32, 20, 1, height, 0, 6, height);
            wooden_a_supports_paint_setup(session, 6, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20150, 0, 0, 32, 20, 1, height, 0, 6, height);
            wooden_a_supports_paint_setup(session, 7, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_6);
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 1:
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20154, 0, 0, 32, 16, 1, height);
            wooden_a_supports_paint_setup(session, 11, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20159, 0, 0, 32, 16, 1, height);
            wooden_a_supports_paint_setup(session, 8, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20164, 0, 0, 32, 16, 1, height, 0, 16, height);
            wooden_a_supports_paint_setup(session, 9, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20149, 0, 0, 32, 16, 1, height, 0, 16, height);
            wooden_a_supports_paint_setup(session, 10, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20153, 0, 0, 16, 16, 1, height, 0, 16, height);
            wooden_a_supports_paint_setup(session, 9, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20158, 0, 0, 16, 16, 1, height, 16, 16, height);
            wooden_a_supports_paint_setup(session, 10, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20163, 0, 0, 16, 16, 1, height, 16, 0, height);
            wooden_a_supports_paint_setup(session, 11, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20148, 0, 0, 16, 16, 1, height);
            wooden_a_supports_paint_setup(session, 8, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 4:
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 5:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20152, 0, 0, 16, 32, 1, height, 16, 0, height);
            wooden_a_supports_paint_setup(session, 11, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20157, 0, 0, 16, 32, 1, height);
            wooden_a_supports_paint_setup(session, 8, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20162, 0, 0, 16, 32, 1, height);
            wooden_a_supports_paint_setup(session, 9, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20147, 0, 0, 16, 32, 1, height, 16, 0, height);
            wooden_a_supports_paint_setup(session, 10, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 6:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20151, 0, 0, 20, 32, 1, height, 6, 0, height);
            wooden_a_supports_paint_setup(session, 7, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20156, 0, 0, 20, 32, 1, height, 6, 0, height);
            wooden_a_supports_paint_setup(session, 6, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20161, 0, 0, 20, 32, 1, height, 6, 0, height);
            wooden_a_supports_paint_setup(session, 7, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20146, 0, 0, 20, 32, 1, height, 6, 0, height);
            wooden_a_supports_paint_setup(session, 6, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        switch (direction) {
        case 2:
            paint_util_push_tunnel_right(session, height, TUNNEL_6);
            break;
        case 3:
            paint_util_push_tunnel_left(session, height, TUNNEL_6);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    }
}

/** rct2: 0x0071C0C4 */
static void mine_train_rc_track_right_quarter_turn_5(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    mine_train_rc_track_left_quarter_turn_5(session, rideIndex, trackSequence, (direction - 1) & 3, height, mapElement);
}

/** rct2: 0x0071C074 */
static void mine_train_rc_track_flat_to_left_bank(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20106, 0, 0, 32, 20, 1, height, 0, 6, height);
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20114, 0, 0, 32, 1, 26, height, 0, 27, height);
        wooden_a_supports_paint_setup(session, 6, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    case 1:
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20107, 0, 0, 32, 20, 1, height, 0, 6, height);
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20115, 0, 0, 32, 1, 26, height, 0, 27, height);
        wooden_a_supports_paint_setup(session, 7, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    case 2:
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20108, 0, 0, 32, 20, 1, height, 0, 6, height);
        wooden_a_supports_paint_setup(session, 6, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    case 3:
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20109, 0, 0, 32, 20, 1, height, 0, 6, height);
        wooden_a_supports_paint_setup(session, 7, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    }
    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_6);
    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x0071C084 */
static void mine_train_rc_track_flat_to_right_bank(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20110, 0, 0, 32, 20, 1, height, 0, 6, height);
        wooden_a_supports_paint_setup(session, 6, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    case 1:
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20111, 0, 0, 32, 20, 1, height, 0, 6, height);
        wooden_a_supports_paint_setup(session, 7, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    case 2:
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20112, 0, 0, 32, 20, 1, height, 0, 6, height);
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20116, 0, 0, 32, 1, 26, height, 0, 27, height);
        wooden_a_supports_paint_setup(session, 6, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    case 3:
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20113, 0, 0, 32, 20, 1, height, 0, 6, height);
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20117, 0, 0, 32, 1, 26, height, 0, 27, height);
        wooden_a_supports_paint_setup(session, 7, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    }
    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_6);
    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x0071C094 */
static void mine_train_rc_track_left_bank_to_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20112, 0, 0, 32, 20, 1, height, 0, 6, height);
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20116, 0, 0, 32, 1, 26, height, 0, 27, height);
        wooden_a_supports_paint_setup(session, 6, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    case 1:
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20113, 0, 0, 32, 20, 1, height, 0, 6, height);
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20117, 0, 0, 32, 1, 26, height, 0, 27, height);
        wooden_a_supports_paint_setup(session, 7, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    case 2:
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20110, 0, 0, 32, 20, 1, height, 0, 6, height);
        wooden_a_supports_paint_setup(session, 6, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    case 3:
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20111, 0, 0, 32, 20, 1, height, 0, 6, height);
        wooden_a_supports_paint_setup(session, 7, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    }
    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_6);
    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x0071C0A4 */
static void mine_train_rc_track_right_bank_to_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20108, 0, 0, 32, 20, 1, height, 0, 6, height);
        wooden_a_supports_paint_setup(session, 6, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    case 1:
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20109, 0, 0, 32, 20, 1, height, 0, 6, height);
        wooden_a_supports_paint_setup(session, 7, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    case 2:
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20106, 0, 0, 32, 20, 1, height, 0, 6, height);
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20114, 0, 0, 32, 1, 26, height, 0, 27, height);
        wooden_a_supports_paint_setup(session, 6, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    case 3:
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20107, 0, 0, 32, 20, 1, height, 0, 6, height);
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20115, 0, 0, 32, 1, 26, height, 0, 27, height);
        wooden_a_supports_paint_setup(session, 7, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    }
    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_6);
    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x0071C0D4 */
static void mine_train_rc_track_banked_left_quarter_turn_5(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20175, 0, 0, 32, 20, 1, height, 0, 6, height);
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20186, 0, 0, 32, 1, 26, height, 0, 27, height);
            wooden_a_supports_paint_setup(session, 6, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20180, 0, 0, 32, 1, 26, height, 0, 27, height);
            wooden_a_supports_paint_setup(session, 7, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20185, 0, 0, 32, 20, 1, height, 0, 6, height);
            wooden_a_supports_paint_setup(session, 6, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20170, 0, 0, 32, 20, 1, height, 0, 6, height);
            wooden_a_supports_paint_setup(session, 7, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_6);
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 1:
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20174, 0, 0, 32, 16, 1, height);
            wooden_a_supports_paint_setup(session, 11, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20179, 0, 0, 32, 16, 1, height, 0, 0, height + 27);
            wooden_a_supports_paint_setup(session, 8, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20184, 0, 0, 32, 16, 1, height, 0, 16, height);
            wooden_a_supports_paint_setup(session, 9, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20169, 0, 0, 32, 16, 1, height, 0, 16, height);
            wooden_a_supports_paint_setup(session, 10, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20173, 0, 0, 16, 16, 1, height, 0, 16, height);
            wooden_a_supports_paint_setup(session, 9, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20178, 0, 0, 16, 16, 1, height, 16, 16, height + 27);
            wooden_a_supports_paint_setup(session, 10, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20183, 0, 0, 16, 16, 1, height, 16, 0, height);
            wooden_a_supports_paint_setup(session, 11, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20168, 0, 0, 16, 16, 1, height);
            wooden_a_supports_paint_setup(session, 8, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 4:
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 5:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20172, 0, 0, 16, 32, 1, height, 16, 0, height);
            wooden_a_supports_paint_setup(session, 11, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20177, 0, 0, 16, 32, 1, height, 0, 0, height + 27);
            wooden_a_supports_paint_setup(session, 8, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20182, 0, 0, 16, 32, 1, height);
            wooden_a_supports_paint_setup(session, 9, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20167, 0, 0, 16, 32, 1, height, 16, 0, height);
            wooden_a_supports_paint_setup(session, 10, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 6:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20171, 0, 0, 20, 32, 1, height, 6, 0, height);
            wooden_a_supports_paint_setup(session, 7, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20176, 0, 0, 1, 32, 26, height, 27, 0, height);
            wooden_a_supports_paint_setup(session, 6, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20181, 0, 0, 20, 32, 1, height, 6, 0, height);
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20187, 0, 0, 1, 32, 26, height, 27, 0, height);
            wooden_a_supports_paint_setup(session, 7, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20166, 0, 0, 20, 32, 1, height, 6, 0, height);
            wooden_a_supports_paint_setup(session, 6, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        switch (direction) {
        case 2:
            paint_util_push_tunnel_right(session, height, TUNNEL_6);
            break;
        case 3:
            paint_util_push_tunnel_left(session, height, TUNNEL_6);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    }
}

/** rct2: 0x0071C0E4 */
static void mine_train_rc_track_banked_right_quarter_turn_5(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    mine_train_rc_track_banked_left_quarter_turn_5(session, rideIndex, trackSequence, (direction - 1) & 3, height, mapElement);
}

/** rct2: 0x0071C204 */
static void mine_train_rc_track_left_bank_to_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20118, 0, 0, 32, 20, 1, height, 0, 6, height);
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20122, 0, 0, 32, 1, 34, height, 0, 27, height);
        wooden_a_supports_paint_setup(session, 6, 1, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    case 1:
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20119, 0, 0, 32, 20, 1, height, 0, 6, height);
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20123, 0, 0, 32, 1, 34, height, 0, 27, height);
        wooden_a_supports_paint_setup(session, 7, 2, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    case 2:
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20120, 0, 0, 32, 20, 1, height, 0, 6, height);
        wooden_a_supports_paint_setup(session, 6, 3, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    case 3:
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20121, 0, 0, 32, 20, 1, height, 0, 6, height);
        wooden_a_supports_paint_setup(session, 7, 4, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    }
    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_6);
    } else {
        paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_8);
    }
    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 48, 0x20);
}

/** rct2: 0x0071C214 */
static void mine_train_rc_track_right_bank_to_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20124, 0, 0, 32, 20, 1, height, 0, 6, height);
        wooden_a_supports_paint_setup(session, 6, 1, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    case 1:
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20125, 0, 0, 32, 20, 1, height, 0, 6, height);
        wooden_a_supports_paint_setup(session, 7, 2, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    case 2:
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20126, 0, 0, 32, 20, 1, height, 0, 6, height);
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20128, 0, 0, 32, 1, 34, height, 0, 27, height);
        wooden_a_supports_paint_setup(session, 6, 3, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    case 3:
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20127, 0, 0, 32, 20, 1, height, 0, 6, height);
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20129, 0, 0, 32, 1, 34, height, 0, 27, height);
        wooden_a_supports_paint_setup(session, 7, 4, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    }
    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_6);
    } else {
        paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_8);
    }
    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 48, 0x20);
}

/** rct2: 0x0071C224 */
static void mine_train_rc_track_25_deg_up_to_left_bank(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20130, 0, 0, 32, 20, 1, height, 0, 6, height);
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20134, 0, 0, 32, 1, 34, height, 0, 27, height);
        wooden_a_supports_paint_setup(session, 6, 5, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    case 1:
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20131, 0, 0, 32, 20, 1, height, 0, 6, height);
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20135, 0, 0, 32, 1, 34, height, 0, 27, height);
        wooden_a_supports_paint_setup(session, 7, 6, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    case 2:
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20132, 0, 0, 32, 20, 1, height, 0, 6, height);
        wooden_a_supports_paint_setup(session, 6, 7, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    case 3:
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20133, 0, 0, 32, 20, 1, height, 0, 6, height);
        wooden_a_supports_paint_setup(session, 7, 8, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    }
    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_6);
    } else {
        paint_util_push_tunnel_rotated(session, direction, height + 8, TUNNEL_14);
    }
    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 40, 0x20);
}

/** rct2: 0x0071C234 */
static void mine_train_rc_track_25_deg_up_to_right_bank(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20136, 0, 0, 32, 20, 1, height, 0, 6, height);
        wooden_a_supports_paint_setup(session, 6, 5, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    case 1:
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20137, 0, 0, 32, 20, 1, height, 0, 6, height);
        wooden_a_supports_paint_setup(session, 7, 6, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    case 2:
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20138, 0, 0, 32, 20, 1, height, 0, 6, height);
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20140, 0, 0, 32, 1, 34, height, 0, 27, height);
        wooden_a_supports_paint_setup(session, 6, 7, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    case 3:
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20139, 0, 0, 32, 20, 1, height, 0, 6, height);
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20141, 0, 0, 32, 1, 34, height, 0, 27, height);
        wooden_a_supports_paint_setup(session, 7, 8, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    }
    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_6);
    } else {
        paint_util_push_tunnel_rotated(session, direction, height + 8, TUNNEL_14);
    }
    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 40, 0x20);
}

/** rct2: 0x0071C244 */
static void mine_train_rc_track_left_bank_to_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    mine_train_rc_track_25_deg_up_to_right_bank(session, rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x0071C254 */
static void mine_train_rc_track_right_bank_to_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    mine_train_rc_track_25_deg_up_to_left_bank(session, rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x0071C264 */
static void mine_train_rc_track_25_deg_down_to_left_bank(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    mine_train_rc_track_right_bank_to_25_deg_up(session, rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x0071C274 */
static void mine_train_rc_track_25_deg_down_to_right_bank(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    mine_train_rc_track_left_bank_to_25_deg_up(session, rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x0071C304 */
static void mine_train_rc_track_left_bank(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20142, 0, 0, 32, 1, 26, height, 0, 27, height);
        wooden_a_supports_paint_setup(session, 6, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    case 1:
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20143, 0, 0, 32, 1, 26, height, 0, 27, height);
        wooden_a_supports_paint_setup(session, 7, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    case 2:
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20144, 0, 0, 32, 20, 1, height, 0, 6, height);
        wooden_a_supports_paint_setup(session, 6, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    case 3:
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20145, 0, 0, 32, 20, 1, height, 0, 6, height);
        wooden_a_supports_paint_setup(session, 7, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    }
    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_6);
    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x0071C314 */
static void mine_train_rc_track_right_bank(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    mine_train_rc_track_left_bank(session, rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x0071C0F4 */
static void mine_train_rc_track_left_quarter_turn_5_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20250, 0, 0, 32, 27, 1, height, 0, 2, height);
            wooden_a_supports_paint_setup(session, 6, 9, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20255, 0, 0, 32, 27, 1, height, 0, 2, height);
            wooden_a_supports_paint_setup(session, 7, 10, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20260, 0, 0, 32, 27, 1, height, 0, 2, height);
            wooden_a_supports_paint_setup(session, 6, 11, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20265, 0, 0, 32, 27, 1, height, 0, 2, height);
            wooden_a_supports_paint_setup(session, 7, 12, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_7);
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 72, 0x20);
        break;
    case 1:
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 72, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20251, 0, 0, 32, 16, 1, height, 0, 0, height);
            wooden_a_supports_paint_setup(session, 11, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20256, 0, 0, 32, 16, 1, height, 0, 0, height);
            wooden_a_supports_paint_setup(session, 8, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20261, 0, 0, 32, 16, 1, height, 0, 16, height);
            wooden_a_supports_paint_setup(session, 9, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20266, 0, 0, 32, 16, 1, height, 0, 16, height);
            wooden_a_supports_paint_setup(session, 10, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 72, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20252, 0, 0, 16, 16, 1, height, 0, 16, height);
            wooden_a_supports_paint_setup(session, 9, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20257, 0, 0, 16, 16, 1, height, 16, 16, height);
            wooden_a_supports_paint_setup(session, 10, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20262, 0, 0, 16, 16, 1, height, 16, 0, height);
            wooden_a_supports_paint_setup(session, 11, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20267, 0, 0, 16, 16, 1, height, 0, 0, height);
            wooden_a_supports_paint_setup(session, 8, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 64, 0x20);
        break;
    case 4:
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 72, 0x20);
        break;
    case 5:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20253, 0, 0, 16, 32, 1, height, 16, 0, height);
            wooden_a_supports_paint_setup(session, 11, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20258, 0, 0, 16, 32, 1, height, 0, 0, height);
            wooden_a_supports_paint_setup(session, 8, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20263, 0, 0, 16, 32, 1, height, 0, 0, height);
            wooden_a_supports_paint_setup(session, 9, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20268, 0, 0, 16, 32, 1, height, 16, 0, height);
            wooden_a_supports_paint_setup(session, 10, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 72, 0x20);
        break;
    case 6:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20254, 0, 0, 27, 32, 1, height, 2, 0, height);
            wooden_a_supports_paint_setup(session, 7, 12, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20259, 0, 0, 27, 32, 1, height, 2, 0, height);
            wooden_a_supports_paint_setup(session, 6, 9, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20264, 0, 0, 27, 32, 1, height, 2, 0, height);
            wooden_a_supports_paint_setup(session, 7, 10, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20269, 0, 0, 27, 32, 1, height, 2, 0, height);
            wooden_a_supports_paint_setup(session, 6, 11, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        switch (direction) {
        case 2:
            paint_util_push_tunnel_right(session, height + 8, TUNNEL_8);
            break;
        case 3:
            paint_util_push_tunnel_left(session, height + 8, TUNNEL_8);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 72, 0x20);
        break;
    }
}

/** rct2: 0x0071C104 */
static void mine_train_rc_track_right_quarter_turn_5_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20230, 0, 0, 32, 27, 1, height, 0, 2, height);
            wooden_a_supports_paint_setup(session, 6, 9, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20235, 0, 0, 32, 27, 1, height, 0, 2, height);
            wooden_a_supports_paint_setup(session, 7, 10, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20240, 0, 0, 32, 27, 1, height, 0, 2, height);
            wooden_a_supports_paint_setup(session, 6, 11, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20245, 0, 0, 32, 27, 1, height, 0, 2, height);
            wooden_a_supports_paint_setup(session, 7, 12, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_7);
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 72, 0x20);
        break;
    case 1:
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 72, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20231, 0, 0, 32, 16, 1, height, 0, 16, height);
            wooden_a_supports_paint_setup(session, 10, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20236, 0, 0, 32, 16, 1, height, 0, 16, height);
            wooden_a_supports_paint_setup(session, 11, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20241, 0, 0, 32, 16, 1, height, 0, 0, height);
            wooden_a_supports_paint_setup(session, 8, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20246, 0, 0, 32, 16, 1, height, 0, 0, height);
            wooden_a_supports_paint_setup(session, 9, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 72, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20232, 0, 0, 16, 16, 1, height, 0, 0, height);
            wooden_a_supports_paint_setup(session, 8, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20237, 0, 0, 16, 16, 1, height, 16, 0, height);
            wooden_a_supports_paint_setup(session, 9, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20242, 0, 0, 16, 16, 1, height, 16, 16, height);
            wooden_a_supports_paint_setup(session, 10, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20247, 0, 0, 16, 16, 1, height, 0, 16, height);
            wooden_a_supports_paint_setup(session, 11, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_BC | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 64, 0x20);
        break;
    case 4:
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 72, 0x20);
        break;
    case 5:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20233, 0, 0, 16, 32, 1, height, 16, 0, height);
            wooden_a_supports_paint_setup(session, 10, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20238, 0, 0, 16, 32, 1, height, 0, 0, height);
            wooden_a_supports_paint_setup(session, 11, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20243, 0, 0, 16, 32, 1, height, 0, 0, height);
            wooden_a_supports_paint_setup(session, 8, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20248, 0, 0, 16, 32, 1, height, 16, 0, height);
            wooden_a_supports_paint_setup(session, 9, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 72, 0x20);
        break;
    case 6:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20234, 0, 0, 27, 32, 1, height, 2, 0, height);
            wooden_a_supports_paint_setup(session, 7, 10, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20239, 0, 0, 27, 32, 1, height, 2, 0, height);
            wooden_a_supports_paint_setup(session, 6, 11, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20244, 0, 0, 27, 32, 1, height, 2, 0, height);
            wooden_a_supports_paint_setup(session, 7, 12, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20249, 0, 0, 27, 32, 1, height, 2, 0, height);
            wooden_a_supports_paint_setup(session, 6, 9, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        switch (direction) {
        case 0:
            paint_util_push_tunnel_right(session, height + 8, TUNNEL_8);
            break;
        case 1:
            paint_util_push_tunnel_left(session, height + 8, TUNNEL_8);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 72, 0x20);
        break;
    }
}

/** rct2: 0x0071C114 */
static void mine_train_rc_track_left_quarter_turn_5_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    mine_train_rc_track_right_quarter_turn_5_25_deg_up(session, rideIndex, trackSequence, (direction + 1) & 3, height, mapElement);
}

/** rct2: 0x0071C124 */
static void mine_train_rc_track_right_quarter_turn_5_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    mine_train_rc_track_left_quarter_turn_5_25_deg_up(session, rideIndex, trackSequence, (direction - 1) & 3, height, mapElement);
}

/** rct2: 0x0071C134 */
static void mine_train_rc_track_s_bend_left(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20270, 0, 0, 32, 20, 1, height, 0, 6, height);
            wooden_a_supports_paint_setup(session, 6, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20274, 0, 0, 32, 20, 1, height, 0, 6, height);
            wooden_a_supports_paint_setup(session, 7, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20273, 0, 0, 32, 20, 1, height, 0, 6, height);
            wooden_a_supports_paint_setup(session, 6, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20277, 0, 0, 32, 20, 1, height, 0, 6, height);
            wooden_a_supports_paint_setup(session, 7, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_6);
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20271, 0, 0, 32, 26, 1, height);
            wooden_a_supports_paint_setup(session, 11, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20275, 0, 0, 32, 26, 1, height);
            wooden_a_supports_paint_setup(session, 8, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20272, 0, 0, 32, 26, 1, height, 0, 6, height);
            wooden_a_supports_paint_setup(session, 9, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20276, 0, 0, 32, 26, 1, height, 0, 6, height);
            wooden_a_supports_paint_setup(session, 10, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20272, 0, 0, 32, 26, 1, height, 0, 6, height);
            wooden_a_supports_paint_setup(session, 9, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20276, 0, 0, 32, 26, 1, height, 0, 6, height);
            wooden_a_supports_paint_setup(session, 10, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20271, 0, 0, 32, 26, 1, height);
            wooden_a_supports_paint_setup(session, 11, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20275, 0, 0, 32, 26, 1, height);
            wooden_a_supports_paint_setup(session, 8, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20273, 0, 0, 32, 20, 1, height, 0, 6, height);
            wooden_a_supports_paint_setup(session, 6, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20277, 0, 0, 32, 20, 1, height, 0, 6, height);
            wooden_a_supports_paint_setup(session, 7, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20270, 0, 0, 32, 20, 1, height, 0, 6, height);
            wooden_a_supports_paint_setup(session, 6, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20274, 0, 0, 32, 20, 1, height, 0, 6, height);
            wooden_a_supports_paint_setup(session, 7, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        switch (direction) {
        case 1:
            paint_util_push_tunnel_right(session, height, TUNNEL_6);
            break;
        case 2:
            paint_util_push_tunnel_left(session, height, TUNNEL_6);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    }
}

/** rct2: 0x0071C144 */
static void mine_train_rc_track_s_bend_right(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20278, 0, 0, 32, 20, 1, height, 0, 6, height);
            wooden_a_supports_paint_setup(session, 6, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20282, 0, 0, 32, 20, 1, height, 0, 6, height);
            wooden_a_supports_paint_setup(session, 7, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20281, 0, 0, 32, 20, 1, height, 0, 6, height);
            wooden_a_supports_paint_setup(session, 6, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20285, 0, 0, 32, 20, 1, height, 0, 6, height);
            wooden_a_supports_paint_setup(session, 7, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_6);
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20279, 0, 0, 32, 26, 1, height, 0, 6, height);
            wooden_a_supports_paint_setup(session, 10, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20283, 0, 0, 32, 26, 1, height, 0, 6, height);
            wooden_a_supports_paint_setup(session, 11, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20280, 0, 0, 32, 26, 1, height);
            wooden_a_supports_paint_setup(session, 8, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20284, 0, 0, 32, 26, 1, height);
            wooden_a_supports_paint_setup(session, 9, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20280, 0, 0, 32, 26, 1, height);
            wooden_a_supports_paint_setup(session, 8, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20284, 0, 0, 32, 26, 1, height);
            wooden_a_supports_paint_setup(session, 9, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20279, 0, 0, 32, 26, 1, height, 0, 6, height);
            wooden_a_supports_paint_setup(session, 10, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20283, 0, 0, 32, 26, 1, height, 0, 6, height);
            wooden_a_supports_paint_setup(session, 11, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20281, 0, 0, 32, 20, 1, height, 0, 6, height);
            wooden_a_supports_paint_setup(session, 6, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20285, 0, 0, 32, 20, 1, height, 0, 6, height);
            wooden_a_supports_paint_setup(session, 7, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20278, 0, 0, 32, 20, 1, height, 0, 6, height);
            wooden_a_supports_paint_setup(session, 6, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20282, 0, 0, 32, 20, 1, height, 0, 6, height);
            wooden_a_supports_paint_setup(session, 7, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        switch (direction) {
        case 1:
            paint_util_push_tunnel_right(session, height, TUNNEL_6);
            break;
        case 2:
            paint_util_push_tunnel_left(session, height, TUNNEL_6);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    }
}

/** rct2: 0x0071C184 */
static void mine_train_rc_track_left_quarter_turn_3(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20193, 0, 0, 32, 20, 1, height, 0, 6, height);
            wooden_a_supports_paint_setup(session, 11, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20196, 0, 0, 32, 20, 1, height, 0, 6, height);
            wooden_a_supports_paint_setup(session, 8, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20199, 0, 0, 32, 20, 1, height, 0, 6, height);
            wooden_a_supports_paint_setup(session, 9, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20190, 0, 0, 32, 20, 1, height, 0, 6, height);
            wooden_a_supports_paint_setup(session, 10, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_6);
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            wooden_a_supports_paint_setup(session, 9, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            wooden_a_supports_paint_setup(session, 10, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            wooden_a_supports_paint_setup(session, 11, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            wooden_a_supports_paint_setup(session, 8, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20192, 0, 0, 16, 16, 1, height, 16, 0, height);
            break;
        case 1:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20195, 0, 0, 16, 16, 1, height);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20198, 0, 0, 16, 16, 1, height, 0, 16, height);
            break;
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20189, 0, 0, 16, 16, 1, height, 16, 16, height);
            break;
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20191, 0, 0, 20, 32, 1, height, 6, 0, height);
            wooden_a_supports_paint_setup(session, 11, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20194, 0, 0, 20, 32, 1, height, 6, 0, height);
            wooden_a_supports_paint_setup(session, 8, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20197, 0, 0, 20, 32, 1, height, 6, 0, height);
            wooden_a_supports_paint_setup(session, 9, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20188, 0, 0, 20, 32, 1, height, 6, 0, height);
            wooden_a_supports_paint_setup(session, 10, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        switch (direction) {
        case 2:
            paint_util_push_tunnel_right(session, height, TUNNEL_6);
            break;
        case 3:
            paint_util_push_tunnel_left(session, height, TUNNEL_6);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    }
}

/** rct2: 0x0071C194 */
static void mine_train_rc_track_right_quarter_turn_3(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    mine_train_rc_track_left_quarter_turn_3(session, rideIndex, trackSequence, (direction - 1) & 3, height, mapElement);
}

/** rct2: 0x0071C1A4 */
static void mine_train_rc_track_left_quarter_turn_3_bank(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20205, 0, 0, 32, 20, 1, height, 0, 6, height);
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20212, 0, 0, 32, 1, 26, height, 0, 27, height);
            wooden_a_supports_paint_setup(session, 11, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20208, 0, 0, 32, 1, 26, height, 0, 27, height);
            wooden_a_supports_paint_setup(session, 8, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20211, 0, 0, 32, 20, 1, height, 0, 6, height);
            wooden_a_supports_paint_setup(session, 9, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20202, 0, 0, 32, 20, 1, height, 0, 6, height);
            wooden_a_supports_paint_setup(session, 10, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_6);
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            wooden_a_supports_paint_setup(session, 9, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            wooden_a_supports_paint_setup(session, 10, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            wooden_a_supports_paint_setup(session, 11, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            wooden_a_supports_paint_setup(session, 8, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20204, 0, 0, 16, 16, 1, height, 16, 0, height);
            break;
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20207, 0, 0, 16, 16, 1, height, 0, 0, height + 27);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20210, 0, 0, 16, 16, 1, height, 0, 16, height);
            break;
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20201, 0, 0, 16, 16, 1, height, 16, 16, height);
            break;
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20203, 0, 0, 20, 32, 1, height, 6, 0, height);
            wooden_a_supports_paint_setup(session, 11, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20206, 0, 0, 1, 32, 26, height, 27, 0, height);
            wooden_a_supports_paint_setup(session, 8, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20209, 0, 0, 20, 32, 1, height, 6, 0, height);
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20213, 0, 0, 1, 32, 26, height, 27, 0, height);
            wooden_a_supports_paint_setup(session, 9, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20200, 0, 0, 20, 32, 1, height, 6, 0, height);
            wooden_a_supports_paint_setup(session, 10, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        switch (direction) {
        case 2:
            paint_util_push_tunnel_right(session, height, TUNNEL_6);
            break;
        case 3:
            paint_util_push_tunnel_left(session, height, TUNNEL_6);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    }
}

/** rct2: 0x0071C1B4 */
static void mine_train_rc_track_right_quarter_turn_3_bank(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    mine_train_rc_track_left_quarter_turn_3_bank(session, rideIndex, trackSequence, (direction - 1) & 3, height, mapElement);
}

/** rct2: 0x0071C1C4 */
static void mine_train_rc_track_left_quarter_turn_3_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20225, 0, 6, 32, 20, 1, height);
            wooden_a_supports_paint_setup(session, 11, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20227, 0, 6, 32, 20, 1, height);
            wooden_a_supports_paint_setup(session, 8, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20229, 0, 6, 32, 20, 1, height);
            wooden_a_supports_paint_setup(session, 3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20223, 0, 6, 32, 20, 1, height);
            wooden_a_supports_paint_setup(session, 10, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_7);
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 72, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            wooden_a_supports_paint_setup(session, 9, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            wooden_a_supports_paint_setup(session, 10, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            wooden_a_supports_paint_setup(session, 11, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            wooden_a_supports_paint_setup(session, 8, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_general_support_height(session, height + 56, 0x20);
        break;
    case 2:
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 56, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20224, 6, 0, 20, 32, 1, height);
            wooden_a_supports_paint_setup(session, 11, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20226, 6, 0, 20, 32, 1, height);
            wooden_a_supports_paint_setup(session, 8, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20228, 6, 0, 20, 32, 1, height);
            wooden_a_supports_paint_setup(session, 9, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20222, 6, 0, 20, 32, 1, height);
            wooden_a_supports_paint_setup(session, 10, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        switch (direction) {
        case 2:
            paint_util_push_tunnel_right(session, height + 8, TUNNEL_8);
            break;
        case 3:
            paint_util_push_tunnel_left(session, height + 8, TUNNEL_8);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 72, 0x20);
        break;
    }
}

/** rct2: 0x0071C1D4 */
static void mine_train_rc_track_right_quarter_turn_3_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20214, 0, 6, 32, 20, 1, height);
            wooden_a_supports_paint_setup(session, 10, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20216, 0, 6, 32, 20, 1, height);
            wooden_a_supports_paint_setup(session, 11, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20218, 0, 6, 32, 20, 1, height);
            wooden_a_supports_paint_setup(session, 8, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20220, 0, 6, 32, 20, 1, height);
            wooden_a_supports_paint_setup(session, 9, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_7);
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 72, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            wooden_a_supports_paint_setup(session, 8, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            wooden_a_supports_paint_setup(session, 9, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            wooden_a_supports_paint_setup(session, 10, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            wooden_a_supports_paint_setup(session, 11, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_general_support_height(session, height + 56, 0x20);
        break;
    case 2:
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 56, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20215, 6, 0, 20, 32, 1, height);
            wooden_a_supports_paint_setup(session, 10, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20217, 6, 0, 20, 32, 1, height);
            wooden_a_supports_paint_setup(session, 11, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20219, 6, 0, 20, 32, 1, height);
            wooden_a_supports_paint_setup(session, 8, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20221, 6, 0, 20, 32, 1, height);
            wooden_a_supports_paint_setup(session, 9, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        switch (direction) {
        case 0:
            paint_util_push_tunnel_right(session, height + 8, TUNNEL_8);
            break;
        case 1:
            paint_util_push_tunnel_left(session, height + 8, TUNNEL_8);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 72, 0x20);
        break;
    }
}

/** rct2: 0x0071C1E4 */
static void mine_train_rc_track_left_quarter_turn_3_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    mine_train_rc_track_right_quarter_turn_3_25_deg_up(session, rideIndex, trackSequence, (direction + 1) & 3, height, mapElement);
}

/** rct2: 0x0071C1F4 */
static void mine_train_rc_track_right_quarter_turn_3_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    mine_train_rc_track_left_quarter_turn_3_25_deg_up(session, rideIndex, trackSequence, (direction - 1) & 3, height, mapElement);
}

/** rct2: 0x0071C284 */
static void mine_train_rc_track_left_half_banked_helix_up_small(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20305, 0, 0, 32, 20, 1, height, 0, 6, height);
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20312, 0, 0, 32, 1, 26, height, 0, 27, height);
            wooden_a_supports_paint_setup(session, 11, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20308, 0, 0, 32, 1, 26, height, 0, 27, height);
            wooden_a_supports_paint_setup(session, 8, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20311, 0, 0, 32, 20, 1, height, 0, 6, height);
            wooden_a_supports_paint_setup(session, 9, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20302, 0, 0, 32, 20, 1, height, 0, 6, height);
            wooden_a_supports_paint_setup(session, 10, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_6);
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_BC | SEGMENT_C0 | SEGMENT_D4, direction), 48, 0x20);
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            wooden_a_supports_paint_setup(session, 9, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            wooden_a_supports_paint_setup(session, 10, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            wooden_a_supports_paint_setup(session, 11, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            wooden_a_supports_paint_setup(session, 8, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20304, 0, 0, 16, 16, 1, height, 16, 0, height);
            break;
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20307, 0, 0, 16, 16, 1, height, 0, 0, height + 27);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20310, 0, 0, 16, 16, 1, height, 0, 16, height);
            break;
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20301, 0, 0, 16, 16, 1, height, 16, 16, height);
            break;
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20303, 0, 0, 20, 32, 1, height, 6, 0, height);
            wooden_a_supports_paint_setup(session, 11, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20306, 0, 0, 1, 32, 26, height, 27, 0, height);
            wooden_a_supports_paint_setup(session, 8, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20309, 0, 0, 20, 32, 1, height, 6, 0, height);
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20313, 0, 0, 1, 32, 26, height, 27, 0, height);
            wooden_a_supports_paint_setup(session, 9, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20300, 0, 0, 20, 32, 1, height, 6, 0, height);
            wooden_a_supports_paint_setup(session, 10, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        switch (direction) {
        case 2:
            paint_util_push_tunnel_right(session, height + 8, TUNNEL_6);
            break;
        case 3:
            paint_util_push_tunnel_left(session, height + 8, TUNNEL_6);
            break;
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_BC | SEGMENT_CC, direction), 48, 0x20);
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 4:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20302, 0, 0, 20, 32, 1, height, 6, 0, height);
            wooden_a_supports_paint_setup(session, 10, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20305, 0, 0, 20, 32, 1, height, 6, 0, height);
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20312, 0, 0, 1, 32, 26, height, 27, 0, height);
            wooden_a_supports_paint_setup(session, 11, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20308, 0, 0, 1, 32, 26, height, 27, 0, height);
            wooden_a_supports_paint_setup(session, 8, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20311, 0, 0, 20, 32, 1, height, 6, 0, height);
            wooden_a_supports_paint_setup(session, 9, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        switch (direction) {
        case 0:
            paint_util_push_tunnel_right(session, height, TUNNEL_6);
            break;
        case 1:
            paint_util_push_tunnel_left(session, height, TUNNEL_6);
            break;
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_BC | SEGMENT_C0 | SEGMENT_CC, direction), 48, 0x20);
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 5:
        switch (direction) {
        case 0:
            wooden_a_supports_paint_setup(session, 8, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            wooden_a_supports_paint_setup(session, 9, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            wooden_a_supports_paint_setup(session, 10, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            wooden_a_supports_paint_setup(session, 11, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 6:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20301, 0, 0, 16, 16, 1, height, 16, 16, height);
            break;
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20304, 0, 0, 16, 16, 1, height, 0, 16, height);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20307, 0, 0, 16, 16, 1, height, 0, 0, height + 27);
            break;
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20310, 0, 0, 16, 16, 1, height, 16, 0, height);
            break;
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 7:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20300, 0, 0, 32, 20, 1, height, 0, 6, height);
            wooden_a_supports_paint_setup(session, 10, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20303, 0, 0, 32, 20, 1, height, 0, 6, height);
            wooden_a_supports_paint_setup(session, 11, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20306, 0, 0, 32, 1, 26, height, 0, 27, height);
            wooden_a_supports_paint_setup(session, 8, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20309, 0, 0, 32, 20, 1, height, 0, 6, height);
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20313, 0, 0, 32, 1, 26, height, 0, 27, height);
            wooden_a_supports_paint_setup(session, 9, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(session, direction, height + 8, TUNNEL_6);
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C8, direction), 48, 0x20);
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    }
}

/** rct2: 0x0071C294 */
static void mine_train_rc_track_right_half_banked_helix_up_small(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20286, 0, 0, 32, 20, 1, height, 0, 6, height);
            wooden_a_supports_paint_setup(session, 10, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20289, 0, 0, 32, 20, 1, height, 0, 6, height);
            wooden_a_supports_paint_setup(session, 11, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20292, 0, 0, 32, 1, 26, height, 0, 27, height);
            wooden_a_supports_paint_setup(session, 8, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20295, 0, 0, 32, 20, 1, height, 0, 6, height);
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20299, 0, 0, 32, 1, 26, height, 0, 27, height);
            wooden_a_supports_paint_setup(session, 9, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_6);
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C8, direction), 48, 0x20);
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            wooden_a_supports_paint_setup(session, 8, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            wooden_a_supports_paint_setup(session, 9, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            wooden_a_supports_paint_setup(session, 10, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            wooden_a_supports_paint_setup(session, 11, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20287, 0, 0, 16, 16, 1, height, 16, 16, height);
            break;
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20290, 0, 0, 16, 16, 1, height, 0, 16, height);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20293, 0, 0, 16, 16, 1, height, 0, 0, height + 27);
            break;
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20296, 0, 0, 16, 16, 1, height, 16, 0, height);
            break;
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20288, 0, 0, 20, 32, 1, height, 6, 0, height);
            wooden_a_supports_paint_setup(session, 10, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20291, 0, 0, 20, 32, 1, height, 6, 0, height);
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20298, 0, 0, 1, 32, 26, height, 27, 0, height);
            wooden_a_supports_paint_setup(session, 11, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20294, 0, 0, 1, 32, 26, height, 27, 0, height);
            wooden_a_supports_paint_setup(session, 8, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20297, 0, 0, 20, 32, 1, height, 6, 0, height);
            wooden_a_supports_paint_setup(session, 9, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        switch (direction) {
        case 0:
            paint_util_push_tunnel_right(session, height + 8, TUNNEL_6);
            break;
        case 1:
            paint_util_push_tunnel_left(session, height + 8, TUNNEL_6);
            break;
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_BC | SEGMENT_C0 | SEGMENT_CC, direction), 48, 0x20);
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 4:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20289, 0, 0, 20, 32, 1, height, 6, 0, height);
            wooden_a_supports_paint_setup(session, 11, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20292, 0, 0, 1, 32, 26, height, 27, 0, height);
            wooden_a_supports_paint_setup(session, 8, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20295, 0, 0, 20, 32, 1, height, 6, 0, height);
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20299, 0, 0, 1, 32, 26, height, 27, 0, height);
            wooden_a_supports_paint_setup(session, 9, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20286, 0, 0, 20, 32, 1, height, 6, 0, height);
            wooden_a_supports_paint_setup(session, 10, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        switch (direction) {
        case 2:
            paint_util_push_tunnel_right(session, height, TUNNEL_6);
            break;
        case 3:
            paint_util_push_tunnel_left(session, height, TUNNEL_6);
            break;
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_BC | SEGMENT_CC, direction), 48, 0x20);
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 5:
        switch (direction) {
        case 0:
            wooden_a_supports_paint_setup(session, 9, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            wooden_a_supports_paint_setup(session, 10, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            wooden_a_supports_paint_setup(session, 11, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            wooden_a_supports_paint_setup(session, 8, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 6:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20290, 0, 0, 16, 16, 1, height, 16, 0, height);
            break;
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20293, 0, 0, 16, 16, 1, height, 0, 0, height + 27);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20296, 0, 0, 16, 16, 1, height, 0, 16, height);
            break;
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20287, 0, 0, 16, 16, 1, height, 16, 16, height);
            break;
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 7:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20291, 0, 0, 32, 20, 1, height, 0, 6, height);
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20298, 0, 0, 32, 1, 26, height, 0, 27, height);
            wooden_a_supports_paint_setup(session, 11, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20294, 0, 0, 32, 1, 26, height, 0, 27, height);
            wooden_a_supports_paint_setup(session, 8, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20297, 0, 0, 32, 20, 1, height, 0, 6, height);
            wooden_a_supports_paint_setup(session, 9, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20288, 0, 0, 32, 20, 1, height, 0, 6, height);
            wooden_a_supports_paint_setup(session, 10, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(session, direction, height + 8, TUNNEL_6);
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_BC | SEGMENT_C0 | SEGMENT_D4, direction), 48, 0x20);
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    }
}

/** rct2: 0x0071C2A4 */
static void mine_train_rc_track_left_half_banked_helix_down_small(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    if (trackSequence >= 4) {
        trackSequence -= 4;
        direction = (direction - 1) & 3;
    }
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    mine_train_rc_track_right_half_banked_helix_up_small(session, rideIndex, trackSequence, (direction + 1) & 3, height, mapElement);
}

/** rct2: 0x0071C2B4 */
static void mine_train_rc_track_right_half_banked_helix_down_small(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    if (trackSequence >= 4) {
        trackSequence -= 4;
        direction = (direction + 1) & 3;
    }
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    mine_train_rc_track_left_half_banked_helix_up_small(session, rideIndex, trackSequence, (direction - 1) & 3, height, mapElement);
}

/** rct2: 0x0071C2C4 */
static void mine_train_rc_track_left_half_banked_helix_up_large(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20345, 0, 0, 32, 20, 1, height, 0, 6, height);
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20356, 0, 0, 32, 1, 26, height, 0, 27, height);
            wooden_a_supports_paint_setup(session, 6, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20350, 0, 0, 32, 1, 26, height, 0, 27, height);
            wooden_a_supports_paint_setup(session, 7, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20355, 0, 0, 32, 20, 1, height, 0, 6, height);
            wooden_a_supports_paint_setup(session, 6, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20340, 0, 0, 32, 20, 1, height, 0, 6, height);
            wooden_a_supports_paint_setup(session, 7, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_6);
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_BC | SEGMENT_C0 | SEGMENT_D4, direction), 48, 0x20);
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 1:
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_CC | SEGMENT_D4, direction), 48, 0x20);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20344, 0, 0, 32, 16, 1, height, 0, 0, height);
            wooden_a_supports_paint_setup(session, 11, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20349, 0, 0, 32, 16, 1, height, 0, 0, height + 27);
            wooden_a_supports_paint_setup(session, 8, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20354, 0, 0, 32, 16, 1, height, 0, 16, height);
            wooden_a_supports_paint_setup(session, 9, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20339, 0, 0, 32, 16, 1, height, 0, 16, height);
            wooden_a_supports_paint_setup(session, 10, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20343, 0, 0, 16, 16, 1, height, 0, 16, height);
            wooden_a_supports_paint_setup(session, 9, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20348, 0, 0, 16, 16, 1, height, 16, 16, height + 27);
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20178, 0, 0, 16, 16, 1, height, 16, 16, height + 27);
            wooden_a_supports_paint_setup(session, 10, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20353, 0, 0, 16, 16, 1, height, 16, 0, height);
            wooden_a_supports_paint_setup(session, 11, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20338, 0, 0, 16, 16, 1, height, 0, 0, height);
            wooden_a_supports_paint_setup(session, 8, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C0 | SEGMENT_C8 | SEGMENT_D0, direction), 48, 0x20);
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 4:
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_CC | SEGMENT_D4, direction), 48, 0x20);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 5:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20342, 0, 0, 16, 32, 1, height, 16, 0, height);
            wooden_a_supports_paint_setup(session, 11, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20347, 0, 0, 16, 32, 1, height, 0, 0, height + 27);
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20177, 0, 0, 16, 32, 1, height, 0, 0, height + 27);
            wooden_a_supports_paint_setup(session, 8, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20352, 0, 0, 16, 32, 1, height, 0, 0, height);
            wooden_a_supports_paint_setup(session, 9, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20337, 0, 0, 16, 32, 1, height, 16, 0, height);
            wooden_a_supports_paint_setup(session, 10, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 6:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20341, 0, 0, 20, 32, 1, height, 6, 0, height);
            wooden_a_supports_paint_setup(session, 7, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20346, 0, 0, 1, 32, 26, height, 27, 0, height);
            wooden_a_supports_paint_setup(session, 6, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20351, 0, 0, 20, 32, 1, height, 6, 0, height);
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20357, 0, 0, 1, 32, 26, height, 27, 0, height);
            wooden_a_supports_paint_setup(session, 7, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20336, 0, 0, 20, 32, 1, height, 6, 0, height);
            wooden_a_supports_paint_setup(session, 6, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        switch (direction) {
        case 2:
            paint_util_push_tunnel_right(session, height + 8, TUNNEL_6);
            break;
        case 3:
            paint_util_push_tunnel_left(session, height + 8, TUNNEL_6);
            break;
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_BC | SEGMENT_CC, direction), 48, 0x20);
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 7:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20340, 0, 0, 20, 32, 1, height, 6, 0, height);
            wooden_a_supports_paint_setup(session, 7, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20345, 0, 0, 20, 32, 1, height, 6, 0, height);
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20356, 0, 0, 1, 32, 26, height, 27, 0, height);
            wooden_a_supports_paint_setup(session, 6, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20350, 0, 0, 1, 32, 26, height, 27, 0, height);
            wooden_a_supports_paint_setup(session, 7, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20355, 0, 0, 20, 32, 1, height, 6, 0, height);
            wooden_a_supports_paint_setup(session, 6, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        switch (direction) {
        case 0:
            paint_util_push_tunnel_right(session, height, TUNNEL_6);
            break;
        case 1:
            paint_util_push_tunnel_left(session, height, TUNNEL_6);
            break;
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_BC | SEGMENT_C0 | SEGMENT_CC, direction), 48, 0x20);
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 8:
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C8 | SEGMENT_CC, direction), 48, 0x20);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 9:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20339, 0, 0, 16, 32, 1, height, 16, 0, height);
            wooden_a_supports_paint_setup(session, 10, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20344, 0, 0, 16, 32, 1, height, 0, 0, height);
            wooden_a_supports_paint_setup(session, 11, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20349, 0, 0, 16, 32, 1, height, 0, 0, height + 27);
            wooden_a_supports_paint_setup(session, 8, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20354, 0, 0, 16, 32, 1, height, 16, 0, height);
            wooden_a_supports_paint_setup(session, 9, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 10:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20338, 0, 0, 16, 16, 1, height, 0, 0, height);
            wooden_a_supports_paint_setup(session, 8, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20343, 0, 0, 16, 16, 1, height, 16, 0, height);
            wooden_a_supports_paint_setup(session, 9, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20348, 0, 0, 16, 16, 1, height, 16, 16, height + 27);
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20178, 0, 0, 16, 16, 1, height, 16, 16, height + 27);
            wooden_a_supports_paint_setup(session, 10, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20353, 0, 0, 16, 16, 1, height, 0, 16, height);
            wooden_a_supports_paint_setup(session, 11, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_BC | SEGMENT_D0 | SEGMENT_D4, direction), 48, 0x20);
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 11:
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C8 | SEGMENT_CC, direction), 48, 0x20);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 12:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20337, 0, 0, 32, 16, 1, height, 0, 16, height);
            wooden_a_supports_paint_setup(session, 10, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20342, 0, 0, 32, 16, 1, height, 0, 16, height);
            wooden_a_supports_paint_setup(session, 11, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20347, 0, 0, 32, 16, 1, height, 0, 0, height + 27);
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20177, 0, 0, 32, 16, 1, height, 0, 0, height + 27);
            wooden_a_supports_paint_setup(session, 8, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20352, 0, 0, 32, 16, 1, height, 0, 0, height);
            wooden_a_supports_paint_setup(session, 9, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 13:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20336, 0, 0, 32, 20, 1, height, 0, 6, height);
            wooden_a_supports_paint_setup(session, 6, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20341, 0, 0, 32, 20, 1, height, 0, 6, height);
            wooden_a_supports_paint_setup(session, 7, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20346, 0, 0, 32, 1, 26, height, 0, 27, height);
            wooden_a_supports_paint_setup(session, 6, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20351, 0, 0, 32, 20, 1, height, 0, 6, height);
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20357, 0, 0, 32, 1, 26, height, 0, 27, height);
            wooden_a_supports_paint_setup(session, 7, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(session, direction, height + 8, TUNNEL_6);
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C8, direction), 48, 0x20);
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    }
}

/** rct2: 0x0071C2D4 */
static void mine_train_rc_track_right_half_banked_helix_up_large(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20314, 0, 0, 32, 20, 1, height, 0, 6, height);
            wooden_a_supports_paint_setup(session, 6, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20319, 0, 0, 32, 20, 1, height, 0, 6, height);
            wooden_a_supports_paint_setup(session, 7, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20324, 0, 0, 32, 1, 26, height, 0, 27, height);
            wooden_a_supports_paint_setup(session, 6, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20329, 0, 0, 32, 20, 1, height, 0, 6, height);
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20335, 0, 0, 32, 1, 26, height, 0, 27, height);
            wooden_a_supports_paint_setup(session, 7, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_6);
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C8, direction), 48, 0x20);
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 1:
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C8 | SEGMENT_CC, direction), 48, 0x20);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20315, 0, 0, 32, 16, 1, height, 0, 16, height);
            wooden_a_supports_paint_setup(session, 10, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20320, 0, 0, 32, 16, 1, height, 0, 16, height);
            wooden_a_supports_paint_setup(session, 11, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20325, 0, 0, 32, 16, 1, height, 0, 0, height + 27);
            wooden_a_supports_paint_setup(session, 8, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20330, 0, 0, 32, 16, 1, height, 0, 0, height);
            wooden_a_supports_paint_setup(session, 9, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20316, 0, 0, 16, 16, 1, height, 0, 0, height);
            wooden_a_supports_paint_setup(session, 8, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20321, 0, 0, 16, 16, 1, height, 16, 0, height);
            wooden_a_supports_paint_setup(session, 9, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20326, 0, 0, 16, 16, 1, height, 16, 16, height + 27);
            wooden_a_supports_paint_setup(session, 10, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20331, 0, 0, 16, 16, 1, height, 0, 16, height);
            wooden_a_supports_paint_setup(session, 11, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_BC | SEGMENT_D0 | SEGMENT_D4, direction), 48, 0x20);
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 4:
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C8 | SEGMENT_CC, direction), 48, 0x20);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 5:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20317, 0, 0, 16, 32, 1, height, 16, 0, height);
            wooden_a_supports_paint_setup(session, 10, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20322, 0, 0, 16, 32, 1, height, 0, 0, height);
            wooden_a_supports_paint_setup(session, 11, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20327, 0, 0, 16, 32, 1, height, 0, 0, height + 27);
            wooden_a_supports_paint_setup(session, 8, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20332, 0, 0, 16, 32, 1, height, 16, 0, height);
            wooden_a_supports_paint_setup(session, 9, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 6:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20318, 0, 0, 20, 32, 1, height, 6, 0, height);
            wooden_a_supports_paint_setup(session, 7, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20323, 0, 0, 20, 32, 1, height, 6, 0, height);
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20334, 0, 0, 1, 32, 26, height, 27, 0, height);
            wooden_a_supports_paint_setup(session, 6, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20328, 0, 0, 1, 32, 26, height, 27, 0, height);
            wooden_a_supports_paint_setup(session, 7, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20333, 0, 0, 20, 32, 1, height, 6, 0, height);
            wooden_a_supports_paint_setup(session, 6, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        switch (direction) {
        case 0:
            paint_util_push_tunnel_right(session, height + 8, TUNNEL_6);
            break;
        case 1:
            paint_util_push_tunnel_left(session, height + 8, TUNNEL_6);
            break;
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_BC | SEGMENT_C0 | SEGMENT_CC, direction), 48, 0x20);
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 7:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20319, 0, 0, 20, 32, 1, height, 6, 0, height);
            wooden_a_supports_paint_setup(session, 7, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20324, 0, 0, 1, 32, 26, height, 27, 0, height);
            wooden_a_supports_paint_setup(session, 6, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20329, 0, 0, 20, 32, 1, height, 6, 0, height);
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20335, 0, 0, 1, 32, 26, height, 27, 0, height);
            wooden_a_supports_paint_setup(session, 7, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20314, 0, 0, 20, 32, 1, height, 6, 0, height);
            wooden_a_supports_paint_setup(session, 6, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        switch (direction) {
        case 2:
            paint_util_push_tunnel_right(session, height, TUNNEL_6);
            break;
        case 3:
            paint_util_push_tunnel_left(session, height, TUNNEL_6);
            break;
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_BC | SEGMENT_CC, direction), 48, 0x20);
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 8:
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_CC | SEGMENT_D4, direction), 48, 0x20);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 9:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20320, 0, 0, 16, 32, 1, height, 16, 0, height);
            wooden_a_supports_paint_setup(session, 11, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20325, 0, 0, 16, 32, 1, height, 0, 0, height + 27);
            wooden_a_supports_paint_setup(session, 8, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20330, 0, 0, 16, 32, 1, height, 0, 0, height);
            wooden_a_supports_paint_setup(session, 9, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20315, 0, 0, 16, 32, 1, height, 16, 0, height);
            wooden_a_supports_paint_setup(session, 10, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 10:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20321, 0, 0, 16, 16, 1, height, 0, 16, height);
            wooden_a_supports_paint_setup(session, 9, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20326, 0, 0, 16, 16, 1, height, 16, 16, height + 27);
            wooden_a_supports_paint_setup(session, 10, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20331, 0, 0, 16, 16, 1, height, 16, 0, height);
            wooden_a_supports_paint_setup(session, 11, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20316, 0, 0, 16, 16, 1, height, 0, 0, height);
            wooden_a_supports_paint_setup(session, 8, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C0 | SEGMENT_C8 | SEGMENT_D0, direction), 48, 0x20);
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 11:
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_CC | SEGMENT_D4, direction), 48, 0x20);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 12:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20322, 0, 0, 32, 16, 1, height, 0, 0, height);
            wooden_a_supports_paint_setup(session, 11, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20327, 0, 0, 32, 16, 1, height, 0, 0, height + 27);
            wooden_a_supports_paint_setup(session, 8, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20332, 0, 0, 32, 16, 1, height, 0, 16, height);
            wooden_a_supports_paint_setup(session, 9, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20317, 0, 0, 32, 16, 1, height, 0, 16, height);
            wooden_a_supports_paint_setup(session, 10, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 13:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20323, 0, 0, 32, 20, 1, height, 0, 6, height);
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20334, 0, 0, 32, 1, 26, height, 0, 27, height);
            wooden_a_supports_paint_setup(session, 6, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20328, 0, 0, 32, 1, 26, height, 0, 27, height);
            wooden_a_supports_paint_setup(session, 7, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20333, 0, 0, 32, 20, 1, height, 0, 6, height);
            wooden_a_supports_paint_setup(session, 6, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20318, 0, 0, 32, 20, 1, height, 0, 6, height);
            wooden_a_supports_paint_setup(session, 7, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(session, direction, height + 8, TUNNEL_6);
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_BC | SEGMENT_C0 | SEGMENT_D4, direction), 48, 0x20);
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    }
}

/** rct2: 0x0071C2E4 */
static void mine_train_rc_track_left_half_banked_helix_down_large(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    if (trackSequence >= 7) {
        trackSequence -= 7;
        direction = (direction - 1) & 3;
    }
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    mine_train_rc_track_right_half_banked_helix_up_large(session, rideIndex, trackSequence, (direction + 1) & 3, height, mapElement);
}

/** rct2: 0x0071C2F4 */
static void mine_train_rc_track_right_half_banked_helix_down_large(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    if (trackSequence >= 7) {
        trackSequence -= 7;
        direction = (direction + 1) & 3;
    }
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    mine_train_rc_track_left_half_banked_helix_up_large(session, rideIndex, trackSequence, (direction - 1) & 3, height, mapElement);
}

/** rct2: 0x0071C324 */
static void mine_train_rc_track_brakes(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
    case 2:
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20058, 0, 0, 32, 20, 1, height, 0, 6, height);
        wooden_a_supports_paint_setup(session, 6, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    case 1:
    case 3:
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20059, 0, 0, 32, 20, 1, height, 0, 6, height);
        wooden_a_supports_paint_setup(session, 7, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    }
    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_6);
    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x0071C334 */
static void mine_train_rc_track_on_ride_photo(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20052, 0, 0, 32, 20, 1, height, 0, 6, height);
        wooden_a_supports_paint_setup(session, 6, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    case 1:
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20053, 0, 0, 32, 20, 1, height, 0, 6, height);
        wooden_a_supports_paint_setup(session, 7, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    case 2:
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20052, 0, 0, 32, 20, 1, height, 0, 6, height);
        wooden_a_supports_paint_setup(session, 6, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    case 3:
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20053, 0, 0, 32, 20, 1, height, 0, 6, height);
        wooden_a_supports_paint_setup(session, 7, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    }
    track_paint_util_onride_photo_paint(session, direction, height + 2, mapElement);
    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_6);
    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 48, 0x20);
}

/** rct2: 0x0071C354 */
static void mine_train_rc_track_left_eighth_to_diag(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20452, 0, 0, 32, 32, 1, height, 0, 0, height);
            wooden_a_supports_paint_setup(session, 6, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20456, 0, 0, 32, 32, 1, height, 0, 0, height);
            wooden_a_supports_paint_setup(session, 7, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20460, 0, 0, 32, 32, 1, height, 0, 0, height);
            wooden_a_supports_paint_setup(session, 6, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20464, 0, 0, 32, 32, 1, height, 0, 0, height);
            wooden_a_supports_paint_setup(session, 7, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_6);
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20453, 0, 0, 32, 16, 1, height, 0, 0, height);
            wooden_a_supports_paint_setup(session, 6, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20457, 0, 0, 34, 16, 1, height, 0, 0, height);
            wooden_a_supports_paint_setup(session, 7, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20461, 0, 0, 32, 16, 1, height, 0, 16, height);
            wooden_a_supports_paint_setup(session, 6, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20465, 0, 0, 32, 16, 1, height, 0, 16, height);
            wooden_a_supports_paint_setup(session, 7, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20454, 0, 0, 16, 16, 1, height, 0, 16, height);
            wooden_a_supports_paint_setup(session, 9, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20458, 0, 0, 16, 16, 1, height, 16, 16, height);
            wooden_a_supports_paint_setup(session, 10, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20462, 0, 0, 16, 16, 1, height, 16, 0, height);
            wooden_a_supports_paint_setup(session, 11, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20466, 0, 0, 16, 16, 1, height, 0, 0, height);
            wooden_a_supports_paint_setup(session, 8, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            wooden_a_supports_paint_setup(session, 11, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            wooden_a_supports_paint_setup(session, 8, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            wooden_a_supports_paint_setup(session, 9, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            wooden_a_supports_paint_setup(session, 10, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 4:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20455, 0, 0, 16, 16, 1, height, 16, 16, height);
            break;
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20459, 0, 0, 16, 18, 1, height, 0, 16, height);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20463, 0, 0, 16, 16, 1, height, 0, 0, height);
            break;
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20467, 0, 0, 16, 16, 1, height, 16, 0, height);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    }
}

/** rct2: 0x0071C364 */
static void mine_train_rc_track_right_eighth_to_diag(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20436, 0, 0, 32, 32, 1, height, 0, 0, height);
            wooden_a_supports_paint_setup(session, 6, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20440, 0, 0, 32, 32, 1, height, 0, 0, height);
            wooden_a_supports_paint_setup(session, 7, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20444, 0, 0, 32, 32, 1, height, 0, 0, height);
            wooden_a_supports_paint_setup(session, 6, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20448, 0, 0, 32, 32, 1, height, 0, 0, height);
            wooden_a_supports_paint_setup(session, 7, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_6);
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20437, 0, 0, 32, 16, 1, height, 0, 16, height);
            wooden_a_supports_paint_setup(session, 6, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20441, 0, 0, 32, 16, 1, height, 0, 16, height);
            wooden_a_supports_paint_setup(session, 7, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20445, 0, 0, 34, 16, 1, height, 0, 0, height);
            wooden_a_supports_paint_setup(session, 6, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20449, 0, 0, 32, 16, 1, height, 0, 0, height);
            wooden_a_supports_paint_setup(session, 7, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20438, 0, 0, 16, 16, 1, height, 0, 0, height);
            wooden_a_supports_paint_setup(session, 8, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20442, 0, 0, 16, 16, 1, height, 16, 0, height);
            wooden_a_supports_paint_setup(session, 9, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20446, 0, 0, 28, 28, 1, height, 4, 4, height);
            wooden_a_supports_paint_setup(session, 10, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20450, 0, 0, 16, 16, 1, height, 0, 16, height);
            wooden_a_supports_paint_setup(session, 11, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            wooden_a_supports_paint_setup(session, 10, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            wooden_a_supports_paint_setup(session, 11, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            wooden_a_supports_paint_setup(session, 8, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            wooden_a_supports_paint_setup(session, 9, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 4:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20439, 0, 0, 16, 16, 1, height, 16, 0, height);
            break;
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20443, 0, 0, 16, 16, 1, height, 0, 0, height);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20447, 0, 0, 16, 18, 1, height, 0, 16, height);
            break;
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20451, 0, 0, 16, 16, 1, height, 16, 16, height);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    }
}

/** rct2: 0x0071C374 */
static void mine_train_rc_track_left_eighth_to_orthogonal(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    mine_train_rc_track_right_eighth_to_diag(session, rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x0071C384 */
static void mine_train_rc_track_right_eighth_to_orthogonal(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    mine_train_rc_track_left_eighth_to_diag(session, rideIndex, trackSequence, (direction + 3) & 3, height, mapElement);
}

/** rct2: 0x0071C394 */
static void mine_train_rc_track_left_eighth_bank_to_diag(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20484, 0, 0, 32, 1, 26, height, 0, 27, height);
            wooden_a_supports_paint_setup(session, 6, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20488, 0, 0, 32, 1, 26, height, 0, 27, height);
            wooden_a_supports_paint_setup(session, 7, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20492, 0, 0, 32, 32, 1, height, 0, 0, height);
            wooden_a_supports_paint_setup(session, 6, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20496, 0, 0, 32, 32, 1, height, 0, 0, height);
            wooden_a_supports_paint_setup(session, 7, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_6);
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20485, 0, 0, 32, 16, 1, height, 0, 0, height);
            wooden_a_supports_paint_setup(session, 6, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20489, 0, 0, 34, 16, 0, height, 0, 0, height + 27);
            wooden_a_supports_paint_setup(session, 7, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20493, 0, 0, 32, 16, 1, height, 0, 16, height);
            wooden_a_supports_paint_setup(session, 6, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20497, 0, 0, 32, 16, 1, height, 0, 16, height);
            wooden_a_supports_paint_setup(session, 7, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20486, 0, 0, 16, 16, 1, height, 0, 16, height);
            wooden_a_supports_paint_setup(session, 9, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20490, 0, 0, 16, 16, 0, height, 16, 16, height + 27);
            wooden_a_supports_paint_setup(session, 10, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20494, 0, 0, 16, 16, 1, height, 16, 0, height);
            wooden_a_supports_paint_setup(session, 11, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20498, 0, 0, 16, 16, 1, height, 0, 0, height);
            wooden_a_supports_paint_setup(session, 8, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            wooden_a_supports_paint_setup(session, 11, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            wooden_a_supports_paint_setup(session, 8, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            wooden_a_supports_paint_setup(session, 9, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            wooden_a_supports_paint_setup(session, 10, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 4:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20487, 0, 0, 16, 16, 1, height, 16, 16, height);
            break;
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20491, 0, 0, 16, 18, 0, height, 0, 16, height + 27);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20495, 0, 0, 16, 16, 1, height, 0, 0, height);
            break;
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20499, 0, 0, 16, 16, 1, height, 16, 0, height);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    }
}

/** rct2: 0x0071C3A4 */
static void mine_train_rc_track_right_eighth_bank_to_diag(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20468, 0, 0, 32, 32, 1, height, 0, 0, height);
            wooden_a_supports_paint_setup(session, 6, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20472, 0, 0, 32, 32, 1, height, 0, 0, height);
            wooden_a_supports_paint_setup(session, 7, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20476, 0, 0, 32, 1, 26, height, 0, 27, height);
            wooden_a_supports_paint_setup(session, 6, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20480, 0, 0, 32, 1, 26, height, 0, 27, height);
            wooden_a_supports_paint_setup(session, 7, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_6);
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20469, 0, 0, 32, 16, 1, height, 0, 16, height);
            wooden_a_supports_paint_setup(session, 6, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20473, 0, 0, 32, 16, 1, height, 0, 16, height);
            wooden_a_supports_paint_setup(session, 7, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20477, 0, 0, 34, 16, 0, height, 0, 0, height + 27);
            wooden_a_supports_paint_setup(session, 6, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20481, 0, 0, 32, 16, 1, height, 0, 0, height);
            wooden_a_supports_paint_setup(session, 7, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20470, 0, 0, 16, 16, 1, height, 0, 0, height);
            wooden_a_supports_paint_setup(session, 8, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20474, 0, 0, 16, 16, 1, height, 16, 0, height);
            wooden_a_supports_paint_setup(session, 9, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20478, 0, 0, 28, 28, 0, height, 4, 4, height + 27);
            wooden_a_supports_paint_setup(session, 10, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20482, 0, 0, 16, 16, 1, height, 0, 16, height);
            wooden_a_supports_paint_setup(session, 11, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            wooden_a_supports_paint_setup(session, 10, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            wooden_a_supports_paint_setup(session, 11, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            wooden_a_supports_paint_setup(session, 8, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            wooden_a_supports_paint_setup(session, 9, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 4:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20471, 0, 0, 16, 16, 1, height, 16, 0, height);
            break;
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20475, 0, 0, 16, 16, 1, height, 0, 0, height);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20479, 0, 0, 16, 18, 0, height, 0, 16, height + 27);
            break;
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20483, 0, 0, 16, 16, 1, height, 16, 16, height);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    }
}

/** rct2: 0x0071C3B4 */
static void mine_train_rc_track_left_eighth_bank_to_orthogonal(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    mine_train_rc_track_right_eighth_bank_to_diag(session, rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x0071C3C4 */
static void mine_train_rc_track_right_eighth_bank_to_orthogonal(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    mine_train_rc_track_left_eighth_bank_to_diag(session, rideIndex, trackSequence, (direction + 3) & 3, height, mapElement);
}

/** rct2: 0x0071C344 */
static void mine_train_rc_track_diag_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 3:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20389, -16, -16, 32, 32, 1, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 3:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20361, -16, -16, 32, 32, 1, height, -16, -16, height);
                break;
            }
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 1:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 0:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20386, -16, -16, 32, 32, 1, height, -16, -16, height);
                wooden_a_supports_paint_setup(session, 8, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 1:
                wooden_a_supports_paint_setup(session, 9, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 2:
                wooden_a_supports_paint_setup(session, 10, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 3:
                wooden_a_supports_paint_setup(session, 11, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20358, -16, -16, 32, 32, 1, height, -16, -16, height);
                wooden_a_supports_paint_setup(session, 8, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 1:
                wooden_a_supports_paint_setup(session, 9, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 2:
                wooden_a_supports_paint_setup(session, 10, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 3:
                wooden_a_supports_paint_setup(session, 11, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            }
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 2:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 0:
                wooden_a_supports_paint_setup(session, 10, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 1:
                wooden_a_supports_paint_setup(session, 11, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 2:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20388, -16, -16, 32, 32, 1, height, -16, -16, height);
                wooden_a_supports_paint_setup(session, 8, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 3:
                wooden_a_supports_paint_setup(session, 9, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                wooden_a_supports_paint_setup(session, 10, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 1:
                wooden_a_supports_paint_setup(session, 11, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 2:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20360, -16, -16, 32, 32, 1, height, -16, -16, height);
                wooden_a_supports_paint_setup(session, 8, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 3:
                wooden_a_supports_paint_setup(session, 9, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            }
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 3:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 1:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20387, -16, -16, 32, 32, 1, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 1:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20359, -16, -16, 32, 32, 1, height, -16, -16, height);
                break;
            }
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    }
}

/** rct2: 0x0071C414 */
static void mine_train_rc_track_diag_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 3:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20401, -16, -16, 32, 32, 1, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 3:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20373, -16, -16, 32, 32, 1, height, -16, -16, height);
                break;
            }
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 56, 0x20);
        break;
    case 1:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 0:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20398, -16, -16, 32, 32, 1, height, -16, -16, height);
                wooden_b_supports_paint_setup(session, 8, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 1:
                wooden_b_supports_paint_setup(session, 9, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 2:
                wooden_b_supports_paint_setup(session, 10, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 3:
                wooden_b_supports_paint_setup(session, 11, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20370, -16, -16, 32, 32, 1, height, -16, -16, height);
                wooden_b_supports_paint_setup(session, 8, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 1:
                wooden_b_supports_paint_setup(session, 9, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 2:
                wooden_b_supports_paint_setup(session, 10, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 3:
                wooden_b_supports_paint_setup(session, 11, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            }
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 56, 0x20);
        break;
    case 2:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 0:
                wooden_b_supports_paint_setup(session, 10, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 1:
                wooden_b_supports_paint_setup(session, 11, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 2:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20400, -16, -16, 32, 32, 1, height, -16, -16, height);
                wooden_b_supports_paint_setup(session, 8, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 3:
                wooden_b_supports_paint_setup(session, 9, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                wooden_b_supports_paint_setup(session, 10, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 1:
                wooden_b_supports_paint_setup(session, 11, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 2:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20372, -16, -16, 32, 32, 1, height, -16, -16, height);
                wooden_b_supports_paint_setup(session, 8, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 3:
                wooden_b_supports_paint_setup(session, 9, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            }
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 56, 0x20);
        break;
    case 3:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 1:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20399, -16, -16, 32, 32, 1, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 1:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20371, -16, -16, 32, 32, 1, height, -16, -16, height);
                break;
            }
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 56, 0x20);
        break;
    }
}

/** rct2: 0x0071C474 */
static void mine_train_rc_track_diag_60_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20385, -16, -16, 32, 32, 1, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 104, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20382, -16, -16, 32, 32, 1, height, -16, -16, height);
            wooden_b_supports_paint_setup(session, 8, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            wooden_b_supports_paint_setup(session, 9, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            wooden_b_supports_paint_setup(session, 10, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            wooden_b_supports_paint_setup(session, 11, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 104, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            wooden_b_supports_paint_setup(session, 10, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            wooden_b_supports_paint_setup(session, 11, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20384, -16, -16, 32, 32, 1, height, -16, -16, height);
            wooden_b_supports_paint_setup(session, 8, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            wooden_b_supports_paint_setup(session, 9, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 104, 0x20);
        break;
    case 3:
        switch (direction) {
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20383, -16, -16, 32, 32, 1, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 104, 0x20);
        break;
    }
}

/** rct2: 0x0071C3F4 */
static void mine_train_rc_track_diag_flat_to_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 3:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20393, -16, -16, 32, 32, 1, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 3:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20365, -16, -16, 32, 32, 1, height, -16, -16, height);
                break;
            }
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 48, 0x20);
        break;
    case 1:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 0:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20390, -16, -16, 32, 32, 1, height, -16, -16, height);
                wooden_a_supports_paint_setup(session, 8, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 1:
                wooden_a_supports_paint_setup(session, 9, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 2:
                wooden_a_supports_paint_setup(session, 10, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 3:
                wooden_a_supports_paint_setup(session, 11, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20362, -16, -16, 32, 32, 1, height, -16, -16, height);
                wooden_a_supports_paint_setup(session, 8, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 1:
                wooden_a_supports_paint_setup(session, 9, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 2:
                wooden_a_supports_paint_setup(session, 10, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 3:
                wooden_a_supports_paint_setup(session, 11, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            }
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 48, 0x20);
        break;
    case 2:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 0:
                wooden_a_supports_paint_setup(session, 10, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 1:
                wooden_a_supports_paint_setup(session, 11, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 2:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20392, -16, -16, 32, 32, 1, height, -16, -16, height);
                wooden_a_supports_paint_setup(session, 8, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 3:
                wooden_a_supports_paint_setup(session, 9, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                wooden_a_supports_paint_setup(session, 10, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 1:
                wooden_a_supports_paint_setup(session, 11, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 2:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20364, -16, -16, 32, 32, 1, height, -16, -16, height);
                wooden_a_supports_paint_setup(session, 8, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 3:
                wooden_a_supports_paint_setup(session, 9, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            }
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 48, 0x20);
        break;
    case 3:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 1:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20391, -16, -16, 32, 32, 1, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 1:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20363, -16, -16, 32, 32, 1, height, -16, -16, height);
                break;
            }
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 48, 0x20);
        break;
    }
}

/** rct2: 0x0071C454 */
static void mine_train_rc_track_diag_25_deg_up_to_60_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20377, -16, -16, 32, 32, 1, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 72, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20374, -16, -16, 32, 32, 1, height, -16, -16, height);
            wooden_b_supports_paint_setup(session, 8, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            wooden_b_supports_paint_setup(session, 9, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            wooden_b_supports_paint_setup(session, 10, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            wooden_b_supports_paint_setup(session, 11, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 72, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            wooden_b_supports_paint_setup(session, 10, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            wooden_b_supports_paint_setup(session, 11, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20376, -16, -16, 32, 32, 1, height, -16, -16, height);
            wooden_b_supports_paint_setup(session, 8, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            wooden_b_supports_paint_setup(session, 9, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 72, 0x20);
        break;
    case 3:
        switch (direction) {
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20375, -16, -16, 32, 32, 1, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 72, 0x20);
        break;
    }
}

/** rct2: 0x0071C464 */
static void mine_train_rc_track_diag_60_deg_up_to_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20381, -16, -16, 32, 32, 1, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 72, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20378, -16, -16, 32, 32, 1, height, -16, -16, height);
            wooden_b_supports_paint_setup(session, 8, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            wooden_b_supports_paint_setup(session, 9, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            wooden_b_supports_paint_setup(session, 10, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            wooden_b_supports_paint_setup(session, 11, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 72, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            wooden_b_supports_paint_setup(session, 10, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            wooden_b_supports_paint_setup(session, 11, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20380, -16, -16, 32, 32, 1, height, -16, -16, height);
            wooden_b_supports_paint_setup(session, 8, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            wooden_b_supports_paint_setup(session, 9, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 72, 0x20);
        break;
    case 3:
        switch (direction) {
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20379, -16, -16, 16, 16, 1, height, 0, 0, height);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 72, 0x20);
        break;
    }
}

/** rct2: 0x0071C404 */
static void mine_train_rc_track_diag_25_deg_up_to_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 3:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20397, -16, -16, 32, 32, 1, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 3:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20369, -16, -16, 32, 32, 1, height, -16, -16, height);
                break;
            }
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 56, 0x20);
        break;
    case 1:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 0:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20394, -16, -16, 32, 32, 1, height, -16, -16, height);
                wooden_b_supports_paint_setup(session, 8, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 1:
                wooden_b_supports_paint_setup(session, 9, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 2:
                wooden_b_supports_paint_setup(session, 10, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 3:
                wooden_b_supports_paint_setup(session, 11, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20366, -16, -16, 32, 32, 1, height, -16, -16, height);
                wooden_b_supports_paint_setup(session, 8, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 1:
                wooden_b_supports_paint_setup(session, 9, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 2:
                wooden_b_supports_paint_setup(session, 10, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 3:
                wooden_b_supports_paint_setup(session, 11, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            }
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 56, 0x20);
        break;
    case 2:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 0:
                wooden_b_supports_paint_setup(session, 10, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 1:
                wooden_b_supports_paint_setup(session, 11, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 2:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20396, -16, -16, 32, 32, 1, height, -16, -16, height);
                wooden_b_supports_paint_setup(session, 8, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 3:
                wooden_b_supports_paint_setup(session, 9, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                wooden_b_supports_paint_setup(session, 10, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 1:
                wooden_b_supports_paint_setup(session, 11, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 2:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20368, -16, -16, 32, 32, 1, height, -16, -16, height);
                wooden_b_supports_paint_setup(session, 8, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 3:
                wooden_b_supports_paint_setup(session, 9, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            }
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 56, 0x20);
        break;
    case 3:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 1:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20395, -16, -16, 32, 32, 1, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 1:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20367, -16, -16, 32, 32, 1, height, -16, -16, height);
                break;
            }
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 56, 0x20);
        break;
    }
}

/** rct2: 0x0071C444 */
static void mine_train_rc_track_diag_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 3:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20399, -16, -16, 32, 32, 1, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 3:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20371, -16, -16, 32, 32, 1, height, -16, -16, height);
                break;
            }
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 56, 0x20);
        break;
    case 1:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 0:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20400, -16, -16, 32, 32, 1, height, -16, -16, height);
                wooden_b_supports_paint_setup(session, 8, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 1:
                wooden_b_supports_paint_setup(session, 9, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 2:
                wooden_b_supports_paint_setup(session, 10, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 3:
                wooden_b_supports_paint_setup(session, 11, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20372, -16, -16, 32, 32, 1, height, -16, -16, height);
                wooden_b_supports_paint_setup(session, 8, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 1:
                wooden_b_supports_paint_setup(session, 9, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 2:
                wooden_b_supports_paint_setup(session, 10, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 3:
                wooden_b_supports_paint_setup(session, 11, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            }
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 56, 0x20);
        break;
    case 2:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 0:
                wooden_b_supports_paint_setup(session, 10, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 1:
                wooden_b_supports_paint_setup(session, 11, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 2:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20398, -16, -16, 32, 32, 1, height, -16, -16, height);
                wooden_b_supports_paint_setup(session, 8, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 3:
                wooden_b_supports_paint_setup(session, 9, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                wooden_b_supports_paint_setup(session, 10, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 1:
                wooden_b_supports_paint_setup(session, 11, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 2:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20370, -16, -16, 32, 32, 1, height, -16, -16, height);
                wooden_b_supports_paint_setup(session, 8, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 3:
                wooden_b_supports_paint_setup(session, 9, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            }
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 56, 0x20);
        break;
    case 3:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 1:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20401, -16, -16, 32, 32, 1, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 1:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20373, -16, -16, 32, 32, 1, height, -16, -16, height);
                break;
            }
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 56, 0x20);
        break;
    }
}

/** rct2: 0x0071C4A4 */
static void mine_train_rc_track_diag_60_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20383, -16, -16, 32, 32, 1, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 104, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20384, -16, -16, 32, 32, 1, height, -16, -16, height);
            wooden_b_supports_paint_setup(session, 8, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            wooden_b_supports_paint_setup(session, 9, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            wooden_b_supports_paint_setup(session, 10, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            wooden_b_supports_paint_setup(session, 11, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 104, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            wooden_b_supports_paint_setup(session, 10, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            wooden_b_supports_paint_setup(session, 11, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20382, -16, -16, 32, 32, 1, height, -16, -16, height);
            wooden_b_supports_paint_setup(session, 8, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            wooden_b_supports_paint_setup(session, 9, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 104, 0x20);
        break;
    case 3:
        switch (direction) {
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20385, -16, -16, 32, 32, 1, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 104, 0x20);
        break;
    }
}

/** rct2: 0x0071C424 */
static void mine_train_rc_track_diag_flat_to_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 3:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20395, -16, -16, 32, 32, 1, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 3:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20367, -16, -16, 32, 32, 1, height, -16, -16, height);
                break;
            }
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        break;
    case 1:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 0:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20396, -16, -16, 32, 32, 1, height, -16, -16, height);
                wooden_b_supports_paint_setup(session, 8, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 1:
                wooden_b_supports_paint_setup(session, 9, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 2:
                wooden_b_supports_paint_setup(session, 10, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 3:
                wooden_b_supports_paint_setup(session, 11, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20368, -16, -16, 32, 32, 1, height, -16, -16, height);
                wooden_b_supports_paint_setup(session, 8, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 1:
                wooden_b_supports_paint_setup(session, 9, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 2:
                wooden_b_supports_paint_setup(session, 10, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 3:
                wooden_b_supports_paint_setup(session, 11, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            }
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        break;
    case 2:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 0:
                wooden_b_supports_paint_setup(session, 10, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 1:
                wooden_b_supports_paint_setup(session, 11, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 2:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20394, -16, -16, 32, 32, 1, height, -16, -16, height);
                wooden_b_supports_paint_setup(session, 8, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 3:
                wooden_b_supports_paint_setup(session, 9, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                wooden_b_supports_paint_setup(session, 10, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 1:
                wooden_b_supports_paint_setup(session, 11, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 2:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20366, -16, -16, 32, 32, 1, height, -16, -16, height);
                wooden_b_supports_paint_setup(session, 8, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 3:
                wooden_b_supports_paint_setup(session, 9, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            }
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        break;
    case 3:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 1:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20397, -16, -16, 32, 32, 1, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 1:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20369, -16, -16, 32, 32, 1, height, -16, -16, height);
                break;
            }
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        break;
    }

    paint_util_set_general_support_height(session, height + 56, 0x20);
}

/** rct2: 0x0071C484 */
static void mine_train_rc_track_diag_25_deg_down_to_60_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20379, -16, -16, 16, 16, 1, height, 0, 0, height);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 72, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20380, -16, -16, 32, 32, 1, height, -16, -16, height);
            wooden_b_supports_paint_setup(session, 8, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            wooden_b_supports_paint_setup(session, 9, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            wooden_b_supports_paint_setup(session, 10, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            wooden_b_supports_paint_setup(session, 11, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 72, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            wooden_b_supports_paint_setup(session, 10, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            wooden_b_supports_paint_setup(session, 11, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20378, -16, -16, 32, 32, 1, height, -16, -16, height);
            wooden_b_supports_paint_setup(session, 8, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            wooden_b_supports_paint_setup(session, 9, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 72, 0x20);
        break;
    case 3:
        switch (direction) {
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20381, -16, -16, 32, 32, 1, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 72, 0x20);
        break;
    }
}

/** rct2: 0x0071C494 */
static void mine_train_rc_track_diag_60_deg_down_to_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20375, -16, -16, 32, 32, 1, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 72, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20376, -16, -16, 32, 32, 1, height, -16, -16, height);
            wooden_b_supports_paint_setup(session, 8, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            wooden_b_supports_paint_setup(session, 9, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            wooden_b_supports_paint_setup(session, 10, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            wooden_b_supports_paint_setup(session, 11, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 72, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            wooden_b_supports_paint_setup(session, 10, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            wooden_b_supports_paint_setup(session, 11, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20374, -16, -16, 32, 32, 1, height, -16, -16, height);
            wooden_b_supports_paint_setup(session, 8, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            wooden_b_supports_paint_setup(session, 9, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 72, 0x20);
        break;
    case 3:
        switch (direction) {
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20377, -16, -16, 32, 32, 1, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 72, 0x20);
        break;
    }
}

/** rct2: 0x0071C434 */
static void mine_train_rc_track_diag_25_deg_down_to_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 3:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20391, -16, -16, 32, 32, 1, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 3:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20363, -16, -16, 32, 32, 1, height, -16, -16, height);
                break;
            }
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 48, 0x20);
        break;
    case 1:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 0:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20392, -16, -16, 32, 32, 1, height, -16, -16, height);
                wooden_a_supports_paint_setup(session, 8, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 1:
                wooden_a_supports_paint_setup(session, 9, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 2:
                wooden_a_supports_paint_setup(session, 10, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 3:
                wooden_a_supports_paint_setup(session, 11, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20364, -16, -16, 32, 32, 1, height, -16, -16, height);
                wooden_a_supports_paint_setup(session, 8, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 1:
                wooden_a_supports_paint_setup(session, 9, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 2:
                wooden_a_supports_paint_setup(session, 10, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 3:
                wooden_a_supports_paint_setup(session, 11, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            }
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 48, 0x20);
        break;
    case 2:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 0:
                wooden_a_supports_paint_setup(session, 10, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 1:
                wooden_a_supports_paint_setup(session, 11, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 2:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20390, -16, -16, 32, 32, 1, height, -16, -16, height);
                wooden_a_supports_paint_setup(session, 8, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 3:
                wooden_a_supports_paint_setup(session, 9, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                wooden_a_supports_paint_setup(session, 10, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 1:
                wooden_a_supports_paint_setup(session, 11, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 2:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20362, -16, -16, 32, 32, 1, height, -16, -16, height);
                wooden_a_supports_paint_setup(session, 8, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 3:
                wooden_a_supports_paint_setup(session, 9, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            }
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 48, 0x20);
        break;
    case 3:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 1:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20393, -16, -16, 32, 32, 1, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 1:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20365, -16, -16, 32, 32, 1, height, -16, -16, height);
                break;
            }
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 48, 0x20);
        break;
    }
}

/** rct2: 0x0071C4D4 */
static void mine_train_rc_track_diag_flat_to_left_bank(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20409, -16, -16, 32, 32, 1, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20406, -16, -16, 32, 32, 1, height, -16, -16, height);
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20410, -16, -16, 32, 32, 0, height, -16, -16, height + 27);
            wooden_a_supports_paint_setup(session, 8, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            wooden_a_supports_paint_setup(session, 9, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            wooden_a_supports_paint_setup(session, 10, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            wooden_a_supports_paint_setup(session, 11, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            wooden_a_supports_paint_setup(session, 10, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            wooden_a_supports_paint_setup(session, 11, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20408, -16, -16, 32, 32, 1, height, -16, -16, height);
            wooden_a_supports_paint_setup(session, 8, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            wooden_a_supports_paint_setup(session, 9, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 3:
        switch (direction) {
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20407, -16, -16, 32, 32, 1, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    }
}

/** rct2: 0x0071C4B4 */
static void mine_train_rc_track_diag_flat_to_right_bank(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20414, -16, -16, 32, 32, 1, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20411, -16, -16, 32, 32, 1, height, -16, -16, height);
            wooden_a_supports_paint_setup(session, 8, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            wooden_a_supports_paint_setup(session, 9, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            wooden_a_supports_paint_setup(session, 10, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            wooden_a_supports_paint_setup(session, 11, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            wooden_a_supports_paint_setup(session, 10, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            wooden_a_supports_paint_setup(session, 11, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20413, -16, -16, 32, 32, 1, height, -16, -16, height);
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20415, -16, -16, 32, 32, 0, height, -16, -16, height + 27);
            wooden_a_supports_paint_setup(session, 8, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            wooden_a_supports_paint_setup(session, 9, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 3:
        switch (direction) {
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20412, -16, -16, 32, 32, 1, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    }
}

/** rct2: 0x0071C4C4 */
static void mine_train_rc_track_diag_left_bank_to_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20412, -16, -16, 32, 32, 1, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20413, -16, -16, 32, 32, 1, height, -16, -16, height);
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20415, -16, -16, 32, 32, 0, height, -16, -16, height + 27);
            wooden_a_supports_paint_setup(session, 8, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            wooden_a_supports_paint_setup(session, 9, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            wooden_a_supports_paint_setup(session, 10, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            wooden_a_supports_paint_setup(session, 11, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            wooden_a_supports_paint_setup(session, 10, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            wooden_a_supports_paint_setup(session, 11, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20411, -16, -16, 32, 32, 1, height, -16, -16, height);
            wooden_a_supports_paint_setup(session, 8, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            wooden_a_supports_paint_setup(session, 9, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 3:
        switch (direction) {
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20414, -16, -16, 32, 32, 1, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    }
}

/** rct2: 0x0071C4E4 */
static void mine_train_rc_track_diag_right_bank_to_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20407, -16, -16, 32, 32, 1, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20408, -16, -16, 32, 32, 1, height, -16, -16, height);
            wooden_a_supports_paint_setup(session, 8, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            wooden_a_supports_paint_setup(session, 9, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            wooden_a_supports_paint_setup(session, 10, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            wooden_a_supports_paint_setup(session, 11, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            wooden_a_supports_paint_setup(session, 10, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            wooden_a_supports_paint_setup(session, 11, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20406, -16, -16, 32, 32, 1, height, -16, -16, height);
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20410, -16, -16, 32, 32, 0, height, -16, -16, height + 27);
            wooden_a_supports_paint_setup(session, 8, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            wooden_a_supports_paint_setup(session, 9, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 3:
        switch (direction) {
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20409, -16, -16, 32, 32, 1, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    }
}

/** rct2: 0x0071C514 */
static void mine_train_rc_track_diag_left_bank_to_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20429, -16, -16, 32, 32, 1, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 48, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20426, -16, -16, 32, 32, 1, height, -16, -16, height);
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20430, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
            wooden_a_supports_paint_setup(session, 8, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            wooden_a_supports_paint_setup(session, 9, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            wooden_a_supports_paint_setup(session, 10, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            wooden_a_supports_paint_setup(session, 11, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 48, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            wooden_a_supports_paint_setup(session, 10, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            wooden_a_supports_paint_setup(session, 11, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20428, -16, -16, 32, 32, 1, height, -16, -16, height);
            wooden_a_supports_paint_setup(session, 8, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            wooden_a_supports_paint_setup(session, 9, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 48, 0x20);
        break;
    case 3:
        switch (direction) {
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20427, -16, -16, 32, 32, 1, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 48, 0x20);
        break;
    }
}

/** rct2: 0x0071C524 */
static void mine_train_rc_track_diag_right_bank_to_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20434, -16, -16, 32, 32, 1, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 48, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20431, -16, -16, 32, 32, 1, height, -16, -16, height);
            wooden_a_supports_paint_setup(session, 8, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            wooden_a_supports_paint_setup(session, 9, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            wooden_a_supports_paint_setup(session, 10, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            wooden_a_supports_paint_setup(session, 11, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 48, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            wooden_a_supports_paint_setup(session, 10, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            wooden_a_supports_paint_setup(session, 11, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20433, -16, -16, 32, 32, 1, height, -16, -16, height);
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20435, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
            wooden_a_supports_paint_setup(session, 8, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            wooden_a_supports_paint_setup(session, 9, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 48, 0x20);
        break;
    case 3:
        switch (direction) {
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20432, -16, -16, 32, 32, 1, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 48, 0x20);
        break;
    }
}

/** rct2: 0x0071C4F4 */
static void mine_train_rc_track_diag_25_deg_up_to_left_bank(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20419, -16, -16, 32, 32, 1, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 56, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20416, -16, -16, 32, 32, 1, height, -16, -16, height);
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20420, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
            wooden_b_supports_paint_setup(session, 8, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            wooden_b_supports_paint_setup(session, 9, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            wooden_b_supports_paint_setup(session, 10, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            wooden_b_supports_paint_setup(session, 11, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 56, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            wooden_b_supports_paint_setup(session, 10, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            wooden_b_supports_paint_setup(session, 11, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20418, -16, -16, 32, 32, 1, height, -16, -16, height);
            wooden_b_supports_paint_setup(session, 8, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            wooden_b_supports_paint_setup(session, 9, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 56, 0x20);
        break;
    case 3:
        switch (direction) {
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20417, -16, -16, 32, 32, 1, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 56, 0x20);
        break;
    }
}

/** rct2: 0x0071C504 */
static void mine_train_rc_track_diag_25_deg_up_to_right_bank(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20424, -16, -16, 32, 32, 1, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 56, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20421, -16, -16, 32, 32, 1, height, -16, -16, height);
            wooden_b_supports_paint_setup(session, 8, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            wooden_b_supports_paint_setup(session, 9, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            wooden_b_supports_paint_setup(session, 10, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            wooden_b_supports_paint_setup(session, 11, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 56, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            wooden_b_supports_paint_setup(session, 10, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            wooden_b_supports_paint_setup(session, 11, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20423, -16, -16, 32, 32, 1, height, -16, -16, height);
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20425, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
            wooden_b_supports_paint_setup(session, 8, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            wooden_b_supports_paint_setup(session, 9, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 56, 0x20);
        break;
    case 3:
        switch (direction) {
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20422, -16, -16, 32, 32, 1, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 56, 0x20);
        break;
    }
}

/** rct2: 0x0071C534 */
static void mine_train_rc_track_diag_left_bank_to_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20422, -16, -16, 32, 32, 1, height, -16, -16, height);
            break;
        }
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20423, -16, -16, 32, 32, 1, height, -16, -16, height);
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20425, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
            wooden_b_supports_paint_setup(session, 8, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            wooden_b_supports_paint_setup(session, 9, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            wooden_b_supports_paint_setup(session, 10, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            wooden_b_supports_paint_setup(session, 11, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        break;
    case 2:
        switch (direction) {
        case 0:
            wooden_b_supports_paint_setup(session, 10, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            wooden_b_supports_paint_setup(session, 11, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20421, -16, -16, 32, 32, 1, height, -16, -16, height);
            wooden_b_supports_paint_setup(session, 8, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            wooden_b_supports_paint_setup(session, 9, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        break;
    case 3:
        switch (direction) {
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20424, -16, -16, 32, 32, 1, height, -16, -16, height);
            break;
        }
        break;
    }

    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 56, 0x20);
}

/** rct2: 0x0071C544 */
static void mine_train_rc_track_diag_right_bank_to_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20417, -16, -16, 32, 32, 1, height, -16, -16, height);
            break;
        }
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20418, -16, -16, 32, 32, 1, height, -16, -16, height);
            wooden_b_supports_paint_setup(session, 8, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            wooden_b_supports_paint_setup(session, 9, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            wooden_b_supports_paint_setup(session, 10, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            wooden_b_supports_paint_setup(session, 11, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        break;
    case 2:
        switch (direction) {
        case 0:
            wooden_b_supports_paint_setup(session, 10, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            wooden_b_supports_paint_setup(session, 11, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20416, -16, -16, 32, 32, 1, height, -16, -16, height);
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20420, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
            wooden_b_supports_paint_setup(session, 8, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            wooden_b_supports_paint_setup(session, 9, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        break;
    case 3:
        switch (direction) {
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20419, -16, -16, 32, 32, 1, height, -16, -16, height);
            break;
        }
        break;
    }

    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 56, 0x20);
}

/** rct2: 0x0071C554 */
static void mine_train_rc_track_diag_25_deg_down_to_left_bank(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20432, -16, -16, 32, 32, 1, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 48, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20433, -16, -16, 32, 32, 1, height, -16, -16, height);
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20435, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
            wooden_a_supports_paint_setup(session, 8, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            wooden_a_supports_paint_setup(session, 9, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            wooden_a_supports_paint_setup(session, 10, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            wooden_a_supports_paint_setup(session, 11, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 48, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            wooden_a_supports_paint_setup(session, 10, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            wooden_a_supports_paint_setup(session, 11, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20431, -16, -16, 32, 32, 1, height, -16, -16, height);
            wooden_a_supports_paint_setup(session, 8, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            wooden_a_supports_paint_setup(session, 9, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 48, 0x20);
        break;
    case 3:
        switch (direction) {
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20434, -16, -16, 32, 32, 1, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 48, 0x20);
        break;
    }
}

/** rct2: 0x0071C564 */
static void mine_train_rc_track_diag_25_deg_down_to_right_bank(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20427, -16, -16, 32, 32, 1, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 48, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20428, -16, -16, 32, 32, 1, height, -16, -16, height);
            wooden_a_supports_paint_setup(session, 8, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            wooden_a_supports_paint_setup(session, 9, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            wooden_a_supports_paint_setup(session, 10, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            wooden_a_supports_paint_setup(session, 11, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 48, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            wooden_a_supports_paint_setup(session, 10, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            wooden_a_supports_paint_setup(session, 11, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20426, -16, -16, 32, 32, 1, height, -16, -16, height);
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20430, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
            wooden_a_supports_paint_setup(session, 8, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            wooden_a_supports_paint_setup(session, 9, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 48, 0x20);
        break;
    case 3:
        switch (direction) {
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20429, -16, -16, 32, 32, 1, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 48, 0x20);
        break;
    }
}

/** rct2: 0x0071C3D4 */
static void mine_train_rc_track_diag_left_bank(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20405, -16, -16, 32, 32, 3, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20402, -16, -16, 32, 32, 0, height, -16, -16, height + 27);
            wooden_a_supports_paint_setup(session, 8, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            wooden_a_supports_paint_setup(session, 9, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            wooden_a_supports_paint_setup(session, 10, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            wooden_a_supports_paint_setup(session, 11, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            wooden_a_supports_paint_setup(session, 10, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            wooden_a_supports_paint_setup(session, 11, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20404, -16, -16, 32, 32, 3, height, -16, -16, height);
            wooden_a_supports_paint_setup(session, 8, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            wooden_a_supports_paint_setup(session, 9, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 3:
        switch (direction) {
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20403, -16, -16, 32, 32, 3, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    }
}

/** rct2: 0x0071C3E4 */
static void mine_train_rc_track_diag_right_bank(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20403, -16, -16, 32, 32, 3, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20404, -16, -16, 32, 32, 3, height, -16, -16, height);
            wooden_a_supports_paint_setup(session, 8, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            wooden_a_supports_paint_setup(session, 9, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            wooden_a_supports_paint_setup(session, 10, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            wooden_a_supports_paint_setup(session, 11, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            wooden_a_supports_paint_setup(session, 10, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            wooden_a_supports_paint_setup(session, 11, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20402, -16, -16, 32, 32, 0, height, -16, -16, height + 27);
            wooden_a_supports_paint_setup(session, 8, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            wooden_a_supports_paint_setup(session, 9, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 3:
        switch (direction) {
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20405, -16, -16, 32, 32, 3, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    }
}

/** rct2: 0x0071C574 */
static void mine_train_rc_track_block_brakes(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
    case 2:
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20060, 0, 0, 32, 20, 1, height, 0, 6, height);
        wooden_a_supports_paint_setup(session, 6, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    case 1:
    case 3:
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 20061, 0, 0, 32, 20, 1, height, 0, 6, height);
        wooden_a_supports_paint_setup(session, 7, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    }
    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_6);
    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

TRACK_PAINT_FUNCTION get_track_paint_function_mine_train_rc(sint32 trackType, sint32 direction)
{
    switch (trackType) {
    case TRACK_ELEM_FLAT:
        return mine_train_rc_track_flat;
    case TRACK_ELEM_END_STATION:
    case TRACK_ELEM_BEGIN_STATION:
    case TRACK_ELEM_MIDDLE_STATION:
        return mine_train_rc_track_station;
    case TRACK_ELEM_25_DEG_UP:
        return mine_train_rc_track_25_deg_up;
    case TRACK_ELEM_60_DEG_UP:
        return mine_train_rc_track_60_deg_up;
    case TRACK_ELEM_FLAT_TO_25_DEG_UP:
        return mine_train_rc_track_flat_to_25_deg_up;
    case TRACK_ELEM_25_DEG_UP_TO_60_DEG_UP:
        return mine_train_rc_track_25_deg_up_to_60_deg_up;
    case TRACK_ELEM_60_DEG_UP_TO_25_DEG_UP:
        return mine_train_rc_track_60_deg_up_to_25_deg_up;
    case TRACK_ELEM_25_DEG_UP_TO_FLAT:
        return mine_train_rc_track_25_deg_up_to_flat;
    case TRACK_ELEM_25_DEG_DOWN:
        return mine_train_rc_track_25_deg_down;
    case TRACK_ELEM_60_DEG_DOWN:
        return mine_train_rc_track_60_deg_down;
    case TRACK_ELEM_FLAT_TO_25_DEG_DOWN:
        return mine_train_rc_track_flat_to_25_deg_down;
    case TRACK_ELEM_25_DEG_DOWN_TO_60_DEG_DOWN:
        return mine_train_rc_track_25_deg_down_to_60_deg_down;
    case TRACK_ELEM_60_DEG_DOWN_TO_25_DEG_DOWN:
        return mine_train_rc_track_60_deg_down_to_25_deg_down;
    case TRACK_ELEM_25_DEG_DOWN_TO_FLAT:
        return mine_train_rc_track_25_deg_down_to_flat;
    case TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES:
        return mine_train_rc_track_left_quarter_turn_5;
    case TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES:
        return mine_train_rc_track_right_quarter_turn_5;
    case TRACK_ELEM_FLAT_TO_LEFT_BANK:
        return mine_train_rc_track_flat_to_left_bank;
    case TRACK_ELEM_FLAT_TO_RIGHT_BANK:
        return mine_train_rc_track_flat_to_right_bank;
    case TRACK_ELEM_LEFT_BANK_TO_FLAT:
        return mine_train_rc_track_left_bank_to_flat;
    case TRACK_ELEM_RIGHT_BANK_TO_FLAT:
        return mine_train_rc_track_right_bank_to_flat;
    case TRACK_ELEM_BANKED_LEFT_QUARTER_TURN_5_TILES:
        return mine_train_rc_track_banked_left_quarter_turn_5;
    case TRACK_ELEM_BANKED_RIGHT_QUARTER_TURN_5_TILES:
        return mine_train_rc_track_banked_right_quarter_turn_5;
    case TRACK_ELEM_LEFT_BANK_TO_25_DEG_UP:
        return mine_train_rc_track_left_bank_to_25_deg_up;
    case TRACK_ELEM_RIGHT_BANK_TO_25_DEG_UP:
        return mine_train_rc_track_right_bank_to_25_deg_up;
    case TRACK_ELEM_25_DEG_UP_TO_LEFT_BANK:
        return mine_train_rc_track_25_deg_up_to_left_bank;
    case TRACK_ELEM_25_DEG_UP_TO_RIGHT_BANK:
        return mine_train_rc_track_25_deg_up_to_right_bank;
    case TRACK_ELEM_LEFT_BANK_TO_25_DEG_DOWN:
        return mine_train_rc_track_left_bank_to_25_deg_down;
    case TRACK_ELEM_RIGHT_BANK_TO_25_DEG_DOWN:
        return mine_train_rc_track_right_bank_to_25_deg_down;
    case TRACK_ELEM_25_DEG_DOWN_TO_LEFT_BANK:
        return mine_train_rc_track_25_deg_down_to_left_bank;
    case TRACK_ELEM_25_DEG_DOWN_TO_RIGHT_BANK:
        return mine_train_rc_track_25_deg_down_to_right_bank;
    case TRACK_ELEM_LEFT_BANK:
        return mine_train_rc_track_left_bank;
    case TRACK_ELEM_RIGHT_BANK:
        return mine_train_rc_track_right_bank;
    case TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES_25_DEG_UP:
        return mine_train_rc_track_left_quarter_turn_5_25_deg_up;
    case TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES_25_DEG_UP:
        return mine_train_rc_track_right_quarter_turn_5_25_deg_up;
    case TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES_25_DEG_DOWN:
        return mine_train_rc_track_left_quarter_turn_5_25_deg_down;
    case TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES_25_DEG_DOWN:
        return mine_train_rc_track_right_quarter_turn_5_25_deg_down;
    case TRACK_ELEM_S_BEND_LEFT:
        return mine_train_rc_track_s_bend_left;
    case TRACK_ELEM_S_BEND_RIGHT:
        return mine_train_rc_track_s_bend_right;
    case TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES:
        return mine_train_rc_track_left_quarter_turn_3;
    case TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES:
        return mine_train_rc_track_right_quarter_turn_3;
    case TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_BANK:
        return mine_train_rc_track_left_quarter_turn_3_bank;
    case TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_BANK:
        return mine_train_rc_track_right_quarter_turn_3_bank;
    case TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_25_DEG_UP:
        return mine_train_rc_track_left_quarter_turn_3_25_deg_up;
    case TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_25_DEG_UP:
        return mine_train_rc_track_right_quarter_turn_3_25_deg_up;
    case TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_25_DEG_DOWN:
        return mine_train_rc_track_left_quarter_turn_3_25_deg_down;
    case TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_25_DEG_DOWN:
        return mine_train_rc_track_right_quarter_turn_3_25_deg_down;
    case TRACK_ELEM_LEFT_HALF_BANKED_HELIX_UP_SMALL:
        return mine_train_rc_track_left_half_banked_helix_up_small;
    case TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_UP_SMALL:
        return mine_train_rc_track_right_half_banked_helix_up_small;
    case TRACK_ELEM_LEFT_HALF_BANKED_HELIX_DOWN_SMALL:
        return mine_train_rc_track_left_half_banked_helix_down_small;
    case TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_DOWN_SMALL:
        return mine_train_rc_track_right_half_banked_helix_down_small;
    case TRACK_ELEM_LEFT_HALF_BANKED_HELIX_UP_LARGE:
        return mine_train_rc_track_left_half_banked_helix_up_large;
    case TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_UP_LARGE:
        return mine_train_rc_track_right_half_banked_helix_up_large;
    case TRACK_ELEM_LEFT_HALF_BANKED_HELIX_DOWN_LARGE:
        return mine_train_rc_track_left_half_banked_helix_down_large;
    case TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_DOWN_LARGE:
        return mine_train_rc_track_right_half_banked_helix_down_large;
    case TRACK_ELEM_BRAKES:
        return mine_train_rc_track_brakes;
    case TRACK_ELEM_ON_RIDE_PHOTO:
        return mine_train_rc_track_on_ride_photo;
    case TRACK_ELEM_LEFT_EIGHTH_TO_DIAG:
        return mine_train_rc_track_left_eighth_to_diag;
    case TRACK_ELEM_RIGHT_EIGHTH_TO_DIAG:
        return mine_train_rc_track_right_eighth_to_diag;
    case TRACK_ELEM_LEFT_EIGHTH_TO_ORTHOGONAL:
        return mine_train_rc_track_left_eighth_to_orthogonal;
    case TRACK_ELEM_RIGHT_EIGHTH_TO_ORTHOGONAL:
        return mine_train_rc_track_right_eighth_to_orthogonal;
    case TRACK_ELEM_LEFT_EIGHTH_BANK_TO_DIAG:
        return mine_train_rc_track_left_eighth_bank_to_diag;
    case TRACK_ELEM_RIGHT_EIGHTH_BANK_TO_DIAG:
        return mine_train_rc_track_right_eighth_bank_to_diag;
    case TRACK_ELEM_LEFT_EIGHTH_BANK_TO_ORTHOGONAL:
        return mine_train_rc_track_left_eighth_bank_to_orthogonal;
    case TRACK_ELEM_RIGHT_EIGHTH_BANK_TO_ORTHOGONAL:
        return mine_train_rc_track_right_eighth_bank_to_orthogonal;
    case TRACK_ELEM_DIAG_FLAT:
        return mine_train_rc_track_diag_flat;
    case TRACK_ELEM_DIAG_25_DEG_UP:
        return mine_train_rc_track_diag_25_deg_up;
    case TRACK_ELEM_DIAG_60_DEG_UP:
        return mine_train_rc_track_diag_60_deg_up;
    case TRACK_ELEM_DIAG_FLAT_TO_25_DEG_UP:
        return mine_train_rc_track_diag_flat_to_25_deg_up;
    case TRACK_ELEM_DIAG_25_DEG_UP_TO_60_DEG_UP:
        return mine_train_rc_track_diag_25_deg_up_to_60_deg_up;
    case TRACK_ELEM_DIAG_60_DEG_UP_TO_25_DEG_UP:
        return mine_train_rc_track_diag_60_deg_up_to_25_deg_up;
    case TRACK_ELEM_DIAG_25_DEG_UP_TO_FLAT:
        return mine_train_rc_track_diag_25_deg_up_to_flat;
    case TRACK_ELEM_DIAG_25_DEG_DOWN:
        return mine_train_rc_track_diag_25_deg_down;
    case TRACK_ELEM_DIAG_60_DEG_DOWN:
        return mine_train_rc_track_diag_60_deg_down;
    case TRACK_ELEM_DIAG_FLAT_TO_25_DEG_DOWN:
        return mine_train_rc_track_diag_flat_to_25_deg_down;
    case TRACK_ELEM_DIAG_25_DEG_DOWN_TO_60_DEG_DOWN:
        return mine_train_rc_track_diag_25_deg_down_to_60_deg_down;
    case TRACK_ELEM_DIAG_60_DEG_DOWN_TO_25_DEG_DOWN:
        return mine_train_rc_track_diag_60_deg_down_to_25_deg_down;
    case TRACK_ELEM_DIAG_25_DEG_DOWN_TO_FLAT:
        return mine_train_rc_track_diag_25_deg_down_to_flat;
    case TRACK_ELEM_DIAG_FLAT_TO_LEFT_BANK:
        return mine_train_rc_track_diag_flat_to_left_bank;
    case TRACK_ELEM_DIAG_FLAT_TO_RIGHT_BANK:
        return mine_train_rc_track_diag_flat_to_right_bank;
    case TRACK_ELEM_DIAG_LEFT_BANK_TO_FLAT:
        return mine_train_rc_track_diag_left_bank_to_flat;
    case TRACK_ELEM_DIAG_RIGHT_BANK_TO_FLAT:
        return mine_train_rc_track_diag_right_bank_to_flat;
    case TRACK_ELEM_DIAG_LEFT_BANK_TO_25_DEG_UP:
        return mine_train_rc_track_diag_left_bank_to_25_deg_up;
    case TRACK_ELEM_DIAG_RIGHT_BANK_TO_25_DEG_UP:
        return mine_train_rc_track_diag_right_bank_to_25_deg_up;
    case TRACK_ELEM_DIAG_25_DEG_UP_TO_LEFT_BANK:
        return mine_train_rc_track_diag_25_deg_up_to_left_bank;
    case TRACK_ELEM_DIAG_25_DEG_UP_TO_RIGHT_BANK:
        return mine_train_rc_track_diag_25_deg_up_to_right_bank;
    case TRACK_ELEM_DIAG_LEFT_BANK_TO_25_DEG_DOWN:
        return mine_train_rc_track_diag_left_bank_to_25_deg_down;
    case TRACK_ELEM_DIAG_RIGHT_BANK_TO_25_DEG_DOWN:
        return mine_train_rc_track_diag_right_bank_to_25_deg_down;
    case TRACK_ELEM_DIAG_25_DEG_DOWN_TO_LEFT_BANK:
        return mine_train_rc_track_diag_25_deg_down_to_left_bank;
    case TRACK_ELEM_DIAG_25_DEG_DOWN_TO_RIGHT_BANK:
        return mine_train_rc_track_diag_25_deg_down_to_right_bank;
    case TRACK_ELEM_DIAG_LEFT_BANK:
        return mine_train_rc_track_diag_left_bank;
    case TRACK_ELEM_DIAG_RIGHT_BANK:
        return mine_train_rc_track_diag_right_bank;
    case TRACK_ELEM_BLOCK_BRAKES:
        return mine_train_rc_track_block_brakes;
    }
    return NULL;
}
