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

/** rct2: 0x008A6370 */
static void looping_rc_track_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    if (track_element_is_lift_hill(mapElement)) {
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15006, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15007, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15008, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15009, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        }
        if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
        }
    } else {
        switch (direction) {
        case 0:
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15004, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 1:
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15005, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        }
        if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
        }
    }
    paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
    paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(height + 32, 0x20);
}

static void looping_rc_track_station(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    static const uint32 imageIds[4][2] = {
        { 15016, SPR_STATION_BASE_B_SW_NE },
        { 15017, SPR_STATION_BASE_B_NW_SE },
        { 15016, SPR_STATION_BASE_B_SW_NE },
        { 15017, SPR_STATION_BASE_B_NW_SE },
    };

    sub_98197C_rotated(direction, imageIds[direction][0] | gTrackColours[SCHEME_TRACK], 0, 0, 32, 20, 1, height, 0, 6, height + 3);
    sub_98196C_rotated(direction, imageIds[direction][1] | gTrackColours[SCHEME_MISC], 0, 0, 32, 32, 1, height);
    track_paint_util_draw_station_metal_supports_2(direction, height, gTrackColours[SCHEME_SUPPORTS], 0);
    track_paint_util_draw_station(rideIndex, trackSequence, direction, height, mapElement);
    paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);
    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 32, 0x20);
}

/** rct2: 0x008A6380 */
static void looping_rc_track_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    if (track_element_is_lift_hill(mapElement)) {
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15060, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15061, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15062, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15063, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        }
        if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
        }
    } else {
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15032, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15033, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15034, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15035, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        }
        if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
        }
    }
    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_1);
    } else {
        paint_util_push_tunnel_rotated(direction, height + 8, TUNNEL_2);
    }
    paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(height + 56, 0x20);
}

/** rct2: 0x008A6390 */
static void looping_rc_track_60_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    if (track_element_is_lift_hill(mapElement)) {
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15076, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15077, 0, 0, 32, 1, 98, height, 0, 27, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15078, 0, 0, 32, 1, 98, height, 0, 27, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15079, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        }
        if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 32, height, gTrackColours[SCHEME_SUPPORTS]);
        }
    } else {
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15048, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15049, 0, 0, 32, 1, 98, height, 0, 27, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15050, 0, 0, 32, 1, 98, height, 0, 27, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15051, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        }
        if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 32, height, gTrackColours[SCHEME_SUPPORTS]);
        }
    }
    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_1);
    } else {
        paint_util_push_tunnel_rotated(direction, height + 56, TUNNEL_2);
    }
    paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(height + 104, 0x20);
}

/** rct2: 0x008A63A0 */
static void looping_rc_track_flat_to_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    if (track_element_is_lift_hill(mapElement)) {
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15052, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15053, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15054, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15055, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        }
        if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 3, height, gTrackColours[SCHEME_SUPPORTS]);
        }
    } else {
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15024, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15025, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15026, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15027, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        }
        if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 3, height, gTrackColours[SCHEME_SUPPORTS]);
        }
    }
    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
    } else {
        paint_util_push_tunnel_rotated(direction, height, TUNNEL_2);
    }
    paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(height + 48, 0x20);
}

/** rct2: 0x008A63B0 */
static void looping_rc_track_25_deg_up_to_60_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    if (track_element_is_lift_hill(mapElement)) {
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15064, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15065, 0, 0, 32, 20, 3, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15068, 0, 0, 32, 1, 66, height, 0, 27, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15066, 0, 0, 32, 20, 3, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15069, 0, 0, 32, 1, 66, height, 0, 27, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15067, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        }
        if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 12, height, gTrackColours[SCHEME_SUPPORTS]);
        }
    } else {
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15036, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15037, 0, 0, 32, 20, 3, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15040, 0, 0, 32, 1, 66, height, 0, 27, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15038, 0, 0, 32, 20, 3, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15041, 0, 0, 32, 1, 66, height, 0, 27, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15039, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        }
        if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 12, height, gTrackColours[SCHEME_SUPPORTS]);
        }
    }
    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_1);
    } else {
        paint_util_push_tunnel_rotated(direction, height + 24, TUNNEL_2);
    }
    paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(height + 72, 0x20);
}

/** rct2: 0x008A63C0 */
static void looping_rc_track_60_deg_up_to_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    if (track_element_is_lift_hill(mapElement)) {
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15070, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15071, 0, 0, 32, 20, 3, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15074, 0, 0, 32, 1, 66, height, 0, 27, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15072, 0, 0, 32, 20, 3, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15075, 0, 0, 32, 1, 66, height, 0, 27, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15073, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        }
        if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 20, height, gTrackColours[SCHEME_SUPPORTS]);
        }
    } else {
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15042, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15043, 0, 0, 32, 20, 3, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15046, 0, 0, 32, 1, 66, height, 0, 27, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15044, 0, 0, 32, 20, 3, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15047, 0, 0, 32, 1, 66, height, 0, 27, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15045, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        }
        if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 20, height, gTrackColours[SCHEME_SUPPORTS]);
        }
    }
    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_1);
    } else {
        paint_util_push_tunnel_rotated(direction, height + 24, TUNNEL_2);
    }
    paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(height + 72, 0x20);
}

/** rct2: 0x008A63D0 */
static void looping_rc_track_25_deg_up_to_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    if (track_element_is_lift_hill(mapElement)) {
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15056, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15057, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15058, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15059, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        }
        if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
        }
    } else {
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15028, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15029, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15030, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15031, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        }
        if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
        }
    }
    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_0);
    } else {
        paint_util_push_tunnel_rotated(direction, height + 8, TUNNEL_12);
    }
    paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(height + 40, 0x20);
}

/** rct2: 0x008A63E0 */
static void looping_rc_track_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    looping_rc_track_25_deg_up(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008A63F0 */
static void looping_rc_track_60_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    looping_rc_track_60_deg_up(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008A6400 */
static void looping_rc_track_flat_to_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    looping_rc_track_25_deg_up_to_flat(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008A6410 */
static void looping_rc_track_25_deg_down_to_60_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    looping_rc_track_60_deg_up_to_25_deg_up(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008A6420 */
static void looping_rc_track_60_deg_down_to_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    looping_rc_track_25_deg_up_to_60_deg_up(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008A6430 */
static void looping_rc_track_25_deg_down_to_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    looping_rc_track_flat_to_25_deg_up(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008A6440 */
static void looping_rc_track_left_quarter_turn_5(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15183, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15188, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15193, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15178, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        }
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 1:
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15182, 0, 0, 32, 16, 3, height, 0, 0, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15187, 0, 0, 32, 16, 3, height, 0, 0, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15192, 0, 0, 32, 16, 3, height, 0, 16, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15177, 0, 0, 32, 16, 3, height, 0, 16, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15181, 0, 0, 16, 16, 3, height, 0, 16, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15186, 0, 0, 16, 16, 3, height, 16, 16, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15191, 0, 0, 16, 16, 3, height, 16, 0, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15176, 0, 0, 16, 16, 3, height, 0, 0, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 4:
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 5:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15180, 0, 0, 16, 32, 3, height, 16, 0, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15185, 0, 0, 16, 32, 3, height, 0, 0, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15190, 0, 0, 16, 32, 3, height, 0, 0, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15175, 0, 0, 16, 32, 3, height, 16, 0, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 6:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15179, 0, 0, 20, 32, 3, height, 6, 0, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15184, 0, 0, 20, 32, 3, height, 6, 0, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15189, 0, 0, 20, 32, 3, height, 6, 0, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15174, 0, 0, 20, 32, 3, height, 6, 0, height);
            break;
        }
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
        switch (direction) {
        case 2:
            paint_util_push_tunnel_right(height, TUNNEL_0);
            break;
        case 3:
            paint_util_push_tunnel_left(height, TUNNEL_0);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    }
}

/** rct2: 0x008A6450 */
static void looping_rc_track_right_quarter_turn_5(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    looping_rc_track_left_quarter_turn_5(rideIndex, trackSequence, (direction - 1) & 3, height, mapElement);
}

/** rct2: 0x008A6460 */
static void looping_rc_track_flat_to_left_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15080, 0, 0, 32, 20, 3, height, 0, 6, height);
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15092, 0, 0, 32, 1, 26, height, 0, 27, height);
        break;
    case 1:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15081, 0, 0, 32, 20, 3, height, 0, 6, height);
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15093, 0, 0, 32, 1, 26, height, 0, 27, height);
        break;
    case 2:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15082, 0, 0, 32, 20, 3, height, 0, 6, height);
        break;
    case 3:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15083, 0, 0, 32, 20, 3, height, 0, 6, height);
        break;
    }
    if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
    }
    paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
    paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(height + 32, 0x20);
}

/** rct2: 0x008A6470 */
static void looping_rc_track_flat_to_right_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15084, 0, 0, 32, 20, 3, height, 0, 6, height);
        break;
    case 1:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15085, 0, 0, 32, 20, 3, height, 0, 6, height);
        break;
    case 2:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15086, 0, 0, 32, 20, 3, height, 0, 6, height);
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15094, 0, 0, 32, 1, 26, height, 0, 27, height);
        break;
    case 3:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15087, 0, 0, 32, 20, 3, height, 0, 6, height);
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15095, 0, 0, 32, 1, 26, height, 0, 27, height);
        break;
    }
    if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
    }
    paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
    paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(height + 32, 0x20);
}

/** rct2: 0x008A6480 */
static void looping_rc_track_left_bank_to_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15086, 0, 0, 32, 20, 3, height, 0, 6, height);
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15094, 0, 0, 32, 1, 26, height, 0, 27, height);
        break;
    case 1:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15087, 0, 0, 32, 20, 3, height, 0, 6, height);
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15095, 0, 0, 32, 1, 26, height, 0, 27, height);
        break;
    case 2:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15084, 0, 0, 32, 20, 3, height, 0, 6, height);
        break;
    case 3:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15085, 0, 0, 32, 20, 3, height, 0, 6, height);
        break;
    }
    if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
    }
    paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
    paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(height + 32, 0x20);
}

/** rct2: 0x008A6490 */
static void looping_rc_track_right_bank_to_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15082, 0, 0, 32, 20, 3, height, 0, 6, height);
        break;
    case 1:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15083, 0, 0, 32, 20, 3, height, 0, 6, height);
        break;
    case 2:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15080, 0, 0, 32, 20, 3, height, 0, 6, height);
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15092, 0, 0, 32, 1, 26, height, 0, 27, height);
        break;
    case 3:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15081, 0, 0, 32, 20, 3, height, 0, 6, height);
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15093, 0, 0, 32, 1, 26, height, 0, 27, height);
        break;
    }
    if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
    }
    paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
    paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(height + 32, 0x20);
}

/** rct2: 0x008A64A0 */
static void looping_rc_track_banked_left_quarter_turn_5(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15203, 0, 0, 32, 20, 3, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15214, 0, 0, 32, 1, 26, height, 0, 27, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15208, 0, 0, 32, 1, 26, height, 0, 27, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15213, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15198, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        }
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 1:
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15202, 0, 0, 32, 16, 3, height, 0, 0, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15207, 0, 0, 32, 16, 1, height, 0, 0, height + 27);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15212, 0, 0, 32, 16, 3, height, 0, 16, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15197, 0, 0, 32, 16, 3, height, 0, 16, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15201, 0, 0, 16, 16, 3, height, 0, 16, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15206, 0, 0, 16, 16, 1, height, 16, 16, height + 27);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15211, 0, 0, 16, 16, 3, height, 16, 0, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15196, 0, 0, 16, 16, 3, height, 0, 0, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 4:
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 5:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15200, 0, 0, 16, 32, 3, height, 16, 0, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15205, 0, 0, 16, 32, 1, height, 0, 0, height + 27);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15210, 0, 0, 16, 32, 3, height, 0, 0, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15195, 0, 0, 16, 32, 3, height, 16, 0, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 6:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15199, 0, 0, 20, 32, 3, height, 6, 0, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15204, 0, 0, 1, 32, 26, height, 27, 0, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15209, 0, 0, 20, 32, 3, height, 6, 0, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15215, 0, 0, 1, 32, 26, height, 27, 0, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15194, 0, 0, 20, 32, 3, height, 6, 0, height);
            break;
        }
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
        switch (direction) {
        case 2:
            paint_util_push_tunnel_right(height, TUNNEL_0);
            break;
        case 3:
            paint_util_push_tunnel_left(height, TUNNEL_0);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    }
}

/** rct2: 0x008A64B0 */
static void looping_rc_track_banked_right_quarter_turn_5(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    looping_rc_track_banked_left_quarter_turn_5(rideIndex, trackSequence, (direction - 1) & 3, height, mapElement);
}

/** rct2: 0x008A64C0 */
static void looping_rc_track_left_bank_to_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15096, 0, 0, 32, 20, 3, height, 0, 6, height);
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15112, 0, 0, 32, 1, 34, height, 0, 27, height);
        break;
    case 1:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15097, 0, 0, 32, 20, 3, height, 0, 6, height);
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15113, 0, 0, 32, 1, 34, height, 0, 27, height);
        break;
    case 2:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15098, 0, 0, 32, 20, 3, height, 0, 6, height);
        break;
    case 3:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15099, 0, 0, 32, 20, 3, height, 0, 6, height);
        break;
    }
    if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 3, height, gTrackColours[SCHEME_SUPPORTS]);
    }
    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
    } else {
        paint_util_push_tunnel_rotated(direction, height, TUNNEL_2);
    }
    paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(height + 48, 0x20);
}

/** rct2: 0x008A64D0 */
static void looping_rc_track_right_bank_to_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15100, 0, 0, 32, 20, 3, height, 0, 6, height);
        break;
    case 1:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15101, 0, 0, 32, 20, 3, height, 0, 6, height);
        break;
    case 2:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15102, 0, 0, 32, 20, 3, height, 0, 6, height);
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15114, 0, 0, 32, 1, 34, height, 0, 27, height);
        break;
    case 3:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15103, 0, 0, 32, 20, 3, height, 0, 6, height);
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15115, 0, 0, 32, 1, 34, height, 0, 27, height);
        break;
    }
    if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 3, height, gTrackColours[SCHEME_SUPPORTS]);
    }
    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
    } else {
        paint_util_push_tunnel_rotated(direction, height, TUNNEL_2);
    }
    paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(height + 48, 0x20);
}

/** rct2: 0x008A64E0 */
static void looping_rc_track_25_deg_up_to_left_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15104, 0, 0, 32, 20, 3, height, 0, 6, height);
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15116, 0, 0, 32, 1, 34, height, 0, 27, height);
        break;
    case 1:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15105, 0, 0, 32, 20, 3, height, 0, 6, height);
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15117, 0, 0, 32, 1, 34, height, 0, 27, height);
        break;
    case 2:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15106, 0, 0, 32, 20, 3, height, 0, 6, height);
        break;
    case 3:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15107, 0, 0, 32, 20, 3, height, 0, 6, height);
        break;
    }
    if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
    }
    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_0);
    } else {
        paint_util_push_tunnel_rotated(direction, height + 8, TUNNEL_12);
    }
    paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(height + 40, 0x20);
}

/** rct2: 0x008A64F0 */
static void looping_rc_track_25_deg_up_to_right_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15108, 0, 0, 32, 20, 3, height, 0, 6, height);
        break;
    case 1:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15109, 0, 0, 32, 20, 3, height, 0, 6, height);
        break;
    case 2:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15110, 0, 0, 32, 20, 3, height, 0, 6, height);
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15118, 0, 0, 32, 1, 34, height, 0, 27, height);
        break;
    case 3:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15111, 0, 0, 32, 20, 3, height, 0, 6, height);
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15119, 0, 0, 32, 1, 34, height, 0, 27, height);
        break;
    }
    if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
    }
    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_0);
    } else {
        paint_util_push_tunnel_rotated(direction, height + 8, TUNNEL_12);
    }
    paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(height + 40, 0x20);
}

/** rct2: 0x008A6500 */
static void looping_rc_track_left_bank_to_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    looping_rc_track_25_deg_up_to_right_bank(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008A6510 */
static void looping_rc_track_right_bank_to_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    looping_rc_track_25_deg_up_to_left_bank(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008A6520 */
static void looping_rc_track_25_deg_down_to_left_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    looping_rc_track_right_bank_to_25_deg_up(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008A6530 */
static void looping_rc_track_25_deg_down_to_right_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    looping_rc_track_left_bank_to_25_deg_up(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008A6540 */
static void looping_rc_track_left_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15088, 0, 0, 32, 1, 26, height, 0, 27, height);
        break;
    case 1:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15089, 0, 0, 32, 1, 26, height, 0, 27, height);
        break;
    case 2:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15090, 0, 0, 32, 20, 3, height, 0, 6, height);
        break;
    case 3:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15091, 0, 0, 32, 20, 3, height, 0, 6, height);
        break;
    }
    if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
    }
    paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
    paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(height + 32, 0x20);
}

/** rct2: 0x008A6550 */
static void looping_rc_track_right_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    looping_rc_track_left_bank(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008A6560 */
static void looping_rc_track_left_quarter_turn_5_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15296, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15301, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15306, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15311, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        }
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_1);
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 1:
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15297, 0, 0, 32, 16, 3, height);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15302, 0, 0, 32, 16, 3, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15307, 0, 0, 32, 16, 3, height, 0, 16, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15312, 0, 0, 32, 16, 3, height, 0, 16, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15298, 0, 0, 16, 16, 3, height, 0, 16, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15303, 0, 0, 16, 16, 3, height, 16, 16, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15308, 0, 0, 16, 16, 3, height, 16, 0, height);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15313, 0, 0, 16, 16, 3, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 64, 0x20);
        break;
    case 4:
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 5:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15299, 0, 0, 16, 32, 3, height, 16, 0, height);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15304, 0, 0, 16, 32, 3, height);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15309, 0, 0, 16, 32, 3, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15314, 0, 0, 16, 32, 3, height, 16, 0, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 6:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15300, 0, 0, 20, 32, 3, height, 6, 0, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15305, 0, 0, 20, 32, 3, height, 6, 0, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15310, 0, 0, 20, 32, 3, height, 6, 0, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15315, 0, 0, 20, 32, 3, height, 6, 0, height);
            break;
        }
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
        switch (direction) {
        case 2:
            paint_util_push_tunnel_right(height + 8, TUNNEL_2);
            break;
        case 3:
            paint_util_push_tunnel_left(height + 8, TUNNEL_2);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    }
}

/** rct2: 0x008A6570 */
static void looping_rc_track_right_quarter_turn_5_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15276, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15281, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15286, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15291, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        }
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_1);
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 1:
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15277, 0, 0, 32, 16, 3, height, 0, 16, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15282, 0, 0, 32, 16, 3, height, 0, 16, height);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15287, 0, 0, 32, 16, 3, height);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15292, 0, 0, 32, 16, 3, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15278, 0, 0, 16, 16, 3, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15283, 0, 0, 16, 16, 3, height, 16, 0, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15288, 0, 0, 16, 16, 3, height, 16, 16, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15293, 0, 0, 16, 16, 3, height, 0, 16, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 64, 0x20);
        break;
    case 4:
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 5:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15279, 0, 0, 16, 32, 3, height, 16, 0, height);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15284, 0, 0, 16, 32, 3, height);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15289, 0, 0, 16, 32, 3, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15294, 0, 0, 16, 32, 3, height, 16, 0, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 6:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15280, 0, 0, 20, 32, 3, height, 6, 0, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15285, 0, 0, 20, 32, 3, height, 6, 0, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15290, 0, 0, 20, 32, 3, height, 6, 0, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15295, 0, 0, 20, 32, 3, height, 6, 0, height);
            break;
        }
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
        switch (direction) {
        case 0:
            paint_util_push_tunnel_right(height + 8, TUNNEL_2);
            break;
        case 1:
            paint_util_push_tunnel_left(height + 8, TUNNEL_2);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    }
}

/** rct2: 0x008A6580 */
static void looping_rc_track_left_quarter_turn_5_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    looping_rc_track_right_quarter_turn_5_25_deg_up(rideIndex, trackSequence, (direction + 1) & 3, height, mapElement);
}

/** rct2: 0x008A6590 */
static void looping_rc_track_right_quarter_turn_5_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    looping_rc_track_left_quarter_turn_5_25_deg_up(rideIndex, trackSequence, (direction - 1) & 3, height, mapElement);
}

/** rct2: 0x008A65A0 */
static void looping_rc_track_s_bend_left(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15260, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15264, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15263, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15267, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        }
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15261, 0, 0, 32, 26, 3, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 5, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15265, 0, 0, 32, 26, 3, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 6, 1, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15262, 0, 0, 32, 26, 3, height, 0, 6, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15266, 0, 0, 32, 26, 3, height, 0, 6, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15262, 0, 0, 32, 26, 3, height, 0, 6, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15266, 0, 0, 32, 26, 3, height, 0, 6, height);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15261, 0, 0, 32, 26, 3, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 5, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15265, 0, 0, 32, 26, 3, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 6, 1, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15263, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15267, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15260, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15264, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        }
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
        switch (direction) {
        case 1:
            paint_util_push_tunnel_right(height, TUNNEL_0);
            break;
        case 2:
            paint_util_push_tunnel_left(height, TUNNEL_0);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    }
}

/** rct2: 0x008A65B0 */
static void looping_rc_track_s_bend_right(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15268, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15272, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15271, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15275, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        }
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15269, 0, 0, 32, 26, 3, height, 0, 6, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 8, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15273, 0, 0, 32, 26, 3, height, 0, 6, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 7, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15270, 0, 0, 32, 26, 3, height);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15274, 0, 0, 32, 26, 3, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15270, 0, 0, 32, 26, 3, height);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15274, 0, 0, 32, 26, 3, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15269, 0, 0, 32, 26, 3, height, 0, 6, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 8, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15273, 0, 0, 32, 26, 3, height, 0, 6, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 7, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15271, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15275, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15268, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15272, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        }
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
        switch (direction) {
        case 1:
            paint_util_push_tunnel_right(height, TUNNEL_0);
            break;
        case 2:
            paint_util_push_tunnel_left(height, TUNNEL_0);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    }
}

/** rct2: 0x008A65C0 */
static void looping_rc_track_left_vertical_loop(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15348, 0, 6, 32, 20, 3, height);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15356, 0, 6, 32, 20, 3, height);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15355, 0, 6, 32, 20, 3, height);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15363, 0, 6, 32, 20, 7, height);
            break;
        }
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_1);
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 56, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15349, 0, 0, 32, 26, 3, height);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15357, 0, 14, 32, 2, 63, height);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15354, 0, 6, 32, 26, 3, height);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15362, 0, 6, 32, 26, 3, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15350, 16, 0, 3, 16, 119, height, 16, 0, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_THICK_CENTRED, 1, 0, height - 8, gTrackColours[SCHEME_TRACK]);
            sub_98199C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15364, 16, 0, 3, 16, 119, height, 16, 0, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15358, 12, 0, 3, 16, 119, height, 12, 0, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_THICK_ALT_CENTRED, 0, 0, height - 8, gTrackColours[SCHEME_TRACK]);
            sub_98199C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15366, 12, 0, 3, 16, 119, height, 12, 0, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15353, 10, 16, 4, 16, 119, height, 10, 16, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_THICK, 2, 0, height - 8, gTrackColours[SCHEME_TRACK]);
            sub_98199C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15365, 10, 16, 4, 16, 119, height, 10, 16, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15361, 16, 16, 2, 16, 119, height, 16, 16, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_THICK_ALT, 3, 0, height - 8, gTrackColours[SCHEME_TRACK]);
            sub_98199C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15367, 16, 16, 2, 16, 119, height, 16, 16, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 168, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15351, 0, 0, 32, 16, 3, height + 32);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15359, 0, 0, 32, 16, 3, height + 32);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15352, 0, 16, 32, 16, 3, height + 32);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15360, 0, 16, 32, 16, 3, height + 32);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 4:
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 5:
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 6:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15352, 0, 16, 32, 16, 3, height + 32);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15360, 0, 16, 32, 16, 3, height + 32);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15351, 0, 0, 32, 16, 3, height + 32);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15359, 0, 0, 32, 16, 3, height + 32);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 7:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15353, 10, 16, 4, 16, 119, height, 10, 16, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_THICK, 2, 0, height - 8, gTrackColours[SCHEME_TRACK]);
            sub_98199C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15365, 10, 16, 4, 16, 119, height, 10, 16, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15361, 16, 16, 2, 16, 119, height, 16, 16, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_THICK_ALT, 3, 0, height - 8, gTrackColours[SCHEME_TRACK]);
            sub_98199C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15367, 16, 16, 2, 16, 119, height, 16, 16, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15350, 16, 0, 3, 16, 119, height, 16, 0, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_THICK_CENTRED, 1, 0, height - 8, gTrackColours[SCHEME_TRACK]);
            sub_98199C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15364, 16, 0, 3, 16, 119, height, 16, 0, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15358, 12, 0, 3, 16, 119, height, 12, 0, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_THICK_ALT_CENTRED, 0, 0, height - 8, gTrackColours[SCHEME_TRACK]);
            sub_98199C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15366, 12, 0, 3, 16, 119, height, 12, 0, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 168, 0x20);
        break;
    case 8:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15354, 0, 6, 32, 26, 3, height);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15362, 0, 6, 32, 26, 3, height);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15349, 0, 0, 32, 26, 3, height);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15357, 0, 14, 32, 2, 63, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 9:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15355, 0, 6, 32, 20, 3, height);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15363, 0, 6, 32, 20, 7, height);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15348, 0, 6, 32, 20, 3, height);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15356, 0, 6, 32, 20, 3, height);
            break;
        }
        switch (direction) {
        case 1:
            paint_util_push_tunnel_right(height - 8, TUNNEL_1);
            break;
        case 2:
            paint_util_push_tunnel_left(height - 8, TUNNEL_1);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 56, 0x20);
        break;
    }
}

/** rct2: 0x008A65D0 */
static void looping_rc_track_right_vertical_loop(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15383, 0, 6, 32, 20, 7, height);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15375, 0, 6, 32, 20, 3, height);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15376, 0, 6, 32, 20, 3, height);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15368, 0, 6, 32, 20, 3, height);
            break;
        }
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_1);
        }
        paint_util_set_general_support_height(height + 56, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15382, 0, 6, 32, 26, 3, height);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15374, 0, 6, 32, 26, 3, height);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15377, 0, 14, 32, 2, 63, height);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15369, 0, 0, 32, 26, 3, height);
            break;
        }
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15381, 16, 16, 2, 16, 119, height, 16, 16, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_THICK, 3, 0, height - 8, gTrackColours[SCHEME_TRACK]);
            sub_98199C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15384, 16, 16, 2, 16, 119, height, 16, 16, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15373, 10, 16, 4, 16, 119, height, 10, 16, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_THICK_ALT, 1, 0, height - 8, gTrackColours[SCHEME_TRACK]);
            sub_98199C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15386, 10, 16, 4, 16, 119, height, 10, 16, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15378, 12, 0, 3, 16, 119, height, 12, 0, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_THICK_CENTRED, 0, 0, height - 8, gTrackColours[SCHEME_TRACK]);
            sub_98199C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15385, 12, 0, 3, 16, 119, height, 12, 0, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15370, 16, 0, 2, 16, 119, height, 16, 0, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_THICK_ALT_CENTRED, 2, 0, height - 8, gTrackColours[SCHEME_TRACK]);
            sub_98199C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15387, 16, 0, 2, 16, 119, height, 16, 0, height);
            break;
        }
        paint_util_set_general_support_height(height + 168, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15380, 0, 16, 32, 16, 3, height + 32);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15372, 0, 16, 32, 16, 3, height + 32);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15379, 0, 0, 32, 16, 3, height + 32);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15371, 0, 0, 32, 16, 3, height + 32);
            break;
        }
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 4:
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 5:
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 6:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15379, 0, 0, 32, 16, 3, height + 32);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15371, 0, 0, 32, 16, 3, height + 32);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15380, 0, 16, 32, 16, 3, height + 32);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15372, 0, 16, 32, 16, 3, height + 32);
            break;
        }
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 7:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15378, 12, 0, 3, 16, 119, height, 12, 0, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_THICK_CENTRED, 0, 0, height - 8, gTrackColours[SCHEME_TRACK]);
            sub_98199C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15385, 12, 0, 3, 16, 119, height, 12, 0, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15370, 16, 0, 2, 16, 119, height, 16, 0, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_THICK_ALT_CENTRED, 2, 0, height - 8, gTrackColours[SCHEME_TRACK]);
            sub_98199C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15387, 16, 0, 2, 16, 119, height, 16, 0, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15381, 16, 16, 2, 16, 119, height, 16, 16, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_THICK, 3, 0, height - 8, gTrackColours[SCHEME_TRACK]);
            sub_98199C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15384, 16, 16, 2, 16, 119, height, 16, 16, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15373, 10, 16, 4, 16, 119, height, 10, 16, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_THICK_ALT, 1, 0, height - 8, gTrackColours[SCHEME_TRACK]);
            sub_98199C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15386, 10, 16, 4, 16, 119, height, 10, 16, height);
            break;
        }
        paint_util_set_general_support_height(height + 168, 0x20);
        break;
    case 8:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15377, 0, 14, 32, 2, 63, height);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15369, 0, 0, 32, 26, 3, height);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15382, 0, 6, 32, 26, 3, height);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15374, 0, 6, 32, 26, 3, height);
            break;
        }
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 9:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15376, 0, 6, 32, 20, 3, height);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15368, 0, 6, 32, 20, 3, height);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15383, 0, 6, 32, 20, 7, height);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15375, 0, 6, 32, 20, 3, height);
            break;
        }
        switch (direction) {
        case 1:
            paint_util_push_tunnel_right(height - 8, TUNNEL_1);
            break;
        case 2:
            paint_util_push_tunnel_left(height - 8, TUNNEL_1);
            break;
        }
        paint_util_set_general_support_height(height + 56, 0x20);
        break;
    }

    track_paint_util_right_vertical_loop_segments(direction, trackSequence);
}

/** rct2: 0x008A6630 */
static void looping_rc_track_left_quarter_turn_3(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15125, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15128, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15131, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15122, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        }
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 1:
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15124, 0, 0, 16, 16, 3, height, 16, 0, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15127, 0, 0, 16, 16, 3, height, 0, 0, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15130, 0, 0, 16, 16, 3, height, 0, 16, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15121, 0, 0, 16, 16, 3, height, 16, 16, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15123, 0, 0, 20, 32, 3, height, 6, 0, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15126, 0, 0, 20, 32, 3, height, 6, 0, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15129, 0, 0, 20, 32, 3, height, 6, 0, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15120, 0, 0, 20, 32, 3, height, 6, 0, height);
            break;
        }
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
        switch (direction) {
        case 2:
            paint_util_push_tunnel_right(height, TUNNEL_0);
            break;
        case 3:
            paint_util_push_tunnel_left(height, TUNNEL_0);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    }
}

/** rct2: 0x008A6640 */
static void looping_rc_track_right_quarter_turn_3(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    looping_rc_track_left_quarter_turn_3(rideIndex, trackSequence, (direction - 1) & 3, height, mapElement);
}

/** rct2: 0x008A6650 */
static void looping_rc_track_left_quarter_turn_3_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15137, 0, 0, 32, 20, 3, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15144, 0, 0, 32, 1, 26, height, 0, 27, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15140, 0, 0, 32, 1, 26, height, 0, 27, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15143, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15134, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        }
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 1:
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15136, 0, 0, 16, 16, 3, height, 16, 0, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15139, 0, 0, 16, 16, 1, height, 0, 0, height + 27);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15142, 0, 0, 16, 16, 3, height, 0, 16, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15133, 0, 0, 16, 16, 3, height, 16, 16, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15135, 0, 0, 20, 32, 3, height, 6, 0, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15138, 0, 0, 1, 32, 26, height, 27, 0, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15141, 0, 0, 20, 32, 3, height, 6, 0, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15145, 0, 0, 1, 32, 26, height, 27, 0, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15132, 0, 0, 20, 32, 3, height, 6, 0, height);
            break;
        }
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
        switch (direction) {
        case 2:
            paint_util_push_tunnel_right(height, TUNNEL_0);
            break;
        case 3:
            paint_util_push_tunnel_left(height, TUNNEL_0);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    }
}

/** rct2: 0x008A6660 */
static void looping_rc_track_right_quarter_turn_3_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    looping_rc_track_left_quarter_turn_3_bank(rideIndex, trackSequence, (direction - 1) & 3, height, mapElement);
}

/** rct2: 0x008A6670 */
static void looping_rc_track_left_quarter_turn_3_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15327, 0, 6, 32, 20, 3, height);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15329, 0, 6, 32, 20, 3, height);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15331, 0, 6, 32, 20, 3, height);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15325, 0, 6, 32, 20, 3, height);
            break;
        }
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_1);
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 1:
        paint_util_set_general_support_height(height + 56, 0x20);
        break;
    case 2:
        paint_util_set_general_support_height(height + 56, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15326, 6, 0, 20, 32, 3, height);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15328, 6, 0, 20, 32, 3, height);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15330, 6, 0, 20, 32, 3, height);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15324, 6, 0, 20, 32, 3, height);
            break;
        }
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
        switch (direction) {
        case 2:
            paint_util_push_tunnel_right(height + 8, TUNNEL_2);
            break;
        case 3:
            paint_util_push_tunnel_left(height + 8, TUNNEL_2);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    }
}

/** rct2: 0x008A6680 */
static void looping_rc_track_right_quarter_turn_3_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15316, 0, 6, 32, 20, 3, height);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15318, 0, 6, 32, 20, 3, height);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15320, 0, 6, 32, 20, 3, height);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15322, 0, 6, 32, 20, 3, height);
            break;
        }
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_1);
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 1:
        paint_util_set_general_support_height(height + 56, 0x20);
        break;
    case 2:
        paint_util_set_general_support_height(height + 56, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15317, 6, 0, 20, 32, 3, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15319, 6, 0, 20, 32, 3, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15321, 6, 0, 20, 32, 3, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 10, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15323, 6, 0, 20, 32, 3, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }
        switch (direction) {
        case 0:
            paint_util_push_tunnel_right(height + 8, TUNNEL_2);
            break;
        case 1:
            paint_util_push_tunnel_left(height + 8, TUNNEL_2);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    }
}

/** rct2: 0x008A6690 */
static void looping_rc_track_left_quarter_turn_3_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    looping_rc_track_right_quarter_turn_3_25_deg_up(rideIndex, trackSequence, (direction + 1) & 3, height, mapElement);
}

/** rct2: 0x008A66A0 */
static void looping_rc_track_right_quarter_turn_3_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    looping_rc_track_left_quarter_turn_3_25_deg_up(rideIndex, trackSequence, (direction - 1) & 3, height, mapElement);
}

/** rct2: 0x008A66B0 */
static void looping_rc_track_left_half_banked_helix_up_small(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15165, 0, 0, 32, 20, 3, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15172, 0, 0, 32, 1, 26, height, 0, 27, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15168, 0, 0, 32, 1, 26, height, 0, 27, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15171, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15162, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        }
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 2, height, gTrackColours[SCHEME_SUPPORTS]);
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 1:
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15164, 0, 0, 16, 16, 3, height, 16, 0, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15167, 0, 0, 16, 16, 1, height, 0, 0, height + 27);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15170, 0, 0, 16, 16, 3, height, 0, 16, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15161, 0, 0, 16, 16, 3, height, 16, 16, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15163, 0, 0, 20, 32, 3, height, 6, 0, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15166, 0, 0, 1, 32, 26, height, 27, 0, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15169, 0, 0, 20, 32, 3, height, 6, 0, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15173, 0, 0, 1, 32, 26, height, 27, 0, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15160, 0, 0, 20, 32, 3, height, 6, 0, height);
            break;
        }
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
        switch (direction) {
        case 2:
            paint_util_push_tunnel_right(height + 8, TUNNEL_0);
            break;
        case 3:
            paint_util_push_tunnel_left(height + 8, TUNNEL_0);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 4:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15162, 0, 0, 20, 32, 3, height, 6, 0, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15165, 0, 0, 20, 32, 3, height, 6, 0, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15172, 0, 0, 1, 32, 26, height, 27, 0, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15168, 0, 0, 1, 32, 26, height, 27, 0, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15171, 0, 0, 20, 32, 3, height, 6, 0, height);
            break;
        }
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 2, height, gTrackColours[SCHEME_SUPPORTS]);
        switch (direction) {
        case 0:
            paint_util_push_tunnel_right(height, TUNNEL_0);
            break;
        case 1:
            paint_util_push_tunnel_left(height, TUNNEL_0);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 5:
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 6:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15161, 0, 0, 16, 16, 3, height, 16, 16, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15164, 0, 0, 16, 16, 3, height, 0, 16, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15167, 0, 0, 16, 16, 1, height, 0, 0, height + 27);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15170, 0, 0, 16, 16, 3, height, 16, 0, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 7:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15160, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15163, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15166, 0, 0, 32, 1, 26, height, 0, 27, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15169, 0, 0, 32, 20, 3, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15173, 0, 0, 32, 1, 26, height, 0, 27, height);
            break;
        }
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height + 8, TUNNEL_0);
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    }
}

/** rct2: 0x008A66C0 */
static void looping_rc_track_right_half_banked_helix_up_small(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15146, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15149, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15152, 0, 0, 32, 1, 26, height, 0, 27, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15155, 0, 0, 32, 20, 3, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15159, 0, 0, 32, 1, 26, height, 0, 27, height);
            break;
        }
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 2, height, gTrackColours[SCHEME_SUPPORTS]);
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 1:
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15147, 0, 0, 16, 16, 3, height, 16, 16, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15150, 0, 0, 16, 16, 3, height, 0, 16, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15153, 0, 0, 16, 16, 1, height, 0, 0, height + 27);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15156, 0, 0, 16, 16, 3, height, 16, 0, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15148, 0, 0, 20, 32, 3, height, 6, 0, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15151, 0, 0, 20, 32, 3, height, 6, 0, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15158, 0, 0, 1, 32, 26, height, 27, 0, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15154, 0, 0, 1, 32, 26, height, 27, 0, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15157, 0, 0, 20, 32, 3, height, 6, 0, height);
            break;
        }
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
        switch (direction) {
        case 0:
            paint_util_push_tunnel_right(height + 8, TUNNEL_0);
            break;
        case 1:
            paint_util_push_tunnel_left(height + 8, TUNNEL_0);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 4:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15149, 0, 0, 20, 32, 3, height, 6, 0, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15152, 0, 0, 1, 32, 26, height, 27, 0, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15155, 0, 0, 20, 32, 3, height, 6, 0, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15159, 0, 0, 1, 32, 26, height, 27, 0, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15146, 0, 0, 20, 32, 3, height, 6, 0, height);
            break;
        }
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 2, height, gTrackColours[SCHEME_SUPPORTS]);
        switch (direction) {
        case 2:
            paint_util_push_tunnel_right(height, TUNNEL_0);
            break;
        case 3:
            paint_util_push_tunnel_left(height, TUNNEL_0);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 5:
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 6:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15150, 0, 0, 16, 16, 3, height, 16, 0, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15153, 0, 0, 16, 16, 1, height, 0, 0, height + 27);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15156, 0, 0, 16, 16, 3, height, 0, 16, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15147, 0, 0, 16, 16, 3, height, 16, 16, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 7:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15151, 0, 0, 32, 20, 3, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15158, 0, 0, 32, 1, 26, height, 0, 27, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15154, 0, 0, 32, 1, 26, height, 0, 27, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15157, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15148, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        }
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height + 8, TUNNEL_0);
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    }
}

/** rct2: 0x008A66D0 */
static void looping_rc_track_left_half_banked_helix_down_small(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    if (trackSequence >= 4) {
        trackSequence -= 4;
        direction = (direction - 1) & 3;
    }
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    looping_rc_track_right_half_banked_helix_up_small(rideIndex, trackSequence, (direction + 1) & 3, height, mapElement);
}

/** rct2: 0x008A66E0 */
static void looping_rc_track_right_half_banked_helix_down_small(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    if (trackSequence >= 4) {
        trackSequence -= 4;
        direction = (direction + 1) & 3;
    }
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    looping_rc_track_left_half_banked_helix_up_small(rideIndex, trackSequence, (direction - 1) & 3, height, mapElement);
}

/** rct2: 0x008A66F0 */
static void looping_rc_track_left_half_banked_helix_up_large(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15247, 0, 0, 32, 20, 3, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15258, 0, 0, 32, 1, 26, height, 0, 27, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15252, 0, 0, 32, 1, 26, height, 0, 27, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15257, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15242, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        }
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 1, height, gTrackColours[SCHEME_SUPPORTS]);
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 1:
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15246, 0, 0, 32, 16, 3, height, 0, 0, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15251, 0, 0, 32, 16, 1, height, 0, 0, height + 27);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15256, 0, 0, 32, 16, 3, height, 0, 16, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15241, 0, 0, 32, 16, 3, height, 0, 16, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15245, 0, 0, 16, 16, 3, height, 0, 16, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15250, 0, 0, 16, 16, 1, height, 16, 16, height + 27);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15255, 0, 0, 16, 16, 3, height, 16, 0, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15240, 0, 0, 16, 16, 3, height, 0, 0, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 4:
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 5:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15244, 0, 0, 16, 32, 3, height, 16, 0, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15249, 0, 0, 16, 32, 1, height, 0, 0, height + 27);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15254, 0, 0, 16, 32, 3, height, 0, 0, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15239, 0, 0, 16, 32, 3, height, 16, 0, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 6:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15243, 0, 0, 20, 32, 3, height, 6, 0, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15248, 0, 0, 1, 32, 26, height, 27, 0, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15253, 0, 0, 20, 32, 3, height, 6, 0, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15259, 0, 0, 1, 32, 26, height, 27, 0, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15238, 0, 0, 20, 32, 3, height, 6, 0, height);
            break;
        }
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 7, height, gTrackColours[SCHEME_SUPPORTS]);
        switch (direction) {
        case 2:
            paint_util_push_tunnel_right(height + 8, TUNNEL_0);
            break;
        case 3:
            paint_util_push_tunnel_left(height + 8, TUNNEL_0);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 7:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15242, 0, 0, 20, 32, 3, height, 6, 0, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15247, 0, 0, 20, 32, 3, height, 6, 0, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15258, 0, 0, 1, 32, 26, height, 27, 0, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15252, 0, 0, 1, 32, 26, height, 27, 0, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15257, 0, 0, 20, 32, 3, height, 6, 0, height);
            break;
        }
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 1, height, gTrackColours[SCHEME_SUPPORTS]);
        switch (direction) {
        case 0:
            paint_util_push_tunnel_right(height, TUNNEL_0);
            break;
        case 1:
            paint_util_push_tunnel_left(height, TUNNEL_0);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 8:
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 9:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15241, 0, 0, 16, 32, 3, height, 16, 0, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15246, 0, 0, 16, 32, 3, height, 0, 0, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15251, 0, 0, 16, 32, 1, height, 0, 0, height + 27);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15256, 0, 0, 16, 32, 3, height, 16, 0, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 10:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15240, 0, 0, 16, 16, 3, height, 0, 0, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15245, 0, 0, 16, 16, 3, height, 16, 0, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15250, 0, 0, 16, 16, 1, height, 16, 16, height + 27);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15255, 0, 0, 16, 16, 3, height, 0, 16, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 11:
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 12:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15239, 0, 0, 32, 16, 3, height, 0, 16, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15244, 0, 0, 32, 16, 3, height, 0, 16, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15249, 0, 0, 32, 16, 1, height, 0, 0, height + 27);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15254, 0, 0, 32, 16, 3, height, 0, 0, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 13:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15238, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15243, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15248, 0, 0, 32, 1, 26, height, 0, 27, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15253, 0, 0, 32, 20, 3, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15259, 0, 0, 32, 1, 26, height, 0, 27, height);
            break;
        }
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 7, height, gTrackColours[SCHEME_SUPPORTS]);
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height + 8, TUNNEL_0);
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    }
}

/** rct2: 0x008A6700 */
static void looping_rc_track_right_half_banked_helix_up_large(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15216, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15221, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15226, 0, 0, 32, 1, 26, height, 0, 27, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15231, 0, 0, 32, 20, 3, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15237, 0, 0, 32, 1, 26, height, 0, 27, height);
            break;
        }
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 1, height, gTrackColours[SCHEME_SUPPORTS]);
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 1:
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15217, 0, 0, 32, 16, 3, height, 0, 16, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15222, 0, 0, 32, 16, 3, height, 0, 16, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15227, 0, 0, 32, 16, 1, height, 0, 0, height + 27);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15232, 0, 0, 32, 16, 3, height, 0, 0, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15218, 0, 0, 16, 16, 3, height, 0, 0, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15223, 0, 0, 16, 16, 3, height, 16, 0, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15228, 0, 0, 16, 16, 1, height, 16, 16, height + 27);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15233, 0, 0, 16, 16, 3, height, 0, 16, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 4:
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 5:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15219, 0, 0, 16, 32, 3, height, 16, 0, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15224, 0, 0, 16, 32, 3, height, 0, 0, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15229, 0, 0, 16, 32, 1, height, 0, 0, height + 27);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15234, 0, 0, 16, 32, 3, height, 16, 0, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 6:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15220, 0, 0, 20, 32, 3, height, 6, 0, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15225, 0, 0, 20, 32, 3, height, 6, 0, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15236, 0, 0, 1, 32, 26, height, 27, 0, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15230, 0, 0, 1, 32, 26, height, 27, 0, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15235, 0, 0, 20, 32, 3, height, 6, 0, height);
            break;
        }
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 7, height, gTrackColours[SCHEME_SUPPORTS]);
        switch (direction) {
        case 0:
            paint_util_push_tunnel_right(height + 8, TUNNEL_0);
            break;
        case 1:
            paint_util_push_tunnel_left(height + 8, TUNNEL_0);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 7:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15221, 0, 0, 20, 32, 3, height, 6, 0, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15226, 0, 0, 1, 32, 26, height, 27, 0, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15231, 0, 0, 20, 32, 3, height, 6, 0, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15237, 0, 0, 1, 32, 26, height, 27, 0, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15216, 0, 0, 20, 32, 3, height, 6, 0, height);
            break;
        }
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 1, height, gTrackColours[SCHEME_SUPPORTS]);
        switch (direction) {
        case 2:
            paint_util_push_tunnel_right(height, TUNNEL_0);
            break;
        case 3:
            paint_util_push_tunnel_left(height, TUNNEL_0);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 8:
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 9:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15222, 0, 0, 16, 32, 3, height, 16, 0, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15227, 0, 0, 16, 32, 1, height, 0, 0, height + 27);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15232, 0, 0, 16, 32, 3, height, 0, 0, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15217, 0, 0, 16, 32, 3, height, 16, 0, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 10:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15223, 0, 0, 16, 16, 3, height, 0, 16, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15228, 0, 0, 16, 16, 1, height, 16, 16, height + 27);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15233, 0, 0, 16, 16, 3, height, 16, 0, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15218, 0, 0, 16, 16, 3, height, 0, 0, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 11:
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 12:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15224, 0, 0, 32, 16, 3, height, 0, 0, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15229, 0, 0, 32, 16, 1, height, 0, 0, height + 27);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15234, 0, 0, 32, 16, 3, height, 0, 16, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15219, 0, 0, 32, 16, 3, height, 0, 16, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 13:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15225, 0, 0, 32, 20, 3, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15236, 0, 0, 32, 1, 26, height, 0, 27, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15230, 0, 0, 32, 1, 26, height, 0, 27, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15235, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15220, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        }
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 7, height, gTrackColours[SCHEME_SUPPORTS]);
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height + 8, TUNNEL_0);
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    }
}

/** rct2: 0x008A6710 */
static void looping_rc_track_left_half_banked_helix_down_large(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    if (trackSequence >= 7) {
        trackSequence -= 7;
        direction = (direction - 1) & 3;
    }
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    looping_rc_track_right_half_banked_helix_up_large(rideIndex, trackSequence, (direction + 1) & 3, height, mapElement);
}

/** rct2: 0x008A6720 */
static void looping_rc_track_right_half_banked_helix_down_large(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    if (trackSequence >= 7) {
        trackSequence -= 7;
        direction = (direction + 1) & 3;
    }
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    looping_rc_track_left_half_banked_helix_up_large(rideIndex, trackSequence, (direction - 1) & 3, height, mapElement);
}

/** rct2: 0x008A6750 */
static void looping_rc_track_left_quarter_turn_1_60_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15341, 0, 0, 28, 28, 3, height, 2, 2, height);
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15345, 0, 0, 28, 28, 1, height, 2, 2, height + 99);
        break;
    case 1:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15342, 0, 0, 28, 28, 3, height, 2, 2, height);
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15346, 0, 0, 28, 28, 1, height, 2, 2, height + 99);
        break;
    case 2:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15343, 0, 0, 28, 28, 3, height, 2, 2, height);
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15347, 0, 0, 28, 28, 1, height, 2, 2, height + 99);
        break;
    case 3:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15340, 0, 0, 28, 28, 3, height, 2, 2, height);
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15344, 0, 0, 28, 28, 1, height, 2, 2, height + 99);
        break;
    }
    track_paint_util_left_quarter_turn_1_tile_tunnel(direction, height, -8, TUNNEL_1, +56, TUNNEL_2);
    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 104, 0x20);
}

/** rct2: 0x008A6730 */
static void looping_rc_track_right_quarter_turn_1_60_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15332, 0, 0, 28, 28, 3, height, 2, 2, height);
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15336, 0, 0, 28, 28, 1, height, 2, 2, height + 99);
        break;
    case 1:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15333, 0, 0, 28, 28, 3, height, 2, 2, height);
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15337, 0, 0, 28, 28, 1, height, 2, 2, height + 99);
        break;
    case 2:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15334, 0, 0, 28, 28, 3, height, 2, 2, height);
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15338, 0, 0, 2, 28, 59, height, 28, 2, height + 2);
        break;
    case 3:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15335, 0, 0, 28, 28, 3, height, 2, 2, height);
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15339, 0, 0, 28, 28, 1, height, 2, 2, height + 99);
        break;
    }
    track_paint_util_right_quarter_turn_1_tile_tunnel(direction, height, -8, TUNNEL_1, +56, TUNNEL_2);
    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 104, 0x20);
}

/** rct2: 0x008A6740 */
static void looping_rc_track_left_quarter_turn_1_60_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    looping_rc_track_right_quarter_turn_1_60_deg_up(rideIndex, trackSequence, (direction + 1) & 3, height, mapElement);
}

/** rct2: 0x008A6760 */
static void looping_rc_track_right_quarter_turn_1_60_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    looping_rc_track_left_quarter_turn_1_60_deg_up(rideIndex, trackSequence, (direction - 1) & 3, height, mapElement);
}

/** rct2: 0x008A6770 */
static void looping_rc_track_brakes(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
    case 2:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15012, 0, 0, 32, 20, 3, height, 0, 6, height);
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15014, 0, 0, 32, 1, 26, height, 0, 27, height);
        break;
    case 1:
    case 3:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15013, 0, 0, 32, 20, 3, height, 0, 6, height);
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15015, 0, 0, 32, 1, 26, height, 0, 27, height);
        break;
    }
    if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
    }
    paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
    paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(height + 32, 0x20);
}

/** rct2: 0x008A6A40 */
static void looping_rc_track_25_deg_up_left_banked(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15594, 0, 0, 32, 20, 3, height, 0, 6, height);
        break;
    case 1:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15595, 0, 0, 32, 1, 34, height, 0, 27, height);
        break;
    case 2:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15596, 0, 0, 32, 20, 3, height, 0, 6, height);
        break;
    case 3:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15597, 0, 0, 32, 20, 3, height, 0, 6, height);
        break;
    }
    if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
    }
    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_1);
    } else {
        paint_util_push_tunnel_rotated(direction, height + 8, TUNNEL_2);
    }
    paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(height + 56, 0x20);
}

/** rct2: 0x008A6A50 */
static void looping_rc_track_25_deg_up_right_banked(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15598, 0, 0, 32, 20, 3, height, 0, 6, height);
        break;
    case 1:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15599, 0, 0, 32, 20, 3, height, 0, 6, height);
        break;
    case 2:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15600, 0, 0, 32, 1, 34, height, 0, 27, height);
        break;
    case 3:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15601, 0, 0, 32, 20, 3, height, 0, 6, height);
        break;
    }
    if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
    }
    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_1);
    } else {
        paint_util_push_tunnel_rotated(direction, height + 8, TUNNEL_2);
    }
    paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(height + 56, 0x20);
}

/** rct2: 0x008A6780 */
static void looping_rc_track_on_ride_photo(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
        sub_98196C_rotated(direction, gTrackColours[SCHEME_MISC] | 22432, 0, 0, 32, 32, 1, height);
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 5, 0, height, gTrackColours[SCHEME_SUPPORTS]);
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 8, 0, height, gTrackColours[SCHEME_SUPPORTS]);
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15004, 0, 0, 32, 20, 0, height, 0, 6, height + 3);
        break;
    case 1:
        sub_98196C_rotated(direction, gTrackColours[SCHEME_MISC] | 22432, 0, 0, 32, 32, 1, height);
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 6, 0, height, gTrackColours[SCHEME_SUPPORTS]);
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 7, 0, height, gTrackColours[SCHEME_SUPPORTS]);
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15005, 0, 0, 32, 20, 0, height, 0, 6, height + 3);
        break;
    case 2:
        sub_98196C_rotated(direction, gTrackColours[SCHEME_MISC] | 22432, 0, 0, 32, 32, 1, height);
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 5, 0, height, gTrackColours[SCHEME_SUPPORTS]);
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 8, 0, height, gTrackColours[SCHEME_SUPPORTS]);
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15004, 0, 0, 32, 20, 0, height, 0, 6, height + 3);
        break;
    case 3:
        sub_98196C_rotated(direction, gTrackColours[SCHEME_MISC] | 22432, 0, 0, 32, 32, 1, height);
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 6, 0, height, gTrackColours[SCHEME_SUPPORTS]);
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 7, 0, height, gTrackColours[SCHEME_SUPPORTS]);
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15005, 0, 0, 32, 20, 0, height, 0, 6, height + 3);
        break;
    }
    track_paint_util_onride_photo_paint(direction, height + 3, mapElement);
    paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 48, 0x20);
}

/** rct2: 0x008A6A60 */
static void looping_rc_track_25_deg_down_left_banked(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    looping_rc_track_25_deg_up_right_banked(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008A6A70 */
static void looping_rc_track_25_deg_down_right_banked(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    looping_rc_track_25_deg_up_left_banked(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008A6860 */
static void looping_rc_track_left_eighth_to_diag(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15526, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15530, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15534, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15538, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        }
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15527, 0, 0, 32, 16, 3, height, 0, 0, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15531, 0, 0, 34, 16, 3, height, 0, 0, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15535, 0, 0, 32, 16, 3, height, 0, 16, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15539, 0, 0, 32, 16, 3, height, 0, 16, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15528, 0, 0, 16, 16, 3, height, 0, 16, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15532, 0, 0, 16, 16, 3, height, 16, 16, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15536, 0, 0, 16, 16, 3, height, 16, 0, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15540, 0, 0, 16, 16, 3, height, 0, 0, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 3:
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 4:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15529, 0, 0, 16, 16, 3, height, 16, 16, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 3, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15533, 0, 0, 16, 18, 3, height, 0, 16, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 1, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15537, 0, 0, 16, 16, 3, height, 0, 0, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 0, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15541, 0, 0, 16, 16, 3, height, 16, 0, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 2, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    }
}

/** rct2: 0x008A6870 */
static void looping_rc_track_right_eighth_to_diag(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15510, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15514, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15518, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15522, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        }
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15511, 0, 0, 32, 16, 3, height, 0, 16, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15515, 0, 0, 32, 16, 3, height, 0, 16, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15519, 0, 0, 34, 16, 3, height, 0, 0, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15523, 0, 0, 32, 16, 3, height, 0, 0, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15512, 0, 0, 16, 16, 3, height, 0, 0, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15516, 0, 0, 16, 16, 3, height, 16, 0, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15520, 0, 0, 28, 28, 3, height, 4, 4, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15524, 0, 0, 16, 16, 3, height, 0, 16, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 3:
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 4:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15513, 0, 0, 16, 16, 3, height, 16, 0, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 1, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15517, 0, 0, 16, 16, 3, height, 0, 0, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 0, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15521, 0, 0, 16, 18, 3, height, 0, 16, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 2, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15525, 0, 0, 16, 16, 3, height, 16, 16, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 3, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    }
}

/** rct2: 0x008A6880 */
static void looping_rc_track_left_eighth_to_orthogonal(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    looping_rc_track_right_eighth_to_diag(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008A6890 */
static void looping_rc_track_right_eighth_to_orthogonal(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    looping_rc_track_left_eighth_to_diag(rideIndex, trackSequence, (direction + 3) & 3, height, mapElement);
}

/** rct2: 0x008A68A0 */
static void looping_rc_track_left_eighth_bank_to_diag(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15558, 0, 0, 32, 1, 26, height, 0, 27, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15562, 0, 0, 32, 1, 26, height, 0, 27, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15566, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15570, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        }
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15559, 0, 0, 32, 16, 3, height, 0, 0, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15563, 0, 0, 34, 16, 0, height, 0, 0, height + 27);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15567, 0, 0, 32, 16, 3, height, 0, 16, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15571, 0, 0, 32, 16, 3, height, 0, 16, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15560, 0, 0, 16, 16, 3, height, 0, 16, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15564, 0, 0, 16, 16, 0, height, 16, 16, height + 27);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15568, 0, 0, 16, 16, 3, height, 16, 0, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15572, 0, 0, 16, 16, 3, height, 0, 0, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 3:
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 4:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15561, 0, 0, 16, 16, 3, height, 16, 16, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 3, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15565, 0, 0, 16, 18, 0, height, 0, 16, height + 27);
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 1, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15569, 0, 0, 16, 16, 3, height, 0, 0, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 0, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15573, 0, 0, 16, 16, 3, height, 16, 0, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 2, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    }
}

/** rct2: 0x008A68B0 */
static void looping_rc_track_right_eighth_bank_to_diag(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15542, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15546, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15550, 0, 0, 32, 1, 26, height, 0, 27, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15554, 0, 0, 32, 1, 26, height, 0, 27, height);
            break;
        }
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15543, 0, 0, 32, 16, 3, height, 0, 16, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15547, 0, 0, 32, 16, 3, height, 0, 16, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15551, 0, 0, 34, 16, 0, height, 0, 0, height + 27);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15555, 0, 0, 32, 16, 3, height, 0, 0, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15544, 0, 0, 16, 16, 3, height, 0, 0, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15548, 0, 0, 16, 16, 3, height, 16, 0, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15552, 0, 0, 28, 28, 0, height, 4, 4, height + 27);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15556, 0, 0, 16, 16, 3, height, 0, 16, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 3:
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 4:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15545, 0, 0, 16, 16, 3, height, 16, 0, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 1, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15549, 0, 0, 16, 16, 3, height, 0, 0, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 0, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15553, 0, 0, 16, 18, 0, height, 0, 16, height + 27);
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 2, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15557, 0, 0, 16, 16, 3, height, 16, 16, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 3, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    }
}

/** rct2: 0x008A68C0 */
static void looping_rc_track_left_eighth_bank_to_orthogonal(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    looping_rc_track_right_eighth_bank_to_diag(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008A68D0 */
static void looping_rc_track_right_eighth_bank_to_orthogonal(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    looping_rc_track_left_eighth_bank_to_diag(rideIndex, trackSequence, (direction + 3) & 3, height, mapElement);
}

/** rct2: 0x008A6790 */
static void looping_rc_track_diag_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 3:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15451, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 3:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15423, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 1:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 0:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15448, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15420, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 2:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 2:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15450, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 2:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15422, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 3:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 0:
                metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 1, 0, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15449, -16, -16, 32, 32, 3, height, -16, -16, height);
                metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 0, 0, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 2:
                metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 2, 0, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 3:
                metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 3, 0, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 1, 0, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15421, -16, -16, 32, 32, 3, height, -16, -16, height);
                metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 0, 0, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 2:
                metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 2, 0, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 3:
                metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 3, 0, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            }
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    }
}

/** rct2: 0x008A67C0 */
static void looping_rc_track_diag_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 3:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15463, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 3:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15435, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 56, 0x20);
        break;
    case 1:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 0:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15460, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15432, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 56, 0x20);
        break;
    case 2:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 2:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15462, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 2:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15434, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 56, 0x20);
        break;
    case 3:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 0:
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 1, 8, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15461, -16, -16, 32, 32, 3, height, -16, -16, height);
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 0, 8, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 2:
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 2, 8, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 3:
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 3, 8, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 1, 8, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15433, -16, -16, 32, 32, 3, height, -16, -16, height);
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 0, 8, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 2:
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 2, 8, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 3:
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 3, 8, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            }
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 56, 0x20);
        break;
    }
}

/** rct2: 0x008A67F0 */
static void looping_rc_track_diag_60_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 3:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15475, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 3:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15447, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 104, 0x20);
        break;
    case 1:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 0:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15472, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15444, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 104, 0x20);
        break;
    case 2:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 2:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15474, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 2:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15446, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 104, 0x20);
        break;
    case 3:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 0:
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 1, 36, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15473, -16, -16, 32, 32, 3, height, -16, -16, height);
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 0, 36, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 2:
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 2, 36, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 3:
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 3, 36, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 1, 36, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15445, -16, -16, 32, 32, 3, height, -16, -16, height);
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 0, 36, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 2:
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 2, 36, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 3:
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 3, 36, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            }
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 104, 0x20);
        break;
    }
}

/** rct2: 0x008A67A0 */
static void looping_rc_track_diag_flat_to_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 3:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15455, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 3:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15427, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 1:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 0:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15452, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15424, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 2:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 2:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15454, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 2:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15426, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 3:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 0:
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 1, 0, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15453, -16, -16, 32, 32, 3, height, -16, -16, height);
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 0, 0, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 2:
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 2, 0, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 3:
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 3, 0, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 1, 0, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15425, -16, -16, 32, 32, 3, height, -16, -16, height);
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 0, 0, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 2:
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 2, 0, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 3:
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 3, 0, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            }
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    }
}

/** rct2: 0x008A67D0 */
static void looping_rc_track_diag_25_deg_up_to_60_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 3:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15467, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 3:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15439, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 1:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 0:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15464, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15436, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 2:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 2:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15466, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 2:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15438, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 3:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 0:
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 1, 16, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15465, -16, -16, 32, 32, 3, height, -16, -16, height);
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 0, 16, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 2:
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 2, 16, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 3:
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 3, 16, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 1, 16, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15437, -16, -16, 32, 32, 3, height, -16, -16, height);
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 0, 16, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 2:
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 2, 16, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 3:
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 3, 16, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            }
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    }
}

/** rct2: 0x008A67E0 */
static void looping_rc_track_diag_60_deg_up_to_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 3:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15471, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 3:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15443, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 1:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 0:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15468, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15440, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 2:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 2:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15470, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 2:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15442, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 3:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 0:
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 1, 21, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15469, -16, -16, 16, 16, 3, height, 0, 0, height);
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 0, 21, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 2:
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 2, 21, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 3:
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 3, 21, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 1, 21, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15441, -16, -16, 16, 16, 3, height, 0, 0, height);
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 0, 21, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 2:
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 2, 21, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 3:
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 3, 21, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            }
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    }
}

/** rct2: 0x008A67B0 */
static void looping_rc_track_diag_25_deg_up_to_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 3:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15459, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 3:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15431, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 56, 0x20);
        break;
    case 1:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 0:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15456, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15428, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 56, 0x20);
        break;
    case 2:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 2:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15458, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 2:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15430, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 56, 0x20);
        break;
    case 3:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 0:
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 1, 4, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15457, -16, -16, 32, 32, 3, height, -16, -16, height);
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 0, 4, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 2:
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 2, 4, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 3:
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 3, 4, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 1, 4, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15429, -16, -16, 32, 32, 3, height, -16, -16, height);
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 0, 4, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 2:
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 2, 4, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 3:
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 3, 4, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            }
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 56, 0x20);
        break;
    }
}

/** rct2: 0x008A6820 */
static void looping_rc_track_diag_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 3:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15461, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 3:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15433, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 56, 0x20);
        break;
    case 1:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 0:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15462, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15434, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 56, 0x20);
        break;
    case 2:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 2:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15460, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 2:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15432, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 56, 0x20);
        break;
    case 3:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 0:
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 1, 8, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15463, -16, -16, 32, 32, 3, height, -16, -16, height);
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 0, 8, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 2:
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 2, 8, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 3:
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 3, 8, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 1, 8, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15435, -16, -16, 32, 32, 3, height, -16, -16, height);
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 0, 8, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 2:
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 2, 8, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 3:
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 3, 8, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            }
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 56, 0x20);
        break;
    }
}

/** rct2: 0x008A6850 */
static void looping_rc_track_diag_60_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 3:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15473, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 3:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15445, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 104, 0x20);
        break;
    case 1:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 0:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15474, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15446, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 104, 0x20);
        break;
    case 2:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 2:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15472, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 2:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15444, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 104, 0x20);
        break;
    case 3:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 0:
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 1, 28, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15475, -16, -16, 32, 32, 3, height, -16, -16, height);
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 0, 28, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 2:
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 2, 28, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 3:
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 3, 28, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 1, 28, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15447, -16, -16, 32, 32, 3, height, -16, -16, height);
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 0, 28, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 2:
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 2, 28, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 3:
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 3, 28, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            }
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 104, 0x20);
        break;
    }
}

/** rct2: 0x008A6800 */
static void looping_rc_track_diag_flat_to_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 3:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15457, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 3:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15429, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        break;
    case 1:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 0:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15458, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15430, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        break;
    case 2:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 2:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15456, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 2:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15428, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        break;
    case 3:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 0:
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 1, 4, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15459, -16, -16, 32, 32, 3, height, -16, -16, height);
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 0, 4, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 2:
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 2, 4, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 3:
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 3, 4, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 1, 4, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15431, -16, -16, 32, 32, 3, height, -16, -16, height);
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 0, 4, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 2:
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 2, 4, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 3:
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 3, 4, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            }
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        break;
    }

    paint_util_set_general_support_height(height + 56, 0x20);
}

/** rct2: 0x008A6830 */
static void looping_rc_track_diag_25_deg_down_to_60_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 3:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15469, -16, -16, 16, 16, 3, height, 0, 0, height);
                break;
            }
        } else {
            switch (direction) {
            case 3:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15441, -16, -16, 16, 16, 3, height, 0, 0, height);
                break;
            }
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 1:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 0:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15470, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15442, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 2:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 2:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15468, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 2:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15440, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 3:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 0:
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 1, 17, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15471, -16, -16, 32, 32, 3, height, -16, -16, height);
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 0, 17, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 2:
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 2, 17, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 3:
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 3, 17, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 1, 17, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15443, -16, -16, 32, 32, 3, height, -16, -16, height);
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 0, 17, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 2:
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 2, 17, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 3:
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 3, 17, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            }
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    }
}

/** rct2: 0x008A6840 */
static void looping_rc_track_diag_60_deg_down_to_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 3:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15465, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 3:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15437, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 1:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 0:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15466, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15438, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 2:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 2:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15464, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 2:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15436, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 3:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 0:
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 1, 8, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15467, -16, -16, 32, 32, 3, height, -16, -16, height);
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 0, 8, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 2:
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 2, 8, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 3:
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 3, 8, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 1, 8, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15439, -16, -16, 32, 32, 3, height, -16, -16, height);
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 0, 8, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 2:
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 2, 8, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 3:
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 3, 8, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            }
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    }
}

/** rct2: 0x008A6810 */
static void looping_rc_track_diag_25_deg_down_to_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 3:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15453, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 3:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15425, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 1:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 0:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15454, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15426, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 2:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 2:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15452, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 2:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15424, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 3:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 0:
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 1, 0, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15455, -16, -16, 32, 32, 3, height, -16, -16, height);
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 0, 0, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 2:
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 2, 0, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 3:
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 3, 0, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 1, 0, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15427, -16, -16, 32, 32, 3, height, -16, -16, height);
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 0, 0, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 2:
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 2, 0, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 3:
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 3, 0, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            }
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    }
}

/** rct2: 0x008A6900 */
static void looping_rc_track_diag_flat_to_left_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15503, -16, -16, 32, 32, 3, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15500, -16, -16, 32, 32, 3, height, -16, -16, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15504, -16, -16, 32, 32, 0, height, -16, -16, height + 27);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 2:
        switch (direction) {
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15502, -16, -16, 32, 32, 3, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 1, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15501, -16, -16, 32, 32, 3, height, -16, -16, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 0, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 2, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 3, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    }
}

/** rct2: 0x008A6910 */
static void looping_rc_track_diag_flat_to_right_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15508, -16, -16, 32, 32, 3, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15505, -16, -16, 32, 32, 3, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 2:
        switch (direction) {
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15507, -16, -16, 32, 32, 3, height, -16, -16, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15509, -16, -16, 32, 32, 0, height, -16, -16, height + 27);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 1, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15506, -16, -16, 32, 32, 3, height, -16, -16, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 0, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 2, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 3, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    }
}

/** rct2: 0x008A6920 */
static void looping_rc_track_diag_left_bank_to_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15506, -16, -16, 32, 32, 3, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15507, -16, -16, 32, 32, 3, height, -16, -16, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15509, -16, -16, 32, 32, 0, height, -16, -16, height + 27);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 2:
        switch (direction) {
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15505, -16, -16, 32, 32, 3, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 1, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15508, -16, -16, 32, 32, 3, height, -16, -16, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 0, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 2, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 3, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    }
}

/** rct2: 0x008A6930 */
static void looping_rc_track_diag_right_bank_to_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15501, -16, -16, 32, 32, 3, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15502, -16, -16, 32, 32, 3, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 2:
        switch (direction) {
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15500, -16, -16, 32, 32, 3, height, -16, -16, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15504, -16, -16, 32, 32, 0, height, -16, -16, height + 27);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 1, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15503, -16, -16, 32, 32, 3, height, -16, -16, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 0, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 2, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 3, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    }
}

/** rct2: 0x008A6960 */
static void looping_rc_track_diag_left_bank_to_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15493, -16, -16, 32, 32, 3, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15490, -16, -16, 32, 32, 3, height, -16, -16, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15494, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 2:
        switch (direction) {
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15492, -16, -16, 32, 32, 3, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 1, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15491, -16, -16, 32, 32, 3, height, -16, -16, height);
            metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 0, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 2, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 3, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    }
}

/** rct2: 0x008A6970 */
static void looping_rc_track_diag_right_bank_to_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15498, -16, -16, 32, 32, 3, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15495, -16, -16, 32, 32, 3, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 2:
        switch (direction) {
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15497, -16, -16, 32, 32, 3, height, -16, -16, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15499, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 1, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15496, -16, -16, 32, 32, 3, height, -16, -16, height);
            metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 0, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 2, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 3, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    }
}

/** rct2: 0x008A6940 */
static void looping_rc_track_diag_25_deg_up_to_left_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15483, -16, -16, 32, 32, 3, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 56, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15480, -16, -16, 32, 32, 3, height, -16, -16, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15484, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 56, 0x20);
        break;
    case 2:
        switch (direction) {
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15482, -16, -16, 32, 32, 3, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 56, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 1, 4, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15481, -16, -16, 32, 32, 3, height, -16, -16, height);
            metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 0, 4, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 2, 4, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 3, 4, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 56, 0x20);
        break;
    }
}

/** rct2: 0x008A6950 */
static void looping_rc_track_diag_25_deg_up_to_right_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15488, -16, -16, 32, 32, 3, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 56, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15485, -16, -16, 32, 32, 3, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 56, 0x20);
        break;
    case 2:
        switch (direction) {
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15487, -16, -16, 32, 32, 3, height, -16, -16, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15489, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 56, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 1, 4, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15486, -16, -16, 32, 32, 3, height, -16, -16, height);
            metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 0, 4, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 2, 4, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 3, 4, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 56, 0x20);
        break;
    }
}

/** rct2: 0x008A6980 */
static void looping_rc_track_diag_left_bank_to_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15486, -16, -16, 32, 32, 3, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15487, -16, -16, 32, 32, 3, height, -16, -16, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15489, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        break;
    case 2:
        switch (direction) {
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15485, -16, -16, 32, 32, 3, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        break;
    case 3:
        switch (direction) {
        case 0:
            metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 1, 4, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15488, -16, -16, 32, 32, 3, height, -16, -16, height);
            metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 0, 4, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 2, 4, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 3, 4, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        break;
    }

    paint_util_set_general_support_height(height + 56, 0x20);
}

/** rct2: 0x008A6990 */
static void looping_rc_track_diag_right_bank_to_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15481, -16, -16, 32, 32, 3, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15482, -16, -16, 32, 32, 3, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        break;
    case 2:
        switch (direction) {
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15480, -16, -16, 32, 32, 3, height, -16, -16, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15484, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        break;
    case 3:
        switch (direction) {
        case 0:
            metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 1, 4, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15483, -16, -16, 32, 32, 3, height, -16, -16, height);
            metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 0, 4, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 2, 4, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 3, 4, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        break;
    }

    paint_util_set_general_support_height(height + 56, 0x20);
}

/** rct2: 0x008A69A0 */
static void looping_rc_track_diag_25_deg_down_to_left_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15496, -16, -16, 32, 32, 3, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15497, -16, -16, 32, 32, 3, height, -16, -16, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15499, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 2:
        switch (direction) {
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15495, -16, -16, 32, 32, 3, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 1, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15498, -16, -16, 32, 32, 3, height, -16, -16, height);
            metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 0, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 2, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 3, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    }
}

/** rct2: 0x008A69B0 */
static void looping_rc_track_diag_25_deg_down_to_right_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15491, -16, -16, 32, 32, 3, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15492, -16, -16, 32, 32, 3, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 2:
        switch (direction) {
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15490, -16, -16, 32, 32, 3, height, -16, -16, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15494, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 1, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15493, -16, -16, 32, 32, 3, height, -16, -16, height);
            metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 0, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 2, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 3, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    }
}

/** rct2: 0x008A68E0 */
static void looping_rc_track_diag_left_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15479, -16, -16, 32, 32, 3, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15476, -16, -16, 32, 32, 0, height, -16, -16, height + 27);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 2:
        switch (direction) {
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15478, -16, -16, 32, 32, 3, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 1, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15477, -16, -16, 32, 32, 3, height, -16, -16, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 0, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 2, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 3, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    }
}

/** rct2: 0x008A68F0 */
static void looping_rc_track_diag_right_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15477, -16, -16, 32, 32, 3, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15478, -16, -16, 32, 32, 3, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 2:
        switch (direction) {
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15476, -16, -16, 32, 32, 0, height, -16, -16, height + 27);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 1, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15479, -16, -16, 32, 32, 3, height, -16, -16, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 0, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 2, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 3, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    }
}

/** rct2: 0x008A6C00 */
static void looping_rc_track_block_brakes(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
    case 2:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15012, 0, 0, 32, 20, 3, height, 0, 6, height);
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15014, 0, 0, 32, 1, 26, height, 0, 27, height);
        break;
    case 1:
    case 3:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15013, 0, 0, 32, 20, 3, height, 0, 6, height);
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15015, 0, 0, 32, 1, 26, height, 0, 27, height);
        break;
    }
    if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
    }
    paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
    paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(height + 32, 0x20);
}

/** rct2: 0x008A6BC0 */
static void looping_rc_track_left_banked_quarter_turn_3_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15689, 0, 6, 32, 20, 3, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15691, 0, 6, 32, 1, 34, height, 0, 27, height);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15693, 0, 6, 32, 20, 3, height);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15687, 0, 6, 32, 20, 3, height);
            break;
        }
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_1);
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 1:
        paint_util_set_general_support_height(height + 56, 0x20);
        break;
    case 2:
        paint_util_set_general_support_height(height + 56, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15688, 6, 0, 20, 32, 3, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15690, 6, 0, 1, 32, 34, height, 27, 0, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15692, 6, 0, 1, 32, 34, height, 27, 0, height);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15686, 6, 0, 20, 32, 3, height);
            break;
        }
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
        switch (direction) {
        case 2:
            paint_util_push_tunnel_right(height + 8, TUNNEL_2);
            break;
        case 3:
            paint_util_push_tunnel_left(height + 8, TUNNEL_2);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    }
}

/** rct2: 0x008A6BD0 */
static void looping_rc_track_right_banked_quarter_turn_3_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15678, 0, 6, 32, 20, 3, height);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15680, 0, 6, 32, 20, 3, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15682, 0, 6, 32, 1, 34, height, 0, 27, height);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15684, 0, 6, 32, 20, 3, height);
            break;
        }
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_1);
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 1:
        paint_util_set_general_support_height(height + 56, 0x20);
        break;
    case 2:
        paint_util_set_general_support_height(height + 56, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15679, 6, 0, 20, 32, 3, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15681, 6, 0, 1, 32, 34, height, 27, 0, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15683, 6, 0, 1, 32, 34, height, 27, 0, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 10, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15685, 6, 0, 20, 32, 3, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }
        switch (direction) {
        case 0:
            paint_util_push_tunnel_right(height + 8, TUNNEL_2);
            break;
        case 1:
            paint_util_push_tunnel_left(height + 8, TUNNEL_2);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    }
}

/** rct2: 0x008A6BE0 */
static void looping_rc_track_left_banked_quarter_turn_3_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    looping_rc_track_right_banked_quarter_turn_3_25_deg_up(rideIndex, trackSequence, (direction + 1) & 3, height, mapElement);
}

/** rct2: 0x008A6BF0 */
static void looping_rc_track_right_banked_quarter_turn_3_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    looping_rc_track_left_banked_quarter_turn_3_25_deg_up(rideIndex, trackSequence, (direction - 1) & 3, height, mapElement);
}

/** rct2: 0x008A6B80 */
static void looping_rc_track_left_banked_quarter_turn_5_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15658, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15663, 0, 0, 32, 1, 34, height, 0, 27, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15668, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15673, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        }
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_1);
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 1:
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15659, 0, 0, 32, 16, 3, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15664, 0, 0, 1, 1, 34, height, 30, 30, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15669, 0, 0, 32, 16, 3, height, 0, 16, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15674, 0, 0, 32, 16, 3, height, 0, 16, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15660, 0, 0, 16, 16, 3, height, 0, 16, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15665, 0, 0, 1, 1, 34, height, 30, 30, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15670, 0, 0, 16, 16, 3, height, 16, 0, height);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15675, 0, 0, 16, 16, 3, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 64, 0x20);
        break;
    case 4:
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 5:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15661, 0, 0, 16, 32, 3, height, 16, 0, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15666, 0, 0, 1, 1, 34, height, 30, 30, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15671, 0, 0, 1, 32, 34, height, 27, 0, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15676, 0, 0, 16, 32, 3, height, 16, 0, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 6:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15662, 0, 0, 20, 32, 3, height, 6, 0, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15667, 0, 0, 1, 32, 34, height, 27, 0, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15672, 0, 0, 1, 32, 34, height, 27, 0, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15677, 0, 0, 20, 32, 3, height, 6, 0, height);
            break;
        }
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
        switch (direction) {
        case 2:
            paint_util_push_tunnel_right(height + 8, TUNNEL_2);
            break;
        case 3:
            paint_util_push_tunnel_left(height + 8, TUNNEL_2);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    }
}

/** rct2: 0x008A6B90 */
static void looping_rc_track_right_banked_quarter_turn_5_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15638, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15643, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15648, 0, 0, 32, 1, 34, height, 0, 27, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15653, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        }
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_1);
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 1:
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15639, 0, 0, 32, 16, 3, height, 0, 16, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15644, 0, 0, 32, 16, 3, height, 0, 16, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15649, 0, 0, 1, 1, 34, height, 30, 30, height);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15654, 0, 0, 32, 16, 3, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15640, 0, 0, 16, 16, 3, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15645, 0, 0, 16, 16, 3, height, 16, 0, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15650, 0, 0, 1, 1, 34, height, 30, 30, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15655, 0, 0, 16, 16, 3, height, 0, 16, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 64, 0x20);
        break;
    case 4:
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 5:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15641, 0, 0, 16, 32, 3, height, 16, 0, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15646, 0, 0, 1, 32, 34, height, 27, 0, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15651, 0, 0, 1, 1, 34, height, 30, 30, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15656, 0, 0, 16, 32, 3, height, 16, 0, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 6:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15642, 0, 0, 20, 32, 3, height, 6, 0, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15647, 0, 0, 1, 32, 34, height, 27, 0, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15652, 0, 0, 1, 32, 34, height, 27, 0, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15657, 0, 0, 20, 32, 3, height, 6, 0, height);
            break;
        }
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
        switch (direction) {
        case 0:
            paint_util_push_tunnel_right(height + 8, TUNNEL_2);
            break;
        case 1:
            paint_util_push_tunnel_left(height + 8, TUNNEL_2);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    }
}

/** rct2: 0x008A6BA0 */
static void looping_rc_track_left_banked_quarter_turn_5_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    looping_rc_track_right_banked_quarter_turn_5_25_deg_up(rideIndex, trackSequence, (direction + 1) & 3, height, mapElement);
}

/** rct2: 0x008A6BB0 */
static void looping_rc_track_right_banked_quarter_turn_5_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    looping_rc_track_left_banked_quarter_turn_5_25_deg_up(rideIndex, trackSequence, (direction - 1) & 3, height, mapElement);
}

/** rct2: 0x008A6A80 */
static void looping_rc_track_25_deg_up_to_left_banked_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15602, 0, 0, 32, 20, 3, height, 0, 6, height);
        break;
    case 1:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15603, 0, 0, 32, 20, 3, height, 0, 6, height);
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15610, 0, 0, 32, 1, 34, height, 0, 27, height);
        break;
    case 2:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15604, 0, 0, 32, 20, 3, height, 0, 6, height);
        break;
    case 3:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15605, 0, 0, 32, 20, 3, height, 0, 6, height);
        break;
    }
    if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
    }
    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_1);
    } else {
        paint_util_push_tunnel_rotated(direction, height + 8, TUNNEL_2);
    }
    paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(height + 56, 0x20);
}

/** rct2: 0x008A6A90 */
static void looping_rc_track_25_deg_up_to_right_banked_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15606, 0, 0, 32, 20, 3, height, 0, 6, height);
        break;
    case 1:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15607, 0, 0, 32, 20, 3, height, 0, 6, height);
        break;
    case 2:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15608, 0, 0, 32, 20, 3, height, 0, 6, height);
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15611, 0, 0, 32, 1, 34, height, 0, 27, height);
        break;
    case 3:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15609, 0, 0, 32, 20, 3, height, 0, 6, height);
        break;
    }
    if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
    }
    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_1);
    } else {
        paint_util_push_tunnel_rotated(direction, height + 8, TUNNEL_2);
    }
    paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(height + 56, 0x20);
}

/** rct2: 0x008A6AA0 */
static void looping_rc_track_left_banked_25_deg_up_to_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15612, 0, 0, 32, 20, 3, height, 0, 6, height);
        break;
    case 1:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15613, 0, 0, 32, 20, 3, height, 0, 6, height);
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15620, 0, 0, 32, 1, 34, height, 0, 27, height);
        break;
    case 2:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15614, 0, 0, 32, 20, 3, height, 0, 6, height);
        break;
    case 3:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15615, 0, 0, 32, 20, 3, height, 0, 6, height);
        break;
    }
    if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
    }
    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_1);
    } else {
        paint_util_push_tunnel_rotated(direction, height + 8, TUNNEL_2);
    }
    paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(height + 56, 0x20);
}

/** rct2: 0x008A6AB0 */
static void looping_rc_track_right_banked_25_deg_up_to_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15616, 0, 0, 32, 20, 3, height, 0, 6, height);
        break;
    case 1:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15617, 0, 0, 32, 20, 3, height, 0, 6, height);
        break;
    case 2:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15618, 0, 0, 32, 20, 3, height, 0, 6, height);
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15621, 0, 0, 32, 1, 34, height, 0, 27, height);
        break;
    case 3:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15619, 0, 0, 32, 20, 3, height, 0, 6, height);
        break;
    }
    if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
    }
    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_1);
    } else {
        paint_util_push_tunnel_rotated(direction, height + 8, TUNNEL_2);
    }
    paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(height + 56, 0x20);
}

/** rct2: 0x008A6AC0 */
static void looping_rc_track_25_deg_down_to_left_banked_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    looping_rc_track_right_banked_25_deg_up_to_25_deg_up(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008A6AD0 */
static void looping_rc_track_25_deg_down_to_right_banked_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    looping_rc_track_left_banked_25_deg_up_to_25_deg_up(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008A6AE0 */
static void looping_rc_track_left_banked_25_deg_down_to_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    looping_rc_track_25_deg_up_to_right_banked_25_deg_up(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008A6AF0 */
static void looping_rc_track_right_banked_25_deg_down_to_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    looping_rc_track_25_deg_up_to_left_banked_25_deg_up(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008A6B00 */
static void looping_rc_track_left_banked_flat_to_left_banked_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15622, 0, 0, 32, 1, 34, height, 0, 27, height);
        break;
    case 1:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15623, 0, 0, 32, 1, 34, height, 0, 27, height);
        break;
    case 2:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15624, 0, 0, 32, 20, 3, height, 0, 6, height);
        break;
    case 3:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15625, 0, 0, 32, 20, 3, height, 0, 6, height);
        break;
    }
    if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 3, height, gTrackColours[SCHEME_SUPPORTS]);
    }
    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
    } else {
        paint_util_push_tunnel_rotated(direction, height, TUNNEL_2);
    }
    paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(height + 48, 0x20);
}

/** rct2: 0x008A6B10 */
static void looping_rc_track_right_banked_flat_to_right_banked_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15626, 0, 0, 32, 20, 3, height, 0, 6, height);
        break;
    case 1:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15627, 0, 0, 32, 20, 3, height, 0, 6, height);
        break;
    case 2:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15628, 0, 0, 32, 1, 34, height, 0, 27, height);
        break;
    case 3:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15629, 0, 0, 32, 20, 3, height, 0, 6, height);
        break;
    }
    if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 3, height, gTrackColours[SCHEME_SUPPORTS]);
    }
    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
    } else {
        paint_util_push_tunnel_rotated(direction, height, TUNNEL_2);
    }
    paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(height + 48, 0x20);
}

/** rct2: 0x008A6B40 */
static void looping_rc_track_left_banked_25_deg_up_to_left_banked_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15630, 0, 0, 32, 1, 34, height, 0, 27, height);
        break;
    case 1:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15631, 0, 0, 32, 1, 34, height, 0, 27, height);
        break;
    case 2:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15632, 0, 0, 32, 20, 3, height, 0, 6, height);
        break;
    case 3:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15633, 0, 0, 32, 20, 3, height, 0, 6, height);
        break;
    }
    if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
    }
    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_0);
    } else {
        paint_util_push_tunnel_rotated(direction, height + 8, TUNNEL_12);
    }
    paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(height + 40, 0x20);
}

/** rct2: 0x008A6B50 */
static void looping_rc_track_right_banked_25_deg_up_to_right_banked_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15634, 0, 0, 32, 20, 3, height, 0, 6, height);
        break;
    case 1:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15635, 0, 0, 32, 20, 3, height, 0, 6, height);
        break;
    case 2:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15636, 0, 0, 32, 1, 34, height, 0, 27, height);
        break;
    case 3:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15637, 0, 0, 32, 20, 3, height, 0, 6, height);
        break;
    }
    if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
    }
    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_0);
    } else {
        paint_util_push_tunnel_rotated(direction, height + 8, TUNNEL_12);
    }
    paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(height + 40, 0x20);
}

/** rct2: 0x008A6B60 */
static void looping_rc_track_left_banked_flat_to_left_banked_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    looping_rc_track_right_banked_25_deg_up_to_right_banked_flat(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008A6B70 */
static void looping_rc_track_right_banked_flat_to_right_banked_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    looping_rc_track_left_banked_25_deg_up_to_left_banked_flat(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008A6B20 */
static void looping_rc_track_left_banked_25_deg_down_to_left_banked_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    looping_rc_track_right_banked_flat_to_right_banked_25_deg_up(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008A6B30 */
static void looping_rc_track_right_banked_25_deg_down_to_right_banked_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    looping_rc_track_left_banked_flat_to_left_banked_25_deg_up(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008A69C0 */
static void looping_rc_track_flat_to_left_banked_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15574, 0, 0, 32, 20, 3, height, 0, 6, height);
        break;
    case 1:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15575, 0, 0, 32, 20, 3, height, 0, 6, height);
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15582, 0, 0, 32, 1, 34, height, 0, 27, height);
        break;
    case 2:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15576, 0, 0, 32, 20, 3, height, 0, 6, height);
        break;
    case 3:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15577, 0, 0, 32, 20, 3, height, 0, 6, height);
        break;
    }
    if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 3, height, gTrackColours[SCHEME_SUPPORTS]);
    }
    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
    } else {
        paint_util_push_tunnel_rotated(direction, height, TUNNEL_2);
    }
    paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(height + 48, 0x20);
}

/** rct2: 0x008A69D0 */
static void looping_rc_track_flat_to_right_banked_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15578, 0, 0, 32, 20, 3, height, 0, 6, height);
        break;
    case 1:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15579, 0, 0, 32, 20, 3, height, 0, 6, height);
        break;
    case 2:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15580, 0, 0, 32, 20, 3, height, 0, 6, height);
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15583, 0, 0, 32, 1, 34, height, 0, 27, height);
        break;
    case 3:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15581, 0, 0, 32, 20, 3, height, 0, 6, height);
        break;
    }
    if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 3, height, gTrackColours[SCHEME_SUPPORTS]);
    }
    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
    } else {
        paint_util_push_tunnel_rotated(direction, height, TUNNEL_2);
    }
    paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(height + 48, 0x20);
}

/** rct2: 0x008A69E0 */
static void looping_rc_track_left_banked_25_deg_up_to_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15584, 0, 0, 32, 20, 3, height, 0, 6, height);
        break;
    case 1:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15585, 0, 0, 32, 20, 3, height, 0, 6, height);
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15592, 0, 0, 32, 1, 34, height, 0, 27, height);
        break;
    case 2:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15586, 0, 0, 32, 20, 3, height, 0, 6, height);
        break;
    case 3:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15587, 0, 0, 32, 20, 3, height, 0, 6, height);
        break;
    }
    if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
    }
    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_0);
    } else {
        paint_util_push_tunnel_rotated(direction, height + 8, TUNNEL_12);
    }
    paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(height + 40, 0x20);
}

/** rct2: 0x008A69F0 */
static void looping_rc_track_right_banked_25_deg_up_to_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15588, 0, 0, 32, 20, 3, height, 0, 6, height);
        break;
    case 1:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15589, 0, 0, 32, 20, 3, height, 0, 6, height);
        break;
    case 2:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15590, 0, 0, 32, 20, 3, height, 0, 6, height);
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15593, 0, 0, 32, 1, 34, height, 0, 27, height);
        break;
    case 3:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 15591, 0, 0, 32, 20, 3, height, 0, 6, height);
        break;
    }
    if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
    }
    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_0);
    } else {
        paint_util_push_tunnel_rotated(direction, height + 8, TUNNEL_12);
    }
    paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(height + 40, 0x20);
}

/** rct2: 0x008A6A00 */
static void looping_rc_track_flat_to_left_banked_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    looping_rc_track_right_banked_25_deg_up_to_flat(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008A6A10 */
static void looping_rc_track_flat_to_right_banked_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    looping_rc_track_left_banked_25_deg_up_to_flat(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008A6A20 */
static void looping_rc_track_left_banked_25_deg_down_to_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    looping_rc_track_flat_to_right_banked_25_deg_up(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008A6A30 */
static void looping_rc_track_right_banked_25_deg_down_to_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    looping_rc_track_flat_to_left_banked_25_deg_up(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

static void looping_rc_track_booster(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    if (!is_csg_loaded())
    {
        looping_rc_track_brakes(rideIndex, trackSequence, direction, height, mapElement);
        return;
    }

    uint32 sprite_ne_sw = SPR_CSG_BEGIN + 55679;
    uint32 sprite_nw_se = SPR_CSG_BEGIN + 55680;


    switch (direction) {
        case 0:
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | sprite_ne_sw, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 1:
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | sprite_nw_se, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
    }
    if (track_paint_util_should_paint_supports(gPaintMapPosition)) {
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
    }
    paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
    paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(height + 32, 0x20);
}

TRACK_PAINT_FUNCTION get_track_paint_function_looping_rc(sint32 trackType, sint32 direction)
{
    switch (trackType) {
    case TRACK_ELEM_FLAT:
        return looping_rc_track_flat;
    case TRACK_ELEM_END_STATION:
    case TRACK_ELEM_BEGIN_STATION:
    case TRACK_ELEM_MIDDLE_STATION:
        return looping_rc_track_station;
    case TRACK_ELEM_25_DEG_UP:
        return looping_rc_track_25_deg_up;
    case TRACK_ELEM_60_DEG_UP:
        return looping_rc_track_60_deg_up;
    case TRACK_ELEM_FLAT_TO_25_DEG_UP:
        return looping_rc_track_flat_to_25_deg_up;
    case TRACK_ELEM_25_DEG_UP_TO_60_DEG_UP:
        return looping_rc_track_25_deg_up_to_60_deg_up;
    case TRACK_ELEM_60_DEG_UP_TO_25_DEG_UP:
        return looping_rc_track_60_deg_up_to_25_deg_up;
    case TRACK_ELEM_25_DEG_UP_TO_FLAT:
        return looping_rc_track_25_deg_up_to_flat;
    case TRACK_ELEM_25_DEG_DOWN:
        return looping_rc_track_25_deg_down;
    case TRACK_ELEM_60_DEG_DOWN:
        return looping_rc_track_60_deg_down;
    case TRACK_ELEM_FLAT_TO_25_DEG_DOWN:
        return looping_rc_track_flat_to_25_deg_down;
    case TRACK_ELEM_25_DEG_DOWN_TO_60_DEG_DOWN:
        return looping_rc_track_25_deg_down_to_60_deg_down;
    case TRACK_ELEM_60_DEG_DOWN_TO_25_DEG_DOWN:
        return looping_rc_track_60_deg_down_to_25_deg_down;
    case TRACK_ELEM_25_DEG_DOWN_TO_FLAT:
        return looping_rc_track_25_deg_down_to_flat;
    case TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES:
        return looping_rc_track_left_quarter_turn_5;
    case TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES:
        return looping_rc_track_right_quarter_turn_5;
    case TRACK_ELEM_FLAT_TO_LEFT_BANK:
        return looping_rc_track_flat_to_left_bank;
    case TRACK_ELEM_FLAT_TO_RIGHT_BANK:
        return looping_rc_track_flat_to_right_bank;
    case TRACK_ELEM_LEFT_BANK_TO_FLAT:
        return looping_rc_track_left_bank_to_flat;
    case TRACK_ELEM_RIGHT_BANK_TO_FLAT:
        return looping_rc_track_right_bank_to_flat;
    case TRACK_ELEM_BANKED_LEFT_QUARTER_TURN_5_TILES:
        return looping_rc_track_banked_left_quarter_turn_5;
    case TRACK_ELEM_BANKED_RIGHT_QUARTER_TURN_5_TILES:
        return looping_rc_track_banked_right_quarter_turn_5;
    case TRACK_ELEM_LEFT_BANK_TO_25_DEG_UP:
        return looping_rc_track_left_bank_to_25_deg_up;
    case TRACK_ELEM_RIGHT_BANK_TO_25_DEG_UP:
        return looping_rc_track_right_bank_to_25_deg_up;
    case TRACK_ELEM_25_DEG_UP_TO_LEFT_BANK:
        return looping_rc_track_25_deg_up_to_left_bank;
    case TRACK_ELEM_25_DEG_UP_TO_RIGHT_BANK:
        return looping_rc_track_25_deg_up_to_right_bank;
    case TRACK_ELEM_LEFT_BANK_TO_25_DEG_DOWN:
        return looping_rc_track_left_bank_to_25_deg_down;
    case TRACK_ELEM_RIGHT_BANK_TO_25_DEG_DOWN:
        return looping_rc_track_right_bank_to_25_deg_down;
    case TRACK_ELEM_25_DEG_DOWN_TO_LEFT_BANK:
        return looping_rc_track_25_deg_down_to_left_bank;
    case TRACK_ELEM_25_DEG_DOWN_TO_RIGHT_BANK:
        return looping_rc_track_25_deg_down_to_right_bank;
    case TRACK_ELEM_LEFT_BANK:
        return looping_rc_track_left_bank;
    case TRACK_ELEM_RIGHT_BANK:
        return looping_rc_track_right_bank;
    case TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES_25_DEG_UP:
        return looping_rc_track_left_quarter_turn_5_25_deg_up;
    case TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES_25_DEG_UP:
        return looping_rc_track_right_quarter_turn_5_25_deg_up;
    case TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES_25_DEG_DOWN:
        return looping_rc_track_left_quarter_turn_5_25_deg_down;
    case TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES_25_DEG_DOWN:
        return looping_rc_track_right_quarter_turn_5_25_deg_down;
    case TRACK_ELEM_S_BEND_LEFT:
        return looping_rc_track_s_bend_left;
    case TRACK_ELEM_S_BEND_RIGHT:
        return looping_rc_track_s_bend_right;
    case TRACK_ELEM_LEFT_VERTICAL_LOOP:
        return looping_rc_track_left_vertical_loop;
    case TRACK_ELEM_RIGHT_VERTICAL_LOOP:
        return looping_rc_track_right_vertical_loop;
    case TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES:
        return looping_rc_track_left_quarter_turn_3;
    case TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES:
        return looping_rc_track_right_quarter_turn_3;
    case TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_BANK:
        return looping_rc_track_left_quarter_turn_3_bank;
    case TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_BANK:
        return looping_rc_track_right_quarter_turn_3_bank;
    case TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_25_DEG_UP:
        return looping_rc_track_left_quarter_turn_3_25_deg_up;
    case TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_25_DEG_UP:
        return looping_rc_track_right_quarter_turn_3_25_deg_up;
    case TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_25_DEG_DOWN:
        return looping_rc_track_left_quarter_turn_3_25_deg_down;
    case TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_25_DEG_DOWN:
        return looping_rc_track_right_quarter_turn_3_25_deg_down;
    case TRACK_ELEM_LEFT_HALF_BANKED_HELIX_UP_SMALL:
        return looping_rc_track_left_half_banked_helix_up_small;
    case TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_UP_SMALL:
        return looping_rc_track_right_half_banked_helix_up_small;
    case TRACK_ELEM_LEFT_HALF_BANKED_HELIX_DOWN_SMALL:
        return looping_rc_track_left_half_banked_helix_down_small;
    case TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_DOWN_SMALL:
        return looping_rc_track_right_half_banked_helix_down_small;
    case TRACK_ELEM_LEFT_HALF_BANKED_HELIX_UP_LARGE:
        return looping_rc_track_left_half_banked_helix_up_large;
    case TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_UP_LARGE:
        return looping_rc_track_right_half_banked_helix_up_large;
    case TRACK_ELEM_LEFT_HALF_BANKED_HELIX_DOWN_LARGE:
        return looping_rc_track_left_half_banked_helix_down_large;
    case TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_DOWN_LARGE:
        return looping_rc_track_right_half_banked_helix_down_large;
    case TRACK_ELEM_LEFT_QUARTER_TURN_1_TILE_60_DEG_UP:
        return looping_rc_track_left_quarter_turn_1_60_deg_up;
    case TRACK_ELEM_RIGHT_QUARTER_TURN_1_TILE_60_DEG_UP:
        return looping_rc_track_right_quarter_turn_1_60_deg_up;
    case TRACK_ELEM_LEFT_QUARTER_TURN_1_TILE_60_DEG_DOWN:
        return looping_rc_track_left_quarter_turn_1_60_deg_down;
    case TRACK_ELEM_RIGHT_QUARTER_TURN_1_TILE_60_DEG_DOWN:
        return looping_rc_track_right_quarter_turn_1_60_deg_down;
    case TRACK_ELEM_BRAKES:
        return looping_rc_track_brakes;
    case TRACK_ELEM_25_DEG_UP_LEFT_BANKED:
        return looping_rc_track_25_deg_up_left_banked;
    case TRACK_ELEM_25_DEG_UP_RIGHT_BANKED:
        return looping_rc_track_25_deg_up_right_banked;
    case TRACK_ELEM_ON_RIDE_PHOTO:
        return looping_rc_track_on_ride_photo;
    case TRACK_ELEM_25_DEG_DOWN_LEFT_BANKED:
        return looping_rc_track_25_deg_down_left_banked;
    case TRACK_ELEM_25_DEG_DOWN_RIGHT_BANKED:
        return looping_rc_track_25_deg_down_right_banked;
    case TRACK_ELEM_LEFT_EIGHTH_TO_DIAG:
        return looping_rc_track_left_eighth_to_diag;
    case TRACK_ELEM_RIGHT_EIGHTH_TO_DIAG:
        return looping_rc_track_right_eighth_to_diag;
    case TRACK_ELEM_LEFT_EIGHTH_TO_ORTHOGONAL:
        return looping_rc_track_left_eighth_to_orthogonal;
    case TRACK_ELEM_RIGHT_EIGHTH_TO_ORTHOGONAL:
        return looping_rc_track_right_eighth_to_orthogonal;
    case TRACK_ELEM_LEFT_EIGHTH_BANK_TO_DIAG:
        return looping_rc_track_left_eighth_bank_to_diag;
    case TRACK_ELEM_RIGHT_EIGHTH_BANK_TO_DIAG:
        return looping_rc_track_right_eighth_bank_to_diag;
    case TRACK_ELEM_LEFT_EIGHTH_BANK_TO_ORTHOGONAL:
        return looping_rc_track_left_eighth_bank_to_orthogonal;
    case TRACK_ELEM_RIGHT_EIGHTH_BANK_TO_ORTHOGONAL:
        return looping_rc_track_right_eighth_bank_to_orthogonal;
    case TRACK_ELEM_DIAG_FLAT:
        return looping_rc_track_diag_flat;
    case TRACK_ELEM_DIAG_25_DEG_UP:
        return looping_rc_track_diag_25_deg_up;
    case TRACK_ELEM_DIAG_60_DEG_UP:
        return looping_rc_track_diag_60_deg_up;
    case TRACK_ELEM_DIAG_FLAT_TO_25_DEG_UP:
        return looping_rc_track_diag_flat_to_25_deg_up;
    case TRACK_ELEM_DIAG_25_DEG_UP_TO_60_DEG_UP:
        return looping_rc_track_diag_25_deg_up_to_60_deg_up;
    case TRACK_ELEM_DIAG_60_DEG_UP_TO_25_DEG_UP:
        return looping_rc_track_diag_60_deg_up_to_25_deg_up;
    case TRACK_ELEM_DIAG_25_DEG_UP_TO_FLAT:
        return looping_rc_track_diag_25_deg_up_to_flat;
    case TRACK_ELEM_DIAG_25_DEG_DOWN:
        return looping_rc_track_diag_25_deg_down;
    case TRACK_ELEM_DIAG_60_DEG_DOWN:
        return looping_rc_track_diag_60_deg_down;
    case TRACK_ELEM_DIAG_FLAT_TO_25_DEG_DOWN:
        return looping_rc_track_diag_flat_to_25_deg_down;
    case TRACK_ELEM_DIAG_25_DEG_DOWN_TO_60_DEG_DOWN:
        return looping_rc_track_diag_25_deg_down_to_60_deg_down;
    case TRACK_ELEM_DIAG_60_DEG_DOWN_TO_25_DEG_DOWN:
        return looping_rc_track_diag_60_deg_down_to_25_deg_down;
    case TRACK_ELEM_DIAG_25_DEG_DOWN_TO_FLAT:
        return looping_rc_track_diag_25_deg_down_to_flat;
    case TRACK_ELEM_DIAG_FLAT_TO_LEFT_BANK:
        return looping_rc_track_diag_flat_to_left_bank;
    case TRACK_ELEM_DIAG_FLAT_TO_RIGHT_BANK:
        return looping_rc_track_diag_flat_to_right_bank;
    case TRACK_ELEM_DIAG_LEFT_BANK_TO_FLAT:
        return looping_rc_track_diag_left_bank_to_flat;
    case TRACK_ELEM_DIAG_RIGHT_BANK_TO_FLAT:
        return looping_rc_track_diag_right_bank_to_flat;
    case TRACK_ELEM_DIAG_LEFT_BANK_TO_25_DEG_UP:
        return looping_rc_track_diag_left_bank_to_25_deg_up;
    case TRACK_ELEM_DIAG_RIGHT_BANK_TO_25_DEG_UP:
        return looping_rc_track_diag_right_bank_to_25_deg_up;
    case TRACK_ELEM_DIAG_25_DEG_UP_TO_LEFT_BANK:
        return looping_rc_track_diag_25_deg_up_to_left_bank;
    case TRACK_ELEM_DIAG_25_DEG_UP_TO_RIGHT_BANK:
        return looping_rc_track_diag_25_deg_up_to_right_bank;
    case TRACK_ELEM_DIAG_LEFT_BANK_TO_25_DEG_DOWN:
        return looping_rc_track_diag_left_bank_to_25_deg_down;
    case TRACK_ELEM_DIAG_RIGHT_BANK_TO_25_DEG_DOWN:
        return looping_rc_track_diag_right_bank_to_25_deg_down;
    case TRACK_ELEM_DIAG_25_DEG_DOWN_TO_LEFT_BANK:
        return looping_rc_track_diag_25_deg_down_to_left_bank;
    case TRACK_ELEM_DIAG_25_DEG_DOWN_TO_RIGHT_BANK:
        return looping_rc_track_diag_25_deg_down_to_right_bank;
    case TRACK_ELEM_DIAG_LEFT_BANK:
        return looping_rc_track_diag_left_bank;
    case TRACK_ELEM_DIAG_RIGHT_BANK:
        return looping_rc_track_diag_right_bank;
    case TRACK_ELEM_BLOCK_BRAKES:
        return looping_rc_track_block_brakes;
    case TRACK_ELEM_LEFT_BANKED_QUARTER_TURN_3_TILE_25_DEG_UP:
        return looping_rc_track_left_banked_quarter_turn_3_25_deg_up;
    case TRACK_ELEM_RIGHT_BANKED_QUARTER_TURN_3_TILE_25_DEG_UP:
        return looping_rc_track_right_banked_quarter_turn_3_25_deg_up;
    case TRACK_ELEM_LEFT_BANKED_QUARTER_TURN_3_TILE_25_DEG_DOWN:
        return looping_rc_track_left_banked_quarter_turn_3_25_deg_down;
    case TRACK_ELEM_RIGHT_BANKED_QUARTER_TURN_3_TILE_25_DEG_DOWN:
        return looping_rc_track_right_banked_quarter_turn_3_25_deg_down;
    case TRACK_ELEM_LEFT_BANKED_QUARTER_TURN_5_TILE_25_DEG_UP:
        return looping_rc_track_left_banked_quarter_turn_5_25_deg_up;
    case TRACK_ELEM_RIGHT_BANKED_QUARTER_TURN_5_TILE_25_DEG_UP:
        return looping_rc_track_right_banked_quarter_turn_5_25_deg_up;
    case TRACK_ELEM_LEFT_BANKED_QUARTER_TURN_5_TILE_25_DEG_DOWN:
        return looping_rc_track_left_banked_quarter_turn_5_25_deg_down;
    case TRACK_ELEM_RIGHT_BANKED_QUARTER_TURN_5_TILE_25_DEG_DOWN:
        return looping_rc_track_right_banked_quarter_turn_5_25_deg_down;
    case TRACK_ELEM_25_DEG_UP_TO_LEFT_BANKED_25_DEG_UP:
        return looping_rc_track_25_deg_up_to_left_banked_25_deg_up;
    case TRACK_ELEM_25_DEG_UP_TO_RIGHT_BANKED_25_DEG_UP:
        return looping_rc_track_25_deg_up_to_right_banked_25_deg_up;
    case TRACK_ELEM_LEFT_BANKED_25_DEG_UP_TO_25_DEG_UP:
        return looping_rc_track_left_banked_25_deg_up_to_25_deg_up;
    case TRACK_ELEM_RIGHT_BANKED_25_DEG_UP_TO_25_DEG_UP:
        return looping_rc_track_right_banked_25_deg_up_to_25_deg_up;
    case TRACK_ELEM_25_DEG_DOWN_TO_LEFT_BANKED_25_DEG_DOWN:
        return looping_rc_track_25_deg_down_to_left_banked_25_deg_down;
    case TRACK_ELEM_25_DEG_DOWN_TO_RIGHT_BANKED_25_DEG_DOWN:
        return looping_rc_track_25_deg_down_to_right_banked_25_deg_down;
    case TRACK_ELEM_LEFT_BANKED_25_DEG_DOWN_TO_25_DEG_DOWN:
        return looping_rc_track_left_banked_25_deg_down_to_25_deg_down;
    case TRACK_ELEM_RIGHT_BANKED_25_DEG_DOWN_TO_25_DEG_DOWN:
        return looping_rc_track_right_banked_25_deg_down_to_25_deg_down;
    case TRACK_ELEM_LEFT_BANKED_FLAT_TO_LEFT_BANKED_25_DEG_UP:
        return looping_rc_track_left_banked_flat_to_left_banked_25_deg_up;
    case TRACK_ELEM_RIGHT_BANKED_FLAT_TO_RIGHT_BANKED_25_DEG_UP:
        return looping_rc_track_right_banked_flat_to_right_banked_25_deg_up;
    case TRACK_ELEM_LEFT_BANKED_25_DEG_UP_TO_LEFT_BANKED_FLAT:
        return looping_rc_track_left_banked_25_deg_up_to_left_banked_flat;
    case TRACK_ELEM_RIGHT_BANKED_25_DEG_UP_TO_RIGHT_BANKED_FLAT:
        return looping_rc_track_right_banked_25_deg_up_to_right_banked_flat;
    case TRACK_ELEM_LEFT_BANKED_FLAT_TO_LEFT_BANKED_25_DEG_DOWN:
        return looping_rc_track_left_banked_flat_to_left_banked_25_deg_down;
    case TRACK_ELEM_RIGHT_BANKED_FLAT_TO_RIGHT_BANKED_25_DEG_DOWN:
        return looping_rc_track_right_banked_flat_to_right_banked_25_deg_down;
    case TRACK_ELEM_LEFT_BANKED_25_DEG_DOWN_TO_LEFT_BANKED_FLAT:
        return looping_rc_track_left_banked_25_deg_down_to_left_banked_flat;
    case TRACK_ELEM_RIGHT_BANKED_25_DEG_DOWN_TO_RIGHT_BANKED_FLAT:
        return looping_rc_track_right_banked_25_deg_down_to_right_banked_flat;
    case TRACK_ELEM_FLAT_TO_LEFT_BANKED_25_DEG_UP:
        return looping_rc_track_flat_to_left_banked_25_deg_up;
    case TRACK_ELEM_FLAT_TO_RIGHT_BANKED_25_DEG_UP:
        return looping_rc_track_flat_to_right_banked_25_deg_up;
    case TRACK_ELEM_LEFT_BANKED_25_DEG_UP_TO_FLAT:
        return looping_rc_track_left_banked_25_deg_up_to_flat;
    case TRACK_ELEM_RIGHT_BANKED_25_DEG_UP_TO_FLAT:
        return looping_rc_track_right_banked_25_deg_up_to_flat;
    case TRACK_ELEM_FLAT_TO_LEFT_BANKED_25_DEG_DOWN:
        return looping_rc_track_flat_to_left_banked_25_deg_down;
    case TRACK_ELEM_FLAT_TO_RIGHT_BANKED_25_DEG_DOWN:
        return looping_rc_track_flat_to_right_banked_25_deg_down;
    case TRACK_ELEM_LEFT_BANKED_25_DEG_DOWN_TO_FLAT:
        return looping_rc_track_left_banked_25_deg_down_to_flat;
    case TRACK_ELEM_RIGHT_BANKED_25_DEG_DOWN_TO_FLAT:
        return looping_rc_track_right_banked_25_deg_down_to_flat;

    case TRACK_ELEM_BOOSTER:
        return looping_rc_track_booster;

    }
    return NULL;
}
