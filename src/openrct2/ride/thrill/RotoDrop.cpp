/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
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
#include "../Ride.h"
#include "../Track.h"
#include "../TrackPaint.h"
#include "../Vehicle.h"
#include "../VehiclePaint.h"

#include <cstring>

enum
{
    SPR_ROTO_DROP_TOWER_SEGMENT = 14558,
    SPR_ROTO_DROP_TOWER_SEGMENT_TOP = 14559,
    SPR_ROTO_DROP_TOWER_BASE = 14560,
    SPR_ROTO_DROP_TOWER_BASE_SEGMENT = 14561,
    SPR_ROTO_DROP_TOWER_BASE_90_DEG = 14562,
    SPR_ROTO_DROP_TOWER_BASE_SEGMENT_90_DEG = 14563,
};

/**
 *
 *  rct2: 0x006D5DA9
 */
void VehicleVisualRotoDrop(
    PaintSession& session, int32_t x, int32_t imageDirection, int32_t y, int32_t z, const Vehicle* vehicle,
    const CarEntry* carEntry)
{
    imageDirection = OpenRCT2::Entity::Yaw::YawTo32(imageDirection);

    auto imageFlags = ImageId(0, vehicle->colours.Body, vehicle->colours.Trim);
    if (vehicle->IsGhost())
    {
        imageFlags = ConstructionMarker;
    }

    ImageId image_id;
    int32_t baseImage_id = (carEntry->base_image_id + 4) + ((vehicle->animation_frame / 4) & 0x3);
    if (vehicle->restraints_position >= 64)
    {
        baseImage_id += 7;
        baseImage_id += (vehicle->restraints_position / 64);
    }

    // Draw back:
    image_id = imageFlags.WithIndex(baseImage_id);
    PaintAddImageAsParent(session, image_id, { 0, 0, z }, { { -11, -11, z + 1 }, { 2, 2, 41 } });

    // Draw front:
    image_id = imageFlags.WithIndex(baseImage_id + 4);
    PaintAddImageAsParent(session, image_id, { 0, 0, z }, { { -5, -5, z + 1 }, { 16, 16, 41 } });

    if (vehicle->num_peeps > 0 && !vehicle->IsGhost())
    {
        uint8_t riding_peep_sprites[64];
        std::fill_n(riding_peep_sprites, sizeof(riding_peep_sprites), 0xFF);
        for (int32_t i = 0; i < vehicle->num_peeps; i++)
        {
            uint8_t cl = (i & 3) * 16;
            cl += (i & 0xFC);
            cl += vehicle->animation_frame / 4;
            cl += (imageDirection / 8) * 16;
            cl &= 0x3F;
            riding_peep_sprites[cl] = vehicle->peep_tshirt_colours[i];
        }

        // Draw riding peep sprites in back to front order:
        for (int32_t j = 0; j <= 48; j++)
        {
            int32_t i = (j % 2) ? (48 - (j / 2)) : (j / 2);
            if (riding_peep_sprites[i] != 0xFF)
            {
                baseImage_id = carEntry->base_image_id + 20 + i;
                if (vehicle->restraints_position >= 64)
                {
                    baseImage_id += 64;
                    baseImage_id += vehicle->restraints_position / 64;
                }
                image_id = ImageId(baseImage_id, riding_peep_sprites[i]);
                PaintAddImageAsChild(session, image_id, { 0, 0, z }, { { -5, -5, z + 1 }, { 16, 16, 41 } });
            }
        }
    }

    assert(carEntry->effect_visual == 1);
    // Although called in original code, effect_visual (splash effects) are not used for many rides and does not make sense so
    // it was taken out
}

/** rct2: 0x00886194 */
static void PaintRotoDropBase(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = track_map_3x3[direction][trackSequence];

    int32_t edges = edges_3x3[trackSequence];

    WoodenASupportsPaintSetup(session, (direction & 1), 0, height, session.TrackColours[SCHEME_MISC]);

    const StationObject* stationObject = ride.GetStationObject();

    TrackPaintUtilPaintFloor(session, edges, session.TrackColours[SCHEME_SUPPORTS], height, floorSpritesMetalB, stationObject);

    TrackPaintUtilPaintFences(
        session, edges, session.MapPosition, trackElement, ride, session.TrackColours[SCHEME_TRACK], height, fenceSpritesMetalB,
        session.CurrentRotation);

    if (trackSequence == 0)
    {
        auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex(
            (direction & 1 ? SPR_ROTO_DROP_TOWER_BASE_90_DEG : SPR_ROTO_DROP_TOWER_BASE));
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 8, 8, height + 3 }, { 2, 2, 27 } });

        imageId = session.TrackColours[SCHEME_TRACK].WithIndex(
            (direction & 1 ? SPR_ROTO_DROP_TOWER_BASE_SEGMENT_90_DEG : SPR_ROTO_DROP_TOWER_BASE_SEGMENT));
        PaintAddImageAsParent(session, imageId, { 0, 0, height + 32 }, { { 8, 8, height + 32 }, { 2, 2, 30 } });

        imageId = session.TrackColours[SCHEME_TRACK].WithIndex(
            (direction & 1 ? SPR_ROTO_DROP_TOWER_BASE_SEGMENT_90_DEG : SPR_ROTO_DROP_TOWER_BASE_SEGMENT));
        PaintAddImageAsParent(session, imageId, { 0, 0, height + 64 }, { { 8, 8, height + 64 }, { 2, 2, 30 } });

        PaintUtilSetVerticalTunnel(session, height + 96);
        PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);

        PaintUtilSetGeneralSupportHeight(session, height + 96, 0x20);

        return;
    }

    int32_t blockedSegments = 0;
    switch (trackSequence)
    {
        case 1:
            blockedSegments = SEGMENT_B8 | SEGMENT_C8 | SEGMENT_B4 | SEGMENT_CC | SEGMENT_BC;
            break;
        case 2:
            blockedSegments = SEGMENT_B4 | SEGMENT_CC | SEGMENT_BC;
            break;
        case 3:
            blockedSegments = SEGMENT_B4 | SEGMENT_CC | SEGMENT_BC | SEGMENT_D4 | SEGMENT_C0;
            break;
        case 4:
            blockedSegments = SEGMENT_B4 | SEGMENT_C8 | SEGMENT_B8;
            break;
        case 5:
            blockedSegments = SEGMENT_BC | SEGMENT_D4 | SEGMENT_C0;
            break;
        case 6:
            blockedSegments = SEGMENT_B4 | SEGMENT_C8 | SEGMENT_B8 | SEGMENT_D0 | SEGMENT_C0;
            break;
        case 7:
            blockedSegments = SEGMENT_B8 | SEGMENT_D0 | SEGMENT_C0 | SEGMENT_D4 | SEGMENT_BC;
            break;
        case 8:
            blockedSegments = SEGMENT_B8 | SEGMENT_D0 | SEGMENT_C0;
            break;
    }
    PaintUtilSetSegmentSupportHeight(session, blockedSegments, 0xFFFF, 0);
    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL & ~blockedSegments, height + 2, 0x20);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

/** rct2: 0x008861A4 */
static void PaintRotoDropTowerSection(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackSequence == 1)
    {
        return;
    }

    auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex(SPR_ROTO_DROP_TOWER_SEGMENT);
    PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 8, 8, height }, { 2, 2, 30 } });

    const TileElement* nextTileElement = reinterpret_cast<const TileElement*>(&trackElement) + 1;

    while (nextTileElement->GetType() != TileElementType::Track && !nextTileElement->IsLastForTile())
    {
        nextTileElement++;
    }

    if (trackElement.IsLastForTile() || trackElement.GetClearanceZ() != nextTileElement->GetBaseZ())
    {
        imageId = session.TrackColours[SCHEME_TRACK].WithIndex(SPR_ROTO_DROP_TOWER_SEGMENT_TOP);
        PaintAddImageAsChild(session, imageId, { 0, 0, height }, { { 8, 8, height }, { 2, 2, 30 } });
    }

    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);

    PaintUtilSetVerticalTunnel(session, height + 32);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

/**
 * rct2: 0x00886074
 */
TRACK_PAINT_FUNCTION GetTrackPaintFunctionRotoDrop(int32_t trackType)
{
    switch (trackType)
    {
        case TrackElemType::TowerBase:
            return PaintRotoDropBase;

        case TrackElemType::TowerSection:
            return PaintRotoDropTowerSection;
    }

    return nullptr;
}
