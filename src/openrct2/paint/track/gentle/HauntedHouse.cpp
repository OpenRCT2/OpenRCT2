/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../../entity/EntityRegistry.h"
#include "../../../interface/Viewport.h"
#include "../../../ride/Ride.h"
#include "../../../ride/RideEntry.h"
#include "../../../ride/Track.h"
#include "../../../ride/TrackPaint.h"
#include "../../../ride/Vehicle.h"
#include "../../Boundbox.h"
#include "../../Paint.h"
#include "../../support/WoodenSupports.h"
#include "../../support/WoodenSupports.hpp"
#include "../../tile_element/Segment.h"
#include "../../track/Segment.h"

using namespace OpenRCT2;

static constexpr BoundBoxXY kHauntedHouseData[] = {
    { { 6, 0 }, { 42, 24 } }, { { 0, 0 }, { 0, 0 } },   { { -16, -16 }, { 32, 32 } },
    { { 0, 0 }, { 0, 0 } },   { { 0, 6 }, { 24, 42 } }, { { 0, 0 }, { 0, 0 } },
};

static void PaintHauntedHouseStructure(
    PaintSession& session, const Ride& ride, uint8_t direction, int8_t xOffset, int8_t yOffset, uint8_t part, uint16_t height,
    ImageId stationColour)
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

    const auto& boundBox = kHauntedHouseData[part];
    auto baseImageIndex = rideEntry->Cars[0].base_image_id;
    auto imageIndex = baseImageIndex + direction;

    auto bb = BoundBoxXYZ{ { boundBox.offset, height }, { boundBox.length, 127 } };
    PaintAddImageAsParent(session, stationColour.WithIndex(imageIndex), { xOffset, yOffset, height }, bb);

    if (session.DPI.zoom_level <= ZoomLevel{ 0 } && frameNum != 0)
    {
        imageIndex = baseImageIndex + 3 + ((direction & 3) * 18) + frameNum;
        PaintAddImageAsChild(
            session, stationColour.WithIndex(imageIndex), { xOffset, yOffset, height },
            { { boundBox.offset, height }, { boundBox.length, 127 } });
    }

    session.CurrentlyDrawnEntity = nullptr;
    session.InteractionType = ViewportInteractionItem::Ride;
}

static void PaintHauntedHouse(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kTrackMap3x3[direction][trackSequence];

    int32_t edges = kEdges3x3[trackSequence];

    DrawSupportForSequenceA<TrackElemType::FlatTrack3x3>(
        session, supportType.wooden, trackSequence, direction, height, GetStationColourScheme(session, trackElement));

    const StationObject* stationObject = ride.GetStationObject();

    TrackPaintUtilPaintFloor(session, edges, session.TrackColours, height, kFloorSpritesCork, stationObject);

    TrackPaintUtilPaintFences(
        session, edges, session.MapPosition, trackElement, ride, GetStationColourScheme(session, trackElement), height,
        kFenceSpritesRope, session.CurrentRotation);

    auto stationColour = GetStationColourScheme(session, trackElement);
    switch (trackSequence)
    {
        case 3:
            PaintHauntedHouseStructure(session, ride, direction, 32, -32, 0, height + 3, stationColour);
            break;
        case 6:
            PaintHauntedHouseStructure(session, ride, direction, -32, 32, 4, height + 3, stationColour);
            break;
        case 7:
            PaintHauntedHouseStructure(session, ride, direction, -32, -32, 2, height + 3, stationColour);
            break;
    }

    int32_t cornerSegments = 0;
    switch (trackSequence)
    {
        case 1:
            // top
            cornerSegments = EnumsToFlags(PaintSegment::topCorner, PaintSegment::topLeftSide, PaintSegment::topRightSide);
            break;
        case 3:
            // right
            cornerSegments = EnumsToFlags(PaintSegment::topRightSide, PaintSegment::rightCorner, PaintSegment::bottomRightSide);
            break;
        case 6:
            // left
            cornerSegments = EnumsToFlags(PaintSegment::topLeftSide, PaintSegment::leftCorner, PaintSegment::bottomLeftSide);
            break;
        case 7:
            // bottom
            cornerSegments = EnumsToFlags(
                PaintSegment::bottomLeftSide, PaintSegment::bottomCorner, PaintSegment::bottomRightSide);
            break;
    }

    PaintUtilSetSegmentSupportHeight(session, cornerSegments, height + 2, 0x20);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll & ~cornerSegments, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 128);
}

TrackPaintFunction GetTrackPaintFunctionHauntedHouse(OpenRCT2::TrackElemType trackType)
{
    if (trackType != TrackElemType::FlatTrack3x3)
    {
        return TrackPaintFunctionDummy;
    }

    return PaintHauntedHouse;
}
