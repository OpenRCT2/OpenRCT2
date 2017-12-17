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

#ifndef _SCENERY_H_
#define _SCENERY_H_

#include "../common.h"
#include "../object/Object.h"
#include "Map.h"

#define SCENERY_SMALL_SCENERY_ID_MIN    0x0
#define SCENERY_SMALL_SCENERY_ID_MAX    0xFC
#define SCENERY_LARGE_SCENERY_ID_MIN    0x300
#define SCENERY_LARGE_SCENERY_ID_MAX    0x380
#define SCENERY_WALLS_ID_MIN            0x200
#define SCENERY_WALLS_ID_MAX            0x280
#define SCENERY_BANNERS_ID_MIN          0x400
#define SCENERY_BANNERS_ID_MAX          0x420
#define SCENERY_PATH_SCENERY_ID_MIN     0x100
#define SCENERY_PATH_SCENERY_ID_MAX     0x10F
#define SCENERY_WITHER_AGE_THRESHOLD_1  0x28
#define SCENERY_WITHER_AGE_THRESHOLD_2  0x37

#pragma pack(push, 1)
struct rct_small_scenery_entry {
    uint32 flags;           // 0x06
    uint8 height;           // 0x0A
    uint8 tool_id;          // 0x0B
    sint16 price;           // 0x0C
    sint16 removal_price;   // 0x0E
    uint8 *frame_offsets;   // 0x10
    uint16 animation_delay; // 0x14
    uint16 animation_mask;  // 0x16
    uint16 num_frames;      // 0x18
    uint8 scenery_tab_id;   // 0x1A
};
#ifdef PLATFORM_32BIT
assert_struct_size(rct_small_scenery_entry, 21);
#endif

struct rct_large_scenery_tile {
    sint16 x_offset;
    sint16 y_offset;
    sint16 z_offset;
    uint8 z_clearance;
    // CCCC WWWW 0SS0 0000
    uint16 flags;
};
assert_struct_size(rct_large_scenery_tile, 9);

enum
{
    LARGE_SCENERY_TILE_FLAG_NO_SUPPORTS             = 0x20,
    LARGE_SCENERY_TILE_FLAG_ALLOW_SUPPORTS_ABOVE    = 0x40,
};

struct rct_large_scenery_text_glyph {
    uint8 image_offset;
    uint8 width;
    uint8 height;
    uint8 pad_3;
};
assert_struct_size(rct_large_scenery_text_glyph, 4);

struct rct_large_scenery_text {
    LocationXY16 offset[2];     // 0x0
    uint16 max_width;       // 0x8
    uint16 pad_A;           // 0xA
    uint8 flags;            // 0xC
    uint8 var_D;            // 0xD
    rct_large_scenery_text_glyph glyphs[256]; // 0xE
};
assert_struct_size(rct_large_scenery_text, 14 + 4 * 256);

enum LARGE_SCENERY_TEXT_FLAGS {
    LARGE_SCENERY_TEXT_FLAG_VERTICAL = (1 << 0),    // 0x1
    LARGE_SCENERY_TEXT_FLAG_TWO_LINE = (1 << 1),    // 0x2
};

struct rct_large_scenery_entry {
    uint8 tool_id;                 // 0x06
    uint8 flags;                   // 0x07
    sint16 price;                  // 0x08
    sint16 removal_price;          // 0x0A
    rct_large_scenery_tile* tiles; // 0x0C
    uint8 scenery_tab_id;          // 0x10
    uint8 scrolling_mode;          // 0x11
    rct_large_scenery_text* text;  // 0x12
    uint32 text_image;             // 0x16
};
#ifdef PLATFORM_32BIT
assert_struct_size(rct_large_scenery_entry, 20);
#endif

enum LARGE_SCENERY_FLAGS {
    LARGE_SCENERY_FLAG_HAS_PRIMARY_COLOUR = (1 << 0),   // 0x1
    LARGE_SCENERY_FLAG_HAS_SECONDARY_COLOUR = (1 << 1), // 0x2
    LARGE_SCENERY_FLAG_3D_TEXT = (1 << 2),              // 0x4
    LARGE_SCENERY_FLAG_ANIMATED = (1 << 3),             // 0x8
    LARGE_SCENERY_FLAG_PHOTOGENIC = (1 << 4),           // 0x10
};

struct rct_wall_scenery_entry {
    uint8 tool_id;          // 0x06
    uint8 flags;            // 0x07
    uint8 height;           // 0x08
    uint8 flags2;           // 0x09
    sint16 price;           // 0x0A
    uint8 scenery_tab_id;   // 0x0C
    uint8 scrolling_mode;   // 0x0D 0xFF if no scrolling
};
assert_struct_size(rct_wall_scenery_entry, 8);

enum WALL_SCENERY_FLAGS
{
    WALL_SCENERY_HAS_PRIMARY_COLOUR = (1 << 0),     // 0x1
    WALL_SCENERY_HAS_GLASS = (1 << 1),      // 0x2
    WALL_SCENERY_CANT_BUILD_ON_SLOPE = (1 << 2),        // 0x4
    WALL_SCENERY_IS_BANNER = (1 << 3),      // 0x8  // Probably indicates translucency
    WALL_SCENERY_IS_DOOR = (1 << 4),        // 0x10
    WALL_SCENERY_LONG_DOOR_ANIMATION = (1 << 5),      // 0x20
    WALL_SCENERY_HAS_SECONDARY_COLOUR = (1 << 6),       // 0x40
    WALL_SCENERY_HAS_TERNARY_COLOUR = (1 << 7),         // 0x80
};

enum WALL_SCENERY_2_FLAGS {
    WALL_SCENERY_2_NO_SELECT_PRIMARY_COLOUR = (1 << 0),     // 0x1
    WALL_SCENERY_2_DOOR_SOUND_MASK = 0x6,
    WALL_SCENERY_2_DOOR_SOUND_SHIFT = 1,
    WALL_SCENERY_2_IS_OPAQUE = (1 << 3),        // 0x8
    WALL_SCENERY_2_ANIMATED = (1 << 4),        // 0x10
};

struct rct_path_bit_scenery_entry {
    uint16 flags;           // 0x06
    uint8 draw_type;        // 0x08
    uint8 tool_id;          // 0x09
    sint16 price;           // 0x0A
    uint8 scenery_tab_id;   // 0x0C
};
assert_struct_size(rct_path_bit_scenery_entry, 7);

struct rct_banner_scenery_entry {
    uint8 scrolling_mode;   // 0x06
    uint8 flags;            // 0x07
    sint16 price;           // 0x08
    uint8 scenery_tab_id;   // 0x0A
};
assert_struct_size(rct_banner_scenery_entry, 5);

struct rct_scenery_entry {
    rct_string_id name;     // 0x00
    uint32 image;           // 0x02
    union {
        rct_small_scenery_entry small_scenery;
        rct_large_scenery_entry large_scenery;
        rct_wall_scenery_entry wall;
        rct_path_bit_scenery_entry path_bit;
        rct_banner_scenery_entry banner;
    };
};
#ifdef PLATFORM_32BIT
assert_struct_size(rct_scenery_entry, 6 + 21);
#endif

struct rct_scenery_group_entry {
    rct_string_id name;             // 0x00
    uint32 image;                   // 0x02
    uint16 scenery_entries[0x80];   // 0x06
    uint8 entry_count;              // 0x106
    uint8 pad_107;
    uint8 priority;                 // 0x108
    uint8 pad_109;
    uint32 entertainer_costumes;    // 0x10A
};
assert_struct_size(rct_scenery_group_entry, 14 + 2 * 0x80);
#pragma pack(pop)

enum {
    PATH_BIT_FLAG_IS_BIN                    = 1 << 0,
    PATH_BIT_FLAG_IS_BENCH                  = 1 << 1,
    PATH_BIT_FLAG_BREAKABLE                 = 1 << 2,
    PATH_BIT_FLAG_LAMP                      = 1 << 3,
    PATH_BIT_FLAG_JUMPING_FOUNTAIN_WATER    = 1 << 4,
    PATH_BIT_FLAG_JUMPING_FOUNTAIN_SNOW     = 1 << 5,
    PATH_BIT_FLAG_DONT_ALLOW_ON_QUEUE       = 1 << 6,
    PATH_BIT_FLAG_DONT_ALLOW_ON_SLOPE       = 1 << 7,
    PATH_BIT_FLAG_IS_QUEUE_SCREEN           = 1 << 8
};

enum {
    PATH_BIT_DRAW_TYPE_LIGHTS,
    PATH_BIT_DRAW_TYPE_BINS,
    PATH_BIT_DRAW_TYPE_BENCHES,
    PATH_BIT_DRAW_TYPE_JUMPING_FOUNTAINS
};

enum {
    SCENERY_TYPE_SMALL,
    SCENERY_TYPE_PATH_ITEM,
    SCENERY_TYPE_WALL,
    SCENERY_TYPE_LARGE,
    SCENERY_TYPE_BANNER
};

enum {
    SCENERY_ENTRY_FLAG_0 = (1 << 0),
    SCENERY_ENTRY_FLAG_1 = (1 << 1),
    SCENERY_ENTRY_FLAG_2 = (1 << 2),
    SCENERY_ENTRY_FLAG_3 = (1 << 3),
    SCENERY_ENTRY_FLAG_4 = (1 << 4)
};

enum {
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

#define SCENERY_ENTRIES_BY_TAB 1024

extern uint8 gWindowSceneryActiveTabIndex;
extern uint16 gWindowSceneryTabSelections[20];
extern uint8 gWindowSceneryClusterEnabled;
extern uint8 gWindowSceneryPaintEnabled;
extern uint8 gWindowSceneryRotation;
extern colour_t gWindowSceneryPrimaryColour;
extern colour_t gWindowScenerySecondaryColour;
extern colour_t gWindowSceneryTertiaryColour;
extern bool gWindowSceneryEyedropperEnabled;

extern rct_tile_element *gSceneryTileElement;
extern uint8 gSceneryTileElementType;

extern money32 gSceneryPlaceCost;
extern sint16 gSceneryPlaceObject;
extern sint16 gSceneryPlaceZ;
extern uint8 gSceneryPlacePathType;
extern uint8 gSceneryPlacePathSlope;
extern uint8 gSceneryPlaceRotation;

extern uint8 gSceneryGhostType;
extern LocationXYZ16 gSceneryGhostPosition;
extern uint32 gSceneryGhostPathObjectType;
extern uint8 gSceneryGhostWallRotation;

extern sint16 gSceneryShiftPressed;
extern sint16 gSceneryShiftPressX;
extern sint16 gSceneryShiftPressY;
extern sint16 gSceneryShiftPressZOffset;

extern sint16 gSceneryCtrlPressed;
extern sint16 gSceneryCtrlPressZ;

extern uint8 gSceneryGroundFlags;

extern const LocationXY8 ScenerySubTileOffsets[];

extern money32 gClearSceneryCost;

void init_scenery();
void scenery_update_tile(sint32 x, sint32 y);
void scenery_update_age(sint32 x, sint32 y, rct_tile_element *tileElement);
void scenery_set_default_placement_configuration();
void scenery_remove_ghost_tool_placement();

rct_scenery_entry *get_small_scenery_entry(sint32 entryIndex);
rct_scenery_entry *get_large_scenery_entry(sint32 entryIndex);
rct_scenery_entry *get_wall_entry(sint32 entryIndex);
rct_scenery_entry *get_banner_entry(sint32 entryIndex);
rct_scenery_entry *get_footpath_item_entry(sint32 entryIndex);
rct_scenery_group_entry *get_scenery_group_entry(sint32 entryIndex);

sint32 get_scenery_id_from_entry_index(uint8 objectType, sint32 entryIndex);
sint32 wall_entry_get_door_sound(const rct_scenery_entry * wallEntry);

#endif
