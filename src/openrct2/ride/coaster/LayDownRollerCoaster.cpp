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

/** rct2: 0x0082491C */
static void lay_down_rc_track_flat(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        if (trackElement.HasChain())
        {
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16226, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16227, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16228, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16229, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
            }
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
        }
        else
        {
            switch (direction)
            {
                case 0:
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16224, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
                case 1:
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16225, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
            }
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
        }
        paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
    }
    else
    {
        if (trackElement.HasChain())
        {
            switch (direction)
            {
                case 0:
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 26557, { 0, 0, height + 24 }, { 32, 20, 1 },
                        { 0, 6, height + 22 });
                    break;
                case 1:
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 26558, { 0, 0, height + 24 }, { 32, 20, 1 },
                        { 0, 6, height + 22 });
                    break;
            }
        }
        else
        {
            switch (direction)
            {
                case 0:
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 26555, { 0, 0, height + 24 }, { 32, 20, 1 },
                        { 0, 6, height + 22 });
                    break;
                case 1:
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 26556, { 0, 0, height + 24 }, { 32, 20, 1 },
                        { 0, 6, height + 22 });
                    break;
            }
        }

        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        if (track_paint_util_should_paint_supports(session.MapPosition))
        {
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
        }

        paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
    }
}

/** rct2: 0x00824B8C, 0x00824B9C, 0x00824BAC */
static void lay_down_rc_track_station(
    paint_session& session, const Ride& ride, [[maybe_unused]] uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackElement.IsInverted())
    {
        static constexpr const uint32_t imageIds[4][3] = {
            { SPR_STATION_BASE_C_SW_NE, 26557, SPR_STATION_INVERTED_BAR_A_SW_NE },
            { SPR_STATION_BASE_C_NW_SE, 26558, SPR_STATION_INVERTED_BAR_A_NW_SE },
            { SPR_STATION_BASE_C_SW_NE, 26557, SPR_STATION_INVERTED_BAR_A_SW_NE },
            { SPR_STATION_BASE_C_NW_SE, 26558, SPR_STATION_INVERTED_BAR_A_NW_SE },
        };

        PaintAddImageAsParentRotated(
            session, direction, imageIds[direction][0] | session.TrackColours[SCHEME_MISC], { 0, 0, height }, { 32, 28, 1 },
            { 0, 2, height });
        PaintAddImageAsParentRotated(
            session, direction, imageIds[direction][1] | session.TrackColours[SCHEME_TRACK], { 0, 0, height + 24 },
            { 32, 20, 3 }, { 0, 6, height + 24 });
        PaintAddImageAsChildRotated(
            session, direction, imageIds[direction][2] | session.TrackColours[SCHEME_SUPPORTS], 0, 6, 32, 20, 1, height + 24, 0,
            6, height + 24);
        track_paint_util_draw_station_metal_supports_2(session, direction, height, session.TrackColours[SCHEME_SUPPORTS], 11);
        track_paint_util_draw_station_inverted(session, ride, direction, height, trackElement, STATION_VARIANT_1);
        paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_INVERTED_9);
    }
    else
    {
        static constexpr const uint32_t imageIds[4][3] = {
            { 16236, 16232, SPR_STATION_BASE_A_SW_NE },
            { 16237, 16233, SPR_STATION_BASE_A_NW_SE },
            { 16236, 16232, SPR_STATION_BASE_A_SW_NE },
            { 16237, 16233, SPR_STATION_BASE_A_NW_SE },
        };

        if (trackElement.GetTrackType() == TrackElemType::EndStation)
        {
            PaintAddImageAsParentRotated(
                session, direction, imageIds[direction][1] | session.TrackColours[SCHEME_TRACK], { 0, 0, height },
                { 32, 20, 1 }, { 0, 6, height + 3 });
        }
        else
        {
            PaintAddImageAsParentRotated(
                session, direction, imageIds[direction][0] | session.TrackColours[SCHEME_TRACK], { 0, 0, height },
                { 32, 20, 1 }, { 0, 6, height + 3 });
        }
        PaintAddImageAsParentRotated(
            session, direction, imageIds[direction][2] | session.TrackColours[SCHEME_MISC], { 0, 0, height }, { 32, 32, 1 });
        track_paint_util_draw_station_metal_supports_2(session, direction, height, session.TrackColours[SCHEME_SUPPORTS], 11);
        track_paint_util_draw_station_2(session, ride, direction, height, trackElement, 9, 11);
        paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
    }
    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x0082492C */
static void lay_down_rc_track_25_deg_up(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        if (trackElement.HasChain())
        {
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16314, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16315, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16316, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16317, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
            }
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
        }
        else
        {
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16286, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16287, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16288, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16289, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
            }
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
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
    else
    {
        if (trackElement.HasChain())
        {
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 26621, { 0, 0, height + 24 }, { 32, 20, 3 },
                        { 0, 6, height + 40 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 26622, { 0, 0, height + 24 }, { 32, 20, 3 },
                        { 0, 6, height + 40 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 26623, { 0, 0, height + 24 }, { 32, 20, 3 },
                        { 0, 6, height + 40 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 26624, { 0, 0, height + 24 }, { 32, 20, 3 },
                        { 0, 6, height + 40 });
                    break;
            }
        }
        else
        {
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 26569, { 0, 0, height + 24 }, { 32, 20, 3 },
                        { 0, 6, height + 40 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 26570, { 0, 0, height + 24 }, { 32, 20, 3 },
                        { 0, 6, height + 40 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 26571, { 0, 0, height + 24 }, { 32, 20, 3 },
                        { 0, 6, height + 40 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 26572, { 0, 0, height + 24 }, { 32, 20, 3 },
                        { 0, 6, height + 40 });
                    break;
            }
        }

        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        if (track_paint_util_should_paint_supports(session.MapPosition))
        {
            switch (direction)
            {
                case 0:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 6, 0, height + 51, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height + 51, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 7, 0, height + 51, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height + 51, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
        }

        if (direction == 0 || direction == 3)
        {
            paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_1);
        }
        else
        {
            paint_util_push_tunnel_rotated(session, direction, height + 8, TUNNEL_2);
        }
        paint_util_set_general_support_height(session, height + 56, 0x20);
    }
}

/** rct2: 0x0082493C */
static void lay_down_rc_track_60_deg_up(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        if (trackElement.HasChain())
        {
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16330, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16331, { 0, 0, height }, { 32, 1, 98 },
                        { 0, 27, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16332, { 0, 0, height }, { 32, 1, 98 },
                        { 0, 27, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16333, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
            }
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 32, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
        }
        else
        {
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16302, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16303, { 0, 0, height }, { 32, 1, 98 },
                        { 0, 27, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16304, { 0, 0, height }, { 32, 1, 98 },
                        { 0, 27, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16305, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
            }
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 32, height, session.TrackColours[SCHEME_SUPPORTS]);
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
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 26585, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 88 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 26586, { 0, 0, height + 24 }, { 32, 2, 81 },
                    { 0, 4, height + 6 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 26587, { 0, 0, height + 24 }, { 32, 2, 81 },
                    { 0, 4, height + 6 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 26588, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 88 });
                break;
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
}

/** rct2: 0x0082494C */
static void lay_down_rc_track_flat_to_25_deg_up(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        if (trackElement.HasChain())
        {
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16306, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16307, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16308, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16309, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
            }
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 3, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
        }
        else
        {
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16278, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16279, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16280, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16281, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
            }
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 3, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
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
    else
    {
        if (trackElement.HasChain())
        {
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 26613, { 0, 0, height + 24 }, { 32, 20, 3 },
                        { 0, 6, height + 32 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 26614, { 0, 0, height + 24 }, { 32, 20, 3 },
                        { 0, 6, height + 32 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 26615, { 0, 0, height + 24 }, { 32, 20, 3 },
                        { 0, 6, height + 32 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 26616, { 0, 0, height + 24 }, { 32, 20, 3 },
                        { 0, 6, height + 32 });
                    break;
            }
        }
        else
        {
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 26561, { 0, 0, height + 24 }, { 32, 20, 3 },
                        { 0, 6, height + 32 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 26562, { 0, 0, height + 24 }, { 32, 20, 3 },
                        { 0, 6, height + 32 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 26563, { 0, 0, height + 24 }, { 32, 20, 3 },
                        { 0, 6, height + 32 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 26564, { 0, 0, height + 24 }, { 32, 20, 3 },
                        { 0, 6, height + 32 });
                    break;
            }
        }

        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        if (track_paint_util_should_paint_supports(session.MapPosition))
        {
            switch (direction)
            {
                case 0:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 6, 0, height + 43, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height + 43, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 7, 0, height + 43, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height + 43, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
        }

        if (direction == 0 || direction == 3)
        {
            paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
        }
        else
        {
            paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_2);
        }
        paint_util_set_general_support_height(session, height + 48, 0x20);
    }
}

/** rct2: 0x0082495C */
static void lay_down_rc_track_25_deg_up_to_60_deg_up(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        if (trackElement.HasChain())
        {
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16318, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16319, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16322, { 0, 0, height }, { 32, 1, 66 },
                        { 0, 27, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16320, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16323, { 0, 0, height }, { 32, 1, 66 },
                        { 0, 27, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16321, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
            }
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 12, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
        }
        else
        {
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16290, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16291, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16294, { 0, 0, height }, { 32, 1, 66 },
                        { 0, 27, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16292, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16295, { 0, 0, height }, { 32, 1, 66 },
                        { 0, 27, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16293, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
            }
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 12, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
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
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 26573, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 56 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 26577, { 0, 0, height + 24 }, { 32, 10, 49 },
                    { 0, 10, height + 6 });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 26574, { 0, 0, height + 24 }, { 32, 2, 49 },
                    { 0, 4, height + 6 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 26578, { 0, 0, height + 24 }, { 32, 10, 49 },
                    { 0, 10, height + 6 });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 26575, { 0, 0, height + 24 }, { 32, 2, 49 },
                    { 0, 4, height + 6 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 26576, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 56 });
                break;
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
}

/** rct2: 0x0082496C */
static void lay_down_rc_track_60_deg_up_to_25_deg_up(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        if (trackElement.HasChain())
        {
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16324, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16325, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16328, { 0, 0, height }, { 32, 1, 66 },
                        { 0, 27, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16326, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16329, { 0, 0, height }, { 32, 1, 66 },
                        { 0, 27, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16327, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
            }
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 20, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
        }
        else
        {
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16296, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16297, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16300, { 0, 0, height }, { 32, 1, 66 },
                        { 0, 27, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16298, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16301, { 0, 0, height }, { 32, 1, 66 },
                        { 0, 27, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16299, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
            }
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 20, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
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
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 26579, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 56 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 26583, { 0, 0, height + 24 }, { 32, 10, 49 },
                    { 0, 10, height + 6 });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 26580, { 0, 0, height + 24 }, { 32, 2, 49 },
                    { 0, 4, height + 6 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 26584, { 0, 0, height + 24 }, { 32, 10, 49 },
                    { 0, 10, height + 6 });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 26581, { 0, 0, height + 24 }, { 32, 2, 49 },
                    { 0, 4, height + 6 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 26582, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 56 });
                break;
        }

        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        if (track_paint_util_should_paint_supports(session.MapPosition))
        {
            switch (direction)
            {
                case 0:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 6, 0, height + 65, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height + 65, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 7, 0, height + 65, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height + 65, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
        }

        if (direction == 0 || direction == 3)
        {
            paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_1);
        }
        else
        {
            paint_util_push_tunnel_rotated(session, direction, height + 24, TUNNEL_2);
        }
        paint_util_set_general_support_height(session, height + 72, 0x20);
    }
}

/** rct2: 0x0082497C */
static void lay_down_rc_track_25_deg_up_to_flat(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        if (trackElement.HasChain())
        {
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16310, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16311, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16312, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16313, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
            }
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
        }
        else
        {
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16282, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16283, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16284, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16285, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
            }
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
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
    else
    {
        if (trackElement.HasChain())
        {
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 26617, { 0, 0, height + 24 }, { 32, 20, 3 },
                        { 0, 6, height + 32 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 26618, { 0, 0, height + 24 }, { 32, 20, 3 },
                        { 0, 6, height + 32 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 26619, { 0, 0, height + 24 }, { 32, 20, 3 },
                        { 0, 6, height + 32 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 26620, { 0, 0, height + 24 }, { 32, 20, 3 },
                        { 0, 6, height + 32 });
                    break;
            }
        }
        else
        {
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 26565, { 0, 0, height + 24 }, { 32, 20, 3 },
                        { 0, 6, height + 32 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 26566, { 0, 0, height + 24 }, { 32, 20, 3 },
                        { 0, 6, height + 32 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 26567, { 0, 0, height + 24 }, { 32, 20, 3 },
                        { 0, 6, height + 32 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 26568, { 0, 0, height + 24 }, { 32, 20, 3 },
                        { 0, 6, height + 32 });
                    break;
            }
        }

        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        if (track_paint_util_should_paint_supports(session.MapPosition))
        {
            switch (direction)
            {
                case 0:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 6, 0, height + 41, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height + 41, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 7, 0, height + 41, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height + 41, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
        }

        if (direction == 0 || direction == 3)
        {
            paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_0);
        }
        else
        {
            paint_util_push_tunnel_rotated(session, direction, height + 8, TUNNEL_12);
        }
        paint_util_set_general_support_height(session, height + 40, 0x20);
    }
}

/** rct2: 0x0082498C */
static void lay_down_rc_track_25_deg_down(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    lay_down_rc_track_25_deg_up(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x0082499C */
static void lay_down_rc_track_60_deg_down(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    lay_down_rc_track_60_deg_up(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008249AC */
static void lay_down_rc_track_flat_to_25_deg_down(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    lay_down_rc_track_25_deg_up_to_flat(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008249BC */
static void lay_down_rc_track_25_deg_down_to_60_deg_down(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    lay_down_rc_track_60_deg_up_to_25_deg_up(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008249CC */
static void lay_down_rc_track_60_deg_down_to_25_deg_down(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    lay_down_rc_track_25_deg_up_to_60_deg_up(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008249DC */
static void lay_down_rc_track_25_deg_down_to_flat(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    lay_down_rc_track_flat_to_25_deg_up(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008249EC */
static void lay_down_rc_track_left_quarter_turn_5(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16343, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16348, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16353, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16338, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                }
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                if (direction == 0 || direction == 3)
                {
                    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 1:
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16342, { 0, 0, height }, { 32, 16, 3 },
                            { 0, 0, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16347, { 0, 0, height }, { 32, 16, 3 },
                            { 0, 0, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16352, { 0, 0, height }, { 32, 16, 3 },
                            { 0, 16, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16337, { 0, 0, height }, { 32, 16, 3 },
                            { 0, 16, height });
                        break;
                }
                paint_util_set_segment_support_height(
                    session,
                    paint_util_rotate_segments(
                        SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                    0xFFFF, 0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16341, { 0, 0, height }, { 16, 16, 3 },
                            { 0, 16, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16346, { 0, 0, height }, { 16, 16, 3 },
                            { 16, 16, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16351, { 0, 0, height }, { 16, 16, 3 },
                            { 16, 0, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16336, { 0, 0, height }, { 16, 16, 3 },
                            { 0, 0, height });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 4:
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 5:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16340, { 0, 0, height }, { 16, 32, 3 },
                            { 16, 0, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16345, { 0, 0, height }, { 16, 32, 3 },
                            { 0, 0, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16350, { 0, 0, height }, { 16, 32, 3 },
                            { 0, 0, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16335, { 0, 0, height }, { 16, 32, 3 },
                            { 16, 0, height });
                        break;
                }
                paint_util_set_segment_support_height(
                    session,
                    paint_util_rotate_segments(
                        SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 6:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16339, { 0, 0, height }, { 20, 32, 3 },
                            { 6, 0, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16344, { 0, 0, height }, { 20, 32, 3 },
                            { 6, 0, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16349, { 0, 0, height }, { 20, 32, 3 },
                            { 6, 0, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16334, { 0, 0, height }, { 20, 32, 3 },
                            { 6, 0, height });
                        break;
                }
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                switch (direction)
                {
                    case 2:
                        paint_util_push_tunnel_right(session, height, TUNNEL_0);
                        break;
                    case 3:
                        paint_util_push_tunnel_left(session, height, TUNNEL_0);
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
        }
    }
    else
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26694, { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26699, { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26704, { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26689, { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                }

                paint_util_set_segment_support_height(
                    session,
                    paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);

                if (direction == 0 || direction == 3)
                {
                    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
                }
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 1:
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26693, { 0, 0, height + 24 },
                            { 32, 16, 3 }, { 0, 0, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26698, { 0, 0, height + 24 },
                            { 32, 16, 3 }, { 0, 0, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26703, { 0, 0, height + 24 },
                            { 32, 16, 3 }, { 0, 16, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26688, { 0, 0, height + 24 },
                            { 32, 16, 3 }, { 0, 16, height + 22 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session,
                    paint_util_rotate_segments(
                        SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                    0xFFFF, 0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26692, { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 0, 16, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26697, { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 16, 16, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26702, { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 16, 0, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26687, { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 0, 0, height + 22 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 4:
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 5:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26691, { 0, 0, height + 24 },
                            { 16, 32, 3 }, { 16, 0, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26696, { 0, 0, height + 24 },
                            { 16, 32, 3 }, { 0, 0, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26701, { 0, 0, height + 24 },
                            { 16, 32, 3 }, { 0, 0, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26686, { 0, 0, height + 24 },
                            { 16, 32, 3 }, { 16, 0, height + 22 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session,
                    paint_util_rotate_segments(
                        SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 6:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26690, { 0, 0, height + 24 },
                            { 20, 32, 3 }, { 6, 0, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26695, { 0, 0, height + 24 },
                            { 20, 32, 3 }, { 6, 0, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26700, { 0, 0, height + 24 },
                            { 20, 32, 3 }, { 6, 0, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26685, { 0, 0, height + 24 },
                            { 20, 32, 3 }, { 6, 0, height + 22 });
                        break;
                }

                paint_util_set_segment_support_height(
                    session,
                    paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D4, direction),
                    0xFFFF, 0);
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);

                switch (direction)
                {
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
}

/** rct2: 0x008249FC */
static void lay_down_rc_track_right_quarter_turn_5(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    lay_down_rc_track_left_quarter_turn_5(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x00824A0C */
static void lay_down_rc_track_flat_to_left_bank(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 16238, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 16246, { 0, 0, height }, { 32, 1, 26 },
                    { 0, 27, height });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 16239, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 16247, { 0, 0, height }, { 32, 1, 26 },
                    { 0, 27, height });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 16240, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 16241, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
        }
        if (track_paint_util_should_paint_supports(session.MapPosition))
        {
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
        }
        paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 26657, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 22 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 26658, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 22 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 26659, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 22 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 26660, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 22 });
                break;
        }

        paint_util_set_segment_support_height(
            session,
            paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
            0xFFFF, 0);
        if (track_paint_util_should_paint_supports(session.MapPosition))
        {
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
        }

        paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
    }
}

/** rct2: 0x00824A1C */
static void lay_down_rc_track_flat_to_right_bank(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 16242, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 16243, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 16244, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 16248, { 0, 0, height }, { 32, 1, 26 },
                    { 0, 27, height });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 16245, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 16249, { 0, 0, height }, { 32, 1, 26 },
                    { 0, 27, height });
                break;
        }
        if (track_paint_util_should_paint_supports(session.MapPosition))
        {
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
        }
        paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 26661, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 22 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 26662, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 22 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 26663, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 22 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 26664, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 22 });
                break;
        }

        paint_util_set_segment_support_height(
            session,
            paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
            0xFFFF, 0);
        if (track_paint_util_should_paint_supports(session.MapPosition))
        {
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
        }

        paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
    }
}

/** rct2: 0x00824A2C */
static void lay_down_rc_track_left_bank_to_flat(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 16244, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 16248, { 0, 0, height }, { 32, 1, 26 },
                    { 0, 27, height });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 16245, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 16249, { 0, 0, height }, { 32, 1, 26 },
                    { 0, 27, height });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 16242, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 16243, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
        }
        if (track_paint_util_should_paint_supports(session.MapPosition))
        {
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
        }
        paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 26663, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 22 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 26664, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 22 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 26661, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 22 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 26662, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 22 });
                break;
        }

        paint_util_set_segment_support_height(
            session,
            paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
            0xFFFF, 0);
        if (track_paint_util_should_paint_supports(session.MapPosition))
        {
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
        }

        paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
    }
}

/** rct2: 0x00824A3C */
static void lay_down_rc_track_right_bank_to_flat(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 16240, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 16241, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 16238, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 16246, { 0, 0, height }, { 32, 1, 26 },
                    { 0, 27, height });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 16239, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 16247, { 0, 0, height }, { 32, 1, 26 },
                    { 0, 27, height });
                break;
        }
        if (track_paint_util_should_paint_supports(session.MapPosition))
        {
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
        }
        paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 26659, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 22 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 26660, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 22 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 26657, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 22 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 26658, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 22 });
                break;
        }

        paint_util_set_segment_support_height(
            session,
            paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
            0xFFFF, 0);
        if (track_paint_util_should_paint_supports(session.MapPosition))
        {
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
        }

        paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
    }
}

/** rct2: 0x00824A4C */
static void lay_down_rc_track_banked_left_quarter_turn_5(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16363, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16374, { 0, 0, height }, { 32, 1, 26 },
                            { 0, 27, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16368, { 0, 0, height }, { 32, 1, 26 },
                            { 0, 27, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16373, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16358, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                }
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                if (direction == 0 || direction == 3)
                {
                    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 1:
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16362, { 0, 0, height }, { 32, 16, 3 },
                            { 0, 0, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16367, { 0, 0, height }, { 32, 16, 1 },
                            { 0, 0, height + 27 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16372, { 0, 0, height }, { 32, 16, 3 },
                            { 0, 16, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16357, { 0, 0, height }, { 32, 16, 3 },
                            { 0, 16, height });
                        break;
                }
                paint_util_set_segment_support_height(
                    session,
                    paint_util_rotate_segments(
                        SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                    0xFFFF, 0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16361, { 0, 0, height }, { 16, 16, 3 },
                            { 0, 16, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16366, { 0, 0, height }, { 16, 16, 1 },
                            { 16, 16, height + 27 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16371, { 0, 0, height }, { 16, 16, 3 },
                            { 16, 0, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16356, { 0, 0, height }, { 16, 16, 3 },
                            { 0, 0, height });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 4:
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 5:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16360, { 0, 0, height }, { 16, 32, 3 },
                            { 16, 0, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16365, { 0, 0, height }, { 16, 32, 1 },
                            { 0, 0, height + 27 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16370, { 0, 0, height }, { 16, 32, 3 },
                            { 0, 0, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16355, { 0, 0, height }, { 16, 32, 3 },
                            { 16, 0, height });
                        break;
                }
                paint_util_set_segment_support_height(
                    session,
                    paint_util_rotate_segments(
                        SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 6:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16359, { 0, 0, height }, { 20, 32, 3 },
                            { 6, 0, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16364, { 0, 0, height }, { 1, 32, 26 },
                            { 27, 0, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16369, { 0, 0, height }, { 20, 32, 3 },
                            { 6, 0, height });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16375, { 0, 0, height }, { 1, 32, 26 },
                            { 27, 0, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16354, { 0, 0, height }, { 20, 32, 3 },
                            { 6, 0, height });
                        break;
                }
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                switch (direction)
                {
                    case 2:
                        paint_util_push_tunnel_right(session, height, TUNNEL_0);
                        break;
                    case 3:
                        paint_util_push_tunnel_left(session, height, TUNNEL_0);
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
        }
    }
    else
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26738, { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 24 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26743, { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 24 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26748, { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 24 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26733, { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 24 });
                        break;
                }

                paint_util_set_segment_support_height(
                    session,
                    paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);

                if (direction == 0 || direction == 3)
                {
                    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
                }
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 1:
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26737, { 0, 0, height + 24 },
                            { 32, 16, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26742, { 0, 0, height + 24 },
                            { 32, 16, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26747, { 0, 0, height + 24 },
                            { 32, 16, 3 }, { 0, 16, height + 24 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26732, { 0, 0, height + 24 },
                            { 32, 16, 3 }, { 0, 16, height + 24 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session,
                    paint_util_rotate_segments(
                        SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                    0xFFFF, 0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26736, { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 0, 16, height + 24 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26741, { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 16, 16, height + 24 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26746, { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 16, 0, height + 24 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26731, { 0, 0, height + 24 },
                            { 16, 16, 3 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 4:
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 5:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26735, { 0, 0, height + 24 },
                            { 16, 32, 3 }, { 16, 0, height + 24 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26740, { 0, 0, height + 24 },
                            { 16, 32, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26745, { 0, 0, height + 24 },
                            { 16, 32, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26730, { 0, 0, height + 24 },
                            { 16, 32, 3 }, { 16, 0, height + 24 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session,
                    paint_util_rotate_segments(
                        SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 6:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26734, { 0, 0, height + 24 },
                            { 20, 32, 3 }, { 6, 0, height + 24 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26739, { 0, 0, height + 24 },
                            { 20, 32, 3 }, { 6, 0, height + 24 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26744, { 0, 0, height + 24 },
                            { 20, 32, 3 }, { 6, 0, height + 24 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26729, { 0, 0, height + 24 },
                            { 20, 32, 3 }, { 6, 0, height + 24 });
                        break;
                }

                paint_util_set_segment_support_height(
                    session,
                    paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D4, direction),
                    0xFFFF, 0);
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);

                switch (direction)
                {
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
}

/** rct2: 0x00824A5C */
static void lay_down_rc_track_banked_right_quarter_turn_5(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    lay_down_rc_track_banked_left_quarter_turn_5(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x00824A6C */
static void lay_down_rc_track_left_bank_to_25_deg_up(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 16250, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 16254, { 0, 0, height }, { 32, 1, 34 },
                    { 0, 27, height });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 16251, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 16255, { 0, 0, height }, { 32, 1, 34 },
                    { 0, 27, height });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 16252, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 16253, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
        }
        if (track_paint_util_should_paint_supports(session.MapPosition))
        {
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 3, height, session.TrackColours[SCHEME_SUPPORTS]);
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
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 26665, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 32 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 26666, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 32 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 26667, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 32 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 26668, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 32 });
                break;
        }

        paint_util_set_segment_support_height(
            session,
            paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
            0xFFFF, 0);
        if (track_paint_util_should_paint_supports(session.MapPosition))
        {
            switch (direction)
            {
                case 0:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 6, 0, height + 41, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height + 41, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 7, 0, height + 41, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height + 41, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
        }

        if (direction == 0 || direction == 3)
        {
            paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
        }
        else
        {
            paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_2);
        }
        paint_util_set_general_support_height(session, height + 48, 0x20);
    }
}

/** rct2: 0x00824A7C */
static void lay_down_rc_track_right_bank_to_25_deg_up(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 16256, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 16257, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 16258, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 16260, { 0, 0, height }, { 32, 1, 34 },
                    { 0, 27, height });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 16259, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 16261, { 0, 0, height }, { 32, 1, 34 },
                    { 0, 27, height });
                break;
        }
        if (track_paint_util_should_paint_supports(session.MapPosition))
        {
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 3, height, session.TrackColours[SCHEME_SUPPORTS]);
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
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 26669, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 32 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 26670, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 32 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 26671, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 32 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 26672, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 32 });
                break;
        }

        paint_util_set_segment_support_height(
            session,
            paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
            0xFFFF, 0);
        if (track_paint_util_should_paint_supports(session.MapPosition))
        {
            switch (direction)
            {
                case 0:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 6, 0, height + 41, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height + 41, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 7, 0, height + 41, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height + 41, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
        }

        if (direction == 0 || direction == 3)
        {
            paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
        }
        else
        {
            paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_2);
        }
        paint_util_set_general_support_height(session, height + 48, 0x20);
    }
}

/** rct2: 0x00824A8C */
static void lay_down_rc_track_25_deg_up_to_left_bank(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 16262, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 16266, { 0, 0, height }, { 32, 1, 34 },
                    { 0, 27, height });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 16263, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 16267, { 0, 0, height }, { 32, 1, 34 },
                    { 0, 27, height });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 16264, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 16265, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
        }
        if (track_paint_util_should_paint_supports(session.MapPosition))
        {
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
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
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 26673, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 32 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 26674, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 32 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 26675, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 32 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 26676, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 32 });
                break;
        }

        paint_util_set_segment_support_height(
            session,
            paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
            0xFFFF, 0);
        if (track_paint_util_should_paint_supports(session.MapPosition))
        {
            switch (direction)
            {
                case 0:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 6, 0, height + 41, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height + 41, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 7, 0, height + 41, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height + 41, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
        }

        if (direction == 0 || direction == 3)
        {
            paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_0);
        }
        else
        {
            paint_util_push_tunnel_rotated(session, direction, height + 8, TUNNEL_12);
        }
        paint_util_set_general_support_height(session, height + 40, 0x20);
    }
}

/** rct2: 0x00824A9C */
static void lay_down_rc_track_25_deg_up_to_right_bank(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 16268, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 16269, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 16270, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 16272, { 0, 0, height }, { 32, 1, 34 },
                    { 0, 27, height });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 16271, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 16273, { 0, 0, height }, { 32, 1, 34 },
                    { 0, 27, height });
                break;
        }
        if (track_paint_util_should_paint_supports(session.MapPosition))
        {
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
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
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 26677, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 32 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 26678, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 32 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 26679, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 32 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 26680, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 32 });
                break;
        }

        paint_util_set_segment_support_height(
            session,
            paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
            0xFFFF, 0);
        if (track_paint_util_should_paint_supports(session.MapPosition))
        {
            switch (direction)
            {
                case 0:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 6, 0, height + 41, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height + 41, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 7, 0, height + 41, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height + 41, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
        }

        if (direction == 0 || direction == 3)
        {
            paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_0);
        }
        else
        {
            paint_util_push_tunnel_rotated(session, direction, height + 8, TUNNEL_12);
        }
        paint_util_set_general_support_height(session, height + 40, 0x20);
    }
}

/** rct2: 0x00824AAC */
static void lay_down_rc_track_left_bank_to_25_deg_down(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    lay_down_rc_track_25_deg_up_to_right_bank(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x00824ABC */
static void lay_down_rc_track_right_bank_to_25_deg_down(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    lay_down_rc_track_25_deg_up_to_left_bank(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x00824ACC */
static void lay_down_rc_track_25_deg_down_to_left_bank(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    lay_down_rc_track_right_bank_to_25_deg_up(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x00824ADC */
static void lay_down_rc_track_25_deg_down_to_right_bank(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    lay_down_rc_track_left_bank_to_25_deg_up(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x00824AEC */
static void lay_down_rc_track_left_bank(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 16274, { 0, 0, height }, { 32, 1, 26 },
                    { 0, 27, height });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 16275, { 0, 0, height }, { 32, 1, 26 },
                    { 0, 27, height });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 16276, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 16277, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
        }
        if (track_paint_util_should_paint_supports(session.MapPosition))
        {
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
        }
        paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 26681, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 22 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 26682, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 22 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 26683, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 22 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 26684, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 22 });
                break;
        }

        paint_util_set_segment_support_height(
            session,
            paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
            0xFFFF, 0);
        if (track_paint_util_should_paint_supports(session.MapPosition))
        {
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
        }

        paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
    }
}

/** rct2: 0x00824AFC */
static void lay_down_rc_track_right_bank(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    lay_down_rc_track_left_bank(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x00824B0C */
static void lay_down_rc_track_left_quarter_turn_5_25_deg_up(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16510, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16515, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16520, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16525, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                }
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                if (direction == 0 || direction == 3)
                {
                    paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_1);
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 72, 0x20);
                break;
            case 1:
                paint_util_set_general_support_height(session, height + 72, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16511, { 0, 0, height }, { 32, 16, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16516, { 0, 0, height }, { 32, 16, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16521, { 0, 0, height }, { 32, 16, 3 },
                            { 0, 16, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16526, { 0, 0, height }, { 32, 16, 3 },
                            { 0, 16, height });
                        break;
                }
                paint_util_set_segment_support_height(
                    session,
                    paint_util_rotate_segments(
                        SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                    0xFFFF, 0);
                paint_util_set_general_support_height(session, height + 72, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16512, { 0, 0, height }, { 16, 16, 3 },
                            { 0, 16, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16517, { 0, 0, height }, { 16, 16, 3 },
                            { 16, 16, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16522, { 0, 0, height }, { 16, 16, 3 },
                            { 16, 0, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16527, { 0, 0, height }, { 16, 16, 3 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 64, 0x20);
                break;
            case 4:
                paint_util_set_general_support_height(session, height + 72, 0x20);
                break;
            case 5:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16513, { 0, 0, height }, { 16, 32, 3 },
                            { 16, 0, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16518, { 0, 0, height }, { 16, 32, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16523, { 0, 0, height }, { 16, 32, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16528, { 0, 0, height }, { 16, 32, 3 },
                            { 16, 0, height });
                        break;
                }
                paint_util_set_segment_support_height(
                    session,
                    paint_util_rotate_segments(
                        SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                paint_util_set_general_support_height(session, height + 72, 0x20);
                break;
            case 6:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16514, { 0, 0, height }, { 20, 32, 3 },
                            { 6, 0, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16519, { 0, 0, height }, { 20, 32, 3 },
                            { 6, 0, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16524, { 0, 0, height }, { 20, 32, 3 },
                            { 6, 0, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16529, { 0, 0, height }, { 20, 32, 3 },
                            { 6, 0, height });
                        break;
                }
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                switch (direction)
                {
                    case 2:
                        paint_util_push_tunnel_right(session, height + 8, TUNNEL_2);
                        break;
                    case 3:
                        paint_util_push_tunnel_left(session, height + 8, TUNNEL_2);
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 72, 0x20);
                break;
        }
    }
    else
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26949, { 0, 6, height + 24 },
                            { 32, 20, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26954, { 0, 6, height + 24 },
                            { 32, 20, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26959, { 0, 6, height + 24 },
                            { 32, 20, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26964, { 0, 6, height + 24 },
                            { 32, 20, 3 });
                        break;
                }

                paint_util_set_segment_support_height(
                    session,
                    paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 43, session.TrackColours[SCHEME_SUPPORTS]);

                if (direction == 0 || direction == 3)
                {
                    paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_1);
                }
                paint_util_set_general_support_height(session, height + 72, 0x20);
                break;
            case 1:
                paint_util_set_general_support_height(session, height + 72, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26950, { 0, 0, height + 24 },
                            { 32, 16, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26955, { 0, 0, height + 24 },
                            { 32, 16, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26960, { 0, 16, height + 24 },
                            { 32, 16, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26965, { 0, 16, height + 24 },
                            { 32, 16, 3 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session,
                    paint_util_rotate_segments(
                        SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                    0xFFFF, 0);
                paint_util_set_general_support_height(session, height + 72, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26951, { 0, 16, height + 24 },
                            { 16, 16, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26956, { 16, 16, height + 24 },
                            { 16, 16, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26961, { 16, 0, height + 24 },
                            { 16, 16, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26966, { 0, 0, height + 24 },
                            { 16, 16, 3 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 64, 0x20);
                break;
            case 4:
                paint_util_set_general_support_height(session, height + 72, 0x20);
                break;
            case 5:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26952, { 16, 0, height + 24 },
                            { 16, 32, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26957, { 0, 0, height + 24 },
                            { 16, 32, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26962, { 0, 0, height + 24 },
                            { 16, 32, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26967, { 16, 0, height + 24 },
                            { 16, 32, 3 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session,
                    paint_util_rotate_segments(
                        SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                paint_util_set_general_support_height(session, height + 72, 0x20);
                break;
            case 6:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26953, { 6, 0, height + 24 },
                            { 20, 32, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26958, { 6, 0, height + 24 },
                            { 20, 32, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26963, { 6, 0, height + 24 },
                            { 20, 32, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26968, { 6, 0, height + 24 },
                            { 20, 32, 3 });
                        break;
                }

                paint_util_set_segment_support_height(
                    session,
                    paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D4, direction),
                    0xFFFF, 0);
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 43, session.TrackColours[SCHEME_SUPPORTS]);

                switch (direction)
                {
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
}

/** rct2: 0x00824B1C */
static void lay_down_rc_track_right_quarter_turn_5_25_deg_up(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16490, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16495, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16500, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16505, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                }
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                if (direction == 0 || direction == 3)
                {
                    paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_1);
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 72, 0x20);
                break;
            case 1:
                paint_util_set_general_support_height(session, height + 72, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16491, { 0, 0, height }, { 32, 16, 3 },
                            { 0, 16, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16496, { 0, 0, height }, { 32, 16, 3 },
                            { 0, 16, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16501, { 0, 0, height }, { 32, 16, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16506, { 0, 0, height }, { 32, 16, 3 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session,
                    paint_util_rotate_segments(
                        SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                paint_util_set_general_support_height(session, height + 72, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16492, { 0, 0, height }, { 16, 16, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16497, { 0, 0, height }, { 16, 16, 3 },
                            { 16, 0, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16502, { 0, 0, height }, { 16, 16, 3 },
                            { 16, 16, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16507, { 0, 0, height }, { 16, 16, 3 },
                            { 0, 16, height });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 64, 0x20);
                break;
            case 4:
                paint_util_set_general_support_height(session, height + 72, 0x20);
                break;
            case 5:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16493, { 0, 0, height }, { 16, 32, 3 },
                            { 16, 0, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16498, { 0, 0, height }, { 16, 32, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16503, { 0, 0, height }, { 16, 32, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16508, { 0, 0, height }, { 16, 32, 3 },
                            { 16, 0, height });
                        break;
                }
                paint_util_set_segment_support_height(
                    session,
                    paint_util_rotate_segments(
                        SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                paint_util_set_general_support_height(session, height + 72, 0x20);
                break;
            case 6:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16494, { 0, 0, height }, { 20, 32, 3 },
                            { 6, 0, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16499, { 0, 0, height }, { 20, 32, 3 },
                            { 6, 0, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16504, { 0, 0, height }, { 20, 32, 3 },
                            { 6, 0, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16509, { 0, 0, height }, { 20, 32, 3 },
                            { 6, 0, height });
                        break;
                }
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                switch (direction)
                {
                    case 0:
                        paint_util_push_tunnel_right(session, height + 8, TUNNEL_2);
                        break;
                    case 1:
                        paint_util_push_tunnel_left(session, height + 8, TUNNEL_2);
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 72, 0x20);
                break;
        }
    }
    else
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26929, { 0, 6, height + 24 },
                            { 32, 20, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26934, { 0, 6, height + 24 },
                            { 32, 20, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26939, { 0, 6, height + 24 },
                            { 32, 20, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26944, { 0, 6, height + 24 },
                            { 32, 20, 3 });
                        break;
                }

                paint_util_set_segment_support_height(
                    session,
                    paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                    0xFFFF, 0);
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 43, session.TrackColours[SCHEME_SUPPORTS]);

                if (direction == 0 || direction == 3)
                {
                    paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_1);
                }
                paint_util_set_general_support_height(session, height + 72, 0x20);
                break;
            case 1:
                paint_util_set_general_support_height(session, height + 72, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26930, { 0, 16, height + 24 },
                            { 32, 16, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26935, { 0, 16, height + 24 },
                            { 32, 16, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26940, { 0, 0, height + 24 },
                            { 32, 16, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26945, { 0, 0, height + 24 },
                            { 32, 16, 3 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session,
                    paint_util_rotate_segments(
                        SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                paint_util_set_general_support_height(session, height + 72, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26931, { 0, 0, height + 24 },
                            { 16, 16, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26936, { 16, 0, height + 24 },
                            { 16, 16, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26941, { 16, 16, height + 24 },
                            { 16, 16, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26946, { 0, 16, height + 24 },
                            { 16, 16, 3 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 64, 0x20);
                break;
            case 4:
                paint_util_set_general_support_height(session, height + 72, 0x20);
                break;
            case 5:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26932, { 16, 0, height + 24 },
                            { 16, 32, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26937, { 0, 0, height + 24 },
                            { 16, 32, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26942, { 0, 0, height + 24 },
                            { 16, 32, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26947, { 16, 0, height + 24 },
                            { 16, 32, 3 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session,
                    paint_util_rotate_segments(
                        SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                paint_util_set_general_support_height(session, height + 72, 0x20);
                break;
            case 6:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26933, { 6, 0, height + 24 },
                            { 20, 32, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26938, { 6, 0, height + 24 },
                            { 20, 32, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26943, { 6, 0, height + 24 },
                            { 20, 32, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26948, { 6, 0, height + 24 },
                            { 20, 32, 3 });
                        break;
                }

                paint_util_set_segment_support_height(
                    session,
                    paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D4, direction),
                    0xFFFF, 0);
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 43, session.TrackColours[SCHEME_SUPPORTS]);

                switch (direction)
                {
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
}

/** rct2: 0x00824B2C */
static void lay_down_rc_track_left_quarter_turn_5_25_deg_down(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    lay_down_rc_track_right_quarter_turn_5_25_deg_up(session, ride, trackSequence, (direction + 1) & 3, height, trackElement);
}

/** rct2: 0x00824B3C */
static void lay_down_rc_track_right_quarter_turn_5_25_deg_down(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    lay_down_rc_track_left_quarter_turn_5_25_deg_up(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x00824B4C */
static void lay_down_rc_track_s_bend_left(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16402, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16406, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16405, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16409, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                }
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                if (direction == 0 || direction == 3)
                {
                    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16403, { 0, 0, height }, { 32, 26, 3 });
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16407, { 0, 0, height }, { 32, 26, 3 });
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 6, 1, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16404, { 0, 0, height }, { 32, 26, 3 },
                            { 0, 6, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16408, { 0, 0, height }, { 32, 26, 3 },
                            { 0, 6, height });
                        break;
                }
                paint_util_set_segment_support_height(
                    session,
                    paint_util_rotate_segments(
                        SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                    0xFFFF, 0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16404, { 0, 0, height }, { 32, 26, 3 },
                            { 0, 6, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16408, { 0, 0, height }, { 32, 26, 3 },
                            { 0, 6, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16403, { 0, 0, height }, { 32, 26, 3 });
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16407, { 0, 0, height }, { 32, 26, 3 });
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 6, 1, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
                paint_util_set_segment_support_height(
                    session,
                    paint_util_rotate_segments(
                        SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16405, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16409, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16402, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16406, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                }
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                    session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
        }
    }
    else
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26641, { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26645, { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26644, { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26648, { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                }

                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF,
                    0);
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);

                if (direction == 0 || direction == 3)
                {
                    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
                }
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26642, { 0, 0, height + 24 },
                            { 32, 26, 3 }, { 0, 0, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26646, { 0, 0, height + 24 },
                            { 32, 26, 3 }, { 0, 0, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26643, { 0, 0, height + 24 },
                            { 32, 26, 3 }, { 0, 6, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26647, { 0, 0, height + 24 },
                            { 32, 26, 3 }, { 0, 6, height + 22 });
                        break;
                }

                paint_util_set_segment_support_height(
                    session,
                    paint_util_rotate_segments(
                        SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                    0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 6, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26643, { 0, 0, height + 24 },
                            { 32, 26, 3 }, { 0, 6, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26647, { 0, 0, height + 24 },
                            { 32, 26, 3 }, { 0, 6, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26642, { 0, 0, height + 24 },
                            { 32, 26, 3 }, { 0, 0, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26646, { 0, 0, height + 24 },
                            { 32, 26, 3 }, { 0, 0, height + 22 });
                        break;
                }

                paint_util_set_segment_support_height(
                    session,
                    paint_util_rotate_segments(
                        SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                switch (direction)
                {
                    case 2:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 6, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26644, { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26648, { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26641, { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26645, { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                }

                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF,
                    0);
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);

                switch (direction)
                {
                    case 1:
                        paint_util_push_tunnel_right(session, height, TUNNEL_0);
                        break;
                    case 2:
                        paint_util_push_tunnel_left(session, height, TUNNEL_0);
                        break;
                }
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
        }
    }
}

/** rct2: 0x00824B5C */
static void lay_down_rc_track_s_bend_right(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16410, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16414, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16413, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16417, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                }
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                if (direction == 0 || direction == 3)
                {
                    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16411, { 0, 0, height }, { 32, 26, 3 },
                            { 0, 6, height });
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16415, { 0, 0, height }, { 32, 26, 3 },
                            { 0, 6, height });
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16412, { 0, 0, height }, { 32, 26, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16416, { 0, 0, height }, { 32, 26, 3 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session,
                    paint_util_rotate_segments(
                        SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16412, { 0, 0, height }, { 32, 26, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16416, { 0, 0, height }, { 32, 26, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16411, { 0, 0, height }, { 32, 26, 3 },
                            { 0, 6, height });
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16415, { 0, 0, height }, { 32, 26, 3 },
                            { 0, 6, height });
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
                paint_util_set_segment_support_height(
                    session,
                    paint_util_rotate_segments(
                        SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                    0xFFFF, 0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16413, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16417, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16410, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16414, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                }
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                    session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
        }
    }
    else
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26649, { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26653, { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26652, { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26656, { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                }

                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF,
                    0);
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);

                if (direction == 0 || direction == 3)
                {
                    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
                }
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26650, { 0, 0, height + 24 },
                            { 32, 26, 3 }, { 0, 6, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26654, { 0, 0, height + 24 },
                            { 32, 26, 3 }, { 0, 6, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26651, { 0, 0, height + 24 },
                            { 32, 26, 3 }, { 0, 0, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26655, { 0, 0, height + 24 },
                            { 32, 26, 3 }, { 0, 0, height + 22 });
                        break;
                }

                paint_util_set_segment_support_height(
                    session,
                    paint_util_rotate_segments(
                        SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 7, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26651, { 0, 0, height + 24 },
                            { 32, 26, 3 }, { 0, 0, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26655, { 0, 0, height + 24 },
                            { 32, 26, 3 }, { 0, 0, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26650, { 0, 0, height + 24 },
                            { 32, 26, 3 }, { 0, 6, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26654, { 0, 0, height + 24 },
                            { 32, 26, 3 }, { 0, 6, height + 22 });
                        break;
                }

                paint_util_set_segment_support_height(
                    session,
                    paint_util_rotate_segments(
                        SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                    0xFFFF, 0);
                switch (direction)
                {
                    case 2:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 7, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26652, { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26656, { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26649, { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26653, { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                }

                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF,
                    0);
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);

                switch (direction)
                {
                    case 1:
                        paint_util_push_tunnel_right(session, height, TUNNEL_0);
                        break;
                    case 2:
                        paint_util_push_tunnel_left(session, height, TUNNEL_0);
                        break;
                }
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
        }
    }
}

/** rct2: 0x00824B6C */
static void lay_down_rc_track_left_vertical_loop(
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16562, { 0, 6, height }, { 32, 20, 3 });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16570, { 0, 6, height }, { 32, 20, 3 });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16569, { 0, 6, height }, { 32, 20, 3 });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16577, { 0, 6, height }, { 32, 20, 7 });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16563, { 0, 0, height }, { 32, 26, 3 });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 4, 20, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16571, { 0, 14, height }, { 32, 2, 63 });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 4, 15, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16568, { 0, 6, height }, { 32, 26, 3 });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 4, 16, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16576, { 0, 6, height }, { 32, 26, 3 });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 4, 10, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16564, { 16, 0, height }, { 3, 16, 119 },
                        { 16, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16572, { 12, 0, height }, { 3, 16, 119 },
                        { 12, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16567, { 10, 16, height }, { 4, 16, 119 },
                        { 10, 16, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16575, { 16, 16, height }, { 2, 16, 119 },
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16565, { 0, 0, height + 32 }, { 32, 16, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16573, { 0, 0, height + 32 }, { 32, 16, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16566, { 0, 16, height + 32 }, { 32, 16, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16574, { 0, 16, height + 32 }, { 32, 16, 3 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16566, { 0, 16, height + 32 }, { 32, 16, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16574, { 0, 16, height + 32 }, { 32, 16, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16565, { 0, 0, height + 32 }, { 32, 16, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16573, { 0, 0, height + 32 }, { 32, 16, 3 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16567, { 10, 16, height }, { 4, 16, 119 },
                        { 10, 16, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16575, { 16, 16, height }, { 2, 16, 119 },
                        { 16, 16, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16564, { 16, 0, height }, { 3, 16, 119 },
                        { 16, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16572, { 12, 0, height }, { 3, 16, 119 },
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16568, { 0, 6, height }, { 32, 26, 3 });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 4, 16, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16576, { 0, 6, height }, { 32, 26, 3 });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 4, 10, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16563, { 0, 0, height }, { 32, 26, 3 });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 4, 20, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16571, { 0, 14, height }, { 32, 2, 63 });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 4, 15, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16569, { 0, 6, height }, { 32, 20, 3 });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16577, { 0, 6, height }, { 32, 20, 7 });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16562, { 0, 6, height }, { 32, 20, 3 });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16570, { 0, 6, height }, { 32, 20, 3 });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
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

/** rct2: 0x00824B7C */
static void lay_down_rc_track_right_vertical_loop(
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16593, { 0, 6, height }, { 32, 20, 7 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16585, { 0, 6, height }, { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16586, { 0, 6, height }, { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16578, { 0, 6, height }, { 32, 20, 3 });
                    break;
            }
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16592, { 0, 6, height }, { 32, 26, 3 });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16584, { 0, 6, height }, { 32, 26, 3 });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 4, 16, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16587, { 0, 14, height }, { 32, 2, 63 });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 4, 12, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16579, { 0, 0, height }, { 32, 26, 3 });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 4, 20, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            paint_util_set_general_support_height(session, height + 72, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16591, { 16, 16, height }, { 2, 16, 119 },
                        { 16, 16, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16583, { 10, 16, height }, { 4, 16, 119 },
                        { 10, 16, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16588, { 12, 0, height }, { 3, 16, 119 },
                        { 12, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16580, { 16, 0, height }, { 2, 16, 119 },
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16590, { 0, 16, height + 32 }, { 32, 16, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16582, { 0, 16, height + 32 }, { 32, 16, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16589, { 0, 0, height + 32 }, { 32, 16, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16581, { 0, 0, height + 32 }, { 32, 16, 3 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16589, { 0, 0, height + 32 }, { 32, 16, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16581, { 0, 0, height + 32 }, { 32, 16, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16590, { 0, 16, height + 32 }, { 32, 16, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16582, { 0, 16, height + 32 }, { 32, 16, 3 });
                    break;
            }
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
        case 7:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16588, { 12, 0, height }, { 3, 16, 119 },
                        { 12, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16580, { 16, 0, height }, { 2, 16, 119 },
                        { 16, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16591, { 16, 16, height }, { 2, 16, 119 },
                        { 16, 16, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16583, { 10, 16, height }, { 4, 16, 119 },
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16587, { 0, 14, height }, { 32, 2, 63 });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 4, 12, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16579, { 0, 0, height }, { 32, 26, 3 });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 4, 20, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16592, { 0, 6, height }, { 32, 26, 3 });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16584, { 0, 6, height }, { 32, 26, 3 });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 4, 16, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            paint_util_set_general_support_height(session, height + 72, 0x20);
            break;
        case 9:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16586, { 0, 6, height }, { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16578, { 0, 6, height }, { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16593, { 0, 6, height }, { 32, 20, 7 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16585, { 0, 6, height }, { 32, 20, 3 });
                    break;
            }
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
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

/** rct2: 0x00824BBC */
static void lay_down_rc_track_left_quarter_turn_3(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16381, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16384, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16387, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16378, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                }
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                if (direction == 0 || direction == 3)
                {
                    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 1:
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16380, { 0, 0, height }, { 16, 16, 3 },
                            { 16, 0, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16383, { 0, 0, height }, { 16, 16, 3 },
                            { 0, 0, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16386, { 0, 0, height }, { 16, 16, 3 },
                            { 0, 16, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16377, { 0, 0, height }, { 16, 16, 3 },
                            { 16, 16, height });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16379, { 0, 0, height }, { 20, 32, 3 },
                            { 6, 0, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16382, { 0, 0, height }, { 20, 32, 3 },
                            { 6, 0, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16385, { 0, 0, height }, { 20, 32, 3 },
                            { 6, 0, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16376, { 0, 0, height }, { 20, 32, 3 },
                            { 6, 0, height });
                        break;
                }
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                switch (direction)
                {
                    case 2:
                        paint_util_push_tunnel_right(session, height, TUNNEL_0);
                        break;
                    case 3:
                        paint_util_push_tunnel_left(session, height, TUNNEL_0);
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
        }
    }
    else
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26710, { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26713, { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26716, { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26707, { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                }

                paint_util_set_segment_support_height(
                    session,
                    paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);

                if (direction == 0 || direction == 3)
                {
                    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
                }
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 1:
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26709, { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 16, 0, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26712, { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 0, 0, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26715, { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 0, 16, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26706, { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 16, 16, height + 22 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26708, { 0, 0, height + 24 },
                            { 20, 32, 3 }, { 6, 0, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26711, { 0, 0, height + 24 },
                            { 20, 32, 3 }, { 6, 0, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26714, { 0, 0, height + 24 },
                            { 20, 32, 3 }, { 6, 0, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26705, { 0, 0, height + 24 },
                            { 20, 32, 3 }, { 6, 0, height + 22 });
                        break;
                }

                paint_util_set_segment_support_height(
                    session,
                    paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D4, direction),
                    0xFFFF, 0);
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);

                switch (direction)
                {
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
}

/** rct2: 0x00824BCC */
static void lay_down_rc_track_right_quarter_turn_3(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    lay_down_rc_track_left_quarter_turn_3(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x00824BDC */
static void lay_down_rc_track_left_quarter_turn_3_bank(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16393, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16400, { 0, 0, height }, { 32, 1, 26 },
                            { 0, 27, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16396, { 0, 0, height }, { 32, 1, 26 },
                            { 0, 27, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16399, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16390, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                }
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                if (direction == 0 || direction == 3)
                {
                    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 1:
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16392, { 0, 0, height }, { 16, 16, 3 },
                            { 16, 0, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16395, { 0, 0, height }, { 16, 16, 1 },
                            { 0, 0, height + 27 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16398, { 0, 0, height }, { 16, 16, 3 },
                            { 0, 16, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16389, { 0, 0, height }, { 16, 16, 3 },
                            { 16, 16, height });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16391, { 0, 0, height }, { 20, 32, 3 },
                            { 6, 0, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16394, { 0, 0, height }, { 1, 32, 26 },
                            { 27, 0, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16397, { 0, 0, height }, { 20, 32, 3 },
                            { 6, 0, height });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16401, { 0, 0, height }, { 1, 32, 26 },
                            { 27, 0, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16388, { 0, 0, height }, { 20, 32, 3 },
                            { 6, 0, height });
                        break;
                }
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                switch (direction)
                {
                    case 2:
                        paint_util_push_tunnel_right(session, height, TUNNEL_0);
                        break;
                    case 3:
                        paint_util_push_tunnel_left(session, height, TUNNEL_0);
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
        }
    }
    else
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26722, { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26725, { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26728, { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26719, { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                }

                paint_util_set_segment_support_height(
                    session,
                    paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);

                if (direction == 0 || direction == 3)
                {
                    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
                }
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 1:
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26721, { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 16, 0, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26724, { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 0, 0, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26727, { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 0, 16, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26718, { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 16, 16, height + 22 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26720, { 0, 0, height + 24 },
                            { 20, 32, 3 }, { 6, 0, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26723, { 0, 0, height + 24 },
                            { 20, 32, 3 }, { 6, 0, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26726, { 0, 0, height + 24 },
                            { 20, 32, 3 }, { 6, 0, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26717, { 0, 0, height + 24 },
                            { 20, 32, 3 }, { 6, 0, height + 22 });
                        break;
                }

                paint_util_set_segment_support_height(
                    session,
                    paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D4, direction),
                    0xFFFF, 0);
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);

                switch (direction)
                {
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
}

/** rct2: 0x00824BEC */
static void lay_down_rc_track_right_quarter_turn_3_bank(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    lay_down_rc_track_left_quarter_turn_3_bank(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x00824BFC */
static void lay_down_rc_track_left_quarter_turn_3_25_deg_up(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16541, { 0, 6, height }, { 32, 20, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16543, { 0, 6, height }, { 32, 20, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16545, { 0, 6, height }, { 32, 20, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16539, { 0, 6, height }, { 32, 20, 3 });
                        break;
                }
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                if (direction == 0 || direction == 3)
                {
                    paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_1);
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 72, 0x20);
                break;
            case 1:
                paint_util_set_general_support_height(session, height + 56, 0x20);
                break;
            case 2:
                paint_util_set_general_support_height(session, height + 56, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16540, { 6, 0, height }, { 20, 32, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16542, { 6, 0, height }, { 20, 32, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16544, { 6, 0, height }, { 20, 32, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16538, { 6, 0, height }, { 20, 32, 3 });
                        break;
                }
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                switch (direction)
                {
                    case 2:
                        paint_util_push_tunnel_right(session, height + 8, TUNNEL_2);
                        break;
                    case 3:
                        paint_util_push_tunnel_left(session, height + 8, TUNNEL_2);
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 72, 0x20);
                break;
        }
    }
    else
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26924, { 0, 6, height + 24 },
                            { 32, 20, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26926, { 0, 6, height + 24 },
                            { 32, 20, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26928, { 0, 6, height + 24 },
                            { 32, 20, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26922, { 0, 6, height + 24 },
                            { 32, 20, 3 });
                        break;
                }

                paint_util_set_segment_support_height(
                    session,
                    paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 41, session.TrackColours[SCHEME_SUPPORTS]);

                if (direction == 0 || direction == 3)
                {
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
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26923, { 6, 0, height + 24 },
                            { 20, 32, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26925, { 6, 0, height + 24 },
                            { 20, 32, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26927, { 6, 0, height + 24 },
                            { 20, 32, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26921, { 6, 0, height + 24 },
                            { 20, 32, 3 });
                        break;
                }

                paint_util_set_segment_support_height(
                    session,
                    paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D4, direction),
                    0xFFFF, 0);
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 41, session.TrackColours[SCHEME_SUPPORTS]);

                switch (direction)
                {
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
}

/** rct2: 0x00824C0C */
static void lay_down_rc_track_right_quarter_turn_3_25_deg_up(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16530, { 0, 6, height }, { 32, 20, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16532, { 0, 6, height }, { 32, 20, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16534, { 0, 6, height }, { 32, 20, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16536, { 0, 6, height }, { 32, 20, 3 });
                        break;
                }
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                if (direction == 0 || direction == 3)
                {
                    paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_1);
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 72, 0x20);
                break;
            case 1:
                paint_util_set_general_support_height(session, height + 56, 0x20);
                break;
            case 2:
                paint_util_set_general_support_height(session, height + 56, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16531, { 6, 0, height }, { 20, 32, 3 });
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16533, { 6, 0, height }, { 20, 32, 3 });
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16535, { 6, 0, height }, { 20, 32, 3 });
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 4, 10, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16537, { 6, 0, height }, { 20, 32, 3 });
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
                switch (direction)
                {
                    case 0:
                        paint_util_push_tunnel_right(session, height + 8, TUNNEL_2);
                        break;
                    case 1:
                        paint_util_push_tunnel_left(session, height + 8, TUNNEL_2);
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 72, 0x20);
                break;
        }
    }
    else
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26913, { 0, 6, height + 24 },
                            { 32, 20, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26915, { 0, 6, height + 24 },
                            { 32, 20, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26917, { 0, 6, height + 24 },
                            { 32, 20, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26919, { 0, 6, height + 24 },
                            { 32, 20, 3 });
                        break;
                }

                paint_util_set_segment_support_height(
                    session,
                    paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                    0xFFFF, 0);
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 41, session.TrackColours[SCHEME_SUPPORTS]);

                if (direction == 0 || direction == 3)
                {
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
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26914, { 6, 0, height + 24 },
                            { 20, 32, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26916, { 6, 0, height + 24 },
                            { 20, 32, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26918, { 6, 0, height + 24 },
                            { 20, 32, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26920, { 6, 0, height + 24 },
                            { 20, 32, 3 });
                        break;
                }

                paint_util_set_segment_support_height(
                    session,
                    paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D4, direction),
                    0xFFFF, 0);
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 41, session.TrackColours[SCHEME_SUPPORTS]);

                switch (direction)
                {
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
}

/** rct2: 0x00824C1C */
static void lay_down_rc_track_left_quarter_turn_3_25_deg_down(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    lay_down_rc_track_right_quarter_turn_3_25_deg_up(session, ride, trackSequence, (direction + 1) & 3, height, trackElement);
}

/** rct2: 0x00824C2C */
static void lay_down_rc_track_right_quarter_turn_3_25_deg_down(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    lay_down_rc_track_left_quarter_turn_3_25_deg_up(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x00824C9C */
static void lay_down_rc_track_left_half_banked_helix_up_small(
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16481, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16488, { 0, 0, height }, { 32, 1, 26 },
                        { 0, 27, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16484, { 0, 0, height }, { 32, 1, 26 },
                        { 0, 27, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16487, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16478, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
            }
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 2, height, session.TrackColours[SCHEME_SUPPORTS]);
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
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16480, { 0, 0, height }, { 16, 16, 3 },
                        { 16, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16483, { 0, 0, height }, { 16, 16, 1 },
                        { 0, 0, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16486, { 0, 0, height }, { 16, 16, 3 },
                        { 0, 16, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16477, { 0, 0, height }, { 16, 16, 3 },
                        { 16, 16, height });
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16479, { 0, 0, height }, { 20, 32, 3 },
                        { 6, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16482, { 0, 0, height }, { 1, 32, 26 },
                        { 27, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16485, { 0, 0, height }, { 20, 32, 3 },
                        { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16489, { 0, 0, height }, { 1, 32, 26 },
                        { 27, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16476, { 0, 0, height }, { 20, 32, 3 },
                        { 6, 0, height });
                    break;
            }
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
            switch (direction)
            {
                case 2:
                    paint_util_push_tunnel_right(session, height + 8, TUNNEL_0);
                    break;
                case 3:
                    paint_util_push_tunnel_left(session, height + 8, TUNNEL_0);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 4:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16478, { 0, 0, height }, { 20, 32, 3 },
                        { 6, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16481, { 0, 0, height }, { 20, 32, 3 },
                        { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16488, { 0, 0, height }, { 1, 32, 26 },
                        { 27, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16484, { 0, 0, height }, { 1, 32, 26 },
                        { 27, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16487, { 0, 0, height }, { 20, 32, 3 },
                        { 6, 0, height });
                    break;
            }
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 2, height, session.TrackColours[SCHEME_SUPPORTS]);
            switch (direction)
            {
                case 0:
                    paint_util_push_tunnel_right(session, height, TUNNEL_0);
                    break;
                case 1:
                    paint_util_push_tunnel_left(session, height, TUNNEL_0);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 5:
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16477, { 0, 0, height }, { 16, 16, 3 },
                        { 16, 16, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16480, { 0, 0, height }, { 16, 16, 3 },
                        { 0, 16, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16483, { 0, 0, height }, { 16, 16, 1 },
                        { 0, 0, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16486, { 0, 0, height }, { 16, 16, 3 },
                        { 16, 0, height });
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 7:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16476, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16479, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16482, { 0, 0, height }, { 32, 1, 26 },
                        { 0, 27, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16485, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16489, { 0, 0, height }, { 32, 1, 26 },
                        { 0, 27, height });
                    break;
            }
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height + 8, TUNNEL_0);
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x00824CAC */
static void lay_down_rc_track_right_half_banked_helix_up_small(
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16462, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16465, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16468, { 0, 0, height }, { 32, 1, 26 },
                        { 0, 27, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16471, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16475, { 0, 0, height }, { 32, 1, 26 },
                        { 0, 27, height });
                    break;
            }
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 2, height, session.TrackColours[SCHEME_SUPPORTS]);
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
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16463, { 0, 0, height }, { 16, 16, 3 },
                        { 16, 16, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16466, { 0, 0, height }, { 16, 16, 3 },
                        { 0, 16, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16469, { 0, 0, height }, { 16, 16, 1 },
                        { 0, 0, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16472, { 0, 0, height }, { 16, 16, 3 },
                        { 16, 0, height });
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16464, { 0, 0, height }, { 20, 32, 3 },
                        { 6, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16467, { 0, 0, height }, { 20, 32, 3 },
                        { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16474, { 0, 0, height }, { 1, 32, 26 },
                        { 27, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16470, { 0, 0, height }, { 1, 32, 26 },
                        { 27, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16473, { 0, 0, height }, { 20, 32, 3 },
                        { 6, 0, height });
                    break;
            }
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
            switch (direction)
            {
                case 0:
                    paint_util_push_tunnel_right(session, height + 8, TUNNEL_0);
                    break;
                case 1:
                    paint_util_push_tunnel_left(session, height + 8, TUNNEL_0);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 4:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16465, { 0, 0, height }, { 20, 32, 3 },
                        { 6, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16468, { 0, 0, height }, { 1, 32, 26 },
                        { 27, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16471, { 0, 0, height }, { 20, 32, 3 },
                        { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16475, { 0, 0, height }, { 1, 32, 26 },
                        { 27, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16462, { 0, 0, height }, { 20, 32, 3 },
                        { 6, 0, height });
                    break;
            }
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 2, height, session.TrackColours[SCHEME_SUPPORTS]);
            switch (direction)
            {
                case 2:
                    paint_util_push_tunnel_right(session, height, TUNNEL_0);
                    break;
                case 3:
                    paint_util_push_tunnel_left(session, height, TUNNEL_0);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 5:
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16466, { 0, 0, height }, { 16, 16, 3 },
                        { 16, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16469, { 0, 0, height }, { 16, 16, 1 },
                        { 0, 0, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16472, { 0, 0, height }, { 16, 16, 3 },
                        { 0, 16, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16463, { 0, 0, height }, { 16, 16, 3 },
                        { 16, 16, height });
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 7:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16467, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16474, { 0, 0, height }, { 32, 1, 26 },
                        { 0, 27, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16470, { 0, 0, height }, { 32, 1, 26 },
                        { 0, 27, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16473, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16464, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
            }
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height + 8, TUNNEL_0);
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x00824CBC */
static void lay_down_rc_track_left_half_banked_helix_down_small(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackSequence >= 4)
    {
        trackSequence -= 4;
        direction = (direction - 1) & 3;
    }
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    lay_down_rc_track_right_half_banked_helix_up_small(session, ride, trackSequence, (direction + 1) & 3, height, trackElement);
}

/** rct2: 0x00824CCC */
static void lay_down_rc_track_right_half_banked_helix_down_small(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackSequence >= 4)
    {
        trackSequence -= 4;
        direction = (direction + 1) & 3;
    }
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    lay_down_rc_track_left_half_banked_helix_up_small(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x00824CDC */
static void lay_down_rc_track_left_half_banked_helix_up_large(
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16449, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16460, { 0, 0, height }, { 32, 1, 26 },
                        { 0, 27, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16454, { 0, 0, height }, { 32, 1, 26 },
                        { 0, 27, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16459, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16444, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
            }
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 1, height, session.TrackColours[SCHEME_SUPPORTS]);
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
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16448, { 0, 0, height }, { 32, 16, 3 },
                        { 0, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16453, { 0, 0, height }, { 32, 16, 1 },
                        { 0, 0, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16458, { 0, 0, height }, { 32, 16, 3 },
                        { 0, 16, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16443, { 0, 0, height }, { 32, 16, 3 },
                        { 0, 16, height });
                    break;
            }
            paint_util_set_segment_support_height(
                session,
                paint_util_rotate_segments(
                    SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16447, { 0, 0, height }, { 16, 16, 3 },
                        { 0, 16, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16452, { 0, 0, height }, { 16, 16, 1 },
                        { 16, 16, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16457, { 0, 0, height }, { 16, 16, 3 },
                        { 16, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16442, { 0, 0, height }, { 16, 16, 3 },
                        { 0, 0, height });
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 4:
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 5:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16446, { 0, 0, height }, { 16, 32, 3 },
                        { 16, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16451, { 0, 0, height }, { 16, 32, 1 },
                        { 0, 0, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16456, { 0, 0, height }, { 16, 32, 3 },
                        { 0, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16441, { 0, 0, height }, { 16, 32, 3 },
                        { 16, 0, height });
                    break;
            }
            paint_util_set_segment_support_height(
                session,
                paint_util_rotate_segments(
                    SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16445, { 0, 0, height }, { 20, 32, 3 },
                        { 6, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16450, { 0, 0, height }, { 1, 32, 26 },
                        { 27, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16455, { 0, 0, height }, { 20, 32, 3 },
                        { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16461, { 0, 0, height }, { 1, 32, 26 },
                        { 27, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16440, { 0, 0, height }, { 20, 32, 3 },
                        { 6, 0, height });
                    break;
            }
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 7, height, session.TrackColours[SCHEME_SUPPORTS]);
            switch (direction)
            {
                case 2:
                    paint_util_push_tunnel_right(session, height + 8, TUNNEL_0);
                    break;
                case 3:
                    paint_util_push_tunnel_left(session, height + 8, TUNNEL_0);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 7:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16444, { 0, 0, height }, { 20, 32, 3 },
                        { 6, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16449, { 0, 0, height }, { 20, 32, 3 },
                        { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16460, { 0, 0, height }, { 1, 32, 26 },
                        { 27, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16454, { 0, 0, height }, { 1, 32, 26 },
                        { 27, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16459, { 0, 0, height }, { 20, 32, 3 },
                        { 6, 0, height });
                    break;
            }
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 1, height, session.TrackColours[SCHEME_SUPPORTS]);
            switch (direction)
            {
                case 0:
                    paint_util_push_tunnel_right(session, height, TUNNEL_0);
                    break;
                case 1:
                    paint_util_push_tunnel_left(session, height, TUNNEL_0);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 8:
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 9:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16443, { 0, 0, height }, { 16, 32, 3 },
                        { 16, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16448, { 0, 0, height }, { 16, 32, 3 },
                        { 0, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16453, { 0, 0, height }, { 16, 32, 1 },
                        { 0, 0, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16458, { 0, 0, height }, { 16, 32, 3 },
                        { 16, 0, height });
                    break;
            }
            paint_util_set_segment_support_height(
                session,
                paint_util_rotate_segments(
                    SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 10:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16442, { 0, 0, height }, { 16, 16, 3 },
                        { 0, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16447, { 0, 0, height }, { 16, 16, 3 },
                        { 16, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16452, { 0, 0, height }, { 16, 16, 1 },
                        { 16, 16, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16457, { 0, 0, height }, { 16, 16, 3 },
                        { 0, 16, height });
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 11:
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 12:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16441, { 0, 0, height }, { 32, 16, 3 },
                        { 0, 16, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16446, { 0, 0, height }, { 32, 16, 3 },
                        { 0, 16, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16451, { 0, 0, height }, { 32, 16, 1 },
                        { 0, 0, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16456, { 0, 0, height }, { 32, 16, 3 },
                        { 0, 0, height });
                    break;
            }
            paint_util_set_segment_support_height(
                session,
                paint_util_rotate_segments(
                    SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 13:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16440, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16445, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16450, { 0, 0, height }, { 32, 1, 26 },
                        { 0, 27, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16455, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16461, { 0, 0, height }, { 32, 1, 26 },
                        { 0, 27, height });
                    break;
            }
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 7, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height + 8, TUNNEL_0);
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x00824CEC */
static void lay_down_rc_track_right_half_banked_helix_up_large(
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16418, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16423, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16428, { 0, 0, height }, { 32, 1, 26 },
                        { 0, 27, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16433, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16439, { 0, 0, height }, { 32, 1, 26 },
                        { 0, 27, height });
                    break;
            }
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 1, height, session.TrackColours[SCHEME_SUPPORTS]);
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
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16419, { 0, 0, height }, { 32, 16, 3 },
                        { 0, 16, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16424, { 0, 0, height }, { 32, 16, 3 },
                        { 0, 16, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16429, { 0, 0, height }, { 32, 16, 1 },
                        { 0, 0, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16434, { 0, 0, height }, { 32, 16, 3 },
                        { 0, 0, height });
                    break;
            }
            paint_util_set_segment_support_height(
                session,
                paint_util_rotate_segments(
                    SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16420, { 0, 0, height }, { 16, 16, 3 },
                        { 0, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16425, { 0, 0, height }, { 16, 16, 3 },
                        { 16, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16430, { 0, 0, height }, { 16, 16, 1 },
                        { 16, 16, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16435, { 0, 0, height }, { 16, 16, 3 },
                        { 0, 16, height });
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 4:
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 5:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16421, { 0, 0, height }, { 16, 32, 3 },
                        { 16, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16426, { 0, 0, height }, { 16, 32, 3 },
                        { 0, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16431, { 0, 0, height }, { 16, 32, 1 },
                        { 0, 0, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16436, { 0, 0, height }, { 16, 32, 3 },
                        { 16, 0, height });
                    break;
            }
            paint_util_set_segment_support_height(
                session,
                paint_util_rotate_segments(
                    SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16422, { 0, 0, height }, { 20, 32, 3 },
                        { 6, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16427, { 0, 0, height }, { 20, 32, 3 },
                        { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16438, { 0, 0, height }, { 1, 32, 26 },
                        { 27, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16432, { 0, 0, height }, { 1, 32, 26 },
                        { 27, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16437, { 0, 0, height }, { 20, 32, 3 },
                        { 6, 0, height });
                    break;
            }
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 7, height, session.TrackColours[SCHEME_SUPPORTS]);
            switch (direction)
            {
                case 0:
                    paint_util_push_tunnel_right(session, height + 8, TUNNEL_0);
                    break;
                case 1:
                    paint_util_push_tunnel_left(session, height + 8, TUNNEL_0);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 7:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16423, { 0, 0, height }, { 20, 32, 3 },
                        { 6, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16428, { 0, 0, height }, { 1, 32, 26 },
                        { 27, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16433, { 0, 0, height }, { 20, 32, 3 },
                        { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16439, { 0, 0, height }, { 1, 32, 26 },
                        { 27, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16418, { 0, 0, height }, { 20, 32, 3 },
                        { 6, 0, height });
                    break;
            }
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 1, height, session.TrackColours[SCHEME_SUPPORTS]);
            switch (direction)
            {
                case 2:
                    paint_util_push_tunnel_right(session, height, TUNNEL_0);
                    break;
                case 3:
                    paint_util_push_tunnel_left(session, height, TUNNEL_0);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 8:
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 9:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16424, { 0, 0, height }, { 16, 32, 3 },
                        { 16, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16429, { 0, 0, height }, { 16, 32, 1 },
                        { 0, 0, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16434, { 0, 0, height }, { 16, 32, 3 },
                        { 0, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16419, { 0, 0, height }, { 16, 32, 3 },
                        { 16, 0, height });
                    break;
            }
            paint_util_set_segment_support_height(
                session,
                paint_util_rotate_segments(
                    SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 10:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16425, { 0, 0, height }, { 16, 16, 3 },
                        { 0, 16, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16430, { 0, 0, height }, { 16, 16, 1 },
                        { 16, 16, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16435, { 0, 0, height }, { 16, 16, 3 },
                        { 16, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16420, { 0, 0, height }, { 16, 16, 3 },
                        { 0, 0, height });
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 11:
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 12:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16426, { 0, 0, height }, { 32, 16, 3 },
                        { 0, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16431, { 0, 0, height }, { 32, 16, 1 },
                        { 0, 0, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16436, { 0, 0, height }, { 32, 16, 3 },
                        { 0, 16, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16421, { 0, 0, height }, { 32, 16, 3 },
                        { 0, 16, height });
                    break;
            }
            paint_util_set_segment_support_height(
                session,
                paint_util_rotate_segments(
                    SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 13:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16427, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16438, { 0, 0, height }, { 32, 1, 26 },
                        { 0, 27, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16432, { 0, 0, height }, { 32, 1, 26 },
                        { 0, 27, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16437, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16422, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
            }
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 7, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height + 8, TUNNEL_0);
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x00824CFC */
static void lay_down_rc_track_left_half_banked_helix_down_large(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackSequence >= 7)
    {
        trackSequence -= 7;
        direction = (direction - 1) & 3;
    }
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    lay_down_rc_track_right_half_banked_helix_up_large(session, ride, trackSequence, (direction + 1) & 3, height, trackElement);
}

/** rct2: 0x00824D0C */
static void lay_down_rc_track_right_half_banked_helix_down_large(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackSequence >= 7)
    {
        trackSequence -= 7;
        direction = (direction + 1) & 3;
    }
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    lay_down_rc_track_left_half_banked_helix_up_large(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x00824D3C */
static void lay_down_rc_track_left_quarter_turn_1_60_deg_up(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 16555, { 0, 0, height }, { 28, 28, 3 },
                    { 2, 2, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 16559, { 0, 0, height }, { 28, 28, 1 },
                    { 2, 2, height + 99 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 16556, { 0, 0, height }, { 28, 28, 3 },
                    { 2, 2, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 16560, { 0, 0, height }, { 28, 28, 1 },
                    { 2, 2, height + 99 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 16557, { 0, 0, height }, { 28, 28, 3 },
                    { 2, 2, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 16561, { 0, 0, height }, { 28, 28, 1 },
                    { 2, 2, height + 99 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 16554, { 0, 0, height }, { 28, 28, 3 },
                    { 2, 2, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 16558, { 0, 0, height }, { 28, 28, 1 },
                    { 2, 2, height + 99 });
                break;
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 26910, { 0, 0, height }, { 28, 28, 3 },
                    { 2, 2, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 26906, { 0, 0, height }, { 28, 28, 1 },
                    { 2, 2, height + 99 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 26911, { 0, 0, height }, { 28, 28, 3 },
                    { 2, 2, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 26907, { 0, 0, height }, { 28, 28, 1 },
                    { 2, 2, height + 99 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 26912, { 0, 0, height }, { 28, 28, 3 },
                    { 2, 2, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 26908, { 0, 0, height }, { 28, 28, 1 },
                    { 2, 2, height + 99 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 26909, { 0, 0, height }, { 28, 28, 3 },
                    { 2, 2, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 26905, { 0, 0, height }, { 28, 28, 1 },
                    { 2, 2, height + 99 });
                break;
        }
    }
    track_paint_util_left_quarter_turn_1_tile_tunnel(session, direction, height, -8, TUNNEL_1, +56, TUNNEL_2);
    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 104, 0x20);
}

/** rct2: 0x00824D1C */
static void lay_down_rc_track_right_quarter_turn_1_60_deg_up(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 16546, { 0, 0, height }, { 28, 28, 3 },
                    { 2, 2, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 16550, { 0, 0, height }, { 28, 28, 1 },
                    { 2, 2, height + 99 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 16547, { 0, 0, height }, { 28, 28, 3 },
                    { 2, 2, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 16551, { 0, 0, height }, { 28, 28, 1 },
                    { 2, 2, height + 99 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 16548, { 0, 0, height }, { 28, 28, 3 },
                    { 2, 2, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 16552, { 0, 0, height }, { 28, 28, 1 },
                    { 2, 2, height + 99 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 16549, { 0, 0, height }, { 28, 28, 3 },
                    { 2, 2, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 16553, { 0, 0, height }, { 28, 28, 1 },
                    { 2, 2, height + 99 });
                break;
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 26901, { 0, 0, height }, { 28, 28, 3 },
                    { 2, 2, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 26897, { 0, 0, height }, { 28, 28, 1 },
                    { 2, 2, height + 99 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 26902, { 0, 0, height }, { 28, 28, 3 },
                    { 2, 2, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 26898, { 0, 0, height }, { 28, 28, 1 },
                    { 2, 2, height + 99 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 26903, { 0, 0, height }, { 28, 28, 3 },
                    { 2, 2, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 26899, { 0, 0, height }, { 28, 28, 1 },
                    { 2, 2, height + 99 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 26904, { 0, 0, height }, { 28, 28, 3 },
                    { 2, 2, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 26900, { 0, 0, height }, { 28, 28, 1 },
                    { 2, 2, height + 99 });
                break;
        }
    }

    track_paint_util_right_quarter_turn_1_tile_tunnel(session, direction, height, -8, TUNNEL_1, +56, TUNNEL_2);
    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 104, 0x20);
}

/** rct2: 0x00824D2C */
static void lay_down_rc_track_left_quarter_turn_1_60_deg_down(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    lay_down_rc_track_right_quarter_turn_1_60_deg_up(session, ride, trackSequence, (direction + 1) & 3, height, trackElement);
}

/** rct2: 0x00824D4C */
static void lay_down_rc_track_right_quarter_turn_1_60_deg_down(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    lay_down_rc_track_left_quarter_turn_1_60_deg_up(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x00824D5C */
static void lay_down_rc_track_brakes(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (direction)
        {
            case 0:
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 16230, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 1:
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 16231, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
        }
        if (track_paint_util_should_paint_supports(session.MapPosition))
        {
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
        }
        paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
    }
    else
    {
        switch (direction)
        {
            case 0:
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 26559, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 24 });
                break;
            case 1:
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 26560, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 24 });
                break;
        }

        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        if (track_paint_util_should_paint_supports(session.MapPosition))
        {
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
        }

        paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
    }
}

/** rct2: 0x00824D6C */
static void lay_down_rc_track_on_ride_photo(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, IMAGE_TYPE_REMAP | SPR_STATION_BASE_D, { 0, 0, height }, { 32, 32, 1 });
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 16224, { 0, 0, height }, { 32, 20, 0 },
                    { 0, 6, height + 3 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, IMAGE_TYPE_REMAP | SPR_STATION_BASE_D, { 0, 0, height }, { 32, 32, 1 });
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 6, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 16225, { 0, 0, height }, { 32, 20, 0 },
                    { 0, 6, height + 3 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, IMAGE_TYPE_REMAP | SPR_STATION_BASE_D, { 0, 0, height }, { 32, 32, 1 });
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 16224, { 0, 0, height }, { 32, 20, 0 },
                    { 0, 6, height + 3 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, IMAGE_TYPE_REMAP | SPR_STATION_BASE_D, { 0, 0, height }, { 32, 32, 1 });
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 6, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 16225, { 0, 0, height }, { 32, 20, 0 },
                    { 0, 6, height + 3 });
                break;
        }
        track_paint_util_onride_photo_paint(session, direction, height + 3, trackElement);
        paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 48, 0x20);
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, IMAGE_TYPE_REMAP | SPR_STATION_BASE_D, { 0, 0, height }, { 32, 32, 1 });
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 26555, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 24 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, IMAGE_TYPE_REMAP | SPR_STATION_BASE_D, { 0, 0, height }, { 32, 32, 1 });
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 6, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 26556, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 24 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, IMAGE_TYPE_REMAP | SPR_STATION_BASE_D, { 0, 0, height }, { 32, 32, 1 });
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 26555, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 24 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, IMAGE_TYPE_REMAP | SPR_STATION_BASE_D, { 0, 0, height }, { 32, 32, 1 });
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 6, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 26556, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 24 });
                break;
        }
        track_paint_util_onride_photo_paint(session, direction, height + 3, trackElement);
        paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 64, 0x20);
    }
}

/** rct2: 0x00824D8C */
static void lay_down_rc_track_left_eighth_to_diag(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16740, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16744, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16748, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16752, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                }
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                if (direction == 0 || direction == 3)
                {
                    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16741, { 0, 0, height }, { 32, 16, 3 },
                            { 0, 0, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16745, { 0, 0, height }, { 34, 16, 3 },
                            { 0, 0, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16749, { 0, 0, height }, { 32, 16, 3 },
                            { 0, 16, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16753, { 0, 0, height }, { 32, 16, 3 },
                            { 0, 16, height });
                        break;
                }
                paint_util_set_segment_support_height(
                    session,
                    paint_util_rotate_segments(
                        SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                    0xFFFF, 0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16742, { 0, 0, height }, { 16, 16, 3 },
                            { 0, 16, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16746, { 0, 0, height }, { 16, 16, 3 },
                            { 16, 16, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16750, { 0, 0, height }, { 16, 16, 3 },
                            { 16, 0, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16754, { 0, 0, height }, { 16, 16, 3 },
                            { 0, 0, height });
                        break;
                }
                paint_util_set_segment_support_height(
                    session,
                    paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction),
                    0xFFFF, 0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 3:
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 4:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16743, { 0, 0, height }, { 16, 16, 3 },
                            { 16, 16, height });
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16747, { 0, 0, height }, { 16, 18, 3 },
                            { 0, 16, height });
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16751, { 0, 0, height }, { 16, 16, 3 },
                            { 0, 0, height });
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16755, { 0, 0, height }, { 16, 16, 3 },
                            { 16, 0, height });
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
                paint_util_set_segment_support_height(
                    session,
                    paint_util_rotate_segments(
                        SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
        }
    }
    else
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26765, { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26769, { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26773, { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26777, { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                }

                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);

                if (direction == 0 || direction == 3)
                {
                    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
                }
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26766, { 0, 0, height + 24 },
                            { 32, 16, 3 }, { 0, 0, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26770, { 0, 0, height + 24 },
                            { 34, 16, 3 }, { 0, 0, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26774, { 0, 0, height + 24 },
                            { 32, 16, 3 }, { 0, 16, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26778, { 0, 0, height + 24 },
                            { 32, 16, 3 }, { 0, 16, height + 22 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session,
                    paint_util_rotate_segments(
                        SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                    0xFFFF, 0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26767, { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 0, 16, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26771, { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 16, 16, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26775, { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 16, 0, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26779, { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 0, 0, height + 22 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session,
                    paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction),
                    0xFFFF, 0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 3:
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 4:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26768, { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 16, 16, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26772, { 0, 0, height + 24 },
                            { 16, 18, 3 }, { 0, 16, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26776, { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 0, 0, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26780, { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 16, 0, height + 22 });
                        break;
                }

                paint_util_set_segment_support_height(
                    session,
                    paint_util_rotate_segments(
                        SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
        }
    }
}

/** rct2: 0x00824D9C */
static void lay_down_rc_track_right_eighth_to_diag(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16724, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16728, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16732, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16736, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                }
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                if (direction == 0 || direction == 3)
                {
                    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16725, { 0, 0, height }, { 32, 16, 3 },
                            { 0, 16, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16729, { 0, 0, height }, { 32, 16, 3 },
                            { 0, 16, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16733, { 0, 0, height }, { 34, 16, 3 },
                            { 0, 0, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16737, { 0, 0, height }, { 32, 16, 3 },
                            { 0, 0, height });
                        break;
                }
                paint_util_set_segment_support_height(
                    session,
                    paint_util_rotate_segments(
                        SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16726, { 0, 0, height }, { 16, 16, 3 },
                            { 0, 0, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16730, { 0, 0, height }, { 16, 16, 3 },
                            { 16, 0, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16734, { 0, 0, height }, { 28, 28, 3 },
                            { 4, 4, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16738, { 0, 0, height }, { 16, 16, 3 },
                            { 0, 16, height });
                        break;
                }
                paint_util_set_segment_support_height(
                    session,
                    paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction),
                    0xFFFF, 0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 3:
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 4:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16727, { 0, 0, height }, { 16, 16, 3 },
                            { 16, 0, height });
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16731, { 0, 0, height }, { 16, 16, 3 },
                            { 0, 0, height });
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16735, { 0, 0, height }, { 16, 18, 3 },
                            { 0, 16, height });
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16739, { 0, 0, height }, { 16, 16, 3 },
                            { 16, 16, height });
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
                paint_util_set_segment_support_height(
                    session,
                    paint_util_rotate_segments(
                        SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
        }
    }
    else
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26749, { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26753, { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26757, { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26761, { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                }

                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);

                if (direction == 0 || direction == 3)
                {
                    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
                }
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26750, { 0, 0, height + 24 },
                            { 32, 16, 3 }, { 0, 16, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26754, { 0, 0, height + 24 },
                            { 32, 16, 3 }, { 0, 16, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26758, { 0, 0, height + 24 },
                            { 34, 16, 3 }, { 0, 0, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26762, { 0, 0, height + 24 },
                            { 32, 16, 3 }, { 0, 0, height + 22 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session,
                    paint_util_rotate_segments(
                        SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26751, { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 0, 0, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26755, { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 16, 0, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26759, { 0, 0, height + 24 },
                            { 28, 28, 3 }, { 4, 4, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26763, { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 0, 16, height + 22 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session,
                    paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction),
                    0xFFFF, 0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 3:
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 4:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26752, { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 16, 0, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26756, { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 0, 0, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26760, { 0, 0, height + 24 },
                            { 16, 18, 3 }, { 0, 16, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26764, { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 16, 16, height + 22 });
                        break;
                }

                paint_util_set_segment_support_height(
                    session,
                    paint_util_rotate_segments(
                        SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
        }
    }
}

/** rct2: 0x00824DAC */
static void lay_down_rc_track_left_eighth_to_orthogonal(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    lay_down_rc_track_right_eighth_to_diag(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x00824DBC */
static void lay_down_rc_track_right_eighth_to_orthogonal(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    lay_down_rc_track_left_eighth_to_diag(session, ride, trackSequence, (direction + 3) & 3, height, trackElement);
}

/** rct2: 0x00824DCC */
static void lay_down_rc_track_left_eighth_bank_to_diag(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16772, { 0, 0, height }, { 32, 1, 26 },
                            { 0, 27, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16776, { 0, 0, height }, { 32, 1, 26 },
                            { 0, 27, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16780, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16784, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                }
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                if (direction == 0 || direction == 3)
                {
                    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16773, { 0, 0, height }, { 32, 16, 3 },
                            { 0, 0, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16777, { 0, 0, height }, { 34, 16, 0 },
                            { 0, 0, height + 27 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16781, { 0, 0, height }, { 32, 16, 3 },
                            { 0, 16, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16785, { 0, 0, height }, { 32, 16, 3 },
                            { 0, 16, height });
                        break;
                }
                paint_util_set_segment_support_height(
                    session,
                    paint_util_rotate_segments(
                        SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                    0xFFFF, 0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16774, { 0, 0, height }, { 16, 16, 3 },
                            { 0, 16, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16778, { 0, 0, height }, { 16, 16, 0 },
                            { 16, 16, height + 27 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16782, { 0, 0, height }, { 16, 16, 3 },
                            { 16, 0, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16786, { 0, 0, height }, { 16, 16, 3 },
                            { 0, 0, height });
                        break;
                }
                paint_util_set_segment_support_height(
                    session,
                    paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction),
                    0xFFFF, 0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 3:
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 4:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16775, { 0, 0, height }, { 16, 16, 3 },
                            { 16, 16, height });
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16779, { 0, 0, height }, { 16, 18, 0 },
                            { 0, 16, height + 27 });
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16783, { 0, 0, height }, { 16, 16, 3 },
                            { 0, 0, height });
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16787, { 0, 0, height }, { 16, 16, 3 },
                            { 16, 0, height });
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
                paint_util_set_segment_support_height(
                    session,
                    paint_util_rotate_segments(
                        SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
        }
    }
    else
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26853, { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26857, { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26861, { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26865, { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                }

                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF,
                    0);
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);

                if (direction == 0 || direction == 3)
                {
                    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
                }
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26854, { 0, 0, height + 24 },
                            { 32, 16, 3 }, { 0, 0, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26858, { 0, 0, height + 24 },
                            { 34, 16, 3 }, { 0, 0, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26862, { 0, 0, height + 24 },
                            { 32, 16, 3 }, { 0, 16, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26866, { 0, 0, height + 24 },
                            { 32, 16, 3 }, { 0, 16, height + 22 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session,
                    paint_util_rotate_segments(
                        SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                    0xFFFF, 0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26855, { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 0, 16, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26859, { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 16, 16, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26863, { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 16, 0, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26867, { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 0, 0, height + 22 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session,
                    paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction),
                    0xFFFF, 0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 3:
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 4:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26856, { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 16, 16, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26860, { 0, 0, height + 24 },
                            { 16, 18, 3 }, { 0, 16, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26864, { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 0, 0, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26868, { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 16, 0, height + 22 });
                        break;
                }

                paint_util_set_segment_support_height(
                    session,
                    paint_util_rotate_segments(
                        SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
        }
    }
}

/** rct2: 0x00824DDC */
static void lay_down_rc_track_right_eighth_bank_to_diag(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16756, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16760, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16764, { 0, 0, height }, { 32, 1, 26 },
                            { 0, 27, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16768, { 0, 0, height }, { 32, 1, 26 },
                            { 0, 27, height });
                        break;
                }
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                if (direction == 0 || direction == 3)
                {
                    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16757, { 0, 0, height }, { 16, 16, 3 },
                            { 0, 0, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16761, { 0, 0, height }, { 32, 16, 3 },
                            { 0, 16, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16765, { 0, 0, height }, { 34, 16, 0 },
                            { 0, 0, height + 27 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16769, { 0, 0, height }, { 32, 16, 3 },
                            { 0, 0, height });
                        break;
                }
                paint_util_set_segment_support_height(
                    session,
                    paint_util_rotate_segments(
                        SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16758, { 0, 0, height }, { 16, 16, 3 },
                            { 0, 0, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16762, { 0, 0, height }, { 16, 16, 3 },
                            { 16, 0, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16766, { 0, 0, height }, { 28, 28, 0 },
                            { 4, 4, height + 27 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16770, { 0, 0, height }, { 16, 16, 3 },
                            { 0, 16, height });
                        break;
                }
                paint_util_set_segment_support_height(
                    session,
                    paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction),
                    0xFFFF, 0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 3:
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 4:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16759, { 0, 0, height }, { 16, 16, 3 },
                            { 16, 0, height });
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16763, { 0, 0, height }, { 16, 16, 3 },
                            { 0, 0, height });
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16767, { 0, 0, height }, { 16, 18, 0 },
                            { 0, 16, height + 27 });
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16771, { 0, 0, height }, { 16, 16, 3 },
                            { 16, 16, height });
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
                paint_util_set_segment_support_height(
                    session,
                    paint_util_rotate_segments(
                        SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
        }
    }
    else
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26837, { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26841, { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26845, { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26849, { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                }

                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF,
                    0);
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);

                if (direction == 0 || direction == 3)
                {
                    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
                }
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26838, { 0, 0, height + 24 },
                            { 32, 16, 3 }, { 0, 16, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26842, { 0, 0, height + 24 },
                            { 32, 16, 3 }, { 0, 16, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26846, { 0, 0, height + 24 },
                            { 34, 16, 3 }, { 0, 0, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26850, { 0, 0, height + 24 },
                            { 32, 16, 3 }, { 0, 0, height + 22 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session,
                    paint_util_rotate_segments(
                        SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26839, { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 0, 0, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26843, { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 16, 0, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26847, { 0, 0, height + 24 },
                            { 28, 28, 3 }, { 4, 4, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26851, { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 0, 16, height + 22 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session,
                    paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction),
                    0xFFFF, 0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 3:
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 4:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26840, { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 16, 0, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26844, { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 0, 0, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26848, { 0, 0, height + 24 },
                            { 16, 18, 3 }, { 0, 16, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26852, { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 16, 16, height + 22 });
                        break;
                }

                paint_util_set_segment_support_height(
                    session,
                    paint_util_rotate_segments(
                        SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
        }
    }
}

/** rct2: 0x00824DEC */
static void lay_down_rc_track_left_eighth_bank_to_orthogonal(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    lay_down_rc_track_right_eighth_bank_to_diag(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x00824DFC */
static void lay_down_rc_track_right_eighth_bank_to_orthogonal(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    lay_down_rc_track_left_eighth_bank_to_diag(session, ride, trackSequence, (direction + 3) & 3, height, trackElement);
}

/** rct2: 0x00824D7C */
static void lay_down_rc_track_diag_flat(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (trackSequence)
        {
            case 0:
                if (trackElement.HasChain())
                {
                    switch (direction)
                    {
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16699, { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16637, { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 1:
                if (trackElement.HasChain())
                {
                    switch (direction)
                    {
                        case 0:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16696, { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 0:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16634, { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 2:
                if (trackElement.HasChain())
                {
                    switch (direction)
                    {
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16698, { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16636, { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 3:
                if (trackElement.HasChain())
                {
                    switch (direction)
                    {
                        case 0:
                            metal_a_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16697, { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            metal_a_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 2:
                            metal_a_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 3:
                            metal_a_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 0:
                            metal_a_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16635, { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            metal_a_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 2:
                            metal_a_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 3:
                            metal_a_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                    }
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
        }
    }
    else
    {
        switch (trackSequence)
        {
            case 0:
                if (trackElement.HasChain())
                {
                    switch (direction)
                    {
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 26812, { -16, -16, height + 24 },
                                { 32, 32, 3 }, { -16, -16, height + 22 });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 26784, { -16, -16, height + 24 },
                                { 32, 32, 3 }, { -16, -16, height + 22 });
                            break;
                    }
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 1:
                if (trackElement.HasChain())
                {
                    switch (direction)
                    {
                        case 0:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 26809, { -16, -16, height + 24 },
                                { 32, 32, 3 }, { -16, -16, height + 22 });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 0:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 26781, { -16, -16, height + 24 },
                                { 32, 32, 3 }, { -16, -16, height + 22 });
                            break;
                    }
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 2:
                if (trackElement.HasChain())
                {
                    switch (direction)
                    {
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 26811, { -16, -16, height + 24 },
                                { 32, 32, 3 }, { -16, -16, height + 22 });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 26783, { -16, -16, height + 24 },
                                { 32, 32, 3 }, { -16, -16, height + 22 });
                            break;
                    }
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 3:
                if (trackElement.HasChain())
                {
                    switch (direction)
                    {
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 26810, { -16, -16, height + 24 },
                                { 32, 32, 3 }, { -16, -16, height + 22 });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 26782, { -16, -16, height + 24 },
                                { 32, 32, 3 }, { -16, -16, height + 22 });
                            break;
                    }
                }

                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF,
                    0);
                switch (direction)
                {
                    case 0:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
        }
    }
}

/** rct2: 0x00824E2C */
static void lay_down_rc_track_diag_25_deg_up(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (trackSequence)
        {
            case 0:
                if (trackElement.HasChain())
                {
                    switch (direction)
                    {
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16711, { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16649, { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 56, 0x20);
                break;
            case 1:
                if (trackElement.HasChain())
                {
                    switch (direction)
                    {
                        case 0:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16708, { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 0:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16646, { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 56, 0x20);
                break;
            case 2:
                if (trackElement.HasChain())
                {
                    switch (direction)
                    {
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16710, { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16648, { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 56, 0x20);
                break;
            case 3:
                if (trackElement.HasChain())
                {
                    switch (direction)
                    {
                        case 0:
                            metal_b_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 1, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16709, { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            metal_b_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 0, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 2:
                            metal_b_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 2, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 3:
                            metal_b_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 3, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 0:
                            metal_b_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 1, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16647, { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            metal_b_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 0, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 2:
                            metal_b_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 2, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 3:
                            metal_b_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 3, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                    }
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 56, 0x20);
                break;
        }
    }
    else
    {
        switch (trackSequence)
        {
            case 0:
                if (trackElement.HasChain())
                {
                    switch (direction)
                    {
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 26824, { -16, -16, height + 24 },
                                { 32, 32, 3 }, { -16, -16, height + 40 });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 26796, { -16, -16, height + 24 },
                                { 32, 32, 3 }, { -16, -16, height + 40 });
                            break;
                    }
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 56, 0x20);
                break;
            case 1:
                if (trackElement.HasChain())
                {
                    switch (direction)
                    {
                        case 0:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 26821, { -16, -16, height + 24 },
                                { 32, 32, 3 }, { -16, -16, height + 40 });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 0:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 26793, { -16, -16, height + 24 },
                                { 32, 32, 3 }, { -16, -16, height + 40 });
                            break;
                    }
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 56, 0x20);
                break;
            case 2:
                if (trackElement.HasChain())
                {
                    switch (direction)
                    {
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 26823, { -16, -16, height + 24 },
                                { 32, 32, 3 }, { -16, -16, height + 40 });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 26795, { -16, -16, height + 24 },
                                { 32, 32, 3 }, { -16, -16, height + 40 });
                            break;
                    }
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 56, 0x20);
                break;
            case 3:
                if (trackElement.HasChain())
                {
                    switch (direction)
                    {
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 26822, { -16, -16, height + 24 },
                                { 32, 32, 3 }, { -16, -16, height + 40 });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 26794, { -16, -16, height + 24 },
                                { 32, 32, 3 }, { -16, -16, height + 40 });
                            break;
                    }
                }

                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF,
                    0);
                switch (direction)
                {
                    case 0:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height + 45, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height + 45, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height + 45, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height + 45, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                paint_util_set_general_support_height(session, height + 56, 0x20);
                break;
        }
    }
}

/** rct2: 0x00824E8C */
static void lay_down_rc_track_diag_60_deg_up(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (trackSequence)
        {
            case 0:
                if (trackElement.HasChain())
                {
                    switch (direction)
                    {
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16723, { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16661, { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 104, 0x20);
                break;
            case 1:
                if (trackElement.HasChain())
                {
                    switch (direction)
                    {
                        case 0:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16720, { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 0:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16658, { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 104, 0x20);
                break;
            case 2:
                if (trackElement.HasChain())
                {
                    switch (direction)
                    {
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16722, { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16660, { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 104, 0x20);
                break;
            case 3:
                if (trackElement.HasChain())
                {
                    switch (direction)
                    {
                        case 0:
                            metal_b_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 1, 32, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16721, { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            metal_b_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 0, 36, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 2:
                            metal_b_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 2, 32, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 3:
                            metal_b_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 3, 36, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 0:
                            metal_b_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 1, 32, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16659, { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            metal_b_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 0, 36, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 2:
                            metal_b_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 2, 32, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 3:
                            metal_b_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 3, 36, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                    }
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 104, 0x20);
                break;
        }
    }
    else
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26808, { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 88 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 104, 0x20);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26805, { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 88 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 104, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26807, { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 88 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 104, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26806, { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 24 });
                        break;
                }

                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF,
                    0);
                switch (direction)
                {
                    case 0:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 32, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 36, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 32, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 36, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                paint_util_set_general_support_height(session, height + 104, 0x20);
                break;
        }
    }
}

/** rct2: 0x00824E0C */
static void lay_down_rc_track_diag_flat_to_25_deg_up(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (trackSequence)
        {
            case 0:
                if (trackElement.HasChain())
                {
                    switch (direction)
                    {
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16703, { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16641, { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 48, 0x20);
                break;
            case 1:
                if (trackElement.HasChain())
                {
                    switch (direction)
                    {
                        case 0:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16700, { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 0:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16638, { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 48, 0x20);
                break;
            case 2:
                if (trackElement.HasChain())
                {
                    switch (direction)
                    {
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16702, { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16640, { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 48, 0x20);
                break;
            case 3:
                if (trackElement.HasChain())
                {
                    switch (direction)
                    {
                        case 0:
                            metal_b_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16701, { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            metal_b_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 2:
                            metal_b_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 3:
                            metal_b_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 0:
                            metal_b_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16639, { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            metal_b_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 2:
                            metal_b_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 3:
                            metal_b_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                    }
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 48, 0x20);
                break;
        }
    }
    else
    {
        switch (trackSequence)
        {
            case 0:
                if (trackElement.HasChain())
                {
                    switch (direction)
                    {
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 26816, { -16, -16, height + 24 },
                                { 32, 32, 3 }, { -16, -16, height + 32 });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 26788, { -16, -16, height + 24 },
                                { 32, 32, 3 }, { -16, -16, height + 32 });
                            break;
                    }
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 48, 0x20);
                break;
            case 1:
                if (trackElement.HasChain())
                {
                    switch (direction)
                    {
                        case 0:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 26813, { -16, -16, height + 24 },
                                { 32, 32, 3 }, { -16, -16, height + 32 });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 0:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 26785, { -16, -16, height + 24 },
                                { 32, 32, 3 }, { -16, -16, height + 32 });
                            break;
                    }
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 48, 0x20);
                break;
            case 2:
                if (trackElement.HasChain())
                {
                    switch (direction)
                    {
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 26815, { -16, -16, height + 24 },
                                { 32, 32, 3 }, { -16, -16, height + 32 });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 26787, { -16, -16, height + 24 },
                                { 32, 32, 3 }, { -16, -16, height + 32 });
                            break;
                    }
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 48, 0x20);
                break;
            case 3:
                if (trackElement.HasChain())
                {
                    switch (direction)
                    {
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 26814, { -16, -16, height + 24 },
                                { 32, 32, 3 }, { -16, -16, height + 32 });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 26786, { -16, -16, height + 24 },
                                { 32, 32, 3 }, { -16, -16, height + 32 });
                            break;
                    }
                }

                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF,
                    0);
                switch (direction)
                {
                    case 0:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height + 37, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height + 37, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height + 37, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height + 37, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                paint_util_set_general_support_height(session, height + 48, 0x20);
                break;
        }
    }
}

/** rct2: 0x00824E6C */
static void lay_down_rc_track_diag_25_deg_up_to_60_deg_up(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (trackSequence)
        {
            case 0:
                if (trackElement.HasChain())
                {
                    switch (direction)
                    {
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16715, { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16653, { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 72, 0x20);
                break;
            case 1:
                if (trackElement.HasChain())
                {
                    switch (direction)
                    {
                        case 0:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16712, { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 0:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16650, { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 72, 0x20);
                break;
            case 2:
                if (trackElement.HasChain())
                {
                    switch (direction)
                    {
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16714, { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16652, { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 72, 0x20);
                break;
            case 3:
                if (trackElement.HasChain())
                {
                    switch (direction)
                    {
                        case 0:
                            metal_b_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 1, 16, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16713, { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            metal_b_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 0, 16, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 2:
                            metal_b_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 2, 16, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 3:
                            metal_b_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 3, 16, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 0:
                            metal_b_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 1, 16, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16651, { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            metal_b_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 0, 16, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 2:
                            metal_b_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 2, 16, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 3:
                            metal_b_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 3, 16, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                    }
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 72, 0x20);
                break;
        }
    }
    else
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26800, { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 56 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 72, 0x20);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26797, { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 56 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 72, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26799, { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 56 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 72, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26798, { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 56 });
                        break;
                }

                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF,
                    0);
                switch (direction)
                {
                    case 0:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 16, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 16, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 16, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 16, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                paint_util_set_general_support_height(session, height + 72, 0x20);
                break;
        }
    }
}

/** rct2: 0x00824E7C */
static void lay_down_rc_track_diag_60_deg_up_to_25_deg_up(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (trackSequence)
        {
            case 0:
                if (trackElement.HasChain())
                {
                    switch (direction)
                    {
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16719, { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16657, { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 72, 0x20);
                break;
            case 1:
                if (trackElement.HasChain())
                {
                    switch (direction)
                    {
                        case 0:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16716, { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 0:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16654, { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 72, 0x20);
                break;
            case 2:
                if (trackElement.HasChain())
                {
                    switch (direction)
                    {
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16718, { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16656, { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 72, 0x20);
                break;
            case 3:
                if (trackElement.HasChain())
                {
                    switch (direction)
                    {
                        case 0:
                            metal_b_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 1, 21, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16717, { -16, -16, height },
                                { 16, 16, 3 }, { 0, 0, height });
                            metal_b_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 0, 21, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 2:
                            metal_b_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 2, 21, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 3:
                            metal_b_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 3, 21, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 0:
                            metal_b_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 1, 21, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16655, { -16, -16, height },
                                { 16, 16, 3 }, { 0, 0, height });
                            metal_b_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 0, 21, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 2:
                            metal_b_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 2, 21, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 3:
                            metal_b_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 3, 21, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                    }
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 72, 0x20);
                break;
        }
    }
    else
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26804, { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 56 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 72, 0x20);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26801, { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 56 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 72, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26803, { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 56 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 72, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26802, { -16, -16, height + 24 },
                            { 16, 16, 3 }, { 0, 0, height + 56 });
                        break;
                }

                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF,
                    0);
                switch (direction)
                {
                    case 0:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 21, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 21, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 21, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 21, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                paint_util_set_general_support_height(session, height + 72, 0x20);
                break;
        }
    }
}

/** rct2: 0x00824E1C */
static void lay_down_rc_track_diag_25_deg_up_to_flat(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (trackSequence)
        {
            case 0:
                if (trackElement.HasChain())
                {
                    switch (direction)
                    {
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16707, { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16645, { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 56, 0x20);
                break;
            case 1:
                if (trackElement.HasChain())
                {
                    switch (direction)
                    {
                        case 0:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16704, { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 0:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16642, { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 56, 0x20);
                break;
            case 2:
                if (trackElement.HasChain())
                {
                    switch (direction)
                    {
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16706, { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16644, { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 56, 0x20);
                break;
            case 3:
                if (trackElement.HasChain())
                {
                    switch (direction)
                    {
                        case 0:
                            metal_b_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 1, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16705, { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            metal_b_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 0, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 2:
                            metal_b_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 2, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 3:
                            metal_b_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 3, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 0:
                            metal_b_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 1, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16643, { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            metal_b_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 0, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 2:
                            metal_b_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 2, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 3:
                            metal_b_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 3, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                    }
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 56, 0x20);
                break;
        }
    }
    else
    {
        switch (trackSequence)
        {
            case 0:
                if (trackElement.HasChain())
                {
                    switch (direction)
                    {
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 26820, { -16, -16, height + 24 },
                                { 32, 32, 3 }, { -16, -16, height + 32 });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 26792, { -16, -16, height + 24 },
                                { 32, 32, 3 }, { -16, -16, height + 32 });
                            break;
                    }
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 56, 0x20);
                break;
            case 1:
                if (trackElement.HasChain())
                {
                    switch (direction)
                    {
                        case 0:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 26817, { -16, -16, height + 24 },
                                { 32, 32, 3 }, { -16, -16, height + 32 });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 0:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 26789, { -16, -16, height + 24 },
                                { 32, 32, 3 }, { -16, -16, height + 32 });
                            break;
                    }
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 56, 0x20);
                break;
            case 2:
                if (trackElement.HasChain())
                {
                    switch (direction)
                    {
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 26819, { -16, -16, height + 24 },
                                { 32, 32, 3 }, { -16, -16, height + 32 });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 26791, { -16, -16, height + 24 },
                                { 32, 32, 3 }, { -16, -16, height + 32 });
                            break;
                    }
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 56, 0x20);
                break;
            case 3:
                if (trackElement.HasChain())
                {
                    switch (direction)
                    {
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 26818, { -16, -16, height + 24 },
                                { 32, 32, 3 }, { -16, -16, height + 32 });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 26790, { -16, -16, height + 24 },
                                { 32, 32, 3 }, { -16, -16, height + 32 });
                            break;
                    }
                }

                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF,
                    0);
                switch (direction)
                {
                    case 0:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                paint_util_set_general_support_height(session, height + 56, 0x20);
                break;
        }
    }
}

/** rct2: 0x00824E5C */
static void lay_down_rc_track_diag_25_deg_down(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (trackSequence)
        {
            case 0:
                if (trackElement.HasChain())
                {
                    switch (direction)
                    {
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16709, { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16647, { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 56, 0x20);
                break;
            case 1:
                if (trackElement.HasChain())
                {
                    switch (direction)
                    {
                        case 0:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16710, { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 0:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16648, { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 56, 0x20);
                break;
            case 2:
                if (trackElement.HasChain())
                {
                    switch (direction)
                    {
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16708, { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16646, { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 56, 0x20);
                break;
            case 3:
                if (trackElement.HasChain())
                {
                    switch (direction)
                    {
                        case 0:
                            metal_b_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 1, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16711, { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            metal_b_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 0, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 2:
                            metal_b_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 2, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 3:
                            metal_b_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 3, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 0:
                            metal_b_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 1, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16649, { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            metal_b_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 0, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 2:
                            metal_b_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 2, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 3:
                            metal_b_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 3, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                    }
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 56, 0x20);
                break;
        }
    }
    else
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26794, { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 40 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 56, 0x20);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26795, { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 40 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 56, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26793, { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 40 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 56, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26796, { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 40 });
                        break;
                }

                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF,
                    0);
                switch (direction)
                {
                    case 0:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height + 45, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height + 45, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height + 45, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height + 45, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                paint_util_set_general_support_height(session, height + 56, 0x20);
                break;
        }
    }
}

/** rct2: 0x00824EBC */
static void lay_down_rc_track_diag_60_deg_down(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (trackSequence)
        {
            case 0:
                if (trackElement.HasChain())
                {
                    switch (direction)
                    {
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16721, { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16659, { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 104, 0x20);
                break;
            case 1:
                if (trackElement.HasChain())
                {
                    switch (direction)
                    {
                        case 0:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16722, { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 0:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16660, { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 104, 0x20);
                break;
            case 2:
                if (trackElement.HasChain())
                {
                    switch (direction)
                    {
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16720, { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16658, { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 104, 0x20);
                break;
            case 3:
                if (trackElement.HasChain())
                {
                    switch (direction)
                    {
                        case 0:
                            metal_b_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 1, 24, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16723, { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            metal_b_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 0, 28, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 2:
                            metal_b_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 2, 24, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 3:
                            metal_b_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 3, 28, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 0:
                            metal_b_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 1, 24, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16661, { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            metal_b_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 0, 28, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 2:
                            metal_b_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 2, 24, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 3:
                            metal_b_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 3, 28, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                    }
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 104, 0x20);
                break;
        }
    }
    else
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26806, { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 24 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 104, 0x20);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26807, { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 88 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 104, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26805, { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 88 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 104, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26808, { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 88 });
                        break;
                }

                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF,
                    0);
                switch (direction)
                {
                    case 0:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 24, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 28, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 24, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 28, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                paint_util_set_general_support_height(session, height + 104, 0x20);
                break;
        }
    }
}

/** rct2: 0x00824E3C */
static void lay_down_rc_track_diag_flat_to_25_deg_down(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (trackSequence)
        {
            case 0:
                if (trackElement.HasChain())
                {
                    switch (direction)
                    {
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16705, { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16643, { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF,
                    0);
                break;
            case 1:
                if (trackElement.HasChain())
                {
                    switch (direction)
                    {
                        case 0:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16706, { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 0:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16644, { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF,
                    0);
                break;
            case 2:
                if (trackElement.HasChain())
                {
                    switch (direction)
                    {
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16704, { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16642, { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF,
                    0);
                break;
            case 3:
                if (trackElement.HasChain())
                {
                    switch (direction)
                    {
                        case 0:
                            metal_b_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 1, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16707, { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            metal_b_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 0, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 2:
                            metal_b_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 2, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 3:
                            metal_b_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 3, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 0:
                            metal_b_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 1, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16645, { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            metal_b_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 0, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 2:
                            metal_b_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 2, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 3:
                            metal_b_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 3, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                    }
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF,
                    0);
                break;
        }
    }
    else
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26790, { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 32 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF,
                    0);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26791, { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 32 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF,
                    0);
                break;
            case 2:
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26789, { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 32 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF,
                    0);
                break;
            case 3:
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26792, { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 32 });
                        break;
                }

                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF,
                    0);
                switch (direction)
                {
                    case 0:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
                break;
        }
    }

    paint_util_set_general_support_height(session, height + 56, 0x20);
}

/** rct2: 0x00824E9C */
static void lay_down_rc_track_diag_25_deg_down_to_60_deg_down(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (trackSequence)
        {
            case 0:
                if (trackElement.HasChain())
                {
                    switch (direction)
                    {
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16717, { -16, -16, height },
                                { 16, 16, 3 }, { 0, 0, height });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16655, { -16, -16, height },
                                { 16, 16, 3 }, { 0, 0, height });
                            break;
                    }
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 72, 0x20);
                break;
            case 1:
                if (trackElement.HasChain())
                {
                    switch (direction)
                    {
                        case 0:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16718, { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 0:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16656, { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 72, 0x20);
                break;
            case 2:
                if (trackElement.HasChain())
                {
                    switch (direction)
                    {
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16716, { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16654, { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 72, 0x20);
                break;
            case 3:
                if (trackElement.HasChain())
                {
                    switch (direction)
                    {
                        case 0:
                            metal_b_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 1, 17, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16719, { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            metal_b_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 0, 17, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 2:
                            metal_b_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 2, 17, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 3:
                            metal_b_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 3, 17, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 0:
                            metal_b_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 1, 17, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16657, { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            metal_b_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 0, 17, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 2:
                            metal_b_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 2, 17, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 3:
                            metal_b_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 3, 17, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                    }
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 72, 0x20);
                break;
        }
    }
    else
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26802, { -16, -16, height + 24 },
                            { 16, 16, 3 }, { 0, 0, height + 56 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 72, 0x20);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26803, { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 56 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 72, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26801, { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 56 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 72, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26804, { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 56 });
                        break;
                }

                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF,
                    0);
                switch (direction)
                {
                    case 0:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 17, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 17, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 17, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 17, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                paint_util_set_general_support_height(session, height + 72, 0x20);
                break;
        }
    }
}

/** rct2: 0x00824EAC */
static void lay_down_rc_track_diag_60_deg_down_to_25_deg_down(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (trackSequence)
        {
            case 0:
                if (trackElement.HasChain())
                {
                    switch (direction)
                    {
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16713, { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16651, { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 72, 0x20);
                break;
            case 1:
                if (trackElement.HasChain())
                {
                    switch (direction)
                    {
                        case 0:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16714, { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 0:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16652, { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 72, 0x20);
                break;
            case 2:
                if (trackElement.HasChain())
                {
                    switch (direction)
                    {
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16712, { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16650, { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 72, 0x20);
                break;
            case 3:
                if (trackElement.HasChain())
                {
                    switch (direction)
                    {
                        case 0:
                            metal_b_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 1, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16715, { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            metal_b_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 0, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 2:
                            metal_b_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 2, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 3:
                            metal_b_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 3, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 0:
                            metal_b_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 1, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16653, { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            metal_b_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 0, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 2:
                            metal_b_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 2, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 3:
                            metal_b_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 3, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                    }
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 72, 0x20);
                break;
        }
    }
    else
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26798, { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 56 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 72, 0x20);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26799, { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 56 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 72, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26797, { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 56 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 72, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26800, { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 56 });
                        break;
                }

                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF,
                    0);
                switch (direction)
                {
                    case 0:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 8, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 8, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 8, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 8, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                paint_util_set_general_support_height(session, height + 72, 0x20);
                break;
        }
    }
}

/** rct2: 0x00824E4C */
static void lay_down_rc_track_diag_25_deg_down_to_flat(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (trackSequence)
        {
            case 0:
                if (trackElement.HasChain())
                {
                    switch (direction)
                    {
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16701, { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 3:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16639, { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 48, 0x20);
                break;
            case 1:
                if (trackElement.HasChain())
                {
                    switch (direction)
                    {
                        case 0:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16702, { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 0:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16640, { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 48, 0x20);
                break;
            case 2:
                if (trackElement.HasChain())
                {
                    switch (direction)
                    {
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16700, { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 2:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16638, { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            break;
                    }
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 48, 0x20);
                break;
            case 3:
                if (trackElement.HasChain())
                {
                    switch (direction)
                    {
                        case 0:
                            metal_b_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16703, { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            metal_b_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 2:
                            metal_b_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 3:
                            metal_b_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                    }
                }
                else
                {
                    switch (direction)
                    {
                        case 0:
                            metal_b_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 1:
                            PaintAddImageAsParentRotated(
                                session, direction, session.TrackColours[SCHEME_TRACK] | 16641, { -16, -16, height },
                                { 32, 32, 3 }, { -16, -16, height });
                            metal_b_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 2:
                            metal_b_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                        case 3:
                            metal_b_supports_paint_setup(
                                session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                            break;
                    }
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 48, 0x20);
                break;
        }
    }
    else
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26786, { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 32 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 48, 0x20);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26787, { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 32 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 48, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26785, { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 32 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 48, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26788, { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 32 });
                        break;
                }

                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF,
                    0);
                switch (direction)
                {
                    case 0:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height + 37, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height + 37, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height + 37, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height + 37, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                paint_util_set_general_support_height(session, height + 48, 0x20);
                break;
        }
    }
}

/** rct2: 0x00824EEC */
static void lay_down_rc_track_diag_flat_to_left_bank(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16669, { -16, -16, height }, { 32, 32, 3 },
                            { -16, -16, height });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16666, { -16, -16, height }, { 32, 32, 3 },
                            { -16, -16, height });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16670, { -16, -16, height }, { 32, 32, 0 },
                            { -16, -16, height + 27 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16668, { -16, -16, height }, { 32, 32, 3 },
                            { -16, -16, height });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 0:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16667, { -16, -16, height }, { 32, 32, 3 },
                            { -16, -16, height });
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
        }
    }
    else
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26876, { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 22 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26873, { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 22 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26875, { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 22 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26874, { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 22 });
                        break;
                }

                paint_util_set_segment_support_height(
                    session,
                    paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction),
                    0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
        }
    }
}

/** rct2: 0x00824EFC */
static void lay_down_rc_track_diag_flat_to_right_bank(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16674, { -16, -16, height }, { 32, 32, 3 },
                            { -16, -16, height });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16671, { -16, -16, height }, { 32, 32, 3 },
                            { -16, -16, height });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16673, { -16, -16, height }, { 32, 32, 3 },
                            { -16, -16, height });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16675, { -16, -16, height }, { 32, 32, 0 },
                            { -16, -16, height + 27 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 0:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16672, { -16, -16, height }, { 32, 32, 3 },
                            { -16, -16, height });
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
        }
    }
    else
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26880, { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 22 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26877, { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 22 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26879, { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 22 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26878, { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 22 });
                        break;
                }

                paint_util_set_segment_support_height(
                    session,
                    paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction),
                    0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
        }
    }
}

/** rct2: 0x00824F0C */
static void lay_down_rc_track_diag_left_bank_to_flat(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16672, { -16, -16, height }, { 32, 32, 3 },
                            { -16, -16, height });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16673, { -16, -16, height }, { 32, 32, 3 },
                            { -16, -16, height });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16675, { -16, -16, height }, { 32, 32, 0 },
                            { -16, -16, height + 27 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16671, { -16, -16, height }, { 32, 32, 3 },
                            { -16, -16, height });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 0:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16674, { -16, -16, height }, { 32, 32, 3 },
                            { -16, -16, height });
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
        }
    }
    else
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26878, { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 22 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26879, { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 22 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26877, { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 22 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26880, { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 22 });
                        break;
                }

                paint_util_set_segment_support_height(
                    session,
                    paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction),
                    0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
        }
    }
}

/** rct2: 0x00824F1C */
static void lay_down_rc_track_diag_right_bank_to_flat(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16667, { -16, -16, height }, { 32, 32, 3 },
                            { -16, -16, height });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16668, { -16, -16, height }, { 32, 32, 3 },
                            { -16, -16, height });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16666, { -16, -16, height }, { 32, 32, 3 },
                            { -16, -16, height });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16670, { -16, -16, height }, { 32, 32, 0 },
                            { -16, -16, height + 27 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 0:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16669, { -16, -16, height }, { 32, 32, 3 },
                            { -16, -16, height });
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
        }
    }
    else
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26874, { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 22 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26875, { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 22 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26873, { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 22 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26876, { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 22 });
                        break;
                }

                paint_util_set_segment_support_height(
                    session,
                    paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction),
                    0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
        }
    }
}

/** rct2: 0x00824F4C */
static void lay_down_rc_track_diag_left_bank_to_25_deg_up(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16689, { -16, -16, height }, { 32, 32, 3 },
                            { -16, -16, height });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 48, 0x20);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16686, { -16, -16, height }, { 32, 32, 3 },
                            { -16, -16, height });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16690, { -16, -16, height }, { 32, 32, 0 },
                            { -16, -16, height + 35 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 48, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16688, { -16, -16, height }, { 32, 32, 3 },
                            { -16, -16, height });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 48, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 0:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16687, { -16, -16, height }, { 32, 32, 3 },
                            { -16, -16, height });
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 48, 0x20);
                break;
        }
    }
    else
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26892, { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 32 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 48, 0x20);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26889, { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 32 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 48, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26891, { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 32 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 48, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26890, { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 32 });
                        break;
                }

                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF,
                    0);
                switch (direction)
                {
                    case 0:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height + 37, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height + 37, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height + 37, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height + 37, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                paint_util_set_general_support_height(session, height + 48, 0x20);
                break;
        }
    }
}

/** rct2: 0x00824F5C */
static void lay_down_rc_track_diag_right_bank_to_25_deg_up(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16694, { -16, -16, height }, { 32, 32, 3 },
                            { -16, -16, height });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 48, 0x20);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16691, { -16, -16, height }, { 32, 32, 3 },
                            { -16, -16, height });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 48, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16693, { -16, -16, height }, { 32, 32, 3 },
                            { -16, -16, height });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16695, { -16, -16, height }, { 32, 32, 0 },
                            { -16, -16, height + 35 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 48, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 0:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16692, { -16, -16, height }, { 32, 32, 3 },
                            { -16, -16, height });
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 48, 0x20);
                break;
        }
    }
    else
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26896, { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 32 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 48, 0x20);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26893, { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 32 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 48, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26895, { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 32 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 48, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26894, { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 32 });
                        break;
                }

                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF,
                    0);
                switch (direction)
                {
                    case 0:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height + 37, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height + 37, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height + 37, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height + 37, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                paint_util_set_general_support_height(session, height + 48, 0x20);
                break;
        }
    }
}

/** rct2: 0x00824F2C */
static void lay_down_rc_track_diag_25_deg_up_to_left_bank(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16679, { -16, -16, height }, { 32, 32, 3 },
                            { -16, -16, height });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 56, 0x20);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16676, { -16, -16, height }, { 32, 32, 3 },
                            { -16, -16, height });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16680, { -16, -16, height }, { 32, 32, 0 },
                            { -16, -16, height + 35 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 56, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16678, { -16, -16, height }, { 32, 32, 3 },
                            { -16, -16, height });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 56, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 0:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16677, { -16, -16, height }, { 32, 32, 3 },
                            { -16, -16, height });
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 56, 0x20);
                break;
        }
    }
    else
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26884, { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 32 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 56, 0x20);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26881, { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 32 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 56, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26883, { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 32 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 56, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26882, { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 32 });
                        break;
                }

                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF,
                    0);
                switch (direction)
                {
                    case 0:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                paint_util_set_general_support_height(session, height + 56, 0x20);
                break;
        }
    }
}

/** rct2: 0x00824F3C */
static void lay_down_rc_track_diag_25_deg_up_to_right_bank(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16684, { -16, -16, height }, { 32, 32, 3 },
                            { -16, -16, height });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 56, 0x20);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16681, { -16, -16, height }, { 32, 32, 3 },
                            { -16, -16, height });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 56, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16683, { -16, -16, height }, { 32, 32, 3 },
                            { -16, -16, height });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16685, { -16, -16, height }, { 32, 32, 0 },
                            { -16, -16, height + 35 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 56, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 0:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16682, { -16, -16, height }, { 32, 32, 3 },
                            { -16, -16, height });
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 56, 0x20);
                break;
        }
    }
    else
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26888, { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 32 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 56, 0x20);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26885, { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 32 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 56, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26887, { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 32 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 56, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26886, { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 32 });
                        break;
                }

                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF,
                    0);
                switch (direction)
                {
                    case 0:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                paint_util_set_general_support_height(session, height + 56, 0x20);
                break;
        }
    }
}

/** rct2: 0x00824F6C */
static void lay_down_rc_track_diag_left_bank_to_25_deg_down(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16682, { -16, -16, height }, { 32, 32, 3 },
                            { -16, -16, height });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF,
                    0);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16683, { -16, -16, height }, { 32, 32, 3 },
                            { -16, -16, height });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16685, { -16, -16, height }, { 32, 32, 0 },
                            { -16, -16, height + 35 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF,
                    0);
                break;
            case 2:
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16681, { -16, -16, height }, { 32, 32, 3 },
                            { -16, -16, height });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF,
                    0);
                break;
            case 3:
                switch (direction)
                {
                    case 0:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16684, { -16, -16, height }, { 32, 32, 3 },
                            { -16, -16, height });
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF,
                    0);
                break;
        }
    }
    else
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26886, { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 32 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF,
                    0);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26887, { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 32 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF,
                    0);
                break;
            case 2:
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26885, { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 32 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF,
                    0);
                break;
            case 3:
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26888, { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 32 });
                        break;
                }

                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF,
                    0);
                switch (direction)
                {
                    case 0:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
                break;
        }
    }

    paint_util_set_general_support_height(session, height + 56, 0x20);
}

/** rct2: 0x00824F7C */
static void lay_down_rc_track_diag_right_bank_to_25_deg_down(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16677, { -16, -16, height }, { 32, 32, 3 },
                            { -16, -16, height });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF,
                    0);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16678, { -16, -16, height }, { 32, 32, 3 },
                            { -16, -16, height });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF,
                    0);
                break;
            case 2:
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16676, { -16, -16, height }, { 32, 32, 3 },
                            { -16, -16, height });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16680, { -16, -16, height }, { 32, 32, 0 },
                            { -16, -16, height + 35 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF,
                    0);
                break;
            case 3:
                switch (direction)
                {
                    case 0:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16679, { -16, -16, height }, { 32, 32, 3 },
                            { -16, -16, height });
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF,
                    0);
                break;
        }
    }
    else
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26882, { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 32 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF,
                    0);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26883, { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 32 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF,
                    0);
                break;
            case 2:
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26881, { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 32 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF,
                    0);
                break;
            case 3:
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26884, { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 32 });
                        break;
                }

                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF,
                    0);
                switch (direction)
                {
                    case 0:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
                break;
        }
    }

    paint_util_set_general_support_height(session, height + 56, 0x20);
}

/** rct2: 0x00824F8C */
static void lay_down_rc_track_diag_25_deg_down_to_left_bank(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16692, { -16, -16, height }, { 32, 32, 3 },
                            { -16, -16, height });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 48, 0x20);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16693, { -16, -16, height }, { 32, 32, 3 },
                            { -16, -16, height });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16695, { -16, -16, height }, { 32, 32, 0 },
                            { -16, -16, height + 35 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 48, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16691, { -16, -16, height }, { 32, 32, 3 },
                            { -16, -16, height });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 48, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 0:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16694, { -16, -16, height }, { 32, 32, 3 },
                            { -16, -16, height });
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 48, 0x20);
                break;
        }
    }
    else
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26894, { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 32 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 48, 0x20);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26895, { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 32 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 48, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26893, { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 32 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 48, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26896, { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 32 });
                        break;
                }

                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF,
                    0);
                switch (direction)
                {
                    case 0:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height + 37, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height + 37, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height + 37, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height + 37, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                paint_util_set_general_support_height(session, height + 48, 0x20);
                break;
        }
    }
}

/** rct2: 0x00824F9C */
static void lay_down_rc_track_diag_25_deg_down_to_right_bank(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16687, { -16, -16, height }, { 32, 32, 3 },
                            { -16, -16, height });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 48, 0x20);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16688, { -16, -16, height }, { 32, 32, 3 },
                            { -16, -16, height });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 48, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16686, { -16, -16, height }, { 32, 32, 3 },
                            { -16, -16, height });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16690, { -16, -16, height }, { 32, 32, 0 },
                            { -16, -16, height + 35 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 48, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 0:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16689, { -16, -16, height }, { 32, 32, 3 },
                            { -16, -16, height });
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 48, 0x20);
                break;
        }
    }
    else
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26890, { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 32 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 48, 0x20);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26891, { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 32 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 48, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26889, { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 32 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 48, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26892, { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 32 });
                        break;
                }

                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF,
                    0);
                switch (direction)
                {
                    case 0:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height + 37, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height + 37, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height + 37, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height + 37, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                paint_util_set_general_support_height(session, height + 48, 0x20);
                break;
        }
    }
}

/** rct2: 0x00824ECC */
static void lay_down_rc_track_diag_left_bank(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16665, { -16, -16, height }, { 32, 32, 3 },
                            { -16, -16, height });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16662, { -16, -16, height }, { 32, 32, 0 },
                            { -16, -16, height + 27 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16664, { -16, -16, height }, { 32, 32, 3 },
                            { -16, -16, height });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 0:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16663, { -16, -16, height }, { 32, 32, 3 },
                            { -16, -16, height });
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
        }
    }
    else
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26872, { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 22 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26869, { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 22 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26871, { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 22 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26870, { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 22 });
                        break;
                }

                paint_util_set_segment_support_height(
                    session,
                    paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction),
                    0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
        }
    }
}

/** rct2: 0x00824EDC */
static void lay_down_rc_track_diag_right_bank(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16663, { -16, -16, height }, { 32, 32, 3 },
                            { -16, -16, height });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16664, { -16, -16, height }, { 32, 32, 3 },
                            { -16, -16, height });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16662, { -16, -16, height }, { 32, 32, 0 },
                            { -16, -16, height + 27 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 0:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 16665, { -16, -16, height }, { 32, 32, 3 },
                            { -16, -16, height });
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
        }
    }
    else
    {
        switch (trackSequence)
        {
            case 0:
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26870, { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 22 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26871, { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 22 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26869, { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 22 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 26872, { -16, -16, height + 24 },
                            { 32, 32, 3 }, { -16, -16, height + 22 });
                        break;
                }

                paint_util_set_segment_support_height(
                    session,
                    paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction),
                    0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
        }
    }
}

/** rct2: 0x00824FAC */
static void lay_down_rc_track_left_flyer_twist_up(
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 26972, { 0, 6, height }, { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 26978, { 0, 6, height }, { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 26971, { 0, 6, height }, { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 26977, { 0, 6, height }, { 32, 20, 3 });
                    break;
            }
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
            }
            paint_util_set_segment_support_height(
                session,
                paint_util_rotate_segments(
                    SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 26973, { 0, 6, height + 24 }, { 32, 20, 3 },
                        { 0, 6, height + 24 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 26979, { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 26970, { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 26976, { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
            }
            paint_util_set_segment_support_height(
                session,
                paint_util_rotate_segments(
                    SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 26974, { 0, 6, height + 24 }, { 32, 20, 3 },
                        { 0, 6, height + 24 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 26980, { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 26969, { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 26975, { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
            }

            paint_util_set_segment_support_height(
                session,
                paint_util_rotate_segments(
                    SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);

            switch (direction)
            {
                case 1:
                    paint_util_push_tunnel_right(session, height, TUNNEL_0);
                    break;
                case 2:
                    paint_util_push_tunnel_left(session, height, TUNNEL_0);
                    break;
            }
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x00824FBC */
static void lay_down_rc_track_right_flyer_twist_up(
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 26984, { 0, 6, height }, { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 26990, { 0, 6, height }, { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 26983, { 0, 6, height }, { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 26989, { 0, 6, height }, { 32, 20, 3 });
                    break;
            }
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
            }
            paint_util_set_segment_support_height(
                session,
                paint_util_rotate_segments(
                    SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 26985, { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 26991, { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 26982, { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 26988, { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
            }
            paint_util_set_segment_support_height(
                session,
                paint_util_rotate_segments(
                    SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 26986, { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 26992, { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 26981, { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 26987, { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
            }

            paint_util_set_segment_support_height(
                session,
                paint_util_rotate_segments(
                    SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);

            switch (direction)
            {
                case 1:
                    paint_util_push_tunnel_right(session, height, TUNNEL_0);
                    break;
                case 2:
                    paint_util_push_tunnel_left(session, height, TUNNEL_0);
                    break;
            }
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x00824FCC */
static void lay_down_rc_track_left_flyer_twist_down(
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 26969, { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 26975, { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 26974, { 0, 6, height + 24 }, { 32, 20, 3 },
                        { 0, 6, height + 24 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 26980, { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
            }

            paint_util_set_segment_support_height(
                session,
                paint_util_rotate_segments(
                    SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);

            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
            }
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 26970, { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 26976, { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 26973, { 0, 6, height + 24 }, { 32, 20, 3 },
                        { 0, 6, height + 24 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 26979, { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
            }
            paint_util_set_segment_support_height(
                session,
                paint_util_rotate_segments(
                    SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 26971, { 0, 6, height }, { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 26977, { 0, 6, height }, { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 26972, { 0, 6, height }, { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 26978, { 0, 6, height }, { 32, 20, 3 });
                    break;
            }
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                session,
                paint_util_rotate_segments(
                    SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x00824FDC */
static void lay_down_rc_track_right_flyer_twist_down(
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 26981, { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 26987, { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 26986, { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 26992, { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
            }

            paint_util_set_segment_support_height(
                session,
                paint_util_rotate_segments(
                    SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);

            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
            }
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 26982, { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 26988, { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 26985, { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 26991, { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
            }
            paint_util_set_segment_support_height(
                session,
                paint_util_rotate_segments(
                    SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 26983, { 0, 6, height }, { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 26989, { 0, 6, height }, { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 26984, { 0, 6, height }, { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 26990, { 0, 6, height }, { 32, 20, 3 });
                    break;
            }
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                session,
                paint_util_rotate_segments(
                    SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x00824C3C */
static void lay_down_rc_track_flyer_half_loop_up(
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16594, { 0, 6, height }, { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16602, { 0, 6, height }, { 32, 20, 11 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16601, { 0, 6, height }, { 32, 20, 9 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16609, { 0, 6, height }, { 32, 20, 7 });
                    break;
            }
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_1);
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 56, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16595, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 4, 20, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16603, { 0, 14, height }, { 3, 20, 63 },
                        { 28, 6, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 4, 15, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16600, { 0, 6, height }, { 3, 20, 63 },
                        { 28, 6, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 4, 16, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16608, { 0, 6, height }, { 32, 20, 3 });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 4, 16, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16596, { 16, 0, height }, { 2, 16, 119 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16604, { 12, 0, height }, { 3, 16, 119 },
                        { 12, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16599, { 10, 16, height }, { 4, 12, 119 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16607, { 16, 16, height }, { 2, 16, 119 },
                        { 15, 6, height });
                    break;
            }
            paint_util_set_segment_support_height(
                session,
                paint_util_rotate_segments(
                    SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 168, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16597, { 0, 0, height + 32 }, { 32, 16, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16605, { 0, 0, height + 32 }, { 32, 16, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16598, { 0, 16, height + 32 }, { 32, 12, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16606, { 0, 16, height + 32 }, { 32, 12, 3 });
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x00824C4C */
static void lay_down_rc_track_flyer_half_loop_down(
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16597, { 0, 0, height + 32 }, { 32, 16, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16605, { 0, 0, height + 32 }, { 32, 16, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16598, { 0, 16, height + 32 }, { 32, 12, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16606, { 0, 16, height + 32 }, { 32, 12, 3 });
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16596, { 16, 0, height }, { 2, 16, 119 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16604, { 12, 0, height }, { 3, 16, 119 },
                        { 12, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16599, { 10, 16, height }, { 4, 12, 119 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16607, { 16, 16, height }, { 2, 16, 119 },
                        { 15, 6, height });
                    break;
            }
            paint_util_set_segment_support_height(
                session,
                paint_util_rotate_segments(
                    SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 168, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16595, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 4, 20, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16603, { 0, 14, height }, { 3, 20, 63 },
                        { 28, 6, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 4, 15, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16600, { 0, 6, height }, { 3, 20, 63 },
                        { 28, 6, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 4, 16, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16608, { 0, 6, height }, { 32, 20, 3 });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 4, 16, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 72, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16594, { 0, 6, height }, { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16602, { 0, 6, height }, { 32, 20, 11 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16601, { 0, 6, height }, { 32, 20, 9 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16609, { 0, 6, height }, { 32, 20, 7 });
                    break;
            }
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_1);
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 56, 0x20);
            break;
    }
}

/** rct2: 0x00824C5C */
static void lay_down_rc_track_left_flyer_corkscrew_up(
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16610, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height + 4 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16613, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height + 4 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16616, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height + 4 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16619, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height + 4 });
                    break;
            }

            if (direction == 2)
            {
                paint_util_set_segment_support_height(
                    session,
                    paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                    0xFFFF, 0);
            }
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction != 2)
            {
                paint_util_set_segment_support_height(
                    session,
                    paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                    0xFFFF, 0);
            }

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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16611, { 0, 0, height }, { 20, 20, 3 },
                        { 6, 6, height + 10 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16614, { 0, 0, height }, { 20, 20, 3 },
                        { 6, 6, height + 10 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16617, { 0, 0, height }, { 20, 20, 3 },
                        { 6, 6, height + 10 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16620, { 0, 0, height }, { 20, 20, 3 },
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16612, { 0, 0, height }, { 20, 32, 3 },
                        { 6, 0, height + 24 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16615, { 0, 0, height }, { 20, 32, 3 },
                        { 6, 0, height + 24 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16618, { 0, 0, height }, { 20, 32, 3 },
                        { 6, 0, height + 24 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16621, { 0, 0, height }, { 20, 32, 3 },
                        { 6, 0, height + 24 });
                    break;
            }

            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 35, session.TrackColours[SCHEME_SUPPORTS]);

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

/** rct2: 0x00824C6C */
static void lay_down_rc_track_right_flyer_corkscrew_up(
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16622, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height + 4 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16625, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height + 4 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16628, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height + 4 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16631, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height + 4 });
                    break;
            }
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16623, { 0, 0, height }, { 20, 20, 3 },
                        { 6, 6, height + 10 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16626, { 0, 0, height }, { 20, 20, 3 },
                        { 6, 6, height + 10 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16629, { 0, 0, height }, { 20, 20, 3 },
                        { 6, 6, height + 10 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16632, { 0, 0, height }, { 20, 20, 3 },
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16624, { 0, 0, height }, { 20, 32, 3 },
                        { 6, 0, height + 24 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16627, { 0, 0, height }, { 20, 32, 3 },
                        { 6, 0, height + 24 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16630, { 0, 0, height }, { 20, 32, 3 },
                        { 6, 0, height + 24 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16633, { 0, 0, height }, { 20, 32, 3 },
                        { 6, 0, height + 24 });
                    break;
            }

            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 35, session.TrackColours[SCHEME_SUPPORTS]);

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

/** rct2: 0x00824C7C */
static void lay_down_rc_track_left_flyer_corkscrew_down(
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16627, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height + 24 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16630, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height + 24 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16633, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height + 24 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16624, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height + 24 });
                    break;
            }

            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 35, session.TrackColours[SCHEME_SUPPORTS]);

            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height + 8, TUNNEL_0);
            }
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16626, { 0, 0, height }, { 20, 20, 3 },
                        { 6, 6, height + 10 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16629, { 0, 0, height }, { 20, 20, 3 },
                        { 6, 6, height + 10 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16632, { 0, 0, height }, { 20, 20, 3 },
                        { 6, 6, height + 10 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16623, { 0, 0, height }, { 20, 20, 3 },
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16625, { 0, 0, height }, { 20, 32, 3 },
                        { 6, 0, height + 4 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16628, { 0, 0, height }, { 20, 32, 3 },
                        { 6, 0, height + 4 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16631, { 0, 0, height }, { 20, 32, 3 },
                        { 6, 0, height + 4 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 16622, { 0, 0, height }, { 20, 32, 3 },
                        { 6, 0, height + 4 });
                    break;
            }
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            switch (direction)
            {
                case 2:
                    paint_util_push_tunnel_right(session, height, TUNNEL_0);
                    break;
                case 3:
                    paint_util_push_tunnel_left(session, height, TUNNEL_0);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x00824C8C */
static void lay_down_rc_track_right_flyer_corkscrew_down(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    lay_down_rc_track_left_flyer_corkscrew_up(session, ride, 2 - trackSequence, (direction + 3) % 4, height, trackElement);
}

/** rct2: 0x00824FEC */
static void lay_down_rc_track_block_brakes(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (direction)
        {
            case 0:
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 16232, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 1:
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 16233, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
        }
        metal_a_supports_paint_setup(
            session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
        paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
    }
    else
    {
        switch (direction)
        {
            case 0:
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 26559, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 24 });
                break;
            case 1:
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 26560, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 24 });
                break;
        }

        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        if (track_paint_util_should_paint_supports(session.MapPosition))
        {
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 33, session.TrackColours[SCHEME_SUPPORTS]);
        }

        paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
    }
}

static void lay_down_rc_track_left_quarter_banked_helix_large_up(
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27118, { 0, 0, height + 24 }, { 32, 20, 3 },
                        { 0, 6, height + 30 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27123, { 0, 0, height + 24 }, { 32, 20, 3 },
                        { 0, 6, height + 30 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27128, { 0, 0, height + 24 }, { 32, 20, 3 },
                        { 0, 6, height + 30 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27113, { 0, 0, height + 24 }, { 32, 20, 3 },
                        { 0, 6, height + 30 });
                    break;
            }

            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);

            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_INVERTED_3);
            }
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
        case 1:
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27117, { 0, 0, height + 24 }, { 32, 16, 3 },
                        { 0, 0, height + 38 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27122, { 0, 0, height + 24 }, { 32, 16, 3 },
                        { 0, 0, height + 38 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27127, { 0, 0, height + 24 }, { 32, 16, 3 },
                        { 0, 16, height + 38 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27112, { 0, 0, height + 24 }, { 32, 16, 3 },
                        { 0, 16, height + 38 });
                    break;
            }
            paint_util_set_segment_support_height(
                session,
                paint_util_rotate_segments(
                    SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27116, { 0, 0, height + 24 }, { 16, 16, 3 },
                        { 0, 16, height + 38 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27121, { 0, 0, height + 24 }, { 16, 16, 3 },
                        { 16, 16, height + 38 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27126, { 0, 0, height + 24 }, { 16, 16, 3 },
                        { 16, 0, height + 38 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27111, { 0, 0, height + 24 }, { 16, 16, 3 },
                        { 0, 0, height + 38 });
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
        case 4:
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
        case 5:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27115, { 0, 0, height + 24 }, { 16, 32, 3 },
                        { 16, 0, height + 38 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27120, { 0, 0, height + 24 }, { 16, 32, 3 },
                        { 0, 0, height + 38 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27125, { 0, 0, height + 24 }, { 16, 32, 3 },
                        { 0, 0, height + 38 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27110, { 0, 0, height + 24 }, { 16, 32, 3 },
                        { 16, 0, height + 38 });
                    break;
            }
            paint_util_set_segment_support_height(
                session,
                paint_util_rotate_segments(
                    SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27114, { 0, 0, height + 24 }, { 20, 32, 3 },
                        { 6, 0, height + 38 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27119, { 0, 0, height + 24 }, { 20, 32, 3 },
                        { 6, 0, height + 38 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27124, { 0, 0, height + 24 }, { 20, 32, 3 },
                        { 6, 0, height + 38 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27109, { 0, 0, height + 24 }, { 20, 32, 3 },
                        { 6, 0, height + 38 });
                    break;
            }

            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D4, direction),
                0xFFFF, 0);
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 51, session.TrackColours[SCHEME_SUPPORTS]);

            switch (direction)
            {
                case 2:
                    paint_util_push_tunnel_right(session, height + 16, TUNNEL_INVERTED_3);
                    break;
                case 3:
                    paint_util_push_tunnel_left(session, height + 16, TUNNEL_INVERTED_3);
                    break;
            }
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
    }
}

static void lay_down_rc_track_right_quarter_banked_helix_large_up(
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27089, { 0, 0, height + 24 }, { 32, 20, 3 },
                        { 0, 6, height + 30 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27094, { 0, 0, height + 24 }, { 32, 20, 3 },
                        { 0, 6, height + 30 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27099, { 0, 0, height + 24 }, { 32, 20, 3 },
                        { 0, 6, height + 30 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27104, { 0, 0, height + 24 }, { 32, 20, 3 },
                        { 0, 6, height + 30 });
                    break;
            }

            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);

            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_INVERTED_3);
            }
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
        case 1:
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27090, { 0, 0, height + 24 }, { 32, 16, 3 },
                        { 0, 16, height + 38 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27095, { 0, 0, height + 24 }, { 32, 16, 3 },
                        { 0, 16, height + 38 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27100, { 0, 0, height + 24 }, { 32, 16, 3 },
                        { 0, 0, height + 38 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27105, { 0, 0, height + 24 }, { 32, 16, 3 },
                        { 0, 0, height + 38 });
                    break;
            }
            paint_util_set_segment_support_height(
                session,
                paint_util_rotate_segments(
                    SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27091, { 0, 0, height + 24 }, { 16, 16, 3 },
                        { 0, 0, height + 38 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27096, { 0, 0, height + 24 }, { 16, 16, 3 },
                        { 16, 0, height + 38 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27101, { 0, 0, height + 24 }, { 16, 16, 3 },
                        { 16, 16, height + 38 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27106, { 0, 0, height + 24 }, { 16, 16, 3 },
                        { 0, 16, height + 38 });
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
        case 4:
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
        case 5:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27092, { 0, 0, height + 24 }, { 16, 32, 3 },
                        { 16, 0, height + 38 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27097, { 0, 0, height + 24 }, { 16, 32, 3 },
                        { 0, 0, height + 38 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27102, { 0, 0, height + 24 }, { 16, 32, 3 },
                        { 0, 0, height + 38 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27107, { 0, 0, height + 24 }, { 16, 32, 3 },
                        { 16, 0, height + 38 });
                    break;
            }
            paint_util_set_segment_support_height(
                session,
                paint_util_rotate_segments(
                    SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27093, { 0, 0, height + 24 }, { 20, 32, 3 },
                        { 6, 0, height + 38 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27098, { 0, 0, height + 24 }, { 20, 32, 3 },
                        { 6, 0, height + 38 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27103, { 0, 0, height + 24 }, { 20, 32, 3 },
                        { 6, 0, height + 38 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27108, { 0, 0, height + 24 }, { 20, 32, 3 },
                        { 6, 0, height + 38 });
                    break;
            }

            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D4, direction),
                0xFFFF, 0);
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 51, session.TrackColours[SCHEME_SUPPORTS]);

            switch (direction)
            {
                case 0:
                    paint_util_push_tunnel_right(session, height + 16, TUNNEL_INVERTED_3);
                    break;
                case 1:
                    paint_util_push_tunnel_left(session, height + 16, TUNNEL_INVERTED_3);
                    break;
            }
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
    }
}

static void lay_down_rc_track_left_quarter_banked_helix_large_down(
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27098, { 0, 0, height + 24 }, { 32, 20, 3 },
                        { 0, 6, height + 38 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27103, { 0, 0, height + 24 }, { 32, 20, 3 },
                        { 0, 6, height + 38 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27108, { 0, 0, height + 24 }, { 32, 20, 3 },
                        { 0, 6, height + 38 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27093, { 0, 0, height + 24 }, { 32, 20, 3 },
                        { 0, 6, height + 38 });
                    break;
            }

            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 51, session.TrackColours[SCHEME_SUPPORTS]);

            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height + 16, TUNNEL_INVERTED_3);
            }
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
        case 1:
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27097, { 0, 0, height + 24 }, { 32, 16, 3 },
                        { 0, 0, height + 38 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27102, { 0, 0, height + 24 }, { 32, 16, 3 },
                        { 0, 0, height + 38 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27107, { 0, 0, height + 24 }, { 32, 16, 3 },
                        { 0, 16, height + 38 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27092, { 0, 0, height + 24 }, { 32, 16, 3 },
                        { 0, 16, height + 38 });
                    break;
            }
            paint_util_set_segment_support_height(
                session,
                paint_util_rotate_segments(
                    SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27096, { 0, 0, height + 24 }, { 16, 16, 3 },
                        { 0, 16, height + 38 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27101, { 0, 0, height + 24 }, { 16, 16, 3 },
                        { 16, 16, height + 38 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27106, { 0, 0, height + 24 }, { 16, 16, 3 },
                        { 16, 0, height + 38 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27091, { 0, 0, height + 24 }, { 16, 16, 3 },
                        { 0, 0, height + 38 });
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
        case 4:
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
        case 5:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27095, { 0, 0, height + 24 }, { 16, 32, 3 },
                        { 16, 0, height + 38 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27100, { 0, 0, height + 24 }, { 16, 32, 3 },
                        { 0, 0, height + 38 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27105, { 0, 0, height + 24 }, { 16, 32, 3 },
                        { 0, 0, height + 38 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27090, { 0, 0, height + 24 }, { 16, 32, 3 },
                        { 16, 0, height + 38 });
                    break;
            }
            paint_util_set_segment_support_height(
                session,
                paint_util_rotate_segments(
                    SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27094, { 0, 0, height + 24 }, { 20, 32, 3 },
                        { 6, 0, height + 30 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27099, { 0, 0, height + 24 }, { 20, 32, 3 },
                        { 6, 0, height + 30 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27104, { 0, 0, height + 24 }, { 20, 32, 3 },
                        { 6, 0, height + 30 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27089, { 0, 0, height + 24 }, { 20, 32, 3 },
                        { 6, 0, height + 30 });
                    break;
            }

            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D4, direction),
                0xFFFF, 0);
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);

            switch (direction)
            {
                case 2:
                    paint_util_push_tunnel_right(session, height, TUNNEL_INVERTED_3);
                    break;
                case 3:
                    paint_util_push_tunnel_left(session, height, TUNNEL_INVERTED_3);
                    break;
            }
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
    }
}

static void lay_down_rc_track_right_quarter_banked_helix_large_down(
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27109, { 0, 0, height + 24 }, { 32, 20, 3 },
                        { 0, 6, height + 38 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27114, { 0, 0, height + 24 }, { 32, 20, 3 },
                        { 0, 6, height + 38 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27119, { 0, 0, height + 24 }, { 32, 20, 3 },
                        { 0, 6, height + 38 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27124, { 0, 0, height + 24 }, { 32, 20, 3 },
                        { 0, 6, height + 38 });
                    break;
            }

            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 51, session.TrackColours[SCHEME_SUPPORTS]);

            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height + 16, TUNNEL_INVERTED_3);
            }
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
        case 1:
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27110, { 0, 0, height + 24 }, { 32, 16, 3 },
                        { 0, 16, height + 38 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27115, { 0, 0, height + 24 }, { 32, 16, 3 },
                        { 0, 16, height + 38 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27120, { 0, 0, height + 24 }, { 32, 16, 3 },
                        { 0, 0, height + 38 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27125, { 0, 0, height + 24 }, { 32, 16, 3 },
                        { 0, 0, height + 38 });
                    break;
            }
            paint_util_set_segment_support_height(
                session,
                paint_util_rotate_segments(
                    SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27111, { 0, 0, height + 24 }, { 16, 16, 3 },
                        { 0, 0, height + 38 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27116, { 0, 0, height + 24 }, { 16, 16, 3 },
                        { 16, 0, height + 38 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27121, { 0, 0, height + 24 }, { 16, 16, 3 },
                        { 16, 16, height + 38 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27126, { 0, 0, height + 24 }, { 16, 16, 3 },
                        { 0, 16, height + 38 });
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
        case 4:
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
        case 5:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27112, { 0, 0, height + 24 }, { 16, 32, 3 },
                        { 16, 0, height + 38 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27117, { 0, 0, height + 24 }, { 16, 32, 3 },
                        { 0, 0, height + 38 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27122, { 0, 0, height + 24 }, { 16, 32, 3 },
                        { 0, 0, height + 38 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27127, { 0, 0, height + 24 }, { 16, 32, 3 },
                        { 16, 0, height + 38 });
                    break;
            }
            paint_util_set_segment_support_height(
                session,
                paint_util_rotate_segments(
                    SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27113, { 0, 0, height + 24 }, { 20, 32, 3 },
                        { 6, 0, height + 30 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27118, { 0, 0, height + 24 }, { 20, 32, 3 },
                        { 6, 0, height + 30 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27123, { 0, 0, height + 24 }, { 20, 32, 3 },
                        { 6, 0, height + 30 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27128, { 0, 0, height + 24 }, { 20, 32, 3 },
                        { 6, 0, height + 30 });
                    break;
            }

            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D4, direction),
                0xFFFF, 0);
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);

            switch (direction)
            {
                case 0:
                    paint_util_push_tunnel_right(session, height, TUNNEL_INVERTED_3);
                    break;
                case 1:
                    paint_util_push_tunnel_left(session, height, TUNNEL_INVERTED_3);
                    break;
            }
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
    }
}

TRACK_PAINT_FUNCTION get_track_paint_function_lay_down_rc(int32_t trackType)
{
    switch (trackType)
    {
        case TrackElemType::Flat:
            return lay_down_rc_track_flat;
        case TrackElemType::EndStation:
        case TrackElemType::BeginStation:
        case TrackElemType::MiddleStation:
            return lay_down_rc_track_station;
        case TrackElemType::Up25:
            return lay_down_rc_track_25_deg_up;
        case TrackElemType::Up60:
            return lay_down_rc_track_60_deg_up;
        case TrackElemType::FlatToUp25:
            return lay_down_rc_track_flat_to_25_deg_up;
        case TrackElemType::Up25ToUp60:
            return lay_down_rc_track_25_deg_up_to_60_deg_up;
        case TrackElemType::Up60ToUp25:
            return lay_down_rc_track_60_deg_up_to_25_deg_up;
        case TrackElemType::Up25ToFlat:
            return lay_down_rc_track_25_deg_up_to_flat;
        case TrackElemType::Down25:
            return lay_down_rc_track_25_deg_down;
        case TrackElemType::Down60:
            return lay_down_rc_track_60_deg_down;
        case TrackElemType::FlatToDown25:
            return lay_down_rc_track_flat_to_25_deg_down;
        case TrackElemType::Down25ToDown60:
            return lay_down_rc_track_25_deg_down_to_60_deg_down;
        case TrackElemType::Down60ToDown25:
            return lay_down_rc_track_60_deg_down_to_25_deg_down;
        case TrackElemType::Down25ToFlat:
            return lay_down_rc_track_25_deg_down_to_flat;
        case TrackElemType::LeftQuarterTurn5Tiles:
            return lay_down_rc_track_left_quarter_turn_5;
        case TrackElemType::RightQuarterTurn5Tiles:
            return lay_down_rc_track_right_quarter_turn_5;
        case TrackElemType::FlatToLeftBank:
            return lay_down_rc_track_flat_to_left_bank;
        case TrackElemType::FlatToRightBank:
            return lay_down_rc_track_flat_to_right_bank;
        case TrackElemType::LeftBankToFlat:
            return lay_down_rc_track_left_bank_to_flat;
        case TrackElemType::RightBankToFlat:
            return lay_down_rc_track_right_bank_to_flat;
        case TrackElemType::BankedLeftQuarterTurn5Tiles:
            return lay_down_rc_track_banked_left_quarter_turn_5;
        case TrackElemType::BankedRightQuarterTurn5Tiles:
            return lay_down_rc_track_banked_right_quarter_turn_5;
        case TrackElemType::LeftBankToUp25:
            return lay_down_rc_track_left_bank_to_25_deg_up;
        case TrackElemType::RightBankToUp25:
            return lay_down_rc_track_right_bank_to_25_deg_up;
        case TrackElemType::Up25ToLeftBank:
            return lay_down_rc_track_25_deg_up_to_left_bank;
        case TrackElemType::Up25ToRightBank:
            return lay_down_rc_track_25_deg_up_to_right_bank;
        case TrackElemType::LeftBankToDown25:
            return lay_down_rc_track_left_bank_to_25_deg_down;
        case TrackElemType::RightBankToDown25:
            return lay_down_rc_track_right_bank_to_25_deg_down;
        case TrackElemType::Down25ToLeftBank:
            return lay_down_rc_track_25_deg_down_to_left_bank;
        case TrackElemType::Down25ToRightBank:
            return lay_down_rc_track_25_deg_down_to_right_bank;
        case TrackElemType::LeftBank:
            return lay_down_rc_track_left_bank;
        case TrackElemType::RightBank:
            return lay_down_rc_track_right_bank;
        case TrackElemType::LeftQuarterTurn5TilesUp25:
            return lay_down_rc_track_left_quarter_turn_5_25_deg_up;
        case TrackElemType::RightQuarterTurn5TilesUp25:
            return lay_down_rc_track_right_quarter_turn_5_25_deg_up;
        case TrackElemType::LeftQuarterTurn5TilesDown25:
            return lay_down_rc_track_left_quarter_turn_5_25_deg_down;
        case TrackElemType::RightQuarterTurn5TilesDown25:
            return lay_down_rc_track_right_quarter_turn_5_25_deg_down;
        case TrackElemType::SBendLeft:
            return lay_down_rc_track_s_bend_left;
        case TrackElemType::SBendRight:
            return lay_down_rc_track_s_bend_right;
        case TrackElemType::LeftVerticalLoop:
            return lay_down_rc_track_left_vertical_loop;
        case TrackElemType::RightVerticalLoop:
            return lay_down_rc_track_right_vertical_loop;
        case TrackElemType::LeftQuarterTurn3Tiles:
            return lay_down_rc_track_left_quarter_turn_3;
        case TrackElemType::RightQuarterTurn3Tiles:
            return lay_down_rc_track_right_quarter_turn_3;
        case TrackElemType::LeftBankedQuarterTurn3Tiles:
            return lay_down_rc_track_left_quarter_turn_3_bank;
        case TrackElemType::RightBankedQuarterTurn3Tiles:
            return lay_down_rc_track_right_quarter_turn_3_bank;
        case TrackElemType::LeftQuarterTurn3TilesUp25:
            return lay_down_rc_track_left_quarter_turn_3_25_deg_up;
        case TrackElemType::RightQuarterTurn3TilesUp25:
            return lay_down_rc_track_right_quarter_turn_3_25_deg_up;
        case TrackElemType::LeftQuarterTurn3TilesDown25:
            return lay_down_rc_track_left_quarter_turn_3_25_deg_down;
        case TrackElemType::RightQuarterTurn3TilesDown25:
            return lay_down_rc_track_right_quarter_turn_3_25_deg_down;
        case TrackElemType::LeftHalfBankedHelixUpSmall:
            return lay_down_rc_track_left_half_banked_helix_up_small;
        case TrackElemType::RightHalfBankedHelixUpSmall:
            return lay_down_rc_track_right_half_banked_helix_up_small;
        case TrackElemType::LeftHalfBankedHelixDownSmall:
            return lay_down_rc_track_left_half_banked_helix_down_small;
        case TrackElemType::RightHalfBankedHelixDownSmall:
            return lay_down_rc_track_right_half_banked_helix_down_small;
        case TrackElemType::LeftHalfBankedHelixUpLarge:
            return lay_down_rc_track_left_half_banked_helix_up_large;
        case TrackElemType::RightHalfBankedHelixUpLarge:
            return lay_down_rc_track_right_half_banked_helix_up_large;
        case TrackElemType::LeftHalfBankedHelixDownLarge:
            return lay_down_rc_track_left_half_banked_helix_down_large;
        case TrackElemType::RightHalfBankedHelixDownLarge:
            return lay_down_rc_track_right_half_banked_helix_down_large;
        case TrackElemType::LeftQuarterTurn1TileUp60:
            return lay_down_rc_track_left_quarter_turn_1_60_deg_up;
        case TrackElemType::RightQuarterTurn1TileUp60:
            return lay_down_rc_track_right_quarter_turn_1_60_deg_up;
        case TrackElemType::LeftQuarterTurn1TileDown60:
            return lay_down_rc_track_left_quarter_turn_1_60_deg_down;
        case TrackElemType::RightQuarterTurn1TileDown60:
            return lay_down_rc_track_right_quarter_turn_1_60_deg_down;
        case TrackElemType::Brakes:
            return lay_down_rc_track_brakes;
        case TrackElemType::OnRidePhoto:
            return lay_down_rc_track_on_ride_photo;
        case TrackElemType::LeftEighthToDiag:
            return lay_down_rc_track_left_eighth_to_diag;
        case TrackElemType::RightEighthToDiag:
            return lay_down_rc_track_right_eighth_to_diag;
        case TrackElemType::LeftEighthToOrthogonal:
            return lay_down_rc_track_left_eighth_to_orthogonal;
        case TrackElemType::RightEighthToOrthogonal:
            return lay_down_rc_track_right_eighth_to_orthogonal;
        case TrackElemType::LeftEighthBankToDiag:
            return lay_down_rc_track_left_eighth_bank_to_diag;
        case TrackElemType::RightEighthBankToDiag:
            return lay_down_rc_track_right_eighth_bank_to_diag;
        case TrackElemType::LeftEighthBankToOrthogonal:
            return lay_down_rc_track_left_eighth_bank_to_orthogonal;
        case TrackElemType::RightEighthBankToOrthogonal:
            return lay_down_rc_track_right_eighth_bank_to_orthogonal;
        case TrackElemType::DiagFlat:
            return lay_down_rc_track_diag_flat;
        case TrackElemType::DiagUp25:
            return lay_down_rc_track_diag_25_deg_up;
        case TrackElemType::DiagUp60:
            return lay_down_rc_track_diag_60_deg_up;
        case TrackElemType::DiagFlatToUp25:
            return lay_down_rc_track_diag_flat_to_25_deg_up;
        case TrackElemType::DiagUp25ToUp60:
            return lay_down_rc_track_diag_25_deg_up_to_60_deg_up;
        case TrackElemType::DiagUp60ToUp25:
            return lay_down_rc_track_diag_60_deg_up_to_25_deg_up;
        case TrackElemType::DiagUp25ToFlat:
            return lay_down_rc_track_diag_25_deg_up_to_flat;
        case TrackElemType::DiagDown25:
            return lay_down_rc_track_diag_25_deg_down;
        case TrackElemType::DiagDown60:
            return lay_down_rc_track_diag_60_deg_down;
        case TrackElemType::DiagFlatToDown25:
            return lay_down_rc_track_diag_flat_to_25_deg_down;
        case TrackElemType::DiagDown25ToDown60:
            return lay_down_rc_track_diag_25_deg_down_to_60_deg_down;
        case TrackElemType::DiagDown60ToDown25:
            return lay_down_rc_track_diag_60_deg_down_to_25_deg_down;
        case TrackElemType::DiagDown25ToFlat:
            return lay_down_rc_track_diag_25_deg_down_to_flat;
        case TrackElemType::DiagFlatToLeftBank:
            return lay_down_rc_track_diag_flat_to_left_bank;
        case TrackElemType::DiagFlatToRightBank:
            return lay_down_rc_track_diag_flat_to_right_bank;
        case TrackElemType::DiagLeftBankToFlat:
            return lay_down_rc_track_diag_left_bank_to_flat;
        case TrackElemType::DiagRightBankToFlat:
            return lay_down_rc_track_diag_right_bank_to_flat;
        case TrackElemType::DiagLeftBankToUp25:
            return lay_down_rc_track_diag_left_bank_to_25_deg_up;
        case TrackElemType::DiagRightBankToUp25:
            return lay_down_rc_track_diag_right_bank_to_25_deg_up;
        case TrackElemType::DiagUp25ToLeftBank:
            return lay_down_rc_track_diag_25_deg_up_to_left_bank;
        case TrackElemType::DiagUp25ToRightBank:
            return lay_down_rc_track_diag_25_deg_up_to_right_bank;
        case TrackElemType::DiagLeftBankToDown25:
            return lay_down_rc_track_diag_left_bank_to_25_deg_down;
        case TrackElemType::DiagRightBankToDown25:
            return lay_down_rc_track_diag_right_bank_to_25_deg_down;
        case TrackElemType::DiagDown25ToLeftBank:
            return lay_down_rc_track_diag_25_deg_down_to_left_bank;
        case TrackElemType::DiagDown25ToRightBank:
            return lay_down_rc_track_diag_25_deg_down_to_right_bank;
        case TrackElemType::DiagLeftBank:
            return lay_down_rc_track_diag_left_bank;
        case TrackElemType::DiagRightBank:
            return lay_down_rc_track_diag_right_bank;
        case TrackElemType::LeftFlyerTwistUp:
            return lay_down_rc_track_left_flyer_twist_up;
        case TrackElemType::RightFlyerTwistUp:
            return lay_down_rc_track_right_flyer_twist_up;
        case TrackElemType::LeftFlyerTwistDown:
            return lay_down_rc_track_left_flyer_twist_down;
        case TrackElemType::RightFlyerTwistDown:
            return lay_down_rc_track_right_flyer_twist_down;
        case TrackElemType::FlyerHalfLoopUp:
            return lay_down_rc_track_flyer_half_loop_up;
        case TrackElemType::FlyerHalfLoopDown:
            return lay_down_rc_track_flyer_half_loop_down;
        case TrackElemType::LeftFlyerCorkscrewUp:
            return lay_down_rc_track_left_flyer_corkscrew_up;
        case TrackElemType::RightFlyerCorkscrewUp:
            return lay_down_rc_track_right_flyer_corkscrew_up;
        case TrackElemType::LeftFlyerCorkscrewDown:
            return lay_down_rc_track_left_flyer_corkscrew_down;
        case TrackElemType::RightFlyerCorkscrewDown:
            return lay_down_rc_track_right_flyer_corkscrew_down;
        case TrackElemType::BlockBrakes:
            return lay_down_rc_track_block_brakes;

        case TrackElemType::LeftQuarterBankedHelixLargeUp:
            return lay_down_rc_track_left_quarter_banked_helix_large_up;
        case TrackElemType::RightQuarterBankedHelixLargeUp:
            return lay_down_rc_track_right_quarter_banked_helix_large_up;
        case TrackElemType::LeftQuarterBankedHelixLargeDown:
            return lay_down_rc_track_left_quarter_banked_helix_large_down;
        case TrackElemType::RightQuarterBankedHelixLargeDown:
            return lay_down_rc_track_right_quarter_banked_helix_large_down;
    }
    return nullptr;
}
