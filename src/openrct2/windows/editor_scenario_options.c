#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#include "../editor.h"
#include "../game.h"
#include "../interface/widget.h"
#include "../interface/window.h"
#include "../localisation/localisation.h"
#include "../management/finance.h"
#include "../rct2.h"
#include "../sprites.h"
#include "dropdown.h"
#include "error.h"

#pragma region Widgets

enum {
	WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_FINANCIAL,
	WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_GUESTS,
	WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_PARK,
	WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_COUNT
};

enum {
	WIDX_BACKGROUND,
	WIDX_TITLE,
	WIDX_CLOSE,
	WIDX_PAGE_BACKGROUND,
	WIDX_TAB_1,
	WIDX_TAB_2,
	WIDX_TAB_3,

	WIDX_NO_MONEY = 7,
	WIDX_INITIAL_CASH,
	WIDX_INITIAL_CASH_INCREASE,
	WIDX_INITIAL_CASH_DECREASE,
	WIDX_INITIAL_LOAN,
	WIDX_INITIAL_LOAN_INCREASE,
	WIDX_INITIAL_LOAN_DECREASE,
	WIDX_MAXIMUM_LOAN,
	WIDX_MAXIMUM_LOAN_INCREASE,
	WIDX_MAXIMUM_LOAN_DECREASE,
	WIDX_INTEREST_RATE,
	WIDX_INTEREST_RATE_INCREASE,
	WIDX_INTEREST_RATE_DECREASE,
	WIDX_FORBID_MARKETING,

	WIDX_CASH_PER_GUEST = 7,
	WIDX_CASH_PER_GUEST_INCREASE,
	WIDX_CASH_PER_GUEST_DECREASE,
	WIDX_GUEST_INITIAL_HAPPINESS,
	WIDX_GUEST_INITIAL_HAPPINESS_INCREASE,
	WIDX_GUEST_INITIAL_HAPPINESS_DECREASE,
	WIDX_GUEST_INITIAL_HUNGER,
	WIDX_GUEST_INITIAL_HUNGER_INCREASE,
	WIDX_GUEST_INITIAL_HUNGER_DECREASE,
	WIDX_GUEST_INITIAL_THIRST,
	WIDX_GUEST_INITIAL_THIRST_INCREASE,
	WIDX_GUEST_INITIAL_THIRST_DECREASE,
	WIDX_GUEST_PREFER_LESS_INTENSE_RIDES,
	WIDX_GUEST_PREFER_MORE_INTENSE_RIDES,

	WIDX_LAND_COST = 7,
	WIDX_LAND_COST_INCREASE,
	WIDX_LAND_COST_DECREASE,
	WIDX_CONSTRUCTION_RIGHTS_COST,
	WIDX_CONSTRUCTION_RIGHTS_COST_INCREASE,
	WIDX_CONSTRUCTION_RIGHTS_COST_DECREASE,
	WIDX_PAY_FOR_PARK_OR_RIDES,
	WIDX_PAY_FOR_PARK_OR_RIDES_DROPDOWN,
	WIDX_ENTRY_PRICE,
	WIDX_ENTRY_PRICE_INCREASE,
	WIDX_ENTRY_PRICE_DECREASE,
	WIDX_FORBID_TREE_REMOVAL,
	WIDX_FORBID_LANDSCAPE_CHANGES,
	WIDX_FORBID_HIGH_CONSTRUCTION,
	WIDX_HARD_PARK_RATING,
	WIDX_HARD_GUEST_GENERATION
};

static rct_widget window_editor_scenario_options_financial_widgets[] = {
	{ WWT_FRAME,			0,	0,		279,	0,		148,	STR_NONE,								STR_NONE									},
	{ WWT_CAPTION,			0,	1,		278,	1,		14,		STR_SCENARIO_OPTIONS_FINANCIAL,			STR_WINDOW_TITLE_TIP						},
	{ WWT_CLOSEBOX,			0,	267,	277,	2,		13,		STR_CLOSE_X,							STR_CLOSE_WINDOW_TIP						},
	{ WWT_RESIZE,			1,	0,		279,	43,		148,	STR_NONE,								STR_NONE									},
	{ WWT_TAB,				1,	3,		33,		17,		43,		0x20000000 | SPR_TAB,					STR_SCENARIO_OPTIONS_FINANCIAL_TIP			},
	{ WWT_TAB,				1,	34,		64,		17,		46,		0x20000000 | SPR_TAB,					STR_SCENARIO_OPTIONS_GUESTS_TIP				},
	{ WWT_TAB,				1,	65,		95,		17,		43,		0x20000000 | SPR_TAB,					STR_SCENARIO_OPTIONS_PARK_TIP				},

	{ WWT_CHECKBOX,			1,	8,		271,	48,		59,		STR_MAKE_PARK_NO_MONEY,					STR_MAKE_PARK_NO_MONEY_TIP					},
	{ WWT_SPINNER,			1,	168,	267,	65,		76,		STR_NONE,								STR_NONE									},
	{ WWT_DROPDOWN_BUTTON,	1,	256,	266,	66,		70,		STR_NUMERIC_UP,							STR_NONE									},
	{ WWT_DROPDOWN_BUTTON,	1,	256,	266,	71,		75,		STR_NUMERIC_DOWN,						STR_NONE									},
	{ WWT_SPINNER,			1,	168,	267,	82,		93,		STR_NONE,								STR_NONE									},
	{ WWT_DROPDOWN_BUTTON,	1,	256,	266,	83,		87,		STR_NUMERIC_UP,							STR_NONE									},
	{ WWT_DROPDOWN_BUTTON,	1,	256,	266,	88,		92,		STR_NUMERIC_DOWN,						STR_NONE									},
	{ WWT_SPINNER,			1,	168,	267,	99,		110,	STR_NONE,								STR_NONE									},
	{ WWT_DROPDOWN_BUTTON,	1,	256,	266,	100,	104,	STR_NUMERIC_UP,							STR_NONE									},
	{ WWT_DROPDOWN_BUTTON,	1,	256,	266,	105,	109,	STR_NUMERIC_DOWN,						STR_NONE									},
	{ WWT_SPINNER,			1,	168,	237,	116,	127,	STR_NONE,								STR_NONE									},
	{ WWT_DROPDOWN_BUTTON,	1,	226,	236,	117,	121,	STR_NUMERIC_UP,							STR_NONE									},
	{ WWT_DROPDOWN_BUTTON,	1,	226,	236,	122,	126,	STR_NUMERIC_DOWN,						STR_NONE									},
	{ WWT_CHECKBOX,			1,	8,		271,	133,	144,	STR_FORBID_MARKETING,					STR_FORBID_MARKETING_TIP					},
	{ WIDGETS_END }
};

static rct_widget window_editor_scenario_options_guests_widgets[] = {
	{ WWT_FRAME,			0,	0,		279,	0,		148,	STR_NONE,								STR_NONE									},
	{ WWT_CAPTION,			0,	1,		278,	1,		14,		STR_SCENARIO_OPTIONS_GUESTS,			STR_WINDOW_TITLE_TIP						},
	{ WWT_CLOSEBOX,			0,	267,	277,	2,		13,		STR_CLOSE_X,							STR_CLOSE_WINDOW_TIP						},
	{ WWT_RESIZE,			1,	0,		279,	43,		148,	STR_NONE,								STR_NONE									},
	{ WWT_TAB,				1,	3,		33,		17,		43,		0x20000000 | SPR_TAB,					STR_SCENARIO_OPTIONS_FINANCIAL_TIP			},
	{ WWT_TAB,				1,	34,		64,		17,		46,		0x20000000 | SPR_TAB,					STR_SCENARIO_OPTIONS_GUESTS_TIP				},
	{ WWT_TAB,				1,	65,		95,		17,		43,		0x20000000 | SPR_TAB,					STR_SCENARIO_OPTIONS_PARK_TIP				},

	{ WWT_SPINNER,			1,	268,	337,	48,		59,		STR_NONE,								STR_NONE									},
	{ WWT_DROPDOWN_BUTTON,	1,	326,	336,	49,		53,		STR_NUMERIC_UP,							STR_NONE									},
	{ WWT_DROPDOWN_BUTTON,	1,	326,	336,	54,		58,		STR_NUMERIC_DOWN,						STR_NONE									},
	{ WWT_SPINNER,			1,	268,	337,	65,		76,		STR_NONE,								STR_NONE									},
	{ WWT_DROPDOWN_BUTTON,	1,	326,	336,	66,		70,		STR_NUMERIC_UP,							STR_NONE									},
	{ WWT_DROPDOWN_BUTTON,	1,	326,	336,	71,		75,		STR_NUMERIC_DOWN,						STR_NONE									},
	{ WWT_SPINNER,			1,	268,	337,	82,		93,		STR_NONE,								STR_NONE									},
	{ WWT_DROPDOWN_BUTTON,	1,	326,	336,	83,		87,		STR_NUMERIC_UP,							STR_NONE									},
	{ WWT_DROPDOWN_BUTTON,	1,	326,	336,	88,		92,		STR_NUMERIC_DOWN,						STR_NONE									},
	{ WWT_SPINNER,			1,	268,	337,	99,		110,	STR_NONE,								STR_NONE									},
	{ WWT_DROPDOWN_BUTTON,	1,	326,	336,	100,	104,	STR_NUMERIC_UP,							STR_NONE									},
	{ WWT_DROPDOWN_BUTTON,	1,	326,	336,	105,	109,	STR_NUMERIC_DOWN,						STR_NONE									},
	{ WWT_CHECKBOX,			1,	8,		371,	116,	127,	STR_GUESTS_PREFER_LESS_INTENSE_RIDES,	STR_GUESTS_PREFER_LESS_INTENSE_RIDES_TIP	},
	{ WWT_CHECKBOX,			1,	8,		371,	133,	144,	STR_GUESTS_PREFER_MORE_INTENSE_RIDES,	STR_GUESTS_PREFER_MORE_INTENSE_RIDES_TIP	},
	{ WIDGETS_END }
};

static rct_widget window_editor_scenario_options_park_widgets[] = {
	{ WWT_FRAME,			0,	0,		279,	0,		148,	STR_NONE,								STR_NONE									},
	{ WWT_CAPTION,			0,	1,		278,	1,		14,		STR_SCENARIO_OPTIONS_PARK,				STR_WINDOW_TITLE_TIP						},
	{ WWT_CLOSEBOX,			0,	267,	277,	2,		13,		STR_CLOSE_X,							STR_CLOSE_WINDOW_TIP						},
	{ WWT_RESIZE,			1,	0,		279,	43,		148,	STR_NONE,								STR_NONE									},
	{ WWT_TAB,				1,	3,		33,		17,		43,		0x20000000 | SPR_TAB,					STR_SCENARIO_OPTIONS_FINANCIAL_TIP			},
	{ WWT_TAB,				1,	34,		64,		17,		46,		0x20000000 | SPR_TAB,					STR_SCENARIO_OPTIONS_GUESTS_TIP				},
	{ WWT_TAB,				1,	65,		95,		17,		43,		0x20000000 | SPR_TAB,					STR_SCENARIO_OPTIONS_PARK_TIP				},

	{ WWT_SPINNER,			1,	188,	257,	48,		59,		STR_NONE,								STR_NONE									},
	{ WWT_DROPDOWN_BUTTON,	1,	246,	256,	49,		53,		STR_NUMERIC_UP,							STR_NONE									},
	{ WWT_DROPDOWN_BUTTON,	1,	246,	256,	54,		58,		STR_NUMERIC_DOWN,						STR_NONE									},
	{ WWT_SPINNER,			1,	188,	257,	65,		76,		STR_NONE,								STR_NONE									},
	{ WWT_DROPDOWN_BUTTON,	1,	246,	256,	66,		70,		STR_NUMERIC_UP,							STR_NONE									},
	{ WWT_DROPDOWN_BUTTON,	1,	246,	256,	71,		75,		STR_NUMERIC_DOWN,						STR_NONE									},
	{ WWT_DROPDOWN,			1,	8,		217,	82,		93,		STR_NONE,								STR_PAY_FOR_PARK_PAY_FOR_RIDES_TIP			},
	{ WWT_DROPDOWN_BUTTON,	1,	206,	216,	83,		92,		STR_DROPDOWN_GLYPH,						STR_PAY_FOR_PARK_PAY_FOR_RIDES_TIP			},
	{ WWT_SPINNER,			1,	328,	397,	82,		93,		STR_NONE,								STR_NONE									},
	{ WWT_DROPDOWN_BUTTON,	1,	386,	396,	83,		87,		STR_NUMERIC_UP,							STR_NONE									},
	{ WWT_DROPDOWN_BUTTON,	1,	386,	396,	88,		92,		STR_NUMERIC_DOWN,						STR_NONE									},
	{ WWT_CHECKBOX,			1,	8,		391,	99,		110,	STR_FORBID_TREE_REMOVAL,				STR_FORBID_TREE_REMOVAL_TIP					},
	{ WWT_CHECKBOX,			1,	8,		391,	116,	127,	STR_FORBID_LANDSCAPE_CHANGES,			STR_FORBID_LANDSCAPE_CHANGES_TIP			},
	{ WWT_CHECKBOX,			1,	8,		391,	133,	144,	STR_FORBID_HIGH_CONSTRUCTION,			STR_FORBID_HIGH_CONSTRUCTION_TIP			},
	{ WWT_CHECKBOX,			1,	8,		391,	150,	161,	STR_HARD_PARK_RATING,					STR_HARD_PARK_RATING_TIP					},
	{ WWT_CHECKBOX,			1,	8,		391,	167,	178,	STR_HARD_GUEST_GENERATION,				STR_HARD_GUEST_GENERATION_TIP				},
	{ WIDGETS_END }
};

static rct_widget *window_editor_scenario_options_widgets[] = {
	window_editor_scenario_options_financial_widgets,
	window_editor_scenario_options_guests_widgets,
	window_editor_scenario_options_park_widgets
};

#pragma endregion

#pragma region Events

static void window_editor_scenario_options_financial_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_editor_scenario_options_financial_resize(rct_window *w);
static void window_editor_scenario_options_financial_mousedown(rct_widgetindex widgetIndex, rct_window *w, rct_widget *widget);
static void window_editor_scenario_options_financial_update(rct_window *w);
static void window_editor_scenario_options_financial_invalidate(rct_window *w);
static void window_editor_scenario_options_financial_paint(rct_window *w, rct_drawpixelinfo *dpi);

static void window_editor_scenario_options_guests_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_editor_scenario_options_guests_resize(rct_window *w);
static void window_editor_scenario_options_guests_mousedown(rct_widgetindex widgetIndex, rct_window *w, rct_widget *widget);
static void window_editor_scenario_options_guests_update(rct_window *w);
static void window_editor_scenario_options_guests_invalidate(rct_window *w);
static void window_editor_scenario_options_guests_paint(rct_window *w, rct_drawpixelinfo *dpi);

static void window_editor_scenario_options_park_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_editor_scenario_options_park_resize(rct_window *w);
static void window_editor_scenario_options_park_mousedown(rct_widgetindex widgetIndex, rct_window *w, rct_widget *widget);
static void window_editor_scenario_options_park_dropdown(rct_window *w, rct_widgetindex widgetIndex, sint32 dropdownIndex);
static void window_editor_scenario_options_park_update(rct_window *w);
static void window_editor_scenario_options_park_invalidate(rct_window *w);
static void window_editor_scenario_options_park_paint(rct_window *w, rct_drawpixelinfo *dpi);

// 0x0097EB60
static rct_window_event_list window_scenario_options_financial_events = {
	NULL,
	window_editor_scenario_options_financial_mouseup,
	window_editor_scenario_options_financial_resize,
	window_editor_scenario_options_financial_mousedown,
	NULL,
	NULL,
	window_editor_scenario_options_financial_update,
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
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	window_editor_scenario_options_financial_invalidate,
	window_editor_scenario_options_financial_paint,
	NULL
};

// 0x0097EBD0
static rct_window_event_list window_scenario_options_guests_events = {
	NULL,
	window_editor_scenario_options_guests_mouseup,
	window_editor_scenario_options_guests_resize,
	window_editor_scenario_options_guests_mousedown,
	NULL,
	NULL,
	window_editor_scenario_options_guests_update,
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
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	window_editor_scenario_options_guests_invalidate,
	window_editor_scenario_options_guests_paint,
	NULL
};

// 0x0097EC40
static rct_window_event_list window_scenario_options_park_events = {
	NULL,
	window_editor_scenario_options_park_mouseup,
	window_editor_scenario_options_park_resize,
	window_editor_scenario_options_park_mousedown,
	window_editor_scenario_options_park_dropdown,
	NULL,
	window_editor_scenario_options_park_update,
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
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	window_editor_scenario_options_park_invalidate,
	window_editor_scenario_options_park_paint,
	NULL
};

static rct_window_event_list *window_editor_scenario_options_page_events[] = {
	&window_scenario_options_financial_events,
	&window_scenario_options_guests_events,
	&window_scenario_options_park_events
};

#pragma endregion

#pragma region Enabled widgets

#define ALWAYS_ENABLED_WIDGETS \
	(1ULL << WIDX_CLOSE) |\
	(1ULL << WIDX_TAB_1) |\
	(1ULL << WIDX_TAB_2) |\
	(1ULL << WIDX_TAB_3)

static uint64 window_editor_scenario_options_page_enabled_widgets[] = {
	ALWAYS_ENABLED_WIDGETS |
		(1ULL << WIDX_NO_MONEY) |
		(1ULL << WIDX_INITIAL_CASH_INCREASE) |
		(1ULL << WIDX_INITIAL_CASH_DECREASE) |
		(1ULL << WIDX_INITIAL_LOAN_INCREASE) |
		(1ULL << WIDX_INITIAL_LOAN_DECREASE) |
		(1ULL << WIDX_MAXIMUM_LOAN_INCREASE) |
		(1ULL << WIDX_MAXIMUM_LOAN_DECREASE) |
		(1ULL << WIDX_INTEREST_RATE_INCREASE) |
		(1ULL << WIDX_INTEREST_RATE_DECREASE) |
		(1ULL << WIDX_FORBID_MARKETING),
	ALWAYS_ENABLED_WIDGETS |
		(1ULL << WIDX_CASH_PER_GUEST_INCREASE) |
		(1ULL << WIDX_CASH_PER_GUEST_DECREASE) |
		(1ULL << WIDX_GUEST_INITIAL_HAPPINESS_INCREASE) |
		(1ULL << WIDX_GUEST_INITIAL_HAPPINESS_DECREASE) |
		(1ULL << WIDX_GUEST_INITIAL_HUNGER_INCREASE) |
		(1ULL << WIDX_GUEST_INITIAL_HUNGER_DECREASE) |
		(1ULL << WIDX_GUEST_INITIAL_THIRST_INCREASE) |
		(1ULL << WIDX_GUEST_INITIAL_THIRST_DECREASE) |
		(1ULL << WIDX_GUEST_PREFER_LESS_INTENSE_RIDES) |
		(1ULL << WIDX_GUEST_PREFER_MORE_INTENSE_RIDES),
	ALWAYS_ENABLED_WIDGETS |
		(1ULL << WIDX_LAND_COST_INCREASE) |
		(1ULL << WIDX_LAND_COST_DECREASE) |
		(1ULL << WIDX_CONSTRUCTION_RIGHTS_COST_INCREASE) |
		(1ULL << WIDX_CONSTRUCTION_RIGHTS_COST_DECREASE) |
		(1ULL << WIDX_PAY_FOR_PARK_OR_RIDES) |
		(1ULL << WIDX_PAY_FOR_PARK_OR_RIDES_DROPDOWN) |
		(1ULL << WIDX_ENTRY_PRICE_INCREASE) |
		(1ULL << WIDX_ENTRY_PRICE_DECREASE) |
		(1ULL << WIDX_FORBID_TREE_REMOVAL) |
		(1ULL << WIDX_FORBID_LANDSCAPE_CHANGES) |
		(1ULL << WIDX_FORBID_HIGH_CONSTRUCTION) |
		(1ULL << WIDX_HARD_PARK_RATING) |
		(1ULL << WIDX_HARD_GUEST_GENERATION)
};

static uint32 window_editor_scenario_options_page_hold_down_widgets[] = {
	(1ULL << WIDX_INITIAL_CASH_INCREASE) |
		(1ULL << WIDX_INITIAL_CASH_DECREASE) |
		(1ULL << WIDX_INITIAL_LOAN_INCREASE) |
		(1ULL << WIDX_INITIAL_LOAN_DECREASE) |
		(1ULL << WIDX_MAXIMUM_LOAN_INCREASE) |
		(1ULL << WIDX_MAXIMUM_LOAN_DECREASE) |
		(1ULL << WIDX_INTEREST_RATE_INCREASE) |
		(1ULL << WIDX_INTEREST_RATE_DECREASE),
	(1ULL << WIDX_CASH_PER_GUEST_INCREASE) |
		(1ULL << WIDX_CASH_PER_GUEST_DECREASE) |
		(1ULL << WIDX_GUEST_INITIAL_HAPPINESS_INCREASE) |
		(1ULL << WIDX_GUEST_INITIAL_HAPPINESS_DECREASE) |
		(1ULL << WIDX_GUEST_INITIAL_HUNGER_INCREASE) |
		(1ULL << WIDX_GUEST_INITIAL_HUNGER_DECREASE) |
		(1ULL << WIDX_GUEST_INITIAL_THIRST_INCREASE) |
		(1ULL << WIDX_GUEST_INITIAL_THIRST_DECREASE),
	(1ULL << WIDX_LAND_COST_INCREASE) |
		(1ULL << WIDX_LAND_COST_DECREASE) |
		(1ULL << WIDX_CONSTRUCTION_RIGHTS_COST_INCREASE) |
		(1ULL << WIDX_CONSTRUCTION_RIGHTS_COST_DECREASE) |
		(1ULL << WIDX_ENTRY_PRICE_INCREASE) |
		(1ULL << WIDX_ENTRY_PRICE_DECREASE)
};

#pragma endregion

/**
 *
 *  rct2: 0x00670138
 */
void window_editor_scenario_options_open()
{
	rct_window *w;

	w = window_bring_to_front_by_class(WC_EDITOR_SCENARIO_OPTIONS);
	if (w != NULL)
		return;

	w = window_create_centred(
		280,
		148,
		window_editor_scenario_options_page_events[0],
		WC_EDITOR_SCENARIO_OPTIONS,
		WF_NO_SCROLLING
	);
	w->widgets = window_editor_scenario_options_widgets[0];
	w->enabled_widgets = window_editor_scenario_options_page_enabled_widgets[0];
	w->hold_down_widgets = window_editor_scenario_options_page_hold_down_widgets[0];
	window_init_scroll_widgets(w);
	w->var_4AE = 0;
	w->page = 0;
}

static void window_editor_scenario_options_set_pressed_tab(rct_window *w)
{
	sint32 i;
	for (i = 0; i < WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_COUNT; i++)
		w->pressed_widgets &= ~(1 << (WIDX_TAB_1 + i));
	w->pressed_widgets |= 1LL << (WIDX_TAB_1 + w->page);
}

static void window_editor_scenario_options_anchor_border_widgets(rct_window *w)
{
	w->widgets[WIDX_BACKGROUND].right = w->width - 1;
	w->widgets[WIDX_BACKGROUND].bottom = w->height - 1;
	w->widgets[WIDX_PAGE_BACKGROUND].right = w->width - 1;
	w->widgets[WIDX_PAGE_BACKGROUND].bottom = w->height - 1;
	w->widgets[WIDX_TITLE].right = w->width - 2;
	w->widgets[WIDX_CLOSE].left = w->width - 13;
	w->widgets[WIDX_CLOSE].right = w->width - 3;
}

/**
 *
 *  rct2: 0x006712E8
 */
static void window_editor_scenario_options_draw_tab_images(rct_window *w, rct_drawpixelinfo *dpi)
{
	rct_widget *widget;
	sint32 spriteIndex;

	// Tab 1
	widget = &w->widgets[WIDX_TAB_1];
	spriteIndex = SPR_TAB_FINANCES_SUMMARY_0;
	if (w->page == WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_FINANCIAL)
		spriteIndex += (w->frame_no / 2) % 8;

	gfx_draw_sprite(dpi, spriteIndex, w->x + widget->left, w->y + widget->top, 0);

	// Tab 2
	widget = &w->widgets[WIDX_TAB_2];
	spriteIndex = SPR_TAB_GUESTS_0;
	if (w->page == WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_GUESTS)
		spriteIndex += (w->frame_no / 4) % 8;

	gfx_draw_sprite(dpi, spriteIndex, w->x + widget->left, w->y + widget->top, 0);

	// Tab 3
	widget = &w->widgets[WIDX_TAB_3];
	spriteIndex = SPR_TAB_PARK;
	gfx_draw_sprite(dpi, spriteIndex, w->x + widget->left, w->y + widget->top, 0);
}

/**
 *
 *  rct2: 0x00668496
 */
static void window_editor_scenario_options_set_page(rct_window *w, sint32 page)
{
	if (w->page == page)
		return;

	w->page = page;
	w->frame_no = 0;
	w->var_492 = 0;
	w->enabled_widgets = window_editor_scenario_options_page_enabled_widgets[page];
	w->hold_down_widgets = window_editor_scenario_options_page_hold_down_widgets[page];
	w->event_handlers = window_editor_scenario_options_page_events[page];
	w->widgets = window_editor_scenario_options_widgets[page];
	window_invalidate(w);
	window_event_resize_call(w);
	window_event_invalidate_call(w);
	window_init_scroll_widgets(w);
	window_invalidate(w);
}

#pragma region Financial

/**
 *
 *  rct2: 0x0067049D
 */
static void window_editor_scenario_options_financial_mouseup(rct_window *w, rct_widgetindex widgetIndex)
{
	switch (widgetIndex) {
	case WIDX_CLOSE:
		window_close(w);
		break;
	case WIDX_TAB_1:
	case WIDX_TAB_2:
	case WIDX_TAB_3:
		window_editor_scenario_options_set_page(w, widgetIndex - WIDX_TAB_1);
		break;
	case WIDX_NO_MONEY:
		game_do_command(
			0, 
			GAME_COMMAND_FLAG_APPLY,
			EDIT_SCENARIOOPTIONS_SETNOMONEY,
			gParkFlags & PARK_FLAGS_NO_MONEY ? 0 : 1,
			GAME_COMMAND_EDIT_SCENARIO_OPTIONS,
			0,
			0
		);
		window_invalidate(w);
		break;
	case WIDX_FORBID_MARKETING:
		game_do_command(
			0,
			GAME_COMMAND_FLAG_APPLY,
			EDIT_SCENARIOOPTIONS_SETFORBIDMARKETINGCAMPAIGNS,
			gParkFlags & PARK_FLAGS_FORBID_MARKETING_CAMPAIGN ? 0 : 1,
			GAME_COMMAND_EDIT_SCENARIO_OPTIONS,
			0,
			0
		);
		window_invalidate(w);
		break;
	}
}

/**
 *
 *  rct2: 0x0067077A
 */
static void window_editor_scenario_options_financial_resize(rct_window *w)
{
	window_set_resize(w, 280, 149, 280, 149);
}

/**
 *
 *  rct2: 0x006704C8
 */
static void window_editor_scenario_options_financial_mousedown(rct_widgetindex widgetIndex, rct_window *w, rct_widget *widget)
{
	switch (widgetIndex) {
	case WIDX_INITIAL_CASH_INCREASE:
		if (gInitialCash < MONEY(1000000,00)) {
			game_do_command(
				0,
				GAME_COMMAND_FLAG_APPLY,
				EDIT_SCENARIOOPTIONS_SETINITIALCASH,
				gInitialCash + MONEY(500,00),
				GAME_COMMAND_EDIT_SCENARIO_OPTIONS,
				0,
				0
			);
		} else {
			window_error_open(STR_CANT_INCREASE_CASH, STR_NONE);
		}
		window_invalidate(w);
		break;
	case WIDX_INITIAL_CASH_DECREASE:
		if (gInitialCash > MONEY(0,00)) {
			game_do_command(
				0,
				GAME_COMMAND_FLAG_APPLY,
				EDIT_SCENARIOOPTIONS_SETINITIALCASH,
				gInitialCash - MONEY(500,00),
				GAME_COMMAND_EDIT_SCENARIO_OPTIONS,
				0,
				0
			);
		} else {
			window_error_open(STR_CANT_REDUCE_CASH, STR_NONE);
		}
		window_invalidate(w);
		break;
	case WIDX_INITIAL_LOAN_INCREASE:
		if (gBankLoan < MONEY(5000000,00)) {
			game_do_command(
				0,
				GAME_COMMAND_FLAG_APPLY,
				EDIT_SCENARIOOPTIONS_SETINITIALLOAN,
				gBankLoan + MONEY(1000,00),
				GAME_COMMAND_EDIT_SCENARIO_OPTIONS,
				0,
				0
			);
		} else {
			window_error_open(STR_CANT_INCREASE_INIT_LOAN, STR_NONE);
		}
		window_invalidate(w);
		break;
	case WIDX_INITIAL_LOAN_DECREASE:
		if (gBankLoan > MONEY(0,00)) {
			game_do_command(
				0,
				GAME_COMMAND_FLAG_APPLY,
				EDIT_SCENARIOOPTIONS_SETINITIALLOAN,
				gBankLoan - MONEY(1000,00),
				GAME_COMMAND_EDIT_SCENARIO_OPTIONS,
				0,
				0
			);
		} else {
			window_error_open(STR_CANT_REDUCE_INIT_LOAN, STR_NONE);
		}
		window_invalidate(w);
		break;
	case WIDX_MAXIMUM_LOAN_INCREASE:
		if (gMaxBankLoan < MONEY(5000000,00)) {
			game_do_command(
				0,
				GAME_COMMAND_FLAG_APPLY,
				EDIT_SCENARIOOPTIONS_SETMAXIMUMLOANSIZE,
				gMaxBankLoan + MONEY(1000,00),
				GAME_COMMAND_EDIT_SCENARIO_OPTIONS,
				0,
				0
			);
		} else {
			window_error_open(STR_CANT_INCREASE_MAX_LOAN, STR_NONE);
		}
		window_invalidate(w);
		break;
	case WIDX_MAXIMUM_LOAN_DECREASE:
		if (gMaxBankLoan > MONEY(0,00)) {
			game_do_command(
				0,
				GAME_COMMAND_FLAG_APPLY,
				EDIT_SCENARIOOPTIONS_SETMAXIMUMLOANSIZE,
				gMaxBankLoan - MONEY(1000,00),
				GAME_COMMAND_EDIT_SCENARIO_OPTIONS,
				0,
				0
			);
		} else {
			window_error_open(STR_CANT_REDUCE_MAX_LOAN, STR_NONE);
		}
		window_invalidate(w);
		break;
	case WIDX_INTEREST_RATE_INCREASE:
		if (gBankLoanInterestRate < 80) {
			if (gBankLoanInterestRate < 0) {
				game_do_command(
					0,
					GAME_COMMAND_FLAG_APPLY,
					EDIT_SCENARIOOPTIONS_SETANNUALINTERESTRATE,
					0,
					GAME_COMMAND_EDIT_SCENARIO_OPTIONS,
					0,
					0
				);
			} else {
				game_do_command(
					0,
					GAME_COMMAND_FLAG_APPLY,
					EDIT_SCENARIOOPTIONS_SETANNUALINTERESTRATE,
					gBankLoanInterestRate + 1,
					GAME_COMMAND_EDIT_SCENARIO_OPTIONS,
					0,
					0
				);
			}
		} else {
			window_error_open(STR_CANT_INCREASE_INTEREST_RATE, STR_NONE);
		}
		window_invalidate(w);
		break;
	case WIDX_INTEREST_RATE_DECREASE:
		if (gBankLoanInterestRate > 0) {
			if (gBankLoanInterestRate > 80) {
				game_do_command(
					0,
					GAME_COMMAND_FLAG_APPLY,
					EDIT_SCENARIOOPTIONS_SETANNUALINTERESTRATE,
					80,
					GAME_COMMAND_EDIT_SCENARIO_OPTIONS,
					0,
					0
				);
			} else {
				game_do_command(
					0,
					GAME_COMMAND_FLAG_APPLY,
					EDIT_SCENARIOOPTIONS_SETANNUALINTERESTRATE,
					gBankLoanInterestRate - 1,
					GAME_COMMAND_EDIT_SCENARIO_OPTIONS,
					0,
					0
				);
			}
		} else {
			window_error_open(STR_CANT_REDUCE_INTEREST_RATE, STR_NONE);
		}
		window_invalidate(w);
		break;
	}

	if(gScreenFlags == SCREEN_FLAGS_PLAYING) {
		window_invalidate_by_class(WC_FINANCES);
		window_invalidate_by_class(WC_BOTTOM_TOOLBAR);
	}
}

/**
 *
 *  rct2: 0x00670760
 */
static void window_editor_scenario_options_financial_update(rct_window *w)
{
	w->frame_no++;
	window_event_invalidate_call(w);
	widget_invalidate(w, WIDX_TAB_1);
}

/**
 *
 *  rct2: 0x006701CF
 */
static void window_editor_scenario_options_financial_invalidate(rct_window *w)
{
	rct_widget *widgets = window_editor_scenario_options_widgets[w->page];
	if (w->widgets != widgets) {
		w->widgets = widgets;
		window_init_scroll_widgets(w);
	}

	window_editor_scenario_options_set_pressed_tab(w);

	if (((gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) && (gParkFlags & PARK_FLAGS_NO_MONEY_SCENARIO)) ||
		(!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) && (gParkFlags & PARK_FLAGS_NO_MONEY))) {
		w->pressed_widgets |= (1 << WIDX_NO_MONEY);
		for (sint32 i = WIDX_INITIAL_CASH; i <= WIDX_FORBID_MARKETING; i++)
			w->widgets[i].type = WWT_EMPTY;
	} else {
		w->pressed_widgets &= ~(1 << WIDX_NO_MONEY);
		w->widgets[WIDX_INITIAL_CASH].type = WWT_SPINNER;
		w->widgets[WIDX_INITIAL_CASH_INCREASE].type = WWT_DROPDOWN_BUTTON;
		w->widgets[WIDX_INITIAL_CASH_DECREASE].type = WWT_DROPDOWN_BUTTON;
		w->widgets[WIDX_INITIAL_LOAN].type = WWT_SPINNER;
		w->widgets[WIDX_INITIAL_LOAN_INCREASE].type =  WWT_DROPDOWN_BUTTON;
		w->widgets[WIDX_INITIAL_LOAN_DECREASE].type =  WWT_DROPDOWN_BUTTON;
		w->widgets[WIDX_MAXIMUM_LOAN].type = WWT_SPINNER;
		w->widgets[WIDX_MAXIMUM_LOAN_INCREASE].type =  WWT_DROPDOWN_BUTTON;
		w->widgets[WIDX_MAXIMUM_LOAN_DECREASE].type =  WWT_DROPDOWN_BUTTON;
		w->widgets[WIDX_INTEREST_RATE].type = WWT_SPINNER;
		w->widgets[WIDX_INTEREST_RATE_INCREASE].type =  WWT_DROPDOWN_BUTTON;
		w->widgets[WIDX_INTEREST_RATE_DECREASE].type =  WWT_DROPDOWN_BUTTON;
		w->widgets[WIDX_FORBID_MARKETING].type = WWT_CHECKBOX;
	}

	if (gParkFlags & PARK_FLAGS_FORBID_MARKETING_CAMPAIGN)
		w->pressed_widgets |= (1 << WIDX_FORBID_MARKETING);
	else
		w->pressed_widgets &= ~(1 << WIDX_FORBID_MARKETING);

	w->widgets[WIDX_CLOSE].type = (gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) ? WWT_EMPTY : WWT_CLOSEBOX;

	window_editor_scenario_options_anchor_border_widgets(w);
}

/**
 *
 *  rct2: 0x00670338
 */
static void window_editor_scenario_options_financial_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
	sint32 x, y;

	window_draw_widgets(w, dpi);
	window_editor_scenario_options_draw_tab_images(w, dpi);

	if (w->widgets[WIDX_INITIAL_CASH].type != WWT_EMPTY) {
		x = w->x + 8;
		y = w->y + w->widgets[WIDX_INITIAL_CASH].top;
		gfx_draw_string_left(dpi, STR_INIT_CASH_LABEL, NULL, COLOUR_BLACK, x, y);

		x = w->x + w->widgets[WIDX_INITIAL_CASH].left + 1;
		y = w->y + w->widgets[WIDX_INITIAL_CASH].top;
		gfx_draw_string_left(dpi, STR_CURRENCY_FORMAT_LABEL, &gInitialCash, COLOUR_BLACK, x, y);
	}

	if (w->widgets[WIDX_INITIAL_LOAN].type != WWT_EMPTY) {
		x = w->x + 8;
		y = w->y + w->widgets[WIDX_INITIAL_LOAN].top;
		gfx_draw_string_left(dpi, STR_INIT_LOAN_LABEL, NULL, COLOUR_BLACK, x, y);

		x = w->x + w->widgets[WIDX_INITIAL_LOAN].left + 1;
		y = w->y + w->widgets[WIDX_INITIAL_LOAN].top;
		gfx_draw_string_left(dpi, STR_CURRENCY_FORMAT_LABEL, &gBankLoan, COLOUR_BLACK, x, y);
	}

	if (w->widgets[WIDX_MAXIMUM_LOAN].type != WWT_EMPTY) {
		x = w->x + 8;
		y = w->y + w->widgets[WIDX_MAXIMUM_LOAN].top;
		gfx_draw_string_left(dpi, STR_MAX_LOAN_LABEL, NULL, COLOUR_BLACK, x, y);

		x = w->x + w->widgets[WIDX_MAXIMUM_LOAN].left + 1;
		y = w->y + w->widgets[WIDX_MAXIMUM_LOAN].top;
		gfx_draw_string_left(dpi, STR_CURRENCY_FORMAT_LABEL, &gMaxBankLoan, COLOUR_BLACK, x, y);
	}

	if (w->widgets[WIDX_INTEREST_RATE].type != WWT_EMPTY) {
		x = w->x + 8;
		y = w->y + w->widgets[WIDX_INTEREST_RATE].top;
		gfx_draw_string_left(dpi, STR_INTEREST_RATE_LABEL, NULL, COLOUR_BLACK, x, y);

		x = w->x + w->widgets[WIDX_INTEREST_RATE].left + 1;
		y = w->y + w->widgets[WIDX_INTEREST_RATE].top;

		sint16 interestRate = clamp(INT16_MIN, (sint16)gBankLoanInterestRate, INT16_MAX);
		gfx_draw_string_left(dpi, STR_PERCENT_FORMAT_LABEL, &interestRate, COLOUR_BLACK, x, y);
	}
}

#pragma endregion

#pragma region Guests

/**
 *
 *  rct2: 0x00670A62
 */
static void window_editor_scenario_options_guests_mouseup(rct_window *w, rct_widgetindex widgetIndex)
{
	switch (widgetIndex) {
	case WIDX_CLOSE:
		window_close(w);
		break;
	case WIDX_TAB_1:
	case WIDX_TAB_2:
	case WIDX_TAB_3:
		window_editor_scenario_options_set_page(w, widgetIndex - WIDX_TAB_1);
		break;
	case WIDX_GUEST_PREFER_LESS_INTENSE_RIDES:
		game_do_command(
			0,
			GAME_COMMAND_FLAG_APPLY,
			EDIT_SCENARIOOPTIONS_SETGUESTSPREFERLESSINTENSERIDES,
			gParkFlags & PARK_FLAGS_PREF_LESS_INTENSE_RIDES ? 0 : 1,
			GAME_COMMAND_EDIT_SCENARIO_OPTIONS,
			0,
			0
		);
		window_invalidate(w);
		break;
	case WIDX_GUEST_PREFER_MORE_INTENSE_RIDES:
		game_do_command(
			0,
			GAME_COMMAND_FLAG_APPLY,
			EDIT_SCENARIOOPTIONS_SETGUESTSPREFERMOREINTENSERIDES,
			gParkFlags & PARK_FLAGS_PREF_MORE_INTENSE_RIDES ? 0 : 1,
			GAME_COMMAND_EDIT_SCENARIO_OPTIONS,
			0,
			0
		);
		window_invalidate(w);
		break;
	}
}

/**
 *
 *  rct2: 0x00670C59
 */
static void window_editor_scenario_options_guests_resize(rct_window *w)
{
	window_set_resize(w, 380, 149, 380, 149);
}

/**
 *
 *  rct2: 0x00670A89
 */
static void window_editor_scenario_options_guests_mousedown(rct_widgetindex widgetIndex, rct_window *w, rct_widget *widget)
{
	switch (widgetIndex) {
	case WIDX_CASH_PER_GUEST_INCREASE:
		if (gGuestInitialCash < MONEY(1000, 00)) {
			game_do_command(
				0,
				GAME_COMMAND_FLAG_APPLY,
				EDIT_SCENARIOOPTIONS_SETAVERAGECASHPERGUEST,
				gGuestInitialCash + MONEY(1, 00),
				GAME_COMMAND_EDIT_SCENARIO_OPTIONS,
				0,
				0
			);
		} else {
			window_error_open(STR_CANT_INCREASE_FURTHER, STR_NONE);
		}
		window_invalidate(w);
		break;
	case WIDX_CASH_PER_GUEST_DECREASE:
		if (gGuestInitialCash > MONEY(0, 00)) {
			game_do_command(
				0,
				GAME_COMMAND_FLAG_APPLY,
				EDIT_SCENARIOOPTIONS_SETAVERAGECASHPERGUEST,
				gGuestInitialCash - MONEY(1, 00),
				GAME_COMMAND_EDIT_SCENARIO_OPTIONS,
				0,
				0
			);
		} else {
			window_error_open(STR_CANT_REDUCE_FURTHER, STR_NONE);
		}
		window_invalidate(w);
		break;
	case WIDX_GUEST_INITIAL_HAPPINESS_INCREASE:
		if (gGuestInitialHappiness < 250) {
			game_do_command(
				0,
				GAME_COMMAND_FLAG_APPLY,
				EDIT_SCENARIOOPTIONS_SETGUESTINITIALHAPPINESS,
				gGuestInitialHappiness + 4,
				GAME_COMMAND_EDIT_SCENARIO_OPTIONS,
				0,
				0
			);
		} else {
			window_error_open(STR_CANT_INCREASE_FURTHER, STR_NONE);
		}
		window_invalidate(w);
		break;
	case WIDX_GUEST_INITIAL_HAPPINESS_DECREASE:
		if (gGuestInitialHappiness > 40) {
			game_do_command(
				0,
				GAME_COMMAND_FLAG_APPLY,
				EDIT_SCENARIOOPTIONS_SETGUESTINITIALHAPPINESS,
				gGuestInitialHappiness - 4,
				GAME_COMMAND_EDIT_SCENARIO_OPTIONS,
				0,
				0
			);
		} else {
			window_error_open(STR_CANT_REDUCE_FURTHER, STR_NONE);
		}
		window_invalidate(w);
		break;
	case WIDX_GUEST_INITIAL_HUNGER_INCREASE:
		if (gGuestInitialHunger > 40) {
			game_do_command(
				0,
				GAME_COMMAND_FLAG_APPLY,
				EDIT_SCENARIOOPTIONS_SETGUESTINITIALHUNGER,
				gGuestInitialHunger - 4,
				GAME_COMMAND_EDIT_SCENARIO_OPTIONS,
				0,
				0
			);
		} else {
			window_error_open(STR_CANT_INCREASE_FURTHER, STR_NONE);
		}
		window_invalidate(w);
		break;
	case WIDX_GUEST_INITIAL_HUNGER_DECREASE:
		if (gGuestInitialHunger < 250) {
			game_do_command(
				0,
				GAME_COMMAND_FLAG_APPLY,
				EDIT_SCENARIOOPTIONS_SETGUESTINITIALHUNGER,
				gGuestInitialHunger + 4,
				GAME_COMMAND_EDIT_SCENARIO_OPTIONS,
				0,
				0
			);
		} else {
			window_error_open(STR_CANT_REDUCE_FURTHER, STR_NONE);
		}
		window_invalidate(w);
		break;
	case WIDX_GUEST_INITIAL_THIRST_INCREASE:
		if (gGuestInitialThirst > 40) {
			game_do_command(
				0,
				GAME_COMMAND_FLAG_APPLY,
				EDIT_SCENARIOOPTIONS_SETGUESTINITIALTHIRST,
				gGuestInitialThirst - 4,
				GAME_COMMAND_EDIT_SCENARIO_OPTIONS,
				0,
				0
			);
		} else {
			window_error_open(STR_CANT_INCREASE_FURTHER, STR_NONE);
		}
		window_invalidate(w);
		break;
	case WIDX_GUEST_INITIAL_THIRST_DECREASE:
		if (gGuestInitialThirst < 250) {
			game_do_command(
				0,
				GAME_COMMAND_FLAG_APPLY,
				EDIT_SCENARIOOPTIONS_SETGUESTINITIALTHIRST,
				gGuestInitialThirst + 4,
				GAME_COMMAND_EDIT_SCENARIO_OPTIONS,
				0,
				0
			);
		} else {
			window_error_open(STR_CANT_REDUCE_FURTHER, STR_NONE);
		}
		window_invalidate(w);
		break;
	}
}

/**
 *
 *  rct2: 0x00670C3F
 */
static void window_editor_scenario_options_guests_update(rct_window *w)
{
	w->frame_no++;
	window_event_invalidate_call(w);
	widget_invalidate(w, WIDX_TAB_2);
}

/**
 *
 *  rct2: 0x006707DB
 */
static void window_editor_scenario_options_guests_invalidate(rct_window *w)
{
	rct_widget *widgets;

	widgets = window_editor_scenario_options_widgets[w->page];
	if (w->widgets != widgets) {
		w->widgets = widgets;
		window_init_scroll_widgets(w);
	}

	window_editor_scenario_options_set_pressed_tab(w);

	if (((gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) && (gParkFlags & PARK_FLAGS_NO_MONEY_SCENARIO)) ||
		(!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) && (gParkFlags & PARK_FLAGS_NO_MONEY))) {
		w->widgets[WIDX_CASH_PER_GUEST].type = WWT_EMPTY;
		w->widgets[WIDX_CASH_PER_GUEST_INCREASE].type = WWT_EMPTY;
		w->widgets[WIDX_CASH_PER_GUEST_DECREASE].type = WWT_EMPTY;
	} else {
		w->widgets[WIDX_CASH_PER_GUEST].type = WWT_SPINNER;
		w->widgets[WIDX_CASH_PER_GUEST_INCREASE].type = WWT_DROPDOWN_BUTTON;
		w->widgets[WIDX_CASH_PER_GUEST_DECREASE].type = WWT_DROPDOWN_BUTTON;
	}

	// Guests prefer less intense rides checkbox
	if (gParkFlags & PARK_FLAGS_PREF_LESS_INTENSE_RIDES)
		w->pressed_widgets |= (1 << WIDX_GUEST_PREFER_LESS_INTENSE_RIDES);
	else
		w->pressed_widgets &= ~(1 << WIDX_GUEST_PREFER_LESS_INTENSE_RIDES);

	// Guests prefer more intense rides checkbox
	if (gParkFlags & PARK_FLAGS_PREF_MORE_INTENSE_RIDES)
		w->pressed_widgets |= (1 << WIDX_GUEST_PREFER_MORE_INTENSE_RIDES);
	else
		w->pressed_widgets &= ~(1 << WIDX_GUEST_PREFER_MORE_INTENSE_RIDES);

	w->widgets[WIDX_CLOSE].type = (gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) ? WWT_EMPTY : WWT_CLOSEBOX;

	window_editor_scenario_options_anchor_border_widgets(w);
}

/**
 *
 *  rct2: 0x006708C4
 */
static void window_editor_scenario_options_guests_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
	sint32 x, y, arg;

	window_draw_widgets(w, dpi);
	window_editor_scenario_options_draw_tab_images(w, dpi);

	if (w->widgets[WIDX_CASH_PER_GUEST].type != WWT_EMPTY) {
		// Cash per guest label
		x = w->x + 8;
		y = w->y + w->widgets[WIDX_CASH_PER_GUEST].top;
		gfx_draw_string_left(dpi, STR_CASH_PER_GUEST_LABEL, NULL, COLOUR_BLACK, x, y);

		// Cash per guest value
		x = w->x + w->widgets[WIDX_CASH_PER_GUEST].left + 1;
		y = w->y + w->widgets[WIDX_CASH_PER_GUEST].top;
		arg = gGuestInitialCash;
		gfx_draw_string_left(dpi, STR_CURRENCY_FORMAT_LABEL, &arg, COLOUR_BLACK, x, y);
	}

	// Guest initial happiness label
	x = w->x + 8;
	y = w->y + w->widgets[WIDX_GUEST_INITIAL_HAPPINESS].top;
	gfx_draw_string_left(dpi, STR_GUEST_INIT_HAPPINESS, NULL, COLOUR_BLACK, x, y);

	// Guest initial happiness value
	x = w->x + w->widgets[WIDX_GUEST_INITIAL_HAPPINESS].left + 1;
	y = w->y + w->widgets[WIDX_GUEST_INITIAL_HAPPINESS].top;
	arg = (gGuestInitialHappiness * 100) / 255;
	gfx_draw_string_left(dpi, STR_PERCENT_FORMAT_LABEL, &arg, COLOUR_BLACK, x, y);

	// Guest initial hunger label
	x = w->x + 8;
	y = w->y + w->widgets[WIDX_GUEST_INITIAL_HUNGER].top;
	gfx_draw_string_left(dpi, STR_GUEST_INIT_HUNGER, NULL, COLOUR_BLACK, x, y);

	// Guest initial hunger value
	x = w->x + w->widgets[WIDX_GUEST_INITIAL_HUNGER].left + 1;
	y = w->y + w->widgets[WIDX_GUEST_INITIAL_HUNGER].top;
	arg = ((255 - gGuestInitialHunger) * 100) / 255;
	gfx_draw_string_left(dpi, STR_PERCENT_FORMAT_LABEL, &arg, COLOUR_BLACK, x, y);

	// Guest initial thirst label
	x = w->x + 8;
	y = w->y + w->widgets[WIDX_GUEST_INITIAL_THIRST].top;
	gfx_draw_string_left(dpi, STR_GUEST_INIT_THIRST, NULL, COLOUR_BLACK, x, y);

	// Guest initial thirst value
	x = w->x + w->widgets[WIDX_GUEST_INITIAL_THIRST].left + 1;
	y = w->y + w->widgets[WIDX_GUEST_INITIAL_THIRST].top;
	arg = ((255 - gGuestInitialThirst) * 100) / 255;
	gfx_draw_string_left(dpi, STR_PERCENT_FORMAT_LABEL, &arg, COLOUR_BLACK, x, y);
}

#pragma endregion

#pragma region Park

/**
 *
 *  rct2: 0x00670FD8
 */
static void window_editor_scenario_options_park_mouseup(rct_window *w, rct_widgetindex widgetIndex)
{
	switch (widgetIndex) {
	case WIDX_CLOSE:
		window_close(w);
		break;
	case WIDX_TAB_1:
	case WIDX_TAB_2:
	case WIDX_TAB_3:
		window_editor_scenario_options_set_page(w, widgetIndex - WIDX_TAB_1);
		break;
	case WIDX_FORBID_TREE_REMOVAL:
		game_do_command(
			0,
			GAME_COMMAND_FLAG_APPLY,
			EDIT_SCENARIOOPTIONS_SETFORBIDTREEREMOVAL,
			gParkFlags & PARK_FLAGS_FORBID_TREE_REMOVAL ? 0 : 1,
			GAME_COMMAND_EDIT_SCENARIO_OPTIONS,
			0,
			0
		);
		window_invalidate(w);
		break;
	case WIDX_FORBID_LANDSCAPE_CHANGES:
		game_do_command(
			0,
			GAME_COMMAND_FLAG_APPLY,
			EDIT_SCENARIOOPTIONS_SETFORBIDLANDSCAPECHANGES,
			gParkFlags & PARK_FLAGS_FORBID_LANDSCAPE_CHANGES ? 0 : 1,
			GAME_COMMAND_EDIT_SCENARIO_OPTIONS,
			0,
			0
		);
		window_invalidate(w);
		break;
	case WIDX_FORBID_HIGH_CONSTRUCTION:
		game_do_command(
			0,
			GAME_COMMAND_FLAG_APPLY,
			EDIT_SCENARIOOPTIONS_SETFORBIDHIGHCONSTRUCTION,
			gParkFlags & PARK_FLAGS_FORBID_HIGH_CONSTRUCTION ? 0 : 1,
			GAME_COMMAND_EDIT_SCENARIO_OPTIONS,
			0,
			0
		);
		window_invalidate(w);
		break;
	case WIDX_HARD_PARK_RATING:
		game_do_command(
			0,
			GAME_COMMAND_FLAG_APPLY,
			EDIT_SCENARIOOPTIONS_SETPARKRATINGHIGHERDIFFICULTLEVEL,
			gParkFlags & PARK_FLAGS_DIFFICULT_PARK_RATING ? 0 : 1,
			GAME_COMMAND_EDIT_SCENARIO_OPTIONS,
			0,
			0
		);
		window_invalidate(w);
		break;
	case WIDX_HARD_GUEST_GENERATION:
		game_do_command(
			0,
			GAME_COMMAND_FLAG_APPLY,
			EDIT_SCENARIOOPTIONS_SETGUESTGENERATIONHIGHERDIFFICULTLEVEL,
			gParkFlags & PARK_FLAGS_DIFFICULT_GUEST_GENERATION ? 0 : 1,
			GAME_COMMAND_EDIT_SCENARIO_OPTIONS,
			0,
			0
		);
		window_invalidate(w);
		break;
	}
}

/**
 *
 *  rct2: 0x00671287
 */
static void window_editor_scenario_options_park_resize(rct_window *w)
{
	window_set_resize(w, 400, 183, 400, 183);
}

/**
 *
 *  rct2: 0x00671019
 */
static void window_editor_scenario_options_park_mousedown(rct_widgetindex widgetIndex, rct_window *w, rct_widget *widget)
{
	rct_widget *dropdownWidget;

	switch (widgetIndex) {
	case WIDX_LAND_COST_INCREASE:
		if (gLandPrice < MONEY(200,00)) {
			game_do_command(
				0,
				GAME_COMMAND_FLAG_APPLY,
				EDIT_SCENARIOOPTIONS_SETCOSTTOBUYLAND,
				gLandPrice + MONEY(1,00),
				GAME_COMMAND_EDIT_SCENARIO_OPTIONS,
				0,
				0
			);
		} else {
			window_error_open(STR_CANT_INCREASE_FURTHER, STR_NONE);
		}
		window_invalidate(w);
		break;
	case WIDX_LAND_COST_DECREASE:
		if (gLandPrice > MONEY(5,00)) {
			game_do_command(
				0,
				GAME_COMMAND_FLAG_APPLY,
				EDIT_SCENARIOOPTIONS_SETCOSTTOBUYLAND,
				gLandPrice - MONEY(1, 00),
				GAME_COMMAND_EDIT_SCENARIO_OPTIONS,
				0,
				0
			);
		} else {
			window_error_open(STR_CANT_REDUCE_FURTHER, STR_NONE);
		}
		window_invalidate(w);
		break;
	case WIDX_CONSTRUCTION_RIGHTS_COST_INCREASE:
		if (gConstructionRightsPrice < MONEY(200,00)) {
			game_do_command(
				0,
				GAME_COMMAND_FLAG_APPLY,
				EDIT_SCENARIOOPTIONS_SETCOSTTOBUYCONSTRUCTIONRIGHTS,
				gConstructionRightsPrice + MONEY(1,00),
				GAME_COMMAND_EDIT_SCENARIO_OPTIONS,
				0,
				0
			);
		} else {
			window_error_open(STR_CANT_INCREASE_FURTHER, STR_NONE);
		}
		window_invalidate(w);
		break;
	case WIDX_CONSTRUCTION_RIGHTS_COST_DECREASE:
		if (gConstructionRightsPrice > MONEY(5,00)) {
			game_do_command(
				0,
				GAME_COMMAND_FLAG_APPLY,
				EDIT_SCENARIOOPTIONS_SETCOSTTOBUYCONSTRUCTIONRIGHTS,
				gConstructionRightsPrice - MONEY(1,00),
				GAME_COMMAND_EDIT_SCENARIO_OPTIONS,
				0,
				0
			);
		} else {
			window_error_open(STR_CANT_REDUCE_FURTHER, STR_NONE);
		}
		window_invalidate(w);
		break;
	case WIDX_ENTRY_PRICE_INCREASE:
		if (gParkEntranceFee < MONEY(100,00)) {
			game_do_command(
				0,
				GAME_COMMAND_FLAG_APPLY,
				EDIT_SCENARIOOPTIONS_SETPARKCHARGEENTRYFEE,
				gParkEntranceFee + MONEY(1,00),
				GAME_COMMAND_EDIT_SCENARIO_OPTIONS,
				0,
				0
			);
		} else {
			window_error_open(STR_CANT_INCREASE_FURTHER, STR_NONE);
		}
		window_invalidate(w);
		break;
	case WIDX_ENTRY_PRICE_DECREASE:
		if (gParkEntranceFee > MONEY(0,00)) {
			game_do_command(
				0,
				GAME_COMMAND_FLAG_APPLY,
				EDIT_SCENARIOOPTIONS_SETPARKCHARGEENTRYFEE,
				gParkEntranceFee - MONEY(1,00),
				GAME_COMMAND_EDIT_SCENARIO_OPTIONS,
				0,
				0
			);
		} else {
			window_error_open(STR_CANT_REDUCE_FURTHER, STR_NONE);
		}
		window_invalidate(w);
		break;
	case WIDX_PAY_FOR_PARK_OR_RIDES_DROPDOWN:
		dropdownWidget = widget - 1;

		gDropdownItemsFormat[0] = STR_DROPDOWN_MENU_LABEL;
		gDropdownItemsArgs[0] = STR_FREE_PARK_ENTER;
		gDropdownItemsFormat[1] = STR_DROPDOWN_MENU_LABEL;
		gDropdownItemsArgs[1] = STR_PAY_PARK_ENTER;

		window_dropdown_show_text_custom_width(
			w->x + dropdownWidget->left,
			w->y + dropdownWidget->top,
			dropdownWidget->bottom - dropdownWidget->top - 1,
			w->colours[1],
			0,
			DROPDOWN_FLAG_STAY_OPEN,
			2,
			dropdownWidget->right - dropdownWidget->left - 3
		);

		dropdown_set_checked((gParkFlags & PARK_FLAGS_PARK_FREE_ENTRY ? 0 : 1), true);
		break;
	}
}

/**
 *
 *  rct2: 0x00671060
 */
static void window_editor_scenario_options_park_dropdown(rct_window *w, rct_widgetindex widgetIndex, sint32 dropdownIndex)
{
	if (widgetIndex == WIDX_PAY_FOR_PARK_OR_RIDES_DROPDOWN && dropdownIndex != -1) {
		game_do_command(
			0,
			GAME_COMMAND_FLAG_APPLY,
			EDIT_SCENARIOOPTIONS_SETPARKCHARGEMETHOD,
			dropdownIndex,
			GAME_COMMAND_EDIT_SCENARIO_OPTIONS,
			0,
			0
		);
		window_invalidate(w);
	}
}

/**
 *
 *  rct2: 0x0067126D
 */
static void window_editor_scenario_options_park_update(rct_window *w)
{
	w->frame_no++;
	window_event_invalidate_call(w);
	widget_invalidate(w, WIDX_TAB_3);
}

/**
 *
 *  rct2: 0x00670CBA
 */
static void window_editor_scenario_options_park_invalidate(rct_window *w)
{
	uint64 pressedWidgets;

	rct_widget *widgets = window_editor_scenario_options_widgets[w->page];
	if (w->widgets != widgets) {
		w->widgets = widgets;
		window_init_scroll_widgets(w);
	}

	window_editor_scenario_options_set_pressed_tab(w);

	if (((gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) && (gParkFlags & PARK_FLAGS_NO_MONEY_SCENARIO)) ||
		(!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) && (gParkFlags & PARK_FLAGS_NO_MONEY))) {
		for (sint32 i = WIDX_LAND_COST; i <= WIDX_ENTRY_PRICE_DECREASE; i++)
			w->widgets[i].type = WWT_EMPTY;
	} else {
		w->widgets[WIDX_LAND_COST].type = WWT_SPINNER;
		w->widgets[WIDX_LAND_COST_INCREASE].type = WWT_DROPDOWN_BUTTON;
		w->widgets[WIDX_LAND_COST_DECREASE].type = WWT_DROPDOWN_BUTTON;
		w->widgets[WIDX_CONSTRUCTION_RIGHTS_COST].type = WWT_SPINNER;
		w->widgets[WIDX_CONSTRUCTION_RIGHTS_COST_INCREASE].type = WWT_DROPDOWN_BUTTON;
		w->widgets[WIDX_CONSTRUCTION_RIGHTS_COST_DECREASE].type = WWT_DROPDOWN_BUTTON;
		w->widgets[WIDX_PAY_FOR_PARK_OR_RIDES].type = WWT_DROPDOWN;
		w->widgets[WIDX_PAY_FOR_PARK_OR_RIDES_DROPDOWN].type = WWT_DROPDOWN_BUTTON;

		if (gParkFlags & PARK_FLAGS_PARK_FREE_ENTRY) {
			w->widgets[WIDX_ENTRY_PRICE].type = WWT_EMPTY;
			w->widgets[WIDX_ENTRY_PRICE_INCREASE].type = WWT_EMPTY;
			w->widgets[WIDX_ENTRY_PRICE_DECREASE].type = WWT_EMPTY;
		} else {
			w->widgets[WIDX_ENTRY_PRICE].type = WWT_SPINNER;
			w->widgets[WIDX_ENTRY_PRICE_INCREASE].type = WWT_DROPDOWN_BUTTON;
			w->widgets[WIDX_ENTRY_PRICE_DECREASE].type = WWT_DROPDOWN_BUTTON;
		}
	}

	// Set checkboxes
	pressedWidgets = w->pressed_widgets;
	pressedWidgets &= ~(1 << WIDX_FORBID_TREE_REMOVAL);
	pressedWidgets &= ~(1 << WIDX_FORBID_LANDSCAPE_CHANGES);
	pressedWidgets &= ~(1 << WIDX_FORBID_HIGH_CONSTRUCTION);
	pressedWidgets &= ~(1 << WIDX_HARD_PARK_RATING);
	pressedWidgets &= ~(1 << WIDX_HARD_GUEST_GENERATION);

	if (gParkFlags & PARK_FLAGS_FORBID_TREE_REMOVAL)
		pressedWidgets |= (1 << WIDX_FORBID_TREE_REMOVAL);
	if (gParkFlags & PARK_FLAGS_FORBID_LANDSCAPE_CHANGES)
		pressedWidgets |= (1 << WIDX_FORBID_LANDSCAPE_CHANGES);
	if (gParkFlags & PARK_FLAGS_FORBID_HIGH_CONSTRUCTION)
		pressedWidgets |= (1 << WIDX_FORBID_HIGH_CONSTRUCTION);
	if (gParkFlags & PARK_FLAGS_DIFFICULT_PARK_RATING)
		pressedWidgets |= (1 << WIDX_HARD_PARK_RATING);
	if (gParkFlags & PARK_FLAGS_DIFFICULT_GUEST_GENERATION)
		pressedWidgets |= (1 << WIDX_HARD_GUEST_GENERATION);

	w->pressed_widgets = pressedWidgets;

	w->widgets[WIDX_CLOSE].type = (gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) ? WWT_EMPTY : WWT_CLOSEBOX;

	window_editor_scenario_options_anchor_border_widgets(w);
}

/**
 *
 *  rct2: 0x00670E5B
 */
static void window_editor_scenario_options_park_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
	sint32 x, y, arg;
	rct_string_id stringId;

	window_draw_widgets(w, dpi);
	window_editor_scenario_options_draw_tab_images(w, dpi);

	if (w->widgets[WIDX_LAND_COST].type != WWT_EMPTY) {
		// Cost to buy land label
		x = w->x + 8;
		y = w->y + w->widgets[WIDX_LAND_COST].top;
		gfx_draw_string_left(dpi, STR_LAND_COST_LABEL, NULL, COLOUR_BLACK, x, y);

		// Cost to buy land value
		x = w->x + w->widgets[WIDX_LAND_COST].left + 1;
		y = w->y + w->widgets[WIDX_LAND_COST].top;
		arg = gLandPrice;
		gfx_draw_string_left(dpi, STR_CURRENCY_FORMAT_LABEL, &arg, COLOUR_BLACK, x, y);
	}

	if (w->widgets[WIDX_CONSTRUCTION_RIGHTS_COST].type != WWT_EMPTY) {
		// Cost to buy construction rights label
		x = w->x + 8;
		y = w->y + w->widgets[WIDX_CONSTRUCTION_RIGHTS_COST].top;
		gfx_draw_string_left(dpi, STR_RIGHTS_COST_LABEL, NULL, COLOUR_BLACK, x, y);

		// Cost to buy construction rights value
		x = w->x + w->widgets[WIDX_CONSTRUCTION_RIGHTS_COST].left + 1;
		y = w->y + w->widgets[WIDX_CONSTRUCTION_RIGHTS_COST].top;
		arg = gConstructionRightsPrice;
		gfx_draw_string_left(dpi, STR_CURRENCY_FORMAT_LABEL, &arg, COLOUR_BLACK, x, y);
	}

	if (w->widgets[WIDX_PAY_FOR_PARK_OR_RIDES].type != WWT_EMPTY) {
		// Pay for park or rides label
		x = w->x + w->widgets[WIDX_PAY_FOR_PARK_OR_RIDES].left + 1;
		y = w->y + w->widgets[WIDX_PAY_FOR_PARK_OR_RIDES].top;
		gfx_draw_string_left(dpi, STR_FREE_PARK_ENTER, NULL, COLOUR_BLACK, x, y);

		// Pay for park or rides value
		stringId = gParkFlags & PARK_FLAGS_PARK_FREE_ENTRY ? STR_FREE_PARK_ENTER : STR_PAY_PARK_ENTER;
		gfx_draw_string_left(dpi, STR_WINDOW_COLOUR_2_STRINGID, &stringId, COLOUR_BLACK, x, y);
	}

	if (w->widgets[WIDX_ENTRY_PRICE].type != WWT_EMPTY) {
		// Entry price label
		x = w->x + w->widgets[WIDX_PAY_FOR_PARK_OR_RIDES].right + 8;
		y = w->y + w->widgets[WIDX_ENTRY_PRICE].top;
		gfx_draw_string_left(dpi, STR_ENTRY_PRICE_LABEL, NULL, COLOUR_BLACK, x, y);

		// Entry price value
		x = w->x + w->widgets[WIDX_ENTRY_PRICE].left + 1;
		y = w->y + w->widgets[WIDX_ENTRY_PRICE].top;
		arg = gParkEntranceFee;
		gfx_draw_string_left(dpi, STR_CURRENCY_FORMAT_LABEL, &arg, COLOUR_BLACK, x, y);
	}
}

#pragma endregion
