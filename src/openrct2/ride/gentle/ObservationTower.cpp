/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../common.h"
#include "../../interface/Viewport.h"
#include "../../paint/Paint.h"
#include "../../paint/Supports.h"
#include "../../world/Map.h"
#include "../Track.h"
#include "../TrackPaint.h"
#include "../VehiclePaint.h"

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
void vehicle_visual_observation_tower(
    paint_session* session, int32_t x, int32_t imageDirection, int32_t y, int32_t z, const Vehicle* vehicle,
    const rct_ride_entry_vehicle* vehicleEntry)
{
    int32_t image_id;
    int32_t baseImage_id = (vehicle->restraints_position / 64);
    if (vehicle->restraints_position >= 64)
    {
        if ((imageDirection / 8) && (imageDirection / 8) != 3)
        {
            baseImage_id *= 2;
            baseImage_id += vehicleEntry->base_image_id + 28;
            if ((imageDirection / 8) != 1)
            {
                baseImage_id -= 6;
            }
        }
        else
        {
            baseImage_id = vehicleEntry->base_image_id + 8;
        }
    }
    else
    {
        baseImage_id = (vehicle->animation_frame * 2) + vehicleEntry->base_image_id + 8;
    }

    image_id = baseImage_id | SPRITE_ID_PALETTE_COLOUR_3(vehicle->colours.body_colour, vehicle->colours.trim_colour);
    if (vehicle->IsGhost())
    {
        image_id = (image_id & 0x7FFFF) | CONSTRUCTION_MARKER;
    }
    paint_struct* ps = sub_98197C(session, image_id, 0, 0, 2, 2, 41, z, -11, -11, z + 1);
    if (ps != nullptr)
    {
        ps->tertiary_colour = vehicle->colours_extended;
    }

    image_id++;

    ps = sub_98197C(session, image_id, 0, 0, 16, 16, 41, z, -5, -5, z + 1);
    if (ps != nullptr)
    {
        ps->tertiary_colour = vehicle->colours_extended;
    }

    assert(vehicleEntry->effect_visual == 1);
}

/** rct2: 0x0070DD6C */
static void paint_observation_tower_base(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    trackSequence = track_map_3x3[direction][trackSequence];

    int32_t edges = edges_3x3[trackSequence];
    CoordsXY position = session->MapPosition;

    wooden_a_supports_paint_setup(session, (direction & 1), 0, height, session->TrackColours[SCHEME_MISC], nullptr);

    uint32_t imageId = SPR_FLOOR_METAL_B | session->TrackColours[SCHEME_SUPPORTS];
    sub_98197C(session, imageId, 0, 0, 32, 32, 1, height, 0, 0, height);

    auto ride = get_ride(rideIndex);
    if (ride != nullptr)
    {
        track_paint_util_paint_fences(
            session, edges, position, tileElement, ride, session->TrackColours[SCHEME_TRACK], height, fenceSpritesMetalB,
            session->CurrentRotation);
    }

    if (trackSequence == 0)
    {
        imageId = SPR_OBSERVATION_TOWER_SEGMENT_BASE | session->TrackColours[SCHEME_TRACK];
        sub_98197C(session, imageId, 0, 0, 2, 2, 27, height, 8, 8, height + 3);

        imageId = SPR_OBSERVATION_TOWER_SEGMENT | session->TrackColours[SCHEME_TRACK];
        sub_98197C(session, imageId, 0, 0, 2, 2, 30, height + 32, 8, 8, height + 32);

        imageId = SPR_OBSERVATION_TOWER_SEGMENT | session->TrackColours[SCHEME_TRACK];
        sub_98197C(session, imageId, 0, 0, 2, 2, 30, height + 64, 8, 8, height + 64);

        paint_util_set_vertical_tunnel(session, height + 96);
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);

#ifdef __TESTPAINT__
        paint_util_set_general_support_height(session, height + 32, 0x20);
#else
        paint_util_set_general_support_height(session, height + 96, 0x20);
#endif
        return;
    }

    int32_t blockedSegments = 0;
    switch (trackSequence)
    {
        case 1:
            blockedSegments = SEGMENT_B8 | SEGMENT_C8 | SEGMENT_B4 | SEGMENT_CC | SEGMENT_BC;
            break;
        case 2:
            blockedSegments = SEGMENT_B4 | SEGMENT_CC | SEGMENT_BC;
            break;
        case 3:
            blockedSegments = SEGMENT_B4 | SEGMENT_CC | SEGMENT_BC | SEGMENT_D4 | SEGMENT_C0;
            break;
        case 4:
            blockedSegments = SEGMENT_B4 | SEGMENT_C8 | SEGMENT_B8;
            break;
        case 5:
            blockedSegments = SEGMENT_BC | SEGMENT_D4 | SEGMENT_C0;
            break;
        case 6:
            blockedSegments = SEGMENT_B4 | SEGMENT_C8 | SEGMENT_B8 | SEGMENT_D0 | SEGMENT_C0;
            break;
        case 7:
            blockedSegments = SEGMENT_B8 | SEGMENT_D0 | SEGMENT_C0 | SEGMENT_D4 | SEGMENT_BC;
            break;
        case 8:
            blockedSegments = SEGMENT_B8 | SEGMENT_D0 | SEGMENT_C0;
            break;
    }
    paint_util_set_segment_support_height(session, blockedSegments, 0xFFFF, 0);
    paint_util_set_segment_support_height(session, SEGMENTS_ALL & ~blockedSegments, height + 2, 0x20);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x0070DD7C */
static void paint_observation_tower_section(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    if (trackSequence == 1)
    {
        return;
    }

    uint32_t imageId = SPR_OBSERVATION_TOWER_SEGMENT | session->TrackColours[SCHEME_TRACK];
    sub_98197C(session, imageId, 0, 0, 2, 2, 30, height, 8, 8, height);

    const TileElement* nextTileElement = tileElement + 1;
    if (tileElement->IsLastForTile() || tileElement->GetClearanceZ() != nextTileElement->GetBaseZ())
    {
        imageId = SPR_OBSERVATION_TOWER_SEGMENT_TOP | session->TrackColours[SCHEME_TRACK];
        sub_98199C(session, imageId, 0, 0, 2, 2, 30, height, 8, 8, height);
    }

    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);

    paint_util_set_vertical_tunnel(session, height + 32);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/**
 * rct2: 0x0070DC5C
 */
TRACK_PAINT_FUNCTION get_track_paint_function_observation_tower(int32_t trackType)
{
    switch (trackType)
    {
        case TrackElemType::TowerBase:
            return paint_observation_tower_base;

        case TrackElemType::TowerSection:
            return paint_observation_tower_section;
    }

    return nullptr;
}
