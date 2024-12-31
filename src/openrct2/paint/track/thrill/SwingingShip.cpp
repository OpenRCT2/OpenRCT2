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

// 1 2 0 3 4
static constexpr uint8_t track_map_1x5[][5] = {
    { 0, 1, 2, 3, 4 },
    { 0, 4, 3, 2, 1 },
    { 0, 4, 3, 2, 1 },
    { 0, 1, 2, 3, 4 },
};
/** rct2: 0x008A83B0 */
static constexpr uint32_t SwingingShipBaseSpriteOffset[] = {
    0,
    9,
    0,
    9,
};

/** rct2: 0x008A83C0 */
static constexpr BoundBoxXY SwingingShipData[] = {
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

static constexpr uint32_t SwingingShipFrameSprites[][2] = {
    { SPR_SWINGING_SHIP_FRAME_SW_NE, SPR_SWINGING_SHIP_FRAME_FRONT_SW_NE },
    { SPR_SWINGING_SHIP_FRAME_NW_SE, SPR_SWINGING_SHIP_FRAME_FRONT_NW_SE },
};

static void PaintSwingingShipRiders(
    PaintSession& session, const Ride& ride, const Vehicle& vehicle, ImageIndex baseImageIndex, Direction direction,
    const CoordsXYZ& offset, const BoundBoxXYZ& bb)
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
            PaintAddImageAsChild(session, imageId, offset, bb);

            peep += 2;
        }
    }
}

static void PaintSwingingShipStructure(
    PaintSession& session, const Ride& ride, uint8_t direction, int8_t axisOffset, uint16_t height, ImageId stationColour)
{
    const auto* rideEntry = GetRideEntryByIndex(ride.subtype);
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
    BoundBoxXYZ bb = { { bounds.offset, height + 7 }, { bounds.length, 80 } };

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

    auto supportsImageTemplate = session.TrackColours;
    auto vehicleImageTemplate = ImageId(0, ride.vehicle_colours[0].Body, ride.vehicle_colours[0].Trim);
    if (stationColour != TrackStationColour)
    {
        vehicleImageTemplate = stationColour;
    }

    // Supports (back)
    auto imageId = supportsImageTemplate.WithIndex(SwingingShipFrameSprites[(direction & 1)][0]);
    PaintAddImageAsParent(session, imageId, offset, bb);

    // Ship
    imageId = vehicleImageTemplate.WithIndex(baseImageId);
    PaintAddImageAsChild(session, imageId, offset, bb);

    if (vehicle != nullptr)
    {
        PaintSwingingShipRiders(session, ride, *vehicle, baseImageId, direction, offset, bb);
    }

    // Supports (front)
    imageId = supportsImageTemplate.WithIndex(SwingingShipFrameSprites[(direction & 1)][1]);
    PaintAddImageAsChild(session, imageId, offset, bb);

    session.CurrentlyDrawnEntity = nullptr;
    session.InteractionType = ViewportInteractionItem::Ride;
}

static void PaintSwingingShip(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    uint8_t relativeTrackSequence = track_map_1x5[direction][trackSequence];

    ImageId imageId;
    bool hasFence;

    const StationObject* stationObject = ride.GetStationObject();

    if (relativeTrackSequence == 1 || relativeTrackSequence == 4)
    {
        WoodenASupportsPaintSetupRotated(
            session, WoodenSupportType::Truss, WoodenSupportSubType::NeSw, direction, height, session.SupportColours);
    }
    else
    {
        DrawSupportsSideBySide(session, direction, height, session.SupportColours, MetalSupportType::Tubes);
        if (stationObject != nullptr && !(stationObject->Flags & STATION_OBJECT_FLAGS::NO_PLATFORMS))
        {
            ImageIndex base = (direction & 1) ? SPR_STATION_BASE_A_NW_SE : SPR_STATION_BASE_A_SW_NE;
            imageId = session.SupportColours.WithIndex(base);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 32, 32, 1 });
        }
    }

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);

    if (stationObject != nullptr && !(stationObject->Flags & STATION_OBJECT_FLAGS::NO_PLATFORMS))
    {
        if (direction & 1)
        {
            if (relativeTrackSequence != 1 && relativeTrackSequence != 4)
            {
                hasFence = TrackPaintUtilHasFence(EDGE_NE, session.MapPosition, trackElement, ride, session.CurrentRotation);
                if (relativeTrackSequence == 2)
                {
                    imageId = session.TrackColours.WithIndex(
                        (hasFence ? SPR_STATION_PLATFORM_BEGIN_FENCED_NW_SE : SPR_STATION_PLATFORM_BEGIN_NW_SE));
                }
                else
                {
                    imageId = session.TrackColours.WithIndex(
                        (hasFence ? SPR_STATION_PLATFORM_FENCED_NW_SE : SPR_STATION_PLATFORM_NW_SE));
                }
                PaintAddImageAsChild(session, imageId, { 0, 0, height + 9 }, { { 0, -2, height + 9 }, { 8, 32, 1 } });

                imageId = session.TrackColours.WithIndex(
                    (relativeTrackSequence == 2 ? SPR_STATION_PLATFORM_BEGIN_NW_SE : SPR_STATION_PLATFORM_NW_SE));
                PaintAddImageAsParent(session, imageId, { 24, 0, height + 9 }, { 8, 32, 1 });

                hasFence = TrackPaintUtilHasFence(EDGE_SW, session.MapPosition, trackElement, ride, session.CurrentRotation);
                if (relativeTrackSequence == 3)
                {
                    if (hasFence)
                    {
                        imageId = session.TrackColours.WithIndex(SPR_STATION_BEGIN_ANGLE_FENCE_NW_SE);
                        PaintAddImageAsParent(session, imageId, { 31, 0, height + 11 }, { 1, 32, 7 });
                    }
                    else
                    {
                        imageId = session.TrackColours.WithIndex(SPR_STATION_FENCE_SMALL_SW_NE);
                        PaintAddImageAsParent(session, imageId, { 23, 31, height + 11 }, { 8, 1, 7 });
                    }

                    imageId = session.TrackColours.WithIndex(SPR_STATION_FENCE_SMALL_SW_NE);
                    PaintAddImageAsParent(session, imageId, { 0, 31, height + 11 }, { 8, 1, 7 });
                }
                else if (hasFence)
                {
                    imageId = session.TrackColours.WithIndex(SPR_STATION_FENCE_NW_SE);
                    PaintAddImageAsParent(session, imageId, { 31, 0, height + 11 }, { 1, 32, 7 });
                }
            }
        }
        else
        {
            if (relativeTrackSequence != 1 && relativeTrackSequence != 4)
            {
                hasFence = TrackPaintUtilHasFence(EDGE_NW, session.MapPosition, trackElement, ride, session.CurrentRotation);
                if (relativeTrackSequence == 2)
                {
                    imageId = session.TrackColours.WithIndex(
                        (hasFence ? SPR_STATION_PLATFORM_BEGIN_FENCED_SW_NE : SPR_STATION_PLATFORM_BEGIN_SW_NE));
                }
                else
                {
                    imageId = session.TrackColours.WithIndex(
                        (hasFence ? SPR_STATION_PLATFORM_FENCED_SW_NE : SPR_STATION_PLATFORM_SW_NE));
                }
                PaintAddImageAsChild(session, imageId, { 0, 0, height + 9 }, { { -2, 0, height + 9 }, { 32, 8, 1 } });

                imageId = session.TrackColours.WithIndex(
                    (relativeTrackSequence == 2 ? SPR_STATION_PLATFORM_BEGIN_SW_NE : SPR_STATION_PLATFORM_SW_NE));
                PaintAddImageAsParent(session, imageId, { 0, 24, height + 9 }, { 32, 8, 1 });

                hasFence = TrackPaintUtilHasFence(EDGE_SE, session.MapPosition, trackElement, ride, session.CurrentRotation);
                if (relativeTrackSequence == 3)
                {
                    if (hasFence)
                    {
                        imageId = session.TrackColours.WithIndex(SPR_STATION_BEGIN_ANGLE_FENCE_SW_NE);
                        PaintAddImageAsParent(session, imageId, { 0, 31, height + 11 }, { 32, 1, 7 });
                    }
                    else
                    {
                        imageId = session.TrackColours.WithIndex(SPR_STATION_FENCE_SMALL_NW_SE);
                        PaintAddImageAsParent(session, imageId, { 31, 23, height + 11 }, { 1, 8, 7 });
                    }

                    imageId = session.TrackColours.WithIndex(SPR_STATION_FENCE_SMALL_NW_SE);
                    PaintAddImageAsParent(session, imageId, { 31, 0, height + 11 }, { 1, 8, 7 });
                }
                else if (hasFence)
                {
                    imageId = session.TrackColours.WithIndex(SPR_STATION_FENCE_SW_NE);
                    PaintAddImageAsParent(session, imageId, { 0, 31, height + 11 }, { 32, 1, 7 });
                }
            }
        }
    }
    auto stationColour = GetStationColourScheme(session, trackElement);
    switch (relativeTrackSequence)
    {
        case 1:
            PaintSwingingShipStructure(session, ride, direction, 64, height, stationColour);
            break;
        case 2:
            PaintSwingingShipStructure(session, ride, direction, 32, height, stationColour);
            break;
        case 0:
            PaintSwingingShipStructure(session, ride, direction, 0, height, stationColour);
            break;
        case 3:
            PaintSwingingShipStructure(session, ride, direction, -32, height, stationColour);
            break;
        case 4:
            PaintSwingingShipStructure(session, ride, direction, -64, height, stationColour);
            break;
    }

    PaintUtilSetGeneralSupportHeight(session, height + 112);
}

TrackPaintFunction GetTrackPaintFunctionSwingingShip(OpenRCT2::TrackElemType trackType)
{
    if (trackType != TrackElemType::FlatTrack1x5)
    {
        return TrackPaintFunctionDummy;
    }

    return PaintSwingingShip;
}
