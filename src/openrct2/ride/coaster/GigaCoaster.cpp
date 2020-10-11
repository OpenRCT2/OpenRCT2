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

/** rct2: 0x008AD674 */
static void giga_rc_track_flat(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    if (tileElement->AsTrack()->HasCableLift())
    {
        switch (direction)
        {
            case 0:
            case 2:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 18692, 0, 0, 32, 20, 3, height, 0, 6, height);
                break;
            case 1:
            case 3:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 18693, 0, 0, 20, 32, 3, height, 6, 0, height);
                break;
        }
        if (track_paint_util_should_paint_supports(session->MapPosition))
        {
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
        }
    }
    else if (tileElement->AsTrack()->HasChain())
    {
        switch (direction)
        {
            case 0:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 18382, 0, 0, 32, 20, 3, height, 0, 6, height);
                break;
            case 1:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 18383, 0, 0, 32, 20, 3, height, 0, 6, height);
                break;
            case 2:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 18384, 0, 0, 32, 20, 3, height, 0, 6, height);
                break;
            case 3:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 18385, 0, 0, 32, 20, 3, height, 0, 6, height);
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
                    session, direction, session->TrackColours[SCHEME_TRACK] | 18074, 0, 0, 32, 20, 3, height, 0, 6, height);
                break;
            case 1:
            case 3:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 18075, 0, 0, 32, 20, 3, height, 0, 6, height);
                break;
        }
        if (track_paint_util_should_paint_supports(session->MapPosition))
        {
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
        }
    }
    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
    paint_util_set_segment_support_height(
        session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

static void giga_rc_track_station(
    paint_session* session, ride_id_t rideIndex, [[maybe_unused]] uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    static constexpr const uint32_t imageIds[4][3] = {
        { 18084, 18076, SPR_STATION_BASE_A_SW_NE },
        { 18085, 18077, SPR_STATION_BASE_A_NW_SE },
        { 18084, 18076, SPR_STATION_BASE_A_SW_NE },
        { 18085, 18077, SPR_STATION_BASE_A_NW_SE },
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

/** rct2: 0x008AD684 */
static void giga_rc_track_25_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    if (tileElement->AsTrack()->HasCableLift())
    {
        switch (direction)
        {
            case 0:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 18702, 0, 0, 32, 20, 3, height, 0, 6, height);
                break;
            case 1:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 18703, 0, 0, 32, 20, 3, height, 0, 6, height);
                break;
            case 2:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 18704, 0, 0, 32, 20, 3, height, 0, 6, height);
                break;
            case 3:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 18705, 0, 0, 32, 20, 3, height, 0, 6, height);
                break;
        }
        if (track_paint_util_should_paint_supports(session->MapPosition))
        {
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 8, height, session->TrackColours[SCHEME_SUPPORTS]);
        }
    }
    else if (tileElement->AsTrack()->HasChain())
    {
        switch (direction)
        {
            case 0:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 18394, 0, 0, 32, 20, 3, height, 0, 6, height);
                break;
            case 1:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 18395, 0, 0, 32, 20, 3, height, 0, 6, height);
                break;
            case 2:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 18396, 0, 0, 32, 20, 3, height, 0, 6, height);
                break;
            case 3:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 18397, 0, 0, 32, 20, 3, height, 0, 6, height);
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
                    session, direction, session->TrackColours[SCHEME_TRACK] | 18134, 0, 0, 32, 20, 3, height, 0, 6, height);
                break;
            case 1:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 18135, 0, 0, 32, 20, 3, height, 0, 6, height);
                break;
            case 2:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 18136, 0, 0, 32, 20, 3, height, 0, 6, height);
                break;
            case 3:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 18137, 0, 0, 32, 20, 3, height, 0, 6, height);
                break;
        }
        if (track_paint_util_should_paint_supports(session->MapPosition))
        {
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 8, height, session->TrackColours[SCHEME_SUPPORTS]);
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
    paint_util_set_segment_support_height(
        session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 56, 0x20);
}

/** rct2: 0x008AD694 */
static void giga_rc_track_60_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    if (tileElement->AsTrack()->HasCableLift())
    {
        switch (direction)
        {
            case 0:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 18718, 0, 0, 32, 20, 3, height, 0, 6, height);
                break;
            case 1:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 18719, 0, 0, 32, 1, 98, height, 0, 27, height);
                break;
            case 2:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 18720, 0, 0, 32, 1, 98, height, 0, 27, height);
                break;
            case 3:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 18721, 0, 0, 32, 20, 3, height, 0, 6, height);
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
                    session, direction, session->TrackColours[SCHEME_TRACK] | 18150, 0, 0, 32, 20, 3, height, 0, 6, height);
                break;
            case 1:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 18151, 0, 0, 32, 1, 98, height, 0, 27, height);
                break;
            case 2:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 18152, 0, 0, 32, 1, 98, height, 0, 27, height);
                break;
            case 3:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 18153, 0, 0, 32, 20, 3, height, 0, 6, height);
                break;
        }
        if (track_paint_util_should_paint_supports(session->MapPosition))
        {
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 32, height, session->TrackColours[SCHEME_SUPPORTS]);
        }
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

/** rct2: 0x008AD6A4 */
static void giga_rc_track_flat_to_25_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    if (tileElement->AsTrack()->HasCableLift())
    {
        switch (direction)
        {
            case 0:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 18694, 0, 0, 32, 20, 3, height, 0, 6, height);
                break;
            case 1:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 18695, 0, 0, 32, 20, 3, height, 0, 6, height);
                break;
            case 2:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 18696, 0, 0, 32, 20, 3, height, 0, 6, height);
                break;
            case 3:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 18697, 0, 0, 32, 20, 3, height, 0, 6, height);
                break;
        }
        if (track_paint_util_should_paint_supports(session->MapPosition))
        {
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 3, height, session->TrackColours[SCHEME_SUPPORTS]);
        }
    }
    else if (tileElement->AsTrack()->HasChain())
    {
        switch (direction)
        {
            case 0:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 18386, 0, 0, 32, 20, 3, height, 0, 6, height);
                break;
            case 1:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 18387, 0, 0, 32, 20, 3, height, 0, 6, height);
                break;
            case 2:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 18388, 0, 0, 32, 20, 3, height, 0, 6, height);
                break;
            case 3:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 18389, 0, 0, 32, 20, 3, height, 0, 6, height);
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
                    session, direction, session->TrackColours[SCHEME_TRACK] | 18126, 0, 0, 32, 20, 3, height, 0, 6, height);
                break;
            case 1:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 18127, 0, 0, 32, 20, 3, height, 0, 6, height);
                break;
            case 2:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 18128, 0, 0, 32, 20, 3, height, 0, 6, height);
                break;
            case 3:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 18129, 0, 0, 32, 20, 3, height, 0, 6, height);
                break;
        }
        if (track_paint_util_should_paint_supports(session->MapPosition))
        {
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 3, height, session->TrackColours[SCHEME_SUPPORTS]);
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
    paint_util_set_segment_support_height(
        session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 48, 0x20);
}

/** rct2: 0x008AD6B4 */
static void giga_rc_track_25_deg_up_to_60_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    if (tileElement->AsTrack()->HasCableLift())
    {
        switch (direction)
        {
            case 0:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 18706, 0, 0, 32, 20, 3, height, 0, 6, height);
                break;
            case 1:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 18707, 0, 0, 32, 20, 3, height, 0, 6, height);
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 18710, 0, 0, 32, 1, 66, height, 0, 27, height);
                break;
            case 2:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 18708, 0, 0, 32, 20, 3, height, 0, 6, height);
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 18711, 0, 0, 32, 1, 66, height, 0, 27, height);
                break;
            case 3:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 18709, 0, 0, 32, 20, 3, height, 0, 6, height);
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
                    session, direction, session->TrackColours[SCHEME_TRACK] | 18138, 0, 0, 32, 20, 3, height, 0, 6, height);
                break;
            case 1:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 18139, 0, 0, 32, 20, 3, height, 0, 6, height);
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 18142, 0, 0, 32, 1, 66, height, 0, 27, height);
                break;
            case 2:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 18140, 0, 0, 32, 20, 3, height, 0, 6, height);
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 18143, 0, 0, 32, 1, 66, height, 0, 27, height);
                break;
            case 3:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 18141, 0, 0, 32, 20, 3, height, 0, 6, height);
                break;
        }
        if (track_paint_util_should_paint_supports(session->MapPosition))
        {
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 12, height, session->TrackColours[SCHEME_SUPPORTS]);
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
    paint_util_set_segment_support_height(
        session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 72, 0x20);
}

/** rct2: 0x008AD6C4 */
static void giga_rc_track_60_deg_up_to_25_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    if (tileElement->AsTrack()->HasCableLift())
    {
        switch (direction)
        {
            case 0:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 18712, 0, 0, 32, 20, 3, height, 0, 6, height);
                break;
            case 1:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 18713, 0, 0, 32, 20, 3, height, 0, 6, height);
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 18716, 0, 0, 32, 1, 66, height, 0, 27, height);
                break;
            case 2:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 18714, 0, 0, 32, 20, 3, height, 0, 6, height);
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 18717, 0, 0, 32, 1, 66, height, 0, 27, height);
                break;
            case 3:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 18715, 0, 0, 32, 20, 3, height, 0, 6, height);
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
                    session, direction, session->TrackColours[SCHEME_TRACK] | 18144, 0, 0, 32, 20, 3, height, 0, 6, height);
                break;
            case 1:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 18145, 0, 0, 32, 20, 3, height, 0, 6, height);
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 18148, 0, 0, 32, 1, 66, height, 0, 27, height);
                break;
            case 2:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 18146, 0, 0, 32, 20, 3, height, 0, 6, height);
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 18149, 0, 0, 32, 1, 66, height, 0, 27, height);
                break;
            case 3:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 18147, 0, 0, 32, 20, 3, height, 0, 6, height);
                break;
        }
        if (track_paint_util_should_paint_supports(session->MapPosition))
        {
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 20, height, session->TrackColours[SCHEME_SUPPORTS]);
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
    paint_util_set_segment_support_height(
        session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 72, 0x20);
}

/** rct2: 0x008AD6D4 */
static void giga_rc_track_25_deg_up_to_flat(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    if (tileElement->AsTrack()->HasCableLift())
    {
        switch (direction)
        {
            case 0:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 18698, 0, 0, 32, 20, 3, height, 0, 6, height);
                break;
            case 1:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 18699, 0, 0, 32, 20, 3, height, 0, 6, height);
                break;
            case 2:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 18700, 0, 0, 32, 20, 3, height, 0, 6, height);
                break;
            case 3:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 18701, 0, 0, 32, 20, 3, height, 0, 6, height);
                break;
        }
        if (track_paint_util_should_paint_supports(session->MapPosition))
        {
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 6, height, session->TrackColours[SCHEME_SUPPORTS]);
        }
    }
    else if (tileElement->AsTrack()->HasChain())
    {
        switch (direction)
        {
            case 0:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 18390, 0, 0, 32, 20, 3, height, 0, 6, height);
                break;
            case 1:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 18391, 0, 0, 32, 20, 3, height, 0, 6, height);
                break;
            case 2:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 18392, 0, 0, 32, 20, 3, height, 0, 6, height);
                break;
            case 3:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 18393, 0, 0, 32, 20, 3, height, 0, 6, height);
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
                    session, direction, session->TrackColours[SCHEME_TRACK] | 18130, 0, 0, 32, 20, 3, height, 0, 6, height);
                break;
            case 1:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 18131, 0, 0, 32, 20, 3, height, 0, 6, height);
                break;
            case 2:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 18132, 0, 0, 32, 20, 3, height, 0, 6, height);
                break;
            case 3:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 18133, 0, 0, 32, 20, 3, height, 0, 6, height);
                break;
        }
        if (track_paint_util_should_paint_supports(session->MapPosition))
        {
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 6, height, session->TrackColours[SCHEME_SUPPORTS]);
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
    paint_util_set_segment_support_height(
        session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 40, 0x20);
}

/** rct2: 0x008AD6E4 */
static void giga_rc_track_25_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    giga_rc_track_25_deg_up(session, rideIndex, trackSequence, (direction + 2) & 3, height, tileElement);
}

/** rct2: 0x008AD6F4 */
static void giga_rc_track_60_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    giga_rc_track_60_deg_up(session, rideIndex, trackSequence, (direction + 2) & 3, height, tileElement);
}

/** rct2: 0x008AD704 */
static void giga_rc_track_flat_to_25_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    giga_rc_track_25_deg_up_to_flat(session, rideIndex, trackSequence, (direction + 2) & 3, height, tileElement);
}

/** rct2: 0x008AD714 */
static void giga_rc_track_25_deg_down_to_60_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    giga_rc_track_60_deg_up_to_25_deg_up(session, rideIndex, trackSequence, (direction + 2) & 3, height, tileElement);
}

/** rct2: 0x008AD724 */
static void giga_rc_track_60_deg_down_to_25_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    giga_rc_track_25_deg_up_to_60_deg_up(session, rideIndex, trackSequence, (direction + 2) & 3, height, tileElement);
}

/** rct2: 0x008AD734 */
static void giga_rc_track_25_deg_down_to_flat(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    giga_rc_track_flat_to_25_deg_up(session, rideIndex, trackSequence, (direction + 2) & 3, height, tileElement);
}

/** rct2: 0x008AD744 */
static void giga_rc_track_left_quarter_turn_5(
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18189, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18194, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18199, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18184, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
            }
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18188, 0, 0, 32, 16, 3, height, 0, 0, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18193, 0, 0, 32, 16, 3, height, 0, 0, height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18198, 0, 0, 32, 16, 3, height, 0, 16,
                        height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18183, 0, 0, 32, 16, 3, height, 0, 16,
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18187, 0, 0, 16, 16, 3, height, 0, 16,
                        height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18192, 0, 0, 16, 16, 3, height, 16, 16,
                        height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18197, 0, 0, 16, 16, 3, height, 16, 0,
                        height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18182, 0, 0, 16, 16, 3, height, 0, 0, height);
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18186, 0, 0, 16, 32, 3, height, 16, 0,
                        height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18191, 0, 0, 16, 32, 3, height, 0, 0, height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18196, 0, 0, 16, 32, 3, height, 0, 0, height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18181, 0, 0, 16, 32, 3, height, 16, 0,
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18185, 0, 0, 20, 32, 3, height, 6, 0, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18190, 0, 0, 20, 32, 3, height, 6, 0, height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18195, 0, 0, 20, 32, 3, height, 6, 0, height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18180, 0, 0, 20, 32, 3, height, 6, 0, height);
                    break;
            }
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
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
                session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008AD754 */
static void giga_rc_track_right_quarter_turn_5(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    giga_rc_track_left_quarter_turn_5(session, rideIndex, trackSequence, (direction - 1) & 3, height, tileElement);
}

/** rct2: 0x008AD764 */
static void giga_rc_track_flat_to_left_bank(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (direction)
    {
        case 0:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18086, 0, 0, 32, 20, 3, height, 0, 6, height);
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18094, 0, 0, 32, 1, 26, height, 0, 27, height);
            break;
        case 1:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18087, 0, 0, 32, 20, 3, height, 0, 6, height);
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18095, 0, 0, 32, 1, 26, height, 0, 27, height);
            break;
        case 2:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18088, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 3:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18089, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
    }
    if (track_paint_util_should_paint_supports(session->MapPosition))
    {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
    }
    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
    paint_util_set_segment_support_height(
        session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x008AD774 */
static void giga_rc_track_flat_to_right_bank(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (direction)
    {
        case 0:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18090, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 1:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18091, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 2:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18092, 0, 0, 32, 20, 3, height, 0, 6, height);
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18096, 0, 0, 32, 1, 26, height, 0, 27, height);
            break;
        case 3:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18093, 0, 0, 32, 20, 3, height, 0, 6, height);
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18097, 0, 0, 32, 1, 26, height, 0, 27, height);
            break;
    }
    if (track_paint_util_should_paint_supports(session->MapPosition))
    {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
    }
    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
    paint_util_set_segment_support_height(
        session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x008AD784 */
static void giga_rc_track_left_bank_to_flat(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (direction)
    {
        case 0:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18092, 0, 0, 32, 20, 3, height, 0, 6, height);
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18096, 0, 0, 32, 1, 26, height, 0, 27, height);
            break;
        case 1:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18093, 0, 0, 32, 20, 3, height, 0, 6, height);
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18097, 0, 0, 32, 1, 26, height, 0, 27, height);
            break;
        case 2:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18090, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 3:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18091, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
    }
    if (track_paint_util_should_paint_supports(session->MapPosition))
    {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
    }
    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
    paint_util_set_segment_support_height(
        session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x008AD794 */
static void giga_rc_track_right_bank_to_flat(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (direction)
    {
        case 0:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18088, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 1:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18089, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 2:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18086, 0, 0, 32, 20, 3, height, 0, 6, height);
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18094, 0, 0, 32, 1, 26, height, 0, 27, height);
            break;
        case 3:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18087, 0, 0, 32, 20, 3, height, 0, 6, height);
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18095, 0, 0, 32, 1, 26, height, 0, 27, height);
            break;
    }
    if (track_paint_util_should_paint_supports(session->MapPosition))
    {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
    }
    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
    paint_util_set_segment_support_height(
        session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x008AD7A4 */
static void giga_rc_track_banked_left_quarter_turn_5(
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18209, 0, 0, 32, 20, 3, height, 0, 6, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18220, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18214, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18219, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18204, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
            }
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18208, 0, 0, 32, 16, 3, height, 0, 0, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18213, 0, 0, 32, 16, 1, height, 0, 0,
                        height + 27);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18218, 0, 0, 32, 16, 3, height, 0, 16,
                        height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18203, 0, 0, 32, 16, 3, height, 0, 16,
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18207, 0, 0, 16, 16, 3, height, 0, 16,
                        height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18212, 0, 0, 16, 16, 1, height, 16, 16,
                        height + 27);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18217, 0, 0, 16, 16, 3, height, 16, 0,
                        height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18202, 0, 0, 16, 16, 3, height, 0, 0, height);
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18206, 0, 0, 16, 32, 3, height, 16, 0,
                        height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18211, 0, 0, 16, 32, 1, height, 0, 0,
                        height + 27);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18216, 0, 0, 16, 32, 3, height, 0, 0, height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18201, 0, 0, 16, 32, 3, height, 16, 0,
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18205, 0, 0, 20, 32, 3, height, 6, 0, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18210, 0, 0, 1, 32, 26, height, 27, 0,
                        height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18215, 0, 0, 20, 32, 3, height, 6, 0, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18221, 0, 0, 1, 32, 26, height, 27, 0,
                        height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18200, 0, 0, 20, 32, 3, height, 6, 0, height);
                    break;
            }
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
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
                session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008AD7B4 */
static void giga_rc_track_banked_right_quarter_turn_5(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    giga_rc_track_banked_left_quarter_turn_5(session, rideIndex, trackSequence, (direction - 1) & 3, height, tileElement);
}

/** rct2: 0x008AD7C4 */
static void giga_rc_track_left_bank_to_25_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (direction)
    {
        case 0:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18098, 0, 0, 32, 20, 3, height, 0, 6, height);
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18102, 0, 0, 32, 1, 34, height, 0, 27, height);
            break;
        case 1:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18099, 0, 0, 32, 20, 3, height, 0, 6, height);
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18103, 0, 0, 32, 1, 34, height, 0, 27, height);
            break;
        case 2:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18100, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 3:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18101, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
    }
    if (track_paint_util_should_paint_supports(session->MapPosition))
    {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 3, height, session->TrackColours[SCHEME_SUPPORTS]);
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

/** rct2: 0x008AD7D4 */
static void giga_rc_track_right_bank_to_25_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (direction)
    {
        case 0:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18104, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 1:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18105, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 2:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18106, 0, 0, 32, 20, 3, height, 0, 6, height);
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18108, 0, 0, 32, 1, 34, height, 0, 27, height);
            break;
        case 3:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18107, 0, 0, 32, 20, 3, height, 0, 6, height);
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18109, 0, 0, 32, 1, 34, height, 0, 27, height);
            break;
    }
    if (track_paint_util_should_paint_supports(session->MapPosition))
    {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 3, height, session->TrackColours[SCHEME_SUPPORTS]);
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

/** rct2: 0x008AD7E4 */
static void giga_rc_track_25_deg_up_to_left_bank(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (direction)
    {
        case 0:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18110, 0, 0, 32, 20, 3, height, 0, 6, height);
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18114, 0, 0, 32, 1, 34, height, 0, 27, height);
            break;
        case 1:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18111, 0, 0, 32, 20, 3, height, 0, 6, height);
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18115, 0, 0, 32, 1, 34, height, 0, 27, height);
            break;
        case 2:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18112, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 3:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18113, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
    }
    if (track_paint_util_should_paint_supports(session->MapPosition))
    {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 6, height, session->TrackColours[SCHEME_SUPPORTS]);
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

/** rct2: 0x008AD7F4 */
static void giga_rc_track_25_deg_up_to_right_bank(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (direction)
    {
        case 0:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18116, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 1:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18117, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 2:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18118, 0, 0, 32, 20, 3, height, 0, 6, height);
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18120, 0, 0, 32, 1, 34, height, 0, 27, height);
            break;
        case 3:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18119, 0, 0, 32, 20, 3, height, 0, 6, height);
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18121, 0, 0, 32, 1, 34, height, 0, 27, height);
            break;
    }
    if (track_paint_util_should_paint_supports(session->MapPosition))
    {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 6, height, session->TrackColours[SCHEME_SUPPORTS]);
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

/** rct2: 0x008AD804 */
static void giga_rc_track_left_bank_to_25_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    giga_rc_track_25_deg_up_to_right_bank(session, rideIndex, trackSequence, (direction + 2) & 3, height, tileElement);
}

/** rct2: 0x008AD814 */
static void giga_rc_track_right_bank_to_25_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    giga_rc_track_25_deg_up_to_left_bank(session, rideIndex, trackSequence, (direction + 2) & 3, height, tileElement);
}

/** rct2: 0x008AD824 */
static void giga_rc_track_25_deg_down_to_left_bank(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    giga_rc_track_right_bank_to_25_deg_up(session, rideIndex, trackSequence, (direction + 2) & 3, height, tileElement);
}

/** rct2: 0x008AD834 */
static void giga_rc_track_25_deg_down_to_right_bank(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    giga_rc_track_left_bank_to_25_deg_up(session, rideIndex, trackSequence, (direction + 2) & 3, height, tileElement);
}

/** rct2: 0x008AD844 */
static void giga_rc_track_left_bank(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (direction)
    {
        case 0:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18122, 0, 0, 32, 1, 26, height, 0, 27, height);
            break;
        case 1:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18123, 0, 0, 32, 1, 26, height, 0, 27, height);
            break;
        case 2:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18124, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 3:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18125, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
    }
    if (track_paint_util_should_paint_supports(session->MapPosition))
    {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
    }
    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
    paint_util_set_segment_support_height(
        session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x008AD854 */
static void giga_rc_track_right_bank(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    giga_rc_track_left_bank(session, rideIndex, trackSequence, (direction + 2) & 3, height, tileElement);
}

/** rct2: 0x008AD864 */
static void giga_rc_track_left_quarter_turn_5_25_deg_up(
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18274, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18279, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18284, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18289, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
            }
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 8, height, session->TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_SQUARE_7);
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18275, 0, 0, 32, 16, 3, height);
                    break;
                case 1:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18280, 0, 0, 32, 16, 3, height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18285, 0, 0, 32, 16, 3, height, 0, 16,
                        height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18290, 0, 0, 32, 16, 3, height, 0, 16,
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18276, 0, 0, 16, 16, 3, height, 0, 16,
                        height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18281, 0, 0, 16, 16, 3, height, 16, 16,
                        height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18286, 0, 0, 16, 16, 3, height, 16, 0,
                        height);
                    break;
                case 3:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18291, 0, 0, 16, 16, 3, height);
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18277, 0, 0, 16, 32, 3, height, 16, 0,
                        height);
                    break;
                case 1:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18282, 0, 0, 16, 32, 3, height);
                    break;
                case 2:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18287, 0, 0, 16, 32, 3, height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18292, 0, 0, 16, 32, 3, height, 16, 0,
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18278, 0, 0, 20, 32, 3, height, 6, 0, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18283, 0, 0, 20, 32, 3, height, 6, 0, height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18288, 0, 0, 20, 32, 3, height, 6, 0, height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18293, 0, 0, 20, 32, 3, height, 6, 0, height);
                    break;
            }
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 8, height, session->TrackColours[SCHEME_SUPPORTS]);
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
                session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 72, 0x20);
            break;
    }
}

/** rct2: 0x008AD874 */
static void giga_rc_track_right_quarter_turn_5_25_deg_up(
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18254, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18259, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18264, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18269, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
            }
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 8, height, session->TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_SQUARE_7);
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18255, 0, 0, 32, 16, 3, height, 0, 16,
                        height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18260, 0, 0, 32, 16, 3, height, 0, 16,
                        height);
                    break;
                case 2:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18265, 0, 0, 32, 16, 3, height);
                    break;
                case 3:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18270, 0, 0, 32, 16, 3, height);
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18256, 0, 0, 16, 16, 3, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18261, 0, 0, 16, 16, 3, height, 16, 0,
                        height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18266, 0, 0, 16, 16, 3, height, 16, 16,
                        height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18271, 0, 0, 16, 16, 3, height, 0, 16,
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18257, 0, 0, 16, 32, 3, height, 16, 0,
                        height);
                    break;
                case 1:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18262, 0, 0, 16, 32, 3, height);
                    break;
                case 2:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18267, 0, 0, 16, 32, 3, height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18272, 0, 0, 16, 32, 3, height, 16, 0,
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18258, 0, 0, 20, 32, 3, height, 6, 0, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18263, 0, 0, 20, 32, 3, height, 6, 0, height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18268, 0, 0, 20, 32, 3, height, 6, 0, height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18273, 0, 0, 20, 32, 3, height, 6, 0, height);
                    break;
            }
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 8, height, session->TrackColours[SCHEME_SUPPORTS]);
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
                session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 72, 0x20);
            break;
    }
}

/** rct2: 0x008AD884 */
static void giga_rc_track_left_quarter_turn_5_25_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    giga_rc_track_right_quarter_turn_5_25_deg_up(session, rideIndex, trackSequence, (direction + 1) & 3, height, tileElement);
}

/** rct2: 0x008AD894 */
static void giga_rc_track_right_quarter_turn_5_25_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    giga_rc_track_left_quarter_turn_5_25_deg_up(session, rideIndex, trackSequence, (direction - 1) & 3, height, tileElement);
}

/** rct2: 0x008AD8A4 */
static void giga_rc_track_s_bend_left(
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18238, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18242, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18241, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18245, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
            }
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18239, 0, 0, 32, 26, 3, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18243, 0, 0, 32, 26, 3, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 6, 1, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18240, 0, 0, 32, 26, 3, height, 0, 6, height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18244, 0, 0, 32, 26, 3, height, 0, 6, height);
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18240, 0, 0, 32, 26, 3, height, 0, 6, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18244, 0, 0, 32, 26, 3, height, 0, 6, height);
                    break;
                case 2:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18239, 0, 0, 32, 26, 3, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18243, 0, 0, 32, 26, 3, height);
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18241, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18245, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18238, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18242, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
            }
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
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
                session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008AD8B4 */
static void giga_rc_track_s_bend_right(
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18246, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18250, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18249, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18253, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
            }
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18247, 0, 0, 32, 26, 3, height, 0, 6, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 8, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18251, 0, 0, 32, 26, 3, height, 0, 6, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 7, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18248, 0, 0, 32, 26, 3, height);
                    break;
                case 3:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18252, 0, 0, 32, 26, 3, height);
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18248, 0, 0, 32, 26, 3, height);
                    break;
                case 1:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18252, 0, 0, 32, 26, 3, height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18247, 0, 0, 32, 26, 3, height, 0, 6, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 8, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18251, 0, 0, 32, 26, 3, height, 0, 6, height);
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18249, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18253, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18246, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18250, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
            }
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
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
                session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008AD8F4 */
static void giga_rc_track_left_quarter_turn_3(
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18159, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18162, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18165, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18156, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
            }
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18158, 0, 0, 16, 16, 3, height, 16, 0,
                        height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18161, 0, 0, 16, 16, 3, height, 0, 0, height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18164, 0, 0, 16, 16, 3, height, 0, 16,
                        height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18155, 0, 0, 16, 16, 3, height, 16, 16,
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18157, 0, 0, 20, 32, 3, height, 6, 0, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18160, 0, 0, 20, 32, 3, height, 6, 0, height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18163, 0, 0, 20, 32, 3, height, 6, 0, height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18154, 0, 0, 20, 32, 3, height, 6, 0, height);
                    break;
            }
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
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
                session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008AD904 */
static void giga_rc_track_right_quarter_turn_3(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    giga_rc_track_left_quarter_turn_3(session, rideIndex, trackSequence, (direction - 1) & 3, height, tileElement);
}

/** rct2: 0x008AD914 */
static void giga_rc_track_left_quarter_turn_3_bank(
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18171, 0, 0, 32, 20, 3, height, 0, 6, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18178, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18174, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18177, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18168, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
            }
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18170, 0, 0, 16, 16, 3, height, 16, 0,
                        height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18173, 0, 0, 16, 16, 1, height, 0, 0,
                        height + 27);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18176, 0, 0, 16, 16, 3, height, 0, 16,
                        height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18167, 0, 0, 16, 16, 3, height, 16, 16,
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18169, 0, 0, 20, 32, 3, height, 6, 0, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18172, 0, 0, 1, 32, 26, height, 27, 0,
                        height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18175, 0, 0, 20, 32, 3, height, 6, 0, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18179, 0, 0, 1, 32, 26, height, 27, 0,
                        height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18166, 0, 0, 20, 32, 3, height, 6, 0, height);
                    break;
            }
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
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
                session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008AD924 */
static void giga_rc_track_right_quarter_turn_3_bank(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    giga_rc_track_left_quarter_turn_3_bank(session, rideIndex, trackSequence, (direction - 1) & 3, height, tileElement);
}

/** rct2: 0x008AD934 */
static void giga_rc_track_left_quarter_turn_3_25_deg_up(
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18305, 0, 6, 32, 20, 3, height);
                    break;
                case 1:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18307, 0, 6, 32, 20, 3, height);
                    break;
                case 2:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18309, 0, 6, 32, 20, 3, height);
                    break;
                case 3:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18303, 0, 6, 32, 20, 3, height);
                    break;
            }
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 8, height, session->TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_SQUARE_7);
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18304, 6, 0, 20, 32, 3, height);
                    break;
                case 1:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18306, 6, 0, 20, 32, 3, height);
                    break;
                case 2:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18308, 6, 0, 20, 32, 3, height);
                    break;
                case 3:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18302, 6, 0, 20, 32, 3, height);
                    break;
            }
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 8, height, session->TrackColours[SCHEME_SUPPORTS]);
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
                session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 72, 0x20);
            break;
    }
}

/** rct2: 0x008AD944 */
static void giga_rc_track_right_quarter_turn_3_25_deg_up(
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18294, 0, 6, 32, 20, 3, height);
                    break;
                case 1:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18296, 0, 6, 32, 20, 3, height);
                    break;
                case 2:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18298, 0, 6, 32, 20, 3, height);
                    break;
                case 3:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18300, 0, 6, 32, 20, 3, height);
                    break;
            }
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 8, height, session->TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_SQUARE_7);
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18295, 6, 0, 20, 32, 3, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 4, 8, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18297, 6, 0, 20, 32, 3, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 4, 8, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18299, 6, 0, 20, 32, 3, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 4, 10, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18301, 6, 0, 20, 32, 3, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 4, 8, height, session->TrackColours[SCHEME_SUPPORTS]);
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
                session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 72, 0x20);
            break;
    }
}

/** rct2: 0x008AD954 */
static void giga_rc_track_left_quarter_turn_3_25_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    giga_rc_track_right_quarter_turn_3_25_deg_up(session, rideIndex, trackSequence, (direction + 1) & 3, height, tileElement);
}

/** rct2: 0x008AD964 */
static void giga_rc_track_right_quarter_turn_3_25_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    giga_rc_track_left_quarter_turn_3_25_deg_up(session, rideIndex, trackSequence, (direction - 1) & 3, height, tileElement);
}

/** rct2: 0x008AD974 */
static void giga_rc_track_left_half_banked_helix_up_small(
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18329, 0, 0, 32, 20, 3, height, 0, 6, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18336, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18332, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18335, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18326, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
            }
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 2, height, session->TrackColours[SCHEME_SUPPORTS]);
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
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18328, 0, 0, 16, 16, 3, height, 16, 0,
                        height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18331, 0, 0, 16, 16, 1, height, 0, 0,
                        height + 27);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18334, 0, 0, 16, 16, 3, height, 0, 16,
                        height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18325, 0, 0, 16, 16, 3, height, 16, 16,
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18327, 0, 0, 20, 32, 3, height, 6, 0, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18330, 0, 0, 1, 32, 26, height, 27, 0,
                        height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18333, 0, 0, 20, 32, 3, height, 6, 0, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18337, 0, 0, 1, 32, 26, height, 27, 0,
                        height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18324, 0, 0, 20, 32, 3, height, 6, 0, height);
                    break;
            }
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 6, height, session->TrackColours[SCHEME_SUPPORTS]);
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
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18326, 0, 0, 20, 32, 3, height, 6, 0, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18329, 0, 0, 20, 32, 3, height, 6, 0, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18336, 0, 0, 1, 32, 26, height, 27, 0,
                        height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18332, 0, 0, 1, 32, 26, height, 27, 0,
                        height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18335, 0, 0, 20, 32, 3, height, 6, 0, height);
                    break;
            }
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 2, height, session->TrackColours[SCHEME_SUPPORTS]);
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
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18325, 0, 0, 16, 16, 3, height, 16, 16,
                        height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18328, 0, 0, 16, 16, 3, height, 0, 16,
                        height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18331, 0, 0, 16, 16, 1, height, 0, 0,
                        height + 27);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18334, 0, 0, 16, 16, 3, height, 16, 0,
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18324, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18327, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18330, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18333, 0, 0, 32, 20, 3, height, 0, 6, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18337, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    break;
            }
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 6, height, session->TrackColours[SCHEME_SUPPORTS]);
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

/** rct2: 0x008AD984 */
static void giga_rc_track_right_half_banked_helix_up_small(
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18310, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18313, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18316, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18319, 0, 0, 32, 20, 3, height, 0, 6, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18323, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    break;
            }
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 2, height, session->TrackColours[SCHEME_SUPPORTS]);
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
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18311, 0, 0, 16, 16, 3, height, 16, 16,
                        height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18314, 0, 0, 16, 16, 3, height, 0, 16,
                        height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18317, 0, 0, 16, 16, 1, height, 0, 0,
                        height + 27);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18320, 0, 0, 16, 16, 3, height, 16, 0,
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18312, 0, 0, 20, 32, 3, height, 6, 0, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18315, 0, 0, 20, 32, 3, height, 6, 0, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18322, 0, 0, 1, 32, 26, height, 27, 0,
                        height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18318, 0, 0, 1, 32, 26, height, 27, 0,
                        height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18321, 0, 0, 20, 32, 3, height, 6, 0, height);
                    break;
            }
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 6, height, session->TrackColours[SCHEME_SUPPORTS]);
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
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18313, 0, 0, 20, 32, 3, height, 6, 0, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18316, 0, 0, 1, 32, 26, height, 27, 0,
                        height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18319, 0, 0, 20, 32, 3, height, 6, 0, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18323, 0, 0, 1, 32, 26, height, 27, 0,
                        height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18310, 0, 0, 20, 32, 3, height, 6, 0, height);
                    break;
            }
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 2, height, session->TrackColours[SCHEME_SUPPORTS]);
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
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18314, 0, 0, 16, 16, 3, height, 16, 0,
                        height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18317, 0, 0, 16, 16, 1, height, 0, 0,
                        height + 27);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18320, 0, 0, 16, 16, 3, height, 0, 16,
                        height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18311, 0, 0, 16, 16, 3, height, 16, 16,
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18315, 0, 0, 32, 20, 3, height, 0, 6, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18322, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18318, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18321, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18312, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
            }
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 6, height, session->TrackColours[SCHEME_SUPPORTS]);
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

/** rct2: 0x008AD994 */
static void giga_rc_track_left_half_banked_helix_down_small(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    if (trackSequence >= 4)
    {
        trackSequence -= 4;
        direction = (direction - 1) & 3;
    }
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    giga_rc_track_right_half_banked_helix_up_small(session, rideIndex, trackSequence, (direction + 1) & 3, height, tileElement);
}

/** rct2: 0x008AD9A4 */
static void giga_rc_track_right_half_banked_helix_down_small(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    if (trackSequence >= 4)
    {
        trackSequence -= 4;
        direction = (direction + 1) & 3;
    }
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    giga_rc_track_left_half_banked_helix_up_small(session, rideIndex, trackSequence, (direction - 1) & 3, height, tileElement);
}

/** rct2: 0x008AD9B4 */
static void giga_rc_track_left_half_banked_helix_up_large(
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18369, 0, 0, 32, 20, 3, height, 0, 6, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18380, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18374, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18379, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18364, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
            }
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 1, height, session->TrackColours[SCHEME_SUPPORTS]);
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
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18368, 0, 0, 32, 16, 3, height, 0, 0, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18373, 0, 0, 32, 16, 1, height, 0, 0,
                        height + 27);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18378, 0, 0, 32, 16, 3, height, 0, 16,
                        height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18363, 0, 0, 32, 16, 3, height, 0, 16,
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18367, 0, 0, 16, 16, 3, height, 0, 16,
                        height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18372, 0, 0, 16, 16, 1, height, 16, 16,
                        height + 27);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18377, 0, 0, 16, 16, 3, height, 16, 0,
                        height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18362, 0, 0, 16, 16, 3, height, 0, 0, height);
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18366, 0, 0, 16, 32, 3, height, 16, 0,
                        height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18371, 0, 0, 16, 32, 1, height, 0, 0,
                        height + 27);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18376, 0, 0, 16, 32, 3, height, 0, 0, height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18361, 0, 0, 16, 32, 3, height, 16, 0,
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18365, 0, 0, 20, 32, 3, height, 6, 0, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18370, 0, 0, 1, 32, 26, height, 27, 0,
                        height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18375, 0, 0, 20, 32, 3, height, 6, 0, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18381, 0, 0, 1, 32, 26, height, 27, 0,
                        height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18360, 0, 0, 20, 32, 3, height, 6, 0, height);
                    break;
            }
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 7, height, session->TrackColours[SCHEME_SUPPORTS]);
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
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18364, 0, 0, 20, 32, 3, height, 6, 0, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18369, 0, 0, 20, 32, 3, height, 6, 0, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18380, 0, 0, 1, 32, 26, height, 27, 0,
                        height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18374, 0, 0, 1, 32, 26, height, 27, 0,
                        height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18379, 0, 0, 20, 32, 3, height, 6, 0, height);
                    break;
            }
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 1, height, session->TrackColours[SCHEME_SUPPORTS]);
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
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18363, 0, 0, 16, 32, 3, height, 16, 0,
                        height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18368, 0, 0, 16, 32, 3, height, 0, 0, height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18373, 0, 0, 16, 32, 1, height, 0, 0,
                        height + 27);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18378, 0, 0, 16, 32, 3, height, 16, 0,
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18362, 0, 0, 16, 16, 3, height, 0, 0, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18367, 0, 0, 16, 16, 3, height, 16, 0,
                        height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18372, 0, 0, 16, 16, 1, height, 16, 16,
                        height + 27);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18377, 0, 0, 16, 16, 3, height, 0, 16,
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18361, 0, 0, 32, 16, 3, height, 0, 16,
                        height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18366, 0, 0, 32, 16, 3, height, 0, 16,
                        height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18371, 0, 0, 32, 16, 1, height, 0, 0,
                        height + 27);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18376, 0, 0, 32, 16, 3, height, 0, 0, height);
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18360, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18365, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18370, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18375, 0, 0, 32, 20, 3, height, 0, 6, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18381, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    break;
            }
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 7, height, session->TrackColours[SCHEME_SUPPORTS]);
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

/** rct2: 0x008AD9C4 */
static void giga_rc_track_right_half_banked_helix_up_large(
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18338, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18343, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18348, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18353, 0, 0, 32, 20, 3, height, 0, 6, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18359, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    break;
            }
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 1, height, session->TrackColours[SCHEME_SUPPORTS]);
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
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18339, 0, 0, 32, 16, 3, height, 0, 16,
                        height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18344, 0, 0, 32, 16, 3, height, 0, 16,
                        height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18349, 0, 0, 32, 16, 1, height, 0, 0,
                        height + 27);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18354, 0, 0, 32, 16, 3, height, 0, 0, height);
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18340, 0, 0, 16, 16, 3, height, 0, 0, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18345, 0, 0, 16, 16, 3, height, 16, 0,
                        height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18350, 0, 0, 16, 16, 1, height, 16, 16,
                        height + 27);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18355, 0, 0, 16, 16, 3, height, 0, 16,
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18341, 0, 0, 16, 32, 3, height, 16, 0,
                        height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18346, 0, 0, 16, 32, 3, height, 0, 0, height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18351, 0, 0, 16, 32, 1, height, 0, 0,
                        height + 27);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18356, 0, 0, 16, 32, 3, height, 16, 0,
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18342, 0, 0, 20, 32, 3, height, 6, 0, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 4, 7, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18347, 0, 0, 20, 32, 3, height, 6, 0, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18358, 0, 0, 1, 32, 26, height, 27, 0,
                        height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 4, 3, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18352, 0, 0, 1, 32, 26, height, 27, 0,
                        height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 4, 7, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18357, 0, 0, 20, 32, 3, height, 6, 0, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 4, 7, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
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
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18343, 0, 0, 20, 32, 3, height, 6, 0, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18348, 0, 0, 1, 32, 26, height, 27, 0,
                        height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18353, 0, 0, 20, 32, 3, height, 6, 0, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18359, 0, 0, 1, 32, 26, height, 27, 0,
                        height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18338, 0, 0, 20, 32, 3, height, 6, 0, height);
                    break;
            }
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 1, height, session->TrackColours[SCHEME_SUPPORTS]);
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
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18344, 0, 0, 16, 32, 3, height, 16, 0,
                        height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18349, 0, 0, 16, 32, 1, height, 0, 0,
                        height + 27);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18354, 0, 0, 16, 32, 3, height, 0, 0, height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18339, 0, 0, 16, 32, 3, height, 16, 0,
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18345, 0, 0, 16, 16, 3, height, 0, 16,
                        height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18350, 0, 0, 16, 16, 1, height, 16, 16,
                        height + 27);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18355, 0, 0, 16, 16, 3, height, 16, 0,
                        height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18340, 0, 0, 16, 16, 3, height, 0, 0, height);
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18346, 0, 0, 32, 16, 3, height, 0, 0, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18351, 0, 0, 32, 16, 1, height, 0, 0,
                        height + 27);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18356, 0, 0, 32, 16, 3, height, 0, 16,
                        height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18341, 0, 0, 32, 16, 3, height, 0, 16,
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18347, 0, 0, 32, 20, 3, height, 0, 6, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18358, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 4, 3, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18352, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 4, 7, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18357, 0, 0, 32, 20, 3, height, 0, 6, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 4, 7, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18342, 0, 0, 32, 20, 3, height, 0, 6, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 4, 7, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
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

/** rct2: 0x008AD9D4 */
static void giga_rc_track_left_half_banked_helix_down_large(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    if (trackSequence >= 7)
    {
        trackSequence -= 7;
        direction = (direction - 1) & 3;
    }
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    giga_rc_track_right_half_banked_helix_up_large(session, rideIndex, trackSequence, (direction + 1) & 3, height, tileElement);
}

/** rct2: 0x008AD9E4 */
static void giga_rc_track_right_half_banked_helix_down_large(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    if (trackSequence >= 7)
    {
        trackSequence -= 7;
        direction = (direction + 1) & 3;
    }
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    giga_rc_track_left_half_banked_helix_up_large(session, rideIndex, trackSequence, (direction - 1) & 3, height, tileElement);
}

/** rct2: 0x008ADA14 */
static void giga_rc_track_left_quarter_turn_1_60_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (direction)
    {
        case 0:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18231, 0, 0, 28, 28, 3, height, 2, 2, height);
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18235, 0, 0, 28, 28, 1, height, 2, 2, height + 99);
            break;
        case 1:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18232, 0, 0, 28, 28, 3, height, 2, 2, height);
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18236, 0, 0, 28, 28, 1, height, 2, 2, height + 99);
            break;
        case 2:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18233, 0, 0, 28, 28, 3, height, 2, 2, height);
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18237, 0, 0, 28, 28, 1, height, 2, 2, height + 99);
            break;
        case 3:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18230, 0, 0, 28, 28, 3, height, 2, 2, height);
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18234, 0, 0, 28, 28, 1, height, 2, 2, height + 99);
            break;
    }
    track_paint_util_left_quarter_turn_1_tile_tunnel(session, direction, height, -8, TUNNEL_SQUARE_7, +56, TUNNEL_SQUARE_8);
    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 104, 0x20);
}

/** rct2: 0x008AD9F4 */
static void giga_rc_track_right_quarter_turn_1_60_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (direction)
    {
        case 0:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18222, 0, 0, 28, 28, 3, height, 2, 2, height);
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18226, 0, 0, 28, 28, 1, height, 2, 2, height + 99);
            break;
        case 1:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18223, 0, 0, 28, 28, 3, height, 2, 2, height);
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18227, 0, 0, 28, 28, 1, height, 2, 2, height + 99);
            break;
        case 2:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18224, 0, 0, 28, 28, 3, height, 2, 2, height);
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18228, 0, 0, 28, 28, 1, height, 2, 2, height + 99);
            break;
        case 3:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18225, 0, 0, 28, 28, 3, height, 2, 2, height);
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18229, 0, 0, 28, 28, 1, height, 2, 2, height + 99);
            break;
    }
    track_paint_util_right_quarter_turn_1_tile_tunnel(session, direction, height, -8, TUNNEL_SQUARE_7, +56, TUNNEL_SQUARE_8);
    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 104, 0x20);
}

/** rct2: 0x008ADA04 */
static void giga_rc_track_left_quarter_turn_1_60_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    giga_rc_track_right_quarter_turn_1_60_deg_up(session, rideIndex, trackSequence, (direction + 1) & 3, height, tileElement);
}

/** rct2: 0x008ADA24 */
static void giga_rc_track_right_quarter_turn_1_60_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    giga_rc_track_left_quarter_turn_1_60_deg_up(session, rideIndex, trackSequence, (direction - 1) & 3, height, tileElement);
}

/** rct2: 0x008ADA34 */
static void giga_rc_track_brakes(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (direction)
    {
        case 0:
        case 2:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18078, 0, 0, 32, 20, 3, height, 0, 6, height);
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18082, 0, 0, 32, 1, 11, height, 0, 27, height + 5);
            break;
        case 1:
        case 3:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18079, 0, 0, 32, 20, 3, height, 0, 6, height);
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18083, 0, 0, 32, 1, 11, height, 0, 27, height + 5);
            break;
    }
    if (track_paint_util_should_paint_supports(session->MapPosition))
    {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
    }
    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
    paint_util_set_segment_support_height(
        session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x008ADC84 */
static void giga_rc_track_25_deg_up_left_banked(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (direction)
    {
        case 0:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18560, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 1:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18561, 0, 0, 32, 1, 34, height, 0, 27, height);
            break;
        case 2:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18562, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 3:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18563, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
    }
    if (track_paint_util_should_paint_supports(session->MapPosition))
    {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 8, height, session->TrackColours[SCHEME_SUPPORTS]);
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

/** rct2: 0x008ADC94 */
static void giga_rc_track_25_deg_up_right_banked(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (direction)
    {
        case 0:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18564, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 1:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18565, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 2:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18566, 0, 0, 32, 1, 34, height, 0, 27, height);
            break;
        case 3:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18567, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
    }
    if (track_paint_util_should_paint_supports(session->MapPosition))
    {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 8, height, session->TrackColours[SCHEME_SUPPORTS]);
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

/** rct2: 0x008ADA44 */
static void giga_rc_track_on_ride_photo(
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
                session, direction, session->TrackColours[SCHEME_TRACK] | 18074, 0, 0, 32, 20, 0, height, 0, 6, height + 3);
            break;
        case 1:
            sub_98196C_rotated(session, direction, IMAGE_TYPE_REMAP | SPR_STATION_BASE_D, 0, 0, 32, 32, 1, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 6, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 7, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18075, 0, 0, 32, 20, 0, height, 0, 6, height + 3);
            break;
        case 2:
            sub_98196C_rotated(session, direction, IMAGE_TYPE_REMAP | SPR_STATION_BASE_D, 0, 0, 32, 32, 1, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 8, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18074, 0, 0, 32, 20, 0, height, 0, 6, height + 3);
            break;
        case 3:
            sub_98196C_rotated(session, direction, IMAGE_TYPE_REMAP | SPR_STATION_BASE_D, 0, 0, 32, 32, 1, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 6, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 7, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18075, 0, 0, 32, 20, 0, height, 0, 6, height + 3);
            break;
    }
    track_paint_util_onride_photo_paint(session, direction, height + 3, tileElement);
    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 48, 0x20);
}

/** rct2: 0x008ADCA4 */
static void giga_rc_track_25_deg_down_left_banked(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    giga_rc_track_25_deg_up_right_banked(session, rideIndex, trackSequence, (direction + 2) & 3, height, tileElement);
}

/** rct2: 0x008ADCB4 */
static void giga_rc_track_25_deg_down_right_banked(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    giga_rc_track_25_deg_up_left_banked(session, rideIndex, trackSequence, (direction + 2) & 3, height, tileElement);
}

/** rct2: 0x008ADED4 */
static void giga_rc_track_flat_to_60_deg_up_long_base(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (trackSequence)
    {
        case 0:
            if (tileElement->AsTrack()->HasCableLift())
            {
                switch (direction)
                {
                    case 0:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 18722, 0, 0, 32, 20, 3, height, 0, 6,
                            height);
                        break;
                    case 1:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 18726, 0, 0, 32, 20, 3, height, 0, 6,
                            height);
                        break;
                    case 2:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 18730, 0, 0, 32, 20, 3, height, 0, 6,
                            height);
                        break;
                    case 3:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 18734, 0, 0, 32, 20, 3, height, 0, 6,
                            height);
                        break;
                }
                if (track_paint_util_should_paint_supports(session->MapPosition))
                {
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 4, 3, height, session->TrackColours[SCHEME_SUPPORTS]);
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 18660, 0, 0, 32, 20, 3, height, 0, 6,
                            height);
                        break;
                    case 1:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 18664, 0, 0, 32, 20, 3, height, 0, 6,
                            height);
                        break;
                    case 2:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 18668, 0, 0, 32, 20, 3, height, 0, 6,
                            height);
                        break;
                    case 3:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 18672, 0, 0, 32, 20, 3, height, 0, 6,
                            height);
                        break;
                }
                if (track_paint_util_should_paint_supports(session->MapPosition))
                {
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 4, 3, height, session->TrackColours[SCHEME_SUPPORTS]);
                }
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
            if (tileElement->AsTrack()->HasCableLift())
            {
                switch (direction)
                {
                    case 0:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 18723, 0, 0, 32, 20, 3, height, 0, 6,
                            height);
                        break;
                    case 1:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 18727, 0, 0, 32, 20, 3, height, 0, 6,
                            height);
                        break;
                    case 2:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 18731, 0, 0, 32, 20, 3, height, 0, 6,
                            height);
                        break;
                    case 3:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 18735, 0, 0, 32, 20, 3, height, 0, 6,
                            height);
                        break;
                }
                if (track_paint_util_should_paint_supports(session->MapPosition))
                {
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 4, 5, height, session->TrackColours[SCHEME_SUPPORTS]);
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 18661, 0, 0, 32, 20, 3, height, 0, 6,
                            height);
                        break;
                    case 1:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 18665, 0, 0, 32, 20, 3, height, 0, 6,
                            height);
                        break;
                    case 2:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 18669, 0, 0, 32, 20, 3, height, 0, 6,
                            height);
                        break;
                    case 3:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 18673, 0, 0, 32, 20, 3, height, 0, 6,
                            height);
                        break;
                }
                if (track_paint_util_should_paint_supports(session->MapPosition))
                {
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 4, 5, height, session->TrackColours[SCHEME_SUPPORTS]);
                }
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
        case 2:
            if (tileElement->AsTrack()->HasCableLift())
            {
                switch (direction)
                {
                    case 0:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 18724, 0, 0, 32, 20, 3, height, 0, 6,
                            height);
                        break;
                    case 1:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 18728, 0, 0, 32, 20, 3, height, 0, 6,
                            height);
                        break;
                    case 2:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 18732, 0, 0, 32, 20, 3, height, 0, 6,
                            height);
                        break;
                    case 3:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 18736, 0, 0, 32, 20, 3, height, 0, 6,
                            height);
                        break;
                }
                if (track_paint_util_should_paint_supports(session->MapPosition))
                {
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 4, 7, height, session->TrackColours[SCHEME_SUPPORTS]);
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 18662, 0, 0, 32, 20, 3, height, 0, 6,
                            height);
                        break;
                    case 1:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 18666, 0, 0, 32, 20, 3, height, 0, 6,
                            height);
                        break;
                    case 2:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 18670, 0, 0, 32, 20, 3, height, 0, 6,
                            height);
                        break;
                    case 3:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 18674, 0, 0, 32, 20, 3, height, 0, 6,
                            height);
                        break;
                }
                if (track_paint_util_should_paint_supports(session->MapPosition))
                {
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 4, 7, height, session->TrackColours[SCHEME_SUPPORTS]);
                }
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 64, 0x20);
            break;
        case 3:
            if (tileElement->AsTrack()->HasCableLift())
            {
                switch (direction)
                {
                    case 0:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 18725, 0, 0, 32, 20, 3, height, 0, 6,
                            height);
                        break;
                    case 1:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 18729, 0, 0, 32, 1, 98, height, 0, 27,
                            height);
                        break;
                    case 2:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 18733, 0, 0, 32, 1, 98, height, 0, 27,
                            height);
                        break;
                    case 3:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 18737, 0, 0, 32, 20, 3, height, 0, 6,
                            height);
                        break;
                }
                if (track_paint_util_should_paint_supports(session->MapPosition))
                {
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 4, 12, height, session->TrackColours[SCHEME_SUPPORTS]);
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 18663, 0, 0, 32, 20, 3, height, 0, 6,
                            height);
                        break;
                    case 1:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 18667, 0, 0, 32, 1, 98, height, 0, 27,
                            height);
                        break;
                    case 2:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 18671, 0, 0, 32, 1, 98, height, 0, 27,
                            height);
                        break;
                    case 3:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 18675, 0, 0, 32, 20, 3, height, 0, 6,
                            height);
                        break;
                }
                if (track_paint_util_should_paint_supports(session->MapPosition))
                {
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 4, 12, height, session->TrackColours[SCHEME_SUPPORTS]);
                }
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

/** rct2: 0x008ADEE4 */
static void giga_rc_track_60_deg_up_to_flat_long_base(
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18676, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18680, 0, 0, 32, 1, 98, height, 0, 27,
                        height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18684, 0, 0, 32, 1, 98, height, 0, 27,
                        height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18688, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
            }
            if (track_paint_util_should_paint_supports(session->MapPosition))
            {
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES, 4, 16, height, session->TrackColours[SCHEME_SUPPORTS]);
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18677, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18681, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18685, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18689, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
            }
            if (track_paint_util_should_paint_supports(session->MapPosition))
            {
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES, 4, 12, height, session->TrackColours[SCHEME_SUPPORTS]);
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18678, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18682, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18686, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18690, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
            }
            if (track_paint_util_should_paint_supports(session->MapPosition))
            {
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES, 4, 9, height, session->TrackColours[SCHEME_SUPPORTS]);
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18679, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18683, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18687, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18691, 0, 0, 32, 20, 3, height, 0, 6, height);
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

/** rct2: 0x008ADEF4 */
static void giga_rc_track_60_deg_down_to_flat_long_base(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    giga_rc_track_flat_to_60_deg_up_long_base(session, rideIndex, 3 - trackSequence, (direction + 2) & 3, height, tileElement);
}

/** rct2: 0x008ADF04 */
static void giga_rc_track_flat_to_60_deg_down_long_base(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    giga_rc_track_60_deg_up_to_flat_long_base(session, rideIndex, 3 - trackSequence, (direction + 2) & 3, height, tileElement);
}

/** rct2: 0x008ADF14 */
static void giga_rc_track_cable_lift_hill(
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18698, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18699, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18700, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18701, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
            }
            if (track_paint_util_should_paint_supports(session->MapPosition))
            {
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES, 4, 6, height, session->TrackColours[SCHEME_SUPPORTS]);
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
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18700, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18701, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18698, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18699, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
            }
            if (track_paint_util_should_paint_supports(session->MapPosition))
            {
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES, 4, 6, height, session->TrackColours[SCHEME_SUPPORTS]);
            }
            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height + 8, TUNNEL_SQUARE_FLAT);
            }
            else
            {
                paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_SQUARE_FLAT);
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 40, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18714, 0, 0, 32, 20, 3, height, 0, 6, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18717, 0, 0, 32, 1, 66, height, 0, 27,
                        height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18715, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18712, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18713, 0, 0, 32, 20, 3, height, 0, 6, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18716, 0, 0, 32, 1, 66, height, 0, 27,
                        height);
                    break;
            }
            if (track_paint_util_should_paint_supports(session->MapPosition))
            {
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES, 4, 20, height, session->TrackColours[SCHEME_SUPPORTS]);
            }
            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height + 24, TUNNEL_SQUARE_8);
            }
            else
            {
                paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_SQUARE_7);
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 72, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18720, 0, 0, 32, 1, 98, height, 0, 27,
                        height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18721, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18718, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18719, 0, 0, 32, 1, 98, height, 0, 27,
                        height);
                    break;
            }
            if (track_paint_util_should_paint_supports(session->MapPosition))
            {
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES, 4, 32, height, session->TrackColours[SCHEME_SUPPORTS]);
            }
            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height + 56, TUNNEL_SQUARE_8);
            }
            else
            {
                paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_SQUARE_7);
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 104, 0x20);
            break;
    }
}

/** rct2: 0x008ADA64 */
static void giga_rc_track_left_eighth_to_diag(
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18414, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18418, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18422, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18426, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
            }
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
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
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18415, 0, 0, 32, 16, 3, height, 0, 0, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18419, 0, 0, 34, 16, 3, height, 0, 0, height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18423, 0, 0, 32, 16, 3, height, 0, 16,
                        height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18427, 0, 0, 32, 16, 3, height, 0, 16,
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18416, 0, 0, 16, 16, 3, height, 0, 16,
                        height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18420, 0, 0, 16, 16, 3, height, 16, 16,
                        height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18424, 0, 0, 16, 16, 3, height, 16, 0,
                        height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18428, 0, 0, 16, 16, 3, height, 0, 0, height);
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18417, 0, 0, 16, 16, 3, height, 16, 16,
                        height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18421, 0, 0, 16, 18, 3, height, 0, 16,
                        height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18425, 0, 0, 16, 16, 3, height, 0, 0, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18429, 0, 0, 16, 16, 3, height, 16, 0,
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

/** rct2: 0x008ADA74 */
static void giga_rc_track_right_eighth_to_diag(
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18398, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18402, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18406, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18410, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
            }
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
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
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18399, 0, 0, 32, 16, 3, height, 0, 16,
                        height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18403, 0, 0, 32, 16, 3, height, 0, 16,
                        height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18407, 0, 0, 34, 16, 3, height, 0, 0, height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18411, 0, 0, 32, 16, 3, height, 0, 0, height);
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18400, 0, 0, 16, 16, 3, height, 0, 0, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18404, 0, 0, 16, 16, 3, height, 16, 0,
                        height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18408, 0, 0, 28, 28, 3, height, 4, 4, height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18412, 0, 0, 16, 16, 3, height, 0, 16,
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18401, 0, 0, 16, 16, 3, height, 16, 0,
                        height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18405, 0, 0, 16, 16, 3, height, 0, 0, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18409, 0, 0, 16, 18, 3, height, 0, 16,
                        height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18413, 0, 0, 16, 16, 3, height, 16, 16,
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

/** rct2: 0x008ADA84 */
static void giga_rc_track_left_eighth_to_orthogonal(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    giga_rc_track_right_eighth_to_diag(session, rideIndex, trackSequence, (direction + 2) & 3, height, tileElement);
}

/** rct2: 0x008ADA94 */
static void giga_rc_track_right_eighth_to_orthogonal(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    giga_rc_track_left_eighth_to_diag(session, rideIndex, trackSequence, (direction + 3) & 3, height, tileElement);
}

/** rct2: 0x008ADAA4 */
static void giga_rc_track_left_eighth_bank_to_diag(
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18446, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18450, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18454, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18458, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
            }
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
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
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18447, 0, 0, 32, 16, 3, height, 0, 0, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18451, 0, 0, 34, 16, 0, height, 0, 0,
                        height + 27);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18455, 0, 0, 32, 16, 3, height, 0, 16,
                        height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18459, 0, 0, 32, 16, 3, height, 0, 16,
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18448, 0, 0, 16, 16, 3, height, 0, 16,
                        height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18452, 0, 0, 16, 16, 0, height, 16, 16,
                        height + 27);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18456, 0, 0, 16, 16, 3, height, 16, 0,
                        height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18460, 0, 0, 16, 16, 3, height, 0, 0, height);
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18449, 0, 0, 16, 16, 3, height, 16, 16,
                        height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18453, 0, 0, 16, 18, 0, height, 0, 16,
                        height + 27);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18457, 0, 0, 16, 16, 3, height, 0, 0, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18461, 0, 0, 16, 16, 3, height, 16, 0,
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

/** rct2: 0x008ADAB4 */
static void giga_rc_track_right_eighth_bank_to_diag(
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18430, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18434, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18438, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18442, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    break;
            }
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
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
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18431, 0, 0, 32, 16, 3, height, 0, 16,
                        height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18435, 0, 0, 32, 16, 3, height, 0, 16,
                        height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18439, 0, 0, 34, 16, 0, height, 0, 0,
                        height + 27);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18443, 0, 0, 32, 16, 3, height, 0, 0, height);
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18432, 0, 0, 16, 16, 3, height, 0, 0, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18436, 0, 0, 16, 16, 3, height, 16, 0,
                        height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18440, 0, 0, 28, 28, 0, height, 4, 4,
                        height + 27);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18444, 0, 0, 16, 16, 3, height, 0, 16,
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18433, 0, 0, 16, 16, 3, height, 16, 0,
                        height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18437, 0, 0, 16, 16, 3, height, 0, 0, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18441, 0, 0, 16, 18, 0, height, 0, 16,
                        height + 27);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18445, 0, 0, 16, 16, 3, height, 16, 16,
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

/** rct2: 0x008ADAC4 */
static void giga_rc_track_left_eighth_bank_to_orthogonal(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    giga_rc_track_right_eighth_bank_to_diag(session, rideIndex, trackSequence, (direction + 2) & 3, height, tileElement);
}

/** rct2: 0x008ADAD4 */
static void giga_rc_track_right_eighth_bank_to_orthogonal(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    giga_rc_track_left_eighth_bank_to_diag(session, rideIndex, trackSequence, (direction + 3) & 3, height, tileElement);
}

/** rct2: 0x008ADA54 */
static void giga_rc_track_diag_flat(
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
                            session, direction, session->TrackColours[SCHEME_TRACK] | 18527, -16, -16, 32, 32, 3, height, -16,
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
                            session, direction, session->TrackColours[SCHEME_TRACK] | 18465, -16, -16, 32, 32, 3, height, -16,
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
                            session, direction, session->TrackColours[SCHEME_TRACK] | 18524, -16, -16, 32, 32, 3, height, -16,
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
                            session, direction, session->TrackColours[SCHEME_TRACK] | 18462, -16, -16, 32, 32, 3, height, -16,
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
                            session, direction, session->TrackColours[SCHEME_TRACK] | 18526, -16, -16, 32, 32, 3, height, -16,
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
                            session, direction, session->TrackColours[SCHEME_TRACK] | 18464, -16, -16, 32, 32, 3, height, -16,
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
                            session, direction, session->TrackColours[SCHEME_TRACK] | 18525, -16, -16, 32, 32, 3, height, -16,
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
                            session, direction, session->TrackColours[SCHEME_TRACK] | 18463, -16, -16, 32, 32, 3, height, -16,
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

/** rct2: 0x008ADB04 */
static void giga_rc_track_diag_25_deg_up(
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
                            session, direction, session->TrackColours[SCHEME_TRACK] | 18539, -16, -16, 32, 32, 3, height, -16,
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
                            session, direction, session->TrackColours[SCHEME_TRACK] | 18477, -16, -16, 32, 32, 3, height, -16,
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
                            session, direction, session->TrackColours[SCHEME_TRACK] | 18536, -16, -16, 32, 32, 3, height, -16,
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
                            session, direction, session->TrackColours[SCHEME_TRACK] | 18474, -16, -16, 32, 32, 3, height, -16,
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
                            session, direction, session->TrackColours[SCHEME_TRACK] | 18538, -16, -16, 32, 32, 3, height, -16,
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
                            session, direction, session->TrackColours[SCHEME_TRACK] | 18476, -16, -16, 32, 32, 3, height, -16,
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
                            session, direction, session->TrackColours[SCHEME_TRACK] | 18537, -16, -16, 32, 32, 3, height, -16,
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
                            session, direction, session->TrackColours[SCHEME_TRACK] | 18475, -16, -16, 32, 32, 3, height, -16,
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

/** rct2: 0x008ADB64 */
static void giga_rc_track_diag_60_deg_up(
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18489, -16, -16, 32, 32, 3, height, -16, -16,
                        height);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 104, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18486, -16, -16, 32, 32, 3, height, -16, -16,
                        height);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 104, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18488, -16, -16, 32, 32, 3, height, -16, -16,
                        height);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 104, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    metal_b_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 1, 32, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18487, -16, -16, 32, 32, 3, height, -16, -16,
                        height);
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
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 104, 0x20);
            break;
    }
}

/** rct2: 0x008ADAE4 */
static void giga_rc_track_diag_flat_to_25_deg_up(
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
                            session, direction, session->TrackColours[SCHEME_TRACK] | 18531, -16, -16, 32, 32, 3, height, -16,
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
                            session, direction, session->TrackColours[SCHEME_TRACK] | 18469, -16, -16, 32, 32, 3, height, -16,
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
                            session, direction, session->TrackColours[SCHEME_TRACK] | 18528, -16, -16, 32, 32, 3, height, -16,
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
                            session, direction, session->TrackColours[SCHEME_TRACK] | 18466, -16, -16, 32, 32, 3, height, -16,
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
                            session, direction, session->TrackColours[SCHEME_TRACK] | 18530, -16, -16, 32, 32, 3, height, -16,
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
                            session, direction, session->TrackColours[SCHEME_TRACK] | 18468, -16, -16, 32, 32, 3, height, -16,
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
                            session, direction, session->TrackColours[SCHEME_TRACK] | 18529, -16, -16, 32, 32, 3, height, -16,
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
                            session, direction, session->TrackColours[SCHEME_TRACK] | 18467, -16, -16, 32, 32, 3, height, -16,
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

/** rct2: 0x008ADB44 */
static void giga_rc_track_diag_25_deg_up_to_60_deg_up(
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18481, -16, -16, 32, 32, 3, height, -16, -16,
                        height);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 72, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18478, -16, -16, 32, 32, 3, height, -16, -16,
                        height);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 72, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18480, -16, -16, 32, 32, 3, height, -16, -16,
                        height);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 72, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    metal_b_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 1, 16, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18479, -16, -16, 32, 32, 3, height, -16, -16,
                        height);
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
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 72, 0x20);
            break;
    }
}

/** rct2: 0x008ADB54 */
static void giga_rc_track_diag_60_deg_up_to_25_deg_up(
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18485, -16, -16, 32, 32, 3, height, -16, -16,
                        height);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 72, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18482, -16, -16, 32, 32, 3, height, -16, -16,
                        height);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 72, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18484, -16, -16, 32, 32, 3, height, -16, -16,
                        height);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 72, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    metal_b_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 1, 21, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18483, -16, -16, 16, 16, 3, height, 0, 0,
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
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 72, 0x20);
            break;
    }
}

/** rct2: 0x008ADAF4 */
static void giga_rc_track_diag_25_deg_up_to_flat(
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
                            session, direction, session->TrackColours[SCHEME_TRACK] | 18535, -16, -16, 32, 32, 3, height, -16,
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
                            session, direction, session->TrackColours[SCHEME_TRACK] | 18473, -16, -16, 32, 32, 3, height, -16,
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
                            session, direction, session->TrackColours[SCHEME_TRACK] | 18532, -16, -16, 32, 32, 3, height, -16,
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
                            session, direction, session->TrackColours[SCHEME_TRACK] | 18470, -16, -16, 32, 32, 3, height, -16,
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
                            session, direction, session->TrackColours[SCHEME_TRACK] | 18534, -16, -16, 32, 32, 3, height, -16,
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
                            session, direction, session->TrackColours[SCHEME_TRACK] | 18472, -16, -16, 32, 32, 3, height, -16,
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
                            session, direction, session->TrackColours[SCHEME_TRACK] | 18533, -16, -16, 32, 32, 3, height, -16,
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
                            session, direction, session->TrackColours[SCHEME_TRACK] | 18471, -16, -16, 32, 32, 3, height, -16,
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

/** rct2: 0x008ADB34 */
static void giga_rc_track_diag_25_deg_down(
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
                            session, direction, session->TrackColours[SCHEME_TRACK] | 18537, -16, -16, 32, 32, 3, height, -16,
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
                            session, direction, session->TrackColours[SCHEME_TRACK] | 18475, -16, -16, 32, 32, 3, height, -16,
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
                            session, direction, session->TrackColours[SCHEME_TRACK] | 18538, -16, -16, 32, 32, 3, height, -16,
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
                            session, direction, session->TrackColours[SCHEME_TRACK] | 18476, -16, -16, 32, 32, 3, height, -16,
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
                            session, direction, session->TrackColours[SCHEME_TRACK] | 18536, -16, -16, 32, 32, 3, height, -16,
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
                            session, direction, session->TrackColours[SCHEME_TRACK] | 18474, -16, -16, 32, 32, 3, height, -16,
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
                            session, direction, session->TrackColours[SCHEME_TRACK] | 18539, -16, -16, 32, 32, 3, height, -16,
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
                            session, direction, session->TrackColours[SCHEME_TRACK] | 18477, -16, -16, 32, 32, 3, height, -16,
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

/** rct2: 0x008ADB94 */
static void giga_rc_track_diag_60_deg_down(
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18487, -16, -16, 32, 32, 3, height, -16, -16,
                        height);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 104, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18488, -16, -16, 32, 32, 3, height, -16, -16,
                        height);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 104, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18486, -16, -16, 32, 32, 3, height, -16, -16,
                        height);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 104, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    metal_b_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 1, 24, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18489, -16, -16, 32, 32, 3, height, -16, -16,
                        height);
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
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 104, 0x20);
            break;
    }
}

/** rct2: 0x008ADB14 */
static void giga_rc_track_diag_flat_to_25_deg_down(
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
                            session, direction, session->TrackColours[SCHEME_TRACK] | 18533, -16, -16, 32, 32, 3, height, -16,
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
                            session, direction, session->TrackColours[SCHEME_TRACK] | 18471, -16, -16, 32, 32, 3, height, -16,
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
                            session, direction, session->TrackColours[SCHEME_TRACK] | 18534, -16, -16, 32, 32, 3, height, -16,
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
                            session, direction, session->TrackColours[SCHEME_TRACK] | 18472, -16, -16, 32, 32, 3, height, -16,
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
                            session, direction, session->TrackColours[SCHEME_TRACK] | 18532, -16, -16, 32, 32, 3, height, -16,
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
                            session, direction, session->TrackColours[SCHEME_TRACK] | 18470, -16, -16, 32, 32, 3, height, -16,
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
                            session, direction, session->TrackColours[SCHEME_TRACK] | 18535, -16, -16, 32, 32, 3, height, -16,
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
                            session, direction, session->TrackColours[SCHEME_TRACK] | 18473, -16, -16, 32, 32, 3, height, -16,
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

/** rct2: 0x008ADB74 */
static void giga_rc_track_diag_25_deg_down_to_60_deg_down(
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18483, -16, -16, 16, 16, 3, height, 0, 0,
                        height);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 72, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18484, -16, -16, 32, 32, 3, height, -16, -16,
                        height);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 72, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18482, -16, -16, 32, 32, 3, height, -16, -16,
                        height);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 72, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    metal_b_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 1, 17, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18485, -16, -16, 32, 32, 3, height, -16, -16,
                        height);
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
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 72, 0x20);
            break;
    }
}

/** rct2: 0x008ADB84 */
static void giga_rc_track_diag_60_deg_down_to_25_deg_down(
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18479, -16, -16, 32, 32, 3, height, -16, -16,
                        height);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 72, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18480, -16, -16, 32, 32, 3, height, -16, -16,
                        height);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 72, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18478, -16, -16, 32, 32, 3, height, -16, -16,
                        height);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 72, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    metal_b_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 1, 8, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18481, -16, -16, 32, 32, 3, height, -16, -16,
                        height);
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
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 72, 0x20);
            break;
    }
}

/** rct2: 0x008ADB24 */
static void giga_rc_track_diag_25_deg_down_to_flat(
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
                            session, direction, session->TrackColours[SCHEME_TRACK] | 18529, -16, -16, 32, 32, 3, height, -16,
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
                            session, direction, session->TrackColours[SCHEME_TRACK] | 18467, -16, -16, 32, 32, 3, height, -16,
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
                            session, direction, session->TrackColours[SCHEME_TRACK] | 18530, -16, -16, 32, 32, 3, height, -16,
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
                            session, direction, session->TrackColours[SCHEME_TRACK] | 18468, -16, -16, 32, 32, 3, height, -16,
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
                            session, direction, session->TrackColours[SCHEME_TRACK] | 18528, -16, -16, 32, 32, 3, height, -16,
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
                            session, direction, session->TrackColours[SCHEME_TRACK] | 18466, -16, -16, 32, 32, 3, height, -16,
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
                            session, direction, session->TrackColours[SCHEME_TRACK] | 18531, -16, -16, 32, 32, 3, height, -16,
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
                            session, direction, session->TrackColours[SCHEME_TRACK] | 18469, -16, -16, 32, 32, 3, height, -16,
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

/** rct2: 0x008ADBC4 */
static void giga_rc_track_diag_flat_to_left_bank(
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18497, -16, -16, 32, 32, 3, height, -16, -16,
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18494, -16, -16, 32, 32, 3, height, -16, -16,
                        height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18498, -16, -16, 32, 32, 0, height, -16, -16,
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18496, -16, -16, 32, 32, 3, height, -16, -16,
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18495, -16, -16, 32, 32, 3, height, -16, -16,
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

/** rct2: 0x008ADBD4 */
static void giga_rc_track_diag_flat_to_right_bank(
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18502, -16, -16, 32, 32, 3, height, -16, -16,
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18499, -16, -16, 32, 32, 3, height, -16, -16,
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18501, -16, -16, 32, 32, 3, height, -16, -16,
                        height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18503, -16, -16, 32, 32, 0, height, -16, -16,
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18500, -16, -16, 32, 32, 3, height, -16, -16,
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

/** rct2: 0x008ADBE4 */
static void giga_rc_track_diag_left_bank_to_flat(
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18500, -16, -16, 32, 32, 3, height, -16, -16,
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18501, -16, -16, 32, 32, 3, height, -16, -16,
                        height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18503, -16, -16, 32, 32, 0, height, -16, -16,
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18499, -16, -16, 32, 32, 3, height, -16, -16,
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18502, -16, -16, 32, 32, 3, height, -16, -16,
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

/** rct2: 0x008ADBF4 */
static void giga_rc_track_diag_right_bank_to_flat(
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18495, -16, -16, 32, 32, 3, height, -16, -16,
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18496, -16, -16, 32, 32, 3, height, -16, -16,
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18494, -16, -16, 32, 32, 3, height, -16, -16,
                        height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18498, -16, -16, 32, 32, 0, height, -16, -16,
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18497, -16, -16, 32, 32, 3, height, -16, -16,
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

/** rct2: 0x008ADC24 */
static void giga_rc_track_diag_left_bank_to_25_deg_up(
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18517, -16, -16, 32, 32, 3, height, -16, -16,
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18514, -16, -16, 32, 32, 3, height, -16, -16,
                        height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18518, -16, -16, 32, 32, 0, height, -16, -16,
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18516, -16, -16, 32, 32, 3, height, -16, -16,
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18515, -16, -16, 32, 32, 3, height, -16, -16,
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

/** rct2: 0x008ADC34 */
static void giga_rc_track_diag_right_bank_to_25_deg_up(
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18522, -16, -16, 32, 32, 3, height, -16, -16,
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18519, -16, -16, 32, 32, 3, height, -16, -16,
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18521, -16, -16, 32, 32, 3, height, -16, -16,
                        height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18523, -16, -16, 32, 32, 0, height, -16, -16,
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18520, -16, -16, 32, 32, 3, height, -16, -16,
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

/** rct2: 0x008ADC04 */
static void giga_rc_track_diag_25_deg_up_to_left_bank(
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18507, -16, -16, 32, 32, 3, height, -16, -16,
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18504, -16, -16, 32, 32, 3, height, -16, -16,
                        height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18508, -16, -16, 32, 32, 0, height, -16, -16,
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18506, -16, -16, 32, 32, 3, height, -16, -16,
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18505, -16, -16, 32, 32, 3, height, -16, -16,
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

/** rct2: 0x008ADC14 */
static void giga_rc_track_diag_25_deg_up_to_right_bank(
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18512, -16, -16, 32, 32, 3, height, -16, -16,
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18509, -16, -16, 32, 32, 3, height, -16, -16,
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18511, -16, -16, 32, 32, 3, height, -16, -16,
                        height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18513, -16, -16, 32, 32, 0, height, -16, -16,
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18510, -16, -16, 32, 32, 3, height, -16, -16,
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

/** rct2: 0x008ADC44 */
static void giga_rc_track_diag_left_bank_to_25_deg_down(
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18510, -16, -16, 32, 32, 3, height, -16, -16,
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18511, -16, -16, 32, 32, 3, height, -16, -16,
                        height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18513, -16, -16, 32, 32, 0, height, -16, -16,
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18509, -16, -16, 32, 32, 3, height, -16, -16,
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18512, -16, -16, 32, 32, 3, height, -16, -16,
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

/** rct2: 0x008ADC54 */
static void giga_rc_track_diag_right_bank_to_25_deg_down(
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18505, -16, -16, 32, 32, 3, height, -16, -16,
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18506, -16, -16, 32, 32, 3, height, -16, -16,
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18504, -16, -16, 32, 32, 3, height, -16, -16,
                        height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18508, -16, -16, 32, 32, 0, height, -16, -16,
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18507, -16, -16, 32, 32, 3, height, -16, -16,
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

/** rct2: 0x008ADC64 */
static void giga_rc_track_diag_25_deg_down_to_left_bank(
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18520, -16, -16, 32, 32, 3, height, -16, -16,
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18521, -16, -16, 32, 32, 3, height, -16, -16,
                        height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18523, -16, -16, 32, 32, 0, height, -16, -16,
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18519, -16, -16, 32, 32, 3, height, -16, -16,
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18522, -16, -16, 32, 32, 3, height, -16, -16,
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

/** rct2: 0x008ADC74 */
static void giga_rc_track_diag_25_deg_down_to_right_bank(
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18515, -16, -16, 32, 32, 3, height, -16, -16,
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18516, -16, -16, 32, 32, 3, height, -16, -16,
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18514, -16, -16, 32, 32, 3, height, -16, -16,
                        height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18518, -16, -16, 32, 32, 0, height, -16, -16,
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18517, -16, -16, 32, 32, 3, height, -16, -16,
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

/** rct2: 0x008ADBA4 */
static void giga_rc_track_diag_left_bank(
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18493, -16, -16, 32, 32, 3, height, -16, -16,
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18490, -16, -16, 32, 32, 0, height, -16, -16,
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18492, -16, -16, 32, 32, 3, height, -16, -16,
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18491, -16, -16, 32, 32, 3, height, -16, -16,
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

/** rct2: 0x008ADBB4 */
static void giga_rc_track_diag_right_bank(
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18491, -16, -16, 32, 32, 3, height, -16, -16,
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18492, -16, -16, 32, 32, 3, height, -16, -16,
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18490, -16, -16, 32, 32, 0, height, -16, -16,
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18493, -16, -16, 32, 32, 3, height, -16, -16,
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

/** rct2: 0x008ADEC4 */
static void giga_rc_track_block_brakes(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (direction)
    {
        case 0:
        case 2:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18076, 0, 0, 32, 20, 3, height, 0, 6, height);
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18080, 0, 0, 32, 1, 11, height, 0, 27, height + 5);
            break;
        case 1:
        case 3:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18077, 0, 0, 32, 20, 3, height, 0, 6, height);
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18081, 0, 0, 32, 1, 11, height, 0, 27, height + 5);
            break;
    }
    if (track_paint_util_should_paint_supports(session->MapPosition))
    {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
    }
    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
    paint_util_set_segment_support_height(
        session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x008ADCC4 */
static void giga_rc_track_left_banked_quarter_turn_3_25_deg_up(
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18655, 0, 6, 32, 20, 3, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18657, 0, 6, 32, 1, 34, height, 0, 27,
                        height);
                    break;
                case 2:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18659, 0, 6, 32, 20, 3, height);
                    break;
                case 3:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18653, 0, 6, 32, 20, 3, height);
                    break;
            }
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 8, height, session->TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_SQUARE_7);
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18654, 6, 0, 20, 32, 3, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18656, 6, 0, 1, 32, 34, height, 27, 0,
                        height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18658, 6, 0, 1, 32, 34, height, 27, 0,
                        height);
                    break;
                case 3:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18652, 6, 0, 20, 32, 3, height);
                    break;
            }
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 8, height, session->TrackColours[SCHEME_SUPPORTS]);
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
                session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 72, 0x20);
            break;
    }
}

/** rct2: 0x008ADCD4 */
static void giga_rc_track_right_banked_quarter_turn_3_25_deg_up(
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18644, 0, 6, 32, 20, 3, height);
                    break;
                case 1:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18646, 0, 6, 32, 20, 3, height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18648, 0, 6, 32, 1, 34, height, 0, 27,
                        height);
                    break;
                case 3:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18650, 0, 6, 32, 20, 3, height);
                    break;
            }
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 8, height, session->TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_SQUARE_7);
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18645, 6, 0, 20, 32, 3, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 4, 8, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18647, 6, 0, 1, 32, 34, height, 27, 0,
                        height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 4, 8, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18649, 6, 0, 1, 32, 34, height, 27, 0,
                        height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 4, 10, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18651, 6, 0, 20, 32, 3, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_TUBES, 4, 8, height, session->TrackColours[SCHEME_SUPPORTS]);
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
                session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 72, 0x20);
            break;
    }
}

/** rct2: 0x008ADCE4 */
static void giga_rc_track_left_banked_quarter_turn_3_25_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    giga_rc_track_right_banked_quarter_turn_3_25_deg_up(
        session, rideIndex, trackSequence, (direction + 1) & 3, height, tileElement);
}

/** rct2: 0x008ADCF4 */
static void giga_rc_track_right_banked_quarter_turn_3_25_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    giga_rc_track_left_banked_quarter_turn_3_25_deg_up(
        session, rideIndex, trackSequence, (direction - 1) & 3, height, tileElement);
}

/** rct2: 0x008ADD04 */
static void giga_rc_track_left_banked_quarter_turn_5_25_deg_up(
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18624, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18629, 0, 0, 32, 1, 34, height, 0, 27,
                        height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18634, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18639, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
            }
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 8, height, session->TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_SQUARE_7);
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18625, 0, 0, 32, 16, 3, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18630, 0, 0, 1, 1, 34, height, 30, 30,
                        height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18635, 0, 0, 32, 16, 3, height, 0, 16,
                        height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18640, 0, 0, 32, 16, 3, height, 0, 16,
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18626, 0, 0, 16, 16, 3, height, 0, 16,
                        height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18631, 0, 0, 1, 1, 34, height, 30, 30,
                        height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18636, 0, 0, 16, 16, 3, height, 16, 0,
                        height);
                    break;
                case 3:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18641, 0, 0, 16, 16, 3, height);
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18627, 0, 0, 16, 32, 3, height, 16, 0,
                        height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18632, 0, 0, 1, 1, 34, height, 30, 30,
                        height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18637, 0, 0, 1, 32, 34, height, 27, 0,
                        height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18642, 0, 0, 16, 32, 3, height, 16, 0,
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18628, 0, 0, 20, 32, 3, height, 6, 0, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18633, 0, 0, 1, 32, 34, height, 27, 0,
                        height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18638, 0, 0, 1, 32, 34, height, 27, 0,
                        height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18643, 0, 0, 20, 32, 3, height, 6, 0, height);
                    break;
            }
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 8, height, session->TrackColours[SCHEME_SUPPORTS]);
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
                session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 72, 0x20);
            break;
    }
}

/** rct2: 0x008ADD14 */
static void giga_rc_track_right_banked_quarter_turn_5_25_deg_up(
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18604, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18609, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18614, 0, 0, 32, 1, 34, height, 0, 27,
                        height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18619, 0, 0, 32, 20, 3, height, 0, 6, height);
                    break;
            }
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 8, height, session->TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_SQUARE_7);
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18605, 0, 0, 32, 16, 3, height, 0, 16,
                        height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18610, 0, 0, 32, 16, 3, height, 0, 16,
                        height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18615, 0, 0, 1, 1, 34, height, 30, 30,
                        height);
                    break;
                case 3:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18620, 0, 0, 32, 16, 3, height);
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18606, 0, 0, 16, 16, 3, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18611, 0, 0, 16, 16, 3, height, 16, 0,
                        height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18616, 0, 0, 1, 1, 34, height, 30, 30,
                        height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18621, 0, 0, 16, 16, 3, height, 0, 16,
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18607, 0, 0, 16, 32, 3, height, 16, 0,
                        height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18612, 0, 0, 1, 32, 34, height, 27, 0,
                        height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18617, 0, 0, 1, 1, 34, height, 30, 30,
                        height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18622, 0, 0, 16, 32, 3, height, 16, 0,
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18608, 0, 0, 20, 32, 3, height, 6, 0, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18613, 0, 0, 1, 32, 34, height, 27, 0,
                        height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18618, 0, 0, 1, 32, 34, height, 27, 0,
                        height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 18623, 0, 0, 20, 32, 3, height, 6, 0, height);
                    break;
            }
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 8, height, session->TrackColours[SCHEME_SUPPORTS]);
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
                session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 72, 0x20);
            break;
    }
}

/** rct2: 0x008ADD24 */
static void giga_rc_track_left_banked_quarter_turn_5_25_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    giga_rc_track_right_banked_quarter_turn_5_25_deg_up(
        session, rideIndex, trackSequence, (direction + 1) & 3, height, tileElement);
}

/** rct2: 0x008ADD34 */
static void giga_rc_track_right_banked_quarter_turn_5_25_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    giga_rc_track_left_banked_quarter_turn_5_25_deg_up(
        session, rideIndex, trackSequence, (direction - 1) & 3, height, tileElement);
}

/** rct2: 0x008ADD44 */
static void giga_rc_track_25_deg_up_to_left_banked_25_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (direction)
    {
        case 0:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18568, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 1:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18569, 0, 0, 32, 20, 3, height, 0, 6, height);
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18576, 0, 0, 32, 1, 34, height, 0, 27, height);
            break;
        case 2:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18570, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 3:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18571, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
    }
    if (track_paint_util_should_paint_supports(session->MapPosition))
    {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 8, height, session->TrackColours[SCHEME_SUPPORTS]);
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

/** rct2: 0x008ADD54 */
static void giga_rc_track_25_deg_up_to_right_banked_25_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (direction)
    {
        case 0:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18572, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 1:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18573, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 2:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18574, 0, 0, 32, 20, 3, height, 0, 6, height);
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18577, 0, 0, 32, 1, 34, height, 0, 27, height);
            break;
        case 3:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18575, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
    }
    if (track_paint_util_should_paint_supports(session->MapPosition))
    {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 8, height, session->TrackColours[SCHEME_SUPPORTS]);
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

/** rct2: 0x008ADD64 */
static void giga_rc_track_left_banked_25_deg_up_to_25_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (direction)
    {
        case 0:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18578, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 1:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18579, 0, 0, 32, 20, 3, height, 0, 6, height);
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18586, 0, 0, 32, 1, 34, height, 0, 27, height);
            break;
        case 2:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18580, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 3:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18581, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
    }
    if (track_paint_util_should_paint_supports(session->MapPosition))
    {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 8, height, session->TrackColours[SCHEME_SUPPORTS]);
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

/** rct2: 0x008ADD74 */
static void giga_rc_track_right_banked_25_deg_up_to_25_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (direction)
    {
        case 0:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18582, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 1:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18583, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 2:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18584, 0, 0, 32, 20, 3, height, 0, 6, height);
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18587, 0, 0, 32, 1, 34, height, 0, 27, height);
            break;
        case 3:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18585, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
    }
    if (track_paint_util_should_paint_supports(session->MapPosition))
    {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 8, height, session->TrackColours[SCHEME_SUPPORTS]);
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

/** rct2: 0x008ADD84 */
static void giga_rc_track_25_deg_down_to_left_banked_25_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    giga_rc_track_right_banked_25_deg_up_to_25_deg_up(
        session, rideIndex, trackSequence, (direction + 2) & 3, height, tileElement);
}

/** rct2: 0x008ADD94 */
static void giga_rc_track_25_deg_down_to_right_banked_25_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    giga_rc_track_left_banked_25_deg_up_to_25_deg_up(
        session, rideIndex, trackSequence, (direction + 2) & 3, height, tileElement);
}

/** rct2: 0x008ADDA4 */
static void giga_rc_track_left_banked_25_deg_down_to_25_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    giga_rc_track_25_deg_up_to_right_banked_25_deg_up(
        session, rideIndex, trackSequence, (direction + 2) & 3, height, tileElement);
}

/** rct2: 0x008ADDB4 */
static void giga_rc_track_right_banked_25_deg_down_to_25_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    giga_rc_track_25_deg_up_to_left_banked_25_deg_up(
        session, rideIndex, trackSequence, (direction + 2) & 3, height, tileElement);
}

/** rct2: 0x008ADDC4 */
static void giga_rc_track_left_banked_flat_to_left_banked_25_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (direction)
    {
        case 0:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18588, 0, 0, 32, 1, 34, height, 0, 27, height);
            break;
        case 1:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18589, 0, 0, 32, 1, 34, height, 0, 27, height);
            break;
        case 2:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18590, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 3:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18591, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
    }
    if (track_paint_util_should_paint_supports(session->MapPosition))
    {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 3, height, session->TrackColours[SCHEME_SUPPORTS]);
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

/** rct2: 0x008ADDD4 */
static void giga_rc_track_right_banked_flat_to_right_banked_25_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (direction)
    {
        case 0:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18592, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 1:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18593, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 2:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18594, 0, 0, 32, 1, 34, height, 0, 27, height);
            break;
        case 3:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18595, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
    }
    if (track_paint_util_should_paint_supports(session->MapPosition))
    {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 3, height, session->TrackColours[SCHEME_SUPPORTS]);
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

/** rct2: 0x008ADE04 */
static void giga_rc_track_left_banked_25_deg_up_to_left_banked_flat(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (direction)
    {
        case 0:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18596, 0, 0, 32, 1, 34, height, 0, 27, height);
            break;
        case 1:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18597, 0, 0, 32, 1, 34, height, 0, 27, height);
            break;
        case 2:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18598, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 3:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18599, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
    }
    if (track_paint_util_should_paint_supports(session->MapPosition))
    {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 6, height, session->TrackColours[SCHEME_SUPPORTS]);
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

/** rct2: 0x008ADE14 */
static void giga_rc_track_right_banked_25_deg_up_to_right_banked_flat(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (direction)
    {
        case 0:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18600, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 1:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18601, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 2:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18602, 0, 0, 32, 1, 34, height, 0, 27, height);
            break;
        case 3:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18603, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
    }
    if (track_paint_util_should_paint_supports(session->MapPosition))
    {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 6, height, session->TrackColours[SCHEME_SUPPORTS]);
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

/** rct2: 0x008ADE24 */
static void giga_rc_track_left_banked_flat_to_left_banked_25_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    giga_rc_track_right_banked_25_deg_up_to_right_banked_flat(
        session, rideIndex, trackSequence, (direction + 2) & 3, height, tileElement);
}

/** rct2: 0x008ADE34 */
static void giga_rc_track_right_banked_flat_to_right_banked_25_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    giga_rc_track_left_banked_25_deg_up_to_left_banked_flat(
        session, rideIndex, trackSequence, (direction + 2) & 3, height, tileElement);
}

/** rct2: 0x008ADDE4 */
static void giga_rc_track_left_banked_25_deg_down_to_left_banked_flat(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    giga_rc_track_right_banked_flat_to_right_banked_25_deg_up(
        session, rideIndex, trackSequence, (direction + 2) & 3, height, tileElement);
}

/** rct2: 0x008ADDF4 */
static void giga_rc_track_right_banked_25_deg_down_to_right_banked_flat(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    giga_rc_track_left_banked_flat_to_left_banked_25_deg_up(
        session, rideIndex, trackSequence, (direction + 2) & 3, height, tileElement);
}

/** rct2: 0x008ADE44 */
static void giga_rc_track_flat_to_left_banked_25_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (direction)
    {
        case 0:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18540, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 1:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18541, 0, 0, 32, 20, 3, height, 0, 6, height);
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18548, 0, 0, 32, 1, 34, height, 0, 27, height);
            break;
        case 2:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18542, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 3:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18543, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
    }
    if (track_paint_util_should_paint_supports(session->MapPosition))
    {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 3, height, session->TrackColours[SCHEME_SUPPORTS]);
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

/** rct2: 0x008ADE54 */
static void giga_rc_track_flat_to_right_banked_25_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (direction)
    {
        case 0:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18544, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 1:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18545, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 2:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18546, 0, 0, 32, 20, 3, height, 0, 6, height);
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18549, 0, 0, 32, 1, 34, height, 0, 27, height);
            break;
        case 3:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18547, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
    }
    if (track_paint_util_should_paint_supports(session->MapPosition))
    {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 3, height, session->TrackColours[SCHEME_SUPPORTS]);
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

/** rct2: 0x008ADE64 */
static void giga_rc_track_left_banked_25_deg_up_to_flat(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (direction)
    {
        case 0:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18550, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 1:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18551, 0, 0, 32, 20, 3, height, 0, 6, height);
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18558, 0, 0, 32, 1, 34, height, 0, 27, height);
            break;
        case 2:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18552, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 3:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18553, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
    }
    if (track_paint_util_should_paint_supports(session->MapPosition))
    {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 6, height, session->TrackColours[SCHEME_SUPPORTS]);
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

/** rct2: 0x008ADE74 */
static void giga_rc_track_right_banked_25_deg_up_to_flat(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (direction)
    {
        case 0:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18554, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 1:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18555, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 2:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18556, 0, 0, 32, 20, 3, height, 0, 6, height);
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18559, 0, 0, 32, 1, 34, height, 0, 27, height);
            break;
        case 3:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | 18557, 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
    }
    if (track_paint_util_should_paint_supports(session->MapPosition))
    {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 6, height, session->TrackColours[SCHEME_SUPPORTS]);
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

/** rct2: 0x008ADE84 */
static void giga_rc_track_flat_to_left_banked_25_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    giga_rc_track_right_banked_25_deg_up_to_flat(session, rideIndex, trackSequence, (direction + 2) & 3, height, tileElement);
}

/** rct2: 0x008ADE94 */
static void giga_rc_track_flat_to_right_banked_25_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    giga_rc_track_left_banked_25_deg_up_to_flat(session, rideIndex, trackSequence, (direction + 2) & 3, height, tileElement);
}

/** rct2: 0x008ADEA4 */
static void giga_rc_track_left_banked_25_deg_down_to_flat(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    giga_rc_track_flat_to_right_banked_25_deg_up(session, rideIndex, trackSequence, (direction + 2) & 3, height, tileElement);
}

/** rct2: 0x008ADEB4 */
static void giga_rc_track_right_banked_25_deg_down_to_flat(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    giga_rc_track_flat_to_left_banked_25_deg_up(session, rideIndex, trackSequence, (direction + 2) & 3, height, tileElement);
}

static void giga_rc_track_booster(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    // These offsets could be moved to the g2.dat file when that supports offsets.
    int8_t ne_sw_offsetX = 7;
    int8_t ne_sw_offsetY = -15;
    int8_t nw_se_offsetX = -15;
    int8_t nw_se_offsetY = 7;

    switch (direction)
    {
        case 0:
        case 2:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | SPR_G2_GIGA_RC_BOOSTER_NE_SW, ne_sw_offsetX,
                ne_sw_offsetY, 32, 20, 3, height, 0, 6, height);
            break;
        case 1:
        case 3:
            sub_98197C_rotated(
                session, direction, session->TrackColours[SCHEME_TRACK] | SPR_G2_GIGA_RC_BOOSTER_NW_SE, nw_se_offsetX,
                nw_se_offsetY, 32, 20, 3, height, 0, 6, height);
            break;
    }
    if (track_paint_util_should_paint_supports(session->MapPosition))
    {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
    }
    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
    paint_util_set_segment_support_height(
        session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

TRACK_PAINT_FUNCTION get_track_paint_function_giga_rc(int32_t trackType)
{
    switch (trackType)
    {
        case TrackElemType::Flat:
            return giga_rc_track_flat;
        case TrackElemType::EndStation:
        case TrackElemType::BeginStation:
        case TrackElemType::MiddleStation:
            return giga_rc_track_station;
        case TrackElemType::Up25:
            return giga_rc_track_25_deg_up;
        case TrackElemType::Up60:
            return giga_rc_track_60_deg_up;
        case TrackElemType::FlatToUp25:
            return giga_rc_track_flat_to_25_deg_up;
        case TrackElemType::Up25ToUp60:
            return giga_rc_track_25_deg_up_to_60_deg_up;
        case TrackElemType::Up60ToUp25:
            return giga_rc_track_60_deg_up_to_25_deg_up;
        case TrackElemType::Up25ToFlat:
            return giga_rc_track_25_deg_up_to_flat;
        case TrackElemType::Down25:
            return giga_rc_track_25_deg_down;
        case TrackElemType::Down60:
            return giga_rc_track_60_deg_down;
        case TrackElemType::FlatToDown25:
            return giga_rc_track_flat_to_25_deg_down;
        case TrackElemType::Down25ToDown60:
            return giga_rc_track_25_deg_down_to_60_deg_down;
        case TrackElemType::Down60ToDown25:
            return giga_rc_track_60_deg_down_to_25_deg_down;
        case TrackElemType::Down25ToFlat:
            return giga_rc_track_25_deg_down_to_flat;
        case TrackElemType::LeftQuarterTurn5Tiles:
            return giga_rc_track_left_quarter_turn_5;
        case TrackElemType::RightQuarterTurn5Tiles:
            return giga_rc_track_right_quarter_turn_5;
        case TrackElemType::FlatToLeftBank:
            return giga_rc_track_flat_to_left_bank;
        case TrackElemType::FlatToRightBank:
            return giga_rc_track_flat_to_right_bank;
        case TrackElemType::LeftBankToFlat:
            return giga_rc_track_left_bank_to_flat;
        case TrackElemType::RightBankToFlat:
            return giga_rc_track_right_bank_to_flat;
        case TrackElemType::BankedLeftQuarterTurn5Tiles:
            return giga_rc_track_banked_left_quarter_turn_5;
        case TrackElemType::BankedRightQuarterTurn5Tiles:
            return giga_rc_track_banked_right_quarter_turn_5;
        case TrackElemType::LeftBankToUp25:
            return giga_rc_track_left_bank_to_25_deg_up;
        case TrackElemType::RightBankToUp25:
            return giga_rc_track_right_bank_to_25_deg_up;
        case TrackElemType::Up25ToLeftBank:
            return giga_rc_track_25_deg_up_to_left_bank;
        case TrackElemType::Up25ToRightBank:
            return giga_rc_track_25_deg_up_to_right_bank;
        case TrackElemType::LeftBankToDown25:
            return giga_rc_track_left_bank_to_25_deg_down;
        case TrackElemType::RightBankToDown25:
            return giga_rc_track_right_bank_to_25_deg_down;
        case TrackElemType::Down25ToLeftBank:
            return giga_rc_track_25_deg_down_to_left_bank;
        case TrackElemType::Down25ToRightBank:
            return giga_rc_track_25_deg_down_to_right_bank;
        case TrackElemType::LeftBank:
            return giga_rc_track_left_bank;
        case TrackElemType::RightBank:
            return giga_rc_track_right_bank;
        case TrackElemType::LeftQuarterTurn5TilesUp25:
            return giga_rc_track_left_quarter_turn_5_25_deg_up;
        case TrackElemType::RightQuarterTurn5TilesUp25:
            return giga_rc_track_right_quarter_turn_5_25_deg_up;
        case TrackElemType::LeftQuarterTurn5TilesDown25:
            return giga_rc_track_left_quarter_turn_5_25_deg_down;
        case TrackElemType::RightQuarterTurn5TilesDown25:
            return giga_rc_track_right_quarter_turn_5_25_deg_down;
        case TrackElemType::SBendLeft:
            return giga_rc_track_s_bend_left;
        case TrackElemType::SBendRight:
            return giga_rc_track_s_bend_right;
        case TrackElemType::LeftQuarterTurn3Tiles:
            return giga_rc_track_left_quarter_turn_3;
        case TrackElemType::RightQuarterTurn3Tiles:
            return giga_rc_track_right_quarter_turn_3;
        case TrackElemType::LeftBankedQuarterTurn3Tiles:
            return giga_rc_track_left_quarter_turn_3_bank;
        case TrackElemType::RightBankedQuarterTurn3Tiles:
            return giga_rc_track_right_quarter_turn_3_bank;
        case TrackElemType::LeftQuarterTurn3TilesUp25:
            return giga_rc_track_left_quarter_turn_3_25_deg_up;
        case TrackElemType::RightQuarterTurn3TilesUp25:
            return giga_rc_track_right_quarter_turn_3_25_deg_up;
        case TrackElemType::LeftQuarterTurn3TilesDown25:
            return giga_rc_track_left_quarter_turn_3_25_deg_down;
        case TrackElemType::RightQuarterTurn3TilesDown25:
            return giga_rc_track_right_quarter_turn_3_25_deg_down;
        case TrackElemType::LeftHalfBankedHelixUpSmall:
            return giga_rc_track_left_half_banked_helix_up_small;
        case TrackElemType::RightHalfBankedHelixUpSmall:
            return giga_rc_track_right_half_banked_helix_up_small;
        case TrackElemType::LeftHalfBankedHelixDownSmall:
            return giga_rc_track_left_half_banked_helix_down_small;
        case TrackElemType::RightHalfBankedHelixDownSmall:
            return giga_rc_track_right_half_banked_helix_down_small;
        case TrackElemType::LeftHalfBankedHelixUpLarge:
            return giga_rc_track_left_half_banked_helix_up_large;
        case TrackElemType::RightHalfBankedHelixUpLarge:
            return giga_rc_track_right_half_banked_helix_up_large;
        case TrackElemType::LeftHalfBankedHelixDownLarge:
            return giga_rc_track_left_half_banked_helix_down_large;
        case TrackElemType::RightHalfBankedHelixDownLarge:
            return giga_rc_track_right_half_banked_helix_down_large;
        case TrackElemType::LeftQuarterTurn1TileUp60:
            return giga_rc_track_left_quarter_turn_1_60_deg_up;
        case TrackElemType::RightQuarterTurn1TileUp60:
            return giga_rc_track_right_quarter_turn_1_60_deg_up;
        case TrackElemType::LeftQuarterTurn1TileDown60:
            return giga_rc_track_left_quarter_turn_1_60_deg_down;
        case TrackElemType::RightQuarterTurn1TileDown60:
            return giga_rc_track_right_quarter_turn_1_60_deg_down;
        case TrackElemType::Brakes:
            return giga_rc_track_brakes;
        case TrackElemType::Up25LeftBanked:
            return giga_rc_track_25_deg_up_left_banked;
        case TrackElemType::Up25RightBanked:
            return giga_rc_track_25_deg_up_right_banked;
        case TrackElemType::OnRidePhoto:
            return giga_rc_track_on_ride_photo;
        case TrackElemType::Down25LeftBanked:
            return giga_rc_track_25_deg_down_left_banked;
        case TrackElemType::Down25RightBanked:
            return giga_rc_track_25_deg_down_right_banked;
        case TrackElemType::FlatToUp60LongBase:
            return giga_rc_track_flat_to_60_deg_up_long_base;
        case TrackElemType::Up60ToFlatLongBase:
            return giga_rc_track_60_deg_up_to_flat_long_base;
        case TrackElemType::Down60ToFlatLongBase:
            return giga_rc_track_60_deg_down_to_flat_long_base;
        case TrackElemType::FlatToDown60LongBase:
            return giga_rc_track_flat_to_60_deg_down_long_base;
        case TrackElemType::CableLiftHill:
            return giga_rc_track_cable_lift_hill;
        case TrackElemType::LeftEighthToDiag:
            return giga_rc_track_left_eighth_to_diag;
        case TrackElemType::RightEighthToDiag:
            return giga_rc_track_right_eighth_to_diag;
        case TrackElemType::LeftEighthToOrthogonal:
            return giga_rc_track_left_eighth_to_orthogonal;
        case TrackElemType::RightEighthToOrthogonal:
            return giga_rc_track_right_eighth_to_orthogonal;
        case TrackElemType::LeftEighthBankToDiag:
            return giga_rc_track_left_eighth_bank_to_diag;
        case TrackElemType::RightEighthBankToDiag:
            return giga_rc_track_right_eighth_bank_to_diag;
        case TrackElemType::LeftEighthBankToOrthogonal:
            return giga_rc_track_left_eighth_bank_to_orthogonal;
        case TrackElemType::RightEighthBankToOrthogonal:
            return giga_rc_track_right_eighth_bank_to_orthogonal;
        case TrackElemType::DiagFlat:
            return giga_rc_track_diag_flat;
        case TrackElemType::DiagUp25:
            return giga_rc_track_diag_25_deg_up;
        case TrackElemType::DiagUp60:
            return giga_rc_track_diag_60_deg_up;
        case TrackElemType::DiagFlatToUp25:
            return giga_rc_track_diag_flat_to_25_deg_up;
        case TrackElemType::DiagUp25ToUp60:
            return giga_rc_track_diag_25_deg_up_to_60_deg_up;
        case TrackElemType::DiagUp60ToUp25:
            return giga_rc_track_diag_60_deg_up_to_25_deg_up;
        case TrackElemType::DiagUp25ToFlat:
            return giga_rc_track_diag_25_deg_up_to_flat;
        case TrackElemType::DiagDown25:
            return giga_rc_track_diag_25_deg_down;
        case TrackElemType::DiagDown60:
            return giga_rc_track_diag_60_deg_down;
        case TrackElemType::DiagFlatToDown25:
            return giga_rc_track_diag_flat_to_25_deg_down;
        case TrackElemType::DiagDown25ToDown60:
            return giga_rc_track_diag_25_deg_down_to_60_deg_down;
        case TrackElemType::DiagDown60ToDown25:
            return giga_rc_track_diag_60_deg_down_to_25_deg_down;
        case TrackElemType::DiagDown25ToFlat:
            return giga_rc_track_diag_25_deg_down_to_flat;
        case TrackElemType::DiagFlatToLeftBank:
            return giga_rc_track_diag_flat_to_left_bank;
        case TrackElemType::DiagFlatToRightBank:
            return giga_rc_track_diag_flat_to_right_bank;
        case TrackElemType::DiagLeftBankToFlat:
            return giga_rc_track_diag_left_bank_to_flat;
        case TrackElemType::DiagRightBankToFlat:
            return giga_rc_track_diag_right_bank_to_flat;
        case TrackElemType::DiagLeftBankToUp25:
            return giga_rc_track_diag_left_bank_to_25_deg_up;
        case TrackElemType::DiagRightBankToUp25:
            return giga_rc_track_diag_right_bank_to_25_deg_up;
        case TrackElemType::DiagUp25ToLeftBank:
            return giga_rc_track_diag_25_deg_up_to_left_bank;
        case TrackElemType::DiagUp25ToRightBank:
            return giga_rc_track_diag_25_deg_up_to_right_bank;
        case TrackElemType::DiagLeftBankToDown25:
            return giga_rc_track_diag_left_bank_to_25_deg_down;
        case TrackElemType::DiagRightBankToDown25:
            return giga_rc_track_diag_right_bank_to_25_deg_down;
        case TrackElemType::DiagDown25ToLeftBank:
            return giga_rc_track_diag_25_deg_down_to_left_bank;
        case TrackElemType::DiagDown25ToRightBank:
            return giga_rc_track_diag_25_deg_down_to_right_bank;
        case TrackElemType::DiagLeftBank:
            return giga_rc_track_diag_left_bank;
        case TrackElemType::DiagRightBank:
            return giga_rc_track_diag_right_bank;
        case TrackElemType::BlockBrakes:
            return giga_rc_track_block_brakes;
        case TrackElemType::LeftBankedQuarterTurn3TileUp25:
            return giga_rc_track_left_banked_quarter_turn_3_25_deg_up;
        case TrackElemType::RightBankedQuarterTurn3TileUp25:
            return giga_rc_track_right_banked_quarter_turn_3_25_deg_up;
        case TrackElemType::LeftBankedQuarterTurn3TileDown25:
            return giga_rc_track_left_banked_quarter_turn_3_25_deg_down;
        case TrackElemType::RightBankedQuarterTurn3TileDown25:
            return giga_rc_track_right_banked_quarter_turn_3_25_deg_down;
        case TrackElemType::LeftBankedQuarterTurn5TileUp25:
            return giga_rc_track_left_banked_quarter_turn_5_25_deg_up;
        case TrackElemType::RightBankedQuarterTurn5TileUp25:
            return giga_rc_track_right_banked_quarter_turn_5_25_deg_up;
        case TrackElemType::LeftBankedQuarterTurn5TileDown25:
            return giga_rc_track_left_banked_quarter_turn_5_25_deg_down;
        case TrackElemType::RightBankedQuarterTurn5TileDown25:
            return giga_rc_track_right_banked_quarter_turn_5_25_deg_down;
        case TrackElemType::Up25ToLeftBankedUp25:
            return giga_rc_track_25_deg_up_to_left_banked_25_deg_up;
        case TrackElemType::Up25ToRightBankedUp25:
            return giga_rc_track_25_deg_up_to_right_banked_25_deg_up;
        case TrackElemType::LeftBankedUp25ToUp25:
            return giga_rc_track_left_banked_25_deg_up_to_25_deg_up;
        case TrackElemType::RightBankedUp25ToUp25:
            return giga_rc_track_right_banked_25_deg_up_to_25_deg_up;
        case TrackElemType::Down25ToLeftBankedDown25:
            return giga_rc_track_25_deg_down_to_left_banked_25_deg_down;
        case TrackElemType::Down25ToRightBankedDown25:
            return giga_rc_track_25_deg_down_to_right_banked_25_deg_down;
        case TrackElemType::LeftBankedDown25ToDown25:
            return giga_rc_track_left_banked_25_deg_down_to_25_deg_down;
        case TrackElemType::RightBankedDown25ToDown25:
            return giga_rc_track_right_banked_25_deg_down_to_25_deg_down;
        case TrackElemType::LeftBankedFlatToLeftBankedUp25:
            return giga_rc_track_left_banked_flat_to_left_banked_25_deg_up;
        case TrackElemType::RightBankedFlatToRightBankedUp25:
            return giga_rc_track_right_banked_flat_to_right_banked_25_deg_up;
        case TrackElemType::LeftBankedUp25ToLeftBankedFlat:
            return giga_rc_track_left_banked_25_deg_up_to_left_banked_flat;
        case TrackElemType::RightBankedUp25ToRightBankedFlat:
            return giga_rc_track_right_banked_25_deg_up_to_right_banked_flat;
        case TrackElemType::LeftBankedFlatToLeftBankedDown25:
            return giga_rc_track_left_banked_flat_to_left_banked_25_deg_down;
        case TrackElemType::RightBankedFlatToRightBankedDown25:
            return giga_rc_track_right_banked_flat_to_right_banked_25_deg_down;
        case TrackElemType::LeftBankedDown25ToLeftBankedFlat:
            return giga_rc_track_left_banked_25_deg_down_to_left_banked_flat;
        case TrackElemType::RightBankedDown25ToRightBankedFlat:
            return giga_rc_track_right_banked_25_deg_down_to_right_banked_flat;
        case TrackElemType::FlatToLeftBankedUp25:
            return giga_rc_track_flat_to_left_banked_25_deg_up;
        case TrackElemType::FlatToRightBankedUp25:
            return giga_rc_track_flat_to_right_banked_25_deg_up;
        case TrackElemType::LeftBankedUp25ToFlat:
            return giga_rc_track_left_banked_25_deg_up_to_flat;
        case TrackElemType::RightBankedUp25ToFlat:
            return giga_rc_track_right_banked_25_deg_up_to_flat;
        case TrackElemType::FlatToLeftBankedDown25:
            return giga_rc_track_flat_to_left_banked_25_deg_down;
        case TrackElemType::FlatToRightBankedDown25:
            return giga_rc_track_flat_to_right_banked_25_deg_down;
        case TrackElemType::LeftBankedDown25ToFlat:
            return giga_rc_track_left_banked_25_deg_down_to_flat;
        case TrackElemType::RightBankedDown25ToFlat:
            return giga_rc_track_right_banked_25_deg_down_to_flat;

        case TrackElemType::Booster:
            return giga_rc_track_booster;
    }
    return nullptr;
}
