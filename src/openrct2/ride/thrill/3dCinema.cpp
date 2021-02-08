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
 * rct2: 0x007664C2
 */
static void paint_3d_cinema_structure(
    paint_session* session, ride_id_t rideIndex, uint8_t direction, int8_t xOffset, int8_t yOffset, uint16_t height)
{
    const TileElement* savedTileElement = static_cast<const TileElement*>(session->CurrentlyDrawnItem);

    auto ride = get_ride(rideIndex);
    if (ride == nullptr)
        return;

    auto rideEntry = ride->GetRideEntry();
    if (rideEntry == nullptr)
        return;

    if (ride->lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK && ride->vehicles[0] != SPRITE_INDEX_NULL)
    {
        session->InteractionType = ViewportInteractionItem::Entity;
        session->CurrentlyDrawnItem = GetEntity<Vehicle>(ride->vehicles[0]);
    }

    uint32_t imageColourFlags = session->TrackColours[SCHEME_MISC];
    if (imageColourFlags == IMAGE_TYPE_REMAP)
    {
        imageColourFlags = SPRITE_ID_PALETTE_COLOUR_2(ride->vehicle_colours[0].Body, ride->vehicle_colours[0].Trim);
    }

    uint32_t imageId = (rideEntry->vehicles[0].base_image_id + direction) | imageColourFlags;
    PaintAddImageAsParent(session, imageId, xOffset, yOffset, 24, 24, 47, height + 3, xOffset + 16, yOffset + 16, height + 3);

    session->CurrentlyDrawnItem = savedTileElement;
    session->InteractionType = ViewportInteractionItem::Ride;
}

/**
 * rct2: 0x0076574C
 */
static void paint_3d_cinema(
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
            session, edges, session->MapPosition, tileElement, ride, session->TrackColours[SCHEME_MISC], height,
            fenceSpritesRope, session->CurrentRotation);
    }

    switch (trackSequence)
    {
        case 1:
            paint_3d_cinema_structure(session, rideIndex, direction, 32, 32, height);
            break;
        case 3:
            paint_3d_cinema_structure(session, rideIndex, direction, 32, -32, height);
            break;
        case 5:
            paint_3d_cinema_structure(session, rideIndex, direction, 0, -32, height);
            break;
        case 6:
            paint_3d_cinema_structure(session, rideIndex, direction, -32, 32, height);
            break;
        case 7:
            paint_3d_cinema_structure(session, rideIndex, direction, -32, -32, height);
            break;
        case 8:
            paint_3d_cinema_structure(session, rideIndex, direction, -32, 0, height);
            break;
    }

    int32_t cornerSegments = 0;
    switch (trackSequence)
    {
        case 1:
            // top
            cornerSegments = SEGMENT_B4 | SEGMENT_C8 | SEGMENT_CC;
            break;
        case 3:
            // right
            cornerSegments = SEGMENT_CC | SEGMENT_BC | SEGMENT_D4;
            break;
        case 6:
            // left
            cornerSegments = SEGMENT_C8 | SEGMENT_B8 | SEGMENT_D0;
            break;
        case 7:
            // bottom
            cornerSegments = SEGMENT_D0 | SEGMENT_C0 | SEGMENT_D4;
            break;
    }

    paint_util_set_segment_support_height(session, cornerSegments, height + 2, 0x20);
    paint_util_set_segment_support_height(session, SEGMENTS_ALL & ~cornerSegments, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 128, 0x20);
}

/* 0x0076554C */
TRACK_PAINT_FUNCTION get_track_paint_function_3d_cinema(int32_t trackType)
{
    if (trackType != TrackElemType::FlatTrack3x3)
    {
        return nullptr;
    }

    return paint_3d_cinema;
}
