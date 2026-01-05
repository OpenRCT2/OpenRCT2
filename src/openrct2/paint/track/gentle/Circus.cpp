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
#include "../../track/Support.h"

using namespace OpenRCT2;

static void PaintCircusTent(
    PaintSession& session, const Ride& ride, uint8_t direction, int8_t al, int8_t cl, uint16_t height, ImageId stationColour)
{
    auto rideEntry = ride.getRideEntry();
    if (rideEntry == nullptr)
        return;

    auto vehicle = getGameState().entities.GetEntity<Vehicle>(ride.vehicles[0]);
    if (ride.lifecycleFlags & RIDE_LIFECYCLE_ON_TRACK && vehicle != nullptr)
    {
        session.InteractionType = ViewportInteractionItem::entity;
        session.CurrentlyDrawnEntity = vehicle;
    }

    auto imageTemplate = ImageId(0, ride.vehicleColours[0].Body, ride.vehicleColours[0].Trim);
    if (stationColour != TrackStationColour)
    {
        imageTemplate = stationColour;
    }
    auto imageIndex = rideEntry->Cars[0].base_image_id + direction;

    PaintAddImageAsParent(
        session, imageTemplate.WithIndex(imageIndex), { al, cl, height + 3 },
        { { al + 16, cl + 16, height + 3 }, { 24, 24, 47 } });

    session.CurrentlyDrawnEntity = nullptr;
    session.InteractionType = ViewportInteractionItem::ride;
}

static void PaintCircus(
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
        session, edges, session.MapPosition, trackElement, ride, session.SupportColours, height, kFenceSpritesRope,
        session.CurrentRotation);

    auto stationColour = GetStationColourScheme(session, trackElement);
    switch (trackSequence)
    {
        case 1:
            PaintCircusTent(session, ride, direction, 32, 32, height, stationColour);
            break;
        case 3:
            PaintCircusTent(session, ride, direction, 32, -32, height, stationColour);
            break;
        case 5:
            PaintCircusTent(session, ride, direction, 0, -32, height, stationColour);
            break;
        case 6:
            PaintCircusTent(session, ride, direction, -32, 32, height, stationColour);
            break;
        case 7:
            PaintCircusTent(session, ride, direction, -32, -32, height, stationColour);
            break;
        case 8:
            PaintCircusTent(session, ride, direction, -32, 0, height, stationColour);
            break;
    }

    int32_t cornerSegments = 0;
    switch (trackSequence)
    {
        case 1:
            // Top
            cornerSegments = EnumsToFlags(PaintSegment::top, PaintSegment::topLeft, PaintSegment::topRight);
            break;
        case 3:
            // Right
            cornerSegments = EnumsToFlags(PaintSegment::topRight, PaintSegment::right, PaintSegment::bottomRight);
            break;
        case 6:
            // Left
            cornerSegments = EnumsToFlags(PaintSegment::topLeft, PaintSegment::left, PaintSegment::bottomLeft);
            break;
        case 7:
            // Bottom
            cornerSegments = EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::bottom, PaintSegment::bottomRight);
            break;
    }

    PaintUtilSetSegmentSupportHeight(session, cornerSegments, height + 2, 0x20);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll & ~cornerSegments, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 128);
}

TrackPaintFunction GetTrackPaintFunctionCircus(TrackElemType trackType)
{
    if (trackType != TrackElemType::flatTrack3x3)
    {
        return TrackPaintFunctionDummy;
    }

    return PaintCircus;
}
