/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../entity/EntityRegistry.h"
#include "../../interface/Viewport.h"
#include "../../paint/Paint.h"
#include "../../paint/support/WoodenSupports.h"
#include "../../ride/Vehicle.h"
#include "../Ride.h"
#include "../RideEntry.h"
#include "../Track.h"
#include "../TrackPaint.h"

static void PaintCircusTent(
    PaintSession& session, const Ride& ride, uint8_t direction, int8_t al, int8_t cl, uint16_t height, ImageId stationColour)
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

    auto imageTemplate = ImageId(0, ride.vehicle_colours[0].Body, ride.vehicle_colours[0].Trim);
    if (stationColour != TrackStationColour)
    {
        imageTemplate = stationColour;
    }
    auto imageIndex = rideEntry->Cars[0].base_image_id + direction;

    PaintAddImageAsParent(
        session, imageTemplate.WithIndex(imageIndex), { al, cl, height + 3 },
        { { al + 16, cl + 16, height + 3 }, { 24, 24, 47 } });

    session.CurrentlyDrawnEntity = nullptr;
    session.InteractionType = ViewportInteractionItem::Ride;
}

static void PaintCircus(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = track_map_3x3[direction][trackSequence];

    int32_t edges = edges_3x3[trackSequence];

    WoodenASupportsPaintSetupRotated(
        session, WoodenSupportType::Truss, WoodenSupportSubType::NeSw, direction, height,
        GetStationColourScheme(session, trackElement));

    const StationObject* stationObject = ride.GetStationObject();

    TrackPaintUtilPaintFloor(session, edges, session.TrackColours, height, floorSpritesCork, stationObject);

    TrackPaintUtilPaintFences(
        session, edges, session.MapPosition, trackElement, ride, session.SupportColours, height, fenceSpritesRope,
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
            cornerSegments = EnumsToFlags(PaintSegment::topCorner, PaintSegment::topLeftSide, PaintSegment::topRightSide);
            break;
        case 3:
            // Right
            cornerSegments = EnumsToFlags(PaintSegment::topRightSide, PaintSegment::rightCorner, PaintSegment::bottomRightSide);
            break;
        case 6:
            // Left
            cornerSegments = EnumsToFlags(PaintSegment::topLeftSide, PaintSegment::leftCorner, PaintSegment::bottomLeftSide);
            break;
        case 7:
            // Bottom
            cornerSegments = EnumsToFlags(
                PaintSegment::bottomLeftSide, PaintSegment::bottomCorner, PaintSegment::bottomRightSide);
            break;
    }

    PaintUtilSetSegmentSupportHeight(session, cornerSegments, height + 2, 0x20);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll & ~cornerSegments, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 128, 0x20);
}

TRACK_PAINT_FUNCTION GetTrackPaintFunctionCircus(int32_t trackType)
{
    if (trackType != TrackElemType::kFlatTrack3x3)
    {
        return nullptr;
    }

    return PaintCircus;
}
