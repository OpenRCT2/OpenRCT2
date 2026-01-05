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

using namespace OpenRCT2;

static void Paint3dCinemaDome(
    PaintSession& session, const Ride& ride, uint8_t direction, int8_t xOffset, int8_t yOffset, uint16_t height,
    ImageId stationColour)
{
    auto rideEntry = ride.getRideEntry();
    if (rideEntry == nullptr)
        return;

    if (ride.lifecycleFlags & RIDE_LIFECYCLE_ON_TRACK && !ride.vehicles[0].IsNull())
    {
        session.InteractionType = ViewportInteractionItem::entity;
        session.CurrentlyDrawnEntity = getGameState().entities.GetEntity<Vehicle>(ride.vehicles[0]);
    }

    auto imageTemplate = ImageId(0, ride.vehicleColours[0].Body, ride.vehicleColours[0].Trim);
    if (stationColour != TrackStationColour)
    {
        imageTemplate = stationColour;
    }

    auto imageId = imageTemplate.WithIndex(rideEntry->Cars[0].base_image_id + direction);
    PaintAddImageAsParent(
        session, imageId, { xOffset, yOffset, height + 3 }, { { xOffset + 16, yOffset + 16, height + 3 }, { 24, 24, 47 } });

    session.CurrentlyDrawnEntity = nullptr;
    session.InteractionType = ViewportInteractionItem::ride;
}

/**
 * rct2: 0x0076574C
 */
static void Paint3dCinema(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kTrackMap3x3[direction][trackSequence];

    int32_t edges = kEdges3x3[trackSequence];

    auto stationColour = GetStationColourScheme(session, trackElement);
    WoodenASupportsPaintSetupRotated(
        session, WoodenSupportType::truss, WoodenSupportSubType::neSw, direction, height, stationColour);

    const StationObject* stationObject = ride.getStationObject();

    TrackPaintUtilPaintFloor(session, edges, session.TrackColours, height, kFloorSpritesMulch, stationObject);

    TrackPaintUtilPaintFences(
        session, edges, session.MapPosition, trackElement, ride, stationColour, height, kFenceSpritesRope,
        session.CurrentRotation);

    switch (trackSequence)
    {
        case 1:
            Paint3dCinemaDome(session, ride, direction, 32, 32, height, stationColour);
            break;
        case 3:
            Paint3dCinemaDome(session, ride, direction, 32, -32, height, stationColour);
            break;
        case 5:
            Paint3dCinemaDome(session, ride, direction, 0, -32, height, stationColour);
            break;
        case 6:
            Paint3dCinemaDome(session, ride, direction, -32, 32, height, stationColour);
            break;
        case 7:
            Paint3dCinemaDome(session, ride, direction, -32, -32, height, stationColour);
            break;
        case 8:
            Paint3dCinemaDome(session, ride, direction, -32, 0, height, stationColour);
            break;
    }

    int32_t cornerSegments = 0;
    switch (trackSequence)
    {
        case 1:
            // top
            cornerSegments = EnumsToFlags(PaintSegment::top, PaintSegment::topLeft, PaintSegment::topRight);
            break;
        case 3:
            // right
            cornerSegments = EnumsToFlags(PaintSegment::topRight, PaintSegment::right, PaintSegment::bottomRight);
            break;
        case 6:
            // left
            cornerSegments = EnumsToFlags(PaintSegment::topLeft, PaintSegment::left, PaintSegment::bottomLeft);
            break;
        case 7:
            // bottom
            cornerSegments = EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::bottom, PaintSegment::bottomRight);
            break;
    }

    PaintUtilSetSegmentSupportHeight(session, cornerSegments, height + 2, 0x20);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll & ~cornerSegments, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 128);
}

TrackPaintFunction GetTrackPaintFunction3dCinema(TrackElemType trackType)
{
    if (trackType != TrackElemType::flatTrack3x3)
    {
        return TrackPaintFunctionDummy;
    }

    return Paint3dCinema;
}
