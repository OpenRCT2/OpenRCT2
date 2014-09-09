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
#include "addresses.h"
#include "game.h"
#include "map.h"
#include "news_item.h"
#include "staff.h"
#include "sprite.h"
#include "ride.h"
#include "sprite.h"
#include "peep.h"
#include "window.h"

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

rct_ride_type **gRideTypeList = RCT2_ADDRESS(0x009ACFA4, rct_ride_type*);
rct_ride* g_ride_list = RCT2_ADDRESS(RCT2_ADDRESS_RIDE_LIST, rct_ride);

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
		ride_measurement->var_00 = 0xFF;
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

	window_invalidate_by_id(WC_RIDE_LIST, 0);
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

/**
 *
 * rct2: 0x006B4800
 */
void ride_construct_new(int list_item)
{
	int eax, ebx, ecx, edx, esi, edi, ebp;
	edx = list_item;
	eax = 0;
	ecx = 0;
	ebx = 1;
	edi = 0;
	esi = 0;

	RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TITLE, uint16) = 0x3DC;

	esi = GAME_COMMAND_6;
	game_do_command_p(esi, &eax, &ebx, &ecx, &edx, &esi, &edi, &ebp);
	if (ebx == 0x80000000) {
		return;
	}

	//Looks like edi became the ride index after the command.
	eax = edi;
	rct_window *w;

	//TODO: replace with window_ride_main_open(eax)
	// window_ride_main_open(eax);
	RCT2_CALLFUNC_X(0x006ACC28, &eax, &ebx, &ecx, &edx, &esi, &edi, &ebp);
	window_get_register(w);

	ecx = w->classification;
	edx = 0x13;
	ebp = (int)w;
	//TODO: replace with window_ride_main_mouseup() after ride-window_merge
	// window_ride_main_mouseup();
	RCT2_CALLFUNC_X(0x006AF17E, &eax, &ebx, &ecx, &edx, &esi, &edi, &ebp);
	rct_window *ride_window = window_find_by_id(w->classification, w->number); //class here
	window_close(ride_window);
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