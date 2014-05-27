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

#ifndef _WIDGET_H_
#define _WIDGET_H_

#include "window.h"

typedef enum {
	WWT_EMPTY = 0,
	WWT_FRAME = 1,
	WWT_RESIZE = 2,
	WWT_IMGBTN = 3,
	WWT_4 = 4,
	WWT_5 = 5,
	WWT_COLORBTN = 6,
	WWT_TRNBTN = 7,
	WWT_TAB = 8,
	WWT_FLATBTN = 9,
	WWT_DROPDOWN_BUTTON = 10,
	WWT_11,
	WWT_12, // looks like a normal label to me
	WWT_13,
	WWT_14,
	WWT_SPINNER = 15,
	WWT_DROPDOWN = 16,
	WWT_VIEWPORT = 17,
	WWT_18,
	WWT_GROUPBOX = 19,
	WWT_CAPTION = 20,
	WWT_CLOSEBOX = 21,
	WWT_SCROLL = 22,
	WWT_CHECKBOX = 23,
	WWT_24,
	WWT_25,
	WWT_LAST = 26,
} WINDOW_WIDGET_TYPES;
#define WIDGETS_END		WWT_LAST, 0, 0, 0, 0, 0, 0, 0

void widget_scroll_update_thumbs(rct_window *w, int widget_index);
void widget_draw(rct_drawpixelinfo *dpi, rct_window *w, int widgetIndex);

int widget_is_enabled(rct_window *w, int widgetIndex);
int widget_is_disabled(rct_window *w, int widgetIndex);
int widget_is_pressed(rct_window *w, int widgetIndex);
int widget_is_highlighted(rct_window *w, int widgetIndex);
int widget_is_active_tool(rct_window *w, int widgetIndex);

#endif
