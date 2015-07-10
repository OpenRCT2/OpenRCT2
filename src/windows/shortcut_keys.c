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
#include "../interface/themes.h"

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

static void window_shortcut_mouseup(rct_window *w, int widgetIndex);
static void window_shortcut_invalidate(rct_window *w);
static void window_shortcut_paint(rct_window *w, rct_drawpixelinfo *dpi);
static void window_shortcut_tooltip(rct_window* w, int widgetIndex, rct_string_id *stringId);
static void window_shortcut_scrollgetsize(rct_window *w, int scrollIndex, int *width, int *height);
static void window_shortcut_scrollmousedown(rct_window *w, int scrollIndex, int x, int y);
static void window_shortcut_scrollmouseover(rct_window *w, int scrollIndex, int x, int y);
static void window_shortcut_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, int scrollIndex);

static rct_window_event_list window_shortcut_events = {
	NULL,
	window_shortcut_mouseup,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	window_shortcut_scrollgetsize,
	window_shortcut_scrollmousedown,
	NULL,
	window_shortcut_scrollmouseover,
	NULL,
	NULL,
	NULL,
	window_shortcut_tooltip,
	NULL,
	NULL,
	window_shortcut_invalidate,
	window_shortcut_paint,
	window_shortcut_scrollpaint
};

const rct_string_id ShortcutStringIds[] = {
	STR_SHORTCUT_CLOSE_TOP_MOST_WINDOW,
	STR_SHORTCUT_CLOSE_ALL_FLOATING_WINDOWS,
	STR_SHORTCUT_CANCEL_CONSTRUCTION_MODE,
	STR_SHORTCUT_PAUSE_GAME,
	STR_SHORTCUT_ZOOM_VIEW_OUT,
	STR_SHORTCUT_ZOOM_VIEW_IN,
	STR_SHORTCUT_ROTATE_VIEW_CLOCKWISE,
	STR_SHORTCUT_ROTATE_VIEW_ANTICLOCKWISE,
	STR_SHORTCUT_ROTATE_CONSTRUCTION_OBJECT,
	STR_SHORTCUT_UNDERGROUND_VIEW_TOGGLE,
	STR_SHORTCUT_REMOVE_BASE_LAND_TOGGLE,
	STR_SHORTCUT_REMOVE_VERTICAL_LAND_TOGGLE,
	STR_SHORTCUT_SEE_THROUGH_RIDES_TOGGLE,
	STR_SHORTCUT_SEE_THROUGH_SCENERY_TOGGLE,
	STR_SHORTCUT_INVISIBLE_SUPPORTS_TOGGLE,
	STR_SHORTCUT_INVISIBLE_PEOPLE_TOGGLE,
	STR_SHORTCUT_HEIGHT_MARKS_ON_LAND_TOGGLE,
	STR_SHORTCUT_HEIGHT_MARKS_ON_RIDE_TRACKS_TOGGLE,
	STR_SHORTCUT_HEIGHT_MARKS_ON_PATHS_TOGGLE,
	STR_SHORTCUT_ADJUST_LAND,
	STR_SHORTCUT_ADJUST_WATER,
	STR_SHORTCUT_BUILD_SCENERY,
	STR_SHORTCUT_BUILD_PATHS,
	STR_SHORTCUT_BUILD_NEW_RIDE,
	STR_SHORTCUT_SHOW_FINANCIAL_INFORMATION,
	STR_SHORTCUT_SHOW_RESEARCH_INFORMATION,
	STR_SHORTCUT_SHOW_RIDES_LIST,
	STR_SHORTCUT_SHOW_PARK_INFORMATION,
	STR_SHORTCUT_SHOW_GUEST_LIST,
	STR_SHORTCUT_SHOW_STAFF_LIST,
	STR_SHORTCUT_SHOW_RECENT_MESSAGES,
	STR_SHORTCUT_SHOW_MAP,
	STR_SHORTCUT_SCREENSHOT,
	STR_SHORTCUT_REDUCE_GAME_SPEED,
	STR_SHORTCUT_INCREASE_GAME_SPEED,
	STR_SHORTCUT_OPEN_CHEATS_WINDOW,
	STR_SHORTCUT_TOGGLE_VISIBILITY_OF_TOOLBARS,
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

	w = window_create_auto_pos(WW, WH, &window_shortcut_events, WC_KEYBOARD_SHORTCUT_LIST, 0);

	w->widgets = window_shortcut_widgets;
	w->enabled_widgets = (1 << WIDX_CLOSE) | (1 << WIDX_RESET);
	window_init_scroll_widgets(w);

	w->no_list_items = SHORTCUT_COUNT;
	w->selected_list_item = -1;
}

/**
*
*  rct2: 0x006E39E4
*/
static void window_shortcut_mouseup(rct_window *w, int widgetIndex)
{
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

static void window_shortcut_invalidate(rct_window *w)
{
	colour_scheme_update(w);
}

/**
*
*  rct2: 0x006E38E0
*/
static void window_shortcut_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
	window_draw_widgets(w, dpi);
}

/**
*
*  rct2: 0x006E3A0C
*/
static void window_shortcut_tooltip(rct_window* w, int widgetIndex, rct_string_id *stringId)
{
	RCT2_GLOBAL(0x013CE952, uint16) = STR_LIST;
}

/**
*
*  rct2: 0x006E3A07
*/
static void window_shortcut_scrollgetsize(rct_window *w, int scrollIndex, int *width, int *height)
{
	*height = w->no_list_items * 10;
}

/**
*
*  rct2: 0x006E3A3E
*/
static void window_shortcut_scrollmousedown(rct_window *w, int scrollIndex, int x, int y)
{
	int selected_item = y / 10;
	if (selected_item >= w->no_list_items)
		return;

	window_shortcut_change_open(selected_item);
}

/**
*
*  rct2: 0x006E3A16
*/
static void window_shortcut_scrollmouseover(rct_window *w, int scrollIndex, int x, int y)
{
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
static void window_shortcut_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, int scrollIndex)
{
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

		RCT2_GLOBAL(0x13CE954, uint16) = ShortcutStringIds[i];
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