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

/**
 * rct2: 0x0077084A
 */
static void paint_circus_tent(
    paint_session* session, ride_id_t rideIndex, uint8_t direction, int8_t al, int8_t cl, uint16_t height)
{
    const TileElement* savedTileElement = static_cast<const TileElement*>(session->CurrentlyDrawnItem);

    auto ride = get_ride(rideIndex);
    if (ride == nullptr)
        return;

    auto rideEntry = ride->GetRideEntry();
    if (rideEntry == nullptr)
        return;

    auto vehicle = GetEntity<Vehicle>(ride->vehicles[0]);
    if (ride->lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK && vehicle != nullptr)
    {
        session->InteractionType = ViewportInteractionItem::Entity;
        session->CurrentlyDrawnItem = vehicle;
    }

    uint32_t imageColourFlags = session->TrackColours[SCHEME_MISC];
    uint32_t imageId = rideEntry->vehicles[0].base_image_id + direction;
    if (imageColourFlags == IMAGE_TYPE_REMAP)
    {
        imageColourFlags = SPRITE_ID_PALETTE_COLOUR_2(ride->vehicle_colours[0].Body, ride->vehicle_colours[0].Trim);
    }

    PaintAddImageAsParent(session, imageId | imageColourFlags, al, cl, 24, 24, 47, height + 3, al + 16, cl + 16, height + 3);

    session->CurrentlyDrawnItem = savedTileElement;
    session->InteractionType = ViewportInteractionItem::Ride;
}
/**
 * rct2: 0x0076FAD4
 */
static void paint_circus(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    trackSequence = track_map_3x3[direction][trackSequence];

    int32_t edges = edges_3x3[trackSequence];

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
            paint_circus_tent(session, rideIndex, direction, 32, 32, height);
            break;
        case 3:
            paint_circus_tent(session, rideIndex, direction, 32, -32, height);
            break;
        case 5:
            paint_circus_tent(session, rideIndex, direction, 0, -32, height);
            break;
        case 6:
            paint_circus_tent(session, rideIndex, direction, -32, 32, height);
            break;
        case 7:
            paint_circus_tent(session, rideIndex, direction, -32, -32, height);
            break;
        case 8:
            paint_circus_tent(session, rideIndex, direction, -32, 0, height);
            break;
    }

    int32_t cornerSegments = 0;
    switch (trackSequence)
    {
        case 1:
            // Top
            cornerSegments = SEGMENT_B4 | SEGMENT_C8 | SEGMENT_CC;
            break;
        case 3:
            // Right
            cornerSegments = SEGMENT_CC | SEGMENT_BC | SEGMENT_D4;
            break;
        case 6:
            // Left
            cornerSegments = SEGMENT_C8 | SEGMENT_B8 | SEGMENT_D0;
            break;
        case 7:
            // Bottom
            cornerSegments = SEGMENT_D0 | SEGMENT_C0 | SEGMENT_D4;
            break;
    }

    paint_util_set_segment_support_height(session, cornerSegments, height + 2, 0x20);
    paint_util_set_segment_support_height(session, SEGMENTS_ALL & ~cornerSegments, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 128, 0x20);
}

/**
 * rct2: 0x0076F8D4
 */
TRACK_PAINT_FUNCTION get_track_paint_function_circus(int32_t trackType)
{
    if (trackType != TrackElemType::FlatTrack3x3)
    {
        return nullptr;
    }

    return paint_circus;
}
