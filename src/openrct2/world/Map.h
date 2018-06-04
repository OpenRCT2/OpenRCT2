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

#include <initializer_list>
#include "../common.h"
#include "Location.hpp"
#include "TileElement.h"

#define MINIMUM_LAND_HEIGHT 2
#define MAXIMUM_LAND_HEIGHT 142

#define MINIMUM_MAP_SIZE_TECHNICAL 15
#define MAXIMUM_MAP_SIZE_TECHNICAL 256
#define MINIMUM_MAP_SIZE_PRACTICAL (MINIMUM_MAP_SIZE_TECHNICAL-2)
#define MAXIMUM_MAP_SIZE_PRACTICAL (MAXIMUM_MAP_SIZE_TECHNICAL-2)

#define MAP_MINIMUM_X_Y -MAXIMUM_MAP_SIZE_TECHNICAL

#define MAX_TILE_ELEMENTS 196096 // 0x30000
#define MAX_TILE_TILE_ELEMENT_POINTERS (MAXIMUM_MAP_SIZE_TECHNICAL * MAXIMUM_MAP_SIZE_TECHNICAL)
#define MAX_PEEP_SPAWNS 2
#define PEEP_SPAWN_UNDEFINED 0xFFFF

#define TILE_ELEMENT_LARGE_TYPE_MASK 0x3FF

#define TILE_UNDEFINED_TILE_ELEMENT NULL

typedef CoordsXYZD PeepSpawn;

struct CoordsXYE
{
    sint32 x, y;
    rct_tile_element * element;
};

enum {
    MAP_SELECT_FLAG_ENABLE              = 1 << 0,
    MAP_SELECT_FLAG_ENABLE_CONSTRUCT    = 1 << 1,
    MAP_SELECT_FLAG_ENABLE_ARROW        = 1 << 2,
    MAP_SELECT_FLAG_GREEN               = 1 << 3,
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

// Used when calling map_can_construct_with_clear_at();
// This assumes that the caller has already done the check on the element it wants to place,
// as this function can only check the element the player wants to build through.
enum
{
    CREATE_CROSSING_MODE_NONE,
    CREATE_CROSSING_MODE_TRACK_OVER_PATH,
    CREATE_CROSSING_MODE_PATH_OVER_TRACK,
};

extern const CoordsXY CoordsDirectionDelta[];
extern const TileCoordsXY TileDirectionDelta[];
extern const money32 TerrainPricing[];

extern uint16 gWidePathTileLoopX;
extern uint16 gWidePathTileLoopY;
extern uint16 gGrassSceneryTileLoopPosition;

extern sint16 gMapSizeUnits;
extern sint16 gMapSizeMinus2;
extern sint16 gMapSize;
extern sint16 gMapSizeMaxXY;
extern sint16 gMapBaseZ;

extern uint16       gMapSelectFlags;
extern uint16       gMapSelectType;
extern LocationXY16     gMapSelectPositionA;
extern LocationXY16     gMapSelectPositionB;
extern LocationXYZ16    gMapSelectArrowPosition;
extern uint8        gMapSelectArrowDirection;

extern uint8 gMapGroundFlags;

extern rct_tile_element gTileElements[MAX_TILE_TILE_ELEMENT_POINTERS * 3];
extern rct_tile_element *gTileElementTilePointers[MAX_TILE_TILE_ELEMENT_POINTERS];

extern LocationXY16 gMapSelectionTiles[300];
extern PeepSpawn gPeepSpawns[MAX_PEEP_SPAWNS];

extern rct_tile_element *gNextFreeTileElement;
extern uint32 gNextFreeTileElementPointerIndex;

// Used in the land tool window to enable mountain tool / land smoothing
extern bool gLandMountainMode;
// Used in the land tool window to allow dragging and changing land styles
extern bool gLandPaintMode;
// Used in the clear scenery tool
extern bool gClearSmallScenery;
extern bool gClearLargeScenery;
extern bool gClearFootpath;

extern uint16 gLandRemainingOwnershipSales;
extern uint16 gLandRemainingConstructionSales;

extern LocationXYZ16 gCommandPosition;

extern bool gMapLandRightsUpdateSuccess;

void map_init(sint32 size);
void map_count_remaining_land_rights();
void map_strip_ghost_flag_from_elements();
void map_update_tile_pointers();
rct_tile_element *map_get_first_element_at(sint32 x, sint32 y);
rct_tile_element *map_get_nth_element_at(sint32 x, sint32 y, sint32 n);
void map_set_tile_elements(sint32 x, sint32 y, rct_tile_element *elements);
sint32 map_height_from_slope(sint32 x, sint32 y, sint32 slope);
rct_tile_element* map_get_banner_element_at(sint32 x, sint32 y, sint32 z, uint8 direction);
rct_tile_element *map_get_surface_element_at(sint32 x, sint32 y);
rct_tile_element * map_get_surface_element_at(CoordsXY coords);
rct_tile_element* map_get_path_element_at(sint32 x, sint32 y, sint32 z);
rct_tile_element *map_get_wall_element_at(sint32 x, sint32 y, sint32 z, sint32 direction);
rct_tile_element *map_get_small_scenery_element_at(sint32 x, sint32 y, sint32 z, sint32 type, uint8 quadrant);
rct_tile_element *map_get_park_entrance_element_at(sint32 x, sint32 y, sint32 z, bool ghost);
rct_tile_element * map_get_ride_entrance_element_at(sint32 x, sint32 y, sint32 z, bool ghost);
rct_tile_element * map_get_ride_exit_element_at(sint32 x, sint32 y, sint32 z, bool ghost);
sint32 tile_element_height(sint32 x, sint32 y);
void sub_68B089();
bool map_coord_is_connected(sint32 x, sint32 y, sint32 z, uint8 faceDirection);
void map_remove_provisional_elements();
void map_restore_provisional_elements();
void map_update_path_wide_flags();
bool map_is_location_valid(CoordsXY coords);
bool map_is_edge(CoordsXY coords);
bool map_can_build_at(sint32 x, sint32 y, sint32 z);
bool map_is_location_owned(sint32 x, sint32 y, sint32 z);
bool map_is_location_in_park(CoordsXY coords);
bool map_is_location_owned_or_has_rights(sint32 x, sint32 y);
bool map_surface_is_blocked(sint16 x, sint16 y);
void tile_element_remove(rct_tile_element *tileElement);
void map_remove_all_rides();
void map_invalidate_map_selection_tiles();
void map_get_bounding_box(sint32 ax, sint32 ay, sint32 bx, sint32 by, sint32 *left, sint32 *top, sint32 *right, sint32 *bottom);
void map_invalidate_selection_rect();
void map_reorganise_elements();
bool map_check_free_elements_and_reorganise(sint32 num_elements);
rct_tile_element *tile_element_insert(sint32 x, sint32 y, sint32 z, sint32 flags);

using CLEAR_FUNC = sint32(*)(rct_tile_element** tile_element, sint32 x, sint32 y, uint8 flags, money32* price);

sint32 map_place_non_scenery_clear_func(rct_tile_element** tile_element, sint32 x, sint32 y, uint8 flags, money32* price);
sint32 map_place_scenery_clear_func(rct_tile_element** tile_element, sint32 x, sint32 y, uint8 flags, money32* price);
bool map_can_construct_with_clear_at(sint32 x, sint32 y, sint32 zLow, sint32 zHigh, CLEAR_FUNC clearFunc, uint8 bl, uint8 flags, money32 *price, uint8 crossingMode);
sint32 map_can_construct_at(sint32 x, sint32 y, sint32 zLow, sint32 zHigh, uint8 bl);
void rotate_map_coordinates(sint16 *x, sint16 *y, sint32 rotation);
LocationXY16 coordinate_3d_to_2d(const LocationXYZ16* coordinate_3d, sint32 rotation);
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
void game_command_place_banner(sint32* eax, sint32* ebx, sint32* ecx, sint32* edx, sint32* esi, sint32* edi, sint32* ebp);
void game_command_place_scenery(sint32* eax, sint32* ebx, sint32* ecx, sint32* edx, sint32* esi, sint32* edi, sint32* ebp);
void game_command_place_wall(sint32* eax, sint32* ebx, sint32* ecx, sint32* edx, sint32* esi, sint32* edi, sint32* ebp);
void game_command_place_large_scenery(sint32* eax, sint32* ebx, sint32* ecx, sint32* edx, sint32* esi, sint32* edi, sint32* ebp);
void game_command_place_park_entrance(sint32* eax, sint32* ebx, sint32* ecx, sint32* edx, sint32* esi, sint32* edi, sint32* ebp);
void game_command_set_banner_name(sint32* eax, sint32* ebx, sint32* ecx, sint32* edx, sint32* esi, sint32* edi, sint32* ebp);
void game_command_set_banner_style(sint32* eax, sint32* ebx, sint32* ecx, sint32* edx, sint32* esi, sint32* edi, sint32* ebp);
void game_command_set_sign_style(sint32* eax, sint32* ebx, sint32* ecx, sint32* edx, sint32* esi, sint32* edi, sint32* ebp);
void game_command_modify_tile(sint32* eax, sint32* ebx, sint32* ecx, sint32* edx, sint32* esi, sint32* edi, sint32* ebp);

struct tile_element_iterator{
    sint32 x;
    sint32 y;
    rct_tile_element *element;
};
#ifdef PLATFORM_32BIT
assert_struct_size(tile_element_iterator, 12);
#endif

void tile_element_iterator_begin(tile_element_iterator *it);
sint32 tile_element_iterator_next(tile_element_iterator *it);
void tile_element_iterator_restart_for_tile(tile_element_iterator *it);

void wall_remove_intersecting_walls(sint32 x, sint32 y, sint32 z0, sint32 z1, sint32 direction);
void map_update_tiles();
sint32 map_get_highest_z(sint32 tileX, sint32 tileY);

bool tile_element_wants_path_connection_towards(TileCoordsXYZD coords, const rct_tile_element * const elementToBeRemoved);

void map_remove_out_of_range_elements();
void map_extend_boundary_surface();

bool sign_set_colour(sint32 x, sint32 y, sint32 z, sint32 direction, sint32 sequence, uint8 mainColour, uint8 textColour);
void wall_remove_at(sint32 x, sint32 y, sint32 z0, sint32 z1);
void wall_remove_at_z(sint32 x, sint32 y, sint32 z);

void map_invalidate_tile(sint32 x, sint32 y, sint32 z0, sint32 z1);
void map_invalidate_tile_zoom1(sint32 x, sint32 y, sint32 z0, sint32 z1);
void map_invalidate_tile_zoom0(sint32 x, sint32 y, sint32 z0, sint32 z1);
void map_invalidate_tile_full(sint32 x, sint32 y);
void map_invalidate_element(sint32 x, sint32 y, rct_tile_element *tileElement);
void map_invalidate_region(const LocationXY16& mins, const LocationXY16& maxs);

sint32 map_get_tile_side(sint32 mapX, sint32 mapY);
sint32 map_get_tile_quadrant(sint32 mapX, sint32 mapY);

void map_clear_all_elements();

rct_tile_element *map_get_large_scenery_segment(sint32 x, sint32 y, sint32 z, sint32 direction, sint32 sequence);
bool map_large_scenery_get_origin(
    sint32 x, sint32 y, sint32 z, sint32 direction, sint32 sequence,
    sint32 *outX, sint32 *outY, sint32 *outZ, rct_tile_element** outElement
);

void map_offset_with_rotation(sint16 *x, sint16 *y, sint16 offsetX, sint16 offsetY, uint8 rotation);
CoordsXY translate_3d_to_2d_with_z(sint32 rotation, CoordsXYZ pos);

rct_tile_element *map_get_track_element_at(sint32 x, sint32 y, sint32 z);
rct_tile_element *map_get_track_element_at_of_type(sint32 x, sint32 y, sint32 z, sint32 trackType);
rct_tile_element *map_get_track_element_at_of_type_seq(sint32 x, sint32 y, sint32 z, sint32 trackType, sint32 sequence);
rct_tile_element *map_get_track_element_at_of_type_from_ride(sint32 x, sint32 y, sint32 z, sint32 trackType, sint32 rideIndex);
rct_tile_element *map_get_track_element_at_from_ride(sint32 x, sint32 y, sint32 z, sint32 rideIndex);
rct_tile_element *map_get_track_element_at_with_direction_from_ride(sint32 x, sint32 y, sint32 z, sint32 direction, sint32 rideIndex);

bool map_is_location_at_edge(sint32 x, sint32 y);
void map_obstruction_set_error_text(rct_tile_element *tileElement);

uint32 map_get_available_peep_spawn_index_list(uint32* peepSpawnIndexList);
uint16 check_max_allowable_land_rights_for_tile(uint8 x, uint8 y, uint8 base_z);

void FixLandOwnershipTiles(std::initializer_list<TileCoordsXY> tiles);
void FixLandOwnershipTilesWithOwnership(std::initializer_list<TileCoordsXY> tiles, uint8 ownership);

bool place_peep_spawn(CoordsXYZD location);

uint8 entrance_element_get_type(const rct_tile_element * tileElement);

#endif
