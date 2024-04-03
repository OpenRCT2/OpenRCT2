/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../Game.h"
#include "../../GameState.h"
#include "../../config/Config.h"
#include "../../interface/Viewport.h"
#include "../../paint/Paint.h"
#include "../../paint/support/WoodenSupports.h"
#include "../../scenario/Scenario.h"
#include "../../world/Map.h"
#include "../Track.h"
#include "../TrackPaint.h"
#include "../Vehicle.h"
#include "../VehiclePaint.h"

using namespace OpenRCT2;

#ifndef NO_VEHICLES
// 0x0099279E:
static constexpr VehicleBoundBox _riverRapidsBoundbox[] = {
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

static constexpr uint32_t river_rapids_track_pieces_25_deg_up[][2] = {
    { SPR_RIVER_RAPIDS_25_DEG_UP_SW_NE, SPR_RIVER_RAPIDS_25_DEG_UP_FRONT_SW_NE },
    { SPR_RIVER_RAPIDS_25_DEG_UP_NW_SE, SPR_RIVER_RAPIDS_25_DEG_UP_FRONT_NW_SE },
    { SPR_RIVER_RAPIDS_25_DEG_UP_NE_SW, SPR_RIVER_RAPIDS_25_DEG_UP_FRONT_NE_SW },
    { SPR_RIVER_RAPIDS_25_DEG_UP_SE_NW, SPR_RIVER_RAPIDS_25_DEG_UP_FRONT_SE_NW },
};
static constexpr uint32_t river_rapids_track_pieces_flat_to_25_deg_up[][2] = {
    { SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_UP_SW_NE, SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_UP_FRONT_SW_NE },
    { SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_UP_NW_SE, SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_UP_FRONT_NW_SE },
    { SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_UP_NE_SW, SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_UP_FRONT_NE_SW },
    { SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_UP_SE_NW, SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_UP_FRONT_SE_NW },
};

static constexpr uint32_t river_rapids_track_pieces_25_deg_up_to_flat[][2] = {
    { SPR_RIVER_RAPIDS_25_DEG_UP_TO_FLAT_SW_NE, SPR_RIVER_RAPIDS_25_DEG_UP_TO_FLAT_FRONT_SW_NE },
    { SPR_RIVER_RAPIDS_25_DEG_UP_TO_FLAT_NW_SE, SPR_RIVER_RAPIDS_25_DEG_UP_TO_FLAT_FRONT_NW_SE },
    { SPR_RIVER_RAPIDS_25_DEG_UP_TO_FLAT_NE_SW, SPR_RIVER_RAPIDS_25_DEG_UP_TO_FLAT_FRONT_NE_SW },
    { SPR_RIVER_RAPIDS_25_DEG_UP_TO_FLAT_SE_NW, SPR_RIVER_RAPIDS_25_DEG_UP_TO_FLAT_FRONT_SE_NW },
};

static constexpr uint32_t river_rapids_track_pieces_25_deg_down[][2] = {
    { SPR_RIVER_RAPIDS_25_DEG_DOWN_SW_NE, SPR_RIVER_RAPIDS_25_DEG_DOWN_FRONT_SW_NE },
    { SPR_RIVER_RAPIDS_25_DEG_DOWN_NW_SE, SPR_RIVER_RAPIDS_25_DEG_DOWN_FRONT_NW_SE },
    { SPR_RIVER_RAPIDS_25_DEG_DOWN_NE_SW, SPR_RIVER_RAPIDS_25_DEG_DOWN_FRONT_NE_SW },
    { SPR_RIVER_RAPIDS_25_DEG_DOWN_SE_NW, SPR_RIVER_RAPIDS_25_DEG_DOWN_FRONT_SE_NW },
};

static constexpr uint32_t river_rapids_track_pieces_flat_to_25_deg_down[][2] = {
    { SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_DOWN_SW_NE, SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_DOWN_FRONT_SW_NE },
    { SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_DOWN_NW_SE, SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_DOWN_FRONT_NW_SE },
    { SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_DOWN_NE_SW, SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_DOWN_FRONT_NE_SW },
    { SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_DOWN_SE_NW, SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_DOWN_FRONT_SE_NW },
};

static constexpr uint32_t river_rapids_track_pieces_25_deg_down_to_flat[][2] = {
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
void VehicleVisualRiverRapids(
    PaintSession& session, int32_t x, int32_t imageDirection, int32_t y, int32_t z, const Vehicle* vehicle,
    const CarEntry* carEntry)
{
    imageDirection = OpenRCT2::Entity::Yaw::YawTo32(imageDirection);

    ImageId image_id;
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

    const auto& riverRapidsBb = _riverRapidsBoundbox[j];
    auto bb = BoundBoxXYZ{ { riverRapidsBb.offset_x, riverRapidsBb.offset_y, riverRapidsBb.offset_z + z },
                           { riverRapidsBb.length_x, riverRapidsBb.length_y, riverRapidsBb.length_z } };
    image_id = ImageId(baseImage_id, vehicle->colours.Body, vehicle->colours.Trim);
    if (vehicle->IsGhost())
    {
        image_id = ConstructionMarker.WithIndex(image_id.GetIndex());
    }
    PaintAddImageAsParent(session, image_id, { 0, 0, z }, bb);

    if (session.DPI.zoom_level < ZoomLevel{ 2 } && vehicle->num_peeps > 0 && !vehicle->IsGhost())
    {
        // Draw peeps: (this particular vehicle doesn't sort them back to front like others so the back ones sometimes clip, but
        // that's how the original does it...)
        int32_t peeps = ((ecx / 8) + 0) & 3;
        image_id = ImageId(baseImage_id + ((peeps + 1) * 72), vehicle->peep_tshirt_colours[0], vehicle->peep_tshirt_colours[1]);
        PaintAddImageAsChild(session, image_id, { 0, 0, z }, bb);
        if (vehicle->num_peeps > 2)
        {
            peeps = ((ecx / 8) + 2) & 3;
            image_id = ImageId(
                baseImage_id + ((peeps + 1) * 72), vehicle->peep_tshirt_colours[2], vehicle->peep_tshirt_colours[3]);
            PaintAddImageAsChild(session, image_id, { 0, 0, z }, bb);
        }
        if (vehicle->num_peeps > 4)
        {
            peeps = ((ecx / 8) + 1) & 3;
            image_id = ImageId(
                baseImage_id + ((peeps + 1) * 72), vehicle->peep_tshirt_colours[4], vehicle->peep_tshirt_colours[5]);
            PaintAddImageAsChild(session, image_id, { 0, 0, z }, bb);
        }
        if (vehicle->num_peeps > 6)
        {
            peeps = ((ecx / 8) + 3) & 3;
            image_id = ImageId(
                baseImage_id + ((peeps + 1) * 72), vehicle->peep_tshirt_colours[6], vehicle->peep_tshirt_colours[7]);
            PaintAddImageAsChild(session, image_id, { 0, 0, z }, bb);
        }
    }

    VehicleVisualSplashEffect(session, z, vehicle, carEntry);
}
#endif

/** rct2: 0x00757650 */
static void PaintRiverRapidsTrackFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    ImageId imageId;

    if (direction & 1)
    {
        imageId = session.TrackColours.WithIndex((direction == 1 ? SPR_RIVER_RAPIDS_FLAT_NW_SE : SPR_RIVER_RAPIDS_FLAT_SE_NW));
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 4, 0, height }, { 24, 32, 11 } });

        imageId = session.TrackColours.WithIndex(
            (direction == 1 ? SPR_RIVER_RAPIDS_FLAT_FRONT_NW_SE : SPR_RIVER_RAPIDS_FLAT_FRONT_SE_NW));
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 10, 10, height }, { 1, 2, 5 } });
    }
    else
    {
        imageId = session.TrackColours.WithIndex((direction == 0 ? SPR_RIVER_RAPIDS_FLAT_SW_NE : SPR_RIVER_RAPIDS_FLAT_NE_SW));
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 4, height }, { 32, 24, 11 } });

        imageId = session.TrackColours.WithIndex(
            (direction == 0 ? SPR_RIVER_RAPIDS_FLAT_FRONT_SW_NE : SPR_RIVER_RAPIDS_FLAT_FRONT_NE_SW));
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 10, 10, height }, { 1, 2, 5 } });
    }

    WoodenASupportsPaintSetupRotated(
        session, WoodenSupportType::Truss, WoodenSupportSubType::NeSw, direction, height, session.SupportColours);

    if (direction & 1)
    {
        PaintUtilPushTunnelRight(session, height, TUNNEL_SQUARE_FLAT);
    }
    else
    {
        PaintUtilPushTunnelLeft(session, height, TUNNEL_SQUARE_FLAT);
    }

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

/** rct2: 0x007576C0 */
static void PaintRiverRapidsStation(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    PaintRiverRapidsTrackFlat(session, ride, trackSequence, direction, height, trackElement);
    TrackPaintUtilDrawNarrowStationPlatform(session, ride, direction, height, 12, trackElement);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

static void PaintRiverRapidsTrack25Deg(PaintSession& session, uint8_t direction, int32_t height, const uint32_t sprites[4][2])
{
    ImageId imageId;
    PaintStruct* ps;

    switch (direction)
    {
        case 0:
            imageId = session.TrackColours.WithIndex(sprites[direction][0]);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 4, height }, { 32, 24, 4 } });

            imageId = session.TrackColours.WithIndex(sprites[direction][1]);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 27, height + 16 }, { 32, 1, 34 } });

            WoodenASupportsPaintSetupRotated(
                session, WoodenSupportType::Truss, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
                WoodenSupportTransitionType::Up25Deg);
            PaintUtilPushTunnelLeft(session, height - 8, TUNNEL_SQUARE_7);
            break;

        case 1:
            imageId = session.TrackColours.WithIndex(sprites[direction][0]);
            ps = PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 4, 0, height }, { 24, 32, 4 } });
            session.WoodenSupportsPrependTo = ps;

            imageId = session.TrackColours.WithIndex(sprites[direction][1]);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 27, 0, height + 16 }, { 1, 32, 34 } });

            WoodenASupportsPaintSetupRotated(
                session, WoodenSupportType::Truss, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
                WoodenSupportTransitionType::Up25Deg);
            PaintUtilPushTunnelRight(session, height + 8, TUNNEL_SQUARE_8);
            break;

        case 2:
            imageId = session.TrackColours.WithIndex(sprites[direction][0]);
            ps = PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 4, height }, { 32, 24, 4 } });
            session.WoodenSupportsPrependTo = ps;

            imageId = session.TrackColours.WithIndex(sprites[direction][1]);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 27, height + 16 }, { 32, 1, 34 } });

            WoodenASupportsPaintSetupRotated(
                session, WoodenSupportType::Truss, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
                WoodenSupportTransitionType::Up25Deg);
            PaintUtilPushTunnelLeft(session, height + 8, TUNNEL_SQUARE_8);
            break;

        case 3:
            imageId = session.TrackColours.WithIndex(sprites[direction][0]);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 4, 0, height }, { 24, 32, 4 } });

            imageId = session.TrackColours.WithIndex(sprites[direction][1]);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 27, 0, height + 16 }, { 1, 32, 34 } });

            WoodenASupportsPaintSetupRotated(
                session, WoodenSupportType::Truss, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
                WoodenSupportTransitionType::Up25Deg);
            PaintUtilPushTunnelRight(session, height - 8, TUNNEL_SQUARE_7);
            break;
    }

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
}

static void PaintRiverRapidsTrack25DegToFlatA(
    PaintSession& session, uint8_t direction, int32_t height, const uint32_t sprites[4][2])
{
    ImageId imageId;
    PaintStruct* ps;

    switch (direction)
    {
        case 0:
            imageId = session.TrackColours.WithIndex(sprites[direction][0]);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 4, height }, { 32, 24, 4 } });

            imageId = session.TrackColours.WithIndex(sprites[direction][1]);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 27, height + 16 }, { 32, 1, 18 } });

            WoodenASupportsPaintSetupRotated(
                session, WoodenSupportType::Truss, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
                WoodenSupportTransitionType::Up25DegToFlat);
            PaintUtilPushTunnelLeft(session, height - 8, TUNNEL_SQUARE_FLAT);
            break;

        case 1:
            imageId = session.TrackColours.WithIndex(sprites[direction][0]);
            ps = PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 4, 0, height }, { 24, 32, 4 } });
            session.WoodenSupportsPrependTo = ps;

            imageId = session.TrackColours.WithIndex(sprites[direction][1]);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 27, 0, height + 16 }, { 1, 32, 18 } });

            WoodenASupportsPaintSetupRotated(
                session, WoodenSupportType::Truss, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
                WoodenSupportTransitionType::Up25DegToFlat);
            PaintUtilPushTunnelRight(session, height + 8, TUNNEL_14);
            break;

        case 2:
            imageId = session.TrackColours.WithIndex(sprites[direction][0]);
            ps = PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 4, height }, { 32, 24, 4 } });
            session.WoodenSupportsPrependTo = ps;

            imageId = session.TrackColours.WithIndex(sprites[direction][1]);

            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 27, height }, { 32, 1, 18 } });

            WoodenASupportsPaintSetupRotated(
                session, WoodenSupportType::Truss, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
                WoodenSupportTransitionType::Up25DegToFlat);
            PaintUtilPushTunnelLeft(session, height + 8, TUNNEL_14);
            break;

        case 3:
            imageId = session.TrackColours.WithIndex(sprites[direction][0]);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 4, 0, height }, { 24, 32, 4 } });

            imageId = session.TrackColours.WithIndex(sprites[direction][1]);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 27, 0, height + 16 }, { 1, 32, 18 } });

            WoodenASupportsPaintSetupRotated(
                session, WoodenSupportType::Truss, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
                WoodenSupportTransitionType::Up25DegToFlat);
            PaintUtilPushTunnelRight(session, height - 8, TUNNEL_SQUARE_FLAT);
            break;
    }

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 40, 0x20);
}

static void PaintRiverRapidsTrack25DegToFlatB(
    PaintSession& session, uint8_t direction, int32_t height, const uint32_t sprites[4][2])
{
    ImageId imageId;
    PaintStruct* ps;

    switch (direction)
    {
        case 0:
            imageId = session.TrackColours.WithIndex(sprites[direction][0]);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 4, height }, { 32, 24, 11 } });

            imageId = session.TrackColours.WithIndex(sprites[direction][1]);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 27, height + 16 }, { 32, 1, 26 } });

            WoodenASupportsPaintSetupRotated(
                session, WoodenSupportType::Truss, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
                WoodenSupportTransitionType::FlatToUp25Deg);
            PaintUtilPushTunnelLeft(session, height, TUNNEL_SQUARE_FLAT);
            break;

        case 1:
            imageId = session.TrackColours.WithIndex(sprites[direction][0]);
            ps = PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 4, 0, height }, { 24, 32, 11 } });
            session.WoodenSupportsPrependTo = ps;

            imageId = session.TrackColours.WithIndex(sprites[direction][1]);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 27, 0, height + 16 }, { 1, 32, 26 } });

            WoodenASupportsPaintSetupRotated(
                session, WoodenSupportType::Truss, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
                WoodenSupportTransitionType::FlatToUp25Deg);
            PaintUtilPushTunnelRight(session, height, TUNNEL_SQUARE_8);
            break;

        case 2:
            imageId = session.TrackColours.WithIndex(sprites[direction][0]);
            ps = PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 4, height }, { 32, 24, 11 } });
            session.WoodenSupportsPrependTo = ps;

            imageId = session.TrackColours.WithIndex(sprites[direction][1]);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 27, height + 16 }, { 32, 1, 26 } });

            WoodenASupportsPaintSetupRotated(
                session, WoodenSupportType::Truss, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
                WoodenSupportTransitionType::FlatToUp25Deg);
            PaintUtilPushTunnelLeft(session, height, TUNNEL_SQUARE_8);
            break;

        case 3:
            imageId = session.TrackColours.WithIndex(sprites[direction][0]);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 4, 0, height }, { 24, 32, 11 } });

            imageId = session.TrackColours.WithIndex(sprites[direction][1]);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 27, 0, height + 16 }, { 1, 32, 26 } });

            WoodenASupportsPaintSetupRotated(
                session, WoodenSupportType::Truss, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
                WoodenSupportTransitionType::FlatToUp25Deg);
            PaintUtilPushTunnelRight(session, height, TUNNEL_SQUARE_FLAT);
            break;
    }

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
}

/** rct2: 0x00757660 */
static void PaintRiverRapidsTrack25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    PaintRiverRapidsTrack25Deg(session, direction, height, river_rapids_track_pieces_25_deg_up);
}

/** rct2: 0x00757670 */
static void PaintRiverRapidsTrackFlatTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    PaintRiverRapidsTrack25DegToFlatB(session, direction, height, river_rapids_track_pieces_flat_to_25_deg_up);
}

/** rct2: 0x00757680 */
static void PaintRiverRapidsTrack25DegUpToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    PaintRiverRapidsTrack25DegToFlatA(session, direction, height, river_rapids_track_pieces_25_deg_up_to_flat);
}

/** rct2: 0x00757690 */
static void PaintRiverRapidsTrack25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    PaintRiverRapidsTrack25Deg(session, (direction + 2) % 4, height, river_rapids_track_pieces_25_deg_down);
}

/** rct2: 0x007576A0 */
static void PaintRiverRapidsTrackFlatTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    PaintRiverRapidsTrack25DegToFlatA(session, (direction + 2) % 4, height, river_rapids_track_pieces_flat_to_25_deg_down);
}

/** rct2: 0x007576B0 */
static void PaintRiverRapidsTrack25DegDownToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    PaintRiverRapidsTrack25DegToFlatB(session, (direction + 2) % 4, height, river_rapids_track_pieces_25_deg_down_to_flat);
}

/** rct2: 0x007576F0 */
static void PaintRiverRapidsTrackLeftQuarterTurn1Tile(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    ImageId imageId;
    switch (direction)
    {
        case 0:
            imageId = session.TrackColours.WithIndex(SPR_RIVER_RAPIDS_LEFT_QUARTER_TURN_1_TILE_SW_NW);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 4, 2, height }, { 28, 26, 11 } });

            imageId = session.TrackColours.WithIndex(SPR_RIVER_RAPIDS_LEFT_QUARTER_TURN_1_TILE_FRONT_SW_NW);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 28, 27, height }, { 2, 1, 7 } });

            PaintUtilPushTunnelLeft(session, height, TUNNEL_SQUARE_FLAT);
            break;

        case 1:
            imageId = session.TrackColours.WithIndex(SPR_RIVER_RAPIDS_LEFT_QUARTER_TURN_1_TILE_NW_NE);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 0, height }, { 25, 25, 5 } });

            imageId = session.TrackColours.WithIndex(SPR_RIVER_RAPIDS_QUARTER_TURN_1_TILE_FRONT_LEFT_NW_NE);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 27, 1, height }, { 1, 30, 7 } });

            imageId = session.TrackColours.WithIndex(SPR_RIVER_RAPIDS_QUARTER_TURN_1_TILE_FRONT_RIGHT_NW_NE);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 1, 27, height }, { 30, 1, 7 } });
            break;

        case 2:
            imageId = session.TrackColours.WithIndex(SPR_RIVER_RAPIDS_LEFT_QUARTER_TURN_1_TILE_NE_SE);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 2, 4, height }, { 26, 28, 11 } });

            imageId = session.TrackColours.WithIndex(SPR_RIVER_RAPIDS_LEFT_QUARTER_TURN_1_TILE_FRONT_NE_SE);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 10, 10, height }, { 1, 2, 5 } });

            PaintUtilPushTunnelRight(session, height, TUNNEL_SQUARE_FLAT);
            break;

        case 3:
            imageId = session.TrackColours.WithIndex(SPR_RIVER_RAPIDS_LEFT_QUARTER_TURN_1_TILE_SE_SW);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 4, 4, height }, { 28, 28, 11 } });

            imageId = session.TrackColours.WithIndex(SPR_RIVER_RAPIDS_LEFT_QUARTER_TURN_1_TILE_FRONT_SE_SW);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 28, 28, height }, { 1, 1, 7 } });

            PaintUtilPushTunnelLeft(session, height, TUNNEL_SQUARE_FLAT);
            PaintUtilPushTunnelRight(session, height, TUNNEL_SQUARE_FLAT);
            break;
    }

    WoodenASupportsPaintSetupRotated(
        session, WoodenSupportType::Truss, WoodenSupportSubType::NeSw, direction, height, session.SupportColours);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

/** rct2: 0x00757700 */
static void PaintRiverRapidsTrackRightQuarterTurn1Tile(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    ImageId imageId;

    switch (direction)
    {
        case 0:
            imageId = session.TrackColours.WithIndex(SPR_RIVER_RAPIDS_RIGHT_QUARTER_TURN_1_TILE_SW_SE);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 4, 4, height }, { 28, 28, 11 } });

            imageId = session.TrackColours.WithIndex(SPR_RIVER_RAPIDS_RIGHT_QUARTER_TURN_1_TILE_FRONT_SW_SE);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 28, 28, height + 2 }, { 1, 1, 7 } });

            PaintUtilPushTunnelLeft(session, height, TUNNEL_SQUARE_FLAT);
            PaintUtilPushTunnelRight(session, height, TUNNEL_SQUARE_FLAT);
            break;

        case 1:
            imageId = session.TrackColours.WithIndex(SPR_RIVER_RAPIDS_RIGHT_QUARTER_TURN_1_TILE_SE_NE);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 4, 2, height }, { 28, 26, 11 } });

            imageId = session.TrackColours.WithIndex(SPR_RIVER_RAPIDS_RIGHT_QUARTER_TURN_1_TILE_FRONT_SE_NE);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 28, 27, height }, { 2, 1, 7 } });

            PaintUtilPushTunnelLeft(session, height, TUNNEL_SQUARE_FLAT);
            break;

        case 2:
            imageId = session.TrackColours.WithIndex(SPR_RIVER_RAPIDS_RIGHT_QUARTER_TURN_1_TILE_NE_NW);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 0, height }, { 28, 28, 11 } });

            imageId = session.TrackColours.WithIndex(SPR_RIVER_RAPIDS_QUARTER_TURN_1_TILE_FRONT_LEFT_NW_NE);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 27, 1, height }, { 1, 22, 7 } });

            imageId = session.TrackColours.WithIndex(SPR_RIVER_RAPIDS_QUARTER_TURN_1_TILE_FRONT_RIGHT_NW_NE);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 1, 27, height }, { 22, 1, 7 } });
            break;

        case 3:
            imageId = session.TrackColours.WithIndex(SPR_RIVER_RAPIDS_RIGHT_QUARTER_TURN_1_TILE_NW_SW);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 2, 4, height }, { 26, 28, 11 } });

            imageId = session.TrackColours.WithIndex(SPR_RIVER_RAPIDS_RIGHT_QUARTER_TURN_1_TILE_FRONT_NW_SW);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 27, 2, height }, { 1, 28, 7 } });

            PaintUtilPushTunnelRight(session, height, TUNNEL_SQUARE_FLAT);
            break;
    }

    WoodenASupportsPaintSetupRotated(
        session, WoodenSupportType::Truss, WoodenSupportSubType::NwSe, direction, height, session.SupportColours);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

/** rct2: 0x00757710 */
static void PaintRiverRapidsTrackWaterfall(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    ImageId imageId;

    uint16_t frameNum = (GetGameState().CurrentTicks / 2) & 7;

    if (direction & 1)
    {
        imageId = session.TrackColours.WithIndex(
            (direction == 1 ? SPR_RIVER_RAPIDS_WATERFALL_NW_SE : SPR_RIVER_RAPIDS_WATERFALL_SE_NW));
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 4, 0, height }, { 24, 32, 11 } });

        imageId = session.TrackColours.WithIndex((SPR_RIVER_RAPIDS_WATERFALL_BASE_NE_FRAME_0 + frameNum));
        PaintAddImageAsChild(session, imageId, { 0, 0, height }, { { 4, 0, height }, { 24, 32, 11 } });

        imageId = session.TrackColours.WithIndex((SPR_RIVER_RAPIDS_WATERFALL_TOP_NE_FRAME_0 + frameNum));
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 4, 0, height + 17 }, { 1, 32, 27 } });

        imageId = session.TrackColours.WithIndex(
            (direction == 1 ? SPR_RIVER_RAPIDS_WATERFALL_FRONT_NW_SE : SPR_RIVER_RAPIDS_WATERFALL_FRONT_SE_NW));
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 27, 0, height + 17 }, { 1, 32, 27 } });

        imageId = session.TrackColours.WithIndex((SPR_RIVER_RAPIDS_WATERFALL_SIDE_SW_FRAME_0 + frameNum));
        PaintAddImageAsChild(session, imageId, { 0, 0, height }, { { 27, 0, height + 17 }, { 1, 32, 27 } });
    }
    else
    {
        imageId = session.TrackColours.WithIndex(
            (direction == 0 ? SPR_RIVER_RAPIDS_WATERFALL_SW_NE : SPR_RIVER_RAPIDS_WATERFALL_NE_SW));

        auto bb = BoundBoxXYZ{ { 0, 4, height }, { 32, 24, 11 } };
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, bb);

        imageId = session.TrackColours.WithIndex((SPR_RIVER_RAPIDS_WATERFALL_BASE_NW_FRAME_0 + frameNum));
        PaintAddImageAsChild(session, imageId, { 0, 0, height }, bb);

        bb.offset = { 0, 4, height + 17 };
        bb.length = { 32, 1, 27 };

        imageId = session.TrackColours.WithIndex((SPR_RIVER_RAPIDS_WATERFALL_TOP_NW_FRAME_0 + frameNum));
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, bb);

        bb.offset = { 0, 27, height + 17 };

        imageId = session.TrackColours.WithIndex(
            (direction == 0 ? SPR_RIVER_RAPIDS_WATERFALL_FRONT_SW_NE : SPR_RIVER_RAPIDS_WATERFALL_FRONT_NE_SW));
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, bb);

        imageId = session.TrackColours.WithIndex((SPR_RIVER_RAPIDS_WATERFALL_SIDE_SE_FRAME_0 + frameNum));
        PaintAddImageAsChild(session, imageId, { 0, 0, height }, bb);
    }

    WoodenASupportsPaintSetupRotated(
        session, WoodenSupportType::Truss, WoodenSupportSubType::NeSw, direction, height, session.SupportColours);

    if (direction & 1)
    {
        PaintUtilPushTunnelRight(session, height, TUNNEL_SQUARE_INVERTED_9);
    }
    else
    {
        PaintUtilPushTunnelLeft(session, height, TUNNEL_SQUARE_INVERTED_9);
    }

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
}

/** rct2: 0x00757720 */
static void PaintRiverRapidsTrackRapids(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    ImageId imageId;

    uint16_t frameNum = (GetGameState().CurrentTicks / 2) & 7;

    if (direction & 1)
    {
        imageId = session.TrackColours.WithIndex((SPR_RIVER_RAPIDS_RAPIDS_NW_SE_FRAME_0 + frameNum));
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 4, 0, height }, { 24, 32, 11 } });

        imageId = session.TrackColours.WithIndex(SPR_RIVER_RAPIDS_RAPIDS_FRONT_NW_SE);
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 27, 0, height + 17 }, { 1, 32, 11 } });
    }
    else
    {
        imageId = session.TrackColours.WithIndex((SPR_RIVER_RAPIDS_RAPIDS_SW_NE_FRAME_0 + frameNum));
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 4, height }, { 32, 24, 11 } });

        imageId = session.TrackColours.WithIndex(SPR_RIVER_RAPIDS_RAPIDS_FRONT_SW_NE);
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 27, height + 17 }, { 32, 1, 11 } });
    }

    WoodenASupportsPaintSetupRotated(
        session, WoodenSupportType::Truss, WoodenSupportSubType::NeSw, direction, height, session.SupportColours);

    if (direction & 1)
    {
        PaintUtilPushTunnelRight(session, height, TUNNEL_SQUARE_FLAT);
    }
    else
    {
        PaintUtilPushTunnelLeft(session, height, TUNNEL_SQUARE_FLAT);
    }

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

/** rct2: 0x00757740 */
static void PaintRiverRapidsTrackOnRidePhoto(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    PaintRiverRapidsTrackFlat(session, ride, trackSequence, direction, height, trackElement);
    TrackPaintUtilOnridePhotoPaint(session, direction, height + 13, trackElement);
    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
}

/** rct2: 0x */
static void PaintRiverRapidsTrackWhirlpool(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    ImageId imageId;

    uint8_t frameNum = (GetGameState().CurrentTicks / 4) % 16;

    if (direction & 1)
    {
        imageId = session.TrackColours.WithIndex((direction == 1 ? SPR_RIVER_RAPIDS_FLAT_NW_SE : SPR_RIVER_RAPIDS_FLAT_SE_NW));

        auto bb = BoundBoxXYZ{ { 4, 0, height }, { 24, 32, 11 } };
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, bb);

        imageId = session.TrackColours.WithIndex((SPR_RIVER_RAPIDS_RAPIDS_WHIRLPOOL_FRAME_0 + frameNum));
        PaintAddImageAsChild(session, imageId, { 0, 0, height }, bb);

        bb.offset = { 10, 10, height };
        bb.length = { 1, 2, 5 };

        imageId = session.TrackColours.WithIndex(
            (direction == 1 ? SPR_RIVER_RAPIDS_FLAT_FRONT_NW_SE : SPR_RIVER_RAPIDS_FLAT_FRONT_SE_NW));
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, bb);
    }
    else
    {
        imageId = session.TrackColours.WithIndex((direction == 0 ? SPR_RIVER_RAPIDS_FLAT_SW_NE : SPR_RIVER_RAPIDS_FLAT_NE_SW));

        auto bb = BoundBoxXYZ{ { 0, 4, height }, { 32, 24, 11 } };
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, bb);

        imageId = session.TrackColours.WithIndex((SPR_RIVER_RAPIDS_RAPIDS_WHIRLPOOL_FRAME_0 + frameNum));
        PaintAddImageAsChild(session, imageId, { 0, 0, height }, bb);

        bb.offset = { 10, 10, height };
        bb.length = { 1, 2, 5 };

        imageId = session.TrackColours.WithIndex(
            (direction == 0 ? SPR_RIVER_RAPIDS_FLAT_FRONT_SW_NE : SPR_RIVER_RAPIDS_FLAT_FRONT_NE_SW));
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, bb);
    }

    WoodenASupportsPaintSetupRotated(
        session, WoodenSupportType::Truss, WoodenSupportSubType::NeSw, direction, height, session.SupportColours);

    if (direction & 1)
    {
        PaintUtilPushTunnelRight(session, height, TUNNEL_SQUARE_FLAT);
    }
    else
    {
        PaintUtilPushTunnelLeft(session, height, TUNNEL_SQUARE_FLAT);
    }

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

/**
 * rct2: 0x0075745C
 **/
TRACK_PAINT_FUNCTION GetTrackPaintFunctionRiverRapids(int32_t trackType)
{
    switch (trackType)
    {
        case TrackElemType::kFlat:
            return PaintRiverRapidsTrackFlat;

        case TrackElemType::kEndStation:
        case TrackElemType::kBeginStation:
        case TrackElemType::kMiddleStation:
            return PaintRiverRapidsStation;

        case TrackElemType::kUp25:
            return PaintRiverRapidsTrack25DegUp;
        case TrackElemType::kFlatToUp25:
            return PaintRiverRapidsTrackFlatTo25DegUp;
        case TrackElemType::kUp25ToFlat:
            return PaintRiverRapidsTrack25DegUpToFlat;

        case TrackElemType::kDown25:
            return PaintRiverRapidsTrack25DegDown;
        case TrackElemType::kFlatToDown25:
            return PaintRiverRapidsTrackFlatTo25DegDown;
        case TrackElemType::kDown25ToFlat:
            return PaintRiverRapidsTrack25DegDownToFlat;

        case TrackElemType::kLeftQuarterTurn1Tile:
            return PaintRiverRapidsTrackLeftQuarterTurn1Tile;
        case TrackElemType::kRightQuarterTurn1Tile:
            return PaintRiverRapidsTrackRightQuarterTurn1Tile;

        case TrackElemType::kWaterfall:
            return PaintRiverRapidsTrackWaterfall;

        case TrackElemType::kRapids:
            return PaintRiverRapidsTrackRapids;

        case TrackElemType::kOnRidePhoto:
            return PaintRiverRapidsTrackOnRidePhoto;

        case TrackElemType::kWhirlpool:
            return PaintRiverRapidsTrackWhirlpool;
    }

    return nullptr;
}
