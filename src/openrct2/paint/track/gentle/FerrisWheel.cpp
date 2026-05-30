/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../../GameState.h"
#include "../../../entity/EntityRegistry.h"
#include "../../../entity/Guest.h"
#include "../../../interface/Viewport.h"
#include "../../../ride/Ride.h"
#include "../../../ride/RideEntry.h"
#include "../../../ride/TrackPaint.h"
#include "../../../ride/Vehicle.h"
#include "../../Boundbox.h"
#include "../../Paint.h"
#include "../../support/WoodenSupports.h"
#include "../../tile_element/Segment.h"
#include "../../track/Segment.h"

using namespace OpenRCT2;

static constexpr uint8_t kEdges1X4NeSw[] = {
    edgeNW | edgeSE,
    edgeNW | edgeSE | edgeNE,
    edgeNW | edgeSE,
    edgeNW | edgeSE | edgeSW,
};

static constexpr uint8_t kEdges1X4NwSe[] = {
    edgeNE | edgeSW,
    edgeNE | edgeSW | edgeNW,
    edgeNE | edgeSW,
    edgeNE | edgeSW | edgeSE,
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
        auto* peep = getGameState().entities.GetEntity<Guest>(vehicle.peep[i]);
        if (peep == nullptr || peep->State != PeepState::onRide)
            continue;

        auto frameNum = (vehicle.flatRideAnimationFrame + i * 4) % 128;
        auto imageIndex = rideEntry.Cars[0].base_image_id + 32 + direction * 128 + frameNum;
        auto imageId = ImageId(imageIndex, vehicle.peep_tshirt_colours[i], vehicle.peep_tshirt_colours[i + 1]);
        paintAddImageAsChild(session, imageId, offset, bb);
    }
}

static void PaintFerrisWheelStructure(
    PaintSession& session, const Ride& ride, uint8_t direction, int8_t axisOffset, uint16_t height, ImageId stationColour)
{
    auto rideEntry = ride.getRideEntry();
    if (rideEntry == nullptr)
        return;

    auto vehicle = getGameState().entities.GetEntity<Vehicle>(ride.vehicles[0]);
    if (ride.flags.has(RideFlag::onTrack) && vehicle != nullptr)
    {
        session.interactionType = ViewportInteractionItem::entity;
        session.currentlyDrawnEntity = vehicle;
    }

    auto boundBox = kFerrisWheelData[direction];
    CoordsXYZ offset((direction & 1) ? 0 : axisOffset, (direction & 1) ? axisOffset : 0, height + 7);
    BoundBoxXYZ bb = { { boundBox.offset, height + 7 }, { boundBox.length, 127 } };

    auto supportsImageTemplate = session.trackColours;
    auto wheelImageTemplate = ImageId(0, ride.vehicleColours[0].Body, ride.vehicleColours[0].Trim);
    if (stationColour != TrackStationColour)
    {
        wheelImageTemplate = stationColour;
    }

    auto imageOffset = vehicle != nullptr ? vehicle->flatRideAnimationFrame % 8 : 0;
    auto leftSupportImageId = supportsImageTemplate.WithIndex(22150 + (direction & 1) * 2);
    auto wheelImageId = wheelImageTemplate.WithIndex(rideEntry->Cars[0].base_image_id + direction * 8 + imageOffset);
    auto rightSupportImageId = leftSupportImageId.WithIndexOffset(1);

    paintAddImageAsParent(session, leftSupportImageId, offset, bb);
    paintAddImageAsChild(session, wheelImageId, offset, bb);
    if (vehicle != nullptr)
    {
        PaintFerrisWheelRiders(session, *rideEntry, *vehicle, direction, offset, bb);
    }
    paintAddImageAsChild(session, rightSupportImageId, offset, bb);

    session.currentlyDrawnEntity = nullptr;
    session.interactionType = ViewportInteractionItem::ride;
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
        session, WoodenSupportType::truss, WoodenSupportSubType::neSw, direction, height, stationColour);

    const StationObject* stationObject = ride.getStationObject();

    TrackPaintUtilPaintFloor(session, edges, session.trackColours, height, kFloorSpritesMulch, stationObject);

    ImageId imageId;
    uint8_t rotation = session.currentRotation;

    if (edges & edgeNW && TrackPaintUtilHasFence(edgeNW, session.mapPosition, trackElement, ride, rotation))
    {
        imageId = stationColour.WithIndex(SPR_FENCE_ROPE_NW);
        paintAddImageAsChild(session, imageId, { 0, 0, height }, { { 0, 2, height + 2 }, { 32, 1, 7 } });
    }
    if (edges & edgeNE && TrackPaintUtilHasFence(edgeNE, session.mapPosition, trackElement, ride, rotation))
    {
        imageId = stationColour.WithIndex(SPR_FENCE_ROPE_NE);
        paintAddImageAsChild(session, imageId, { 0, 0, height }, { { 2, 0, height + 2 }, { 1, 32, 7 } });
    }
    if (edges & edgeSE && TrackPaintUtilHasFence(edgeSE, session.mapPosition, trackElement, ride, rotation))
    {
        // Bound box is slightly different from TrackPaintUtilPaintFences
        imageId = stationColour.WithIndex(SPR_FENCE_ROPE_SE);
        paintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 29, height + 3 }, { 28, 1, 7 } });
    }
    if (edges & edgeSW && TrackPaintUtilHasFence(edgeSW, session.mapPosition, trackElement, ride, rotation))
    {
        imageId = stationColour.WithIndex(SPR_FENCE_ROPE_SW);
        paintAddImageAsParent(session, imageId, { 0, 0, height }, { { 30, 0, height + 2 }, { 1, 32, 7 } });
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

    paintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    paintUtilSetGeneralSupportHeight(session, height + 176);
}

TrackPaintFunction GetTrackPaintFunctionFerrisWheel(TrackElemType trackType)
{
    if (trackType != TrackElemType::flatTrack1x4C)
    {
        return TrackPaintFunctionDummy;
    }

    return PaintFerrisWheel;
}
