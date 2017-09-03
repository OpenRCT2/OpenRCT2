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

/** rct2: 0x008B0460 */
static void inverted_impulse_rc_track_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
    case 2:
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 19662, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 29);
        break;
    case 1:
    case 3:
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 19663, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 29);
        break;
    }

    paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    if (track_paint_util_should_paint_supports(session->MapPosition)) {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 44, gTrackColours[SCHEME_SUPPORTS]);
    }

    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_3);
    paint_util_set_general_support_height(session, height + 48, 0x20);
}

/** rct2: 0x008B0470, 0x008B0480, 0x008B0490 */
static void inverted_impulse_rc_track_station(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    static const uint32 imageIds[4][3] = {
        { SPR_STATION_BASE_C_SW_NE, 19662, SPR_STATION_INVERTED_BAR_B_SW_NE },
        { SPR_STATION_BASE_C_NW_SE, 19663, SPR_STATION_INVERTED_BAR_B_NW_SE },
        { SPR_STATION_BASE_C_SW_NE, 19662, SPR_STATION_INVERTED_BAR_B_SW_NE },
        { SPR_STATION_BASE_C_NW_SE, 19663, SPR_STATION_INVERTED_BAR_B_NW_SE },
    };

    sub_98197C_rotated(session, direction, imageIds[direction][0] | gTrackColours[SCHEME_MISC], 0, 0, 32, 28, 1, height, 0, 2, height);
    sub_98197C_rotated(session, direction, imageIds[direction][1] | gTrackColours[SCHEME_TRACK], 0, 0, 32, 20, 3, height + 29, 0, 6, height + 29);
    sub_98199C_rotated(session, direction, imageIds[direction][2] | gTrackColours[SCHEME_SUPPORTS], 0, 6, 32, 20, 3, height + 29, 0, 6, height + 29);
    track_paint_util_draw_station_metal_supports_2(session, direction, height, gTrackColours[SCHEME_SUPPORTS], 11);
    track_paint_util_draw_station_inverted(session, rideIndex, trackSequence, direction, height, mapElement, STATION_VARIANT_TALL);
    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_9);
    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 48, 0x20);
}

/** rct2: 0x008B04A0 */
static void inverted_impulse_rc_track_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 19672, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 45);
        break;
    case 1:
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 19673, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 45);
        break;
    case 2:
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 19674, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 45);
        break;
    case 3:
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 19675, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 45);
        break;
    }

    paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    if (track_paint_util_should_paint_supports(session->MapPosition)) {
        switch (direction) {
        case 0:
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES_INVERTED, 6, 0, height + 62, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height + 62, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES_INVERTED, 7, 0, height + 62, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height + 62, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }
    }

    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_4);
    } else {
        paint_util_push_tunnel_rotated(session, direction, height + 8, TUNNEL_5);
    }
    paint_util_set_general_support_height(session, height + 72, 0x20);
}

/** rct2: 0x008B04B0 */
static void inverted_impulse_rc_track_60_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 19688, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 93);
        break;
    case 1:
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 19689, 0, 0, 32, 2, 81, height + 29, 0, 4, height + 11);
        break;
    case 2:
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 19690, 0, 0, 32, 2, 81, height + 29, 0, 4, height + 11);
        break;
    case 3:
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 19691, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 93);
        break;
    }
    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_4);
    } else {
        paint_util_push_tunnel_rotated(session, direction, height + 56, TUNNEL_5);
    }
    paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 120, 0x20);
}

/** rct2: 0x008B04C0 */
static void inverted_impulse_rc_track_flat_to_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 19664, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 37);
        break;
    case 1:
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 19665, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 37);
        break;
    case 2:
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 19666, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 37);
        break;
    case 3:
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 19667, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 37);
        break;
    }

    paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    switch (direction) {
    case 0:
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES_INVERTED, 6, 0, height + 54, gTrackColours[SCHEME_SUPPORTS]);
        break;
    case 1:
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height + 54, gTrackColours[SCHEME_SUPPORTS]);
        break;
    case 2:
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES_INVERTED, 7, 0, height + 54, gTrackColours[SCHEME_SUPPORTS]);
        break;
    case 3:
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height + 54, gTrackColours[SCHEME_SUPPORTS]);
        break;
    }

    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_3);
    } else {
        paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_5);
    }
    paint_util_set_general_support_height(session, height + 64, 0x20);
}

/** rct2: 0x008B04D0 */
static void inverted_impulse_rc_track_25_deg_up_to_60_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 19676, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 61);
        break;
    case 1:
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 19680, 0, 0, 32, 10, 49, height + 29, 0, 10, height + 11);
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 19677, 0, 0, 32, 2, 49, height + 29, 0, 4, height + 11);
        break;
    case 2:
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 19681, 0, 0, 32, 10, 49, height + 29, 0, 10, height + 11);
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 19678, 0, 0, 32, 2, 49, height + 29, 0, 4, height + 11);
        break;
    case 3:
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 19679, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 61);
        break;
    }
    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_4);
    } else {
        paint_util_push_tunnel_rotated(session, direction, height + 24, TUNNEL_5);
    }
    paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 88, 0x20);
}

/** rct2: 0x008B04E0 */
static void inverted_impulse_rc_track_60_deg_up_to_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 19682, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 61);
        break;
    case 1:
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 19686, 0, 0, 32, 10, 49, height + 29, 0, 10, height + 11);
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 19683, 0, 0, 32, 2, 49, height + 29, 0, 4, height + 11);
        break;
    case 2:
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 19687, 0, 0, 32, 10, 49, height + 29, 0, 10, height + 11);
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 19684, 0, 0, 32, 2, 49, height + 29, 0, 4, height + 11);
        break;
    case 3:
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 19685, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 61);
        break;
    }
    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_4);
    } else {
        paint_util_push_tunnel_rotated(session, direction, height + 24, TUNNEL_5);
    }
    paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 88, 0x20);
}

/** rct2: 0x008B04F0 */
static void inverted_impulse_rc_track_25_deg_up_to_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 19668, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 37);
        break;
    case 1:
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 19669, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 37);
        break;
    case 2:
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 19670, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 37);
        break;
    case 3:
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 19671, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 37);
        break;
    }

    paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    switch (direction) {
    case 0:
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES_INVERTED, 6, 0, height + 52, gTrackColours[SCHEME_SUPPORTS]);
        break;
    case 1:
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height + 52, gTrackColours[SCHEME_SUPPORTS]);
        break;
    case 2:
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES_INVERTED, 7, 0, height + 52, gTrackColours[SCHEME_SUPPORTS]);
        break;
    case 3:
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height + 52, gTrackColours[SCHEME_SUPPORTS]);
        break;
    }

    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_3);
    } else {
        paint_util_push_tunnel_rotated(session, direction, height + 8, TUNNEL_13);
    }
    paint_util_set_general_support_height(session, height + 56, 0x20);
}

/** rct2: 0x008B0500 */
static void inverted_impulse_rc_track_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    inverted_impulse_rc_track_25_deg_up(session, rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008B0510 */
static void inverted_impulse_rc_track_60_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    inverted_impulse_rc_track_60_deg_up(session, rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008B0520 */
static void inverted_impulse_rc_track_flat_to_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    inverted_impulse_rc_track_25_deg_up_to_flat(session, rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008B0530 */
static void inverted_impulse_rc_track_25_deg_down_to_60_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    inverted_impulse_rc_track_60_deg_up_to_25_deg_up(session, rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008B0540 */
static void inverted_impulse_rc_track_60_deg_down_to_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    inverted_impulse_rc_track_25_deg_up_to_60_deg_up(session, rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008B0550 */
static void inverted_impulse_rc_track_25_deg_down_to_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    inverted_impulse_rc_track_flat_to_25_deg_up(session, rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008B05A0 */
static void inverted_impulse_rc_track_90_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 19700, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 61);
            break;
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 19701, 0, 0, 32, 2, 31, height + 29, 0, 4, height + 11);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 19702, 0, 0, 32, 2, 31, height + 29, 0, 4, height + 11);
            break;
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 19703, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 61);
            break;
        }
        paint_util_set_vertical_tunnel(session, height + 32);
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 1:
        break;
    }
}

/** rct2: 0x008B05B0 */
static void inverted_impulse_rc_track_90_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    inverted_impulse_rc_track_90_deg_up(session, rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008B0560 */
static void inverted_impulse_rc_track_60_deg_up_to_90_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 19692, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 85);
            break;
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 19693, 0, 0, 32, 2, 55, height + 29, 0, 4, height + 11);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 19694, 0, 0, 32, 2, 55, height + 29, 0, 4, height + 11);
            break;
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 19695, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 85);
            break;
        }
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_4);
        }
        paint_util_set_vertical_tunnel(session, height + 56);
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 72, 0x20);
        break;
    case 1:
        break;
    }
}

/** rct2: 0x008B0570 */
static void inverted_impulse_rc_track_90_deg_down_to_60_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    inverted_impulse_rc_track_60_deg_up_to_90_deg_up(session, rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008B0580 */
static void inverted_impulse_rc_track_90_deg_up_to_60_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 19696, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 85);
        break;
    case 1:
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 19697, 0, 0, 32, 2, 55, height + 29, 0, 4, height + 11);
        break;
    case 2:
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 19698, 0, 0, 32, 2, 55, height + 29, 0, 4, height + 11);
        break;
    case 3:
        sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 19699, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 85);
        break;
    }
    switch (direction) {
    case 1:
        paint_util_push_tunnel_right(session, height + 48, TUNNEL_5);
        break;
    case 2:
        paint_util_push_tunnel_left(session, height + 48, TUNNEL_5);
        break;
    }
    paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 96, 0x20);
}

/** rct2: 0x008B0590 */
static void inverted_impulse_rc_track_60_deg_down_to_90_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 19698, 0, 0, 32, 2, 55, height + 29, 0, 4, height + 11);
            break;
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 19699, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 85);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 19696, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 85);
            break;
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 19697, 0, 0, 32, 2, 55, height + 29, 0, 4, height + 11);
            break;
        }
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(session, direction, height + 48, TUNNEL_5);
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 96, 0x20);
        break;
    case 1:
        break;
    }
}

/** rct2: 0x008B05C0 */
static void inverted_impulse_rc_track_left_quarter_turn_1_90_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 19708, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 125);
            break;
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 19709, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 125);
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 19717, 0, 0, 2, 32, 31, height + 29, 4, 0, height + 11);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 19710, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 125);
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 19718, 0, 0, 32, 2, 31, height + 29, 0, 4, height + 11);
            break;
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 19711, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 125);
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 19719, 0, 0, 32, 2, 31, height + 29, 0, 4, height + 11);
            break;
        }
        paint_util_set_vertical_tunnel(session, height + 96);
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 96, 0x20);
        break;
    case 1:
        break;
    }
}

/** rct2: 0x008B05D0 */
static void inverted_impulse_rc_track_right_quarter_turn_1_90_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 19704, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 125);
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 19712, 0, 0, 32, 2, 31, height + 29, 0, 4, height + 11);
            break;
        case 1:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 19705, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 125);
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 19713, 0, 0, 32, 2, 31, height + 29, 0, 4, height + 11);
            break;
        case 2:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 19706, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 125);
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 19714, 0, 0, 32, 2, 31, height + 29, 0, 4, height + 11);
            break;
        case 3:
            sub_98197C_rotated(session, direction, gTrackColours[SCHEME_TRACK] | 19707, 0, 0, 32, 20, 3, height + 29, 0, 6, height + 125);
            break;
        }
        paint_util_set_vertical_tunnel(session, height + 96);
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 96, 0x20);
        break;
    case 1:
        break;
    }
}

/** rct2: 0x008B05E0 */
static void inverted_impulse_rc_track_left_quarter_turn_1_90_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    inverted_impulse_rc_track_right_quarter_turn_1_90_deg_up(session, rideIndex, trackSequence, (direction + 1) & 3, height, mapElement);
}

/** rct2: 0x008B05F0 */
static void inverted_impulse_rc_track_right_quarter_turn_1_90_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    inverted_impulse_rc_track_left_quarter_turn_1_90_deg_up(session, rideIndex, trackSequence, (direction - 1) & 3, height, mapElement);
}

TRACK_PAINT_FUNCTION get_track_paint_function_inverted_impulse_rc(sint32 trackType, sint32 direction)
{
    switch (trackType) {
    case TRACK_ELEM_FLAT:
        return inverted_impulse_rc_track_flat;
    case TRACK_ELEM_END_STATION:
    case TRACK_ELEM_BEGIN_STATION:
    case TRACK_ELEM_MIDDLE_STATION:
        return inverted_impulse_rc_track_station;
    case TRACK_ELEM_25_DEG_UP:
        return inverted_impulse_rc_track_25_deg_up;
    case TRACK_ELEM_60_DEG_UP:
        return inverted_impulse_rc_track_60_deg_up;
    case TRACK_ELEM_FLAT_TO_25_DEG_UP:
        return inverted_impulse_rc_track_flat_to_25_deg_up;
    case TRACK_ELEM_25_DEG_UP_TO_60_DEG_UP:
        return inverted_impulse_rc_track_25_deg_up_to_60_deg_up;
    case TRACK_ELEM_60_DEG_UP_TO_25_DEG_UP:
        return inverted_impulse_rc_track_60_deg_up_to_25_deg_up;
    case TRACK_ELEM_25_DEG_UP_TO_FLAT:
        return inverted_impulse_rc_track_25_deg_up_to_flat;
    case TRACK_ELEM_25_DEG_DOWN:
        return inverted_impulse_rc_track_25_deg_down;
    case TRACK_ELEM_60_DEG_DOWN:
        return inverted_impulse_rc_track_60_deg_down;
    case TRACK_ELEM_FLAT_TO_25_DEG_DOWN:
        return inverted_impulse_rc_track_flat_to_25_deg_down;
    case TRACK_ELEM_25_DEG_DOWN_TO_60_DEG_DOWN:
        return inverted_impulse_rc_track_25_deg_down_to_60_deg_down;
    case TRACK_ELEM_60_DEG_DOWN_TO_25_DEG_DOWN:
        return inverted_impulse_rc_track_60_deg_down_to_25_deg_down;
    case TRACK_ELEM_25_DEG_DOWN_TO_FLAT:
        return inverted_impulse_rc_track_25_deg_down_to_flat;
    case TRACK_ELEM_90_DEG_UP:
        return inverted_impulse_rc_track_90_deg_up;
    case TRACK_ELEM_90_DEG_DOWN:
        return inverted_impulse_rc_track_90_deg_down;
    case TRACK_ELEM_60_DEG_UP_TO_90_DEG_UP:
        return inverted_impulse_rc_track_60_deg_up_to_90_deg_up;
    case TRACK_ELEM_90_DEG_DOWN_TO_60_DEG_DOWN:
        return inverted_impulse_rc_track_90_deg_down_to_60_deg_down;
    case TRACK_ELEM_90_DEG_UP_TO_60_DEG_UP:
        return inverted_impulse_rc_track_90_deg_up_to_60_deg_up;
    case TRACK_ELEM_60_DEG_DOWN_TO_90_DEG_DOWN:
        return inverted_impulse_rc_track_60_deg_down_to_90_deg_down;
    case TRACK_ELEM_LEFT_QUARTER_TURN_1_TILE_90_DEG_UP:
        return inverted_impulse_rc_track_left_quarter_turn_1_90_deg_up;
    case TRACK_ELEM_RIGHT_QUARTER_TURN_1_TILE_90_DEG_UP:
        return inverted_impulse_rc_track_right_quarter_turn_1_90_deg_up;
    case TRACK_ELEM_LEFT_QUARTER_TURN_1_TILE_90_DEG_DOWN:
        return inverted_impulse_rc_track_left_quarter_turn_1_90_deg_down;
    case TRACK_ELEM_RIGHT_QUARTER_TURN_1_TILE_90_DEG_DOWN:
        return inverted_impulse_rc_track_right_quarter_turn_1_90_deg_down;
    }
    return NULL;
}
