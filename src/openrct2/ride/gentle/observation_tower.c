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
#include "../../paint/paint.h"
#include "../track_paint.h"
#include "../track.h"
#include "../../world/map.h"
#include "../../paint/supports.h"
#include "../vehicle_paint.h"

enum
{
    SPR_OBSERVATION_TOWER_SEGMENT_BASE = 14986,
    SPR_OBSERVATION_TOWER_SEGMENT = 14987,
    SPR_OBSERVATION_TOWER_SEGMENT_TOP = 14988,
};

/**
 *
 *  rct2: 0x006D6258
 */
void vehicle_visual_observation_tower(paint_session * session, sint32 x, sint32 imageDirection, sint32 y, sint32 z, rct_vehicle *vehicle, const rct_ride_entry_vehicle *vehicleEntry)
{
    sint32 image_id;
    sint32 baseImage_id = (vehicle->restraints_position / 64);
    if (vehicle->restraints_position >= 64) {
        if ((imageDirection / 8) && (imageDirection / 8) != 3) {
            baseImage_id *= 2;
            baseImage_id += vehicleEntry->base_image_id + 28;
            if ((imageDirection / 8) != 1) {
                baseImage_id -= 6;
            }
        } else {
            baseImage_id = vehicleEntry->base_image_id + 8;
        }
    } else {
        baseImage_id = (vehicle->var_C5 * 2) + vehicleEntry->base_image_id + 8;
    }

    image_id = baseImage_id | (vehicle->colours.body_colour << 19) | (vehicle->colours.trim_colour << 24) | IMAGE_TYPE_REMAP_2_PLUS;
    paint_struct* ps = sub_98197C(session, image_id, 0, 0, 2, 2, 41, z, -11, -11, z + 1, get_current_rotation());
    if (ps != NULL) {
        ps->tertiary_colour = vehicle->colours_extended;
    }

    image_id++;

    ps = sub_98197C(session, image_id, 0, 0, 16, 16, 41, z, -5, -5, z + 1, get_current_rotation());
    if (ps != NULL) {
        ps->tertiary_colour = vehicle->colours_extended;
    }

    assert(vehicleEntry->effect_visual == 1);
}

/** rct2: 0x0070DD6C */
static void paint_observation_tower_base(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    trackSequence = track_map_3x3[direction][trackSequence];

    sint32 edges = edges_3x3[trackSequence];
    rct_ride * ride = get_ride(rideIndex);
    rct_xy16 position = session->MapPosition;

    wooden_a_supports_paint_setup(session, (direction & 1), 0, height, gTrackColours[SCHEME_MISC], NULL);

    uint32 imageId = SPR_FLOOR_METAL_B | gTrackColours[SCHEME_SUPPORTS];
    sub_98197C(session, imageId, 0, 0, 32, 32, 1, height, 0, 0, height, get_current_rotation());

    track_paint_util_paint_fences(session, edges, position, mapElement, ride, gTrackColours[SCHEME_TRACK], height, fenceSpritesMetalB, get_current_rotation());

    if (trackSequence == 0) {
        imageId = SPR_OBSERVATION_TOWER_SEGMENT_BASE | gTrackColours[SCHEME_TRACK];
        sub_98197C(session, imageId, 0, 0, 2, 2, 27, height, 8, 8, height + 3, get_current_rotation());

        imageId = SPR_OBSERVATION_TOWER_SEGMENT | gTrackColours[SCHEME_TRACK];
        sub_98197C(session, imageId, 0, 0, 2, 2, 30, height + 32, 8, 8, height + 32, get_current_rotation());

        imageId = SPR_OBSERVATION_TOWER_SEGMENT | gTrackColours[SCHEME_TRACK];
        sub_98197C(session, imageId, 0, 0, 2, 2, 30, height + 64, 8, 8, height + 64, get_current_rotation());

        paint_util_set_vertical_tunnel(session, height + 96);
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);

#ifdef __TESTPAINT__
        paint_util_set_general_support_height(session, height + 32, 0x20);
#else
        paint_util_set_general_support_height(session, height + 96, 0x20);
#endif
        return;
    }

    sint32 blockedSegments = 0;
    switch (trackSequence) {
        case 0: blockedSegments = SEGMENTS_ALL; break;
        case 1: blockedSegments = SEGMENT_B8 | SEGMENT_C8 | SEGMENT_B4 | SEGMENT_CC | SEGMENT_BC; break;
        case 2: blockedSegments = SEGMENT_B4 | SEGMENT_CC | SEGMENT_BC; break;
        case 3: blockedSegments = SEGMENT_B4 | SEGMENT_CC | SEGMENT_BC | SEGMENT_D4 | SEGMENT_C0; break;
        case 4: blockedSegments = SEGMENT_B4 | SEGMENT_C8 | SEGMENT_B8; break;
        case 5: blockedSegments = SEGMENT_BC | SEGMENT_D4 | SEGMENT_C0; break;
        case 6: blockedSegments = SEGMENT_B4 | SEGMENT_C8 | SEGMENT_B8 | SEGMENT_D0 | SEGMENT_C0; break;
        case 7: blockedSegments = SEGMENT_B8 | SEGMENT_D0 | SEGMENT_C0 | SEGMENT_D4 | SEGMENT_BC; break;
        case 8: blockedSegments = SEGMENT_B8 | SEGMENT_D0 | SEGMENT_C0; break;
    }
    paint_util_set_segment_support_height(session, blockedSegments, 0xFFFF, 0);
    paint_util_set_segment_support_height(session, SEGMENTS_ALL & ~blockedSegments, height + 2, 0x20);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x0070DD7C */
static void paint_observation_tower_section(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    if (trackSequence == 1) {
        return;
    }

    uint32 imageId = SPR_OBSERVATION_TOWER_SEGMENT | gTrackColours[SCHEME_TRACK];
    sub_98197C(session, imageId, 0, 0, 2, 2, 30, height, 8, 8, height, get_current_rotation());

    rct_map_element * nextMapElement = mapElement + 1;
    if (map_element_is_last_for_tile(mapElement) || mapElement->clearance_height != nextMapElement->base_height) {
        imageId = SPR_OBSERVATION_TOWER_SEGMENT_TOP | gTrackColours[SCHEME_TRACK];
        sub_98199C(session, imageId, 0, 0, 2, 2, 30, height, 8, 8, height, get_current_rotation());
    }

    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);

    paint_util_set_vertical_tunnel(session, height + 32);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/**
 * rct2: 0x0070DC5C
 */
TRACK_PAINT_FUNCTION get_track_paint_function_observation_tower(sint32 trackType, sint32 direction)
{
    switch (trackType) {
        case TRACK_ELEM_TOWER_BASE:
            return paint_observation_tower_base;

        case TRACK_ELEM_TOWER_SECTION:
            return paint_observation_tower_section;
    }

    return NULL;
}
