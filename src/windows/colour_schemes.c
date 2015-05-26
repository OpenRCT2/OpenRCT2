/*****************************************************************************
* Copyright (c) 2014 Maciek Baron, Dániel Tar
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
#include "../game.h"
#include "../drawing/drawing.h"
#include "../input.h"
#include "../interface/viewport.h"
#include "../interface/widget.h"
#include "../interface/window.h"
#include "../localisation/localisation.h"
#include "../peep/peep.h"
#include "../peep/staff.h"
#include "../world/sprite.h"
#include "dropdown.h"

enum {
	WINDOW_COLOUR_SCHEMES_TAB_WINDOWS,
	WINDOW_COLOUR_SCHEMES_TAB_PRESETS,
} WINDOW_COLOUR_SCHEMES_TAB;

static void window_colour_schemes_emptysub() { }
static void window_colour_schemes_close();
static void window_colour_schemes_mouseup();
static void window_colour_schemes_resize();
static void window_colour_schemes_mousedown(int widgetIndex, rct_window*w, rct_widget* widget);
static void window_colour_schemes_dropdown();
static void window_colour_schemes_update(rct_window *w);
static void window_colour_schemes_scrollgetsize();
static void window_colour_schemes_scrollmousedown();
static void window_colour_schemes_scrollmouseover();
static void window_colour_schemes_tooltip();
static void window_colour_schemes_invalidate();
static void window_colour_schemes_paint();
static void window_colour_schemes_scrollpaint();

static void* window_colour_schemes_events[] = {
	window_colour_schemes_close,
	window_colour_schemes_mouseup,
	window_colour_schemes_resize,
	window_colour_schemes_mousedown,
	window_colour_schemes_dropdown,
	window_colour_schemes_emptysub,
	window_colour_schemes_update,
	window_colour_schemes_emptysub,
	window_colour_schemes_emptysub,
	window_colour_schemes_emptysub,
	window_colour_schemes_emptysub,
	window_colour_schemes_emptysub,
	window_colour_schemes_emptysub,
	window_colour_schemes_emptysub,
	window_colour_schemes_emptysub,
	window_colour_schemes_scrollgetsize,
	window_colour_schemes_scrollmousedown,
	window_colour_schemes_emptysub,
	window_colour_schemes_scrollmouseover,
	window_colour_schemes_emptysub,
	window_colour_schemes_emptysub,
	window_colour_schemes_emptysub,
	window_colour_schemes_tooltip,
	window_colour_schemes_emptysub,
	window_colour_schemes_emptysub,
	window_colour_schemes_invalidate,
	window_colour_schemes_paint,
	window_colour_schemes_scrollpaint,
};

enum WINDOW_STAFF_LIST_WIDGET_IDX {
	WIDX_COLOUR_SCHEMES_BACKGROUND,
	WIDX_COLOUR_SCHEMES_TITLE,
	WIDX_COLOUR_SCHEMES_CLOSE,
	WIDX_COLOUR_SCHEMES_TAB_CONTENT_PANEL,
	WIDX_COLOUR_SCHEMES_WINDOWS_TAB,
	WIDX_COLOUR_SCHEMES_PRESETS_TAB,
	WIDX_COLOUR_SCHEMES_COLORBTN_MASK,
	WIDX_COLOUR_SCHEMES_LIST,
};

static rct_widget window_colour_schemes_widgets[] = {
	{ WWT_FRAME,			0,	0,		319,	0,		269,	0x0FFFFFFFF,					STR_NONE },							// panel / background
	{ WWT_CAPTION,			0,	1,		318,	1,		14,		STR_STAFF,						STR_WINDOW_TITLE_TIP },				// title bar
	{ WWT_CLOSEBOX,			0,	307,	317,	2,		13,		STR_CLOSE_X,					STR_CLOSE_WINDOW_TIP },				// close button
	{ WWT_RESIZE,			1,	0,		319,	43,		269,	0x0FFFFFFFF,					STR_NONE },							// tab content panel
	{ WWT_TAB,				1,	3,		33,		17,		43,		0x02000144E,					STR_NONE },							// windows tab
	{ WWT_TAB,				1,	34,		64,		17,		43,		0x02000144E,					STR_NONE },							// presets tab
	{ WWT_COLORBTN,			1,	0,		0,		0,		0,		STR_NONE,						STR_NONE },							// color button mask
	{ WWT_SCROLL,			1,	3,		316,	72,		266,	3,								STR_NONE },							// staff list
	{ WIDGETS_END },
};


static uint8 _window_colors[16][6];
static sint16 _color_index_1 = -1;
static sint8 _color_index_2 = -1;
static uint8 _selected_tab = 0;
static const uint8 _row_height = 16;
static const uint8 _button_offset_x = 110;
static const uint8 _button_offset_y = 2;

static uint16 _window_staff_list_selected_type_count = 0;
// TODO: These are still referenced in non-decompiled code
//static int _window_staff_list_highlighted_index;
//static int _window_staff_list_selected_tab;

void window_colour_schemes_init_vars()
{
	_selected_tab = WINDOW_COLOUR_SCHEMES_TAB_WINDOWS;
}

void window_colour_schemes_open()
{
	rct_window* window;

	// Check if window is already open
	window = window_bring_to_front_by_class(WC_COLOUR_SCHEMES);
	if (window != NULL)
		return;

	window = window_create_auto_pos(320, 270, (uint32*)window_colour_schemes_events, WC_COLOUR_SCHEMES, 0x0400);
	window->widgets = window_colour_schemes_widgets;
	window->enabled_widgets =
		(1 << WIDX_COLOUR_SCHEMES_CLOSE) |
		(1 << WIDX_COLOUR_SCHEMES_WINDOWS_TAB) |
		(1 << WIDX_COLOUR_SCHEMES_PRESETS_TAB) |
		(1 << WIDX_COLOUR_SCHEMES_COLORBTN_MASK);

	window_init_scroll_widgets(window);
	window->list_information_type = 0;
	_color_index_1 = -1;
	_color_index_2 = -1;
	window->min_width = 320;
	window->min_height = 270;
	window->max_width = 500;
	window->max_height = 450;
	window->flags |= WF_RESIZABLE;
}

void window_colour_schemes_close() {
	rct_window *w;

	window_get_register(w);
}

static void window_colour_schemes_mouseup()
{
	short widgetIndex;
	rct_window *w;

	window_widget_get_registers(w, widgetIndex);

	switch (widgetIndex) {
	case WIDX_COLOUR_SCHEMES_CLOSE:
		window_close(w);
		break;
	}
}

static void window_colour_schemes_resize()
{
	rct_window *w;

	window_get_register(w);

	w->min_width = 320;
	w->min_height = 270;
	if (w->width < w->min_width) {
		w->width = w->min_width;
		window_invalidate(w);
	}
	if (w->height < w->min_height) {
		w->height = w->min_height;
		window_invalidate(w);
	}
}

static void window_colour_schemes_mousedown(int widgetIndex, rct_window* w, rct_widget* widget)
{
	short newSelectedTab;

	switch (widgetIndex) {
	case WIDX_COLOUR_SCHEMES_WINDOWS_TAB:
	case WIDX_COLOUR_SCHEMES_PRESETS_TAB:
		newSelectedTab = widgetIndex - WIDX_COLOUR_SCHEMES_WINDOWS_TAB;
		if (_selected_tab == newSelectedTab)
			break;
		_selected_tab = (uint8)newSelectedTab;
		window_invalidate(w);
		w->scrolls[0].v_top = 0;
		break;
	}


}

static void window_colour_schemes_dropdown()
{
	rct_window* w;
	short widgetIndex, dropdownIndex;
	window_dropdown_get_registers(w, widgetIndex, dropdownIndex);

	if (widgetIndex == WIDX_COLOUR_SCHEMES_LIST && dropdownIndex != -1) {
		_window_colors[_color_index_1][_color_index_2] = dropdownIndex;
		window_invalidate_all();
	}
	_color_index_1 = -1;
	_color_index_2 = -1;
}

void window_colour_schemes_update(rct_window *w)
{
	/*int spriteIndex;
	rct_peep *peep;

	w->list_information_type++;
	if (w->list_information_type >= 24) {
		w->list_information_type = 0;
	}
	else {
		widget_invalidate(w, WIDX_COLOUR_SCHEMES_WINDOWS_TAB + _selected_tab);
	}*/
}

void window_colour_schemes_scrollgetsize() {
	int spriteIndex;
	rct_peep *peep;
	rct_window *w;

	window_get_register(w);

	uint16 staffCount = 15;

	_window_staff_list_selected_type_count = staffCount;

	if (RCT2_GLOBAL(RCT2_ADDRESS_STAFF_HIGHLIGHTED_INDEX, short) != -1) {
		RCT2_GLOBAL(RCT2_ADDRESS_STAFF_HIGHLIGHTED_INDEX, short) = -1;
		window_invalidate(w);
	}

	int rowHeight = 15;

	int scrollHeight = staffCount * rowHeight;
	int i = scrollHeight - window_colour_schemes_widgets[WIDX_COLOUR_SCHEMES_LIST].bottom + window_colour_schemes_widgets[WIDX_COLOUR_SCHEMES_LIST].top + 21;
	if (i < 0)
		i = 0;
	if (i < w->scrolls[0].v_top) {
		w->scrolls[0].v_top = i;
		window_invalidate(w);
	}

	#ifdef _MSC_VER
		__asm mov ecx, 420
	#else
		__asm__("mov ecx, 420 ");
	#endif

	#ifdef _MSC_VER
		__asm mov edx, scrollHeight
	#else
		__asm__("mov edx, %[scrollHeight] " : [scrollHeight] "+m" (scrollHeight));
	#endif
}

void window_colour_schemes_scrollmousedown() {
	int i, spriteIndex;
	short x, y, scrollIndex;
	uint8 selectedTab;
	rct_window *w;
	rct_drawpixelinfo *dpi;

	window_scrollmouse_get_registers(w, scrollIndex, x, y);
	
	int rowHeight = 15;
	selectedTab = _selected_tab;
	if (y / rowHeight < 15) {
		if (x >= 110 && x < 110 + 12 * 6) {
			_color_index_1 = y / rowHeight;
			_color_index_2 = ((x - 110) / 12);
			window_colour_schemes_widgets[WIDX_COLOUR_SCHEMES_COLORBTN_MASK].left = 110 + _color_index_2 * 12 + window_colour_schemes_widgets[WIDX_COLOUR_SCHEMES_LIST].left;
			window_colour_schemes_widgets[WIDX_COLOUR_SCHEMES_COLORBTN_MASK].top = _color_index_1 * rowHeight - scrollIndex + window_colour_schemes_widgets[WIDX_COLOUR_SCHEMES_LIST].top;
			window_colour_schemes_widgets[WIDX_COLOUR_SCHEMES_COLORBTN_MASK].right = window_colour_schemes_widgets[WIDX_COLOUR_SCHEMES_COLORBTN_MASK].left + 11;
			window_colour_schemes_widgets[WIDX_COLOUR_SCHEMES_COLORBTN_MASK].bottom = window_colour_schemes_widgets[WIDX_COLOUR_SCHEMES_COLORBTN_MASK].top + 11;
			window_dropdown_show_colour(w, &(window_colour_schemes_widgets[WIDX_COLOUR_SCHEMES_COLORBTN_MASK]), w->colours[1], _window_colors[_color_index_1][_color_index_2]);
		}
	}

	/*for (int i = y / rowHeight; i >= 0; i--) {
		if (i == 0) {
			if (x >= 110 && x < 110 + 12 * 6) {
				_color_index_1 = i;
				_color_index_2 = ((x - 110) / 12);
				window_dropdown_show_colour(w, &(w->widgets[WIDX_COLOUR_SCHEMES_LIST]), w->colours[1], _window_colors[_color_index_1][_color_index_2]);
			}
			break;
		}

		i--;
	}*/
}

void window_colour_schemes_scrollmouseover() {
	int i;
	short x, y, scrollIndex;
	rct_window *w;

	window_scrollmouse_get_registers(w, scrollIndex, x, y);

	/*i = y / 10;
	if (i != RCT2_GLOBAL(RCT2_ADDRESS_STAFF_HIGHLIGHTED_INDEX, short)) {
		RCT2_GLOBAL(RCT2_ADDRESS_STAFF_HIGHLIGHTED_INDEX, short) = i;
		window_invalidate(w);
	}*/
}

void window_colour_schemes_tooltip()
{
	RCT2_GLOBAL(0x013CE952, uint16) = STR_LIST;
}

void window_colour_schemes_invalidate()
{
	rct_window *w;

	window_get_register(w);

	if (!gConfigInterface.rct1_colour_scheme)
	{
		w->colours[0] = 1;
		w->colours[1] = 4;
		w->colours[2] = 4;
	}
	else
	{
		w->colours[0] = 12;
		w->colours[1] = 4;
		w->colours[2] = 4;
	}

	int pressed_widgets = w->pressed_widgets & 0xFFFFFFCF;
	uint8 tabIndex = _selected_tab;
	uint8 widgetIndex = tabIndex + 4;

	w->pressed_widgets = pressed_widgets | (1 << widgetIndex);

	if (window_find_by_class(WC_DROPDOWN) == NULL) {
		_color_index_1 = -1;
		_color_index_2 = -1;
	}


	window_colour_schemes_widgets[WIDX_COLOUR_SCHEMES_BACKGROUND].right = w->width - 1;
	window_colour_schemes_widgets[WIDX_COLOUR_SCHEMES_BACKGROUND].bottom = w->height - 1;
	window_colour_schemes_widgets[WIDX_COLOUR_SCHEMES_TAB_CONTENT_PANEL].right = w->width - 1;
	window_colour_schemes_widgets[WIDX_COLOUR_SCHEMES_TAB_CONTENT_PANEL].bottom = w->height - 1;
	window_colour_schemes_widgets[WIDX_COLOUR_SCHEMES_TITLE].right = w->width - 2;
	window_colour_schemes_widgets[WIDX_COLOUR_SCHEMES_CLOSE].left = w->width - 2 - 0x0B;
	window_colour_schemes_widgets[WIDX_COLOUR_SCHEMES_CLOSE].right = w->width - 2 - 0x0B + 0x0A;
	window_colour_schemes_widgets[WIDX_COLOUR_SCHEMES_LIST].right = w->width - 4;
	window_colour_schemes_widgets[WIDX_COLOUR_SCHEMES_LIST].bottom = w->height - 0x0F;
}

void window_colour_schemes_paint() {
	int i;
	uint8 selectedTab;
	rct_window *w;
	rct_drawpixelinfo *dpi;

	window_paint_get_registers(w, dpi);

	// Widgets
	window_draw_widgets(w, dpi);

	selectedTab = RCT2_GLOBAL(RCT2_ADDRESS_WINDOW_STAFF_LIST_SELECTED_TAB, uint8);

	// Handymen tab image
	/*i = (selectedTab == 0 ? w->list_information_type & 0x0FFFFFFFC : 0);
	i += RCT2_ADDRESS(RCT2_GLOBAL(0x00982710, int), int)[0] + 1;
	i |= 0x20000000;
	i |= RCT2_GLOBAL(RCT2_ADDRESS_HANDYMAN_COLOUR, uint8) << 19;
	gfx_draw_sprite(
		dpi,
		i,
		(window_colour_schemes_widgets[WIDX_COLOUR_SCHEMES_WINDOWS_TAB].left + window_colour_schemes_widgets[WIDX_COLOUR_SCHEMES_WINDOWS_TAB].right) / 2 + w->x,
		window_colour_schemes_widgets[WIDX_COLOUR_SCHEMES_WINDOWS_TAB].bottom - 6 + w->y, 0
		);*/

}

/**
*
*  rct2: 0x006BD785
*/
void window_colour_schemes_scrollpaint()
{
	int spriteIndex, y;
	uint8 selectedTab;
	rct_window *w;
	rct_drawpixelinfo *dpi;

	window_paint_get_registers(w, dpi);

	gfx_fill_rect(dpi, dpi->x, dpi->y, dpi->x + dpi->width - 1, dpi->y + dpi->height - 1, ((char*)0x0141FC48)[w->colours[1] * 8]);
	int rowHeight = 15;
	y = 0;
	selectedTab = _selected_tab;
	for (int i = 0; i < 15; i++) {
		if (y > dpi->y + dpi->height) {
			break;
		}
		if (y + rowHeight >= dpi->y) {
			for (int j = 0; j < 6; j++) {
				uint32 image = (_window_colors[i][j] << 19) + 0x600013C3;
				if (i == _color_index_1 && j == _color_index_2) {
					image = (_window_colors[i][j] << 19) + 0x600013C4;
				}
				gfx_draw_sprite(dpi, image, 110 + 12 * j, y - 1, 0);
			}
		}

		y += rowHeight;
	}
}
