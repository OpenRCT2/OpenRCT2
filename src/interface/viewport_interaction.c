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
#include "../localisation/localisation.h"
#include "../ride/ride.h"
#include "../ride/ride_data.h"
#include "../scenario.h"
#include "../world/banner.h"
#include "../world/map.h"
#include "../world/scenery.h"
#include "viewport.h"

static void viewport_interaction_remove_scenery(rct_map_element *mapElement, int x, int y);
static void viewport_interaction_remove_footpath(rct_map_element *mapElement, int x, int y);
static void viewport_interaction_remove_footpath_item(rct_map_element *mapElement, int x, int y);
static void viewport_interaction_remove_park_entrance(rct_map_element *mapElement, int x, int y);
static void viewport_interaction_remove_park_wall(rct_map_element *mapElement, int x, int y);
static void viewport_interaction_remove_large_scenery(rct_map_element *mapElement, int x, int y);

/**
 * 
 *  rct2: 0x006EDE88
 */
int viewport_interaction_get_item(int x, int y, rct_map_element **outMapElement, int *outX, int *outY)
{
	{
		int eax, ebx, ecx, edx, esi, edi, ebp;
		eax = x;
		ebx = y;
		RCT2_CALLFUNC_X(0x006EDE88, &eax, &ebx, &ecx, &edx, &esi, &edi, &ebp);

		*outMapElement = (rct_map_element*)edx;
		*outX = eax & 0xFFFF;
		*outY = ecx & 0xFFFF;
		return ebx & 0xFF;
	}

	rct_s6_info *s6Info = (rct_s6_info*)0x00141F570;
	rct_map_element *mapElement;
	rct_scenery_entry *sceneryEntry;
	rct_banner *banner;
	rct_ride *ride;
	int i, outZ;

	// No click input for title screen or track manager
	if (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & (SCREEN_FLAGS_TITLE_DEMO | SCREEN_FLAGS_TRACK_MANAGER))
		return 0;

	// 
	if ((RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_TRACK_DESIGNER) && s6Info->var_000 != 6)
		return 0;

	get_map_coordinates_from_pos(x, y, 9, outX, outY, &outZ, &mapElement);
	*outMapElement = mapElement;

	switch (outZ) {
	case VIEWPORT_INTERACTION_ITEM_2:
		if ((RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_SCENARIO_EDITOR) || mapElement->type != 0)
			return 0;

		mapElement += 6;
		ride = GET_RIDE(mapElement->type);
		if (ride->status == RIDE_STATUS_CLOSED) {
			RCT2_GLOBAL(RCT2_ADDRESS_MAP_TOOLTIP_ARGS + 0, uint16) = 1163;
			RCT2_GLOBAL(RCT2_ADDRESS_MAP_TOOLTIP_ARGS + 2, uint16) = ride->name;
			RCT2_GLOBAL(RCT2_ADDRESS_MAP_TOOLTIP_ARGS + 4, uint32) = ride->name_arguments;
		}
		return 2;

	case VIEWPORT_INTERACTION_ITEM_RIDE:
		if (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_SCENARIO_EDITOR)
			return 0;
		if ((mapElement->type & MAP_ELEMENT_TYPE_MASK) == MAP_ELEMENT_TYPE_PATH)
			return 0;

		ride = GET_RIDE(mapElement->properties.track.ride_index);
		if (ride->status != RIDE_STATUS_CLOSED)
			return 0;

		RCT2_GLOBAL(RCT2_ADDRESS_MAP_TOOLTIP_ARGS + 0, uint16) = 1163;

		if ((mapElement->type & MAP_ELEMENT_TYPE_MASK) == MAP_ELEMENT_TYPE_ENTRANCE) {
			RCT2_GLOBAL(RCT2_ADDRESS_MAP_TOOLTIP_ARGS + 2, uint16) =
				mapElement->properties.track.type == ENTRANCE_TYPE_RIDE_ENTRANCE ? 1335 : 1337;
		} else if (mapElement->properties.track.type == 1 || mapElement->properties.track.type == 2 || mapElement->properties.track.type == 3) {
			RCT2_GLOBAL(RCT2_ADDRESS_MAP_TOOLTIP_ARGS + 2, uint16) = 1333;
		} else {
			if (!sub_664F72(x, y, mapElement->base_height << 4))
				return 0;

			RCT2_GLOBAL(RCT2_ADDRESS_MAP_TOOLTIP_ARGS + 2, uint16) = ride->name;
			RCT2_GLOBAL(RCT2_ADDRESS_MAP_TOOLTIP_ARGS + 4, uint32) = ride->name_arguments;
			return 3;
		}

		if (ride->num_stations > 1)
			RCT2_GLOBAL(RCT2_ADDRESS_MAP_TOOLTIP_ARGS + 2, uint16)++;

		RCT2_GLOBAL(RCT2_ADDRESS_MAP_TOOLTIP_ARGS + 4, uint16) = ride->name;
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_TOOLTIP_ARGS + 6, uint32) = ride->name_arguments;
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_TOOLTIP_ARGS + 10, uint16) = RideNameConvention[ride->type].station_name + 2;

		int edi = (mapElement->properties.track.sequence & 0x70) >> 4;
		for (i = edi; i >= 0; i--)
			if (ride->station_starts[i] == 0xFFFF)
				edi--;
		edi++;
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_TOOLTIP_ARGS + 12, uint16) = edi;
		return 3;

	case VIEWPORT_INTERACTION_ITEM_WALL:
		sceneryEntry = g_wallSceneryEntries[mapElement->properties.scenery.type];
		if (sceneryEntry->wall.var_0D != 255) {
			RCT2_GLOBAL(RCT2_ADDRESS_MAP_TOOLTIP_ARGS + 0, uint16) = 1163;
			RCT2_GLOBAL(RCT2_ADDRESS_MAP_TOOLTIP_ARGS + 2, uint16) = sceneryEntry->name;
			return 9;
		}

	case VIEWPORT_INTERACTION_ITEM_LARGE_SCENERY:
		sceneryEntry = g_largeSceneryEntries[mapElement->properties.scenerymultiple.type & 0x3FF];
		if (sceneryEntry->large_scenery.var_11 != 255) {
			RCT2_GLOBAL(RCT2_ADDRESS_MAP_TOOLTIP_ARGS + 0, uint16) = 1163;
			RCT2_GLOBAL(RCT2_ADDRESS_MAP_TOOLTIP_ARGS + 2, uint16) = sceneryEntry->name;
			return 10;
		}

	case VIEWPORT_INTERACTION_ITEM_BANNER:
		banner = &gBanners[mapElement->properties.banner.index];
		sceneryEntry = g_bannerSceneryEntries[banner->type];

		RCT2_GLOBAL(RCT2_ADDRESS_MAP_TOOLTIP_ARGS + 0, uint16) = 1163;
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_TOOLTIP_ARGS + 2, uint16) = sceneryEntry->name;
		return 12;
	}

	if ((RCT2_GLOBAL(RCT2_ADDRESS_INPUT_FLAGS, uint32) & 0x48) != 0x48)
		if (window_find_by_class(WC_RIDE_CONSTRUCTION) == NULL && window_find_by_class(WC_FOOTPATH) == NULL)
			return 0;

	switch (outZ) {
	case VIEWPORT_INTERACTION_ITEM_SCENERY:
		sceneryEntry = g_smallSceneryEntries[mapElement->properties.scenery.type];
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_TOOLTIP_ARGS + 0, uint16) = 1164;
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_TOOLTIP_ARGS + 2, uint16) = sceneryEntry->name;
		return 5;

	case VIEWPORT_INTERACTION_ITEM_FOOTPATH:
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_TOOLTIP_ARGS + 0, uint16) = 1164;
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_TOOLTIP_ARGS + 2, uint16) = 1425;
		if (mapElement->type & 1)
			RCT2_GLOBAL(RCT2_ADDRESS_MAP_TOOLTIP_ARGS + 2, uint16) = 1426;
		return 6;

	case VIEWPORT_INTERACTION_ITEM_FOOTPATH_ITEM:
		sceneryEntry = g_pathBitSceneryEntries[mapElement->properties.scenery.age & 0x0F];
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_TOOLTIP_ARGS + 0, uint16) = 1164;
		if (mapElement->flags & 0x20) {
			RCT2_GLOBAL(RCT2_ADDRESS_MAP_TOOLTIP_ARGS + 2, uint16) = 3124;
			RCT2_GLOBAL(RCT2_ADDRESS_MAP_TOOLTIP_ARGS + 4, uint16) = sceneryEntry->name;
		} else {
			RCT2_GLOBAL(RCT2_ADDRESS_MAP_TOOLTIP_ARGS + 2, uint16) = sceneryEntry->name;
		}
		return 7;

	case VIEWPORT_INTERACTION_ITEM_PARK_ENTRANCE:
		if (RCT2_ADDRESS_SCREEN_FLAGS & SCREEN_FLAGS_SCENARIO_EDITOR)
			return 0;

		if ((mapElement->type & MAP_ELEMENT_TYPE_MASK) != MAP_ELEMENT_TYPE_ENTRANCE)
			return 0;

		RCT2_GLOBAL(RCT2_ADDRESS_MAP_TOOLTIP_ARGS + 0, uint16) = 1164;
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_TOOLTIP_ARGS + 2, uint16) = 3192;
		return 8;

	case VIEWPORT_INTERACTION_ITEM_WALL:
		sceneryEntry = g_wallSceneryEntries[mapElement->properties.scenery.type];
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_TOOLTIP_ARGS + 0, uint16) = 1164;
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_TOOLTIP_ARGS + 2, uint16) = sceneryEntry->name;
		return 9;

	case VIEWPORT_INTERACTION_ITEM_LARGE_SCENERY:
		sceneryEntry = g_largeSceneryEntries[mapElement->properties.scenery.type & 0x3FF];
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_TOOLTIP_ARGS + 0, uint16) = 1164;
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_TOOLTIP_ARGS + 2, uint16) = sceneryEntry->name;
		return 10;
	}

	return 0;
}

void viewport_interaction_hover(int x, int y)
{
	rct_map_element *mapElement;

	viewport_interaction_get_item(x, y, &mapElement, &x, &y);
}

/**
 * 
 *  rct2: 0x006E8A62
 */
void viewport_interaction_right_click(int x, int y)
{
	rct_map_element *mapElement;

	switch (viewport_interaction_get_item(x, y, &mapElement, &x, &y)) {
	case 2:
		if (mapElement->type == 0)
			RCT2_CALLPROC_X(0x006B4857, x, 0, y, (int)mapElement, 0, 0, 0);
		break;
	case VIEWPORT_INTERACTION_ITEM_RIDE:
		ride_modify(mapElement, x, y);
		break;
	case VIEWPORT_INTERACTION_ITEM_SCENERY:
		viewport_interaction_remove_scenery(mapElement, x, y);
		break;
	case VIEWPORT_INTERACTION_ITEM_FOOTPATH:
		viewport_interaction_remove_footpath(mapElement, x, y);
		break;
	case VIEWPORT_INTERACTION_ITEM_FOOTPATH_ITEM:
		viewport_interaction_remove_footpath_item(mapElement, x, y);
		break;
	case VIEWPORT_INTERACTION_ITEM_PARK_ENTRANCE:
		viewport_interaction_remove_park_entrance(mapElement, x, y);
		break;
	case VIEWPORT_INTERACTION_ITEM_WALL:
		viewport_interaction_remove_park_wall(mapElement, x, y);
		break;
	case VIEWPORT_INTERACTION_ITEM_LARGE_SCENERY:
		viewport_interaction_remove_large_scenery(mapElement, x, y);
		break;
	case VIEWPORT_INTERACTION_ITEM_BANNER:
		window_banner_open(mapElement->properties.banner.index);
		break;
	}
}

/**
 * 
 *  rct2: 0x006E08D2
 */
static void viewport_interaction_remove_scenery(rct_map_element *mapElement, int x, int y)
{
	RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TITLE, uint16) = STR_CANT_REMOVE_THIS;
	game_do_command(
		x,
		(mapElement->type << 8) | 1,
		y,
		(mapElement->properties.scenery.type << 8) | mapElement->base_height,
		GAME_COMMAND_REMOVE_SCENERY,
		0,
		0
	);
}

/**
 * 
 *  rct2: 0x006A614A
 */
static void viewport_interaction_remove_footpath(rct_map_element *mapElement, int x, int y)
{
	int z;
	rct_window *w;
	rct_map_element *mapElement2;

	z = mapElement->base_height;
	
	w = window_find_by_class(WC_FOOTPATH);
	if (w != NULL)
		RCT2_CALLPROC_EBPSAFE(0x006A7831);

	mapElement2 = TILE_MAP_ELEMENT_POINTER((y / 32) * 256 + (x / 32));
	do {
		if ((mapElement2->type & MAP_ELEMENT_TYPE_MASK) == MAP_ELEMENT_TYPE_PATH && mapElement2->base_height == z) {
			RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TITLE, uint16) = STR_CANT_REMOVE_FOOTPATH_FROM_HERE;
			game_do_command(x, 1, y, z, GAME_COMMAND_REMOVE_PATH, 0, 0);
		}
	} while (!((mapElement2++)->flags & MAP_ELEMENT_FLAG_LAST_TILE));
}

/**
 * 
 *  rct2: 0x006A61AB
 */
static void viewport_interaction_remove_footpath_item(rct_map_element *mapElement, int x, int y)
{
	int type;

	type = mapElement->properties.scenery.type >> 4;
	if (mapElement->type & 0x80)
		type |= 0x80;

	RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TITLE, uint16) = STR_CANT_REMOVE_THIS;
	game_do_command(
		x,
		((mapElement->properties.scenery.type & 7) << 8) | 1,
		y,
		(type << 8) | mapElement->base_height,
		GAME_COMMAND_PLACE_PATH,
		0,
		0
	);
}

/**
 * 
 *  rct2: 0x00666C0E
 */
static void viewport_interaction_remove_park_entrance(rct_map_element *mapElement, int x, int y)
{
	RCT2_CALLPROC_X(0x00666C0E, x, 0, y, (int)mapElement, 0, 0, 0);
}

/**
 * 
 *  rct2: 0x006E57A9
 */
static void viewport_interaction_remove_park_wall(rct_map_element *mapElement, int x, int y)
{
	rct_scenery_entry* sceneryEntry;

	sceneryEntry = g_wallSceneryEntries[mapElement->properties.fence.slope];
	if (sceneryEntry->wall.var_0D != 0xFF){
		window_sign_small_open(mapElement->properties.fence.item[0]);
	} else {
		RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_STRING_ID, rct_string_id) = 1158;
		game_do_command(
			x,
			1,
			y,
			(mapElement->type & 0x3) | (mapElement->base_height << 8),
			GAME_COMMAND_REMOVE_FENCE,
			0,
			0
		);
	}
}

/**
 * 
 *  rct2: 0x006B88DC
 */
static void viewport_interaction_remove_large_scenery(rct_map_element *mapElement, int x, int y)
{
	int ebx;
	rct_scenery_entry* sceneryEntry;

	ebx = mapElement->properties.scenerymultiple.type;
	ebx |= (mapElement->properties.scenerymultiple.index & 0x3) << 8;
	sceneryEntry = g_largeSceneryEntries[ebx];

	if (sceneryEntry->large_scenery.var_11 != 0xFF){
		int id = (mapElement->type & 0xC0) |
			((mapElement->properties.scenerymultiple.colour[0] & 0xE0) >> 2) |
			((mapElement->properties.scenerymultiple.colour[1] & 0xE0) >> 5);
		window_sign_open(id);
	} else {
		RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_STRING_ID, rct_string_id) = 1158;
		game_do_command(
			x, 
			1 | ((mapElement->type & 0x3) << 8), 
			y, 
			mapElement->base_height | ((mapElement->properties.scenerymultiple.index >> 2) << 8),
			GAME_COMMAND_44, 
			0, 
			0
		);
	}
}