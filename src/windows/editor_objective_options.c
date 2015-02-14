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
#include "../game.h"
#include "../localisation/date.h"
#include "../localisation/localisation.h"
#include "../interface/widget.h"
#include "../interface/window.h"
#include "../scenario.h"
#include "../world/park.h"
#include "dropdown.h"
#include "error.h"

#define DISABLE_SIX_FLAGS_CHECKBOX

#pragma region Widgets

enum {
	WINDOW_EDITOR_OBJECTIVE_OPTIONS_PAGE_MAIN,
	WINDOW_EDITOR_OBJECTIVE_OPTIONS_PAGE_RIDES,
	WINDOW_EDITOR_OBJECTIVE_OPTIONS_PAGE_COUNT
};

enum {
	WIDX_BACKGROUND,
	WIDX_TITLE,
	WIDX_CLOSE,
	WIDX_PAGE_BACKGROUND,
	WIDX_TAB_1,
	WIDX_TAB_2,

	WIDX_OBJECTIVE = 6,
	WIDX_OBJECTIVE_DROPDOWN,
	WIDX_OBJECTIVE_ARG_1,
	WIDX_OBJECTIVE_ARG_1_INCREASE,
	WIDX_OBJECTIVE_ARG_1_DECREASE,
	WIDX_OBJECTIVE_ARG_2,
	WIDX_OBJECTIVE_ARG_2_INCREASE,
	WIDX_OBJECTIVE_ARG_2_DECREASE,
	WIDX_CLIMATE,
	WIDX_CLIMATE_DROPDOWN,
	WIDX_PARK_NAME,
	WIDX_SCENARIO_NAME,
	WIDX_CATEGORY,
	WIDX_CATEGORY_DROPDOWN,
	WIDX_DETAILS,
	WIDX_SIX_FLAGS,

	WIDX_RIDES = 6
};

static rct_widget window_editor_objective_options_main_widgets[] = {
	{ WWT_FRAME,			0,	0,		449,	0,		228,	STR_NONE,					STR_NONE											},
	{ WWT_CAPTION,			0,	1,		448,	1,		14,		STR_OBJECTIVE_SELECTION,	STR_WINDOW_TITLE_TIP								},
	{ WWT_CLOSEBOX,			0,	437,	447,	2,		13,		STR_CLOSE_X,				STR_CLOSE_WINDOW_TIP								},
	{ WWT_RESIZE,			1,	0,		279,	43,		148,	STR_NONE,					STR_NONE											},
	{ WWT_TAB,				1,	3,		33,		17,		43,		0x2000144E,					STR_SELECT_OBJECTIVE_AND_PARK_NAME_TIP				},
	{ WWT_TAB,				1,	34,		64,		17,		46,		0x2000144E,					STR_SELECT_RIDES_TO_BE_PRESERVED_TIP				},
	{ WWT_DROPDOWN,			1,	98,		441,	48,		59,		STR_NONE,					STR_SELECT_OBJECTIVE_FOR_THIS_SCENARIO_TIP			},
	{ WWT_DROPDOWN_BUTTON,	1,	430,	440,	49,		58,		STR_DROPDOWN_GLYPH,			STR_SELECT_OBJECTIVE_FOR_THIS_SCENARIO_TIP			},
	{ WWT_SPINNER,			1,	158,	237,	65,		76,		STR_NONE,					STR_NONE											},
	{ WWT_DROPDOWN_BUTTON,	1,	226,	236,	66,		70,		STR_NUMERIC_UP,				STR_NONE											},
	{ WWT_DROPDOWN_BUTTON,	1,	226,	236,	71,		75,		STR_NUMERIC_DOWN,			STR_NONE											},
	{ WWT_SPINNER,			1,	158,	277,	82,		93,		STR_NONE,					STR_NONE											},
	{ WWT_DROPDOWN_BUTTON,	1,	266,	276,	83,		87,		STR_NUMERIC_UP,				STR_NONE											},
	{ WWT_DROPDOWN_BUTTON,	1,	266,	276,	88,		92,		STR_NUMERIC_DOWN,			STR_NONE											},
	{ WWT_DROPDOWN,			1,	98,		277,	99,		110,	STR_NONE,					STR_SELECT_CLIMATE_TIP								},
	{ WWT_DROPDOWN_BUTTON,	1,	266,	276,	100,	109,	STR_DROPDOWN_GLYPH,			STR_SELECT_CLIMATE_TIP								},
	{ WWT_DROPDOWN_BUTTON,	1,	370,	444,	116,	127,	STR_CHANGE,					STR_CHANGE_NAME_OF_PARK_TIP							},
	{ WWT_DROPDOWN_BUTTON,	1,	370,	444,	133,	144,	STR_CHANGE,					STR_CHANGE_NAME_OF_SCENARIO_TIP						},
	{ WWT_DROPDOWN,			1,	98,		277,	150,	161,	STR_NONE,					STR_SELECT_WHICH_GROUP_THIS_SCENARIO_APPEARS_IN		},
	{ WWT_DROPDOWN_BUTTON,	1,	266,	276,	151,	160,	STR_DROPDOWN_GLYPH,			STR_SELECT_WHICH_GROUP_THIS_SCENARIO_APPEARS_IN		},
	{ WWT_DROPDOWN_BUTTON,	1,	370,	444,	167,	178,	STR_CHANGE,					STR_CHANGE_DETAIL_NOTES_ABOUT_PARK_SCENARIO_TIP		},
	{ WWT_CHECKBOX,			1,	8,		441,	215,	226,	STR_SIX_FLAGS_PARK,			STR_NONE											},
	{ WIDGETS_END }
};

static rct_widget window_editor_objective_options_rides_widgets[] = {
	{ WWT_FRAME,			0,	0,		449,	0,		228,	STR_NONE,					STR_NONE											},
	{ WWT_CAPTION,			0,	1,		448,	1,		14,		STR_OBJECTIVE_SELECTION,	STR_WINDOW_TITLE_TIP								},
	{ WWT_CLOSEBOX,			0,	437,	447,	2,		13,		STR_CLOSE_X,				STR_CLOSE_WINDOW_TIP								},
	{ WWT_RESIZE,			1,	0,		279,	43,		148,	STR_NONE,					STR_NONE											},
	{ WWT_TAB,				1,	3,		33,		17,		43,		0x2000144E,					STR_SELECT_OBJECTIVE_AND_PARK_NAME_TIP				},
	{ WWT_TAB,				1,	34,		64,		17,		46,		0x2000144E,					STR_SELECT_RIDES_TO_BE_PRESERVED_TIP				},
	{ WWT_SCROLL,			1,	3,		376,	60,		220,	2,							STR_NONE											},
	{ WIDGETS_END }
};

static rct_widget *window_editor_objective_options_widgets[] = {
	window_editor_objective_options_main_widgets,
	window_editor_objective_options_rides_widgets
};

#pragma endregion

#pragma region Events

static void window_editor_objective_options_emptysub() { }

static void window_editor_objective_options_main_mouseup();
static void window_editor_objective_options_main_resize();
static void window_editor_objective_options_main_mousedown(int widgetIndex, rct_window*w, rct_widget* widget);
static void window_editor_objective_options_main_dropdown();
static void window_editor_objective_options_main_update(rct_window *w);
static void window_editor_objective_options_main_textinput();
static void window_editor_objective_options_main_invalidate();
static void window_editor_objective_options_main_paint();

static void window_editor_objective_options_rides_mouseup();
static void window_editor_objective_options_rides_resize();
static void window_editor_objective_options_rides_update(rct_window *w);
static void window_editor_objective_options_rides_scrollgetheight();
static void window_editor_objective_options_rides_scrollmousedown();
static void window_editor_objective_options_rides_scrollmouseover();
static void window_editor_objective_options_rides_invalidate();
static void window_editor_objective_options_rides_paint();
static void window_editor_objective_options_rides_scrollpaint();

// 0x009A9DF4
static void* window_objective_options_main_events[] = {
	window_editor_objective_options_emptysub,
	window_editor_objective_options_main_mouseup,
	window_editor_objective_options_main_resize,
	window_editor_objective_options_main_mousedown,
	window_editor_objective_options_main_dropdown,
	window_editor_objective_options_emptysub,
	window_editor_objective_options_main_update,
	window_editor_objective_options_emptysub,
	window_editor_objective_options_emptysub,
	window_editor_objective_options_emptysub,
	window_editor_objective_options_emptysub,
	window_editor_objective_options_emptysub,
	window_editor_objective_options_emptysub,
	window_editor_objective_options_emptysub,
	window_editor_objective_options_emptysub,
	window_editor_objective_options_emptysub,
	window_editor_objective_options_emptysub,
	window_editor_objective_options_emptysub,
	window_editor_objective_options_emptysub,
	window_editor_objective_options_main_textinput,
	window_editor_objective_options_emptysub,
	window_editor_objective_options_emptysub,
	window_editor_objective_options_emptysub,
	window_editor_objective_options_emptysub,
	window_editor_objective_options_emptysub,
	window_editor_objective_options_main_invalidate,
	window_editor_objective_options_main_paint,
	window_editor_objective_options_emptysub
};

// 0x009A9F58
static void* window_objective_options_rides_events[] = {
	window_editor_objective_options_emptysub,
	window_editor_objective_options_rides_mouseup,
	window_editor_objective_options_rides_resize,
	window_editor_objective_options_emptysub,
	window_editor_objective_options_emptysub,
	window_editor_objective_options_emptysub,
	window_editor_objective_options_rides_update,
	window_editor_objective_options_emptysub,
	window_editor_objective_options_emptysub,
	window_editor_objective_options_emptysub,
	window_editor_objective_options_emptysub,
	window_editor_objective_options_emptysub,
	window_editor_objective_options_emptysub,
	window_editor_objective_options_emptysub,
	window_editor_objective_options_emptysub,
	window_editor_objective_options_rides_scrollgetheight,
	window_editor_objective_options_rides_scrollmousedown,
	window_editor_objective_options_emptysub,
	window_editor_objective_options_rides_scrollmouseover,
	window_editor_objective_options_emptysub,
	window_editor_objective_options_emptysub,
	window_editor_objective_options_emptysub,
	window_editor_objective_options_emptysub,
	window_editor_objective_options_emptysub,
	window_editor_objective_options_emptysub,
	window_editor_objective_options_rides_invalidate,
	window_editor_objective_options_rides_paint,
	window_editor_objective_options_rides_scrollpaint
};

static void* window_editor_objective_options_page_events[] = {
	window_objective_options_main_events,
	window_objective_options_rides_events
};

#pragma endregion

#pragma region Enabled widgets

static uint64 window_editor_objective_options_page_enabled_widgets[] = {
	(1 << WIDX_CLOSE) |
	(1 << WIDX_TAB_1) |
	(1 << WIDX_TAB_2) |
	(1 << WIDX_OBJECTIVE) |
	(1 << WIDX_OBJECTIVE_DROPDOWN) |
	(1 << WIDX_OBJECTIVE_ARG_1_INCREASE) |
	(1 << WIDX_OBJECTIVE_ARG_1_DECREASE) |
	(1 << WIDX_OBJECTIVE_ARG_2_INCREASE) |
	(1 << WIDX_OBJECTIVE_ARG_2_DECREASE) |
	(1 << WIDX_CLIMATE) |
	(1 << WIDX_CLIMATE_DROPDOWN) |
	(1 << WIDX_PARK_NAME) |
	(1 << WIDX_SCENARIO_NAME) |
	(1 << WIDX_CATEGORY) |
	(1 << WIDX_CATEGORY_DROPDOWN) |
	(1 << WIDX_DETAILS) |
	(1 << WIDX_SIX_FLAGS),

	(1 << WIDX_CLOSE) |
	(1 << WIDX_TAB_1) |
	(1 << WIDX_TAB_2)
};

static uint64 window_editor_objective_options_page_hold_down_widgets[] = {
	(1 << WIDX_OBJECTIVE_ARG_1_INCREASE) |
	(1 << WIDX_OBJECTIVE_ARG_1_DECREASE) |
	(1 << WIDX_OBJECTIVE_ARG_2_INCREASE) |
	(1 << WIDX_OBJECTIVE_ARG_2_DECREASE),

	0
};

#pragma endregion

/**
 *
 *  rct2: 0x0067137D
 */
void window_editor_objective_options_open()
{
	rct_window *w;

	w = window_bring_to_front_by_class(WC_EDTIOR_OBJECTIVE_OPTIONS);
	if (w != NULL)
		return;

	w = window_create_centred(
		450,
		228,
		(uint32*)window_objective_options_main_events,
		WC_EDTIOR_OBJECTIVE_OPTIONS,
		WF_10
	);
	w->widgets = window_editor_objective_options_main_widgets;
	w->enabled_widgets = window_editor_objective_options_page_enabled_widgets[WINDOW_EDITOR_OBJECTIVE_OPTIONS_PAGE_MAIN];
	w->pressed_widgets = 0;
	w->hold_down_widgets = window_editor_objective_options_page_hold_down_widgets[WINDOW_EDITOR_OBJECTIVE_OPTIONS_PAGE_MAIN];
	window_init_scroll_widgets(w);
	w->var_4AE = 0;
	w->selected_tab = WINDOW_EDITOR_OBJECTIVE_OPTIONS_PAGE_MAIN;
	w->no_list_items = 0;
	w->selected_list_item = -1;
	RCT2_CALLPROC_X(0x00672609, 0, 0, 0, 0, (int)w, 0, 0);
	w->colours[0] = 4;
	w->colours[1] = 1;
	w->colours[2] = 1;
}

static void window_editor_objective_options_set_pressed_tab(rct_window *w)
{
	int i;
	for (i = 0; i < 2; i++)
		w->pressed_widgets &= ~(1 << (WIDX_TAB_1 + i));
	w->pressed_widgets |= 1LL << (WIDX_TAB_1 + w->page);
}

static void window_editor_objective_options_anchor_border_widgets(rct_window *w)
{
	w->widgets[WIDX_BACKGROUND].right = w->width - 1;
	w->widgets[WIDX_BACKGROUND].bottom = w->height - 1;
	w->widgets[WIDX_PAGE_BACKGROUND].right = w->width - 1;
	w->widgets[WIDX_PAGE_BACKGROUND].bottom = w->height - 1;
	w->widgets[WIDX_TITLE].right = w->width - 2;
	w->widgets[WIDX_CLOSE].left = w->width - 13;
	w->widgets[WIDX_CLOSE].right = w->width - 3;
}

static void window_editor_objective_options_draw_tab_images(rct_window *w, rct_drawpixelinfo *dpi)
{
	rct_widget *widget;
	int spriteIndex;

	// Tab 1
	widget = &w->widgets[WIDX_TAB_1];

	spriteIndex = 5511;
	if (w->page == WINDOW_EDITOR_OBJECTIVE_OPTIONS_PAGE_MAIN)
		spriteIndex += (w->frame_no / 4) % 16;

	gfx_draw_sprite(dpi, spriteIndex, w->x + widget->left, w->y + widget->top, 0);

	// Tab 2
	if (!(w->disabled_widgets & (1 << WIDX_TAB_2))) {
		widget = &w->widgets[WIDX_TAB_2];
		spriteIndex = 5442;
		if (w->page == WINDOW_EDITOR_OBJECTIVE_OPTIONS_PAGE_RIDES)
			spriteIndex += (w->frame_no / 4) % 16;

		gfx_draw_sprite(dpi, spriteIndex, w->x + widget->left, w->y + widget->top, 0);
	}
}

/**
 *
 *  rct2: 0x00668496
 */
static void window_editor_objective_options_set_page(rct_window *w, int page)
{
	if (w->page == page)
		return;

	w->page = page;
	w->frame_no = 0;
	w->var_492 = 0;
	w->no_list_items = 0;
	w->selected_list_item = -1;
	w->enabled_widgets = window_editor_objective_options_page_enabled_widgets[page];
	w->hold_down_widgets = window_editor_objective_options_page_hold_down_widgets[page];
	w->event_handlers = window_editor_objective_options_page_events[page];
	w->widgets = window_editor_objective_options_widgets[page];
	window_invalidate(w);
	RCT2_CALLPROC_X(0x00672609, 0, 0, 0, 0, (int)w, 0, 0);
	window_event_resize_call(w);
	window_event_invalidate_call(w);
	window_init_scroll_widgets(w);
	window_invalidate(w);
}

/**
 *
 *  rct2: 0x0067201D
 */
static void window_editor_objective_options_set_objective(rct_window *w, int objective)
{
	RCT2_GLOBAL(RCT2_ADDRESS_OBJECTIVE_TYPE, uint8) = objective;
	window_invalidate(w);

	// Set default objective arguments
	switch (objective) {
	case OBJECTIVE_NONE:
	case OBJECTIVE_HAVE_FUN:
	case OBJECTIVE_BUILD_THE_BEST:
	case OBJECTIVE_10_ROLLERCOASTERS:
		break;
	case OBJECTIVE_GUESTS_BY:
		RCT2_GLOBAL(RCT2_ADDRESS_OBJECTIVE_YEAR, uint8) = 3;
		RCT2_GLOBAL(RCT2_ADDRESS_OBJECTIVE_NUM_GUESTS, uint16) = 1500;
		break;
	case OBJECTIVE_PARK_VALUE_BY:
		RCT2_GLOBAL(RCT2_ADDRESS_OBJECTIVE_YEAR, uint8) = 3;
		RCT2_GLOBAL(RCT2_ADDRESS_OBJECTIVE_CURRENCY, money32) = MONEY(50000,00);
		break;
	case OBJECTIVE_GUESTS_AND_RATING:
		RCT2_GLOBAL(RCT2_ADDRESS_OBJECTIVE_NUM_GUESTS, uint16) = 2000;
		break;
	case OBJECTIVE_MONTHLY_RIDE_INCOME:
		RCT2_GLOBAL(RCT2_ADDRESS_OBJECTIVE_CURRENCY, money32) = MONEY(10000,00);
		break;
	case OBJECTIVE_10_ROLLERCOASTERS_LENGTH:
		RCT2_GLOBAL(RCT2_ADDRESS_OBJECTIVE_NUM_GUESTS, uint16) = 1200;
		break;
	case OBJECTIVE_FINISH_5_ROLLERCOASTERS:
		RCT2_GLOBAL(RCT2_ADDRESS_OBJECTIVE_CURRENCY, money32) = FIXED_2DP(6,70);
		break;
	case OBJECTIVE_REPLAY_LOAN_AND_PARK_VALUE:
		RCT2_GLOBAL(RCT2_ADDRESS_OBJECTIVE_CURRENCY, money32) = MONEY(50000,00);
		break;
	case OBJECTIVE_MONTHLY_FOOD_INCOME:
		RCT2_GLOBAL(RCT2_ADDRESS_OBJECTIVE_CURRENCY, money32) = MONEY(1000,00);
		break;
	}
}

/**
 *
 *  rct2: 0x006719CA
 */
static void window_editor_objective_options_main_mouseup()
{
	rct_s6_info *s6Info = (rct_s6_info*)0x00141F570;
	short widgetIndex;
	rct_window *w;

	window_widget_get_registers(w, widgetIndex);

	switch (widgetIndex) {
	case WIDX_CLOSE:
		window_close(w);
		break;
	case WIDX_TAB_1:
	case WIDX_TAB_2:
		window_editor_objective_options_set_page(w, widgetIndex - WIDX_TAB_1);
		break;
	case WIDX_PARK_NAME:
		RCT2_GLOBAL(0x013CE962, uint32) = RCT2_GLOBAL(0x013573D8, uint32);
		window_text_input_open(w, WIDX_PARK_NAME, STR_PARK_NAME, STR_ENTER_PARK_NAME, RCT2_GLOBAL(RCT2_ADDRESS_PARK_NAME, rct_string_id), 0, 32);
		break;
	case WIDX_SCENARIO_NAME:
		strcpy((char*)0x009BC677, s6Info->name);
		window_text_input_open(w, WIDX_SCENARIO_NAME, 3313, 3314, 3165, 0, 32);
		break;
	case WIDX_DETAILS:
		strcpy((char*)0x009BC677, s6Info->details);
		window_text_input_open(w, WIDX_DETAILS, 3315, 3316, 3165, 0, 256);
		break;
	case WIDX_SIX_FLAGS:
		RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) ^= PARK_FLAGS_SIX_FLAGS;
		window_invalidate(w);
		break;
	}
}

/**
 *
 *  rct2: 0x00672254
 */
static void window_editor_objective_options_main_resize()
{
	rct_window *w;

	window_get_register(w);

	window_set_resize(w, 450, 229, 450, 229);
}

static void window_editor_objective_options_show_objective_dropdown(rct_window *w)
{
	int i, numItems, objectiveType;
	rct_widget *dropdownWidget;
	uint32 parkFlags;

	dropdownWidget = &w->widgets[WIDX_OBJECTIVE];
	parkFlags = RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32);
	numItems = 0;

	if (!(parkFlags & PARK_FLAGS_NO_MONEY_SCENARIO)) {
		numItems += 2;
		if (parkFlags & PARK_FLAGS_PARK_FREE_ENTRY)
			numItems++;
	}

	numItems += 5;

	i = 0;
	gDropdownItemsFormat[i] = 1142;
	gDropdownItemsArgs[i] = STR_OBJECTIVE_DROPDOWN_HAVE_FUN;
	i++;

	if (!(parkFlags & PARK_FLAGS_NO_MONEY_SCENARIO)) {
		gDropdownItemsFormat[i] = 1142;
		gDropdownItemsArgs[i] = STR_OBJECTIVE_DROPDOWN_NUMBER_OF_GUESTS_AT_A_GIVEN_DATE;
		i++;
		gDropdownItemsFormat[i] = 1142;
		gDropdownItemsArgs[i] = STR_OBJECTIVE_DROPDOWN_MONTHLY_PROFIT_FROM_FOOD_MERCHANDISE;
		i++;
		gDropdownItemsFormat[i] = 1142;
		gDropdownItemsArgs[i] = STR_OBJECTIVE_DROPDOWN_REPAY_LOAN_AND_ACHIEVE_A_GIVEN_PARK_VALUE;
		i++;
		gDropdownItemsFormat[i] = 1142;
		gDropdownItemsArgs[i] = STR_OBJECTIVE_DROPDOWN_PARK_VALUE_AT_A_GIVEN_DATE;
		i++;
		if (parkFlags & PARK_FLAGS_PARK_FREE_ENTRY) {
			gDropdownItemsFormat[i] = 1142;
			gDropdownItemsArgs[i] = STR_OBJECTIVE_DROPDOWN_MONTHLY_INCOME_FROM_RIDE_TICKETS;
			i++;
		}
	}

	gDropdownItemsFormat[i] = 1142;
	gDropdownItemsArgs[i] = STR_OBJECTIVE_DROPDOWN_NUMBER_OF_GUESTS_IN_PARK;
	i++;

	gDropdownItemsFormat[i] = 1142;
	gDropdownItemsArgs[i] = STR_OBJECTIVE_DROPDOWN_BUILD_10_ROLLER_COASTERS;
	i++;

	gDropdownItemsFormat[i] = 1142;
	gDropdownItemsArgs[i] = STR_OBJECTIVE_DROPDOWN_BUILD_10_ROLLER_COASTERS_OF_A_GIVEN_LENGTH;
	i++;

	gDropdownItemsFormat[i] = 1142;
	gDropdownItemsArgs[i] = STR_OBJECTIVE_DROPDOWN_FINISH_BUILDING_5_ROLLER_COASTERS;
	i++;

	window_dropdown_show_text_custom_width(
		w->x + dropdownWidget->left,
		w->y + dropdownWidget->top,
		dropdownWidget->bottom - dropdownWidget->top + 1,
		w->colours[1],
		0x80,
		numItems,
		dropdownWidget->right - dropdownWidget->left - 3
	);

	objectiveType = RCT2_GLOBAL(RCT2_ADDRESS_OBJECTIVE_TYPE, uint8);
	for (i = 0; i < numItems; i++) {
		if (gDropdownItemsArgs[i] - STR_OBJECTIVE_DROPDOWN_NONE == objectiveType) {
			gDropdownItemsChecked = (1 << i);
			break;
		}
	}
}

static void window_editor_objective_options_show_climate_dropdown(rct_window *w)
{
	int i;
	rct_widget *dropdownWidget;
	
	dropdownWidget = &w->widgets[WIDX_CLIMATE];

	for (i = 0; i < 4; i++) {
		gDropdownItemsFormat[i] = 1142;
		gDropdownItemsArgs[i] = STR_CLIMATE_COOL_AND_WET + i;
	}
	window_dropdown_show_text_custom_width(
		w->x + dropdownWidget->left,
		w->y + dropdownWidget->top,
		dropdownWidget->bottom - dropdownWidget->top + 1,
		w->colours[1],
		0x80,
		4,
		dropdownWidget->right - dropdownWidget->left - 3
	);
	gDropdownItemsChecked = (1 << RCT2_GLOBAL(RCT2_ADDRESS_CLIMATE, uint8));
}

static void window_editor_objective_options_show_category_dropdown(rct_window *w)
{
	rct_s6_info *s6Info = (rct_s6_info*)0x00141F570;
	int i;
	rct_widget *dropdownWidget;
	
	dropdownWidget = &w->widgets[WIDX_CATEGORY];

	for (i = 0; i < 5; i++) {
		gDropdownItemsFormat[i] = 1142;
		gDropdownItemsArgs[i] = STR_BEGINNER_PARKS + i;
	}
	window_dropdown_show_text_custom_width(
		w->x + dropdownWidget->left,
		w->y + dropdownWidget->top,
		dropdownWidget->bottom - dropdownWidget->top + 1,
		w->colours[1],
		0x80,
		5,
		dropdownWidget->right - dropdownWidget->left - 3
	);
	gDropdownItemsChecked = (1 << s6Info->category);
}

static void window_editor_objective_options_arg_1_increase(rct_window *w)
{
	switch (RCT2_GLOBAL(RCT2_ADDRESS_OBJECTIVE_TYPE, uint8)) {
	case OBJECTIVE_PARK_VALUE_BY:
	case OBJECTIVE_MONTHLY_RIDE_INCOME:
	case OBJECTIVE_REPLAY_LOAN_AND_PARK_VALUE:
		if (RCT2_GLOBAL(RCT2_ADDRESS_OBJECTIVE_CURRENCY, money32) >= MONEY(2000000,00)) {
			window_error_open(3264, STR_NONE);
		} else {
			RCT2_GLOBAL(RCT2_ADDRESS_OBJECTIVE_CURRENCY, money32) += MONEY(1000,0);
			window_invalidate(w);
		}
		break;
	case OBJECTIVE_MONTHLY_FOOD_INCOME:
		if (RCT2_GLOBAL(RCT2_ADDRESS_OBJECTIVE_CURRENCY, money32) >= MONEY(2000000,00)) {
			window_error_open(3264, STR_NONE);
		} else {
			RCT2_GLOBAL(RCT2_ADDRESS_OBJECTIVE_CURRENCY, money32) += MONEY(100,0);
			window_invalidate(w);
		}
		break;
	case OBJECTIVE_10_ROLLERCOASTERS_LENGTH:
		if (RCT2_GLOBAL(RCT2_ADDRESS_OBJECTIVE_NUM_GUESTS, uint16) >= 5000) {
			window_error_open(3264, STR_NONE);
		} else {
			RCT2_GLOBAL(RCT2_ADDRESS_OBJECTIVE_NUM_GUESTS, uint16) += 100;
			window_invalidate(w);
		}
		break;
	case OBJECTIVE_FINISH_5_ROLLERCOASTERS:
		if (RCT2_GLOBAL(RCT2_ADDRESS_OBJECTIVE_CURRENCY, money32) >= FIXED_2DP(9,90)) {
			window_error_open(3264, STR_NONE);
		} else {
			RCT2_GLOBAL(RCT2_ADDRESS_OBJECTIVE_CURRENCY, money32) += FIXED_2DP(0,10);
			window_invalidate(w);
		}
		break;
	default:
		if (RCT2_GLOBAL(RCT2_ADDRESS_OBJECTIVE_NUM_GUESTS, uint16) >= 5000) {
			window_error_open(3264, STR_NONE);
		} else {
			RCT2_GLOBAL(RCT2_ADDRESS_OBJECTIVE_NUM_GUESTS, uint16) += 100;
			window_invalidate(w);
		}
		break;
	}
}

static void window_editor_objective_options_arg_1_decrease(rct_window *w)
{
	switch (RCT2_GLOBAL(RCT2_ADDRESS_OBJECTIVE_TYPE, uint8)) {
	case OBJECTIVE_PARK_VALUE_BY:
	case OBJECTIVE_MONTHLY_RIDE_INCOME:
	case OBJECTIVE_REPLAY_LOAN_AND_PARK_VALUE:
		if (RCT2_GLOBAL(RCT2_ADDRESS_OBJECTIVE_CURRENCY, money32) <= MONEY(1000,00)) {
			window_error_open(3265, STR_NONE);
		} else {
			RCT2_GLOBAL(RCT2_ADDRESS_OBJECTIVE_CURRENCY, money32) -= MONEY(1000,0);
			window_invalidate(w);
		}
		break;
	case OBJECTIVE_MONTHLY_FOOD_INCOME:
		if (RCT2_GLOBAL(RCT2_ADDRESS_OBJECTIVE_CURRENCY, money32) <= MONEY(1000,00)) {
			window_error_open(3265, STR_NONE);
		} else {
			RCT2_GLOBAL(RCT2_ADDRESS_OBJECTIVE_CURRENCY, money32) -= MONEY(100,0);
			window_invalidate(w);
		}
		break;
	case OBJECTIVE_10_ROLLERCOASTERS_LENGTH:
		if (RCT2_GLOBAL(RCT2_ADDRESS_OBJECTIVE_NUM_GUESTS, uint16) <= 1000) {
			window_error_open(3265, STR_NONE);
		} else {
			RCT2_GLOBAL(RCT2_ADDRESS_OBJECTIVE_NUM_GUESTS, uint16) -= 100;
			window_invalidate(w);
		}
		break;
	case OBJECTIVE_FINISH_5_ROLLERCOASTERS:
		if (RCT2_GLOBAL(RCT2_ADDRESS_OBJECTIVE_CURRENCY, money32) <= FIXED_2DP(4,00)) {
			window_error_open(3265, STR_NONE);
		} else {
			RCT2_GLOBAL(RCT2_ADDRESS_OBJECTIVE_CURRENCY, money32) -= FIXED_2DP(0,10);
			window_invalidate(w);
		}
		break;
	default:
		if (RCT2_GLOBAL(RCT2_ADDRESS_OBJECTIVE_NUM_GUESTS, uint16) <= 500) {
			window_error_open(3265, STR_NONE);
		} else {
			RCT2_GLOBAL(RCT2_ADDRESS_OBJECTIVE_NUM_GUESTS, uint16) -= 100;
			window_invalidate(w);
		}
		break;
	}
}

static void window_editor_objective_options_arg_2_increase(rct_window *w)
{
	if (RCT2_GLOBAL(RCT2_ADDRESS_OBJECTIVE_YEAR, uint8) >= 25) {
		window_error_open(3264, STR_NONE);
	} else {
		RCT2_GLOBAL(RCT2_ADDRESS_OBJECTIVE_YEAR, uint8)++;
		window_invalidate(w);
	}
}

static void window_editor_objective_options_arg_2_decrease(rct_window *w)
{
	if (RCT2_GLOBAL(RCT2_ADDRESS_OBJECTIVE_YEAR, uint8) <= 1) {
		window_error_open(3265, STR_NONE);
	} else {
		RCT2_GLOBAL(RCT2_ADDRESS_OBJECTIVE_YEAR, uint8)--;
		window_invalidate(w);
	}
}

/**
 *
 *  rct2: 0x00671A0D
 */
static void window_editor_objective_options_main_mousedown(int widgetIndex, rct_window *w, rct_widget* widget)
{
	switch (widgetIndex) {
	case WIDX_OBJECTIVE_DROPDOWN:
		window_editor_objective_options_show_objective_dropdown(w);
		break;
	case WIDX_OBJECTIVE_ARG_1_INCREASE:
		window_editor_objective_options_arg_1_increase(w);
		break;
	case WIDX_OBJECTIVE_ARG_1_DECREASE:
		window_editor_objective_options_arg_1_decrease(w);
		break;
	case WIDX_OBJECTIVE_ARG_2_INCREASE:
		window_editor_objective_options_arg_2_increase(w);
		break;
	case WIDX_OBJECTIVE_ARG_2_DECREASE:
		window_editor_objective_options_arg_2_decrease(w);
		break;
	case WIDX_CLIMATE_DROPDOWN:
		window_editor_objective_options_show_climate_dropdown(w);
		break;
	case WIDX_CATEGORY_DROPDOWN:
		window_editor_objective_options_show_category_dropdown(w);
		break;
	}
}

/**
 *
 *  rct2: 0x00671A54
 */
static void window_editor_objective_options_main_dropdown()
{
	rct_s6_info *s6Info = (rct_s6_info*)0x00141F570;
	rct_window *w;
	short widgetIndex, dropdownIndex;
	uint8 newObjectiveType;

	window_dropdown_get_registers(w, widgetIndex, dropdownIndex);

	if (dropdownIndex == -1)
		return;

	switch (widgetIndex) {
	case WIDX_OBJECTIVE_DROPDOWN:
		newObjectiveType = (uint8)(gDropdownItemsArgs[dropdownIndex] - 2397);
		if (RCT2_GLOBAL(RCT2_ADDRESS_OBJECTIVE_TYPE, uint8) != newObjectiveType)
			window_editor_objective_options_set_objective(w, newObjectiveType);
		break;
	case WIDX_CLIMATE_DROPDOWN:
		if (RCT2_GLOBAL(RCT2_ADDRESS_CLIMATE, uint8) != (uint8)dropdownIndex) {
			RCT2_GLOBAL(RCT2_ADDRESS_CLIMATE, uint8) = (uint8)dropdownIndex;
			window_invalidate(w);
		}
		break;
	case WIDX_CATEGORY_DROPDOWN:
		if (s6Info->category != (uint8)dropdownIndex) {
			s6Info->category = (uint8)dropdownIndex;
			window_invalidate(w);
		}
		break;
	}
}

/**
 *
 *  rct2: 0x006721E7
 */
static void window_editor_objective_options_main_update(rct_window *w)
{
	uint32 parkFlags;
	uint8 objectiveType;

	w->frame_no++;
	window_event_invalidate_call(w);
	widget_invalidate(w, WIDX_TAB_1);

	parkFlags = RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32);
	objectiveType = RCT2_GLOBAL(RCT2_ADDRESS_OBJECTIVE_TYPE, uint8);

	// Reset objective if invalid
	if ((
		(parkFlags & PARK_FLAGS_NO_MONEY_SCENARIO) &&

		// The following objectives are the only valid objectives when there is no money
		objectiveType != OBJECTIVE_HAVE_FUN &&
		objectiveType != OBJECTIVE_10_ROLLERCOASTERS &&
		objectiveType != OBJECTIVE_GUESTS_AND_RATING &&
		objectiveType != OBJECTIVE_10_ROLLERCOASTERS_LENGTH &&
		objectiveType != OBJECTIVE_FINISH_5_ROLLERCOASTERS
	) || (
		// The park must be free for the monthly ride income objective
		!(parkFlags & PARK_FLAGS_PARK_FREE_ENTRY) &&
		objectiveType == OBJECTIVE_MONTHLY_RIDE_INCOME
	)) {
		// Reset objective
		window_editor_objective_options_set_objective(w, OBJECTIVE_GUESTS_AND_RATING);
	}
}

/**
 *
 *  rct2: 0x00671A73
 */
static void window_editor_objective_options_main_textinput()
{
	rct_s6_info *s6Info = (rct_s6_info*)0x00141F570;
	uint8 result;
	short widgetIndex;
	rct_window *w;
	char *text;

	window_textinput_get_registers(w, widgetIndex, result, text);

	if (!result)
		return;

	switch (widgetIndex) {
	case WIDX_PARK_NAME:
		park_set_name(text);

		if (s6Info->name[0] == 0)
			format_string(s6Info->name, RCT2_GLOBAL(RCT2_ADDRESS_PARK_NAME, rct_string_id), (void*)RCT2_ADDRESS_PARK_NAME_ARGS);
		break;
	case WIDX_SCENARIO_NAME:
		strncpy(s6Info->name, text, 64);
		window_invalidate(w);
		break;
	case WIDX_DETAILS:
		strncpy(s6Info->details, text, 256);
		window_invalidate(w);
		break;
	}
}

/**
 *
 *  rct2: 0x0067161C
 */
static void window_editor_objective_options_main_invalidate()
{
	rct_window *w;
	rct_widget *widgets;
	rct_stex_entry *stex;

	window_get_register(w);

	stex = RCT2_GLOBAL(0x009ADAE4, rct_stex_entry*);
	if (stex == (rct_stex_entry*)0xFFFFFFFF)
		stex = NULL;

	widgets = window_editor_objective_options_widgets[w->page];
	if (w->widgets != widgets) {
		w->widgets = widgets;
		window_init_scroll_widgets(w);
	}

	window_editor_objective_options_set_pressed_tab(w);

	// This options was only available in development version
#ifdef DISABLE_SIX_FLAGS_CHECKBOX
	window_editor_objective_options_main_widgets[WIDX_SIX_FLAGS].type = WWT_EMPTY;
#endif

	if (RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & PARK_FLAGS_SIX_FLAGS)
		w->pressed_widgets |= (1 << WIDX_SIX_FLAGS);
	else
		w->pressed_widgets &= ~(1 << WIDX_SIX_FLAGS);

	if (stex == NULL)
		w->disabled_widgets &= ~(WIDX_PARK_NAME | WIDX_SCENARIO_NAME);
	else
		w->disabled_widgets |= (WIDX_PARK_NAME | WIDX_SCENARIO_NAME);

	switch (RCT2_GLOBAL(RCT2_ADDRESS_OBJECTIVE_TYPE, uint8)) {
	case OBJECTIVE_GUESTS_BY:
	case OBJECTIVE_PARK_VALUE_BY:
		window_editor_objective_options_main_widgets[WIDX_OBJECTIVE_ARG_1].type = WWT_SPINNER;
		window_editor_objective_options_main_widgets[WIDX_OBJECTIVE_ARG_1_INCREASE].type = WWT_DROPDOWN_BUTTON;
		window_editor_objective_options_main_widgets[WIDX_OBJECTIVE_ARG_1_DECREASE].type = WWT_DROPDOWN_BUTTON;
		window_editor_objective_options_main_widgets[WIDX_OBJECTIVE_ARG_2].type = WWT_SPINNER;
		window_editor_objective_options_main_widgets[WIDX_OBJECTIVE_ARG_2_INCREASE].type = WWT_DROPDOWN_BUTTON;
		window_editor_objective_options_main_widgets[WIDX_OBJECTIVE_ARG_2_DECREASE].type = WWT_DROPDOWN_BUTTON;
		break;
	case OBJECTIVE_GUESTS_AND_RATING:
	case OBJECTIVE_MONTHLY_RIDE_INCOME:
	case OBJECTIVE_10_ROLLERCOASTERS_LENGTH:
	case OBJECTIVE_FINISH_5_ROLLERCOASTERS:
	case OBJECTIVE_REPLAY_LOAN_AND_PARK_VALUE:
	case OBJECTIVE_MONTHLY_FOOD_INCOME:
		window_editor_objective_options_main_widgets[WIDX_OBJECTIVE_ARG_1].type = WWT_SPINNER;
		window_editor_objective_options_main_widgets[WIDX_OBJECTIVE_ARG_1_INCREASE].type = WWT_DROPDOWN_BUTTON;
		window_editor_objective_options_main_widgets[WIDX_OBJECTIVE_ARG_1_DECREASE].type = WWT_DROPDOWN_BUTTON;
		window_editor_objective_options_main_widgets[WIDX_OBJECTIVE_ARG_2].type = WWT_EMPTY;
		window_editor_objective_options_main_widgets[WIDX_OBJECTIVE_ARG_2_INCREASE].type = WWT_EMPTY;
		window_editor_objective_options_main_widgets[WIDX_OBJECTIVE_ARG_2_DECREASE].type = WWT_EMPTY;
		break;
	default:
		window_editor_objective_options_main_widgets[WIDX_OBJECTIVE_ARG_1].type = WWT_EMPTY;
		window_editor_objective_options_main_widgets[WIDX_OBJECTIVE_ARG_1_INCREASE].type = WWT_EMPTY;
		window_editor_objective_options_main_widgets[WIDX_OBJECTIVE_ARG_1_DECREASE].type = WWT_EMPTY;
		window_editor_objective_options_main_widgets[WIDX_OBJECTIVE_ARG_2].type = WWT_EMPTY;
		window_editor_objective_options_main_widgets[WIDX_OBJECTIVE_ARG_2_INCREASE].type = WWT_EMPTY;
		window_editor_objective_options_main_widgets[WIDX_OBJECTIVE_ARG_2_DECREASE].type = WWT_EMPTY;
		break;
	}

	window_editor_objective_options_main_widgets[WIDX_CLOSE].type =
		RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_SCENARIO_EDITOR ? WWT_EMPTY : WWT_CLOSEBOX;

	window_editor_objective_options_anchor_border_widgets(w);
}

/**
 *
 *  rct2: 0x0067161C
 */
static void window_editor_objective_options_main_paint()
{
	rct_s6_info *s6Info = (rct_s6_info*)0x00141F570;
	rct_stex_entry *stex;
	rct_window *w;
	rct_drawpixelinfo *dpi;
	int x, y, width;
	rct_string_id stringId;
	uint32 arg;

	window_paint_get_registers(w, dpi);

	window_draw_widgets(w, dpi);
	window_editor_objective_options_draw_tab_images(w, dpi);

	stex = RCT2_GLOBAL(0x009ADAE4, rct_stex_entry*);
	if (stex == (rct_stex_entry*)0xFFFFFFFF)
		stex = NULL;

	// Objective label
	x = w->x + 8;
	y = w->y + w->widgets[WIDX_OBJECTIVE].top;
	gfx_draw_string_left(dpi, 3287, NULL, 0, x, y);

	// Objective value
	x = w->x + w->widgets[WIDX_OBJECTIVE].left + 1;
	y = w->y + w->widgets[WIDX_OBJECTIVE].top;
	stringId = STR_OBJECTIVE_2_NONE + RCT2_GLOBAL(RCT2_ADDRESS_OBJECTIVE_TYPE, uint8);
	gfx_draw_string_left(dpi, 1193, &stringId, 0, x, y);

	if (w->widgets[WIDX_OBJECTIVE_ARG_1].type != WWT_EMPTY) {
		// Objective argument 1 label
		x = w->x + 28;
		y = w->y + w->widgets[WIDX_OBJECTIVE_ARG_1].top;
		switch (RCT2_GLOBAL(RCT2_ADDRESS_OBJECTIVE_TYPE, uint8)) {
		case OBJECTIVE_GUESTS_BY:
		case OBJECTIVE_GUESTS_AND_RATING:
			stringId = 3303;
			break;
		case OBJECTIVE_PARK_VALUE_BY:
		case OBJECTIVE_REPLAY_LOAN_AND_PARK_VALUE:
			stringId = 3304;
			break;
		case OBJECTIVE_MONTHLY_RIDE_INCOME:
			stringId = 3305;
			break;
		case OBJECTIVE_MONTHLY_FOOD_INCOME:
			stringId = 3306;
			break;
		case OBJECTIVE_10_ROLLERCOASTERS_LENGTH:
			stringId = 3307;
			break;
		default:
			stringId = 3308;
			break;
		}
		gfx_draw_string_left(dpi, stringId, NULL, 0, x, y);

		// Objective argument 1 value
		x = w->x + w->widgets[WIDX_OBJECTIVE_ARG_1].left + 1;
		y = w->y + w->widgets[WIDX_OBJECTIVE_ARG_1].top;
		switch (RCT2_GLOBAL(RCT2_ADDRESS_OBJECTIVE_TYPE, uint8)) {
		case OBJECTIVE_GUESTS_BY:
		case OBJECTIVE_GUESTS_AND_RATING:
			stringId = 3309;
			arg = RCT2_GLOBAL(RCT2_ADDRESS_OBJECTIVE_NUM_GUESTS, uint16);
			break;
		case OBJECTIVE_PARK_VALUE_BY:
		case OBJECTIVE_REPLAY_LOAN_AND_PARK_VALUE:
		case OBJECTIVE_MONTHLY_RIDE_INCOME:
		case OBJECTIVE_MONTHLY_FOOD_INCOME:
			stringId = 3246;
			arg = RCT2_GLOBAL(RCT2_ADDRESS_OBJECTIVE_CURRENCY, money32);
			break;
		case OBJECTIVE_10_ROLLERCOASTERS_LENGTH:
			stringId = 3310;
			arg = RCT2_GLOBAL(RCT2_ADDRESS_OBJECTIVE_NUM_GUESTS, uint16);
			break;
		default:
			stringId = 3311;
			arg = RCT2_GLOBAL(RCT2_ADDRESS_OBJECTIVE_CURRENCY, money32);
			break;
		}
		gfx_draw_string_left(dpi, stringId, &arg, 0, x, y);
	}

	if (w->widgets[WIDX_OBJECTIVE_ARG_2].type != WWT_EMPTY) {
		// Objective argument 2 label
		x = w->x + 28;
		y = w->y + w->widgets[WIDX_OBJECTIVE_ARG_2].top;
		gfx_draw_string_left(dpi, 3301, NULL, 0, x, y);

		// Objective argument 2 value
		x = w->x + w->widgets[WIDX_OBJECTIVE_ARG_2].left + 1;
		y = w->y + w->widgets[WIDX_OBJECTIVE_ARG_2].top;
		arg = (RCT2_GLOBAL(RCT2_ADDRESS_OBJECTIVE_YEAR, uint8) * MONTH_COUNT) - 1;
		gfx_draw_string_left(dpi, 3302, &arg, 0, x, y);
	}

	// Climate label
	x = w->x + 8;
	y = w->y + w->widgets[WIDX_CLIMATE].top;
	gfx_draw_string_left(dpi, 3289, NULL, 0, x, y);

	// Climate value
	x = w->x + w->widgets[WIDX_CLIMATE].left + 1;
	y = w->y + w->widgets[WIDX_CLIMATE].top;
	stringId = STR_CLIMATE_COOL_AND_WET + RCT2_GLOBAL(RCT2_ADDRESS_CLIMATE, uint8);
	gfx_draw_string_left(dpi, 1193, &stringId, 0, x, y);

	// Park name
	x = w->x + 8;
	y = w->y + w->widgets[WIDX_PARK_NAME].top;
	width = w->widgets[WIDX_PARK_NAME].left - 16;

	if (stex != NULL) {
		RCT2_GLOBAL(0x013CE952 + 0, uint16) = stex->park_name;
	} else {
		RCT2_GLOBAL(0x013CE952 + 0, uint16) = RCT2_GLOBAL(RCT2_ADDRESS_PARK_NAME, rct_string_id);
	}
	RCT2_GLOBAL(0x013CE952 + 2, uint32) = RCT2_GLOBAL(0x0013573D8, uint32);
	gfx_draw_string_left_clipped(dpi, 3298, (void*)0x013CE952, 0, x, y, width);

	// Scenario name
	x = w->x + 8;
	y = w->y + w->widgets[WIDX_SCENARIO_NAME].top;
	width = w->widgets[WIDX_SCENARIO_NAME].left - 16;

	if (stex != NULL) {
		RCT2_GLOBAL(0x013CE952 + 0, uint16) = stex->scenario_name;
	} else {
		strcpy((char*)0x009BC677, s6Info->name);
		RCT2_GLOBAL(0x013CE952 + 0, uint16) = 3165;
	}
	RCT2_GLOBAL(0x013CE952 + 2, uint32) = RCT2_GLOBAL(0x0013573D8, uint32);
	gfx_draw_string_left_clipped(dpi, 3300, (void*)0x013CE952, 0, x, y, width);

	// Scenario details label
	x = w->x + 8;
	y = w->y + w->widgets[WIDX_DETAILS].top;
	gfx_draw_string_left(dpi, 3299, NULL, 0, x, y);

	// Scenario details value
	x = w->x + 16;
	y = w->y + w->widgets[WIDX_DETAILS].top + 10;
	width = w->widgets[WIDX_DETAILS].left - 4;

	if (stex != NULL) {
		RCT2_GLOBAL(0x013CE952 + 0, uint16) = stex->details;
	} else {
		strcpy((char*)0x009BC677, s6Info->details);
		RCT2_GLOBAL(0x013CE952 + 0, uint16) = 3165;
	}
	RCT2_GLOBAL(0x013CE952 + 2, uint32) = RCT2_GLOBAL(0x0013573D8, uint32);
	gfx_draw_string_left_wrapped(dpi, (void*)0x013CE952, x, y, width, 1191, 0);

	// Scenario category label
	x = w->x + 8;
	y = w->y + w->widgets[WIDX_CATEGORY].top;
	gfx_draw_string_left(dpi, 3319, NULL, 0, x, y);

	// Scenario category value
	x = w->x + w->widgets[WIDX_CATEGORY].left + 1;
	y = w->y + w->widgets[WIDX_CATEGORY].top;
	stringId = STR_BEGINNER_PARKS + s6Info->category;
	gfx_draw_string_left(dpi, 1193, &stringId, 0, x, y);
}

/**
 *
 *  rct2: 0x006724A4
 */
static void window_editor_objective_options_rides_mouseup()
{
	rct_s6_info *s6Info = (rct_s6_info*)0x00141F570;
	short widgetIndex;
	rct_window *w;

	window_widget_get_registers(w, widgetIndex);

	switch (widgetIndex) {
	case WIDX_CLOSE:
		window_close(w);
		break;
	case WIDX_TAB_1:
	case WIDX_TAB_2:
		window_editor_objective_options_set_page(w, widgetIndex - WIDX_TAB_1);
		break;
	}
}

/**
 *
 *  rct2: 0x006725A8
 */
static void window_editor_objective_options_rides_resize()
{
	rct_window *w;

	window_get_register(w);

	window_set_resize(w, 380, 224, 380, 224);
}

/**
 *
 *  rct2: 0x00672544
 */
static void window_editor_objective_options_rides_update(rct_window *w)
{
	int i, numItems;
	rct_ride *ride;

	w->frame_no++;
	window_event_invalidate_call(w);
	window_event_resize_call(w);
	widget_invalidate(w, WIDX_TAB_2);

	numItems = 0;
	FOR_ALL_RIDES(i, ride) {
		if (gRideClassifications[ride->type] == RIDE_CLASS_RIDE) {
			w->list_item_positions[numItems] = i;
			numItems++;
		}
	}

	if (w->no_list_items != numItems) {
		w->no_list_items = numItems;
		window_invalidate(w);
	}
}

/**
 *
 *  rct2: 0x006724BF
 */
static void window_editor_objective_options_rides_scrollgetheight()
{
	int width, height;
	rct_window *w;

	window_get_register(w);

	width = 0;
	height = w->no_list_items * 12;

	window_scrollsize_set_registers(width, height);
}

/**
 *
 *  rct2: 0x006724FC
 */
static void window_editor_objective_options_rides_scrollmousedown()
{
	rct_ride *ride;
	rct_window *w;
	short x, y, scrollIndex;
	int i;

	window_scrollmouse_get_registers(w, scrollIndex, x, y);

	i = y / 12;
	if (i < 0 || i >= w->no_list_items)
		return;

	ride = GET_RIDE(i);
	ride->lifecycle_flags ^= RIDE_LIFECYCLE_INDESTRUCTIBLE;
	window_invalidate(w);
}

/**
 *
 *  rct2: 0x006724CC
 */
static void window_editor_objective_options_rides_scrollmouseover()
{
	rct_window *w;
	short x, y, scrollIndex;
	int i;

	window_scrollmouse_get_registers(w, scrollIndex, x, y);

	i = y / 12;
	if (i < 0 || i >= w->no_list_items)
		return;

	if (w->selected_list_item != i) {
		w->selected_list_item = i;
		window_invalidate(w);
	}
}

/**
 *
 *  rct2: 0x006722B5
 */
static void window_editor_objective_options_rides_invalidate()
{
	rct_window *w;
	rct_widget *widgets;

	window_get_register(w);

	widgets = window_editor_objective_options_widgets[w->page];
	if (w->widgets != widgets) {
		w->widgets = widgets;
		window_init_scroll_widgets(w);
	}

	window_editor_objective_options_set_pressed_tab(w);

	window_editor_objective_options_main_widgets[WIDX_CLOSE].type =
		RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_SCENARIO_EDITOR ? WWT_EMPTY : WWT_CLOSEBOX;

	window_editor_objective_options_anchor_border_widgets(w);
}

/**
 *
 *  rct2: 0x00672340
 */
static void window_editor_objective_options_rides_paint()
{
	rct_window *w;
	rct_drawpixelinfo *dpi;

	window_paint_get_registers(w, dpi);

	window_draw_widgets(w, dpi);
	window_editor_objective_options_draw_tab_images(w, dpi);

	gfx_draw_string_left(dpi, 3312, NULL, 0, w->x + 6, w->y + w->widgets[WIDX_PAGE_BACKGROUND].top + 3);
}

/**
 *
 *  rct2: 0x0067236F
 */
static void window_editor_objective_options_rides_scrollpaint()
{
	int i, y, colour;
	rct_string_id stringId;
	rct_window *w;
	rct_drawpixelinfo *dpi;
	rct_ride *ride;

	window_paint_get_registers(w, dpi);

	colour = RCT2_GLOBAL(0x0141FC48 + (w->colours[1] * 8), uint8);
	gfx_fill_rect(dpi, dpi->x, dpi->y, dpi->x + dpi->width - 1, dpi->y + dpi->height - 1, colour);

	for (i = 0; i < w->no_list_items; i++) {
		y = i * 12;

		if (y + 12 < dpi->y || y >= dpi->y + dpi->height)
			continue;

		// Checkbox
		gfx_fill_rect_inset(dpi, 2, y, 11, y + 10, w->colours[1], 224);

		// Highlighted
		if (i == w->selected_list_item) {
			stringId = 1193;
			gfx_fill_rect(dpi, 0, y, w->width, y + 11, 0x2000031);
		} else {
			stringId = 1191;
		}

		// Checkbox mark
		ride = GET_RIDE(i);
		if (ride->lifecycle_flags & RIDE_LIFECYCLE_INDESTRUCTIBLE) {
			RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_FONT_SPRITE_BASE, uint16) = stringId == 1193 ? 0xFFFE : 0xFFFF;
			gfx_draw_string(dpi, (char*)0x009DED72, w->colours[1] & 0x7F, 2, y);
		}

		// Ride name
		gfx_draw_string_left(dpi, stringId, &ride->name, 0, 15, y);
	}
}