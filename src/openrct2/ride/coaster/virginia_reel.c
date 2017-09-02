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
#include "../track_paint.h"
#include "../track.h"
#include "../vehicle_paint.h"
#include "../../interface/viewport.h"
#include "../../paint/paint.h"
#include "../../paint/supports.h"

// 0x009927E6:
static const vehicle_boundbox _virginiaReelBoundbox[] = {
    { -11, -11,  1, 22, 22, 13 },
    { -11, -11,  1, 22, 22, 13 },
    { -11, -11,  1, 22, 22, 13 },
    { -11, -11,  1, 22, 22, 13 },
    { -11, -11,  1, 22, 22, 13 },
    { -11, -11,  1, 22, 22, 13 },
    { -11, -11,  1, 22, 22, 13 },
    { -11, -11,  1, 22, 22, 13 },
    { -11, -11,  1, 22, 22, 13 },
};

enum
{
    SPR_VIRGINIA_REEL_FLAT_SW_NE = 21458,
    SPR_VIRGINIA_REEL_FLAT_NW_SE = 21459,
    SPR_VIRGINIA_REEL_FLAT_TO_25_DEG_UP_SW_NE = 21460,
    SPR_VIRGINIA_REEL_FLAT_TO_25_DEG_UP_NW_SE = 21461,
    SPR_VIRGINIA_REEL_FLAT_TO_25_DEG_UP_NE_SW = 21462,
    SPR_VIRGINIA_REEL_FLAT_TO_25_DEG_UP_SE_NW = 21463,
    SPR_VIRGINIA_REEL_25_DEG_UP_TO_FLAT_SW_NE = 21464,
    SPR_VIRGINIA_REEL_25_DEG_UP_TO_FLAT_NW_SE = 21465,
    SPR_VIRGINIA_REEL_25_DEG_UP_TO_FLAT_NE_SW = 21466,
    SPR_VIRGINIA_REEL_25_DEG_UP_TO_FLAT_SE_NW = 21467,
    SPR_VIRGINIA_REEL_25_DEG_UP_SW_NE = 21468,
    SPR_VIRGINIA_REEL_25_DEG_UP_NW_SE = 21469,
    SPR_VIRGINIA_REEL_25_DEG_UP_NE_SW = 21470,
    SPR_VIRGINIA_REEL_25_DEG_UP_SE_NW = 21471,
    SPR_VIRGINIA_REEL_FLAT_QUARTER_TURN_3_TILES_SW_SE_PART_0 = 21472,
    SPR_VIRGINIA_REEL_FLAT_QUARTER_TURN_3_TILES_SW_SE_PART_1 = 21473,
    SPR_VIRGINIA_REEL_FLAT_QUARTER_TURN_3_TILES_SW_SE_PART_2 = 21474,
    SPR_VIRGINIA_REEL_FLAT_QUARTER_TURN_3_TILES_NW_SW_PART_0 = 21475,
    SPR_VIRGINIA_REEL_FLAT_QUARTER_TURN_3_TILES_NW_SW_PART_1 = 21476,
    SPR_VIRGINIA_REEL_FLAT_QUARTER_TURN_3_TILES_NW_SW_PART_2 = 21477,
    SPR_VIRGINIA_REEL_FLAT_QUARTER_TURN_3_TILES_NE_NW_PART_0 = 21478,
    SPR_VIRGINIA_REEL_FLAT_QUARTER_TURN_3_TILES_NE_NW_PART_1 = 21479,
    SPR_VIRGINIA_REEL_FLAT_QUARTER_TURN_3_TILES_NE_NW_PART_2 = 21480,
    SPR_VIRGINIA_REEL_FLAT_QUARTER_TURN_3_TILES_SE_NE_PART_0 = 21481,
    SPR_VIRGINIA_REEL_FLAT_QUARTER_TURN_3_TILES_SE_NE_PART_1 = 21482,
    SPR_VIRGINIA_REEL_FLAT_QUARTER_TURN_3_TILES_SE_NE_PART_2 = 21483,
    SPR_VIRGINIA_REEL_FLAT_QUARTER_TURN_1_TILE_SW_NW = 21484,
    SPR_VIRGINIA_REEL_FLAT_QUARTER_TURN_1_TILE_NW_NE = 21485,
    SPR_VIRGINIA_REEL_FLAT_QUARTER_TURN_1_TILE_NE_SE = 21486,
    SPR_VIRGINIA_REEL_FLAT_QUARTER_TURN_1_TILE_SE_SW = 21487,
    SPR_VIRGINIA_REEL_FLAT_LIFT_HILL_SW_NE = 21488,
    SPR_VIRGINIA_REEL_FLAT_LIFT_HILL_NW_SE = 21489,
    SPR_VIRGINIA_REEL_FLAT_LIFT_HILL_NE_SW = 21490,
    SPR_VIRGINIA_REEL_FLAT_LIFT_HILL_SE_NW = 21491,
    SPR_VIRGINIA_REEL_FLAT_TO_25_DEG_UP_LIFT_HILL_SW_NE = 21492,
    SPR_VIRGINIA_REEL_FLAT_TO_25_DEG_UP_LIFT_HILL_NW_SE = 21493,
    SPR_VIRGINIA_REEL_FLAT_TO_25_DEG_UP_LIFT_HILL_NE_SW = 21494,
    SPR_VIRGINIA_REEL_FLAT_TO_25_DEG_UP_LIFT_HILL_SE_NW = 21495,
    SPR_VIRGINIA_REEL_25_DEG_UP_TO_FLAT_LIFT_HILL_SW_NE = 21496,
    SPR_VIRGINIA_REEL_25_DEG_UP_TO_FLAT_LIFT_HILL_NW_SE = 21497,
    SPR_VIRGINIA_REEL_25_DEG_UP_TO_FLAT_LIFT_HILL_NE_SW = 21498,
    SPR_VIRGINIA_REEL_25_DEG_UP_TO_FLAT_LIFT_HILL_SE_NW = 21499,
    SPR_VIRGINIA_REEL_25_DEG_UP_LIFT_HILL_SW_NE = 21500,
    SPR_VIRGINIA_REEL_25_DEG_UP_LIFT_HILL_NW_SE = 21501,
    SPR_VIRGINIA_REEL_25_DEG_UP_LIFT_HILL_NE_SW = 21502,
    SPR_VIRGINIA_REEL_25_DEG_UP_LIFT_HILL_SE_NW = 21503,
};

static const uint32 virginia_reel_track_pieces_flat[4] = {
    SPR_VIRGINIA_REEL_FLAT_SW_NE,
    SPR_VIRGINIA_REEL_FLAT_NW_SE,
    SPR_VIRGINIA_REEL_FLAT_SW_NE,
    SPR_VIRGINIA_REEL_FLAT_NW_SE,
};

static const uint32 virginia_reel_track_pieces_flat_lift_hill[4] = {
    SPR_VIRGINIA_REEL_FLAT_LIFT_HILL_SW_NE,
    SPR_VIRGINIA_REEL_FLAT_LIFT_HILL_NW_SE,
    SPR_VIRGINIA_REEL_FLAT_LIFT_HILL_NE_SW,
    SPR_VIRGINIA_REEL_FLAT_LIFT_HILL_SE_NW,
};

static const uint32 virginia_reel_track_pieces_flat_to_25_deg_up[4] = {
    SPR_VIRGINIA_REEL_FLAT_TO_25_DEG_UP_SW_NE,
    SPR_VIRGINIA_REEL_FLAT_TO_25_DEG_UP_NW_SE,
    SPR_VIRGINIA_REEL_FLAT_TO_25_DEG_UP_NE_SW,
    SPR_VIRGINIA_REEL_FLAT_TO_25_DEG_UP_SE_NW,
};

static const uint32 virginia_reel_track_pieces_flat_to_25_deg_up_lift_hill[4] = {
    SPR_VIRGINIA_REEL_FLAT_TO_25_DEG_UP_LIFT_HILL_SW_NE,
    SPR_VIRGINIA_REEL_FLAT_TO_25_DEG_UP_LIFT_HILL_NW_SE,
    SPR_VIRGINIA_REEL_FLAT_TO_25_DEG_UP_LIFT_HILL_NE_SW,
    SPR_VIRGINIA_REEL_FLAT_TO_25_DEG_UP_LIFT_HILL_SE_NW,
};

static const uint32 virginia_reel_track_pieces_25_deg_up_to_flat[4] = {
    SPR_VIRGINIA_REEL_25_DEG_UP_TO_FLAT_SW_NE,
    SPR_VIRGINIA_REEL_25_DEG_UP_TO_FLAT_NW_SE,
    SPR_VIRGINIA_REEL_25_DEG_UP_TO_FLAT_NE_SW,
    SPR_VIRGINIA_REEL_25_DEG_UP_TO_FLAT_SE_NW,
};

static const uint32 virginia_reel_track_pieces_25_deg_up_to_flat_lift_hill[4] = {
    SPR_VIRGINIA_REEL_25_DEG_UP_TO_FLAT_LIFT_HILL_SW_NE,
    SPR_VIRGINIA_REEL_25_DEG_UP_TO_FLAT_LIFT_HILL_NW_SE,
    SPR_VIRGINIA_REEL_25_DEG_UP_TO_FLAT_LIFT_HILL_NE_SW,
    SPR_VIRGINIA_REEL_25_DEG_UP_TO_FLAT_LIFT_HILL_SE_NW,
};

static const uint32 virginia_reel_track_pieces_25_deg_up[4] = {
    SPR_VIRGINIA_REEL_25_DEG_UP_SW_NE,
    SPR_VIRGINIA_REEL_25_DEG_UP_NW_SE,
    SPR_VIRGINIA_REEL_25_DEG_UP_NE_SW,
    SPR_VIRGINIA_REEL_25_DEG_UP_SE_NW,
};

static const uint32 virginia_reel_track_pieces_25_deg_up_lift_hill[4] = {
    SPR_VIRGINIA_REEL_25_DEG_UP_LIFT_HILL_SW_NE,
    SPR_VIRGINIA_REEL_25_DEG_UP_LIFT_HILL_NW_SE,
    SPR_VIRGINIA_REEL_25_DEG_UP_LIFT_HILL_NE_SW,
    SPR_VIRGINIA_REEL_25_DEG_UP_LIFT_HILL_SE_NW,
};

static const uint32 virginia_reel_track_pieces_flat_quarter_turn_3_tiles[4][3] = {
    {
        SPR_VIRGINIA_REEL_FLAT_QUARTER_TURN_3_TILES_SW_SE_PART_0,
        SPR_VIRGINIA_REEL_FLAT_QUARTER_TURN_3_TILES_SW_SE_PART_1,
        SPR_VIRGINIA_REEL_FLAT_QUARTER_TURN_3_TILES_SW_SE_PART_2
    },
    {
        SPR_VIRGINIA_REEL_FLAT_QUARTER_TURN_3_TILES_NW_SW_PART_0,
        SPR_VIRGINIA_REEL_FLAT_QUARTER_TURN_3_TILES_NW_SW_PART_1,
        SPR_VIRGINIA_REEL_FLAT_QUARTER_TURN_3_TILES_NW_SW_PART_2
    },
    {
        SPR_VIRGINIA_REEL_FLAT_QUARTER_TURN_3_TILES_NE_NW_PART_0,
        SPR_VIRGINIA_REEL_FLAT_QUARTER_TURN_3_TILES_NE_NW_PART_1,
        SPR_VIRGINIA_REEL_FLAT_QUARTER_TURN_3_TILES_NE_NW_PART_2
    },
    {
        SPR_VIRGINIA_REEL_FLAT_QUARTER_TURN_3_TILES_SE_NE_PART_0,
        SPR_VIRGINIA_REEL_FLAT_QUARTER_TURN_3_TILES_SE_NE_PART_1,
        SPR_VIRGINIA_REEL_FLAT_QUARTER_TURN_3_TILES_SE_NE_PART_2
    }
};

static const uint32 virginia_reel_track_pieces_flat_quarter_turn_1_tile[4] = {
    SPR_VIRGINIA_REEL_FLAT_QUARTER_TURN_1_TILE_SW_NW,
    SPR_VIRGINIA_REEL_FLAT_QUARTER_TURN_1_TILE_NW_NE,
    SPR_VIRGINIA_REEL_FLAT_QUARTER_TURN_1_TILE_NE_SE,
    SPR_VIRGINIA_REEL_FLAT_QUARTER_TURN_1_TILE_SE_SW,
};

/**
 *
 *  rct2: 0x006D5B48
 */
void vehicle_visual_virginia_reel(paint_session * session, sint32 x, sint32 imageDirection, sint32 y, sint32 z, rct_vehicle *vehicle, const rct_ride_entry_vehicle *vehicleEntry)
{
    sint32 image_id;
    sint32 baseImage_id = imageDirection;
    const uint8 rotation = get_current_rotation();
    sint32 ecx = ((vehicle->var_BA / 8) + (rotation * 8)) & 31;
    sint32 j = 0;
    if (vehicle->vehicle_sprite_type == 0) {
        baseImage_id = ecx & 7;
    } else {
        if (vehicle->vehicle_sprite_type == 1 || vehicle->vehicle_sprite_type == 5) {
            if (vehicle->vehicle_sprite_type == 5){
                baseImage_id = imageDirection ^ 16;
            }
            baseImage_id &= 24;
            j = (baseImage_id / 8) + 1;
            baseImage_id += (ecx & 7);
            baseImage_id += 8;
        } else
        if (vehicle->vehicle_sprite_type == 2 || vehicle->vehicle_sprite_type == 6) {
            if (vehicle->vehicle_sprite_type == 6){
                baseImage_id = imageDirection ^ 16;
            }
            baseImage_id &= 24;
            j = (baseImage_id / 8) + 5;
            baseImage_id += (ecx & 7);
            baseImage_id += 40;
        } else {
            baseImage_id = ecx & 7;
        }
    }
    baseImage_id += vehicleEntry->base_image_id;

    const vehicle_boundbox *bb = &_virginiaReelBoundbox[j];
    image_id = baseImage_id | SPRITE_ID_PALETTE_COLOUR_2(vehicle->colours.body_colour, vehicle->colours.trim_colour);
    sub_98197C(image_id, 0, 0, bb->length_x, bb->length_y, bb->length_z, z, bb->offset_x, bb->offset_y, bb->offset_z + z, rotation);

    if (session->Unk140E9A8->zoom_level < 2 && vehicle->num_peeps > 0) {
        uint8 riding_peep_sprites[4] = {0xFF, 0xFF, 0xFF, 0xFF};
        for (sint32 i = 0; i < vehicle->num_peeps; i++) {
            riding_peep_sprites[((ecx / 8) + i) & 3] = vehicle->peep_tshirt_colours[i];
        }
        sint32 draw_order[4] = {0, 1, 3, 2};
        for (sint32 i = 0; i < countof(draw_order); i++) {
            if (riding_peep_sprites[draw_order[i]] != 0xFF) {
                image_id = (baseImage_id + ((draw_order[i] + 1) * 72)) | SPRITE_ID_PALETTE_COLOUR_1(riding_peep_sprites[draw_order[i]]);
                sub_98199C(image_id, 0, 0, bb->length_x, bb->length_y, bb->length_z, z, bb->offset_x, bb->offset_y, bb->offset_z + z, rotation);
            }
        }
    }

    assert(vehicleEntry->effect_visual == 1);
}

/** rct2: 0x00811264 */
static void paint_virginia_reel_track_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    const uint32 * sprites = virginia_reel_track_pieces_flat;
    if (mapElement->type & 0x80) {
        sprites = virginia_reel_track_pieces_flat_lift_hill;
    }

    uint32 imageId = sprites[direction] | gTrackColours[SCHEME_TRACK];
    if (direction & 1) {
        sub_98197C(imageId, 0, 0, 27, 32, 2, height, 2, 0, height, get_current_rotation());
        paint_util_push_tunnel_right(height, TUNNEL_6);
    } else {
        sub_98197C(imageId, 0, 0, 32, 27, 2, height, 0, 2, height, get_current_rotation());
        paint_util_push_tunnel_left(height, TUNNEL_6);
    }

    wooden_a_supports_paint_setup((direction & 1), 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);

    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 32, 0x20);
}

/** rct2: 0x00811274 */
static void paint_virginia_reel_track_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    const uint32 * sprites = virginia_reel_track_pieces_25_deg_up;
    if (mapElement->type & 0x80) {
        sprites = virginia_reel_track_pieces_25_deg_up_lift_hill;
    }

    uint32 imageId = sprites[direction] | gTrackColours[SCHEME_TRACK];
    paint_struct * ps;

    if (direction & 1) {
        ps = sub_98197C(imageId, 0, 0, 27, 32, 2, height, 2, 0, height, get_current_rotation());
    } else {
        ps = sub_98197C(imageId, 0, 0, 32, 27, 2, height, 0, 2, height, get_current_rotation());
    }

    if (direction == 1 || direction == 2) {
        gPaintSession.WoodenSupportsPrependTo = ps;
    }

    switch (direction) {
        case 0:
            wooden_a_supports_paint_setup(0, 9, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            paint_util_push_tunnel_left(height - 8, TUNNEL_7);
            break;
        case 1:
            wooden_a_supports_paint_setup(1, 10, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            paint_util_push_tunnel_right(height + 8, TUNNEL_8);
            break;
        case 2:
            wooden_a_supports_paint_setup(0, 11, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            paint_util_push_tunnel_left(height + 8, TUNNEL_8);
            break;
        case 3:
            wooden_a_supports_paint_setup(1, 12, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            paint_util_push_tunnel_right(height - 8, TUNNEL_7);
            break;
    }

    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 56, 0x20);
}

/** rct2: 0x00811294 */
static void paint_virginia_reel_track_flat_to_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    const uint32 * sprites = virginia_reel_track_pieces_flat_to_25_deg_up;
    if (mapElement->type & 0x80) {
        sprites = virginia_reel_track_pieces_flat_to_25_deg_up_lift_hill;
    }

    uint32 imageId = sprites[direction] | gTrackColours[SCHEME_TRACK];
    paint_struct * ps;
    switch (direction) {
        case 0:
            sub_98197C(imageId, 0, 0, 32, 27, 2, height, 0, 2, height, get_current_rotation());

            wooden_a_supports_paint_setup(0, 1, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            paint_util_push_tunnel_left(height, TUNNEL_6);
            break;
        case 1:
            ps = sub_98197C(imageId, 0, 0, 27, 32, 2, height, 2, 0, height, get_current_rotation());
            gPaintSession.WoodenSupportsPrependTo = ps;

            wooden_a_supports_paint_setup(1, 2, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            paint_util_push_tunnel_right(height, TUNNEL_8);
            break;
        case 2:
            ps = sub_98197C(imageId, 0, 0, 32, 27, 2, height, 0, 2, height, get_current_rotation());
            gPaintSession.WoodenSupportsPrependTo = ps;

            wooden_a_supports_paint_setup(0, 3, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            paint_util_push_tunnel_left(height, TUNNEL_8);
            break;
        case 3:
            sub_98197C(imageId, 0, 0, 27, 32, 2, height, 2, 0, height, get_current_rotation());

            wooden_a_supports_paint_setup(1, 4, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            paint_util_push_tunnel_right(height, TUNNEL_6);
            break;
    }

    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 48, 0x20);
}

/** rct2: 0x00811294 */
static void paint_virginia_reel_track_25_deg_up_to_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    const uint32 * sprites = virginia_reel_track_pieces_25_deg_up_to_flat;
    if (mapElement->type & 0x80) {
        sprites = virginia_reel_track_pieces_25_deg_up_to_flat_lift_hill;
    }

    uint32 imageId = sprites[direction] | gTrackColours[SCHEME_TRACK];
    paint_struct * ps;

    if (direction & 1) {
        ps = sub_98197C(imageId, 0, 0, 27, 32, 2, height, 2, 0, height, get_current_rotation());
    } else {
        ps = sub_98197C(imageId, 0, 0, 32, 27, 2, height, 0, 2, height, get_current_rotation());
    }

    if (direction == 1 || direction == 2) {
        gPaintSession.WoodenSupportsPrependTo = ps;
    }

    switch (direction) {
        case 0:
            wooden_a_supports_paint_setup(0, 5, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            paint_util_push_tunnel_left(height - 8, TUNNEL_6);
            break;
        case 1:
            wooden_a_supports_paint_setup(1, 6, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            paint_util_push_tunnel_right(height + 8, TUNNEL_14);
            break;
        case 2:
            wooden_a_supports_paint_setup(0, 7, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            paint_util_push_tunnel_left(height + 8, TUNNEL_14);
            break;
        case 3:
            wooden_a_supports_paint_setup(1, 8, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            paint_util_push_tunnel_right(height - 8, TUNNEL_6);
            break;
    }

    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 40, 0x20);
}

/** rct2: 0x008112A4 */
static void paint_virginia_reel_track_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    paint_virginia_reel_track_25_deg_up(session, rideIndex, trackSequence, (direction + 2) % 4, height, mapElement);
}

/** rct2: 0x008112B4 */
static void paint_virginia_reel_track_flat_to_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    paint_virginia_reel_track_25_deg_up_to_flat(session, rideIndex, trackSequence, (direction + 2) % 4, height, mapElement);
}

/** rct2: 0x008112C4 */
static void paint_virginia_reel_track_25_deg_down_to_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    paint_virginia_reel_track_flat_to_25_deg_up(session, rideIndex, trackSequence, (direction + 2) % 4, height, mapElement);
}

/** rct2: 0x008112D4, 0x008112E4, 0x008112F4 */
static void paint_virginia_reel_station(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    uint32 imageId;

    if (direction == 0 || direction == 2) {
        imageId = SPR_STATION_BASE_B_SW_NE | gTrackColours[SCHEME_MISC];
        sub_98197C(imageId, 0, 0, 32, 28, 2, height - 2, 0, 2, height, get_current_rotation());

        imageId = SPR_VIRGINIA_REEL_FLAT_SW_NE | gTrackColours[SCHEME_TRACK];
        sub_98199C(imageId, 0, 0, 32, 20, 2, height, 0, 0, height, get_current_rotation());

        paint_util_push_tunnel_left(height, TUNNEL_6);
    } else if (direction == 1 || direction == 3) {
        imageId = SPR_STATION_BASE_B_NW_SE | gTrackColours[SCHEME_MISC];
        sub_98197C(imageId, 0, 0, 28, 32, 2, height - 2, 2, 0, height, get_current_rotation());

        imageId = SPR_VIRGINIA_REEL_FLAT_NW_SE | gTrackColours[SCHEME_TRACK];
        sub_98199C(imageId, 0, 0, 20, 32, 2, height, 0, 0, height, get_current_rotation());

        paint_util_push_tunnel_right(height, TUNNEL_6);
    }

    wooden_a_supports_paint_setup((direction & 1), 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
    track_paint_util_draw_station(rideIndex, trackSequence, direction, height, mapElement);

    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 32, 0x20);
}

static const uint8 virginia_reel_left_quarter_turn_supports[] = {5, 2, 3, 4};

/** rct2: 0x00811304 */
static void paint_virginia_reel_track_left_quarter_turn_3_tiles(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    track_paint_util_left_quarter_turn_3_tiles_paint(2, height, direction, trackSequence, gTrackColours[SCHEME_TRACK], virginia_reel_track_pieces_flat_quarter_turn_3_tiles, get_current_rotation());
    track_paint_util_left_quarter_turn_3_tiles_tunnel(height, TUNNEL_6, direction, trackSequence);

    switch (trackSequence) {
        case 2:
            paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C8 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_B8, direction), 0xFFFF, 0);
            break;
        case 0:
        case 3:
            wooden_a_supports_paint_setup(virginia_reel_left_quarter_turn_supports[direction], 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
            break;
    }

    paint_util_set_general_support_height(height + 32, 0x20);
}

static const uint8 virginia_reel_right_quarter_turn_3_tiles_to_left_turn_map[] = {3, 1, 2, 0};

/** rct2: 0x00811314 */
static void paint_virginia_reel_track_right_quarter_turn_3_tiles(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    trackSequence = virginia_reel_right_quarter_turn_3_tiles_to_left_turn_map[trackSequence];
    paint_virginia_reel_track_left_quarter_turn_3_tiles(session, rideIndex, trackSequence, (direction + 3) % 4, height, mapElement);
}

/** rct2: 0x00811324 */
static void paint_virginia_reel_track_left_quarter_turn_1_tile(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    track_paint_util_left_quarter_turn_1_tile_paint(2, height, 0, direction, gTrackColours[SCHEME_TRACK], virginia_reel_track_pieces_flat_quarter_turn_1_tile, get_current_rotation());

    switch (direction) {
        case 0:
            wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            paint_util_push_tunnel_left(height, TUNNEL_6);
            break;
        case 1:
            wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            paint_util_push_tunnel_right(height, TUNNEL_6);
            break;
        case 3:
            wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            paint_util_push_tunnel_right(height, TUNNEL_6);
            paint_util_push_tunnel_left(height, TUNNEL_6);
            break;
    }

    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 32, 0x20);
}

/** rct2: 0x00811334 */
static void paint_virginia_reel_track_right_quarter_turn_1_tile(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    paint_virginia_reel_track_left_quarter_turn_1_tile(session, rideIndex, trackSequence, (direction + 3) % 4, height, mapElement);
}

/**
 * rct2: 0x00811184
 */
TRACK_PAINT_FUNCTION get_track_paint_function_virginia_reel(sint32 trackType, sint32 direction)
{
    switch (trackType) {
        case TRACK_ELEM_FLAT:
            return paint_virginia_reel_track_flat;

        case TRACK_ELEM_END_STATION:
        case TRACK_ELEM_BEGIN_STATION:
        case TRACK_ELEM_MIDDLE_STATION:
            return paint_virginia_reel_station;

        case TRACK_ELEM_25_DEG_UP:
            return paint_virginia_reel_track_25_deg_up;
        case TRACK_ELEM_FLAT_TO_25_DEG_UP:
            return paint_virginia_reel_track_flat_to_25_deg_up;
        case TRACK_ELEM_25_DEG_UP_TO_FLAT:
            return paint_virginia_reel_track_25_deg_up_to_flat;

        case TRACK_ELEM_25_DEG_DOWN:
            return paint_virginia_reel_track_25_deg_down;
        case TRACK_ELEM_FLAT_TO_25_DEG_DOWN:
            return paint_virginia_reel_track_flat_to_25_deg_down;
        case TRACK_ELEM_25_DEG_DOWN_TO_FLAT:
            return paint_virginia_reel_track_25_deg_down_to_flat;

        case TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES:
            return paint_virginia_reel_track_left_quarter_turn_3_tiles;
        case TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES:
            return paint_virginia_reel_track_right_quarter_turn_3_tiles;

        case TRACK_ELEM_LEFT_QUARTER_TURN_1_TILE:
            return paint_virginia_reel_track_left_quarter_turn_1_tile;
        case TRACK_ELEM_RIGHT_QUARTER_TURN_1_TILE:
            return paint_virginia_reel_track_right_quarter_turn_1_tile;
    }

    return NULL;
}
