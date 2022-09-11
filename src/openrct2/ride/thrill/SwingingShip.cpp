/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
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

// 1 2 0 3 4
static constexpr const uint8_t track_map_1x5[][5] = {
    { 0, 1, 2, 3, 4 },
    { 0, 4, 3, 2, 1 },
    { 0, 4, 3, 2, 1 },
    { 0, 1, 2, 3, 4 },
};
/** rct2: 0x008A83B0 */
static constexpr const uint32_t SwingingShipBaseSpriteOffset[] = {
    0,
    9,
    0,
    9,
};

/** rct2: 0x008A83C0 */
static constexpr const BoundBoxXY SwingingShipData[] = {
    { { 1, 8 }, { 31, 16 } },
    { { 8, 1 }, { 16, 31 } },
    { { 1, 8 }, { 31, 16 } },
    { { 8, 1 }, { 16, 31 } },
};

enum
{
    SPR_SWINGING_SHIP_FRAME_SW_NE = 21994,
    SPR_SWINGING_SHIP_FRAME_FRONT_SW_NE = 21995,
    SPR_SWINGING_SHIP_FRAME_NW_SE = 21996,
    SPR_SWINGING_SHIP_FRAME_FRONT_NW_SE = 21997,
};

static constexpr const uint32_t SwingingShipFrameSprites[][2] = {
    { SPR_SWINGING_SHIP_FRAME_SW_NE, SPR_SWINGING_SHIP_FRAME_FRONT_SW_NE },
    { SPR_SWINGING_SHIP_FRAME_NW_SE, SPR_SWINGING_SHIP_FRAME_FRONT_NW_SE },
};

static void PaintSwingingShipRiders(
    paint_session& session, const Ride& ride, const Vehicle& vehicle, ImageIndex baseImageIndex, Direction direction,
    const CoordsXYZ& offset, const CoordsXYZ& bbLength, const CoordsXYZ& bbOffset)
{
    if (session.DPI.zoom_level > ZoomLevel{ 1 })
        return;

    if (!(ride.lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK))
        return;

    int32_t peep = 0;
    for (int32_t row = 0; row < 4; row++)
    {
        for (int32_t col = 0; col < 2; col++)
        {
            if (vehicle.num_peeps <= peep)
                break;

            auto frameNum = 1 + (row * 2) + ((direction >> 1) ^ col);
            auto imageIndex = baseImageIndex + frameNum;
            auto imageId = ImageId(imageIndex, vehicle.peep_tshirt_colours[peep], vehicle.peep_tshirt_colours[peep + 1]);
            PaintAddImageAsChild(session, imageId, offset, bbLength, bbOffset);

            peep += 2;
        }
    }
}

static void PaintSwingingShipStructure(
    paint_session& session, const Ride& ride, uint8_t direction, int8_t axisOffset, uint16_t height)
{
    rct_ride_entry* rideEntry = get_ride_entry(ride.subtype);
    if (rideEntry == nullptr)
        return;

    Vehicle* vehicle = nullptr;
    if (ride.lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK && !ride.vehicles[0].IsNull())
    {
        vehicle = GetEntity<Vehicle>(ride.vehicles[0]);
        session.InteractionType = ViewportInteractionItem::Entity;
        session.CurrentlyDrawnEntity = vehicle;
    }

    const auto& bounds = SwingingShipData[direction];
    CoordsXYZ offset((direction & 1) ? 0 : axisOffset, (direction & 1) ? axisOffset : 0, height + 7);
    CoordsXYZ bbLength(bounds.length.x, bounds.length.y, 80);
    CoordsXYZ bbOffset(bounds.offset.x, bounds.offset.y, height + 7);

    auto baseImageId = rideEntry->Cars[0].base_image_id + SwingingShipBaseSpriteOffset[direction];
    if (vehicle != nullptr)
    {
        int32_t rotation = static_cast<int8_t>(vehicle->Pitch);
        if (rotation != 0)
        {
            if (direction & 2)
            {
                rotation = -rotation;
            }
            if (rotation < 0)
            {
                rotation = -rotation + 9;
            }

            baseImageId += rotation * 18;
        }
    }

    auto supportsImageTemplate = ImageId::FromUInt32(session.TrackColours[SCHEME_TRACK]);
    auto vehicleImageTemplate = ImageId(0, ride.vehicle_colours[0].Body, ride.vehicle_colours[0].Trim);
    auto imageFlags = session.TrackColours[SCHEME_MISC];
    if (imageFlags != IMAGE_TYPE_REMAP)
    {
        vehicleImageTemplate = ImageId::FromUInt32(imageFlags);
    }

    // Supports (back)
    auto imageId = supportsImageTemplate.WithIndex(SwingingShipFrameSprites[(direction & 1)][0]);
    PaintAddImageAsParent(session, imageId, offset, bbLength, bbOffset);

    // Ship
    imageId = vehicleImageTemplate.WithIndex(baseImageId);
    PaintAddImageAsChild(session, imageId, offset, bbLength, bbOffset);

    if (vehicle != nullptr)
    {
        PaintSwingingShipRiders(session, ride, *vehicle, baseImageId, direction, offset, bbLength, bbOffset);
    }

    // Supports (front)
    imageId = supportsImageTemplate.WithIndex(SwingingShipFrameSprites[(direction & 1)][1]);
    PaintAddImageAsChild(session, imageId, offset, bbLength, bbOffset);

    session.CurrentlyDrawnEntity = nullptr;
    session.InteractionType = ViewportInteractionItem::Ride;
}

static void PaintSwingingShip(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    uint8_t relativeTrackSequence = track_map_1x5[direction][trackSequence];

    uint32_t imageId;
    bool hasFence;

    const StationObject* stationObject = ride.GetStationObject();

    if (relativeTrackSequence == 1 || relativeTrackSequence == 4)
    {
        wooden_a_supports_paint_setup(session, direction & 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
    }
    else if (direction & 1)
    {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 6, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);

        if (stationObject != nullptr && !(stationObject->Flags & STATION_OBJECT_FLAGS::NO_PLATFORMS))
        {
            imageId = SPR_STATION_BASE_A_NW_SE | session.TrackColours[SCHEME_SUPPORTS];
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 32, 32, 1 });
        }
    }
    else
    {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 5, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);

        if (stationObject != nullptr && !(stationObject->Flags & STATION_OBJECT_FLAGS::NO_PLATFORMS))
        {
            imageId = SPR_STATION_BASE_A_SW_NE | session.TrackColours[SCHEME_SUPPORTS];
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 32, 32, 1 });
        }
    }

    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);

    if (stationObject != nullptr && !(stationObject->Flags & STATION_OBJECT_FLAGS::NO_PLATFORMS))
    {
        if (direction & 1)
        {
            if (relativeTrackSequence != 1 && relativeTrackSequence != 4)
            {
                hasFence = track_paint_util_has_fence(
                    EDGE_NE, session.MapPosition, trackElement, ride, session.CurrentRotation);
                if (relativeTrackSequence == 2)
                {
                    imageId = (hasFence ? SPR_STATION_PLATFORM_BEGIN_FENCED_NW_SE : SPR_STATION_PLATFORM_BEGIN_NW_SE)
                        | session.TrackColours[SCHEME_TRACK];
                }
                else
                {
                    imageId = (hasFence ? SPR_STATION_PLATFORM_FENCED_NW_SE : SPR_STATION_PLATFORM_NW_SE)
                        | session.TrackColours[SCHEME_TRACK];
                }
                PaintAddImageAsChild(session, imageId, { 0, 0, height + 9 }, { 8, 32, 1 }, { 0, -2, height + 9 });

                imageId = (relativeTrackSequence == 2 ? SPR_STATION_PLATFORM_BEGIN_NW_SE : SPR_STATION_PLATFORM_NW_SE)
                    | session.TrackColours[SCHEME_TRACK];
                PaintAddImageAsParent(session, imageId, { 24, 0, height + 9 }, { 8, 32, 1 });

                hasFence = track_paint_util_has_fence(
                    EDGE_SW, session.MapPosition, trackElement, ride, session.CurrentRotation);
                if (relativeTrackSequence == 3)
                {
                    if (hasFence)
                    {
                        imageId = SPR_STATION_BEGIN_ANGLE_FENCE_NW_SE | session.TrackColours[SCHEME_TRACK];
                        PaintAddImageAsParent(session, imageId, { 31, 0, height + 11 }, { 1, 32, 7 });
                    }
                    else
                    {
                        imageId = SPR_STATION_FENCE_SMALL_SW_NE | session.TrackColours[SCHEME_TRACK];
                        PaintAddImageAsParent(session, imageId, { 23, 31, height + 11 }, { 8, 1, 7 });
                    }

                    imageId = SPR_STATION_FENCE_SMALL_SW_NE | session.TrackColours[SCHEME_TRACK];
                    PaintAddImageAsParent(session, imageId, { 0, 31, height + 11 }, { 8, 1, 7 });
                }
                else if (hasFence)
                {
                    imageId = SPR_STATION_FENCE_NW_SE | session.TrackColours[SCHEME_TRACK];
                    PaintAddImageAsParent(session, imageId, { 31, 0, height + 11 }, { 1, 32, 7 });
                }
            }
        }
        else
        {
            if (relativeTrackSequence != 1 && relativeTrackSequence != 4)
            {
                hasFence = track_paint_util_has_fence(
                    EDGE_NW, session.MapPosition, trackElement, ride, session.CurrentRotation);
                if (relativeTrackSequence == 2)
                {
                    imageId = (hasFence ? SPR_STATION_PLATFORM_BEGIN_FENCED_SW_NE : SPR_STATION_PLATFORM_BEGIN_SW_NE)
                        | session.TrackColours[SCHEME_TRACK];
                }
                else
                {
                    imageId = (hasFence ? SPR_STATION_PLATFORM_FENCED_SW_NE : SPR_STATION_PLATFORM_SW_NE)
                        | session.TrackColours[SCHEME_TRACK];
                }
                PaintAddImageAsChild(session, imageId, { 0, 0, height + 9 }, { 32, 8, 1 }, { -2, 0, height + 9 });

                imageId = (relativeTrackSequence == 2 ? SPR_STATION_PLATFORM_BEGIN_SW_NE : SPR_STATION_PLATFORM_SW_NE)
                    | session.TrackColours[SCHEME_TRACK];
                PaintAddImageAsParent(session, imageId, { 0, 24, height + 9 }, { 32, 8, 1 });

                hasFence = track_paint_util_has_fence(
                    EDGE_SE, session.MapPosition, trackElement, ride, session.CurrentRotation);
                if (relativeTrackSequence == 3)
                {
                    if (hasFence)
                    {
                        imageId = SPR_STATION_BEGIN_ANGLE_FENCE_SW_NE | session.TrackColours[SCHEME_TRACK];
                        PaintAddImageAsParent(session, imageId, { 0, 31, height + 11 }, { 32, 1, 7 });
                    }
                    else
                    {
                        imageId = SPR_STATION_FENCE_SMALL_NW_SE | session.TrackColours[SCHEME_TRACK];
                        PaintAddImageAsParent(session, imageId, { 31, 23, height + 11 }, { 1, 8, 7 });
                    }

                    imageId = SPR_STATION_FENCE_SMALL_NW_SE | session.TrackColours[SCHEME_TRACK];
                    PaintAddImageAsParent(session, imageId, { 31, 0, height + 11 }, { 1, 8, 7 });
                }
                else if (hasFence)
                {
                    imageId = SPR_STATION_FENCE_SW_NE | session.TrackColours[SCHEME_TRACK];
                    PaintAddImageAsParent(session, imageId, { 0, 31, height + 11 }, { 32, 1, 7 });
                }
            }
        }
    }
    switch (relativeTrackSequence)
    {
        case 1:
            PaintSwingingShipStructure(session, ride, direction, 64, height);
            break;
        case 2:
            PaintSwingingShipStructure(session, ride, direction, 32, height);
            break;
        case 0:
            PaintSwingingShipStructure(session, ride, direction, 0, height);
            break;
        case 3:
            PaintSwingingShipStructure(session, ride, direction, -32, height);
            break;
        case 4:
            PaintSwingingShipStructure(session, ride, direction, -64, height);
            break;
    }

    paint_util_set_general_support_height(session, height + 112, 0x20);
}

TRACK_PAINT_FUNCTION get_track_paint_function_swinging_ship(int32_t trackType)
{
    if (trackType != TrackElemType::FlatTrack1x5)
    {
        return nullptr;
    }

    return PaintSwingingShip;
}
