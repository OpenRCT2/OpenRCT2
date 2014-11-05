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
#include "../input.h"
#include "../interface/window.h"
#include "../localisation/date.h"
#include "../localisation/localisation.h"
#include "../management/news_item.h"
#include "../peep/peep.h"
#include "../peep/staff.h"
#include "../scenario.h"
#include "../util/util.h"
#include "../windows/error.h"
#include "../world/map.h"
#include "../world/sprite.h"
#include "ride.h"
#include "ride_data.h"
#include "station.h"

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

// Static function declarations
rct_peep *find_closest_mechanic(int x, int y, int forInspection);
static void ride_breakdown_status_update(int rideIndex);
static void ride_breakdown_update(int rideIndex);
static void ride_call_closest_mechanic(int rideIndex);
static void ride_call_mechanic(int rideIndex, rct_peep *mechanic, int forInspection);
static void ride_chairlift_update(rct_ride *ride);
static void ride_entrance_exit_connected(rct_ride* ride, int ride_idx);
static int ride_get_new_breakdown_problem(rct_ride *ride);
static void ride_inspection_update(rct_ride *ride);
static void ride_mechanic_status_update(int rideIndex, int mechanicStatus);
static void ride_music_update(int rideIndex);
static void ride_prepare_breakdown(int rideIndex, int breakdownReason);
static void ride_shop_connected(rct_ride* ride, int ride_idx);
static void ride_spiral_slide_update(rct_ride *ride);
static void ride_update(int rideIndex);

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

#pragma region Initialisation functions

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
void reset_all_ride_build_dates()
{
	int i;
	rct_ride *ride;

	FOR_ALL_RIDES(i, ride)
		ride->build_date -= RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_MONTH_YEAR, uint16);
}

#pragma endregion

#pragma region Construction

static int ride_check_if_construction_allowed(rct_ride *ride)
{
	if (ride->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN) {
		RCT2_GLOBAL(0x013CE952 + 6, uint16) = ride->name;
		RCT2_GLOBAL(0x013CE952 + 8, uint32) = ride->name_arguments;
		window_error_open(STR_CANT_START_CONSTRUCTION_ON, STR_HAS_BROKEN_DOWN_AND_REQUIRES_FIXING);
		return 0;
	}

	if (ride->status != RIDE_STATUS_CLOSED) {
		RCT2_GLOBAL(0x013CE952 + 6, uint16) = ride->name;
		RCT2_GLOBAL(0x013CE952 + 8, uint32) = ride->name_arguments;
		window_error_open(STR_CANT_START_CONSTRUCTION_ON, STR_MUST_BE_CLOSED_FIRST);		
		return 0;
	}

	return 1;
}

static rct_window *ride_create_or_find_construction_window(int rideIndex)
{
	rct_window *w;

	w = window_find_by_class(WC_RIDE_CONSTRUCTION);
	if (w == NULL || w->number != rideIndex) {
		window_close_construction_windows();
		RCT2_GLOBAL(0x00F440A7, uint8) = rideIndex;
		w = window_construction_open(rideIndex);
	} else {
		RCT2_CALLPROC_X(0x006C9627, 0, 0, 0, 0, 0, 0, 0);
		RCT2_GLOBAL(0x00F440A7, uint8) = rideIndex;
	}

	return w;
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
 * rct2: 0x006C84CE
 */
static void sub_6C84CE()
{
	RCT2_CALLPROC_X(0x006C84CE, 0, 0, 0, 0, 0, 0, 0);
}

/**
 * 
 * rct2: 0x006DD4D5
 */
static void ride_remove_cable_lift(rct_ride *ride)
{
	uint16 spriteIndex;
	rct_vehicle *vehicle;

	if (ride->lifecycle_flags & RIDE_LIFECYCLE_CABLE_LIFT) {
		ride->lifecycle_flags &= ~RIDE_LIFECYCLE_CABLE_LIFT;
		spriteIndex = ride->cable_lift;
		do {
			vehicle = &(g_sprite_list[spriteIndex].vehicle);
			RCT2_CALLPROC_X(0x006EC473, 0, 0, 0, 0, (int)vehicle, 0, 0);
			RCT2_CALLPROC_X(0x0069EDB6, 0, 0, 0, 0, (int)vehicle, 0, 0);
			spriteIndex = vehicle->next_vehicle_on_train;
		} while (spriteIndex != SPRITE_INDEX_NULL);
	}
}

/**
 * 
 * rct2: 0x006DD506
 */
static void ride_remove_vehicles(rct_ride *ride)
{
	int i;
	uint16 spriteIndex;
	rct_vehicle *vehicle;

	if (ride->lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK) {
		ride->lifecycle_flags &= ~RIDE_LIFECYCLE_ON_TRACK;
		ride->lifecycle_flags &= ~(RIDE_LIFECYCLE_TEST_IN_PROGRESS | RIDE_LIFECYCLE_11);

		for (i = 0; i < 32; i++) {
			spriteIndex = ride->vehicles[i];
			while (spriteIndex != SPRITE_INDEX_NULL) {
				vehicle = &(g_sprite_list[spriteIndex].vehicle);
				RCT2_CALLPROC_X(0x006EC473, 0, 0, 0, 0, (int)vehicle, 0, 0);
				RCT2_CALLPROC_X(0x0069EDB6, 0, 0, 0, 0, (int)vehicle, 0, 0);
				spriteIndex = vehicle->next_vehicle_on_train;
			}

			ride->vehicles[i] = SPRITE_INDEX_NULL;
		}

		for (i = 0; i < 4; i++)
			ride->var_066[i] = 255;
	}
}

/**
 * 
 * rct2: 0x006DD4AC
 */
static void ride_clear_for_construction(int rideIndex)
{
	rct_ride *ride;
	rct_window *w;

	ride = GET_RIDE(rideIndex);

	ride_measurement_clear(ride);
	
	ride->lifecycle_flags &= ~(RIDE_LIFECYCLE_BREAKDOWN_PENDING | RIDE_LIFECYCLE_BROKEN_DOWN);
	ride->var_14D |= 0x0C;

	ride_remove_cable_lift(ride);
	ride_remove_vehicles(ride);

	w = window_find_by_number(WC_RIDE, rideIndex);
	if (w != NULL)
		window_event_helper(w, 0, WE_RESIZE);
}

/**
 * 
 * rct2: 0x006664DF
 */
static void ride_remove_peeps(int rideIndex)
{
	int i, stationIndex, x, y, z, exitX, exitY, exitZ, exitDirection;
	uint16 xy, spriteIndex;
	rct_ride *ride;
	rct_map_element *mapElement;
	rct_peep *peep;

	ride = GET_RIDE(rideIndex);

	// Find first station
	stationIndex = -1;
	for (i = 0; i < 4; i++) {
		if (ride->station_starts[i] != 0xFFFF) {
			stationIndex = i;
			break;
		}
	}

	// Get exit position and direction
	exitDirection = 255;
	if (stationIndex != -1) {
		xy = ride->exits[stationIndex];
		if (xy != 0xFFFF) {
			exitX = xy & 0xFF;
			exitY = xy >> 8;
			exitZ = ride->station_heights[stationIndex];
			mapElement = ride_get_station_exit_element(ride, exitX, exitY, exitZ);

			exitDirection = mapElement->type & 3;
			exitX = (exitX * 32) - (RCT2_ADDRESS(0x00981D6C, sint16)[exitDirection * 2] * 20) + 16;
			exitY = (exitY * 32) - (RCT2_ADDRESS(0x00981D6E, sint16)[exitDirection * 2] * 20) + 16;
			exitZ = (exitZ * 8) + 2;

			// Reverse direction
			exitDirection ^= 2;

			exitDirection *= 8;
		}
	}

	// Place all the peeps at exit
	FOR_ALL_PEEPS(spriteIndex, peep) {
		if (
			peep->state == PEEP_STATE_QUEUING_FRONT ||
			peep->state == PEEP_STATE_ENTERING_RIDE ||
			peep->state == PEEP_STATE_LEAVING_RIDE ||
			peep->state == PEEP_STATE_ON_RIDE
		) {
			if (peep->current_ride != rideIndex)
				continue;

			RCT2_CALLPROC_X(0x0069A409, 0, 0, 0, 0, (int)peep, 0, 0);
			if (peep->state == PEEP_STATE_QUEUING_FRONT && peep->var_2C == 0)
				RCT2_CALLPROC_X(0x006966A9, 0, 0, 0, 0, (int)peep, 0, 0);

			RCT2_CALLPROC_X(0x006EC473, 0, 0, 0, 0, (int)peep, 0, 0);

			if (exitDirection == 255) {
				x = peep->next_x + 16;
				y = peep->next_y + 16;
				z = (peep->next_z & 0xFF) * 8;
				if ((peep->next_z >> 8) & 4)
					z += 8;
				z++;
				RCT2_CALLPROC_X(0x0069E9D3, exitX, 0, exitY, exitZ, (int)peep, 0, 0);
			} else {
				RCT2_CALLPROC_X(0x0069E9D3, exitX, 0, exitY, exitZ, (int)peep, 0, 0);
				peep->sprite_direction = exitDirection;
			}

			RCT2_CALLPROC_X(0x006EC473, 0, 0, 0, 0, (int)peep, 0, 0);
			peep->state = PEEP_STATE_0;
			RCT2_CALLPROC_X(0x00693BE5, 0, 0, 0, 0, (int)peep, 0, 0);
			
			peep->happiness = min(peep->happiness, peep->happiness_growth_rate) / 2;
			peep->happiness_growth_rate = peep->happiness;
			peep->var_45 |= 2;
		}
	}

	ride->num_riders = 0;
	ride->var_15D = 0;
	ride->var_14D |= 4;
}

void sub_6C683D(int x, int y, int z, int direction, int type, int esi, int edi, int ebp)
{
	RCT2_CALLPROC_X(0x006C683D, x, (direction << 8) | type, y, z, esi, edi, ebp);
}

void sub_6CE254()
{
	RCT2_CALLPROC_X(0x006CE254, 0, 0, 0, 0, 0, 0, 0);
}


void sub_6C96C0()
{
	RCT2_CALLPROC_X(0x006C96C0, 0, 0, 0, 0, 0, 0, 0);
}

static void sub_6C9627()
{
	switch (RCT2_GLOBAL(0x00F440A6, uint8)) {
	case 3:
		sub_6C683D(
			RCT2_GLOBAL(0x00F440A8, uint16),
			RCT2_GLOBAL(0x00F440AA, uint16),
			RCT2_GLOBAL(0x00F440AC, uint16),
			RCT2_GLOBAL(0x00F440AE, uint8) & 3,
			RCT2_GLOBAL(0x00F440AF, uint8),
			0,
			0,
			1
		);
		break;
	case 6:
	case 7:
	case 8:
		sub_6CE254();
		break;
	default:
		if (RCT2_GLOBAL(0x00F440B0, uint8) & 1) {
			RCT2_GLOBAL(0x00F440B0, uint8) &= ~1;
			RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint8) &= ~4;
			map_invalidate_tile_full(RCT2_GLOBAL(0x00F440A8, uint16), RCT2_GLOBAL(0x00F440AA, uint16));
		}
		sub_6C96C0();
		break;
	}
}

/**
 * 
 * rct2: 0x006C9296
 */
static void sub_6C9296()
{
	RCT2_CALLPROC_X(0x006C9296, 0, 0, 0, 0, 0, 0, 0);
}

/**
 * 
 * rct2: 0x006C93B8
 */
static void sub_6C93B8()
{
	RCT2_CALLPROC_X(0x006C93B8, 0, 0, 0, 0, 0, 0, 0);
}

/**
 * 
 * rct2: 0x006CC2CA
 */
static int ride_modify_entrance_or_exit(rct_map_element *mapElement, int x, int y)
{
	int rideIndex, entranceType;
	rct_window *constructionWindow;

	rideIndex = mapElement->properties.entrance.ride_index;

	entranceType = mapElement->properties.entrance.type;
	if (entranceType != ENTRANCE_TYPE_RIDE_ENTRANCE && entranceType != ENTRANCE_TYPE_RIDE_EXIT)
		return 0;

	int bl = (mapElement->properties.entrance.index & 0x70) >> 4;

	// Get or create construction window for ride
	constructionWindow = window_find_by_class(WC_RIDE_CONSTRUCTION);
	if (constructionWindow == NULL) {
		if (!sub_6CC3FB(rideIndex))
			return 0;

		constructionWindow = window_find_by_class(WC_RIDE_CONSTRUCTION);
		if (constructionWindow == NULL)
			return 0;
	}

	sub_6C9627();
	if (
		RCT2_GLOBAL(0x00F440A6, uint8) != 5 ||
		!(RCT2_GLOBAL(RCT2_ADDRESS_INPUT_FLAGS, uint8) & INPUT_FLAG_TOOL_ACTIVE) ||
		RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WINDOWCLASS, rct_windowclass) != WC_RIDE_CONSTRUCTION
	) {
		// Replace entrance / exit
		tool_set(constructionWindow, entranceType == 0 ? 29 : 30, 12);
		RCT2_GLOBAL(0x00F44191, uint8) = entranceType;
		RCT2_GLOBAL(0x00F44192, uint8) = rideIndex;
		RCT2_GLOBAL(0x00F44193, uint8) = bl;
		RCT2_GLOBAL(RCT2_ADDRESS_INPUT_FLAGS, uint8) |= INPUT_FLAG_6;
		int al = RCT2_GLOBAL(0x00F440A6, uint8);
		if (al != 5) {
			RCT2_GLOBAL(0x00F440A6, uint8) = 5;
			RCT2_GLOBAL(0x00F440CC, uint8) = al;
		}

		sub_6C84CE();
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) &= ~2;
	} else {
		// Remove entrance / exit
		game_do_command(x, 9, y, rideIndex, GAME_COMMAND_13, bl, 0);
		RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WIDGETINDEX, uint16) = entranceType == ENTRANCE_TYPE_RIDE_ENTRANCE ? 29 : 30;
		RCT2_GLOBAL(0x00F44191, uint8) = entranceType;
	}
		
	window_invalidate_by_class(WC_RIDE_CONSTRUCTION);
	return 1;
}

/**
 * 
 * rct2: 0x006CC287
 */
int ride_modify_maze(rct_map_element *mapElement, int x, int y)
{
	RCT2_GLOBAL(0x00F440A7, uint8) = mapElement->properties.track.ride_index;
	RCT2_GLOBAL(0x00F440A6, uint8) = 6;
	RCT2_GLOBAL(0x00F440A8, uint16) = x;
	RCT2_GLOBAL(0x00F440AA, uint16) = y;
	RCT2_GLOBAL(0x00F440AC, uint16) = mapElement->base_height * 8;
	RCT2_GLOBAL(0x00F440B0, uint8) = 0;
	RCT2_GLOBAL(0x00F440B1, uint8) = 0;
	RCT2_CALLPROC_X(0x006CD887, 0, 0, 0, 0, 0, 0, 0);
	return 1;
}

/**
 * 
 * rct2: 0x006CC056
 */
int ride_modify(rct_map_element *mapElement, int x, int y)
{
	int rideIndex, z, direction, type;
	rct_ride *ride;
	rct_window *constructionWindow;

	rideIndex = mapElement->properties.track.ride_index;
	ride = GET_RIDE(rideIndex);

	if (!ride_check_if_construction_allowed(ride))
		return 0;

	if (ride->lifecycle_flags & RIDE_LIFECYCLE_INDESTRUCTIBLE) {
		RCT2_GLOBAL(0x013CE952 + 6, uint16) = ride->name;
		RCT2_GLOBAL(0x013CE952 + 8, uint32) = ride->name_arguments;
		window_error_open(STR_CANT_START_CONSTRUCTION_ON, STR_LOCAL_AUTHORITY_FORBIDS_DEMOLITION_OR_MODIFICATIONS_TO_THIS_RIDE);
		return 0;
	}

	if (ride->lifecycle_flags & RIDE_LIFECYCLE_19) {
		RCT2_GLOBAL(0x013CE952 + 6, uint16) = ride->name;
		RCT2_GLOBAL(0x013CE952 + 8, uint32) = ride->name_arguments;
		window_error_open(STR_CANT_START_CONSTRUCTION_ON, STR_THIS_RIDE_CANNOT_BE_MODIFIED);
		return 0;
	}

	ride_clear_for_construction(rideIndex);
	ride_remove_peeps(rideIndex);

	// Check if element is a station entrance or exit
	if ((mapElement->type & MAP_ELEMENT_TYPE_MASK) == MAP_ELEMENT_TYPE_ENTRANCE)
		return ride_modify_entrance_or_exit(mapElement, x, y);

	constructionWindow = ride_create_or_find_construction_window(rideIndex);

	if (ride->type == RIDE_TYPE_MAZE)
		return ride_modify_maze(mapElement, x, y);

	if (RCT2_GLOBAL(0x0097CF40 + (ride->type * 8), uint32) & 0x100) {
		int outX, outY;
		mapElement = ride_find_track_gap(mapElement, &outX, &outY);
	}

	z = mapElement->base_height * 8;
	direction = mapElement->type & 3;
	type = mapElement->properties.track.type;
	sub_6C683D(x, y, z, direction, type, 0, 0, 0);

	RCT2_GLOBAL(0x00F440A7, uint8) = rideIndex;
	RCT2_GLOBAL(0x00F440A6, uint8) = 3;
	RCT2_GLOBAL(0x00F440A8, uint16) = x;
	RCT2_GLOBAL(0x00F440AA, uint16) = y;
	RCT2_GLOBAL(0x00F440AC, uint16) = z;
	RCT2_GLOBAL(0x00F440AE, uint8) = direction;
	RCT2_GLOBAL(0x00F440AF, uint8) = type;
	RCT2_GLOBAL(0x00F440B0, uint8) = 0;
	RCT2_GLOBAL(0x00F440B1, uint8) = 0;

	if (RCT2_GLOBAL(0x0097CF40 + (ride->type * 8), uint32) & 0x8000) {
		sub_6C84CE();
		return 1;
	}

	sub_6C9296();
	if (RCT2_GLOBAL(0x00F440A6, uint8) == 1) {
		sub_6C84CE();
		return 1;
	}

	RCT2_GLOBAL(0x00F440A6, uint8) = 3;
	RCT2_GLOBAL(0x00F440A8, uint16) = x;
	RCT2_GLOBAL(0x00F440AA, uint16) = y;
	RCT2_GLOBAL(0x00F440AC, uint16) = z;
	RCT2_GLOBAL(0x00F440AE, uint8) = direction;
	RCT2_GLOBAL(0x00F440AF, uint8) = type;
	RCT2_GLOBAL(0x00F440B0, uint8) = 0;
	RCT2_GLOBAL(0x00F440B1, uint8) = 0;

	sub_6C93B8();

	if (RCT2_GLOBAL(0x00F440A6, uint8) != 2) {
		RCT2_GLOBAL(0x00F440A6, uint8) = 3;
		RCT2_GLOBAL(0x00F440A8, uint16) = x;
		RCT2_GLOBAL(0x00F440AA, uint16) = y;
		RCT2_GLOBAL(0x00F440AC, uint16) = z;
		RCT2_GLOBAL(0x00F440AE, uint8) = direction;
		RCT2_GLOBAL(0x00F440AF, uint8) = type;
		RCT2_GLOBAL(0x00F440B0, uint8) = 0;
		RCT2_GLOBAL(0x00F440B1, uint8) = 0;
	}

	sub_6C84CE();
	return 1;

	// Success stored in carry flag which can't be accessed after call using is macro
	// RCT2_CALLPROC_X(0x006CC056, 0, 0, 0, (int)trackMapElement, 0, 0, 0);
	// return 1;
}

/**
 * 
 * rct2: 0x006CC3FB
 */
int sub_6CC3FB(int rideIndex)
{
	rct_ride *ride;
	rct_window *w;

	tool_cancel();
	ride = GET_RIDE(rideIndex);

	if (!ride_check_if_construction_allowed(ride))
		return 0;

	ride_clear_for_construction(rideIndex);
	ride_remove_peeps(rideIndex);

	w = ride_create_or_find_construction_window(rideIndex);

	tool_set(w, 23, 12);
	RCT2_GLOBAL(RCT2_ADDRESS_INPUT_FLAGS, uint32) |= INPUT_FLAG_6;

	ride = GET_RIDE(RCT2_GLOBAL(0x00F440A7, uint8));

	RCT2_GLOBAL(0x00F440A0, uint16) = RCT2_ADDRESS(0x0097CC68, uint8)[ride->type * 2] | 0x100;
	RCT2_GLOBAL(0x00F440B2, uint8) = 0;
	RCT2_GLOBAL(0x00F440B3, uint8) = 0;
	RCT2_GLOBAL(0x00F440B4, uint8) = 0;
	RCT2_GLOBAL(0x00F440B5, uint8) = 0;

	if (RCT2_GLOBAL(0x0097D4F2 + (ride->type * 8), uint16) & 0x8000)
		RCT2_GLOBAL(0x00F440B5, uint8) |= 2;

	RCT2_GLOBAL(0x00F440B6, uint8) = 0;
	RCT2_GLOBAL(0x00F440B7, uint8) = 0;

	RCT2_GLOBAL(0x00F440AE, uint8) = 0;
	RCT2_GLOBAL(0x00F440A6, uint8) = 4;
	RCT2_GLOBAL(0x00F440B0, uint8) = 0;
	RCT2_GLOBAL(0x00F440B1, uint8) = 0;
	RCT2_GLOBAL(0x00F44159, uint8) = 0;
	RCT2_GLOBAL(0x00F4415C, uint8) = 0;

	sub_6C84CE();
	return 1;
}

#pragma endregion

#pragma region Update functions

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

	window_update_viewport_ride_music();

	// Update rides
	FOR_ALL_RIDES(i, ride)
		ride_update(i);

	sub_6BC6D8();
}

/**
 *
 *  rct2: 0x006ABE73
 */
static void ride_update(int rideIndex)
{
	int i;
	rct_ride *ride = GET_RIDE(rideIndex);
	
	if (ride->var_1CA != 0)
		ride->var_1CA--;

	ride_music_update(rideIndex);

	// Update stations
	if (ride->type != RIDE_TYPE_MAZE)
		for (i = 0; i < 4; i++)
			ride_update_station(ride, i);

	// Update financial statistics
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
		ride->var_120 = 0;
		ride->var_14D |= 1;

		ride->income_per_hour = ride_calculate_income_per_hour(ride);
		ride->var_14D |= 2;

		if (ride->upkeep_cost != (money16)0xFFFF)
			ride->profit = (money16)ride->income_per_hour - (ride->upkeep_cost * 16);
	}

	// Ride specific updates
	if (ride->type == RIDE_TYPE_CHAIRLIFT)
		ride_chairlift_update(ride);
	else if (ride->type == RIDE_TYPE_SPIRAL_SLIDE)
		ride_spiral_slide_update(ride);

	ride_breakdown_update(rideIndex);

	// Various things include news messages
	if (ride->lifecycle_flags & (RIDE_LIFECYCLE_BREAKDOWN_PENDING | RIDE_LIFECYCLE_BROKEN_DOWN | RIDE_LIFECYCLE_DUE_INSPECTION))
		if (((RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_TICKS, uint32) >> 1) & 255) == rideIndex)
			ride_breakdown_status_update(rideIndex);

	ride_inspection_update(ride);
}

/**
 *
 *  rct2: 0x006AC489
 */
static void ride_chairlift_update(rct_ride *ride)
{
	int x, y, z, ax, bx, cx;

	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK))
		return;
	if (!(ride->lifecycle_flags & (RIDE_LIFECYCLE_BREAKDOWN_PENDING | RIDE_LIFECYCLE_BROKEN_DOWN | RIDE_LIFECYCLE_CRASHED)))
		return;
	if (ride->breakdown_reason_pending == 0)
		return;

	ax = ride->var_0D0 * 2048;
	bx = ride->var_148;
	cx = bx + ax;
	ride->var_148 = cx;
	if (bx >> 14 == cx >> 14)
		return;

	x = (ride->var_13A & 0xFF) * 32;
	y = (ride->var_13A >> 8) * 32;
	z = ride->var_13E * 8;
	map_invalidate_tile(x, y, z, z + (4 * 8));

	x = (ride->var_13C & 0xFF) * 32;
	y = (ride->var_13C >> 8) * 32;
	z = ride->var_13F * 8;
	map_invalidate_tile(x, y, z, z + (4 * 8));
}

/**
 *
 *  rct2: 0x006AC545
 */
static void ride_spiral_slide_update(rct_ride *ride)
{
	int i, x, y, z;
	rct_map_element *mapElement;
	rct_peep *peep;

	if (RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_TICKS, uint32) & 3)
		return;
	if (ride->var_15D == 0)
		return;

	ride->var_176++;
	if (ride->var_176 >= 48) {
		ride->var_15D--;

		peep = &(g_sprite_list[ride->maze_tiles].peep);
		peep->var_32++;
	}

	// Invalidate something related to station start
	for (i = 0; i < 4; i++) {
		if (ride->station_starts[i] == 0xFFFF)
			continue;

		x = ride->station_starts[i] & 0xFF;
		y = ride->station_starts[i] >> 8;
		z = ride->station_heights[i];

		mapElement = ride_get_station_start_track_element(ride, i);
		int rotation = ((mapElement->type & 3) << 2) | RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_ROTATION, uint8);
		x += RCT2_GLOBAL(0x0098DDB8 + (rotation * 4), uint16);
		y += RCT2_GLOBAL(0x0098DDBA + (rotation * 4), uint16);

		map_invalidate_tile(x, y, mapElement->base_height * 8, mapElement->clearance_height * 8);
	}
}

#pragma endregion

#pragma region Breakdown and inspection functions

static uint8 _breakdownProblemProbabilities[] = {
	25,		// BREAKDOWN_SAFETY_CUT_OUT
	12,		// BREAKDOWN_RESTRAINTS_STUCK_CLOSED
	10,		// BREAKDOWN_RESTRAINTS_STUCK_OPEN
	13,		// BREAKDOWN_DOORS_STUCK_CLOSED
	10,		// BREAKDOWN_DOORS_STUCK_OPEN
	6,		// BREAKDOWN_VEHICLE_MALFUNCTION
	0,		// BREAKDOWN_BRAKES_FAILURE
	3		// BREAKDOWN_CONTROL_FAILURE
};

/**
 *
 *  rct2: 0x006AC7C2
 */
static void ride_inspection_update(rct_ride *ride)
{
	int i;

	if (RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_TICKS, uint32) & 2047)
		return;
	if (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_TRACK_DESIGNER)
		return;

	ride->last_inspection++;
	if (ride->last_inspection == 0)
		ride->last_inspection--;

	int inspectionIntervalMinutes = RideInspectionInterval[ride->inspection_interval];
	if (inspectionIntervalMinutes == 0)
		return;

	if (RCT2_ADDRESS(0x0097C740, uint32)[ride->type] == 0)
		return;

	if (inspectionIntervalMinutes > ride->last_inspection)
		return;

	if (ride->lifecycle_flags & (RIDE_LIFECYCLE_BREAKDOWN_PENDING | RIDE_LIFECYCLE_BROKEN_DOWN | RIDE_LIFECYCLE_DUE_INSPECTION | RIDE_LIFECYCLE_CRASHED))
		return;

	// Inspect the first station that has an exit
	ride->lifecycle_flags |= RIDE_LIFECYCLE_DUE_INSPECTION;
	ride->mechanic_status = RIDE_MECHANIC_STATUS_CALLING;
	ride->inspection_station = 0;
	for (i = 0; i < 4; i++) {
		if (ride->exits[i] != 0xFFFF) {
			ride->inspection_station = i;
			break;
		}
	}
}

/**
 *
 *  rct2: 0x006AC622
 */
static void ride_breakdown_update(int rideIndex)
{
	int agePenalty, years, ax, breakdownReason;
	rct_ride *ride = GET_RIDE(rideIndex);

	if (RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_TICKS, uint32) & 255)
		return;
	if (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_TRACK_DESIGNER)
		return;
	
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

	if (ride->lifecycle_flags & (RIDE_LIFECYCLE_BREAKDOWN_PENDING | RIDE_LIFECYCLE_BROKEN_DOWN | RIDE_LIFECYCLE_CRASHED))
		return;
	if (ride->status == RIDE_STATUS_CLOSED)
		return;

	// Calculate breakdown probability?
	ax = ride->var_198;
	agePenalty;
	years = date_get_year(RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_MONTH_YEAR, uint16) - ride->build_date);
	switch (years) {
	case 0:
		agePenalty = 0;
		break;
	case 1:
		agePenalty = ax >> 3;
		break;
	case 2:
		agePenalty = ax >> 2;
		break;
	case 3:
		agePenalty = ax >> 1;
		break;
	case 4:
	case 5:
	case 6:
		agePenalty = ax >> 0;
		break;
	default:
		agePenalty = ax << 1;
		break;
	}
	ax += agePenalty;
	ride->var_196 = max(0, ride->var_196 - ax);
	ride->var_14D |= 32;

	// Random probability of a breakdown
	if (ride->var_196 == 0 || (int)(scenario_rand() & 0x2FFFFF) <= 25856 - ride->var_196) {
		breakdownReason = ride_get_new_breakdown_problem(ride);
		if (breakdownReason != -1)
			ride_prepare_breakdown(rideIndex, breakdownReason);
	}
}

/**
 *
 *  rct2: 0x006B7294
 */
static int ride_get_new_breakdown_problem(rct_ride *ride)
{
	int availableBreakdownProblems, monthsOld, totalProbability, randomProbability, problemBits, breakdownProblem;
	rct_ride_type *entry;

	// Brake failure is more likely when its raining
	_breakdownProblemProbabilities[BREAKDOWN_BRAKES_FAILURE] =
		RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_RAIN_LEVEL, uint8) == 0 ? 3 : 20;

	entry = ride_get_entry(ride);
	if (entry->var_008 & 0x4000)
		return -1;
	
	availableBreakdownProblems = RideAvailableBreakdowns[ride->type];

	// Calculate the total probability range for all possible breakdown problems
	totalProbability = 0;
	problemBits = availableBreakdownProblems;
	while (problemBits != 0) {
		breakdownProblem = bitscanforward(problemBits);
		problemBits &= ~(1 << breakdownProblem);
		totalProbability += _breakdownProblemProbabilities[breakdownProblem];
	}
	if (totalProbability == 0)
		return -1;

	// Choose a random number within this range
	randomProbability = scenario_rand() % totalProbability;

	// Find which problem range the random number lies
	problemBits = availableBreakdownProblems;
	do {
		breakdownProblem = bitscanforward(problemBits);
		problemBits &= ~(1 << breakdownProblem);
		randomProbability -= _breakdownProblemProbabilities[breakdownProblem];
	} while (randomProbability >= 0);

	if (breakdownProblem != BREAKDOWN_BRAKES_FAILURE)
		return breakdownProblem;

	// Breaks failure can not happen if block breaks are used (so long as there is more than one vehicle)
	// However if this is the case, break failure should be taken out the equation, otherwise block brake
	// rides have a lower probability to break down due to a random implementation reason.
	if (ride->mode == RIDE_MODE_CONTINUOUS_CIRCUIT_BLOCK_SECTIONED || ride->mode == RIDE_MODE_POWERED_LAUNCH_BLOCK_SECTIONED)
		if (ride->num_vehicles != 1)
			return -1;

	// Again the probability is lower, this time if young or two other unknown reasons...
	monthsOld = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_MONTH_YEAR, uint8) - ride->build_date;
	if (monthsOld < 16 || ride->var_196 > 12800 || ride->lifecycle_flags & RIDE_LIFECYCLE_19)
		return -1;

	return BREAKDOWN_BRAKES_FAILURE;
}

/**
 *
 *  rct2: 0x006B7348
 */
static void ride_prepare_breakdown(int rideIndex, int breakdownReason)
{
	int i;
	rct_ride *ride;
	rct_vehicle *vehicle;

	ride = GET_RIDE(rideIndex);
	if (ride->lifecycle_flags & (RIDE_LIFECYCLE_BREAKDOWN_PENDING | RIDE_LIFECYCLE_BROKEN_DOWN | RIDE_LIFECYCLE_CRASHED))
		return;

	ride->lifecycle_flags &= ~RIDE_LIFECYCLE_DUE_INSPECTION;
	ride->lifecycle_flags |= RIDE_LIFECYCLE_BREAKDOWN_PENDING;

	ride->breakdown_reason_pending = breakdownReason;
	ride->mechanic_status = RIDE_MECHANIC_STATUS_UNDEFINED;
	ride->var_1AC = 0;
	ride->var_1AD = 0;
	
	switch (breakdownReason) {
	case BREAKDOWN_SAFETY_CUT_OUT:
	case BREAKDOWN_CONTROL_FAILURE:
		// Inspect first station with an exit
		for (i = 0; i < 4; i++) {
			if (ride->exits[i] != 0xFFFF) {
				ride->inspection_station = i;
				break;
			}
		}
		break;
	case BREAKDOWN_RESTRAINTS_STUCK_CLOSED:
	case BREAKDOWN_RESTRAINTS_STUCK_OPEN:
	case BREAKDOWN_DOORS_STUCK_CLOSED:
	case BREAKDOWN_DOORS_STUCK_OPEN:
		// Choose a random train and car
		ride->broken_vehicle = scenario_rand() % ride->num_vehicles;
		ride->broken_car = scenario_rand() % ride->num_cars_per_train;

		// Set flag on broken car
		vehicle = &(g_sprite_list[ride->vehicles[ride->broken_vehicle]].vehicle);
		for (i = ride->broken_car; i > 0; i--)
			vehicle = &(g_sprite_list[vehicle->next_vehicle_on_train].vehicle);
		vehicle->var_48 |= 0x100;
		break;
	case BREAKDOWN_VEHICLE_MALFUNCTION:
		// Choose a random train
		ride->broken_vehicle = scenario_rand() % ride->num_vehicles;
		ride->broken_car = 0;

		// Set flag on broken train, first car
		vehicle = &(g_sprite_list[ride->vehicles[ride->broken_vehicle]].vehicle);
		vehicle->var_48 |= 0x200;
		break;
	case BREAKDOWN_BRAKES_FAILURE:
		// Original code generates a random number but does not use it
		// Unsure if this was supposed to choose a random station (or random station with an exit)
		for (i = 0; i < 4; i++) {
			ride->inspection_station = i;
			if (ride->exits[i] != 0xFFFF)
				break;
		}
		break;
	}
}

/**
 *
 *  rct2: 0x006B74FA
 */
void ride_breakdown_add_news_item(int rideIndex)
{
	rct_ride *ride = GET_RIDE(rideIndex);

	RCT2_GLOBAL(0x0013CE952 + 0, uint16) = ride->name;
	RCT2_GLOBAL(0x0013CE952 + 2, uint32) = ride->name_arguments;
	news_item_add_to_queue(NEWS_ITEM_RIDE, 1927, rideIndex);
}

/**
 *
 *  rct2: 0x006B75C8
 */
static void ride_breakdown_status_update(int rideIndex)
{
	rct_ride *ride = GET_RIDE(rideIndex);

	// Warn player if ride hasnt been fixed for ages
	if (ride->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN) {
		ride->var_1AD++;
		if (ride->var_1AD == 0)
			ride->var_1AD -= 16;

		if (
			!(ride->var_1AD & 15) &&
			ride->mechanic_status != RIDE_MECHANIC_STATUS_FIXING &&
			ride->mechanic_status != RIDE_MECHANIC_STATUS_4
		) {
			RCT2_GLOBAL(0x0013CE952 + 0, uint16) = ride->name;
			RCT2_GLOBAL(0x0013CE952 + 2, uint32) = ride->name_arguments;
			news_item_add_to_queue(NEWS_ITEM_RIDE, 1929, rideIndex);
		}
	}

	ride_mechanic_status_update(rideIndex, ride->mechanic_status);
}

/**
 *
 *  rct2: 0x006B762F
 */
static void ride_mechanic_status_update(int rideIndex, int mechanicStatus)
{
	int breakdownReason;
	rct_ride *ride;
	rct_peep *mechanic;
	
	ride = GET_RIDE(rideIndex);
	switch (mechanicStatus) {
	case RIDE_MECHANIC_STATUS_UNDEFINED:
		breakdownReason = ride->breakdown_reason_pending;
		if (
			breakdownReason == BREAKDOWN_SAFETY_CUT_OUT ||
			breakdownReason == BREAKDOWN_BRAKES_FAILURE ||
			breakdownReason == BREAKDOWN_CONTROL_FAILURE
		) {
			ride->lifecycle_flags |= RIDE_LIFECYCLE_BROKEN_DOWN;
			ride->var_14D |= 0x2C;
			ride->mechanic_status = RIDE_MECHANIC_STATUS_CALLING;
			ride->breakdown_reason = breakdownReason;
			ride_breakdown_add_news_item(rideIndex);
		}
		break;
	case RIDE_MECHANIC_STATUS_CALLING:
		if (RideAvailableBreakdowns[ride->type] == 0) {
			ride->lifecycle_flags &= ~(RIDE_LIFECYCLE_BREAKDOWN_PENDING | RIDE_LIFECYCLE_BROKEN_DOWN | RIDE_LIFECYCLE_DUE_INSPECTION);
			break;
		}

		ride_call_closest_mechanic(rideIndex);
		break;
	case RIDE_MECHANIC_STATUS_HEADING:
		mechanic = &(g_sprite_list[ride->mechanic].peep);
		if (
			!peep_is_mechanic(mechanic) ||
			(mechanic->state != PEEP_STATE_HEADING_TO_INSPECTION && mechanic->state != PEEP_STATE_ANSWERING) ||
			mechanic->current_ride != rideIndex
		) {
			ride->mechanic_status = RIDE_MECHANIC_STATUS_CALLING;
			ride->var_14D |= 0x20;
			ride_mechanic_status_update(rideIndex, RIDE_MECHANIC_STATUS_CALLING);
		}
		break;
	case RIDE_MECHANIC_STATUS_FIXING:
		mechanic = &(g_sprite_list[ride->mechanic].peep);
		if (
			!peep_is_mechanic(mechanic) ||
			(
				mechanic->state != PEEP_STATE_HEADING_TO_INSPECTION &&
				mechanic->state != PEEP_STATE_FIXING &&
				mechanic->state != PEEP_STATE_INSPECTING &&
				mechanic->state != PEEP_STATE_ANSWERING
			)
		) {
			ride->mechanic_status = RIDE_MECHANIC_STATUS_CALLING;
			ride->var_14D |= 0x20;
			ride_mechanic_status_update(rideIndex, RIDE_MECHANIC_STATUS_CALLING);
		}
		break;
	}
}

/**
 *
 *  rct2: 0x006B796C
 */
static void ride_call_mechanic(int rideIndex, rct_peep *mechanic, int forInspection)
{
	rct_ride *ride;

	ride = GET_RIDE(rideIndex);
	RCT2_CALLPROC_X(0x0069A409, 0, 0, 0, 0, (int)mechanic, 0, 0);
	mechanic->state = forInspection ? PEEP_STATE_HEADING_TO_INSPECTION : PEEP_STATE_ANSWERING;
	RCT2_CALLPROC_X(0x0069A42F, 0, 0, 0, 0, (int)mechanic, 0, 0);
	mechanic->var_2C = 0;
	ride->mechanic_status = RIDE_MECHANIC_STATUS_HEADING;
	ride->var_14D |= 0x20;
	ride->mechanic = mechanic->sprite_index;
	mechanic->current_ride = rideIndex;
	mechanic->current_ride_station = ride->inspection_station;
}

/**
 *
 *  rct2: 0x006B76AB
 */
static void ride_call_closest_mechanic(int rideIndex)
{
	rct_ride *ride;
	rct_peep *mechanic;
	int forInspection;

	ride = GET_RIDE(rideIndex);
	forInspection = (ride->lifecycle_flags & (RIDE_LIFECYCLE_BREAKDOWN_PENDING | RIDE_LIFECYCLE_BROKEN_DOWN)) == 0;

	mechanic = ride_find_closest_mechanic(ride, forInspection);
	if (mechanic != NULL)
		ride_call_mechanic(rideIndex, mechanic, forInspection);
}

rct_peep *ride_find_closest_mechanic(rct_ride *ride, int forInspection)
{
	int x, y, z, stationIndex, direction;
	uint16 xy;
	rct_map_element *mapElement;
	
	// Get either exit position or entrance position if there is no exit
	stationIndex = ride->inspection_station;
	xy = ride->exits[stationIndex];
	if (xy == 0xFFFF) {
		xy = ride->entrances[stationIndex];
		if (xy == 0xFFFF)
			return NULL;
	}

	// Get station start track element and position
	x = xy & 0xFF;
	y = xy >> 8;
	z = ride->station_heights[stationIndex];
	mapElement = ride_get_station_exit_element(ride, x, y, z);
	if (mapElement == NULL)
		return NULL;

	x *= 32;
	y *= 32;
	direction = mapElement->type & 3;
	x -= RCT2_ADDRESS(0x00993CCC, sint16)[direction * 2];
	y -= RCT2_ADDRESS(0x00993CCE, sint16)[direction * 2];
	x += 16;
	y += 16;

	return find_closest_mechanic(x, y, forInspection);
}

/**
 *
 *  rct2: 0x006B774B (forInspection = 0)
 *  rct2: 0x006B78C3 (forInspection = 1)
 */
rct_peep *find_closest_mechanic(int x, int y, int forInspection)
{
	unsigned int closestDistance, distance;
	uint16 spriteIndex;
	rct_peep *peep, *closestMechanic;
		
	closestDistance = -1;
	FOR_ALL_STAFF(spriteIndex, peep) {
		if (peep->staff_type != STAFF_TYPE_MECHANIC)
			continue;

		if (forInspection) {
			if ((peep->state != PEEP_STATE_HEADING_TO_INSPECTION || peep->var_2C >= 4) && peep->state != PEEP_STATE_PATROLLING)
				continue;

			if (!(peep->staff_orders & 2))
				continue;
		} else {
			if (peep->state != PEEP_STATE_PATROLLING && !(peep->staff_orders & 1))
				continue;
		}

		if (map_is_location_in_park(x, y))
			if (!mechanic_is_location_in_patrol(peep, x & 0xFFE0, y & 0xFFE0))
				continue;
		
		if (peep->x == (sint16)0x8000)
			continue;

		// Should probably be euclidean or manhattan distance, this seems a bit naive
		distance = max(abs(peep->x - x), abs(peep->y - y));
		if (distance < closestDistance) {
			closestDistance = distance;
			closestMechanic = peep;
		}
	}

	return closestDistance == -1 ? NULL : closestMechanic;
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

#pragma endregion

#pragma region Music functions

/**
 *
 *  rct2: 0x006ABE85
 */
static void ride_music_update(int rideIndex)
{
	int x, y, z;
	rct_vehicle *vehicle;
	rct_ride *ride = GET_RIDE(rideIndex);

	if (!(RCT2_GLOBAL(0x0097D4F2 + (ride->type * 8), uint16) & 6))
		return;

	if (ride->status != RIDE_STATUS_OPEN || !(ride->lifecycle_flags & RIDE_LIFECYCLE_MUSIC)) {
		ride->music_tune_id = 255;
		return;
	}

	if (ride->type == RIDE_TYPE_CIRCUS_SHOW) {
		vehicle = &(g_sprite_list[ride->vehicles[0]].vehicle);
		if (vehicle->status != VEHICLE_STATUS_DOING_CIRCUS_SHOW) {
			ride->music_tune_id = 255;
			return;
		}
	}

	// Oscillate parameters for a power cut effect when breaking down 
	if (ride->lifecycle_flags & (RIDE_LIFECYCLE_BREAKDOWN_PENDING | RIDE_LIFECYCLE_BROKEN_DOWN)) {
		if (ride->breakdown_reason_pending == BREAKDOWN_CONTROL_FAILURE) {
			if (!(RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_TICKS, uint32) & 7))
				if (ride->var_1AC != 255)
					ride->var_1AC++;
		} else {
			if (
				(ride->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN) ||
				ride->breakdown_reason_pending == BREAKDOWN_BRAKES_FAILURE ||
				ride->breakdown_reason_pending == BREAKDOWN_CONTROL_FAILURE
			) {
				if (ride->var_1AC != 255)
					ride->var_1AC++;
			}

			if (ride->var_1AC == 255) {
				ride->music_tune_id = 255;
				return;
			}
		}
	}

	// Select random tune from available tunes for a music style (of course only merry-go-rounds have more than one tune)
	if (ride->music_tune_id == 255) {
		uint8 *musicStyleTunes = RCT2_ADDRESS(0x009AEF28, uint8*)[ride->music];
		uint8 numTunes = *musicStyleTunes++;
		ride->music_tune_id = musicStyleTunes[scenario_rand() % numTunes];
		ride->music_position = 0;
		return;
	}

	x = (ride->station_starts[0] & 0xFF) * 32 + 16;
	y = (ride->station_starts[0] >> 8) * 32 + 16;
	z = ride->station_heights[0] * 8;

	int sampleRate = 22050;

	// Alter sample rate for a power cut effect
	if (ride->lifecycle_flags & (RIDE_LIFECYCLE_BREAKDOWN_PENDING | RIDE_LIFECYCLE_BROKEN_DOWN)) {
		sampleRate = ride->var_1AC * 70;
		if (ride->breakdown_reason_pending != BREAKDOWN_CONTROL_FAILURE)
			sampleRate *= -1;
		sampleRate += 22050;
	}

	ride->music_position = sub_6BC3AC(x, y, z, rideIndex, sampleRate, ride->music_position, &ride->music_tune_id);
}

#pragma endregion

#pragma region Measurement functions

/**
 *
 *  rct2: 0x006B642B
 */
void ride_measurement_clear(rct_ride *ride)
{
	rct_ride_measurement *measurement;

	if (ride->measurement_index == 255)
		return;

	measurement = GET_RIDE_MEASUREMENT(ride->measurement_index);
	measurement->ride_index = 255;
	ride->measurement_index = 255;
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

rct_ride_measurement *ride_get_existing_measurement(int rideIndex)
{
	int i;
	rct_ride_measurement *measurement;

	for (i = 0; i < MAX_RIDE_MEASUREMENTS; i++) {
		measurement = GET_RIDE_MEASUREMENT(i);
		if (measurement->ride_index == rideIndex)
			return measurement;
	}

	return NULL;
}

rct_ride_measurement *ride_get_free_measurement()
{
	int i;
	rct_ride_measurement *measurement;

	for (i = 0; i < MAX_RIDE_MEASUREMENTS; i++) {
		measurement = GET_RIDE_MEASUREMENT(i);
		if (measurement->ride_index == 255)
			return measurement;
	}

	return NULL;
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
	measurement = ride_get_existing_measurement(rideIndex);
	if (measurement == NULL) {
		// Find a free measurement
		measurement = ride_get_free_measurement();
		if (measurement == NULL) {
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
		}

		measurement->ride_index = rideIndex;
		ride->measurement_index = i;
		measurement->flags = 0;
		if (RCT2_GLOBAL(RCT2_ADDRESS_RIDE_FLAGS + (ride->type * 8), uint32) & 0x80)
			measurement->flags |= RIDE_MEASUREMENT_FLAG_G_FORCES;
		measurement->num_items = 0;
		measurement->current_item = 0;
	}

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

#pragma endregion

#pragma region Colour functions

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

#pragma endregion

#pragma region Reachability

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
 * rct2: 0x006B7C59
 * @return 1 if the coordinate is reachable or has no entrance, 0 otherwise
 */
static int ride_entrance_exit_is_reachable(uint16 coordinate, rct_ride* ride, int index) {
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

static void ride_entrance_exit_connected(rct_ride* ride, int ride_idx)
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

static void ride_shop_connected(rct_ride* ride, int ride_idx)
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

#pragma endregion