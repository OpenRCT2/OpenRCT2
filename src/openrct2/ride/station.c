#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#include "../game.h"
#include "../scenario/scenario.h"
#include "../world/sprite.h"
#include "station.h"

static void ride_update_station_blocksection(rct_ride *ride, sint32 stationIndex);
static void ride_update_station_bumpercar(rct_ride *ride, sint32 stationIndex);
static void ride_update_station_normal(rct_ride *ride, sint32 stationIndex);
static void ride_update_station_race(rct_ride *ride, sint32 stationIndex);
static void ride_race_init_vehicle_speeds(rct_ride *ride);
static void ride_invalidate_station_start(rct_ride *ride, sint32 stationIndex, sint32 dl);

/**
 *
 *  rct2: 0x006ABFFB
 */
void ride_update_station(rct_ride *ride, sint32 stationIndex)
{
    if (ride->station_starts[stationIndex] == 0xFFFF)
        return;

    switch (ride->mode) {
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
static void ride_update_station_blocksection(rct_ride *ride, sint32 stationIndex)
{
    rct_map_element *mapElement;

    mapElement = ride_get_station_start_track_element(ride, stationIndex);

    if ((ride->status == RIDE_STATUS_CLOSED && ride->num_riders == 0) || (mapElement != NULL && mapElement->flags & 0x20)) {
        ride->station_depart[stationIndex] &= ~STATION_DEPART_FLAG;

        if ((ride->station_depart[stationIndex] & STATION_DEPART_FLAG) || (mapElement != NULL && (mapElement->properties.track.sequence & 0x80)))
            ride_invalidate_station_start(ride, stationIndex, 0);
    } else {
        if (!(ride->station_depart[stationIndex] & STATION_DEPART_FLAG)) {
            ride->station_depart[stationIndex] |= STATION_DEPART_FLAG;
            ride_invalidate_station_start(ride, stationIndex, 1);
        } else if (mapElement != NULL && mapElement->properties.track.sequence & 0x80) {
            ride_invalidate_station_start(ride, stationIndex, 1);
        }
    }
}

/**
 *
 *  rct2: 0x006AC12B
 */
static void ride_update_station_bumpercar(rct_ride *ride, sint32 stationIndex)
{
    // Change of station depart flag should really call invalidate_station_start
    // but since dodgems do not have station lights there is no point.
    if (
        ride->status == RIDE_STATUS_CLOSED ||
        (ride->lifecycle_flags & (RIDE_LIFECYCLE_BROKEN_DOWN | RIDE_LIFECYCLE_CRASHED))
    ) {
        ride->station_depart[stationIndex] &= ~STATION_DEPART_FLAG;
        return;
    }

    if (ride->lifecycle_flags & RIDE_LIFECYCLE_PASS_STATION_NO_STOPPING) {
        sint32 dx = ride->time_limit * 32;
        sint32 dl = dx & 0xFF;
        sint32 dh = (dx >> 8) & 0xFF;
        for (sint32 i = 0; i < ride->num_vehicles; i++) {
            rct_vehicle *vehicle = &(get_sprite(ride->vehicles[i])->vehicle);
            if (vehicle->var_CE < dh || (vehicle->var_CE < dh && vehicle->sub_state > dl))
                continue;

            // End match
            ride->lifecycle_flags &= ~RIDE_LIFECYCLE_PASS_STATION_NO_STOPPING;
            ride->station_depart[stationIndex] &= ~STATION_DEPART_FLAG;
            return;
        }

        // Continue match
        ride->station_depart[stationIndex] |= STATION_DEPART_FLAG;
    } else {
        // Check if all vehicles are ready to go
        for (sint32 i = 0; i < ride->num_vehicles; i++) {
            rct_vehicle *vehicle = &(get_sprite(ride->vehicles[i])->vehicle);
            if (vehicle->status != VEHICLE_STATUS_WAITING_TO_DEPART) {
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
static void ride_update_station_normal(rct_ride *ride, sint32 stationIndex)
{
    sint32 time;

    time = ride->station_depart[stationIndex] & STATION_DEPART_MASK;
    if (
        (ride->lifecycle_flags & (RIDE_LIFECYCLE_BROKEN_DOWN | RIDE_LIFECYCLE_CRASHED))  ||
        (ride->status == RIDE_STATUS_CLOSED && ride->num_riders == 0)
    ) {
        if (time != 0 && time != 127 && !(gCurrentTicks & 7))
            time--;

        ride->station_depart[stationIndex] = time;
        ride_invalidate_station_start(ride, stationIndex, 0);
    } else {
        if (time == 0) {
            ride->station_depart[stationIndex] |= STATION_DEPART_FLAG;
            ride_invalidate_station_start(ride, stationIndex, 1);
        } else {
            if (time != 127 && !(gCurrentTicks & 31))
                time--;

            ride->station_depart[stationIndex] = time;
            ride_invalidate_station_start(ride, stationIndex, 0);
        }
    }
}

/**
 *
 *  rct2: 0x006AC1DF
 */
static void ride_update_station_race(rct_ride *ride, sint32 stationIndex)
{
    if (
        ride->status == RIDE_STATUS_CLOSED ||
        (ride->lifecycle_flags & (RIDE_LIFECYCLE_BROKEN_DOWN | RIDE_LIFECYCLE_CRASHED))
    ) {
        if (ride->station_depart[stationIndex] & STATION_DEPART_FLAG){
            ride->station_depart[stationIndex] &= ~STATION_DEPART_FLAG;
            ride_invalidate_station_start(ride, stationIndex, 0);
        }
        return;
    }

    if (ride->lifecycle_flags & RIDE_LIFECYCLE_PASS_STATION_NO_STOPPING) {
        sint32 numLaps = ride->num_laps;
        for (sint32 i = 0; i < ride->num_vehicles; i++) {
            rct_vehicle *vehicle = &(get_sprite(ride->vehicles[i])->vehicle);
            if (vehicle->status != VEHICLE_STATUS_WAITING_TO_DEPART && vehicle->num_laps >= numLaps) {
                // Found a winner
                if (vehicle->num_peeps != 0) {
                    rct_peep *peep = &(get_sprite(vehicle->peep[0])->peep);
                    ride->race_winner = peep->sprite_index;
                    ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_MAIN | RIDE_INVALIDATE_RIDE_LIST;
                }

                // Race is over
                ride->lifecycle_flags &= ~RIDE_LIFECYCLE_PASS_STATION_NO_STOPPING;
                if (ride->station_depart[stationIndex] & STATION_DEPART_FLAG){
                    ride->station_depart[stationIndex] &= ~STATION_DEPART_FLAG;
                    ride_invalidate_station_start(ride, stationIndex, 0);
                }
                return;
            }
        }

        // Continue racing
        ride->station_depart[stationIndex] |= STATION_DEPART_FLAG;
    } else {
        // Check if all vehicles are ready to go
        for (sint32 i = 0; i < ride->num_vehicles; i++) {
            rct_vehicle *vehicle = &(get_sprite(ride->vehicles[i])->vehicle);
            if (vehicle->status != VEHICLE_STATUS_WAITING_TO_DEPART && vehicle->status != VEHICLE_STATUS_DEPARTING) {
                if (ride->station_depart[stationIndex] & STATION_DEPART_FLAG){
                    ride->station_depart[stationIndex] &= ~STATION_DEPART_FLAG;
                    ride_invalidate_station_start(ride, stationIndex, 0);
                }
                return;
            }
        }

        // Begin the race
        ride_race_init_vehicle_speeds(ride);
        ride->lifecycle_flags |= RIDE_LIFECYCLE_PASS_STATION_NO_STOPPING;
        if (!(ride->station_depart[stationIndex] & STATION_DEPART_FLAG)){
            ride->station_depart[stationIndex] |= STATION_DEPART_FLAG;
            ride_invalidate_station_start(ride, stationIndex, 1);
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
static void ride_race_init_vehicle_speeds(rct_ride *ride)
{
    for (sint32 i = 0; i < ride->num_vehicles; i++) {
        rct_vehicle *vehicle = &get_sprite(ride->vehicles[i])->vehicle;
        vehicle->update_flags &= ~VEHICLE_UPDATE_FLAG_6;

        rct_ride_entry *rideEntry = get_ride_entry(vehicle->ride_subtype);

        vehicle->speed = (scenario_rand() & 16) - 8 + rideEntry->vehicles[vehicle->vehicle_type].powered_max_speed;

        if (vehicle->num_peeps != 0) {
            rct_peep *peep = &get_sprite(vehicle->peep[0])->peep;

            switch (peep_get_easteregg_name_id(peep)) {
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
static void ride_invalidate_station_start(rct_ride *ride, sint32 stationIndex, sint32 dl)
{
    sint32 x, y;
    rct_map_element *mapElement;

    x = (ride->station_starts[stationIndex] & 0xFF) * 32;
    y = (ride->station_starts[stationIndex] >> 8) * 32;
    mapElement = ride_get_station_start_track_element(ride, stationIndex);

    // If no station track found return
    if (mapElement == NULL)
        return;

    mapElement->properties.track.sequence &= 0x7F;
    if (dl != 0)
        mapElement->properties.track.sequence |= 0x80;

    // Invalidate map tile
    map_invalidate_tile_zoom1(x, y, mapElement->base_height * 8, mapElement->clearance_height * 8);
}

rct_map_element *ride_get_station_start_track_element(rct_ride *ride, sint32 stationIndex)
{
    sint32 x, y, z;
    rct_map_element *mapElement;

    x = ride->station_starts[stationIndex] & 0xFF;
    y = ride->station_starts[stationIndex] >> 8;
    z = ride->station_heights[stationIndex];

    // Find the station track element
    mapElement = map_get_first_element_at(x, y);
    do {
        if (map_element_get_type(mapElement) == MAP_ELEMENT_TYPE_TRACK && z == mapElement->base_height)
            return mapElement;

    } while (!map_element_is_last_for_tile(mapElement++));

    return NULL;
}

rct_map_element *ride_get_station_exit_element(rct_ride *ride, sint32 x, sint32 y, sint32 z)
{
    rct_map_element *mapElement;

    // Find the station track element
    mapElement = map_get_first_element_at(x, y);
    do {
        if (map_element_get_type(mapElement) == MAP_ELEMENT_TYPE_ENTRANCE && z == mapElement->base_height)
            return mapElement;
    } while (!map_element_is_last_for_tile(mapElement++));

    return NULL;
}
