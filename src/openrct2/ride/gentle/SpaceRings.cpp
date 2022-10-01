/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
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
#include "../../paint/Supports.h"
#include "../RideEntry.h"
#include "../Track.h"
#include "../TrackPaint.h"
#include "../Vehicle.h"
enum
{
    SPR_SPACE_RINGS_FENCE_NE = 22146,
    SPR_SPACE_RINGS_FENCE_SE = 22147,
    SPR_SPACE_RINGS_FENCE_SW = 22148,
    SPR_SPACE_RINGS_FENCE_NW = 22149,
};

static constexpr const uint32_t space_rings_fence_sprites[] = {
    SPR_SPACE_RINGS_FENCE_NE,
    SPR_SPACE_RINGS_FENCE_SE,
    SPR_SPACE_RINGS_FENCE_SW,
    SPR_SPACE_RINGS_FENCE_NW,
};

/** rct2: 0x00768A3B */
static void paint_space_rings_structure(
    paint_session& session, const Ride& ride, uint8_t direction, uint32_t segment, int32_t height)
{
    uint32_t vehicleIndex = (segment - direction) & 0x3;

    if (ride.num_stations == 0 || vehicleIndex < ride.NumTrains)
    {
        rct_ride_entry* rideEntry = get_ride_entry(ride.subtype);

        int32_t frameNum = direction;

        uint32_t baseImageId = rideEntry->Cars[0].base_image_id;
        auto vehicle = GetEntity<Vehicle>(ride.vehicles[vehicleIndex]);
        if (ride.lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK && vehicle != nullptr)
        {
            session.InteractionType = ViewportInteractionItem::Entity;
            session.CurrentlyDrawnEntity = vehicle;
            frameNum += static_cast<int8_t>(vehicle->Pitch) * 4;
        }

        auto imageColourFlags = session.TrackColours[SCHEME_MISC];
        if ((ride.colour_scheme_type & 3) != RIDE_COLOUR_SCHEME_MODE_DIFFERENT_PER_TRAIN)
        {
            vehicleIndex = 0;
        }

        if (imageColourFlags.ToUInt32() == IMAGE_TYPE_REMAP)
        {
            imageColourFlags = ImageId(0, ride.vehicle_colours[vehicleIndex].Body, ride.vehicle_colours[vehicleIndex].Trim);
        }

        auto imageId = imageColourFlags.WithIndex(baseImageId + frameNum);
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 20, 20, 23 }, { -10, -10, height });

        if (vehicle != nullptr && vehicle->num_peeps > 0)
        {
            auto* rider = GetEntity<Guest>(vehicle->peep[0]);
            if (rider != nullptr)
            {
                imageColourFlags = ImageId(0, rider->TshirtColour, rider->TrousersColour);
                imageId = imageColourFlags.WithIndex((baseImageId & 0x7FFFF) + 352 + frameNum);
                PaintAddImageAsChild(session, imageId, { 0, 0, height }, { 20, 20, 23 }, { -10, -10, height });
            }
        }
    }

    session.CurrentlyDrawnEntity = nullptr;
    session.InteractionType = ViewportInteractionItem::Ride;
}

/** rct2: 0x00767C40 */
static void paint_space_rings(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = track_map_3x3[direction][trackSequence];

    int32_t edges = edges_3x3[trackSequence];
    CoordsXY position = session.MapPosition;

    ImageId imageId;

    wooden_a_supports_paint_setup(session, (direction & 1), 0, height, session.TrackColours[SCHEME_MISC]);

    const StationObject* stationObject = ride.GetStationObject();
    track_paint_util_paint_floor(session, edges, session.TrackColours[SCHEME_TRACK], height, floorSpritesCork, stationObject);

    switch (trackSequence)
    {
        case 7:
            if (track_paint_util_has_fence(EDGE_SW, position, trackElement, ride, session.CurrentRotation))
            {
                imageId = session.TrackColours[SCHEME_MISC].WithIndex(SPR_SPACE_RINGS_FENCE_SW);
                PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 1, 28, 7 }, { 29, 0, height + 2 });
            }
            if (track_paint_util_has_fence(EDGE_SE, position, trackElement, ride, session.CurrentRotation))
            {
                imageId = session.TrackColours[SCHEME_MISC].WithIndex(SPR_SPACE_RINGS_FENCE_SE);
                PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 28, 1, 7 }, { 0, 29, height + 2 });
            }
            break;
        default:
            track_paint_util_paint_fences(
                session, edges, position, trackElement, ride, session.TrackColours[SCHEME_MISC], height,
                space_rings_fence_sprites, session.CurrentRotation);
            break;
    }

    switch (trackSequence)
    {
        case 0:
            paint_space_rings_structure(session, ride, direction, 0, height + 3);
            break;
        case 5:
            paint_space_rings_structure(session, ride, direction, 1, height + 3);
            break;
        case 7:
            paint_space_rings_structure(session, ride, direction, 2, height + 3);
            break;
        case 8:
            paint_space_rings_structure(session, ride, direction, 3, height + 3);
            break;
    }

    int32_t cornerSegments = 0;
    switch (trackSequence)
    {
        case 0:
            cornerSegments = 0;
            break;
        case 1:
            cornerSegments = SEGMENT_B8 | SEGMENT_C8 | SEGMENT_B4 | SEGMENT_CC | SEGMENT_BC;
            break;
        case 2:
            cornerSegments = SEGMENT_B4 | SEGMENT_CC | SEGMENT_BC;
            break;
        case 3:
            cornerSegments = SEGMENT_B4 | SEGMENT_CC | SEGMENT_BC | SEGMENT_D4 | SEGMENT_C0;
            break;
        case 4:
            cornerSegments = SEGMENT_B4 | SEGMENT_C8 | SEGMENT_B8;
            break;
        case 5:
            cornerSegments = SEGMENT_BC | SEGMENT_D4 | SEGMENT_C0;
            break;
        case 6:
            cornerSegments = SEGMENT_B4 | SEGMENT_C8 | SEGMENT_B8 | SEGMENT_D0 | SEGMENT_C0;
            break;
        case 7:
            cornerSegments = SEGMENT_B8 | SEGMENT_D0 | SEGMENT_C0 | SEGMENT_D4 | SEGMENT_BC;
            break;
        case 8:
            cornerSegments = SEGMENT_B8 | SEGMENT_D0 | SEGMENT_C0;
            break;
    }
    paint_util_set_segment_support_height(session, cornerSegments, height + 2, 0x20);
    paint_util_set_segment_support_height(session, SEGMENTS_ALL & ~cornerSegments, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 48, 0x20);
}

/**
 * rct2: 0x0x00767A40
 */
TRACK_PAINT_FUNCTION get_track_paint_function_space_rings(int32_t trackType)
{
    if (trackType != TrackElemType::FlatTrack3x3)
    {
        return nullptr;
    }

    return paint_space_rings;
}
