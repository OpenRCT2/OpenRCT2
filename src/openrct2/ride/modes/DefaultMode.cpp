/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "DefaultMode.h"
#include "../Vehicle.h"
#include "../../entity/Peep.h"
#include "../../entity/EntityRegistry.h"
#include "../../entity/Guest.h"
#include "../Ride.h"

rct_string_id OpenRCT2::RideModes::Default::GetOperationErrorMessage(Ride* ride) const
{
    if (ride->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_NO_VEHICLES))
    {
        return STR_CANT_CHANGE_THIS;
    }
    return STR_CANT_CHANGE_LAUNCH_SPEED;
}

void OpenRCT2::RideModes::Default::PeepChooseSeatFromCar(Peep* peep, Ride* ride, Vehicle* vehicle) const
{
    if (vehicle == nullptr)
    {
        return;
    }
    uint8_t chosen_seat = vehicle->next_free_seat;
    peep->CurrentSeat = chosen_seat;
    vehicle->next_free_seat++;

    vehicle->peep[peep->CurrentSeat] = peep->sprite_index;
    vehicle->peep_tshirt_colours[peep->CurrentSeat] = peep->TshirtColour;
}

bool OpenRCT2::RideModes::Default::FindVehicleToEnter(Guest* guest, Ride* ride, std::vector<uint8_t>& car_array) const
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
        car_array.push_back(i);
    }

    return !car_array.empty();
}

void OpenRCT2::RideModes::Default::UpdateRideFreeVehicleCheck(Guest* guest) const
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

    uint8_t seat = guest->CurrentSeat | 1;
    if (seat < vehicle->next_free_seat)
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

    vehicle->next_free_seat--;
    vehicle->peep[guest->CurrentSeat] = EntityId::GetNull();

    peep_update_ride_no_free_vehicle_rejoin_queue(guest, ride);
}

void OpenRCT2::RideModes::Default::UpdateRideEnterVehicle(Guest* guest) const
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

void OpenRCT2::RideModes::Default::UpdateRideLeaveVehicle(Guest* guest) const
{
}

void OpenRCT2::RideModes::Default::UpdateRideApproachSpiralSlide(Guest* guest) const
{
}

StationIndex OpenRCT2::RideModes::Default::CheckValidStationNumbers(Ride* ride) const
{
    return StationIndex();
}

bool OpenRCT2::RideModes::Default::CreateVehicles(Ride* ride, const CoordsXYE& element, bool isApplying) const
{
    return false;
}

bool OpenRCT2::RideModes::Default::CreateCableLift(RideId rideIndex, bool isApplying) const
{
    return false;
}

void OpenRCT2::RideModes::Default::Test(Ride* ride, RideStatus newStatus, bool isApplying) const
{
}

void OpenRCT2::RideModes::Default::Open(Ride* ride, bool isApplying) const
{
}

bool OpenRCT2::RideModes::Default::IsPoweredLaunched(Ride* ride) const
{
    return false;
}

bool OpenRCT2::RideModes::Default::IsBlockSectioned(Ride* ride) const
{
    return false;
}

void OpenRCT2::RideModes::Default::UpdateMaxVehicles(Ride* ride) const
{
}

uint16_t OpenRCT2::RideModes::Default::ComputeUpkeep(RideRatingUpdateState& state, Ride* ride) const
{
    return uint16_t();
}

void OpenRCT2::RideModes::Default::RideRatingsCalculateLaunchedFreefall(Ride* ride, RideRatingUpdateState& state) const
{
}

void OpenRCT2::RideModes::Default::RideRatingsCalculateSpiralSlide(Ride* ride, RideRatingUpdateState& state) const
{
}

void OpenRCT2::RideModes::Default::RideRatingsCalculateGoKarts(Ride* ride, RideRatingUpdateState& state) const
{
}

void OpenRCT2::RideModes::Default::RideRatingsCalculateMotionSimulator(Ride* ride, RideRatingUpdateState& state) const
{
}

void OpenRCT2::RideModes::Default::RideRatingsCalculate3dCinema(Ride* ride, RideRatingUpdateState& state) const
{
}

void OpenRCT2::RideModes::Default::RideRatingsCalculateTopSpin(Ride* ride, RideRatingUpdateState& state) const
{
}

void OpenRCT2::RideModes::Default::RideRatingsCalculateCompactInvertedCoaster(Ride* ride, RideRatingUpdateState& state) const
{
}

void OpenRCT2::RideModes::Default::RideUpdateStation(Ride* ride, StationIndex stationIndex) const
{
}

void OpenRCT2::RideModes::Default::UpdateMovingToEndOfStation(Vehicle* vehicle) const
{
}

void OpenRCT2::RideModes::Default::TrainReadyToDepart(
    Vehicle* vehicle, uint8_t num_peeps_on_train, uint8_t num_used_seats) const
{
}

void OpenRCT2::RideModes::Default::UpdateWaitingToDepart(Vehicle* vehicle) const
{
}

void OpenRCT2::RideModes::Default::UpdateDeparting(Vehicle* vehicle) const
{
}

void OpenRCT2::RideModes::Default::FinishDeparting(Vehicle* vehicle) const
{
}

void OpenRCT2::RideModes::Default::UpdateTravelling(Vehicle* vehicle) const
{
}

void OpenRCT2::RideModes::Default::UpdateArrivingPassThroughStation(
    Vehicle* vehicle, const Ride& curRide, const rct_ride_entry_vehicle& vehicleEntry, bool stationBrakesWork) const
{
}

void OpenRCT2::RideModes::Default::UpdateArriving(Vehicle* vehicle) const
{
}

void OpenRCT2::RideModes::Default::UpdateUnloadingPassengers(Vehicle* vehicle) const
{
}

void OpenRCT2::RideModes::Default::UpdateFerrisWheelRotating(Vehicle* vehicle) const
{
}

uint8_t OpenRCT2::RideModes::Default::WindowRideModeTweakStep() const
{
    return uint8_t();
}

void OpenRCT2::RideModes::Default::WindowRideOperatingInvalidate(
    Formatter& ft, rct_string_id& format, rct_string_id& caption, rct_string_id& tooltip) const
{
}

uint8_t OpenRCT2::RideModes::Default::WindowRideOperatingPaintOffset() const
{
    return uint8_t();
}
