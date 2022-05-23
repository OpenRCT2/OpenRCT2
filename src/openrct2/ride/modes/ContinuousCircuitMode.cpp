/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/
#include "ContinuousCircuitMode.h"

#include "../../Game.h"
#include "../../core/Numerics.hpp"
#include "../CableLift.h"
#include "../Ride.h"
#include "../Vehicle.h"

bool OpenRCT2::RideModes::ContinuousCircuit::CanHaveMultipleCircuits(const Ride* ride) const
{
    if (!(ride->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_ALLOW_MULTIPLE_CIRCUITS)))
        return false;

    // Must have no more than one vehicle and one station
    if (ride->num_vehicles > 1 || ride->num_stations > 1)
        return false;

    return true;
}

bool OpenRCT2::RideModes::ContinuousCircuit::CreateCableLift(RideId rideIndex, bool isApplying) const
{
    auto ride = get_ride(rideIndex);
    if (ride == nullptr)
        return false;

    if (ride->num_circuits > 1)
    {
        gGameCommandErrorText = STR_MULTICIRCUIT_NOT_POSSIBLE_WITH_CABLE_LIFT_HILL;
        return false;
    }

    if (count_free_misc_sprite_slots() <= 5)
    {
        gGameCommandErrorText = STR_UNABLE_TO_CREATE_ENOUGH_VEHICLES;
        return false;
    }

    if (!ride_initialise_cable_lift_track(ride, isApplying))
    {
        return false;
    }

    if (!isApplying)
    {
        return true;
    }

    auto cableLiftLoc = ride->CableLiftLoc;
    auto tileElement = map_get_track_element_at(cableLiftLoc);
    int32_t direction = tileElement->GetDirection();

    Vehicle* head = nullptr;
    Vehicle* tail = nullptr;
    uint32_t ebx = 0;
    for (int32_t i = 0; i < 5; i++)
    {
        uint32_t edx = Numerics::ror32(0x15478, 10);
        uint16_t var_44 = edx & 0xFFFF;
        edx = Numerics::rol32(edx, 10) >> 1;
        ebx -= edx;
        int32_t remaining_distance = ebx;
        ebx -= edx;

        Vehicle* current = cable_lift_segment_create(
            *ride, cableLiftLoc.x, cableLiftLoc.y, cableLiftLoc.z / 8, direction, var_44, remaining_distance, i == 0);
        current->next_vehicle_on_train = EntityId::GetNull();
        if (i == 0)
        {
            head = current;
        }
        else
        {
            tail->next_vehicle_on_train = current->sprite_index;
            tail->next_vehicle_on_ride = current->sprite_index;
            current->prev_vehicle_on_ride = tail->sprite_index;
        }
        tail = current;
    }
    head->prev_vehicle_on_ride = tail->sprite_index;
    tail->next_vehicle_on_ride = head->sprite_index;

    ride->lifecycle_flags |= RIDE_LIFECYCLE_CABLE_LIFT;
    head->CableLiftUpdateTrackMotion();
    return true;
}
