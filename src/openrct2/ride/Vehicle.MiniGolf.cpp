/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Vehicle.h"

#include "../Diagnostic.h"
#include "../GameState.h"
#include "../core/Speed.hpp"
#include "../entity/EntityRegistry.h"
#include "../entity/Guest.h"
#include "../paint/vehicle/Vehicle.MiniGolf.h"
#include "../scenario/Scenario.h"
#include "../world/Map.h"
#include "../world/tile_element/TileElement.h"
#include "../world/tile_element/TrackElement.h"
#include "Ride.h"
#include "RideData.h"
#include "Track.h"
#include "TrackData.h"
#include "VehicleGeometry.h"

using namespace OpenRCT2;
using namespace OpenRCT2::RideVehicle;
using namespace OpenRCT2::TrackMetaData;

static PitchAndRoll PitchAndRollStart(bool useInvertedSprites, TileElement* tileElement)
{
    auto trackType = tileElement->AsTrack()->GetTrackType();
    const auto& ted = GetTrackElementDescriptor(trackType);
    return PitchAndRoll{ ted.definition.pitchStart, TrackGetActualBank3(useInvertedSprites, tileElement) };
}

void RideUpdateMeasurementsSpecialElements_MiniGolf(Ride& ride, const TrackElemType trackType)
{
    const auto& ted = GetTrackElementDescriptor(trackType);
    if (ted.flags.has(TrackElementFlag::isGolfHole))
    {
        if (ride.numHoles < Limits::kMaxGolfHoles)
            ride.numHoles++;
    }
}

[[nodiscard]] Vehicle::UpdateMiniGolfSubroutineStatus Vehicle::UpdateTrackMotionMiniGolfFlagsStatus(const Ride& curRide)
{
    if (var_D3 != 0)
    {
        var_D3--;
        remaining_distance -= 0x368A;
        if (remaining_distance < 0)
        {
            remaining_distance = 0;
        }

        if (remaining_distance < 0x368A)
        {
            Loc6DCDE4(curRide);
            return UpdateMiniGolfSubroutineStatus::stop;
        }
        acceleration = Geometry::getAccelerationFromPitch(pitch);
        _vehicleUnkF64E10++;
        return UpdateMiniGolfSubroutineStatus::restart;
    }

    if (mini_golf_flags & MiniGolfFlag::Flag2)
    {
        uint8_t nextFrame = animation_frame + 1;
        if (nextFrame < kMiniGolfPeepAnimationLengths[EnumValue(mini_golf_current_animation)])
        {
            animation_frame = nextFrame;
            remaining_distance -= 0x368A;
            if (remaining_distance < 0)
            {
                remaining_distance = 0;
            }

            if (remaining_distance < 0x368A)
            {
                Loc6DCDE4(curRide);
                return UpdateMiniGolfSubroutineStatus::stop;
            }
            acceleration = Geometry::getAccelerationFromPitch(pitch);
            _vehicleUnkF64E10++;
            return UpdateMiniGolfSubroutineStatus::restart;
        }
        mini_golf_flags &= ~MiniGolfFlag::Flag2;
    }

    if (mini_golf_flags & MiniGolfFlag::Flag0)
    {
        auto vehicleIdx = IsHead() ? next_vehicle_on_ride : prev_vehicle_on_ride;
        Vehicle* vEDI = getGameState().entities.GetEntity<Vehicle>(vehicleIdx);
        if (vEDI == nullptr)
        {
            return UpdateMiniGolfSubroutineStatus::stop;
        }
        if (!(vEDI->mini_golf_flags & MiniGolfFlag::Flag0) || (vEDI->mini_golf_flags & MiniGolfFlag::Flag2))
        {
            remaining_distance -= 0x368A;
            if (remaining_distance < 0)
            {
                remaining_distance = 0;
            }

            if (remaining_distance < 0x368A)
            {
                Loc6DCDE4(curRide);
                return UpdateMiniGolfSubroutineStatus::stop;
            }
            acceleration = Geometry::getAccelerationFromPitch(pitch);
            _vehicleUnkF64E10++;
            return UpdateMiniGolfSubroutineStatus::restart;
        }
        if (vEDI->var_D3 != 0)
        {
            remaining_distance -= 0x368A;
            if (remaining_distance < 0)
            {
                remaining_distance = 0;
            }

            if (remaining_distance < 0x368A)
            {
                Loc6DCDE4(curRide);
                return UpdateMiniGolfSubroutineStatus::stop;
            }
            acceleration = Geometry::getAccelerationFromPitch(pitch);
            _vehicleUnkF64E10++;
            return UpdateMiniGolfSubroutineStatus::restart;
        }
        vEDI->mini_golf_flags &= ~MiniGolfFlag::Flag0;
        mini_golf_flags &= ~MiniGolfFlag::Flag0;
    }

    if (mini_golf_flags & MiniGolfFlag::Flag1)
    {
        auto vehicleIdx = IsHead() ? next_vehicle_on_ride : prev_vehicle_on_ride;
        Vehicle* vEDI = getGameState().entities.GetEntity<Vehicle>(vehicleIdx);
        if (vEDI == nullptr)
        {
            return UpdateMiniGolfSubroutineStatus::stop;
        }
        if (!(vEDI->mini_golf_flags & MiniGolfFlag::Flag1) || (vEDI->mini_golf_flags & MiniGolfFlag::Flag2))
        {
            remaining_distance -= 0x368A;
            if (remaining_distance < 0)
            {
                remaining_distance = 0;
            }

            if (remaining_distance < 0x368A)
            {
                Loc6DCDE4(curRide);
                return UpdateMiniGolfSubroutineStatus::stop;
            }
            acceleration = Geometry::getAccelerationFromPitch(pitch);
            _vehicleUnkF64E10++;
            return UpdateMiniGolfSubroutineStatus::restart;
        }
        if (vEDI->var_D3 != 0)
        {
            remaining_distance -= 0x368A;
            if (remaining_distance < 0)
            {
                remaining_distance = 0;
            }

            if (remaining_distance < 0x368A)
            {
                Loc6DCDE4(curRide);
                return UpdateMiniGolfSubroutineStatus::stop;
            }
            acceleration = Geometry::getAccelerationFromPitch(pitch);
            _vehicleUnkF64E10++;
            return UpdateMiniGolfSubroutineStatus::restart;
        }
        vEDI->mini_golf_flags &= ~MiniGolfFlag::Flag1;
        mini_golf_flags &= ~MiniGolfFlag::Flag1;
    }

    if (mini_golf_flags & MiniGolfFlag::Flag3)
    {
        Vehicle* vEDI = this;

        for (;;)
        {
            vEDI = getGameState().entities.GetEntity<Vehicle>(vEDI->prev_vehicle_on_ride);
            if (vEDI == this || vEDI == nullptr)
            {
                break;
            }
            if (vEDI->IsHead())
                continue;
            if (!(vEDI->mini_golf_flags & MiniGolfFlag::Flag4))
                continue;
            if (vEDI->TrackLocation != TrackLocation)
                continue;
            remaining_distance -= 0x368A;
            if (remaining_distance < 0)
            {
                remaining_distance = 0;
            }

            if (remaining_distance < 0x368A)
            {
                Loc6DCDE4(curRide);
                return UpdateMiniGolfSubroutineStatus::stop;
            }
            acceleration = Geometry::getAccelerationFromPitch(pitch);
            _vehicleUnkF64E10++;
            return UpdateMiniGolfSubroutineStatus::restart;
        }

        mini_golf_flags |= MiniGolfFlag::Flag4;
        mini_golf_flags &= ~MiniGolfFlag::Flag3;
    }

    return UpdateMiniGolfSubroutineStatus::carryOn;
}

[[nodiscard]] Vehicle::UpdateMiniGolfSubroutineStatus Vehicle::Loc6DC462(const Ride& curRide)
{
    while (true)
    {
        UpdateMiniGolfSubroutineStatus flagsStatus = UpdateMiniGolfSubroutineStatus::restart;
        while (flagsStatus == UpdateMiniGolfSubroutineStatus::restart)
        {
            flagsStatus = UpdateTrackMotionMiniGolfFlagsStatus(curRide);
        }
        if (flagsStatus == UpdateMiniGolfSubroutineStatus::stop)
            return UpdateMiniGolfSubroutineStatus::stop;

        uint16_t trackTotalProgress = GetTrackProgress();
        if (track_progress + 1 >= trackTotalProgress)
        {
            CoordsXYZ trackPos;
            Direction direction;
            auto* tileElement = MapGetTrackElementAtOfTypeSeq(TrackLocation, GetTrackType(), 0);
            CoordsXYE output;
            int32_t outZ{};
            int32_t outDirection{};
            CoordsXYE input = { TrackLocation, tileElement };
            if (!TrackBlockGetNext(&input, &output, &outZ, &outDirection))
            {
                _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_5;
                _vehicleVelocityF64E0C -= remaining_distance + 1;
                remaining_distance = -1;
                acceleration += Geometry::getAccelerationFromPitch(pitch);
                _vehicleUnkF64E10++;
                return UpdateMiniGolfSubroutineStatus::carryOn;
            }
            tileElement = output.element;
            trackPos = { output.x, output.y, outZ };
            direction = outDirection;

            if (PitchAndRollStart(false, tileElement) != TrackPitchAndRollEnd(GetTrackType()))
            {
                _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_5;
                _vehicleVelocityF64E0C -= remaining_distance + 1;
                remaining_distance = -1;
                if (remaining_distance >= 0)
                {
                    Loc6DCDE4(curRide);
                }
                acceleration += Geometry::getAccelerationFromPitch(pitch);
                _vehicleUnkF64E10++;
                return UpdateMiniGolfSubroutineStatus::carryOn;
            }

            TrackLocation = trackPos;

            if (!IsHead())
            {
                Vehicle* prevVehicle = getGameState().entities.GetEntity<Vehicle>(prev_vehicle_on_ride);
                if (prevVehicle != nullptr)
                {
                    TrackSubposition = prevVehicle->TrackSubposition;
                }
                if (TrackSubposition != VehicleTrackSubposition::MiniGolfStart9)
                {
                    TrackSubposition = VehicleTrackSubposition{ static_cast<uint8_t>(
                        static_cast<uint8_t>(TrackSubposition) - 1u) };
                }
            }

            flags.unset(VehicleFlag::onLiftHill);
            SetTrackType(tileElement->AsTrack()->GetTrackType());
            SetTrackDirection(direction);
            brake_speed = tileElement->AsTrack()->GetBrakeBoosterSpeed();
            track_progress = 0;
        }
        else
        {
            track_progress += 1;
        }

        if (!IsHead())
        {
            animation_frame++;
            if (animation_frame >= 6)
            {
                animation_frame = 0;
            }
        }
        const VehicleInfo* moveInfo;
        for (;;)
        {
            moveInfo = GetMoveInfo();
            if (moveInfo->x != kLocationNull)
            {
                break;
            }
            switch (MiniGolfState(moveInfo->y))
            {
                case MiniGolfState::Unk0: // Loc6DC7B4
                    if (!IsHead())
                    {
                        mini_golf_flags |= MiniGolfFlag::Flag3;
                    }
                    else
                    {
                        uint16_t rand16 = ScenarioRand() & 0xFFFF;
                        VehicleTrackSubposition nextTrackSubposition = VehicleTrackSubposition::MiniGolfBallPathC14;
                        if (rand16 <= 0xA000)
                        {
                            nextTrackSubposition = VehicleTrackSubposition::MiniGolfBallPathB12;
                            if (rand16 <= 0x900)
                            {
                                nextTrackSubposition = VehicleTrackSubposition::MiniGolfBallPathA10;
                            }
                        }
                        TrackSubposition = nextTrackSubposition;
                    }
                    track_progress++;
                    break;
                case MiniGolfState::Unk1: // Loc6DC7ED
                    LOG_ERROR("Unused move info...");
                    assert(false);
                    var_D3 = static_cast<uint8_t>(moveInfo->z);
                    track_progress++;
                    break;
                case MiniGolfState::Unk2: // Loc6DC800
                    mini_golf_flags |= MiniGolfFlag::Flag0;
                    track_progress++;
                    break;
                case MiniGolfState::Unk3: // Loc6DC810
                    mini_golf_flags |= MiniGolfFlag::Flag1;
                    track_progress++;
                    break;
                case MiniGolfState::Unk4: // Loc6DC820
                {
                    auto animation = MiniGolfAnimation(moveInfo->z);
                    // When the ride is closed occasionally the peep is removed
                    // but the vehicle is still on the track. This will prevent
                    // it from crashing in that situation.
                    auto* curPeep = getGameState().entities.TryGetEntity<Guest>(peep[0]);
                    if (curPeep != nullptr)
                    {
                        if (animation == MiniGolfAnimation::SwingLeft)
                        {
                            if (curPeep->PeepId & 7)
                            {
                                animation = MiniGolfAnimation::Swing;
                            }
                        }
                        if (animation == MiniGolfAnimation::PuttLeft)
                        {
                            if (curPeep->PeepId & 7)
                            {
                                animation = MiniGolfAnimation::Putt;
                            }
                        }
                    }
                    mini_golf_current_animation = animation;
                    animation_frame = 0;
                    track_progress++;
                    break;
                }
                case MiniGolfState::Unk5: // Loc6DC87A
                    mini_golf_flags |= MiniGolfFlag::Flag2;
                    track_progress++;
                    break;
                case MiniGolfState::Unk6: // Loc6DC88A
                    mini_golf_flags &= ~MiniGolfFlag::Flag4;
                    mini_golf_flags |= MiniGolfFlag::Flag5;
                    track_progress++;
                    break;
                default:
                    LOG_ERROR("Invalid move info...");
                    assert(false);
                    break;
            }
        }

        // Loc6DC8A1
        CoordsXYZ trackPos = { TrackLocation.x + moveInfo->x, TrackLocation.y + moveInfo->y,
                               TrackLocation.z + moveInfo->z + GetRideTypeDescriptor(curRide.type).Heights.VehicleZOffset };

        remaining_distance -= 0x368A;
        if (remaining_distance < 0)
        {
            remaining_distance = 0;
        }

        _vehicleCurPosition = trackPos;
        Orientation = moveInfo->yaw;
        roll = moveInfo->roll;
        pitch = moveInfo->pitch;

        if (this == _vehicleFrontVehicle)
        {
            if (_vehicleVelocityF64E08 >= 0)
            {
                auto otherVehicleIndex = prev_vehicle_on_ride;
                UpdateMotionCollisionDetection(trackPos, &otherVehicleIndex);
            }
        }
        if (remaining_distance < 0x368A)
        {
            Loc6DCDE4(curRide);
            return UpdateMiniGolfSubroutineStatus::stop;
        }
        acceleration = Geometry::getAccelerationFromPitch(pitch);
        _vehicleUnkF64E10++;
    }
}

[[nodiscard]] Vehicle::UpdateMiniGolfSubroutineStatus Vehicle::Loc6DCA9A(const Ride& curRide)
{
    while (true)
    {
        if (track_progress == 0)
        {
            auto tileElement = MapGetTrackElementAtOfTypeSeq(TrackLocation, GetTrackType(), 0);
            TrackBeginEnd trackBeginEnd;
            if (!TrackBlockGetPrevious({ TrackLocation, tileElement }, &trackBeginEnd))
            {
                _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_5;
                _vehicleVelocityF64E0C -= remaining_distance + 1;
                remaining_distance = -1;
                acceleration += Geometry::getAccelerationFromPitch(pitch);
                _vehicleUnkF64E10++;
                continue;
            }
            CoordsXYZ trackPos = { trackBeginEnd.begin_x, trackBeginEnd.begin_y, trackBeginEnd.begin_z };
            auto direction = trackBeginEnd.begin_direction;
            tileElement = trackBeginEnd.begin_element;

            if (PitchAndRollStart(false, tileElement) != TrackPitchAndRollEnd(GetTrackType()))
            {
                _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_5;
                _vehicleVelocityF64E0C -= remaining_distance - 0x368A;
                remaining_distance = 0x368A;
                acceleration = Geometry::getAccelerationFromPitch(pitch);
                _vehicleUnkF64E10++;
                return UpdateMiniGolfSubroutineStatus::restart;
            }

            TrackLocation = trackPos;

            if (flags.has(VehicleFlag::onLiftHill))
            {
                flags.unset(VehicleFlag::onLiftHill);
                if (next_vehicle_on_train.IsNull())
                {
                    if (_vehicleVelocityF64E08 < 0)
                    {
                        _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_8;
                    }
                }
            }

            SetTrackType(tileElement->AsTrack()->GetTrackType());
            SetTrackDirection(direction);
            brake_speed = tileElement->AsTrack()->GetBrakeBoosterSpeed();

            // There are two bytes before the move info list
            track_progress = GetTrackProgress();
        }
        else
        {
            track_progress -= 1;
        }

        auto moveInfo = GetMoveInfo();
        CoordsXYZ trackPos = { TrackLocation.x + moveInfo->x, TrackLocation.y + moveInfo->y,
                               TrackLocation.z + moveInfo->z + GetRideTypeDescriptor(curRide.type).Heights.VehicleZOffset };

        remaining_distance -= 0x368A;
        if (remaining_distance < 0)
        {
            remaining_distance = 0;
        }

        _vehicleCurPosition = trackPos;
        Orientation = moveInfo->yaw;
        roll = moveInfo->roll;
        pitch = moveInfo->pitch;

        if (this == _vehicleFrontVehicle)
        {
            if (_vehicleVelocityF64E08 >= 0)
            {
                auto otherVehicleIndex = EntityId::FromUnderlying(var_44); // Possibly wrong?.
                if (UpdateMotionCollisionDetection(trackPos, &otherVehicleIndex))
                {
                    _vehicleVelocityF64E0C -= remaining_distance - 0x368A;
                    remaining_distance = 0x368A;
                    {
                        Vehicle* vEBP = getGameState().entities.GetEntity<Vehicle>(otherVehicleIndex);
                        if (vEBP == nullptr)
                        {
                            return UpdateMiniGolfSubroutineStatus::stop;
                        }
                        Vehicle* vEDI = gCurrentVehicle;
                        if (abs(vEDI->velocity - vEBP->velocity) > 14.0_mph)
                        {
                            _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_VEHICLE_COLLISION;
                        }
                        vEDI->velocity = vEBP->velocity >> 1;
                        vEBP->velocity = vEDI->velocity >> 1;
                    }
                    _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_2;
                    acceleration = Geometry::getAccelerationFromPitch(pitch);
                    _vehicleUnkF64E10++;
                    return UpdateMiniGolfSubroutineStatus::restart;
                }
            }
        }

        if (remaining_distance >= 0)
        {
            Loc6DCDE4(curRide);
            return UpdateMiniGolfSubroutineStatus::stop;
        }
        acceleration += Geometry::getAccelerationFromPitch(pitch);
        _vehicleUnkF64E10++;
    }
}

/**
 *  rct2: 0x006DC3A7
 *
 *
 */

void Vehicle::UpdateTrackMotionMiniGolfVehicle(const Ride& curRide, const RideObjectEntry& rideEntry, const CarEntry* carEntry)
{
    _vehicleUnkF64E10 = 1;
    acceleration = Geometry::getAccelerationFromPitch(pitch);
    if (!flags.has(VehicleFlag::moveSingleCar))
    {
        remaining_distance = _vehicleVelocityF64E0C + remaining_distance;
    }
    if (remaining_distance >= 0 && remaining_distance < 0x368A)
    {
        Loc6DCE02(curRide);
        return;
    }
    sound2_flags &= ~VEHICLE_SOUND2_FLAGS_LIFT_HILL;
    _vehicleCurPosition.x = x;
    _vehicleCurPosition.y = y;
    _vehicleCurPosition.z = z;
    Invalidate();
    bool skipLoc6DC462 = (remaining_distance < 0);

    while (true)
    {
        if (!skipLoc6DC462)
        {
            auto result1 = Loc6DC462(curRide);
            if (result1 == UpdateMiniGolfSubroutineStatus::stop)
                return;
        }
        skipLoc6DC462 = false;
        auto result2 = Loc6DCA9A(curRide);
        if (result2 == UpdateMiniGolfSubroutineStatus::stop)
            return;
    }
}

void Vehicle::Loc6DCDE4(const Ride& curRide)
{
    MoveTo(_vehicleCurPosition);

    Loc6DCE02(curRide);
}

void Vehicle::Loc6DCE02(const Ride& curRide)
{
    acceleration /= _vehicleUnkF64E10;
    if (TrackSubposition == VehicleTrackSubposition::ChairliftGoingBack)
    {
        return;
    }

    auto trackType = GetTrackType();
    const auto& ted = GetTrackElementDescriptor(trackType);
    if (!ted.sequences[0].flags.has(SequenceFlag::trackOrigin))
    {
        return;
    }
    _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_3;
    if (trackType != TrackElemType::endStation)
    {
        return;
    }
    if (this != gCurrentVehicle)
    {
        return;
    }
    if (_vehicleVelocityF64E08 < 0)
    {
        if (track_progress > 11)
        {
            return;
        }
    }
    if (track_progress <= 8)
    {
        return;
    }

    _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_VEHICLE_AT_STATION;

    for (const auto& station : curRide.getStations())
    {
        if (TrackLocation != station.Start)
        {
            continue;
        }
        if (TrackLocation.z != station.GetBaseZ())
        {
            continue;
        }
        _vehicleStationIndex = curRide.getStationIndex(&station);
    }
}

static constexpr int32_t GetAccelerationDecrease2(const int32_t velocity, const int32_t totalMass)
{
    int32_t accelerationDecrease2 = velocity >> 8;
    accelerationDecrease2 *= accelerationDecrease2;
    if (velocity < 0)
    {
        accelerationDecrease2 = -accelerationDecrease2;
    }
    accelerationDecrease2 >>= 4;
    // OpenRCT2: vehicles from different track types can have  0 mass.
    if (totalMass != 0)
    {
        return accelerationDecrease2 / totalMass;
    }

    return accelerationDecrease2;
}

int32_t Vehicle::UpdateTrackMotionMiniGolfCalculateAcceleration(const CarEntry& carEntry)
{
    int32_t sumAcceleration = 0;
    int32_t numVehicles = 0;
    uint16_t totalMass = 0;

    for (Vehicle* vehicle = this; vehicle != nullptr;
         vehicle = getGameState().entities.GetEntity<Vehicle>(vehicle->next_vehicle_on_train))
    {
        numVehicles++;
        totalMass += vehicle->mass;
        sumAcceleration += vehicle->acceleration;
    }

    int32_t newAcceleration = ((sumAcceleration / numVehicles) * 21) >> 9;
    newAcceleration -= velocity >> 12;
    newAcceleration -= GetAccelerationDecrease2(velocity, totalMass);

    {
        int32_t poweredAcceleration = speed << 14;
        int32_t quarterForce = (speed * totalMass) >> 2;
        if (flags.has(VehicleFlag::poweredCarInReverse))
        {
            poweredAcceleration = -poweredAcceleration;
        }
        poweredAcceleration -= velocity;
        poweredAcceleration *= powered_acceleration << 1;
        if (quarterForce != 0)
            poweredAcceleration /= quarterForce;

        if (abs(velocity) > 1.0_mph)
        {
            newAcceleration = 0;
        }
        newAcceleration += poweredAcceleration;
    }

    return newAcceleration;
}

int32_t Vehicle::UpdateTrackMotionMiniGolf(int32_t* outStation)
{
    auto curRide = GetRide();
    if (curRide == nullptr)
        return 0;

    const auto* rideEntry = GetRideEntry();
    if (rideEntry == nullptr)
        return 0;

    const auto* carEntry = Entry();

    gCurrentVehicle = this;
    _vehicleMotionTrackFlags = 0;
    velocity += acceleration;
    _vehicleVelocityF64E08 = velocity;
    _vehicleVelocityF64E0C = (velocity >> 10) * 42;
    _vehicleFrontVehicle = _vehicleVelocityF64E08 < 0 ? TrainTail() : this;

    for (Vehicle* vehicle = _vehicleFrontVehicle; vehicle != nullptr;)
    {
        vehicle->UpdateTrackMotionMiniGolfVehicle(*curRide, *rideEntry, carEntry);
        if (vehicle->flags.has(VehicleFlag::onLiftHill))
        {
            _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_VEHICLE_ON_LIFT_HILL;
        }
        if (vehicle->flags.has(VehicleFlag::moveSingleCar))
        {
            if (outStation != nullptr)
                *outStation = _vehicleStationIndex.ToUnderlying();
            return _vehicleMotionTrackFlags;
        }
        if (_vehicleVelocityF64E08 >= 0)
        {
            vehicle = getGameState().entities.GetEntity<Vehicle>(vehicle->next_vehicle_on_train);
        }
        else
        {
            if (vehicle == gCurrentVehicle)
            {
                break;
            }
            vehicle = getGameState().entities.GetEntity<Vehicle>(vehicle->prev_vehicle_on_ride);
        }
    }

    acceleration = UpdateTrackMotionMiniGolfCalculateAcceleration(*carEntry);

    if (outStation != nullptr)
        *outStation = _vehicleStationIndex.ToUnderlying();
    return _vehicleMotionTrackFlags;
}
