/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "BackwardRotationMode.h"
#include "../Vehicle.h"
#include "../../entity/Peep.h"
#include "../../entity/EntityRegistry.h"
#include "../../entity/Guest.h"

rct_string_id OpenRCT2::RideModes::BackwardRotation::GetOperationErrorMessage(Ride* ride) const
{
    return STR_CANT_CHANGE_NUMBER_OF_ROTATIONS;
}

void OpenRCT2::RideModes::BackwardRotation::PeepChooseSeatFromCar(Peep* peep, Ride* ride, Vehicle* vehicle) const
{
    if (vehicle == nullptr)
    {
        return;
    }
    uint8_t chosen_seat = vehicle->next_free_seat;

    chosen_seat = (((~vehicle->Pitch + 1) >> 3) & 0xF) * 2;
    if (vehicle->next_free_seat & 1)
    {
        chosen_seat++;
    }
    peep->CurrentSeat = chosen_seat;
    vehicle->next_free_seat++;

    vehicle->peep[peep->CurrentSeat] = peep->sprite_index;
    vehicle->peep_tshirt_colours[peep->CurrentSeat] = peep->TshirtColour;
}

bool OpenRCT2::RideModes::BackwardRotation::FindVehicleToEnter(Guest* guest, Ride* ride, std::vector<uint8_t>& car_array) const
{
    uint8_t chosen_train = ride->GetStation(guest->CurrentRideStation).TrainAtStation;
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

        uint8_t position = (((~vehicle->Pitch + 1) >> 3) & 0xF) * 2;
        if (!vehicle->peep[position].IsNull())
            continue;
        car_array.push_back(i);
    }

    return !car_array.empty();
}
