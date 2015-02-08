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

#ifndef _INPUT_H_
#define _INPUT_H_

#include "interface/window.h"

enum {
	INPUT_FLAG_WIDGET_PRESSED = (1 << 0),

	// Related to dropdowns, set on flag 0x80
	INPUT_FLAG_1 = (1 << 1),

	// Related to dropdowns
	INPUT_FLAG_2 = (1 << 2),

	INPUT_FLAG_TOOL_ACTIVE = (1 << 3),

	// Left click on a viewport
	INPUT_FLAG_4 = (1 << 4),

	INPUT_FLAG_5 = (1 << 5),

	// Some of the map tools (clear, footpath, scenery)
	// never read as far as I know.
	INPUT_FLAG_6 = (1 << 6),

	INPUT_FLAG_VIEWPORT_SCROLLING = (1 << 7)
};

void title_handle_keyboard_input();
void game_handle_input();
void game_handle_keyboard_input();

void store_mouse_input(int state);

void input_window_position_begin(rct_window *w, int widgetIndex, int x, int y);

#endif