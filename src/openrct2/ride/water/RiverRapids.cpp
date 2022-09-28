/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../Game.h"
#include "../../config/Config.h"
#include "../../interface/Viewport.h"
#include "../../paint/Paint.h"
#include "../../paint/Supports.h"
#include "../../scenario/Scenario.h"
#include "../../world/Map.h"
#include "../Track.h"
#include "../TrackPaint.h"
#include "../Vehicle.h"
#include "../VehiclePaint.h"

#ifndef NO_VEHICLES
// 0x0099279E:
static constexpr const vehicle_boundbox _riverRapidsBoundbox[] = {
    { -13, -13, 1, 26, 26, 13 }, { -13, -13, 1, 26, 26, 13 }, { -13, -13, 1, 26, 26, 13 },
    { -13, -13, 1, 26, 26, 13 }, { -13, -13, 1, 26, 26, 13 }, { -13, -13, 1, 26, 26, 13 },
    { -13, -13, 1, 26, 26, 13 }, { -13, -13, 1, 26, 26, 13 }, { -13, -13, 1, 26, 26, 13 },
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

static constexpr const uint32_t river_rapids_track_pieces_25_deg_up[][2] = {
    { SPR_RIVER_RAPIDS_25_DEG_UP_SW_NE, SPR_RIVER_RAPIDS_25_DEG_UP_FRONT_SW_NE },
    { SPR_RIVER_RAPIDS_25_DEG_UP_NW_SE, SPR_RIVER_RAPIDS_25_DEG_UP_FRONT_NW_SE },
    { SPR_RIVER_RAPIDS_25_DEG_UP_NE_SW, SPR_RIVER_RAPIDS_25_DEG_UP_FRONT_NE_SW },
    { SPR_RIVER_RAPIDS_25_DEG_UP_SE_NW, SPR_RIVER_RAPIDS_25_DEG_UP_FRONT_SE_NW },
};
static constexpr const uint32_t river_rapids_track_pieces_flat_to_25_deg_up[][2] = {
    { SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_UP_SW_NE, SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_UP_FRONT_SW_NE },
    { SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_UP_NW_SE, SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_UP_FRONT_NW_SE },
    { SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_UP_NE_SW, SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_UP_FRONT_NE_SW },
    { SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_UP_SE_NW, SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_UP_FRONT_SE_NW },
};

static constexpr const uint32_t river_rapids_track_pieces_25_deg_up_to_flat[][2] = {
    { SPR_RIVER_RAPIDS_25_DEG_UP_TO_FLAT_SW_NE, SPR_RIVER_RAPIDS_25_DEG_UP_TO_FLAT_FRONT_SW_NE },
    { SPR_RIVER_RAPIDS_25_DEG_UP_TO_FLAT_NW_SE, SPR_RIVER_RAPIDS_25_DEG_UP_TO_FLAT_FRONT_NW_SE },
    { SPR_RIVER_RAPIDS_25_DEG_UP_TO_FLAT_NE_SW, SPR_RIVER_RAPIDS_25_DEG_UP_TO_FLAT_FRONT_NE_SW },
    { SPR_RIVER_RAPIDS_25_DEG_UP_TO_FLAT_SE_NW, SPR_RIVER_RAPIDS_25_DEG_UP_TO_FLAT_FRONT_SE_NW },
};

static constexpr const uint32_t river_rapids_track_pieces_25_deg_down[][2] = {
    { SPR_RIVER_RAPIDS_25_DEG_DOWN_SW_NE, SPR_RIVER_RAPIDS_25_DEG_DOWN_FRONT_SW_NE },
    { SPR_RIVER_RAPIDS_25_DEG_DOWN_NW_SE, SPR_RIVER_RAPIDS_25_DEG_DOWN_FRONT_NW_SE },
    { SPR_RIVER_RAPIDS_25_DEG_DOWN_NE_SW, SPR_RIVER_RAPIDS_25_DEG_DOWN_FRONT_NE_SW },
    { SPR_RIVER_RAPIDS_25_DEG_DOWN_SE_NW, SPR_RIVER_RAPIDS_25_DEG_DOWN_FRONT_SE_NW },
};

static constexpr const uint32_t river_rapids_track_pieces_flat_to_25_deg_down[][2] = {
    { SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_DOWN_SW_NE, SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_DOWN_FRONT_SW_NE },
    { SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_DOWN_NW_SE, SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_DOWN_FRONT_NW_SE },
    { SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_DOWN_NE_SW, SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_DOWN_FRONT_NE_SW },
    { SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_DOWN_SE_NW, SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_DOWN_FRONT_SE_NW },
};

static constexpr const uint32_t river_rapids_track_pieces_25_deg_down_to_flat[][2] = {
    { SPR_RIVER_RAPIDS_25_DEG_DOWN_TO_FLAT_SW_NE, SPR_RIVER_RAPIDS_25_DEG_DOWN_TO_FLAT_FRONT_SW_NE },
    { SPR_RIVER_RAPIDS_25_DEG_DOWN_TO_FLAT_NW_SE, SPR_RIVER_RAPIDS_25_DEG_DOWN_TO_FLAT_FRONT_NW_SE },
    { SPR_RIVER_RAPIDS_25_DEG_DOWN_TO_FLAT_NE_SW, SPR_RIVER_RAPIDS_25_DEG_DOWN_TO_FLAT_FRONT_NE_SW },
    { SPR_RIVER_RAPIDS_25_DEG_DOWN_TO_FLAT_SE_NW, SPR_RIVER_RAPIDS_25_DEG_DOWN_TO_FLAT_FRONT_SE_NW },
};

#ifndef NO_VEHICLES
/**
 *
 *  rct2: 0x006D5889
 */
void vehicle_visual_river_rapids(
    paint_session& session, int32_t x, int32_t imageDirection, int32_t y, int32_t z, const Vehicle* vehicle,
    const CarEntry* carEntry)
{
    imageDirection = OpenRCT2::Entity::Yaw::YawTo32(imageDirection);

    int32_t image_id;
    int32_t baseImage_id = imageDirection;
    uint32_t rotation = session.CurrentRotation;
    int32_t ecx = ((vehicle->spin_sprite / 8) + (rotation * 8)) & 31;
    int32_t j = 0;
    if (vehicle->Pitch == 0)
    {
        baseImage_id = ecx & 7;
    }
    else
    {
        if (vehicle->Pitch == 1 || vehicle->Pitch == 5)
        {
            if (vehicle->Pitch == 5)
            {
                baseImage_id = imageDirection ^ 16;
            }
            baseImage_id &= 24;
            j = (baseImage_id / 8) + 1;
            baseImage_id += (ecx & 7);
            baseImage_id += 8;
        }
        else if (vehicle->Pitch == 2 || vehicle->Pitch == 6)
        {
            if (vehicle->Pitch == 6)
            {
                baseImage_id = imageDirection ^ 16;
            }
            baseImage_id &= 24;
            j = (baseImage_id / 8) + 5;
            baseImage_id += (ecx & 7);
            baseImage_id += 40;
        }
        else
        {
            baseImage_id = ecx & 7;
        }
    }
    baseImage_id += carEntry->base_image_id;

    const vehicle_boundbox* bb = &_riverRapidsBoundbox[j];
    image_id = baseImage_id | SPRITE_ID_PALETTE_COLOUR_2(vehicle->colours.Body, vehicle->colours.Trim);
    if (vehicle->IsGhost())
    {
        image_id &= 0x7FFFF;
        image_id |= CONSTRUCTION_MARKER;
    }
    PaintAddImageAsParent(
        session, image_id, { 0, 0, z }, { bb->length_x, bb->length_y, bb->length_z },
        { bb->offset_x, bb->offset_y, bb->offset_z + z });

    if (session.DPI.zoom_level < ZoomLevel{ 2 } && vehicle->num_peeps > 0 && !vehicle->IsGhost())
    {
        // Draw peeps: (this particular vehicle doesn't sort them back to front like others so the back ones sometimes clip, but
        // that's how the original does it...)
        int32_t peeps = ((ecx / 8) + 0) & 3;
        image_id = (baseImage_id + ((peeps + 1) * 72))
            | SPRITE_ID_PALETTE_COLOUR_2(vehicle->peep_tshirt_colours[0], vehicle->peep_tshirt_colours[1]);
        PaintAddImageAsChild(
            session, image_id, { 0, 0, z }, { bb->length_x, bb->length_y, bb->length_z },
            { bb->offset_x, bb->offset_y, bb->offset_z + z });
        if (vehicle->num_peeps > 2)
        {
            peeps = ((ecx / 8) + 2) & 3;
            image_id = (baseImage_id + ((peeps + 1) * 72))
                | SPRITE_ID_PALETTE_COLOUR_2(vehicle->peep_tshirt_colours[2], vehicle->peep_tshirt_colours[3]);
            PaintAddImageAsChild(
                session, image_id, { 0, 0, z }, { bb->length_x, bb->length_y, bb->length_z },
                { bb->offset_x, bb->offset_y, bb->offset_z + z });
        }
        if (vehicle->num_peeps > 4)
        {
            peeps = ((ecx / 8) + 1) & 3;
            image_id = (baseImage_id + ((peeps + 1) * 72))
                | SPRITE_ID_PALETTE_COLOUR_2(vehicle->peep_tshirt_colours[4], vehicle->peep_tshirt_colours[5]);
            PaintAddImageAsChild(
                session, image_id, { 0, 0, z }, { bb->length_x, bb->length_y, bb->length_z },
                { bb->offset_x, bb->offset_y, bb->offset_z + z });
        }
        if (vehicle->num_peeps > 6)
        {
            peeps = ((ecx / 8) + 3) & 3;
            image_id = (baseImage_id + ((peeps + 1) * 72))
                | SPRITE_ID_PALETTE_COLOUR_2(vehicle->peep_tshirt_colours[6], vehicle->peep_tshirt_colours[7]);
            PaintAddImageAsChild(
                session, image_id, { 0, 0, z }, { bb->length_x, bb->length_y, bb->length_z },
                { bb->offset_x, bb->offset_y, bb->offset_z + z });
        }
    }

    vehicle_visual_splash_effect(session, z, vehicle, carEntry);
}
#endif

/** rct2: 0x00757650 */
static void paint_river_rapids_track_flat(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    ImageId imageId;

    if (direction & 1)
    {
        imageId = session.TrackColours[SCHEME_TRACK].WithIndex(
            (direction == 1 ? SPR_RIVER_RAPIDS_FLAT_NW_SE : SPR_RIVER_RAPIDS_FLAT_SE_NW));
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 24, 32, 11 }, { 4, 0, height });

        imageId = session.TrackColours[SCHEME_TRACK].WithIndex(
            (direction == 1 ? SPR_RIVER_RAPIDS_FLAT_FRONT_NW_SE : SPR_RIVER_RAPIDS_FLAT_FRONT_SE_NW));
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 1, 32, 3 }, { 27, 0, height + 17 });
    }
    else
    {
        imageId = session.TrackColours[SCHEME_TRACK].WithIndex(
            (direction == 0 ? SPR_RIVER_RAPIDS_FLAT_SW_NE : SPR_RIVER_RAPIDS_FLAT_NE_SW));
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 32, 24, 11 }, { 0, 4, height });

        imageId = session.TrackColours[SCHEME_TRACK].WithIndex(
            (direction == 0 ? SPR_RIVER_RAPIDS_FLAT_FRONT_SW_NE : SPR_RIVER_RAPIDS_FLAT_FRONT_NE_SW));
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 32, 1, 3 }, { 0, 27, height + 17 });
    }

    wooden_a_supports_paint_setup(session, (direction & 1), 0, height, session.TrackColours[SCHEME_SUPPORTS]);

    if (direction & 1)
    {
        paint_util_push_tunnel_right(session, height, TUNNEL_SQUARE_FLAT);
    }
    else
    {
        paint_util_push_tunnel_left(session, height, TUNNEL_SQUARE_FLAT);
    }

    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x007576C0 */
static void paint_river_rapids_station(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    paint_river_rapids_track_flat(session, ride, trackSequence, direction, height, trackElement);
    track_paint_util_draw_narrow_station_platform(session, ride, direction, height, 12, trackElement);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

static void paint_river_rapids_track_25_deg(
    paint_session& session, uint8_t direction, int32_t height, const uint32_t sprites[4][2])
{
    ImageId imageId;
    paint_struct* ps;

    switch (direction)
    {
        case 0:
            imageId = session.TrackColours[SCHEME_TRACK].WithIndex(sprites[direction][0]);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 32, 24, 4 }, { 0, 4, height });

            imageId = session.TrackColours[SCHEME_TRACK].WithIndex(sprites[direction][1]);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 32, 1, 34 }, { 0, 27, height + 16 });

            wooden_a_supports_paint_setup(session, 0, 9, height, session.TrackColours[SCHEME_SUPPORTS]);
            paint_util_push_tunnel_left(session, height - 8, TUNNEL_SQUARE_7);
            break;

        case 1:
            imageId = session.TrackColours[SCHEME_TRACK].WithIndex(sprites[direction][0]);
            ps = PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 24, 32, 4 }, { 4, 0, height });
            session.WoodenSupportsPrependTo = ps;

            imageId = session.TrackColours[SCHEME_TRACK].WithIndex(sprites[direction][1]);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 1, 32, 34 }, { 27, 0, height + 16 });

            wooden_a_supports_paint_setup(session, 1, 10, height, session.TrackColours[SCHEME_SUPPORTS]);
            paint_util_push_tunnel_right(session, height + 8, TUNNEL_SQUARE_8);
            break;

        case 2:
            imageId = session.TrackColours[SCHEME_TRACK].WithIndex(sprites[direction][0]);
            ps = PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 32, 24, 4 }, { 0, 4, height });
            session.WoodenSupportsPrependTo = ps;

            imageId = session.TrackColours[SCHEME_TRACK].WithIndex(sprites[direction][1]);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 32, 1, 34 }, { 0, 27, height + 16 });

            wooden_a_supports_paint_setup(session, 0, 11, height, session.TrackColours[SCHEME_SUPPORTS]);
            paint_util_push_tunnel_left(session, height + 8, TUNNEL_SQUARE_8);
            break;

        case 3:
            imageId = session.TrackColours[SCHEME_TRACK].WithIndex(sprites[direction][0]);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 24, 32, 4 }, { 4, 0, height });

            imageId = session.TrackColours[SCHEME_TRACK].WithIndex(sprites[direction][1]);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 1, 32, 34 }, { 27, 0, height + 16 });

            wooden_a_supports_paint_setup(session, 1, 12, height, session.TrackColours[SCHEME_SUPPORTS]);
            paint_util_push_tunnel_right(session, height - 8, TUNNEL_SQUARE_7);
            break;
    }

    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 56, 0x20);
}

static void paint_river_rapids_track_25_deg_to_flat_a(
    paint_session& session, uint8_t direction, int32_t height, const uint32_t sprites[4][2])
{
    ImageId imageId;
    paint_struct* ps;

    switch (direction)
    {
        case 0:
            imageId = session.TrackColours[SCHEME_TRACK].WithIndex(sprites[direction][0]);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 32, 24, 4 }, { 0, 4, height });

            imageId = session.TrackColours[SCHEME_TRACK].WithIndex(sprites[direction][1]);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 32, 1, 18 }, { 0, 27, height + 16 });

            wooden_a_supports_paint_setup(session, 0, 5, height, session.TrackColours[SCHEME_SUPPORTS]);
            paint_util_push_tunnel_left(session, height - 8, TUNNEL_SQUARE_FLAT);
            break;

        case 1:
            imageId = session.TrackColours[SCHEME_TRACK].WithIndex(sprites[direction][0]);
            ps = PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 24, 32, 4 }, { 4, 0, height });
            session.WoodenSupportsPrependTo = ps;

            imageId = session.TrackColours[SCHEME_TRACK].WithIndex(sprites[direction][1]);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 1, 32, 18 }, { 27, 0, height + 16 });

            wooden_a_supports_paint_setup(session, 1, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
            paint_util_push_tunnel_right(session, height + 8, TUNNEL_14);
            break;

        case 2:
            imageId = session.TrackColours[SCHEME_TRACK].WithIndex(sprites[direction][0]);
            ps = PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 32, 24, 4 }, { 0, 4, height });
            session.WoodenSupportsPrependTo = ps;

            imageId = session.TrackColours[SCHEME_TRACK].WithIndex(sprites[direction][1]);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 32, 1, 18 }, { 0, 27, height + 16 });

            wooden_a_supports_paint_setup(session, 0, 7, height, session.TrackColours[SCHEME_SUPPORTS]);
            paint_util_push_tunnel_left(session, height + 8, TUNNEL_14);
            break;

        case 3:
            imageId = session.TrackColours[SCHEME_TRACK].WithIndex(sprites[direction][0]);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 24, 32, 4 }, { 4, 0, height });

            imageId = session.TrackColours[SCHEME_TRACK].WithIndex(sprites[direction][1]);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 1, 32, 18 }, { 27, 0, height + 16 });

            wooden_a_supports_paint_setup(session, 1, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
            paint_util_push_tunnel_right(session, height - 8, TUNNEL_SQUARE_FLAT);
            break;
    }

    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 40, 0x20);
}

static void paint_river_rapids_track_25_deg_to_flat_b(
    paint_session& session, uint8_t direction, int32_t height, const uint32_t sprites[4][2])
{
    ImageId imageId;
    paint_struct* ps;

    switch (direction)
    {
        case 0:
            imageId = session.TrackColours[SCHEME_TRACK].WithIndex(sprites[direction][0]);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 32, 24, 11 }, { 0, 4, height });

            imageId = session.TrackColours[SCHEME_TRACK].WithIndex(sprites[direction][1]);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 32, 1, 26 }, { 0, 27, height + 16 });

            wooden_a_supports_paint_setup(session, 0, 1, height, session.TrackColours[SCHEME_SUPPORTS]);
            paint_util_push_tunnel_left(session, height, TUNNEL_SQUARE_FLAT);
            break;

        case 1:
            imageId = session.TrackColours[SCHEME_TRACK].WithIndex(sprites[direction][0]);
            ps = PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 24, 32, 11 }, { 4, 0, height });
            session.WoodenSupportsPrependTo = ps;

            imageId = session.TrackColours[SCHEME_TRACK].WithIndex(sprites[direction][1]);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 1, 32, 26 }, { 27, 0, height + 16 });

            wooden_a_supports_paint_setup(session, 1, 2, height, session.TrackColours[SCHEME_SUPPORTS]);
            paint_util_push_tunnel_right(session, height, TUNNEL_SQUARE_8);
            break;

        case 2:
            imageId = session.TrackColours[SCHEME_TRACK].WithIndex(sprites[direction][0]);
            ps = PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 32, 24, 11 }, { 0, 4, height });
            session.WoodenSupportsPrependTo = ps;

            imageId = session.TrackColours[SCHEME_TRACK].WithIndex(sprites[direction][1]);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 32, 1, 26 }, { 0, 27, height + 16 });

            wooden_a_supports_paint_setup(session, 0, 3, height, session.TrackColours[SCHEME_SUPPORTS]);
            paint_util_push_tunnel_left(session, height, TUNNEL_SQUARE_8);
            break;

        case 3:
            imageId = session.TrackColours[SCHEME_TRACK].WithIndex(sprites[direction][0]);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 24, 32, 11 }, { 4, 0, height });

            imageId = session.TrackColours[SCHEME_TRACK].WithIndex(sprites[direction][1]);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 1, 32, 26 }, { 27, 0, height + 16 });

            wooden_a_supports_paint_setup(session, 1, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
            paint_util_push_tunnel_right(session, height, TUNNEL_SQUARE_FLAT);
            break;
    }

    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 48, 0x20);
}

/** rct2: 0x00757660 */
static void paint_river_rapids_track_25_deg_up(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    paint_river_rapids_track_25_deg(session, direction, height, river_rapids_track_pieces_25_deg_up);
}

/** rct2: 0x00757670 */
static void paint_river_rapids_track_flat_to_25_deg_up(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    paint_river_rapids_track_25_deg_to_flat_b(session, direction, height, river_rapids_track_pieces_flat_to_25_deg_up);
}

/** rct2: 0x00757680 */
static void paint_river_rapids_track_25_deg_up_to_flat(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    paint_river_rapids_track_25_deg_to_flat_a(session, direction, height, river_rapids_track_pieces_25_deg_up_to_flat);
}

/** rct2: 0x00757690 */
static void paint_river_rapids_track_25_deg_down(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    paint_river_rapids_track_25_deg(session, (direction + 2) % 4, height, river_rapids_track_pieces_25_deg_down);
}

/** rct2: 0x007576A0 */
static void paint_river_rapids_track_flat_to_25_deg_down(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    paint_river_rapids_track_25_deg_to_flat_a(
        session, (direction + 2) % 4, height, river_rapids_track_pieces_flat_to_25_deg_down);
}

/** rct2: 0x007576B0 */
static void paint_river_rapids_track_25_deg_down_to_flat(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    paint_river_rapids_track_25_deg_to_flat_b(
        session, (direction + 2) % 4, height, river_rapids_track_pieces_25_deg_down_to_flat);
}

/** rct2: 0x007576F0 */
static void paint_river_rapids_track_left_quarter_turn_1_tile(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    ImageId imageId;
    switch (direction)
    {
        case 0:
            imageId = session.TrackColours[SCHEME_TRACK].WithIndex(SPR_RIVER_RAPIDS_LEFT_QUARTER_TURN_1_TILE_SW_NW);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 28, 26, 11 }, { 4, 2, height });

            imageId = session.TrackColours[SCHEME_TRACK].WithIndex(SPR_RIVER_RAPIDS_LEFT_QUARTER_TURN_1_TILE_FRONT_SW_NW);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 2, 1, 7 }, { 28, 27, height + 13 });

            paint_util_push_tunnel_left(session, height, TUNNEL_SQUARE_FLAT);
            break;

        case 1:
            imageId = session.TrackColours[SCHEME_TRACK].WithIndex(SPR_RIVER_RAPIDS_LEFT_QUARTER_TURN_1_TILE_NW_NE);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 28, 28, 11 }, { 0, 0, height });

            imageId = session.TrackColours[SCHEME_TRACK].WithIndex(SPR_RIVER_RAPIDS_QUARTER_TURN_1_TILE_FRONT_LEFT_NW_NE);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 1, 30, 7 }, { 27, 1, height + 13 });

            imageId = session.TrackColours[SCHEME_TRACK].WithIndex(SPR_RIVER_RAPIDS_QUARTER_TURN_1_TILE_FRONT_RIGHT_NW_NE);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 30, 1, 7 }, { 1, 27, height + 13 });
            break;

        case 2:
            imageId = session.TrackColours[SCHEME_TRACK].WithIndex(SPR_RIVER_RAPIDS_LEFT_QUARTER_TURN_1_TILE_NE_SE);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 26, 28, 11 }, { 2, 4, height });

            imageId = session.TrackColours[SCHEME_TRACK].WithIndex(SPR_RIVER_RAPIDS_LEFT_QUARTER_TURN_1_TILE_FRONT_NE_SE);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 1, 2, 7 }, { 27, 28, height + 13 });

            paint_util_push_tunnel_right(session, height, TUNNEL_SQUARE_FLAT);
            break;

        case 3:
            imageId = session.TrackColours[SCHEME_TRACK].WithIndex(SPR_RIVER_RAPIDS_LEFT_QUARTER_TURN_1_TILE_SE_SW);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 28, 28, 11 }, { 4, 4, height });

            imageId = session.TrackColours[SCHEME_TRACK].WithIndex(SPR_RIVER_RAPIDS_LEFT_QUARTER_TURN_1_TILE_FRONT_SE_SW);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 1, 1, 7 }, { 28, 28, height + 13 });

            paint_util_push_tunnel_left(session, height, TUNNEL_SQUARE_FLAT);
            paint_util_push_tunnel_right(session, height, TUNNEL_SQUARE_FLAT);
            break;
    }

    wooden_a_supports_paint_setup(session, (direction & 1), 0, height, session.TrackColours[SCHEME_SUPPORTS]);
    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x00757700 */
static void paint_river_rapids_track_right_quarter_turn_1_tile(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    ImageId imageId;

    switch (direction)
    {
        case 0:
            imageId = session.TrackColours[SCHEME_TRACK].WithIndex(SPR_RIVER_RAPIDS_RIGHT_QUARTER_TURN_1_TILE_SW_SE);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 28, 28, 11 }, { 4, 4, height });

            imageId = session.TrackColours[SCHEME_TRACK].WithIndex(SPR_RIVER_RAPIDS_RIGHT_QUARTER_TURN_1_TILE_FRONT_SW_SE);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 1, 1, 7 }, { 28, 28, height + 13 });

            paint_util_push_tunnel_left(session, height, TUNNEL_SQUARE_FLAT);
            paint_util_push_tunnel_right(session, height, TUNNEL_SQUARE_FLAT);
            break;

        case 1:
            imageId = session.TrackColours[SCHEME_TRACK].WithIndex(SPR_RIVER_RAPIDS_RIGHT_QUARTER_TURN_1_TILE_SE_NE);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 28, 26, 11 }, { 4, 2, height });

            imageId = session.TrackColours[SCHEME_TRACK].WithIndex(SPR_RIVER_RAPIDS_RIGHT_QUARTER_TURN_1_TILE_FRONT_SE_NE);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 2, 1, 7 }, { 28, 27, height + 13 });

            paint_util_push_tunnel_left(session, height, TUNNEL_SQUARE_FLAT);
            break;

        case 2:
            imageId = session.TrackColours[SCHEME_TRACK].WithIndex(SPR_RIVER_RAPIDS_RIGHT_QUARTER_TURN_1_TILE_NE_NW);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 28, 28, 11 }, { 0, 0, height });

            imageId = session.TrackColours[SCHEME_TRACK].WithIndex(SPR_RIVER_RAPIDS_QUARTER_TURN_1_TILE_FRONT_LEFT_NW_NE);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 1, 30, 7 }, { 27, 1, height + 13 });

            imageId = session.TrackColours[SCHEME_TRACK].WithIndex(SPR_RIVER_RAPIDS_QUARTER_TURN_1_TILE_FRONT_RIGHT_NW_NE);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 30, 1, 7 }, { 1, 27, height + 13 });
            break;

        case 3:
            imageId = session.TrackColours[SCHEME_TRACK].WithIndex(SPR_RIVER_RAPIDS_RIGHT_QUARTER_TURN_1_TILE_NW_SW);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 26, 28, 11 }, { 2, 4, height });

            imageId = session.TrackColours[SCHEME_TRACK].WithIndex(SPR_RIVER_RAPIDS_RIGHT_QUARTER_TURN_1_TILE_FRONT_NW_SW);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 1, 28, 7 }, { 27, 2, height + 13 });

            paint_util_push_tunnel_right(session, height, TUNNEL_SQUARE_FLAT);
            break;
    }

    wooden_a_supports_paint_setup(session, 1 - (direction & 1), 0, height, session.TrackColours[SCHEME_SUPPORTS]);
    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x00757710 */
static void paint_river_rapids_track_waterfall(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    ImageId imageId;

    uint16_t frameNum = (gCurrentTicks / 2) & 7;

    if (direction & 1)
    {
        imageId = session.TrackColours[SCHEME_TRACK].WithIndex(
            (direction == 1 ? SPR_RIVER_RAPIDS_WATERFALL_NW_SE : SPR_RIVER_RAPIDS_WATERFALL_SE_NW));
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 24, 32, 11 }, { 4, 0, height });

        imageId = session.TrackColours[SCHEME_TRACK].WithIndex((SPR_RIVER_RAPIDS_WATERFALL_BASE_NE_FRAME_0 + frameNum));
        PaintAddImageAsChild(session, imageId, { 0, 0, height }, { 24, 32, 11 }, { 4, 0, height });

        imageId = session.TrackColours[SCHEME_TRACK].WithIndex((SPR_RIVER_RAPIDS_WATERFALL_TOP_NE_FRAME_0 + frameNum));
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 1, 32, 27 }, { 4, 0, height + 17 });

        imageId = session.TrackColours[SCHEME_TRACK].WithIndex(
            (direction == 1 ? SPR_RIVER_RAPIDS_WATERFALL_FRONT_NW_SE : SPR_RIVER_RAPIDS_WATERFALL_FRONT_SE_NW));
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 1, 32, 27 }, { 27, 0, height + 17 });

        imageId = session.TrackColours[SCHEME_TRACK].WithIndex((SPR_RIVER_RAPIDS_WATERFALL_SIDE_SW_FRAME_0 + frameNum));
        PaintAddImageAsChild(session, imageId, { 0, 0, height }, { 1, 32, 27 }, { 27, 0, height + 17 });
    }
    else
    {
        imageId = session.TrackColours[SCHEME_TRACK].WithIndex(
            (direction == 0 ? SPR_RIVER_RAPIDS_WATERFALL_SW_NE : SPR_RIVER_RAPIDS_WATERFALL_NE_SW));
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 32, 24, 11 }, { 0, 4, height });

        imageId = session.TrackColours[SCHEME_TRACK].WithIndex((SPR_RIVER_RAPIDS_WATERFALL_BASE_NW_FRAME_0 + frameNum));
        PaintAddImageAsChild(session, imageId, { 0, 0, height }, { 32, 24, 11 }, { 0, 4, height });

        imageId = session.TrackColours[SCHEME_TRACK].WithIndex((SPR_RIVER_RAPIDS_WATERFALL_TOP_NW_FRAME_0 + frameNum));
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 32, 1, 27 }, { 0, 4, height + 17 });

        imageId = session.TrackColours[SCHEME_TRACK].WithIndex(
            (direction == 0 ? SPR_RIVER_RAPIDS_WATERFALL_FRONT_SW_NE : SPR_RIVER_RAPIDS_WATERFALL_FRONT_NE_SW));
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 32, 1, 27 }, { 0, 27, height + 17 });

        imageId = session.TrackColours[SCHEME_TRACK].WithIndex((SPR_RIVER_RAPIDS_WATERFALL_SIDE_SE_FRAME_0 + frameNum));
        PaintAddImageAsChild(session, imageId, { 0, 0, height }, { 32, 1, 27 }, { 0, 27, height + 17 });
    }

    wooden_a_supports_paint_setup(session, (direction & 1), 0, height, session.TrackColours[SCHEME_SUPPORTS]);

    if (direction & 1)
    {
        paint_util_push_tunnel_right(session, height, TUNNEL_SQUARE_INVERTED_9);
    }
    else
    {
        paint_util_push_tunnel_left(session, height, TUNNEL_SQUARE_INVERTED_9);
    }

    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 48, 0x20);
}

/** rct2: 0x00757720 */
static void paint_river_rapids_track_rapids(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    ImageId imageId;

    uint16_t frameNum = (gCurrentTicks / 2) & 7;

    if (direction & 1)
    {
        imageId = session.TrackColours[SCHEME_TRACK].WithIndex((SPR_RIVER_RAPIDS_RAPIDS_NW_SE_FRAME_0 + frameNum));
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 24, 32, 11 }, { 4, 0, height });

        imageId = session.TrackColours[SCHEME_TRACK].WithIndex(SPR_RIVER_RAPIDS_RAPIDS_FRONT_NW_SE);
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 1, 32, 11 }, { 27, 0, height + 17 });
    }
    else
    {
        imageId = session.TrackColours[SCHEME_TRACK].WithIndex((SPR_RIVER_RAPIDS_RAPIDS_SW_NE_FRAME_0 + frameNum));
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 32, 24, 11 }, { 0, 4, height });

        imageId = session.TrackColours[SCHEME_TRACK].WithIndex(SPR_RIVER_RAPIDS_RAPIDS_FRONT_SW_NE);
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 32, 1, 11 }, { 0, 27, height + 17 });
    }

    wooden_a_supports_paint_setup(session, (direction & 1), 0, height, session.TrackColours[SCHEME_SUPPORTS]);

    if (direction & 1)
    {
        paint_util_push_tunnel_right(session, height, TUNNEL_SQUARE_FLAT);
    }
    else
    {
        paint_util_push_tunnel_left(session, height, TUNNEL_SQUARE_FLAT);
    }

    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x00757740 */
static void paint_river_rapids_track_on_ride_photo(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    paint_river_rapids_track_flat(session, ride, trackSequence, direction, height, trackElement);
    track_paint_util_onride_photo_paint(session, direction, height + 13, trackElement);
    paint_util_set_general_support_height(session, height + 48, 0x20);
}

/** rct2: 0x */
static void paint_river_rapids_track_whirlpool(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    ImageId imageId;

    uint8_t frameNum = (gCurrentTicks / 4) % 16;

    if (direction & 1)
    {
        imageId = session.TrackColours[SCHEME_TRACK].WithIndex(
            (direction == 1 ? SPR_RIVER_RAPIDS_FLAT_NW_SE : SPR_RIVER_RAPIDS_FLAT_SE_NW));
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 24, 32, 11 }, { 4, 0, height });

        imageId = session.TrackColours[SCHEME_TRACK].WithIndex((SPR_RIVER_RAPIDS_RAPIDS_WHIRLPOOL_FRAME_0 + frameNum));
        PaintAddImageAsChild(session, imageId, { 0, 0, height }, { 24, 32, 11 }, { 4, 0, height });

        imageId = session.TrackColours[SCHEME_TRACK].WithIndex(
            (direction == 1 ? SPR_RIVER_RAPIDS_FLAT_FRONT_NW_SE : SPR_RIVER_RAPIDS_FLAT_FRONT_SE_NW));
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 1, 32, 3 }, { 27, 0, height + 17 });
    }
    else
    {
        imageId = session.TrackColours[SCHEME_TRACK].WithIndex(
            (direction == 0 ? SPR_RIVER_RAPIDS_FLAT_SW_NE : SPR_RIVER_RAPIDS_FLAT_NE_SW));
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 32, 24, 11 }, { 0, 4, height });

        imageId = session.TrackColours[SCHEME_TRACK].WithIndex((SPR_RIVER_RAPIDS_RAPIDS_WHIRLPOOL_FRAME_0 + frameNum));
        PaintAddImageAsChild(session, imageId, { 0, 0, height }, { 32, 24, 11 }, { 0, 4, height });

        imageId = session.TrackColours[SCHEME_TRACK].WithIndex(
            (direction == 0 ? SPR_RIVER_RAPIDS_FLAT_FRONT_SW_NE : SPR_RIVER_RAPIDS_FLAT_FRONT_NE_SW));
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 32, 1, 3 }, { 0, 27, height + 17 });
    }

    wooden_a_supports_paint_setup(session, (direction & 1), 0, height, session.TrackColours[SCHEME_SUPPORTS]);

    if (direction & 1)
    {
        paint_util_push_tunnel_right(session, height, TUNNEL_SQUARE_FLAT);
    }
    else
    {
        paint_util_push_tunnel_left(session, height, TUNNEL_SQUARE_FLAT);
    }

    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/**
 * rct2: 0x0075745C
 **/
TRACK_PAINT_FUNCTION get_track_paint_function_river_rapids(int32_t trackType)
{
    switch (trackType)
    {
        case TrackElemType::Flat:
            return paint_river_rapids_track_flat;

        case TrackElemType::EndStation:
        case TrackElemType::BeginStation:
        case TrackElemType::MiddleStation:
            return paint_river_rapids_station;

        case TrackElemType::Up25:
            return paint_river_rapids_track_25_deg_up;
        case TrackElemType::FlatToUp25:
            return paint_river_rapids_track_flat_to_25_deg_up;
        case TrackElemType::Up25ToFlat:
            return paint_river_rapids_track_25_deg_up_to_flat;

        case TrackElemType::Down25:
            return paint_river_rapids_track_25_deg_down;
        case TrackElemType::FlatToDown25:
            return paint_river_rapids_track_flat_to_25_deg_down;
        case TrackElemType::Down25ToFlat:
            return paint_river_rapids_track_25_deg_down_to_flat;

        case TrackElemType::LeftQuarterTurn1Tile:
            return paint_river_rapids_track_left_quarter_turn_1_tile;
        case TrackElemType::RightQuarterTurn1Tile:
            return paint_river_rapids_track_right_quarter_turn_1_tile;

        case TrackElemType::Waterfall:
            return paint_river_rapids_track_waterfall;

        case TrackElemType::Rapids:
            return paint_river_rapids_track_rapids;

        case TrackElemType::OnRidePhoto:
            return paint_river_rapids_track_on_ride_photo;

        case TrackElemType::Whirlpool:
            return paint_river_rapids_track_whirlpool;
    }

    return nullptr;
}
