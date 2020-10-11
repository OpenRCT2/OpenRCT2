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
#include "../RideData.h"
#include "../Track.h"
#include "../TrackPaint.h"
#include "../VehiclePaint.h"

#ifndef NO_VEHICLES
/**
 *
 *  rct2: 0x006D44D5
 */
void vehicle_visual_submarine(
    paint_session* session, int32_t x, int32_t imageDirection, int32_t y, int32_t z, const Vehicle* vehicle,
    const rct_ride_entry_vehicle* vehicleEntry)
{
    auto imageFlags = SPRITE_ID_PALETTE_COLOUR_3(vehicle->colours.body_colour, vehicle->colours.trim_colour);
    if (vehicle->IsGhost())
    {
        imageFlags = CONSTRUCTION_MARKER;
    }

    int32_t baseImage_id = imageDirection;
    int32_t image_id;
    if (vehicle->restraints_position >= 64)
    {
        if ((vehicleEntry->sprite_flags & VEHICLE_SPRITE_FLAG_RESTRAINT_ANIMATION) && !(imageDirection & 3))
        {
            baseImage_id /= 8;
            baseImage_id += ((vehicle->restraints_position - 64) / 64) * 4;
            baseImage_id *= vehicleEntry->base_num_frames;
            baseImage_id += vehicleEntry->restraint_image_id;
        }
    }
    else
    {
        if (vehicleEntry->flags & VEHICLE_ENTRY_FLAG_11)
        {
            baseImage_id /= 2;
        }
        if (vehicleEntry->sprite_flags & VEHICLE_SPRITE_FLAG_15)
        {
            baseImage_id /= 8;
        }
        baseImage_id *= vehicleEntry->base_num_frames;
        baseImage_id += vehicleEntry->base_image_id;
        baseImage_id += vehicle->SwingSprite;
    }

    vehicle_boundbox bb = VehicleBoundboxes[vehicleEntry->draw_order][imageDirection / 2];

    image_id = baseImage_id | imageFlags;
    paint_struct* ps = sub_98197C(
        session, image_id, 0, 0, bb.length_x, bb.length_y, bb.length_z, z, bb.offset_x, bb.offset_y, bb.offset_z + z);
    if (ps != nullptr)
    {
        ps->tertiary_colour = vehicle->colours_extended;
    }

    image_id = (baseImage_id + 1) | imageFlags;
    ps = sub_98197C(session, image_id, 0, 0, bb.length_x, bb.length_y, 2, z, bb.offset_x, bb.offset_y, bb.offset_z + z - 10);
    if (ps != nullptr)
    {
        ps->tertiary_colour = vehicle->colours_extended;
    }

    assert(vehicleEntry->effect_visual == 1);
}
#endif

static void submarine_ride_paint_track_station(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    auto ride = get_ride(rideIndex);
    if (ride == nullptr)
        return;

    auto stationObj = ride_get_station_object(ride);
    int32_t heightLower = height - 16;
    uint32_t imageId;

    if (direction & 1)
    {
        imageId = SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_SE_NW | session->TrackColours[SCHEME_TRACK];
        sub_98197C(session, imageId, 0, 0, 20, 32, 3, heightLower, 6, 0, heightLower);

        paint_util_push_tunnel_right(session, height, TUNNEL_SQUARE_FLAT);
        track_paint_util_draw_pier(
            session, ride, stationObj, session->MapPosition, direction, height, tileElement, session->CurrentRotation);
    }
    else
    {
        imageId = SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_NE_SW | session->TrackColours[SCHEME_TRACK];
        sub_98197C(session, imageId, 0, 0, 32, 20, 3, heightLower, 0, 6, heightLower);

        paint_util_push_tunnel_left(session, height, TUNNEL_SQUARE_FLAT);
        track_paint_util_draw_pier(
            session, ride, stationObj, session->MapPosition, direction, height, tileElement, session->CurrentRotation);
    }

    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

static void submarine_ride_paint_track_flat(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    int32_t heightLower = height - 16;
    uint32_t imageId;

    if (direction & 1)
    {
        imageId = SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_SE_NW | session->TrackColours[SCHEME_TRACK];
        sub_98197C(session, imageId, 0, 0, 20, 32, 3, heightLower, 6, 0, heightLower);
        paint_util_push_tunnel_right(session, heightLower, TUNNEL_0);
    }
    else
    {
        imageId = SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_NE_SW | session->TrackColours[SCHEME_TRACK];
        sub_98197C(session, imageId, 0, 0, 32, 20, 3, heightLower, 0, 6, heightLower);
        paint_util_push_tunnel_left(session, heightLower, TUNNEL_0);
    }

    if (track_paint_util_should_paint_supports(session->MapPosition))
    {
        metal_a_supports_paint_setup(
            session, (direction & 1) ? METAL_SUPPORTS_STICK_ALT : METAL_SUPPORTS_STICK, 4, -1, heightLower,
            session->TrackColours[SCHEME_SUPPORTS]);
    }

    paint_util_set_segment_support_height(
        session, paint_util_rotate_segments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 16, 0x20);
}

static void submarine_ride_paint_track_left_quarter_turn_3_tiles(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    track_paint_util_left_quarter_turn_3_tiles_paint(
        session, 3, height - 16, direction, trackSequence, session->TrackColours[SCHEME_TRACK],
        trackSpritesSubmarineRideMiniHelicoptersQuarterTurn3Tiles);
    track_paint_util_left_quarter_turn_3_tiles_tunnel(session, height - 16, TUNNEL_0, direction, trackSequence);

    switch (trackSequence)
    {
        case 0:
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_STICK, 4, -1, height - 16, session->TrackColours[SCHEME_SUPPORTS]);
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_B4, direction), 0xFFFF, 0);
            break;
        case 2:
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_C8 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_B8, direction), 0xFFFF, 0);
            break;
        case 3:
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_STICK, 4, -1, height - 16, session->TrackColours[SCHEME_SUPPORTS]);
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_C8 | SEGMENT_C4 | SEGMENT_D4 | SEGMENT_C0, direction), 0xFFFF, 0);
            break;
    }

    paint_util_set_general_support_height(session, height + 16, 0x20);
}

static constexpr const uint8_t submarine_ride_right_quarter_turn_3_tiles_to_left_turn_map[] = { 3, 1, 2, 0 };
static void submarine_ride_paint_track_right_quarter_turn_3_tiles(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    trackSequence = submarine_ride_right_quarter_turn_3_tiles_to_left_turn_map[trackSequence];
    submarine_ride_paint_track_left_quarter_turn_3_tiles(
        session, rideIndex, trackSequence, (direction + 3) % 4, height, tileElement);
}

static void submarine_ride_paint_track_left_quarter_turn_1_tile(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    track_paint_util_left_quarter_turn_1_tile_paint(
        session, 1, height - 16, 0, direction, session->TrackColours[SCHEME_TRACK],
        trackSpritesSubmarineRideMiniHelicoptersQuarterTurn1Tile);
    track_paint_util_left_quarter_turn_1_tile_tunnel(session, direction, height - 16, 0, TUNNEL_0, 0, TUNNEL_0);

    paint_util_set_segment_support_height(
        session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C8 | SEGMENT_C4 | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 16, 0x20);
}

static void submarine_ride_paint_track_right_quarter_turn_1_tile(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    submarine_ride_paint_track_left_quarter_turn_1_tile(
        session, rideIndex, trackSequence, (direction + 3) % 4, height, tileElement);
}

/**
 * rct2: 0x008995D4
 */
TRACK_PAINT_FUNCTION get_track_paint_function_submarine_ride(int32_t trackType)
{
    switch (trackType)
    {
        case TrackElemType::BeginStation:
        case TrackElemType::MiddleStation:
        case TrackElemType::EndStation:
            return submarine_ride_paint_track_station;

        case TrackElemType::Flat:
            return submarine_ride_paint_track_flat;

        case TrackElemType::LeftQuarterTurn3Tiles:
            return submarine_ride_paint_track_left_quarter_turn_3_tiles;
        case TrackElemType::RightQuarterTurn3Tiles:
            return submarine_ride_paint_track_right_quarter_turn_3_tiles;

        case TrackElemType::LeftQuarterTurn1Tile:
            return submarine_ride_paint_track_left_quarter_turn_1_tile;
        case TrackElemType::RightQuarterTurn1Tile:
            return submarine_ride_paint_track_right_quarter_turn_1_tile;
    }

    return nullptr;
}
