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
    SPR_WILD_MOUSE_FLAT_SW_NE = 16900,
    SPR_WILD_MOUSE_FLAT_NW_SE = 16901,
    SPR_WILD_MOUSE_BRAKES_SW_NE = 16902,
    SPR_WILD_MOUSE_BRAKES_NW_SE = 16903,
    SPR_WILD_MOUSE_BLOCK_BRAKES_SW_NE_OPEN = 16904,
    SPR_WILD_MOUSE_BLOCK_BRAKES_NW_SE_OPEN = 16905,
    SPR_WILD_MOUSE_BLOCK_BRAKES_SW_NE_CLOSED = 16906,
    SPR_WILD_MOUSE_BLOCK_BRAKES_NW_SE_CLOSED = 16907,

    SPR_WILD_MOUSE_ROTATION_CONTROL_TOGGLE_SW_NE = 16908,
    SPR_WILD_MOUSE_ROTATION_CONTROL_TOGGLE_NW_SE = 16909,
    SPR_WILD_MOUSE_FLAT_CHAIN_SW_NE = 16910,
    SPR_WILD_MOUSE_FLAT_CHAIN_NW_SE = 16911,
    SPR_WILD_MOUSE_FLAT_CHAIN_NE_SW = 16912,
    SPR_WILD_MOUSE_FLAT_CHAIN_SE_NW = 16913,
    SPR_WILD_MOUSE_FLAT_TO_25_DEG_SW_NE = 16914,
    SPR_WILD_MOUSE_FLAT_TO_25_DEG_NW_SE = 16915,
    SPR_WILD_MOUSE_FLAT_TO_25_DEG_NE_SW = 16916,
    SPR_WILD_MOUSE_FLAT_TO_25_DEG_SE_NW = 16917,
    SPR_WILD_MOUSE_25_DEG_TO_FLAT_SW_NE = 16918,
    SPR_WILD_MOUSE_25_DEG_TO_FLAT_NW_SE = 16919,
    SPR_WILD_MOUSE_25_DEG_TO_FLAT_NE_SW = 16920,
    SPR_WILD_MOUSE_25_DEG_TO_FLAT_SE_NW = 16921,
    SPR_WILD_MOUSE_25_DEG_SW_NE = 16922,
    SPR_WILD_MOUSE_25_DEG_NW_SE = 16923,
    SPR_WILD_MOUSE_25_DEG_NE_SW = 16924,
    SPR_WILD_MOUSE_25_DEG_SE_NW = 16925,
    SPR_WILD_MOUSE_25_DEG_TO_60_DEG_SW_NE = 16926,
    SPR_WILD_MOUSE_25_DEG_TO_60_DEG_NW_SE = 16927,
    SPR_WILD_MOUSE_25_DEG_TO_60_DEG_NE_SW = 16928,
    SPR_WILD_MOUSE_25_DEG_TO_60_DEG_SE_NW = 16929,
    SPR_WILD_MOUSE_25_DEG_TO_60_DEG_FRONT_NW_SE = 16930,
    SPR_WILD_MOUSE_25_DEG_TO_60_DEG_FRONT_NE_SW = 16931,
    SPR_WILD_MOUSE_60_DEG_TO_25_DEG_SW_NE = 16932,
    SPR_WILD_MOUSE_60_DEG_TO_25_DEG_NW_SE = 16933,
    SPR_WILD_MOUSE_60_DEG_TO_25_DEG_NE_SW = 16934,
    SPR_WILD_MOUSE_60_DEG_TO_25_DEG_SE_NW = 16935,
    SPR_WILD_MOUSE_60_DEG_TO_25_DEG_FRONT_NW_SE = 16936,
    SPR_WILD_MOUSE_60_DEG_TO_25_DEG_FRONT_NE_SW = 16937,
    SPR_WILD_MOUSE_60_DEG_SW_NE = 16938,
    SPR_WILD_MOUSE_60_DEG_NW_SE = 16939,
    SPR_WILD_MOUSE_60_DEG_NE_SW = 16940,
    SPR_WILD_MOUSE_60_DEG_SE_NW = 16941,
    SPR_WILD_MOUSE_FLAT_TO_25_DEG_CHAIN_SW_NE = 16942,
    SPR_WILD_MOUSE_FLAT_TO_25_DEG_CHAIN_NW_SE = 16943,
    SPR_WILD_MOUSE_FLAT_TO_25_DEG_CHAIN_NE_SW = 16944,
    SPR_WILD_MOUSE_FLAT_TO_25_DEG_CHAIN_SE_NW = 16945,
    SPR_WILD_MOUSE_25_DEG_TO_FLAT_CHAIN_SW_NE = 16946,
    SPR_WILD_MOUSE_25_DEG_TO_FLAT_CHAIN_NW_SE = 16947,
    SPR_WILD_MOUSE_25_DEG_TO_FLAT_CHAIN_NE_SW = 16948,
    SPR_WILD_MOUSE_25_DEG_TO_FLAT_CHAIN_SE_NW = 16949,
    SPR_WILD_MOUSE_25_DEG_CHAIN_SW_NE = 16950,
    SPR_WILD_MOUSE_25_DEG_CHAIN_NW_SE = 16951,
    SPR_WILD_MOUSE_25_DEG_CHAIN_NE_SW = 16952,
    SPR_WILD_MOUSE_25_DEG_CHAIN_SE_NW = 16953,
    SPR_WILD_MOUSE_25_DEG_TO_60_DEG_CHAIN_SW_NE = 16954,
    SPR_WILD_MOUSE_25_DEG_TO_60_DEG_CHAIN_NW_SE = 16955,
    SPR_WILD_MOUSE_25_DEG_TO_60_DEG_CHAIN_NE_SW = 16956,
    SPR_WILD_MOUSE_25_DEG_TO_60_DEG_CHAIN_SE_NW = 16957,
    SPR_WILD_MOUSE_25_DEG_TO_60_DEG_CHAIN_FRONT_NW_SE = 16958,
    SPR_WILD_MOUSE_25_DEG_TO_60_DEG_CHAIN_FRONT_NE_SW = 16959,
    SPR_WILD_MOUSE_60_DEG_TO_25_DEG_CHAIN_SW_NE = 16960,
    SPR_WILD_MOUSE_60_DEG_TO_25_DEG_CHAIN_NW_SE = 16961,
    SPR_WILD_MOUSE_60_DEG_TO_25_DEG_CHAIN_NE_SW = 16962,
    SPR_WILD_MOUSE_60_DEG_TO_25_DEG_CHAIN_SE_NW = 16963,
    SPR_WILD_MOUSE_60_DEG_TO_25_DEG_CHAIN_FRONT_NW_SE = 16964,
    SPR_WILD_MOUSE_60_DEG_TO_25_DEG_CHAIN_FRONT_NE_SW = 16965,
    SPR_WILD_MOUSE_60_DEG_CHAIN_SW_NE = 16966,
    SPR_WILD_MOUSE_60_DEG_CHAIN_NW_SE = 16967,
    SPR_WILD_MOUSE_60_DEG_CHAIN_NE_SW = 16968,
    SPR_WILD_MOUSE_60_DEG_CHAIN_SE_NW = 16969,
    SPR_WILD_MOUSE_FLAT_TO_60_DEG_SW_NE = 16970,
    SPR_WILD_MOUSE_FLAT_TO_60_DEG_NW_SE = 16971,
    SPR_WILD_MOUSE_FLAT_TO_60_DEG_NE_SW = 16972,
    SPR_WILD_MOUSE_FLAT_TO_60_DEG_SE_NW = 16973,
    SPR_WILD_MOUSE_FLAT_TO_60_DEG_FRONT_NW_SE = 16974,
    SPR_WILD_MOUSE_FLAT_TO_60_DEG_FRONT_NE_SW = 16975,
    SPR_WILD_MOUSE_60_DEG_TO_FLAT_SW_NE = 16976,
    SPR_WILD_MOUSE_60_DEG_TO_FLAT_NW_SE = 16977,
    SPR_WILD_MOUSE_60_DEG_TO_FLAT_NE_SW = 16978,
    SPR_WILD_MOUSE_60_DEG_TO_FLAT_SE_NW = 16979,
    SPR_WILD_MOUSE_60_DEG_TO_FLAT_FRONT_NW_SE = 16980,
    SPR_WILD_MOUSE_60_DEG_TO_FLAT_FRONT_NE_SW = 16981,
    SPR_WILD_MOUSE_FLAT_TO_60_DEG_CHAIN_SW_NE = 16982,
    SPR_WILD_MOUSE_FLAT_TO_60_DEG_CHAIN_NW_SE = 16983,
    SPR_WILD_MOUSE_FLAT_TO_60_DEG_CHAIN_NE_SW = 16984,
    SPR_WILD_MOUSE_FLAT_TO_60_DEG_CHAIN_SE_NW = 16985,
    SPR_WILD_MOUSE_FLAT_TO_60_DEG_CHAIN_FRONT_NW_SE = 16986,
    SPR_WILD_MOUSE_FLAT_TO_60_DEG_CHAIN_FRONT_NE_SW = 16987,
    SPR_WILD_MOUSE_60_DEG_TO_FLAT_CHAIN_SW_NE = 16988,
    SPR_WILD_MOUSE_60_DEG_TO_FLAT_CHAIN_NW_SE = 16989,
    SPR_WILD_MOUSE_60_DEG_TO_FLAT_CHAIN_NE_SW = 16990,
    SPR_WILD_MOUSE_60_DEG_TO_FLAT_CHAIN_SE_NW = 16991,
    SPR_WILD_MOUSE_60_DEG_TO_FLAT_CHAIN_FRONT_NW_SE = 16992,
    SPR_WILD_MOUSE_60_DEG_TO_FLAT_CHAIN_FRONT_NE_SW = 16993,
    SPR_WILD_MOUSE_QUARTER_TURN_1_SW_NE = 16994,
    SPR_WILD_MOUSE_QUARTER_TURN_1_NW_SE = 16995,
    SPR_WILD_MOUSE_QUARTER_TURN_1_NE_SW = 16996,
    SPR_WILD_MOUSE_QUARTER_TURN_1_SE_NW = 16997,
    SPR_WILD_MOUSE_QUARTER_TURN_3_SW_SE_PART_0 = 16998,
    SPR_WILD_MOUSE_QUARTER_TURN_3_SW_SE_PART_1 = 16999,
    SPR_WILD_MOUSE_QUARTER_TURN_3_SW_SE_PART_2 = 17000,
    SPR_WILD_MOUSE_QUARTER_TURN_3_NW_SW_PART_0 = 17001,
    SPR_WILD_MOUSE_QUARTER_TURN_3_NW_SW_PART_1 = 17002,
    SPR_WILD_MOUSE_QUARTER_TURN_3_NW_SW_PART_2 = 17003,
    SPR_WILD_MOUSE_QUARTER_TURN_3_NE_NW_PART_0 = 17004,
    SPR_WILD_MOUSE_QUARTER_TURN_3_NE_NW_PART_1 = 17005,
    SPR_WILD_MOUSE_QUARTER_TURN_3_NE_NW_PART_2 = 17006,
    SPR_WILD_MOUSE_QUARTER_TURN_3_SE_NE_PART_0 = 17007,
    SPR_WILD_MOUSE_QUARTER_TURN_3_SE_NE_PART_1 = 17008,
    SPR_WILD_MOUSE_QUARTER_TURN_3_SE_NE_PART_2 = 17009,
    SPR_WILD_MOUSE_QUARTER_TURN_3_25_DEG_UP_SW_SE_PART_0 = 17010,
    SPR_WILD_MOUSE_QUARTER_TURN_3_25_DEG_UP_SW_SE_PART_1 = 17011,
    SPR_WILD_MOUSE_QUARTER_TURN_3_25_DEG_UP_NW_SW_PART_0 = 17012,
    SPR_WILD_MOUSE_QUARTER_TURN_3_25_DEG_UP_NW_SW_PART_1 = 17013,
    SPR_WILD_MOUSE_QUARTER_TURN_3_25_DEG_UP_NE_NW_PART_0 = 17014,
    SPR_WILD_MOUSE_QUARTER_TURN_3_25_DEG_UP_NE_NW_PART_1 = 17015,
    SPR_WILD_MOUSE_QUARTER_TURN_3_25_DEG_UP_SE_NE_PART_0 = 17016,
    SPR_WILD_MOUSE_QUARTER_TURN_3_25_DEG_UP_SE_NE_PART_1 = 17017,
    SPR_WILD_MOUSE_QUARTER_TURN_3_25_DEG_DOWN_SW_SE_PART_0 = 17018,
    SPR_WILD_MOUSE_QUARTER_TURN_3_25_DEG_DOWN_SW_SE_PART_1 = 17019,
    SPR_WILD_MOUSE_QUARTER_TURN_3_25_DEG_DOWN_NW_SW_PART_0 = 17020,
    SPR_WILD_MOUSE_QUARTER_TURN_3_25_DEG_DOWN_NW_SW_PART_1 = 17021,
    SPR_WILD_MOUSE_QUARTER_TURN_3_25_DEG_DOWN_NE_NW_PART_0 = 17022,
    SPR_WILD_MOUSE_QUARTER_TURN_3_25_DEG_DOWN_NE_NW_PART_1 = 17023,
    SPR_WILD_MOUSE_QUARTER_TURN_3_25_DEG_DOWN_SE_NE_PART_0 = 17024,
    SPR_WILD_MOUSE_QUARTER_TURN_3_25_DEG_DOWN_SE_NE_PART_1 = 17025,
};

static constexpr const uint32_t _wild_mouse_brakes_image_ids[4] = {
    SPR_WILD_MOUSE_BRAKES_SW_NE,
    SPR_WILD_MOUSE_BRAKES_NW_SE,
    SPR_WILD_MOUSE_BRAKES_SW_NE,
    SPR_WILD_MOUSE_BRAKES_NW_SE,
};

static constexpr const uint32_t _wild_mouse_block_brakes_image_ids[NumOrthogonalDirections][2] = {
    { SPR_WILD_MOUSE_BLOCK_BRAKES_SW_NE_OPEN, SPR_WILD_MOUSE_BLOCK_BRAKES_SW_NE_CLOSED },
    { SPR_WILD_MOUSE_BLOCK_BRAKES_NW_SE_OPEN, SPR_WILD_MOUSE_BLOCK_BRAKES_NW_SE_CLOSED },
    { SPR_WILD_MOUSE_BLOCK_BRAKES_SW_NE_OPEN, SPR_WILD_MOUSE_BLOCK_BRAKES_SW_NE_CLOSED },
    { SPR_WILD_MOUSE_BLOCK_BRAKES_NW_SE_OPEN, SPR_WILD_MOUSE_BLOCK_BRAKES_NW_SE_CLOSED },
};

/** rct2: 0x0078B1E4 */
static void wild_mouse_track_flat(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    static constexpr const uint32_t imageIds[4][2] = {
        { SPR_WILD_MOUSE_FLAT_SW_NE, SPR_WILD_MOUSE_FLAT_CHAIN_SW_NE },
        { SPR_WILD_MOUSE_FLAT_NW_SE, SPR_WILD_MOUSE_FLAT_CHAIN_NW_SE },
        { SPR_WILD_MOUSE_FLAT_SW_NE, SPR_WILD_MOUSE_FLAT_CHAIN_NE_SW },
        { SPR_WILD_MOUSE_FLAT_NW_SE, SPR_WILD_MOUSE_FLAT_CHAIN_SE_NW },
    };

    uint8_t isChained = trackElement.HasChain() ? 1 : 0;
    uint32_t imageId = imageIds[direction][isChained] | session.TrackColours[SCHEME_TRACK];
    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
    if (track_paint_util_should_paint_supports(session.MapPosition))
    {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, -1, height, session.TrackColours[SCHEME_SUPPORTS]);
    }
    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
    paint_util_set_segment_support_height(
        session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

static void wild_mouse_track_station(
    paint_session& session, const Ride& ride, [[maybe_unused]] uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    static constexpr const uint32_t baseImageIds[4] = {
        SPR_STATION_BASE_B_SW_NE,
        SPR_STATION_BASE_B_NW_SE,
        SPR_STATION_BASE_B_SW_NE,
        SPR_STATION_BASE_B_NW_SE,
    };

    int32_t trackType = trackElement.GetTrackType();
    PaintAddImageAsParentRotated(
        session,  direction,  baseImageIds[direction] | session.TrackColours[SCHEME_MISC], { 0,  0,  height - 2}, { 32,  28,  2}, { 0,  2, 
        height);
        if (trackType == TrackElemType::EndStation)
        {
        bool isClosed = trackElement.BlockBrakeClosed(
        });
        PaintAddImageAsChildRotated(
            session, direction, _wild_mouse_block_brakes_image_ids[direction][isClosed] | session.TrackColours[SCHEME_TRACK], 0,
            0, 32, 20, 2, height, 0, 0, height);
    }
    else
    {
        PaintAddImageAsChildRotated(
            session, direction, _wild_mouse_brakes_image_ids[direction] | session.TrackColours[SCHEME_TRACK], 0, 0, 32, 20, 2,
            height, 0, 0, height);
    }
    track_paint_util_draw_station_metal_supports(session, direction, height, session.TrackColours[SCHEME_SUPPORTS]);
    track_paint_util_draw_station(session, ride, direction, height, trackElement);
    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x0078B1F4 */
static void wild_mouse_track_25_deg_up(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    static constexpr const uint32_t imageIds[4][2] = {
        { SPR_WILD_MOUSE_25_DEG_SW_NE, SPR_WILD_MOUSE_25_DEG_CHAIN_SW_NE },
        { SPR_WILD_MOUSE_25_DEG_NW_SE, SPR_WILD_MOUSE_25_DEG_CHAIN_NW_SE },
        { SPR_WILD_MOUSE_25_DEG_NE_SW, SPR_WILD_MOUSE_25_DEG_CHAIN_NE_SW },
        { SPR_WILD_MOUSE_25_DEG_SE_NW, SPR_WILD_MOUSE_25_DEG_CHAIN_SE_NW },
    };

    uint8_t isChained = trackElement.HasChain() ? 1 : 0;
    uint32_t imageId = imageIds[direction][isChained] | session.TrackColours[SCHEME_TRACK];
    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
    if (track_paint_util_should_paint_supports(session.MapPosition))
    {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, -9, height, session.TrackColours[SCHEME_SUPPORTS]);
    }
    if (direction == 0 || direction == 3)
    {
        paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_1);
    }
    else
    {
        paint_util_push_tunnel_rotated(session, direction, height + 8, TUNNEL_2);
    }
    paint_util_set_segment_support_height(
        session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 56, 0x20);
}

/** rct2: 0x0078B204 */
static void wild_mouse_track_60_deg_up(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    static constexpr const uint32_t imageIds[4][2] = {
        { SPR_WILD_MOUSE_60_DEG_SW_NE, SPR_WILD_MOUSE_60_DEG_CHAIN_SW_NE },
        { SPR_WILD_MOUSE_60_DEG_NW_SE, SPR_WILD_MOUSE_60_DEG_CHAIN_NW_SE },
        { SPR_WILD_MOUSE_60_DEG_NE_SW, SPR_WILD_MOUSE_60_DEG_CHAIN_NE_SW },
        { SPR_WILD_MOUSE_60_DEG_SE_NW, SPR_WILD_MOUSE_60_DEG_CHAIN_SE_NW },
    };

    uint8_t isChained = trackElement.HasChain() ? 1 : 0;
    uint32_t imageId = imageIds[direction][isChained] | session.TrackColours[SCHEME_TRACK];
    if (direction == 0 || direction == 3)
    {
        PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
    }
    else
    {
        PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { 32, 1, 98 }, { 0, 27, height });
    }
    if (track_paint_util_should_paint_supports(session.MapPosition))
    {
        if (direction == 0 || direction == 3)
        {
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, -33, height, session.TrackColours[SCHEME_SUPPORTS]);
        }
        else
        {
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 32, height, session.TrackColours[SCHEME_SUPPORTS]);
        }
    }
    if (direction == 0 || direction == 3)
    {
        paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_1);
    }
    else
    {
        paint_util_push_tunnel_rotated(session, direction, height + 56, TUNNEL_2);
    }
    paint_util_set_segment_support_height(
        session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 104, 0x20);
}

/** rct2: 0x0078B214 */
static void wild_mouse_track_flat_to_25_deg_up(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    static constexpr const uint32_t imageIds[4][2] = {
        { SPR_WILD_MOUSE_FLAT_TO_25_DEG_SW_NE, SPR_WILD_MOUSE_FLAT_TO_25_DEG_CHAIN_SW_NE },
        { SPR_WILD_MOUSE_FLAT_TO_25_DEG_NW_SE, SPR_WILD_MOUSE_FLAT_TO_25_DEG_CHAIN_NW_SE },
        { SPR_WILD_MOUSE_FLAT_TO_25_DEG_NE_SW, SPR_WILD_MOUSE_FLAT_TO_25_DEG_CHAIN_NE_SW },
        { SPR_WILD_MOUSE_FLAT_TO_25_DEG_SE_NW, SPR_WILD_MOUSE_FLAT_TO_25_DEG_CHAIN_SE_NW },
    };

    uint8_t isChained = trackElement.HasChain() ? 1 : 0;
    uint32_t imageId = imageIds[direction][isChained] | session.TrackColours[SCHEME_TRACK];
    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
    if (track_paint_util_should_paint_supports(session.MapPosition))
    {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, -4, height, session.TrackColours[SCHEME_SUPPORTS]);
    }
    if (direction == 0 || direction == 3)
    {
        paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
    }
    else
    {
        paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_2);
    }
    paint_util_set_segment_support_height(
        session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 48, 0x20);
}

/** rct2: 0x0078B224 */
static void wild_mouse_track_25_deg_up_to_60_deg_up(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    static constexpr const uint32_t imageIds[4][2] = {
        { SPR_WILD_MOUSE_25_DEG_TO_60_DEG_SW_NE, SPR_WILD_MOUSE_25_DEG_TO_60_DEG_CHAIN_SW_NE },
        { SPR_WILD_MOUSE_25_DEG_TO_60_DEG_NW_SE, SPR_WILD_MOUSE_25_DEG_TO_60_DEG_CHAIN_NW_SE },
        { SPR_WILD_MOUSE_25_DEG_TO_60_DEG_NE_SW, SPR_WILD_MOUSE_25_DEG_TO_60_DEG_CHAIN_NE_SW },
        { SPR_WILD_MOUSE_25_DEG_TO_60_DEG_SE_NW, SPR_WILD_MOUSE_25_DEG_TO_60_DEG_CHAIN_SE_NW },
    };
    static constexpr const uint32_t frontImageIds[4][2] = {
        { 0, 0 },
        { SPR_WILD_MOUSE_25_DEG_TO_60_DEG_FRONT_NW_SE, SPR_WILD_MOUSE_25_DEG_TO_60_DEG_CHAIN_FRONT_NW_SE },
        { SPR_WILD_MOUSE_25_DEG_TO_60_DEG_FRONT_NE_SW, SPR_WILD_MOUSE_25_DEG_TO_60_DEG_CHAIN_FRONT_NE_SW },
        { 0, 0 },
    };

    uint8_t isChained = trackElement.HasChain() ? 1 : 0;
    uint32_t imageId = imageIds[direction][isChained] | session.TrackColours[SCHEME_TRACK];
    uint32_t frontImageId = frontImageIds[direction][isChained] | session.TrackColours[SCHEME_TRACK];
    if (direction == 0 || direction == 3)
    {
        PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
    }
    else
    {
        PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
        PaintAddImageAsParentRotated(session, direction, frontImageId, { 0, 0, height }, { 32, 1, 66 }, { 0, 27, height });
    }
    if (track_paint_util_should_paint_supports(session.MapPosition))
    {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, -13, height, session.TrackColours[SCHEME_SUPPORTS]);
    }
    if (direction == 0 || direction == 3)
    {
        paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_1);
    }
    else
    {
        paint_util_push_tunnel_rotated(session, direction, height + 24, TUNNEL_2);
    }
    paint_util_set_segment_support_height(
        session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 72, 0x20);
}

/** rct2: 0x0078B234 */
static void wild_mouse_track_60_deg_up_to_25_deg_up(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    static constexpr const uint32_t imageIds[4][2] = {
        { SPR_WILD_MOUSE_60_DEG_TO_25_DEG_SW_NE, SPR_WILD_MOUSE_60_DEG_TO_25_DEG_CHAIN_SW_NE },
        { SPR_WILD_MOUSE_60_DEG_TO_25_DEG_NW_SE, SPR_WILD_MOUSE_60_DEG_TO_25_DEG_CHAIN_NW_SE },
        { SPR_WILD_MOUSE_60_DEG_TO_25_DEG_NE_SW, SPR_WILD_MOUSE_60_DEG_TO_25_DEG_CHAIN_NE_SW },
        { SPR_WILD_MOUSE_60_DEG_TO_25_DEG_SE_NW, SPR_WILD_MOUSE_60_DEG_TO_25_DEG_CHAIN_SE_NW },
    };
    static constexpr const uint32_t frontImageIds[4][2] = {
        { 0, 0 },
        { SPR_WILD_MOUSE_60_DEG_TO_25_DEG_FRONT_NW_SE, SPR_WILD_MOUSE_60_DEG_TO_25_DEG_CHAIN_FRONT_NW_SE },
        { SPR_WILD_MOUSE_60_DEG_TO_25_DEG_FRONT_NE_SW, SPR_WILD_MOUSE_60_DEG_TO_25_DEG_CHAIN_FRONT_NE_SW },
        { 0, 0 },
    };

    uint8_t isChained = trackElement.HasChain() ? 1 : 0;
    uint32_t imageId = imageIds[direction][isChained] | session.TrackColours[SCHEME_TRACK];
    uint32_t frontImageId = frontImageIds[direction][isChained] | session.TrackColours[SCHEME_TRACK];
    if (direction == 0 || direction == 3)
    {
        PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
    }
    else
    {
        PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
        PaintAddImageAsParentRotated(session, direction, frontImageId, { 0, 0, height }, { 32, 1, 66 }, { 0, 27, height });
    }
    if (track_paint_util_should_paint_supports(session.MapPosition))
    {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, -21, height, session.TrackColours[SCHEME_SUPPORTS]);
    }
    if (direction == 0 || direction == 3)
    {
        paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_1);
    }
    else
    {
        paint_util_push_tunnel_rotated(session, direction, height + 24, TUNNEL_2);
    }
    paint_util_set_segment_support_height(
        session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 72, 0x20);
}

/** rct2: 0x0078B244 */
static void wild_mouse_track_25_deg_up_to_flat(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    static constexpr const uint32_t imageIds[4][2] = {
        { SPR_WILD_MOUSE_25_DEG_TO_FLAT_SW_NE, SPR_WILD_MOUSE_25_DEG_TO_FLAT_CHAIN_SW_NE },
        { SPR_WILD_MOUSE_25_DEG_TO_FLAT_NW_SE, SPR_WILD_MOUSE_25_DEG_TO_FLAT_CHAIN_NW_SE },
        { SPR_WILD_MOUSE_25_DEG_TO_FLAT_NE_SW, SPR_WILD_MOUSE_25_DEG_TO_FLAT_CHAIN_NE_SW },
        { SPR_WILD_MOUSE_25_DEG_TO_FLAT_SE_NW, SPR_WILD_MOUSE_25_DEG_TO_FLAT_CHAIN_SE_NW },
    };

    uint8_t isChained = trackElement.HasChain() ? 1 : 0;
    uint32_t imageId = imageIds[direction][isChained] | session.TrackColours[SCHEME_TRACK];
    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
    if (track_paint_util_should_paint_supports(session.MapPosition))
    {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, -7, height, session.TrackColours[SCHEME_SUPPORTS]);
    }
    if (direction == 0 || direction == 3)
    {
        paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_0);
    }
    else
    {
        paint_util_push_tunnel_rotated(session, direction, height + 8, TUNNEL_12);
    }
    paint_util_set_segment_support_height(
        session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 40, 0x20);
}

/** rct2: 0x0078B254 */
static void wild_mouse_track_25_deg_down(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    wild_mouse_track_25_deg_up(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x0078B264 */
static void wild_mouse_track_60_deg_down(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    wild_mouse_track_60_deg_up(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x0078B274 */
static void wild_mouse_track_flat_to_25_deg_down(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    wild_mouse_track_25_deg_up_to_flat(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x0078B284 */
static void wild_mouse_track_25_deg_down_to_60_deg_down(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    wild_mouse_track_60_deg_up_to_25_deg_up(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x0078B294 */
static void wild_mouse_track_60_deg_down_to_25_deg_down(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    wild_mouse_track_25_deg_up_to_60_deg_up(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x0078B2A4 */
static void wild_mouse_track_25_deg_down_to_flat(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    wild_mouse_track_flat_to_25_deg_up(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

static void wild_mouse_track_right_quarter_turn_3(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    static constexpr const sprite_bb imageIds[4][3] = {
        {
            { SPR_WILD_MOUSE_QUARTER_TURN_3_SW_SE_PART_0, { 0, 0, 0 }, { 0, 6, 0 }, { 32, 20, 3 } },
            { SPR_WILD_MOUSE_QUARTER_TURN_3_SW_SE_PART_1, { 0, 0, 0 }, { 16, 16, 0 }, { 16, 16, 3 } },
            { SPR_WILD_MOUSE_QUARTER_TURN_3_SW_SE_PART_2, { 0, 0, 0 }, { 6, 0, 0 }, { 20, 32, 3 } },
        },
        {
            { SPR_WILD_MOUSE_QUARTER_TURN_3_NW_SW_PART_0, { 0, 0, 0 }, { 6, 0, 0 }, { 20, 32, 3 } },
            { SPR_WILD_MOUSE_QUARTER_TURN_3_NW_SW_PART_1, { 0, 0, 0 }, { 16, 0, 0 }, { 16, 16, 3 } },
            { SPR_WILD_MOUSE_QUARTER_TURN_3_NW_SW_PART_2, { 0, 0, 0 }, { 0, 6, 0 }, { 32, 20, 3 } },
        },
        {
            { SPR_WILD_MOUSE_QUARTER_TURN_3_NE_NW_PART_0, { 0, 0, 0 }, { 0, 6, 0 }, { 32, 20, 3 } },
            { SPR_WILD_MOUSE_QUARTER_TURN_3_NE_NW_PART_1, { 0, 0, 0 }, { 0, 0, 0 }, { 16, 16, 3 } },
            { SPR_WILD_MOUSE_QUARTER_TURN_3_NE_NW_PART_2, { 0, 0, 0 }, { 6, 0, 0 }, { 20, 32, 3 } },
        },
        {
            { SPR_WILD_MOUSE_QUARTER_TURN_3_SE_NE_PART_0, { 0, 0, 0 }, { 6, 0, 0 }, { 20, 32, 3 } },
            { SPR_WILD_MOUSE_QUARTER_TURN_3_SE_NE_PART_1, { 0, 0, 0 }, { 0, 16, 0 }, { 16, 16, 3 } },
            { SPR_WILD_MOUSE_QUARTER_TURN_3_SE_NE_PART_2, { 0, 0, 0 }, { 0, 6, 0 }, { 32, 20, 3 } },
        },
    };

    track_paint_util_right_quarter_turn_3_tiles_paint_3(
        session, height, direction, trackSequence, session.TrackColours[SCHEME_TRACK], imageIds);
    track_paint_util_right_quarter_turn_3_tiles_tunnel(session, height, direction, trackSequence, TUNNEL_0);

    switch (trackSequence)
    {
        case 0:
        case 3:
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, -1, height, session.TrackColours[SCHEME_SUPPORTS]);
            break;
    }

    int32_t blockedSegments = 0;
    switch (trackSequence)
    {
        case 0:
            blockedSegments = SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0;
            break;
        case 2:
            blockedSegments = SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4;
            break;
        case 3:
            blockedSegments = SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4;
            break;
    }
    paint_util_set_segment_support_height(session, paint_util_rotate_segments(blockedSegments, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

static void wild_mouse_track_right_quarter_turn_3_25_deg_down(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    static constexpr const sprite_bb imageIds[4][2] = {
        {
            { SPR_WILD_MOUSE_QUARTER_TURN_3_25_DEG_DOWN_SW_SE_PART_0, { 0, 6, 0 }, { 0, 0, 0 }, { 32, 20, 3 } },
            { SPR_WILD_MOUSE_QUARTER_TURN_3_25_DEG_DOWN_SW_SE_PART_1, { 6, 0, 0 }, { 0, 0, 0 }, { 20, 32, 3 } },
        },
        {
            { SPR_WILD_MOUSE_QUARTER_TURN_3_25_DEG_DOWN_NW_SW_PART_0, { 6, 0, 0 }, { 0, 0, 0 }, { 20, 32, 3 } },
            { SPR_WILD_MOUSE_QUARTER_TURN_3_25_DEG_DOWN_NW_SW_PART_1, { 0, 6, 0 }, { 0, 0, 0 }, { 32, 20, 3 } },
        },
        {
            { SPR_WILD_MOUSE_QUARTER_TURN_3_25_DEG_DOWN_NE_NW_PART_0, { 0, 6, 0 }, { 0, 0, 0 }, { 32, 20, 3 } },
            { SPR_WILD_MOUSE_QUARTER_TURN_3_25_DEG_DOWN_NE_NW_PART_1, { 6, 0, 0 }, { 0, 0, 0 }, { 20, 32, 3 } },
        },
        {
            { SPR_WILD_MOUSE_QUARTER_TURN_3_25_DEG_DOWN_SE_NE_PART_0, { 6, 0, 0 }, { 0, 0, 0 }, { 20, 32, 3 } },
            { SPR_WILD_MOUSE_QUARTER_TURN_3_25_DEG_DOWN_SE_NE_PART_1, { 0, 6, 0 }, { 0, 0, 0 }, { 32, 20, 3 } },
        },
    };
    static constexpr const int16_t generalSupportHeights[] = { 72, 56, 56, 72 };

    if (trackSequence == 0 || trackSequence == 3)
    {
        int32_t part = trackSequence == 0 ? 0 : 1;
        const sprite_bb* sbb = &imageIds[direction][part];
        const auto& offset = sbb->offset;
        PaintAddImageAsParent(
            session, sbb->sprite_id | session.TrackColours[SCHEME_TRACK], { offset.x, offset.y, height + offset.z },
            sbb->bb_size);
    }

    track_paint_util_right_quarter_turn_3_tiles_25_deg_down_tunnel(
        session, height, direction, trackSequence, TUNNEL_2, TUNNEL_1);

    switch (trackSequence)
    {
        case 0:
        case 3:
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, -9, height, session.TrackColours[SCHEME_SUPPORTS]);
            break;
    }

    int32_t blockedSegments = 0;
    switch (trackSequence)
    {
        case 0:
            blockedSegments = SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0;
            break;
        case 3:
            blockedSegments = SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4;
            break;
    }
    paint_util_set_segment_support_height(session, paint_util_rotate_segments(blockedSegments, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + generalSupportHeights[trackSequence], 0x20);
}

static void wild_mouse_track_left_quarter_turn_3_25_deg_up(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    wild_mouse_track_right_quarter_turn_3_25_deg_down(session, ride, trackSequence, (direction + 1) % 4, height, trackElement);
}

/** rct2: 0x0078B314 */
static void wild_mouse_track_right_quarter_turn_3_25_deg_up(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    static constexpr const sprite_bb imageIds[4][2] = {
        {
            { SPR_WILD_MOUSE_QUARTER_TURN_3_25_DEG_UP_SW_SE_PART_0, { 0, 6, 0 }, { 0, 0, 0 }, { 32, 20, 3 } },
            { SPR_WILD_MOUSE_QUARTER_TURN_3_25_DEG_UP_SW_SE_PART_1, { 6, 0, 0 }, { 0, 0, 0 }, { 20, 32, 3 } },
        },
        {
            { SPR_WILD_MOUSE_QUARTER_TURN_3_25_DEG_UP_NW_SW_PART_0, { 6, 0, 0 }, { 0, 0, 0 }, { 20, 32, 3 } },
            { SPR_WILD_MOUSE_QUARTER_TURN_3_25_DEG_UP_NW_SW_PART_1, { 0, 6, 0 }, { 0, 0, 0 }, { 32, 20, 3 } },
        },
        {
            { SPR_WILD_MOUSE_QUARTER_TURN_3_25_DEG_UP_NE_NW_PART_0, { 0, 6, 0 }, { 0, 0, 0 }, { 32, 20, 3 } },
            { SPR_WILD_MOUSE_QUARTER_TURN_3_25_DEG_UP_NE_NW_PART_1, { 6, 0, 0 }, { 0, 0, 0 }, { 20, 32, 3 } },
        },
        {
            { SPR_WILD_MOUSE_QUARTER_TURN_3_25_DEG_UP_SE_NE_PART_0, { 6, 0, 0 }, { 0, 0, 0 }, { 20, 32, 3 } },
            { SPR_WILD_MOUSE_QUARTER_TURN_3_25_DEG_UP_SE_NE_PART_1, { 0, 6, 0 }, { 0, 0, 0 }, { 32, 20, 3 } },
        },
    };
    static constexpr const int16_t generalSupportHeights[] = { 72, 56, 56, 72 };

    if (trackSequence == 0 || trackSequence == 3)
    {
        int32_t part = trackSequence == 0 ? 0 : 1;
        const sprite_bb* sbb = &imageIds[direction][part];
        PaintAddImageAsParent(
            session, sbb->sprite_id | session.TrackColours[SCHEME_TRACK],
            { sbb->offset.x, sbb->offset.y, height + sbb->offset.z }, sbb->bb_size);
    }

    track_paint_util_right_quarter_turn_3_tiles_25_deg_up_tunnel(session, height, direction, trackSequence, TUNNEL_1, TUNNEL_2);

    switch (trackSequence)
    {
        case 0:
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, -9, height, session.TrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            if (direction == 2)
            {
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES, 4, -11, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            else
            {
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES, 4, -9, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
    }

    int32_t blockedSegments = 0;
    switch (trackSequence)
    {
        case 0:
            blockedSegments = SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0;
            break;
        case 3:
            blockedSegments = SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4;
            break;
    }
    paint_util_set_segment_support_height(session, paint_util_rotate_segments(blockedSegments, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + generalSupportHeights[trackSequence], 0x20);
}

/** rct2: 0x0078B324 */
static void wild_mouse_track_left_quarter_turn_3_25_deg_down(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    wild_mouse_track_right_quarter_turn_3_25_deg_up(session, ride, trackSequence, (direction + 1) % 4, height, trackElement);
}

static void wild_mouse_track_left_quarter_turn_3(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    wild_mouse_track_right_quarter_turn_3(session, ride, trackSequence, (direction + 1) % 4, height, trackElement);
}

/** rct2: 0x0078B394 */
static void wild_mouse_track_left_quarter_turn_1(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    static constexpr const uint32_t imageIds[4] = {
        SPR_WILD_MOUSE_QUARTER_TURN_1_SW_NE,
        SPR_WILD_MOUSE_QUARTER_TURN_1_NW_SE,
        SPR_WILD_MOUSE_QUARTER_TURN_1_NE_SW,
        SPR_WILD_MOUSE_QUARTER_TURN_1_SE_NW,
    };

    uint32_t imageId = imageIds[direction] | session.TrackColours[SCHEME_TRACK];
    switch (direction)
    {
        case 0:
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 26, 24, 2 }, { 6, 2, height });
            break;
        case 1:
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 26, 26, 2 });
            break;
        case 2:
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 24, 26, 2 }, { 2, 6, height });
            break;
        case 3:
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 24, 24, 2 }, { 6, 6, height });
            break;
    }
    metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, -1, height, session.TrackColours[SCHEME_SUPPORTS]);
    track_paint_util_left_quarter_turn_1_tile_tunnel(session, direction, height, 0, TUNNEL_0, 0, TUNNEL_0);
    paint_util_set_segment_support_height(
        session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x0078B3A4 */
static void wild_mouse_track_right_quarter_turn_1(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    wild_mouse_track_left_quarter_turn_1(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x0078B354 */
static void wild_mouse_track_flat_to_60_deg_up(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    static constexpr const uint32_t imageIds[4][2] = {
        { SPR_WILD_MOUSE_FLAT_TO_60_DEG_SW_NE, SPR_WILD_MOUSE_FLAT_TO_60_DEG_CHAIN_SW_NE },
        { SPR_WILD_MOUSE_FLAT_TO_60_DEG_NW_SE, SPR_WILD_MOUSE_FLAT_TO_60_DEG_CHAIN_NW_SE },
        { SPR_WILD_MOUSE_FLAT_TO_60_DEG_NE_SW, SPR_WILD_MOUSE_FLAT_TO_60_DEG_CHAIN_NE_SW },
        { SPR_WILD_MOUSE_FLAT_TO_60_DEG_SE_NW, SPR_WILD_MOUSE_FLAT_TO_60_DEG_CHAIN_SE_NW },
    };
    static constexpr const uint32_t frontImageIds[4][2] = {
        { 0, 0 },
        { SPR_WILD_MOUSE_FLAT_TO_60_DEG_FRONT_NW_SE, SPR_WILD_MOUSE_FLAT_TO_60_DEG_CHAIN_FRONT_NW_SE },
        { SPR_WILD_MOUSE_FLAT_TO_60_DEG_FRONT_NE_SW, SPR_WILD_MOUSE_FLAT_TO_60_DEG_CHAIN_FRONT_NE_SW },
        { 0, 0 },
    };

    uint8_t isChained = trackElement.HasChain() ? 1 : 0;
    uint32_t imageId = imageIds[direction][isChained] | session.TrackColours[SCHEME_TRACK];
    uint32_t frontImageId = frontImageIds[direction][isChained] | session.TrackColours[SCHEME_TRACK];
    if (direction == 0 || direction == 3)
    {
        PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { 32, 27, 2 }, { 0, 2, height });
    }
    else
    {
        PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { 1, 24, 43 }, { 29, 4, height + 2 });
        PaintAddImageAsParentRotated(session, direction, frontImageId, { 0, 0, height }, { 32, 2, 43 }, { 0, 4, height });
    }
    if (track_paint_util_should_paint_supports(session.MapPosition))
    {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, -5, height, session.TrackColours[SCHEME_SUPPORTS]);
    }
    if (direction == 0 || direction == 3)
    {
        paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
    }
    else
    {
        paint_util_push_tunnel_rotated(session, direction, height + 24, TUNNEL_2);
    }
    paint_util_set_segment_support_height(
        session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 64, 0x20);
}

/** rct2: 0x0078B364 */
static void wild_mouse_track_60_deg_up_to_flat(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    static constexpr const uint32_t imageIds[4][2] = {
        { SPR_WILD_MOUSE_60_DEG_TO_FLAT_SW_NE, SPR_WILD_MOUSE_60_DEG_TO_FLAT_CHAIN_SW_NE },
        { SPR_WILD_MOUSE_60_DEG_TO_FLAT_NW_SE, SPR_WILD_MOUSE_60_DEG_TO_FLAT_CHAIN_NW_SE },
        { SPR_WILD_MOUSE_60_DEG_TO_FLAT_NE_SW, SPR_WILD_MOUSE_60_DEG_TO_FLAT_CHAIN_NE_SW },
        { SPR_WILD_MOUSE_60_DEG_TO_FLAT_SE_NW, SPR_WILD_MOUSE_60_DEG_TO_FLAT_CHAIN_SE_NW },
    };
    static constexpr const uint32_t frontImageIds[4][2] = {
        { 0, 0 },
        { SPR_WILD_MOUSE_60_DEG_TO_FLAT_FRONT_NW_SE, SPR_WILD_MOUSE_60_DEG_TO_FLAT_CHAIN_FRONT_NW_SE },
        { SPR_WILD_MOUSE_60_DEG_TO_FLAT_FRONT_NE_SW, SPR_WILD_MOUSE_60_DEG_TO_FLAT_CHAIN_FRONT_NE_SW },
        { 0, 0 },
    };

    uint8_t isChained = trackElement.HasChain() ? 1 : 0;
    uint32_t imageId = imageIds[direction][isChained] | session.TrackColours[SCHEME_TRACK];
    uint32_t frontImageId = frontImageIds[direction][isChained] | session.TrackColours[SCHEME_TRACK];
    if (direction == 0 || direction == 3)
    {
        PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { 32, 27, 2 }, { 0, 2, height });
    }
    else
    {
        PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { 1, 24, 43 }, { 29, 4, height + 2 });
        PaintAddImageAsParentRotated(session, direction, frontImageId, { 0, 0, height }, { 32, 2, 43 }, { 0, 4, height });
    }
    if (track_paint_util_should_paint_supports(session.MapPosition))
    {
        if (direction == 0 || direction == 3)
        {
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, -17, height, session.TrackColours[SCHEME_SUPPORTS]);
        }
        else
        {
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 16, height, session.TrackColours[SCHEME_SUPPORTS]);
        }
    }
    if (direction == 0 || direction == 3)
    {
        paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_1);
    }
    else
    {
        paint_util_push_tunnel_rotated(session, direction, height + 24, TUNNEL_0);
    }
    paint_util_set_segment_support_height(
        session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 72, 0x20);
}

/** rct2: 0x0078B374 */
static void wild_mouse_track_flat_to_60_deg_down(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    wild_mouse_track_60_deg_up_to_flat(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x0078B384 */
static void wild_mouse_track_60_deg_down_to_flat(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    wild_mouse_track_flat_to_60_deg_up(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x0078B344 */
static void wild_mouse_track_brakes(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    uint32_t imageId = _wild_mouse_brakes_image_ids[direction] | session.TrackColours[SCHEME_TRACK];
    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
    if (track_paint_util_should_paint_supports(session.MapPosition))
    {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
    }
    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
    paint_util_set_segment_support_height(
        session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x0078B3C4 */
static void wild_mouse_track_rotation_control_toggle(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    static constexpr const uint32_t imageIds[4] = {
        SPR_WILD_MOUSE_ROTATION_CONTROL_TOGGLE_SW_NE,
        SPR_WILD_MOUSE_ROTATION_CONTROL_TOGGLE_NW_SE,
        SPR_WILD_MOUSE_ROTATION_CONTROL_TOGGLE_SW_NE,
        SPR_WILD_MOUSE_ROTATION_CONTROL_TOGGLE_NW_SE,
    };

    uint32_t imageId = imageIds[direction] | session.TrackColours[SCHEME_TRACK];
    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
    if (track_paint_util_should_paint_supports(session.MapPosition))
    {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
    }
    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
    paint_util_set_segment_support_height(
        session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x0078B3B4 */
static void wild_mouse_track_block_brakes(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    bool isClosed = trackElement.BlockBrakeClosed();
    uint32_t imageId = _wild_mouse_block_brakes_image_ids[direction][isClosed] | session.TrackColours[SCHEME_TRACK];
    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
    if (track_paint_util_should_paint_supports(session.MapPosition))
    {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
    }
    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
    paint_util_set_segment_support_height(
        session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

TRACK_PAINT_FUNCTION get_track_paint_function_wild_mouse(int32_t trackType)
{
    switch (trackType)
    {
        case TrackElemType::Flat:
            return wild_mouse_track_flat;
        case TrackElemType::EndStation:
        case TrackElemType::BeginStation:
        case TrackElemType::MiddleStation:
            return wild_mouse_track_station;
        case TrackElemType::Up25:
            return wild_mouse_track_25_deg_up;
        case TrackElemType::Up60:
            return wild_mouse_track_60_deg_up;
        case TrackElemType::FlatToUp25:
            return wild_mouse_track_flat_to_25_deg_up;
        case TrackElemType::Up25ToUp60:
            return wild_mouse_track_25_deg_up_to_60_deg_up;
        case TrackElemType::Up60ToUp25:
            return wild_mouse_track_60_deg_up_to_25_deg_up;
        case TrackElemType::Up25ToFlat:
            return wild_mouse_track_25_deg_up_to_flat;
        case TrackElemType::Down25:
            return wild_mouse_track_25_deg_down;
        case TrackElemType::Down60:
            return wild_mouse_track_60_deg_down;
        case TrackElemType::FlatToDown25:
            return wild_mouse_track_flat_to_25_deg_down;
        case TrackElemType::Down25ToDown60:
            return wild_mouse_track_25_deg_down_to_60_deg_down;
        case TrackElemType::Down60ToDown25:
            return wild_mouse_track_60_deg_down_to_25_deg_down;
        case TrackElemType::Down25ToFlat:
            return wild_mouse_track_25_deg_down_to_flat;
        case TrackElemType::LeftQuarterTurn3Tiles:
            return wild_mouse_track_left_quarter_turn_3;
        case TrackElemType::RightQuarterTurn3Tiles:
            return wild_mouse_track_right_quarter_turn_3;
        case TrackElemType::LeftQuarterTurn3TilesUp25:
            return wild_mouse_track_left_quarter_turn_3_25_deg_up;
        case TrackElemType::RightQuarterTurn3TilesUp25:
            return wild_mouse_track_right_quarter_turn_3_25_deg_up;
        case TrackElemType::LeftQuarterTurn3TilesDown25:
            return wild_mouse_track_left_quarter_turn_3_25_deg_down;
        case TrackElemType::RightQuarterTurn3TilesDown25:
            return wild_mouse_track_right_quarter_turn_3_25_deg_down;
        case TrackElemType::LeftQuarterTurn1Tile:
            return wild_mouse_track_left_quarter_turn_1;
        case TrackElemType::RightQuarterTurn1Tile:
            return wild_mouse_track_right_quarter_turn_1;
        case TrackElemType::FlatToUp60:
            return wild_mouse_track_flat_to_60_deg_up;
        case TrackElemType::Up60ToFlat:
            return wild_mouse_track_60_deg_up_to_flat;
        case TrackElemType::FlatToDown60:
            return wild_mouse_track_flat_to_60_deg_down;
        case TrackElemType::Down60ToFlat:
            return wild_mouse_track_60_deg_down_to_flat;
        case TrackElemType::Brakes:
            return wild_mouse_track_brakes;
        case TrackElemType::RotationControlToggle:
            return wild_mouse_track_rotation_control_toggle;
        case TrackElemType::BlockBrakes:
            return wild_mouse_track_block_brakes;
    }
    return nullptr;
}
