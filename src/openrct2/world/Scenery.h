/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifndef _SCENERY_H_
#define _SCENERY_H_

#include "../common.h"
#include "../world/Location.hpp"
#include "../world/ScenerySelection.h"
#include "TileElement.h"

#include <limits>

#define SCENERY_WITHER_AGE_THRESHOLD_1 0x28
#define SCENERY_WITHER_AGE_THRESHOLD_2 0x37

#pragma pack(push, 1)
struct rct_small_scenery_entry
{
    uint32_t flags;                  // 0x06
    uint8_t height;                  // 0x0A
    uint8_t tool_id;                 // 0x0B
    int16_t price;                   // 0x0C
    int16_t removal_price;           // 0x0E
    uint8_t* frame_offsets;          // 0x10
    uint16_t animation_delay;        // 0x14
    uint16_t animation_mask;         // 0x16
    uint16_t num_frames;             // 0x18
    ObjectEntryIndex scenery_tab_id; // 0x1A
};

struct rct_large_scenery_tile
{
    int16_t x_offset;
    int16_t y_offset;
    int16_t z_offset;
    uint8_t z_clearance;
    // CCCC WWWW 0SS0 0000
    uint16_t flags;
};
assert_struct_size(rct_large_scenery_tile, 9);

enum
{
    LARGE_SCENERY_TILE_FLAG_NO_SUPPORTS = 0x20,
    LARGE_SCENERY_TILE_FLAG_ALLOW_SUPPORTS_ABOVE = 0x40,
};

struct rct_large_scenery_text_glyph
{
    uint8_t image_offset;
    uint8_t width;
    uint8_t height;
    uint8_t pad_3;
};
assert_struct_size(rct_large_scenery_text_glyph, 4);

struct rct_large_scenery_text
{
    LocationXY16 offset[2];                   // 0x0
    uint16_t max_width;                       // 0x8
    uint16_t pad_A;                           // 0xA
    uint8_t flags;                            // 0xC
    uint8_t num_images;                       // 0xD
    rct_large_scenery_text_glyph glyphs[256]; // 0xE
};
assert_struct_size(rct_large_scenery_text, 14 + 4 * 256);

enum LARGE_SCENERY_TEXT_FLAGS
{
    LARGE_SCENERY_TEXT_FLAG_VERTICAL = (1 << 0), // 0x1
    LARGE_SCENERY_TEXT_FLAG_TWO_LINE = (1 << 1), // 0x2
};

struct rct_large_scenery_entry
{
    uint8_t tool_id;
    uint8_t flags;
    int16_t price;
    int16_t removal_price;
    rct_large_scenery_tile* tiles;
    ObjectEntryIndex scenery_tab_id;
    uint8_t scrolling_mode;
    rct_large_scenery_text* text;
    uint32_t text_image;
};

enum LARGE_SCENERY_FLAGS
{
    LARGE_SCENERY_FLAG_HAS_PRIMARY_COLOUR = (1 << 0),   // 0x1
    LARGE_SCENERY_FLAG_HAS_SECONDARY_COLOUR = (1 << 1), // 0x2
    LARGE_SCENERY_FLAG_3D_TEXT = (1 << 2),              // 0x4
    LARGE_SCENERY_FLAG_ANIMATED = (1 << 3),             // 0x8
    LARGE_SCENERY_FLAG_PHOTOGENIC = (1 << 4),           // 0x10
};

struct rct_wall_scenery_entry
{
    uint8_t tool_id;
    uint8_t flags;
    uint8_t height;
    uint8_t flags2;
    int16_t price;
    ObjectEntryIndex scenery_tab_id;
    uint8_t scrolling_mode;
};

enum WALL_SCENERY_FLAGS
{
    WALL_SCENERY_HAS_PRIMARY_COLOUR = (1 << 0),   // 0x1
    WALL_SCENERY_HAS_GLASS = (1 << 1),            // 0x2
    WALL_SCENERY_CANT_BUILD_ON_SLOPE = (1 << 2),  // 0x4
    WALL_SCENERY_IS_BANNER = (1 << 3),            // 0x8  // Probably indicates translucency
    WALL_SCENERY_IS_DOOR = (1 << 4),              // 0x10
    WALL_SCENERY_LONG_DOOR_ANIMATION = (1 << 5),  // 0x20
    WALL_SCENERY_HAS_SECONDARY_COLOUR = (1 << 6), // 0x40
    WALL_SCENERY_HAS_TERNARY_COLOUR = (1 << 7),   // 0x80
};

enum WALL_SCENERY_2_FLAGS
{
    WALL_SCENERY_2_NO_SELECT_PRIMARY_COLOUR = (1 << 0), // 0x1
    WALL_SCENERY_2_DOOR_SOUND_MASK = 0x6,
    WALL_SCENERY_2_DOOR_SOUND_SHIFT = 1,
    WALL_SCENERY_2_IS_OPAQUE = (1 << 3), // 0x8
    WALL_SCENERY_2_ANIMATED = (1 << 4),  // 0x10
};

struct rct_path_bit_scenery_entry
{
    uint16_t flags;                  // 0x06
    uint8_t draw_type;               // 0x08
    uint8_t tool_id;                 // 0x09
    int16_t price;                   // 0x0A
    ObjectEntryIndex scenery_tab_id; // 0x0C
};

struct rct_banner_scenery_entry
{
    uint8_t scrolling_mode;          // 0x06
    uint8_t flags;                   // 0x07
    int16_t price;                   // 0x08
    ObjectEntryIndex scenery_tab_id; // 0x0A
};

struct rct_scenery_entry
{
    rct_string_id name; // 0x00
    uint32_t image;     // 0x02
    union
    {
        rct_small_scenery_entry small_scenery;
        rct_large_scenery_entry large_scenery;
        rct_wall_scenery_entry wall;
        rct_path_bit_scenery_entry path_bit;
        rct_banner_scenery_entry banner;
    };
};

#pragma pack(pop)

struct rct_scenery_group_entry
{
    rct_string_id name;
    uint32_t image;
    ScenerySelection scenery_entries[0x80];
    uint8_t entry_count;
    uint8_t priority;
    uint32_t entertainer_costumes;
};

enum
{
    PATH_BIT_FLAG_IS_BIN = 1 << 0,
    PATH_BIT_FLAG_IS_BENCH = 1 << 1,
    PATH_BIT_FLAG_BREAKABLE = 1 << 2,
    PATH_BIT_FLAG_LAMP = 1 << 3,
    PATH_BIT_FLAG_JUMPING_FOUNTAIN_WATER = 1 << 4,
    PATH_BIT_FLAG_JUMPING_FOUNTAIN_SNOW = 1 << 5,
    PATH_BIT_FLAG_DONT_ALLOW_ON_QUEUE = 1 << 6,
    PATH_BIT_FLAG_DONT_ALLOW_ON_SLOPE = 1 << 7,
    PATH_BIT_FLAG_IS_QUEUE_SCREEN = 1 << 8
};

enum
{
    PATH_BIT_DRAW_TYPE_LIGHTS,
    PATH_BIT_DRAW_TYPE_BINS,
    PATH_BIT_DRAW_TYPE_BENCHES,
    PATH_BIT_DRAW_TYPE_JUMPING_FOUNTAINS
};

enum
{
    SCENERY_TYPE_SMALL,
    SCENERY_TYPE_PATH_ITEM,
    SCENERY_TYPE_WALL,
    SCENERY_TYPE_LARGE,
    SCENERY_TYPE_BANNER,

    SCENERY_TYPE_COUNT,
};

enum
{
    SCENERY_GHOST_FLAG_0 = (1 << SCENERY_TYPE_SMALL),
    SCENERY_GHOST_FLAG_1 = (1 << SCENERY_TYPE_PATH_ITEM),
    SCENERY_GHOST_FLAG_2 = (1 << SCENERY_TYPE_WALL),
    SCENERY_GHOST_FLAG_3 = (1 << SCENERY_TYPE_LARGE),
    SCENERY_GHOST_FLAG_4 = (1 << SCENERY_TYPE_BANNER)
};

enum
{
    BANNER_ENTRY_FLAG_HAS_PRIMARY_COLOUR = (1 << 0),
};

enum class ScatterToolDensity : uint8_t
{
    LowDensity,
    MediumDensity,
    HighDensity
};

extern uint8_t gSceneryQuadrant;

extern money32 gSceneryPlaceCost;
extern ScenerySelection gSceneryPlaceObject;
extern uint16_t gSceneryPlaceObjectEntryIndex;
extern int16_t gSceneryPlaceZ;
extern uint8_t gSceneryPlaceRotation;

extern uint8_t gSceneryGhostType;
extern CoordsXYZ gSceneryGhostPosition;
extern uint8_t gSceneryGhostWallRotation;

extern int16_t gSceneryShiftPressed;
extern int16_t gSceneryShiftPressX;
extern int16_t gSceneryShiftPressY;
extern int16_t gSceneryShiftPressZOffset;

extern int16_t gSceneryCtrlPressed;
extern int16_t gSceneryCtrlPressZ;

extern const CoordsXY SceneryQuadrantOffsets[];

extern money32 gClearSceneryCost;

void init_scenery();
void scenery_update_tile(const CoordsXY& sceneryPos);
void scenery_set_default_placement_configuration();
void scenery_remove_ghost_tool_placement();

rct_scenery_entry* get_wall_entry(ObjectEntryIndex entryIndex);
rct_scenery_entry* get_banner_entry(ObjectEntryIndex entryIndex);
rct_scenery_entry* get_footpath_item_entry(ObjectEntryIndex entryIndex);
rct_scenery_group_entry* get_scenery_group_entry(ObjectEntryIndex entryIndex);

int32_t wall_entry_get_door_sound(const rct_scenery_entry* wallEntry);

#endif
