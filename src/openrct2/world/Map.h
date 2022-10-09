/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "Location.hpp"
#include "TileElement.h"

#include <initializer_list>
#include <vector>

#define MINIMUM_LAND_HEIGHT 2
#define MAXIMUM_LAND_HEIGHT 142
#define MINIMUM_WATER_HEIGHT 2
#define MAXIMUM_WATER_HEIGHT 142

#define MINIMUM_MAP_SIZE_TECHNICAL 15
#define MAXIMUM_MAP_SIZE_TECHNICAL 1001
#define MINIMUM_MAP_SIZE_PRACTICAL (MINIMUM_MAP_SIZE_TECHNICAL - 2)
#define MAXIMUM_MAP_SIZE_PRACTICAL (MAXIMUM_MAP_SIZE_TECHNICAL - 2)
constexpr const int32_t MAXIMUM_MAP_SIZE_BIG = COORDS_XY_STEP * MAXIMUM_MAP_SIZE_TECHNICAL;
constexpr const int32_t MAXIMUM_TILE_START_XY = MAXIMUM_MAP_SIZE_BIG - COORDS_XY_STEP;
constexpr const int32_t LAND_HEIGHT_STEP = 2 * COORDS_Z_STEP;
constexpr const int32_t WATER_HEIGHT_STEP = 2 * COORDS_Z_STEP;
constexpr const int32_t MINIMUM_LAND_HEIGHT_BIG = MINIMUM_LAND_HEIGHT * COORDS_Z_STEP;
constexpr const TileCoordsXY DEFAULT_MAP_SIZE = { 150, 150 };
// How high construction has to be off the ground when the player owns construction rights, in tile coords.
constexpr const uint8_t ConstructionRightsClearanceSmall = 3;
// Same as previous, but in big coords.
constexpr const uint8_t ConstructionRightsClearanceBig = 3 * COORDS_Z_STEP;

#define MAP_MINIMUM_X_Y (-MAXIMUM_MAP_SIZE_TECHNICAL)

constexpr const uint32_t MAX_TILE_ELEMENTS_WITH_SPARE_ROOM = 0x1000000;
constexpr const uint32_t MAX_TILE_ELEMENTS = MAX_TILE_ELEMENTS_WITH_SPARE_ROOM - 512;
#define MAX_TILE_TILE_ELEMENT_POINTERS (MAXIMUM_MAP_SIZE_TECHNICAL * MAXIMUM_MAP_SIZE_TECHNICAL)

#define TILE_UNDEFINED_TILE_ELEMENT NULL

using PeepSpawn = CoordsXYZD;

struct CoordsXYE : public CoordsXY
{
    CoordsXYE() = default;
    constexpr CoordsXYE(int32_t _x, int32_t _y, TileElement* _e)
        : CoordsXY(_x, _y)
        , element(_e)
    {
    }

    constexpr CoordsXYE(const CoordsXY& c, TileElement* _e)
        : CoordsXY(c)
        , element(_e)
    {
    }
    TileElement* element = nullptr;
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

// Used when calling MapCanConstructWithClearAt();
// This assumes that the caller has already done the check on the element it wants to place,
// as this function can only check the element the player wants to build through.
enum
{
    CREATE_CROSSING_MODE_NONE,
    CREATE_CROSSING_MODE_TRACK_OVER_PATH,
    CREATE_CROSSING_MODE_PATH_OVER_TRACK,
};

extern const std::array<CoordsXY, 8> CoordsDirectionDelta;
extern const TileCoordsXY TileDirectionDelta[];

extern TileCoordsXY gWidePathTileLoopPosition;
extern uint16_t gGrassSceneryTileLoopPosition;

extern TileCoordsXY gMapSize;
extern int32_t gMapBaseZ;

inline CoordsXY GetMapSizeUnits()
{
    return { (gMapSize.x - 1) * COORDS_XY_STEP, (gMapSize.y - 1) * COORDS_XY_STEP };
}
inline CoordsXY GetMapSizeMinus2()
{
    return { (gMapSize.x * COORDS_XY_STEP) + (8 * COORDS_XY_STEP - 2),
             (gMapSize.y * COORDS_XY_STEP) + (8 * COORDS_XY_STEP - 2) };
}
inline CoordsXY GetMapSizeMaxXY()
{
    return GetMapSizeUnits() - CoordsXY{ 1, 1 };
}

extern uint16_t gMapSelectFlags;
extern uint16_t gMapSelectType;
extern CoordsXY gMapSelectPositionA;
extern CoordsXY gMapSelectPositionB;
extern CoordsXYZ gMapSelectArrowPosition;
extern uint8_t gMapSelectArrowDirection;

extern std::vector<CoordsXY> gMapSelectionTiles;
extern std::vector<PeepSpawn> gPeepSpawns;

// Used in the land tool window to enable mountain tool / land smoothing
extern bool gLandMountainMode;
// Used in the land tool window to allow dragging and changing land styles
extern bool gLandPaintMode;
// Used in the clear scenery tool
extern bool gClearSmallScenery;
extern bool gClearLargeScenery;
extern bool gClearFootpath;

extern uint32_t gLandRemainingOwnershipSales;
extern uint32_t gLandRemainingConstructionSales;

extern bool gMapLandRightsUpdateSuccess;

constexpr auto SURFACE_STYLE_FLAG_RAISE_OR_LOWER_BASE_HEIGHT = 0x20;
extern const uint8_t tile_element_lower_styles[9][32];
extern const uint8_t tile_element_raise_styles[9][32];

void ReorganiseTileElements();
const std::vector<TileElement>& GetTileElements();
void SetTileElements(std::vector<TileElement>&& tileElements);
void StashMap();
void UnstashMap();
std::vector<TileElement> GetReorganisedTileElementsWithoutGhosts();

void map_init(const TileCoordsXY& size);

void map_count_remaining_land_rights();
void map_strip_ghost_flag_from_elements();
TileElement* map_get_first_element_at(const CoordsXY& tilePos);
TileElement* map_get_first_element_at(const TileCoordsXY& tilePos);
TileElement* map_get_nth_element_at(const CoordsXY& coords, int32_t n);
TileElement* MapGetFirstTileElementWithBaseHeightBetween(const TileCoordsXYRangedZ& loc, TileElementType type);
void map_set_tile_element(const TileCoordsXY& tilePos, TileElement* elements);
int32_t map_height_from_slope(const CoordsXY& coords, int32_t slopeDirection, bool isSloped);
BannerElement* map_get_banner_element_at(const CoordsXYZ& bannerPos, uint8_t direction);
SurfaceElement* map_get_surface_element_at(const CoordsXY& coords);
PathElement* map_get_path_element_at(const TileCoordsXYZ& loc);
WallElement* map_get_wall_element_at(const CoordsXYZD& wallCoords);
WallElement* map_get_wall_element_at(const CoordsXYRangedZ& coords);
SmallSceneryElement* map_get_small_scenery_element_at(const CoordsXYZ& sceneryCoords, int32_t type, uint8_t quadrant);
EntranceElement* map_get_park_entrance_element_at(const CoordsXYZ& entranceCoords, bool ghost);
EntranceElement* map_get_ride_entrance_element_at(const CoordsXYZ& entranceCoords, bool ghost);
EntranceElement* map_get_ride_exit_element_at(const CoordsXYZ& exitCoords, bool ghost);
int16_t tile_element_height(const CoordsXY& loc);
int16_t tile_element_water_height(const CoordsXY& loc);
uint8_t map_get_highest_land_height(const MapRange& range);
uint8_t map_get_lowest_land_height(const MapRange& range);
bool map_coord_is_connected(const TileCoordsXYZ& loc, uint8_t faceDirection);
void map_remove_provisional_elements();
void map_restore_provisional_elements();
void map_update_path_wide_flags();
bool map_is_location_valid(const CoordsXY& coords);
bool map_is_edge(const CoordsXY& coords);
bool map_can_build_at(const CoordsXYZ& loc);
bool map_is_location_owned(const CoordsXYZ& loc);
bool map_is_location_in_park(const CoordsXY& coords);
bool map_is_location_owned_or_has_rights(const CoordsXY& loc);
bool map_surface_is_blocked(const CoordsXY& mapCoords);
void tile_element_remove(TileElement* tileElement);
void map_remove_all_rides();
void map_invalidate_map_selection_tiles();
void map_invalidate_selection_rect();
bool MapCheckCapacityAndReorganise(const CoordsXY& loc, size_t numElements = 1);
TileElement* tile_element_insert(const CoordsXYZ& loc, int32_t occupiedQuadrants, TileElementType type);

template<typename T = TileElement> T* MapGetFirstTileElementWithBaseHeightBetween(const TileCoordsXYRangedZ& loc)
{
    auto* element = MapGetFirstTileElementWithBaseHeightBetween(loc, T::ElementType);
    return element != nullptr ? element->template as<T>() : nullptr;
}

template<typename T> T* TileElementInsert(const CoordsXYZ& loc, int32_t occupiedQuadrants)
{
    auto* element = tile_element_insert(loc, occupiedQuadrants, T::ElementType);
    return (element != nullptr) ? element->template as<T>() : nullptr;
}

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

void map_update_tiles();
int32_t map_get_highest_z(const CoordsXY& loc);

bool TileElementWantsPathConnectionTowards(const TileCoordsXYZD& coords, const TileElement* const elementToBeRemoved);

void map_remove_out_of_range_elements();
void map_extend_boundary_surface_x();
void map_extend_boundary_surface_y();

bool map_large_scenery_sign_set_colour(const CoordsXYZD& signPos, int32_t sequence, uint8_t mainColour, uint8_t textColour);
void WallRemoveAt(const CoordsXYRangedZ& wallPos);
void WallRemoveAtZ(const CoordsXYZ& wallPos);
void WallRemoveIntersectingWalls(const CoordsXYRangedZ& wallPos, Direction direction);

void map_invalidate_tile(const CoordsXYRangedZ& tilePos);
void map_invalidate_tile_zoom1(const CoordsXYRangedZ& tilePos);
void map_invalidate_tile_zoom0(const CoordsXYRangedZ& tilePos);
void map_invalidate_tile_full(const CoordsXY& tilePos);
void map_invalidate_element(const CoordsXY& elementPos, TileElement* tileElement);
void map_invalidate_region(const CoordsXY& mins, const CoordsXY& maxs);

int32_t map_get_tile_side(const CoordsXY& mapPos);
int32_t map_get_tile_quadrant(const CoordsXY& mapPos);
int32_t map_get_corner_height(int32_t z, int32_t slope, int32_t direction);
int32_t tile_element_get_corner_height(const SurfaceElement* surfaceElement, int32_t direction);

void map_clear_all_elements();

LargeSceneryElement* map_get_large_scenery_segment(const CoordsXYZD& sceneryPos, int32_t sequence);
std::optional<CoordsXYZ> map_large_scenery_get_origin(
    const CoordsXYZD& sceneryPos, int32_t sequence, LargeSceneryElement** outElement);

ScreenCoordsXY translate_3d_to_2d_with_z(int32_t rotation, const CoordsXYZ& pos);

TrackElement* map_get_track_element_at(const CoordsXYZ& trackPos);
TileElement* map_get_track_element_at_of_type(const CoordsXYZ& trackPos, track_type_t trackType);
TileElement* map_get_track_element_at_of_type_seq(const CoordsXYZ& trackPos, track_type_t trackType, int32_t sequence);
TrackElement* map_get_track_element_at_of_type(const CoordsXYZD& location, track_type_t trackType);
TrackElement* map_get_track_element_at_of_type_seq(const CoordsXYZD& location, track_type_t trackType, int32_t sequence);
TileElement* map_get_track_element_at_of_type_from_ride(const CoordsXYZ& trackPos, track_type_t trackType, RideId rideIndex);
TileElement* map_get_track_element_at_from_ride(const CoordsXYZ& trackPos, RideId rideIndex);
TileElement* map_get_track_element_at_with_direction_from_ride(const CoordsXYZD& trackPos, RideId rideIndex);

bool map_is_location_at_edge(const CoordsXY& loc);

uint16_t check_max_allowable_land_rights_for_tile(const CoordsXYZ& tileMapPos);

void FixLandOwnershipTiles(std::initializer_list<TileCoordsXY> tiles);
void FixLandOwnershipTilesWithOwnership(
    std::initializer_list<TileCoordsXY> tiles, uint8_t ownership, bool doNotDowngrade = false);
MapRange ClampRangeWithinMap(const MapRange& range);
