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

#include <windows.h>
#include "../addresses.h"
#include "../audio/audio.h"
#include "../game.h"
#include "../interface/window.h"
#include "../localisation/localisation.h"
#include "../management/news_item.h"
#include "../peep/peep.h"
#include "../peep/staff.h"
#include "../scenario.h"
#include "../world/map.h"
#include "../world/sprite.h"
#include "ride.h"
#include "ride_data.h"

#pragma region Ride classification table

const uint8 gRideClassifications[255] = {
	RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
	RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
	RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
	RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
	RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
	RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
	RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
	RIDE_CLASS_SHOP_OR_STALL, RIDE_CLASS_SHOP_OR_STALL, RIDE_CLASS_SHOP_OR_STALL,
	RIDE_CLASS_SHOP_OR_STALL, RIDE_CLASS_SHOP_OR_STALL, RIDE_CLASS_RIDE, RIDE_CLASS_SHOP_OR_STALL,
	RIDE_CLASS_KIOSK_OR_FACILITY, RIDE_CLASS_KIOSK_OR_FACILITY, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
	RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
	RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_KIOSK_OR_FACILITY, RIDE_CLASS_RIDE,
	RIDE_CLASS_RIDE, RIDE_CLASS_KIOSK_OR_FACILITY, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
	RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
	RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
	RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
	RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
	RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
	RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
	RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
	RIDE_CLASS_RIDE, RIDE_CLASS_SHOP_OR_STALL, RIDE_CLASS_RIDE, RIDE_CLASS_SHOP_OR_STALL,
	RIDE_CLASS_SHOP_OR_STALL, RIDE_CLASS_SHOP_OR_STALL, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
	RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
	RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
	RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
	RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
	RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
	RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
	RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
	RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
	RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
	RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
	RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
	RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
	RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
	RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
	RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
	RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
	RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
	RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
	RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
	RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
	RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
	RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
	RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
	RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
	RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
	RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
	RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
	RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
	RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
	RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
	RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
	RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
	RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
	RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
	RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
	RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
	RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
	RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
	RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
	RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
	RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
	RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE
};

#pragma endregion

static const int RideInspectionInterval[] = {
	10, 20, 30, 45, 60, 120, 0, 0
};

rct_ride_type **gRideTypeList = RCT2_ADDRESS(0x009ACFA4, rct_ride_type*);
rct_ride* g_ride_list = RCT2_ADDRESS(RCT2_ADDRESS_RIDE_LIST, rct_ride);

static void ride_init_vehicle_speed(rct_ride *ride);

int ride_get_count()
{
	rct_ride *ride;
	int i, count = 0;

	FOR_ALL_RIDES(i, ride)
		count++;

	return count;
}

int ride_get_total_queue_length(rct_ride *ride)
{
	int i, queueLength = 0;
	for (i = 0; i < 4; i++)
		if (ride->entrances[i] != 0xFFFF)
			queueLength += ride->queue_length[i];
	return queueLength;
}

int ride_get_max_queue_time(rct_ride *ride)
{
	int i, queueTime = 0;
	for (i = 0; i < 4; i++)
		if (ride->entrances[i] != 0xFFFF)
			queueTime = max(queueTime, ride->queue_time[i]);
	return queueTime;
}

/**
 *
 *  rct2: 0x006ACA89
 */
void ride_init_all()
{
	int i;
	rct_ride *ride;
	rct_ride_measurement *ride_measurement;

	for (i = 0; i < MAX_RIDES; i++) {
		ride = &g_ride_list[i];
		ride->type = RIDE_TYPE_NULL;
	}

	RCT2_GLOBAL(0x0138B590, sint8) = 0;
	RCT2_GLOBAL(0x0138B591, sint8) = 0;

	for (i = 0; i < MAX_RIDE_MEASUREMENTS; i++) {
		ride_measurement = GET_RIDE_MEASUREMENT(i);
		ride_measurement->ride_index = 255;
	}
}

/**
*
*  rct2: 0x006B7A38
*/
void reset_all_ride_build_dates() {
	int i;
	rct_ride *ride;
	FOR_ALL_RIDES(i, ride) {
		//mov	ax, current_month_year
		//sub	[esi + 180h], ax
		ride->build_date -= RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_MONTH_YEAR, uint16);
	}
}

/**
  * rct2: 0x006AC916
  */
void ride_update_favourited_stat()
{
	int i;
	rct_ride *ride;
	uint16 spriteIndex;
	rct_peep* peep;

	FOR_ALL_RIDES(i, ride)
		ride->guests_favourite = 0;

	FOR_ALL_PEEPS(spriteIndex, peep) {
		if (peep->linked_list_type_offset != SPRITE_LINKEDLIST_OFFSET_PEEP)
			return;
		if (peep->favourite_ride != 0xff) {
			ride = &g_ride_list[peep->favourite_ride];
			ride->guests_favourite++;
			ride->var_14D |= 1;

		}

	}

	window_invalidate_by_class(WC_RIDE_LIST);
}

money32 get_shop_item_cost(int shopItem)
{
	return shopItem < 32 ?
		RCT2_GLOBAL(0x00982164 + (shopItem * 8), uint16) :
		RCT2_GLOBAL(0x00982144 + (shopItem * 8), uint16);
}

/**
 *
 * rct2: 0x006AC3AB
 */
money32 ride_calculate_income_per_hour(rct_ride *ride)
{
	rct_ride_type *entry;
	money32 incomePerHour, priceMinusCost;
	int shopItem;

	entry = GET_RIDE_ENTRY(ride->subtype);
	incomePerHour =
		ride->var_124 +
		ride->var_126 +
		ride->var_128 +
		ride->var_12A +
		ride->var_12C +
		ride->var_12E +
		ride->age +
		ride->running_cost +
		ride->var_134 +
		ride->var_136;
	incomePerHour *= 12;
	priceMinusCost = ride->price;

	shopItem = entry->shop_item;
	if (shopItem != 255) {
		priceMinusCost -= get_shop_item_cost(shopItem);

		shopItem = entry->shop_item_secondary;
		if (shopItem != 255) {
			priceMinusCost -= get_shop_item_cost(shopItem);
			priceMinusCost /= 2;
		}
	}

	incomePerHour *= priceMinusCost;
	return incomePerHour;
}

/**
 *
 *  rct2: 0x006BC6D8
 */
void ride_play_music()
{

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

/**
 *
 *  rct2: 0x006ECB60
 * NOTE: x, y and z are in pixels, not tile units
 */
void map_invalidate_tile(int x, int y, int zLow, int zHigh)
{
	RCT2_CALLPROC_X(0x006ECB60, x, 0, y, 0, zHigh, zLow, 0);
}

/**
 *
 *  rct2: 0x006AC2C7
 */
void ride_invalidate_station_start(rct_ride *ride, int stationIndex, int dl)
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

void sub_6AC2AF(rct_ride *ride, int stationIndex, int dl)
{
	if (dl != 0)
		ride->var_062[stationIndex] |= (1 << 7);
	else
		ride->var_062[stationIndex] &= ~(1 << 7);
}

/**
 *
 *  rct2: 0x006AC1DF
 */
void ride_update_station_race(rct_ride *ride, int stationIndex, int dl)
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
		
		ride_init_vehicle_speed(ride);
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
 *  rct2: 0x006AC12B
 */
void ride_update_station_bumpercar(rct_ride *ride, int stationIndex, int dl)
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
 *  rct2: 0x006AC0A1
 */
void ride_update_station_blocksection(rct_ride *ride, int stationIndex, int dl)
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

int sub_6B7294(rct_ride *ride)
{
	int eax, ebx, ecx, edx, esi, edi, ebp;

	esi = (int)ride;
	RCT2_CALLFUNC_X(0x006B7294, &eax, &ebx, &ecx, &edx, &esi, &edi, &ebp);
	return ebx;
}

/**
 *
 *  rct2: 0x006ABE73
 */
void ride_update(int index)
{
	rct_vehicle *vehicle;
	int j, x, y, z;
	rct_map_element *mapElement;
	rct_ride *ride = GET_RIDE(index);
	
	if (ride->var_1CA != 0)
		ride->var_1CA--;

	if (!(RCT2_GLOBAL(0x0097D4F2 + (ride->type * 8), uint16) & 6)) {
		if (ride->status == RIDE_STATUS_OPEN && !(ride->lifecycle_flags & RIDE_LIFECYCLE_MUSIC)) {
			if (ride->type == RIDE_TYPE_CIRCUS_SHOW) {
				vehicle = &(g_sprite_list[ride->vehicles[0]].vehicle);
				if (vehicle->status != VEHICLE_STATUS_DOING_CIRCUS_SHOW) {
					ride->var_15C = 255;
					goto loc_6ABF76;
				}
			}

			if (!(ride->lifecycle_flags & (RIDE_LIFECYCLE_6 | RIDE_LIFECYCLE_BROKEN_DOWN))) {
				if (ride->var_18C == 7) {
					if (!(RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_TICKS, uint32) & 7))
						if (ride->var_1AC != 255)
							ride->var_1AC++;
				} else {
					if ((ride->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN) || ride->var_18C == 6 || ride->var_18C != 7)
						if (ride->var_1AC != 255)
							ride->var_1AC++;

					if (ride->var_1AC == 255) {
						ride->var_15C = 255;
						goto loc_6ABF76;
					}
				}
			}

			if (ride->var_15C == 255) {
				uint8 *someKindOfMusicInfo = RCT2_ADDRESS(0x009AEF28, uint8*)[ride->music];
				uint8 someKindOfMusicInfoCount = *someKindOfMusicInfo++;
				ride->var_15C = someKindOfMusicInfo[scenario_rand() % someKindOfMusicInfoCount];
				ride->var_188 = 0;
			}
			goto loc_6ABF76;
		}
		ride->var_15C = 255;

	loc_6ABF76:
		if (ride->var_15C != 255) {
			// Ride music of some sort, possibly break down power cut effect
			x = (ride->station_starts[0] & 0xFF) * 32 + 16;
			y = (ride->station_starts[0] >> 8) * 32 + 16;
			z = ride->station_heights[0] * 8;

			uint8 bh = ride->var_15C;
			int sampleRate = 22050;
			if (!(ride->lifecycle_flags & (RIDE_LIFECYCLE_6 | RIDE_LIFECYCLE_BROKEN_DOWN))) {
				sampleRate = ride->var_1AC * 70;
				if (ride->var_18C != 7)
					sampleRate *= -1;
				sampleRate += 22050;
			}

			ride->var_188 = sub_6BC3AC(x, y, z, (ride->var_15C << 8) | index, ride->var_188, sampleRate);
			ride->var_15C = bh;
		}
	}

	// Update stations
	if (ride->type != RIDE_TYPE_MAZE)
		for (j = 0; j < 4; j++)
			ride_update_station(ride, j);

	// 
	ride->var_122++;
	if (ride->var_122 >= 960) {
		ride->var_122 = 0;

		ride->var_136 = ride->var_134;
		ride->var_134 = ride->running_cost;
		ride->running_cost = ride->age;
		ride->age = ride->var_12E;
		ride->var_12E = ride->var_12C;
		ride->var_12C = ride->var_12A;
		ride->var_12A = ride->var_128;
		ride->var_128 = ride->var_126;
		ride->var_126 = ride->var_124;
		ride->var_124 = ride->var_120;
		ride->var_14D |= 1;

		ride->income_per_hour = ride_calculate_income_per_hour(ride);
		ride->var_14D |= 2;

		if (ride->upkeep_cost != (money16)0xFFFF) {
			ride->upkeep_cost *= -16;
			ride->upkeep_cost += (money16)ride->income_per_hour;
		}
	}

	if (ride->type == RIDE_TYPE_CHAIRLIFT && (ride->lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK)) {
		if (
			(ride->lifecycle_flags & (RIDE_LIFECYCLE_6 | RIDE_LIFECYCLE_BROKEN_DOWN | RIDE_LIFECYCLE_CRASHED)) ||
			ride->var_18C != 0
		) {
			int ax = ride->var_0D0 * 2048;
			int bx = ride->var_148;
			int cx = bx + ax;
			ride->var_148 = cx;
			if (bx >> 14 != cx >> 14) {
				x = (ride->var_13A & 0xFF) * 32;
				y = (ride->var_13A >> 8) * 32;
				z = ride->var_13E * 8;
				map_invalidate_tile(x, y, z, z + (4 * 8));

				x = (ride->var_13C & 0xFF) * 32;
				y = (ride->var_13C >> 8) * 32;
				z = ride->var_13F * 8;
				map_invalidate_tile(x, y, z, z + (4 * 8));
			}
		}
	}

	if (
		!(RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_TICKS, uint32) & 3) &&
		ride->type == RIDE_TYPE_SPIRAL_SLIDE &&
		ride->var_15D != 0
	) {
		ride->var_176++;
		if (ride->var_176 >= 48) {
			ride->var_15D--;

			// TODO find out what type of sprite is stored here
			rct_sprite *sprite = &g_sprite_list[ride->maze_tiles];
			RCT2_GLOBAL((int)sprite + 0x32, uint16)++;
		}

		// Invalidate something related to station start
		for (j = 0; j < 4; j++) {
			if (ride->station_starts[j] == 0xFFFF)
				continue;

			x = ride->station_starts[j] & 0xFF;
			y = ride->station_starts[j] >> 8;
			z = ride->station_heights[j];

			mapElement = ride_get_station_start_track_element(ride, j);
			int rotation = ((mapElement->type & 3) << 2) | RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_ROTATION, uint8);
			x += RCT2_GLOBAL(0x0098DDB8 + (rotation * 4), uint16);
			y += RCT2_GLOBAL(0x0098DDBA + (rotation * 4), uint16);

			map_invalidate_tile(x, y, mapElement->base_height * 8, mapElement->clearance_height * 8);
		}
	}

	if (
		!(RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_TICKS, uint32) & 255) &&
		!(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_TRACK_DESIGNER)
	) {
		if (ride->lifecycle_flags & (RIDE_LIFECYCLE_BROKEN_DOWN | RIDE_LIFECYCLE_CRASHED))
			ride->var_19C++;

		if (!(RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_TICKS, uint32) & 8191)) {
			int ax =
				ride->var_19C +
				ride->var_19D +
				ride->var_19E +
				ride->var_1A0 +
				ride->var_1A2 +
				ride->var_1A3;
			ride->var_199 = min(ax / 2, 100);

			ride->var_1A3 = ride->var_1A2;
			ride->var_1A2 = ride->var_1A1;
			ride->var_1A1 = ride->var_1A0;
			ride->var_1A0 = ride->var_19F;
			ride->var_19F = ride->var_19E;
			ride->var_19E = ride->var_19D;
			ride->var_19D = ride->var_19C;
			ride->var_19C = 0;
			ride->var_14D |= 32;
		}

		if (
			!(ride->lifecycle_flags & (RIDE_LIFECYCLE_6 | RIDE_LIFECYCLE_BROKEN_DOWN | RIDE_LIFECYCLE_CRASHED)) &&
			ride->status != RIDE_STATUS_CLOSED
		) {
			int ax = ride->var_198;
			int dx = 0;
			int cx = (RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_MONTH_YEAR, uint16) - ride->build_date) >> 3;
			if (cx != 0) {
				dx = ax >> 3;
				if (cx != 1) {
					dx = ax >> 2;
					if (cx != 2) {
						dx = ax >> 1;
						if (cx > 4) {
							dx = ax;
							if (cx > 7) {
								dx = ax << 1;
							}
						}
					}
				}
			}
			ax += dx;
			ride->var_196 = max(0, ride->var_196 - ax);
			ride->var_14D |= 32;

			int ebx = ride->var_196;
			if (ebx == 0) {
				goto loc_6AC787;
			} else {
				ebx *= -1;
				ebx += 25856;
				if ((scenario_rand() & 0x2FFFFF) <= ebx) {
				loc_6AC787:
					if (sub_6B7294(ride) != -1) {
						// Possibly do breakdown function
						RCT2_CALLPROC_X(0x006B75C8, 0, 0, 0, index, 0, 0, 0);
					}
				}
			}
		}
	}

	// ?
	if (ride->lifecycle_flags & (RIDE_LIFECYCLE_6 | RIDE_LIFECYCLE_BROKEN_DOWN | RIDE_LIFECYCLE_DUE_INSPECTION))
		if (((RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_TICKS, uint32) >> 1) & 255) == index)
			RCT2_CALLPROC_X(0x006B75C8, 0, 0, 0, index, 0, 0, 0);

	// Inspection
	if (!(RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_TICKS, uint32) & 2047)) {
		if (!(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_TRACK_DESIGNER)) {
			ride->last_inspection++;
			if (ride->last_inspection == 0)
				ride->last_inspection--;

			int inspectionIntervalMinutes = RideInspectionInterval[ride->inspection_interval];
			if (inspectionIntervalMinutes != 0) {
				if (RCT2_ADDRESS(0x0097C740, uint32)[ride->type] != 0) {
					if (inspectionIntervalMinutes <= ride->last_inspection) {
						if (!(ride->lifecycle_flags & (RIDE_LIFECYCLE_6 | RIDE_LIFECYCLE_BROKEN_DOWN | RIDE_LIFECYCLE_DUE_INSPECTION | RIDE_LIFECYCLE_CRASHED))) {
							ride->lifecycle_flags |= RIDE_LIFECYCLE_DUE_INSPECTION;
								
							ride->var_190 = 0;
							for (j = 0; j < 4; j++) {
								if (ride->exits[j] != 0xFFFF) {
									ride->var_190 = j;
									break;
								}
							}
						}
					}
				}
			}
		}
	}
}

/**
 *
 *  rct2: 0x006ABE4C
 */
void ride_update_all()
{
	rct_s6_info *s6Info = (rct_s6_info*)0x0141F570;
	rct_ride *ride;
	int i;

	// Remove all rides if certain flags are set (possible scenario editor?)
	int *esi = (int*)0x9DCE9E;
	if (esi[0x1BCA] & 2) {
		if (s6Info->var_000 <= 2)
			FOR_ALL_RIDES(i, ride)
				ride->type = RIDE_TYPE_NULL;
		return;
	}

	// Something related to windows
	RCT2_CALLPROC_EBPSAFE(0x006BC348);

	// Update rides
	FOR_ALL_RIDES(i, ride)
		ride_update(i);

	ride_play_music();
}

/**
 * rct2: 0x006B7C59
 * @return 1 if the coordinate is reachable or has no entrance, 0 otherwise
 */
int ride_entrance_exit_is_reachable(uint16 coordinate, rct_ride* ride, int index) {
	int x = ((coordinate >> 8) & 0xFF) << 5, // cx
		y = (coordinate & 0xFF) << 5;		 // ax	
	uint8 station_height = ride->station_heights[index];
	int tile_idx = ((x << 8) | y) >> 5;
	rct_map_element* tile = RCT2_ADDRESS(RCT2_ADDRESS_TILE_MAP_ELEMENT_POINTERS, rct_map_element*)[tile_idx];

	while(1) {
        uint8 element_type = tile->type & MAP_ELEMENT_TYPE_MASK;
        if (element_type == MAP_ELEMENT_TYPE_ENTRANCE && station_height == tile->base_height) {
            break;
        } else if (tile->flags & MAP_ELEMENT_FLAG_LAST_TILE) {
            return 1;
        }
        tile++;
	}

	uint8 face_direction = tile->type & 3;
	y -= RCT2_ADDRESS(0x00993CCC, sint16)[face_direction * 2];
	x -= RCT2_ADDRESS(0x00993CCE, sint16)[face_direction * 2];
	tile_idx = ((x << 8) | y) >> 5;

    return map_coord_is_connected(tile_idx, station_height, face_direction);
}


void ride_entrance_exit_connected(rct_ride* ride, int ride_idx)
{
	for (int i = 0; i < 4; ++i) {
		uint16 station_start = ride->station_starts[i],
			entrance = ride->entrances[i],
			exit = ride->exits[i];

		if (station_start == -1 )
			continue;
		if (entrance != -1 && !ride_entrance_exit_is_reachable(entrance, ride, i)) {
			// name of ride is parameter of the format string
			RCT2_GLOBAL(0x013CE952, uint16) = ride->name;
			RCT2_GLOBAL(0x013CE954, uint32) = ride->name_arguments;			
			news_item_add_to_queue(1, STR_ENTRANCE_NOT_CONNECTED, ride_idx);
			ride->connected_message_throttle = 3;
		}
			
		if (exit != -1 && !ride_entrance_exit_is_reachable(exit, ride, i)) {
			// name of ride is parameter of the format string
			RCT2_GLOBAL(0x013CE952, uint16) = ride->name;
			RCT2_GLOBAL(0x013CE954, uint32) = ride->name_arguments;
			news_item_add_to_queue(1, STR_EXIT_NOT_CONNECTED, ride_idx);
			ride->connected_message_throttle = 3;
		}

	} 
}


void ride_shop_connected(rct_ride* ride, int ride_idx)
{
	rct_ride* ride_back = ride;
    uint16 coordinate = ride->station_starts[0];
	if (coordinate == 0xFFFF)
		return;

	int x = ((coordinate >> 8) & 0xFF) << 5, // cx
		y = (coordinate & 0xFF) << 5;		 // ax	
	
	rct_map_element* tile = RCT2_ADDRESS(RCT2_ADDRESS_TILE_MAP_ELEMENT_POINTERS, rct_map_element*)[coordinate];
	
    for (; ; tile++){
        uint8 element_type = tile->type & MAP_ELEMENT_TYPE_MASK;
        if(element_type == MAP_ELEMENT_TYPE_TRACK && tile->properties.track.ride_index == ride_idx)
            break;
        if(tile->flags & MAP_ELEMENT_FLAG_LAST_TILE)
            return;
    }

	uint16 entrance_directions = 0;
    uint8 track_type = tile->properties.track.type;
	ride = &g_ride_list[tile->properties.track.ride_index];
    if (RCT2_GLOBAL(RCT2_ADDRESS_RIDE_FLAGS + ride->type * 8, uint32) & 0x80000) {
		entrance_directions = RCT2_ADDRESS(0x0099CA64, uint8)[track_type * 16];
    } else {
		entrance_directions = RCT2_ADDRESS(0x0099BA64, uint8)[track_type * 16];
    }

	
	uint8 tile_direction = tile->type & MAP_ELEMENT_DIRECTION_MASK;
	entrance_directions <<= tile_direction;
	entrance_directions = ((entrance_directions >> 12) | entrance_directions) & 0xF;

	// now each bit in entrance_directions stands for an entrance direction to check
	if (entrance_directions == 0)
		return;

	for (int count = 0; entrance_directions != 0; ++count) {
		if (!(entrance_directions & 1)) {
			entrance_directions >>= 1;
            continue;
        }
		entrance_directions >>= 1;

        uint8 face_direction = count ^ 2; // flip direction north<->south, east<->west
		int y2 = y - RCT2_ADDRESS(0x00993CCC, sint16)[face_direction * 2];
		int x2 = x - RCT2_ADDRESS(0x00993CCE, sint16)[face_direction * 2];
        int tile_idx = ((x2 << 8) | y2) >> 5;

        if (map_coord_is_connected(tile_idx, tile->base_height, face_direction))
            return;
    }    
    
	// name of ride is parameter of the format string
    RCT2_GLOBAL(0x013CE952, uint16) = ride->name;
	RCT2_GLOBAL(0x013CE954, uint32) = ride->name_arguments;
	news_item_add_to_queue(1, STR_ENTRANCE_NOT_CONNECTED, ride_idx);

    ride->connected_message_throttle = 3;
}



/**
 * rct2: 0x006B7A5E
 **/
void ride_check_all_reachable()
{
	rct_ride *ride;
	int i;
	
	FOR_ALL_RIDES(i, ride) {		
		if (ride->connected_message_throttle != 0)
			ride->connected_message_throttle--;
		if (ride->status != RIDE_STATUS_OPEN || ride->connected_message_throttle != 0)
			continue;

		if (RCT2_GLOBAL(RCT2_ADDRESS_RIDE_FLAGS + ride->type * 8, uint32) & 0x20000)
            ride_shop_connected(ride, i);
		else
			ride_entrance_exit_connected(ride, i);
	}
}

/**
 * 
 * rct2: 0x006CAF80
 * ax result x
 * bx result y
 * dl ride index
 * esi result map element
 */
rct_map_element *sub_6CAF80(int rideIndex, int *outX, int *outY)
{
	rct_map_element *resultMapElement, *mapElement;
	int foundSpecialTrackPiece;

	resultMapElement = (rct_map_element*)-1;
	foundSpecialTrackPiece = 0;

	uint16 x, y;
	for (x = 0; x < 256; x++) {
		for (y = 0; y < 256; y++) {
			// Iterate through map elements on tile
			int tileIndex = (y << 8) | x;
			mapElement = TILE_MAP_ELEMENT_POINTER(tileIndex);
			do {
				if ((mapElement->type & MAP_ELEMENT_TYPE_MASK) != MAP_ELEMENT_TYPE_TRACK)
					continue;
				if (rideIndex != mapElement->properties.track.ride_index)
					continue;

				// Found a track piece for target ride

				// Check if its a ???
				int specialTrackPiece = (
					(mapElement->properties.track.type != 2 && mapElement->properties.track.type != 3) &&
					(RCT2_ADDRESS(0x0099BA64, uint8)[mapElement->properties.track.type * 16] & 0x10)
				);

				// Set result tile to this track piece if first found track or a ???
				if (resultMapElement == (rct_map_element*)-1 || specialTrackPiece) {
					resultMapElement = mapElement;

					if (outX != NULL) *outX = x * 32;
					if (outY != NULL) *outY = y * 32;
				}

				if (specialTrackPiece) {
					foundSpecialTrackPiece = 1;
					return resultMapElement;
				}
			} while (!(mapElement->flags & MAP_ELEMENT_FLAG_LAST_TILE) && mapElement++);
		}
	}
	return resultMapElement;
}

/**
 * 
 * rct2: 0x006CB02F
 * ax result x
 * bx result y
 * esi input / output map element
 */
rct_map_element *ride_find_track_gap(rct_map_element *startTrackElement, int *outX, int *outY)
{
	int eax, ebx, ecx, edx, esi, edi, ebp;
	esi = (int)startTrackElement;
	eax = *outX;
	ebx = 0;
	ecx = *outY;
	edx = 0;
	edi = 0;
	ebp = 0;
	RCT2_CALLFUNC_X(0x006CB02F, &eax, &ebx, &ecx, &edx, &esi, &edi, &ebp);

	if (outX != NULL) *outX = eax & 0xFFFF;
	if (outY != NULL) *outY = ecx & 0xFFFF;
	return (rct_map_element*)esi;
}

int ride_create_ride(ride_list_item listItem)
{
	int eax, ebx, ecx, edx, esi, edi, ebp;
	edx = *((uint16*)&listItem);
	eax = 0;
	ecx = 0;
	ebx = 1;
	edi = 0;
	esi = 0;

	RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TITLE, uint16) = 0x3DC;

	esi = GAME_COMMAND_6;
	game_do_command_p(esi, &eax, &ebx, &ecx, &edx, &esi, &edi, &ebp);
	return ebx == 0x80000000 ? -1 : edi;
}

/**
 *
 * rct2: 0x006B4800
 */
void ride_construct_new(ride_list_item listItem)
{
	rct_window *w;
	int rideIndex;

	rideIndex = ride_create_ride(listItem);
	if (rideIndex == -1)
		return;

	// Open construction window
	// HACK In the original game this created a mouse up event. This has been
	// replaced with a direct call to the function that gets called.
	// Eventually should be changed so the ride window does not need to be opened.
	w = window_ride_main_open(rideIndex);
	window_ride_construct(w);
	window_close_by_number(WC_RIDE, rideIndex);
}

/**
 * 
 * rct2: 0x006CC056
 */
int ride_try_construct(rct_map_element *trackMapElement)
{
	// Success stored in carry flag which can't be accessed after call using is macro
	RCT2_CALLPROC_X(0x006CC056, 0, 0, 0, (int)trackMapElement, 0, 0, 0);
	return 1;
}

/**
 * 
 *  rct2: 0x006AF561
 */
void ride_get_status(int rideIndex, int *formatSecondary, int *argument)
{
	rct_ride *ride = &g_ride_list[rideIndex];

	if (ride->lifecycle_flags & RIDE_LIFECYCLE_CRASHED) {
		*formatSecondary = STR_CRASHED;
		return;
	}
	if (ride->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN) {
		*formatSecondary = STR_BROKEN_DOWN;
		return;
	}
	if (ride->status == RIDE_STATUS_CLOSED) {
		*formatSecondary = STR_CLOSED;
		return;
	}
	if (ride->status == RIDE_STATUS_TESTING) {
		*formatSecondary = STR_TEST_RUN;
		return;
	}
	rct_peep *peep = GET_PEEP(ride->race_winner);
	if (ride->mode == RIDE_MODE_RACE && !(ride->lifecycle_flags & RIDE_LIFECYCLE_PASS_STATION_NO_STOPPING) && ride->race_winner != 0xFFFF && peep->sprite_identifier == SPRITE_IDENTIFIER_PEEP) {
		if (peep->name_string_idx == STR_GUEST) {
			*argument = peep->id;
			*formatSecondary = STR_RACE_WON_BY_GUEST;
		} else {
			*argument = peep->name_string_idx;
			*formatSecondary = STR_RACE_WON_BY;
		}
	} else {
		if (!(RCT2_GLOBAL(RCT2_ADDRESS_RIDE_FLAGS + ride->type * 8, uint32) & 0x20000)) {
			*argument = ride->num_riders;
			*formatSecondary = STR_PERSON_ON_RIDE;
			if(*argument != 1)
				*formatSecondary = STR_PEOPLE_ON_RIDE;

		} else {
			*formatSecondary = STR_OPEN;
		}
	}
}

rct_peep *ride_get_assigned_mechanic(rct_ride *ride)
{
	rct_peep *peep;

	if (ride->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN) {
		if (
			ride->mechanic_status == RIDE_MECHANIC_STATUS_HEADING ||
			ride->mechanic_status == 3 ||
			ride->mechanic_status == 4
		) {
			peep = &(g_sprite_list[ride->mechanic].peep);
			if (peep_is_mechanic(peep))
				return peep;
		}
	}

	return NULL;
}

int ride_get_total_length(rct_ride *ride)
{
	int i, totalLength = 0;
	for (i = 0; i < ride->num_stations; i++)
		totalLength += ride->length[i];
	return totalLength;
}

int ride_can_have_multiple_circuits(rct_ride *ride)
{
	if (!(RCT2_GLOBAL(0x0097D4F2 + (ride->type * 8), uint16) & 0x200))
		return 0;

	// Only allow circuit or launch modes
	if (
		ride->mode != RIDE_MODE_CONTINUOUS_CIRCUIT &&
		ride->mode != RIDE_MODE_REVERSE_INCLINE_LAUNCHED_SHUTTLE &&
		ride->mode != RIDE_MODE_POWERED_LAUNCH
	) {
		return 0;
	}
	
	// Must have no more than one vehicle and one station
	if (ride->num_vehicles > 1 || ride->num_stations > 1)
		return 0;

	return 1;
}

track_colour ride_get_track_colour(rct_ride *ride, int colourScheme)
{
	track_colour result;
	result.main = ride->track_colour_main[colourScheme];
	result.additional = ride->track_colour_additional[colourScheme];
	result.supports = ride->track_colour_supports[colourScheme];
	return result;
}

vehicle_colour ride_get_vehicle_colour(rct_ride *ride, int vehicleIndex)
{
	vehicle_colour result;
	result.main = ride->vehicle_colours[vehicleIndex] & 0xFF;
	result.additional_1 = ride->vehicle_colours[vehicleIndex] >> 8;
	result.additional_2 = ride->vehicle_colours_extended[vehicleIndex];
	return result;
}

/**
 *
 *  rct2: 0x006AC988
 * set the speed of the go kart type vehicle at the start to a random value or alter if peep name is an easter egg
 * @param ride (esi)
 */
static void ride_init_vehicle_speed(rct_ride *ride)
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

rct_ride_type *ride_get_entry(rct_ride *ride)
{
	return GET_RIDE_ENTRY(ride->subtype);
}

uint8 *get_ride_entry_indices_for_ride_type(uint8 rideType)
{
	uint8 *typeToRideEntryIndexMap = (uint8*)0x009E32F8;
	uint8 *entryIndexList = typeToRideEntryIndexMap;
	while (rideType > 0) {
		do {
			entryIndexList++;
		} while (*(entryIndexList - 1) != 255);
		rideType--;
	}
	return entryIndexList;
}



/**
 *  rct2: 0x006B64F2
 */
void ride_measurement_update(rct_ride_measurement *measurement)
{
	uint16 spriteIndex;
	rct_ride *ride;
	rct_vehicle *vehicle;
	int unk, velocity, altitude, verticalG, lateralG;

	ride = GET_RIDE(measurement->ride_index);
	spriteIndex = ride->vehicles[measurement->vehicle_index];
	if (spriteIndex == SPRITE_INDEX_NULL)
		return;

	vehicle = &(g_sprite_list[spriteIndex].vehicle);

	if (measurement->flags & RIDE_MEASUREMENT_FLAG_UNLOADING) {
		if (vehicle->status != VEHICLE_STATUS_DEPARTING && vehicle->status != VEHICLE_STATUS_STOPPING)
			return;

		measurement->flags &= ~RIDE_MEASUREMENT_FLAG_UNLOADING;
		if (measurement->var_0B == vehicle->var_4B)
			measurement->current_item = 0;
	}

	if (vehicle->status == VEHICLE_STATUS_UNLOADING_PASSENGERS) {
		measurement->flags |= RIDE_MEASUREMENT_FLAG_UNLOADING;
		return;
	}

	unk = (vehicle->var_36 / 4) & 0xFF;
	if (unk == 216 || unk == 123 || unk == 9 || unk == 63 || unk == 147 || unk == 155)
		if (vehicle->velocity == 0)
			return;

	if (measurement->current_item >= RIDE_MEASUREMENT_MAX_ITEMS)
		return;

	if (measurement->flags & RIDE_MEASUREMENT_FLAG_G_FORCES) {
		vehicle_get_g_forces(vehicle, &verticalG, &lateralG);
		verticalG = clamp(-127, verticalG / 8, 127);
		lateralG = clamp(-127, lateralG / 8, 127);

		if (RCT2_GLOBAL(RCT2_ADDRESS_SCENARIO_TICKS, uint32) & 1) {
			verticalG = (verticalG + measurement->vertical[measurement->current_item]) / 2;
			lateralG = (lateralG + measurement->lateral[measurement->current_item]) / 2;
		}

		measurement->vertical[measurement->current_item] = verticalG & 0xFF;
		measurement->lateral[measurement->current_item] = lateralG & 0xFF;
	}

	velocity = min(abs((vehicle->velocity * 5) >> 16), 255);
	altitude = min(vehicle->z / 8, 255);

	if (RCT2_GLOBAL(RCT2_ADDRESS_SCENARIO_TICKS, uint32) & 1) {
		velocity = (velocity + measurement->velocity[measurement->current_item]) / 2;
		altitude = (altitude + measurement->altitude[measurement->current_item]) / 2;
	}

	measurement->velocity[measurement->current_item] = velocity & 0xFF;
	measurement->altitude[measurement->current_item] = altitude & 0xFF;

	if (RCT2_GLOBAL(RCT2_ADDRESS_SCENARIO_TICKS, uint32) & 1) {
		measurement->current_item++;
		measurement->num_items = max(measurement->num_items, measurement->current_item);
	}
}

/**
 *  rct2: 0x006B6456
 */
void ride_measurements_update()
{
	rct_ride *ride;
	rct_ride_measurement *measurement;
	rct_vehicle *vehicle;
	int i, j;
	uint16 spriteIndex;

	if (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_SCENARIO_EDITOR)
		return;

	// For each ride measurement
	for (i = 0; i < MAX_RIDE_MEASUREMENTS; i++) {
		measurement = GET_RIDE_MEASUREMENT(i);
		if (measurement->ride_index == 255)
			continue;

		ride = GET_RIDE(measurement->ride_index);
		if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK))
			continue;

		if (measurement->flags & RIDE_MEASUREMENT_FLAG_RUNNING) {
			ride_measurement_update(measurement);
		} else {
			// For each vehicle
			for (j = 0; j < ride->num_vehicles; j++) {
				spriteIndex = ride->vehicles[j];
				if (spriteIndex == SPRITE_INDEX_NULL)
					continue;

				vehicle = &(g_sprite_list[spriteIndex].vehicle);
				if (vehicle->status == VEHICLE_STATUS_DEPARTING || vehicle->status == VEHICLE_STATUS_STOPPING) {
					measurement->vehicle_index = j;
					measurement->var_0B = vehicle->var_4B;
					measurement->flags |= RIDE_MEASUREMENT_FLAG_RUNNING;
					measurement->flags &= ~RIDE_MEASUREMENT_FLAG_UNLOADING;
					ride_measurement_update(measurement);
					break;
				}
			}

		}
	}
}

/**
 * 
 * rct2: 0x006B66D9
 */
rct_ride_measurement *ride_get_measurement(int rideIndex, rct_string_id *message)
{
	rct_ride *ride;
	rct_ride_measurement *measurement;
	uint32 lruTicks;
	int i, lruIndex;

	ride = GET_RIDE(rideIndex);

	// Check if ride type supports data logging
	if (!(RCT2_GLOBAL(RCT2_ADDRESS_RIDE_FLAGS + (ride->type * 8), uint32) & 0x200)) {
		if (message != NULL) *message = STR_DATA_LOGGING_NOT_AVAILABLE_FOR_THIS_TYPE_OF_RIDE;
		return NULL;
	}

	// Check if a measurement already exists for this ride
	for (i = 0; i < MAX_RIDE_MEASUREMENTS; i++) {
		measurement = GET_RIDE_MEASUREMENT(i);
		if (measurement->ride_index == i)
			goto use_measurement;
	}

	// Find a free measurement
	for (i = 0; i < MAX_RIDE_MEASUREMENTS; i++) {
		measurement = GET_RIDE_MEASUREMENT(i);
		if (measurement->ride_index == 255)
			goto new_measurement;
	}

	// Use last recently used measurement for some other ride
	lruIndex = 0;
	lruTicks = 0xFFFFFFFF;
	for (i = 0; i < MAX_RIDE_MEASUREMENTS; i++) {
		measurement = GET_RIDE_MEASUREMENT(i);

		if (measurement->last_use_tick <= lruTicks) {
			lruTicks = measurement->last_use_tick;
			lruIndex = i;
		}
	}

	i = lruIndex;
	measurement = GET_RIDE_MEASUREMENT(i);
	ride->measurement_index = 255;

new_measurement:
	measurement->ride_index = rideIndex;
	ride->measurement_index = i;
	measurement->flags = 0;
	if (RCT2_GLOBAL(RCT2_ADDRESS_RIDE_FLAGS + (ride->type * 8), uint32) & 0x80)
		measurement->flags |= RIDE_MEASUREMENT_FLAG_G_FORCES;
	measurement->num_items = 0;
	measurement->current_item = 0;

use_measurement:
	measurement->last_use_tick = RCT2_GLOBAL(RCT2_ADDRESS_SCENARIO_TICKS, uint32);
	if (measurement->flags & 1) {
		if (message != NULL) *message = 0;
		return measurement;
	} else {
		RCT2_GLOBAL(0x013CE952, uint16) = RideNameConvention[ride->type].vehicle_name;
		RCT2_GLOBAL(0x013CE952 + 2, uint16) = RideNameConvention[ride->type].station_name;
		if (message != NULL) *message = STR_DATA_LOGGING_WILL_START_WHEN_NEXT_LEAVES;
		return NULL;
	}
}