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
#include "../audio/audio.h"
#include "../audio/mixer.h"
#include "../cheats.h"
#include "../common.h"
#include "../config.h"
#include "../game.h"
#include "../input.h"
#include "../interface/window.h"
#include "../localisation/date.h"
#include "../localisation/localisation.h"
#include "../management/finance.h"
#include "../management/marketing.h"
#include "../management/news_item.h"
#include "../peep/peep.h"
#include "../peep/staff.h"
#include "../rct1.h"
#include "../scenario.h"
#include "../util/util.h"
#include "../windows/error.h"
#include "../world/banner.h"
#include "../world/footpath.h"
#include "../world/map.h"
#include "../world/map_animation.h"
#include "../world/sprite.h"
#include "ride.h"
#include "ride_data.h"
#include "track.h"
#include "track_data.h"
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
void ride_prepare_breakdown(int rideIndex, int breakdownReason);
static void ride_shop_connected(rct_ride* ride, int ride_idx);
static void ride_spiral_slide_update(rct_ride *ride);
static void ride_update(int rideIndex);
static void ride_update_vehicle_colours(int rideIndex);
static void sub_6DE52C(rct_ride *ride);

rct_ride_type *ride_get_entry(rct_ride *ride)
{
	return GET_RIDE_ENTRY(ride->subtype);
}

/**
*
*  rct2: 0x006DED68
*/
void reset_type_to_ride_entry_index_map(){
	uint8* typeToRideEntryIndexMap = RCT2_ADDRESS(0x009E32F8, uint8);
	memset(typeToRideEntryIndexMap, 0xFF, 91);
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
			ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_CUSTOMER;

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
			priceMinusCost += ride->price_secondary;
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
int sub_6CAF80(int rideIndex, rct_xy_element *output)
{
	map_element_iterator it;
	rct_map_element *resultMapElement;
	int foundSpecialTrackPiece;

	resultMapElement = NULL;
	foundSpecialTrackPiece = 0;

	map_element_iterator_begin(&it);
	do {
		if (map_element_get_type(it.element) != MAP_ELEMENT_TYPE_TRACK)
			continue;
		if (rideIndex != it.element->properties.track.ride_index)
			continue;

		// Found a track piece for target ride

		// Check if its not the station or ??? (but allow end piece of station)
		int specialTrackPiece = (
			it.element->properties.track.type != 2 &&
			it.element->properties.track.type != 3 &&
			(RCT2_ADDRESS(0x0099BA64, uint8)[it.element->properties.track.type * 16] & 0x10)
		);

		// Set result tile to this track piece if first found track or a ???
		if (resultMapElement == NULL || specialTrackPiece) {
			resultMapElement = it.element;

			if (output != NULL) {
				output->element = resultMapElement;
				output->x = it.x * 32;
				output->y = it.y * 32;
			}
		}

		if (specialTrackPiece) {
			foundSpecialTrackPiece = 1;
			return 1;
		}
	} while (map_element_iterator_next(&it));

	return resultMapElement != NULL;
}

/**
*
* rct2: 0x006C6096
* Gets the next track block coordinates from the
* coordinates of the first of element of a track block.
* Use track_block_get_next if you are unsure if you are
* on the first element of a track block
*/
bool track_block_get_next_from_zero(sint16 x, sint16 y, sint16 z_start, uint8 rideIndex, uint8 direction_start, rct_xy_element *output, int *z, int *direction)
{
	rct_ride* ride = GET_RIDE(rideIndex);
	RCT2_GLOBAL(0x00F441D3, uint8) = direction_start;

	if (!(direction_start & (1 << 2))){
		x += RCT2_ADDRESS(0x00993CCC, sint16)[RCT2_GLOBAL(0x00F441D3, uint8) * 2];
		y += RCT2_ADDRESS(0x00993CCE, sint16)[RCT2_GLOBAL(0x00F441D3, uint8) * 2];
	}

	rct_map_element* mapElement = map_get_first_element_at(x / 32, y / 32);
	if (mapElement == NULL){
		output->element = NULL;
		output->x = (sint16)0x8000;
		return 0;
	}

	do{
		if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_TRACK)
			continue;

		if (mapElement->properties.track.ride_index != rideIndex)
			continue;

		if ((mapElement->properties.track.sequence & 0xF) != 0)
			continue;

		const rct_preview_track* nextTrackBlock = get_track_def_from_ride(ride, mapElement->properties.track.type);
		const rct_track_coordinates* nextTrackCoordinate = get_track_coord_from_ride(ride, mapElement->properties.track.type);

		uint8 nextRotation =
			((nextTrackCoordinate->rotation_begin +
			(mapElement->type & MAP_ELEMENT_DIRECTION_MASK)) & MAP_ELEMENT_DIRECTION_MASK) |
			(nextTrackCoordinate->rotation_begin & (1 << 2));

		if (nextRotation != RCT2_GLOBAL(0x00F441D3, uint8))
			continue;

		sint16 nextZ = nextTrackCoordinate->z_begin - nextTrackBlock->z + mapElement->base_height * 8;
		if (nextZ != z_start)
			continue;

		if (z != NULL) *z = mapElement->base_height * 8;
		if (direction != NULL) *direction = nextRotation;
		output->x = x;
		output->y = y;
		output->element = mapElement;
		return 1;
	} while (!map_element_is_last_for_tile(mapElement++));

	if (direction != NULL) *direction = RCT2_GLOBAL(0x00F441D3, uint8);
	if (z != NULL) *z = z_start;
	output->x = x;
	output->y = y;
	output->element = --mapElement;
	return 0;
}

/**
 * 
 * rct2: 0x006C60C2
 */
bool track_block_get_next(rct_xy_element *input, rct_xy_element *output, int *z, int *direction)
{
	uint8 rideIndex = input->element->properties.track.ride_index;
	RCT2_GLOBAL(0x00F441D2, uint8) = rideIndex;
	rct_ride* ride = GET_RIDE(rideIndex);

	const rct_preview_track* trackBlock = get_track_def_from_ride(ride, input->element->properties.track.type);
	uint8 sequence = input->element->properties.track.sequence & 0xF;
	trackBlock += sequence;

	const rct_track_coordinates* trackCoordinate = get_track_coord_from_ride(ride, input->element->properties.track.type);

	int x = input->x;
	int y = input->y;
	int OriginZ = input->element->base_height * 8;

	uint8 rotation = input->element->type & MAP_ELEMENT_DIRECTION_MASK;
	switch (rotation){
	case 0:
		x += trackCoordinate->x;
		x -= trackBlock->x;
		y += trackCoordinate->y;
		y -= trackBlock->y;
		break;
	case 1:
		x += trackCoordinate->y;
		x -= trackBlock->y;
		y -= trackCoordinate->x;
		y += trackBlock->x;
		break;
	case 2:
		x -= trackCoordinate->x;
		x += trackBlock->x;
		y -= trackCoordinate->y;
		y += trackBlock->y;
		break;
	case 3:
		x -= trackCoordinate->y;
		x += trackBlock->y;
		y += trackCoordinate->x;
		y -= trackBlock->x;
		break;
	}

	OriginZ -= trackBlock->z;
	OriginZ += trackCoordinate->z_end;

	uint8 directionStart = ((trackCoordinate->rotation_end + rotation) & MAP_ELEMENT_DIRECTION_MASK) |
		(trackCoordinate->rotation_end & (1 << 2));

	return track_block_get_next_from_zero(x, y, OriginZ, rideIndex, directionStart, output, z, direction);
}

/* rct2: 0x006C63D6 
 * Returns the begin position / direction and end position / direction of the track piece that procedes the given location.
 * Gets the previous track block coordinates from the 
 * coordinates of the first of element of a track block.
 * Use track_block_get_previous if you are unsure if you are
 * on the first element of a track block
 */
bool track_block_get_previous_from_zero(sint16 x, sint16 y, sint16 z, uint8 rideIndex, uint8 direction, track_begin_end *outTrackBeginEnd){
	rct_ride* ride = GET_RIDE(rideIndex);

	RCT2_GLOBAL(0x00F441D3, uint8) = direction;
	direction ^= (1 << 1);

	if (!(direction & (1 << 2))){
		x += RCT2_ADDRESS(0x00993CCC, sint16)[direction * 2];
		y += RCT2_ADDRESS(0x00993CCE, sint16)[direction * 2];
	}

	rct_map_element* mapElement = map_get_first_element_at(x / 32, y / 32);
	if (mapElement == NULL){
		outTrackBeginEnd->end_x = x;
		outTrackBeginEnd->end_y = y;
		outTrackBeginEnd->begin_element = NULL;
		outTrackBeginEnd->begin_direction = RCT2_GLOBAL(0x00F441D3, uint8) ^ (1 << 1);
		return 0;
	}

	do{
		if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_TRACK)
			continue;

		if (mapElement->properties.track.ride_index != rideIndex)
			continue;

		const rct_preview_track* nextTrackBlock = get_track_def_from_ride(ride, mapElement->properties.track.type);
		const rct_track_coordinates* nextTrackCoordinate = get_track_coord_from_ride(ride, mapElement->properties.track.type);

		nextTrackBlock += mapElement->properties.track.sequence & 0xF;
		if ((nextTrackBlock + 1)->index != 255)
			continue;

		uint8 nextRotation =
			((nextTrackCoordinate->rotation_end +
			(mapElement->type & MAP_ELEMENT_DIRECTION_MASK)) & MAP_ELEMENT_DIRECTION_MASK) |
			(nextTrackCoordinate->rotation_end & (1 << 2));

		if (nextRotation != RCT2_GLOBAL(0x00F441D3, uint8))
			continue;

		sint16 nextZ = nextTrackCoordinate->z_end - nextTrackBlock->z + mapElement->base_height * 8;
		if (nextZ != z)
			continue;

		nextRotation =
			((nextTrackCoordinate->rotation_begin +
			(mapElement->type & MAP_ELEMENT_DIRECTION_MASK)) & MAP_ELEMENT_DIRECTION_MASK) |
			(nextTrackCoordinate->rotation_begin & (1 << 2));
		outTrackBeginEnd->begin_element = mapElement;
		outTrackBeginEnd->begin_x = x;
		outTrackBeginEnd->begin_y = y;
		outTrackBeginEnd->end_x = x;
		outTrackBeginEnd->end_y = y;
		switch (nextRotation & 3){
		case 0:
			outTrackBeginEnd->begin_x -= nextTrackCoordinate->x;
			outTrackBeginEnd->begin_y -= nextTrackCoordinate->y;
			break;
		case 1:
			outTrackBeginEnd->begin_x -= nextTrackCoordinate->y;
			outTrackBeginEnd->begin_y += nextTrackCoordinate->x;
			break;
		case 2:
			outTrackBeginEnd->begin_x += nextTrackCoordinate->x;
			outTrackBeginEnd->begin_y += nextTrackCoordinate->y;
			break;
		case 3:
			outTrackBeginEnd->begin_x += nextTrackCoordinate->y;
			outTrackBeginEnd->begin_y -= nextTrackCoordinate->x;
			break;
		}

		outTrackBeginEnd->begin_z = mapElement->base_height * 8;
		outTrackBeginEnd->begin_z +=
			get_track_def_from_ride(ride, mapElement->properties.track.type)->z -
			nextTrackBlock->z;

		outTrackBeginEnd->begin_direction = nextRotation;
		outTrackBeginEnd->end_direction = RCT2_GLOBAL(0x00F441D3, uint8) ^ (1 << 1);
		return 1;
	} while (!map_element_is_last_for_tile(mapElement++));

	outTrackBeginEnd->end_x = x;
	outTrackBeginEnd->end_y = y;
	outTrackBeginEnd->begin_z = z;
	outTrackBeginEnd->begin_element = NULL;
	outTrackBeginEnd->end_direction = RCT2_GLOBAL(0x00F441D3, uint8) ^ (1 << 1);
	return 0;
}

/**
 * 
 * rct2: 0x006C6402
 *
 * @remarks outTrackBeginEnd.begin_x and outTrackBeginEnd.begin_y will be in the higher two bytes of ecx and edx where as
            outTrackBeginEnd.end_x and outTrackBeginEnd.end_y will be in the lower two bytes (cx and dx).
 */
bool track_block_get_previous(int x, int y, rct_map_element *mapElement, track_begin_end *outTrackBeginEnd)
{
	uint8 rideIndex = mapElement->properties.track.ride_index;
	RCT2_GLOBAL(0x00F441D2, uint8) = rideIndex;
	rct_ride* ride = GET_RIDE(rideIndex);

	const rct_preview_track* trackBlock = get_track_def_from_ride(ride, mapElement->properties.track.type);
	uint8 sequence = mapElement->properties.track.sequence & 0xF;
	trackBlock += sequence;

	const rct_track_coordinates* trackCoordinate = get_track_coord_from_ride(ride, mapElement->properties.track.type);

	int z = mapElement->base_height * 8;

	uint8 rotation = mapElement->type & MAP_ELEMENT_DIRECTION_MASK;
	switch (rotation){
	case 0:
		x -= trackBlock->x;
		y -= trackBlock->y;
		break;
	case 1:
		x -= trackBlock->y;
		y += trackBlock->x;
		break;
	case 2:
		x += trackBlock->x;
		y += trackBlock->y;
		break;
	case 3:
		x += trackBlock->y;
		y -= trackBlock->x;
		break;
	}

	z -= trackBlock->z;
	z += trackCoordinate->z_begin;

	rotation =
		((trackCoordinate->rotation_begin + rotation) & MAP_ELEMENT_DIRECTION_MASK) |
		(trackCoordinate->rotation_begin & (1 << 2));

	return track_block_get_previous_from_zero(x, y, z, rideIndex, rotation, outTrackBeginEnd);
}

/**
 * 
 * Make sure to pass in the x and y of the start track element too.
 * rct2: 0x006CB02F
 * ax result x
 * bx result y
 * esi input / output map element
 */
int ride_find_track_gap(rct_xy_element *input, rct_xy_element *output)
{
	rct_window *w;
	rct_ride *ride;
	track_circuit_iterator it;
	int rideIndex;

	rideIndex = input->element->properties.track.ride_index;
	ride = GET_RIDE(rideIndex);

	if (ride->type == RIDE_TYPE_MAZE)
		return 0;
	
	w = window_find_by_class(WC_RIDE_CONSTRUCTION);
	if (w != NULL && _rideConstructionState != RIDE_CONSTRUCTION_STATE_0 && _currentRideIndex == rideIndex)
		sub_6C9627();

	track_circuit_iterator_begin(&it, *input);
	while (track_circuit_iterator_next(&it)) {
		if (!track_is_connected_by_shape(it.last.element, it.current.element)) {
			*output = it.current;
			return 1;
		}
	}
	if (!it.looped) {
		*output = it.last;
		return 1;
	}

	return 0;
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
		if (!ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_IS_SHOP)) {
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

int ride_get_total_time(rct_ride *ride)
{
	int i, totalTime = 0;
	for (i = 0; i < ride->num_stations; i++)
		totalTime += ride->time[i];
	return totalTime;
}

int ride_can_have_multiple_circuits(rct_ride *ride)
{
	if (!(RCT2_GLOBAL(0x0097D4F2 + (ride->type * 8), uint16) & 0x200))
		return 0;

	// Only allow circuit or launch modes
	if (
		ride->mode != RIDE_MODE_CONTINUOUS_CIRCUIT &&
		ride->mode != RIDE_MODE_REVERSE_INCLINE_LAUNCHED_SHUTTLE &&
		ride->mode != RIDE_MODE_POWERED_LAUNCH_PASSTROUGH
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
		_currentRideIndex = rideIndex;
		w = window_ride_construction_open(rideIndex);
	} else {
		sub_6C9627();
		_currentRideIndex = rideIndex;
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

	esi = GAME_COMMAND_CREATE_RIDE;
	game_do_command_p(esi, &eax, &ebx, &ecx, &edx, &esi, &edi, &ebp);
	return ebx == MONEY32_UNDEFINED ? -1 : edi;
}

/**
 *
 * rct2: 0x006B4800
 */
void ride_construct_new(ride_list_item listItem)
{
	int rideIndex;

	rideIndex = ride_create_ride(listItem);
	if (rideIndex != -1)
		ride_construct(rideIndex);
}

/**
 *
 * rct2: 0x006B4857
 */
void ride_construct(int rideIndex)
{
	rct_xy_element trackElement;
	rct_window *w;

	if (sub_6CAF80(rideIndex, &trackElement)) {
		ride_find_track_gap(&trackElement, &trackElement);

		w = window_get_main();
		if (w != NULL && ride_modify(&trackElement))
			window_scroll_to_location(w, trackElement.x, trackElement.y, trackElement.element->base_height * 8);
	} else {
		sub_6CC3FB(rideIndex);
	}
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
			invalidate_sprite((rct_sprite*)vehicle);
			sprite_remove((rct_sprite*)vehicle);
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
				invalidate_sprite((rct_sprite*)vehicle);
				sprite_remove((rct_sprite*)vehicle);
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
void ride_clear_for_construction(int rideIndex)
{
	rct_ride *ride;
	rct_window *w;

	ride = GET_RIDE(rideIndex);

	ride_measurement_clear(ride);
	
	ride->lifecycle_flags &= ~(RIDE_LIFECYCLE_BREAKDOWN_PENDING | RIDE_LIFECYCLE_BROKEN_DOWN);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_MAIN | RIDE_INVALIDATE_RIDE_LIST;

	ride_remove_cable_lift(ride);
	ride_remove_vehicles(ride);

	w = window_find_by_number(WC_RIDE, rideIndex);
	if (w != NULL)
		window_event_resize_call(w);
}

/**
 * 
 * rct2: 0x006664DF
 */
void ride_remove_peeps(int rideIndex)
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

			peep_decrement_num_riders(peep);
			if (peep->state == PEEP_STATE_QUEUING_FRONT && peep->sub_state == 0)
				remove_peep_from_queue(peep);

			invalidate_sprite((rct_sprite*)peep);

			if (exitDirection == 255) {
				x = peep->next_x + 16;
				y = peep->next_y + 16;
				z = peep->next_z * 8;
				if (peep->next_var_29 & 4)
					z += 8;
				z++;
				sprite_move(x, y, z, (rct_sprite*)peep);
			} else {
				sprite_move(exitX, exitY, exitZ, (rct_sprite*)peep);
				peep->sprite_direction = exitDirection;
			}

			invalidate_sprite((rct_sprite*)peep);
			peep->state = PEEP_STATE_FALLING;
			sub_693BE5(peep, 0);
			
			peep->happiness = min(peep->happiness, peep->happiness_growth_rate) / 2;
			peep->happiness_growth_rate = peep->happiness;
			peep->window_invalidate_flags |= PEEP_INVALIDATE_PEEP_STATS;
		}
	}

	ride->num_riders = 0;
	ride->var_15D = 0;
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_MAIN;
}

/**
 * Gets the origin track element (sequence 0). Seems to do more than that though and even invalidates track.
 * rct2: 0x006C683D 
 * ax : x
 * bx : direction << 8, type
 * cx : y
 * dx : z
 * si : extra_params
 * di : output_element
 * bp : flags
 */
int sub_6C683D(int* x, int* y, int* z, int direction, int type, uint16 extra_params, rct_map_element** output_element, uint16 flags)
{
	rct_map_element *mapElement = map_get_first_element_at(*x / 32, *y / 32);
	rct_map_element *successMapElement = NULL;

	do {
		if (mapElement->base_height != *z / 8)
			continue;

		if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_TRACK)
			continue;

		if ((mapElement->type & MAP_ELEMENT_DIRECTION_MASK) != direction)
			continue;

		if (type != mapElement->properties.track.type)
			continue;

		successMapElement = mapElement;
		if ((mapElement->properties.track.sequence & 0x0F) == 0)
			break;
	} while (!map_element_is_last_for_tile(mapElement++));

	mapElement = successMapElement;
	if (mapElement == NULL)
		return 1;

	// Possibly z should be & 0xF8
	const rct_preview_track *trackBlock = get_track_def_from_ride_index(mapElement->properties.track.ride_index, type);

	int sequence = mapElement->properties.track.sequence & 0x0F;
	uint8 mapDirection = mapElement->type & MAP_ELEMENT_DIRECTION_MASK;

	switch (mapDirection){
	case MAP_ELEMENT_DIRECTION_WEST:
		*x -= trackBlock[sequence].x;
		*y -= trackBlock[sequence].y;
		break;
	case MAP_ELEMENT_DIRECTION_NORTH:
		*x -= trackBlock[sequence].y;
		*y += trackBlock[sequence].x;
		break;
	case MAP_ELEMENT_DIRECTION_EAST:
		*x += trackBlock[sequence].x;
		*y += trackBlock[sequence].y;
		break;
	case MAP_ELEMENT_DIRECTION_SOUTH:
		*x += trackBlock[sequence].y;
		*y -= trackBlock[sequence].x;
		break;
	}
	*z -= trackBlock[sequence].z;

	int start_x = *x, start_y = *y, start_z = *z;
	*z += trackBlock[0].z;
	for (int i = 0; trackBlock[i].index != 0xFF; ++i){
		int cur_x = start_x, cur_y = start_y, cur_z = start_z;
		switch (mapDirection){
		case MAP_ELEMENT_DIRECTION_WEST:
			cur_x += trackBlock[i].x;
			cur_y += trackBlock[i].y;
			break;
		case MAP_ELEMENT_DIRECTION_NORTH:
			cur_x += trackBlock[i].y;
			cur_y -= trackBlock[i].x;
			break;
		case MAP_ELEMENT_DIRECTION_EAST:
			cur_x -= trackBlock[i].x;
			cur_y -= trackBlock[i].y;
			break;
		case MAP_ELEMENT_DIRECTION_SOUTH:
			cur_x -= trackBlock[i].y;
			cur_y += trackBlock[i].x;
			break;
		}
		cur_z += trackBlock[i].z;

		map_invalidate_tile_full(cur_x, cur_y);

		mapElement = map_get_first_element_at(cur_x / 32, cur_y / 32);
		successMapElement = NULL;
		do {
			if (mapElement->base_height != cur_z / 8)
				continue;

			if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_TRACK)
				continue;

			if ((mapElement->type & MAP_ELEMENT_DIRECTION_MASK) != direction)
				continue;

			if ((mapElement->properties.track.sequence & 0xF) != trackBlock[i].index)
				continue;

			if (type == mapElement->properties.track.type) {
				successMapElement = mapElement;
				break;
			}
		} while (!map_element_is_last_for_tile(mapElement++));

		if (successMapElement == NULL) {
			return 1;
		}
		if (i == 0 && output_element != NULL) {
			*output_element = mapElement;
		}
		if (flags & (1 << 0)) {
			// Switch highlight off
			mapElement->type &= ~MAP_ELEMENT_TYPE_FLAG_HIGHLIGHT;
		}
		if (flags & (1 << 1)) {
			// Switch highlight on
			mapElement->type |= MAP_ELEMENT_TYPE_FLAG_HIGHLIGHT;
		}
		if (flags & (1 << 2)) {
			mapElement->properties.track.colour &= 0xFC;
			mapElement->properties.track.colour |= extra_params & 0xFF;
		}
		if (flags & (1 << 5)) {
			// Seat rotation
			mapElement->properties.track.colour &= 0x0F;
			mapElement->properties.track.colour |= (extra_params & 0xFF) << 4;
		}
		if (flags & (1 << 3)) {
			mapElement->properties.track.colour |= (1 << 3);
		}
		if (flags & (1 << 4)) {
			mapElement->properties.track.colour &= 0xF7;
		}
	}

	return 0;
}

/**
 * 
 * rct2: 0x006C96C0
 */
void sub_6C96C0()
{
	rct_ride *ride;
	int rideIndex, x, y, z, direction;

	if (_currentTrackSelectionFlags & 4) {
		_currentTrackSelectionFlags &= ~4;
		game_do_command(
			RCT2_GLOBAL(0x00F440BF, uint16),
			41,
			RCT2_GLOBAL(0x00F440C1, uint16),
			_currentRideIndex,
			GAME_COMMAND_REMOVE_RIDE_ENTRANCE_OR_EXIT,
			RCT2_GLOBAL(0x00F440C4, uint8),
			0
		);
	}
	if (_currentTrackSelectionFlags & 2) {
		_currentTrackSelectionFlags &= ~2;

		rideIndex = _currentRideIndex;
		RCT2_GLOBAL(0x00F441D2, uint8) = rideIndex;
		
		x = RCT2_GLOBAL(0x00F440C5, uint16);
		y = RCT2_GLOBAL(0x00F440C7, uint16);
		z = RCT2_GLOBAL(0x00F440C9, uint16);

		ride = GET_RIDE(rideIndex);
		if (ride->type == RIDE_TYPE_MAZE) {
			game_do_command(x     , 41 | (0 << 8), y     , rideIndex | (2 << 8), GAME_COMMAND_SET_MAZE_TRACK, z, 0);
			game_do_command(x     , 41 | (1 << 8), y + 16, rideIndex | (2 << 8), GAME_COMMAND_SET_MAZE_TRACK, z, 0);
			game_do_command(x + 16, 41 | (2 << 8), y + 16, rideIndex | (2 << 8), GAME_COMMAND_SET_MAZE_TRACK, z, 0);
			game_do_command(x + 16, 41 | (3 << 8), y     , rideIndex | (2 << 8), GAME_COMMAND_SET_MAZE_TRACK, z, 0);
		} else {
			direction = RCT2_GLOBAL(0x00F440CB, uint8);
			if (!(direction & 4)) {
				x -= TileDirectionDelta[direction].x;
				y -= TileDirectionDelta[direction].y;
			}
			rct_xy_element next_track;
			
			if (track_block_get_next_from_zero(x, y, z, rideIndex, direction, &next_track, &z, &direction)) {
				game_do_command(
					next_track.x,
					105 | ((direction & 3) << 8),
					next_track.y,
					next_track.element->properties.track.type | ((next_track.element->properties.track.sequence & 0x0F) << 8),
					GAME_COMMAND_REMOVE_TRACK,
					z,
					0
				);
			}
		}
	}
}

void sub_6C9627()
{
	int x, y, z;

	switch (_rideConstructionState) {
	case RIDE_CONSTRUCTION_STATE_SELECTED:
		x = _currentTrackBeginX;
		y = _currentTrackBeginY;
		z = _currentTrackBeginZ;
		sub_6C683D(
			&x,
			&y,
			&z,
			_currentTrackPieceDirection & 3,
			_currentTrackPieceType,
			0,
			NULL,
			1
		);
		break;
	case RIDE_CONSTRUCTION_STATE_6:
	case RIDE_CONSTRUCTION_STATE_7:
	case RIDE_CONSTRUCTION_STATE_8:
		if (_currentTrackSelectionFlags & 1) {
			map_invalidate_tile_full(
				_currentTrackBeginX & 0xFFE0,
				_currentTrackBeginY & 0xFFE0
			);
			RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) &= ~4;
		}
		break;
	default:
		if (_currentTrackSelectionFlags & 1) {
			_currentTrackSelectionFlags &= ~1;
			RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint8) &= ~4;
			map_invalidate_tile_full(_currentTrackBeginX, _currentTrackBeginY);
		}
		sub_6C96C0();
		break;
	}
}

/**
 * 
 * rct2: 0x006C9B19
 */
static void ride_construction_reset_current_piece()
{
	rct_ride *ride;

	ride = GET_RIDE(_currentRideIndex);
	if (!ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_15) || ride->num_stations == 0) {
		_currentTrackCurve = RCT2_GLOBAL(0x0097CC68 + (ride->type * 2), uint8) | 0x100;
		_currentTrackSlopeEnd = 0;
		_currentTrackBankEnd = 0;
		_currentTrackLiftHill = 0;
		_currentTrackCovered = 0;
		if (RCT2_GLOBAL(0x0097D4F2 + (ride->type * 8), uint16) & 0x8000) {
			_currentTrackCovered |= 2;
		}
		_previousTrackSlopeEnd = 0;
		_previousTrackBankEnd = 0;
	} else {
		_currentTrackCurve = 0xFFFF;
		_rideConstructionState = RIDE_CONSTRUCTION_STATE_0;
	}
}

/**
 * 
 * rct2: 0x006C9800
 */
void ride_construction_set_default_next_piece()
{
	int x, y, z, direction, rideIndex, trackType, curve, bank, slope;
	rct_ride *ride;
	track_begin_end trackBeginEnd;
	rct_xy_element xyElement;
	rct_map_element *mapElement;

	_currentTrackPrice = MONEY32_UNDEFINED;
	switch (_rideConstructionState) {
	case RIDE_CONSTRUCTION_STATE_FRONT:
		rideIndex = _currentRideIndex;
		ride = GET_RIDE(rideIndex);

		RCT2_GLOBAL(0x00F441D2, uint8) = rideIndex;
		x = _currentTrackBeginX;
		y = _currentTrackBeginY;
		z = _currentTrackBeginZ;
		direction = _currentTrackPieceDirection;
		if (!track_block_get_previous_from_zero(x, y, z, rideIndex, direction, &trackBeginEnd)) {
			ride_construction_reset_current_piece();
			return;
		}
		mapElement = trackBeginEnd.begin_element;
		trackType = mapElement->properties.track.type;

		if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_15)) {
			ride_construction_reset_current_piece();
			return;
		}

		// Set whether track is covered
		_currentTrackCovered &= ~2;
		if (RCT2_GLOBAL(0x0097D4F2 + (ride->type * 8), uint16) & 8) {
			if (mapElement->properties.track.colour & 4) {
				_currentTrackCovered |= 2;
			}
		}

		if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_FLAT_RIDE)) {
			curve = gFlatRideTrackCurveChain[trackType].next;
			bank = gFlatRideTrackDefinitions[trackType].bank_end;
			slope = gFlatRideTrackDefinitions[trackType].vangle_end;
		} else {
			curve = gTrackCurveChain[trackType].next;
			bank = gTrackDefinitions[trackType].bank_end;
			slope = gTrackDefinitions[trackType].vangle_end;
		}

		// Set track curve
		_currentTrackCurve = curve;

		// Set track banking
		if (RCT2_GLOBAL(0x0097D4F2 + (ride->type * 8), uint16) & 8) {
			if (bank == TRACK_BANK_UPSIDE_DOWN) {
				bank = TRACK_BANK_NONE;
				_currentTrackCovered ^= 2;
			}
		}
		_currentTrackBankEnd = bank;
		_previousTrackBankEnd = bank;

		// Set track slope and lift hill
		_currentTrackSlopeEnd = slope;
		_previousTrackSlopeEnd = slope;
		_currentTrackLiftHill = ((mapElement->type & 0x80) && slope != TRACK_SLOPE_DOWN_25 && slope != TRACK_SLOPE_DOWN_60);
		break;
	case RIDE_CONSTRUCTION_STATE_BACK:
		rideIndex = _currentRideIndex;
		ride = GET_RIDE(rideIndex);

		RCT2_GLOBAL(0x00F441D2, uint8) = rideIndex;
		x = _currentTrackBeginX;
		y = _currentTrackBeginY;
		z = _currentTrackBeginZ;
		direction = _currentTrackPieceDirection ^ 2;
		if (!track_block_get_next_from_zero(x, y, z, rideIndex, direction, &xyElement, &z, &direction)) {
			ride_construction_reset_current_piece();
			return;
		}
		mapElement = xyElement.element;
		trackType = mapElement->properties.track.type;

		// Set whether track is covered
		_currentTrackCovered &= ~2;
		if (RCT2_GLOBAL(0x0097D4F2 + (ride->type * 8), uint16) & 8) {
			if (mapElement->properties.track.colour & 4) {
				_currentTrackCovered |= 2;
			}
		}

		if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_FLAT_RIDE)) {
			curve = gFlatRideTrackCurveChain[trackType].previous;
			bank = gFlatRideTrackDefinitions[trackType].bank_start;
			slope = gFlatRideTrackDefinitions[trackType].vangle_start;
		} else {
			curve = gTrackCurveChain[trackType].previous;
			bank = gTrackDefinitions[trackType].bank_start;
			slope = gTrackDefinitions[trackType].vangle_start;
		}

		// Set track curve
		_currentTrackCurve = curve;

		// Set track banking
		if (RCT2_GLOBAL(0x0097D4F2 + (ride->type * 8), uint16) & 8) {
			if (bank == TRACK_BANK_UPSIDE_DOWN) {
				bank = TRACK_BANK_NONE;
				_currentTrackCovered ^= 2;
			}
		}
		_currentTrackBankEnd = bank;
		_previousTrackBankEnd = bank;

		// Set track slope and lift hill
		_currentTrackSlopeEnd = slope;
		_previousTrackSlopeEnd = slope;
		_currentTrackLiftHill = (mapElement->type & 0x80);
		break;
	}
}

/**
 * 
 * rct2: 0x006C9296
 */
void ride_select_next_section()
{
	int x, y, z, direction, type;
	rct_map_element *mapElement;
	rct_xy_element inputElement, outputElement;

	if (_rideConstructionState == RIDE_CONSTRUCTION_STATE_SELECTED) {
		sub_6C9627();
		x = _currentTrackBeginX;
		y = _currentTrackBeginY;
		z = _currentTrackBeginZ;
		direction = _currentTrackPieceDirection;
		type = _currentTrackPieceType;
		if (sub_6C683D(&x, &y, &z, direction & 3, type, 0, &mapElement, 0)) {
			_rideConstructionState = RIDE_CONSTRUCTION_STATE_0;
			sub_6C84CE();
			return;
		}
		inputElement.x = x;
		inputElement.y = y;
		inputElement.element = mapElement;
		if (track_block_get_next(&inputElement, &outputElement, &z, &direction)) {
			x = outputElement.x;
			y = outputElement.y;
			mapElement = outputElement.element;
		} else {
			_rideConstructionState = RIDE_CONSTRUCTION_STATE_FRONT;
			_currentTrackBeginX = outputElement.x;
			_currentTrackBeginY = outputElement.y;
			_currentTrackBeginZ = z;
			_currentTrackPieceDirection = direction;
			_currentTrackPieceType = mapElement->properties.track.type;
			_currentTrackSelectionFlags = 0;
			_rideConstructionArrowPulseTime = 0;
			ride_construction_set_default_next_piece();
			sub_6C84CE();
			return;
		}

		_currentTrackBeginX = x;
		_currentTrackBeginY = y;
		_currentTrackBeginZ = z;
		_currentTrackPieceDirection = (mapElement->type & MAP_ELEMENT_DIRECTION_MASK);
		_currentTrackPieceType = mapElement->properties.track.type;
		_currentTrackSelectionFlags = 0;
		_rideConstructionArrowPulseTime = 0;
		sub_6C84CE();
	} else if (_rideConstructionState == RIDE_CONSTRUCTION_STATE_BACK) {
		if (ride_select_forwards_from_back()) {
			sub_6C84CE();
		}
	}
}

/**
 * 
 * rct2: 0x006C93B8
 */
void ride_select_previous_section()
{
	int x, y, z, direction, type;
	rct_map_element *mapElement;
	track_begin_end trackBeginEnd;

	if (_rideConstructionState == RIDE_CONSTRUCTION_STATE_SELECTED) {
		sub_6C9627();
		x = _currentTrackBeginX;
		y = _currentTrackBeginY;
		z = _currentTrackBeginZ;
		direction = _currentTrackPieceDirection;
		type = _currentTrackPieceType;
		if (sub_6C683D(&x, &y, &z, direction & 3, type, 0, &mapElement, 0)) {
			_rideConstructionState = RIDE_CONSTRUCTION_STATE_0;
			sub_6C84CE();
			return;
		}
		if (track_block_get_previous(x, y, mapElement, &trackBeginEnd)) {
			_currentTrackBeginX = trackBeginEnd.begin_x;
			_currentTrackBeginY = trackBeginEnd.begin_y;
			_currentTrackBeginZ = trackBeginEnd.begin_z;
			_currentTrackPieceDirection = trackBeginEnd.begin_direction;
			_currentTrackPieceType = trackBeginEnd.begin_element->properties.track.type;
			_currentTrackSelectionFlags = 0;
			_rideConstructionArrowPulseTime = 0;
			sub_6C84CE();
		} else {
			_rideConstructionState = RIDE_CONSTRUCTION_STATE_BACK;
			_currentTrackBeginX = trackBeginEnd.end_x;
			_currentTrackBeginY = trackBeginEnd.end_y;
			_currentTrackBeginZ = trackBeginEnd.begin_z;
			_currentTrackPieceDirection = trackBeginEnd.end_direction;
			_currentTrackPieceType = mapElement->properties.track.type;
			_currentTrackSelectionFlags = 0;
			_rideConstructionArrowPulseTime = 0;
			ride_construction_set_default_next_piece();
			sub_6C84CE();
		}
	} else if (_rideConstructionState == RIDE_CONSTRUCTION_STATE_FRONT) {
		if (ride_select_backwards_from_front()) {
			sub_6C84CE();
		}
	}
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
		_rideConstructionState != RIDE_CONSTRUCTION_STATE_ENTRANCE_EXIT ||
		!(RCT2_GLOBAL(RCT2_ADDRESS_INPUT_FLAGS, uint8) & INPUT_FLAG_TOOL_ACTIVE) ||
		RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WINDOWCLASS, rct_windowclass) != WC_RIDE_CONSTRUCTION
	) {
		// Replace entrance / exit
		tool_set(constructionWindow, entranceType == 0 ? 29 : 30, 12);
		RCT2_GLOBAL(0x00F44191, uint8) = entranceType;
		RCT2_GLOBAL(0x00F44192, uint8) = rideIndex;
		RCT2_GLOBAL(0x00F44193, uint8) = bl;
		RCT2_GLOBAL(RCT2_ADDRESS_INPUT_FLAGS, uint8) |= INPUT_FLAG_6;
		if (_rideConstructionState != RIDE_CONSTRUCTION_STATE_ENTRANCE_EXIT) {
			RCT2_GLOBAL(0x00F440CC, uint8) = _rideConstructionState;
			_rideConstructionState = RIDE_CONSTRUCTION_STATE_ENTRANCE_EXIT;
		}

		sub_6C84CE();
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) &= ~2;
	} else {
		// Remove entrance / exit
		game_do_command(x, 9, y, rideIndex, GAME_COMMAND_REMOVE_RIDE_ENTRANCE_OR_EXIT, bl, 0);
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
	_currentRideIndex = mapElement->properties.track.ride_index;
	_rideConstructionState = RIDE_CONSTRUCTION_STATE_6;
	_currentTrackBeginX = x;
	_currentTrackBeginY = y;
	_currentTrackBeginZ = mapElement->base_height * 8;
	_currentTrackSelectionFlags = 0;
	_rideConstructionArrowPulseTime = 0;
	window_maze_construction_update_pressed_widgets();
	return 1;
}

/**
 * 
 * rct2: 0x006CC056
 */
int ride_modify(rct_xy_element *input)
{
	int rideIndex, x, y, z, direction, type;
	rct_xy_element mapElement, endOfTrackElement;
	rct_ride *ride;
	rct_window *constructionWindow;

	mapElement = *input;
	rideIndex = mapElement.element->properties.track.ride_index;
	ride = GET_RIDE(rideIndex);

	if (!ride_check_if_construction_allowed(ride))
		return 0;

	if (ride->lifecycle_flags & RIDE_LIFECYCLE_INDESTRUCTIBLE) {
		RCT2_GLOBAL(0x013CE952 + 6, uint16) = ride->name;
		RCT2_GLOBAL(0x013CE952 + 8, uint32) = ride->name_arguments;
		window_error_open(STR_CANT_START_CONSTRUCTION_ON, STR_LOCAL_AUTHORITY_FORBIDS_DEMOLITION_OR_MODIFICATIONS_TO_THIS_RIDE);
		return 0;
	}

	if (ride->lifecycle_flags & RIDE_LIFECYCLE_SIX_FLAGS) {
		RCT2_GLOBAL(0x013CE952 + 6, uint16) = ride->name;
		RCT2_GLOBAL(0x013CE952 + 8, uint32) = ride->name_arguments;
		window_error_open(STR_CANT_START_CONSTRUCTION_ON, STR_THIS_RIDE_CANNOT_BE_MODIFIED);
		return 0;
	}

	ride_clear_for_construction(rideIndex);
	ride_remove_peeps(rideIndex);

	// Check if element is a station entrance or exit
	if (map_element_get_type(mapElement.element) == MAP_ELEMENT_TYPE_ENTRANCE)
		return ride_modify_entrance_or_exit(mapElement.element, mapElement.x, mapElement.y);

	constructionWindow = ride_create_or_find_construction_window(rideIndex);

	if (ride->type == RIDE_TYPE_MAZE)
		return ride_modify_maze(mapElement.element, mapElement.x, mapElement.y);

	if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_8)) {
		if (ride_find_track_gap(&mapElement, &endOfTrackElement))
			mapElement = endOfTrackElement;
	}

	x = mapElement.x;
	y = mapElement.y;
	z = mapElement.element->base_height * 8;
	direction = mapElement.element->type & 3;
	type = mapElement.element->properties.track.type;
	
	if (sub_6C683D(&x, &y, &z, direction, type, 0, NULL, 0))
		return 0;

	_currentRideIndex = rideIndex;
	_rideConstructionState = RIDE_CONSTRUCTION_STATE_SELECTED;
	_currentTrackBeginX = x;
	_currentTrackBeginY = y;
	_currentTrackBeginZ = z;
	_currentTrackPieceDirection = direction;
	_currentTrackPieceType = type;
	_currentTrackSelectionFlags = 0;
	_rideConstructionArrowPulseTime = 0;

	if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_15)) {
		sub_6C84CE();
		return 1;
	}

	ride_select_next_section();
	if (_rideConstructionState == RIDE_CONSTRUCTION_STATE_FRONT) {
		sub_6C84CE();
		return 1;
	}

	_rideConstructionState = RIDE_CONSTRUCTION_STATE_SELECTED;
	_currentTrackBeginX = x;
	_currentTrackBeginY = y;
	_currentTrackBeginZ = z;
	_currentTrackPieceDirection = direction;
	_currentTrackPieceType = type;
	_currentTrackSelectionFlags = 0;
	_rideConstructionArrowPulseTime = 0;

	ride_select_previous_section();

	if (_rideConstructionState != RIDE_CONSTRUCTION_STATE_BACK) {
		_rideConstructionState = RIDE_CONSTRUCTION_STATE_SELECTED;
		_currentTrackBeginX = x;
		_currentTrackBeginY = y;
		_currentTrackBeginZ = z;
		_currentTrackPieceDirection = direction;
		_currentTrackPieceType = type;
		_currentTrackSelectionFlags = 0;
		_rideConstructionArrowPulseTime = 0;
	}

	sub_6C84CE();
	return 1;
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

	ride = GET_RIDE(_currentRideIndex);

	_currentTrackCurve = RCT2_ADDRESS(0x0097CC68, uint8)[ride->type * 2] | 0x100;
	_currentTrackSlopeEnd = 0;
	RCT2_GLOBAL(0x00F440B3, uint8) = 0;
	_currentTrackLiftHill = 0;
	_currentTrackCovered = 0;

	if (RCT2_GLOBAL(0x0097D4F2 + (ride->type * 8), uint16) & 0x8000)
		_currentTrackCovered |= 2;

	_previousTrackBankEnd = 0;
	_previousTrackSlopeEnd = 0;

	_currentTrackPieceDirection = 0;
	_rideConstructionState = RIDE_CONSTRUCTION_STATE_PLACE;
	_currentTrackSelectionFlags = 0;
	_rideConstructionArrowPulseTime = 0;
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

	// Remove all rides if scenario editor
	if (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_SCENARIO_EDITOR) {
		if (s6Info->var_000 <= 2)
			FOR_ALL_RIDES(i, ride)
				ride->type = RIDE_TYPE_NULL;
		return;
	}

	window_update_viewport_ride_music();

	// Update rides
	FOR_ALL_RIDES(i, ride)
		ride_update(i);

	ride_music_update_final();
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
		ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_CUSTOMER;

		ride->income_per_hour = ride_calculate_income_per_hour(ride);
		ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

		if (ride->upkeep_cost != (money16)0xFFFF)
			ride->profit = (ride->income_per_hour - ((money32)ride->upkeep_cost * 16));
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

	if (ride->status == RIDE_STATUS_TESTING && gConfigGeneral.no_test_crashes) {
		for (int i = 0; i < ride->num_vehicles; i++) {
			rct_vehicle *vehicle = &(g_sprite_list[ride->vehicles[i]].vehicle);

			if (vehicle->status == VEHICLE_STATUS_CRASHED || vehicle->status == VEHICLE_STATUS_CRASHING) {
				ride_set_status(rideIndex, RIDE_STATUS_CLOSED);
				ride_set_status(rideIndex, RIDE_STATUS_CLOSED);
				ride_set_status(rideIndex, RIDE_STATUS_TESTING);
				break;
			}
		}
	}
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

	ax = ride->operation_option * 2048;
	bx = ride->var_148;
	cx = bx + ax;
	ride->var_148 = cx;
	if (bx >> 14 == cx >> 14)
		return;

	x = (ride->var_13A & 0xFF) * 32;
	y = (ride->var_13A >> 8) * 32;
	z = ride->var_13E * 8;
	map_invalidate_tile_zoom1(x, y, z, z + (4 * 8));

	x = (ride->var_13C & 0xFF) * 32;
	y = (ride->var_13C >> 8) * 32;
	z = ride->var_13F * 8;
	map_invalidate_tile_zoom1(x, y, z, z + (4 * 8));
}

/**
 * rct2: 0x0069A3A2
 * edi: ride (in code as bytes offset from start of rides list)
 * bl: happiness
 */
void ride_update_satisfaction(rct_ride* ride, uint8 happiness) {
	ride->satisfaction_next += happiness;
	ride->satisfaction_time_out++;
	if (ride->satisfaction_time_out >= 20) {
		ride->satisfaction = ride->satisfaction_next >> 2;
		ride->satisfaction_next = 0;
		ride->satisfaction_time_out = 0;
		ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_CUSTOMER;

	}
}

/* rct2: 0x0069A3D7
 * Updates the ride popularity
 * edi : ride
 * bl  : pop_amount
 * pop_amount can be zero if peep visited but did not purchase.
 */
void ride_update_popularity(rct_ride* ride, uint8 pop_amount){
	ride->popularity_next += pop_amount;
	ride->popularity_time_out++;
	if (ride->popularity_time_out < 25)return;

	ride->popularity = ride->popularity_next;
	ride->popularity_next = 0;
	ride->popularity_time_out = 0;
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_CUSTOMER;
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
		peep->destination_x++;
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
		x *= 32;
		y *= 32;
		x += RCT2_GLOBAL(0x0098DDB8 + (rotation * 4), sint16);
		y += RCT2_GLOBAL(0x0098DDBA + (rotation * 4), sint16);

		map_invalidate_tile_zoom0(x, y, mapElement->base_height * 8, mapElement->clearance_height * 8);
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

static int get_age_penalty(rct_ride *ride) {
	int years;
	years = date_get_year(RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_MONTH_YEAR, uint16) - ride->build_date);
	switch (years) {
	case 0:
		return 0;
	case 1:
		return ride->unreliability_factor / 8;
	case 2:
		return ride->unreliability_factor / 4;
	case 3:
	case 4:
		return ride->unreliability_factor / 2;
	case 5:
	case 6:
	case 7:
		return ride->unreliability_factor;
	default:
		return ride->unreliability_factor * 2;
	}
}

/**
 *
 *  rct2: 0x006AC622
 */
static void ride_breakdown_update(int rideIndex)
{
	int breakdownReason, unreliabilityAccumulator;
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
			ride->var_19F +
			ride->var_1A0 +
			ride->var_1A2 +
			ride->var_1A3;
		ride->downtime = min(ax / 2, 100);

		ride->var_1A3 = ride->var_1A2;
		ride->var_1A2 = ride->var_1A1;
		ride->var_1A1 = ride->var_1A0;
		ride->var_1A0 = ride->var_19F;
		ride->var_19F = ride->var_19E;
		ride->var_19E = ride->var_19D;
		ride->var_19D = ride->var_19C;
		ride->var_19C = 0;
		ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_MAINTENANCE;
	}

	if (ride->lifecycle_flags & (RIDE_LIFECYCLE_BREAKDOWN_PENDING | RIDE_LIFECYCLE_BROKEN_DOWN | RIDE_LIFECYCLE_CRASHED))
		return;
	if (ride->status == RIDE_STATUS_CLOSED)
		return;

	// Calculate breakdown probability?
	unreliabilityAccumulator = ride->unreliability_factor + get_age_penalty(ride);
	ride->reliability = max(0, ride->reliability - unreliabilityAccumulator);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_MAINTENANCE;

	// Random probability of a breakdown. Roughly this is 1 in
	//
	// (25000 - reliability) / 3 000 000
	//
	// a 0.8% chance, less the breakdown factor which accumulates as the game
	// continues.
	if ((ride->reliability == 0 || (int)(scenario_rand() & 0x2FFFFF) <= 1 + RIDE_INITIAL_RELIABILITY - ride->reliability) && !gConfigCheat.disable_all_breakdowns) {
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
	if (entry->flags & RIDE_ENTRY_FLAG_14)
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

	// Brakes failure can not happen if block brakes are used (so long as there is more than one vehicle)
	// However if this is the case, brake failure should be taken out the equation, otherwise block brake
	// rides have a lower probability to break down due to a random implementation reason.
	if (ride->mode == RIDE_MODE_CONTINUOUS_CIRCUIT_BLOCK_SECTIONED || ride->mode == RIDE_MODE_POWERED_LAUNCH_BLOCK_SECTIONED)
		if (ride->num_vehicles != 1)
			return -1;

	// If brakes failure is disabled, also take it out of the equation (see above comment why)
	if (gConfigCheat.disable_brakes_failure)
		return -1;

	monthsOld = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_MONTH_YEAR, uint8) - ride->build_date;
	if (monthsOld < 16 || ride->reliability > (50 << 8) || ride->lifecycle_flags & RIDE_LIFECYCLE_SIX_FLAGS)
		return -1;

	return BREAKDOWN_BRAKES_FAILURE;
}

/**
 *
 *  rct2: 0x006B7348
 */
void ride_prepare_breakdown(int rideIndex, int breakdownReason)
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
		for (i = ride->broken_car; i > 0; i--) {
			if (vehicle->next_vehicle_on_train == (uint16)0xFFFFFFFF) {
				vehicle = NULL;
				break;
			}
			else {
				vehicle = &(g_sprite_list[vehicle->next_vehicle_on_train].vehicle);
			}
		}
		if (vehicle != NULL)
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
			ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_MAINTENANCE | RIDE_INVALIDATE_RIDE_LIST | RIDE_INVALIDATE_RIDE_MAIN;
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
			ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_MAINTENANCE;
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
			ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_MAINTENANCE;
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
	peep_decrement_num_riders(mechanic);
	mechanic->state = forInspection ? PEEP_STATE_HEADING_TO_INSPECTION : PEEP_STATE_ANSWERING;
	peep_window_state_update(mechanic);
	mechanic->sub_state = 0;
	ride->mechanic_status = RIDE_MECHANIC_STATUS_HEADING;
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_MAINTENANCE;
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

		if (!forInspection) {
			if (peep->state == PEEP_STATE_HEADING_TO_INSPECTION){
				if (peep->sub_state >= 4)
					continue;
			}
			else if (peep->state != PEEP_STATE_PATROLLING)
				continue;

			if (!(peep->staff_orders & STAFF_ORDERS_FIX_RIDES))
				continue;
		} else {
			if (peep->state != PEEP_STATE_PATROLLING || !(peep->staff_orders & STAFF_ORDERS_INSPECT_RIDES))
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

#define MAKE_TUNEID_LIST(...) (uint8[]){(countof(((uint8[]){__VA_ARGS__}))), __VA_ARGS__}

//0x009AEF28
uint8 *ride_music_style_tuneids[] = {
	MAKE_TUNEID_LIST(13), // MUSIC_STYLE_DODGEMS_BEAT
	MAKE_TUNEID_LIST(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12), // MUSIC_STYLE_FAIRGROUND_ORGAN
	MAKE_TUNEID_LIST(15), // MUSIC_STYLE_ROMAN_FANFARE
	MAKE_TUNEID_LIST(16), // MUSIC_STYLE_ORIENTAL
	MAKE_TUNEID_LIST(17), // MUSIC_STYLE_MARTIAN
	MAKE_TUNEID_LIST(18), // MUSIC_STYLE_JUNGLE_DRUMS
	MAKE_TUNEID_LIST(19), // MUSIC_STYLE_EGYPTIAN
	MAKE_TUNEID_LIST(20), // MUSIC_STYLE_TOYLAND
	MAKE_TUNEID_LIST(21), // MUSIC_STYLE_8
	MAKE_TUNEID_LIST(22), // MUSIC_STYLE_SPACE
	MAKE_TUNEID_LIST(23), // MUSIC_STYLE_HORROR
	MAKE_TUNEID_LIST(24), // MUSIC_STYLE_TECHNO
	MAKE_TUNEID_LIST(25), // MUSIC_STYLE_GENTLE
	MAKE_TUNEID_LIST(26), // MUSIC_STYLE_SUMMER
	MAKE_TUNEID_LIST(27), // MUSIC_STYLE_WATER
	MAKE_TUNEID_LIST(28), // MUSIC_STYLE_WILD_WEST
	MAKE_TUNEID_LIST(29), // MUSIC_STYLE_JURASSIC
	MAKE_TUNEID_LIST(30), // MUSIC_STYLE_ROCK
	MAKE_TUNEID_LIST(31), // MUSIC_STYLE_RAGTIME
	MAKE_TUNEID_LIST(32), // MUSIC_STYLE_FANTASY
	MAKE_TUNEID_LIST(33), // MUSIC_STYLE_ROCK_STYLE_2
	MAKE_TUNEID_LIST(34), // MUSIC_STYLE_ICE
	MAKE_TUNEID_LIST(35), // MUSIC_STYLE_SNOW
	MAKE_TUNEID_LIST(36), // MUSIC_STYLE_CUSTOM_MUSIC_1
	MAKE_TUNEID_LIST(37), // MUSIC_STYLE_CUSTOM_MUSIC_2
	MAKE_TUNEID_LIST(38), // MUSIC_STYLE_MEDIEVAL
	MAKE_TUNEID_LIST(39), // MUSIC_STYLE_URBAN
	MAKE_TUNEID_LIST(40), // MUSIC_STYLE_ORGAN
	MAKE_TUNEID_LIST(41), // MUSIC_STYLE_MECHANICAL
	MAKE_TUNEID_LIST(42), // MUSIC_STYLE_MODERN
	MAKE_TUNEID_LIST(43), // MUSIC_STYLE_PIRATES
	MAKE_TUNEID_LIST(44), // MUSIC_STYLE_ROCK_STYLE_3
	MAKE_TUNEID_LIST(45), // MUSIC_STYLE_CANDY_STYLE
};

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
		uint8 *musicStyleTunes = ride_music_style_tuneids[ride->music];
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

	ride->music_position = ride_music_params_update(x, y, z, rideIndex, sampleRate, ride->music_position, &ride->music_tune_id);
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
		if (measurement->current_station == vehicle->current_station)
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
					measurement->current_station = vehicle->current_station;
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

int ride_get_free_measurement()
{
	int i;
	rct_ride_measurement *measurement;

	for (i = 0; i < MAX_RIDE_MEASUREMENTS; i++) {
		measurement = GET_RIDE_MEASUREMENT(i);
		if (measurement->ride_index == 255)
			return i;
	}

	return -1;
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
	if (!ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_HAS_DATA_LOGGING)) {
		if (message != NULL) *message = STR_DATA_LOGGING_NOT_AVAILABLE_FOR_THIS_TYPE_OF_RIDE;
		return NULL;
	}

	// Check if a measurement already exists for this ride
	measurement = ride_get_existing_measurement(rideIndex);
	if (measurement == NULL) {
		// Find a free measurement
		i = ride_get_free_measurement();
		if (i == -1) {
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
			GET_RIDE(measurement->ride_index)->measurement_index = 255;
		} else {
			measurement = GET_RIDE_MEASUREMENT(i);
		}
		
		measurement->ride_index = rideIndex;
		ride->measurement_index = i;
		measurement->flags = 0;
		if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_HAS_G_FORCES))
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
	result.main = ride->vehicle_colours[vehicleIndex].body_colour;
	result.additional_1 = ride->vehicle_colours[vehicleIndex].trim_colour;
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

		if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_IS_SHOP))
			ride_shop_connected(ride, i);
		else
			ride_entrance_exit_connected(ride, i);
	}
}

/**
 * rct2: 0x006B7C59
 * @return 1 if the coordinate is reachable or has no entrance, 0 otherwise
 */
static int ride_entrance_exit_is_reachable(uint16 coordinate, rct_ride* ride, int index)
{
	int x, y, z;
	rct_map_element *mapElement;

	x = coordinate & 0xFF;
	y = (coordinate >> 8) & 0xFF;
	z = ride->station_heights[index];
	mapElement = map_get_first_element_at(x, y);

	for (;;) {
		if (map_element_get_type(mapElement) == MAP_ELEMENT_TYPE_ENTRANCE && z == mapElement->base_height) {
			break;
		} else if (map_element_is_last_for_tile(mapElement)) {
			return 1;
		}
		mapElement++;
	}

	uint8 face_direction = mapElement->type & 3;

	x *= 32;
	y *= 32;
	x -= RCT2_ADDRESS(0x00993CCC, sint16)[face_direction * 2];
	y -= RCT2_ADDRESS(0x00993CCE, sint16)[face_direction * 2];
	x /= 32;
	y /= 32;

	return map_coord_is_connected(x, y, z, face_direction);
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
	int x, y, count;
	rct_map_element *mapElement;

    uint16 coordinate = ride->station_starts[0];
	if (coordinate == 0xFFFF)
		return;

	x = (coordinate & 0xFF);
	y = (coordinate >> 8) & 0xFF;
	
	mapElement = map_get_first_element_at(x, y);
	do {
		if (map_element_get_type(mapElement) == MAP_ELEMENT_TYPE_TRACK && mapElement->properties.track.ride_index == ride_idx)
			break;
	} while (!map_element_is_last_for_tile(mapElement++));

	uint16 entrance_directions = 0;
	uint8 track_type = mapElement->properties.track.type;
	ride = &g_ride_list[mapElement->properties.track.ride_index];
	if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_FLAT_RIDE)) {
		entrance_directions = RCT2_ADDRESS(0x0099CA64, uint8)[track_type * 16];
	} else {
		entrance_directions = RCT2_ADDRESS(0x0099BA64, uint8)[track_type * 16];
	}

	uint8 tile_direction = mapElement->type & MAP_ELEMENT_DIRECTION_MASK;
	entrance_directions <<= tile_direction;
	entrance_directions = ((entrance_directions >> 12) | entrance_directions) & 0xF;

	// Now each bit in entrance_directions stands for an entrance direction to check
	if (entrance_directions == 0)
		return;

	// Turn x, y from tiles into units
	x *= 32;
	y *= 32;

	for (count = 0; entrance_directions != 0; count++) {
		if (!(entrance_directions & 1)) {
			entrance_directions >>= 1;
			continue;
		}
		entrance_directions >>= 1;

		// Flip direction north<->south, east<->west
		uint8 face_direction = count ^ 2;

		int y2 = y - RCT2_ADDRESS(0x00993CCE, sint16)[face_direction * 2];
		int x2 = x - RCT2_ADDRESS(0x00993CCC, sint16)[face_direction * 2];

		if (map_coord_is_connected(x2 / 32, y2 / 32, mapElement->base_height, face_direction))
			return;
	}

	// Name of ride is parameter of the format string
	RCT2_GLOBAL(0x013CE952, uint16) = ride->name;
	RCT2_GLOBAL(0x013CE954, uint32) = ride->name_arguments;
	news_item_add_to_queue(1, STR_ENTRANCE_NOT_CONNECTED, ride_idx);

	ride->connected_message_throttle = 3;
}

#pragma endregion

#pragma region Interface

static void ride_track_set_map_tooltip(rct_map_element *mapElement)
{
	int rideIndex;
	rct_ride *ride;

	rideIndex = mapElement->properties.track.ride_index;
	ride = GET_RIDE(rideIndex);

	RCT2_GLOBAL(RCT2_ADDRESS_MAP_TOOLTIP_ARGS + 0, uint16) = 2215;
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_TOOLTIP_ARGS + 2, uint16) = ride->name;
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_TOOLTIP_ARGS + 4, uint32) = ride->name_arguments;

	int arg0, arg1;
	ride_get_status(rideIndex, &arg0, &arg1);
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_TOOLTIP_ARGS + 8, uint16) = (uint16)arg0;
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_TOOLTIP_ARGS + 10, uint32) = arg1;
}

static void ride_station_set_map_tooltip(rct_map_element *mapElement)
{
	int i, rideIndex, stationIndex;
	rct_ride *ride;

	rideIndex = mapElement->properties.track.ride_index;
	ride = GET_RIDE(rideIndex);

	stationIndex = map_get_station(mapElement);
	for (i = stationIndex; i >= 0; i--)
		if (ride->station_starts[i] == 0xFFFF)
			stationIndex--;

	RCT2_GLOBAL(RCT2_ADDRESS_MAP_TOOLTIP_ARGS + 0, uint16) = 2215;
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_TOOLTIP_ARGS + 2, uint16) = ride->num_stations <= 1 ? 1333 : 1334;
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_TOOLTIP_ARGS + 4, uint16) = ride->name;
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_TOOLTIP_ARGS + 6, uint32) = ride->name_arguments;
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_TOOLTIP_ARGS + 10, uint16) = RideNameConvention[ride->type].station_name + 2;
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_TOOLTIP_ARGS + 12, uint16) = stationIndex + 1;

	int arg0, arg1;
	ride_get_status(rideIndex, &arg0, &arg1);
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_TOOLTIP_ARGS + 14, uint16) = (uint16)arg0;
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_TOOLTIP_ARGS + 16, uint32) = arg1;
}

static void ride_entrance_set_map_tooltip(rct_map_element *mapElement)
{
	int i, rideIndex, stationIndex, queueLength;
	rct_ride *ride;

	rideIndex = mapElement->properties.track.ride_index;
	ride = GET_RIDE(rideIndex);

	// Get the station
	stationIndex = map_get_station(mapElement);
	for (i = stationIndex; i >= 0; i--)
		if (ride->station_starts[i] == 0xFFFF)
			stationIndex--;

	if (mapElement->properties.entrance.type == ENTRANCE_TYPE_RIDE_ENTRANCE) {
		// Get the queue length
		queueLength = 0;
		if (ride->entrances[stationIndex] != 0xFFFF)
			queueLength = ride->queue_length[stationIndex];

		RCT2_GLOBAL(RCT2_ADDRESS_MAP_TOOLTIP_ARGS + 0, uint16) = 2215;
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_TOOLTIP_ARGS + 2, uint16) = ride->num_stations <= 1 ? 1335 : 1336;
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_TOOLTIP_ARGS + 4, uint16) = ride->name;
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_TOOLTIP_ARGS + 6, uint32) = ride->name_arguments;
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_TOOLTIP_ARGS + 12, uint16) = stationIndex + 1;
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_TOOLTIP_ARGS + 14, uint16) =
			queueLength == 0 ?
				1201 :
				queueLength == 1 ?
					1202 :
					1203;
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_TOOLTIP_ARGS + 16, uint16) = queueLength;
	} else {
		// Get the station
		stationIndex = map_get_station(mapElement);
		for (i = stationIndex; i >= 0; i--)
			if (ride->station_starts[i] == 0xFFFF)
				stationIndex--;

		RCT2_GLOBAL(RCT2_ADDRESS_MAP_TOOLTIP_ARGS + 0, uint16) = ride->num_stations <= 1 ? 1337 : 1338;
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_TOOLTIP_ARGS + 2, uint16) = ride->name;
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_TOOLTIP_ARGS + 4, uint32) = ride->name_arguments;
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_TOOLTIP_ARGS + 10, uint16) = stationIndex + 1;
	}
}

void ride_set_map_tooltip(rct_map_element *mapElement)
{
	if (map_element_get_type(mapElement) == MAP_ELEMENT_TYPE_ENTRANCE) {
		ride_entrance_set_map_tooltip(mapElement);
	} else {
		if (
			mapElement->properties.track.type == 2 ||
			mapElement->properties.track.type == 3 ||
			mapElement->properties.track.type == 1
		) {
			ride_station_set_map_tooltip(mapElement);
		} else {
			ride_track_set_map_tooltip(mapElement);
		}
	}
}

/**
 * 
 *  rct2: 0x006BC3AC
 * Update ride music parameters
 * @param x (ax)
 * @param y (cx)
 * @param z (dx)
 * @param sampleRate (di)
 * @param rideIndex (bl)
 * @param position (ebp)
 * @param tuneId (bh)
 * @returns new position (ebp)
 */
int ride_music_params_update(sint16 x, sint16 y, sint16 z, uint8 rideIndex, uint16 sampleRate, uint32 position, uint8 *tuneId)
{
	if(!(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_SCENARIO_EDITOR) && !gGameSoundsOff && RCT2_GLOBAL(0x00F438A4, rct_viewport*) != (rct_viewport*)-1) {
		RCT2_GLOBAL(0x009AF47C, uint16) = sampleRate;
		sint16 v11;
		sint16 v12;
		switch (RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_ROTATION, uint32)) {
			case 0:
				v11 = y - x;
				v12 = ((y + x) / 2) - z;
				break;
			case 1:
				v11 = -x - y;
				v12 = ((y - x) / 2) - z;
				break;
			case 2:
				v11 = x - y;
				v12 = ((-y - x) / 2) - z;
				break;
			case 3:
				v11 = y + x;
				v12 = ((x - y) / 2) - z;
				break;
		}
		RCT2_GLOBAL(0x009AF5A0, sint16) = v11;
		RCT2_GLOBAL(0x009AF5A2, sint16) = v12;
		rct_viewport* viewport = RCT2_GLOBAL(0x00F438A4, rct_viewport*);
		sint16 view_width = RCT2_GLOBAL(0x00F438A4, rct_viewport*)->view_width;
		sint16 view_width2 = view_width * 2;
		sint16 view_x = RCT2_GLOBAL(0x00F438A4, rct_viewport*)->view_x - view_width2;
		sint16 view_y = RCT2_GLOBAL(0x00F438A4, rct_viewport*)->view_y - view_width;
		sint16 v16 = view_width2 + view_width2 + RCT2_GLOBAL(0x00F438A4, rct_viewport*)->view_width + view_x;
		sint16 v17 = view_width + view_width + RCT2_GLOBAL(0x00F438A4, rct_viewport*)->view_height + view_y;
		if (view_x >= RCT2_GLOBAL(0x009AF5A0, sint16) ||
			view_y >= RCT2_GLOBAL(0x009AF5A2, sint16) ||
			v16 < RCT2_GLOBAL(0x009AF5A0, sint16) ||
			v17 < RCT2_GLOBAL(0x009AF5A2, sint16)) {
				goto label58;
		}
		int x2 = RCT2_GLOBAL(0x00F438A4, rct_viewport*)->x + ((RCT2_GLOBAL(0x009AF5A0, sint16) - RCT2_GLOBAL(0x00F438A4, rct_viewport*)->view_x) >> RCT2_GLOBAL(0x00F438A4, rct_viewport*)->zoom);
		x2 <<= 16;
		uint16 screenwidth = RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_WIDTH, uint16);
		if (screenwidth < 64) {
			screenwidth = 64;
		}
		int panx = ((x2 / screenwidth) - 0x8000) >> 4;

		int y2 = RCT2_GLOBAL(0x00F438A4, rct_viewport*)->y + ((RCT2_GLOBAL(0x009AF5A2, sint16) - RCT2_GLOBAL(0x00F438A4, rct_viewport*)->view_y) >> RCT2_GLOBAL(0x00F438A4, rct_viewport*)->zoom);
		y2 <<= 16;
		uint16 screenheight = RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_HEIGHT, uint16);
		if (screenheight < 64) {
			screenheight = 64;
		}
		int pany = ((y2 / screenheight) - 0x8000) >> 4;

		uint8 vol1 = -1;
		uint8 vol2 = -1;
		int panx2 = panx;
		int pany2 = pany;
		if (pany2 < 0) {
			pany2 = -pany2;
		}
		if (pany2 > 6143) {
			pany2 = 6143;
		}
		pany2 -= 2048;
		if (pany2 > 0) {
			pany2 = -((pany2 / 4) - 1024) / 4;
			vol1 = (uint8)pany2;
			if (pany2 >= 256) {
				vol1 = -1;
			}
		}

		if (panx2 < 0) {
			panx2 = -panx2;
		}
		if (panx2 > 6143) {
			panx2 = 6143;
		}
		panx2 -= 2048;
		if (panx2 > 0) {
			panx2 = -((panx2 / 4) - 1024) / 4;
			vol2 = (uint8)panx2;
			if (panx2 >= 256) {
				vol2 = -1;
			}
		}
		if (vol1 >= vol2) {
			vol1 = vol2;
		}
		if (vol1 < RCT2_GLOBAL(RCT2_ADDRESS_VOLUME_ADJUST_ZOOM, uint8) * 3) {
			vol1 = 0;
		} else {
			vol1 = vol1 - (RCT2_GLOBAL(RCT2_ADDRESS_VOLUME_ADJUST_ZOOM, uint8) * 3);
		}
		int v32 = -(((uint8)(-vol1 - 1) * (uint8)(-vol1 - 1)) / 16) - 700;
		if (vol1 && v32 >= -4000) {
			if (panx > 10000) {
				panx = 10000;
			}
			if (panx < -10000) {
				panx = -10000;
			}
			if (!RCT2_GLOBAL(0x009AAC6D, uint8)) {
				panx = 0;
			}
			rct_ride_music* ride_music = &gRideMusicList[0];//&RCT2_GLOBAL(0x009AF46C, rct_ride_music);
			int channel = 0;
			uint32 a1;
			while (ride_music->rideid != rideIndex || ride_music->tuneid != *tuneId) {
				ride_music++;
				channel++;
				if (channel >= AUDIO_MAX_RIDE_MUSIC) {
					rct_ride_music_info* ride_music_info = ride_music_info_list[*tuneId];
					a1 = position + ride_music_info->offset;
					goto label51;
				}
			}
#ifdef USE_MIXER
			int playing = Mixer_Channel_IsPlaying(gRideMusicList[channel].sound_channel);
#else
			RCT2_GLOBAL(0x014241BC, uint32) = 1;
			int playing = sound_channel_is_playing(channel);
			RCT2_GLOBAL(0x014241BC, uint32) = 0;
#endif
			if (!playing) {
				*tuneId = 0xFF;
				return 0;
			}
#ifdef USE_MIXER
			a1 = Mixer_Channel_GetOffset(gRideMusicList[channel].sound_channel);
#else
			RCT2_GLOBAL(0x014241BC, uint32) = 1;
			a1 = sub_401B46(channel);
			RCT2_GLOBAL(0x014241BC, uint32) = 0;
#endif
		label51:
			if (a1 < ride_music_info_list[*tuneId]->length) {
				position = a1;
				rct_ride_music_params* ride_music_params = gRideMusicParamsListEnd;//RCT2_GLOBAL(0x009AF42C, rct_ride_music_params*);
				if (ride_music_params < &gRideMusicParamsList[AUDIO_MAX_RIDE_MUSIC]/*(rct_ride_music_params*)0x009AF46C*/) {
					ride_music_params->rideid = rideIndex;
					ride_music_params->tuneid = *tuneId;
					ride_music_params->offset = a1;
					ride_music_params->volume = v32;
					ride_music_params->pan = panx;
					ride_music_params->freq = RCT2_GLOBAL(0x009AF47C, uint16);
					gRideMusicParamsListEnd++;//RCT2_GLOBAL(0x009AF42C, rct_ride_music_params*)++;
				}
			} else {
				*tuneId = 0xFF;
				return 0;
			}
		} else {
		label58:
			;
			rct_ride_music_info* ride_music_info = ride_music_info_list[*tuneId];
			position += ride_music_info->offset;
			if (position < ride_music_info->length) {
				return position;
			} else {
				*tuneId = 0xFF;
				return 0;
			}
		}
	}
	return position;
}

#define INIT_MUSIC_INFO(pathid, offset, length, unknown) (rct_ride_music_info[]){length, offset, pathid, unknown}

//0x009AF1C8
rct_ride_music_info* ride_music_info_list[] = {
	INIT_MUSIC_INFO(PATH_ID_CSS4, 1378, 8139054, 0),
	INIT_MUSIC_INFO(PATH_ID_CSS5, 1378, 7796656, 0),
	INIT_MUSIC_INFO(PATH_ID_CSS6, 1378, 15787850, 0),
	INIT_MUSIC_INFO(PATH_ID_CSS7, 1378, 15331658, 0),
	INIT_MUSIC_INFO(PATH_ID_CSS8, 1378, 17503414, 0),
	INIT_MUSIC_INFO(PATH_ID_CSS9, 1378, 7005802, 0),
	INIT_MUSIC_INFO(PATH_ID_CSS10, 1378, 0, 0),
	INIT_MUSIC_INFO(PATH_ID_CSS11, 1378, 7023288, 0),
	INIT_MUSIC_INFO(PATH_ID_CSS12, 1378, 2767948, 0),
	INIT_MUSIC_INFO(PATH_ID_CSS13, 1378, 3373390, 0),
	INIT_MUSIC_INFO(PATH_ID_CSS14, 1378, 20783042, 0),
	INIT_MUSIC_INFO(PATH_ID_CSS15, 1378, 10009312, 0),
	INIT_MUSIC_INFO(PATH_ID_CSS16, 1378, 0, 0),
	INIT_MUSIC_INFO(PATH_ID_CSS3, 689, 1244886, 1),
	INIT_MUSIC_INFO(PATH_ID_CSS17, 2756, -1, 0),
	INIT_MUSIC_INFO(PATH_ID_CSS18, 2756, 8429568, 1),
	INIT_MUSIC_INFO(PATH_ID_CSS19, 2756, 10143784, 1),
	INIT_MUSIC_INFO(PATH_ID_CSS20, 2756, 12271656, 1),
	INIT_MUSIC_INFO(PATH_ID_CSS21, 2756, 9680968, 1),
	INIT_MUSIC_INFO(PATH_ID_CSS22, 2756, 10062056, 1),
	INIT_MUSIC_INFO(PATH_ID_CSS23, 2756, 11067432, 1),
	INIT_MUSIC_INFO(PATH_ID_CSS24, 2756, 12427456, 0),
	INIT_MUSIC_INFO(PATH_ID_CSS25, 2756, 15181512, 1),
	INIT_MUSIC_INFO(PATH_ID_CSS26, 2756, 10694816, 1),
	INIT_MUSIC_INFO(PATH_ID_CSS27, 2756, 10421232, 1),
	INIT_MUSIC_INFO(PATH_ID_CSS28, 2756, 13118376, 1),
	INIT_MUSIC_INFO(PATH_ID_CSS29, 2756, 15310892, 1),
	INIT_MUSIC_INFO(PATH_ID_CSS30, 2756, 10215464, 1),
	INIT_MUSIC_INFO(PATH_ID_CSS31, 2756, 11510316, 1),
	INIT_MUSIC_INFO(PATH_ID_CSS32, 2756, 11771944, 1),
	INIT_MUSIC_INFO(PATH_ID_CSS33, 2756, 10759724, 1),
	INIT_MUSIC_INFO(PATH_ID_CSS34, 2756, 14030716, 1),
	INIT_MUSIC_INFO(PATH_ID_CSS35, 2756, 11642576, 1),
	INIT_MUSIC_INFO(PATH_ID_CSS36, 2756, 8953764, 1),
	INIT_MUSIC_INFO(PATH_ID_CSS37, 2756, 13303852, 1),
	INIT_MUSIC_INFO(PATH_ID_CSS38, 2756, 10093888, 1),
	INIT_MUSIC_INFO(PATH_ID_CUSTOM1, 2756, 0, 1),
	INIT_MUSIC_INFO(PATH_ID_CUSTOM2, 2756, 0, 1),
	INIT_MUSIC_INFO(PATH_ID_CSS39, 2756, 7531564, 1),
	INIT_MUSIC_INFO(PATH_ID_CSS40, 1378, 5291306, 1),
	INIT_MUSIC_INFO(PATH_ID_CSS41, 2756, 27860700, 1),
	INIT_MUSIC_INFO(PATH_ID_CSS42, 2756, 6774090, 1),
	INIT_MUSIC_INFO(PATH_ID_CSS43, 2756, 15630412, 1),
	INIT_MUSIC_INFO(PATH_ID_CSS44, 2756, 8209704, 1),
	INIT_MUSIC_INFO(PATH_ID_CSS45, 2756, 10006740, 1),
	INIT_MUSIC_INFO(PATH_ID_CSS46, 2756, 6772776, 1),
};

/**
*  Play/update ride music based on structs updated in 0x006BC3AC
*  rct2: 0x006BC6D8
*/
void ride_music_update_final()
{
	rct_ride_music_params* edi;
	int ebx;
	if (!(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & 2)) {
		if ((RCT2_GLOBAL(0x009AF284, uint32) & (1 << 0))) {
			if (!gGameSoundsOff && gConfigSound.sound && gConfigSound.ride_music && !(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & 1)) {
				// set to stop music if volume <= 1 ?
				while (1) {
					int v8 = 0;
					int v9 = 1;
					rct_ride_music_params* ride_music_params = &gRideMusicParamsList[0];//&RCT2_GLOBAL(0x009AF430, rct_ride_music_params);
					while (ride_music_params < gRideMusicParamsListEnd/*RCT2_GLOBAL(0x009AF42C, rct_ride_music_params*)*/) {
						if (ride_music_params->rideid != (uint8)-1) {
							rct_ride_music_info* ride_music_info = ride_music_info_list[ride_music_params->tuneid];
							if (RCT2_ADDRESS(0x009AA0B1, uint8*)[ride_music_info->pathid]) { // file_on_cdrom[]
								v8++;
								if (v9 >= ride_music_params->volume) {
									v9 = ride_music_params->volume;
									edi = ride_music_params;
								}
							}
						}
						ride_music_params++;
					}
					if (v8 <= 1) {
						break;
					}
					edi->rideid = -1;
				}
				while (1) {
					int v8 = 0;
					int v9 = 1;
					rct_ride_music_params* ride_music_params = &gRideMusicParamsList[0];//&RCT2_GLOBAL(0x009AF430, rct_ride_music_params);
					while (ride_music_params < gRideMusicParamsListEnd/*RCT2_GLOBAL(0x009AF42C, rct_ride_music_params*)*/) {
						if (ride_music_params->rideid != (uint8)-1) {
							v8++;
							if (v9 >= ride_music_params->volume) {
								v9 = ride_music_params->volume;
								edi = ride_music_params;
							}
						}
						ride_music_params++;
					}
					if (v8 <= 2) {
						break;
					}
					edi->rideid = -1;
				}

				// stop currently playing music that is not in music params list or not playing?
				rct_ride_music* ride_music = &gRideMusicList[0];//&RCT2_GLOBAL(0x009AF46C, rct_ride_music);
				int channel = 0;
				do {
					if (ride_music->rideid != (uint8)-1) {
						rct_ride_music_params* ride_music_params = &gRideMusicParamsList[0];//&RCT2_GLOBAL(0x009AF430, rct_ride_music_params);
						while (ride_music_params < gRideMusicParamsListEnd/*RCT2_GLOBAL(0x009AF42C, rct_ride_music_params*)*/) {
							if (ride_music_params->rideid == ride_music->rideid && ride_music_params->tuneid == ride_music->tuneid) {
#ifdef USE_MIXER
								int isplaying = Mixer_Channel_IsPlaying(gRideMusicList[channel].sound_channel);
#else
								RCT2_GLOBAL(0x014241BC, uint32) = 1;
								int isplaying = sound_channel_is_playing(channel);
								RCT2_GLOBAL(0x014241BC, uint32) = 0;
#endif
								if (isplaying) {
									goto label32;
								}
								break;
							}
							ride_music_params++;
						}
#ifdef USE_MIXER
						Mixer_Stop_Channel(gRideMusicList[channel].sound_channel);
#else
						RCT2_GLOBAL(0x014241BC, uint32) = 1;
						sound_channel_stop(channel);
						RCT2_GLOBAL(0x014241BC, uint32) = 0;
#endif
						ride_music->rideid = -1;
					}
				label32:
					ride_music++;
					channel++;
				} while(channel < AUDIO_MAX_RIDE_MUSIC);

				for (rct_ride_music_params* ride_music_params = &gRideMusicParamsList[0]/*&RCT2_GLOBAL(0x009AF430, rct_ride_music_params)*/; ride_music_params < gRideMusicParamsListEnd/*RCT2_GLOBAL(0x009AF42C, rct_ride_music_params*)*/; ride_music_params++) {
					if (ride_music_params->rideid != (uint8)-1) {
						rct_ride_music* ride_music = &gRideMusicList[0];//&RCT2_GLOBAL(0x009AF46C, rct_ride_music);
						int channel = 0;
						while (ride_music_params->rideid != ride_music->rideid || ride_music_params->tuneid != ride_music->tuneid) {
							if (ride_music->rideid == (uint8)-1) {
								ebx = channel;
							}
							ride_music++;
							channel++;
							if (channel >= AUDIO_MAX_RIDE_MUSIC) {
								rct_ride_music_info* ride_music_info = ride_music_info_list[ride_music_params->tuneid];
#ifdef USE_MIXER
								rct_ride_music* ride_music = &gRideMusicList[ebx];
								ride_music->sound_channel = Mixer_Play_Music(ride_music_info->pathid, MIXER_LOOP_NONE, true);
								if (ride_music->sound_channel) {
									ride_music->volume = ride_music_params->volume;
									ride_music->pan = ride_music_params->pan;
									ride_music->freq = ride_music_params->freq;
									ride_music->rideid = ride_music_params->rideid;
									ride_music->tuneid = ride_music_params->tuneid;
									Mixer_Channel_Volume(ride_music->sound_channel, DStoMixerVolume(ride_music->volume));
									Mixer_Channel_Pan(ride_music->sound_channel, DStoMixerPan(ride_music->pan));
									Mixer_Channel_Rate(ride_music->sound_channel, DStoMixerRate(ride_music->freq));
									int offset = ride_music_params->offset - 10000;
									if (offset < 0) {
										offset = 0;
									}
									Mixer_Channel_SetOffset(ride_music->sound_channel, offset);
								} else {
									//RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_MUSIC, uint8) = 0;
								}
#else
								const char* filename = get_file_path(ride_music_info->pathid);
								RCT2_GLOBAL(0x014241BC, uint32) = 3;
								HANDLE hfile = osinterface_file_open(filename);
								RCT2_GLOBAL(0x014241BC, uint32) = 0;
								if (hfile != INVALID_HANDLE_VALUE) {
									RCT2_GLOBAL(0x014241BC, uint32) = 3;
									osinterface_file_read(hfile, &RCT2_GLOBAL(0x009AF47E, uint32), 4);
									RCT2_GLOBAL(0x014241BC, uint32) = 3;
									osinterface_file_close(hfile);
									RCT2_GLOBAL(0x014241BC, uint32) = 0;
								}
								if (hfile == INVALID_HANDLE_VALUE || RCT2_GLOBAL(0x009AF47E, uint32) != 0x78787878) {
									int offset = ride_music_params->offset - 10000;
									if (offset < 0) {
										offset = 0;
									}
									RCT2_GLOBAL(0x014241BC, uint32) = 1;
									int musicloaded = sound_channel_load_file2(ebx, filename, offset & 0xFFFFFFF0);
									RCT2_GLOBAL(0x014241BC, uint32) = 0;
									if (musicloaded) {
										RCT2_GLOBAL(0x014241BC, uint32) = 1;
										int musicplayed = sound_channel_play(ebx, 0, ride_music_params->volume, ride_music_params->pan, ride_music_params->freq);
										RCT2_GLOBAL(0x014241BC, uint32) = 0;
										if (musicplayed) {
											rct_ride_music_info* ride_music_info = &RCT2_GLOBAL(0x009AF1C8, rct_ride_music_info*)[ride_music_params->tuneid];
											if (ride_music_info->var_9) {
												RCT2_GLOBAL(0x014241BC, uint32) = 1;
												sub_401AF3(ebx, get_file_path(ride_music_info->pathid), 1, 0);
												RCT2_GLOBAL(0x014241BC, uint32) = 0;
											}
											rct_ride_music* ride_music = &gRideMusicList[ebx];//&RCT2_ADDRESS(0x009AF46C, rct_ride_music)[ebx];
											ride_music->volume = ride_music_params->volume;
											ride_music->pan = ride_music_params->pan;
											ride_music->freq = ride_music_params->freq;
											ride_music->rideid = ride_music_params->rideid;
											ride_music->tuneid = ride_music_params->tuneid;
										}
									} else {
										RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_MUSIC, uint8) = 0;
									}
								}
#endif
								return;
							}
						}

						if (ride_music_params->volume != ride_music->volume) {
							ride_music->volume = ride_music_params->volume;
#ifdef USE_MIXER
							Mixer_Channel_Volume(ride_music->sound_channel, DStoMixerVolume(ride_music->volume));
#else
							RCT2_GLOBAL(0x014241BC, uint32) = 1;
							sound_channel_set_volume(channel, ride_music_params->volume);
							RCT2_GLOBAL(0x014241BC, uint32) = 0;
#endif
						}
						if (ride_music_params->pan != ride_music->pan) {
							ride_music->pan = ride_music_params->pan;
#ifdef USE_MIXER
							Mixer_Channel_Pan(ride_music->sound_channel, DStoMixerPan(ride_music->pan));
#else
							RCT2_GLOBAL(0x014241BC, uint32) = 1;
							sound_channel_set_pan(channel, ride_music_params->pan);
							RCT2_GLOBAL(0x014241BC, uint32) = 0;
#endif
						}
						if (ride_music_params->freq != ride_music->freq) {
							ride_music->freq = ride_music_params->freq;
#ifdef USE_MIXER
							Mixer_Channel_Rate(ride_music->sound_channel, DStoMixerRate(ride_music->freq));
#else
							RCT2_GLOBAL(0x014241BC, uint32) = 1;
							sound_channel_set_frequency(channel, ride_music_params->freq);
							RCT2_GLOBAL(0x014241BC, uint32) = 0;
#endif
						}

					}
				}
			}
		}
	}
}

#pragma endregion

/* rct2: 0x006B5559 */
void game_command_set_ride_setting(int *eax, int *ebx, int *ecx, int *edx, int *esi, int *edi, int *ebp)
{
	RCT2_GLOBAL(RCT2_ADDRESS_NEXT_EXPENDITURE_TYPE, uint8) = RCT_EXPENDITURE_TYPE_RIDE_RUNNING_COSTS * 4;
	
	uint8 ride_id = *edx & 0xFF;
	rct_ride* ride = GET_RIDE(ride_id);

	uint8 flags = *ebx & 0xFF;
	uint8 new_value = (*ebx >> 8) & 0xFF;

	uint8 setting = (*edx >> 8) & 0xFF;

	if (setting == 0){
		if (ride->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN){
			RCT2_GLOBAL(0x141E9AC, uint16) = 1796;
			*ebx = MONEY32_UNDEFINED;
			return;
		}

		if (ride->status != RIDE_STATUS_CLOSED){
			RCT2_GLOBAL(0x141E9AC, uint16) = 1006;
			*ebx = MONEY32_UNDEFINED;
			return;
		}
	}

	if (ride->lifecycle_flags & RIDE_LIFECYCLE_SIX_FLAGS){
		if (setting == 0 || setting == 4 || setting == 8 || setting == 9)
		{ 
			RCT2_GLOBAL(0x141E9AC, uint16) = 1797;
			*ebx = MONEY32_UNDEFINED;
			return;
		}
	}

	if (setting == 9 &&
		ride->lifecycle_flags & RIDE_LIFECYCLE_CABLE_LIFT &&
		new_value > 1){
		RCT2_GLOBAL(0x141E9AC, uint16) = 3141;
		*ebx = MONEY32_UNDEFINED;
		return;
	}

	if (flags == 0){
		*ebx = 0;
		return;
	}

	switch (setting){
	case 0:
		invalidate_test_results(ride_id);
		ride_clear_for_construction(ride_id);
		ride_remove_peeps(ride_id);

		rct_ride_type* ride_entry = GET_RIDE_ENTRY(ride->subtype);
		const uint8* available_modes = RideAvailableModes;

		for (int i = 0; i < ride->type; i++) {
			while (*(available_modes++) != 255) {}
		}
		if (ride_entry->flags & RIDE_ENTRY_FLAG_17){
			available_modes += 2;
		}

		uint8 default_mode = available_modes[0];
		for (; *available_modes != 0xFF; available_modes++){
			if (*available_modes == new_value)
				break;
		}

		if (*available_modes == 0xFF) new_value = default_mode;

		if (available_modes[1] == 0xFF){
			if (ride_entry->flags & RIDE_ENTRY_FLAG_15)
				new_value = default_mode;
		}

		ride->mode = new_value;
		ride_update_max_vehicles(ride_id);
		break;
	case 1:
		ride->depart_flags = new_value;
		break;
	case 2:
		ride->min_waiting_time = new_value;
		ride->max_waiting_time = max(new_value, ride->max_waiting_time);
		break;
	case 3:
		ride->max_waiting_time = new_value;
		ride->min_waiting_time = min(new_value, ride->min_waiting_time);
		break;
	case 4:
		invalidate_test_results(ride_id);
		ride->time_limit = new_value;
		break;
	case 5:
		ride->inspection_interval = new_value;
		break;
	case 6:
		ride->lifecycle_flags &= ~RIDE_LIFECYCLE_MUSIC;
		if (new_value){
			ride->lifecycle_flags |= RIDE_LIFECYCLE_MUSIC;
		}
		break;
	case 7:
		if (new_value != ride->music){
			ride->music = new_value;
			ride->music_tune_id = 0xFF;
		}
		break;
	case 8:
		if (new_value != ride->lift_hill_speed){
			ride->lift_hill_speed = new_value;
			invalidate_test_results(ride_id);
		}
		break;
	case 9:
		if (new_value != ride->num_circuits){
			ride->num_circuits = new_value;
			invalidate_test_results(ride_id);
		}
		break;
	}

	window_invalidate_by_number(WC_RIDE, ride_id);
	*ebx = 0;
}

/**
 * 
 *  rct2: 0x006B4CC1
 */
int ride_mode_check_valid_station_numbers(rct_ride *ride)
{
	uint8 no_stations = 0;
	for (uint8 station_index = 0; station_index < 4; ++station_index){
		if (ride->station_starts[station_index] != 0xFFFF)no_stations++;
	}

	switch (ride->mode){
	case RIDE_MODE_REVERSE_INCLINE_LAUNCHED_SHUTTLE:
	case RIDE_MODE_POWERED_LAUNCH_PASSTROUGH:
	case RIDE_MODE_POWERED_LAUNCH:
	case RIDE_MODE_LIM_POWERED_LAUNCH:
		if (no_stations <= 1) return 1;
		RCT2_GLOBAL(0x141E9AC, uint16) = 1015;
		return 0;
	case RIDE_MODE_SHUTTLE:
		if (no_stations >= 2) return 1;
		RCT2_GLOBAL(0x141E9AC, uint16) = 1016;
		return 0;
	}

	if (ride->type == RIDE_TYPE_GO_KARTS || ride->type == RIDE_TYPE_MINI_GOLF){
		if (no_stations <= 1) return 1;
		RCT2_GLOBAL(0x141E9AC, uint16) = 1015;
		return 0;
	}

	return 1;
}

/* returns stationIndex of first station on success
 * -1 on failure.
 */
int ride_mode_check_station_present(rct_ride* ride){
	int stationIndex = -1;
	for (int i = 0; i < 4; i++) {
		if (ride->station_starts[i] != 0xFFFF) {
			stationIndex = i;
			break;
		}
	}

	if (stationIndex == -1) {
		RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TEXT, uint16) = STR_NOT_YET_CONSTRUCTED;
		if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_15))
			return -1;

		if (ride->type == RIDE_TYPE_MAZE)
			return -1;

		RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TEXT, uint16) = STR_REQUIRES_A_STATION_PLATFORM;
		return -1;
	}

	return stationIndex;
}

/**
 * 
 *  rct2: 0x006B5872
 */
int ride_check_for_entrance_exit(int rideIndex)
{
	rct_ride* ride = GET_RIDE(rideIndex);

	if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_IS_SHOP))
		return 1;

	int i;
	uint8 entrance = 0;
	uint8 exit = 0;
	for (i = 0; i < 4; i++) {
		if (ride->station_starts[i] == 0xFFFF)
			continue;

		if (ride->entrances[i] != 0xFFFF) {
			entrance = 1;
		}
		
		if (ride->exits[i] != 0xFFFF) {
			exit = 1;
		}
	
		// If station start and no entrance/exit
		// Sets same error message as no entrance
		if (ride->exits[i] == 0xFFFF && ride->entrances[i] == 0xFFFF){
			entrance = 0;
			break;
		}
	}

	if (entrance == 0){
		RCT2_GLOBAL(0x141E9AC, uint16) = 1146;
		return 0;
	}

	if (exit == 0){
		RCT2_GLOBAL(0x141E9AC, uint16) = 1147;
		return 0;
	}

	return 1;
}

/**
 * 
 *  rct2: 0x006B5952
 */
void sub_6B5952(int rideIndex)
{
	rct_ride *ride = GET_RIDE(rideIndex);

	for (int i = 0; i < 4; i++) {
		uint16 xy = ride->entrances[i];
		if (xy == 0xFFFF)
			continue;

		int x = (xy & 0xFF) * 32;
		int y = (xy >> 8) * 32;
		int z = ride->station_heights[i];

		rct_map_element *mapElement = map_get_first_element_at(x / 32, y / 32);
		do {
			if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_ENTRANCE)
				continue;
			if (mapElement->base_height != z)
				continue;

			int direction = mapElement->type & MAP_ELEMENT_DIRECTION_MASK;
			footpath_chain_ride_queue(rideIndex, i, x, y, mapElement, direction ^ 2);
		} while (!map_element_is_last_for_tile(mapElement++));
	}
}

/**
 * 
 *  rct2: 0x006D3319
 */
int ride_check_block_brakes(rct_xy_element *input, rct_xy_element *output)
{
	rct_window *w;
	track_circuit_iterator it;
	int rideIndex, type;

	rideIndex = input->element->properties.track.ride_index;
	w = window_find_by_class(WC_RIDE_CONSTRUCTION);
	if (w != NULL && _rideConstructionState != RIDE_CONSTRUCTION_STATE_0 && _currentRideIndex == rideIndex)
		sub_6C9627();

	track_circuit_iterator_begin(&it, *input);
	while (track_circuit_iterator_next(&it)) {
		if (it.current.element->properties.track.type == 216) {
			type = it.last.element->properties.track.type;
			if (type == 1) {
				RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TEXT, uint16) = STR_BLOCK_BRAKES_CANNOT_BE_USED_DIRECTLY_AFTER_STATION;
				*output = it.current;
				return 0;
			}
			if (type == 216) {
				RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TEXT, uint16) = STR_BLOCK_BRAKES_CANNOT_BE_USED_DIRECTLY_AFTER_EACH_OTHER;
				*output = it.current;
				return 0;
			}
			if ((it.last.element->type & 0x80) && type != 209 && type != 210) {
				RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TEXT, uint16) = STR_BLOCK_BRAKES_CANNOT_BE_USED_DIRECTLY_AFTER_THE_TOP_OF_THIS_LIFT_HILL;
				*output = it.current;
				return 0;
			}
		}
	}
	if (!it.looped) {
		// Not sure why this is the case...
		RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TEXT, uint16) = STR_BLOCK_BRAKES_CANNOT_BE_USED_DIRECTLY_AFTER_STATION;
		*output = it.last;
		return 0;
	}

	return 1;
}

/**
 * Iterates along the track until an inversion (loop, corkscrew, barrel roll etc.) track piece is reached.
 * @param input The start track element and position.
 * @param output The first track element and position which is classified as an inversion.
 * @returns true if an inversion track piece is found, otherwise false.
 *  rct2: 0x006CB149
 */
bool ride_check_track_contains_inversions(rct_xy_element *input, rct_xy_element *output)
{
	rct_window *w;
	rct_ride *ride;
	int rideIndex, trackType;
	track_circuit_iterator it;

	rideIndex = input->element->properties.track.ride_index;
	ride = GET_RIDE(rideIndex);
	if (ride->type == RIDE_TYPE_MAZE)
		return true;

	w = window_find_by_class(WC_RIDE_CONSTRUCTION);
	if (w != NULL && _rideConstructionState != RIDE_CONSTRUCTION_STATE_0 && rideIndex == _currentRideIndex) {
		sub_6C9627();
	}

	track_circuit_iterator_begin(&it, *input);
	while (track_circuit_iterator_next(&it)) {
		trackType = output->element->properties.track.type;
		if (RCT2_ADDRESS(0x0099423C, uint16)[trackType] & 0x4000) {
			*output = it.current;
			return true;
		}
	}
	return false;
}

/**
 * Iterates along the track until a banked track piece is reached.
 * @param input The start track element and position.
 * @param output The first track element and position which is banked.
 * @returns true if a banked track piece is found, otherwise false.
 *  rct2: 0x006CB1D3
 */
bool ride_check_track_contains_banked(rct_xy_element *input, rct_xy_element *output)
{
	rct_window *w;
	rct_ride *ride;
	int rideIndex, trackType;
	track_circuit_iterator it;

	rideIndex = input->element->properties.track.ride_index;
	ride = GET_RIDE(rideIndex);
	if (ride->type == RIDE_TYPE_MAZE)
		return true;

	w = window_find_by_class(WC_RIDE_CONSTRUCTION);
	if (w != NULL && _rideConstructionState != RIDE_CONSTRUCTION_STATE_0 && rideIndex == _currentRideIndex) {
		sub_6C9627();
	}

	track_circuit_iterator_begin(&it, *input);
	while (track_circuit_iterator_next(&it)) {
		trackType = output->element->properties.track.type;
		if (RCT2_ADDRESS(0x0099423C, uint16)[trackType] & 0x8000) {
			*output = it.current;
			return true;
		}
	}
	return false;
}

/**
 * 
 *  rct2: 0x006CB25D
 */
int ride_check_station_length(rct_xy_element *input, rct_xy_element *output)
{
	rct_window* w = window_find_by_class(WC_RIDE_CONSTRUCTION);
	if (w != 0 && 
		_rideConstructionState != RIDE_CONSTRUCTION_STATE_0 &&
		_currentRideIndex == input->element->properties.track.ride_index){
		sub_6C9627();
	}

	output->x = input->x;
	output->y = input->y;
	output->element = input->element;
	track_begin_end trackBeginEnd;
	while (track_block_get_previous(output->x, output->y, output->element, &trackBeginEnd)){
		output->x = trackBeginEnd.begin_x;
		output->y = trackBeginEnd.begin_y;
		output->element = trackBeginEnd.begin_element;
	}

	int num_station_elements = 0;
	rct_xy_element last_good_station = *output;

	do{
		if (RCT2_ADDRESS(0x0099BA64, uint8)[output->element->properties.track.type * 16] & 0x10){
			num_station_elements++;
			last_good_station = *output;
		}
		else{
			if (num_station_elements == 0)
				continue;
			if (num_station_elements == 1){
				return 0;
			}
			num_station_elements = 0;
		}
	} while (track_block_get_next(output, output, NULL, NULL));

	// Prevent returning a pointer to a map element with no track.
	*output = last_good_station;
	if (num_station_elements == 1)
		return 0;

	return 1;
}

/**
 * 
 *  rct2: 0x006CB2DA
 */
bool ride_check_start_and_end_is_station(rct_xy_element *input, rct_xy_element *output)
{
	rct_window *w;
	rct_ride *ride;
	int rideIndex, trackType;
	rct_xy_element trackBack, trackFront;

	rideIndex = input->element->properties.track.ride_index;
	ride = GET_RIDE(rideIndex);

	w = window_find_by_class(WC_RIDE_CONSTRUCTION);
	if (w != NULL && _rideConstructionState != RIDE_CONSTRUCTION_STATE_0 && rideIndex == _currentRideIndex) {
		sub_6C9627();
	}

	// Check back of the track
	track_get_back(input, &trackBack);
	trackType = trackBack.element->properties.track.type;
	if (!(RCT2_GLOBAL(0x0099BA64 + (trackType * 16), uint32) & 0x10)) {
		return false;
	}
	ride->var_13A = (trackBack.x >> 5) | ((trackBack.y >> 5) << 8);
	ride->var_13E = trackBack.element->base_height;

	// Check front of the track
	track_get_front(input, &trackFront);
	trackType = trackFront.element->properties.track.type;
	if (!(RCT2_GLOBAL(0x0099BA64 + (trackType * 16), uint32) & 0x10)) {
		return false;
	}
	ride->var_13C = (trackFront.x >> 5) | ((trackFront.y >> 5) << 8);
	ride->var_13F = trackFront.element->base_height;
	return true;
}

/**
 * 
 *  rct2: 0x006B4D26
 */
void sub_6B4D26(int rideIndex, rct_xy_element *startElement)
{
	RCT2_CALLPROC_X(0x006B4D26, startElement->x, 0, startElement->y, rideIndex, (int)startElement->element, 0, 0); return;

	rct_xy_element currentElement;
	rct_ride *ride;
	int trackType;

	ride = GET_RIDE(rideIndex);
	if (ride->type == RIDE_TYPE_BOAT_RIDE) {

	} else if (ride->type != RIDE_TYPE_MAZE) {

	}

	if (
		(
			ride->mode == RIDE_MODE_CONTINUOUS_CIRCUIT_BLOCK_SECTIONED ||
			ride->mode == RIDE_MODE_POWERED_LAUNCH_BLOCK_SECTIONED
		) &&
		!(ride->lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK)
	) {
		// Set flag on track pieces where a train can start
		currentElement = *startElement;
		do {
			trackType = currentElement.element->properties.track.type;
			switch (trackType) {
			case TRACK_ELEM_END_STATION:
			case TRACK_ELEM_CABLE_LIFT_HILL:
			case TRACK_ELEM_25_DEG_UP_TO_FLAT:
			case TRACK_ELEM_60_DEG_UP_TO_FLAT:
			case TRACK_ELEM_DIAG_25_DEG_UP_TO_FLAT:	
			case TRACK_ELEM_DIAG_60_DEG_UP_TO_FLAT:	
			case 216:		// block brakes
				currentElement.element->flags &= ~(1 << 5);
				break;
			}
		} while (track_block_get_next(&currentElement, &currentElement, NULL, NULL) && currentElement.element != startElement->element);
	}
}

/**
 * 
 *  rct2: 0x006DD84C
 */
int sub_6DD84C(rct_ride *ride, int rideIndex, rct_xy_element *element, int isApplying)
{
	return RCT2_CALLPROC_X(0x006DD84C, element->x, isApplying, element->y, rideIndex, (int)ride, (int)element->element, 0) & 0x100;
}

/**
 * 
 *  rct2: 0x006DF4D4
 */
int sub_6DF4D4(rct_ride *ride, rct_xy_element *element, int isApplying)
{
	return RCT2_CALLPROC_X(0x006DF4D4, element->x, isApplying, element->y, 0, (int)ride, (int)element->element, 0) & 0x100;
}

/**
 * 
 *  rct2: 0x006B51C0
 */
void loc_6B51C0(int rideIndex)
{
	int i, x, y, z;
	rct_ride *ride;
	rct_xy_element trackElement;
	rct_window *w;

	ride = GET_RIDE(rideIndex);

	if (RCT2_GLOBAL(0x0141F568, uint8) != RCT2_GLOBAL(0x013CA740, uint8))
		return;

	w = window_get_main();
	if (w == NULL)
		return;

	sint8 entranceOrExit = -1;
	for (i = 0; i < 4; i++) {
		if (ride->station_starts[i] == 0xFFFF)
			continue;

		if (ride->entrances[i] == 0xFFFF) {
			entranceOrExit = 0;
			break;
		}

		if (ride->exits[i] == 0xFFFF) {
			entranceOrExit = 1;
			break;
		}
	}

	if (entranceOrExit == -1)
		return;

	if (ride->type != RIDE_TYPE_MAZE) {
		x = (ride->station_starts[i] & 0xFF) * 32;
		y = (ride->station_starts[i] >> 8) * 32;
		z = ride->station_heights[i] * 8;
		window_scroll_to_location(w, x, y, z);

		sub_6CAF80(rideIndex, &trackElement);
		ride_find_track_gap(&trackElement, &trackElement);
		ride_modify(&trackElement);

		w = window_find_by_class(WC_RIDE_CONSTRUCTION);
		if (w != NULL)
			window_event_mouse_up_call(w, 29 + entranceOrExit);
	}
}

/**
 * 
 *  rct2: 0x006B528A
 */
void loc_6B528A(rct_xy_element *trackElement)
{
	rct_ride *ride;
	rct_window *w;

	ride = GET_RIDE(trackElement->element->properties.track.ride_index);

	if (RCT2_GLOBAL(0x0141F568, uint8) != RCT2_GLOBAL(0x013CA740, uint8))
		return;

	w = window_get_main();
	if (w == NULL)
		return;

	window_scroll_to_location(w, trackElement->x, trackElement->y, trackElement->element->base_height * 8);
	ride_modify(trackElement);
}

/**
 * 
 *  rct2: 0x006B4F6B
 */
rct_map_element *loc_6B4F6B(int rideIndex, int x, int y)
{
	rct_map_element *mapElement;

	mapElement = map_get_first_element_at(x / 32, y / 32);
	do {
		if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_TRACK)
			continue;

		uint32 unk = mapElement->properties.track.type << 4;
		if (RCT2_GLOBAL(0x00F43484, uint32) & 0x80000) {
			if (!(RCT2_ADDRESS(0x0099CA64, uint8)[unk] & 0x10))
				continue;
		} else {
			if (!(RCT2_ADDRESS(0x0099BA64, uint8)[unk] & 0x10))
				continue;
		}

		if (mapElement->properties.track.ride_index == rideIndex)
			return mapElement;
	} while (!map_element_is_last_for_tile(mapElement++));
	
	return NULL;
}

int ride_is_valid_for_test(int rideIndex, int goingToBeOpen, int isApplying)
{
	int stationIndex;
	rct_ride *ride;
	rct_xy_element trackElement, problematicTrackElement;

	ride = GET_RIDE(rideIndex);

	window_close_by_class(WC_RIDE_CONSTRUCTION);

	stationIndex = ride_mode_check_station_present(ride);
	if (stationIndex == -1)return 0;

	if (!ride_mode_check_valid_station_numbers(ride))
		return 0;

	if (!ride_check_for_entrance_exit(rideIndex)) {
		loc_6B51C0(rideIndex);
		return 0;
	}

	if (goingToBeOpen && isApplying) {
		sub_6B5952(rideIndex);
		ride->lifecycle_flags |= RIDE_LIFECYCLE_EVER_BEEN_OPENED;
	}

	// z = ride->station_heights[i] * 8;
	trackElement.x = (ride->station_starts[stationIndex] & 0xFF) * 32;
	trackElement.y = (ride->station_starts[stationIndex] >> 8) * 32;
	trackElement.element = loc_6B4F6B(rideIndex, trackElement.x, trackElement.y);
	if (trackElement.element == NULL) {
		// Maze is strange, station start is 0... investigation required
		if (ride->type != RIDE_TYPE_MAZE)
			return 0;
	}

	if (
		ride->type == RIDE_TYPE_AIR_POWERED_VERTICAL_COASTER ||
		ride->mode == RIDE_MODE_CONTINUOUS_CIRCUIT ||
		ride->mode == RIDE_MODE_CONTINUOUS_CIRCUIT_BLOCK_SECTIONED ||
		ride->mode == RIDE_MODE_POWERED_LAUNCH_BLOCK_SECTIONED
		) {
		if (ride_find_track_gap(&trackElement, &problematicTrackElement) && (!gConfigGeneral.test_unfinished_tracks ||
			ride->mode == RIDE_MODE_CONTINUOUS_CIRCUIT_BLOCK_SECTIONED || ride->mode == RIDE_MODE_POWERED_LAUNCH_BLOCK_SECTIONED)) {
			RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TEXT, uint16) = STR_TRACK_IS_NOT_A_COMPLETE_CIRCUIT;
			loc_6B528A(&problematicTrackElement);
			return 0;
		}
	}

	if (
		ride->mode == RIDE_MODE_CONTINUOUS_CIRCUIT_BLOCK_SECTIONED ||
		ride->mode == RIDE_MODE_POWERED_LAUNCH_BLOCK_SECTIONED
		) {
		if (!ride_check_block_brakes(&trackElement, &problematicTrackElement)) {
			loc_6B528A(&problematicTrackElement);
			return 0;
		}
	}

	if (ride->subtype != 255) {
		rct_ride_type *rideType = GET_RIDE_ENTRY(ride->subtype);
		if (rideType->flags & RIDE_ENTRY_FLAG_NO_INVERSIONS) {
			RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TEXT, uint16) = STR_TRACK_UNSUITABLE_FOR_TYPE_OF_TRAIN;
			if (ride_check_track_contains_inversions(&trackElement, &problematicTrackElement)) {
				loc_6B528A(&problematicTrackElement);
				return 0;
			}
		}
		if (rideType->flags & RIDE_ENTRY_FLAG_NO_BANKED_TRACK) {
			RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TEXT, uint16) = STR_TRACK_UNSUITABLE_FOR_TYPE_OF_TRAIN;
			if (ride_check_track_contains_banked(&trackElement, &problematicTrackElement)) {
				loc_6B528A(&problematicTrackElement);
				return 0;
			}
		}
	}

	if (ride->mode == RIDE_MODE_STATION_TO_STATION) {
		if (!ride_find_track_gap(&trackElement, &problematicTrackElement)) {
			RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TEXT, uint16) = STR_RIDE_MUST_START_AND_END_WITH_STATIONS;
			return 0;
		}

		RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TEXT, uint16) = STR_STATION_NOT_LONG_ENOUGH;
		if (!ride_check_station_length(&trackElement, &problematicTrackElement)) {
			loc_6B528A(&problematicTrackElement);
			return 0;
		}

		RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TEXT, uint16) = STR_RIDE_MUST_START_AND_END_WITH_STATIONS;
		if (!ride_check_start_and_end_is_station(&trackElement, &problematicTrackElement)) {
			loc_6B528A(&problematicTrackElement);
			return 0;
		}
	}

	if (isApplying)
		sub_6B4D26(rideIndex, &trackElement);

	if (
		!ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_13) &&
		!(ride->lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK)
		) {
		if (sub_6DD84C(ride, rideIndex, &trackElement, isApplying))
			return 0;
	}

	if (
		(RCT2_GLOBAL(0x0097D4F2 + (ride->type * 8), uint32) & 0x400) &&
		(ride->lifecycle_flags & RIDE_LIFECYCLE_16) &&
		!(ride->lifecycle_flags & RIDE_LIFECYCLE_CABLE_LIFT)
		) {
		if (sub_6DF4D4(ride, &trackElement, isApplying))
			return 0;
	}

	return 1;
}
/**
 * 
 *  rct2: 0x006B4EEA
 */
int ride_is_valid_for_open(int rideIndex, int goingToBeOpen, int isApplying)
{
	int stationIndex;
	rct_ride *ride;
	rct_xy_element trackElement, problematicTrackElement;

	ride = GET_RIDE(rideIndex);

	window_close_by_class(WC_RIDE_CONSTRUCTION);

	stationIndex = ride_mode_check_station_present(ride);
	if (stationIndex == -1)return 0;

	if (!ride_mode_check_valid_station_numbers(ride))
		return 0;

	if (!ride_check_for_entrance_exit(rideIndex)) {
		loc_6B51C0(rideIndex);
		return 0;
	}

	if (goingToBeOpen && isApplying) {
		sub_6B5952(rideIndex);
		ride->lifecycle_flags |= RIDE_LIFECYCLE_EVER_BEEN_OPENED;
	}
	
	// z = ride->station_heights[i] * 8;
	trackElement.x = (ride->station_starts[stationIndex] & 0xFF) * 32;
	trackElement.y = (ride->station_starts[stationIndex] >> 8) * 32;
	trackElement.element = loc_6B4F6B(rideIndex, trackElement.x, trackElement.y);
	if (trackElement.element == NULL) {
		// Maze is strange, station start is 0... investigation required
		if (ride->type != RIDE_TYPE_MAZE)
			return 0;
	}

	if (
		ride->type == RIDE_TYPE_AIR_POWERED_VERTICAL_COASTER ||
		ride->mode == RIDE_MODE_RACE ||
		ride->mode == RIDE_MODE_CONTINUOUS_CIRCUIT ||
		ride->mode == RIDE_MODE_CONTINUOUS_CIRCUIT_BLOCK_SECTIONED ||
		ride->mode == RIDE_MODE_POWERED_LAUNCH_BLOCK_SECTIONED
	) {
		if (ride_find_track_gap(&trackElement, &problematicTrackElement)) {
			RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TEXT, uint16) = STR_TRACK_IS_NOT_A_COMPLETE_CIRCUIT;
			loc_6B528A(&problematicTrackElement);
			return 0;
		}
	}

	if (
		ride->mode == RIDE_MODE_CONTINUOUS_CIRCUIT_BLOCK_SECTIONED ||
		ride->mode == RIDE_MODE_POWERED_LAUNCH_BLOCK_SECTIONED
	) {
		if (!ride_check_block_brakes(&trackElement, &problematicTrackElement)) {
			loc_6B528A(&problematicTrackElement);
			return 0;
		}
	}

	if (ride->subtype != 255) {
		rct_ride_type *rideType = GET_RIDE_ENTRY(ride->subtype);
		if (rideType->flags & RIDE_ENTRY_FLAG_NO_INVERSIONS) {
			RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TEXT, uint16) = STR_TRACK_UNSUITABLE_FOR_TYPE_OF_TRAIN;
			if (ride_check_track_contains_inversions(&trackElement, &problematicTrackElement)) {
				loc_6B528A(&problematicTrackElement);
				return 0;
			}
		}
		if (rideType->flags & RIDE_ENTRY_FLAG_NO_BANKED_TRACK) {
			RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TEXT, uint16) = STR_TRACK_UNSUITABLE_FOR_TYPE_OF_TRAIN;
			if (ride_check_track_contains_banked(&trackElement, &problematicTrackElement)) {
				loc_6B528A(&problematicTrackElement);
				return 0;
			}
		}
	}

	if (ride->mode == RIDE_MODE_STATION_TO_STATION) {
		if (!ride_find_track_gap(&trackElement, &problematicTrackElement)) {
			RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TEXT, uint16) = STR_RIDE_MUST_START_AND_END_WITH_STATIONS;
			return 0;
		}

		RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TEXT, uint16) = STR_STATION_NOT_LONG_ENOUGH;
		if (!ride_check_station_length(&trackElement, &problematicTrackElement)) {
			loc_6B528A(&problematicTrackElement);
			return 0;
		}

		RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TEXT, uint16) = STR_RIDE_MUST_START_AND_END_WITH_STATIONS;
		if (!ride_check_start_and_end_is_station(&trackElement, &problematicTrackElement)) {
			loc_6B528A(&problematicTrackElement);
			return 0;
		}
	}

	if (isApplying)
		sub_6B4D26(rideIndex, &trackElement);

	if (
		!ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_13) &&
		!(ride->lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK)
	) {
		if (sub_6DD84C(ride, rideIndex, &trackElement, isApplying))
			return 0;
	}

	if (
		(RCT2_GLOBAL(0x0097D4F2 + (ride->type * 8), uint32) & 0x400) &&
		(ride->lifecycle_flags & RIDE_LIFECYCLE_16) &&
		!(ride->lifecycle_flags & RIDE_LIFECYCLE_CABLE_LIFT)
	) {
		if (sub_6DF4D4(ride, &trackElement, isApplying))
			return 0;
	}

	return 1;
}

void ride_set_status(int rideIndex, int status)
{
	game_do_command(0, GAME_COMMAND_FLAG_APPLY, 0, rideIndex | (status << 8), GAME_COMMAND_SET_RIDE_STATUS, 0, 0);
}

/**
 * 
 *  rct2: 0x006B4EA6
 */
void game_command_set_ride_status(int *eax, int *ebx, int *ecx, int *edx, int *esi, int *edi, int *ebp)
{
	int rideIndex, targetStatus;
	rct_ride *ride;

	rideIndex = *edx & 0xFF;
	targetStatus = (*edx >> 8) & 0xFF;

	RCT2_GLOBAL(RCT2_ADDRESS_NEXT_EXPENDITURE_TYPE, uint8) = RCT_EXPENDITURE_TYPE_RIDE_RUNNING_COSTS * 4;

	ride = GET_RIDE(rideIndex);
	RCT2_GLOBAL(0x00F43484, uint32) = RCT2_GLOBAL(RCT2_ADDRESS_RIDE_FLAGS + (ride->type * 8), uint32);

	switch (targetStatus) {
	case RIDE_STATUS_CLOSED:
		if (*ebx & GAME_COMMAND_FLAG_APPLY) {
			if (ride->status == targetStatus) {
				if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN)) {
					ride->lifecycle_flags &= ~RIDE_LIFECYCLE_CRASHED;
					ride_clear_for_construction(rideIndex);
					ride_remove_peeps(rideIndex);
				}
			}
		
			ride->status = RIDE_STATUS_CLOSED;
			ride->lifecycle_flags &= ~RIDE_LIFECYCLE_PASS_STATION_NO_STOPPING;
			ride->race_winner = 0xFFFF;
			ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_MAIN | RIDE_INVALIDATE_RIDE_LIST;
			window_invalidate_by_number(WC_RIDE, rideIndex);
		}
		*ebx = 0;
		return;
	case RIDE_STATUS_TESTING:
	case RIDE_STATUS_OPEN:
		if (ride->status == targetStatus) {
			*ebx = 0;
			return;
		}

		if (targetStatus == RIDE_STATUS_TESTING) {
			if (!ride_is_valid_for_test(rideIndex, targetStatus == RIDE_STATUS_OPEN, *ebx & GAME_COMMAND_FLAG_APPLY)) {
				*ebx = MONEY32_UNDEFINED;
				return;
			}
		}
		else if (!ride_is_valid_for_open(rideIndex, targetStatus == RIDE_STATUS_OPEN, *ebx & GAME_COMMAND_FLAG_APPLY)) {
			*ebx = MONEY32_UNDEFINED;
			return;
		}

		if (*ebx & GAME_COMMAND_FLAG_APPLY) {
			ride->race_winner = 0xFFFF;
			ride->status = targetStatus;
			ride_get_measurement(rideIndex, NULL);
			ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_MAIN | RIDE_INVALIDATE_RIDE_LIST;
			window_invalidate_by_number(WC_RIDE, rideIndex);
		}
		*ebx = 0;
		return;
	}
}

void ride_set_name(int rideIndex, const char *name)
{
	RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TITLE, uint16) = STR_CANT_RENAME_RIDE_ATTRACTION;
	game_do_command(1, (rideIndex << 8) | 1, 0, *((int*)(name +  0)), GAME_COMMAND_SET_RIDE_NAME, *((int*)(name +  8)), *((int*)(name +  4)));
	game_do_command(2, (rideIndex << 8) | 1, 0, *((int*)(name + 12)), GAME_COMMAND_SET_RIDE_NAME, *((int*)(name + 20)), *((int*)(name + 16)));
	game_do_command(0, (rideIndex << 8) | 1, 0, *((int*)(name + 24)), GAME_COMMAND_SET_RIDE_NAME, *((int*)(name + 32)), *((int*)(name + 28)));
}

/**
 * 
 *  rct2: 0x006B578B
 */
void game_command_set_ride_name(int *eax, int *ebx, int *ecx, int *edx, int *esi, int *edi, int *ebp)
{
	rct_window *w;
	rct_ride *ride;
	rct_string_id newUserStringId;
	char oldName[128];
	static char newName[128];

	int rideIndex = (*ebx >> 8) & 0xFF;
	int nameChunkIndex = *eax & 0xFFFF;

	RCT2_GLOBAL(RCT2_ADDRESS_NEXT_EXPENDITURE_TYPE, uint8) = RCT_EXPENDITURE_TYPE_RIDE_RUNNING_COSTS * 4;
	if (*ebx & GAME_COMMAND_FLAG_APPLY) {
		int nameChunkOffset = nameChunkIndex - 1;
		if (nameChunkOffset < 0)
			nameChunkOffset = 2;
		nameChunkOffset *= 12;
		RCT2_GLOBAL(newName + nameChunkOffset + 0, uint32) = *edx;
		RCT2_GLOBAL(newName + nameChunkOffset + 4, uint32) = *ebp;
		RCT2_GLOBAL(newName + nameChunkOffset + 8, uint32) = *edi;
	}

	if (nameChunkIndex != 0) {
		*ebx = 0;
		return;
	}

	ride = GET_RIDE(rideIndex);
	format_string(oldName, ride->name, &ride->name_arguments);
	if (strcmp(oldName, newName) == 0) {
		*ebx = 0;
		return;
	}

	if (newName[0] == 0) {
		RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TEXT, uint16) = STR_INVALID_RIDE_ATTRACTION_NAME;
		*ebx = MONEY32_UNDEFINED;
		return;
	}

	newUserStringId = user_string_allocate(4, newName);
	if (newUserStringId == 0) {
		*ebx = MONEY32_UNDEFINED;
		return;
	}

	if (*ebx & GAME_COMMAND_FLAG_APPLY) {
		// Free the old ride name
		user_string_free(ride->name);

		ride->name = newUserStringId;

		gfx_invalidate_screen();

		// Force ride list window refresh
		w = window_find_by_class(WC_RIDE_LIST);
		if (w != NULL)
			w->no_list_items = 0;
	} else {
		user_string_free(newUserStringId);
	}

	*ebx = 0;
}

/**
 * 
 *  rct2: 0x006CB7FB
 */
int ride_get_refund_price(int ride_id)
{
	uint8 oldpaused = RCT2_GLOBAL(RCT2_ADDRESS_GAME_PAUSED, uint8);
	RCT2_GLOBAL(RCT2_ADDRESS_GAME_PAUSED, uint8) = 0;
	RCT2_GLOBAL(0x00F4413A, money32) = 0;

	map_element_iterator it;

	map_element_iterator_begin(&it);
	while (map_element_iterator_next(&it)) {
		if (map_element_get_type(it.element) != MAP_ELEMENT_TYPE_TRACK)
			continue;

		if (it.element->properties.track.ride_index != ride_id)
			continue;

		int x = it.x * 32, y = it.y * 32;
		int z = it.element->base_height * 8;

		uint8 rotation = it.element->type & MAP_ELEMENT_DIRECTION_MASK;
		uint8 type = it.element->properties.track.type;

		if (type != TRACK_ELEM_INVERTED_90_DEG_UP_TO_FLAT_QUARTER_LOOP){
			RCT2_GLOBAL(0x00F4413A, money32) += game_do_command(
				x,
				GAME_COMMAND_FLAG_APPLY | (rotation << 8),
				y,
				type | ((it.element->properties.track.sequence & 0xF) << 8),
				GAME_COMMAND_REMOVE_TRACK,
				z,
				0);
			map_element_iterator_restart_for_tile(&it);
			continue;
		}

		RCT2_GLOBAL(0x00F4413A, money32) += game_do_command(
			x,
			GAME_COMMAND_FLAG_APPLY | (0 << 8),
			y,
			ride_id | (2 << 8),
			GAME_COMMAND_SET_MAZE_TRACK,
			z,
			0);

		RCT2_GLOBAL(0x00F4413A, money32) += game_do_command(
			x,
			GAME_COMMAND_FLAG_APPLY | (1 << 8),
			y + 16,
			ride_id | (2 << 8),
			GAME_COMMAND_SET_MAZE_TRACK,
			z,
			0);

		RCT2_GLOBAL(0x00F4413A, money32) += game_do_command(
			x + 16,
			GAME_COMMAND_FLAG_APPLY | (2 << 8),
			y + 16,
			ride_id | (2 << 8),
			GAME_COMMAND_SET_MAZE_TRACK,
			z,
			0);

		RCT2_GLOBAL(0x00F4413A, money32) += game_do_command(
			x + 16,
			GAME_COMMAND_FLAG_APPLY | (3 << 8),
			y,
			ride_id | (2 << 8),
			GAME_COMMAND_SET_MAZE_TRACK,
			z,
			0);
		map_element_iterator_restart_for_tile(&it);
	}
	RCT2_GLOBAL(RCT2_ADDRESS_GAME_PAUSED, uint8) = oldpaused;
	return RCT2_GLOBAL(0x00F4413A, int);
}

/**
 * 
 *  rct2: 0x00696707
 */
static void ride_stop_peeps_queuing(int rideIndex)
{
	uint16 spriteIndex;
	rct_peep *peep;

	FOR_ALL_PEEPS(spriteIndex, peep) {
		if (peep->state != PEEP_STATE_QUEUING)
			continue;
		if (peep->current_ride != rideIndex)
			continue;

		remove_peep_from_queue(peep);
		peep_decrement_num_riders(peep);

		peep->state = PEEP_STATE_FALLING;

		peep_window_state_update(peep);
	}
}

static int ride_get_empty_slot()
{
	rct_ride *ride;
	for (int i = 0; i < MAX_RIDES; i++) {
		ride = GET_RIDE(i);
		if (ride->type == RIDE_TYPE_NULL) {
			return i;
		}
	}
	return -1;
}

static int ride_get_default_mode(rct_ride *ride)
{
	const rct_ride_type *rideEntry = GET_RIDE_ENTRY(ride->subtype);
	const uint8 *availableModes = RideAvailableModes;

	for (int i = 0; i < ride->type; i++) {
		while (*(availableModes++) != 255) {}
	}
	if (rideEntry->flags & RIDE_ENTRY_FLAG_17) {
		availableModes += 2;
	}
	return availableModes[0];
}

static bool ride_with_colour_config_exists(int rideType, const track_colour *colours)
{
	rct_ride *ride;
	int i;

	FOR_ALL_RIDES(i, ride) {
		if (ride->type != rideType) continue;
		if (ride->track_colour_main[0] != colours->main) continue;
		if (ride->track_colour_additional[0] != colours->additional) continue;
		if (ride->track_colour_supports[0] != colours->supports) continue;

		return true;
	}
	return false;
}

static bool ride_name_exists(char *name)
{
	char buffer[256];
	rct_ride *ride;
	int i;

	FOR_ALL_RIDES(i, ride) {
		format_string(buffer, ride->name, &ride->name_arguments);
		if (strcmp(buffer, name) == 0) {
			return true;
		}
	}

	return false;
}

/**
 * 
 *  rct2: 0x006B4776
 */
static void ride_set_to_random_colour_preset(rct_ride *ride)
{
	const track_colour_preset_list *colourPresets;
	const track_colour *colours;

	colourPresets = RCT2_ADDRESS(0x0097D934, track_colour_preset_list*)[ride->type];

	// 200 attempts to find a colour preset that hasn't already been used in the park for this ride type
	for (int i = 0; i < 200; i++) {
		int listIndex = scenario_rand() % colourPresets->count;
		colours = &colourPresets->list[listIndex];

		if (!ride_with_colour_config_exists(ride->type, colours)) {
			break;
		}
	}

	for (int i = 0; i < 4; i++) {
		ride->track_colour_main[i] = colours->main;
		ride->track_colour_additional[i] = colours->additional;
		ride->track_colour_supports[i] = colours->supports;
	}
	ride->colour_scheme_type = 0;
}

static money32 ride_get_common_price(rct_ride *forRide)
{
	rct_ride *ride;
	int i;

	FOR_ALL_RIDES(i, ride) {
		if (ride->type == forRide->type && ride != forRide) {
			return ride->price;
		}
	}

	return MONEY32_UNDEFINED;
}

static money32 shop_item_get_common_price(rct_ride *forRide, int shopItem)
{
	rct_ride_type *rideEntry;
	rct_ride *ride;
	int i;

	FOR_ALL_RIDES(i, ride) {
		if (ride != forRide) {
			rideEntry = GET_RIDE_ENTRY(ride->subtype);
			if (rideEntry->shop_item == shopItem) {
				return ride->price;
			}
			if (rideEntry->shop_item_secondary == shopItem) {
				return ride->price_secondary;
			}
		}
	}

	return MONEY32_UNDEFINED;
}

static bool shop_item_has_common_price(int shopItem)
{
	if (shopItem < 32) {
		return RCT2_GLOBAL(0x01358838, uint32) & (1 << shopItem);
	} else {
		return RCT2_GLOBAL(0x0135934C, uint32) & (1 << (shopItem - 32));
	}
}

/**
 * 
 *  rct2: 0x006B3F0F
 */
money32 ride_create(int type, int subType, int flags, int *outRideIndex)
{
	char rideNameBuffer[256];
	rct_ride *ride;
	rct_ride_type *rideEntry;
	int rideIndex, rideEntryIndex;

	if (subType == 255) {
		uint8 *availableRideEntries = get_ride_entry_indices_for_ride_type(type);
		for (uint8 *rei = availableRideEntries; *rei != 255; rei++) {
			rideEntry = GET_RIDE_ENTRY(*rei);
			if ((rideEntry->flags & RIDE_ENTRY_FLAG_SEPARATE_RIDE_NAME) && !rideTypeShouldLoseSeparateFlag(rideEntry)) {
				subType = *rei;
				goto foundRideEntry;
			}
		}
		subType = availableRideEntries[0];
	}

foundRideEntry:
	rideEntryIndex = subType;
	rideIndex = ride_get_empty_slot();
	if (rideIndex == -1) {
		RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TEXT, rct_string_id) = STR_TOO_MANY_RIDES;
		return MONEY32_UNDEFINED;
	}
	*outRideIndex = rideIndex;

	if (!(flags & GAME_COMMAND_FLAG_APPLY)) {
		RCT2_GLOBAL(RCT2_ADDRESS_NEXT_EXPENDITURE_TYPE, uint8) = RCT_EXPENDITURE_TYPE_RIDE_CONSTRUCTION;
		RCT2_GLOBAL(0x009DEA5E, uint16) = 0x8000;
		return 0;
	}

	ride = GET_RIDE(rideIndex);
	rideEntry = GET_RIDE_ENTRY(rideEntryIndex);
	ride->type = type;
	ride->subtype = rideEntryIndex;
	ride_set_to_random_colour_preset(ride);
	ride->overall_view = 0xFFFF;

	// Ride name
	if (rideEntryIndex == 255) {
	useDefaultName:
		ride->name = STR_NONE;

		struct {
			uint16 type_name;
			uint16 number;
		} name_args;
		name_args.type_name = 2 + ride->type;
		name_args.number = 0;
		do {
			name_args.number++;
			format_string(rideNameBuffer, 1, &name_args);
		} while (ride_name_exists(rideNameBuffer));
		ride->name = 1;
		ride->name_arguments_type_name = name_args.type_name;
		ride->name_arguments_number = name_args.number;
	} else {
		if (!(rideEntry->flags & RIDE_ENTRY_FLAG_SEPARATE_RIDE_NAME) || rideTypeShouldLoseSeparateFlag(rideEntry)) {
			goto useDefaultName;
		}
		ride->name = 1;
		ride->name_arguments_type_name = rideEntry->name;
		ride->name_arguments_number = 0;

		rct_string_id rideNameStringId = 0;
		for (int i = 0; i < 100; i++) {
			ride->name_arguments_number++;
			format_string(rideNameBuffer, ride->name, &ride->name_arguments);

			rideNameStringId = user_string_allocate(4, rideNameBuffer);
			if (rideNameStringId != 0) {
				ride->name = rideNameStringId;
				break;
			}
		}
		if (rideNameStringId == 0) {
			goto useDefaultName;
		}
	}

	for (int i = 0; i < 4; i++) {
		ride->station_starts[i] = 0xFFFF;
		ride->entrances[i] = 0xFFFF;
		ride->exits[i] = 0xFFFF;
		ride->var_066[i] = 255;
		ride->queue_time[i] = 0;
	}

	for (int i = 0; i < 32; i++) {
		ride->vehicles[i] = 0xFFFF;
	}

	ride->status = RIDE_STATUS_CLOSED;
	ride->lifecycle_flags = 0;
	ride->var_1CA = 0;
	ride->num_stations = 0;
	ride->num_vehicles = 1;
	ride->var_0CA = 32;
	ride->max_trains = 32;
	ride->num_cars_per_train = 1;
	ride->var_0CB = 12;
	ride->min_waiting_time = 10;
	ride->max_waiting_time = 60;
	ride->depart_flags = RIDE_DEPART_WAIT_FOR_MINIMUM_LENGTH | 3;
	if (RCT2_ADDRESS(0x0097D4F2, uint16)[ride->type * 4] & 2) {
		ride->lifecycle_flags |= RIDE_LIFECYCLE_MUSIC;
	}
	ride->music = RCT2_ADDRESS(0x0097D4F4, uint8)[ride->type * 8];

	ride->operation_option = (
		RCT2_GLOBAL(0x0097CF40 + 4 + (ride->type * 8), uint8) +
		RCT2_GLOBAL(0x0097CF40 + 4 + (ride->type * 8), uint8) +
		RCT2_GLOBAL(0x0097CF40 + 4 + (ride->type * 8), uint8) +
		RCT2_GLOBAL(0x0097CF40 + 5 + (ride->type * 8), uint8)
	) / 4;

	ride->lift_hill_speed = RCT2_ADDRESS(0x0097D7C9, uint8)[ride->type * 4];

	ride->measurement_index = 255;
	ride->excitement = (ride_rating)-1;
	ride->var_120 = 0;
	ride->var_122 = 0;
	ride->var_148 = 0;
	
	ride->price = 0;
	ride->price_secondary = 0;
	if (!(RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & PARK_FLAGS_NO_MONEY)) {
		ride->price = RCT2_GLOBAL(0x0097D4F0 + 0 + (ride->type * 8), uint8);
		ride->price_secondary = RCT2_GLOBAL(0x0097D4F0 + 1 + (ride->type * 8), uint8);

		if (rideEntry->shop_item != 255) {
			ride->price = RCT2_ADDRESS(0x00982358, money8)[rideEntry->shop_item];
		}
		if (rideEntry->shop_item_secondary != 255) {
			ride->price = RCT2_ADDRESS(0x00982358, money8)[rideEntry->shop_item_secondary];
		}
		if (rideEntry->shop_item == 255 && !(RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & PARK_FLAGS_PARK_FREE_ENTRY)) {
			ride->price = 0;
		}
		if (RCT2_GLOBAL(RCT2_ADDRESS_OBJECTIVE_TYPE, uint8) == OBJECTIVE_BUILD_THE_BEST) {
			ride->price = 0;
		}

		if (ride->type == RIDE_TYPE_TOILETS) {
			if (RCT2_GLOBAL(0x01358838, uint32) & (1 << 31)) {
				money32 price = ride_get_common_price(ride);
				if (price != MONEY32_UNDEFINED) {
					ride->price = (money16)price;
				}
			}
		}

		if (rideEntry->shop_item != 255) {
			if (shop_item_has_common_price(rideEntry->shop_item)) {
				money32 price = shop_item_get_common_price(ride, rideEntry->shop_item);
				if (price != MONEY32_UNDEFINED) {
					ride->price = (money16)price;
				}
			}
		}

		if (rideEntry->shop_item_secondary != 255) {
			if (shop_item_has_common_price(rideEntry->shop_item_secondary)) {
				money32 price = shop_item_get_common_price(ride, rideEntry->shop_item_secondary);
				if (price != MONEY32_UNDEFINED) {
					ride->price_secondary = (money16)price;
				}
			}
		}
	}

	// The next 10 variables are treated like an array of 10 items
	ride->var_124 = 0;
	ride->var_124 = 0;
	ride->var_126 = 0;
	ride->var_128 = 0;
	ride->var_12A = 0;
	ride->var_12C = 0;
	ride->var_12E = 0;
	ride->age = 0;
	ride->running_cost = 0;
	ride->var_134 = 0;
	ride->var_136 = 0;
	
	ride->value = 0xFFFF;
	ride->satisfaction = 255;
	ride->satisfaction_time_out = 0;
	ride->satisfaction_next = 0;
	ride->popularity = 255;
	ride->popularity_time_out = 0;
	ride->popularity_next = 0;
	ride->window_invalidate_flags = 0;
	ride->total_customers = 0;
	ride->total_profit = 0;
	ride->num_riders = 0;
	ride->var_15D = 0;
	ride->maze_tiles = 0;
	ride->build_date = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_MONTH_YEAR, uint16);
	ride->music_tune_id = 255;

	ride->breakdown_reason = 255;
	ride->upkeep_cost = (money16)-1;
	ride->reliability = 0x64FF;
	ride->unreliability_factor = 1;
	ride->inspection_interval = RIDE_INSPECTION_EVERY_30_MINUTES;
	ride->last_inspection = 0;
	ride->downtime = 0;
	ride->var_19C = 0;
	ride->var_1A0 = 0;
	ride->no_primary_items_sold = 0;
	ride->no_secondary_items_sold = 0;
	ride->last_crash_type = RIDE_CRASH_TYPE_NONE;
	ride->income_per_hour = MONEY32_UNDEFINED;
	ride->profit = MONEY32_UNDEFINED;
	ride->connected_message_throttle = 0;
	ride->entrance_style = RIDE_ENTRANCE_STYLE_PLAIN;
	ride->num_block_brakes = 0;
	ride->guests_favourite = 0;

	ride->num_circuits = 1;
	ride->mode = ride_get_default_mode(ride);
	ride->min_max_cars_per_train = (rideEntry->min_cars_in_train << 4) | rideEntry->max_cars_in_train;
	sub_6DE52C(ride);
	window_invalidate_by_class(WC_RIDE_LIST);

	RCT2_GLOBAL(RCT2_ADDRESS_NEXT_EXPENDITURE_TYPE, uint8) = RCT_EXPENDITURE_TYPE_RIDE_CONSTRUCTION;
	RCT2_GLOBAL(0x009DEA5E, uint16) = 0x8000;
	return 0;
}

/**
 * 
 *  rct2: 0x006B3F0F
 */
void game_command_create_ride(int *eax, int *ebx, int *ecx, int *edx, int *esi, int *edi, int *ebp)
{
	*ebx = ride_create(*edx & 0xFF, (*edx >> 8) & 0xFF, *ebx & 0xFF, edi);
}

/**
 * 
 *  rct2: 0x006B49D9
 */
void game_command_demolish_ride(int *eax, int *ebx, int *ecx, int *edx, int *esi, int *edi, int *ebp)
{
	uint8 ride_id = *(uint8*)edx;

	RCT2_GLOBAL(0x009DEA5E, uint16) = 0;
	RCT2_GLOBAL(0x009DEA60, uint16) = 0;
	RCT2_GLOBAL(0x009DEA62, uint16) = 0;
	rct_ride *ride = &g_ride_list[ride_id];
	int x = 0, y = 0, z = 0;
	if(ride->overall_view != (uint16)-1){
		x = ((ride->overall_view & 0xFF) * 32) + 16;
		y = ((ride->overall_view >> 8) * 32) + 16;
		z = map_element_height(x, y);
		RCT2_GLOBAL(0x009DEA5E, uint16) = x;
		RCT2_GLOBAL(0x009DEA60, uint16) = y;
		RCT2_GLOBAL(0x009DEA62, uint16) = z;
	}
	if(!(*ebx & 0x40) && RCT2_GLOBAL(RCT2_ADDRESS_GAME_PAUSED, uint8) && !gConfigCheat.build_in_pause_mode){
		RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TEXT, uint16) = STR_CONSTRUCTION_NOT_POSSIBLE_WHILE_GAME_IS_PAUSED;
		*ebx = MONEY32_UNDEFINED;
		return;
	}else{
		if(*ebx & GAME_COMMAND_FLAG_APPLY){
			if(!(*ebx & 8)){
				window_close_by_number(WC_RIDE_CONSTRUCTION, ride_id);
			}
			window_close_by_number(WC_RIDE, ride_id);
			window_close_by_number(WC_DEMOLISH_RIDE_PROMPT, ride_id);
			window_close_by_class(WC_NEW_CAMPAIGN);
			if(RCT2_GLOBAL(0x01358103, uint8) && ride_id == RCT2_GLOBAL(0x01358117, uint8)){
				RCT2_GLOBAL(0x01358103, uint8) = 0;
			}
			if(RCT2_GLOBAL(0x01358107, uint8) && ride_id == RCT2_GLOBAL(0x0135811B, uint8)){
				RCT2_GLOBAL(0x01358107, uint8) = 0;
			}
			ride_clear_for_construction(ride_id);
			ride_remove_peeps(ride_id);
			ride_stop_peeps_queuing(ride_id);
			*ebx = ride_get_refund_price(ride_id);

			sub_6CB945(ride_id);
			news_item_disable_news(NEWS_ITEM_RIDE, ride_id);
			
			for(int i = 0; i < MAX_BANNERS; i++){
				rct_banner *banner = &gBanners[i];
				if(banner->type != BANNER_NULL && banner->flags & BANNER_FLAG_2 && banner->colour == ride_id){
					banner->flags &= 0xFB;
					banner->string_idx = 778;
				}
			}

			uint16 spriteIndex;
			rct_peep *peep;	
			FOR_ALL_GUESTS(spriteIndex, peep){
				uint8 ride_id_bit = ride_id & 0x3;
				uint8 ride_id_offset = ride_id / 8;
				peep->rides_been_on[ride_id_offset] &= ~(1 << ride_id_bit); // clear ride from potentially being in rides_been_on
				if(peep->state == PEEP_STATE_WATCHING){
					if(peep->current_ride == ride_id){
						peep->current_ride = MAX_RIDES;
						if(peep->time_to_stand >= 50){ // make peep stop watching the ride
							peep->time_to_stand = 50;
						}
					}
				}
				// remove any free voucher for this ride from peep
				if(peep->item_standard_flags & PEEP_ITEM_VOUCHER){
					if(peep->voucher_type == VOUCHER_TYPE_RIDE_FREE && peep->voucher_arguments == ride_id){
						peep->item_standard_flags &= ~(PEEP_ITEM_VOUCHER);
					}
				}
				// remove any photos of this ride from peep
				if(peep->item_standard_flags & PEEP_ITEM_PHOTO){
					if(peep->photo1_ride_ref == ride_id){
						peep->item_standard_flags &= ~PEEP_ITEM_PHOTO;
					}
				}
				if(peep->item_extra_flags && PEEP_ITEM_PHOTO2){
					if(peep->photo2_ride_ref == ride_id){
						peep->item_extra_flags &= ~PEEP_ITEM_PHOTO2;
					}
				}
				if(peep->item_extra_flags && PEEP_ITEM_PHOTO3){
					if(peep->photo3_ride_ref == ride_id){
						peep->item_extra_flags &= ~PEEP_ITEM_PHOTO3;
					}
				}
				if(peep->item_extra_flags && PEEP_ITEM_PHOTO4){
					if(peep->photo4_ride_ref == ride_id){
						peep->item_extra_flags &= ~PEEP_ITEM_PHOTO4;
					}
				}
				if(peep->guest_heading_to_ride_id == ride_id){
					peep->guest_heading_to_ride_id = MAX_RIDES;
				}
				if(peep->favourite_ride == ride_id){
					peep->favourite_ride = MAX_RIDES;
				}
				for (int i = 0; i < PEEP_MAX_THOUGHTS; i++) {
					if(peep->thoughts[i].item == ride_id){
						// Clear top thought, push others up
						memmove(&peep->thoughts[i], &peep->thoughts[i + 1], sizeof(rct_peep_thought)*(PEEP_MAX_THOUGHTS - i - 1));
						peep->thoughts[PEEP_MAX_THOUGHTS - 1].type = PEEP_THOUGHT_TYPE_NONE;
					}
				}
			}

			user_string_free(ride->name);
			ride->type = RIDE_TYPE_NULL;
			window_invalidate_by_class(WC_RIDE_LIST);
			RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_PARK_VALUE, money32) = calculate_park_value();
			RCT2_GLOBAL(0x009DEA5E, uint16) = x;
			RCT2_GLOBAL(0x009DEA60, uint16) = y;
			RCT2_GLOBAL(0x009DEA62, uint16) = z;
			RCT2_GLOBAL(RCT2_ADDRESS_NEXT_EXPENDITURE_TYPE, uint8) = RCT_EXPENDITURE_TYPE_RIDE_CONSTRUCTION * 4;
			return;
		}else{
			*ebx = 0;
			RCT2_GLOBAL(RCT2_ADDRESS_NEXT_EXPENDITURE_TYPE, uint8) = RCT_EXPENDITURE_TYPE_RIDE_CONSTRUCTION * 4;
			return;
		}
	}
}

/**
 * 
 *  rct2: 0x006B2FC5
 */
void game_command_set_ride_appearance(int *eax, int *ebx, int *ecx, int *edx, int *esi, int *edi, int *ebp)
{
	if(*ebx & GAME_COMMAND_FLAG_APPLY){
		uint8 ride_id = *edx;
		uint8 type = *ebx >> 8;
		uint8 value = *edx >> 8;
		int index = *edi;
		rct_ride *ride = &g_ride_list[ride_id];
		switch(type){
			case 0:
				ride->track_colour_main[index] = value;
				gfx_invalidate_screen();
				break;
			case 1:
				ride->track_colour_additional[index] = value;
				gfx_invalidate_screen();
				break;
			case 2:
				*((uint8*)(&ride->vehicle_colours[index])) = value;
				ride_update_vehicle_colours(ride_id);
				break;
			case 3:
				*((uint8*)(&ride->vehicle_colours[index]) + 1) = value;
				ride_update_vehicle_colours(ride_id);
				break;
			case 4:
				ride->track_colour_supports[index] = value;
				gfx_invalidate_screen();
				break;
			case 5:
				ride->colour_scheme_type &= ~(RIDE_COLOUR_SCHEME_DIFFERENT_PER_TRAIN | RIDE_COLOUR_SCHEME_DIFFERENT_PER_CAR);
				ride->colour_scheme_type |= value;
				for(int i = 1; i < countof(ride->vehicle_colours); i++){
					ride->vehicle_colours[i] = ride->vehicle_colours[0];
					ride->vehicle_colours_extended[i] = ride->vehicle_colours_extended[0];
				}
				ride_update_vehicle_colours(ride_id);
				break;
			case 6:
				ride->entrance_style = value;
				RCT2_GLOBAL(0x01358840, uint8) = value;
				gfx_invalidate_screen();
				break;
			case 7:
				ride->vehicle_colours_extended[index] = value;
				ride_update_vehicle_colours(ride_id);
				break;
		}
		window_invalidate_by_number(WC_RIDE, ride_id);
	}
	*ebx = 0;
}

/**
 * 
 *  rct2: 0x006B53E9
 */
void game_command_set_ride_price(int *eax, int *ebx, int *ecx, int *edx, int *esi, int *edi, int *ebp)
{
	uint32 flags, shop_item;
	uint8 ride_number;
	money16 price;
	rct_ride *ride;
	rct_ride_type *ride_type;
	bool secondary_price;
	
	flags = *ebx;
	ride_number = (*edx & 0xFF);
	ride = GET_RIDE(ride_number);
	ride_type = gRideTypeList[ride->subtype];
	price = *edi;
	secondary_price = (*edx >> 8);

	//eax
	//ebx flags
	//ecx ecx
	//edx ride_number
	//ebp ride_type

	RCT2_GLOBAL(RCT2_ADDRESS_NEXT_EXPENDITURE_TYPE, uint8) = RCT_EXPENDITURE_TYPE_PARK_RIDE_TICKETS * 4;
	if (flags & 0x1) {
		if (!secondary_price) {
			shop_item = 0x1F;
			if (ride->type != RIDE_TYPE_TOILETS) {
				shop_item = ride_type->shop_item;
				if (shop_item == 0xFF) {
					ride->price = price;
					window_invalidate_by_class(WC_RIDE);
					return;
				}
			}
			// Check same price in park flags
			if ((shop_item < 32 ? RCT2_GLOBAL(0x01358838, uint32) & (1 << shop_item) : RCT2_GLOBAL(0x0135934C, uint32) & (1 << (shop_item - 32))) == 0) {
				ride->price = price;
				window_invalidate_by_class(WC_RIDE);
				return;
			}
		}
		else {
			shop_item = ride_type->shop_item_secondary;
			if (shop_item == 0xFF) {
				shop_item = RCT2_GLOBAL(0x0097D7CB + (ride->type * 4), uint8);
				if ((ride->lifecycle_flags & RIDE_LIFECYCLE_ON_RIDE_PHOTO) == 0) {
					ride->price_secondary = price;
					window_invalidate_by_class(WC_RIDE);
					return;
				}
			}
			// Check same price in park flags
			if ((shop_item < 32 ? RCT2_GLOBAL(0x01358838, uint32) & (1 << shop_item) : RCT2_GLOBAL(0x0135934C, uint32) & (1 << (shop_item - 32))) == 0) {
				ride->price_secondary = price;
				window_invalidate_by_class(WC_RIDE);
				return;
			}
		}
		ride = GET_RIDE(0);
		ride_type = gRideTypeList[ride->subtype];
		uint8 count = 0;
		while (count < 0xFF) {
			if (ride->type != 0xFF) {
				if (ride->type != RIDE_TYPE_TOILETS || shop_item != 0x1F) {
					if (ride_type->shop_item == shop_item) {
						ride->price = price;
						window_invalidate_by_number(WC_RIDE, count);
					}
				}
				else {
					ride->price = price;
					window_invalidate_by_number(WC_RIDE, count);
				}
				// If the shop item is the same or an on-ride photo
				if (ride_type->shop_item_secondary == shop_item ||
					(ride_type->shop_item_secondary == 0xFF &&
					(shop_item == 0x3 || shop_item == 0x20 || shop_item == 0x21 || shop_item == 0x22))) {

					ride->price_secondary = price;
					window_invalidate_by_number(WC_RIDE, count);
				}
			}
			count++;
			ride++;
			ride_type = gRideTypeList[ride->subtype];
		}
	}
}

bool ride_type_has_flag(int rideType, int flag)
{
	return (RCT2_GLOBAL(RCT2_ADDRESS_RIDE_FLAGS + (rideType * 8), uint32) & flag) != 0;
}

/*
 * The next six functions are helpers to access ride data at the offset 10E &
 * 110. We believe it stores three distinct values in the following format:
 *
 * unknown1: bits 9-11
 * unknown2: bits 6-8
 * unknown3: low 5 bits
 */

int get_var_10E_unk_1(rct_ride* ride) {
	return (ride->var_10E >> 8) & 0x7;
}

int get_var_10E_unk_2(rct_ride* ride) {
	return (ride->var_10E >> 5) & 0x7;
}

int get_var_10E_unk_3(rct_ride* ride) {
	return ride->var_10E & 0x1F;
}

int get_var_110_unk_1(rct_ride* ride) {
	return (ride->var_110 >> 8) & 0x7;
}

int get_var_110_unk_2(rct_ride* ride) {
	return (ride->var_110 >> 5) & 0x7;
}

int get_var_110_unk_3(rct_ride* ride) {
	return ride->var_110 & 0x1F;
}

int get_var_112_unk_1(rct_ride* ride) {
	return (ride->var_112 >> 11) & 0x3F;
}

int get_var_112_unk_2(rct_ride* ride) {
	return (ride->var_112 >> 8) & 7;
}

int get_var_112_unk_3(rct_ride* ride) {
	return (ride->var_112 >> 5) & 7;
}

int get_var_112_unk_4(rct_ride* ride) {
	return ride->var_112 & 0x1F;
}

bool ride_has_spinning_tunnel(rct_ride *ride) {
	return ride->special_track_elements & RIDE_ELEMENT_TUNNEL_SPLASH_OR_RAPIDS;
}

bool ride_has_water_splash(rct_ride *ride) {
	return ride->special_track_elements & RIDE_ELEMENT_TUNNEL_SPLASH_OR_RAPIDS;
}

bool ride_has_rapids(rct_ride *ride) {
	return ride->special_track_elements & RIDE_ELEMENT_TUNNEL_SPLASH_OR_RAPIDS;
}

bool ride_has_log_reverser(rct_ride *ride) {
	return ride->special_track_elements & RIDE_ELEMENT_REVERSER_OR_WATERFALL;
}

bool ride_has_waterfall(rct_ride *ride) {
	return ride->special_track_elements & RIDE_ELEMENT_REVERSER_OR_WATERFALL;
}

bool ride_has_whirlpool(rct_ride *ride) {
	return ride->special_track_elements & RIDE_ELEMENT_WHIRLPOOL;
}

uint8 ride_get_helix_sections(rct_ride *ride) {
	// Helix sections stored in the low 5 bits.
	return ride->special_track_elements & 0x1F;
}

bool ride_is_powered_launched(rct_ride *ride)
{
	return
		ride->mode == RIDE_MODE_POWERED_LAUNCH_PASSTROUGH ||
		ride->mode == RIDE_MODE_POWERED_LAUNCH ||
		ride->mode == RIDE_MODE_POWERED_LAUNCH_BLOCK_SECTIONED;
}

bool ride_has_any_track_elements(int rideIndex)
{
	map_element_iterator it;

	map_element_iterator_begin(&it);
	while (map_element_iterator_next(&it)) {
		if (map_element_get_type(it.element) != MAP_ELEMENT_TYPE_TRACK)
			continue;
		if (it.element->properties.track.ride_index != rideIndex)
			continue;
		if (it.element->flags & MAP_ELEMENT_FLAG_GHOST)
			continue;

		return true;
	}

	return false;
}

void ride_all_has_any_track_elements(bool *rideIndexArray)
{
	map_element_iterator it;

	memset(rideIndexArray, 0, MAX_RIDES * sizeof(bool));

	map_element_iterator_begin(&it);
	while (map_element_iterator_next(&it)) {
		if (map_element_get_type(it.element) != MAP_ELEMENT_TYPE_TRACK)
			continue;
		if (it.element->flags & MAP_ELEMENT_FLAG_GHOST)
			continue;

		rideIndexArray[it.element->properties.track.ride_index] = true;
	}
}

/* rct2: 0x006847BA */
void set_vehicle_type_image_max_sizes(rct_ride_type_vehicle* vehicle_type, int num_images){
	char bitmap[200][200] = { 0 };

	rct_drawpixelinfo dpi = {
		.bits = (char*)bitmap,
		.x = -100,
		.y = -100,
		.width = 200,
		.height = 200,
		.pitch = 0,
		.zoom_level = 0
	};

	for (int i = 0; i < num_images; ++i){
		gfx_draw_sprite(&dpi, vehicle_type->base_image_id + i, 0, 0, 0);
	}
	int al = -1;
	for (int i = 99; i != 0; --i){
		for (int j = 0; j < 200; j++){
			if (bitmap[j][100 - i] != 0){
				al = i;
				break;
			}
		}

		if (al != -1)
			break;

		for (int j = 0; j < 200; j++){
			if (bitmap[j][100 + i] != 0){
				al = i;
				break;
			}
		}

		if (al != -1)
			break;
	}

	al++;
	int bl = -1;

	for (int i = 99; i != 0; --i){
		for (int j = 0; j < 200; j++){
			if (bitmap[100 - i][j] != 0){
				bl = i;
				break;
			}
		}

		if (bl != -1)
			break;
	}
	bl++;

	int bh = -1;

	for (int i = 99; i != 0; --i){
		for (int j = 0; j < 200; j++){
			if (bitmap[100 + i][j] != 0){
				bh = i;
				break;
			}
		}

		if (bh != -1)
			break;
	}
	bh++;

	// Moved from object paint

	if (vehicle_type->var_12 & 0x2000){
		bl += 16;
	}

	vehicle_type->var_0E = al;
	vehicle_type->var_0F = bl;
	vehicle_type->var_10 = bh;
}

/**
 *
 * rct2: 0x006CA28C
 */
money32 ride_get_entrance_or_exit_price(int rideIndex, int x, int y, int direction, int dh, int di)
{
	sub_6C96C0();
	money32 result = game_do_command(
		x,
		105 | (direction << 8),
		y,
		rideIndex | (dh << 8),
		GAME_COMMAND_PLACE_RIDE_ENTRANCE_OR_EXIT,
		di,
		0
	);
	if (result != MONEY32_UNDEFINED) {
		_currentTrackSelectionFlags |= (1 << 2);
		RCT2_GLOBAL(0x00F440BF, uint16) = x;
		RCT2_GLOBAL(0x00F440C1, uint16) = y;
		RCT2_GLOBAL(0x00F440C3, uint8) = direction;
		RCT2_GLOBAL(0x00F440C4, uint8) = di & 0xFF;
	}
	return result;
}

int loc_6CD18E(short mapX, short mapY, short entranceMinX, short entranceMinY, short entranceMaxX, short entranceMaxY)
{
	int direction = 0;
	if (mapX == entranceMinX) {
		if (mapY > entranceMinY && mapY < entranceMaxY) {
			return direction;
		}
	}
	direction = 1;
	if (mapY == entranceMaxY) {
		if (mapX > entranceMinX && mapX < entranceMaxX) {
			return direction;
		}
	}
	direction = 2;
	if (mapX == entranceMaxX) {
		if (mapY > entranceMinY && mapY < entranceMaxY) {
			return direction;
		}
	}
	direction = 3;
	if (mapY == entranceMinY) {
		if (mapX > entranceMinX && mapX < entranceMaxX) {
			return direction;
		}
	}
	return -1;
}

/**
 * 
 * rct2: 0x006CCF70
 */
void ride_get_entrance_or_exit_position_from_screen_position(int screenX, int screenY, int *outX, int *outY, int *outDirection)
{
	short mapX, mapY;
	short entranceMinX, entranceMinY, entranceMaxX, entranceMaxY, word_F4418C, word_F4418E;
	int interactionType, direction, stationHeight, stationDirection;
	rct_map_element *mapElement;
	rct_viewport *viewport;
	rct_ride *ride;

	RCT2_GLOBAL(0x00F44194, uint8) = 255;
	get_map_coordinates_from_pos(screenX, screenY, 0xFFFB, &mapX, &mapY, &interactionType, &mapElement, &viewport);
	if (interactionType != 0) {
		if (map_element_get_type(mapElement) == MAP_ELEMENT_TYPE_TRACK) {
			if (mapElement->properties.track.ride_index == RCT2_GLOBAL(0x00F44192, uint8)) {
				if (RCT2_ADDRESS(0x0099BA64, uint8)[mapElement->properties.track.type << 4] & 0x10) {
					if (mapElement->properties.track.type == 101) {
						RCT2_GLOBAL(0x00F44193, uint8) = 0;
					} else {
						RCT2_GLOBAL(0x00F44193, uint8) = (mapElement->properties.track.sequence & 0x70) >> 4;
					}
				}
			}
		}
	}

	ride = GET_RIDE(RCT2_GLOBAL(0x00F44192, uint8));
	stationHeight = ride->station_heights[RCT2_GLOBAL(0x00F44193, uint8)];

	screen_get_map_xy_with_z(screenX, screenY, stationHeight * 8, &mapX, &mapY);
	if (mapX == (short)0x8000) {
		*outX = 0x8000;
		return;
	}

	word_F4418C = mapX;
	word_F4418E = mapY;
	RCT2_GLOBAL(0x00F44188, uint16) = floor2(mapX, 32);
	RCT2_GLOBAL(0x00F4418A, uint16) = floor2(mapY, 32);
	*outX = RCT2_GLOBAL(0x00F44188, uint16);
	*outY = RCT2_GLOBAL(0x00F4418A, uint16);

	if (ride->type == RIDE_TYPE_NULL)
		return;

	uint16 stationStartXY = ride->station_starts[RCT2_GLOBAL(0x00F44193, uint8)];
	if (stationStartXY == 0xFFFF)
		return;

	RCT2_GLOBAL(0x00F44190, uint8) = stationHeight;

	if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_3)) {
		mapX = (word_F4418C & 0x1F) - 16;
		mapY = (word_F4418E & 0x1F) - 16;
		if (abs(mapX) < abs(mapY)) {
			direction = mapY < 0 ? 3 : 1;
		} else {
			direction = mapX < 0 ? 0 : 2;
		}

		for (int i = 0; i < 4; i++) {
			mapX = RCT2_GLOBAL(0x00F44188, uint16) + TileDirectionDelta[direction].x;
			mapY = RCT2_GLOBAL(0x00F4418A, uint16) + TileDirectionDelta[direction].y;
			if (mapX >= 0 && mapY >= 0 && mapX < (256 * 32) && mapY < (256 * 32)) {
				mapElement = map_get_first_element_at(mapX >> 5, mapY >> 5);
				do {
					if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_TRACK)
						continue;
					if (mapElement->base_height != stationHeight)
						continue;
					if (mapElement->properties.track.ride_index != RCT2_GLOBAL(0x00F44192, uint8))
						continue;
					if (mapElement->properties.track.type == 101) {
						RCT2_GLOBAL(0x00F44194, uint8) = direction ^ 2;
						*outDirection = direction ^ 2;
						return;
					}
					if (map_get_station(mapElement) != RCT2_GLOBAL(0x00F44193, uint8))
						continue;
					
					int ebx = (mapElement->properties.track.type << 4) + (mapElement->properties.track.sequence & 0x0F);
					int eax = (direction + 2 - mapElement->type) & MAP_ELEMENT_DIRECTION_MASK;
					if (RCT2_ADDRESS(0x0099CA64, uint8)[ebx] & (1 << eax)) {
						RCT2_GLOBAL(0x00F44194, uint8) = direction ^ 2;
						*outDirection = direction ^ 2;
						return;
					}
				} while (!map_element_is_last_for_tile(mapElement++));
			}
			direction = (direction + 1) & 3;
		}
		RCT2_GLOBAL(0x00F44194, uint8) = 0xFF;
	} else {
		mapX = (stationStartXY & 0xFF) * 32;
		mapY = (stationStartXY >> 8) * 32;
		entranceMinX = mapX;
		entranceMinY = mapY;

		mapElement = ride_get_station_start_track_element(ride, RCT2_GLOBAL(0x00F44193, uint8));
		direction = mapElement->type & MAP_ELEMENT_DIRECTION_MASK;
		stationDirection = direction;

	nextTile:
		entranceMaxX = mapX;
		entranceMaxY = mapY;
		mapX -= TileDirectionDelta[direction].x;
		mapY -= TileDirectionDelta[direction].y;
		mapElement = map_get_first_element_at(mapX >> 5, mapY >> 5);
		do {
			if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_TRACK)
				continue;
			if (mapElement->properties.track.ride_index != RCT2_GLOBAL(0x00F44192, uint8))
				continue;
			if (map_get_station(mapElement) != RCT2_GLOBAL(0x00F44193, uint8))
				continue;

			switch (mapElement->properties.track.type) {
			case TRACK_ELEM_END_STATION:
			case TRACK_ELEM_BEGIN_STATION:
			case TRACK_ELEM_MIDDLE_STATION:
				goto nextTile;
			}
		} while (!map_element_is_last_for_tile(mapElement++));

		mapX = entranceMinX;
		if (mapX > entranceMaxX) {
			entranceMinX = entranceMaxX;
			entranceMaxX = mapX;
		}

		mapY = entranceMinY;
		if (mapY > entranceMaxY) {
			entranceMinY = entranceMaxY;
			entranceMaxY = mapY;
		}

		direction = loc_6CD18E(*outX, *outY, entranceMinX - 32, entranceMinY - 32, entranceMaxX + 32, entranceMaxY + 32);
		if (direction != -1 && direction != stationDirection && direction != (stationDirection ^ 2)) {
			RCT2_GLOBAL(0x00F44194, uint8) = direction;
			*outDirection = direction;
			return;
		}
	}
}

bool ride_select_backwards_from_front()
{
	track_begin_end trackBeginEnd;

	sub_6C9627();
	RCT2_GLOBAL(0x00F441D2, uint8) = _currentRideIndex;
	if (track_block_get_previous_from_zero(_currentTrackBeginX, _currentTrackBeginY, _currentTrackBeginZ, _currentRideIndex, _currentTrackPieceDirection, &trackBeginEnd)) {
		_rideConstructionState = RIDE_CONSTRUCTION_STATE_SELECTED;
		_currentTrackBeginX = trackBeginEnd.begin_x;
		_currentTrackBeginY = trackBeginEnd.begin_y;
		_currentTrackBeginZ = trackBeginEnd.begin_z;
		_currentTrackPieceDirection = trackBeginEnd.begin_direction;
		_currentTrackPieceType = trackBeginEnd.begin_element->properties.track.type;
		_currentTrackSelectionFlags = 0;
		_rideConstructionArrowPulseTime = 0;
		return true;
	} else {
		return false;
	}
}

bool ride_select_forwards_from_back()
{
	int x, y, z, direction;

	sub_6C9627();
	RCT2_GLOBAL(0x00F441D2, uint8) = _currentRideIndex;

	x = _currentTrackBeginX;
	y = _currentTrackBeginY;
	z = _currentTrackBeginZ;
	direction = _currentTrackPieceDirection ^ 2;
	rct_xy_element next_track;

	if (track_block_get_next_from_zero(x, y, z, _currentRideIndex, direction, &next_track, &z, &direction)) {
		_rideConstructionState = RIDE_CONSTRUCTION_STATE_SELECTED;
		_currentTrackBeginX = next_track.x;
		_currentTrackBeginY = next_track.y;
		_currentTrackBeginZ = z;
		_currentTrackPieceDirection = (next_track.element->type & MAP_ELEMENT_DIRECTION_MASK);
		_currentTrackPieceType = next_track.element->properties.track.type;
		_currentTrackSelectionFlags = 0;
		_rideConstructionArrowPulseTime = 0;
		return true;
	} else {
		return false;
	}
}

money32 ride_remove_track_piece(int x, int y, int z, int direction, int type)
{
	RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TITLE, rct_string_id) = STR_RIDE_CONSTRUCTION_CANT_REMOVE_THIS;
	return game_do_command(x, (GAME_COMMAND_FLAG_APPLY) | ((direction & 3) << 8), y, type, GAME_COMMAND_REMOVE_TRACK, z, 0);
}

/**
 *
 * rct2: 0x006B58EF
 */
bool ride_are_all_possible_entrances_and_exits_built(rct_ride *ride)
{
	if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_IS_SHOP))
		return true;
	
	for (int i = 0; i < 4; i++) {
		if (ride->station_starts[i] == 0xFFFF) continue;
		if (ride->entrances[i] == 0xFFFF) {
			RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TEXT, rct_string_id) = STR_ENTRANCE_NOT_YET_BUILT;
			return false;
		}
		if (ride->exits[i] == 0xFFFF) {
			RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TEXT, rct_string_id) = STR_EXIT_NOT_YET_BUILT;
			return false;
		}
	}
	return true;
}

/**
 *
 * rct2: 0x006B59C6
 */
void invalidate_test_results(int rideIndex)
{
	rct_ride *ride = GET_RIDE(rideIndex);

	ride_measurement_clear(ride);
	ride->excitement = 0xFFFF;
	ride->lifecycle_flags &= ~RIDE_LIFECYCLE_TESTED;
	ride->lifecycle_flags &= ~RIDE_LIFECYCLE_TEST_IN_PROGRESS;
	if (ride->lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK) {
		for (int i = 0; i < ride->num_vehicles; i++) {
			uint16 spriteIndex = ride->vehicles[i];
			if (spriteIndex != SPRITE_INDEX_NULL) {
				rct_vehicle *vehicle = &(g_sprite_list[spriteIndex].vehicle);
				vehicle->var_48 &= ~(1 << 5);
			}
		}
	}
	window_invalidate_by_number(WC_RIDE, rideIndex);
}

/**
 *
 * rct2: 0x006B7481
 */
void ride_fix_breakdown(int rideIndex, int reliabilityIncreaseFactor)
{
	rct_ride *ride = GET_RIDE(rideIndex);

	ride->lifecycle_flags &= ~RIDE_LIFECYCLE_BREAKDOWN_PENDING;
	ride->lifecycle_flags &= ~RIDE_LIFECYCLE_BROKEN_DOWN;
	ride->lifecycle_flags &= ~RIDE_LIFECYCLE_DUE_INSPECTION;
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_CUSTOMER;

	if (ride->lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK) {
		rct_vehicle *vehicle;
		uint16 spriteIndex;

		for (int i = 0; i < ride->num_vehicles; i++) {
			spriteIndex = ride->vehicles[i];
			while (spriteIndex != SPRITE_INDEX_NULL) {
				vehicle = &(g_sprite_list[spriteIndex].vehicle);
				vehicle->var_48 &= ~(1 << 7);
				vehicle->var_48 &= ~(1 << 8);
				vehicle->var_48 &= ~(1 << 9);
				spriteIndex = vehicle->next_vehicle_on_train;
			}
		}
	}

	uint8 unreliability = 100 - ((ride->reliability >> 8) & 0xFF);
	ride->reliability += reliabilityIncreaseFactor * (unreliability / 2);
}

/**
 *
 * rct2: 0x006DE102
 */
static void ride_update_vehicle_colours(int rideIndex)
{
	rct_ride *ride;
	rct_vehicle *vehicle;
	rct_vehicle_colour colours;
	uint16 spriteIndex;
	uint8 coloursExtended;

	ride = GET_RIDE(rideIndex);
	if (ride->type == RIDE_TYPE_SPACE_RINGS || ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_16)) {
		gfx_invalidate_screen();
	}

	for (int i = 0; i < 32; i++) {
		int carIndex = 0;
		spriteIndex = ride->vehicles[i];
		while (spriteIndex != SPRITE_INDEX_NULL) {
			vehicle = &(g_sprite_list[spriteIndex].vehicle);
			switch (ride->colour_scheme_type & 3) {
			case RIDE_COLOUR_SCHEME_ALL_SAME:
				colours = ride->vehicle_colours[0];
				coloursExtended = ride->vehicle_colours_extended[0];
				break;
			case RIDE_COLOUR_SCHEME_DIFFERENT_PER_TRAIN:
				colours = ride->vehicle_colours[i];
				coloursExtended = ride->vehicle_colours_extended[i];
				break;
			case RIDE_COLOUR_SCHEME_DIFFERENT_PER_CAR:
				colours = ride->vehicle_colours[carIndex];
				coloursExtended = ride->vehicle_colours_extended[carIndex];
				break;
			}

			vehicle->colours = colours;
			vehicle->colours_extended = coloursExtended;
			invalidate_sprite((rct_sprite*)vehicle);
			spriteIndex = vehicle->next_vehicle_on_train;
			carIndex++;
		}
	}
}

/**
 * 
 * rct2: 0x006DE4CD
 * trainLayout: Originally fixed to 0x00F64E38. This no longer postfixes with 255.
 */
void ride_entry_get_train_layout(int rideEntryIndex, int numCarsPerTrain, uint8 *trainLayout)
{
	rct_ride_type *rideEntry = GET_RIDE_ENTRY(rideEntryIndex);
	
	for (int i = 0; i < numCarsPerTrain; i++) {
		uint8 vehicleType = rideEntry->default_vehicle;
		if (i == 0 && rideEntry->front_vehicle != 255) {
			vehicleType = rideEntry->front_vehicle;
		} else if (i == 1 && rideEntry->second_vehicle != 255) {
			vehicleType = rideEntry->second_vehicle;
		} else if (i == 2 && rideEntry->third_vehicle != 255) {
			vehicleType = rideEntry->third_vehicle;
		} else if (i == numCarsPerTrain - 1 && rideEntry->rear_vehicle != 255) {
			vehicleType = rideEntry->rear_vehicle;
		}
		trainLayout[i] = vehicleType;
	}
}

int ride_get_smallest_station_length(rct_ride *ride)
{
	uint32 result = -1;
	for (int i = 0; i < 4; i++) {
		if (ride->station_starts[i] != 0xFFFF) {
			result = min(result, (uint32)(ride->station_length[i] & 0x0F));
		}
	}
	return (int)result;
}

/**
 *
 *  rct2: 0x006CB3AA
 */
static int ride_get_track_length(rct_ride *ride)
{
	rct_window *w;
	rct_map_element *mapElement;
	track_circuit_iterator it;
	int x, y, z, trackType, rideIndex, result;

	for (int i = 0; i < 4; i++) {
		uint16 xy = ride->station_starts[i];
		if (xy == 0xFFFF)
			continue;

		x = (xy & 0xFF) * 32;
		y = (xy >> 8) * 32;
		z = ride->station_heights[i];

		mapElement = map_get_first_element_at(x >> 5, y >> 5);
		do {
			if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_TRACK)
				continue;

			trackType = mapElement->properties.track.type;
			if (!RCT2_GLOBAL(0x0099BA64 + (trackType * 16), uint32) & 0x10)
				continue;

			if (mapElement->base_height != z)
				continue;

			goto foundTrack;
		} while (!map_element_is_last_for_tile(mapElement++));
	}
	return 0;

foundTrack:
	rideIndex = mapElement->properties.track.ride_index;

	w = window_find_by_class(WC_RIDE_CONSTRUCTION);
	if (w != NULL && _rideConstructionState != RIDE_CONSTRUCTION_STATE_0 && _currentRideIndex == rideIndex) {
		sub_6C9627();
	}

	result = 0;
	track_circuit_iterator_begin(&it, (rct_xy_element){ x, y, mapElement });
	while (track_circuit_iterator_next(&it)) {
		trackType = it.current.element->properties.track.type;
		result += TrackPieceLengths[trackType];
	}
	return result;
}

/**
 *
 *  rct2: 0x006DD57D
 */
void ride_update_max_vehicles(int rideIndex)
{
	rct_ride *ride;
	rct_ride_type *rideEntry;
	rct_ride_type_vehicle *vehicleEntry;
	uint8 trainLayout[16], numCarsPerTrain, numVehicles;
	int trainLength, maxNumTrains;

	ride = GET_RIDE(rideIndex);
	if (ride->subtype == 0xFF)
		return;

	rideEntry = GET_RIDE_ENTRY(ride->subtype);
	if (rideEntry->cars_per_flat_ride == 0xFF) {
		ride->num_cars_per_train = max(rideEntry->min_cars_in_train, ride->num_cars_per_train);
		ride->min_max_cars_per_train = rideEntry->max_cars_in_train | (rideEntry->min_cars_in_train << 4);

		// Calculate maximum train length based on smallest station length
		int stationLength = ride_get_smallest_station_length(ride);
		if (stationLength == -1)
			return;

		stationLength = (stationLength * 0x44180) - 0x16B2A;
		int maxFriction = RCT2_GLOBAL(0x0097D21B + (ride->type * 8), uint8) << 8;
		int maxCarsPerTrain = 1;
		for (int numCars = rideEntry->max_cars_in_train; numCars > 0; numCars--) {
			ride_entry_get_train_layout(ride->subtype, numCars, trainLayout);
			trainLength = 0;
			int totalFriction = 0;
			for (int i = 0; i < numCars; i++) {
				vehicleEntry = &rideEntry->vehicles[trainLayout[i]];
				trainLength += vehicleEntry->var_04;
				totalFriction += vehicleEntry->var_08;
			}

			if (trainLength <= stationLength && totalFriction <= maxFriction) {
				maxCarsPerTrain = numCars;
				break;
			}
		}
		maxCarsPerTrain = max(maxCarsPerTrain, rideEntry->min_cars_in_train);
		ride->min_max_cars_per_train = maxCarsPerTrain | (rideEntry->min_cars_in_train << 4);

		switch (ride->mode) {
		case RIDE_MODE_CONTINUOUS_CIRCUIT_BLOCK_SECTIONED:
		case RIDE_MODE_POWERED_LAUNCH_BLOCK_SECTIONED:
			maxNumTrains = clamp(1, ride->num_stations + ride->num_block_brakes - 1, 31);
			break;
		case RIDE_MODE_REVERSE_INCLINE_LAUNCHED_SHUTTLE:
		case RIDE_MODE_POWERED_LAUNCH_PASSTROUGH:
		case RIDE_MODE_SHUTTLE:
		case RIDE_MODE_LIM_POWERED_LAUNCH:
		case RIDE_MODE_POWERED_LAUNCH:
			maxNumTrains = 1;
			break;
		default:
			// Calculate maximum number of trains
			ride_entry_get_train_layout(ride->subtype, maxCarsPerTrain, trainLayout);
			trainLength = 0;
			for (int i = 0; i < maxCarsPerTrain; i++) {
				vehicleEntry = &rideEntry->vehicles[trainLayout[i]];
				trainLength += vehicleEntry->var_04;
			}

			int totalLength = trainLength / 2;
			if (maxCarsPerTrain != 1)
				totalLength /= 2;

			maxNumTrains = 0;
			do {
				maxNumTrains++;
				totalLength += trainLength;
			} while (totalLength <= stationLength);

			if (
				(ride->mode != RIDE_MODE_STATION_TO_STATION && ride->mode != RIDE_MODE_CONTINUOUS_CIRCUIT) ||
				!(RCT2_GLOBAL(0x0097D4F2 + (ride->type * 8), uint16) & 0x40)
			) {
				maxNumTrains = min(maxNumTrains, 31);
			} else {
				ride_entry_get_train_layout(ride->subtype, maxCarsPerTrain, trainLayout);
				vehicleEntry = &rideEntry->vehicles[trainLayout[0]];
				int unk = vehicleEntry->var_5C;

				int totalSpacing = 0;
				for (int i = 0; i < maxCarsPerTrain; i++) {
					vehicleEntry = &rideEntry->vehicles[trainLayout[i]];
					totalSpacing += vehicleEntry->var_04;
				}

				totalSpacing >>= 13;
				int trackLength = ride_get_track_length(ride) / 4;
				if (unk > 10) trackLength = (trackLength * 3) / 4;
				if (unk > 25) trackLength = (trackLength * 3) / 4;
				if (unk > 40) trackLength = (trackLength * 3) / 4;

				maxNumTrains = 0;
				int length = 0;
				do {
					maxNumTrains++;
					length += totalSpacing;
				} while (maxNumTrains < 31 && length < trackLength);
			}
			break;
		}
		ride->max_trains = maxNumTrains;

		numCarsPerTrain = min(ride->var_0CB, maxCarsPerTrain);
		numVehicles = min(ride->var_0CA, maxNumTrains);
	} else {
		ride->max_trains = rideEntry->cars_per_flat_ride;
		ride->min_max_cars_per_train = rideEntry->max_cars_in_train | (rideEntry->min_cars_in_train << 4);
		numCarsPerTrain = rideEntry->max_cars_in_train;
		numVehicles = min(ride->var_0CA, rideEntry->cars_per_flat_ride);
	}

	// Refresh new current num vehicles / num cars per vehicle
	if (numVehicles != ride->num_vehicles || numCarsPerTrain != ride->num_cars_per_train) {
		ride->num_cars_per_train = numCarsPerTrain;
		ride->num_vehicles = numVehicles;
		window_invalidate_by_number(WC_RIDE, rideIndex);
	}
}

static void sub_6DE52C(rct_ride *ride)
{
	RCT2_CALLPROC_X(0x006DE52C, 0, 0, 0, 0, (int)ride, 0, 0);
}

void ride_set_ride_entry(int rideIndex, int rideEntry)
{
	RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TITLE, rct_string_id) = STR_RIDE_SET_VEHICLE_TYPE_FAIL;
	game_do_command(
		0,
		GAME_COMMAND_FLAG_APPLY | (RIDE_SET_VEHICLES_COMMAND_TYPE_RIDE_ENTRY << 8),
		0,
		(rideEntry << 8) | rideIndex,
		GAME_COMMAND_SET_RIDE_VEHICLES,
		0,
		0
	);
}

void ride_set_num_vehicles(int rideIndex, int numVehicles)
{
	RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TITLE, rct_string_id) = STR_RIDE_SET_VEHICLE_SET_NUM_TRAINS_FAIL;
	game_do_command(
		0,
		GAME_COMMAND_FLAG_APPLY | (RIDE_SET_VEHICLES_COMMAND_TYPE_NUM_TRAINS << 8),
		0,
		(numVehicles << 8) | rideIndex,
		GAME_COMMAND_SET_RIDE_VEHICLES,
		0,
		0
	);
}

void ride_set_num_cars_per_vehicle(int rideIndex, int numCarsPerVehicle)
{
	RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TITLE, rct_string_id) = STR_RIDE_SET_VEHICLE_SET_NUM_CARS_PER_TRAIN_FAIL;
	game_do_command(
		0,
		GAME_COMMAND_FLAG_APPLY | (RIDE_SET_VEHICLES_COMMAND_TYPE_NUM_CARS_PER_TRAIN << 8),
		0,
		(numCarsPerVehicle << 8) | rideIndex,
		GAME_COMMAND_SET_RIDE_VEHICLES,
		0,
		0
	);
}

/**
 *
 *  rct2: 0x006B52D4
 */
void game_command_set_ride_vehicles(int *eax, int *ebx, int *ecx, int *edx, int *esi, int *edi, int *ebp)
{
	rct_ride *ride;
	rct_ride_type *rideEntry;
	rct_window *w;
	int rideIndex, commandType, value;

	commandType = (*ebx >> 8) & 0xFF;
	rideIndex = *edx & 0xFF;
	value = (*edx >> 8) & 0xFF;

	ride = GET_RIDE(rideIndex);

	RCT2_GLOBAL(RCT2_ADDRESS_NEXT_EXPENDITURE_TYPE, uint8) = RCT_EXPENDITURE_TYPE_RIDE_RUNNING_COSTS * 4;

	if (ride->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN) {
		RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TEXT, rct_string_id) = STR_HAS_BROKEN_DOWN_AND_REQUIRES_FIXING;
		*ebx = MONEY32_UNDEFINED;
		return;
	}

	if (ride->status != RIDE_STATUS_CLOSED) {
		RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TEXT, rct_string_id) = STR_MUST_BE_CLOSED_FIRST;
		*ebx = MONEY32_UNDEFINED;
		return;
	}

	if (!(*ebx & GAME_COMMAND_FLAG_APPLY)) {
		*ebx = 0;
		return;
	}

	invalidate_test_results(rideIndex);
	ride_clear_for_construction(rideIndex);
	ride_remove_peeps(rideIndex);
	ride->var_1CA = 100;
	if (ride->type != RIDE_TYPE_ENTERPRISE) {
		gfx_invalidate_screen();
	}

	switch (commandType) {
	case RIDE_SET_VEHICLES_COMMAND_TYPE_NUM_TRAINS:
		ride->var_0CA = value;
		if (ride->type != RIDE_TYPE_SPACE_RINGS) {
			gfx_invalidate_screen();
		}
		break;
	case RIDE_SET_VEHICLES_COMMAND_TYPE_NUM_CARS_PER_TRAIN:
		rideEntry = GET_RIDE_ENTRY(ride->subtype);
		value = clamp(rideEntry->min_cars_in_train, value, rideEntry->max_cars_in_train);
		ride->var_0CB = value;
		break;
	case RIDE_SET_VEHICLES_COMMAND_TYPE_RIDE_ENTRY:
		ride->subtype = value;
		sub_6DE52C(ride);
		break;
	}

	ride->num_circuits = 1;
	ride_update_max_vehicles(rideIndex);

	w = window_find_by_number(WC_RIDE, rideIndex);
	if (w != NULL) {
		if (w->page == 4) {
			w->var_48C = 0;
		}
		window_invalidate(w);
	}
	*ebx = 0;
}

/**
 *
 *  rct2: 0x006CB945
 */
void sub_6CB945(int rideIndex)
{
	RCT2_CALLPROC_X(0x006CB945, 0, 0, 0, rideIndex, 0, 0, 0);
}

money32 place_ride_entrance_or_exit(sint16 x, sint16 y, sint16 z, uint8 direction, uint8 flags, uint8 rideIndex, uint8 station_num, uint8 is_exit){
	// Remember when in Unknown station num mode rideIndex is unknown and z is set
	// When in known station num mode rideIndex is known and z is unknown

	RCT2_GLOBAL(0x009E32B8, uint32) = 0;
	RCT2_GLOBAL(0x009DEA5E, sint16) = x;
	RCT2_GLOBAL(0x009DEA60, sint16) = y;

	if (!sub_68B044()) {
		return MONEY32_UNDEFINED;
	}

	if (RCT2_GLOBAL(RCT2_ADDRESS_GAME_PAUSED, uint8) != 0){
		RCT2_GLOBAL(0x00141E9AC, rct_string_id) = 2214;
		return MONEY32_UNDEFINED;
	}

	if (station_num == 0xFF){
		z *= 16;
		if (flags & GAME_COMMAND_FLAG_APPLY)
			return MONEY32_UNDEFINED;

		if (!gCheatsSandboxMode && !map_is_location_owned(x, y, z)){
			return MONEY32_UNDEFINED;
		}

		sint16 clear_z = z / 8 + (is_exit ? 5 : 7);
		RCT2_GLOBAL(0x009E32C4, sint16) = x;
		RCT2_GLOBAL(0x009E32C6, sint16) = y;

		// Horrible hack until map_can_construct_with_clear_at is implemented.
		RCT2_GLOBAL(0x009E32C8, uint8*) = (&flags) - 4;

		if (!gCheatsDisableClearanceChecks && !map_can_construct_with_clear_at(x, y, z / 8, clear_z, (void*)0x0066637E, 0xF)){
			return MONEY32_UNDEFINED;
		}

		if (RCT2_GLOBAL(0x00F1AD60, uint8) & (1 << 2)){
			RCT2_GLOBAL(0x00141E9AC, rct_string_id) = 1031;
			return MONEY32_UNDEFINED;
		}

		if (z > 1952){
			RCT2_GLOBAL(0x00141E9AC, rct_string_id) = 878;
			return MONEY32_UNDEFINED;
		}

	}
	else{
		rct_ride* ride = GET_RIDE(rideIndex);
		if (ride->status != RIDE_STATUS_CLOSED){
			RCT2_GLOBAL(0x00141E9AC, rct_string_id) = 1006;
			return MONEY32_UNDEFINED;
		}

		if (ride->lifecycle_flags & RIDE_LIFECYCLE_INDESTRUCTIBLE_TRACK){
			RCT2_GLOBAL(0x00141E9AC, rct_string_id) = 3092;
			return MONEY32_UNDEFINED;
		}

		ride_clear_for_construction(rideIndex);
		ride_remove_peeps(rideIndex);

		uint8 requires_remove = 0;
		sint16 remove_x = 0;
		sint16 remove_y = 0;

		if (is_exit){
			if (ride->exits[station_num] != 0xFFFF){
				if (flags & (1 << 6)){
					RCT2_GLOBAL(0x00141E9AC, rct_string_id) = 0;
					return MONEY32_UNDEFINED;
				}

				remove_x = (ride->exits[station_num] & 0xFF) * 32;
				remove_y = ((ride->exits[station_num] >> 8) & 0xFF) * 32;
				requires_remove = 1;
			}
		}
		else{
			if (ride->entrances[station_num] != 0xFFFF){
				if (flags & (1 << 6)){
					RCT2_GLOBAL(0x00141E9AC, rct_string_id) = 0;
					return MONEY32_UNDEFINED;
				}

				remove_x = (ride->entrances[station_num] & 0xFF) * 32;
				remove_y = ((ride->entrances[station_num] >> 8) & 0xFF) * 32;
				requires_remove = 1;
			}
		}

		if (requires_remove){
			money32 success = game_do_command(
				remove_x,
				flags,
				remove_y,
				rideIndex,
				GAME_COMMAND_REMOVE_RIDE_ENTRANCE_OR_EXIT,
				station_num,
				0
				);

			if (success == MONEY32_UNDEFINED){
				return MONEY32_UNDEFINED;
			}
		}

		z = ride->station_heights[station_num] * 8;
		RCT2_GLOBAL(0x009DEA62, sint16) = z;

		if (flags & GAME_COMMAND_FLAG_APPLY && !(flags & 0x48)){
			footpath_remove_litter(x, y, z);
			map_remove_walls_at_z(x, y, z);
		}

		if (!gCheatsSandboxMode && !map_is_location_owned(x, y, z)){
			return MONEY32_UNDEFINED;
		}

		sint8 clear_z = (z / 8) + (is_exit ? 5 : 7);
		RCT2_GLOBAL(0x009E32C4, sint16) = x;
		RCT2_GLOBAL(0x009E32C6, sint16) = y;

		// Horrible hack until map_can_construct_with_clear_at is implemented.
		RCT2_GLOBAL(0x009E32C8, uint8*) = (&flags) - 4;

		if (!gCheatsDisableClearanceChecks && !map_can_construct_with_clear_at(x, y, z / 8, clear_z, (void*)0x0066637E, 0xF)){
			return MONEY32_UNDEFINED;
		}

		if (RCT2_GLOBAL(0x00F1AD60, uint8) & (1 << 2)){
			RCT2_GLOBAL(0x00141E9AC, rct_string_id) = 1031;
			return MONEY32_UNDEFINED;
		}

		if (z / 8 > 244){
			RCT2_GLOBAL(0x00141E9AC, rct_string_id) = 878;
			return MONEY32_UNDEFINED;
		}

		if (flags & GAME_COMMAND_FLAG_APPLY){

			rct_map_element* mapElement = map_element_insert(x / 32, y / 32, z / 8, 0xF);
			mapElement->clearance_height = clear_z;
			mapElement->properties.entrance.type = is_exit;
			mapElement->properties.entrance.index = station_num << 4;
			mapElement->properties.entrance.ride_index = rideIndex;
			mapElement->type = MAP_ELEMENT_TYPE_ENTRANCE | direction;

			if (flags & (1 << 6)){
				mapElement->flags |= MAP_ELEMENT_FLAG_GHOST;
			}

			if (is_exit){
				ride->exits[station_num] = (x / 32) | (y / 32 << 8);
			}
			else{
				ride->entrances[station_num] = (x / 32) | (y / 32 << 8);
				ride->first_peep_in_queue[station_num] = 0xFFFF;
				ride->queue_length[station_num] = 0;

				map_animation_create(MAP_ANIMATION_TYPE_RIDE_ENTRANCE, x, y, z / 8);
			}

			sub_6A7594();
			if (!(flags & (1 << 6))){
				RCT2_CALLPROC_X(0x00666CBE, x, 0, y, 0, (int)mapElement, 0, 0);
			}
			footpath_connect_edges(x, y, mapElement, flags);
			sub_6A759F();

			map_invalidate_tile_full(x, y);
		}
	}

	RCT2_GLOBAL(RCT2_ADDRESS_NEXT_EXPENDITURE_TYPE, uint8) = RCT_EXPENDITURE_TYPE_RIDE_CONSTRUCTION * 4;
	return RCT2_GLOBAL(0x009E32B8, money32);
}

/* rct2: 0x006660A8 */
void game_command_place_ride_entrance_or_exit(int *eax, int *ebx, int *ecx, int *edx, int *esi, int *edi, int *ebp){
	*ebx = place_ride_entrance_or_exit(
		*eax & 0xFFFF,
		*ecx & 0xFFFF,
		*edx & 0xFF,
		(*ebx >> 8) & 0xFF,
		*ebx & 0xFF,
		*edx & 0xFF,
		*edi & 0xFF,
		(*edx >> 8) & 0xFF
		);
}

money32 remove_ride_entrance_or_exit(sint16 x, sint16 y, uint8 rideIndex, uint8 station_num, uint8 flags){
	rct_ride* ride = GET_RIDE(rideIndex);

	if (!(flags & (1 << 6))){
		if (RCT2_GLOBAL(RCT2_ADDRESS_GAME_PAUSED, uint8) != 0){
			RCT2_GLOBAL(0x00141E9AC, rct_string_id) = 2214;
			return MONEY32_UNDEFINED;
		}
	}

	if (ride->status != RIDE_STATUS_CLOSED){
		RCT2_GLOBAL(0x00141E9AC, rct_string_id) = 1006;
		return MONEY32_UNDEFINED;
	}

	if (flags & GAME_COMMAND_FLAG_APPLY){
		ride_clear_for_construction(rideIndex);
		ride_remove_peeps(rideIndex);
		invalidate_test_results(rideIndex);

		uint8 found = 0;
		rct_map_element* mapElement = map_get_first_element_at(x / 32, y / 32);
		do{
			if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_ENTRANCE)
				continue;

			if (mapElement->base_height != ride->station_heights[station_num])
				continue;

			if (flags & (1 << 5) && !(mapElement->flags & MAP_ELEMENT_FLAG_GHOST))
				continue;

			found = 1;
			break;
		} while (!map_element_is_last_for_tile(mapElement++));

		if (!found){
			return MONEY32_UNDEFINED;
		}

		sub_6A7594();
		RCT2_CALLPROC_X(0x00666D6F, x, 0, y, 0, (int)mapElement, 0, 0);
		footpath_remove_edges_at(x, y, mapElement);

		uint8 is_exit = mapElement->properties.entrance.type;

		map_element_remove(mapElement);

		if (is_exit){
			ride->exits[station_num] = 0xFFFF;
		}
		else{
			ride->entrances[station_num] = 0xFFFF;
		}

		sub_6A759F();

		map_invalidate_tile_full(x, y);
	}

	RCT2_GLOBAL(RCT2_ADDRESS_NEXT_EXPENDITURE_TYPE, uint8) = RCT_EXPENDITURE_TYPE_RIDE_CONSTRUCTION * 4;
	return 0;
}

/* rct2: 0x0066640B */
void game_command_remove_ride_entrance_or_exit(int *eax, int *ebx, int *ecx, int *edx, int *esi, int *edi, int *ebp){
	*ebx = remove_ride_entrance_or_exit(
		*eax & 0xFFFF,
		*ecx & 0xFFFF,
		*edx & 0xFF,
		*edi & 0xFF,
		*ebx & 0xFF
		);
}

/**
 *
 *  rct2: 0x006B752C
 */
void ride_crash(int rideIndex, int vehicleIndex)
{
	rct_ride *ride;
	rct_vehicle *vehicle;
	rct_window *w;

	// TODO Remove these when hook is no longer used
	rideIndex &= 0xFF;
	vehicleIndex &= 0xFF;

	ride = GET_RIDE(rideIndex);
	vehicle = &(g_sprite_list[ride->vehicles[vehicleIndex]]).vehicle;

	w = window_ride_open_vehicle(vehicle);
	if (w->viewport != NULL) {
		w->viewport->flags |= VIEWPORT_FLAG_SOUND_ON;
	}

	RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS + 0, rct_string_id) = ride->name;
	RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS + 2, uint32) = ride->name_arguments;
	news_item_add_to_queue(NEWS_ITEM_RIDE, STR_RIDE_HAS_CRASHED, rideIndex);
}

bool ride_type_is_intamin(int rideType)
{
	return
		rideType == RIDE_TYPE_HEARTLINE_TWISTER_COASTER ||
		rideType == RIDE_TYPE_GIGA_COASTER ||
		rideType == RIDE_TYPE_INVERTED_IMPULSE_COASTER;
}
