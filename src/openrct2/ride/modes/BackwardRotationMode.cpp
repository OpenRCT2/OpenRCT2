/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "BackwardRotationMode.h"

#include "../../entity/EntityRegistry.h"
#include "../../entity/Guest.h"
#include "../../entity/Peep.h"
#include "../../world/TileElementsView.h"
#include "../Vehicle.h"

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

void OpenRCT2::RideModes::BackwardRotation::UpdateRideLeaveVehicle(Guest* guest) const
{
    auto ride = get_ride(guest->CurrentRide);
    if (ride == nullptr)
        return;

    Vehicle* vehicle = GetEntity<Vehicle>(ride->vehicles[guest->CurrentTrain]);
    if (vehicle == nullptr)
        return;

    StationIndex ride_station = vehicle->current_station;
    vehicle = vehicle->GetCar(guest->CurrentCar);
    if (vehicle == nullptr)
    {
        return;
    }

    if (vehicle->num_peeps - 1 != guest->CurrentSeat)
        return;

    guest->ActionSpriteImageOffset++;
    if (guest->ActionSpriteImageOffset & 3)
        return;

    guest->ActionSpriteImageOffset = 0;

    vehicle->num_peeps--;
    vehicle->ApplyMass(-guest->Mass);
    vehicle->Invalidate();

    if (ride_station.ToUnderlying() >= OpenRCT2::Limits::MaxStationsPerRide)
    {
        // HACK #5658: Some parks have hacked rides which end up in this state
        auto bestStationIndex = ride_get_first_valid_station_exit(ride);
        if (bestStationIndex.IsNull())
        {
            bestStationIndex = StationIndex::FromUnderlying(0);
        }
        ride_station = bestStationIndex;
    }
    guest->CurrentRideStation = ride_station;
    rct_ride_entry* rideEntry = vehicle->GetRideEntry();
    if (rideEntry == nullptr)
    {
        return;
    }

    rct_ride_entry_vehicle* vehicle_entry = &rideEntry->vehicles[vehicle->vehicle_type];

    assert(guest->CurrentRideStation.ToUnderlying() < OpenRCT2::Limits::MaxStationsPerRide);
    auto& station = ride->GetStation(guest->CurrentRideStation);

    if (!(vehicle_entry->flags & VEHICLE_ENTRY_FLAG_LOADING_WAYPOINTS))
    {
        TileCoordsXYZD exitLocation = station.Exit;
        CoordsXYZD platformLocation;
        platformLocation.z = station.GetBaseZ();

        platformLocation.direction = direction_reverse(exitLocation.direction);

        if (!ride->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_VEHICLE_IS_INTEGRAL))
        {
            for (; vehicle != nullptr && !vehicle->IsHead(); vehicle = GetEntity<Vehicle>(vehicle->prev_vehicle_on_ride))
            {
                auto trackType = vehicle->GetTrackType();
                if (trackType == TrackElemType::Flat || trackType > TrackElemType::MiddleStation)
                    continue;

                bool foundStation = false;
                for (auto* trackElement : TileElementsView<TrackElement>(vehicle->TrackLocation))
                {
                    if (trackElement->GetBaseZ() != vehicle->TrackLocation.z)
                        continue;

                    if (trackElement->GetStationIndex() != guest->CurrentRideStation)
                        continue;

                    foundStation = true;
                    break;
                }

                if (foundStation)
                    break;
            }

            if (vehicle == nullptr)
            {
                return;
            }
            uint8_t shiftMultiplier = 12;
            uint8_t specialDirection = platformLocation.direction;

            rideEntry = get_ride_entry(ride->subtype);

            if (rideEntry != nullptr)
            {
                vehicle_entry = &rideEntry->vehicles[rideEntry->default_vehicle];

                if (vehicle_entry->flags & VEHICLE_ENTRY_FLAG_GO_KART)
                {
                    shiftMultiplier = 9;
                }

                if (vehicle_entry->flags & (VEHICLE_ENTRY_FLAG_CHAIRLIFT | VEHICLE_ENTRY_FLAG_GO_KART))
                {
                    specialDirection = ((vehicle->sprite_direction + 3) / 8) + 1;
                    specialDirection &= 3;

                    if (vehicle->TrackSubposition == VehicleTrackSubposition::GoKartsRightLane)
                        specialDirection = direction_reverse(specialDirection);
                }
            }

            int16_t xShift = DirectionOffsets[specialDirection].x;
            int16_t yShift = DirectionOffsets[specialDirection].y;

            platformLocation.x = vehicle->x + xShift * shiftMultiplier;
            platformLocation.y = vehicle->y + yShift * shiftMultiplier;

            peep_go_to_ride_exit(
                guest, ride, platformLocation.x, platformLocation.y, platformLocation.z, platformLocation.direction);
            return;
        }

        platformLocation.x = vehicle->x + DirectionOffsets[platformLocation.direction].x * 12;
        platformLocation.y = vehicle->y + DirectionOffsets[platformLocation.direction].y * 12;

        // This can evaluate to false with buggy custom rides.
        if (guest->CurrentSeat < vehicle_entry->peep_loading_positions.size())
        {
            int8_t loadPosition = vehicle_entry->peep_loading_positions[guest->CurrentSeat];

            switch (vehicle->sprite_direction / 8)
            {
                case 0:
                    platformLocation.x -= loadPosition;
                    break;
                case 1:
                    platformLocation.y += loadPosition;
                    break;
                case 2:
                    platformLocation.x += loadPosition;
                    break;
                case 3:
                    platformLocation.y -= loadPosition;
                    break;
            }
        }
        else
        {
            log_verbose(
                "CurrentSeat %d is too large! (Vehicle entry has room for %d.)", guest->CurrentSeat,
                vehicle_entry->peep_loading_positions.size());
        }

        platformLocation.z = station.GetBaseZ();

        peep_go_to_ride_exit(
            guest, ride, platformLocation.x, platformLocation.y, platformLocation.z, platformLocation.direction);
        return;
    }

    auto exitLocation = station.Exit.ToCoordsXYZD();
    Guard::Assert(!exitLocation.IsNull());

    auto waypointLoc = CoordsXYZ{ station.Start.ToTileCentre(),
                                  exitLocation.z + ride->GetRideTypeDescriptor().Heights.PlatformHeight };

    TileElement* trackElement = ride_get_station_start_track_element(ride, guest->CurrentRideStation);

    Direction station_direction = (trackElement == nullptr ? 0 : trackElement->GetDirection());

    vehicle = GetEntity<Vehicle>(ride->vehicles[guest->CurrentTrain]);
    if (vehicle == nullptr)
    {
        return;
    }

    rideEntry = vehicle->GetRideEntry();
    rct_ride_entry_vehicle* vehicleEntry = &rideEntry->vehicles[vehicle->vehicle_type];
    if (vehicleEntry == nullptr)
        return;

    guest->Var37 = ((exitLocation.direction | guest->GetWaypointedSeatLocation(*ride, vehicleEntry, station_direction) * 4) * 4) | 1;

    if (ride->type == RIDE_TYPE_ENTERPRISE)
    {
        waypointLoc.x = vehicle->x;
        waypointLoc.y = vehicle->y;
    }

    Guard::Assert(vehicleEntry->peep_loading_waypoints.size() >= static_cast<size_t>(guest->Var37 / 4));
    CoordsXYZ exitWaypointLoc = waypointLoc;

    exitWaypointLoc.x += vehicleEntry->peep_loading_waypoints[guest->Var37 / 4][2].x;
    exitWaypointLoc.y += vehicleEntry->peep_loading_waypoints[guest->Var37 / 4][2].y;

    if (ride->type == RIDE_TYPE_MOTION_SIMULATOR)
        exitWaypointLoc.z += 15;

    guest->MoveTo(exitWaypointLoc);

    waypointLoc.x += vehicleEntry->peep_loading_waypoints[guest->Var37 / 4][1].x;
    waypointLoc.y += vehicleEntry->peep_loading_waypoints[guest->Var37 / 4][1].y;

    guest->SetDestination(waypointLoc, 2);
    guest->RideSubState = PeepRideSubState::ApproachExitWaypoints;
}
