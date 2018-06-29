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

enum
{
    SPR_MOTION_SIMULATOR_STAIRS_R0      = 22154,
    SPR_MOTION_SIMULATOR_STAIRS_R1      = 22155,
    SPR_MOTION_SIMULATOR_STAIRS_R2      = 22156,
    SPR_MOTION_SIMULATOR_STAIRS_R3      = 22157,
    SPR_MOTION_SIMULATOR_STAIRS_RAIL_R0 = 22158,
    SPR_MOTION_SIMULATOR_STAIRS_RAIL_R1 = 22159,
    SPR_MOTION_SIMULATOR_STAIRS_RAIL_R2 = 22160,
    SPR_MOTION_SIMULATOR_STAIRS_RAIL_R3 = 22161,
};

/**
 *
 *  rct2: 0x0076522A
 */
static void paint_motionsimulator_vehicle(
    paint_session * session, int8_t offsetX, int8_t offsetY, uint8_t direction, int32_t height, const rct_tile_element * tileElement)
{
    Ride *           ride      = get_ride(track_element_get_ride_index(tileElement));
    rct_ride_entry * rideEntry = get_ride_entry_by_ride(ride);

    const rct_tile_element * savedTileElement = static_cast<const rct_tile_element *>(session->CurrentlyDrawnItem);

    rct_vehicle * vehicle = nullptr;
    if (ride->lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK)
    {
        uint16_t spriteIndex = ride->vehicles[0];
        if (spriteIndex != SPRITE_INDEX_NULL)
        {
            vehicle                     = GET_VEHICLE(spriteIndex);
            session->InteractionType    = VIEWPORT_INTERACTION_ITEM_SPRITE;
            session->CurrentlyDrawnItem = vehicle;
        }
    }

    uint32_t simulatorImageId = rideEntry->vehicles[0].base_image_id + direction;
    if (vehicle != nullptr)
    {
        if (vehicle->restraints_position >= 64)
        {
            simulatorImageId += (vehicle->restraints_position >> 6) << 2;
        }
        else
        {
            simulatorImageId += vehicle->vehicle_sprite_type * 4;
        }
    }

    uint32_t imageColourFlags = session->TrackColours[SCHEME_MISC];
    if (imageColourFlags == IMAGE_TYPE_REMAP)
    {
        imageColourFlags =
            SPRITE_ID_PALETTE_COLOUR_2(ride->vehicle_colours[0].body_colour, ride->vehicle_colours[0].trim_colour);
    }
    simulatorImageId |= imageColourFlags;

    int16_t offsetZ = height + 2;
    uint32_t imageId;
    switch (direction)
    {
    case 0:
        // Simulator
        imageId = simulatorImageId;
        sub_98197C(session, imageId, offsetX, offsetY, 20, 20, 44, offsetZ, offsetX, offsetY, offsetZ);
        // Stairs
        imageId = (SPR_MOTION_SIMULATOR_STAIRS_R0 + direction) | session->TrackColours[SCHEME_MISC];
        sub_98199C(session, imageId, offsetX, offsetY, 20, 20, 44, offsetZ, offsetX, offsetY, offsetZ);
        // Stairs (rail)
        imageId = (SPR_MOTION_SIMULATOR_STAIRS_RAIL_R0 + direction) | session->TrackColours[SCHEME_MISC];
        sub_98197C(session, imageId, offsetX, offsetY, 20, 2, 44, offsetZ, offsetX, offsetY + 32, offsetZ);
        break;
    case 1:
        // Simulator
        imageId = simulatorImageId;
        sub_98197C(session, imageId, offsetX, offsetY, 20, 20, 44, offsetZ, offsetX, offsetY, offsetZ);
        // Stairs
        imageId = (SPR_MOTION_SIMULATOR_STAIRS_R0 + direction) | session->TrackColours[SCHEME_MISC];
        sub_98199C(session, imageId, offsetX, offsetY, 20, 20, 44, offsetZ, offsetX, offsetY, offsetZ);
        // Stairs (rail)
        imageId = (SPR_MOTION_SIMULATOR_STAIRS_RAIL_R0 + direction) | session->TrackColours[SCHEME_MISC];
        sub_98197C(session, imageId, offsetX, offsetY, 2, 20, 44, offsetZ, offsetX + 34, offsetY, offsetZ);
        break;
    case 2:
        // Stairs (rail)
        imageId = (SPR_MOTION_SIMULATOR_STAIRS_RAIL_R0 + direction) | session->TrackColours[SCHEME_MISC];
        sub_98197C(session, imageId, offsetX, offsetY, 20, 2, 44, offsetZ, offsetX, offsetY - 10, offsetZ);
        // Stairs
        imageId = (SPR_MOTION_SIMULATOR_STAIRS_R0 + direction) | session->TrackColours[SCHEME_MISC];
        sub_98197C(session, imageId, offsetX, offsetY, 20, 20, 44, offsetZ, offsetX, offsetY + 5, offsetZ);
        // Simulator
        imageId = simulatorImageId;
        sub_98199C(session, imageId, offsetX, offsetY, 20, 20, 44, offsetZ, offsetX, offsetY + 5, offsetZ);
        break;
    case 3:
        // Stairs (rail)
        imageId = (SPR_MOTION_SIMULATOR_STAIRS_RAIL_R0 + direction) | session->TrackColours[SCHEME_MISC];
        sub_98197C(session, imageId, offsetX, offsetY, 2, 20, 44, offsetZ, offsetX - 10, offsetY, offsetZ);
        // Stairs
        imageId = (SPR_MOTION_SIMULATOR_STAIRS_R0 + direction) | session->TrackColours[SCHEME_MISC];
        sub_98197C(session, imageId, offsetX, offsetY, 20, 20, 44, offsetZ, offsetX + 5, offsetY, offsetZ);
        // Simulator
        imageId = simulatorImageId;
        sub_98199C(session, imageId, offsetX, offsetY, 20, 20, 44, offsetZ, offsetX + 5, offsetY, offsetZ);
        break;
    }

    session->CurrentlyDrawnItem = savedTileElement;
    session->InteractionType    = VIEWPORT_INTERACTION_ITEM_RIDE;
}

/** rct2: 0x008A85C4 */
static void paint_motionsimulator(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement)
{
    trackSequence = track_map_2x2[direction][trackSequence];

    int32_t   edges    = edges_2x2[trackSequence];
    Ride *   ride     = get_ride(rideIndex);
    LocationXY16 position = { session->MapPosition.x, session->MapPosition.y };

    wooden_a_supports_paint_setup(session, (direction & 1), 0, height, session->TrackColours[SCHEME_MISC], nullptr);
    track_paint_util_paint_floor(session, edges, session->TrackColours[SCHEME_TRACK], height, floorSpritesCork);
    track_paint_util_paint_fences(
        session, edges, position, tileElement, ride, session->TrackColours[SCHEME_SUPPORTS], height, fenceSpritesRope,
        session->CurrentRotation);

    switch (trackSequence)
    {
    case 1:
        paint_motionsimulator_vehicle(session, 16, -16, direction, height, tileElement);
        break;
    case 2:
        paint_motionsimulator_vehicle(session, -16, 16, direction, height, tileElement);
        break;
    case 3:
        paint_motionsimulator_vehicle(session, -16, -16, direction, height, tileElement);
        break;
    }

    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 128, 0x20);
}

/**
 *
 *  rct2: 0x00763520
 */
TRACK_PAINT_FUNCTION get_track_paint_function_motionsimulator(int32_t trackType, int32_t direction)
{
    switch (trackType)
    {
    case FLAT_TRACK_ELEM_2_X_2:
        return paint_motionsimulator;
    }
    return nullptr;
}
