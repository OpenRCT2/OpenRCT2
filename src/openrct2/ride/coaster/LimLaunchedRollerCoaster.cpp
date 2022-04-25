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

static constexpr const uint32_t LIM_LAUNCHED_RC_BLOCK_BRAKE_SW_NE_OPEN = 15020;
static constexpr const uint32_t LIM_LAUNCHED_RC_BLOCK_BRAKE_NW_SE_OPEN = 15021;
static constexpr const uint32_t LIM_LAUNCHED_RC_BLOCK_BRAKE_SW_NE_CLOSED = 15022;
static constexpr const uint32_t LIM_LAUNCHED_RC_BLOCK_BRAKE_NW_SE_CLOSED = 15023;

static constexpr const uint32_t _LimLauncherBlockBrakeImages[NumOrthogonalDirections][2] = {
    { LIM_LAUNCHED_RC_BLOCK_BRAKE_SW_NE_OPEN, LIM_LAUNCHED_RC_BLOCK_BRAKE_SW_NE_CLOSED },
    { LIM_LAUNCHED_RC_BLOCK_BRAKE_NW_SE_OPEN, LIM_LAUNCHED_RC_BLOCK_BRAKE_NW_SE_CLOSED },
    { LIM_LAUNCHED_RC_BLOCK_BRAKE_SW_NE_OPEN, LIM_LAUNCHED_RC_BLOCK_BRAKE_SW_NE_CLOSED },
    { LIM_LAUNCHED_RC_BLOCK_BRAKE_NW_SE_OPEN, LIM_LAUNCHED_RC_BLOCK_BRAKE_NW_SE_CLOSED },
};

/** rct2: 0x008A6D50, 0x008A6D60, 0x008A6D70 */
static void lim_launched_rc_track_station(
    paint_session& session, const Ride& ride, [[maybe_unused]] uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    static constexpr const uint32_t imageIds[4][2] = {
        { 15018, SPR_STATION_BASE_B_SW_NE },
        { 15019, SPR_STATION_BASE_B_NW_SE },
        { 15018, SPR_STATION_BASE_B_SW_NE },
        { 15019, SPR_STATION_BASE_B_NW_SE },
    };

    if (trackElement.GetTrackType() == TrackElemType::EndStation)
    {
        bool isClosed = trackElement.BlockBrakeClosed();
        PaintAddImageAsParentRotated(
            session, direction, _LimLauncherBlockBrakeImages[direction][isClosed] | session.TrackColours[SCHEME_TRACK],
            { 0, 0, height }, { 32, 20, 1 }, { 0, 6, height + 3 });
    }
    else
    {
        PaintAddImageAsParentRotated(
            session, direction, imageIds[direction][0] | session.TrackColours[SCHEME_TRACK], { 0, 0, height }, { 32, 20, 1 },
            { 0, 6, height + 3 });
    }
    PaintAddImageAsParentRotated(
        session, direction, imageIds[direction][1] | session.TrackColours[SCHEME_MISC], { 0, 0, height }, { 32, 32, 1 });
    track_paint_util_draw_station_metal_supports_2(session, direction, height, session.TrackColours[SCHEME_SUPPORTS], 0);
    track_paint_util_draw_station(session, ride, direction, height, trackElement);
    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x008A65E0 */
static void lim_launched_rc_track_left_vertical_loop(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15388, { 0, 6, height }, { 32, 20, 3 });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15396, { 0, 6, height }, { 32, 20, 3 });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15395, { 0, 6, height }, { 32, 20, 3 });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15403, { 0, 6, height }, { 32, 20, 7 });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_1);
            }
            paint_util_set_segment_support_height(
                session,
                paint_util_rotate_segments(
                    SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 56, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15389, { 0, 0, height }, { 32, 26, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15397, { 0, 14, height }, { 32, 2, 63 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15394, { 0, 6, height }, { 32, 26, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15402, { 0, 6, height }, { 32, 26, 3 });
                    break;
            }
            paint_util_set_segment_support_height(
                session,
                paint_util_rotate_segments(
                    SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 72, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15390, { 16, 0, height }, { 3, 16, 119 },
                        { 16, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15398, { 12, 0, height }, { 3, 16, 119 },
                        { 12, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15393, { 10, 16, height }, { 4, 16, 119 },
                        { 10, 16, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15401, { 16, 16, height }, { 2, 16, 119 },
                        { 16, 16, height });
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 168, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15391, { 0, 0, height + 32 }, { 32, 16, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15399, { 0, 0, height + 32 }, { 32, 16, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15392, { 0, 16, height + 32 }, { 32, 16, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15400, { 0, 16, height + 32 }, { 32, 16, 3 });
                    break;
            }
            paint_util_set_segment_support_height(
                session,
                paint_util_rotate_segments(
                    SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
        case 4:
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
        case 5:
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15392, { 0, 16, height + 32 }, { 32, 16, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15400, { 0, 16, height + 32 }, { 32, 16, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15391, { 0, 0, height + 32 }, { 32, 16, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15399, { 0, 0, height + 32 }, { 32, 16, 3 });
                    break;
            }
            paint_util_set_segment_support_height(
                session,
                paint_util_rotate_segments(
                    SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
        case 7:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15393, { 10, 16, height }, { 4, 16, 119 },
                        { 10, 16, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15401, { 16, 16, height }, { 2, 16, 119 },
                        { 16, 16, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15390, { 16, 0, height }, { 3, 16, 119 },
                        { 16, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15398, { 12, 0, height }, { 3, 16, 119 },
                        { 12, 0, height });
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 168, 0x20);
            break;
        case 8:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15394, { 0, 6, height }, { 32, 26, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15402, { 0, 6, height }, { 32, 26, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15389, { 0, 0, height }, { 32, 26, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15397, { 0, 14, height }, { 32, 2, 63 });
                    break;
            }
            paint_util_set_segment_support_height(
                session,
                paint_util_rotate_segments(
                    SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 72, 0x20);
            break;
        case 9:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15395, { 0, 6, height }, { 32, 20, 3 });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15403, { 0, 6, height }, { 32, 20, 7 });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15388, { 0, 6, height }, { 32, 20, 3 });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15396, { 0, 6, height }, { 32, 20, 3 });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            switch (direction)
            {
                case 1:
                    paint_util_push_tunnel_right(session, height - 8, TUNNEL_1);
                    break;
                case 2:
                    paint_util_push_tunnel_left(session, height - 8, TUNNEL_1);
                    break;
            }
            paint_util_set_segment_support_height(
                session,
                paint_util_rotate_segments(
                    SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 56, 0x20);
            break;
    }
}

/** rct2: 0x008A65F0 */
static void lim_launched_rc_track_right_vertical_loop(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15419, { 0, 6, height }, { 32, 20, 7 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15411, { 0, 6, height }, { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15412, { 0, 6, height }, { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15404, { 0, 6, height }, { 32, 20, 3 });
                    break;
            }
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_1);
            }
            paint_util_set_general_support_height(session, height + 56, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15418, { 0, 6, height }, { 32, 26, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15410, { 0, 6, height }, { 32, 26, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15413, { 0, 14, height }, { 32, 2, 63 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15405, { 0, 0, height }, { 32, 26, 3 });
                    break;
            }
            paint_util_set_general_support_height(session, height + 72, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15417, { 16, 16, height }, { 2, 16, 119 },
                        { 16, 16, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15409, { 10, 16, height }, { 4, 16, 119 },
                        { 10, 16, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15414, { 12, 0, height }, { 3, 16, 119 },
                        { 12, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15406, { 16, 0, height }, { 2, 16, 119 },
                        { 16, 0, height });
                    break;
            }
            paint_util_set_general_support_height(session, height + 168, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15416, { 0, 16, height + 32 }, { 32, 16, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15408, { 0, 16, height + 32 }, { 32, 16, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15415, { 0, 0, height + 32 }, { 32, 16, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15407, { 0, 0, height + 32 }, { 32, 16, 3 });
                    break;
            }
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
        case 4:
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
        case 5:
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15415, { 0, 0, height + 32 }, { 32, 16, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15407, { 0, 0, height + 32 }, { 32, 16, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15416, { 0, 16, height + 32 }, { 32, 16, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15408, { 0, 16, height + 32 }, { 32, 16, 3 });
                    break;
            }
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
        case 7:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15414, { 12, 0, height }, { 3, 16, 119 },
                        { 12, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15406, { 16, 0, height }, { 2, 16, 119 },
                        { 16, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15417, { 16, 16, height }, { 2, 16, 119 },
                        { 16, 16, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15409, { 10, 16, height }, { 4, 16, 119 },
                        { 10, 16, height });
                    break;
            }
            paint_util_set_general_support_height(session, height + 168, 0x20);
            break;
        case 8:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15413, { 0, 14, height }, { 32, 2, 63 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15405, { 0, 0, height }, { 32, 26, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15418, { 0, 6, height }, { 32, 26, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15410, { 0, 6, height }, { 32, 26, 3 });
                    break;
            }
            paint_util_set_general_support_height(session, height + 72, 0x20);
            break;
        case 9:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15412, { 0, 6, height }, { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15404, { 0, 6, height }, { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15419, { 0, 6, height }, { 32, 20, 7 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15411, { 0, 6, height }, { 32, 20, 3 });
                    break;
            }
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
            switch (direction)
            {
                case 1:
                    paint_util_push_tunnel_right(session, height - 8, TUNNEL_1);
                    break;
                case 2:
                    paint_util_push_tunnel_left(session, height - 8, TUNNEL_1);
                    break;
            }
            paint_util_set_general_support_height(session, height + 56, 0x20);
            break;
    }

    track_paint_util_right_vertical_loop_segments(session, direction, trackSequence);
}

/** rct2: 0x008A6D10 */
static void lim_launched_rc_track_left_twist_down_to_up(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15758, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15782, { 0, 0, height }, { 32, 20, 0 },
                        { 0, 6, height + 28 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15761, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15785, { 0, 0, height }, { 32, 20, 0 },
                        { 0, 6, height + 28 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15764, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15788, { 0, 0, height }, { 32, 20, 0 },
                        { 0, 6, height + 28 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15767, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15791, { 0, 0, height }, { 32, 20, 0 },
                        { 0, 6, height + 28 });
                    break;
            }
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15759, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15783, { 0, 0, height }, { 32, 20, 0 },
                        { 0, 6, height + 28 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15762, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15786, { 0, 0, height }, { 32, 20, 0 },
                        { 0, 6, height + 28 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15765, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15789, { 0, 0, height }, { 32, 20, 0 },
                        { 0, 6, height + 28 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15768, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15792, { 0, 0, height }, { 32, 20, 0 },
                        { 0, 6, height + 28 });
                    break;
            }
            paint_util_set_segment_support_height(
                session,
                paint_util_rotate_segments(
                    SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15760, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15784, { 0, 0, height }, { 32, 20, 0 },
                        { 0, 6, height + 44 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15763, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15787, { 0, 0, height }, { 32, 20, 0 },
                        { 0, 6, height + 44 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15766, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15790, { 0, 0, height }, { 32, 20, 0 },
                        { 0, 6, height + 44 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15769, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15793, { 0, 0, height }, { 32, 20, 0 },
                        { 0, 6, height + 44 });
                    break;
            }
            switch (direction)
            {
                case 1:
                    paint_util_push_tunnel_right(session, height - 32, TUNNEL_INVERTED_3);
                    break;
                case 2:
                    paint_util_push_tunnel_left(session, height - 32, TUNNEL_INVERTED_3);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            paint_util_set_general_support_height(session, height, 0x20);
            break;
    }
}

/** rct2: 0x008A6D20 */
static void lim_launched_rc_track_right_twist_down_to_up(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15770, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15794, { 0, 0, height }, { 32, 20, 0 },
                        { 0, 6, height + 28 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15773, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15797, { 0, 0, height }, { 32, 20, 0 },
                        { 0, 6, height + 28 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15776, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15800, { 0, 0, height }, { 32, 20, 0 },
                        { 0, 6, height + 28 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15779, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15803, { 0, 0, height }, { 32, 20, 0 },
                        { 0, 6, height + 28 });
                    break;
            }
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15771, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15795, { 0, 0, height }, { 32, 20, 0 },
                        { 0, 6, height + 28 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15774, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15798, { 0, 0, height }, { 32, 20, 0 },
                        { 0, 6, height + 28 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15777, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15801, { 0, 0, height }, { 32, 20, 0 },
                        { 0, 6, height + 28 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15780, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15804, { 0, 0, height }, { 32, 20, 0 },
                        { 0, 6, height + 28 });
                    break;
            }
            paint_util_set_segment_support_height(
                session,
                paint_util_rotate_segments(
                    SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15772, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15796, { 0, 0, height }, { 32, 20, 0 },
                        { 0, 6, height + 44 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15775, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15799, { 0, 0, height }, { 32, 20, 0 },
                        { 0, 6, height + 44 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15778, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15802, { 0, 0, height }, { 32, 20, 0 },
                        { 0, 6, height + 44 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15781, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15805, { 0, 0, height }, { 32, 20, 0 },
                        { 0, 6, height + 44 });
                    break;
            }
            switch (direction)
            {
                case 1:
                    paint_util_push_tunnel_right(session, height + 32, TUNNEL_INVERTED_3);
                    break;
                case 2:
                    paint_util_push_tunnel_left(session, height + 32, TUNNEL_INVERTED_3);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            paint_util_set_general_support_height(session, height, 0x20);
            break;
    }
}

/** rct2: 0x008A6D30 */
static void lim_launched_rc_track_left_twist_up_to_down(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15766, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15790, { 0, 0, height }, { 32, 20, 0 },
                        { 0, 6, height + 44 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15769, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15793, { 0, 0, height }, { 32, 20, 0 },
                        { 0, 6, height + 44 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15760, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15784, { 0, 0, height }, { 32, 20, 0 },
                        { 0, 6, height + 44 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15763, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15787, { 0, 0, height }, { 32, 20, 0 },
                        { 0, 6, height + 44 });
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height - 32, TUNNEL_INVERTED_3);
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            paint_util_set_general_support_height(session, height, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15765, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15789, { 0, 0, height }, { 32, 20, 0 },
                        { 0, 6, height + 28 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15768, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15792, { 0, 0, height }, { 32, 20, 0 },
                        { 0, 6, height + 28 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15759, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15783, { 0, 0, height }, { 32, 20, 0 },
                        { 0, 6, height + 28 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15762, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15786, { 0, 0, height }, { 32, 20, 0 },
                        { 0, 6, height + 28 });
                    break;
            }
            paint_util_set_segment_support_height(
                session,
                paint_util_rotate_segments(
                    SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15764, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15788, { 0, 0, height }, { 32, 20, 0 },
                        { 0, 6, height + 28 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15767, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15791, { 0, 0, height }, { 32, 20, 0 },
                        { 0, 6, height + 28 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15758, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15782, { 0, 0, height }, { 32, 20, 0 },
                        { 0, 6, height + 28 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15761, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15785, { 0, 0, height }, { 32, 20, 0 },
                        { 0, 6, height + 28 });
                    break;
            }
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            switch (direction)
            {
                case 1:
                    paint_util_push_tunnel_right(session, height, TUNNEL_0);
                    break;
                case 2:
                    paint_util_push_tunnel_left(session, height, TUNNEL_0);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008A6D40 */
static void lim_launched_rc_track_right_twist_up_to_down(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15778, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15802, { 0, 0, height }, { 32, 20, 0 },
                        { 0, 6, height + 44 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15781, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15805, { 0, 0, height }, { 32, 20, 0 },
                        { 0, 6, height + 44 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15772, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15796, { 0, 0, height }, { 32, 20, 0 },
                        { 0, 6, height + 44 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15775, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15799, { 0, 0, height }, { 32, 20, 0 },
                        { 0, 6, height + 44 });
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height + 32, TUNNEL_INVERTED_3);
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 64, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15777, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15801, { 0, 0, height }, { 32, 20, 0 },
                        { 0, 6, height + 28 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15780, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15804, { 0, 0, height }, { 32, 20, 0 },
                        { 0, 6, height + 28 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15771, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15795, { 0, 0, height }, { 32, 20, 0 },
                        { 0, 6, height + 28 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15774, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15798, { 0, 0, height }, { 32, 20, 0 },
                        { 0, 6, height + 28 });
                    break;
            }
            paint_util_set_segment_support_height(
                session,
                paint_util_rotate_segments(
                    SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15776, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15800, { 0, 0, height }, { 32, 20, 0 },
                        { 0, 6, height + 28 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15779, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15803, { 0, 0, height }, { 32, 20, 0 },
                        { 0, 6, height + 28 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15770, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15794, { 0, 0, height }, { 32, 20, 0 },
                        { 0, 6, height + 28 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15773, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15797, { 0, 0, height }, { 32, 20, 0 },
                        { 0, 6, height + 28 });
                    break;
            }
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            switch (direction)
            {
                case 1:
                    paint_util_push_tunnel_right(session, height, TUNNEL_0);
                    break;
                case 2:
                    paint_util_push_tunnel_left(session, height, TUNNEL_0);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008A6CD0 */
static void lim_launched_rc_track_left_corkscrew_up(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15734, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height + 4 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15737, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height + 4 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15740, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height + 4 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15743, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height + 4 });
                    break;
            }

            track_paint_util_left_corkscrew_up_supports(session, direction, height);

            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
            }
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15735, { 0, 0, height }, { 20, 20, 3 },
                        { 6, 6, height + 10 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15738, { 0, 0, height }, { 20, 20, 3 },
                        { 6, 6, height + 10 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15741, { 0, 0, height }, { 20, 20, 3 },
                        { 6, 6, height + 10 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15744, { 0, 0, height }, { 20, 20, 3 },
                        { 6, 6, height + 10 });
                    break;
            }
            paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 72, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15736, { 0, 0, height }, { 20, 32, 3 },
                        { 6, 0, height + 24 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15739, { 0, 0, height }, { 20, 32, 3 },
                        { 6, 0, height + 24 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15742, { 0, 0, height }, { 20, 32, 3 },
                        { 6, 0, height + 24 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15745, { 0, 0, height }, { 20, 32, 3 },
                        { 6, 0, height + 24 });
                    break;
            }

            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES, 4, 0, height + 35, session.TrackColours[SCHEME_SUPPORTS]);

            switch (direction)
            {
                case 2:
                    paint_util_push_tunnel_right(session, height + 8, TUNNEL_0);
                    break;
                case 3:
                    paint_util_push_tunnel_left(session, height + 8, TUNNEL_0);
                    break;
            }
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x008A6CE0 */
static void lim_launched_rc_track_right_corkscrew_up(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15746, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height + 4 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15749, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height + 4 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15752, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height + 4 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15755, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height + 4 });
                    break;
            }
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15747, { 0, 0, height }, { 20, 20, 3 },
                        { 6, 6, height + 10 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15750, { 0, 0, height }, { 20, 20, 3 },
                        { 6, 6, height + 10 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15753, { 0, 0, height }, { 20, 20, 3 },
                        { 6, 6, height + 10 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15756, { 0, 0, height }, { 20, 20, 3 },
                        { 6, 6, height + 10 });
                    break;
            }
            paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 72, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15748, { 0, 0, height }, { 20, 32, 3 },
                        { 6, 0, height + 24 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15751, { 0, 0, height }, { 20, 32, 3 },
                        { 6, 0, height + 24 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15754, { 0, 0, height }, { 20, 32, 3 },
                        { 6, 0, height + 24 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15757, { 0, 0, height }, { 20, 32, 3 },
                        { 6, 0, height + 24 });
                    break;
            }

            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES, 4, 0, height + 35, session.TrackColours[SCHEME_SUPPORTS]);

            switch (direction)
            {
                case 0:
                    paint_util_push_tunnel_right(session, height + 8, TUNNEL_0);
                    break;
                case 1:
                    paint_util_push_tunnel_left(session, height + 8, TUNNEL_0);
                    break;
            }
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x008A6CF0 */
static void lim_launched_rc_track_left_corkscrew_down(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    lim_launched_rc_track_right_corkscrew_up(session, ride, 2 - trackSequence, (direction + 1) & 3, height, trackElement);
}

/** rct2: 0x008A6D00 */
static void lim_launched_rc_track_right_corkscrew_down(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    lim_launched_rc_track_left_corkscrew_up(session, ride, 2 - trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x008A6D80 */
static void lim_launched_rc_track_brakes(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK] | 15018, { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            break;
        case 1:
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK] | 15019, { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            break;
    }
    if (track_paint_util_should_paint_supports(session.MapPosition))
    {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
    }
    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
    paint_util_set_segment_support_height(
        session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x008A6C10 */
static void lim_launched_rc_track_90_deg_up(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15702, { 0, 0, height }, { 2, 20, 31 },
                        { 4, 6, height + 8 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15703, { 0, 0, height }, { 2, 20, 31 },
                        { 24, 6, height + 8 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15704, { 0, 0, height }, { 2, 20, 31 },
                        { 24, 6, height + 8 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15705, { 0, 0, height }, { 2, 20, 31 },
                        { 4, 6, height + 8 });
                    break;
            }
            paint_util_set_vertical_tunnel(session, height + 32);
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 1:
            break;
    }
}

/** rct2: 0x008A6C20 */
static void lim_launched_rc_track_90_deg_down(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    lim_launched_rc_track_90_deg_up(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008A6C30 */
static void lim_launched_rc_track_60_deg_up_to_90_deg_up(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15694, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15695, { 0, 0, height }, { 2, 20, 55 },
                        { 24, 6, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15696, { 0, 0, height }, { 2, 20, 55 },
                        { 24, 6, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15697, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_1);
            }
            paint_util_set_vertical_tunnel(session, height + 56);
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 56, 0x20);
            break;
        case 1:
            break;
    }
}

/** rct2: 0x008A6C40 */
static void lim_launched_rc_track_90_deg_down_to_60_deg_down(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    lim_launched_rc_track_60_deg_up_to_90_deg_up(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008A6C50 */
static void lim_launched_rc_track_90_deg_up_to_60_deg_up(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK] | 15698, { 0, 0, height }, { 6, 20, 3 },
                { 0, 6, height + 2 });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK] | 15699, { 0, 0, height }, { 2, 20, 31 },
                { 39, 6, height + 8 });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK] | 15700, { 0, 0, height }, { 2, 20, 31 },
                { 39, 6, height + 8 });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK] | 15701, { 0, 0, height }, { 6, 20, 3 },
                { 0, 6, height + 2 });
            break;
    }
    switch (direction)
    {
        case 1:
            paint_util_push_tunnel_right(session, height + 48, TUNNEL_2);
            break;
        case 2:
            paint_util_push_tunnel_left(session, height + 48, TUNNEL_2);
            break;
    }
    paint_util_set_segment_support_height(
        session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 80, 0x20);
}

/** rct2: 0x008A6C60 */
static void lim_launched_rc_track_60_deg_down_to_90_deg_down(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15700, { 0, 0, height }, { 2, 20, 31 },
                        { 39, 6, height + 8 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15701, { 0, 0, height }, { 6, 20, 3 },
                        { 0, 6, height + 2 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15698, { 0, 0, height }, { 6, 20, 3 },
                        { 0, 6, height + 2 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15699, { 0, 0, height }, { 2, 20, 31 },
                        { 39, 6, height + 8 });
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height + 48, TUNNEL_2);
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 80, 0x20);
            break;
        case 1:
            break;
    }
}

/** rct2: 0x008A6C70 */
static void lim_launched_rc_track_90_deg_to_inverted_flat_quarter_loop_up(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15722, { 0, 0, height }, { 2, 20, 31 },
                        { 4, 6, height + 8 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15725, { 0, 0, height }, { 2, 20, 31 },
                        { 24, 6, height + 8 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15728, { 0, 0, height }, { 2, 20, 31 },
                        { 24, 6, height + 8 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15731, { 0, 0, height }, { 2, 20, 31 },
                        { 4, 6, height + 8 });
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 88, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15723, { 0, 0, height }, { 2, 20, 31 },
                        { -8, 6, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15726, { 0, 0, height }, { 2, 20, 63 },
                        { 24, 6, height + 8 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15729, { 0, 0, height }, { 2, 20, 63 },
                        { 24, 6, height + 8 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15732, { 0, 0, height }, { 2, 20, 31 },
                        { -8, 6, height });
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 64, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15724, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height + 24 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15727, { 0, 0, height }, { 2, 20, 31 },
                        { 24, 6, height + 8 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15730, { 0, 0, height }, { 2, 20, 31 },
                        { 24, 6, height + 8 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15733, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height + 24 });
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height + 16, TUNNEL_0);
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x008A6C80 */
static void lim_launched_rc_track_inverted_flat_to_90_deg_quarter_loop_down(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    lim_launched_rc_track_90_deg_to_inverted_flat_quarter_loop_up(
        session, ride, 2 - trackSequence, direction, height, trackElement);
}

/** rct2: 0x008A6D90 */
static void lim_launched_rc_track_block_brakes(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    bool isClosed = trackElement.BlockBrakeClosed();
    PaintAddImageAsParentRotated(
        session, direction, session.TrackColours[SCHEME_TRACK] | _LimLauncherBlockBrakeImages[direction][isClosed],
        { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });

    if (track_paint_util_should_paint_supports(session.MapPosition))
    {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
    }
    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
    paint_util_set_segment_support_height(
        session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

static void lim_launched_rc_track_left_quarter_turn_1_90_deg_up(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15706, { 0, 0, height }, { 2, 20, 63 },
                        { 4, 6, height + 8 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15707, { 0, 0, height }, { 2, 20, 63 },
                        { 4, 6, height + 8 });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15715, { 0, 0, height }, { 2, 20, 63 },
                        { 24, 6, height + 8 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15716, { 0, 0, height }, { 2, 2, 63 },
                        { 24, 24, height + 8 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15709, { 0, 0, height }, { 2, 20, 63 },
                        { 4, 6, height + 8 });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15717, { 0, 0, height }, { 2, 20, 63 },
                        { 24, 6, height + 8 });
                    break;
            }
            paint_util_set_vertical_tunnel(session, height + 96);
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 96, 0x20);
            break;
        case 1:
            break;
    }
}

/** rct2: 0x008A6CA0 */
static void lim_launched_rc_track_right_quarter_turn_1_90_deg_up(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15710, { 0, 0, height }, { 2, 20, 63 },
                        { 4, 6, height + 8 });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15718, { 0, 0, height }, { 2, 20, 63 },
                        { 24, 6, height + 8 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15719, { 0, 0, height }, { 2, 2, 63 },
                        { 24, 24, height + 8 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15712, { 0, 0, height }, { 2, 20, 63 },
                        { 4, 6, height + 8 });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15720, { 0, 0, height }, { 2, 20, 63 },
                        { 24, 6, height + 8 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 15713, { 0, 0, height }, { 2, 20, 63 },
                        { 4, 6, height + 8 });
                    break;
            }
            paint_util_set_vertical_tunnel(session, height + 96);
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 96, 0x20);
            break;
        case 1:
            break;
    }
}

/** rct2: 0x008A6CB0 */
static void lim_launched_rc_track_left_quarter_turn_1_90_deg_down(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    lim_launched_rc_track_right_quarter_turn_1_90_deg_up(
        session, ride, trackSequence, (direction + 1) & 3, height, trackElement);
}

/** rct2: 0x008A6CC0 */
static void lim_launched_rc_track_right_quarter_turn_1_90_deg_down(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    lim_launched_rc_track_left_quarter_turn_1_90_deg_up(
        session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

TRACK_PAINT_FUNCTION get_track_paint_function_lim_launched_rc(int32_t trackType)
{
    switch (trackType)
    {
        // Use Looping Roller Coaster
        case TrackElemType::Flat:
        case TrackElemType::Up25:
        case TrackElemType::Up60:
        case TrackElemType::FlatToUp25:
        case TrackElemType::Up25ToUp60:
        case TrackElemType::Up60ToUp25:
        case TrackElemType::Up25ToFlat:
        case TrackElemType::Down25:
        case TrackElemType::Down60:
        case TrackElemType::FlatToDown25:
        case TrackElemType::Down25ToDown60:
        case TrackElemType::Down60ToDown25:
        case TrackElemType::Down25ToFlat:
        case TrackElemType::LeftQuarterTurn5Tiles:
        case TrackElemType::RightQuarterTurn5Tiles:
        case TrackElemType::FlatToLeftBank:
        case TrackElemType::FlatToRightBank:
        case TrackElemType::LeftBankToFlat:
        case TrackElemType::RightBankToFlat:
        case TrackElemType::BankedLeftQuarterTurn5Tiles:
        case TrackElemType::BankedRightQuarterTurn5Tiles:
        case TrackElemType::LeftBankToUp25:
        case TrackElemType::RightBankToUp25:
        case TrackElemType::Up25ToLeftBank:
        case TrackElemType::Up25ToRightBank:
        case TrackElemType::LeftBankToDown25:
        case TrackElemType::RightBankToDown25:
        case TrackElemType::Down25ToLeftBank:
        case TrackElemType::Down25ToRightBank:
        case TrackElemType::LeftBank:
        case TrackElemType::RightBank:
        case TrackElemType::LeftQuarterTurn5TilesUp25:
        case TrackElemType::RightQuarterTurn5TilesUp25:
        case TrackElemType::LeftQuarterTurn5TilesDown25:
        case TrackElemType::RightQuarterTurn5TilesDown25:
        case TrackElemType::SBendLeft:
        case TrackElemType::SBendRight:
        case TrackElemType::LeftQuarterTurn3Tiles:
        case TrackElemType::RightQuarterTurn3Tiles:
        case TrackElemType::LeftBankedQuarterTurn3Tiles:
        case TrackElemType::RightBankedQuarterTurn3Tiles:
        case TrackElemType::LeftQuarterTurn3TilesUp25:
        case TrackElemType::RightQuarterTurn3TilesUp25:
        case TrackElemType::LeftQuarterTurn3TilesDown25:
        case TrackElemType::RightQuarterTurn3TilesDown25:
        case TrackElemType::LeftHalfBankedHelixUpSmall:
        case TrackElemType::RightHalfBankedHelixUpSmall:
        case TrackElemType::LeftHalfBankedHelixDownSmall:
        case TrackElemType::RightHalfBankedHelixDownSmall:
        case TrackElemType::LeftHalfBankedHelixUpLarge:
        case TrackElemType::RightHalfBankedHelixUpLarge:
        case TrackElemType::LeftHalfBankedHelixDownLarge:
        case TrackElemType::RightHalfBankedHelixDownLarge:
        case TrackElemType::LeftQuarterTurn1TileUp60:
        case TrackElemType::RightQuarterTurn1TileUp60:
        case TrackElemType::LeftQuarterTurn1TileDown60:
        case TrackElemType::RightQuarterTurn1TileDown60:
        case TrackElemType::Up25LeftBanked:
        case TrackElemType::Up25RightBanked:
        case TrackElemType::OnRidePhoto:
        case TrackElemType::Down25LeftBanked:
        case TrackElemType::Down25RightBanked:
        case TrackElemType::LeftEighthToDiag:
        case TrackElemType::RightEighthToDiag:
        case TrackElemType::LeftEighthToOrthogonal:
        case TrackElemType::RightEighthToOrthogonal:
        case TrackElemType::LeftEighthBankToDiag:
        case TrackElemType::RightEighthBankToDiag:
        case TrackElemType::LeftEighthBankToOrthogonal:
        case TrackElemType::RightEighthBankToOrthogonal:
        case TrackElemType::DiagFlat:
        case TrackElemType::DiagUp25:
        case TrackElemType::DiagUp60:
        case TrackElemType::DiagFlatToUp25:
        case TrackElemType::DiagUp25ToUp60:
        case TrackElemType::DiagUp60ToUp25:
        case TrackElemType::DiagUp25ToFlat:
        case TrackElemType::DiagDown25:
        case TrackElemType::DiagDown60:
        case TrackElemType::DiagFlatToDown25:
        case TrackElemType::DiagDown25ToDown60:
        case TrackElemType::DiagDown60ToDown25:
        case TrackElemType::DiagDown25ToFlat:
        case TrackElemType::DiagFlatToLeftBank:
        case TrackElemType::DiagFlatToRightBank:
        case TrackElemType::DiagLeftBankToFlat:
        case TrackElemType::DiagRightBankToFlat:
        case TrackElemType::DiagLeftBankToUp25:
        case TrackElemType::DiagRightBankToUp25:
        case TrackElemType::DiagUp25ToLeftBank:
        case TrackElemType::DiagUp25ToRightBank:
        case TrackElemType::DiagLeftBankToDown25:
        case TrackElemType::DiagRightBankToDown25:
        case TrackElemType::DiagDown25ToLeftBank:
        case TrackElemType::DiagDown25ToRightBank:
        case TrackElemType::DiagLeftBank:
        case TrackElemType::DiagRightBank:
        case TrackElemType::LeftBankedQuarterTurn3TileUp25:
        case TrackElemType::RightBankedQuarterTurn3TileUp25:
        case TrackElemType::LeftBankedQuarterTurn3TileDown25:
        case TrackElemType::RightBankedQuarterTurn3TileDown25:
        case TrackElemType::LeftBankedQuarterTurn5TileUp25:
        case TrackElemType::RightBankedQuarterTurn5TileUp25:
        case TrackElemType::LeftBankedQuarterTurn5TileDown25:
        case TrackElemType::RightBankedQuarterTurn5TileDown25:
        case TrackElemType::Up25ToLeftBankedUp25:
        case TrackElemType::Up25ToRightBankedUp25:
        case TrackElemType::LeftBankedUp25ToUp25:
        case TrackElemType::RightBankedUp25ToUp25:
        case TrackElemType::Down25ToLeftBankedDown25:
        case TrackElemType::Down25ToRightBankedDown25:
        case TrackElemType::LeftBankedDown25ToDown25:
        case TrackElemType::RightBankedDown25ToDown25:
        case TrackElemType::LeftBankedFlatToLeftBankedUp25:
        case TrackElemType::RightBankedFlatToRightBankedUp25:
        case TrackElemType::LeftBankedUp25ToLeftBankedFlat:
        case TrackElemType::RightBankedUp25ToRightBankedFlat:
        case TrackElemType::LeftBankedFlatToLeftBankedDown25:
        case TrackElemType::RightBankedFlatToRightBankedDown25:
        case TrackElemType::LeftBankedDown25ToLeftBankedFlat:
        case TrackElemType::RightBankedDown25ToRightBankedFlat:
        case TrackElemType::FlatToLeftBankedUp25:
        case TrackElemType::FlatToRightBankedUp25:
        case TrackElemType::LeftBankedUp25ToFlat:
        case TrackElemType::RightBankedUp25ToFlat:
        case TrackElemType::FlatToLeftBankedDown25:
        case TrackElemType::FlatToRightBankedDown25:
        case TrackElemType::LeftBankedDown25ToFlat:
        case TrackElemType::RightBankedDown25ToFlat:
            return get_track_paint_function_looping_rc(trackType);

        case TrackElemType::EndStation:
        case TrackElemType::BeginStation:
        case TrackElemType::MiddleStation:
            return lim_launched_rc_track_station;
        case TrackElemType::LeftVerticalLoop:
            return lim_launched_rc_track_left_vertical_loop;
        case TrackElemType::RightVerticalLoop:
            return lim_launched_rc_track_right_vertical_loop;
        case TrackElemType::LeftTwistDownToUp:
            return lim_launched_rc_track_left_twist_down_to_up;
        case TrackElemType::RightTwistDownToUp:
            return lim_launched_rc_track_right_twist_down_to_up;
        case TrackElemType::LeftTwistUpToDown:
            return lim_launched_rc_track_left_twist_up_to_down;
        case TrackElemType::RightTwistUpToDown:
            return lim_launched_rc_track_right_twist_up_to_down;
        case TrackElemType::LeftCorkscrewUp:
            return lim_launched_rc_track_left_corkscrew_up;
        case TrackElemType::RightCorkscrewUp:
            return lim_launched_rc_track_right_corkscrew_up;
        case TrackElemType::LeftCorkscrewDown:
            return lim_launched_rc_track_left_corkscrew_down;
        case TrackElemType::RightCorkscrewDown:
            return lim_launched_rc_track_right_corkscrew_down;
        case TrackElemType::Brakes:
            return lim_launched_rc_track_brakes;
        case TrackElemType::Up90:
            return lim_launched_rc_track_90_deg_up;
        case TrackElemType::Down90:
            return lim_launched_rc_track_90_deg_down;
        case TrackElemType::Up60ToUp90:
            return lim_launched_rc_track_60_deg_up_to_90_deg_up;
        case TrackElemType::Down90ToDown60:
            return lim_launched_rc_track_90_deg_down_to_60_deg_down;
        case TrackElemType::Up90ToUp60:
            return lim_launched_rc_track_90_deg_up_to_60_deg_up;
        case TrackElemType::Down60ToDown90:
            return lim_launched_rc_track_60_deg_down_to_90_deg_down;
        case TrackElemType::Up90ToInvertedFlatQuarterLoop:
            return lim_launched_rc_track_90_deg_to_inverted_flat_quarter_loop_up;
        case TrackElemType::InvertedFlatToDown90QuarterLoop:
            return lim_launched_rc_track_inverted_flat_to_90_deg_quarter_loop_down;
        case TrackElemType::BlockBrakes:
            return lim_launched_rc_track_block_brakes;
        case TrackElemType::LeftQuarterTurn1TileUp90:
            return lim_launched_rc_track_left_quarter_turn_1_90_deg_up;
        case TrackElemType::RightQuarterTurn1TileUp90:
            return lim_launched_rc_track_right_quarter_turn_1_90_deg_up;
        case TrackElemType::LeftQuarterTurn1TileDown90:
            return lim_launched_rc_track_left_quarter_turn_1_90_deg_down;
        case TrackElemType::RightQuarterTurn1TileDown90:
            return lim_launched_rc_track_right_quarter_turn_1_90_deg_down;
    }
    return nullptr;
}
