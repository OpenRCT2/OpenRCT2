/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
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
#include "../Track.h"
#include "../TrackPaint.h"
#include "../../world/Sprite.h"

/** rct2: 0x008A2ABC */
static void paint_enterprise_structure(
    paint_session * session, Ride * ride, int8_t xOffset, int8_t yOffset, uint16_t height, const rct_tile_element * tileElement)
{
    height += 7;

    const rct_tile_element * savedTileElement = static_cast<const rct_tile_element *>(session->CurrentlyDrawnItem);
    rct_ride_entry *  rideEntry       = get_ride_entry(ride->subtype);
    rct_vehicle *     vehicle         = nullptr;

    if (rideEntry == nullptr)
    {
        return;
    }

    uint32_t baseImageId = rideEntry->vehicles[0].base_image_id;

    if (ride->lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK && ride->vehicles[0] != SPRITE_INDEX_NULL)
    {
        session->InteractionType    = VIEWPORT_INTERACTION_ITEM_SPRITE;
        vehicle                     = GET_VEHICLE(ride->vehicles[0]);
        session->CurrentlyDrawnItem = vehicle;
    }

    uint32_t imageOffset = tile_element_get_direction_with_offset(tileElement, session->CurrentRotation);
    if (vehicle != nullptr)
    {
        imageOffset = (vehicle->vehicle_sprite_type << 2) + (((vehicle->sprite_direction >> 3) + session->CurrentRotation) % 4);
    }

    uint32_t imageColourFlags = session->TrackColours[SCHEME_MISC];
    if (imageColourFlags == IMAGE_TYPE_REMAP)
    {
        imageColourFlags =
            SPRITE_ID_PALETTE_COLOUR_2(ride->vehicle_colours[0].body_colour, ride->vehicle_colours[0].trim_colour);
    }

    uint32_t imageId = (baseImageId + imageOffset) | imageColourFlags;
    sub_98197C(session, imageId, xOffset, yOffset, 24, 24, 48, height, 0, 0, height);

    rct_drawpixelinfo * dpi = session->DPI;

    if (dpi->zoom_level == 0 && imageOffset < 12 && ride->lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK && vehicle != nullptr)
    {
        for (int32_t i = 0; i < 15; i++)
        {
            if (vehicle->num_peeps <= i)
            {
                break;
            }

            uint32_t peepFrameOffset = ((imageOffset % 4) * 4 + (i * 4) % 15) & 0x0F;
            uint32_t ax              = (imageOffset & 0xFFFFFFFC) << 2;
            imageId = (baseImageId + 196 + peepFrameOffset + ax) | SPRITE_ID_PALETTE_COLOUR_1(vehicle->peep_tshirt_colours[i]);
            sub_98199C(session, imageId, xOffset, yOffset, 24, 24, 48, height, 0, 0, height);
        }
    }

    session->CurrentlyDrawnItem = savedTileElement;
    session->InteractionType    = VIEWPORT_INTERACTION_ITEM_RIDE;
}

/** rct2: 0x008A1584 */
static void paint_enterprise(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement)
{
    trackSequence = track_map_4x4[direction][trackSequence];

    int32_t   edges    = edges_4x4[trackSequence];
    Ride *   ride     = get_ride(rideIndex);
    LocationXY16 position = session->MapPosition;

    wooden_a_supports_paint_setup(session, direction & 1, 0, height, session->TrackColours[SCHEME_MISC], nullptr);

    track_paint_util_paint_floor(session, edges, session->TrackColours[SCHEME_TRACK], height, floorSpritesCork);

    track_paint_util_paint_fences(
        session, edges, position, tileElement, ride, session->TrackColours[SCHEME_TRACK], height, fenceSpritesRope,
        session->CurrentRotation);

    switch (trackSequence)
    {
    case 5:
        paint_enterprise_structure(session, ride, 16, 16, height, tileElement);
        break;
    case 6:
        paint_enterprise_structure(session, ride, 16, -16, height, tileElement);
        break;
    case 10:
        paint_enterprise_structure(session, ride, -16, -16, height, tileElement);
        break;
    case 9:
        paint_enterprise_structure(session, ride, -16, 16, height, tileElement);
        break;

    case 0:
        paint_enterprise_structure(session, ride, 48, 48, height, tileElement);
        break;
    case 3:
        paint_enterprise_structure(session, ride, 48, -48, height, tileElement);
        break;
    case 15:
        paint_enterprise_structure(session, ride, -48, -48, height, tileElement);
        break;
    case 12:
        paint_enterprise_structure(session, ride, -48, 48, height, tileElement);
        break;

    case 7:
        paint_enterprise_structure(session, ride, 16, -48, height, tileElement);
        break;
    case 11:
        paint_enterprise_structure(session, ride, -16, -48, height, tileElement);
        break;
    case 14:
        paint_enterprise_structure(session, ride, -48, -16, height, tileElement);
        break;
    case 13:
        paint_enterprise_structure(session, ride, -48, 16, height, tileElement);
        break;
    }

    int32_t cornerSegments = 0;
    switch (trackSequence)
    {
    case 0:
        cornerSegments = SEGMENT_B4 | SEGMENT_C8 | SEGMENT_CC;
        break;
    case 3:
        cornerSegments = SEGMENT_CC | SEGMENT_BC | SEGMENT_D4;
        break;
    case 12:
        cornerSegments = SEGMENT_C8 | SEGMENT_B8 | SEGMENT_D0;
        break;
    case 15:
        cornerSegments = SEGMENT_D0 | SEGMENT_C0 | SEGMENT_D4;
        break;
    }
    paint_util_set_segment_support_height(session, cornerSegments, height + 2, 0x20);
    paint_util_set_segment_support_height(session, SEGMENTS_ALL & ~cornerSegments, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 160, 0x20);
}

/**
 * rct2: 0x008A13B4
 */
TRACK_PAINT_FUNCTION get_track_paint_function_enterprise(int32_t trackType, int32_t direction)
{
    if (trackType != FLAT_TRACK_ELEM_4_X_4)
    {
        return nullptr;
    }

    return paint_enterprise;
}
