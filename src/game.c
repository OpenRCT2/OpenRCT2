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
#include "rct2.h"
#include "game.h"

void game_update()
{
	int eax, tmp;

	// 0x006E3AEC // screen_game_process_mouse_input();
	RCT2_CALLPROC_EBPSAFE(0x006E3AEC); // screen_game_process_keyboard_input();

	// do game logic
	eax = RCT2_GLOBAL(0x009DE588, uint16) / 31;
	if (eax == 0)
		eax = 1;
	if (eax > 4)
		eax = 4;

	// if (ted_fastforwarding)
	//	eax += 8 - 1;

	if (RCT2_GLOBAL(0x009DEA6E, uint8) == 0) {
		for (; eax > 0; eax--) {
			game_logic_update();

			/*
			if (rctmem->dword_009E2D74 == 1) {
			rctmem->dword_009E2D74 = 0;
			break;
			} else {
			if (rctmem->input_state != INPUT_STATE_WIDGET_RESET && rctmem->input_state != INPUT_STATE_WIDGET_NORMAL)
			break;

			tmp = rctmem->dword_009DE518 & 0x80;
			rctmem->dword_009DE518 &= ~0x80;
			if (tmp)
			break;
			}
			*/
		}
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

void game_logic_update()
{
	short _bx, _dx;

	RCT2_GLOBAL(0x013628F4, sint32)++;
	RCT2_GLOBAL(0x00F663AC, sint32)++;
	RCT2_GLOBAL(0x009DEA66, sint16)++;
	if (RCT2_GLOBAL(0x009DEA66, sint16) == 0)
		RCT2_GLOBAL(0x009DEA66, sint16)--;

	RCT2_CALLPROC_EBPSAFE(0x0068B089);
	RCT2_CALLPROC_EBPSAFE(0x006C44B1);
	RCT2_CALLPROC_EBPSAFE(0x006C46B1);
	RCT2_CALLPROC_EBPSAFE(0x006646E1);
	RCT2_CALLPROC_EBPSAFE(0x006A876D);

	RCT2_CALLPROC_EBPSAFE(0x0068F0A9); // peep_update_all()

	// update ride
	RCT2_CALLPROC_EBPSAFE(0x006D4204);

	// update text effects
	RCT2_CALLPROC_EBPSAFE(0x00672AA4);
	RCT2_CALLPROC_EBPSAFE(0x006ABE4C);
	RCT2_CALLPROC_EBPSAFE(0x006674F7);
	RCT2_CALLPROC_EBPSAFE(0x00684C7A);
	RCT2_CALLPROC_EBPSAFE(0x006B5A2A);
	RCT2_CALLPROC_EBPSAFE(0x006B6456);
	RCT2_CALLPROC_EBPSAFE(0x0068AFAD);
	RCT2_CALLPROC_EBPSAFE(0x006BBC6B);
	RCT2_CALLPROC_EBPSAFE(0x006BD18A);
	RCT2_CALLPROC_EBPSAFE(0x006BCB91);
	RCT2_CALLPROC_EBPSAFE(0x006BD0F8);

	RCT2_CALLPROC_EBPSAFE(0x0066E252); // news_item_update_current()
	RCT2_CALLPROC_EBPSAFE(0x0067009A);

	// Update windows
	RCT2_CALLPROC_EBPSAFE(0x006ED7B0); // window_update_all()

	if (RCT2_GLOBAL(0x009AC31B, uint8) != 0) {
		_bx = 3010;
		_dx = RCT2_GLOBAL(0x009AC31C, uint16);
		if (RCT2_GLOBAL(0x009AC31B, uint8) != 254) {
			_bx = RCT2_GLOBAL(0x009AC31C, uint16);
			_dx = 0xFFFF;
		}
		RCT2_GLOBAL(0x009AC31B, uint8) = 0;

		RCT2_CALLPROC_X(0x0066792F, 0, _bx, 0, _dx, 0, 0, 0);
	}
}