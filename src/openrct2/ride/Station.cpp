/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../Game.h"
#include "Station.h"
#include "Track.h"
#include "../scenario/Scenario.h"
#include "../world/Sprite.h"

static void ride_update_station_blocksection(Ride * ride, int32_t stationIndex);
static void ride_update_station_bumpercar(Ride * ride, int32_t stationIndex);
static void ride_update_station_normal(Ride * ride, int32_t stationIndex);
static void ride_update_station_race(Ride * ride, int32_t stationIndex);
static void ride_race_init_vehicle_speeds(Ride * ride);
static void ride_invalidate_station_start(Ride * ride, int32_t stationIndex, bool greenLight);

/**
 *
 *  rct2: 0x006ABFFB
 */
void ride_update_station(Ride * ride, int32_t stationIndex)
{
    if (ride->station_starts[stationIndex].xy == RCT_XY8_UNDEFINED)
        return;

    switch (ride->mode)
    {
    case RIDE_MODE_RACE:
        ride_update_station_race(ride, stationIndex);
        break;
    case RIDE_MODE_BUMPERCAR:
        ride_update_station_bumpercar(ride, stationIndex);
        break;
    case RIDE_MODE_CONTINUOUS_CIRCUIT_BLOCK_SECTIONED:
    case RIDE_MODE_POWERED_LAUNCH_BLOCK_SECTIONED:
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
static void ride_update_station_blocksection(Ride * ride, int32_t stationIndex)
{
    rct_tile_element * tileElement = ride_get_station_start_track_element(ride, stationIndex);

    if ((ride->status == RIDE_STATUS_CLOSED && ride->num_riders == 0) || (tileElement != nullptr && tileElement->flags & 0x20))
    {
        ride->station_depart[stationIndex] &= ~STATION_DEPART_FLAG;

        if ((ride->station_depart[stationIndex] & STATION_DEPART_FLAG) || (tileElement != nullptr && tile_element_get_green_light(tileElement)))
            ride_invalidate_station_start(ride, stationIndex, false);
    }
    else
    {
        if (!(ride->station_depart[stationIndex] & STATION_DEPART_FLAG))
        {
            ride->station_depart[stationIndex] |= STATION_DEPART_FLAG;
            ride_invalidate_station_start(ride, stationIndex, true);
        }
        else if (tileElement != nullptr && tile_element_get_green_light(tileElement))
        {
            ride_invalidate_station_start(ride, stationIndex, true);
        }
    }
}

/**
 *
 *  rct2: 0x006AC12B
 */
static void ride_update_station_bumpercar(Ride * ride, int32_t stationIndex)
{
    // Change of station depart flag should really call invalidate_station_start
    // but since dodgems do not have station lights there is no point.
    if (ride->status == RIDE_STATUS_CLOSED ||
        (ride->lifecycle_flags & (RIDE_LIFECYCLE_BROKEN_DOWN | RIDE_LIFECYCLE_CRASHED)))
    {
        ride->station_depart[stationIndex] &= ~STATION_DEPART_FLAG;
        return;
    }

    if (ride->lifecycle_flags & RIDE_LIFECYCLE_PASS_STATION_NO_STOPPING)
    {
        int32_t dx = ride->time_limit * 32;
        int32_t dl = dx & 0xFF;
        int32_t dh = (dx >> 8) & 0xFF;
        for (size_t i = 0; i < ride->num_vehicles; i++)
        {
            rct_vehicle * vehicle = &(get_sprite(ride->vehicles[i])->vehicle);
            if (vehicle->var_CE < dh || (vehicle->var_CE < dh && vehicle->sub_state > dl))
                continue;

            // End match
            ride->lifecycle_flags &= ~RIDE_LIFECYCLE_PASS_STATION_NO_STOPPING;
            ride->station_depart[stationIndex] &= ~STATION_DEPART_FLAG;
            return;
        }

        // Continue match
        ride->station_depart[stationIndex] |= STATION_DEPART_FLAG;
    }
    else
    {
        // Check if all vehicles are ready to go
        for (size_t i = 0; i < ride->num_vehicles; i++)
        {
            rct_vehicle * vehicle = &(get_sprite(ride->vehicles[i])->vehicle);
            if (vehicle->status != VEHICLE_STATUS_WAITING_TO_DEPART)
            {
                ride->station_depart[stationIndex] &= ~STATION_DEPART_FLAG;
                return;
            }
        }

        // Begin the match
        ride->lifecycle_flags |= RIDE_LIFECYCLE_PASS_STATION_NO_STOPPING;
        ride->station_depart[stationIndex] |= STATION_DEPART_FLAG;
        ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_MAIN | RIDE_INVALIDATE_RIDE_LIST;
    }
}

/**
 *
 *  rct2: 0x006AC02C
 */
static void ride_update_station_normal(Ride * ride, int32_t stationIndex)
{
    int32_t time = ride->station_depart[stationIndex] & STATION_DEPART_MASK;
    if ((ride->lifecycle_flags & (RIDE_LIFECYCLE_BROKEN_DOWN | RIDE_LIFECYCLE_CRASHED)) ||
        (ride->status == RIDE_STATUS_CLOSED && ride->num_riders == 0))
    {
        if (time != 0 && time != 127 && !(gCurrentTicks & 7))
            time--;

        ride->station_depart[stationIndex] = time;
        ride_invalidate_station_start(ride, stationIndex, false);
    }
    else
    {
        if (time == 0)
        {
            ride->station_depart[stationIndex] |= STATION_DEPART_FLAG;
            ride_invalidate_station_start(ride, stationIndex, true);
        }
        else
        {
            if (time != 127 && !(gCurrentTicks & 31))
                time--;

            ride->station_depart[stationIndex] = time;
            ride_invalidate_station_start(ride, stationIndex, false);
        }
    }
}

/**
 *
 *  rct2: 0x006AC1DF
 */
static void ride_update_station_race(Ride * ride, int32_t stationIndex)
{
    if (ride->status == RIDE_STATUS_CLOSED ||
        (ride->lifecycle_flags & (RIDE_LIFECYCLE_BROKEN_DOWN | RIDE_LIFECYCLE_CRASHED)))
    {
        if (ride->station_depart[stationIndex] & STATION_DEPART_FLAG)
        {
            ride->station_depart[stationIndex] &= ~STATION_DEPART_FLAG;
            ride_invalidate_station_start(ride, stationIndex, false);
        }
        return;
    }

    if (ride->lifecycle_flags & RIDE_LIFECYCLE_PASS_STATION_NO_STOPPING)
    {
        int32_t numLaps = ride->num_laps;
        for (size_t i = 0; i < ride->num_vehicles; i++)
        {
            rct_vehicle * vehicle = &(get_sprite(ride->vehicles[i])->vehicle);
            if (vehicle->status != VEHICLE_STATUS_WAITING_TO_DEPART && vehicle->num_laps >= numLaps)
            {
                // Found a winner
                if (vehicle->num_peeps != 0)
                {
                    rct_peep * peep = &(get_sprite(vehicle->peep[0])->peep);
                    ride->race_winner = peep->sprite_index;
                    ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_MAIN | RIDE_INVALIDATE_RIDE_LIST;
                }

                // Race is over
                ride->lifecycle_flags &= ~RIDE_LIFECYCLE_PASS_STATION_NO_STOPPING;
                if (ride->station_depart[stationIndex] & STATION_DEPART_FLAG)
                {
                    ride->station_depart[stationIndex] &= ~STATION_DEPART_FLAG;
                    ride_invalidate_station_start(ride, stationIndex, false);
                }
                return;
            }
        }

        // Continue racing
        ride->station_depart[stationIndex] |= STATION_DEPART_FLAG;
    }
    else
    {
        // Check if all vehicles are ready to go
        for (size_t i = 0; i < ride->num_vehicles; i++)
        {
            rct_vehicle * vehicle = &(get_sprite(ride->vehicles[i])->vehicle);
            if (vehicle->status != VEHICLE_STATUS_WAITING_TO_DEPART && vehicle->status != VEHICLE_STATUS_DEPARTING)
            {
                if (ride->station_depart[stationIndex] & STATION_DEPART_FLAG)
                {
                    ride->station_depart[stationIndex] &= ~STATION_DEPART_FLAG;
                    ride_invalidate_station_start(ride, stationIndex, false);
                }
                return;
            }
        }

        // Begin the race
        ride_race_init_vehicle_speeds(ride);
        ride->lifecycle_flags |= RIDE_LIFECYCLE_PASS_STATION_NO_STOPPING;
        if (!(ride->station_depart[stationIndex] & STATION_DEPART_FLAG))
        {
            ride->station_depart[stationIndex] |= STATION_DEPART_FLAG;
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
static void ride_race_init_vehicle_speeds(Ride * ride)
{
    for (size_t i = 0; i < ride->num_vehicles; i++)
    {
        rct_vehicle * vehicle = &get_sprite(ride->vehicles[i])->vehicle;
        vehicle->update_flags &= ~VEHICLE_UPDATE_FLAG_6;

        rct_ride_entry * rideEntry = get_ride_entry(vehicle->ride_subtype);

        vehicle->speed = (scenario_rand() & 16) - 8 + rideEntry->vehicles[vehicle->vehicle_type].powered_max_speed;

        if (vehicle->num_peeps != 0)
        {
            rct_peep * peep = &get_sprite(vehicle->peep[0])->peep;

            switch (peep_get_easteregg_name_id(peep))
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

/**
 *
 *  rct2: 0x006AC2C7
 */
static void ride_invalidate_station_start(Ride * ride, int32_t stationIndex, bool greenLight)
{
    int32_t x = ride->station_starts[stationIndex].x * 32;
    int32_t y = ride->station_starts[stationIndex].y * 32;
    rct_tile_element * tileElement = ride_get_station_start_track_element(ride, stationIndex);

    // If no station track found return
    if (tileElement == nullptr)
        return;

    tile_element_set_green_light(tileElement, greenLight);

    // Invalidate map tile
    map_invalidate_tile_zoom1(x, y, tileElement->base_height * 8, tileElement->clearance_height * 8);
}

rct_tile_element * ride_get_station_start_track_element(Ride * ride, int32_t stationIndex)
{
    int32_t x = ride->station_starts[stationIndex].x;
    int32_t y = ride->station_starts[stationIndex].y;
    int32_t z = ride->station_heights[stationIndex];

    // Find the station track element
    rct_tile_element * tileElement = map_get_first_element_at(x, y);
    do
    {
        if (tileElement->GetType() == TILE_ELEMENT_TYPE_TRACK && z == tileElement->base_height)
            return tileElement;

    }
    while (!(tileElement++)->IsLastForTile());

    return nullptr;
}

rct_tile_element * ride_get_station_exit_element(int32_t x, int32_t y, int32_t z)
{
    // Find the station track element
    rct_tile_element * tileElement = map_get_first_element_at(x, y);
    do
    {
        if (tileElement->GetType() == TILE_ELEMENT_TYPE_ENTRANCE && z == tileElement->base_height)
            return tileElement;
    }
    while (!(tileElement++)->IsLastForTile());

    return nullptr;
}

int8_t ride_get_first_valid_station_exit(Ride * ride)
{
    for (int32_t i = 0; i < MAX_STATIONS; i++)
    {
        if (ride->exits[i].x != COORDS_NULL)
        {
            return i;
        }
    }
    return -1;
}

int8_t ride_get_first_valid_station_start(const Ride * ride)
{
    for (int8_t i = 0; i < MAX_STATIONS; i++)
    {
        if (ride->station_starts[i].xy != RCT_XY8_UNDEFINED)
        {
            return i;
        }
    }
    return -1;
}

int8_t ride_get_first_empty_station_start(const Ride * ride)
{
    for (int8_t i = 0; i < MAX_STATIONS; i++)
    {
        if (ride->station_starts[i].xy == RCT_XY8_UNDEFINED)
        {
            return i;
        }
    }
    return -1;
}

TileCoordsXYZD ride_get_entrance_location(const int32_t rideIndex, const int32_t stationIndex)
{
    const Ride * ride = get_ride(rideIndex);
    return ride->entrances[stationIndex];
}

TileCoordsXYZD ride_get_exit_location(const int32_t rideIndex, const int32_t stationIndex)
{
    const Ride * ride = get_ride(rideIndex);
    return ride->exits[stationIndex];
}

TileCoordsXYZD ride_get_entrance_location(const Ride * ride, const int32_t stationIndex)
{
    return ride->entrances[stationIndex];
}

TileCoordsXYZD ride_get_exit_location(const Ride * ride, const int32_t stationIndex)
{
    return ride->exits[stationIndex];
}

void ride_clear_entrance_location(
        Ride * ride,
        const int32_t stationIndex)
{
    ride->entrances[stationIndex].x = COORDS_NULL;
}

void ride_clear_exit_location(
        Ride * ride,
        const int32_t stationIndex)
{
    ride->exits[stationIndex].x = COORDS_NULL;
}

void ride_set_entrance_location(Ride * ride, const int32_t stationIndex, const TileCoordsXYZD location)
{
    ride->entrances[stationIndex] = location;
}

void ride_set_exit_location(Ride * ride, const int32_t stationIndex, const TileCoordsXYZD location)
{
    ride->exits[stationIndex] = location;
}
