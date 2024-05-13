/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../common.h"
#include "../../entity/EntityRegistry.h"
#include "../../entity/Guest.h"
#include "../../interface/Viewport.h"
#include "../../paint/Paint.h"
#include "../../paint/support/WoodenSupports.h"
#include "../../paint/tile_element/Segment.h"
#include "../../paint/track/Segment.h"
#include "../RideEntry.h"
#include "../Track.h"
#include "../TrackPaint.h"
#include "../Vehicle.h"
enum
{
    SprSpaceRingsFenceNe = 22146,
    SprSpaceRingsFenceSe = 22147,
    SprSpaceRingsFenceSw = 22148,
    SprSpaceRingsFenceNw = 22149,
};

static constexpr uint32_t SpaceRingsFenceSprites[] = {
    SprSpaceRingsFenceNe,
    SprSpaceRingsFenceSe,
    SprSpaceRingsFenceSw,
    SprSpaceRingsFenceNw,
};

/** rct2: 0x00768A3B */
static void PaintSpaceRingsStructure(
    PaintSession& session, const Ride& ride, uint8_t direction, uint32_t segment, int32_t height, ImageId stationColour)
{
    uint32_t vehicleIndex = (segment - direction) & 0x3;

    if (ride.num_stations == 0 || vehicleIndex < ride.NumTrains)
    {
        const auto* rideEntry = GetRideEntryByIndex(ride.subtype);

        int32_t frameNum = direction;

        uint32_t baseImageId = rideEntry->Cars[0].base_image_id;
        auto vehicle = GetEntity<Vehicle>(ride.vehicles[vehicleIndex]);
        if (ride.lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK && vehicle != nullptr)
        {
            session.InteractionType = ViewportInteractionItem::Entity;
            session.CurrentlyDrawnEntity = vehicle;
            frameNum += static_cast<int8_t>(vehicle->Pitch) * 4;
        }

        if ((ride.colour_scheme_type & 3) != RIDE_COLOUR_SCHEME_MODE_DIFFERENT_PER_TRAIN)
        {
            vehicleIndex = 0;
        }

        if (stationColour == TrackStationColour)
        {
            stationColour = ImageId(0, ride.vehicle_colours[vehicleIndex].Body, ride.vehicle_colours[vehicleIndex].Trim);
        }

        auto imageId = stationColour.WithIndex(baseImageId + frameNum);
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { -10, -10, height }, { 20, 20, 23 } });

        if (vehicle != nullptr && vehicle->num_peeps > 0)
        {
            auto* rider = GetEntity<Guest>(vehicle->peep[0]);
            if (rider != nullptr)
            {
                stationColour = ImageId(0, rider->TshirtColour, rider->TrousersColour);
                imageId = stationColour.WithIndex(baseImageId + 352 + frameNum);
                PaintAddImageAsChild(session, imageId, { 0, 0, height }, { { -10, -10, height }, { 20, 20, 23 } });
            }
        }
    }

    session.CurrentlyDrawnEntity = nullptr;
    session.InteractionType = ViewportInteractionItem::Ride;
}

/** rct2: 0x00767C40 */
static void PaintSpaceRings(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = kTrackMap3x3[direction][trackSequence];

    int32_t edges = kEdges3x3[trackSequence];
    CoordsXY position = session.MapPosition;

    ImageId imageId;

    auto stationColour = GetStationColourScheme(session, trackElement);
    WoodenASupportsPaintSetupRotated(
        session, WoodenSupportType::Truss, WoodenSupportSubType::NeSw, direction, height, stationColour);

    const StationObject* stationObject = ride.GetStationObject();
    TrackPaintUtilPaintFloor(session, edges, session.TrackColours, height, kFloorSpritesCork, stationObject);

    switch (trackSequence)
    {
        case 7:
            if (TrackPaintUtilHasFence(EDGE_SW, position, trackElement, ride, session.CurrentRotation))
            {
                imageId = stationColour.WithIndex(SprSpaceRingsFenceSw);
                PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 29, 0, height + 2 }, { 1, 28, 7 } });
            }
            if (TrackPaintUtilHasFence(EDGE_SE, position, trackElement, ride, session.CurrentRotation))
            {
                imageId = stationColour.WithIndex(SprSpaceRingsFenceSe);
                PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 29, height + 2 }, { 28, 1, 7 } });
            }
            break;
        default:
            TrackPaintUtilPaintFences(
                session, edges, position, trackElement, ride, stationColour, height, SpaceRingsFenceSprites,
                session.CurrentRotation);
            break;
    }

    switch (trackSequence)
    {
        case 0:
            PaintSpaceRingsStructure(session, ride, direction, 0, height + 3, stationColour);
            break;
        case 5:
            PaintSpaceRingsStructure(session, ride, direction, 1, height + 3, stationColour);
            break;
        case 7:
            PaintSpaceRingsStructure(session, ride, direction, 2, height + 3, stationColour);
            break;
        case 8:
            PaintSpaceRingsStructure(session, ride, direction, 3, height + 3, stationColour);
            break;
    }

    int32_t cornerSegments = 0;
    switch (trackSequence)
    {
        case 0:
            cornerSegments = 0;
            break;
        case 1:
            cornerSegments = EnumsToFlags(
                PaintSegment::leftCorner, PaintSegment::topLeftSide, PaintSegment::topCorner, PaintSegment::topRightSide,
                PaintSegment::rightCorner);
            break;
        case 2:
            cornerSegments = EnumsToFlags(PaintSegment::topCorner, PaintSegment::topRightSide, PaintSegment::rightCorner);
            break;
        case 3:
            cornerSegments = EnumsToFlags(
                PaintSegment::topCorner, PaintSegment::topRightSide, PaintSegment::rightCorner, PaintSegment::bottomRightSide,
                PaintSegment::bottomCorner);
            break;
        case 4:
            cornerSegments = EnumsToFlags(PaintSegment::topCorner, PaintSegment::topLeftSide, PaintSegment::leftCorner);
            break;
        case 5:
            cornerSegments = EnumsToFlags(PaintSegment::rightCorner, PaintSegment::bottomRightSide, PaintSegment::bottomCorner);
            break;
        case 6:
            cornerSegments = EnumsToFlags(
                PaintSegment::topCorner, PaintSegment::topLeftSide, PaintSegment::leftCorner, PaintSegment::bottomLeftSide,
                PaintSegment::bottomCorner);
            break;
        case 7:
            cornerSegments = EnumsToFlags(
                PaintSegment::leftCorner, PaintSegment::bottomLeftSide, PaintSegment::bottomCorner,
                PaintSegment::bottomRightSide, PaintSegment::rightCorner);
            break;
        case 8:
            cornerSegments = EnumsToFlags(PaintSegment::leftCorner, PaintSegment::bottomLeftSide, PaintSegment::bottomCorner);
            break;
    }
    PaintUtilSetSegmentSupportHeight(session, cornerSegments, height + 2, 0x20);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll & ~cornerSegments, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48);
}

/**
 * rct2: 0x0x00767A40
 */
TRACK_PAINT_FUNCTION GetTrackPaintFunctionSpaceRings(int32_t trackType)
{
    if (trackType != TrackElemType::FlatTrack3x3)
    {
        return nullptr;
    }

    return PaintSpaceRings;
}
