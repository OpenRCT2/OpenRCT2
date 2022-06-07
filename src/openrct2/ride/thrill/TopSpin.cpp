/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../entity/EntityRegistry.h"
#include "../../interface/Viewport.h"
#include "../../localisation/Localisation.h"
#include "../../paint/Paint.h"
#include "../../paint/Supports.h"
#include "../../sprites.h"
#include "../../world/Map.h"
#include "../RideData.h"
#include "../TrackData.h"
#include "../TrackPaint.h"
#include "../Vehicle.h"

static int16_t TopSpinSeatHeightOffset[] = {
    -10, -10, -9, -7, -4, -1, 2,  6,  11, 16, 21, 26, 31, 37, 42, 47, 52, 57, 61, 64, 67, 70, 72, 73,
    73,  73,  72, 70, 67, 64, 61, 57, 52, 47, 42, 37, 31, 26, 21, 16, 11, 6,  2,  -1, -4, -7, -9, -10,
};

/**
 * Can be calculated as Rounddown(34*sin(x)+0.5)
 * where x is in 7.5 deg segments.
 */
static int8_t TopSpinSeatPositionOffset[] = {
    0, 4,  9,  13,  17,  21,  24,  27,  29,  31,  33,  34,  34,  34,  33,  31,  29,  27,  24,  21,  17,  13,  9,  4,
    0, -3, -8, -12, -16, -20, -23, -26, -28, -30, -32, -33, -33, -33, -32, -30, -28, -26, -23, -20, -16, -12, -8, -3,
};

static void PaintTopSpinRiders(
    paint_session& session, const Vehicle& vehicle, ImageIndex seatImageIndex, const CoordsXYZ& seatCoords,
    const CoordsXYZ& bbLength, const CoordsXYZ& bbOffset)
{
    if (session.DPI.zoom_level >= ZoomLevel{ 2 })
        return;

    for (int i = 0; i < 4; i++)
    {
        auto peepIndex = i * 2;
        if (vehicle.num_peeps > peepIndex)
        {
            auto imageIndex = seatImageIndex + ((i + 1) * 76);
            auto imageId = ImageId(
                imageIndex, vehicle.peep_tshirt_colours[peepIndex], vehicle.peep_tshirt_colours[peepIndex + 1]);
            PaintAddImageAsChild(session, imageId, seatCoords, bbLength, bbOffset);
        }
        else
        {
            break;
        }
    }
}

static void PaintTopSpinSeat(
    paint_session& session, const Ride& ride, const rct_ride_entry& rideEntry, const Vehicle* vehicle, Direction direction,
    uint32_t armRotation, uint32_t seatRotation, const CoordsXYZ& offset, const CoordsXYZ& bbLength, const CoordsXYZ& bbOffset)
{
    if (armRotation >= std::size(TopSpinSeatHeightOffset))
        return;

    const auto& vehicleEntry = rideEntry.Cars[0];

    ImageIndex seatImageIndex;
    if (vehicle != nullptr && vehicle->restraints_position >= 64)
    {
        // Open Restraints
        seatImageIndex = vehicleEntry.base_image_id + 64;
        seatImageIndex += (vehicle->restraints_position - 64) >> 6;
        seatImageIndex += direction * 3;
    }
    else
    {
        // Var_20 Rotation of seats
        seatImageIndex = vehicleEntry.base_image_id;
        seatImageIndex += direction * 16;
        seatImageIndex += seatRotation;
    }

    auto seatCoords = offset;
    seatCoords.z += TopSpinSeatHeightOffset[armRotation];
    switch (direction)
    {
        case 0:
            seatCoords.x -= TopSpinSeatPositionOffset[armRotation];
            break;
        case 1:
            seatCoords.y += TopSpinSeatPositionOffset[armRotation];
            break;
        case 2:
            seatCoords.x += TopSpinSeatPositionOffset[armRotation];
            break;
        case 3:
            seatCoords.y -= TopSpinSeatPositionOffset[armRotation];
            break;
    }

    auto imageFlags = session.TrackColours[SCHEME_MISC];
    auto imageTemplate = ImageId(0, ride.vehicle_colours[0].Body, ride.vehicle_colours[0].Trim);
    if (imageFlags != IMAGE_TYPE_REMAP)
    {
        imageTemplate = ImageId::FromUInt32(imageFlags);
    }

    PaintAddImageAsChild(session, imageTemplate.WithIndex(seatImageIndex), seatCoords, bbLength, bbOffset);
    if (vehicle != nullptr)
    {
        PaintTopSpinRiders(session, *vehicle, seatImageIndex, seatCoords, bbLength, bbOffset);
    }
}

static void PaintTopSpinVehicle(
    paint_session& session, int32_t al, int32_t cl, const Ride& ride, uint8_t direction, int32_t height,
    const TrackElement& tileElement)
{
    const auto* rideEntry = get_ride_entry(ride.subtype);
    if (rideEntry == nullptr)
        return;

    const auto& vehicleEntry = rideEntry->Cars[0];

    height += 3;
    uint8_t seatRotation = 0;
    uint8_t armRotation = 0;
    auto* vehicle = GetEntity<Vehicle>(ride.vehicles[0]);
    if (ride.lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK && vehicle != nullptr)
    {
        session.InteractionType = ViewportInteractionItem::Entity;
        session.CurrentlyDrawnEntity = vehicle;

        armRotation = vehicle->Pitch;
        seatRotation = vehicle->bank_rotation;
    }

    int32_t armImageOffset = armRotation;
    if (direction & 2)
    {
        armImageOffset = -armImageOffset;
        if (armImageOffset != 0)
            armImageOffset += 48;
    }

    CoordsXYZ offset = { al, cl, height };
    CoordsXYZ bbLength = { 24, 24, 90 };
    CoordsXYZ bbOffset = { al + 16, cl + 16, height };

    auto imageFlags = session.TrackColours[SCHEME_MISC];
    auto supportImageTemplate = ImageId(0, ride.track_colour[0].main, ride.track_colour[0].supports);
    auto armImageTemplate = ImageId(0, ride.track_colour[0].main, ride.track_colour[0].additional);
    if (imageFlags != IMAGE_TYPE_REMAP)
    {
        supportImageTemplate = ImageId::FromUInt32(imageFlags);
        armImageTemplate = supportImageTemplate;
    }

    // Left back bottom support
    auto imageIndex = vehicleEntry.base_image_id + 572 + ((direction & 1) << 1);
    PaintAddImageAsParent(session, supportImageTemplate.WithIndex(imageIndex), offset, bbLength, bbOffset);

    // Left hand arm
    imageIndex = vehicleEntry.base_image_id + 380 + armImageOffset + ((direction & 1) * 48);
    PaintAddImageAsChild(session, armImageTemplate.WithIndex(imageIndex), offset, bbLength, bbOffset);

    // Seat
    PaintTopSpinSeat(session, ride, *rideEntry, vehicle, direction, armRotation, seatRotation, offset, bbLength, bbOffset);

    // Right hand arm
    imageIndex = vehicleEntry.base_image_id + 476 + armImageOffset + ((direction & 1) * 48);
    PaintAddImageAsChild(session, armImageTemplate.WithIndex(imageIndex), offset, bbLength, bbOffset);

    // Right back bottom support
    imageIndex = vehicleEntry.base_image_id + 573 + ((direction & 1) << 1);
    PaintAddImageAsChild(session, supportImageTemplate.WithIndex(imageIndex), offset, bbLength, bbOffset);

    session.CurrentlyDrawnEntity = nullptr;
    session.InteractionType = ViewportInteractionItem::Ride;
}

static void PaintTopSpin(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = track_map_3x3[direction][trackSequence];

    int32_t edges = edges_3x3[trackSequence];

    wooden_a_supports_paint_setup(session, direction & 1, 0, height, session.TrackColours[SCHEME_MISC]);

    const StationObject* stationObject = ride.GetStationObject();

    track_paint_util_paint_floor(session, edges, session.TrackColours[SCHEME_TRACK], height, floorSpritesCork, stationObject);

    track_paint_util_paint_fences(
        session, edges, session.MapPosition, trackElement, ride, session.TrackColours[SCHEME_MISC], height, fenceSpritesRope,
        session.CurrentRotation);

    switch (trackSequence)
    {
        case 1:
            PaintTopSpinVehicle(session, 32, 32, ride, direction, height, trackElement);
            break;
        case 3:
            PaintTopSpinVehicle(session, 32, -32, ride, direction, height, trackElement);
            break;
        case 5:
            PaintTopSpinVehicle(session, 0, -32, ride, direction, height, trackElement);
            break;
        case 6:
            PaintTopSpinVehicle(session, -32, 32, ride, direction, height, trackElement);
            break;
        case 7:
            PaintTopSpinVehicle(session, -32, -32, ride, direction, height, trackElement);
            break;
        case 8:
            PaintTopSpinVehicle(session, -32, 0, ride, direction, height, trackElement);
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
    paint_util_set_general_support_height(session, height + 112, 0x20);
}

TRACK_PAINT_FUNCTION get_track_paint_function_topspin(int32_t trackType)
{
    if (trackType != TrackElemType::FlatTrack3x3)
    {
        return nullptr;
    }

    return PaintTopSpin;
}
