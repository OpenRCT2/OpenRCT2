/*****************************************************************************
* Copyright (c) 2014 Maciek Baron
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
#include "game.h"
#include "scenario.h"
#include "string_ids.h"
#include "sprites.h"
#include "widget.h"
#include "window.h"

enum {
	WINDOW_FINANCES_PAGE_SUMMARY,
	WINDOW_FINANCES_PAGE_FINANCIAL_GRAPH,
	WINDOW_FINANCES_PAGE_VALUE_GRAPH,
	WINDOW_FINANCES_PAGE_PROFIT_GRAPH,
	WINDOW_FINANCES_PAGE_MARKETING,
	WINDOW_FINANCES_PAGE_RESEARCH,
	WINDOW_FINANCES_PAGE_COUNT
};

enum {
	WIDX_BACKGROUND,
	WIDX_TITLE,
	WIDX_CLOSE,
	WIDX_PAGE_BACKGROUND,
	WIDX_TAB_1,
	WIDX_TAB_2,
	WIDX_TAB_3,
	WIDX_TAB_4,
	WIDX_TAB_5,
	WIDX_TAB_6,

	WIDX_LOAN = 10,
	WIDX_LOAN_INCREASE,
	WIDX_LOAN_DECREASE,

	WIDX_CAMPAIGN_1 = 12,
	WIDX_CAMPAIGN_2,
	WIDX_CAMPAIGN_3,
	WIDX_CAMPAIGN_4,
	WIDX_CAMPAIGN_5,
	WIDX_CAMPAIGN_6,

	WIDX_RESEARCH_FUNDING = 11,
	WIDX_RESEARCH_FUNDING_DROPDOWN_BUTTON,
	WIDX_TRANSPORT_RIDES = 14,
	WIDX_GENTLE_RIDES,
	WIDX_ROLLER_COASTERS,
	WIDX_THRILL_RIDES,
	WIDX_WATER_RIDES,
	WIDX_SHOPS_AND_STALLS,
	WIDX_SCENERY_AND_THEMING,
};

#pragma region Widgets

static rct_widget window_finances_summary_widgets[] = {
	{ WWT_FRAME,			0,	0,		529,	0,		256,	0xFFFFFFFF,				STR_NONE },
	{ WWT_CAPTION,			0,	1,		528,	1,		14,		STR_FINANCIAL_SUMMARY,	STR_WINDOW_TITLE_TIP },
	{ WWT_CLOSEBOX,			0,	517,	527,	2,		13,		STR_CLOSE_X,			STR_CLOSE_WINDOW_TIP },
	{ WWT_RESIZE,			1,	0,		529,	43,		256,	0xFFFFFFFF,				STR_NONE },
	{ WWT_TAB,				1,	3,		33,		17,		43,		0x2000144E,				STR_FINANCES_SHOW_SUMMARY_TAB_TIP },
	{ WWT_TAB,				1,	34,		64,		17,		43,		0x2000144E,				STR_FINANCES_SHOW_CASH_TAB_TIP },
	{ WWT_TAB,				1,	65,		95,		17,		43,		0x2000144E,				STR_FINANCES_SHOW_PARK_VALUE_TAB_TIP },
	{ WWT_TAB,				1,	96,		126,	17,		43,		0x2000144E,				STR_FINANCES_SHOW_WEEKLY_PROFIT_TAB_TIP },
	{ WWT_TAB,				1,	127,	157,	17,		43,		0x2000144E,				STR_FINANCES_SHOW_MARKETING_TAB_TIP },
	{ WWT_TAB,				1,	158,	188,	17,		43,		0x2000144E,				STR_FINANCES_RESEARCH },
	{ WWT_SPINNER,			1,	64,		153,	229,	240,	1917,					STR_NONE },
	{ WWT_DROPDOWN_BUTTON,	1,	142,	152,	230,	234,	STR_NUMERIC_UP,			STR_NONE },
	{ WWT_DROPDOWN_BUTTON,	1,	142,	152,	235,	239,	STR_NUMERIC_DOWN,		STR_NONE },
	{ WIDGETS_END },
};

static rct_widget window_finances_cash_widgets[] = {
	{ WWT_FRAME,			0,	0,		529,	0,		256,	0xFFFFFFFF,				STR_NONE },
	{ WWT_CAPTION,			0,	1,		528,	1,		14,		STR_FINANCIAL_GRAPH,	STR_WINDOW_TITLE_TIP },
	{ WWT_CLOSEBOX,			0,	517,	527,	2,		13,		STR_CLOSE_X,			STR_CLOSE_WINDOW_TIP },
	{ WWT_RESIZE,			1,	0,		529,	43,		256,	0xFFFFFFFF,				STR_NONE },
	{ WWT_TAB,				1,	3,		33,		17,		43,		0x2000144E,				STR_FINANCES_SHOW_SUMMARY_TAB_TIP },
	{ WWT_TAB,				1,	34,		64,		17,		43,		0x2000144E,				STR_FINANCES_SHOW_CASH_TAB_TIP },
	{ WWT_TAB,				1,	65,		95,		17,		43,		0x2000144E,				STR_FINANCES_SHOW_PARK_VALUE_TAB_TIP },
	{ WWT_TAB,				1,	96,		126,	17,		43,		0x2000144E,				STR_FINANCES_SHOW_WEEKLY_PROFIT_TAB_TIP },
	{ WWT_TAB,				1,	127,	157,	17,		43,		0x2000144E,				STR_FINANCES_SHOW_MARKETING_TAB_TIP },
	{ WWT_TAB,				1,	158,	188,	17,		43,		0x2000144E,				STR_FINANCES_RESEARCH },
	{ WIDGETS_END },
};

static rct_widget window_finances_park_value_widgets[] = {
	{ WWT_FRAME,			0,	0,		529,	0,		256,	0xFFFFFFFF,				STR_NONE },
	{ WWT_CAPTION,			0,	1,		528,	1,		14,		STR_PARK_VALUE_GRAPH,	STR_WINDOW_TITLE_TIP },
	{ WWT_CLOSEBOX,			0,	517,	527,	2,		13,		STR_CLOSE_X,			STR_CLOSE_WINDOW_TIP },
	{ WWT_RESIZE,			1,	0,		529,	43,		256,	0xFFFFFFFF,				STR_NONE },
	{ WWT_TAB,				1,	3,		33,		17,		43,		0x2000144E,				STR_FINANCES_SHOW_SUMMARY_TAB_TIP },
	{ WWT_TAB,				1,	34,		64,		17,		43,		0x2000144E,				STR_FINANCES_SHOW_CASH_TAB_TIP },
	{ WWT_TAB,				1,	65,		95,		17,		43,		0x2000144E,				STR_FINANCES_SHOW_PARK_VALUE_TAB_TIP },
	{ WWT_TAB,				1,	96,		126,	17,		43,		0x2000144E,				STR_FINANCES_SHOW_WEEKLY_PROFIT_TAB_TIP },
	{ WWT_TAB,				1,	127,	157,	17,		43,		0x2000144E,				STR_FINANCES_SHOW_MARKETING_TAB_TIP },
	{ WWT_TAB,				1,	158,	188,	17,		43,		0x2000144E,				STR_FINANCES_RESEARCH },
	{ WIDGETS_END },
};

static rct_widget window_finances_profit_widgets[] = {
	{ WWT_FRAME,			0,	0,		529,	0,		256,	0xFFFFFFFF,				STR_NONE },
	{ WWT_CAPTION,			0,	1,		528,	1,		14,		STR_PROFIT_GRAPH,		STR_WINDOW_TITLE_TIP },
	{ WWT_CLOSEBOX,			0,	517,	527,	2,		13,		STR_CLOSE_X,			STR_CLOSE_WINDOW_TIP },
	{ WWT_RESIZE,			1,	0,		529,	43,		256,	0xFFFFFFFF,				STR_NONE },
	{ WWT_TAB,				1,	3,		33,		17,		43,		0x2000144E,				STR_FINANCES_SHOW_SUMMARY_TAB_TIP },
	{ WWT_TAB,				1,	34,		64,		17,		43,		0x2000144E,				STR_FINANCES_SHOW_CASH_TAB_TIP },
	{ WWT_TAB,				1,	65,		95,		17,		43,		0x2000144E,				STR_FINANCES_SHOW_PARK_VALUE_TAB_TIP },
	{ WWT_TAB,				1,	96,		126,	17,		43,		0x2000144E,				STR_FINANCES_SHOW_WEEKLY_PROFIT_TAB_TIP },
	{ WWT_TAB,				1,	127,	157,	17,		43,		0x2000144E,				STR_FINANCES_SHOW_MARKETING_TAB_TIP },
	{ WWT_TAB,				1,	158,	188,	17,		43,		0x2000144E,				STR_FINANCES_RESEARCH },
	{ WIDGETS_END },
};

static rct_widget window_finances_marketing_widgets[] = {
	{ WWT_FRAME,			0,	0,		529,	0,		256,	0xFFFFFFFF,								STR_NONE },
	{ WWT_CAPTION,			0,	1,		528,	1,		14,		STR_MARKETING,							STR_WINDOW_TITLE_TIP },
	{ WWT_CLOSEBOX,			0,	517,	527,	2,		13,		STR_CLOSE_X,							STR_CLOSE_WINDOW_TIP },
	{ WWT_RESIZE,			1,	0,		529,	43,		256,	0xFFFFFFFF,								STR_NONE },
	{ WWT_TAB,				1,	3,		33,		17,		43,		0x2000144E,								STR_FINANCES_SHOW_SUMMARY_TAB_TIP },
	{ WWT_TAB,				1,	34,		64,		17,		43,		0x2000144E,								STR_FINANCES_SHOW_CASH_TAB_TIP },
	{ WWT_TAB,				1,	65,		95,		17,		43,		0x2000144E,								STR_FINANCES_SHOW_PARK_VALUE_TAB_TIP },
	{ WWT_TAB,				1,	96,		126,	17,		43,		0x2000144E,								STR_FINANCES_SHOW_WEEKLY_PROFIT_TAB_TIP },
	{ WWT_TAB,				1,	127,	157,	17,		43,		0x2000144E,								STR_FINANCES_SHOW_MARKETING_TAB_TIP },
	{ WWT_TAB,				1,	158,	188,	17,		43,		0x2000144E,								STR_FINANCES_RESEARCH },
	{ WWT_GROUPBOX,			2,	3,		526,	47,		91,		STR_MARKETING_CAMPAIGNS_IN_OPERATION,	STR_NONE },
	{ WWT_GROUPBOX,			2,	3,		526,	47,		252,	STR_MARKETING_CAMPAIGNS_AVAILABLE,		STR_NONE },
	{ WWT_IMGBTN,			1,	8,		521,	0,		11,		0xFFFFFFFF,								STR_START_THIS_MARKETING_CAMPAIGN },
	{ WWT_IMGBTN,			1,	8,		521,	0,		11,		0xFFFFFFFF,								STR_START_THIS_MARKETING_CAMPAIGN },
	{ WWT_IMGBTN,			1,	8,		521,	0,		11,		0xFFFFFFFF,								STR_START_THIS_MARKETING_CAMPAIGN },
	{ WWT_IMGBTN,			1,	8,		521,	0,		11,		0xFFFFFFFF,								STR_START_THIS_MARKETING_CAMPAIGN },
	{ WWT_IMGBTN,			1,	8,		521,	0,		11,		0xFFFFFFFF,								STR_START_THIS_MARKETING_CAMPAIGN },
	{ WWT_IMGBTN,			1,	8,		521,	0,		11,		0xFFFFFFFF,								STR_START_THIS_MARKETING_CAMPAIGN },
	{ WIDGETS_END },
};

static rct_widget window_finances_research_widgets[] = {
	{ WWT_FRAME,			0,	0,		529,	0,		256,	0xFFFFFFFF,								STR_NONE },
	{ WWT_CAPTION,			0,	1,		528,	1,		14,		STR_RESEARCH_FUNDING,					STR_WINDOW_TITLE_TIP },
	{ WWT_CLOSEBOX,			0,	517,	527,	2,		13,		STR_CLOSE_X,							STR_CLOSE_WINDOW_TIP },
	{ WWT_RESIZE,			1,	0,		529,	43,		256,	0xFFFFFFFF,								STR_NONE },
	{ WWT_TAB,				1,	3,		33,		17,		43,		0x2000144E,								STR_FINANCES_SHOW_SUMMARY_TAB_TIP },
	{ WWT_TAB,				1,	34,		64,		17,		43,		0x2000144E,								STR_FINANCES_SHOW_CASH_TAB_TIP },
	{ WWT_TAB,				1,	65,		95,		17,		43,		0x2000144E,								STR_FINANCES_SHOW_PARK_VALUE_TAB_TIP },
	{ WWT_TAB,				1,	96,		126,	17,		43,		0x2000144E,								STR_FINANCES_SHOW_WEEKLY_PROFIT_TAB_TIP },
	{ WWT_TAB,				1,	127,	157,	17,		43,		0x2000144E,								STR_FINANCES_SHOW_MARKETING_TAB_TIP },
	{ WWT_TAB,				1,	158,	188,	17,		43,		0x2000144E,								STR_FINANCES_RESEARCH },
	{ WWT_GROUPBOX,			2,	3,		316,	47,		91,		STR_RESEARCH_FUNDING_,					STR_NONE },
	{ WWT_DROPDOWN,			2,	8,		167,	59,		70,		0xFFFFFFFF,								STR_SELECT_LEVEL_OF_RESEARCH_AND_DEVELOPMENT },
	{ WWT_DROPDOWN_BUTTON,	2,	156,	166,	60,		69,		876,									STR_SELECT_LEVEL_OF_RESEARCH_AND_DEVELOPMENT },
	{ WWT_GROUPBOX,			2,	3,		316,	96,		202,	STR_RESEARCH_PRIORITIES,				STR_NONE },
	{ WWT_CHECKBOX,			2,	8,		311,	108,	119,	STR_RESEARCH_TRANSPORT_RIDES,			STR_RESEARCH_NEW_TRANSPORT_RIDES },
	{ WWT_CHECKBOX,			2,	8,		311,	121,	132,	STR_RESEARCH_GENTLE_RIDES,				STR_RESEARCH_NEW_GENTLE_RIDES },
	{ WWT_CHECKBOX,			2,	8,		311,	134,	145,	STR_RESEARCH_ROLLER_COASTERS,			STR_RESEARCH_NEW_ROLLER_COASTERS },
	{ WWT_CHECKBOX,			2,	8,		311,	147,	158,	STR_RESEARCH_THRILL_RIDES,				STR_RESEARCH_NEW_THRILL_RIDES },
	{ WWT_CHECKBOX,			2,	8,		311,	160,	171,	STR_RESEARCH_WATER_RIDES,				STR_RESEARCH_NEW_WATER_RIDES },
	{ WWT_CHECKBOX,			2,	8,		311,	173,	184,	STR_RESEARCH_SHOPS_AND_STALLS,			STR_RESEARCH_NEW_SHOPS_AND_STALLS },
	{ WWT_CHECKBOX,			2,	8,		311,	186,	197,	STR_RESEARCH_SCENERY_AND_THEMING,		STR_RESEARCH_NEW_SCENERY_AND_THEMING },
	{ WIDGETS_END },
};

static rct_widget *window_finances_page_widgets[] = {
	window_finances_summary_widgets,
	window_finances_cash_widgets,
	window_finances_park_value_widgets,
	window_finances_profit_widgets,
	window_finances_marketing_widgets,
	window_finances_research_widgets
};

#pragma endregion

#pragma region Events

static void window_finances_emptysub() { }

static void window_finances_summary_mouseup();
static void window_finances_summary_mousedown(int widgetIndex, rct_window*w, rct_widget* widget);
static void window_finances_summary_update(rct_window *w);
static void window_finances_summary_invalidate();
static void window_finances_summary_paint();

// 0x00988EB8
static void* window_finances_summary_events[] = {
	window_finances_emptysub,
	window_finances_summary_mouseup,
	window_finances_emptysub,
	window_finances_summary_mousedown,
	window_finances_emptysub,
	window_finances_emptysub,
	window_finances_summary_update,
	window_finances_emptysub,
	window_finances_emptysub,
	window_finances_emptysub,
	window_finances_emptysub,
	window_finances_emptysub,
	window_finances_emptysub,
	window_finances_emptysub,
	window_finances_emptysub,
	window_finances_emptysub,
	window_finances_emptysub,
	window_finances_emptysub,
	window_finances_emptysub,
	window_finances_emptysub,
	window_finances_emptysub,
	window_finances_emptysub,
	window_finances_emptysub,
	window_finances_emptysub,
	window_finances_emptysub,
	window_finances_summary_invalidate,
	window_finances_summary_paint,
	window_finances_emptysub
};

static void* window_finances_page_events[] = {
	window_finances_summary_events,
	(void*)0x00988F28,
	(void*)0x00988F98,
	(void*)0x00989008,
	(void*)0x00989078,
	(void*)0x009890E8
};

#pragma endregion

#pragma region Enabled widgets

static uint32 window_finances_page_enabled_widgets[] = {
	(1 << WIDX_CLOSE) |
	(1 << WIDX_TAB_1) |
	(1 << WIDX_TAB_2) |
	(1 << WIDX_TAB_3) |
	(1 << WIDX_TAB_4) |
	(1 << WIDX_TAB_5) |
	(1 << WIDX_TAB_6) |
	(1 << WIDX_LOAN_INCREASE) |
	(1 << WIDX_LOAN_DECREASE),

	(1 << WIDX_CLOSE) |
	(1 << WIDX_TAB_1) |
	(1 << WIDX_TAB_2) |
	(1 << WIDX_TAB_3) |
	(1 << WIDX_TAB_4) |
	(1 << WIDX_TAB_5) |
	(1 << WIDX_TAB_6),

	(1 << WIDX_CLOSE) |
	(1 << WIDX_TAB_1) |
	(1 << WIDX_TAB_2) |
	(1 << WIDX_TAB_3) |
	(1 << WIDX_TAB_4) |
	(1 << WIDX_TAB_5) |
	(1 << WIDX_TAB_6),

	(1 << WIDX_CLOSE) |
	(1 << WIDX_TAB_1) |
	(1 << WIDX_TAB_2) |
	(1 << WIDX_TAB_3) |
	(1 << WIDX_TAB_4) |
	(1 << WIDX_TAB_5) |
	(1 << WIDX_TAB_6),

	(1 << WIDX_CLOSE) |
	(1 << WIDX_TAB_1) |
	(1 << WIDX_TAB_2) |
	(1 << WIDX_TAB_3) |
	(1 << WIDX_TAB_4) |
	(1 << WIDX_TAB_5) |
	(1 << WIDX_TAB_6) |
	(1 << WIDX_CAMPAIGN_1) |
	(1 << WIDX_CAMPAIGN_2) |
	(1 << WIDX_CAMPAIGN_3) |
	(1 << WIDX_CAMPAIGN_4) |
	(1 << WIDX_CAMPAIGN_5) |
	(1 << WIDX_CAMPAIGN_6),

	(1 << WIDX_CLOSE) |
	(1 << WIDX_TAB_1) |
	(1 << WIDX_TAB_2) |
	(1 << WIDX_TAB_3) |
	(1 << WIDX_TAB_4) |
	(1 << WIDX_TAB_5) |
	(1 << WIDX_TAB_6) |

	(1 << WIDX_RESEARCH_FUNDING) |
	(1 << WIDX_RESEARCH_FUNDING_DROPDOWN_BUTTON) |
	(1 << WIDX_TRANSPORT_RIDES) |
	(1 << WIDX_GENTLE_RIDES) |
	(1 << WIDX_ROLLER_COASTERS) |
	(1 << WIDX_THRILL_RIDES) |
	(1 << WIDX_WATER_RIDES) |
	(1 << WIDX_SHOPS_AND_STALLS) |
	(1 << WIDX_SCENERY_AND_THEMING)
};

#pragma endregion

const int window_finances_tab_animation_loops[] = { 16, 32, 32, 32, 38, 16 };

static void window_finances_set_page(rct_window *w, int page);
static void window_finances_set_pressed_tab(rct_window *w);
static void window_finances_draw_tab_images(rct_drawpixelinfo *dpi, rct_window *w);

/**
 *
 *  rct2: 0x0069DDF1
 */
void window_finances_open()
{
	rct_window *w;

	w = window_bring_to_front_by_id(WC_FINANCES, 0);
	if (w == NULL) {
		w = window_create_auto_pos(530, 257, window_finances_page_events[0], WC_FINANCES, WF_10);
		w->widgets = window_finances_page_widgets[0];
		w->enabled_widgets = 0x1BF4;
		w->number = 0;
		w->page = 0;
		w->frame_no = 0;
		w->disabled_widgets = 0;
		w->colours[0] = 1;
		w->colours[1] = 19;
		w->colours[2] = 19;
		RCT2_CALLPROC_EBPSAFE(0x00684BAE);
	}

	w->page = 0;
	window_invalidate(w);
	w->width = 530;
	w->height = 257;
	window_invalidate(w);

	w->widgets = window_finances_page_widgets[0];
	w->enabled_widgets = window_finances_page_enabled_widgets[0];
	w->var_020 = RCT2_GLOBAL(0x00988E3C, uint32);
	w->event_handlers = window_finances_page_events[0];
	w->pressed_widgets = 0;
	w->disabled_widgets = 0;
	window_init_scroll_widgets(w);
}

#pragma region Summary page

/**
 *
 *  rct2: 0x0069CA99
 */
static void window_finances_summary_mouseup()
{
	short widgetIndex;
	rct_window *w;

	#ifdef _MSC_VER
	__asm mov widgetIndex, dx
	#else
	__asm__ ( "mov %[widgetIndex], dx " : [widgetIndex] "+m" (widgetIndex) );
	#endif

	#ifdef _MSC_VER
	__asm mov w, esi
	#else
	__asm__ ( "mov %[w], esi " : [w] "+m" (w) );
	#endif

	if (widgetIndex == WIDX_CLOSE)
		window_close(w);
	else if (widgetIndex >= WIDX_TAB_1 && widgetIndex <= WIDX_TAB_6)
		window_finances_set_page(w, widgetIndex - WIDX_TAB_1);
}

/**
 *
 *  rct2: 0x0069CAB0
 */
static void window_finances_summary_mousedown(int widgetIndex, rct_window*w, rct_widget* widget)
{
	money32 newLoan;

	switch (widgetIndex) {
	case WIDX_LOAN_INCREASE:
		newLoan = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_LOAN, money32) + MONEY(1000, 00);
		RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TITLE, uint16) = STR_CANT_BORROW_ANY_MORE_MONEY;
		game_do_command(0, 1, 0, newLoan, GAME_COMMAND_SET_CURRENT_LOAN, 0, 0);
		break;
	case WIDX_LOAN_DECREASE:
		if (RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_LOAN, money32) > 0) {
			newLoan = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_LOAN, money32) - MONEY(1000, 00);
			RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TITLE, uint16) = STR_CANT_PAY_BACK_LOAN;
			game_do_command(0, 1, 0, newLoan, GAME_COMMAND_SET_CURRENT_LOAN, 0, 0);
		}
		break;
	}
}

/**
 *
 *  rct2: 0x0069CBA6
 */
static void window_finances_summary_update(rct_window *w)
{
	// Tab animation
	if (++w->frame_no >= window_finances_tab_animation_loops[w->page])
		w->frame_no = 0;
	widget_invalidate(w->classification, w->number, WIDX_TAB_1);
}

/**
 * 
 *  rct2: 0x0069C732
 */
static void window_finances_summary_invalidate()
{
	rct_window *w;

	#ifdef _MSC_VER
	__asm mov w, esi
	#else
	__asm__ ( "mov %[w], esi " : [w] "+m" (w) );
	#endif

	if (w->widgets != window_finances_page_widgets[0]) {
		w->widgets = window_finances_page_widgets[0];
		window_init_scroll_widgets(w);
	}

	window_finances_set_pressed_tab(w);
	RCT2_GLOBAL(0x013CE952 + 6, money32) = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_LOAN, money32);
}

/**
 * 
 *  rct2: 0x0069C771
 */
static void window_finances_summary_paint()
{
	rct_window *w;
	rct_drawpixelinfo *dpi;
	int i, j, x, y;

	#ifdef _MSC_VER
	__asm mov w, esi
	#else
	__asm__ ( "mov %[w], esi " : [w] "+m" (w) );
	#endif

	#ifdef _MSC_VER
	__asm mov dpi, edi
	#else
	__asm__ ( "mov %[dpi], edi " : [dpi] "+m" (dpi) );
	#endif

	window_draw_widgets(w, dpi);
	window_finances_draw_tab_images(dpi, w);

	x = w->x + 8;
	y = w->y + 47;

	// Expenditure / Income heading
	draw_string_left_underline(dpi, STR_FINANCES_SUMMARY_EXPENDITURE_INCOME, NULL, 0, x, y - 1);
	y += 14;

	// Expenditure / Income row labels
	for (i = 0; i < 14; i++) {
		// Darken every even row
		if (i % 2 == 0)
			gfx_fill_rect(dpi, x, y, x + 513 - 2, y + 9, RCT2_GLOBAL(0x0141FC4A + (w->colours[1] * 8), uint8) | 0x1000000);

		gfx_draw_string_left(dpi, STR_FINANCES_SUMMARY_RIDE_CONSTRUCTION + i, NULL, 0, x, y - 1);
		y += 10;
	}

	// Expenditure / Income values for each month
	x = w->x + 118;
	sint16 currentMonthYear = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_MONTH_YEAR, sint16);
	for (i = 4; i >= 0; i--) {
		y = w->y + 47;

		sint16 monthyear = currentMonthYear - i;
		if (monthyear < 0)
			continue;

		// Month heading
		RCT2_GLOBAL(0x0013CE952, uint16) = STR_FINANCES_SUMMARY_MONTH_HEADING;
		RCT2_GLOBAL(0x0013CE952 + 2, uint16) = monthyear;
		draw_string_right_underline(
			dpi,
			monthyear == currentMonthYear ? 1193 : 1191,
			(void*)0x0013CE952,
			0,
			x + 80,
			y - 1
		);
		y += 14;
		
		// Month expenditures
		money32 profit = 0;
		money32 *expenditures = &RCT2_ADDRESS(RCT2_ADDRESS_EXPENDITURE_TABLE, money32)[i * 14];
		for (j = 0; j < 14; j++) {
			money32 expenditure = expenditures[j];
			if (expenditure != 0) {
				profit += expenditure;
				gfx_draw_string_right(
					dpi,
					expenditure >= 0 ? STR_FINANCES_SUMMARY_INCOME_VALUE : STR_FINANCES_SUMMARY_EXPENDITURE_VALUE,
					&expenditure,
					0,
					x + 80,
					y - 1
				);
			}
			y += 10;
		}
		y += 4;

		// Month profit
		gfx_draw_string_right(
			dpi,
			profit >= 0 ? STR_FINANCES_SUMMARY_INCOME_VALUE : STR_FINANCES_SUMMARY_LOSS_VALUE,
			&profit,
			0,
			x + 80,
			y - 1
		);
		gfx_fill_rect(dpi, x + 10, y - 2, x + 10 + 70, y - 2, 10);

		x += 80;
	}

	
	// Horizontal rule below expenditure / income table
	gfx_fill_rect_inset(dpi, w->x + 8, w->y + 223, w->x + 8 + 513, w->y + 223 + 1, w->colours[1], 0x20);

	// Loan and interest rate
	gfx_draw_string_left(dpi, STR_FINANCES_SUMMARY_LOAN, NULL, 0, w->x + 4, w->y + 229);
	RCT2_GLOBAL(0x0013CE952, uint16) = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_INTEREST_RATE, uint8);
	gfx_draw_string_left(dpi, STR_FINANCES_SUMMARY_AT_X_PER_YEAR, (void*)0x0013CE952, 0, w->x + 156, w->y + 229);

	// Current cash
	RCT2_GLOBAL(0x0013CE952, money32) = DECRYPT_MONEY(RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_MONEY_ENCRYPTED, money32));
	gfx_draw_string_left(dpi, STR_CASH_LABEL, (void*)0x0013CE952, 0, w->x + 4, w->y + 244);

	// Objective related financial information
	if (RCT2_GLOBAL(RCT2_ADDRESS_OBJECTIVE_TYPE, uint8) == OBJECTIVE_MONTHLY_FOOD_INCOME) {
		// Last month's profit from food, drink and merchandise
		money32 lastMonthProfit = 0;
		if (RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_MONTH_YEAR, uint16) != 0) {
			lastMonthProfit += RCT2_GLOBAL(0x01357898, money32);
			lastMonthProfit += RCT2_GLOBAL(0x0135789C, money32);
			lastMonthProfit += RCT2_GLOBAL(0x013578A0, money32);
			lastMonthProfit += RCT2_GLOBAL(0x013578A4, money32);
		}
		RCT2_GLOBAL(0x0013CE952, money32) = lastMonthProfit;
		gfx_draw_string_left(dpi, STR_LAST_MONTH_PROFIT_FROM_FOOD_DRINK_MERCHANDISE_SALES_LABEL, (void*)0x0013CE952, 0, w->x + 280, w->y + 229);
	} else {
		// Park value and company value
		gfx_draw_string_left(dpi, STR_PARK_VALUE_LABEL, (void*)RCT2_ADDRESS_CURRENT_PARK_VALUE, 0, w->x + 280, w->y + 229);
		gfx_draw_string_left(dpi, STR_COMPANY_VALUE_LABEL, (void*)RCT2_ADDRESS_CURRENT_COMPANY_VALUE, 0, w->x + 280, w->y + 244);
	}
}

#pragma endregion

#pragma region Common

/**
 *
 *  rct2: 0x0069CAC5
 */
static void window_finances_set_page(rct_window *w, int page)
{
	w->page = page;
	w->frame_no = 0;
	if (w->viewport != NULL) {
		w->viewport->width = 0;
		w->viewport = NULL;
	}

	w->enabled_widgets = window_finances_page_enabled_widgets[page];
	w->var_020 = RCT2_ADDRESS(0x00988E3C, uint32)[page];
	w->event_handlers = window_finances_page_events[page];
	w->widgets = window_finances_page_widgets[page];
	w->disabled_widgets = 0;
	
	window_invalidate(w);
	if (w->page == WINDOW_FINANCES_PAGE_RESEARCH) {
		w->width = 320;
		w->height = 207;
	} else {
		w->width = 530;
		w->height = 257;
	}
	RCT2_CALLPROC_X(w->event_handlers[WE_RESIZE], 0, 0, 0, 0, (int)w, 0, 0);
	RCT2_CALLPROC_X(w->event_handlers[WE_INVALIDATE], 0, 0, 0, 0, (int)w, 0, 0);

	window_init_scroll_widgets(w);
	window_invalidate(w);
}

static void window_finances_set_pressed_tab(rct_window *w)
{
	int i;
	for (i = 0; i < WINDOW_FINANCES_PAGE_COUNT; i++)
		w->pressed_widgets &= ~(1 << (WIDX_TAB_1 + i));
	w->pressed_widgets |= 1LL << (WIDX_TAB_1 + w->page);
}

static void window_finances_draw_tab_image(rct_drawpixelinfo *dpi, rct_window *w, int page, int spriteIndex)
{
	int widgetIndex = WIDX_TAB_1 + page;

	if (!(w->disabled_widgets & (1LL << widgetIndex))) {
		if (w->page == page) {
			int frame = w->frame_no / 2;
			if (page == WINDOW_FINANCES_PAGE_SUMMARY)
				frame %= 8;
			spriteIndex += frame;
		}

		gfx_draw_sprite(dpi, spriteIndex, w->x + w->widgets[widgetIndex].left, w->y + w->widgets[widgetIndex].top);
	}
}

static void window_finances_draw_tab_images(rct_drawpixelinfo *dpi, rct_window *w)
{
	window_finances_draw_tab_image(dpi, w, WINDOW_FINANCES_PAGE_SUMMARY, SPR_TAB_FINANCES_SUMMARY_0);
	window_finances_draw_tab_image(dpi, w, WINDOW_FINANCES_PAGE_FINANCIAL_GRAPH, SPR_TAB_FINANCES_FINANCIAL_GRAPH_0);
	window_finances_draw_tab_image(dpi, w, WINDOW_FINANCES_PAGE_VALUE_GRAPH, SPR_TAB_FINANCES_VALUE_GRAPH_0);
	window_finances_draw_tab_image(dpi, w, WINDOW_FINANCES_PAGE_PROFIT_GRAPH, SPR_TAB_FINANCES_PROFIT_GRAPH_0);
	window_finances_draw_tab_image(dpi, w, WINDOW_FINANCES_PAGE_MARKETING, SPR_TAB_FINANCES_MARKETING_0);
	window_finances_draw_tab_image(dpi, w, WINDOW_FINANCES_PAGE_RESEARCH, SPR_TAB_FINANCES_RESEARCH_0);
}

#pragma endregion