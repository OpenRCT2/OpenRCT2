/*****************************************************************************
 * Copyright (c) 2014 Ted John
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
 
#include "addresses.h"
#include "game.h"
#include "gfx.h"
#include "news_item.h"
#include "rct2.h"
#include "ride.h"
#include "intro.h"
#include "viewport.h"

static void title_create_windows();
static void title_init_showcase();

void window_main_open();
void window_title_menu_open();
void window_title_exit_open();
void window_title_logo_open();

/**
 *
 *  rct2: 0x0068E8DA
 */
void title_load()
{
	if (RCT2_GLOBAL(0x009DEA6E, uint8) & 1)
		RCT2_CALLPROC_X(0x00667C15, 0, 1, 0, 0, 0, 0, 0);

	RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) = SCREEN_FLAGS_TITLE_DEMO;

	RCT2_CALLPROC_EBPSAFE(0x00667104);
	RCT2_CALLPROC_EBPSAFE(0x006C4209);
	RCT2_CALLPROC_EBPSAFE(0x0069EB13);
	ride_init_all();
	RCT2_CALLPROC_EBPSAFE(0x0068F083);
	RCT2_CALLPROC_EBPSAFE(0x006BD3A4);
	RCT2_CALLPROC_EBPSAFE(0x0068AB4C);
	RCT2_CALLPROC_EBPSAFE(0x00667132);
	RCT2_CALLPROC_EBPSAFE(0x006C4494);
	RCT2_CALLPROC_X(0x006C45ED, 0, 0, 0, 0, 0, 0, 0);
	RCT2_CALLPROC_EBPSAFE(0x006DFEE4);
	RCT2_CALLPROC_EBPSAFE(0x006ACA58);
	RCT2_CALLPROC_EBPSAFE(0x0068F050);
	RCT2_CALLPROC_EBPSAFE(0x006BD39C);
	RCT2_CALLPROC_EBPSAFE(0x0068AFFD);
	RCT2_CALLPROC_EBPSAFE(0x0069EBE4);
	viewport_init_all();
	news_item_init_queue();
	title_create_windows();
	title_init_showcase();
	gfx_invalidate_screen();
	RCT2_GLOBAL(0x009DEA66, uint16) = 0;
}

/**
 * Creates the windows shown on the title screen; New game, load game,
 * tutorial, toolbox and exit.
 *  rct2: 0x0066B5C0 (part of 0x0066B3E8)
 */
static void title_create_windows()
{
	// RCT2_CALLPROC_EBPSAFE(0x0066B3E8);

	window_main_open();
	window_title_menu_open();
	window_title_exit_open();
	window_title_logo_open();
	RCT2_CALLPROC_EBPSAFE(0x0066B905);
}

/**
 *
 *  rct2: 0x00678680
 */
static void title_init_showcase()
{
	RCT2_CALLPROC_EBPSAFE(0x00678680);
}

void title_update()
{
	int tmp;

	if (RCT2_GLOBAL(0x009DEA6E, uint8) == 0) {
		RCT2_CALLPROC_EBPSAFE(0x00678761); // screen_title_script_tick()
		game_logic_update();
	}

	RCT2_GLOBAL(0x009DE518, uint32) &= ~0x80;
	RCT2_GLOBAL(0x009AC861, uint16) &= ~0x8000;
	RCT2_GLOBAL(0x009AC861, uint16) &= ~0x02;
	tmp = RCT2_GLOBAL(0x009AC861, uint16) & 0x01;
	RCT2_GLOBAL(0x009AC861, uint16) &= ~0x01;
	if (!tmp)
		RCT2_GLOBAL(0x009AC861, uint16) |= 0x02;
	RCT2_GLOBAL(0x009AC861, uint16) &= ~0x08;
	tmp = RCT2_GLOBAL(0x009AC861, uint16) & 0x04;
	RCT2_GLOBAL(0x009AC861, uint16) &= ~0x04;
	if (!tmp)
		RCT2_GLOBAL(0x009AC861, uint16) |= 0x04;

	RCT2_CALLPROC_EBPSAFE(0x006EE77A);

	window_update_all();

	RCT2_GLOBAL(0x01388698, uint16)++;

	// Input
	RCT2_CALLPROC_X(0x00667919, 1, 0, 0, 0, 0, 0, 0); // read_input(1)
	RCT2_CALLPROC_EBPSAFE(0x006EA627); // window_manager_handle_input();

	RCT2_CALLPROC_EBPSAFE(0x006838BD);
	RCT2_CALLPROC_EBPSAFE(0x00684218);

	if (RCT2_GLOBAL(0x009AAC73, uint8) != 255) {
		RCT2_GLOBAL(0x009AAC73, uint8)++;
		if (RCT2_GLOBAL(0x009AAC73, uint8) == 255) {
			RCT2_CALLPROC_EBPSAFE(0x00675487);
		}
	}
}