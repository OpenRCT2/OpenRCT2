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
#include "../../world/Map.h"
#include "../Ride.h"
#include "../Track.h"
#include "../TrackPaint.h"
#include "../Vehicle.h"
#include "../VehiclePaint.h"

enum
{
    SprObservationTowerSegmentBase = 14986,
    SprObservationTowerSegment = 14987,
    SprObservationTowerSegmentTop = 14988,
};

static uint32_t GetObservationTowerVehicleBaseImageId(const Vehicle* vehicle, const CarEntry* carEntry, int32_t imageDirection)
{
    uint32_t result = (vehicle->restraints_position / 64);
    if (vehicle->restraints_position >= 64)
    {
        auto directionOffset = imageDirection / 8;
        if ((directionOffset == 0) || (directionOffset == 3))
        {
            result = carEntry->base_image_id + 8;
        }
        else
        {
            result *= 2;
            result += carEntry->base_image_id;
            if (directionOffset == 1)
            {
                result += 28;
            }
            else
            {
                result += 22;
            }
        }
    }
    else
    {
        result = (vehicle->animation_frame * 2) + carEntry->base_image_id + 8;
    }
    return result;
}

/**
 *
 *  rct2: 0x006D6258
 */
void VehicleVisualObservationTower(
    PaintSession& session, int32_t x, int32_t imageDirection, int32_t y, int32_t z, const Vehicle* vehicle,
    const CarEntry* carEntry)
{
    auto baseImageId = GetObservationTowerVehicleBaseImageId(vehicle, carEntry, imageDirection);
    auto imageId0 = ImageId(baseImageId + 0, vehicle->colours.Body, vehicle->colours.Trim, vehicle->colours.Tertiary);
    auto imageId1 = ImageId(baseImageId + 1, vehicle->colours.Body, vehicle->colours.Trim, vehicle->colours.Tertiary);
    if (vehicle->IsGhost())
    {
        imageId0 = ImageId(baseImageId + 0).WithRemap(FilterPaletteID::Palette44);
        imageId1 = ImageId(baseImageId + 1).WithRemap(FilterPaletteID::Palette44);
    }

    PaintAddImageAsParent(session, imageId0, { 0, 0, z }, { 2, 2, 41 }, { -11, -11, z + 1 });
    PaintAddImageAsParent(session, imageId1, { 0, 0, z }, { 16, 16, 41 }, { -5, -5, z + 1 });
    assert(carEntry->effect_visual == 1);
}

/** rct2: 0x0070DD6C */
static void PaintObservationTowerBase(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = track_map_3x3[direction][trackSequence];

    int32_t edges = edges_3x3[trackSequence];
    CoordsXY position = session.MapPosition;

    WoodenASupportsPaintSetup(session, (direction & 1), 0, height, session.TrackColours[SCHEME_MISC]);

    const StationObject* stationObject = ride.GetStationObject();

    TrackPaintUtilPaintFloor(session, edges, session.TrackColours[SCHEME_SUPPORTS], height, floorSpritesMetalB, stationObject);

    TrackPaintUtilPaintFences(
        session, edges, position, trackElement, ride, session.TrackColours[SCHEME_TRACK], height, fenceSpritesMetalB,
        session.CurrentRotation);

    if (trackSequence == 0)
    {
        auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex(SprObservationTowerSegmentBase);
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 2, 2, 27 }, { 8, 8, height + 3 });

        imageId = session.TrackColours[SCHEME_TRACK].WithIndex(SprObservationTowerSegment);
        PaintAddImageAsParent(session, imageId, { 0, 0, height + 32 }, { 2, 2, 30 }, { 8, 8, height + 32 });

        imageId = session.TrackColours[SCHEME_TRACK].WithIndex(SprObservationTowerSegment);
        PaintAddImageAsParent(session, imageId, { 0, 0, height + 64 }, { 2, 2, 30 }, { 8, 8, height + 64 });

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

/** rct2: 0x0070DD7C */
static void PaintObservationTowerSection(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackSequence == 1)
    {
        return;
    }

    auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex(SprObservationTowerSegment);
    PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 2, 2, 30 }, { 8, 8, height });

    const TileElement* nextTileElement = reinterpret_cast<const TileElement*>(&trackElement) + 1;
    if (trackElement.IsLastForTile() || trackElement.GetClearanceZ() != nextTileElement->GetBaseZ())
    {
        imageId = session.TrackColours[SCHEME_TRACK].WithIndex(SprObservationTowerSegmentTop);
        PaintAddImageAsChild(session, imageId, { 0, 0, height }, { { 8, 8, height }, { 2, 2, 30 } });
    }

    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);

    PaintUtilSetVerticalTunnel(session, height + 32);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

/**
 * rct2: 0x0070DC5C
 */
TRACK_PAINT_FUNCTION GetTrackPaintFunctionObservationTower(int32_t trackType)
{
    switch (trackType)
    {
        case TrackElemType::TowerBase:
            return PaintObservationTowerBase;

        case TrackElemType::TowerSection:
            return PaintObservationTowerSection;
    }

    return nullptr;
}
