/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifndef _MAP_H_
#define _MAP_H_

#include "../common.h"
#include "Location.hpp"
#include "TileElement.h"

#include <initializer_list>
#include <vector>

#define MINIMUM_LAND_HEIGHT 2
#define MAXIMUM_LAND_HEIGHT 142
#define MINIMUM_WATER_HEIGHT 2
#define MAXIMUM_WATER_HEIGHT 58

#define MINIMUM_MAP_SIZE_TECHNICAL 15
#define MAXIMUM_MAP_SIZE_TECHNICAL 256
#define MINIMUM_MAP_SIZE_PRACTICAL (MINIMUM_MAP_SIZE_TECHNICAL - 2)
#define MAXIMUM_MAP_SIZE_PRACTICAL (MAXIMUM_MAP_SIZE_TECHNICAL - 2)

#define MAP_MINIMUM_X_Y (-MAXIMUM_MAP_SIZE_TECHNICAL)

#define MAX_TILE_ELEMENTS 196096 // 0x30000
#define MAX_TILE_TILE_ELEMENT_POINTERS (MAXIMUM_MAP_SIZE_TECHNICAL * MAXIMUM_MAP_SIZE_TECHNICAL)
#define MAX_PEEP_SPAWNS 8
#define PEEP_SPAWN_UNDEFINED 0xFFFF

#define TILE_ELEMENT_LARGE_TYPE_MASK 0x3FF

#define TILE_UNDEFINED_TILE_ELEMENT NULL

typedef CoordsXYZD PeepSpawn;

struct CoordsXYE
{
    int32_t x, y;
    TileElement* element;
};

enum
{
    MAP_SELECT_FLAG_ENABLE = 1 << 0,
    MAP_SELECT_FLAG_ENABLE_CONSTRUCT = 1 << 1,
    MAP_SELECT_FLAG_ENABLE_ARROW = 1 << 2,
    MAP_SELECT_FLAG_GREEN = 1 << 3,
};

enum
{
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

extern uint16_t gWidePathTileLoopX;
extern uint16_t gWidePathTileLoopY;
extern uint16_t gGrassSceneryTileLoopPosition;

extern int16_t gMapSizeUnits;
extern int16_t gMapSizeMinus2;
extern int16_t gMapSize;
extern int16_t gMapSizeMaxXY;
extern int16_t gMapBaseZ;

extern uint16_t gMapSelectFlags;
extern uint16_t gMapSelectType;
extern LocationXY16 gMapSelectPositionA;
extern LocationXY16 gMapSelectPositionB;
extern LocationXYZ16 gMapSelectArrowPosition;
extern uint8_t gMapSelectArrowDirection;

extern uint8_t gMapGroundFlags;

extern TileElement gTileElements[MAX_TILE_TILE_ELEMENT_POINTERS * 3];
extern TileElement* gTileElementTilePointers[MAX_TILE_TILE_ELEMENT_POINTERS];

extern LocationXY16 gMapSelectionTiles[300];
extern std::vector<PeepSpawn> gPeepSpawns;

extern TileElement* gNextFreeTileElement;
extern uint32_t gNextFreeTileElementPointerIndex;

// Used in the land tool window to enable mountain tool / land smoothing
extern bool gLandMountainMode;
// Used in the land tool window to allow dragging and changing land styles
extern bool gLandPaintMode;
// Used in the clear scenery tool
extern bool gClearSmallScenery;
extern bool gClearLargeScenery;
extern bool gClearFootpath;

extern uint16_t gLandRemainingOwnershipSales;
extern uint16_t gLandRemainingConstructionSales;

extern LocationXYZ16 gCommandPosition;

extern bool gMapLandRightsUpdateSuccess;

void map_init(int32_t size);
void map_count_remaining_land_rights();
void map_strip_ghost_flag_from_elements();
void map_update_tile_pointers();
TileElement* map_get_first_element_at(int32_t x, int32_t y);
TileElement* map_get_nth_element_at(int32_t x, int32_t y, int32_t n);
void map_set_tile_elements(int32_t x, int32_t y, TileElement* elements);
int32_t map_height_from_slope(CoordsXY coords, int32_t slope, bool isSloped);
BannerElement* map_get_banner_element_at(int32_t x, int32_t y, int32_t z, uint8_t direction);
TileElement* map_get_surface_element_at(int32_t x, int32_t y);
TileElement* map_get_surface_element_at(CoordsXY coords);
TileElement* map_get_path_element_at(int32_t x, int32_t y, int32_t z);
TileElement* map_get_wall_element_at(int32_t x, int32_t y, int32_t z, int32_t direction);
TileElement* map_get_small_scenery_element_at(int32_t x, int32_t y, int32_t z, int32_t type, uint8_t quadrant);
EntranceElement* map_get_park_entrance_element_at(int32_t x, int32_t y, int32_t z, bool ghost);
EntranceElement* map_get_ride_entrance_element_at(int32_t x, int32_t y, int32_t z, bool ghost);
EntranceElement* map_get_ride_exit_element_at(int32_t x, int32_t y, int32_t z, bool ghost);
int32_t tile_element_height(int32_t x, int32_t y);
bool map_coord_is_connected(int32_t x, int32_t y, int32_t z, uint8_t faceDirection);
void map_remove_provisional_elements();
void map_restore_provisional_elements();
void map_update_path_wide_flags();
bool map_is_location_valid(CoordsXY coords);
bool map_is_edge(CoordsXY coords);
bool map_can_build_at(int32_t x, int32_t y, int32_t z);
bool map_is_location_owned(int32_t x, int32_t y, int32_t z);
bool map_is_location_in_park(CoordsXY coords);
bool map_is_location_owned_or_has_rights(int32_t x, int32_t y);
bool map_surface_is_blocked(int16_t x, int16_t y);
void tile_element_remove(TileElement* tileElement);
void map_remove_all_rides();
void map_invalidate_map_selection_tiles();
void map_get_bounding_box(
    int32_t ax, int32_t ay, int32_t bx, int32_t by, int32_t* left, int32_t* top, int32_t* right, int32_t* bottom);
void map_invalidate_selection_rect();
void map_reorganise_elements();
bool map_check_free_elements_and_reorganise(int32_t num_elements);
TileElement* tile_element_insert(int32_t x, int32_t y, int32_t z, int32_t flags);

using CLEAR_FUNC = int32_t (*)(TileElement** tile_element, int32_t x, int32_t y, uint8_t flags, money32* price);

int32_t map_place_non_scenery_clear_func(TileElement** tile_element, int32_t x, int32_t y, uint8_t flags, money32* price);
int32_t map_place_scenery_clear_func(TileElement** tile_element, int32_t x, int32_t y, uint8_t flags, money32* price);
bool map_can_construct_with_clear_at(
    int32_t x, int32_t y, int32_t zLow, int32_t zHigh, CLEAR_FUNC clearFunc, QuarterTile bl, uint8_t flags, money32* price,
    uint8_t crossingMode);
int32_t map_can_construct_at(int32_t x, int32_t y, int32_t zLow, int32_t zHigh, QuarterTile bl);
void rotate_map_coordinates(int16_t* x, int16_t* y, int32_t rotation);
LocationXY16 coordinate_3d_to_2d(const LocationXYZ16* coordinate_3d, int32_t rotation);
money32 map_clear_scenery(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t clear, int32_t flags);
money32 lower_water(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint8_t flags);
money32 raise_water(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint8_t flags);
money32 wall_place(
    int32_t type, int32_t x, int32_t y, int32_t z, int32_t edge, int32_t primaryColour, int32_t secondaryColour,
    int32_t tertiaryColour, int32_t flags);

void game_command_set_land_ownership(
    int32_t* eax, int32_t* ebx, int32_t* ecx, int32_t* edx, int32_t* esi, int32_t* edi, int32_t* ebp);
void game_command_remove_banner(
    int32_t* eax, int32_t* ebx, int32_t* ecx, int32_t* edx, int32_t* esi, int32_t* edi, int32_t* ebp);
void game_command_set_scenery_colour(
    int32_t* eax, int32_t* ebx, int32_t* ecx, int32_t* edx, int32_t* esi, int32_t* edi, int32_t* ebp);
void game_command_set_wall_colour(
    int32_t* eax, int32_t* ebx, int32_t* ecx, int32_t* edx, int32_t* esi, int32_t* edi, int32_t* ebp);
void game_command_set_large_scenery_colour(
    int32_t* eax, int32_t* ebx, int32_t* ecx, int32_t* edx, int32_t* esi, int32_t* edi, int32_t* ebp);
void game_command_set_banner_colour(
    int32_t* eax, int32_t* ebx, int32_t* ecx, int32_t* edx, int32_t* esi, int32_t* edi, int32_t* ebp);
void game_command_change_surface_style(
    int32_t* eax, int32_t* ebx, int32_t* ecx, int32_t* edx, int32_t* esi, int32_t* edi, int32_t* ebp);
void game_command_raise_land(int32_t* eax, int32_t* ebx, int32_t* ecx, int32_t* edx, int32_t* esi, int32_t* edi, int32_t* ebp);
void game_command_lower_land(int32_t* eax, int32_t* ebx, int32_t* ecx, int32_t* edx, int32_t* esi, int32_t* edi, int32_t* ebp);
void game_command_smooth_land(int32_t* eax, int32_t* ebx, int32_t* ecx, int32_t* edx, int32_t* esi, int32_t* edi, int32_t* ebp);
void game_command_raise_water(int32_t* eax, int32_t* ebx, int32_t* ecx, int32_t* edx, int32_t* esi, int32_t* edi, int32_t* ebp);
void game_command_lower_water(int32_t* eax, int32_t* ebx, int32_t* ecx, int32_t* edx, int32_t* esi, int32_t* edi, int32_t* ebp);
void game_command_set_water_height(
    int32_t* eax, int32_t* ebx, int32_t* ecx, int32_t* edx, int32_t* esi, int32_t* edi, int32_t* ebp);
void game_command_place_banner(
    int32_t* eax, int32_t* ebx, int32_t* ecx, int32_t* edx, int32_t* esi, int32_t* edi, int32_t* ebp);
void game_command_place_wall(int32_t* eax, int32_t* ebx, int32_t* ecx, int32_t* edx, int32_t* esi, int32_t* edi, int32_t* ebp);
void game_command_place_large_scenery(
    int32_t* eax, int32_t* ebx, int32_t* ecx, int32_t* edx, int32_t* esi, int32_t* edi, int32_t* ebp);
void game_command_place_park_entrance(
    int32_t* eax, int32_t* ebx, int32_t* ecx, int32_t* edx, int32_t* esi, int32_t* edi, int32_t* ebp);
void game_command_set_banner_name(
    int32_t* eax, int32_t* ebx, int32_t* ecx, int32_t* edx, int32_t* esi, int32_t* edi, int32_t* ebp);
void game_command_set_banner_style(
    int32_t* eax, int32_t* ebx, int32_t* ecx, int32_t* edx, int32_t* esi, int32_t* edi, int32_t* ebp);
void game_command_modify_tile(int32_t* eax, int32_t* ebx, int32_t* ecx, int32_t* edx, int32_t* esi, int32_t* edi, int32_t* ebp);

struct tile_element_iterator
{
    int32_t x;
    int32_t y;
    TileElement* element;
};
#ifdef PLATFORM_32BIT
assert_struct_size(tile_element_iterator, 12);
#endif

void tile_element_iterator_begin(tile_element_iterator* it);
int32_t tile_element_iterator_next(tile_element_iterator* it);
void tile_element_iterator_restart_for_tile(tile_element_iterator* it);

void wall_remove_intersecting_walls(int32_t x, int32_t y, int32_t z0, int32_t z1, int32_t direction);
void map_update_tiles();
int32_t map_get_highest_z(int32_t tileX, int32_t tileY);

bool tile_element_wants_path_connection_towards(TileCoordsXYZD coords, const TileElement* const elementToBeRemoved);

void map_remove_out_of_range_elements();
void map_extend_boundary_surface();

bool sign_set_colour(
    int32_t x, int32_t y, int32_t z, int32_t direction, int32_t sequence, uint8_t mainColour, uint8_t textColour);
void wall_remove_at(int32_t x, int32_t y, int32_t z0, int32_t z1);
void wall_remove_at_z(int32_t x, int32_t y, int32_t z);

void map_invalidate_tile(int32_t x, int32_t y, int32_t z0, int32_t z1);
void map_invalidate_tile_zoom1(int32_t x, int32_t y, int32_t z0, int32_t z1);
void map_invalidate_tile_zoom0(int32_t x, int32_t y, int32_t z0, int32_t z1);
void map_invalidate_tile_full(int32_t x, int32_t y);
void map_invalidate_element(int32_t x, int32_t y, TileElement* tileElement);
void map_invalidate_region(const LocationXY16& mins, const LocationXY16& maxs);

int32_t map_get_tile_side(int32_t mapX, int32_t mapY);
int32_t map_get_tile_quadrant(int32_t mapX, int32_t mapY);
int32_t map_get_corner_height(int32_t z, int32_t slope, int32_t direction);
int32_t tile_element_get_corner_height(const TileElement* tileElement, int32_t direction);

void map_clear_all_elements();

TileElement* map_get_large_scenery_segment(int32_t x, int32_t y, int32_t z, int32_t direction, int32_t sequence);
bool map_large_scenery_get_origin(
    int32_t x, int32_t y, int32_t z, int32_t direction, int32_t sequence, int32_t* outX, int32_t* outY, int32_t* outZ,
    TileElement** outElement);

void map_offset_with_rotation(int16_t* x, int16_t* y, int16_t offsetX, int16_t offsetY, uint8_t rotation);
CoordsXY translate_3d_to_2d_with_z(int32_t rotation, CoordsXYZ pos);

TileElement* map_get_track_element_at(int32_t x, int32_t y, int32_t z);
TileElement* map_get_track_element_at_of_type(int32_t x, int32_t y, int32_t z, int32_t trackType);
TileElement* map_get_track_element_at_of_type_seq(int32_t x, int32_t y, int32_t z, int32_t trackType, int32_t sequence);
TileElement* map_get_track_element_at_of_type_from_ride(
    int32_t x, int32_t y, int32_t z, int32_t trackType, ride_id_t rideIndex);
TileElement* map_get_track_element_at_from_ride(int32_t x, int32_t y, int32_t z, ride_id_t rideIndex);
TileElement* map_get_track_element_at_with_direction_from_ride(
    int32_t x, int32_t y, int32_t z, int32_t direction, ride_id_t rideIndex);

bool map_is_location_at_edge(int32_t x, int32_t y);
void map_obstruction_set_error_text(TileElement* tileElement);

uint16_t check_max_allowable_land_rights_for_tile(uint8_t x, uint8_t y, uint8_t base_z);

void FixLandOwnershipTiles(std::initializer_list<TileCoordsXY> tiles);
void FixLandOwnershipTilesWithOwnership(std::initializer_list<TileCoordsXY> tiles, uint8_t ownership);

bool place_peep_spawn(CoordsXYZD location);

#endif
