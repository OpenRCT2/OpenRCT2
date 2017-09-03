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
#include "../../sprites.h"
#include "../../world/map.h"
#include "../track_paint.h"
#include "../track.h"

/**
 *
 *  rct2: 0x00763234
 *  rct2: 0x0076338C
 *  rct2: 0x00762F50
 *  rct2: 0x007630DE
 */
static void facility_paint_setup(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bool hasSupports = wooden_a_supports_paint_setup(session, direction & 1, 0, height, gTrackColours[SCHEME_3], NULL);

    rct_ride *ride = get_ride(rideIndex);
    rct_ride_entry *rideEntry = get_ride_entry(ride->subtype);
    rct_ride_entry_vehicle *firstVehicleEntry = &rideEntry->vehicles[0];

    if (rideEntry == NULL || firstVehicleEntry == NULL)
    {
        log_error("Error drawing facility, rideEntry or firstVehicleEntry is NULL.");
        return;
    }

    uint32 imageId = gTrackColours[SCHEME_TRACK];
    imageId |= firstVehicleEntry->base_image_id;
    imageId += (direction + 2) & 3;

    sint32 rotation = get_current_rotation();
    sint32 lengthX = (direction & 1) == 0 ? 28 : 2;
    sint32 lengthY = (direction & 1) == 0 ? 2 : 28;
    if (hasSupports) {
        uint32 foundationImageId = ((direction & 1) ? SPR_FLOOR_PLANKS_90_DEG : SPR_FLOOR_PLANKS) | gTrackColours[SCHEME_3];
        sub_98197C(foundationImageId, 0, 0, lengthX, lengthY, 29, height, direction == 3 ? 28 : 2, direction == 0 ? 28 : 2, height, rotation);

        // Door image or base
        sub_98199C(imageId, 0, 0, lengthX, lengthY, 29, height, direction == 3 ? 28 : 2, direction == 0 ? 28 : 2, height, rotation);
    } else {
        // Door image or base
        sub_98197C(imageId, 0, 0, lengthX, lengthY, 29, height, direction == 3 ? 28 : 2, direction == 0 ? 28 : 2, height, rotation);
    }

    // Base image if door was drawn
    if (direction == 1) {
        imageId += 2;
        sub_98197C(imageId, 0, 0, 2, 28, 29, height, 28, 2, height, rotation);
    } else if (direction == 2) {
        imageId += 4;
        sub_98197C(imageId, 0, 0, 28, 2, 29, height, 2, 28, height, rotation);
    }

    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 32, 0x20);
}

/* 0x00762D44 */
TRACK_PAINT_FUNCTION get_track_paint_function_facility(sint32 trackType, sint32 direction)
{
    switch (trackType) {
    case FLAT_TRACK_ELEM_1_X_1_A: return facility_paint_setup;
    }
    return NULL;
}
