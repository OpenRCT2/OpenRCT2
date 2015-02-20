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
#include "footpath.h"
#include "map.h"

void sub_673883(int x, int y, int z);
void sub_69A48B(int x, int y, int z);
void sub_6A6C66(int x, int y, rct_map_element *mapElement, int flags);
void sub_6A759F();

enum {
	FOOTPATH_CONSTRUCTION_FLAG_ALLOW_DURING_PAUSED = 1 << 3
};

const rct_xy16 word_981D6C[] = {
	{ -1,  0 },
	{  0,  1 },
	{  1,  0 },
	{  0, -1 }
};

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
		if (y != RCT2_GLOBAL(0x001358830, uint16) - 32) {
			direction++;
			if (x != RCT2_GLOBAL(0x001358830, uint16) - 32) {
				direction++;
				if (y != 32)
					return;
			}
		}
	}

	peepSpawn->x = (word_981D6C[direction].x * 15) + 16;
	peepSpawn->y = (word_981D6C[direction].y * 15) + 16;
	peepSpawn->direction = direction;
	peepSpawn->z = z / 2;
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
		sub_6A6C66(x, y, mapElement, flags);

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
		sub_673883(x, y, RCT2_GLOBAL(0x009DEA62, uint16));

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

	if (!map_can_construct_with_clear_at(x, y, z, zHigh, (void*)0x006A6733, bl))
		return MONEY32_UNDEFINED;

	RCT2_GLOBAL(0x00F3EFA4, uint8) = RCT2_GLOBAL(0x00F1AD60, uint8);
	if (RCT2_GLOBAL(0x00F1AD60, uint8) & 4) {
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
			mapElement->flags |= 1 << 4;

		RCT2_GLOBAL(0x00F3EFF4, uint32) = 0x00F3EFF8;

		if (!(flags & (1 << 7)))
			sub_6A6AA7(x, y, mapElement);

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
			uint8 *unk = RCT2_ADDRESS(0x009ADA50, uint8*)[RCT2_GLOBAL(0x00F3EF88, uint16)];
			uint16 unk6 = RCT2_GLOBAL(unk + 6, uint16);

			if ((unk6 & 0x80) && (mapElement->properties.path.type & 4)) {
				RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TEXT, rct_string_id) = STR_CANT_BUILD_THIS_ON_SLOPED_FOOTPATH;
				return MONEY32_UNDEFINED;
			}

			if ((unk6 & 0x40) && (mapElement->type & 1)) {
				RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TEXT, rct_string_id) = STR_CANNOT_PLACE_THESE_ON_QUEUE_LINE_AREA;
				return MONEY32_UNDEFINED;
			}

			if ((unk6 & 0x30) && (mapElement->properties.path.edges & 0x0F) == 0x0F) {
				RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TEXT, rct_string_id) = STR_NONE;
				return MONEY32_UNDEFINED;
			}

			if ((unk6 & 0x100) && !(mapElement->type & 1)) {
				RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TEXT, rct_string_id) = STR_CAN_ONLY_PLACE_THESE_ON_QUEUE_AREA;
				return MONEY32_UNDEFINED;
			}

			RCT2_GLOBAL(0x00F3EFD9, money32) += RCT2_GLOBAL(unk + 10, money16);
		}

		if (flags & (1 << 4))
			return MONEY32_UNDEFINED;

		if (!(flags & (1 << 6))) {
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
			uint8 *unk = RCT2_ADDRESS(0x009ADA50, uint8*)[RCT2_GLOBAL(0x00F3EF88, uint16)];
			uint16 unk6 = RCT2_GLOBAL(unk + 6, uint16);
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
			sub_6A6AA7(x, y, mapElement);

		mapElement->properties.path.type = (mapElement->properties.path.type & 0x0F) | (type << 4);
		mapElement->type = (mapElement->type & 0xFE) | (type >> 7);
		mapElement->properties.path.additions = (mapElement->properties.path.additions & 0xF0) | RCT2_GLOBAL(0x00F3EF88, uint8);
		mapElement->flags &= ~MAP_ELEMENT_FLAG_BROKEN;

		loc_6A6620(flags, x, y, mapElement);
	}

	return RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & PARK_FLAGS_NO_MONEY ? 0 : RCT2_GLOBAL(0x00F3EFD9, money32);
}

static money32 footpath_place_real(int type, int x, int y, int z, int slope, int flags)
{
	rct_map_element *mapElement;

	RCT2_GLOBAL(0x0141F56C, uint8) = 12;
	RCT2_GLOBAL(0x009DEA5E, uint16) = x + 16;
	RCT2_GLOBAL(0x009DEA60, uint16) = y + 16;
	RCT2_GLOBAL(0x009DEA62, uint16) = z * 8;

	if (!(flags & FOOTPATH_CONSTRUCTION_FLAG_ALLOW_DURING_PAUSED) && RCT2_GLOBAL(0x009DEA6E, uint8) != 0) {
		RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TEXT, rct_string_id) = STR_CONSTRUCTION_NOT_POSSIBLE_WHILE_GAME_IS_PAUSED;
		return MONEY32_UNDEFINED;
	}

	if (flags & GAME_COMMAND_FLAG_APPLY)
		sub_69A48B(x, y, z * 8);

	RCT2_GLOBAL(0x00F3EFD9, money32) = 0;
	RCT2_GLOBAL(0x00F3EFA4, uint8) = 0;
	RCT2_GLOBAL(0x00F3EF88, uint16) = 0; // di

	if (x >= RCT2_GLOBAL(0x01358830, uint16) || y >= RCT2_GLOBAL(0x01358830, uint16)) {
		RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TEXT, rct_string_id) = STR_OFF_EDGE_OF_MAP;
		return MONEY32_UNDEFINED;
	}

	if (!(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_SCENARIO_EDITOR) && !map_is_location_owned(x, y, z * 8))
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

money32 footpath_remove_real(int x, int y, int z, int flags)
{
	rct_map_element *mapElement;

	RCT2_GLOBAL(0x0141F56C, uint8) = 12;
	RCT2_GLOBAL(0x009DEA5E, uint16) = x + 16;
	RCT2_GLOBAL(0x009DEA60, uint16) = y + 16;
	RCT2_GLOBAL(0x009DEA62, uint16) = z * 8;

	if (!(flags & FOOTPATH_CONSTRUCTION_FLAG_ALLOW_DURING_PAUSED) && RCT2_GLOBAL(0x009DEA6E, uint8) != 0) {
		RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TEXT, rct_string_id) = STR_CONSTRUCTION_NOT_POSSIBLE_WHILE_GAME_IS_PAUSED;
		return MONEY32_UNDEFINED;
	}

	if (flags & GAME_COMMAND_FLAG_APPLY) {
		sub_69A48B(x, y, z * 8);
		sub_673883(x, y, z * 8);
	}

	if (!(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_SCENARIO_EDITOR) && !map_is_location_owned(x, y, z * 8))
		return MONEY32_UNDEFINED;

	mapElement = map_get_footpath_element(x / 32, y / 32, z);
	if (mapElement != NULL && (flags & GAME_COMMAND_FLAG_APPLY)) {
		RCT2_GLOBAL(0x00F3EFF4, uint32) = 0x00F3EFF8;
		RCT2_CALLPROC_X(0x006BA23E, 0, 0, 0, 0, (int)mapElement, 0, 0);
		sub_6A6AA7(x, y, mapElement);
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
	if (*ebx & (1 << 5))
		RCT2_CALLFUNC_X(0x006A61DE, eax, ebx, ecx, edx, esi, edi, ebp);
	else
		*ebx = footpath_place_real((*edx >> 8) & 0xFF, *eax & 0xFFFF, *ecx & 0xFFFF, *edx & 0xFF, (*ebx >> 8) & 0xFF, *ebx & 0xFF);
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
void sub_6A7831()
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
 * 
 *  rct2: 0x0068A0C9
 */
void sub_68A0C9(int screenX, int screenY, int *x, int *y, int *direction, rct_map_element **mapElement)
{
	int eax, ebx, ecx, edx, esi, edi, ebp;
	eax = screenX;
	ebx = screenY;
	RCT2_CALLFUNC_X(0x0068A0C9, &eax, &ebx, &ecx, &edx, &esi, &edi, &ebp);
	if (x != NULL) *x = *((uint16*)&eax);
	if (y != NULL) *y = *((uint16*)&ebx);
	if (direction != NULL) *direction = *((uint8*)&ecx);
	if (mapElement != NULL) *mapElement = (rct_map_element*)edx;
}

/**
 * 
 *  rct2: 0x00673883
 */
void sub_673883(int x, int y, int z)
{
	RCT2_CALLPROC_X(0x00673883, x, 0, y, z, 0, 0, 0);
}

/**
 * 
 *  rct2: 0x0069A48B
 */
void sub_69A48B(int x, int y, int z)
{
	RCT2_CALLPROC_X(0x0069A48B, x, 0, y, z, 0, 0, 0);
}

/**
 * 
 *  rct2: 0x006A6C66
 */
void sub_6A6C66(int x, int y, rct_map_element *mapElement, int flags)
{
	RCT2_CALLPROC_X(0x006A6C66, x, flags, y, 0, (int)mapElement, 0, 0);
}

void sub_6A742F(int rideIndex, int entranceIndex, int x, int y, rct_map_element *mapElement, int direction)
{
	RCT2_CALLPROC_X(0x006A742F, x, direction, y, (entranceIndex << 8) | rideIndex, (int)mapElement, 0, 0);
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
				sub_6A742F(rideIndex, i, x << 5, y << 5, mapElement, direction);
			} while (!map_element_is_last_for_tile(mapElement++));
		}
	}
}