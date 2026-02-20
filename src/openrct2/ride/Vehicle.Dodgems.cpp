/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Vehicle.h"

#include "../GameState.h"
#include "../core/Speed.hpp"
#include "../entity/EntityList.h"
#include "../entity/EntityRegistry.h"
#include "../scenario/Scenario.h"
#include "Ride.h"
#include "TrackData.h"
#include "VehicleGeometry.h"

using namespace OpenRCT2;
using namespace OpenRCT2::TrackMetaData;
using namespace OpenRCT2::RideVehicle;

// External globals from Vehicle.cpp
extern uint32_t _vehicleMotionTrackFlags;
extern int32_t _vehicleVelocityF64E08;
extern int32_t _vehicleVelocityF64E0C;
extern int32_t _vehicleUnkF64E10;
extern CoordsXYZ _vehicleCurPosition;

/** rct2: 0x009A37C4 */
static constexpr CoordsXY kSurroundingTiles[] = {
    { 0, 0 },
    { 0, +kCoordsXYStep },
    { +kCoordsXYStep, 0 },
    { 0, -kCoordsXYStep },
    { 0, -kCoordsXYStep },
    { -kCoordsXYStep, 0 },
    { -kCoordsXYStep, 0 },
    { 0, +kCoordsXYStep },
    { 0, +kCoordsXYStep },
};

/**
 *
 *  rct2: 0x006D91BF
 */
void Vehicle::UpdateDodgemsMode()
{
    auto curRide = GetRide();
    if (curRide == nullptr)
        return;

    const auto* rideEntry = GetRideEntry();
    if (rideEntry == nullptr)
    {
        return;
    }
    const auto& carEntry = rideEntry->Cars[vehicle_type];

    // Mark the dodgem as in use.
    if (carEntry.flags.has(CarEntryFlag::hasDodgemInUseLights) && animation_frame != 1)
    {
        animation_frame = 1;
        Invalidate();
    }

    UpdateMotionDodgems();

    // Update the length of time vehicle has been in dodgems mode
    if (sub_state++ == 0xFF)
    {
        TimeActive++;
    }

    if (curRide->lifecycleFlags & RIDE_LIFECYCLE_PASS_STATION_NO_STOPPING)
        return;

    // Mark the dodgem as not in use.
    animation_frame = 0;
    Invalidate();
    velocity = 0;
    acceleration = 0;
    SetState(Status::unloadingPassengers);
}

/**
 *
 *  rct2: 0x006DA44E
 */
int32_t Vehicle::UpdateMotionDodgems()
{
    _vehicleMotionTrackFlags = 0;

    auto curRide = GetRide();
    if (curRide == nullptr)
        return _vehicleMotionTrackFlags;

    int32_t nextVelocity = velocity + acceleration;
    if (curRide->lifecycleFlags & (RIDE_LIFECYCLE_BREAKDOWN_PENDING | RIDE_LIFECYCLE_BROKEN_DOWN)
        && curRide->breakdownReasonPending == BREAKDOWN_SAFETY_CUT_OUT)
    {
        nextVelocity = 0;
    }
    velocity = nextVelocity;

    _vehicleVelocityF64E08 = nextVelocity;
    _vehicleVelocityF64E0C = (nextVelocity / 1024) * 42;
    _vehicleUnkF64E10 = 1;

    acceleration = 0;
    if (!(curRide->lifecycleFlags & (RIDE_LIFECYCLE_BREAKDOWN_PENDING | RIDE_LIFECYCLE_BROKEN_DOWN))
        || curRide->breakdownReasonPending != BREAKDOWN_SAFETY_CUT_OUT)
    {
        if ((getGameState().currentTicks & 1) && var_34 != 0)
        {
            if (var_34 > 0)
            {
                var_34--;
                Orientation += 2;
            }
            else
            {
                var_34++;
                Orientation -= 2;
            }
            Orientation &= 0x1E;
            Invalidate();
        }
        else if ((ScenarioRand() & 0xFFFF) <= 2849)
        {
            if (var_35 & (1 << 6))
                Orientation -= 2;
            else
                Orientation += 2;
            Orientation &= 0x1E;
            Invalidate();
        }
    }

    std::optional<EntityId> collideSprite;
    if (DodgemsCollisionDirection != 0)
    {
        uint8_t oldCollisionDirection = DodgemsCollisionDirection & 0x1E;
        DodgemsCollisionDirection = 0;

        CoordsXYZ location = { x, y, z };

        location.x += Geometry::getFreeroamVehicleMovementData(oldCollisionDirection).x;
        location.y += Geometry::getFreeroamVehicleMovementData(oldCollisionDirection).y;
        location.x += Geometry::getFreeroamVehicleMovementData(oldCollisionDirection + 1).x;
        location.y += Geometry::getFreeroamVehicleMovementData(oldCollisionDirection + 1).y;

        if (collideSprite = DodgemsCarWouldCollideAt(location); !collideSprite.has_value())
        {
            MoveTo(location);
        }
    }

    remaining_distance += _vehicleVelocityF64E0C;

    if (remaining_distance >= 13962)
    {
        sound2_flags &= ~VEHICLE_SOUND2_FLAGS_LIFT_HILL;
        _vehicleCurPosition.x = x;
        _vehicleCurPosition.y = y;
        _vehicleCurPosition.z = z;

        while (true)
        {
            var_35++;
            uint8_t direction = Orientation;
            direction |= var_35 & 1;

            CoordsXY location = _vehicleCurPosition;
            location.x += Geometry::getFreeroamVehicleMovementData(direction).x;
            location.y += Geometry::getFreeroamVehicleMovementData(direction).y;

            if (collideSprite = DodgemsCarWouldCollideAt(location); collideSprite.has_value())
            {
                break;
            }

            remaining_distance -= Geometry::getFreeroamVehicleMovementData(direction).distance;
            _vehicleCurPosition.x = location.x;
            _vehicleCurPosition.y = location.y;
            if (remaining_distance < 13962)
            {
                break;
            }
            _vehicleUnkF64E10++;
        }

        if (remaining_distance >= 13962)
        {
            int32_t oldVelocity = velocity;
            remaining_distance = 0;
            velocity = 0;
            uint8_t direction = Orientation | 1;

            Vehicle* collideVehicle = getGameState().entities.GetEntity<Vehicle>(collideSprite.value());
            if (collideVehicle != nullptr)
            {
                var_34 = (ScenarioRand() & 1) ? 1 : -1;

                if (oldVelocity >= 2.0_mph)
                {
                    collideVehicle->DodgemsCollisionDirection = direction;
                    DodgemsCollisionDirection = direction ^ (1 << 4);
                }
            }
            else
            {
                var_34 = (ScenarioRand() & 1) ? 6 : -6;

                if (oldVelocity >= 2.0_mph)
                {
                    DodgemsCollisionDirection = direction ^ (1 << 4);
                }
            }
        }

        MoveTo(_vehicleCurPosition);
    }

    int32_t eax = velocity / 2;
    int32_t edx = velocity >> 8;
    edx *= edx;
    if (velocity < 0)
        edx = -edx;
    edx >>= 5;
    eax += edx;
    if (mass != 0)
    {
        eax /= mass;
    }
    const auto* rideEntry = GetRideEntry();
    const auto& carEntry = rideEntry->Cars[vehicle_type];

    if (!carEntry.flags.has(CarEntryFlag::isPowered))
    {
        acceleration = -eax;
        return _vehicleMotionTrackFlags;
    }

    int32_t momentum = (speed * mass) >> 2;
    int32_t _eax = speed << 14;
    if (flags.has(VehicleFlag::poweredCarInReverse))
    {
        _eax = -_eax;
    }
    _eax -= velocity;
    _eax *= powered_acceleration * 2;
    if (momentum != 0)
        _eax /= momentum;

    acceleration = _eax - eax;
    return _vehicleMotionTrackFlags;
}

/**
 *
 *  rct2: 0x006DD365
 */
static bool wouldCollideWithDodgemsTrackEdge(
    const CoordsXY& coords, const CoordsXY& trackLocation, TrackElemType trackType, uint16_t dodgemsCarRadius)
{
    int16_t rideLeft = trackLocation.x + GetDodgemsTrackSize(trackType).left;
    int16_t rideRight = trackLocation.x + GetDodgemsTrackSize(trackType).right;
    int16_t rideTop = trackLocation.y + GetDodgemsTrackSize(trackType).top;
    int16_t rideBottom = trackLocation.y + GetDodgemsTrackSize(trackType).bottom;

    return coords.x - dodgemsCarRadius < rideLeft || coords.y - dodgemsCarRadius < rideTop
        || coords.x + dodgemsCarRadius > rideRight || coords.y + dodgemsCarRadius > rideBottom;
}

std::optional<EntityId> Vehicle::DodgemsCarWouldCollideAt(const CoordsXY& coords) const
{
    auto trackType = GetTrackType();

    if (wouldCollideWithDodgemsTrackEdge(coords, TrackLocation, trackType, (var_44 * 30) >> 9))
    {
        return EntityId::GetNull();
    }

    auto location = coords;

    RideId rideIndex = ride;
    for (auto xy_offset : kSurroundingTiles)
    {
        location += xy_offset;

        for (auto vehicle2 : EntityTileList<Vehicle>(location))
        {
            if (vehicle2 == this)
                continue;
            if (vehicle2->ride != rideIndex)
                continue;

            int32_t distX = abs(coords.x - vehicle2->x);
            if (distX > 32768)
                continue;

            int32_t distY = abs(coords.y - vehicle2->y);
            if (distY > 32768)
                continue;

            int32_t ecx = (var_44 + vehicle2->var_44) / 2;
            ecx *= 30;
            ecx >>= 8;
            if (std::max(distX, distY) < ecx)
            {
                return vehicle2->Id;
            }
        }
    }

    return std::nullopt;
}
