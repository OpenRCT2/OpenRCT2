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

/** rct2: 0x0076E5C9 */
static void paint_twist_structure(paint_session * session, Ride * ride, uint8_t direction, int8_t xOffset, int8_t yOffset,
                                  uint16_t height)
{
    const rct_tile_element * savedTileElement = static_cast<const rct_tile_element *>(session->CurrentlyDrawnItem);

    rct_ride_entry * rideEntry = get_ride_entry(ride->subtype);
    rct_vehicle *    vehicle   = nullptr;

    if (rideEntry == nullptr)
    {
        return;
    }

    height += 7;
    uint32_t baseImageId = rideEntry->vehicles[0].base_image_id;

    if (ride->lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK && ride->vehicles[0] != SPRITE_INDEX_NULL)
    {
        vehicle = GET_VEHICLE(ride->vehicles[0]);

        session->InteractionType    = VIEWPORT_INTERACTION_ITEM_SPRITE;
        session->CurrentlyDrawnItem = vehicle;
    }

    uint32_t frameNum = (direction * 88) % 216;
    if (vehicle != nullptr)
    {
        frameNum += (vehicle->sprite_direction >> 3) << 4;
        frameNum += vehicle->vehicle_sprite_type;
        frameNum = frameNum % 216;
    }

    uint32_t imageColourFlags = session->TrackColours[SCHEME_MISC];
    if (imageColourFlags == IMAGE_TYPE_REMAP)
    {
        imageColourFlags =
            SPRITE_ID_PALETTE_COLOUR_2(ride->vehicle_colours[0].body_colour, ride->vehicle_colours[0].trim_colour);
    }

    uint32_t structureFrameNum = frameNum % 24;
    uint32_t imageId           = (baseImageId + structureFrameNum) | imageColourFlags;
    sub_98197C(session, imageId, xOffset, yOffset, 24, 24, 48, height, xOffset + 16, yOffset + 16, height);

    rct_drawpixelinfo * dpi = session->DPI;

    if (dpi->zoom_level < 1 && ride->lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK && vehicle != nullptr)
    {

        for (int32_t i = 0; i < vehicle->num_peeps; i += 2)
        {
            imageColourFlags = SPRITE_ID_PALETTE_COLOUR_2(vehicle->peep_tshirt_colours[i], vehicle->peep_tshirt_colours[i + 1]);

            uint32_t peepFrameNum = (frameNum + i * 12) % 216;
            imageId             = (baseImageId + 24 + peepFrameNum) | imageColourFlags;
            sub_98199C(session, imageId, xOffset, yOffset, 24, 24, 48, height, xOffset + 16, yOffset + 16, height);
        }
    }

    session->CurrentlyDrawnItem = savedTileElement;
    session->InteractionType    = VIEWPORT_INTERACTION_ITEM_RIDE;
}

/** rct2: 0x0076D858 */
static void paint_twist(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement)
{
    trackSequence = track_map_3x3[direction][trackSequence];

    const uint8_t edges    = edges_3x3[trackSequence];
    Ride *      ride     = get_ride(rideIndex);
    LocationXY16    position = session->MapPosition;

    uint32_t imageId;

    wooden_a_supports_paint_setup(session, (direction & 1), 0, height, session->TrackColours[SCHEME_MISC], nullptr);

    track_paint_util_paint_floor(session, edges, session->TrackColours[SCHEME_MISC], height, floorSpritesCork);

    switch (trackSequence)
    {
    case 7:
        if (track_paint_util_has_fence(EDGE_SW, position, tileElement, ride, session->CurrentRotation))
        {
            imageId = SPR_FENCE_ROPE_SW | session->TrackColours[SCHEME_MISC];
            sub_98197C(session, imageId, 0, 0, 1, 28, 7, height, 29, 0, height + 3);
        }
        if (track_paint_util_has_fence(EDGE_SE, position, tileElement, ride, session->CurrentRotation))
        {
            imageId = SPR_FENCE_ROPE_SE | session->TrackColours[SCHEME_MISC];
            sub_98197C(session, imageId, 0, 0, 28, 1, 7, height, 0, 29, height + 3);
        }
        break;
    default:
        track_paint_util_paint_fences(
            session, edges, position, tileElement, ride, session->TrackColours[SCHEME_MISC], height, fenceSpritesRope,
            session->CurrentRotation);
        break;
    }

    switch (trackSequence)
    {
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

    int32_t cornerSegments = 0;
    switch (trackSequence)
    {
    case 1:
        cornerSegments = SEGMENT_B4 | SEGMENT_C8 | SEGMENT_CC;
        break;
    case 3:
        cornerSegments = SEGMENT_CC | SEGMENT_BC | SEGMENT_D4;
        break;
    case 6:
        cornerSegments = SEGMENT_C8 | SEGMENT_B8 | SEGMENT_D0;
        break;
    case 7:
        cornerSegments = SEGMENT_D0 | SEGMENT_C0 | SEGMENT_D4;
        break;
    }

    paint_util_set_segment_support_height(session, cornerSegments, height + 2, 0x20);
    paint_util_set_segment_support_height(session, SEGMENTS_ALL & ~cornerSegments, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 64, 0x20);
}

/**
 * rct2: 0x0076D658
 */
TRACK_PAINT_FUNCTION get_track_paint_function_twist(int32_t trackType, int32_t direction)
{
    if (trackType != FLAT_TRACK_ELEM_3_X_3)
    {
        return nullptr;
    }

    return paint_twist;
}
