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

#include "../../config/Config.h"
#include "../../interface/viewport.h"
#include "../../world/sprite.h"
#include "../../paint/paint.h"
#include "../vehicle_paint.h"
#include "../../game.h"
#include "../track_paint.h"
#include "../track.h"
#include "../../paint/supports.h"
#include "../../world/map.h"

#ifndef NO_VEHICLES
// 0x0099279E:
static const vehicle_boundbox _riverRapidsBoundbox[] = {
    { -13, -13,  1, 26, 26, 13 },
    { -13, -13,  1, 26, 26, 13 },
    { -13, -13,  1, 26, 26, 13 },
    { -13, -13,  1, 26, 26, 13 },
    { -13, -13,  1, 26, 26, 13 },
    { -13, -13,  1, 26, 26, 13 },
    { -13, -13,  1, 26, 26, 13 },
    { -13, -13,  1, 26, 26, 13 },
    { -13, -13,  1, 26, 26, 13 }
};
#endif

enum
{
    SPR_RIVER_RAPIDS_FLAT_SW_NE = 21132,
    SPR_RIVER_RAPIDS_FLAT_NW_SE = 21133,
    SPR_RIVER_RAPIDS_FLAT_NE_SW = 21134,
    SPR_RIVER_RAPIDS_FLAT_SE_NW = 21135,
    SPR_RIVER_RAPIDS_FLAT_FRONT_SW_NE = 21136,
    SPR_RIVER_RAPIDS_FLAT_FRONT_NW_SE = 21137,
    SPR_RIVER_RAPIDS_FLAT_FRONT_NE_SW = 21138,
    SPR_RIVER_RAPIDS_FLAT_FRONT_SE_NW = 21139,
    SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_UP_SW_NE = 21140,
    SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_UP_NW_SE = 21141,
    SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_UP_NE_SW = 21142,
    SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_UP_SE_NW = 21143,
    SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_UP_FRONT_SW_NE = 21144,
    SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_UP_FRONT_NW_SE = 21145,
    SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_UP_FRONT_NE_SW = 21146,
    SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_UP_FRONT_SE_NW = 21147,
    SPR_RIVER_RAPIDS_25_DEG_UP_TO_FLAT_SW_NE = 21148,
    SPR_RIVER_RAPIDS_25_DEG_UP_TO_FLAT_NW_SE = 21149,
    SPR_RIVER_RAPIDS_25_DEG_UP_TO_FLAT_NE_SW = 21150,
    SPR_RIVER_RAPIDS_25_DEG_UP_TO_FLAT_SE_NW = 21151,
    SPR_RIVER_RAPIDS_25_DEG_UP_TO_FLAT_FRONT_SW_NE = 21152,
    SPR_RIVER_RAPIDS_25_DEG_UP_TO_FLAT_FRONT_NW_SE = 21153,
    SPR_RIVER_RAPIDS_25_DEG_UP_TO_FLAT_FRONT_NE_SW = 21154,
    SPR_RIVER_RAPIDS_25_DEG_UP_TO_FLAT_FRONT_SE_NW = 21155,
    SPR_RIVER_RAPIDS_25_DEG_UP_SW_NE = 21156,
    SPR_RIVER_RAPIDS_25_DEG_UP_NW_SE = 21157,
    SPR_RIVER_RAPIDS_25_DEG_UP_NE_SW = 21158,
    SPR_RIVER_RAPIDS_25_DEG_UP_SE_NW = 21159,
    SPR_RIVER_RAPIDS_25_DEG_UP_FRONT_SW_NE = 21160,
    SPR_RIVER_RAPIDS_25_DEG_UP_FRONT_NW_SE = 21161,
    SPR_RIVER_RAPIDS_25_DEG_UP_FRONT_NE_SW = 21162,
    SPR_RIVER_RAPIDS_25_DEG_UP_FRONT_SE_NW = 21163,
    SPR_RIVER_RAPIDS_25_DEG_DOWN_TO_FLAT_SW_NE = 21164,
    SPR_RIVER_RAPIDS_25_DEG_DOWN_TO_FLAT_NW_SE = 21165,
    SPR_RIVER_RAPIDS_25_DEG_DOWN_TO_FLAT_NE_SW = 21166,
    SPR_RIVER_RAPIDS_25_DEG_DOWN_TO_FLAT_SE_NW = 21167,
    SPR_RIVER_RAPIDS_25_DEG_DOWN_TO_FLAT_FRONT_SW_NE = 21168,
    SPR_RIVER_RAPIDS_25_DEG_DOWN_TO_FLAT_FRONT_NW_SE = 21169,
    SPR_RIVER_RAPIDS_25_DEG_DOWN_TO_FLAT_FRONT_NE_SW = 21170,
    SPR_RIVER_RAPIDS_25_DEG_DOWN_TO_FLAT_FRONT_SE_NW = 21171,
    SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_DOWN_SW_NE = 21172,
    SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_DOWN_NW_SE = 21173,
    SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_DOWN_NE_SW = 21174,
    SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_DOWN_SE_NW = 21175,
    SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_DOWN_FRONT_SW_NE = 21176,
    SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_DOWN_FRONT_NW_SE = 21177,
    SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_DOWN_FRONT_NE_SW = 21178,
    SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_DOWN_FRONT_SE_NW = 21179,
    SPR_RIVER_RAPIDS_25_DEG_DOWN_SW_NE = 21180,
    SPR_RIVER_RAPIDS_25_DEG_DOWN_NW_SE = 21181,
    SPR_RIVER_RAPIDS_25_DEG_DOWN_NE_SW = 21182,
    SPR_RIVER_RAPIDS_25_DEG_DOWN_SE_NW = 21183,
    SPR_RIVER_RAPIDS_25_DEG_DOWN_FRONT_SW_NE = 21184,
    SPR_RIVER_RAPIDS_25_DEG_DOWN_FRONT_NW_SE = 21185,
    SPR_RIVER_RAPIDS_25_DEG_DOWN_FRONT_NE_SW = 21186,
    SPR_RIVER_RAPIDS_25_DEG_DOWN_FRONT_SE_NW = 21187,
    SPR_RIVER_RAPIDS_RIGHT_QUARTER_TURN_1_TILE_SW_SE = 21188,
    SPR_RIVER_RAPIDS_RIGHT_QUARTER_TURN_1_TILE_SE_NE = 21189,
    SPR_RIVER_RAPIDS_RIGHT_QUARTER_TURN_1_TILE_NE_NW = 21190,
    SPR_RIVER_RAPIDS_RIGHT_QUARTER_TURN_1_TILE_NW_SW = 21191,
    SPR_RIVER_RAPIDS_LEFT_QUARTER_TURN_1_TILE_SE_SW = 21192,
    SPR_RIVER_RAPIDS_LEFT_QUARTER_TURN_1_TILE_SW_NW = 21193,
    SPR_RIVER_RAPIDS_LEFT_QUARTER_TURN_1_TILE_NW_NE = 21194,
    SPR_RIVER_RAPIDS_LEFT_QUARTER_TURN_1_TILE_NE_SE = 21195,
    SPR_RIVER_RAPIDS_RIGHT_QUARTER_TURN_1_TILE_FRONT_SW_SE = 21196,
    SPR_RIVER_RAPIDS_RIGHT_QUARTER_TURN_1_TILE_FRONT_SE_NE = 21197,
    SPR_RIVER_RAPIDS_QUARTER_TURN_1_TILE_FRONT_LEFT_NW_NE = 21198,
    SPR_RIVER_RAPIDS_RIGHT_QUARTER_TURN_1_TILE_FRONT_NW_SW = 21199,
    SPR_RIVER_RAPIDS_LEFT_QUARTER_TURN_1_TILE_FRONT_SE_SW = 21200,
    SPR_RIVER_RAPIDS_LEFT_QUARTER_TURN_1_TILE_FRONT_SW_NW = 21201,
    SPR_RIVER_RAPIDS_QUARTER_TURN_1_TILE_FRONT_RIGHT_NW_NE = 21202,
    SPR_RIVER_RAPIDS_LEFT_QUARTER_TURN_1_TILE_FRONT_NE_SE = 21203,
    SPR_RIVER_RAPIDS_WATERFALL_SW_NE = 21204,
    SPR_RIVER_RAPIDS_WATERFALL_NW_SE = 21205,
    SPR_RIVER_RAPIDS_WATERFALL_NE_SW = 21206,
    SPR_RIVER_RAPIDS_WATERFALL_SE_NW = 21207,
    SPR_RIVER_RAPIDS_WATERFALL_FRONT_SW_NE = 21208,
    SPR_RIVER_RAPIDS_WATERFALL_FRONT_NW_SE = 21209,
    SPR_RIVER_RAPIDS_WATERFALL_FRONT_NE_SW = 21210,
    SPR_RIVER_RAPIDS_WATERFALL_FRONT_SE_NW = 21211,
    SPR_RIVER_RAPIDS_WATERFALL_BASE_NW_FRAME_0 = 21212,

    SPR_RIVER_RAPIDS_WATERFALL_BASE_NE_FRAME_0 = 21220,

    SPR_RIVER_RAPIDS_WATERFALL_SIDE_SE_FRAME_0 = 21228,

    SPR_RIVER_RAPIDS_WATERFALL_SIDE_SW_FRAME_0 = 21236,

    SPR_RIVER_RAPIDS_WATERFALL_TOP_NW_FRAME_0 = 21244,

    SPR_RIVER_RAPIDS_WATERFALL_TOP_NE_FRAME_0 = 21252,

    SPR_RIVER_RAPIDS_RAPIDS_SW_NE_FRAME_0 = 21260,

    SPR_RIVER_RAPIDS_RAPIDS_FRONT_SW_NE = 21268,
    SPR_RIVER_RAPIDS_RAPIDS_NW_SE_FRAME_0 = 21269,

    SPR_RIVER_RAPIDS_RAPIDS_FRONT_NW_SE = 21277,
    SPR_RIVER_RAPIDS_RAPIDS_WHIRLPOOL_FRAME_0 = 21278,
};

static const uint32 river_rapids_track_pieces_25_deg_up[][2] = {
    {SPR_RIVER_RAPIDS_25_DEG_UP_SW_NE, SPR_RIVER_RAPIDS_25_DEG_UP_FRONT_SW_NE},
    {SPR_RIVER_RAPIDS_25_DEG_UP_NW_SE, SPR_RIVER_RAPIDS_25_DEG_UP_FRONT_NW_SE},
    {SPR_RIVER_RAPIDS_25_DEG_UP_NE_SW, SPR_RIVER_RAPIDS_25_DEG_UP_FRONT_NE_SW},
    {SPR_RIVER_RAPIDS_25_DEG_UP_SE_NW, SPR_RIVER_RAPIDS_25_DEG_UP_FRONT_SE_NW},
};
static const uint32 river_rapids_track_pieces_flat_to_25_deg_up[][2] = {
    {SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_UP_SW_NE, SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_UP_FRONT_SW_NE},
    {SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_UP_NW_SE, SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_UP_FRONT_NW_SE},
    {SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_UP_NE_SW, SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_UP_FRONT_NE_SW},
    {SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_UP_SE_NW, SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_UP_FRONT_SE_NW},
};

static const uint32 river_rapids_track_pieces_25_deg_up_to_flat[][2] = {
    {SPR_RIVER_RAPIDS_25_DEG_UP_TO_FLAT_SW_NE, SPR_RIVER_RAPIDS_25_DEG_UP_TO_FLAT_FRONT_SW_NE},
    {SPR_RIVER_RAPIDS_25_DEG_UP_TO_FLAT_NW_SE, SPR_RIVER_RAPIDS_25_DEG_UP_TO_FLAT_FRONT_NW_SE},
    {SPR_RIVER_RAPIDS_25_DEG_UP_TO_FLAT_NE_SW, SPR_RIVER_RAPIDS_25_DEG_UP_TO_FLAT_FRONT_NE_SW},
    {SPR_RIVER_RAPIDS_25_DEG_UP_TO_FLAT_SE_NW, SPR_RIVER_RAPIDS_25_DEG_UP_TO_FLAT_FRONT_SE_NW},
};

static const uint32 river_rapids_track_pieces_25_deg_down[][2] = {
    {SPR_RIVER_RAPIDS_25_DEG_DOWN_SW_NE, SPR_RIVER_RAPIDS_25_DEG_DOWN_FRONT_SW_NE},
    {SPR_RIVER_RAPIDS_25_DEG_DOWN_NW_SE, SPR_RIVER_RAPIDS_25_DEG_DOWN_FRONT_NW_SE},
    {SPR_RIVER_RAPIDS_25_DEG_DOWN_NE_SW, SPR_RIVER_RAPIDS_25_DEG_DOWN_FRONT_NE_SW},
    {SPR_RIVER_RAPIDS_25_DEG_DOWN_SE_NW, SPR_RIVER_RAPIDS_25_DEG_DOWN_FRONT_SE_NW},
};

static const uint32 river_rapids_track_pieces_flat_to_25_deg_down[][2] = {
    {SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_DOWN_SW_NE, SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_DOWN_FRONT_SW_NE},
    {SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_DOWN_NW_SE, SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_DOWN_FRONT_NW_SE},
    {SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_DOWN_NE_SW, SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_DOWN_FRONT_NE_SW},
    {SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_DOWN_SE_NW, SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_DOWN_FRONT_SE_NW},
};

static const uint32 river_rapids_track_pieces_25_deg_down_to_flat[][2] = {
    {SPR_RIVER_RAPIDS_25_DEG_DOWN_TO_FLAT_SW_NE, SPR_RIVER_RAPIDS_25_DEG_DOWN_TO_FLAT_FRONT_SW_NE},
    {SPR_RIVER_RAPIDS_25_DEG_DOWN_TO_FLAT_NW_SE, SPR_RIVER_RAPIDS_25_DEG_DOWN_TO_FLAT_FRONT_NW_SE},
    {SPR_RIVER_RAPIDS_25_DEG_DOWN_TO_FLAT_NE_SW, SPR_RIVER_RAPIDS_25_DEG_DOWN_TO_FLAT_FRONT_NE_SW},
    {SPR_RIVER_RAPIDS_25_DEG_DOWN_TO_FLAT_SE_NW, SPR_RIVER_RAPIDS_25_DEG_DOWN_TO_FLAT_FRONT_SE_NW},
};

#ifndef NO_VEHICLES
/**
 *
 *  rct2: 0x006D5889
 */
void vehicle_visual_river_rapids(paint_session * session, sint32 x, sint32 imageDirection, sint32 y, sint32 z, rct_vehicle *vehicle, const rct_ride_entry_vehicle *vehicleEntry)
{
    sint32 image_id;
    sint32 baseImage_id = imageDirection;
    uint32 rotation = get_current_rotation();
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

    const vehicle_boundbox *bb = &_riverRapidsBoundbox[j];
    image_id = baseImage_id | SPRITE_ID_PALETTE_COLOUR_2(vehicle->colours.body_colour, vehicle->colours.trim_colour);
    sub_98197C(session, image_id, 0, 0, bb->length_x, bb->length_y, bb->length_z, z, bb->offset_x, bb->offset_y, bb->offset_z + z, rotation);

    if (session->Unk140E9A8->zoom_level < 2 && vehicle->num_peeps > 0) {
        // Draw peeps: (this particular vehicle doesn't sort them back to front like others so the back ones sometimes clip, but thats how the original does it...)
        sint32 peeps = ((ecx / 8) + 0) & 3;
        image_id = (baseImage_id + ((peeps + 1) * 72)) | SPRITE_ID_PALETTE_COLOUR_2(vehicle->peep_tshirt_colours[0], vehicle->peep_tshirt_colours[1]) ;
        sub_98199C(session, image_id, 0, 0, bb->length_x, bb->length_y, bb->length_z, z, bb->offset_x, bb->offset_y, bb->offset_z + z, rotation);
        if (vehicle->num_peeps > 2) {
            peeps = ((ecx / 8) + 2) & 3;
            image_id = (baseImage_id + ((peeps + 1) * 72)) | SPRITE_ID_PALETTE_COLOUR_2(vehicle->peep_tshirt_colours[2], vehicle->peep_tshirt_colours[3]);
            sub_98199C(session, image_id, 0, 0, bb->length_x, bb->length_y, bb->length_z, z, bb->offset_x, bb->offset_y, bb->offset_z + z, rotation);
        }
        if (vehicle->num_peeps > 4) {
            peeps = ((ecx / 8) + 1) & 3;
            image_id = (baseImage_id + ((peeps + 1) * 72)) | SPRITE_ID_PALETTE_COLOUR_2(vehicle->peep_tshirt_colours[4], vehicle->peep_tshirt_colours[5]);
            sub_98199C(session, image_id, 0, 0, bb->length_x, bb->length_y, bb->length_z, z, bb->offset_x, bb->offset_y, bb->offset_z + z, rotation);
        }
        if (vehicle->num_peeps > 6) {
            peeps = ((ecx / 8) + 3) & 3;
            image_id = (baseImage_id + ((peeps + 1) * 72)) | SPRITE_ID_PALETTE_COLOUR_2(vehicle->peep_tshirt_colours[6], vehicle->peep_tshirt_colours[7]);
            sub_98199C(session, image_id, 0, 0, bb->length_x, bb->length_y, bb->length_z, z, bb->offset_x, bb->offset_y, bb->offset_z + z, rotation);
        }
    }

    vehicle_visual_splash_effect(session, z, vehicle, vehicleEntry);
}
#endif

/** rct2: 0x00757650 */
static void paint_river_rapids_track_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    uint32 imageId;

    if (direction & 1) {
        imageId = (direction == 1 ? SPR_RIVER_RAPIDS_FLAT_NW_SE : SPR_RIVER_RAPIDS_FLAT_SE_NW) | gTrackColours[SCHEME_TRACK];
        sub_98197C(session, imageId, 0, 0, 24, 32, 11, height, 4, 0, height, get_current_rotation());

        imageId = (direction == 1 ? SPR_RIVER_RAPIDS_FLAT_FRONT_NW_SE : SPR_RIVER_RAPIDS_FLAT_FRONT_SE_NW) | gTrackColours[SCHEME_TRACK];
        sub_98197C(session, imageId, 0, 0, 1, 32, 3, height, 27, 0, height + 17, get_current_rotation());
    } else {
        imageId = (direction == 0 ? SPR_RIVER_RAPIDS_FLAT_SW_NE : SPR_RIVER_RAPIDS_FLAT_NE_SW) | gTrackColours[SCHEME_TRACK];
        sub_98197C(session, imageId, 0, 0, 32, 24, 11, height, 0, 4, height, get_current_rotation());

        imageId = (direction == 0 ? SPR_RIVER_RAPIDS_FLAT_FRONT_SW_NE : SPR_RIVER_RAPIDS_FLAT_FRONT_NE_SW) | gTrackColours[SCHEME_TRACK];
        sub_98197C(session, imageId, 0, 0, 32, 1, 3, height, 0, 27, height + 17, get_current_rotation());
    }

    wooden_a_supports_paint_setup(session, (direction & 1), 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);

    if (direction & 1) {
        paint_util_push_tunnel_right(session, height, TUNNEL_6);
    } else {
        paint_util_push_tunnel_left(session, height, TUNNEL_6);
    }

    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x007576C0 */
static void paint_river_rapids_station(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    rct_ride *ride = get_ride(rideIndex);

    paint_river_rapids_track_flat(session, rideIndex, trackSequence, direction, height, mapElement);
    track_paint_util_draw_station_platform(session, ride, direction, height, 12, mapElement);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

static void paint_river_rapids_track_25_deg(paint_session * session, uint8 direction, sint32 height, const uint32 sprites[4][2])
{
    uint32 imageId;
    paint_struct * ps;

    switch (direction) {
        case 0:
            imageId = sprites[direction][0] | gTrackColours[SCHEME_TRACK];
            sub_98197C(session, imageId, 0, 0, 32, 24, 4, height, 0, 4, height, get_current_rotation());

            imageId = sprites[direction][1] | gTrackColours[SCHEME_TRACK];
            sub_98197C(session, imageId, 0, 0, 32, 1, 34, height, 0, 27, height + 16, get_current_rotation());

            wooden_a_supports_paint_setup(session, 0, 9, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            paint_util_push_tunnel_left(session, height - 8, TUNNEL_7);
            break;

        case 1:
            imageId = sprites[direction][0] | gTrackColours[SCHEME_TRACK];
            ps = sub_98197C(session, imageId, 0, 0, 24, 32, 4, height, 4, 0, height, get_current_rotation());
            session->WoodenSupportsPrependTo = ps;

            imageId = sprites[direction][1] | gTrackColours[SCHEME_TRACK];
            sub_98197C(session, imageId, 0, 0, 1, 32, 34, height, 27, 0, height + 16, get_current_rotation());

            wooden_a_supports_paint_setup(session, 1, 10, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            paint_util_push_tunnel_right(session, height + 8, TUNNEL_8);
            break;

        case 2:
            imageId = sprites[direction][0] | gTrackColours[SCHEME_TRACK];
            ps = sub_98197C(session, imageId, 0, 0, 32, 24, 4, height, 0, 4, height, get_current_rotation());
            session->WoodenSupportsPrependTo = ps;

            imageId = sprites[direction][1] | gTrackColours[SCHEME_TRACK];
            sub_98197C(session, imageId, 0, 0, 32, 1, 34, height, 0, 27, height + 16, get_current_rotation());

            wooden_a_supports_paint_setup(session, 0, 11, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            paint_util_push_tunnel_left(session, height + 8, TUNNEL_8);
            break;

        case 3:
            imageId = sprites[direction][0] | gTrackColours[SCHEME_TRACK];
            sub_98197C(session, imageId, 0, 0, 24, 32, 4, height, 4, 0, height, get_current_rotation());

            imageId = sprites[direction][1] | gTrackColours[SCHEME_TRACK];
            sub_98197C(session, imageId, 0, 0, 1, 32, 34, height, 27, 0, height + 16, get_current_rotation());

            wooden_a_supports_paint_setup(session, 1, 12, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            paint_util_push_tunnel_right(session, height - 8, TUNNEL_7);
            break;
    }

    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 56, 0x20);
}

static void paint_river_rapids_track_25_deg_to_flat_a(paint_session * session, uint8 direction, sint32 height, const uint32 sprites[4][2])
{
    uint32 imageId;
    paint_struct * ps;

    switch (direction) {
        case 0:
            imageId = sprites[direction][0] | gTrackColours[SCHEME_TRACK];
            sub_98197C(session, imageId, 0, 0, 32, 24, 4, height, 0, 4, height, get_current_rotation());

            imageId = sprites[direction][1] | gTrackColours[SCHEME_TRACK];
            sub_98197C(session, imageId, 0, 0, 32, 1, 18, height, 0, 27, height + 16, get_current_rotation());

            wooden_a_supports_paint_setup(session, 0, 5, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            paint_util_push_tunnel_left(session, height - 8, TUNNEL_6);
            break;

        case 1:
            imageId = sprites[direction][0] | gTrackColours[SCHEME_TRACK];
            ps = sub_98197C(session, imageId, 0, 0, 24, 32, 4, height, 4, 0, height, get_current_rotation());
            session->WoodenSupportsPrependTo = ps;

            imageId = sprites[direction][1] | gTrackColours[SCHEME_TRACK];
            sub_98197C(session, imageId, 0, 0, 1, 32, 18, height, 27, 0, height + 16, get_current_rotation());

            wooden_a_supports_paint_setup(session, 1, 6, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            paint_util_push_tunnel_right(session, height + 8, TUNNEL_14);
            break;

        case 2:
            imageId = sprites[direction][0] | gTrackColours[SCHEME_TRACK];
            ps = sub_98197C(session, imageId, 0, 0, 32, 24, 4, height, 0, 4, height, get_current_rotation());
            session->WoodenSupportsPrependTo = ps;

            imageId = sprites[direction][1] | gTrackColours[SCHEME_TRACK];
            sub_98197C(session, imageId, 0, 0, 32, 1, 18, height, 0, 27, height + 16, get_current_rotation());

            wooden_a_supports_paint_setup(session, 0, 7, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            paint_util_push_tunnel_left(session, height + 8, TUNNEL_14);
            break;

        case 3:
            imageId = sprites[direction][0] | gTrackColours[SCHEME_TRACK];
            sub_98197C(session, imageId, 0, 0, 24, 32, 4, height, 4, 0, height, get_current_rotation());

            imageId = sprites[direction][1] | gTrackColours[SCHEME_TRACK];
            sub_98197C(session, imageId, 0, 0, 1, 32, 18, height, 27, 0, height + 16, get_current_rotation());

            wooden_a_supports_paint_setup(session, 1, 8, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            paint_util_push_tunnel_right(session, height - 8, TUNNEL_6);
            break;
    }

    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 40, 0x20);
}

static void paint_river_rapids_track_25_deg_to_flat_b(paint_session * session, uint8 direction, sint32 height, const uint32 sprites[4][2])
{
    uint32 imageId;
    paint_struct * ps;

    switch (direction) {
        case 0:
            imageId = sprites[direction][0] | gTrackColours[SCHEME_TRACK];
            sub_98197C(session, imageId, 0, 0, 32, 24, 11, height, 0, 4, height, get_current_rotation());

            imageId = sprites[direction][1] | gTrackColours[SCHEME_TRACK];
            sub_98197C(session, imageId, 0, 0, 32, 1, 26, height, 0, 27, height + 16, get_current_rotation());

            wooden_a_supports_paint_setup(session, 0, 1, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            paint_util_push_tunnel_left(session, height, TUNNEL_6);
            break;

        case 1:
            imageId = sprites[direction][0] | gTrackColours[SCHEME_TRACK];
            ps = sub_98197C(session, imageId, 0, 0, 24, 32, 11, height, 4, 0, height, get_current_rotation());
            session->WoodenSupportsPrependTo = ps;

            imageId = sprites[direction][1] | gTrackColours[SCHEME_TRACK];
            sub_98197C(session, imageId, 0, 0, 1, 32, 26, height, 27, 0, height + 16, get_current_rotation());

            wooden_a_supports_paint_setup(session, 1, 2, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            paint_util_push_tunnel_right(session, height, TUNNEL_8);
            break;

        case 2:
            imageId = sprites[direction][0] | gTrackColours[SCHEME_TRACK];
            ps = sub_98197C(session, imageId, 0, 0, 32, 24, 11, height, 0, 4, height, get_current_rotation());
            session->WoodenSupportsPrependTo = ps;

            imageId = sprites[direction][1] | gTrackColours[SCHEME_TRACK];
            sub_98197C(session, imageId, 0, 0, 32, 1, 26, height, 0, 27, height + 16, get_current_rotation());

            wooden_a_supports_paint_setup(session, 0, 3, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            paint_util_push_tunnel_left(session, height, TUNNEL_8);
            break;

        case 3:
            imageId = sprites[direction][0] | gTrackColours[SCHEME_TRACK];
            sub_98197C(session, imageId, 0, 0, 24, 32, 11, height, 4, 0, height, get_current_rotation());

            imageId = sprites[direction][1] | gTrackColours[SCHEME_TRACK];
            sub_98197C(session, imageId, 0, 0, 1, 32, 26, height, 27, 0, height + 16, get_current_rotation());

            wooden_a_supports_paint_setup(session, 1, 4, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            paint_util_push_tunnel_right(session, height, TUNNEL_6);
            break;
    }

    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 48, 0x20);
}

/** rct2: 0x00757660 */
static void paint_river_rapids_track_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    paint_river_rapids_track_25_deg(session, direction, height, river_rapids_track_pieces_25_deg_up);
}

/** rct2: 0x00757670 */
static void paint_river_rapids_track_flat_to_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    paint_river_rapids_track_25_deg_to_flat_b(session, direction, height, river_rapids_track_pieces_flat_to_25_deg_up);
}

/** rct2: 0x00757680 */
static void paint_river_rapids_track_25_deg_up_to_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    paint_river_rapids_track_25_deg_to_flat_a(session, direction, height, river_rapids_track_pieces_25_deg_up_to_flat);
}

/** rct2: 0x00757690 */
static void paint_river_rapids_track_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    paint_river_rapids_track_25_deg(session, (direction + 2) % 4, height, river_rapids_track_pieces_25_deg_down);
}

/** rct2: 0x007576A0 */
static void paint_river_rapids_track_flat_to_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    paint_river_rapids_track_25_deg_to_flat_a(session, (direction + 2) % 4, height, river_rapids_track_pieces_flat_to_25_deg_down);
}

/** rct2: 0x007576B0 */
static void paint_river_rapids_track_25_deg_down_to_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    paint_river_rapids_track_25_deg_to_flat_b(session, (direction + 2) % 4, height, river_rapids_track_pieces_25_deg_down_to_flat);
}

/** rct2: 0x007576F0 */
static void paint_river_rapids_track_left_quarter_turn_1_tile(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    uint32 imageId;
    switch (direction) {
        case 0:
            imageId = SPR_RIVER_RAPIDS_LEFT_QUARTER_TURN_1_TILE_SW_NW | gTrackColours[SCHEME_TRACK];
            sub_98197C(session, imageId, 0, 0, 28, 26, 11, height, 4, 2, height, get_current_rotation());

            imageId = SPR_RIVER_RAPIDS_LEFT_QUARTER_TURN_1_TILE_FRONT_SW_NW | gTrackColours[SCHEME_TRACK];
            sub_98197C(session, imageId, 0, 0, 2, 1, 7, height, 28, 27, height + 13, get_current_rotation());

            paint_util_push_tunnel_left(session, height, TUNNEL_6);
            break;

        case 1:
            imageId = SPR_RIVER_RAPIDS_LEFT_QUARTER_TURN_1_TILE_NW_NE | gTrackColours[SCHEME_TRACK];
            sub_98197C(session, imageId, 0, 0, 28, 28, 11, height, 0, 0, height, get_current_rotation());

            imageId = SPR_RIVER_RAPIDS_QUARTER_TURN_1_TILE_FRONT_LEFT_NW_NE | gTrackColours[SCHEME_TRACK];
            sub_98197C(session, imageId, 0, 0, 1, 30, 7, height, 27, 1, height + 13, get_current_rotation());

            imageId = SPR_RIVER_RAPIDS_QUARTER_TURN_1_TILE_FRONT_RIGHT_NW_NE | gTrackColours[SCHEME_TRACK];
            sub_98197C(session, imageId, 0, 0, 30, 1, 7, height, 1, 27, height + 13, get_current_rotation());
            break;

        case 2:
            imageId = SPR_RIVER_RAPIDS_LEFT_QUARTER_TURN_1_TILE_NE_SE | gTrackColours[SCHEME_TRACK];
            sub_98197C(session, imageId, 0, 0, 26, 28, 11, height, 2, 4, height, get_current_rotation());

            imageId = SPR_RIVER_RAPIDS_LEFT_QUARTER_TURN_1_TILE_FRONT_NE_SE | gTrackColours[SCHEME_TRACK];
            sub_98197C(session, imageId, 0, 0, 1, 2, 7, height, 27, 28, height + 13, get_current_rotation());

            paint_util_push_tunnel_right(session, height, TUNNEL_6);
            break;

        case 3:
            imageId = SPR_RIVER_RAPIDS_LEFT_QUARTER_TURN_1_TILE_SE_SW | gTrackColours[SCHEME_TRACK];
            sub_98197C(session, imageId, 0, 0, 28, 28, 11, height, 4, 4, height, get_current_rotation());

            imageId = SPR_RIVER_RAPIDS_LEFT_QUARTER_TURN_1_TILE_FRONT_SE_SW | gTrackColours[SCHEME_TRACK];
            sub_98197C(session, imageId, 0, 0, 1, 1, 7, height, 28, 28, height + 13, get_current_rotation());

            paint_util_push_tunnel_left(session, height, TUNNEL_6);
            paint_util_push_tunnel_right(session, height, TUNNEL_6);
            break;
    }

    wooden_a_supports_paint_setup(session, (direction & 1), 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x00757700 */
static void paint_river_rapids_track_right_quarter_turn_1_tile(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    uint32 imageId;

    switch (direction) {
        case 0:
            imageId = SPR_RIVER_RAPIDS_RIGHT_QUARTER_TURN_1_TILE_SW_SE | gTrackColours[SCHEME_TRACK];
            sub_98197C(session, imageId, 0, 0, 28, 28, 11, height, 4, 4, height, get_current_rotation());

            imageId = SPR_RIVER_RAPIDS_RIGHT_QUARTER_TURN_1_TILE_FRONT_SW_SE | gTrackColours[SCHEME_TRACK];
            sub_98197C(session, imageId, 0, 0, 1, 1, 7, height, 28, 28, height + 13, get_current_rotation());

            paint_util_push_tunnel_left(session, height, TUNNEL_6);
            paint_util_push_tunnel_right(session, height, TUNNEL_6);
            break;

        case 1:
            imageId = SPR_RIVER_RAPIDS_RIGHT_QUARTER_TURN_1_TILE_SE_NE | gTrackColours[SCHEME_TRACK];
            sub_98197C(session, imageId, 0, 0, 28, 26, 11, height, 4, 2, height, get_current_rotation());

            imageId = SPR_RIVER_RAPIDS_RIGHT_QUARTER_TURN_1_TILE_FRONT_SE_NE | gTrackColours[SCHEME_TRACK];
            sub_98197C(session, imageId, 0, 0, 2, 1, 7, height, 28, 27, height + 13, get_current_rotation());

            paint_util_push_tunnel_left(session, height, TUNNEL_6);
            break;

        case 2:
            imageId = SPR_RIVER_RAPIDS_RIGHT_QUARTER_TURN_1_TILE_NE_NW | gTrackColours[SCHEME_TRACK];
            sub_98197C(session, imageId, 0, 0, 28, 28, 11, height, 0, 0, height, get_current_rotation());

            imageId = SPR_RIVER_RAPIDS_QUARTER_TURN_1_TILE_FRONT_LEFT_NW_NE | gTrackColours[SCHEME_TRACK];
            sub_98197C(session, imageId, 0, 0, 1, 30, 7, height, 27, 1, height + 13, get_current_rotation());

            imageId = SPR_RIVER_RAPIDS_QUARTER_TURN_1_TILE_FRONT_RIGHT_NW_NE | gTrackColours[SCHEME_TRACK];
            sub_98197C(session, imageId, 0, 0, 30, 1, 7, height, 1, 27, height + 13, get_current_rotation());
            break;

        case 3:
            imageId = SPR_RIVER_RAPIDS_RIGHT_QUARTER_TURN_1_TILE_NW_SW | gTrackColours[SCHEME_TRACK];
            sub_98197C(session, imageId, 0, 0, 26, 28, 11, height, 2, 4, height, get_current_rotation());

            imageId = SPR_RIVER_RAPIDS_RIGHT_QUARTER_TURN_1_TILE_FRONT_NW_SW | gTrackColours[SCHEME_TRACK];
            sub_98197C(session, imageId, 0, 0, 1, 28, 7, height, 27, 2, height + 13, get_current_rotation());

            paint_util_push_tunnel_right(session, height, TUNNEL_6);
            break;
    }

    wooden_a_supports_paint_setup(session, 1 - (direction & 1), 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x00757710 */
static void paint_river_rapids_track_waterfall(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    uint32 imageId;

    uint16 frameNum = (gScenarioTicks / 2) & 7;

    if (direction & 1) {
        imageId = (direction == 1 ? SPR_RIVER_RAPIDS_WATERFALL_NW_SE : SPR_RIVER_RAPIDS_WATERFALL_SE_NW) | gTrackColours[SCHEME_TRACK];
        sub_98197C(session, imageId, 0, 0, 24, 32, 11, height, 4, 0, height, get_current_rotation());

        imageId = (SPR_RIVER_RAPIDS_WATERFALL_BASE_NE_FRAME_0 + frameNum) | gTrackColours[SCHEME_TRACK];
        sub_98199C(session, imageId, 0, 0, 24, 32, 11, height, 4, 0, height, get_current_rotation());

        imageId = (SPR_RIVER_RAPIDS_WATERFALL_TOP_NE_FRAME_0 + frameNum) | gTrackColours[SCHEME_TRACK];
        sub_98197C(session, imageId, 0, 0, 1, 32, 27, height, 4, 0, height + 17, get_current_rotation());

        imageId = (direction == 1 ? SPR_RIVER_RAPIDS_WATERFALL_FRONT_NW_SE : SPR_RIVER_RAPIDS_WATERFALL_FRONT_SE_NW) | gTrackColours[SCHEME_TRACK];
        sub_98197C(session, imageId, 0, 0, 1, 32, 27, height, 27, 0, height + 17, get_current_rotation());

        imageId = (SPR_RIVER_RAPIDS_WATERFALL_SIDE_SW_FRAME_0 + frameNum) | gTrackColours[SCHEME_TRACK];
        sub_98199C(session, imageId, 0, 0, 1, 32, 27, height, 27, 0, height + 17, get_current_rotation());
    } else {
        imageId = (direction == 0 ? SPR_RIVER_RAPIDS_WATERFALL_SW_NE : SPR_RIVER_RAPIDS_WATERFALL_NE_SW) | gTrackColours[SCHEME_TRACK];
        sub_98197C(session, imageId, 0, 0, 32, 24, 11, height, 0, 4, height, get_current_rotation());

        imageId = (SPR_RIVER_RAPIDS_WATERFALL_BASE_NW_FRAME_0 + frameNum) | gTrackColours[SCHEME_TRACK];
        sub_98199C(session, imageId, 0, 0, 32, 24, 11, height, 0, 4, height, get_current_rotation());

        imageId = (SPR_RIVER_RAPIDS_WATERFALL_TOP_NW_FRAME_0 + frameNum) | gTrackColours[SCHEME_TRACK];
        sub_98197C(session, imageId, 0, 0, 32, 1, 27, height, 0, 4, height + 17, get_current_rotation());

        imageId = (direction == 0 ? SPR_RIVER_RAPIDS_WATERFALL_FRONT_SW_NE : SPR_RIVER_RAPIDS_WATERFALL_FRONT_NE_SW) | gTrackColours[SCHEME_TRACK];
        sub_98197C(session, imageId, 0, 0, 32, 1, 27, height, 0, 27, height + 17, get_current_rotation());

        imageId = (SPR_RIVER_RAPIDS_WATERFALL_SIDE_SE_FRAME_0 + frameNum) | gTrackColours[SCHEME_TRACK];
        sub_98199C(session, imageId, 0, 0, 32, 1, 27, height, 0, 27, height + 17, get_current_rotation());
    }

    wooden_a_supports_paint_setup(session, (direction & 1), 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);

    if (direction & 1) {
        paint_util_push_tunnel_right(session, height, TUNNEL_9);
    } else {
        paint_util_push_tunnel_left(session, height, TUNNEL_9);
    }

    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 48, 0x20);
}

/** rct2: 0x00757720 */
static void paint_river_rapids_track_rapids(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    uint32 imageId;

    uint16 frameNum = (gScenarioTicks / 2) & 7;

    if (direction & 1) {
        imageId = (SPR_RIVER_RAPIDS_RAPIDS_NW_SE_FRAME_0 + frameNum) | gTrackColours[SCHEME_TRACK];
        sub_98197C(session, imageId, 0, 0, 24, 32, 11, height, 4, 0, height, get_current_rotation());

        imageId = SPR_RIVER_RAPIDS_RAPIDS_FRONT_NW_SE | gTrackColours[SCHEME_TRACK];
        sub_98197C(session, imageId, 0, 0, 1, 32, 11, height, 27, 0, height + 17, get_current_rotation());
    } else {
        imageId = (SPR_RIVER_RAPIDS_RAPIDS_SW_NE_FRAME_0 + frameNum) | gTrackColours[SCHEME_TRACK];
        sub_98197C(session, imageId, 0, 0, 32, 24, 11, height, 0, 4, height, get_current_rotation());

        imageId = SPR_RIVER_RAPIDS_RAPIDS_FRONT_SW_NE | gTrackColours[SCHEME_TRACK];
        sub_98197C(session, imageId, 0, 0, 32, 1, 11, height, 0, 27, height + 17, get_current_rotation());
    }

    wooden_a_supports_paint_setup(session, (direction & 1), 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);

    if (direction & 1) {
        paint_util_push_tunnel_right(session, height, TUNNEL_6);
    } else {
        paint_util_push_tunnel_left(session, height, TUNNEL_6);
    }

    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x00757740 */
static void paint_river_rapids_track_on_ride_photo(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    paint_river_rapids_track_flat(session, rideIndex, trackSequence, direction, height, mapElement);
    track_paint_util_onride_photo_paint(session, direction, height + 13, mapElement);
    paint_util_set_general_support_height(session, height + 48, 0x20);
}

/** rct2: 0x */
static void paint_river_rapids_track_whirlpool(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    uint32 imageId;

    uint8 frameNum = (gScenarioTicks / 4) % 16;

    if (direction & 1) {
        imageId = (direction == 1 ? SPR_RIVER_RAPIDS_FLAT_NW_SE : SPR_RIVER_RAPIDS_FLAT_SE_NW) | gTrackColours[SCHEME_TRACK];
        sub_98197C(session, imageId, 0, 0, 24, 32, 11, height, 4, 0, height, get_current_rotation());

        imageId = (SPR_RIVER_RAPIDS_RAPIDS_WHIRLPOOL_FRAME_0 + frameNum) | gTrackColours[SCHEME_TRACK];
        sub_98199C(session, imageId, 0, 0, 24, 32, 11, height, 4, 0, height, get_current_rotation());

        imageId = (direction == 1 ? SPR_RIVER_RAPIDS_FLAT_FRONT_NW_SE : SPR_RIVER_RAPIDS_FLAT_FRONT_SE_NW) | gTrackColours[SCHEME_TRACK];
        sub_98197C(session, imageId, 0, 0, 1, 32, 3, height, 27, 0, height + 17, get_current_rotation());
    } else {
        imageId = (direction == 0 ? SPR_RIVER_RAPIDS_FLAT_SW_NE : SPR_RIVER_RAPIDS_FLAT_NE_SW) | gTrackColours[SCHEME_TRACK];
        sub_98197C(session, imageId, 0, 0, 32, 24, 11, height, 0, 4, height, get_current_rotation());

        imageId = (SPR_RIVER_RAPIDS_RAPIDS_WHIRLPOOL_FRAME_0 + frameNum) | gTrackColours[SCHEME_TRACK];
        sub_98199C(session, imageId, 0, 0, 32, 24, 11, height, 0, 4, height, get_current_rotation());

        imageId = (direction == 0 ? SPR_RIVER_RAPIDS_FLAT_FRONT_SW_NE : SPR_RIVER_RAPIDS_FLAT_FRONT_NE_SW) | gTrackColours[SCHEME_TRACK];
        sub_98197C(session, imageId, 0, 0, 32, 1, 3, height, 0, 27, height + 17, get_current_rotation());
    }

    wooden_a_supports_paint_setup(session, (direction & 1), 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);

    if (direction & 1) {
        paint_util_push_tunnel_right(session, height, TUNNEL_6);
    } else {
        paint_util_push_tunnel_left(session, height, TUNNEL_6);
    }

    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/**
 * rct2: 0x0075745C
 **/
TRACK_PAINT_FUNCTION get_track_paint_function_river_rapids(sint32 trackType, sint32 direction)
{
    switch (trackType) {
        case TRACK_ELEM_FLAT:
            return paint_river_rapids_track_flat;

        case TRACK_ELEM_END_STATION:
        case TRACK_ELEM_BEGIN_STATION:
        case TRACK_ELEM_MIDDLE_STATION:
            return paint_river_rapids_station;

        case TRACK_ELEM_25_DEG_UP:
            return paint_river_rapids_track_25_deg_up;
        case TRACK_ELEM_FLAT_TO_25_DEG_UP:
            return paint_river_rapids_track_flat_to_25_deg_up;
        case TRACK_ELEM_25_DEG_UP_TO_FLAT:
            return paint_river_rapids_track_25_deg_up_to_flat;

        case TRACK_ELEM_25_DEG_DOWN:
            return paint_river_rapids_track_25_deg_down;
        case TRACK_ELEM_FLAT_TO_25_DEG_DOWN:
            return paint_river_rapids_track_flat_to_25_deg_down;
        case TRACK_ELEM_25_DEG_DOWN_TO_FLAT:
            return paint_river_rapids_track_25_deg_down_to_flat;

        case TRACK_ELEM_LEFT_QUARTER_TURN_1_TILE:
            return paint_river_rapids_track_left_quarter_turn_1_tile;
        case TRACK_ELEM_RIGHT_QUARTER_TURN_1_TILE:
            return paint_river_rapids_track_right_quarter_turn_1_tile;

        case TRACK_ELEM_WATERFALL:
            return paint_river_rapids_track_waterfall;

        case TRACK_ELEM_RAPIDS:
            return paint_river_rapids_track_rapids;

        case TRACK_ELEM_ON_RIDE_PHOTO:
            return paint_river_rapids_track_on_ride_photo;

        case TRACK_ELEM_WHIRLPOOL:
            return paint_river_rapids_track_whirlpool;
    }

    return NULL;
}
