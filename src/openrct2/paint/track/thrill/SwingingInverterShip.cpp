/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../../entity/EntityRegistry.h"
#include "../../../interface/Viewport.h"
#include "../../../object/StationObject.h"
#include "../../../ride/Ride.h"
#include "../../../ride/RideEntry.h"
#include "../../../ride/Track.h"
#include "../../../ride/TrackPaint.h"
#include "../../../ride/Vehicle.h"
#include "../../Boundbox.h"
#include "../../Paint.h"
#include "../../support/WoodenSupports.h"
#include "../../tile_element/Segment.h"
#include "../../track/Segment.h"

using namespace OpenRCT2;

/** rct2: 0x01428010 */
static constexpr uint32_t kSwingingInverterShipBaseSpriteOffset[] = {
    0,
    16,
    0,
    16,
};

/** rct2: 0x01428020 */
static constexpr uint32_t kSwingingInverterShipAnimatingBaseSpriteOffset[] = {
    32,
    33,
    32,
    33,
};

/** rct2: 0x01428020 */
static constexpr BoundBoxXY kSwingingInverterShipBounds[] = {
    { { 0, 8 }, { 32, 16 } },
    { { 8, 0 }, { 16, 32 } },
    { { 0, 8 }, { 32, 16 } },
    { { 8, 0 }, { 16, 32 } },
};

enum
{
    SPR_SWINGING_INVERTER_SHIP_FRAME_0 = 21998,
    SPR_SWINGING_INVERTER_SHIP_FRAME_1 = 21999,
    SPR_SWINGING_INVERTER_SHIP_FRAME_2 = 22000,
    SPR_SWINGING_INVERTER_SHIP_FRAME_3 = 22001,
};

static constexpr uint32_t kSwingingInverterShipFrameSprites[] = {
    SPR_SWINGING_INVERTER_SHIP_FRAME_0,
    SPR_SWINGING_INVERTER_SHIP_FRAME_1,
    SPR_SWINGING_INVERTER_SHIP_FRAME_2,
    SPR_SWINGING_INVERTER_SHIP_FRAME_3,
};

static void PaintSwingingInverterShipStructure(
    PaintSession& session, const Ride& ride, uint8_t direction, int8_t axisOffset, uint16_t height, ImageId stationColour)
{
    const auto* rideEntry = GetRideEntryByIndex(ride.subtype);
    if (rideEntry == nullptr)
        return;

    const auto& boundBox = kSwingingInverterShipBounds[direction];
    CoordsXYZ offset((direction & 1) ? 0 : axisOffset, (direction & 1) ? axisOffset : 0, height);
    BoundBoxXYZ bb = { { boundBox.offset, height }, { boundBox.length, 127 } };

    Vehicle* vehicle = nullptr;
    if (ride.lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK)
    {
        vehicle = GetEntity<Vehicle>(ride.vehicles[0]);
        if (vehicle != nullptr)
        {
            session.InteractionType = ViewportInteractionItem::Entity;
            session.CurrentlyDrawnEntity = vehicle;
        }
    }

    ImageIndex vehicleImageIndex = rideEntry->Cars[0].base_image_id + kSwingingInverterShipBaseSpriteOffset[direction];
    if (vehicle != nullptr)
    {
        int32_t rotation = static_cast<int8_t>(vehicle->Pitch);
        if (rotation != 0)
        {
            vehicleImageIndex = rideEntry->Cars[0].base_image_id + kSwingingInverterShipAnimatingBaseSpriteOffset[direction];
            if (direction & 2)
            {
                rotation = -rotation;
            }
            if (rotation < 0)
            {
                rotation += 72;
            }
            vehicleImageIndex += (rotation - 1) << 1;
        }
    }

    auto vehicleImageTemplate = ImageId(0, ride.vehicle_colours[0].Body, ride.vehicle_colours[0].Trim);
    if (stationColour != TrackStationColour)
    {
        vehicleImageTemplate = stationColour;
    }
    auto frameImageTemplate = session.TrackColours;
    auto vehicleImageId = vehicleImageTemplate.WithIndex(vehicleImageIndex);
    auto frameImageId = frameImageTemplate.WithIndex(kSwingingInverterShipFrameSprites[direction]);

    if (direction & 2)
    {
        PaintAddImageAsParent(session, vehicleImageId, offset, bb);
        PaintAddImageAsChild(session, frameImageId, offset, bb);
    }
    else
    {
        PaintAddImageAsParent(session, frameImageId, offset, bb);
        PaintAddImageAsChild(session, vehicleImageId, offset, bb);
    }

    session.CurrentlyDrawnEntity = nullptr;
    session.InteractionType = ViewportInteractionItem::Ride;
}

static void PaintSwingingInverterShip(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    uint8_t relativeTrackSequence = kTrackMap1x4[direction][trackSequence];
    ImageId imageId;

    const StationObject* stationObject = ride.GetStationObject();

    if (relativeTrackSequence != 1 && relativeTrackSequence != 3)
    {
        DrawSupportsSideBySide(session, direction, height, session.SupportColours, MetalSupportType::Tubes);

        if (stationObject != nullptr && !(stationObject->Flags & STATION_OBJECT_FLAGS::NO_PLATFORMS))
        {
            imageId = session.SupportColours.WithIndex(SPR_STATION_BASE_D);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 32, 32, 1 });

            switch (direction)
            {
                case 0:
                    imageId = session.TrackColours.WithIndex(SPR_STATION_PLATFORM_SW_NE);
                    PaintAddImageAsParent(session, imageId, { 0, 24, height + 9 }, { 32, 8, 1 });
                    break;
                case 1:
                    imageId = session.TrackColours.WithIndex(SPR_STATION_PLATFORM_NW_SE);
                    PaintAddImageAsParent(session, imageId, { 24, 0, height + 9 }, { 8, 32, 1 });
                    break;
                case 2:
                    imageId = session.TrackColours.WithIndex(SPR_STATION_PLATFORM_SW_NE);
                    PaintAddImageAsChild(session, imageId, { 0, 0, height + 9 }, { { -2, 0, height }, { 32, 8, 1 } });
                    break;
                case 3:
                    imageId = session.TrackColours.WithIndex(SPR_STATION_PLATFORM_NW_SE);
                    PaintAddImageAsChild(session, imageId, { 0, 0, height + 9 }, { { 0, -2, height }, { 8, 32, 1 } });
                    break;
            }
        }
    }

    auto stationColour = GetStationColourScheme(session, trackElement);
    switch (relativeTrackSequence)
    {
        case 1:
            PaintSwingingInverterShipStructure(session, ride, direction, 48, height + 7, stationColour);
            break;
        case 2:
            PaintSwingingInverterShipStructure(session, ride, direction, 16, height + 7, stationColour);
            break;
        case 0:
            PaintSwingingInverterShipStructure(session, ride, direction, -16, height + 7, stationColour);
            break;
        case 3:
            PaintSwingingInverterShipStructure(session, ride, direction, -48, height + 7, stationColour);
            break;
    }

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 176);
}

TrackPaintFunction GetTrackPaintFunctionSwingingInverterShip(OpenRCT2::TrackElemType trackType)
{
    if (trackType != TrackElemType::FlatTrack1x4B)
    {
        return TrackPaintFunctionDummy;
    }

    return PaintSwingingInverterShip;
}
