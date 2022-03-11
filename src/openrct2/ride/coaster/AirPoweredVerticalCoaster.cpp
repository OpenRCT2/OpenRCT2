/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../drawing/Drawing.h"
#include "../../interface/Viewport.h"
#include "../../paint/Paint.h"
#include "../../paint/Supports.h"
#include "../../paint/tile_element/Paint.TileElement.h"
#include "../../sprites.h"
#include "../../world/Map.h"
#include "../RideData.h"
#include "../TrackData.h"
#include "../TrackPaint.h"

enum
{
    SPR_REVERSE_FREEFALL_RC_FLAT_SW_NE = 22164,
    SPR_REVERSE_FREEFALL_RC_FLAT_NW_SE = 22165,

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

static uint32_t air_powered_vertical_rc_get_support_colour(paint_session& session)
{
    uint32_t colourFlags = session.TrackColours[SCHEME_SUPPORTS];
    uint32_t trackColour = session.TrackColours[SCHEME_TRACK];
    if (trackColour & IMAGE_TYPE_REMAP_2_PLUS)
    {
        colourFlags |= (trackColour & 0x9F000000);
    }
    return colourFlags;
}

/** rct2: 0x008AFAD4 */
static void air_powered_vertical_rc_track_flat(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    static constexpr const uint32_t imageIds[4] = {
        SPR_AIR_POWERED_VERTICAL_RC_FLAT_SW_NE,
        SPR_AIR_POWERED_VERTICAL_RC_FLAT_NW_SE,
        SPR_AIR_POWERED_VERTICAL_RC_FLAT_SW_NE,
        SPR_AIR_POWERED_VERTICAL_RC_FLAT_NW_SE,
    };

    uint32_t imageId = imageIds[direction] | session.TrackColours[SCHEME_TRACK];
    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { 32, 20, 1 }, { 0, 6, height });

    wooden_a_supports_paint_setup(session, direction & 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);

    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);

    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

static void air_powered_vertical_rc_track_station(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    static constexpr const uint32_t imageIds[4][2] = {
        { SPR_AIR_POWERED_VERTICAL_RC_STATION_SW_NE, SPR_STATION_BASE_B_SW_NE },
        { SPR_AIR_POWERED_VERTICAL_RC_STATION_NW_SE, SPR_STATION_BASE_B_NW_SE },
        { SPR_AIR_POWERED_VERTICAL_RC_STATION_SW_NE, SPR_STATION_BASE_B_SW_NE },
        { SPR_AIR_POWERED_VERTICAL_RC_STATION_NW_SE, SPR_STATION_BASE_B_NW_SE },
    };

    PaintAddImageAsParentRotated(
        session, direction, imageIds[direction][1] | session.TrackColours[SCHEME_MISC], { 0, 0, height - 2 }, { 32, 28, 1 },
        { 0, 2, height });
    PaintAddImageAsChildRotated(
        session, direction, imageIds[direction][0] | session.TrackColours[SCHEME_TRACK], 0, 0, 32, 20, 1, height, 0, 6, height);

    wooden_a_supports_paint_setup(session, direction & 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);

    track_paint_util_draw_narrow_station_platform(session, ride, direction, height, 5, trackElement);

    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);

    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

static void air_powered_vertical_rc_track_right_quarter_turn_5(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    static constexpr const sprite_bb imageIds[4][5] = {
        {
            { SPR_AIR_POWERED_VERTICAL_RC_QUARTER_TURN_5_SW_SE_PART_0, { 0, 2, 0 }, { 0, 0, 0 }, { 32, 32, 2 } },
            { SPR_AIR_POWERED_VERTICAL_RC_QUARTER_TURN_5_SW_SE_PART_1, { 0, 16, 0 }, { 0, 0, 0 }, { 32, 16, 2 } },
            { SPR_AIR_POWERED_VERTICAL_RC_QUARTER_TURN_5_SW_SE_PART_2, { 0, 0, 0 }, { 0, 0, 0 }, { 16, 16, 2 } },
            { SPR_AIR_POWERED_VERTICAL_RC_QUARTER_TURN_5_SW_SE_PART_3, { 16, 0, 0 }, { 0, 0, 0 }, { 16, 32, 2 } },
            { SPR_AIR_POWERED_VERTICAL_RC_QUARTER_TURN_5_SW_SE_PART_4, { 2, 0, 0 }, { 0, 0, 0 }, { 32, 32, 2 } },
        },
        {
            { SPR_AIR_POWERED_VERTICAL_RC_QUARTER_TURN_5_NW_SW_PART_0, { 2, 0, 0 }, { 0, 0, 0 }, { 32, 32, 2 } },
            { SPR_AIR_POWERED_VERTICAL_RC_QUARTER_TURN_5_NW_SW_PART_1, { 16, 0, 0 }, { 0, 0, 0 }, { 16, 34, 2 } },
            { SPR_AIR_POWERED_VERTICAL_RC_QUARTER_TURN_5_NW_SW_PART_2, { 0, 16, 0 }, { 0, 0, 0 }, { 16, 16, 2 } },
            { SPR_AIR_POWERED_VERTICAL_RC_QUARTER_TURN_5_NW_SW_PART_3, { 0, 0, 0 }, { 0, 0, 0 }, { 32, 16, 2 } },
            { SPR_AIR_POWERED_VERTICAL_RC_QUARTER_TURN_5_NW_SW_PART_4, { 0, 2, 0 }, { 0, 0, 0 }, { 32, 27, 2 } },
        },
        {
            { SPR_AIR_POWERED_VERTICAL_RC_QUARTER_TURN_5_NE_NW_PART_0, { 0, 2, 0 }, { 0, 0, 0 }, { 32, 27, 2 } },
            { SPR_AIR_POWERED_VERTICAL_RC_QUARTER_TURN_5_NE_NW_PART_1, { 0, 0, 0 }, { 0, 0, 0 }, { 32, 16, 2 } },
            { SPR_AIR_POWERED_VERTICAL_RC_QUARTER_TURN_5_NE_NW_PART_2, { 16, 16, 0 }, { 0, 0, 0 }, { 16, 16, 2 } },
            { SPR_AIR_POWERED_VERTICAL_RC_QUARTER_TURN_5_NE_NW_PART_3, { 0, 0, 0 }, { 0, 0, 0 }, { 16, 32, 2 } },
            { SPR_AIR_POWERED_VERTICAL_RC_QUARTER_TURN_5_NE_NW_PART_4, { 2, 0, 0 }, { 0, 0, 0 }, { 27, 32, 2 } },
        },
        {
            { SPR_AIR_POWERED_VERTICAL_RC_QUARTER_TURN_5_SE_NE_PART_0, { 2, 0, 0 }, { 0, 0, 0 }, { 27, 32, 2 } },
            { SPR_AIR_POWERED_VERTICAL_RC_QUARTER_TURN_5_SE_NE_PART_1, { 0, 0, 0 }, { 0, 0, 0 }, { 16, 32, 2 } },
            { SPR_AIR_POWERED_VERTICAL_RC_QUARTER_TURN_5_SE_NE_PART_2, { 16, 0, 0 }, { 0, 0, 0 }, { 16, 16, 2 } },
            { SPR_AIR_POWERED_VERTICAL_RC_QUARTER_TURN_5_SE_NE_PART_3, { 0, 16, 0 }, { 0, 0, 0 }, { 32, 16, 2 } },
            { SPR_AIR_POWERED_VERTICAL_RC_QUARTER_TURN_5_SE_NE_PART_4, { 0, 2, 0 }, { 0, 0, 0 }, { 32, 32, 2 } },
        },
    };

    track_paint_util_right_quarter_turn_5_tiles_paint_3(
        session, height, direction, trackSequence, session.TrackColours[SCHEME_TRACK], imageIds);
    track_paint_util_right_quarter_turn_5_tiles_wooden_supports(session, height, direction, trackSequence);
    track_paint_util_right_quarter_turn_5_tiles_tunnel(session, height, direction, trackSequence, TUNNEL_SQUARE_FLAT);

    switch (trackSequence)
    {
        case 0:
            paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
            break;
        case 1:
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            break;
        case 2:
            paint_util_set_segment_support_height(
                session,
                paint_util_rotate_segments(
                    SEGMENT_D0 | SEGMENT_C4 | SEGMENT_D4 | SEGMENT_BC | SEGMENT_C0 | SEGMENT_CC, direction),
                0xFFFF, 0);
            break;
        case 3:
            paint_util_set_segment_support_height(
                session,
                paint_util_rotate_segments(
                    SEGMENT_B4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_C4 | SEGMENT_B8 | SEGMENT_BC | SEGMENT_D0 | SEGMENT_D4,
                    direction),
                0xFFFF, 0);
            break;
        case 4:
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            break;
        case 5:
            paint_util_set_segment_support_height(
                session,
                paint_util_rotate_segments(
                    SEGMENT_D4 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C8, direction),
                0xFFFF, 0);
            break;
        case 6:
            paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
            break;
    }

    paint_util_set_general_support_height(session, height + 32, 0x20);
}

static void air_powered_vertical_rc_track_left_quarter_turn_5(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    air_powered_vertical_rc_track_right_quarter_turn_5(session, ride, trackSequence, (direction + 1) % 4, height, trackElement);
}

/** rct2: 0x008AFB74 */
static void air_powered_vertical_rc_track_flat_to_left_bank(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    static constexpr const uint32_t imageIds[4][2] = {
        { SPR_AIR_POWERED_VERTICAL_RC_FLAT_TO_LEFT_BANK_SW_NE, SPR_AIR_POWERED_VERTICAL_RC_FLAT_TO_LEFT_BANK_FRONT_SW_NE },
        { SPR_AIR_POWERED_VERTICAL_RC_FLAT_TO_LEFT_BANK_NW_SE, SPR_AIR_POWERED_VERTICAL_RC_FLAT_TO_LEFT_BANK_FRONT_NW_SE },
        { SPR_AIR_POWERED_VERTICAL_RC_FLAT_TO_LEFT_BANK_NE_SW, SPR_AIR_POWERED_VERTICAL_RC_FLAT_TO_LEFT_BANK_FRONT_NE_SW },
        { SPR_AIR_POWERED_VERTICAL_RC_FLAT_TO_LEFT_BANK_SE_NW, SPR_AIR_POWERED_VERTICAL_RC_FLAT_TO_LEFT_BANK_FRONT_SE_NW },
    };

    uint32_t imageId = imageIds[direction][0] | session.TrackColours[SCHEME_TRACK];
    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });

    if (direction == 0 || direction == 1)
    {
        imageId = imageIds[direction][1] | session.TrackColours[SCHEME_TRACK];
        PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { 32, 1, 26 }, { 0, 27, height });
    }

    wooden_a_supports_paint_setup(session, direction & 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);

    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);

    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x008AFB84 */
static void air_powered_vertical_rc_track_flat_to_right_bank(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    static constexpr const uint32_t imageIds[4][2] = {
        { SPR_AIR_POWERED_VERTICAL_RC_FLAT_TO_RIGHT_BANK_SW_NE, SPR_AIR_POWERED_VERTICAL_RC_FLAT_TO_LEFT_BANK_FRONT_SW_NE },
        { SPR_AIR_POWERED_VERTICAL_RC_FLAT_TO_RIGHT_BANK_NW_SE, SPR_AIR_POWERED_VERTICAL_RC_FLAT_TO_LEFT_BANK_FRONT_NW_SE },
        { SPR_AIR_POWERED_VERTICAL_RC_FLAT_TO_RIGHT_BANK_NE_SW, SPR_AIR_POWERED_VERTICAL_RC_FLAT_TO_LEFT_BANK_FRONT_NE_SW },
        { SPR_AIR_POWERED_VERTICAL_RC_FLAT_TO_RIGHT_BANK_SE_NW, SPR_AIR_POWERED_VERTICAL_RC_FLAT_TO_LEFT_BANK_FRONT_SE_NW },
    };

    uint32_t imageId = imageIds[direction][0] | session.TrackColours[SCHEME_TRACK];
    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });

    if (direction == 2 || direction == 3)
    {
        imageId = imageIds[direction][1] | session.TrackColours[SCHEME_TRACK];
        PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { 32, 1, 26 }, { 0, 27, height });
    }

    wooden_a_supports_paint_setup(session, direction & 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);

    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);

    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

static void air_powered_vertical_rc_track_left_bank_to_flat(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    air_powered_vertical_rc_track_flat_to_right_bank(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008AFBA4 */
static void air_powered_vertical_rc_track_right_bank_to_flat(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    air_powered_vertical_rc_track_flat_to_left_bank(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

static void air_powered_vertical_rc_track_banked_right_quarter_turn_5(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    static constexpr const sprite_bb imageIds[4][5] = {
        {
            { SPR_AIR_POWERED_VERTICAL_RC_BANKED_QUARTER_TURN_5_SW_SE_PART_0, { 0, 0, 0 }, { 0, 6, 0 }, { 32, 20, 3 } },
            { SPR_AIR_POWERED_VERTICAL_RC_BANKED_QUARTER_TURN_5_SW_SE_PART_1, { 0, 0, 0 }, { 0, 16, 0 }, { 32, 16, 3 } },
            { SPR_AIR_POWERED_VERTICAL_RC_BANKED_QUARTER_TURN_5_SW_SE_PART_2, { 0, 0, 0 }, { 0, 0, 0 }, { 16, 16, 3 } },
            { SPR_AIR_POWERED_VERTICAL_RC_BANKED_QUARTER_TURN_5_SW_SE_PART_3, { 0, 0, 0 }, { 16, 0, 0 }, { 16, 32, 3 } },
            { SPR_AIR_POWERED_VERTICAL_RC_BANKED_QUARTER_TURN_5_SW_SE_PART_4, { 0, 0, 0 }, { 6, 0, 0 }, { 20, 32, 3 } },
        },
        {
            { SPR_AIR_POWERED_VERTICAL_RC_BANKED_QUARTER_TURN_5_NW_SW_PART_0, { 0, 0, 0 }, { 6, 0, 0 }, { 20, 32, 3 } },
            { SPR_AIR_POWERED_VERTICAL_RC_BANKED_QUARTER_TURN_5_NW_SW_PART_1, { 0, 0, 0 }, { 16, 0, 0 }, { 16, 32, 3 } },
            { SPR_AIR_POWERED_VERTICAL_RC_BANKED_QUARTER_TURN_5_NW_SW_PART_2, { 0, 0, 0 }, { 0, 16, 0 }, { 16, 16, 3 } },
            { SPR_AIR_POWERED_VERTICAL_RC_BANKED_QUARTER_TURN_5_NW_SW_PART_3, { 0, 0, 0 }, { 0, 0, 0 }, { 32, 16, 3 } },
            { SPR_AIR_POWERED_VERTICAL_RC_BANKED_QUARTER_TURN_5_NW_SW_PART_4, { 0, 0, 0 }, { 0, 6, 0 }, { 32, 20, 3 } },
        },
        {
            { SPR_AIR_POWERED_VERTICAL_RC_BANKED_QUARTER_TURN_5_NE_NW_PART_0, { 0, 0, 0 }, { 0, 27, 0 }, { 32, 1, 26 } },
            { SPR_AIR_POWERED_VERTICAL_RC_BANKED_QUARTER_TURN_5_NE_NW_PART_1, { 0, 0, 0 }, { 0, 0, 27 }, { 32, 16, 1 } },
            { SPR_AIR_POWERED_VERTICAL_RC_BANKED_QUARTER_TURN_5_NE_NW_PART_2, { 0, 0, 0 }, { 16, 16, 27 }, { 16, 16, 1 } },
            { SPR_AIR_POWERED_VERTICAL_RC_BANKED_QUARTER_TURN_5_NE_NW_PART_3, { 0, 0, 0 }, { 0, 0, 27 }, { 16, 32, 1 } },
            { SPR_AIR_POWERED_VERTICAL_RC_BANKED_QUARTER_TURN_5_NE_NW_PART_4, { 0, 0, 0 }, { 27, 0, 0 }, { 1, 32, 26 } },
        },
        {
            { SPR_AIR_POWERED_VERTICAL_RC_BANKED_QUARTER_TURN_5_SE_NE_PART_0, { 0, 0, 0 }, { 6, 0, 0 }, { 20, 32, 3 } },
            { SPR_AIR_POWERED_VERTICAL_RC_BANKED_QUARTER_TURN_5_SE_NE_PART_1, { 0, 0, 0 }, { 0, 0, 0 }, { 16, 32, 3 } },
            { SPR_AIR_POWERED_VERTICAL_RC_BANKED_QUARTER_TURN_5_SE_NE_PART_2, { 0, 0, 0 }, { 16, 0, 0 }, { 16, 16, 3 } },
            { SPR_AIR_POWERED_VERTICAL_RC_BANKED_QUARTER_TURN_5_SE_NE_PART_3, { 0, 0, 0 }, { 0, 16, 0 }, { 32, 16, 3 } },
            { SPR_AIR_POWERED_VERTICAL_RC_BANKED_QUARTER_TURN_5_SE_NE_PART_4, { 0, 0, 0 }, { 0, 6, 0 }, { 32, 20, 3 } },
        },
    };

    track_paint_util_right_quarter_turn_5_tiles_paint_2(
        session, height, direction, trackSequence, session.TrackColours[SCHEME_TRACK], imageIds);

    if (direction == 1 && trackSequence == 6)
    {
        uint32_t imageId = SPR_AIR_POWERED_VERTICAL_RC_BANKED_QUARTER_TURN_5_FRONT_NW_SW_PART_4
            | session.TrackColours[SCHEME_TRACK];
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 32, 1, 26 }, { 0, 27, height });
    }
    else if (direction == 3 && trackSequence == 0)
    {
        uint32_t imageId = SPR_AIR_POWERED_VERTICAL_RC_BANKED_QUARTER_TURN_5_FRONT_SE_NE_PART_0
            | session.TrackColours[SCHEME_TRACK];
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 1, 32, 26 }, { 27, 0, height });
    }

    track_paint_util_right_quarter_turn_5_tiles_wooden_supports(session, height, direction, trackSequence);
    track_paint_util_right_quarter_turn_5_tiles_tunnel(session, height, direction, trackSequence, TUNNEL_SQUARE_FLAT);

    switch (trackSequence)
    {
        case 0:
            paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
            break;
        case 1:
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            break;
        case 2:
            paint_util_set_segment_support_height(
                session,
                paint_util_rotate_segments(
                    SEGMENT_D0 | SEGMENT_C4 | SEGMENT_D4 | SEGMENT_BC | SEGMENT_C0 | SEGMENT_CC, direction),
                0xFFFF, 0);
            break;
        case 3:
            paint_util_set_segment_support_height(
                session,
                paint_util_rotate_segments(
                    SEGMENT_B4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_C4 | SEGMENT_B8 | SEGMENT_BC | SEGMENT_D0 | SEGMENT_D4,
                    direction),
                0xFFFF, 0);
            break;
        case 4:
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            break;
        case 5:
            paint_util_set_segment_support_height(
                session,
                paint_util_rotate_segments(
                    SEGMENT_D4 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C8, direction),
                0xFFFF, 0);
            break;
        case 6:
            paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
            break;
    }

    paint_util_set_general_support_height(session, height + 32, 0x20);
}

static void air_powered_vertical_rc_track_banked_left_quarter_turn_5(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    air_powered_vertical_rc_track_banked_right_quarter_turn_5(
        session, ride, trackSequence, (direction + 1) % 4, height, trackElement);
}

/** rct2: 0x008AFBD4 */
static void air_powered_vertical_rc_track_left_bank(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    static constexpr const uint32_t imageIds[4] = {
        SPR_AIR_POWERED_VERTICAL_RC_LEFT_BANK_SW_NE,
        SPR_AIR_POWERED_VERTICAL_RC_LEFT_BANK_NW_SE,
        SPR_AIR_POWERED_VERTICAL_RC_LEFT_BANK_NE_SW,
        SPR_AIR_POWERED_VERTICAL_RC_LEFT_BANK_SE_NW,
    };

    uint32_t imageId = imageIds[direction] | session.TrackColours[SCHEME_TRACK];
    if (direction == 0 || direction == 1)
    {
        PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { 32, 1, 26 }, { 0, 27, height });
    }
    else
    {
        PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
    }

    wooden_a_supports_paint_setup(session, direction & 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);

    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);

    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

static void air_powered_vertical_rc_track_right_bank(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    air_powered_vertical_rc_track_left_bank(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

static void air_powered_vertical_rc_track_brakes(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    static constexpr const uint32_t imageIds[4] = {
        SPR_AIR_POWERED_VERTICAL_RC_BRAKES_NW_SE,
        SPR_AIR_POWERED_VERTICAL_RC_BRAKES_SW_NE,
        SPR_AIR_POWERED_VERTICAL_RC_BRAKES_NW_SE,
        SPR_AIR_POWERED_VERTICAL_RC_BRAKES_SW_NE,
    };

    uint32_t imageId = imageIds[direction] | session.TrackColours[SCHEME_TRACK];
    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { 32, 20, 1 }, { 0, 6, height });

    wooden_a_supports_paint_setup(session, direction & 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);

    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);

    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

static void air_powered_vertical_rc_track_vertical_slope_up(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    static constexpr const uint32_t trackImageIds[7][4] = {
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
        { 0, 0, 0, 0 },
        {
            SPR_AIR_POWERED_VERTICAL_RC_SLOPE_SW_NE_6,
            SPR_AIR_POWERED_VERTICAL_RC_SLOPE_NW_SE_6,
            SPR_AIR_POWERED_VERTICAL_RC_SLOPE_NE_SW_6,
            SPR_AIR_POWERED_VERTICAL_RC_SLOPE_SE_NW_6,
        },
    };

    static constexpr const uint32_t supportImageIds[7][4] = {
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

    static constexpr const int8_t bbHeights03[] = {
        1, 6, 14, 37, 76,
    };
    static constexpr const int8_t bbHeights12[] = {
        1, 6, 14, 27, 59,
    };
    static constexpr const int32_t supportHeights[] = {
        48, 64, 128, 176, 208, 240, 240,
    };

    uint32_t supportsImageId = supportImageIds[trackSequence][direction] | air_powered_vertical_rc_get_support_colour(session);
    uint32_t trackImageId = trackImageIds[trackSequence][direction] | session.TrackColours[SCHEME_TRACK];
    int8_t bbHeight;
    bool isDirection03 = (direction == 0 || direction == 3);
    switch (trackSequence)
    {
        case 0:
            bbHeight = bbHeights12[trackSequence];
            PaintAddImageAsParentRotated(
                session, direction, supportsImageId, { 0, 0, height }, { 20, 32, bbHeight }, { 0, 6, height });
            PaintAddImageAsChildRotated(session, direction, trackImageId, 0, 0, 20, 32, bbHeight, height, 0, 6, height);

            wooden_a_supports_paint_setup(session, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);

            paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);

            paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + supportHeights[trackSequence], 0x20);
            break;
        case 1:
        case 2:
        case 3:
            if (isDirection03)
            {
                bbHeight = bbHeights03[trackSequence];

                PaintAddImageAsParentRotated(
                    session, direction, supportsImageId, { 0, 0, height }, { 32, 20, bbHeight }, { 0, 6, height });
                PaintAddImageAsChildRotated(session, direction, trackImageId, 0, 0, 32, 20, bbHeight, height, 0, 6, height);
            }
            else
            {
                bbHeight = bbHeights12[trackSequence];
                PaintAddImageAsParentRotated(
                    session, direction, supportsImageId, { 0, 0, height }, { 32, 20, bbHeight }, { 0, 6, height });
                PaintAddImageAsChildRotated(session, direction, trackImageId, 0, 0, 32, 20, bbHeight, height, 0, 6, height);
            }

            wooden_a_supports_paint_setup(session, direction & 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);

            if (trackSequence == 0)
            {
                paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
            }

            paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + supportHeights[trackSequence], 0x20);
            break;
        case 4:
            if (isDirection03)
            {
                bbHeight = bbHeights03[trackSequence];
                PaintAddImageAsParentRotated(
                    session, direction, supportsImageId, { 0, 0, height }, { 32, 20, bbHeight }, { 0, 6, height });
                PaintAddImageAsChildRotated(session, direction, trackImageId, 0, 0, 32, 20, bbHeight, height, 0, 6, height);
            }
            else
            {
                bbHeight = bbHeights12[trackSequence];
                PaintAddImageAsParentRotated(
                    session, direction, trackImageId, { 0, 0, height }, { 32, 20, bbHeight }, { 0, 6, height });
                PaintAddImageAsChildRotated(session, direction, supportsImageId, 0, 0, 32, 20, bbHeight, height, 0, 6, height);
            }

            wooden_a_supports_paint_setup(session, direction & 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + supportHeights[trackSequence], 0x20);
            break;
        case 5:
            if (wooden_a_supports_paint_setup(session, direction & 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]))
            {
                uint32_t floorImageId;
                if (direction & 1)
                {
                    floorImageId = SPR_FLOOR_PLANKS_90_DEG | session.TrackColours[SCHEME_SUPPORTS];
                }
                else
                {
                    floorImageId = SPR_FLOOR_PLANKS | session.TrackColours[SCHEME_SUPPORTS];
                }
                PaintAddImageAsParent(session, floorImageId, { 0, 0, height }, { 26, 26, 126 }, { 3, 3, height });
                PaintAddImageAsChildRotated(session, direction, supportsImageId, 0, 0, 26, 26, 126, height, 3, 3, height);
            }
            else
            {
                PaintAddImageAsParentRotated(
                    session, direction, supportsImageId, { 0, 0, height }, { 26, 26, 126 }, { 3, 3, height });
            }
            paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + supportHeights[trackSequence], 0x20);
            break;
        case 6:
            if (isDirection03)
            {
                PaintAddImageAsParentRotated(
                    session, direction, supportsImageId, { 0, 0, height }, { 5, 20, 79 }, { 0, 6, height + 128 });
                PaintAddImageAsChildRotated(session, direction, trackImageId, 0, 0, 5, 20, 79, height, 0, 6, height + 128);
            }
            else
            {
                PaintAddImageAsParentRotated(
                    session, direction, trackImageId, { 0, 0, height }, { 1, 20, 126 }, { 27, 6, height });
                PaintAddImageAsChildRotated(session, direction, supportsImageId, 0, 0, 1, 20, 126, height, 27, 6, height);
            }
            wooden_a_supports_paint_setup(session, direction & 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);

            paint_util_set_vertical_tunnel(session, height + 240);

            paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + supportHeights[trackSequence], 0x20);
            break;
    }
}

static void air_powered_vertical_rc_track_vertical_up(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    static constexpr const uint32_t imageIds[4][2] = {
        { SPR_AIR_POWERED_VERTICAL_RC_VERTICAL_UP_SW_NE_SEQ_0, SPR_AIR_POWERED_VERTICAL_RC_VERTICAL_UP_SW_NE_SEQ_1 },
        { SPR_AIR_POWERED_VERTICAL_RC_VERTICAL_UP_NW_SE_SEQ_0, SPR_AIR_POWERED_VERTICAL_RC_VERTICAL_UP_NW_SE_SEQ_1 },
        { SPR_AIR_POWERED_VERTICAL_RC_VERTICAL_UP_NE_SW_SEQ_0, SPR_AIR_POWERED_VERTICAL_RC_VERTICAL_UP_NE_SW_SEQ_1 },
        { SPR_AIR_POWERED_VERTICAL_RC_VERTICAL_UP_SE_NW_SEQ_0, SPR_AIR_POWERED_VERTICAL_RC_VERTICAL_UP_SE_NW_SEQ_1 },
    };

    uint32_t imageId;
    switch (trackSequence)
    {
        case 0:
            imageId = imageIds[direction][0] | air_powered_vertical_rc_get_support_colour(session);
            PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { 26, 26, 79 }, { 3, 3, height });
            break;
        case 1:
            imageId = imageIds[direction][1] | session.TrackColours[SCHEME_TRACK];
            if (direction == 0 || direction == 3)
            {
                PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { 2, 20, 79 }, { 0, 6, height });
            }
            else
            {
                PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { 2, 20, 79 }, { 30, 6, height });
            }

            paint_util_set_vertical_tunnel(session, height + 80);
            break;
    }

    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 80, 0x20);
}

static void air_powered_vertical_rc_track_vertical_top(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (direction == 2 || direction == 3)
    {
        air_powered_vertical_rc_track_vertical_top(session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement);
        return;
    }

    static constexpr const uint32_t imageIds[4][6] = {
        {
            SPR_AIR_POWERED_VERTICAL_RC_VERTICAL_TOP_SUPPORT_SW_NE,
            SPR_AIR_POWERED_VERTICAL_RC_VERTICAL_TOP_TRACK_SW_NE_SEQ_0,
            SPR_AIR_POWERED_VERTICAL_RC_VERTICAL_TOP_TRACK_SW_NE_SEQ_1,
            SPR_AIR_POWERED_VERTICAL_RC_VERTICAL_TOP_TRACK_SW_NE_SEQ_2,
            SPR_AIR_POWERED_VERTICAL_RC_VERTICAL_TOP_SUPPORT_NE_SW,
            SPR_AIR_POWERED_VERTICAL_RC_VERTICAL_TOP_TRACK_SW_NE_SEQ_3,
        },
        {
            SPR_AIR_POWERED_VERTICAL_RC_VERTICAL_TOP_SUPPORT_NW_SE,
            SPR_AIR_POWERED_VERTICAL_RC_VERTICAL_TOP_TRACK_NW_SE_SEQ_0,
            SPR_AIR_POWERED_VERTICAL_RC_VERTICAL_TOP_TRACK_NW_SE_SEQ_1,
            SPR_AIR_POWERED_VERTICAL_RC_VERTICAL_TOP_TRACK_NW_SE_SEQ_2,
            SPR_AIR_POWERED_VERTICAL_RC_VERTICAL_TOP_SUPPORT_SE_NW,
            SPR_AIR_POWERED_VERTICAL_RC_VERTICAL_TOP_TRACK_NW_SE_SEQ_3,
        },
    };

    uint32_t imageIdS, imageIdT;
    switch (trackSequence)
    {
        case 0:
            imageIdS = imageIds[direction][0] | air_powered_vertical_rc_get_support_colour(session);
            imageIdT = imageIds[direction][1] | session.TrackColours[SCHEME_TRACK];
            if (direction == 0)
            {
                PaintAddImageAsParentRotated(session, direction, imageIdS, { 0, 0, height }, { 32, 20, 15 }, { 0, 6, height });
                PaintAddImageAsChildRotated(session, direction, imageIdT, 0, 0, 31, 20, 15, height, 1, 6, height);
            }
            else
            {
                PaintAddImageAsParentRotated(session, direction, imageIdS, { 0, 0, height }, { 5, 20, 1 }, { 24, 6, height });
                PaintAddImageAsChildRotated(session, direction, imageIdT, 0, 0, 5, 20, 1, height, 24, 6, height);
            }
            break;
        case 1:
            imageIdT = imageIds[direction][2] | session.TrackColours[SCHEME_TRACK];
            if (direction == 0)
            {
                PaintAddImageAsParentRotated(session, direction, imageIdT, { 0, 0, height }, { 2, 20, 15 }, { 0, 6, height });
            }
            else
            {
                PaintAddImageAsParentRotated(session, direction, imageIdT, { 0, 0, height }, { 2, 20, 1 }, { 33, 6, height });
            }
            paint_util_set_vertical_tunnel(session, height + 80);
            break;
        case 2:
            imageIdT = imageIds[direction][3] | session.TrackColours[SCHEME_TRACK];
            if (direction == 0)
            {
                PaintAddImageAsParentRotated(session, direction, imageIdT, { 0, 0, height }, { 2, 20, 1 }, { 33, 6, height });
            }
            else
            {
                PaintAddImageAsParentRotated(session, direction, imageIdT, { 0, 0, height }, { 2, 20, 15 }, { 0, 6, height });
            }
            paint_util_set_vertical_tunnel(session, height + 80);
            break;
        case 3:
            imageIdS = imageIds[direction][4] | air_powered_vertical_rc_get_support_colour(session);
            imageIdT = imageIds[direction][5] | session.TrackColours[SCHEME_TRACK];
            if (direction == 0)
            {
                PaintAddImageAsParentRotated(session, direction, imageIdS, { 0, 0, height }, { 5, 20, 1 }, { 24, 6, height });
                PaintAddImageAsChildRotated(session, direction, imageIdT, 0, 0, 5, 20, 1, height, 24, 6, height);
            }
            else
            {
                PaintAddImageAsParentRotated(session, direction, imageIdS, { 0, 0, height }, { 32, 20, 15 }, { 0, 6, height });
                PaintAddImageAsChildRotated(session, direction, imageIdT, 0, 0, 32, 20, 15, height, 0, 6, height);
            }
            break;
    }

    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 80, 0x20);
}

static void air_powered_vertical_rc_track_vertical_down(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    air_powered_vertical_rc_track_vertical_up(session, ride, trackSequence ^ 1, (direction + 2) & 3, height, trackElement);
}

static void air_powered_vertical_rc_track_vertical_slope_down(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    air_powered_vertical_rc_track_vertical_slope_up(
        session, ride, 6 - trackSequence, (direction + 2) & 3, height, trackElement);
}

static void air_powered_vertical_rc_track_booster(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    // The booster piece is borrowed from the Reverse Freefall Colour.
    // It has two track colours, instead of the one that the APVC has.
    uint32_t colour = session.TrackColours[SCHEME_TRACK];
    if (!trackElement.IsGhost() && !trackElement.IsHighlighted())
    {
        // Replace remap colour 2 (bits 24-28) with a copy of remap colour 1 (bits 19-23).
        colour_t colour1 = (colour >> 19) & 31;
        colour &= ~0x1F000000;
        colour |= (colour1 << 24);
    }

    if (direction & 1)
    {
        uint32_t imageId = SPR_REVERSE_FREEFALL_RC_FLAT_NW_SE | colour;
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 20, 32, 1 }, { 6, 0, height });
        paint_util_push_tunnel_right(session, height, TUNNEL_SQUARE_FLAT);
    }
    else
    {
        uint32_t imageId = SPR_REVERSE_FREEFALL_RC_FLAT_SW_NE | colour;
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 32, 20, 1 }, { 0, 6, height });
        paint_util_push_tunnel_left(session, height, TUNNEL_SQUARE_FLAT);
    }

    wooden_a_supports_paint_setup(session, (direction & 1) ? 1 : 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

static void air_powered_vertical_rc_track_onride_photo(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    static constexpr const uint32_t imageIds[4] = {
        SPR_AIR_POWERED_VERTICAL_RC_FLAT_SW_NE,
        SPR_AIR_POWERED_VERTICAL_RC_FLAT_NW_SE,
        SPR_AIR_POWERED_VERTICAL_RC_FLAT_SW_NE,
        SPR_AIR_POWERED_VERTICAL_RC_FLAT_NW_SE,
    };

    uint32_t imageId = imageIds[direction] | session.TrackColours[SCHEME_TRACK];
    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { 32, 20, 1 }, { 0, 6, height });

    wooden_a_supports_paint_setup(session, direction & 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);

    track_paint_util_onride_photo_paint(session, direction, height + 3, trackElement);
    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);

    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

TRACK_PAINT_FUNCTION get_track_paint_function_air_powered_vertical_rc(int32_t trackType)
{
    switch (trackType)
    {
        case TrackElemType::Flat:
            return air_powered_vertical_rc_track_flat;
        case TrackElemType::EndStation:
        case TrackElemType::BeginStation:
        case TrackElemType::MiddleStation:
            return air_powered_vertical_rc_track_station;
        case TrackElemType::LeftQuarterTurn5Tiles:
            return air_powered_vertical_rc_track_left_quarter_turn_5;
        case TrackElemType::RightQuarterTurn5Tiles:
            return air_powered_vertical_rc_track_right_quarter_turn_5;
        case TrackElemType::FlatToLeftBank:
            return air_powered_vertical_rc_track_flat_to_left_bank;
        case TrackElemType::FlatToRightBank:
            return air_powered_vertical_rc_track_flat_to_right_bank;
        case TrackElemType::LeftBankToFlat:
            return air_powered_vertical_rc_track_left_bank_to_flat;
        case TrackElemType::RightBankToFlat:
            return air_powered_vertical_rc_track_right_bank_to_flat;
        case TrackElemType::BankedLeftQuarterTurn5Tiles:
            return air_powered_vertical_rc_track_banked_left_quarter_turn_5;
        case TrackElemType::BankedRightQuarterTurn5Tiles:
            return air_powered_vertical_rc_track_banked_right_quarter_turn_5;
        case TrackElemType::LeftBank:
            return air_powered_vertical_rc_track_left_bank;
        case TrackElemType::RightBank:
            return air_powered_vertical_rc_track_right_bank;
        case TrackElemType::Brakes:
            return air_powered_vertical_rc_track_brakes;
        case TrackElemType::ReverseFreefallSlope:
            return air_powered_vertical_rc_track_vertical_slope_up;
        case TrackElemType::ReverseFreefallVertical:
            return air_powered_vertical_rc_track_vertical_up;
        case TrackElemType::AirThrustTopCap:
            return air_powered_vertical_rc_track_vertical_top;
        case TrackElemType::AirThrustVerticalDown:
            return air_powered_vertical_rc_track_vertical_down;
        case TrackElemType::AirThrustVerticalDownToLevel:
            return air_powered_vertical_rc_track_vertical_slope_down;
        case TrackElemType::Booster:
            return air_powered_vertical_rc_track_booster;
        case TrackElemType::OnRidePhoto:
            return air_powered_vertical_rc_track_onride_photo;
    }
    return nullptr;
}
