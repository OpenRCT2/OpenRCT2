/*****************************************************************************
* Copyright (c) 2014 Dániel Tar
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

#ifndef _SCENERY_H_
#define _SCENERY_H_

#include "../common.h"

typedef struct {
	uint32 flags;			// 0x06
	uint8 height;			// 0x0A
	uint8 tool_id;			// 0x0B
	sint16 price;			// 0x0C
	sint16 removal_price;	// 0x0E
	uint32 var_10;
	uint8 pad_14[0x06];
	uint8 scenery_tab_id;	// 0x1A
} rct_small_scenery_entry;

typedef enum {
	SMALL_SCENERY_FLAG1 = (1 << 0),		// 0x1
	SMALL_SCENERY_FLAG2 = (1 << 1),		// 0x2
	SMALL_SCENERY_FLAG3 = (1 << 2),		// 0x4
	SMALL_SCENERY_FLAG4 = (1 << 3),		// 0x8
	SMALL_SCENERY_FLAG5 = (1 << 4),		// 0x10
	SMALL_SCENERY_FLAG6 = (1 << 5),		// 0x20
	SMALL_SCENERY_FLAG7 = (1 << 6),		// 0x40
	SMALL_SCENERY_FLAG8 = (1 << 7),		// 0x80
	SMALL_SCENERY_FLAG9 = (1 << 8),		// 0x100
	SMALL_SCENERY_FLAG10 = (1 << 9),	// 0x200
	SMALL_SCENERY_HAS_PRIMARY_COLOUR = (1 << 10),	// 0x400
	SMALL_SCENERY_FLAG12 = (1 << 11),	// 0x800
	SMALL_SCENERY_FLAG13 = (1 << 12),	// 0x1000
	SMALL_SCENERY_FLAG14 = (1 << 13),	// 0x2000
	SMALL_SCENERY_FLAG15 = (1 << 14),	// 0x4000
	SMALL_SCENERY_FLAG16 = (1 << 15),	// 0x8000
	SMALL_SCENERY_FLAG17 = (1 << 16),	// 0x10000
	SMALL_SCENERY_FLAG18 = (1 << 17),	// 0x20000
	SMALL_SCENERY_FLAG19 = (1 << 18),	// 0x40000
	SMALL_SCENERY_HAS_SECONDARY_COLOUR = (1 << 19),	// 0x80000
} SMALL_SCENERY_FLAGS;

typedef struct {
	uint8 tool_id;			// 0x06
	uint8 flags;			// 0x07
	sint16 price;			// 0x08
	sint16 removal_price;	// 0x0A
	uint32 var_0C;
	uint8 scenery_tab_id;	// 0x10
	uint8 var_11;
	uint32 var_12;
	uint32 var_16;
} rct_large_scenery_entry;


typedef struct {
	uint8 tool_id;			// 0x06
	uint8 flags;			// 0x07
	uint8 height;			// 0x08
	uint8 flags2;			// 0x09
	sint16 price;			// 0x0A
	uint8 scenery_tab_id;	// 0x0C
	uint8 var_0D; 
} rct_wall_scenery_entry;

typedef enum {
	WALL_SCENERY_FLAG1 = (1 << 0),		// 0x1
	WALL_SCENERY_FLAG2 = (1 << 1),		// 0x2
	WALL_SCENERY_FLAG3 = (1 << 2),		// 0x4
	WALL_SCENERY_FLAG4 = (1 << 3),		// 0x8
	WALL_SCENERY_FLAG5 = (1 << 4),		// 0x10
	WALL_SCENERY_FLAG6 = (1 << 5),		// 0x20
	WALL_SCENERY_HAS_SECONDARY_COLOUR = (1 << 6),		// 0x40
	WALL_SCENERY_HAS_TERNARY_COLOUR = (1 << 7),			// 0x80
} WALL_SCENERY_FLAGS;

typedef struct {
	uint16 var_06;
	uint8 pad_08;
	uint8 tool_id;			// 0x09
	uint16 price;			// 0x0A
	uint8 scenery_tab_id;	// 0x0C
} rct_path_bit_scenery_entry;

typedef struct {
	uint8 var_06;
	uint8 flags;			// 0x07
	uint16 price;			// 0x08
	uint8 scenery_tab_id;	// 0x0A
} rct_banner_scenery_entry;

typedef struct {
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

typedef struct {
	rct_string_id name;				// 0x00
	uint32 image;					// 0x02
	uint16 scenery_entries[0x80];	// 0x06
	uint8 entry_count;				// 0x106
	uint8 pad_107;
	uint8 var_108;					// 0x108, order?
	uint8 pad_109;
	uint32 var_10A;
} rct_scenery_set_entry;

#define g_smallSceneryEntries RCT2_ADDRESS(RCT2_ADDRESS_SMALL_SCENERY_ENTRIES, rct_scenery_entry*)
#define g_largeSceneryEntries RCT2_ADDRESS(RCT2_ADDRESS_LARGE_SCENERY_ENTRIES, rct_scenery_entry*)
#define g_wallSceneryEntries RCT2_ADDRESS(RCT2_ADDRESS_WALL_SCENERY_ENTRIES, rct_scenery_entry*)
#define g_bannerSceneryEntries RCT2_ADDRESS(RCT2_ADDRESS_BANNER_SCENERY_ENTRIES, rct_scenery_entry*)
#define g_pathBitSceneryEntries RCT2_ADDRESS(RCT2_ADDRESS_PATH_BIT_SCENERY_ENTRIES, rct_scenery_entry*)
#define g_scenerySetEntries RCT2_ADDRESS(RCT2_ADDRESS_SCENERY_SET_ENTRIES, rct_scenery_set_entry*)

void init_scenery();

#endif