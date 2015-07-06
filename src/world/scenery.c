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
#include "../game.h"
#include "../common.h"
#include "../localisation/localisation.h"
#include "../scenario.h"
#include "climate.h"
#include "fountain.h"
#include "map.h"
#include "park.h"
#include "scenery.h"

void scenery_increase_age(int x, int y, rct_map_element *mapElement);

void scenery_update_tile(int x, int y)
{
	rct_map_element *mapElement;

	mapElement = map_get_first_element_at(x >> 5, y >> 5);
	do {
		if (map_element_get_type(mapElement) == MAP_ELEMENT_TYPE_SCENERY) {
			scenery_update_age(x, y, mapElement);
		} else if (map_element_get_type(mapElement) == MAP_ELEMENT_TYPE_PATH) {
			int additions = mapElement->properties.path.additions & 0x0F;
			if (additions != 0 && !(mapElement->properties.path.additions & 0x80)) {
				rct_scenery_entry *sceneryEntry;
				sceneryEntry = g_pathBitSceneryEntries[additions - 1];
				if (sceneryEntry->path_bit.var_06 & PATH_BIT_FLAG_JUMPING_FOUNTAIN_WATER) {
					jumping_fountain_begin(JUMPING_FOUNTAIN_TYPE_WATER, x, y, mapElement);
				} else if (sceneryEntry->path_bit.var_06 & PATH_BIT_FLAG_JUMPING_FOUNTAIN_SNOW) {
					jumping_fountain_begin(JUMPING_FOUNTAIN_TYPE_SNOW, x, y, mapElement);
				}
			}
		}
	} while (!map_element_is_last_for_tile(mapElement++));
}

/**
 *
 *  rct2: 0x006E33D9
 */
void scenery_update_age(int x, int y, rct_map_element *mapElement)
{
	rct_map_element *mapElementAbove;
	rct_scenery_entry *sceneryEntry;

	sceneryEntry = g_smallSceneryEntries[mapElement->properties.scenery.type];
	if (
		!(sceneryEntry->small_scenery.flags & SMALL_SCENERY_FLAG_CAN_BE_WATERED) ||
		(RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_WEATHER, uint8) < WEATHER_RAIN) ||
		(mapElement->properties.scenery.age < 5)
	) {
		scenery_increase_age(x, y, mapElement);
		return;
	}

	// Check map elements above, presumebly to see if map element is blocked from rain
	mapElementAbove = mapElement;
	while (!(mapElementAbove->flags & 7)) {
		mapElementAbove++;

		switch (map_element_get_type(mapElementAbove)) {
		case MAP_ELEMENT_TYPE_SCENERY_MULTIPLE:
		case MAP_ELEMENT_TYPE_ENTRANCE:
		case MAP_ELEMENT_TYPE_PATH:
			map_invalidate_tile_zoom1(x, y, mapElementAbove->base_height * 8, mapElementAbove->clearance_height * 8);
			scenery_increase_age(x, y, mapElement);
			return;
		case MAP_ELEMENT_TYPE_SCENERY:
			sceneryEntry = g_smallSceneryEntries[mapElementAbove->properties.scenery.type];
			if (sceneryEntry->small_scenery.flags & SMALL_SCENERY_FLAG_VOFFSET_CENTRE) {
				scenery_increase_age(x, y, mapElement);
				return;
			}
			break;
		}
	}
	
	// Reset age / water plant
	mapElement->properties.scenery.age = 0;
	map_invalidate_tile_zoom1(x, y, mapElement->base_height * 8, mapElement->clearance_height * 8);
}

void scenery_increase_age(int x, int y, rct_map_element *mapElement)
{
	if (mapElement->flags & SMALL_SCENERY_FLAG_ANIMATED)
		return;

	if (mapElement->properties.scenery.age < 255) {
		mapElement->properties.scenery.age++;
		map_invalidate_tile_zoom1(x, y, mapElement->base_height * 8, mapElement->clearance_height * 8);
	}
}


/* 0x006E2712 */
void scenery_remove_ghost_tool_placement(){
	sint16 x, y, z;

	x = RCT2_GLOBAL(0x00F64EC4, sint16);
	y = RCT2_GLOBAL(0x00F64EC6, sint16);
	z = RCT2_GLOBAL(0x00F64F09, uint8);

	if (RCT2_GLOBAL(0x00F64F0D, uint8) & (1 << 0)){
		RCT2_GLOBAL(0x00F64F0D, uint8) &= ~(1 << 0);

		game_do_command(
			x, 
			105 | (RCT2_GLOBAL(0x00F64F0C, uint8) << 8), 
			y, 
			z | (RCT2_GLOBAL(0x00F64EDA, uint8) << 8), 
			GAME_COMMAND_REMOVE_SCENERY,
			0, 
			0);
	}

	if (RCT2_GLOBAL(0x00F64F0D, uint8) & (1 << 1)){
		RCT2_GLOBAL(0x00F64F0D, uint8) &= ~(1 << 1);

		rct_map_element* map_element = map_get_first_element_at(x / 32, y / 32);

		do{
			if (map_element_get_type(map_element) != MAP_ELEMENT_TYPE_PATH)
				continue;

			if (map_element->base_height != z)
				continue;

			game_do_command(
				x,
				233 | (RCT2_GLOBAL(0x00F64F0F, uint8) << 8),
				y,
				z | (RCT2_GLOBAL(0x00F64F10, uint8) << 8),
				GAME_COMMAND_PLACE_PATH,
				RCT2_GLOBAL(0x00F64EAC, uint32) & 0xFFFF0000,
				0);
			break;
		} while (!map_element_is_last_for_tile(map_element++));
	}

	if (RCT2_GLOBAL(0x00F64F0D, uint8) & (1 << 2)){
		RCT2_GLOBAL(0x00F64F0D, uint8) &= ~(1 << 2);

		game_do_command(
			x,
			105 | (RCT2_GLOBAL(0x00F64F0C, uint8) << 8),
			y,
			RCT2_GLOBAL(0x00F64F11, uint8) |(z << 8),
			GAME_COMMAND_REMOVE_FENCE,
			0,
			0);
	}

	if (RCT2_GLOBAL(0x00F64F0D, uint8) & (1 << 3)){
		RCT2_GLOBAL(0x00F64F0D, uint8) &= ~(1 << 3);

		game_do_command(
			x,
			105 | (RCT2_GLOBAL(0x00F64EC0, uint8) << 8),
			y,
			z,
			GAME_COMMAND_REMOVE_LARGE_SCENERY,
			0,
			0);
	}

	if (RCT2_GLOBAL(0x00F64F0D, uint8) & (1 << 4)){
		RCT2_GLOBAL(0x00F64F0D, uint8) &= ~(1 << 4);

		game_do_command(
			x,
			105,
			y,
			z | (RCT2_GLOBAL(0x00F64EC0, uint8) << 8),
			GAME_COMMAND_REMOVE_BANNER,
			0,
			0);
	}
}