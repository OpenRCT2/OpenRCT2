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

enum {
    PLANE_BACK,
    PLANE_FRONT,
};

enum {
    SPR_MAGIC_CARPET_FRAME_NW       = 22002,
    SPR_MAGIC_CARPET_FRAME_SE       = 22003,
    SPR_MAGIC_CARPET_FRAME_NE       = 22004,
    SPR_MAGIC_CARPET_FRAME_SW       = 22005,
    SPR_MAGIC_CARPET_PENDULUM_NW    = 22006,
    SPR_MAGIC_CARPET_PENDULUM_NE    = 22038,
    SPR_MAGIC_CARPET_PENDULUM_SE    = 22070,
    SPR_MAGIC_CARPET_PENDULUM_SW    = 22102,
};

typedef struct bound_box {
    sint16 x;
    sint16 y;
    sint16 width;
    sint16 height;
} bound_box;

/** rct2: 0x01428220 */
static const sint16 MagicCarpetOscillationZ[] = {
    -2, -1, 1, 5, 10, 16, 23, 30, 37, 45, 52, 59, 65, 70, 74, 76, 77,
    76, 74, 70, 65, 59, 52, 45, 37, 30, 23, 16, 10, 5, 1, -1
};

/** rct2: 0x01428260 */
static const sint8 MagicCarpetOscillationXY[] = {
    0, 6, 12, 18, 23, 27, 30, 31, 32, 31, 30, 27, 23, 18, 12, 6, 0,
    -5, -11, -17, -22, -26, -29, -30, -31, -30, -29, -26, -22, -17, -11, -5
};

/** rct2: 0x014281F0 */
static const bound_box MagicCarpetBounds[] = {
    { 0, 8, 32, 16 },
    { 8, 0, 16, 32 },
    { 0, 8, 32, 16 },
    { 8, 0, 16, 32 }
};

static rct_vehicle *get_first_vehicle(rct_ride *ride)
{
    if (ride->lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK) {
        uint16 vehicleSpriteIndex = ride->vehicles[0];
        if (vehicleSpriteIndex != SPRITE_INDEX_NULL) {
            return GET_VEHICLE(vehicleSpriteIndex);
        }
    }
    return NULL;
}

static void paint_magic_carpet_frame(paint_session * session, uint8 plane, uint8 direction,
                                     rct_xyz16 offset, rct_xyz16 bbOffset, rct_xyz16 bbSize)
{
    uint32 imageId;
    if (direction & 1) {
        imageId = plane == PLANE_BACK ? SPR_MAGIC_CARPET_FRAME_NE :
                                        SPR_MAGIC_CARPET_FRAME_SW;
    } else {
        imageId = plane == PLANE_BACK ? SPR_MAGIC_CARPET_FRAME_NW :
                                        SPR_MAGIC_CARPET_FRAME_SE;
    }
    imageId |= gTrackColours[SCHEME_TRACK];
    if (plane == PLANE_BACK) {
        sub_98197C(imageId, (sint8)offset.x, (sint8)offset.y, bbSize.x, bbSize.y, 127, offset.z, bbOffset.x, bbOffset.y, bbOffset.z, get_current_rotation());
    } else {
        sub_98199C(imageId, (sint8)offset.x, (sint8)offset.y, bbSize.x, bbSize.y, 127, offset.z, bbOffset.x, bbOffset.y, bbOffset.z, get_current_rotation());
    }
}

static void paint_magic_carpet_pendulum(paint_session * session, uint8 plane, uint32 swingImageId, uint8 direction,
                                        rct_xyz16 offset, rct_xyz16 bbOffset, rct_xyz16 bbSize)
{
    uint32 imageId = swingImageId;
    if (direction & 2) {
        imageId = (0 - ((sint32)imageId)) & 31;
    }
    if (direction & 1) {
        imageId += plane == PLANE_BACK ? SPR_MAGIC_CARPET_PENDULUM_NE :
                                         SPR_MAGIC_CARPET_PENDULUM_SW;
    } else {
        imageId += plane == PLANE_BACK ? SPR_MAGIC_CARPET_PENDULUM_NW :
                                         SPR_MAGIC_CARPET_PENDULUM_SE;
    }
    imageId |= gTrackColours[SCHEME_TRACK];
    sub_98199C(imageId, (sint8)offset.x, (sint8)offset.y, bbSize.x, bbSize.y, 127, offset.z, bbOffset.x, bbOffset.y, bbOffset.z, get_current_rotation());
}

static void paint_magic_carpet_vehicle(paint_session * session, rct_ride *ride, uint8 direction, uint32 swingImageId,
                                       rct_xyz16 offset, rct_xyz16 bbOffset, rct_xyz16 bbSize)
{
    rct_ride_entry *rideEntry = get_ride_entry_by_ride(ride);
    uint32 vehicleImageId = rideEntry->vehicles[0].base_image_id + direction;

    // Vehicle
    uint32 imageColourFlags = gTrackColours[SCHEME_MISC];
    if (imageColourFlags == IMAGE_TYPE_REMAP)
    {
        imageColourFlags = SPRITE_ID_PALETTE_COLOUR_2(ride->vehicle_colours[0].body_colour, ride->vehicle_colours[0].trim_colour);
    }

    sint8 directionalOffset = MagicCarpetOscillationXY[swingImageId];
    switch (direction) {
    case 0: offset.x -= directionalOffset; break;
    case 1: offset.y += directionalOffset; break;
    case 2: offset.x += directionalOffset; break;
    case 3: offset.y -= directionalOffset; break;
    }
    offset.z += MagicCarpetOscillationZ[swingImageId];

    sub_98199C(vehicleImageId | imageColourFlags, (sint8)offset.x, (sint8)offset.y, bbSize.x, bbSize.y, 127, offset.z, bbOffset.x, bbOffset.y, bbOffset.z, get_current_rotation());

    // Riders
    rct_drawpixelinfo *dpi = session->Unk140E9A8;
    if (dpi->zoom_level <= 1 && (ride->lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK)) {
        rct_vehicle *vehicle = get_first_vehicle(ride);
        if (vehicle != NULL) {
            uint32 baseImageId = IMAGE_TYPE_REMAP | IMAGE_TYPE_REMAP_2_PLUS | (vehicleImageId + 4);
            for (uint8 peepIndex = 0; peepIndex < vehicle->num_peeps; peepIndex += 2) {
                uint32 imageId = baseImageId + (peepIndex * 2);
                imageId |= (vehicle->peep_tshirt_colours[peepIndex + 0] << 19);
                imageId |= (vehicle->peep_tshirt_colours[peepIndex + 1] << 24);
                sub_98199C(imageId, (sint8)offset.x, (sint8)offset.y, bbSize.x, bbSize.y, 127, offset.z, bbOffset.x, bbOffset.y, bbOffset.z, get_current_rotation());
            }
        }
    }
}

/** rct2: 0x00899104 */
static void paint_magic_carpet_structure(paint_session * session, rct_ride *ride, uint8 direction, sint8 axisOffset, uint16 height)
{
    rct_map_element * savedMapElement = session->CurrentlyDrawnItem;
    rct_vehicle *vehicle = get_first_vehicle(ride);

    uint32 swingImageId = 0;
    if (vehicle != NULL) {
        swingImageId = vehicle->vehicle_sprite_type;
        session->InteractionType = VIEWPORT_INTERACTION_ITEM_SPRITE;
        session->CurrentlyDrawnItem = vehicle;
    }

    bound_box bb = MagicCarpetBounds[direction];
    rct_xyz16 offset, bbOffset, bbSize;
    offset.x = (direction & 1) ? 0 : axisOffset;
    offset.y = (direction & 1) ? axisOffset : 0;
    offset.z = height + 7;
    bbOffset.x = bb.x;
    bbOffset.y = bb.y;
    bbOffset.z = height + 7;
    bbSize.x = bb.width;
    bbSize.y = bb.height;
    bbSize.z = 127;

    paint_magic_carpet_frame(session, PLANE_BACK, direction, offset, bbOffset, bbSize);
    paint_magic_carpet_pendulum(session, PLANE_BACK, swingImageId, direction, offset, bbOffset, bbSize);
    paint_magic_carpet_vehicle(session, ride, direction, swingImageId, offset, bbOffset, bbSize);
    paint_magic_carpet_pendulum(session, PLANE_FRONT, swingImageId, direction, offset, bbOffset, bbSize);
    paint_magic_carpet_frame(session, PLANE_FRONT, direction, offset, bbOffset, bbSize);

    session->InteractionType = VIEWPORT_INTERACTION_ITEM_RIDE;
    session->CurrentlyDrawnItem = savedMapElement;
}

/** rct2: 0x00898514 */
static void paint_magic_carpet(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    uint8 relativeTrackSequence = track_map_1x4[direction][trackSequence];

    // The end tiles do not have a platform
    switch (relativeTrackSequence) {
    case 0:
    case 2:
        if (direction & 1) {
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 6, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 7, 0, height, gTrackColours[SCHEME_SUPPORTS]);
        } else {
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 5, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 8, 0, height, gTrackColours[SCHEME_SUPPORTS]);
        }

        uint32 imageId = SPR_STATION_BASE_D | gTrackColours[SCHEME_SUPPORTS];
        sub_98196C(imageId, 0, 0, 32, 32, 1, height, get_current_rotation());
        break;
    }

    rct_ride *ride = get_ride(rideIndex);
    switch (relativeTrackSequence) {
    case 3: paint_magic_carpet_structure(session, ride, direction, -48, height); break;
    case 0: paint_magic_carpet_structure(session, ride, direction, -16, height); break;
    case 2: paint_magic_carpet_structure(session, ride, direction,  16, height); break;
    case 1: paint_magic_carpet_structure(session, ride, direction,  48, height); break;
    }

    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 176, 0x20);
}

/**
 *
 *  rct2: 0x00898384
 */
TRACK_PAINT_FUNCTION get_track_paint_function_magic_carpet(sint32 trackType, sint32 direction)
{
    switch (trackType) {
    case FLAT_TRACK_ELEM_1_X_4_A: return paint_magic_carpet;
    }
    return NULL;
}
