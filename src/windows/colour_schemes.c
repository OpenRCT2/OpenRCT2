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
#include "../sprites.h"
#include "dropdown.h"
#include "../interface/colour_schemes.h"
#include "error.h"

enum {
	WINDOW_COLOUR_SCHEMES_TAB_MAIN_UI,
	WINDOW_COLOUR_SCHEMES_TAB_PARK,
	WINDOW_COLOUR_SCHEMES_TAB_TOOLS,
	WINDOW_COLOUR_SCHEMES_TAB_RIDES_PEEPS,
	WINDOW_COLOUR_SCHEMES_TAB_EDITORS,
	WINDOW_COLOUR_SCHEMES_TAB_MISC,
	WINDOW_COLOUR_SCHEMES_TAB_PROMPTS,
	WINDOW_COLOUR_SCHEMES_TAB_SETTINGS
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
static void window_colour_schemes_textinput();
static void window_colour_schemes_tooltip();
static void window_colour_schemes_invalidate();
static void window_colour_schemes_paint();
static void window_colour_schemes_scrollpaint();
static void window_colour_schemes_draw_tab_images(rct_drawpixelinfo *dpi, rct_window *w);

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
	window_colour_schemes_textinput,
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
	WIDX_COLOUR_SCHEMES_MAIN_UI_TAB,
	WIDX_COLOUR_SCHEMES_PARK_TAB,
	WIDX_COLOUR_SCHEMES_TOOLS_TAB,
	WIDX_COLOUR_SCHEMES_RIDE_PEEPS_TAB,
	WIDX_COLOUR_SCHEMES_EDITORS_TAB,
	WIDX_COLOUR_SCHEMES_MISC_TAB,
	WIDX_COLOUR_SCHEMES_PROMPTS_TAB,
	WIDX_COLOUR_SCHEMES_SETTINGS_TAB,
	WIDX_COLOUR_SCHEMES_PRESETS,
	WIDX_COLOUR_SCHEMES_PRESETS_DROPDOWN,
	WIDX_COLOUR_SCHEMES_DUPLICATE_BUTTON,
	WIDX_COLOUR_SCHEMES_DELETE_BUTTON,
	WIDX_COLOUR_SCHEMES_RENAME_BUTTON,
	WIDX_COLOUR_SCHEMES_COLORBTN_MASK,
	WIDX_COLOUR_SCHEMES_LIST,
};

static rct_widget window_colour_schemes_widgets[] = {
	{ WWT_FRAME,			0,	0,		319,	0,		269,	0x0FFFFFFFF,					STR_NONE },					// panel / background
	{ WWT_CAPTION,			0,	1,		318,	1,		14,		5244,							STR_WINDOW_TITLE_TIP },		// title bar
	{ WWT_CLOSEBOX,			0,	307,	317,	2,		13,		STR_CLOSE_X,					STR_CLOSE_WINDOW_TIP },		// close button
	{ WWT_RESIZE,			1,	0,		319,	43,		269,	0x0FFFFFFFF,					STR_NONE },					// tab content panel
	{ WWT_TAB,				1,	3,		33,		17,		43,		0x02000144E,					5228 },						// main ui tab
	{ WWT_TAB,				1,	34,		64,		17,		43,		0x02000144E,					5229 },						// park tab
	{ WWT_TAB,				1,	65,		95,		17,		43,		0x02000144E,					5230 },						// tools tab
	{ WWT_TAB,				1,	96,		126,	17,		43,		0x02000144E,					5231 },						// rides and peeps tab
	{ WWT_TAB,				1,	127,	157,	17,		43,		0x02000144E,					5232 },						// editors tab
	{ WWT_TAB,				1,	158,	188,	17,		43,		0x02000144E,					5233 },						// misc tab
	{ WWT_TAB,				1,	189,	219,	17,		43,		0x02000144E,					5234 },						// prompts tab
	{ WWT_TAB,				1,	220,	250,	17,		43,		0x02000144E,					5235 },						// settings tab
	{ WWT_DROPDOWN,			1,	125,	299,	60,		71,		STR_NONE,						STR_NONE },					// Preset colour schemes
	{ WWT_DROPDOWN_BUTTON,	1,	288,	298,	61,		70,		876,							STR_NONE },
	{ WWT_DROPDOWN_BUTTON,	1,	10,		100,	82,		93,		5239,							STR_NONE },					// Duplicate button
	{ WWT_DROPDOWN_BUTTON,	1,	110,	200,	82,		93,		3349,							STR_NONE },					// Delete button
	{ WWT_DROPDOWN_BUTTON,	1,	210,	300,	82,		93,		3348,							STR_NONE },					// Rename button
	{ WWT_COLORBTN,			1,	0,		0,		0,		0,		STR_NONE,						STR_NONE },					// color button mask
	{ WWT_SCROLL,			1,	3,		316,	60,		266,	2,								STR_NONE },					// staff list
	{ WIDGETS_END },
};

static int window_colour_schemes_tab_animation_loops[] = {
	32,
	1,
	1,
	64,
	32,
	8,
	14,
	32
};
static int window_colour_schemes_tab_animation_divisor[] = {
	4,
	1,
	1,
	4,
	2,
	2,
	2,
	4
};
static int window_colour_schemes_tab_sprites[] = {
	SPR_TAB_KIOSKS_AND_FACILITIES_0,
	5200,
	SPR_G2_TAB_LAND,
	SPR_TAB_RIDE_0,
	5205,
	5201,
	SPR_TAB_STAFF_OPTIONS_0,
	5221
};

static rct_windowclass window_colour_schemes_tab_1_classes[] = {
	WC_TOP_TOOLBAR,
	WC_BOTTOM_TOOLBAR,
	WC_EDITOR_SCENARIO_BOTTOM_TOOLBAR,
	WC_EDITOR_TRACK_BOTTOM_TOOLBAR,
	WC_TITLE_MENU,
	WC_TITLE_EXIT,
	WC_TITLE_OPTIONS,
	WC_SCENARIO_SELECT
};

static rct_windowclass window_colour_schemes_tab_2_classes[] = {
	WC_PARK_INFORMATION,
	WC_FINANCES,
	WC_NEW_CAMPAIGN,
	WC_RESEARCH,
	WC_MAP,
	WC_VIEWPORT,
	WC_RECENT_NEWS
};

static rct_windowclass window_colour_schemes_tab_3_classes[] = {
	WC_LAND,
	WC_WATER,
	WC_CLEAR_SCENERY,
	WC_LAND_RIGHTS,
	WC_SCENERY,
	WC_FOOTPATH,
	WC_RIDE_CONSTRUCTION,
	WC_TRACK_DESIGN_PLACE,
	WC_CONSTRUCT_RIDE,
	WC_TRACK_DESIGN_LIST
};

static rct_windowclass window_colour_schemes_tab_4_classes[] = {
	WC_RIDE,
	WC_RIDE_LIST,
	WC_PEEP,
	WC_GUEST_LIST,
	WC_STAFF,
	WC_STAFF_LIST,
	WC_BANNER
};

static rct_windowclass window_colour_schemes_tab_5_classes[] = {
	WC_EDITOR_OBJECT_SELECTION,
	WC_EDITOR_INVENTION_LIST,
	WC_EDITOR_SCENARIO_OPTIONS,
	WC_EDTIOR_OBJECTIVE_OPTIONS,
	WC_MAPGEN,
	WC_MANAGE_TRACK_DESIGN,
	WC_INSTALL_TRACK
};

static rct_windowclass window_colour_schemes_tab_6_classes[] = {
	WC_CHEATS,
	WC_COLOUR_SCHEMES,
	WC_OPTIONS,
	WC_KEYBOARD_SHORTCUT_LIST,
	WC_CHANGE_KEYBOARD_SHORTCUT,
	WC_LOADSAVE
};

static rct_windowclass window_colour_schemes_tab_7_classes[] = {
	WC_SAVE_PROMPT,
	WC_DEMOLISH_RIDE_PROMPT,
	WC_FIRE_PROMPT,
	WC_TRACK_DELETE_PROMPT,
	WC_LOADSAVE_OVERWRITE_PROMPT
};

// Info, Research, Wrench, Entrance, Slide, Gears, Point Finger
// Info, Entrance, Construction, Slide, Wrench, Gear, Todo
static uint8 _selected_tab = 0;
static sint16 _color_index_1 = -1;
static sint8 _color_index_2 = -1;
static const uint8 _row_height = 32;
static const uint8 _button_offset_x = 220;
static const uint8 _button_offset_y = 3;
static const uint8 _check_offset_y = 3 + 12 + 2;


void window_colour_schemes_init_vars()
{
	_selected_tab = WINDOW_COLOUR_SCHEMES_TAB_MAIN_UI;
}

static window_colour_scheme* get_colour_scheme_tab()
{
	switch (_selected_tab) {
	case 0: return colour_scheme_get_by_class(window_colour_schemes_tab_1_classes[_color_index_1]);
	case 1: return colour_scheme_get_by_class(window_colour_schemes_tab_2_classes[_color_index_1]);
	case 2: return colour_scheme_get_by_class(window_colour_schemes_tab_3_classes[_color_index_1]);
	case 3: return colour_scheme_get_by_class(window_colour_schemes_tab_4_classes[_color_index_1]);
	case 4: return colour_scheme_get_by_class(window_colour_schemes_tab_5_classes[_color_index_1]);
	case 5: return colour_scheme_get_by_class(window_colour_schemes_tab_6_classes[_color_index_1]);
	case 6: return colour_scheme_get_by_class(window_colour_schemes_tab_7_classes[_color_index_1]);
	}
	return NULL;
}
static window_colour_scheme* get_colour_scheme_tab_by_index(int index)
{
	switch (_selected_tab) {
	case 0: return colour_scheme_get_by_class(window_colour_schemes_tab_1_classes[index]);
	case 1: return colour_scheme_get_by_class(window_colour_schemes_tab_2_classes[index]);
	case 2: return colour_scheme_get_by_class(window_colour_schemes_tab_3_classes[index]);
	case 3: return colour_scheme_get_by_class(window_colour_schemes_tab_4_classes[index]);
	case 4: return colour_scheme_get_by_class(window_colour_schemes_tab_5_classes[index]);
	case 5: return colour_scheme_get_by_class(window_colour_schemes_tab_6_classes[index]);
	case 6: return colour_scheme_get_by_class(window_colour_schemes_tab_7_classes[index]);
	}
	return NULL;
}

static int get_colour_scheme_tab_count()
{
	switch (_selected_tab) {
	case 0: return sizeof(window_colour_schemes_tab_1_classes);
	case 1: return sizeof(window_colour_schemes_tab_2_classes);
	case 2: return sizeof(window_colour_schemes_tab_3_classes);
	case 3: return sizeof(window_colour_schemes_tab_4_classes);
	case 4: return sizeof(window_colour_schemes_tab_5_classes);
	case 5: return sizeof(window_colour_schemes_tab_6_classes);
	case 6: return sizeof(window_colour_schemes_tab_7_classes);
	}
	return 0;
}

static int get_colour_scheme_index() {
	switch (_selected_tab) {
	case 0: return colour_scheme_get_index_by_class(window_colour_schemes_tab_1_classes[_color_index_1]);
	case 1: return colour_scheme_get_index_by_class(window_colour_schemes_tab_2_classes[_color_index_1]);
	case 2: return colour_scheme_get_index_by_class(window_colour_schemes_tab_3_classes[_color_index_1]);
	case 3: return colour_scheme_get_index_by_class(window_colour_schemes_tab_4_classes[_color_index_1]);
	case 4: return colour_scheme_get_index_by_class(window_colour_schemes_tab_5_classes[_color_index_1]);
	case 5: return colour_scheme_get_index_by_class(window_colour_schemes_tab_6_classes[_color_index_1]);
	case 6: return colour_scheme_get_index_by_class(window_colour_schemes_tab_7_classes[_color_index_1]);
	}
	return -1;
}

static void window_colour_schemes_draw_tab_images(rct_drawpixelinfo *dpi, rct_window *w)
{
	int sprite_idx;

	for (int i = 0; i < 8; i++) {
		sprite_idx = window_colour_schemes_tab_sprites[i];
		if (_selected_tab == i)
			sprite_idx += w->frame_no / window_colour_schemes_tab_animation_divisor[_selected_tab];
		gfx_draw_sprite(dpi, sprite_idx, w->x + w->widgets[WIDX_COLOUR_SCHEMES_MAIN_UI_TAB + i].left, w->y + w->widgets[WIDX_COLOUR_SCHEMES_MAIN_UI_TAB + i].top, 0);
	}
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
		(1 << WIDX_COLOUR_SCHEMES_MAIN_UI_TAB) |
		(1 << WIDX_COLOUR_SCHEMES_PARK_TAB) |
		(1 << WIDX_COLOUR_SCHEMES_TOOLS_TAB) |
		(1 << WIDX_COLOUR_SCHEMES_RIDE_PEEPS_TAB) |
		(1 << WIDX_COLOUR_SCHEMES_EDITORS_TAB) |
		(1 << WIDX_COLOUR_SCHEMES_MISC_TAB) |
		(1 << WIDX_COLOUR_SCHEMES_PROMPTS_TAB) |
		(1 << WIDX_COLOUR_SCHEMES_SETTINGS_TAB) |
		(1 << WIDX_COLOUR_SCHEMES_COLORBTN_MASK) |
		(1 << WIDX_COLOUR_SCHEMES_PRESETS) |
		(1 << WIDX_COLOUR_SCHEMES_PRESETS_DROPDOWN) |
		(1 << WIDX_COLOUR_SCHEMES_DUPLICATE_BUTTON) |
		(1 << WIDX_COLOUR_SCHEMES_DELETE_BUTTON) |
		(1 << WIDX_COLOUR_SCHEMES_RENAME_BUTTON);

	window_colour_schemes_widgets[WIDX_COLOUR_SCHEMES_LIST].type = WWT_SCROLL;

	window_colour_schemes_init_vars();

	window_init_scroll_widgets(window);
	window->list_information_type = 0;
	_color_index_1 = -1;
	_color_index_2 = -1;
	window->min_width = 320;
	window->min_height = 270;
	window->max_width = 320;
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
	case WIDX_COLOUR_SCHEMES_DUPLICATE_BUTTON:
		window_text_input_open(w, widgetIndex, 5239, 5240, 1170, (uint32)&gConfigColourSchemes.presets[gCurrentColourSchemePreset].name, 64);
		break;
	case WIDX_COLOUR_SCHEMES_DELETE_BUTTON:
		if (gCurrentColourSchemePreset >= 2) {
			colour_scheme_delete_preset(gCurrentColourSchemePreset);
		}
		else {
			window_error_open(5241, STR_NONE);
		}
		break;
	case WIDX_COLOUR_SCHEMES_RENAME_BUTTON:
		if (gCurrentColourSchemePreset >= 2) {
			window_text_input_open(w, widgetIndex, 3348, 5240, 1170, (uint32)&gConfigColourSchemes.presets[gCurrentColourSchemePreset].name, 64);
		}
		else {
			window_error_open(5241, STR_NONE);
		}
		break;
	}
}

static void window_colour_schemes_resize()
{
	rct_window *w;

	window_get_register(w);

	if (_selected_tab == WINDOW_COLOUR_SCHEMES_TAB_SETTINGS) {
		window_set_resize(w, 320, 107, 320, 107);
		gfx_invalidate_screen();
	}
	else {
		w->min_width = 320;
		w->min_height = 270;
		w->max_width = 320;
		w->max_height = 450;

		if (w->width < w->min_width) {
			w->width = w->min_width;
			window_invalidate(w);
		}
		if (w->height < w->min_height) {
			w->height = w->min_height;
			window_invalidate(w);
		}
		if (w->width > w->max_width) {
			w->width = w->max_width;
			window_invalidate(w);
		}
		if (w->height > w->max_height) {
			w->height = w->max_height;
			window_invalidate(w);
		}
	}
}

static void window_colour_schemes_mousedown(int widgetIndex, rct_window* w, rct_widget* widget)
{
	short newSelectedTab;
	int num_items, i;

	switch (widgetIndex) {
	case WIDX_COLOUR_SCHEMES_MAIN_UI_TAB:
	case WIDX_COLOUR_SCHEMES_PARK_TAB:
	case WIDX_COLOUR_SCHEMES_TOOLS_TAB:
	case WIDX_COLOUR_SCHEMES_RIDE_PEEPS_TAB:
	case WIDX_COLOUR_SCHEMES_EDITORS_TAB:
	case WIDX_COLOUR_SCHEMES_MISC_TAB:
	case WIDX_COLOUR_SCHEMES_PROMPTS_TAB:
	case WIDX_COLOUR_SCHEMES_SETTINGS_TAB:
		/*if (widgetIndex == WIDX_COLOUR_SCHEMES_SETTINGS_TAB) {
			window_set_resize(w, 320, 100, 320, 100);
			w->flags &= ~WF_RESIZABLE;
		}
		else if (_selected_tab == WINDOW_COLOUR_SCHEMES_TAB_SETTINGS) {
			w->flags |= WF_RESIZABLE;
			window_set_resize(w, 320, 270, 320, 450);
		}*/
		newSelectedTab = widgetIndex - WIDX_COLOUR_SCHEMES_MAIN_UI_TAB;
		if (_selected_tab == newSelectedTab)
			break;
		_selected_tab = (uint8)newSelectedTab;
		w->scrolls[0].v_top = 0;
		w->frame_no = 0;
		window_event_resize_call(w);
		window_invalidate(w);
		break;
	case WIDX_COLOUR_SCHEMES_PRESETS_DROPDOWN:
		num_items = gConfigColourSchemes.num_presets;

		widget--;
		for (i = 0; i < num_items; i++) {
			gDropdownItemsFormat[i] = 2777;
			gDropdownItemsArgs[i] = (uint64)&gConfigColourSchemes.presets[i].name;
		}

		window_dropdown_show_text_custom_width(
			w->x + widget->left,
			w->y + widget->top,
			widget->bottom - widget->top + 1,
			w->colours[1],
			DROPDOWN_FLAG_STAY_OPEN,
			num_items,
			widget->right - widget->left - 3
			);

		gDropdownItemsChecked = 1 << gCurrentColourSchemePreset;
		break;
	}
}

static void window_colour_schemes_dropdown()
{
	rct_window* w;
	short widgetIndex, dropdownIndex;
	window_dropdown_get_registers(w, widgetIndex, dropdownIndex);

	switch (widgetIndex) {
	case WIDX_COLOUR_SCHEMES_LIST:
		if (dropdownIndex != -1) {
			get_colour_scheme_tab()->colours[_color_index_2] = dropdownIndex | get_colour_scheme_tab()->colours[_color_index_2] & 0x80;
			gConfigColourSchemes.presets[gCurrentColourSchemePreset].colour_schemes[get_colour_scheme_index()].colours[_color_index_2] = dropdownIndex | get_colour_scheme_tab()->colours[_color_index_2] & 0x80;
			window_invalidate_all();
			_color_index_1 = -1;
			_color_index_2 = -1;

			if (gCurrentColourSchemePreset >= 2)
				colour_schemes_save_preset(gCurrentColourSchemePreset);
		}
		break;
	case WIDX_COLOUR_SCHEMES_PRESETS_DROPDOWN:
		if (dropdownIndex != -1) {
			colour_scheme_change_preset(dropdownIndex);
		}
		config_save_default();
		break;
	}

}

void window_colour_schemes_update(rct_window *w)
{
	w->frame_no++;
	if (w->frame_no >= window_colour_schemes_tab_animation_loops[_selected_tab])
		w->frame_no = 0;

	widget_invalidate(w, WIDX_COLOUR_SCHEMES_MAIN_UI_TAB + _selected_tab);

}

void window_colour_schemes_scrollgetsize() {
	rct_window *w;

	window_get_register(w);

	int scrollHeight = get_colour_scheme_tab_count() * _row_height;
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
	short x, y, scrollIndex;
	rct_window *w;

	window_scrollmouse_get_registers(w, scrollIndex, x, y);

	if (_selected_tab == 7)
		return;

	if (y / _row_height < get_colour_scheme_tab_count()) {
		int y2 = y % _row_height;
		_color_index_1 = y / _row_height;
		_color_index_2 = ((x - _button_offset_x) / 12);
		if (_color_index_2 < get_colour_scheme_tab()->num_colours) {
			if (x >= _button_offset_x && x < _button_offset_x + 12 * 6 && y2 >= _button_offset_y && y2 < _button_offset_y + 11) {
				window_colour_schemes_widgets[WIDX_COLOUR_SCHEMES_COLORBTN_MASK].left = _button_offset_x + _color_index_2 * 12 + window_colour_schemes_widgets[WIDX_COLOUR_SCHEMES_LIST].left;
				window_colour_schemes_widgets[WIDX_COLOUR_SCHEMES_COLORBTN_MASK].top = _color_index_1 * _row_height + _button_offset_y - w->scrolls[0].v_top + window_colour_schemes_widgets[WIDX_COLOUR_SCHEMES_LIST].top;
				window_colour_schemes_widgets[WIDX_COLOUR_SCHEMES_COLORBTN_MASK].right = window_colour_schemes_widgets[WIDX_COLOUR_SCHEMES_COLORBTN_MASK].left + 11;
				window_colour_schemes_widgets[WIDX_COLOUR_SCHEMES_COLORBTN_MASK].bottom = window_colour_schemes_widgets[WIDX_COLOUR_SCHEMES_COLORBTN_MASK].top + 11;
				window_dropdown_show_colour(w, &(window_colour_schemes_widgets[WIDX_COLOUR_SCHEMES_COLORBTN_MASK]), w->colours[1], get_colour_scheme_tab()->colours[_color_index_2]);
			}
			else if (x >= _button_offset_x && x < _button_offset_x + 12 * 6 - 1 && y2 >= _check_offset_y && y2 < _check_offset_y + 11) {
				if (get_colour_scheme_tab()->colours[_color_index_2] & 0x80) {
					get_colour_scheme_tab()->colours[_color_index_2] &= 0x7F;

					gConfigColourSchemes.presets[gCurrentColourSchemePreset].colour_schemes[get_colour_scheme_index()].colours[_color_index_2] &= 0x7F;
				}
				else {
					get_colour_scheme_tab()->colours[_color_index_2] |= 0x80;
					gConfigColourSchemes.presets[gCurrentColourSchemePreset].colour_schemes[get_colour_scheme_index()].colours[_color_index_2] |= 0x80;
				}
				if (gCurrentColourSchemePreset >= 2)
					colour_schemes_save_preset(gCurrentColourSchemePreset);
				window_invalidate_all();
			}
		}
	}
}

void window_colour_schemes_scrollmouseover() {
	short x, y, scrollIndex;
	rct_window *w;

	window_scrollmouse_get_registers(w, scrollIndex, x, y);

	//if (_selected_tab == 7)
	//	return;
}

static bool valid_characters(const char *name)
{
	for (int i = 0; name[i] != '\0'; i++) {
		if (name[i] == '\\' || name[i] == '/' || name[i] == ':' || name[i] == '?' || name[i] == '*' || name[i] == '<' || name[i] == '>' || name[i] == '|')
			return false;
	}
	return true;
}

static void window_colour_schemes_textinput()
{
	rct_window *w;
	short widgetIndex;
	uint8 result;
	char *text;

	window_textinput_get_registers(w, widgetIndex, result, text);

	if (!result || text[0] == 0)
		return;

	switch (widgetIndex) {
	case WIDX_COLOUR_SCHEMES_DUPLICATE_BUTTON:
	case WIDX_COLOUR_SCHEMES_RENAME_BUTTON:
		if (valid_characters(text)) {
			bool nameTaken = false;
			for (int i = 0; i < gConfigColourSchemes.num_presets; i++) {
				if (strcmp(gConfigColourSchemes.presets[i].name, text) == 0) {
					window_error_open(5242, STR_NONE);
					nameTaken = true;
					break;
				}
			}
			if (!nameTaken) {
				if (widgetIndex == WIDX_COLOUR_SCHEMES_DUPLICATE_BUTTON) {
					colour_scheme_create_preset(text);
				}
				else {
					colour_scheme_rename_preset(gCurrentColourSchemePreset, text);
				}
				config_save_default();
				window_invalidate(w);
			}
		}
		else {
			window_error_open(5243, STR_NONE);
		}
		break;
	}
}

void window_colour_schemes_tooltip()
{
	RCT2_GLOBAL(0x013CE952, uint16) = STR_LIST;
}

void window_colour_schemes_invalidate()
{
	rct_window *w;

	window_get_register(w);
	colour_scheme_update(w);

	int pressed_widgets = w->pressed_widgets & 0xFFFFF00F;
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

	if (_selected_tab == 7) {
		window_colour_schemes_widgets[WIDX_COLOUR_SCHEMES_LIST].type = WWT_EMPTY;
	}
	else {
		window_colour_schemes_widgets[WIDX_COLOUR_SCHEMES_LIST].type = WWT_SCROLL;
	}
}

void window_colour_schemes_paint() {
	rct_window *w;
	rct_drawpixelinfo *dpi;

	window_paint_get_registers(w, dpi);

	// Widgets
	window_draw_widgets(w, dpi);
	window_colour_schemes_draw_tab_images(dpi, w);

	if (_selected_tab < 7) {

		gfx_draw_string_left(dpi, 5236, w, w->colours[1], w->x + 6, 58 - 12 + w->y + 1);
		gfx_draw_string_left(dpi, 5237, w, w->colours[1], w->x + 220, 58 - 12 + w->y + 1);
	}
	else {
		RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS + 0, uint32) = (uint32)&gConfigColourSchemes.presets[gCurrentColourSchemePreset].name;
		gfx_draw_string_left(dpi, 5238, NULL, w->colours[1], w->x + 10, w->y + window_colour_schemes_widgets[WIDX_COLOUR_SCHEMES_PRESETS].top + 1);
		gfx_draw_string_left(
			dpi,
			1170,
			(void*)RCT2_ADDRESS_COMMON_FORMAT_ARGS,
			w->colours[1],
			w->x + window_colour_schemes_widgets[WIDX_COLOUR_SCHEMES_PRESETS].left + 1,
			w->y + window_colour_schemes_widgets[WIDX_COLOUR_SCHEMES_PRESETS].top
			);
	}
}

/**
*
*  rct2: 0x006BD785
*/
void window_colour_schemes_scrollpaint()
{
	int y;
	uint8 selectedTab;
	rct_window *w;
	rct_drawpixelinfo *dpi;

	window_paint_get_registers(w, dpi);

	if (_selected_tab == 7)
		return;

	if ((w->colours[1] & 0x80) == 0)
		//gfx_fill_rect(dpi, dpi->x, dpi->y, dpi->x + dpi->width - 1, dpi->y + dpi->height - 1, ((char*)0x0141FC48)[w->colours[1] * 8]);
		gfx_clear(dpi, ((char*)0x0141FC48)[w->colours[1] * 8] * 0x1010101);
	y = 0;
	selectedTab = _selected_tab;
	for (int i = 0; i < get_colour_scheme_tab_count(); i++) {
		if (y > dpi->y + dpi->height) {
			break;
		}
		if (y + _row_height >= dpi->y) {
			if (i + 1 < get_colour_scheme_tab_count()) {
				int colour = w->colours[1];
				if (colour & 0x80) {
					colour = RCT2_ADDRESS(0x009DEDF4, uint8)[colour];

					colour = colour | 0x2000000;
					gfx_fill_rect(dpi, 0, y + _row_height - 2, window_colour_schemes_widgets[WIDX_COLOUR_SCHEMES_LIST].right, y + _row_height - 2, colour + 1);
					gfx_fill_rect(dpi, 0, y + _row_height - 1, window_colour_schemes_widgets[WIDX_COLOUR_SCHEMES_LIST].right, y + _row_height - 1, colour + 2);
				}
				else {
					colour = RCT2_ADDRESS(0x0141FC47, uint8)[w->colours[1] * 8];
					gfx_fill_rect(dpi, 0, y + _row_height - 2, window_colour_schemes_widgets[WIDX_COLOUR_SCHEMES_LIST].right, y + _row_height - 2, colour);
					colour = RCT2_ADDRESS(0x0141FC4B, uint8)[w->colours[1] * 8];
					gfx_fill_rect(dpi, 0, y + _row_height - 1, window_colour_schemes_widgets[WIDX_COLOUR_SCHEMES_LIST].right, y + _row_height - 1, colour);
				}
			}
			//gfx_fill_rect_inset(dpi, 0, y + _row_height - 2, window_colour_schemes_widgets[WIDX_COLOUR_SCHEMES_LIST].right + 1, y + _row_height - 1, w->colours[1], 0x20);

			for (int j = 0; j < get_colour_scheme_tab_by_index(i)->num_colours; j++) {

				gfx_draw_string_left(dpi, get_colour_scheme_tab_by_index(i)->name, NULL, w->colours[1], 2, y + 4);

				uint32 image = ((get_colour_scheme_tab_by_index(i)->colours[j] & 0x7F) << 19) + 0x600013C3;
				if (i == _color_index_1 && j == _color_index_2) {
					image = ((get_colour_scheme_tab_by_index(i)->colours[j] & 0x7F) << 19) + 0x600013C4;
				}
				gfx_draw_sprite(dpi, image, _button_offset_x + 12 * j, y + _button_offset_y, 0);

				gfx_fill_rect_inset(dpi, _button_offset_x + 12 * j, y + _check_offset_y, _button_offset_x + 12 * j + 9, y + _check_offset_y + 10, w->colours[1], 0xE0);
				if (get_colour_scheme_tab_by_index(i)->colours[j] & 0x80) {
					RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_FONT_SPRITE_BASE, sint16) = -1;
					gfx_draw_string(dpi, (char*)0x009DED72, w->colours[1] & 0x7F, _button_offset_x + 12 * j, y + _check_offset_y);
				}

			}
		}

		y += _row_height;
	}
}
