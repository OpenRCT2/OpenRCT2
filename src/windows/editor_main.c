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

#include "../addresses.h"
#include "../interface/viewport.h"
#include "../interface/widget.h"
#include "../interface/window.h"

static void window_editor_main_emptysub() { }

static void window_editor_main_paint();

static void* window_editor_main_events[] = {
	window_editor_main_emptysub,
	window_editor_main_emptysub,
	window_editor_main_emptysub,
	window_editor_main_emptysub,
	window_editor_main_emptysub,
	window_editor_main_emptysub,
	window_editor_main_emptysub,
	window_editor_main_emptysub,
	window_editor_main_emptysub,
	window_editor_main_emptysub,
	window_editor_main_emptysub,
	window_editor_main_emptysub,
	window_editor_main_emptysub,
	window_editor_main_emptysub,
	window_editor_main_emptysub,
	window_editor_main_emptysub,
	window_editor_main_emptysub,
	window_editor_main_emptysub,
	window_editor_main_emptysub,
	window_editor_main_emptysub,
	window_editor_main_emptysub,
	window_editor_main_emptysub,
	window_editor_main_emptysub,
	window_editor_main_emptysub,
	window_editor_main_emptysub,
	window_editor_main_emptysub,
	window_editor_main_paint,// 0x0066FC97, //window_editor_main_paint,
	window_editor_main_emptysub,
};

static rct_widget window_editor_main_widgets[] = {
	{ WWT_VIEWPORT, 0, 0, 0xFFFF, 0, 0xFFFF, 0xFFFFFFFE, 0xFFFF },
	{ WIDGETS_END },
};


/**
* Creates the main editor window that holds the main viewport.
*  rct2: 0x0066EF38
*/
void window_editor_main_open()
{
	rct_window* window;

	window_editor_main_widgets[0].right = RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_WIDTH, uint16);
	window_editor_main_widgets[0].bottom = RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_HEIGHT, uint16);
	window = window_create(0, 0, window_editor_main_widgets[0].right, window_editor_main_widgets[0].bottom,
		(uint32*)window_editor_main_events, WC_MAIN_WINDOW, WF_STICK_TO_BACK);
	window->widgets = window_editor_main_widgets;

	viewport_create(window, window->x, window->y, window->width, window->height, 0, 0x0FFF, 0x0FFF, 0, 0x1, -1);
	window->viewport->flags |= 0x0400;

	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_ROTATION, sint32) = 0;
	RCT2_GLOBAL(0x009E32B0, uint8) = 0;
	RCT2_GLOBAL(0x009E32B2, uint8) = 0;
	RCT2_GLOBAL(0x009E32B3, uint8) = 0;
	RCT2_GLOBAL(RCT2_ADDRESS_SELECTED_PATH_TYPE, uint8) = 0;

	window_top_toolbar_open();
	window_editor_bottom_toolbar_open();
}

/**
*
* rct2: 0x0066FC97
* This function immediately jumps to 0x00685BE1
*/
static void window_editor_main_paint() {
	rct_window* w;
	rct_drawpixelinfo* dpi;

	window_paint_get_registers(w, dpi);

	viewport_render(dpi, w->viewport, dpi->x, dpi->y, dpi->x + dpi->width, dpi->y + dpi->height);
}