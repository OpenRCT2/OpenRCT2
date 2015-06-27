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
	uint8 slope; //4 0xE0 Edge Style, 0x1F Slope
	uint8 terrain; //5 0xE0 Terrain Style, 0x1F Water height
	uint8 grass_length; //6
	uint8 ownership; //7
} rct_map_element_surface_properties;

typedef struct {
	uint8 type; //4
	uint8 additions; //5
	uint8 edges; //6
	uint8 addition_status; //7
} rct_map_element_path_properties;

typedef struct {
	uint8 type; //4
	union{
		struct{
			uint8 sequence; //5
			uint8 colour; //6
		};
		uint16 maze_entry; // 5
	};
	uint8 ride_index; //7
} rct_map_element_track_properties;

typedef struct {
	uint8 type; //4
	uint8 age; //5
	uint8 colour_1; //6
	uint8 colour_2; //7
} rct_map_element_scenery_properties;

typedef struct {
	uint8 type; //4
	uint8 index; //5
	uint8 path_type; //6
	uint8 ride_index; //7
} rct_map_element_entrance_properties;

typedef struct {
	uint8 type; //4
	uint8 item[3]; //5
} rct_map_element_fence_properties;

typedef struct {
	uint16 type; //4
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
	MAP_ELEMENT_TYPE_FLAG_HIGHLIGHT = (1 << 6)
};

enum {
	MAP_ELEMENT_DIRECTION_WEST,
	MAP_ELEMENT_DIRECTION_NORTH,
	MAP_ELEMENT_DIRECTION_EAST,
	MAP_ELEMENT_DIRECTION_SOUTH
};

enum {
	MAP_ELEMENT_FLAG_GHOST = (1 << 4),
	MAP_ELEMENT_FLAG_BROKEN = (1 << 5),
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
	GRASS_LENGTH_MOWED,
	GRASS_LENGTH_CLEAR_0,
	GRASS_LENGTH_CLEAR_1,
	GRASS_LENGTH_CLEAR_2,
	GRASS_LENGTH_CLUMPS_0,
	GRASS_LENGTH_CLUMPS_1,
	GRASS_LENGTH_CLUMPS_2
};

enum {
	OWNERSHIP_CONSTRUCTION_RIGHTS_OWNED = (1 << 4),
	OWNERSHIP_OWNED = (1 << 5),
	OWNERSHIP_CONSTRUCTION_RIGHTS_AVAILABLE = (1 << 6),
	OWNERSHIP_AVAILABLE = (1 << 7)
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
	PATH_FLAG_QUEUE_BANNER = 1 << 3
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
#define MAP_ELEMENT_SLOPE_EDGE_STYLE_MASK 0xE0

// Terrain
#define MAP_ELEMENT_WATER_HEIGHT_MASK 0x1F
#define MAP_ELEMENT_SURFACE_TERRAIN_MASK 0xE0

#define MAP_MINIMUM_X_Y -256

#define MAX_MAP_ELEMENTS 196608
#define MAX_TILE_MAP_ELEMENT_POINTERS (256 * 256)

#define MAP_ELEMENT_LARGE_TYPE_MASK 0x3FF

#define TILE_UNDEFINED_MAP_ELEMENT (rct_map_element*)-1

typedef struct {
	uint8 x, y;
} rct_xy8;

typedef struct {
	sint16 x, y;
} rct_xy16;

typedef struct {
	sint16 x, y, z;
} rct_xyz16;

typedef struct {
	int x, y;
	rct_map_element *element;
} rct_xy_element;

typedef struct {
	uint16 x;
	uint16 y;
	uint8 z;
	uint8 direction;
} rct2_peep_spawn;

extern const rct_xy16 TileDirectionDelta[];
extern rct_xy16 *gMapSelectionTiles;
// Used in the land tool window to allow dragging and changing land styles
extern bool LandPaintMode;
// Used in the land rights tool window to either buy land rights or construction rights
extern bool LandRightsMode;
// Used in the clear scenery tool
extern bool gClearSmallScenery;
extern bool gClearLargeScenery;
extern bool gClearFootpath;

void map_init(int size);
void map_update_tile_pointers();
rct_map_element *map_get_first_element_at(int x, int y);
void map_set_tile_elements(int x, int y, rct_map_element *elements);
int map_element_is_last_for_tile(rct_map_element *element);
int map_element_get_type(rct_map_element *element);
int map_element_get_direction(rct_map_element *element);
int map_element_get_terrain(rct_map_element *element);
int map_element_get_terrain_edge(rct_map_element *element);
void map_element_set_terrain(rct_map_element *element, int terrain);
void map_element_set_terrain_edge(rct_map_element *element, int terrain);
int map_height_from_slope(int x, int y, int slope);
rct_map_element *map_get_surface_element_at(int x, int y);
rct_map_element* map_get_path_element_at(int x, int y, int z);
int map_element_height(int x, int y);
void sub_68B089();
int map_coord_is_connected(int x, int y, int z, uint8 faceDirection);
void sub_6A876D();
int map_is_location_owned(int x, int y, int z);
int map_is_location_in_park(int x, int y);
void map_invalidate_tile(int x, int y, int zLow, int zHigh);
void map_invalidate_tile_full(int x, int y);
int map_get_station(rct_map_element *mapElement);
void map_element_remove(rct_map_element *mapElement);
void sub_6A6AA7(int x, int y, rct_map_element *mapElement);
void map_remove_all_rides();
void map_invalidate_map_selection_tiles();
void map_invalidate_selection_rect();
void map_reorganise_elements();
int sub_68B044();
rct_map_element *map_element_insert(int x, int y, int z, int flags);
int map_can_construct_with_clear_at(int x, int y, int zLow, int zHigh, void *clearFunc, uint8 bl);
int map_can_construct_at(int x, int y, int zLow, int zHigh, uint8 bl);
void rotate_map_coordinates(sint16* x, sint16* y, int rotation);
rct_xy16 coordinate_3d_to_2d(const rct_xyz16* coordinate_3d, int rotation);
money32 map_clear_scenery(int x0, int y0, int x1, int y1, int flags);
money32 lower_water(sint16 x0, sint16 y0, sint16 x1, sint16 y1, uint8 flags);
money32 raise_water(sint16 x0, sint16 y0, sint16 x1, sint16 y1, uint8 flags);

void game_command_remove_scenery(int* eax, int* ebx, int* ecx, int* edx, int* esi, int* edi, int* ebp);
void game_command_remove_large_scenery(int* eax, int* ebx, int* ecx, int* edx, int* esi, int* edi, int* ebp);
void game_command_remove_banner(int* eax, int* ebx, int* ecx, int* edx, int* esi, int* edi, int* ebp);
void game_command_set_scenery_colour(int* eax, int* ebx, int* ecx, int* edx, int* esi, int* edi, int* ebp);
void game_command_set_fence_colour(int* eax, int* ebx, int* ecx, int* edx, int* esi, int* edi, int* ebp);
void game_command_set_large_scenery_colour(int* eax, int* ebx, int* ecx, int* edx, int* esi, int* edi, int* ebp);
void game_command_set_banner_colour(int* eax, int* ebx, int* ecx, int* edx, int* esi, int* edi, int* ebp);
void game_command_clear_scenery(int* eax, int* ebx, int* ecx, int* edx, int* esi, int* edi, int* ebp);
void game_command_change_surface_style(int* eax, int* ebx, int* ecx, int* edx, int* esi, int* edi, int* ebp);
void game_command_raise_land(int* eax, int* ebx, int* ecx, int* edx, int* esi, int* edi, int* ebp);
void game_command_lower_land(int* eax, int* ebx, int* ecx, int* edx, int* esi, int* edi, int* ebp);
void game_command_raise_water(int* eax, int* ebx, int* ecx, int* edx, int* esi, int* edi, int* ebp);
void game_command_lower_water(int* eax, int* ebx, int* ecx, int* edx, int* esi, int* edi, int* ebp);
void game_command_remove_fence(int* eax, int* ebx, int* ecx, int* edx, int* esi, int* edi, int* ebp);
void game_command_place_banner(int* eax, int* ebx, int* ecx, int* edx, int* esi, int* edi, int* ebp);
void game_command_place_scenery(int* eax, int* ebx, int* ecx, int* edx, int* esi, int* edi, int* ebp);
void game_command_place_fence(int* eax, int* ebx, int* ecx, int* edx, int* esi, int* edi, int* ebp);
void game_command_place_large_scenery(int* eax, int* ebx, int* ecx, int* edx, int* esi, int* edi, int* ebp);

#define GET_MAP_ELEMENT(x) (&(RCT2_ADDRESS(RCT2_ADDRESS_MAP_ELEMENTS, rct_map_element)[x]))
#define TILE_MAP_ELEMENT_POINTER(x) (RCT2_ADDRESS(RCT2_ADDRESS_TILE_MAP_ELEMENT_POINTERS, rct_map_element*)[x])

typedef struct {
	int x;
	int y;
	rct_map_element *element;
} map_element_iterator;

void map_element_iterator_begin(map_element_iterator *it);
int map_element_iterator_next(map_element_iterator *it);
void map_element_iterator_restart_for_tile(map_element_iterator *it);

void map_remove_intersecting_walls(int x, int y, int z0, int z1, int direction);
void map_update_tiles();
int map_get_highest_z(int tileX, int tileY);

void sub_6A7594();
int map_element_get_banner_index(rct_map_element *mapElement);
void map_element_remove_banner_entry(rct_map_element *mapElement);

#endif
