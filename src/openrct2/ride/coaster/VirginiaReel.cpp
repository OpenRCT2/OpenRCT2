/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
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
#include "../Vehicle.h"
#include "../VehiclePaint.h"

// 0x009927E6:
static constexpr const vehicle_boundbox _virginiaReelBoundbox[] = {
    { -11, -11, 1, 22, 22, 13 }, { -11, -11, 1, 22, 22, 13 }, { -11, -11, 1, 22, 22, 13 },
    { -11, -11, 1, 22, 22, 13 }, { -11, -11, 1, 22, 22, 13 }, { -11, -11, 1, 22, 22, 13 },
    { -11, -11, 1, 22, 22, 13 }, { -11, -11, 1, 22, 22, 13 }, { -11, -11, 1, 22, 22, 13 },
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

static constexpr const uint32_t virginia_reel_track_pieces_flat[4] = {
    SPR_VIRGINIA_REEL_FLAT_SW_NE,
    SPR_VIRGINIA_REEL_FLAT_NW_SE,
    SPR_VIRGINIA_REEL_FLAT_SW_NE,
    SPR_VIRGINIA_REEL_FLAT_NW_SE,
};

static constexpr const uint32_t virginia_reel_track_pieces_flat_lift_hill[4] = {
    SPR_VIRGINIA_REEL_FLAT_LIFT_HILL_SW_NE,
    SPR_VIRGINIA_REEL_FLAT_LIFT_HILL_NW_SE,
    SPR_VIRGINIA_REEL_FLAT_LIFT_HILL_NE_SW,
    SPR_VIRGINIA_REEL_FLAT_LIFT_HILL_SE_NW,
};

static constexpr const uint32_t virginia_reel_track_pieces_flat_to_25_deg_up[4] = {
    SPR_VIRGINIA_REEL_FLAT_TO_25_DEG_UP_SW_NE,
    SPR_VIRGINIA_REEL_FLAT_TO_25_DEG_UP_NW_SE,
    SPR_VIRGINIA_REEL_FLAT_TO_25_DEG_UP_NE_SW,
    SPR_VIRGINIA_REEL_FLAT_TO_25_DEG_UP_SE_NW,
};

static constexpr const uint32_t virginia_reel_track_pieces_flat_to_25_deg_up_lift_hill[4] = {
    SPR_VIRGINIA_REEL_FLAT_TO_25_DEG_UP_LIFT_HILL_SW_NE,
    SPR_VIRGINIA_REEL_FLAT_TO_25_DEG_UP_LIFT_HILL_NW_SE,
    SPR_VIRGINIA_REEL_FLAT_TO_25_DEG_UP_LIFT_HILL_NE_SW,
    SPR_VIRGINIA_REEL_FLAT_TO_25_DEG_UP_LIFT_HILL_SE_NW,
};

static constexpr const uint32_t virginia_reel_track_pieces_25_deg_up_to_flat[4] = {
    SPR_VIRGINIA_REEL_25_DEG_UP_TO_FLAT_SW_NE,
    SPR_VIRGINIA_REEL_25_DEG_UP_TO_FLAT_NW_SE,
    SPR_VIRGINIA_REEL_25_DEG_UP_TO_FLAT_NE_SW,
    SPR_VIRGINIA_REEL_25_DEG_UP_TO_FLAT_SE_NW,
};

static constexpr const uint32_t virginia_reel_track_pieces_25_deg_up_to_flat_lift_hill[4] = {
    SPR_VIRGINIA_REEL_25_DEG_UP_TO_FLAT_LIFT_HILL_SW_NE,
    SPR_VIRGINIA_REEL_25_DEG_UP_TO_FLAT_LIFT_HILL_NW_SE,
    SPR_VIRGINIA_REEL_25_DEG_UP_TO_FLAT_LIFT_HILL_NE_SW,
    SPR_VIRGINIA_REEL_25_DEG_UP_TO_FLAT_LIFT_HILL_SE_NW,
};

static constexpr const uint32_t virginia_reel_track_pieces_25_deg_up[4] = {
    SPR_VIRGINIA_REEL_25_DEG_UP_SW_NE,
    SPR_VIRGINIA_REEL_25_DEG_UP_NW_SE,
    SPR_VIRGINIA_REEL_25_DEG_UP_NE_SW,
    SPR_VIRGINIA_REEL_25_DEG_UP_SE_NW,
};

static constexpr const uint32_t virginia_reel_track_pieces_25_deg_up_lift_hill[4] = {
    SPR_VIRGINIA_REEL_25_DEG_UP_LIFT_HILL_SW_NE,
    SPR_VIRGINIA_REEL_25_DEG_UP_LIFT_HILL_NW_SE,
    SPR_VIRGINIA_REEL_25_DEG_UP_LIFT_HILL_NE_SW,
    SPR_VIRGINIA_REEL_25_DEG_UP_LIFT_HILL_SE_NW,
};

static constexpr const uint32_t virginia_reel_track_pieces_flat_quarter_turn_3_tiles[4][3] = {
    {
        SPR_VIRGINIA_REEL_FLAT_QUARTER_TURN_3_TILES_SW_SE_PART_0,
        SPR_VIRGINIA_REEL_FLAT_QUARTER_TURN_3_TILES_SW_SE_PART_1,
        SPR_VIRGINIA_REEL_FLAT_QUARTER_TURN_3_TILES_SW_SE_PART_2,
    },
    {
        SPR_VIRGINIA_REEL_FLAT_QUARTER_TURN_3_TILES_NW_SW_PART_0,
        SPR_VIRGINIA_REEL_FLAT_QUARTER_TURN_3_TILES_NW_SW_PART_1,
        SPR_VIRGINIA_REEL_FLAT_QUARTER_TURN_3_TILES_NW_SW_PART_2,
    },
    {
        SPR_VIRGINIA_REEL_FLAT_QUARTER_TURN_3_TILES_NE_NW_PART_0,
        SPR_VIRGINIA_REEL_FLAT_QUARTER_TURN_3_TILES_NE_NW_PART_1,
        SPR_VIRGINIA_REEL_FLAT_QUARTER_TURN_3_TILES_NE_NW_PART_2,
    },
    {
        SPR_VIRGINIA_REEL_FLAT_QUARTER_TURN_3_TILES_SE_NE_PART_0,
        SPR_VIRGINIA_REEL_FLAT_QUARTER_TURN_3_TILES_SE_NE_PART_1,
        SPR_VIRGINIA_REEL_FLAT_QUARTER_TURN_3_TILES_SE_NE_PART_2,
    },
};

static constexpr const uint32_t virginia_reel_track_pieces_flat_quarter_turn_1_tile[4] = {
    SPR_VIRGINIA_REEL_FLAT_QUARTER_TURN_1_TILE_SW_NW,
    SPR_VIRGINIA_REEL_FLAT_QUARTER_TURN_1_TILE_NW_NE,
    SPR_VIRGINIA_REEL_FLAT_QUARTER_TURN_1_TILE_NE_SE,
    SPR_VIRGINIA_REEL_FLAT_QUARTER_TURN_1_TILE_SE_SW,
};

/**
 *
 *  rct2: 0x006D5B48
 */
void vehicle_visual_virginia_reel(
    PaintSession& session, int32_t x, int32_t imageDirection, int32_t y, int32_t z, const Vehicle* vehicle,
    const CarEntry* carEntry)
{
    imageDirection = OpenRCT2::Entity::Yaw::YawTo32(imageDirection);
    const uint8_t rotation = session.CurrentRotation;
    int32_t ecx = ((vehicle->spin_sprite / 8) + (rotation * 8)) & 31;
    int32_t baseImage_id = [&] {
        switch (vehicle->Pitch)
        {
            case 1:
                return (imageDirection & 24) + 8;
            case 2:
                return (imageDirection & 24) + 40;
            case 5:
                return ((imageDirection ^ 16) & 24) + 8;
            case 6:
                return ((imageDirection ^ 16) & 24) + 40;
            default:
                return 0;
        }
    }();
    baseImage_id += ecx & 7;
    const vehicle_boundbox* bb = &_virginiaReelBoundbox[baseImage_id >> 3];

    baseImage_id += carEntry->base_image_id;
    auto image_id = ImageId(baseImage_id, vehicle->colours.Body, vehicle->colours.Trim);
    if (vehicle->IsGhost())
    {
        image_id = ConstructionMarker.WithIndex(image_id.GetIndex());
    }
    PaintAddImageAsParent(
        session, image_id, { 0, 0, z }, { bb->length_x, bb->length_y, bb->length_z },
        { bb->offset_x, bb->offset_y, bb->offset_z + z });

    if (session.DPI.zoom_level < ZoomLevel{ 2 } && vehicle->num_peeps > 0 && !vehicle->IsGhost())
    {
        uint8_t riding_peep_sprites[4] = { 0xFF, 0xFF, 0xFF, 0xFF };
        for (int32_t i = 0; i < vehicle->num_peeps; i++)
        {
            riding_peep_sprites[((ecx / 8) + i) & 3] = vehicle->peep_tshirt_colours[i];
        }
        int32_t draw_order[4] = { 0, 1, 3, 2 };
        for (auto i : draw_order)
        {
            if (riding_peep_sprites[i] != 0xFF)
            {
                image_id = ImageId(baseImage_id + ((i + 1) * 72), riding_peep_sprites[i]);
                PaintAddImageAsChild(
                    session, image_id, { 0, 0, z }, { bb->length_x, bb->length_y, bb->length_z },
                    { bb->offset_x, bb->offset_y, bb->offset_z + z });
            }
        }
    }

    assert(carEntry->effect_visual == 1);
}

/** rct2: 0x00811264 */
static void paint_virginia_reel_track_flat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    const uint32_t* sprites = virginia_reel_track_pieces_flat;
    if (trackElement.HasChain())
    {
        sprites = virginia_reel_track_pieces_flat_lift_hill;
    }

    auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex(sprites[direction]);
    if (direction & 1)
    {
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 2, 0, height }, { 27, 32, 2 } });
        PaintUtilPushTunnelRight(session, height, TUNNEL_SQUARE_FLAT);
    }
    else
    {
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 2, height }, { 32, 27, 2 } });
        PaintUtilPushTunnelLeft(session, height, TUNNEL_SQUARE_FLAT);
    }

    WoodenASupportsPaintSetup(session, (direction & 1), 0, height, session.TrackColours[SCHEME_SUPPORTS]);

    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

/** rct2: 0x00811274 */
static void paint_virginia_reel_track_25_deg_up(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    const uint32_t* sprites = virginia_reel_track_pieces_25_deg_up;
    if (trackElement.HasChain())
    {
        sprites = virginia_reel_track_pieces_25_deg_up_lift_hill;
    }

    auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex(sprites[direction]);
    PaintStruct* ps;

    if (direction & 1)
    {
        ps = PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 2, 0, height }, { 27, 32, 2 } });
    }
    else
    {
        ps = PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 2, height }, { 32, 27, 2 } });
    }

    if (direction == 1 || direction == 2)
    {
        session.WoodenSupportsPrependTo = ps;
    }

    switch (direction)
    {
        case 0:
            WoodenASupportsPaintSetup(session, 0, 9, height, session.TrackColours[SCHEME_SUPPORTS]);
            PaintUtilPushTunnelLeft(session, height - 8, TUNNEL_SQUARE_7);
            break;
        case 1:
            WoodenASupportsPaintSetup(session, 1, 10, height, session.TrackColours[SCHEME_SUPPORTS]);
            PaintUtilPushTunnelRight(session, height + 8, TUNNEL_SQUARE_8);
            break;
        case 2:
            WoodenASupportsPaintSetup(session, 0, 11, height, session.TrackColours[SCHEME_SUPPORTS]);
            PaintUtilPushTunnelLeft(session, height + 8, TUNNEL_SQUARE_8);
            break;
        case 3:
            WoodenASupportsPaintSetup(session, 1, 12, height, session.TrackColours[SCHEME_SUPPORTS]);
            PaintUtilPushTunnelRight(session, height - 8, TUNNEL_SQUARE_7);
            break;
    }

    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
}

/** rct2: 0x00811294 */
static void paint_virginia_reel_track_flat_to_25_deg_up(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    const uint32_t* sprites = virginia_reel_track_pieces_flat_to_25_deg_up;
    if (trackElement.HasChain())
    {
        sprites = virginia_reel_track_pieces_flat_to_25_deg_up_lift_hill;
    }

    auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex(sprites[direction]);
    PaintStruct* ps;
    switch (direction)
    {
        case 0:
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 2, height }, { 32, 27, 2 } });

            WoodenASupportsPaintSetup(session, 0, 1, height, session.TrackColours[SCHEME_SUPPORTS]);
            PaintUtilPushTunnelLeft(session, height, TUNNEL_SQUARE_FLAT);
            break;
        case 1:
            ps = PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 2, 0, height }, { 27, 32, 2 } });
            session.WoodenSupportsPrependTo = ps;

            WoodenASupportsPaintSetup(session, 1, 2, height, session.TrackColours[SCHEME_SUPPORTS]);
            PaintUtilPushTunnelRight(session, height, TUNNEL_SQUARE_8);
            break;
        case 2:
            ps = PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 2, height }, { 32, 27, 2 } });
            session.WoodenSupportsPrependTo = ps;

            WoodenASupportsPaintSetup(session, 0, 3, height, session.TrackColours[SCHEME_SUPPORTS]);
            PaintUtilPushTunnelLeft(session, height, TUNNEL_SQUARE_8);
            break;
        case 3:
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 2, 0, height }, { 27, 32, 2 } });

            WoodenASupportsPaintSetup(session, 1, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
            PaintUtilPushTunnelRight(session, height, TUNNEL_SQUARE_FLAT);
            break;
    }

    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
}

/** rct2: 0x00811294 */
static void paint_virginia_reel_track_25_deg_up_to_flat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    const uint32_t* sprites = virginia_reel_track_pieces_25_deg_up_to_flat;
    if (trackElement.HasChain())
    {
        sprites = virginia_reel_track_pieces_25_deg_up_to_flat_lift_hill;
    }

    auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex(sprites[direction]);
    PaintStruct* ps;

    if (direction & 1)
    {
        ps = PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 2, 0, height }, { 27, 32, 2 } });
    }
    else
    {
        ps = PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 2, height }, { 32, 27, 2 } });
    }

    if (direction == 1 || direction == 2)
    {
        session.WoodenSupportsPrependTo = ps;
    }

    switch (direction)
    {
        case 0:
            WoodenASupportsPaintSetup(session, 0, 5, height, session.TrackColours[SCHEME_SUPPORTS]);
            PaintUtilPushTunnelLeft(session, height - 8, TUNNEL_SQUARE_FLAT);
            break;
        case 1:
            WoodenASupportsPaintSetup(session, 1, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
            PaintUtilPushTunnelRight(session, height + 8, TUNNEL_14);
            break;
        case 2:
            WoodenASupportsPaintSetup(session, 0, 7, height, session.TrackColours[SCHEME_SUPPORTS]);
            PaintUtilPushTunnelLeft(session, height + 8, TUNNEL_14);
            break;
        case 3:
            WoodenASupportsPaintSetup(session, 1, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
            PaintUtilPushTunnelRight(session, height - 8, TUNNEL_SQUARE_FLAT);
            break;
    }

    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 40, 0x20);
}

/** rct2: 0x008112A4 */
static void paint_virginia_reel_track_25_deg_down(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    paint_virginia_reel_track_25_deg_up(session, ride, trackSequence, (direction + 2) % 4, height, trackElement);
}

/** rct2: 0x008112B4 */
static void paint_virginia_reel_track_flat_to_25_deg_down(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    paint_virginia_reel_track_25_deg_up_to_flat(session, ride, trackSequence, (direction + 2) % 4, height, trackElement);
}

/** rct2: 0x008112C4 */
static void paint_virginia_reel_track_25_deg_down_to_flat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    paint_virginia_reel_track_flat_to_25_deg_up(session, ride, trackSequence, (direction + 2) % 4, height, trackElement);
}

/** rct2: 0x008112D4, 0x008112E4, 0x008112F4 */
static void paint_virginia_reel_station(
    PaintSession& session, const Ride& ride, [[maybe_unused]] uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    ImageId imageId;

    if (direction == 0 || direction == 2)
    {
        imageId = session.TrackColours[SCHEME_MISC].WithIndex(SPR_STATION_BASE_B_SW_NE);
        PaintAddImageAsParent(session, imageId, { 0, 0, height - 2 }, { { 0, 2, height }, { 32, 28, 2 } });

        imageId = session.TrackColours[SCHEME_TRACK].WithIndex(SPR_VIRGINIA_REEL_FLAT_SW_NE);
        PaintAddImageAsChild(session, imageId, { 0, 0, height }, { 32, 20, 2 }, { 0, 0, height });

        PaintUtilPushTunnelLeft(session, height, TUNNEL_SQUARE_FLAT);
    }
    else if (direction == 1 || direction == 3)
    {
        imageId = session.TrackColours[SCHEME_MISC].WithIndex(SPR_STATION_BASE_B_NW_SE);
        PaintAddImageAsParent(session, imageId, { 0, 0, height - 2 }, { { 2, 0, height }, { 28, 32, 2 } });

        imageId = session.TrackColours[SCHEME_TRACK].WithIndex(SPR_VIRGINIA_REEL_FLAT_NW_SE);
        PaintAddImageAsChild(session, imageId, { 0, 0, height }, { 20, 32, 2 }, { 0, 0, height });

        PaintUtilPushTunnelRight(session, height, TUNNEL_SQUARE_FLAT);
    }

    WoodenASupportsPaintSetup(session, (direction & 1), 0, height, session.TrackColours[SCHEME_SUPPORTS]);
    track_paint_util_draw_station(session, ride, direction, height, trackElement);

    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

static constexpr const uint8_t virginia_reel_left_quarter_turn_supports[] = {
    5,
    2,
    3,
    4,
};

/** rct2: 0x00811304 */
static void paint_virginia_reel_track_left_quarter_turn_3_tiles(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    track_paint_util_left_quarter_turn_3_tiles_paint(
        session, 2, height, direction, trackSequence, session.TrackColours[SCHEME_TRACK],
        virginia_reel_track_pieces_flat_quarter_turn_3_tiles);
    track_paint_util_left_quarter_turn_3_tiles_tunnel(session, height, TUNNEL_SQUARE_FLAT, direction, trackSequence);

    switch (trackSequence)
    {
        case 2:
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C8 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_B8, direction), 0xFFFF, 0);
            break;
        case 0:
        case 3:
            WoodenASupportsPaintSetup(
                session, virginia_reel_left_quarter_turn_supports[direction], 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(SEGMENTS_ALL, direction), 0xFFFF, 0);
            break;
    }

    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

static constexpr const uint8_t virginia_reel_right_quarter_turn_3_tiles_to_left_turn_map[] = {
    3,
    1,
    2,
    0,
};

/** rct2: 0x00811314 */
static void paint_virginia_reel_track_right_quarter_turn_3_tiles(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = virginia_reel_right_quarter_turn_3_tiles_to_left_turn_map[trackSequence];
    paint_virginia_reel_track_left_quarter_turn_3_tiles(
        session, ride, trackSequence, (direction + 3) % 4, height, trackElement);
}

/** rct2: 0x00811324 */
static void paint_virginia_reel_track_left_quarter_turn_1_tile(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    track_paint_util_left_quarter_turn_1_tile_paint(
        session, 2, height, 0, direction, session.TrackColours[SCHEME_TRACK],
        virginia_reel_track_pieces_flat_quarter_turn_1_tile);

    switch (direction)
    {
        case 0:
            WoodenASupportsPaintSetup(session, 5, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            PaintUtilPushTunnelLeft(session, height, TUNNEL_SQUARE_FLAT);
            break;
        case 1:
            WoodenASupportsPaintSetup(session, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            WoodenASupportsPaintSetup(session, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            PaintUtilPushTunnelRight(session, height, TUNNEL_SQUARE_FLAT);
            break;
        case 3:
            WoodenASupportsPaintSetup(session, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            PaintUtilPushTunnelRight(session, height, TUNNEL_SQUARE_FLAT);
            PaintUtilPushTunnelLeft(session, height, TUNNEL_SQUARE_FLAT);
            break;
    }

    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

/** rct2: 0x00811334 */
static void paint_virginia_reel_track_right_quarter_turn_1_tile(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    paint_virginia_reel_track_left_quarter_turn_1_tile(session, ride, trackSequence, (direction + 3) % 4, height, trackElement);
}

/**
 * rct2: 0x00811184
 */
TRACK_PAINT_FUNCTION get_track_paint_function_virginia_reel(int32_t trackType)
{
    switch (trackType)
    {
        case TrackElemType::Flat:
            return paint_virginia_reel_track_flat;

        case TrackElemType::EndStation:
        case TrackElemType::BeginStation:
        case TrackElemType::MiddleStation:
            return paint_virginia_reel_station;

        case TrackElemType::Up25:
            return paint_virginia_reel_track_25_deg_up;
        case TrackElemType::FlatToUp25:
            return paint_virginia_reel_track_flat_to_25_deg_up;
        case TrackElemType::Up25ToFlat:
            return paint_virginia_reel_track_25_deg_up_to_flat;

        case TrackElemType::Down25:
            return paint_virginia_reel_track_25_deg_down;
        case TrackElemType::FlatToDown25:
            return paint_virginia_reel_track_flat_to_25_deg_down;
        case TrackElemType::Down25ToFlat:
            return paint_virginia_reel_track_25_deg_down_to_flat;

        case TrackElemType::LeftQuarterTurn3Tiles:
            return paint_virginia_reel_track_left_quarter_turn_3_tiles;
        case TrackElemType::RightQuarterTurn3Tiles:
            return paint_virginia_reel_track_right_quarter_turn_3_tiles;

        case TrackElemType::LeftQuarterTurn1Tile:
            return paint_virginia_reel_track_left_quarter_turn_1_tile;
        case TrackElemType::RightQuarterTurn1Tile:
            return paint_virginia_reel_track_right_quarter_turn_1_tile;
    }

    return nullptr;
}
