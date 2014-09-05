/*****************************************************************************
 * Copyright (c) 2014 Ted John, Dennis Devriendt
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
#include "window.h"
#include "viewport.h"

/**
 * rct2: 0x006E0FEF
 */
void window_scenery_open()
{
	rct_window* window = window_bring_to_front_by_id(WC_SCENERY, 0);
	if (window == NULL) {
		int eax, ebx, ecx, edx, edi, ebp;
		RCT2_CALLFUNC_X(0x006DFA00, &eax, &ebx, &ecx, &edx, (int*)&window, &edi, &ebp);
		eax = RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_WIDTH, uint16);
		eax -= 634;
		ebx = 0x008E027A;
		ecx = 0x00040012;
		edx = 0x009A3E9C;
		window = window_create(eax, 0x1D, 634, 142, (uint32*)edx, 0x12, 0x0400);
		window->widgets = (rct_widget*)0x009DE298;
		window->enabled_widgets |= 0x04 | 0x02000000;
		window->enabled_widgets |= 0x10 | 0x20 | 0x40 | 0x80;
		window->enabled_widgets |= 0x100 | 0x200 | 0x400 | 0x800;
		window->enabled_widgets |= 0x1000 | 0x2000 | 0x4000 | 0x8000;
		window->enabled_widgets |= 0x10000 | 0x20000 | 0x40000 | 0x80000;
		window->enabled_widgets |= 0x100000 | 0x200000 | 0x400000 | 0x800000;
		window->enabled_widgets |= 0x8000000 | 0x10000000 | 0x20000000 | 0x4000000 | 0x40000000;
		window_init_scroll_widgets(window);
		//Appears to only take esi and dx
		RCT2_CALLFUNC_X(0x006E1EB4, &eax, &ebx, &ecx, &edx, (int*)&window, &edi, &ebp);
		show_gridlines();
		RCT2_GLOBAL(0x00F64F05, uint8) = 3;
		RCT2_GLOBAL(0x00F64F12, uint8) = 0;
		RCT2_GLOBAL(0x00F64F13, uint8) = 0;
		window->viewport_focus_coordinates.var_480 = -1;
		window->viewport_focus_coordinates.x = 0;
		window_push_others_below(window);
		RCT2_GLOBAL(0x00F64F0D, uint8) = 0;
		RCT2_GLOBAL(0x00F64EB4, uint32) = 0x80000000;
		RCT2_GLOBAL(0x00F64EC0, uint16) = 0;
		RCT2_GLOBAL(0xF64F19, uint8) = 0;
		RCT2_GLOBAL(0xF64F1A, uint8) = 0;
		window->min_width = 634;
		window->min_height = 142;
		window->max_width = 634;
		window->max_height = 142;
		window->colours[0] = 24;
		window->colours[1] = 12;
		window->colours[2] = 12;
	}
}

