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
#include "../../paint/tile_element/Segment.h"
#include "../../paint/track/Segment.h"
#include "../Ride.h"
#include "../RideEntry.h"
#include "../Track.h"
#include "../TrackPaint.h"
#include "../Vehicle.h"

using namespace OpenRCT2;

static void PaintEnterpriseRiders(
    PaintSession& session, const RideObjectEntry& rideEntry, Vehicle& vehicle, uint32_t imageOffset, const CoordsXYZ& offset,
    const BoundBoxXYZ& bb)
{
    if (session.DPI.zoom_level > ZoomLevel{ 0 })
        return;
    if (imageOffset >= 12)
        return;

    auto baseImageIndex = rideEntry.Cars[0].base_image_id;
    for (int32_t i = 0; i < 15; i++)
    {
        if (vehicle.num_peeps <= i)
            break;

        auto frameOffset1 = ((imageOffset % 4) * 4 + (i * 4) % 15) & 0x0F;
        auto frameOffset2 = Floor2(imageOffset, 4) * 4;
        auto imageTemplate = ImageId(0, vehicle.peep_tshirt_colours[i]);
        auto imageId = imageTemplate.WithIndex(baseImageIndex + 196 + frameOffset1 + frameOffset2);
        PaintAddImageAsChild(session, imageId, offset, bb);
    }
}

static void PaintEnterpriseStructure(
    PaintSession& session, const Ride& ride, int8_t xOffset, int8_t yOffset, uint16_t height, const TrackElement& trackElement)
{
    const auto* rideEntry = GetRideEntryByIndex(ride.subtype);
    if (rideEntry == nullptr)
        return;

    Vehicle* vehicle = nullptr;
    if (ride.lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK)
    {
        vehicle = GetEntity<Vehicle>(ride.vehicles[0]);
        if (vehicle != nullptr)
        {
            session.InteractionType = ViewportInteractionItem::Entity;
            session.CurrentlyDrawnEntity = vehicle;
        }
    }

    CoordsXYZ offset(xOffset, yOffset, height + 7);
    BoundBoxXYZ bb = { { 0, 0, height + 7 }, { 24, 24, 48 } };

    uint32_t imageOffset = trackElement.GetDirectionWithOffset(session.CurrentRotation);
    if (vehicle != nullptr)
    {
        imageOffset = (vehicle->Pitch << 2) + (((vehicle->Orientation >> 3) + session.CurrentRotation) % 4);
    }

    auto imageTemplate = ImageId(0, ride.vehicle_colours[0].Body, ride.vehicle_colours[0].Trim);
    auto imageFlags = GetStationColourScheme(session, trackElement);
    if (imageFlags != TrackStationColour)
    {
        imageTemplate = imageFlags;
    }
    auto imageId = imageTemplate.WithIndex(rideEntry->Cars[0].base_image_id + imageOffset);
    PaintAddImageAsParent(session, imageId, offset, bb);

    if (vehicle != nullptr)
    {
        PaintEnterpriseRiders(session, *rideEntry, *vehicle, imageOffset, offset, bb);
    }

    session.CurrentlyDrawnEntity = nullptr;
    session.InteractionType = ViewportInteractionItem::Ride;
}

static void PaintEnterprise(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = kTrackMap4x4[direction][trackSequence];

    int32_t edges = kEdges4x4[trackSequence];

    WoodenASupportsPaintSetupRotated(
        session, WoodenSupportType::Truss, WoodenSupportSubType::NeSw, direction, height,
        GetStationColourScheme(session, trackElement));

    const StationObject* stationObject = ride.GetStationObject();
    TrackPaintUtilPaintFloor(session, edges, session.TrackColours, height, kFloorSpritesCork, stationObject);

    TrackPaintUtilPaintFences(
        session, edges, session.MapPosition, trackElement, ride, session.TrackColours, height, kFenceSpritesRope,
        session.CurrentRotation);

    switch (trackSequence)
    {
        case 5:
            PaintEnterpriseStructure(session, ride, 16, 16, height, trackElement);
            break;
        case 6:
            PaintEnterpriseStructure(session, ride, 16, -16, height, trackElement);
            break;
        case 10:
            PaintEnterpriseStructure(session, ride, -16, -16, height, trackElement);
            break;
        case 9:
            PaintEnterpriseStructure(session, ride, -16, 16, height, trackElement);
            break;

        case 0:
            PaintEnterpriseStructure(session, ride, 48, 48, height, trackElement);
            break;
        case 3:
            PaintEnterpriseStructure(session, ride, 48, -48, height, trackElement);
            break;
        case 15:
            PaintEnterpriseStructure(session, ride, -48, -48, height, trackElement);
            break;
        case 12:
            PaintEnterpriseStructure(session, ride, -48, 48, height, trackElement);
            break;

        case 7:
            PaintEnterpriseStructure(session, ride, 16, -48, height, trackElement);
            break;
        case 11:
            PaintEnterpriseStructure(session, ride, -16, -48, height, trackElement);
            break;
        case 14:
            PaintEnterpriseStructure(session, ride, -48, -16, height, trackElement);
            break;
        case 13:
            PaintEnterpriseStructure(session, ride, -48, 16, height, trackElement);
            break;
    }

    int32_t cornerSegments = 0;
    switch (trackSequence)
    {
        case 0:
            cornerSegments = EnumsToFlags(PaintSegment::topCorner, PaintSegment::topLeftSide, PaintSegment::topRightSide);
            break;
        case 3:
            cornerSegments = EnumsToFlags(PaintSegment::topRightSide, PaintSegment::rightCorner, PaintSegment::bottomRightSide);
            break;
        case 12:
            cornerSegments = EnumsToFlags(PaintSegment::topLeftSide, PaintSegment::leftCorner, PaintSegment::bottomLeftSide);
            break;
        case 15:
            cornerSegments = EnumsToFlags(
                PaintSegment::bottomLeftSide, PaintSegment::bottomCorner, PaintSegment::bottomRightSide);
            break;
    }
    PaintUtilSetSegmentSupportHeight(session, cornerSegments, height + 2, 0x20);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll & ~cornerSegments, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 160);
}

TRACK_PAINT_FUNCTION GetTrackPaintFunctionEnterprise(int32_t trackType)
{
    if (trackType != TrackElemType::FlatTrack4x4)
    {
        return nullptr;
    }

    return PaintEnterprise;
}
