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

#define INITIAL_NUM_UNLOCKED_SCENARIOS 5

enum {
	LIST_ITEM_TYPE_HEADING,
	LIST_ITEM_TYPE_SCENARIO,
	LIST_ITEM_TYPE_END,
};

typedef struct {
	uint8 type;
	union {
		struct {
			rct_string_id string_id;
		} heading;
		struct {
			scenario_index_entry *scenario;
			bool is_locked;
		} scenario;
	};
} sc_list_item;

static sc_list_item *_listItems = NULL;

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

static void window_scenarioselect_init_tabs(rct_window *w);

static void window_scenarioselect_close(rct_window *w);
static void window_scenarioselect_mouseup(rct_window *w, int widgetIndex);
static void window_scenarioselect_mousedown(int widgetIndex, rct_window*w, rct_widget* widget);
static void window_scenarioselect_scrollgetsize(rct_window *w, int scrollIndex, int *width, int *height);
static void window_scenarioselect_scrollmousedown(rct_window *w, int scrollIndex, int x, int y);
static void window_scenarioselect_scrollmouseover(rct_window *w, int scrollIndex, int x, int y);
static void window_scenarioselect_invalidate(rct_window *w);
static void window_scenarioselect_paint(rct_window *w, rct_drawpixelinfo *dpi);
static void window_scenarioselect_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, int scrollIndex);

static rct_window_event_list window_scenarioselect_events = {
	window_scenarioselect_close,
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

static void draw_category_heading(rct_window *w, rct_drawpixelinfo *dpi, int left, int right, int y, rct_string_id stringId);
static void initialise_list_items(rct_window *w);
static bool is_scenario_visible(rct_window *w, scenario_index_entry *scenario);
static bool is_locking_enabled(rct_window *w);

/**
 *
 *  rct2: 0x006781B5
 */
void window_scenarioselect_open()
{
	rct_window* window;
	int windowWidth;
	int windowHeight = 334;

	if (window_bring_to_front_by_class(WC_SCENARIO_SELECT) != NULL)
		return;

	// Load scenario list
	scenario_load_list();

	// Shrink the window if we're showing scenarios by difficulty level.
	if (gConfigGeneral.scenario_select_mode == SCENARIO_SELECT_MODE_DIFFICULTY) {
		windowWidth = 610;
	} else {
		windowWidth = 733;
	}

	window = window_create_centred(
		windowWidth,
		windowHeight,
		&window_scenarioselect_events,
		WC_SCENARIO_SELECT,
		WF_10
	);
	window->widgets = window_scenarioselect_widgets;
	window->enabled_widgets = (1 << WIDX_CLOSE) | (1 << WIDX_TAB1) | (1 << WIDX_TAB2)
							| (1 << WIDX_TAB3) | (1 << WIDX_TAB4) | (1 << WIDX_TAB5)
							| (1 << WIDX_TAB6) | (1 << WIDX_TAB7) | (1 << WIDX_TAB8);

	window_scenarioselect_init_tabs(window);
	initialise_list_items(window);

	window_init_scroll_widgets(window);
	window->viewport_focus_coordinates.var_480 = -1;
	window->highlighted_scenario = NULL;
}

/**
 *
 *  rct2: 0x00677C8A
 */
static void window_scenarioselect_init_tabs(rct_window *w)
{
	int showPages = 0;
	for (int i = 0; i < gScenarioListCount; i++) {
		scenario_index_entry *scenario = &gScenarioList[i];
		if (gConfigGeneral.scenario_select_mode == SCENARIO_SELECT_MODE_ORIGIN) {
			showPages |= 1 << scenario->source_game;
		} else {
			int category = scenario->category;
			if (category > SCENARIO_CATEGORY_OTHER) {
				category = SCENARIO_CATEGORY_OTHER;
			}
			showPages |= 1 << category;
		}
		}

	int firstPage = bitscanforward(showPages);
	if (firstPage != -1) {
		w->selected_tab = firstPage;
	}

	int x = 3;
	for (int i = 0; i < 8; i++) {
		rct_widget* widget = &w->widgets[i + 4];
		if (!(showPages & (1 << i))) {
			widget->type = WWT_EMPTY;
			continue;
		}

		widget->type = WWT_TAB;
		widget->left = x;
		widget->right = x + 90;
		x += 91;
	}
}

static void window_scenarioselect_close(rct_window *w)
{
	SafeFree(_listItems);
}

static void window_scenarioselect_mouseup(rct_window *w, int widgetIndex)
{
	if (widgetIndex == WIDX_CLOSE) {
		window_close(w);
	}
}

static void window_scenarioselect_mousedown(int widgetIndex, rct_window*w, rct_widget* widget)
{
	if (widgetIndex >= WIDX_TAB1 && widgetIndex <= WIDX_TAB8) {
		w->selected_tab = widgetIndex - 4;
		w->highlighted_scenario = NULL;
		initialise_list_items(w);
		window_invalidate(w);
		window_event_resize_call(w);
		window_event_invalidate_call(w);
		window_init_scroll_widgets(w);
		window_invalidate(w);
	}
}

static void window_scenarioselect_scrollgetsize(rct_window *w, int scrollIndex, int *width, int *height)
{
	int y = 0;
	for (sc_list_item *listItem = _listItems; listItem->type != LIST_ITEM_TYPE_END; listItem++) {
		switch (listItem->type) {
		case LIST_ITEM_TYPE_HEADING:
			y += 18;
			break;
		case LIST_ITEM_TYPE_SCENARIO:
			y += 24;
			break;
		}
	}
	*height = y;
}

/**
 *
 *  rct2: 0x6780FE
 */
static void window_scenarioselect_scrollmousedown(rct_window *w, int scrollIndex, int x, int y)
{
	for (sc_list_item *listItem = _listItems; listItem->type != LIST_ITEM_TYPE_END; listItem++) {
		switch (listItem->type) {
		case LIST_ITEM_TYPE_HEADING:
			y -= 18;
			break;
		case LIST_ITEM_TYPE_SCENARIO:
			y -= 24;
			if (y < 0 && !listItem->scenario.is_locked) {
				audio_play_sound_panned(SOUND_CLICK_1, w->width / 2 + w->x, 0, 0, 0);
				scenario_load_and_play_from_path(listItem->scenario.scenario->path);
			}
			break;
		}
		if (y < 0) {
			break;
		}
	}
}

/**
 *
 *  rct2: 0x678162
 */
static void window_scenarioselect_scrollmouseover(rct_window *w, int scrollIndex, int x, int y)
{
	scenario_index_entry *selected = NULL;
	for (sc_list_item *listItem = _listItems; listItem->type != LIST_ITEM_TYPE_END; listItem++) {
		switch (listItem->type) {
		case LIST_ITEM_TYPE_HEADING:
			y -= 18;
			break;
		case LIST_ITEM_TYPE_SCENARIO:
			y -= 24;
			if (y < 0 && !listItem->scenario.is_locked) {
				selected = listItem->scenario.scenario;
			}
			break;
		}
		if (y < 0) {
			break;
		}
	}

	if (w->highlighted_scenario != selected) {
		w->highlighted_scenario = selected;
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

	int windowWidth = w->width;
	window_scenarioselect_widgets[WIDX_BACKGROUND].right = windowWidth - 1;
	window_scenarioselect_widgets[WIDX_TITLEBAR].right = windowWidth - 2;
	window_scenarioselect_widgets[WIDX_CLOSE].left  = windowWidth - 13;
	window_scenarioselect_widgets[WIDX_CLOSE].right = windowWidth - 3;
	window_scenarioselect_widgets[WIDX_TABCONTENT].right = windowWidth - 1;
	window_scenarioselect_widgets[WIDX_SCENARIOLIST].right = windowWidth - 179;

	int windowHeight = w->height;
	window_scenarioselect_widgets[WIDX_BACKGROUND].bottom = windowHeight - 1;
	window_scenarioselect_widgets[WIDX_TABCONTENT].bottom = windowHeight - 1;

	const int bottomMargin = gConfigGeneral.debugging_tools ? 17 : 5;
	window_scenarioselect_widgets[WIDX_SCENARIOLIST].bottom = windowHeight - bottomMargin;
}

static void window_scenarioselect_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
	int i, x, y, format;
	rct_widget *widget;
	scenario_index_entry *scenario;

	window_draw_widgets(w, dpi);

	format = (theme_get_preset()->features.rct1_scenario_font) ? 5138 : 1193;

	// Text for each tab
	for (i = 0; i < 8; i++) {
		widget = &window_scenarioselect_widgets[WIDX_TAB1 + i];
		if (widget->type == WWT_EMPTY)
			continue;

		x = (widget->left + widget->right) / 2 + w->x;
		y = (widget->top + widget->bottom) / 2 + w->y - 3;

		if (gConfigGeneral.scenario_select_mode == SCENARIO_SELECT_MODE_ORIGIN) {
			RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS + 0, short) = STR_SCENARIO_CATEGORY_RCT1 + i;
		} else { // old-style
			RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS + 0, short) = ScenarioCategoryStringIds[i];
		}
		gfx_draw_string_centred_wrapped(dpi, (void*)RCT2_ADDRESS_COMMON_FORMAT_ARGS, x, y, 87, format, 10);
	}

	// Return if no scenario highlighted
	scenario = w->highlighted_scenario;
	if (scenario == NULL) {
		return;
	}

	// Scenario path
	if (gConfigGeneral.debugging_tools) {
		utf8 path[MAX_PATH];
		shorten_path(path, sizeof(path), scenario->path, w->width - 6);

		const utf8 *pathPtr = path;
		gfx_draw_string_left(dpi, 1170, (void*)&pathPtr, w->colours[1], w->x + 3, w->y + w->height - 3 - 11);
	}

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
	if (scenario->highscore != NULL) {
		const utf8 *completedByName = "???";
		if (!str_is_null_or_empty(scenario->highscore->name)) {
			completedByName = scenario->highscore->name;
		}
		safe_strncpy((char*)0x009BC677, completedByName, 64);
		RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS + 0, short) = 3165; // empty string
		RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS + 2, int) = scenario->highscore->company_value;
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

	bool wide = gConfigGeneral.scenario_select_mode == SCENARIO_SELECT_MODE_ORIGIN;

	rct_widget *listWidget = &w->widgets[WIDX_SCENARIOLIST];
	int listWidth = listWidget->right - listWidget->left - 12;

	int y = 0;
	for (sc_list_item *listItem = _listItems; listItem->type != LIST_ITEM_TYPE_END; listItem++) {
		if (y > dpi->y + dpi->height) {
			continue;
		}

		switch (listItem->type) {
		case LIST_ITEM_TYPE_HEADING:;
			const int horizontalRuleMargin = 4;
			draw_category_heading(w, dpi, horizontalRuleMargin, listWidth - horizontalRuleMargin, y + 2, listItem->heading.string_id);
			y += 18;
			break;
		case LIST_ITEM_TYPE_SCENARIO:;
			// Draw hover highlight
			scenario_index_entry *scenario = listItem->scenario.scenario;
			bool isHighlighted = w->highlighted_scenario == scenario;
			if (isHighlighted) {
				gfx_fill_rect(dpi, 0, y, w->width, y + 23, 0x02000031);
			}

			bool isCompleted = scenario->highscore != NULL;
			bool isDisabled = listItem->scenario.is_locked;

			// Draw scenario name
			rct_string_id placeholderStringId = 3165;
			safe_strncpy((char*)language_get_string(placeholderStringId), scenario->name, 64);
			int format = isDisabled ? 865 : (isHighlighted ? highlighted_format : unhighlighted_format);
			colour = isDisabled ? w->colours[1] | 0x40 : COLOUR_BLACK;
			gfx_draw_string_centred(dpi, format, wide ? 270 : 210, y + 1, colour, &placeholderStringId);

			// Check if scenario is completed
			if (isCompleted) {
				// Draw completion tick
				gfx_draw_sprite(dpi, 0x5A9F, wide ? 500 : 395, y + 1, 0);

				// Draw completion score
				const utf8 *completedByName = "???";
				if (!str_is_null_or_empty(scenario->highscore->name)) {
					completedByName = scenario->highscore->name;
				}
				safe_strncpy((char*)language_get_string(placeholderStringId), completedByName, 64);
				RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS + 0, rct_string_id) = 2793;
				RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS + 2, rct_string_id) = placeholderStringId;
				gfx_draw_string_centred(dpi, format, wide ? 270 : 210, y + 11, 0, (void*)RCT2_ADDRESS_COMMON_FORMAT_ARGS);
			}

			y += 24;
			break;
		}
	}
}

static void draw_category_heading(rct_window *w, rct_drawpixelinfo *dpi, int left, int right, int y, rct_string_id stringId)
{
	uint8 baseColour = w->colours[1];
	uint8 lightColour = ColourMapA[baseColour].lighter;
	uint8 darkColour = ColourMapA[baseColour].mid_dark;

	// Draw string
	int centreX = (left + right) / 2;
	gfx_draw_string_centred(dpi, stringId, centreX, y, baseColour, NULL);
	
	// Get string dimensions
	utf8 *buffer = RCT2_ADDRESS(RCT2_ADDRESS_COMMON_STRING_FORMAT_BUFFER, utf8);
	format_string(buffer, stringId, NULL);
	int categoryStringHalfWidth = (gfx_get_string_width(buffer) / 2) + 4;
	int strLeft = centreX - categoryStringHalfWidth;
	int strRight = centreX + categoryStringHalfWidth;

	// Draw light horizontal rule
	int lineY = y + 4;
	gfx_draw_line(dpi, left, lineY, strLeft, lineY, lightColour);
	gfx_draw_line(dpi, strRight, lineY, right, lineY, lightColour);

	// Draw dark horizontal rule
	lineY++;
	gfx_draw_line(dpi, left, lineY, strLeft, lineY, darkColour);
	gfx_draw_line(dpi, strRight, lineY, right, lineY, darkColour);
}

static void initialise_list_items(rct_window *w)
{
	SafeFree(_listItems);

	int capacity = gScenarioListCount + 16;
	int length = 0;
	_listItems = malloc(capacity * sizeof(sc_list_item));

	// Mega park unlock
	const uint32 rct1RequiredCompletedScenarios = (1 << SC_MEGA_PARK) - 1;
	uint32 rct1CompletedScenarios = 0;
	int megaParkListItemIndex = -1;

	int numUnlocks = INITIAL_NUM_UNLOCKED_SCENARIOS;
	uint8 currentHeading = UINT8_MAX;
	for (int i = 0; i < gScenarioListCount; i++) {
		scenario_index_entry *scenario = &gScenarioList[i];
		if (!is_scenario_visible(w, scenario)) {
			continue;
		}

		sc_list_item *listItem;

		// Category heading
		rct_string_id headingStringId = STR_NONE;
		if (gConfigGeneral.scenario_select_mode == SCENARIO_SELECT_MODE_ORIGIN) {
			if (w->selected_tab != SCENARIO_SOURCE_REAL && currentHeading != scenario->category) {
				currentHeading = scenario->category;
				headingStringId = ScenarioCategoryStringIds[currentHeading];
			}
		} else {
			if (w->selected_tab <= SCENARIO_CATEGORY_EXPERT) {
				if (currentHeading != scenario->source_game) {
					currentHeading = scenario->source_game;
					headingStringId = STR_SCENARIO_CATEGORY_RCT1 + currentHeading;
				}
			} else if (w->selected_tab == SCENARIO_CATEGORY_OTHER) {
				int category = scenario->category;
				if (category <= SCENARIO_CATEGORY_REAL) {
					category = SCENARIO_CATEGORY_OTHER;
				}
				if (currentHeading != category) {
					currentHeading = category;
					headingStringId = ScenarioCategoryStringIds[category];
				}
			}
		}
		if (headingStringId != (rct_string_id)STR_NONE) {
			// Ensure list capacity
			if (length == capacity) {
				capacity += 32;
				_listItems = realloc(_listItems, capacity * sizeof(sc_list_item));
			}
			listItem = &_listItems[length++];

			listItem->type = LIST_ITEM_TYPE_HEADING;
			listItem->heading.string_id = headingStringId;
		}

		// Ensure list capacity
		if (length == capacity) {
			capacity += 32;
			_listItems = realloc(_listItems, capacity * sizeof(sc_list_item));
		}
		listItem = &_listItems[length++];

		// Scenario
		listItem->type = LIST_ITEM_TYPE_SCENARIO;
		listItem->scenario.scenario = scenario;
		if (is_locking_enabled(w)) {
			listItem->scenario.is_locked = numUnlocks <= 0;
			if (scenario->highscore == NULL) {
				numUnlocks--;
			} else {
				// Mark RCT1 scenario as completed
				if (scenario->sc_id < SC_MEGA_PARK) {
					rct1CompletedScenarios |= 1 << scenario->sc_id;
				}
			}

			// If scenario is Mega Park, keep a reference to it
			if (scenario->sc_id == SC_MEGA_PARK) {
				megaParkListItemIndex = length - 1;
			}
		} else {
			listItem->scenario.is_locked = false;
		}
	}

	length++;
	_listItems = realloc(_listItems, length * sizeof(sc_list_item));
	_listItems[length - 1].type = LIST_ITEM_TYPE_END;

	// Mega park handling
	if (megaParkListItemIndex != -1) {
		bool megaParkLocked = (rct1CompletedScenarios & rct1RequiredCompletedScenarios) != rct1RequiredCompletedScenarios;
		_listItems[megaParkListItemIndex].scenario.is_locked = megaParkLocked;
		if (megaParkLocked && gConfigGeneral.scenario_hide_mega_park) {
			// Remove mega park
			int remainingItems = length - megaParkListItemIndex - 1;
			memmove(&_listItems[megaParkListItemIndex], &_listItems[megaParkListItemIndex + 1], remainingItems);

			// Remove empty headings
			int i = 0;
			for (sc_list_item *listItem = _listItems; listItem->type != LIST_ITEM_TYPE_END; listItem++) {
				if (listItem->type == LIST_ITEM_TYPE_HEADING && (listItem + 1)->type != LIST_ITEM_TYPE_SCENARIO) {
					remainingItems = length - i - 1;
					memmove(&_listItems[i], &_listItems[i + 1], remainingItems);
					listItem--;
				} else {
					i++;
				}
			}
		}
	}
}

static bool is_scenario_visible(rct_window *w, scenario_index_entry *scenario)
{
	if (gConfigGeneral.scenario_select_mode == SCENARIO_SELECT_MODE_ORIGIN) {
		if (scenario->source_game != w->selected_tab) {
			return false;
		}
	} else {
		int category = scenario->category;
		if (category > SCENARIO_CATEGORY_OTHER) {
			category = SCENARIO_CATEGORY_OTHER;
		}
		if (category != w->selected_tab) {
			return false;
		}
	}
	return true;
}

static bool is_locking_enabled(rct_window *w)
{
	if (gConfigGeneral.scenario_select_mode != SCENARIO_SELECT_MODE_ORIGIN) {
		return false;
	}
	if (!gConfigGeneral.scenario_unlocking_enabled) {
		return false;
	}
	if (w->selected_tab >= 6) {
		return false;
	}
	return true;
}
