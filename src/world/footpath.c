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
#include "../cheats.h"
#include "../config.h"
#include "../game.h"
#include "../localisation/localisation.h"
#include "../management/finance.h"
#include "../util/util.h"
#include "footpath.h"
#include "map.h"
#include "map_animation.h"
#include "scenery.h"

void footpath_interrupt_peeps(int x, int y, int z);
void sub_6A7642(int x, int y, rct_map_element *mapElement);
void sub_6A76E9(int rideIndex);

enum {
	FOOTPATH_CONSTRUCTION_FLAG_ALLOW_DURING_PAUSED = 1 << 3
};

const rct_xy16 word_981D6C[4] = {
	{ -1,  0 },
	{  0,  1 },
	{  1,  0 },
	{  0, -1 }
};

// rct2: 0x0097B974
static const uint16 EntranceDirections[] = {
	(4    ), 0, 0, 0, 0, 0, 0, 0,	// ENTRANCE_TYPE_RIDE_ENTRANCE,
	(4    ), 0, 0, 0, 0, 0, 0, 0,	// ENTRANCE_TYPE_RIDE_EXIT,
	(4 | 1), 0, 0, 0, 0, 0, 0, 0,	// ENTRANCE_TYPE_PARK_ENTRANCE
};

static int entrance_get_directions(rct_map_element *mapElement)
{
	uint8 entranceType = mapElement->properties.entrance.type;
	uint8 sequence = mapElement->properties.entrance.index & 0x0F;
	return EntranceDirections[(entranceType * 8) + sequence];
}

static bool entrance_has_direction(rct_map_element *mapElement, int direction)
{
	return entrance_get_directions(mapElement) & (1 << direction);
}

/**
 *
 *  rct2: 0x006A65AD
 */
static void automatically_set_peep_spawn(int x, int y, int z)
{
	rct2_peep_spawn *peepSpawn = (rct2_peep_spawn*)RCT2_ADDRESS_PEEP_SPAWNS;
	int direction = 0;
	if (x != 32) {
		direction++;
		if (y != RCT2_GLOBAL(RCT2_ADDRESS_MAP_SIZE_UNITS, uint16) - 32) {
			direction++;
			if (x != RCT2_GLOBAL(RCT2_ADDRESS_MAP_SIZE_UNITS, uint16) - 32) {
				direction++;
				if (y != 32)
					return;
			}
		}
	}

	peepSpawn->x = x + (word_981D6C[direction].x * 15) + 16;
	peepSpawn->y = y + (word_981D6C[direction].y * 15) + 16;
	peepSpawn->direction = direction;
	peepSpawn->z = z;
}

rct_map_element *map_get_footpath_element(int x, int y, int z)
{
	rct_map_element *mapElement;

	mapElement = map_get_first_element_at(x, y);
	do {
		if (map_element_get_type(mapElement) == MAP_ELEMENT_TYPE_PATH && mapElement->base_height == z)
			return mapElement;
	} while (!map_element_is_last_for_tile(mapElement++));

	return NULL;
}

rct_map_element *map_get_footpath_element_slope(int x, int y, int z, int slope)
{
	rct_map_element *mapElement;

	mapElement = map_get_first_element_at(x, y);
	do {
		if (
			map_element_get_type(mapElement) == MAP_ELEMENT_TYPE_PATH &&
			mapElement->base_height == z &&
			(mapElement->properties.path.type & 7) == slope
		) {
			return mapElement;
		}
	} while (!map_element_is_last_for_tile(mapElement++));
	
	return NULL;
}

static void loc_6A6620(int flags, int x, int y, rct_map_element *mapElement)
{
	int direction, z;

	if ((mapElement->properties.path.type & 4) && !(flags & (1 << 6))) {
		direction = mapElement->properties.path.type & 3;
		z = mapElement->base_height;
		map_remove_intersecting_walls(x, y, z, z + 6, direction ^ 2);
		map_remove_intersecting_walls(x, y, z, z + 6, direction);
		mapElement = map_get_footpath_element(x / 32, y / 32, z);
	}

	if (!(flags & (1 << 7)))
		footpath_connect_edges(x, y, mapElement, flags);

	sub_6A759F();
	map_invalidate_tile_full(x, y);
}

static money32 footpath_element_insert(int type, int x, int y, int z, int slope, int flags)
{
	rct_map_element *mapElement;
	int bl, zHigh;

	if (!sub_68B044())
		return MONEY32_UNDEFINED;

	if ((flags & GAME_COMMAND_FLAG_APPLY) && !(flags & (FOOTPATH_CONSTRUCTION_FLAG_ALLOW_DURING_PAUSED | (1 << 6))))
		footpath_remove_litter(x, y, RCT2_GLOBAL(0x009DEA62, uint16));

	// loc_6A649D:
	RCT2_GLOBAL(0x00F3EFD9, money32) += MONEY(12, 00);

	bl = 15;
	zHigh = z + 4;
	if (slope & 4) {
		bl = RCT2_ADDRESS(0x0098D7EC, uint8)[slope & 3];
		zHigh += 2;
	}

	RCT2_GLOBAL(0x00F3EF84, uint16) = x;
	RCT2_GLOBAL(0x00F3EF86, uint16) = y;
		
	// Ugh, hack until 0x006A6733 is written
	// 0x006A6733 expects the flags to be at (*0xF3EF7C) + 8
	RCT2_GLOBAL(0x00F3EF7C, uint32) = (uint32)(&flags - 2);

	if (!gCheatsDisableClearanceChecks && !map_can_construct_with_clear_at(x, y, z, zHigh, (void*)0x006A6733, bl))
		return MONEY32_UNDEFINED;

	RCT2_GLOBAL(0x00F3EFA4, uint8) = RCT2_GLOBAL(0x00F1AD60, uint8);
	if (!gCheatsDisableClearanceChecks && (RCT2_GLOBAL(0x00F1AD60, uint8) & 4)) {
		RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TEXT, rct_string_id) = STR_CANT_BUILD_THIS_UNDERWATER;
		return MONEY32_UNDEFINED;
	}

	mapElement = map_get_surface_element_at((x / 32), (y / 32));

	int supportHeight = z - mapElement->base_height;
	RCT2_GLOBAL(0x00F3EFD9, money32) += supportHeight < 0 ? MONEY(20, 00) : (supportHeight / 2) * MONEY(5, 00);

	if (flags & GAME_COMMAND_FLAG_APPLY) {
		mapElement = map_element_insert(x / 32, y / 32, z, 0x0F);
		mapElement->type = MAP_ELEMENT_TYPE_PATH;
		mapElement->clearance_height = z + 4 + (slope & 4 ? 2 : 0);
		mapElement->properties.path.type = (type << 4) | (slope & 7);
		mapElement->type |= type >> 7;
		mapElement->properties.path.additions = RCT2_GLOBAL(0x00F3EF88, uint8);
		mapElement->properties.path.addition_status = 255;
		mapElement->flags &= ~MAP_ELEMENT_FLAG_BROKEN;
		if (flags & (1 << 6))
			mapElement->flags |= MAP_ELEMENT_FLAG_GHOST;

		RCT2_GLOBAL(0x00F3EFF4, uint32) = 0x00F3EFF8;

		if (!(flags & (1 << 7)))
			footpath_remove_edges_at(x, y, mapElement);

		if ((RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_SCENARIO_EDITOR) && !(flags & (1 << 6)))
			automatically_set_peep_spawn(x, y, mapElement->base_height / 2);

		loc_6A6620(flags, x, y, mapElement);
	}
	return RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & PARK_FLAGS_NO_MONEY ? 0 : RCT2_GLOBAL(0x00F3EFD9, money32);
}

static money32 footpath_element_update(int x, int y, rct_map_element *mapElement, int type, int flags)
{
	if ((mapElement->properties.path.type >> 4) != (type & 0x0F) || (mapElement->type & 1) != (type >> 7)) {
		RCT2_GLOBAL(0x00F3EFD9, money32) += MONEY(6, 00);
	} else if (RCT2_GLOBAL(0x00F3EF88, uint16) != 0) {
		if (
			!(flags & (1 << 6)) &&
			(mapElement->properties.path.additions & 0x0F) == RCT2_GLOBAL(0x00F3EF88, uint16) &&
			!(mapElement->flags & MAP_ELEMENT_FLAG_BROKEN)
		) {
			if (flags & (1 << 4))
				return MONEY32_UNDEFINED;

			return RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & PARK_FLAGS_NO_MONEY ? 0 : RCT2_GLOBAL(0x00F3EFD9, money32);
		}

		if (RCT2_GLOBAL(0x00F3EF88, uint16) != 0) {
			rct_scenery_entry* scenery_entry = g_pathBitSceneryEntries[RCT2_GLOBAL(0x00F3EF88, uint16) - 1];
			uint16 unk6 = scenery_entry->path_bit.var_06;

			if ((unk6 & 0x80) && (mapElement->properties.path.type & 4)) {
				RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TEXT, rct_string_id) = STR_CANT_BUILD_THIS_ON_SLOPED_FOOTPATH;
				return MONEY32_UNDEFINED;
			}

			if ((unk6 & 0x40) && footpath_element_is_queue(mapElement)) {
				RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TEXT, rct_string_id) = STR_CANNOT_PLACE_THESE_ON_QUEUE_LINE_AREA;
				return MONEY32_UNDEFINED;
			}

			if ((unk6 & 0x30) && (mapElement->properties.path.edges & 0x0F) == 0x0F) {
				RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TEXT, rct_string_id) = STR_NONE;
				return MONEY32_UNDEFINED;
			}

			if ((unk6 & 0x100) && !footpath_element_is_queue(mapElement)) {
				RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TEXT, rct_string_id) = STR_CAN_ONLY_PLACE_THESE_ON_QUEUE_AREA;
				return MONEY32_UNDEFINED;
			}

			RCT2_GLOBAL(0x00F3EFD9, money32) += scenery_entry->path_bit.price;
		}

		if (flags & (1 << 4))
			return MONEY32_UNDEFINED;

		if (flags & (1 << 6)) {
			if (mapElement->properties.path.additions & 0x0F) {
				RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TEXT, rct_string_id) = STR_NONE;
				return MONEY32_UNDEFINED;
			}

			if (flags & GAME_COMMAND_FLAG_APPLY)
				mapElement->properties.path.additions |= 0x80;
		}

		if (!(flags & GAME_COMMAND_FLAG_APPLY))
			return RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & PARK_FLAGS_NO_MONEY ? 0 : RCT2_GLOBAL(0x00F3EFD9, money32);

		if (
			(
				RCT2_GLOBAL(0x00F3EF88, uint16) != 0 &&
				!(flags & (1 << 6))
			) ||
			(
				RCT2_GLOBAL(0x00F3EF88, uint16) == 0 ||
				(mapElement->properties.path.additions & 0x80)
			)
		) {
			mapElement->properties.path.additions &= ~0x80;
		}

		mapElement->properties.path.additions = (mapElement->properties.path.additions & 0xF0) | RCT2_GLOBAL(0x00F3EF88, uint8);
		mapElement->flags &= ~0x20;
		if (RCT2_GLOBAL(0x00F3EF88, uint16) != 0) {
			rct_scenery_entry* scenery_entry = g_pathBitSceneryEntries[RCT2_GLOBAL(0x00F3EF88, uint16) - 1];
			uint16 unk6 = scenery_entry->path_bit.var_06;
			if (unk6 & 1)
				mapElement->properties.path.addition_status = 255;
		}
		map_invalidate_tile_full(x, y);
		return RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & PARK_FLAGS_NO_MONEY ? 0 : RCT2_GLOBAL(0x00F3EFD9, money32);
	}

	if (flags & (1 << 4))
		return MONEY32_UNDEFINED;

	if (flags & GAME_COMMAND_FLAG_APPLY) {
		RCT2_GLOBAL(0x00F3EFF4, uint32) = 0x00F3EFF8;

		if (!(flags & (1 << 7)))
			footpath_remove_edges_at(x, y, mapElement);

		mapElement->properties.path.type = (mapElement->properties.path.type & 0x0F) | (type << 4);
		mapElement->type = (mapElement->type & 0xFE) | (type >> 7);
		mapElement->properties.path.additions = (mapElement->properties.path.additions & 0xF0) | RCT2_GLOBAL(0x00F3EF88, uint8);
		mapElement->flags &= ~MAP_ELEMENT_FLAG_BROKEN;

		loc_6A6620(flags, x, y, mapElement);
	}

	return RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & PARK_FLAGS_NO_MONEY ? 0 : RCT2_GLOBAL(0x00F3EFD9, money32);
}

static money32 footpath_place_real(int type, int x, int y, int z, int slope, int flags, uint8 path_bit_type)
{
	rct_map_element *mapElement;

	RCT2_GLOBAL(RCT2_ADDRESS_NEXT_EXPENDITURE_TYPE, uint8) = RCT_EXPENDITURE_TYPE_LANDSCAPING * 4;
	RCT2_GLOBAL(0x009DEA5E, uint16) = x + 16;
	RCT2_GLOBAL(0x009DEA60, uint16) = y + 16;
	RCT2_GLOBAL(0x009DEA62, uint16) = z * 8;

	if (!(flags & FOOTPATH_CONSTRUCTION_FLAG_ALLOW_DURING_PAUSED) && RCT2_GLOBAL(RCT2_ADDRESS_GAME_PAUSED, uint8) != 0 && !gConfigCheat.build_in_pause_mode) {
		RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TEXT, rct_string_id) = STR_CONSTRUCTION_NOT_POSSIBLE_WHILE_GAME_IS_PAUSED;
		return MONEY32_UNDEFINED;
	}

	if (flags & GAME_COMMAND_FLAG_APPLY)
		footpath_interrupt_peeps(x, y, z * 8);

	RCT2_GLOBAL(0x00F3EFD9, money32) = 0;
	RCT2_GLOBAL(0x00F3EFA4, uint8) = 0;
	RCT2_GLOBAL(0x00F3EF88, uint16) = path_bit_type; // di

	if (x >= RCT2_GLOBAL(RCT2_ADDRESS_MAP_SIZE_UNITS, uint16) || y >= RCT2_GLOBAL(RCT2_ADDRESS_MAP_SIZE_UNITS, uint16)) {
		RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TEXT, rct_string_id) = STR_OFF_EDGE_OF_MAP;
		return MONEY32_UNDEFINED;
	}

	if (!((RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_SCENARIO_EDITOR) || gCheatsSandboxMode) && !map_is_location_owned(x, y, z * 8))
		return MONEY32_UNDEFINED;

	if (slope & 8) {
		RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TEXT, rct_string_id) = STR_LAND_SLOPE_UNSUITABLE;
		return MONEY32_UNDEFINED;
	}

	if (z < 2) {
		RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TEXT, rct_string_id) = STR_TOO_LOW;
		return MONEY32_UNDEFINED;
	}

	if (z > 248) {
		RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TEXT, rct_string_id) = STR_TOO_HIGH;
		return MONEY32_UNDEFINED;
	}

	mapElement = map_get_footpath_element_slope((x / 32), (y / 32), z, slope);
	return mapElement == NULL ?
		footpath_element_insert(type, x, y, z, slope, flags) :
		footpath_element_update(x, y, mapElement, type, flags);
}

/* rct2: 0x006BA23E */
void remove_banners_at_element(int x, int y, rct_map_element* mapElement){
	while (!map_element_is_last_for_tile(mapElement++)){
		if (map_element_get_type(mapElement) == MAP_ELEMENT_TYPE_PATH)return;
		else if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_BANNER)continue;

		game_do_command(x, 1, y, mapElement->base_height | mapElement->properties.banner.position << 8, GAME_COMMAND_REMOVE_BANNER, 0, 0);
		mapElement--;
	}
}

money32 footpath_remove_real(int x, int y, int z, int flags)
{
	rct_map_element *mapElement;

	RCT2_GLOBAL(RCT2_ADDRESS_NEXT_EXPENDITURE_TYPE, uint8) = RCT_EXPENDITURE_TYPE_LANDSCAPING * 4;
	RCT2_GLOBAL(0x009DEA5E, uint16) = x + 16;
	RCT2_GLOBAL(0x009DEA60, uint16) = y + 16;
	RCT2_GLOBAL(0x009DEA62, uint16) = z * 8;

	if (!(flags & FOOTPATH_CONSTRUCTION_FLAG_ALLOW_DURING_PAUSED) && RCT2_GLOBAL(RCT2_ADDRESS_GAME_PAUSED, uint8) != 0 && !gConfigCheat.build_in_pause_mode) {
		RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TEXT, rct_string_id) = STR_CONSTRUCTION_NOT_POSSIBLE_WHILE_GAME_IS_PAUSED;
		return MONEY32_UNDEFINED;
	}

	if (flags & GAME_COMMAND_FLAG_APPLY) {
		footpath_interrupt_peeps(x, y, z * 8);
		footpath_remove_litter(x, y, z * 8);
	}

	if (!((RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_SCENARIO_EDITOR) || gCheatsSandboxMode) && !map_is_location_owned(x, y, z * 8))
		return MONEY32_UNDEFINED;

	mapElement = map_get_footpath_element(x / 32, y / 32, z);
	if (mapElement != NULL && (flags & GAME_COMMAND_FLAG_APPLY)) {
		RCT2_GLOBAL(0x00F3EFF4, uint32) = 0x00F3EFF8;
		remove_banners_at_element(x, y, mapElement);
		footpath_remove_edges_at(x, y, mapElement);
		map_invalidate_tile_full(x, y);
		map_element_remove(mapElement);
		sub_6A759F();
	}

	return (flags & (1 << 5)) || (RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & PARK_FLAGS_NO_MONEY) ? 0 : -MONEY(10,00);
}

/**
 *
 *  rct2: 0x006A61DE
 */
void game_command_place_footpath(int *eax, int *ebx, int *ecx, int *edx, int *esi, int *edi, int *ebp)
{
	*ebx = footpath_place_real(
		(*edx >> 8) & 0xFF,
		*eax & 0xFFFF,
		*ecx & 0xFFFF,
		*edx & 0xFF,
		(*ebx >> 8) & 0xFF,
		*ebx & 0xFF,
		*edi & 0xFF
	);
}

/**
 *
 *  rct2: 0x006A67C0
 */
void game_command_remove_footpath(int *eax, int *ebx, int *ecx, int *edx, int *esi, int *edi, int *ebp)
{
	*ebx = footpath_remove_real((*eax & 0xFFFF), (*ecx & 0xFFFF), (*edx & 0xFF), (*ebx & 0xFF));
}

money32 footpath_place(int type, int x, int y, int z, int slope, int flags)
{
	return game_do_command(x, (slope << 8) | flags, y, (type << 8) | z, GAME_COMMAND_PLACE_PATH, 0, 0);
}

void footpath_remove(int x, int y, int z, int flags)
{
	game_do_command(x, flags, y, z, GAME_COMMAND_REMOVE_PATH, 0, 0);
}

/**
 * 
 *  rct2: 0x006A76FF
 */
money32 footpath_provisional_set(int type, int x, int y, int z, int slope)
{
	money32 cost;

	footpath_provisional_remove();

	cost = footpath_place(type, x, y, z, slope, (1 << 6) | (1 << 5) | (1 << 4) | FOOTPATH_CONSTRUCTION_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_APPLY);
	if (cost != MONEY32_UNDEFINED) {
		RCT2_GLOBAL(RCT2_ADDRESS_PROVISIONAL_PATH_X, uint16) = x;
		RCT2_GLOBAL(RCT2_ADDRESS_PROVISIONAL_PATH_Y, uint16) = y;
		RCT2_GLOBAL(RCT2_ADDRESS_PROVISIONAL_PATH_Z, uint8) = z & 0xFF;
		RCT2_GLOBAL(RCT2_ADDRESS_PROVISIONAL_PATH_FLAGS, uint8) |= (1 << 1);

		viewport_set_visibility(RCT2_GLOBAL(0x00F3EFA4, uint8) & 2 ? 1 : 3);
	}

	return cost;
}

/**
 * 
 *  rct2: 0x006A77FF
 */
void footpath_provisional_remove()
{
	if (RCT2_GLOBAL(RCT2_ADDRESS_PROVISIONAL_PATH_FLAGS, uint8) & (1 << 1)) {
		RCT2_GLOBAL(RCT2_ADDRESS_PROVISIONAL_PATH_FLAGS, uint8) &= ~(1 << 1);

		footpath_remove(
			RCT2_GLOBAL(RCT2_ADDRESS_PROVISIONAL_PATH_X, uint16),
			RCT2_GLOBAL(RCT2_ADDRESS_PROVISIONAL_PATH_Y, uint16),
			RCT2_GLOBAL(RCT2_ADDRESS_PROVISIONAL_PATH_Z, uint16),
			(1 << 0) | FOOTPATH_CONSTRUCTION_FLAG_ALLOW_DURING_PAUSED | (1 << 5)
		);
	}
}

/**
 * 
 *  rct2: 0x006A7831
 */
void footpath_provisional_update()
{
	if (RCT2_GLOBAL(RCT2_ADDRESS_PROVISIONAL_PATH_FLAGS, uint8) & PROVISIONAL_PATH_FLAG_SHOW_ARROW) {
		RCT2_GLOBAL(RCT2_ADDRESS_PROVISIONAL_PATH_FLAGS, uint8) &= ~PROVISIONAL_PATH_FLAG_SHOW_ARROW;

		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint8) &= ~(1 << 2);
		map_invalidate_tile_full(
			RCT2_GLOBAL(RCT2_ADDRESS_CONSTRUCT_PATH_FROM_X, uint16),
			RCT2_GLOBAL(RCT2_ADDRESS_CONSTRUCT_PATH_FROM_Y, uint16)
		);
	}
	footpath_provisional_remove();
}

/**
 *  Determines the location of the footpath at which we point with the cursor. If no footpath is underneath the cursor,
 *  then return the location of the ground tile. Besides the location it also computes the direction of the yellow arrow
 *  when we are going to build a footpath bridge/tunnel.
 *  rct2: 0x00689726
 *  In:
 *		screenX: eax
 *		screenY: ebx
 *  Out:
 *		x: ax
 *		y: bx
 *		direction: ecx
 *		mapElement: edx
 */
void footpath_get_coordinates_from_pos(int screenX, int screenY, int *x, int *y, int *direction, rct_map_element **mapElement)
{
	int z, interactionType;
	rct_map_element *myMapElement;
	rct_viewport *viewport;
	rct_xy16 map_pos = { 0 };

	get_map_coordinates_from_pos(screenX, screenY, VIEWPORT_INTERACTION_MASK_FOOTPATH, &map_pos.x, &map_pos.y, &interactionType, &myMapElement, &viewport);
	if (interactionType != VIEWPORT_INTERACTION_ITEM_FOOTPATH || !(viewport->flags & (VIEWPORT_FLAG_UNDERGROUND_INSIDE | VIEWPORT_FLAG_HIDE_BASE | VIEWPORT_FLAG_HIDE_VERTICAL))) {
		get_map_coordinates_from_pos(screenX, screenY, VIEWPORT_INTERACTION_MASK_FOOTPATH & VIEWPORT_INTERACTION_MASK_TERRAIN, &map_pos.x, &map_pos.y, &interactionType, &myMapElement, &viewport);
		if (interactionType == VIEWPORT_INTERACTION_ITEM_NONE) {
			if (x != NULL) *x = (sint16)0x8000;
			return;
		}
	}

	RCT2_GLOBAL(0x00F1AD3E, uint8) = interactionType;
	RCT2_GLOBAL(0x00F1AD30, rct_map_element*) = myMapElement;

	if (interactionType == VIEWPORT_INTERACTION_ITEM_FOOTPATH) {
		z = myMapElement->base_height * 8;
		if (myMapElement->properties.path.type & (1 << 2))
			z += 8;
	}

	RCT2_GLOBAL(0x00F1AD3C, uint16) = z;
	RCT2_GLOBAL(0x00F1AD34, sint16) = map_pos.x;
	RCT2_GLOBAL(0x00F1AD36, sint16) = map_pos.y;
	RCT2_GLOBAL(0x00F1AD38, sint16) = map_pos.x + 31;
	RCT2_GLOBAL(0x00F1AD3A, sint16) = map_pos.y + 31;

	map_pos.x += 16;
	map_pos.y += 16;

	rct_xy16 start_vp_pos = screen_coord_to_viewport_coord(viewport, screenX, screenY);

	for (int i = 0; i < 5; i++) {
		if (RCT2_GLOBAL(0x00F1AD3E, uint8) != 6) {
			z = map_element_height(map_pos.x, map_pos.y);
		} else {
			z = RCT2_GLOBAL(0x00F1AD3C, uint16);
		}
		map_pos = viewport_coord_to_map_coord(start_vp_pos.x, start_vp_pos.y, z);
		map_pos.x = clamp(RCT2_GLOBAL(0x00F1AD34, sint16), map_pos.x, RCT2_GLOBAL(0x00F1AD38, sint16));
		map_pos.y = clamp(RCT2_GLOBAL(0x00F1AD36, sint16), map_pos.y, RCT2_GLOBAL(0x00F1AD3A, sint16));
	}

	// Determine to which edge the cursor is closest
	uint32 myDirection;
	int mod_x = map_pos.x & 0x1F, mod_y = map_pos.y & 0x1F;
	if (mod_x < mod_y) {
		if (mod_x + mod_y < 32) {
			myDirection = 0;
		} else {
			myDirection = 1;
		}
	} else {
		if (mod_x + mod_y < 32) {
			myDirection = 3;
		} else {
			myDirection = 2;
		}
	}

	if (x != NULL) *x = map_pos.x & ~0x1F;
	if (y != NULL) *y = map_pos.y & ~0x1F;
	if (direction != NULL) *direction = myDirection;
	if (mapElement != NULL) *mapElement = myMapElement;
	// We should get the rct_map_element from 0x00F1AD30 here, but we set it earlier to our myMapElement anyway.
}

/**
 * 
 *  rct2: 0x0068A0C9
 * screenX: eax
 * screenY: ebx
 * x: ax
 * y: bx
 * direction: cl
 * mapElement: edx
 */
void footpath_bridge_get_info_from_pos(int screenX, int screenY, int *x, int *y, int *direction, rct_map_element **mapElement)
{
	// First check if we point at an entrance or exit. In that case, we would want the path coming from the entrance/exit.
	int interactionType;
	rct_viewport *viewport;

	rct_xy16 map_pos = { 0 };
	get_map_coordinates_from_pos(screenX, screenY, VIEWPORT_INTERACTION_MASK_RIDE, &map_pos.x, &map_pos.y, &interactionType, mapElement, &viewport);
	*x = map_pos.x;
	*y = map_pos.y;

	if (interactionType == VIEWPORT_INTERACTION_ITEM_RIDE
		&& viewport->flags & (VIEWPORT_FLAG_UNDERGROUND_INSIDE | VIEWPORT_FLAG_HIDE_BASE | VIEWPORT_FLAG_HIDE_VERTICAL)
		&& map_element_get_type(*mapElement) == MAP_ELEMENT_TYPE_ENTRANCE
	) {
		int directions = entrance_get_directions(*mapElement);
		if (directions & 0x0F) {
			int bx = bitscanforward(directions);
			bx += (*mapElement)->type;
			bx &= 3;
			if (direction != NULL) *direction = bx;
			return;
		}
	}
	
	get_map_coordinates_from_pos(screenX, screenY, VIEWPORT_INTERACTION_MASK_RIDE & VIEWPORT_INTERACTION_MASK_FOOTPATH & VIEWPORT_INTERACTION_MASK_TERRAIN, &map_pos.x, &map_pos.y, &interactionType, mapElement, &viewport);
	*x = map_pos.x;
	*y = map_pos.y;
	if (interactionType == VIEWPORT_INTERACTION_ITEM_RIDE && map_element_get_type(*mapElement) == MAP_ELEMENT_TYPE_ENTRANCE) {
		int directions = entrance_get_directions(*mapElement);
		if (directions & 0x0F) {
			int bx = bitscanforward(directions);
			bx += (*mapElement)->type; // First two bits seem to contain the direction of entrance/exit
			bx &= 3;
			if (direction != NULL) *direction = bx;
			return;
		}
	}

	// We point at something else
	footpath_get_coordinates_from_pos(screenX, screenY, x, y, direction, mapElement);
}

/**
 * 
 *  rct2: 0x00673883
 */
void footpath_remove_litter(int x, int y, int z)
{
	int index;
	uint16 spriteIndex, nextSpriteIndex;
	rct_litter *sprite;

	index = (x & 0x1FE0) << 3 | (y >> 5);
	spriteIndex = RCT2_ADDRESS(0x00F1EF60, uint16)[index];
	while (spriteIndex != SPRITE_INDEX_NULL) {
		sprite = &g_sprite_list[spriteIndex].litter;
		nextSpriteIndex = sprite->next_in_quadrant;
		if (sprite->linked_list_type_offset == SPRITE_LINKEDLIST_OFFSET_LITTER) {
			int distanceZ = abs(sprite->z - z);
			if (distanceZ <= 32) {
				sub_6EC60B((rct_sprite*)sprite);
				sprite_remove((rct_sprite*)sprite);
			}
		}
		spriteIndex = nextSpriteIndex;
	}
}

/**
 * 
 *  rct2: 0x0069A48B
 */
void footpath_interrupt_peeps(int x, int y, int z)
{
	int index;
	uint16 spriteIndex, nextSpriteIndex;
	rct_peep *peep;

	index = (x & 0x1FE0) << 3 | (y >> 5);
	spriteIndex = RCT2_ADDRESS(0x00F1EF60, uint16)[index];
	while (spriteIndex != SPRITE_INDEX_NULL) {
		peep = &g_sprite_list[spriteIndex].peep;
		nextSpriteIndex = peep->next_in_quadrant;
		if (peep->linked_list_type_offset == SPRITE_LINKEDLIST_OFFSET_PEEP) {
			if (peep->state == PEEP_STATE_SITTING || peep->state == PEEP_STATE_WATCHING) {
				if (peep->z == 0) {
					peep_decrement_num_riders(peep);
					peep->state = PEEP_STATE_WALKING;
					peep_window_state_update(peep);
					peep->destination_x = (peep->x & 0xFFE0) + 16;
					peep->destination_y = (peep->y & 0xFFE0) + 16;
					peep->destination_tolerence = 5;
					sub_693B58(peep);
				}
			}
		}
		spriteIndex = nextSpriteIndex;
	}
}

bool sub_6E59DC(int x, int y, int z0, int z1, int direction)
{
	rct_map_element *mapElement;

	mapElement = map_get_first_element_at(x >> 5, y >> 5);
	do {
		if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_FENCE)
			continue;
		if (mapElement->flags & MAP_ELEMENT_FLAG_GHOST)
			continue;
		if (z0 >= mapElement->clearance_height)
			continue;
		if (z1 <= mapElement->base_height)
			continue;
		if ((mapElement->type & MAP_ELEMENT_DIRECTION_MASK) != direction)
			continue;

		return true;
	} while (!map_element_is_last_for_tile(mapElement++));
	return false;
}

bool map_is_edge(int x, int y)
{
	return (
		x < 32 ||
		y < 32 ||
		x >= RCT2_GLOBAL(RCT2_ADDRESS_MAP_SIZE_UNITS, uint16) ||
		y >= RCT2_GLOBAL(RCT2_ADDRESS_MAP_SIZE_UNITS, uint16)
	);
}

static rct_map_element *footpath_connect_corners_get_neighbour(int x, int y, int z, int requireEdges)
{
	rct_map_element *mapElement = map_get_first_element_at(x >> 5, y >> 5);
	do {
		if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_PATH)
			continue;
		if (footpath_element_is_queue(mapElement))
			continue;
		if (mapElement->base_height != z)
			continue;
		if (!(mapElement->properties.path.edges & requireEdges))
			continue;

		return mapElement;
	} while (!map_element_is_last_for_tile(mapElement++));
	return NULL;
}

/**
 * Sets the corner edges of four path tiles.
 * The function will search for a path in the direction given, then check clockwise to see if it there is a path and again until
 * it reaches the initial path. In other words, checks if there are four paths together so that it can set the inner corners of
 * each one.
 *
 *  rct2: 0x006A70EB
 */
static void footpath_connect_corners(int initialX, int initialY, rct_map_element *initialMapElement)
{
	rct_map_element *mapElement[4];
	
	if (footpath_element_is_queue(initialMapElement))
		return;
	if (footpath_element_is_sloped(initialMapElement))
		return;
	
	mapElement[0] = initialMapElement;
	int z = initialMapElement->base_height;
	for (int initialDirection = 0; initialDirection < 4; initialDirection++) {		
		int x = initialX;
		int y = initialY;
		int direction = initialDirection;

		x += TileDirectionDelta[direction].x;
		y += TileDirectionDelta[direction].y;
		mapElement[1] = footpath_connect_corners_get_neighbour(x, y, z, (1 << (direction ^ 2)));
		if (mapElement[1] == NULL)
			continue;

		direction = (direction + 1) & 3;
		x += TileDirectionDelta[direction].x;
		y += TileDirectionDelta[direction].y;
		mapElement[2] = footpath_connect_corners_get_neighbour(x, y, z, (1 << (direction ^ 2)));
		if (mapElement[2] == NULL)
			continue;

		direction = (direction + 1) & 3;
		x += TileDirectionDelta[direction].x;
		y += TileDirectionDelta[direction].y;
		mapElement[3] = footpath_connect_corners_get_neighbour(x, y, z, (1 << (direction ^ 2)) | (1 << (((direction ^ 2) - 1) & 3)));
		if (mapElement[3] == NULL)
			continue;

		direction = (direction + 1) & 3;
		mapElement[3]->properties.path.edges |= (1 << (direction + 4));
		map_invalidate_element(x, y, mapElement[3]);

		direction = (direction - 1) & 3;
		mapElement[2]->properties.path.edges |= (1 << (direction + 4));
		map_invalidate_element(x, y, mapElement[2]);

		direction = (direction - 1) & 3;
		mapElement[1]->properties.path.edges |= (1 << (direction + 4));
		map_invalidate_element(x, y, mapElement[1]);

		direction = (direction - 1) & 3;
		mapElement[0]->properties.path.edges |= (1 << (direction + 4));
		map_invalidate_element(x, y, mapElement[0]);
	}
}

typedef struct {
	uint8 order;
	uint8 direction;
} rct_neighbour;

typedef struct {
	rct_neighbour items[8];
	int count;
} rct_neighbour_list;

static int rct_neighbour_compare(const void *a, const void *b)
{
	uint8 va = ((rct_neighbour*)a)->order;
	uint8 vb = ((rct_neighbour*)b)->order;
	if (va < vb) return 1;
	else if (va > vb) return -1;
	else {
		uint8 da = ((rct_neighbour*)a)->direction;
		uint8 db = ((rct_neighbour*)b)->direction;
		if (va < vb) return -1;
		else if (va > vb) return 1;
		else return 0;
	}
}

static void neighbour_list_init(rct_neighbour_list *neighbourList)
{
	neighbourList->count = 0;
}

static void neighbour_list_push(rct_neighbour_list *neighbourList, int order, int direction)
{
	neighbourList->items[neighbourList->count].order = order;
	neighbourList->items[neighbourList->count].direction = direction;
	neighbourList->count++;
}

static bool neighbour_list_pop(rct_neighbour_list *neighbourList, rct_neighbour *outNeighbour)
{
	if (neighbourList->count == 0)
		return false;

	*outNeighbour = neighbourList->items[0];
	for (int i = 0; i < neighbourList->count - 1; i++)
		neighbourList->items[i] = neighbourList->items[i + 1];
	neighbourList->count--;
	return true;
}

static void neighbour_list_sort(rct_neighbour_list *neighbourList)
{
	qsort(neighbourList->items, neighbourList->count, sizeof(rct_neighbour), rct_neighbour_compare);
}

static rct_map_element *footpath_get_element(int x, int y, int z0, int z1, int direction)
{
	rct_map_element *mapElement;
	int slope;

	mapElement = map_get_first_element_at(x >> 5, y >> 5);
	do {
		if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_PATH)
			continue;

		if (z1 == mapElement->base_height) {
			if (footpath_element_is_sloped(mapElement)) {
				slope = footpath_element_get_slope_direction(mapElement);
				if (slope != direction)
					break;
			}
			return mapElement;
		}
		if (z0 == mapElement->base_height) {
			if (!footpath_element_is_sloped(mapElement))
				break;

			slope = footpath_element_get_slope_direction(mapElement) ^ 2;
			if (slope != direction)
				break;

			return mapElement;
		}
	} while (!map_element_is_last_for_tile(mapElement++));
	return NULL;
}

static bool sub_footpath_disconnect_queue_from_path(int x, int y, rct_map_element *mapElement, int action, int direction) {
	if (((mapElement->properties.path.edges & (1 << direction)) == 0) ^ (action < 0))
		return false;
	if ((action < 0) && sub_6E59DC(x, y, mapElement->base_height, mapElement->clearance_height, direction))
		return false;

	int x1 = x + TileDirectionDelta[direction].x;
	int y1 = y + TileDirectionDelta[direction].y;
	int z = mapElement->base_height;
	rct_map_element *otherMapElement = footpath_get_element(x1, y1, z - 2, z, direction);
	if (otherMapElement != NULL && !footpath_element_is_queue(otherMapElement)) {
		mapElement->properties.path.type &= 0xFC;
		if (action > 0) {
			mapElement->properties.path.edges &= ~(1 << direction);
			otherMapElement->properties.path.edges &= ~(1 << ((direction + 2) & 3));
			if (action >= 2) mapElement->properties.path.type |= direction;
		}
		else if (action < 0) {
			mapElement->properties.path.edges |= (1 << direction);
			otherMapElement->properties.path.edges |= (1 << ((direction + 2) & 3));
		}
		if (action != 0) map_invalidate_tile_full(x1, y1);
		return true;
	}
	return false;
}

static bool footpath_disconnect_queue_from_path(int x, int y, rct_map_element *mapElement, int action) {
	if (!footpath_element_is_queue(mapElement)) return false;

	if (mapElement->properties.path.type & 4) return false;

	uint8 c = RCT2_ADDRESS(0x0098D7F0, uint8)[mapElement->properties.path.edges & 0x0F];
	if ((action < 0) ? (c >= 2) : (c < 2)) return false;

	if (action < 0) {
		if (sub_footpath_disconnect_queue_from_path(x, y, mapElement, action, mapElement->properties.path.type & 3))
			return true;
	}

	for (int direction = 0; direction < 4; direction++) {
		if ((action < 0) && (direction == (mapElement->properties.path.type & 3))) continue;
		if (sub_footpath_disconnect_queue_from_path(x, y, mapElement, action, direction))
			return true;
	}

	return false;
}

/**
 * 
 *  rct2: 0x006A6D7E
 */
static void loc_6A6D7E(
	int initialX, int initialY, int z, int direction, rct_map_element *initialMapElement,
	int flags, bool query, rct_neighbour_list *neighbourList
) {
	int x = initialX + TileDirectionDelta[direction].x;
	int y = initialY + TileDirectionDelta[direction].y;
	if (((RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_SCENARIO_EDITOR) || gCheatsSandboxMode) && map_is_edge(x, y)) {
		if (query) {
			neighbour_list_push(neighbourList, 7, direction);
		}
	} else {
		rct_map_element *mapElement = map_get_first_element_at(x >> 5, y >> 5);
		do {
			switch (map_element_get_type(mapElement)) {
			case MAP_ELEMENT_TYPE_PATH:
				if (z == mapElement->base_height) {
					if (footpath_element_is_sloped(mapElement) && footpath_element_get_slope_direction(mapElement) != direction) {
						return;
					} else {
						goto loc_6A6F1F;
					}
				}
				if (z - 2 == mapElement->base_height) {
					if (!footpath_element_is_sloped(mapElement) && footpath_element_get_slope_direction(mapElement) != (direction ^ 2)) {
						return;
					}
					goto loc_6A6F1F;
				}
				break;
			case MAP_ELEMENT_TYPE_TRACK:
				if (z == mapElement->base_height) {
					rct_ride *ride = GET_RIDE(mapElement->properties.track.ride_index);
					if (!ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_FLAT_RIDE)) {
						continue;
					}
					uint16 di = (mapElement->properties.track.sequence & 0x0F) | (mapElement->properties.track.type << 4);
					if (!(RCT2_ADDRESS(0x0099CA64, uint8)[di] & (1 << 5))) {
						return;
					}
					uint16 dx = ((direction - mapElement->type) & 3) ^ 2;
					if (!(RCT2_ADDRESS(0x0099CA64, uint16)[di / 2] & (1 << dx))) {
						return;
					}
					if (query) {
						neighbour_list_push(neighbourList, 1, direction);
					}
					goto loc_6A6FD2;
				}
				break;
			case MAP_ELEMENT_TYPE_ENTRANCE:
				if (z == mapElement->base_height) {
					if (entrance_has_direction(mapElement, ((direction - mapElement->type) & 3) ^ 2)) {
						if (query) {
							neighbour_list_push(neighbourList, 8, direction);
						} else {
							if (mapElement->properties.entrance.type != ENTRANCE_TYPE_PARK_ENTRANCE) {
								sub_6A76E9(mapElement->properties.entrance.ride_index);
							}
						}
						goto loc_6A6FD2;
					}
				}
				break;
			}
		} while (!map_element_is_last_for_tile(mapElement++));
		return;

	loc_6A6F1F:
		if (query) {
			if (sub_6E59DC(x, y, mapElement->base_height, mapElement->clearance_height, direction ^ 2)) {
				return;
			}
			if (footpath_element_is_queue(mapElement)) {
				if (RCT2_ADDRESS(0x0098D7F0, uint8)[mapElement->properties.path.edges & 0x0F] < 2) {
					neighbour_list_push(neighbourList, 3, direction);
				} else {
					if (map_element_get_type(initialMapElement) == MAP_ELEMENT_TYPE_PATH &&
						footpath_element_is_queue(initialMapElement)) {
						if (footpath_disconnect_queue_from_path(x, y, mapElement, 0)) {
							neighbour_list_push(neighbourList, 3, direction);
						}
					}
				}
			} else {
				neighbour_list_push(neighbourList, 2, direction);
			}
		} else {
			footpath_disconnect_queue_from_path(x, y, mapElement, 1 + ((flags >> 6) & 1));
			mapElement->properties.path.edges |= (1 << (direction ^ 2));
			if (footpath_element_is_queue(mapElement)) {
				sub_6A76E9(mapElement->properties.path.ride_index);
			}
		}
		if (!(flags & 0x48)) {
			footpath_interrupt_peeps(x, y, mapElement->base_height * 8);
		}
		map_invalidate_element(x, y, mapElement);
	}

loc_6A6FD2:
	if (map_element_get_type(initialMapElement) == MAP_ELEMENT_TYPE_PATH) {
		if (!query) {
			initialMapElement->properties.path.edges |= (1 << direction);
			map_invalidate_element(initialX, initialY, initialMapElement);
		}
	}
}

static void loc_6A6C85(
	int x, int y, int direction, rct_map_element *mapElement,
	int flags, bool query, rct_neighbour_list *neighbourList
) {
	if (query && sub_6E59DC(x, y, mapElement->base_height, mapElement->clearance_height, direction))
		return;

	if (map_element_get_type(mapElement) == MAP_ELEMENT_TYPE_ENTRANCE) {
		if (!entrance_has_direction(mapElement, (direction - mapElement->type) & 3)) {
			return;
		}
	}

	if (map_element_get_type(mapElement) == MAP_ELEMENT_TYPE_TRACK) {
		rct_ride *ride = GET_RIDE(mapElement->properties.track.ride_index);
		if (!ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_FLAT_RIDE)) {
			return;
		}
		uint16 di = (mapElement->properties.track.sequence & 0x0F) | (mapElement->properties.track.type << 4);
		if (!(RCT2_ADDRESS(0x0099CA64, uint8)[di] & (1 << 5))) {
			return;
		}
		uint16 dx = (direction - mapElement->type) & 3;
		if (!(RCT2_ADDRESS(0x0099CA64, uint16)[di / 2] & (1 << dx))) {
			return;
		}
	}

	int z = mapElement->base_height;
	if (map_element_get_type(mapElement) == MAP_ELEMENT_TYPE_PATH) {
		if (footpath_element_is_sloped(mapElement)) {
			if ((footpath_element_get_slope_direction(mapElement) - direction) & 1) {
				return;
			}
			if (footpath_element_get_slope_direction(mapElement) == direction) {
				z += 2;
			}
		}
	}

	loc_6A6D7E(x, y, z, direction, mapElement, flags, query, neighbourList);
}

/**
 * 
 *  rct2: 0x006A6C66
 */
void footpath_connect_edges(int x, int y, rct_map_element *mapElement, int flags)
{
	rct_neighbour_list neighbourList;
	rct_neighbour neighbour;

	neighbour_list_init(&neighbourList);

	sub_6A7642(x, y, mapElement);
	for (int direction = 0; direction < 4; direction++) {
		loc_6A6C85(x, y, direction, mapElement, flags, true, &neighbourList);
	}

	neighbour_list_sort(&neighbourList);

	if (map_element_get_type(mapElement) == MAP_ELEMENT_TYPE_PATH
			&& footpath_element_is_queue(mapElement)) {
		neighbourList.count = min(neighbourList.count, 2);
	}

	while (neighbour_list_pop(&neighbourList, &neighbour)) {
		loc_6A6C85(x, y, neighbour.direction, mapElement, flags, false, NULL);
	}

	if (map_element_get_type(mapElement) == MAP_ELEMENT_TYPE_PATH) {
		footpath_connect_corners(x, y, mapElement);
	}
}

/**
 * 
 *  rct2: 0x006A742F
 */
void footpath_chain_ride_queue(int rideIndex, int entranceIndex, int x, int y, rct_map_element *mapElement, int direction)
{
	rct_map_element *lastPathElement, *lastQueuePathElement;
	int lastPathX, lastPathY, lastPathDirection;

	lastPathElement = NULL;
	lastQueuePathElement = NULL;
	int z = mapElement->base_height;
	for (;;) {
		if (map_element_get_type(mapElement) == MAP_ELEMENT_TYPE_PATH) {
			lastPathElement = mapElement;
			lastPathX = x;
			lastPathY = y;
			lastPathDirection = direction;
			if (footpath_element_is_sloped(mapElement)) {
				if (footpath_element_get_slope_direction(mapElement) == direction) {
					z += 2;
				}
			}
		}
		
		x += TileDirectionDelta[direction].x;
		y += TileDirectionDelta[direction].y;
		mapElement = map_get_first_element_at(x >> 5, y >> 5);
		do {
			if (lastQueuePathElement == mapElement)
				continue;
			if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_PATH)
				continue;
			if (mapElement->base_height == z) {
				if (footpath_element_is_sloped(mapElement)) {
					if (footpath_element_get_slope_direction(mapElement) != direction)
						break;
				}
				goto foundNextPath;
			}
			if (mapElement->base_height == z - 2) {
				if (!footpath_element_is_sloped(mapElement))
					break;

				if ((footpath_element_get_slope_direction(mapElement) ^ 2) != direction)
					break;

				z -= 2;
				goto foundNextPath;
			}
		} while (!map_element_is_last_for_tile(mapElement++));
		break;

	foundNextPath:
		if (footpath_element_is_queue(mapElement)) {
			mapElement->properties.path.type &= ~(1 << 3);
			mapElement->properties.path.edges |= (1 << (direction ^ 2));
			mapElement->properties.path.ride_index = rideIndex;
			mapElement->properties.path.additions &= 0x8F;
			mapElement->properties.path.additions |= (entranceIndex & 7) << 4;
			if (lastQueuePathElement == NULL) {
				lastQueuePathElement = mapElement;
			}

			if (mapElement->properties.path.edges & (1 << direction))
				continue;

			direction = (direction + 1) & 3;
			if (mapElement->properties.path.edges & (1 << direction))
				continue;

			direction ^= 2;
			if (mapElement->properties.path.edges & (1 << direction))
				continue;
		}
		break;
	}

	if (rideIndex != 255 && lastPathElement != NULL) {
		if (footpath_element_is_queue(lastPathElement)) {
			lastPathElement->properties.path.type |= (1 << 3);
			lastPathElement->type &= 0x3F;
			lastPathElement->type |= lastPathDirection << 6;

			map_animation_create(
				MAP_ANIMATION_TYPE_QUEUE_BANNER,
				lastPathX,
				lastPathY,
				lastPathElement->base_height
			);
		}
	}

}

/**
 * 
 *  rct2: 0x006A759F
 */
void sub_6A759F()
{
	uint8 *esi;
	int i, x, y, z, direction, rideIndex;
	rct_ride *ride;
	rct_map_element *mapElement;

	for (esi = (uint8*)0x00F3EFF8; esi < RCT2_GLOBAL(0x00F3EFF4, uint8*); esi++) {
		rideIndex = *esi;
		ride = GET_RIDE(rideIndex);
		if (ride->type == RIDE_TYPE_NULL)
			continue;

		for (i = 0; i < 4; i++) {
			if (ride->entrances[i] == 0xFFFF)
				continue;

			x = ride->entrances[i] & 0xFF;
			y = ride->entrances[i] >> 8;
			z = ride->station_heights[i];

			mapElement = map_get_first_element_at(x, y);
			do {
				if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_ENTRANCE)
					continue;
				if (mapElement->base_height != z)
					continue;
				if (mapElement->properties.entrance.type != ENTRANCE_TYPE_RIDE_ENTRANCE)
					continue;

				direction = (mapElement->type & 3) ^ 2;
				footpath_chain_ride_queue(rideIndex, i, x << 5, y << 5, mapElement, direction);
			} while (!map_element_is_last_for_tile(mapElement++));
		}
	}
}

/**
 *
 *  rct2: 0x0069ADBD
 */
static void footpath_unown(int x, int y, rct_map_element *pathElement)
{
	int ownershipUnk = 0;
	int z = pathElement->base_height;
	rct_map_element *surfaceElement = map_get_surface_element_at(x >> 5, y >> 5);
	if (surfaceElement->base_height |= z) {
		z -= 2;
		if (surfaceElement->base_height |= z) {
			ownershipUnk = (surfaceElement->properties.surface.ownership & 0xCF) >> 4;
		}
	}
	map_buy_land_rights(x, y, x, y, 6, 1);
}

bool get_next_direction(int edges, int *direction)
{
	int index = bitscanforward(edges);
	if (index == -1)
		return false;

	*direction = index;
	return true;
}

/**
 *
 *  rct2: 0x0069AC1A
 */
int footpath_is_connected_to_map_edge_recurse(
	int x, int y, int z, int direction, int flags,
	int level, int distanceFromJunction, int junctionTolerance
) {
	rct_map_element *mapElement;
	int edges, slopeDirection;

	x += TileDirectionDelta[direction].x;
	y += TileDirectionDelta[direction].y;
	if (++level > 250)
		return FOOTPATH_SEARCH_TOO_COMPLEX;

	// Check if we are at edge of map
	if (x < 32 || y < 32)
		return FOOTPATH_SEARCH_SUCCESS;
	if (x >= RCT2_GLOBAL(RCT2_ADDRESS_MAP_SIZE_UNITS, uint16) || y >= RCT2_GLOBAL(RCT2_ADDRESS_MAP_SIZE_UNITS, uint16))
		return FOOTPATH_SEARCH_SUCCESS;

	mapElement = map_get_first_element_at(x >> 5, y >> 5);
	do {
		if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_PATH)
			continue;

		if (
			footpath_element_is_sloped(mapElement) &&
			(slopeDirection = footpath_element_get_slope_direction(mapElement)) != direction
		) {
			if ((slopeDirection ^ 2) != direction) continue;
			if (mapElement->base_height + 2 != z) continue;
		} else if (mapElement->base_height != z) {
			continue;
		}

		if (mapElement->type & RCT2_GLOBAL(0x00F1AEE0, uint8)) continue;

		if (flags & 0x20) {
			footpath_unown(x, y, mapElement);
		}
		edges = mapElement->properties.path.edges & 0x0F;
		direction ^= 2;
		if (!(flags & 0x80)) {
			if (mapElement[1].type == MAP_ELEMENT_TYPE_BANNER) {
				for (int i = 1; i < 4; i++) {
					if (map_element_is_last_for_tile(&mapElement[i - 1])) break;
					if (mapElement[i].type != MAP_ELEMENT_TYPE_BANNER) break;
					edges &= mapElement[i].properties.banner.flags;
				}
			}
			if (mapElement[2].type == MAP_ELEMENT_TYPE_BANNER && mapElement[1].type != MAP_ELEMENT_TYPE_PATH) {
				for (int i = 1; i < 6; i++) {
					if (map_element_is_last_for_tile(&mapElement[i - 1])) break;
					if (mapElement[i].type != MAP_ELEMENT_TYPE_BANNER) break;
					edges &= mapElement[i].properties.banner.flags;
				}
			}
		}
		goto searchFromFootpath;
	} while (!map_element_is_last_for_tile(mapElement++));
	return level == 1 ? FOOTPATH_SEARCH_NOT_FOUND : FOOTPATH_SEARCH_INCOMPLETE;

searchFromFootpath:
	// Exclude direction we came from
	z = mapElement->base_height;
	edges &= ~(1 << direction);

	// Find next direction to go
	if (!get_next_direction(edges, &direction)) {
		return FOOTPATH_SEARCH_INCOMPLETE;
	}

	edges &= ~(1 << direction);
	if (edges == 0) {
		// Only possible direction to go
		if (footpath_element_is_sloped(mapElement) && footpath_element_get_slope_direction(mapElement) == direction) {
			z += 2;
		}
		return footpath_is_connected_to_map_edge_recurse(
			x, y, z, direction, flags,
			level, distanceFromJunction + 1, junctionTolerance
		);
	} else {
		// We have reached a junction
		if (distanceFromJunction != 0) {
			junctionTolerance--;
		}
		junctionTolerance--;
		if (junctionTolerance < 0) {
			return FOOTPATH_SEARCH_TOO_COMPLEX;
		}

		do {
			edges &= ~(1 << direction);
			if (footpath_element_is_sloped(mapElement) && footpath_element_get_slope_direction(mapElement) == direction) {
				z += 2;
			}
			int result = footpath_is_connected_to_map_edge_recurse(x, y, z, direction, flags, level, 0, junctionTolerance);
			if (result == FOOTPATH_SEARCH_SUCCESS) {
				return result;
			}
		} while (get_next_direction(edges, &direction));

		return FOOTPATH_SEARCH_INCOMPLETE;
	}
}

int footpath_is_connected_to_map_edge(int x, int y, int z, int direction, int flags)
{
	RCT2_GLOBAL(0x00F1AEE0, uint8) = 1;
	return footpath_is_connected_to_map_edge_recurse(x, y, z, direction, flags, 0, 0, 16);
}

bool footpath_element_is_sloped(rct_map_element *mapElement)
{
	return mapElement->properties.path.type & 4;
}

int footpath_element_get_slope_direction(rct_map_element *mapElement)
{
	return mapElement->properties.path.type & 3;
}

bool footpath_element_is_queue(rct_map_element *mapElement)
{
	return mapElement->type & 1;
}

/**
 *
 *  rct2: 0x006A76E9
 */
void sub_6A76E9(int rideIndex)
{
	if (rideIndex == 255)
		return;

	*(RCT2_GLOBAL(0x00F3EFF4, uint8*)) = rideIndex;
	RCT2_GLOBAL(0x00F3EFF4, uint8*)++;
}

/**
 *
 *  rct2: 0x006A7642
 */
void sub_6A7642(int x, int y, rct_map_element *mapElement)
{
	int elementType = map_element_get_type(mapElement);
	switch (elementType) {
	case MAP_ELEMENT_TYPE_PATH:
		if (footpath_element_is_queue(mapElement)) {
			sub_6A76E9(mapElement->properties.path.ride_index);
			for (int direction = 0; direction < 4; direction++) {
				if (mapElement->properties.path.edges & (1 << direction)) {
					footpath_chain_ride_queue(255, 0, x, y, mapElement, direction);
				}
			}
			mapElement->properties.path.ride_index = 255;
		}
		break;
	case MAP_ELEMENT_TYPE_ENTRANCE:
		if (mapElement->properties.entrance.type == ENTRANCE_TYPE_RIDE_ENTRANCE) {
			sub_6A76E9(mapElement->properties.entrance.ride_index);
			footpath_chain_ride_queue(255, 0, x, y, mapElement, (mapElement->type & MAP_ELEMENT_DIRECTION_MASK) ^ 2);
		}
		break;
	}
}

/**
 *
 *  rct2: 0x006A6B7F
 */
static void footpath_remove_edges_towards_here(int x, int y, int z, int direction, rct_map_element *mapElement, bool isQueue)
{
	int d;

	if (footpath_element_is_queue(mapElement)) {
		sub_6A76E9(mapElement->properties.path.ride_index);
	}

	d = direction ^ 2;
	mapElement->properties.path.edges &= ~(1 << d);
	d = ((d - 1) & 3) + 4;
	mapElement->properties.path.edges &= ~(1 << d);
	d = (((d - 4) + 1) & 3) + 4;
	mapElement->properties.path.edges &= ~(1 << d);
	map_invalidate_tile(x, y, mapElement->base_height, mapElement->clearance_height);

	if (isQueue) footpath_disconnect_queue_from_path(x, y, mapElement, -1);

	direction = (direction + 1) & 3;
	x += TileDirectionDelta[direction].x;
	y += TileDirectionDelta[direction].y;

	mapElement = map_get_first_element_at(x >> 5, y >> 5);
	do {
		if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_PATH)
			continue;
		if (mapElement->base_height != z)
			continue;

		if (footpath_element_is_sloped(mapElement))
			break;

		d = ((direction + 1) & 3) + 4;
		mapElement->properties.path.edges &= ~(1 << d);
		map_invalidate_tile(x, y, mapElement->base_height, mapElement->clearance_height);
		break;
	} while (!map_element_is_last_for_tile(mapElement++));
}

/**
 *
 *  rct2: 0x006A6B14
 */
static void footpath_remove_edges_towards(int x, int y, int z0, int z1, int direction, bool isQueue)
{
	rct_map_element *mapElement;
	int slope;

	mapElement = map_get_first_element_at(x >> 5, y >> 5);
	do {
		if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_PATH)
			continue;

		if (z1 == mapElement->base_height) {
			if (footpath_element_is_sloped(mapElement)) {
				slope = footpath_element_get_slope_direction(mapElement);
				if (slope != direction)
					break;
			}
			footpath_remove_edges_towards_here(x, y, z1, direction, mapElement, isQueue);
			break;
		}
		if (z0 == mapElement->base_height) {
			if (!footpath_element_is_sloped(mapElement))
				break;

			slope = footpath_element_get_slope_direction(mapElement) ^ 2;
			if (slope != direction)
				break;

			footpath_remove_edges_towards_here(x, y, z1, direction, mapElement, isQueue);
			break;
		}
	} while (!map_element_is_last_for_tile(mapElement++));
}

/**
 *
 *  rct2: 0x006A6AA7
 * @param x x-coordinate in units (not tiles)
 * @param y y-coordinate in units (not tiles)
 */
void footpath_remove_edges_at(int x, int y, rct_map_element *mapElement)
{
	rct_ride *ride;
	int z0, z1, slope;

	if (map_element_get_type(mapElement) == MAP_ELEMENT_TYPE_TRACK) {
		int rideIndex = mapElement->properties.track.ride_index;
		ride = GET_RIDE(rideIndex);
		if (!ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_FLAT_RIDE))
			return;
	}

	sub_6A7642(x, y, mapElement);

	for (int direction = 0; direction < 4; direction++) {
		z1 = mapElement->base_height;
		if (map_element_get_type(mapElement) == MAP_ELEMENT_TYPE_PATH) {
			if (footpath_element_is_sloped(mapElement)) {
				slope = footpath_element_get_slope_direction(mapElement);
				if ((slope - direction) & 1)
					continue;

				z1 += slope == direction ? 2 : 0;
			}
		}
		z0 = z1 - 2;
		footpath_remove_edges_towards(x + TileDirectionDelta[direction].x, y + TileDirectionDelta[direction].y,
			z0, z1, direction, footpath_element_is_queue(mapElement));
	}

	if (map_element_get_type(mapElement) == MAP_ELEMENT_TYPE_PATH)
		mapElement->properties.path.edges = 0;
}
