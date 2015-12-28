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

#include "../addresses.h"
#include "../config.h"
#include "../audio/audio.h"
#include "../localisation/date.h"
#include "../localisation/localisation.h"
#include "../scenario.h"
#include "../sprites.h"
#include "../interface/widget.h"
#include "../interface/window.h"
#include "../interface/themes.h"
#include "../util/util.h"

enum {
	WIDX_BACKGROUND,
	WIDX_TITLEBAR,
	WIDX_CLOSE,
	WIDX_TABCONTENT,
	WIDX_TAB1,
	WIDX_TAB2,
	WIDX_TAB3,
	WIDX_TAB4,
	WIDX_TAB5,
	WIDX_TAB6,
	WIDX_TAB7,
	WIDX_TAB8,
	WIDX_SCENARIOLIST
};

static rct_widget window_scenarioselect_widgets[] = {
	{ WWT_FRAME,	0,	0,		733,	0,		333,	-1,						STR_NONE },				// panel / background
	{ WWT_CAPTION,	0,	1,		732,	1,		14,		STR_SELECT_SCENARIO,	STR_WINDOW_TITLE_TIP },	// title bar
	{ WWT_CLOSEBOX,	0,	721,	731,	2,		13,		824,					STR_CLOSE_WINDOW_TIP },	// close x button
	{ WWT_IMGBTN,	1,	0,		733,	50,		333,	-1,						STR_NONE },				// tab content panel
	{ WWT_TAB,		1,	3,		93,		17,		50,		0x200015BC,				STR_NONE },				// tab 1
	{ WWT_TAB,		1,	94,		184,	17,		50,		0x200015BC,				STR_NONE },				// tab 2
	{ WWT_TAB,		1,	185,	275,	17,		50,		0x200015BC,				STR_NONE },				// tab 3
	{ WWT_TAB,		1,	276,	366,	17,		50,		0x200015BC,				STR_NONE },				// tab 4
	{ WWT_TAB,		1,	367,	457,	17,		50,		0x200015BC,				STR_NONE },				// tab 5
	{ WWT_TAB,		1,	458,	593,	17,		50,		0x200015BC,				STR_NONE },				// tab 6
	{ WWT_TAB,		1,	594,	684,	17,		50,		0x200015BC,				STR_NONE },				// tab 7
	{ WWT_TAB,		1,	685,	775,	17,		50,		0x200015BC,				STR_NONE },				// tab 8
	{ WWT_SCROLL,	1,	3,		555,	54,		329,	2,						STR_NONE },				// level list
	{ WIDGETS_END },
};

static void window_scenarioselect_init_tabs();

static void window_scenarioselect_mouseup(rct_window *w, int widgetIndex);
static void window_scenarioselect_mousedown(int widgetIndex, rct_window*w, rct_widget* widget);
static void window_scenarioselect_scrollgetsize(rct_window *w, int scrollIndex, int *width, int *height);
static void window_scenarioselect_scrollmousedown(rct_window *w, int scrollIndex, int x, int y);
static void window_scenarioselect_scrollmouseover(rct_window *w, int scrollIndex, int x, int y);
static void window_scenarioselect_invalidate(rct_window *w);
static void window_scenarioselect_paint(rct_window *w, rct_drawpixelinfo *dpi);
static void window_scenarioselect_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, int scrollIndex);

static rct_window_event_list window_scenarioselect_events = {
	NULL,
	window_scenarioselect_mouseup,
	NULL,
	window_scenarioselect_mousedown,
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
	window_scenarioselect_scrollgetsize,
	window_scenarioselect_scrollmousedown,
	NULL,
	window_scenarioselect_scrollmouseover,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	window_scenarioselect_invalidate,
	window_scenarioselect_paint,
	window_scenarioselect_scrollpaint
};

/**
 *
 *  rct2: 0x006781B5
 */
void window_scenarioselect_open()
{
	rct_window* window;
	int window_width;

	if (window_bring_to_front_by_class(WC_SCENARIO_SELECT) != NULL)
		return;

	// Load scenario list
	scenario_load_list();

	// Shrink the window if we're showing scenarios by difficulty level.
	if (gConfigGeneral.scenario_select_mode == 2)
	{
		window_width = 610;
		window_scenarioselect_widgets[WIDX_BACKGROUND].right = 609;
		window_scenarioselect_widgets[WIDX_TITLEBAR].right = 608;
		window_scenarioselect_widgets[WIDX_CLOSE].left  = 597;
		window_scenarioselect_widgets[WIDX_CLOSE].right = 607;
		window_scenarioselect_widgets[WIDX_TABCONTENT].right = 609;
		window_scenarioselect_widgets[WIDX_SCENARIOLIST].right = 433;
	}
	else
		window_width = 733;

	window = window_create_centred(
		window_width,
		334,
		&window_scenarioselect_events,
		WC_SCENARIO_SELECT,
		WF_10
	);
	window->widgets = window_scenarioselect_widgets;

	window->enabled_widgets = (1 << WIDX_CLOSE) | (1 << WIDX_TAB1) | (1 << WIDX_TAB2)
							| (1 << WIDX_TAB3) | (1 << WIDX_TAB4) | (1 << WIDX_TAB5)
							| (1 << WIDX_TAB6) | (1 << WIDX_TAB7) | (1 << WIDX_TAB8);

	window_init_scroll_widgets(window);
	window->viewport_focus_coordinates.var_480 = -1;
	window->highlighted_item = 0;

	window_scenarioselect_init_tabs();

	window->selected_tab = 0;
}

/**
 *
 *  rct2: 0x00677C8A
 */
static void window_scenarioselect_init_tabs()
{
	int show_pages = 0;
	for (int i = 0; i < gScenarioListCount; i++) {
		rct_scenario_basic* scenario = &gScenarioList[i];
		if (scenario->flags & SCENARIO_FLAGS_VISIBLE)
		{
			if (gConfigGeneral.scenario_select_mode == 1)
				show_pages |= 1 << scenario->source_game;
			else
				show_pages |= 1 << scenario->category;
		}
	}

	int x = 3;
	for (int i = 0; i < 8; i++) {
		rct_widget* widget = &window_scenarioselect_widgets[i + 4];
		if (!(show_pages & (1 << i))) {
			widget->type = WWT_EMPTY;
			continue;
		}

		widget->type = WWT_TAB;
		widget->left = x;
		widget->right = x + 90;
		x += 91;
	}
}

static void window_scenarioselect_mouseup(rct_window *w, int widgetIndex)
{
	if (widgetIndex == WIDX_CLOSE)
		window_close(w);
}

static void window_scenarioselect_mousedown(int widgetIndex, rct_window*w, rct_widget* widget)
{
	if (widgetIndex >= WIDX_TAB1 && widgetIndex <= WIDX_TAB8) {
		w->selected_tab = widgetIndex - 4;
		w->highlighted_item = 0;
		window_invalidate(w);
		window_event_resize_call(w);
		window_event_invalidate_call(w);
		window_init_scroll_widgets(w);
		window_invalidate(w);
	}
}

static void window_scenarioselect_scrollgetsize(rct_window *w, int scrollIndex, int *width, int *height)
{
	*height = 0;
	for (int i = 0; i < gScenarioListCount; i++) {
		rct_scenario_basic *scenario = &gScenarioList[i];

		if ((gConfigGeneral.scenario_select_mode == 1 && scenario->source_game != w->selected_tab) ||
			(gConfigGeneral.scenario_select_mode == 2 && scenario->category != w->selected_tab))
			continue;

		if (scenario->flags & SCENARIO_FLAGS_VISIBLE)
			*height += 24;
	}
}

/**
 *
 *  rct2: 0x6780FE
 */
static void window_scenarioselect_scrollmousedown(rct_window *w, int scrollIndex, int x, int y)
{
	int num_unlocks = 5;
	for (int i = 0; i < gScenarioListCount; i++) {
		rct_scenario_basic *scenario = &gScenarioList[i];

		if ((gConfigGeneral.scenario_select_mode == 1 && scenario->source_game != w->selected_tab) ||
			(gConfigGeneral.scenario_select_mode == 2 && scenario->category != w->selected_tab))
			continue;

		if (!(scenario->flags & SCENARIO_FLAGS_VISIBLE))
			continue;

		if (gConfigGeneral.scenario_unlocking_enabled) {
			if (num_unlocks <= 0)
				break;

			bool is_completed = scenario->flags & SCENARIO_FLAGS_COMPLETED;
			if (is_completed) {
				num_unlocks++;
			} else {
				num_unlocks--;
			}
		}

		y -= 24;
		if (y >= 0)
			continue;

		audio_play_sound_panned(SOUND_CLICK_1, w->width / 2 + w->x, 0, 0, 0);
		scenario_load_and_play(scenario);
		break;
	}
}

/**
 *
 *  rct2: 0x678162
 */
static void window_scenarioselect_scrollmouseover(rct_window *w, int scrollIndex, int x, int y)
{
	rct_scenario_basic *selected = NULL;
	int num_unlocks = 5;
	for (int i = 0; i < gScenarioListCount; i++) {
		rct_scenario_basic *scenario = &gScenarioList[i];
		if ((gConfigGeneral.scenario_select_mode == 1 && scenario->source_game != w->selected_tab) ||
			(gConfigGeneral.scenario_select_mode == 2 && scenario->category != w->selected_tab))
			continue;

		if (!(scenario->flags & SCENARIO_FLAGS_VISIBLE))
			continue;

		if (gConfigGeneral.scenario_unlocking_enabled) {
			if (num_unlocks <= 0)
				break;

			bool is_completed = scenario->flags & SCENARIO_FLAGS_COMPLETED;
			num_unlocks += is_completed ? 1 : -1;
		}

		y -= 24;
		if (y >= 0)
			continue;

		selected = scenario;
		break;
	}
	if (w->highlighted_item != (uint32)selected) {
		w->highlighted_item = (uint32)selected;
		window_invalidate(w);
	}
}

static void window_scenarioselect_invalidate(rct_window *w)
{
	colour_scheme_update(w);

	w->pressed_widgets &= ~( (1 << WIDX_CLOSE) | (1 << WIDX_TAB1) | (1 << WIDX_TAB2)
						   | (1 << WIDX_TAB3) | (1 << WIDX_TAB4) | (1 << WIDX_TAB5)
						   | (1 << WIDX_TAB6) | (1 << WIDX_TAB7) | (1 << WIDX_TAB8) );

	w->pressed_widgets |= 1LL << (w->selected_tab + 4);
}

static void window_scenarioselect_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
	int i, x, y, format;
	rct_widget *widget;
	rct_scenario_basic *scenario;

	window_draw_widgets(w, dpi);

	format = (theme_get_preset()->features.rct1_scenario_font) ? 5138 : 1193;

	// Text for each tab
	for (i = 0; i < 8; i++) {
		widget = &window_scenarioselect_widgets[WIDX_TAB1 + i];
		if (widget->type == WWT_EMPTY)
			continue;

		x = (widget->left + widget->right) / 2 + w->x;
		y = (widget->top + widget->bottom) / 2 + w->y - 3;

		if (gConfigGeneral.scenario_select_mode == 1) {
			RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS + 0, short) = STR_SCENARIO_CATEGORY_RCT1_CLASSIC + i;
		} else { // old-style
			RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS + 0, short) = STR_BEGINNER_PARKS + i;
		}
		gfx_draw_string_centred_wrapped(dpi, (void*)RCT2_ADDRESS_COMMON_FORMAT_ARGS, x, y, 87, format, 10);
	}

	// Return if no scenario highlighted
	scenario = (rct_scenario_basic*)w->highlighted_item;
	if (scenario == NULL)
		return;

	// Scenario name
	x = w->x + window_scenarioselect_widgets[WIDX_SCENARIOLIST].right + 4;
	y = w->y + window_scenarioselect_widgets[WIDX_TABCONTENT].top + 5;
	safe_strncpy((char*)0x009BC677, scenario->name, 64);
	RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS + 0, short) = 3165; // empty string
	gfx_draw_string_centred_clipped(dpi, 1193, (void*)RCT2_ADDRESS_COMMON_FORMAT_ARGS, 0, x + 85, y, 170);
	y += 15;

	// Scenario details
	safe_strncpy((char*)0x009BC677, scenario->details, 256);
	RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS + 0, short) = 3165; // empty string
	y += gfx_draw_string_left_wrapped(dpi, (void*)RCT2_ADDRESS_COMMON_FORMAT_ARGS, x, y, 170, 1191, 0) + 5;

	// Scenario objective
	RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS + 0, short) = scenario->objective_type + STR_OBJECTIVE_NONE;
	RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS + 2, short) = scenario->objective_arg_3;
	RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS + 4, short) = date_get_total_months(MONTH_OCTOBER, scenario->objective_arg_1);
	RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS + 6, int) = scenario->objective_arg_2;
	y += gfx_draw_string_left_wrapped(dpi, (void*)RCT2_ADDRESS_COMMON_FORMAT_ARGS, x, y, 170, STR_OBJECTIVE, 0) + 5;

	// Scenario score
	if (scenario->flags & SCENARIO_FLAGS_COMPLETED) {
		safe_strncpy((char*)0x009BC677, scenario->completed_by, 64);
		RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS + 0, short) = 3165; // empty string
		RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS + 2, int) = scenario->company_value;
		y += gfx_draw_string_left_wrapped(dpi, (void*)RCT2_ADDRESS_COMMON_FORMAT_ARGS, x, y, 170, STR_COMPLETED_BY_WITH_COMPANY_VALUE, 0);
	}
}

static void window_scenarioselect_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, int scrollIndex)
{
	int colour = ColourMapA[w->colours[1]].mid_light;
	colour = (colour << 24) | (colour << 16) | (colour << 8) | colour;
	gfx_clear(dpi, colour);

	int highlighted_format = (theme_get_preset()->features.rct1_scenario_font) ? 5139 : 1193;
	int unhighlighted_format = (theme_get_preset()->features.rct1_scenario_font) ? 5139 : 1191;
	int disabled_format = 5619;

	bool wide = gConfigGeneral.scenario_select_mode == 1;

	int y = 0;
	int num_unlocks = 5;
	for (int i = 0; i < gScenarioListCount; i++) {
		rct_scenario_basic *scenario = &gScenarioList[i];

		if ((gConfigGeneral.scenario_select_mode == 1 && scenario->source_game != w->selected_tab) ||
			(gConfigGeneral.scenario_select_mode == 2 && scenario->category != w->selected_tab))
			continue;

		if (!(scenario->flags & SCENARIO_FLAGS_VISIBLE))
			continue;

		if (y > dpi->y + dpi->height)
			continue;

		// Draw hover highlight
		bool is_highlighted = w->highlighted_item == (int)scenario;
		if (is_highlighted)
			gfx_fill_rect(dpi, 0, y, w->width, y + 23, 0x02000031);

		bool is_completed = scenario->flags & SCENARIO_FLAGS_COMPLETED;
		bool is_disabled = false;
		if (gConfigGeneral.scenario_unlocking_enabled) {
			if (num_unlocks <= 0)
				is_disabled = true;

			num_unlocks += is_completed ? 1 : -1;
		}

		int format = is_disabled ? 5619 : (is_highlighted ? highlighted_format : unhighlighted_format);

		// Draw scenario name
		safe_strncpy((char*)0x009BC677, scenario->name, 64);
		RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS, short) = 3165;
		gfx_draw_string_centred(dpi, format, wide ? 270 : 210, y + 1, 0, (void*)RCT2_ADDRESS_COMMON_FORMAT_ARGS);

		// Check if scenario is completed
		if (is_completed) {
			// Draw completion tick
			gfx_draw_sprite(dpi, 0x5A9F, wide ? 500 : 395, y + 1, 0);

			// Draw completion score
			safe_strncpy((char*)0x009BC677, scenario->completed_by, 64);
			RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS, short) = 2793;
			RCT2_GLOBAL(0x013CE954, short) = 3165;
			gfx_draw_string_centred(dpi, format, wide ? 270 : 210, y + 11, 0, (void*)RCT2_ADDRESS_COMMON_FORMAT_ARGS);
		}

		y += 24;
	}
}
