/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../entity/EntityRegistry.h"
#include "../../interface/Viewport.h"
#include "../../paint/Boundbox.h"
#include "../../paint/Paint.h"
#include "../../paint/Supports.h"
#include "../Ride.h"
#include "../RideEntry.h"
#include "../Track.h"
#include "../TrackPaint.h"
#include "../Vehicle.h"

enum
{
    SPR_MOTION_SIMULATOR_STAIRS_R0 = 22154,
    SPR_MOTION_SIMULATOR_STAIRS_R1 = 22155,
    SPR_MOTION_SIMULATOR_STAIRS_R2 = 22156,
    SPR_MOTION_SIMULATOR_STAIRS_R3 = 22157,
    SPR_MOTION_SIMULATOR_STAIRS_RAIL_R0 = 22158,
    SPR_MOTION_SIMULATOR_STAIRS_RAIL_R1 = 22159,
    SPR_MOTION_SIMULATOR_STAIRS_RAIL_R2 = 22160,
    SPR_MOTION_SIMULATOR_STAIRS_RAIL_R3 = 22161,
};

static void PaintMotionSimulatorVehicle(
    PaintSession& session, const Ride& ride, int8_t offsetX, int8_t offsetY, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    auto rideEntry = ride.GetRideEntry();
    if (rideEntry == nullptr)
        return;

    CoordsXYZ offset(offsetX, offsetY, height + 2);

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

    auto imageIndex = rideEntry->Cars[0].base_image_id + direction;
    if (vehicle != nullptr)
    {
        if (vehicle->restraints_position >= 64)
        {
            imageIndex += (vehicle->restraints_position >> 6) << 2;
        }
        else
        {
            imageIndex += vehicle->Pitch * 4;
        }
    }

    auto imageTemplate = ImageId(0, ride.vehicle_colours[0].Body, ride.vehicle_colours[0].Trim);
    auto imageFlags = session.TrackColours[SCHEME_MISC];
    if (imageFlags != TrackGhost)
    {
        imageTemplate = imageFlags;
    }
    auto simulatorImageId = imageTemplate.WithIndex(imageIndex);
    auto stairsImageId = imageTemplate.WithIndex(SPR_MOTION_SIMULATOR_STAIRS_R0 + direction);
    auto stairsRailImageId = imageTemplate.WithIndex(SPR_MOTION_SIMULATOR_STAIRS_RAIL_R0 + direction);
    switch (direction)
    {
        case 0:
            PaintAddImageAsParent(session, simulatorImageId, offset, { offset, { 20, 20, 44 } });
            PaintAddImageAsChild(session, stairsImageId, offset, { offset, { 20, 20, 44 } });
            PaintAddImageAsParent(session, stairsRailImageId, offset, { { offset.x, offset.y + 32, offset.z }, { 20, 2, 44 } });
            break;
        case 1:
            PaintAddImageAsParent(session, simulatorImageId, offset, { offset, { 20, 20, 44 } });
            PaintAddImageAsChild(session, stairsImageId, offset, { offset, { 20, 20, 44 } });
            PaintAddImageAsParent(session, stairsRailImageId, offset, { { offset.x + 34, offset.y, offset.z }, { 2, 20, 44 } });
            break;
        case 2:
            PaintAddImageAsParent(session, stairsRailImageId, offset, { { offset.x, offset.y - 10, offset.z }, { 20, 2, 44 } });
            PaintAddImageAsParent(session, stairsImageId, offset, { { offset.x, offset.y + 5, offset.z }, { 20, 20, 44 } });
            PaintAddImageAsChild(session, simulatorImageId, offset, { { offset.x, offset.y + 5, offset.z }, { 20, 20, 44 } });
            break;
        case 3:
            PaintAddImageAsParent(session, stairsRailImageId, offset, { { offset.x - 10, offset.y, offset.z }, { 2, 20, 44 } });
            PaintAddImageAsParent(session, stairsImageId, offset, { { offset.x + 5, offset.y, offset.z }, { 20, 20, 44 } });
            PaintAddImageAsChild(session, simulatorImageId, offset, { { offset.x + 5, offset.y, offset.z }, { 20, 20, 44 } });
            break;
    }

    session.CurrentlyDrawnEntity = nullptr;
    session.InteractionType = ViewportInteractionItem::Ride;
}

static void PaintMotionSimulator(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = track_map_2x2[direction][trackSequence];

    int32_t edges = edges_2x2[trackSequence];

    WoodenASupportsPaintSetup(session, (direction & 1), 0, height, session.TrackColours[SCHEME_MISC]);

    const StationObject* stationObject = ride.GetStationObject();

    TrackPaintUtilPaintFloor(session, edges, session.TrackColours[SCHEME_TRACK], height, floorSpritesCork, stationObject);

    TrackPaintUtilPaintFences(
        session, edges, session.MapPosition, trackElement, ride, session.TrackColours[SCHEME_SUPPORTS], height,
        fenceSpritesRope, session.CurrentRotation);

    switch (trackSequence)
    {
        case 1:
            PaintMotionSimulatorVehicle(session, ride, 16, -16, direction, height, trackElement);
            break;
        case 2:
            PaintMotionSimulatorVehicle(session, ride, -16, 16, direction, height, trackElement);
            break;
        case 3:
            PaintMotionSimulatorVehicle(session, ride, -16, -16, direction, height, trackElement);
            break;
    }

    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 128, 0x20);
}

/**
 *
 *  rct2: 0x00763520
 */
TRACK_PAINT_FUNCTION GetTrackPaintFunctionMotionsimulator(int32_t trackType)
{
    switch (trackType)
    {
        case TrackElemType::FlatTrack2x2:
            return PaintMotionSimulator;
    }
    return nullptr;
}
