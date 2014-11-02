/*****************************************************************************
 * Copyright (c) 2014 Ted John
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * This file is part of OpenRCT2.
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *****************************************************************************/

#include "../addresses.h"
#include "../scenario.h"
#include "../world/sprite.h"
#include "station.h"

static void ride_update_station_blocksection(rct_ride *ride, int stationIndex, int dl);
static void ride_update_station_bumpercar(rct_ride *ride, int stationIndex, int dl);
static void ride_update_station_race(rct_ride *ride, int stationIndex, int dl);
static void ride_race_init_vehicle_speeds(rct_ride *ride);
static void ride_invalidate_station_start(rct_ride *ride, int stationIndex, int dl);
static void sub_6AC2AF(rct_ride *ride, int stationIndex, int dl);

/**
 *
 *  rct2: 0x006ABFFB
 */
void ride_update_station(rct_ride *ride, int stationIndex)
{
	int dl, dh;

	if (ride->station_starts[stationIndex] == 0xFFFF)
		return;

	dl = 0;
	switch (ride->mode) {
	case RIDE_MODE_RACE:
		ride_update_station_race(ride, stationIndex, dl);
		break;
	case RIDE_MODE_BUMPERCAR:
		ride_update_station_bumpercar(ride, stationIndex, dl);
		break;
	case RIDE_MODE_CONTINUOUS_CIRCUIT_BLOCK_SECTIONED:
	case RIDE_MODE_POWERED_LAUNCH_BLOCK_SECTIONED:
		ride_update_station_blocksection(ride, stationIndex, dl);
		break;
	default:
		if (
			(ride->lifecycle_flags & (RIDE_LIFECYCLE_BROKEN_DOWN | RIDE_LIFECYCLE_CRASHED)) &&
			(ride->status == RIDE_STATUS_CLOSED && ride->num_riders == 0)
		) {
			dh = ride->var_062[stationIndex] & 0x7F;
			if (dh != 0 && dh != 0x7F && !(RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_TICKS, uint32) & 7))
				ride->var_062[stationIndex]--;
		} else {
			dl = 1;
			dh = ride->var_062[stationIndex] & 0x7F;
			if (dh != 0) {
				if (dh != 0x7F && !(RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_TICKS, uint32) & 31))
					ride->var_062[stationIndex]--;
				dl = 0;
			}
		}
		sub_6AC2AF(ride, stationIndex, dl);
		break;
	}
}

/**
 *
 *  rct2: 0x006AC0A1
 */
static void ride_update_station_blocksection(rct_ride *ride, int stationIndex, int dl)
{
	rct_map_element *mapElement;

	mapElement = ride_get_station_start_track_element(ride, stationIndex);

	if ((ride->status == RIDE_STATUS_CLOSED && ride->num_riders == 0) || mapElement->flags & 0x20) {
		dl = 0;
		if (ride->var_062[stationIndex] & (1 << 7)) {
			ride->var_062[stationIndex] &= ~(1 << 7);
			ride_invalidate_station_start(ride, stationIndex, dl);
			return;
		}

		if (mapElement->properties.track.sequence & 0x80) {
			ride_invalidate_station_start(ride, stationIndex, dl);
			return;
		}
	} else {
		dl = 1;
		if (!(ride->var_062[stationIndex] & (1 << 7))) {
			ride->var_062[stationIndex] |= (1 << 7);
			ride_invalidate_station_start(ride, stationIndex, dl);
			return;
		}

		if (mapElement->properties.track.sequence & 0x80) {
			ride_invalidate_station_start(ride, stationIndex, dl);
			return;
		}
	}

	sub_6AC2AF(ride, stationIndex, dl);
}

/**
 *
 *  rct2: 0x006AC12B
 */
static void ride_update_station_bumpercar(rct_ride *ride, int stationIndex, int dl)
{
	int i, dx, dh;
	rct_vehicle *vehicle;

	if (
		ride->status == RIDE_STATUS_CLOSED ||
		(ride->lifecycle_flags & (RIDE_LIFECYCLE_BROKEN_DOWN | RIDE_LIFECYCLE_CRASHED))
	) {
		sub_6AC2AF(ride, stationIndex, dl);
		return;
	}

	if (ride->lifecycle_flags & RIDE_LIFECYCLE_PASS_STATION_NO_STOPPING) {
		dx = ride->var_0D0 * 32;
		dl = dx & 0xFF;
		dh = (dx >> 8) & 0xFF;
		for (i = 0; i < ride->num_vehicles; i++) {
			vehicle = &(g_sprite_list[ride->vehicles[i]].vehicle);
			if (dh > vehicle->var_CE)
				continue;

			if (dh < vehicle->var_CE) {
				ride->lifecycle_flags &= ~VEHICLE_STATUS_WAITING_TO_DEPART;
				dh = 0;
				sub_6AC2AF(ride, stationIndex, dl);
				return;
			}

			if (dl > vehicle->var_51)
				continue;
		}

		dl = 1;
	} else {
		for (i = 0; i < ride->num_vehicles; i++) {
			vehicle = &(g_sprite_list[ride->vehicles[i]].vehicle);
			if (vehicle->status != VEHICLE_STATUS_WAITING_TO_DEPART) {
				sub_6AC2AF(ride, stationIndex, dl);
				return;
			}
		}

		ride->lifecycle_flags |= VEHICLE_STATUS_WAITING_TO_DEPART;
		ride->var_14D = 12;
		dl = 1;
	}
	sub_6AC2AF(ride, stationIndex, dl);
}

/**
 *
 *  rct2: 0x006AC1DF
 */
static void ride_update_station_race(rct_ride *ride, int stationIndex, int dl)
{
	int i, dh;
	rct_vehicle *vehicle;
	rct_peep *peep;

	if (
		ride->status == RIDE_STATUS_CLOSED ||
		(ride->lifecycle_flags & (RIDE_LIFECYCLE_BROKEN_DOWN | RIDE_LIFECYCLE_CRASHED))
	) {
		sub_6AC2AF(ride, stationIndex, dl);
		return;
	}

	if (!(ride->lifecycle_flags & VEHICLE_STATUS_WAITING_TO_DEPART)) {
		for (i = 0; i < ride->num_vehicles; i++) {
			vehicle = &(g_sprite_list[ride->vehicles[i]].vehicle);
			if (vehicle->status != VEHICLE_STATUS_WAITING_TO_DEPART && vehicle->status != VEHICLE_STATUS_DEPARTING) {
				sub_6AC2AF(ride, stationIndex, dl);
				return;
			}
		}
		
		ride_race_init_vehicle_speeds(ride);
		ride->lifecycle_flags |= VEHICLE_STATUS_WAITING_TO_DEPART;
		ride->var_14D = 12;
	} else {
		dh = ride->var_0D0;
		for (i = 0; i < ride->num_vehicles; i++) {
			vehicle = &(g_sprite_list[ride->vehicles[i]].vehicle);
			if (vehicle->status != VEHICLE_STATUS_WAITING_TO_DEPART && dh <= vehicle->var_CE) {
				ride->lifecycle_flags &= ~VEHICLE_STATUS_WAITING_TO_DEPART;
				if (vehicle->var_B3 != 0) {
					peep = &(g_sprite_list[vehicle->peep].peep);
					ride->race_winner = peep->sprite_index;
					ride->var_14D = 12;
				}
				sub_6AC2AF(ride, stationIndex, dl);
				return;
			}
		}
	}
	dl = 1;
	sub_6AC2AF(ride, stationIndex, dl);
}

/**
 *
 *  rct2: 0x006AC988
 * set the speed of the go kart type vehicle at the start to a random value or alter if peep name is an easter egg
 * @param ride (esi)
 */
static void ride_race_init_vehicle_speeds(rct_ride *ride)
{
	rct_ride_type *rideEntry;
	rct_vehicle *vehicle;
	uint8 *unk;
	int i;

	for (i = 0; i < ride->num_vehicles; i++) {
		vehicle = &g_sprite_list[ride->vehicles[i]].vehicle;
		vehicle->var_48 &= ~(1 << 6);

		rideEntry = GET_RIDE_ENTRY(vehicle->var_D6);
		unk = (uint8*)((int)rideEntry + (vehicle->var_31 * 0x65));

		vehicle->speed = (scenario_rand() & 16) - 8 + RCT2_GLOBAL(unk + 0x76, uint8);

		if (vehicle->var_B3) {
			rct_peep *peep = &g_sprite_list[vehicle->peep].peep;

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
static void ride_invalidate_station_start(rct_ride *ride, int stationIndex, int dl)
{
	int x, y;
	rct_map_element *mapElement;

	x = (ride->station_starts[stationIndex] & 0xFF) * 32;
	y = (ride->station_starts[stationIndex] >> 8) * 32;
	mapElement = ride_get_station_start_track_element(ride, stationIndex);

	mapElement->properties.track.sequence &= 0x7F;
	if (dl != 0)
		mapElement->properties.track.sequence |= 0x80;

	// Invalidate map tile
	map_invalidate_tile(x, y, mapElement->base_height * 8, mapElement->clearance_height * 8);
}

/**
 *
 *  rct2: 0x006AC2AF
 */
static void sub_6AC2AF(rct_ride *ride, int stationIndex, int dl)
{
	if (dl != 0)
		ride->var_062[stationIndex] |= (1 << 7);
	else
		ride->var_062[stationIndex] &= ~(1 << 7);
}

rct_map_element *ride_get_station_start_track_element(rct_ride *ride, int stationIndex)
{
	int x, y, z;
	rct_map_element *mapElement;

	x = ride->station_starts[stationIndex] & 0xFF;
	y = ride->station_starts[stationIndex] >> 8;
	z = ride->station_heights[stationIndex];

	// Get first element of the tile
	mapElement = TILE_MAP_ELEMENT_POINTER(y * 256 + x);

	// Find the station track element
	do {
		if ((mapElement->type & MAP_ELEMENT_TYPE_MASK) == MAP_ELEMENT_TYPE_TRACK && z == mapElement->base_height)
			return mapElement;

		mapElement++;
	} while (!((mapElement - 1)->flags & MAP_ELEMENT_FLAG_LAST_TILE));

	return NULL;
}