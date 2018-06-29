/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../common.h"
#include "../../config/Config.h"
#include "../../interface/Viewport.h"
#include "../../paint/Paint.h"
#include "../../paint/Supports.h"
#include "../../world/Sprite.h"
#include "../Track.h"
#include "../TrackPaint.h"
#include "../VehiclePaint.h"

enum
{
    SPR_LAUNCHED_FREEFALL_TOWER_BASE        = 14564,
    SPR_LAUNCHED_FREEFALL_TOWER_SEGMENT     = 14565,
    SPR_LAUNCHED_FREEFALL_TOWER_SEGMENT_TOP = 14566,
};

static constexpr const uint32_t launched_freefall_fence_sprites[] = { SPR_FENCE_METAL_NE, SPR_FENCE_METAL_SE, SPR_FENCE_METAL_SW,
                                                          SPR_FENCE_METAL_NW };

/**
 *
 *  rct2: 0x006D5FAB
 */
void vehicle_visual_launched_freefall(paint_session * session, int32_t x, int32_t imageDirection, int32_t y, int32_t z,
                                      const rct_vehicle * vehicle, const rct_ride_entry_vehicle * vehicleEntry)
{
    int32_t image_id;
    int32_t baseImage_id = vehicleEntry->base_image_id + ((vehicle->restraints_position / 64) * 2);

    // Draw back:
    image_id = (baseImage_id + 2) | SPRITE_ID_PALETTE_COLOUR_2(vehicle->colours.body_colour, vehicle->colours.trim_colour);
    sub_98197C(session, image_id, 0, 0, 2, 2, 41, z, -11, -11, z + 1);

    // Draw front:
    image_id = (baseImage_id + 1) | SPRITE_ID_PALETTE_COLOUR_2(vehicle->colours.body_colour, vehicle->colours.trim_colour);
    sub_98197C(session, image_id, 0, 0, 16, 16, 41, z, -5, -5, z + 1);

    // Draw peeps:
    if (session->DPI->zoom_level < 2)
    {
        if (vehicle->num_peeps > 0)
        {
            baseImage_id = vehicleEntry->base_image_id + 9;
            if ((vehicle->restraints_position / 64) == 3)
            {
                baseImage_id += 2; // Draw peeps sitting without transparent area between them for restraints
            }
            image_id = (baseImage_id + ((((imageDirection / 8) + 0) & 3) * 3)) |
                       SPRITE_ID_PALETTE_COLOUR_2(vehicle->peep_tshirt_colours[0], vehicle->peep_tshirt_colours[1]);
            sub_98199C(session, image_id, 0, 0, 16, 16, 41, z, -5, -5, z + 1);
            if (vehicle->num_peeps > 2)
            {
                image_id = (baseImage_id + ((((imageDirection / 8) + 1) & 3) * 3)) |
                           SPRITE_ID_PALETTE_COLOUR_2(vehicle->peep_tshirt_colours[2], vehicle->peep_tshirt_colours[3]);
                sub_98199C(session, image_id, 0, 0, 16, 16, 41, z, -5, -5, z + 1);
            }
            if (vehicle->num_peeps > 4)
            {
                image_id = (baseImage_id + ((((imageDirection / 8) + 2) & 3) * 3)) |
                           SPRITE_ID_PALETTE_COLOUR_2(vehicle->peep_tshirt_colours[4], vehicle->peep_tshirt_colours[5]);
                sub_98199C(session, image_id, 0, 0, 16, 16, 41, z, -5, -5, z + 1);
            }
            if (vehicle->num_peeps > 6)
            {
                image_id = (baseImage_id + ((((imageDirection / 8) + 3) & 3) * 3)) |
                           SPRITE_ID_PALETTE_COLOUR_2(vehicle->peep_tshirt_colours[6], vehicle->peep_tshirt_colours[7]);
                sub_98199C(session, image_id, 0, 0, 16, 16, 41, z, -5, -5, z + 1);
            }
        }
    }

    assert(vehicleEntry->effect_visual == 1);
}

/** rct2: 0x006FD1F8 */
static void paint_launched_freefall_base(
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

    uint32_t imageId = SPR_FLOOR_METAL | session->TrackColours[SCHEME_SUPPORTS];
    sub_98197C(session, imageId, 0, 0, 32, 32, 1, height, 0, 0, height);

    track_paint_util_paint_fences(
        session, edges, position, tileElement, ride, session->TrackColours[SCHEME_TRACK], height,
        launched_freefall_fence_sprites, session->CurrentRotation);

    if (trackSequence == 0)
    {
        imageId = SPR_LAUNCHED_FREEFALL_TOWER_BASE | session->TrackColours[SCHEME_TRACK];
        sub_98197C(session, imageId, 0, 0, 2, 2, 27, height, 8, 8, height + 3);

        height += 32;
        imageId = SPR_LAUNCHED_FREEFALL_TOWER_SEGMENT | session->TrackColours[SCHEME_TRACK];
        sub_98197C(session, imageId, 0, 0, 2, 2, 30, height, 8, 8, height);

        height += 32;
        imageId = SPR_LAUNCHED_FREEFALL_TOWER_SEGMENT | session->TrackColours[SCHEME_TRACK];
        sub_98197C(session, imageId, 0, 0, 2, 2, 30, height, 8, 8, height);

        paint_util_set_vertical_tunnel(session, height + 32);

        height -= 64;
    }

    int32_t blockedSegments = 0;
    switch (trackSequence)
    {
    case 0:
        blockedSegments = SEGMENTS_ALL;
        break;
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

/** rct2: 0x006FD208 */
static void paint_launched_freefall_tower_section(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement)
{
    if (trackSequence == 1)
    {
        return;
    }

    uint32_t imageId = SPR_LAUNCHED_FREEFALL_TOWER_SEGMENT | session->TrackColours[SCHEME_TRACK];
    sub_98197C(session, imageId, 0, 0, 2, 2, 30, height, 8, 8, height);

    const rct_tile_element * nextTileElement = tileElement + 1;
    if (tileElement->IsLastForTile() || tileElement->clearance_height != nextTileElement->base_height)
    {
        imageId = SPR_LAUNCHED_FREEFALL_TOWER_SEGMENT_TOP | session->TrackColours[SCHEME_TRACK];
        sub_98199C(session, imageId, 0, 0, 2, 2, 30, height, 8, 8, height);
    }

    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);

    paint_util_set_vertical_tunnel(session, height + 32);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/**
 * rct2: 0x006FD0E8
 */
TRACK_PAINT_FUNCTION get_track_paint_function_launched_freefall(int32_t trackType, int32_t direction)
{
    switch (trackType)
    {
    case TRACK_ELEM_TOWER_BASE:
        return paint_launched_freefall_base;

    case TRACK_ELEM_TOWER_SECTION:
        return paint_launched_freefall_tower_section;
    }

    return nullptr;
}
