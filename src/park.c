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
#include "park.h"
#include "peep.h"
#include "ride.h"
#include "sprite.h"
#include "window.h"

int park_is_open()
{
	return (RCT2_GLOBAL(RCT2_ADDRESS_GAME_FLAGS, uint32) & GAME_FLAGS_PARK_OPEN) != 0;
}

/**
 * 
 *  rct2: 0x00667132
 */
void park_init()
{
	RCT2_CALLPROC_EBPSAFE(0x00667132);
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

	if (tiles != RCT2_GLOBAL(0x013580EA, sint16)) {
		RCT2_GLOBAL(0x013580EA, sint16) = tiles;
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
	{
		int eax, ebx, ecx, edx, esi, edi, ebp;
		RCT2_CALLFUNC_X(0x00669EAA, &eax, &ebx, &ecx, &edx, &esi, &edi, &ebp);
		return eax & 0xFFFF;
	}


	int result;

	result = 1150;
	if (RCT2_GLOBAL(RCT2_ADDRESS_GAME_FLAGS, uint32) & 0x4000)
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
			if (!(peep->var_C8 & 0x01))
				continue;
			if (peep->var_C6 <= 89)
				_bp++;
		}

		// Peep happiness -500 to +0
		result -= 500;
		if (num_happy_peeps > 0)
			result += 2 * min(250, (RCT2_GLOBAL(RCT2_ADDRESS_GUESTS_IN_PARK, uint16) * 300) / num_happy_peeps);

		// ?
		_bp -= 25;
		if (_bp >= 0)
			result -= _bp * 7;
	}

	// Rides
	{
		int i;
		short _ax, _bx;
		int num_rides;
		rct_ride* ride;

		// 
		_ax = 0;
		num_rides = 0;
		for (i = 0; i < 255; i++) {
			ride = &(RCT2_ADDRESS(RCT2_ADDRESS_RIDE_LIST, rct_ride)[i]);

			if (ride->type == RIDE_TYPE_NULL)
				continue;
			_ax += 100 - ride->var_199;
			num_rides++;
		}
		result -= 200;
		if (num_rides > 0)
			result += (_ax / num_rides) * 2;

		// 
		_ax = 0;
		_bx = 0;
		for (i = 0; i < 255; i++) {
			ride = &(RCT2_ADDRESS(RCT2_ADDRESS_RIDE_LIST, rct_ride)[i]);

			if (ride->type == RIDE_TYPE_NULL)
				continue;
			if (ride->var_140 == -1)
				continue;
			_ax += ride->var_140 / 8;
			_bx += ride->var_142 / 8;
		}
		_ax = min(1000, _ax);
		_bx = min(1000, _bx);
		result -= 200 - ((_ax + _bx) / 10);
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
			if (litter->var_24 - RCT2_GLOBAL(0x00F663AC, sint32) >= 7680)
				num_litter++;
		}
		result -= 600 - (4 * (150 - min(150, num_litter)));
	}

	// Rides
	{
		int i, num_rides;
		short _ax, _bx;
		rct_ride* ride;

		num_rides = 0;
		_ax = 0;
		_bx = 0;
		for (i = 0; i < 255; i++) {
			ride = &(RCT2_ADDRESS(RCT2_ADDRESS_RIDE_LIST, rct_ride)[i]);

			if (ride->type == RIDE_TYPE_NULL)
				continue;
			if (ride->var_140 == -1)
				continue;
			_ax += ride->var_140 / 8;
			_bx += ride->var_142 / 8;
			num_rides++;
		}

		result -= 100;
		if (num_rides > 0) {
			_bx = _ax / num_rides;
			_ax = _bx / num_rides;
			_bx -= 46;
			if (_bx >= 0)
				_bx = -_bx;
			_ax -= 65;
			if (_ax >= 0)
				_ax = -_ax;
			_bx /= 2;
			_ax /= 2;
			_bx = min(50, _bx);
			_ax = min(50, _ax);
			result += 100 - _ax - _bx;
		}
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
