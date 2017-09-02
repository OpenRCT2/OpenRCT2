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

#include "../../interface/viewport.h"
#include "../../paint/paint.h"
#include "../../paint/supports.h"
#include "../track_paint.h"
#include "../track.h"

// 1 2 0 3 4
static const uint8 track_map_1x5[][5] = {
    {0, 1, 2, 3, 4},
    {0, 4, 3, 2, 1},
    {0, 4, 3, 2, 1},
    {0, 1, 2, 3, 4},
};

typedef struct pirate_ship_bound_box
{
    sint16 length_x;
    sint16 length_y;
    sint16 offset_x;
    sint16 offset_y;
} pirate_ship_bound_box;

/** rct2: 0x008A83B0 */
static const uint32 pirate_ship_base_sprite_offset[] = {
    0,
    9,
    0,
    9
};

/** rct2: 0x008A83C0 */
static const pirate_ship_bound_box pirate_ship_data[] = {
    {31, 16, 1, 8},
    {16, 31, 8, 1},
    {31, 16, 1, 8},
    {16, 31, 8, 1},
};

enum
{
    SPR_PIRATE_SHIP_FRAME_SW_NE = 21994,
    SPR_PIRATE_SHIP_FRAME_FRONT_SW_NE = 21995,
    SPR_PIRATE_SHIP_FRAME_NW_SE = 21996,
    SPR_PIRATE_SHIP_FRAME_FRONT_NW_SE = 21997,
};

static const uint32 pirate_ship_frame_sprites[][2] = {
    {SPR_PIRATE_SHIP_FRAME_SW_NE, SPR_PIRATE_SHIP_FRAME_FRONT_SW_NE},
    {SPR_PIRATE_SHIP_FRAME_NW_SE, SPR_PIRATE_SHIP_FRAME_FRONT_NW_SE},
};

/** rct2: 0x4AF254 */
static void paint_pirate_ship_structure(paint_session * session, rct_ride * ride, uint8 direction, sint8 axisOffset, uint16 height)
{
    uint32 imageId, baseImageId;

    rct_map_element * savedMapElement = session->CurrentlyDrawnItem;

    rct_ride_entry * rideType = get_ride_entry(ride->subtype);
    rct_vehicle * vehicle = NULL;

    sint8 xOffset = !(direction & 1) ? axisOffset : 0;
    sint8 yOffset = (direction & 1) ? axisOffset : 0;

    height += 7;

    if (ride->lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK
        && ride->vehicles[0] != SPRITE_INDEX_NULL) {
        vehicle = GET_VEHICLE(ride->vehicles[0]);

        session->InteractionType = VIEWPORT_INTERACTION_ITEM_SPRITE;
        session->CurrentlyDrawnItem = vehicle;
    }

    baseImageId = rideType->vehicles[0].base_image_id + pirate_ship_base_sprite_offset[direction];
    if (vehicle != NULL) {
        sint32 rotation = (sint8) vehicle->vehicle_sprite_type;
        if (rotation != 0) {
            if (direction & 2) {
                rotation = -rotation;
            }
            if (rotation < 0) {
                rotation = -rotation + 9;
            }

            baseImageId += rotation * 18;
        }
    }

    uint32 imageColourFlags = gTrackColours[SCHEME_MISC];
    if (imageColourFlags == IMAGE_TYPE_REMAP) {
        imageColourFlags = SPRITE_ID_PALETTE_COLOUR_2(ride->vehicle_colours[0].body_colour, ride->vehicle_colours[0].trim_colour);
    }

    pirate_ship_bound_box bounds = pirate_ship_data[direction];

    imageId = pirate_ship_frame_sprites[(direction & 1)][0] | gTrackColours[SCHEME_TRACK];
    sub_98197C(imageId, xOffset, yOffset, bounds.length_x, bounds.length_y, 80, height, bounds.offset_x, bounds.offset_y, height, get_current_rotation());

    imageId = baseImageId | imageColourFlags;
    sub_98199C(imageId, xOffset, yOffset, bounds.length_x, bounds.length_y, 80, height, bounds.offset_x, bounds.offset_y, height, get_current_rotation());

    rct_drawpixelinfo * dpi = session->Unk140E9A8;

    if (dpi->zoom_level <= 1
        && ride->lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK
        && vehicle != NULL) {
        sint32 peep = 0;
        sint32 offset = 1;
        while (peep < 16) {
            if (vehicle->num_peeps <= peep) {
                break;
            }

            sint32 frameNum = offset + (direction >> 1);
            imageColourFlags = SPRITE_ID_PALETTE_COLOUR_2(vehicle->peep_tshirt_colours[peep], vehicle->peep_tshirt_colours[peep + 1]);
            imageId = (baseImageId + frameNum) | imageColourFlags;
            sub_98199C(imageId, xOffset, yOffset, bounds.length_x, bounds.length_y, 80, height, bounds.offset_x, bounds.offset_y, height, get_current_rotation());

            peep += 2;

            if (vehicle->num_peeps <= peep) {
                break;
            }

            frameNum = offset + ((direction >> 1) ^ 1);
            imageColourFlags = SPRITE_ID_PALETTE_COLOUR_2(vehicle->peep_tshirt_colours[peep], vehicle->peep_tshirt_colours[peep + 1]);
            imageId = (baseImageId + frameNum) | imageColourFlags;
            sub_98199C(imageId, xOffset, yOffset, bounds.length_x, bounds.length_y, 80, height, bounds.offset_x, bounds.offset_y, height, get_current_rotation());

            peep += 2;
            offset += 2;
        }
    }

    imageId = pirate_ship_frame_sprites[(direction & 1)][1] | gTrackColours[SCHEME_TRACK];
    sub_98199C(imageId, xOffset, yOffset, bounds.length_x, bounds.length_y, 80, height, bounds.offset_x, bounds.offset_y, height, get_current_rotation());

    session->CurrentlyDrawnItem = savedMapElement;
    session->InteractionType = VIEWPORT_INTERACTION_ITEM_RIDE;
}

/** rct2: 0x008A85C4 */
static void paint_pirate_ship(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    uint8 relativeTrackSequence = track_map_1x5[direction][trackSequence];
    rct_ride * ride = get_ride(rideIndex);
    rct_xy16 position = session->MapPosition;

    uint32 imageId;
    bool hasFence;

    if (relativeTrackSequence == 1 || relativeTrackSequence == 4) {
        wooden_a_supports_paint_setup(direction & 1, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
    } else if (direction & 1) {
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 6, 0, height, gTrackColours[SCHEME_SUPPORTS]);
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 7, 0, height, gTrackColours[SCHEME_SUPPORTS]);

        imageId = SPR_STATION_BASE_A_NW_SE | gTrackColours[SCHEME_SUPPORTS];
        sub_98196C(imageId, 0, 0, 32, 32, 1, height, get_current_rotation());
    } else {
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 5, 0, height, gTrackColours[SCHEME_SUPPORTS]);
        metal_a_supports_paint_setup(METAL_SUPPORTS_TUBES, 8, 0, height, gTrackColours[SCHEME_SUPPORTS]);

        imageId = SPR_STATION_BASE_A_SW_NE | gTrackColours[SCHEME_SUPPORTS];
        sub_98196C(imageId, 0, 0, 32, 32, 1, height, get_current_rotation());
    }

    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);

    if (direction & 1) {
        if (relativeTrackSequence != 1 && relativeTrackSequence != 4) {
            hasFence = track_paint_util_has_fence(EDGE_NE, position, mapElement, ride, get_current_rotation());
            if (relativeTrackSequence == 2) {
                imageId = (hasFence ? SPR_STATION_PLATFORM_BEGIN_FENCED_NW_SE : SPR_STATION_PLATFORM_BEGIN_NW_SE) | gTrackColours[SCHEME_TRACK];
            } else {
                imageId = (hasFence ? SPR_STATION_PLATFORM_FENCED_NW_SE : SPR_STATION_PLATFORM_NW_SE) | gTrackColours[SCHEME_TRACK];
            }
            sub_98199C(imageId, 0, 0, 8, 32, 1, height + 9, 0, -2, height + 9, get_current_rotation());

            imageId = (relativeTrackSequence == 2 ? SPR_STATION_PLATFORM_BEGIN_NW_SE : SPR_STATION_PLATFORM_NW_SE) | gTrackColours[SCHEME_TRACK];
            sub_98196C(imageId, 24, 0, 8, 32, 1, height + 9, get_current_rotation());

            hasFence = track_paint_util_has_fence(EDGE_SW, position, mapElement, ride, get_current_rotation());
            if (relativeTrackSequence == 3) {
                if (hasFence) {
                    imageId = SPR_STATION_BEGIN_ANGLE_FENCE_NW_SE | gTrackColours[SCHEME_TRACK];
                    sub_98196C(imageId, 31, 0, 1, 32, 7, height + 11, get_current_rotation());
                } else {
                    imageId = SPR_STATION_FENCE_SMALL_SW_NE | gTrackColours[SCHEME_TRACK];
                    sub_98196C(imageId, 23, 31, 8, 1, 7, height + 11, get_current_rotation());
                }

                imageId = SPR_STATION_FENCE_SMALL_SW_NE | gTrackColours[SCHEME_TRACK];
                sub_98196C(imageId, 0, 31, 8, 1, 7, height + 11, get_current_rotation());
            } else if (hasFence) {
                imageId = SPR_STATION_FENCE_NW_SE | gTrackColours[SCHEME_TRACK];
                sub_98196C(imageId, 31, 0, 1, 32, 7, height + 11, get_current_rotation());
            }
        }
    } else {
        if (relativeTrackSequence != 1 && relativeTrackSequence != 4) {
            hasFence = track_paint_util_has_fence(EDGE_NW, position, mapElement, ride, get_current_rotation());
            if (relativeTrackSequence == 2) {
                imageId = (hasFence ? SPR_STATION_PLATFORM_BEGIN_FENCED_SW_NE : SPR_STATION_PLATFORM_BEGIN_SW_NE) | gTrackColours[SCHEME_TRACK];
            } else {
                imageId = (hasFence ? SPR_STATION_PLATFORM_FENCED_SW_NE : SPR_STATION_PLATFORM_SW_NE) | gTrackColours[SCHEME_TRACK];
            }
            sub_98199C(imageId, 0, 0, 32, 8, 1, height + 9, -2, 0, height + 9, get_current_rotation());

            imageId = (relativeTrackSequence == 2 ? SPR_STATION_PLATFORM_BEGIN_SW_NE : SPR_STATION_PLATFORM_SW_NE) | gTrackColours[SCHEME_TRACK];
            sub_98196C(imageId, 0, 24, 32, 8, 1, height + 9, get_current_rotation());

            hasFence = track_paint_util_has_fence(EDGE_SE, position, mapElement, ride, get_current_rotation());
            if (relativeTrackSequence == 3) {
                if (hasFence) {
                    imageId = SPR_STATION_BEGIN_ANGLE_FENCE_SW_NE | gTrackColours[SCHEME_TRACK];
                    sub_98196C(imageId, 0, 31, 32, 1, 7, height + 11, get_current_rotation());
                } else {
                    imageId = SPR_STATION_FENCE_SMALL_NW_SE | gTrackColours[SCHEME_TRACK];
                    sub_98196C(imageId, 31, 23, 1, 8, 7, height + 11, get_current_rotation());
                }

                imageId = SPR_STATION_FENCE_SMALL_NW_SE | gTrackColours[SCHEME_TRACK];
                sub_98196C(imageId, 31, 0, 1, 8, 7, height + 11, get_current_rotation());
            } else if (hasFence) {
                imageId = SPR_STATION_FENCE_SW_NE | gTrackColours[SCHEME_TRACK];
                sub_98196C(imageId, 0, 31, 32, 1, 7, height + 11, get_current_rotation());
            }
        }
    }

    switch (relativeTrackSequence) {
        case 1: paint_pirate_ship_structure(session, ride, direction, 64, height); break;
        case 2: paint_pirate_ship_structure(session, ride, direction, 32, height); break;
        case 0: paint_pirate_ship_structure(session, ride, direction, 0, height); break;
        case 3: paint_pirate_ship_structure(session, ride, direction, -32, height); break;
        case 4: paint_pirate_ship_structure(session, ride, direction, -64, height); break;
    }

    paint_util_set_general_support_height(height + 112, 0x20);
}

/**
 * rct2: 0x008A83E0
 */
TRACK_PAINT_FUNCTION get_track_paint_function_pirate_ship(sint32 trackType, sint32 direction)
{
    if (trackType != FLAT_TRACK_ELEM_1_X_5) {
        return NULL;
    }

    return paint_pirate_ship;
}
