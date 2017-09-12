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

#include "../../paint/supports.h"
#include "../../interface/viewport.h"
#include "../../paint/paint.h"
#include "../../localisation/localisation.h"
#include "../../sprites.h"
#include "../../world/map.h"
#include "../../world/sprite.h"
#include "../ride_data.h"
#include "../track_data.h"
#include "../track_paint.h"

/** rct2: 0x014280BC */
static sint16 TopSpinSeatHeightOffset[] = {
    -10, -10,  -9,  -7,  -4,  -1,   2,   6,  11,  16,  21,  26,  31,  37,  42,  47,
     52,  57,  61,  64,  67,  70,  72,  73,  73,  73,  72,  70,  67,  64,  61,  57,
     52,  47,  42,  37,  31,  26,  21,  16,  11,   6,   2,  -1,  -4,  -7,  -9, -10,
};

/**
 *
 *  rct2: 0x0142811C
 * Can be calculated as Rounddown(34*sin(x)+0.5)
 * where x is in 7.5 deg segments.
 */
static sint8 TopSpinSeatPositionOffset[] = {
      0,   4,   9,  13,  17,  21,  24,  27,  29,  31,  33,  34,  34,  34,  33,  31,
     29,  27,  24,  21,  17,  13,   9,   4,   0,  -3,  -8, -12, -16, -20, -23, -26,
    -28, -30, -32, -33, -33, -33, -32, -30, -28, -26, -23, -20, -16, -12,  -8,  -3,
};

/**
 *
 *  rct2: 0x0076750D
 */
static void top_spin_paint_vehicle(paint_session * session, sint8 al, sint8 cl, uint8 rideIndex, uint8 direction, sint32 height, rct_map_element* mapElement) {
    uint16 boundBoxOffsetX, boundBoxOffsetY, boundBoxOffsetZ;
    // As we will be drawing a vehicle we need to backup the mapElement that
    // is assigned to the drawings.
    rct_map_element* curMapElement = session->CurrentlyDrawnItem;

    height += 3;

    Ride* ride = get_ride(rideIndex);
    rct_ride_entry* rideEntry = get_ride_entry(ride->subtype);
    rct_vehicle* vehicle = NULL;

    uint8 seatRotation = 0;
    sint8 armRotation = 0;

    if (ride->lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK &&
        ride->vehicles[0] != SPRITE_INDEX_NULL) {
        vehicle = GET_VEHICLE(ride->vehicles[0]);

        session->InteractionType = VIEWPORT_INTERACTION_ITEM_SPRITE;
        session->CurrentlyDrawnItem = vehicle;

        armRotation = vehicle->vehicle_sprite_type;
        seatRotation = vehicle->bank_rotation;
    }

    boundBoxOffsetX = al + 16;
    boundBoxOffsetY = cl + 16;
    boundBoxOffsetZ = height;

    //di
    uint8 lengthX = 24;
    //si
    uint8 lengthY = 24;

    uint32 image_id = session->TrackColours[SCHEME_MISC];
    if (image_id == IMAGE_TYPE_REMAP) {
        image_id = SPRITE_ID_PALETTE_COLOUR_2(ride->track_colour_main[0], ride->track_colour_supports[0]);
    }

    const uint8 rotation = get_current_rotation();
    image_id += (direction & 1) << 1;
    image_id += rideEntry->vehicles[0].base_image_id;
    // Left back bottom support
    image_id += 572;
    sub_98197C(session, image_id, al, cl, lengthX, lengthY, 90, height, boundBoxOffsetX, boundBoxOffsetY, boundBoxOffsetZ, rotation);

    image_id = session->TrackColours[SCHEME_MISC];
    if (image_id == IMAGE_TYPE_REMAP) {
        image_id = SPRITE_ID_PALETTE_COLOUR_2(ride->track_colour_main[0], ride->track_colour_additional[0]);
    }

    sint32 var_1F = armRotation;
    if (direction & 2) {
        var_1F = -var_1F;
        if (var_1F != 0)
            var_1F += 48;
    }
    image_id += var_1F;
    image_id += (direction & 1) * 48;
    image_id += rideEntry->vehicles[0].base_image_id;
    // Left hand arm
    image_id += 380;

    sub_98199C(session, image_id, al, cl, lengthX, lengthY, 90, height, boundBoxOffsetX, boundBoxOffsetY, boundBoxOffsetZ, rotation);

    uint32 seatImageId;

    if (vehicle != NULL && vehicle->restraints_position >= 64) {
        // Open Restraints
        image_id = (vehicle->restraints_position - 64) >> 6;
        image_id += direction * 3;
        image_id += rideEntry->vehicles[0].base_image_id;
        image_id += 64;
        seatImageId = image_id;
    }
    else {
        image_id = direction * 16;
        // Var_20 Rotation of seats
        image_id += seatRotation;
        image_id += rideEntry->vehicles[0].base_image_id;
        seatImageId = image_id;
    }

    image_id = session->TrackColours[SCHEME_MISC];
    if (image_id == IMAGE_TYPE_REMAP) {
        image_id = SPRITE_ID_PALETTE_COLOUR_2(ride->vehicle_colours[0].body_colour, ride->vehicle_colours[0].trim_colour);
    }
    image_id += seatImageId;

    rct_xyz16 seatCoords = {
        .x = al,
        .y = cl,
        .z = height
    };

    seatCoords.z += TopSpinSeatHeightOffset[armRotation];

    assert(armRotation < sizeof(TopSpinSeatPositionOffset));
    switch (direction) {
    case 0:
        seatCoords.x -= TopSpinSeatPositionOffset[armRotation];
        break;
    case 1:
        seatCoords.y += TopSpinSeatPositionOffset[armRotation];
        break;
    case 2:
        seatCoords.x += TopSpinSeatPositionOffset[armRotation];
        break;
    case 3:
        seatCoords.y -= TopSpinSeatPositionOffset[armRotation];
        break;
    }

    sub_98199C(session, image_id, (sint8) seatCoords.x, (sint8) seatCoords.y, lengthX, lengthY, 90, seatCoords.z, boundBoxOffsetX, boundBoxOffsetY, boundBoxOffsetZ, rotation);

    rct_drawpixelinfo* dpi = session->Unk140E9A8;
    if (dpi->zoom_level < 2 && vehicle != NULL && vehicle->num_peeps != 0)
    {
        image_id = (seatImageId + (1 * 76)) | SPRITE_ID_PALETTE_COLOUR_2(vehicle->peep_tshirt_colours[0], vehicle->peep_tshirt_colours[1]);

        sub_98199C(session, image_id, (sint8) seatCoords.x, (sint8) seatCoords.y, lengthX, lengthY, 90, seatCoords.z, boundBoxOffsetX, boundBoxOffsetY, boundBoxOffsetZ, rotation);

        if (vehicle->num_peeps > 2)
        {
            image_id = (seatImageId + (2 * 76)) | SPRITE_ID_PALETTE_COLOUR_2(vehicle->peep_tshirt_colours[2], vehicle->peep_tshirt_colours[3]);

            sub_98199C(session, image_id, (sint8) seatCoords.x, (sint8) seatCoords.y, lengthX, lengthY, 90, seatCoords.z, boundBoxOffsetX, boundBoxOffsetY, boundBoxOffsetZ, rotation);
        }

        if (vehicle->num_peeps > 4)
        {
            image_id = (seatImageId + (3 * 76)) | SPRITE_ID_PALETTE_COLOUR_2(vehicle->peep_tshirt_colours[4], vehicle->peep_tshirt_colours[5]);

            sub_98199C(session, image_id, (sint8) seatCoords.x, (sint8) seatCoords.y, lengthX, lengthY, 90, seatCoords.z, boundBoxOffsetX, boundBoxOffsetY, boundBoxOffsetZ, rotation);
        }

        if (vehicle->num_peeps > 6)
        {
            image_id = (seatImageId + (4 * 76)) | SPRITE_ID_PALETTE_COLOUR_2(vehicle->peep_tshirt_colours[6], vehicle->peep_tshirt_colours[7]);

            sub_98199C(session, image_id, (sint8) seatCoords.x, (sint8) seatCoords.y, lengthX, lengthY, 90, seatCoords.z, boundBoxOffsetX, boundBoxOffsetY, boundBoxOffsetZ, rotation);
        }
    }

    image_id = session->TrackColours[SCHEME_MISC];
    if (image_id == IMAGE_TYPE_REMAP) {
        image_id = SPRITE_ID_PALETTE_COLOUR_2(ride->track_colour_main[0], ride->track_colour_additional[0]);
    }

    image_id += var_1F;
    image_id += (direction & 1) * 48;
    image_id += rideEntry->vehicles[0].base_image_id;
    // Right hand arm
    image_id += 476;

    sub_98199C(session, image_id, al, cl, lengthX, lengthY, 90, height, boundBoxOffsetX, boundBoxOffsetY, boundBoxOffsetZ, rotation);

    image_id = session->TrackColours[SCHEME_MISC];
    if (image_id == IMAGE_TYPE_REMAP)
    {
        image_id = SPRITE_ID_PALETTE_COLOUR_2(ride->track_colour_main[0], ride->track_colour_supports[0]);
    }

    image_id += (direction & 1) << 1;
    image_id += rideEntry->vehicles[0].base_image_id;
    // Right back bottom support
    image_id += 573;

    sub_98199C(session, image_id, al, cl, lengthX, lengthY, 90, height, boundBoxOffsetX, boundBoxOffsetY, boundBoxOffsetZ, rotation);

    session->CurrentlyDrawnItem = curMapElement;
    session->InteractionType = VIEWPORT_INTERACTION_ITEM_RIDE;
}

/**
 * rct2: 0x0076679C
 */
static void paint_top_spin(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement) {
    trackSequence = track_map_3x3[direction][trackSequence];

    sint32 edges = edges_3x3[trackSequence];
    Ride *ride = get_ride(rideIndex);
    rct_xy16 position = session->MapPosition;

    wooden_a_supports_paint_setup(session, direction & 1, 0, height, session->TrackColours[SCHEME_MISC], NULL);

    track_paint_util_paint_floor(session, edges, session->TrackColours[SCHEME_TRACK], height, floorSpritesCork, get_current_rotation());

    track_paint_util_paint_fences(session, edges, position, mapElement, ride, session->TrackColours[SCHEME_MISC], height, fenceSpritesRope, get_current_rotation());

    switch (trackSequence) {
        case 1: top_spin_paint_vehicle(session, 32, 32, rideIndex, direction, height, mapElement); break;
        case 3: top_spin_paint_vehicle(session, 32, -32, rideIndex, direction, height, mapElement); break;
        case 5: top_spin_paint_vehicle(session, 0, -32, rideIndex, direction, height, mapElement); break;
        case 6: top_spin_paint_vehicle(session, -32, 32, rideIndex, direction, height, mapElement); break;
        case 7: top_spin_paint_vehicle(session, -32, -32, rideIndex, direction, height, mapElement); break;
        case 8: top_spin_paint_vehicle(session, -32, 0, rideIndex, direction, height, mapElement); break;
    }

    sint32 cornerSegments = 0;
    switch (trackSequence) {
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
    paint_util_set_general_support_height(session, height + 112, 0x20);
}


/* 0x0076659C */
TRACK_PAINT_FUNCTION get_track_paint_function_topspin(sint32 trackType, sint32 direction)
{
    if (trackType != FLAT_TRACK_ELEM_3_X_3) {
        return NULL;
    }

    return paint_top_spin;
}
