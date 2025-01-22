/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../../entity/EntityRegistry.h"
#include "../../../entity/Guest.h"
#include "../../../interface/Viewport.h"
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

static constexpr uint8_t kEdges1X4NeSw[] = {
    EDGE_NW | EDGE_SE,
    EDGE_NW | EDGE_SE | EDGE_NE,
    EDGE_NW | EDGE_SE,
    EDGE_NW | EDGE_SE | EDGE_SW,
};

static constexpr uint8_t kEdges1X4NwSe[] = {
    EDGE_NE | EDGE_SW,
    EDGE_NE | EDGE_SW | EDGE_NW,
    EDGE_NE | EDGE_SW,
    EDGE_NE | EDGE_SW | EDGE_SE,
};

/** rct2: 0x008A8CA8 */
static constexpr BoundBoxXY kFerrisWheelData[] = {
    { { 1, 8 }, { 31, 16 } },
    { { 8, 1 }, { 16, 31 } },
    { { 1, 8 }, { 31, 16 } },
    { { 8, 1 }, { 16, 31 } },
};

static void PaintFerrisWheelRiders(
    PaintSession& session, const RideObjectEntry& rideEntry, const Vehicle& vehicle, uint8_t direction, const CoordsXYZ offset,
    const BoundBoxXYZ& bb)
{
    for (int32_t i = 0; i < 32; i += 2)
    {
        auto* peep = GetEntity<Guest>(vehicle.peep[i]);
        if (peep == nullptr || peep->State != PeepState::OnRide)
            continue;

        auto frameNum = (vehicle.Pitch + i * 4) % 128;
        auto imageIndex = rideEntry.Cars[0].base_image_id + 32 + direction * 128 + frameNum;
        auto imageId = ImageId(imageIndex, vehicle.peep_tshirt_colours[i], vehicle.peep_tshirt_colours[i + 1]);
        PaintAddImageAsChild(session, imageId, offset, bb);
    }
}

static void PaintFerrisWheelStructure(
    PaintSession& session, const Ride& ride, uint8_t direction, int8_t axisOffset, uint16_t height, ImageId stationColour)
{
    auto rideEntry = ride.GetRideEntry();
    if (rideEntry == nullptr)
        return;

    auto vehicle = GetEntity<Vehicle>(ride.vehicles[0]);
    if (ride.lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK && vehicle != nullptr)
    {
        session.InteractionType = ViewportInteractionItem::Entity;
        session.CurrentlyDrawnEntity = vehicle;
    }

    auto boundBox = kFerrisWheelData[direction];
    CoordsXYZ offset((direction & 1) ? 0 : axisOffset, (direction & 1) ? axisOffset : 0, height + 7);
    BoundBoxXYZ bb = { { boundBox.offset, height + 7 }, { boundBox.length, 127 } };

    auto supportsImageTemplate = session.TrackColours;
    auto wheelImageTemplate = ImageId(0, ride.vehicle_colours[0].Body, ride.vehicle_colours[0].Trim);
    if (stationColour != TrackStationColour)
    {
        wheelImageTemplate = stationColour;
    }

    auto imageOffset = vehicle != nullptr ? vehicle->Pitch % 8 : 0;
    auto leftSupportImageId = supportsImageTemplate.WithIndex(22150 + (direction & 1) * 2);
    auto wheelImageId = wheelImageTemplate.WithIndex(rideEntry->Cars[0].base_image_id + direction * 8 + imageOffset);
    auto rightSupportImageId = leftSupportImageId.WithIndexOffset(1);

    PaintAddImageAsParent(session, leftSupportImageId, offset, bb);
    PaintAddImageAsChild(session, wheelImageId, offset, bb);
    if (vehicle != nullptr)
    {
        PaintFerrisWheelRiders(session, *rideEntry, *vehicle, direction, offset, bb);
    }
    PaintAddImageAsChild(session, rightSupportImageId, offset, bb);

    session.CurrentlyDrawnEntity = nullptr;
    session.InteractionType = ViewportInteractionItem::Ride;
}

static void PaintFerrisWheel(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    uint8_t relativeTrackSequence = kTrackMap1x4[direction][trackSequence];

    int32_t edges;
    if (direction & 1)
    {
        edges = kEdges1X4NwSe[relativeTrackSequence];
    }
    else
    {
        edges = kEdges1X4NeSw[relativeTrackSequence];
    }

    auto stationColour = GetStationColourScheme(session, trackElement);
    WoodenASupportsPaintSetupRotated(
        session, WoodenSupportType::Truss, WoodenSupportSubType::NeSw, direction, height, stationColour);

    const StationObject* stationObject = ride.GetStationObject();

    TrackPaintUtilPaintFloor(session, edges, session.TrackColours, height, kFloorSpritesCork, stationObject);

    ImageId imageId;
    uint8_t rotation = session.CurrentRotation;

    if (edges & EDGE_NW && TrackPaintUtilHasFence(EDGE_NW, session.MapPosition, trackElement, ride, rotation))
    {
        imageId = stationColour.WithIndex(SPR_FENCE_ROPE_NW);
        PaintAddImageAsChild(session, imageId, { 0, 0, height }, { { 0, 2, height + 2 }, { 32, 1, 7 } });
    }
    if (edges & EDGE_NE && TrackPaintUtilHasFence(EDGE_NE, session.MapPosition, trackElement, ride, rotation))
    {
        imageId = stationColour.WithIndex(SPR_FENCE_ROPE_NE);
        PaintAddImageAsChild(session, imageId, { 0, 0, height }, { { 2, 0, height + 2 }, { 1, 32, 7 } });
    }
    if (edges & EDGE_SE && TrackPaintUtilHasFence(EDGE_SE, session.MapPosition, trackElement, ride, rotation))
    {
        // Bound box is slightly different from TrackPaintUtilPaintFences
        imageId = stationColour.WithIndex(SPR_FENCE_ROPE_SE);
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 29, height + 3 }, { 28, 1, 7 } });
    }
    if (edges & EDGE_SW && TrackPaintUtilHasFence(EDGE_SW, session.MapPosition, trackElement, ride, rotation))
    {
        imageId = stationColour.WithIndex(SPR_FENCE_ROPE_SW);
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 30, 0, height + 2 }, { 1, 32, 7 } });
    }

    switch (relativeTrackSequence)
    {
        case 1:
            PaintFerrisWheelStructure(session, ride, direction, 48, height, stationColour);
            break;
        case 2:
            PaintFerrisWheelStructure(session, ride, direction, 16, height, stationColour);
            break;
        case 0:
            PaintFerrisWheelStructure(session, ride, direction, -16, height, stationColour);
            break;
        case 3:
            PaintFerrisWheelStructure(session, ride, direction, -48, height, stationColour);
            break;
    }

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 176);
}

TrackPaintFunction GetTrackPaintFunctionFerrisWheel(OpenRCT2::TrackElemType trackType)
{
    if (trackType != TrackElemType::FlatTrack1x4C)
    {
        return TrackPaintFunctionDummy;
    }

    return PaintFerrisWheel;
}
