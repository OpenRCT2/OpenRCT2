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

/** rct2: 0x008A2ABC */
static void paint_enterprise_structure(paint_session * session, rct_ride * ride, sint8 xOffset, sint8 yOffset, uint16 height, rct_map_element * mapElement)
{
    height += 7;

    rct_map_element * savedMapElement = session->CurrentlyDrawnItem;
    rct_ride_entry * rideEntry = get_ride_entry(ride->subtype);
    rct_vehicle * vehicle = NULL;

    uint32 baseImageId = rideEntry->vehicles[0].base_image_id;

    if (ride->lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK
        && ride->vehicles[0] != SPRITE_INDEX_NULL) {
        session->InteractionType = VIEWPORT_INTERACTION_ITEM_SPRITE;
        vehicle = GET_VEHICLE(ride->vehicles[0]);
        session->CurrentlyDrawnItem = vehicle;
    }

    uint32 imageOffset = map_element_get_direction_with_offset(mapElement, get_current_rotation());
    if (vehicle != NULL) {
        imageOffset = (vehicle->vehicle_sprite_type << 2) + (((vehicle->sprite_direction >> 3) + get_current_rotation()) % 4);
    }

    uint32 imageColourFlags = gTrackColours[SCHEME_MISC];
    if (imageColourFlags == IMAGE_TYPE_REMAP) {
        imageColourFlags = SPRITE_ID_PALETTE_COLOUR_2(ride->vehicle_colours[0].body_colour, ride->vehicle_colours[0].trim_colour);
    }

    uint32 imageId = (baseImageId + imageOffset) | imageColourFlags;
    sub_98197C(imageId, xOffset, yOffset, 24, 24, 48, height, 0, 0, height, get_current_rotation());

    rct_drawpixelinfo * dpi = session->Unk140E9A8;

    if (dpi->zoom_level == 0
        && imageOffset < 12
        && ride->lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK
        && vehicle != NULL) {
        for (sint32 i = 0; i < 15; i++) {
            if (vehicle->num_peeps <= i) {
                break;
            }

            uint32 peepFrameOffset = ((imageOffset % 4) * 4 + (i * 4) % 15) & 0x0F;
            uint32 ax = (imageOffset & 0xFFFFFFFC) << 2;
            imageId = (baseImageId + 196 + peepFrameOffset + ax) | SPRITE_ID_PALETTE_COLOUR_1(vehicle->peep_tshirt_colours[i]);
            sub_98199C(imageId, xOffset, yOffset, 24, 24, 48, height, 0, 0, height, get_current_rotation());
        }
    }

    session->CurrentlyDrawnItem = savedMapElement;
    session->InteractionType = VIEWPORT_INTERACTION_ITEM_RIDE;
}

/** rct2: 0x008A1584 */
static void paint_enterprise(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    trackSequence = track_map_4x4[direction][trackSequence];

    sint32 edges = edges_4x4[trackSequence];
    rct_ride * ride = get_ride(rideIndex);
    rct_xy16 position = session->MapPosition;

    wooden_a_supports_paint_setup(session, direction & 1, 0, height, gTrackColours[SCHEME_MISC], NULL);

    track_paint_util_paint_floor(edges, gTrackColours[SCHEME_TRACK], height, floorSpritesCork, get_current_rotation());

    track_paint_util_paint_fences(edges, position, mapElement, ride, gTrackColours[SCHEME_TRACK], height, fenceSpritesRope, get_current_rotation());

    switch (trackSequence) {
        case 5: paint_enterprise_structure(session, ride, 16, 16, height, mapElement); break;
        case 6: paint_enterprise_structure(session, ride, 16, -16, height, mapElement); break;
        case 10: paint_enterprise_structure(session, ride, -16, -16, height, mapElement); break;
        case 9: paint_enterprise_structure(session, ride, -16, 16, height, mapElement); break;

        case 0: paint_enterprise_structure(session, ride, 48, 48, height, mapElement); break;
        case 3: paint_enterprise_structure(session, ride, 48, -48, height, mapElement); break;
        case 15: paint_enterprise_structure(session, ride, -48, -48, height, mapElement); break;
        case 12: paint_enterprise_structure(session, ride, -48, 48, height, mapElement); break;

        case 7: paint_enterprise_structure(session, ride, 16, -48, height, mapElement); break;
        case 11: paint_enterprise_structure(session, ride, -16, -48, height, mapElement); break;
        case 14: paint_enterprise_structure(session, ride, -48, -16, height, mapElement); break;
        case 13: paint_enterprise_structure(session, ride, -48, 16, height, mapElement); break;
    }

    sint32 cornerSegments = 0;
    switch (trackSequence) {
        case 0: cornerSegments = SEGMENT_B4 | SEGMENT_C8 | SEGMENT_CC; break;
        case 3: cornerSegments = SEGMENT_CC | SEGMENT_BC | SEGMENT_D4; break;
        case 12: cornerSegments = SEGMENT_C8 | SEGMENT_B8 | SEGMENT_D0; break;
        case 15: cornerSegments = SEGMENT_D0 | SEGMENT_C0 | SEGMENT_D4; break;
    }
    paint_util_set_segment_support_height(cornerSegments, height + 2, 0x20);
    paint_util_set_segment_support_height(SEGMENTS_ALL & ~cornerSegments, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 160, 0x20);

}

/**
 * rct2: 0x008A13B4
 */
TRACK_PAINT_FUNCTION get_track_paint_function_enterprise(sint32 trackType, sint32 direction)
{
    if (trackType != FLAT_TRACK_ELEM_4_X_4) {
        return NULL;
    }

    return paint_enterprise;
}
