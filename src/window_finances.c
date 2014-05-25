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
#include "string_ids.h"
#include "widget.h"
#include "window.h"

enum {
	WINDOW_FINANCES_TAB_SUMMARY,
	WINDOW_FINANCES_TAB_FINANCIAL_GRAPH,
	WINDOW_FINANCES_TAB_VALUE_GRAPH,
	WINDOW_FINANCES_TAB_PROFIT_GRAPH,
	WINDOW_FINANCES_TAB_MARKETING,
	WINDOW_FINANCES_TAB_RESEARCH
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

static void* window_finances_page_events[] = {
	(void*)0x00988EB8,
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
		w->var_48E = 0;
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