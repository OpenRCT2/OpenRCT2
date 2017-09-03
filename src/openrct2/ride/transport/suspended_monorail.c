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

/** rct2: 0x008636F4 */
static void suspended_monorail_track_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
    case 2:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25853, 0, 0, 32, 20, 3, height + 32, 0, 6, height + 32);
        break;
    case 1:
    case 3:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25854, 0, 0, 32, 20, 3, height + 32, 0, 6, height + 32);
        break;
    }

    paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    if (track_paint_util_should_paint_supports(session->MapPosition)) {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 4, 0, height + 42, gTrackColours[SCHEME_SUPPORTS]);
    }

    paint_util_push_tunnel_rotated(direction, height, TUNNEL_9);
    paint_util_set_general_support_height(height + 48, 0x20);
}

/** rct2: 0x008637A4, 0x008637B4, 0x008637C4 */
static void suspended_monorail_track_station(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    static const uint32 imageIds[4][3] = {
        { SPR_STATION_BASE_C_SW_NE, 25853, SPR_STATION_INVERTED_BAR_F_SW_NE },
        { SPR_STATION_BASE_C_NW_SE, 25854, SPR_STATION_INVERTED_BAR_F_NW_SE },
        { SPR_STATION_BASE_C_SW_NE, 25853, SPR_STATION_INVERTED_BAR_F_SW_NE },
        { SPR_STATION_BASE_C_NW_SE, 25854, SPR_STATION_INVERTED_BAR_F_NW_SE },
    };

    sub_98197C_rotated(direction, imageIds[direction][0] | gTrackColours[SCHEME_MISC], 0, 0, 32, 28, 1, height, 0, 2, height);
    sub_98197C_rotated(direction, imageIds[direction][1] | gTrackColours[SCHEME_TRACK], 0, 0, 32, 20, 3, height + 32, 0, 6, height + 32);
    sub_98199C_rotated(direction, imageIds[direction][2] | gTrackColours[SCHEME_SUPPORTS], 0, 6, 32, 20, 3, height + 32, 0, 6, height + 32);
    track_paint_util_draw_station_metal_supports_2(session, direction, height, gTrackColours[SCHEME_SUPPORTS], 3);
    track_paint_util_draw_station_inverted(session, rideIndex, trackSequence, direction, height, mapElement, STATION_VARIANT_TALL);
    paint_util_push_tunnel_rotated(direction, height, TUNNEL_9);
    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 48, 0x20);
}

/** rct2: 0x00863704 */
static void suspended_monorail_track_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25945, 0, 0, 32, 20, 3, height + 32, 0, 6, height + 48);
        break;
    case 1:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25946, 0, 0, 32, 20, 3, height + 32, 0, 6, height + 48);
        break;
    case 2:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25947, 0, 0, 32, 20, 3, height + 32, 0, 6, height + 48);
        break;
    case 3:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25948, 0, 0, 32, 20, 3, height + 32, 0, 6, height + 48);
        break;
    }

    paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    if (track_paint_util_should_paint_supports(session->MapPosition)) {
        switch (direction) {
        case 0:
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 6, 0, height + 60, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 8, 0, height + 60, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 7, 0, height + 60, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 5, 0, height + 60, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }
    }

    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_7);
    } else {
        paint_util_push_tunnel_rotated(direction, height + 8, TUNNEL_8);
    }
    paint_util_set_general_support_height(height + 72, 0x20);
}

/** rct2: 0x00863714 */
static void suspended_monorail_track_flat_to_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25937, 0, 0, 32, 20, 3, height + 32, 0, 6, height + 40);
        break;
    case 1:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25938, 0, 0, 32, 20, 3, height + 32, 0, 6, height + 40);
        break;
    case 2:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25939, 0, 0, 32, 20, 3, height + 32, 0, 6, height + 40);
        break;
    case 3:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25940, 0, 0, 32, 20, 3, height + 32, 0, 6, height + 40);
        break;
    }

    paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    if (track_paint_util_should_paint_supports(session->MapPosition)) {
        switch (direction) {
        case 0:
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 6, 0, height + 52, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 8, 0, height + 52, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 7, 0, height + 52, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 5, 0, height + 52, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }
    }

    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(direction, height, TUNNEL_9);
    } else {
        paint_util_push_tunnel_rotated(direction, height, TUNNEL_8);
    }
    paint_util_set_general_support_height(height + 64, 0x20);
}

/** rct2: 0x00863724 */
static void suspended_monorail_track_25_deg_up_to_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25941, 0, 0, 32, 20, 3, height + 32, 0, 6, height + 40);
        break;
    case 1:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25942, 0, 0, 32, 20, 3, height + 32, 0, 6, height + 40);
        break;
    case 2:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25943, 0, 0, 32, 20, 3, height + 32, 0, 6, height + 40);
        break;
    case 3:
        sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25944, 0, 0, 32, 20, 3, height + 32, 0, 6, height + 40);
        break;
    }

    paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    if (track_paint_util_should_paint_supports(session->MapPosition)) {
        switch (direction) {
        case 0:
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 6, 0, height + 50, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 8, 0, height + 50, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 7, 0, height + 50, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 5, 0, height + 50, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }
    }

    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_9);
    } else {
        paint_util_push_tunnel_rotated(direction, height + 8, TUNNEL_15);
    }
    paint_util_set_general_support_height(height + 56, 0x20);
}

/** rct2: 0x00863734 */
static void suspended_monorail_track_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    suspended_monorail_track_25_deg_up(session, rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x00863744 */
static void suspended_monorail_track_flat_to_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    suspended_monorail_track_25_deg_up_to_flat(session, rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x00863754 */
static void suspended_monorail_track_25_deg_down_to_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    suspended_monorail_track_flat_to_25_deg_up(session, rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x00863764 */
static void suspended_monorail_track_left_quarter_turn_5(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25876, 0, 0, 32, 20, 3, height + 32, 0, 6, height + 32);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25881, 0, 0, 32, 20, 3, height + 32, 0, 6, height + 32);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25886, 0, 0, 32, 20, 3, height + 32, 0, 6, height + 32);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25871, 0, 0, 32, 20, 3, height + 32, 0, 6, height + 32);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 4, 0, height + 42, gTrackColours[SCHEME_SUPPORTS]);

        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height, TUNNEL_9);
        }
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 1:
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25875, 0, 0, 32, 16, 3, height + 32);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25880, 0, 0, 32, 16, 3, height + 32);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25885, 0, 0, 32, 16, 3, height + 32, 0, 16, height + 32);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25870, 0, 0, 32, 16, 3, height + 32, 0, 16, height + 32);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25874, 0, 0, 16, 16, 3, height + 32, 0, 16, height + 32);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25879, 0, 0, 16, 16, 3, height + 32, 16, 16, height + 32);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25884, 0, 0, 16, 16, 3, height + 32, 16, 0, height + 32);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25869, 0, 0, 16, 16, 3, height + 32);
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
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25873, 0, 0, 16, 32, 3, height + 32, 16, 0, height + 32);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25878, 0, 0, 16, 32, 3, height + 32);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25883, 0, 0, 16, 32, 3, height + 32);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25868, 0, 0, 16, 32, 3, height + 32, 16, 0, height + 32);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 6:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25872, 0, 0, 20, 32, 3, height + 32, 6, 0, height + 32);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25877, 0, 0, 20, 32, 3, height + 32, 6, 0, height + 32);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25882, 0, 0, 20, 32, 3, height + 32, 6, 0, height + 32);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25867, 0, 0, 20, 32, 3, height + 32, 6, 0, height + 32);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 4, 0, height + 42, gTrackColours[SCHEME_SUPPORTS]);

        switch (direction) {
        case 2:
            paint_util_push_tunnel_right(height, TUNNEL_9);
            break;
        case 3:
            paint_util_push_tunnel_left(height, TUNNEL_9);
            break;
        }
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    }
}

/** rct2: 0x00863774 */
static void suspended_monorail_track_right_quarter_turn_5(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    suspended_monorail_track_left_quarter_turn_5(session, rideIndex, trackSequence, (direction - 1) & 3, height, mapElement);
}

/** rct2: 0x00863784 */
static void suspended_monorail_track_s_bend_left(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25919, 0, 0, 32, 20, 3, height + 32, 0, 6, height + 32);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25923, 0, 0, 32, 20, 3, height + 32, 0, 6, height + 32);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25922, 0, 0, 32, 20, 3, height + 32, 0, 6, height + 32);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25926, 0, 0, 32, 20, 3, height + 32, 0, 6, height + 32);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 4, 0, height + 42, gTrackColours[SCHEME_SUPPORTS]);

        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height, TUNNEL_9);
        }
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25920, 0, 0, 32, 26, 3, height + 32);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25924, 0, 0, 32, 26, 3, height + 32);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25921, 0, 0, 32, 26, 3, height + 32, 0, 6, height + 32);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25925, 0, 0, 32, 26, 3, height + 32, 0, 6, height + 32);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        switch (direction) {
        case 0:
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 5, 0, height + 42, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 6, 0, height + 42, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }

        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25921, 0, 0, 32, 26, 3, height + 32, 0, 6, height + 32);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25925, 0, 0, 32, 26, 3, height + 32, 0, 6, height + 32);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25920, 0, 0, 32, 26, 3, height + 32);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25924, 0, 0, 32, 26, 3, height + 32);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        switch (direction) {
        case 2:
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 5, 0, height + 42, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 6, 0, height + 42, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }

        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25922, 0, 0, 32, 20, 3, height + 32, 0, 6, height + 32);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25926, 0, 0, 32, 20, 3, height + 32, 0, 6, height + 32);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25919, 0, 0, 32, 20, 3, height + 32, 0, 6, height + 32);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25923, 0, 0, 32, 20, 3, height + 32, 0, 6, height + 32);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 4, 0, height + 42, gTrackColours[SCHEME_SUPPORTS]);

        switch (direction) {
        case 1:
            paint_util_push_tunnel_right(height, TUNNEL_9);
            break;
        case 2:
            paint_util_push_tunnel_left(height, TUNNEL_9);
            break;
        }
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    }
}

/** rct2: 0x00863794 */
static void suspended_monorail_track_s_bend_right(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25927, 0, 0, 32, 20, 3, height + 32, 0, 6, height + 32);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25931, 0, 0, 32, 20, 3, height + 32, 0, 6, height + 32);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25930, 0, 0, 32, 20, 3, height + 32, 0, 6, height + 32);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25934, 0, 0, 32, 20, 3, height + 32, 0, 6, height + 32);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 4, 0, height + 42, gTrackColours[SCHEME_SUPPORTS]);

        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height, TUNNEL_9);
        }
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25928, 0, 0, 32, 26, 3, height + 32, 0, 6, height + 32);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25932, 0, 0, 32, 26, 3, height + 32, 0, 6, height + 32);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25929, 0, 0, 32, 26, 3, height + 32);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25933, 0, 0, 32, 26, 3, height + 32);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        switch (direction) {
        case 0:
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 8, 0, height + 42, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 7, 0, height + 42, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }

        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25929, 0, 0, 32, 26, 3, height + 32);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25933, 0, 0, 32, 26, 3, height + 32);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25928, 0, 0, 32, 26, 3, height + 32, 0, 6, height + 32);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25932, 0, 0, 32, 26, 3, height + 32, 0, 6, height + 32);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        switch (direction) {
        case 2:
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 8, 0, height + 42, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 7, 0, height + 42, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }

        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25930, 0, 0, 32, 20, 3, height + 32, 0, 6, height + 32);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25934, 0, 0, 32, 20, 3, height + 32, 0, 6, height + 32);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25927, 0, 0, 32, 20, 3, height + 32, 0, 6, height + 32);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25931, 0, 0, 32, 20, 3, height + 32, 0, 6, height + 32);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 4, 0, height + 42, gTrackColours[SCHEME_SUPPORTS]);

        switch (direction) {
        case 1:
            paint_util_push_tunnel_right(height, TUNNEL_9);
            break;
        case 2:
            paint_util_push_tunnel_left(height, TUNNEL_9);
            break;
        }
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    }
}

/** rct2: 0x008637D4 */
static void suspended_monorail_track_left_quarter_turn_3(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25860, 0, 0, 32, 20, 3, height + 32, 0, 6, height + 32);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25863, 0, 0, 32, 20, 3, height + 32, 0, 6, height + 32);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25866, 0, 0, 32, 20, 3, height + 32, 0, 6, height + 32);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25857, 0, 0, 32, 20, 3, height + 32, 0, 6, height + 32);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 4, 0, height + 42, gTrackColours[SCHEME_SUPPORTS]);

        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height, TUNNEL_9);
        }
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 1:
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25859, 0, 0, 16, 16, 3, height + 32, 16, 0, height + 32);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25862, 0, 0, 16, 16, 3, height + 32);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25865, 0, 0, 16, 16, 3, height + 32, 0, 16, height + 32);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25856, 0, 0, 16, 16, 3, height + 32, 16, 16, height + 32);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25858, 0, 0, 20, 32, 3, height + 32, 6, 0, height + 32);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25861, 0, 0, 20, 32, 3, height + 32, 6, 0, height + 32);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25864, 0, 0, 20, 32, 3, height + 32, 6, 0, height + 32);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25855, 0, 0, 20, 32, 3, height + 32, 6, 0, height + 32);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 4, 0, height + 42, gTrackColours[SCHEME_SUPPORTS]);

        switch (direction) {
        case 2:
            paint_util_push_tunnel_right(height, TUNNEL_9);
            break;
        case 3:
            paint_util_push_tunnel_left(height, TUNNEL_9);
            break;
        }
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    }
}

/** rct2: 0x008637E4 */
static void suspended_monorail_track_right_quarter_turn_3(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    suspended_monorail_track_left_quarter_turn_3(session, rideIndex, trackSequence, (direction - 1) & 3, height, mapElement);
}

/** rct2: 0x00863864 */
static void suspended_monorail_track_left_eighth_to_diag(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25903, 0, 0, 32, 20, 3, height + 32, 0, 6, height + 32);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25907, 0, 0, 32, 20, 3, height + 32, 0, 6, height + 32);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25911, 0, 0, 32, 20, 3, height + 32, 0, 6, height + 32);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25915, 0, 0, 32, 20, 3, height + 32, 0, 6, height + 32);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 4, 0, height + 42, gTrackColours[SCHEME_SUPPORTS]);

        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height, TUNNEL_9);
        }
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25904, 0, 0, 32, 16, 3, height + 32, 0, 0, height + 32);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25908, 0, 0, 34, 16, 3, height + 32, 0, 0, height + 32);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25912, 0, 0, 32, 16, 3, height + 32, 0, 16, height + 32);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25916, 0, 0, 32, 16, 3, height + 32, 0, 16, height + 32);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25905, 0, 0, 16, 16, 3, height + 32, 0, 16, height + 32);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25909, 0, 0, 16, 16, 3, height + 32, 16, 16, height + 32);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25913, 0, 0, 16, 16, 3, height + 32, 16, 0, height + 32);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25917, 0, 0, 16, 16, 3, height + 32, 0, 0, height + 32);
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
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25906, 0, 0, 16, 16, 3, height + 32, 16, 16, height + 32);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25910, 0, 0, 16, 18, 3, height + 32, 0, 16, height + 32);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25914, 0, 0, 16, 16, 3, height + 32, 0, 0, height + 32);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25918, 0, 0, 16, 16, 3, height + 32, 16, 0, height + 32);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        switch (direction) {
        case 0:
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 3, 0, height + 42, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 1, 0, height + 42, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 0, 0, height + 42, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 2, 0, height + 42, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }

        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    }
}

/** rct2: 0x00863874 */
static void suspended_monorail_track_right_eighth_to_diag(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25887, 0, 0, 32, 20, 3, height + 32, 0, 6, height + 32);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25891, 0, 0, 32, 20, 3, height + 32, 0, 6, height + 32);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25895, 0, 0, 32, 20, 3, height + 32, 0, 6, height + 32);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25899, 0, 0, 32, 20, 3, height + 32, 0, 6, height + 32);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 4, 0, height + 42, gTrackColours[SCHEME_SUPPORTS]);

        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height, TUNNEL_9);
        }
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25888, 0, 0, 32, 16, 3, height + 32, 0, 16, height + 32);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25892, 0, 0, 32, 16, 3, height + 32, 0, 16, height + 32);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25896, 0, 0, 34, 16, 3, height + 32, 0, 0, height + 32);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25900, 0, 0, 32, 16, 3, height + 32, 0, 0, height + 32);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25889, 0, 0, 16, 16, 3, height + 32, 0, 0, height + 32);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25893, 0, 0, 16, 16, 3, height + 32, 16, 0, height + 32);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25897, 0, 0, 28, 28, 3, height + 32, 4, 4, height + 32);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25901, 0, 0, 16, 16, 3, height + 32, 0, 16, height + 32);
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
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25890, 0, 0, 16, 16, 3, height + 32, 16, 0, height + 32);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25894, 0, 0, 16, 16, 3, height + 32, 0, 0, height + 32);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25898, 0, 0, 16, 18, 3, height + 32, 0, 16, height + 32);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25902, 0, 0, 16, 16, 3, height + 32, 16, 16, height + 32);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        switch (direction) {
        case 0:
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 1, 0, height + 42, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 0, 0, height + 42, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 2, 0, height + 42, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 3, 0, height + 42, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }

        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    }
}

/** rct2: 0x00863884 */
static void suspended_monorail_track_left_eighth_to_orthogonal(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    suspended_monorail_track_right_eighth_to_diag(session, rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x00863894 */
static void suspended_monorail_track_right_eighth_to_orthogonal(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    suspended_monorail_track_left_eighth_to_diag(session, rideIndex, trackSequence, (direction + 3) & 3, height, mapElement);
}

/** rct2: 0x008637F4 */
static void suspended_monorail_track_diag_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25936, -16, -16, 32, 32, 3, height + 32, -16, -16, height + 32);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25935, -16, -16, 32, 32, 3, height + 32, -16, -16, height + 32);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 2:
        switch (direction) {
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25935, -16, -16, 32, 32, 3, height + 32, -16, -16, height + 32);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 3:
        switch (direction) {
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25936, -16, -16, 32, 32, 3, height + 32, -16, -16, height + 32);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        switch (direction) {
        case 0:
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 1, 0, height + 42, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 0, 0, height + 42, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 2, 0, height + 42, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 3, 0, height + 42, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }

        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    }
}

/** rct2: 0x00863824 */
static void suspended_monorail_track_diag_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25960, -16, -16, 32, 32, 3, height + 32, -16, -16, height + 48);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25957, -16, -16, 32, 32, 3, height + 32, -16, -16, height + 48);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 2:
        switch (direction) {
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25959, -16, -16, 32, 32, 3, height + 32, -16, -16, height + 48);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 3:
        switch (direction) {
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25958, -16, -16, 32, 32, 3, height + 32, -16, -16, height + 48);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        switch (direction) {
        case 0:
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 1, 0, height + 55, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 0, 0, height + 55, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 2, 0, height + 55, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 3, 0, height + 57, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }

        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    }
}

/** rct2: 0x00863804 */
static void suspended_monorail_track_diag_flat_to_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25952, -16, -16, 32, 32, 3, height + 32, -16, -16, height + 40);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 64, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25949, -16, -16, 32, 32, 3, height + 32, -16, -16, height + 40);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 64, 0x20);
        break;
    case 2:
        switch (direction) {
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25951, -16, -16, 32, 32, 3, height + 32, -16, -16, height + 40);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 64, 0x20);
        break;
    case 3:
        switch (direction) {
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25950, -16, -16, 32, 32, 3, height + 32, -16, -16, height + 40);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        switch (direction) {
        case 0:
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 1, 0, height + 48, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 0, 0, height + 48, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 2, 0, height + 48, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 3, 0, height + 50, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }

        paint_util_set_general_support_height(height + 64, 0x20);
        break;
    }
}

/** rct2: 0x00863814 */
static void suspended_monorail_track_diag_25_deg_up_to_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25956, -16, -16, 32, 32, 3, height + 32, -16, -16, height + 40);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25953, -16, -16, 32, 32, 3, height + 32, -16, -16, height + 40);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 2:
        switch (direction) {
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25955, -16, -16, 32, 32, 3, height + 32, -16, -16, height + 40);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 3:
        switch (direction) {
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25954, -16, -16, 32, 32, 3, height + 32, -16, -16, height + 40);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        switch (direction) {
        case 0:
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 1, 0, height + 51, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 0, 0, height + 51, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 2, 0, height + 51, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 3, 0, height + 51, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }

        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    }
}

/** rct2: 0x00863854 */
static void suspended_monorail_track_diag_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25958, -16, -16, 32, 32, 3, height + 32, -16, -16, height + 48);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25959, -16, -16, 32, 32, 3, height + 32, -16, -16, height + 48);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 2:
        switch (direction) {
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25957, -16, -16, 32, 32, 3, height + 32, -16, -16, height + 48);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 3:
        switch (direction) {
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25960, -16, -16, 32, 32, 3, height + 32, -16, -16, height + 48);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        switch (direction) {
        case 0:
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 1, 0, height + 54, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 0, 0, height + 54, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 2, 0, height + 51, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 3, 0, height + 58, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }

        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    }
}

/** rct2: 0x00863834 */
static void suspended_monorail_track_diag_flat_to_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25954, -16, -16, 32, 32, 3, height + 32, -16, -16, height + 40);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25955, -16, -16, 32, 32, 3, height + 32, -16, -16, height + 40);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        break;
    case 2:
        switch (direction) {
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25953, -16, -16, 32, 32, 3, height + 32, -16, -16, height + 40);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        break;
    case 3:
        switch (direction) {
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25956, -16, -16, 32, 32, 3, height + 32, -16, -16, height + 40);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        switch (direction) {
        case 0:
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 1, 0, height + 48, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 0, 0, height + 48, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 2, 0, height + 48, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 3, 0, height + 48, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }
        break;
    }

    paint_util_set_general_support_height(height + 72, 0x20);
}

/** rct2: 0x00863844 */
static void suspended_monorail_track_diag_25_deg_down_to_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25950, -16, -16, 32, 32, 3, height + 32, -16, -16, height + 40);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 64, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25951, -16, -16, 32, 32, 3, height + 32, -16, -16, height + 40);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 64, 0x20);
        break;
    case 2:
        switch (direction) {
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25949, -16, -16, 32, 32, 3, height + 32, -16, -16, height + 40);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 64, 0x20);
        break;
    case 3:
        switch (direction) {
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 25952, -16, -16, 32, 32, 3, height + 32, -16, -16, height + 40);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        switch (direction) {
        case 0:
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 1, 0, height + 46, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 0, 0, height + 46, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 2, 0, height + 46, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 3, 0, height + 46, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }

        paint_util_set_general_support_height(height + 64, 0x20);
        break;
    }
}

TRACK_PAINT_FUNCTION get_track_paint_function_suspended_monorail(sint32 trackType, sint32 direction)
{
    switch (trackType) {
    case TRACK_ELEM_FLAT:
        return suspended_monorail_track_flat;
    case TRACK_ELEM_END_STATION:
    case TRACK_ELEM_BEGIN_STATION:
    case TRACK_ELEM_MIDDLE_STATION:
        return suspended_monorail_track_station;
    case TRACK_ELEM_25_DEG_UP:
        return suspended_monorail_track_25_deg_up;
    case TRACK_ELEM_FLAT_TO_25_DEG_UP:
        return suspended_monorail_track_flat_to_25_deg_up;
    case TRACK_ELEM_25_DEG_UP_TO_FLAT:
        return suspended_monorail_track_25_deg_up_to_flat;
    case TRACK_ELEM_25_DEG_DOWN:
        return suspended_monorail_track_25_deg_down;
    case TRACK_ELEM_FLAT_TO_25_DEG_DOWN:
        return suspended_monorail_track_flat_to_25_deg_down;
    case TRACK_ELEM_25_DEG_DOWN_TO_FLAT:
        return suspended_monorail_track_25_deg_down_to_flat;
    case TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES:
        return suspended_monorail_track_left_quarter_turn_5;
    case TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES:
        return suspended_monorail_track_right_quarter_turn_5;
    case TRACK_ELEM_S_BEND_LEFT:
        return suspended_monorail_track_s_bend_left;
    case TRACK_ELEM_S_BEND_RIGHT:
        return suspended_monorail_track_s_bend_right;
    case TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES:
        return suspended_monorail_track_left_quarter_turn_3;
    case TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES:
        return suspended_monorail_track_right_quarter_turn_3;
    case TRACK_ELEM_LEFT_EIGHTH_TO_DIAG:
        return suspended_monorail_track_left_eighth_to_diag;
    case TRACK_ELEM_RIGHT_EIGHTH_TO_DIAG:
        return suspended_monorail_track_right_eighth_to_diag;
    case TRACK_ELEM_LEFT_EIGHTH_TO_ORTHOGONAL:
        return suspended_monorail_track_left_eighth_to_orthogonal;
    case TRACK_ELEM_RIGHT_EIGHTH_TO_ORTHOGONAL:
        return suspended_monorail_track_right_eighth_to_orthogonal;
    case TRACK_ELEM_DIAG_FLAT:
        return suspended_monorail_track_diag_flat;
    case TRACK_ELEM_DIAG_25_DEG_UP:
        return suspended_monorail_track_diag_25_deg_up;
    case TRACK_ELEM_DIAG_FLAT_TO_25_DEG_UP:
        return suspended_monorail_track_diag_flat_to_25_deg_up;
    case TRACK_ELEM_DIAG_25_DEG_UP_TO_FLAT:
        return suspended_monorail_track_diag_25_deg_up_to_flat;
    case TRACK_ELEM_DIAG_25_DEG_DOWN:
        return suspended_monorail_track_diag_25_deg_down;
    case TRACK_ELEM_DIAG_FLAT_TO_25_DEG_DOWN:
        return suspended_monorail_track_diag_flat_to_25_deg_down;
    case TRACK_ELEM_DIAG_25_DEG_DOWN_TO_FLAT:
        return suspended_monorail_track_diag_25_deg_down_to_flat;
    }
    return NULL;
}
