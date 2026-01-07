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
#include "../../../interface/Viewport.h"
#include "../../../ride/Ride.h"
#include "../../../ride/RideEntry.h"
#include "../../../ride/Track.h"
#include "../../../ride/TrackPaint.h"
#include "../../../ride/Vehicle.h"
#include "../../Paint.h"
#include "../../support/WoodenSupports.h"
#include "../../tile_element/Segment.h"
#include "../../track/Segment.h"

using namespace OpenRCT2;

static constexpr uint32_t kMerryGoRoundRiderOffsets[] = {
    0, 32, 64, 96, 16, 48, 80, 112,
};

static constexpr uint16_t kMerryGoRoundBreakdownVibration[] = {
    0, 1, 2, 3, 4, 3, 2, 1, 0, 0,
};

static void PaintRiders(
    PaintSession& session, const Ride& ride, const RideObjectEntry& rideEntry, const Vehicle& vehicle, int32_t rotationOffset,
    const CoordsXYZ& offset, const BoundBoxXYZ& bb)
{
    if (session.rt.zoom_level > ZoomLevel{ 0 })
        return;
    if (!(ride.lifecycleFlags & RIDE_LIFECYCLE_ON_TRACK))
        return;

    for (int32_t peep = 0; peep <= 14; peep += 2)
    {
        if (vehicle.num_peeps <= peep)
            break;

        auto imageOffset = (kMerryGoRoundRiderOffsets[peep / 2] + rotationOffset) % 128;
        imageOffset -= 13;
        if (imageOffset >= 68)
            continue;

        auto imageIndex = rideEntry.Cars[0].base_image_id + 32 + imageOffset;
        auto imageId = ImageId(imageIndex, vehicle.peep_tshirt_colours[peep], vehicle.peep_tshirt_colours[peep + 1]);
        PaintAddImageAsChild(session, imageId, offset, bb);
    }
}

static void PaintCarousel(
    PaintSession& session, const Ride& ride, uint8_t direction, int8_t xOffset, int8_t yOffset, uint16_t height,
    ImageId stationColour)
{
    height += 7;

    auto rideEntry = ride.getRideEntry();
    if (rideEntry == nullptr)
        return;

    auto vehicle = getGameState().entities.GetEntity<Vehicle>(ride.vehicles[0]);
    if (ride.lifecycleFlags & RIDE_LIFECYCLE_ON_TRACK && vehicle != nullptr)
    {
        session.InteractionType = ViewportInteractionItem::entity;
        session.CurrentlyDrawnEntity = vehicle;

        if (ride.lifecycleFlags & (RIDE_LIFECYCLE_BREAKDOWN_PENDING | RIDE_LIFECYCLE_BROKEN_DOWN)
            && ride.breakdownReasonPending == BREAKDOWN_CONTROL_FAILURE && ride.breakdownSoundModifier >= 128)
        {
            height += kMerryGoRoundBreakdownVibration[(vehicle->current_time >> 1) & 7];
        }
    }

    auto rotationOffset = 0;
    if (vehicle != nullptr)
    {
        auto rotation = ((vehicle->Orientation >> 3) + session.CurrentRotation) << 5;
        rotationOffset = (vehicle->flatRideAnimationFrame + rotation) % 128;
    }

    CoordsXYZ offset(xOffset, yOffset, height);
    BoundBoxXYZ bb = { { xOffset + 16, yOffset + 16, height }, { 24, 24, 48 } };

    auto imageTemplate = ImageId(0, ride.vehicleColours[0].Body, ride.vehicleColours[0].Trim);
    if (stationColour != TrackStationColour)
    {
        imageTemplate = stationColour;
    }
    auto imageOffset = rotationOffset & 0x1F;
    auto imageId = imageTemplate.WithIndex(rideEntry->Cars[0].base_image_id + imageOffset);
    PaintAddImageAsParent(session, imageId, offset, bb);

    if (vehicle != nullptr && vehicle->num_peeps > 0)
    {
        PaintRiders(session, ride, *rideEntry, *vehicle, rotationOffset, offset, bb);
    }

    session.CurrentlyDrawnEntity = nullptr;
    session.InteractionType = ViewportInteractionItem::ride;
}

static void PaintMerryGoRound(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kTrackMap3x3[direction][trackSequence];

    int32_t edges = kEdges3x3[trackSequence];

    WoodenASupportsPaintSetupRotated(
        session, WoodenSupportType::truss, WoodenSupportSubType::neSw, direction, height,
        GetStationColourScheme(session, trackElement));

    const StationObject* stationObject = ride.getStationObject();

    TrackPaintUtilPaintFloor(session, edges, session.TrackColours, height, kFloorSpritesMulch, stationObject);

    TrackPaintUtilPaintFences(
        session, edges, session.MapPosition, trackElement, ride, GetStationColourScheme(session, trackElement), height,
        kFenceSpritesRope, session.CurrentRotation);

    auto stationColour = GetStationColourScheme(session, trackElement);
    switch (trackSequence)
    {
        case 1:
            PaintCarousel(session, ride, direction, 32, 32, height, stationColour);
            break;
        case 3:
            PaintCarousel(session, ride, direction, 32, -32, height, stationColour);
            break;
        case 5:
            PaintCarousel(session, ride, direction, 0, -32, height, stationColour);
            break;
        case 6:
            PaintCarousel(session, ride, direction, -32, 32, height, stationColour);
            break;
        case 7:
            PaintCarousel(session, ride, direction, -32, -32, height, stationColour);
            break;
        case 8:
            PaintCarousel(session, ride, direction, -32, 0, height, stationColour);
            break;
    }

    int32_t cornerSegments = 0;
    switch (trackSequence)
    {
        case 1:
            // top
            cornerSegments = EnumsToFlags(PaintSegment::top, PaintSegment::topLeft, PaintSegment::topRight);
            break;
        case 3:
            // right
            cornerSegments = EnumsToFlags(PaintSegment::topRight, PaintSegment::right, PaintSegment::bottomRight);
            break;
        case 6:
            // left
            cornerSegments = EnumsToFlags(PaintSegment::topLeft, PaintSegment::left, PaintSegment::bottomLeft);
            break;
        case 7:
            // bottom
            cornerSegments = EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::bottom, PaintSegment::bottomRight);
            break;
    }

    PaintUtilSetSegmentSupportHeight(session, cornerSegments, height + 2, 0x20);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll & ~cornerSegments, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 64);
}

TrackPaintFunction GetTrackPaintFunctionMerryGoRound(TrackElemType trackType)
{
    if (trackType != TrackElemType::flatTrack3x3)
    {
        return TrackPaintFunctionDummy;
    }

    return PaintMerryGoRound;
}
