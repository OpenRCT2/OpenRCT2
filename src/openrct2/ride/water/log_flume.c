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

#include "../../interface/viewport.h"
#include "../../paint/paint.h"
#include "../../paint/supports.h"
#include "../../world/sprite.h"
#include "../track.h"
#include "../track_paint.h"

enum {
    SPR_LOG_FLUME_FLAT_SW_NE = 20996,
    SPR_LOG_FLUME_FLAT_NW_SE = 20997,
    SPR_LOG_FLUME_FLAT_NE_SW = 20998,
    SPR_LOG_FLUME_FLAT_SE_NW = 20999,
    SPR_LOG_FLUME_FLAT_FRONT_SW_NE = 21000,
    SPR_LOG_FLUME_FLAT_FRONT_NW_SE = 21001,
    SPR_LOG_FLUME_FLAT_FRONT_NE_SW = 21002,
    SPR_LOG_FLUME_FLAT_FRONT_SE_NW = 21003,
    SPR_LOG_FLUME_REVERSER_SW_NE = 21004,
    SPR_LOG_FLUME_REVERSER_NW_SE = 21005,
    SPR_LOG_FLUME_REVERSER_NE_SW = 21006,
    SPR_LOG_FLUME_REVERSER_SE_NW = 21007,
    SPR_LOG_FLUME_REVERSER_FRONT_SW_NE = 21008,
    SPR_LOG_FLUME_REVERSER_FRONT_NW_SE = 21009,
    SPR_LOG_FLUME_REVERSER_FRONT_NE_SW = 21010,
    SPR_LOG_FLUME_REVERSER_FRONT_SE_NW = 21011,
    SPR_LOG_FLUME_FLAT_TO_25_DEG_UP_SW_NE = 21012,
    SPR_LOG_FLUME_FLAT_TO_25_DEG_UP_NW_SE = 21013,
    SPR_LOG_FLUME_FLAT_TO_25_DEG_UP_NE_SW = 21014,
    SPR_LOG_FLUME_FLAT_TO_25_DEG_UP_SE_NW = 21015,
    SPR_LOG_FLUME_FLAT_TO_25_DEG_UP_FRONT_SW_NE = 21016,
    SPR_LOG_FLUME_FLAT_TO_25_DEG_UP_FRONT_NW_SE = 21017,
    SPR_LOG_FLUME_FLAT_TO_25_DEG_UP_FRONT_NE_SW = 21018,
    SPR_LOG_FLUME_FLAT_TO_25_DEG_UP_FRONT_SE_NW = 21019,
    SPR_LOG_FLUME_25_DEG_UP_TO_FLAT_SW_NE = 21020,
    SPR_LOG_FLUME_25_DEG_UP_TO_FLAT_NW_SE = 21021,
    SPR_LOG_FLUME_25_DEG_UP_TO_FLAT_NE_SW = 21022,
    SPR_LOG_FLUME_25_DEG_UP_TO_FLAT_SE_NW = 21023,
    SPR_LOG_FLUME_25_DEG_UP_TO_FLAT_FRONT_SW_NE = 21024,
    SPR_LOG_FLUME_25_DEG_UP_TO_FLAT_FRONT_NW_SE = 21025,
    SPR_LOG_FLUME_25_DEG_UP_TO_FLAT_FRONT_NE_SW = 21026,
    SPR_LOG_FLUME_25_DEG_UP_TO_FLAT_FRONT_SE_NW = 21027,
    SPR_LOG_FLUME_25_DEG_UP_SW_NE = 21028,
    SPR_LOG_FLUME_25_DEG_UP_NW_SE = 21029,
    SPR_LOG_FLUME_25_DEG_UP_NE_SW = 21030,
    SPR_LOG_FLUME_25_DEG_UP_SE_NW = 21031,
    SPR_LOG_FLUME_25_DEG_UP_FRONT_SW_NE = 21032,
    SPR_LOG_FLUME_25_DEG_UP_FRONT_NW_SE = 21033,
    SPR_LOG_FLUME_25_DEG_UP_FRONT_NE_SW = 21034,
    SPR_LOG_FLUME_25_DEG_UP_FRONT_SE_NW = 21035,
    SPR_LOG_FLUME_25_DEG_DOWN_TO_FLAT_NE_SW = 21036,
    SPR_LOG_FLUME_25_DEG_DOWN_TO_FLAT_SE_NW = 21037,
    SPR_LOG_FLUME_25_DEG_DOWN_TO_FLAT_SW_NE = 21038,
    SPR_LOG_FLUME_25_DEG_DOWN_TO_FLAT_NW_SE = 21039,
    SPR_LOG_FLUME_FLAT_TO_25_DEG_DOWN_NE_SW = 21040,
    SPR_LOG_FLUME_FLAT_TO_25_DEG_DOWN_SE_NW = 21041,
    SPR_LOG_FLUME_FLAT_TO_25_DEG_DOWN_SW_NE = 21042,
    SPR_LOG_FLUME_FLAT_TO_25_DEG_DOWN_NW_SE = 21043,
    SPR_LOG_FLUME_25_DEG_DOWN_NE_SW = 21044,
    SPR_LOG_FLUME_25_DEG_DOWN_SE_NW = 21045,
    SPR_LOG_FLUME_25_DEG_DOWN_SW_NE = 21046,
    SPR_LOG_FLUME_25_DEG_DOWN_NW_SE = 21047,
    SPR_LOG_FLUME_3_TURN_NE_SE_SEQ_0 = 21048,
    SPR_LOG_FLUME_3_TURN_NE_SE_SEQ_2 = 21049,
    SPR_LOG_FLUME_3_TURN_NE_SE_SEQ_3 = 21050,
    SPR_LOG_FLUME_3_TURN_SE_SW_SEQ_0 = 21051,
    SPR_LOG_FLUME_3_TURN_SE_SW_SEQ_2 = 21052,
    SPR_LOG_FLUME_3_TURN_SE_SW_SEQ_3 = 21053,
    SPR_LOG_FLUME_3_TURN_SW_NW_SEQ_0 = 21054,
    SPR_LOG_FLUME_3_TURN_SW_NW_SEQ_2 = 21055,
    SPR_LOG_FLUME_3_TURN_SW_NW_SEQ_3 = 21056,
    SPR_LOG_FLUME_3_TURN_NW_NE_SEQ_0 = 21057,
    SPR_LOG_FLUME_3_TURN_NW_NE_SEQ_2 = 21058,
    SPR_LOG_FLUME_3_TURN_NW_NE_SEQ_3 = 21059,
    SPR_LOG_FLUME_3_TURN_FRONT_SW_SE_SEQ_0 = 21060,
    SPR_LOG_FLUME_3_TURN_FRONT_SW_SE_SEQ_2 = 21061,
    SPR_LOG_FLUME_3_TURN_FRONT_SW_SE_SEQ_3 = 21062,
    SPR_LOG_FLUME_3_TURN_FRONT_SE_SW_SEQ_0 = 21063,
    SPR_LOG_FLUME_3_TURN_FRONT_SE_SW_SEQ_2 = 21064,
    SPR_LOG_FLUME_3_TURN_FRONT_SE_SW_SEQ_3 = 21065,
    SPR_LOG_FLUME_3_TURN_FRONT_SW_NW_SEQ_0 = 21066,
    SPR_LOG_FLUME_3_TURN_FRONT_SW_NW_SEQ_2 = 21067,
    SPR_LOG_FLUME_3_TURN_FRONT_SW_NW_SEQ_3 = 21068,
    SPR_LOG_FLUME_3_TURN_FRONT_NW_NE_SEQ_0 = 21069,
    SPR_LOG_FLUME_3_TURN_FRONT_NW_NE_SEQ_2 = 21070,
    SPR_LOG_FLUME_3_TURN_FRONT_NW_NE_SEQ_3 = 21071,
    SPR_LOG_FLUME_3_TURN_NW_SW_SEQ_0 = 21072,
    SPR_LOG_FLUME_3_TURN_NW_SW_SEQ_2 = 21073,
    SPR_LOG_FLUME_3_TURN_NW_SW_SEQ_3 = 21074,
    SPR_LOG_FLUME_3_TURN_NE_NW_SEQ_0 = 21075,
    SPR_LOG_FLUME_3_TURN_NE_NW_SEQ_2 = 21076,
    SPR_LOG_FLUME_3_TURN_NE_NW_SEQ_3 = 21077,
    SPR_LOG_FLUME_3_TURN_SE_NE_SEQ_0 = 21078,
    SPR_LOG_FLUME_3_TURN_SE_NE_SEQ_2 = 21079,
    SPR_LOG_FLUME_3_TURN_SE_NE_SEQ_3 = 21080,
    SPR_LOG_FLUME_3_TURN_SW_SE_SEQ_0 = 21081,
    SPR_LOG_FLUME_3_TURN_SW_SE_SEQ_2 = 21082,
    SPR_LOG_FLUME_3_TURN_SW_SE_SEQ_3 = 21083,
    SPR_LOG_FLUME_3_TURN_SW_SE_SW_SEQ_3 = 21084,
    SPR_LOG_FLUME_3_TURN_SW_SE_SW_SEQ_2 = 21085,
    SPR_LOG_FLUME_3_TURN_SW_SE_SW_SEQ_1 = 21086,
    SPR_LOG_FLUME_3_TURN_SW_SE_SW_SEQ_0 = 21087,
    SPR_LOG_FLUME_3_TURN_NW_SW_NW_SEQ_0 = 21088,
    SPR_LOG_FLUME_3_TURN_NW_SW_NW_SEQ_1 = 21089,
    SPR_LOG_FLUME_3_TURN_NW_SW_NW_SEQ_2 = 21090,
    SPR_LOG_FLUME_3_TURN_NW_SW_NW_SEQ_3 = 21091,
    SPR_LOG_FLUME_3_TURN_SW_NW_SW_SEQ_3 = 21092,
    SPR_LOG_FLUME_3_TURN_SW_NW_SW_SEQ_2 = 21093,
    SPR_LOG_FLUME_3_TURN_SW_NW_SW_SEQ_1 = 21094,
    SPR_LOG_FLUME_3_TURN_SW_NW_SW_SEQ_0 = 21095,
    SPR_LOG_FLUME_3_TURN_SE_SW_SE_SEQ_0 = 21096,
    SPR_LOG_FLUME_3_TURN_SE_SW_SE_SEQ_1 = 21097,
    SPR_LOG_FLUME_3_TURN_SE_SW_SE_SEQ_2 = 21098,
    SPR_LOG_FLUME_3_TURN_SE_SW_SE_SEQ_3 = 21099,
    SPR_LOG_FLUME_3_TURN_FRONT_NE_NW_NE_SEQ_0 = 21100,
    SPR_LOG_FLUME_3_TURN_FRONT_NE_NW_NE_SEQ_1 = 21101,
    SPR_LOG_FLUME_3_TURN_FRONT_NE_NW_NE_SEQ_2 = 21102,
    SPR_LOG_FLUME_3_TURN_FRONT_NE_NW_NE_SEQ_3 = 21103,
    SPR_LOG_FLUME_3_TURN_FRONT_NW_SW_NW_SEQ_0 = 21104,
    SPR_LOG_FLUME_3_TURN_FRONT_NW_SW_NW_SEQ_1 = 21105,
    SPR_LOG_FLUME_3_TURN_FRONT_NW_SW_NW_SEQ_2 = 21106,
    SPR_LOG_FLUME_3_TURN_FRONT_NW_SW_NW_SEQ_3 = 21107,
    SPR_LOG_FLUME_3_TURN_FRONT_NE_SE_NE_SEQ_0 = 21108,
    SPR_LOG_FLUME_3_TURN_FRONT_NE_SE_NE_SEQ_1 = 21109,
    SPR_LOG_FLUME_3_TURN_FRONT_NE_SE_NE_SEQ_2 = 21110,
    SPR_LOG_FLUME_3_TURN_FRONT_NE_SE_NE_SEQ_3 = 21111,
    SPR_LOG_FLUME_3_TURN_FRONT_SE_SW_SE_SEQ_0 = 21112,
    SPR_LOG_FLUME_3_TURN_FRONT_SE_SW_SE_SEQ_1 = 21113,
    SPR_LOG_FLUME_3_TURN_FRONT_SE_SW_SE_SEQ_2 = 21114,
    SPR_LOG_FLUME_3_TURN_FRONT_SE_SW_SE_SEQ_3 = 21115,
    SPR_LOG_FLUME_3_TURN_NE_NW_NE_SEQ_0 = 21116,
    SPR_LOG_FLUME_3_TURN_NE_NW_NE_SEQ_1 = 21117,
    SPR_LOG_FLUME_3_TURN_NE_NW_NE_SEQ_2 = 21118,
    SPR_LOG_FLUME_3_TURN_NE_NW_NE_SEQ_3 = 21119,
    SPR_LOG_FLUME_3_TURN_SE_NE_SE_SEQ_3 = 21120,
    SPR_LOG_FLUME_3_TURN_SE_NE_SE_SEQ_2 = 21121,
    SPR_LOG_FLUME_3_TURN_SE_NE_SE_SEQ_1 = 21122,
    SPR_LOG_FLUME_3_TURN_SE_NE_SE_SEQ_0 = 21123,
    SPR_LOG_FLUME_3_TURN_NE_SE_NE_SEQ_0 = 21124,
    SPR_LOG_FLUME_3_TURN_NE_SE_NE_SEQ_1 = 21125,
    SPR_LOG_FLUME_3_TURN_NE_SE_NE_SEQ_2 = 21126,
    SPR_LOG_FLUME_3_TURN_NE_SE_NE_SEQ_3 = 21127,
    SPR_LOG_FLUME_3_TURN_NW_NE_NW_SEQ_3 = 21128,
    SPR_LOG_FLUME_3_TURN_NW_NE_NW_SEQ_2 = 21129,
    SPR_LOG_FLUME_3_TURN_NW_NE_NW_SEQ_1 = 21130,
    SPR_LOG_FLUME_3_TURN_NW_NE_NW_SEQ_0 = 21131,
};

static const uint32 LogFlumeTrackFlatImageIds[4][2] =
{
    { SPR_LOG_FLUME_FLAT_SW_NE, SPR_LOG_FLUME_FLAT_FRONT_SW_NE },
    { SPR_LOG_FLUME_FLAT_NW_SE, SPR_LOG_FLUME_FLAT_FRONT_NW_SE },
    { SPR_LOG_FLUME_FLAT_NE_SW, SPR_LOG_FLUME_FLAT_FRONT_NE_SW },
    { SPR_LOG_FLUME_FLAT_SE_NW, SPR_LOG_FLUME_FLAT_FRONT_SE_NW },
};

static void paint_log_flume_track_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    uint32 imageId = LogFlumeTrackFlatImageIds[direction][0] | gTrackColours[SCHEME_TRACK];
    uint32 frontImageId = LogFlumeTrackFlatImageIds[direction][1] | gTrackColours[SCHEME_TRACK];

    sub_98197C_rotated(direction, imageId, 0, 0, 32, 20, 2, height, 0, 6, height);
    sub_98197C_rotated(direction, frontImageId, 0, 0, 32, 1, 26, height, 0, 27, height);

    if (track_paint_util_should_paint_supports(session->MapPosition)) {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
    }

    paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
    paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(height + 32, 0x20);
}

static void paint_log_flume_track_station(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    uint32 imageId = LogFlumeTrackFlatImageIds[direction][0] | gTrackColours[SCHEME_TRACK];
    sub_98197C_rotated(direction, imageId, 0, 0, 32, 20, 1, height, 0, 6, height + 3);

    if (direction & 1) {
        imageId = SPR_STATION_BASE_B_NW_SE | gTrackColours[SCHEME_MISC];
    } else {
        imageId = SPR_STATION_BASE_B_SW_NE | gTrackColours[SCHEME_MISC];
    }
    sub_98196C(imageId, 0, 0, 32, 32, 1, height, get_current_rotation());

    if (direction & 1) {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 6, 0, height, gTrackColours[SCHEME_SUPPORTS]);
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 7, 0, height, gTrackColours[SCHEME_SUPPORTS]);
    } else {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 5, 0, height, gTrackColours[SCHEME_SUPPORTS]);
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 8, 0, height, gTrackColours[SCHEME_SUPPORTS]);
    }

    track_paint_util_draw_station_3(rideIndex, trackSequence, direction, height + 2, height, mapElement);
    // Covers shouldn't be offset by +2

    paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);

    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 32, 0x20);
}

static void paint_log_flume_track_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    static const uint32 imageIds[4][2] =
    {
        { SPR_LOG_FLUME_25_DEG_UP_SW_NE, SPR_LOG_FLUME_25_DEG_UP_FRONT_SW_NE },
        { SPR_LOG_FLUME_25_DEG_UP_NW_SE, SPR_LOG_FLUME_25_DEG_UP_FRONT_NW_SE },
        { SPR_LOG_FLUME_25_DEG_UP_NE_SW, SPR_LOG_FLUME_25_DEG_UP_FRONT_NE_SW },
        { SPR_LOG_FLUME_25_DEG_UP_SE_NW, SPR_LOG_FLUME_25_DEG_UP_FRONT_SE_NW },
    };

    uint32 imageId = imageIds[direction][0] | gTrackColours[SCHEME_TRACK];
    uint32 frontImageId = imageIds[direction][1] | gTrackColours[SCHEME_TRACK];

    sub_98197C_rotated(direction, imageId, 0, 0, 32, 20, 2, height, 0, 6, height);
    sub_98197C_rotated(direction, frontImageId, 0, 0, 32, 1, 50, height, 0, 27, height);

    if (track_paint_util_should_paint_supports(session->MapPosition)) {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
    }

    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_1);
    } else {
        paint_util_push_tunnel_rotated(direction, height + 8, TUNNEL_2);
    }
    paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(height + 56, 0x20);
}

static void paint_log_flume_track_flat_to_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    static const uint32 imageIds[4][2] =
    {
        { SPR_LOG_FLUME_FLAT_TO_25_DEG_UP_SW_NE, SPR_LOG_FLUME_FLAT_TO_25_DEG_UP_FRONT_SW_NE },
        { SPR_LOG_FLUME_FLAT_TO_25_DEG_UP_NW_SE, SPR_LOG_FLUME_FLAT_TO_25_DEG_UP_FRONT_NW_SE },
        { SPR_LOG_FLUME_FLAT_TO_25_DEG_UP_NE_SW, SPR_LOG_FLUME_FLAT_TO_25_DEG_UP_FRONT_NE_SW },
        { SPR_LOG_FLUME_FLAT_TO_25_DEG_UP_SE_NW, SPR_LOG_FLUME_FLAT_TO_25_DEG_UP_FRONT_SE_NW },
    };

    uint32 imageId = imageIds[direction][0] | gTrackColours[SCHEME_TRACK];
    uint32 frontImageId = imageIds[direction][1] | gTrackColours[SCHEME_TRACK];

    sub_98197C_rotated(direction, imageId, 0, 0, 32, 20, 2, height, 0, 6, height);
    sub_98197C_rotated(direction, frontImageId, 0, 0, 32, 1, 42, height, 0, 27, height);

    if (track_paint_util_should_paint_supports(session->MapPosition)) {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 4, 3, height, gTrackColours[SCHEME_SUPPORTS]);
    }

    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
    } else {
        paint_util_push_tunnel_rotated(direction, height, TUNNEL_2);
    }
    paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(height + 48, 0x20);
}

static void paint_log_flume_track_25_deg_up_to_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    static const uint32 imageIds[4][2] =
    {
        { SPR_LOG_FLUME_25_DEG_UP_TO_FLAT_SW_NE, SPR_LOG_FLUME_25_DEG_UP_TO_FLAT_FRONT_SW_NE },
        { SPR_LOG_FLUME_25_DEG_UP_TO_FLAT_NW_SE, SPR_LOG_FLUME_25_DEG_UP_TO_FLAT_FRONT_NW_SE },
        { SPR_LOG_FLUME_25_DEG_UP_TO_FLAT_NE_SW, SPR_LOG_FLUME_25_DEG_UP_TO_FLAT_FRONT_NE_SW },
        { SPR_LOG_FLUME_25_DEG_UP_TO_FLAT_SE_NW, SPR_LOG_FLUME_25_DEG_UP_TO_FLAT_FRONT_SE_NW },
    };

    uint32 imageId = imageIds[direction][0] | gTrackColours[SCHEME_TRACK];
    uint32 frontImageId = imageIds[direction][1] | gTrackColours[SCHEME_TRACK];

    sub_98197C_rotated(direction, imageId, 0, 0, 32, 20, 2, height, 0, 6, height);
    sub_98197C_rotated(direction, frontImageId, 0, 0, 32, 1, 34, height, 0, 27, height);

    if (track_paint_util_should_paint_supports(session->MapPosition)) {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
    }

    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_0);
    } else {
        paint_util_push_tunnel_rotated(direction, height + 8, TUNNEL_12);
    }
    paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(height + 40, 0x20);
}

static void paint_log_flume_track_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    static const uint32 imageIds[4][2] =
    {
        { SPR_LOG_FLUME_25_DEG_DOWN_SW_NE, SPR_LOG_FLUME_25_DEG_UP_FRONT_NE_SW },
        { SPR_LOG_FLUME_25_DEG_DOWN_NW_SE, SPR_LOG_FLUME_25_DEG_UP_FRONT_SE_NW },
        { SPR_LOG_FLUME_25_DEG_DOWN_NE_SW, SPR_LOG_FLUME_25_DEG_UP_FRONT_SW_NE },
        { SPR_LOG_FLUME_25_DEG_DOWN_SE_NW, SPR_LOG_FLUME_25_DEG_UP_FRONT_NW_SE },
    };

    uint32 imageId = imageIds[direction][0] | gTrackColours[SCHEME_TRACK];
    uint32 frontImageId = imageIds[direction][1] | gTrackColours[SCHEME_TRACK];

    sub_98197C_rotated(direction, imageId, 0, 0, 32, 20, 2, height, 0, 6, height);
    sub_98197C_rotated(direction, frontImageId, 0, 0, 32, 1, 50, height, 0, 27, height);

    if (track_paint_util_should_paint_supports(session->MapPosition)) {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
    }

    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(direction, height + 8, TUNNEL_2);
    } else {
        paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_1);
    }
    paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(height + 56, 0x20);
}

static void paint_log_flume_track_flat_to_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    static const uint32 imageIds[4][2] =
    {
        { SPR_LOG_FLUME_FLAT_TO_25_DEG_DOWN_SW_NE, SPR_LOG_FLUME_25_DEG_UP_TO_FLAT_FRONT_NE_SW },
        { SPR_LOG_FLUME_FLAT_TO_25_DEG_DOWN_NW_SE, SPR_LOG_FLUME_25_DEG_UP_TO_FLAT_FRONT_SE_NW },
        { SPR_LOG_FLUME_FLAT_TO_25_DEG_DOWN_NE_SW, SPR_LOG_FLUME_25_DEG_UP_TO_FLAT_FRONT_SW_NE },
        { SPR_LOG_FLUME_FLAT_TO_25_DEG_DOWN_SE_NW, SPR_LOG_FLUME_25_DEG_UP_TO_FLAT_FRONT_NW_SE },
    };

    uint32 imageId = imageIds[direction][0] | gTrackColours[SCHEME_TRACK];
    uint32 frontImageId = imageIds[direction][1] | gTrackColours[SCHEME_TRACK];

    sub_98197C_rotated(direction, imageId, 0, 0, 32, 20, 2, height, 0, 6, height);
    sub_98197C_rotated(direction, frontImageId, 0, 0, 32, 1, 34, height, 0, 27, height);

    if (track_paint_util_should_paint_supports(session->MapPosition)) {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
    }

    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(direction, height + 8, TUNNEL_12);
    } else {
        paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_0);
    }
    paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(height + 40, 0x20);
}

static void paint_log_flume_track_25_deg_down_to_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    static const uint32 imageIds[4][2] =
    {
        { SPR_LOG_FLUME_25_DEG_DOWN_TO_FLAT_SW_NE, SPR_LOG_FLUME_FLAT_TO_25_DEG_UP_FRONT_NE_SW },
        { SPR_LOG_FLUME_25_DEG_DOWN_TO_FLAT_NW_SE, SPR_LOG_FLUME_FLAT_TO_25_DEG_UP_FRONT_SE_NW },
        { SPR_LOG_FLUME_25_DEG_DOWN_TO_FLAT_NE_SW, SPR_LOG_FLUME_FLAT_TO_25_DEG_UP_FRONT_SW_NE },
        { SPR_LOG_FLUME_25_DEG_DOWN_TO_FLAT_SE_NW, SPR_LOG_FLUME_FLAT_TO_25_DEG_UP_FRONT_NW_SE },
    };

    uint32 imageId = imageIds[direction][0] | gTrackColours[SCHEME_TRACK];
    uint32 frontImageId = imageIds[direction][1] | gTrackColours[SCHEME_TRACK];

    sub_98197C_rotated(direction, imageId, 0, 0, 32, 20, 2, height, 0, 6, height);
    sub_98197C_rotated(direction, frontImageId, 0, 0, 32, 1, 42, height, 0, 27, height);

    if (track_paint_util_should_paint_supports(session->MapPosition)) {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 4, 3, height, gTrackColours[SCHEME_SUPPORTS]);
    }

    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(direction, height, TUNNEL_2);
    } else {
        paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
    }
    paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(height + 48, 0x20);
}

static void paint_log_flume_track_s_bend_left(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    static const uint32 imageIds[4][4][2] =
    {
        {   { SPR_LOG_FLUME_3_TURN_NE_NW_NE_SEQ_0, SPR_LOG_FLUME_3_TURN_FRONT_NE_NW_NE_SEQ_0 },
            { SPR_LOG_FLUME_3_TURN_NE_NW_NE_SEQ_1, SPR_LOG_FLUME_3_TURN_FRONT_NE_NW_NE_SEQ_1 },
            { SPR_LOG_FLUME_3_TURN_NE_NW_NE_SEQ_2, SPR_LOG_FLUME_3_TURN_FRONT_NE_NW_NE_SEQ_2 },
            { SPR_LOG_FLUME_3_TURN_NE_NW_NE_SEQ_3, SPR_LOG_FLUME_3_TURN_FRONT_NE_NW_NE_SEQ_3 }  },
        {   { SPR_LOG_FLUME_3_TURN_NW_SW_NW_SEQ_0, SPR_LOG_FLUME_3_TURN_FRONT_NW_SW_NW_SEQ_0 },
            { SPR_LOG_FLUME_3_TURN_NW_SW_NW_SEQ_1, SPR_LOG_FLUME_3_TURN_FRONT_NW_SW_NW_SEQ_1 },
            { SPR_LOG_FLUME_3_TURN_NW_SW_NW_SEQ_2, SPR_LOG_FLUME_3_TURN_FRONT_NW_SW_NW_SEQ_2 },
            { SPR_LOG_FLUME_3_TURN_NW_SW_NW_SEQ_3, SPR_LOG_FLUME_3_TURN_FRONT_NW_SW_NW_SEQ_3 }  },
        {   { SPR_LOG_FLUME_3_TURN_SW_SE_SW_SEQ_0, SPR_LOG_FLUME_3_TURN_FRONT_NE_NW_NE_SEQ_3 },
            { SPR_LOG_FLUME_3_TURN_SW_SE_SW_SEQ_1, SPR_LOG_FLUME_3_TURN_FRONT_NE_NW_NE_SEQ_2 },
            { SPR_LOG_FLUME_3_TURN_SW_SE_SW_SEQ_2, SPR_LOG_FLUME_3_TURN_FRONT_NE_NW_NE_SEQ_1 },
            { SPR_LOG_FLUME_3_TURN_SW_SE_SW_SEQ_3, SPR_LOG_FLUME_3_TURN_FRONT_NE_NW_NE_SEQ_0 }  },
        {   { SPR_LOG_FLUME_3_TURN_SE_NE_SE_SEQ_0, SPR_LOG_FLUME_3_TURN_FRONT_NW_SW_NW_SEQ_3 },
            { SPR_LOG_FLUME_3_TURN_SE_NE_SE_SEQ_1, SPR_LOG_FLUME_3_TURN_FRONT_NW_SW_NW_SEQ_2 },
            { SPR_LOG_FLUME_3_TURN_SE_NE_SE_SEQ_2, SPR_LOG_FLUME_3_TURN_FRONT_NW_SW_NW_SEQ_1 },
            { SPR_LOG_FLUME_3_TURN_SE_NE_SE_SEQ_3, SPR_LOG_FLUME_3_TURN_FRONT_NW_SW_NW_SEQ_0 }  },
    };

    uint32 imageId = imageIds[direction][trackSequence][0] | gTrackColours[SCHEME_TRACK];
    uint32 frontImageId = imageIds[direction][trackSequence][1] | gTrackColours[SCHEME_TRACK];
    sint16 bboy;

    switch (trackSequence) {
    case 0:
        sub_98197C_rotated(direction, imageId, 0, 0, 32, 27, 2, height, 0, 2, height);
        sub_98197C_rotated(direction, frontImageId, 0, 0, 32, 27, 0, height, 0, 2, height + 27);
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        break;
    case 1:
        bboy = (direction == 0 || direction == 1) ? 0 : 6;
        sub_98197C_rotated(direction, imageId, 0, 0, 32, 26, 2, height, 0, bboy, height);
        sub_98197C_rotated(direction, frontImageId, 0, 0, 32, 26, 0, height, 0, bboy, height + 27);
        if (direction == 0 || direction == 1) {
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 5 + (direction & 1), (direction & 1), height, gTrackColours[SCHEME_SUPPORTS]);
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        break;
    case 2:
        bboy = (direction == 2 || direction == 3) ? 0 : 6;
        sub_98197C_rotated(direction, imageId, 0, 0, 32, 26, 2, height, 0, bboy, height);
        sub_98197C_rotated(direction, frontImageId, 0, 0, 32, 26, 0, height, 0, bboy, height + 27);
        if (direction == 2 || direction == 3) {
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 5 + (direction & 1), (direction & 1), height, gTrackColours[SCHEME_SUPPORTS]);
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        break;
    case 3:
        sub_98197C_rotated(direction, imageId, 0, 0, 32, 27, 2, height, 0, 2, height);
        sub_98197C_rotated(direction, frontImageId, 0, 0, 32, 27, 0, height, 0, 2, height + 27);
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        break;
    }

    if (trackSequence == 0) {
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
        }
    } else if (trackSequence == 3) {
        if (direction == 1 || direction == 2) {
            paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
        }
    }

    paint_util_set_general_support_height(height + 32, 0x20);
}

static void paint_log_flume_track_s_bend_right(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    static const uint32 imageIds[4][4][2] =
    {
        {   { SPR_LOG_FLUME_3_TURN_NE_SE_NE_SEQ_0, SPR_LOG_FLUME_3_TURN_FRONT_NE_SE_NE_SEQ_0 },
            { SPR_LOG_FLUME_3_TURN_NE_SE_NE_SEQ_1, SPR_LOG_FLUME_3_TURN_FRONT_NE_SE_NE_SEQ_1 },
            { SPR_LOG_FLUME_3_TURN_NE_SE_NE_SEQ_2, SPR_LOG_FLUME_3_TURN_FRONT_NE_SE_NE_SEQ_2 },
            { SPR_LOG_FLUME_3_TURN_NE_SE_NE_SEQ_3, SPR_LOG_FLUME_3_TURN_FRONT_NE_SE_NE_SEQ_3 }  },
        {   { SPR_LOG_FLUME_3_TURN_SE_SW_SE_SEQ_0, SPR_LOG_FLUME_3_TURN_FRONT_SE_SW_SE_SEQ_0 },
            { SPR_LOG_FLUME_3_TURN_SE_SW_SE_SEQ_1, SPR_LOG_FLUME_3_TURN_FRONT_SE_SW_SE_SEQ_1 },
            { SPR_LOG_FLUME_3_TURN_SE_SW_SE_SEQ_2, SPR_LOG_FLUME_3_TURN_FRONT_SE_SW_SE_SEQ_2 },
            { SPR_LOG_FLUME_3_TURN_SE_SW_SE_SEQ_3, SPR_LOG_FLUME_3_TURN_FRONT_SE_SW_SE_SEQ_3 }  },
        {   { SPR_LOG_FLUME_3_TURN_SW_NW_SW_SEQ_0, SPR_LOG_FLUME_3_TURN_FRONT_NE_SE_NE_SEQ_3 },
            { SPR_LOG_FLUME_3_TURN_SW_NW_SW_SEQ_1, SPR_LOG_FLUME_3_TURN_FRONT_NE_SE_NE_SEQ_2 },
            { SPR_LOG_FLUME_3_TURN_SW_NW_SW_SEQ_2, SPR_LOG_FLUME_3_TURN_FRONT_NE_SE_NE_SEQ_1 },
            { SPR_LOG_FLUME_3_TURN_SW_NW_SW_SEQ_3, SPR_LOG_FLUME_3_TURN_FRONT_NE_SE_NE_SEQ_0 }  },
        {   { SPR_LOG_FLUME_3_TURN_NW_NE_NW_SEQ_0, SPR_LOG_FLUME_3_TURN_FRONT_SE_SW_SE_SEQ_3 },
            { SPR_LOG_FLUME_3_TURN_NW_NE_NW_SEQ_1, SPR_LOG_FLUME_3_TURN_FRONT_SE_SW_SE_SEQ_2 },
            { SPR_LOG_FLUME_3_TURN_NW_NE_NW_SEQ_2, SPR_LOG_FLUME_3_TURN_FRONT_SE_SW_SE_SEQ_1 },
            { SPR_LOG_FLUME_3_TURN_NW_NE_NW_SEQ_3, SPR_LOG_FLUME_3_TURN_FRONT_SE_SW_SE_SEQ_0 }  },
    };

    uint32 imageId = imageIds[direction][trackSequence][0] | gTrackColours[SCHEME_TRACK];
    uint32 frontImageId = imageIds[direction][trackSequence][1] | gTrackColours[SCHEME_TRACK];
    sint16 bboy;

    switch (trackSequence) {
    case 0:
        sub_98197C_rotated(direction, imageId, 0, 0, 32, 27, 2, height, 0, 2, height);
        sub_98197C_rotated(direction, frontImageId, 0, 0, 32, 27, 0, height, 0, 2, height + 27);
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        break;
    case 1:
        bboy = (direction == 2 || direction == 3) ? 0 : 6;
        sub_98197C_rotated(direction, imageId, 0, 0, 32, 26, 2, height, 0, bboy, height);
        sub_98197C_rotated(direction, frontImageId, 0, 0, 32, 26, 0, height, 0, bboy, height + 27);
        if (direction == 0 || direction == 1) {
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 8 - (direction & 1), 0, height, gTrackColours[SCHEME_SUPPORTS]);
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        break;
    case 2:
        bboy = (direction == 0 || direction == 1) ? 0 : 6;
        sub_98197C_rotated(direction, imageId, 0, 0, 32, 26, 2, height, 0, bboy, height);
        sub_98197C_rotated(direction, frontImageId, 0, 0, 32, 26, 0, height, 0, bboy, height + 27);
        if (direction == 2 || direction == 3) {
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 8 - (direction & 1), 0, height, gTrackColours[SCHEME_SUPPORTS]);
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        break;
    case 3:
        sub_98197C_rotated(direction, imageId, 0, 0, 32, 27, 2, height, 0, 2, height);
        sub_98197C_rotated(direction, frontImageId, 0, 0, 32, 27, 0, height, 0, 2, height + 27);
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        break;
    }

    if (trackSequence == 0) {
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
        }
    } else if (trackSequence == 3) {
        if (direction == 1 || direction == 2) {
            paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
        }
    }

    paint_util_set_general_support_height(height + 32, 0x20);
}

static void paint_log_flume_track_left_quarter_turn_3_tiles(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    static const uint32 imageIds[4][3] =
    {
        { SPR_LOG_FLUME_3_TURN_NW_SW_SEQ_0, SPR_LOG_FLUME_3_TURN_NW_SW_SEQ_2, SPR_LOG_FLUME_3_TURN_NW_SW_SEQ_3 },
        { SPR_LOG_FLUME_3_TURN_NE_NW_SEQ_0, SPR_LOG_FLUME_3_TURN_NE_NW_SEQ_2, SPR_LOG_FLUME_3_TURN_NE_NW_SEQ_3 },
        { SPR_LOG_FLUME_3_TURN_SE_NE_SEQ_0, SPR_LOG_FLUME_3_TURN_SE_NE_SEQ_2, SPR_LOG_FLUME_3_TURN_SE_NE_SEQ_3 },
        { SPR_LOG_FLUME_3_TURN_SW_SE_SEQ_0, SPR_LOG_FLUME_3_TURN_SW_SE_SEQ_2, SPR_LOG_FLUME_3_TURN_SW_SE_SEQ_3 },
    };

    static const uint32 imageIdsFront[4][3] =
    {
        { SPR_LOG_FLUME_3_TURN_FRONT_SW_SE_SEQ_0, SPR_LOG_FLUME_3_TURN_FRONT_SW_SE_SEQ_2, SPR_LOG_FLUME_3_TURN_FRONT_SW_SE_SEQ_3 },
        { SPR_LOG_FLUME_3_TURN_FRONT_SE_SW_SEQ_0, SPR_LOG_FLUME_3_TURN_FRONT_SE_SW_SEQ_2, SPR_LOG_FLUME_3_TURN_FRONT_SE_SW_SEQ_3 },
        { SPR_LOG_FLUME_3_TURN_FRONT_SW_NW_SEQ_0, SPR_LOG_FLUME_3_TURN_FRONT_SW_NW_SEQ_2, SPR_LOG_FLUME_3_TURN_FRONT_SW_NW_SEQ_3 },
        { SPR_LOG_FLUME_3_TURN_FRONT_NW_NE_SEQ_0, SPR_LOG_FLUME_3_TURN_FRONT_NW_NE_SEQ_2, SPR_LOG_FLUME_3_TURN_FRONT_NW_NE_SEQ_3 },
    };

    track_paint_util_left_quarter_turn_3_tiles_paint(2, height, direction, trackSequence, gTrackColours[SCHEME_TRACK], imageIds, get_current_rotation());
    track_paint_util_left_quarter_turn_3_tiles_paint_with_height_offset(0, height, direction, trackSequence, gTrackColours[SCHEME_TRACK], imageIdsFront, get_current_rotation(), 27);

    if (trackSequence != 1 && trackSequence != 2) {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
    }

    switch (trackSequence) {
    case 0:
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
        }
        break;
    case 3:
        if (direction == 2 || direction == 3) {
            paint_util_push_tunnel_rotated(direction ^ 1, height, TUNNEL_0);
        }
        break;
    }

    switch (trackSequence) {
    case 0: paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0); break;
    case 2: paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0); break;
    case 3: paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0); break;
    }

    paint_util_set_general_support_height(height + 32, 0x20);
}

static void paint_log_flume_track_right_quarter_turn_3_tiles(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    static const uint32 imageIds[4][3] =
    {
        { SPR_LOG_FLUME_3_TURN_NW_NE_SEQ_0, SPR_LOG_FLUME_3_TURN_NW_NE_SEQ_2, SPR_LOG_FLUME_3_TURN_NW_NE_SEQ_3 },
        { SPR_LOG_FLUME_3_TURN_NE_SE_SEQ_0, SPR_LOG_FLUME_3_TURN_NE_SE_SEQ_2, SPR_LOG_FLUME_3_TURN_NE_SE_SEQ_3 },
        { SPR_LOG_FLUME_3_TURN_SE_SW_SEQ_0, SPR_LOG_FLUME_3_TURN_SE_SW_SEQ_2, SPR_LOG_FLUME_3_TURN_SE_SW_SEQ_3 },
        { SPR_LOG_FLUME_3_TURN_SW_NW_SEQ_0, SPR_LOG_FLUME_3_TURN_SW_NW_SEQ_2, SPR_LOG_FLUME_3_TURN_SW_NW_SEQ_3 }
    };

    static const uint32 imageIdsFront[4][3] =
    {
        { SPR_LOG_FLUME_3_TURN_FRONT_NW_NE_SEQ_0, SPR_LOG_FLUME_3_TURN_FRONT_NW_NE_SEQ_2, SPR_LOG_FLUME_3_TURN_FRONT_NW_NE_SEQ_3 },
        { SPR_LOG_FLUME_3_TURN_FRONT_SW_SE_SEQ_0, SPR_LOG_FLUME_3_TURN_FRONT_SW_SE_SEQ_2, SPR_LOG_FLUME_3_TURN_FRONT_SW_SE_SEQ_3 },
        { SPR_LOG_FLUME_3_TURN_FRONT_SE_SW_SEQ_0, SPR_LOG_FLUME_3_TURN_FRONT_SE_SW_SEQ_2, SPR_LOG_FLUME_3_TURN_FRONT_SE_SW_SEQ_3 },
        { SPR_LOG_FLUME_3_TURN_FRONT_SW_NW_SEQ_0, SPR_LOG_FLUME_3_TURN_FRONT_SW_NW_SEQ_2, SPR_LOG_FLUME_3_TURN_FRONT_SW_NW_SEQ_3 },
    };

    track_paint_util_right_quarter_turn_3_tiles_paint_2(2, height, direction, trackSequence, gTrackColours[SCHEME_TRACK], imageIds, get_current_rotation());
    track_paint_util_right_quarter_turn_3_tiles_paint_2_with_height_offset(0, height, direction, trackSequence, gTrackColours[SCHEME_TRACK], imageIdsFront, get_current_rotation(), 27);

    if (trackSequence != 1 && trackSequence != 2) {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
    }

    switch (trackSequence) {
    case 0:
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
        }
        break;
    case 3:
        if (direction == 0 || direction == 1) {
            paint_util_push_tunnel_rotated(direction ^ 1, height, TUNNEL_0);
        }
        break;
    }

    switch (trackSequence) {
    case 0: paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0); break;
    case 2: paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0); break;
    case 3: paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0); break;
    }

    paint_util_set_general_support_height(height + 32, 0x20);
}

static void paint_log_flume_track_on_ride_photo(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    uint32 imageId = SPR_STATION_BASE_D | IMAGE_TYPE_REMAP;
    sub_98196C(imageId, 0, 0, 32, 32, 1, height, get_current_rotation());

    if (direction & 1) {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_FORK_ALT, 6, 6, height, gTrackColours[SCHEME_SUPPORTS]);
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_FORK_ALT, 7, 6, height, gTrackColours[SCHEME_SUPPORTS]);
    } else {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_FORK, 5, 6, height, gTrackColours[SCHEME_SUPPORTS]);
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_FORK, 8, 6, height, gTrackColours[SCHEME_SUPPORTS]);
    }

    imageId = LogFlumeTrackFlatImageIds[direction][0] | gTrackColours[SCHEME_TRACK];
    sub_98197C_rotated(direction, imageId, 0, 0, 32, 20, 0, height, 0, 6, height + 3);

    imageId = LogFlumeTrackFlatImageIds[direction][1] | gTrackColours[SCHEME_TRACK];
    sub_98197C_rotated(direction, imageId, 0, 0, 32, 1, 21, height, 0, 27, height + 5);

    track_paint_util_onride_photo_paint(direction, height + 3, mapElement);

    paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);
    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 48, 0x20);
}

static void paint_log_flume_track_reverser(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    static const uint32 imageIds[4][2] =
    {
        { SPR_LOG_FLUME_REVERSER_SW_NE, SPR_LOG_FLUME_REVERSER_FRONT_SW_NE },
        { SPR_LOG_FLUME_REVERSER_NW_SE, SPR_LOG_FLUME_REVERSER_FRONT_NW_SE },
        { SPR_LOG_FLUME_REVERSER_NE_SW, SPR_LOG_FLUME_REVERSER_FRONT_NE_SW },
        { SPR_LOG_FLUME_REVERSER_SE_NW, SPR_LOG_FLUME_REVERSER_FRONT_SE_NW },
    };

    uint32 imageId = imageIds[direction][0] | gTrackColours[SCHEME_TRACK];
    uint32 frontImageId = imageIds[direction][1] | gTrackColours[SCHEME_TRACK];

    sub_98197C_rotated(direction, imageId, 0, 0, 32, 20, 2, height, 0, 6, height);
    sub_98197C_rotated(direction, frontImageId, 0, 0, 32, 1, 26, height, 0, 27, height);

    metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);

    paint_util_push_tunnel_rotated(direction, height, TUNNEL_0);
    paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(height + 32, 0x20);
}

TRACK_PAINT_FUNCTION get_track_paint_function_log_flume(sint32 trackType, sint32 direction)
{
    switch (trackType) {
    case TRACK_ELEM_FLAT:
        return paint_log_flume_track_flat;

    case TRACK_ELEM_END_STATION:
    case TRACK_ELEM_BEGIN_STATION:
    case TRACK_ELEM_MIDDLE_STATION:
        return paint_log_flume_track_station;
    case TRACK_ELEM_25_DEG_UP:
        return paint_log_flume_track_25_deg_up;
    case TRACK_ELEM_FLAT_TO_25_DEG_UP:
        return paint_log_flume_track_flat_to_25_deg_up;
    case TRACK_ELEM_25_DEG_UP_TO_FLAT:
        return paint_log_flume_track_25_deg_up_to_flat;
    case TRACK_ELEM_25_DEG_DOWN:
        return paint_log_flume_track_25_deg_down;
    case TRACK_ELEM_FLAT_TO_25_DEG_DOWN:
        return paint_log_flume_track_flat_to_25_deg_down;
    case TRACK_ELEM_25_DEG_DOWN_TO_FLAT:
        return paint_log_flume_track_25_deg_down_to_flat;
    case TRACK_ELEM_S_BEND_LEFT:
        return paint_log_flume_track_s_bend_left;
    case TRACK_ELEM_S_BEND_RIGHT:
        return paint_log_flume_track_s_bend_right;
    case TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES:
        return paint_log_flume_track_left_quarter_turn_3_tiles;
    case TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES:
        return paint_log_flume_track_right_quarter_turn_3_tiles;
    case TRACK_ELEM_ON_RIDE_PHOTO:
        return paint_log_flume_track_on_ride_photo;
    case TRACK_ELEM_LOG_FLUME_REVERSER:
        return paint_log_flume_track_reverser;
    }

    return NULL;
}
