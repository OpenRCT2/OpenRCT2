/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../common.h"
#include "../../interface/Viewport.h"
#include "../../paint/Paint.h"
#include "../../paint/support/WoodenSupports.h"
#include "../../paint/tile_element/Segment.h"
#include "../../paint/track/Segment.h"
#include "../Track.h"
#include "../TrackPaint.h"
#include "../Vehicle.h"
#include "../VehiclePaint.h"

static constexpr WoodenSupportType kSupportType = WoodenSupportType::Truss;

// 0x009927E6:
static constexpr VehicleBoundBox _virginiaReelBoundbox[] = {
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

static constexpr uint32_t virginia_reel_track_pieces_flat[4] = {
    SPR_VIRGINIA_REEL_FLAT_SW_NE,
    SPR_VIRGINIA_REEL_FLAT_NW_SE,
    SPR_VIRGINIA_REEL_FLAT_SW_NE,
    SPR_VIRGINIA_REEL_FLAT_NW_SE,
};

static constexpr uint32_t virginia_reel_track_pieces_flat_lift_hill[4] = {
    SPR_VIRGINIA_REEL_FLAT_LIFT_HILL_SW_NE,
    SPR_VIRGINIA_REEL_FLAT_LIFT_HILL_NW_SE,
    SPR_VIRGINIA_REEL_FLAT_LIFT_HILL_NE_SW,
    SPR_VIRGINIA_REEL_FLAT_LIFT_HILL_SE_NW,
};

static constexpr uint32_t virginia_reel_track_pieces_flat_to_25_deg_up[4] = {
    SPR_VIRGINIA_REEL_FLAT_TO_25_DEG_UP_SW_NE,
    SPR_VIRGINIA_REEL_FLAT_TO_25_DEG_UP_NW_SE,
    SPR_VIRGINIA_REEL_FLAT_TO_25_DEG_UP_NE_SW,
    SPR_VIRGINIA_REEL_FLAT_TO_25_DEG_UP_SE_NW,
};

static constexpr uint32_t virginia_reel_track_pieces_flat_to_25_deg_up_lift_hill[4] = {
    SPR_VIRGINIA_REEL_FLAT_TO_25_DEG_UP_LIFT_HILL_SW_NE,
    SPR_VIRGINIA_REEL_FLAT_TO_25_DEG_UP_LIFT_HILL_NW_SE,
    SPR_VIRGINIA_REEL_FLAT_TO_25_DEG_UP_LIFT_HILL_NE_SW,
    SPR_VIRGINIA_REEL_FLAT_TO_25_DEG_UP_LIFT_HILL_SE_NW,
};

static constexpr uint32_t virginia_reel_track_pieces_25_deg_up_to_flat[4] = {
    SPR_VIRGINIA_REEL_25_DEG_UP_TO_FLAT_SW_NE,
    SPR_VIRGINIA_REEL_25_DEG_UP_TO_FLAT_NW_SE,
    SPR_VIRGINIA_REEL_25_DEG_UP_TO_FLAT_NE_SW,
    SPR_VIRGINIA_REEL_25_DEG_UP_TO_FLAT_SE_NW,
};

static constexpr uint32_t virginia_reel_track_pieces_25_deg_up_to_flat_lift_hill[4] = {
    SPR_VIRGINIA_REEL_25_DEG_UP_TO_FLAT_LIFT_HILL_SW_NE,
    SPR_VIRGINIA_REEL_25_DEG_UP_TO_FLAT_LIFT_HILL_NW_SE,
    SPR_VIRGINIA_REEL_25_DEG_UP_TO_FLAT_LIFT_HILL_NE_SW,
    SPR_VIRGINIA_REEL_25_DEG_UP_TO_FLAT_LIFT_HILL_SE_NW,
};

static constexpr uint32_t virginia_reel_track_pieces_25_deg_up[4] = {
    SPR_VIRGINIA_REEL_25_DEG_UP_SW_NE,
    SPR_VIRGINIA_REEL_25_DEG_UP_NW_SE,
    SPR_VIRGINIA_REEL_25_DEG_UP_NE_SW,
    SPR_VIRGINIA_REEL_25_DEG_UP_SE_NW,
};

static constexpr uint32_t virginia_reel_track_pieces_25_deg_up_lift_hill[4] = {
    SPR_VIRGINIA_REEL_25_DEG_UP_LIFT_HILL_SW_NE,
    SPR_VIRGINIA_REEL_25_DEG_UP_LIFT_HILL_NW_SE,
    SPR_VIRGINIA_REEL_25_DEG_UP_LIFT_HILL_NE_SW,
    SPR_VIRGINIA_REEL_25_DEG_UP_LIFT_HILL_SE_NW,
};

static constexpr uint32_t virginia_reel_track_pieces_flat_quarter_turn_3_tiles[4][3] = {
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

static constexpr uint32_t virginia_reel_track_pieces_flat_quarter_turn_1_tile[4] = {
    SPR_VIRGINIA_REEL_FLAT_QUARTER_TURN_1_TILE_SW_NW,
    SPR_VIRGINIA_REEL_FLAT_QUARTER_TURN_1_TILE_NW_NE,
    SPR_VIRGINIA_REEL_FLAT_QUARTER_TURN_1_TILE_NE_SE,
    SPR_VIRGINIA_REEL_FLAT_QUARTER_TURN_1_TILE_SE_SW,
};

/**
 *
 *  rct2: 0x006D5B48
 */
void VehicleVisualVirginiaReel(
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
    const auto& vehicleBb = _virginiaReelBoundbox[baseImage_id >> 3];
    auto bb = BoundBoxXYZ{ { vehicleBb.offset_x, vehicleBb.offset_y, vehicleBb.offset_z + z },
                           { vehicleBb.length_x, vehicleBb.length_y, vehicleBb.length_z } };

    baseImage_id += carEntry->base_image_id;
    auto image_id = ImageId(baseImage_id, vehicle->colours.Body, vehicle->colours.Trim);
    if (vehicle->IsGhost())
    {
        image_id = ConstructionMarker.WithIndex(image_id.GetIndex());
    }
    PaintAddImageAsParent(session, image_id, { 0, 0, z }, bb);

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
                PaintAddImageAsChild(session, image_id, { 0, 0, z }, bb);
            }
        }
    }

    assert(carEntry->effect_visual == 1);
}

/** rct2: 0x00811264 */
static void PaintVirginiaReelTrackFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    const uint32_t* sprites = virginia_reel_track_pieces_flat;
    if (trackElement.HasChain())
    {
        sprites = virginia_reel_track_pieces_flat_lift_hill;
    }

    auto imageId = session.TrackColours.WithIndex(sprites[direction]);
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

    WoodenASupportsPaintSetupRotated(
        session, kSupportType, WoodenSupportSubType::NeSw, direction, height, session.SupportColours);

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32);
}

/** rct2: 0x00811274 */
static void PaintVirginiaReelTrack25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    const uint32_t* sprites = virginia_reel_track_pieces_25_deg_up;
    if (trackElement.HasChain())
    {
        sprites = virginia_reel_track_pieces_25_deg_up_lift_hill;
    }

    auto imageId = session.TrackColours.WithIndex(sprites[direction]);
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
            WoodenASupportsPaintSetupRotated(
                session, kSupportType, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
                WoodenSupportTransitionType::Up25Deg);
            PaintUtilPushTunnelLeft(session, height - 8, TUNNEL_SQUARE_7);
            break;
        case 1:
            WoodenASupportsPaintSetupRotated(
                session, kSupportType, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
                WoodenSupportTransitionType::Up25Deg);
            PaintUtilPushTunnelRight(session, height + 8, TUNNEL_SQUARE_8);
            break;
        case 2:
            WoodenASupportsPaintSetupRotated(
                session, kSupportType, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
                WoodenSupportTransitionType::Up25Deg);
            PaintUtilPushTunnelLeft(session, height + 8, TUNNEL_SQUARE_8);
            break;
        case 3:
            WoodenASupportsPaintSetupRotated(
                session, kSupportType, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
                WoodenSupportTransitionType::Up25Deg);
            PaintUtilPushTunnelRight(session, height - 8, TUNNEL_SQUARE_7);
            break;
    }

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56);
}

/** rct2: 0x00811294 */
static void PaintVirginiaReelTrackFlatTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    const uint32_t* sprites = virginia_reel_track_pieces_flat_to_25_deg_up;
    if (trackElement.HasChain())
    {
        sprites = virginia_reel_track_pieces_flat_to_25_deg_up_lift_hill;
    }

    auto imageId = session.TrackColours.WithIndex(sprites[direction]);
    PaintStruct* ps;
    switch (direction)
    {
        case 0:
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 2, height }, { 32, 27, 2 } });

            WoodenASupportsPaintSetupRotated(
                session, kSupportType, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
                WoodenSupportTransitionType::FlatToUp25Deg);
            PaintUtilPushTunnelLeft(session, height, TUNNEL_SQUARE_FLAT);
            break;
        case 1:
            ps = PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 2, 0, height }, { 27, 32, 2 } });
            session.WoodenSupportsPrependTo = ps;

            WoodenASupportsPaintSetupRotated(
                session, kSupportType, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
                WoodenSupportTransitionType::FlatToUp25Deg);
            PaintUtilPushTunnelRight(session, height, TUNNEL_SQUARE_8);
            break;
        case 2:
            ps = PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 2, height }, { 32, 27, 2 } });
            session.WoodenSupportsPrependTo = ps;

            WoodenASupportsPaintSetupRotated(
                session, kSupportType, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
                WoodenSupportTransitionType::FlatToUp25Deg);
            PaintUtilPushTunnelLeft(session, height, TUNNEL_SQUARE_8);
            break;
        case 3:
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 2, 0, height }, { 27, 32, 2 } });

            WoodenASupportsPaintSetupRotated(
                session, kSupportType, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
                WoodenSupportTransitionType::FlatToUp25Deg);
            PaintUtilPushTunnelRight(session, height, TUNNEL_SQUARE_FLAT);
            break;
    }

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48);
}

/** rct2: 0x00811294 */
static void PaintVirginiaReelTrack25DegUpToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    const uint32_t* sprites = virginia_reel_track_pieces_25_deg_up_to_flat;
    if (trackElement.HasChain())
    {
        sprites = virginia_reel_track_pieces_25_deg_up_to_flat_lift_hill;
    }

    auto imageId = session.TrackColours.WithIndex(sprites[direction]);
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
            WoodenASupportsPaintSetupRotated(
                session, kSupportType, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
                WoodenSupportTransitionType::Up25DegToFlat);
            PaintUtilPushTunnelLeft(session, height - 8, TUNNEL_SQUARE_FLAT);
            break;
        case 1:
            WoodenASupportsPaintSetupRotated(
                session, kSupportType, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
                WoodenSupportTransitionType::Up25DegToFlat);
            PaintUtilPushTunnelRight(session, height + 8, TUNNEL_14);
            break;
        case 2:
            WoodenASupportsPaintSetupRotated(
                session, kSupportType, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
                WoodenSupportTransitionType::Up25DegToFlat);
            PaintUtilPushTunnelLeft(session, height + 8, TUNNEL_14);
            break;
        case 3:
            WoodenASupportsPaintSetupRotated(
                session, kSupportType, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
                WoodenSupportTransitionType::Up25DegToFlat);
            PaintUtilPushTunnelRight(session, height - 8, TUNNEL_SQUARE_FLAT);
            break;
    }

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 40);
}

/** rct2: 0x008112A4 */
static void PaintVirginiaReelTrack25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    PaintVirginiaReelTrack25DegUp(session, ride, trackSequence, (direction + 2) % 4, height, trackElement);
}

/** rct2: 0x008112B4 */
static void PaintVirginiaReelTrackFlatTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    PaintVirginiaReelTrack25DegUpToFlat(session, ride, trackSequence, (direction + 2) % 4, height, trackElement);
}

/** rct2: 0x008112C4 */
static void PaintVirginiaReelTrack25DegDownToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    PaintVirginiaReelTrackFlatTo25DegUp(session, ride, trackSequence, (direction + 2) % 4, height, trackElement);
}

/** rct2: 0x008112D4, 0x008112E4, 0x008112F4 */
static void PaintVirginiaReelStation(
    PaintSession& session, const Ride& ride, [[maybe_unused]] uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    ImageId imageId;

    if (direction == 0 || direction == 2)
    {
        imageId = GetStationColourScheme(session, trackElement).WithIndex(SPR_STATION_BASE_B_SW_NE);
        PaintAddImageAsParent(session, imageId, { 0, 0, height - 2 }, { { 0, 2, height }, { 32, 28, 2 } });

        imageId = session.TrackColours.WithIndex(SPR_VIRGINIA_REEL_FLAT_SW_NE);
        PaintAddImageAsChild(session, imageId, { 0, 0, height }, { { 0, 0, height }, { 32, 20, 2 } });

        PaintUtilPushTunnelLeft(session, height, TUNNEL_SQUARE_FLAT);
    }
    else if (direction == 1 || direction == 3)
    {
        imageId = GetStationColourScheme(session, trackElement).WithIndex(SPR_STATION_BASE_B_NW_SE);
        PaintAddImageAsParent(session, imageId, { 0, 0, height - 2 }, { { 2, 0, height }, { 28, 32, 2 } });

        imageId = session.TrackColours.WithIndex(SPR_VIRGINIA_REEL_FLAT_NW_SE);
        PaintAddImageAsChild(session, imageId, { 0, 0, height }, { { 0, 0, height }, { 20, 32, 2 } });

        PaintUtilPushTunnelRight(session, height, TUNNEL_SQUARE_FLAT);
    }

    WoodenASupportsPaintSetupRotated(
        session, kSupportType, WoodenSupportSubType::NeSw, direction, height, session.SupportColours);
    TrackPaintUtilDrawStation(session, ride, direction, height, trackElement);

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32);
}

/** rct2: 0x00811304 */
static void PaintVirginiaReelTrackLeftQuarterTurn3Tiles(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    TrackPaintUtilLeftQuarterTurn3TilesPaint(
        session, 2, height, direction, trackSequence, session.TrackColours,
        virginia_reel_track_pieces_flat_quarter_turn_3_tiles);
    TrackPaintUtilLeftQuarterTurn3TilesTunnel(session, height, TUNNEL_SQUARE_FLAT, direction, trackSequence);

    switch (trackSequence)
    {
        case 2:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::topLeftSide, PaintSegment::centre, PaintSegment::bottomLeftSide,
                        PaintSegment::leftCorner),
                    direction),
                0xFFFF, 0);
            break;
        case 0:
        case 3:
            WoodenASupportsPaintSetupRotated(
                session, kSupportType, WoodenSupportSubType::Corner3, direction, height, session.SupportColours);
            PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(kSegmentsAll, direction), 0xFFFF, 0);
            break;
    }

    PaintUtilSetGeneralSupportHeight(session, height + 32);
}

static constexpr uint8_t virginia_reel_right_quarter_turn_3_tiles_to_left_turn_map[] = {
    3,
    1,
    2,
    0,
};

/** rct2: 0x00811314 */
static void PaintVirginiaReelTrackRightQuarterTurn3Tiles(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = virginia_reel_right_quarter_turn_3_tiles_to_left_turn_map[trackSequence];
    PaintVirginiaReelTrackLeftQuarterTurn3Tiles(session, ride, trackSequence, (direction + 3) % 4, height, trackElement);
}

/** rct2: 0x00811324 */
static void PaintVirginiaReelTrackLeftQuarterTurn1Tile(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    TrackPaintUtilLeftQuarterTurn1TilePaint(
        session, 2, height, 0, direction, session.TrackColours, virginia_reel_track_pieces_flat_quarter_turn_1_tile);

    switch (direction)
    {
        case 0:
            WoodenASupportsPaintSetup(session, kSupportType, WoodenSupportSubType::Corner3, height, session.SupportColours);
            PaintUtilPushTunnelLeft(session, height, TUNNEL_SQUARE_FLAT);
            break;
        case 1:
            WoodenASupportsPaintSetup(session, kSupportType, WoodenSupportSubType::Corner0, height, session.SupportColours);
            break;
        case 2:
            WoodenASupportsPaintSetup(session, kSupportType, WoodenSupportSubType::Corner1, height, session.SupportColours);
            PaintUtilPushTunnelRight(session, height, TUNNEL_SQUARE_FLAT);
            break;
        case 3:
            WoodenASupportsPaintSetup(session, kSupportType, WoodenSupportSubType::Corner2, height, session.SupportColours);
            PaintUtilPushTunnelRight(session, height, TUNNEL_SQUARE_FLAT);
            PaintUtilPushTunnelLeft(session, height, TUNNEL_SQUARE_FLAT);
            break;
    }

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32);
}

/** rct2: 0x00811334 */
static void PaintVirginiaReelTrackRightQuarterTurn1Tile(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    PaintVirginiaReelTrackLeftQuarterTurn1Tile(session, ride, trackSequence, (direction + 3) % 4, height, trackElement);
}

/**
 * rct2: 0x00811184
 */
TRACK_PAINT_FUNCTION GetTrackPaintFunctionVirginiaReel(int32_t trackType)
{
    switch (trackType)
    {
        case TrackElemType::Flat:
            return PaintVirginiaReelTrackFlat;

        case TrackElemType::EndStation:
        case TrackElemType::BeginStation:
        case TrackElemType::MiddleStation:
            return PaintVirginiaReelStation;

        case TrackElemType::Up25:
            return PaintVirginiaReelTrack25DegUp;
        case TrackElemType::FlatToUp25:
            return PaintVirginiaReelTrackFlatTo25DegUp;
        case TrackElemType::Up25ToFlat:
            return PaintVirginiaReelTrack25DegUpToFlat;

        case TrackElemType::Down25:
            return PaintVirginiaReelTrack25DegDown;
        case TrackElemType::FlatToDown25:
            return PaintVirginiaReelTrackFlatTo25DegDown;
        case TrackElemType::Down25ToFlat:
            return PaintVirginiaReelTrack25DegDownToFlat;

        case TrackElemType::LeftQuarterTurn3Tiles:
            return PaintVirginiaReelTrackLeftQuarterTurn3Tiles;
        case TrackElemType::RightQuarterTurn3Tiles:
            return PaintVirginiaReelTrackRightQuarterTurn3Tiles;

        case TrackElemType::LeftQuarterTurn1Tile:
            return PaintVirginiaReelTrackLeftQuarterTurn1Tile;
        case TrackElemType::RightQuarterTurn1Tile:
            return PaintVirginiaReelTrackRightQuarterTurn1Tile;
    }

    return nullptr;
}
