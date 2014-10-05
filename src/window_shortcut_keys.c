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

#include "window.h"
#include "widget.h"

#define WW 340
#define WH 240

enum WINDOW_SHORTCUT_WIDGET_IDX {
	WIDX_BACKGROUND,
	WIDX_TITLE,
	WIDX_CLOSE,
	WIDX_SCROLL,
	WIDX_RESET
};

// 9DE48C
static rct_widget window_shortcut_widgets[] = {
	{ WWT_FRAME,			0,	0,		WW - 1,	0,		WH - 1,		STR_NONE,		STR_NONE },
	{ WWT_CAPTION,			0,	1,		WW - 2,	1,		14,			STR_OPTIONS,	STR_WINDOW_TITLE_TIP },
	{ WWT_CLOSEBOX,			0,	WW-13,	WW - 3,	2,		13,			STR_CLOSE_X,	STR_CLOSE_WINDOW_TIP },
	{ WWT_SCROLL,			0,	4,		WW - 5,	18,		WH - 18,	2,				2786 },
	{ WWT_DROPDOWN_BUTTON,	0,	4,		153,	WH-15,	WH - 4,		2491,			2492 },
	{ WIDGETS_END }
};

void window_shortcut_emptysub() { }

static void* window_options_events[] = {
	window_shortcut_emptysub,
	(void*)0x6E39E4,
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
	(void*)0x6E3A07,
	(void*)0x6E3A3E,
	window_shortcut_emptysub,
	(void*)0x6E3A16,
	window_shortcut_emptysub,
	window_shortcut_emptysub,
	window_shortcut_emptysub,
	(void*)0x6E3A0C,
	window_shortcut_emptysub,
	window_shortcut_emptysub,
	window_shortcut_emptysub,
	(void*)0x6E38E0,
	(void*)0x6E38E6
};

/**
 *
 *  rct2: 0x006E3884
 */
void window_shortcut_keys_open()
{
	rct_window* w;

	w = window_bring_to_front_by_id(WC_KEYBOARD_SHORTCUT_LIST, 0);

	if (w) return;

	w = window_create_auto_pos(WW, WH, (uint32*)0x9A3F0C, WC_KEYBOARD_SHORTCUT_LIST, 0);

	w->widgets = window_shortcut_widgets;
	w->enabled_widgets = (1 << WIDX_CLOSE) | (1 << WIDX_RESET);
	window_init_scroll_widgets(w);

	w->colours[0] = 7;
	w->colours[1] = 7;
	w->colours[2] = 7;
	w->no_list_items = 32;
	w->selected_list_item = -1;
}