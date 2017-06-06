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
    SPIRAL_SLIDE_LEFT_R0 = 20548,
    SPIRAL_SLIDE_CENTER_R0 = 20549,
    SPIRAL_SLIDE_RIGHT_R0 = 20550,

    SPIRAL_SLIDE_LEFT_R1 = 20551,
    SPIRAL_SLIDE_CENTER_R1 = 20552,
    SPIRAL_SLIDE_RIGHT_R1 = 20553,

    SPIRAL_SLIDE_LEFT_R2 = 20554,
    SPIRAL_SLIDE_CENTER_R2 = 20555,
    SPIRAL_SLIDE_RIGHT_R2 = 20556,

    SPIRAL_SLIDE_LEFT_R3 = 20557,
    SPIRAL_SLIDE_CENTER_R3 = 20558,
    SPIRAL_SLIDE_RIGHT_R3 = 20559,

    SPIRAL_SLIDE_INSIDE_R1 = 20560,
    SPIRAL_SLIDE_INSIDE_R2 = 20561,

    SPIRAL_SLIDE_BASE_A = 20562,
    SPIRAL_SLIDE_BASE_B = 20563,

    SPIRAL_SLIDE_FENCE_TOP_RIGHT = 20564,
    SPIRAL_SLIDE_FENCE_BOTTOM_RIGHT = 20565,
    SPIRAL_SLIDE_FENCE_BOTTOM_LEFT = 20566,
    SPIRAL_SLIDE_FENCE_TOP_LEFT = 20567,

    SPIRAL_SLIDE_PEEP = 20568, // 46 sprites per direction
};

static void spiral_slide_paint_tile_right(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement) {
    uint32 image_id = 0;

    if (direction == 0) image_id = SPIRAL_SLIDE_RIGHT_R0 | gTrackColours[SCHEME_TRACK];
    if (direction == 1) image_id = SPIRAL_SLIDE_RIGHT_R1 | gTrackColours[SCHEME_TRACK];
    if (direction == 2) image_id = SPIRAL_SLIDE_RIGHT_R2 | gTrackColours[SCHEME_TRACK];
    if (direction == 3) image_id = SPIRAL_SLIDE_RIGHT_R3 | gTrackColours[SCHEME_TRACK];

    sub_98197C(image_id, 16, 16, 16, 16, 108, height, 16, 0, height + 3, get_current_rotation());

}

static void spiral_slide_paint_tile_left(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement) {
    uint32 image_id = 0;

    if (direction == 0) image_id = SPIRAL_SLIDE_LEFT_R0 | gTrackColours[SCHEME_TRACK];
    if (direction == 1) image_id = SPIRAL_SLIDE_LEFT_R1 | gTrackColours[SCHEME_TRACK];
    if (direction == 2) image_id = SPIRAL_SLIDE_LEFT_R2 | gTrackColours[SCHEME_TRACK];
    if (direction == 3) image_id = SPIRAL_SLIDE_LEFT_R3 | gTrackColours[SCHEME_TRACK];

    sub_98197C(image_id, 16, 16, 16, 16, 108, height, 0, 16, height + 3, get_current_rotation());
}

static void spiral_slide_paint_tile_front(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement) {
    uint32 image_id = 0;

    rct_ride *ride = get_ride(rideIndex);

    if (direction == 1) {
        image_id = SPIRAL_SLIDE_INSIDE_R1 | gTrackColours[SCHEME_TRACK];
        sub_98197C(image_id, 16, 16, 2, 16, 108, height, -12, 0, height + 3, get_current_rotation());
    } else if (direction == 2) {
        image_id = SPIRAL_SLIDE_INSIDE_R2 | gTrackColours[SCHEME_TRACK];
        sub_98197C(image_id, 16, 16, 16, 2, 108, height, 0, -12, height + 3, get_current_rotation());
    }

    if (direction == 0) {
        image_id = SPIRAL_SLIDE_CENTER_R0 | gTrackColours[SCHEME_TRACK];
        sub_98197C(image_id, 16, 16, 16, 8, 108, height, 0, 8, height + 3, get_current_rotation());
    } else if (direction == 1) {
        image_id = SPIRAL_SLIDE_CENTER_R1 | gTrackColours[SCHEME_TRACK];
        sub_98197C(image_id, 16, 16, 2, 16, 108, height, 14, 0, height + 3, get_current_rotation());
    } else if (direction == 2) {
        image_id = SPIRAL_SLIDE_CENTER_R2 | gTrackColours[SCHEME_TRACK];
        sub_98197C(image_id, 16, 16, 16, 2, 108, height, 0, 14, height + 3, get_current_rotation());
    } else if (direction == 3) {
        image_id = SPIRAL_SLIDE_CENTER_R3 | gTrackColours[SCHEME_TRACK];
        sub_98197C(image_id, 16, 16, 8, 16, 108, height, 8, 0, height + 3, get_current_rotation());
    }

    rct_drawpixelinfo *dpi = unk_140E9A8;
    if (dpi->zoom_level == 0 && ride->slide_in_use != 0) {
        uint8 slide_progress = ride->spiral_slide_progress;
        if (slide_progress != 0) {
            slide_progress--;
        }

        if (slide_progress == 46) {
            slide_progress--;
        }

        if (slide_progress < 46) {

            sint32 offset = SPIRAL_SLIDE_PEEP + 46 * direction;
            rct_xyz8 boundingBox = {
                .z = 108
            };
            rct_xyz16 boundingBoxOffset = {
                .z = height + 3
            };

            if (direction == 0) {
                boundingBoxOffset.x = 0;
                boundingBoxOffset.y = 8;
                boundingBox.y = 8;
                boundingBox.x = 16;
            } else if (direction == 1) {
                boundingBoxOffset.x = 14;
                boundingBoxOffset.y = 0;
                boundingBox.y = 2;
                boundingBox.x = 16;
            } else if (direction == 2) {
                boundingBoxOffset.x = 0;
                boundingBoxOffset.y = 14;
                boundingBox.y = 16;
                boundingBox.x = 2;
            } else if (direction == 3) {
                boundingBoxOffset.x = 8;
                boundingBoxOffset.y = 0;
                boundingBox.y = 16;
                boundingBox.x = 8;
            }

            image_id = (offset + slide_progress) | (ride->slide_peep_t_shirt_colour << 19) | (1 << 29);

            sub_98199C(image_id, 16, 16, boundingBox.x, boundingBox.y, boundingBox.z, height, boundingBoxOffset.x, boundingBoxOffset.y, boundingBoxOffset.z, get_current_rotation());
        }
    }
}

static const uint32 spiral_slide_fence_sprites[] = {
    SPIRAL_SLIDE_FENCE_TOP_RIGHT,
    SPIRAL_SLIDE_FENCE_BOTTOM_RIGHT,
    SPIRAL_SLIDE_FENCE_BOTTOM_LEFT,
    SPIRAL_SLIDE_FENCE_TOP_LEFT,
};

/**
 * rct: 0x007485C8
 */
static void paint_spiral_slide(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element *mapElement) {
    trackSequence = track_map_2x2[direction][trackSequence];

    sint32 edges = edges_2x2[trackSequence];
    rct_ride * ride = get_ride(rideIndex);
    rct_xy16 position = {gPaintMapPosition.x, gPaintMapPosition.y};

    wooden_a_supports_paint_setup(direction & 1, 0, height, gTrackColours[SCHEME_MISC], NULL);

    // Base
    uint32 imageId = ((direction & 1) ? SPIRAL_SLIDE_BASE_B : SPIRAL_SLIDE_BASE_A) | gTrackColours[SCHEME_SUPPORTS];
    sub_98197C(imageId, 0, 0, 32, 32, 1, height, 0, 0, height, get_current_rotation());

    track_paint_util_paint_fences(edges, position, mapElement, ride, gTrackColours[SCHEME_TRACK], height, spiral_slide_fence_sprites, get_current_rotation());

    switch (trackSequence) {
        case 1: spiral_slide_paint_tile_right(rideIndex, trackSequence, direction, height, mapElement); break;
        case 2: spiral_slide_paint_tile_left(rideIndex, trackSequence, direction, height, mapElement); break;
        case 3: spiral_slide_paint_tile_front(rideIndex, trackSequence, direction, height, mapElement); break;
    }

    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 128, 0x20);
}

/**
 * rct2: 0x0074840C
 */
TRACK_PAINT_FUNCTION get_track_paint_function_spiral_slide(sint32 trackType, sint32 direction) {
    if (trackType != FLAT_TRACK_ELEM_2_X_2) {
        return NULL;
    }

    return paint_spiral_slide;
}
