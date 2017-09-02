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

/** rct2: 0x008AFE9C */
static void mini_suspended_rc_track_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    if (track_element_is_lift_hill(mapElement)) {
        switch (direction) {
        case 0:
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28433, 0, 6, 32, 20, 1, height + 24);
            break;
        case 1:
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28434, 0, 6, 32, 20, 1, height + 24);
            break;
        }
    } else {
        switch (direction) {
        case 0:
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28383, 0, 6, 32, 20, 1, height + 24);
            break;
        case 1:
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28384, 0, 6, 32, 20, 1, height + 24);
            break;
        }
    }

    paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    if (track_paint_util_should_paint_supports(gPaintSession.MapPosition)) {
        switch (direction) {
        case 0:
        case 2:
            metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 4, 0, height + 30, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
        case 3:
            metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 4, 0, height + 30, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }
    }

    paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
    paint_util_set_general_support_height(height + 48, 0x20);
}

/** rct2: 0x008AFF4C, 0x008AFF5C, 0x008AFF6C */
static void mini_suspended_rc_track_station(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    static const uint32 imageIds[4][3] = {
        { SPR_STATION_BASE_C_SW_NE, 28433, SPR_STATION_INVERTED_BAR_E_SW_NE },
        { SPR_STATION_BASE_C_NW_SE, 28434, SPR_STATION_INVERTED_BAR_E_NW_SE },
        { SPR_STATION_BASE_C_SW_NE, 28433, SPR_STATION_INVERTED_BAR_E_SW_NE },
        { SPR_STATION_BASE_C_NW_SE, 28434, SPR_STATION_INVERTED_BAR_E_NW_SE },
    };

    sub_98197C_rotated(direction, imageIds[direction][0] | gTrackColours[SCHEME_MISC], 0, 0, 32, 28, 1, height, 0, 2, height);
    sub_98196C_rotated(direction, imageIds[direction][1] | gTrackColours[SCHEME_TRACK], 0, 6, 32, 20, 1, height + 24);
    sub_98199C_rotated(direction, imageIds[direction][2] | gTrackColours[SCHEME_SUPPORTS], 0, 6, 32, 20, 1, height + 24, 0, 2, height);
    track_paint_util_draw_station_metal_supports_2(direction, height, gTrackColours[SCHEME_SUPPORTS], 3);
    track_paint_util_draw_station_inverted(rideIndex, trackSequence, direction, height, mapElement, STATION_VARIANT_1);
    paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);
    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 32, 0x20);
}

/** rct2: 0x008AFEAC */
static void mini_suspended_rc_track_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    if (track_element_is_lift_hill(mapElement)) {
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28435, 0, 6, 32, 20, 1, height + 24, 0, 6, height + 40);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28441, 0, 6, 32, 20, 1, height + 24, 0, 6, height + 40);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28437, 0, 6, 32, 20, 1, height + 24, 0, 6, height + 40);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28442, 0, 6, 32, 20, 1, height + 24, 0, 6, height + 40);
            break;
        }
    } else {
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28385, 0, 6, 32, 20, 1, height + 24, 0, 6, height + 40);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28391, 0, 6, 32, 20, 1, height + 24, 0, 6, height + 40);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28387, 0, 6, 32, 20, 1, height + 24, 0, 6, height + 40);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28392, 0, 6, 32, 20, 1, height + 24, 0, 6, height + 40);
            break;
        }
    }

    paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    if (track_paint_util_should_paint_supports(gPaintSession.MapPosition)) {
        switch (direction) {
        case 0:
            metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 6, 0, height + 46, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 8, 0, height + 46, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 7, 0, height + 46, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 5, 0, height + 46, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }
    }

    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_1);
    } else {
        paint_util_push_tunnel_rotated(direction, height + 8, TUNNEL_2);
    }
    paint_util_set_general_support_height(height + 72, 0x20);
}

/** rct2: 0x008AFEBC */
static void mini_suspended_rc_track_flat_to_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    if (track_element_is_lift_hill(mapElement)) {
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28436, 0, 6, 32, 20, 1, height + 24, 0, 6, height + 32);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28443, 0, 6, 32, 20, 1, height + 24, 0, 6, height + 32);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28438, 0, 6, 32, 20, 1, height + 24, 0, 6, height + 32);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28444, 0, 6, 32, 20, 1, height + 24, 0, 6, height + 32);
            break;
        }
    } else {
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28386, 0, 6, 32, 20, 1, height + 24, 0, 6, height + 32);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28393, 0, 6, 32, 20, 1, height + 24, 0, 6, height + 32);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28388, 0, 6, 32, 20, 1, height + 24, 0, 6, height + 32);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28394, 0, 6, 32, 20, 1, height + 24, 0, 6, height + 32);
            break;
        }
    }

    paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    if (track_paint_util_should_paint_supports(gPaintSession.MapPosition)) {
        switch (direction) {
        case 0:
            metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 6, 0, height + 38, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 8, 0, height + 38, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 7, 0, height + 38, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 5, 0, height + 38, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }
    }

    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
    } else {
        paint_util_push_tunnel_rotated(direction, height, TUNNEL_2);
    }
    paint_util_set_general_support_height(height + 64, 0x20);
}

/** rct2: 0x008AFECC */
static void mini_suspended_rc_track_25_deg_up_to_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    if (track_element_is_lift_hill(mapElement)) {
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28440, 0, 6, 32, 20, 1, height + 24, 0, 6, height + 32);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28446, 0, 6, 32, 20, 1, height + 24, 0, 6, height + 32);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28439, 0, 6, 32, 20, 1, height + 24, 0, 6, height + 32);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28445, 0, 6, 32, 20, 1, height + 24, 0, 6, height + 32);
            break;
        }
    } else {
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28390, 0, 6, 32, 20, 1, height + 24, 0, 6, height + 32);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28396, 0, 6, 32, 20, 1, height + 24, 0, 6, height + 32);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28389, 0, 6, 32, 20, 1, height + 24, 0, 6, height + 32);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28395, 0, 6, 32, 20, 1, height + 24, 0, 6, height + 32);
            break;
        }
    }

    paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    if (track_paint_util_should_paint_supports(gPaintSession.MapPosition)) {
        switch (direction) {
        case 0:
            metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 6, 0, height + 38, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 8, 0, height + 38, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 7, 0, height + 38, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 5, 0, height + 38, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }
    }

    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_0);
    } else {
        paint_util_push_tunnel_rotated(direction, height + 8, TUNNEL_12);
    }
    paint_util_set_general_support_height(height + 56, 0x20);
}

/** rct2: 0x008AFEDC */
static void mini_suspended_rc_track_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    mini_suspended_rc_track_25_deg_up(session, rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008AFEEC */
static void mini_suspended_rc_track_flat_to_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    mini_suspended_rc_track_25_deg_up_to_flat(session, rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008AFEFC */
static void mini_suspended_rc_track_25_deg_down_to_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    mini_suspended_rc_track_flat_to_25_deg_up(session, rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008AFF0C */
static void mini_suspended_rc_track_left_quarter_turn_5(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28406, 0, 6, 32, 20, 1, height + 24);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28411, 0, 6, 32, 20, 1, height + 24);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28416, 0, 6, 32, 20, 1, height + 24);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28401, 0, 6, 32, 20, 1, height + 24);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        switch (direction) {
        case 0:
        case 2:
            metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 4, 0, height + 30, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
        case 3:
            metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 4, 0, height + 30, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }

        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
        }
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 1:
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28405, 0, 0, 32, 16, 1, height + 24);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28410, 0, 0, 32, 16, 1, height + 24);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28415, 0, 16, 32, 16, 1, height + 24);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28400, 0, 16, 32, 16, 1, height + 24);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28404, 0, 16, 16, 16, 1, height + 24);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28409, 16, 16, 16, 16, 1, height + 24);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28414, 16, 0, 16, 16, 1, height + 24);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28399, 0, 0, 16, 16, 1, height + 24);
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
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28403, 16, 0, 16, 32, 1, height + 24);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28408, 0, 0, 16, 32, 1, height + 24);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28413, 0, 0, 16, 32, 1, height + 24);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28398, 16, 0, 16, 32, 1, height + 24);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 6:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28402, 6, 0, 20, 32, 1, height + 24);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28407, 6, 0, 20, 32, 1, height + 24);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28412, 6, 0, 20, 32, 1, height + 24);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28397, 6, 0, 20, 32, 1, height + 24);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        switch (direction) {
        case 0:
        case 2:
            metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 4, 0, height + 30, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
        case 3:
            metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 4, 0, height + 30, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }

        switch (direction) {
        case 2:
            paint_util_push_tunnel_right(height, TUNNEL_0);
            break;
        case 3:
            paint_util_push_tunnel_left(height, TUNNEL_0);
            break;
        }
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    }
}

/** rct2: 0x008AFF1C */
static void mini_suspended_rc_track_right_quarter_turn_5(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    mini_suspended_rc_track_left_quarter_turn_5(session, rideIndex, trackSequence, (direction - 1) & 3, height, mapElement);
}

/** rct2: 0x008AFF2C */
static void mini_suspended_rc_track_s_bend_left(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28421, 0, 6, 32, 20, 1, height + 24);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28428, 0, 6, 32, 20, 1, height + 24);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28424, 0, 6, 32, 20, 1, height + 24);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28425, 0, 6, 32, 20, 1, height + 24);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        switch (direction) {
        case 0:
        case 2:
            metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 4, 0, height + 30, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
        case 3:
            metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 4, 0, height + 30, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }

        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
        }
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28422, 0, 0, 32, 26, 1, height + 24);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28427, 0, 0, 32, 26, 1, height + 24);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28423, 0, 6, 32, 26, 1, height + 24);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28426, 0, 6, 32, 26, 1, height + 24);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        switch (direction) {
        case 0:
            metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 5, 0, height + 30, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 6, 0, height + 30, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }

        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28423, 0, 6, 32, 26, 1, height + 24);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28426, 0, 6, 32, 26, 1, height + 24);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28422, 0, 0, 32, 26, 1, height + 24);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28427, 0, 0, 32, 26, 1, height + 24);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        switch (direction) {
        case 2:
            metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 5, 0, height + 30, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 6, 0, height + 30, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }

        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28424, 0, 6, 32, 20, 1, height + 24);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28425, 0, 6, 32, 20, 1, height + 24);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28421, 0, 6, 32, 20, 1, height + 24);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28428, 0, 6, 32, 20, 1, height + 24);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        switch (direction) {
        case 0:
        case 2:
            metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 4, 0, height + 30, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
        case 3:
            metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 4, 0, height + 30, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }

        switch (direction) {
        case 1:
            paint_util_push_tunnel_right(height, TUNNEL_0);
            break;
        case 2:
            paint_util_push_tunnel_left(height, TUNNEL_0);
            break;
        }
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    }
}

/** rct2: 0x008AFF3C */
static void mini_suspended_rc_track_s_bend_right(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28417, 0, 6, 32, 20, 1, height + 24);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28432, 0, 6, 32, 20, 1, height + 24);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28420, 0, 6, 32, 20, 1, height + 24);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28429, 0, 6, 32, 20, 1, height + 24);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        switch (direction) {
        case 0:
        case 2:
            metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 4, 0, height + 30, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
        case 3:
            metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 4, 0, height + 30, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }

        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
        }
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28418, 0, 6, 32, 26, 1, height + 24);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28431, 0, 6, 32, 26, 1, height + 24);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28419, 0, 0, 32, 26, 1, height + 24);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28430, 0, 0, 32, 26, 1, height + 24);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        switch (direction) {
        case 0:
            metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 8, 0, height + 30, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 7, 0, height + 30, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }

        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28419, 0, 0, 32, 26, 1, height + 24);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28430, 0, 0, 32, 26, 1, height + 24);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28418, 0, 6, 32, 26, 1, height + 24);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28431, 0, 6, 32, 26, 1, height + 24);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        switch (direction) {
        case 2:
            metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 8, 0, height + 30, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 7, 0, height + 30, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }

        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28420, 0, 6, 32, 20, 1, height + 24);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28429, 0, 6, 32, 20, 1, height + 24);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28417, 0, 6, 32, 20, 1, height + 24);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28432, 0, 6, 32, 20, 1, height + 24);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        switch (direction) {
        case 0:
        case 2:
            metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 4, 0, height + 30, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
        case 3:
            metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 4, 0, height + 30, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }

        switch (direction) {
        case 1:
            paint_util_push_tunnel_right(height, TUNNEL_0);
            break;
        case 2:
            paint_util_push_tunnel_left(height, TUNNEL_0);
            break;
        }
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    }
}

/** rct2: 0x008AFF7C */
static void mini_suspended_rc_track_left_quarter_turn_3(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28452, 0, 6, 32, 20, 3, height + 24);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28455, 0, 6, 32, 20, 3, height + 24);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28458, 0, 6, 32, 20, 3, height + 24);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28449, 0, 6, 32, 20, 3, height + 24);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        switch (direction) {
        case 0:
        case 2:
            metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 4, 0, height + 30, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
        case 3:
            metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 4, 0, height + 30, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }

        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
        }
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 1:
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28451, 16, 0, 16, 16, 3, height + 24);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28454, 0, 0, 16, 16, 3, height + 24);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28457, 0, 16, 16, 16, 3, height + 24);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28448, 16, 16, 16, 16, 3, height + 24);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28450, 6, 0, 20, 32, 3, height + 24);
            break;
        case 1:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28453, 6, 0, 20, 32, 3, height + 24);
            break;
        case 2:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28456, 6, 0, 20, 32, 3, height + 24);
            break;
        case 3:
            sub_98196C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28447, 6, 0, 20, 32, 3, height + 24);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        switch (direction) {
        case 0:
        case 2:
            metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 4, 0, height + 30, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
        case 3:
            metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 4, 0, height + 30, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }

        switch (direction) {
        case 2:
            paint_util_push_tunnel_right(height, TUNNEL_0);
            break;
        case 3:
            paint_util_push_tunnel_left(height, TUNNEL_0);
            break;
        }
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    }
}

/** rct2: 0x008AFF8C */
static void mini_suspended_rc_track_right_quarter_turn_3(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    mini_suspended_rc_track_left_quarter_turn_3(session, rideIndex, trackSequence, (direction - 1) & 3, height, mapElement);
}

/** rct2: 0x008AFFAC */
static void mini_suspended_rc_track_left_eighth_to_diag(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28475, 0, 0, 32, 20, 1, height + 24, 0, 6, height + 24);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28479, 0, 0, 32, 20, 1, height + 24, 0, 6, height + 24);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28483, 0, 0, 32, 20, 1, height + 24, 0, 6, height + 24);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28487, 0, 0, 32, 20, 1, height + 24, 0, 6, height + 24);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        switch (direction) {
        case 0:
        case 2:
            metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 4, 0, height + 30, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
        case 3:
            metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 4, 0, height + 30, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }

        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
        }
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28476, 0, 0, 32, 16, 1, height + 24, 0, 0, height + 24);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28480, 0, 0, 34, 16, 1, height + 24, 0, 0, height + 24);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28484, 0, 0, 32, 16, 1, height + 24, 0, 16, height + 24);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28488, 0, 0, 32, 16, 1, height + 24, 0, 16, height + 24);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28477, 0, 0, 16, 16, 1, height + 24, 0, 16, height + 24);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28481, 0, 0, 16, 16, 1, height + 24, 16, 16, height + 24);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28485, 0, 0, 16, 16, 1, height + 24, 16, 0, height + 24);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28489, 0, 0, 16, 16, 1, height + 24, 0, 0, height + 24);
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
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28478, 0, 0, 16, 16, 1, height + 24, 16, 16, height + 24);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28482, 0, 0, 16, 18, 1, height + 24, 0, 16, height + 24);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28486, 0, 0, 16, 16, 1, height + 24, 0, 0, height + 24);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28490, 0, 0, 16, 16, 1, height + 24, 16, 0, height + 24);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        switch (direction) {
        case 0:
            metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 3, 0, height + 30, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 1, 0, height + 30, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 0, 0, height + 30, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 2, 0, height + 30, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }

        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    }
}

/** rct2: 0x008AFFBC */
static void mini_suspended_rc_track_right_eighth_to_diag(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28459, 0, 0, 32, 20, 1, height + 24, 0, 6, height + 24);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28463, 0, 0, 32, 20, 1, height + 24, 0, 6, height + 24);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28467, 0, 0, 32, 20, 1, height + 24, 0, 6, height + 24);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28471, 0, 0, 32, 20, 1, height + 24, 0, 6, height + 24);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        switch (direction) {
        case 0:
        case 2:
            metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 4, 0, height + 30, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
        case 3:
            metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 4, 0, height + 30, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }

        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
        }
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28460, 0, 0, 32, 16, 1, height + 24, 0, 16, height + 24);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28464, 0, 0, 32, 16, 1, height + 24, 0, 16, height + 24);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28468, 0, 0, 34, 16, 1, height + 24, 0, 0, height + 24);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28472, 0, 0, 32, 16, 1, height + 24, 0, 0, height + 24);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28461, 0, 0, 16, 16, 1, height + 24, 0, 0, height + 24);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28465, 0, 0, 16, 16, 1, height + 24, 16, 0, height + 24);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28469, 0, 0, 28, 28, 1, height + 24, 4, 2, height + 24);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28473, 0, 0, 16, 16, 1, height + 24, 0, 16, height + 24);
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
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28462, 0, 0, 16, 16, 1, height + 24, 16, 0, height + 24);
            break;
        case 1:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28466, 0, 0, 16, 16, 1, height + 24, 0, 0, height + 24);
            break;
        case 2:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28470, 0, 0, 16, 18, 1, height + 24, 0, 16, height + 24);
            break;
        case 3:
            sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28474, 0, 0, 16, 16, 1, height + 24, 16, 16, height + 24);
            break;
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        switch (direction) {
        case 0:
            metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 1, 0, height + 30, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 0, 0, height + 30, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 2, 0, height + 30, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 3, 0, height + 30, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }

        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    }
}

/** rct2: 0x008AFFCC */
static void mini_suspended_rc_track_left_eighth_to_orthogonal(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    mini_suspended_rc_track_right_eighth_to_diag(session, rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008AFFDC */
static void mini_suspended_rc_track_right_eighth_to_orthogonal(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    mini_suspended_rc_track_left_eighth_to_diag(session, rideIndex, trackSequence, (direction + 3) & 3, height, mapElement);
}

/** rct2: 0x008AFF9C */
static void mini_suspended_rc_track_diag_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 3:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28510, -16, -16, 32, 32, 1, height + 24, -16, -16, height + 24);
                break;
            }
        } else {
            switch (direction) {
            case 3:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28494, -16, -16, 32, 32, 1, height + 24, -16, -16, height + 24);
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
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28507, -16, -16, 32, 32, 1, height + 24, -16, -16, height + 24);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28491, -16, -16, 32, 32, 1, height + 24, -16, -16, height + 24);
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
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28509, -16, -16, 32, 32, 1, height + 24, -16, -16, height + 24);
                break;
            }
        } else {
            switch (direction) {
            case 2:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28493, -16, -16, 32, 32, 1, height + 24, -16, -16, height + 24);
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
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28508, -16, -16, 32, 32, 1, height + 24, -16, -16, height + 24);
                break;
            }
        } else {
            switch (direction) {
            case 1:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28492, -16, -16, 32, 32, 1, height + 24, -16, -16, height + 24);
                break;
            }
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        switch (direction) {
        case 0:
            metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 1, 0, height + 30, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 0, 0, height + 30, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 2, 0, height + 30, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 3, 0, height + 30, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }

        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    }
}

/** rct2: 0x008B000C */
static void mini_suspended_rc_track_diag_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 3:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28522, -16, -16, 32, 32, 1, height + 24, -16, -16, height + 40);
                break;
            }
        } else {
            switch (direction) {
            case 3:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28506, -16, -16, 32, 32, 1, height + 24, -16, -16, height + 40);
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
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28519, -16, -16, 32, 32, 1, height + 24, -16, -16, height + 40);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28503, -16, -16, 32, 32, 1, height + 24, -16, -16, height + 40);
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
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28521, -16, -16, 32, 32, 1, height + 24, -16, -16, height + 40);
                break;
            }
        } else {
            switch (direction) {
            case 2:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28505, -16, -16, 32, 32, 1, height + 24, -16, -16, height + 40);
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
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28520, -16, -16, 32, 32, 1, height + 24, -16, -16, height + 40);
                break;
            }
        } else {
            switch (direction) {
            case 1:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28504, -16, -16, 32, 32, 1, height + 24, -16, -16, height + 40);
                break;
            }
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        switch (direction) {
        case 0:
            metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 1, 0, height + 42, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 0, 0, height + 42, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 2, 0, height + 42, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 3, 0, height + 42, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }

        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    }
}

/** rct2: 0x008AFFEC */
static void mini_suspended_rc_track_diag_flat_to_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 3:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28514, -16, -16, 32, 32, 1, height + 24, -16, -16, height + 32);
                break;
            }
        } else {
            switch (direction) {
            case 3:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28498, -16, -16, 32, 32, 1, height + 24, -16, -16, height + 32);
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
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28511, -16, -16, 32, 32, 1, height + 24, -16, -16, height + 32);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28495, -16, -16, 32, 32, 1, height + 24, -16, -16, height + 32);
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
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28513, -16, -16, 32, 32, 1, height + 24, -16, -16, height + 32);
                break;
            }
        } else {
            switch (direction) {
            case 2:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28497, -16, -16, 32, 32, 1, height + 24, -16, -16, height + 32);
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
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28512, -16, -16, 32, 32, 1, height + 24, -16, -16, height + 32);
                break;
            }
        } else {
            switch (direction) {
            case 1:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28496, -16, -16, 32, 32, 1, height + 24, -16, -16, height + 32);
                break;
            }
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        switch (direction) {
        case 0:
            metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 1, 0, height + 34, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 0, 0, height + 34, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 2, 0, height + 34, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 3, 0, height + 34, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }

        paint_util_set_general_support_height(height + 64, 0x20);
        break;
    }
}

/** rct2: 0x008AFFFC */
static void mini_suspended_rc_track_diag_25_deg_up_to_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 3:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28518, -16, -16, 32, 32, 1, height + 24, -16, -16, height + 32);
                break;
            }
        } else {
            switch (direction) {
            case 3:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28502, -16, -16, 32, 32, 1, height + 24, -16, -16, height + 32);
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
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28515, -16, -16, 32, 32, 1, height + 24, -16, -16, height + 32);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28499, -16, -16, 32, 32, 1, height + 24, -16, -16, height + 32);
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
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28517, -16, -16, 32, 32, 1, height + 24, -16, -16, height + 32);
                break;
            }
        } else {
            switch (direction) {
            case 2:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28501, -16, -16, 32, 32, 1, height + 24, -16, -16, height + 32);
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
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28516, -16, -16, 32, 32, 1, height + 24, -16, -16, height + 32);
                break;
            }
        } else {
            switch (direction) {
            case 1:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28500, -16, -16, 32, 32, 1, height + 24, -16, -16, height + 32);
                break;
            }
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        switch (direction) {
        case 0:
            metal_b_supports_paint_setup(METAL_SUPPORTS_FORK, 1, 0, height + 36, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 0, 0, height + 36, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 2, 0, height + 36, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 3, 0, height + 36, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }

        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    }
}

/** rct2: 0x008B003C */
static void mini_suspended_rc_track_diag_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 3:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28520, -16, -16, 32, 32, 1, height + 24, -16, -16, height + 40);
                break;
            }
        } else {
            switch (direction) {
            case 3:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28504, -16, -16, 32, 32, 1, height + 24, -16, -16, height + 40);
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
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28521, -16, -16, 32, 32, 1, height + 24, -16, -16, height + 40);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28505, -16, -16, 32, 32, 1, height + 24, -16, -16, height + 40);
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
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28519, -16, -16, 32, 32, 1, height + 24, -16, -16, height + 40);
                break;
            }
        } else {
            switch (direction) {
            case 2:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28503, -16, -16, 32, 32, 1, height + 24, -16, -16, height + 40);
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
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28522, -16, -16, 32, 32, 1, height + 24, -16, -16, height + 40);
                break;
            }
        } else {
            switch (direction) {
            case 1:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28506, -16, -16, 32, 32, 1, height + 24, -16, -16, height + 40);
                break;
            }
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        switch (direction) {
        case 0:
            metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 1, 0, height + 42, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 0, 0, height + 42, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 2, 0, height + 42, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 3, 0, height + 42, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }

        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    }
}

/** rct2: 0x008B001C */
static void mini_suspended_rc_track_diag_flat_to_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 3:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28516, -16, -16, 32, 32, 1, height + 24, -16, -16, height + 32);
                break;
            }
        } else {
            switch (direction) {
            case 3:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28500, -16, -16, 32, 32, 1, height + 24, -16, -16, height + 32);
                break;
            }
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        break;
    case 1:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 0:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28517, -16, -16, 32, 32, 1, height + 24, -16, -16, height + 32);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28501, -16, -16, 32, 32, 1, height + 24, -16, -16, height + 32);
                break;
            }
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        break;
    case 2:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 2:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28515, -16, -16, 32, 32, 1, height + 24, -16, -16, height + 32);
                break;
            }
        } else {
            switch (direction) {
            case 2:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28499, -16, -16, 32, 32, 1, height + 24, -16, -16, height + 32);
                break;
            }
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        break;
    case 3:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 1:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28518, -16, -16, 32, 32, 1, height + 24, -16, -16, height + 32);
                break;
            }
        } else {
            switch (direction) {
            case 1:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28502, -16, -16, 32, 32, 1, height + 24, -16, -16, height + 32);
                break;
            }
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        switch (direction) {
        case 0:
            metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 1, 0, height + 36, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 0, 0, height + 36, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 2, 0, height + 36, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 3, 0, height + 36, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }
        break;
    }

    paint_util_set_general_support_height(height + 72, 0x20);
}

/** rct2: 0x008B002C */
static void mini_suspended_rc_track_diag_25_deg_down_to_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 3:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28512, -16, -16, 32, 32, 1, height + 24, -16, -16, height + 32);
                break;
            }
        } else {
            switch (direction) {
            case 3:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28496, -16, -16, 32, 32, 1, height + 24, -16, -16, height + 32);
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
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28513, -16, -16, 32, 32, 1, height + 24, -16, -16, height + 32);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28497, -16, -16, 32, 32, 1, height + 24, -16, -16, height + 32);
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
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28511, -16, -16, 32, 32, 1, height + 24, -16, -16, height + 32);
                break;
            }
        } else {
            switch (direction) {
            case 2:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28495, -16, -16, 32, 32, 1, height + 24, -16, -16, height + 32);
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
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28514, -16, -16, 32, 32, 1, height + 24, -16, -16, height + 32);
                break;
            }
        } else {
            switch (direction) {
            case 1:
                sub_98197C_rotated(direction, gTrackColours[SCHEME_TRACK] | 28498, -16, -16, 32, 32, 1, height + 24, -16, -16, height + 32);
                break;
            }
        }

        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        switch (direction) {
        case 0:
            metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 1, 0, height + 34, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 0, 0, height + 34, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            metal_a_supports_paint_setup(METAL_SUPPORTS_FORK, 2, 0, height + 34, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            metal_a_supports_paint_setup(METAL_SUPPORTS_FORK_ALT, 3, 0, height + 34, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }

        paint_util_set_general_support_height(height + 64, 0x20);
        break;
    }
}

TRACK_PAINT_FUNCTION get_track_paint_function_mini_suspended_rc(sint32 trackType, sint32 direction)
{
    switch (trackType) {
    case TRACK_ELEM_FLAT:
        return mini_suspended_rc_track_flat;
    case TRACK_ELEM_END_STATION:
    case TRACK_ELEM_BEGIN_STATION:
    case TRACK_ELEM_MIDDLE_STATION:
        return mini_suspended_rc_track_station;
    case TRACK_ELEM_25_DEG_UP:
        return mini_suspended_rc_track_25_deg_up;
    case TRACK_ELEM_FLAT_TO_25_DEG_UP:
        return mini_suspended_rc_track_flat_to_25_deg_up;
    case TRACK_ELEM_25_DEG_UP_TO_FLAT:
        return mini_suspended_rc_track_25_deg_up_to_flat;
    case TRACK_ELEM_25_DEG_DOWN:
        return mini_suspended_rc_track_25_deg_down;
    case TRACK_ELEM_FLAT_TO_25_DEG_DOWN:
        return mini_suspended_rc_track_flat_to_25_deg_down;
    case TRACK_ELEM_25_DEG_DOWN_TO_FLAT:
        return mini_suspended_rc_track_25_deg_down_to_flat;
    case TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES:
        return mini_suspended_rc_track_left_quarter_turn_5;
    case TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES:
        return mini_suspended_rc_track_right_quarter_turn_5;
    case TRACK_ELEM_S_BEND_LEFT:
        return mini_suspended_rc_track_s_bend_left;
    case TRACK_ELEM_S_BEND_RIGHT:
        return mini_suspended_rc_track_s_bend_right;
    case TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES:
        return mini_suspended_rc_track_left_quarter_turn_3;
    case TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES:
        return mini_suspended_rc_track_right_quarter_turn_3;
    case TRACK_ELEM_LEFT_EIGHTH_TO_DIAG:
        return mini_suspended_rc_track_left_eighth_to_diag;
    case TRACK_ELEM_RIGHT_EIGHTH_TO_DIAG:
        return mini_suspended_rc_track_right_eighth_to_diag;
    case TRACK_ELEM_LEFT_EIGHTH_TO_ORTHOGONAL:
        return mini_suspended_rc_track_left_eighth_to_orthogonal;
    case TRACK_ELEM_RIGHT_EIGHTH_TO_ORTHOGONAL:
        return mini_suspended_rc_track_right_eighth_to_orthogonal;
    case TRACK_ELEM_DIAG_FLAT:
        return mini_suspended_rc_track_diag_flat;
    case TRACK_ELEM_DIAG_25_DEG_UP:
        return mini_suspended_rc_track_diag_25_deg_up;
    case TRACK_ELEM_DIAG_FLAT_TO_25_DEG_UP:
        return mini_suspended_rc_track_diag_flat_to_25_deg_up;
    case TRACK_ELEM_DIAG_25_DEG_UP_TO_FLAT:
        return mini_suspended_rc_track_diag_25_deg_up_to_flat;
    case TRACK_ELEM_DIAG_25_DEG_DOWN:
        return mini_suspended_rc_track_diag_25_deg_down;
    case TRACK_ELEM_DIAG_FLAT_TO_25_DEG_DOWN:
        return mini_suspended_rc_track_diag_flat_to_25_deg_down;
    case TRACK_ELEM_DIAG_25_DEG_DOWN_TO_FLAT:
        return mini_suspended_rc_track_diag_25_deg_down_to_flat;
    }
    return NULL;
}
