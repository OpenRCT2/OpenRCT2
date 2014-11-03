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

#ifndef _MAP_H_
#define _MAP_H_

#include "../common.h"

typedef struct {
	uint8 slope; //4
	uint8 terrain; //5
	uint8 grass_length;
	uint8 ownership;
} rct_map_element_surface_properties;

typedef struct {
	uint8 type; //4
	uint8 additions; //5
	uint8 edges; //6
	uint8 addition_status; //7
} rct_map_element_path_properties;

typedef struct {
	uint8 type; //4
	uint8 sequence; //5
	uint8 colour; //6
	uint8 ride_index; //7
} rct_map_element_track_properties;

typedef struct {
	uint8 type; //4
	uint8 age; //5
	uint8 colour; //6
	uint8 unused; //7
} rct_map_element_scenery_properties;

typedef struct {
	uint8 type; //4
	uint8 index; //5
	uint8 path_type; //6
	uint8 ride_index; //7
} rct_map_element_entrance_properties;

typedef struct {
	uint8 slope; //4
	uint8 item[3]; //5
} rct_map_element_fence_properties;

typedef struct {
	uint8 type; //4
	uint8 index; //5
	uint8 colour[2]; //6
} rct_map_element_scenerymultiple_properties;

typedef struct {
	uint8 index; //4
	uint8 position; //5
	uint8 flags; //6
	uint8 unused; //7
} rct_map_element_banner_properties;

typedef union {
	rct_map_element_surface_properties surface;
	rct_map_element_path_properties path;
	rct_map_element_track_properties track;
	rct_map_element_scenery_properties scenery;
	rct_map_element_entrance_properties entrance;
	rct_map_element_fence_properties fence;
	rct_map_element_scenerymultiple_properties scenerymultiple;
	rct_map_element_banner_properties banner;
} rct_map_element_properties;

/**
 * Map element structure
 * size: 0x08
 */
typedef struct {
	uint8 type; //0
	uint8 flags; //1
	uint8 base_height; //2
	uint8 clearance_height; //3
	rct_map_element_properties properties;
} rct_map_element;

enum {
	MAP_ELEMENT_QUADRANT_SW,
	MAP_ELEMENT_QUADRANT_NW,
	MAP_ELEMENT_QUADRANT_NE,
	MAP_ELEMENT_QUADRANT_SE
};

enum {
	MAP_ELEMENT_TYPE_SURFACE = (0 << 2),
	MAP_ELEMENT_TYPE_PATH = (1 << 2),
	MAP_ELEMENT_TYPE_TRACK = (2 << 2),
	MAP_ELEMENT_TYPE_SCENERY = (3 << 2),
	MAP_ELEMENT_TYPE_ENTRANCE = (4 << 2),
	MAP_ELEMENT_TYPE_FENCE = (5 << 2),
	MAP_ELEMENT_TYPE_SCENERY_MULTIPLE = (6 << 2),
	MAP_ELEMENT_TYPE_BANNER = (7 << 2)
};

enum {
	MAP_ELEMENT_DIRECTION_WEST,
	MAP_ELEMENT_DIRECTION_NORTH,
	MAP_ELEMENT_DIRECTION_EAST,
	MAP_ELEMENT_DIRECTION_SOUTH
};

enum {
	MAP_ELEMENT_FLAG_LAST_TILE = (1 << 7)
};

enum {
	TERRAIN_GRASS,
	TERRAIN_SAND,
	TERRAIN_DIRT,
	TERRAIN_ROCK,
	TERRAIN_MARTIAN,
	TERRAIN_CHECKERBOARD,
	TERRAIN_GRASS_CLUMPS,
	TERRAIN_ICE,
	TERRAIN_GRID_RED,
	TERRAIN_GRID_YELLOW,
	TERRAIN_GRID_BLUE,
	TERRAIN_GRID_GREEN,
	TERRAIN_SAND_DARK,
	TERRAIN_SAND_LIGHT,
	TERRAIN_CHECKERBOARD_INVERTED,
	TERRAIN_UNDERGROUND_VIEW,
};

enum {
	TERRAIN_EDGE_ROCK,
	TERRAIN_EDGE_WOOD_RED,
	TERRAIN_EDGE_WOOD_BLACK,
	TERRAIN_EDGE_ICE
};

enum {
	PATH_QUEUE,
	PATH_TARMAC,
	PATH_DIRT,
	PATH_CRAZY,
	PATH_ROAD,
	PATH_TILE
};

enum {
	ENTRANCE_TYPE_RIDE_ENTRANCE,
	ENTRANCE_TYPE_RIDE_EXIT,
	ENTRANCE_TYPE_PARK_ENTRANCE
};

#define MAP_ELEMENT_QUADRANT_MASK 0xC0
#define MAP_ELEMENT_TYPE_MASK 0x3C
#define MAP_ELEMENT_DIRECTION_MASK 0x03

#define MAP_ELEMENT_SLOPE_MASK 0x1F
#define MAP_ELEMENT_WATER_HEIGHT_MASK 0x1F
#define MAP_ELEMENT_SURFACE_TERRAIN_MASK 0xE0

#define MAP_MINIMUM_X_Y -256

#define MAX_MAP_ELEMENTS 196608
#define MAX_TILE_MAP_ELEMENT_POINTERS (256 * 256)

#define TILE_UNDEFINED_MAP_ELEMENT (rct_map_element*)-1


typedef struct {
	uint16 x;
	uint16 y;
	uint8 z;
	uint8 direction;
} rct2_peep_spawn;


void map_init();
void map_update_tile_pointers();
int map_element_get_terrain(rct_map_element *element);
int map_element_get_terrain_edge(rct_map_element *element);
void map_element_set_terrain(rct_map_element *element, int terrain);
void map_element_set_terrain_edge(rct_map_element *element, int terrain);
rct_map_element *map_get_surface_element_at(int x, int y);
int map_element_height(int x, int y);
void sub_68B089();
int map_coord_is_connected(uint16 coordinate, uint8 height, uint8 face_direction);
void map_invalidate_animations();
void sub_6A876D();
int sub_664F72(int x, int y, int z);
int map_is_location_in_park(int x, int y);
void map_invalidate_tile(int x, int y, int zLow, int zHigh);

void fountain_update_all();

#define GET_MAP_ELEMENT(x) (&(RCT2_ADDRESS(RCT2_ADDRESS_MAP_ELEMENTS, rct_map_element)[x]))
#define TILE_MAP_ELEMENT_POINTER(x) (RCT2_ADDRESS(RCT2_ADDRESS_TILE_MAP_ELEMENT_POINTERS, rct_map_element*)[x])

#endif
