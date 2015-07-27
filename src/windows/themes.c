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
#include "../interface/themes.h"
#include "error.h"
#include "../util/util.h"

enum {
	WINDOW_THEMES_TAB_SETTINGS,
	WINDOW_THEMES_TAB_MAIN_UI,
	WINDOW_THEMES_TAB_PARK,
	WINDOW_THEMES_TAB_TOOLS,
	WINDOW_THEMES_TAB_RIDES_PEEPS,
	WINDOW_THEMES_TAB_EDITORS,
	WINDOW_THEMES_TAB_MISC,
	WINDOW_THEMES_TAB_PROMPTS,
	WINDOW_THEMES_TAB_FEATURES,
	WINDOW_THEMES_TAB_COUNT
} WINDOW_THEMES_TAB;

static void window_themes_mouseup(rct_window *w, int widgetIndex);
static void window_themes_resize(rct_window *w);
static void window_themes_mousedown(int widgetIndex, rct_window*w, rct_widget* widget);
static void window_themes_dropdown(rct_window *w, int widgetIndex, int dropdownIndex);
static void window_themes_update(rct_window *w);
static void window_themes_scrollgetsize(rct_window *w, int scrollIndex, int *width, int *height);
static void window_themes_scrollmousedown(rct_window *w, int scrollIndex, int x, int y);
static void window_themes_scrollmouseover(rct_window *w, int scrollIndex, int x, int y);
static void window_themes_textinput(rct_window *w, int widgetIndex, char *text);
static void window_themes_tooltip(rct_window* w, int widgetIndex, rct_string_id *stringId);
static void window_themes_invalidate(rct_window *w);
static void window_themes_paint(rct_window *w, rct_drawpixelinfo *dpi);
static void window_themes_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, int scrollIndex);
static void window_themes_draw_tab_images(rct_drawpixelinfo *dpi, rct_window *w);

static rct_window_event_list window_themes_events = {
	NULL,
	window_themes_mouseup,
	window_themes_resize,
	window_themes_mousedown,
	window_themes_dropdown,
	NULL,
	window_themes_update,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	window_themes_scrollgetsize,
	window_themes_scrollmousedown,
	NULL,
	window_themes_scrollmouseover,
	window_themes_textinput,
	NULL,
	NULL,
	window_themes_tooltip,
	NULL,
	NULL,
	window_themes_invalidate,
	window_themes_paint,
	window_themes_scrollpaint,
};

enum WINDOW_STAFF_LIST_WIDGET_IDX {
	WIDX_THEMES_BACKGROUND,
	WIDX_THEMES_TITLE,
	WIDX_THEMES_CLOSE,
	WIDX_THEMES_TAB_CONTENT_PANEL,
	WIDX_THEMES_SETTINGS_TAB,
	WIDX_THEMES_MAIN_UI_TAB,
	WIDX_THEMES_PARK_TAB,
	WIDX_THEMES_TOOLS_TAB,
	WIDX_THEMES_RIDE_PEEPS_TAB,
	WIDX_THEMES_EDITORS_TAB,
	WIDX_THEMES_MISC_TAB,
	WIDX_THEMES_PROMPTS_TAB,
	WIDX_THEMES_FEATURES_TAB,
	WIDX_THEMES_PRESETS,
	WIDX_THEMES_PRESETS_DROPDOWN,
	WIDX_THEMES_DUPLICATE_BUTTON,
	WIDX_THEMES_DELETE_BUTTON,
	WIDX_THEMES_RENAME_BUTTON,
	WIDX_THEMES_COLORBTN_MASK,
	WIDX_THEMES_LIST,
	WIDX_THEMES_RCT1_RIDE_LIGHTS,
	WIDX_THEMES_RCT1_PARK_LIGHTS,
	WIDX_THEMES_RCT1_SCENARIO_FONT
};

static rct_widget window_themes_widgets[] = {
	{ WWT_FRAME,			0,	0,		319,	0,		106,	0x0FFFFFFFF,					STR_NONE },					// panel / background
	{ WWT_CAPTION,			0,	1,		318,	1,		14,		5244,							STR_WINDOW_TITLE_TIP },		// title bar
	{ WWT_CLOSEBOX,			0,	307,	317,	2,		13,		STR_CLOSE_X,					STR_CLOSE_WINDOW_TIP },		// close button
	{ WWT_RESIZE,			1,	0,		319,	43,		106,	0x0FFFFFFFF,					STR_NONE },					// tab content panel
	{ WWT_TAB,				1,	3,		33,		17,		43,		0x02000144E,					5235 },						// settings tab
	{ WWT_TAB,				1,	34,		64,		17,		43,		0x02000144E,					5228 },						// main ui tab
	{ WWT_TAB,				1,	65,		95,		17,		43,		0x02000144E,					5229 },						// park tab
	{ WWT_TAB,				1,	96,		126,	17,		43,		0x02000144E,					5230 },						// tools tab
	{ WWT_TAB,				1,	127,	157,	17,		43,		0x02000144E,					5231 },						// rides and peeps tab
	{ WWT_TAB,				1,	158,	188,	17,		43,		0x02000144E,					5232 },						// editors tab
	{ WWT_TAB,				1,	189,	219,	17,		43,		0x02000144E,					5233 },						// misc tab
	{ WWT_TAB,				1,	220,	250,	17,		43,		0x02000144E,					5234 },						// prompts tab
	{ WWT_TAB,				1,	251,	281,	17,		43,		0x02000144E,					5281 },						// features tab
	{ WWT_DROPDOWN,			1,	125,	299,	60,		71,		STR_NONE,						STR_NONE },					// Preset colour schemes
	{ WWT_DROPDOWN_BUTTON,	1,	288,	298,	61,		70,		876,							STR_NONE },
	{ WWT_DROPDOWN_BUTTON,	1,	10,		100,	82,		93,		5239,							5257 },						// Duplicate button
	{ WWT_DROPDOWN_BUTTON,	1,	110,	200,	82,		93,		3349,							5258 },						// Delete button
	{ WWT_DROPDOWN_BUTTON,	1,	210,	300,	82,		93,		3348,							5259 },						// Rename button
	{ WWT_COLORBTN,			1,	0,		0,		0,		0,		STR_NONE,						STR_NONE },					// color button mask
	{ WWT_SCROLL,			1,	3,		316,	60,		103,	2,								STR_NONE },					// staff list
	{ WWT_CHECKBOX,			1,	10,		299,	54,		65,		5282,							STR_NONE },					// rct1 ride lights
	{ WWT_CHECKBOX,			1,	10,		299,	69,		80,		5283,							STR_NONE },					// rct1 park lights
	{ WWT_CHECKBOX,			1,	10,		299,	84,		95,		5284,							STR_NONE },					// rct1 scenario font
	{ WIDGETS_END },
};

static int window_themes_tab_animation_loops[] = {
	32,
	32,
	1,
	1,
	64,
	32,
	8,
	14,
	38
};
static int window_themes_tab_animation_divisor[] = {
	4,
	4,
	1,
	1,
	4,
	2,
	2,
	2,
	2
};
static int window_themes_tab_sprites[] = {
	5221,
	SPR_TAB_KIOSKS_AND_FACILITIES_0,
	5200,
	SPR_G2_TAB_LAND,
	SPR_TAB_RIDE_0,
	5205,
	5201,
	SPR_TAB_STAFF_OPTIONS_0,
	SPR_TAB_FINANCES_MARKETING_0
};

static rct_windowclass window_themes_tab_1_classes[] = {
	WC_TOP_TOOLBAR,
	WC_BOTTOM_TOOLBAR,
	WC_EDITOR_SCENARIO_BOTTOM_TOOLBAR,
	WC_EDITOR_TRACK_BOTTOM_TOOLBAR,
	WC_TITLE_MENU,
	WC_TITLE_EXIT,
	WC_TITLE_OPTIONS,
	WC_SCENARIO_SELECT
};

static rct_windowclass window_themes_tab_2_classes[] = {
	WC_PARK_INFORMATION,
	WC_FINANCES,
	WC_NEW_CAMPAIGN,
	WC_RESEARCH,
	WC_MAP,
	WC_VIEWPORT,
	WC_RECENT_NEWS
};

static rct_windowclass window_themes_tab_3_classes[] = {
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

static rct_windowclass window_themes_tab_4_classes[] = {
	WC_RIDE,
	WC_RIDE_LIST,
	WC_PEEP,
	WC_GUEST_LIST,
	WC_STAFF,
	WC_STAFF_LIST,
	WC_BANNER
};

static rct_windowclass window_themes_tab_5_classes[] = {
	WC_EDITOR_OBJECT_SELECTION,
	WC_EDITOR_INVENTION_LIST,
	WC_EDITOR_SCENARIO_OPTIONS,
	WC_EDTIOR_OBJECTIVE_OPTIONS,
	WC_MAPGEN,
	WC_MANAGE_TRACK_DESIGN,
	WC_INSTALL_TRACK
};

static rct_windowclass window_themes_tab_6_classes[] = {
	WC_CHEATS,
	WC_THEMES,
	WC_TITLE_EDITOR,
	WC_OPTIONS,
	WC_KEYBOARD_SHORTCUT_LIST,
	WC_CHANGE_KEYBOARD_SHORTCUT,
	WC_LOADSAVE
};

static rct_windowclass window_themes_tab_7_classes[] = {
	WC_SAVE_PROMPT,
	WC_DEMOLISH_RIDE_PROMPT,
	WC_FIRE_PROMPT,
	WC_TRACK_DELETE_PROMPT,
	WC_LOADSAVE_OVERWRITE_PROMPT
};

static uint8 _selected_tab = 0;
static sint16 _color_index_1 = -1;
static sint8 _color_index_2 = -1;
static const uint8 _row_height = 32;
static const uint8 _button_offset_x = 220;
static const uint8 _button_offset_y = 3;
static const uint8 _check_offset_y = 3 + 12 + 2;

void window_themes_init_vars()
{
	_selected_tab = WINDOW_THEMES_TAB_SETTINGS;
}

static theme_window_definition* get_colour_scheme_tab_definition()
{
	switch (_selected_tab) {
	case 1: return theme_window_definition_get_by_class(window_themes_tab_1_classes[_color_index_1]);
	case 2: return theme_window_definition_get_by_class(window_themes_tab_2_classes[_color_index_1]);
	case 3: return theme_window_definition_get_by_class(window_themes_tab_3_classes[_color_index_1]);
	case 4: return theme_window_definition_get_by_class(window_themes_tab_4_classes[_color_index_1]);
	case 5: return theme_window_definition_get_by_class(window_themes_tab_5_classes[_color_index_1]);
	case 6: return theme_window_definition_get_by_class(window_themes_tab_6_classes[_color_index_1]);
	case 7: return theme_window_definition_get_by_class(window_themes_tab_7_classes[_color_index_1]);
	}
	return NULL;
}
static theme_window_definition* get_colour_scheme_tab_definition_by_index(int index)
{
	switch (_selected_tab) {
	case 1: return theme_window_definition_get_by_class(window_themes_tab_1_classes[index]);
	case 2: return theme_window_definition_get_by_class(window_themes_tab_2_classes[index]);
	case 3: return theme_window_definition_get_by_class(window_themes_tab_3_classes[index]);
	case 4: return theme_window_definition_get_by_class(window_themes_tab_4_classes[index]);
	case 5: return theme_window_definition_get_by_class(window_themes_tab_5_classes[index]);
	case 6: return theme_window_definition_get_by_class(window_themes_tab_6_classes[index]);
	case 7: return theme_window_definition_get_by_class(window_themes_tab_7_classes[index]);
	}
	return NULL;
}
static theme_window* get_colour_scheme_tab()
{
	switch (_selected_tab) {
	case 1: return theme_window_get_by_class(window_themes_tab_1_classes[_color_index_1]);
	case 2: return theme_window_get_by_class(window_themes_tab_2_classes[_color_index_1]);
	case 3: return theme_window_get_by_class(window_themes_tab_3_classes[_color_index_1]);
	case 4: return theme_window_get_by_class(window_themes_tab_4_classes[_color_index_1]);
	case 5: return theme_window_get_by_class(window_themes_tab_5_classes[_color_index_1]);
	case 6: return theme_window_get_by_class(window_themes_tab_6_classes[_color_index_1]);
	case 7: return theme_window_get_by_class(window_themes_tab_7_classes[_color_index_1]);
	}
	return NULL;
}
static theme_window* get_colour_scheme_tab_by_index(int index)
{
	switch (_selected_tab) {
	case 1: return theme_window_get_by_class(window_themes_tab_1_classes[index]);
	case 2: return theme_window_get_by_class(window_themes_tab_2_classes[index]);
	case 3: return theme_window_get_by_class(window_themes_tab_3_classes[index]);
	case 4: return theme_window_get_by_class(window_themes_tab_4_classes[index]);
	case 5: return theme_window_get_by_class(window_themes_tab_5_classes[index]);
	case 6: return theme_window_get_by_class(window_themes_tab_6_classes[index]);
	case 7: return theme_window_get_by_class(window_themes_tab_7_classes[index]);
	}
	return NULL;
}

static int get_colour_scheme_tab_count()
{
	switch (_selected_tab) {
	case 1: return sizeof(window_themes_tab_1_classes);
	case 2: return sizeof(window_themes_tab_2_classes);
	case 3: return sizeof(window_themes_tab_3_classes);
	case 4: return sizeof(window_themes_tab_4_classes);
	case 5: return sizeof(window_themes_tab_5_classes);
	case 6: return sizeof(window_themes_tab_6_classes);
	case 7: return sizeof(window_themes_tab_7_classes);
	}
	return 0;
}

/*static int get_colour_scheme_index() {
	switch (_selected_tab) {
	case 1: return theme_window_get_index_by_class(window_themes_tab_1_classes[_color_index_1]);
	case 2: return theme_window_get_index_by_class(window_themes_tab_2_classes[_color_index_1]);
	case 3: return theme_window_get_index_by_class(window_themes_tab_3_classes[_color_index_1]);
	case 4: return theme_window_get_index_by_class(window_themes_tab_4_classes[_color_index_1]);
	case 5: return theme_window_get_index_by_class(window_themes_tab_5_classes[_color_index_1]);
	case 6: return theme_window_get_index_by_class(window_themes_tab_6_classes[_color_index_1]);
	case 7: return theme_window_get_index_by_class(window_themes_tab_7_classes[_color_index_1]);
	}
	return -1;
}*/

static void window_themes_draw_tab_images(rct_drawpixelinfo *dpi, rct_window *w)
{
	int sprite_idx;

	for (int i = 0; i < WINDOW_THEMES_TAB_COUNT; i++) {
		sprite_idx = window_themes_tab_sprites[i];
		if (_selected_tab == i)
			sprite_idx += w->frame_no / window_themes_tab_animation_divisor[_selected_tab];
		gfx_draw_sprite(dpi, sprite_idx, w->x + w->widgets[WIDX_THEMES_SETTINGS_TAB + i].left, w->y + w->widgets[WIDX_THEMES_SETTINGS_TAB + i].top, 0);
	}
}

void window_themes_open()
{
	rct_window* window;

	// Check if window is already open
	window = window_bring_to_front_by_class(WC_THEMES);
	if (window != NULL)
		return;

	window = window_create_auto_pos(320, 107, &window_themes_events, WC_THEMES, WF_10 | WF_RESIZABLE);
	window->widgets = window_themes_widgets;
	window->enabled_widgets =
		(1 << WIDX_THEMES_CLOSE) |
		(1 << WIDX_THEMES_SETTINGS_TAB) |
		(1 << WIDX_THEMES_MAIN_UI_TAB) |
		(1 << WIDX_THEMES_PARK_TAB) |
		(1 << WIDX_THEMES_TOOLS_TAB) |
		(1 << WIDX_THEMES_RIDE_PEEPS_TAB) |
		(1 << WIDX_THEMES_EDITORS_TAB) |
		(1 << WIDX_THEMES_MISC_TAB) |
		(1 << WIDX_THEMES_PROMPTS_TAB) |
		(1 << WIDX_THEMES_FEATURES_TAB) |
		(1 << WIDX_THEMES_COLORBTN_MASK) |
		(1 << WIDX_THEMES_PRESETS) |
		(1 << WIDX_THEMES_PRESETS_DROPDOWN) |
		(1 << WIDX_THEMES_DUPLICATE_BUTTON) |
		(1 << WIDX_THEMES_DELETE_BUTTON) |
		(1 << WIDX_THEMES_RENAME_BUTTON) |
		(1 << WIDX_THEMES_RCT1_RIDE_LIGHTS) |
		(1 << WIDX_THEMES_RCT1_PARK_LIGHTS) |
		(1 << WIDX_THEMES_RCT1_SCENARIO_FONT);

	window_themes_init_vars();

	window_init_scroll_widgets(window);
	window->list_information_type = 0;
	_color_index_1 = -1;
	_color_index_2 = -1;
	window->min_width = 320;
	window->min_height = 107;
	window->max_width = 320;
	window->max_height = 107;
}

static void window_themes_mouseup(rct_window *w, int widgetIndex)
{
	switch (widgetIndex) {
	case WIDX_THEMES_CLOSE:
		window_close(w);
		break;
	case WIDX_THEMES_DUPLICATE_BUTTON:
		window_text_input_open(w, widgetIndex, 5239, 5240, 1170, (uint32)&gConfigThemes.presets[gCurrentTheme].name, 64);
		break;
	case WIDX_THEMES_DELETE_BUTTON:
		if (gCurrentTheme >= 2) {
			theme_delete_preset(gCurrentTheme);
		}
		else {
			window_error_open(5241, STR_NONE);
		}
		break;
	case WIDX_THEMES_RENAME_BUTTON:
		if (gCurrentTheme >= 2) {
			window_text_input_open(w, widgetIndex, 3348, 5240, 1170, (uint32)&gConfigThemes.presets[gCurrentTheme].name, 64);
		}
		else {
			window_error_open(5241, STR_NONE);
		}
		break;
	}
}

static void window_themes_resize(rct_window *w)
{
	if (_selected_tab == WINDOW_THEMES_TAB_SETTINGS) {
		w->min_width = 320;
		w->min_height = 107;
		w->max_width = 320;
		w->max_height = 107;

		if (w->width < w->min_width) {
			w->width = w->min_width;
			gfx_invalidate_screen();
		}
		if (w->height < w->min_height) {
			w->height = w->min_height;
			gfx_invalidate_screen();
		}
		if (w->width > w->max_width) {
			w->width = w->max_width;
			gfx_invalidate_screen();
		}
		if (w->height > w->max_height) {
			w->height = w->max_height;
			gfx_invalidate_screen();
		}
	}
	else if (_selected_tab == WINDOW_THEMES_TAB_FEATURES) {
		w->min_width = 320;
		w->min_height = 107;
		w->max_width = 320;
		w->max_height = 107;

		if (w->width < w->min_width) {
			w->width = w->min_width;
			gfx_invalidate_screen();
		}
		if (w->height < w->min_height) {
			w->height = w->min_height;
			gfx_invalidate_screen();
		}
		if (w->width > w->max_width) {
			w->width = w->max_width;
			gfx_invalidate_screen();
		}
		if (w->height > w->max_height) {
			w->height = w->max_height;
			gfx_invalidate_screen();
		}
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

static void window_themes_mousedown(int widgetIndex, rct_window* w, rct_widget* widget)
{
	short newSelectedTab;
	int num_items, i;

	switch (widgetIndex) {
	case WIDX_THEMES_SETTINGS_TAB:
	case WIDX_THEMES_MAIN_UI_TAB:
	case WIDX_THEMES_PARK_TAB:
	case WIDX_THEMES_TOOLS_TAB:
	case WIDX_THEMES_RIDE_PEEPS_TAB:
	case WIDX_THEMES_EDITORS_TAB:
	case WIDX_THEMES_MISC_TAB:
	case WIDX_THEMES_PROMPTS_TAB:
	case WIDX_THEMES_FEATURES_TAB:
		newSelectedTab = widgetIndex - WIDX_THEMES_SETTINGS_TAB;
		if (_selected_tab == newSelectedTab)
			break;
		_selected_tab = (uint8)newSelectedTab;
		w->scrolls[0].v_top = 0;
		w->frame_no = 0;
		window_event_resize_call(w);
		window_invalidate(w);
		break;
	case WIDX_THEMES_PRESETS_DROPDOWN:
		num_items = gConfigThemes.num_presets;

		widget--;
		gDropdownItemsFormat[0] = 2777;
		gDropdownItemsArgs[0] = (uint32)&gConfigThemes.presets[1].name;
		gDropdownItemsFormat[1] = 2777;
		gDropdownItemsArgs[1] = (uint32)&gConfigThemes.presets[0].name;

		for (i = 2; i < num_items; i++) {
			gDropdownItemsFormat[i] = 2777;
			gDropdownItemsArgs[i] = (uint32)&gConfigThemes.presets[i].name;
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

		if (gCurrentTheme == 0 || gCurrentTheme == 1)
			gDropdownItemsChecked = 1 << (gCurrentTheme ^ 1);
		else
			gDropdownItemsChecked = 1 << (gCurrentTheme);
		break;
	case WIDX_THEMES_RCT1_RIDE_LIGHTS:
		if (gCurrentTheme >= 2) {
			theme_get_preset()->features.rct1_ride_lights ^= 1;
			themes_save_preset(gCurrentTheme);
			window_invalidate_all();
		}
		else {
			window_error_open(5241, STR_NONE);
		}
		break;
	case WIDX_THEMES_RCT1_PARK_LIGHTS:
		if (gCurrentTheme >= 2) {
			theme_get_preset()->features.rct1_park_lights ^= 1;
			themes_save_preset(gCurrentTheme);
			window_invalidate_all();
		}
		else {
			window_error_open(5241, STR_NONE);
		}
		break;
	case WIDX_THEMES_RCT1_SCENARIO_FONT:
		if (gCurrentTheme >= 2) {
			theme_get_preset()->features.rct1_scenario_font ^= 1;
			themes_save_preset(gCurrentTheme);
			window_invalidate_all();
		}
		else {
			window_error_open(5241, STR_NONE);
		}
		break;
	}
}

static void window_themes_dropdown(rct_window *w, int widgetIndex, int dropdownIndex)
{
	switch (widgetIndex) {
	case WIDX_THEMES_LIST:
		if (dropdownIndex != -1) {
			get_colour_scheme_tab()->colours[_color_index_2] = dropdownIndex | get_colour_scheme_tab()->colours[_color_index_2] & 0x80;
			window_invalidate_all();
			_color_index_1 = -1;
			_color_index_2 = -1;

			if (gCurrentTheme >= 2)
				themes_save_preset(gCurrentTheme);
		}
		break;
	case WIDX_THEMES_PRESETS_DROPDOWN:
		if (dropdownIndex != -1) {
			if (dropdownIndex == 0 || dropdownIndex == 1)
				dropdownIndex ^= 1;
			theme_change_preset(dropdownIndex);
		}
		config_save_default();
		break;
	}

}

void window_themes_update(rct_window *w)
{
	w->frame_no++;
	if (w->frame_no >= window_themes_tab_animation_loops[_selected_tab])
		w->frame_no = 0;

	widget_invalidate(w, WIDX_THEMES_SETTINGS_TAB + _selected_tab);

}

void window_themes_scrollgetsize(rct_window *w, int scrollIndex, int *width, int *height)
{
	if (_selected_tab == WINDOW_THEMES_TAB_SETTINGS || _selected_tab == WINDOW_THEMES_TAB_FEATURES)
		return;

	int scrollHeight = get_colour_scheme_tab_count() * _row_height;
	int i = scrollHeight - window_themes_widgets[WIDX_THEMES_LIST].bottom + window_themes_widgets[WIDX_THEMES_LIST].top + 21;
	if (i < 0)
		i = 0;
	if (i < w->scrolls[0].v_top) {
		w->scrolls[0].v_top = i;
		window_invalidate(w);
	}

	*width = 420;
	*height = scrollHeight;
}

void window_themes_scrollmousedown(rct_window *w, int scrollIndex, int x, int y)
{
	if (y / _row_height < get_colour_scheme_tab_count()) {
		int y2 = y % _row_height;
		_color_index_1 = y / _row_height;
		_color_index_2 = ((x - _button_offset_x) / 12);
		if (_color_index_2 < get_colour_scheme_tab_definition()->num_colours) {
			if (x >= _button_offset_x && x < _button_offset_x + 12 * 6 && y2 >= _button_offset_y && y2 < _button_offset_y + 11) {
				if (gCurrentTheme >= 2) {
					window_themes_widgets[WIDX_THEMES_COLORBTN_MASK].left = _button_offset_x + _color_index_2 * 12 + window_themes_widgets[WIDX_THEMES_LIST].left;
					window_themes_widgets[WIDX_THEMES_COLORBTN_MASK].top = _color_index_1 * _row_height + _button_offset_y - w->scrolls[0].v_top + window_themes_widgets[WIDX_THEMES_LIST].top;
					window_themes_widgets[WIDX_THEMES_COLORBTN_MASK].right = window_themes_widgets[WIDX_THEMES_COLORBTN_MASK].left + 12;
					window_themes_widgets[WIDX_THEMES_COLORBTN_MASK].bottom = window_themes_widgets[WIDX_THEMES_COLORBTN_MASK].top + 12;
					window_dropdown_show_colour(w, &(window_themes_widgets[WIDX_THEMES_COLORBTN_MASK]), w->colours[1], get_colour_scheme_tab()->colours[_color_index_2]);
					widget_invalidate(w, WIDX_THEMES_LIST);
				}
				else {
					window_error_open(5241, 5256);
				}
			}
			else if (x >= _button_offset_x && x < _button_offset_x + 12 * 6 - 1 && y2 >= _check_offset_y && y2 < _check_offset_y + 11) {
				if (gCurrentTheme >= 2) {
					if (get_colour_scheme_tab()->colours[_color_index_2] & 0x80) {
						get_colour_scheme_tab()->colours[_color_index_2] &= 0x7F;
					}
					else {
						get_colour_scheme_tab()->colours[_color_index_2] |= 0x80;
					}
						themes_save_preset(gCurrentTheme);
					window_invalidate_all();
				}
				else {
					window_error_open(5241, 5256);
				}
			}
		}
	}
}

void window_themes_scrollmouseover(rct_window *w, int scrollIndex, int x, int y)
{
	//if (_selected_tab == WINDOW_THEMES_TAB_SETTINGS)
	//	return;
}

static void window_themes_textinput(rct_window *w, int widgetIndex, char *text)
{
	if (text == NULL || text[0] == 0)
		return;

	switch (widgetIndex) {
	case WIDX_THEMES_DUPLICATE_BUTTON:
	case WIDX_THEMES_RENAME_BUTTON:
		if (filename_valid_characters(text)) {
			bool nameTaken = false;
			for (int i = 0; i < gConfigThemes.num_presets; i++) {
				if (strcmp(gConfigThemes.presets[i].name, text) == 0) {
					window_error_open(5242, STR_NONE);
					nameTaken = true;
					break;
				}
			}
			if (!nameTaken) {
				if (widgetIndex == WIDX_THEMES_DUPLICATE_BUTTON) {
					theme_create_preset(gCurrentTheme, text);
				}
				else {
					theme_rename_preset(gCurrentTheme, text);
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

void window_themes_tooltip(rct_window* w, int widgetIndex, rct_string_id *stringId)
{
	RCT2_GLOBAL(0x013CE952, uint16) = STR_LIST;
}

void window_themes_invalidate(rct_window *w)
{
	colour_scheme_update(w);

	int pressed_widgets = w->pressed_widgets & 0xFFFFE00F;
	uint8 widgetIndex = _selected_tab + 4;

	w->pressed_widgets = pressed_widgets | (1 << widgetIndex);

	if (window_find_by_class(WC_DROPDOWN) == NULL) {
		_color_index_1 = -1;
		_color_index_2 = -1;
	}

	window_themes_widgets[WIDX_THEMES_BACKGROUND].right = w->width - 1;
	window_themes_widgets[WIDX_THEMES_BACKGROUND].bottom = w->height - 1;
	window_themes_widgets[WIDX_THEMES_TAB_CONTENT_PANEL].right = w->width - 1;
	window_themes_widgets[WIDX_THEMES_TAB_CONTENT_PANEL].bottom = w->height - 1;
	window_themes_widgets[WIDX_THEMES_TITLE].right = w->width - 2;
	window_themes_widgets[WIDX_THEMES_CLOSE].left = w->width - 2 - 0x0B;
	window_themes_widgets[WIDX_THEMES_CLOSE].right = w->width - 2 - 0x0B + 0x0A;
	window_themes_widgets[WIDX_THEMES_LIST].right = w->width - 4;
	window_themes_widgets[WIDX_THEMES_LIST].bottom = w->height - 0x0F;

	
	window_themes_widgets[WIDX_THEMES_LIST].type = WWT_EMPTY;
	window_themes_widgets[WIDX_THEMES_RCT1_RIDE_LIGHTS].type = WWT_EMPTY;
	window_themes_widgets[WIDX_THEMES_RCT1_PARK_LIGHTS].type = WWT_EMPTY;
	window_themes_widgets[WIDX_THEMES_RCT1_SCENARIO_FONT].type = WWT_EMPTY;
	window_themes_widgets[WIDX_THEMES_DUPLICATE_BUTTON].type = WWT_EMPTY;
	window_themes_widgets[WIDX_THEMES_DELETE_BUTTON].type = WWT_EMPTY;
	window_themes_widgets[WIDX_THEMES_RENAME_BUTTON].type = WWT_EMPTY;
	window_themes_widgets[WIDX_THEMES_PRESETS].type = WWT_EMPTY;
	window_themes_widgets[WIDX_THEMES_PRESETS_DROPDOWN].type = WWT_EMPTY;

	if (_selected_tab == WINDOW_THEMES_TAB_SETTINGS) {
		window_themes_widgets[WIDX_THEMES_LIST].type = WWT_EMPTY;
		window_themes_widgets[WIDX_THEMES_RCT1_RIDE_LIGHTS].type = WWT_EMPTY;
		window_themes_widgets[WIDX_THEMES_RCT1_PARK_LIGHTS].type = WWT_EMPTY;
		window_themes_widgets[WIDX_THEMES_RCT1_SCENARIO_FONT].type = WWT_EMPTY;
		window_themes_widgets[WIDX_THEMES_DUPLICATE_BUTTON].type = WWT_DROPDOWN_BUTTON;
		window_themes_widgets[WIDX_THEMES_DELETE_BUTTON].type = WWT_DROPDOWN_BUTTON;
		window_themes_widgets[WIDX_THEMES_RENAME_BUTTON].type = WWT_DROPDOWN_BUTTON;
		window_themes_widgets[WIDX_THEMES_PRESETS].type = WWT_DROPDOWN;
		window_themes_widgets[WIDX_THEMES_PRESETS_DROPDOWN].type = WWT_DROPDOWN_BUTTON;
	}
	else if (_selected_tab == WINDOW_THEMES_TAB_FEATURES) {
		window_themes_widgets[WIDX_THEMES_LIST].type = WWT_EMPTY;
		window_themes_widgets[WIDX_THEMES_RCT1_RIDE_LIGHTS].type = WWT_CHECKBOX;
		window_themes_widgets[WIDX_THEMES_RCT1_PARK_LIGHTS].type = WWT_CHECKBOX;
		window_themes_widgets[WIDX_THEMES_RCT1_SCENARIO_FONT].type = WWT_CHECKBOX;
		window_themes_widgets[WIDX_THEMES_DUPLICATE_BUTTON].type = WWT_EMPTY;
		window_themes_widgets[WIDX_THEMES_DELETE_BUTTON].type = WWT_EMPTY;
		window_themes_widgets[WIDX_THEMES_RENAME_BUTTON].type = WWT_EMPTY;
		window_themes_widgets[WIDX_THEMES_PRESETS].type = WWT_EMPTY;
		window_themes_widgets[WIDX_THEMES_PRESETS_DROPDOWN].type = WWT_EMPTY;

		widget_set_checkbox_value(w, WIDX_THEMES_RCT1_RIDE_LIGHTS, theme_get_preset()->features.rct1_ride_lights);
		widget_set_checkbox_value(w, WIDX_THEMES_RCT1_PARK_LIGHTS, theme_get_preset()->features.rct1_park_lights);
		widget_set_checkbox_value(w, WIDX_THEMES_RCT1_SCENARIO_FONT, theme_get_preset()->features.rct1_scenario_font);
	}
	else {
		window_themes_widgets[WIDX_THEMES_LIST].type = WWT_SCROLL;
		window_themes_widgets[WIDX_THEMES_RCT1_RIDE_LIGHTS].type = WWT_EMPTY;
		window_themes_widgets[WIDX_THEMES_RCT1_PARK_LIGHTS].type = WWT_EMPTY;
		window_themes_widgets[WIDX_THEMES_RCT1_SCENARIO_FONT].type = WWT_EMPTY;
		window_themes_widgets[WIDX_THEMES_DUPLICATE_BUTTON].type = WWT_EMPTY;
		window_themes_widgets[WIDX_THEMES_DELETE_BUTTON].type = WWT_EMPTY;
		window_themes_widgets[WIDX_THEMES_RENAME_BUTTON].type = WWT_EMPTY;
		window_themes_widgets[WIDX_THEMES_PRESETS].type = WWT_EMPTY;
		window_themes_widgets[WIDX_THEMES_PRESETS_DROPDOWN].type = WWT_EMPTY;
	}
}

void window_themes_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
	// Widgets
	window_draw_widgets(w, dpi);
	window_themes_draw_tab_images(dpi, w);

	if (_selected_tab == WINDOW_THEMES_TAB_SETTINGS) {
		RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS + 0, uint32) = (uint32)&gConfigThemes.presets[gCurrentTheme].name;
		gfx_draw_string_left(dpi, 5238, NULL, w->colours[1], w->x + 10, w->y + window_themes_widgets[WIDX_THEMES_PRESETS].top + 1);
		gfx_draw_string_left_clipped(
			dpi,
			1170,
			(void*)RCT2_ADDRESS_COMMON_FORMAT_ARGS,
			w->colours[1],
			w->x + window_themes_widgets[WIDX_THEMES_PRESETS].left + 1,
			w->y + window_themes_widgets[WIDX_THEMES_PRESETS].top,
			w->x + window_themes_widgets[WIDX_THEMES_PRESETS_DROPDOWN].left - window_themes_widgets[WIDX_THEMES_PRESETS].left - 4
			);
	}
	else if (_selected_tab == WINDOW_THEMES_TAB_FEATURES) {

	}
	else {
		gfx_draw_string_left(dpi, 5236, w, w->colours[1], w->x + 6, 58 - 12 + w->y + 1);
		gfx_draw_string_left(dpi, 5237, w, w->colours[1], w->x + 220, 58 - 12 + w->y + 1);
	}
}

/**
*
*  rct2: 0x006BD785
*/
void window_themes_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, int scrollIndex)
{
	int y;

	if (_selected_tab == WINDOW_THEMES_TAB_SETTINGS || _selected_tab == WINDOW_THEMES_TAB_FEATURES)
		return;

	if ((w->colours[1] & 0x80) == 0)
		//gfx_fill_rect(dpi, dpi->x, dpi->y, dpi->x + dpi->width - 1, dpi->y + dpi->height - 1, ((char*)0x0141FC48)[w->colours[1] * 8]);
		gfx_clear(dpi, ((char*)0x0141FC48)[w->colours[1] * 8] * 0x1010101);
	y = 0;
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
					gfx_fill_rect(dpi, 0, y + _row_height - 2, window_themes_widgets[WIDX_THEMES_LIST].right, y + _row_height - 2, colour + 1);
					gfx_fill_rect(dpi, 0, y + _row_height - 1, window_themes_widgets[WIDX_THEMES_LIST].right, y + _row_height - 1, colour + 2);
				}
				else {
					colour = RCT2_ADDRESS(0x0141FC47, uint8)[w->colours[1] * 8];
					gfx_fill_rect(dpi, 0, y + _row_height - 2, window_themes_widgets[WIDX_THEMES_LIST].right, y + _row_height - 2, colour);
					colour = RCT2_ADDRESS(0x0141FC4B, uint8)[w->colours[1] * 8];
					gfx_fill_rect(dpi, 0, y + _row_height - 1, window_themes_widgets[WIDX_THEMES_LIST].right, y + _row_height - 1, colour);
				}
			}

			for (int j = 0; j < get_colour_scheme_tab_definition_by_index(i)->num_colours; j++) {

				gfx_draw_string_left(dpi, get_colour_scheme_tab_definition_by_index(i)->name, NULL, w->colours[1], 2, y + 4);

				uint32 image = ((get_colour_scheme_tab_by_index(i)->colours[j] & 0x7F) << 19) + 0x600013C3;
				if (i == _color_index_1 && j == _color_index_2) {
					image = ((get_colour_scheme_tab_by_index(i)->colours[j] & 0x7F) << 19) + 0x600013C4;
				}
				gfx_draw_sprite(dpi, image, _button_offset_x + 12 * j, y + _button_offset_y, 0);

				gfx_fill_rect_inset(dpi, _button_offset_x + 12 * j, y + _check_offset_y, _button_offset_x + 12 * j + 9, y + _check_offset_y + 10, w->colours[1], 0xE0);
				if (get_colour_scheme_tab_by_index(i)->colours[j] & 0x80) {
					RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_FONT_SPRITE_BASE, sint16) = -1;
					gfx_draw_string(dpi, (char*)CheckBoxMarkString, w->colours[1] & 0x7F, _button_offset_x + 12 * j, y + _check_offset_y);
				}

			}
		}

		y += _row_height;
	}
}
