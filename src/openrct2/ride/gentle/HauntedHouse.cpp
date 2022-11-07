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
#include "../../paint/Boundbox.h"
#include "../../paint/Paint.h"
#include "../../paint/Supports.h"
#include "../Ride.h"
#include "../RideEntry.h"
#include "../Track.h"
#include "../TrackPaint.h"
#include "../Vehicle.h"

static constexpr BoundBoxXY HauntedHouseData[] = {
    { { 6, 0 }, { 42, 24 } }, { { 0, 0 }, { 0, 0 } },   { { -16, -16 }, { 32, 32 } },
    { { 0, 0 }, { 0, 0 } },   { { 0, 6 }, { 24, 42 } }, { { 0, 0 }, { 0, 0 } },
};

static void PaintHauntedHouseStructure(
    PaintSession& session, const Ride& ride, uint8_t direction, int8_t xOffset, int8_t yOffset, uint8_t part, uint16_t height)
{
    uint8_t frameNum = 0;

    auto rideEntry = ride.GetRideEntry();
    if (rideEntry == nullptr)
        return;

    auto vehicle = GetEntity<Vehicle>(ride.vehicles[0]);
    if (ride.lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK && vehicle != nullptr)
    {
        session.InteractionType = ViewportInteractionItem::Entity;
        session.CurrentlyDrawnEntity = vehicle;
        frameNum = vehicle->Pitch;
    }

    const auto& boundBox = HauntedHouseData[part];
    auto imageTemplate = session.TrackColours[SCHEME_MISC];
    auto baseImageIndex = rideEntry->Cars[0].base_image_id;
    auto imageIndex = baseImageIndex + direction;
    PaintAddImageAsParent(
        session, imageTemplate.WithIndex(imageIndex), { xOffset, yOffset, height }, { boundBox.length, 127 },
        { boundBox.offset, height });

    if (session.DPI.zoom_level <= ZoomLevel{ 0 } && frameNum != 0)
    {
        imageIndex = baseImageIndex + 3 + ((direction & 3) * 18) + frameNum;
        PaintAddImageAsChild(
            session, imageTemplate.WithIndex(imageIndex), { xOffset, yOffset, height },
            { { boundBox.offset, height }, { boundBox.length, 127 } });
    }

    session.CurrentlyDrawnEntity = nullptr;
    session.InteractionType = ViewportInteractionItem::Ride;
}

static void PaintHauntedHouse(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = track_map_3x3[direction][trackSequence];

    int32_t edges = edges_3x3[trackSequence];

    WoodenASupportsPaintSetup(session, (direction & 1), 0, height, session.TrackColours[SCHEME_MISC]);

    const StationObject* stationObject = ride.GetStationObject();

    track_paint_util_paint_floor(session, edges, session.TrackColours[SCHEME_TRACK], height, floorSpritesCork, stationObject);

    track_paint_util_paint_fences(
        session, edges, session.MapPosition, trackElement, ride, session.TrackColours[SCHEME_MISC], height, fenceSpritesRope,
        session.CurrentRotation);

    switch (trackSequence)
    {
        case 3:
            PaintHauntedHouseStructure(session, ride, direction, 32, -32, 0, height + 3);
            break;
        case 6:
            PaintHauntedHouseStructure(session, ride, direction, -32, 32, 4, height + 3);
            break;
        case 7:
            PaintHauntedHouseStructure(session, ride, direction, -32, -32, 2, height + 3);
            break;
    }

    int32_t cornerSegments = 0;
    switch (trackSequence)
    {
        case 1:
            // top
            cornerSegments = SEGMENT_B4 | SEGMENT_C8 | SEGMENT_CC;
            break;
        case 3:
            // right
            cornerSegments = SEGMENT_CC | SEGMENT_BC | SEGMENT_D4;
            break;
        case 6:
            // left
            cornerSegments = SEGMENT_C8 | SEGMENT_B8 | SEGMENT_D0;
            break;
        case 7:
            // bottom
            cornerSegments = SEGMENT_D0 | SEGMENT_C0 | SEGMENT_D4;
            break;
    }

    PaintUtilSetSegmentSupportHeight(session, cornerSegments, height + 2, 0x20);
    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL & ~cornerSegments, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 128, 0x20);
}

TRACK_PAINT_FUNCTION GetTrackPaintFunctionHauntedHouse(int32_t trackType)
{
    if (trackType != TrackElemType::FlatTrack3x3)
    {
        return nullptr;
    }

    return PaintHauntedHouse;
}
