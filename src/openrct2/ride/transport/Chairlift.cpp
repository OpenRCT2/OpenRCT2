/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../interface/Viewport.h"
#include "../../paint/Paint.h"
#include "../../paint/Supports.h"
#include "../../world/Map.h"
#include "../Ride.h"
#include "../RideData.h"
#include "../Track.h"
#include "../TrackPaint.h"

enum
{
    SPR_CHAIRLIFT_CABLE_FLAT_SW_NE = 20500,
    SPR_CHAIRLIFT_CABLE_FLAT_SE_NW,
    SPR_20502,
    SPR_20503,
    SPR_20504,
    SPR_20505,
    SPR_CHAIRLIFT_STATION_COLUMN_NE_SW,
    SPR_CHAIRLIFT_STATION_COLUMN_SE_NW,
    SPR_20508,
    SPR_20509,
    SPR_20510,
    SPR_20511,
    SPR_20512,
    SPR_20513,
    SPR_20514,
    SPR_20515,
    SPR_CHAIRLIFT_CABLE_UP_SW_NE,
    SPR_CHAIRLIFT_CABLE_UP_NW_SE,
    SPR_CHAIRLIFT_CABLE_UP_NE_SW,
    SPR_CHAIRLIFT_CABLE_UP_SE_NW,
    SPR_20520,
    SPR_20521,
    SPR_20522,
    SPR_20523,
    SPR_20524,
    SPR_20525,
    SPR_20526,
    SPR_20527,
    SPR_CHAIRLIFT_CORNER_NW_SW,
    SPR_CHAIRLIFT_CORNER_NW_NE,
    SPR_CHAIRLIFT_CORNER_SE_NE,
    SPR_CHAIRLIFT_CORNER_SW_SE,
    SPR_20532,
    SPR_20533,
    SPR_20534,
    SPR_20535,
    SPR_20536,
    SPR_20537,
    SPR_20538,
    SPR_20539,
    SPR_CHAIRLIFT_BULLWHEEL_FRAME_1,
    SPR_CHAIRLIFT_BULLWHEEL_FRAME_2,
    SPR_CHAIRLIFT_BULLWHEEL_FRAME_3,
    SPR_CHAIRLIFT_BULLWHEEL_FRAME_4,
    SPR_CHAIRLIFT_STATION_END_CAP_NE,
    SPR_CHAIRLIFT_STATION_END_CAP_SE,
    SPR_CHAIRLIFT_STATION_END_CAP_SW,
    SPR_CHAIRLIFT_STATION_END_CAP_NW,
};

const uint32_t chairlift_bullwheel_frames[] = { SPR_CHAIRLIFT_BULLWHEEL_FRAME_1, SPR_CHAIRLIFT_BULLWHEEL_FRAME_2,
                                                SPR_CHAIRLIFT_BULLWHEEL_FRAME_3, SPR_CHAIRLIFT_BULLWHEEL_FRAME_4 };

static void chairlift_paint_util_draw_supports(paint_session* session, int32_t segments, uint16_t height)
{
    bool success = false;

    for (int32_t s = 0; s < 9; s++)
    {
        if (!(segments & segment_offsets[s]))
        {
            continue;
        }

        if (metal_a_supports_paint_setup(session, METAL_SUPPORTS_TRUSS, s, 0, height, session->TrackColours[SCHEME_SUPPORTS]))
        {
            success = true;
        }
    }

    if (success)
    {
        return;
    }

    support_height* supportSegments = session->SupportSegments;
    for (int32_t s = 0; s < 9; s++)
    {
        if (!(segments & segment_offsets[s]))
        {
            continue;
        }
        uint16_t temp = supportSegments[s].height;
        supportSegments[s].height = session->Support.height;
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_TRUSS, s, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
        supportSegments[s].height = temp;
    }
}

static const TileElement* chairlift_paint_util_map_get_track_element_at_from_ride_fuzzy(
    int32_t x, int32_t y, int32_t z, ride_id_t rideIndex)
{
    const TileElement* tileElement = map_get_first_element_at({ x, y });
    if (tileElement == nullptr)
    {
        return nullptr;
    }

    do
    {
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_TRACK)
            continue;
        if (tileElement->AsTrack()->GetRideIndex() != rideIndex)
            continue;
        if (tileElement->base_height != z && tileElement->base_height != z - 1)
            continue;

        return tileElement;
    } while (!(tileElement++)->IsLastForTile());

    return nullptr;
};

static bool chairlift_paint_util_is_first_track(
    ride_id_t rideIndex, const TileElement* tileElement, const CoordsXY& pos, uint8_t trackType)
{
    if (tileElement->AsTrack()->GetTrackType() != TrackElemType::BeginStation)
    {
        return false;
    }

    CoordsXY delta = CoordsDirectionDelta[tileElement->GetDirection()];
    CoordsXY newPos = {
        static_cast<int32_t>(pos.x - delta.x),
        static_cast<int32_t>(pos.y - delta.y),
    };

    const TileElement* nextTrack = chairlift_paint_util_map_get_track_element_at_from_ride_fuzzy(
        newPos.x, newPos.y, tileElement->base_height, rideIndex);

    return nextTrack == nullptr;
}

static bool chairlift_paint_util_is_last_track(
    ride_id_t rideIndex, const TileElement* tileElement, const CoordsXY& pos, uint8_t trackType)
{
    if (tileElement->AsTrack()->GetTrackType() != TrackElemType::EndStation)
    {
        return false;
    }

    CoordsXY delta = CoordsDirectionDelta[tileElement->GetDirection()];
    CoordsXY newPos = {
        static_cast<int32_t>(pos.x + delta.x),
        static_cast<int32_t>(pos.y + delta.y),
    };

    const TileElement* nextTrack = chairlift_paint_util_map_get_track_element_at_from_ride_fuzzy(
        newPos.x, newPos.y, tileElement->base_height, rideIndex);

    return nextTrack == nullptr;
}

static void chairlift_paint_station_ne_sw(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    auto ride = get_ride(rideIndex);
    if (ride == nullptr)
        return;

    const CoordsXY pos = session->MapPosition;
    auto trackType = tileElement->AsTrack()->GetTrackType();
    uint32_t imageId;

    bool isStart = chairlift_paint_util_is_first_track(rideIndex, tileElement, pos, trackType);
    bool isEnd = chairlift_paint_util_is_last_track(rideIndex, tileElement, pos, trackType);

    auto stationObj = ride_get_station_object(ride);

    wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_MISC], nullptr);

    if (!isStart && !isEnd)
    {
        imageId = ((direction == 0) ? SPR_20502 : SPR_20504) | session->TrackColours[SCHEME_TRACK];
        sub_98197C(session, imageId, 0, 0, 32, 6, 2, height, 0, 13, height + 28);
    }

    imageId = SPR_FLOOR_METAL | session->TrackColours[SCHEME_SUPPORTS];
    sub_98197C(session, imageId, 0, 0, 32, 32, 1, height, 0, 0, height);

    bool hasFence = track_paint_util_has_fence(EDGE_NW, pos, tileElement, ride, session->CurrentRotation);
    if (hasFence)
    {
        imageId = SPR_FENCE_METAL_NW | session->TrackColours[SCHEME_TRACK];
        sub_98199C(session, imageId, 0, 0, 32, 1, 7, height, 0, 2, height + 2);
    }
    track_paint_util_draw_station_covers(session, EDGE_NW, hasFence, stationObj, height);

    if ((direction == 2 && isStart) || (direction == 0 && isEnd))
    {
        imageId = SPR_FENCE_METAL_NE | session->TrackColours[SCHEME_TRACK];
        sub_98199C(session, imageId, 0, 0, 1, 28, 7, height, 2, 2, height + 4);
    }

    hasFence = track_paint_util_has_fence(EDGE_SE, pos, tileElement, ride, session->CurrentRotation);
    if (hasFence)
    {
        imageId = SPR_FENCE_METAL_SE | session->TrackColours[SCHEME_TRACK];
        sub_98197C(session, imageId, 0, 0, 32, 1, 27, height, 0, 30, height + 2);
    }
    track_paint_util_draw_station_covers(session, EDGE_SE, hasFence, stationObj, height);

    bool drawFrontColumn = true;
    bool drawBackColumn = true;
    if ((direction == 0 && isStart) || (direction == 2 && isEnd))
    {
        imageId = SPR_FENCE_METAL_SW | session->TrackColours[SCHEME_TRACK];
        sub_98197C(session, imageId, 0, 0, 1, 28, 27, height, 30, 2, height + 4);

        imageId = chairlift_bullwheel_frames[ride->chairlift_bullwheel_rotation / 16384] | session->TrackColours[SCHEME_TRACK];
        sub_98197C(session, imageId, 0, 0, 4, 4, 26, height, 14, 14, height + 4);

        imageId = SPR_CHAIRLIFT_STATION_END_CAP_NE | session->TrackColours[SCHEME_TRACK];
        sub_98199C(session, imageId, 0, 0, 4, 4, 26, height, 14, 14, height + 4);

        drawFrontColumn = false;
    }
    else if ((direction == 2 && isStart) || (direction == 0 && isEnd))
    {
        imageId = chairlift_bullwheel_frames[ride->chairlift_bullwheel_rotation / 16384] | session->TrackColours[SCHEME_TRACK];
        sub_98197C(session, imageId, 0, 0, 4, 4, 26, height, 14, 14, height + 4);

        imageId = SPR_CHAIRLIFT_STATION_END_CAP_SW | session->TrackColours[SCHEME_TRACK];
        sub_98199C(session, imageId, 0, 0, 4, 4, 26, height, 14, 14, height + 4);

        drawBackColumn = false;
    }

    if (drawBackColumn)
    {
        imageId = SPR_CHAIRLIFT_STATION_COLUMN_NE_SW | session->TrackColours[SCHEME_TRACK];
        sub_98197C(session, imageId, 0, 16, 1, 1, 7, height + 2, 1, 16, height + 2);
    }

    if (drawFrontColumn)
    {
        imageId = SPR_CHAIRLIFT_STATION_COLUMN_NE_SW | session->TrackColours[SCHEME_TRACK];
        sub_98197C(session, imageId, 30, 16, 1, 1, 7, height + 2, 1, 16, height + 2); // bound offset x is wrong?
    }

    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_push_tunnel_left(session, height, TUNNEL_SQUARE_FLAT);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

static void chairlift_paint_station_se_nw(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    auto ride = get_ride(rideIndex);
    if (ride == nullptr)
        return;

    const CoordsXY pos = session->MapPosition;
    auto trackType = tileElement->AsTrack()->GetTrackType();
    uint32_t imageId;

    bool isStart = chairlift_paint_util_is_first_track(rideIndex, tileElement, pos, trackType);
    bool isEnd = chairlift_paint_util_is_last_track(rideIndex, tileElement, pos, trackType);

    auto stationObj = ride_get_station_object(ride);

    wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_MISC], nullptr);

    if (!isStart && !isEnd)
    {
        imageId = ((direction == 1) ? SPR_20503 : SPR_20505) | session->TrackColours[SCHEME_TRACK];
        sub_98197C(session, imageId, 0, 0, 6, 32, 2, height, 13, 0, height + 28);
    }

    imageId = SPR_FLOOR_METAL | session->TrackColours[SCHEME_SUPPORTS];
    sub_98197C(session, imageId, 0, 0, 32, 32, 1, height, 0, 0, height);

    bool hasFence = track_paint_util_has_fence(EDGE_NE, pos, tileElement, ride, session->CurrentRotation);
    if (hasFence)
    {
        imageId = SPR_FENCE_METAL_NE | session->TrackColours[SCHEME_TRACK];
        sub_98199C(session, imageId, 0, 0, 1, 32, 7, height, 2, 0, height + 2);
    }
    track_paint_util_draw_station_covers(session, EDGE_NE, hasFence, stationObj, height);

    if ((direction == 1 && isStart) || (direction == 3 && isEnd))
    {
        imageId = SPR_FENCE_METAL_NW | session->TrackColours[SCHEME_TRACK];
        sub_98199C(session, imageId, 0, 0, 28, 1, 7, height, 2, 2, height + 4);
    }

    hasFence = track_paint_util_has_fence(EDGE_SW, pos, tileElement, ride, session->CurrentRotation);
    if (hasFence)
    {
        imageId = SPR_FENCE_METAL_SW | session->TrackColours[SCHEME_TRACK];
        sub_98197C(session, imageId, 0, 0, 1, 32, 27, height, 30, 0, height + 2);
    }
    track_paint_util_draw_station_covers(session, EDGE_SW, hasFence, stationObj, height);

    bool drawRightColumn = true;
    bool drawLeftColumn = true;
    if ((direction == 1 && isStart) || (direction == 3 && isEnd))
    {
        imageId = chairlift_bullwheel_frames[ride->chairlift_bullwheel_rotation / 16384] | session->TrackColours[SCHEME_TRACK];
        sub_98197C(session, imageId, 0, 0, 4, 4, 26, height, 14, 14, height + 4);

        imageId = SPR_CHAIRLIFT_STATION_END_CAP_SE | session->TrackColours[SCHEME_TRACK];
        sub_98199C(session, imageId, 0, 0, 4, 4, 26, height, 14, 14, height + 4);

        drawLeftColumn = false;
    }
    else if ((direction == 3 && isStart) || (direction == 1 && isEnd))
    {
        imageId = SPR_FENCE_METAL_SE | session->TrackColours[SCHEME_TRACK];
        sub_98197C(session, imageId, 0, 0, 28, 1, 27, height, 2, 30, height + 4);

        imageId = chairlift_bullwheel_frames[ride->chairlift_bullwheel_rotation / 16384] | session->TrackColours[SCHEME_TRACK];
        sub_98197C(session, imageId, 0, 0, 4, 4, 26, height, 14, 14, height + 4);

        imageId = SPR_CHAIRLIFT_STATION_END_CAP_NW | session->TrackColours[SCHEME_TRACK];
        sub_98199C(session, imageId, 0, 0, 4, 4, 26, height, 14, 14, height + 4);

        drawRightColumn = false;
    }

    if (drawLeftColumn)
    {
        imageId = SPR_CHAIRLIFT_STATION_COLUMN_SE_NW | session->TrackColours[SCHEME_TRACK];
        sub_98197C(session, imageId, 16, 0, 1, 1, 7, height + 2, 16, 1, height + 2);
    }

    if (drawRightColumn)
    {
        imageId = SPR_CHAIRLIFT_STATION_COLUMN_SE_NW | session->TrackColours[SCHEME_TRACK];
        sub_98197C(session, imageId, 16, 30, 1, 1, 7, height + 2, 16, 1, height + 2); // bound offset x is wrong?

        paint_util_push_tunnel_right(session, height, TUNNEL_SQUARE_FLAT);
    }

    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x00744068 */
static void chairlift_paint_station(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    if (direction % 2)
    {
        chairlift_paint_station_se_nw(session, rideIndex, trackSequence, direction, height, tileElement);
    }
    else
    {
        chairlift_paint_station_ne_sw(session, rideIndex, trackSequence, direction, height, tileElement);
    }
}

static void chairlift_paint_flat(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    uint32_t imageId;
    if (direction & 1)
    {
        imageId = SPR_CHAIRLIFT_CABLE_FLAT_SE_NW | session->TrackColours[SCHEME_TRACK];
        sub_98197C(session, imageId, 0, 0, 6, 32, 2, height, 13, 0, height + 28);
        paint_util_push_tunnel_right(session, height, TUNNEL_SQUARE_FLAT);
    }
    else
    {
        imageId = SPR_CHAIRLIFT_CABLE_FLAT_SW_NE | session->TrackColours[SCHEME_TRACK];
        sub_98197C(session, imageId, 0, 0, 32, 6, 2, height, 0, 13, height + 28);
        paint_util_push_tunnel_left(session, height, TUNNEL_SQUARE_FLAT);
    }

    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x00743FD8 */
static void chairlift_paint_25_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    uint32_t imageId;

    switch (direction)
    {
        case 0:
            imageId = SPR_CHAIRLIFT_CABLE_UP_SW_NE | session->TrackColours[SCHEME_TRACK];
            sub_98197C(session, imageId, 0, 0, 32, 6, 2, height, 0, 13, height + 28);
            paint_util_push_tunnel_left(session, height - 8, TUNNEL_SQUARE_7);
            break;

        case 1:
            imageId = SPR_CHAIRLIFT_CABLE_UP_NW_SE | session->TrackColours[SCHEME_TRACK];
            sub_98197C(session, imageId, 0, 0, 6, 32, 2, height, 13, 0, height + 28);
            paint_util_push_tunnel_right(session, height + 8, TUNNEL_SQUARE_8);
            break;

        case 2:
            imageId = SPR_CHAIRLIFT_CABLE_UP_NE_SW | session->TrackColours[SCHEME_TRACK];
            sub_98197C(session, imageId, 0, 0, 32, 6, 2, height, 0, 13, height + 28);
            paint_util_push_tunnel_left(session, height + 8, TUNNEL_SQUARE_8);
            break;

        case 3:
            imageId = SPR_CHAIRLIFT_CABLE_UP_SE_NW | session->TrackColours[SCHEME_TRACK];
            sub_98197C(session, imageId, 0, 0, 6, 32, 2, height, 13, 0, height + 28);
            paint_util_push_tunnel_right(session, height - 8, TUNNEL_SQUARE_7);
            break;
    }

    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 56, 0x20);
}

/** rct2: 0x00743FD8 */
static void chairlift_paint_flat_to_25_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    uint32_t imageId;

    switch (direction)
    {
        case 0:
            imageId = SPR_20508 | session->TrackColours[SCHEME_TRACK];
            sub_98197C(session, imageId, 0, 0, 32, 6, 2, height, 0, 13, height + 28);

            imageId = SPR_20520 | session->TrackColours[SCHEME_SUPPORTS];
            sub_98197C(session, imageId, 0, 0, 4, 4, 25, height, 14, 14, height + 1);

            paint_util_push_tunnel_left(session, height, TUNNEL_SQUARE_FLAT);
            break;

        case 1:
            imageId = SPR_20509 | session->TrackColours[SCHEME_TRACK];
            sub_98197C(session, imageId, 0, 0, 6, 32, 2, height, 13, 0, height + 28);

            imageId = SPR_20521 | session->TrackColours[SCHEME_SUPPORTS];
            sub_98197C(session, imageId, 0, 0, 4, 4, 25, height, 14, 14, height + 1);

            paint_util_push_tunnel_right(session, height, TUNNEL_SQUARE_8);
            break;

        case 2:
            imageId = SPR_20510 | session->TrackColours[SCHEME_TRACK];
            sub_98197C(session, imageId, 0, 0, 32, 6, 2, height, 0, 13, height + 28);

            imageId = SPR_20522 | session->TrackColours[SCHEME_SUPPORTS];
            sub_98197C(session, imageId, 0, 0, 4, 4, 25, height, 14, 14, height + 1);

            paint_util_push_tunnel_left(session, height, TUNNEL_SQUARE_8);
            break;

        case 3:
            imageId = SPR_20511 | session->TrackColours[SCHEME_TRACK];
            sub_98197C(session, imageId, 0, 0, 6, 32, 2, height, 13, 0, height + 28);

            imageId = SPR_20523 | session->TrackColours[SCHEME_SUPPORTS];
            sub_98197C(session, imageId, 0, 0, 4, 4, 25, height, 14, 14, height + 1);

            paint_util_push_tunnel_right(session, height, TUNNEL_SQUARE_FLAT);
            break;
    }

    chairlift_paint_util_draw_supports(session, SEGMENT_C4, height);
    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 48, 0x20);
}

/** rct2: 0x00743FF8 */
static void chairlift_paint_25_deg_up_to_flat(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    uint32_t imageId;

    switch (direction)
    {
        case 0:
            imageId = SPR_20512 | session->TrackColours[SCHEME_TRACK];
            sub_98197C(session, imageId, 0, 0, 32, 6, 2, height, 0, 13, height + 28);

            imageId = SPR_20524 | session->TrackColours[SCHEME_SUPPORTS];
            sub_98197C(session, imageId, 0, 0, 4, 4, 25, height, 14, 14, height + 1);

            paint_util_push_tunnel_left(session, height - 8, TUNNEL_SQUARE_FLAT);
            break;

        case 1:
            imageId = SPR_20513 | session->TrackColours[SCHEME_TRACK];
            sub_98197C(session, imageId, 0, 0, 6, 32, 2, height, 13, 0, height + 28);

            imageId = SPR_20525 | session->TrackColours[SCHEME_SUPPORTS];
            sub_98197C(session, imageId, 0, 0, 4, 4, 25, height, 14, 14, height + 1);

            paint_util_push_tunnel_right(session, height + 8, TUNNEL_14);
            break;

        case 2:
            imageId = SPR_20514 | session->TrackColours[SCHEME_TRACK];
            sub_98197C(session, imageId, 0, 0, 32, 6, 2, height, 0, 13, height + 28);

            imageId = SPR_20526 | session->TrackColours[SCHEME_SUPPORTS];
            sub_98197C(session, imageId, 0, 0, 4, 4, 25, height, 14, 14, height + 1);

            paint_util_push_tunnel_left(session, height + 8, TUNNEL_14);
            break;

        case 3:
            imageId = SPR_20515 | session->TrackColours[SCHEME_TRACK];
            sub_98197C(session, imageId, 0, 0, 6, 32, 2, height, 13, 0, height + 28);

            imageId = SPR_20527 | session->TrackColours[SCHEME_SUPPORTS];
            sub_98197C(session, imageId, 0, 0, 4, 4, 25, height, 14, 14, height + 1);

            paint_util_push_tunnel_right(session, height - 8, TUNNEL_SQUARE_FLAT);
            break;
    }

    chairlift_paint_util_draw_supports(session, SEGMENT_C4, height);
    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 40, 0x20);
}

/** rct2: 0x00744008 */
static void chairlift_paint_25_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    chairlift_paint_25_deg_up(session, rideIndex, trackSequence, (direction + 2) % 4, height, tileElement);
}

/** rct2: 0x00744018 */
static void chairlift_paint_flat_to_25_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    chairlift_paint_25_deg_up_to_flat(session, rideIndex, trackSequence, (direction + 2) % 4, height, tileElement);
}

/** rct2: 0x00744028 */
static void chairlift_paint_25_deg_down_to_flat(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    chairlift_paint_flat_to_25_deg_up(session, rideIndex, trackSequence, (direction + 2) % 4, height, tileElement);
}

/** rct2: 0x00744038 */
static void chairlift_paint_left_quarter_turn_1_tile(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    uint32_t imageId;

    switch (direction)
    {
        case 0:
            imageId = SPR_CHAIRLIFT_CORNER_NW_SW | session->TrackColours[SCHEME_TRACK];
            sub_98197C(session, imageId, 0, 0, 16, 16, 2, height, 16, 0, height + 28);

            imageId = SPR_20532 | session->TrackColours[SCHEME_SUPPORTS];
            sub_98197C(session, imageId, 0, 0, 2, 2, 27, height, 16, 4, height);

            imageId = SPR_20536 | session->TrackColours[SCHEME_SUPPORTS];
            sub_98197C(session, imageId, 0, 0, 2, 2, 27, height, 28, 4, height);

            paint_util_push_tunnel_left(session, height, TUNNEL_SQUARE_FLAT);
            break;

        case 1:
            imageId = SPR_CHAIRLIFT_CORNER_NW_NE | session->TrackColours[SCHEME_TRACK];
            sub_98197C(session, imageId, 0, 0, 16, 16, 2, height, 0, 0, height + 28);

            imageId = SPR_20533 | session->TrackColours[SCHEME_SUPPORTS];
            sub_98197C(session, imageId, 0, 0, 2, 2, 27, height, 16, 4, height);

            imageId = SPR_20537 | session->TrackColours[SCHEME_SUPPORTS];
            sub_98197C(session, imageId, 0, 0, 2, 2, 27, height, 4, 16, height);
            break;

        case 2:
            imageId = SPR_CHAIRLIFT_CORNER_SE_NE | session->TrackColours[SCHEME_TRACK];
            sub_98197C(session, imageId, 0, 0, 16, 16, 2, height, 0, 16, height + 28);

            imageId = SPR_20534 | session->TrackColours[SCHEME_SUPPORTS];
            sub_98197C(session, imageId, 0, 0, 2, 2, 27, height, 4, 16, height);

            imageId = SPR_20538 | session->TrackColours[SCHEME_SUPPORTS];
            sub_98197C(session, imageId, 0, 0, 2, 2, 27, height, 16, 28, height);

            paint_util_push_tunnel_right(session, height, TUNNEL_SQUARE_FLAT);
            break;

        case 3:
            imageId = SPR_CHAIRLIFT_CORNER_SW_SE | session->TrackColours[SCHEME_TRACK];
            sub_98197C(session, imageId, 0, 0, 16, 16, 2, height, 16, 16, height + 28);

            imageId = SPR_20535 | session->TrackColours[SCHEME_SUPPORTS];
            sub_98197C(session, imageId, 0, 0, 2, 2, 27, height, 28, 16, height);

            imageId = SPR_20539 | session->TrackColours[SCHEME_SUPPORTS];
            sub_98197C(session, imageId, 0, 0, 2, 2, 27, height, 16, 28, height);

            paint_util_push_tunnel_left(session, height, TUNNEL_SQUARE_FLAT);
            paint_util_push_tunnel_right(session, height, TUNNEL_SQUARE_FLAT);
            break;
    }

    chairlift_paint_util_draw_supports(session, paint_util_rotate_segments(SEGMENT_C8 | SEGMENT_D0, direction), height);

    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x00744048 */
static void chairlift_paint_right_quarter_turn_1_tile(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    chairlift_paint_left_quarter_turn_1_tile(session, rideIndex, trackSequence, (direction + 3) % 4, height, tileElement);
}

/* 0x008AAA0C */
TRACK_PAINT_FUNCTION get_track_paint_function_chairlift(int32_t trackType)
{
    switch (trackType)
    {
        case TrackElemType::BeginStation:
        case TrackElemType::MiddleStation:
        case TrackElemType::EndStation:
            return chairlift_paint_station;

        case TrackElemType::Flat:
            return chairlift_paint_flat;

        case TrackElemType::FlatToUp25:
            return chairlift_paint_flat_to_25_deg_up;
        case TrackElemType::Up25:
            return chairlift_paint_25_deg_up;
        case TrackElemType::Up25ToFlat:
            return chairlift_paint_25_deg_up_to_flat;

        case TrackElemType::FlatToDown25:
            return chairlift_paint_flat_to_25_deg_down;
        case TrackElemType::Down25:
            return chairlift_paint_25_deg_down;
        case TrackElemType::Down25ToFlat:
            return chairlift_paint_25_deg_down_to_flat;

        case TrackElemType::LeftQuarterTurn1Tile:
            return chairlift_paint_left_quarter_turn_1_tile;
        case TrackElemType::RightQuarterTurn1Tile:
            return chairlift_paint_right_quarter_turn_1_tile;
    }

    return nullptr;
}
