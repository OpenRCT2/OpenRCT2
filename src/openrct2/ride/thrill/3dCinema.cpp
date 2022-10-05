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
#include "../Ride.h"
#include "../RideEntry.h"
#include "../Track.h"
#include "../TrackPaint.h"
#include "../Vehicle.h"

static void Paint3dCinemaDome(
    paint_session& session, const Ride& ride, uint8_t direction, int8_t xOffset, int8_t yOffset, uint16_t height)
{
    auto rideEntry = ride.GetRideEntry();
    if (rideEntry == nullptr)
        return;

    if (ride.lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK && !ride.vehicles[0].IsNull())
    {
        session.InteractionType = ViewportInteractionItem::Entity;
        session.CurrentlyDrawnEntity = GetEntity<Vehicle>(ride.vehicles[0]);
    }

    auto imageTemplate = ImageId(0, ride.vehicle_colours[0].Body, ride.vehicle_colours[0].Trim);
    auto imageFlags = session.TrackColours[SCHEME_MISC];
    if (imageFlags.ToUInt32() != IMAGE_TYPE_REMAP)
    {
        imageTemplate = imageFlags;
    }

    auto imageId = imageTemplate.WithIndex(rideEntry->Cars[0].base_image_id + direction);
    PaintAddImageAsParent(
        session, imageId, { xOffset, yOffset, height + 3 }, { 24, 24, 47 }, { xOffset + 16, yOffset + 16, height + 3 });

    session.CurrentlyDrawnEntity = nullptr;
    session.InteractionType = ViewportInteractionItem::Ride;
}

/**
 * rct2: 0x0076574C
 */
static void Paint3dCinema(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = track_map_3x3[direction][trackSequence];

    int32_t edges = edges_3x3[trackSequence];

    wooden_a_supports_paint_setup(session, (direction & 1), 0, height, session.TrackColours[SCHEME_MISC]);

    const StationObject* stationObject = ride.GetStationObject();

    track_paint_util_paint_floor(session, edges, session.TrackColours[SCHEME_TRACK], height, floorSpritesCork, stationObject);

    track_paint_util_paint_fences(
        session, edges, session.MapPosition, trackElement, ride, session.TrackColours[SCHEME_MISC], height, fenceSpritesRope,
        session.CurrentRotation);

    switch (trackSequence)
    {
        case 1:
            Paint3dCinemaDome(session, ride, direction, 32, 32, height);
            break;
        case 3:
            Paint3dCinemaDome(session, ride, direction, 32, -32, height);
            break;
        case 5:
            Paint3dCinemaDome(session, ride, direction, 0, -32, height);
            break;
        case 6:
            Paint3dCinemaDome(session, ride, direction, -32, 32, height);
            break;
        case 7:
            Paint3dCinemaDome(session, ride, direction, -32, -32, height);
            break;
        case 8:
            Paint3dCinemaDome(session, ride, direction, -32, 0, height);
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

    paint_util_set_segment_support_height(session, cornerSegments, height + 2, 0x20);
    paint_util_set_segment_support_height(session, SEGMENTS_ALL & ~cornerSegments, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 128, 0x20);
}

TRACK_PAINT_FUNCTION get_track_paint_function_3d_cinema(int32_t trackType)
{
    if (trackType != TrackElemType::FlatTrack3x3)
    {
        return nullptr;
    }

    return Paint3dCinema;
}
