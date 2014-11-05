/*****************************************************************************
 * Copyright (c) 2014 Ted John, Peter Hill
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
#include "../localisation/date.h"
#include "../localisation/localisation.h"
#include "climate.h"
#include "map.h"
#include "park.h"
#include "scenery.h"

int _sub_6A876D_save_x;
int _sub_6A876D_save_y;

static void tiles_init();
static void sub_6A87BB(int x, int y);

int map_element_get_terrain(rct_map_element *element)
{
	int terrain = (element->properties.surface.terrain >> 5) & 7;
	if (element->type & 1)
		terrain |= (1 << 3);
	return terrain;
}

int map_element_get_terrain_edge(rct_map_element *element)
{
	int terrain_edge = (element->properties.surface.slope >> 5) & 7;
	if (element->type & 128)
		terrain_edge |= (1 << 3);
	return terrain_edge;
}

void map_element_set_terrain(rct_map_element *element, int terrain)
{
	// Bit 3 for terrain is stored in element.type bit 0
	if (terrain & 8)
		element->type |= 1;
	else
		element->type &= ~1;

	// Bits 0, 1, 2 for terrain are stored in element.terrain bit 5, 6, 7
	element->properties.surface.terrain &= ~0xE0;
	element->properties.surface.terrain = (terrain & 7) << 5;
}

void map_element_set_terrain_edge(rct_map_element *element, int terrain)
{
	// Bit 3 for terrain is stored in element.type bit 7
	if (terrain & 8)
		element->type |= 128;
	else
		element->type &= ~128;

	// Bits 0, 1, 2 for terrain are stored in element.slope bit 5, 6, 7
	element->properties.surface.slope &= ~0xE0;
	element->properties.surface.slope |= (terrain & 7) << 5;
}

rct_map_element *map_get_surface_element_at(int x, int y)
{
	// Get first element of the tile
	rct_map_element *mapElement = TILE_MAP_ELEMENT_POINTER(y * 256 + x);

	// Find the first surface element
	while ((mapElement->type & MAP_ELEMENT_TYPE_MASK) != MAP_ELEMENT_TYPE_SURFACE) {
		// Check if last element on tile
		if (mapElement->flags & MAP_ELEMENT_FLAG_LAST_TILE)
			return NULL;

		mapElement++;
	}

	return mapElement;
}

/**
 * 
 *  rct2: 0x0068AB4C
 */
void map_init()
{
	int i;
	rct_map_element *map_element;

	date_reset();
	RCT2_GLOBAL(0x0138B580, sint16) = 0;
	RCT2_GLOBAL(0x010E63B8, sint32) = 0;

	for (i = 0; i < MAX_TILE_MAP_ELEMENT_POINTERS; i++) {
		map_element = GET_MAP_ELEMENT(i);
		map_element->type = (MAP_ELEMENT_TYPE_SURFACE << 2);
		map_element->flags = MAP_ELEMENT_FLAG_LAST_TILE;
		map_element->base_height = 14;
		map_element->clearance_height = 14;
		map_element->properties.surface.slope = 0;
		map_element->properties.surface.grass_length = 1;
		map_element->properties.surface.ownership = 0;

		map_element_set_terrain(map_element, TERRAIN_GRASS);
		map_element_set_terrain_edge(map_element, TERRAIN_EDGE_ROCK);
	}

	RCT2_GLOBAL(0x013B0E70, sint16) = 0;
	_sub_6A876D_save_x = 0;
	_sub_6A876D_save_y = 0;
	RCT2_GLOBAL(0x01358830, sint16) = 4768;
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_MAXIMUM_X_Y, sint16) = 5054;
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_SIZE, sint16) = 150;
	RCT2_GLOBAL(0x01358836, sint16) = 4767;
	RCT2_GLOBAL(0x01359208, sint16) = 7;
	map_update_tile_pointers();
	RCT2_CALLPROC_EBPSAFE(0x0068ADBC);

	climate_reset(CLIMATE_WARM);
}

/**
 * 
 *  rct2: 0x0068AFFD
 */
void map_update_tile_pointers()
{
	int i, x, y, lastTile;

	for (i = 0; i < MAX_TILE_MAP_ELEMENT_POINTERS; i++)
		TILE_MAP_ELEMENT_POINTER(i) = TILE_UNDEFINED_MAP_ELEMENT;

	rct_map_element *mapElement = RCT2_ADDRESS(RCT2_ADDRESS_MAP_ELEMENTS, rct_map_element);
	rct_map_element **tile = RCT2_ADDRESS(RCT2_ADDRESS_TILE_MAP_ELEMENT_POINTERS, rct_map_element*);
	for (y = 0; y < 256; y++) {
		for (x = 0; x < 256; x++) {
			*tile++ = mapElement;
			do {
				lastTile = (mapElement->flags & MAP_ELEMENT_FLAG_LAST_TILE);
				mapElement++;
			} while (!lastTile);
		}
	}

	// Possible next free map element
	RCT2_GLOBAL(0x0140E9A4, rct_map_element*) = mapElement;
}

/**
 * Return the absolute height of an element, given its (x,y) coordinates
 *
 *  ax: x
 *  cx: y
 *  dx: return remember to & with 0xFFFF if you don't want water affecting results
 *  rct2: 0x00662783
 */
int map_element_height(int x, int y)
{
	rct_map_element *mapElement;

	// Off the map
	if (x >= 8192 || y >= 8192)
		return 16;

	// Truncate subtile coordinates
	int x_tile = x & 0xFFFFFFE0;
	int y_tile = y & 0xFFFFFFE0;

	// Get the surface element for the tile
	mapElement = map_get_surface_element_at(x_tile / 32, y_tile / 32);

	uint32 height =
		((mapElement->properties.surface.terrain & MAP_ELEMENT_WATER_HEIGHT_MASK) << 20) |
		(mapElement->base_height << 3);

	uint32 slope = (mapElement->properties.surface.slope & MAP_ELEMENT_SLOPE_MASK);
	uint8 extra_height = (slope & 0x10) >> 4; // 0x10 is the 5th bit - sets slope to double height
	// Remove the extra height bit
	slope &= 0xF;

	sint8 quad, quad_extra; // which quadrant the element is in?
	                        // quad_extra is for extra height tiles

	uint8 xl, yl;	    // coordinates across this tile

	uint8 TILE_SIZE = 31;

	xl = x & 0x1f;
	yl = y & 0x1f;

	// Slope logic:
	// Each of the four bits in slope represents that corner being raised
	// slope == 15 (all four bits) is not used and slope == 0 is flat
	// If the extra_height bit is set, then the slope goes up two z-levels

	// We arbitrarily take the SW corner to be closest to the viewer

	// One corner up
	if ((slope == 1) || (slope == 2) || (slope == 4) || (slope == 8)) {
		switch (slope) {
		case 1:    // NE corner up
			quad = xl + yl - TILE_SIZE;
			break;
		case 2:    // SE corner up
			quad = xl - yl;
			break;
		case 4:   // SW corner up
			quad = TILE_SIZE - yl - xl;
			break;
		case 8:   // NW corner up
			quad = yl - xl;
			break;
		}
		// If the element is in the quadrant with the slope, raise its height
		if (quad > 0) {
			height += quad / 2;
		}
	}

	// One side up
	switch (slope) {
	case 3:   // E side up
		height += xl / 2 + 1;
		break;
	case 6:   // S side up
		height += (TILE_SIZE - yl) / 2;
		break;
	case 9:    // N side up
		height += yl / 2;
		height++;
		break;
	case 12:  // W side up
		height += (TILE_SIZE - xl) / 2;
		break;
	}

	// One corner down
	if ((slope == 7) || (slope == 11) || (slope == 13) || (slope == 14)) {
		switch (slope) {
		case 7:   // NW corner down
			quad_extra = xl + TILE_SIZE - yl;
			quad = xl - yl;
			break;
		case 11:  // SW corner down
			quad_extra = xl + yl;
			quad = xl + yl - TILE_SIZE - 1;
			break;
		case 13:  // SE corner down
			quad_extra = TILE_SIZE - xl + yl;
			quad = yl - xl;
			break;
		case 14:  // NE corner down
			quad_extra = (TILE_SIZE - xl) + (TILE_SIZE - yl);
			quad = TILE_SIZE - yl - xl - 1;
			break;
		}

		if (extra_height) {
			height += quad_extra / 2;
			height++;
			return height;
		}
		// This tile is essentially at the next height level
		height += 0x10;
		// so we move *down* the slope
		if (quad < 0) {
			height += quad / 2;
		}
	}

	// Valleys
	if ((slope == 5) || (slope == 10)) {
		switch (slope) {
		case 5:  // NW-SE valley
			if (xl + yl <= TILE_SIZE + 1) {
				return height;
			}
			quad = TILE_SIZE - xl - yl;
			break;
		case 10: // NE-SW valley
			quad = xl - yl;
			break;
		}
		if (quad > 0) {
			height += quad / 2;
		}
	}

	return height;
}

/**
 *
 *  rct2: 0x0068B089
 */
void sub_68B089()
{
	int i;
	rct_map_element *mapElementFirst, *mapElement;

	if (RCT2_GLOBAL(0x009DEA6F, uint8) & 1)
		return;

	i = RCT2_GLOBAL(0x0010E63B8, uint32);
	do {
		i++;
		if (i >= MAX_TILE_MAP_ELEMENT_POINTERS)
			i = 0;
	} while (TILE_MAP_ELEMENT_POINTER(i) == TILE_UNDEFINED_MAP_ELEMENT);
	RCT2_GLOBAL(0x0010E63B8, uint32) = i;

	mapElementFirst = mapElement = TILE_MAP_ELEMENT_POINTER(i);
	do {
		mapElement--;
		if (mapElement < (rct_map_element*)RCT2_ADDRESS_MAP_ELEMENTS)
			break;
	} while (mapElement->base_height == 255);
	mapElement++;

	if (mapElement == mapElementFirst)
		return;

	// 
	TILE_MAP_ELEMENT_POINTER(i) = mapElement;
	do {
		*mapElement = *mapElementFirst;
		mapElementFirst->base_height = 255;

		mapElement++;
		mapElementFirst++;
	} while (!((mapElement - 1)->flags & MAP_ELEMENT_FLAG_LAST_TILE));

	// Update next free element?
	mapElement = RCT2_GLOBAL(0x0140E9A4, rct_map_element*);
	do {
		mapElement--;
	} while (mapElement->base_height == 255);
	mapElement++;
	RCT2_GLOBAL(0x0140E9A4, rct_map_element*) = mapElement;
}


/**
 * Checks if the tile at coordinate at height counts as connected.
 * @return 1 if connected, 0 otherwise
 */
int map_coord_is_connected(uint16 tile_idx, uint8 height, uint8 face_direction)
{
    rct_map_element* tile = RCT2_ADDRESS(RCT2_ADDRESS_TILE_MAP_ELEMENT_POINTERS, rct_map_element*)[tile_idx];

    do {
        rct_map_element_path_properties props = tile->properties.path;
        uint8 path_type = props.type >> 2, path_dir = props.type & 3;
        uint8 element_type = tile->type & MAP_ELEMENT_TYPE_MASK;

        if (element_type != PATH_ROAD)
            continue;

        if (path_type & 1) {
			if (path_dir == face_direction) {
				if (height == tile->base_height + 2)
					return 1;
			}
			else if ((path_dir ^ 2) == face_direction && height == tile->base_height) {
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
 *
 *  rct2: 0x0068AFAD
 */
void map_invalidate_animations()
{
	RCT2_CALLPROC_EBPSAFE(0x0068AFAD);
}

/**
 *
 *  rct2: 0x006A876D
 */
void sub_6A876D()
{
	int i, x, y;

	if (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & (SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER))
		return;

	// Presumebly sub_6A87BB is too computationally expensive to call for every
	// tile every update, so word_13CE774 and word_13CE776 store the x and y
	// progress. A maximum of 128 calls is done per update.
	x = _sub_6A876D_save_x;
	y = _sub_6A876D_save_y;
	for (i = 0; i < 128; i++) {
		sub_6A87BB(x, y);

		// Next x, y tile
		x += 32;
		if (x >= 8192) {
			x = 0;
			y += 32;
			if (y >= 8192)
				y = 0;
		}
	}
	_sub_6A876D_save_x = x;
	_sub_6A876D_save_y = y;
}

/**
 *
 *  rct2: 0x006A87BB
 */
static void sub_6A87BB(int x, int y)
{
	RCT2_CALLPROC_X(0x006A87BB, x, 0, y, 0, 0, 0, 0);
}

/* rct2: 0x6A7B84 */
int map_height_from_slope(int x, int y, int slope){
	if (!(slope & 4)) return 0;

	switch (slope & 3){
	case 0:
		return (31 - (x & 31)) / 2;
	case 1:
		return (y & 31) / 2;
	case 2:
		return (x & 31) / 2;
	case 3:
		return (31 - (y & 31)) / 2;
	}
	return 0;
}

/**
 *
 *  rct2: 0x00664F72
 */
int sub_664F72(int x, int y, int z)
{
	rct_map_element *mapElement;

	if (x < (256 * 32) && y < (256 * 32)) {
		mapElement = map_get_surface_element_at(x / 32, y / 32);
		if (mapElement->properties.surface.ownership & 0x20)
			return 1;

		if (mapElement->properties.surface.ownership & 0x10) {
			z /= 8;
			if (z < mapElement->base_height || z - 2 > mapElement->base_height)
				return 1;
		}
	}

	RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TEXT, uint16) = 1729;
	return 0;
}

/**
 *
 *  rct2: 0x00664F2C
 */
int map_is_location_in_park(int x, int y)
{
	rct_map_element *mapElement;

	if (x < (256 * 32) && y < (256 * 32)) {
		mapElement = map_get_surface_element_at(x / 32, y / 32);
		if (mapElement->properties.surface.ownership & 0x20)
		return 1;
	}

	RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TEXT, uint16) = 1729;
	return 0;
}

/**
 *
 *  rct2: 0x006ECB60
 * NOTE: x, y and z are in pixels, not tile units
 */
void map_invalidate_tile(int x, int y, int zLow, int zHigh)
{
	RCT2_CALLPROC_X(0x006ECB60, x, 0, y, 0, zHigh, zLow, 0);
}

/**
 *
 *  rct2: 0x006E0E01
 */
money32 map_try_clear_scenery(int x, int y, rct_map_element *mapElement, int flags)
{
	money32 cost;
	rct_scenery_entry *entry;

	entry = g_smallSceneryEntries[mapElement->properties.scenery.type];
	cost = entry->small_scenery.removal_price * 10;

	RCT2_GLOBAL(0x0141F56C, uint8) = 12;
	RCT2_GLOBAL(0x009DEA5E, uint32) = x * 32 + 16;
	RCT2_GLOBAL(0x009DEA60, uint32) = y * 32 + 16;
	RCT2_GLOBAL(0x009DEA62, uint32) = mapElement->base_height * 8;

	x *= 32;
	y *= 32;

	if (!(flags & 0x40) && RCT2_GLOBAL(0x009DEA6E, uint8) != 0) {
		RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TEXT, uint16) = STR_CONSTRUCTION_NOT_POSSIBLE_WHILE_GAME_IS_PAUSED;
		return MONEY32_UNDEFINED;
	}

	if (!(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_SCENARIO_EDITOR) && !(flags & 0x40)) {
		// Check if allowed to remove item
		if (RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & PARK_FLAGS_FORBID_TREE_REMOVAL) {
			if (entry->small_scenery.height > 64) {
				RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TEXT, uint16) = STR_FORBIDDEN_BY_THE_LOCAL_AUTHORITY;
				return MONEY32_UNDEFINED;
			}
		}

		// Check if the land is owned
		if (!(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_SCENARIO_EDITOR))
			if (!sub_664F72(x, y, RCT2_GLOBAL(0x009DEA62, uint32)))
				return MONEY32_UNDEFINED;
	}

	if ((flags & 0x40) && !(mapElement->flags & 0x10))
		return 0;

	// Remove element
	if (flags & 1) {
		map_invalidate_tile_full(x, y);
		RCT2_CALLPROC_X(0x0068B280, 0, 0, 0, 0, (int)mapElement, 0, 0);
	}
	return RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & PARK_FLAGS_NO_MONEY ? 0 : cost;
}

/**
 *
 *  rct2: 0x006E5597
 */
money32 sub_6E5597(int x, int y, int dl, int dh, int bl)
{
	int eax, ebx, ecx, edx, esi, edi, ebp;
	eax = x * 32;
	ecx = y * 32;
	ebx = bl & 0xFF;
	edx = ((dh & 0xFF) << 8) | (dl & 0xFF);
	RCT2_CALLFUNC_X(0x006E5597, &eax, &ebx, &ecx, &edx, &esi, &edi, &ebp);
	return ebx;
}

money32 sub_6A67C0(int x, int y, int z, int flags)
{
	int eax, ebx, ecx, edx, esi, edi, ebp;
	eax = x * 32;
	ecx = y * 32;
	ebx = flags & 0xFF;
	edx = z & 0xFF;
	RCT2_CALLFUNC_X(0x006A67C0, &eax, &ebx, &ecx, &edx, &esi, &edi, &ebp);
	return ebx;
}

// This will cause clear scenery to remove paths
// This should be a flag for the game command which can be set via a checkbox on the clear scenery window.
// #define CLEAR_SCENERY_REMOVES_PATHS

/**
 *
 *  rct2: 0x0068DFE4
 */
money32 map_clear_scenery_from_tile(int x, int y, int flags)
{
	int type;
	money32 cost, totalCost;
	rct_map_element *mapElement;

	totalCost = 0;

restart_from_beginning:
	mapElement = TILE_MAP_ELEMENT_POINTER(y * 256 + x);
	do {
		type = mapElement->type & MAP_ELEMENT_TYPE_MASK;
		switch (type) {
		case MAP_ELEMENT_TYPE_PATH:
#ifdef CLEAR_SCENERY_REMOVES_PATHS
			cost = sub_6A67C0(x, y, mapElement->base_height, flags);
			if (cost == MONEY32_UNDEFINED)
				return MONEY32_UNDEFINED;

			totalCost += cost;
			if (flags & 1)
				goto restart_from_beginning;
#endif
			break;
		case MAP_ELEMENT_TYPE_SCENERY:
			cost = map_try_clear_scenery(x, y, mapElement, flags);
			if (cost == MONEY32_UNDEFINED)
				return MONEY32_UNDEFINED;

			totalCost += cost;
			if (flags & 1)
				goto restart_from_beginning;

			break;
		case MAP_ELEMENT_TYPE_FENCE:
			cost = sub_6E5597(x, y, mapElement->type & 3, mapElement->base_height, flags);
			if (cost == MONEY32_UNDEFINED)
				return MONEY32_UNDEFINED;

			totalCost += cost;
			if (flags & 1)
				goto restart_from_beginning;

			break;
		}
	} while (!((mapElement++)->flags & MAP_ELEMENT_FLAG_LAST_TILE));

	return totalCost;
}

money32 map_clear_scenery(int x0, int y0, int x1, int y1, int flags)
{
	int x, y, z;
	money32 totalCost, cost;

	RCT2_GLOBAL(0x0141F56C, uint8) = 12;

	x = (x0 + x1) / 2 + 16;
	y = (y0 + y1) / 2 + 16;
	z = map_element_height(x, y);
	RCT2_GLOBAL(0x009DEA5E, uint16) = x;
	RCT2_GLOBAL(0x009DEA60, uint16) = y;
	RCT2_GLOBAL(0x009DEA62, uint16) = z;

	x0 = clamp(0, x0, 255);
	y0 = clamp(0, y0, 255);
	x1 = clamp(0, x1, 255);
	y1 = clamp(0, y1, 255);

	totalCost = 0;
	for (y = y0; y <= y1; y++) {
		for (x = x0; x <= x1; x++) {
			cost = map_clear_scenery_from_tile(x, y, flags);
			if (cost == MONEY32_UNDEFINED)
				return MONEY32_UNDEFINED;

			totalCost += cost;
		}
	}

	return totalCost;
}

/**
 *
 *  rct2: 0x0068DF91
 */
void game_command_clear_scenery(int* eax, int* ebx, int* ecx, int* edx, int* esi, int* edi, int* ebp)
{
	*ebx = map_clear_scenery(
		(*eax & 0xFFFF) / 32,
		(*ecx & 0xFFFF) / 32,
		(*edi & 0xFFFF) / 32,
		(*ebp & 0xFFFF) / 32,
		*ebx & 0xFF
	);
}

/**
 *
 *  rct2: 0x006EC6D7
 */
void map_invalidate_tile_full(int x, int y)
{
	RCT2_CALLPROC_X(0x006EC6D7, x, 0, y, 0, 0, 0, 0);
}