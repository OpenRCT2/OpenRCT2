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

/** rct2: 0x008AE6E0 */
static void compact_inverted_rc_track_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    if (track_element_is_lift_hill(mapElement)) {
        switch (direction) {
        case 0:
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26557, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 29);
            break;
        case 1:
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26558, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 29);
            break;
        }
    } else {
        switch (direction) {
        case 0:
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26555, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 29);
            break;
        case 1:
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26556, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 29);
            break;
        }
    }

    paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    if (track_paint_util_should_paint_supports(gPaintSession.MapPosition)) {
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 38, gTrackColours[SCHEME_SUPPORTS]);
    }

    paint_util_push_tunnel_rotated(direction, height, TUNNEL_3);
    paint_util_set_general_support_height(height + 48, 0x20);
}

/** rct2: 0x008AE950, 0x008AE960, 0x008AE970 */
static void compact_inverted_rc_track_station(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    static const uint32 imageIds[4][3] = {
        { SPR_STATION_BASE_C_SW_NE, 26557, SPR_STATION_INVERTED_BAR_A_SW_NE },
        { SPR_STATION_BASE_C_NW_SE, 26558, SPR_STATION_INVERTED_BAR_A_NW_SE },
        { SPR_STATION_BASE_C_SW_NE, 26557, SPR_STATION_INVERTED_BAR_A_SW_NE },
        { SPR_STATION_BASE_C_NW_SE, 26558, SPR_STATION_INVERTED_BAR_A_NW_SE },
    };

    sub_98197C_rotated(direction, imageIds[direction][0] | gTrackColours[SCHEME_MISC], 0, 0, 32, 28, 1, height, 0, 2, height);
    sub_98197C_rotated(direction, imageIds[direction][1] | gTrackColours[SCHEME_TRACK], 0, 0, 32, 20, 3, height + 29, 0, 6, height + 29);
    sub_98199C_rotated(direction, imageIds[direction][2] | gTrackColours[SCHEME_SUPPORTS], 0, 6, 32, 20, 3, height + 29, 0, 6, height + 29);
    track_paint_util_draw_station_metal_supports_2(direction, height, gTrackColours[SCHEME_SUPPORTS], 11);
    track_paint_util_draw_station_inverted(rideIndex, trackSequence, direction, height, mapElement, STATION_VARIANT_TALL);
    paint_util_push_tunnel_rotated(direction, height, TUNNEL_9);
    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 48, 0x20);
}

/** rct2: 0x008AE6F0 */
static void compact_inverted_rc_track_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    if (track_element_is_lift_hill(mapElement)) {
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26621, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 45);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26622, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 45);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26623, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 45);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26624, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 45);
            break;
        }
    } else {
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26569, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 45);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26570, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 45);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26571, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 45);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26572, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 45);
            break;
        }
    }

    paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    if (track_paint_util_should_paint_supports(gPaintSession.MapPosition)) {
        switch (direction) {
        case 0:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 6, 0, height + 56, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height + 56, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 7, 0, height + 56, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height + 56, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }
    }

    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_4);
    } else {
        paint_util_push_tunnel_rotated(direction, height + 8, TUNNEL_5);
    }
    paint_util_set_general_support_height(height + 72, 0x20);
}

/** rct2: 0x008AE700 */
static void compact_inverted_rc_track_60_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    if (track_element_is_lift_hill(mapElement)) {
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26637, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 93);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26638, 0, 0, 32, 2, 81, height + 29, 0, 4, height + 11);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26639, 0, 0, 32, 2, 81, height + 29, 0, 4, height + 11);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26640, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 93);
            break;
        }
    } else {
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26585, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 93);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26586, 0, 0, 32, 2, 81, height + 29, 0, 4, height + 11);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26587, 0, 0, 32, 2, 81, height + 29, 0, 4, height + 11);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26588, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 93);
            break;
        }
    }
    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_4);
    } else {
        paint_util_push_tunnel_rotated(direction, height + 56, TUNNEL_5);
    }
    paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(height + 120, 0x20);
}

/** rct2: 0x008AE710 */
static void compact_inverted_rc_track_flat_to_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    if (track_element_is_lift_hill(mapElement)) {
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26613, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 37);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26614, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 37);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26615, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 37);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26616, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 37);
            break;
        }
    } else {
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26561, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 37);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26562, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 37);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26563, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 37);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26564, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 37);
            break;
        }
    }

    paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    if (track_paint_util_should_paint_supports(gPaintSession.MapPosition)) {
        switch (direction) {
        case 0:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 6, 0, height + 48, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height + 48, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 7, 0, height + 48, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height + 48, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }
    }

    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(direction, height, TUNNEL_3);
    } else {
        paint_util_push_tunnel_rotated(direction, height, TUNNEL_5);
    }
    paint_util_set_general_support_height(height + 64, 0x20);
}

/** rct2: 0x008AE720 */
static void compact_inverted_rc_track_25_deg_up_to_60_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    if (track_element_is_lift_hill(mapElement)) {
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26625, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 61);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26629, 0, 0, 32, 10, 49, height + 29, 0, 10, height + 11);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26626, 0, 0, 32, 2, 49, height + 29, 0, 4, height + 11);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26630, 0, 0, 32, 10, 49, height + 29, 0, 10, height + 11);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26627, 0, 0, 32, 2, 49, height + 29, 0, 4, height + 11);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26628, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 61);
            break;
        }
    } else {
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26573, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 61);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26577, 0, 0, 32, 10, 49, height + 29, 0, 10, height + 11);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26574, 0, 0, 32, 2, 49, height + 29, 0, 4, height + 11);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26578, 0, 0, 32, 10, 49, height + 29, 0, 10, height + 11);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26575, 0, 0, 32, 2, 49, height + 29, 0, 4, height + 11);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26576, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 61);
            break;
        }
    }
    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_4);
    } else {
        paint_util_push_tunnel_rotated(direction, height + 24, TUNNEL_5);
    }
    paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(height + 88, 0x20);
}

/** rct2: 0x008AE730 */
static void compact_inverted_rc_track_60_deg_up_to_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    if (track_element_is_lift_hill(mapElement)) {
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26631, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 61);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26635, 0, 0, 32, 10, 49, height + 29, 0, 10, height + 11);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26632, 0, 0, 32, 2, 49, height + 29, 0, 4, height + 11);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26636, 0, 0, 32, 10, 49, height + 29, 0, 10, height + 11);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26633, 0, 0, 32, 2, 49, height + 29, 0, 4, height + 11);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26634, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 61);
            break;
        }
    } else {
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26579, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 61);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26583, 0, 0, 32, 10, 49, height + 29, 0, 10, height + 11);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26580, 0, 0, 32, 2, 49, height + 29, 0, 4, height + 11);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26584, 0, 0, 32, 10, 49, height + 29, 0, 10, height + 11);
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26581, 0, 0, 32, 2, 49, height + 29, 0, 4, height + 11);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26582, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 61);
            break;
        }
    }

    paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    if (track_paint_util_should_paint_supports(gPaintSession.MapPosition)) {
        switch (direction) {
        case 0:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 6, 0, height + 70, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height + 70, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 7, 0, height + 70, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height + 70, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }
    }

    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_4);
    } else {
        paint_util_push_tunnel_rotated(direction, height + 24, TUNNEL_5);
    }
    paint_util_set_general_support_height(height + 88, 0x20);
}

/** rct2: 0x008AE740 */
static void compact_inverted_rc_track_25_deg_up_to_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    if (track_element_is_lift_hill(mapElement)) {
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26617, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 37);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26618, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 37);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26619, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 37);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26620, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 37);
            break;
        }
    } else {
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26565, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 37);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26566, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 37);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26567, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 37);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26568, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 37);
            break;
        }
    }

    paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    if (track_paint_util_should_paint_supports(gPaintSession.MapPosition)) {
        switch (direction) {
        case 0:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 6, 0, height + 46, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height + 46, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 7, 0, height + 46, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height + 46, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }
    }

    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_3);
    } else {
        paint_util_push_tunnel_rotated(direction, height + 8, TUNNEL_13);
    }
    paint_util_set_general_support_height(height + 56, 0x20);
}

/** rct2: 0x008AE750 */
static void compact_inverted_rc_track_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    compact_inverted_rc_track_25_deg_up(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008AE760 */
static void compact_inverted_rc_track_60_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    compact_inverted_rc_track_60_deg_up(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008AE770 */
static void compact_inverted_rc_track_flat_to_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    compact_inverted_rc_track_25_deg_up_to_flat(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008AE780 */
static void compact_inverted_rc_track_25_deg_down_to_60_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    compact_inverted_rc_track_60_deg_up_to_25_deg_up(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008AE790 */
static void compact_inverted_rc_track_60_deg_down_to_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    compact_inverted_rc_track_25_deg_up_to_60_deg_up(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008AE7A0 */
static void compact_inverted_rc_track_25_deg_down_to_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    compact_inverted_rc_track_flat_to_25_deg_up(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008AE7B0 */
static void compact_inverted_rc_track_left_quarter_turn_5(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26694, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 29);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26699, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 29);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26704, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 29);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26689, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 29);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 38, gTrackColours[SCHEME_SUPPORTS]);

        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height, TUNNEL_3);
        }
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 1:
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26693, 0, 0, 32, 16, 3, height + 29);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26698, 0, 0, 32, 16, 3, height + 29);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26703, 0, 0, 32, 16, 3, height + 29, 0, 16, height + 29);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26688, 0, 0, 32, 16, 3, height + 29, 0, 16, height + 29);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26692, 0, 0, 16, 16, 3, height + 29, 0, 16, height + 29);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26697, 0, 0, 16, 16, 3, height + 29, 16, 16, height + 29);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26702, 0, 0, 16, 16, 3, height + 29, 16, 0, height + 29);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26687, 0, 0, 16, 16, 3, height + 29);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 4:
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 5:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26691, 0, 0, 16, 32, 3, height + 29, 16, 0, height + 29);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26696, 0, 0, 16, 32, 3, height + 29);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26701, 0, 0, 16, 32, 3, height + 29);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26686, 0, 0, 16, 32, 3, height + 29, 16, 0, height + 29);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 6:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26690, 0, 0, 20, 32, 3, height + 29, 6, 0, height + 29);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26695, 0, 0, 20, 32, 3, height + 29, 6, 0, height + 29);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26700, 0, 0, 20, 32, 3, height + 29, 6, 0, height + 29);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26685, 0, 0, 20, 32, 3, height + 29, 6, 0, height + 29);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 38, gTrackColours[SCHEME_SUPPORTS]);

        switch (direction) {
        case 2:
            paint_util_push_tunnel_right(height, TUNNEL_3);
            break;
        case 3:
            paint_util_push_tunnel_left(height, TUNNEL_3);
            break;
        }
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    }
}

/** rct2: 0x008AE7C0 */
static void compact_inverted_rc_track_right_quarter_turn_5(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    compact_inverted_rc_track_left_quarter_turn_5(rideIndex, trackSequence, (direction - 1) & 3, height, mapElement);
}

/** rct2: 0x008AE7D0 */
static void compact_inverted_rc_track_flat_to_left_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26657, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 29);
        break;
    case 1:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26658, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 29);
        break;
    case 2:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26659, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 29);
        break;
    case 3:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26660, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 29);
        break;
    }

    paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
    if (track_paint_util_should_paint_supports(gPaintSession.MapPosition)) {
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 38, gTrackColours[SCHEME_SUPPORTS]);
    }

    paint_util_push_tunnel_rotated(direction, height, TUNNEL_3);
    paint_util_set_general_support_height(height + 48, 0x20);
}

/** rct2: 0x008AE7E0 */
static void compact_inverted_rc_track_flat_to_right_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26661, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 29);
        break;
    case 1:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26662, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 29);
        break;
    case 2:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26663, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 29);
        break;
    case 3:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26664, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 29);
        break;
    }

    paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    if (track_paint_util_should_paint_supports(gPaintSession.MapPosition)) {
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 38, gTrackColours[SCHEME_SUPPORTS]);
    }

    paint_util_push_tunnel_rotated(direction, height, TUNNEL_3);
    paint_util_set_general_support_height(height + 48, 0x20);
}

/** rct2: 0x008AE7F0 */
static void compact_inverted_rc_track_left_bank_to_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26663, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 29);
        break;
    case 1:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26664, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 29);
        break;
    case 2:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26661, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 29);
        break;
    case 3:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26662, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 29);
        break;
    }

    paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
    if (track_paint_util_should_paint_supports(gPaintSession.MapPosition)) {
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 38, gTrackColours[SCHEME_SUPPORTS]);
    }

    paint_util_push_tunnel_rotated(direction, height, TUNNEL_3);
    paint_util_set_general_support_height(height + 48, 0x20);
}

/** rct2: 0x008AE800 */
static void compact_inverted_rc_track_right_bank_to_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26659, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 29);
        break;
    case 1:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26660, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 29);
        break;
    case 2:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26657, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 29);
        break;
    case 3:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26658, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 29);
        break;
    }

    paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    if (track_paint_util_should_paint_supports(gPaintSession.MapPosition)) {
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 38, gTrackColours[SCHEME_SUPPORTS]);
    }

    paint_util_push_tunnel_rotated(direction, height, TUNNEL_3);
    paint_util_set_general_support_height(height + 48, 0x20);
}

/** rct2: 0x008AE810 */
static void compact_inverted_rc_track_banked_left_quarter_turn_5(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26738, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 29);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26743, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 29);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26748, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 29);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26733, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 29);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 38, gTrackColours[SCHEME_SUPPORTS]);

        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height, TUNNEL_3);
        }
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 1:
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26737, 0, 0, 32, 16, 3, height + 29);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26742, 0, 0, 32, 16, 3, height + 29);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26747, 0, 0, 32, 16, 3, height + 29, 0, 16, height + 29);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26732, 0, 0, 32, 16, 3, height + 29, 0, 16, height + 29);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26736, 0, 0, 16, 16, 3, height + 29, 0, 16, height + 29);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26741, 0, 0, 16, 16, 3, height + 29, 16, 16, height + 29);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26746, 0, 0, 16, 16, 3, height + 29, 16, 0, height + 29);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26731, 0, 0, 16, 16, 3, height + 29);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 4:
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 5:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26735, 0, 0, 16, 32, 3, height + 29, 16, 0, height + 29);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26740, 0, 0, 16, 32, 3, height + 29);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26745, 0, 0, 16, 32, 3, height + 29);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26730, 0, 0, 16, 32, 3, height + 29, 16, 0, height + 29);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 6:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26734, 0, 0, 20, 32, 3, height + 29, 6, 0, height + 29);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26739, 0, 0, 20, 32, 3, height + 29, 6, 0, height + 29);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26744, 0, 0, 20, 32, 3, height + 29, 6, 0, height + 29);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26729, 0, 0, 20, 32, 3, height + 29, 6, 0, height + 29);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 38, gTrackColours[SCHEME_SUPPORTS]);

        switch (direction) {
        case 2:
            paint_util_push_tunnel_right(height, TUNNEL_3);
            break;
        case 3:
            paint_util_push_tunnel_left(height, TUNNEL_3);
            break;
        }
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    }
}

/** rct2: 0x008AE820 */
static void compact_inverted_rc_track_banked_right_quarter_turn_5(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    compact_inverted_rc_track_banked_left_quarter_turn_5(rideIndex, trackSequence, (direction - 1) & 3, height, mapElement);
}

/** rct2: 0x008AE830 */
static void compact_inverted_rc_track_left_bank_to_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26665, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 37);
        break;
    case 1:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26666, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 37);
        break;
    case 2:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26667, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 37);
        break;
    case 3:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26668, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 37);
        break;
    }

    paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
    if (track_paint_util_should_paint_supports(gPaintSession.MapPosition)) {
        switch (direction) {
        case 0:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 6, 0, height + 46, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height + 46, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 7, 0, height + 46, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height + 46, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }
    }

    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(direction, height, TUNNEL_3);
    } else {
        paint_util_push_tunnel_rotated(direction, height, TUNNEL_5);
    }
    paint_util_set_general_support_height(height + 64, 0x20);
}

/** rct2: 0x008AE840 */
static void compact_inverted_rc_track_right_bank_to_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26669, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 37);
        break;
    case 1:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26670, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 37);
        break;
    case 2:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26671, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 37);
        break;
    case 3:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26672, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 37);
        break;
    }

    paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    if (track_paint_util_should_paint_supports(gPaintSession.MapPosition)) {
        switch (direction) {
        case 0:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 6, 0, height + 46, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height + 46, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 7, 0, height + 46, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height + 46, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }
    }

    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(direction, height, TUNNEL_3);
    } else {
        paint_util_push_tunnel_rotated(direction, height, TUNNEL_5);
    }
    paint_util_set_general_support_height(height + 64, 0x20);
}

/** rct2: 0x008AE850 */
static void compact_inverted_rc_track_25_deg_up_to_left_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26673, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 37);
        break;
    case 1:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26674, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 37);
        break;
    case 2:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26675, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 37);
        break;
    case 3:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26676, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 37);
        break;
    }

    paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
    if (track_paint_util_should_paint_supports(gPaintSession.MapPosition)) {
        switch (direction) {
        case 0:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 6, 0, height + 46, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height + 46, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 7, 0, height + 46, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height + 46, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }
    }

    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_3);
    } else {
        paint_util_push_tunnel_rotated(direction, height + 8, TUNNEL_13);
    }
    paint_util_set_general_support_height(height + 56, 0x20);
}

/** rct2: 0x008AE860 */
static void compact_inverted_rc_track_25_deg_up_to_right_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26677, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 37);
        break;
    case 1:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26678, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 37);
        break;
    case 2:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26679, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 37);
        break;
    case 3:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26680, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 37);
        break;
    }

    paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    if (track_paint_util_should_paint_supports(gPaintSession.MapPosition)) {
        switch (direction) {
        case 0:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 6, 0, height + 46, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height + 46, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 7, 0, height + 46, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height + 46, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }
    }

    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_3);
    } else {
        paint_util_push_tunnel_rotated(direction, height + 8, TUNNEL_13);
    }
    paint_util_set_general_support_height(height + 56, 0x20);
}

/** rct2: 0x008AE870 */
static void compact_inverted_rc_track_left_bank_to_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    compact_inverted_rc_track_25_deg_up_to_right_bank(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008AE880 */
static void compact_inverted_rc_track_right_bank_to_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    compact_inverted_rc_track_25_deg_up_to_left_bank(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008AE890 */
static void compact_inverted_rc_track_25_deg_down_to_left_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    compact_inverted_rc_track_right_bank_to_25_deg_up(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008AE8A0 */
static void compact_inverted_rc_track_25_deg_down_to_right_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    compact_inverted_rc_track_left_bank_to_25_deg_up(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008AE8B0 */
static void compact_inverted_rc_track_left_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26681, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 29);
        break;
    case 1:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26682, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 29);
        break;
    case 2:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26683, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 29);
        break;
    case 3:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26684, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 29);
        break;
    }

    paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
    if (track_paint_util_should_paint_supports(gPaintSession.MapPosition)) {
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 38, gTrackColours[SCHEME_SUPPORTS]);
    }

    paint_util_push_tunnel_rotated(direction, height, TUNNEL_3);
    paint_util_set_general_support_height(height + 48, 0x20);
}

/** rct2: 0x008AE8C0 */
static void compact_inverted_rc_track_right_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    compact_inverted_rc_track_left_bank(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008AE8D0 */
static void compact_inverted_rc_track_left_quarter_turn_5_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26949, 0, 6, 32, 20, 3, height + 29);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26954, 0, 6, 32, 20, 3, height + 29);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26959, 0, 6, 32, 20, 3, height + 29);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26964, 0, 6, 32, 20, 3, height + 29);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 48, gTrackColours[SCHEME_SUPPORTS]);

        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_4);
        }
        paint_util_set_general_support_height(height + 88, 0x20);
        break;
    case 1:
        paint_util_set_general_support_height(height + 88, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26950, 0, 0, 32, 16, 3, height + 29);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26955, 0, 0, 32, 16, 3, height + 29);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26960, 0, 16, 32, 16, 3, height + 29);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26965, 0, 16, 32, 16, 3, height + 29);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 88, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26951, 0, 16, 16, 16, 3, height + 29);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26956, 16, 16, 16, 16, 3, height + 29);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26961, 16, 0, 16, 16, 3, height + 29);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26966, 0, 0, 16, 16, 3, height + 29);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 80, 0x20);
        break;
    case 4:
        paint_util_set_general_support_height(height + 88, 0x20);
        break;
    case 5:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26952, 16, 0, 16, 32, 3, height + 29);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26957, 0, 0, 16, 32, 3, height + 29);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26962, 0, 0, 16, 32, 3, height + 29);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26967, 16, 0, 16, 32, 3, height + 29);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 88, 0x20);
        break;
    case 6:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26953, 6, 0, 20, 32, 3, height + 29);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26958, 6, 0, 20, 32, 3, height + 29);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26963, 6, 0, 20, 32, 3, height + 29);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26968, 6, 0, 20, 32, 3, height + 29);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 48, gTrackColours[SCHEME_SUPPORTS]);

        switch (direction) {
        case 2:
            paint_util_push_tunnel_right(height + 8, TUNNEL_5);
            break;
        case 3:
            paint_util_push_tunnel_left(height + 8, TUNNEL_5);
            break;
        }
        paint_util_set_general_support_height(height + 88, 0x20);
        break;
    }
}

/** rct2: 0x008AE8E0 */
static void compact_inverted_rc_track_right_quarter_turn_5_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26929, 0, 6, 32, 20, 3, height + 29);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26934, 0, 6, 32, 20, 3, height + 29);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26939, 0, 6, 32, 20, 3, height + 29);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26944, 0, 6, 32, 20, 3, height + 29);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 48, gTrackColours[SCHEME_SUPPORTS]);

        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_4);
        }
        paint_util_set_general_support_height(height + 88, 0x20);
        break;
    case 1:
        paint_util_set_general_support_height(height + 88, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26930, 0, 16, 32, 16, 3, height + 29);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26935, 0, 16, 32, 16, 3, height + 29);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26940, 0, 0, 32, 16, 3, height + 29);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26945, 0, 0, 32, 16, 3, height + 29);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 88, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26931, 0, 0, 16, 16, 3, height + 29);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26936, 16, 0, 16, 16, 3, height + 29);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26941, 16, 16, 16, 16, 3, height + 29);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26946, 0, 16, 16, 16, 3, height + 29);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 80, 0x20);
        break;
    case 4:
        paint_util_set_general_support_height(height + 88, 0x20);
        break;
    case 5:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26932, 16, 0, 16, 32, 3, height + 29);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26937, 0, 0, 16, 32, 3, height + 29);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26942, 0, 0, 16, 32, 3, height + 29);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26947, 16, 0, 16, 32, 3, height + 29);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 88, 0x20);
        break;
    case 6:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26933, 6, 0, 20, 32, 3, height + 29);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26938, 6, 0, 20, 32, 3, height + 29);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26943, 6, 0, 20, 32, 3, height + 29);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26948, 6, 0, 20, 32, 3, height + 29);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 48, gTrackColours[SCHEME_SUPPORTS]);

        switch (direction) {
        case 0:
            paint_util_push_tunnel_right(height + 8, TUNNEL_5);
            break;
        case 1:
            paint_util_push_tunnel_left(height + 8, TUNNEL_5);
            break;
        }
        paint_util_set_general_support_height(height + 88, 0x20);
        break;
    }
}

/** rct2: 0x008AE8F0 */
static void compact_inverted_rc_track_left_quarter_turn_5_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    compact_inverted_rc_track_right_quarter_turn_5_25_deg_up(rideIndex, trackSequence, (direction + 1) & 3, height, mapElement);
}

/** rct2: 0x008AE900 */
static void compact_inverted_rc_track_right_quarter_turn_5_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    compact_inverted_rc_track_left_quarter_turn_5_25_deg_up(rideIndex, trackSequence, (direction - 1) & 3, height, mapElement);
}

/** rct2: 0x008AE910 */
static void compact_inverted_rc_track_s_bend_left(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26641, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 29);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26645, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 29);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26644, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 29);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26648, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 29);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 38, gTrackColours[SCHEME_SUPPORTS]);

        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height, TUNNEL_3);
        }
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26642, 0, 0, 32, 26, 3, height + 29);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26646, 0, 0, 32, 26, 3, height + 29);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26643, 0, 0, 32, 26, 3, height + 29, 0, 6, height + 29);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26647, 0, 0, 32, 26, 3, height + 29, 0, 6, height + 29);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        switch (direction) {
        case 0:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height + 38, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 6, 0, height + 38, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }

        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26643, 0, 0, 32, 26, 3, height + 29, 0, 6, height + 29);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26647, 0, 0, 32, 26, 3, height + 29, 0, 6, height + 29);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26642, 0, 0, 32, 26, 3, height + 29);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26646, 0, 0, 32, 26, 3, height + 29);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        switch (direction) {
        case 2:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height + 38, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 6, 0, height + 38, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }

        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26644, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 29);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26648, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 29);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26641, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 29);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26645, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 29);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 38, gTrackColours[SCHEME_SUPPORTS]);

        switch (direction) {
        case 1:
            paint_util_push_tunnel_right(height, TUNNEL_3);
            break;
        case 2:
            paint_util_push_tunnel_left(height, TUNNEL_3);
            break;
        }
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    }
}

/** rct2: 0x008AE920 */
static void compact_inverted_rc_track_s_bend_right(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26649, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 29);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26653, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 29);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26652, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 29);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26656, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 29);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 38, gTrackColours[SCHEME_SUPPORTS]);

        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height, TUNNEL_3);
        }
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26650, 0, 0, 32, 26, 3, height + 29, 0, 6, height + 29);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26654, 0, 0, 32, 26, 3, height + 29, 0, 6, height + 29);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26651, 0, 0, 32, 26, 3, height + 29);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26655, 0, 0, 32, 26, 3, height + 29);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        switch (direction) {
        case 0:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height + 38, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 7, 0, height + 38, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }

        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26651, 0, 0, 32, 26, 3, height + 29);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26655, 0, 0, 32, 26, 3, height + 29);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26650, 0, 0, 32, 26, 3, height + 29, 0, 6, height + 29);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26654, 0, 0, 32, 26, 3, height + 29, 0, 6, height + 29);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        switch (direction) {
        case 2:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height + 38, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 7, 0, height + 38, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }

        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26652, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 29);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26656, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 29);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26649, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 29);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26653, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 29);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 38, gTrackColours[SCHEME_SUPPORTS]);

        switch (direction) {
        case 1:
            paint_util_push_tunnel_right(height, TUNNEL_3);
            break;
        case 2:
            paint_util_push_tunnel_left(height, TUNNEL_3);
            break;
        }
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    }
}

/** rct2: 0x008AE930 */
static void compact_inverted_rc_track_left_vertical_loop(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27017, 0, 6, 32, 2, 3, height + 29, 0, 29, height + 29);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27025, 0, 6, 32, 2, 3, height + 29, 0, 29, height + 29);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27024, 0, 6, 32, 2, 3, height + 29, 0, 29, height + 29);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27032, 0, 6, 32, 10, 7, height + 29, 0, 6, height + 45);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        switch (direction) {
        case 0:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 6, 0, height + 49, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height + 49, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 7, 0, height + 49, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height + 49, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }

        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_4);
        }
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27018, 0, 0, 32, 2, 3, height + 29, 0, 29, height + 29);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27026, 0, 14, 32, 2, 63, height + 29, 0, 29, height + 29);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27023, 0, 6, 32, 2, 3, height + 29, 0, 0, height + 29);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27031, 0, 6, 2, 2, 3, height + 29, 29, 20, height + 29);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 88, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27019, 16, 0, 5, 2, 119, height + 2, 16, 29, height + 2);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27027, 12, 0, 32, 2, 119, height + 2, 0, -6, height + 2);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27022, 10, 16, 4, 2, 119, height + 2, 10, 0, height + 2);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27030, 16, 16, 2, 2, 119, height + 2, 29, 29, height + 2);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 168, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27020, 0, 0, 32, 2, 3, height + 34, 0, 0, height - 6);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27028, 0, 0, 2, 2, 3, height + 34, 0, 0, height - 6);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27021, 0, 16, 32, 2, 3, height + 34, 0, 0, height - 6);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27029, 0, 16, 2, 2, 3, height + 34, 0, 0, height - 6);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 64, 0x20);
        break;
    case 4:
        paint_util_set_general_support_height(height + 64, 0x20);
        break;
    case 5:
        paint_util_set_general_support_height(height + 64, 0x20);
        break;
    case 6:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27021, 0, 16, 32, 2, 3, height + 34, 0, 0, height - 6);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27029, 0, 16, 2, 2, 3, height + 34, 0, 0, height - 6);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27020, 0, 0, 32, 2, 3, height + 34, 0, 0, height - 6);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27028, 0, 0, 2, 2, 3, height + 34, 0, 0, height - 6);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 64, 0x20);
        break;
    case 7:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27022, 10, 16, 4, 2, 119, height + 2, 10, 0, height + 2);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27030, 16, 16, 2, 2, 119, height + 2, 29, 29, height + 2);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27019, 16, 0, 5, 2, 119, height + 2, 16, 29, height + 2);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27027, 12, 0, 32, 2, 119, height + 2, 0, -6, height + 2);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 168, 0x20);
        break;
    case 8:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27023, 0, 6, 32, 2, 3, height + 29, 0, 0, height + 29);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27031, 0, 6, 2, 2, 3, height + 29, 29, 20, height + 29);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27018, 0, 0, 32, 2, 3, height + 29, 0, 29, height + 29);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27026, 0, 14, 32, 2, 63, height + 29, 0, 29, height + 29);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 88, 0x20);
        break;
    case 9:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27024, 0, 6, 32, 2, 3, height + 29, 0, 29, height + 29);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27032, 0, 6, 32, 10, 7, height + 29, 0, 6, height + 45);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27017, 0, 6, 32, 2, 3, height + 29, 0, 29, height + 29);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27025, 0, 6, 32, 2, 3, height + 29, 0, 29, height + 29);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        switch (direction) {
        case 0:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 7, 0, height + 49, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height + 49, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 6, 0, height + 49, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height + 49, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }

        switch (direction) {
        case 1:
            paint_util_push_tunnel_right(height - 8, TUNNEL_4);
            break;
        case 2:
            paint_util_push_tunnel_left(height - 8, TUNNEL_4);
            break;
        }
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    }
}

/** rct2: 0x008AE940 */
static void compact_inverted_rc_track_right_vertical_loop(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27048, 0, 6, 32, 10, 7, height + 29, 0, 6, height + 45);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27040, 0, 6, 32, 2, 3, height + 29, 0, 29, height + 29);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27041, 0, 6, 32, 2, 3, height + 29, 0, 29, height + 29);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27033, 0, 6, 32, 2, 3, height + 29, 0, 29, height + 29);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        switch (direction) {
        case 0:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 6, 0, height + 49, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height + 49, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 7, 0, height + 49, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height + 49, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }

        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_4);
        }
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27047, 0, 6, 2, 2, 3, height + 29, 29, 20, height + 29);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27039, 0, 6, 32, 2, 3, height + 29, 0, 0, height + 29);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27042, 0, 14, 32, 2, 63, height + 29, 0, 29, height + 29);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27034, 0, 0, 32, 2, 3, height + 29, 0, 29, height + 29);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 88, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27046, 16, 16, 2, 2, 119, height + 2, 29, 29, height + 2);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27038, 10, 16, 4, 2, 119, height + 2, 10, 0, height + 2);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27043, 12, 0, 32, 2, 119, height + 2, 0, -6, height + 2);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27035, 16, 0, 5, 2, 119, height + 2, 16, 29, height + 2);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 168, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27045, 0, 16, 2, 2, 3, height + 34, 0, 0, height - 6);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27037, 0, 16, 32, 2, 3, height + 34, 0, 0, height - 6);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27044, 0, 0, 2, 2, 3, height + 34, 0, 0, height - 6);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27036, 0, 0, 32, 2, 3, height + 34, 0, 0, height - 6);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 64, 0x20);
        break;
    case 4:
        paint_util_set_general_support_height(height + 64, 0x20);
        break;
    case 5:
        paint_util_set_general_support_height(height + 64, 0x20);
        break;
    case 6:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27044, 0, 0, 2, 2, 3, height + 34, 0, 0, height - 6);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27036, 0, 0, 32, 2, 3, height + 34, 0, 0, height - 6);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27045, 0, 16, 2, 2, 3, height + 34, 0, 0, height - 6);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27037, 0, 16, 32, 2, 3, height + 34, 0, 0, height - 6);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 64, 0x20);
        break;
    case 7:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27043, 12, 0, 32, 2, 119, height + 2, 0, -6, height + 2);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27035, 16, 0, 5, 2, 119, height + 2, 16, 29, height + 2);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27046, 16, 16, 2, 2, 119, height + 2, 29, 29, height + 2);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27038, 10, 16, 4, 2, 119, height + 2, 10, 0, height + 2);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 168, 0x20);
        break;
    case 8:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27042, 0, 14, 32, 2, 63, height + 29, 0, 29, height + 29);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27034, 0, 0, 32, 2, 3, height + 29, 0, 29, height + 29);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27047, 0, 6, 2, 2, 3, height + 29, 29, 20, height + 29);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27039, 0, 6, 32, 2, 3, height + 29, 0, 0, height + 29);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 88, 0x20);
        break;
    case 9:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27041, 0, 6, 32, 2, 3, height + 29, 0, 29, height + 29);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27033, 0, 6, 32, 2, 3, height + 29, 0, 29, height + 29);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27048, 0, 6, 32, 10, 7, height + 29, 0, 6, height + 45);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27040, 0, 6, 32, 2, 3, height + 29, 0, 29, height + 29);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_BC | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        switch (direction) {
        case 0:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 7, 0, height + 49, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height + 49, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 6, 0, height + 49, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height + 49, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }

        switch (direction) {
        case 1:
            paint_util_push_tunnel_right(height - 8, TUNNEL_4);
            break;
        case 2:
            paint_util_push_tunnel_left(height - 8, TUNNEL_4);
            break;
        }
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    }
}

/** rct2: 0x008AE980 */
static void compact_inverted_rc_track_left_quarter_turn_3(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26710, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 29);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26713, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 29);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26716, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 29);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26707, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 29);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 38, gTrackColours[SCHEME_SUPPORTS]);

        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height, TUNNEL_3);
        }
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 1:
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26709, 0, 0, 16, 16, 3, height + 29, 16, 0, height + 29);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26712, 0, 0, 16, 16, 3, height + 29);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26715, 0, 0, 16, 16, 3, height + 29, 0, 16, height + 29);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26706, 0, 0, 16, 16, 3, height + 29, 16, 16, height + 29);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26708, 0, 0, 20, 32, 3, height + 29, 6, 0, height + 29);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26711, 0, 0, 20, 32, 3, height + 29, 6, 0, height + 29);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26714, 0, 0, 20, 32, 3, height + 29, 6, 0, height + 29);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26705, 0, 0, 20, 32, 3, height + 29, 6, 0, height + 29);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 38, gTrackColours[SCHEME_SUPPORTS]);

        switch (direction) {
        case 2:
            paint_util_push_tunnel_right(height, TUNNEL_3);
            break;
        case 3:
            paint_util_push_tunnel_left(height, TUNNEL_3);
            break;
        }
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    }
}

/** rct2: 0x008AE990 */
static void compact_inverted_rc_track_right_quarter_turn_3(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    compact_inverted_rc_track_left_quarter_turn_3(rideIndex, trackSequence, (direction - 1) & 3, height, mapElement);
}

/** rct2: 0x008AE9A0 */
static void compact_inverted_rc_track_left_quarter_turn_3_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26722, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 29);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26725, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 29);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26728, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 29);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26719, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 29);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 38, gTrackColours[SCHEME_SUPPORTS]);

        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height, TUNNEL_3);
        }
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 1:
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26721, 0, 0, 16, 16, 3, height + 29, 16, 0, height + 29);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26724, 0, 0, 16, 16, 3, height + 29, 0, 0, height + 29);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26727, 0, 0, 16, 16, 3, height + 29, 0, 16, height + 29);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26718, 0, 0, 16, 16, 3, height + 29, 16, 16, height + 29);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26720, 0, 0, 20, 32, 3, height + 29, 6, 0, height + 29);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26723, 0, 0, 20, 32, 3, height + 29, 6, 0, height + 29);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26726, 0, 0, 20, 32, 3, height + 29, 6, 0, height + 29);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26717, 0, 0, 20, 32, 3, height + 29, 6, 0, height + 29);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 38, gTrackColours[SCHEME_SUPPORTS]);

        switch (direction) {
        case 2:
            paint_util_push_tunnel_right(height, TUNNEL_3);
            break;
        case 3:
            paint_util_push_tunnel_left(height, TUNNEL_3);
            break;
        }
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    }
}

/** rct2: 0x008AE9B0 */
static void compact_inverted_rc_track_right_quarter_turn_3_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    compact_inverted_rc_track_left_quarter_turn_3_bank(rideIndex, trackSequence, (direction - 1) & 3, height, mapElement);
}

/** rct2: 0x008AE9C0 */
static void compact_inverted_rc_track_left_quarter_turn_3_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26924, 0, 6, 32, 20, 3, height + 29);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26926, 0, 6, 32, 20, 3, height + 29);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26928, 0, 6, 32, 20, 3, height + 29);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26922, 0, 6, 32, 20, 3, height + 29);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 46, gTrackColours[SCHEME_SUPPORTS]);

        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_4);
        }
        paint_util_set_general_support_height(height + 88, 0x20);
        break;
    case 1:
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 2:
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26923, 6, 0, 20, 32, 3, height + 29);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26925, 6, 0, 20, 32, 3, height + 29);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26927, 6, 0, 20, 32, 3, height + 29);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26921, 6, 0, 20, 32, 3, height + 29);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 46, gTrackColours[SCHEME_SUPPORTS]);

        switch (direction) {
        case 2:
            paint_util_push_tunnel_right(height + 8, TUNNEL_5);
            break;
        case 3:
            paint_util_push_tunnel_left(height + 8, TUNNEL_5);
            break;
        }
        paint_util_set_general_support_height(height + 88, 0x20);
        break;
    }
}

/** rct2: 0x008AE9D0 */
static void compact_inverted_rc_track_right_quarter_turn_3_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26913, 0, 6, 32, 20, 3, height + 29);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26915, 0, 6, 32, 20, 3, height + 29);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26917, 0, 6, 32, 20, 3, height + 29);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26919, 0, 6, 32, 20, 3, height + 29);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 46, gTrackColours[SCHEME_SUPPORTS]);

        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_4);
        }
        paint_util_set_general_support_height(height + 88, 0x20);
        break;
    case 1:
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 2:
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26914, 6, 0, 20, 32, 3, height + 29);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26916, 6, 0, 20, 32, 3, height + 29);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26918, 6, 0, 20, 32, 3, height + 29);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26920, 6, 0, 20, 32, 3, height + 29);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 46, gTrackColours[SCHEME_SUPPORTS]);

        switch (direction) {
        case 0:
            paint_util_push_tunnel_right(height + 8, TUNNEL_5);
            break;
        case 1:
            paint_util_push_tunnel_left(height + 8, TUNNEL_5);
            break;
        }
        paint_util_set_general_support_height(height + 88, 0x20);
        break;
    }
}

/** rct2: 0x008AE9E0 */
static void compact_inverted_rc_track_left_quarter_turn_3_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    compact_inverted_rc_track_right_quarter_turn_3_25_deg_up(rideIndex, trackSequence, (direction + 1) & 3, height, mapElement);
}

/** rct2: 0x008AE9F0 */
static void compact_inverted_rc_track_right_quarter_turn_3_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    compact_inverted_rc_track_left_quarter_turn_3_25_deg_up(rideIndex, trackSequence, (direction - 1) & 3, height, mapElement);
}

/** rct2: 0x008AEA00 */
static void compact_inverted_rc_track_left_twist_down_to_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26993, 0, 6, 32, 20, 3, height + 29);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26999, 0, 6, 32, 20, 3, height + 29);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26998, 0, 6, 32, 20, 3, height + 29);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27004, 0, 6, 32, 20, 3, height + 29);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 38, gTrackColours[SCHEME_SUPPORTS]);

        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height, TUNNEL_3);
        }
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26994, 0, 6, 32, 20, 3, height + 29);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27000, 0, 6, 32, 20, 3, height + 29);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26997, 0, 6, 32, 20, 3, height + 29);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27003, 0, 6, 32, 20, 3, height + 29);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26995, 0, 6, 32, 20, 3, height);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27001, 0, 6, 32, 20, 3, height);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26996, 0, 6, 32, 20, 3, height);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27002, 0, 6, 32, 20, 3, height);
            break;
        }
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
        switch (direction) {
        case 1:
            paint_util_push_tunnel_right(height, TUNNEL_3);
            break;
        case 2:
            paint_util_push_tunnel_left(height, TUNNEL_3);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    }
}

/** rct2: 0x008AEA10 */
static void compact_inverted_rc_track_right_twist_down_to_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27005, 0, 6, 32, 20, 3, height + 29);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27011, 0, 6, 32, 20, 3, height + 29);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27010, 0, 6, 32, 20, 3, height + 29);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27016, 0, 6, 32, 20, 3, height + 29);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 38, gTrackColours[SCHEME_SUPPORTS]);

        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height, TUNNEL_3);
        }
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27006, 0, 6, 32, 20, 3, height + 29);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27012, 0, 6, 32, 20, 3, height + 29);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27009, 0, 6, 32, 20, 3, height + 29);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27015, 0, 6, 32, 20, 3, height + 29);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27007, 0, 6, 32, 20, 3, height);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27013, 0, 6, 32, 20, 3, height);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27008, 0, 6, 32, 20, 3, height);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27014, 0, 6, 32, 20, 3, height);
            break;
        }
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
        switch (direction) {
        case 1:
            paint_util_push_tunnel_right(height, TUNNEL_3);
            break;
        case 2:
            paint_util_push_tunnel_left(height, TUNNEL_3);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    }
}

/** rct2: 0x008AEA20 */
static void compact_inverted_rc_track_left_twist_up_to_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26996, 0, 6, 32, 20, 3, height);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27002, 0, 6, 32, 20, 3, height);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26995, 0, 6, 32, 20, 3, height);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27001, 0, 6, 32, 20, 3, height);
            break;
        }
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height, TUNNEL_3);
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26997, 0, 6, 32, 20, 3, height + 29);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27003, 0, 6, 32, 20, 3, height + 29);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26994, 0, 6, 32, 20, 3, height + 29);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27000, 0, 6, 32, 20, 3, height + 29);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26998, 0, 6, 32, 20, 3, height + 29);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27004, 0, 6, 32, 20, 3, height + 29);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26993, 0, 6, 32, 20, 3, height + 29);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26999, 0, 6, 32, 20, 3, height + 29);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 38, gTrackColours[SCHEME_SUPPORTS]);

        switch (direction) {
        case 1:
            paint_util_push_tunnel_right(height, TUNNEL_3);
            break;
        case 2:
            paint_util_push_tunnel_left(height, TUNNEL_3);
            break;
        }
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    }
}

/** rct2: 0x008AEA30 */
static void compact_inverted_rc_track_right_twist_up_to_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27008, 0, 6, 32, 20, 3, height);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27014, 0, 6, 32, 20, 3, height);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27007, 0, 6, 32, 20, 3, height);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27013, 0, 6, 32, 20, 3, height);
            break;
        }
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height, TUNNEL_3);
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27009, 0, 6, 32, 20, 3, height + 29);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27015, 0, 6, 32, 20, 3, height + 29);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27006, 0, 6, 32, 20, 3, height + 29);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27012, 0, 6, 32, 20, 3, height + 29);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27010, 0, 6, 32, 20, 3, height + 29);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27016, 0, 6, 32, 20, 3, height + 29);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27005, 0, 6, 32, 20, 3, height + 29);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27011, 0, 6, 32, 20, 3, height + 29);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 38, gTrackColours[SCHEME_SUPPORTS]);

        switch (direction) {
        case 1:
            paint_util_push_tunnel_right(height, TUNNEL_3);
            break;
        case 2:
            paint_util_push_tunnel_left(height, TUNNEL_3);
            break;
        }
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    }
}

/** rct2: 0x008AEA40 */
static void compact_inverted_rc_track_half_loop_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27073, 0, 6, 32, 20, 3, height + 29, 0, 6, height + 45);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27081, 0, 6, 32, 20, 3, height + 29, 0, 6, height + 45);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27080, 0, 6, 32, 20, 3, height + 29, 0, 6, height + 45);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27088, 0, 6, 32, 20, 3, height + 29, 0, 6, height + 45);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        switch (direction) {
        case 0:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 6, 0, height + 56, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height + 56, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 7, 0, height + 56, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height + 56, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }

        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_4);
        }
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27074, 0, 0, 32, 2, 3, height + 29, 0, 29, height + 29);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27082, 0, 14, 32, 2, 63, height + 29, 0, 29, height + 29);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27079, 0, 6, 32, 2, 3, height + 29, 0, 0, height + 29);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27087, 0, 6, 2, 2, 3, height + 29, 29, 20, height + 29);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 88, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27075, 16, 0, 5, 2, 119, height + 2, 16, 29, height + 2);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27083, 12, 0, 32, 2, 119, height + 2, 0, -6, height + 2);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27078, 10, 16, 4, 2, 119, height + 2, 10, 0, height + 2);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27086, 16, 16, 2, 2, 119, height + 2, 29, 29, height + 2);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 168, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27076, 0, 0, 32, 20, 3, height + 34, 0, 6, height + 34);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27084, 0, 0, 2, 2, 3, height + 34, 0, 0, height - 6);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27077, 0, 16, 32, 2, 3, height + 34, 0, 0, height - 6);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27085, 0, 16, 32, 20, 3, height + 34, 0, 6, height + 34);
            break;
        }
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height + 32, TUNNEL_3);
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 64, 0x20);
        break;
    }
}

/** rct2: 0x008AEA50 */
static void compact_inverted_rc_track_half_loop_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    compact_inverted_rc_track_half_loop_up(rideIndex, 3 - trackSequence, direction, height, mapElement);
}

/** rct2: 0x008AEA60 */
static void compact_inverted_rc_track_left_corkscrew_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27049, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 33);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27052, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 33);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27055, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 33);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27058, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 33);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 44, gTrackColours[SCHEME_SUPPORTS]);

        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height, TUNNEL_3);
        }
        paint_util_set_general_support_height(height + 64, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27050, 0, 0, 20, 20, 3, height + 14, 6, 6, height + 24);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27053, 0, 0, 20, 20, 3, height + 14, 6, 6, height + 24);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27056, 0, 0, 20, 20, 3, height + 14, 6, 6, height + 24);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27059, 0, 0, 20, 20, 3, height + 14, 6, 6, height + 24);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 88, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27051, 0, 0, 20, 32, 3, height + 29, 6, 0, height + 39);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27054, 0, 0, 20, 32, 3, height + 29, 6, 0, height + 39);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27057, 0, 0, 20, 32, 3, height + 29, 6, 0, height + 39);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27060, 0, 0, 20, 32, 3, height + 29, 6, 0, height + 39);
            break;
        }
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 28, gTrackColours[SCHEME_SUPPORTS]);
        switch (direction) {
        case 2:
            paint_util_push_tunnel_right(height + 40, TUNNEL_3);
            break;
        case 3:
            paint_util_push_tunnel_left(height + 40, TUNNEL_3);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 64, 0x20);
        break;
    }
}

/** rct2: 0x008AEA70 */
static void compact_inverted_rc_track_right_corkscrew_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27061, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 33);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27064, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 33);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27067, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 33);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27070, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 33);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_BC | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 44, gTrackColours[SCHEME_SUPPORTS]);

        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height, TUNNEL_3);
        }
        paint_util_set_general_support_height(height + 64, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27062, 0, 0, 20, 20, 3, height + 14, 6, 6, height + 24);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27065, 0, 0, 20, 20, 3, height + 14, 6, 6, height + 24);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27068, 0, 0, 20, 20, 3, height + 14, 6, 6, height + 24);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27071, 0, 0, 20, 20, 3, height + 14, 6, 6, height + 24);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 88, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27063, 0, 0, 20, 32, 3, height + 29, 6, 0, height + 39);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27066, 0, 0, 20, 32, 3, height + 29, 6, 0, height + 39);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27069, 0, 0, 20, 32, 3, height + 29, 6, 0, height + 39);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27072, 0, 0, 20, 32, 3, height + 29, 6, 0, height + 39);
            break;
        }
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 28, gTrackColours[SCHEME_SUPPORTS]);
        switch (direction) {
        case 0:
            paint_util_push_tunnel_right(height + 40, TUNNEL_3);
            break;
        case 1:
            paint_util_push_tunnel_left(height + 40, TUNNEL_3);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_BC | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 64, 0x20);
        break;
    }
}

/** rct2: 0x008AEA80 */
static void compact_inverted_rc_track_left_corkscrew_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    compact_inverted_rc_track_right_corkscrew_up(rideIndex, 2 - trackSequence, (direction + 1) & 3, height, mapElement);
}

/** rct2: 0x008AEA90 */
static void compact_inverted_rc_track_right_corkscrew_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    compact_inverted_rc_track_left_corkscrew_up(rideIndex, 2 - trackSequence, (direction - 1) & 3, height, mapElement);
}

/** rct2: 0x008AEAD0 */
static void compact_inverted_rc_track_left_quarter_turn_1_60_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26910, 0, 0, 28, 28, 3, height + 5, 2, 2, height + 5);
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26906, 0, 0, 28, 28, 1, height + 5, 2, 2, height + 104);
        break;
    case 1:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26911, 0, 0, 28, 28, 3, height + 5, 2, 2, height + 5);
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26907, 0, 0, 28, 28, 1, height + 5, 2, 2, height + 104);
        break;
    case 2:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26912, 0, 0, 28, 28, 3, height + 5, 2, 2, height + 5);
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26908, 0, 0, 28, 28, 1, height + 5, 2, 2, height + 104);
        break;
    case 3:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26909, 0, 0, 28, 28, 3, height + 5, 2, 2, height + 5);
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26905, 0, 0, 28, 28, 1, height + 5, 2, 2, height + 104);
        break;
    }
    track_paint_util_left_quarter_turn_1_tile_tunnel(direction, height, -8, TUNNEL_4, +56, TUNNEL_5);
    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 104, 0x20);
}

/** rct2: 0x008AEAB0 */
static void compact_inverted_rc_track_right_quarter_turn_1_60_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26901, 0, 0, 28, 28, 3, height + 5, 2, 2, height + 5);
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26897, 0, 0, 28, 28, 1, height + 5, 2, 2, height + 104);
        break;
    case 1:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26902, 0, 0, 28, 28, 3, height + 5, 2, 2, height + 5);
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26898, 0, 0, 28, 28, 1, height + 5, 2, 2, height + 104);
        break;
    case 2:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26903, 0, 0, 28, 28, 3, height + 5, 2, 2, height + 5);
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26899, 0, 0, 28, 28, 1, height + 5, 2, 2, height + 104);
        break;
    case 3:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26904, 0, 0, 28, 28, 3, height + 5, 2, 2, height + 5);
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26900, 0, 0, 28, 28, 1, height + 5, 2, 2, height + 104);
        break;
    }
    track_paint_util_right_quarter_turn_1_tile_tunnel(direction, height, -8, TUNNEL_4, +56, TUNNEL_5);
    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 104, 0x20);
}

/** rct2: 0x008AEAC0 */
static void compact_inverted_rc_track_left_quarter_turn_1_60_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    compact_inverted_rc_track_right_quarter_turn_1_60_deg_up(rideIndex, trackSequence, (direction + 1) & 3, height, mapElement);
}

/** rct2: 0x008AEAE0 */
static void compact_inverted_rc_track_right_quarter_turn_1_60_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    compact_inverted_rc_track_left_quarter_turn_1_60_deg_up(rideIndex, trackSequence, (direction - 1) & 3, height, mapElement);
}

/** rct2: 0x008AEAA0 */
static void compact_inverted_rc_track_brakes(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
    case 2:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26559, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 29);
        break;
    case 1:
    case 3:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26560, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 29);
        break;
    }

    paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    if (track_paint_util_should_paint_supports(gPaintSession.MapPosition)) {
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 38, gTrackColours[SCHEME_SUPPORTS]);
    }

    paint_util_push_tunnel_rotated(direction, height, TUNNEL_3);
    paint_util_set_general_support_height(height + 48, 0x20);
}

/** rct2: 0x008AEAF0 */
static void compact_inverted_rc_track_left_quarter_banked_helix_large_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27118, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 35);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27123, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 35);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27128, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 35);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27113, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 35);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 44, gTrackColours[SCHEME_SUPPORTS]);

        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height, TUNNEL_3);
        }
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 1:
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27117, 0, 0, 32, 16, 3, height + 29, 0, 0, height + 43);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27122, 0, 0, 32, 16, 3, height + 29, 0, 0, height + 43);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27127, 0, 0, 32, 16, 3, height + 29, 0, 16, height + 43);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27112, 0, 0, 32, 16, 3, height + 29, 0, 16, height + 43);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27116, 0, 0, 16, 16, 3, height + 29, 0, 16, height + 43);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27121, 0, 0, 16, 16, 3, height + 29, 16, 16, height + 43);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27126, 0, 0, 16, 16, 3, height + 29, 16, 0, height + 43);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27111, 0, 0, 16, 16, 3, height + 29, 0, 0, height + 43);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 4:
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 5:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27115, 0, 0, 16, 32, 3, height + 29, 16, 0, height + 43);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27120, 0, 0, 16, 32, 3, height + 29, 0, 0, height + 43);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27125, 0, 0, 16, 32, 3, height + 29, 0, 0, height + 43);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27110, 0, 0, 16, 32, 3, height + 29, 16, 0, height + 43);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 6:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27114, 0, 0, 20, 32, 3, height + 29, 6, 0, height + 43);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27119, 0, 0, 20, 32, 3, height + 29, 6, 0, height + 43);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27124, 0, 0, 20, 32, 3, height + 29, 6, 0, height + 43);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27109, 0, 0, 20, 32, 3, height + 29, 6, 0, height + 43);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 56, gTrackColours[SCHEME_SUPPORTS]);

        switch (direction) {
        case 2:
            paint_util_push_tunnel_right(height + 16, TUNNEL_3);
            break;
        case 3:
            paint_util_push_tunnel_left(height + 16, TUNNEL_3);
            break;
        }
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    }
}

/** rct2: 0x008AEB00 */
static void compact_inverted_rc_track_right_quarter_banked_helix_large_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27089, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 35);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27094, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 35);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27099, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 35);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27104, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 35);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 44, gTrackColours[SCHEME_SUPPORTS]);

        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height, TUNNEL_3);
        }
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 1:
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27090, 0, 0, 32, 16, 3, height + 29, 0, 16, height + 43);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27095, 0, 0, 32, 16, 3, height + 29, 0, 16, height + 43);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27100, 0, 0, 32, 16, 3, height + 29, 0, 0, height + 43);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27105, 0, 0, 32, 16, 3, height + 29, 0, 0, height + 43);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27091, 0, 0, 16, 16, 3, height + 29, 0, 0, height + 43);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27096, 0, 0, 16, 16, 3, height + 29, 16, 0, height + 43);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27101, 0, 0, 16, 16, 3, height + 29, 16, 16, height + 43);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27106, 0, 0, 16, 16, 3, height + 29, 0, 16, height + 43);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 4:
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 5:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27092, 0, 0, 16, 32, 3, height + 29, 16, 0, height + 43);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27097, 0, 0, 16, 32, 3, height + 29, 0, 0, height + 43);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27102, 0, 0, 16, 32, 3, height + 29, 0, 0, height + 43);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27107, 0, 0, 16, 32, 3, height + 29, 16, 0, height + 43);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 6:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27093, 0, 0, 20, 32, 3, height + 29, 6, 0, height + 43);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27098, 0, 0, 20, 32, 3, height + 29, 6, 0, height + 43);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27103, 0, 0, 20, 32, 3, height + 29, 6, 0, height + 43);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27108, 0, 0, 20, 32, 3, height + 29, 6, 0, height + 43);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 56, gTrackColours[SCHEME_SUPPORTS]);

        switch (direction) {
        case 0:
            paint_util_push_tunnel_right(height + 16, TUNNEL_3);
            break;
        case 1:
            paint_util_push_tunnel_left(height + 16, TUNNEL_3);
            break;
        }
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    }
}

/** rct2: 0x008AEB10 */
static void compact_inverted_rc_track_left_quarter_banked_helix_large_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27098, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 43);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27103, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 43);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27108, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 43);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27093, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 43);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 56, gTrackColours[SCHEME_SUPPORTS]);

        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height + 16, TUNNEL_3);
        }
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 1:
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27097, 0, 0, 32, 16, 3, height + 29, 0, 0, height + 43);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27102, 0, 0, 32, 16, 3, height + 29, 0, 0, height + 43);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27107, 0, 0, 32, 16, 3, height + 29, 0, 16, height + 43);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27092, 0, 0, 32, 16, 3, height + 29, 0, 16, height + 43);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27096, 0, 0, 16, 16, 3, height + 29, 0, 16, height + 43);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27101, 0, 0, 16, 16, 3, height + 29, 16, 16, height + 43);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27106, 0, 0, 16, 16, 3, height + 29, 16, 0, height + 43);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27091, 0, 0, 16, 16, 3, height + 29, 0, 0, height + 43);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 4:
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 5:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27095, 0, 0, 16, 32, 3, height + 29, 16, 0, height + 43);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27100, 0, 0, 16, 32, 3, height + 29, 0, 0, height + 43);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27105, 0, 0, 16, 32, 3, height + 29, 0, 0, height + 43);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27090, 0, 0, 16, 32, 3, height + 29, 16, 0, height + 43);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 6:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27094, 0, 0, 20, 32, 3, height + 29, 6, 0, height + 35);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27099, 0, 0, 20, 32, 3, height + 29, 6, 0, height + 35);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27104, 0, 0, 20, 32, 3, height + 29, 6, 0, height + 35);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27089, 0, 0, 20, 32, 3, height + 29, 6, 0, height + 35);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 44, gTrackColours[SCHEME_SUPPORTS]);

        switch (direction) {
        case 2:
            paint_util_push_tunnel_right(height, TUNNEL_3);
            break;
        case 3:
            paint_util_push_tunnel_left(height, TUNNEL_3);
            break;
        }
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    }
}

/** rct2: 0x008AEB20 */
static void compact_inverted_rc_track_right_quarter_banked_helix_large_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27109, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 43);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27114, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 43);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27119, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 43);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27124, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 43);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 56, gTrackColours[SCHEME_SUPPORTS]);

        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height + 16, TUNNEL_3);
        }
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 1:
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27110, 0, 0, 32, 16, 3, height + 29, 0, 16, height + 43);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27115, 0, 0, 32, 16, 3, height + 29, 0, 16, height + 43);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27120, 0, 0, 32, 16, 3, height + 29, 0, 0, height + 43);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27125, 0, 0, 32, 16, 3, height + 29, 0, 0, height + 43);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27111, 0, 0, 16, 16, 3, height + 29, 0, 0, height + 43);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27116, 0, 0, 16, 16, 3, height + 29, 16, 0, height + 43);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27121, 0, 0, 16, 16, 3, height + 29, 16, 16, height + 43);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27126, 0, 0, 16, 16, 3, height + 29, 0, 16, height + 43);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 4:
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 5:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27112, 0, 0, 16, 32, 3, height + 29, 16, 0, height + 43);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27117, 0, 0, 16, 32, 3, height + 29, 0, 0, height + 43);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27122, 0, 0, 16, 32, 3, height + 29, 0, 0, height + 43);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27127, 0, 0, 16, 32, 3, height + 29, 16, 0, height + 43);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 6:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27113, 0, 0, 20, 32, 3, height + 29, 6, 0, height + 35);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27118, 0, 0, 20, 32, 3, height + 29, 6, 0, height + 35);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27123, 0, 0, 20, 32, 3, height + 29, 6, 0, height + 35);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 27128, 0, 0, 20, 32, 3, height + 29, 6, 0, height + 35);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 44, gTrackColours[SCHEME_SUPPORTS]);

        switch (direction) {
        case 0:
            paint_util_push_tunnel_right(height, TUNNEL_3);
            break;
        case 1:
            paint_util_push_tunnel_left(height, TUNNEL_3);
            break;
        }
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    }
}

/** rct2: 0x008AEB30 */
static void compact_inverted_rc_track_on_ride_photo(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
        sub_98196C_rotated(direction, IMAGE_TYPE_REMAP | SPR_STATION_BASE_D, 0, 0, 32, 32, 1, height);
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height, gTrackColours[SCHEME_SUPPORTS]);
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height, gTrackColours[SCHEME_SUPPORTS]);
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26555, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 29);
        break;
    case 1:
        sub_98196C_rotated(direction, IMAGE_TYPE_REMAP | SPR_STATION_BASE_D, 0, 0, 32, 32, 1, height);
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 6, 0, height, gTrackColours[SCHEME_SUPPORTS]);
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 7, 0, height, gTrackColours[SCHEME_SUPPORTS]);
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26556, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 29);
        break;
    case 2:
        sub_98196C_rotated(direction, IMAGE_TYPE_REMAP | SPR_STATION_BASE_D, 0, 0, 32, 32, 1, height);
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height, gTrackColours[SCHEME_SUPPORTS]);
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height, gTrackColours[SCHEME_SUPPORTS]);
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26555, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 29);
        break;
    case 3:
        sub_98196C_rotated(direction, IMAGE_TYPE_REMAP | SPR_STATION_BASE_D, 0, 0, 32, 32, 1, height);
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 6, 0, height, gTrackColours[SCHEME_SUPPORTS]);
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 7, 0, height, gTrackColours[SCHEME_SUPPORTS]);
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26556, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 29);
        break;
    }
    track_paint_util_onride_photo_paint(direction, height + 3, mapElement);
    paint_util_push_tunnel_rotated(direction, height, TUNNEL_3);
    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 64, 0x20);
}

/** rct2: 0x008AEDB0 */
static void compact_inverted_rc_track_90_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 0:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26609, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 61);
                break;
            case 1:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26610, 0, 0, 32, 2, 31, height + 29, 0, 4, height + 11);
                break;
            case 2:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26611, 0, 0, 32, 2, 31, height + 29, 0, 4, height + 11);
                break;
            case 3:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26612, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 61);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26605, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 61);
                break;
            case 1:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26606, 0, 0, 32, 2, 31, height + 29, 0, 4, height + 11);
                break;
            case 2:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26607, 0, 0, 32, 2, 31, height + 29, 0, 4, height + 11);
                break;
            case 3:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26608, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 61);
                break;
            }
        }
        paint_util_set_vertical_tunnel(height + 32);
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 1:
        break;
    }
}

/** rct2: 0x008AEDC0 */
static void compact_inverted_rc_track_90_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    compact_inverted_rc_track_90_deg_up(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008AED70 */
static void compact_inverted_rc_track_60_deg_up_to_90_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 0:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26597, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 85);
                break;
            case 1:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26598, 0, 0, 32, 2, 55, height + 29, 0, 4, height + 11);
                break;
            case 2:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26599, 0, 0, 32, 2, 55, height + 29, 0, 4, height + 11);
                break;
            case 3:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26600, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 85);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26589, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 85);
                break;
            case 1:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26590, 0, 0, 32, 2, 55, height + 29, 0, 4, height + 11);
                break;
            case 2:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26591, 0, 0, 32, 2, 55, height + 29, 0, 4, height + 11);
                break;
            case 3:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26592, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 85);
                break;
            }
        }
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_4);
        }
        paint_util_set_vertical_tunnel(height + 56);
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 1:
        break;
    }
}

/** rct2: 0x008AED80 */
static void compact_inverted_rc_track_90_deg_down_to_60_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    compact_inverted_rc_track_60_deg_up_to_90_deg_up(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008AED90 */
static void compact_inverted_rc_track_90_deg_up_to_60_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    if (track_element_is_lift_hill(mapElement)) {
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26601, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 85);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26602, 0, 0, 32, 2, 55, height + 29, 0, 4, height + 11);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26603, 0, 0, 32, 2, 55, height + 29, 0, 4, height + 11);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26604, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 85);
            break;
        }
    } else {
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26593, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 85);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26594, 0, 0, 32, 2, 55, height + 29, 0, 4, height + 11);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26595, 0, 0, 32, 2, 55, height + 29, 0, 4, height + 11);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26596, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 85);
            break;
        }
    }
    switch (direction) {
    case 1:
        paint_util_push_tunnel_right(height + 48, TUNNEL_5);
        break;
    case 2:
        paint_util_push_tunnel_left(height + 48, TUNNEL_5);
        break;
    }
    paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(height + 96, 0x20);
}

/** rct2: 0x008AEDA0 */
static void compact_inverted_rc_track_60_deg_down_to_90_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 0:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26603, 0, 0, 32, 2, 55, height + 29, 0, 4, height + 11);
                break;
            case 1:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26604, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 85);
                break;
            case 2:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26601, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 85);
                break;
            case 3:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26602, 0, 0, 32, 2, 55, height + 29, 0, 4, height + 11);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26595, 0, 0, 32, 2, 55, height + 29, 0, 4, height + 11);
                break;
            case 1:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26596, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 85);
                break;
            case 2:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26593, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 85);
                break;
            case 3:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26594, 0, 0, 32, 2, 55, height + 29, 0, 4, height + 11);
                break;
            }
        }
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height + 48, TUNNEL_5);
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 96, 0x20);
        break;
    case 1:
        break;
    }
}

/** rct2: 0x008AEB40 */
static void compact_inverted_rc_track_left_eighth_to_diag(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26765, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 29);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26769, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 29);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26773, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 29);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26777, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 29);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 38, gTrackColours[SCHEME_SUPPORTS]);

        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height, TUNNEL_3);
        }
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26766, 0, 0, 32, 16, 3, height + 29, 0, 0, height + 29);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26770, 0, 0, 34, 16, 3, height + 29, 0, 0, height + 29);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26774, 0, 0, 32, 16, 3, height + 29, 0, 16, height + 29);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26778, 0, 0, 32, 16, 3, height + 29, 0, 16, height + 29);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26767, 0, 0, 16, 16, 3, height + 29, 0, 16, height + 29);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26771, 0, 0, 16, 16, 3, height + 29, 16, 16, height + 29);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26775, 0, 0, 16, 16, 3, height + 29, 16, 0, height + 29);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26779, 0, 0, 16, 16, 3, height + 29, 0, 0, height + 29);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 3:
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 4:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26768, 0, 0, 16, 16, 3, height + 29, 16, 16, height + 29);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26772, 0, 0, 16, 18, 3, height + 29, 0, 16, height + 29);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26776, 0, 0, 16, 16, 3, height + 29, 0, 0, height + 29);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26780, 0, 0, 16, 16, 3, height + 29, 16, 0, height + 29);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        switch (direction) {
        case 0:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height + 38, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height + 38, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height + 38, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height + 38, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }

        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    }
}

/** rct2: 0x008AEB50 */
static void compact_inverted_rc_track_right_eighth_to_diag(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26749, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 29);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26753, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 29);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26757, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 29);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26761, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 29);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 38, gTrackColours[SCHEME_SUPPORTS]);

        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height, TUNNEL_3);
        }
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26750, 0, 0, 32, 16, 3, height + 29, 0, 16, height + 29);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26754, 0, 0, 32, 16, 3, height + 29, 0, 16, height + 29);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26758, 0, 0, 34, 16, 3, height + 29, 0, 0, height + 29);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26762, 0, 0, 32, 16, 3, height + 29, 0, 0, height + 29);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26751, 0, 0, 16, 16, 3, height + 29, 0, 0, height + 29);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26755, 0, 0, 16, 16, 3, height + 29, 16, 0, height + 29);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26759, 0, 0, 28, 28, 3, height + 29, 4, 4, height + 29);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26763, 0, 0, 16, 16, 3, height + 29, 0, 16, height + 29);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 3:
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 4:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26752, 0, 0, 16, 16, 3, height + 29, 16, 0, height + 29);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26756, 0, 0, 16, 16, 3, height + 29, 0, 0, height + 29);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26760, 0, 0, 16, 18, 3, height + 29, 0, 16, height + 29);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26764, 0, 0, 16, 16, 3, height + 29, 16, 16, height + 29);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        switch (direction) {
        case 0:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height + 38, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height + 38, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height + 38, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height + 38, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }

        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    }
}

/** rct2: 0x008AEB60 */
static void compact_inverted_rc_track_left_eighth_to_orthogonal(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    compact_inverted_rc_track_right_eighth_to_diag(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008AEB70 */
static void compact_inverted_rc_track_right_eighth_to_orthogonal(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    compact_inverted_rc_track_left_eighth_to_diag(rideIndex, trackSequence, (direction + 3) & 3, height, mapElement);
}

/** rct2: 0x008AED30 */
static void compact_inverted_rc_track_left_eighth_bank_to_diag(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26853, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 29);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26857, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 29);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26861, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 29);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26865, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 29);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 38, gTrackColours[SCHEME_SUPPORTS]);

        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height, TUNNEL_3);
        }
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26854, 0, 0, 32, 16, 3, height + 29, 0, 0, height + 29);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26858, 0, 0, 34, 16, 3, height + 29, 0, 0, height + 29);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26862, 0, 0, 32, 16, 3, height + 29, 0, 16, height + 29);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26866, 0, 0, 32, 16, 3, height + 29, 0, 16, height + 29);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26855, 0, 0, 16, 16, 3, height + 29, 0, 16, height + 29);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26859, 0, 0, 16, 16, 3, height + 29, 16, 16, height + 29);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26863, 0, 0, 16, 16, 3, height + 29, 16, 0, height + 29);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26867, 0, 0, 16, 16, 3, height + 29, 0, 0, height + 29);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 3:
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 4:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26856, 0, 0, 16, 16, 3, height + 29, 16, 16, height + 29);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26860, 0, 0, 16, 18, 3, height + 29, 0, 16, height + 29);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26864, 0, 0, 16, 16, 3, height + 29, 0, 0, height + 29);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26868, 0, 0, 16, 16, 3, height + 29, 16, 0, height + 29);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        switch (direction) {
        case 0:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height + 38, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height + 38, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height + 38, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height + 38, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }

        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    }
}

/** rct2: 0x008AED40 */
static void compact_inverted_rc_track_right_eighth_bank_to_diag(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26837, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 29);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26841, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 29);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26845, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 29);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26849, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 29);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 38, gTrackColours[SCHEME_SUPPORTS]);

        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height, TUNNEL_3);
        }
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26838, 0, 0, 32, 16, 3, height + 29, 0, 16, height + 29);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26842, 0, 0, 32, 16, 3, height + 29, 0, 16, height + 29);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26846, 0, 0, 34, 16, 3, height + 29, 0, 0, height + 29);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26850, 0, 0, 32, 16, 3, height + 29, 0, 0, height + 29);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26839, 0, 0, 16, 16, 3, height + 29, 0, 0, height + 29);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26843, 0, 0, 16, 16, 3, height + 29, 16, 0, height + 29);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26847, 0, 0, 28, 28, 3, height + 29, 4, 4, height + 29);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26851, 0, 0, 16, 16, 3, height + 29, 0, 16, height + 29);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 3:
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 4:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26840, 0, 0, 16, 16, 3, height + 29, 16, 0, height + 29);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26844, 0, 0, 16, 16, 3, height + 29, 0, 0, height + 29);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26848, 0, 0, 16, 18, 3, height + 29, 0, 16, height + 29);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26852, 0, 0, 16, 16, 3, height + 29, 16, 16, height + 29);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        switch (direction) {
        case 0:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height + 38, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height + 38, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height + 38, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height + 38, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }

        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    }
}

/** rct2: 0x008AED50 */
static void compact_inverted_rc_track_left_eighth_bank_to_orthogonal(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    compact_inverted_rc_track_right_eighth_bank_to_diag(rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008AED60 */
static void compact_inverted_rc_track_right_eighth_bank_to_orthogonal(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    compact_inverted_rc_track_left_eighth_bank_to_diag(rideIndex, trackSequence, (direction + 3) & 3, height, mapElement);
}

/** rct2: 0x008AEB80 */
static void compact_inverted_rc_track_diag_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 3:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26812, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 29);
                break;
            }
        } else {
            switch (direction) {
            case 3:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26784, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 29);
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
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26809, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 29);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26781, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 29);
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
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26811, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 29);
                break;
            }
        } else {
            switch (direction) {
            case 2:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26783, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 29);
                break;
            }
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 3:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 1:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26810, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 29);
                break;
            }
        } else {
            switch (direction) {
            case 1:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26782, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 29);
                break;
            }
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        switch (direction) {
        case 0:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height + 38, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height + 38, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height + 38, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height + 38, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }

        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    }
}

/** rct2: 0x008AEBB0 */
static void compact_inverted_rc_track_diag_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 3:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26824, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 45);
                break;
            }
        } else {
            switch (direction) {
            case 3:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26796, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 45);
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
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26821, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 45);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26793, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 45);
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
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26823, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 45);
                break;
            }
        } else {
            switch (direction) {
            case 2:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26795, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 45);
                break;
            }
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 3:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 1:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26822, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 45);
                break;
            }
        } else {
            switch (direction) {
            case 1:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26794, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 45);
                break;
            }
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        switch (direction) {
        case 0:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height + 50, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height + 50, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height + 50, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height + 50, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }

        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    }
}

/** rct2: 0x008AEC10 */
static void compact_inverted_rc_track_diag_60_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26808, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 93);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 120, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26805, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 93);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 120, 0x20);
        break;
    case 2:
        switch (direction) {
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26807, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 93);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 120, 0x20);
        break;
    case 3:
        switch (direction) {
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26806, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 29);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        switch (direction) {
        case 0:
            metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 1, 32, height + 38, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 0, 36, height + 38, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 2, 32, height + 38, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 3, 36, height + 38, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }

        paint_util_set_general_support_height(height + 120, 0x20);
        break;
    }
}

/** rct2: 0x008AEB90 */
static void compact_inverted_rc_track_diag_flat_to_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 3:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26816, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 37);
                break;
            }
        } else {
            switch (direction) {
            case 3:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26788, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 37);
                break;
            }
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 64, 0x20);
        break;
    case 1:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 0:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26813, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 37);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26785, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 37);
                break;
            }
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 64, 0x20);
        break;
    case 2:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 2:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26815, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 37);
                break;
            }
        } else {
            switch (direction) {
            case 2:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26787, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 37);
                break;
            }
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 64, 0x20);
        break;
    case 3:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 1:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26814, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 37);
                break;
            }
        } else {
            switch (direction) {
            case 1:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26786, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 37);
                break;
            }
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        switch (direction) {
        case 0:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height + 42, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height + 42, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height + 42, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height + 42, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }

        paint_util_set_general_support_height(height + 64, 0x20);
        break;
    }
}

/** rct2: 0x008AEBF0 */
static void compact_inverted_rc_track_diag_25_deg_up_to_60_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26800, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 61);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 88, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26797, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 61);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 88, 0x20);
        break;
    case 2:
        switch (direction) {
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26799, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 61);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 88, 0x20);
        break;
    case 3:
        switch (direction) {
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26798, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 61);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        switch (direction) {
        case 0:
            metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 1, 16, height + 38, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 0, 16, height + 38, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 2, 16, height + 38, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 3, 16, height + 38, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }

        paint_util_set_general_support_height(height + 88, 0x20);
        break;
    }
}

/** rct2: 0x008AEC00 */
static void compact_inverted_rc_track_diag_60_deg_up_to_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26804, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 61);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 88, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26801, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 61);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 88, 0x20);
        break;
    case 2:
        switch (direction) {
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26803, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 61);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 88, 0x20);
        break;
    case 3:
        switch (direction) {
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26802, -16, -16, 16, 16, 3, height + 29, 0, 0, height + 61);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        switch (direction) {
        case 0:
            metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 1, 21, height + 38, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 0, 21, height + 38, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 2, 21, height + 38, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 3, 21, height + 38, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }

        paint_util_set_general_support_height(height + 88, 0x20);
        break;
    }
}

/** rct2: 0x008AEBA0 */
static void compact_inverted_rc_track_diag_25_deg_up_to_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 3:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26820, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 37);
                break;
            }
        } else {
            switch (direction) {
            case 3:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26792, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 37);
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
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26817, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 37);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26789, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 37);
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
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26819, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 37);
                break;
            }
        } else {
            switch (direction) {
            case 2:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26791, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 37);
                break;
            }
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 3:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 1:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26818, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 37);
                break;
            }
        } else {
            switch (direction) {
            case 1:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26790, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 37);
                break;
            }
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        switch (direction) {
        case 0:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height + 44, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height + 44, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height + 44, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height + 44, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }

        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    }
}

/** rct2: 0x008AEBE0 */
static void compact_inverted_rc_track_diag_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 3:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26822, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 45);
                break;
            }
        } else {
            switch (direction) {
            case 3:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26794, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 45);
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
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26823, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 45);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26795, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 45);
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
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26821, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 45);
                break;
            }
        } else {
            switch (direction) {
            case 2:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26793, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 45);
                break;
            }
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 3:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 1:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26824, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 45);
                break;
            }
        } else {
            switch (direction) {
            case 1:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26796, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 45);
                break;
            }
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        switch (direction) {
        case 0:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height + 50, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height + 50, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height + 50, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height + 50, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }

        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    }
}

/** rct2: 0x008AEC40 */
static void compact_inverted_rc_track_diag_60_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26806, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 29);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 120, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26807, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 93);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 120, 0x20);
        break;
    case 2:
        switch (direction) {
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26805, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 93);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 120, 0x20);
        break;
    case 3:
        switch (direction) {
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26808, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 93);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        switch (direction) {
        case 0:
            metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 1, 24, height + 38, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 0, 28, height + 38, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 2, 24, height + 38, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 3, 28, height + 38, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }

        paint_util_set_general_support_height(height + 120, 0x20);
        break;
    }
}

/** rct2: 0x008AEBC0 */
static void compact_inverted_rc_track_diag_flat_to_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 3:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26818, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 37);
                break;
            }
        } else {
            switch (direction) {
            case 3:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26790, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 37);
                break;
            }
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        break;
    case 1:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 0:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26819, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 37);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26791, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 37);
                break;
            }
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        break;
    case 2:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 2:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26817, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 37);
                break;
            }
        } else {
            switch (direction) {
            case 2:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26789, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 37);
                break;
            }
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        break;
    case 3:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 1:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26820, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 37);
                break;
            }
        } else {
            switch (direction) {
            case 1:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26792, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 37);
                break;
            }
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        switch (direction) {
        case 0:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height + 44, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height + 44, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height + 44, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height + 44, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }
        break;
    }

    paint_util_set_general_support_height(height + 72, 0x20);
}

/** rct2: 0x008AEC20 */
static void compact_inverted_rc_track_diag_25_deg_down_to_60_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26802, -16, -16, 16, 16, 3, height + 29, 0, 0, height + 61);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 88, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26803, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 61);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 88, 0x20);
        break;
    case 2:
        switch (direction) {
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26801, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 61);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 88, 0x20);
        break;
    case 3:
        switch (direction) {
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26804, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 61);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        switch (direction) {
        case 0:
            metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 1, 17, height + 38, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 0, 17, height + 38, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 2, 17, height + 38, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 3, 17, height + 38, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }

        paint_util_set_general_support_height(height + 88, 0x20);
        break;
    }
}

/** rct2: 0x008AEC30 */
static void compact_inverted_rc_track_diag_60_deg_down_to_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26798, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 61);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 88, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26799, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 61);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 88, 0x20);
        break;
    case 2:
        switch (direction) {
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26797, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 61);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 88, 0x20);
        break;
    case 3:
        switch (direction) {
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26800, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 61);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        switch (direction) {
        case 0:
            metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 1, 8, height + 38, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 0, 8, height + 38, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 2, 8, height + 38, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            metal_b_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 3, 8, height + 38, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }

        paint_util_set_general_support_height(height + 88, 0x20);
        break;
    }
}

/** rct2: 0x008AEBD0 */
static void compact_inverted_rc_track_diag_25_deg_down_to_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 3:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26814, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 37);
                break;
            }
        } else {
            switch (direction) {
            case 3:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26786, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 37);
                break;
            }
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 64, 0x20);
        break;
    case 1:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 0:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26815, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 37);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26787, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 37);
                break;
            }
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 64, 0x20);
        break;
    case 2:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 2:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26813, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 37);
                break;
            }
        } else {
            switch (direction) {
            case 2:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26785, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 37);
                break;
            }
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 64, 0x20);
        break;
    case 3:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 1:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26816, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 37);
                break;
            }
        } else {
            switch (direction) {
            case 1:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26788, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 37);
                break;
            }
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        switch (direction) {
        case 0:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height + 42, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height + 42, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height + 42, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height + 42, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }

        paint_util_set_general_support_height(height + 64, 0x20);
        break;
    }
}

/** rct2: 0x008AEC70 */
static void compact_inverted_rc_track_diag_flat_to_left_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26876, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 29);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26873, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 29);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 2:
        switch (direction) {
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26875, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 29);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 3:
        switch (direction) {
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26874, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 29);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        switch (direction) {
        case 0:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height + 38, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height + 38, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height + 38, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height + 38, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }

        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    }
}

/** rct2: 0x008AEC80 */
static void compact_inverted_rc_track_diag_flat_to_right_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26880, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 29);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26877, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 29);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 2:
        switch (direction) {
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26879, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 29);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 3:
        switch (direction) {
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26878, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 29);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        switch (direction) {
        case 0:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height + 38, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height + 38, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height + 38, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height + 38, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }

        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    }
}

/** rct2: 0x008AEC90 */
static void compact_inverted_rc_track_diag_left_bank_to_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26878, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 29);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26879, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 29);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 2:
        switch (direction) {
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26877, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 29);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 3:
        switch (direction) {
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26880, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 29);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        switch (direction) {
        case 0:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height + 38, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height + 38, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height + 38, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height + 38, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }

        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    }
}

/** rct2: 0x008AECA0 */
static void compact_inverted_rc_track_diag_right_bank_to_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26874, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 29);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26875, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 29);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 2:
        switch (direction) {
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26873, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 29);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 3:
        switch (direction) {
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26876, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 29);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        switch (direction) {
        case 0:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height + 38, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height + 38, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height + 38, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height + 38, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }

        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    }
}

/** rct2: 0x008AECD0 */
static void compact_inverted_rc_track_diag_left_bank_to_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26892, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 37);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 64, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26889, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 37);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 64, 0x20);
        break;
    case 2:
        switch (direction) {
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26891, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 37);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 64, 0x20);
        break;
    case 3:
        switch (direction) {
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26890, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 37);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        switch (direction) {
        case 0:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height + 42, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height + 42, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height + 42, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height + 42, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }

        paint_util_set_general_support_height(height + 64, 0x20);
        break;
    }
}

/** rct2: 0x008AECE0 */
static void compact_inverted_rc_track_diag_right_bank_to_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26896, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 37);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 64, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26893, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 37);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 64, 0x20);
        break;
    case 2:
        switch (direction) {
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26895, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 37);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 64, 0x20);
        break;
    case 3:
        switch (direction) {
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26894, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 37);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        switch (direction) {
        case 0:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height + 42, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height + 42, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height + 42, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height + 42, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }

        paint_util_set_general_support_height(height + 64, 0x20);
        break;
    }
}

/** rct2: 0x008AECB0 */
static void compact_inverted_rc_track_diag_25_deg_up_to_left_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26884, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 37);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26881, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 37);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 2:
        switch (direction) {
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26883, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 37);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 3:
        switch (direction) {
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26882, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 37);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        switch (direction) {
        case 0:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height + 44, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height + 44, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height + 44, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height + 44, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }

        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    }
}

/** rct2: 0x008AECC0 */
static void compact_inverted_rc_track_diag_25_deg_up_to_right_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26888, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 37);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26885, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 37);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 2:
        switch (direction) {
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26887, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 37);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 3:
        switch (direction) {
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26886, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 37);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        switch (direction) {
        case 0:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height + 44, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height + 44, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height + 44, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height + 44, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }

        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    }
}

/** rct2: 0x008AECF0 */
static void compact_inverted_rc_track_diag_left_bank_to_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26886, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 37);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26887, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 37);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        break;
    case 2:
        switch (direction) {
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26885, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 37);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        break;
    case 3:
        switch (direction) {
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26888, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 37);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        switch (direction) {
        case 0:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height + 44, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height + 44, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height + 44, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height + 44, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }
        break;
    }

    paint_util_set_general_support_height(height + 72, 0x20);
}

/** rct2: 0x008AED00 */
static void compact_inverted_rc_track_diag_right_bank_to_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26882, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 37);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26883, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 37);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        break;
    case 2:
        switch (direction) {
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26881, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 37);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        break;
    case 3:
        switch (direction) {
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26884, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 37);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        switch (direction) {
        case 0:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height + 44, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height + 44, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height + 44, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height + 44, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }
        break;
    }

    paint_util_set_general_support_height(height + 72, 0x20);
}

/** rct2: 0x008AED10 */
static void compact_inverted_rc_track_diag_25_deg_down_to_left_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26894, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 37);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 64, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26895, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 37);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 64, 0x20);
        break;
    case 2:
        switch (direction) {
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26893, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 37);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 64, 0x20);
        break;
    case 3:
        switch (direction) {
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26896, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 37);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        switch (direction) {
        case 0:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height + 42, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height + 42, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height + 42, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height + 42, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }

        paint_util_set_general_support_height(height + 64, 0x20);
        break;
    }
}

/** rct2: 0x008AED20 */
static void compact_inverted_rc_track_diag_25_deg_down_to_right_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26890, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 37);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 64, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26891, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 37);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 64, 0x20);
        break;
    case 2:
        switch (direction) {
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26889, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 37);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 64, 0x20);
        break;
    case 3:
        switch (direction) {
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26892, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 37);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        switch (direction) {
        case 0:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height + 42, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height + 42, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height + 42, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height + 42, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }

        paint_util_set_general_support_height(height + 64, 0x20);
        break;
    }
}

/** rct2: 0x008AEC50 */
static void compact_inverted_rc_track_diag_left_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26872, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 29);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26869, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 29);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 2:
        switch (direction) {
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26871, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 29);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 3:
        switch (direction) {
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26870, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 29);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        switch (direction) {
        case 0:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height + 38, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height + 38, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height + 38, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height + 38, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }

        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    }
}

/** rct2: 0x008AEC60 */
static void compact_inverted_rc_track_diag_right_bank(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26870, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 29);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26871, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 29);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 2:
        switch (direction) {
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26869, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 29);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 3:
        switch (direction) {
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26872, -16, -16, 32, 32, 3, height + 29, -16, -16, height + 29);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        switch (direction) {
        case 0:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height + 38, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height + 38, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height + 38, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height + 38, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }

        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    }
}

/** rct2: 0x008AEAA0 */
static void compact_inverted_rc_track_block_brakes(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
    case 2:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26559, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 29);
        break;
    case 1:
    case 3:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 26560, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 29);
        break;
    }

    paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    if (track_paint_util_should_paint_supports(gPaintSession.MapPosition)) {
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 38, gTrackColours[SCHEME_SUPPORTS]);
    }

    paint_util_push_tunnel_rotated(direction, height, TUNNEL_3);
    paint_util_set_general_support_height(height + 48, 0x20);
}

TRACK_PAINT_FUNCTION get_track_paint_function_compact_inverted_rc(sint32 trackType, sint32 direction)
{
    switch (trackType) {
    case TRACK_ELEM_FLAT:
        return compact_inverted_rc_track_flat;
    case TRACK_ELEM_END_STATION:
    case TRACK_ELEM_BEGIN_STATION:
    case TRACK_ELEM_MIDDLE_STATION:
        return compact_inverted_rc_track_station;
    case TRACK_ELEM_25_DEG_UP:
        return compact_inverted_rc_track_25_deg_up;
    case TRACK_ELEM_60_DEG_UP:
        return compact_inverted_rc_track_60_deg_up;
    case TRACK_ELEM_FLAT_TO_25_DEG_UP:
        return compact_inverted_rc_track_flat_to_25_deg_up;
    case TRACK_ELEM_25_DEG_UP_TO_60_DEG_UP:
        return compact_inverted_rc_track_25_deg_up_to_60_deg_up;
    case TRACK_ELEM_60_DEG_UP_TO_25_DEG_UP:
        return compact_inverted_rc_track_60_deg_up_to_25_deg_up;
    case TRACK_ELEM_25_DEG_UP_TO_FLAT:
        return compact_inverted_rc_track_25_deg_up_to_flat;
    case TRACK_ELEM_25_DEG_DOWN:
        return compact_inverted_rc_track_25_deg_down;
    case TRACK_ELEM_60_DEG_DOWN:
        return compact_inverted_rc_track_60_deg_down;
    case TRACK_ELEM_FLAT_TO_25_DEG_DOWN:
        return compact_inverted_rc_track_flat_to_25_deg_down;
    case TRACK_ELEM_25_DEG_DOWN_TO_60_DEG_DOWN:
        return compact_inverted_rc_track_25_deg_down_to_60_deg_down;
    case TRACK_ELEM_60_DEG_DOWN_TO_25_DEG_DOWN:
        return compact_inverted_rc_track_60_deg_down_to_25_deg_down;
    case TRACK_ELEM_25_DEG_DOWN_TO_FLAT:
        return compact_inverted_rc_track_25_deg_down_to_flat;
    case TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES:
        return compact_inverted_rc_track_left_quarter_turn_5;
    case TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES:
        return compact_inverted_rc_track_right_quarter_turn_5;
    case TRACK_ELEM_FLAT_TO_LEFT_BANK:
        return compact_inverted_rc_track_flat_to_left_bank;
    case TRACK_ELEM_FLAT_TO_RIGHT_BANK:
        return compact_inverted_rc_track_flat_to_right_bank;
    case TRACK_ELEM_LEFT_BANK_TO_FLAT:
        return compact_inverted_rc_track_left_bank_to_flat;
    case TRACK_ELEM_RIGHT_BANK_TO_FLAT:
        return compact_inverted_rc_track_right_bank_to_flat;
    case TRACK_ELEM_BANKED_LEFT_QUARTER_TURN_5_TILES:
        return compact_inverted_rc_track_banked_left_quarter_turn_5;
    case TRACK_ELEM_BANKED_RIGHT_QUARTER_TURN_5_TILES:
        return compact_inverted_rc_track_banked_right_quarter_turn_5;
    case TRACK_ELEM_LEFT_BANK_TO_25_DEG_UP:
        return compact_inverted_rc_track_left_bank_to_25_deg_up;
    case TRACK_ELEM_RIGHT_BANK_TO_25_DEG_UP:
        return compact_inverted_rc_track_right_bank_to_25_deg_up;
    case TRACK_ELEM_25_DEG_UP_TO_LEFT_BANK:
        return compact_inverted_rc_track_25_deg_up_to_left_bank;
    case TRACK_ELEM_25_DEG_UP_TO_RIGHT_BANK:
        return compact_inverted_rc_track_25_deg_up_to_right_bank;
    case TRACK_ELEM_LEFT_BANK_TO_25_DEG_DOWN:
        return compact_inverted_rc_track_left_bank_to_25_deg_down;
    case TRACK_ELEM_RIGHT_BANK_TO_25_DEG_DOWN:
        return compact_inverted_rc_track_right_bank_to_25_deg_down;
    case TRACK_ELEM_25_DEG_DOWN_TO_LEFT_BANK:
        return compact_inverted_rc_track_25_deg_down_to_left_bank;
    case TRACK_ELEM_25_DEG_DOWN_TO_RIGHT_BANK:
        return compact_inverted_rc_track_25_deg_down_to_right_bank;
    case TRACK_ELEM_LEFT_BANK:
        return compact_inverted_rc_track_left_bank;
    case TRACK_ELEM_RIGHT_BANK:
        return compact_inverted_rc_track_right_bank;
    case TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES_25_DEG_UP:
        return compact_inverted_rc_track_left_quarter_turn_5_25_deg_up;
    case TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES_25_DEG_UP:
        return compact_inverted_rc_track_right_quarter_turn_5_25_deg_up;
    case TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES_25_DEG_DOWN:
        return compact_inverted_rc_track_left_quarter_turn_5_25_deg_down;
    case TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES_25_DEG_DOWN:
        return compact_inverted_rc_track_right_quarter_turn_5_25_deg_down;
    case TRACK_ELEM_S_BEND_LEFT:
        return compact_inverted_rc_track_s_bend_left;
    case TRACK_ELEM_S_BEND_RIGHT:
        return compact_inverted_rc_track_s_bend_right;
    case TRACK_ELEM_LEFT_VERTICAL_LOOP:
        return compact_inverted_rc_track_left_vertical_loop;
    case TRACK_ELEM_RIGHT_VERTICAL_LOOP:
        return compact_inverted_rc_track_right_vertical_loop;
    case TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES:
        return compact_inverted_rc_track_left_quarter_turn_3;
    case TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES:
        return compact_inverted_rc_track_right_quarter_turn_3;
    case TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_BANK:
        return compact_inverted_rc_track_left_quarter_turn_3_bank;
    case TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_BANK:
        return compact_inverted_rc_track_right_quarter_turn_3_bank;
    case TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_25_DEG_UP:
        return compact_inverted_rc_track_left_quarter_turn_3_25_deg_up;
    case TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_25_DEG_UP:
        return compact_inverted_rc_track_right_quarter_turn_3_25_deg_up;
    case TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_25_DEG_DOWN:
        return compact_inverted_rc_track_left_quarter_turn_3_25_deg_down;
    case TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_25_DEG_DOWN:
        return compact_inverted_rc_track_right_quarter_turn_3_25_deg_down;
    case TRACK_ELEM_LEFT_TWIST_DOWN_TO_UP:
        return compact_inverted_rc_track_left_twist_down_to_up;
    case TRACK_ELEM_RIGHT_TWIST_DOWN_TO_UP:
        return compact_inverted_rc_track_right_twist_down_to_up;
    case TRACK_ELEM_LEFT_TWIST_UP_TO_DOWN:
        return compact_inverted_rc_track_left_twist_up_to_down;
    case TRACK_ELEM_RIGHT_TWIST_UP_TO_DOWN:
        return compact_inverted_rc_track_right_twist_up_to_down;
    case TRACK_ELEM_HALF_LOOP_UP:
        return compact_inverted_rc_track_half_loop_up;
    case TRACK_ELEM_HALF_LOOP_DOWN:
        return compact_inverted_rc_track_half_loop_down;
    case TRACK_ELEM_LEFT_CORKSCREW_UP:
        return compact_inverted_rc_track_left_corkscrew_up;
    case TRACK_ELEM_RIGHT_CORKSCREW_UP:
        return compact_inverted_rc_track_right_corkscrew_up;
    case TRACK_ELEM_LEFT_CORKSCREW_DOWN:
        return compact_inverted_rc_track_left_corkscrew_down;
    case TRACK_ELEM_RIGHT_CORKSCREW_DOWN:
        return compact_inverted_rc_track_right_corkscrew_down;
    case TRACK_ELEM_LEFT_QUARTER_TURN_1_TILE_60_DEG_UP:
        return compact_inverted_rc_track_left_quarter_turn_1_60_deg_up;
    case TRACK_ELEM_RIGHT_QUARTER_TURN_1_TILE_60_DEG_UP:
        return compact_inverted_rc_track_right_quarter_turn_1_60_deg_up;
    case TRACK_ELEM_LEFT_QUARTER_TURN_1_TILE_60_DEG_DOWN:
        return compact_inverted_rc_track_left_quarter_turn_1_60_deg_down;
    case TRACK_ELEM_RIGHT_QUARTER_TURN_1_TILE_60_DEG_DOWN:
        return compact_inverted_rc_track_right_quarter_turn_1_60_deg_down;
    case TRACK_ELEM_BRAKES:
        return compact_inverted_rc_track_brakes;
    case TRACK_ELEM_LEFT_QUARTER_BANKED_HELIX_LARGE_UP:
        return compact_inverted_rc_track_left_quarter_banked_helix_large_up;
    case TRACK_ELEM_RIGHT_QUARTER_BANKED_HELIX_LARGE_UP:
        return compact_inverted_rc_track_right_quarter_banked_helix_large_up;
    case TRACK_ELEM_LEFT_QUARTER_BANKED_HELIX_LARGE_DOWN:
        return compact_inverted_rc_track_left_quarter_banked_helix_large_down;
    case TRACK_ELEM_RIGHT_QUARTER_BANKED_HELIX_LARGE_DOWN:
        return compact_inverted_rc_track_right_quarter_banked_helix_large_down;
    case TRACK_ELEM_ON_RIDE_PHOTO:
        return compact_inverted_rc_track_on_ride_photo;
    case TRACK_ELEM_90_DEG_UP:
        return compact_inverted_rc_track_90_deg_up;
    case TRACK_ELEM_90_DEG_DOWN:
        return compact_inverted_rc_track_90_deg_down;
    case TRACK_ELEM_60_DEG_UP_TO_90_DEG_UP:
        return compact_inverted_rc_track_60_deg_up_to_90_deg_up;
    case TRACK_ELEM_90_DEG_DOWN_TO_60_DEG_DOWN:
        return compact_inverted_rc_track_90_deg_down_to_60_deg_down;
    case TRACK_ELEM_90_DEG_UP_TO_60_DEG_UP:
        return compact_inverted_rc_track_90_deg_up_to_60_deg_up;
    case TRACK_ELEM_60_DEG_DOWN_TO_90_DEG_DOWN:
        return compact_inverted_rc_track_60_deg_down_to_90_deg_down;
    case TRACK_ELEM_LEFT_EIGHTH_TO_DIAG:
        return compact_inverted_rc_track_left_eighth_to_diag;
    case TRACK_ELEM_RIGHT_EIGHTH_TO_DIAG:
        return compact_inverted_rc_track_right_eighth_to_diag;
    case TRACK_ELEM_LEFT_EIGHTH_TO_ORTHOGONAL:
        return compact_inverted_rc_track_left_eighth_to_orthogonal;
    case TRACK_ELEM_RIGHT_EIGHTH_TO_ORTHOGONAL:
        return compact_inverted_rc_track_right_eighth_to_orthogonal;
    case TRACK_ELEM_LEFT_EIGHTH_BANK_TO_DIAG:
        return compact_inverted_rc_track_left_eighth_bank_to_diag;
    case TRACK_ELEM_RIGHT_EIGHTH_BANK_TO_DIAG:
        return compact_inverted_rc_track_right_eighth_bank_to_diag;
    case TRACK_ELEM_LEFT_EIGHTH_BANK_TO_ORTHOGONAL:
        return compact_inverted_rc_track_left_eighth_bank_to_orthogonal;
    case TRACK_ELEM_RIGHT_EIGHTH_BANK_TO_ORTHOGONAL:
        return compact_inverted_rc_track_right_eighth_bank_to_orthogonal;
    case TRACK_ELEM_DIAG_FLAT:
        return compact_inverted_rc_track_diag_flat;
    case TRACK_ELEM_DIAG_25_DEG_UP:
        return compact_inverted_rc_track_diag_25_deg_up;
    case TRACK_ELEM_DIAG_60_DEG_UP:
        return compact_inverted_rc_track_diag_60_deg_up;
    case TRACK_ELEM_DIAG_FLAT_TO_25_DEG_UP:
        return compact_inverted_rc_track_diag_flat_to_25_deg_up;
    case TRACK_ELEM_DIAG_25_DEG_UP_TO_60_DEG_UP:
        return compact_inverted_rc_track_diag_25_deg_up_to_60_deg_up;
    case TRACK_ELEM_DIAG_60_DEG_UP_TO_25_DEG_UP:
        return compact_inverted_rc_track_diag_60_deg_up_to_25_deg_up;
    case TRACK_ELEM_DIAG_25_DEG_UP_TO_FLAT:
        return compact_inverted_rc_track_diag_25_deg_up_to_flat;
    case TRACK_ELEM_DIAG_25_DEG_DOWN:
        return compact_inverted_rc_track_diag_25_deg_down;
    case TRACK_ELEM_DIAG_60_DEG_DOWN:
        return compact_inverted_rc_track_diag_60_deg_down;
    case TRACK_ELEM_DIAG_FLAT_TO_25_DEG_DOWN:
        return compact_inverted_rc_track_diag_flat_to_25_deg_down;
    case TRACK_ELEM_DIAG_25_DEG_DOWN_TO_60_DEG_DOWN:
        return compact_inverted_rc_track_diag_25_deg_down_to_60_deg_down;
    case TRACK_ELEM_DIAG_60_DEG_DOWN_TO_25_DEG_DOWN:
        return compact_inverted_rc_track_diag_60_deg_down_to_25_deg_down;
    case TRACK_ELEM_DIAG_25_DEG_DOWN_TO_FLAT:
        return compact_inverted_rc_track_diag_25_deg_down_to_flat;
    case TRACK_ELEM_DIAG_FLAT_TO_LEFT_BANK:
        return compact_inverted_rc_track_diag_flat_to_left_bank;
    case TRACK_ELEM_DIAG_FLAT_TO_RIGHT_BANK:
        return compact_inverted_rc_track_diag_flat_to_right_bank;
    case TRACK_ELEM_DIAG_LEFT_BANK_TO_FLAT:
        return compact_inverted_rc_track_diag_left_bank_to_flat;
    case TRACK_ELEM_DIAG_RIGHT_BANK_TO_FLAT:
        return compact_inverted_rc_track_diag_right_bank_to_flat;
    case TRACK_ELEM_DIAG_LEFT_BANK_TO_25_DEG_UP:
        return compact_inverted_rc_track_diag_left_bank_to_25_deg_up;
    case TRACK_ELEM_DIAG_RIGHT_BANK_TO_25_DEG_UP:
        return compact_inverted_rc_track_diag_right_bank_to_25_deg_up;
    case TRACK_ELEM_DIAG_25_DEG_UP_TO_LEFT_BANK:
        return compact_inverted_rc_track_diag_25_deg_up_to_left_bank;
    case TRACK_ELEM_DIAG_25_DEG_UP_TO_RIGHT_BANK:
        return compact_inverted_rc_track_diag_25_deg_up_to_right_bank;
    case TRACK_ELEM_DIAG_LEFT_BANK_TO_25_DEG_DOWN:
        return compact_inverted_rc_track_diag_left_bank_to_25_deg_down;
    case TRACK_ELEM_DIAG_RIGHT_BANK_TO_25_DEG_DOWN:
        return compact_inverted_rc_track_diag_right_bank_to_25_deg_down;
    case TRACK_ELEM_DIAG_25_DEG_DOWN_TO_LEFT_BANK:
        return compact_inverted_rc_track_diag_25_deg_down_to_left_bank;
    case TRACK_ELEM_DIAG_25_DEG_DOWN_TO_RIGHT_BANK:
        return compact_inverted_rc_track_diag_25_deg_down_to_right_bank;
    case TRACK_ELEM_DIAG_LEFT_BANK:
        return compact_inverted_rc_track_diag_left_bank;
    case TRACK_ELEM_DIAG_RIGHT_BANK:
        return compact_inverted_rc_track_diag_right_bank;
    case TRACK_ELEM_BLOCK_BRAKES:
        return compact_inverted_rc_track_block_brakes;
    }
    return NULL;
}
