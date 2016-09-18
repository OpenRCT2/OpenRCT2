#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#include "../cheats.h"
#include "../config.h"
#include "../game.h"
#include "../localisation/localisation.h"
#include "../management/finance.h"
#include "../network/network.h"
#include "../util/util.h"
#include "../object_list.h"
#include "footpath.h"
#include "map.h"
#include "map_animation.h"
#include "scenery.h"
#include "../ride/track.h"
#include "../ride/track_data.h"

void footpath_interrupt_peeps(int x, int y, int z);
void sub_6A7642(int x, int y, rct_map_element *mapElement);

uint8 gFootpathProvisionalFlags;
rct_xyz16 gFootpathProvisionalPosition;
uint8 gFootpathConstructionMode;
uint16 gFootpathSelectedId;
uint8 gFootpathSelectedType;
rct_xyz16 gFootpathConstructFromPosition;
uint8 gFootpathConstructDirection;
uint8 gFootpathConstructSlope;
uint8 gFootpathConstructValidDirections;
money32 gFootpathPrice;
uint8 gFootpathGroundFlags;

static uint8 *_footpathQueueChainNext;
static uint8 _footpathQueueChain[64];

/** rct2: 0x00981D6C, 0x00981D6E */
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

/** rct2: 0x0098D7F0 */
static const uint8 connected_path_count[] = {
	0, // 0b0000
	1, // 0b0001
	1, // 0b0010
	2, // 0b0011
	1, // 0b0100
	2, // 0b0101
	2, // 0b0110
	3, // 0b0111
	1, // 0b1000
	2, // 0b1001
	2, // 0b1010
	3, // 0b1011
	2, // 0b1100
	3, // 0b1101
	3, // 0b1110
	4, // 0b1111
};

int entrance_get_directions(rct_map_element *mapElement)
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
	int direction = 0;
	if (x != 32) {
		direction++;
		if (y != gMapSizeUnits - 32) {
			direction++;
			if (x != gMapSizeUnits - 32) {
				direction++;
				if (y != 32)
					return;
			}
		}
	}

	rct2_peep_spawn *peepSpawn = &gPeepSpawns[0];
	peepSpawn->x = x + (word_981D6C[direction].x * 15) + 16;
	peepSpawn->y = y + (word_981D6C[direction].y * 15) + 16;
	peepSpawn->direction = direction;
	peepSpawn->z = z;
}

static rct_map_element *map_get_footpath_element(int x, int y, int z)
{
	rct_map_element *mapElement;

	mapElement = map_get_first_element_at(x, y);
	do {
		if (map_element_get_type(mapElement) == MAP_ELEMENT_TYPE_PATH && mapElement->base_height == z)
			return mapElement;
	} while (!map_element_is_last_for_tile(mapElement++));

	return NULL;
}

static rct_map_element *map_get_footpath_element_slope(int x, int y, int z, int slope)
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

	if ((mapElement->properties.path.type & 4) && !(flags & GAME_COMMAND_FLAG_GHOST)) {
		direction = mapElement->properties.path.type & 3;
		z = mapElement->base_height;
		map_remove_intersecting_walls(x, y, z, z + 6, direction ^ 2);
		map_remove_intersecting_walls(x, y, z, z + 6, direction);
		mapElement = map_get_footpath_element(x / 32, y / 32, z);
	}

	if (!(flags & GAME_COMMAND_FLAG_7))
		footpath_connect_edges(x, y, mapElement, flags);

	sub_6A759F();
	map_invalidate_tile_full(x, y);
}

/** rct2: 0x0098D7EC */
static const uint8 byte_98D7EC[] = {
	207, 159, 63, 111
};

static money32 footpath_element_insert(int type, int x, int y, int z, int slope, int flags, uint8 pathItemType)
{
	rct_map_element *mapElement;
	int bl, zHigh;

	if (!sub_68B044())
		return MONEY32_UNDEFINED;

	if ((flags & GAME_COMMAND_FLAG_APPLY) && !(flags & (GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_GHOST)))
		footpath_remove_litter(x, y, gCommandPosition.z);

	// loc_6A649D:
	gFootpathPrice += MONEY(12, 00);

	bl = 15;
	zHigh = z + 4;
	if (slope & 4) {
		bl = byte_98D7EC[slope & 3];
		zHigh += 2;
	}

	if (!gCheatsDisableClearanceChecks && !map_can_construct_with_clear_at(x, y, z, zHigh, &map_place_non_scenery_clear_func, bl, flags, &gFootpathPrice))
		return MONEY32_UNDEFINED;

	gFootpathGroundFlags = gMapGroundFlags;
	if (!gCheatsDisableClearanceChecks && (gMapGroundFlags & ELEMENT_IS_UNDERWATER)) {
		gGameCommandErrorText = STR_CANT_BUILD_THIS_UNDERWATER;
		return MONEY32_UNDEFINED;
	}

	mapElement = map_get_surface_element_at((x / 32), (y / 32));

	int supportHeight = z - mapElement->base_height;
	gFootpathPrice += supportHeight < 0 ? MONEY(20, 00) : (supportHeight / 2) * MONEY(5, 00);

	if (flags & GAME_COMMAND_FLAG_APPLY) {
		mapElement = map_element_insert(x / 32, y / 32, z, 0x0F);
		assert(mapElement != NULL);
		mapElement->type = MAP_ELEMENT_TYPE_PATH;
		mapElement->clearance_height = z + 4 + (slope & 4 ? 2 : 0);
		mapElement->properties.path.type = (type << 4) | (slope & 7);
		mapElement->type |= type >> 7;
		mapElement->properties.path.additions = pathItemType;
		mapElement->properties.path.addition_status = 255;
		mapElement->flags &= ~MAP_ELEMENT_FLAG_BROKEN;
		if (flags & GAME_COMMAND_FLAG_GHOST)
			mapElement->flags |= MAP_ELEMENT_FLAG_GHOST;

		footpath_queue_chain_reset();

		if (!(flags & (1 << 7)))
			footpath_remove_edges_at(x, y, mapElement);

		if ((gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) && !(flags & (1 << 6)))
			automatically_set_peep_spawn(x, y, mapElement->base_height / 2);

		loc_6A6620(flags, x, y, mapElement);
	}
	return gParkFlags & PARK_FLAGS_NO_MONEY ? 0 : gFootpathPrice;
}

static money32 footpath_element_update(int x, int y, rct_map_element *mapElement, int type, int flags, uint8 pathItemType)
{
	if ((mapElement->properties.path.type >> 4) != (type & 0x0F) || (mapElement->type & 1) != (type >> 7)) {
		gFootpathPrice += MONEY(6, 00);
	} else if (pathItemType != 0) {
		if (
			!(flags & GAME_COMMAND_FLAG_GHOST) &&
			footpath_element_get_path_scenery(mapElement) == pathItemType &&
			!(mapElement->flags & MAP_ELEMENT_FLAG_BROKEN)
		) {
			if (flags & GAME_COMMAND_FLAG_4)
				return MONEY32_UNDEFINED;

			return gParkFlags & PARK_FLAGS_NO_MONEY ? 0 : gFootpathPrice;
		}

		if (pathItemType != 0) {
			rct_scenery_entry* scenery_entry = get_footpath_item_entry(pathItemType - 1);
			uint16 unk6 = scenery_entry->path_bit.flags;

			if ((unk6 & PATH_BIT_FLAG_DONT_ALLOW_ON_SLOPE) && footpath_element_is_sloped(mapElement)) {
				gGameCommandErrorText = STR_CANT_BUILD_THIS_ON_SLOPED_FOOTPATH;
				return MONEY32_UNDEFINED;
			}

			if ((unk6 & PATH_BIT_FLAG_DONT_ALLOW_ON_QUEUE) && footpath_element_is_queue(mapElement)) {
				gGameCommandErrorText = STR_CANNOT_PLACE_THESE_ON_QUEUE_LINE_AREA;
				return MONEY32_UNDEFINED;
			}

			if (!(unk6 & (PATH_BIT_FLAG_JUMPING_FOUNTAIN_WATER | PATH_BIT_FLAG_JUMPING_FOUNTAIN_SNOW)) && (mapElement->properties.path.edges & 0x0F) == 0x0F) {
				gGameCommandErrorText = STR_NONE;
				return MONEY32_UNDEFINED;
			}

			if ((unk6 & PATH_BIT_FLAG_IS_QUEUE_SCREEN) && !footpath_element_is_queue(mapElement)) {
				gGameCommandErrorText = STR_CAN_ONLY_PLACE_THESE_ON_QUEUE_AREA;
				return MONEY32_UNDEFINED;
			}

			gFootpathPrice += scenery_entry->path_bit.price;
		}

		if (flags & GAME_COMMAND_FLAG_4)
			return MONEY32_UNDEFINED;

		// Should place a ghost?
		if (flags & GAME_COMMAND_FLAG_GHOST) {
			// Check if there is something on the path already
			if (footpath_element_has_path_scenery(mapElement)) {
				gGameCommandErrorText = STR_NONE;
				return MONEY32_UNDEFINED;
			}

			// There is nothing yet - check if we should place a ghost
			if (flags & GAME_COMMAND_FLAG_APPLY)
				footpath_scenery_set_is_ghost(mapElement, true);
		}

		if (!(flags & GAME_COMMAND_FLAG_APPLY))
			return gParkFlags & PARK_FLAGS_NO_MONEY ? 0 : gFootpathPrice;

		if (
			(pathItemType != 0 && !(flags & GAME_COMMAND_FLAG_GHOST)) ||
			(pathItemType == 0 && footpath_element_path_scenery_is_ghost(mapElement))
		) {
			footpath_scenery_set_is_ghost(mapElement, false);
		}

		footpath_element_set_path_scenery(mapElement, pathItemType);
		mapElement->flags &= ~MAP_ELEMENT_FLAG_BROKEN;
		if (pathItemType != 0) {
			rct_scenery_entry* scenery_entry = get_footpath_item_entry(pathItemType - 1);
			if (scenery_entry->path_bit.flags & PATH_BIT_FLAG_IS_BIN) {
				mapElement->properties.path.addition_status = 255;
			}
		}
		map_invalidate_tile_full(x, y);
		return gParkFlags & PARK_FLAGS_NO_MONEY ? 0 : gFootpathPrice;
	}

	if (flags & GAME_COMMAND_FLAG_4)
		return MONEY32_UNDEFINED;

	if (flags & GAME_COMMAND_FLAG_APPLY) {
		footpath_queue_chain_reset();

		if (!(flags & GAME_COMMAND_FLAG_7))
			footpath_remove_edges_at(x, y, mapElement);

		mapElement->properties.path.type = (mapElement->properties.path.type & 0x0F) | (type << 4);
		mapElement->type = (mapElement->type & 0xFE) | (type >> 7);
		footpath_element_set_path_scenery(mapElement, pathItemType);
		mapElement->flags &= ~MAP_ELEMENT_FLAG_BROKEN;

		loc_6A6620(flags, x, y, mapElement);
	}

	return gParkFlags & PARK_FLAGS_NO_MONEY ? 0 : gFootpathPrice;
}

static money32 footpath_place_real(int type, int x, int y, int z, int slope, int flags, uint8 pathItemType)
{
	rct_map_element *mapElement;

	gCommandExpenditureType = RCT_EXPENDITURE_TYPE_LANDSCAPING;
	gCommandPosition.x = x + 16;
	gCommandPosition.y = y + 16;
	gCommandPosition.z = z * 8;

	if (!(flags & GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED) && game_is_paused() && !gCheatsBuildInPauseMode) {
		gGameCommandErrorText = STR_CONSTRUCTION_NOT_POSSIBLE_WHILE_GAME_IS_PAUSED;
		return MONEY32_UNDEFINED;
	}

	if (flags & GAME_COMMAND_FLAG_APPLY)
		footpath_interrupt_peeps(x, y, z * 8);

	gFootpathPrice = 0;
	gFootpathGroundFlags = 0;

	if (x >= gMapSizeUnits || y >= gMapSizeUnits) {
		gGameCommandErrorText = STR_OFF_EDGE_OF_MAP;
		return MONEY32_UNDEFINED;
	}

	if (!((gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) || gCheatsSandboxMode) && !map_is_location_owned(x, y, z * 8))
		return MONEY32_UNDEFINED;

	if (slope & 8) {
		gGameCommandErrorText = STR_LAND_SLOPE_UNSUITABLE;
		return MONEY32_UNDEFINED;
	}

	if (z < 2) {
		gGameCommandErrorText = STR_TOO_LOW;
		return MONEY32_UNDEFINED;
	}

	if (z > 248) {
		gGameCommandErrorText = STR_TOO_HIGH;
		return MONEY32_UNDEFINED;
	}

	// Force ride construction to recheck area
	_currentTrackSelectionFlags |= 8;

	if (gGameCommandNestLevel == 1 && !(flags & GAME_COMMAND_FLAG_GHOST)) {
		rct_xyz16 coord;
		coord.x = x + 16;
		coord.y = y + 16;
		coord.z = map_element_height(coord.x, coord.y);
		network_set_player_last_action_coord(network_get_player_index(game_command_playerid), coord);
	}

	footpath_provisional_remove();
	mapElement = map_get_footpath_element_slope((x / 32), (y / 32), z, slope);
	if (mapElement == NULL) {
		return footpath_element_insert(type, x, y, z, slope, flags, pathItemType);
	} else {
		return footpath_element_update(x, y, mapElement, type, flags, pathItemType);
	}
}

/**
 *
 *  rct2: 0x006BA23E
 */
static void remove_banners_at_element(int x, int y, rct_map_element* mapElement){
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

	gCommandExpenditureType = RCT_EXPENDITURE_TYPE_LANDSCAPING;
	gCommandPosition.x = x + 16;
	gCommandPosition.y = y + 16;
	gCommandPosition.z = z * 8;

	if (!(flags & GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED) && game_is_paused() && !gCheatsBuildInPauseMode) {
		gGameCommandErrorText = STR_CONSTRUCTION_NOT_POSSIBLE_WHILE_GAME_IS_PAUSED;
		return MONEY32_UNDEFINED;
	}

	if (flags & GAME_COMMAND_FLAG_APPLY) {
		footpath_interrupt_peeps(x, y, z * 8);
		footpath_remove_litter(x, y, z * 8);
	}

	if (!((gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) || gCheatsSandboxMode) && !map_is_location_owned(x, y, z * 8))
		return MONEY32_UNDEFINED;

	mapElement = map_get_footpath_element(x / 32, y / 32, z);
	if (mapElement != NULL && (flags & GAME_COMMAND_FLAG_APPLY)) {
		if (gGameCommandNestLevel == 1 && !(flags & GAME_COMMAND_FLAG_GHOST)) {
			rct_xyz16 coord;
			coord.x = x + 16;
			coord.y = y + 16;
			coord.z = map_element_height(coord.x, coord.y);
			network_set_player_last_action_coord(network_get_player_index(game_command_playerid), coord);
		}

		footpath_queue_chain_reset();
		remove_banners_at_element(x, y, mapElement);
		footpath_remove_edges_at(x, y, mapElement);
		map_invalidate_tile_full(x, y);
		map_element_remove(mapElement);
		sub_6A759F();
	}

	return (flags & (1 << 5)) || (gParkFlags & PARK_FLAGS_NO_MONEY) ? 0 : -MONEY(10,00);
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

static money32 footpath_place_from_track(int type, int x, int y, int z, int slope, int edges, int flags)
{
	rct_map_element *mapElement;

	gCommandExpenditureType = RCT_EXPENDITURE_TYPE_LANDSCAPING;
	gCommandPosition.x = x + 16;
	gCommandPosition.y = y + 16;
	gCommandPosition.z = z * 8;

	if (!(flags & GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED) && game_is_paused() && !gCheatsBuildInPauseMode) {
		gGameCommandErrorText = STR_CONSTRUCTION_NOT_POSSIBLE_WHILE_GAME_IS_PAUSED;
		return MONEY32_UNDEFINED;
	}

	if (flags & GAME_COMMAND_FLAG_APPLY)
		footpath_interrupt_peeps(x, y, z * 8);

	gFootpathPrice = 0;
	gFootpathGroundFlags = 0;

	if (!map_is_location_owned(x, y, z * 8) && !gCheatsSandboxMode) {
		return MONEY32_UNDEFINED;
	}

	if (!((gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) || gCheatsSandboxMode) && !map_is_location_owned(x, y, z * 8))
		return MONEY32_UNDEFINED;

	if (z < 2) {
		gGameCommandErrorText = STR_TOO_LOW;
		return MONEY32_UNDEFINED;
	}

	if (z > 248) {
		gGameCommandErrorText = STR_TOO_HIGH;
		return MONEY32_UNDEFINED;
	}

	if (flags & GAME_COMMAND_FLAG_APPLY) {
		if (!(flags & (GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_GHOST))) {
			footpath_remove_litter(x, y, z * 8);
		}
	}

	gFootpathPrice += 120;
	uint8 bl = 15;
	int zHigh = z + 4;
	if (slope & 4) {
		bl = byte_98D7EC[slope & 3];
		zHigh += 2;
	}

	if (!gCheatsDisableClearanceChecks && !map_can_construct_with_clear_at(x, y, z, zHigh, &map_place_non_scenery_clear_func, bl, flags, &gFootpathPrice))
		return MONEY32_UNDEFINED;

	gFootpathGroundFlags = gMapGroundFlags;
	if (!gCheatsDisableClearanceChecks && (gMapGroundFlags & ELEMENT_IS_UNDERWATER)) {
		gGameCommandErrorText = STR_CANT_BUILD_THIS_UNDERWATER;
		return MONEY32_UNDEFINED;
	}

	mapElement = map_get_surface_element_at((x / 32), (y / 32));

	int supportHeight = z - mapElement->base_height;
	gFootpathPrice += supportHeight < 0 ? MONEY(20, 00) : (supportHeight / 2) * MONEY(5, 00);

	if (flags & GAME_COMMAND_FLAG_APPLY) {
		if (gGameCommandNestLevel == 1 && !(flags & GAME_COMMAND_FLAG_GHOST)) {
			rct_xyz16 coord;
			coord.x = x + 16;
			coord.y = y + 16;
			coord.z = map_element_height(coord.x, coord.y);
			network_set_player_last_action_coord(network_get_player_index(game_command_playerid), coord);
		}

		mapElement = map_element_insert(x / 32, y / 32, z, 0x0F);
		assert(mapElement != NULL);
		mapElement->type = MAP_ELEMENT_TYPE_PATH;
		mapElement->clearance_height = z + 4 + (slope & 4 ? 2 : 0);
		mapElement->properties.path.type = (type << 4) | (slope & 7);
		mapElement->type |= type >> 7;
		mapElement->properties.path.additions = 0;
		mapElement->properties.path.addition_status = 255;
		mapElement->properties.path.edges = edges & 0xF;
		mapElement->flags &= ~MAP_ELEMENT_FLAG_BROKEN;
		if (flags & (1 << 6))
			mapElement->flags |= MAP_ELEMENT_FLAG_GHOST;

		map_invalidate_tile_full(x, y);
	}
	return gParkFlags & PARK_FLAGS_NO_MONEY ? 0 : gFootpathPrice;
}

/**
 *
 *  rct2: 0x006A68AE
 */
void game_command_place_footpath_from_track(int *eax, int *ebx, int *ecx, int *edx, int *esi, int *edi, int *ebp)
{
	*ebx = footpath_place_from_track(
		(*edx >> 8) & 0xFF,
		*eax & 0xFFFF,
		*ecx & 0xFFFF,
		*edx & 0xFF,
		((*ebx >> 13) & 0x3) | ((*ebx >> 10) & 0x4),
		(*ebx >> 8) & 0xF,
		*ebx & 0xFF
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

	cost = footpath_place(type, x, y, z, slope, GAME_COMMAND_FLAG_GHOST | GAME_COMMAND_FLAG_5 | GAME_COMMAND_FLAG_4 | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_APPLY);
	if (cost != MONEY32_UNDEFINED) {
		gFootpathProvisionalPosition.x = x;
		gFootpathProvisionalPosition.y = y;
		gFootpathProvisionalPosition.z = z & 0xFF;
		gFootpathProvisionalFlags |= PROVISIONAL_PATH_FLAG_1;

		if (gFootpathGroundFlags & ELEMENT_IS_UNDERGROUND) {
			viewport_set_visibility(1);
		} else {
			viewport_set_visibility(3);
		}
	}

	return cost;
}

/**
 *
 *  rct2: 0x006A77FF
 */
void footpath_provisional_remove()
{
	if (gFootpathProvisionalFlags & PROVISIONAL_PATH_FLAG_1) {
		gFootpathProvisionalFlags &= ~PROVISIONAL_PATH_FLAG_1;

		footpath_remove(
			gFootpathProvisionalPosition.x,
			gFootpathProvisionalPosition.y,
			gFootpathProvisionalPosition.z,
			GAME_COMMAND_FLAG_APPLY | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_5
		);
	}
}

/**
 *
 *  rct2: 0x006A7831
 */
void footpath_provisional_update()
{
	if (gFootpathProvisionalFlags & PROVISIONAL_PATH_FLAG_SHOW_ARROW) {
		gFootpathProvisionalFlags &= ~PROVISIONAL_PATH_FLAG_SHOW_ARROW;

		gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_ARROW;
		map_invalidate_tile_full(
			gFootpathConstructFromPosition.x,
			gFootpathConstructFromPosition.y
		);
	}
	footpath_provisional_remove();
}

/**
 * Determines the location of the footpath at which we point with the cursor. If no footpath is underneath the cursor,
 * then return the location of the ground tile. Besides the location it also computes the direction of the yellow arrow
 * when we are going to build a footpath bridge/tunnel.
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
	int z = 0, interactionType;
	rct_map_element *myMapElement;
	rct_viewport *viewport;
	rct_xy16 position = { 0 };

	get_map_coordinates_from_pos(screenX, screenY, VIEWPORT_INTERACTION_MASK_FOOTPATH, &position.x, &position.y, &interactionType, &myMapElement, &viewport);
	if (interactionType != VIEWPORT_INTERACTION_ITEM_FOOTPATH || !(viewport->flags & (VIEWPORT_FLAG_UNDERGROUND_INSIDE | VIEWPORT_FLAG_HIDE_BASE | VIEWPORT_FLAG_HIDE_VERTICAL))) {
		get_map_coordinates_from_pos(screenX, screenY, VIEWPORT_INTERACTION_MASK_FOOTPATH & VIEWPORT_INTERACTION_MASK_TERRAIN, &position.x, &position.y, &interactionType, &myMapElement, &viewport);
		if (interactionType == VIEWPORT_INTERACTION_ITEM_NONE) {
			if (x != NULL) *x = (sint16)0x8000;
			return;
		}
	}

	rct_xy16 minPosition = position;
	rct_xy16 maxPosition = { position.x + 31, position.y + 31 };

	position.x += 16;
	position.y += 16;

	if (interactionType == VIEWPORT_INTERACTION_ITEM_FOOTPATH) {
		z = myMapElement->base_height * 8;
		if (myMapElement->properties.path.type & (1 << 2)) {
			z += 8;
		}
	}

	rct_xy16 start_vp_pos = screen_coord_to_viewport_coord(viewport, screenX, screenY);

	for (int i = 0; i < 5; i++) {
		if (interactionType != VIEWPORT_INTERACTION_ITEM_FOOTPATH) {
			z = map_element_height(position.x, position.y);
		}
		position = viewport_coord_to_map_coord(start_vp_pos.x, start_vp_pos.y, z);
		position.x = clamp(minPosition.x, position.x, maxPosition.x);
		position.y = clamp(minPosition.y, position.y, maxPosition.y);
	}

	// Determine to which edge the cursor is closest
	uint32 myDirection;
	int mod_x = position.x & 0x1F, mod_y = position.y & 0x1F;
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

	if (x != NULL) *x = position.x & ~0x1F;
	if (y != NULL) *y = position.y & ~0x1F;
	if (direction != NULL) *direction = myDirection;
	if (mapElement != NULL) *mapElement = myMapElement;
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
	uint16 spriteIndex = sprite_get_first_in_quadrant(x, y);
	while (spriteIndex != SPRITE_INDEX_NULL) {
		rct_litter *sprite = &get_sprite(spriteIndex)->litter;
		uint16 nextSpriteIndex = sprite->next_in_quadrant;
		if (sprite->linked_list_type_offset == SPRITE_LIST_LITTER * 2) {
			int distanceZ = abs(sprite->z - z);
			if (distanceZ <= 32) {
				invalidate_sprite_0((rct_sprite*)sprite);
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
	uint16 spriteIndex = sprite_get_first_in_quadrant(x, y);
	while (spriteIndex != SPRITE_INDEX_NULL) {
		rct_peep *peep = &get_sprite(spriteIndex)->peep;
		uint16 nextSpriteIndex = peep->next_in_quadrant;
		if (peep->linked_list_type_offset == SPRITE_LIST_PEEP * 2) {
			if (peep->state == PEEP_STATE_SITTING || peep->state == PEEP_STATE_WATCHING) {
				if (peep->z == z) {
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

/**
 *
 *  rct2: 0x006E59DC
 */
bool fence_in_the_way(int x, int y, int z0, int z1, int direction)
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

static bool map_is_edge(int x, int y)
{
	return (
		x < 32 ||
		y < 32 ||
		x >= gMapSizeUnits ||
		y >= gMapSizeUnits
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
		// First check link to previous tile
		mapElement[3] = footpath_connect_corners_get_neighbour(x, y, z, (1 << (direction ^ 2)));
		if (mapElement[3] == NULL)
			continue;
		// Second check link to initial tile
		mapElement[3] = footpath_connect_corners_get_neighbour(x, y, z, (1 << ((direction + 1) & 3)));
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

		direction = initialDirection;
		mapElement[0]->properties.path.edges |= (1 << (direction + 4));
		map_invalidate_element(x, y, mapElement[0]);
	}
}

typedef struct rct_neighbour {
	uint8 order;
	uint8 direction;
	uint8 ride_index;
	uint8 entrance_index;
} rct_neighbour;

typedef struct rct_neighbour_list {
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
		if (da < db) return -1;
		else if (da > db) return 1;
		else return 0;
	}
}

static void neighbour_list_init(rct_neighbour_list *neighbourList)
{
	neighbourList->count = 0;
}

static void neighbour_list_push(rct_neighbour_list *neighbourList, int order, int direction, uint8 rideIndex, uint8 entrance_index)
{
	neighbourList->items[neighbourList->count].order = order;
	neighbourList->items[neighbourList->count].direction = direction;
	neighbourList->items[neighbourList->count].ride_index = rideIndex;
	neighbourList->items[neighbourList->count].entrance_index = entrance_index;
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

static void neighbour_list_remove(rct_neighbour_list *neighbourList, int index)
{
	int itemsRemaining = neighbourList->count - index - 1;
	if (itemsRemaining > 0) {
		memmove(&neighbourList->items[index], &neighbourList->items[index + 1], sizeof(rct_neighbour) * itemsRemaining);
	}
	neighbourList->count--;
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
	if ((action < 0) && fence_in_the_way(x, y, mapElement->base_height, mapElement->clearance_height, direction))
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

	if (footpath_element_is_sloped(mapElement)) return false;

	uint8 c = connected_path_count[mapElement->properties.path.edges & 0x0F];
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

static bool footpath_is_queue_connected_to_path(int x, int y, rct_map_element *mapElement, int direction)
{
	if (!footpath_element_is_queue(mapElement)) return false;
	if (!(mapElement->properties.path.edges & (1 << direction))) return false;

	x += TileDirectionDelta[direction].x;
	y += TileDirectionDelta[direction].y;
	mapElement = map_get_path_element_at(x / 32, y / 32, mapElement->base_height);
	if (mapElement == NULL) return false;
	if (footpath_element_is_queue(mapElement)) return false;
	if (mapElement->properties.path.edges & ((1 << direction) ^ 2)) return true;
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
	if (((gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) || gCheatsSandboxMode) && map_is_edge(x, y)) {
		if (query) {
			neighbour_list_push(neighbourList, 7, direction, 255, 255);
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
					if (!footpath_element_is_sloped(mapElement) || footpath_element_get_slope_direction(mapElement) != (direction ^ 2)) {
						return;
					}
					goto loc_6A6F1F;
				}
				break;
			case MAP_ELEMENT_TYPE_TRACK:
				if (z == mapElement->base_height) {
					rct_ride *ride = get_ride(mapElement->properties.track.ride_index);
					if (!ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_FLAT_RIDE)) {
						continue;
					}

					const uint8 trackType = mapElement->properties.track.type;
					const uint8 trackSequence = mapElement->properties.track.sequence & 0x0F;
					if (!(FlatRideTrackSequenceProperties[trackType][trackSequence] & TRACK_SEQUENCE_FLAG_CONNECTS_TO_PATH)) {
						return;
					}
					uint16 dx = ((direction - mapElement->type) & 3) ^ 2;
					if (!(FlatRideTrackSequenceProperties[trackType][trackSequence] & (1 << dx))) {
						return;
					}
					if (query) {
						neighbour_list_push(neighbourList, 1, direction, mapElement->properties.track.ride_index, 255);
					}
					goto loc_6A6FD2;
				}
				break;
			case MAP_ELEMENT_TYPE_ENTRANCE:
				if (z == mapElement->base_height) {
					if (entrance_has_direction(mapElement, ((direction - mapElement->type) & 3) ^ 2)) {
						if (query) {
							neighbour_list_push(neighbourList, 8, direction, mapElement->properties.entrance.ride_index,  mapElement->properties.entrance.index);
						} else {
							if (mapElement->properties.entrance.type != ENTRANCE_TYPE_PARK_ENTRANCE) {
								footpath_queue_chain_push(mapElement->properties.entrance.ride_index);
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
			if (fence_in_the_way(x, y, mapElement->base_height, mapElement->clearance_height, direction ^ 2)) {
				return;
			}
			if (footpath_element_is_queue(mapElement)) {
				if (connected_path_count[mapElement->properties.path.edges & 0x0F] < 2) {
					neighbour_list_push(neighbourList, 4, direction, mapElement->properties.path.ride_index, mapElement->properties.entrance.index);
				} else {
					if (map_element_get_type(initialMapElement) == MAP_ELEMENT_TYPE_PATH &&
						footpath_element_is_queue(initialMapElement)) {
						if (footpath_disconnect_queue_from_path(x, y, mapElement, 0)) {
							neighbour_list_push(neighbourList, 3, direction, mapElement->properties.path.ride_index, mapElement->properties.entrance.index);
						}
					}
				}
			} else {
				neighbour_list_push(neighbourList, 2, direction, 255, 255);
			}
		} else {
			footpath_disconnect_queue_from_path(x, y, mapElement, 1 + ((flags >> 6) & 1));
			mapElement->properties.path.edges |= (1 << (direction ^ 2));
			if (footpath_element_is_queue(mapElement)) {
				footpath_queue_chain_push(mapElement->properties.path.ride_index);
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
	if (query && fence_in_the_way(x, y, mapElement->base_height, mapElement->clearance_height, direction))
		return;

	if (map_element_get_type(mapElement) == MAP_ELEMENT_TYPE_ENTRANCE) {
		if (!entrance_has_direction(mapElement, (direction - mapElement->type) & 3)) {
			return;
		}
	}

	if (map_element_get_type(mapElement) == MAP_ELEMENT_TYPE_TRACK) {
		rct_ride *ride = get_ride(mapElement->properties.track.ride_index);
		if (!ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_FLAT_RIDE)) {
			return;
		}
		const uint8 trackType = mapElement->properties.track.type;
		const uint8 trackSequence = mapElement->properties.track.sequence & 0x0F;
		if (!(FlatRideTrackSequenceProperties[trackType][trackSequence] & TRACK_SEQUENCE_FLAG_CONNECTS_TO_PATH)) {
			return;
		}
		uint16 dx = (direction - mapElement->type) & 3;
		if (!(FlatRideTrackSequenceProperties[trackType][trackSequence] & (1 << dx))) {
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

	sub_6A759F();

	neighbour_list_init(&neighbourList);

	sub_6A7642(x, y, mapElement);
	for (int direction = 0; direction < 4; direction++) {
		loc_6A6C85(x, y, direction, mapElement, flags, true, &neighbourList);
	}

	neighbour_list_sort(&neighbourList);

	if (map_element_get_type(mapElement) == MAP_ELEMENT_TYPE_PATH && footpath_element_is_queue(mapElement)) {
		int rideIndex = -1;
		uint8 entranceIndex = 255;
		for (int i = 0; i < neighbourList.count; i++) {
			if (neighbourList.items[i].ride_index != 255) {
				if (rideIndex == -1) {
					rideIndex = neighbourList.items[i].ride_index;
					entranceIndex = neighbourList.items[i].entrance_index;
				} else if (rideIndex != neighbourList.items[i].ride_index) {
					neighbour_list_remove(&neighbourList, i);
				} else if (rideIndex == neighbourList.items[i].ride_index
					&& entranceIndex != neighbourList.items[i].entrance_index
					&&  neighbourList.items[i].entrance_index != 255) {
					neighbour_list_remove(&neighbourList, i);
				}
			}
		}

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
	int lastPathX = x, lastPathY = y, lastPathDirection = direction;

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
			// Fix #2051: Stop queue paths that are already connected to two other tiles
			//            from connecting to the tile we are coming from.
			int edges = mapElement->properties.path.edges;
			int numEdges = bitcount(edges);
			if (numEdges >= 2) {
				int requiredEdgeMask = 1 << (direction ^ 2);
				if (!(edges & requiredEdgeMask)) {
					break;
				}
			}

			mapElement->properties.path.type &= ~(1 << 3);
			mapElement->properties.path.edges |= (1 << (direction ^ 2));
			mapElement->properties.path.ride_index = rideIndex;
			mapElement->properties.path.additions &= 0x8F;
			mapElement->properties.path.additions |= (entranceIndex & 7) << 4;
			
			map_invalidate_element(x, y, mapElement);

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

void footpath_queue_chain_reset()
{
	_footpathQueueChainNext = _footpathQueueChain;
}

/**
 *
 *  rct2: 0x006A76E9
 */
void footpath_queue_chain_push(uint8 rideIndex)
{
	if (rideIndex != 255) {
		uint8 * lastSlot = _footpathQueueChain + countof(_footpathQueueChain) - 1;
		if (_footpathQueueChainNext <= lastSlot) {
			*_footpathQueueChainNext++ = rideIndex;
		}
	}
}

/**
 *
 *  rct2: 0x006A759F
 */
void sub_6A759F()
{
	for (uint8 *queueChainPtr = _footpathQueueChain; queueChainPtr < _footpathQueueChainNext; queueChainPtr++) {
		uint8 rideIndex = *queueChainPtr;
		rct_ride *ride = get_ride(rideIndex);
		if (ride->type == RIDE_TYPE_NULL) {
			continue;
		}

		for (int i = 0; i < 4; i++) {
			if (ride->entrances[i] == 0xFFFF) {
				continue;
			}

			uint8 x = ride->entrances[i] & 0xFF;
			uint8 y = ride->entrances[i] >> 8;
			uint8 z = ride->station_heights[i];

			rct_map_element *mapElement = map_get_first_element_at(x, y);
			do {
				if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_ENTRANCE) continue;
				if (mapElement->base_height != z) continue;
				if (mapElement->properties.entrance.type != ENTRANCE_TYPE_RIDE_ENTRANCE) continue;

				uint8 direction = (mapElement->type & 3) ^ 2;
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
	map_buy_land_rights(x, y, x, y, 6, 1);
}

static bool get_next_direction(int edges, int *direction)
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
 * @param flags (1 << 0): Ignore queues
 *              (1 << 5): Unown
 *              (1 << 7): Ignore no entry signs
 */
static int footpath_is_connected_to_map_edge_recurse(
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
	if (x >= gMapSizeUnits || y >= gMapSizeUnits)
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

		if (!(flags & (1 << 0))) {
			if (footpath_element_is_queue(mapElement)) {
				continue;
			}
		}

		if (flags & (1 << 5)) {
			footpath_unown(x, y, mapElement);
		}
		edges = mapElement->properties.path.edges & 0x0F;
		direction ^= 2;
		if (!(flags & (1 << 7))) {
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
	flags |= (1 << 0);
	return footpath_is_connected_to_map_edge_recurse(x, y, z, direction, flags, 0, 0, 16);
}

bool footpath_element_is_sloped(rct_map_element *mapElement)
{
	return mapElement->properties.path.type & 4;
}

uint8 footpath_element_get_slope_direction(rct_map_element *mapElement)
{
	return mapElement->properties.path.type & 3;
}

bool footpath_element_is_queue(rct_map_element *mapElement)
{
	return mapElement->type & 1;
}

bool footpath_element_is_wide(rct_map_element *mapElement)
{
	return mapElement->type & 2;
}

bool footpath_element_has_path_scenery(rct_map_element *mapElement)
{
	return (mapElement->properties.path.additions & 0xF) > 0;
}

uint8 footpath_element_get_path_scenery(rct_map_element *mapElement)
{
	return mapElement->properties.path.additions & 0xF;
}

void footpath_element_set_path_scenery(rct_map_element *mapElement, uint8 pathSceneryType)
{
	mapElement->properties.path.additions = (mapElement->properties.path.additions & 0xF0) | pathSceneryType;
}

uint8 footpath_element_get_path_scenery_index(rct_map_element *mapElement)
{
	return footpath_element_get_path_scenery(mapElement) - 1;
}

bool footpath_element_path_scenery_is_ghost(rct_map_element *mapElement)
{
	return (mapElement->properties.path.additions & 0x80) == 0x80;
}

void footpath_scenery_set_is_ghost(rct_map_element *mapElement, bool isGhost)
{
	// Remove ghost flag
	mapElement->properties.path.additions &= ~0x80;
	// Set flag if it should be a ghost
	if (isGhost)
		mapElement->properties.path.additions |= 0x80;
}

uint8 footpath_element_get_type(rct_map_element *mapElement)
{
	return mapElement->properties.path.type >> 4;
}

/**
*
*  rct2: 0x006A8B12
*  clears the wide footpath flag for all footpaths
*  at location
*/
static void footpath_clear_wide(int x, int y)
{
	rct_map_element *mapElement = map_get_first_element_at(x / 32, y / 32);
	do {
		if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_PATH)
			continue;
		mapElement->type &= ~0x2;
	} while (!map_element_is_last_for_tile(mapElement++));
}

/**
*
*  rct2: 0x006A8ACF
*  returns footpath element if it can be made wide
*  returns NULL if it can not be made wide
*/
static rct_map_element* footpath_can_be_wide(int x, int y, uint8 height)
{
	rct_map_element *mapElement = map_get_first_element_at(x / 32, y / 32);
	do {
		if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_PATH)
			continue;
		if (height != mapElement->base_height)
			continue;
		if (footpath_element_is_queue(mapElement))
			continue;
		if (footpath_element_is_sloped(mapElement))
			continue;
		return mapElement;
	} while (!map_element_is_last_for_tile(mapElement++));

	return NULL;
}


/**
*
*  rct2: 0x006A87BB
*/
void footpath_update_path_wide_flags(int x, int y)
{
	if (x < 0x20)
		return;
	if (y < 0x20)
		return;
	if (x > 0x1FDF)
		return;
	if (y > 0x1FDF)
		return;

	footpath_clear_wide(x, y);
	x += 0x20;
	footpath_clear_wide(x, y);
	y += 0x20;
	footpath_clear_wide(x, y);
	x -= 0x20;
	footpath_clear_wide(x, y);
	y -= 0x20;

	rct_map_element *mapElement = map_get_first_element_at(x / 32, y / 32);
	do {
		if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_PATH)
			continue;

		if (footpath_element_is_queue(mapElement))
			continue;

		if (footpath_element_is_sloped(mapElement))
			continue;

		if ((mapElement->properties.path.edges & 0xF) == 0)
			continue;

		uint8 height = mapElement->base_height;

		// pathList is a list of elements, set by sub_6A8ACF adjacent to x,y
		// Spanned from 0x00F3EFA8 to 0x00F3EFC7 (8 elements) in the original
		rct_map_element *pathList[8];

		x -= 0x20;
		y -= 0x20;
		pathList[0] = footpath_can_be_wide(x, y, height);
		y += 0x20;
		pathList[1] = footpath_can_be_wide(x, y, height);
		y += 0x20;
		pathList[2] = footpath_can_be_wide(x, y, height);
		x += 0x20;
		pathList[3] = footpath_can_be_wide(x, y, height);
		x += 0x20;
		pathList[4] = footpath_can_be_wide(x, y, height);
		y -= 0x20;
		pathList[5] = footpath_can_be_wide(x, y, height);
		y -= 0x20;
		pathList[6] = footpath_can_be_wide(x, y, height);
		x -= 0x20;
		pathList[7] = footpath_can_be_wide(x, y, height);
		y += 0x20;

		uint8 F3EFA5 = 0;
		if (mapElement->properties.path.edges & 8) {
			F3EFA5 |= 0x80;
			if (pathList[7] != NULL) {
				if (footpath_element_is_wide(pathList[7])) {
					F3EFA5 &= ~0x80;
				}
			}
		}

		if (mapElement->properties.path.edges & 1) {
			F3EFA5 |= 0x2;
			if (pathList[1] != NULL) {
				if (footpath_element_is_wide(pathList[1])) {
					F3EFA5 &= ~0x2;
				}
			}
		}

		if (mapElement->properties.path.edges & 2) {
			F3EFA5 |= 0x8;
			if (pathList[3] != NULL) {
				if (footpath_element_is_wide(pathList[3])) {
					F3EFA5 &= ~0x8;
				}
			}
		}

		if (mapElement->properties.path.edges & 4) {
			F3EFA5 |= 0x20;
			if (pathList[5] != NULL) {
				if (footpath_element_is_wide(pathList[5])) {
					F3EFA5 &= ~0x20;
				}
			}
		}

		if ((F3EFA5 & 0x80) && (pathList[7] != NULL) && !(footpath_element_is_wide(pathList[7]))) {
			if ((F3EFA5 & 2) &&
				(pathList[0] != NULL) && (!footpath_element_is_wide(pathList[0])) &&
				((pathList[0]->properties.path.edges & 6) == 6) && // N E
				(pathList[1] != NULL) && (!footpath_element_is_wide(pathList[1]))) {
				F3EFA5 |= 0x1;
			}

			if ((F3EFA5 & 0x20) &&
				(pathList[6] != NULL) && (!footpath_element_is_wide(pathList[6])) &&
				((pathList[6]->properties.path.edges & 3) == 3) && // N W
				(pathList[5] != NULL) && (!footpath_element_is_wide(pathList[5]))) {
				F3EFA5 |= 0x40;
			}
		}


		if ((F3EFA5 & 0x8) && (pathList[3] != NULL) && !(pathList[3]->type & 2)) {
			if ((F3EFA5 & 2) &&
				(pathList[2] != NULL) && (!footpath_element_is_wide(pathList[2])) &&
				((pathList[2]->properties.path.edges & 0xC) == 0xC) &&
				(pathList[1] != NULL) && (!footpath_element_is_wide(pathList[1]))) {
				F3EFA5 |= 0x4;
			}

			if ((F3EFA5 & 0x20) &&
				(pathList[4] != NULL) && (!footpath_element_is_wide(pathList[4])) &&
				((pathList[4]->properties.path.edges & 9) == 9) &&
				(pathList[5] != NULL) && (!footpath_element_is_wide(pathList[5]))) {
				F3EFA5 |= 0x10;
			}
		}

		if ((F3EFA5 & 0x80) && (F3EFA5 & (0x40 | 0x1)))
			F3EFA5 &= ~0x80;

		if ((F3EFA5 & 0x2) && (F3EFA5 & (0x4 | 0x1)))
			F3EFA5 &= ~0x2;

		if ((F3EFA5 & 0x8) && (F3EFA5 & (0x10 | 0x4)))
			F3EFA5 &= ~0x8;

		if ((F3EFA5 & 0x20) && (F3EFA5 & (0x40 | 0x10)))
			F3EFA5 &= ~0x20;

		if (!(F3EFA5 & (0x2 | 0x8 | 0x20 | 0x80))) {
			uint8 e = mapElement->properties.path.edges;
			if ((e != 0xAF) && (e != 0x5F) && (e != 0xEF))
				mapElement->type |= 2;
		}
	} while (!map_element_is_last_for_tile(mapElement++));
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
			footpath_queue_chain_push(mapElement->properties.path.ride_index);
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
			footpath_queue_chain_push(mapElement->properties.entrance.ride_index);
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
		footpath_queue_chain_push(mapElement->properties.path.ride_index);
	}

	d = direction ^ 2;
	mapElement->properties.path.edges &= ~(1 << d);
	d = ((d - 1) & 3) + 4;
	mapElement->properties.path.edges &= ~(1 << d);
	d = (((d - 4) + 1) & 3) + 4;
	mapElement->properties.path.edges &= ~(1 << d);
	map_invalidate_tile(x, y, mapElement->base_height * 8, mapElement->clearance_height * 8);

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
		map_invalidate_tile(x, y, mapElement->base_height * 8, mapElement->clearance_height * 8);
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
		ride = get_ride(rideIndex);
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

rct_footpath_entry *get_footpath_entry(int entryIndex)
{
	return gFootpathEntries[entryIndex];
}
