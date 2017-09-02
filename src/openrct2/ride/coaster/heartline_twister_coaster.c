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

/** rct2: 0x0087694C */
static void heartline_twister_rc_track_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    if (track_element_is_lift_hill(mapElement)) {
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21354, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21356, 0, 0, 32, 1, 26, height, 0, 27, height);
            wooden_a_supports_paint_setup(6, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21355, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21357, 0, 0, 32, 1, 26, height, 0, 27, height);
            wooden_a_supports_paint_setup(7, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21358, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21360, 0, 0, 32, 1, 26, height, 0, 27, height);
            wooden_a_supports_paint_setup(6, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21359, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21361, 0, 0, 32, 1, 26, height, 0, 27, height);
            wooden_a_supports_paint_setup(7, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
    } else {
        switch (direction) {
        case 0:
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21294, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21296, 0, 0, 32, 1, 26, height, 0, 27, height);
            wooden_a_supports_paint_setup(6, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21295, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21297, 0, 0, 32, 1, 26, height, 0, 27, height);
            wooden_a_supports_paint_setup(7, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
    }
    paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 32, 0x20);
}

static void heartline_twister_rc_track_station(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    static const uint32 imageIds[4][3] = {
        { 19732, SPR_STATION_BASE_B_SW_NE },
        { 19733, SPR_STATION_BASE_B_NW_SE },
        { 19732, SPR_STATION_BASE_B_SW_NE },
        { 19733, SPR_STATION_BASE_B_NW_SE },
    };

    sub_98197C_rotated(direction, imageIds[direction][0] | gTrackColours[SCHEME_TRACK], 0, 0, 32, 20, 1, height, 0, 6, height + 3);
    sub_98196C_rotated(direction, imageIds[direction][1] | gTrackColours[SCHEME_MISC], 0, 0, 32, 32, 1, height);
    track_paint_util_draw_station_metal_supports_2(direction, height, gTrackColours[SCHEME_SUPPORTS], 0);
    track_paint_util_draw_station(rideIndex, trackSequence, direction, height, mapElement);
    paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);
    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 32, 0x20);
}

/** rct2: 0x0087695C */
static void heartline_twister_rc_track_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    if (track_element_is_lift_hill(mapElement)) {
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21378, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21382, 0, 0, 32, 1, 50, height, 0, 27, height);
            wooden_a_supports_paint_setup(6, 9, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21379, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21383, 0, 0, 32, 1, 50, height, 0, 27, height);
            wooden_a_supports_paint_setup(7, 10, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21380, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21384, 0, 0, 32, 1, 50, height, 0, 27, height);
            wooden_a_supports_paint_setup(6, 11, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21381, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21385, 0, 0, 32, 1, 50, height, 0, 27, height);
            wooden_a_supports_paint_setup(7, 12, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
    } else {
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21322, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21326, 0, 0, 32, 1, 50, height, 0, 27, height);
            wooden_a_supports_paint_setup(6, 9, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21323, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21327, 0, 0, 32, 1, 50, height, 0, 27, height);
            wooden_a_supports_paint_setup(7, 10, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21324, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21328, 0, 0, 32, 1, 50, height, 0, 27, height);
            wooden_a_supports_paint_setup(6, 11, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21325, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21329, 0, 0, 32, 1, 50, height, 0, 27, height);
            wooden_a_supports_paint_setup(7, 12, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
    }
    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_1);
    } else {
        paint_util_push_tunnel_rotated(direction, height + 8, TUNNEL_2);
    }
    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 56, 0x20);
}

/** rct2: 0x008769FC */
static void heartline_twister_rc_track_60_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    if (track_element_is_lift_hill(mapElement)) {
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21402, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21406, 0, 0, 32, 1, 98, height, 0, 27, height);
            wooden_a_supports_paint_setup(6, 21, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            gPaintSession.WoodenSupportsPrependTo = sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21403, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21407, 0, 0, 32, 1, 98, height, 0, 27, height);
            wooden_a_supports_paint_setup(7, 22, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            gPaintSession.WoodenSupportsPrependTo = sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21404, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21408, 0, 0, 32, 1, 98, height, 0, 27, height);
            wooden_a_supports_paint_setup(6, 23, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21405, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21409, 0, 0, 32, 1, 98, height, 0, 27, height);
            wooden_a_supports_paint_setup(7, 24, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
    } else {
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21346, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21350, 0, 0, 32, 1, 98, height, 0, 27, height);
            wooden_a_supports_paint_setup(6, 21, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            gPaintSession.WoodenSupportsPrependTo = sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21347, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21351, 0, 0, 32, 1, 98, height, 0, 27, height);
            wooden_a_supports_paint_setup(7, 22, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            gPaintSession.WoodenSupportsPrependTo = sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21348, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21352, 0, 0, 32, 1, 98, height, 0, 27, height);
            wooden_a_supports_paint_setup(6, 23, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21349, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21353, 0, 0, 32, 1, 98, height, 0, 27, height);
            wooden_a_supports_paint_setup(7, 24, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
    }
    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_1);
    } else {
        paint_util_push_tunnel_rotated(direction, height + 56, TUNNEL_2);
    }
    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 104, 0x20);
}

/** rct2: 0x0087696C */
static void heartline_twister_rc_track_flat_to_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    if (track_element_is_lift_hill(mapElement)) {
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21362, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21366, 0, 0, 32, 1, 42, height, 0, 27, height);
            wooden_a_supports_paint_setup(6, 1, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21363, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21367, 0, 0, 32, 1, 42, height, 0, 27, height);
            wooden_a_supports_paint_setup(7, 2, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21364, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21368, 0, 0, 32, 1, 42, height, 0, 27, height);
            wooden_a_supports_paint_setup(6, 3, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21365, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21369, 0, 0, 32, 1, 42, height, 0, 27, height);
            wooden_a_supports_paint_setup(7, 4, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
    } else {
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21306, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21310, 0, 0, 32, 1, 42, height, 0, 27, height);
            wooden_a_supports_paint_setup(6, 1, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21307, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21311, 0, 0, 32, 1, 42, height, 0, 27, height);
            wooden_a_supports_paint_setup(7, 2, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21308, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21312, 0, 0, 32, 1, 42, height, 0, 27, height);
            wooden_a_supports_paint_setup(6, 3, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21309, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21313, 0, 0, 32, 1, 42, height, 0, 27, height);
            wooden_a_supports_paint_setup(7, 4, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
    }
    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
    } else {
        paint_util_push_tunnel_rotated(direction, height, TUNNEL_2);
    }
    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 48, 0x20);
}

/** rct2: 0x008769BC */
static void heartline_twister_rc_track_25_deg_up_to_60_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    if (track_element_is_lift_hill(mapElement)) {
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21386, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21390, 0, 0, 32, 1, 66, height, 0, 27, height);
            wooden_a_supports_paint_setup(6, 13, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            gPaintSession.WoodenSupportsPrependTo = sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21387, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21391, 0, 0, 32, 1, 66, height, 0, 27, height);
            wooden_a_supports_paint_setup(7, 14, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            gPaintSession.WoodenSupportsPrependTo = sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21388, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21392, 0, 0, 32, 1, 66, height, 0, 27, height);
            wooden_a_supports_paint_setup(6, 15, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21389, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21393, 0, 0, 32, 1, 66, height, 0, 27, height);
            wooden_a_supports_paint_setup(7, 16, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
    } else {
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21330, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21334, 0, 0, 32, 1, 66, height, 0, 27, height);
            wooden_a_supports_paint_setup(6, 13, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            gPaintSession.WoodenSupportsPrependTo = sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21331, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21335, 0, 0, 32, 1, 66, height, 0, 27, height);
            wooden_a_supports_paint_setup(7, 14, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            gPaintSession.WoodenSupportsPrependTo = sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21332, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21336, 0, 0, 32, 1, 66, height, 0, 27, height);
            wooden_a_supports_paint_setup(6, 15, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21333, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21337, 0, 0, 32, 1, 66, height, 0, 27, height);
            wooden_a_supports_paint_setup(7, 16, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
    }
    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_1);
    } else {
        paint_util_push_tunnel_rotated(direction, height + 24, TUNNEL_2);
    }
    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 72, 0x20);
}

/** rct2: 0x008769CC */
static void heartline_twister_rc_track_60_deg_up_to_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    if (track_element_is_lift_hill(mapElement)) {
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21394, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21398, 0, 0, 32, 1, 66, height, 0, 27, height);
            wooden_a_supports_paint_setup(6, 17, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            gPaintSession.WoodenSupportsPrependTo = sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21395, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21399, 0, 0, 32, 1, 66, height, 0, 27, height);
            wooden_a_supports_paint_setup(7, 18, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            gPaintSession.WoodenSupportsPrependTo = sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21396, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21400, 0, 0, 32, 1, 66, height, 0, 27, height);
            wooden_a_supports_paint_setup(6, 19, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21397, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21401, 0, 0, 32, 1, 66, height, 0, 27, height);
            wooden_a_supports_paint_setup(7, 20, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
    } else {
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21338, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21342, 0, 0, 32, 1, 66, height, 0, 27, height);
            wooden_a_supports_paint_setup(6, 17, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            gPaintSession.WoodenSupportsPrependTo = sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21339, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21343, 0, 0, 32, 1, 66, height, 0, 27, height);
            wooden_a_supports_paint_setup(7, 18, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            gPaintSession.WoodenSupportsPrependTo = sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21340, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21344, 0, 0, 32, 1, 66, height, 0, 27, height);
            wooden_a_supports_paint_setup(6, 19, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21341, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21345, 0, 0, 32, 1, 66, height, 0, 27, height);
            wooden_a_supports_paint_setup(7, 20, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
    }
    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_1);
    } else {
        paint_util_push_tunnel_rotated(direction, height + 24, TUNNEL_2);
    }
    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 72, 0x20);
}

/** rct2: 0x0087697C */
static void heartline_twister_rc_track_25_deg_up_to_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    if (track_element_is_lift_hill(mapElement)) {
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21370, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21374, 0, 0, 32, 1, 34, height, 0, 27, height);
            wooden_a_supports_paint_setup(6, 5, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21371, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21375, 0, 0, 32, 1, 34, height, 0, 27, height);
            wooden_a_supports_paint_setup(7, 6, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21372, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21376, 0, 0, 32, 1, 34, height, 0, 27, height);
            wooden_a_supports_paint_setup(6, 7, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21373, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21377, 0, 0, 32, 1, 34, height, 0, 27, height);
            wooden_a_supports_paint_setup(7, 8, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
    } else {
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21314, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21318, 0, 0, 32, 1, 34, height, 0, 27, height);
            wooden_a_supports_paint_setup(6, 5, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21315, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21319, 0, 0, 32, 1, 34, height, 0, 27, height);
            wooden_a_supports_paint_setup(7, 6, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21316, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21320, 0, 0, 32, 1, 34, height, 0, 27, height);
            wooden_a_supports_paint_setup(6, 7, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21317, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21321, 0, 0, 32, 1, 34, height, 0, 27, height);
            wooden_a_supports_paint_setup(7, 8, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
    }
    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_0);
    } else {
        paint_util_push_tunnel_rotated(direction, height + 8, TUNNEL_12);
    }
    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 40, 0x20);
}

/** rct2: 0x0087698C */
static void heartline_twister_rc_track_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    heartline_twister_rc_track_25_deg_up(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x00876A0C */
static void heartline_twister_rc_track_60_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    heartline_twister_rc_track_60_deg_up(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x0087699C */
static void heartline_twister_rc_track_flat_to_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    heartline_twister_rc_track_25_deg_up_to_flat(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008769DC */
static void heartline_twister_rc_track_25_deg_down_to_60_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    heartline_twister_rc_track_60_deg_up_to_25_deg_up(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008769EC */
static void heartline_twister_rc_track_60_deg_down_to_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    heartline_twister_rc_track_25_deg_up_to_60_deg_up(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008769AC */
static void heartline_twister_rc_track_25_deg_down_to_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    heartline_twister_rc_track_flat_to_25_deg_up(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x00876A6C */
static void heartline_twister_rc_track_heartline_transfer_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21306, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21310, 0, 0, 32, 20, 0, height, 0, 6, height + 23);
            wooden_a_supports_paint_setup(6, 1, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21307, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21311, 0, 0, 32, 20, 0, height, 0, 6, height + 23);
            wooden_a_supports_paint_setup(7, 2, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21308, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21312, 0, 0, 32, 20, 0, height, 0, 6, height + 23);
            wooden_a_supports_paint_setup(6, 3, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21309, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21313, 0, 0, 32, 20, 0, height, 0, 6, height + 23);
            wooden_a_supports_paint_setup(7, 4, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
        } else {
            paint_util_push_tunnel_rotated(direction, height, TUNNEL_2);
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21314, 0, 0, 32, 20, 2, height + 8, 0, 6, height + 8);
            sub_98199C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21308, 0, 0, 32, 20, 2, height + 16, 0, 6, height + 16);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21318, 0, 0, 32, 1, 34, height + 8, 0, 27, height + 8);
            sub_98199C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21312, 0, 0, 32, 1, 34, height + 16, 0, 27, height + 16);
            wooden_a_supports_paint_setup(6, 5, height + 8, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21315, 0, 0, 32, 20, 2, height + 8, 0, 6, height + 8);
            sub_98199C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21309, 0, 0, 32, 20, 2, height + 16, 0, 6, height + 16);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21319, 0, 0, 32, 1, 34, height + 8, 0, 27, height + 8);
            sub_98199C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21313, 0, 0, 32, 1, 34, height + 16, 0, 27, height + 16);
            wooden_a_supports_paint_setup(7, 6, height + 8, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21316, 0, 0, 32, 20, 2, height + 8, 0, 6, height + 8);
            sub_98199C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21306, 0, 0, 32, 20, 2, height + 16, 0, 6, height + 16);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21320, 0, 0, 32, 1, 34, height + 8, 0, 27, height + 8);
            sub_98199C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21310, 0, 0, 32, 1, 34, height + 16, 0, 27, height + 16);
            wooden_a_supports_paint_setup(6, 7, height + 8, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21317, 0, 0, 32, 20, 2, height + 8, 0, 6, height + 8);
            sub_98199C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21307, 0, 0, 32, 20, 2, height + 16, 0, 6, height + 16);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21321, 0, 0, 32, 1, 34, height + 8, 0, 27, height + 8);
            sub_98199C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21311, 0, 0, 32, 1, 34, height + 16, 0, 27, height + 16);
            wooden_a_supports_paint_setup(7, 8, height + 8, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21298, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21302, 0, 0, 32, 1, 26, height, 0, 27, height);
            wooden_a_supports_paint_setup(6, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21299, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21303, 0, 0, 32, 1, 26, height, 0, 27, height);
            wooden_a_supports_paint_setup(7, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21300, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21304, 0, 0, 32, 1, 26, height, 0, 27, height);
            wooden_a_supports_paint_setup(6, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21301, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21305, 0, 0, 32, 1, 26, height, 0, 27, height);
            wooden_a_supports_paint_setup(7, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21316, 0, 0, 32, 20, 2, height - 8, 0, 6, height - 7);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21320, 0, 0, 32, 1, 34, height - 8, 0, 27, height - 8);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21317, 0, 0, 32, 20, 2, height - 8, 0, 6, height - 7);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21321, 0, 0, 32, 1, 34, height - 8, 0, 27, height - 8);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21314, 0, 0, 32, 20, 2, height - 8, 0, 6, height - 7);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21318, 0, 0, 32, 1, 34, height - 8, 0, 27, height - 8);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21315, 0, 0, 32, 20, 2, height - 8, 0, 6, height - 7);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21319, 0, 0, 32, 1, 34, height - 8, 0, 27, height - 8);
            break;
        }
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
        } else {
            paint_util_push_tunnel_rotated(direction, height - 16, TUNNEL_0);
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    }
}

/** rct2: 0x00876A7C */
static void heartline_twister_rc_track_heartline_transfer_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21316, 0, 0, 32, 20, 2, height - 8, 0, 6, height - 7);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21320, 0, 0, 32, 1, 34, height - 8, 0, 27, height - 8);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21317, 0, 0, 32, 20, 2, height - 8, 0, 6, height - 7);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21321, 0, 0, 32, 1, 34, height - 8, 0, 27, height - 8);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21314, 0, 0, 32, 20, 2, height - 8, 0, 6, height - 7);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21318, 0, 0, 32, 1, 34, height - 8, 0, 27, height - 8);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21315, 0, 0, 32, 20, 2, height - 8, 0, 6, height - 7);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21319, 0, 0, 32, 1, 34, height - 8, 0, 27, height - 8);
            break;
        }
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
        } else {
            paint_util_push_tunnel_rotated(direction, height - 16, TUNNEL_0);
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21314, 0, 0, 32, 20, 2, height + 8, 0, 6, height + 8);
            sub_98199C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21308, 0, 0, 32, 20, 2, height + 16, 0, 6, height + 16);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21318, 0, 0, 32, 1, 34, height + 8, 0, 27, height + 8);
            sub_98199C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21312, 0, 0, 32, 1, 34, height + 16, 0, 27, height + 16);
            wooden_a_supports_paint_setup(6, 5, height + 8, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21315, 0, 0, 32, 20, 2, height + 8, 0, 6, height + 8);
            sub_98199C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21309, 0, 0, 32, 20, 2, height + 16, 0, 6, height + 16);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21319, 0, 0, 32, 1, 34, height + 8, 0, 27, height + 8);
            sub_98199C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21313, 0, 0, 32, 1, 34, height + 16, 0, 27, height + 16);
            wooden_a_supports_paint_setup(7, 6, height + 8, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21316, 0, 0, 32, 20, 2, height + 8, 0, 6, height + 8);
            sub_98199C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21306, 0, 0, 32, 20, 2, height + 16, 0, 6, height + 16);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21320, 0, 0, 32, 1, 34, height + 8, 0, 27, height + 8);
            sub_98199C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21310, 0, 0, 32, 1, 34, height + 16, 0, 27, height + 16);
            wooden_a_supports_paint_setup(6, 7, height + 8, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21317, 0, 0, 32, 20, 2, height + 8, 0, 6, height + 8);
            sub_98199C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21307, 0, 0, 32, 20, 2, height + 16, 0, 6, height + 16);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21321, 0, 0, 32, 1, 34, height + 8, 0, 27, height + 8);
            sub_98199C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21311, 0, 0, 32, 1, 34, height + 16, 0, 27, height + 16);
            wooden_a_supports_paint_setup(7, 8, height + 8, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21298, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21302, 0, 0, 32, 1, 26, height, 0, 27, height);
            wooden_a_supports_paint_setup(6, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21299, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21303, 0, 0, 32, 1, 26, height, 0, 27, height);
            wooden_a_supports_paint_setup(7, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21300, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21304, 0, 0, 32, 1, 26, height, 0, 27, height);
            wooden_a_supports_paint_setup(6, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21301, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21305, 0, 0, 32, 1, 26, height, 0, 27, height);
            wooden_a_supports_paint_setup(7, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21306, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21310, 0, 0, 32, 20, 0, height, 0, 6, height + 23);
            wooden_a_supports_paint_setup(6, 1, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21307, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21311, 0, 0, 32, 20, 0, height, 0, 6, height + 23);
            wooden_a_supports_paint_setup(7, 2, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21308, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21312, 0, 0, 32, 20, 0, height, 0, 6, height + 23);
            wooden_a_supports_paint_setup(6, 3, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21309, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21313, 0, 0, 32, 20, 0, height, 0, 6, height + 23);
            wooden_a_supports_paint_setup(7, 4, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
        } else {
            paint_util_push_tunnel_rotated(direction, height, TUNNEL_2);
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    }
}

/** rct2: 0x00876A4C */
static void heartline_twister_rc_track_left_heartline_roll(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21410, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21422, 0, 0, 32, 1, 26, height, 0, 27, height);
            wooden_a_supports_paint_setup(6, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21413, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21425, 0, 0, 32, 1, 26, height, 0, 27, height);
            wooden_a_supports_paint_setup(7, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21416, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21428, 0, 0, 32, 1, 26, height, 0, 27, height);
            wooden_a_supports_paint_setup(6, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21419, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21431, 0, 0, 32, 1, 26, height, 0, 27, height);
            wooden_a_supports_paint_setup(7, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21411, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21423, 0, 0, 32, 1, 26, height, 0, 27, height);
            wooden_a_supports_paint_setup(6, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21414, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21426, 0, 0, 32, 1, 26, height, 0, 27, height);
            wooden_a_supports_paint_setup(7, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21417, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21429, 0, 0, 32, 1, 26, height, 0, 27, height);
            wooden_a_supports_paint_setup(6, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21420, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21432, 0, 0, 32, 1, 26, height, 0, 27, height);
            wooden_a_supports_paint_setup(7, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21412, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21424, 0, 0, 32, 1, 26, height, 0, 27, height);
            wooden_a_supports_paint_setup(6, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21415, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21427, 0, 0, 32, 1, 26, height, 0, 27, height);
            wooden_a_supports_paint_setup(7, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21418, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21430, 0, 0, 32, 1, 26, height, 0, 27, height);
            wooden_a_supports_paint_setup(6, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21421, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21433, 0, 0, 32, 1, 26, height, 0, 27, height);
            wooden_a_supports_paint_setup(7, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21418, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21430, 0, 0, 32, 1, 26, height, 0, 27, height);
            wooden_a_supports_paint_setup(6, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21421, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21433, 0, 0, 32, 1, 26, height, 0, 27, height);
            wooden_a_supports_paint_setup(7, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21412, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21424, 0, 0, 32, 1, 26, height, 0, 27, height);
            wooden_a_supports_paint_setup(6, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21415, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21427, 0, 0, 32, 1, 26, height, 0, 27, height);
            wooden_a_supports_paint_setup(7, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 4:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21417, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21429, 0, 0, 32, 1, 26, height, 0, 27, height);
            wooden_a_supports_paint_setup(6, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21420, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21432, 0, 0, 32, 1, 26, height, 0, 27, height);
            wooden_a_supports_paint_setup(7, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21411, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21423, 0, 0, 32, 1, 26, height, 0, 27, height);
            wooden_a_supports_paint_setup(6, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21414, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21426, 0, 0, 32, 1, 26, height, 0, 27, height);
            wooden_a_supports_paint_setup(7, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 5:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21416, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21428, 0, 0, 32, 1, 26, height, 0, 27, height);
            wooden_a_supports_paint_setup(6, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21419, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21431, 0, 0, 32, 1, 26, height, 0, 27, height);
            wooden_a_supports_paint_setup(7, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21410, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21422, 0, 0, 32, 1, 26, height, 0, 27, height);
            wooden_a_supports_paint_setup(6, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21413, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21425, 0, 0, 32, 1, 26, height, 0, 27, height);
            wooden_a_supports_paint_setup(7, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    }
}

/** rct2: 0x00876A5C */
static void heartline_twister_rc_track_right_heartline_roll(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21434, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21446, 0, 0, 32, 1, 26, height, 0, 27, height);
            wooden_a_supports_paint_setup(6, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21437, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21449, 0, 0, 32, 1, 26, height, 0, 27, height);
            wooden_a_supports_paint_setup(7, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21440, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21452, 0, 0, 32, 1, 26, height, 0, 27, height);
            wooden_a_supports_paint_setup(6, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21443, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21455, 0, 0, 32, 1, 26, height, 0, 27, height);
            wooden_a_supports_paint_setup(7, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21435, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21447, 0, 0, 32, 1, 26, height, 0, 27, height);
            wooden_a_supports_paint_setup(6, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21438, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21450, 0, 0, 32, 1, 26, height, 0, 27, height);
            wooden_a_supports_paint_setup(7, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21441, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21453, 0, 0, 32, 1, 26, height, 0, 27, height);
            wooden_a_supports_paint_setup(6, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21444, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21456, 0, 0, 32, 1, 26, height, 0, 27, height);
            wooden_a_supports_paint_setup(7, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21436, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21448, 0, 0, 32, 1, 26, height, 0, 27, height);
            wooden_a_supports_paint_setup(6, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21439, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21451, 0, 0, 32, 1, 26, height, 0, 27, height);
            wooden_a_supports_paint_setup(7, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21442, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21454, 0, 0, 32, 1, 26, height, 0, 27, height);
            wooden_a_supports_paint_setup(6, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21445, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21457, 0, 0, 32, 1, 26, height, 0, 27, height);
            wooden_a_supports_paint_setup(7, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21442, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21454, 0, 0, 32, 1, 26, height, 0, 27, height);
            wooden_a_supports_paint_setup(6, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21445, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21457, 0, 0, 32, 1, 26, height, 0, 27, height);
            wooden_a_supports_paint_setup(7, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21436, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21448, 0, 0, 32, 1, 26, height, 0, 27, height);
            wooden_a_supports_paint_setup(6, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21439, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21451, 0, 0, 32, 1, 26, height, 0, 27, height);
            wooden_a_supports_paint_setup(7, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 4:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21441, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21453, 0, 0, 32, 1, 26, height, 0, 27, height);
            wooden_a_supports_paint_setup(6, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21444, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21456, 0, 0, 32, 1, 26, height, 0, 27, height);
            wooden_a_supports_paint_setup(7, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21435, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21447, 0, 0, 32, 1, 26, height, 0, 27, height);
            wooden_a_supports_paint_setup(6, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21438, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21450, 0, 0, 32, 1, 26, height, 0, 27, height);
            wooden_a_supports_paint_setup(7, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 5:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21440, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21452, 0, 0, 32, 1, 26, height, 0, 27, height);
            wooden_a_supports_paint_setup(6, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21443, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21455, 0, 0, 32, 1, 26, height, 0, 27, height);
            wooden_a_supports_paint_setup(7, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21434, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21446, 0, 0, 32, 1, 26, height, 0, 27, height);
            wooden_a_supports_paint_setup(6, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21437, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 21449, 0, 0, 32, 1, 26, height, 0, 27, height);
            wooden_a_supports_paint_setup(7, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    }
}

TRACK_PAINT_FUNCTION get_track_paint_function_heartline_twister_rc(sint32 trackType, sint32 direction)
{
    switch (trackType) {
    case TRACK_ELEM_FLAT:
        return heartline_twister_rc_track_flat;
    case TRACK_ELEM_END_STATION:
    case TRACK_ELEM_BEGIN_STATION:
    case TRACK_ELEM_MIDDLE_STATION:
        return heartline_twister_rc_track_station;
    case TRACK_ELEM_25_DEG_UP:
        return heartline_twister_rc_track_25_deg_up;
    case TRACK_ELEM_60_DEG_UP:
        return heartline_twister_rc_track_60_deg_up;
    case TRACK_ELEM_FLAT_TO_25_DEG_UP:
        return heartline_twister_rc_track_flat_to_25_deg_up;
    case TRACK_ELEM_25_DEG_UP_TO_60_DEG_UP:
        return heartline_twister_rc_track_25_deg_up_to_60_deg_up;
    case TRACK_ELEM_60_DEG_UP_TO_25_DEG_UP:
        return heartline_twister_rc_track_60_deg_up_to_25_deg_up;
    case TRACK_ELEM_25_DEG_UP_TO_FLAT:
        return heartline_twister_rc_track_25_deg_up_to_flat;
    case TRACK_ELEM_25_DEG_DOWN:
        return heartline_twister_rc_track_25_deg_down;
    case TRACK_ELEM_60_DEG_DOWN:
        return heartline_twister_rc_track_60_deg_down;
    case TRACK_ELEM_FLAT_TO_25_DEG_DOWN:
        return heartline_twister_rc_track_flat_to_25_deg_down;
    case TRACK_ELEM_25_DEG_DOWN_TO_60_DEG_DOWN:
        return heartline_twister_rc_track_25_deg_down_to_60_deg_down;
    case TRACK_ELEM_60_DEG_DOWN_TO_25_DEG_DOWN:
        return heartline_twister_rc_track_60_deg_down_to_25_deg_down;
    case TRACK_ELEM_25_DEG_DOWN_TO_FLAT:
        return heartline_twister_rc_track_25_deg_down_to_flat;
    case TRACK_ELEM_HEARTLINE_TRANSFER_UP:
        return heartline_twister_rc_track_heartline_transfer_up;
    case TRACK_ELEM_HEARTLINE_TRANSFER_DOWN:
        return heartline_twister_rc_track_heartline_transfer_down;
    case TRACK_ELEM_LEFT_HEARTLINE_ROLL:
        return heartline_twister_rc_track_left_heartline_roll;
    case TRACK_ELEM_RIGHT_HEARTLINE_ROLL:
        return heartline_twister_rc_track_right_heartline_roll;
    }
    return NULL;
}
