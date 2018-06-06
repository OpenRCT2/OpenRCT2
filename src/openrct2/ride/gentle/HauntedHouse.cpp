#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#include "../../interface/Viewport.h"
#include "../../paint/Paint.h"
#include "../../paint/Supports.h"
#include "../Track.h"
#include "../TrackPaint.h"
#include "../../world/Sprite.h"

struct haunted_house_bound_box
{
    sint16 offset_x;
    sint16 offset_y;
    sint16 length_x;
    sint16 length_y;
};

/** rct2: 0x1428180 */
static haunted_house_bound_box haunted_house_data[] = {
    { 6, 0, 42, 24 }, { 0, 0, 0, 0 }, { -16, -16, 32, 32 }, { 0, 0, 0, 0 }, { 0, 6, 24, 42 }, { 0, 0, 0, 0 },
};

/**
 * rct2: 0x0076F72C
 */
static void paint_haunted_house_structure(paint_session * session, uint8 rideIndex, uint8 direction, sint8 xOffset,
                                          sint8 yOffset, uint8 part, uint16 height)
{
    const rct_tile_element * savedTileElement = static_cast<const rct_tile_element *>(session->CurrentlyDrawnItem);

    uint8 frameNum = 0;

    Ride *           ride      = get_ride(rideIndex);
    rct_ride_entry * rideEntry = get_ride_entry(ride->subtype);

    uint32 baseImageId = rideEntry->vehicles[0].base_image_id;

    if (ride->lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK && ride->vehicles[0] != SPRITE_INDEX_NULL)
    {
        session->InteractionType    = VIEWPORT_INTERACTION_ITEM_SPRITE;
        rct_vehicle * vehicle       = GET_VEHICLE(ride->vehicles[0]);
        session->CurrentlyDrawnItem = vehicle;
        frameNum                    = vehicle->vehicle_sprite_type;
    }

    uint32                  imageId  = (baseImageId + direction) | session->TrackColours[SCHEME_MISC];
    haunted_house_bound_box boundBox = haunted_house_data[part];
    sub_98197C(
        session, imageId, xOffset, yOffset, boundBox.length_x, boundBox.length_y, 127, height, boundBox.offset_x,
        boundBox.offset_y, height);

    rct_drawpixelinfo * dpi = session->DPI;
    if (dpi->zoom_level == 0 && frameNum != 0)
    {
        switch (direction)
        {
        case 0:
            imageId = baseImageId + 3 + frameNum;
            break;
        case 1:
            imageId = baseImageId + 21 + frameNum;
            break;
        case 2:
            imageId = baseImageId + 39 + frameNum;
            break;
        case 3:
            imageId = baseImageId + 57 + frameNum;
            break;
        }
        imageId = imageId | session->TrackColours[SCHEME_MISC];
        sub_98199C(
            session, imageId, xOffset, yOffset, boundBox.length_x, boundBox.length_y, 127, height, boundBox.offset_x,
            boundBox.offset_y, height);
    }

    session->CurrentlyDrawnItem = savedTileElement;
    session->InteractionType    = VIEWPORT_INTERACTION_ITEM_RIDE;
}

/**
 * rct2: 0x0076E9B0
 */
static void paint_haunted_house(
    paint_session *          session,
    uint8                    rideIndex,
    uint8                    trackSequence,
    uint8                    direction,
    sint32                   height,
    const rct_tile_element * tileElement)
{
    trackSequence = track_map_3x3[direction][trackSequence];

    sint32   edges    = edges_3x3[trackSequence];
    Ride *   ride     = get_ride(rideIndex);
    LocationXY16 position = session->MapPosition;

    wooden_a_supports_paint_setup(session, (direction & 1), 0, height, session->TrackColours[SCHEME_MISC], nullptr);

    track_paint_util_paint_floor(session, edges, session->TrackColours[SCHEME_TRACK], height, floorSpritesCork);

    track_paint_util_paint_fences(
        session, edges, position, tileElement, ride, session->TrackColours[SCHEME_MISC], height, fenceSpritesRope,
        session->CurrentRotation);

    switch (trackSequence)
    {
    case 3:
        paint_haunted_house_structure(session, rideIndex, direction, 32, -32, 0, height + 3);
        break;
    case 6:
        paint_haunted_house_structure(session, rideIndex, direction, -32, 32, 4, height + 3);
        break;
    case 7:
        paint_haunted_house_structure(session, rideIndex, direction, -32, -32, 2, height + 3);
        break;
    }

    sint32 cornerSegments = 0;
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

/**
 * rct2: 0x0076E7B0
 */
TRACK_PAINT_FUNCTION get_track_paint_function_haunted_house(sint32 trackType, sint32 direction)
{
    if (trackType != FLAT_TRACK_ELEM_3_X_3)
    {
        return nullptr;
    }

    return paint_haunted_house;
}
