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

/** rct2: 0x008A7114 */
static void stand_up_rc_track_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    if (track_element_is_lift_hill(mapElement)) {
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25383, 0, 6, 32, 20, 3, height);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25384, 0, 6, 32, 20, 3, height);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25453, 0, 6, 32, 20, 3, height);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25454, 0, 6, 32, 20, 3, height);
            break;
        }
        if (track_paint_util_should_paint_supports(gPaintSession.MapPosition)) {
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
        }
    } else {
        switch (direction) {
        case 0:
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25229, 0, 6, 32, 20, 3, height);
            break;
        case 1:
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25230, 0, 6, 32, 20, 3, height);
            break;
        }
        if (track_paint_util_should_paint_supports(gPaintSession.MapPosition)) {
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
        }
    }
    paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
    paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(height + 32, 0x20);
}

/** rct2: 0x008A7384, 0x008A7394, 0x008A73A4 */
static void stand_up_rc_track_station(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    static const uint32 imageIds[4][3] = {
        { 25567, 25571, SPR_STATION_BASE_A_SW_NE },
        { 25568, 25572, SPR_STATION_BASE_A_NW_SE },
        { 25567, 25571, SPR_STATION_BASE_A_SW_NE },
        { 25568, 25572, SPR_STATION_BASE_A_NW_SE },
    };

    if (mapElement->properties.track.type == TRACK_ELEM_END_STATION) {
        sub_98197C_rotated(direction, imageIds[direction][1] | gTrackColours[SCHEME_TRACK], 0, 6, 32, 20, 1, height, 0, 6, height + 3);
    } else {
        sub_98197C_rotated(direction, imageIds[direction][0] | gTrackColours[SCHEME_TRACK], 0, 6, 32, 20, 1, height, 0, 6, height + 3);
    }
    sub_98196C_rotated(direction, imageIds[direction][2] | gTrackColours[SCHEME_MISC], 0, 0, 32, 32, 1, height);
    track_paint_util_draw_station_metal_supports_2(direction, height, gTrackColours[SCHEME_SUPPORTS], 0);
    track_paint_util_draw_station_2(rideIndex, trackSequence, direction, height, mapElement, 9, 11);
    paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);
    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 32, 0x20);
}

/** rct2: 0x008A7124 */
static void stand_up_rc_track_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    if (track_element_is_lift_hill(mapElement)) {
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25385, 0, 6, 32, 20, 3, height);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25397, 0, 6, 32, 20, 3, height);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25389, 0, 6, 32, 20, 3, height);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25399, 0, 6, 32, 20, 3, height);
            break;
        }
        if (track_paint_util_should_paint_supports(gPaintSession.MapPosition)) {
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
        }
    } else {
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25231, 0, 6, 32, 20, 3, height);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25243, 0, 6, 32, 20, 3, height);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25235, 0, 6, 32, 20, 3, height);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25245, 0, 6, 32, 20, 3, height);
            break;
        }
        if (track_paint_util_should_paint_supports(gPaintSession.MapPosition)) {
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

/** rct2: 0x008A7134 */
static void stand_up_rc_track_60_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    if (track_element_is_lift_hill(mapElement)) {
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25386, 0, 6, 32, 20, 3, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25398, 0, 6, 1, 10, 75, height, 29, 10, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25390, 0, 6, 1, 10, 75, height, 29, 10, height);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25400, 0, 6, 32, 20, 3, height);
            break;
        }
        if (track_paint_util_should_paint_supports(gPaintSession.MapPosition)) {
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 32, height, gTrackColours[SCHEME_SUPPORTS]);
        }
    } else {
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25232, 0, 6, 32, 20, 3, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25244, 0, 6, 1, 10, 75, height, 29, 10, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25236, 0, 6, 1, 10, 75, height, 29, 10, height);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25246, 0, 6, 32, 20, 3, height);
            break;
        }
        if (track_paint_util_should_paint_supports(gPaintSession.MapPosition)) {
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

/** rct2: 0x008A7144 */
static void stand_up_rc_track_flat_to_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    if (track_element_is_lift_hill(mapElement)) {
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25387, 0, 6, 32, 20, 3, height);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25401, 0, 6, 32, 20, 3, height);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25391, 0, 6, 32, 20, 3, height);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25403, 0, 6, 32, 20, 3, height);
            break;
        }
        if (track_paint_util_should_paint_supports(gPaintSession.MapPosition)) {
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 3, height, gTrackColours[SCHEME_SUPPORTS]);
        }
    } else {
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25233, 0, 6, 32, 20, 3, height);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25247, 0, 6, 32, 20, 3, height);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25237, 0, 6, 32, 20, 3, height);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25249, 0, 6, 32, 20, 3, height);
            break;
        }
        if (track_paint_util_should_paint_supports(gPaintSession.MapPosition)) {
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

/** rct2: 0x008A7154 */
static void stand_up_rc_track_25_deg_up_to_60_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    if (track_element_is_lift_hill(mapElement)) {
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25388, 0, 6, 32, 20, 3, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25411, 0, 6, 32, 10, 43, height, 0, 10, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25402, 0, 6, 32, 2, 43, height, 0, 4, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25409, 0, 6, 32, 10, 43, height, 0, 10, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25392, 0, 6, 32, 2, 43, height, 0, 4, height);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25404, 0, 6, 32, 20, 3, height);
            break;
        }
        if (track_paint_util_should_paint_supports(gPaintSession.MapPosition)) {
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 12, height, gTrackColours[SCHEME_SUPPORTS]);
        }
    } else {
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25234, 0, 6, 32, 20, 3, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25277, 0, 6, 32, 10, 43, height, 0, 10, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25248, 0, 6, 32, 2, 43, height, 0, 4, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25275, 0, 6, 32, 10, 43, height, 0, 10, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25238, 0, 6, 32, 2, 43, height, 0, 4, height);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25250, 0, 6, 32, 20, 3, height);
            break;
        }
        if (track_paint_util_should_paint_supports(gPaintSession.MapPosition)) {
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

/** rct2: 0x008A7164 */
static void stand_up_rc_track_60_deg_up_to_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    if (track_element_is_lift_hill(mapElement)) {
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25396, 0, 6, 32, 20, 3, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25412, 0, 6, 32, 10, 43, height, 0, 10, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25408, 0, 6, 32, 2, 43, height, 0, 4, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25410, 0, 6, 32, 10, 43, height, 0, 10, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25394, 0, 6, 32, 2, 43, height, 0, 4, height);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25406, 0, 6, 32, 20, 3, height);
            break;
        }
        if (track_paint_util_should_paint_supports(gPaintSession.MapPosition)) {
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 20, height, gTrackColours[SCHEME_SUPPORTS]);
        }
    } else {
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25242, 0, 6, 32, 20, 3, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25278, 0, 6, 32, 10, 43, height, 0, 10, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25254, 0, 6, 32, 2, 43, height, 0, 4, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25276, 0, 6, 32, 10, 43, height, 0, 10, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25240, 0, 6, 32, 2, 43, height, 0, 4, height);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25252, 0, 6, 32, 20, 3, height);
            break;
        }
        if (track_paint_util_should_paint_supports(gPaintSession.MapPosition)) {
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

/** rct2: 0x008A7174 */
static void stand_up_rc_track_25_deg_up_to_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    if (track_element_is_lift_hill(mapElement)) {
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25395, 0, 6, 32, 20, 3, height);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25407, 0, 6, 32, 20, 3, height);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25393, 0, 6, 32, 20, 3, height);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25405, 0, 6, 32, 20, 3, height);
            break;
        }
        if (track_paint_util_should_paint_supports(gPaintSession.MapPosition)) {
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
        }
    } else {
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25241, 0, 6, 32, 20, 3, height);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25253, 0, 6, 32, 20, 3, height);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25239, 0, 6, 32, 20, 3, height);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25251, 0, 6, 32, 20, 3, height);
            break;
        }
        if (track_paint_util_should_paint_supports(gPaintSession.MapPosition)) {
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

/** rct2: 0x008A7184 */
static void stand_up_rc_track_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    stand_up_rc_track_25_deg_up(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008A7194 */
static void stand_up_rc_track_60_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    stand_up_rc_track_60_deg_up(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008A71A4 */
static void stand_up_rc_track_flat_to_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    stand_up_rc_track_25_deg_up_to_flat(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008A71B4 */
static void stand_up_rc_track_25_deg_down_to_60_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    stand_up_rc_track_60_deg_up_to_25_deg_up(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008A71C4 */
static void stand_up_rc_track_60_deg_down_to_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    stand_up_rc_track_25_deg_up_to_60_deg_up(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008A71D4 */
static void stand_up_rc_track_25_deg_down_to_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    stand_up_rc_track_flat_to_25_deg_up(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008A71E4 */
static void stand_up_rc_track_left_quarter_turn_5(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25264, 0, 6, 32, 20, 3, height);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25269, 0, 6, 32, 20, 3, height);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25274, 0, 6, 32, 20, 3, height);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25259, 0, 6, 32, 20, 3, height);
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
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25263, 0, 0, 32, 16, 3, height);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25268, 0, 0, 32, 16, 3, height);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25273, 0, 16, 32, 16, 3, height);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25258, 0, 16, 32, 16, 3, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25262, 0, 16, 16, 16, 3, height);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25267, 16, 16, 16, 16, 3, height);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25272, 16, 0, 16, 16, 3, height);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25257, 0, 0, 16, 16, 3, height);
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
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25261, 16, 0, 16, 32, 3, height);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25266, 0, 0, 16, 32, 3, height);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25271, 0, 0, 16, 32, 3, height);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25256, 16, 0, 16, 32, 3, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 6:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25260, 6, 0, 20, 32, 3, height);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25265, 6, 0, 20, 32, 3, height);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25270, 6, 0, 20, 32, 3, height);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25255, 6, 0, 20, 32, 3, height);
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

/** rct2: 0x008A71F4 */
static void stand_up_rc_track_right_quarter_turn_5(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    stand_up_rc_track_left_quarter_turn_5(rideIndex, trackSequence, (direction - 1) & 3, height, mapElement);
}

/** rct2: 0x008A7204 */
static void stand_up_rc_track_flat_to_left_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
        sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25281, 0, 6, 32, 20, 3, height);
        break;
    case 1:
        sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25286, 0, 6, 32, 20, 3, height);
        break;
    case 2:
        sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25280, 0, 6, 32, 20, 3, height);
        break;
    case 3:
        sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25283, 0, 6, 32, 20, 3, height);
        break;
    }
    if (track_paint_util_should_paint_supports(gPaintSession.MapPosition)) {
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
    }
    paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
    paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(height + 32, 0x20);
}

/** rct2: 0x008A7214 */
static void stand_up_rc_track_flat_to_right_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
        sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25279, 0, 6, 32, 20, 3, height);
        break;
    case 1:
        sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25284, 0, 6, 32, 20, 3, height);
        break;
    case 2:
        sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25282, 0, 6, 32, 20, 3, height);
        break;
    case 3:
        sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25285, 0, 6, 32, 20, 3, height);
        break;
    }
    if (track_paint_util_should_paint_supports(gPaintSession.MapPosition)) {
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
    }
    paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
    paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(height + 32, 0x20);
}

/** rct2: 0x008A7224 */
static void stand_up_rc_track_left_bank_to_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
        sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25282, 0, 6, 32, 20, 3, height);
        break;
    case 1:
        sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25285, 0, 6, 32, 20, 3, height);
        break;
    case 2:
        sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25279, 0, 6, 32, 20, 3, height);
        break;
    case 3:
        sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25284, 0, 6, 32, 20, 3, height);
        break;
    }
    if (track_paint_util_should_paint_supports(gPaintSession.MapPosition)) {
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
    }
    paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
    paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(height + 32, 0x20);
}

/** rct2: 0x008A7234 */
static void stand_up_rc_track_right_bank_to_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
        sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25280, 0, 6, 32, 20, 3, height);
        break;
    case 1:
        sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25283, 0, 6, 32, 20, 3, height);
        break;
    case 2:
        sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25281, 0, 6, 32, 20, 3, height);
        break;
    case 3:
        sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25286, 0, 6, 32, 20, 3, height);
        break;
    }
    if (track_paint_util_should_paint_supports(gPaintSession.MapPosition)) {
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
    }
    paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
    paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(height + 32, 0x20);
}

/** rct2: 0x008A7244 */
static void stand_up_rc_track_banked_left_quarter_turn_5(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25464, 0, 0, 32, 20, 3, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25475, 0, 0, 32, 1, 26, height, 0, 27, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25469, 0, 0, 32, 1, 26, height, 0, 27, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25474, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25459, 0, 0, 32, 20, 3, height, 0, 6, height);
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
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25463, 0, 0, 32, 16, 3, height, 0, 0, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25468, 0, 0, 32, 16, 1, height, 0, 0, height + 27);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25473, 0, 0, 32, 16, 3, height, 0, 16, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25458, 0, 0, 32, 16, 3, height, 0, 16, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25462, 0, 0, 16, 16, 3, height, 0, 16, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25467, 0, 0, 16, 16, 1, height, 16, 16, height + 27);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25472, 0, 0, 16, 16, 3, height, 16, 0, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25457, 0, 0, 16, 16, 3, height, 0, 0, height);
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
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25461, 0, 0, 16, 32, 3, height, 16, 0, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25466, 0, 0, 16, 32, 1, height, 0, 0, height + 27);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25471, 0, 0, 16, 32, 3, height, 0, 0, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25456, 0, 0, 16, 32, 3, height, 16, 0, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 6:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25460, 0, 0, 20, 32, 3, height, 6, 0, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25465, 0, 0, 1, 32, 26, height, 27, 0, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25470, 0, 0, 20, 32, 3, height, 6, 0, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25476, 0, 0, 1, 32, 26, height, 27, 0, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25455, 0, 0, 20, 32, 3, height, 6, 0, height);
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

/** rct2: 0x008A7254 */
static void stand_up_rc_track_banked_right_quarter_turn_5(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    stand_up_rc_track_banked_left_quarter_turn_5(rideIndex, trackSequence, (direction - 1) & 3, height, mapElement);
}

/** rct2: 0x008A7264 */
static void stand_up_rc_track_left_bank_to_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
        sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25315, 0, 6, 32, 20, 3, height);
        break;
    case 1:
        sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25316, 0, 6, 32, 20, 3, height);
        break;
    case 2:
        sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25317, 0, 6, 32, 20, 3, height);
        break;
    case 3:
        sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25318, 0, 6, 32, 20, 3, height);
        break;
    }
    if (track_paint_util_should_paint_supports(gPaintSession.MapPosition)) {
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

/** rct2: 0x008A7274 */
static void stand_up_rc_track_right_bank_to_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
        sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25319, 0, 6, 32, 20, 3, height);
        break;
    case 1:
        sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25320, 0, 6, 32, 20, 3, height);
        break;
    case 2:
        sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25321, 0, 6, 32, 20, 3, height);
        break;
    case 3:
        sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25322, 0, 6, 32, 20, 3, height);
        break;
    }
    if (track_paint_util_should_paint_supports(gPaintSession.MapPosition)) {
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

/** rct2: 0x008A7284 */
static void stand_up_rc_track_25_deg_up_to_left_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
        sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25311, 0, 6, 32, 20, 3, height);
        break;
    case 1:
        sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25312, 0, 6, 32, 20, 3, height);
        break;
    case 2:
        sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25313, 0, 6, 32, 20, 3, height);
        break;
    case 3:
        sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25314, 0, 6, 32, 20, 3, height);
        break;
    }
    if (track_paint_util_should_paint_supports(gPaintSession.MapPosition)) {
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

/** rct2: 0x008A7294 */
static void stand_up_rc_track_25_deg_up_to_right_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
        sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25307, 0, 6, 32, 20, 3, height);
        break;
    case 1:
        sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25308, 0, 6, 32, 20, 3, height);
        break;
    case 2:
        sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25309, 0, 6, 32, 20, 3, height);
        break;
    case 3:
        sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25310, 0, 6, 32, 20, 3, height);
        break;
    }
    if (track_paint_util_should_paint_supports(gPaintSession.MapPosition)) {
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

/** rct2: 0x008A72A4 */
static void stand_up_rc_track_left_bank_to_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    stand_up_rc_track_25_deg_up_to_right_bank(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008A72B4 */
static void stand_up_rc_track_right_bank_to_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    stand_up_rc_track_25_deg_up_to_left_bank(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008A72C4 */
static void stand_up_rc_track_25_deg_down_to_left_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    stand_up_rc_track_right_bank_to_25_deg_up(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008A72D4 */
static void stand_up_rc_track_25_deg_down_to_right_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    stand_up_rc_track_left_bank_to_25_deg_up(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008A72E4 */
static void stand_up_rc_track_left_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
        sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25323, 0, 6, 32, 20, 3, height);
        break;
    case 1:
        sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25324, 0, 6, 32, 20, 3, height);
        break;
    case 2:
        sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25325, 0, 6, 32, 20, 3, height);
        break;
    case 3:
        sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25326, 0, 6, 32, 20, 3, height);
        break;
    }
    if (track_paint_util_should_paint_supports(gPaintSession.MapPosition)) {
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
    }
    paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
    paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(height + 32, 0x20);
}

/** rct2: 0x008A72F4 */
static void stand_up_rc_track_right_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    stand_up_rc_track_left_bank(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008A7304 */
static void stand_up_rc_track_left_quarter_turn_5_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 0:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25433, 0, 6, 32, 20, 3, height);
                break;
            case 1:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25438, 0, 6, 32, 20, 3, height);
                break;
            case 2:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25443, 0, 6, 32, 20, 3, height);
                break;
            case 3:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25448, 0, 6, 32, 20, 3, height);
                break;
            }
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
        } else {
            switch (direction) {
            case 0:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25347, 0, 6, 32, 20, 3, height);
                break;
            case 1:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25352, 0, 6, 32, 20, 3, height);
                break;
            case 2:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25357, 0, 6, 32, 20, 3, height);
                break;
            case 3:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25362, 0, 6, 32, 20, 3, height);
                break;
            }
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
        }
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
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 0:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25434, 0, 0, 32, 16, 3, height);
                break;
            case 1:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25439, 0, 0, 32, 16, 3, height);
                break;
            case 2:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25444, 0, 16, 32, 16, 3, height);
                break;
            case 3:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25449, 0, 16, 32, 16, 3, height);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25348, 0, 0, 32, 16, 3, height);
                break;
            case 1:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25353, 0, 0, 32, 16, 3, height);
                break;
            case 2:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25358, 0, 16, 32, 16, 3, height);
                break;
            case 3:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25363, 0, 16, 32, 16, 3, height);
                break;
            }
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 3:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 0:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25435, 0, 16, 16, 16, 3, height);
                break;
            case 1:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25440, 16, 16, 16, 16, 3, height);
                break;
            case 2:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25445, 16, 0, 16, 16, 3, height);
                break;
            case 3:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25450, 0, 0, 16, 16, 3, height);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25349, 0, 16, 16, 16, 3, height);
                break;
            case 1:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25354, 16, 16, 16, 16, 3, height);
                break;
            case 2:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25359, 16, 0, 16, 16, 3, height);
                break;
            case 3:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25364, 0, 0, 16, 16, 3, height);
                break;
            }
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 64, 0x20);
        break;
    case 4:
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 5:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 0:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25436, 16, 0, 16, 32, 3, height);
                break;
            case 1:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25441, 0, 0, 16, 32, 3, height);
                break;
            case 2:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25446, 0, 0, 16, 32, 3, height);
                break;
            case 3:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25451, 16, 0, 16, 32, 3, height);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25350, 16, 0, 16, 32, 3, height);
                break;
            case 1:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25355, 0, 0, 16, 32, 3, height);
                break;
            case 2:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25360, 0, 0, 16, 32, 3, height);
                break;
            case 3:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25365, 16, 0, 16, 32, 3, height);
                break;
            }
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 6:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 0:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25437, 6, 0, 20, 32, 3, height);
                break;
            case 1:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25442, 6, 0, 20, 32, 3, height);
                break;
            case 2:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25447, 6, 0, 20, 32, 3, height);
                break;
            case 3:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25452, 6, 0, 20, 32, 3, height);
                break;
            }
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
        } else {
            switch (direction) {
            case 0:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25351, 6, 0, 20, 32, 3, height);
                break;
            case 1:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25356, 6, 0, 20, 32, 3, height);
                break;
            case 2:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25361, 6, 0, 20, 32, 3, height);
                break;
            case 3:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25366, 6, 0, 20, 32, 3, height);
                break;
            }
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
        }
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

/** rct2: 0x008A7314 */
static void stand_up_rc_track_right_quarter_turn_5_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 0:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25413, 0, 6, 32, 20, 3, height);
                break;
            case 1:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25418, 0, 6, 32, 20, 3, height);
                break;
            case 2:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25423, 0, 6, 32, 20, 3, height);
                break;
            case 3:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25428, 0, 6, 32, 20, 3, height);
                break;
            }
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
        } else {
            switch (direction) {
            case 0:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25327, 0, 6, 32, 20, 3, height);
                break;
            case 1:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25332, 0, 6, 32, 20, 3, height);
                break;
            case 2:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25337, 0, 6, 32, 20, 3, height);
                break;
            case 3:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25342, 0, 6, 32, 20, 3, height);
                break;
            }
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
        }
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
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 0:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25414, 0, 16, 32, 16, 3, height);
                break;
            case 1:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25419, 0, 16, 32, 16, 3, height);
                break;
            case 2:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25424, 0, 0, 32, 16, 3, height);
                break;
            case 3:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25429, 0, 0, 32, 16, 3, height);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25328, 0, 16, 32, 16, 3, height);
                break;
            case 1:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25333, 0, 16, 32, 16, 3, height);
                break;
            case 2:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25338, 0, 0, 32, 16, 3, height);
                break;
            case 3:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25343, 0, 0, 32, 16, 3, height);
                break;
            }
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 3:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 0:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25415, 0, 0, 16, 16, 3, height);
                break;
            case 1:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25420, 16, 0, 16, 16, 3, height);
                break;
            case 2:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25425, 16, 16, 16, 16, 3, height);
                break;
            case 3:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25430, 0, 16, 16, 16, 3, height);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25329, 0, 0, 16, 16, 3, height);
                break;
            case 1:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25334, 16, 0, 16, 16, 3, height);
                break;
            case 2:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25339, 16, 16, 16, 16, 3, height);
                break;
            case 3:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25344, 0, 16, 16, 16, 3, height);
                break;
            }
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 64, 0x20);
        break;
    case 4:
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 5:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 0:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25416, 16, 0, 16, 32, 3, height);
                break;
            case 1:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25421, 0, 0, 16, 32, 3, height);
                break;
            case 2:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25426, 0, 0, 16, 32, 3, height);
                break;
            case 3:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25431, 16, 0, 16, 32, 3, height);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25330, 16, 0, 16, 32, 3, height);
                break;
            case 1:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25335, 0, 0, 16, 32, 3, height);
                break;
            case 2:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25340, 0, 0, 16, 32, 3, height);
                break;
            case 3:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25345, 16, 0, 16, 32, 3, height);
                break;
            }
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 6:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 0:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25417, 6, 0, 20, 32, 3, height);
                break;
            case 1:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25422, 6, 0, 20, 32, 3, height);
                break;
            case 2:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25427, 6, 0, 20, 32, 3, height);
                break;
            case 3:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25432, 6, 0, 20, 32, 3, height);
                break;
            }
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
        } else {
            switch (direction) {
            case 0:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25331, 6, 0, 20, 32, 3, height);
                break;
            case 1:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25336, 6, 0, 20, 32, 3, height);
                break;
            case 2:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25341, 6, 0, 20, 32, 3, height);
                break;
            case 3:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25346, 6, 0, 20, 32, 3, height);
                break;
            }
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
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

/** rct2: 0x008A7324 */
static void stand_up_rc_track_left_quarter_turn_5_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    stand_up_rc_track_right_quarter_turn_5_25_deg_up(rideIndex, trackSequence, (direction + 1) & 3, height, mapElement);
}

/** rct2: 0x008A7334 */
static void stand_up_rc_track_right_quarter_turn_5_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    stand_up_rc_track_left_quarter_turn_5_25_deg_up(rideIndex, trackSequence, (direction - 1) & 3, height, mapElement);
}

/** rct2: 0x008A7344 */
static void stand_up_rc_track_s_bend_left(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25371, 0, 6, 32, 20, 3, height);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25378, 0, 6, 32, 20, 3, height);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25374, 0, 6, 32, 20, 3, height);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25375, 0, 6, 32, 20, 3, height);
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
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25372, 0, 0, 32, 26, 3, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 5, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25377, 0, 0, 32, 26, 3, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 6, 1, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25373, 0, 6, 32, 26, 3, height);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25376, 0, 6, 32, 26, 3, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25373, 0, 6, 32, 26, 3, height);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25376, 0, 6, 32, 26, 3, height);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25372, 0, 0, 32, 26, 3, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 5, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25377, 0, 0, 32, 26, 3, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 6, 1, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25374, 0, 6, 32, 20, 3, height);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25375, 0, 6, 32, 20, 3, height);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25371, 0, 6, 32, 20, 3, height);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25378, 0, 6, 32, 20, 3, height);
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

/** rct2: 0x008A7354 */
static void stand_up_rc_track_s_bend_right(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25367, 0, 6, 32, 20, 3, height);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25382, 0, 6, 32, 20, 3, height);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25370, 0, 6, 32, 20, 3, height);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25379, 0, 6, 32, 20, 3, height);
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
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25368, 0, 6, 32, 26, 3, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 8, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25381, 0, 6, 32, 26, 3, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 7, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25369, 0, 0, 32, 26, 3, height);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25380, 0, 0, 32, 26, 3, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25369, 0, 0, 32, 26, 3, height);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25380, 0, 0, 32, 26, 3, height);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25368, 0, 6, 32, 26, 3, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 8, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25381, 0, 6, 32, 26, 3, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 7, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25370, 0, 6, 32, 20, 3, height);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25379, 0, 6, 32, 20, 3, height);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25367, 0, 6, 32, 20, 3, height);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25382, 0, 6, 32, 20, 3, height);
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

/** rct2: 0x008A7364 */
static void stand_up_rc_track_left_vertical_loop(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25477, 0, 6, 32, 20, 3, height);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25485, 0, 6, 32, 20, 3, height);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25484, 0, 6, 32, 20, 3, height);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25492, 0, 6, 32, 20, 7, height);
            break;
        }
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_1);
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 56, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25478, 0, 0, 32, 26, 3, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 20, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25486, 0, 14, 32, 2, 63, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 15, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25483, 0, 6, 32, 26, 3, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 16, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25491, 0, 6, 32, 26, 3, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 16, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25479, 16, 0, 5, 16, 119, height);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25487, 12, 0, 3, 16, 119, height);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25482, 10, 16, 4, 16, 119, height);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25490, 16, 16, 4, 16, 119, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 168, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25480, 0, 0, 32, 16, 3, height + 32);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25488, 0, 0, 32, 16, 3, height + 32);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25481, 0, 16, 32, 16, 3, height + 32);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25489, 0, 16, 32, 16, 3, height + 32);
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
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25481, 0, 16, 32, 16, 3, height + 32);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25489, 0, 16, 32, 16, 3, height + 32);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25480, 0, 0, 32, 16, 3, height + 32);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25488, 0, 0, 32, 16, 3, height + 32);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 7:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25482, 10, 16, 4, 16, 119, height);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25490, 16, 16, 4, 16, 119, height);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25479, 16, 0, 5, 16, 119, height);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25487, 12, 0, 3, 16, 119, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 168, 0x20);
        break;
    case 8:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25483, 0, 6, 32, 26, 3, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 16, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25491, 0, 6, 32, 26, 3, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 16, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25478, 0, 0, 32, 26, 3, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 20, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25486, 0, 14, 32, 2, 63, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 15, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 9:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25484, 0, 6, 32, 20, 3, height);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25492, 0, 6, 32, 20, 7, height);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25477, 0, 6, 32, 20, 3, height);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25485, 0, 6, 32, 20, 3, height);
            break;
        }
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
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


/** rct2: 0x008A7374 */
static void stand_up_rc_track_right_vertical_loop(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25508, 0, 6, 32, 20, 7, height);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25500, 0, 6, 32, 20, 3, height);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25501, 0, 6, 32, 20, 3, height);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25493, 0, 6, 32, 20, 3, height);
            break;
        }
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_1);
        }
        paint_util_set_general_support_height(height + 56, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25507, 0, 6, 32, 26, 3, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 16, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25499, 0, 6, 32, 26, 3, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 16, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25502, 0, 14, 32, 2, 63, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 15, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25494, 0, 0, 32, 26, 3, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 20, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25506, 16, 16, 4, 16, 119, height);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25498, 10, 16, 4, 16, 119, height);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25503, 12, 0, 3, 16, 119, height);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25495, 16, 0, 5, 16, 119, height);
            break;
        }
        paint_util_set_general_support_height(height + 168, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25505, 0, 16, 32, 16, 3, height + 32);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25497, 0, 16, 32, 16, 3, height + 32);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25504, 0, 0, 32, 16, 3, height + 32);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25496, 0, 0, 32, 16, 3, height + 32);
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
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25504, 0, 0, 32, 16, 3, height + 32);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25496, 0, 0, 32, 16, 3, height + 32);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25505, 0, 16, 32, 16, 3, height + 32);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25497, 0, 16, 32, 16, 3, height + 32);
            break;
        }
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 7:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25503, 12, 0, 3, 16, 119, height);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25495, 16, 0, 5, 16, 119, height);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25506, 16, 16, 4, 16, 119, height);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25498, 10, 16, 4, 16, 119, height);
            break;
        }
        paint_util_set_general_support_height(height + 168, 0x20);
        break;
    case 8:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25502, 0, 14, 32, 2, 63, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 15, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25494, 0, 0, 32, 26, 3, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 20, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25507, 0, 6, 32, 26, 3, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 16, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25499, 0, 6, 32, 26, 3, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 16, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 9:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25501, 0, 6, 32, 20, 3, height);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25493, 0, 6, 32, 20, 3, height);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25508, 0, 6, 32, 20, 7, height);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25500, 0, 6, 32, 20, 3, height);
            break;
        }
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
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

/** rct2: 0x008A73B4 */
static void stand_up_rc_track_left_quarter_turn_3(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25514, 0, 6, 32, 20, 3, height);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25517, 0, 6, 32, 20, 3, height);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25520, 0, 6, 32, 20, 3, height);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25511, 0, 6, 32, 20, 3, height);
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
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25513, 16, 0, 16, 16, 3, height);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25516, 0, 0, 16, 16, 3, height);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25519, 0, 16, 16, 16, 3, height);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25510, 16, 16, 16, 16, 3, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25512, 6, 0, 20, 32, 3, height);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25515, 6, 0, 20, 32, 3, height);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25518, 6, 0, 20, 32, 3, height);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25509, 6, 0, 20, 32, 3, height);
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

/** rct2: 0x008A73C4 */
static void stand_up_rc_track_right_quarter_turn_3(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    stand_up_rc_track_left_quarter_turn_3(rideIndex, trackSequence, (direction - 1) & 3, height, mapElement);
}

/** rct2: 0x008A73D4 */
static void stand_up_rc_track_left_quarter_turn_3_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25526, 0, 0, 32, 20, 3, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25533, 0, 0, 32, 1, 26, height, 0, 27, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25529, 0, 0, 32, 1, 26, height, 0, 27, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25532, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25523, 0, 0, 32, 20, 3, height, 0, 6, height);
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
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25525, 0, 0, 16, 16, 3, height, 16, 0, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25528, 0, 0, 16, 16, 1, height, 0, 0, height + 27);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25531, 0, 0, 16, 16, 3, height, 0, 16, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25522, 0, 0, 16, 16, 3, height, 16, 16, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25524, 0, 0, 20, 32, 3, height, 6, 0, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25527, 0, 0, 1, 32, 26, height, 27, 0, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25530, 0, 0, 20, 32, 3, height, 6, 0, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25534, 0, 0, 1, 32, 26, height, 27, 0, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25521, 0, 0, 20, 32, 3, height, 6, 0, height);
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

/** rct2: 0x008A73E4 */
static void stand_up_rc_track_right_quarter_turn_3_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    stand_up_rc_track_left_quarter_turn_3_bank(rideIndex, trackSequence, (direction - 1) & 3, height, mapElement);
}

/** rct2: 0x008A73F4 */
static void stand_up_rc_track_left_quarter_turn_3_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 0:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25562, 0, 6, 32, 20, 3, height);
                break;
            case 1:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25564, 0, 6, 32, 20, 3, height);
                break;
            case 2:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25566, 0, 6, 32, 20, 3, height);
                break;
            case 3:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25560, 0, 6, 32, 20, 3, height);
                break;
            }
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
        } else {
            switch (direction) {
            case 0:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25546, 0, 6, 32, 20, 3, height);
                break;
            case 1:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25548, 0, 6, 32, 20, 3, height);
                break;
            case 2:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25550, 0, 6, 32, 20, 3, height);
                break;
            case 3:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25544, 0, 6, 32, 20, 3, height);
                break;
            }
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
        }
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
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 0:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25561, 6, 0, 20, 32, 3, height);
                break;
            case 1:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25563, 6, 0, 20, 32, 3, height);
                break;
            case 2:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25565, 6, 0, 20, 32, 3, height);
                break;
            case 3:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25559, 6, 0, 20, 32, 3, height);
                break;
            }
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
        } else {
            switch (direction) {
            case 0:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25545, 6, 0, 20, 32, 3, height);
                break;
            case 1:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25547, 6, 0, 20, 32, 3, height);
                break;
            case 2:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25549, 6, 0, 20, 32, 3, height);
                break;
            case 3:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25543, 6, 0, 20, 32, 3, height);
                break;
            }
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
        }
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

/** rct2: 0x008A7404 */
static void stand_up_rc_track_right_quarter_turn_3_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 0:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25551, 0, 6, 32, 20, 3, height);
                break;
            case 1:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25553, 0, 6, 32, 20, 3, height);
                break;
            case 2:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25555, 0, 6, 32, 20, 3, height);
                break;
            case 3:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25557, 0, 6, 32, 20, 3, height);
                break;
            }
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
        } else {
            switch (direction) {
            case 0:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25535, 0, 6, 32, 20, 3, height);
                break;
            case 1:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25537, 0, 6, 32, 20, 3, height);
                break;
            case 2:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25539, 0, 6, 32, 20, 3, height);
                break;
            case 3:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25541, 0, 6, 32, 20, 3, height);
                break;
            }
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
        }
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
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 0:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25552, 6, 0, 20, 32, 3, height);
                metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25554, 6, 0, 20, 32, 3, height);
                metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 2:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25556, 6, 0, 20, 32, 3, height);
                metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 10, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 3:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25558, 6, 0, 20, 32, 3, height);
                metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25536, 6, 0, 20, 32, 3, height);
                metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25538, 6, 0, 20, 32, 3, height);
                metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 2:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25540, 6, 0, 20, 32, 3, height);
                metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 10, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 3:
                sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25542, 6, 0, 20, 32, 3, height);
                metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            }
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

/** rct2: 0x008A7414 */
static void stand_up_rc_track_left_quarter_turn_3_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    stand_up_rc_track_right_quarter_turn_3_25_deg_up(rideIndex, trackSequence, (direction + 1) & 3, height, mapElement);
}

/** rct2: 0x008A7424 */
static void stand_up_rc_track_right_quarter_turn_3_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    stand_up_rc_track_left_quarter_turn_3_25_deg_up(rideIndex, trackSequence, (direction - 1) & 3, height, mapElement);
}

/** rct2: 0x008A7434 */
static void stand_up_rc_track_half_loop_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25575, 0, 6, 32, 20, 3, height);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25583, 0, 6, 32, 20, 3, height);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25582, 0, 6, 32, 20, 9, height);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25590, 0, 6, 32, 20, 7, height);
            break;
        }
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_1);
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 56, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25576, 0, 0, 32, 20, 3, height, 0, 6, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 20, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25584, 0, 14, 32, 2, 63, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 15, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25581, 0, 6, 32, 2, 3, height, 0, 20, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 16, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25589, 0, 6, 32, 20, 3, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 16, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25577, 16, 0, 2, 16, 119, height);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25585, 12, 0, 3, 16, 119, height);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25580, 10, 16, 4, 12, 119, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25588, 16, 16, 2, 16, 119, height, 15, 6, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 168, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25578, 0, 0, 32, 16, 3, height + 32);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25586, 0, 0, 32, 16, 3, height + 32);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25579, 0, 16, 32, 12, 3, height + 32);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25587, 0, 16, 32, 12, 3, height + 32);
            break;
        }
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    }
}

/** rct2: 0x008A7444 */
static void stand_up_rc_track_half_loop_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    stand_up_rc_track_half_loop_up(rideIndex, 3 - trackSequence, direction, height, mapElement);
}

/** rct2: 0x008A7454 */
static void stand_up_rc_track_left_corkscrew_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25591, 0, 0, 32, 20, 3, height, 0, 6, height + 4);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25594, 0, 0, 32, 20, 3, height, 0, 6, height + 4);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25597, 0, 0, 32, 20, 3, height, 0, 6, height + 4);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25600, 0, 0, 32, 20, 3, height, 0, 6, height + 4);
            break;
        }

        track_paint_util_left_corkscrew_up_supports(direction, height);

        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
        }
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25592, 0, 0, 20, 20, 3, height, 6, 6, height + 10);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25595, 0, 0, 20, 20, 3, height, 6, 6, height + 10);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25598, 0, 0, 20, 20, 3, height, 6, 6, height + 10);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25601, 0, 0, 20, 20, 3, height, 6, 6, height + 10);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25593, 0, 0, 20, 32, 3, height, 6, 0, height + 24);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25596, 0, 0, 20, 32, 3, height, 6, 0, height + 24);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25599, 0, 0, 20, 32, 3, height, 6, 0, height + 24);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25602, 0, 0, 20, 32, 3, height, 6, 0, height + 24);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 0, height + 35, gTrackColours[SCHEME_SUPPORTS]);

        switch (direction) {
        case 2:
            paint_util_push_tunnel_right(height + 8, TUNNEL_0);
            break;
        case 3:
            paint_util_push_tunnel_left(height + 8, TUNNEL_0);
            break;
        }
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    }
}

/** rct2: 0x008A7464 */
static void stand_up_rc_track_right_corkscrew_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25603, 0, 0, 32, 20, 3, height, 0, 6, height + 4);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25606, 0, 0, 32, 20, 3, height, 0, 6, height + 4);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25609, 0, 0, 32, 20, 3, height, 0, 6, height + 4);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25612, 0, 0, 32, 20, 3, height, 0, 6, height + 4);
            break;
        }
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25604, 0, 0, 20, 20, 3, height, 6, 6, height + 10);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25607, 0, 0, 20, 20, 3, height, 6, 6, height + 10);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25610, 0, 0, 20, 20, 3, height, 6, 6, height + 10);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25613, 0, 0, 20, 20, 3, height, 6, 6, height + 10);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25605, 0, 0, 20, 32, 3, height, 6, 0, height + 24);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25608, 0, 0, 20, 32, 3, height, 6, 0, height + 24);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25611, 0, 0, 20, 32, 3, height, 6, 0, height + 24);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25614, 0, 0, 20, 32, 3, height, 6, 0, height + 24);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 0, height + 35, gTrackColours[SCHEME_SUPPORTS]);

        switch (direction) {
        case 0:
            paint_util_push_tunnel_right(height + 8, TUNNEL_0);
            break;
        case 1:
            paint_util_push_tunnel_left(height + 8, TUNNEL_0);
            break;
        }
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    }
}

/** rct2: 0x008A7474 */
static void stand_up_rc_track_left_corkscrew_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    stand_up_rc_track_right_corkscrew_up(rideIndex, 2 - trackSequence, (direction + 1) & 3, height, mapElement);
}

/** rct2: 0x008A7484 */
static void stand_up_rc_track_right_corkscrew_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    stand_up_rc_track_left_corkscrew_up(rideIndex, 2 - trackSequence, (direction - 1) & 3, height, mapElement);
}

/** rct2: 0x008A7734 */
static void stand_up_rc_track_left_half_banked_helix_up_small(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25844, 0, 0, 32, 20, 3, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25851, 0, 0, 32, 1, 26, height, 0, 27, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25847, 0, 0, 32, 1, 26, height, 0, 27, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25850, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25841, 0, 0, 32, 20, 3, height, 0, 6, height);
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
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25843, 0, 0, 16, 16, 3, height, 16, 0, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25846, 0, 0, 16, 16, 1, height, 0, 0, height + 27);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25849, 0, 0, 16, 16, 3, height, 0, 16, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25840, 0, 0, 16, 16, 3, height, 16, 16, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25842, 0, 0, 20, 32, 3, height, 6, 0, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25845, 0, 0, 1, 32, 26, height, 27, 0, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25848, 0, 0, 20, 32, 3, height, 6, 0, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25852, 0, 0, 1, 32, 26, height, 27, 0, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25839, 0, 0, 20, 32, 3, height, 6, 0, height);
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
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25841, 0, 0, 20, 32, 3, height, 6, 0, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25844, 0, 0, 20, 32, 3, height, 6, 0, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25851, 0, 0, 1, 32, 26, height, 27, 0, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25847, 0, 0, 1, 32, 26, height, 27, 0, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25850, 0, 0, 20, 32, 3, height, 6, 0, height);
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
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25840, 0, 0, 16, 16, 3, height, 16, 16, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25843, 0, 0, 16, 16, 3, height, 0, 16, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25846, 0, 0, 16, 16, 1, height, 0, 0, height + 27);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25849, 0, 0, 16, 16, 3, height, 16, 0, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 7:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25839, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25842, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25845, 0, 0, 32, 1, 26, height, 0, 27, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25848, 0, 0, 32, 20, 3, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25852, 0, 0, 32, 1, 26, height, 0, 27, height);
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

/** rct2: 0x008A7744 */
static void stand_up_rc_track_right_half_banked_helix_up_small(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25825, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25828, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25831, 0, 0, 32, 1, 26, height, 0, 27, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25834, 0, 0, 32, 20, 3, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25838, 0, 0, 32, 1, 26, height, 0, 27, height);
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
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25826, 0, 0, 16, 16, 3, height, 16, 16, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25829, 0, 0, 16, 16, 3, height, 0, 16, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25832, 0, 0, 16, 16, 1, height, 0, 0, height + 27);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25835, 0, 0, 16, 16, 3, height, 16, 0, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25827, 0, 0, 20, 32, 3, height, 6, 0, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25830, 0, 0, 20, 32, 3, height, 6, 0, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25837, 0, 0, 1, 32, 26, height, 27, 0, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25833, 0, 0, 1, 32, 26, height, 27, 0, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25836, 0, 0, 20, 32, 3, height, 6, 0, height);
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
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25828, 0, 0, 20, 32, 3, height, 6, 0, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25831, 0, 0, 1, 32, 26, height, 27, 0, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25834, 0, 0, 20, 32, 3, height, 6, 0, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25838, 0, 0, 1, 32, 26, height, 27, 0, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25825, 0, 0, 20, 32, 3, height, 6, 0, height);
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
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25829, 0, 0, 16, 16, 3, height, 16, 0, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25832, 0, 0, 16, 16, 1, height, 0, 0, height + 27);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25835, 0, 0, 16, 16, 3, height, 0, 16, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25826, 0, 0, 16, 16, 3, height, 16, 16, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 7:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25830, 0, 0, 32, 20, 3, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25837, 0, 0, 32, 1, 26, height, 0, 27, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25833, 0, 0, 32, 1, 26, height, 0, 27, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25836, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25827, 0, 0, 32, 20, 3, height, 0, 6, height);
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

/** rct2: 0x008A7754 */
static void stand_up_rc_track_left_half_banked_helix_down_small(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    if (trackSequence >= 4) {
        trackSequence -= 4;
        direction = (direction - 1) & 3;
    }
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    stand_up_rc_track_right_half_banked_helix_up_small(rideIndex, trackSequence, (direction + 1) & 3, height, mapElement);
}

/** rct2: 0x008A7764 */
static void stand_up_rc_track_right_half_banked_helix_down_small(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    if (trackSequence >= 4) {
        trackSequence -= 4;
        direction = (direction + 1) & 3;
    }
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    stand_up_rc_track_left_half_banked_helix_up_small(rideIndex, trackSequence, (direction - 1) & 3, height, mapElement);
}

/** rct2: 0x008A76F4 */
static void stand_up_rc_track_left_half_banked_helix_up_large(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25812, 0, 0, 32, 20, 3, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25823, 0, 0, 32, 1, 26, height, 0, 27, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25817, 0, 0, 32, 1, 26, height, 0, 27, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25822, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25807, 0, 0, 32, 20, 3, height, 0, 6, height);
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
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25811, 0, 0, 32, 16, 3, height, 0, 0, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25816, 0, 0, 32, 16, 1, height, 0, 0, height + 27);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25821, 0, 0, 32, 16, 3, height, 0, 16, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25806, 0, 0, 32, 16, 3, height, 0, 16, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25810, 0, 0, 16, 16, 3, height, 0, 16, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25815, 0, 0, 16, 16, 1, height, 16, 16, height + 27);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25820, 0, 0, 16, 16, 3, height, 16, 0, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25805, 0, 0, 16, 16, 3, height, 0, 0, height);
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
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25809, 0, 0, 16, 32, 3, height, 16, 0, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25814, 0, 0, 16, 32, 1, height, 0, 0, height + 27);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25819, 0, 0, 16, 32, 3, height, 0, 0, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25804, 0, 0, 16, 32, 3, height, 16, 0, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 6:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25808, 0, 0, 20, 32, 3, height, 6, 0, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25813, 0, 0, 1, 32, 26, height, 27, 0, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25818, 0, 0, 20, 32, 3, height, 6, 0, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25824, 0, 0, 1, 32, 26, height, 27, 0, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25803, 0, 0, 20, 32, 3, height, 6, 0, height);
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
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25807, 0, 0, 20, 32, 3, height, 6, 0, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25812, 0, 0, 20, 32, 3, height, 6, 0, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25823, 0, 0, 1, 32, 26, height, 27, 0, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25817, 0, 0, 1, 32, 26, height, 27, 0, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25822, 0, 0, 20, 32, 3, height, 6, 0, height);
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
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25806, 0, 0, 16, 32, 3, height, 16, 0, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25811, 0, 0, 16, 32, 3, height, 0, 0, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25816, 0, 0, 16, 32, 1, height, 0, 0, height + 27);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25821, 0, 0, 16, 32, 3, height, 16, 0, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 10:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25805, 0, 0, 16, 16, 3, height, 0, 0, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25810, 0, 0, 16, 16, 3, height, 16, 0, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25815, 0, 0, 16, 16, 1, height, 16, 16, height + 27);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25820, 0, 0, 16, 16, 3, height, 0, 16, height);
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
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25804, 0, 0, 32, 16, 3, height, 0, 16, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25809, 0, 0, 32, 16, 3, height, 0, 16, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25814, 0, 0, 32, 16, 1, height, 0, 0, height + 27);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25819, 0, 0, 32, 16, 3, height, 0, 0, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 13:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25803, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25808, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25813, 0, 0, 32, 1, 26, height, 0, 27, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25818, 0, 0, 32, 20, 3, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25824, 0, 0, 32, 1, 26, height, 0, 27, height);
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

/** rct2: 0x008A7704 */
static void stand_up_rc_track_right_half_banked_helix_up_large(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25781, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25786, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25791, 0, 0, 32, 1, 26, height, 0, 27, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25796, 0, 0, 32, 20, 3, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25802, 0, 0, 32, 1, 26, height, 0, 27, height);
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
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25782, 0, 0, 32, 16, 3, height, 0, 16, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25787, 0, 0, 32, 16, 3, height, 0, 16, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25792, 0, 0, 32, 16, 1, height, 0, 0, height + 27);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25797, 0, 0, 32, 16, 3, height, 0, 0, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25783, 0, 0, 16, 16, 3, height, 0, 0, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25788, 0, 0, 16, 16, 3, height, 16, 0, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25793, 0, 0, 16, 16, 1, height, 16, 16, height + 27);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25798, 0, 0, 16, 16, 3, height, 0, 16, height);
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
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25784, 0, 0, 16, 32, 3, height, 16, 0, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25789, 0, 0, 16, 32, 3, height, 0, 0, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25794, 0, 0, 16, 32, 1, height, 0, 0, height + 27);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25799, 0, 0, 16, 32, 3, height, 16, 0, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 6:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25785, 0, 0, 20, 32, 3, height, 6, 0, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25790, 0, 0, 20, 32, 3, height, 6, 0, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25801, 0, 0, 1, 32, 26, height, 27, 0, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25795, 0, 0, 1, 32, 26, height, 27, 0, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25800, 0, 0, 20, 32, 3, height, 6, 0, height);
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
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25786, 0, 0, 20, 32, 3, height, 6, 0, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25791, 0, 0, 1, 32, 26, height, 27, 0, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25796, 0, 0, 20, 32, 3, height, 6, 0, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25802, 0, 0, 1, 32, 26, height, 27, 0, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25781, 0, 0, 20, 32, 3, height, 6, 0, height);
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
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25787, 0, 0, 16, 32, 3, height, 16, 0, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25792, 0, 0, 16, 32, 1, height, 0, 0, height + 27);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25797, 0, 0, 16, 32, 3, height, 0, 0, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25782, 0, 0, 16, 32, 3, height, 16, 0, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 10:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25788, 0, 0, 16, 16, 3, height, 0, 16, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25793, 0, 0, 16, 16, 1, height, 16, 16, height + 27);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25798, 0, 0, 16, 16, 3, height, 16, 0, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25783, 0, 0, 16, 16, 3, height, 0, 0, height);
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
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25789, 0, 0, 32, 16, 3, height, 0, 0, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25794, 0, 0, 32, 16, 1, height, 0, 0, height + 27);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25799, 0, 0, 32, 16, 3, height, 0, 16, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25784, 0, 0, 32, 16, 3, height, 0, 16, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 13:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25790, 0, 0, 32, 20, 3, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25801, 0, 0, 32, 1, 26, height, 0, 27, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25795, 0, 0, 32, 1, 26, height, 0, 27, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25800, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25785, 0, 0, 32, 20, 3, height, 0, 6, height);
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

/** rct2: 0x008A7714 */
static void stand_up_rc_track_left_half_banked_helix_down_large(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    if (trackSequence >= 7) {
        trackSequence -= 7;
        direction = (direction - 1) & 3;
    }
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    stand_up_rc_track_right_half_banked_helix_up_large(rideIndex, trackSequence, (direction + 1) & 3, height, mapElement);
}

/** rct2: 0x008A7724 */
static void stand_up_rc_track_right_half_banked_helix_down_large(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    if (trackSequence >= 7) {
        trackSequence -= 7;
        direction = (direction + 1) & 3;
    }
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    stand_up_rc_track_left_half_banked_helix_up_large(rideIndex, trackSequence, (direction - 1) & 3, height, mapElement);
}

/** rct2: 0x008A7494 */
static void stand_up_rc_track_brakes(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
    case 2:
        sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25569, 0, 6, 32, 20, 3, height);
        break;
    case 1:
    case 3:
        sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25570, 0, 6, 32, 20, 3, height);
        break;
    }
    if (track_paint_util_should_paint_supports(gPaintSession.MapPosition)) {
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
    }
    paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
    paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(height + 32, 0x20);
}

/** rct2: 0x008A74A4 */
static void stand_up_rc_track_on_ride_photo(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
        sub_98196C_rotated(direction, IMAGE_TYPE_REMAP | SPR_STATION_BASE_D, 0, 0, 32, 32, 1, height);
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 5, 0, height, gTrackColours[SCHEME_SUPPORTS]);
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 8, 0, height, gTrackColours[SCHEME_SUPPORTS]);
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25229, 0, 6, 32, 20, 1, height, 0, 6, height + 3);
        break;
    case 1:
        sub_98196C_rotated(direction, IMAGE_TYPE_REMAP | SPR_STATION_BASE_D, 0, 0, 32, 32, 1, height);
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 6, 0, height, gTrackColours[SCHEME_SUPPORTS]);
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 7, 0, height, gTrackColours[SCHEME_SUPPORTS]);
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25230, 0, 6, 32, 20, 1, height, 0, 6, height + 3);
        break;
    case 2:
        sub_98196C_rotated(direction, IMAGE_TYPE_REMAP | SPR_STATION_BASE_D, 0, 0, 32, 32, 1, height);
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 5, 0, height, gTrackColours[SCHEME_SUPPORTS]);
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 8, 0, height, gTrackColours[SCHEME_SUPPORTS]);
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25229, 0, 6, 32, 20, 1, height, 0, 6, height + 3);
        break;
    case 3:
        sub_98196C_rotated(direction, IMAGE_TYPE_REMAP | SPR_STATION_BASE_D, 0, 0, 32, 32, 1, height);
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 6, 0, height, gTrackColours[SCHEME_SUPPORTS]);
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 7, 0, height, gTrackColours[SCHEME_SUPPORTS]);
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25230, 0, 6, 32, 20, 1, height, 0, 6, height + 3);
        break;
    }
    track_paint_util_onride_photo_paint(direction, height + 3, mapElement);
    paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 48, 0x20);
}

/** rct2: 0x008A74B4 */
static void stand_up_rc_track_left_eighth_to_diag(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25643, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25647, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25651, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25655, 0, 0, 32, 20, 3, height, 0, 6, height);
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
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25644, 0, 0, 32, 16, 3, height, 0, 0, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25648, 0, 0, 34, 16, 3, height, 0, 0, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25652, 0, 0, 32, 16, 3, height, 0, 16, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25656, 0, 0, 32, 16, 3, height, 0, 16, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25645, 0, 0, 16, 16, 3, height, 0, 16, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25649, 0, 0, 16, 16, 3, height, 16, 16, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25653, 0, 0, 16, 16, 3, height, 16, 0, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25657, 0, 0, 16, 16, 3, height, 0, 0, height);
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
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25646, 0, 0, 16, 16, 3, height, 16, 16, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 3, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25650, 0, 0, 16, 18, 3, height, 0, 16, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 1, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25654, 0, 0, 16, 16, 3, height, 0, 0, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 0, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25658, 0, 0, 16, 16, 3, height, 16, 0, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 2, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    }
}

/** rct2: 0x008A74C4 */
static void stand_up_rc_track_right_eighth_to_diag(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25627, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25631, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25635, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25639, 0, 0, 32, 20, 3, height, 0, 6, height);
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
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25628, 0, 0, 32, 16, 3, height, 0, 16, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25632, 0, 0, 32, 16, 3, height, 0, 16, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25636, 0, 0, 34, 16, 3, height, 0, 0, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25640, 0, 0, 32, 16, 3, height, 0, 0, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25629, 0, 0, 16, 16, 3, height, 0, 0, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25633, 0, 0, 16, 16, 3, height, 16, 0, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25637, 0, 0, 28, 28, 3, height, 4, 4, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25641, 0, 0, 16, 16, 3, height, 0, 16, height);
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
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25630, 0, 0, 16, 16, 3, height, 16, 0, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 1, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25634, 0, 0, 16, 16, 3, height, 0, 0, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 0, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25638, 0, 0, 16, 18, 3, height, 0, 16, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 2, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25642, 0, 0, 16, 16, 3, height, 16, 16, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 3, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    }
}

/** rct2: 0x008A74D4 */
static void stand_up_rc_track_left_eighth_to_orthogonal(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    stand_up_rc_track_right_eighth_to_diag(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008A74E4 */
static void stand_up_rc_track_right_eighth_to_orthogonal(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    stand_up_rc_track_left_eighth_to_diag(rideIndex, trackSequence, (direction + 3) & 3, height, mapElement);
}

/** rct2: 0x008A76A4 */
static void stand_up_rc_track_left_eighth_bank_to_diag(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25675, 0, 0, 32, 1, 26, height, 0, 27, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25679, 0, 0, 32, 1, 26, height, 0, 27, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25683, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25687, 0, 0, 32, 20, 3, height, 0, 6, height);
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
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25676, 0, 0, 32, 16, 3, height, 0, 0, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25680, 0, 0, 34, 16, 0, height, 0, 0, height + 27);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25684, 0, 0, 32, 16, 3, height, 0, 16, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25688, 0, 0, 32, 16, 3, height, 0, 16, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25677, 0, 0, 16, 16, 3, height, 0, 16, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25681, 0, 0, 16, 16, 0, height, 16, 16, height + 27);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25685, 0, 0, 16, 16, 3, height, 16, 0, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25689, 0, 0, 16, 16, 3, height, 0, 0, height);
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
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25678, 0, 0, 16, 16, 3, height, 16, 16, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 3, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25682, 0, 0, 16, 18, 0, height, 0, 16, height + 27);
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 1, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25686, 0, 0, 16, 16, 3, height, 0, 0, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 0, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25690, 0, 0, 16, 16, 3, height, 16, 0, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 2, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    }
}

/** rct2: 0x008A76B4 */
static void stand_up_rc_track_right_eighth_bank_to_diag(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25659, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25663, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25667, 0, 0, 32, 1, 26, height, 0, 27, height);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25671, 0, 0, 32, 1, 26, height, 0, 27, height);
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
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25660, 0, 0, 16, 16, 3, height, 0, 0, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25664, 0, 0, 32, 16, 3, height, 0, 16, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25668, 0, 0, 34, 16, 0, height, 0, 0, height + 27);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25672, 0, 0, 32, 16, 3, height, 0, 0, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25661, 0, 0, 16, 16, 3, height, 0, 0, height);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25665, 0, 0, 16, 16, 3, height, 16, 0, height);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25669, 0, 0, 28, 28, 0, height, 4, 4, height + 27);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25673, 0, 0, 16, 16, 3, height, 0, 16, height);
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
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25662, 0, 0, 16, 16, 3, height, 16, 0, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 1, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25666, 0, 0, 16, 16, 3, height, 0, 0, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 0, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25670, 0, 0, 16, 18, 0, height, 0, 16, height + 27);
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 2, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25674, 0, 0, 16, 16, 3, height, 16, 16, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 3, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    }
}

/** rct2: 0x008A76C4 */
static void stand_up_rc_track_left_eighth_bank_to_orthogonal(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    stand_up_rc_track_right_eighth_bank_to_diag(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008A76D4 */
static void stand_up_rc_track_right_eighth_bank_to_orthogonal(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    stand_up_rc_track_left_eighth_bank_to_diag(rideIndex, trackSequence, (direction + 3) & 3, height, mapElement);
}

/** rct2: 0x008A74F4 */
static void stand_up_rc_track_diag_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 3:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25756, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 3:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25694, -16, -16, 32, 32, 3, height, -16, -16, height);
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
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25753, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25691, -16, -16, 32, 32, 3, height, -16, -16, height);
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
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25755, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 2:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25693, -16, -16, 32, 32, 3, height, -16, -16, height);
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
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25754, -16, -16, 32, 32, 3, height, -16, -16, height);
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
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25692, -16, -16, 32, 32, 3, height, -16, -16, height);
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

/** rct2: 0x008A7524 */
static void stand_up_rc_track_diag_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 3:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25768, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 3:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25706, -16, -16, 32, 32, 3, height, -16, -16, height);
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
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25765, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25703, -16, -16, 32, 32, 3, height, -16, -16, height);
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
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25767, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 2:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25705, -16, -16, 32, 32, 3, height, -16, -16, height);
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
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25766, -16, -16, 32, 32, 3, height, -16, -16, height);
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
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25704, -16, -16, 32, 32, 3, height, -16, -16, height);
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

/** rct2: 0x008A7584 */
static void stand_up_rc_track_diag_60_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 3:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25780, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 3:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25718, -16, -16, 32, 32, 3, height, -16, -16, height);
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
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25777, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25715, -16, -16, 32, 32, 3, height, -16, -16, height);
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
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25779, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 2:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25717, -16, -16, 32, 32, 3, height, -16, -16, height);
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
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 1, 32, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25778, -16, -16, 32, 32, 3, height, -16, -16, height);
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 0, 36, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 2:
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 2, 32, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 3:
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 3, 36, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 1, 32, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25716, -16, -16, 32, 32, 3, height, -16, -16, height);
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 0, 36, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 2:
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 2, 32, height, gTrackColours[SCHEME_SUPPORTS]);
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

/** rct2: 0x008A7504 */
static void stand_up_rc_track_diag_flat_to_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 3:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25760, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 3:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25698, -16, -16, 32, 32, 3, height, -16, -16, height);
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
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25757, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25695, -16, -16, 32, 32, 3, height, -16, -16, height);
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
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25759, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 2:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25697, -16, -16, 32, 32, 3, height, -16, -16, height);
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
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25758, -16, -16, 32, 32, 3, height, -16, -16, height);
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
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25696, -16, -16, 32, 32, 3, height, -16, -16, height);
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

/** rct2: 0x008A7564 */
static void stand_up_rc_track_diag_25_deg_up_to_60_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 3:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25772, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 3:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25710, -16, -16, 32, 32, 3, height, -16, -16, height);
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
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25769, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25707, -16, -16, 32, 32, 3, height, -16, -16, height);
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
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25771, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 2:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25709, -16, -16, 32, 32, 3, height, -16, -16, height);
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
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25770, -16, -16, 32, 32, 3, height, -16, -16, height);
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
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25708, -16, -16, 32, 32, 3, height, -16, -16, height);
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

/** rct2: 0x008A7574 */
static void stand_up_rc_track_diag_60_deg_up_to_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 3:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25776, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 3:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25714, -16, -16, 32, 32, 3, height, -16, -16, height);
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
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25773, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25711, -16, -16, 32, 32, 3, height, -16, -16, height);
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
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25775, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 2:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25713, -16, -16, 32, 32, 3, height, -16, -16, height);
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
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25774, -16, -16, 16, 16, 3, height, 0, 0, height);
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
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25712, -16, -16, 16, 16, 3, height, 0, 0, height);
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

/** rct2: 0x008A7514 */
static void stand_up_rc_track_diag_25_deg_up_to_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 3:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25764, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 3:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25702, -16, -16, 32, 32, 3, height, -16, -16, height);
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
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25761, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25699, -16, -16, 32, 32, 3, height, -16, -16, height);
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
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25763, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 2:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25701, -16, -16, 32, 32, 3, height, -16, -16, height);
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
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25762, -16, -16, 32, 32, 3, height, -16, -16, height);
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
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25700, -16, -16, 32, 32, 3, height, -16, -16, height);
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

/** rct2: 0x008A7554 */
static void stand_up_rc_track_diag_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 3:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25766, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 3:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25704, -16, -16, 32, 32, 3, height, -16, -16, height);
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
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25767, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25705, -16, -16, 32, 32, 3, height, -16, -16, height);
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
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25765, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 2:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25703, -16, -16, 32, 32, 3, height, -16, -16, height);
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
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25768, -16, -16, 32, 32, 3, height, -16, -16, height);
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
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25706, -16, -16, 32, 32, 3, height, -16, -16, height);
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

/** rct2: 0x008A75B4 */
static void stand_up_rc_track_diag_60_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 3:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25778, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 3:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25716, -16, -16, 32, 32, 3, height, -16, -16, height);
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
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25779, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25717, -16, -16, 32, 32, 3, height, -16, -16, height);
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
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25777, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 2:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25715, -16, -16, 32, 32, 3, height, -16, -16, height);
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
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 1, 24, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25780, -16, -16, 32, 32, 3, height, -16, -16, height);
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 0, 28, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 2:
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 2, 24, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 3:
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 3, 28, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 1, 24, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25718, -16, -16, 32, 32, 3, height, -16, -16, height);
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 0, 28, height, gTrackColours[SCHEME_SUPPORTS]);
                break;
            case 2:
                metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES, 2, 24, height, gTrackColours[SCHEME_SUPPORTS]);
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

/** rct2: 0x008A7534 */
static void stand_up_rc_track_diag_flat_to_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 3:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25762, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 3:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25700, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        break;
    case 1:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 0:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25763, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25701, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        break;
    case 2:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 2:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25761, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 2:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25699, -16, -16, 32, 32, 3, height, -16, -16, height);
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
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25764, -16, -16, 32, 32, 3, height, -16, -16, height);
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
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25702, -16, -16, 32, 32, 3, height, -16, -16, height);
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

/** rct2: 0x008A7594 */
static void stand_up_rc_track_diag_25_deg_down_to_60_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 3:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25774, -16, -16, 16, 16, 3, height, 0, 0, height);
                break;
            }
        } else {
            switch (direction) {
            case 3:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25712, -16, -16, 16, 16, 3, height, 0, 0, height);
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
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25775, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25713, -16, -16, 32, 32, 3, height, -16, -16, height);
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
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25773, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 2:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25711, -16, -16, 32, 32, 3, height, -16, -16, height);
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
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25776, -16, -16, 32, 32, 3, height, -16, -16, height);
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
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25714, -16, -16, 32, 32, 3, height, -16, -16, height);
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

/** rct2: 0x008A75A4 */
static void stand_up_rc_track_diag_60_deg_down_to_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 3:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25770, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 3:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25708, -16, -16, 32, 32, 3, height, -16, -16, height);
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
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25771, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25709, -16, -16, 32, 32, 3, height, -16, -16, height);
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
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25769, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 2:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25707, -16, -16, 32, 32, 3, height, -16, -16, height);
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
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25772, -16, -16, 32, 32, 3, height, -16, -16, height);
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
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25710, -16, -16, 32, 32, 3, height, -16, -16, height);
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

/** rct2: 0x008A7544 */
static void stand_up_rc_track_diag_25_deg_down_to_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 3:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25758, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 3:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25696, -16, -16, 32, 32, 3, height, -16, -16, height);
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
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25759, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25697, -16, -16, 32, 32, 3, height, -16, -16, height);
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
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25757, -16, -16, 32, 32, 3, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 2:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25695, -16, -16, 32, 32, 3, height, -16, -16, height);
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
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25760, -16, -16, 32, 32, 3, height, -16, -16, height);
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
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25698, -16, -16, 32, 32, 3, height, -16, -16, height);
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

/** rct2: 0x008A75E4 */
static void stand_up_rc_track_diag_flat_to_left_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25726, -16, -16, 32, 32, 3, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25723, -16, -16, 32, 32, 3, height, -16, -16, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25727, -16, -16, 32, 32, 0, height, -16, -16, height + 27);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 2:
        switch (direction) {
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25725, -16, -16, 32, 32, 3, height, -16, -16, height);
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
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25724, -16, -16, 32, 32, 3, height, -16, -16, height);
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

/** rct2: 0x008A75F4 */
static void stand_up_rc_track_diag_flat_to_right_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25731, -16, -16, 32, 32, 3, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25728, -16, -16, 32, 32, 3, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 2:
        switch (direction) {
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25730, -16, -16, 32, 32, 3, height, -16, -16, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25732, -16, -16, 32, 32, 0, height, -16, -16, height + 27);
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
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25729, -16, -16, 32, 32, 3, height, -16, -16, height);
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

/** rct2: 0x008A7604 */
static void stand_up_rc_track_diag_left_bank_to_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25729, -16, -16, 32, 32, 3, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25730, -16, -16, 32, 32, 3, height, -16, -16, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25732, -16, -16, 32, 32, 0, height, -16, -16, height + 27);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 2:
        switch (direction) {
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25728, -16, -16, 32, 32, 3, height, -16, -16, height);
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
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25731, -16, -16, 32, 32, 3, height, -16, -16, height);
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

/** rct2: 0x008A7614 */
static void stand_up_rc_track_diag_right_bank_to_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25724, -16, -16, 32, 32, 3, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25725, -16, -16, 32, 32, 3, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 2:
        switch (direction) {
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25723, -16, -16, 32, 32, 3, height, -16, -16, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25727, -16, -16, 32, 32, 0, height, -16, -16, height + 27);
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
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25726, -16, -16, 32, 32, 3, height, -16, -16, height);
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

/** rct2: 0x008A7644 */
static void stand_up_rc_track_diag_left_bank_to_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25746, -16, -16, 32, 32, 3, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25743, -16, -16, 32, 32, 3, height, -16, -16, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25747, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 2:
        switch (direction) {
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25745, -16, -16, 32, 32, 3, height, -16, -16, height);
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
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25744, -16, -16, 32, 32, 3, height, -16, -16, height);
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

/** rct2: 0x008A7654 */
static void stand_up_rc_track_diag_right_bank_to_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25751, -16, -16, 32, 32, 3, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25748, -16, -16, 32, 32, 3, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 2:
        switch (direction) {
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25750, -16, -16, 32, 32, 3, height, -16, -16, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25752, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
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
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25749, -16, -16, 32, 32, 3, height, -16, -16, height);
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

/** rct2: 0x008A7624 */
static void stand_up_rc_track_diag_25_deg_up_to_left_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25736, -16, -16, 32, 32, 3, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 56, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25733, -16, -16, 32, 32, 3, height, -16, -16, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25737, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 56, 0x20);
        break;
    case 2:
        switch (direction) {
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25735, -16, -16, 32, 32, 3, height, -16, -16, height);
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
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25734, -16, -16, 32, 32, 3, height, -16, -16, height);
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

/** rct2: 0x008A7634 */
static void stand_up_rc_track_diag_25_deg_up_to_right_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25741, -16, -16, 32, 32, 3, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 56, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25738, -16, -16, 32, 32, 3, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 56, 0x20);
        break;
    case 2:
        switch (direction) {
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25740, -16, -16, 32, 32, 3, height, -16, -16, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25742, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
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
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25739, -16, -16, 32, 32, 3, height, -16, -16, height);
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

/** rct2: 0x008A7664 */
static void stand_up_rc_track_diag_left_bank_to_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25739, -16, -16, 32, 32, 3, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25740, -16, -16, 32, 32, 3, height, -16, -16, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25742, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        break;
    case 2:
        switch (direction) {
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25738, -16, -16, 32, 32, 3, height, -16, -16, height);
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
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25741, -16, -16, 32, 32, 3, height, -16, -16, height);
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

/** rct2: 0x008A7674 */
static void stand_up_rc_track_diag_right_bank_to_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25734, -16, -16, 32, 32, 3, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25735, -16, -16, 32, 32, 3, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        break;
    case 2:
        switch (direction) {
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 16676, -16, -16, 32, 32, 3, height, -16, -16, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 16680, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
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
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25736, -16, -16, 32, 32, 3, height, -16, -16, height);
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

/** rct2: 0x008A7684 */
static void stand_up_rc_track_diag_25_deg_down_to_left_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25749, -16, -16, 32, 32, 3, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25750, -16, -16, 32, 32, 3, height, -16, -16, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25752, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 2:
        switch (direction) {
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25748, -16, -16, 32, 32, 3, height, -16, -16, height);
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
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25751, -16, -16, 32, 32, 3, height, -16, -16, height);
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

/** rct2: 0x008A7694 */
static void stand_up_rc_track_diag_25_deg_down_to_right_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25744, -16, -16, 32, 32, 3, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25745, -16, -16, 32, 32, 3, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 2:
        switch (direction) {
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25743, -16, -16, 32, 32, 3, height, -16, -16, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25747, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
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
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25746, -16, -16, 32, 32, 3, height, -16, -16, height);
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

/** rct2: 0x008A75C4 */
static void stand_up_rc_track_diag_left_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25722, -16, -16, 32, 32, 3, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25719, -16, -16, 32, 32, 0, height, -16, -16, height + 27);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 2:
        switch (direction) {
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25721, -16, -16, 32, 32, 3, height, -16, -16, height);
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
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25720, -16, -16, 32, 32, 3, height, -16, -16, height);
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

/** rct2: 0x008A75D4 */
static void stand_up_rc_track_diag_right_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25720, -16, -16, 32, 32, 3, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25721, -16, -16, 32, 32, 3, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 2:
        switch (direction) {
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25719, -16, -16, 32, 32, 0, height, -16, -16, height + 27);
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
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25722, -16, -16, 32, 32, 3, height, -16, -16, height);
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

/** rct2: 0x008A76E4 */
static void stand_up_rc_track_block_brakes(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
    case 2:
        sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25571, 0, 6, 32, 20, 3, height);
        break;
    case 1:
    case 3:
        sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25572, 0, 6, 32, 20, 3, height);
        break;
    }
    if (track_paint_util_should_paint_supports(gPaintSession.MapPosition)) {
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
    }
    paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
    paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(height + 32, 0x20);
}

TRACK_PAINT_FUNCTION get_track_paint_function_stand_up_rc(sint32 trackType, sint32 direction)
{
    switch (trackType) {
    case TRACK_ELEM_FLAT:
        return stand_up_rc_track_flat;
    case TRACK_ELEM_END_STATION:
    case TRACK_ELEM_BEGIN_STATION:
    case TRACK_ELEM_MIDDLE_STATION:
        return stand_up_rc_track_station;
    case TRACK_ELEM_25_DEG_UP:
        return stand_up_rc_track_25_deg_up;
    case TRACK_ELEM_60_DEG_UP:
        return stand_up_rc_track_60_deg_up;
    case TRACK_ELEM_FLAT_TO_25_DEG_UP:
        return stand_up_rc_track_flat_to_25_deg_up;
    case TRACK_ELEM_25_DEG_UP_TO_60_DEG_UP:
        return stand_up_rc_track_25_deg_up_to_60_deg_up;
    case TRACK_ELEM_60_DEG_UP_TO_25_DEG_UP:
        return stand_up_rc_track_60_deg_up_to_25_deg_up;
    case TRACK_ELEM_25_DEG_UP_TO_FLAT:
        return stand_up_rc_track_25_deg_up_to_flat;
    case TRACK_ELEM_25_DEG_DOWN:
        return stand_up_rc_track_25_deg_down;
    case TRACK_ELEM_60_DEG_DOWN:
        return stand_up_rc_track_60_deg_down;
    case TRACK_ELEM_FLAT_TO_25_DEG_DOWN:
        return stand_up_rc_track_flat_to_25_deg_down;
    case TRACK_ELEM_25_DEG_DOWN_TO_60_DEG_DOWN:
        return stand_up_rc_track_25_deg_down_to_60_deg_down;
    case TRACK_ELEM_60_DEG_DOWN_TO_25_DEG_DOWN:
        return stand_up_rc_track_60_deg_down_to_25_deg_down;
    case TRACK_ELEM_25_DEG_DOWN_TO_FLAT:
        return stand_up_rc_track_25_deg_down_to_flat;
    case TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES:
        return stand_up_rc_track_left_quarter_turn_5;
    case TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES:
        return stand_up_rc_track_right_quarter_turn_5;
    case TRACK_ELEM_FLAT_TO_LEFT_BANK:
        return stand_up_rc_track_flat_to_left_bank;
    case TRACK_ELEM_FLAT_TO_RIGHT_BANK:
        return stand_up_rc_track_flat_to_right_bank;
    case TRACK_ELEM_LEFT_BANK_TO_FLAT:
        return stand_up_rc_track_left_bank_to_flat;
    case TRACK_ELEM_RIGHT_BANK_TO_FLAT:
        return stand_up_rc_track_right_bank_to_flat;
    case TRACK_ELEM_BANKED_LEFT_QUARTER_TURN_5_TILES:
        return stand_up_rc_track_banked_left_quarter_turn_5;
    case TRACK_ELEM_BANKED_RIGHT_QUARTER_TURN_5_TILES:
        return stand_up_rc_track_banked_right_quarter_turn_5;
    case TRACK_ELEM_LEFT_BANK_TO_25_DEG_UP:
        return stand_up_rc_track_left_bank_to_25_deg_up;
    case TRACK_ELEM_RIGHT_BANK_TO_25_DEG_UP:
        return stand_up_rc_track_right_bank_to_25_deg_up;
    case TRACK_ELEM_25_DEG_UP_TO_LEFT_BANK:
        return stand_up_rc_track_25_deg_up_to_left_bank;
    case TRACK_ELEM_25_DEG_UP_TO_RIGHT_BANK:
        return stand_up_rc_track_25_deg_up_to_right_bank;
    case TRACK_ELEM_LEFT_BANK_TO_25_DEG_DOWN:
        return stand_up_rc_track_left_bank_to_25_deg_down;
    case TRACK_ELEM_RIGHT_BANK_TO_25_DEG_DOWN:
        return stand_up_rc_track_right_bank_to_25_deg_down;
    case TRACK_ELEM_25_DEG_DOWN_TO_LEFT_BANK:
        return stand_up_rc_track_25_deg_down_to_left_bank;
    case TRACK_ELEM_25_DEG_DOWN_TO_RIGHT_BANK:
        return stand_up_rc_track_25_deg_down_to_right_bank;
    case TRACK_ELEM_LEFT_BANK:
        return stand_up_rc_track_left_bank;
    case TRACK_ELEM_RIGHT_BANK:
        return stand_up_rc_track_right_bank;
    case TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES_25_DEG_UP:
        return stand_up_rc_track_left_quarter_turn_5_25_deg_up;
    case TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES_25_DEG_UP:
        return stand_up_rc_track_right_quarter_turn_5_25_deg_up;
    case TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES_25_DEG_DOWN:
        return stand_up_rc_track_left_quarter_turn_5_25_deg_down;
    case TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES_25_DEG_DOWN:
        return stand_up_rc_track_right_quarter_turn_5_25_deg_down;
    case TRACK_ELEM_S_BEND_LEFT:
        return stand_up_rc_track_s_bend_left;
    case TRACK_ELEM_S_BEND_RIGHT:
        return stand_up_rc_track_s_bend_right;
    case TRACK_ELEM_LEFT_VERTICAL_LOOP:
        return stand_up_rc_track_left_vertical_loop;
    case TRACK_ELEM_RIGHT_VERTICAL_LOOP:
        return stand_up_rc_track_right_vertical_loop;
    case TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES:
        return stand_up_rc_track_left_quarter_turn_3;
    case TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES:
        return stand_up_rc_track_right_quarter_turn_3;
    case TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_BANK:
        return stand_up_rc_track_left_quarter_turn_3_bank;
    case TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_BANK:
        return stand_up_rc_track_right_quarter_turn_3_bank;
    case TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_25_DEG_UP:
        return stand_up_rc_track_left_quarter_turn_3_25_deg_up;
    case TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_25_DEG_UP:
        return stand_up_rc_track_right_quarter_turn_3_25_deg_up;
    case TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_25_DEG_DOWN:
        return stand_up_rc_track_left_quarter_turn_3_25_deg_down;
    case TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_25_DEG_DOWN:
        return stand_up_rc_track_right_quarter_turn_3_25_deg_down;
    case TRACK_ELEM_HALF_LOOP_UP:
        return stand_up_rc_track_half_loop_up;
    case TRACK_ELEM_HALF_LOOP_DOWN:
        return stand_up_rc_track_half_loop_down;
    case TRACK_ELEM_LEFT_CORKSCREW_UP:
        return stand_up_rc_track_left_corkscrew_up;
    case TRACK_ELEM_RIGHT_CORKSCREW_UP:
        return stand_up_rc_track_right_corkscrew_up;
    case TRACK_ELEM_LEFT_CORKSCREW_DOWN:
        return stand_up_rc_track_left_corkscrew_down;
    case TRACK_ELEM_RIGHT_CORKSCREW_DOWN:
        return stand_up_rc_track_right_corkscrew_down;
    case TRACK_ELEM_LEFT_HALF_BANKED_HELIX_UP_SMALL:
        return stand_up_rc_track_left_half_banked_helix_up_small;
    case TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_UP_SMALL:
        return stand_up_rc_track_right_half_banked_helix_up_small;
    case TRACK_ELEM_LEFT_HALF_BANKED_HELIX_DOWN_SMALL:
        return stand_up_rc_track_left_half_banked_helix_down_small;
    case TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_DOWN_SMALL:
        return stand_up_rc_track_right_half_banked_helix_down_small;
    case TRACK_ELEM_LEFT_HALF_BANKED_HELIX_UP_LARGE:
        return stand_up_rc_track_left_half_banked_helix_up_large;
    case TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_UP_LARGE:
        return stand_up_rc_track_right_half_banked_helix_up_large;
    case TRACK_ELEM_LEFT_HALF_BANKED_HELIX_DOWN_LARGE:
        return stand_up_rc_track_left_half_banked_helix_down_large;
    case TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_DOWN_LARGE:
        return stand_up_rc_track_right_half_banked_helix_down_large;
    case TRACK_ELEM_BRAKES:
        return stand_up_rc_track_brakes;
    case TRACK_ELEM_ON_RIDE_PHOTO:
        return stand_up_rc_track_on_ride_photo;
    case TRACK_ELEM_LEFT_EIGHTH_TO_DIAG:
        return stand_up_rc_track_left_eighth_to_diag;
    case TRACK_ELEM_RIGHT_EIGHTH_TO_DIAG:
        return stand_up_rc_track_right_eighth_to_diag;
    case TRACK_ELEM_LEFT_EIGHTH_TO_ORTHOGONAL:
        return stand_up_rc_track_left_eighth_to_orthogonal;
    case TRACK_ELEM_RIGHT_EIGHTH_TO_ORTHOGONAL:
        return stand_up_rc_track_right_eighth_to_orthogonal;
    case TRACK_ELEM_LEFT_EIGHTH_BANK_TO_DIAG:
        return stand_up_rc_track_left_eighth_bank_to_diag;
    case TRACK_ELEM_RIGHT_EIGHTH_BANK_TO_DIAG:
        return stand_up_rc_track_right_eighth_bank_to_diag;
    case TRACK_ELEM_LEFT_EIGHTH_BANK_TO_ORTHOGONAL:
        return stand_up_rc_track_left_eighth_bank_to_orthogonal;
    case TRACK_ELEM_RIGHT_EIGHTH_BANK_TO_ORTHOGONAL:
        return stand_up_rc_track_right_eighth_bank_to_orthogonal;
    case TRACK_ELEM_DIAG_FLAT:
        return stand_up_rc_track_diag_flat;
    case TRACK_ELEM_DIAG_25_DEG_UP:
        return stand_up_rc_track_diag_25_deg_up;
    case TRACK_ELEM_DIAG_60_DEG_UP:
        return stand_up_rc_track_diag_60_deg_up;
    case TRACK_ELEM_DIAG_FLAT_TO_25_DEG_UP:
        return stand_up_rc_track_diag_flat_to_25_deg_up;
    case TRACK_ELEM_DIAG_25_DEG_UP_TO_60_DEG_UP:
        return stand_up_rc_track_diag_25_deg_up_to_60_deg_up;
    case TRACK_ELEM_DIAG_60_DEG_UP_TO_25_DEG_UP:
        return stand_up_rc_track_diag_60_deg_up_to_25_deg_up;
    case TRACK_ELEM_DIAG_25_DEG_UP_TO_FLAT:
        return stand_up_rc_track_diag_25_deg_up_to_flat;
    case TRACK_ELEM_DIAG_25_DEG_DOWN:
        return stand_up_rc_track_diag_25_deg_down;
    case TRACK_ELEM_DIAG_60_DEG_DOWN:
        return stand_up_rc_track_diag_60_deg_down;
    case TRACK_ELEM_DIAG_FLAT_TO_25_DEG_DOWN:
        return stand_up_rc_track_diag_flat_to_25_deg_down;
    case TRACK_ELEM_DIAG_25_DEG_DOWN_TO_60_DEG_DOWN:
        return stand_up_rc_track_diag_25_deg_down_to_60_deg_down;
    case TRACK_ELEM_DIAG_60_DEG_DOWN_TO_25_DEG_DOWN:
        return stand_up_rc_track_diag_60_deg_down_to_25_deg_down;
    case TRACK_ELEM_DIAG_25_DEG_DOWN_TO_FLAT:
        return stand_up_rc_track_diag_25_deg_down_to_flat;
    case TRACK_ELEM_DIAG_FLAT_TO_LEFT_BANK:
        return stand_up_rc_track_diag_flat_to_left_bank;
    case TRACK_ELEM_DIAG_FLAT_TO_RIGHT_BANK:
        return stand_up_rc_track_diag_flat_to_right_bank;
    case TRACK_ELEM_DIAG_LEFT_BANK_TO_FLAT:
        return stand_up_rc_track_diag_left_bank_to_flat;
    case TRACK_ELEM_DIAG_RIGHT_BANK_TO_FLAT:
        return stand_up_rc_track_diag_right_bank_to_flat;
    case TRACK_ELEM_DIAG_LEFT_BANK_TO_25_DEG_UP:
        return stand_up_rc_track_diag_left_bank_to_25_deg_up;
    case TRACK_ELEM_DIAG_RIGHT_BANK_TO_25_DEG_UP:
        return stand_up_rc_track_diag_right_bank_to_25_deg_up;
    case TRACK_ELEM_DIAG_25_DEG_UP_TO_LEFT_BANK:
        return stand_up_rc_track_diag_25_deg_up_to_left_bank;
    case TRACK_ELEM_DIAG_25_DEG_UP_TO_RIGHT_BANK:
        return stand_up_rc_track_diag_25_deg_up_to_right_bank;
    case TRACK_ELEM_DIAG_LEFT_BANK_TO_25_DEG_DOWN:
        return stand_up_rc_track_diag_left_bank_to_25_deg_down;
    case TRACK_ELEM_DIAG_RIGHT_BANK_TO_25_DEG_DOWN:
        return stand_up_rc_track_diag_right_bank_to_25_deg_down;
    case TRACK_ELEM_DIAG_25_DEG_DOWN_TO_LEFT_BANK:
        return stand_up_rc_track_diag_25_deg_down_to_left_bank;
    case TRACK_ELEM_DIAG_25_DEG_DOWN_TO_RIGHT_BANK:
        return stand_up_rc_track_diag_25_deg_down_to_right_bank;
    case TRACK_ELEM_DIAG_LEFT_BANK:
        return stand_up_rc_track_diag_left_bank;
    case TRACK_ELEM_DIAG_RIGHT_BANK:
        return stand_up_rc_track_diag_right_bank;
    case TRACK_ELEM_BLOCK_BRAKES:
        return stand_up_rc_track_block_brakes;
    }
    return NULL;
}
