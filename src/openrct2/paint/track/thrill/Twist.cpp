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
#include "../../tile_element/Segment.h"
#include "../../track/Segment.h"

using namespace OpenRCT2;

/** rct2: 0x0076E5C9 */
static void PaintTwistStructure(
    PaintSession& session, const Ride& ride, uint8_t direction, int8_t xOffset, int8_t yOffset, uint16_t height,
    ImageId stationColour)
{
    const auto* rideEntry = GetRideEntryByIndex(ride.subtype);
    Vehicle* vehicle = nullptr;

    if (rideEntry == nullptr)
    {
        return;
    }

    height += 7;

    if (ride.lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK && !ride.vehicles[0].IsNull())
    {
        vehicle = GetEntity<Vehicle>(ride.vehicles[0]);

        session.InteractionType = ViewportInteractionItem::Entity;
        session.CurrentlyDrawnEntity = vehicle;
    }

    uint32_t frameNum = (direction * 88) % 216;
    if (vehicle != nullptr)
    {
        frameNum += (vehicle->Orientation >> 3) << 4;
        frameNum += vehicle->Pitch;
        frameNum = frameNum % 216;
    }

    auto imageTemplate = ImageId(0, ride.vehicle_colours[0].Body, ride.vehicle_colours[0].Trim);
    if (stationColour != TrackStationColour)
    {
        imageTemplate = stationColour;
    }

    auto baseImageId = rideEntry->Cars[0].base_image_id;
    auto structureFrameNum = frameNum % 24;
    auto imageId = imageTemplate.WithIndex(baseImageId + structureFrameNum);
    const BoundBoxXYZ bb = {
        { xOffset + 16, yOffset + 16, height },
        { 24, 24, 48 },
    };
    PaintAddImageAsParent(session, imageId, { xOffset, yOffset, height }, bb);

    if (session.DPI.zoom_level < ZoomLevel{ 1 } && ride.lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK && vehicle != nullptr)
    {
        for (int32_t i = 0; i < vehicle->num_peeps; i += 2)
        {
            imageTemplate = ImageId(0, vehicle->peep_tshirt_colours[i], vehicle->peep_tshirt_colours[i + 1]);
            auto peepFrameNum = (frameNum + i * 12) % 216;
            imageId = imageTemplate.WithIndex(baseImageId + 24 + peepFrameNum);
            PaintAddImageAsChild(session, imageId, { xOffset, yOffset, height }, bb);
        }
    }

    session.CurrentlyDrawnEntity = nullptr;
    session.InteractionType = ViewportInteractionItem::Ride;
}

/** rct2: 0x0076D858 */
static void PaintTwist(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kTrackMap3x3[direction][trackSequence];

    const uint8_t edges = kEdges3x3[trackSequence];

    ImageId imageId;

    auto stationColour = GetStationColourScheme(session, trackElement);
    WoodenASupportsPaintSetupRotated(
        session, WoodenSupportType::Truss, WoodenSupportSubType::NeSw, direction, height, stationColour);

    const StationObject* stationObject = ride.GetStationObject();
    TrackPaintUtilPaintFloor(session, edges, stationColour, height, kFloorSpritesCork, stationObject);

    switch (trackSequence)
    {
        case 7:
            if (TrackPaintUtilHasFence(EDGE_SW, session.MapPosition, trackElement, ride, session.CurrentRotation))
            {
                imageId = stationColour.WithIndex(SPR_FENCE_ROPE_SW);
                PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 29, 0, height + 3 }, { 1, 28, 7 } });
            }
            if (TrackPaintUtilHasFence(EDGE_SE, session.MapPosition, trackElement, ride, session.CurrentRotation))
            {
                imageId = stationColour.WithIndex(SPR_FENCE_ROPE_SE);
                PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 29, height + 3 }, { 28, 1, 7 } });
            }
            break;
        default:
            TrackPaintUtilPaintFences(
                session, edges, session.MapPosition, trackElement, ride, GetStationColourScheme(session, trackElement), height,
                kFenceSpritesRope, session.CurrentRotation);
            break;
    }

    switch (trackSequence)
    {
        case 1:
            PaintTwistStructure(session, ride, direction, 32, 32, height, stationColour);
            break;
        case 3:
            PaintTwistStructure(session, ride, direction, 32, -32, height, stationColour);
            break;
        case 5:
            PaintTwistStructure(session, ride, direction, 0, -32, height, stationColour);
            break;
        case 6:
            PaintTwistStructure(session, ride, direction, -32, 32, height, stationColour);
            break;
        case 7:
            PaintTwistStructure(session, ride, direction, -32, -32, height, stationColour);
            break;
        case 8:
            PaintTwistStructure(session, ride, direction, -32, 0, height, stationColour);
            break;
    }

    int32_t cornerSegments = 0;
    switch (trackSequence)
    {
        case 1:
            cornerSegments = EnumsToFlags(PaintSegment::topCorner, PaintSegment::topLeftSide, PaintSegment::topRightSide);
            break;
        case 3:
            cornerSegments = EnumsToFlags(PaintSegment::topRightSide, PaintSegment::rightCorner, PaintSegment::bottomRightSide);
            break;
        case 6:
            cornerSegments = EnumsToFlags(PaintSegment::topLeftSide, PaintSegment::leftCorner, PaintSegment::bottomLeftSide);
            break;
        case 7:
            cornerSegments = EnumsToFlags(
                PaintSegment::bottomLeftSide, PaintSegment::bottomCorner, PaintSegment::bottomRightSide);
            break;
    }

    PaintUtilSetSegmentSupportHeight(session, cornerSegments, height + 2, 0x20);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll & ~cornerSegments, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 64);
}

/**
 * rct2: 0x0076D658
 */
TrackPaintFunction GetTrackPaintFunctionTwist(OpenRCT2::TrackElemType trackType)
{
    if (trackType != TrackElemType::FlatTrack3x3)
    {
        return TrackPaintFunctionDummy;
    }

    return PaintTwist;
}
