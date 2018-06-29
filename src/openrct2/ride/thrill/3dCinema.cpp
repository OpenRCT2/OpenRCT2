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

/**
 * rct2: 0x007664C2
 */
static void paint_3d_cinema_structure(paint_session * session, uint8_t rideIndex, uint8_t direction, int8_t xOffset, int8_t yOffset,
                                      uint16_t height)
{
    const rct_tile_element * savedTileElement = static_cast<const rct_tile_element *>(session->CurrentlyDrawnItem);

    Ride *           ride      = get_ride(rideIndex);
    rct_ride_entry * rideEntry = get_ride_entry(ride->subtype);

    if (rideEntry == nullptr)
    {
        return;
    }

    if (ride->lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK && ride->vehicles[0] != SPRITE_INDEX_NULL)
    {
        session->InteractionType    = VIEWPORT_INTERACTION_ITEM_SPRITE;
        session->CurrentlyDrawnItem = GET_VEHICLE(ride->vehicles[0]);
    }

    uint32_t imageColourFlags = session->TrackColours[SCHEME_MISC];
    if (imageColourFlags == IMAGE_TYPE_REMAP)
    {
        imageColourFlags =
            SPRITE_ID_PALETTE_COLOUR_2(ride->vehicle_colours[0].body_colour, ride->vehicle_colours[0].trim_colour);
    }

    uint32_t imageId = (rideEntry->vehicles[0].base_image_id + direction) | imageColourFlags;
    sub_98197C(session, imageId, xOffset, yOffset, 24, 24, 47, height + 3, xOffset + 16, yOffset + 16, height + 3);

    session->CurrentlyDrawnItem = savedTileElement;
    session->InteractionType    = VIEWPORT_INTERACTION_ITEM_RIDE;
}

/**
 * rct2: 0x0076574C
 */
static void paint_3d_cinema(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement)
{
    trackSequence = track_map_3x3[direction][trackSequence];

    int32_t   edges    = edges_3x3[trackSequence];
    Ride *   ride     = get_ride(rideIndex);
    LocationXY16 position = session->MapPosition;

    wooden_a_supports_paint_setup(session, (direction & 1), 0, height, session->TrackColours[SCHEME_MISC], nullptr);

    track_paint_util_paint_floor(session, edges, session->TrackColours[SCHEME_TRACK], height, floorSpritesCork);

    track_paint_util_paint_fences(
        session, edges, position, tileElement, ride, session->TrackColours[SCHEME_MISC], height, fenceSpritesRope,
        session->CurrentRotation);

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
TRACK_PAINT_FUNCTION get_track_paint_function_3d_cinema(int32_t trackType, int32_t direction)
{
    if (trackType != FLAT_TRACK_ELEM_3_X_3)
    {
        return nullptr;
    }

    return paint_3d_cinema;
}
