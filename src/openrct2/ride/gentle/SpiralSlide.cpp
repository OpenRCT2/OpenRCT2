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

enum
{
    SPIRAL_SLIDE_LEFT_R0   = 20548,
    SPIRAL_SLIDE_CENTRE_R0 = 20549,
    SPIRAL_SLIDE_RIGHT_R0  = 20550,

    SPIRAL_SLIDE_LEFT_R1   = 20551,
    SPIRAL_SLIDE_CENTRE_R1 = 20552,
    SPIRAL_SLIDE_RIGHT_R1  = 20553,

    SPIRAL_SLIDE_LEFT_R2   = 20554,
    SPIRAL_SLIDE_CENTRE_R2 = 20555,
    SPIRAL_SLIDE_RIGHT_R2  = 20556,

    SPIRAL_SLIDE_LEFT_R3   = 20557,
    SPIRAL_SLIDE_CENTRE_R3 = 20558,
    SPIRAL_SLIDE_RIGHT_R3  = 20559,

    SPIRAL_SLIDE_INSIDE_R1 = 20560,
    SPIRAL_SLIDE_INSIDE_R2 = 20561,

    SPIRAL_SLIDE_BASE_A = 20562,
    SPIRAL_SLIDE_BASE_B = 20563,

    SPIRAL_SLIDE_FENCE_TOP_RIGHT    = 20564,
    SPIRAL_SLIDE_FENCE_BOTTOM_RIGHT = 20565,
    SPIRAL_SLIDE_FENCE_BOTTOM_LEFT  = 20566,
    SPIRAL_SLIDE_FENCE_TOP_LEFT     = 20567,

    SPIRAL_SLIDE_PEEP = 20568, // 46 sprites per direction
};

static void spiral_slide_paint_tile_right(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement)
{
    uint32_t image_id = 0;

    if (direction == 0)
        image_id = SPIRAL_SLIDE_RIGHT_R0 | session->TrackColours[SCHEME_TRACK];
    if (direction == 1)
        image_id = SPIRAL_SLIDE_RIGHT_R1 | session->TrackColours[SCHEME_TRACK];
    if (direction == 2)
        image_id = SPIRAL_SLIDE_RIGHT_R2 | session->TrackColours[SCHEME_TRACK];
    if (direction == 3)
        image_id = SPIRAL_SLIDE_RIGHT_R3 | session->TrackColours[SCHEME_TRACK];

    sub_98197C(session, image_id, 16, 16, 16, 16, 108, height, 16, 0, height + 3);
}

static void spiral_slide_paint_tile_left(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement)
{
    uint32_t image_id = 0;

    if (direction == 0)
        image_id = SPIRAL_SLIDE_LEFT_R0 | session->TrackColours[SCHEME_TRACK];
    if (direction == 1)
        image_id = SPIRAL_SLIDE_LEFT_R1 | session->TrackColours[SCHEME_TRACK];
    if (direction == 2)
        image_id = SPIRAL_SLIDE_LEFT_R2 | session->TrackColours[SCHEME_TRACK];
    if (direction == 3)
        image_id = SPIRAL_SLIDE_LEFT_R3 | session->TrackColours[SCHEME_TRACK];

    sub_98197C(session, image_id, 16, 16, 16, 16, 108, height, 0, 16, height + 3);
}

static void spiral_slide_paint_tile_front(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement)
{
    uint32_t image_id = 0;

    Ride * ride = get_ride(rideIndex);

    if (direction == 1)
    {
        image_id = SPIRAL_SLIDE_INSIDE_R1 | session->TrackColours[SCHEME_TRACK];
        sub_98197C(session, image_id, 16, 16, 2, 16, 108, height, -12, 0, height + 3);
    }
    else if (direction == 2)
    {
        image_id = SPIRAL_SLIDE_INSIDE_R2 | session->TrackColours[SCHEME_TRACK];
        sub_98197C(session, image_id, 16, 16, 16, 2, 108, height, 0, -12, height + 3);
    }

    if (direction == 0)
    {
        image_id = SPIRAL_SLIDE_CENTRE_R0 | session->TrackColours[SCHEME_TRACK];
        sub_98197C(session, image_id, 16, 16, 16, 8, 108, height, 0, 8, height + 3);
    }
    else if (direction == 1)
    {
        image_id = SPIRAL_SLIDE_CENTRE_R1 | session->TrackColours[SCHEME_TRACK];
        sub_98197C(session, image_id, 16, 16, 2, 16, 108, height, 14, 0, height + 3);
    }
    else if (direction == 2)
    {
        image_id = SPIRAL_SLIDE_CENTRE_R2 | session->TrackColours[SCHEME_TRACK];
        sub_98197C(session, image_id, 16, 16, 16, 2, 108, height, 0, 14, height + 3);
    }
    else if (direction == 3)
    {
        image_id = SPIRAL_SLIDE_CENTRE_R3 | session->TrackColours[SCHEME_TRACK];
        sub_98197C(session, image_id, 16, 16, 8, 16, 108, height, 8, 0, height + 3);
    }

    rct_drawpixelinfo * dpi = session->DPI;
    if (dpi->zoom_level == 0 && ride->slide_in_use != 0)
    {
        uint8_t slide_progress = ride->spiral_slide_progress;
        if (slide_progress != 0)
        {
            slide_progress--;
        }

        if (slide_progress == 46)
        {
            slide_progress--;
        }

        if (slide_progress < 46)
        {

            int32_t       offset            = SPIRAL_SLIDE_PEEP + 46 * direction;
            CoordsXYZ boundingBox       = { 0, 0, 108 };
            CoordsXYZ boundingBoxOffset = { 0, 0, static_cast<int16_t>(height + 3) };

            if (direction == 0)
            {
                boundingBoxOffset.x = 0;
                boundingBoxOffset.y = 8;
                boundingBox.y       = 8;
                boundingBox.x       = 16;
            }
            else if (direction == 1)
            {
                boundingBoxOffset.x = 14;
                boundingBoxOffset.y = 0;
                boundingBox.y       = 2;
                boundingBox.x       = 16;
            }
            else if (direction == 2)
            {
                boundingBoxOffset.x = 0;
                boundingBoxOffset.y = 14;
                boundingBox.y       = 16;
                boundingBox.x       = 2;
            }
            else if (direction == 3)
            {
                boundingBoxOffset.x = 8;
                boundingBoxOffset.y = 0;
                boundingBox.y       = 16;
                boundingBox.x       = 8;
            }

            image_id = (offset + slide_progress) | (ride->slide_peep_t_shirt_colour << 19) | (1 << 29);

            sub_98199C(
                session, image_id, 16, 16, boundingBox.x, boundingBox.y, boundingBox.z, height, boundingBoxOffset.x,
                boundingBoxOffset.y, boundingBoxOffset.z);
        }
    }
}

static constexpr const uint32_t spiral_slide_fence_sprites[] = {
    SPIRAL_SLIDE_FENCE_TOP_RIGHT,
    SPIRAL_SLIDE_FENCE_BOTTOM_RIGHT,
    SPIRAL_SLIDE_FENCE_BOTTOM_LEFT,
    SPIRAL_SLIDE_FENCE_TOP_LEFT,
};

/**
 * rct: 0x007485C8
 */
static void paint_spiral_slide(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement)
{
    trackSequence = track_map_2x2[direction][trackSequence];

    int32_t   edges    = edges_2x2[trackSequence];
    Ride *   ride     = get_ride(rideIndex);
    LocationXY16 position = session->MapPosition;

    wooden_a_supports_paint_setup(session, direction & 1, 0, height, session->TrackColours[SCHEME_MISC], nullptr);

    // Base
    uint32_t imageId = ((direction & 1) ? SPIRAL_SLIDE_BASE_B : SPIRAL_SLIDE_BASE_A) | session->TrackColours[SCHEME_SUPPORTS];
    sub_98197C(session, imageId, 0, 0, 32, 32, 1, height, 0, 0, height);

    track_paint_util_paint_fences(
        session, edges, position, tileElement, ride, session->TrackColours[SCHEME_TRACK], height, spiral_slide_fence_sprites,
        session->CurrentRotation);

    switch (trackSequence)
    {
    case 1:
        spiral_slide_paint_tile_right(session, rideIndex, trackSequence, direction, height, tileElement);
        break;
    case 2:
        spiral_slide_paint_tile_left(session, rideIndex, trackSequence, direction, height, tileElement);
        break;
    case 3:
        spiral_slide_paint_tile_front(session, rideIndex, trackSequence, direction, height, tileElement);
        break;
    }

    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 128, 0x20);
}

/**
 * rct2: 0x0074840C
 */
TRACK_PAINT_FUNCTION get_track_paint_function_spiral_slide(int32_t trackType, int32_t direction)
{
    if (trackType != FLAT_TRACK_ELEM_2_X_2)
    {
        return nullptr;
    }

    return paint_spiral_slide;
}
