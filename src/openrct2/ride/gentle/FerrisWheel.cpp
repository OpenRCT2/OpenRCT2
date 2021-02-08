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

static constexpr const uint8_t edges_1x4_ne_sw[] = {
    EDGE_NW | EDGE_SE,
    EDGE_NW | EDGE_SE | EDGE_NE,
    EDGE_NW | EDGE_SE,
    EDGE_NW | EDGE_SE | EDGE_SW,
};

static constexpr const uint8_t edges_1x4_nw_se[] = {
    EDGE_NE | EDGE_SW,
    EDGE_NE | EDGE_SW | EDGE_NW,
    EDGE_NE | EDGE_SW,
    EDGE_NE | EDGE_SW | EDGE_SE,
};

struct ferris_wheel_bound_box
{
    int16_t length_x;
    int16_t length_y;
    int16_t offset_x;
    int16_t offset_y;
};

/** rct2: 0x008A8CA8 */
static ferris_wheel_bound_box ferris_wheel_data[] = {
    { 31, 16, 1, 8 },
    { 16, 31, 8, 1 },
    { 31, 16, 1, 8 },
    { 16, 31, 8, 1 },
};

/**
 * rct2: 0x004C3874
 */
static void paint_ferris_wheel_structure(
    paint_session* session, ride_id_t rideIndex, uint8_t direction, int8_t axisOffset, uint16_t height)
{
    uint32_t imageId, baseImageId;

    const TileElement* savedTileElement = static_cast<const TileElement*>(session->CurrentlyDrawnItem);

    auto ride = get_ride(rideIndex);
    if (ride == nullptr)
        return;

    auto rideEntry = ride->GetRideEntry();
    if (rideEntry == nullptr)
        return;

    int8_t xOffset = !(direction & 1) ? axisOffset : 0;
    int8_t yOffset = (direction & 1) ? axisOffset : 0;

    height += 7;

    baseImageId = rideEntry->vehicles[0].base_image_id;

    auto vehicle = GetEntity<Vehicle>(ride->vehicles[0]);
    if (ride->lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK && vehicle != nullptr)
    {
        session->InteractionType = ViewportInteractionItem::Entity;
        session->CurrentlyDrawnItem = vehicle;
    }

    uint32_t imageOffset = 0;
    if (vehicle != nullptr)
    {
        imageOffset = vehicle->vehicle_sprite_type % 8;
    }

    uint32_t imageColourFlags = session->TrackColours[SCHEME_MISC];
    if (imageColourFlags == IMAGE_TYPE_REMAP)
    {
        imageColourFlags = SPRITE_ID_PALETTE_COLOUR_2(ride->vehicle_colours[0].Body, ride->vehicle_colours[0].Trim);
    }

    ferris_wheel_bound_box boundBox = ferris_wheel_data[direction];

    imageId = (22150 + (direction & 1) * 2) | session->TrackColours[SCHEME_TRACK];
    PaintAddImageAsParent(
        session, imageId, xOffset, yOffset, boundBox.length_x, boundBox.length_y, 127, height, boundBox.offset_x,
        boundBox.offset_y, height);

    imageId = (baseImageId + direction * 8 + imageOffset) | imageColourFlags;
    PaintAddImageAsChild(
        session, imageId, xOffset, yOffset, boundBox.length_x, boundBox.length_y, 127, height, boundBox.offset_x,
        boundBox.offset_y, height);

    if (ride->lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK && vehicle != nullptr)
    {
        for (int32_t i = 0; i < 32; i += 2)
        {
            auto* peep = GetEntity<Guest>(vehicle->peep[i]);
            if (peep == nullptr || peep->State != PeepState::OnRide)
            {
                continue;
            }

            int32_t frameNum = (vehicle->vehicle_sprite_type + i * 4) % 128;
            imageColourFlags = SPRITE_ID_PALETTE_COLOUR_2(vehicle->peep_tshirt_colours[i], vehicle->peep_tshirt_colours[i + 1]);
            imageId = (baseImageId + 32 + direction * 128 + frameNum) | imageColourFlags;
            PaintAddImageAsChild(
                session, imageId, xOffset, yOffset, boundBox.length_x, boundBox.length_y, 127, height, boundBox.offset_x,
                boundBox.offset_y, height);
        }
    }

    imageId = (22150 + (direction & 1) * 2 + 1) | session->TrackColours[SCHEME_TRACK];
    PaintAddImageAsChild(
        session, imageId, xOffset, yOffset, boundBox.length_x, boundBox.length_y, 127, height, boundBox.offset_x,
        boundBox.offset_y, height);

    session->CurrentlyDrawnItem = savedTileElement;
    session->InteractionType = ViewportInteractionItem::Ride;
}

/**
 * rct2: 0x008A8EC4
 */
static void paint_ferris_wheel(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    uint8_t relativeTrackSequence = track_map_1x4[direction][trackSequence];

    int32_t edges;
    if (direction & 1)
    {
        edges = edges_1x4_nw_se[relativeTrackSequence];
    }
    else
    {
        edges = edges_1x4_ne_sw[relativeTrackSequence];
    }

    wooden_a_supports_paint_setup(session, direction & 1, 0, height, session->TrackColours[SCHEME_MISC], nullptr);

    track_paint_util_paint_floor(session, edges, session->TrackColours[SCHEME_TRACK], height, floorSpritesCork);

    uint32_t imageId;
    uint8_t rotation = session->CurrentRotation;
    uint32_t colourFlags = session->TrackColours[SCHEME_MISC];
    auto ride = get_ride(rideIndex);
    if (ride != nullptr)
    {
        if (edges & EDGE_NW && track_paint_util_has_fence(EDGE_NW, session->MapPosition, tileElement, ride, rotation))
        {
            imageId = SPR_FENCE_ROPE_NW | colourFlags;
            PaintAddImageAsChild(session, imageId, 0, 0, 32, 1, 7, height, 0, 2, height + 2);
        }
        if (edges & EDGE_NE && track_paint_util_has_fence(EDGE_NE, session->MapPosition, tileElement, ride, rotation))
        {
            imageId = SPR_FENCE_ROPE_NE | colourFlags;
            PaintAddImageAsChild(session, imageId, 0, 0, 1, 32, 7, height, 2, 0, height + 2);
        }
        if (edges & EDGE_SE && track_paint_util_has_fence(EDGE_SE, session->MapPosition, tileElement, ride, rotation))
        {
            // Bound box is slightly different from track_paint_util_paint_fences
            imageId = SPR_FENCE_ROPE_SE | colourFlags;
            PaintAddImageAsParent(session, imageId, 0, 0, 28, 1, 7, height, 0, 29, height + 3);
        }
        if (edges & EDGE_SW && track_paint_util_has_fence(EDGE_SW, session->MapPosition, tileElement, ride, rotation))
        {
            imageId = SPR_FENCE_ROPE_SW | colourFlags;
            PaintAddImageAsParent(session, imageId, 0, 0, 1, 32, 7, height, 30, 0, height + 2);
        }
    }

    switch (relativeTrackSequence)
    {
        case 1:
            paint_ferris_wheel_structure(session, rideIndex, direction, 48, height);
            break;
        case 2:
            paint_ferris_wheel_structure(session, rideIndex, direction, 16, height);
            break;
        case 0:
            paint_ferris_wheel_structure(session, rideIndex, direction, -16, height);
            break;
        case 3:
            paint_ferris_wheel_structure(session, rideIndex, direction, -48, height);
            break;
    }

    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 176, 0x20);
}

/**
 * rct2: 0x008A8CC8
 */
TRACK_PAINT_FUNCTION get_track_paint_function_ferris_wheel(int32_t trackType)
{
    if (trackType != TrackElemType::FlatTrack1x4C)
    {
        return nullptr;
    }

    return paint_ferris_wheel;
}
