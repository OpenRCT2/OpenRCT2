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
#include "../../world/Sprite.h"
#include "../RideData.h"
#include "../TrackData.h"
#include "../TrackPaint.h"

/** rct2: 0x008A7AF8 */
static void corkscrew_rc_track_flat(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    if (tileElement->AsTrack()->HasChain())
    {
        switch (direction)
        {
            case 0:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 16226, 0, 0, 32, 20, 3, height, 0, 6, height);
                break;
            case 1:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 16227, 0, 0, 32, 20, 3, height, 0, 6, height);
                break;
            case 2:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 16228, 0, 0, 32, 20, 3, height, 0, 6, height);
                break;
            case 3:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 16229, 0, 0, 32, 20, 3, height, 0, 6, height);
                break;
        }
        if (track_paint_util_should_paint_supports(session->MapPosition))
        {
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
            case 2:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 16224, 0, 0, 32, 20, 3, height, 0, 6, height);
                break;
            case 1:
            case 3:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 16225, 0, 0, 32, 20, 3, height, 0, 6, height);
                break;
        }
        if (track_paint_util_should_paint_supports(session->MapPosition))
        {
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
        }
    }
    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
    paint_util_set_segment_support_height(
        session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x008A7D68, 0x008A7D78, 0x008A7D88 */
static void corkscrew_rc_track_station(
    paint_session* session, ride_id_t rideIndex, [[maybe_unused]] uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    static constexpr const uint32_t imageIds[4][3] = {
        { 16236, 16232, SPR_STATION_BASE_A_SW_NE },
        { 16237, 16233, SPR_STATION_BASE_A_NW_SE },
        { 16236, 16232, SPR_STATION_BASE_A_SW_NE },
        { 16237, 16233, SPR_STATION_BASE_A_NW_SE },
    };

    if (tileElement->AsTrack()->GetTrackType() == TrackElemType::EndStation)
    {
        sub_98197C_rotated(
            session, direction, imageIds[direction][1] | session->TrackColours[SCHEME_TRACK], 0, 0, 32, 20, 1, height, 0, 6,
            height + 3);
    }
    else
    {
        sub_98197C_rotated(
            session, direction, imageIds[direction][0] | session->TrackColours[SCHEME_TRACK], 0, 0, 32, 20, 1, height, 0, 6,
            height + 3);
    }
    sub_98196C_rotated(
        session, direction, imageIds[direction][2] | session->TrackColours[SCHEME_MISC], 0, 0, 32, 32, 1, height);
    track_paint_util_draw_station_metal_supports_2(session, direction, height, session->TrackColours[SCHEME_SUPPORTS], 0);
    track_paint_util_draw_station_2(session, rideIndex, direction, height, tileElement, 9, 11);
    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x008A7B08 */
static void corkscrew_rc_track_25_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    if (tileElement->AsTrack()->HasChain())
    {
        switch (direction)
        {
            case 0:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 16314, 0, 0, 32, 20, 3, height, 0, 6, height);
                break;
            case 1:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 16315, 0, 0, 32, 20, 3, height, 0, 6, height);
                break;
            case 2:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 16316, 0, 0, 32, 20, 3, height, 0, 6, height);
                break;
            case 3:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 16317, 0, 0, 32, 20, 3, height, 0, 6, height);
                break;
        }
        if (track_paint_util_should_paint_supports(session->MapPosition))
        {
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 8, height, session->TrackColours[SCHEME_SUPPORTS]);
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 16286, 0, 0, 32, 20, 3, height, 0, 6, height);
                break;
            case 1:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 16287, 0, 0, 32, 20, 3, height, 0, 6, height);
                break;
            case 2:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 16288, 0, 0, 32, 20, 3, height, 0, 6, height);
                break;
            case 3:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 16289, 0, 0, 32, 20, 3, height, 0, 6, height);
                break;
        }
        if (track_paint_util_should_paint_supports(session->MapPosition))
        {
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 8, height, session->TrackColours[SCHEME_SUPPORTS]);
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

/** rct2: 0x008A7B18 */
static void corkscrew_rc_track_60_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    if (tileElement->AsTrack()->HasChain())
    {
        switch (direction)
        {
            case 0:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 16330, 0, 0, 32, 20, 3, height, 0, 6, height);
                break;
            case 1:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 16331, 0, 0, 32, 1, 98, height, 0, 27, height);
                break;
            case 2:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 16332, 0, 0, 32, 1, 98, height, 0, 27, height);
                break;
            case 3:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 16333, 0, 0, 32, 20, 3, height, 0, 6, height);
                break;
        }
        if (track_paint_util_should_paint_supports(session->MapPosition))
        {
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 32, height, session->TrackColours[SCHEME_SUPPORTS]);
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 16302, 0, 0, 32, 20, 3, height, 0, 6, height);
                break;
            case 1:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 16303, 0, 0, 32, 1, 98, height, 0, 27, height);
                break;
            case 2:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 16304, 0, 0, 32, 1, 98, height, 0, 27, height);
                break;
            case 3:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 16305, 0, 0, 32, 20, 3, height, 0, 6, height);
                break;
        }
        if (track_paint_util_should_paint_supports(session->MapPosition))
        {
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 32, height, session->TrackColours[SCHEME_SUPPORTS]);
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

/** rct2: 0x008A7B28 */
static void corkscrew_rc_track_flat_to_25_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    if (tileElement->AsTrack()->HasChain())
    {
        switch (direction)
        {
            case 0:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 16306, 0, 0, 32, 20, 3, height, 0, 6, height);
                break;
            case 1:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 16307, 0, 0, 32, 20, 3, height, 0, 6, height);
                break;
            case 2:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 16308, 0, 0, 32, 20, 3, height, 0, 6, height);
                break;
            case 3:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 16309, 0, 0, 32, 20, 3, height, 0, 6, height);
                break;
        }
        if (track_paint_util_should_paint_supports(session->MapPosition))
        {
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 3, height, session->TrackColours[SCHEME_SUPPORTS]);
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 16278, 0, 0, 32, 20, 3, height, 0, 6, height);
                break;
            case 1:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 16279, 0, 0, 32, 20, 3, height, 0, 6, height);
                break;
            case 2:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 16280, 0, 0, 32, 20, 3, height, 0, 6, height);
                break;
            case 3:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 16281, 0, 0, 32, 20, 3, height, 0, 6, height);
                break;
        }
        if (track_paint_util_should_paint_supports(session->MapPosition))
        {
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 3, height, session->TrackColours[SCHEME_SUPPORTS]);
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

/** rct2: 0x008A7B38 */
static void corkscrew_rc_track_25_deg_up_to_60_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    if (tileElement->AsTrack()->HasChain())
    {
        switch (direction)
        {
            case 0:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 16318, 0, 0, 32, 20, 3, height, 0, 6, height);
                break;
            case 1:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 16319, 0, 0, 32, 20, 3, height, 0, 6, height);
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 16322, 0, 0, 32, 1, 66, height, 0, 27, height);
                break;
            case 2:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 16320, 0, 0, 32, 20, 3, height, 0, 6, height);
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 16323, 0, 0, 32, 1, 66, height, 0, 27, height);
                break;
            case 3:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 16321, 0, 0, 32, 20, 3, height, 0, 6, height);
                break;
        }
        if (track_paint_util_should_paint_supports(session->MapPosition))
        {
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 12, height, session->TrackColours[SCHEME_SUPPORTS]);
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 16290, 0, 0, 32, 20, 3, height, 0, 6, height);
                break;
            case 1:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 16291, 0, 0, 32, 20, 3, height, 0, 6, height);
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 16294, 0, 0, 32, 1, 66, height, 0, 27, height);
                break;
            case 2:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 16292, 0, 0, 32, 20, 3, height, 0, 6, height);
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 16295, 0, 0, 32, 1, 66, height, 0, 27, height);
                break;
            case 3:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 16293, 0, 0, 32, 20, 3, height, 0, 6, height);
                break;
        }
        if (track_paint_util_should_paint_supports(session->MapPosition))
        {
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 12, height, session->TrackColours[SCHEME_SUPPORTS]);
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

/** rct2: 0x008A7B48 */
static void corkscrew_rc_track_60_deg_up_to_25_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    if (tileElement->AsTrack()->HasChain())
    {
        switch (direction)
        {
            case 0:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 16324, 0, 0, 32, 20, 3, height, 0, 6, height);
                break;
            case 1:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 16325, 0, 0, 32, 20, 3, height, 0, 6, height);
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 16328, 0, 0, 32, 1, 66, height, 0, 27, height);
                break;
            case 2:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 16326, 0, 0, 32, 20, 3, height, 0, 6, height);
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 16329, 0, 0, 32, 1, 66, height, 0, 27, height);
                break;
            case 3:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 16327, 0, 0, 32, 20, 3, height, 0, 6, height);
                break;
        }
        if (track_paint_util_should_paint_supports(session->MapPosition))
        {
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 20, height, session->TrackColours[SCHEME_SUPPORTS]);
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 16296, 0, 0, 32, 20, 3, height, 0, 6, height);
                break;
            case 1:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 16297, 0, 0, 32, 20, 3, height, 0, 6, height);
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 16300, 0, 0, 32, 1, 66, height, 0, 27, height);
                break;
            case 2:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 16298, 0, 0, 32, 20, 3, height, 0, 6, height);
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 16301, 0, 0, 32, 1, 66, height, 0, 27, height);
                break;
            case 3:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 16299, 0, 0, 32, 20, 3, height, 0, 6, height);
                break;
        }
        if (track_paint_util_should_paint_supports(session->MapPosition))
        {
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 20, height, session->TrackColours[SCHEME_SUPPORTS]);
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

/** rct2: 0x008A7B58 */
static void corkscrew_rc_track_25_deg_up_to_flat(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    if (tileElement->AsTrack()->HasChain())
    {
        switch (direction)
        {
            case 0:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 16310, 0, 0, 32, 20, 3, height, 0, 6, height);
                break;
            case 1:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 16311, 0, 0, 32, 20, 3, height, 0, 6, height);
                break;
            case 2:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 16312, 0, 0, 32, 20, 3, height, 0, 6, height);
                break;
            case 3:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 16313, 0, 0, 32, 20, 3, height, 0, 6, height);
                break;
        }
        if (track_paint_util_should_paint_supports(session->MapPosition))
        {
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 6, height, session->TrackColours[SCHEME_SUPPORTS]);
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 16282, 0, 0, 32, 20, 3, height, 0, 6, height);
                break;
            case 1:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 16283, 0, 0, 32, 20, 3, height, 0, 6, height);
                break;
            case 2:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 16284, 0, 0, 32, 20, 3, height, 0, 6, height);
                break;
            case 3:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 16285, 0, 0, 32, 20, 3, height, 0, 6, height);
                break;
        }
        if (track_paint_util_should_paint_supports(session->MapPosition))
        {
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 6, height, session->TrackColours[SCHEME_SUPPORTS]);
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

/** rct2: 0x008A7B68 */
static void corkscrew_rc_track_25_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    corkscrew_rc_track_25_deg_up(session, rideIndex, trackSequence, (direction + 2) & 3, height, tileElement);
}

/** rct2: 0x008A7B78 */
static void corkscrew_rc_track_60_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    corkscrew_rc_track_60_deg_up(session, rideIndex, trackSequence, (direction + 2) & 3, height, tileElement);
}

/** rct2: 0x008A7B88 */
static void corkscrew_rc_track_flat_to_25_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    corkscrew_rc_track_25_deg_up_to_flat(session, rideIndex, trackSequence, (direction + 2) & 3, height, tileElement);
}

/** rct2: 0x008A7B98 */
static void corkscrew_rc_track_25_deg_down_to_60_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    corkscrew_rc_track_60_deg_up_to_25_deg_up(session, rideIndex, trackSequence, (direction + 2) & 3, height, tileElement);
}

/** rct2: 0x008A7BA8 */
static void corkscrew_rc_track_60_deg_down_to_25_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    corkscrew_rc_track_25_deg_up_to_60_deg_up(session, rideIndex, trackSequence, (direction + 2) & 3, height, tileElement);
}

/** rct2: 0x008A7BB8 */
static void corkscrew_rc_track_25_deg_down_to_flat(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    corkscrew_rc_track_flat_to_25_deg_up(session, rideIndex, trackSequence, (direction + 2) & 3, height, tileElement);
}

/** rct2: 0x008A7BC8 */
static void corkscrew_rc_track_left_quarter_turn_5(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16343, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16348, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16353, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16338, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
            }
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 1:
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16342, 0, 0, 32, 16, 3, height, 0, 0, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16347, 0, 0, 32, 16, 3, height, 0, 0, height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16352, 0, 0, 32, 16, 3, height, 0, 16,
                        height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16337, 0, 0, 32, 16, 3, height, 0, 16,
                        height);
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
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16341, 0, 0, 16, 16, 3, height, 0, 16,
                        height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16346, 0, 0, 16, 16, 3, height, 16, 16,
                        height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16351, 0, 0, 16, 16, 3, height, 16, 0,
                        height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16336, 0, 0, 16, 16, 3, height, 0, 0, height);
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
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16340, 0, 0, 16, 32, 3, height, 16, 0,
                        height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16345, 0, 0, 16, 32, 3, height, 0, 0, height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16350, 0, 0, 16, 32, 3, height, 0, 0, height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16335, 0, 0, 16, 32, 3, height, 16, 0,
                        height);
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
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16339, 0, 0, 20, 32, 3, height, 6, 0, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16344, 0, 0, 20, 32, 3, height, 6, 0, height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16349, 0, 0, 20, 32, 3, height, 6, 0, height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16334, 0, 0, 20, 32, 3, height, 6, 0, height);
                    break;
            }
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
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
                session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008A7BD8 */
static void corkscrew_rc_track_right_quarter_turn_5(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    corkscrew_rc_track_left_quarter_turn_5(session, rideIndex, trackSequence, (direction - 1) & 3, height, tileElement);
}

/** rct2: 0x008A7BE8 */
static void corkscrew_rc_track_flat_to_left_bank(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (direction)
    {
        case 0:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 16238, 0, 0, 32, 20, 3, height, 0, 6, height);
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 16246, 0, 0, 32, 1, 26, height, 0, 27, height);
            break;
        case 1:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 16239, 0, 0, 32, 20, 3, height, 0, 6, height);
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 16247, 0, 0, 32, 1, 26, height, 0, 27, height);
            break;
        case 2:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 16240, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 3:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 16241, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
    }
    if (track_paint_util_should_paint_supports(session->MapPosition))
    {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
    }
    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
    paint_util_set_segment_support_height(
        session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x008A7BF8 */
static void corkscrew_rc_track_flat_to_right_bank(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (direction)
    {
        case 0:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 16242, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 1:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 16243, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 2:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 16244, 0, 0, 32, 20, 3, height, 0, 6, height);
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 16248, 0, 0, 32, 1, 26, height, 0, 27, height);
            break;
        case 3:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 16245, 0, 0, 32, 20, 3, height, 0, 6, height);
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 16249, 0, 0, 32, 1, 26, height, 0, 27, height);
            break;
    }
    if (track_paint_util_should_paint_supports(session->MapPosition))
    {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
    }
    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
    paint_util_set_segment_support_height(
        session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x008A7C08 */
static void corkscrew_rc_track_left_bank_to_flat(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (direction)
    {
        case 0:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 16244, 0, 0, 32, 20, 3, height, 0, 6, height);
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 16248, 0, 0, 32, 1, 26, height, 0, 27, height);
            break;
        case 1:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 16245, 0, 0, 32, 20, 3, height, 0, 6, height);
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 16249, 0, 0, 32, 1, 26, height, 0, 27, height);
            break;
        case 2:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 16242, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 3:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 16243, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
    }
    if (track_paint_util_should_paint_supports(session->MapPosition))
    {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
    }
    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
    paint_util_set_segment_support_height(
        session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x008A7C18 */
static void corkscrew_rc_track_right_bank_to_flat(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (direction)
    {
        case 0:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 16240, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 1:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 16241, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 2:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 16238, 0, 0, 32, 20, 3, height, 0, 6, height);
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 16246, 0, 0, 32, 1, 26, height, 0, 27, height);
            break;
        case 3:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 16239, 0, 0, 32, 20, 3, height, 0, 6, height);
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 16247, 0, 0, 32, 1, 26, height, 0, 27, height);
            break;
    }
    if (track_paint_util_should_paint_supports(session->MapPosition))
    {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
    }
    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
    paint_util_set_segment_support_height(
        session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x008A7C28 */
static void corkscrew_rc_track_banked_left_quarter_turn_5(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16363, 0, 0, 32, 20, 3, height, 0, 6, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16374, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16368, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16373, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16358, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
            }
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 1:
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16362, 0, 0, 32, 16, 3, height, 0, 0, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16367, 0, 0, 32, 16, 1, height, 0, 0,
                        height + 27);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16372, 0, 0, 32, 16, 3, height, 0, 16,
                        height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16357, 0, 0, 32, 16, 3, height, 0, 16,
                        height);
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
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16361, 0, 0, 16, 16, 3, height, 0, 16,
                        height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16366, 0, 0, 16, 16, 1, height, 16, 16,
                        height + 27);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16371, 0, 0, 16, 16, 3, height, 16, 0,
                        height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16356, 0, 0, 16, 16, 3, height, 0, 0, height);
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
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16360, 0, 0, 16, 32, 3, height, 16, 0,
                        height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16365, 0, 0, 16, 32, 1, height, 0, 0,
                        height + 27);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16370, 0, 0, 16, 32, 3, height, 0, 0, height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16355, 0, 0, 16, 32, 3, height, 16, 0,
                        height);
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
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16359, 0, 0, 20, 32, 3, height, 6, 0, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16364, 0, 0, 1, 32, 26, height, 27, 0,
                        height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16369, 0, 0, 20, 32, 3, height, 6, 0, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16375, 0, 0, 1, 32, 26, height, 27, 0,
                        height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16354, 0, 0, 20, 32, 3, height, 6, 0, height);
                    break;
            }
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
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
                session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008A7C38 */
static void corkscrew_rc_track_banked_right_quarter_turn_5(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    corkscrew_rc_track_banked_left_quarter_turn_5(session, rideIndex, trackSequence, (direction - 1) & 3, height, tileElement);
}

/** rct2: 0x008A7C48 */
static void corkscrew_rc_track_left_bank_to_25_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (direction)
    {
        case 0:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 16250, 0, 0, 32, 20, 3, height, 0, 6, height);
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 16254, 0, 0, 32, 1, 34, height, 0, 27, height);
            break;
        case 1:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 16251, 0, 0, 32, 20, 3, height, 0, 6, height);
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 16255, 0, 0, 32, 1, 34, height, 0, 27, height);
            break;
        case 2:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 16252, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 3:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 16253, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
    }
    if (track_paint_util_should_paint_supports(session->MapPosition))
    {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 3, height, session->TrackColours[SCHEME_SUPPORTS]);
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

/** rct2: 0x008A7C58 */
static void corkscrew_rc_track_right_bank_to_25_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (direction)
    {
        case 0:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 16256, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 1:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 16257, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 2:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 16258, 0, 0, 32, 20, 3, height, 0, 6, height);
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 16260, 0, 0, 32, 1, 34, height, 0, 27, height);
            break;
        case 3:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 16259, 0, 0, 32, 20, 3, height, 0, 6, height);
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 16261, 0, 0, 32, 1, 34, height, 0, 27, height);
            break;
    }
    if (track_paint_util_should_paint_supports(session->MapPosition))
    {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 3, height, session->TrackColours[SCHEME_SUPPORTS]);
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

/** rct2: 0x008A7C68 */
static void corkscrew_rc_track_25_deg_up_to_left_bank(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (direction)
    {
        case 0:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 16262, 0, 0, 32, 20, 3, height, 0, 6, height);
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 16266, 0, 0, 32, 1, 34, height, 0, 27, height);
            break;
        case 1:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 16263, 0, 0, 32, 20, 3, height, 0, 6, height);
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 16267, 0, 0, 32, 1, 34, height, 0, 27, height);
            break;
        case 2:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 16264, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 3:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 16265, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
    }
    if (track_paint_util_should_paint_supports(session->MapPosition))
    {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 6, height, session->TrackColours[SCHEME_SUPPORTS]);
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

/** rct2: 0x008A7C78 */
static void corkscrew_rc_track_25_deg_up_to_right_bank(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (direction)
    {
        case 0:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 16268, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 1:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 16269, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 2:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 16270, 0, 0, 32, 20, 3, height, 0, 6, height);
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 16272, 0, 0, 32, 1, 34, height, 0, 27, height);
            break;
        case 3:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 16271, 0, 0, 32, 20, 3, height, 0, 6, height);
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 16273, 0, 0, 32, 1, 34, height, 0, 27, height);
            break;
    }
    if (track_paint_util_should_paint_supports(session->MapPosition))
    {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 6, height, session->TrackColours[SCHEME_SUPPORTS]);
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

/** rct2: 0x008A7C88 */
static void corkscrew_rc_track_left_bank_to_25_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    corkscrew_rc_track_25_deg_up_to_right_bank(session, rideIndex, trackSequence, (direction + 2) & 3, height, tileElement);
}

/** rct2: 0x008A7C98 */
static void corkscrew_rc_track_right_bank_to_25_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    corkscrew_rc_track_25_deg_up_to_left_bank(session, rideIndex, trackSequence, (direction + 2) & 3, height, tileElement);
}

/** rct2: 0x008A7CA8 */
static void corkscrew_rc_track_25_deg_down_to_left_bank(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    corkscrew_rc_track_right_bank_to_25_deg_up(session, rideIndex, trackSequence, (direction + 2) & 3, height, tileElement);
}

/** rct2: 0x008A7CB8 */
static void corkscrew_rc_track_25_deg_down_to_right_bank(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    corkscrew_rc_track_left_bank_to_25_deg_up(session, rideIndex, trackSequence, (direction + 2) & 3, height, tileElement);
}

/** rct2: 0x008A7CC8 */
static void corkscrew_rc_track_left_bank(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (direction)
    {
        case 0:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 16274, 0, 0, 32, 1, 26, height, 0, 27, height);
            break;
        case 1:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 16275, 0, 0, 32, 1, 26, height, 0, 27, height);
            break;
        case 2:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 16276, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 3:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 16277, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
    }
    if (track_paint_util_should_paint_supports(session->MapPosition))
    {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
    }
    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
    paint_util_set_segment_support_height(
        session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x008A7CD8 */
static void corkscrew_rc_track_right_bank(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    corkscrew_rc_track_left_bank(session, rideIndex, trackSequence, (direction + 2) & 3, height, tileElement);
}

/** rct2: 0x008A7CE8 */
static void corkscrew_rc_track_left_quarter_turn_5_25_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16510, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16515, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16520, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16525, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
            }
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 8, height, session->TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_1);
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 72, 0x20);
            break;
        case 1:
            paint_util_set_general_support_height(session, height + 72, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16511, 0, 0, 32, 16, 3, height);
                    break;
                case 1:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16516, 0, 0, 32, 16, 3, height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16521, 0, 0, 32, 16, 3, height, 0, 16,
                        height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16526, 0, 0, 32, 16, 3, height, 0, 16,
                        height);
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
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16512, 0, 0, 16, 16, 3, height, 0, 16,
                        height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16517, 0, 0, 16, 16, 3, height, 16, 16,
                        height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16522, 0, 0, 16, 16, 3, height, 16, 0,
                        height);
                    break;
                case 3:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16527, 0, 0, 16, 16, 3, height);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 64, 0x20);
            break;
        case 4:
            paint_util_set_general_support_height(session, height + 72, 0x20);
            break;
        case 5:
            switch (direction)
            {
                case 0:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16513, 0, 0, 16, 32, 3, height, 16, 0,
                        height);
                    break;
                case 1:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16518, 0, 0, 16, 32, 3, height);
                    break;
                case 2:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16523, 0, 0, 16, 32, 3, height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16528, 0, 0, 16, 32, 3, height, 16, 0,
                        height);
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
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16514, 0, 0, 20, 32, 3, height, 6, 0, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16519, 0, 0, 20, 32, 3, height, 6, 0, height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16524, 0, 0, 20, 32, 3, height, 6, 0, height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16529, 0, 0, 20, 32, 3, height, 6, 0, height);
                    break;
            }
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 8, height, session->TrackColours[SCHEME_SUPPORTS]);
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
                session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 72, 0x20);
            break;
    }
}

/** rct2: 0x008A7CF8 */
static void corkscrew_rc_track_right_quarter_turn_5_25_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16490, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16495, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16500, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16505, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
            }
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 8, height, session->TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_1);
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 72, 0x20);
            break;
        case 1:
            paint_util_set_general_support_height(session, height + 72, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16491, 0, 0, 32, 16, 3, height, 0, 16,
                        height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16496, 0, 0, 32, 16, 3, height, 0, 16,
                        height);
                    break;
                case 2:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16501, 0, 0, 32, 16, 3, height);
                    break;
                case 3:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16506, 0, 0, 32, 16, 3, height);
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
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16492, 0, 0, 16, 16, 3, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16497, 0, 0, 16, 16, 3, height, 16, 0,
                        height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16502, 0, 0, 16, 16, 3, height, 16, 16,
                        height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16507, 0, 0, 16, 16, 3, height, 0, 16,
                        height);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 64, 0x20);
            break;
        case 4:
            paint_util_set_general_support_height(session, height + 72, 0x20);
            break;
        case 5:
            switch (direction)
            {
                case 0:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16493, 0, 0, 16, 32, 3, height, 16, 0,
                        height);
                    break;
                case 1:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16498, 0, 0, 16, 32, 3, height);
                    break;
                case 2:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16503, 0, 0, 16, 32, 3, height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16508, 0, 0, 16, 32, 3, height, 16, 0,
                        height);
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
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16494, 0, 0, 20, 32, 3, height, 6, 0, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16499, 0, 0, 20, 32, 3, height, 6, 0, height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16504, 0, 0, 20, 32, 3, height, 6, 0, height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16509, 0, 0, 20, 32, 3, height, 6, 0, height);
                    break;
            }
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 8, height, session->TrackColours[SCHEME_SUPPORTS]);
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
                session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 72, 0x20);
            break;
    }
}

/** rct2: 0x008A7D08 */
static void corkscrew_rc_track_left_quarter_turn_5_25_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    corkscrew_rc_track_right_quarter_turn_5_25_deg_up(
        session, rideIndex, trackSequence, (direction + 1) & 3, height, tileElement);
}

/** rct2: 0x008A7D18 */
static void corkscrew_rc_track_right_quarter_turn_5_25_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    corkscrew_rc_track_left_quarter_turn_5_25_deg_up(
        session, rideIndex, trackSequence, (direction - 1) & 3, height, tileElement);
}

/** rct2: 0x008A7D28 */
static void corkscrew_rc_track_s_bend_left(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16402, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16406, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16405, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16409, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
            }
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16403, 0, 0, 32, 26, 3, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16407, 0, 0, 32, 26, 3, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 6, 1, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16404, 0, 0, 32, 26, 3, height, 0, 6, height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16408, 0, 0, 32, 26, 3, height, 0, 6, height);
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
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16404, 0, 0, 32, 26, 3, height, 0, 6, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16408, 0, 0, 32, 26, 3, height, 0, 6, height);
                    break;
                case 2:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16403, 0, 0, 32, 26, 3, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16407, 0, 0, 32, 26, 3, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 6, 1, height, session->TrackColours[SCHEME_SUPPORTS]);
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
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16405, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16409, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16402, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16406, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
            }
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
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
                session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008A7D38 */
static void corkscrew_rc_track_s_bend_right(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16410, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16414, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16413, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16417, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
            }
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16411, 0, 0, 32, 26, 3, height, 0, 6, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 8, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16415, 0, 0, 32, 26, 3, height, 0, 6, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 7, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16412, 0, 0, 32, 26, 3, height);
                    break;
                case 3:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16416, 0, 0, 32, 26, 3, height);
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
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16412, 0, 0, 32, 26, 3, height);
                    break;
                case 1:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16416, 0, 0, 32, 26, 3, height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16411, 0, 0, 32, 26, 3, height, 0, 6, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 8, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16415, 0, 0, 32, 26, 3, height, 0, 6, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 7, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
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
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16413, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16417, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16410, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16414, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
            }
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
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
                session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008A7D48 */
static void corkscrew_rc_track_left_vertical_loop(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16562, 0, 6, 32, 20, 3, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 4, 8, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16570, 0, 6, 32, 20, 3, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 4, 8, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16569, 0, 6, 32, 20, 3, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 4, 8, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16577, 0, 6, 32, 20, 7, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 4, 6, height, session->TrackColours[SCHEME_SUPPORTS]);
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
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16563, 0, 0, 32, 26, 3, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 4, 20, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16571, 0, 14, 32, 2, 63, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 4, 15, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16568, 0, 6, 32, 26, 3, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 4, 16, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16576, 0, 6, 32, 26, 3, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 4, 10, height, session->TrackColours[SCHEME_SUPPORTS]);
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
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16564, 16, 0, 3, 16, 119, height, 16, 0,
                        height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16572, 12, 0, 3, 16, 119, height, 12, 0,
                        height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16567, 10, 16, 4, 16, 119, height, 10, 16,
                        height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16575, 16, 16, 2, 16, 119, height, 16, 16,
                        height);
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
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16565, 0, 0, 32, 16, 3, height + 32);
                    break;
                case 1:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16573, 0, 0, 32, 16, 3, height + 32);
                    break;
                case 2:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16566, 0, 16, 32, 16, 3, height + 32);
                    break;
                case 3:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16574, 0, 16, 32, 16, 3, height + 32);
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
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16566, 0, 16, 32, 16, 3, height + 32);
                    break;
                case 1:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16574, 0, 16, 32, 16, 3, height + 32);
                    break;
                case 2:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16565, 0, 0, 32, 16, 3, height + 32);
                    break;
                case 3:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16573, 0, 0, 32, 16, 3, height + 32);
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
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16567, 10, 16, 4, 16, 119, height, 10, 16,
                        height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16575, 16, 16, 2, 16, 119, height, 16, 16,
                        height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16564, 16, 0, 3, 16, 119, height, 16, 0,
                        height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16572, 12, 0, 3, 16, 119, height, 12, 0,
                        height);
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
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16568, 0, 6, 32, 26, 3, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 4, 16, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16576, 0, 6, 32, 26, 3, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 4, 10, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16563, 0, 0, 32, 26, 3, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 4, 20, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16571, 0, 14, 32, 2, 63, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 4, 15, height, session->TrackColours[SCHEME_SUPPORTS]);
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
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16569, 0, 6, 32, 20, 3, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 4, 8, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16577, 0, 6, 32, 20, 7, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 4, 6, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16562, 0, 6, 32, 20, 3, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 4, 8, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16570, 0, 6, 32, 20, 3, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 4, 8, height, session->TrackColours[SCHEME_SUPPORTS]);
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

/** rct2: 0x008A7D58 */
static void corkscrew_rc_track_right_vertical_loop(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16593, 0, 6, 32, 20, 7, height);
                    break;
                case 1:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16585, 0, 6, 32, 20, 3, height);
                    break;
                case 2:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16586, 0, 6, 32, 20, 3, height);
                    break;
                case 3:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16578, 0, 6, 32, 20, 3, height);
                    break;
            }
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 8, height, session->TrackColours[SCHEME_SUPPORTS]);
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
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16592, 0, 6, 32, 26, 3, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 4, 8, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16584, 0, 6, 32, 26, 3, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 4, 16, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16587, 0, 14, 32, 2, 63, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 4, 12, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16579, 0, 0, 32, 26, 3, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 4, 20, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            paint_util_set_general_support_height(session, height + 72, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16591, 16, 16, 2, 16, 119, height, 16, 16,
                        height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16583, 10, 16, 4, 16, 119, height, 10, 16,
                        height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16588, 12, 0, 3, 16, 119, height, 12, 0,
                        height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16580, 16, 0, 2, 16, 119, height, 16, 0,
                        height);
                    break;
            }
            paint_util_set_general_support_height(session, height + 168, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16590, 0, 16, 32, 16, 3, height + 32);
                    break;
                case 1:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16582, 0, 16, 32, 16, 3, height + 32);
                    break;
                case 2:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16589, 0, 0, 32, 16, 3, height + 32);
                    break;
                case 3:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16581, 0, 0, 32, 16, 3, height + 32);
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
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16589, 0, 0, 32, 16, 3, height + 32);
                    break;
                case 1:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16581, 0, 0, 32, 16, 3, height + 32);
                    break;
                case 2:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16590, 0, 16, 32, 16, 3, height + 32);
                    break;
                case 3:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16582, 0, 16, 32, 16, 3, height + 32);
                    break;
            }
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
        case 7:
            switch (direction)
            {
                case 0:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16588, 12, 0, 3, 16, 119, height, 12, 0,
                        height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16580, 16, 0, 2, 16, 119, height, 16, 0,
                        height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16591, 16, 16, 2, 16, 119, height, 16, 16,
                        height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16583, 10, 16, 4, 16, 119, height, 10, 16,
                        height);
                    break;
            }
            paint_util_set_general_support_height(session, height + 168, 0x20);
            break;
        case 8:
            switch (direction)
            {
                case 0:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16587, 0, 14, 32, 2, 63, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 4, 12, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16579, 0, 0, 32, 26, 3, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 4, 20, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16592, 0, 6, 32, 26, 3, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 4, 8, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16584, 0, 6, 32, 26, 3, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 4, 16, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            paint_util_set_general_support_height(session, height + 72, 0x20);
            break;
        case 9:
            switch (direction)
            {
                case 0:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16586, 0, 6, 32, 20, 3, height);
                    break;
                case 1:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16578, 0, 6, 32, 20, 3, height);
                    break;
                case 2:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16593, 0, 6, 32, 20, 7, height);
                    break;
                case 3:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16585, 0, 6, 32, 20, 3, height);
                    break;
            }
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 8, height, session->TrackColours[SCHEME_SUPPORTS]);
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

/** rct2: 0x008A7D98 */
static void corkscrew_rc_track_left_quarter_turn_3(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16381, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16384, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16387, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16378, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
            }
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 1:
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16380, 0, 0, 16, 16, 3, height, 16, 0,
                        height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16383, 0, 0, 16, 16, 3, height, 0, 0, height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16386, 0, 0, 16, 16, 3, height, 0, 16,
                        height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16377, 0, 0, 16, 16, 3, height, 16, 16,
                        height);
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
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16379, 0, 0, 20, 32, 3, height, 6, 0, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16382, 0, 0, 20, 32, 3, height, 6, 0, height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16385, 0, 0, 20, 32, 3, height, 6, 0, height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16376, 0, 0, 20, 32, 3, height, 6, 0, height);
                    break;
            }
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
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
                session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008A7DA8 */
static void corkscrew_rc_track_right_quarter_turn_3(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    corkscrew_rc_track_left_quarter_turn_3(session, rideIndex, trackSequence, (direction - 1) & 3, height, tileElement);
}

/** rct2: 0x008A7DB8 */
static void corkscrew_rc_track_left_quarter_turn_3_bank(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16393, 0, 0, 32, 20, 3, height, 0, 6, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16400, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16396, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16399, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16390, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
            }
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 1:
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16392, 0, 0, 16, 16, 3, height, 16, 0,
                        height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16395, 0, 0, 16, 16, 1, height, 0, 0,
                        height + 27);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16398, 0, 0, 16, 16, 3, height, 0, 16,
                        height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16389, 0, 0, 16, 16, 3, height, 16, 16,
                        height);
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
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16391, 0, 0, 20, 32, 3, height, 6, 0, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16394, 0, 0, 1, 32, 26, height, 27, 0,
                        height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16397, 0, 0, 20, 32, 3, height, 6, 0, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16401, 0, 0, 1, 32, 26, height, 27, 0,
                        height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16388, 0, 0, 20, 32, 3, height, 6, 0, height);
                    break;
            }
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
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
                session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008A7DC8 */
static void corkscrew_rc_track_right_quarter_turn_3_bank(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    corkscrew_rc_track_left_quarter_turn_3_bank(session, rideIndex, trackSequence, (direction - 1) & 3, height, tileElement);
}

/** rct2: 0x008A7DD8 */
static void corkscrew_rc_track_left_quarter_turn_3_25_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16541, 0, 6, 32, 20, 3, height);
                    break;
                case 1:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16543, 0, 6, 32, 20, 3, height);
                    break;
                case 2:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16545, 0, 6, 32, 20, 3, height);
                    break;
                case 3:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16539, 0, 6, 32, 20, 3, height);
                    break;
            }
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 8, height, session->TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_1);
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
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
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16540, 6, 0, 20, 32, 3, height);
                    break;
                case 1:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16542, 6, 0, 20, 32, 3, height);
                    break;
                case 2:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16544, 6, 0, 20, 32, 3, height);
                    break;
                case 3:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16538, 6, 0, 20, 32, 3, height);
                    break;
            }
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 8, height, session->TrackColours[SCHEME_SUPPORTS]);
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
                session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 72, 0x20);
            break;
    }
}

/** rct2: 0x008A7DE8 */
static void corkscrew_rc_track_right_quarter_turn_3_25_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16530, 0, 6, 32, 20, 3, height);
                    break;
                case 1:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16532, 0, 6, 32, 20, 3, height);
                    break;
                case 2:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16534, 0, 6, 32, 20, 3, height);
                    break;
                case 3:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16536, 0, 6, 32, 20, 3, height);
                    break;
            }
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 8, height, session->TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_1);
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
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
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16531, 6, 0, 20, 32, 3, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 4, 8, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16533, 6, 0, 20, 32, 3, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 4, 8, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16535, 6, 0, 20, 32, 3, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 4, 10, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16537, 6, 0, 20, 32, 3, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 4, 8, height, session->TrackColours[SCHEME_SUPPORTS]);
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
                session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 72, 0x20);
            break;
    }
}

/** rct2: 0x008A7DF8 */
static void corkscrew_rc_track_left_quarter_turn_3_25_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    corkscrew_rc_track_right_quarter_turn_3_25_deg_up(
        session, rideIndex, trackSequence, (direction + 1) & 3, height, tileElement);
}

/** rct2: 0x008A7E08 */
static void corkscrew_rc_track_right_quarter_turn_3_25_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    corkscrew_rc_track_left_quarter_turn_3_25_deg_up(
        session, rideIndex, trackSequence, (direction - 1) & 3, height, tileElement);
}

/** rct2: 0x008A7E18 */
static void corkscrew_rc_track_half_loop_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16594, 0, 6, 32, 20, 3, height);
                    break;
                case 1:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16602, 0, 6, 32, 20, 11, height);
                    break;
                case 2:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16601, 0, 6, 32, 20, 9, height);
                    break;
                case 3:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16609, 0, 6, 32, 20, 7, height);
                    break;
            }
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 8, height, session->TrackColours[SCHEME_SUPPORTS]);
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
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16595, 0, 0, 32, 20, 3, height, 0, 6, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 4, 20, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16603, 0, 14, 3, 20, 63, height, 28, 6,
                        height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 4, 15, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16600, 0, 6, 3, 20, 63, height, 28, 6,
                        height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 4, 16, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16608, 0, 6, 32, 20, 3, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 4, 16, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 72, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16596, 16, 0, 2, 16, 119, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16604, 12, 0, 3, 16, 119, height, 12, 0,
                        height);
                    break;
                case 2:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16599, 10, 16, 4, 12, 119, height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16607, 16, 16, 2, 16, 119, height, 15, 6,
                        height);
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
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16597, 0, 0, 32, 16, 3, height + 32);
                    break;
                case 1:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16605, 0, 0, 32, 16, 3, height + 32);
                    break;
                case 2:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16598, 0, 16, 32, 12, 3, height + 32);
                    break;
                case 3:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16606, 0, 16, 32, 12, 3, height + 32);
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

/** rct2: 0x008A7E28 */
static void corkscrew_rc_track_half_loop_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    corkscrew_rc_track_half_loop_up(session, rideIndex, 3 - trackSequence, direction, height, tileElement);
}

/** rct2: 0x008A7E38 */
static void corkscrew_rc_track_left_corkscrew_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16610, 0, 0, 32, 20, 3, height, 0, 6,
                        height + 4);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16613, 0, 0, 32, 20, 3, height, 0, 6,
                        height + 4);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16616, 0, 0, 32, 20, 3, height, 0, 6,
                        height + 4);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16619, 0, 0, 32, 20, 3, height, 0, 6,
                        height + 4);
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
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16611, 0, 0, 20, 20, 3, height, 6, 6,
                        height + 10);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16614, 0, 0, 20, 20, 3, height, 6, 6,
                        height + 10);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16617, 0, 0, 20, 20, 3, height, 6, 6,
                        height + 10);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16620, 0, 0, 20, 20, 3, height, 6, 6,
                        height + 10);
                    break;
            }
            paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 72, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16612, 0, 0, 20, 32, 3, height, 6, 0,
                        height + 24);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16615, 0, 0, 20, 32, 3, height, 6, 0,
                        height + 24);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16618, 0, 0, 20, 32, 3, height, 6, 0,
                        height + 24);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16621, 0, 0, 20, 32, 3, height, 6, 0,
                        height + 24);
                    break;
            }

            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES, 4, 0, height + 35, session->TrackColours[SCHEME_SUPPORTS]);

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

/** rct2: 0x008A7E48 */
static void corkscrew_rc_track_right_corkscrew_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16622, 0, 0, 32, 20, 3, height, 0, 6,
                        height + 4);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16625, 0, 0, 32, 20, 3, height, 0, 6,
                        height + 4);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16628, 0, 0, 32, 20, 3, height, 0, 6,
                        height + 4);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16631, 0, 0, 32, 20, 3, height, 0, 6,
                        height + 4);
                    break;
            }
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
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
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16623, 0, 0, 20, 20, 3, height, 6, 6,
                        height + 10);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16626, 0, 0, 20, 20, 3, height, 6, 6,
                        height + 10);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16629, 0, 0, 20, 20, 3, height, 6, 6,
                        height + 10);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16632, 0, 0, 20, 20, 3, height, 6, 6,
                        height + 10);
                    break;
            }
            paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 72, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16624, 0, 0, 20, 32, 3, height, 6, 0,
                        height + 24);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16627, 0, 0, 20, 32, 3, height, 6, 0,
                        height + 24);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16630, 0, 0, 20, 32, 3, height, 6, 0,
                        height + 24);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16633, 0, 0, 20, 32, 3, height, 6, 0,
                        height + 24);
                    break;
            }

            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES, 4, 0, height + 35, session->TrackColours[SCHEME_SUPPORTS]);

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

/** rct2: 0x008A7E58 */
static void corkscrew_rc_track_left_corkscrew_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    corkscrew_rc_track_right_corkscrew_up(session, rideIndex, 2 - trackSequence, (direction + 1) & 3, height, tileElement);
}

/** rct2: 0x008A7E68 */
static void corkscrew_rc_track_right_corkscrew_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    corkscrew_rc_track_left_corkscrew_up(session, rideIndex, 2 - trackSequence, (direction - 1) & 3, height, tileElement);
}

/** rct2: 0x008A7E78 */
static void corkscrew_rc_track_left_half_banked_helix_up_small(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16481, 0, 0, 32, 20, 3, height, 0, 6, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16488, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16484, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16487, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16478, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
            }
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 2, height, session->TrackColours[SCHEME_SUPPORTS]);
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
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16480, 0, 0, 16, 16, 3, height, 16, 0,
                        height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16483, 0, 0, 16, 16, 1, height, 0, 0,
                        height + 27);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16486, 0, 0, 16, 16, 3, height, 0, 16,
                        height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16477, 0, 0, 16, 16, 3, height, 16, 16,
                        height);
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
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16479, 0, 0, 20, 32, 3, height, 6, 0, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16482, 0, 0, 1, 32, 26, height, 27, 0,
                        height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16485, 0, 0, 20, 32, 3, height, 6, 0, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16489, 0, 0, 1, 32, 26, height, 27, 0,
                        height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16476, 0, 0, 20, 32, 3, height, 6, 0, height);
                    break;
            }
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 6, height, session->TrackColours[SCHEME_SUPPORTS]);
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
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16478, 0, 0, 20, 32, 3, height, 6, 0, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16481, 0, 0, 20, 32, 3, height, 6, 0, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16488, 0, 0, 1, 32, 26, height, 27, 0,
                        height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16484, 0, 0, 1, 32, 26, height, 27, 0,
                        height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16487, 0, 0, 20, 32, 3, height, 6, 0, height);
                    break;
            }
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 2, height, session->TrackColours[SCHEME_SUPPORTS]);
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
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16477, 0, 0, 16, 16, 3, height, 16, 16,
                        height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16480, 0, 0, 16, 16, 3, height, 0, 16,
                        height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16483, 0, 0, 16, 16, 1, height, 0, 0,
                        height + 27);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16486, 0, 0, 16, 16, 3, height, 16, 0,
                        height);
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
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16476, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16479, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16482, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16485, 0, 0, 32, 20, 3, height, 0, 6, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16489, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    break;
            }
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 6, height, session->TrackColours[SCHEME_SUPPORTS]);
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

/** rct2: 0x008A7E88 */
static void corkscrew_rc_track_right_half_banked_helix_up_small(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16462, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16465, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16468, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16471, 0, 0, 32, 20, 3, height, 0, 6, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16475, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    break;
            }
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 2, height, session->TrackColours[SCHEME_SUPPORTS]);
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
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16463, 0, 0, 16, 16, 3, height, 16, 16,
                        height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16466, 0, 0, 16, 16, 3, height, 0, 16,
                        height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16469, 0, 0, 16, 16, 1, height, 0, 0,
                        height + 27);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16472, 0, 0, 16, 16, 3, height, 16, 0,
                        height);
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
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16464, 0, 0, 20, 32, 3, height, 6, 0, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16467, 0, 0, 20, 32, 3, height, 6, 0, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16474, 0, 0, 1, 32, 26, height, 27, 0,
                        height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16470, 0, 0, 1, 32, 26, height, 27, 0,
                        height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16473, 0, 0, 20, 32, 3, height, 6, 0, height);
                    break;
            }
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 6, height, session->TrackColours[SCHEME_SUPPORTS]);
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
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16465, 0, 0, 20, 32, 3, height, 6, 0, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16468, 0, 0, 1, 32, 26, height, 27, 0,
                        height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16471, 0, 0, 20, 32, 3, height, 6, 0, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16475, 0, 0, 1, 32, 26, height, 27, 0,
                        height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16462, 0, 0, 20, 32, 3, height, 6, 0, height);
                    break;
            }
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 2, height, session->TrackColours[SCHEME_SUPPORTS]);
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
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16466, 0, 0, 16, 16, 3, height, 16, 0,
                        height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16469, 0, 0, 16, 16, 1, height, 0, 0,
                        height + 27);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16472, 0, 0, 16, 16, 3, height, 0, 16,
                        height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16463, 0, 0, 16, 16, 3, height, 16, 16,
                        height);
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
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16467, 0, 0, 32, 20, 3, height, 0, 6, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16474, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16470, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16473, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16464, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
            }
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 6, height, session->TrackColours[SCHEME_SUPPORTS]);
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

/** rct2: 0x008A7E98 */
static void corkscrew_rc_track_left_half_banked_helix_down_small(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    if (trackSequence >= 4)
    {
        trackSequence -= 4;
        direction = (direction - 1) & 3;
    }
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    corkscrew_rc_track_right_half_banked_helix_up_small(
        session, rideIndex, trackSequence, (direction + 1) & 3, height, tileElement);
}

/** rct2: 0x008A7EA8 */
static void corkscrew_rc_track_right_half_banked_helix_down_small(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    if (trackSequence >= 4)
    {
        trackSequence -= 4;
        direction = (direction + 1) & 3;
    }
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    corkscrew_rc_track_left_half_banked_helix_up_small(
        session, rideIndex, trackSequence, (direction - 1) & 3, height, tileElement);
}

/** rct2: 0x008A7EB8 */
static void corkscrew_rc_track_left_half_banked_helix_up_large(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16449, 0, 0, 32, 20, 3, height, 0, 6, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16460, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16454, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16459, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16444, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
            }
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 1, height, session->TrackColours[SCHEME_SUPPORTS]);
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
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16448, 0, 0, 32, 16, 3, height, 0, 0, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16453, 0, 0, 32, 16, 1, height, 0, 0,
                        height + 27);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16458, 0, 0, 32, 16, 3, height, 0, 16,
                        height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16443, 0, 0, 32, 16, 3, height, 0, 16,
                        height);
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
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16447, 0, 0, 16, 16, 3, height, 0, 16,
                        height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16452, 0, 0, 16, 16, 1, height, 16, 16,
                        height + 27);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16457, 0, 0, 16, 16, 3, height, 16, 0,
                        height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16442, 0, 0, 16, 16, 3, height, 0, 0, height);
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
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16446, 0, 0, 16, 32, 3, height, 16, 0,
                        height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16451, 0, 0, 16, 32, 1, height, 0, 0,
                        height + 27);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16456, 0, 0, 16, 32, 3, height, 0, 0, height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16441, 0, 0, 16, 32, 3, height, 16, 0,
                        height);
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
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16445, 0, 0, 20, 32, 3, height, 6, 0, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16450, 0, 0, 1, 32, 26, height, 27, 0,
                        height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16455, 0, 0, 20, 32, 3, height, 6, 0, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16461, 0, 0, 1, 32, 26, height, 27, 0,
                        height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16440, 0, 0, 20, 32, 3, height, 6, 0, height);
                    break;
            }
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 7, height, session->TrackColours[SCHEME_SUPPORTS]);
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
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16444, 0, 0, 20, 32, 3, height, 6, 0, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16449, 0, 0, 20, 32, 3, height, 6, 0, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16460, 0, 0, 1, 32, 26, height, 27, 0,
                        height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16454, 0, 0, 1, 32, 26, height, 27, 0,
                        height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16459, 0, 0, 20, 32, 3, height, 6, 0, height);
                    break;
            }
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 1, height, session->TrackColours[SCHEME_SUPPORTS]);
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
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16443, 0, 0, 16, 32, 3, height, 16, 0,
                        height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16448, 0, 0, 16, 32, 3, height, 0, 0, height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16453, 0, 0, 16, 32, 1, height, 0, 0,
                        height + 27);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16458, 0, 0, 16, 32, 3, height, 16, 0,
                        height);
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
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16442, 0, 0, 16, 16, 3, height, 0, 0, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16447, 0, 0, 16, 16, 3, height, 16, 0,
                        height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16452, 0, 0, 16, 16, 1, height, 16, 16,
                        height + 27);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16457, 0, 0, 16, 16, 3, height, 0, 16,
                        height);
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
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16441, 0, 0, 32, 16, 3, height, 0, 16,
                        height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16446, 0, 0, 32, 16, 3, height, 0, 16,
                        height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16451, 0, 0, 32, 16, 1, height, 0, 0,
                        height + 27);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16456, 0, 0, 32, 16, 3, height, 0, 0, height);
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
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16440, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16445, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16450, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16455, 0, 0, 32, 20, 3, height, 0, 6, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16461, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    break;
            }
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 7, height, session->TrackColours[SCHEME_SUPPORTS]);
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

/** rct2: 0x008A7EC8 */
static void corkscrew_rc_track_right_half_banked_helix_up_large(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16418, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16423, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16428, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16433, 0, 0, 32, 20, 3, height, 0, 6, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16439, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    break;
            }
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 1, height, session->TrackColours[SCHEME_SUPPORTS]);
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
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16419, 0, 0, 32, 16, 3, height, 0, 16,
                        height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16424, 0, 0, 32, 16, 3, height, 0, 16,
                        height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16429, 0, 0, 32, 16, 1, height, 0, 0,
                        height + 27);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16434, 0, 0, 32, 16, 3, height, 0, 0, height);
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
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16420, 0, 0, 16, 16, 3, height, 0, 0, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16425, 0, 0, 16, 16, 3, height, 16, 0,
                        height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16430, 0, 0, 16, 16, 1, height, 16, 16,
                        height + 27);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16435, 0, 0, 16, 16, 3, height, 0, 16,
                        height);
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
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16421, 0, 0, 16, 32, 3, height, 16, 0,
                        height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16426, 0, 0, 16, 32, 3, height, 0, 0, height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16431, 0, 0, 16, 32, 1, height, 0, 0,
                        height + 27);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16436, 0, 0, 16, 32, 3, height, 16, 0,
                        height);
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
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16422, 0, 0, 20, 32, 3, height, 6, 0, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16427, 0, 0, 20, 32, 3, height, 6, 0, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16438, 0, 0, 1, 32, 26, height, 27, 0,
                        height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16432, 0, 0, 1, 32, 26, height, 27, 0,
                        height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16437, 0, 0, 20, 32, 3, height, 6, 0, height);
                    break;
            }
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 7, height, session->TrackColours[SCHEME_SUPPORTS]);
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
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16423, 0, 0, 20, 32, 3, height, 6, 0, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16428, 0, 0, 1, 32, 26, height, 27, 0,
                        height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16433, 0, 0, 20, 32, 3, height, 6, 0, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16439, 0, 0, 1, 32, 26, height, 27, 0,
                        height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16418, 0, 0, 20, 32, 3, height, 6, 0, height);
                    break;
            }
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 1, height, session->TrackColours[SCHEME_SUPPORTS]);
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
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16424, 0, 0, 16, 32, 3, height, 16, 0,
                        height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16429, 0, 0, 16, 32, 1, height, 0, 0,
                        height + 27);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16434, 0, 0, 16, 32, 3, height, 0, 0, height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16419, 0, 0, 16, 32, 3, height, 16, 0,
                        height);
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
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16425, 0, 0, 16, 16, 3, height, 0, 16,
                        height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16430, 0, 0, 16, 16, 1, height, 16, 16,
                        height + 27);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16435, 0, 0, 16, 16, 3, height, 16, 0,
                        height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16420, 0, 0, 16, 16, 3, height, 0, 0, height);
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
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16426, 0, 0, 32, 16, 3, height, 0, 0, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16431, 0, 0, 32, 16, 1, height, 0, 0,
                        height + 27);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16436, 0, 0, 32, 16, 3, height, 0, 16,
                        height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16421, 0, 0, 32, 16, 3, height, 0, 16,
                        height);
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
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16427, 0, 0, 32, 20, 3, height, 0, 6, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16438, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16432, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16437, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16422, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
            }
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 7, height, session->TrackColours[SCHEME_SUPPORTS]);
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

/** rct2: 0x008A7ED8 */
static void corkscrew_rc_track_left_half_banked_helix_down_large(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    if (trackSequence >= 7)
    {
        trackSequence -= 7;
        direction = (direction - 1) & 3;
    }
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    corkscrew_rc_track_right_half_banked_helix_up_large(
        session, rideIndex, trackSequence, (direction + 1) & 3, height, tileElement);
}

/** rct2: 0x008A7EE8 */
static void corkscrew_rc_track_right_half_banked_helix_down_large(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    if (trackSequence >= 7)
    {
        trackSequence -= 7;
        direction = (direction + 1) & 3;
    }
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    corkscrew_rc_track_left_half_banked_helix_up_large(
        session, rideIndex, trackSequence, (direction - 1) & 3, height, tileElement);
}

/** rct2: 0x008A7F18 */
static void corkscrew_rc_track_left_quarter_turn_1_60_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (direction)
    {
        case 0:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 16555, 0, 0, 28, 28, 3, height, 2, 2, height);
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 16559, 0, 0, 28, 28, 1, height, 2, 2, height + 99);
            break;
        case 1:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 16556, 0, 0, 28, 28, 3, height, 2, 2, height);
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 16560, 0, 0, 28, 28, 1, height, 2, 2, height + 99);
            break;
        case 2:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 16557, 0, 0, 28, 28, 3, height, 2, 2, height);
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 16561, 0, 0, 28, 28, 1, height, 2, 2, height + 99);
            break;
        case 3:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 16554, 0, 0, 28, 28, 3, height, 2, 2, height);
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 16558, 0, 0, 28, 28, 1, height, 2, 2, height + 99);
            break;
    }
    track_paint_util_left_quarter_turn_1_tile_tunnel(session, direction, height, -8, TUNNEL_1, +56, TUNNEL_2);
    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 104, 0x20);
}

/** rct2: 0x008A7EF8 */
static void corkscrew_rc_track_right_quarter_turn_1_60_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (direction)
    {
        case 0:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 16546, 0, 0, 28, 28, 3, height, 2, 2, height);
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 16550, 0, 0, 28, 28, 1, height, 2, 2, height + 99);
            break;
        case 1:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 16547, 0, 0, 28, 28, 3, height, 2, 2, height);
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 16551, 0, 0, 28, 28, 1, height, 2, 2, height + 99);
            break;
        case 2:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 16548, 0, 0, 28, 28, 3, height, 2, 2, height);
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 16552, 0, 0, 28, 28, 1, height, 2, 2, height + 99);
            break;
        case 3:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 16549, 0, 0, 28, 28, 3, height, 2, 2, height);
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 16553, 0, 0, 28, 28, 1, height, 2, 2, height + 99);
            break;
    }
    track_paint_util_right_quarter_turn_1_tile_tunnel(session, direction, height, -8, TUNNEL_1, +56, TUNNEL_2);
    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 104, 0x20);
}

/** rct2: 0x008A7F08 */
static void corkscrew_rc_track_left_quarter_turn_1_60_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    corkscrew_rc_track_right_quarter_turn_1_60_deg_up(
        session, rideIndex, trackSequence, (direction + 1) & 3, height, tileElement);
}

/** rct2: 0x008A7F28 */
static void corkscrew_rc_track_right_quarter_turn_1_60_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    corkscrew_rc_track_left_quarter_turn_1_60_deg_up(
        session, rideIndex, trackSequence, (direction - 1) & 3, height, tileElement);
}

/** rct2: 0x008A7F38 */
static void corkscrew_rc_track_brakes(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (direction)
    {
        case 0:
        case 2:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 16230, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 1:
        case 3:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 16231, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
    }
    if (track_paint_util_should_paint_supports(session->MapPosition))
    {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
    }
    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
    paint_util_set_segment_support_height(
        session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x008A7F48 */
static void corkscrew_rc_track_on_ride_photo(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (direction)
    {
        case 0:
            sub_98196C_rotated(session, direction, IMAGE_TYPE_REMAP | SPR_STATION_BASE_D, 0, 0, 32, 32, 1, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 8, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 16224, 0, 0, 32, 20, 0, height, 0, 6, height + 3);
            break;
        case 1:
            sub_98196C_rotated(session, direction, IMAGE_TYPE_REMAP | SPR_STATION_BASE_D, 0, 0, 32, 32, 1, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 6, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 7, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 16225, 0, 0, 32, 20, 0, height, 0, 6, height + 3);
            break;
        case 2:
            sub_98196C_rotated(session, direction, IMAGE_TYPE_REMAP | SPR_STATION_BASE_D, 0, 0, 32, 32, 1, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 8, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 16224, 0, 0, 32, 20, 0, height, 0, 6, height + 3);
            break;
        case 3:
            sub_98196C_rotated(session, direction, IMAGE_TYPE_REMAP | SPR_STATION_BASE_D, 0, 0, 32, 32, 1, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 6, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 7, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 16225, 0, 0, 32, 20, 0, height, 0, 6, height + 3);
            break;
    }
    track_paint_util_onride_photo_paint(session, direction, height + 3, tileElement);
    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 48, 0x20);
}

/** rct2: 0x008A8198 */
static void corkscrew_rc_track_flat_to_60_deg_up_long_base(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16788, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16792, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16796, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16800, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
            }
            if (track_paint_util_should_paint_supports(session->MapPosition))
            {
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES, 4, 3, height, session->TrackColours[SCHEME_SUPPORTS]);
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
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16789, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16793, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16797, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16801, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
            }
            if (track_paint_util_should_paint_supports(session->MapPosition))
            {
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES, 4, 7, height, session->TrackColours[SCHEME_SUPPORTS]);
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16790, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16794, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16798, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16802, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
            }
            if (track_paint_util_should_paint_supports(session->MapPosition))
            {
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES, 4, 9, height, session->TrackColours[SCHEME_SUPPORTS]);
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 64, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16791, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16795, 0, 0, 32, 1, 98, height, 0, 27,
                        height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16799, 0, 0, 32, 1, 98, height, 0, 27,
                        height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16803, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
            }
            if (track_paint_util_should_paint_supports(session->MapPosition))
            {
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES, 4, 18, height, session->TrackColours[SCHEME_SUPPORTS]);
            }
            switch (direction)
            {
                case 1:
                    paint_util_push_tunnel_right(session, height + 24, TUNNEL_SQUARE_8);
                    break;
                case 2:
                    paint_util_push_tunnel_left(session, height + 24, TUNNEL_SQUARE_8);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 80, 0x20);
            break;
    }
}

/** rct2: 0x008A81A8 */
static void corkscrew_rc_track_60_deg_up_to_flat_long_base(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16804, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16808, 0, 0, 32, 1, 98, height, 0, 27,
                        height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16812, 0, 0, 32, 1, 98, height, 0, 27,
                        height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16816, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
            }
            if (track_paint_util_should_paint_supports(session->MapPosition))
            {
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES, 4, 20, height, session->TrackColours[SCHEME_SUPPORTS]);
            }
            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_7);
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 80, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16805, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16809, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16813, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16817, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
            }
            if (track_paint_util_should_paint_supports(session->MapPosition))
            {
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES, 4, 16, height, session->TrackColours[SCHEME_SUPPORTS]);
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 80, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16806, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16810, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16814, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16818, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
            }
            if (track_paint_util_should_paint_supports(session->MapPosition))
            {
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES, 4, 13, height, session->TrackColours[SCHEME_SUPPORTS]);
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 56, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16807, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16811, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16815, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16819, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
            }
            if (track_paint_util_should_paint_supports(session->MapPosition))
            {
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES, 4, 5, height, session->TrackColours[SCHEME_SUPPORTS]);
            }
            switch (direction)
            {
                case 1:
                    paint_util_push_tunnel_right(session, height + 8, TUNNEL_SQUARE_FLAT);
                    break;
                case 2:
                    paint_util_push_tunnel_left(session, height + 8, TUNNEL_SQUARE_FLAT);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 40, 0x20);
            break;
    }
}

/** rct2: 0x008A81B8 */
static void corkscrew_rc_track_flat_to_60_deg_down_long_base(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    corkscrew_rc_track_flat_to_60_deg_up_long_base(
        session, rideIndex, 3 - trackSequence, (direction + 2) & 3, height, tileElement);
}

/** rct2: 0x008A81C8 */
static void corkscrew_rc_track_60_deg_up_to_flat_long_base122(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    corkscrew_rc_track_60_deg_up_to_flat_long_base(
        session, rideIndex, 3 - trackSequence, (direction + 2) & 3, height, tileElement);
}

/** rct2: 0x008A7F68 */
static void corkscrew_rc_track_left_eighth_to_diag(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16740, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16744, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16748, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16752, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
            }
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
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
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16741, 0, 0, 32, 16, 3, height, 0, 0, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16745, 0, 0, 34, 16, 3, height, 0, 0, height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16749, 0, 0, 32, 16, 3, height, 0, 16,
                        height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16753, 0, 0, 32, 16, 3, height, 0, 16,
                        height);
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
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16742, 0, 0, 16, 16, 3, height, 0, 16,
                        height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16746, 0, 0, 16, 16, 3, height, 16, 16,
                        height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16750, 0, 0, 16, 16, 3, height, 16, 0,
                        height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16754, 0, 0, 16, 16, 3, height, 0, 0, height);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction),
                0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 3:
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 4:
            switch (direction)
            {
                case 0:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16743, 0, 0, 16, 16, 3, height, 16, 16,
                        height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16747, 0, 0, 16, 18, 3, height, 0, 16,
                        height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16751, 0, 0, 16, 16, 3, height, 0, 0, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16755, 0, 0, 16, 16, 3, height, 16, 0,
                        height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
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

/** rct2: 0x008A7F78 */
static void corkscrew_rc_track_right_eighth_to_diag(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16724, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16728, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16732, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16736, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
            }
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
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
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16725, 0, 0, 32, 16, 3, height, 0, 16,
                        height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16729, 0, 0, 32, 16, 3, height, 0, 16,
                        height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16733, 0, 0, 34, 16, 3, height, 0, 0, height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16737, 0, 0, 32, 16, 3, height, 0, 0, height);
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
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16726, 0, 0, 16, 16, 3, height, 0, 0, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16730, 0, 0, 16, 16, 3, height, 16, 0,
                        height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16734, 0, 0, 28, 28, 3, height, 4, 4, height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16738, 0, 0, 16, 16, 3, height, 0, 16,
                        height);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction),
                0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 3:
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 4:
            switch (direction)
            {
                case 0:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16727, 0, 0, 16, 16, 3, height, 16, 0,
                        height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16731, 0, 0, 16, 16, 3, height, 0, 0, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16735, 0, 0, 16, 18, 3, height, 0, 16,
                        height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16739, 0, 0, 16, 16, 3, height, 16, 16,
                        height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
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

/** rct2: 0x008A7F88 */
static void corkscrew_rc_track_left_eighth_to_orthogonal(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    corkscrew_rc_track_right_eighth_to_diag(session, rideIndex, trackSequence, (direction + 2) & 3, height, tileElement);
}

/** rct2: 0x008A7F98 */
static void corkscrew_rc_track_right_eighth_to_orthogonal(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    corkscrew_rc_track_left_eighth_to_diag(session, rideIndex, trackSequence, (direction + 3) & 3, height, tileElement);
}

/** rct2: 0x008A7FA8 */
static void corkscrew_rc_track_left_eighth_bank_to_diag(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16772, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16776, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16780, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16784, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
            }
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
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
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16773, 0, 0, 32, 16, 3, height, 0, 0, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16777, 0, 0, 34, 16, 0, height, 0, 0,
                        height + 27);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16781, 0, 0, 32, 16, 3, height, 0, 16,
                        height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16785, 0, 0, 32, 16, 3, height, 0, 16,
                        height);
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
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16774, 0, 0, 16, 16, 3, height, 0, 16,
                        height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16778, 0, 0, 16, 16, 0, height, 16, 16,
                        height + 27);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16782, 0, 0, 16, 16, 3, height, 16, 0,
                        height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16786, 0, 0, 16, 16, 3, height, 0, 0, height);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction),
                0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 3:
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 4:
            switch (direction)
            {
                case 0:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16775, 0, 0, 16, 16, 3, height, 16, 16,
                        height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16779, 0, 0, 16, 18, 0, height, 0, 16,
                        height + 27);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16783, 0, 0, 16, 16, 3, height, 0, 0, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16787, 0, 0, 16, 16, 3, height, 16, 0,
                        height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
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

/** rct2: 0x008A7FB8 */
static void corkscrew_rc_track_right_eighth_bank_to_diag(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16756, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16760, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16764, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16768, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    break;
            }
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
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
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16757, 0, 0, 16, 16, 3, height, 0, 0, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16761, 0, 0, 32, 16, 3, height, 0, 16,
                        height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16765, 0, 0, 34, 16, 0, height, 0, 0,
                        height + 27);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16769, 0, 0, 32, 16, 3, height, 0, 0, height);
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
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16758, 0, 0, 16, 16, 3, height, 0, 0, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16762, 0, 0, 16, 16, 3, height, 16, 0,
                        height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16766, 0, 0, 28, 28, 0, height, 4, 4,
                        height + 27);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16770, 0, 0, 16, 16, 3, height, 0, 16,
                        height);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction),
                0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 3:
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 4:
            switch (direction)
            {
                case 0:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16759, 0, 0, 16, 16, 3, height, 16, 0,
                        height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16763, 0, 0, 16, 16, 3, height, 0, 0, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16767, 0, 0, 16, 18, 0, height, 0, 16,
                        height + 27);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16771, 0, 0, 16, 16, 3, height, 16, 16,
                        height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
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

/** rct2: 0x008A7FC8 */
static void corkscrew_rc_track_left_eighth_bank_to_orthogonal(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    corkscrew_rc_track_right_eighth_bank_to_diag(session, rideIndex, trackSequence, (direction + 2) & 3, height, tileElement);
}

/** rct2: 0x008A7FD8 */
static void corkscrew_rc_track_right_eighth_bank_to_orthogonal(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    corkscrew_rc_track_left_eighth_bank_to_diag(session, rideIndex, trackSequence, (direction + 3) & 3, height, tileElement);
}

/** rct2: 0x008A7F58 */
static void corkscrew_rc_track_diag_flat(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (trackSequence)
    {
        case 0:
            if (tileElement->AsTrack()->HasChain())
            {
                switch (direction)
                {
                    case 3:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16699, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 3:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16637, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 1:
            if (tileElement->AsTrack()->HasChain())
            {
                switch (direction)
                {
                    case 0:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16696, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16634, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 2:
            if (tileElement->AsTrack()->HasChain())
            {
                switch (direction)
                {
                    case 2:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16698, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 2:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16636, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 3:
            if (tileElement->AsTrack()->HasChain())
            {
                switch (direction)
                {
                    case 0:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16697, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16635, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008A8008 */
static void corkscrew_rc_track_diag_25_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (trackSequence)
    {
        case 0:
            if (tileElement->AsTrack()->HasChain())
            {
                switch (direction)
                {
                    case 3:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16711, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 3:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16649, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 56, 0x20);
            break;
        case 1:
            if (tileElement->AsTrack()->HasChain())
            {
                switch (direction)
                {
                    case 0:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16708, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16646, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 56, 0x20);
            break;
        case 2:
            if (tileElement->AsTrack()->HasChain())
            {
                switch (direction)
                {
                    case 2:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16710, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 2:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16648, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 56, 0x20);
            break;
        case 3:
            if (tileElement->AsTrack()->HasChain())
            {
                switch (direction)
                {
                    case 0:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 1, 8, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16709, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 0, 8, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 2, 8, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 3, 8, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 1, 8, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16647, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 0, 8, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 2, 8, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 3, 8, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 56, 0x20);
            break;
    }
}

/** rct2: 0x008A8068 */
static void corkscrew_rc_track_diag_60_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (trackSequence)
    {
        case 0:
            if (tileElement->AsTrack()->HasChain())
            {
                switch (direction)
                {
                    case 3:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16723, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 3:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16661, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 104, 0x20);
            break;
        case 1:
            if (tileElement->AsTrack()->HasChain())
            {
                switch (direction)
                {
                    case 0:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16720, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16658, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 104, 0x20);
            break;
        case 2:
            if (tileElement->AsTrack()->HasChain())
            {
                switch (direction)
                {
                    case 2:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16722, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 2:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16660, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 104, 0x20);
            break;
        case 3:
            if (tileElement->AsTrack()->HasChain())
            {
                switch (direction)
                {
                    case 0:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 1, 32, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16721, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 0, 36, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 2, 32, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 3, 36, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 1, 32, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16659, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 0, 36, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 2, 32, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 3, 36, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 104, 0x20);
            break;
    }
}

/** rct2: 0x008A7FE8 */
static void corkscrew_rc_track_diag_flat_to_25_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (trackSequence)
    {
        case 0:
            if (tileElement->AsTrack()->HasChain())
            {
                switch (direction)
                {
                    case 3:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16703, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 3:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16641, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
        case 1:
            if (tileElement->AsTrack()->HasChain())
            {
                switch (direction)
                {
                    case 0:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16700, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16638, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
        case 2:
            if (tileElement->AsTrack()->HasChain())
            {
                switch (direction)
                {
                    case 2:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16702, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 2:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16640, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
        case 3:
            if (tileElement->AsTrack()->HasChain())
            {
                switch (direction)
                {
                    case 0:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16701, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16639, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x008A8048 */
static void corkscrew_rc_track_diag_25_deg_up_to_60_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (trackSequence)
    {
        case 0:
            if (tileElement->AsTrack()->HasChain())
            {
                switch (direction)
                {
                    case 3:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16715, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 3:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16653, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 72, 0x20);
            break;
        case 1:
            if (tileElement->AsTrack()->HasChain())
            {
                switch (direction)
                {
                    case 0:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16712, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16650, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 72, 0x20);
            break;
        case 2:
            if (tileElement->AsTrack()->HasChain())
            {
                switch (direction)
                {
                    case 2:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16714, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 2:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16652, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 72, 0x20);
            break;
        case 3:
            if (tileElement->AsTrack()->HasChain())
            {
                switch (direction)
                {
                    case 0:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 1, 16, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16713, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 0, 16, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 2, 16, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 3, 16, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 1, 16, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16651, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 0, 16, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 2, 16, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 3, 16, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 72, 0x20);
            break;
    }
}

/** rct2: 0x008A8058 */
static void corkscrew_rc_track_diag_60_deg_up_to_25_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (trackSequence)
    {
        case 0:
            if (tileElement->AsTrack()->HasChain())
            {
                switch (direction)
                {
                    case 3:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16719, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 3:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16657, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 72, 0x20);
            break;
        case 1:
            if (tileElement->AsTrack()->HasChain())
            {
                switch (direction)
                {
                    case 0:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16716, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16654, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 72, 0x20);
            break;
        case 2:
            if (tileElement->AsTrack()->HasChain())
            {
                switch (direction)
                {
                    case 2:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16718, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 2:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16656, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 72, 0x20);
            break;
        case 3:
            if (tileElement->AsTrack()->HasChain())
            {
                switch (direction)
                {
                    case 0:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 1, 21, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16717, -16, -16, 16, 16, 3, height, 0, 0,
                            height);
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 0, 21, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 2, 21, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 3, 21, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 1, 21, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16655, -16, -16, 16, 16, 3, height, 0, 0,
                            height);
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 0, 21, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 2, 21, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 3, 21, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 72, 0x20);
            break;
    }
}

/** rct2: 0x008A7FF8 */
static void corkscrew_rc_track_diag_25_deg_up_to_flat(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (trackSequence)
    {
        case 0:
            if (tileElement->AsTrack()->HasChain())
            {
                switch (direction)
                {
                    case 3:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16707, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 3:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16645, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 56, 0x20);
            break;
        case 1:
            if (tileElement->AsTrack()->HasChain())
            {
                switch (direction)
                {
                    case 0:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16704, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16642, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 56, 0x20);
            break;
        case 2:
            if (tileElement->AsTrack()->HasChain())
            {
                switch (direction)
                {
                    case 2:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16706, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 2:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16644, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 56, 0x20);
            break;
        case 3:
            if (tileElement->AsTrack()->HasChain())
            {
                switch (direction)
                {
                    case 0:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 1, 4, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16705, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 0, 4, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 2, 4, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 3, 4, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 1, 4, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16643, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 0, 4, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 2, 4, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 3, 4, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 56, 0x20);
            break;
    }
}

/** rct2: 0x008A8038 */
static void corkscrew_rc_track_diag_25_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (trackSequence)
    {
        case 0:
            if (tileElement->AsTrack()->HasChain())
            {
                switch (direction)
                {
                    case 3:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16709, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 3:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16647, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 56, 0x20);
            break;
        case 1:
            if (tileElement->AsTrack()->HasChain())
            {
                switch (direction)
                {
                    case 0:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16710, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16648, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 56, 0x20);
            break;
        case 2:
            if (tileElement->AsTrack()->HasChain())
            {
                switch (direction)
                {
                    case 2:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16708, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 2:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16646, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 56, 0x20);
            break;
        case 3:
            if (tileElement->AsTrack()->HasChain())
            {
                switch (direction)
                {
                    case 0:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 1, 8, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16711, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 0, 8, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 2, 8, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 3, 8, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 1, 8, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16649, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 0, 8, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 2, 8, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 3, 8, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 56, 0x20);
            break;
    }
}

/** rct2: 0x008A8098 */
static void corkscrew_rc_track_diag_60_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (trackSequence)
    {
        case 0:
            if (tileElement->AsTrack()->HasChain())
            {
                switch (direction)
                {
                    case 3:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16721, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 3:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16659, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 104, 0x20);
            break;
        case 1:
            if (tileElement->AsTrack()->HasChain())
            {
                switch (direction)
                {
                    case 0:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16722, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16660, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 104, 0x20);
            break;
        case 2:
            if (tileElement->AsTrack()->HasChain())
            {
                switch (direction)
                {
                    case 2:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16720, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 2:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16658, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 104, 0x20);
            break;
        case 3:
            if (tileElement->AsTrack()->HasChain())
            {
                switch (direction)
                {
                    case 0:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 1, 24, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16723, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 0, 28, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 2, 24, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 3, 28, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 1, 24, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16661, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 0, 28, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 2, 24, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 3, 28, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 104, 0x20);
            break;
    }
}

/** rct2: 0x008A8018 */
static void corkscrew_rc_track_diag_flat_to_25_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (trackSequence)
    {
        case 0:
            if (tileElement->AsTrack()->HasChain())
            {
                switch (direction)
                {
                    case 3:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16705, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 3:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16643, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            break;
        case 1:
            if (tileElement->AsTrack()->HasChain())
            {
                switch (direction)
                {
                    case 0:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16706, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16644, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            break;
        case 2:
            if (tileElement->AsTrack()->HasChain())
            {
                switch (direction)
                {
                    case 2:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16704, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 2:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16642, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            break;
        case 3:
            if (tileElement->AsTrack()->HasChain())
            {
                switch (direction)
                {
                    case 0:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 1, 4, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16707, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 0, 4, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 2, 4, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 3, 4, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 1, 4, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16645, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 0, 4, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 2, 4, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 3, 4, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            break;
    }

    paint_util_set_general_support_height(session, height + 56, 0x20);
}

/** rct2: 0x008A8078 */
static void corkscrew_rc_track_diag_25_deg_down_to_60_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (trackSequence)
    {
        case 0:
            if (tileElement->AsTrack()->HasChain())
            {
                switch (direction)
                {
                    case 3:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16717, -16, -16, 16, 16, 3, height, 0, 0,
                            height);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 3:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16655, -16, -16, 16, 16, 3, height, 0, 0,
                            height);
                        break;
                }
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 72, 0x20);
            break;
        case 1:
            if (tileElement->AsTrack()->HasChain())
            {
                switch (direction)
                {
                    case 0:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16718, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16656, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 72, 0x20);
            break;
        case 2:
            if (tileElement->AsTrack()->HasChain())
            {
                switch (direction)
                {
                    case 2:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16716, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 2:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16654, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 72, 0x20);
            break;
        case 3:
            if (tileElement->AsTrack()->HasChain())
            {
                switch (direction)
                {
                    case 0:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 1, 17, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16719, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 0, 17, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 2, 17, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 3, 17, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 1, 17, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16657, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 0, 17, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 2, 17, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 3, 17, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 72, 0x20);
            break;
    }
}

/** rct2: 0x008A8088 */
static void corkscrew_rc_track_diag_60_deg_down_to_25_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (trackSequence)
    {
        case 0:
            if (tileElement->AsTrack()->HasChain())
            {
                switch (direction)
                {
                    case 3:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16713, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 3:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16651, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 72, 0x20);
            break;
        case 1:
            if (tileElement->AsTrack()->HasChain())
            {
                switch (direction)
                {
                    case 0:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16714, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16652, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 72, 0x20);
            break;
        case 2:
            if (tileElement->AsTrack()->HasChain())
            {
                switch (direction)
                {
                    case 2:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16712, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 2:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16650, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 72, 0x20);
            break;
        case 3:
            if (tileElement->AsTrack()->HasChain())
            {
                switch (direction)
                {
                    case 0:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 1, 8, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16715, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 0, 8, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 2, 8, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 3, 8, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 1, 8, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16653, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 0, 8, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 2, 8, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 3, 8, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 72, 0x20);
            break;
    }
}

/** rct2: 0x008A8028 */
static void corkscrew_rc_track_diag_25_deg_down_to_flat(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (trackSequence)
    {
        case 0:
            if (tileElement->AsTrack()->HasChain())
            {
                switch (direction)
                {
                    case 3:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16701, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 3:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16639, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
        case 1:
            if (tileElement->AsTrack()->HasChain())
            {
                switch (direction)
                {
                    case 0:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16702, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16640, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
        case 2:
            if (tileElement->AsTrack()->HasChain())
            {
                switch (direction)
                {
                    case 2:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16700, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 2:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16638, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
        case 3:
            if (tileElement->AsTrack()->HasChain())
            {
                switch (direction)
                {
                    case 0:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16703, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 16641, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_TUBES, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x008A80C8 */
static void corkscrew_rc_track_diag_flat_to_left_bank(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16669, -16, -16, 32, 32, 3, height, -16, -16,
                        height);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16666, -16, -16, 32, 32, 3, height, -16, -16,
                        height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16670, -16, -16, 32, 32, 0, height, -16, -16,
                        height + 27);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16668, -16, -16, 32, 32, 3, height, -16, -16,
                        height);
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
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16667, -16, -16, 32, 32, 3, height, -16, -16,
                        height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008A80D8 */
static void corkscrew_rc_track_diag_flat_to_right_bank(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16674, -16, -16, 32, 32, 3, height, -16, -16,
                        height);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16671, -16, -16, 32, 32, 3, height, -16, -16,
                        height);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16673, -16, -16, 32, 32, 3, height, -16, -16,
                        height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16675, -16, -16, 32, 32, 0, height, -16, -16,
                        height + 27);
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
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16672, -16, -16, 32, 32, 3, height, -16, -16,
                        height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008A80E8 */
static void corkscrew_rc_track_diag_left_bank_to_flat(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16672, -16, -16, 32, 32, 3, height, -16, -16,
                        height);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16673, -16, -16, 32, 32, 3, height, -16, -16,
                        height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16675, -16, -16, 32, 32, 0, height, -16, -16,
                        height + 27);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16671, -16, -16, 32, 32, 3, height, -16, -16,
                        height);
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
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16674, -16, -16, 32, 32, 3, height, -16, -16,
                        height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008A80F8 */
static void corkscrew_rc_track_diag_right_bank_to_flat(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16667, -16, -16, 32, 32, 3, height, -16, -16,
                        height);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16668, -16, -16, 32, 32, 3, height, -16, -16,
                        height);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16666, -16, -16, 32, 32, 3, height, -16, -16,
                        height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16670, -16, -16, 32, 32, 0, height, -16, -16,
                        height + 27);
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
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16669, -16, -16, 32, 32, 3, height, -16, -16,
                        height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008A8128 */
static void corkscrew_rc_track_diag_left_bank_to_25_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16689, -16, -16, 32, 32, 3, height, -16, -16,
                        height);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16686, -16, -16, 32, 32, 3, height, -16, -16,
                        height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16690, -16, -16, 32, 32, 0, height, -16, -16,
                        height + 35);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16688, -16, -16, 32, 32, 3, height, -16, -16,
                        height);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    metal_b_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16687, -16, -16, 32, 32, 3, height, -16, -16,
                        height);
                    metal_b_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    metal_b_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    metal_b_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x008A8138 */
static void corkscrew_rc_track_diag_right_bank_to_25_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16694, -16, -16, 32, 32, 3, height, -16, -16,
                        height);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16691, -16, -16, 32, 32, 3, height, -16, -16,
                        height);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16693, -16, -16, 32, 32, 3, height, -16, -16,
                        height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16695, -16, -16, 32, 32, 0, height, -16, -16,
                        height + 35);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    metal_b_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16692, -16, -16, 32, 32, 3, height, -16, -16,
                        height);
                    metal_b_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    metal_b_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    metal_b_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x008A8108 */
static void corkscrew_rc_track_diag_25_deg_up_to_left_bank(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16679, -16, -16, 32, 32, 3, height, -16, -16,
                        height);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 56, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16676, -16, -16, 32, 32, 3, height, -16, -16,
                        height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16680, -16, -16, 32, 32, 0, height, -16, -16,
                        height + 35);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 56, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16678, -16, -16, 32, 32, 3, height, -16, -16,
                        height);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 56, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    metal_b_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 1, 4, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16677, -16, -16, 32, 32, 3, height, -16, -16,
                        height);
                    metal_b_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 0, 4, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    metal_b_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 2, 4, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    metal_b_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 3, 4, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 56, 0x20);
            break;
    }
}

/** rct2: 0x008A8118 */
static void corkscrew_rc_track_diag_25_deg_up_to_right_bank(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16684, -16, -16, 32, 32, 3, height, -16, -16,
                        height);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 56, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16681, -16, -16, 32, 32, 3, height, -16, -16,
                        height);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 56, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16683, -16, -16, 32, 32, 3, height, -16, -16,
                        height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16685, -16, -16, 32, 32, 0, height, -16, -16,
                        height + 35);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 56, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    metal_b_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 1, 4, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16682, -16, -16, 32, 32, 3, height, -16, -16,
                        height);
                    metal_b_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 0, 4, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    metal_b_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 2, 4, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    metal_b_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 3, 4, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 56, 0x20);
            break;
    }
}

/** rct2: 0x008A8148 */
static void corkscrew_rc_track_diag_left_bank_to_25_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16682, -16, -16, 32, 32, 3, height, -16, -16,
                        height);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16683, -16, -16, 32, 32, 3, height, -16, -16,
                        height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16685, -16, -16, 32, 32, 0, height, -16, -16,
                        height + 35);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16681, -16, -16, 32, 32, 3, height, -16, -16,
                        height);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    metal_b_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 1, 4, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16684, -16, -16, 32, 32, 3, height, -16, -16,
                        height);
                    metal_b_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 0, 4, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    metal_b_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 2, 4, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    metal_b_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 3, 4, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            break;
    }

    paint_util_set_general_support_height(session, height + 56, 0x20);
}

/** rct2: 0x008A8158 */
static void corkscrew_rc_track_diag_right_bank_to_25_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16677, -16, -16, 32, 32, 3, height, -16, -16,
                        height);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16678, -16, -16, 32, 32, 3, height, -16, -16,
                        height);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16676, -16, -16, 32, 32, 3, height, -16, -16,
                        height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16680, -16, -16, 32, 32, 0, height, -16, -16,
                        height + 35);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    metal_b_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 1, 4, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16679, -16, -16, 32, 32, 3, height, -16, -16,
                        height);
                    metal_b_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 0, 4, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    metal_b_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 2, 4, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    metal_b_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 3, 4, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            break;
    }

    paint_util_set_general_support_height(session, height + 56, 0x20);
}

/** rct2: 0x008A8168 */
static void corkscrew_rc_track_diag_25_deg_down_to_left_bank(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16692, -16, -16, 32, 32, 3, height, -16, -16,
                        height);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16693, -16, -16, 32, 32, 3, height, -16, -16,
                        height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16695, -16, -16, 32, 32, 0, height, -16, -16,
                        height + 35);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16691, -16, -16, 32, 32, 3, height, -16, -16,
                        height);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    metal_b_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16694, -16, -16, 32, 32, 3, height, -16, -16,
                        height);
                    metal_b_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    metal_b_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    metal_b_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x008A8178 */
static void corkscrew_rc_track_diag_25_deg_down_to_right_bank(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16687, -16, -16, 32, 32, 3, height, -16, -16,
                        height);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16688, -16, -16, 32, 32, 3, height, -16, -16,
                        height);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16686, -16, -16, 32, 32, 3, height, -16, -16,
                        height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16690, -16, -16, 32, 32, 0, height, -16, -16,
                        height + 35);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    metal_b_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16689, -16, -16, 32, 32, 3, height, -16, -16,
                        height);
                    metal_b_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    metal_b_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    metal_b_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x008A80A8 */
static void corkscrew_rc_track_diag_left_bank(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16665, -16, -16, 32, 32, 3, height, -16, -16,
                        height);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16662, -16, -16, 32, 32, 0, height, -16, -16,
                        height + 27);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16664, -16, -16, 32, 32, 3, height, -16, -16,
                        height);
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
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16663, -16, -16, 32, 32, 3, height, -16, -16,
                        height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008A80B8 */
static void corkscrew_rc_track_diag_right_bank(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16663, -16, -16, 32, 32, 3, height, -16, -16,
                        height);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16664, -16, -16, 32, 32, 3, height, -16, -16,
                        height);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16662, -16, -16, 32, 32, 0, height, -16, -16,
                        height + 27);
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
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 16665, -16, -16, 32, 32, 3, height, -16, -16,
                        height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008A8188 */
static void corkscrew_rc_track_block_brakes(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (direction)
    {
        case 0:
        case 2:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 16232, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 1:
        case 3:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 16233, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
    }
    metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
    paint_util_set_segment_support_height(
        session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

static void corkscrew_rc_track_booster(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    if (!is_csg_loaded())
    {
        corkscrew_rc_track_brakes(session, rideIndex, trackSequence, direction, height, tileElement);
        return;
    }

    uint32_t sprite_ne_sw_behind = SPR_CSG_BEGIN + 56213;
    uint32_t sprite_nw_se_behind = SPR_CSG_BEGIN + 56214;
    uint32_t sprite_ne_sw_after = SPR_CSG_BEGIN + 56215;
    uint32_t sprite_nw_se_after = SPR_CSG_BEGIN + 56216;

    switch (direction)
    {
        case 0:
        case 2:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | sprite_ne_sw_behind, 0, 0, 32, 20, 3, height, 0, 6,
                height);
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | sprite_ne_sw_after, 0, 0, 32, 1, 26, height, 0, 27,
                height);
            break;
        case 1:
        case 3:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | sprite_nw_se_behind, 0, 0, 32, 20, 3, height, 0, 6,
                height);
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | sprite_nw_se_after, 0, 0, 32, 1, 26, height, 0, 27,
                height);
            break;
    }
    if (track_paint_util_should_paint_supports(session->MapPosition))
    {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
    }
    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
    paint_util_set_segment_support_height(
        session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

TRACK_PAINT_FUNCTION get_track_paint_function_corkscrew_rc(int32_t trackType)
{
    switch (trackType)
    {
        case TrackElemType::Flat:
            return corkscrew_rc_track_flat;
        case TrackElemType::EndStation:
        case TrackElemType::BeginStation:
        case TrackElemType::MiddleStation:
            return corkscrew_rc_track_station;
        case TrackElemType::Up25:
            return corkscrew_rc_track_25_deg_up;
        case TrackElemType::Up60:
            return corkscrew_rc_track_60_deg_up;
        case TrackElemType::FlatToUp25:
            return corkscrew_rc_track_flat_to_25_deg_up;
        case TrackElemType::Up25ToUp60:
            return corkscrew_rc_track_25_deg_up_to_60_deg_up;
        case TrackElemType::Up60ToUp25:
            return corkscrew_rc_track_60_deg_up_to_25_deg_up;
        case TrackElemType::Up25ToFlat:
            return corkscrew_rc_track_25_deg_up_to_flat;
        case TrackElemType::Down25:
            return corkscrew_rc_track_25_deg_down;
        case TrackElemType::Down60:
            return corkscrew_rc_track_60_deg_down;
        case TrackElemType::FlatToDown25:
            return corkscrew_rc_track_flat_to_25_deg_down;
        case TrackElemType::Down25ToDown60:
            return corkscrew_rc_track_25_deg_down_to_60_deg_down;
        case TrackElemType::Down60ToDown25:
            return corkscrew_rc_track_60_deg_down_to_25_deg_down;
        case TrackElemType::Down25ToFlat:
            return corkscrew_rc_track_25_deg_down_to_flat;
        case TrackElemType::LeftQuarterTurn5Tiles:
            return corkscrew_rc_track_left_quarter_turn_5;
        case TrackElemType::RightQuarterTurn5Tiles:
            return corkscrew_rc_track_right_quarter_turn_5;
        case TrackElemType::FlatToLeftBank:
            return corkscrew_rc_track_flat_to_left_bank;
        case TrackElemType::FlatToRightBank:
            return corkscrew_rc_track_flat_to_right_bank;
        case TrackElemType::LeftBankToFlat:
            return corkscrew_rc_track_left_bank_to_flat;
        case TrackElemType::RightBankToFlat:
            return corkscrew_rc_track_right_bank_to_flat;
        case TrackElemType::BankedLeftQuarterTurn5Tiles:
            return corkscrew_rc_track_banked_left_quarter_turn_5;
        case TrackElemType::BankedRightQuarterTurn5Tiles:
            return corkscrew_rc_track_banked_right_quarter_turn_5;
        case TrackElemType::LeftBankToUp25:
            return corkscrew_rc_track_left_bank_to_25_deg_up;
        case TrackElemType::RightBankToUp25:
            return corkscrew_rc_track_right_bank_to_25_deg_up;
        case TrackElemType::Up25ToLeftBank:
            return corkscrew_rc_track_25_deg_up_to_left_bank;
        case TrackElemType::Up25ToRightBank:
            return corkscrew_rc_track_25_deg_up_to_right_bank;
        case TrackElemType::LeftBankToDown25:
            return corkscrew_rc_track_left_bank_to_25_deg_down;
        case TrackElemType::RightBankToDown25:
            return corkscrew_rc_track_right_bank_to_25_deg_down;
        case TrackElemType::Down25ToLeftBank:
            return corkscrew_rc_track_25_deg_down_to_left_bank;
        case TrackElemType::Down25ToRightBank:
            return corkscrew_rc_track_25_deg_down_to_right_bank;
        case TrackElemType::LeftBank:
            return corkscrew_rc_track_left_bank;
        case TrackElemType::RightBank:
            return corkscrew_rc_track_right_bank;
        case TrackElemType::LeftQuarterTurn5TilesUp25:
            return corkscrew_rc_track_left_quarter_turn_5_25_deg_up;
        case TrackElemType::RightQuarterTurn5TilesUp25:
            return corkscrew_rc_track_right_quarter_turn_5_25_deg_up;
        case TrackElemType::LeftQuarterTurn5TilesDown25:
            return corkscrew_rc_track_left_quarter_turn_5_25_deg_down;
        case TrackElemType::RightQuarterTurn5TilesDown25:
            return corkscrew_rc_track_right_quarter_turn_5_25_deg_down;
        case TrackElemType::SBendLeft:
            return corkscrew_rc_track_s_bend_left;
        case TrackElemType::SBendRight:
            return corkscrew_rc_track_s_bend_right;
        case TrackElemType::LeftVerticalLoop:
            return corkscrew_rc_track_left_vertical_loop;
        case TrackElemType::RightVerticalLoop:
            return corkscrew_rc_track_right_vertical_loop;
        case TrackElemType::LeftQuarterTurn3Tiles:
            return corkscrew_rc_track_left_quarter_turn_3;
        case TrackElemType::RightQuarterTurn3Tiles:
            return corkscrew_rc_track_right_quarter_turn_3;
        case TrackElemType::LeftBankedQuarterTurn3Tiles:
            return corkscrew_rc_track_left_quarter_turn_3_bank;
        case TrackElemType::RightBankedQuarterTurn3Tiles:
            return corkscrew_rc_track_right_quarter_turn_3_bank;
        case TrackElemType::LeftQuarterTurn3TilesUp25:
            return corkscrew_rc_track_left_quarter_turn_3_25_deg_up;
        case TrackElemType::RightQuarterTurn3TilesUp25:
            return corkscrew_rc_track_right_quarter_turn_3_25_deg_up;
        case TrackElemType::LeftQuarterTurn3TilesDown25:
            return corkscrew_rc_track_left_quarter_turn_3_25_deg_down;
        case TrackElemType::RightQuarterTurn3TilesDown25:
            return corkscrew_rc_track_right_quarter_turn_3_25_deg_down;
        case TrackElemType::HalfLoopUp:
            return corkscrew_rc_track_half_loop_up;
        case TrackElemType::HalfLoopDown:
            return corkscrew_rc_track_half_loop_down;
        case TrackElemType::LeftCorkscrewUp:
            return corkscrew_rc_track_left_corkscrew_up;
        case TrackElemType::RightCorkscrewUp:
            return corkscrew_rc_track_right_corkscrew_up;
        case TrackElemType::LeftCorkscrewDown:
            return corkscrew_rc_track_left_corkscrew_down;
        case TrackElemType::RightCorkscrewDown:
            return corkscrew_rc_track_right_corkscrew_down;
        case TrackElemType::LeftHalfBankedHelixUpSmall:
            return corkscrew_rc_track_left_half_banked_helix_up_small;
        case TrackElemType::RightHalfBankedHelixUpSmall:
            return corkscrew_rc_track_right_half_banked_helix_up_small;
        case TrackElemType::LeftHalfBankedHelixDownSmall:
            return corkscrew_rc_track_left_half_banked_helix_down_small;
        case TrackElemType::RightHalfBankedHelixDownSmall:
            return corkscrew_rc_track_right_half_banked_helix_down_small;
        case TrackElemType::LeftHalfBankedHelixUpLarge:
            return corkscrew_rc_track_left_half_banked_helix_up_large;
        case TrackElemType::RightHalfBankedHelixUpLarge:
            return corkscrew_rc_track_right_half_banked_helix_up_large;
        case TrackElemType::LeftHalfBankedHelixDownLarge:
            return corkscrew_rc_track_left_half_banked_helix_down_large;
        case TrackElemType::RightHalfBankedHelixDownLarge:
            return corkscrew_rc_track_right_half_banked_helix_down_large;
        case TrackElemType::LeftQuarterTurn1TileUp60:
            return corkscrew_rc_track_left_quarter_turn_1_60_deg_up;
        case TrackElemType::RightQuarterTurn1TileUp60:
            return corkscrew_rc_track_right_quarter_turn_1_60_deg_up;
        case TrackElemType::LeftQuarterTurn1TileDown60:
            return corkscrew_rc_track_left_quarter_turn_1_60_deg_down;
        case TrackElemType::RightQuarterTurn1TileDown60:
            return corkscrew_rc_track_right_quarter_turn_1_60_deg_down;
        case TrackElemType::Brakes:
            return corkscrew_rc_track_brakes;
        case TrackElemType::OnRidePhoto:
            return corkscrew_rc_track_on_ride_photo;
        case TrackElemType::FlatToUp60LongBase:
            return corkscrew_rc_track_flat_to_60_deg_up_long_base;
        case TrackElemType::Up60ToFlatLongBase:
            return corkscrew_rc_track_60_deg_up_to_flat_long_base;
        case TrackElemType::FlatToDown60LongBase:
            return corkscrew_rc_track_flat_to_60_deg_down_long_base;
        case TrackElemType::Up60ToFlatLongBase122:
            return corkscrew_rc_track_60_deg_up_to_flat_long_base122;
        case TrackElemType::LeftEighthToDiag:
            return corkscrew_rc_track_left_eighth_to_diag;
        case TrackElemType::RightEighthToDiag:
            return corkscrew_rc_track_right_eighth_to_diag;
        case TrackElemType::LeftEighthToOrthogonal:
            return corkscrew_rc_track_left_eighth_to_orthogonal;
        case TrackElemType::RightEighthToOrthogonal:
            return corkscrew_rc_track_right_eighth_to_orthogonal;
        case TrackElemType::LeftEighthBankToDiag:
            return corkscrew_rc_track_left_eighth_bank_to_diag;
        case TrackElemType::RightEighthBankToDiag:
            return corkscrew_rc_track_right_eighth_bank_to_diag;
        case TrackElemType::LeftEighthBankToOrthogonal:
            return corkscrew_rc_track_left_eighth_bank_to_orthogonal;
        case TrackElemType::RightEighthBankToOrthogonal:
            return corkscrew_rc_track_right_eighth_bank_to_orthogonal;
        case TrackElemType::DiagFlat:
            return corkscrew_rc_track_diag_flat;
        case TrackElemType::DiagUp25:
            return corkscrew_rc_track_diag_25_deg_up;
        case TrackElemType::DiagUp60:
            return corkscrew_rc_track_diag_60_deg_up;
        case TrackElemType::DiagFlatToUp25:
            return corkscrew_rc_track_diag_flat_to_25_deg_up;
        case TrackElemType::DiagUp25ToUp60:
            return corkscrew_rc_track_diag_25_deg_up_to_60_deg_up;
        case TrackElemType::DiagUp60ToUp25:
            return corkscrew_rc_track_diag_60_deg_up_to_25_deg_up;
        case TrackElemType::DiagUp25ToFlat:
            return corkscrew_rc_track_diag_25_deg_up_to_flat;
        case TrackElemType::DiagDown25:
            return corkscrew_rc_track_diag_25_deg_down;
        case TrackElemType::DiagDown60:
            return corkscrew_rc_track_diag_60_deg_down;
        case TrackElemType::DiagFlatToDown25:
            return corkscrew_rc_track_diag_flat_to_25_deg_down;
        case TrackElemType::DiagDown25ToDown60:
            return corkscrew_rc_track_diag_25_deg_down_to_60_deg_down;
        case TrackElemType::DiagDown60ToDown25:
            return corkscrew_rc_track_diag_60_deg_down_to_25_deg_down;
        case TrackElemType::DiagDown25ToFlat:
            return corkscrew_rc_track_diag_25_deg_down_to_flat;
        case TrackElemType::DiagFlatToLeftBank:
            return corkscrew_rc_track_diag_flat_to_left_bank;
        case TrackElemType::DiagFlatToRightBank:
            return corkscrew_rc_track_diag_flat_to_right_bank;
        case TrackElemType::DiagLeftBankToFlat:
            return corkscrew_rc_track_diag_left_bank_to_flat;
        case TrackElemType::DiagRightBankToFlat:
            return corkscrew_rc_track_diag_right_bank_to_flat;
        case TrackElemType::DiagLeftBankToUp25:
            return corkscrew_rc_track_diag_left_bank_to_25_deg_up;
        case TrackElemType::DiagRightBankToUp25:
            return corkscrew_rc_track_diag_right_bank_to_25_deg_up;
        case TrackElemType::DiagUp25ToLeftBank:
            return corkscrew_rc_track_diag_25_deg_up_to_left_bank;
        case TrackElemType::DiagUp25ToRightBank:
            return corkscrew_rc_track_diag_25_deg_up_to_right_bank;
        case TrackElemType::DiagLeftBankToDown25:
            return corkscrew_rc_track_diag_left_bank_to_25_deg_down;
        case TrackElemType::DiagRightBankToDown25:
            return corkscrew_rc_track_diag_right_bank_to_25_deg_down;
        case TrackElemType::DiagDown25ToLeftBank:
            return corkscrew_rc_track_diag_25_deg_down_to_left_bank;
        case TrackElemType::DiagDown25ToRightBank:
            return corkscrew_rc_track_diag_25_deg_down_to_right_bank;
        case TrackElemType::DiagLeftBank:
            return corkscrew_rc_track_diag_left_bank;
        case TrackElemType::DiagRightBank:
            return corkscrew_rc_track_diag_right_bank;
        case TrackElemType::BlockBrakes:
            return corkscrew_rc_track_block_brakes;

        case TrackElemType::Booster:
            return corkscrew_rc_track_booster;
    }
    return nullptr;
}
