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

void OpenRCT2::RideModes::BackwardRotation::UpdateRideFreeVehicleCheck(Guest* guest) const
{
    auto ride = get_ride(guest->CurrentRide);
    if (ride == nullptr)
        return;

    if (ride->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_NO_VEHICLES))
    {
        if (ride->status != RideStatus::Open || ride->vehicle_change_timeout != 0 || (++guest->RejoinQueueTimeout) == 0)
        {
            peep_update_ride_no_free_vehicle_rejoin_queue(guest, ride);
            return;
        }

        guest->UpdateRideFreeVehicleEnterRide(ride);
        return;
    }

    Vehicle* vehicle = GetEntity<Vehicle>(ride->vehicles[guest->CurrentTrain]);
    if (vehicle == nullptr)
    {
        // TODO: Leave ride on failure goes for all returns on nullptr in this function
        return;
    }
    vehicle = vehicle->GetCar(guest->CurrentCar);
    if (vehicle == nullptr)
        return;

    rct_ride_entry* ride_entry = vehicle->GetRideEntry();
    if (ride_entry == nullptr)
    {
        return;
    }

    if (ride_entry->vehicles[0].flags & VEHICLE_ENTRY_FLAG_MINI_GOLF)
    {
        vehicle->mini_golf_flags &= ~MiniGolfFlag::Flag5;

        for (size_t i = 0; i < ride->num_vehicles; ++i)
        {
            Vehicle* train = GetEntity<Vehicle>(ride->vehicles[i]);
            if (train == nullptr)
                continue;

            Vehicle* second_vehicle = GetEntity<Vehicle>(train->next_vehicle_on_train);
            if (second_vehicle == nullptr)
                continue;

            if (second_vehicle->num_peeps == 0)
                continue;

            if (second_vehicle->mini_golf_flags & MiniGolfFlag::Flag5)
                continue;

            return;
        }
    }

    if (!vehicle->IsUsedInPairs())
    {
        guest->UpdateRideFreeVehicleEnterRide(ride);
        return;
    }

    if (guest->CurrentSeat & 1 || !(vehicle->next_free_seat & 1))
    {
        guest->UpdateRideFreeVehicleEnterRide(ride);
        return;
    }

    Vehicle* currentTrain = GetEntity<Vehicle>(ride->vehicles[guest->CurrentTrain]);
    if (currentTrain == nullptr)
    {
        return;
    }
    if (ride->status == RideStatus::Open && ++guest->RejoinQueueTimeout != 0
        && !currentTrain->HasUpdateFlag(VEHICLE_UPDATE_FLAG_TRAIN_READY_DEPART))
    {
        return;
    }


    if (vehicle->next_free_seat - 1 != guest->CurrentSeat)
        return;

    vehicle->next_free_seat--;
    vehicle->peep[guest->CurrentSeat] = EntityId::GetNull();

    peep_update_ride_no_free_vehicle_rejoin_queue(guest, ride);
}

void OpenRCT2::RideModes::BackwardRotation::UpdateRideEnterVehicle(Guest* guest) const
{
    auto* ride = get_ride(guest->CurrentRide);
    if (ride != nullptr)
    {
        auto* vehicle = GetEntity<Vehicle>(ride->vehicles[guest->CurrentTrain]);
        if (vehicle != nullptr)
        {
            vehicle = vehicle->GetCar(guest->CurrentCar);
            if (vehicle == nullptr)
            {
                return;
            }

            if (guest->CurrentSeat != vehicle->num_peeps)
                return;

            if (vehicle->IsUsedInPairs())
            {
                auto* seatedGuest = GetEntity<Guest>(vehicle->peep[guest->CurrentSeat ^ 1]);
                if (seatedGuest != nullptr)
                {
                    if (seatedGuest->RideSubState != PeepRideSubState::EnterVehicle)
                        return;

                    vehicle->num_peeps++;
                    ride->cur_num_customers++;

                    vehicle->ApplyMass(seatedGuest->Mass);
                    seatedGuest->MoveTo({ LOCATION_NULL, 0, 0 });
                    seatedGuest->SetState(PeepState::OnRide);
                    seatedGuest->GuestTimeOnRide = 0;
                    seatedGuest->RideSubState = PeepRideSubState::OnRide;
                    seatedGuest->OnEnterRide(ride);
                }
            }

            vehicle->num_peeps++;
            ride->cur_num_customers++;

            vehicle->ApplyMass(guest->Mass);
            vehicle->Invalidate();

            guest->MoveTo({ LOCATION_NULL, 0, 0 });

            guest->SetState(PeepState::OnRide);

            guest->GuestTimeOnRide = 0;
            guest->RideSubState = PeepRideSubState::OnRide;
            guest->OnEnterRide(ride);
        }
    }
}
