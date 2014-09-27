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

#include <windows.h>
#include <memory.h>
#include "addresses.h"
#include "editor.h"
#include "ride.h"
#include "string_ids.h"
#include "widget.h"
#include "window.h"
#include "window_error.h"

enum {
	WIDX_BACKGROUND,
	WIDX_TITLE,
	WIDX_CLOSE,
	WIDX_TRACK_LIST,
	WIDX_UNK,
	WIDX_ROTATE,
	WIDX_TOGGLE_SCENERY
};

static rct_widget window_track_list_widgets[] = {
	{ WWT_FRAME,			0,	0,		599,	0,		399,	0xFFFFFFFF,				STR_NONE								},
	{ WWT_CAPTION,			0,	1,		598,	1,		14,		STR_SELECT_DESIGN,		STR_WINDOW_TITLE_TIP					},
	{ WWT_CLOSEBOX,			0,	587,	597,	2,		13,		STR_CLOSE_X,			STR_CLOSE_WINDOW_TIP					},
	{ WWT_SCROLL,			0,	4,		221,	18,		395,	2,						STR_CLICK_ON_DESIGN_TO_BUILD_IT_TIP		},
	{ WWT_FLATBTN,			0,	224,	595,	18,		236,	0xFFFFFFFF,				STR_NONE								},
	{ WWT_FLATBTN,			0,	574,	597,	374,	397,	5169,					STR_ROTATE_90_TIP						},
	{ WWT_FLATBTN,			0,	574,	597,	350,	373,	5171,					STR_TOGGLE_SCENERY_TIP					},
	{ WIDGETS_END },
};

static void window_track_list_emptysub() { }
static void window_track_list_close();
static void window_track_list_mouseup();
static void window_track_list_scrollgetsize();
static void window_track_list_scrollmousedown();
static void window_track_list_scrollmouseover();
static void window_track_list_tooltip();
static void window_track_list_invalidate();
static void window_track_list_paint();
static void window_track_list_scrollpaint();

static void* window_track_list_events[] = {
	(uint32*)window_track_list_close,
	(uint32*)window_track_list_mouseup,
	(uint32*)window_track_list_emptysub,
	(uint32*)window_track_list_emptysub,
	(uint32*)window_track_list_emptysub,
	(uint32*)window_track_list_emptysub,
	(uint32*)window_track_list_emptysub,
	(uint32*)window_track_list_emptysub,
	(uint32*)window_track_list_emptysub,
	(uint32*)window_track_list_emptysub,
	(uint32*)window_track_list_emptysub,
	(uint32*)window_track_list_emptysub,
	(uint32*)window_track_list_emptysub,
	(uint32*)window_track_list_emptysub,
	(uint32*)window_track_list_emptysub,
	(uint32*)window_track_list_scrollgetsize,
	(uint32*)window_track_list_scrollmousedown,
	(uint32*)window_track_list_emptysub,
	(uint32*)window_track_list_scrollmouseover,
	(uint32*)window_track_list_emptysub,
	(uint32*)window_track_list_emptysub,
	(uint32*)window_track_list_emptysub,
	(uint32*)window_track_list_tooltip,
	(uint32*)window_track_list_emptysub,
	(uint32*)window_track_list_emptysub,
	(uint32*)window_track_list_invalidate,
	(uint32*)window_track_list_paint,
	(uint32*)window_track_list_scrollpaint
};

static ride_list_item _window_track_list_item;

/**
 *
 *  rct2: 0x006CF1A2
 */
void window_track_list_open(ride_list_item item)
{
	rct_window *w;
	int x, y;
	void *mem;

	window_close_construction_windows();
	_window_track_list_item = item;

	if (RCT2_GLOBAL(0x00F635ED, uint8) & 1)
		window_error_open(STR_WARNING, STR_TOO_MANY_TRACK_DESIGNS_OF_THIS_TYPE);

	mem = malloc(1285292);
	if (mem == NULL)
		return;

	RCT2_GLOBAL(0x00F44105, void*) = mem;
	RCT2_CALLPROC_EBPSAFE(0x006D1DCE);

	if (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_TRACK_MANAGER) {
		x = RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_WIDTH, uint16) / 2 - 300;
		y = max(28, RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_HEIGHT, uint16) / 2 - 200);
	} else {
		x = 0;
		y = 29;
	}
	w = window_create(0, 29, 600, 400, (uint32*)window_track_list_events, WC_TRACK_DESIGN_LIST, 0);
	w->widgets = window_track_list_widgets;
	w->enabled_widgets = (1 << WIDX_CLOSE) | (1 << WIDX_ROTATE) | (1 << WIDX_TOGGLE_SCENERY);
	window_init_scroll_widgets(w);
	w->colours[0] = 26;
	w->colours[1] = 26;
	w->colours[2] = 26;
	w->track_list.var_480 = 0xFFFF;
	w->track_list.var_482 = RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_TRACK_MANAGER ? 1 : 0;
	w->track_list.var_484 = 0;
	RCT2_GLOBAL(0x00F44152, uint8) = 0;
	window_push_others_right(w);
	RCT2_GLOBAL(0x00F440AE, uint8) = 2;
}

/**
 *
 *  rct2: 0x006CFD76
 */
static void window_track_list_close()
{
	free(RCT2_GLOBAL(0x00F44105, void*));
}

/**
 *
 *  rct2: 0x006CFA31
 */
static void window_track_list_mouseup()
{
	rct_window *w;
	short widgetIndex;

	window_widget_get_registers(w, widgetIndex);

	switch (widgetIndex) {
	case WIDX_CLOSE:
		window_close(w);
		if (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_TRACK_MANAGER) {
			window_close_by_id(WC_47, w->number);
			window_close_by_id(WC_48, w->number);
			trackmanager_load();
		}
		break;
	case WIDX_ROTATE:
		RCT2_GLOBAL(0x00F440AE, uint8)++;
		RCT2_GLOBAL(0x00F440AE, uint8) %= 4;
		window_invalidate(w);
		break;
	case WIDX_TOGGLE_SCENERY:
		RCT2_GLOBAL(0x00F44152, uint8) ^= 1;
		RCT2_CALLPROC_EBPSAFE(0x006D1DCE);
		window_invalidate(w);
		break;
	}
}

/**
 *
 *  rct2: 0x006CFAB0
 */
static void window_track_list_scrollgetsize()
{
	rct_window *w;
	int height;
	uint8 *trackDesignItem, *trackDesignList = (uint8*)0x00F441EC;

	window_get_register(w);

	height = RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_TRACK_MANAGER & 8 ? 0 : 10;
	for (trackDesignItem = trackDesignList; *trackDesignItem != 0; trackDesignItem += 128)
		height += 10;

	#ifdef _MSC_VER
	__asm mov ecx, 0
	#else
	__asm__ ( "mov ecx, 0 "  );
	#endif

	#ifdef _MSC_VER
	__asm mov edx, height
	#else
	__asm__ ( "mov edx, %[height] " : [height] "+m" (height) );
	#endif
}

/**
 *
 *  rct2: 0x006CFB39
 */
static void window_track_list_scrollmousedown()
{
	rct_window *w;
	short x, y;

	window_scrollmouse_get_registers(w, x, y);
}

/**
 *
 *  rct2: 0x006CFAD7
 */
static void window_track_list_scrollmouseover()
{
	rct_window *w;
	short i, x, y;
	uint8 *trackDesignItem, *trackDesignList = (uint8*)0x00F441EC;

	window_scrollmouse_get_registers(w, x, y);

	if (w->track_list.var_484 & 1)
		return;
	if (RCT2_GLOBAL(0x009DEA6E, uint8) != 0)
		return;

	i = 0;
	if (!(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_TRACK_MANAGER & 8)) {
		y -= 10;
		if (y < 0) {
			if (w->track_list.var_482 != i) {
				w->track_list.var_482 = i;
				window_invalidate(w);
			}
			return;
		}
		i++;
	}

	for (trackDesignItem = trackDesignList; *trackDesignItem != 0; trackDesignItem += 128) {
		y -= 10;
		if (y < 0) {
			if (w->track_list.var_482 != i) {
				w->track_list.var_482 = i;
				window_invalidate(w);
			}
			break;
		}
		i++;
	}
}

/**
 *
 *  rct2: 0x006CFD6C
 */
static void window_track_list_tooltip()
{
	RCT2_GLOBAL(0x013CE952, uint16) = STR_LIST;
}

/**
 *
 *  rct2: 0x006CF2D6
 */
static void window_track_list_invalidate()
{
	rct_window *w;
	rct_ride_type *entry;
	rct_string_id stringId;

	window_get_register(w);

	entry = GET_RIDE_ENTRY(_window_track_list_item.entry_index);

	stringId = entry->name;
	if (!(entry->var_008 & 0x1000))
		stringId = _window_track_list_item.type + 2;

	RCT2_GLOBAL(0x013CE952, uint16) = stringId;
	if (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_TRACK_MANAGER) {
		window_track_list_widgets[WIDX_TITLE].image = STR_TRACK_DESIGNS;
		window_track_list_widgets[WIDX_TRACK_LIST].tooltip = STR_CLICK_ON_DESIGN_TO_RENAME_OR_DELETE_IT;
	} else {
		window_track_list_widgets[WIDX_TITLE].image = STR_SELECT_DESIGN;
		window_track_list_widgets[WIDX_TRACK_LIST].tooltip = STR_CLICK_ON_DESIGN_TO_BUILD_IT_TIP;
	}

	if ((RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_TRACK_MANAGER) || w->track_list.var_482 != 0) {
		w->pressed_widgets |= 1 << WIDX_UNK;
		w->disabled_widgets &= ~(1 << WIDX_UNK);
		window_track_list_widgets[WIDX_ROTATE].type = WWT_FLATBTN;
		window_track_list_widgets[WIDX_TOGGLE_SCENERY].type = WWT_FLATBTN;
		if (RCT2_GLOBAL(0x00F44152, uint8) == 0)
			w->pressed_widgets |= (1 << WIDX_TOGGLE_SCENERY);
		else
			w->pressed_widgets &= ~(1 << WIDX_TOGGLE_SCENERY);
	} else {
		w->pressed_widgets &= ~(1 << WIDX_UNK);
		w->disabled_widgets |= (1 << WIDX_UNK);
		window_track_list_widgets[WIDX_ROTATE].type = WWT_EMPTY;
		window_track_list_widgets[WIDX_TOGGLE_SCENERY].type = WWT_EMPTY;
	}
}

/**
 *
 *  rct2: 0x006CF387
 */
static void window_track_list_paint()
{
	rct_window *w;
	rct_drawpixelinfo *dpi;

	window_paint_get_registers(w, dpi);

	window_draw_widgets(w, dpi);
}

/**
 *
 *  rct2: 0x006CF8CD
 */
static void window_track_list_scrollpaint()
{
	rct_window *w;
	rct_drawpixelinfo *dpi;
	rct_string_id stringId, stringId2;
	int i, j, x, y, colour;
	uint8 *trackDesignItem, *trackDesignList = (uint8*)0x00F441EC;
	char *trackName, *ch;

	window_paint_get_registers(w, dpi);

	colour = RCT2_GLOBAL(0x00141FC48 + (w->colours[0] * 8), uint8);
	colour = (colour << 24) | (colour << 16) | (colour << 8) | colour;
	gfx_clear(dpi, colour);

	i = 0;
	x = 0;
	y = 0;

	trackDesignItem = trackDesignList;
	if (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_TRACK_MANAGER) {
		if (*trackDesignItem == 0) {
			// No track designs
			gfx_draw_string_left(dpi, STR_NO_TRACK_DESIGNS_OF_THIS_TYPE, NULL, 0, x, y - 1);
			return;
		}
	} else {
		// Build custom track item
		if (i == w->track_list.var_482) {
			// Highlight
			gfx_fill_rect(dpi, x, y, w->width, y + 9, 0x2000000 | 49);
			stringId = 1193;
		} else {
			stringId = 1191;
		}

		stringId2 = STR_BUILD_CUSTOM_DESIGN;
		gfx_draw_string_left(dpi, stringId, &stringId2, 0, x, y - 1);
		y += 10;
	}

	i++;
	while (*trackDesignItem != 0) {
		if (y + 10 >= dpi->y && y < dpi->y + dpi->height) {
			if (i == w->track_list.var_482) {
				// Highlight
				gfx_fill_rect(dpi, x, y, w->width, y + 9, 0x2000000 | 49);
				stringId = 1193;
			} else {
				stringId = 1191;
			}

			// Get name of track
			trackName = ch = (char*)0x009BC678;
			*ch++ = FORMAT_OPENQUOTES;
			for (j = 0; j < 128; j++) {
				if (trackDesignItem[j] == '.')
					break;
				*ch++ = trackDesignItem[j];
			}
			*ch++ = FORMAT_ENDQUOTES;
			*ch = 0;

			// Draw track name
			stringId2 = 3165;
			gfx_draw_string_left(dpi, stringId, &stringId2, 0, x, y - 1);
		}
		y += 10;
		i++;
		trackDesignItem += 128;
	}
}