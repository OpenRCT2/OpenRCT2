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
#include "../../ride/Vehicle.h"
#include "../Ride.h"
#include "../RideEntry.h"
#include "../Track.h"
#include "../TrackPaint.h"

static constexpr const BoundBoxXY CrookedHouseData[] = {
    {
        { 6, 0 },
        { 42, 24 },
    },
    {
        { 0, 0 },
        { 0, 0 },
    },
    {
        { -16, -16 },
        { 32, 32 },
    },
    {
        // Unused
        { 0, 0 },
        { 0, 0 },
    },
    {
        { 0, 6 },
        { 24, 42 },
    },
};

/**
 *  rct2: 0x0088ABA4
 */
static void PaintCrookedHouseStructure(
    PaintSession& session, uint8_t direction, int32_t x_offset, int32_t y_offset, uint32_t segment, int32_t height)
{
    const auto* tileElement = session.CurrentlyDrawnTileElement;
    if (tileElement == nullptr)
        return;

    auto ride = get_ride(tileElement->AsTrack()->GetRideIndex());
    if (ride == nullptr)
        return;

    auto rideEntry = ride->GetRideEntry();
    if (rideEntry == nullptr)
        return;

    if (ride->lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK)
    {
        auto vehicle = GetEntity<Vehicle>(ride->vehicles[0]);
        if (vehicle != nullptr)
        {
            session.InteractionType = ViewportInteractionItem::Entity;
            session.CurrentlyDrawnEntity = vehicle;
        }
    }

    const auto& boundBox = CrookedHouseData[segment];
    auto imageTemplate = session.TrackColours[SCHEME_MISC];
    auto imageIndex = rideEntry->Cars[0].base_image_id + direction;
    PaintAddImageAsParent(
        session, imageTemplate.WithIndex(imageIndex), { x_offset, y_offset, height + 3 }, { boundBox.length, 127 },
        { boundBox.offset, height + 3 });

    session.CurrentlyDrawnEntity = nullptr;
}

static void PaintCrookedHouse(
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
            PaintCrookedHouseStructure(session, direction, 32, -32, 0, height);
            break;
        case 6:
            PaintCrookedHouseStructure(session, direction, -32, 32, 4, height);
            break;
        case 7:
            PaintCrookedHouseStructure(session, direction, -32, -32, 2, height);
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

TRACK_PAINT_FUNCTION GetTrackPaintFunctionCrookedHouse(int32_t trackType)
{
    if (trackType != TrackElemType::FlatTrack3x3)
    {
        return nullptr;
    }

    return PaintCrookedHouse;
}
