/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "meta/Condor.h"

#include "../../common.h"
#include "../../core/DataSerialiser.h"
#include "../../interface/Viewport.h"
#include "../../paint/Paint.h"
#include "../../paint/Supports.h"
#include "../../scenario/Scenario.h"
#include "../Ride.h"
#include "../Track.h"
#include "../TrackPaint.h"
#include "../Vehicle.h"
#include "../VehiclePaint.h"
#include "../RideBoundboxBuilder.h"

#include <algorithm>
#include <cstring>

enum
{
    SPR_ROTO_DROP_TOWER_SEGMENT = 14558,
    SPR_ROTO_DROP_TOWER_SEGMENT_TOP = 14559,
    SPR_ROTO_DROP_TOWER_BASE = 14560,
    SPR_ROTO_DROP_TOWER_BASE_SEGMENT = 14561,
    SPR_ROTO_DROP_TOWER_BASE_90_DEG = 14562,
    SPR_ROTO_DROP_TOWER_BASE_SEGMENT_90_DEG = 14563,
};

/**
 *
 *  rct2: 0x006D5DA9
 */
void VehicleVisualCondor(
    PaintSession& session, int32_t x, int32_t imageDirection, int32_t y, int32_t z, const Vehicle* vehicle,
    const CarEntry* carEntry)
{
    //dont draw here, it doesn't work since the sprites are wider than 64 pixels!
    /*imageDirection = OpenRCT2::Entity::Yaw::YawTo32(imageDirection);

    auto imageFlags = ImageId(0, vehicle->colours.Body, vehicle->colours.Trim);
    if (vehicle->IsGhost())
    {
        imageFlags = ConstructionMarker;
    }

    ImageId bodyIdLeft = imageFlags.WithIndex(carEntry->base_image_id);
    ImageId bodyIdRight = imageFlags.WithIndex(carEntry->base_image_id + 1);
    PaintAddImageAsParent(session, bodyIdLeft, { 0, 0, z + 32 }, { 64, 64, 96 }, { 0, 0, z + 2 });
    PaintAddImageAsParent(session, bodyIdRight, { 0, 0, z + 32 }, { 64, 64, 96 }, { 0, 0, z + 2 });

    /*int32_t baseImage_id = (carEntry->base_image_id + 4) + ((vehicle->animation_frame / 4) & 0x3);
    if (vehicle->restraints_position >= 64)
    {
        baseImage_id += 7;
        baseImage_id += (vehicle->restraints_position / 64);
    }

    // Draw back:
    image_id = imageFlags.WithIndex(baseImage_id);
    PaintAddImageAsParent(session, image_id, { 0, 0, z }, { 2, 2, 41 }, { -11, -11, z + 1 });

    // Draw front:
    image_id = imageFlags.WithIndex(baseImage_id + 4);
    PaintAddImageAsParent(session, image_id, { 0, 0, z }, { 16, 16, 41 }, { -5, -5, z + 1 });

    if (vehicle->num_peeps > 0 && !vehicle->IsGhost())
    {
        uint8_t riding_peep_sprites[64];
        std::fill_n(riding_peep_sprites, sizeof(riding_peep_sprites), 0xFF);
        for (int32_t i = 0; i < vehicle->num_peeps; i++)
        {
            uint8_t cl = (i & 3) * 16;
            cl += (i & 0xFC);
            cl += vehicle->animation_frame / 4;
            cl += (imageDirection / 8) * 16;
            cl &= 0x3F;
            riding_peep_sprites[cl] = vehicle->peep_tshirt_colours[i];
        }

        // Draw riding peep sprites in back to front order:
        for (int32_t j = 0; j <= 48; j++)
        {
            int32_t i = (j % 2) ? (48 - (j / 2)) : (j / 2);
            if (riding_peep_sprites[i] != 0xFF)
            {
                baseImage_id = carEntry->base_image_id + 20 + i;
                if (vehicle->restraints_position >= 64)
                {
                    baseImage_id += 64;
                    baseImage_id += vehicle->restraints_position / 64;
                }
                image_id = ImageId(baseImage_id, riding_peep_sprites[i]);
                PaintAddImageAsChild(session, image_id, { 0, 0, z }, { { -5, -5, z + 1 }, { 16, 16, 41 } });
            }
        }
    }

    assert(carEntry->effect_visual == 1);
    // Although called in original code, effect_visual (splash effects) are not used for many rides and does not make sense so
    // it was taken out*/
}

static void PaintCondorStructure(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, CoordsXYZ bbOffset, CoordsXYZ bbSize)
{
    auto rideEntry = ride.GetRideEntry();
    if (rideEntry == nullptr)
        return;

    if (ride.lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK && !ride.vehicles[0].IsNull())
    {
        session.InteractionType = ViewportInteractionItem::Entity;
        session.CurrentlyDrawnEntity = GetEntity<Vehicle>(ride.vehicles[0]);
    }

    /*auto imageTemplate = ImageId(0, ride.vehicle_colours[0].Body, ride.vehicle_colours[0].Trim);
    auto imageFlags = session.TrackColours[SCHEME_MISC];
    if (imageFlags != TrackGhost)
    {
        imageTemplate = imageFlags;
    }

    auto imageId = imageTemplate.WithIndex(rideEntry->Cars[0].base_image_id + direction);*/
    auto imageTemplate = ImageId(0, ride.vehicle_colours[0].Body, ride.vehicle_colours[0].Trim);
    auto imageFlags = session.TrackColours[SCHEME_MISC];
    if (imageFlags != TrackGhost)
    {
        imageTemplate = imageFlags;
    }
    auto imageId = imageTemplate.WithIndex(rideEntry->Cars[0].base_image_id);

    std::array<CoordsXY, 4> offsets = { CoordsXY{ 0, 128 }, CoordsXY{ 128, 256 }, CoordsXY{ 256, 128 }, CoordsXY{ 128, 0 } };
    std::array<CoordsXY, 4>
            offsets2 = { CoordsXY{ 50, 0 }, CoordsXY{ 0, -50 }, CoordsXY{ -50, 0 }, CoordsXY{ 0, 50 } };

    const int NumVehicleAnimationFrames = 8;
    auto armsImageId = imageTemplate.WithIndex(rideEntry->Cars[0].base_image_id + NumVehicleAnimationFrames);
    /*PaintAddImageAsParent(
        session, imageId,
        { -bbOffset.x + offsets[1].x + offsets2[1].x, -bbOffset.y + offsets[1].y + offsets2[1].y, height + 32 },
        { { offsets[1].x + offsets2[1].x, offsets[1].y + offsets2[1].y, height + 32 }, { 1, 1, 1 } });
    PaintAddImageAsChild(
        session, armsImageId, { -bbOffset.x + 128, -bbOffset.y + 128, height + 32 },
        { { 128, 128, height + 200 }, { 24, 24, 1 } });
    PaintAddImageAsParent(
        session, imageId,
        { -bbOffset.x + offsets[2].x + offsets2[2].x, -bbOffset.y + offsets[2].y + offsets2[2].y, height + 32 },
        { { offsets[2].x + offsets2[2].x, offsets[2].y + offsets2[2].y, height + 32 }, { 1, 1, 1 } });
    PaintAddImageAsChild(
        session, armsImageId, { -bbOffset.x + 128, -bbOffset.y + 128, height + 32 },
        { { 128, 128, height + 200 }, { 24, 24, 1 } });*/

    for (int i = 0; i < 4; i++)
    {
        PaintAddImageAsParent(
            session, imageId,
            { -bbOffset.x + offsets[i].x + offsets2[i].x, -bbOffset.y + offsets[i].y + offsets2[i].y, height + 32 },
            { { offsets[i].x + offsets2[i].x, offsets[i].y + offsets2[i].y, height + 32 }, { 24, 24, 12 } });
        PaintAddImageAsChild(
            session, armsImageId, { -bbOffset.x + 128, -bbOffset.y + 128, height + 32 },
            { { 128, 128, height + 32 }, { 24, 24, 12 } });
    }
    /*PaintAddImageAsParent(
        session, imageId,
        { -bbOffset.x + offsets[2].x + offsets2[2].x, -bbOffset.y + offsets[2].y + offsets2[2].y, height + 32 },
        { { offsets[2].x + offsets2[2].x, offsets[2].y + offsets2[2].y, height + 32 }, { 24, 24, 1 } });
    PaintAddImageAsChild(
        session, imageId,
        { -bbOffset.x + offsets[1].x + offsets2[1].x, -bbOffset.y + offsets[1].y + offsets2[1].y, height + 32 },
        { { offsets[1].x + offsets2[1].x, offsets[1].y + offsets2[1].y, height + 32 }, { 24, 24, 1 } });*/







    /*PaintAddImageAsParent(
        session, imageId, { -bbOffset.x + 0 + 64, -bbOffset.y + 144 + 0, height + 64 },
        { { 64, 144, height + 7 }, { 24, 24, 48 } });*/
    /*PaintAddImageAsParent(
        session, imageId, { -bbOffset.x, -bbOffset.y + 128, height + 48 }, { { 0, 0, height + 7 }, { 24, 24, 48 } });*/
}

/** rct2: 0x00886194 */
static void PaintCondorBase(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = TrackMap9x9[direction][trackSequence];

    int32_t edges = Edges9x9[trackSequence];

    WoodenASupportsPaintSetup(session, (direction & 1), 0, height, session.TrackColours[SCHEME_MISC]);

    const StationObject* stationObject = ride.GetStationObject();

    TrackPaintUtilPaintFloor(session, edges, session.TrackColours[SCHEME_SUPPORTS], height, floorSpritesMetalB, stationObject);

    TrackPaintUtilPaintFences(
        session, edges, session.MapPosition, trackElement, ride, session.TrackColours[SCHEME_TRACK], height, fenceSpritesMetalB,
        session.CurrentRotation);

    if (trackSequence == 40)
    {
        auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex(
            (direction & 1 ? SPR_ROTO_DROP_TOWER_BASE_90_DEG : SPR_ROTO_DROP_TOWER_BASE));
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 2, 2, 27 }, { 8, 8, height + 3 });

        imageId = session.TrackColours[SCHEME_TRACK].WithIndex(
            (direction & 1 ? SPR_ROTO_DROP_TOWER_BASE_SEGMENT_90_DEG : SPR_ROTO_DROP_TOWER_BASE_SEGMENT));
        PaintAddImageAsParent(session, imageId, { 0, 0, height + 32 }, { 2, 2, 30 }, { 8, 8, height + 32 });

        imageId = session.TrackColours[SCHEME_TRACK].WithIndex(
            (direction & 1 ? SPR_ROTO_DROP_TOWER_BASE_SEGMENT_90_DEG : SPR_ROTO_DROP_TOWER_BASE_SEGMENT));
        PaintAddImageAsParent(session, imageId, { 0, 0, height + 64 }, { 2, 2, 30 }, { 8, 8, height + 64 });

        PaintUtilSetVerticalTunnel(session, height + 96);
        PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);

        PaintUtilSetGeneralSupportHeight(session, height + 96, 0x20);

        return;
    }

    const auto& bbBuilder = RideBoundboxBuilder(TowerBase9x9TED.Block, 81);
    auto offsets = bbBuilder.GetOffsets();

    auto sequenceValid = std::find_if(
        offsets.begin(), offsets.end(), [trackSequence](const auto& elem) { return elem.first == trackSequence; });
    if (sequenceValid != offsets.end())
        PaintCondorStructure(
            session, ride, trackSequence, direction, height, trackElement, CoordsXYZ{ sequenceValid->second, 32 },
            CoordsXYZ{ 32, 32, 32 });

    int32_t blockedSegments = 0;
    switch (trackSequence)
    {
        case 1:
            blockedSegments = SEGMENT_B8 | SEGMENT_C8 | SEGMENT_B4 | SEGMENT_CC | SEGMENT_BC;
            break;
        case 2:
            blockedSegments = SEGMENT_B4 | SEGMENT_CC | SEGMENT_BC;
            break;
        case 3:
            blockedSegments = SEGMENT_B4 | SEGMENT_CC | SEGMENT_BC | SEGMENT_D4 | SEGMENT_C0;
            break;
        case 4:
            blockedSegments = SEGMENT_B4 | SEGMENT_C8 | SEGMENT_B8;
            break;
        case 5:
            blockedSegments = SEGMENT_BC | SEGMENT_D4 | SEGMENT_C0;
            break;
        case 6:
            blockedSegments = SEGMENT_B4 | SEGMENT_C8 | SEGMENT_B8 | SEGMENT_D0 | SEGMENT_C0;
            break;
        case 7:
            blockedSegments = SEGMENT_B8 | SEGMENT_D0 | SEGMENT_C0 | SEGMENT_D4 | SEGMENT_BC;
            break;
        case 8:
            blockedSegments = SEGMENT_B8 | SEGMENT_D0 | SEGMENT_C0;
            break;
    }
    PaintUtilSetSegmentSupportHeight(session, blockedSegments, 0xFFFF, 0);
    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL & ~blockedSegments, height + 2, 0x20);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

/** rct2: 0x008861A4 */
static void PaintCondorTowerSection(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackSequence == 1)
    {
        return;
    }

    auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex(SPR_ROTO_DROP_TOWER_SEGMENT);
    PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 2, 2, 30 }, { 8, 8, height });

    const TileElement* nextTileElement = reinterpret_cast<const TileElement*>(&trackElement) + 1;
    if (trackElement.IsLastForTile() || trackElement.GetClearanceZ() != nextTileElement->GetBaseZ())
    {
        imageId = session.TrackColours[SCHEME_TRACK].WithIndex(SPR_ROTO_DROP_TOWER_SEGMENT_TOP);
        PaintAddImageAsChild(session, imageId, { 0, 0, height }, { { 8, 8, height }, { 2, 2, 30 } });
    }

    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);

    PaintUtilSetVerticalTunnel(session, height + 32);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

/**
 * rct2: 0x00886074
 */
TRACK_PAINT_FUNCTION GetTrackPaintFunctionCondor(int32_t trackType)
{
    switch (trackType)
    {
        case TrackElemType::FlatTrack9x9:
            return PaintCondorBase;

        case TrackElemType::TowerSection:
            return PaintCondorTowerSection;
    }

    return nullptr;
}

static constexpr const CoordsXY word_9A3AB4[4] = {
    { 0, 0 },
    { 0, -96 },
    { -96, -96 },
    { -96, 0 },
};

static constexpr const CoordsXY StartLocations[] = { { 80, 0 }, { 0, 80 }, { -80, 0 }, { 0, -80 } };

CondorVehicleData::CondorVehicleData()
    : VehicleIndex(0)
{
}

CondorVehicleData ::~CondorVehicleData()
{
}

VehicleDataType CondorVehicleData::GetType() const
{
    return VehicleDataType::Condor;
}

void CondorVehicleData::Serialise(DataSerialiser& stream)
{
    stream << VehicleIndex;
}

RideDataType CondorRideData::GetType() const
{
    return RideDataType::Condor;
}

CondorRideData::CondorRideData()
    : State(CondorRideState::Waiting)
    , VehiclesZ(0)
    , TowerTop(0)
    , TowerBase(0)
{
}

static uint32_t CondorGetTowerHeight(const Vehicle& vehicle)
{
    TileCoordsXYZ result;

    TileElement* tileElement = MapGetTrackElementAtOfType(vehicle.TrackLocation, vehicle.GetTrackType());
    if (tileElement == nullptr)
        return 0;

    while (!tileElement->IsLastForTile())
    {
        tileElement++;

        if (tileElement->IsGhost())
            continue;

        if (tileElement->GetType() != TileElementType::Track)
            continue;

        const auto* trackElement = tileElement->AsTrack();
        if (trackElement->GetRideIndex() != vehicle.ride)
            continue;

        if (trackElement->GetTrackType() != TrackElemType::TowerSection
            && trackElement->GetTrackType() != TrackElemType::TowerBase)
            continue;

        int32_t tileTop = tileElement->ClearanceHeight;
        if (result.z < tileTop)
            result.z = tileTop;
    }

    return result.ToCoordsXYZ().z;
}

void CondorCreateVehicle(
    Vehicle* vehicle, Ride* ride, int32_t carIndex, const CoordsXYZ& carPosition, TrackElement* trackElement)
{
    // Loc6DDCA4:
    vehicle->TrackSubposition = VehicleTrackSubposition::Default;
    vehicle->TrackLocation = carPosition;
    vehicle->current_station = trackElement->GetStationIndex();

    vehicle->SetTrackDirection(0);
    vehicle->SetTrackType(trackElement->GetTrackType());
    vehicle->track_progress = 0;
    vehicle->SetState(Vehicle::Status::MovingToEndOfStation);
    vehicle->update_flags = 0;

    // place the car in a circle, centered around the tower
    auto centerOffset = CoordsXY{ 144, 144 };
    auto chosenLoc = carPosition + CoordsXYZ{ StartLocations[carIndex % 4] + centerOffset, 0 };
    vehicle->MoveTo(chosenLoc);
    vehicle->sprite_direction = 0;
    vehicle->Pitch = 0;

    vehicle->VehicleData = std::make_unique<CondorVehicleData>();
    auto condorData = static_cast<CondorVehicleData*>(vehicle->VehicleData.get());
    condorData->VehicleIndex = carIndex;

    if (carIndex == 0)
    {
        ride->Data = std::make_unique<CondorRideData>();

        auto condorRideData = static_cast<CondorRideData*>(ride->Data.get());
        condorRideData->State = CondorRideState::Waiting;
        condorRideData->TowerTop = CondorGetTowerHeight(*vehicle) - 64;
        condorRideData->TowerBase = vehicle->z;
        condorRideData->VehiclesZ = vehicle->z;
    }
}

void CondorRideUpdateWating(Ride& ride)
{
    auto condorRideData = static_cast<CondorRideData*>(ride.Data.get());

    // check if all vehicles are departing
    const auto trainId = ride.vehicles[0];
    auto* car = GetEntity<Vehicle>(trainId);

    if (car != nullptr)
    {
        if (car->status == Vehicle::Status::Travelling)
        {
            // car->FinishDeparting();
            condorRideData->State = CondorRideState::Climbing;
        }
    }
}

/*static void CondorRideUpdateClimbing(Ride& ride)
{
    auto condorRideData = static_cast<CondorRideData*>(ride.Data.get());
    if (condorRideData != nullptr)
    {
        int32_t height = condorRideData->VehiclesZ + 1;
        if (height > condorRideData->TowerTop)
        {
            height = condorRideData->TowerTop;
            condorRideData->State = CondorRideState::Falling;
        }
        condorRideData->VehiclesZ = height;
    }
}

static void CondorRideUpdateFalling(Ride& ride)
{
    auto condorRideData = static_cast<CondorRideData*>(ride.Data.get());
    if (condorRideData != nullptr)
    {
        int32_t height = condorRideData->VehiclesZ - 1;
        if (height < condorRideData->TowerBase)
        {
            height = condorRideData->TowerBase;
            condorRideData->State = CondorRideState::Waiting;
        }
        condorRideData->VehiclesZ = height;
    }
}*/

void CondorRideUpdate(Ride& ride)
{
    auto condorRideData = static_cast<CondorRideData*>(ride.Data.get());
    if (condorRideData != nullptr)
    {
        switch (condorRideData->State)
        {
            case CondorRideState::Waiting:
                CondorRideUpdateWating(ride);
                break;
            case CondorRideState::Climbing:
                //CondorRideUpdateClimbing(ride);
                break;
            case CondorRideState::Falling:
                //CondorRideUpdateFalling(ride);
                break;
        }
    }
}

void CondorUpdateDeparting(Vehicle& vehicle)
{
    vehicle.FinishDeparting();
}

void CondorUpdateTravelling(Vehicle& vehicle)
{
    // auto condorData = static_cast<CondorVehicleData*>(vehicle.VehicleData.get());
    auto ride = vehicle.GetRide();
    auto condorRideData = static_cast<CondorRideData*>(ride->Data.get());

    if (condorRideData != nullptr)
    {
        auto* car = &vehicle;
        do
        {
            if (condorRideData->State == CondorRideState::Climbing || condorRideData->State == CondorRideState::Falling)
            {
                auto target = car->GetLocation();
                target.z = condorRideData->VehiclesZ;

                car->MoveTo(target);
                car->TrackLocation.z = target.z;
                car->Invalidate();
            }
            else
            {
                car->SetState(Vehicle::Status::Arriving);
            }
            car = GetEntity<Vehicle>(car->next_vehicle_on_train);
        } while (car != nullptr);
    }
}

void CondorUpdateMotion(Vehicle& vehicle)
{
    vehicle.current_station = StationIndex::FromUnderlying(0);
    vehicle.velocity = 0;
    vehicle.acceleration = 0;
    vehicle.SetState(Vehicle::Status::WaitingForPassengers);
}

void CondorUpdateWaitingForDepart(Vehicle& vehicle)
{
}
