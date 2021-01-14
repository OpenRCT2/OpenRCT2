/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Station.h"

#include "../Game.h"
#include "../entity/Guest.h"
#include "../scenario/Scenario.h"
#include "../world/Location.hpp"
#include "RideEntry.h"
#include "Track.h"
#include "Vehicle.h"

static void ride_update_station_blocksection(Ride* ride, StationIndex stationIndex);
static void ride_update_station_dodgems(Ride* ride, StationIndex stationIndex);
static void ride_update_station_normal(Ride* ride, StationIndex stationIndex);
static void ride_update_station_race(Ride* ride, StationIndex stationIndex);
static void ride_race_init_vehicle_speeds(Ride* ride);
static void ride_invalidate_station_start(Ride* ride, StationIndex stationIndex, bool greenLight);

/**
 *
 *  rct2: 0x006ABFFB
 */
void ride_update_station(Ride* ride, StationIndex stationIndex)
{
    if (ride->stations[stationIndex].Start.IsNull())
        return;

    switch (ride->mode)
    {
        case RideMode::Race:
            ride_update_station_race(ride, stationIndex);
            break;
        case RideMode::Dodgems:
            ride_update_station_dodgems(ride, stationIndex);
            break;
        case RideMode::ContinuousCircuitBlockSectioned:
        case RideMode::PoweredLaunchBlockSectioned:
            ride_update_station_blocksection(ride, stationIndex);
            break;
        default:
            ride_update_station_normal(ride, stationIndex);
            break;
    }
}

/**
 *
 *  rct2: 0x006AC0A1
 */
static void ride_update_station_blocksection(Ride* ride, StationIndex stationIndex)
{
    TileElement* tileElement = ride_get_station_start_track_element(ride, stationIndex);

    if ((ride->status == RideStatus::Closed && ride->num_riders == 0)
        || (tileElement != nullptr && tileElement->AsTrack()->GetBrakeClosed()))
    {
        ride->stations[stationIndex].Depart &= ~STATION_DEPART_FLAG;

        if ((ride->stations[stationIndex].Depart & STATION_DEPART_FLAG)
            || (tileElement != nullptr && tileElement->AsTrack()->HasGreenLight()))
            ride_invalidate_station_start(ride, stationIndex, false);
    }
    else
    {
        if (!(ride->stations[stationIndex].Depart & STATION_DEPART_FLAG))
        {
            ride->stations[stationIndex].Depart |= STATION_DEPART_FLAG;
            ride_invalidate_station_start(ride, stationIndex, true);
        }
        else if (tileElement != nullptr && tileElement->AsTrack()->HasGreenLight())
        {
            ride_invalidate_station_start(ride, stationIndex, true);
        }
    }
}

/**
 *
 *  rct2: 0x006AC12B
 */
static void ride_update_station_dodgems(Ride* ride, StationIndex stationIndex)
{
    // Change of station depart flag should really call invalidate_station_start
    // but since dodgems do not have station lights there is no point.
    if (ride->status == RideStatus::Closed || (ride->lifecycle_flags & (RIDE_LIFECYCLE_BROKEN_DOWN | RIDE_LIFECYCLE_CRASHED)))
    {
        ride->stations[stationIndex].Depart &= ~STATION_DEPART_FLAG;
        return;
    }

    if (ride->lifecycle_flags & RIDE_LIFECYCLE_PASS_STATION_NO_STOPPING)
    {
        int32_t dx = ride->time_limit * 32;
        int32_t dh = (dx >> 8) & 0xFF;
        for (size_t i = 0; i < ride->num_vehicles; i++)
        {
            Vehicle* vehicle = GetEntity<Vehicle>(ride->vehicles[i]);
            if (vehicle == nullptr)
                continue;

            if (vehicle->var_CE < dh)
                continue;

            // End match
            ride->lifecycle_flags &= ~RIDE_LIFECYCLE_PASS_STATION_NO_STOPPING;
            ride->stations[stationIndex].Depart &= ~STATION_DEPART_FLAG;
            return;
        }

        // Continue match
        ride->stations[stationIndex].Depart |= STATION_DEPART_FLAG;
    }
    else
    {
        // Check if all vehicles are ready to go
        for (size_t i = 0; i < ride->num_vehicles; i++)
        {
            Vehicle* vehicle = GetEntity<Vehicle>(ride->vehicles[i]);
            if (vehicle == nullptr)
                continue;

            if (vehicle->status != Vehicle::Status::WaitingToDepart)
            {
                ride->stations[stationIndex].Depart &= ~STATION_DEPART_FLAG;
                return;
            }
        }

        // Begin the match
        ride->lifecycle_flags |= RIDE_LIFECYCLE_PASS_STATION_NO_STOPPING;
        ride->stations[stationIndex].Depart |= STATION_DEPART_FLAG;
        ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_MAIN | RIDE_INVALIDATE_RIDE_LIST;
    }
}

/**
 *
 *  rct2: 0x006AC02C
 */
static void ride_update_station_normal(Ride* ride, StationIndex stationIndex)
{
    int32_t time = ride->stations[stationIndex].Depart & STATION_DEPART_MASK;
    if ((ride->lifecycle_flags & (RIDE_LIFECYCLE_BROKEN_DOWN | RIDE_LIFECYCLE_CRASHED))
        || (ride->status == RideStatus::Closed && ride->num_riders == 0))
    {
        if (time != 0 && time != 127 && !(gCurrentTicks & 7))
            time--;

        ride->stations[stationIndex].Depart = time;
        ride_invalidate_station_start(ride, stationIndex, false);
    }
    else
    {
        if (time == 0)
        {
            ride->stations[stationIndex].Depart |= STATION_DEPART_FLAG;
            ride_invalidate_station_start(ride, stationIndex, true);
        }
        else
        {
            if (time != 127 && !(gCurrentTicks & 31))
                time--;

            ride->stations[stationIndex].Depart = time;
            ride_invalidate_station_start(ride, stationIndex, false);
        }
    }
}

/**
 *
 *  rct2: 0x006AC1DF
 */
static void ride_update_station_race(Ride* ride, StationIndex stationIndex)
{
    if (ride->status == RideStatus::Closed || (ride->lifecycle_flags & (RIDE_LIFECYCLE_BROKEN_DOWN | RIDE_LIFECYCLE_CRASHED)))
    {
        if (ride->stations[stationIndex].Depart & STATION_DEPART_FLAG)
        {
            ride->stations[stationIndex].Depart &= ~STATION_DEPART_FLAG;
            ride_invalidate_station_start(ride, stationIndex, false);
        }
        return;
    }

    if (ride->lifecycle_flags & RIDE_LIFECYCLE_PASS_STATION_NO_STOPPING)
    {
        int32_t numLaps = ride->num_laps;

        for (size_t i = 0; i < ride->num_vehicles; i++)
        {
            Vehicle* vehicle = GetEntity<Vehicle>(ride->vehicles[i]);
            if (vehicle == nullptr)
                continue;

            if (vehicle->status != Vehicle::Status::WaitingToDepart && vehicle->num_laps >= numLaps)
            {
                // Found a winner
                if (vehicle->num_peeps != 0)
                {
                    auto* peep = GetEntity<Guest>(vehicle->peep[0]);
                    if (peep != nullptr)
                    {
                        ride->race_winner = peep->sprite_index;
                        ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_MAIN | RIDE_INVALIDATE_RIDE_LIST;
                    }
                }

                // Race is over
                ride->lifecycle_flags &= ~RIDE_LIFECYCLE_PASS_STATION_NO_STOPPING;
                if (ride->stations[stationIndex].Depart & STATION_DEPART_FLAG)
                {
                    ride->stations[stationIndex].Depart &= ~STATION_DEPART_FLAG;
                    ride_invalidate_station_start(ride, stationIndex, false);
                }
                return;
            }
        }

        // Continue racing
        ride->stations[stationIndex].Depart |= STATION_DEPART_FLAG;
    }
    else
    {
        // Check if all vehicles are ready to go
        for (size_t i = 0; i < ride->num_vehicles; i++)
        {
            Vehicle* vehicle = GetEntity<Vehicle>(ride->vehicles[i]);
            if (vehicle == nullptr)
                continue;

            if (vehicle->status != Vehicle::Status::WaitingToDepart && vehicle->status != Vehicle::Status::Departing)
            {
                if (ride->stations[stationIndex].Depart & STATION_DEPART_FLAG)
                {
                    ride->stations[stationIndex].Depart &= ~STATION_DEPART_FLAG;
                    ride_invalidate_station_start(ride, stationIndex, false);
                }
                return;
            }
        }

        // Begin the race
        ride_race_init_vehicle_speeds(ride);
        ride->lifecycle_flags |= RIDE_LIFECYCLE_PASS_STATION_NO_STOPPING;
        if (!(ride->stations[stationIndex].Depart & STATION_DEPART_FLAG))
        {
            ride->stations[stationIndex].Depart |= STATION_DEPART_FLAG;
            ride_invalidate_station_start(ride, stationIndex, true);
        }
        ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_MAIN | RIDE_INVALIDATE_RIDE_LIST;
    }
}

/**
 *
 *  rct2: 0x006AC988
 * set the speed of the go kart type vehicle at the start to a random value or alter if peep name is an easter egg
 * @param ride (esi)
 */
static void ride_race_init_vehicle_speeds(Ride* ride)
{
    for (size_t i = 0; i < ride->num_vehicles; i++)
    {
        Vehicle* vehicle = GetEntity<Vehicle>(ride->vehicles[i]);
        if (vehicle == nullptr)
            continue;

        vehicle->ClearUpdateFlag(VEHICLE_UPDATE_FLAG_6);

        rct_ride_entry* rideEntry = vehicle->GetRideEntry();

        vehicle->speed = (scenario_rand() & 15) - 8 + rideEntry->vehicles[vehicle->vehicle_type].powered_max_speed;

        if (vehicle->num_peeps != 0)
        {
            auto* guest = GetEntity<Guest>(vehicle->peep[0]);

            // Easter egg names should only work on guests
            if (guest != nullptr)
            {
                switch (guest->GetEasterEggNameId())
                {
                    case EASTEREGG_PEEP_NAME_MICHAEL_SCHUMACHER:
                        vehicle->speed += 35;
                        break;
                    case EASTEREGG_PEEP_NAME_JACQUES_VILLENEUVE:
                        vehicle->speed += 25;
                        break;
                    case EASTEREGG_PEEP_NAME_DAMON_HILL:
                        vehicle->speed += 55;
                        break;
                    case EASTEREGG_PEEP_NAME_CHRIS_SAWYER:
                        vehicle->speed += 14;
                        break;
                    case EASTEREGG_PEEP_NAME_MR_BEAN:
                        vehicle->speed = 9;
                        break;
                }
            }
        }
    }
}

/**
 *
 *  rct2: 0x006AC2C7
 */
static void ride_invalidate_station_start(Ride* ride, StationIndex stationIndex, bool greenLight)
{
    auto startPos = ride->stations[stationIndex].Start;
    TileElement* tileElement = ride_get_station_start_track_element(ride, stationIndex);

    // If no station track found return
    if (tileElement == nullptr)
        return;

    tileElement->AsTrack()->SetHasGreenLight(greenLight);

    // Invalidate map tile
    map_invalidate_tile_zoom1({ startPos, tileElement->GetBaseZ(), tileElement->GetClearanceZ() });
}

TileElement* ride_get_station_start_track_element(const Ride* ride, StationIndex stationIndex)
{
    auto stationStart = ride->stations[stationIndex].GetStart();

    // Find the station track element
    TileElement* tileElement = map_get_first_element_at(stationStart);
    if (tileElement == nullptr)
        return nullptr;
    do
    {
        if (tileElement->GetType() == TileElementType::Track && stationStart.z == tileElement->GetBaseZ())
            return tileElement;

    } while (!(tileElement++)->IsLastForTile());

    return nullptr;
}

TileElement* ride_get_station_exit_element(const CoordsXYZ& elementPos)
{
    // Find the station track element
    TileElement* tileElement = map_get_first_element_at(elementPos);
    if (tileElement == nullptr)
        return nullptr;
    do
    {
        if (tileElement == nullptr)
            break;
        if (tileElement->GetType() == TileElementType::Entrance && elementPos.z == tileElement->GetBaseZ())
            return tileElement;
    } while (!(tileElement++)->IsLastForTile());

    return nullptr;
}

StationIndex ride_get_first_valid_station_exit(Ride* ride)
{
    for (StationIndex i = 0; i < OpenRCT2::Limits::MaxStationsPerRide; i++)
    {
        if (!ride->stations[i].Exit.IsNull())
        {
            return i;
        }
    }
    return STATION_INDEX_NULL;
}

StationIndex ride_get_first_valid_station_start(const Ride* ride)
{
    for (StationIndex i = 0; i < OpenRCT2::Limits::MaxStationsPerRide; i++)
    {
        if (!ride->stations[i].Start.IsNull())
        {
            return i;
        }
    }
    return STATION_INDEX_NULL;
}

StationIndex ride_get_first_empty_station_start(const Ride* ride)
{
    for (StationIndex i = 0; i < OpenRCT2::Limits::MaxStationsPerRide; i++)
    {
        if (ride->stations[i].Start.IsNull())
        {
            return i;
        }
    }
    return STATION_INDEX_NULL;
}

TileCoordsXYZD ride_get_entrance_location(const Ride* ride, const StationIndex stationIndex)
{
    return ride->stations[stationIndex].Entrance;
}

TileCoordsXYZD ride_get_exit_location(const Ride* ride, const StationIndex stationIndex)
{
    return ride->stations[stationIndex].Exit;
}

void ride_clear_entrance_location(Ride* ride, const StationIndex stationIndex)
{
    ride->stations[stationIndex].Entrance.SetNull();
}

void ride_clear_exit_location(Ride* ride, const StationIndex stationIndex)
{
    ride->stations[stationIndex].Exit.SetNull();
}

void ride_set_entrance_location(Ride* ride, const StationIndex stationIndex, const TileCoordsXYZD& location)
{
    ride->stations[stationIndex].Entrance = location;
}

void ride_set_exit_location(Ride* ride, const StationIndex stationIndex, const TileCoordsXYZD& location)
{
    ride->stations[stationIndex].Exit = location;
}

int32_t RideStation::GetBaseZ() const
{
    return Height * COORDS_Z_STEP;
}

void RideStation::SetBaseZ(int32_t newZ)
{
    Height = newZ / COORDS_Z_STEP;
}

CoordsXYZ RideStation::GetStart() const
{
    return { Start, GetBaseZ() };
}
