/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifndef _WORLD_FOOTPATH_H_
#define _WORLD_FOOTPATH_H_

#include "../common.h"
#include "../interface/Viewport.h"
#include "../object/Object.h"

enum
{
    PROVISIONAL_PATH_FLAG_SHOW_ARROW = (1 << 0),
    PROVISIONAL_PATH_FLAG_1 = (1 << 1),
    PROVISIONAL_PATH_FLAG_2 = (1 << 2),
};

#define FOOTPATH_ELEMENT_INSERT_QUEUE 0x80

#pragma pack(push, 1)
struct rct_footpath_entry
{
    rct_string_id string_idx; // 0x00
    uint32_t image;           // 0x02
    uint32_t bridge_image;    // 0x06
    uint8_t support_type;     // 0x0A
    uint8_t flags;            // 0x0B
    uint8_t scrolling_mode;   // 0x0C
};
assert_struct_size(rct_footpath_entry, 13);
#pragma pack(pop)

struct PathSurfaceEntry
{
    rct_string_id string_idx;
    uint32_t image;
    uint32_t queue_image;
    uint32_t preview;
    uint8_t flags;
};

struct PathRailingsEntry
{
    rct_string_id string_idx;
    uint32_t preview;
    uint32_t bridge_image;
    uint32_t railings_image;
    uint8_t support_type;
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

// Masks and flags for values stored in in TileElement.properties.path.additions
enum
{
    FOOTPATH_PROPERTIES_ADDITIONS_TYPE_MASK = (1 << 0) | (1 << 1) | (1 << 2) | (1 << 3),
    // The most significant bit in this mask will always be zero, since rides can only have 4 stations
    FOOTPATH_PROPERTIES_ADDITIONS_STATION_INDEX_MASK = (1 << 4) | (1 << 5) | (1 << 6),
    FOOTPATH_PROPERTIES_ADDITIONS_FLAG_GHOST = (1 << 7),
};

enum
{
    FOOTPATH_ENTRY_SUPPORT_TYPE_BOX = 0,
    FOOTPATH_ENTRY_SUPPORT_TYPE_POLE = 1,
    FOOTPATH_ENTRY_SUPPORT_TYPE_COUNT
};

enum
{
    FOOTPATH_ENTRY_FLAG_SHOW_ONLY_IN_SCENARIO_EDITOR = (1 << 2),
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
    FOOTPATH_ADDITION_FLAG_IS_GHOST = (1 << 7),
};

enum
{
    FOOTPATH_CLEAR_DIRECTIONAL = (1 << 8), // Flag set when direction is used.
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

extern uint8_t gFootpathProvisionalFlags;
extern LocationXYZ16 gFootpathProvisionalPosition;
extern uint8_t gFootpathProvisionalType;
extern uint8_t gFootpathProvisionalSlope;
extern uint8_t gFootpathConstructionMode;
extern uint16_t gFootpathSelectedId;
extern uint8_t gFootpathSelectedType;
extern LocationXYZ16 gFootpathConstructFromPosition;
extern uint8_t gFootpathConstructDirection;
extern uint8_t gFootpathConstructSlope;
extern uint8_t gFootpathConstructValidDirections;
extern money32 gFootpathPrice;
extern uint8_t gFootpathGroundFlags;

extern const LocationXY16 word_981D6C[4];
extern const LocationXY16 BinUseOffsets[4];
extern const LocationXY16 BenchUseOffsets[8];

TileElement* map_get_footpath_element(int32_t x, int32_t y, int32_t z);
struct PathElement;
PathElement* map_get_footpath_element_slope(int32_t x, int32_t y, int32_t z, int32_t slope);
void footpath_interrupt_peeps(int32_t x, int32_t y, int32_t z);
void game_command_place_footpath_from_track(
    int32_t* eax, int32_t* ebx, int32_t* ecx, int32_t* edx, int32_t* esi, int32_t* edi, int32_t* ebp);
void game_command_remove_footpath(
    int32_t* eax, int32_t* ebx, int32_t* ecx, int32_t* edx, int32_t* esi, int32_t* edi, int32_t* ebp);
money32 footpath_remove(int32_t x, int32_t y, int32_t z, int32_t flags);
money32 footpath_provisional_set(int32_t type, int32_t x, int32_t y, int32_t z, int32_t slope);
void footpath_provisional_remove();
void footpath_provisional_update();
void remove_banners_at_element(int32_t x, int32_t y, TileElement* tileElement);
void footpath_get_coordinates_from_pos(
    int32_t screenX, int32_t screenY, int32_t* x, int32_t* y, int32_t* direction, TileElement** tileElement);
void footpath_bridge_get_info_from_pos(
    int32_t screenX, int32_t screenY, int32_t* x, int32_t* y, int32_t* direction, TileElement** tileElement);
void footpath_remove_litter(int32_t x, int32_t y, int32_t z);
void footpath_connect_edges(int32_t x, int32_t y, TileElement* tileElement, int32_t flags);
void footpath_update_queue_chains();
bool fence_in_the_way(int32_t x, int32_t y, int32_t z0, int32_t z1, int32_t direction);
void footpath_chain_ride_queue(
    ride_id_t rideIndex, int32_t entranceIndex, int32_t x, int32_t y, TileElement* tileElement, int32_t direction);
void footpath_update_path_wide_flags(int32_t x, int32_t y);
bool footpath_is_blocked_by_vehicle(const TileCoordsXYZ& position);

int32_t footpath_is_connected_to_map_edge(int32_t x, int32_t y, int32_t z, int32_t direction, int32_t flags);
void footpath_remove_edges_at(int32_t x, int32_t y, TileElement* tileElement);
int32_t entrance_get_directions(const TileElement* tileElement);

PathSurfaceEntry* get_path_surface_entry(int32_t entryIndex);
PathRailingsEntry* get_path_railings_entry(int32_t entryIndex);

void footpath_queue_chain_reset();
void footpath_queue_chain_push(ride_id_t rideIndex);

#endif
