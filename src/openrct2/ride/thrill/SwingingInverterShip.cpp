/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../entity/EntityRegistry.h"
#include "../../interface/Viewport.h"
#include "../../object/StationObject.h"
#include "../../paint/Boundbox.h"
#include "../../paint/Paint.h"
#include "../../paint/Supports.h"
#include "../Ride.h"
#include "../RideEntry.h"
#include "../Track.h"
#include "../TrackPaint.h"
#include "../Vehicle.h"

/** rct2: 0x01428010 */
static constexpr const uint32_t SwingingInverterShipBaseSpriteOffset[] = {
    0,
    16,
    0,
    16,
};

/** rct2: 0x01428020 */
static constexpr const uint32_t SwingingInverterShipAnimatingBaseSpriteOffset[] = {
    32,
    33,
    32,
    33,
};

/** rct2: 0x01428020 */
static constexpr const BoundBoxXY SwingingInverterShipBounds[] = {
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

static constexpr const uint32_t SwingingInverterShipFrameSprites[] = {
    SPR_SWINGING_INVERTER_SHIP_FRAME_0,
    SPR_SWINGING_INVERTER_SHIP_FRAME_1,
    SPR_SWINGING_INVERTER_SHIP_FRAME_2,
    SPR_SWINGING_INVERTER_SHIP_FRAME_3,
};

static void PaintSwingingInverterShipStructure(
    paint_session& session, const Ride& ride, uint8_t direction, int8_t axisOffset, uint16_t height)
{
    rct_ride_entry* rideEntry = get_ride_entry(ride.subtype);
    if (rideEntry == nullptr)
        return;

    const auto& boundBox = SwingingInverterShipBounds[direction];
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

    ImageIndex vehicleImageIndex = rideEntry->Cars[0].base_image_id + SwingingInverterShipBaseSpriteOffset[direction];
    if (vehicle != nullptr)
    {
        int32_t rotation = static_cast<int8_t>(vehicle->Pitch);
        if (rotation != 0)
        {
            vehicleImageIndex = rideEntry->Cars[0].base_image_id + SwingingInverterShipAnimatingBaseSpriteOffset[direction];
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
    auto imageFlags = session.TrackColours[SCHEME_MISC];
    if (imageFlags.ToUInt32() != IMAGE_TYPE_REMAP)
    {
        vehicleImageTemplate = imageFlags;
    }
    auto frameImageTemplate = session.TrackColours[SCHEME_TRACK];
    auto vehicleImageId = vehicleImageTemplate.WithIndex(vehicleImageIndex);
    auto frameImageId = frameImageTemplate.WithIndex(SwingingInverterShipFrameSprites[direction]);

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
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    uint8_t relativeTrackSequence = track_map_1x4[direction][trackSequence];
    ImageId imageId;

    const StationObject* stationObject = ride.GetStationObject();

    if (relativeTrackSequence != 1 && relativeTrackSequence != 3)
    {
        if (direction & 1)
        {
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 6, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
        }
        else
        {
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 5, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
        }

        if (stationObject != nullptr && !(stationObject->Flags & STATION_OBJECT_FLAGS::NO_PLATFORMS))
        {
            imageId = session.TrackColours[SCHEME_SUPPORTS].WithIndex(SPR_STATION_BASE_D);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 32, 32, 1 });

            switch (direction)
            {
                case 0:
                    imageId = session.TrackColours[SCHEME_TRACK].WithIndex(SPR_STATION_PLATFORM_SW_NE);
                    PaintAddImageAsParent(session, imageId, { 0, 24, height + 9 }, { 32, 8, 1 });
                    break;
                case 1:
                    imageId = session.TrackColours[SCHEME_TRACK].WithIndex(SPR_STATION_PLATFORM_NW_SE);
                    PaintAddImageAsParent(session, imageId, { 24, 0, height + 9 }, { 8, 32, 1 });
                    break;
                case 2:
                    imageId = session.TrackColours[SCHEME_TRACK].WithIndex(SPR_STATION_PLATFORM_SW_NE);
                    PaintAddImageAsChild(session, imageId, { 0, 0, height + 9 }, { 32, 8, 1 }, { -2, 0, height });
                    break;
                case 3:
                    imageId = session.TrackColours[SCHEME_TRACK].WithIndex(SPR_STATION_PLATFORM_NW_SE);
                    PaintAddImageAsChild(session, imageId, { 0, 0, height + 9 }, { 8, 32, 1 }, { 0, -2, height });
                    break;
            }
        }
    }

    switch (relativeTrackSequence)
    {
        case 1:
            PaintSwingingInverterShipStructure(session, ride, direction, 48, height + 7);
            break;
        case 2:
            PaintSwingingInverterShipStructure(session, ride, direction, 16, height + 7);
            break;
        case 0:
            PaintSwingingInverterShipStructure(session, ride, direction, -16, height + 7);
            break;
        case 3:
            PaintSwingingInverterShipStructure(session, ride, direction, -48, height + 7);
            break;
    }

    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 176, 0x20);
}

TRACK_PAINT_FUNCTION get_track_paint_function_swinging_inverter_ship(int32_t trackType)
{
    if (trackType != TrackElemType::FlatTrack1x4B)
    {
        return nullptr;
    }

    return PaintSwingingInverterShip;
}
