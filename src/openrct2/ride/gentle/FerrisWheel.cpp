/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../entity/EntityRegistry.h"
#include "../../entity/Guest.h"
#include "../../interface/Viewport.h"
#include "../../paint/Boundbox.h"
#include "../../paint/Paint.h"
#include "../../paint/Supports.h"
#include "../Ride.h"
#include "../RideEntry.h"
#include "../Track.h"
#include "../TrackPaint.h"
#include "../Vehicle.h"

static constexpr const uint8_t edges_1x4_ne_sw[] = {
    EDGE_NW | EDGE_SE,
    EDGE_NW | EDGE_SE | EDGE_NE,
    EDGE_NW | EDGE_SE,
    EDGE_NW | EDGE_SE | EDGE_SW,
};

static constexpr const uint8_t edges_1x4_nw_se[] = {
    EDGE_NE | EDGE_SW,
    EDGE_NE | EDGE_SW | EDGE_NW,
    EDGE_NE | EDGE_SW,
    EDGE_NE | EDGE_SW | EDGE_SE,
};

/** rct2: 0x008A8CA8 */
static constexpr BoundBoxXY FerrisWheelData[] = {
    { { 1, 8 }, { 31, 16 } },
    { { 8, 1 }, { 16, 31 } },
    { { 1, 8 }, { 31, 16 } },
    { { 8, 1 }, { 16, 31 } },
};

static void PaintFerrisWheelRiders(
    paint_session& session, const rct_ride_entry& rideEntry, const Vehicle& vehicle, uint8_t direction, const CoordsXYZ offset,
    const BoundBoxXYZ& bb)
{
    for (int32_t i = 0; i < 32; i += 2)
    {
        auto* peep = GetEntity<Guest>(vehicle.peep[i]);
        if (peep == nullptr || peep->State != PeepState::OnRide)
            continue;

        auto frameNum = (vehicle.Pitch + i * 4) % 128;
        auto imageIndex = rideEntry.Cars[0].base_image_id + 32 + direction * 128 + frameNum;
        auto imageId = ImageId(imageIndex, vehicle.peep_tshirt_colours[i], vehicle.peep_tshirt_colours[i + 1]);
        PaintAddImageAsChild(session, imageId, offset, bb);
    }
}

static void PaintFerrisWheelStructure(
    paint_session& session, const Ride& ride, uint8_t direction, int8_t axisOffset, uint16_t height)
{
    auto rideEntry = ride.GetRideEntry();
    if (rideEntry == nullptr)
        return;

    auto vehicle = GetEntity<Vehicle>(ride.vehicles[0]);
    if (ride.lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK && vehicle != nullptr)
    {
        session.InteractionType = ViewportInteractionItem::Entity;
        session.CurrentlyDrawnEntity = vehicle;
    }

    auto boundBox = FerrisWheelData[direction];
    CoordsXYZ offset((direction & 1) ? 0 : axisOffset, (direction & 1) ? axisOffset : 0, height + 7);
    BoundBoxXYZ bb = { { boundBox.offset, height + 7 }, { boundBox.length, 127 } };

    auto supportsImageTemplate = session.TrackColours[SCHEME_TRACK];
    auto wheelImageTemplate = ImageId(0, ride.vehicle_colours[0].Body, ride.vehicle_colours[0].Trim);
    auto wheelImageFlags = session.TrackColours[SCHEME_MISC];
    if (!wheelImageFlags.HasPrimary())
    {
        wheelImageTemplate = wheelImageFlags;
    }

    auto imageOffset = vehicle != nullptr ? vehicle->Pitch % 8 : 0;
    auto leftSupportImageId = supportsImageTemplate.WithIndex(22150 + (direction & 1) * 2);
    auto wheelImageId = wheelImageTemplate.WithIndex(rideEntry->Cars[0].base_image_id + direction * 8 + imageOffset);
    auto rightSupportImageId = leftSupportImageId.WithIndexOffset(1);

    PaintAddImageAsParent(session, leftSupportImageId, offset, bb);
    PaintAddImageAsChild(session, wheelImageId, offset, bb);
    if (vehicle != nullptr)
    {
        PaintFerrisWheelRiders(session, *rideEntry, *vehicle, direction, offset, bb);
    }
    PaintAddImageAsChild(session, rightSupportImageId, offset, bb);

    session.CurrentlyDrawnEntity = nullptr;
    session.InteractionType = ViewportInteractionItem::Ride;
}

static void PaintFerrisWheel(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    uint8_t relativeTrackSequence = track_map_1x4[direction][trackSequence];

    int32_t edges;
    if (direction & 1)
    {
        edges = edges_1x4_nw_se[relativeTrackSequence];
    }
    else
    {
        edges = edges_1x4_ne_sw[relativeTrackSequence];
    }

    wooden_a_supports_paint_setup(session, direction & 1, 0, height, session.TrackColours[SCHEME_MISC]);

    const StationObject* stationObject = ride.GetStationObject();

    track_paint_util_paint_floor(session, edges, session.TrackColours[SCHEME_TRACK], height, floorSpritesCork, stationObject);

    ImageId imageId;
    uint8_t rotation = session.CurrentRotation;
    auto colourFlags = session.TrackColours[SCHEME_MISC];

    if (edges & EDGE_NW && track_paint_util_has_fence(EDGE_NW, session.MapPosition, trackElement, ride, rotation))
    {
        imageId = colourFlags.WithIndex(SPR_FENCE_ROPE_NW);
        PaintAddImageAsChild(session, imageId, { 0, 0, height }, { 32, 1, 7 }, { 0, 2, height + 2 });
    }
    if (edges & EDGE_NE && track_paint_util_has_fence(EDGE_NE, session.MapPosition, trackElement, ride, rotation))
    {
        imageId = colourFlags.WithIndex(SPR_FENCE_ROPE_NE);
        PaintAddImageAsChild(session, imageId, { 0, 0, height }, { 1, 32, 7 }, { 2, 0, height + 2 });
    }
    if (edges & EDGE_SE && track_paint_util_has_fence(EDGE_SE, session.MapPosition, trackElement, ride, rotation))
    {
        // Bound box is slightly different from track_paint_util_paint_fences
        imageId = colourFlags.WithIndex(SPR_FENCE_ROPE_SE);
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 28, 1, 7 }, { 0, 29, height + 3 });
    }
    if (edges & EDGE_SW && track_paint_util_has_fence(EDGE_SW, session.MapPosition, trackElement, ride, rotation))
    {
        imageId = colourFlags.WithIndex(SPR_FENCE_ROPE_SW);
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 1, 32, 7 }, { 30, 0, height + 2 });
    }

    switch (relativeTrackSequence)
    {
        case 1:
            PaintFerrisWheelStructure(session, ride, direction, 48, height);
            break;
        case 2:
            PaintFerrisWheelStructure(session, ride, direction, 16, height);
            break;
        case 0:
            PaintFerrisWheelStructure(session, ride, direction, -16, height);
            break;
        case 3:
            PaintFerrisWheelStructure(session, ride, direction, -48, height);
            break;
    }

    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 176, 0x20);
}

TRACK_PAINT_FUNCTION get_track_paint_function_ferris_wheel(int32_t trackType)
{
    if (trackType != TrackElemType::FlatTrack1x4C)
    {
        return nullptr;
    }

    return PaintFerrisWheel;
}
