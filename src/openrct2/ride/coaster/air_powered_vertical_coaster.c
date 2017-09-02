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

enum {
    SPR_AIR_POWERED_VERTICAL_RC_FLAT_SW_NE = 22226,
    SPR_AIR_POWERED_VERTICAL_RC_FLAT_NW_SE = 22227,
    SPR_AIR_POWERED_VERTICAL_RC_STATION_SW_NE = 22228,
    SPR_AIR_POWERED_VERTICAL_RC_STATION_NW_SE = 22229,
    SPR_AIR_POWERED_VERTICAL_RC_BRAKES_NW_SE = 22230,
    SPR_AIR_POWERED_VERTICAL_RC_BRAKES_SW_NE = 22231,

    SPR_AIR_POWERED_VERTICAL_RC_VERTICAL_UP_SW_NE_SEQ_0 = 22236,
    SPR_AIR_POWERED_VERTICAL_RC_VERTICAL_UP_NW_SE_SEQ_0 = 22237,
    SPR_AIR_POWERED_VERTICAL_RC_VERTICAL_UP_NE_SW_SEQ_0 = 22238,
    SPR_AIR_POWERED_VERTICAL_RC_VERTICAL_UP_SE_NW_SEQ_0 = 22239,
    SPR_AIR_POWERED_VERTICAL_RC_VERTICAL_UP_SW_NE_SEQ_1 = 22232,
    SPR_AIR_POWERED_VERTICAL_RC_VERTICAL_UP_NW_SE_SEQ_1 = 22233,
    SPR_AIR_POWERED_VERTICAL_RC_VERTICAL_UP_NE_SW_SEQ_1 = 22234,
    SPR_AIR_POWERED_VERTICAL_RC_VERTICAL_UP_SE_NW_SEQ_1 = 22235,

    SPR_AIR_POWERED_VERTICAL_RC_FLAT_TO_LEFT_BANK_SW_NE = 22334,
    SPR_AIR_POWERED_VERTICAL_RC_FLAT_TO_LEFT_BANK_NW_SE = 22335,
    SPR_AIR_POWERED_VERTICAL_RC_FLAT_TO_LEFT_BANK_NE_SW = 22336,
    SPR_AIR_POWERED_VERTICAL_RC_FLAT_TO_LEFT_BANK_SE_NW = 22337,
    SPR_AIR_POWERED_VERTICAL_RC_FLAT_TO_RIGHT_BANK_SW_NE = 22338,
    SPR_AIR_POWERED_VERTICAL_RC_FLAT_TO_RIGHT_BANK_NW_SE = 22339,
    SPR_AIR_POWERED_VERTICAL_RC_FLAT_TO_RIGHT_BANK_NE_SW = 22340,
    SPR_AIR_POWERED_VERTICAL_RC_FLAT_TO_RIGHT_BANK_SE_NW = 22341,
    SPR_AIR_POWERED_VERTICAL_RC_FLAT_TO_LEFT_BANK_FRONT_SW_NE = 22342,
    SPR_AIR_POWERED_VERTICAL_RC_FLAT_TO_LEFT_BANK_FRONT_NW_SE = 22343,
    SPR_AIR_POWERED_VERTICAL_RC_FLAT_TO_LEFT_BANK_FRONT_NE_SW = 22344,
    SPR_AIR_POWERED_VERTICAL_RC_FLAT_TO_LEFT_BANK_FRONT_SE_NW = 22345,
    SPR_AIR_POWERED_VERTICAL_RC_LEFT_BANK_SW_NE = 22346,
    SPR_AIR_POWERED_VERTICAL_RC_LEFT_BANK_NW_SE = 22347,
    SPR_AIR_POWERED_VERTICAL_RC_LEFT_BANK_NE_SW = 22348,
    SPR_AIR_POWERED_VERTICAL_RC_LEFT_BANK_SE_NW = 22349,
    SPR_AIR_POWERED_VERTICAL_RC_VERTICAL_TOP_TRACK_SW_NE_SEQ_1 = 22350,
    SPR_AIR_POWERED_VERTICAL_RC_VERTICAL_TOP_TRACK_SW_NE_SEQ_0 = 22351,
    SPR_AIR_POWERED_VERTICAL_RC_VERTICAL_TOP_TRACK_SW_NE_SEQ_3 = 22352,
    SPR_AIR_POWERED_VERTICAL_RC_VERTICAL_TOP_TRACK_SW_NE_SEQ_2 = 22353,
    SPR_AIR_POWERED_VERTICAL_RC_VERTICAL_TOP_TRACK_NW_SE_SEQ_1 = 22354,
    SPR_AIR_POWERED_VERTICAL_RC_VERTICAL_TOP_TRACK_NW_SE_SEQ_0 = 22355,
    SPR_AIR_POWERED_VERTICAL_RC_VERTICAL_TOP_TRACK_NW_SE_SEQ_3 = 22356,
    SPR_AIR_POWERED_VERTICAL_RC_VERTICAL_TOP_TRACK_NW_SE_SEQ_2 = 22357,
    SPR_AIR_POWERED_VERTICAL_RC_VERTICAL_TOP_SUPPORT_SW_NE = 22358,
    SPR_AIR_POWERED_VERTICAL_RC_VERTICAL_TOP_SUPPORT_NW_SE = 22359,
    SPR_AIR_POWERED_VERTICAL_RC_VERTICAL_TOP_SUPPORT_NE_SW = 22360,
    SPR_AIR_POWERED_VERTICAL_RC_VERTICAL_TOP_SUPPORT_SE_NW = 22361,

    SPR_AIR_POWERED_VERTICAL_RC_SLOPE_SW_NE_0 = 22240,
    SPR_AIR_POWERED_VERTICAL_RC_SLOPE_SW_NE_1 = 22241,
    SPR_AIR_POWERED_VERTICAL_RC_SLOPE_SW_NE_2 = 22242,
    SPR_AIR_POWERED_VERTICAL_RC_SLOPE_SW_NE_3 = 22243,
    SPR_AIR_POWERED_VERTICAL_RC_SLOPE_SW_NE_4 = 22244,
    SPR_AIR_POWERED_VERTICAL_RC_SLOPE_SW_NE_6 = 22245,
    SPR_AIR_POWERED_VERTICAL_RC_SLOPE_NW_SE_0 = 22246,
    SPR_AIR_POWERED_VERTICAL_RC_SLOPE_NW_SE_1 = 22247,
    SPR_AIR_POWERED_VERTICAL_RC_SLOPE_NW_SE_2 = 22248,
    SPR_AIR_POWERED_VERTICAL_RC_SLOPE_NW_SE_3 = 22249,
    SPR_AIR_POWERED_VERTICAL_RC_SLOPE_NW_SE_4 = 22250,
    SPR_AIR_POWERED_VERTICAL_RC_SLOPE_NW_SE_6 = 22251,
    SPR_AIR_POWERED_VERTICAL_RC_SLOPE_NE_SW_0 = 22252,
    SPR_AIR_POWERED_VERTICAL_RC_SLOPE_NE_SW_1 = 22253,
    SPR_AIR_POWERED_VERTICAL_RC_SLOPE_NE_SW_2 = 22254,
    SPR_AIR_POWERED_VERTICAL_RC_SLOPE_NE_SW_3 = 22255,
    SPR_AIR_POWERED_VERTICAL_RC_SLOPE_NE_SW_4 = 22256,
    SPR_AIR_POWERED_VERTICAL_RC_SLOPE_NE_SW_6 = 22257,
    SPR_AIR_POWERED_VERTICAL_RC_SLOPE_SE_NW_0 = 22258,
    SPR_AIR_POWERED_VERTICAL_RC_SLOPE_SE_NW_1 = 22259,
    SPR_AIR_POWERED_VERTICAL_RC_SLOPE_SE_NW_2 = 22260,
    SPR_AIR_POWERED_VERTICAL_RC_SLOPE_SE_NW_3 = 22261,
    SPR_AIR_POWERED_VERTICAL_RC_SLOPE_SE_NW_4 = 22262,
    SPR_AIR_POWERED_VERTICAL_RC_SLOPE_SE_NW_6 = 22263,
    SPR_AIR_POWERED_VERTICAL_RC_SLOPE_SUPPORTS_SW_NE_0 = 22264,
    SPR_AIR_POWERED_VERTICAL_RC_SLOPE_SUPPORTS_SW_NE_1 = 22265,
    SPR_AIR_POWERED_VERTICAL_RC_SLOPE_SUPPORTS_SW_NE_2 = 22266,
    SPR_AIR_POWERED_VERTICAL_RC_SLOPE_SUPPORTS_SW_NE_3 = 22267,
    SPR_AIR_POWERED_VERTICAL_RC_SLOPE_SUPPORTS_SW_NE_4 = 22268,
    SPR_AIR_POWERED_VERTICAL_RC_SLOPE_SUPPORTS_SW_NE_6 = 22269,
    SPR_AIR_POWERED_VERTICAL_RC_SLOPE_SUPPORTS_SW_NE_5 = 22270,
    SPR_AIR_POWERED_VERTICAL_RC_SLOPE_SUPPORTS_NW_SE_0 = 22271,
    SPR_AIR_POWERED_VERTICAL_RC_SLOPE_SUPPORTS_NW_SE_1 = 22272,
    SPR_AIR_POWERED_VERTICAL_RC_SLOPE_SUPPORTS_NW_SE_2 = 22273,
    SPR_AIR_POWERED_VERTICAL_RC_SLOPE_SUPPORTS_NW_SE_3 = 22274,
    SPR_AIR_POWERED_VERTICAL_RC_SLOPE_SUPPORTS_NW_SE_4 = 22275,
    SPR_AIR_POWERED_VERTICAL_RC_SLOPE_SUPPORTS_NW_SE_6 = 22276,
    SPR_AIR_POWERED_VERTICAL_RC_SLOPE_SUPPORTS_NW_SE_5 = 22277,
    SPR_AIR_POWERED_VERTICAL_RC_SLOPE_SUPPORTS_NE_SW_0 = 22278,
    SPR_AIR_POWERED_VERTICAL_RC_SLOPE_SUPPORTS_NE_SW_1 = 22279,
    SPR_AIR_POWERED_VERTICAL_RC_SLOPE_SUPPORTS_NE_SW_2 = 22280,
    SPR_AIR_POWERED_VERTICAL_RC_SLOPE_SUPPORTS_NE_SW_3 = 22281,
    SPR_AIR_POWERED_VERTICAL_RC_SLOPE_SUPPORTS_NE_SW_4 = 22282,
    SPR_AIR_POWERED_VERTICAL_RC_SLOPE_SUPPORTS_NE_SW_6 = 22283,
    SPR_AIR_POWERED_VERTICAL_RC_SLOPE_SUPPORTS_NE_SW_5 = 22284,
    SPR_AIR_POWERED_VERTICAL_RC_SLOPE_SUPPORTS_SE_NW_0 = 22285,
    SPR_AIR_POWERED_VERTICAL_RC_SLOPE_SUPPORTS_SE_NW_1 = 22286,
    SPR_AIR_POWERED_VERTICAL_RC_SLOPE_SUPPORTS_SE_NW_2 = 22287,
    SPR_AIR_POWERED_VERTICAL_RC_SLOPE_SUPPORTS_SE_NW_3 = 22288,
    SPR_AIR_POWERED_VERTICAL_RC_SLOPE_SUPPORTS_SE_NW_4 = 22289,
    SPR_AIR_POWERED_VERTICAL_RC_SLOPE_SUPPORTS_SE_NW_6 = 22290,
    SPR_AIR_POWERED_VERTICAL_RC_SLOPE_SUPPORTS_SE_NW_5 = 22291,
    SPR_AIR_POWERED_VERTICAL_RC_QUARTER_TURN_5_SW_SE_PART_0 = 22292,
    SPR_AIR_POWERED_VERTICAL_RC_QUARTER_TURN_5_SW_SE_PART_1 = 22293,
    SPR_AIR_POWERED_VERTICAL_RC_QUARTER_TURN_5_SW_SE_PART_2 = 22294,
    SPR_AIR_POWERED_VERTICAL_RC_QUARTER_TURN_5_SW_SE_PART_3 = 22295,
    SPR_AIR_POWERED_VERTICAL_RC_QUARTER_TURN_5_SW_SE_PART_4 = 22296,
    SPR_AIR_POWERED_VERTICAL_RC_QUARTER_TURN_5_NW_SW_PART_0 = 22297,
    SPR_AIR_POWERED_VERTICAL_RC_QUARTER_TURN_5_NW_SW_PART_1 = 22298,
    SPR_AIR_POWERED_VERTICAL_RC_QUARTER_TURN_5_NW_SW_PART_2 = 22299,
    SPR_AIR_POWERED_VERTICAL_RC_QUARTER_TURN_5_NW_SW_PART_3 = 22300,
    SPR_AIR_POWERED_VERTICAL_RC_QUARTER_TURN_5_NW_SW_PART_4 = 22301,
    SPR_AIR_POWERED_VERTICAL_RC_QUARTER_TURN_5_NE_NW_PART_0 = 22302,
    SPR_AIR_POWERED_VERTICAL_RC_QUARTER_TURN_5_NE_NW_PART_1 = 22303,
    SPR_AIR_POWERED_VERTICAL_RC_QUARTER_TURN_5_NE_NW_PART_2 = 22304,
    SPR_AIR_POWERED_VERTICAL_RC_QUARTER_TURN_5_NE_NW_PART_3 = 22305,
    SPR_AIR_POWERED_VERTICAL_RC_QUARTER_TURN_5_NE_NW_PART_4 = 22306,
    SPR_AIR_POWERED_VERTICAL_RC_QUARTER_TURN_5_SE_NE_PART_0 = 22307,
    SPR_AIR_POWERED_VERTICAL_RC_QUARTER_TURN_5_SE_NE_PART_1 = 22308,
    SPR_AIR_POWERED_VERTICAL_RC_QUARTER_TURN_5_SE_NE_PART_2 = 22309,
    SPR_AIR_POWERED_VERTICAL_RC_QUARTER_TURN_5_SE_NE_PART_3 = 22310,
    SPR_AIR_POWERED_VERTICAL_RC_QUARTER_TURN_5_SE_NE_PART_4 = 22311,
    SPR_AIR_POWERED_VERTICAL_RC_BANKED_QUARTER_TURN_5_SW_SE_PART_0 = 22312,
    SPR_AIR_POWERED_VERTICAL_RC_BANKED_QUARTER_TURN_5_SW_SE_PART_1 = 22313,
    SPR_AIR_POWERED_VERTICAL_RC_BANKED_QUARTER_TURN_5_SW_SE_PART_2 = 22314,
    SPR_AIR_POWERED_VERTICAL_RC_BANKED_QUARTER_TURN_5_SW_SE_PART_3 = 22315,
    SPR_AIR_POWERED_VERTICAL_RC_BANKED_QUARTER_TURN_5_SW_SE_PART_4 = 22316,
    SPR_AIR_POWERED_VERTICAL_RC_BANKED_QUARTER_TURN_5_NW_SW_PART_0 = 22317,
    SPR_AIR_POWERED_VERTICAL_RC_BANKED_QUARTER_TURN_5_NW_SW_PART_1 = 22318,
    SPR_AIR_POWERED_VERTICAL_RC_BANKED_QUARTER_TURN_5_NW_SW_PART_2 = 22319,
    SPR_AIR_POWERED_VERTICAL_RC_BANKED_QUARTER_TURN_5_NW_SW_PART_3 = 22320,
    SPR_AIR_POWERED_VERTICAL_RC_BANKED_QUARTER_TURN_5_NW_SW_PART_4 = 22321,
    SPR_AIR_POWERED_VERTICAL_RC_BANKED_QUARTER_TURN_5_NE_NW_PART_0 = 22322,
    SPR_AIR_POWERED_VERTICAL_RC_BANKED_QUARTER_TURN_5_NE_NW_PART_1 = 22323,
    SPR_AIR_POWERED_VERTICAL_RC_BANKED_QUARTER_TURN_5_NE_NW_PART_2 = 22324,
    SPR_AIR_POWERED_VERTICAL_RC_BANKED_QUARTER_TURN_5_NE_NW_PART_3 = 22325,
    SPR_AIR_POWERED_VERTICAL_RC_BANKED_QUARTER_TURN_5_NE_NW_PART_4 = 22326,
    SPR_AIR_POWERED_VERTICAL_RC_BANKED_QUARTER_TURN_5_SE_NE_PART_0 = 22327,
    SPR_AIR_POWERED_VERTICAL_RC_BANKED_QUARTER_TURN_5_SE_NE_PART_1 = 22328,
    SPR_AIR_POWERED_VERTICAL_RC_BANKED_QUARTER_TURN_5_SE_NE_PART_2 = 22329,
    SPR_AIR_POWERED_VERTICAL_RC_BANKED_QUARTER_TURN_5_SE_NE_PART_3 = 22330,
    SPR_AIR_POWERED_VERTICAL_RC_BANKED_QUARTER_TURN_5_SE_NE_PART_4 = 22331,
    SPR_AIR_POWERED_VERTICAL_RC_BANKED_QUARTER_TURN_5_FRONT_NW_SW_PART_4 = 22332,
    SPR_AIR_POWERED_VERTICAL_RC_BANKED_QUARTER_TURN_5_FRONT_SE_NE_PART_0 = 22333,
};

static uint32 air_powered_vertical_rc_get_support_colour()
{
    uint32 colourFlags = gTrackColours[SCHEME_SUPPORTS];
    uint32 trackColour = gTrackColours[SCHEME_TRACK];
    if (trackColour & IMAGE_TYPE_REMAP_2_PLUS) {
        colourFlags |= (trackColour & 0x9F000000);
    }
    return colourFlags;
}

/** rct2: 0x008AFAD4 */
static void air_powered_vertical_rc_track_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    static const uint32 imageIds[4] = {
        SPR_AIR_POWERED_VERTICAL_RC_FLAT_SW_NE,
        SPR_AIR_POWERED_VERTICAL_RC_FLAT_NW_SE,
        SPR_AIR_POWERED_VERTICAL_RC_FLAT_SW_NE,
        SPR_AIR_POWERED_VERTICAL_RC_FLAT_NW_SE,
    };

    uint32 imageId = imageIds[direction] | gTrackColours[SCHEME_TRACK];
    sub_98197C_rotated(direction, imageId, 0, 0, 32, 20, 1, height, 0, 6, height);

    wooden_a_supports_paint_setup(direction & 1, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);

    paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);

    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 32, 0x20);
}

static void air_powered_vertical_rc_track_station(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    static const uint32 imageIds[4][2] = {
        { SPR_AIR_POWERED_VERTICAL_RC_STATION_SW_NE, SPR_STATION_BASE_B_SW_NE },
        { SPR_AIR_POWERED_VERTICAL_RC_STATION_NW_SE, SPR_STATION_BASE_B_NW_SE },
        { SPR_AIR_POWERED_VERTICAL_RC_STATION_SW_NE, SPR_STATION_BASE_B_SW_NE },
        { SPR_AIR_POWERED_VERTICAL_RC_STATION_NW_SE, SPR_STATION_BASE_B_NW_SE },
    };

    sub_98197C_rotated(direction, imageIds[direction][1] | gTrackColours[SCHEME_MISC], 0, 0, 32, 28, 1, height - 2, 0, 2, height);
    sub_98199C_rotated(direction, imageIds[direction][0] | gTrackColours[SCHEME_TRACK], 0, 0, 32, 20, 1, height, 0, 6, height);

    wooden_a_supports_paint_setup(direction & 1, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);

    rct_ride * ride = get_ride(rideIndex);
    track_paint_util_draw_station_platform(ride, direction, height, 5, mapElement);

    paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);

    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 32, 0x20);
}

static void air_powered_vertical_rc_track_right_quarter_turn_5(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    static const sprite_bb imageIds[4][5] = {
        {
            { SPR_AIR_POWERED_VERTICAL_RC_QUARTER_TURN_5_SW_SE_PART_0, {  0,  2, 0 }, { 0, 0, 0 }, { 32, 32, 2 } },
            { SPR_AIR_POWERED_VERTICAL_RC_QUARTER_TURN_5_SW_SE_PART_1, {  0, 16, 0 }, { 0, 0, 0 }, { 32, 16, 2 } },
            { SPR_AIR_POWERED_VERTICAL_RC_QUARTER_TURN_5_SW_SE_PART_2, {  0,  0, 0 }, { 0, 0, 0 }, { 16, 16, 2 } },
            { SPR_AIR_POWERED_VERTICAL_RC_QUARTER_TURN_5_SW_SE_PART_3, { 16,  0, 0 }, { 0, 0, 0 }, { 16, 32, 2 } },
            { SPR_AIR_POWERED_VERTICAL_RC_QUARTER_TURN_5_SW_SE_PART_4, {  2,  0, 0 }, { 0, 0, 0 }, { 32, 32, 2 } },
        },
        {
            { SPR_AIR_POWERED_VERTICAL_RC_QUARTER_TURN_5_NW_SW_PART_0, { 2,   0, 0 }, { 0, 0, 0 }, { 32, 32, 2 } },
            { SPR_AIR_POWERED_VERTICAL_RC_QUARTER_TURN_5_NW_SW_PART_1, { 16,  0, 0 }, { 0, 0, 0 }, { 16, 34, 2 } },
            { SPR_AIR_POWERED_VERTICAL_RC_QUARTER_TURN_5_NW_SW_PART_2, { 0,  16, 0 }, { 0, 0, 0 }, { 16, 16, 2 } },
            { SPR_AIR_POWERED_VERTICAL_RC_QUARTER_TURN_5_NW_SW_PART_3, { 0,   0, 0 }, { 0, 0, 0 }, { 32, 16, 2 } },
            { SPR_AIR_POWERED_VERTICAL_RC_QUARTER_TURN_5_NW_SW_PART_4, { 0,   2, 0 }, { 0, 0, 0 }, { 32, 27, 2 } },
        },
        {
            { SPR_AIR_POWERED_VERTICAL_RC_QUARTER_TURN_5_NE_NW_PART_0, { 0,   2, 0 }, { 0, 0, 0 }, { 32, 27, 2 } },
            { SPR_AIR_POWERED_VERTICAL_RC_QUARTER_TURN_5_NE_NW_PART_1, { 0,   0, 0 }, { 0, 0, 0 }, { 32, 16, 2 } },
            { SPR_AIR_POWERED_VERTICAL_RC_QUARTER_TURN_5_NE_NW_PART_2, { 16, 16, 0 }, { 0, 0, 0 }, { 16, 16, 2 } },
            { SPR_AIR_POWERED_VERTICAL_RC_QUARTER_TURN_5_NE_NW_PART_3, { 0,   0, 0 }, { 0, 0, 0 }, { 16, 32, 2 } },
            { SPR_AIR_POWERED_VERTICAL_RC_QUARTER_TURN_5_NE_NW_PART_4, { 2,   0, 0 }, { 0, 0, 0 }, { 27, 32, 2 } },
        },
        {
            { SPR_AIR_POWERED_VERTICAL_RC_QUARTER_TURN_5_SE_NE_PART_0, {  2,  0, 0 }, { 0, 0, 0 }, { 27, 32, 2 } },
            { SPR_AIR_POWERED_VERTICAL_RC_QUARTER_TURN_5_SE_NE_PART_1, {  0,  0, 0 }, { 0, 0, 0 }, { 16, 32, 2 } },
            { SPR_AIR_POWERED_VERTICAL_RC_QUARTER_TURN_5_SE_NE_PART_2, { 16,  0, 0 }, { 0, 0, 0 }, { 16, 16, 2 } },
            { SPR_AIR_POWERED_VERTICAL_RC_QUARTER_TURN_5_SE_NE_PART_3, {  0, 16, 0 }, { 0, 0, 0 }, { 32, 16, 2 } },
            { SPR_AIR_POWERED_VERTICAL_RC_QUARTER_TURN_5_SE_NE_PART_4, {  0,  2, 0 }, { 0, 0, 0 }, { 32, 32, 2 } },
        }
    };

    track_paint_util_right_quarter_turn_5_tiles_paint_3(height, direction, get_current_rotation(), trackSequence, gTrackColours[SCHEME_TRACK], imageIds);
    track_paint_util_right_quarter_turn_5_tiles_wooden_supports(height, direction, trackSequence);
    track_paint_util_right_quarter_turn_5_tiles_tunnel(height, direction, trackSequence, TUNNEL_6);

    switch (trackSequence) {
    case 0: paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0); break;
    case 1: paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0); break;
    case 2: paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_D4 | SEGMENT_BC | SEGMENT_C0 | SEGMENT_CC, direction), 0xFFFF, 0); break;
    case 3: paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_C4 | SEGMENT_B8 | SEGMENT_BC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0); break;
    case 4: paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0); break;
    case 5: paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_D4 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C8, direction), 0xFFFF, 0); break;
    case 6: paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0); break;
    }

    paint_util_set_general_support_height(height + 32, 0x20);
}

static void air_powered_vertical_rc_track_left_quarter_turn_5(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    air_powered_vertical_rc_track_right_quarter_turn_5(session, rideIndex, trackSequence, (direction + 1) % 4, height, mapElement);
}

/** rct2: 0x008AFB74 */
static void air_powered_vertical_rc_track_flat_to_left_bank(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    static const uint32 imageIds[4][2] = {
        { SPR_AIR_POWERED_VERTICAL_RC_FLAT_TO_LEFT_BANK_SW_NE, SPR_AIR_POWERED_VERTICAL_RC_FLAT_TO_LEFT_BANK_FRONT_SW_NE },
        { SPR_AIR_POWERED_VERTICAL_RC_FLAT_TO_LEFT_BANK_NW_SE, SPR_AIR_POWERED_VERTICAL_RC_FLAT_TO_LEFT_BANK_FRONT_NW_SE },
        { SPR_AIR_POWERED_VERTICAL_RC_FLAT_TO_LEFT_BANK_NE_SW, SPR_AIR_POWERED_VERTICAL_RC_FLAT_TO_LEFT_BANK_FRONT_NE_SW },
        { SPR_AIR_POWERED_VERTICAL_RC_FLAT_TO_LEFT_BANK_SE_NW, SPR_AIR_POWERED_VERTICAL_RC_FLAT_TO_LEFT_BANK_FRONT_SE_NW },
    };

    uint32 imageId = imageIds[direction][0] | gTrackColours[SCHEME_TRACK];
    sub_98197C_rotated(direction, imageId, 0, 0, 32, 20, 3, height, 0, 6, height);

    if (direction == 0 || direction == 1) {
        imageId = imageIds[direction][1] | gTrackColours[SCHEME_TRACK];
        sub_98197C_rotated(direction, imageId, 0, 0, 32, 1, 26, height, 0, 27, height);
    }

    wooden_a_supports_paint_setup(direction & 1, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);

    paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);

    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 32, 0x20);
}

/** rct2: 0x008AFB84 */
static void air_powered_vertical_rc_track_flat_to_right_bank(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    static const uint32 imageIds[4][2] = {
        { SPR_AIR_POWERED_VERTICAL_RC_FLAT_TO_RIGHT_BANK_SW_NE, SPR_AIR_POWERED_VERTICAL_RC_FLAT_TO_LEFT_BANK_FRONT_SW_NE },
        { SPR_AIR_POWERED_VERTICAL_RC_FLAT_TO_RIGHT_BANK_NW_SE, SPR_AIR_POWERED_VERTICAL_RC_FLAT_TO_LEFT_BANK_FRONT_NW_SE },
        { SPR_AIR_POWERED_VERTICAL_RC_FLAT_TO_RIGHT_BANK_NE_SW, SPR_AIR_POWERED_VERTICAL_RC_FLAT_TO_LEFT_BANK_FRONT_NE_SW },
        { SPR_AIR_POWERED_VERTICAL_RC_FLAT_TO_RIGHT_BANK_SE_NW, SPR_AIR_POWERED_VERTICAL_RC_FLAT_TO_LEFT_BANK_FRONT_SE_NW },
    };

    uint32 imageId = imageIds[direction][0] | gTrackColours[SCHEME_TRACK];
    sub_98197C_rotated(direction, imageId, 0, 0, 32, 20, 3, height, 0, 6, height);

    if (direction == 2 || direction == 3) {
        imageId = imageIds[direction][1] | gTrackColours[SCHEME_TRACK];
        sub_98197C_rotated(direction, imageId, 0, 0, 32, 1, 26, height, 0, 27, height);
    }

    wooden_a_supports_paint_setup(direction & 1, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);

    paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);

    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 32, 0x20);
}

static void air_powered_vertical_rc_track_left_bank_to_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    air_powered_vertical_rc_track_flat_to_right_bank(session, rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008AFBA4 */
static void air_powered_vertical_rc_track_right_bank_to_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    air_powered_vertical_rc_track_flat_to_left_bank(session, rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

static void air_powered_vertical_rc_track_banked_right_quarter_turn_5(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    static const sprite_bb imageIds[4][5] = {
        {
            { SPR_AIR_POWERED_VERTICAL_RC_BANKED_QUARTER_TURN_5_SW_SE_PART_0, { 0, 0, 0 }, {  0,  6, 0 }, { 32, 20, 3 } },
            { SPR_AIR_POWERED_VERTICAL_RC_BANKED_QUARTER_TURN_5_SW_SE_PART_1, { 0, 0, 0 }, {  0, 16, 0 }, { 32, 16, 3 } },
            { SPR_AIR_POWERED_VERTICAL_RC_BANKED_QUARTER_TURN_5_SW_SE_PART_2, { 0, 0, 0 }, {  0,  0, 0 }, { 16, 16, 3 } },
            { SPR_AIR_POWERED_VERTICAL_RC_BANKED_QUARTER_TURN_5_SW_SE_PART_3, { 0, 0, 0 }, { 16,  0, 0 }, { 16, 32, 3 } },
            { SPR_AIR_POWERED_VERTICAL_RC_BANKED_QUARTER_TURN_5_SW_SE_PART_4, { 0, 0, 0 }, {  6,  0, 0 }, { 20, 32, 3 } },
        },
        {
            { SPR_AIR_POWERED_VERTICAL_RC_BANKED_QUARTER_TURN_5_NW_SW_PART_0, { 0, 0, 0 }, {  6,  0, 0 }, { 20, 32, 3 } },
            { SPR_AIR_POWERED_VERTICAL_RC_BANKED_QUARTER_TURN_5_NW_SW_PART_1, { 0, 0, 0 }, { 16,  0, 0 }, { 16, 32, 3 } },
            { SPR_AIR_POWERED_VERTICAL_RC_BANKED_QUARTER_TURN_5_NW_SW_PART_2, { 0, 0, 0 }, {  0, 16, 0 }, { 16, 16, 3 } },
            { SPR_AIR_POWERED_VERTICAL_RC_BANKED_QUARTER_TURN_5_NW_SW_PART_3, { 0, 0, 0 }, {  0,  0, 0 }, { 32, 16, 3 } },
            { SPR_AIR_POWERED_VERTICAL_RC_BANKED_QUARTER_TURN_5_NW_SW_PART_4, { 0, 0, 0 }, {  0,  6, 0 }, { 32, 20, 3 } },
        },
        {
            { SPR_AIR_POWERED_VERTICAL_RC_BANKED_QUARTER_TURN_5_NE_NW_PART_0, { 0, 0, 0 }, {  0, 27,  0 }, { 32,  1, 26 } },
            { SPR_AIR_POWERED_VERTICAL_RC_BANKED_QUARTER_TURN_5_NE_NW_PART_1, { 0, 0, 0 }, {  0,  0, 27 }, { 32, 16,  1 } },
            { SPR_AIR_POWERED_VERTICAL_RC_BANKED_QUARTER_TURN_5_NE_NW_PART_2, { 0, 0, 0 }, { 16, 16, 27 }, { 16, 16,  1 } },
            { SPR_AIR_POWERED_VERTICAL_RC_BANKED_QUARTER_TURN_5_NE_NW_PART_3, { 0, 0, 0 }, {  0,  0, 27 }, { 16, 32,  1 } },
            { SPR_AIR_POWERED_VERTICAL_RC_BANKED_QUARTER_TURN_5_NE_NW_PART_4, { 0, 0, 0 }, { 27,  0,  0 }, {  1, 32, 26 } },
        },
        {
            { SPR_AIR_POWERED_VERTICAL_RC_BANKED_QUARTER_TURN_5_SE_NE_PART_0, { 0, 0, 0 }, {  6,  0, 0 }, { 20, 32, 3 } },
            { SPR_AIR_POWERED_VERTICAL_RC_BANKED_QUARTER_TURN_5_SE_NE_PART_1, { 0, 0, 0 }, {  0,  0, 0 }, { 16, 32, 3 } },
            { SPR_AIR_POWERED_VERTICAL_RC_BANKED_QUARTER_TURN_5_SE_NE_PART_2, { 0, 0, 0 }, { 16,  0, 0 }, { 16, 16, 3 } },
            { SPR_AIR_POWERED_VERTICAL_RC_BANKED_QUARTER_TURN_5_SE_NE_PART_3, { 0, 0, 0 }, {  0, 16, 0 }, { 32, 16, 3 } },
            { SPR_AIR_POWERED_VERTICAL_RC_BANKED_QUARTER_TURN_5_SE_NE_PART_4, { 0, 0, 0 }, {  0,  6, 0 }, { 32, 20, 3 } },
        }
    };

    track_paint_util_right_quarter_turn_5_tiles_paint_2(height, direction, get_current_rotation(), trackSequence, gTrackColours[SCHEME_TRACK], imageIds);

    if (direction == 1 && trackSequence == 6) {
        uint32 imageId = SPR_AIR_POWERED_VERTICAL_RC_BANKED_QUARTER_TURN_5_FRONT_NW_SW_PART_4 | gTrackColours[SCHEME_TRACK];
        sub_98197C(imageId, 0, 0, 32, 1, 26, height, 0, 27, height, get_current_rotation());
    } else if (direction == 3 && trackSequence == 0) {
        uint32 imageId = SPR_AIR_POWERED_VERTICAL_RC_BANKED_QUARTER_TURN_5_FRONT_SE_NE_PART_0 | gTrackColours[SCHEME_TRACK];
        sub_98197C(imageId, 0, 0, 1, 32, 26, height, 27, 0, height, get_current_rotation());
    }

    track_paint_util_right_quarter_turn_5_tiles_wooden_supports(height, direction, trackSequence);
    track_paint_util_right_quarter_turn_5_tiles_tunnel(height, direction, trackSequence, TUNNEL_6);

    switch (trackSequence) {
    case 0: paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0); break;
    case 1: paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0); break;
    case 2: paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_D4 | SEGMENT_BC | SEGMENT_C0 | SEGMENT_CC, direction), 0xFFFF, 0); break;
    case 3: paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_C4 | SEGMENT_B8 | SEGMENT_BC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0); break;
    case 4: paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0); break;
    case 5: paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_D4 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C8, direction), 0xFFFF, 0); break;
    case 6: paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0); break;
    }

    paint_util_set_general_support_height(height + 32, 0x20);
}

static void air_powered_vertical_rc_track_banked_left_quarter_turn_5(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    air_powered_vertical_rc_track_banked_right_quarter_turn_5(session, rideIndex, trackSequence, (direction + 1) % 4, height, mapElement);
}

/** rct2: 0x008AFBD4 */
static void air_powered_vertical_rc_track_left_bank(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    static const uint32 imageIds[4] = {
        SPR_AIR_POWERED_VERTICAL_RC_LEFT_BANK_SW_NE,
        SPR_AIR_POWERED_VERTICAL_RC_LEFT_BANK_NW_SE,
        SPR_AIR_POWERED_VERTICAL_RC_LEFT_BANK_NE_SW,
        SPR_AIR_POWERED_VERTICAL_RC_LEFT_BANK_SE_NW,
    };

    uint32 imageId = imageIds[direction] | gTrackColours[SCHEME_TRACK];
    if (direction == 0 || direction == 1) {
        sub_98197C_rotated(direction, imageId, 0, 0, 32, 1, 26, height, 0, 27, height);
    } else {
        sub_98197C_rotated(direction, imageId, 0, 0, 32, 20, 3, height, 0, 6, height);
    }

    wooden_a_supports_paint_setup(direction & 1, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);

    paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);

    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 32, 0x20);
}

static void air_powered_vertical_rc_track_right_bank(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    air_powered_vertical_rc_track_left_bank(session, rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

static void air_powered_vertical_rc_track_brakes(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    static const uint32 imageIds[4] = {
        SPR_AIR_POWERED_VERTICAL_RC_BRAKES_NW_SE,
        SPR_AIR_POWERED_VERTICAL_RC_BRAKES_SW_NE,
        SPR_AIR_POWERED_VERTICAL_RC_BRAKES_NW_SE,
        SPR_AIR_POWERED_VERTICAL_RC_BRAKES_SW_NE,
    };

    uint32 imageId = imageIds[direction] | gTrackColours[SCHEME_TRACK];
    sub_98197C_rotated(direction, imageId, 0, 0, 32, 20, 1, height, 0, 6, height);

    wooden_a_supports_paint_setup(direction & 1, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);

    paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);

    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 32, 0x20);
}

static void air_powered_vertical_rc_track_vertical_slope_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    static const uint32 trackImageIds[7][4] = {
        {
            SPR_AIR_POWERED_VERTICAL_RC_SLOPE_SW_NE_0,
            SPR_AIR_POWERED_VERTICAL_RC_SLOPE_NW_SE_0,
            SPR_AIR_POWERED_VERTICAL_RC_SLOPE_NE_SW_0,
            SPR_AIR_POWERED_VERTICAL_RC_SLOPE_SE_NW_0,
        },
        {
            SPR_AIR_POWERED_VERTICAL_RC_SLOPE_SW_NE_1,
            SPR_AIR_POWERED_VERTICAL_RC_SLOPE_NW_SE_1,
            SPR_AIR_POWERED_VERTICAL_RC_SLOPE_NE_SW_1,
            SPR_AIR_POWERED_VERTICAL_RC_SLOPE_SE_NW_1,
        },
        {
            SPR_AIR_POWERED_VERTICAL_RC_SLOPE_SW_NE_2,
            SPR_AIR_POWERED_VERTICAL_RC_SLOPE_NW_SE_2,
            SPR_AIR_POWERED_VERTICAL_RC_SLOPE_NE_SW_2,
            SPR_AIR_POWERED_VERTICAL_RC_SLOPE_SE_NW_2,
        },
        {
            SPR_AIR_POWERED_VERTICAL_RC_SLOPE_SW_NE_3,
            SPR_AIR_POWERED_VERTICAL_RC_SLOPE_NW_SE_3,
            SPR_AIR_POWERED_VERTICAL_RC_SLOPE_NE_SW_3,
            SPR_AIR_POWERED_VERTICAL_RC_SLOPE_SE_NW_3,
        },
        {
            SPR_AIR_POWERED_VERTICAL_RC_SLOPE_SW_NE_4,
            SPR_AIR_POWERED_VERTICAL_RC_SLOPE_NW_SE_4,
            SPR_AIR_POWERED_VERTICAL_RC_SLOPE_NE_SW_4,
            SPR_AIR_POWERED_VERTICAL_RC_SLOPE_SE_NW_4,
        },
        {
            0,
            0,
            0,
            0
        },
        {
            SPR_AIR_POWERED_VERTICAL_RC_SLOPE_SW_NE_6,
            SPR_AIR_POWERED_VERTICAL_RC_SLOPE_NW_SE_6,
            SPR_AIR_POWERED_VERTICAL_RC_SLOPE_NE_SW_6,
            SPR_AIR_POWERED_VERTICAL_RC_SLOPE_SE_NW_6,
        },
    };

    static const uint32 supportImageIds[7][4] = {
        {
            22264,
            SPR_AIR_POWERED_VERTICAL_RC_SLOPE_SUPPORTS_NW_SE_0,
            SPR_AIR_POWERED_VERTICAL_RC_SLOPE_SUPPORTS_NE_SW_0,
            SPR_AIR_POWERED_VERTICAL_RC_SLOPE_SUPPORTS_SE_NW_0,
        },
        {
            SPR_AIR_POWERED_VERTICAL_RC_SLOPE_SUPPORTS_SW_NE_1,
            SPR_AIR_POWERED_VERTICAL_RC_SLOPE_SUPPORTS_NW_SE_1,
            SPR_AIR_POWERED_VERTICAL_RC_SLOPE_SUPPORTS_NE_SW_1,
            SPR_AIR_POWERED_VERTICAL_RC_SLOPE_SUPPORTS_SE_NW_1,
        },
        {
            SPR_AIR_POWERED_VERTICAL_RC_SLOPE_SUPPORTS_SW_NE_2,
            SPR_AIR_POWERED_VERTICAL_RC_SLOPE_SUPPORTS_NW_SE_2,
            SPR_AIR_POWERED_VERTICAL_RC_SLOPE_SUPPORTS_NE_SW_2,
            SPR_AIR_POWERED_VERTICAL_RC_SLOPE_SUPPORTS_SE_NW_2,
        },
        {
            SPR_AIR_POWERED_VERTICAL_RC_SLOPE_SUPPORTS_SW_NE_3,
            SPR_AIR_POWERED_VERTICAL_RC_SLOPE_SUPPORTS_NW_SE_3,
            SPR_AIR_POWERED_VERTICAL_RC_SLOPE_SUPPORTS_NE_SW_3,
            SPR_AIR_POWERED_VERTICAL_RC_SLOPE_SUPPORTS_SE_NW_3,
        },
        {
            SPR_AIR_POWERED_VERTICAL_RC_SLOPE_SUPPORTS_SW_NE_4,
            SPR_AIR_POWERED_VERTICAL_RC_SLOPE_SUPPORTS_NW_SE_4,
            SPR_AIR_POWERED_VERTICAL_RC_SLOPE_SUPPORTS_NE_SW_4,
            SPR_AIR_POWERED_VERTICAL_RC_SLOPE_SUPPORTS_SE_NW_4,
        },
        {
            SPR_AIR_POWERED_VERTICAL_RC_SLOPE_SUPPORTS_SW_NE_5,
            SPR_AIR_POWERED_VERTICAL_RC_SLOPE_SUPPORTS_NW_SE_5,
            SPR_AIR_POWERED_VERTICAL_RC_SLOPE_SUPPORTS_NE_SW_5,
            SPR_AIR_POWERED_VERTICAL_RC_SLOPE_SUPPORTS_SE_NW_5,
        },
        {
            SPR_AIR_POWERED_VERTICAL_RC_SLOPE_SUPPORTS_SW_NE_6,
            SPR_AIR_POWERED_VERTICAL_RC_SLOPE_SUPPORTS_NW_SE_6,
            SPR_AIR_POWERED_VERTICAL_RC_SLOPE_SUPPORTS_NE_SW_6,
            SPR_AIR_POWERED_VERTICAL_RC_SLOPE_SUPPORTS_SE_NW_6,
        },
    };

    static const sint8 bbHeights03[] = { 1, 6, 14, 37, 76 };
    static const sint8 bbHeights12[] = { 1, 6, 14, 27, 59 };
    static const sint32 supportHeights[] = { 48, 64, 128, 176, 208, 240, 240 };

    uint32 supportsImageId = supportImageIds[trackSequence][direction] | air_powered_vertical_rc_get_support_colour();
    uint32 trackImageId = trackImageIds[trackSequence][direction] | gTrackColours[SCHEME_TRACK];
    sint8 bbHeight;
    bool isDirection03 = (direction == 0 || direction == 3);
    switch (trackSequence) {
    case 0:
        // HACK this might be a mistake in original code
        if (direction & 1) {
            bbHeight = bbHeights12[trackSequence];
            sub_98197C_rotated(direction, supportsImageId, 0, 0, 20, 32, bbHeight, height, 0, 6, height);
            sub_98199C_rotated(direction, trackImageId, 0, 0, 20, 32, bbHeight, height, 0, 6, height);

            wooden_a_supports_paint_setup(0, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);

            paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);

            paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
            paint_util_set_general_support_height(height + supportHeights[trackSequence], 0x20);
            break;
        }
    case 1:
    case 2:
    case 3:
        if (isDirection03) {
            bbHeight = bbHeights03[trackSequence];

            sub_98197C_rotated(direction, supportsImageId, 0, 0, 32, 20, bbHeight, height, 0, 6, height);
            sub_98199C_rotated(direction, trackImageId, 0, 0, 32, 20, bbHeight, height, 0, 6, height);
        } else {
            bbHeight = bbHeights12[trackSequence];
            sub_98197C_rotated(direction, supportsImageId, 0, 0, 32, 20, bbHeight, height, 0, 6, height);
            sub_98199C_rotated(direction, trackImageId, 0, 0, 32, 20, bbHeight, height, 0, 6, height);
        }

        wooden_a_supports_paint_setup(direction & 1, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);

        if (trackSequence == 0) {
            paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);
        }

        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + supportHeights[trackSequence], 0x20);
        break;
    case 4:
        if (isDirection03) {
            bbHeight = bbHeights03[trackSequence];
            sub_98197C_rotated(direction, supportsImageId, 0, 0, 32, 20, bbHeight, height, 0, 6, height);
            sub_98199C_rotated(direction, trackImageId, 0, 0, 32, 20, bbHeight, height, 0, 6, height);
        } else {
            bbHeight = bbHeights12[trackSequence];
            sub_98197C_rotated(direction, trackImageId, 0, 0, 32, 20, bbHeight, height, 0, 6, height);
            sub_98199C_rotated(direction, supportsImageId, 0, 0, 32, 20, bbHeight, height, 0, 6, height);
        }

        wooden_a_supports_paint_setup(direction & 1, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + supportHeights[trackSequence], 0x20);
        break;
    case 5:
        if (wooden_a_supports_paint_setup(direction & 1, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL)) {
            uint32 floorImageId;
            if (direction & 1) {
                floorImageId = SPR_FLOOR_PLANKS_90_DEG | gTrackColours[SCHEME_SUPPORTS];
            } else {
                floorImageId = SPR_FLOOR_PLANKS | gTrackColours[SCHEME_SUPPORTS];
            }
            sub_98197C(floorImageId, 0, 0, 26, 26, 126, height, 3, 3, height, get_current_rotation());
            sub_98199C_rotated(direction, supportsImageId, 0, 0, 26, 26, 126, height, 3, 3, height);
        } else {
            sub_98197C_rotated(direction, supportsImageId, 0, 0, 26, 26, 126, height, 3, 3, height);
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + supportHeights[trackSequence], 0x20);
        break;
    case 6:
        if (isDirection03) {
            sub_98197C_rotated(direction, supportsImageId, 0, 0, 5, 20, 79, height, 0, 6, height + 128);
            sub_98199C_rotated(direction, trackImageId, 0, 0, 5, 20, 79, height, 0, 6, height + 128);
        } else {
            sub_98197C_rotated(direction, trackImageId, 0, 0, 1, 20, 126, height, 27, 6, height);
            sub_98199C_rotated(direction, supportsImageId, 0, 0, 1, 20, 126, height, 27, 6, height);
        }
        wooden_a_supports_paint_setup(direction & 1, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);

        paint_util_set_vertical_tunnel(height + 240);

        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + supportHeights[trackSequence], 0x20);
        break;
    }
}

static void air_powered_vertical_rc_track_vertical_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    static const uint32 imageIds[4][2] = {
        { SPR_AIR_POWERED_VERTICAL_RC_VERTICAL_UP_SW_NE_SEQ_0, SPR_AIR_POWERED_VERTICAL_RC_VERTICAL_UP_SW_NE_SEQ_1 },
        { SPR_AIR_POWERED_VERTICAL_RC_VERTICAL_UP_NW_SE_SEQ_0, SPR_AIR_POWERED_VERTICAL_RC_VERTICAL_UP_NW_SE_SEQ_1 },
        { SPR_AIR_POWERED_VERTICAL_RC_VERTICAL_UP_NE_SW_SEQ_0, SPR_AIR_POWERED_VERTICAL_RC_VERTICAL_UP_NE_SW_SEQ_1 },
        { SPR_AIR_POWERED_VERTICAL_RC_VERTICAL_UP_SE_NW_SEQ_0, SPR_AIR_POWERED_VERTICAL_RC_VERTICAL_UP_SE_NW_SEQ_1 },
    };

    uint32 imageId;
    switch (trackSequence) {
    case 0:
        imageId = imageIds[direction][0] | air_powered_vertical_rc_get_support_colour();
        sub_98197C_rotated(direction, imageId, 0, 0, 26, 26, 79, height, 3, 3, height);
        break;
    case 1:
        imageId = imageIds[direction][1] | gTrackColours[SCHEME_TRACK];
        if (direction == 0 || direction == 3) {
            sub_98197C_rotated(direction, imageId, 0, 0, 2, 20, 79, height, 0, 6, height);
        } else {
            sub_98197C_rotated(direction, imageId, 0, 0, 2, 20, 79, height, 30, 6, height);
        }

        paint_util_set_vertical_tunnel(height + 80);
        break;
    }

    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 80, 0x20);
}

static void air_powered_vertical_rc_track_vertical_top(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    if (direction == 2 || direction == 3) {
        air_powered_vertical_rc_track_vertical_top(session, rideIndex, 3 - trackSequence, (direction + 2) & 3, height, mapElement);
        return;
    }

    static const uint32 imageIds[4][6] = {
        {
            SPR_AIR_POWERED_VERTICAL_RC_VERTICAL_TOP_SUPPORT_SW_NE,
            SPR_AIR_POWERED_VERTICAL_RC_VERTICAL_TOP_TRACK_SW_NE_SEQ_0,
            SPR_AIR_POWERED_VERTICAL_RC_VERTICAL_TOP_TRACK_SW_NE_SEQ_1,
            SPR_AIR_POWERED_VERTICAL_RC_VERTICAL_TOP_TRACK_SW_NE_SEQ_2,
            SPR_AIR_POWERED_VERTICAL_RC_VERTICAL_TOP_SUPPORT_NE_SW,
            SPR_AIR_POWERED_VERTICAL_RC_VERTICAL_TOP_TRACK_SW_NE_SEQ_3
        },
        {
            SPR_AIR_POWERED_VERTICAL_RC_VERTICAL_TOP_SUPPORT_NW_SE,
            SPR_AIR_POWERED_VERTICAL_RC_VERTICAL_TOP_TRACK_NW_SE_SEQ_0,
            SPR_AIR_POWERED_VERTICAL_RC_VERTICAL_TOP_TRACK_NW_SE_SEQ_1,
            SPR_AIR_POWERED_VERTICAL_RC_VERTICAL_TOP_TRACK_NW_SE_SEQ_2,
            SPR_AIR_POWERED_VERTICAL_RC_VERTICAL_TOP_SUPPORT_SE_NW,
            SPR_AIR_POWERED_VERTICAL_RC_VERTICAL_TOP_TRACK_NW_SE_SEQ_3
        },
    };

    uint32 imageIdS, imageIdT;
    switch (trackSequence) {
    case 0:
        imageIdS = imageIds[direction][0] | air_powered_vertical_rc_get_support_colour();
        imageIdT = imageIds[direction][1] | gTrackColours[SCHEME_TRACK];
        if (direction == 0) {
            sub_98197C_rotated(direction, imageIdS, 0, 0, 32, 20, 15, height, 0, 6, height);
            sub_98199C_rotated(direction, imageIdT, 0, 0, 31, 20, 15, height, 1, 6, height);
        } else {
            sub_98197C_rotated(direction, imageIdS, 0, 0, 5, 20, 1, height, 24, 6, height);
            sub_98199C_rotated(direction, imageIdT, 0, 0, 5, 20, 1, height, 24, 6, height);
        }
        break;
    case 1:
        imageIdT = imageIds[direction][2] | gTrackColours[SCHEME_TRACK];
        if (direction == 0) {
            sub_98197C_rotated(direction, imageIdT, 0, 0, 2, 20, 15, height, 0, 6, height);
        } else {
            sub_98197C_rotated(direction, imageIdT, 0, 0, 2, 20, 1, height, 33, 6, height);
        }
        paint_util_set_vertical_tunnel(height + 80);
        break;
    case 2:
        imageIdT = imageIds[direction][3] | gTrackColours[SCHEME_TRACK];
        if (direction == 0) {
            sub_98197C_rotated(direction, imageIdT, 0, 0, 2, 20, 1, height, 33, 6, height);
        } else {
            sub_98197C_rotated(direction, imageIdT, 0, 0, 2, 20, 15, height, 0, 6, height);
        }
        paint_util_set_vertical_tunnel(height + 80);
        break;
    case 3:
        imageIdS = imageIds[direction][4] | air_powered_vertical_rc_get_support_colour();
        imageIdT = imageIds[direction][5] | gTrackColours[SCHEME_TRACK];
        if (direction == 0) {
            sub_98197C_rotated(direction, imageIdS, 0, 0, 5, 20, 1, height, 24, 6, height);
            sub_98199C_rotated(direction, imageIdT, 0, 0, 5, 20, 1, height, 24, 6, height);
        } else {
            sub_98197C_rotated(direction, imageIdS, 0, 0, 32, 20, 15, height, 0, 6, height);
            sub_98199C_rotated(direction, imageIdT, 0, 0, 32, 20, 15, height, 0, 6, height);
        }
        break;
    }

    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 80, 0x20);
}

static void air_powered_vertical_rc_track_vertical_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    air_powered_vertical_rc_track_vertical_up(session, rideIndex, trackSequence ^ 1, (direction + 2) & 3, height, mapElement);
}

static void air_powered_vertical_rc_track_vertical_slope_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    air_powered_vertical_rc_track_vertical_slope_up(session, rideIndex, 6 - trackSequence, (direction + 2) & 3, height, mapElement);
}

TRACK_PAINT_FUNCTION get_track_paint_function_air_powered_vertical_rc(sint32 trackType, sint32 direction)
{
    switch (trackType) {
    case TRACK_ELEM_FLAT:
        return air_powered_vertical_rc_track_flat;
    case TRACK_ELEM_END_STATION:
    case TRACK_ELEM_BEGIN_STATION:
    case TRACK_ELEM_MIDDLE_STATION:
        return air_powered_vertical_rc_track_station;
    case TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES:
        return air_powered_vertical_rc_track_left_quarter_turn_5;
    case TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES:
        return air_powered_vertical_rc_track_right_quarter_turn_5;
    case TRACK_ELEM_FLAT_TO_LEFT_BANK:
        return air_powered_vertical_rc_track_flat_to_left_bank;
    case TRACK_ELEM_FLAT_TO_RIGHT_BANK:
        return air_powered_vertical_rc_track_flat_to_right_bank;
    case TRACK_ELEM_LEFT_BANK_TO_FLAT:
        return air_powered_vertical_rc_track_left_bank_to_flat;
    case TRACK_ELEM_RIGHT_BANK_TO_FLAT:
        return air_powered_vertical_rc_track_right_bank_to_flat;
    case TRACK_ELEM_BANKED_LEFT_QUARTER_TURN_5_TILES:
        return air_powered_vertical_rc_track_banked_left_quarter_turn_5;
    case TRACK_ELEM_BANKED_RIGHT_QUARTER_TURN_5_TILES:
        return air_powered_vertical_rc_track_banked_right_quarter_turn_5;
    case TRACK_ELEM_LEFT_BANK:
        return air_powered_vertical_rc_track_left_bank;
    case TRACK_ELEM_RIGHT_BANK:
        return air_powered_vertical_rc_track_right_bank;
    case TRACK_ELEM_BRAKES:
        return air_powered_vertical_rc_track_brakes;
    case TRACK_ELEM_REVERSE_FREEFALL_SLOPE:
        return air_powered_vertical_rc_track_vertical_slope_up;
    case TRACK_ELEM_REVERSE_FREEFALL_VERTICAL:
        return air_powered_vertical_rc_track_vertical_up;
    case TRACK_ELEM_AIR_THRUST_TOP_CAP:
        return air_powered_vertical_rc_track_vertical_top;
    case TRACK_ELEM_AIR_THRUST_VERTICAL_DOWN:
        return air_powered_vertical_rc_track_vertical_down;
    case TRACK_ELEM_AIR_THRUST_VERTICAL_DOWN_TO_LEVEL:
        return air_powered_vertical_rc_track_vertical_slope_down;
    }
    return NULL;
}
