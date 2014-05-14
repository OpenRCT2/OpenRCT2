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
#include "finance.h"
#include "map.h"
#include "park.h"
#include "peep.h"
#include "ride.h"
#include "scenario.h"
#include "sprite.h"
#include "strings.h"
#include "window.h"

int park_is_open()
{
	return (RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & PARK_FLAGS_PARK_OPEN) != 0;
}

/**
 * 
 *  rct2: 0x00667132
 */
void park_init()
{
	int i;

	RCT2_GLOBAL(0x013CA740, uint8) = 0;
	RCT2_GLOBAL(0x013573D4, uint16) = 777;
	RCT2_GLOBAL(RCT2_ADDRESS_HANDYMAN_COLOUR, uint8) = 28;
	RCT2_GLOBAL(RCT2_ADDRESS_MECHANIC_COLOUR, uint8) = 28;
	RCT2_GLOBAL(RCT2_ADDRESS_SECURITY_COLOUR, uint8) = 28;
	RCT2_GLOBAL(RCT2_ADDRESS_GUESTS_IN_PARK, uint16) = 0;
	RCT2_GLOBAL(0x01357BC8, uint16) = 0;
	RCT2_GLOBAL(0x01357846, uint16) = 0;
	RCT2_GLOBAL(0x013573FE, uint16) = 0;
	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_PARK_RATING, uint16) = 0;
	RCT2_GLOBAL(0x013580EC, uint16) = 0;
	RCT2_GLOBAL(0x013580EE, uint16) = 0;
	RCT2_GLOBAL(0x01357CF4, sint32) = -1;

	for (i = 0; i < 20; i++)
		RCT2_ADDRESS(0x01358102, uint8)[i] = 0;

	RCT2_GLOBAL(0x01358844, uint32) = 0xFFFFFFFF;
	RCT2_GLOBAL(0x01358849, uint32) = 0xFFFFFFFE;
	RCT2_GLOBAL(0x0135884E, uint32) = 0xFFFFFFFD;
	finance_init();

	for (i = 0; i < 2; i++)
		RCT2_ADDRESS(0x01357404, uint32)[i] = 0;

	for (i = 0; i < 56; i++)
		RCT2_ADDRESS(0x01357BD0, sint32)[i] = -1;

	RCT2_GLOBAL(RCT2_ADDRESS_PARK_ENTRANCE_FEE, uint16) = CURRENCY(10, 00);
	RCT2_GLOBAL(0x013573F2, sint16) = -1;
	RCT2_GLOBAL(0x013573F8, sint16) = -1;
	RCT2_GLOBAL(0x01357CF2, uint16) = 127;
	RCT2_GLOBAL(0x013573FF, uint8) = 2;

	RCT2_GLOBAL(0x013580F4, uint16) = 500;
	RCT2_GLOBAL(0x013580E9, uint8) = 128;
	RCT2_GLOBAL(0x013580F6, uint8) = 200;
	RCT2_GLOBAL(0x013580F7, uint8) = 200;
	RCT2_GLOBAL(RCT2_ADDRESS_OBJECTIVE_TYPE, uint8) = 1;
	RCT2_GLOBAL(RCT2_ADDRESS_OBJECTIVE_YEAR, uint8) = 4;
	RCT2_GLOBAL(RCT2_ADDRESS_OBJECTIVE_NUM_GUESTS, uint16) = 1000;
	RCT2_GLOBAL(0x01358770, uint16) = 900;
	RCT2_GLOBAL(0x01358772, uint16) = 400;
	RCT2_GLOBAL(0x01358774, uint16) = 0;
	RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) = PARK_FLAGS_11 | PARK_FLAGS_SHOW_REAL_GUEST_NAMES;
	park_reset_awards_and_history();

	rct_s6_info *info = (rct_s6_info*)0x0141F570;
	info->name[0] = '\0';
	format_string(info->details, STR_NO_DETAILS_YET, NULL);
}

/**
 * 
 *  rct2: 0x0066729F
 */
void park_reset_awards_and_history()
{
	int i;

	// Reset park rating and guests in park history
	for (i = 0; i < 32; i++) {
		RCT2_ADDRESS(RCT2_ADDRESS_PARK_RATING_HISTORY, uint8)[i] = 255;
		RCT2_ADDRESS(RCT2_ADDRESS_GUESTS_IN_PARK_HISTORY, uint8)[i] = 255;
	}

	// Reset finance history
	for (i = 0; i < 128; i++) {
		RCT2_ADDRESS(RCT2_ADDRESS_BALANCE_HISTORY, uint32)[i] = 0x80000000;
		RCT2_ADDRESS(RCT2_ADDRESS_WEEKLY_PROFIT_HISTORY, uint32)[i] = 0x80000000;
		RCT2_ADDRESS(RCT2_ADDRESS_PARK_VALUE_HISTORY, uint32)[i] = 0x80000000;
	}

	// Reset awards
	for (i = 0; i < 4; i++)
		RCT2_ADDRESS(RCT2_ADDRESS_AWARD_LIST, rct_award)[i].time = 0;
}

/**
 * 
 *  rct2: 0x0066A348
 */
int park_calculate_size()
{
	int tiles, x, y;
	rct_map_element *mapElement;

	tiles = 0;
	for (y = 0; y < 256; y++) {
		for (x = 0; x < 256; x++) {
			mapElement = RCT2_ADDRESS(RCT2_ADDRESS_TILE_MAP_ELEMENT_POINTERS, rct_map_element*)[y * 256 + x];
			while (mapElement->type & MAP_ELEMENT_TYPE_MASK) {
				mapElement++;
			}

			if (mapElement->properties.surface.ownership & 0x30)
				tiles++;
		}
	}

	if (tiles != RCT2_GLOBAL(RCT2_ADDRESS_PARK_SIZE, sint16)) {
		RCT2_GLOBAL(RCT2_ADDRESS_PARK_SIZE, sint16) = tiles;
		window_invalidate_by_id(WC_PARK_INFORMATION, 0);
	}
	
	return tiles;
}

/**
 * 
 *  rct2: 0x00669EAA
 */
int calculate_park_rating()
{
	int result;

	result = 1150;
	if (RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & 0x4000)
		result = 1050;
	
	// Guests
	{
		rct_peep* peep;
		uint16 sprite_idx;
		int num_happy_peeps;
		short _bp;
		
		// -150 to +3 based on a range of guests from 0 to 2000
		result -= 150 - (min(2000, RCT2_GLOBAL(RCT2_ADDRESS_GUESTS_IN_PARK, uint16)) / 13);

		// Guests, happiness, ?
		num_happy_peeps = 0;
		_bp = 0;
		for (sprite_idx = RCT2_GLOBAL(RCT2_ADDRESS_SPRITES_START_PEEP, uint16); sprite_idx != SPRITE_INDEX_NULL; sprite_idx = peep->next) {
			peep = &(RCT2_ADDRESS(RCT2_ADDRESS_SPRITE_LIST, rct_sprite)[sprite_idx].peep);
			if (peep->type != PEEP_TYPE_GUEST)
				continue;
			if (peep->var_2A != 0)
				continue;
			if (peep->happiness > 128)
				num_happy_peeps++;
			if (!(peep->flags & PEEP_FLAGS_LEAVING_PARK))
				continue;
			if (peep->var_C6 <= 89)
				_bp++;
		}
		
		// Peep happiness -500 to +0
		result -= 500;

		if (RCT2_GLOBAL(RCT2_ADDRESS_GUESTS_IN_PARK, uint16) > 0)
			result += 2 * min(250, (num_happy_peeps * 300) / RCT2_GLOBAL(RCT2_ADDRESS_GUESTS_IN_PARK, uint16));

		// ?
		_bp -= 25;
		if (_bp >= 0)
			result -= _bp * 7;
	}

	// Rides
	{
		int i;
		short _ax, total_ride_intensity = 0, total_ride_excitement = 0, average_intensity, average_excitement;
		int num_rides, num_exciting_rides = 0;
		rct_ride* ride;

		// 
		_ax = 0;
		num_rides = 0;
		for (i = 0; i < 255; i++) {
			ride = &(RCT2_ADDRESS(RCT2_ADDRESS_RIDE_LIST, rct_ride)[i]);

			if (ride->type == RIDE_TYPE_NULL)
				continue;
			_ax += 100 - ride->var_199;

			if (ride->excitement != -1){
				total_ride_excitement += ride->excitement / 8;
				total_ride_intensity += ride->intensity / 8;
				num_exciting_rides++;
			}
			num_rides++;
		}
		result -= 200;
		if (num_rides > 0)
			result += (_ax / num_rides) * 2;

		result -= 100;

		if (num_exciting_rides>0){
			average_excitement = total_ride_excitement / num_exciting_rides;
			average_intensity = total_ride_intensity / num_exciting_rides;

			average_excitement -= 46;
			if (average_excitement < 0){
				average_excitement = -average_excitement;
			}

			average_intensity -= 65;
			if (average_intensity < 0){
				average_intensity = -average_intensity;
			}

			average_excitement = min(average_excitement / 2, 50);
			average_intensity = min(average_intensity / 2, 50);
			result += 100 - average_excitement - average_intensity;
		}

		total_ride_excitement = min(1000, total_ride_excitement);
		total_ride_intensity = min(1000, total_ride_intensity);
		result -= 200 - ((total_ride_excitement + total_ride_intensity) / 10);
	}

	// Litter
	{
		rct_litter* litter;
		uint16 sprite_idx;
		short num_litter;

		num_litter = 0;
		for (sprite_idx = RCT2_GLOBAL(RCT2_ADDRESS_SPRITES_START_LITTER, uint16); sprite_idx != SPRITE_INDEX_NULL; sprite_idx = litter->next) {
			litter = &(RCT2_ADDRESS(RCT2_ADDRESS_SPRITE_LIST, rct_sprite)[sprite_idx].litter);

			// Guessing this eliminates recently dropped litter
			if (litter->var_24 - RCT2_GLOBAL(0x00F663AC, uint32) >= 7680)
				num_litter++;
		}
		result -= 600 - (4 * (150 - min(150, num_litter)));
	}
	
	result -= RCT2_GLOBAL(0x0135882E, sint16);
	result = clamp(0, result, 999);
	return result;
}

/**
 * 
 *  rct2: 0x0066A3F6
 */
int calculate_park_value()
{
	int result, value, i;
	rct_ride* ride;

	result = 0;
	for (i = 0; i < 255; i++) {
		ride = &(RCT2_ADDRESS(RCT2_ADDRESS_RIDE_LIST, rct_ride)[i]);
		if (ride->type == RIDE_TYPE_NULL)
			continue;
		if (ride->reliability == 0xFFFF)
			continue;
		value = 0;
		value += ride->var_124 + ride->var_126 + ride->var_128 + ride->var_12A;
		value += ride->var_12C + ride->var_12E + ride->age + ride->running_cost;
		value += ride->var_134 + ride->var_136;
		value += *((uint8*)(0x0097D21E + (ride->type * 8))) * 4;
		value *= ride->reliability * 10;
		result += value;
	}

	result += RCT2_GLOBAL(RCT2_ADDRESS_GUESTS_IN_PARK, uint16) * 70;

	return result;
}

/**
 * 
 *  rct2: 0x0066A498
 */
int calculate_company_value()
{
	int result;

	result = DECRYPT_MONEY(RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_MONEY_ENCRYPTED, sint32));
	result += RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_PARK_VALUE, sint32);
	result -= RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_LOAN, sint32);

	return result;
}

/**
*
*  rct2: 0x00667104
*/
void reset_park_entrances() {
	RCT2_GLOBAL(0x013573D4, uint16) = 0;

	for (short i = 0; i < 4; i++) {
		RCT2_ADDRESS(RCT2_ADDRESS_PARK_ENTRANCE_X, uint16)[i] = 0x8000;
	}

	RCT2_GLOBAL(0x013573F2, uint16) = 0xFFFF;
	RCT2_GLOBAL(0x013573F8, uint16) = 0xFFFF;
}
