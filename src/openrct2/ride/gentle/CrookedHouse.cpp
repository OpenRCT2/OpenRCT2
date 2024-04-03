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
#include "../../paint/Boundbox.h"
#include "../../paint/Paint.h"
#include "../../paint/support/WoodenSupports.h"
#include "../../ride/Vehicle.h"
#include "../Ride.h"
#include "../RideEntry.h"
#include "../Track.h"
#include "../TrackPaint.h"

static constexpr BoundBoxXY CrookedHouseData[] = {
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
    PaintSession& session, uint8_t direction, int32_t x_offset, int32_t y_offset, uint32_t segment, int32_t height,
    ImageId stationColour)
{
    const auto* tileElement = session.CurrentlyDrawnTileElement;
    if (tileElement == nullptr)
        return;

    auto ride = GetRide(tileElement->AsTrack()->GetRideIndex());
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
    auto imageIndex = rideEntry->Cars[0].base_image_id + direction;
    PaintAddImageAsParent(
        session, stationColour.WithIndex(imageIndex), { x_offset, y_offset, height + 3 },
        { { boundBox.offset, height + 3 }, { boundBox.length, 127 } });

    session.CurrentlyDrawnEntity = nullptr;
}

static void PaintCrookedHouse(
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
        session, edges, session.MapPosition, trackElement, ride, GetStationColourScheme(session, trackElement), height,
        fenceSpritesRope, session.CurrentRotation);

    auto stationColour = GetStationColourScheme(session, trackElement);
    switch (trackSequence)
    {
        case 3:
            PaintCrookedHouseStructure(session, direction, 32, -32, 0, height, stationColour);
            break;
        case 6:
            PaintCrookedHouseStructure(session, direction, -32, 32, 4, height, stationColour);
            break;
        case 7:
            PaintCrookedHouseStructure(session, direction, -32, -32, 2, height, stationColour);
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

TRACK_PAINT_FUNCTION GetTrackPaintFunctionCrookedHouse(int32_t trackType)
{
    if (trackType != TrackElemType::kFlatTrack3x3)
    {
        return nullptr;
    }

    return PaintCrookedHouse;
}
