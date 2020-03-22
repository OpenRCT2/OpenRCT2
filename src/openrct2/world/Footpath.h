/*****************************************************************************
 * Copyright (c) 2014-2019 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "../interface/Viewport.h"
#include "../object/Object.h"

enum
{
    PROVISIONAL_PATH_FLAG_SHOW_ARROW = (1 << 0),
    PROVISIONAL_PATH_FLAG_1 = (1 << 1),
    PROVISIONAL_PATH_FLAG_2 = (1 << 2),
};

constexpr auto FootpathMaxHeight = 248 * COORDS_Z_STEP;
constexpr auto FootpathMinHeight = 2 * COORDS_Z_STEP;
constexpr auto PATH_HEIGHT_STEP = 2 * COORDS_Z_STEP;
constexpr auto PATH_CLEARANCE = 4 * COORDS_Z_STEP;

#define FOOTPATH_ELEMENT_INSERT_QUEUE 0x80

using PathSurfaceIndex = uint16_t;
constexpr PathSurfaceIndex PATH_SURFACE_INDEX_NULL = (PathSurfaceIndex)-1;

using PathRailingsIndex = uint8_t;
constexpr PathRailingsIndex PATH_RAILINGS_INDEX_NULL = (PathRailingsIndex)-1;

enum class RailingEntrySupportType : uint8_t
{
    Box = 0,
    Pole = 1,
    Count
};

#pragma pack(push, 1)
struct rct_footpath_entry
{
    rct_string_id string_idx;             // 0x00
    uint32_t image;                       // 0x02
    uint32_t bridge_image;                // 0x06
    RailingEntrySupportType support_type; // 0x0A
    uint8_t flags;                        // 0x0B
    uint8_t scrolling_mode;               // 0x0C
};
assert_struct_size(rct_footpath_entry, 13);
#pragma pack(pop)

struct PathSurfaceEntry
{
    rct_string_id string_idx;
    uint32_t image;
    uint32_t preview;
    uint8_t flags;
};

struct PathRailingsEntry
{
    rct_string_id string_idx;
    uint32_t preview;
    uint32_t bridge_image;
    uint32_t railings_image;
    RailingEntrySupportType support_type;
    uint8_t flags;
    uint8_t scrolling_mode;
};

// Masks for values stored in TileElement.type
enum
{
    FOOTPATH_ELEMENT_TYPE_FLAG_IS_QUEUE = (1 << 0),
    FOOTPATH_ELEMENT_TYPE_FLAG_IS_WIDE = (1 << 1),
    FOOTPATH_ELEMENT_TYPE_DIRECTION_MASK = (1 << 6) | (1 << 7),
};

// Masks and flags for values stored in TileElement.properties.path.type
enum
{
    FOOTPATH_PROPERTIES_SLOPE_DIRECTION_MASK = (1 << 0) | (1 << 1),
    FOOTPATH_PROPERTIES_FLAG_IS_SLOPED = (1 << 2),
    FOOTPATH_PROPERTIES_FLAG_HAS_QUEUE_BANNER = (1 << 3),
    FOOTPATH_PROPERTIES_TYPE_MASK = (1 << 4) | (1 << 5) | (1 << 6) | (1 << 7),
};

// Masks and flags for values stored in in TileElement.properties.path.edges
enum
{
    FOOTPATH_PROPERTIES_EDGES_EDGES_MASK = (1 << 0) | (1 << 1) | (1 << 2) | (1 << 3),
    FOOTPATH_PROPERTIES_EDGES_CORNERS_MASK = (1 << 4) | (1 << 5) | (1 << 6) | (1 << 7),
};

enum
{
    FOOTPATH_ELEMENT_FLAGS2_IS_SLOPED = 1 << 0,
    FOOTPATH_ELEMENT_FLAGS2_HAS_QUEUE_BANNER = (1 << 1),
    FOOTPATH_ELEMENT_FLAGS2_ADDITION_IS_GHOST = (1 << 2),
    FOOTPATH_ELEMENT_FLAGS2_BLOCKED_BY_VEHICLE = (1 << 3),
    FOOTPATH_ELEMENT_FLAGS2_ADDITION_IS_BROKEN = (1 << 4),
};

enum
{
    FOOTPATH_ENTRY_FLAG_SHOW_ONLY_IN_SCENARIO_EDITOR = (1 << 2),
    FOOTPATH_ENTRY_FLAG_IS_QUEUE = (1 << 3),
};

enum
{
    RAILING_ENTRY_FLAG_HAS_SUPPORT_BASE_SPRITE = (1 << 0),
    RAILING_ENTRY_FLAG_DRAW_PATH_OVER_SUPPORTS = (1 << 1), // When elevated
};

enum
{
    FOOTPATH_SEARCH_SUCCESS,
    FOOTPATH_SEARCH_NOT_FOUND,
    FOOTPATH_SEARCH_INCOMPLETE,
    FOOTPATH_SEARCH_TOO_COMPLEX
};

enum
{
    SLOPE_IS_IRREGULAR_FLAG = (1 << 3), // Flag set in `DefaultPathSlope[]` and checked in `footpath_place_real`
    RAISE_FOOTPATH_FLAG = (1 << 4)
};

enum
{
    FOOTPATH_CORNER_0 = (1 << 0),
    FOOTPATH_CORNER_1 = (1 << 1),
    FOOTPATH_CORNER_2 = (1 << 2),
    FOOTPATH_CORNER_3 = (1 << 3),
};

enum
{
    FOOTPATH_CONNECTION_S = (1 << 0),
    FOOTPATH_CONNECTION_NE = (1 << 1),
    FOOTPATH_CONNECTION_W = (1 << 2),
    FOOTPATH_CONNECTION_SE = (1 << 3),
    FOOTPATH_CONNECTION_N = (1 << 4),
    FOOTPATH_CONNECTION_SW = (1 << 5),
    FOOTPATH_CONNECTION_E = (1 << 6),
    FOOTPATH_CONNECTION_NW = (1 << 7),
};

enum
{
    WIDE_GROUP_PRIMARY_DIRECTION,
    WIDE_GROUP_SECONDARY_DIRECTION,
    WIDE_GROUP_PRIMARY_DIRECTION_REVERSE,
    WIDE_GROUP_SECONDARY_DIRECTION_REVERSE,
    WIDE_GROUP_FIRST_CORNER,
    WIDE_GROUP_SECOND_CORNER,
    WIDE_GROUP_FINAL_CORNER_CARDINAL,
    WIDE_GROUP_THIRD_CORNER_CARDINAL,
};
extern uint8_t gFootpathProvisionalFlags;
extern CoordsXYZ gFootpathProvisionalPosition;
extern uint8_t gFootpathProvisionalType;
extern uint8_t gFootpathProvisionalSlope;
extern uint8_t gFootpathConstructionMode;
extern uint16_t gFootpathSelectedId;
extern uint8_t gFootpathSelectedType;
extern CoordsXYZ gFootpathConstructFromPosition;
extern uint8_t gFootpathConstructDirection;
extern uint8_t gFootpathConstructSlope;
extern uint8_t gFootpathConstructValidDirections;
extern money32 gFootpathPrice;
extern uint8_t gFootpathGroundFlags;

// Given a direction, this will return how to increase/decrease the x and y coordinates.
extern const CoordsXY DirectionOffsets[NumOrthogonalDirections];
extern const CoordsXY BinUseOffsets[NumOrthogonalDirections];
extern const CoordsXY BenchUseOffsets[NumOrthogonalDirections * 2];

TileElement* map_get_footpath_element(const CoordsXYZ& coords);
struct PathElement;
PathElement* map_get_footpath_element_slope(const CoordsXYZ& footpathPos, int32_t slope);
void footpath_interrupt_peeps(const CoordsXYZ& footpathPos);
money32 footpath_remove(const CoordsXYZ& footpathLoc, int32_t flags);
money32 footpath_provisional_set(int32_t type, const CoordsXYZ& footpathLoc, int32_t slope);
void footpath_provisional_remove();
void footpath_provisional_update();
CoordsXY footpath_get_coordinates_from_pos(const ScreenCoordsXY& screenCoords, int32_t* direction, TileElement** tileElement);
CoordsXY footpath_bridge_get_info_from_pos(const ScreenCoordsXY& screenCoords, int32_t* direction, TileElement** tileElement);
void footpath_remove_litter(const CoordsXYZ& footpathPos);
void footpath_connect_edges(const CoordsXY& footpathPos, TileElement* tileElement, int32_t flags);
void footpath_update_queue_chains();
bool fence_in_the_way(const CoordsXYRangedZ& fencePos, int32_t direction);
void footpath_chain_ride_queue(
    ride_id_t rideIndex, int32_t entranceIndex, const CoordsXY& footpathPos, TileElement* tileElement, int32_t direction);
void footpath_update_path_wide_flags(const CoordsXY& footpathPos, uint8_t wideGroup);
bool footpath_is_blocked_by_vehicle(const TileCoordsXYZ& position);

int32_t footpath_is_connected_to_map_edge(const CoordsXYZ& footpathPos, int32_t direction, int32_t flags);
void footpath_remove_edges_at(const CoordsXY& footpathPos, TileElement* tileElement);
int32_t entrance_get_directions(const TileElement* tileElement);

PathSurfaceEntry* get_path_surface_entry(PathSurfaceIndex entryIndex);
PathRailingsEntry* get_path_railings_entry(PathRailingsIndex entryIndex);

void footpath_queue_chain_reset();
void footpath_queue_chain_push(ride_id_t rideIndex);
