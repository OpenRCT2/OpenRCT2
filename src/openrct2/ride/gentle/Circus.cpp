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
#include "../../paint/Paint.h"
#include "../../paint/Supports.h"
#include "../../ride/Vehicle.h"
#include "../Ride.h"
#include "../RideEntry.h"
#include "../Track.h"
#include "../TrackPaint.h"

static void PaintCircusTent(paint_session& session, const Ride& ride, uint8_t direction, int8_t al, int8_t cl, uint16_t height)
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
    auto imageFlags = session.TrackColours[SCHEME_MISC];
    if (!imageFlags.HasPrimary())
    {
        imageTemplate = imageFlags;
    }
    auto imageIndex = rideEntry->Cars[0].base_image_id + direction;

    PaintAddImageAsParent(
        session, imageTemplate.WithIndex(imageIndex), { al, cl, height + 3 }, { 24, 24, 47 }, { al + 16, cl + 16, height + 3 });

    session.CurrentlyDrawnEntity = nullptr;
    session.InteractionType = ViewportInteractionItem::Ride;
}

static void PaintCircus(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = track_map_3x3[direction][trackSequence];

    int32_t edges = edges_3x3[trackSequence];

    WoodenASupportsPaintSetup(session, (direction & 1), 0, height, session.TrackColours[SCHEME_MISC]);

    const StationObject* stationObject = ride.GetStationObject();

    track_paint_util_paint_floor(session, edges, session.TrackColours[SCHEME_TRACK], height, floorSpritesCork, stationObject);

    track_paint_util_paint_fences(
        session, edges, session.MapPosition, trackElement, ride, session.TrackColours[SCHEME_SUPPORTS], height,
        fenceSpritesRope, session.CurrentRotation);

    switch (trackSequence)
    {
        case 1:
            PaintCircusTent(session, ride, direction, 32, 32, height);
            break;
        case 3:
            PaintCircusTent(session, ride, direction, 32, -32, height);
            break;
        case 5:
            PaintCircusTent(session, ride, direction, 0, -32, height);
            break;
        case 6:
            PaintCircusTent(session, ride, direction, -32, 32, height);
            break;
        case 7:
            PaintCircusTent(session, ride, direction, -32, -32, height);
            break;
        case 8:
            PaintCircusTent(session, ride, direction, -32, 0, height);
            break;
    }

    int32_t cornerSegments = 0;
    switch (trackSequence)
    {
        case 1:
            // Top
            cornerSegments = SEGMENT_B4 | SEGMENT_C8 | SEGMENT_CC;
            break;
        case 3:
            // Right
            cornerSegments = SEGMENT_CC | SEGMENT_BC | SEGMENT_D4;
            break;
        case 6:
            // Left
            cornerSegments = SEGMENT_C8 | SEGMENT_B8 | SEGMENT_D0;
            break;
        case 7:
            // Bottom
            cornerSegments = SEGMENT_D0 | SEGMENT_C0 | SEGMENT_D4;
            break;
    }

    PaintUtilSetSegmentSupportHeight(session, cornerSegments, height + 2, 0x20);
    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL & ~cornerSegments, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 128, 0x20);
}

TRACK_PAINT_FUNCTION GetTrackPaintFunctionCircus(int32_t trackType)
{
    if (trackType != TrackElemType::FlatTrack3x3)
    {
        return nullptr;
    }

    return PaintCircus;
}
