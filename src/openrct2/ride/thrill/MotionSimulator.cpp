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
#include "../../world/Sprite.h"
#include "../Track.h"
#include "../TrackPaint.h"

enum
{
    SPR_MOTION_SIMULATOR_STAIRS_R0 = 22154,
    SPR_MOTION_SIMULATOR_STAIRS_R1 = 22155,
    SPR_MOTION_SIMULATOR_STAIRS_R2 = 22156,
    SPR_MOTION_SIMULATOR_STAIRS_R3 = 22157,
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
    paint_session* session, int8_t offsetX, int8_t offsetY, uint8_t direction, int32_t height, const TileElement* tileElement)
{
    auto ride = get_ride(tileElement->AsTrack()->GetRideIndex());
    if (ride == nullptr)
        return;

    auto rideEntry = ride->GetRideEntry();
    if (rideEntry == nullptr)
        return;

    const TileElement* savedTileElement = static_cast<const TileElement*>(session->CurrentlyDrawnItem);

    Vehicle* vehicle = nullptr;
    if (ride->lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK)
    {
        uint16_t spriteIndex = ride->vehicles[0];
        if (spriteIndex != SPRITE_INDEX_NULL)
        {
            vehicle = GetEntity<Vehicle>(spriteIndex);
            session->InteractionType = ViewportInteractionItem::Entity;
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
        imageColourFlags = SPRITE_ID_PALETTE_COLOUR_2(ride->vehicle_colours[0].Body, ride->vehicle_colours[0].Trim);
    }
    simulatorImageId |= imageColourFlags;

    int16_t offsetZ = height + 2;
    uint32_t imageId;
    switch (direction)
    {
        case 0:
            // Simulator
            imageId = simulatorImageId;
            PaintAddImageAsParent(session, imageId, offsetX, offsetY, 20, 20, 44, offsetZ, offsetX, offsetY, offsetZ);
            // Stairs
            imageId = (SPR_MOTION_SIMULATOR_STAIRS_R0 + direction) | session->TrackColours[SCHEME_MISC];
            PaintAddImageAsChild(session, imageId, offsetX, offsetY, 20, 20, 44, offsetZ, offsetX, offsetY, offsetZ);
            // Stairs (rail)
            imageId = (SPR_MOTION_SIMULATOR_STAIRS_RAIL_R0 + direction) | session->TrackColours[SCHEME_MISC];
            PaintAddImageAsParent(session, imageId, offsetX, offsetY, 20, 2, 44, offsetZ, offsetX, offsetY + 32, offsetZ);
            break;
        case 1:
            // Simulator
            imageId = simulatorImageId;
            PaintAddImageAsParent(session, imageId, offsetX, offsetY, 20, 20, 44, offsetZ, offsetX, offsetY, offsetZ);
            // Stairs
            imageId = (SPR_MOTION_SIMULATOR_STAIRS_R0 + direction) | session->TrackColours[SCHEME_MISC];
            PaintAddImageAsChild(session, imageId, offsetX, offsetY, 20, 20, 44, offsetZ, offsetX, offsetY, offsetZ);
            // Stairs (rail)
            imageId = (SPR_MOTION_SIMULATOR_STAIRS_RAIL_R0 + direction) | session->TrackColours[SCHEME_MISC];
            PaintAddImageAsParent(session, imageId, offsetX, offsetY, 2, 20, 44, offsetZ, offsetX + 34, offsetY, offsetZ);
            break;
        case 2:
            // Stairs (rail)
            imageId = (SPR_MOTION_SIMULATOR_STAIRS_RAIL_R0 + direction) | session->TrackColours[SCHEME_MISC];
            PaintAddImageAsParent(session, imageId, offsetX, offsetY, 20, 2, 44, offsetZ, offsetX, offsetY - 10, offsetZ);
            // Stairs
            imageId = (SPR_MOTION_SIMULATOR_STAIRS_R0 + direction) | session->TrackColours[SCHEME_MISC];
            PaintAddImageAsParent(session, imageId, offsetX, offsetY, 20, 20, 44, offsetZ, offsetX, offsetY + 5, offsetZ);
            // Simulator
            imageId = simulatorImageId;
            PaintAddImageAsChild(session, imageId, offsetX, offsetY, 20, 20, 44, offsetZ, offsetX, offsetY + 5, offsetZ);
            break;
        case 3:
            // Stairs (rail)
            imageId = (SPR_MOTION_SIMULATOR_STAIRS_RAIL_R0 + direction) | session->TrackColours[SCHEME_MISC];
            PaintAddImageAsParent(session, imageId, offsetX, offsetY, 2, 20, 44, offsetZ, offsetX - 10, offsetY, offsetZ);
            // Stairs
            imageId = (SPR_MOTION_SIMULATOR_STAIRS_R0 + direction) | session->TrackColours[SCHEME_MISC];
            PaintAddImageAsParent(session, imageId, offsetX, offsetY, 20, 20, 44, offsetZ, offsetX + 5, offsetY, offsetZ);
            // Simulator
            imageId = simulatorImageId;
            PaintAddImageAsChild(session, imageId, offsetX, offsetY, 20, 20, 44, offsetZ, offsetX + 5, offsetY, offsetZ);
            break;
    }

    session->CurrentlyDrawnItem = savedTileElement;
    session->InteractionType = ViewportInteractionItem::Ride;
}

/** rct2: 0x008A85C4 */
static void paint_motionsimulator(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    trackSequence = track_map_2x2[direction][trackSequence];

    int32_t edges = edges_2x2[trackSequence];

    wooden_a_supports_paint_setup(session, (direction & 1), 0, height, session->TrackColours[SCHEME_MISC], nullptr);
    track_paint_util_paint_floor(session, edges, session->TrackColours[SCHEME_TRACK], height, floorSpritesCork);

    auto ride = get_ride(rideIndex);
    if (ride != nullptr)
    {
        track_paint_util_paint_fences(
            session, edges, session->MapPosition, tileElement, ride, session->TrackColours[SCHEME_SUPPORTS], height,
            fenceSpritesRope, session->CurrentRotation);
    }

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
TRACK_PAINT_FUNCTION get_track_paint_function_motionsimulator(int32_t trackType)
{
    switch (trackType)
    {
        case TrackElemType::FlatTrack2x2:
            return paint_motionsimulator;
    }
    return nullptr;
}
