/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "CableLift.h"

#include "../audio/audio.h"
#include "../entity/EntityList.h"
#include "../rct12/RCT12.h"
#include "../util/Util.h"
#include "../world/tile_element/TileElement.h"
#include "../world/tile_element/TrackElement.h"
#include "Ride.h"
#include "RideData.h"
#include "Track.h"
#include "Vehicle.h"
#include "VehicleData.h"

using namespace OpenRCT2;

Vehicle* CableLiftSegmentCreate(
    Ride& ride, int32_t x, int32_t y, int32_t z, int32_t direction, uint16_t var_44, int32_t remaining_distance, bool head)
{
    Vehicle* current = CreateEntity<Vehicle>();
    current->ride = ride.id;
    current->ride_subtype = kObjectEntryIndexNull;
    if (head)
    {
        ride.cable_lift = current->Id;
    }
    current->SubType = head ? Vehicle::Type::Head : Vehicle::Type::Tail;
    current->var_44 = var_44;
    current->remaining_distance = remaining_distance;
    current->SpriteData.Width = 10;
    current->SpriteData.HeightMin = 10;
    current->SpriteData.HeightMax = 10;
    current->mass = 100;
    current->num_seats = 0;
    current->speed = 20;
    current->powered_acceleration = 80;
    current->velocity = 0;
    current->acceleration = 0;
    current->SwingSprite = 0;
    current->SwingPosition = 0;
    current->SwingSpeed = 0;
    current->restraints_position = 0;
    current->spin_sprite = 0;
    current->spin_speed = 0;
    current->sound2_flags = 0;
    current->sound1_id = OpenRCT2::Audio::SoundId::Null;
    current->sound2_id = OpenRCT2::Audio::SoundId::Null;
    current->CollisionDetectionTimer = 0;
    current->animation_frame = 0;
    current->animationState = 0;
    current->scream_sound_id = OpenRCT2::Audio::SoundId::Null;
    current->Pitch = 0;
    current->bank_rotation = 0;
    for (auto& peep : current->peep)
    {
        peep = EntityId::GetNull();
    }
    current->TrackSubposition = VehicleTrackSubposition::Default;
    current->Orientation = direction << 3;

    z = z * kCoordsZStep;
    current->TrackLocation = { x, y, z };
    z += ride.GetRideTypeDescriptor().Heights.VehicleZOffset;

    current->MoveTo({ 16, 16, z });
    current->SetTrackType(TrackElemType::CableLiftHill);
    current->SetTrackDirection(current->Orientation >> 3);
    current->track_progress = 164;
    current->Flags = VehicleFlags::CollisionDisabled;
    current->SetState(Vehicle::Status::MovingToEndOfStation, 0);
    current->num_peeps = 0;
    current->next_free_seat = 0;
    current->BoatLocation.SetNull();
    return current;
}

void Vehicle::CableLiftUpdate()
{
    switch (status)
    {
        case Vehicle::Status::MovingToEndOfStation:
            CableLiftUpdateMovingToEndOfStation();
            break;
        case Vehicle::Status::WaitingForPassengers:
            // Stays in this state until a train puts it into next state
            break;
        case Vehicle::Status::WaitingToDepart:
            CableLiftUpdateWaitingToDepart();
            break;
        case Vehicle::Status::Departing:
            CableLiftUpdateDeparting();
            break;
        case Vehicle::Status::Travelling:
            CableLiftUpdateTravelling();
            break;
        case Vehicle::Status::Arriving:
            CableLiftUpdateArriving();
            break;
        default:
            break;
    }
}

/**
 *
 *  rct2: 0x006DF8A4
 */
void Vehicle::CableLiftUpdateMovingToEndOfStation()
{
    if (velocity >= -439800)
    {
        acceleration = -2932;
    }

    if (velocity < -439800)
    {
        velocity -= velocity / 16;
        acceleration = 0;
    }

    if (!(CableLiftUpdateTrackMotion() & VEHICLE_UPDATE_MOTION_TRACK_FLAG_VEHICLE_AT_STATION))
        return;

    velocity = 0;
    acceleration = 0;
    SetState(Vehicle::Status::WaitingForPassengers, sub_state);
}

/**
 *
 *  rct2: 0x006DF8F1
 */
void Vehicle::CableLiftUpdateWaitingToDepart()
{
    if (velocity >= -58640)
    {
        acceleration = -14660;
    }

    if (velocity < -58640)
    {
        velocity -= velocity / 16;
        acceleration = 0;
    }

    CableLiftUpdateTrackMotion();

    // Next check to see if the second part of the cable lift
    // is at the front of the passenger vehicle to simulate the
    // cable being attached underneath the train.
    Vehicle* passengerVehicle = GetEntity<Vehicle>(cable_lift_target);
    Vehicle* cableLiftSecondPart = GetEntity<Vehicle>(prev_vehicle_on_ride);
    if (passengerVehicle == nullptr || cableLiftSecondPart == nullptr)
    {
        return;
    }

    int16_t distX = abs(passengerVehicle->x - cableLiftSecondPart->x);
    int16_t distY = abs(passengerVehicle->y - cableLiftSecondPart->y);

    if (distX + distY > 2)
        return;

    velocity = 0;
    acceleration = 0;
    SetState(Vehicle::Status::Departing, 0);
}

/**
 *
 *  rct2: 0x006DF97A
 */
void Vehicle::CableLiftUpdateDeparting()
{
    sub_state++;
    if (sub_state < 16)
        return;

    Vehicle* passengerVehicle = GetEntity<Vehicle>(cable_lift_target);
    if (passengerVehicle == nullptr)
    {
        return;
    }
    SetState(Vehicle::Status::Travelling, sub_state);
    passengerVehicle->SetState(Vehicle::Status::TravellingCableLift, passengerVehicle->sub_state);
}

/**
 *
 *  rct2: 0x006DF99C
 */
void Vehicle::CableLiftUpdateTravelling()
{
    Vehicle* passengerVehicle = GetEntity<Vehicle>(cable_lift_target);
    if (passengerVehicle == nullptr)
    {
        return;
    }

    velocity = std::min(passengerVehicle->velocity, 439800);
    acceleration = 0;
    if (passengerVehicle->HasFlag(VehicleFlags::TrainIsBroken))
        return;

    if (!(CableLiftUpdateTrackMotion() & VEHICLE_UPDATE_MOTION_TRACK_FLAG_1))
        return;

    velocity = 0;
    acceleration = 0;
    SetState(Vehicle::Status::Arriving, 0);
}

/**
 *
 *  rct2: 0x006DF9F0
 */
void Vehicle::CableLiftUpdateArriving()
{
    sub_state++;
    if (sub_state >= 64)
        SetState(Vehicle::Status::MovingToEndOfStation, sub_state);
}

bool Vehicle::CableLiftUpdateTrackMotionForwards()
{
    auto curRide = GetRide();
    if (curRide == nullptr)
        return false;

    for (; remaining_distance >= 13962; _vehicleUnkF64E10++)
    {
        auto trackType = GetTrackType();
        if (trackType == TrackElemType::CableLiftHill && track_progress == 160)
        {
            _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_1;
        }

        uint16_t trackProgress = track_progress + 1;

        uint16_t trackTotalProgress = GetTrackProgress();
        if (trackProgress >= trackTotalProgress)
        {
            TileElement* trackElement = MapGetTrackElementAtOfTypeSeq(TrackLocation, trackType, 0);

            CoordsXYE output;
            int32_t outputZ;
            int32_t outputDirection;

            auto input = CoordsXYE{ TrackLocation, trackElement };

            if (!TrackBlockGetNext(&input, &output, &outputZ, &outputDirection))
                return false;

            if (TrackPitchAndRollEnd(trackType) != TrackPitchAndRollStart(output.element->AsTrack()->GetTrackType()))
                return false;

            TrackLocation = { output, outputZ };
            SetTrackDirection(outputDirection);
            SetTrackType(output.element->AsTrack()->GetTrackType());
            trackProgress = 0;
        }

        track_progress = trackProgress;
        const auto moveInfo = GetMoveInfo();
        auto nextVehiclePosition = CoordsXYZ{ moveInfo->x, moveInfo->y, moveInfo->z } + TrackLocation;

        uint8_t remainingDistanceFlags = 0;
        nextVehiclePosition.z += GetRideTypeDescriptor(curRide->type).Heights.VehicleZOffset;
        if (nextVehiclePosition.x != _vehicleCurPosition.x)
            remainingDistanceFlags |= (1 << 0);
        if (nextVehiclePosition.y != _vehicleCurPosition.y)
            remainingDistanceFlags |= (1 << 1);
        if (nextVehiclePosition.z != _vehicleCurPosition.z)
            remainingDistanceFlags |= (1 << 2);

        remaining_distance -= SubpositionTranslationDistances[remainingDistanceFlags];
        _vehicleCurPosition.x = nextVehiclePosition.x;
        _vehicleCurPosition.y = nextVehiclePosition.y;
        _vehicleCurPosition.z = nextVehiclePosition.z;

        Orientation = moveInfo->direction;
        bank_rotation = moveInfo->bank_rotation;
        Pitch = moveInfo->Pitch;

        if (remaining_distance >= 13962)
        {
            acceleration += AccelerationFromPitch[Pitch];
        }
    }
    return true;
}

bool Vehicle::CableLiftUpdateTrackMotionBackwards()
{
    auto curRide = GetRide();
    if (curRide == nullptr)
        return false;

    for (; remaining_distance < 0; _vehicleUnkF64E10++)
    {
        uint16_t trackProgress = track_progress - 1;

        if (static_cast<int16_t>(trackProgress) == -1)
        {
            auto trackType = GetTrackType();
            TileElement* trackElement = MapGetTrackElementAtOfTypeSeq(TrackLocation, trackType, 0);

            auto input = CoordsXYE{ TrackLocation, trackElement };
            TrackBeginEnd output;

            if (!TrackBlockGetPrevious(input, &output))
                return false;

            if (TrackPitchAndRollStart(trackType) != TrackPitchAndRollEnd(output.begin_element->AsTrack()->GetTrackType()))
                return false;

            TrackLocation = { output.begin_x, output.begin_y, output.begin_z };
            SetTrackDirection(output.begin_direction);
            SetTrackType(output.begin_element->AsTrack()->GetTrackType());

            // Doesn't check for diagonal block brakes because there is no diagonal cable lift piece,
            // no way for a cable lift to start from a diagonal brake.
            if (output.begin_element->AsTrack()->GetTrackType() == TrackElemType::EndStation
                || output.begin_element->AsTrack()->GetTrackType() == TrackElemType::BlockBrakes)
            {
                _vehicleMotionTrackFlags = VEHICLE_UPDATE_MOTION_TRACK_FLAG_VEHICLE_AT_STATION;
            }

            uint16_t trackTotalProgress = GetTrackProgress();
            trackProgress = trackTotalProgress - 1;
        }
        track_progress = trackProgress;
        const auto moveInfo = GetMoveInfo();
        auto unk = CoordsXYZ{ moveInfo->x, moveInfo->y, moveInfo->z } + TrackLocation;

        uint8_t remainingDistanceFlags = 0;
        unk.z += GetRideTypeDescriptor(curRide->type).Heights.VehicleZOffset;
        if (unk.x != _vehicleCurPosition.x)
            remainingDistanceFlags |= (1 << 0);
        if (unk.y != _vehicleCurPosition.y)
            remainingDistanceFlags |= (1 << 1);
        if (unk.z != _vehicleCurPosition.z)
            remainingDistanceFlags |= (1 << 2);

        remaining_distance += SubpositionTranslationDistances[remainingDistanceFlags];
        _vehicleCurPosition.x = unk.x;
        _vehicleCurPosition.y = unk.y;
        _vehicleCurPosition.z = unk.z;

        Orientation = moveInfo->direction;
        bank_rotation = moveInfo->bank_rotation;
        Pitch = moveInfo->Pitch;

        if (remaining_distance < 0)
        {
            acceleration += AccelerationFromPitch[Pitch];
        }
    }
    return true;
}

/**
 *
 *  rct2: 0x006DEF56
 */
int32_t Vehicle::CableLiftUpdateTrackMotion()
{
    _vehicleF64E2C = 0;
    gCurrentVehicle = this;
    _vehicleMotionTrackFlags = 0;
    _vehicleStationIndex = StationIndex::GetNull();

    velocity += acceleration;
    _vehicleVelocityF64E08 = velocity;
    _vehicleVelocityF64E0C = (velocity / 1024) * 42;

    Vehicle* frontVehicle = this;
    if (velocity < 0)
    {
        frontVehicle = TrainTail();
    }

    _vehicleFrontVehicle = frontVehicle;

    for (Vehicle* vehicle = frontVehicle; vehicle != nullptr;)
    {
        vehicle->acceleration = AccelerationFromPitch[vehicle->Pitch];
        _vehicleUnkF64E10 = 1;
        vehicle->remaining_distance += _vehicleVelocityF64E0C;

        if (vehicle->remaining_distance < 0 || vehicle->remaining_distance >= 13962)
        {
            _vehicleCurPosition = vehicle->GetLocation();
            vehicle->Invalidate();

            while (true)
            {
                if (vehicle->remaining_distance < 0)
                {
                    if (vehicle->CableLiftUpdateTrackMotionBackwards())
                    {
                        break;
                    }

                    _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_5;
                    _vehicleVelocityF64E0C -= vehicle->remaining_distance - 13962;
                    vehicle->remaining_distance = 13962;
                    vehicle->acceleration += AccelerationFromPitch[vehicle->Pitch];
                    _vehicleUnkF64E10++;
                    continue;
                }

                if (vehicle->CableLiftUpdateTrackMotionForwards())
                {
                    break;
                }

                _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_5;
                _vehicleVelocityF64E0C -= vehicle->remaining_distance + 1;
                vehicle->remaining_distance = -1;
                vehicle->acceleration += AccelerationFromPitch[vehicle->Pitch];
                _vehicleUnkF64E10++;
            }
            vehicle->MoveTo(_vehicleCurPosition);
        }
        vehicle->acceleration /= _vehicleUnkF64E10;
        if (_vehicleVelocityF64E08 >= 0)
        {
            vehicle = GetEntity<Vehicle>(vehicle->next_vehicle_on_train);
        }
        else
        {
            if (vehicle == this)
                break;
            vehicle = GetEntity<Vehicle>(vehicle->prev_vehicle_on_ride);
        }
    }

    uint32_t vehicleCount = 0;
    uint16_t massTotal = 0;
    int32_t accelerationTotal = 0;

    for (Vehicle* vehicle = GetEntity<Vehicle>(Id); vehicle != nullptr;
         vehicle = GetEntity<Vehicle>(vehicle->next_vehicle_on_train))
    {
        vehicleCount++;

        massTotal += vehicle->mass;
        accelerationTotal = AddClamp<int32_t>(accelerationTotal, vehicle->acceleration);
    }

    int32_t newAcceleration = (accelerationTotal / vehicleCount) >> 9;
    newAcceleration -= velocity >> 12;

    int32_t edx = velocity >> 8;
    edx *= edx;
    if (velocity < 0)
    {
        edx = -edx;
    }
    edx >>= 4;
    newAcceleration -= edx / massTotal;

    acceleration = newAcceleration;
    return _vehicleMotionTrackFlags;
}
