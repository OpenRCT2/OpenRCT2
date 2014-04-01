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
#include "rct2.h"
#include "intro.h"

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

	RCT2_CALLPROC_EBPSAFE(0x006E77A1); // window_refresh_all();

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