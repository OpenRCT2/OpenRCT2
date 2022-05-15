/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "RaceMode.h"
#include "../Vehicle.h"
#include "../../entity/Peep.h"
#include "../../entity/EntityRegistry.h"
#include "../../entity/Guest.h"

rct_string_id OpenRCT2::RideModes::Race::GetOperationErrorMessage(Ride* ride) const
{
    return STR_CANT_CHANGE_NUMBER_OF_LAPS;
}

bool OpenRCT2::RideModes::Race::FindVehicleToEnter(Guest* guest, Ride* ride, std::vector<uint8_t>& car_array) const
{
    uint8_t chosen_train = RideStation::NO_TRAIN;

    if (ride->lifecycle_flags & RIDE_LIFECYCLE_PASS_STATION_NO_STOPPING)
        return false;

    for (int32_t i = 0; i < ride->num_vehicles; ++i)
    {
        Vehicle* vehicle = GetEntity<Vehicle>(ride->vehicles[i]);
        if (vehicle == nullptr)
            continue;

        if (vehicle->next_free_seat >= vehicle->num_seats)
            continue;

        if (vehicle->status != Vehicle::Status::WaitingForPassengers)
            continue;
        chosen_train = i;
        break;
    }
    if (chosen_train >= OpenRCT2::Limits::MaxTrainsPerRide)
    {
        return false;
    }

    guest->CurrentTrain = chosen_train;

    int32_t i = 0;

    auto vehicle_id = ride->vehicles[chosen_train];
    for (Vehicle* vehicle = GetEntity<Vehicle>(vehicle_id); vehicle != nullptr;
         vehicle = GetEntity<Vehicle>(vehicle->next_vehicle_on_train), ++i)
    {
        uint8_t num_seats = vehicle->num_seats;
        if (vehicle->IsUsedInPairs())
        {
            if (vehicle->next_free_seat & 1)
            {
                car_array.clear();
                car_array.push_back(i);
                return true;
            }
            num_seats &= VEHICLE_SEAT_NUM_MASK;
        }
        if (num_seats == vehicle->next_free_seat)
            continue;
        car_array.push_back(i);
    }

    return !car_array.empty();
}
