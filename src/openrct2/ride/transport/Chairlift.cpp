/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
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

#include <iterator>

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

const uint32_t chairlift_bullwheel_frames[] = {
    SPR_CHAIRLIFT_BULLWHEEL_FRAME_1,
    SPR_CHAIRLIFT_BULLWHEEL_FRAME_2,
    SPR_CHAIRLIFT_BULLWHEEL_FRAME_3,
    SPR_CHAIRLIFT_BULLWHEEL_FRAME_4,
};

static void chairlift_paint_util_draw_supports(PaintSession& session, int32_t segments, uint16_t height)
{
    bool success = false;

    for (uint8_t s = 0; s < std::size(segment_offsets); s++)
    {
        if (!(segments & segment_offsets[s]))
        {
            continue;
        }

        if (MetalASupportsPaintSetup(session, METAL_SUPPORTS_TRUSS, s, 0, height, session.TrackColours[SCHEME_SUPPORTS]))
        {
            success = true;
        }
    }

    if (success)
    {
        return;
    }

    SupportHeight* supportSegments = session.SupportSegments;
    for (uint8_t s = 0; s < std::size(segment_offsets); s++)
    {
        if (!(segments & segment_offsets[s]))
        {
            continue;
        }
        uint16_t temp = supportSegments[s].height;
        supportSegments[s].height = session.Support.height;
        MetalASupportsPaintSetup(session, METAL_SUPPORTS_TRUSS, s, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
        supportSegments[s].height = temp;
    }
}

static const TrackElement* chairlift_paint_util_map_get_track_element_at_from_ride_fuzzy(
    int32_t x, int32_t y, int32_t z, const Ride& ride)
{
    const TileElement* tileElement = MapGetFirstElementAt(CoordsXY{ x, y });
    if (tileElement == nullptr)
    {
        return nullptr;
    }

    do
    {
        if (tileElement->GetType() != TileElementType::Track)
            continue;
        if (tileElement->GetRideIndex() != ride.id)
            continue;
        if (tileElement->base_height != z && tileElement->base_height != z - 1)
            continue;

        return tileElement->AsTrack();
    } while (!(tileElement++)->IsLastForTile());

    return nullptr;
};

static bool chairlift_paint_util_is_first_track(
    const Ride& ride, const TrackElement& trackElement, const CoordsXY& pos, track_type_t trackType)
{
    if (trackElement.GetTrackType() != TrackElemType::BeginStation)
    {
        return false;
    }

    CoordsXY delta = CoordsDirectionDelta[trackElement.GetDirection()];
    CoordsXY newPos = {
        static_cast<int32_t>(pos.x - delta.x),
        static_cast<int32_t>(pos.y - delta.y),
    };

    const TrackElement* nextTrack = chairlift_paint_util_map_get_track_element_at_from_ride_fuzzy(
        newPos.x, newPos.y, trackElement.base_height, ride);

    return nextTrack == nullptr;
}

static bool chairlift_paint_util_is_last_track(
    const Ride& ride, const TrackElement& trackElement, const CoordsXY& pos, track_type_t trackType)
{
    if (trackElement.GetTrackType() != TrackElemType::EndStation)
    {
        return false;
    }

    CoordsXY delta = CoordsDirectionDelta[trackElement.GetDirection()];
    CoordsXY newPos = {
        static_cast<int32_t>(pos.x + delta.x),
        static_cast<int32_t>(pos.y + delta.y),
    };

    const TrackElement* nextTrack = chairlift_paint_util_map_get_track_element_at_from_ride_fuzzy(
        newPos.x, newPos.y, trackElement.base_height, ride);

    return nextTrack == nullptr;
}

static void chairlift_paint_station_ne_sw(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    const CoordsXY pos = session.MapPosition;
    auto trackType = trackElement.GetTrackType();
    ImageId imageId;

    bool isStart = chairlift_paint_util_is_first_track(ride, trackElement, pos, trackType);
    bool isEnd = chairlift_paint_util_is_last_track(ride, trackElement, pos, trackType);

    const auto* stationObj = ride.GetStationObject();

    WoodenASupportsPaintSetup(session, 0, 0, height, session.TrackColours[SCHEME_MISC]);

    if (!isStart && !isEnd)
    {
        imageId = session.TrackColours[SCHEME_TRACK].WithIndex(((direction == 0) ? SPR_20502 : SPR_20504));
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 32, 6, 2 }, { 0, 13, height + 28 });
    }

    imageId = session.TrackColours[SCHEME_SUPPORTS].WithIndex(SPR_FLOOR_METAL);
    PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 32, 32, 1 }, { 0, 0, height });

    bool hasFence = track_paint_util_has_fence(EDGE_NW, pos, trackElement, ride, session.CurrentRotation);
    if (hasFence)
    {
        imageId = session.TrackColours[SCHEME_TRACK].WithIndex(SPR_FENCE_METAL_NW);
        PaintAddImageAsChild(session, imageId, { 0, 0, height }, { 32, 1, 7 }, { 0, 2, height + 2 });
    }
    track_paint_util_draw_station_covers(session, EDGE_NW, hasFence, stationObj, height);

    if ((direction == 2 && isStart) || (direction == 0 && isEnd))
    {
        imageId = session.TrackColours[SCHEME_TRACK].WithIndex(SPR_FENCE_METAL_NE);
        PaintAddImageAsChild(session, imageId, { 0, 0, height }, { 1, 28, 7 }, { 2, 2, height + 4 });
    }

    hasFence = track_paint_util_has_fence(EDGE_SE, pos, trackElement, ride, session.CurrentRotation);
    if (hasFence)
    {
        imageId = session.TrackColours[SCHEME_TRACK].WithIndex(SPR_FENCE_METAL_SE);
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 32, 1, 27 }, { 0, 30, height + 2 });
    }
    track_paint_util_draw_station_covers(session, EDGE_SE, hasFence, stationObj, height);

    bool drawFrontColumn = true;
    bool drawBackColumn = true;
    if ((direction == 0 && isStart) || (direction == 2 && isEnd))
    {
        imageId = session.TrackColours[SCHEME_TRACK].WithIndex(SPR_FENCE_METAL_SW);
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 1, 28, 27 }, { 30, 2, height + 4 });

        imageId = session.TrackColours[SCHEME_TRACK].WithIndex(
            chairlift_bullwheel_frames[ride.chairlift_bullwheel_rotation / 16384]);
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 4, 4, 26 }, { 14, 14, height + 4 });

        imageId = session.TrackColours[SCHEME_TRACK].WithIndex(SPR_CHAIRLIFT_STATION_END_CAP_NE);
        PaintAddImageAsChild(session, imageId, { 0, 0, height }, { 4, 4, 26 }, { 14, 14, height + 4 });

        drawFrontColumn = false;
    }
    else if ((direction == 2 && isStart) || (direction == 0 && isEnd))
    {
        imageId = session.TrackColours[SCHEME_TRACK].WithIndex(
            chairlift_bullwheel_frames[ride.chairlift_bullwheel_rotation / 16384]);
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 4, 4, 26 }, { 14, 14, height + 4 });

        imageId = session.TrackColours[SCHEME_TRACK].WithIndex(SPR_CHAIRLIFT_STATION_END_CAP_SW);
        PaintAddImageAsChild(session, imageId, { 0, 0, height }, { 4, 4, 26 }, { 14, 14, height + 4 });

        drawBackColumn = false;
    }

    if (drawBackColumn)
    {
        imageId = session.TrackColours[SCHEME_TRACK].WithIndex(SPR_CHAIRLIFT_STATION_COLUMN_NE_SW);
        PaintAddImageAsParent(session, imageId, { 0, 16, height + 2 }, { 1, 1, 7 }, { 1, 16, height + 2 });
    }

    if (drawFrontColumn)
    {
        imageId = session.TrackColours[SCHEME_TRACK].WithIndex(SPR_CHAIRLIFT_STATION_COLUMN_NE_SW);
        PaintAddImageAsParent(
            session, imageId, { 30, 16, height + 2 }, { 1, 1, 7 }, { 1, 16, height + 2 }); // bound offset x is wrong?
    }

    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
    PaintUtilPushTunnelLeft(session, height, TUNNEL_SQUARE_FLAT);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

static void chairlift_paint_station_se_nw(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    const CoordsXY pos = session.MapPosition;
    auto trackType = trackElement.GetTrackType();
    ImageId imageId;

    bool isStart = chairlift_paint_util_is_first_track(ride, trackElement, pos, trackType);
    bool isEnd = chairlift_paint_util_is_last_track(ride, trackElement, pos, trackType);

    const auto* stationObj = ride.GetStationObject();

    WoodenASupportsPaintSetup(session, 1, 0, height, session.TrackColours[SCHEME_MISC]);

    if (!isStart && !isEnd)
    {
        imageId = session.TrackColours[SCHEME_TRACK].WithIndex(((direction == 1) ? SPR_20503 : SPR_20505));
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 6, 32, 2 }, { 13, 0, height + 28 });
    }

    imageId = session.TrackColours[SCHEME_SUPPORTS].WithIndex(SPR_FLOOR_METAL);
    PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 32, 32, 1 }, { 0, 0, height });

    bool hasFence = track_paint_util_has_fence(EDGE_NE, pos, trackElement, ride, session.CurrentRotation);
    if (hasFence)
    {
        imageId = session.TrackColours[SCHEME_TRACK].WithIndex(SPR_FENCE_METAL_NE);
        PaintAddImageAsChild(session, imageId, { 0, 0, height }, { 1, 32, 7 }, { 2, 0, height + 2 });
    }
    track_paint_util_draw_station_covers(session, EDGE_NE, hasFence, stationObj, height);

    if ((direction == 1 && isStart) || (direction == 3 && isEnd))
    {
        imageId = session.TrackColours[SCHEME_TRACK].WithIndex(SPR_FENCE_METAL_NW);
        PaintAddImageAsChild(session, imageId, { 0, 0, height }, { 28, 1, 7 }, { 2, 2, height + 4 });
    }

    hasFence = track_paint_util_has_fence(EDGE_SW, pos, trackElement, ride, session.CurrentRotation);
    if (hasFence)
    {
        imageId = session.TrackColours[SCHEME_TRACK].WithIndex(SPR_FENCE_METAL_SW);
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 1, 32, 27 }, { 30, 0, height + 2 });
    }
    track_paint_util_draw_station_covers(session, EDGE_SW, hasFence, stationObj, height);

    bool drawRightColumn = true;
    bool drawLeftColumn = true;
    if ((direction == 1 && isStart) || (direction == 3 && isEnd))
    {
        imageId = session.TrackColours[SCHEME_TRACK].WithIndex(
            chairlift_bullwheel_frames[ride.chairlift_bullwheel_rotation / 16384]);
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 4, 4, 26 }, { 14, 14, height + 4 });

        imageId = session.TrackColours[SCHEME_TRACK].WithIndex(SPR_CHAIRLIFT_STATION_END_CAP_SE);
        PaintAddImageAsChild(session, imageId, { 0, 0, height }, { 4, 4, 26 }, { 14, 14, height + 4 });

        drawLeftColumn = false;
    }
    else if ((direction == 3 && isStart) || (direction == 1 && isEnd))
    {
        imageId = session.TrackColours[SCHEME_TRACK].WithIndex(SPR_FENCE_METAL_SE);
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 28, 1, 27 }, { 2, 30, height + 4 });

        imageId = session.TrackColours[SCHEME_TRACK].WithIndex(
            chairlift_bullwheel_frames[ride.chairlift_bullwheel_rotation / 16384]);
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 4, 4, 26 }, { 14, 14, height + 4 });

        imageId = session.TrackColours[SCHEME_TRACK].WithIndex(SPR_CHAIRLIFT_STATION_END_CAP_NW);
        PaintAddImageAsChild(session, imageId, { 0, 0, height }, { 4, 4, 26 }, { 14, 14, height + 4 });

        drawRightColumn = false;
    }

    if (drawLeftColumn)
    {
        imageId = session.TrackColours[SCHEME_TRACK].WithIndex(SPR_CHAIRLIFT_STATION_COLUMN_SE_NW);
        PaintAddImageAsParent(session, imageId, { 16, 0, height + 2 }, { 1, 1, 7 }, { 16, 1, height + 2 });
    }

    if (drawRightColumn)
    {
        imageId = session.TrackColours[SCHEME_TRACK].WithIndex(SPR_CHAIRLIFT_STATION_COLUMN_SE_NW);
        PaintAddImageAsParent(
            session, imageId, { 16, 30, height + 2 }, { 1, 1, 7 }, { 16, 1, height + 2 }); // bound offset x is wrong?

        PaintUtilPushTunnelRight(session, height, TUNNEL_SQUARE_FLAT);
    }

    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

/** rct2: 0x00744068 */
static void chairlift_paint_station(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (direction % 2)
    {
        chairlift_paint_station_se_nw(session, ride, trackSequence, direction, height, trackElement);
    }
    else
    {
        chairlift_paint_station_ne_sw(session, ride, trackSequence, direction, height, trackElement);
    }
}

static void chairlift_paint_flat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    ImageId imageId;
    if (direction & 1)
    {
        imageId = session.TrackColours[SCHEME_TRACK].WithIndex(SPR_CHAIRLIFT_CABLE_FLAT_SE_NW);
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 6, 32, 2 }, { 13, 0, height + 28 });
        PaintUtilPushTunnelRight(session, height, TUNNEL_SQUARE_FLAT);
    }
    else
    {
        imageId = session.TrackColours[SCHEME_TRACK].WithIndex(SPR_CHAIRLIFT_CABLE_FLAT_SW_NE);
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 32, 6, 2 }, { 0, 13, height + 28 });
        PaintUtilPushTunnelLeft(session, height, TUNNEL_SQUARE_FLAT);
    }

    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

/** rct2: 0x00743FD8 */
static void chairlift_paint_25_deg_up(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    ImageId imageId;

    switch (direction)
    {
        case 0:
            imageId = session.TrackColours[SCHEME_TRACK].WithIndex(SPR_CHAIRLIFT_CABLE_UP_SW_NE);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 32, 6, 2 }, { 0, 13, height + 28 });
            PaintUtilPushTunnelLeft(session, height - 8, TUNNEL_SQUARE_7);
            break;

        case 1:
            imageId = session.TrackColours[SCHEME_TRACK].WithIndex(SPR_CHAIRLIFT_CABLE_UP_NW_SE);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 6, 32, 2 }, { 13, 0, height + 28 });
            PaintUtilPushTunnelRight(session, height + 8, TUNNEL_SQUARE_8);
            break;

        case 2:
            imageId = session.TrackColours[SCHEME_TRACK].WithIndex(SPR_CHAIRLIFT_CABLE_UP_NE_SW);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 32, 6, 2 }, { 0, 13, height + 28 });
            PaintUtilPushTunnelLeft(session, height + 8, TUNNEL_SQUARE_8);
            break;

        case 3:
            imageId = session.TrackColours[SCHEME_TRACK].WithIndex(SPR_CHAIRLIFT_CABLE_UP_SE_NW);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 6, 32, 2 }, { 13, 0, height + 28 });
            PaintUtilPushTunnelRight(session, height - 8, TUNNEL_SQUARE_7);
            break;
    }

    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
}

/** rct2: 0x00743FD8 */
static void chairlift_paint_flat_to_25_deg_up(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    ImageId imageId;

    switch (direction)
    {
        case 0:
            imageId = session.TrackColours[SCHEME_TRACK].WithIndex(SPR_20508);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 32, 6, 2 }, { 0, 13, height + 28 });

            imageId = session.TrackColours[SCHEME_SUPPORTS].WithIndex(SPR_20520);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 4, 4, 25 }, { 14, 14, height + 1 });

            PaintUtilPushTunnelLeft(session, height, TUNNEL_SQUARE_FLAT);
            break;

        case 1:
            imageId = session.TrackColours[SCHEME_TRACK].WithIndex(SPR_20509);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 6, 32, 2 }, { 13, 0, height + 28 });

            imageId = session.TrackColours[SCHEME_SUPPORTS].WithIndex(SPR_20521);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 4, 4, 25 }, { 14, 14, height + 1 });

            PaintUtilPushTunnelRight(session, height, TUNNEL_SQUARE_8);
            break;

        case 2:
            imageId = session.TrackColours[SCHEME_TRACK].WithIndex(SPR_20510);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 32, 6, 2 }, { 0, 13, height + 28 });

            imageId = session.TrackColours[SCHEME_SUPPORTS].WithIndex(SPR_20522);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 4, 4, 25 }, { 14, 14, height + 1 });

            PaintUtilPushTunnelLeft(session, height, TUNNEL_SQUARE_8);
            break;

        case 3:
            imageId = session.TrackColours[SCHEME_TRACK].WithIndex(SPR_20511);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 6, 32, 2 }, { 13, 0, height + 28 });

            imageId = session.TrackColours[SCHEME_SUPPORTS].WithIndex(SPR_20523);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 4, 4, 25 }, { 14, 14, height + 1 });

            PaintUtilPushTunnelRight(session, height, TUNNEL_SQUARE_FLAT);
            break;
    }

    chairlift_paint_util_draw_supports(session, SEGMENT_C4, height);
    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
}

/** rct2: 0x00743FF8 */
static void chairlift_paint_25_deg_up_to_flat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    ImageId imageId;

    switch (direction)
    {
        case 0:
            imageId = session.TrackColours[SCHEME_TRACK].WithIndex(SPR_20512);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 32, 6, 2 }, { 0, 13, height + 28 });

            imageId = session.TrackColours[SCHEME_SUPPORTS].WithIndex(SPR_20524);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 4, 4, 25 }, { 14, 14, height + 1 });

            PaintUtilPushTunnelLeft(session, height - 8, TUNNEL_SQUARE_FLAT);
            break;

        case 1:
            imageId = session.TrackColours[SCHEME_TRACK].WithIndex(SPR_20513);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 6, 32, 2 }, { 13, 0, height + 28 });

            imageId = session.TrackColours[SCHEME_SUPPORTS].WithIndex(SPR_20525);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 4, 4, 25 }, { 14, 14, height + 1 });

            PaintUtilPushTunnelRight(session, height + 8, TUNNEL_14);
            break;

        case 2:
            imageId = session.TrackColours[SCHEME_TRACK].WithIndex(SPR_20514);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 32, 6, 2 }, { 0, 13, height + 28 });

            imageId = session.TrackColours[SCHEME_SUPPORTS].WithIndex(SPR_20526);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 4, 4, 25 }, { 14, 14, height + 1 });

            PaintUtilPushTunnelLeft(session, height + 8, TUNNEL_14);
            break;

        case 3:
            imageId = session.TrackColours[SCHEME_TRACK].WithIndex(SPR_20515);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 6, 32, 2 }, { 13, 0, height + 28 });

            imageId = session.TrackColours[SCHEME_SUPPORTS].WithIndex(SPR_20527);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 4, 4, 25 }, { 14, 14, height + 1 });

            PaintUtilPushTunnelRight(session, height - 8, TUNNEL_SQUARE_FLAT);
            break;
    }

    chairlift_paint_util_draw_supports(session, SEGMENT_C4, height);
    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 40, 0x20);
}

/** rct2: 0x00744008 */
static void chairlift_paint_25_deg_down(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    chairlift_paint_25_deg_up(session, ride, trackSequence, (direction + 2) % 4, height, trackElement);
}

/** rct2: 0x00744018 */
static void chairlift_paint_flat_to_25_deg_down(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    chairlift_paint_25_deg_up_to_flat(session, ride, trackSequence, (direction + 2) % 4, height, trackElement);
}

/** rct2: 0x00744028 */
static void chairlift_paint_25_deg_down_to_flat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    chairlift_paint_flat_to_25_deg_up(session, ride, trackSequence, (direction + 2) % 4, height, trackElement);
}

/** rct2: 0x00744038 */
static void chairlift_paint_left_quarter_turn_1_tile(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    ImageId imageId;

    switch (direction)
    {
        case 0:
            imageId = session.TrackColours[SCHEME_TRACK].WithIndex(SPR_CHAIRLIFT_CORNER_NW_SW);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 16, 16, 2 }, { 16, 0, height + 28 });

            imageId = session.TrackColours[SCHEME_SUPPORTS].WithIndex(SPR_20532);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 2, 2, 27 }, { 16, 4, height });

            imageId = session.TrackColours[SCHEME_SUPPORTS].WithIndex(SPR_20536);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 2, 2, 27 }, { 28, 4, height });

            PaintUtilPushTunnelLeft(session, height, TUNNEL_SQUARE_FLAT);
            break;

        case 1:
            imageId = session.TrackColours[SCHEME_TRACK].WithIndex(SPR_CHAIRLIFT_CORNER_NW_NE);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 16, 16, 2 }, { 0, 0, height + 28 });

            imageId = session.TrackColours[SCHEME_SUPPORTS].WithIndex(SPR_20533);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 2, 2, 27 }, { 16, 4, height });

            imageId = session.TrackColours[SCHEME_SUPPORTS].WithIndex(SPR_20537);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 2, 2, 27 }, { 4, 16, height });
            break;

        case 2:
            imageId = session.TrackColours[SCHEME_TRACK].WithIndex(SPR_CHAIRLIFT_CORNER_SE_NE);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 16, 16, 2 }, { 0, 16, height + 28 });

            imageId = session.TrackColours[SCHEME_SUPPORTS].WithIndex(SPR_20534);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 2, 2, 27 }, { 4, 16, height });

            imageId = session.TrackColours[SCHEME_SUPPORTS].WithIndex(SPR_20538);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 2, 2, 27 }, { 16, 28, height });

            PaintUtilPushTunnelRight(session, height, TUNNEL_SQUARE_FLAT);
            break;

        case 3:
            imageId = session.TrackColours[SCHEME_TRACK].WithIndex(SPR_CHAIRLIFT_CORNER_SW_SE);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 16, 16, 2 }, { 16, 16, height + 28 });

            imageId = session.TrackColours[SCHEME_SUPPORTS].WithIndex(SPR_20535);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 2, 2, 27 }, { 28, 16, height });

            imageId = session.TrackColours[SCHEME_SUPPORTS].WithIndex(SPR_20539);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 2, 2, 27 }, { 16, 28, height });

            PaintUtilPushTunnelLeft(session, height, TUNNEL_SQUARE_FLAT);
            PaintUtilPushTunnelRight(session, height, TUNNEL_SQUARE_FLAT);
            break;
    }

    chairlift_paint_util_draw_supports(session, PaintUtilRotateSegments(SEGMENT_C8 | SEGMENT_D0, direction), height);

    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

/** rct2: 0x00744048 */
static void chairlift_paint_right_quarter_turn_1_tile(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    chairlift_paint_left_quarter_turn_1_tile(session, ride, trackSequence, (direction + 3) % 4, height, trackElement);
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
