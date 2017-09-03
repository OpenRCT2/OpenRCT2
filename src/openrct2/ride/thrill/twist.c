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

#include "../../common.h"
#include "../../interface/viewport.h"
#include "../track_paint.h"
#include "../../paint/supports.h"
#include "../../paint/paint.h"
#include "../track.h"

/** rct2: 0x0076E5C9 */
static void paint_twist_structure(paint_session * session, rct_ride * ride, uint8 direction, sint8 xOffset, sint8 yOffset, uint16 height)
{
    rct_map_element * savedMapElement = session->CurrentlyDrawnItem;

    rct_ride_entry * rideEntry = get_ride_entry(ride->subtype);
    rct_vehicle * vehicle = NULL;

    height += 7;
    uint32 baseImageId = rideEntry->vehicles[0].base_image_id;

    if (ride->lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK
        && ride->vehicles[0] != SPRITE_INDEX_NULL) {
        vehicle = GET_VEHICLE(ride->vehicles[0]);

        session->InteractionType = VIEWPORT_INTERACTION_ITEM_SPRITE;
        session->CurrentlyDrawnItem = vehicle;
    }

    uint32 frameNum = (direction * 88) % 216;
    if (vehicle != NULL) {
        frameNum += (vehicle->sprite_direction >> 3) << 4;
        frameNum += vehicle->vehicle_sprite_type;
        frameNum = frameNum % 216;
    }


    uint32 imageColourFlags = gTrackColours[SCHEME_MISC];
    if (imageColourFlags == IMAGE_TYPE_REMAP)
    {
        imageColourFlags = SPRITE_ID_PALETTE_COLOUR_2(ride->vehicle_colours[0].body_colour, ride->vehicle_colours[0].trim_colour);
    }

    uint32 structureFrameNum = frameNum % 24;
    uint32 imageId = (baseImageId + structureFrameNum) | imageColourFlags;
    sub_98197C(session, imageId, xOffset, yOffset, 24, 24, 48, height, xOffset + 16, yOffset + 16, height, get_current_rotation());

    rct_drawpixelinfo * dpi = session->Unk140E9A8;

    if (dpi->zoom_level < 1
        && ride->lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK
        && vehicle != NULL) {

        for (sint32 i = 0; i < vehicle->num_peeps; i += 2) {
            imageColourFlags = SPRITE_ID_PALETTE_COLOUR_2(vehicle->peep_tshirt_colours[i], vehicle->peep_tshirt_colours[i + 1]);

            uint32 peepFrameNum = (frameNum + i * 12) % 216;
            imageId = (baseImageId + 24 + peepFrameNum) | imageColourFlags;
            sub_98199C(session, imageId, xOffset, yOffset, 24, 24, 48, height, xOffset + 16, yOffset + 16, height, get_current_rotation());
        }
    }

    session->CurrentlyDrawnItem = savedMapElement;
    session->InteractionType = VIEWPORT_INTERACTION_ITEM_RIDE;
}


/** rct2: 0x0076D858 */
static void paint_twist(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    trackSequence = track_map_3x3[direction][trackSequence];

    const uint8 edges = edges_3x3[trackSequence];
    rct_ride * ride = get_ride(rideIndex);
    rct_xy16 position = session->MapPosition;

    uint32 imageId;

    wooden_a_supports_paint_setup(session, (direction & 1), 0, height, gTrackColours[SCHEME_MISC], NULL);

    track_paint_util_paint_floor(session, edges, gTrackColours[SCHEME_MISC], height, floorSpritesCork, get_current_rotation());

    switch (trackSequence) {
        case 7:
            if (track_paint_util_has_fence(EDGE_SW, position, mapElement, ride, get_current_rotation())) {
                imageId = SPR_FENCE_ROPE_SW | gTrackColours[SCHEME_MISC];
                sub_98197C(session, imageId, 0, 0, 1, 28, 7, height, 29, 0, height + 3, get_current_rotation());
            }
            if (track_paint_util_has_fence(EDGE_SE, position, mapElement, ride, get_current_rotation())) {
                imageId = SPR_FENCE_ROPE_SE | gTrackColours[SCHEME_MISC];
                sub_98197C(session, imageId, 0, 0, 28, 1, 7, height, 0, 29, height + 3, get_current_rotation());
            }
            break;
        default:
            track_paint_util_paint_fences(session, edges, position, mapElement, ride, gTrackColours[SCHEME_MISC], height, fenceSpritesRope, get_current_rotation());
            break;
    }

    switch (trackSequence) {
        case 1:
            paint_twist_structure(session, ride, direction, 32, 32, height);
            break;
        case 3:
            paint_twist_structure(session, ride, direction, 32, -32, height);
            break;
        case 5:
            paint_twist_structure(session, ride, direction, 0, -32, height);
            break;
        case 6:
            paint_twist_structure(session, ride, direction, -32, 32, height);
            break;
        case 7:
            paint_twist_structure(session, ride, direction, -32, -32, height);
            break;
        case 8:
            paint_twist_structure(session, ride, direction, -32, 0, height);
            break;
    }

    sint32 cornerSegments = 0;
    switch (trackSequence) {
        case 1: cornerSegments = SEGMENT_B4 | SEGMENT_C8 | SEGMENT_CC; break;
        case 3: cornerSegments = SEGMENT_CC | SEGMENT_BC | SEGMENT_D4; break;
        case 6: cornerSegments = SEGMENT_C8 | SEGMENT_B8 | SEGMENT_D0; break;
        case 7: cornerSegments = SEGMENT_D0 | SEGMENT_C0 | SEGMENT_D4; break;
    }

    paint_util_set_segment_support_height(session, cornerSegments, height + 2, 0x20);
    paint_util_set_segment_support_height(session, SEGMENTS_ALL & ~cornerSegments, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 64, 0x20);
}

/**
 * rct2: 0x0076D658
 */
TRACK_PAINT_FUNCTION get_track_paint_function_twist(sint32 trackType, sint32 direction)
{
    if (trackType != FLAT_TRACK_ELEM_3_X_3) {
        return NULL;
    }

    return paint_twist;
}
