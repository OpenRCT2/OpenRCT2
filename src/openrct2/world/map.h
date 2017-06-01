#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
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

#ifndef _MAP_H_
#define _MAP_H_

#include "../common.h"

#pragma pack(push, 1)
typedef struct rct_map_element_surface_properties {
	uint8 slope; //4 0xE0 Edge Style, 0x1F Slope
	uint8 terrain; //5 0xE0 Terrain Style, 0x1F Water height
	uint8 grass_length; //6
	uint8 ownership; //7
} rct_map_element_surface_properties;
assert_struct_size(rct_map_element_surface_properties, 4);

typedef struct rct_map_element_path_properties {
	uint8 type; //4 0xF0 Path type, 0x08 Ride sign, 0x04 Set when path is diagonal, 0x03 Rotation
	uint8 additions; //5
	uint8 edges; //6
	union {
		uint8 addition_status; //7
		uint8 ride_index;
	};
} rct_map_element_path_properties;
assert_struct_size(rct_map_element_path_properties, 4);

typedef struct rct_map_element_track_properties {
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
assert_struct_size(rct_map_element_track_properties, 4);

typedef struct rct_map_element_scenery_properties {
	uint8 type; //4
	uint8 age; //5
	uint8 colour_1; //6
	uint8 colour_2; //7
} rct_map_element_scenery_properties;
assert_struct_size(rct_map_element_scenery_properties, 4);

typedef struct rct_map_element_entrance_properties {
	uint8 type; //4
	uint8 index; //5
	uint8 path_type; //6
	uint8 ride_index; //7
} rct_map_element_entrance_properties;
assert_struct_size(rct_map_element_entrance_properties, 4);

typedef struct rct_map_element_wall_properties {
	uint8 type; //4
	union {
		uint8 colour_3; //5
		uint8 banner_index; //5
	};
	uint8 colour_1; //6 0b_2221_1111 2 = colour_2 (uses flags for rest of colour2), 1 = colour_1
	uint8 animation; //7 0b_dfff_ft00 d = direction, f = frame num, t = across track flag (not used)
} rct_map_element_wall_properties;
assert_struct_size(rct_map_element_wall_properties, 4);

typedef struct rct_map_element_scenerymultiple_properties {
	uint16 type; //4
	uint8 colour[2]; //6
} rct_map_element_scenerymultiple_properties;
assert_struct_size(rct_map_element_scenerymultiple_properties, 4);

typedef struct rct_map_element_banner_properties {
	uint8 index; //4
	uint8 position; //5
	uint8 flags; //6
	uint8 unused; //7
} rct_map_element_banner_properties;
assert_struct_size(rct_map_element_banner_properties, 4);

typedef union {
	rct_map_element_surface_properties surface;
	rct_map_element_path_properties path;
	rct_map_element_track_properties track;
	rct_map_element_scenery_properties scenery;
	rct_map_element_entrance_properties entrance;
	rct_map_element_wall_properties wall;
	rct_map_element_scenerymultiple_properties scenerymultiple;
	rct_map_element_banner_properties banner;
} rct_map_element_properties;
assert_struct_size(rct_map_element_properties, 4);

/**
 * Map element structure
 * size: 0x08
 */
typedef struct rct_map_element {
	uint8 type; //0
	uint8 flags; //1
	uint8 base_height; //2
	uint8 clearance_height; //3
	rct_map_element_properties properties;
} rct_map_element;
assert_struct_size(rct_map_element, 8);
#pragma pack(pop)

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
	MAP_ELEMENT_TYPE_WALL = (5 << 2),
	MAP_ELEMENT_TYPE_SCENERY_MULTIPLE = (6 << 2),
	MAP_ELEMENT_TYPE_BANNER = (7 << 2),
	// The corrupt element type is used for skipping drawing other following
	// elements on a given tile.
	MAP_ELEMENT_TYPE_CORRUPT = (8 << 2),
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
	MAP_ELEMENT_FLAG_BLOCK_BRAKE_CLOSED = (1 << 5),
	MAP_ELEMENT_FLAG_CANNOT_REMOVE_TRACK = (1 << 6),
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
	WALL_ANIMATION_FLAG_ACROSS_TRACK = (1 << 2),
	// 3 - 6 animation frame number
	WALL_ANIMATION_FLAG_DIRECTION_BACKWARD = (1 << 7),
	WALL_ANIMATION_FLAG_ALL_FLAGS = WALL_ANIMATION_FLAG_ACROSS_TRACK | WALL_ANIMATION_FLAG_DIRECTION_BACKWARD
};

enum {
	ENTRANCE_TYPE_RIDE_ENTRANCE,
	ENTRANCE_TYPE_RIDE_EXIT,
	ENTRANCE_TYPE_PARK_ENTRANCE
};

enum {
	ELEMENT_IS_ABOVE_GROUND = 1 << 0,
	ELEMENT_IS_UNDERGROUND = 1 << 1,
	ELEMENT_IS_UNDERWATER = 1 << 2,
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
#define MAP_LOCATION_NULL ((sint16)(uint16)0x8000)

#define MAX_MAP_ELEMENTS 196096
#define MAX_TILE_MAP_ELEMENT_POINTERS (256 * 256)
#define MAX_PEEP_SPAWNS 2
#define PEEP_SPAWN_UNDEFINED 0xFFFF

#define MAP_ELEMENT_LARGE_TYPE_MASK 0x3FF

#define TILE_UNDEFINED_MAP_ELEMENT (rct_map_element*)-1

#pragma pack(push, 1)
typedef struct rct_xy8 {
	union {
		struct {
			uint8 x, y;
		};
		uint16 xy;
	};
} rct_xy8;
assert_struct_size(rct_xy8, 2);

typedef struct rct_xyz8 {
	uint8 x, y, z;
} rct_xyz8;
assert_struct_size(rct_xyz8, 3);

typedef struct rct_xyzd8 {
	uint8 x, y, z, direction;
} rct_xyzd8;
assert_struct_size(rct_xyzd8, 4);

typedef struct rct_xy16 {
	sint16 x, y;
} rct_xy16;
assert_struct_size(rct_xy16, 4);

typedef struct rct_xyz16 {
	sint16 x, y, z;
} rct_xyz16;
assert_struct_size(rct_xyz16, 6);

typedef struct rct_xyzd16 {
	sint16 x, y, z;
	uint8 direction;
} rct_xyzd16;
assert_struct_size(rct_xyzd16, 7);

typedef struct rct_xy32 {
	sint32 x, y;
} rct_xy32;

typedef struct rct_xyz32 {
	sint32 x, y, z;
} rct_xyz32;

typedef struct rct_xy_element {
	sint32 x, y;
	rct_map_element *element;
} rct_xy_element;
#ifdef PLATFORM_32BIT
assert_struct_size(rct_xy_element, 12);
#endif

typedef struct rct2_peep_spawn {
	uint16 x;
	uint16 y;
	uint8 z;
	uint8 direction;
} rct2_peep_spawn;
assert_struct_size(rct2_peep_spawn, 6);
#pragma pack(pop)

enum {
	MAP_SELECT_FLAG_ENABLE				= 1 << 0,
	MAP_SELECT_FLAG_ENABLE_CONSTRUCT	= 1 << 1,
	MAP_SELECT_FLAG_ENABLE_ARROW		= 1 << 2,
	MAP_SELECT_FLAG_GREEN				= 1 << 3,
};

enum {
	MAP_SELECT_TYPE_CORNER_0,
	MAP_SELECT_TYPE_CORNER_1,
	MAP_SELECT_TYPE_CORNER_2,
	MAP_SELECT_TYPE_CORNER_3,
	MAP_SELECT_TYPE_FULL,
	MAP_SELECT_TYPE_FULL_WATER,
	MAP_SELECT_TYPE_QUARTER_0,
	MAP_SELECT_TYPE_QUARTER_1,
	MAP_SELECT_TYPE_QUARTER_2,
	MAP_SELECT_TYPE_QUARTER_3,
	MAP_SELECT_TYPE_EDGE_0,
	MAP_SELECT_TYPE_EDGE_1,
	MAP_SELECT_TYPE_EDGE_2,
	MAP_SELECT_TYPE_EDGE_3,
};

extern const rct_xy16 TileDirectionDelta[];
extern const money32 TerrainPricing[];

extern uint16 gWidePathTileLoopX;
extern uint16 gWidePathTileLoopY;
extern uint16 gGrassSceneryTileLoopPosition;

extern sint16 gMapSizeUnits;
extern sint16 gMapSizeMinus2;
extern sint16 gMapSize;
extern sint16 gMapSizeMaxXY;
extern sint16 gMapBaseZ;

extern uint16		gMapSelectFlags;
extern uint16		gMapSelectType;
extern rct_xy16		gMapSelectPositionA;
extern rct_xy16		gMapSelectPositionB;
extern rct_xyz16	gMapSelectArrowPosition;
extern uint8		gMapSelectArrowDirection;

extern uint8 gMapGroundFlags;

#ifdef NO_RCT2
extern rct_map_element gMapElements[];
extern rct_map_element *gMapElementTilePointers[];
#else
extern rct_map_element *gMapElements;
extern rct_map_element **gMapElementTilePointers;
#endif

extern rct_xy16 gMapSelectionTiles[300];
extern rct2_peep_spawn gPeepSpawns[MAX_PEEP_SPAWNS];

extern rct_map_element *gNextFreeMapElement;
extern uint32 gNextFreeMapElementPointerIndex;

// Used in the land tool window to enable mountain tool / land smoothing
extern bool gLandMountainMode;
// Used in the land tool window to allow dragging and changing land styles
extern bool gLandPaintMode;
// Used in the land rights tool window to either buy land rights or construction rights
extern bool LandRightsMode;
// Used in the clear scenery tool
extern bool gClearSmallScenery;
extern bool gClearLargeScenery;
extern bool gClearFootpath;

extern uint16 gLandToolSize;
extern money32 gLandToolRaiseCost;
extern money32 gLandToolLowerCost;
extern uint8 gLandToolTerrainSurface;
extern uint8 gLandToolTerrainEdge;
extern money32 gWaterToolRaiseCost;
extern money32 gWaterToolLowerCost;
extern money32 gLandRightsCost;

extern uint16 gLandRemainingOwnershipSales;
extern uint16 gLandRemainingConstructionSales;

extern rct_xyz16 gCommandPosition;

extern uint8 gUnk9E2E28;

void map_init(sint32 size);
void map_count_remaining_land_rights();
void map_strip_ghost_flag_from_elements();
void map_update_tile_pointers();
rct_map_element *map_get_first_element_at(sint32 x, sint32 y);
rct_map_element *map_get_nth_element_at(sint32 x, sint32 y, sint32 n);
void map_set_tile_elements(sint32 x, sint32 y, rct_map_element *elements);
sint32 map_element_is_last_for_tile(const rct_map_element *element);
bool map_element_is_ghost(const rct_map_element *element);
uint8 map_element_get_scenery_quadrant(const rct_map_element *element);
sint32 map_element_get_type(const rct_map_element *element);
sint32 map_element_get_direction(const rct_map_element *element);
sint32 map_element_get_terrain(const rct_map_element *element);
sint32 map_element_get_terrain_edge(const rct_map_element *element);
void map_element_set_terrain(rct_map_element *element, sint32 terrain);
void map_element_set_terrain_edge(rct_map_element *element, sint32 terrain);
sint32 map_height_from_slope(sint32 x, sint32 y, sint32 slope);
rct_map_element* map_get_banner_element_at(sint32 x, sint32 y, sint32 z, uint8 direction);
rct_map_element *map_get_surface_element_at(sint32 x, sint32 y);
rct_map_element* map_get_path_element_at(sint32 x, sint32 y, sint32 z);
rct_map_element *map_get_wall_element_at(sint32 x, sint32 y, sint32 z, sint32 direction);
rct_map_element *map_get_small_scenery_element_at(sint32 x, sint32 y, sint32 z, sint32 type, uint8 quadrant);
rct_map_element *map_get_park_entrance_element_at(sint32 x, sint32 y, sint32 z, bool ghost);
sint32 map_element_height(sint32 x, sint32 y);
void sub_68B089();
sint32 map_coord_is_connected(sint32 x, sint32 y, sint32 z, uint8 faceDirection);
void map_remove_provisional_elements();
void map_restore_provisional_elements();
void map_update_path_wide_flags();
bool map_is_location_valid(sint32 x, sint32 y);
bool map_can_build_at(sint32 x, sint32 y, sint32 z);
bool map_is_location_owned(sint32 x, sint32 y, sint32 z);
bool map_is_location_in_park(sint32 x, sint32 y);
bool map_is_location_owned_or_has_rights(sint32 x, sint32 y);
bool map_surface_is_blocked(sint16 x, sint16 y);
sint32 map_get_station(rct_map_element *mapElement);
void map_element_remove(rct_map_element *mapElement);
void map_remove_all_rides();
void map_invalidate_map_selection_tiles();
void map_invalidate_selection_rect();
void map_reorganise_elements();
bool map_check_free_elements_and_reorganise(sint32 num_elements);
rct_map_element *map_element_insert(sint32 x, sint32 y, sint32 z, sint32 flags);
bool map_element_check_address(const rct_map_element * const element);

typedef sint32 (CLEAR_FUNC)(rct_map_element** map_element, sint32 x, sint32 y, uint8 flags, money32* price);
sint32 map_place_non_scenery_clear_func(rct_map_element** map_element, sint32 x, sint32 y, uint8 flags, money32* price);
sint32 map_can_construct_with_clear_at(sint32 x, sint32 y, sint32 zLow, sint32 zHigh, CLEAR_FUNC *clearFunc, uint8 bl, uint8 flags, money32 *price);
sint32 map_can_construct_at(sint32 x, sint32 y, sint32 zLow, sint32 zHigh, uint8 bl);
void rotate_map_coordinates(sint16 *x, sint16 *y, sint32 rotation);
rct_xy16 coordinate_3d_to_2d(const rct_xyz16* coordinate_3d, sint32 rotation);
money32 map_clear_scenery(sint32 x0, sint32 y0, sint32 x1, sint32 y1, sint32 clear, sint32 flags);
money32 lower_water(sint16 x0, sint16 y0, sint16 x1, sint16 y1, uint8 flags);
money32 raise_water(sint16 x0, sint16 y0, sint16 x1, sint16 y1, uint8 flags);
money32 wall_place(sint32 type, sint32 x, sint32 y, sint32 z, sint32 edge, sint32 primaryColour, sint32 secondaryColour, sint32 tertiaryColour, sint32 flags);

void game_command_set_land_height(sint32 *eax, sint32 *ebx, sint32 *ecx, sint32 *edx, sint32 *esi, sint32 *edi, sint32 *ebp);
void game_command_set_land_ownership(sint32 *eax, sint32 *ebx, sint32 *ecx, sint32 *edx, sint32 *esi, sint32 *edi, sint32 *ebp);
void game_command_remove_scenery(sint32* eax, sint32* ebx, sint32* ecx, sint32* edx, sint32* esi, sint32* edi, sint32* ebp);
void game_command_remove_large_scenery(sint32* eax, sint32* ebx, sint32* ecx, sint32* edx, sint32* esi, sint32* edi, sint32* ebp);
void game_command_remove_banner(sint32* eax, sint32* ebx, sint32* ecx, sint32* edx, sint32* esi, sint32* edi, sint32* ebp);
void game_command_set_scenery_colour(sint32* eax, sint32* ebx, sint32* ecx, sint32* edx, sint32* esi, sint32* edi, sint32* ebp);
void game_command_set_wall_colour(sint32* eax, sint32* ebx, sint32* ecx, sint32* edx, sint32* esi, sint32* edi, sint32* ebp);
void game_command_set_large_scenery_colour(sint32* eax, sint32* ebx, sint32* ecx, sint32* edx, sint32* esi, sint32* edi, sint32* ebp);
void game_command_set_banner_colour(sint32* eax, sint32* ebx, sint32* ecx, sint32* edx, sint32* esi, sint32* edi, sint32* ebp);
void game_command_clear_scenery(sint32* eax, sint32* ebx, sint32* ecx, sint32* edx, sint32* esi, sint32* edi, sint32* ebp);
void game_command_change_surface_style(sint32* eax, sint32* ebx, sint32* ecx, sint32* edx, sint32* esi, sint32* edi, sint32* ebp);
void game_command_raise_land(sint32* eax, sint32* ebx, sint32* ecx, sint32* edx, sint32* esi, sint32* edi, sint32* ebp);
void game_command_lower_land(sint32* eax, sint32* ebx, sint32* ecx, sint32* edx, sint32* esi, sint32* edi, sint32* ebp);
void game_command_smooth_land(sint32* eax, sint32* ebx, sint32* ecx, sint32* edx, sint32* esi, sint32* edi, sint32* ebp);
void game_command_raise_water(sint32* eax, sint32* ebx, sint32* ecx, sint32* edx, sint32* esi, sint32* edi, sint32* ebp);
void game_command_lower_water(sint32* eax, sint32* ebx, sint32* ecx, sint32* edx, sint32* esi, sint32* edi, sint32* ebp);
void game_command_set_water_height(sint32* eax, sint32* ebx, sint32* ecx, sint32* edx, sint32* esi, sint32* edi, sint32* ebp);
void game_command_remove_wall(sint32* eax, sint32* ebx, sint32* ecx, sint32* edx, sint32* esi, sint32* edi, sint32* ebp);
void game_command_place_banner(sint32* eax, sint32* ebx, sint32* ecx, sint32* edx, sint32* esi, sint32* edi, sint32* ebp);
void game_command_place_scenery(sint32* eax, sint32* ebx, sint32* ecx, sint32* edx, sint32* esi, sint32* edi, sint32* ebp);
void game_command_place_wall(sint32* eax, sint32* ebx, sint32* ecx, sint32* edx, sint32* esi, sint32* edi, sint32* ebp);
void game_command_place_large_scenery(sint32* eax, sint32* ebx, sint32* ecx, sint32* edx, sint32* esi, sint32* edi, sint32* ebp);
void game_command_place_park_entrance(sint32* eax, sint32* ebx, sint32* ecx, sint32* edx, sint32* esi, sint32* edi, sint32* ebp);
void game_command_set_banner_name(sint32* eax, sint32* ebx, sint32* ecx, sint32* edx, sint32* esi, sint32* edi, sint32* ebp);
void game_command_set_sign_name(sint32* eax, sint32* ebx, sint32* ecx, sint32* edx, sint32* esi, sint32* edi, sint32* ebp);
void game_command_set_banner_style(sint32* eax, sint32* ebx, sint32* ecx, sint32* edx, sint32* esi, sint32* edi, sint32* ebp);
void game_command_set_sign_style(sint32* eax, sint32* ebx, sint32* ecx, sint32* edx, sint32* esi, sint32* edi, sint32* ebp);
void game_command_modify_tile(sint32* eax, sint32* ebx, sint32* ecx, sint32* edx, sint32* esi, sint32* edi, sint32* ebp);

typedef struct map_element_iterator {
	sint32 x;
	sint32 y;
	rct_map_element *element;
} map_element_iterator;
#ifdef PLATFORM_32BIT
assert_struct_size(map_element_iterator, 12);
#endif

void map_element_iterator_begin(map_element_iterator *it);
sint32 map_element_iterator_next(map_element_iterator *it);
void map_element_iterator_restart_for_tile(map_element_iterator *it);

void wall_remove_intersecting_walls(sint32 x, sint32 y, sint32 z0, sint32 z1, sint32 direction);
void map_update_tiles();
sint32 map_get_highest_z(sint32 tileX, sint32 tileY);

sint32 map_element_get_banner_index(rct_map_element *mapElement);
void map_element_remove_banner_entry(rct_map_element *mapElement);

bool map_element_is_underground(rct_map_element *mapElement);

void map_remove_out_of_range_elements();
void map_extend_boundary_surface();

bool sign_set_colour(sint32 x, sint32 y, sint32 z, sint32 direction, sint32 sequence, uint8 mainColour, uint8 textColour);
void wall_remove_at(sint32 x, sint32 y, sint32 z0, sint32 z1);
void wall_remove_at_z(sint32 x, sint32 y, sint32 z);

void map_invalidate_tile(sint32 x, sint32 y, sint32 z0, sint32 z1);
void map_invalidate_tile_zoom1(sint32 x, sint32 y, sint32 z0, sint32 z1);
void map_invalidate_tile_zoom0(sint32 x, sint32 y, sint32 z0, sint32 z1);
void map_invalidate_tile_full(sint32 x, sint32 y);
void map_invalidate_element(sint32 x, sint32 y, rct_map_element *mapElement);

sint32 map_get_tile_side(sint32 mapX, sint32 mapY);
sint32 map_get_tile_quadrant(sint32 mapX, sint32 mapY);

void map_clear_all_elements();

rct_map_element *map_get_large_scenery_segment(sint32 x, sint32 y, sint32 z, sint32 direction, sint32 sequence);
bool map_large_scenery_get_origin(
	sint32 x, sint32 y, sint32 z, sint32 direction, sint32 sequence,
	sint32 *outX, sint32 *outY, sint32 *outZ, rct_map_element** outElement
);

void map_offset_with_rotation(sint16 *x, sint16 *y, sint16 offsetX, sint16 offsetY, uint8 rotation);
rct_xy32 translate_3d_to_2d_with_z(sint32 rotation, rct_xyz32 pos);

rct_map_element *map_get_track_element_at(sint32 x, sint32 y, sint32 z);
rct_map_element *map_get_track_element_at_of_type(sint32 x, sint32 y, sint32 z, sint32 trackType);
rct_map_element *map_get_track_element_at_of_type_seq(sint32 x, sint32 y, sint32 z, sint32 trackType, sint32 sequence);
rct_map_element *map_get_track_element_at_of_type_from_ride(sint32 x, sint32 y, sint32 z, sint32 trackType, sint32 rideIndex);
rct_map_element *map_get_track_element_at_from_ride(sint32 x, sint32 y, sint32 z, sint32 rideIndex);
rct_map_element *map_get_track_element_at_with_direction_from_ride(sint32 x, sint32 y, sint32 z, sint32 direction, sint32 rideIndex);

bool map_is_location_at_edge(sint32 x, sint32 y);
void map_obstruction_set_error_text(rct_map_element *mapElement);
uint8 wall_element_get_animation_frame(rct_map_element *fenceElement);
void wall_element_set_animation_frame(rct_map_element * wallElement, uint8 frameNum);
uint8 wall_element_get_secondary_colour(rct_map_element * wallElement);
void wall_element_set_secondary_colour(rct_map_element * wallElement, uint8 secondaryColour);

uint32 map_get_available_peep_spawn_index_list(uint32* peepSpawnIndexList);

#endif
