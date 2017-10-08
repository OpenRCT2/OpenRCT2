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

/** rct2: 0x00890CB4 */
static void inverted_hairpin_rc_track_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    if (track_element_is_lift_hill(mapElement)) {
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17030, 0, 0, 32, 20, 1, height + 24, 0, 6, height + 22);
            break;
        case 1:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17031, 0, 0, 32, 20, 1, height + 24, 0, 6, height + 22);
            break;
        case 2:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17032, 0, 0, 32, 20, 1, height + 24, 0, 6, height + 22);
            break;
        case 3:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17033, 0, 0, 32, 20, 1, height + 24, 0, 6, height + 22);
            break;
        }
    } else {
        switch (direction) {
        case 0:
        case 2:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17026, 0, 0, 32, 20, 1, height + 24, 0, 6, height + 22);
            break;
        case 1:
        case 3:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17027, 0, 0, 32, 20, 1, height + 24, 0, 6, height + 22);
            break;
        }
    }

    paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    if (track_paint_util_should_paint_supports(session->MapPosition)) {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 30, session->TrackColours[SCHEME_SUPPORTS]);
    }

    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x00890D84, 0x00890D94, 0x00890DA4 */
static void inverted_hairpin_rc_track_station(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    static const uint32 imageIds[4][3] = {
        { SPR_STATION_BASE_C_SW_NE, 17028, SPR_STATION_INVERTED_BAR_0_SW_NE },
        { SPR_STATION_BASE_C_NW_SE, 17029, SPR_STATION_INVERTED_BAR_0_NW_SE },
        { SPR_STATION_BASE_C_SW_NE, 17028, SPR_STATION_INVERTED_BAR_0_SW_NE },
        { SPR_STATION_BASE_C_NW_SE, 17029, SPR_STATION_INVERTED_BAR_0_NW_SE },
    };

    sub_98197C_rotated(session, direction, imageIds[direction][0] | session->TrackColours[SCHEME_MISC], 0, 0, 32, 28, 1, height, 0, 2, height);
    sub_98197C_rotated(session, direction, imageIds[direction][1] | session->TrackColours[SCHEME_TRACK], 0, 0, 32, 20, 3, height + 24, 0, 6, height + 24);
    sub_98199C_rotated(session, direction, imageIds[direction][2] | session->TrackColours[SCHEME_SUPPORTS], 0, 6, 32, 20, 1, height + 24, 0, 6, height + 24);
    track_paint_util_draw_station_metal_supports_2(session, direction, height, session->TrackColours[SCHEME_SUPPORTS], 11);
    track_paint_util_draw_station_inverted(session, rideIndex, trackSequence, direction, height, mapElement, STATION_VARIANT_1);
    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_6);
    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x00890CC4 */
static void inverted_hairpin_rc_track_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    if (track_element_is_lift_hill(mapElement)) {
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17070, 0, 0, 32, 20, 3, height + 24, 0, 6, height + 40);
            break;
        case 1:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17071, 0, 0, 32, 20, 3, height + 24, 0, 6, height + 40);
            break;
        case 2:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17072, 0, 0, 32, 20, 3, height + 24, 0, 6, height + 40);
            break;
        case 3:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17073, 0, 0, 32, 20, 3, height + 24, 0, 6, height + 40);
            break;
        }
    } else {
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17042, 0, 0, 32, 20, 3, height + 24, 0, 6, height + 40);
            break;
        case 1:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17043, 0, 0, 32, 20, 3, height + 24, 0, 6, height + 40);
            break;
        case 2:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17044, 0, 0, 32, 20, 3, height + 24, 0, 6, height + 40);
            break;
        case 3:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17045, 0, 0, 32, 20, 3, height + 24, 0, 6, height + 40);
            break;
        }
    }

    paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    if (track_paint_util_should_paint_supports(session->MapPosition)) {
        switch (direction) {
        case 0:
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES_INVERTED, 6, 0, height + 48, session->TrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height + 48, session->TrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES_INVERTED, 7, 0, height + 48, session->TrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height + 48, session->TrackColours[SCHEME_SUPPORTS]);
            break;
        }
    }

    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_1);
    } else {
        paint_util_push_tunnel_rotated(session, direction, height + 8, TUNNEL_2);
    }
    paint_util_set_general_support_height(session, height + 56, 0x20);
}

/** rct2: 0x00890CD4 */
static void inverted_hairpin_rc_track_60_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    if (track_element_is_lift_hill(mapElement)) {
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17086, 0, 0, 32, 20, 3, height + 24, 0, 6, height + 88);
            break;
        case 1:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17087, 0, 0, 32, 2, 81, height + 24, 0, 4, height + 6);
            break;
        case 2:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17088, 0, 0, 32, 2, 81, height + 24, 0, 4, height + 6);
            break;
        case 3:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17089, 0, 0, 32, 20, 3, height + 24, 0, 6, height + 88);
            break;
        }
    } else {
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17058, 0, 0, 32, 20, 3, height + 24, 0, 6, height + 88);
            break;
        case 1:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17059, 0, 0, 32, 2, 81, height + 24, 0, 4, height + 6);
            break;
        case 2:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17060, 0, 0, 32, 2, 81, height + 24, 0, 4, height + 6);
            break;
        case 3:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17061, 0, 0, 32, 20, 3, height + 24, 0, 6, height + 88);
            break;
        }
    }
    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_1);
    } else {
        paint_util_push_tunnel_rotated(session, direction, height + 56, TUNNEL_2);
    }
    paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 104, 0x20);
}

/** rct2: 0x00890CE4 */
static void inverted_hairpin_rc_track_flat_to_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    if (track_element_is_lift_hill(mapElement)) {
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17062, 0, 0, 32, 20, 3, height + 24, 0, 6, height + 32);
            break;
        case 1:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17063, 0, 0, 32, 20, 3, height + 24, 0, 6, height + 32);
            break;
        case 2:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17064, 0, 0, 32, 20, 3, height + 24, 0, 6, height + 32);
            break;
        case 3:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17065, 0, 0, 32, 20, 3, height + 24, 0, 6, height + 32);
            break;
        }
    } else {
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17034, 0, 0, 32, 20, 3, height + 24, 0, 6, height + 32);
            break;
        case 1:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17035, 0, 0, 32, 20, 3, height + 24, 0, 6, height + 32);
            break;
        case 2:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17036, 0, 0, 32, 20, 3, height + 24, 0, 6, height + 32);
            break;
        case 3:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17037, 0, 0, 32, 20, 3, height + 24, 0, 6, height + 32);
            break;
        }
    }

    paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    if (track_paint_util_should_paint_supports(session->MapPosition)) {
        switch (direction) {
        case 0:
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES_INVERTED, 6, 0, height + 40, session->TrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height + 40, session->TrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES_INVERTED, 7, 0, height + 40, session->TrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height + 40, session->TrackColours[SCHEME_SUPPORTS]);
            break;
        }
    }

    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
    } else {
        paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_2);
    }
    paint_util_set_general_support_height(session, height + 48, 0x20);
}

/** rct2: 0x00890CF4 */
static void inverted_hairpin_rc_track_25_deg_up_to_60_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    if (track_element_is_lift_hill(mapElement)) {
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17074, 0, 0, 32, 20, 3, height + 24, 0, 6, height + 56);
            break;
        case 1:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17078, 0, 0, 32, 10, 49, height + 24, 0, 10, height + 6);
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17075, 0, 0, 32, 2, 49, height + 24, 0, 4, height + 6);
            break;
        case 2:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17079, 0, 0, 32, 10, 49, height + 24, 0, 10, height + 6);
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17076, 0, 0, 32, 2, 49, height + 24, 0, 4, height + 6);
            break;
        case 3:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17077, 0, 0, 32, 20, 3, height + 24, 0, 6, height + 56);
            break;
        }
    } else {
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17046, 0, 0, 32, 20, 3, height + 24, 0, 6, height + 56);
            break;
        case 1:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17050, 0, 0, 32, 10, 49, height + 24, 0, 10, height + 6);
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17047, 0, 0, 32, 2, 49, height + 24, 0, 4, height + 6);
            break;
        case 2:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17051, 0, 0, 32, 10, 49, height + 24, 0, 10, height + 6);
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17048, 0, 0, 32, 2, 49, height + 24, 0, 4, height + 6);
            break;
        case 3:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17049, 0, 0, 32, 20, 3, height + 24, 0, 6, height + 56);
            break;
        }
    }
    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_1);
    } else {
        paint_util_push_tunnel_rotated(session, direction, height + 24, TUNNEL_2);
    }
    paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 72, 0x20);
}

/** rct2: 0x00890D04 */
static void inverted_hairpin_rc_track_60_deg_up_to_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    if (track_element_is_lift_hill(mapElement)) {
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17080, 0, 0, 32, 20, 3, height + 24, 0, 6, height + 56);
            break;
        case 1:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17084, 0, 0, 32, 10, 49, height + 24, 0, 10, height + 6);
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17081, 0, 0, 32, 2, 49, height + 24, 0, 4, height + 6);
            break;
        case 2:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17085, 0, 0, 32, 10, 49, height + 24, 0, 10, height + 6);
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17082, 0, 0, 32, 2, 49, height + 24, 0, 4, height + 6);
            break;
        case 3:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17083, 0, 0, 32, 20, 3, height + 24, 0, 6, height + 56);
            break;
        }
    } else {
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17052, 0, 0, 32, 20, 3, height + 24, 0, 6, height + 56);
            break;
        case 1:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17056, 0, 0, 32, 10, 49, height + 24, 0, 10, height + 6);
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17053, 0, 0, 32, 2, 49, height + 24, 0, 4, height + 6);
            break;
        case 2:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17057, 0, 0, 32, 10, 49, height + 24, 0, 10, height + 6);
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17054, 0, 0, 32, 2, 49, height + 24, 0, 4, height + 6);
            break;
        case 3:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17055, 0, 0, 32, 20, 3, height + 24, 0, 6, height + 56);
            break;
        }
    }

    paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    if (track_paint_util_should_paint_supports(session->MapPosition)) {
        switch (direction) {
        case 0:
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES_INVERTED, 6, 0, height + 62, session->TrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height + 62, session->TrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES_INVERTED, 7, 0, height + 62, session->TrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height + 62, session->TrackColours[SCHEME_SUPPORTS]);
            break;
        }
    }

    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_1);
    } else {
        paint_util_push_tunnel_rotated(session, direction, height + 24, TUNNEL_2);
    }
    paint_util_set_general_support_height(session, height + 72, 0x20);
}

/** rct2: 0x00890D14 */
static void inverted_hairpin_rc_track_25_deg_up_to_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    if (track_element_is_lift_hill(mapElement)) {
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17066, 0, 0, 32, 20, 3, height + 24, 0, 6, height + 32);
            break;
        case 1:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17067, 0, 0, 32, 20, 3, height + 24, 0, 6, height + 32);
            break;
        case 2:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17068, 0, 0, 32, 20, 3, height + 24, 0, 6, height + 32);
            break;
        case 3:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17069, 0, 0, 32, 20, 3, height + 24, 0, 6, height + 32);
            break;
        }
    } else {
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17038, 0, 0, 32, 20, 3, height + 24, 0, 6, height + 32);
            break;
        case 1:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17039, 0, 0, 32, 20, 3, height + 24, 0, 6, height + 32);
            break;
        case 2:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17040, 0, 0, 32, 20, 3, height + 24, 0, 6, height + 32);
            break;
        case 3:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17041, 0, 0, 32, 20, 3, height + 24, 0, 6, height + 32);
            break;
        }
    }

    paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    if (track_paint_util_should_paint_supports(session->MapPosition)) {
        switch (direction) {
        case 0:
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES_INVERTED, 6, 0, height + 38, session->TrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height + 38, session->TrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES_INVERTED, 7, 0, height + 38, session->TrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height + 38, session->TrackColours[SCHEME_SUPPORTS]);
            break;
        }
    }

    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_0);
    } else {
        paint_util_push_tunnel_rotated(session, direction, height + 8, TUNNEL_12);
    }
    paint_util_set_general_support_height(session, height + 40, 0x20);
}

/** rct2: 0x00890D24 */
static void inverted_hairpin_rc_track_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    inverted_hairpin_rc_track_25_deg_up(session, rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x00890D34 */
static void inverted_hairpin_rc_track_60_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    inverted_hairpin_rc_track_60_deg_up(session, rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x00890D44 */
static void inverted_hairpin_rc_track_flat_to_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    inverted_hairpin_rc_track_25_deg_up_to_flat(session, rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x00890D54 */
static void inverted_hairpin_rc_track_25_deg_down_to_60_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    inverted_hairpin_rc_track_60_deg_up_to_25_deg_up(session, rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x00890D64 */
static void inverted_hairpin_rc_track_60_deg_down_to_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    inverted_hairpin_rc_track_25_deg_up_to_60_deg_up(session, rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x00890D74 */
static void inverted_hairpin_rc_track_25_deg_down_to_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    inverted_hairpin_rc_track_flat_to_25_deg_up(session, rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x00890DB4 */
static void inverted_hairpin_rc_track_left_quarter_turn_3(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17123, 0, 0, 32, 20, 3, height + 24, 0, 6, height + 22);
            break;
        case 1:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17126, 0, 0, 32, 20, 3, height + 24, 0, 6, height + 22);
            break;
        case 2:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17129, 0, 0, 32, 20, 3, height + 24, 0, 6, height + 22);
            break;
        case 3:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17120, 0, 0, 32, 20, 3, height + 24, 0, 6, height + 22);
            break;
        }

        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 30, session->TrackColours[SCHEME_SUPPORTS]);

        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
        }
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 1:
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17122, 0, 0, 16, 16, 3, height + 24, 16, 0, height + 22);
            break;
        case 1:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17125, 0, 0, 16, 16, 3, height + 24, 0, 0, height + 22);
            break;
        case 2:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17128, 0, 0, 16, 16, 3, height + 24, 0, 16, height + 22);
            break;
        case 3:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17119, 0, 0, 16, 16, 3, height + 24, 16, 16, height + 22);
            break;
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17121, 0, 0, 20, 32, 3, height + 24, 6, 0, height + 22);
            break;
        case 1:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17124, 0, 0, 20, 32, 3, height + 24, 6, 0, height + 22);
            break;
        case 2:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17127, 0, 0, 20, 32, 3, height + 24, 6, 0, height + 22);
            break;
        case 3:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17118, 0, 0, 20, 32, 3, height + 24, 6, 0, height + 22);
            break;
        }

        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 30, session->TrackColours[SCHEME_SUPPORTS]);

        switch (direction) {
        case 2:
            paint_util_push_tunnel_right(session, height, TUNNEL_0);
            break;
        case 3:
            paint_util_push_tunnel_left(session, height, TUNNEL_0);
            break;
        }
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    }
}

/** rct2: 0x00890DC4 */
static void inverted_hairpin_rc_track_right_quarter_turn_3(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    inverted_hairpin_rc_track_left_quarter_turn_3(session, rideIndex, trackSequence, (direction - 1) & 3, height, mapElement);
}

/** rct2: 0x00890DD4 */
static void inverted_hairpin_rc_track_left_quarter_turn_3_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17141, 0, 6, 32, 20, 3, height + 24);
            break;
        case 1:
            sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17143, 0, 6, 32, 20, 3, height + 24);
            break;
        case 2:
            sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17145, 0, 6, 32, 20, 3, height + 24);
            break;
        case 3:
            sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17139, 0, 6, 32, 20, 3, height + 24);
            break;
        }

        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 38, session->TrackColours[SCHEME_SUPPORTS]);

        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_1);
        }
        paint_util_set_general_support_height(session, height + 72, 0x20);
        break;
    case 1:
        paint_util_set_general_support_height(session, height + 56, 0x20);
        break;
    case 2:
        paint_util_set_general_support_height(session, height + 56, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17140, 6, 0, 20, 32, 3, height + 24);
            break;
        case 1:
            sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17142, 6, 0, 20, 32, 3, height + 24);
            break;
        case 2:
            sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17144, 6, 0, 20, 32, 3, height + 24);
            break;
        case 3:
            sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17138, 6, 0, 20, 32, 3, height + 24);
            break;
        }

        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 38, session->TrackColours[SCHEME_SUPPORTS]);

        switch (direction) {
        case 2:
            paint_util_push_tunnel_right(session, height + 8, TUNNEL_2);
            break;
        case 3:
            paint_util_push_tunnel_left(session, height + 8, TUNNEL_2);
            break;
        }
        paint_util_set_general_support_height(session, height + 72, 0x20);
        break;
    }
}

/** rct2: 0x00890DE4 */
static void inverted_hairpin_rc_track_right_quarter_turn_3_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17130, 0, 6, 32, 20, 3, height + 24);
            break;
        case 1:
            sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17132, 0, 6, 32, 20, 3, height + 24);
            break;
        case 2:
            sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17134, 0, 6, 32, 20, 3, height + 24);
            break;
        case 3:
            sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17136, 0, 6, 32, 20, 3, height + 24);
            break;
        }

        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 38, session->TrackColours[SCHEME_SUPPORTS]);

        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_1);
        }
        paint_util_set_general_support_height(session, height + 72, 0x20);
        break;
    case 1:
        paint_util_set_general_support_height(session, height + 56, 0x20);
        break;
    case 2:
        paint_util_set_general_support_height(session, height + 56, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17131, 6, 0, 20, 32, 3, height + 24);
            break;
        case 1:
            sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17133, 6, 0, 20, 32, 3, height + 24);
            break;
        case 2:
            sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17135, 6, 0, 20, 32, 3, height + 24);
            break;
        case 3:
            sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17137, 6, 0, 20, 32, 3, height + 24);
            break;
        }

        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 38, session->TrackColours[SCHEME_SUPPORTS]);

        switch (direction) {
        case 0:
            paint_util_push_tunnel_right(session, height + 8, TUNNEL_2);
            break;
        case 1:
            paint_util_push_tunnel_left(session, height + 8, TUNNEL_2);
            break;
        }
        paint_util_set_general_support_height(session, height + 72, 0x20);
        break;
    }
}

/** rct2: 0x00890DF4 */
static void inverted_hairpin_rc_track_left_quarter_turn_3_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    inverted_hairpin_rc_track_right_quarter_turn_3_25_deg_up(session, rideIndex, trackSequence, (direction + 1) & 3, height, mapElement);
}

/** rct2: 0x00890E04 */
static void inverted_hairpin_rc_track_right_quarter_turn_3_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    inverted_hairpin_rc_track_left_quarter_turn_3_25_deg_up(session, rideIndex, trackSequence, (direction - 1) & 3, height, mapElement);
}

/** rct2: 0x00890E64 */
static void inverted_hairpin_rc_track_left_quarter_turn_1(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
        sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17114, 0, 0, 26, 24, 3, height + 24, 6, 2, height + 24);
        break;
    case 1:
        sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17115, 0, 0, 26, 26, 3, height + 24, 0, 0, height + 24);
        break;
    case 2:
        sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17116, 0, 0, 24, 26, 3, height + 24, 2, 6, height + 24);
        break;
    case 3:
        sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17117, 0, 0, 24, 24, 3, height + 24, 6, 6, height + 24);
        break;
    }

    paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
    metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 30, session->TrackColours[SCHEME_SUPPORTS]);

    switch (direction) {
    case 0:
        paint_util_push_tunnel_left(session, height, TUNNEL_0);
        break;
    case 2:
        paint_util_push_tunnel_right(session, height, TUNNEL_0);
        break;
    case 3:
        paint_util_push_tunnel_right(session, height, TUNNEL_0);
        paint_util_push_tunnel_left(session, height, TUNNEL_0);
        break;
    }
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x00890E74 */
static void inverted_hairpin_rc_track_right_quarter_turn_1(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    inverted_hairpin_rc_track_left_quarter_turn_1(session, rideIndex, trackSequence, (direction - 1) & 3, height, mapElement);
}

/** rct2: 0x00890E24 */
static void inverted_hairpin_rc_track_flat_to_60_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    if (track_element_is_lift_hill(mapElement)) {
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17102, 0, 0, 30, 20, 3, height + 24, 0, 6, height + 56);
            break;
        case 1:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17106, 0, 0, 32, 10, 49, height + 24, 0, 10, height + 6);
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17103, 0, 0, 32, 2, 49, height + 24, 0, 4, height + 6);
            break;
        case 2:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17107, 0, 0, 32, 10, 49, height + 24, 0, 10, height + 6);
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17104, 0, 0, 32, 2, 49, height + 24, 0, 4, height + 6);
            break;
        case 3:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17105, 0, 0, 30, 20, 3, height + 24, 0, 6, height + 56);
            break;
        }
    } else {
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17090, 0, 0, 30, 20, 3, height + 24, 0, 6, height + 56);
            break;
        case 1:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17094, 0, 0, 32, 10, 49, height + 24, 0, 10, height + 6);
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17091, 0, 0, 32, 2, 49, height + 24, 0, 4, height + 6);
            break;
        case 2:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17095, 0, 0, 32, 10, 49, height + 24, 0, 10, height + 6);
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17092, 0, 0, 32, 2, 49, height + 24, 0, 4, height + 6);
            break;
        case 3:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17093, 0, 0, 30, 20, 3, height + 24, 0, 6, height + 56);
            break;
        }
    }
    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
    } else {
        paint_util_push_tunnel_rotated(session, direction, height + 24, TUNNEL_2);
    }
    paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 64, 0x20);
}

/** rct2: 0x00890E34 */
static void inverted_hairpin_rc_track_60_deg_up_to_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    if (track_element_is_lift_hill(mapElement)) {
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17108, 0, 0, 30, 20, 3, height + 24, 0, 6, height + 48);
            break;
        case 1:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17112, 0, 0, 32, 10, 41, height + 24, 0, 10, height + 6);
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17109, 0, 0, 32, 2, 41, height + 24, 0, 4, height + 6);
            break;
        case 2:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17113, 0, 0, 32, 10, 41, height + 24, 0, 10, height + 6);
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17110, 0, 0, 32, 2, 41, height + 24, 0, 4, height + 6);
            break;
        case 3:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17111, 0, 0, 30, 20, 3, height + 24, 0, 6, height + 48);
            break;
        }
    } else {
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17096, 0, 0, 30, 20, 3, height + 24, 0, 6, height + 48);
            break;
        case 1:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17100, 0, 0, 32, 10, 41, height + 24, 0, 10, height + 6);
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17097, 0, 0, 32, 2, 41, height + 24, 0, 4, height + 6);
            break;
        case 2:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17101, 0, 0, 32, 10, 41, height + 24, 0, 10, height + 6);
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17098, 0, 0, 32, 2, 41, height + 24, 0, 4, height + 6);
            break;
        case 3:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17099, 0, 0, 30, 20, 3, height + 24, 0, 6, height + 48);
            break;
        }
    }

    paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    switch (direction) {
    case 0:
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES_INVERTED, 6, 0, height + 54, session->TrackColours[SCHEME_SUPPORTS]);
        break;
    case 1:
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height + 54, session->TrackColours[SCHEME_SUPPORTS]);
        break;
    case 2:
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES_INVERTED, 7, 0, height + 54, session->TrackColours[SCHEME_SUPPORTS]);
        break;
    case 3:
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height + 54, session->TrackColours[SCHEME_SUPPORTS]);
        break;
    }

    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_1);
    } else {
        paint_util_push_tunnel_rotated(session, direction, height + 24, TUNNEL_0);
    }
    paint_util_set_general_support_height(session, height + 56, 0x20);
}

/** rct2: 0x00890E44 */
static void inverted_hairpin_rc_track_flat_to_60_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    inverted_hairpin_rc_track_60_deg_up_to_flat(session, rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x00890E54 */
static void inverted_hairpin_rc_track_60_deg_down_to_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    inverted_hairpin_rc_track_flat_to_60_deg_up(session, rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x00890E14 */
static void inverted_hairpin_rc_track_brakes(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
    case 2:
        sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17028, 0, 0, 32, 20, 1, height + 24, 0, 6, height + 22);
        break;
    case 1:
    case 3:
        sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17029, 0, 0, 32, 20, 1, height + 24, 0, 6, height + 22);
        break;
    }

    paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    if (track_paint_util_should_paint_supports(session->MapPosition)) {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 30, session->TrackColours[SCHEME_SUPPORTS]);
    }

    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x00890E84 */
static void inverted_hairpin_rc_track_block_brakes(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
    case 2:
        sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17028, 0, 0, 32, 20, 1, height + 24, 0, 6, height + 22);
        break;
    case 1:
    case 3:
        sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 17029, 0, 0, 32, 20, 1, height + 24, 0, 6, height + 22);
        break;
    }

    paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    if (track_paint_util_should_paint_supports(session->MapPosition)) {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 30, session->TrackColours[SCHEME_SUPPORTS]);
    }

    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

TRACK_PAINT_FUNCTION get_track_paint_function_inverted_hairpin_rc(sint32 trackType, sint32 direction)
{
    switch (trackType) {
    case TRACK_ELEM_FLAT:
        return inverted_hairpin_rc_track_flat;
    case TRACK_ELEM_END_STATION:
    case TRACK_ELEM_BEGIN_STATION:
    case TRACK_ELEM_MIDDLE_STATION:
        return inverted_hairpin_rc_track_station;
    case TRACK_ELEM_25_DEG_UP:
        return inverted_hairpin_rc_track_25_deg_up;
    case TRACK_ELEM_60_DEG_UP:
        return inverted_hairpin_rc_track_60_deg_up;
    case TRACK_ELEM_FLAT_TO_25_DEG_UP:
        return inverted_hairpin_rc_track_flat_to_25_deg_up;
    case TRACK_ELEM_25_DEG_UP_TO_60_DEG_UP:
        return inverted_hairpin_rc_track_25_deg_up_to_60_deg_up;
    case TRACK_ELEM_60_DEG_UP_TO_25_DEG_UP:
        return inverted_hairpin_rc_track_60_deg_up_to_25_deg_up;
    case TRACK_ELEM_25_DEG_UP_TO_FLAT:
        return inverted_hairpin_rc_track_25_deg_up_to_flat;
    case TRACK_ELEM_25_DEG_DOWN:
        return inverted_hairpin_rc_track_25_deg_down;
    case TRACK_ELEM_60_DEG_DOWN:
        return inverted_hairpin_rc_track_60_deg_down;
    case TRACK_ELEM_FLAT_TO_25_DEG_DOWN:
        return inverted_hairpin_rc_track_flat_to_25_deg_down;
    case TRACK_ELEM_25_DEG_DOWN_TO_60_DEG_DOWN:
        return inverted_hairpin_rc_track_25_deg_down_to_60_deg_down;
    case TRACK_ELEM_60_DEG_DOWN_TO_25_DEG_DOWN:
        return inverted_hairpin_rc_track_60_deg_down_to_25_deg_down;
    case TRACK_ELEM_25_DEG_DOWN_TO_FLAT:
        return inverted_hairpin_rc_track_25_deg_down_to_flat;
    case TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES:
        return inverted_hairpin_rc_track_left_quarter_turn_3;
    case TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES:
        return inverted_hairpin_rc_track_right_quarter_turn_3;
    case TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_25_DEG_UP:
        return inverted_hairpin_rc_track_left_quarter_turn_3_25_deg_up;
    case TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_25_DEG_UP:
        return inverted_hairpin_rc_track_right_quarter_turn_3_25_deg_up;
    case TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_25_DEG_DOWN:
        return inverted_hairpin_rc_track_left_quarter_turn_3_25_deg_down;
    case TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_25_DEG_DOWN:
        return inverted_hairpin_rc_track_right_quarter_turn_3_25_deg_down;
    case TRACK_ELEM_LEFT_QUARTER_TURN_1_TILE:
        return inverted_hairpin_rc_track_left_quarter_turn_1;
    case TRACK_ELEM_RIGHT_QUARTER_TURN_1_TILE:
        return inverted_hairpin_rc_track_right_quarter_turn_1;
    case TRACK_ELEM_FLAT_TO_60_DEG_UP:
        return inverted_hairpin_rc_track_flat_to_60_deg_up;
    case TRACK_ELEM_60_DEG_UP_TO_FLAT:
        return inverted_hairpin_rc_track_60_deg_up_to_flat;
    case TRACK_ELEM_FLAT_TO_60_DEG_DOWN:
        return inverted_hairpin_rc_track_flat_to_60_deg_down;
    case TRACK_ELEM_60_DEG_DOWN_TO_FLAT:
        return inverted_hairpin_rc_track_60_deg_down_to_flat;
    case TRACK_ELEM_BRAKES:
        return inverted_hairpin_rc_track_brakes;
    case TRACK_ELEM_BLOCK_BRAKES:
        return inverted_hairpin_rc_track_block_brakes;
    }
    return NULL;
}
