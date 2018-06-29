/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../interface/Viewport.h"
#include "../../paint/Paint.h"
#include "../../paint/Supports.h"
#include "../Track.h"
#include "../TrackPaint.h"
#include "../../world/Sprite.h"

/** rct2: 0x01428010 */
static constexpr const uint32_t swinging_inverter_ship_base_sprite_offset[] = { 0, 16, 0, 16 };

/** rct2: 0x01428020 */
static constexpr const uint32_t swinging_inverter_ship_animating_base_sprite_offset[] = { 32, 33, 32, 33 };

struct swinging_inverter_ship_bound_box
{
    int16_t length_x;
    int16_t length_y;
    int16_t offset_x;
    int16_t offset_y;
};

/** rct2: 0x01428020 */
static constexpr const swinging_inverter_ship_bound_box swinging_inverter_ship_bounds[] = {
    { 32, 16, 0, 8 }, { 16, 32, 8, 0 }, { 32, 16, 0, 8 }, { 16, 32, 8, 0 }
};

enum
{
    SPR_SWINGING_INVERTER_SHIP_FRAME_0 = 21998,
    SPR_SWINGING_INVERTER_SHIP_FRAME_1 = 21999,
    SPR_SWINGING_INVERTER_SHIP_FRAME_2 = 22000,
    SPR_SWINGING_INVERTER_SHIP_FRAME_3 = 22001,
};

static constexpr const uint32_t swinging_inverter_ship_frame_sprites[] = { SPR_SWINGING_INVERTER_SHIP_FRAME_0,
                                                               SPR_SWINGING_INVERTER_SHIP_FRAME_1,
                                                               SPR_SWINGING_INVERTER_SHIP_FRAME_2,
                                                               SPR_SWINGING_INVERTER_SHIP_FRAME_3 };

static void paint_swinging_inverter_ship_structure(paint_session * session, Ride * ride, uint8_t direction, int8_t axisOffset,
                                                   uint16_t height)
{
    const rct_tile_element * savedTileElement = static_cast<const rct_tile_element *>(session->CurrentlyDrawnItem);

    rct_ride_entry * rideEntry = get_ride_entry(ride->subtype);
    rct_vehicle *    vehicle  = nullptr;

    int8_t xOffset = !(direction & 1) ? axisOffset : 0;
    int8_t yOffset = (direction & 1) ? axisOffset : 0;

    if (ride->lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK && ride->vehicles[0] != SPRITE_INDEX_NULL)
    {
        vehicle = GET_VEHICLE(ride->vehicles[0]);

        session->InteractionType    = VIEWPORT_INTERACTION_ITEM_SPRITE;
        session->CurrentlyDrawnItem = vehicle;
    }

    uint32_t vehicleImageId = rideEntry->vehicles[0].base_image_id + swinging_inverter_ship_base_sprite_offset[direction];
    if (vehicle != nullptr)
    {
        int32_t rotation = (int8_t)vehicle->vehicle_sprite_type;
        if (rotation != 0)
        {
            vehicleImageId =
                rideEntry->vehicles[0].base_image_id + swinging_inverter_ship_animating_base_sprite_offset[direction];

            if (direction & 2)
            {
                rotation = -rotation;
            }
            if (rotation < 0)
            {
                rotation += 72;
            }

            vehicleImageId += (rotation - 1) << 1;
        }
    }

    uint32_t colourFlags = session->TrackColours[SCHEME_MISC];
    if (colourFlags == IMAGE_TYPE_REMAP)
    {
        colourFlags = SPRITE_ID_PALETTE_COLOUR_2(ride->vehicle_colours[0].body_colour, ride->vehicle_colours[0].trim_colour);
    }

    swinging_inverter_ship_bound_box boundBox = swinging_inverter_ship_bounds[direction];
    vehicleImageId                            = vehicleImageId | colourFlags;
    uint32_t frameImageId = swinging_inverter_ship_frame_sprites[direction] | session->TrackColours[SCHEME_TRACK];

    if (direction & 2)
    {
        sub_98197C(
            session, vehicleImageId, xOffset, yOffset, boundBox.length_x, boundBox.length_y, 127, height, boundBox.offset_x,
            boundBox.offset_y, height);
        sub_98199C(
            session, frameImageId, xOffset, yOffset, boundBox.length_x, boundBox.length_y, 127, height, boundBox.offset_x,
            boundBox.offset_y, height);
    }
    else
    {
        sub_98197C(
            session, frameImageId, xOffset, yOffset, boundBox.length_x, boundBox.length_y, 127, height, boundBox.offset_x,
            boundBox.offset_y, height);
        sub_98199C(
            session, vehicleImageId, xOffset, yOffset, boundBox.length_x, boundBox.length_y, 127, height, boundBox.offset_x,
            boundBox.offset_y, height);
    }

    session->CurrentlyDrawnItem = savedTileElement;
    session->InteractionType    = VIEWPORT_INTERACTION_ITEM_RIDE;
}

/** rct2: 0x00760260 */
static void paint_swinging_inverter_ship(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement)
{
    uint8_t relativeTrackSequence = track_map_1x4[direction][trackSequence];

    Ride * ride = get_ride(rideIndex);

    uint32_t imageId;

    if (relativeTrackSequence != 1 && relativeTrackSequence != 3)
    {
        if (direction & 1)
        {
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 6, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 7, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
        }
        else
        {
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 8, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
        }

        imageId = SPR_STATION_BASE_D | session->TrackColours[SCHEME_SUPPORTS];
        sub_98196C(session, imageId, 0, 0, 32, 32, 1, height);

        switch (direction)
        {
        case 0:
            imageId = SPR_STATION_PLATFORM_SW_NE | session->TrackColours[SCHEME_TRACK];
            sub_98196C(session, imageId, 0, 24, 32, 8, 1, height + 9);
            break;
        case 1:
            imageId = SPR_STATION_PLATFORM_NW_SE | session->TrackColours[SCHEME_TRACK];
            sub_98196C(session, imageId, 24, 0, 8, 32, 1, height + 9);
            break;
        case 2:
            imageId = SPR_STATION_PLATFORM_SW_NE | session->TrackColours[SCHEME_TRACK];
            sub_98199C(session, imageId, 0, 0, 32, 8, 1, height + 9, -2, 0, height);
            break;
        case 3:
            imageId = SPR_STATION_PLATFORM_NW_SE | session->TrackColours[SCHEME_TRACK];
            sub_98199C(session, imageId, 0, 0, 8, 32, 1, height + 9, 0, -2, height);
            break;
        }
    }

    switch (relativeTrackSequence)
    {
    case 1:
        paint_swinging_inverter_ship_structure(session, ride, direction, 48, height + 7);
        break;
    case 2:
        paint_swinging_inverter_ship_structure(session, ride, direction, 16, height + 7);
        break;
    case 0:
        paint_swinging_inverter_ship_structure(session, ride, direction, -16, height + 7);
        break;
    case 3:
        paint_swinging_inverter_ship_structure(session, ride, direction, -48, height + 7);
        break;
    }

    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 176, 0x20);
}

/**
 * rct2: 0x00760070
 */
TRACK_PAINT_FUNCTION get_track_paint_function_swinging_inverter_ship(int32_t trackType, int32_t direction)
{
    if (trackType != FLAT_TRACK_ELEM_1_X_4_B)
    {
        return nullptr;
    }

    return paint_swinging_inverter_ship;
}
