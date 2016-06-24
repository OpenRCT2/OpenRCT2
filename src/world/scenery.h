#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
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
#include "../object.h"
#include "../world/map.h"

#pragma pack(push, 1)
typedef struct rct_small_scenery_entry {
	uint32 flags;			// 0x06
	uint8 height;			// 0x0A
	uint8 tool_id;			// 0x0B
	sint16 price;			// 0x0C
	sint16 removal_price;	// 0x0E
	uint32 var_10;
	uint16 var_14;
	uint16 var_16;
	uint16 var_18;
	uint8 scenery_tab_id;	// 0x1A
} rct_small_scenery_entry;
assert_struct_size(rct_small_scenery_entry, 21);

typedef enum {
	SMALL_SCENERY_FLAG_FULL_TILE = (1 << 0),					// 0x1
	SMALL_SCENERY_FLAG_VOFFSET_CENTRE = (1 << 1),				// 0x2
	SMALL_SCENERY_FLAG_REQUIRE_FLAT_SURFACE = (1 << 2),			// 0x4
	SMALL_SCENERY_FLAG4 = (1 << 3),								// 0x8
	SMALL_SCENERY_FLAG_ANIMATED = (1 << 4),						// 0x10
	SMALL_SCENERY_FLAG6 = (1 << 5),								// 0x20
	SMALL_SCENERY_FLAG_CAN_BE_WATERED = (1 << 6),				// 0x40
	SMALL_SCENERY_FLAG8 = (1 << 7),								// 0x80
	SMALL_SCENERY_FLAG9 = (1 << 8),								// 0x100
	SMALL_SCENERY_FLAG10 = (1 << 9),							// 0x200
	SMALL_SCENERY_FLAG_HAS_PRIMARY_COLOUR = (1 << 10),			// 0x400
	SMALL_SCENERY_FLAG12 = (1 << 11),							// 0x800
	SMALL_SCENERY_FLAG13 = (1 << 12),							// 0x1000
	SMALL_SCENERY_FLAG_IS_CLOCK = (1 << 13),					// 0x2000
	SMALL_SCENERY_FLAG15 = (1 << 14),							// 0x4000
	SMALL_SCENERY_FLAG16 = (1 << 15),							// 0x8000
	SMALL_SCENERY_FLAG17 = (1 << 16),							// 0x10000
	SMALL_SCENERY_FLAG18 = (1 << 17),							// 0x20000
	SMALL_SCENERY_FLAG_ALLOW_WALLS = (1 << 18),					// 0x40000
	SMALL_SCENERY_FLAG_HAS_SECONDARY_COLOUR = (1 << 19),		// 0x80000
	SMALL_SCENERY_FLAG20 = (1 << 20),							// 0x100000
	SMALL_SCENERY_FLAG21 = (1 << 21),							// 0x200000
	SMALL_SCENERY_FLAG22 = (1 << 22),							// 0x400000
	SMALL_SCENERY_FLAG23 = (1 << 23),							// 0x800000
	SMALL_SCENERY_FLAG24 = (1 << 24),							// 0x1000000
	SMALL_SCENERY_FLAG25 = (1 << 25),							// 0x2000000
	SMALL_SCENERY_FLAG26 = (1 << 26),							// 0x4000000
	SMALL_SCENERY_FLAG27 = (1 << 27),							// 0x8000000
} SMALL_SCENERY_FLAGS;

typedef struct rct_large_scenery_tile {
	sint16 x_offset;
	sint16 y_offset;
	sint16 z_offset;
	uint8 z_clearance;
	uint16 var_7;
} rct_large_scenery_tile;
assert_struct_size(rct_large_scenery_tile, 9);

typedef struct rct_large_scenery_text_glyph {
	uint8 image_offset;
	uint8 width;
	uint8 height;
	uint8 var_3;
} rct_large_scenery_text_glyph;
assert_struct_size(rct_large_scenery_text_glyph, 4);

typedef struct rct_large_scenery_text {
	rct_xy16 offset[2];		// 0x0
	uint16 max_width;		// 0x8
	uint16 var_A;			// 0xA
	uint16 var_C;			// 0xC
	rct_large_scenery_text_glyph glyphs[256]; // 0xE
} rct_large_scenery_text;
assert_struct_size(rct_large_scenery_text, 14 + 4 * 256);

typedef struct rct_large_scenery_entry {
	uint8 tool_id;			// 0x06
	uint8 flags;			// 0x07
	sint16 price;			// 0x08
	sint16 removal_price;	// 0x0A
	rct_large_scenery_tile* tiles; // 0x0C
	uint8 scenery_tab_id;	// 0x10
	uint8 var_11;
	rct_large_scenery_text* text; // 0x12
	uint32 text_image;	// 0x16
} rct_large_scenery_entry;
#ifdef PLATFORM_32BIT
assert_struct_size(rct_large_scenery_entry, 20);
#endif

typedef struct rct_wall_scenery_entry {
	uint8 tool_id;			// 0x06
	uint8 flags;			// 0x07
	uint8 height;			// 0x08
	uint8 flags2;			// 0x09
	sint16 price;			// 0x0A
	uint8 scenery_tab_id;	// 0x0C
	uint8 var_0D;
} rct_wall_scenery_entry;
assert_struct_size(rct_wall_scenery_entry, 8);

typedef enum {
	WALL_SCENERY_FLAG1 = (1 << 0),		// 0x1
	WALL_SCENERY_FLAG2 = (1 << 1),		// 0x2
	WALL_SCENERY_FLAG3 = (1 << 2),		// 0x4
	WALL_SCENERY_FLAG4 = (1 << 3),		// 0x8  // Probably indicates translucency
	WALL_SCENERY_IS_DOOR = (1 << 4),		// 0x10
	WALL_SCENERY_FLAG6 = (1 << 5),		// 0x20
	WALL_SCENERY_HAS_SECONDARY_COLOUR = (1 << 6),		// 0x40
	WALL_SCENERY_HAS_TERNARY_COLOUR = (1 << 7),			// 0x80
} WALL_SCENERY_FLAGS;

typedef enum {
	WALL_SCENERY_2_FLAG_5 = (1 << 4),		// 0x10
} WALL_SCENERY_2_FLAGS;

typedef struct rct_path_bit_scenery_entry {
	uint16 flags;			// 0x06
	uint8 draw_type;		// 0x08
	uint8 tool_id;			// 0x09
	sint16 price;			// 0x0A
	uint8 scenery_tab_id;	// 0x0C
} rct_path_bit_scenery_entry;
assert_struct_size(rct_path_bit_scenery_entry, 7);

typedef struct rct_banner_scenery_entry {
	uint8 scrolling_mode;	// 0x06
	uint8 flags;			// 0x07
	sint16 price;			// 0x08
	uint8 scenery_tab_id;	// 0x0A
} rct_banner_scenery_entry;
assert_struct_size(rct_banner_scenery_entry, 5);

typedef struct rct_scenery_entry {
	rct_string_id name;		// 0x00
	uint32 image;			// 0x02
	union {
		rct_small_scenery_entry small_scenery;
		rct_large_scenery_entry large_scenery;
		rct_wall_scenery_entry wall;
		rct_path_bit_scenery_entry path_bit;
		rct_banner_scenery_entry banner;
	};
} rct_scenery_entry;
#ifdef PLATFORM_32BIT
assert_struct_size(rct_scenery_entry, 6 + 21);
#endif

typedef struct rct_scenery_set_entry {
	rct_string_id name;				// 0x00
	uint32 image;					// 0x02
	uint16 scenery_entries[0x80];	// 0x06
	uint8 entry_count;				// 0x106
	uint8 var_107;
	uint8 var_108;					// 0x108, order?
	uint8 pad_109;
	uint32 var_10A;
} rct_scenery_set_entry;
assert_struct_size(rct_scenery_set_entry, 14 + 2 * 0x80);
#pragma pack(pop)

enum {
	PATH_BIT_FLAG_IS_BIN					= 1 << 0,
	PATH_BIT_FLAG_IS_BENCH					= 1 << 1,
	PATH_BIT_FLAG_BREAKABLE					= 1 << 2,
	PATH_BIT_FLAG_LAMP						= 1 << 3,
	PATH_BIT_FLAG_JUMPING_FOUNTAIN_WATER	= 1 << 4,
	PATH_BIT_FLAG_JUMPING_FOUNTAIN_SNOW		= 1 << 5,
	PATH_BIT_FLAG_DONT_ALLOW_ON_QUEUE		= 1 << 6,
	PATH_BIT_FLAG_DONT_ALLOW_ON_SLOPE		= 1 << 7,
	PATH_BIT_FLAG_IS_QUEUE_SCREEN			= 1 << 8
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

#define SCENERY_ENTRIES_BY_TAB 128

extern uint8 gWindowSceneryActiveTabIndex;
extern uint16 gWindowSceneryTabSelections[20];
extern uint8 gWindowSceneryClusterEnabled;
extern uint8 gWindowSceneryPaintEnabled;
extern uint8 gWindowSceneryRotation;
extern colour_t gWindowSceneryPrimaryColour;
extern colour_t gWindowScenerySecondaryColour;
extern colour_t gWindowSceneryTertiaryColour;

extern rct_map_element *gSceneryMapElement;
extern uint8 gSceneryMapElementType; 

extern money32 gSceneryPlaceCost;
extern sint16 gSceneryPlaceObject;
extern sint16 gSceneryPlaceZ;
extern uint8 gSceneryPlacePathType;
extern uint8 gSceneryPlacePathSlope;
extern uint8 gSceneryPlaceRotation;

extern uint8 gSceneryGhostType;
extern rct_xyz16 gSceneryGhostPosition;
extern uint32 gSceneryGhostPathObjectType;
extern uint8 gSceneryGhostWallRotation;

extern sint16 gSceneryShiftPressed;
extern sint16 gSceneryShiftPressX;
extern sint16 gSceneryShiftPressY;
extern sint16 gSceneryShiftPressZOffset;

extern sint16 gSceneryCtrlPressed;
extern sint16 gSceneryCtrlPressZ;

extern uint8 gSceneryGroundFlags;

extern const rct_xy8 ScenerySubTileOffsets[];

extern sint16 window_scenery_tab_entries[20][SCENERY_ENTRIES_BY_TAB + 1];

void init_scenery();
void scenery_update_tile(int x, int y);
void scenery_update_age(int x, int y, rct_map_element *mapElement);
void scenery_set_default_placement_configuration();
void scenery_remove_ghost_tool_placement();
void scenery_set_default_placement_configuration();

rct_scenery_entry *get_small_scenery_entry(int entryIndex);
rct_scenery_entry *get_large_scenery_entry(int entryIndex);
rct_scenery_entry *get_wall_entry(int entryIndex);
rct_scenery_entry *get_banner_entry(int entryIndex);
rct_scenery_entry *get_footpath_item_entry(int entryIndex);
rct_scenery_set_entry *get_scenery_group_entry(int entryIndex);


#endif
