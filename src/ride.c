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

#include "addresses.h"
#include "map.h"
#include "news_item.h"
#include "sprite.h"
#include "ride.h"
#include "sprite.h"
#include "peep.h"
#include "window.h"

#define GET_RIDE(x) (&(RCT2_ADDRESS(RCT2_ADDRESS_RIDE_LIST, rct_ride)[x]))
#define GET_RIDE_MEASUREMENT(x) (&(RCT2_ADDRESS(RCT2_ADDRESS_RIDE_MEASUREMENTS, rct_ride_measurement)[x]))

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

int ride_get_count()
{
	rct_ride *ride;
	int i, count = 0;

	for (i = 0; i < MAX_RIDES; i++) {
		ride = GET_RIDE(i);
		if (ride->type != RIDE_TYPE_NULL)
			count++;
	}

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
		ride = GET_RIDE(i);
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
	for (i = 0; i < MAX_RIDES; i++) {
		ride = GET_RIDE(i);
		if (ride->type != RIDE_TYPE_NULL) {
			//mov	ax, current_month_year
			//sub	[esi + 180h], ax
			ride->build_date -= RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_MONTH_YEAR, uint16);
		}
	}
}

/**
  * rct2: 0x006AC916
  */
void ride_update_favourited_stat()
{
	rct_ride *ride;
	rct_peep* peep;

	for (int i = 0; i < MAX_RIDES; i++) {
		ride = GET_RIDE(i);
		if (ride->type != RIDE_TYPE_NULL)
			ride->guests_favourite = 0;

	}
	for (int sprite_idx = RCT2_GLOBAL(RCT2_ADDRESS_SPRITES_START_PEEP, uint16); sprite_idx != SPRITE_INDEX_NULL; sprite_idx = peep->next) {
		peep = &(RCT2_ADDRESS(RCT2_ADDRESS_SPRITE_LIST, rct_sprite)[sprite_idx].peep);
		if (peep->var_08 != 4)
			return;
		if (peep->favourite_ride != 0xff) {
			ride = GET_RIDE(peep->favourite_ride);
			ride->guests_favourite++;
			ride->var_14D |= 1;

		}

	}
	window_invalidate_by_id(WC_RIDE_LIST, 0);
}


int map_coord_is_connected(uint16 coordinate, uint8 height, uint8 face_direction)
{
    rct_map_element* tile = RCT2_ADDRESS(RCT2_ADDRESS_TILE_MAP_ELEMENT_POINTERS, rct_map_element*)[coordinate / 4];

    do {
        rct_map_element_path_properties props = tile->properties.path;
        uint8 path_type = props.type >> 2, path_dir = props.type & 3;
        uint8 element_type = tile->type & 0x3C;

        if (!(element_type & PATH_ROAD))
            continue;

        if (path_type & 1) {
			if (path_dir == face_direction) {
				if (height == tile->base_height + 2)
					return 1;
			}
			else if (path_dir ^ 2 == face_direction && height == tile->base_height) {
				return 1;
			}
        } else {
			if (height == tile->base_height)
				return 1;
        }
            
    } while (!(tile->flags & MAP_ELEMENT_FLAG_LAST_TILE) && tile++);

    return 0;
}



/**
 * rct2: 0x006B7C59
 * @return 1 if the coordinate is reachable or has no entrance, 0 otw
 */
int ride_is_reachable(uint16 coordinate, rct_ride* ride, int index) {
	int x = ((coordinate >> 8) & 0xFF) << 5, // cx
		y = (coordinate & 0xFF) << 5;		 // ax	
	uint8 station_height = ride->pad_05A[index]; // pad_05a is uint8 station_base_height[4]
	int tile_idx = ((x << 8) | y) >> 3;
	rct_map_element* tile = RCT2_ADDRESS(RCT2_ADDRESS_TILE_MAP_ELEMENT_POINTERS, rct_map_element*)[tile_idx / 4];

	while(1) {
        uint8 element_type = tile->type & 0x3C;
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
	tile_idx = ((x << 8) | y) >> 3;

    return map_coord_is_connected(tile_idx, station_height, face_direction);
	/* while (1) { */
	/* 	rct_map_element_path_properties props; */

	/* 	element_type = tile->type & 0x3C; */
	/* 	if (!(element_type & MAP_ELEMENT_TYPE_PATH)) */
	/* 		goto end; */

	/* 	props = tile->properties.path; */
	/* 	if ( props.type & PATH_ROAD) { // with roads slopes count as connected sometimes */
	/* 		if ((props.type & 3) == face_direction) { */
	/* 			if (station_height == tile->base_height + 2) */
	/* 				return 1; */
	/* 		} */
	/* 		else { */
	/* 			uint8 madness = (props.type & 3) ^ 2; */
	/* 			if (madness == face_direction && station_height == tile->base_height) */
	/* 				return 1; */
	/* 		} */
	/* 	} else { // off-road only same height counts as connected */
	/* 		if (station_height == tile->base_height) */
	/* 			return 1;			 */
	/* 	} */

	/* end: */
	/* 	if (tile->flags & MAP_ELEMENT_FLAG_LAST_TILE) */
	/* 		return 0; */
    /*     tile++; */
	/* } */
}


void ride_entrance_exit_connected(rct_ride* ride, int ride_idx)
{
	for (int i = 0; i < 4; ++i) {
		uint16 station_start = ride->station_starts[i],
			entrance = ride->entrances[i],
			exit = ride->exits[i];

		if (station_start == -1 )
			continue;
		if (entrance != -1 && !ride_is_reachable(entrance, ride, i)) {
			RCT2_GLOBAL(0x013CE952, uint16) = ride->var_04A;
			RCT2_GLOBAL(0x013CE954, uint32) = ride->var_04C;			
			news_item_add_to_queue(1, 0xb26, ride_idx);
			ride->var_1AF = 3;
		}
			
		if (exit != -1 && !ride_is_reachable(exit, ride, i)) {
			RCT2_GLOBAL(0x013CE952, uint16) = ride->var_04A;
			RCT2_GLOBAL(0x013CE954, uint32) = ride->var_04C;
			news_item_add_to_queue(1, 0xb27, ride_idx);
			ride->var_1AF = 3;
		}

	} 
}


void blue_reachable(rct_ride* ride, int ride_idx)
{
    uint16 coordinate = ride->station_starts[ride_idx];
	int x = ((coordinate >> 8) & 0xFF) << 5, // cx
		y = (coordinate & 0xFF) << 5;		 // ax	
    uint16 magic = 0;
	int tile_idx = ((x << 8) | y) >> 3, count = 0;
	rct_map_element* tile = RCT2_ADDRESS(RCT2_ADDRESS_TILE_MAP_ELEMENT_POINTERS, rct_map_element*)[tile_idx / 4];

    while (1) {
        // First find the appropriate track element for our ride
        uint8 element_type = tile->type & 0x3C;
        if(element_type == MAP_ELEMENT_TYPE_TRACK && tile->properties.track.ride_index == ride_idx)
            break;

        if(tile->flags & MAP_ELEMENT_FLAG_LAST_TILE)
            return;
        tile++;
    }

    uint8 track_type = tile->properties.track.type;
    if (RCT2_GLOBAL(RCT2_ADDRESS_RIDE_FLAGS + ride->type * 8, uint32) & 0x80000) {
        magic = RCT2_ADDRESS(0x0099BA64, uint8)[track_type * 16];
    } else {
        magic = RCT2_ADDRESS(0x0099CA64, uint8)[track_type * 16];
    }

    magic = magic << (tile->type & 3);
    magic = ((magic >> 12) | magic) & 0xF;
    
    for (int count = 0; magic != 0; ++count) {        
        if (!(magic & 1)) {
            magic >>= 1;
            continue;
        }

        uint8 face_direction = count ^ 2;
        y -= RCT2_ADDRESS(0x00993CCC, sint16)[face_direction * 2];
        x -= RCT2_ADDRESS(0x00993CCE, sint16)[face_direction * 2];
        tile_idx = ((x << 8) | y) >> 3;

        if (map_coord_is_connected(tile, tile->base_height, face_direction))
            return;
    }    
    
    RCT2_GLOBAL(0x013CE952, uint16) = ride->var_04A;
    RCT2_GLOBAL(0x013CE954, uint32) = ride->var_04C;			
    news_item_add_to_queue(1, 0xb26, ride_idx);
    ride->var_1AF = 3;
}



/**
 * rct2: 0x006B7A5E
 **/
void ride_check_all_reachable()
{
/* XXX:  coordinates gleich um 5 shiften und / 4 ueberall weg */
	rct_ride *ride;

	for (int i = 0; i < MAX_RIDES; i++) {
		ride = GET_RIDE(i);
		if (ride->type == RIDE_TYPE_NULL)
			continue;
		if (ride->var_1AF != 0)
			ride->var_1AF--;
		if (ride->status != RIDE_STATUS_OPEN || ride->var_1AF != 0)
			continue;

		if (RCT2_GLOBAL(RCT2_ADDRESS_RIDE_FLAGS + ride->type * 8, uint32) & 0x20000) {
			//lightblue
            blue_reachable(ride, i);
			return;
		}
		else {
			//pink
			ride_entrance_exit_connected(ride, i);
		}

	}
}

