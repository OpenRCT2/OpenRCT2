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

/** rct2: 0x008A59A8 */
static void steeplechase_track_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    if (track_element_is_lift_hill(mapElement)) {
        switch (direction) {
        case 0:
        case 2:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28635, 0, 6, 32, 20, 3, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
        case 3:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28636, 0, 6, 32, 20, 3, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK_ALT, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }
    } else {
        switch (direction) {
        case 0:
        case 2:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28633, 0, 6, 32, 20, 3, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
        case 3:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28634, 0, 6, 32, 20, 3, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK_ALT, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }
    }
    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
    paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

static void steeplechase_track_station(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    static const uint32 imageIds[4][3] = {
        { 28635, SPR_STATION_BASE_B_SW_NE },
        { 28636, SPR_STATION_BASE_B_NW_SE },
        { 28635, SPR_STATION_BASE_B_SW_NE },
        { 28636, SPR_STATION_BASE_B_NW_SE },
    };

    sub_98197C_rotated(session, direction, imageIds[direction][1] | gTrackColours[SCHEME_MISC], 0, 0, 32, 28, 3, height - 2, 0, 2, height);
    sub_98199C_rotated(session, direction, imageIds[direction][0] | gTrackColours[SCHEME_TRACK], 0, 6, 32, 20, 3, height, 0, 0, height);
    track_paint_util_draw_station_metal_supports_2(session, direction, height, gTrackColours[SCHEME_SUPPORTS], 3);
    track_paint_util_draw_station(session, rideIndex, trackSequence, direction, height, mapElement);
    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_6);
    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x008A59B8 */
static void steeplechase_track_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    if (track_element_is_lift_hill(mapElement)) {
        switch (direction) {
        case 0:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28649, 0, 6, 32, 20, 3, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28655, 0, 6, 32, 20, 3, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK_ALT, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28651, 0, 6, 32, 20, 3, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28656, 0, 6, 32, 20, 3, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK_ALT, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }
    } else {
        switch (direction) {
        case 0:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28637, 0, 6, 32, 20, 3, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28643, 0, 6, 32, 20, 3, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK_ALT, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28639, 0, 6, 32, 20, 3, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28644, 0, 6, 32, 20, 3, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK_ALT, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }
    }
    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_1);
    } else {
        paint_util_push_tunnel_rotated(session, direction, height + 8, TUNNEL_2);
    }
    paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 56, 0x20);
}

/** rct2: 0x008A59C8 */
static void steeplechase_track_flat_to_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    if (track_element_is_lift_hill(mapElement)) {
        switch (direction) {
        case 0:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28650, 0, 6, 32, 20, 3, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK, 4, 3, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28657, 0, 6, 32, 20, 3, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK_ALT, 4, 2, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28652, 0, 6, 32, 20, 3, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK, 4, 2, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28658, 0, 6, 32, 20, 3, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK_ALT, 4, 2, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }
    } else {
        switch (direction) {
        case 0:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28638, 0, 6, 32, 20, 3, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK, 4, 3, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28645, 0, 6, 32, 20, 3, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK_ALT, 4, 2, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28640, 0, 6, 32, 20, 3, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK, 4, 2, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28646, 0, 6, 32, 20, 3, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK_ALT, 4, 2, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }
    }
    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
    } else {
        paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_2);
    }
    paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 48, 0x20);
}

/** rct2: 0x008A59D8 */
static void steeplechase_track_25_deg_up_to_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    if (track_element_is_lift_hill(mapElement)) {
        switch (direction) {
        case 0:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28654, 0, 6, 32, 20, 3, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28660, 0, 6, 32, 20, 3, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK_ALT, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28653, 0, 6, 32, 20, 3, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK, 4, 5, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28659, 0, 6, 32, 20, 3, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK_ALT, 4, 5, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }
    } else {
        switch (direction) {
        case 0:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28642, 0, 6, 32, 20, 3, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28648, 0, 6, 32, 20, 3, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK_ALT, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28641, 0, 6, 32, 20, 3, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK, 4, 5, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28647, 0, 6, 32, 20, 3, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK_ALT, 4, 5, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }
    }
    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_0);
    } else {
        paint_util_push_tunnel_rotated(session, direction, height + 8, TUNNEL_12);
    }
    paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 40, 0x20);
}

/** rct2: 0x008A59E8 */
static void steeplechase_track_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    steeplechase_track_25_deg_up(session, rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008A59F8 */
static void steeplechase_track_flat_to_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    steeplechase_track_25_deg_up_to_flat(session, rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008A5A08 */
static void steeplechase_track_25_deg_down_to_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    steeplechase_track_flat_to_25_deg_up(session, rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008A5A18 */
static void steeplechase_track_left_quarter_turn_5(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28698, 0, 6, 32, 20, 3, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK, 4, 1, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28703, 0, 6, 32, 20, 3, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK_ALT, 4, 1, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28708, 0, 6, 32, 20, 3, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK, 4, 0, height - 3, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28693, 0, 6, 32, 20, 3, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK_ALT, 4, 0, height - 1, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 1:
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28697, 0, 0, 32, 16, 3, height);
            break;
        case 1:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28702, 0, 0, 32, 16, 3, height);
            break;
        case 2:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28707, 0, 16, 32, 16, 3, height);
            break;
        case 3:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28692, 0, 16, 32, 16, 3, height);
            break;
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28696, 0, 16, 16, 16, 3, height);
            break;
        case 1:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28701, 16, 16, 16, 16, 3, height);
            break;
        case 2:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28706, 16, 0, 16, 16, 3, height);
            break;
        case 3:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28691, 0, 0, 16, 16, 3, height);
            break;
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 4:
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 5:
        switch (direction) {
        case 0:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28695, 16, 0, 16, 32, 3, height);
            break;
        case 1:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28700, 0, 0, 16, 32, 3, height);
            break;
        case 2:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28705, 0, 0, 16, 32, 3, height);
            break;
        case 3:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28690, 16, 0, 16, 32, 3, height);
            break;
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 6:
        switch (direction) {
        case 0:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28694, 6, 0, 20, 32, 3, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK_ALT, 4, 0, height - 3, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28699, 6, 0, 20, 32, 3, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28704, 6, 0, 20, 32, 3, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK_ALT, 4, 1, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28689, 6, 0, 20, 32, 3, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK, 4, 0, height - 2, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }
        switch (direction) {
        case 2:
            paint_util_push_tunnel_right(session, height, TUNNEL_0);
            break;
        case 3:
            paint_util_push_tunnel_left(session, height, TUNNEL_0);
            break;
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    }
}

/** rct2: 0x008A5A28 */
static void steeplechase_track_right_quarter_turn_5(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    steeplechase_track_left_quarter_turn_5(session, rideIndex, trackSequence, (direction - 1) & 3, height, mapElement);
}

/** rct2: 0x008A5A38 */
static void steeplechase_track_s_bend_left(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28665, 0, 6, 32, 20, 3, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28672, 0, 6, 32, 20, 3, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK_ALT, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28668, 0, 6, 32, 20, 3, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28669, 0, 6, 32, 20, 3, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK_ALT, 4, 0, height - 2, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28666, 0, 0, 32, 26, 3, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK, 5, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28671, 0, 0, 32, 26, 3, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK_ALT, 6, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28667, 0, 6, 32, 26, 3, height);
            break;
        case 3:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28670, 0, 6, 32, 26, 3, height);
            break;
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28667, 0, 6, 32, 26, 3, height);
            break;
        case 1:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28670, 0, 6, 32, 26, 3, height);
            break;
        case 2:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28666, 0, 0, 32, 26, 3, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK, 5, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28671, 0, 0, 32, 26, 3, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK_ALT, 6, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28668, 0, 6, 32, 20, 3, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28669, 0, 6, 32, 20, 3, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK_ALT, 4, 0, height - 2, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28665, 0, 6, 32, 20, 3, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28672, 0, 6, 32, 20, 3, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK_ALT, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }
        switch (direction) {
        case 1:
            paint_util_push_tunnel_right(session, height, TUNNEL_0);
            break;
        case 2:
            paint_util_push_tunnel_left(session, height, TUNNEL_0);
            break;
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    }
}

/** rct2: 0x008A5A48 */
static void steeplechase_track_s_bend_right(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28661, 0, 6, 32, 20, 3, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28676, 0, 6, 32, 20, 3, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK_ALT, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28664, 0, 6, 32, 20, 3, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28673, 0, 6, 32, 20, 3, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK_ALT, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28662, 0, 6, 32, 26, 3, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK, 8, 0, height - 2, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28675, 0, 6, 32, 26, 3, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK_ALT, 7, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28663, 0, 0, 32, 26, 3, height);
            break;
        case 3:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28674, 0, 0, 32, 26, 3, height);
            break;
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28663, 0, 0, 32, 26, 3, height);
            break;
        case 1:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28674, 0, 0, 32, 26, 3, height);
            break;
        case 2:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28662, 0, 6, 32, 26, 3, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK, 8, 0, height - 2, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28675, 0, 6, 32, 26, 3, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK_ALT, 7, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28664, 0, 6, 32, 20, 3, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28673, 0, 6, 32, 20, 3, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK_ALT, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28661, 0, 6, 32, 20, 3, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28676, 0, 6, 32, 20, 3, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK_ALT, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }
        switch (direction) {
        case 1:
            paint_util_push_tunnel_right(session, height, TUNNEL_0);
            break;
        case 2:
            paint_util_push_tunnel_left(session, height, TUNNEL_0);
            break;
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    }
}

/** rct2: 0x008A5A88 */
static void steeplechase_track_left_quarter_turn_3(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28682, 0, 6, 32, 20, 3, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK, 4, 3, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28685, 0, 6, 32, 20, 3, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK_ALT, 4, 1, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28688, 0, 6, 32, 20, 3, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28679, 0, 6, 32, 20, 3, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK_ALT, 4, 0, height - 3, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 1:
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28681, 16, 0, 16, 16, 3, height);
            break;
        case 1:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28684, 0, 0, 16, 16, 3, height);
            break;
        case 2:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28687, 0, 16, 16, 16, 3, height);
            break;
        case 3:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28678, 16, 16, 16, 16, 3, height);
            break;
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28680, 6, 0, 20, 32, 3, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK_ALT, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28683, 6, 0, 20, 32, 3, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK, 4, 1, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28686, 6, 0, 20, 32, 3, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK_ALT, 4, 3, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28677, 6, 0, 20, 32, 3, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK, 4, 0, height - 3, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }
        switch (direction) {
        case 2:
            paint_util_push_tunnel_right(session, height, TUNNEL_0);
            break;
        case 3:
            paint_util_push_tunnel_left(session, height, TUNNEL_0);
            break;
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    }
}

/** rct2: 0x008A5A98 */
static void steeplechase_track_right_quarter_turn_3(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    steeplechase_track_left_quarter_turn_3(session, rideIndex, trackSequence, (direction - 1) & 3, height, mapElement);
}

/** rct2: 0x008A5AA8 */
static void steeplechase_track_brakes(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
    case 2:
        sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28635, 0, 6, 32, 20, 3, height);
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
        break;
    case 1:
    case 3:
        sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28636, 0, 6, 32, 20, 3, height);
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK_ALT, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
        break;
    }
    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
    paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x008A5AD8 */
static void steeplechase_track_left_eighth_to_diag(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28725, 0, 0, 32, 20, 3, height, 0, 6, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28729, 0, 0, 32, 20, 3, height, 0, 6, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK_ALT, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28733, 0, 0, 32, 20, 3, height, 0, 6, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28737, 0, 0, 32, 20, 3, height, 0, 6, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK_ALT, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28726, 0, 0, 32, 16, 3, height, 0, 0, height);
            break;
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28730, 0, 0, 34, 16, 3, height, 0, 0, height);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28734, 0, 0, 32, 16, 3, height, 0, 16, height);
            break;
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28738, 0, 0, 32, 16, 3, height, 0, 16, height);
            break;
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28727, 0, 0, 16, 16, 3, height, 0, 16, height);
            break;
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28731, 0, 0, 16, 16, 3, height, 16, 16, height);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28735, 0, 0, 16, 16, 3, height, 16, 0, height);
            break;
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28739, 0, 0, 16, 16, 3, height, 0, 0, height);
            break;
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 3:
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 4:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28728, 0, 0, 16, 16, 3, height, 16, 16, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK, 3, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28732, 0, 0, 16, 18, 3, height, 0, 16, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK_ALT, 1, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28736, 0, 0, 16, 16, 3, height, 0, 0, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK, 0, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28740, 0, 0, 16, 16, 3, height, 16, 0, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK_ALT, 2, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    }
}

/** rct2: 0x008A5AE8 */
static void steeplechase_track_right_eighth_to_diag(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28709, 0, 0, 32, 20, 3, height, 0, 6, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28713, 0, 0, 32, 20, 3, height, 0, 6, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK_ALT, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28717, 0, 0, 32, 20, 3, height, 0, 6, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28721, 0, 0, 32, 20, 3, height, 0, 6, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK_ALT, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28710, 0, 0, 32, 16, 3, height, 0, 16, height);
            break;
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28714, 0, 0, 32, 16, 3, height, 0, 16, height);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28718, 0, 0, 34, 16, 3, height, 0, 0, height);
            break;
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28722, 0, 0, 32, 16, 3, height, 0, 0, height);
            break;
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28711, 0, 0, 16, 16, 3, height, 0, 0, height);
            break;
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28715, 0, 0, 16, 16, 3, height, 16, 0, height);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28719, 0, 0, 28, 28, 3, height, 4, 4, height);
            break;
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28723, 0, 0, 16, 16, 3, height, 0, 16, height);
            break;
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 3:
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 4:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28712, 0, 0, 16, 16, 3, height, 16, 0, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK, 1, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28716, 0, 0, 16, 16, 3, height, 0, 0, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK_ALT, 0, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28720, 0, 0, 16, 18, 3, height, 0, 16, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK, 2, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28724, 0, 0, 16, 16, 3, height, 16, 16, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK_ALT, 3, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    }
}

/** rct2: 0x008A5AF8 */
static void steeplechase_track_left_eighth_to_orthogonal(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    steeplechase_track_right_eighth_to_diag(session, rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008A5B08 */
static void steeplechase_track_right_eighth_to_orthogonal(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    steeplechase_track_left_eighth_to_diag(session, rideIndex, trackSequence, (direction + 3) & 3, height, mapElement);
}

/** rct2: 0x008A5AC8 */
static void steeplechase_track_diag_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 3:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28760, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 3:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28744, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 1:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 0:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28757, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28741, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 2:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 2:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28759, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 2:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28743, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 3:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 0:
                metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK, 1, 0, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28758, -16, -16, 32, 32, 3, height, -16, -16, height);
                metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK_ALT, 0, 0, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 2:
                metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK, 2, 0, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 3:
                metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK_ALT, 3, 0, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK, 1, 0, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28742, -16, -16, 32, 32, 3, height, -16, -16, height);
                metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK_ALT, 0, 0, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 2:
                metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK, 2, 0, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 3:
                metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK_ALT, 3, 0, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            }
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    }
}

/** rct2: 0x008A5B38 */
static void steeplechase_track_diag_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 3:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28772, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 3:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28756, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 56, 0x20);
        break;
    case 1:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 0:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28769, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28753, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 56, 0x20);
        break;
    case 2:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 2:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28771, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 2:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28755, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 56, 0x20);
        break;
    case 3:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 0:
                metal_b_supports_paint_setup(session, METAL_SUPPORTS_STICK, 1, 6, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28770, -16, -16, 32, 32, 3, height, -16, -16, height);
                metal_b_supports_paint_setup(session, METAL_SUPPORTS_STICK_ALT, 0, 6, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 2:
                metal_b_supports_paint_setup(session, METAL_SUPPORTS_STICK, 2, 6, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 3:
                metal_b_supports_paint_setup(session, METAL_SUPPORTS_STICK_ALT, 3, 6, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                metal_b_supports_paint_setup(session, METAL_SUPPORTS_STICK, 1, 6, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28754, -16, -16, 32, 32, 3, height, -16, -16, height);
                metal_b_supports_paint_setup(session, METAL_SUPPORTS_STICK_ALT, 0, 6, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 2:
                metal_b_supports_paint_setup(session, METAL_SUPPORTS_STICK, 2, 6, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 3:
                metal_b_supports_paint_setup(session, METAL_SUPPORTS_STICK_ALT, 3, 6, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            }
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 56, 0x20);
        break;
    }
}

/** rct2: 0x008A5B18 */
static void steeplechase_track_diag_flat_to_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 3:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28764, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 3:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28748, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 48, 0x20);
        break;
    case 1:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 0:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28761, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28745, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 48, 0x20);
        break;
    case 2:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 2:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28763, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 2:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28747, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 48, 0x20);
        break;
    case 3:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 0:
                metal_b_supports_paint_setup(session, METAL_SUPPORTS_STICK, 1, 0, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28762, -16, -16, 32, 32, 3, height, -16, -16, height);
                metal_b_supports_paint_setup(session, METAL_SUPPORTS_STICK_ALT, 0, 0, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 2:
                metal_b_supports_paint_setup(session, METAL_SUPPORTS_STICK, 2, 0, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 3:
                metal_b_supports_paint_setup(session, METAL_SUPPORTS_STICK_ALT, 3, 0, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                metal_b_supports_paint_setup(session, METAL_SUPPORTS_STICK, 1, 0, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28746, -16, -16, 32, 32, 3, height, -16, -16, height);
                metal_b_supports_paint_setup(session, METAL_SUPPORTS_STICK_ALT, 0, 0, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 2:
                metal_b_supports_paint_setup(session, METAL_SUPPORTS_STICK, 2, 0, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 3:
                metal_b_supports_paint_setup(session, METAL_SUPPORTS_STICK_ALT, 3, 0, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            }
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 48, 0x20);
        break;
    }
}

/** rct2: 0x008A5B28 */
static void steeplechase_track_diag_25_deg_up_to_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 3:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28768, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 3:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28752, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 56, 0x20);
        break;
    case 1:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 0:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28765, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28749, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 56, 0x20);
        break;
    case 2:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 2:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28767, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 2:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28751, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 56, 0x20);
        break;
    case 3:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 0:
                metal_b_supports_paint_setup(session, METAL_SUPPORTS_STICK, 1, 4, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28766, -16, -16, 32, 32, 3, height, -16, -16, height);
                metal_b_supports_paint_setup(session, METAL_SUPPORTS_STICK_ALT, 0, 4, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 2:
                metal_b_supports_paint_setup(session, METAL_SUPPORTS_STICK, 2, 4, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 3:
                metal_b_supports_paint_setup(session, METAL_SUPPORTS_STICK_ALT, 3, 4, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                metal_b_supports_paint_setup(session, METAL_SUPPORTS_STICK, 1, 4, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28750, -16, -16, 32, 32, 3, height, -16, -16, height);
                metal_b_supports_paint_setup(session, METAL_SUPPORTS_STICK_ALT, 0, 4, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 2:
                metal_b_supports_paint_setup(session, METAL_SUPPORTS_STICK, 2, 4, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 3:
                metal_b_supports_paint_setup(session, METAL_SUPPORTS_STICK_ALT, 3, 4, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            }
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 56, 0x20);
        break;
    }
}

/** rct2: 0x008A5B68 */
static void steeplechase_track_diag_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 3:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28770, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 3:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28754, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 56, 0x20);
        break;
    case 1:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 0:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28771, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28755, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 56, 0x20);
        break;
    case 2:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 2:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28769, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 2:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28753, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 56, 0x20);
        break;
    case 3:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 0:
                metal_b_supports_paint_setup(session, METAL_SUPPORTS_STICK, 1, 6, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28772, -16, -16, 32, 32, 3, height, -16, -16, height);
                metal_b_supports_paint_setup(session, METAL_SUPPORTS_STICK_ALT, 0, 6, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 2:
                metal_b_supports_paint_setup(session, METAL_SUPPORTS_STICK, 2, 6, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 3:
                metal_b_supports_paint_setup(session, METAL_SUPPORTS_STICK_ALT, 3, 6, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                metal_b_supports_paint_setup(session, METAL_SUPPORTS_STICK, 1, 6, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28756, -16, -16, 32, 32, 3, height, -16, -16, height);
                metal_b_supports_paint_setup(session, METAL_SUPPORTS_STICK_ALT, 0, 6, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 2:
                metal_b_supports_paint_setup(session, METAL_SUPPORTS_STICK, 2, 6, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 3:
                metal_b_supports_paint_setup(session, METAL_SUPPORTS_STICK_ALT, 3, 6, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            }
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 56, 0x20);
        break;
    }
}

/** rct2: 0x008A5B48 */
static void steeplechase_track_diag_flat_to_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 3:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28766, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 3:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28750, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        break;
    case 1:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 0:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28767, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28751, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        break;
    case 2:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 2:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28765, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 2:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28749, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        break;
    case 3:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 0:
                metal_b_supports_paint_setup(session, METAL_SUPPORTS_STICK, 1, 4, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28768, -16, -16, 32, 32, 3, height, -16, -16, height);
                metal_b_supports_paint_setup(session, METAL_SUPPORTS_STICK_ALT, 0, 4, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 2:
                metal_b_supports_paint_setup(session, METAL_SUPPORTS_STICK, 2, 4, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 3:
                metal_b_supports_paint_setup(session, METAL_SUPPORTS_STICK_ALT, 3, 4, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                metal_b_supports_paint_setup(session, METAL_SUPPORTS_STICK, 1, 4, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28752, -16, -16, 32, 32, 3, height, -16, -16, height);
                metal_b_supports_paint_setup(session, METAL_SUPPORTS_STICK_ALT, 0, 4, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 2:
                metal_b_supports_paint_setup(session, METAL_SUPPORTS_STICK, 2, 4, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 3:
                metal_b_supports_paint_setup(session, METAL_SUPPORTS_STICK_ALT, 3, 4, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            }
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        break;
    }

    paint_util_set_general_support_height(session, height + 56, 0x20);
}

/** rct2: 0x008A5B58 */
static void steeplechase_track_diag_25_deg_down_to_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 3:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28762, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 3:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28746, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 48, 0x20);
        break;
    case 1:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 0:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28763, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28747, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 48, 0x20);
        break;
    case 2:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 2:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28761, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 2:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28745, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 48, 0x20);
        break;
    case 3:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 0:
                metal_b_supports_paint_setup(session, METAL_SUPPORTS_STICK, 1, 0, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28764, -16, -16, 32, 32, 3, height, -16, -16, height);
                metal_b_supports_paint_setup(session, METAL_SUPPORTS_STICK_ALT, 0, 0, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 2:
                metal_b_supports_paint_setup(session, METAL_SUPPORTS_STICK, 2, 0, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 3:
                metal_b_supports_paint_setup(session, METAL_SUPPORTS_STICK_ALT, 3, 0, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                metal_b_supports_paint_setup(session, METAL_SUPPORTS_STICK, 1, 0, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
                sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28748, -16, -16, 32, 32, 3, height, -16, -16, height);
                metal_b_supports_paint_setup(session, METAL_SUPPORTS_STICK_ALT, 0, 0, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 2:
                metal_b_supports_paint_setup(session, METAL_SUPPORTS_STICK, 2, 0, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 3:
                metal_b_supports_paint_setup(session, METAL_SUPPORTS_STICK_ALT, 3, 0, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            }
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 48, 0x20);
        break;
    }
}

/** rct2: 0x008A5AB8 */
static void steeplechase_track_block_brakes(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
    case 2:
        sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28635, 0, 6, 32, 20, 3, height);
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
        break;
    case 1:
    case 3:
        sub_98196C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 28636, 0, 6, 32, 20, 3, height);
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK_ALT, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
        break;
    }
    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
    paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

TRACK_PAINT_FUNCTION get_track_paint_function_steeplechase(sint32 trackType, sint32 direction)
{
    switch (trackType) {
    case TRACK_ELEM_FLAT:
        return steeplechase_track_flat;
    case TRACK_ELEM_END_STATION:
    case TRACK_ELEM_BEGIN_STATION:
    case TRACK_ELEM_MIDDLE_STATION:
        return steeplechase_track_station;
    case TRACK_ELEM_25_DEG_UP:
        return steeplechase_track_25_deg_up;
    case TRACK_ELEM_FLAT_TO_25_DEG_UP:
        return steeplechase_track_flat_to_25_deg_up;
    case TRACK_ELEM_25_DEG_UP_TO_FLAT:
        return steeplechase_track_25_deg_up_to_flat;
    case TRACK_ELEM_25_DEG_DOWN:
        return steeplechase_track_25_deg_down;
    case TRACK_ELEM_FLAT_TO_25_DEG_DOWN:
        return steeplechase_track_flat_to_25_deg_down;
    case TRACK_ELEM_25_DEG_DOWN_TO_FLAT:
        return steeplechase_track_25_deg_down_to_flat;
    case TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES:
        return steeplechase_track_left_quarter_turn_5;
    case TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES:
        return steeplechase_track_right_quarter_turn_5;
    case TRACK_ELEM_S_BEND_LEFT:
        return steeplechase_track_s_bend_left;
    case TRACK_ELEM_S_BEND_RIGHT:
        return steeplechase_track_s_bend_right;
    case TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES:
        return steeplechase_track_left_quarter_turn_3;
    case TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES:
        return steeplechase_track_right_quarter_turn_3;
    case TRACK_ELEM_BRAKES:
        return steeplechase_track_brakes;
    case TRACK_ELEM_LEFT_EIGHTH_TO_DIAG:
        return steeplechase_track_left_eighth_to_diag;
    case TRACK_ELEM_RIGHT_EIGHTH_TO_DIAG:
        return steeplechase_track_right_eighth_to_diag;
    case TRACK_ELEM_LEFT_EIGHTH_TO_ORTHOGONAL:
        return steeplechase_track_left_eighth_to_orthogonal;
    case TRACK_ELEM_RIGHT_EIGHTH_TO_ORTHOGONAL:
        return steeplechase_track_right_eighth_to_orthogonal;
    case TRACK_ELEM_DIAG_FLAT:
        return steeplechase_track_diag_flat;
    case TRACK_ELEM_DIAG_25_DEG_UP:
        return steeplechase_track_diag_25_deg_up;
    case TRACK_ELEM_DIAG_FLAT_TO_25_DEG_UP:
        return steeplechase_track_diag_flat_to_25_deg_up;
    case TRACK_ELEM_DIAG_25_DEG_UP_TO_FLAT:
        return steeplechase_track_diag_25_deg_up_to_flat;
    case TRACK_ELEM_DIAG_25_DEG_DOWN:
        return steeplechase_track_diag_25_deg_down;
    case TRACK_ELEM_DIAG_FLAT_TO_25_DEG_DOWN:
        return steeplechase_track_diag_flat_to_25_deg_down;
    case TRACK_ELEM_DIAG_25_DEG_DOWN_TO_FLAT:
        return steeplechase_track_diag_25_deg_down_to_flat;
    case TRACK_ELEM_BLOCK_BRAKES:
        return steeplechase_track_block_brakes;
    }
    return NULL;
}
