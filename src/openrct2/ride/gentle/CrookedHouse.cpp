/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../interface/Viewport.h"
#include "../../paint/Paint.h"
#include "../../paint/Supports.h"
#include "../Track.h"
#include "../TrackPaint.h"
#include "../../world/Sprite.h"

struct rct_crooked_house_bound_box
{
    int16_t offset_x;
    int16_t offset_y;
    int16_t length_x;
    int16_t length_y;
};

static constexpr const rct_crooked_house_bound_box crooked_house_data[] = { { 6, 0, 42, 24 },
                                                     { 0, 0, 0, 0 },
                                                     { -16, -16, 32, 32 },
                                                     { 0, 0, 0, 0 }, // Unused
                                                     { 0, 6, 24, 42 } };

/**
 *  rct2: 0x0088ABA4
 */
static void paint_crooked_house_structure(paint_session * session, uint8_t direction, uint8_t x_offset, uint8_t y_offset,
                                          uint32_t segment, int32_t height)
{
    const rct_tile_element * original_tile_element = static_cast<const rct_tile_element *>(session->CurrentlyDrawnItem);

    Ride * ride = get_ride(track_element_get_ride_index(original_tile_element));

    rct_ride_entry * rideEntry = get_ride_entry(ride->subtype);

    if (ride->lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK)
    {
        if (ride->vehicles[0] != SPRITE_INDEX_NULL)
        {
            rct_sprite * sprite         = get_sprite(ride->vehicles[0]);
            session->InteractionType    = VIEWPORT_INTERACTION_ITEM_SPRITE;
            session->CurrentlyDrawnItem = sprite;
        }
    }

    uint32_t image_id = (direction + rideEntry->vehicles[0].base_image_id) | session->TrackColours[SCHEME_MISC];

    rct_crooked_house_bound_box boundBox = crooked_house_data[segment];
    sub_98197C(
        session, image_id, x_offset, y_offset, boundBox.length_x, boundBox.length_y, 127, height + 3, boundBox.offset_x,
        boundBox.offset_y, height + 3);
}

static void paint_crooked_house(
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

    track_paint_util_paint_floor(session, edges, session->TrackColours[SCHEME_TRACK], height, floorSpritesCork);

    track_paint_util_paint_fences(
        session, edges, position, tileElement, ride, session->TrackColours[SCHEME_MISC], height, fenceSpritesRope,
        session->CurrentRotation);

    switch (trackSequence)
    {
    case 3:
        paint_crooked_house_structure(session, direction, 32, 224, 0, height);
        break;
    // case 5: sub_88ABA4(direction, 0, 224, 1, height); break;
    case 6:
        paint_crooked_house_structure(session, direction, 224, 32, 4, height);
        break;
    case 7:
        paint_crooked_house_structure(session, direction, 224, 224, 2, height);
        break;
        // case 8: sub_88ABA4(rideIndex, 224, 0, 3, height); break;
    }

    int32_t cornerSegments = 0;
    switch (trackSequence)
    {
    case 1:
        // Top
        cornerSegments = SEGMENT_B4 | SEGMENT_C8 | SEGMENT_CC;
        break;
    case 3:
        // Right
        cornerSegments = SEGMENT_CC | SEGMENT_BC | SEGMENT_D4;
        break;
    case 6:
        // Left
        cornerSegments = SEGMENT_C8 | SEGMENT_B8 | SEGMENT_D0;
        break;
    case 7:
        // Bottom
        cornerSegments = SEGMENT_D0 | SEGMENT_C0 | SEGMENT_D4;
        break;
    }

    paint_util_set_segment_support_height(session, cornerSegments, height + 2, 0x20);
    paint_util_set_segment_support_height(session, SEGMENTS_ALL & ~cornerSegments, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 128, 0x20);
}

TRACK_PAINT_FUNCTION get_track_paint_function_crooked_house(int32_t trackType, int32_t direction)
{
    if (trackType != FLAT_TRACK_ELEM_3_X_3)
    {
        return nullptr;
    }

    return paint_crooked_house;
}
