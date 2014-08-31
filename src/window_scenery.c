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

#include <string.h>
#include <stdbool.h>
#include "addresses.h"
#include "game.h"
#include "map.h"
#include "gfx.h"
#include "peep.h"
#include "sprite.h"
#include "string_ids.h"
#include "viewport.h"
#include "widget.h"
#include "window.h"

#define WINDOW_SCENERY_WIDTH 0x27A
#define WINDOW_SCENERY_HEIGHT 0x8E

typedef struct {
	uint8 pad_02[4];
	uint8 flags;		// 0x06
	uint8 flags2;		// 0x07
	uint8 pad_08[4];
	uint16 price;		// 0x0C
	uint8 pad_0E[12];
	uint8 var_1A;		// 0x1A
} rct_small_scenery_entry;

typedef struct {
	uint8 pad_02[5];
	uint8 flags;		// 0x07
	uint16 price;		// 0x08
	uint8 pad_0A[6];
	uint8 var_10;		// 0x10
} rct_large_scenery_entry;


typedef struct {
	uint8 pad_02[5];
	uint8 flags;		// 0x07
	uint8 pad_08;
	uint8 flags2;		// 0x09
	uint16 price;		// 0x0A
	uint8 var_0C;		// 0x0C
} rct_wall_scenery_entry;

typedef struct {
	uint8 pad_02[8];
	uint16 price;		// 0x0A
	uint8 var_0C;		// 0x0C
} rct_path_bit_scenery_entry;

typedef struct {
	uint16 var_02;
	uint16 var_04;
	uint8 var_06;
	uint8 flags;		// 0x07
	uint16 price;		// 0x08
	uint8 var_0A;		// 0x0A
} rct_banner_scenery_entry;

typedef struct {
	rct_string_id name;
	union {
		rct_small_scenery_entry small_scenery;
		rct_large_scenery_entry large_scenery;
		rct_wall_scenery_entry wall;
		rct_path_bit_scenery_entry path_bit;
		rct_banner_scenery_entry banner;
	};
	/*uint16 var_02;
	uint16 var_04;
	uint8 var_06;
	uint8 var_07;
	uint8 var_08;
	uint8 var_09;
	uint16 var_0A;
	uint16 var_0C;
	uint16 var_0E;
	uint16 var_10;
	uint16 var_12;
	uint16 var_14;
	uint16 var_16;
	uint16 var_18;
	uint16 var_1A;*/
} rct_scenery_entry;

#define g_smallSceneryEntries RCT2_ADDRESS(RCT2_ADDRESS_SMALL_SCENERY_ENTRIES, rct_scenery_entry*)
#define g_largeSceneryEntries RCT2_ADDRESS(RCT2_ADDRESS_LARGE_SCENERY_ENTRIES, rct_scenery_entry*)
#define g_wallSceneryEntries RCT2_ADDRESS(RCT2_ADDRESS_WALL_SCENERY_ENTRIES, rct_scenery_entry*)
#define g_bannerSceneryEntries RCT2_ADDRESS(RCT2_ADDRESS_BANNER_SCENERY_ENTRIES, rct_scenery_entry*)
#define g_pathBitSceneryEntries RCT2_ADDRESS(RCT2_ADDRESS_PATH_BIT_SCENERY_ENTRIES, rct_scenery_entry*)

enum {
	WINDOW_SCENERY_TAB_1,
	WINDOW_SCENERY_TAB_2,
	WINDOW_SCENERY_TAB_3,
	WINDOW_SCENERY_TAB_4,
	WINDOW_SCENERY_TAB_5,
	WINDOW_SCENERY_TAB_6,
	WINDOW_SCENERY_TAB_7,
	WINDOW_SCENERY_TAB_8,
	WINDOW_SCENERY_TAB_9,
	WINDOW_SCENERY_TAB_10,
	WINDOW_SCENERY_TAB_11,
	WINDOW_SCENERY_TAB_12,
	WINDOW_SCENERY_TAB_13,
	WINDOW_SCENERY_TAB_14,
	WINDOW_SCENERY_TAB_15,
	WINDOW_SCENERY_TAB_16,
	WINDOW_SCENERY_TAB_17,
	WINDOW_SCENERY_TAB_18,
	WINDOW_SCENERY_TAB_19,
	WINDOW_SCENERY_TAB_20
} WINDOW_SCENERY_LIST_TAB;

static void window_scenery_emptysub() { }
static void window_scenery_close();
static void window_scenery_update(rct_window *w);
static void window_scenery_invalidate();
static void window_scenery_paint();
static void window_scenery_tooltip();

static void* window_scenery_events[] = {
	window_scenery_close, //(void*)0x006E1A73,    // window_scenery_close
	(void*)0x006E19FC,    // window_scenery_mouseup
	(void*)0x006E1E48,    // window_scenery_resize,
	(void*)0x006E1A25,    // window_scenery_mousedown,
	(void*)0x006E1A54,    // window_scenery_dropdown,
	window_scenery_emptysub,
	/*window_scenery_update,/*/(void*)0x006E1CD3,    // window_scenery_update,
	(void*)0x006E1B9F,	  // window_scenery_emptysub,
	window_scenery_emptysub,
	window_scenery_emptysub,
	window_scenery_emptysub,			   // window_scenery_tooldown
	window_scenery_emptysub,
	window_scenery_emptysub,
	window_scenery_emptysub,
	window_scenery_emptysub,
	(void*)0x006E1A91,	  // window_scenery_scrollgetsize,
	(void*)0x006E1C4A,	  // window_scenery_scrollmousedown,
	window_scenery_emptysub,
	(void*)0x006E1BB8,	  // window_scenery_scrollmouseover,
	window_scenery_emptysub,
	window_scenery_emptysub,
	window_scenery_emptysub,
	window_scenery_tooltip, //(void*)0x006E1C05,	  // window_scenery_tooltip,
	window_scenery_emptysub,
	window_scenery_emptysub,
	window_scenery_invalidate, //(void*)0x006E118B,	  // window_scenery_invalidate,
	window_scenery_paint,	  // (void*)0x006E1462,	  // window_scenery_paint, 
	(void*)0x006E15ED,	  // window_scenery_scrollpaint,
};


enum WINDOW_SCENERY_LIST_WIDGET_IDX {
	WIDX_SCENERY_BACKGROUND,				// 1
	WIDX_SCENERY_TITLE,						// 2
	WIDX_SCENERY_CLOSE,						// 4
	WIDX_SCENERY_TAB_CONTENT_PANEL,			// 8
	WIDX_SCENERY_TAB_1,						// 10
	WIDX_SCENERY_TAB_2,						// 20
	WIDX_SCENERY_TAB_3,						// 40
	WIDX_SCENERY_TAB_4,						// 80
	WIDX_SCENERY_TAB_5,						// 100
	WIDX_SCENERY_TAB_6,						// 200
	WIDX_SCENERY_TAB_7,						// 400
	WIDX_SCENERY_TAB_8,						// 800
	WIDX_SCENERY_TAB_9,						// 1000
	WIDX_SCENERY_TAB_10,					// 2000
	WIDX_SCENERY_TAB_11,					// 4000
	WIDX_SCENERY_TAB_12,					// 8000
	WIDX_SCENERY_TAB_13,					// 10000
	WIDX_SCENERY_TAB_14,					// 20000
	WIDX_SCENERY_TAB_15,					// 40000
	WIDX_SCENERY_TAB_16,					// 80000
	WIDX_SCENERY_TAB_17,					// 100000
	WIDX_SCENERY_TAB_18,					// 200000
	WIDX_SCENERY_TAB_19,					// 400000
	WIDX_SCENERY_TAB_20,					// 800000
	WIDX_SCENERY_LIST,						// 1000000
	WIDX_SCENERY_FLATBUTTON1,				// 2000000
	WIDX_SCENERY_FLATBUTTON2,				// 4000000
	WIDX_SCENERY_COLORBUTTON1,				// 8000000
	WIDX_SCENERY_COLORBUTTON2,				// 10000000
	WIDX_SCENERY_COLORBUTTON3,				// 20000000
	WIDX_SCENERY_FLATBUTTON3,				// 40000000
};

static rct_widget window_scenery_widgets[] = {
	{ WWT_FRAME, 0, 0, 633, 0, 141, 0xFFFFFFFF, STR_NONE },								// 1				0x009DE298
	{ WWT_CAPTION, 0, 1, 632, 1, 14, 0xFFFFFFFF, STR_WINDOW_TITLE_TIP },				// 2				0x009DE2A8
	{ WWT_CLOSEBOX, 0, 621, 631, 2, 13, STR_CLOSE_X, STR_CLOSE_WINDOW_TIP },			// 4				0x009DE2B8
	{ WWT_RESIZE, 1, 0, 633, 43, 141, 0xFFFFFFFF, STR_NONE },							// 8				0x009DE2C8
	{ WWT_TAB, 1, 3, 33, 17, 43, 0xFFFFFFFF, 1812 },									// 10				0x009DE2D8
	{ WWT_TAB, 1, 34, 64, 17, 43, 0xFFFFFFFF, 1812 },									// 20				0x009DE2E8
	{ WWT_TAB, 1, 65, 95, 17, 43, 0xFFFFFFFF, 1812 },									// 40				0x009DE2F8
	{ WWT_TAB, 1, 96, 126, 17, 43, 0xFFFFFFFF, 1812 },									// 80				0x009DE308
	{ WWT_TAB, 1, 127, 157, 17, 43, 0xFFFFFFFF, 1812 },									// 100				0x009DE318
	{ WWT_TAB, 1, 158, 188, 17, 43, 0xFFFFFFFF, 1812 },									// 200				0x009DE328
	{ WWT_TAB, 1, 189, 219, 17, 43, 0xFFFFFFFF, 1812 },									// 400				0x009DE338
	{ WWT_TAB, 1, 220, 250, 17, 43, 0xFFFFFFFF, 1812 },									// 800				0x009DE348
	{ WWT_TAB, 1, 251, 281, 17, 43, 0xFFFFFFFF, 1812 },									// 1000				0x009DE358
	{ WWT_TAB, 1, 282, 312, 17, 43, 0xFFFFFFFF, 1812 },									// 2000				0x009DE368
	{ WWT_TAB, 1, 313, 343, 17, 43, 0xFFFFFFFF, 1812 },									// 4000				0x009DE378
	{ WWT_TAB, 1, 344, 374, 17, 43, 0xFFFFFFFF, 1812 },									// 8000				0x009DE388
	{ WWT_TAB, 1, 375, 405, 17, 43, 0xFFFFFFFF, 1812 },									// 10000			0x009DE398
	{ WWT_TAB, 1, 406, 436, 17, 43, 0xFFFFFFFF, 1812 },									// 20000			0x009DE3A8
	{ WWT_TAB, 1, 437, 467, 17, 43, 0xFFFFFFFF, 1812 },									// 40000			0x009DE3B8
	{ WWT_TAB, 1, 468, 498, 17, 43, 0xFFFFFFFF, 1812 },									// 80000			0x009DE3C8
	{ WWT_TAB, 1, 468, 498, 17, 43, 0xFFFFFFFF, 1812 },									// 100000			0x009DE3D8
	{ WWT_TAB, 1, 468, 498, 17, 43, 0xFFFFFFFF, 1812 },									// 200000			0x009DE3E8
	{ WWT_TAB, 1, 468, 498, 17, 43, 0xFFFFFFFF, 1812 },									// 400000			0x009DE3F8
	{ WWT_TAB, 1, 468, 498, 17, 43, 0x20001598, 1812 },									// 800000			0x009DE408
	{ WWT_SCROLL, 1, 2, 608, 47, 126, 2, STR_NONE },									// 1000000			0x009DE418
	{ WWT_FLATBTN, 1, 609, 632, 44, 67, 5169, STR_ROTATE_OBJECTS_90 },					// 2000000			0x009DE428
	{ WWT_FLATBTN, 1, 609, 632, 68, 91, 5173, 3102 },									// 4000000			0x009DE438
	{ WWT_COLORBTN, 1, 615, 626, 93, 104, 0xFFFFFFFF, STR_SELECT_COLOUR },				// 8000000			0x009DE448
	{ WWT_COLORBTN, 1, 615, 626, 105, 116, 0xFFFFFFFF, STR_SELECT_SECONDARY_COLOUR },	// 10000000			0x009DE458
	{ WWT_COLORBTN, 1, 615, 626, 117, 128, 0xFFFFFFFF, STR_SELECT_TERNARY_COLOUR },		// 20000000			0x009DE468
	{ WWT_FLATBTN, 1, 609, 632, 117, 140, 5172, 3225 },									// 40000000			0x009DE478
	{ WIDGETS_END },
};

/*
* rct2: 0x006DFA00
**/
void init_scenery() {
	uint32 edi = 0;
	uint32 baseAddress = 0x00F64F7C;

	for (int i = 0; i < 0x14; i++) {
		RCT2_ADDRESS(0x00F64F2C, uint32)[i] = baseAddress;
		baseAddress += 0x102;
	}

	uint32 dword_F663A4 = 0;

	for (int edi = 0; edi < 0x14; edi++) {
		int esi = RCT2_ADDRESS(0x00F64F2C, uint32)[edi];
		RCT2_GLOBAL(esi, uint16) = 0xFFFF;
		if (edi == 13)
			continue;

		uint32 ebp = RCT2_ADDRESS(RCT2_ADDRESS_SCENERY_SET_ENTRIES, uint32)[edi];
		if (ebp == 0xFFFFFFFF)
			continue;

		for (int ebx = 0;; ebx++) {
			if (ebx >= RCT2_GLOBAL(ebp + 0x106, uint8))
				break;

			uint16 ax = RCT2_GLOBAL(ebp + ebx * 2 + 6, uint16);

			uint32 ecx = RCT2_ADDRESS(0x01357BD0, uint32)[ax >> 5];
			uint32 edx = 1 << (ax & 0x1F);
			if (ecx & edx) {
				RCT2_GLOBAL(esi, uint16) = ax;
				esi += 2;
				RCT2_GLOBAL(esi, uint16) = 0xFFFF;
			} else {
				dword_F663A4 |= (1 << edi);
			}
		}
	}

	// small scenery 
	for (uint16 edi = 0; edi < 0xFC; edi++) {
		if ((uint32)g_smallSceneryEntries[edi] == 0xFFFFFFFF)
			continue;

		rct_scenery_entry* sceneryEntry = g_smallSceneryEntries[edi];

		if (RCT2_ADDRESS(0x01357BD0, sint32)[edi >> 5] & (1 << (edi & 0x1F))) {
			if (sceneryEntry->small_scenery.var_1A != 0xFF) {
				uint32 esi = RCT2_ADDRESS(0x00F64F2C, uint32)[sceneryEntry->small_scenery.var_1A];

				bool found = false;
				for (int ebx = 0; ebx < 0x80; ebx++) {
					if (RCT2_ADDRESS(esi, uint16)[ebx] == 0xFFFF)
					{
						RCT2_ADDRESS(esi, uint16)[ebx] = edi;
						RCT2_ADDRESS(esi, uint16)[ebx + 1] = 0xFFFF;
						found = true;
						break;
					}
				}

				if (found == true)
					continue;
			}

			bool found2 = false;

			for (int ecx = 0; ecx < 0x13; ecx++) {
				int baseEdx = RCT2_ADDRESS(0x00F64F2C, uint32)[ecx];
				int counter = 0;

				while (RCT2_ADDRESS(baseEdx, uint16)[counter] != 0xFFFF)
				{
					if (RCT2_ADDRESS(baseEdx, uint16)[counter] == edi) {
						ecx = 0x13;
						found2 = true;
						break;
					}

					counter++;
				}
			}

			if (found2 == true)
				continue;

			uint32 edx = RCT2_GLOBAL(0x00F64F78, uint32);
			for (int ecx = 0; ecx < 0x80; ecx++) {
				if (RCT2_ADDRESS(edx, uint16)[ecx] == 0xFFFF)
				{
					RCT2_ADDRESS(edx, uint16)[ecx] = edi;
					RCT2_ADDRESS(edx, uint16)[ecx + 1] = 0xFFFF;
					break;
				}
			}
		}
	}

	// large scenery
	for (int edi = 0x300; edi < 0x380; edi++) {
		int largeSceneryIndex = edi - 0x300;

		if ((uint32)g_largeSceneryEntries[largeSceneryIndex] == 0xFFFFFFFF)
			continue;

		rct_scenery_entry* sceneryEntry = g_largeSceneryEntries[largeSceneryIndex];

		if (RCT2_ADDRESS(0x01357BD0, sint32)[edi >> 5] & (1 << (edi + 0x1F))) {
			if (sceneryEntry->large_scenery.var_10 != 0xFF) {
				uint32 esi = RCT2_ADDRESS(0x00F64F2C, uint32)[sceneryEntry->large_scenery.var_10];

				bool found = false;
				for (int ebx = 0; ebx < 0x80; ebx++) {
					if (RCT2_ADDRESS(esi, uint16)[ebx] == 0xFFFF)
					{
						RCT2_ADDRESS(esi, uint16)[ebx] = edi;
						RCT2_ADDRESS(esi, uint16)[ebx + 1] = 0xFFFF;
						found = true;
						break;
					}
				}

				if (found == true)
					continue;
			}

			bool found2 = false;

			for (int ecx = 0; ecx < 0x13; ecx++) {
				int baseEdx = RCT2_ADDRESS(0x00F64F2C, uint32)[ecx];
				int counter = 0;

				while (RCT2_ADDRESS(baseEdx, uint16)[counter] != 0xFFFF)
				{
					if (RCT2_ADDRESS(baseEdx, uint16)[counter] == edi) {
						ecx = 0x13;
						found2 = true;
						break;
					}

					counter++;
				}
			}

			if (found2 == true)
				continue;

			uint32 edx = RCT2_GLOBAL(0x00F64F78, uint32);
			for (int ecx = 0; ecx < 0x80; ecx++) {
				if (RCT2_ADDRESS(edx, uint16)[ecx] == 0xFFFF)
				{
					RCT2_ADDRESS(edx, uint16)[ecx] = edi;
					RCT2_ADDRESS(edx, uint16)[ecx + 1] = 0xFFFF;
					break;
				}
			}
		}
	}

	// walls
	for (int edi = 0x200; edi < 0x280; edi++) {
		int wallSceneryIndex = edi - 0x200;

		if ((uint32)g_wallSceneryEntries[wallSceneryIndex] == 0xFFFFFFFF)
			continue;

		rct_scenery_entry* sceneryEntry = g_wallSceneryEntries[wallSceneryIndex];

		if (RCT2_ADDRESS(0x01357BD0, sint32)[edi >> 5] & (1 << (edi + 0x1F))) {
			if (sceneryEntry->wall.var_0C != 0xFF) {
				uint32 esi = RCT2_ADDRESS(0x00F64F2C, uint32)[sceneryEntry->wall.var_0C];

				bool found = false;
				for (int ebx = 0; ebx < 0x80; ebx++) {
					if (RCT2_ADDRESS(esi, uint16)[ebx] == 0xFFFF)
					{
						RCT2_ADDRESS(esi, uint16)[ebx] = edi;
						RCT2_ADDRESS(esi, uint16)[ebx + 1] = 0xFFFF;
						found = true;
						break;
					}
				}

				if (found == true)
					continue;
			}

			bool found2 = false;

			for (int ecx = 0; ecx < 0x13; ecx++) {
				int baseEdx = RCT2_ADDRESS(0x00F64F2C, uint32)[ecx];
				int counter = 0;

				while (RCT2_ADDRESS(baseEdx, uint16)[counter] != 0xFFFF)
				{
					if (RCT2_ADDRESS(baseEdx, uint16)[counter] == edi) {
						ecx = 0x13;
						found2 = true;
						break;
					}

					counter++;
				}
			}

			if (found2 == true)
				continue;

			uint32 edx = RCT2_GLOBAL(0x00F64F78, uint32);
			for (int ecx = 0; ecx < 0x80; ecx++) {
				if (RCT2_ADDRESS(edx, uint16)[ecx] == 0xFFFF)
				{
					RCT2_ADDRESS(edx, uint16)[ecx] = edi;
					RCT2_ADDRESS(edx, uint16)[ecx + 1] = 0xFFFF;
					break;
				}
			}
		}
	}

	// banners
	for (int edi = 0x400; edi < 0x420; edi++) {
		int bannerIndex = edi - 0x400;

		if ((uint32)g_bannerSceneryEntries[bannerIndex] == 0xFFFFFFFF)
			continue;

		rct_scenery_entry* sceneryEntry = g_bannerSceneryEntries[bannerIndex];

		if (RCT2_ADDRESS(0x01357BD0, sint32)[edi >> 5] & (1 << (edi + 0x1F))) {
			if (sceneryEntry->banner.var_0A != 0xFF) {
				uint32 esi = RCT2_ADDRESS(0x00F64F2C, uint32)[sceneryEntry->banner.var_0A];

				bool found = false;
				for (int ebx = 0; ebx < 0x80; ebx++) {
					if (RCT2_ADDRESS(esi, uint16)[ebx] == 0xFFFF)
					{
						RCT2_ADDRESS(esi, uint16)[ebx] = edi;
						RCT2_ADDRESS(esi, uint16)[ebx + 1] = 0xFFFF;
						found = true;
						break;
					}
				}

				if (found == true)
					continue;
			}

			bool found2 = false;

			for (int ecx = 0; ecx < 0x13; ecx++) {
				int baseEdx = RCT2_ADDRESS(0x00F64F2C, uint32)[ecx];
				int counter = 0;

				while (RCT2_ADDRESS(baseEdx, uint16)[counter] != 0xFFFF)
				{
					if (RCT2_ADDRESS(baseEdx, uint16)[counter] == edi) {
						ecx = 0x13;
						found2 = true;
						break;
					}

					counter++;
				}
			}

			if (found2 == true)
				continue;

			uint32 edx = RCT2_GLOBAL(0x00F64F78, uint32);
			for (int ecx = 0; ecx < 0x80; ecx++) {
				if (RCT2_ADDRESS(edx, uint16)[ecx] == 0xFFFF)
				{
					RCT2_ADDRESS(edx, uint16)[ecx] = edi;
					RCT2_ADDRESS(edx, uint16)[ecx + 1] = 0xFFFF;
					break;
				}
			}
		}
	}

	// path bits
	for (int edi = 0x100; edi < 0x10F; edi++) {
		int bannerIndex = edi - 0x100;

		if ((uint32)g_pathBitSceneryEntries[bannerIndex] == 0xFFFFFFFF)
			continue;

		rct_scenery_entry* sceneryEntry = g_pathBitSceneryEntries[bannerIndex];

		if (RCT2_ADDRESS(0x01357BD0, sint32)[edi >> 5] & (1 << (edi + 0x1F))) {
			if (sceneryEntry->path_bit.var_0C != 0xFF) {
				uint32 esi = RCT2_ADDRESS(0x00F64F2C, uint32)[sceneryEntry->path_bit.var_0C];

				bool found = false;
				for (int ebx = 0; ebx < 0x80; ebx++) {
					if (RCT2_ADDRESS(esi, uint16)[ebx] == 0xFFFF)
					{
						RCT2_ADDRESS(esi, uint16)[ebx] = edi;
						RCT2_ADDRESS(esi, uint16)[ebx + 1] = 0xFFFF;
						found = true;
						break;
					}
				}

				if (found == true)
					continue;
			}

			bool found2 = false;

			for (int ecx = 0; ecx < 0x13; ecx++) {
				int baseEdx = RCT2_ADDRESS(0x00F64F2C, uint32)[ecx];
				int counter = 0;

				while (RCT2_ADDRESS(baseEdx, uint16)[counter] != 0xFFFF)
				{
					if (RCT2_ADDRESS(baseEdx, uint16)[counter] == edi) {
						ecx = 0x13;
						found2 = true;
						break;
					}

					counter++;
				}
			}

			if (found2 == true)
				continue;

			uint32 edx = RCT2_GLOBAL(0x00F64F78, uint32);
			for (int ecx = 0; ecx < 0x80; ecx++) {
				if (RCT2_ADDRESS(edx, uint16)[ecx] == 0xFFFF)
				{
					RCT2_ADDRESS(edx, uint16)[ecx] = edi;
					RCT2_ADDRESS(edx, uint16)[ecx + 1] = 0xFFFF;
					break;
				}
			}
		}
	}

	for (int widgetIndex = WIDX_SCENERY_TAB_1; widgetIndex < WIDX_SCENERY_LIST; widgetIndex++)
		window_scenery_widgets[widgetIndex].type = 0;

	uint8 stuff[0x50];
	int ebp = 0;

	for (int ebx = 0; ebx < 0x13; ebx++) {
		uint32 scenerySetAddress = RCT2_ADDRESS(RCT2_ADDRESS_SCENERY_SET_ENTRIES, uint32)[edi];
		if (scenerySetAddress == 0xFFFFFFFF)
			continue;

		stuff[ebp] = ebx;
		stuff[ebp + 1] = RCT2_GLOBAL(scenerySetAddress + 0x108, uint8);

		ebp += 4;
		int prev_edi = ebp - 4;

		while (true) {
			if (prev_edi <= 0)
				break;

			prev_edi -= 4;
			if (stuff[prev_edi + 5] >= stuff[prev_edi + 1])
				break;

			int eax = stuff[prev_edi + 4];
			stuff[prev_edi + 4] = stuff[prev_edi];
			stuff[prev_edi] = eax;
		}
	}

	stuff[ebp] = 0x13;
	ebp += 4;

	uint16 cx = 3;
	for (int ebx = 0; ebx < ebp; ebx += 4) {
		uint32 tabIndex = stuff[ebx];
		rct_widget* currentTab = &window_scenery_widgets[tabIndex + WIDX_SCENERY_TAB_1];
		int eax = RCT2_ADDRESS(0x00F64F2C, uint32)[tabIndex];

		if (cx != 3 || tabIndex == 0x13) {
			if (RCT2_GLOBAL(eax, uint16) == 0xFFFF)
				continue;

			if (dword_F663A4 & (1 << tabIndex))
				continue;
		}

		currentTab->type = WWT_TAB;
		currentTab->left = cx;
		currentTab->right = cx + 0x1E;
		cx += 0x1F;

		if (edi >= 0x13)
			continue;

		int sceneryEntry = RCT2_ADDRESS(RCT2_ADDRESS_SCENERY_SET_ENTRIES, uint32)[tabIndex];
		currentTab->image = RCT2_GLOBAL(sceneryEntry + 2, uint32) | 0x20000000;
	}

	window_invalidate_by_id(WC_SCENERY, 0);
}

/*
* rct2: 0x006E0FEF
**/
void window_scenery_open()
{
	rct_window* window;

	// Check if window is already open
	window = window_bring_to_front_by_id(WC_SCENERY, 0);
	if (window != NULL)
		return;

	init_scenery();
	//RCT2_CALLPROC_EBPSAFE(0x006DFA00);

	window = window_create(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_WIDTH, sint16) - WINDOW_SCENERY_WIDTH, 0x1D, WINDOW_SCENERY_WIDTH, WINDOW_SCENERY_HEIGHT,
		(uint32*)window_scenery_events, WC_SCENERY, WF_2);
	window->widgets = window_scenery_widgets;

	window->enabled_widgets =
		(1 << WIDX_SCENERY_CLOSE) |
		(1 << WIDX_SCENERY_FLATBUTTON1) |
		(1 << WIDX_SCENERY_TAB_1) |
		(1 << WIDX_SCENERY_TAB_2) |
		(1 << WIDX_SCENERY_TAB_3) |
		(1 << WIDX_SCENERY_TAB_4) |
		(1 << WIDX_SCENERY_TAB_5) |
		(1 << WIDX_SCENERY_TAB_6) |
		(1 << WIDX_SCENERY_TAB_7) |
		(1 << WIDX_SCENERY_TAB_8) |
		(1 << WIDX_SCENERY_TAB_9) |
		(1 << WIDX_SCENERY_TAB_10) |
		(1 << WIDX_SCENERY_TAB_11) |
		(1 << WIDX_SCENERY_TAB_12) |
		(1 << WIDX_SCENERY_TAB_13) |
		(1 << WIDX_SCENERY_TAB_14) |
		(1 << WIDX_SCENERY_TAB_15) |
		(1 << WIDX_SCENERY_TAB_16) |
		(1 << WIDX_SCENERY_TAB_17) |
		(1 << WIDX_SCENERY_TAB_18) |
		(1 << WIDX_SCENERY_TAB_19) |
		(1 << WIDX_SCENERY_TAB_20) |
		(1 << WIDX_SCENERY_COLORBUTTON1) |
		(1 << WIDX_SCENERY_COLORBUTTON2) |
		(1 << WIDX_SCENERY_FLATBUTTON2) |
		(1 << WIDX_SCENERY_COLORBUTTON3) |
		(1 << WIDX_SCENERY_FLATBUTTON3);

	window_init_scroll_widgets(window);
	RCT2_CALLPROC_X(0x006E1EB4, 0, 0, 0, 0, (int)window, 0, 0);
	show_gridlines();
	RCT2_GLOBAL(0x00F64F05, uint8) = 3;
	RCT2_GLOBAL(0x00F64F12, uint8) = 0;
	RCT2_GLOBAL(0x00F64F13, uint8) = 0;
	window->scenery.var_480 = 0xFFFF;
	window->scenery.var_482 = 0;
	window_push_others_below(window);
	RCT2_GLOBAL(0x00F64F0D, uint8) = 0;
	RCT2_GLOBAL(0x00F64EB4, uint32) = 0x80000000;
	RCT2_GLOBAL(0x00F64EC0, uint16) = 0;
	RCT2_GLOBAL(0x00F64F19, uint8) = 0; // repaint colored scenery tool state
	RCT2_GLOBAL(0x00F64F1A, uint8) = 0;

	window->min_width = WINDOW_SCENERY_WIDTH;
	window->max_width = WINDOW_SCENERY_WIDTH;
	window->min_height = WINDOW_SCENERY_HEIGHT;
	window->max_height = WINDOW_SCENERY_HEIGHT;
	window->colours[0] = 0x18;
	window->colours[1] = 0x0C;
	window->colours[2] = 0x0C;
}

/*
* rct2: 0x006E1A73
**/
void window_scenery_close() {
	rct_window *w;

	window_get_register(w);

	RCT2_CALLPROC_EBPSAFE(0x6E2712);
	hide_gridlines();
	RCT2_CALLPROC_X(0x006CB70A, 0, 0, 0, 0, 0, 0, 0);

	int toolWindowClassification = RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WINDOWCLASS, rct_windowclass);
	int toolWidgetIndex = RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WIDGETINDEX, rct_windownumber);

	if (RCT2_GLOBAL(0x009DE518, uint32) & (1 << 3))
		if (toolWindowClassification == WC_TOP_TOOLBAR && toolWidgetIndex == 9)
			tool_cancel();
}

/**
*
*  rct2: 0x006E1CD3
*/
static void window_scenery_update(rct_window *w)
{
	rct_window* other = window_find_from_point(RCT2_GLOBAL(0x0142406C, int), RCT2_GLOBAL(0x01424070, int));
	if (other == w) {
		int window_x = RCT2_GLOBAL(0x0142406C, int) - w->x + 0x1A;
		int window_y = RCT2_GLOBAL(0x01424070, int) - w->y;

		if (window_y >= 0x2C) {

		}
	}
}

/**
*
*  rct2: 0x006E1C05
*/
void window_scenery_tooltip() {
	uint16 tooltipIndex;

	#ifdef _MSC_VER
	__asm mov tooltipIndex, ax
	#else
	__asm__("mov %[tooltipIndex], ax " : [tooltipIndex] "+m" (tooltipIndex));
	#endif

	if (tooltipIndex == 0x18)
	{
		RCT2_GLOBAL(0x013CE952, uint16) = 3159;
	}
	else if (tooltipIndex >= 4 && tooltipIndex < 0x17)
	{
		uint32* sceneryEntry = RCT2_ADDRESS(RCT2_ADDRESS_SCENERY_SET_ENTRIES, uint32*)[tooltipIndex - 4];
		RCT2_GLOBAL(0x013CE952, uint16) = RCT2_GLOBAL((int)sceneryEntry, uint16);
	}
}

/**
*
*  rct2: 0x006E118B
*/
void window_scenery_invalidate() {
	rct_window* w;

	window_get_register(w);

	uint16 typeId = RCT2_GLOBAL(0x00F64EDC, uint8);
	uint32 edx = 0x715;
	if (typeId >= 0x13) {
		edx = *RCT2_GLOBAL(RCT2_ADDRESS_SCENERY_SET_ENTRIES + (typeId & 0xFFFF) * 4, uint8*);
	}

	w->pressed_widgets = (((uint32)w->pressed_widgets & 0xFF00000F) | (1 << (typeId + 4))) & 0xBBFFFFFF;

	if (RCT2_GLOBAL(0x00F64F19, uint8) != 1) { // repaint colored scenery tool is off
		w->pressed_widgets |= 0x04000000;
	}

	uint8 byte_9DE428 = 0;
	uint8 byte_9DE478 = 0;
	uint8 byte_9DE448 = 0;

	uint16 bp = RCT2_ADDRESS(0x00F64EDD, uint16)[typeId];
	if (bp == 0xFFFF) {
		if (bp > 0x100) {
			if (!(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_SCENARIO_EDITOR) && !(RCT2_GLOBAL(0x00F64F19, uint8) & 1))  {
				byte_9DE478 = 9;
			}

			rct_scenery_entry* ebx = g_smallSceneryEntries[typeId];
			if (!(ebx->small_scenery.flags & 0x600)) {
				byte_9DE448 = 6;
			}
			int i = 0;
		}
	}

	window_scenery_widgets[WIDX_SCENERY_COLORBUTTON1].image =
		(RCT2_GLOBAL(0x00F64F06, uint8) << 19) + 0x600013C3;
	window_scenery_widgets[WIDX_SCENERY_COLORBUTTON2].image =
		(RCT2_GLOBAL(0x00F64F07, uint8) << 19) + 0x600013C3;
	window_scenery_widgets[WIDX_SCENERY_COLORBUTTON3].image =
		(RCT2_GLOBAL(0x00F64F08, uint8) << 19) + 0x600013C3;

	window_scenery_widgets[WIDX_SCENERY_COLORBUTTON1].type = WWT_EMPTY;
	window_scenery_widgets[WIDX_SCENERY_COLORBUTTON2].type = WWT_EMPTY;
	window_scenery_widgets[WIDX_SCENERY_COLORBUTTON3].type = WWT_EMPTY;

	
	if (RCT2_GLOBAL(0x00F64F19, uint8) & 1) { // repaint colored scenery tool is on
		window_scenery_widgets[WIDX_SCENERY_COLORBUTTON1].type = WWT_COLORBTN;
		window_scenery_widgets[WIDX_SCENERY_COLORBUTTON2].type = WWT_COLORBTN;
		window_scenery_widgets[WIDX_SCENERY_COLORBUTTON3].type = WWT_COLORBTN;
	} else if (bp != 0xFFFF) {
		rct_scenery_entry* sceneryEntry = NULL;

		if (bp >= 0x400) {
			sceneryEntry = g_bannerSceneryEntries[bp - 0x400];
			
			if (sceneryEntry->banner.flags & 1)
				window_scenery_widgets[WIDX_SCENERY_COLORBUTTON1].type = WWT_COLORBTN;
		}
		else if (bp >= 0x300) {
			sceneryEntry = g_largeSceneryEntries[bp - 0x300];

			if (sceneryEntry->large_scenery.flags & 1)
				window_scenery_widgets[WIDX_SCENERY_COLORBUTTON1].type = WWT_COLORBTN;
			if (sceneryEntry->large_scenery.flags & 2)
				window_scenery_widgets[WIDX_SCENERY_COLORBUTTON2].type = WWT_COLORBTN;
		}
		else if (bp >= 0x200) {
			sceneryEntry = g_wallSceneryEntries[bp - 0x200];
			if (sceneryEntry->wall.flags & 1)
				window_scenery_widgets[WIDX_SCENERY_COLORBUTTON1].type = WWT_COLORBTN;
			if (sceneryEntry->wall.flags & 0x40) {
				window_scenery_widgets[WIDX_SCENERY_COLORBUTTON2].type = WWT_COLORBTN;

				if (sceneryEntry->wall.flags2 & 1)
					window_scenery_widgets[WIDX_SCENERY_COLORBUTTON1].type = WWT_COLORBTN;
				if (sceneryEntry->wall.flags & 0x80)
					window_scenery_widgets[WIDX_SCENERY_COLORBUTTON3].type = WWT_COLORBTN;
			}
		}
		else if (bp < 0x100) {
			sceneryEntry = g_smallSceneryEntries[bp];
			
			if (sceneryEntry->small_scenery.flags & 0x600) {
				window_scenery_widgets[WIDX_SCENERY_COLORBUTTON1].type = WWT_COLORBTN;

				if (sceneryEntry->small_scenery.flags2 & 0x8)
					window_scenery_widgets[WIDX_SCENERY_COLORBUTTON2].type = WWT_COLORBTN;
			}
		}
	}

	window_scenery_widgets[WIDX_SCENERY_BACKGROUND].right = w->width - 1;
	window_scenery_widgets[WIDX_SCENERY_BACKGROUND].bottom = w->height - 1;
	window_scenery_widgets[WIDX_SCENERY_TAB_CONTENT_PANEL].right = w->width - 1;
	window_scenery_widgets[WIDX_SCENERY_TAB_CONTENT_PANEL].bottom = w->height - 1;
	window_scenery_widgets[WIDX_SCENERY_TITLE].right = w->width - 2;
	window_scenery_widgets[WIDX_SCENERY_CLOSE].left = w->width - 13;
	window_scenery_widgets[WIDX_SCENERY_CLOSE].right = window_scenery_widgets[WIDX_SCENERY_CLOSE].left + 10;
	window_scenery_widgets[WIDX_SCENERY_LIST].right = w->width - 0x1A;
	window_scenery_widgets[WIDX_SCENERY_LIST].bottom = w->height - 0x0E;

	window_scenery_widgets[WIDX_SCENERY_FLATBUTTON1].left = w->width - 25;
	window_scenery_widgets[WIDX_SCENERY_FLATBUTTON2].left = w->width - 25;
	window_scenery_widgets[WIDX_SCENERY_FLATBUTTON3].left = w->width - 25;
	window_scenery_widgets[WIDX_SCENERY_FLATBUTTON1].right = w->width - 2;
	window_scenery_widgets[WIDX_SCENERY_FLATBUTTON2].right = w->width - 2;
	window_scenery_widgets[WIDX_SCENERY_FLATBUTTON3].right = w->width - 2;

	window_scenery_widgets[WIDX_SCENERY_COLORBUTTON1].left = w->width - 19;
	window_scenery_widgets[WIDX_SCENERY_COLORBUTTON2].left = w->width - 19;
	window_scenery_widgets[WIDX_SCENERY_COLORBUTTON3].left = w->width - 19;
	window_scenery_widgets[WIDX_SCENERY_COLORBUTTON1].right = w->width - 8;
	window_scenery_widgets[WIDX_SCENERY_COLORBUTTON2].right = w->width - 8;
	window_scenery_widgets[WIDX_SCENERY_COLORBUTTON3].right = w->width - 8;
}

/**
*
*  rct2: 0x006E1462
*/
void window_scenery_paint() {
	rct_window *w;
	rct_drawpixelinfo *dpi;

	window_paint_get_registers(w, dpi);

	window_draw_widgets(w, dpi);

	uint16 typeId = RCT2_GLOBAL(0x00F64EDC, uint8);
	uint16 selectedTab = typeId + 4;
	uint32 image_id = ((w->colours[1] << 19) | window_scenery_widgets[selectedTab].image) + 1ul;
	
	gfx_draw_sprite(dpi, image_id,
		w->x + window_scenery_widgets[selectedTab].left,
		w->y + window_scenery_widgets[selectedTab].top,
		selectedTab);
	
	uint16 bp = w->scenery.var_480;
	if (bp == 0xFFFF) {
		if (RCT2_GLOBAL(0x00F64F19, uint8) & 1)  // repaint colored scenery tool is on
			return;

		bp = RCT2_ADDRESS(0x00F64EDD, uint16)[typeId];

		if (bp == 0xFFFF)
			return;
	}

	uint32 price = 0;

	rct_scenery_entry* sceneryEntry = NULL;
	if (bp >= 0x400) {
		sceneryEntry = g_bannerSceneryEntries[bp - 0x400];
		price = sceneryEntry->banner.price;
	} else if (bp >= 0x300) {
		sceneryEntry = g_largeSceneryEntries[bp - 0x300];
		price = sceneryEntry->large_scenery.price * 10;
	} else if (bp >= 0x200) {
		sceneryEntry = g_wallSceneryEntries[bp - 0x200];
		price = sceneryEntry->wall.price;
	} else if (bp >= 0x100) {
		sceneryEntry = g_pathBitSceneryEntries[bp - 0x100];
		price = sceneryEntry->path_bit.price;
	} else {
		sceneryEntry = g_smallSceneryEntries[bp];
		price = sceneryEntry->small_scenery.price * 10;
	}

	if (w->scenery.var_480 == 0xFFFF && RCT2_GLOBAL(0x00F64EB4, uint32) != 0x80000000) {
		price = RCT2_GLOBAL(0x00F64EB4, uint32);
	}

	RCT2_GLOBAL(0x013CE952, uint32) = price;

	if (!(RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & PARK_FLAGS_NO_MONEY)) {
		gfx_draw_string_right(dpi, STR_COST_LABEL, (void*)0x013CE952, 0,
			w->x + w->width - 14, w->y + w->height - 13);
	}

	RCT2_GLOBAL(0x013CE952, uint16) = sceneryEntry->name;
	gfx_draw_string_left_clipped(dpi, 0x4A7, (void*)0x013CE952, 0,
		w->x + 3, w->y + w->height - 13, w->width - 19);
}