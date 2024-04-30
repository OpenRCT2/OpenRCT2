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
        imageId0 = ImageId(baseImageId + 0).WithRemap(FilterPaletteID::PaletteGhost);
        imageId1 = ImageId(baseImageId + 1).WithRemap(FilterPaletteID::PaletteGhost);
    }

    PaintAddImageAsParent(session, imageId0, { 0, 0, z }, { { -11, -11, z + 1 }, { 2, 2, 41 } });
    PaintAddImageAsParent(session, imageId1, { 0, 0, z }, { { -5, -5, z + 1 }, { 16, 16, 41 } });
    assert(carEntry->effect_visual == 1);
}

/** rct2: 0x0070DD6C */
static void PaintObservationTowerBase(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = kTrackMap3x3[direction][trackSequence];

    int32_t edges = kEdges3x3[trackSequence];
    CoordsXY position = session.MapPosition;

    WoodenASupportsPaintSetupRotated(
        session, WoodenSupportType::Truss, WoodenSupportSubType::NeSw, direction, height,
        GetStationColourScheme(session, trackElement));

    const StationObject* stationObject = ride.GetStationObject();

    TrackPaintUtilPaintFloor(session, edges, session.SupportColours, height, kFloorSpritesMetalB, stationObject);

    TrackPaintUtilPaintFences(
        session, edges, position, trackElement, ride, session.TrackColours, height, fenceSpritesMetalB,
        session.CurrentRotation);

    if (trackSequence == 0)
    {
        auto imageId = session.TrackColours.WithIndex(SprObservationTowerSegmentBase);
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 8, 8, height + 3 }, { 2, 2, 27 } });

        imageId = session.TrackColours.WithIndex(SprObservationTowerSegment);
        PaintAddImageAsParent(session, imageId, { 0, 0, height + 32 }, { { 8, 8, height + 32 }, { 2, 2, 30 } });

        imageId = session.TrackColours.WithIndex(SprObservationTowerSegment);
        PaintAddImageAsParent(session, imageId, { 0, 0, height + 64 }, { { 8, 8, height + 64 }, { 2, 2, 30 } });

        PaintUtilSetVerticalTunnel(session, height + 96);
        PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);

        PaintUtilSetGeneralSupportHeight(session, height + 96, 0x20);

        return;
    }

    int32_t blockedSegments = 0;
    switch (trackSequence)
    {
        case 1:
            blockedSegments = EnumsToFlags(
                PaintSegment::leftCorner, PaintSegment::topLeftSide, PaintSegment::topCorner, PaintSegment::topRightSide,
                PaintSegment::rightCorner);
            break;
        case 2:
            blockedSegments = EnumsToFlags(PaintSegment::topCorner, PaintSegment::topRightSide, PaintSegment::rightCorner);
            break;
        case 3:
            blockedSegments = EnumsToFlags(
                PaintSegment::topCorner, PaintSegment::topRightSide, PaintSegment::rightCorner, PaintSegment::bottomRightSide,
                PaintSegment::bottomCorner);
            break;
        case 4:
            blockedSegments = EnumsToFlags(PaintSegment::topCorner, PaintSegment::topLeftSide, PaintSegment::leftCorner);
            break;
        case 5:
            blockedSegments = EnumsToFlags(
                PaintSegment::rightCorner, PaintSegment::bottomRightSide, PaintSegment::bottomCorner);
            break;
        case 6:
            blockedSegments = EnumsToFlags(
                PaintSegment::topCorner, PaintSegment::topLeftSide, PaintSegment::leftCorner, PaintSegment::bottomLeftSide,
                PaintSegment::bottomCorner);
            break;
        case 7:
            blockedSegments = EnumsToFlags(
                PaintSegment::leftCorner, PaintSegment::bottomLeftSide, PaintSegment::bottomCorner,
                PaintSegment::bottomRightSide, PaintSegment::rightCorner);
            break;
        case 8:
            blockedSegments = EnumsToFlags(PaintSegment::leftCorner, PaintSegment::bottomLeftSide, PaintSegment::bottomCorner);
            break;
    }
    PaintUtilSetSegmentSupportHeight(session, blockedSegments, 0xFFFF, 0);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll & ~blockedSegments, height + 2, 0x20);
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

    auto imageId = session.TrackColours.WithIndex(SprObservationTowerSegment);
    PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 8, 8, height }, { 2, 2, 30 } });

    const TileElement* nextTileElement = reinterpret_cast<const TileElement*>(&trackElement) + 1;
    if (trackElement.IsLastForTile() || trackElement.GetClearanceZ() != nextTileElement->GetBaseZ())
    {
        imageId = session.TrackColours.WithIndex(SprObservationTowerSegmentTop);
        PaintAddImageAsChild(session, imageId, { 0, 0, height }, { { 8, 8, height }, { 2, 2, 30 } });
    }

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);

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
