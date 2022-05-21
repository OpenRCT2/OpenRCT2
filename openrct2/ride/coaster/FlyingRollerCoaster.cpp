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
#include "BolligerMabillardTrack.hpp"

/** rct2: 0x007C6FF4 */
static void flying_rc_track_flat(
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17486, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17487, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17488, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17489, { 0, 0, height }, { 32, 20, 3 },
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17146, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
                case 1:
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17147, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
            }
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
        }
        paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27131, { 0, 0, height + 24 }, { 32, 20, 1 },
                        { 0, 6, height + 22 });
                    break;
                case 1:
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27132, { 0, 0, height + 24 }, { 32, 20, 1 },
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27129, { 0, 0, height + 24 }, { 32, 20, 1 },
                        { 0, 6, height + 22 });
                    break;
                case 1:
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27130, { 0, 0, height + 24 }, { 32, 20, 1 },
                        { 0, 6, height + 22 });
                    break;
            }
        }

        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        if (track_paint_util_should_paint_supports(session.MapPosition))
        {
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
        }

        paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
        paint_util_set_general_support_height(session, height + 32, 0x20);
    }
}

/** rct2: 0x007C7244, 0x007C7254, 0x007C7264 */
static void flying_rc_track_station(
    paint_session& session, const Ride& ride, [[maybe_unused]] uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackElement.IsInverted())
    {
        static constexpr const uint32_t imageIds[4][3] = {
            { SPR_STATION_BASE_C_SW_NE, 27131, SPR_STATION_INVERTED_BAR_C_SW_NE },
            { SPR_STATION_BASE_C_NW_SE, 27132, SPR_STATION_INVERTED_BAR_C_NW_SE },
            { SPR_STATION_BASE_C_SW_NE, 27131, SPR_STATION_INVERTED_BAR_C_SW_NE },
            { SPR_STATION_BASE_C_NW_SE, 27132, SPR_STATION_INVERTED_BAR_C_NW_SE },
        };
        PaintAddImageAsParentRotated(
            session, direction, imageIds[direction][0] | session.TrackColours[SCHEME_MISC], { 0, 0, height }, { 32, 28, 1 },
            { 0, 2, height });
        PaintAddImageAsParentRotated(
            session, direction, imageIds[direction][1] | session.TrackColours[SCHEME_TRACK], { 0, 0, height + 24 },
            { 32, 20, 3 }, { 0, 6, height + 24 });
        PaintAddImageAsChildRotated(
            session, direction, imageIds[direction][2] | session.TrackColours[SCHEME_SUPPORTS], { 0, 6, height + 24 },
            { 32, 20, 1 }, { 0, 6, height + 24 });
        track_paint_util_draw_station_metal_supports_2(session, direction, height, session.TrackColours[SCHEME_SUPPORTS], 11);
        track_paint_util_draw_station_inverted(session, ride, direction, height, trackElement, STATION_VARIANT_1);
    }
    else
    {
        static constexpr const uint32_t imageIds[4][3] = {
            { 17154, 17150, SPR_STATION_BASE_A_SW_NE },
            { 17155, 17151, SPR_STATION_BASE_A_NW_SE },
            { 17154, 17150, SPR_STATION_BASE_A_SW_NE },
            { 17155, 17151, SPR_STATION_BASE_A_NW_SE },
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
    }

    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x007C7004 */
static void flying_rc_track_25_deg_up(
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17498, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17499, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17500, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17501, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
            }
        }
        else
        {
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17204, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17205, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17206, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17207, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
            }
        }
        if (track_paint_util_should_paint_supports(session.MapPosition))
        {
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
        }
        if (direction == 0 || direction == 3)
        {
            paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_SQUARE_7);
        }
        else
        {
            paint_util_push_tunnel_rotated(session, direction, height + 8, TUNNEL_SQUARE_8);
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27249, { 0, 0, height + 24 }, { 32, 20, 3 },
                        { 0, 6, height + 40 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27250, { 0, 0, height + 24 }, { 32, 20, 3 },
                        { 0, 6, height + 40 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27251, { 0, 0, height + 24 }, { 32, 20, 3 },
                        { 0, 6, height + 40 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27252, { 0, 0, height + 24 }, { 32, 20, 3 },
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27221, { 0, 0, height + 24 }, { 32, 20, 3 },
                        { 0, 6, height + 40 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27222, { 0, 0, height + 24 }, { 32, 20, 3 },
                        { 0, 6, height + 40 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27223, { 0, 0, height + 24 }, { 32, 20, 3 },
                        { 0, 6, height + 40 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27224, { 0, 0, height + 24 }, { 32, 20, 3 },
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
                        session, METAL_SUPPORTS_TUBES_INVERTED, 6, 0, height + 57, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height + 57, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 7, 0, height + 57, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height + 57, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
        }

        if (direction == 0 || direction == 3)
        {
            paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_SQUARE_7);
        }
        else
        {
            paint_util_push_tunnel_rotated(session, direction, height + 8, TUNNEL_SQUARE_8);
        }
        paint_util_set_general_support_height(session, height + 56, 0x20);
    }
}

/** rct2: 0x007C7014 */
static void flying_rc_track_60_deg_up(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17220, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17221, { 0, 0, height }, { 32, 1, 98 },
                    { 0, 27, height });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17222, { 0, 0, height }, { 32, 1, 98 },
                    { 0, 27, height });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17223, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
        }
        if (track_paint_util_should_paint_supports(session.MapPosition))
        {
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 32, height, session.TrackColours[SCHEME_SUPPORTS]);
        }
        if (direction == 0 || direction == 3)
        {
            paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_SQUARE_7);
        }
        else
        {
            paint_util_push_tunnel_rotated(session, direction, height + 56, TUNNEL_SQUARE_8);
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
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27237, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 88 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27238, { 0, 0, height + 24 }, { 32, 2, 81 },
                    { 0, 4, height + 6 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27239, { 0, 0, height + 24 }, { 32, 2, 81 },
                    { 0, 4, height + 6 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27240, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 88 });
                break;
        }
        if (direction == 0 || direction == 3)
        {
            paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_SQUARE_7);
        }
        else
        {
            paint_util_push_tunnel_rotated(session, direction, height + 56, TUNNEL_SQUARE_8);
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 104, 0x20);
    }
}

/** rct2: 0x007C7024 */
static void flying_rc_track_flat_to_25_deg_up(
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17490, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17491, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17492, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17493, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
            }
        }
        else
        {
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17196, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17197, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17198, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17199, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
            }
        }
        if (track_paint_util_should_paint_supports(session.MapPosition))
        {
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 3, height, session.TrackColours[SCHEME_SUPPORTS]);
        }
        if (direction == 0 || direction == 3)
        {
            paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
        }
        else
        {
            paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_8);
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27241, { 0, 0, height + 24 }, { 32, 20, 3 },
                        { 0, 6, height + 32 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27242, { 0, 0, height + 24 }, { 32, 20, 3 },
                        { 0, 6, height + 32 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27243, { 0, 0, height + 24 }, { 32, 20, 3 },
                        { 0, 6, height + 32 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27244, { 0, 0, height + 24 }, { 32, 20, 3 },
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27213, { 0, 0, height + 24 }, { 32, 20, 3 },
                        { 0, 6, height + 32 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27214, { 0, 0, height + 24 }, { 32, 20, 3 },
                        { 0, 6, height + 32 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27215, { 0, 0, height + 24 }, { 32, 20, 3 },
                        { 0, 6, height + 32 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27216, { 0, 0, height + 24 }, { 32, 20, 3 },
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
                        session, METAL_SUPPORTS_TUBES_INVERTED, 6, 0, height + 49, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height + 49, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 7, 0, height + 49, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height + 49, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
        }

        if (direction == 0 || direction == 3)
        {
            paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
        }
        else
        {
            paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_8);
        }
        paint_util_set_general_support_height(session, height + 48, 0x20);
    }
}

/** rct2: 0x007C7034 */
static void flying_rc_track_25_deg_up_to_60_deg_up(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17208, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17209, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17212, { 0, 0, height }, { 32, 1, 66 },
                    { 0, 27, height });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17210, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17213, { 0, 0, height }, { 32, 1, 66 },
                    { 0, 27, height });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17211, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
        }
        if (track_paint_util_should_paint_supports(session.MapPosition))
        {
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 12, height, session.TrackColours[SCHEME_SUPPORTS]);
        }
        if (direction == 0 || direction == 3)
        {
            paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_SQUARE_7);
        }
        else
        {
            paint_util_push_tunnel_rotated(session, direction, height + 24, TUNNEL_SQUARE_8);
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
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27225, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 56 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27229, { 0, 0, height + 24 }, { 32, 10, 49 },
                    { 0, 10, height + 6 });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27226, { 0, 0, height + 24 }, { 32, 2, 49 },
                    { 0, 4, height + 6 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27230, { 0, 0, height + 24 }, { 32, 10, 49 },
                    { 0, 10, height + 6 });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27227, { 0, 0, height + 24 }, { 32, 2, 49 },
                    { 0, 4, height + 6 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27228, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 56 });
                break;
        }
        if (direction == 0 || direction == 3)
        {
            paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_SQUARE_7);
        }
        else
        {
            paint_util_push_tunnel_rotated(session, direction, height + 24, TUNNEL_SQUARE_8);
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 72, 0x20);
    }
}

/** rct2: 0x007C7044 */
static void flying_rc_track_60_deg_up_to_25_deg_up(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17214, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17215, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17218, { 0, 0, height }, { 32, 1, 66 },
                    { 0, 27, height });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17216, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17219, { 0, 0, height }, { 32, 1, 66 },
                    { 0, 27, height });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17217, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
        }
        if (track_paint_util_should_paint_supports(session.MapPosition))
        {
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 20, height, session.TrackColours[SCHEME_SUPPORTS]);
        }
        if (direction == 0 || direction == 3)
        {
            paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_SQUARE_7);
        }
        else
        {
            paint_util_push_tunnel_rotated(session, direction, height + 24, TUNNEL_SQUARE_8);
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
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27231, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 56 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27235, { 0, 0, height + 24 }, { 32, 10, 49 },
                    { 0, 10, height + 6 });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27232, { 0, 0, height + 24 }, { 32, 2, 49 },
                    { 0, 4, height + 6 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27236, { 0, 0, height + 24 }, { 32, 10, 49 },
                    { 0, 10, height + 6 });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27233, { 0, 0, height + 24 }, { 32, 2, 49 },
                    { 0, 4, height + 6 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27234, { 0, 0, height + 24 }, { 32, 20, 3 },
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
                        session, METAL_SUPPORTS_TUBES_INVERTED, 6, 0, height + 71, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height + 71, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 7, 0, height + 71, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height + 71, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
        }

        if (direction == 0 || direction == 3)
        {
            paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_SQUARE_7);
        }
        else
        {
            paint_util_push_tunnel_rotated(session, direction, height + 24, TUNNEL_SQUARE_8);
        }
        paint_util_set_general_support_height(session, height + 72, 0x20);
    }
}

/** rct2: 0x007C7054 */
static void flying_rc_track_25_deg_up_to_flat(
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17494, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17495, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17496, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17497, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
            }
        }
        else
        {
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17200, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17201, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17202, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17203, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
            }
        }
        if (track_paint_util_should_paint_supports(session.MapPosition))
        {
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
        }
        if (direction == 0 || direction == 3)
        {
            paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_SQUARE_FLAT);
        }
        else
        {
            paint_util_push_tunnel_rotated(session, direction, height + 8, TUNNEL_14);
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27245, { 0, 0, height + 24 }, { 32, 20, 3 },
                        { 0, 6, height + 32 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27246, { 0, 0, height + 24 }, { 32, 20, 3 },
                        { 0, 6, height + 32 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27247, { 0, 0, height + 24 }, { 32, 20, 3 },
                        { 0, 6, height + 32 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27248, { 0, 0, height + 24 }, { 32, 20, 3 },
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27217, { 0, 0, height + 24 }, { 32, 20, 3 },
                        { 0, 6, height + 32 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27218, { 0, 0, height + 24 }, { 32, 20, 3 },
                        { 0, 6, height + 32 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27219, { 0, 0, height + 24 }, { 32, 20, 3 },
                        { 0, 6, height + 32 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27220, { 0, 0, height + 24 }, { 32, 20, 3 },
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
                        session, METAL_SUPPORTS_TUBES_INVERTED, 6, 0, height + 47, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height + 47, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 7, 0, height + 47, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height + 47, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
        }

        if (direction == 0 || direction == 3)
        {
            paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_SQUARE_FLAT);
        }
        else
        {
            paint_util_push_tunnel_rotated(session, direction, height + 8, TUNNEL_14);
        }
        paint_util_set_general_support_height(session, height + 40, 0x20);
    }
}

/** rct2: 0x007C7064 */
static void flying_rc_track_25_deg_down(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    flying_rc_track_25_deg_up(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x007C7074 */
static void flying_rc_track_60_deg_down(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    flying_rc_track_60_deg_up(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x007C7084 */
static void flying_rc_track_flat_to_25_deg_down(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    flying_rc_track_25_deg_up_to_flat(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x007C7094 */
static void flying_rc_track_25_deg_down_to_60_deg_down(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    flying_rc_track_60_deg_up_to_25_deg_up(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x007C70A4 */
static void flying_rc_track_60_deg_down_to_25_deg_down(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    flying_rc_track_25_deg_up_to_60_deg_up(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x007C70B4 */
static void flying_rc_track_25_deg_down_to_flat(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    flying_rc_track_flat_to_25_deg_up(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x007C70C4 */
static void flying_rc_track_left_quarter_turn_5(
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17259, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17264, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17269, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17254, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                }
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                if (direction == 0 || direction == 3)
                {
                    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17258, { 0, 0, height }, { 32, 16, 3 },
                            { 0, 0, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17263, { 0, 0, height }, { 32, 16, 3 },
                            { 0, 0, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17268, { 0, 0, height }, { 32, 16, 3 },
                            { 0, 16, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17253, { 0, 0, height }, { 32, 16, 3 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17257, { 0, 0, height }, { 16, 16, 3 },
                            { 0, 16, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17262, { 0, 0, height }, { 16, 16, 3 },
                            { 16, 16, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17267, { 0, 0, height }, { 16, 16, 3 },
                            { 16, 0, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17252, { 0, 0, height }, { 16, 16, 3 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17256, { 0, 0, height }, { 16, 32, 3 },
                            { 16, 0, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17261, { 0, 0, height }, { 16, 32, 3 },
                            { 0, 0, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17266, { 0, 0, height }, { 16, 32, 3 },
                            { 0, 0, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17251, { 0, 0, height }, { 16, 32, 3 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17255, { 0, 0, height }, { 20, 32, 3 },
                            { 6, 0, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17260, { 0, 0, height }, { 20, 32, 3 },
                            { 6, 0, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17265, { 0, 0, height }, { 20, 32, 3 },
                            { 6, 0, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17250, { 0, 0, height }, { 20, 32, 3 },
                            { 6, 0, height });
                        break;
                }
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                switch (direction)
                {
                    case 2:
                        paint_util_push_tunnel_right(session, height, TUNNEL_SQUARE_FLAT);
                        break;
                    case 3:
                        paint_util_push_tunnel_left(session, height, TUNNEL_SQUARE_FLAT);
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27142, { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27147, { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27152, { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27137, { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                }

                paint_util_set_segment_support_height(
                    session,
                    paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);

                if (direction == 0 || direction == 3)
                {
                    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27141, { 0, 0, height + 24 },
                            { 32, 16, 3 }, { 0, 0, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27146, { 0, 0, height + 24 },
                            { 32, 16, 3 }, { 0, 0, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27151, { 0, 0, height + 24 },
                            { 32, 16, 3 }, { 0, 16, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27136, { 0, 0, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27140, { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 0, 16, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27145, { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 16, 16, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27150, { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 16, 0, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27135, { 0, 0, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27139, { 0, 0, height + 24 },
                            { 16, 32, 3 }, { 16, 0, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27144, { 0, 0, height + 24 },
                            { 16, 32, 3 }, { 0, 0, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27149, { 0, 0, height + 24 },
                            { 16, 32, 3 }, { 0, 0, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27134, { 0, 0, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27138, { 0, 0, height + 24 },
                            { 20, 32, 3 }, { 6, 0, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27143, { 0, 0, height + 24 },
                            { 20, 32, 3 }, { 6, 0, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27148, { 0, 0, height + 24 },
                            { 20, 32, 3 }, { 6, 0, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27133, { 0, 0, height + 24 },
                            { 20, 32, 3 }, { 6, 0, height + 22 });
                        break;
                }

                paint_util_set_segment_support_height(
                    session,
                    paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D4, direction),
                    0xFFFF, 0);
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);

                switch (direction)
                {
                    case 2:
                        paint_util_push_tunnel_right(session, height, TUNNEL_SQUARE_FLAT);
                        break;
                    case 3:
                        paint_util_push_tunnel_left(session, height, TUNNEL_SQUARE_FLAT);
                        break;
                }
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
        }
    }
}

/** rct2: 0x007C70D4 */
static void flying_rc_track_right_quarter_turn_5(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    flying_rc_track_left_quarter_turn_5(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x007C70E4 */
static void flying_rc_track_flat_to_left_bank(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17156, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17164, { 0, 0, height }, { 32, 1, 26 },
                    { 0, 27, height });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17157, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17165, { 0, 0, height }, { 32, 1, 26 },
                    { 0, 27, height });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17158, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17159, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
        }
        if (track_paint_util_should_paint_supports(session.MapPosition))
        {
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
        }
        paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
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
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27269, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 22 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27270, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 22 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27271, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 22 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27272, { 0, 0, height + 24 }, { 32, 20, 3 },
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
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
        }

        paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
        paint_util_set_general_support_height(session, height + 32, 0x20);
    }
}

/** rct2: 0x007C70F4 */
static void flying_rc_track_flat_to_right_bank(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17160, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17161, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17162, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17166, { 0, 0, height }, { 32, 1, 26 },
                    { 0, 27, height });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17163, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17167, { 0, 0, height }, { 32, 1, 26 },
                    { 0, 27, height });
                break;
        }
        if (track_paint_util_should_paint_supports(session.MapPosition))
        {
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
        }
        paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
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
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27273, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 22 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27274, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 22 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27275, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 22 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27276, { 0, 0, height + 24 }, { 32, 20, 3 },
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
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
        }

        paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
        paint_util_set_general_support_height(session, height + 32, 0x20);
    }
}

/** rct2: 0x007C7104 */
static void flying_rc_track_left_bank_to_flat(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17162, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17166, { 0, 0, height }, { 32, 1, 26 },
                    { 0, 27, height });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17163, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17167, { 0, 0, height }, { 32, 1, 26 },
                    { 0, 27, height });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17160, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17161, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
        }
        if (track_paint_util_should_paint_supports(session.MapPosition))
        {
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
        }
        paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
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
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27275, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 22 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27276, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 22 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27273, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 22 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27274, { 0, 0, height + 24 }, { 32, 20, 3 },
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
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
        }

        paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
        paint_util_set_general_support_height(session, height + 32, 0x20);
    }
}

/** rct2: 0x007C7114 */
static void flying_rc_track_right_bank_to_flat(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17158, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17159, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17156, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17164, { 0, 0, height }, { 32, 1, 26 },
                    { 0, 27, height });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17157, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17165, { 0, 0, height }, { 32, 1, 26 },
                    { 0, 27, height });
                break;
        }
        if (track_paint_util_should_paint_supports(session.MapPosition))
        {
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
        }
        paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
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
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27271, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 22 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27272, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 22 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27269, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 22 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27270, { 0, 0, height + 24 }, { 32, 20, 3 },
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
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
        }

        paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
        paint_util_set_general_support_height(session, height + 32, 0x20);
    }
}

/** rct2: 0x007C7124 */
static void flying_rc_track_banked_left_quarter_turn_5(
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17279, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17290, { 0, 0, height }, { 32, 1, 26 },
                            { 0, 27, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17284, { 0, 0, height }, { 32, 1, 26 },
                            { 0, 27, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17289, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17274, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                }
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                if (direction == 0 || direction == 3)
                {
                    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17278, { 0, 0, height }, { 32, 16, 3 },
                            { 0, 0, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17283, { 0, 0, height }, { 32, 16, 1 },
                            { 0, 0, height + 27 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17288, { 0, 0, height }, { 32, 16, 3 },
                            { 0, 16, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17273, { 0, 0, height }, { 32, 16, 3 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17277, { 0, 0, height }, { 16, 16, 3 },
                            { 0, 16, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17282, { 0, 0, height }, { 16, 16, 1 },
                            { 16, 16, height + 27 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17287, { 0, 0, height }, { 16, 16, 3 },
                            { 16, 0, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17272, { 0, 0, height }, { 16, 16, 3 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17276, { 0, 0, height }, { 16, 32, 3 },
                            { 16, 0, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17281, { 0, 0, height }, { 16, 32, 1 },
                            { 0, 0, height + 27 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17286, { 0, 0, height }, { 16, 32, 3 },
                            { 0, 0, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17271, { 0, 0, height }, { 16, 32, 3 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17275, { 0, 0, height }, { 20, 32, 3 },
                            { 6, 0, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17280, { 0, 0, height }, { 1, 32, 26 },
                            { 27, 0, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17285, { 0, 0, height }, { 20, 32, 3 },
                            { 6, 0, height });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17291, { 0, 0, height }, { 1, 32, 26 },
                            { 27, 0, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17270, { 0, 0, height }, { 20, 32, 3 },
                            { 6, 0, height });
                        break;
                }
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                switch (direction)
                {
                    case 2:
                        paint_util_push_tunnel_right(session, height, TUNNEL_SQUARE_FLAT);
                        break;
                    case 3:
                        paint_util_push_tunnel_left(session, height, TUNNEL_SQUARE_FLAT);
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27162, { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 24 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27167, { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 24 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27172, { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 24 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27157, { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 24 });
                        break;
                }

                paint_util_set_segment_support_height(
                    session,
                    paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);

                if (direction == 0 || direction == 3)
                {
                    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27161, { 0, 0, height + 24 },
                            { 32, 16, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27166, { 0, 0, height + 24 },
                            { 32, 16, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27171, { 0, 0, height + 24 },
                            { 32, 16, 3 }, { 0, 16, height + 24 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27156, { 0, 0, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27160, { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 0, 16, height + 24 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27165, { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 16, 16, height + 24 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27170, { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 16, 0, height + 24 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27155, { 0, 0, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27159, { 0, 0, height + 24 },
                            { 16, 32, 3 }, { 16, 0, height + 24 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27164, { 0, 0, height + 24 },
                            { 16, 32, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27169, { 0, 0, height + 24 },
                            { 16, 32, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27154, { 0, 0, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27158, { 0, 0, height + 24 },
                            { 20, 32, 3 }, { 6, 0, height + 24 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27163, { 0, 0, height + 24 },
                            { 20, 32, 3 }, { 6, 0, height + 24 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27168, { 0, 0, height + 24 },
                            { 20, 32, 3 }, { 6, 0, height + 24 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27153, { 0, 0, height + 24 },
                            { 20, 32, 3 }, { 6, 0, height + 24 });
                        break;
                }

                paint_util_set_segment_support_height(
                    session,
                    paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D4, direction),
                    0xFFFF, 0);
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);

                switch (direction)
                {
                    case 2:
                        paint_util_push_tunnel_right(session, height, TUNNEL_SQUARE_FLAT);
                        break;
                    case 3:
                        paint_util_push_tunnel_left(session, height, TUNNEL_SQUARE_FLAT);
                        break;
                }
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
        }
    }
}

/** rct2: 0x007C7134 */
static void flying_rc_track_banked_right_quarter_turn_5(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    flying_rc_track_banked_left_quarter_turn_5(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x007C7144 */
static void flying_rc_track_left_bank_to_25_deg_up(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17168, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17172, { 0, 0, height }, { 32, 1, 34 },
                    { 0, 27, height });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17169, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17173, { 0, 0, height }, { 32, 1, 34 },
                    { 0, 27, height });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17170, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17171, { 0, 0, height }, { 32, 20, 3 },
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
            paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
        }
        else
        {
            paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_8);
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
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27277, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 32 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27278, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 32 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27279, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 32 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27280, { 0, 0, height + 24 }, { 32, 20, 3 },
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
                        session, METAL_SUPPORTS_TUBES_INVERTED, 6, 0, height + 47, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height + 47, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 7, 0, height + 47, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height + 47, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
        }

        if (direction == 0 || direction == 3)
        {
            paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
        }
        else
        {
            paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_8);
        }
        paint_util_set_general_support_height(session, height + 48, 0x20);
    }
}

/** rct2: 0x007C7154 */
static void flying_rc_track_right_bank_to_25_deg_up(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17174, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17175, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17176, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17178, { 0, 0, height }, { 32, 1, 34 },
                    { 0, 27, height });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17177, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17179, { 0, 0, height }, { 32, 1, 34 },
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
            paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
        }
        else
        {
            paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_8);
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
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27281, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 32 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27282, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 32 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27283, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 32 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27284, { 0, 0, height + 24 }, { 32, 20, 3 },
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
                        session, METAL_SUPPORTS_TUBES_INVERTED, 6, 0, height + 47, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height + 47, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 7, 0, height + 47, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height + 47, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
        }

        if (direction == 0 || direction == 3)
        {
            paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
        }
        else
        {
            paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_8);
        }
        paint_util_set_general_support_height(session, height + 48, 0x20);
    }
}

/** rct2: 0x007C7164 */
static void flying_rc_track_25_deg_up_to_left_bank(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17180, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17184, { 0, 0, height }, { 32, 1, 34 },
                    { 0, 27, height });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17181, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17185, { 0, 0, height }, { 32, 1, 34 },
                    { 0, 27, height });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17182, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17183, { 0, 0, height }, { 32, 20, 3 },
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
            paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_SQUARE_FLAT);
        }
        else
        {
            paint_util_push_tunnel_rotated(session, direction, height + 8, TUNNEL_14);
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
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27285, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 32 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27286, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 32 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27287, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 32 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27288, { 0, 0, height + 24 }, { 32, 20, 3 },
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
                        session, METAL_SUPPORTS_TUBES_INVERTED, 6, 0, height + 47, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height + 47, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 7, 0, height + 47, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height + 47, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
        }

        if (direction == 0 || direction == 3)
        {
            paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_SQUARE_FLAT);
        }
        else
        {
            paint_util_push_tunnel_rotated(session, direction, height + 8, TUNNEL_14);
        }
        paint_util_set_general_support_height(session, height + 40, 0x20);
    }
}

/** rct2: 0x007C7174 */
static void flying_rc_track_25_deg_up_to_right_bank(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17186, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17187, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17188, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17190, { 0, 0, height }, { 32, 1, 34 },
                    { 0, 27, height });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17189, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17191, { 0, 0, height }, { 32, 1, 34 },
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
            paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_SQUARE_FLAT);
        }
        else
        {
            paint_util_push_tunnel_rotated(session, direction, height + 8, TUNNEL_14);
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
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27289, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 32 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27290, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 32 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27291, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 32 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27292, { 0, 0, height + 24 }, { 32, 20, 3 },
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
                        session, METAL_SUPPORTS_TUBES_INVERTED, 6, 0, height + 47, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height + 47, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 7, 0, height + 47, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height + 47, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
        }

        if (direction == 0 || direction == 3)
        {
            paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_SQUARE_FLAT);
        }
        else
        {
            paint_util_push_tunnel_rotated(session, direction, height + 8, TUNNEL_14);
        }
        paint_util_set_general_support_height(session, height + 40, 0x20);
    }
}

/** rct2: 0x007C7184 */
static void flying_rc_track_left_bank_to_25_deg_down(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    flying_rc_track_25_deg_up_to_right_bank(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x007C7194 */
static void flying_rc_track_right_bank_to_25_deg_down(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    flying_rc_track_25_deg_up_to_left_bank(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x007C71A4 */
static void flying_rc_track_25_deg_down_to_left_bank(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    flying_rc_track_right_bank_to_25_deg_up(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x007C71B4 */
static void flying_rc_track_25_deg_down_to_right_bank(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    flying_rc_track_left_bank_to_25_deg_up(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x007C71C4 */
static void flying_rc_track_left_bank(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17192, { 0, 0, height }, { 32, 1, 26 },
                    { 0, 27, height });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17193, { 0, 0, height }, { 32, 1, 26 },
                    { 0, 27, height });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17194, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17195, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
        }
        if (track_paint_util_should_paint_supports(session.MapPosition))
        {
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
        }
        paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
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
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27293, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 22 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27294, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 22 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27295, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 22 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27296, { 0, 0, height + 24 }, { 32, 20, 3 },
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
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
        }

        paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
        paint_util_set_general_support_height(session, height + 32, 0x20);
    }
}

/** rct2: 0x007C71D4 */
static void flying_rc_track_right_bank(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    flying_rc_track_left_bank(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x007C71E4 */
static void flying_rc_track_left_quarter_turn_5_25_deg_up(
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17344, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17349, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17354, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17359, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                }
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                if (direction == 0 || direction == 3)
                {
                    paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_SQUARE_7);
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17345, { 0, 0, height }, { 32, 16, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17350, { 0, 0, height }, { 32, 16, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17355, { 0, 0, height }, { 32, 16, 3 },
                            { 0, 16, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17360, { 0, 0, height }, { 32, 16, 3 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17346, { 0, 0, height }, { 16, 16, 3 },
                            { 0, 16, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17351, { 0, 0, height }, { 16, 16, 3 },
                            { 16, 16, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17356, { 0, 0, height }, { 16, 16, 3 },
                            { 16, 0, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17361, { 0, 0, height }, { 16, 16, 3 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17347, { 0, 0, height }, { 16, 32, 3 },
                            { 16, 0, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17352, { 0, 0, height }, { 16, 32, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17357, { 0, 0, height }, { 16, 32, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17362, { 0, 0, height }, { 16, 32, 3 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17348, { 0, 0, height }, { 20, 32, 3 },
                            { 6, 0, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17353, { 0, 0, height }, { 20, 32, 3 },
                            { 6, 0, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17358, { 0, 0, height }, { 20, 32, 3 },
                            { 6, 0, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17363, { 0, 0, height }, { 20, 32, 3 },
                            { 6, 0, height });
                        break;
                }
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                switch (direction)
                {
                    case 2:
                        paint_util_push_tunnel_right(session, height + 8, TUNNEL_SQUARE_8);
                        break;
                    case 3:
                        paint_util_push_tunnel_left(session, height + 8, TUNNEL_SQUARE_8);
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27317, { 0, 6, height + 24 },
                            { 32, 20, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27322, { 0, 6, height + 24 },
                            { 32, 20, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27327, { 0, 6, height + 24 },
                            { 32, 20, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27332, { 0, 6, height + 24 },
                            { 32, 20, 3 });
                        break;
                }

                paint_util_set_segment_support_height(
                    session,
                    paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 49, session.TrackColours[SCHEME_SUPPORTS]);

                if (direction == 0 || direction == 3)
                {
                    paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_SQUARE_7);
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27318, { 0, 0, height + 24 },
                            { 32, 16, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27323, { 0, 0, height + 24 },
                            { 32, 16, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27328, { 0, 16, height + 24 },
                            { 32, 16, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27333, { 0, 16, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27319, { 0, 16, height + 24 },
                            { 16, 16, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27324, { 16, 16, height + 24 },
                            { 16, 16, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27329, { 16, 0, height + 24 },
                            { 16, 16, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27334, { 0, 0, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27320, { 16, 0, height + 24 },
                            { 16, 32, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27325, { 0, 0, height + 24 },
                            { 16, 32, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27330, { 0, 0, height + 24 },
                            { 16, 32, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27335, { 16, 0, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27321, { 6, 0, height + 24 },
                            { 20, 32, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27326, { 6, 0, height + 24 },
                            { 20, 32, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27331, { 6, 0, height + 24 },
                            { 20, 32, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27336, { 6, 0, height + 24 },
                            { 20, 32, 3 });
                        break;
                }

                paint_util_set_segment_support_height(
                    session,
                    paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D4, direction),
                    0xFFFF, 0);
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 49, session.TrackColours[SCHEME_SUPPORTS]);

                switch (direction)
                {
                    case 2:
                        paint_util_push_tunnel_right(session, height + 8, TUNNEL_SQUARE_8);
                        break;
                    case 3:
                        paint_util_push_tunnel_left(session, height + 8, TUNNEL_SQUARE_8);
                        break;
                }
                paint_util_set_general_support_height(session, height + 72, 0x20);
                break;
        }
    }
}

/** rct2: 0x007C71F4 */
static void flying_rc_track_right_quarter_turn_5_25_deg_up(
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17324, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17329, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17334, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17339, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                }
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                if (direction == 0 || direction == 3)
                {
                    paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_SQUARE_7);
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17325, { 0, 0, height }, { 32, 16, 3 },
                            { 0, 16, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17330, { 0, 0, height }, { 32, 16, 3 },
                            { 0, 16, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17335, { 0, 0, height }, { 32, 16, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17340, { 0, 0, height }, { 32, 16, 3 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17326, { 0, 0, height }, { 16, 16, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17331, { 0, 0, height }, { 16, 16, 3 },
                            { 16, 0, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17336, { 0, 0, height }, { 16, 16, 3 },
                            { 16, 16, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17341, { 0, 0, height }, { 16, 16, 3 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17327, { 0, 0, height }, { 16, 32, 3 },
                            { 16, 0, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17332, { 0, 0, height }, { 16, 32, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17337, { 0, 0, height }, { 16, 32, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17342, { 0, 0, height }, { 16, 32, 3 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17328, { 0, 0, height }, { 20, 32, 3 },
                            { 6, 0, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17333, { 0, 0, height }, { 20, 32, 3 },
                            { 6, 0, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17338, { 0, 0, height }, { 20, 32, 3 },
                            { 6, 0, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17343, { 0, 0, height }, { 20, 32, 3 },
                            { 6, 0, height });
                        break;
                }
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                switch (direction)
                {
                    case 0:
                        paint_util_push_tunnel_right(session, height + 8, TUNNEL_SQUARE_8);
                        break;
                    case 1:
                        paint_util_push_tunnel_left(session, height + 8, TUNNEL_SQUARE_8);
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27297, { 0, 6, height + 24 },
                            { 32, 20, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27302, { 0, 6, height + 24 },
                            { 32, 20, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27307, { 0, 6, height + 24 },
                            { 32, 20, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27312, { 0, 6, height + 24 },
                            { 32, 20, 3 });
                        break;
                }

                paint_util_set_segment_support_height(
                    session,
                    paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                    0xFFFF, 0);
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 49, session.TrackColours[SCHEME_SUPPORTS]);

                if (direction == 0 || direction == 3)
                {
                    paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_SQUARE_7);
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27298, { 0, 16, height + 24 },
                            { 32, 16, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27303, { 0, 16, height + 24 },
                            { 32, 16, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27308, { 0, 0, height + 24 },
                            { 32, 16, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27313, { 0, 0, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27299, { 0, 0, height + 24 },
                            { 16, 16, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27304, { 16, 0, height + 24 },
                            { 16, 16, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27309, { 16, 16, height + 24 },
                            { 16, 16, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27314, { 0, 16, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27300, { 16, 0, height + 24 },
                            { 16, 32, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27305, { 0, 0, height + 24 },
                            { 16, 32, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27310, { 0, 0, height + 24 },
                            { 16, 32, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27315, { 16, 0, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27301, { 6, 0, height + 24 },
                            { 20, 32, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27306, { 6, 0, height + 24 },
                            { 20, 32, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27311, { 6, 0, height + 24 },
                            { 20, 32, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27316, { 6, 0, height + 24 },
                            { 20, 32, 3 });
                        break;
                }

                paint_util_set_segment_support_height(
                    session,
                    paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D4, direction),
                    0xFFFF, 0);
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 49, session.TrackColours[SCHEME_SUPPORTS]);

                switch (direction)
                {
                    case 0:
                        paint_util_push_tunnel_right(session, height + 8, TUNNEL_SQUARE_8);
                        break;
                    case 1:
                        paint_util_push_tunnel_left(session, height + 8, TUNNEL_SQUARE_8);
                        break;
                }
                paint_util_set_general_support_height(session, height + 72, 0x20);
                break;
        }
    }
}

/** rct2: 0x007C7204 */
static void flying_rc_track_left_quarter_turn_5_25_deg_down(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    flying_rc_track_right_quarter_turn_5_25_deg_up(session, ride, trackSequence, (direction + 1) & 3, height, trackElement);
}

/** rct2: 0x007C7214 */
static void flying_rc_track_right_quarter_turn_5_25_deg_down(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    flying_rc_track_left_quarter_turn_5_25_deg_up(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x007C7224 */
static void flying_rc_track_s_bend_left(
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17308, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17312, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17311, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17315, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                }
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                if (direction == 0 || direction == 3)
                {
                    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17309, { 0, 0, height }, { 32, 26, 3 });
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17313, { 0, 0, height }, { 32, 26, 3 });
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 6, 1, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17310, { 0, 0, height }, { 32, 26, 3 },
                            { 0, 6, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17314, { 0, 0, height }, { 32, 26, 3 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17310, { 0, 0, height }, { 32, 26, 3 },
                            { 0, 6, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17314, { 0, 0, height }, { 32, 26, 3 },
                            { 0, 6, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17309, { 0, 0, height }, { 32, 26, 3 });
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17313, { 0, 0, height }, { 32, 26, 3 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17311, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17315, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17308, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17312, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                }
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                switch (direction)
                {
                    case 1:
                        paint_util_push_tunnel_right(session, height, TUNNEL_SQUARE_FLAT);
                        break;
                    case 2:
                        paint_util_push_tunnel_left(session, height, TUNNEL_SQUARE_FLAT);
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27253, { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27257, { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27256, { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27260, { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                }

                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF,
                    0);
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);

                if (direction == 0 || direction == 3)
                {
                    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
                }
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27254, { 0, 0, height + 24 },
                            { 32, 26, 3 }, { 0, 0, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27258, { 0, 0, height + 24 },
                            { 32, 26, 3 }, { 0, 0, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27255, { 0, 0, height + 24 },
                            { 32, 26, 3 }, { 0, 6, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27259, { 0, 0, height + 24 },
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
                            session, METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 6, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27255, { 0, 0, height + 24 },
                            { 32, 26, 3 }, { 0, 6, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27259, { 0, 0, height + 24 },
                            { 32, 26, 3 }, { 0, 6, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27254, { 0, 0, height + 24 },
                            { 32, 26, 3 }, { 0, 0, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27258, { 0, 0, height + 24 },
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
                            session, METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 6, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27256, { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27260, { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27253, { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27257, { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                }

                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF,
                    0);
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);

                switch (direction)
                {
                    case 1:
                        paint_util_push_tunnel_right(session, height, TUNNEL_SQUARE_FLAT);
                        break;
                    case 2:
                        paint_util_push_tunnel_left(session, height, TUNNEL_SQUARE_FLAT);
                        break;
                }
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
        }
    }
}

/** rct2: 0x007C7234 */
static void flying_rc_track_s_bend_right(
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17316, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17320, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17319, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17323, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                }
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                if (direction == 0 || direction == 3)
                {
                    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17317, { 0, 0, height }, { 32, 26, 3 },
                            { 0, 6, height });
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17321, { 0, 0, height }, { 32, 26, 3 },
                            { 0, 6, height });
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17318, { 0, 0, height }, { 32, 26, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17322, { 0, 0, height }, { 32, 26, 3 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17318, { 0, 0, height }, { 32, 26, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17322, { 0, 0, height }, { 32, 26, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17317, { 0, 0, height }, { 32, 26, 3 },
                            { 0, 6, height });
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17321, { 0, 0, height }, { 32, 26, 3 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17319, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17323, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17316, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17320, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                }
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                switch (direction)
                {
                    case 1:
                        paint_util_push_tunnel_right(session, height, TUNNEL_SQUARE_FLAT);
                        break;
                    case 2:
                        paint_util_push_tunnel_left(session, height, TUNNEL_SQUARE_FLAT);
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27261, { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27265, { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27264, { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27268, { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                }

                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF,
                    0);
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);

                if (direction == 0 || direction == 3)
                {
                    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
                }
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27262, { 0, 0, height + 24 },
                            { 32, 26, 3 }, { 0, 6, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27266, { 0, 0, height + 24 },
                            { 32, 26, 3 }, { 0, 6, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27263, { 0, 0, height + 24 },
                            { 32, 26, 3 }, { 0, 0, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27267, { 0, 0, height + 24 },
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
                            session, METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 7, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27263, { 0, 0, height + 24 },
                            { 32, 26, 3 }, { 0, 0, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27267, { 0, 0, height + 24 },
                            { 32, 26, 3 }, { 0, 0, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27262, { 0, 0, height + 24 },
                            { 32, 26, 3 }, { 0, 6, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27266, { 0, 0, height + 24 },
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
                            session, METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 7, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27264, { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27268, { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27261, { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27265, { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                }

                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF,
                    0);
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);

                switch (direction)
                {
                    case 1:
                        paint_util_push_tunnel_right(session, height, TUNNEL_SQUARE_FLAT);
                        break;
                    case 2:
                        paint_util_push_tunnel_left(session, height, TUNNEL_SQUARE_FLAT);
                        break;
                }
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
        }
    }
}

/** rct2: 0x007C7274 */
static void flying_rc_track_left_quarter_turn_3(
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17229, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17232, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17235, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17226, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                }
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                if (direction == 0 || direction == 3)
                {
                    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17228, { 0, 0, height }, { 16, 16, 3 },
                            { 16, 0, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17231, { 0, 0, height }, { 16, 16, 3 },
                            { 0, 0, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17234, { 0, 0, height }, { 16, 16, 3 },
                            { 0, 16, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17225, { 0, 0, height }, { 16, 16, 3 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17227, { 0, 0, height }, { 20, 32, 3 },
                            { 6, 0, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17230, { 0, 0, height }, { 20, 32, 3 },
                            { 6, 0, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17233, { 0, 0, height }, { 20, 32, 3 },
                            { 6, 0, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17224, { 0, 0, height }, { 20, 32, 3 },
                            { 6, 0, height });
                        break;
                }
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                switch (direction)
                {
                    case 2:
                        paint_util_push_tunnel_right(session, height, TUNNEL_SQUARE_FLAT);
                        break;
                    case 3:
                        paint_util_push_tunnel_left(session, height, TUNNEL_SQUARE_FLAT);
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27392, { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27395, { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27398, { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27389, { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                }

                paint_util_set_segment_support_height(
                    session,
                    paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);

                if (direction == 0 || direction == 3)
                {
                    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27391, { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 16, 0, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27394, { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 0, 0, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27397, { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 0, 16, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27388, { 0, 0, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27390, { 0, 0, height + 24 },
                            { 20, 32, 3 }, { 6, 0, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27393, { 0, 0, height + 24 },
                            { 20, 32, 3 }, { 6, 0, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27396, { 0, 0, height + 24 },
                            { 20, 32, 3 }, { 6, 0, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27387, { 0, 0, height + 24 },
                            { 20, 32, 3 }, { 6, 0, height + 22 });
                        break;
                }

                paint_util_set_segment_support_height(
                    session,
                    paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D4, direction),
                    0xFFFF, 0);
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);

                switch (direction)
                {
                    case 2:
                        paint_util_push_tunnel_right(session, height, TUNNEL_SQUARE_FLAT);
                        break;
                    case 3:
                        paint_util_push_tunnel_left(session, height, TUNNEL_SQUARE_FLAT);
                        break;
                }
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
        }
    }
}

/** rct2: 0x007C7284 */
static void flying_rc_track_right_quarter_turn_3(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    flying_rc_track_left_quarter_turn_3(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x007C7294 */
static void flying_rc_track_left_quarter_turn_3_bank(
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17241, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17248, { 0, 0, height }, { 32, 1, 26 },
                            { 0, 27, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17244, { 0, 0, height }, { 32, 1, 26 },
                            { 0, 27, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17247, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17238, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                }
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                if (direction == 0 || direction == 3)
                {
                    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17240, { 0, 0, height }, { 16, 16, 3 },
                            { 16, 0, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17243, { 0, 0, height }, { 16, 16, 1 },
                            { 0, 0, height + 27 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17246, { 0, 0, height }, { 16, 16, 3 },
                            { 0, 16, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17237, { 0, 0, height }, { 16, 16, 3 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17239, { 0, 0, height }, { 20, 32, 3 },
                            { 6, 0, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17242, { 0, 0, height }, { 1, 32, 26 },
                            { 27, 0, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17245, { 0, 0, height }, { 20, 32, 3 },
                            { 6, 0, height });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17249, { 0, 0, height }, { 1, 32, 26 },
                            { 27, 0, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17236, { 0, 0, height }, { 20, 32, 3 },
                            { 6, 0, height });
                        break;
                }
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                switch (direction)
                {
                    case 2:
                        paint_util_push_tunnel_right(session, height, TUNNEL_SQUARE_FLAT);
                        break;
                    case 3:
                        paint_util_push_tunnel_left(session, height, TUNNEL_SQUARE_FLAT);
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27404, { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27407, { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27410, { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27401, { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                }

                paint_util_set_segment_support_height(
                    session,
                    paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);

                if (direction == 0 || direction == 3)
                {
                    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27403, { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 16, 0, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27406, { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 0, 0, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27409, { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 0, 16, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27400, { 0, 0, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27402, { 0, 0, height + 24 },
                            { 20, 32, 3 }, { 6, 0, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27405, { 0, 0, height + 24 },
                            { 20, 32, 3 }, { 6, 0, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27408, { 0, 0, height + 24 },
                            { 20, 32, 3 }, { 6, 0, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27399, { 0, 0, height + 24 },
                            { 20, 32, 3 }, { 6, 0, height + 22 });
                        break;
                }

                paint_util_set_segment_support_height(
                    session,
                    paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D4, direction),
                    0xFFFF, 0);
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);

                switch (direction)
                {
                    case 2:
                        paint_util_push_tunnel_right(session, height, TUNNEL_SQUARE_FLAT);
                        break;
                    case 3:
                        paint_util_push_tunnel_left(session, height, TUNNEL_SQUARE_FLAT);
                        break;
                }
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
        }
    }
}

/** rct2: 0x007C72A4 */
static void flying_rc_track_right_quarter_turn_3_bank(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    flying_rc_track_left_quarter_turn_3_bank(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x007C72B4 */
static void flying_rc_track_left_quarter_turn_3_25_deg_up(
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17375, { 0, 6, height }, { 32, 20, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17377, { 0, 6, height }, { 32, 20, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17379, { 0, 6, height }, { 32, 20, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17373, { 0, 6, height }, { 32, 20, 3 });
                        break;
                }
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                if (direction == 0 || direction == 3)
                {
                    paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_SQUARE_7);
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17374, { 6, 0, height }, { 20, 32, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17376, { 6, 0, height }, { 20, 32, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17378, { 6, 0, height }, { 20, 32, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17372, { 6, 0, height }, { 20, 32, 3 });
                        break;
                }
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                switch (direction)
                {
                    case 2:
                        paint_util_push_tunnel_right(session, height + 8, TUNNEL_SQUARE_8);
                        break;
                    case 3:
                        paint_util_push_tunnel_left(session, height + 8, TUNNEL_SQUARE_8);
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27422, { 0, 6, height + 24 },
                            { 32, 20, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27424, { 0, 6, height + 24 },
                            { 32, 20, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27426, { 0, 6, height + 24 },
                            { 32, 20, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27420, { 0, 6, height + 24 },
                            { 32, 20, 3 });
                        break;
                }

                paint_util_set_segment_support_height(
                    session,
                    paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 47, session.TrackColours[SCHEME_SUPPORTS]);

                if (direction == 0 || direction == 3)
                {
                    paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_SQUARE_7);
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27421, { 6, 0, height + 24 },
                            { 20, 32, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27423, { 6, 0, height + 24 },
                            { 20, 32, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27425, { 6, 0, height + 24 },
                            { 20, 32, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27419, { 6, 0, height + 24 },
                            { 20, 32, 3 });
                        break;
                }

                paint_util_set_segment_support_height(
                    session,
                    paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D4, direction),
                    0xFFFF, 0);
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 47, session.TrackColours[SCHEME_SUPPORTS]);

                switch (direction)
                {
                    case 2:
                        paint_util_push_tunnel_right(session, height + 8, TUNNEL_SQUARE_8);
                        break;
                    case 3:
                        paint_util_push_tunnel_left(session, height + 8, TUNNEL_SQUARE_8);
                        break;
                }
                paint_util_set_general_support_height(session, height + 72, 0x20);
                break;
        }
    }
}

/** rct2: 0x007C72C4 */
static void flying_rc_track_right_quarter_turn_3_25_deg_up(
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17364, { 0, 6, height }, { 32, 20, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17366, { 0, 6, height }, { 32, 20, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17368, { 0, 6, height }, { 32, 20, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17370, { 0, 6, height }, { 32, 20, 3 });
                        break;
                }
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                if (direction == 0 || direction == 3)
                {
                    paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_SQUARE_7);
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17365, { 6, 0, height }, { 20, 32, 3 });
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17367, { 6, 0, height }, { 20, 32, 3 });
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17369, { 6, 0, height }, { 20, 32, 3 });
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 4, 10, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17371, { 6, 0, height }, { 20, 32, 3 });
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
                switch (direction)
                {
                    case 0:
                        paint_util_push_tunnel_right(session, height + 8, TUNNEL_SQUARE_8);
                        break;
                    case 1:
                        paint_util_push_tunnel_left(session, height + 8, TUNNEL_SQUARE_8);
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27411, { 0, 6, height + 24 },
                            { 32, 20, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27413, { 0, 6, height + 24 },
                            { 32, 20, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27415, { 0, 6, height + 24 },
                            { 32, 20, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27417, { 0, 6, height + 24 },
                            { 32, 20, 3 });
                        break;
                }

                paint_util_set_segment_support_height(
                    session,
                    paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                    0xFFFF, 0);
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 47, session.TrackColours[SCHEME_SUPPORTS]);

                if (direction == 0 || direction == 3)
                {
                    paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_SQUARE_7);
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27412, { 6, 0, height + 24 },
                            { 20, 32, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27414, { 6, 0, height + 24 },
                            { 20, 32, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27416, { 6, 0, height + 24 },
                            { 20, 32, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27418, { 6, 0, height + 24 },
                            { 20, 32, 3 });
                        break;
                }

                paint_util_set_segment_support_height(
                    session,
                    paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D4, direction),
                    0xFFFF, 0);
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 47, session.TrackColours[SCHEME_SUPPORTS]);

                switch (direction)
                {
                    case 0:
                        paint_util_push_tunnel_right(session, height + 8, TUNNEL_SQUARE_8);
                        break;
                    case 1:
                        paint_util_push_tunnel_left(session, height + 8, TUNNEL_SQUARE_8);
                        break;
                }
                paint_util_set_general_support_height(session, height + 72, 0x20);
                break;
        }
    }
}

/** rct2: 0x007C72D4 */
static void flying_rc_track_left_quarter_turn_3_25_deg_down(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    flying_rc_track_right_quarter_turn_3_25_deg_up(session, ride, trackSequence, (direction + 1) & 3, height, trackElement);
}

/** rct2: 0x007C72E4 */
static void flying_rc_track_right_quarter_turn_3_25_deg_down(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    flying_rc_track_left_quarter_turn_3_25_deg_up(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x007C7314 */
static void flying_rc_track_left_half_banked_helix_up_small(
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17399, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17406, { 0, 0, height }, { 32, 1, 26 },
                        { 0, 27, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17402, { 0, 0, height }, { 32, 1, 26 },
                        { 0, 27, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17405, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17396, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
            }
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 2, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17398, { 0, 0, height }, { 16, 16, 3 },
                        { 16, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17401, { 0, 0, height }, { 16, 16, 1 },
                        { 0, 0, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17404, { 0, 0, height }, { 16, 16, 3 },
                        { 0, 16, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17395, { 0, 0, height }, { 16, 16, 3 },
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17397, { 0, 0, height }, { 20, 32, 3 },
                        { 6, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17400, { 0, 0, height }, { 1, 32, 26 },
                        { 27, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17403, { 0, 0, height }, { 20, 32, 3 },
                        { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17407, { 0, 0, height }, { 1, 32, 26 },
                        { 27, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17394, { 0, 0, height }, { 20, 32, 3 },
                        { 6, 0, height });
                    break;
            }
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
            switch (direction)
            {
                case 2:
                    paint_util_push_tunnel_right(session, height + 8, TUNNEL_SQUARE_FLAT);
                    break;
                case 3:
                    paint_util_push_tunnel_left(session, height + 8, TUNNEL_SQUARE_FLAT);
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17396, { 0, 0, height }, { 20, 32, 3 },
                        { 6, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17399, { 0, 0, height }, { 20, 32, 3 },
                        { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17406, { 0, 0, height }, { 1, 32, 26 },
                        { 27, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17402, { 0, 0, height }, { 1, 32, 26 },
                        { 27, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17405, { 0, 0, height }, { 20, 32, 3 },
                        { 6, 0, height });
                    break;
            }
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 2, height, session.TrackColours[SCHEME_SUPPORTS]);
            switch (direction)
            {
                case 0:
                    paint_util_push_tunnel_right(session, height, TUNNEL_SQUARE_FLAT);
                    break;
                case 1:
                    paint_util_push_tunnel_left(session, height, TUNNEL_SQUARE_FLAT);
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17395, { 0, 0, height }, { 16, 16, 3 },
                        { 16, 16, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17398, { 0, 0, height }, { 16, 16, 3 },
                        { 0, 16, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17401, { 0, 0, height }, { 16, 16, 1 },
                        { 0, 0, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17404, { 0, 0, height }, { 16, 16, 3 },
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17394, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17397, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17400, { 0, 0, height }, { 32, 1, 26 },
                        { 0, 27, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17403, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17407, { 0, 0, height }, { 32, 1, 26 },
                        { 0, 27, height });
                    break;
            }
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height + 8, TUNNEL_SQUARE_FLAT);
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x007C7324 */
static void flying_rc_track_right_half_banked_helix_up_small(
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17380, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17383, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17386, { 0, 0, height }, { 32, 1, 26 },
                        { 0, 27, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17389, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17393, { 0, 0, height }, { 32, 1, 26 },
                        { 0, 27, height });
                    break;
            }
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 2, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17381, { 0, 0, height }, { 16, 16, 3 },
                        { 16, 16, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17384, { 0, 0, height }, { 16, 16, 3 },
                        { 0, 16, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17387, { 0, 0, height }, { 16, 16, 1 },
                        { 0, 0, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17390, { 0, 0, height }, { 16, 16, 3 },
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17382, { 0, 0, height }, { 20, 32, 3 },
                        { 6, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17385, { 0, 0, height }, { 20, 32, 3 },
                        { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17392, { 0, 0, height }, { 1, 32, 26 },
                        { 27, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17388, { 0, 0, height }, { 1, 32, 26 },
                        { 27, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17391, { 0, 0, height }, { 20, 32, 3 },
                        { 6, 0, height });
                    break;
            }
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
            switch (direction)
            {
                case 0:
                    paint_util_push_tunnel_right(session, height + 8, TUNNEL_SQUARE_FLAT);
                    break;
                case 1:
                    paint_util_push_tunnel_left(session, height + 8, TUNNEL_SQUARE_FLAT);
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17383, { 0, 0, height }, { 20, 32, 3 },
                        { 6, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17386, { 0, 0, height }, { 1, 32, 26 },
                        { 27, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17389, { 0, 0, height }, { 20, 32, 3 },
                        { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17393, { 0, 0, height }, { 1, 32, 26 },
                        { 27, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17380, { 0, 0, height }, { 20, 32, 3 },
                        { 6, 0, height });
                    break;
            }
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 2, height, session.TrackColours[SCHEME_SUPPORTS]);
            switch (direction)
            {
                case 2:
                    paint_util_push_tunnel_right(session, height, TUNNEL_SQUARE_FLAT);
                    break;
                case 3:
                    paint_util_push_tunnel_left(session, height, TUNNEL_SQUARE_FLAT);
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17384, { 0, 0, height }, { 16, 16, 3 },
                        { 16, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17387, { 0, 0, height }, { 16, 16, 1 },
                        { 0, 0, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17390, { 0, 0, height }, { 16, 16, 3 },
                        { 0, 16, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17381, { 0, 0, height }, { 16, 16, 3 },
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17385, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17392, { 0, 0, height }, { 32, 1, 26 },
                        { 0, 27, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17388, { 0, 0, height }, { 32, 1, 26 },
                        { 0, 27, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17391, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17382, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
            }
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height + 8, TUNNEL_SQUARE_FLAT);
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x007C7334 */
static void flying_rc_track_left_half_banked_helix_down_small(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackSequence >= 4)
    {
        trackSequence -= 4;
        direction = (direction - 1) & 3;
    }
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    flying_rc_track_right_half_banked_helix_up_small(session, ride, trackSequence, (direction + 1) & 3, height, trackElement);
}

/** rct2: 0x007C7344 */
static void flying_rc_track_right_half_banked_helix_down_small(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackSequence >= 4)
    {
        trackSequence -= 4;
        direction = (direction + 1) & 3;
    }
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    flying_rc_track_left_half_banked_helix_up_small(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x007C7354 */
static void flying_rc_track_left_half_banked_helix_up_large(
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17439, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17450, { 0, 0, height }, { 32, 1, 26 },
                        { 0, 27, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17444, { 0, 0, height }, { 32, 1, 26 },
                        { 0, 27, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17449, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17434, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
            }
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 1, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17438, { 0, 0, height }, { 32, 16, 3 },
                        { 0, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17443, { 0, 0, height }, { 32, 16, 1 },
                        { 0, 0, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17448, { 0, 0, height }, { 32, 16, 3 },
                        { 0, 16, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17433, { 0, 0, height }, { 32, 16, 3 },
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17437, { 0, 0, height }, { 16, 16, 3 },
                        { 0, 16, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17442, { 0, 0, height }, { 16, 16, 1 },
                        { 16, 16, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17447, { 0, 0, height }, { 16, 16, 3 },
                        { 16, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17432, { 0, 0, height }, { 16, 16, 3 },
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17436, { 0, 0, height }, { 16, 32, 3 },
                        { 16, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17441, { 0, 0, height }, { 16, 32, 1 },
                        { 0, 0, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17446, { 0, 0, height }, { 16, 32, 3 },
                        { 0, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17431, { 0, 0, height }, { 16, 32, 3 },
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17435, { 0, 0, height }, { 20, 32, 3 },
                        { 6, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17440, { 0, 0, height }, { 1, 32, 26 },
                        { 27, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17445, { 0, 0, height }, { 20, 32, 3 },
                        { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17451, { 0, 0, height }, { 1, 32, 26 },
                        { 27, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17430, { 0, 0, height }, { 20, 32, 3 },
                        { 6, 0, height });
                    break;
            }
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 7, height, session.TrackColours[SCHEME_SUPPORTS]);
            switch (direction)
            {
                case 2:
                    paint_util_push_tunnel_right(session, height + 8, TUNNEL_SQUARE_FLAT);
                    break;
                case 3:
                    paint_util_push_tunnel_left(session, height + 8, TUNNEL_SQUARE_FLAT);
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17434, { 0, 0, height }, { 20, 32, 3 },
                        { 6, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17439, { 0, 0, height }, { 20, 32, 3 },
                        { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17450, { 0, 0, height }, { 1, 32, 26 },
                        { 27, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17444, { 0, 0, height }, { 1, 32, 26 },
                        { 27, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17449, { 0, 0, height }, { 20, 32, 3 },
                        { 6, 0, height });
                    break;
            }
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 1, height, session.TrackColours[SCHEME_SUPPORTS]);
            switch (direction)
            {
                case 0:
                    paint_util_push_tunnel_right(session, height, TUNNEL_SQUARE_FLAT);
                    break;
                case 1:
                    paint_util_push_tunnel_left(session, height, TUNNEL_SQUARE_FLAT);
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17433, { 0, 0, height }, { 16, 32, 3 },
                        { 16, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17438, { 0, 0, height }, { 16, 32, 3 },
                        { 0, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17443, { 0, 0, height }, { 16, 32, 1 },
                        { 0, 0, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17448, { 0, 0, height }, { 16, 32, 3 },
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17432, { 0, 0, height }, { 16, 16, 3 },
                        { 0, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17437, { 0, 0, height }, { 16, 16, 3 },
                        { 16, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17442, { 0, 0, height }, { 16, 16, 1 },
                        { 16, 16, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17447, { 0, 0, height }, { 16, 16, 3 },
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17431, { 0, 0, height }, { 32, 16, 3 },
                        { 0, 16, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17436, { 0, 0, height }, { 32, 16, 3 },
                        { 0, 16, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17441, { 0, 0, height }, { 32, 16, 1 },
                        { 0, 0, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17446, { 0, 0, height }, { 32, 16, 3 },
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17430, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17435, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17440, { 0, 0, height }, { 32, 1, 26 },
                        { 0, 27, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17445, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17451, { 0, 0, height }, { 32, 1, 26 },
                        { 0, 27, height });
                    break;
            }
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 7, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height + 8, TUNNEL_SQUARE_FLAT);
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x007C7364 */
static void flying_rc_track_right_half_banked_helix_up_large(
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17408, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17413, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17418, { 0, 0, height }, { 32, 1, 26 },
                        { 0, 27, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17423, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17429, { 0, 0, height }, { 32, 1, 26 },
                        { 0, 27, height });
                    break;
            }
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 1, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17409, { 0, 0, height }, { 32, 16, 3 },
                        { 0, 16, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17414, { 0, 0, height }, { 32, 16, 3 },
                        { 0, 16, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17419, { 0, 0, height }, { 32, 16, 1 },
                        { 0, 0, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17424, { 0, 0, height }, { 32, 16, 3 },
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17410, { 0, 0, height }, { 16, 16, 3 },
                        { 0, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17415, { 0, 0, height }, { 16, 16, 3 },
                        { 16, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17420, { 0, 0, height }, { 16, 16, 1 },
                        { 16, 16, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17425, { 0, 0, height }, { 16, 16, 3 },
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17411, { 0, 0, height }, { 16, 32, 3 },
                        { 16, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17416, { 0, 0, height }, { 16, 32, 3 },
                        { 0, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17421, { 0, 0, height }, { 16, 32, 1 },
                        { 0, 0, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17426, { 0, 0, height }, { 16, 32, 3 },
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17412, { 0, 0, height }, { 20, 32, 3 },
                        { 6, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17417, { 0, 0, height }, { 20, 32, 3 },
                        { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17428, { 0, 0, height }, { 1, 32, 26 },
                        { 27, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17422, { 0, 0, height }, { 1, 32, 26 },
                        { 27, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17427, { 0, 0, height }, { 20, 32, 3 },
                        { 6, 0, height });
                    break;
            }
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 7, height, session.TrackColours[SCHEME_SUPPORTS]);
            switch (direction)
            {
                case 0:
                    paint_util_push_tunnel_right(session, height + 8, TUNNEL_SQUARE_FLAT);
                    break;
                case 1:
                    paint_util_push_tunnel_left(session, height + 8, TUNNEL_SQUARE_FLAT);
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17413, { 0, 0, height }, { 20, 32, 3 },
                        { 6, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17418, { 0, 0, height }, { 1, 32, 26 },
                        { 27, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17423, { 0, 0, height }, { 20, 32, 3 },
                        { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17429, { 0, 0, height }, { 1, 32, 26 },
                        { 27, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17408, { 0, 0, height }, { 20, 32, 3 },
                        { 6, 0, height });
                    break;
            }
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 1, height, session.TrackColours[SCHEME_SUPPORTS]);
            switch (direction)
            {
                case 2:
                    paint_util_push_tunnel_right(session, height, TUNNEL_SQUARE_FLAT);
                    break;
                case 3:
                    paint_util_push_tunnel_left(session, height, TUNNEL_SQUARE_FLAT);
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17414, { 0, 0, height }, { 16, 32, 3 },
                        { 16, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17419, { 0, 0, height }, { 16, 32, 1 },
                        { 0, 0, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17424, { 0, 0, height }, { 16, 32, 3 },
                        { 0, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17409, { 0, 0, height }, { 16, 32, 3 },
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17415, { 0, 0, height }, { 16, 16, 3 },
                        { 0, 16, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17420, { 0, 0, height }, { 16, 16, 1 },
                        { 16, 16, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17425, { 0, 0, height }, { 16, 16, 3 },
                        { 16, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17410, { 0, 0, height }, { 16, 16, 3 },
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17416, { 0, 0, height }, { 32, 16, 3 },
                        { 0, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17421, { 0, 0, height }, { 32, 16, 1 },
                        { 0, 0, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17426, { 0, 0, height }, { 32, 16, 3 },
                        { 0, 16, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17411, { 0, 0, height }, { 32, 16, 3 },
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17417, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17428, { 0, 0, height }, { 32, 1, 26 },
                        { 0, 27, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17422, { 0, 0, height }, { 32, 1, 26 },
                        { 0, 27, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17427, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17412, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    break;
            }
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 7, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height + 8, TUNNEL_SQUARE_FLAT);
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x007C7374 */
static void flying_rc_track_left_half_banked_helix_down_large(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackSequence >= 7)
    {
        trackSequence -= 7;
        direction = (direction - 1) & 3;
    }
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    flying_rc_track_right_half_banked_helix_up_large(session, ride, trackSequence, (direction + 1) & 3, height, trackElement);
}

/** rct2: 0x007C7384 */
static void flying_rc_track_right_half_banked_helix_down_large(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackSequence >= 7)
    {
        trackSequence -= 7;
        direction = (direction + 1) & 3;
    }
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    flying_rc_track_left_half_banked_helix_up_large(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x007C73B4 */
static void flying_rc_track_left_quarter_turn_1_60_deg_up(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17301, { 0, 0, height }, { 28, 28, 3 },
                    { 2, 2, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17305, { 0, 0, height }, { 28, 28, 1 },
                    { 2, 2, height + 99 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17302, { 0, 0, height }, { 28, 28, 3 },
                    { 2, 2, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17306, { 0, 0, height }, { 28, 28, 1 },
                    { 2, 2, height + 99 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17303, { 0, 0, height }, { 28, 28, 3 },
                    { 2, 2, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17307, { 0, 0, height }, { 28, 28, 1 },
                    { 2, 2, height + 99 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17300, { 0, 0, height }, { 28, 28, 3 },
                    { 2, 2, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17304, { 0, 0, height }, { 28, 28, 1 },
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
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27352, { 0, 0, height - 5 }, { 28, 28, 3 },
                    { 2, 2, height - 5 });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27348, { 0, 0, height - 5 }, { 28, 28, 1 },
                    { 2, 2, height + 94 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27353, { 0, 0, height - 5 }, { 28, 28, 3 },
                    { 2, 2, height - 5 });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27349, { 0, 0, height - 5 }, { 28, 28, 1 },
                    { 2, 2, height + 94 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27354, { 0, 0, height - 5 }, { 28, 28, 3 },
                    { 2, 2, height - 5 });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27350, { 0, 0, height - 5 }, { 28, 28, 1 },
                    { 2, 2, height + 94 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27351, { 0, 0, height - 5 }, { 28, 28, 3 },
                    { 2, 2, height - 5 });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27347, { 0, 0, height - 5 }, { 28, 28, 1 },
                    { 2, 2, height + 94 });
                break;
        }
    }

    track_paint_util_left_quarter_turn_1_tile_tunnel(session, direction, height, -8, TUNNEL_SQUARE_7, +56, TUNNEL_SQUARE_8);
    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 104, 0x20);
}

/** rct2: 0x007C7394 */
static void flying_rc_track_right_quarter_turn_1_60_deg_up(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17292, { 0, 0, height }, { 28, 28, 3 },
                    { 2, 2, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17296, { 0, 0, height }, { 28, 28, 1 },
                    { 2, 2, height + 99 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17293, { 0, 0, height }, { 28, 28, 3 },
                    { 2, 2, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17297, { 0, 0, height }, { 28, 28, 1 },
                    { 2, 2, height + 99 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17294, { 0, 0, height }, { 28, 28, 3 },
                    { 2, 2, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17298, { 0, 0, height }, { 28, 28, 1 },
                    { 2, 2, height + 99 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17295, { 0, 0, height }, { 28, 28, 3 },
                    { 2, 2, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17299, { 0, 0, height }, { 28, 28, 1 },
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
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27343, { 0, 0, height - 5 }, { 28, 28, 3 },
                    { 2, 2, height - 5 });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27339, { 0, 0, height - 5 }, { 28, 28, 1 },
                    { 2, 2, height + 94 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27344, { 0, 0, height - 5 }, { 28, 28, 3 },
                    { 2, 2, height - 5 });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27340, { 0, 0, height - 5 }, { 28, 28, 1 },
                    { 2, 2, height + 94 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27345, { 0, 0, height - 5 }, { 28, 28, 3 },
                    { 2, 2, height - 5 });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27341, { 0, 0, height - 5 }, { 28, 28, 1 },
                    { 2, 2, height + 94 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27346, { 0, 0, height - 5 }, { 28, 28, 3 },
                    { 2, 2, height - 5 });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27342, { 0, 0, height - 5 }, { 28, 28, 1 },
                    { 2, 2, height + 94 });
                break;
        }
    }
    track_paint_util_right_quarter_turn_1_tile_tunnel(session, direction, height, -8, TUNNEL_SQUARE_7, +56, TUNNEL_SQUARE_8);
    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 104, 0x20);
}

/** rct2: 0x007C73A4 */
static void flying_rc_track_left_quarter_turn_1_60_deg_down(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    flying_rc_track_right_quarter_turn_1_60_deg_up(session, ride, trackSequence, (direction + 1) & 3, height, trackElement);
}

/** rct2: 0x007C73C4 */
static void flying_rc_track_right_quarter_turn_1_60_deg_down(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    flying_rc_track_left_quarter_turn_1_60_deg_up(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x007C73D4 */
static void flying_rc_track_brakes(
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
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17148, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 1:
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17149, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
        }
        if (track_paint_util_should_paint_supports(session.MapPosition))
        {
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
        }
        paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
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
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27337, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 24 });
                break;
            case 1:
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27338, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 24 });
                break;
        }

        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        if (track_paint_util_should_paint_supports(session.MapPosition))
        {
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
        }

        paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
        paint_util_set_general_support_height(session, height + 32, 0x20);
    }
}

static void flying_rc_track_booster(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        bolliger_mabillard_track_booster<METAL_SUPPORTS_TUBES_INVERTED>(
            session, ride, trackSequence, direction, height, trackElement);
    }
    else
    {
        // Should not occur, except when converting from other coaster types.
        flying_rc_track_brakes(session, ride, trackSequence, direction, height, trackElement);
    }
}

/** rct2: 0x007C7674 */
static void flying_rc_track_left_quarter_banked_helix_large_up(
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27202, { 0, 0, height + 24 }, { 32, 20, 3 },
                        { 0, 6, height + 30 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27207, { 0, 0, height + 24 }, { 32, 20, 3 },
                        { 0, 6, height + 30 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27212, { 0, 0, height + 24 }, { 32, 20, 3 },
                        { 0, 6, height + 30 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27197, { 0, 0, height + 24 }, { 32, 20, 3 },
                        { 0, 6, height + 30 });
                    break;
            }

            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 41, session.TrackColours[SCHEME_SUPPORTS]);

            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_INVERTED_9);
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27201, { 0, 0, height + 24 }, { 32, 16, 3 },
                        { 0, 0, height + 38 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27206, { 0, 0, height + 24 }, { 32, 16, 3 },
                        { 0, 0, height + 38 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27211, { 0, 0, height + 24 }, { 32, 16, 3 },
                        { 0, 16, height + 38 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27196, { 0, 0, height + 24 }, { 32, 16, 3 },
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27200, { 0, 0, height + 24 }, { 16, 16, 3 },
                        { 0, 16, height + 38 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27205, { 0, 0, height + 24 }, { 16, 16, 3 },
                        { 16, 16, height + 38 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27210, { 0, 0, height + 24 }, { 16, 16, 3 },
                        { 16, 0, height + 38 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27195, { 0, 0, height + 24 }, { 16, 16, 3 },
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27199, { 0, 0, height + 24 }, { 16, 32, 3 },
                        { 16, 0, height + 38 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27204, { 0, 0, height + 24 }, { 16, 32, 3 },
                        { 0, 0, height + 38 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27209, { 0, 0, height + 24 }, { 16, 32, 3 },
                        { 0, 0, height + 38 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27194, { 0, 0, height + 24 }, { 16, 32, 3 },
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27198, { 0, 0, height + 24 }, { 20, 32, 3 },
                        { 6, 0, height + 38 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27203, { 0, 0, height + 24 }, { 20, 32, 3 },
                        { 6, 0, height + 38 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27208, { 0, 0, height + 24 }, { 20, 32, 3 },
                        { 6, 0, height + 38 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27193, { 0, 0, height + 24 }, { 20, 32, 3 },
                        { 6, 0, height + 38 });
                    break;
            }

            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D4, direction),
                0xFFFF, 0);
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 53, session.TrackColours[SCHEME_SUPPORTS]);

            switch (direction)
            {
                case 2:
                    paint_util_push_tunnel_right(session, height + 16, TUNNEL_SQUARE_INVERTED_9);
                    break;
                case 3:
                    paint_util_push_tunnel_left(session, height + 16, TUNNEL_SQUARE_INVERTED_9);
                    break;
            }
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x007C7684 */
static void flying_rc_track_right_quarter_banked_helix_large_up(
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27173, { 0, 0, height + 24 }, { 32, 20, 3 },
                        { 0, 6, height + 30 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27178, { 0, 0, height + 24 }, { 32, 20, 3 },
                        { 0, 6, height + 30 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27183, { 0, 0, height + 24 }, { 32, 20, 3 },
                        { 0, 6, height + 30 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27188, { 0, 0, height + 24 }, { 32, 20, 3 },
                        { 0, 6, height + 30 });
                    break;
            }

            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 41, session.TrackColours[SCHEME_SUPPORTS]);

            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_INVERTED_9);
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27174, { 0, 0, height + 24 }, { 32, 16, 3 },
                        { 0, 16, height + 38 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27179, { 0, 0, height + 24 }, { 32, 16, 3 },
                        { 0, 16, height + 38 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27184, { 0, 0, height + 24 }, { 32, 16, 3 },
                        { 0, 0, height + 38 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27189, { 0, 0, height + 24 }, { 32, 16, 3 },
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27175, { 0, 0, height + 24 }, { 16, 16, 3 },
                        { 0, 0, height + 38 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27180, { 0, 0, height + 24 }, { 16, 16, 3 },
                        { 16, 0, height + 38 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27185, { 0, 0, height + 24 }, { 16, 16, 3 },
                        { 16, 16, height + 38 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27190, { 0, 0, height + 24 }, { 16, 16, 3 },
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27176, { 0, 0, height + 24 }, { 16, 32, 3 },
                        { 16, 0, height + 38 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27181, { 0, 0, height + 24 }, { 16, 32, 3 },
                        { 0, 0, height + 38 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27186, { 0, 0, height + 24 }, { 16, 32, 3 },
                        { 0, 0, height + 38 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27191, { 0, 0, height + 24 }, { 16, 32, 3 },
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27177, { 0, 0, height + 24 }, { 20, 32, 3 },
                        { 6, 0, height + 38 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27182, { 0, 0, height + 24 }, { 20, 32, 3 },
                        { 6, 0, height + 38 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27187, { 0, 0, height + 24 }, { 20, 32, 3 },
                        { 6, 0, height + 38 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27192, { 0, 0, height + 24 }, { 20, 32, 3 },
                        { 6, 0, height + 38 });
                    break;
            }

            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D4, direction),
                0xFFFF, 0);
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 53, session.TrackColours[SCHEME_SUPPORTS]);

            switch (direction)
            {
                case 0:
                    paint_util_push_tunnel_right(session, height + 16, TUNNEL_SQUARE_INVERTED_9);
                    break;
                case 1:
                    paint_util_push_tunnel_left(session, height + 16, TUNNEL_SQUARE_INVERTED_9);
                    break;
            }
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x007C7694 */
static void flying_rc_track_left_quarter_banked_helix_large_down(
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27182, { 0, 0, height + 24 }, { 32, 20, 3 },
                        { 0, 6, height + 38 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27187, { 0, 0, height + 24 }, { 32, 20, 3 },
                        { 0, 6, height + 38 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27192, { 0, 0, height + 24 }, { 32, 20, 3 },
                        { 0, 6, height + 38 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27177, { 0, 0, height + 24 }, { 32, 20, 3 },
                        { 0, 6, height + 38 });
                    break;
            }

            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 53, session.TrackColours[SCHEME_SUPPORTS]);

            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height + 16, TUNNEL_SQUARE_INVERTED_9);
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27181, { 0, 0, height + 24 }, { 32, 16, 3 },
                        { 0, 0, height + 38 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27186, { 0, 0, height + 24 }, { 32, 16, 3 },
                        { 0, 0, height + 38 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27191, { 0, 0, height + 24 }, { 32, 16, 3 },
                        { 0, 16, height + 38 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27176, { 0, 0, height + 24 }, { 32, 16, 3 },
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27180, { 0, 0, height + 24 }, { 16, 16, 3 },
                        { 0, 16, height + 38 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27185, { 0, 0, height + 24 }, { 16, 16, 3 },
                        { 16, 16, height + 38 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27190, { 0, 0, height + 24 }, { 16, 16, 3 },
                        { 16, 0, height + 38 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27175, { 0, 0, height + 24 }, { 16, 16, 3 },
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27179, { 0, 0, height + 24 }, { 16, 32, 3 },
                        { 16, 0, height + 38 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27184, { 0, 0, height + 24 }, { 16, 32, 3 },
                        { 0, 0, height + 38 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27189, { 0, 0, height + 24 }, { 16, 32, 3 },
                        { 0, 0, height + 38 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27174, { 0, 0, height + 24 }, { 16, 32, 3 },
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27178, { 0, 0, height + 24 }, { 20, 32, 3 },
                        { 6, 0, height + 30 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27183, { 0, 0, height + 24 }, { 20, 32, 3 },
                        { 6, 0, height + 30 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27188, { 0, 0, height + 24 }, { 20, 32, 3 },
                        { 6, 0, height + 30 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27173, { 0, 0, height + 24 }, { 20, 32, 3 },
                        { 6, 0, height + 30 });
                    break;
            }

            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D4, direction),
                0xFFFF, 0);
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 41, session.TrackColours[SCHEME_SUPPORTS]);

            switch (direction)
            {
                case 2:
                    paint_util_push_tunnel_right(session, height, TUNNEL_SQUARE_INVERTED_9);
                    break;
                case 3:
                    paint_util_push_tunnel_left(session, height, TUNNEL_SQUARE_INVERTED_9);
                    break;
            }
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x007C76A4 */
static void flying_rc_track_right_quarter_banked_helix_large_down(
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27193, { 0, 0, height + 24 }, { 32, 20, 3 },
                        { 0, 6, height + 38 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27198, { 0, 0, height + 24 }, { 32, 20, 3 },
                        { 0, 6, height + 38 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27203, { 0, 0, height + 24 }, { 32, 20, 3 },
                        { 0, 6, height + 38 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27208, { 0, 0, height + 24 }, { 32, 20, 3 },
                        { 0, 6, height + 38 });
                    break;
            }

            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 53, session.TrackColours[SCHEME_SUPPORTS]);

            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height + 16, TUNNEL_SQUARE_INVERTED_9);
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27194, { 0, 0, height + 24 }, { 32, 16, 3 },
                        { 0, 16, height + 38 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27199, { 0, 0, height + 24 }, { 32, 16, 3 },
                        { 0, 16, height + 38 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27204, { 0, 0, height + 24 }, { 32, 16, 3 },
                        { 0, 0, height + 38 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27209, { 0, 0, height + 24 }, { 32, 16, 3 },
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27195, { 0, 0, height + 24 }, { 16, 16, 3 },
                        { 0, 0, height + 38 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27200, { 0, 0, height + 24 }, { 16, 16, 3 },
                        { 16, 0, height + 38 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27205, { 0, 0, height + 24 }, { 16, 16, 3 },
                        { 16, 16, height + 38 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27210, { 0, 0, height + 24 }, { 16, 16, 3 },
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27196, { 0, 0, height + 24 }, { 16, 32, 3 },
                        { 16, 0, height + 38 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27201, { 0, 0, height + 24 }, { 16, 32, 3 },
                        { 0, 0, height + 38 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27206, { 0, 0, height + 24 }, { 16, 32, 3 },
                        { 0, 0, height + 38 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27211, { 0, 0, height + 24 }, { 16, 32, 3 },
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27197, { 0, 0, height + 24 }, { 20, 32, 3 },
                        { 6, 0, height + 30 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27202, { 0, 0, height + 24 }, { 20, 32, 3 },
                        { 6, 0, height + 30 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27207, { 0, 0, height + 24 }, { 20, 32, 3 },
                        { 6, 0, height + 30 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27212, { 0, 0, height + 24 }, { 20, 32, 3 },
                        { 6, 0, height + 30 });
                    break;
            }

            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D4, direction),
                0xFFFF, 0);
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 41, session.TrackColours[SCHEME_SUPPORTS]);

            switch (direction)
            {
                case 0:
                    paint_util_push_tunnel_right(session, height, TUNNEL_SQUARE_INVERTED_9);
                    break;
                case 1:
                    paint_util_push_tunnel_left(session, height, TUNNEL_SQUARE_INVERTED_9);
                    break;
            }
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x007C78B4 */
static void flying_rc_track_25_deg_up_left_banked(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17914, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17915, { 0, 0, height }, { 32, 1, 34 },
                    { 0, 27, height });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17916, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17917, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
        }
        if (track_paint_util_should_paint_supports(session.MapPosition))
        {
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
        }
        if (direction == 0 || direction == 3)
        {
            paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_SQUARE_7);
        }
        else
        {
            paint_util_push_tunnel_rotated(session, direction, height + 8, TUNNEL_SQUARE_8);
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 56, 0x20);
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27711, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 40 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27712, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 40 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27713, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 40 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27714, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 40 });
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
                        session, METAL_SUPPORTS_TUBES_INVERTED, 6, 0, height + 57, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height + 57, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 7, 0, height + 57, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height + 57, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
        }

        if (direction == 0 || direction == 3)
        {
            paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_SQUARE_7);
        }
        else
        {
            paint_util_push_tunnel_rotated(session, direction, height + 8, TUNNEL_SQUARE_8);
        }
        paint_util_set_general_support_height(session, height + 72, 0x20);
    }
}

/** rct2: 0x007C78C4 */
static void flying_rc_track_25_deg_up_right_banked(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17918, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17919, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17920, { 0, 0, height }, { 32, 1, 34 },
                    { 0, 27, height });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17921, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
        }
        if (track_paint_util_should_paint_supports(session.MapPosition))
        {
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
        }
        if (direction == 0 || direction == 3)
        {
            paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_SQUARE_7);
        }
        else
        {
            paint_util_push_tunnel_rotated(session, direction, height + 8, TUNNEL_SQUARE_8);
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 56, 0x20);
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27715, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 40 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27716, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 40 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27717, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 40 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27718, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 40 });
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
                        session, METAL_SUPPORTS_TUBES_INVERTED, 6, 0, height + 57, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height + 57, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 7, 0, height + 57, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height + 57, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
        }

        if (direction == 0 || direction == 3)
        {
            paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_SQUARE_7);
        }
        else
        {
            paint_util_push_tunnel_rotated(session, direction, height + 8, TUNNEL_SQUARE_8);
        }
        paint_util_set_general_support_height(session, height + 72, 0x20);
    }
}

/** rct2: 0x007C73E4 */
static void flying_rc_track_on_ride_photo(
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
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17146, { 0, 0, height }, { 32, 20, 0 },
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
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17147, { 0, 0, height }, { 32, 20, 0 },
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
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17146, { 0, 0, height }, { 32, 20, 0 },
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
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17147, { 0, 0, height }, { 32, 20, 0 },
                    { 0, 6, height + 3 });
                break;
        }
        track_paint_util_onride_photo_paint(session, direction, height + 3, trackElement);
        paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
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
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27129, { 0, 0, height + 24 }, { 32, 20, 3 },
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
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27130, { 0, 0, height + 24 }, { 32, 20, 3 },
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
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27129, { 0, 0, height + 24 }, { 32, 20, 3 },
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
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27130, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 24 });
                break;
        }
        track_paint_util_onride_photo_paint(session, direction, height + 3, trackElement);
        paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 64, 0x20);
    }
}

/** rct2: 0x007C78D4 */
static void flying_rc_track_25_deg_down_left_banked(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    flying_rc_track_25_deg_up_right_banked(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x007C78E4 */
static void flying_rc_track_25_deg_down_right_banked(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    flying_rc_track_25_deg_up_left_banked(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x007C7404 */
static void flying_rc_track_left_eighth_to_diag(
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17546, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17550, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17554, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17558, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                }
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                if (direction == 0 || direction == 3)
                {
                    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17547, { 0, 0, height }, { 32, 16, 3 },
                            { 0, 0, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17551, { 0, 0, height }, { 34, 16, 3 },
                            { 0, 0, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17555, { 0, 0, height }, { 32, 16, 3 },
                            { 0, 16, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17559, { 0, 0, height }, { 32, 16, 3 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17548, { 0, 0, height }, { 16, 16, 3 },
                            { 0, 16, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17552, { 0, 0, height }, { 16, 16, 3 },
                            { 16, 16, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17556, { 0, 0, height }, { 16, 16, 3 },
                            { 16, 0, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17560, { 0, 0, height }, { 16, 16, 3 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17549, { 0, 0, height }, { 16, 16, 3 },
                            { 16, 16, height });
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17553, { 0, 0, height }, { 16, 18, 3 },
                            { 0, 16, height });
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17557, { 0, 0, height }, { 16, 16, 3 },
                            { 0, 0, height });
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17561, { 0, 0, height }, { 16, 16, 3 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27507, { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27511, { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27515, { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27519, { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                }

                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);

                if (direction == 0 || direction == 3)
                {
                    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
                }
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27508, { 0, 0, height + 24 },
                            { 32, 16, 3 }, { 0, 0, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27512, { 0, 0, height + 24 },
                            { 34, 16, 3 }, { 0, 0, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27516, { 0, 0, height + 24 },
                            { 32, 16, 3 }, { 0, 16, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27520, { 0, 0, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27509, { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 0, 16, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27513, { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 16, 16, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27517, { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 16, 0, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27521, { 0, 0, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27510, { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 16, 16, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27514, { 0, 0, height + 24 },
                            { 16, 18, 3 }, { 0, 16, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27518, { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 0, 0, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27522, { 0, 0, height + 24 },
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
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
        }
    }
}

/** rct2: 0x007C7414 */
static void flying_rc_track_right_eighth_to_diag(
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17530, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17534, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17538, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17542, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                }
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                if (direction == 0 || direction == 3)
                {
                    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17531, { 0, 0, height }, { 32, 16, 3 },
                            { 0, 16, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17535, { 0, 0, height }, { 32, 16, 3 },
                            { 0, 16, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17539, { 0, 0, height }, { 34, 16, 3 },
                            { 0, 0, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17543, { 0, 0, height }, { 32, 16, 3 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17532, { 0, 0, height }, { 16, 16, 3 },
                            { 0, 0, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17536, { 0, 0, height }, { 16, 16, 3 },
                            { 16, 0, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17540, { 0, 0, height }, { 28, 28, 3 },
                            { 4, 4, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17544, { 0, 0, height }, { 16, 16, 3 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17533, { 0, 0, height }, { 16, 16, 3 },
                            { 16, 0, height });
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17537, { 0, 0, height }, { 16, 16, 3 },
                            { 0, 0, height });
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17541, { 0, 0, height }, { 16, 18, 3 },
                            { 0, 16, height });
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17545, { 0, 0, height }, { 16, 16, 3 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27491, { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27495, { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27499, { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27503, { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                }

                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);

                if (direction == 0 || direction == 3)
                {
                    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
                }
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27492, { 0, 0, height + 24 },
                            { 32, 16, 3 }, { 0, 16, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27496, { 0, 0, height + 24 },
                            { 32, 16, 3 }, { 0, 16, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27500, { 0, 0, height + 24 },
                            { 34, 16, 3 }, { 0, 0, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27504, { 0, 0, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27493, { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 0, 0, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27497, { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 16, 0, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27501, { 0, 0, height + 24 },
                            { 28, 28, 3 }, { 4, 4, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27505, { 0, 0, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27494, { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 16, 0, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27498, { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 0, 0, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27502, { 0, 0, height + 24 },
                            { 16, 18, 3 }, { 0, 16, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27506, { 0, 0, height + 24 },
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

                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
        }
    }
}

/** rct2: 0x007C7424 */
static void flying_rc_track_left_eighth_to_orthogonal(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    flying_rc_track_right_eighth_to_diag(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x007C7434 */
static void flying_rc_track_right_eighth_to_orthogonal(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    flying_rc_track_left_eighth_to_diag(session, ride, trackSequence, (direction + 3) & 3, height, trackElement);
}

/** rct2: 0x007C7444 */
static void flying_rc_track_left_eighth_bank_to_diag(
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17578, { 0, 0, height }, { 32, 1, 26 },
                            { 0, 27, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17582, { 0, 0, height }, { 32, 1, 26 },
                            { 0, 27, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17586, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17590, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                }
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                if (direction == 0 || direction == 3)
                {
                    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17579, { 0, 0, height }, { 32, 16, 3 },
                            { 0, 0, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17583, { 0, 0, height }, { 34, 16, 0 },
                            { 0, 0, height + 27 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17587, { 0, 0, height }, { 32, 16, 3 },
                            { 0, 16, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17591, { 0, 0, height }, { 32, 16, 3 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17580, { 0, 0, height }, { 16, 16, 3 },
                            { 0, 16, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17584, { 0, 0, height }, { 16, 16, 0 },
                            { 16, 16, height + 27 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17588, { 0, 0, height }, { 16, 16, 3 },
                            { 16, 0, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17592, { 0, 0, height }, { 16, 16, 3 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17581, { 0, 0, height }, { 16, 16, 3 },
                            { 16, 16, height });
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17585, { 0, 0, height }, { 16, 18, 0 },
                            { 0, 16, height + 27 });
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17589, { 0, 0, height }, { 16, 16, 3 },
                            { 0, 0, height });
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17593, { 0, 0, height }, { 16, 16, 3 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27539, { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27543, { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27547, { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27551, { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                }

                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF,
                    0);
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);

                if (direction == 0 || direction == 3)
                {
                    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
                }
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27540, { 0, 0, height + 24 },
                            { 32, 16, 3 }, { 0, 0, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27544, { 0, 0, height + 24 },
                            { 34, 16, 3 }, { 0, 0, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27548, { 0, 0, height + 24 },
                            { 32, 16, 3 }, { 0, 16, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27552, { 0, 0, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27541, { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 0, 16, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27545, { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 16, 16, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27549, { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 16, 0, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27553, { 0, 0, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27542, { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 16, 16, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27546, { 0, 0, height + 24 },
                            { 16, 18, 3 }, { 0, 16, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27550, { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 0, 0, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27554, { 0, 0, height + 24 },
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
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
        }
    }
}

/** rct2: 0x007C7454 */
static void flying_rc_track_right_eighth_bank_to_diag(
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17562, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17566, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17570, { 0, 0, height }, { 32, 1, 26 },
                            { 0, 27, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17574, { 0, 0, height }, { 32, 1, 26 },
                            { 0, 27, height });
                        break;
                }
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                if (direction == 0 || direction == 3)
                {
                    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17563, { 0, 0, height }, { 16, 16, 3 },
                            { 0, 0, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17567, { 0, 0, height }, { 32, 16, 3 },
                            { 0, 16, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17571, { 0, 0, height }, { 34, 16, 0 },
                            { 0, 0, height + 27 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17575, { 0, 0, height }, { 32, 16, 3 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17564, { 0, 0, height }, { 16, 16, 3 },
                            { 0, 0, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17568, { 0, 0, height }, { 16, 16, 3 },
                            { 16, 0, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17572, { 0, 0, height }, { 28, 28, 0 },
                            { 4, 4, height + 27 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17576, { 0, 0, height }, { 16, 16, 3 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17565, { 0, 0, height }, { 16, 16, 3 },
                            { 16, 0, height });
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17569, { 0, 0, height }, { 16, 16, 3 },
                            { 0, 0, height });
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17573, { 0, 0, height }, { 16, 18, 0 },
                            { 0, 16, height + 27 });
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17577, { 0, 0, height }, { 16, 16, 3 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27523, { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27527, { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27531, { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27535, { 0, 0, height + 24 },
                            { 32, 20, 3 }, { 0, 6, height + 22 });
                        break;
                }

                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF,
                    0);
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);

                if (direction == 0 || direction == 3)
                {
                    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
                }
                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27524, { 0, 0, height + 24 },
                            { 32, 16, 3 }, { 0, 16, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27528, { 0, 0, height + 24 },
                            { 32, 16, 3 }, { 0, 16, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27532, { 0, 0, height + 24 },
                            { 34, 16, 3 }, { 0, 0, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27536, { 0, 0, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27525, { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 0, 0, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27529, { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 16, 0, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27533, { 0, 0, height + 24 },
                            { 28, 28, 3 }, { 4, 4, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27537, { 0, 0, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27526, { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 16, 0, height + 22 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27530, { 0, 0, height + 24 },
                            { 16, 16, 3 }, { 0, 0, height + 22 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27534, { 0, 0, height + 24 },
                            { 16, 18, 3 }, { 0, 16, height + 22 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27538, { 0, 0, height + 24 },
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

                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
        }
    }
}

/** rct2: 0x007C7464 */
static void flying_rc_track_left_eighth_bank_to_orthogonal(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    flying_rc_track_right_eighth_bank_to_diag(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x007C7474 */
static void flying_rc_track_right_eighth_bank_to_orthogonal(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    flying_rc_track_left_eighth_bank_to_diag(session, ride, trackSequence, (direction + 3) & 3, height, trackElement);
}

/** rct2: 0x007C73F4 */
static void flying_rc_track_diag_flat(
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17861, { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17791, { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17858, { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17788, { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17860, { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17790, { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17859, { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17789, { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 27614, { -16, -16, height + 24 },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 27558, { -16, -16, height + 24 },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 27611, { -16, -16, height + 24 },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 27555, { -16, -16, height + 24 },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 27613, { -16, -16, height + 24 },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 27557, { -16, -16, height + 24 },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 27612, { -16, -16, height + 24 },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 27556, { -16, -16, height + 24 },
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

                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
        }
    }
}

/** rct2: 0x007C74A4 */
static void flying_rc_track_diag_25_deg_up(
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17873, { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17803, { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17870, { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17800, { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17872, { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17802, { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17871, { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17801, { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 27626, { -16, -16, height + 24 },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 27570, { -16, -16, height + 24 },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 27623, { -16, -16, height + 24 },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 27567, { -16, -16, height + 24 },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 27625, { -16, -16, height + 24 },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 27569, { -16, -16, height + 24 },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 27624, { -16, -16, height + 24 },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 27568, { -16, -16, height + 24 },
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
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height + 51, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height + 51, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height + 51, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height + 51, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                paint_util_set_general_support_height(session, height + 56, 0x20);
                break;
        }
    }
}

/** rct2: 0x007C7504 */
static void flying_rc_track_diag_60_deg_up(
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17885, { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17815, { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17882, { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17812, { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17884, { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17814, { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17883, { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17813, { -16, -16, height },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27582, { -16, -16, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27579, { -16, -16, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27581, { -16, -16, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27580, { -16, -16, height + 24 },
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
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 32, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 36, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 32, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 36, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                paint_util_set_general_support_height(session, height + 104, 0x20);
                break;
        }
    }
}

/** rct2: 0x007C7484 */
static void flying_rc_track_diag_flat_to_25_deg_up(
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17865, { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17795, { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17862, { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17792, { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17864, { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17794, { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17863, { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17793, { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 27618, { -16, -16, height + 24 },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 27562, { -16, -16, height + 24 },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 27615, { -16, -16, height + 24 },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 27559, { -16, -16, height + 24 },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 27617, { -16, -16, height + 24 },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 27561, { -16, -16, height + 24 },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 27616, { -16, -16, height + 24 },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 27560, { -16, -16, height + 24 },
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
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height + 43, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height + 43, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height + 43, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height + 43, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                paint_util_set_general_support_height(session, height + 48, 0x20);
                break;
        }
    }
}

/** rct2: 0x007C74E4 */
static void flying_rc_track_diag_25_deg_up_to_60_deg_up(
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17877, { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17807, { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17874, { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17804, { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17876, { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17806, { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17875, { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17805, { -16, -16, height },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27574, { -16, -16, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27571, { -16, -16, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27573, { -16, -16, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27572, { -16, -16, height + 24 },
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
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 16, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 16, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 16, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 16, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                paint_util_set_general_support_height(session, height + 72, 0x20);
                break;
        }
    }
}

/** rct2: 0x007C74F4 */
static void flying_rc_track_diag_60_deg_up_to_25_deg_up(
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17881, { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17811, { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17878, { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17808, { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17880, { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17810, { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17879, { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17809, { -16, -16, height },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27578, { -16, -16, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27575, { -16, -16, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27577, { -16, -16, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27576, { -16, -16, height + 24 },
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
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 21, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 21, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 21, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 21, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                paint_util_set_general_support_height(session, height + 72, 0x20);
                break;
        }
    }
}

/** rct2: 0x007C7494 */
static void flying_rc_track_diag_25_deg_up_to_flat(
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17869, { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17799, { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17866, { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17796, { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17868, { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17798, { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17867, { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17797, { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 27622, { -16, -16, height + 24 },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 27566, { -16, -16, height + 24 },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 27619, { -16, -16, height + 24 },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 27563, { -16, -16, height + 24 },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 27621, { -16, -16, height + 24 },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 27565, { -16, -16, height + 24 },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 27620, { -16, -16, height + 24 },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 27564, { -16, -16, height + 24 },
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

/** rct2: 0x007C74D4 */
static void flying_rc_track_diag_25_deg_down(
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17871, { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17801, { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17872, { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17802, { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17870, { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17800, { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17873, { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17803, { -16, -16, height },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27568, { -16, -16, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27569, { -16, -16, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27567, { -16, -16, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27570, { -16, -16, height + 24 },
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
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height + 51, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height + 51, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height + 51, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height + 51, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                paint_util_set_general_support_height(session, height + 56, 0x20);
                break;
        }
    }
}

/** rct2: 0x007C7534 */
static void flying_rc_track_diag_60_deg_down(
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17883, { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17813, { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17884, { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17814, { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17882, { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17812, { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17885, { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17815, { -16, -16, height },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27580, { -16, -16, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27581, { -16, -16, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27579, { -16, -16, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27582, { -16, -16, height + 24 },
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
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 24, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 28, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 24, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 28, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                paint_util_set_general_support_height(session, height + 104, 0x20);
                break;
        }
    }
}

/** rct2: 0x007C74B4 */
static void flying_rc_track_diag_flat_to_25_deg_down(
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17867, { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17797, { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17868, { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17798, { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17866, { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17796, { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17869, { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17799, { -16, -16, height },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27564, { -16, -16, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27565, { -16, -16, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27563, { -16, -16, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27566, { -16, -16, height + 24 },
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
                break;
        }
    }

    paint_util_set_general_support_height(session, height + 56, 0x20);
}

/** rct2: 0x007C7514 */
static void flying_rc_track_diag_25_deg_down_to_60_deg_down(
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17879, { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17809, { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17880, { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17810, { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17878, { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17808, { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17881, { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17811, { -16, -16, height },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27576, { -16, -16, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27577, { -16, -16, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27575, { -16, -16, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27578, { -16, -16, height + 24 },
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
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 17, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 17, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 17, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 17, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                paint_util_set_general_support_height(session, height + 72, 0x20);
                break;
        }
    }
}

/** rct2: 0x007C7524 */
static void flying_rc_track_diag_60_deg_down_to_25_deg_down(
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17875, { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17805, { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17876, { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17806, { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17874, { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17804, { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17877, { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17807, { -16, -16, height },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27572, { -16, -16, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27573, { -16, -16, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27571, { -16, -16, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27574, { -16, -16, height + 24 },
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
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 8, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 8, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 8, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 8, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                paint_util_set_general_support_height(session, height + 72, 0x20);
                break;
        }
    }
}

/** rct2: 0x007C74C4 */
static void flying_rc_track_diag_25_deg_down_to_flat(
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17863, { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17793, { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17864, { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17794, { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17862, { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17792, { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17865, { -16, -16, height },
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
                                session, direction, session.TrackColours[SCHEME_TRACK] | 17795, { -16, -16, height },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27560, { -16, -16, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27561, { -16, -16, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27559, { -16, -16, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27562, { -16, -16, height + 24 },
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
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height + 43, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height + 43, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height + 43, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height + 43, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                paint_util_set_general_support_height(session, height + 48, 0x20);
                break;
        }
    }
}

/** rct2: 0x007C7564 */
static void flying_rc_track_diag_flat_to_left_bank(
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17831, { -16, -16, height }, { 32, 32, 3 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17828, { -16, -16, height }, { 32, 32, 3 },
                            { -16, -16, height });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17832, { -16, -16, height }, { 32, 32, 0 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17830, { -16, -16, height }, { 32, 32, 3 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17829, { -16, -16, height }, { 32, 32, 3 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27590, { -16, -16, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27587, { -16, -16, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27589, { -16, -16, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27588, { -16, -16, height + 24 },
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

                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
        }
    }
}

/** rct2: 0x007C7574 */
static void flying_rc_track_diag_flat_to_right_bank(
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17836, { -16, -16, height }, { 32, 32, 3 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17833, { -16, -16, height }, { 32, 32, 3 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17835, { -16, -16, height }, { 32, 32, 3 },
                            { -16, -16, height });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17837, { -16, -16, height }, { 32, 32, 0 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17834, { -16, -16, height }, { 32, 32, 3 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27594, { -16, -16, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27591, { -16, -16, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27593, { -16, -16, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27592, { -16, -16, height + 24 },
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

                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
        }
    }
}

/** rct2: 0x007C7584 */
static void flying_rc_track_diag_left_bank_to_flat(
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17834, { -16, -16, height }, { 32, 32, 3 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17835, { -16, -16, height }, { 32, 32, 3 },
                            { -16, -16, height });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17837, { -16, -16, height }, { 32, 32, 0 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17833, { -16, -16, height }, { 32, 32, 3 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17836, { -16, -16, height }, { 32, 32, 3 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27592, { -16, -16, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27593, { -16, -16, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27591, { -16, -16, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27594, { -16, -16, height + 24 },
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

                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
        }
    }
}

/** rct2: 0x007C7594 */
static void flying_rc_track_diag_right_bank_to_flat(
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17829, { -16, -16, height }, { 32, 32, 3 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17830, { -16, -16, height }, { 32, 32, 3 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17828, { -16, -16, height }, { 32, 32, 3 },
                            { -16, -16, height });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17832, { -16, -16, height }, { 32, 32, 0 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17831, { -16, -16, height }, { 32, 32, 3 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27588, { -16, -16, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27589, { -16, -16, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27587, { -16, -16, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27590, { -16, -16, height + 24 },
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

                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
        }
    }
}

/** rct2: 0x007C75C4 */
static void flying_rc_track_diag_left_bank_to_25_deg_up(
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17851, { -16, -16, height }, { 32, 32, 3 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17848, { -16, -16, height }, { 32, 32, 3 },
                            { -16, -16, height });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17852, { -16, -16, height }, { 32, 32, 0 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17850, { -16, -16, height }, { 32, 32, 3 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17849, { -16, -16, height }, { 32, 32, 3 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27606, { -16, -16, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27603, { -16, -16, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27605, { -16, -16, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27604, { -16, -16, height + 24 },
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
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height + 43, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height + 43, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height + 43, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height + 43, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                paint_util_set_general_support_height(session, height + 48, 0x20);
                break;
        }
    }
}

/** rct2: 0x007C75D4 */
static void flying_rc_track_diag_right_bank_to_25_deg_up(
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17856, { -16, -16, height }, { 32, 32, 3 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17853, { -16, -16, height }, { 32, 32, 3 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17855, { -16, -16, height }, { 32, 32, 3 },
                            { -16, -16, height });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17857, { -16, -16, height }, { 32, 32, 0 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17854, { -16, -16, height }, { 32, 32, 3 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27610, { -16, -16, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27607, { -16, -16, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27609, { -16, -16, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27608, { -16, -16, height + 24 },
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
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height + 43, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height + 43, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height + 43, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height + 43, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                paint_util_set_general_support_height(session, height + 48, 0x20);
                break;
        }
    }
}

/** rct2: 0x007C75A4 */
static void flying_rc_track_diag_25_deg_up_to_left_bank(
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17841, { -16, -16, height }, { 32, 32, 3 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17838, { -16, -16, height }, { 32, 32, 3 },
                            { -16, -16, height });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17842, { -16, -16, height }, { 32, 32, 0 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17840, { -16, -16, height }, { 32, 32, 3 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17839, { -16, -16, height }, { 32, 32, 3 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27598, { -16, -16, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27595, { -16, -16, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27597, { -16, -16, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27596, { -16, -16, height + 24 },
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

/** rct2: 0x007C75B4 */
static void flying_rc_track_diag_25_deg_up_to_right_bank(
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17846, { -16, -16, height }, { 32, 32, 3 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17843, { -16, -16, height }, { 32, 32, 3 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17845, { -16, -16, height }, { 32, 32, 3 },
                            { -16, -16, height });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17847, { -16, -16, height }, { 32, 32, 0 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17844, { -16, -16, height }, { 32, 32, 3 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27602, { -16, -16, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27599, { -16, -16, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27601, { -16, -16, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27600, { -16, -16, height + 24 },
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

/** rct2: 0x007C75E4 */
static void flying_rc_track_diag_left_bank_to_25_deg_down(
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17844, { -16, -16, height }, { 32, 32, 3 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17845, { -16, -16, height }, { 32, 32, 3 },
                            { -16, -16, height });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17847, { -16, -16, height }, { 32, 32, 0 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17843, { -16, -16, height }, { 32, 32, 3 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17846, { -16, -16, height }, { 32, 32, 3 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27600, { -16, -16, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27601, { -16, -16, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27599, { -16, -16, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27602, { -16, -16, height + 24 },
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
                break;
        }
    }

    paint_util_set_general_support_height(session, height + 56, 0x20);
}

/** rct2: 0x007C75F4 */
static void flying_rc_track_diag_right_bank_to_25_deg_down(
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17839, { -16, -16, height }, { 32, 32, 3 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17840, { -16, -16, height }, { 32, 32, 3 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17838, { -16, -16, height }, { 32, 32, 3 },
                            { -16, -16, height });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17842, { -16, -16, height }, { 32, 32, 0 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17841, { -16, -16, height }, { 32, 32, 3 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27596, { -16, -16, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27597, { -16, -16, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27595, { -16, -16, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27598, { -16, -16, height + 24 },
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
                break;
        }
    }

    paint_util_set_general_support_height(session, height + 56, 0x20);
}

/** rct2: 0x007C7604 */
static void flying_rc_track_diag_25_deg_down_to_left_bank(
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17854, { -16, -16, height }, { 32, 32, 3 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17855, { -16, -16, height }, { 32, 32, 3 },
                            { -16, -16, height });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17857, { -16, -16, height }, { 32, 32, 0 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17853, { -16, -16, height }, { 32, 32, 3 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17856, { -16, -16, height }, { 32, 32, 3 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27608, { -16, -16, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27609, { -16, -16, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27607, { -16, -16, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27610, { -16, -16, height + 24 },
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
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height + 43, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height + 43, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height + 43, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height + 43, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                paint_util_set_general_support_height(session, height + 48, 0x20);
                break;
        }
    }
}

/** rct2: 0x007C7614 */
static void flying_rc_track_diag_25_deg_down_to_right_bank(
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17849, { -16, -16, height }, { 32, 32, 3 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17850, { -16, -16, height }, { 32, 32, 3 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17848, { -16, -16, height }, { 32, 32, 3 },
                            { -16, -16, height });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17852, { -16, -16, height }, { 32, 32, 0 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17851, { -16, -16, height }, { 32, 32, 3 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27604, { -16, -16, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27605, { -16, -16, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27603, { -16, -16, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27606, { -16, -16, height + 24 },
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
                            session, METAL_SUPPORTS_TUBES_INVERTED, 1, 0, height + 43, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 0, 0, height + 43, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 2, 0, height + 43, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 3, 0, height + 43, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                paint_util_set_general_support_height(session, height + 48, 0x20);
                break;
        }
    }
}

/** rct2: 0x007C7544 */
static void flying_rc_track_diag_left_bank(
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17827, { -16, -16, height }, { 32, 32, 3 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17824, { -16, -16, height }, { 32, 32, 0 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17826, { -16, -16, height }, { 32, 32, 3 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17825, { -16, -16, height }, { 32, 32, 3 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27586, { -16, -16, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27583, { -16, -16, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27585, { -16, -16, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27584, { -16, -16, height + 24 },
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

                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
        }
    }
}

/** rct2: 0x007C7554 */
static void flying_rc_track_diag_right_bank(
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17825, { -16, -16, height }, { 32, 32, 3 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17826, { -16, -16, height }, { 32, 32, 3 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17824, { -16, -16, height }, { 32, 32, 0 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17827, { -16, -16, height }, { 32, 32, 3 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27584, { -16, -16, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27585, { -16, -16, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27583, { -16, -16, height + 24 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27586, { -16, -16, height + 24 },
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

                paint_util_set_general_support_height(session, height + 32, 0x20);
                break;
        }
    }
}

/** rct2: 0x007C7624 */
static void flying_rc_track_left_flyer_twist_up(
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27430, { 0, 6, height - 5 }, { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27436, { 0, 6, height - 5 }, { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27429, { 0, 6, height - 5 }, { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27435, { 0, 6, height - 5 }, { 32, 20, 3 });
                    break;
            }
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height - 5, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27431, { 0, 6, height + 24 }, { 32, 20, 3 },
                        { 0, 6, height + 24 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27437, { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27428, { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27434, { 0, 6, height + 24 }, { 32, 20, 3 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27432, { 0, 6, height + 24 }, { 32, 20, 3 },
                        { 0, 6, height + 24 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27438, { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27427, { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27433, { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
            }

            paint_util_set_segment_support_height(
                session,
                paint_util_rotate_segments(
                    SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);

            switch (direction)
            {
                case 1:
                    paint_util_push_tunnel_right(session, height, TUNNEL_SQUARE_FLAT);
                    break;
                case 2:
                    paint_util_push_tunnel_left(session, height, TUNNEL_SQUARE_FLAT);
                    break;
            }
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x007C7634 */
static void flying_rc_track_right_flyer_twist_up(
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27442, { 0, 6, height - 5 }, { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27448, { 0, 6, height - 5 }, { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27441, { 0, 6, height - 5 }, { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27447, { 0, 6, height - 5 }, { 32, 20, 3 });
                    break;
            }
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height - 5, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27443, { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27449, { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27440, { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27446, { 0, 6, height + 24 }, { 32, 20, 3 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27444, { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27450, { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27439, { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27445, { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
            }

            paint_util_set_segment_support_height(
                session,
                paint_util_rotate_segments(
                    SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);

            switch (direction)
            {
                case 1:
                    paint_util_push_tunnel_right(session, height, TUNNEL_SQUARE_FLAT);
                    break;
                case 2:
                    paint_util_push_tunnel_left(session, height, TUNNEL_SQUARE_FLAT);
                    break;
            }
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x007C7644 */
static void flying_rc_track_left_flyer_twist_down(
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27427, { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27433, { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27432, { 0, 6, height + 24 }, { 32, 20, 3 },
                        { 0, 6, height + 24 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27438, { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
            }

            paint_util_set_segment_support_height(
                session,
                paint_util_rotate_segments(
                    SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);

            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
            }
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27428, { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27434, { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27431, { 0, 6, height + 24 }, { 32, 20, 3 },
                        { 0, 6, height + 24 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27437, { 0, 6, height + 24 }, { 32, 20, 3 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27429, { 0, 6, height - 5 }, { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27435, { 0, 6, height - 5 }, { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27430, { 0, 6, height - 5 }, { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27436, { 0, 6, height - 5 }, { 32, 20, 3 });
                    break;
            }
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height - 5, session.TrackColours[SCHEME_SUPPORTS]);
            switch (direction)
            {
                case 1:
                    paint_util_push_tunnel_right(session, height, TUNNEL_SQUARE_FLAT);
                    break;
                case 2:
                    paint_util_push_tunnel_left(session, height, TUNNEL_SQUARE_FLAT);
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

/** rct2: 0x007C7654 */
static void flying_rc_track_right_flyer_twist_down(
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27439, { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27445, { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27444, { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27450, { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
            }

            paint_util_set_segment_support_height(
                session,
                paint_util_rotate_segments(
                    SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);

            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
            }
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27440, { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27446, { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27443, { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27449, { 0, 6, height + 24 }, { 32, 20, 3 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27441, { 0, 6, height - 5 }, { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27447, { 0, 6, height - 5 }, { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27442, { 0, 6, height - 5 }, { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 27448, { 0, 6, height - 5 }, { 32, 20, 3 });
                    break;
            }
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height - 5, session.TrackColours[SCHEME_SUPPORTS]);
            switch (direction)
            {
                case 1:
                    paint_util_push_tunnel_right(session, height, TUNNEL_SQUARE_FLAT);
                    break;
                case 2:
                    paint_util_push_tunnel_left(session, height, TUNNEL_SQUARE_FLAT);
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

/** rct2: 0x007C72F4 */
static void flying_rc_track_flyer_half_loop_up(
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17626, { 0, 6, height }, { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17634, { 0, 6, height }, { 32, 20, 11 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17633, { 0, 6, height }, { 32, 20, 9 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17641, { 0, 6, height }, { 32, 20, 7 });
                    break;
            }
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_SQUARE_7);
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17627, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 4, 20, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17635, { 0, 14, height }, { 3, 20, 63 },
                        { 28, 6, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 4, 15, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17632, { 0, 6, height }, { 3, 20, 63 },
                        { 28, 6, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 4, 16, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17640, { 0, 6, height }, { 32, 20, 3 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17628, { 16, 0, height }, { 2, 16, 119 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17636, { 12, 0, height }, { 3, 16, 119 },
                        { 12, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17631, { 10, 16, height }, { 4, 12, 119 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17639, { 16, 16, height }, { 2, 16, 119 },
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17629, { 0, 0, height + 32 }, { 32, 16, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17637, { 0, 0, height + 32 }, { 32, 16, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17630, { 0, 16, height + 32 }, { 32, 12, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17638, { 0, 16, height + 32 }, { 32, 12, 3 });
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x007C7304 */
static void flying_rc_track_flyer_half_loop_down(
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17629, { 0, 0, height + 32 }, { 32, 16, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17637, { 0, 0, height + 32 }, { 32, 16, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17630, { 0, 16, height + 32 }, { 32, 12, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17638, { 0, 16, height + 32 }, { 32, 12, 3 });
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17628, { 16, 0, height }, { 2, 16, 119 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17636, { 12, 0, height }, { 3, 16, 119 },
                        { 12, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17631, { 10, 16, height }, { 4, 12, 119 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17639, { 16, 16, height }, { 2, 16, 119 },
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17627, { 0, 0, height }, { 32, 20, 3 },
                        { 0, 6, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 4, 20, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17635, { 0, 14, height }, { 3, 20, 63 },
                        { 28, 6, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 4, 15, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17632, { 0, 6, height }, { 3, 20, 63 },
                        { 28, 6, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 4, 16, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17640, { 0, 6, height }, { 32, 20, 3 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17626, { 0, 6, height }, { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17634, { 0, 6, height }, { 32, 20, 11 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17633, { 0, 6, height }, { 32, 20, 9 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK] | 17641, { 0, 6, height }, { 32, 20, 7 });
                    break;
            }
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_SQUARE_7);
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 56, 0x20);
            break;
    }
}

/** rct2: 0x007C7664 */
static void flying_rc_track_block_brakes(
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
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17150, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 1:
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17151, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
        }
        metal_a_supports_paint_setup(
            session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
        paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
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
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27337, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 24 });
                break;
            case 1:
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27338, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 24 });
                break;
        }

        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        if (track_paint_util_should_paint_supports(session.MapPosition))
        {
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 39, session.TrackColours[SCHEME_SUPPORTS]);
        }

        paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
        paint_util_set_general_support_height(session, height + 32, 0x20);
    }
}

/** rct2: 0x007C76B4 */
static void flying_rc_track_left_banked_quarter_turn_3_25_deg_up(
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 18025, { 0, 6, height }, { 32, 20, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 18027, { 0, 6, height }, { 32, 1, 34 },
                            { 0, 27, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 18029, { 0, 6, height }, { 32, 20, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 18023, { 0, 6, height }, { 32, 20, 3 });
                        break;
                }
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                if (direction == 0 || direction == 3)
                {
                    paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_SQUARE_7);
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 18024, { 6, 0, height }, { 20, 32, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 18026, { 6, 0, height }, { 1, 32, 34 },
                            { 27, 0, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 18028, { 6, 0, height }, { 1, 32, 34 },
                            { 27, 0, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 18022, { 6, 0, height }, { 20, 32, 3 });
                        break;
                }
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                switch (direction)
                {
                    case 2:
                        paint_util_push_tunnel_right(session, height + 8, TUNNEL_SQUARE_8);
                        break;
                    case 3:
                        paint_util_push_tunnel_left(session, height + 8, TUNNEL_SQUARE_8);
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27762, { 0, 6, height + 24 },
                            { 32, 20, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27764, { 0, 6, height + 24 },
                            { 32, 20, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27766, { 0, 6, height + 24 },
                            { 32, 20, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27760, { 0, 6, height + 24 },
                            { 32, 20, 3 });
                        break;
                }

                paint_util_set_segment_support_height(
                    session,
                    paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 47, session.TrackColours[SCHEME_SUPPORTS]);

                if (direction == 0 || direction == 3)
                {
                    paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_SQUARE_7);
                }
                paint_util_set_general_support_height(session, height + 88, 0x20);
                break;
            case 1:
                paint_util_set_general_support_height(session, height + 72, 0x20);
                break;
            case 2:
                paint_util_set_general_support_height(session, height + 72, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27761, { 6, 0, height + 24 },
                            { 20, 32, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27763, { 6, 0, height + 24 },
                            { 20, 32, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27765, { 6, 0, height + 24 },
                            { 20, 32, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27759, { 6, 0, height + 24 },
                            { 20, 32, 3 });
                        break;
                }

                paint_util_set_segment_support_height(
                    session,
                    paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D4, direction),
                    0xFFFF, 0);
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 47, session.TrackColours[SCHEME_SUPPORTS]);

                switch (direction)
                {
                    case 2:
                        paint_util_push_tunnel_right(session, height + 8, TUNNEL_SQUARE_8);
                        break;
                    case 3:
                        paint_util_push_tunnel_left(session, height + 8, TUNNEL_SQUARE_8);
                        break;
                }
                paint_util_set_general_support_height(session, height + 88, 0x20);
                break;
        }
    }
}

/** rct2: 0x007C76C4 */
static void flying_rc_track_right_banked_quarter_turn_3_25_deg_up(
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 18014, { 0, 6, height }, { 32, 20, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 18016, { 0, 6, height }, { 32, 20, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 18018, { 0, 6, height }, { 32, 1, 34 },
                            { 0, 27, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 18020, { 0, 6, height }, { 32, 20, 3 });
                        break;
                }
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                if (direction == 0 || direction == 3)
                {
                    paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_SQUARE_7);
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 18015, { 6, 0, height }, { 20, 32, 3 });
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 18017, { 6, 0, height }, { 1, 32, 34 },
                            { 27, 0, height });
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 18019, { 6, 0, height }, { 1, 32, 34 },
                            { 27, 0, height });
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 4, 10, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 18021, { 6, 0, height }, { 20, 32, 3 });
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES_INVERTED, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
                switch (direction)
                {
                    case 0:
                        paint_util_push_tunnel_right(session, height + 8, TUNNEL_SQUARE_8);
                        break;
                    case 1:
                        paint_util_push_tunnel_left(session, height + 8, TUNNEL_SQUARE_8);
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27751, { 0, 6, height + 24 },
                            { 32, 20, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27753, { 0, 6, height + 24 },
                            { 32, 20, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27755, { 0, 6, height + 24 },
                            { 32, 20, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27757, { 0, 6, height + 24 },
                            { 32, 20, 3 });
                        break;
                }

                paint_util_set_segment_support_height(
                    session,
                    paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                    0xFFFF, 0);
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 47, session.TrackColours[SCHEME_SUPPORTS]);

                if (direction == 0 || direction == 3)
                {
                    paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_SQUARE_7);
                }
                paint_util_set_general_support_height(session, height + 88, 0x20);
                break;
            case 1:
                paint_util_set_general_support_height(session, height + 72, 0x20);
                break;
            case 2:
                paint_util_set_general_support_height(session, height + 72, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27752, { 6, 0, height + 24 },
                            { 20, 32, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27754, { 6, 0, height + 24 },
                            { 20, 32, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27756, { 6, 0, height + 24 },
                            { 20, 32, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27758, { 6, 0, height + 24 },
                            { 20, 32, 3 });
                        break;
                }

                paint_util_set_segment_support_height(
                    session,
                    paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D4, direction),
                    0xFFFF, 0);
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 47, session.TrackColours[SCHEME_SUPPORTS]);

                switch (direction)
                {
                    case 0:
                        paint_util_push_tunnel_right(session, height + 8, TUNNEL_SQUARE_8);
                        break;
                    case 1:
                        paint_util_push_tunnel_left(session, height + 8, TUNNEL_SQUARE_8);
                        break;
                }
                paint_util_set_general_support_height(session, height + 88, 0x20);
                break;
        }
    }
}

/** rct2: 0x007C76D4 */
static void flying_rc_track_left_banked_quarter_turn_3_25_deg_down(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    flying_rc_track_right_banked_quarter_turn_3_25_deg_up(
        session, ride, trackSequence, (direction + 1) & 3, height, trackElement);
}

/** rct2: 0x007C76E4 */
static void flying_rc_track_right_banked_quarter_turn_3_25_deg_down(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    flying_rc_track_left_banked_quarter_turn_3_25_deg_up(
        session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x007C76F4 */
static void flying_rc_track_left_banked_quarter_turn_5_25_deg_up(
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17978, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17983, { 0, 0, height }, { 32, 1, 34 },
                            { 0, 27, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17988, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17993, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                }
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                if (direction == 0 || direction == 3)
                {
                    paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_SQUARE_7);
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17979, { 0, 0, height }, { 32, 16, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17984, { 0, 0, height }, { 1, 1, 34 },
                            { 30, 30, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17989, { 0, 0, height }, { 32, 16, 3 },
                            { 0, 16, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17994, { 0, 0, height }, { 32, 16, 3 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17980, { 0, 0, height }, { 16, 16, 3 },
                            { 0, 16, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17985, { 0, 0, height }, { 1, 1, 34 },
                            { 30, 30, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17990, { 0, 0, height }, { 16, 16, 3 },
                            { 16, 0, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17995, { 0, 0, height }, { 16, 16, 3 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17981, { 0, 0, height }, { 16, 32, 3 },
                            { 16, 0, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17986, { 0, 0, height }, { 1, 1, 34 },
                            { 30, 30, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17991, { 0, 0, height }, { 1, 32, 34 },
                            { 27, 0, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17996, { 0, 0, height }, { 16, 32, 3 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17982, { 0, 0, height }, { 20, 32, 3 },
                            { 6, 0, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17987, { 0, 0, height }, { 1, 32, 34 },
                            { 27, 0, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17992, { 0, 0, height }, { 1, 32, 34 },
                            { 27, 0, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17997, { 0, 0, height }, { 20, 32, 3 },
                            { 6, 0, height });
                        break;
                }
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                switch (direction)
                {
                    case 2:
                        paint_util_push_tunnel_right(session, height + 8, TUNNEL_SQUARE_8);
                        break;
                    case 3:
                        paint_util_push_tunnel_left(session, height + 8, TUNNEL_SQUARE_8);
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27787, { 0, 6, height + 24 },
                            { 32, 20, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27792, { 0, 6, height + 24 },
                            { 32, 20, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27797, { 0, 6, height + 24 },
                            { 32, 20, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27802, { 0, 6, height + 24 },
                            { 32, 20, 3 });
                        break;
                }

                paint_util_set_segment_support_height(
                    session,
                    paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 49, session.TrackColours[SCHEME_SUPPORTS]);

                if (direction == 0 || direction == 3)
                {
                    paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_SQUARE_7);
                }
                paint_util_set_general_support_height(session, height + 88, 0x20);
                break;
            case 1:
                paint_util_set_general_support_height(session, height + 88, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27788, { 0, 0, height + 24 },
                            { 32, 16, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27793, { 0, 0, height + 24 },
                            { 32, 16, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27798, { 0, 16, height + 24 },
                            { 32, 16, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27803, { 0, 16, height + 24 },
                            { 32, 16, 3 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session,
                    paint_util_rotate_segments(
                        SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                    0xFFFF, 0);
                paint_util_set_general_support_height(session, height + 88, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27789, { 0, 16, height + 24 },
                            { 16, 16, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27794, { 16, 16, height + 24 },
                            { 16, 16, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27799, { 16, 0, height + 24 },
                            { 16, 16, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27804, { 0, 0, height + 24 },
                            { 16, 16, 3 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 80, 0x20);
                break;
            case 4:
                paint_util_set_general_support_height(session, height + 88, 0x20);
                break;
            case 5:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27790, { 16, 0, height + 24 },
                            { 16, 32, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27795, { 0, 0, height + 24 },
                            { 16, 32, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27800, { 0, 0, height + 24 },
                            { 16, 32, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27805, { 16, 0, height + 24 },
                            { 16, 32, 3 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session,
                    paint_util_rotate_segments(
                        SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                paint_util_set_general_support_height(session, height + 88, 0x20);
                break;
            case 6:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27791, { 6, 0, height + 24 },
                            { 20, 32, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27796, { 6, 0, height + 24 },
                            { 20, 32, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27801, { 6, 0, height + 24 },
                            { 20, 32, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27806, { 6, 0, height + 24 },
                            { 20, 32, 3 });
                        break;
                }

                paint_util_set_segment_support_height(
                    session,
                    paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D4, direction),
                    0xFFFF, 0);
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 49, session.TrackColours[SCHEME_SUPPORTS]);

                switch (direction)
                {
                    case 2:
                        paint_util_push_tunnel_right(session, height + 8, TUNNEL_SQUARE_8);
                        break;
                    case 3:
                        paint_util_push_tunnel_left(session, height + 8, TUNNEL_SQUARE_8);
                        break;
                }
                paint_util_set_general_support_height(session, height + 88, 0x20);
                break;
        }
    }
}

/** rct2: 0x007C7704 */
static void flying_rc_track_right_banked_quarter_turn_5_25_deg_up(
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17958, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17963, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17968, { 0, 0, height }, { 32, 1, 34 },
                            { 0, 27, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17973, { 0, 0, height }, { 32, 20, 3 },
                            { 0, 6, height });
                        break;
                }
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                if (direction == 0 || direction == 3)
                {
                    paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_SQUARE_7);
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17959, { 0, 0, height }, { 32, 16, 3 },
                            { 0, 16, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17964, { 0, 0, height }, { 32, 16, 3 },
                            { 0, 16, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17969, { 0, 0, height }, { 1, 1, 34 },
                            { 30, 30, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17974, { 0, 0, height }, { 32, 16, 3 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17960, { 0, 0, height }, { 16, 16, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17965, { 0, 0, height }, { 16, 16, 3 },
                            { 16, 0, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17970, { 0, 0, height }, { 1, 1, 34 },
                            { 30, 30, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17975, { 0, 0, height }, { 16, 16, 3 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17961, { 0, 0, height }, { 16, 32, 3 },
                            { 16, 0, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17966, { 0, 0, height }, { 1, 32, 34 },
                            { 27, 0, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17971, { 0, 0, height }, { 1, 1, 34 },
                            { 30, 30, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17976, { 0, 0, height }, { 16, 32, 3 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17962, { 0, 0, height }, { 20, 32, 3 },
                            { 6, 0, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17967, { 0, 0, height }, { 1, 32, 34 },
                            { 27, 0, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17972, { 0, 0, height }, { 1, 32, 34 },
                            { 27, 0, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 17977, { 0, 0, height }, { 20, 32, 3 },
                            { 6, 0, height });
                        break;
                }
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                switch (direction)
                {
                    case 0:
                        paint_util_push_tunnel_right(session, height + 8, TUNNEL_SQUARE_8);
                        break;
                    case 1:
                        paint_util_push_tunnel_left(session, height + 8, TUNNEL_SQUARE_8);
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
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27767, { 0, 6, height + 24 },
                            { 32, 20, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27772, { 0, 6, height + 24 },
                            { 32, 20, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27777, { 0, 6, height + 24 },
                            { 32, 20, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27782, { 0, 6, height + 24 },
                            { 32, 20, 3 });
                        break;
                }

                paint_util_set_segment_support_height(
                    session,
                    paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                    0xFFFF, 0);
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 49, session.TrackColours[SCHEME_SUPPORTS]);

                if (direction == 0 || direction == 3)
                {
                    paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_SQUARE_7);
                }
                paint_util_set_general_support_height(session, height + 88, 0x20);
                break;
            case 1:
                paint_util_set_general_support_height(session, height + 88, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27768, { 0, 16, height + 24 },
                            { 32, 16, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27773, { 0, 16, height + 24 },
                            { 32, 16, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27778, { 0, 0, height + 24 },
                            { 32, 16, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27783, { 0, 0, height + 24 },
                            { 32, 16, 3 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session,
                    paint_util_rotate_segments(
                        SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                paint_util_set_general_support_height(session, height + 88, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27769, { 0, 0, height + 24 },
                            { 16, 16, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27774, { 16, 0, height + 24 },
                            { 16, 16, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27779, { 16, 16, height + 24 },
                            { 16, 16, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27784, { 0, 16, height + 24 },
                            { 16, 16, 3 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF,
                    0);
                paint_util_set_general_support_height(session, height + 80, 0x20);
                break;
            case 4:
                paint_util_set_general_support_height(session, height + 88, 0x20);
                break;
            case 5:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27770, { 16, 0, height + 24 },
                            { 16, 32, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27775, { 0, 0, height + 24 },
                            { 16, 32, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27780, { 0, 0, height + 24 },
                            { 16, 32, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27785, { 16, 0, height + 24 },
                            { 16, 32, 3 });
                        break;
                }
                paint_util_set_segment_support_height(
                    session,
                    paint_util_rotate_segments(
                        SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                paint_util_set_general_support_height(session, height + 88, 0x20);
                break;
            case 6:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27771, { 6, 0, height + 24 },
                            { 20, 32, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27776, { 6, 0, height + 24 },
                            { 20, 32, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27781, { 6, 0, height + 24 },
                            { 20, 32, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK] | 27786, { 6, 0, height + 24 },
                            { 20, 32, 3 });
                        break;
                }

                paint_util_set_segment_support_height(
                    session,
                    paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D4, direction),
                    0xFFFF, 0);
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 49, session.TrackColours[SCHEME_SUPPORTS]);

                switch (direction)
                {
                    case 0:
                        paint_util_push_tunnel_right(session, height + 8, TUNNEL_SQUARE_8);
                        break;
                    case 1:
                        paint_util_push_tunnel_left(session, height + 8, TUNNEL_SQUARE_8);
                        break;
                }
                paint_util_set_general_support_height(session, height + 88, 0x20);
                break;
        }
    }
}

/** rct2: 0x007C7714 */
static void flying_rc_track_left_banked_quarter_turn_5_25_deg_down(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    flying_rc_track_right_banked_quarter_turn_5_25_deg_up(
        session, ride, trackSequence, (direction + 1) & 3, height, trackElement);
}

/** rct2: 0x007C7724 */
static void flying_rc_track_right_banked_quarter_turn_5_25_deg_down(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    flying_rc_track_left_banked_quarter_turn_5_25_deg_up(
        session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x007C7734 */
static void flying_rc_track_25_deg_up_to_left_banked_25_deg_up(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17922, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17923, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17930, { 0, 0, height }, { 32, 1, 34 },
                    { 0, 27, height });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17924, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17925, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
        }
        if (track_paint_util_should_paint_supports(session.MapPosition))
        {
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
        }
        if (direction == 0 || direction == 3)
        {
            paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_SQUARE_7);
        }
        else
        {
            paint_util_push_tunnel_rotated(session, direction, height + 8, TUNNEL_SQUARE_8);
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 56, 0x20);
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27719, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 40 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27720, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 40 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27721, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 40 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27722, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 40 });
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
                        session, METAL_SUPPORTS_TUBES_INVERTED, 6, 0, height + 57, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height + 57, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 7, 0, height + 57, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height + 57, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
        }

        if (direction == 0 || direction == 3)
        {
            paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_SQUARE_7);
        }
        else
        {
            paint_util_push_tunnel_rotated(session, direction, height + 8, TUNNEL_SQUARE_8);
        }
        paint_util_set_general_support_height(session, height + 72, 0x20);
    }
}

/** rct2: 0x007C7744 */
static void flying_rc_track_25_deg_up_to_right_banked_25_deg_up(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17926, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17927, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17928, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17931, { 0, 0, height }, { 32, 1, 34 },
                    { 0, 27, height });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17929, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
        }
        if (track_paint_util_should_paint_supports(session.MapPosition))
        {
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
        }
        if (direction == 0 || direction == 3)
        {
            paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_SQUARE_7);
        }
        else
        {
            paint_util_push_tunnel_rotated(session, direction, height + 8, TUNNEL_SQUARE_8);
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 56, 0x20);
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27723, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 40 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27724, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 40 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27725, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 40 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27726, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 40 });
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
                        session, METAL_SUPPORTS_TUBES_INVERTED, 6, 0, height + 57, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height + 57, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 7, 0, height + 57, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height + 57, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
        }

        if (direction == 0 || direction == 3)
        {
            paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_SQUARE_7);
        }
        else
        {
            paint_util_push_tunnel_rotated(session, direction, height + 8, TUNNEL_SQUARE_8);
        }
        paint_util_set_general_support_height(session, height + 72, 0x20);
    }
}

/** rct2: 0x007C7754 */
static void flying_rc_track_left_banked_25_deg_up_to_25_deg_up(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17932, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17933, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17940, { 0, 0, height }, { 32, 1, 34 },
                    { 0, 27, height });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17934, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17935, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
        }
        if (track_paint_util_should_paint_supports(session.MapPosition))
        {
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
        }
        if (direction == 0 || direction == 3)
        {
            paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_SQUARE_7);
        }
        else
        {
            paint_util_push_tunnel_rotated(session, direction, height + 8, TUNNEL_SQUARE_8);
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 56, 0x20);
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27727, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 40 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27728, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 40 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27729, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 40 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27730, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 40 });
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
                        session, METAL_SUPPORTS_TUBES_INVERTED, 6, 0, height + 57, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height + 57, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 7, 0, height + 57, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height + 57, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
        }

        if (direction == 0 || direction == 3)
        {
            paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_SQUARE_7);
        }
        else
        {
            paint_util_push_tunnel_rotated(session, direction, height + 8, TUNNEL_SQUARE_8);
        }
        paint_util_set_general_support_height(session, height + 72, 0x20);
    }
}

/** rct2: 0x007C7764 */
static void flying_rc_track_right_banked_25_deg_up_to_25_deg_up(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17936, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17937, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17938, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17941, { 0, 0, height }, { 32, 1, 34 },
                    { 0, 27, height });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17939, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
        }
        if (track_paint_util_should_paint_supports(session.MapPosition))
        {
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
        }
        if (direction == 0 || direction == 3)
        {
            paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_SQUARE_7);
        }
        else
        {
            paint_util_push_tunnel_rotated(session, direction, height + 8, TUNNEL_SQUARE_8);
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 56, 0x20);
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27731, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 40 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27732, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 40 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27733, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 40 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27734, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 40 });
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
                        session, METAL_SUPPORTS_TUBES_INVERTED, 6, 0, height + 57, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height + 57, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 7, 0, height + 57, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height + 57, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
        }

        if (direction == 0 || direction == 3)
        {
            paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_SQUARE_7);
        }
        else
        {
            paint_util_push_tunnel_rotated(session, direction, height + 8, TUNNEL_SQUARE_8);
        }
        paint_util_set_general_support_height(session, height + 72, 0x20);
    }
}

/** rct2: 0x007C7774 */
static void flying_rc_track_25_deg_down_to_left_banked_25_deg_down(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    flying_rc_track_right_banked_25_deg_up_to_25_deg_up(
        session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x007C7784 */
static void flying_rc_track_25_deg_down_to_right_banked_25_deg_down(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    flying_rc_track_left_banked_25_deg_up_to_25_deg_up(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x007C7794 */
static void flying_rc_track_left_banked_25_deg_down_to_25_deg_down(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    flying_rc_track_25_deg_up_to_right_banked_25_deg_up(
        session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x007C77A4 */
static void flying_rc_track_right_banked_25_deg_down_to_25_deg_down(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    flying_rc_track_25_deg_up_to_left_banked_25_deg_up(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x007C77B4 */
static void flying_rc_track_left_banked_flat_to_left_banked_25_deg_up(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17942, { 0, 0, height }, { 32, 1, 34 },
                    { 0, 27, height });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17943, { 0, 0, height }, { 32, 1, 34 },
                    { 0, 27, height });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17944, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17945, { 0, 0, height }, { 32, 20, 3 },
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
            paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
        }
        else
        {
            paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_8);
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
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27735, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 32 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27736, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 32 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27737, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 32 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27738, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 32 });
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
                        session, METAL_SUPPORTS_TUBES_INVERTED, 6, 0, height + 49, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height + 49, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 7, 0, height + 49, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height + 49, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
        }

        if (direction == 0 || direction == 3)
        {
            paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
        }
        else
        {
            paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_8);
        }
        paint_util_set_general_support_height(session, height + 64, 0x20);
    }
}

/** rct2: 0x007C77C4 */
static void flying_rc_track_right_banked_flat_to_right_banked_25_deg_up(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17946, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17947, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17948, { 0, 0, height }, { 32, 1, 34 },
                    { 0, 27, height });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17949, { 0, 0, height }, { 32, 20, 3 },
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
            paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
        }
        else
        {
            paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_8);
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
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27739, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 32 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27740, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 32 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27741, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 32 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27742, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 32 });
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
                        session, METAL_SUPPORTS_TUBES_INVERTED, 6, 0, height + 49, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height + 49, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 7, 0, height + 49, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height + 49, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
        }

        if (direction == 0 || direction == 3)
        {
            paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
        }
        else
        {
            paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_8);
        }
        paint_util_set_general_support_height(session, height + 64, 0x20);
    }
}

/** rct2: 0x007C77F4 */
static void flying_rc_track_left_banked_25_deg_up_to_left_banked_flat(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17950, { 0, 0, height }, { 32, 1, 34 },
                    { 0, 27, height });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17951, { 0, 0, height }, { 32, 1, 34 },
                    { 0, 27, height });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17952, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17953, { 0, 0, height }, { 32, 20, 3 },
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
            paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_SQUARE_FLAT);
        }
        else
        {
            paint_util_push_tunnel_rotated(session, direction, height + 8, TUNNEL_SQUARE_FLAT);
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
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27743, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 32 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27744, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 32 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27745, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 32 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27746, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 32 });
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
                        session, METAL_SUPPORTS_TUBES_INVERTED, 6, 0, height + 47, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height + 47, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 7, 0, height + 47, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height + 47, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
        }

        if (direction == 0 || direction == 3)
        {
            paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_SQUARE_FLAT);
        }
        else
        {
            paint_util_push_tunnel_rotated(session, direction, height + 8, TUNNEL_14);
        }
        paint_util_set_general_support_height(session, height + 56, 0x20);
    }
}

/** rct2: 0x007C7804 */
static void flying_rc_track_right_banked_25_deg_up_to_right_banked_flat(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17954, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17955, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17956, { 0, 0, height }, { 32, 1, 34 },
                    { 0, 27, height });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17957, { 0, 0, height }, { 32, 20, 3 },
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
            paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_SQUARE_FLAT);
        }
        else
        {
            paint_util_push_tunnel_rotated(session, direction, height + 8, TUNNEL_14);
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
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27747, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 32 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27748, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 32 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27749, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 32 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27750, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 32 });
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
                        session, METAL_SUPPORTS_TUBES_INVERTED, 6, 0, height + 47, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height + 47, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 7, 0, height + 47, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height + 47, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
        }

        if (direction == 0 || direction == 3)
        {
            paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_SQUARE_FLAT);
        }
        else
        {
            paint_util_push_tunnel_rotated(session, direction, height + 8, TUNNEL_14);
        }
        paint_util_set_general_support_height(session, height + 56, 0x20);
    }
}

/** rct2: 0x007C7814 */
static void flying_rc_track_left_banked_flat_to_left_banked_25_deg_down(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    flying_rc_track_right_banked_25_deg_up_to_right_banked_flat(
        session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x007C7824 */
static void flying_rc_track_right_banked_flat_to_right_banked_25_deg_down(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    flying_rc_track_left_banked_25_deg_up_to_left_banked_flat(
        session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x007C77D4 */
static void flying_rc_track_left_banked_25_deg_down_to_left_banked_flat(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    flying_rc_track_right_banked_flat_to_right_banked_25_deg_up(
        session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x007C77E4 */
static void flying_rc_track_right_banked_25_deg_down_to_right_banked_flat(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    flying_rc_track_left_banked_flat_to_left_banked_25_deg_up(
        session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x007C7834 */
static void flying_rc_track_flat_to_left_banked_25_deg_up(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17894, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17895, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17902, { 0, 0, height }, { 32, 1, 34 },
                    { 0, 27, height });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17896, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17897, { 0, 0, height }, { 32, 20, 3 },
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
            paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
        }
        else
        {
            paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_8);
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
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27695, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 32 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27696, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 32 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27697, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 32 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27698, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 32 });
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
                        session, METAL_SUPPORTS_TUBES_INVERTED, 6, 0, height + 49, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height + 49, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 7, 0, height + 49, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height + 49, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
        }

        if (direction == 0 || direction == 3)
        {
            paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
        }
        else
        {
            paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_8);
        }
        paint_util_set_general_support_height(session, height + 64, 0x20);
    }
}

/** rct2: 0x007C7844 */
static void flying_rc_track_flat_to_right_banked_25_deg_up(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17898, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17899, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17900, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17903, { 0, 0, height }, { 32, 1, 34 },
                    { 0, 27, height });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17901, { 0, 0, height }, { 32, 20, 3 },
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
            paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
        }
        else
        {
            paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_8);
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
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27699, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 32 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27700, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 32 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27701, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 32 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27702, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 32 });
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
                        session, METAL_SUPPORTS_TUBES_INVERTED, 6, 0, height + 49, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height + 49, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 7, 0, height + 49, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height + 49, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
        }

        if (direction == 0 || direction == 3)
        {
            paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
        }
        else
        {
            paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_8);
        }
        paint_util_set_general_support_height(session, height + 64, 0x20);
    }
}

/** rct2: 0x007C7854 */
static void flying_rc_track_left_banked_25_deg_up_to_flat(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17904, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17905, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17912, { 0, 0, height }, { 32, 1, 34 },
                    { 0, 27, height });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17906, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17907, { 0, 0, height }, { 32, 20, 3 },
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
            paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_SQUARE_FLAT);
        }
        else
        {
            paint_util_push_tunnel_rotated(session, direction, height + 8, TUNNEL_14);
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
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27703, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 32 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27704, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 32 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27705, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 32 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27706, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 32 });
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
                        session, METAL_SUPPORTS_TUBES_INVERTED, 6, 0, height + 47, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height + 47, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 7, 0, height + 47, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height + 47, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
        }

        if (direction == 0 || direction == 3)
        {
            paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_SQUARE_FLAT);
        }
        else
        {
            paint_util_push_tunnel_rotated(session, direction, height + 8, TUNNEL_14);
        }
        paint_util_set_general_support_height(session, height + 56, 0x20);
    }
}

/** rct2: 0x007C7864 */
static void flying_rc_track_right_banked_25_deg_up_to_flat(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!trackElement.IsInverted())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17908, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17909, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17910, { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17913, { 0, 0, height }, { 32, 1, 34 },
                    { 0, 27, height });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 17911, { 0, 0, height }, { 32, 20, 3 },
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
            paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_SQUARE_FLAT);
        }
        else
        {
            paint_util_push_tunnel_rotated(session, direction, height + 8, TUNNEL_14);
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
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27707, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 32 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27708, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 32 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27709, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 32 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK] | 27710, { 0, 0, height + 24 }, { 32, 20, 3 },
                    { 0, 6, height + 32 });
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
                        session, METAL_SUPPORTS_TUBES_INVERTED, 6, 0, height + 47, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height + 47, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 7, 0, height + 47, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height + 47, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
        }

        if (direction == 0 || direction == 3)
        {
            paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_SQUARE_FLAT);
        }
        else
        {
            paint_util_push_tunnel_rotated(session, direction, height + 8, TUNNEL_14);
        }
        paint_util_set_general_support_height(session, height + 56, 0x20);
    }
}

/** rct2: 0x007C7874 */
static void flying_rc_track_flat_to_left_banked_25_deg_down(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    flying_rc_track_right_banked_25_deg_up_to_flat(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x007C7884 */
static void flying_rc_track_flat_to_right_banked_25_deg_down(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    flying_rc_track_left_banked_25_deg_up_to_flat(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x007C7894 */
static void flying_rc_track_left_banked_25_deg_down_to_flat(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    flying_rc_track_flat_to_right_banked_25_deg_up(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x007C78A4 */
static void flying_rc_track_right_banked_25_deg_down_to_flat(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    flying_rc_track_flat_to_left_banked_25_deg_up(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

TRACK_PAINT_FUNCTION get_track_paint_function_flying_rc(int32_t trackType)
{
    switch (trackType)
    {
        case TrackElemType::Flat:
            return flying_rc_track_flat;
        case TrackElemType::EndStation:
        case TrackElemType::BeginStation:
        case TrackElemType::MiddleStation:
            return flying_rc_track_station;
        case TrackElemType::Up25:
            return flying_rc_track_25_deg_up;
        case TrackElemType::Up60:
            return flying_rc_track_60_deg_up;
        case TrackElemType::FlatToUp25:
            return flying_rc_track_flat_to_25_deg_up;
        case TrackElemType::Up25ToUp60:
            return flying_rc_track_25_deg_up_to_60_deg_up;
        case TrackElemType::Up60ToUp25:
            return flying_rc_track_60_deg_up_to_25_deg_up;
        case TrackElemType::Up25ToFlat:
            return flying_rc_track_25_deg_up_to_flat;
        case TrackElemType::Down25:
            return flying_rc_track_25_deg_down;
        case TrackElemType::Down60:
            return flying_rc_track_60_deg_down;
        case TrackElemType::FlatToDown25:
            return flying_rc_track_flat_to_25_deg_down;
        case TrackElemType::Down25ToDown60:
            return flying_rc_track_25_deg_down_to_60_deg_down;
        case TrackElemType::Down60ToDown25:
            return flying_rc_track_60_deg_down_to_25_deg_down;
        case TrackElemType::Down25ToFlat:
            return flying_rc_track_25_deg_down_to_flat;
        case TrackElemType::LeftQuarterTurn5Tiles:
            return flying_rc_track_left_quarter_turn_5;
        case TrackElemType::RightQuarterTurn5Tiles:
            return flying_rc_track_right_quarter_turn_5;
        case TrackElemType::FlatToLeftBank:
            return flying_rc_track_flat_to_left_bank;
        case TrackElemType::FlatToRightBank:
            return flying_rc_track_flat_to_right_bank;
        case TrackElemType::LeftBankToFlat:
            return flying_rc_track_left_bank_to_flat;
        case TrackElemType::RightBankToFlat:
            return flying_rc_track_right_bank_to_flat;
        case TrackElemType::BankedLeftQuarterTurn5Tiles:
            return flying_rc_track_banked_left_quarter_turn_5;
        case TrackElemType::BankedRightQuarterTurn5Tiles:
            return flying_rc_track_banked_right_quarter_turn_5;
        case TrackElemType::LeftBankToUp25:
            return flying_rc_track_left_bank_to_25_deg_up;
        case TrackElemType::RightBankToUp25:
            return flying_rc_track_right_bank_to_25_deg_up;
        case TrackElemType::Up25ToLeftBank:
            return flying_rc_track_25_deg_up_to_left_bank;
        case TrackElemType::Up25ToRightBank:
            return flying_rc_track_25_deg_up_to_right_bank;
        case TrackElemType::LeftBankToDown25:
            return flying_rc_track_left_bank_to_25_deg_down;
        case TrackElemType::RightBankToDown25:
            return flying_rc_track_right_bank_to_25_deg_down;
        case TrackElemType::Down25ToLeftBank:
            return flying_rc_track_25_deg_down_to_left_bank;
        case TrackElemType::Down25ToRightBank:
            return flying_rc_track_25_deg_down_to_right_bank;
        case TrackElemType::LeftBank:
            return flying_rc_track_left_bank;
        case TrackElemType::RightBank:
            return flying_rc_track_right_bank;
        case TrackElemType::LeftQuarterTurn5TilesUp25:
            return flying_rc_track_left_quarter_turn_5_25_deg_up;
        case TrackElemType::RightQuarterTurn5TilesUp25:
            return flying_rc_track_right_quarter_turn_5_25_deg_up;
        case TrackElemType::LeftQuarterTurn5TilesDown25:
            return flying_rc_track_left_quarter_turn_5_25_deg_down;
        case TrackElemType::RightQuarterTurn5TilesDown25:
            return flying_rc_track_right_quarter_turn_5_25_deg_down;
        case TrackElemType::SBendLeft:
            return flying_rc_track_s_bend_left;
        case TrackElemType::SBendRight:
            return flying_rc_track_s_bend_right;
        case TrackElemType::LeftQuarterTurn3Tiles:
            return flying_rc_track_left_quarter_turn_3;
        case TrackElemType::RightQuarterTurn3Tiles:
            return flying_rc_track_right_quarter_turn_3;
        case TrackElemType::LeftBankedQuarterTurn3Tiles:
            return flying_rc_track_left_quarter_turn_3_bank;
        case TrackElemType::RightBankedQuarterTurn3Tiles:
            return flying_rc_track_right_quarter_turn_3_bank;
        case TrackElemType::LeftQuarterTurn3TilesUp25:
            return flying_rc_track_left_quarter_turn_3_25_deg_up;
        case TrackElemType::RightQuarterTurn3TilesUp25:
            return flying_rc_track_right_quarter_turn_3_25_deg_up;
        case TrackElemType::LeftQuarterTurn3TilesDown25:
            return flying_rc_track_left_quarter_turn_3_25_deg_down;
        case TrackElemType::RightQuarterTurn3TilesDown25:
            return flying_rc_track_right_quarter_turn_3_25_deg_down;
        case TrackElemType::LeftHalfBankedHelixUpSmall:
            return flying_rc_track_left_half_banked_helix_up_small;
        case TrackElemType::RightHalfBankedHelixUpSmall:
            return flying_rc_track_right_half_banked_helix_up_small;
        case TrackElemType::LeftHalfBankedHelixDownSmall:
            return flying_rc_track_left_half_banked_helix_down_small;
        case TrackElemType::RightHalfBankedHelixDownSmall:
            return flying_rc_track_right_half_banked_helix_down_small;
        case TrackElemType::LeftHalfBankedHelixUpLarge:
            return flying_rc_track_left_half_banked_helix_up_large;
        case TrackElemType::RightHalfBankedHelixUpLarge:
            return flying_rc_track_right_half_banked_helix_up_large;
        case TrackElemType::LeftHalfBankedHelixDownLarge:
            return flying_rc_track_left_half_banked_helix_down_large;
        case TrackElemType::RightHalfBankedHelixDownLarge:
            return flying_rc_track_right_half_banked_helix_down_large;
        case TrackElemType::LeftQuarterTurn1TileUp60:
            return flying_rc_track_left_quarter_turn_1_60_deg_up;
        case TrackElemType::RightQuarterTurn1TileUp60:
            return flying_rc_track_right_quarter_turn_1_60_deg_up;
        case TrackElemType::LeftQuarterTurn1TileDown60:
            return flying_rc_track_left_quarter_turn_1_60_deg_down;
        case TrackElemType::RightQuarterTurn1TileDown60:
            return flying_rc_track_right_quarter_turn_1_60_deg_down;
        case TrackElemType::Brakes:
            return flying_rc_track_brakes;
        case TrackElemType::LeftQuarterBankedHelixLargeUp:
            return flying_rc_track_left_quarter_banked_helix_large_up;
        case TrackElemType::RightQuarterBankedHelixLargeUp:
            return flying_rc_track_right_quarter_banked_helix_large_up;
        case TrackElemType::LeftQuarterBankedHelixLargeDown:
            return flying_rc_track_left_quarter_banked_helix_large_down;
        case TrackElemType::RightQuarterBankedHelixLargeDown:
            return flying_rc_track_right_quarter_banked_helix_large_down;
        case TrackElemType::Up25LeftBanked:
            return flying_rc_track_25_deg_up_left_banked;
        case TrackElemType::Up25RightBanked:
            return flying_rc_track_25_deg_up_right_banked;
        case TrackElemType::OnRidePhoto:
            return flying_rc_track_on_ride_photo;
        case TrackElemType::Down25LeftBanked:
            return flying_rc_track_25_deg_down_left_banked;
        case TrackElemType::Down25RightBanked:
            return flying_rc_track_25_deg_down_right_banked;
        case TrackElemType::LeftEighthToDiag:
            return flying_rc_track_left_eighth_to_diag;
        case TrackElemType::RightEighthToDiag:
            return flying_rc_track_right_eighth_to_diag;
        case TrackElemType::LeftEighthToOrthogonal:
            return flying_rc_track_left_eighth_to_orthogonal;
        case TrackElemType::RightEighthToOrthogonal:
            return flying_rc_track_right_eighth_to_orthogonal;
        case TrackElemType::LeftEighthBankToDiag:
            return flying_rc_track_left_eighth_bank_to_diag;
        case TrackElemType::RightEighthBankToDiag:
            return flying_rc_track_right_eighth_bank_to_diag;
        case TrackElemType::LeftEighthBankToOrthogonal:
            return flying_rc_track_left_eighth_bank_to_orthogonal;
        case TrackElemType::RightEighthBankToOrthogonal:
            return flying_rc_track_right_eighth_bank_to_orthogonal;
        case TrackElemType::DiagFlat:
            return flying_rc_track_diag_flat;
        case TrackElemType::DiagUp25:
            return flying_rc_track_diag_25_deg_up;
        case TrackElemType::DiagUp60:
            return flying_rc_track_diag_60_deg_up;
        case TrackElemType::DiagFlatToUp25:
            return flying_rc_track_diag_flat_to_25_deg_up;
        case TrackElemType::DiagUp25ToUp60:
            return flying_rc_track_diag_25_deg_up_to_60_deg_up;
        case TrackElemType::DiagUp60ToUp25:
            return flying_rc_track_diag_60_deg_up_to_25_deg_up;
        case TrackElemType::DiagUp25ToFlat:
            return flying_rc_track_diag_25_deg_up_to_flat;
        case TrackElemType::DiagDown25:
            return flying_rc_track_diag_25_deg_down;
        case TrackElemType::DiagDown60:
            return flying_rc_track_diag_60_deg_down;
        case TrackElemType::DiagFlatToDown25:
            return flying_rc_track_diag_flat_to_25_deg_down;
        case TrackElemType::DiagDown25ToDown60:
            return flying_rc_track_diag_25_deg_down_to_60_deg_down;
        case TrackElemType::DiagDown60ToDown25:
            return flying_rc_track_diag_60_deg_down_to_25_deg_down;
        case TrackElemType::DiagDown25ToFlat:
            return flying_rc_track_diag_25_deg_down_to_flat;
        case TrackElemType::DiagFlatToLeftBank:
            return flying_rc_track_diag_flat_to_left_bank;
        case TrackElemType::DiagFlatToRightBank:
            return flying_rc_track_diag_flat_to_right_bank;
        case TrackElemType::DiagLeftBankToFlat:
            return flying_rc_track_diag_left_bank_to_flat;
        case TrackElemType::DiagRightBankToFlat:
            return flying_rc_track_diag_right_bank_to_flat;
        case TrackElemType::DiagLeftBankToUp25:
            return flying_rc_track_diag_left_bank_to_25_deg_up;
        case TrackElemType::DiagRightBankToUp25:
            return flying_rc_track_diag_right_bank_to_25_deg_up;
        case TrackElemType::DiagUp25ToLeftBank:
            return flying_rc_track_diag_25_deg_up_to_left_bank;
        case TrackElemType::DiagUp25ToRightBank:
            return flying_rc_track_diag_25_deg_up_to_right_bank;
        case TrackElemType::DiagLeftBankToDown25:
            return flying_rc_track_diag_left_bank_to_25_deg_down;
        case TrackElemType::DiagRightBankToDown25:
            return flying_rc_track_diag_right_bank_to_25_deg_down;
        case TrackElemType::DiagDown25ToLeftBank:
            return flying_rc_track_diag_25_deg_down_to_left_bank;
        case TrackElemType::DiagDown25ToRightBank:
            return flying_rc_track_diag_25_deg_down_to_right_bank;
        case TrackElemType::DiagLeftBank:
            return flying_rc_track_diag_left_bank;
        case TrackElemType::DiagRightBank:
            return flying_rc_track_diag_right_bank;
        case TrackElemType::LeftFlyerTwistUp:
            return flying_rc_track_left_flyer_twist_up;
        case TrackElemType::RightFlyerTwistUp:
            return flying_rc_track_right_flyer_twist_up;
        case TrackElemType::LeftFlyerTwistDown:
            return flying_rc_track_left_flyer_twist_down;
        case TrackElemType::RightFlyerTwistDown:
            return flying_rc_track_right_flyer_twist_down;
        case TrackElemType::FlyerHalfLoopUp:
            return flying_rc_track_flyer_half_loop_up;
        case TrackElemType::FlyerHalfLoopDown:
            return flying_rc_track_flyer_half_loop_down;
        case TrackElemType::BlockBrakes:
            return flying_rc_track_block_brakes;
        case TrackElemType::LeftBankedQuarterTurn3TileUp25:
            return flying_rc_track_left_banked_quarter_turn_3_25_deg_up;
        case TrackElemType::RightBankedQuarterTurn3TileUp25:
            return flying_rc_track_right_banked_quarter_turn_3_25_deg_up;
        case TrackElemType::LeftBankedQuarterTurn3TileDown25:
            return flying_rc_track_left_banked_quarter_turn_3_25_deg_down;
        case TrackElemType::RightBankedQuarterTurn3TileDown25:
            return flying_rc_track_right_banked_quarter_turn_3_25_deg_down;
        case TrackElemType::LeftBankedQuarterTurn5TileUp25:
            return flying_rc_track_left_banked_quarter_turn_5_25_deg_up;
        case TrackElemType::RightBankedQuarterTurn5TileUp25:
            return flying_rc_track_right_banked_quarter_turn_5_25_deg_up;
        case TrackElemType::LeftBankedQuarterTurn5TileDown25:
            return flying_rc_track_left_banked_quarter_turn_5_25_deg_down;
        case TrackElemType::RightBankedQuarterTurn5TileDown25:
            return flying_rc_track_right_banked_quarter_turn_5_25_deg_down;
        case TrackElemType::Up25ToLeftBankedUp25:
            return flying_rc_track_25_deg_up_to_left_banked_25_deg_up;
        case TrackElemType::Up25ToRightBankedUp25:
            return flying_rc_track_25_deg_up_to_right_banked_25_deg_up;
        case TrackElemType::LeftBankedUp25ToUp25:
            return flying_rc_track_left_banked_25_deg_up_to_25_deg_up;
        case TrackElemType::RightBankedUp25ToUp25:
            return flying_rc_track_right_banked_25_deg_up_to_25_deg_up;
        case TrackElemType::Down25ToLeftBankedDown25:
            return flying_rc_track_25_deg_down_to_left_banked_25_deg_down;
        case TrackElemType::Down25ToRightBankedDown25:
            return flying_rc_track_25_deg_down_to_right_banked_25_deg_down;
        case TrackElemType::LeftBankedDown25ToDown25:
            return flying_rc_track_left_banked_25_deg_down_to_25_deg_down;
        case TrackElemType::RightBankedDown25ToDown25:
            return flying_rc_track_right_banked_25_deg_down_to_25_deg_down;
        case TrackElemType::LeftBankedFlatToLeftBankedUp25:
            return flying_rc_track_left_banked_flat_to_left_banked_25_deg_up;
        case TrackElemType::RightBankedFlatToRightBankedUp25:
            return flying_rc_track_right_banked_flat_to_right_banked_25_deg_up;
        case TrackElemType::LeftBankedUp25ToLeftBankedFlat:
            return flying_rc_track_left_banked_25_deg_up_to_left_banked_flat;
        case TrackElemType::RightBankedUp25ToRightBankedFlat:
            return flying_rc_track_right_banked_25_deg_up_to_right_banked_flat;
        case TrackElemType::LeftBankedFlatToLeftBankedDown25:
            return flying_rc_track_left_banked_flat_to_left_banked_25_deg_down;
        case TrackElemType::RightBankedFlatToRightBankedDown25:
            return flying_rc_track_right_banked_flat_to_right_banked_25_deg_down;
        case TrackElemType::LeftBankedDown25ToLeftBankedFlat:
            return flying_rc_track_left_banked_25_deg_down_to_left_banked_flat;
        case TrackElemType::RightBankedDown25ToRightBankedFlat:
            return flying_rc_track_right_banked_25_deg_down_to_right_banked_flat;
        case TrackElemType::FlatToLeftBankedUp25:
            return flying_rc_track_flat_to_left_banked_25_deg_up;
        case TrackElemType::FlatToRightBankedUp25:
            return flying_rc_track_flat_to_right_banked_25_deg_up;
        case TrackElemType::LeftBankedUp25ToFlat:
            return flying_rc_track_left_banked_25_deg_up_to_flat;
        case TrackElemType::RightBankedUp25ToFlat:
            return flying_rc_track_right_banked_25_deg_up_to_flat;
        case TrackElemType::FlatToLeftBankedDown25:
            return flying_rc_track_flat_to_left_banked_25_deg_down;
        case TrackElemType::FlatToRightBankedDown25:
            return flying_rc_track_flat_to_right_banked_25_deg_down;
        case TrackElemType::LeftBankedDown25ToFlat:
            return flying_rc_track_left_banked_25_deg_down_to_flat;
        case TrackElemType::RightBankedDown25ToFlat:
            return flying_rc_track_right_banked_25_deg_down_to_flat;
        case TrackElemType::Booster:
            return flying_rc_track_booster;
    }
    return nullptr;
}
