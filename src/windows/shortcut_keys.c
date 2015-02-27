/*****************************************************************************
* Copyright (c) 2014 Ted John, Duncan Frost
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
#include "../config.h"
#include "../interface/window.h"
#include "../interface/widget.h"
#include "../localisation/localisation.h"
#include "../platform/platform.h"

#define WW 340
#define WH 240

enum WINDOW_SHORTCUT_WIDGET_IDX {
	WIDX_BACKGROUND,
	WIDX_TITLE,
	WIDX_CLOSE,
	WIDX_SCROLL,
	WIDX_RESET
};

// 0x9DE48C
static rct_widget window_shortcut_widgets[] = {
	{ WWT_FRAME,			0,	0,		WW - 1,	0,		WH - 1,		STR_NONE,		STR_NONE },
	{ WWT_CAPTION,			0,	1,		WW - 2,	1,		14,			STR_OPTIONS,	STR_WINDOW_TITLE_TIP },
	{ WWT_CLOSEBOX,			0,	WW-13,	WW - 3,	2,		13,			STR_CLOSE_X,	STR_CLOSE_WINDOW_TIP },
	{ WWT_SCROLL,			0,	4,		WW - 5,	18,		WH - 18,	2,				2786 },
	{ WWT_DROPDOWN_BUTTON,	0,	4,		153,	WH-15,	WH - 4,		2491,			2492 },
	{ WIDGETS_END }
};

void window_shortcut_emptysub() { }
static void window_shortcut_mouseup();
static void window_shortcut_paint();
static void window_shortcut_tooltip();
static void window_shortcut_scrollgetsize();
static void window_shortcut_scrollmousedown();
static void window_shortcut_scrollmouseover();
static void window_shortcut_scrollpaint();

static void* window_shortcut_events[] = {
	window_shortcut_emptysub,
	window_shortcut_mouseup,
	window_shortcut_emptysub,
	window_shortcut_emptysub,
	window_shortcut_emptysub,
	window_shortcut_emptysub,
	window_shortcut_emptysub,
	window_shortcut_emptysub,
	window_shortcut_emptysub,
	window_shortcut_emptysub,
	window_shortcut_emptysub,
	window_shortcut_emptysub,
	window_shortcut_emptysub,
	window_shortcut_emptysub,
	window_shortcut_emptysub,
	window_shortcut_scrollgetsize,
	window_shortcut_scrollmousedown,
	window_shortcut_emptysub,
	window_shortcut_scrollmouseover,
	window_shortcut_emptysub,
	window_shortcut_emptysub,
	window_shortcut_emptysub,
	window_shortcut_tooltip,
	window_shortcut_emptysub,
	window_shortcut_emptysub,
	window_shortcut_emptysub,
	window_shortcut_paint,
	window_shortcut_scrollpaint
};

/**
 *
 *  rct2: 0x006E3884
 */
void window_shortcut_keys_open()
{
	rct_window* w;

	w = window_bring_to_front_by_class(WC_KEYBOARD_SHORTCUT_LIST);

	if (w) return;

	w = window_create_auto_pos(WW, WH, (uint32*)window_shortcut_events, WC_KEYBOARD_SHORTCUT_LIST, 0);

	w->widgets = window_shortcut_widgets;
	w->enabled_widgets = (1 << WIDX_CLOSE) | (1 << WIDX_RESET);
	window_init_scroll_widgets(w);

	w->colours[0] = 7;
	w->colours[1] = 7;
	w->colours[2] = 7;
	w->no_list_items = 32;
	w->selected_list_item = -1;
}

/**
*
*  rct2: 0x006E39E4
*/
static void window_shortcut_mouseup()
{
	short widgetIndex;
	rct_window *w;

	window_widget_get_registers(w, widgetIndex);

	switch (widgetIndex){
	case WIDX_CLOSE:
		window_close(w);
		break;
	case WIDX_RESET:
		config_reset_shortcut_keys();
		config_shortcut_keys_save();
		window_invalidate(w);
		break;
	}
}

/**
*
*  rct2: 0x006E38E0
*/
static void window_shortcut_paint()
{
	rct_window *w;
	rct_drawpixelinfo *dpi;

	window_paint_get_registers(w, dpi);

	window_draw_widgets(w, dpi);
}

/**
*
*  rct2: 0x006E3A0C
*/
static void window_shortcut_tooltip()
{
	RCT2_GLOBAL(0x013CE952, uint16) = STR_LIST;
}

/**
*
*  rct2: 0x006E3A07
*/
static void window_shortcut_scrollgetsize()
{
	rct_window *w;
	int width, height;

	window_get_register(w);

	width = 0;
	height = 32 * 10;

	window_scrollsize_set_registers(width, height);
}

/**
*
*  rct2: 0x006E3A3E
*/
static void window_shortcut_scrollmousedown()
{
	short x, y, scrollIndex;
	rct_window *w;

	window_scrollmouse_get_registers(w, scrollIndex, x, y);

	int selected_item = y / 10;
	if (selected_item >= w->no_list_items)
		return;

	window_shortcut_change_open(selected_item);
}

/**
*
*  rct2: 0x006E3A16
*/
static void window_shortcut_scrollmouseover()
{
	short x, y, scrollIndex;
	rct_window *w;

	window_scrollmouse_get_registers(w, scrollIndex, x, y);

	int selected_item = y / 10;
	if (selected_item >= w->no_list_items)
		return;
	
	w->selected_list_item = selected_item;

	window_invalidate(w);
}

/**
 * 
 *  rct2: 0x006E38E6
 */
static void window_shortcut_scrollpaint()
{
	rct_window *w;
	rct_drawpixelinfo *dpi;

	window_paint_get_registers(w, dpi);

	gfx_fill_rect(dpi, dpi->x, dpi->y, dpi->x + dpi->width - 1, dpi->y + dpi->height - 1, RCT2_ADDRESS(0x0141FC48,uint8)[w->colours[1] * 8]);
	
	for (int i = 0; i < w->no_list_items; ++i) {
		int y = i * 10;
		if (y > dpi->y + dpi->height)
			break;

		if (y + 10 < dpi->y)continue;
		int format = STR_BLACK_STRING;
		if (i == w->selected_list_item) {
			format = STR_WINDOW_COLOUR_2_STRING;
			gfx_fill_rect(dpi, 0, y, 800, y + 9, 0x2000031);
		}

		RCT2_GLOBAL(0x13CE954, uint16) = i + STR_SHORTCUT_DESCRIPTION_0;
		RCT2_GLOBAL(0x13CE956, uint16) = 0;
		RCT2_GLOBAL(0x13CE958, uint16) = 0;

		uint16 shortcut_entry = gShortcutKeys[i];
		if (shortcut_entry != 0xFFFF) {
			rct_string_id templateStringId = 2525;
			const char *scanCodeName = SDL_GetScancodeName(shortcut_entry & 0xFF);
			char *templateString = (char*)language_get_string(templateStringId);
			strcpy(templateString, scanCodeName);

			RCT2_GLOBAL(0x13CE958, uint16) = templateStringId;
			
			// Display the modifer
			if (shortcut_entry & 0x100)
				RCT2_GLOBAL(0x13CE956, uint16) = STR_SHIFT_PLUS;
			else if (shortcut_entry & 0x200)
				RCT2_GLOBAL(0x13CE956, uint16) = STR_CTRL_PLUS;
		}

		RCT2_GLOBAL(0x13CE952, uint16) = STR_SHORTCUT_ENTRY_FORMAT;
		gfx_draw_string_left(dpi, format, (void*)0x13CE952, 0, 0, y - 1);
	}
}