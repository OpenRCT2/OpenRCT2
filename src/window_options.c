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
#include "strings.h"
#include "widget.h"
#include "window.h"

static enum WINDOW_OPTIONS_WIDGET_IDX {
	WIDX_BACKGROUND,
	WIDX_TITLE,
	WIDX_CLOSE,
	WIDX_SOUND_DROPDOWN = 5,
	WIDX_MUSIC = 6,
	WIDX_SOUND_QUALITY = 8,
	WIDX_CURRENCY = 12,
	WIDX_DISTANCE = 14,
	WIDX_TEMPERATURE = 16,
	WIDX_HEIGHT_LABELS = 18,
	WIDX_RESOLUTION = 21,
	WIDX_CONSTRUCTION_MARKER = 25
};

static rct_widget window_options_widgets[] = {
	{ WWT_FRAME,			0,	0,		309,	0,		371,	0x0FFFFFFFF,	STR_NONE },
	{ WWT_CAPTION,			0,	1,		308,	1,		14,		0x490,			STR_WINDOW_TITLE_TIP },
	{ WWT_CLOSEBOX,			0,	297,	307,	2,		13,		0x338,			STR_CLOSE_WINDOW_TIP },
	{ WWT_GROUPBOX,			0,	3,		306,	17,		93,		0x91D,			STR_NONE },
	{ WWT_DROPDOWN,			0,	10,		299,	31,		42,		0x361,			STR_NONE },
	{ WWT_DROPDOWN_BUTTON,	0,	288,	298,	32,		41,		0x36C,			STR_NONE },
	{ WWT_DROPDOWN,			0,	155,	299,	46,		57,		0x365,			STR_NONE },
	{ WWT_DROPDOWN_BUTTON,	0,	288,	298,	47,		56,		0x36C,			STR_NONE },
	{ WWT_DROPDOWN,			0,	155,	299,	61,		72,		0x366,			STR_NONE },
	{ WWT_DROPDOWN_BUTTON,	0,	288,	298,	62,		71,		0x36C,			STR_NONE },
	{ WWT_CHECKBOX,			0,	10,		299,	76,		87,		0x0D22,			STR_SOUND_FORCED_SOFTWARE_BUFFER_MIXING_TIP },
	{ WWT_GROUPBOX,			0,	3,		306,	100,	176,	0x91C,			STR_NONE },
	{ WWT_DROPDOWN,			0,	155,	299,	114,	125,	0x367,			STR_NONE },
	{ WWT_DROPDOWN_BUTTON,	0,	288,	298,	115,	124,	0x36C,			STR_NONE },
	{ WWT_DROPDOWN,			0,	155,	299,	129,	140,	0x368,			STR_NONE },
	{ WWT_DROPDOWN_BUTTON,	0,	288,	298,	130,	139,	0x36C,			STR_NONE },
	{ WWT_DROPDOWN,			0,	155,	299,	144,	155,	0x36B,			STR_NONE },
	{ WWT_DROPDOWN_BUTTON,	0,	288,	298,	145,	154,	0x36C,			STR_NONE },
	{ WWT_DROPDOWN,			0,	155,	299,	159,	170,	0x364,			STR_NONE },
	{ WWT_DROPDOWN_BUTTON,	0,	288,	298,	160,	169,	0x36C,			STR_NONE },
	{ WWT_GROUPBOX,			0,	3,		306,	182,	258,	0x92A,			STR_NONE },
	{ WWT_DROPDOWN,			0,	155,	299,	196,	207,	0x348,			STR_NONE },
	{ WWT_DROPDOWN_BUTTON,	0,	288,	298,	197,	206,	0x36C,			STR_NONE },
	{ WWT_CHECKBOX,			0,	10,		299,	212,	223,	0x939,			STR_TILE_EDGE_SMOOTHING_TIP },
	{ WWT_CHECKBOX,			0,	10,		299,	227,	238,	0x93B,			STR_TURN_GRIDLINES_ON_TIP },
	{ WWT_DROPDOWN,			0,	155,	299,	241,	252,	0x0FFFFFFFF,	STR_NONE },
	{ WWT_DROPDOWN_BUTTON,	0,	288,	298,	242,	251,	0x36C,			STR_NONE },
	{ WWT_GROUPBOX,			0,	3,		306,	264,	310,	0x9B5,			STR_NONE },
	{ WWT_CHECKBOX,			2,	10,		299,	279,	290,	0x0AED,			STR_SCREEN_EDGE_SCROLL_TIP },
	{ WWT_DROPDOWN_BUTTON,	0,	26,		185,	293,	304,	0x9B9,			STR_CHANGE_HOTKEY_TIP },
	{ WWT_GROUPBOX,			0,	3,		306,	317,	365,	0x9B6,			STR_NONE },
	{ WWT_CHECKBOX,			2,	10,		299,	331,	342,	0x9B7,			STR_REAL_NAME_TOGGLE_TIP },
	{ WWT_CHECKBOX,			2,	10,		299,	346,	357,	0x0D05,			STR_SAVE_PLUGIN_DATA_TIP },
	{ WIDGETS_END },
};

static void window_options_emptysub() { }
static void window_options_mouseup();
static void window_options_mousedown();
static void window_options_dropdown();
static void window_options_textinput();
static void window_options_update();
static void window_options_paint();

static uint32 window_options_events[] = {
	window_options_emptysub,
	window_options_mouseup,
	window_options_emptysub,
	window_options_mousedown,
	window_options_dropdown,
	window_options_emptysub,
	window_options_emptysub,
	window_options_emptysub,
	window_options_emptysub,
	window_options_emptysub,
	window_options_emptysub,
	window_options_emptysub,
	window_options_emptysub,
	window_options_emptysub,
	window_options_emptysub,
	window_options_emptysub,
	window_options_emptysub,
	window_options_emptysub,
	window_options_emptysub,
	window_options_emptysub,
	window_options_emptysub,
	window_options_emptysub,
	window_options_emptysub,
	window_options_emptysub,
	window_options_emptysub,
	window_options_update,
	window_options_paint,
	window_options_emptysub
};

/**
*
*  rct2: 0x006BAC5B
*/
void window_options_open()
{
	rct_window* w;

	// Check if window is already open
	w = window_bring_to_front_by_id(WC_OPTIONS, 0);
	if (w != NULL)
		return;

	w = window_create_auto_pos(310, 372, window_options_events, WC_OPTIONS, 0);
	w->widgets = window_options_widgets;
	w->enabled_widgets = 
		(1 << WIDX_CLOSE) | 
		0x10 | 
		(1 << WIDX_SOUND_DROPDOWN) |
		0x40 | 
		0x80 | 
		0x100 | 
		0x200 | 
		0x1000 | 
		0x2000 | 
		0x4000 | 
		0x8000 | 
		0x200000 | 
		0x400000 | 
		0x10000 | 
		0x20000 | 
		0x20000000 | 
		0x10000000 | 
		0x80000000 | 
		0x2000000 | 
		0x4000000 | 
		0x40000 | 
		0x80000 | 
		0x800000 | 
		0x1000000 | 
		0x400;
	// TODO: missing .text:006BAD22                 or      dword ptr [esi+0Ch], 1

	window_init_scroll_widgets(w);
	w->colours[0] = 7;
	w->colours[1] = 7;
	w->colours[2] = 7;
}

/**
*
*  rct2: 0x006BAFCA
*/
static void window_options_mouseup()
{
	RCT2_CALLPROC_EBPSAFE(0x006BAFCA);
}

/**
*
*  rct2: 0x006BB01B
*/
static void window_options_mousedown()
{
	RCT2_CALLPROC_EBPSAFE(0x006BB01B);
}

/**
*
*  rct2: 0x006BB076
*/
static void window_options_dropdown()
{
	RCT2_CALLPROC_EBPSAFE(0x006BB076);
	/*short widgetIndex;
	rct_window *w;

	__asm mov widgetIndex, dx
	__asm mov w, esi

	switch (widgetIndex) {
	case WIDX_SOUND_DROPDOWN:
		RCT2_CALLPROC_EBPSAFE(0x006BB757);
		break;
	}*/
}

/**
*
*  rct2: 0x006BAD48
*/
static void window_options_update()
{
	RCT2_CALLPROC_EBPSAFE(0x006BAD48);
}

/**
*
*  rct2: 0x006BAEB4
*/
static void window_options_paint()
{
	rct_window *w;
	rct_drawpixelinfo *dpi;

	__asm mov w, esi
	__asm mov dpi, edi

	window_draw_widgets(w, dpi);

	// units
	gfx_draw_string_left(dpi, STR_CURRENCY, w, 0, w->x + 10,
		w->y + window_options_widgets[WIDX_CURRENCY].top + 1);
	gfx_draw_string_left(dpi, STR_DISTANCE_AND_SPEED, w, 0, w->x + 10,
		w->y + window_options_widgets[WIDX_DISTANCE].top + 1);
	gfx_draw_string_left(dpi, STR_TEMPERATURE, w, 0, w->x + 10,
		w->y + window_options_widgets[WIDX_TEMPERATURE].top + 1);
	gfx_draw_string_left(dpi, STR_HEIGHT_LABELS, w, 0, w->x + 10,
		w->y + window_options_widgets[WIDX_HEIGHT_LABELS].top + 1);

	// display
	gfx_draw_string_left(dpi, STR_DISPLAY_RESOLUTION, w, 0, w->x + 10,
		w->y + window_options_widgets[WIDX_RESOLUTION].top + 1);
	gfx_draw_string_left(dpi, STR_CONSTRUCTION_MARKER, w, 0, w->x + 10,
		w->y + window_options_widgets[WIDX_CONSTRUCTION_MARKER].top + 1);

	// sound
	gfx_draw_string_left(dpi, STR_MUSIC, w, 0, w->x + 10,
		w->y + window_options_widgets[WIDX_MUSIC].top + 1);
	gfx_draw_string_left(dpi, STR_SOUND_QUALITY, w, 0, w->x + 10,
		w->y + window_options_widgets[WIDX_SOUND_QUALITY].top + 1);
}
