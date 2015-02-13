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
#include "../localisation/localisation.h"
#include "../interface/widget.h"
#include "../interface/window.h"
#include "../management/finance.h"
#include "../management/news_item.h"
#include "../management/research.h"
#include "../ride/ride.h"
#include "../sprites.h"
#include "../world/scenery.h"
#include "dropdown.h"

enum {
	WINDOW_RESEARCH_PAGE_DEVELOPMENT,
	WINDOW_RESEARCH_PAGE_FUNDING,
	WINDOW_RESEARCH_PAGE_COUNT
};

enum {
	WIDX_BACKGROUND,
	WIDX_TITLE,
	WIDX_CLOSE,
	WIDX_PAGE_BACKGROUND,
	WIDX_TAB_1,
	WIDX_TAB_2,

	WIDX_CURRENTLY_IN_DEVELOPMENT_GROUP,
	WIDX_LAST_DEVELOPMENT_GROUP,
	WIDX_LAST_DEVELOPMENT_BUTTON,

	WIDX_FUNDING_GROUP = 6,
	WIDX_RESEARCH_FUNDING,
	WIDX_RESEARCH_FUNDING_DROPDOWN_BUTTON,
	WIDX_PRIORITIES_GROUP,
	WIDX_TRANSPORT_RIDES,
	WIDX_GENTLE_RIDES,
	WIDX_ROLLER_COASTERS,
	WIDX_THRILL_RIDES,
	WIDX_WATER_RIDES,
	WIDX_SHOPS_AND_STALLS,
	WIDX_SCENERY_AND_THEMING,
};

#pragma region Widgets

static rct_widget window_research_development_widgets[] = {
	{ WWT_FRAME,			0,	0,		299,	0,		195,	0xFFFFFFFF,								STR_NONE },
	{ WWT_CAPTION,			0,	1,		298,	1,		14,		STR_RESEARCH_AND_DEVELOPMENT,			STR_WINDOW_TITLE_TIP },
	{ WWT_CLOSEBOX,			0,	287,	297,	2,		13,		STR_CLOSE_X,							STR_CLOSE_WINDOW_TIP },
	{ WWT_RESIZE,			1,	0,		299,	43,		195,	0xFFFFFFFF,								STR_NONE },
	{ WWT_TAB,				1,	3,		33,		17,		43,		0x2000144E,								STR_RESEARCH_AND_DEVELOPMENT_TIP },
	{ WWT_TAB,				1,	34,		64,		17,		43,		0x2000144E,								STR_FINANCES_RESEARCH },
	{ WWT_GROUPBOX,			2,	3,		292,	47,		116,	STR_CURRENTLY_IN_DEVELOPMENT,			STR_NONE },
	{ WWT_GROUPBOX,			2,	3,		292,	124,	188,	STR_LAST_DEVELOPMENT,					STR_NONE },
	{ WWT_FLATBTN,			2,	265,	288,	161,	184,	0xFFFFFFFF,								STR_RESEARCH_SHOW_DETAILS_TIP },
	{ WIDGETS_END },
};

static rct_widget window_research_funding_widgets[] = {
	{ WWT_FRAME,			0,	0,		319,	0,		206,	0xFFFFFFFF,								STR_NONE },
	{ WWT_CAPTION,			0,	1,		318,	1,		14,		STR_RESEARCH_FUNDING,					STR_WINDOW_TITLE_TIP },
	{ WWT_CLOSEBOX,			0,	307,	317,	2,		13,		STR_CLOSE_X,							STR_CLOSE_WINDOW_TIP },
	{ WWT_RESIZE,			1,	0,		319,	43,		206,	0xFFFFFFFF,								STR_NONE },
	{ WWT_TAB,				1,	3,		33,		17,		43,		0x2000144E,								STR_RESEARCH_AND_DEVELOPMENT_TIP },
	{ WWT_TAB,				1,	34,		64,		17,		43,		0x2000144E,								STR_FINANCES_RESEARCH },
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

static rct_widget *window_research_page_widgets[] = {
	window_research_development_widgets,
	window_research_funding_widgets
};

#pragma endregion

#pragma region Events

static void window_research_emptysub() { }

static void window_research_development_mouseup();
static void window_research_development_update(rct_window *w);
static void window_research_development_invalidate();
static void window_research_development_paint();

static void window_research_funding_mouseup();
static void window_research_funding_mousedown(int widgetIndex, rct_window*w, rct_widget* widget);
static void window_research_funding_dropdown();
static void window_research_funding_update(rct_window *w);
static void window_research_funding_invalidate();
static void window_research_funding_paint();

// 
static void* window_research_development_events[] = {
	window_research_emptysub,
	window_research_development_mouseup,
	window_research_emptysub,
	window_research_emptysub,
	window_research_emptysub,
	window_research_emptysub,
	window_research_development_update,
	window_research_emptysub,
	window_research_emptysub,
	window_research_emptysub,
	window_research_emptysub,
	window_research_emptysub,
	window_research_emptysub,
	window_research_emptysub,
	window_research_emptysub,
	window_research_emptysub,
	window_research_emptysub,
	window_research_emptysub,
	window_research_emptysub,
	window_research_emptysub,
	window_research_emptysub,
	window_research_emptysub,
	window_research_emptysub,
	window_research_emptysub,
	window_research_emptysub,
	window_research_development_invalidate,
	window_research_development_paint,
	window_research_emptysub
};

// 0x009890E8
static void* window_research_funding_events[] = {
	window_research_emptysub,
	window_research_funding_mouseup,
	window_research_emptysub,
	window_research_funding_mousedown,
	window_research_funding_dropdown,
	window_research_emptysub,
	window_research_funding_update,
	window_research_emptysub,
	window_research_emptysub,
	window_research_emptysub,
	window_research_emptysub,
	window_research_emptysub,
	window_research_emptysub,
	window_research_emptysub,
	window_research_emptysub,
	window_research_emptysub,
	window_research_emptysub,
	window_research_emptysub,
	window_research_emptysub,
	window_research_emptysub,
	window_research_emptysub,
	window_research_emptysub,
	window_research_emptysub,
	window_research_emptysub,
	window_research_emptysub,
	window_research_funding_invalidate,
	window_research_funding_paint,
	window_research_emptysub
};

static void* window_research_page_events[] = {
	window_research_development_events,
	window_research_funding_events
};

#pragma endregion

#pragma region Enabled widgets

static uint32 window_research_page_enabled_widgets[] = {
	(1 << WIDX_CLOSE) |
	(1 << WIDX_TAB_1) |
	(1 << WIDX_TAB_2) |
	(1 << WIDX_LAST_DEVELOPMENT_BUTTON),

	(1 << WIDX_CLOSE) |
	(1 << WIDX_TAB_1) |
	(1 << WIDX_TAB_2) |
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

const int window_research_tab_animation_loops[] = { 16, 16 };

static void window_research_set_page(rct_window *w, int page);
static void window_research_set_pressed_tab(rct_window *w);
static void window_research_draw_tab_images(rct_drawpixelinfo *dpi, rct_window *w);

void window_research_open()
{
	rct_window *w;

	w = window_bring_to_front_by_class(WC_RESEARCH);
	if (w == NULL) {
		w = window_create_auto_pos(530, 257, window_research_page_events[0], WC_RESEARCH, WF_10);
		w->widgets = window_research_page_widgets[0];
		w->enabled_widgets = window_research_page_enabled_widgets[0];
		w->number = 0;
		w->page = 0;
		w->frame_no = 0;
		w->disabled_widgets = 0;
		w->colours[0] = 1;
		w->colours[1] = 19;
		w->colours[2] = 19;
		research_update_uncompleted_types();
	}

	w->page = 0;
	window_invalidate(w);
	w->width = 300;
	w->height = 196;
	window_invalidate(w);

	w->widgets = window_research_page_widgets[0];
	w->enabled_widgets = window_research_page_enabled_widgets[0];
	w->hold_down_widgets = 0;
	w->event_handlers = window_research_page_events[0];
	w->pressed_widgets = 0;
	w->disabled_widgets = 0;
	window_init_scroll_widgets(w);
}

#pragma region Development page

/**
 *
 *  rct2: 0x006B6B38
 */
static void window_research_development_mouseup()
{
	short widgetIndex;
	rct_window *w;

	window_widget_get_registers(w, widgetIndex);

	switch (widgetIndex) {
	case WIDX_CLOSE:
		window_close(w);
		break;
	case WIDX_TAB_1:
	case WIDX_TAB_2:
		window_research_set_page(w, widgetIndex - WIDX_TAB_1);
		break;
	case WIDX_LAST_DEVELOPMENT_BUTTON:
		news_item_open_subject(NEWS_ITEM_RESEARCH, RCT2_GLOBAL(RCT2_ADDRESS_LAST_RESEARCHED_ITEM_SUBJECT, sint32));
		break;
	}
}

/**
 *
 *  rct2: 0x0069CBA6
 */
static void window_research_development_update(rct_window *w)
{
	// Tab animation
	if (++w->frame_no >= window_research_tab_animation_loops[w->page])
		w->frame_no = 0;
	widget_invalidate(w, WIDX_TAB_1);
}

/**
 * 
 *  rct2: 0x006B6819
 */
static void window_research_development_invalidate()
{
	rct_window *w;

	window_get_register(w);

	if (w->widgets != window_research_page_widgets[WINDOW_RESEARCH_PAGE_DEVELOPMENT]) {
		w->widgets = window_research_page_widgets[WINDOW_RESEARCH_PAGE_DEVELOPMENT];
		window_init_scroll_widgets(w);
	}

	window_research_set_pressed_tab(w);

	window_research_development_widgets[WIDX_LAST_DEVELOPMENT_BUTTON].type = WWT_EMPTY;
	uint32 typeId = RCT2_GLOBAL(RCT2_ADDRESS_LAST_RESEARCHED_ITEM_SUBJECT, uint32);
	if (typeId != 0xFFFFFFFF) {
		window_research_development_widgets[WIDX_LAST_DEVELOPMENT_BUTTON].type = WWT_FLATBTN;
		window_research_development_widgets[WIDX_LAST_DEVELOPMENT_BUTTON].image = typeId >= 0x10000 ? 5189 : 5191;
	}
}

/**
 * 
 *  rct2: 0x006B689B
 */
static void window_research_development_paint()
{
	rct_window *w;
	rct_drawpixelinfo *dpi;
	int x, y;
	rct_string_id stringId;

	window_paint_get_registers(w, dpi);

	window_draw_widgets(w, dpi);
	window_research_draw_tab_images(dpi, w);

	x = w->x + 10;
	y = w->y + window_research_development_widgets[WIDX_CURRENTLY_IN_DEVELOPMENT_GROUP].top + 12;

	// Research type
	stringId = STR_RESEARCH_UNKNOWN;
	if (RCT2_GLOBAL(RCT2_ADDRESS_RESEARH_PROGRESS_STAGE, uint8) != 0) {
		stringId = STR_TRANSPORT_RIDE + RCT2_GLOBAL(RCT2_ADDRESS_NEXT_RESEARCH_CATEGORY, uint8);
		if (RCT2_GLOBAL(RCT2_ADDRESS_RESEARH_PROGRESS_STAGE, uint8) != 1) {
			uint32 typeId = RCT2_GLOBAL(RCT2_ADDRESS_NEXT_RESEARCH_ITEM, uint32);
			if (typeId >= 0x10000) {
				rct_ride_type *rideEntry = RCT2_GLOBAL(0x009ACFA4 + (typeId & 0xFF) * 4, rct_ride_type*);
				stringId = rideEntry->var_008 & 0x1000 ?
					rideEntry->name :
					((typeId >> 8) & 0xFF) + 2;
			} else {
				stringId = g_scenerySetEntries[typeId]->name;
			}
		}
	}
	gfx_draw_string_left_wrapped(dpi, &stringId, x, y, 296, STR_RESEARCH_TYPE_LABEL, 0);
	y += 25;

	// Progress
	stringId = 2285 + RCT2_GLOBAL(RCT2_ADDRESS_RESEARH_PROGRESS_STAGE, uint8);
	gfx_draw_string_left_wrapped(dpi, &stringId, x, y, 296, STR_RESEARCH_PROGRESS_LABEL, 0);
	y += 15;

	// Expected
	RCT2_GLOBAL(0x013CE952, uint16) = STR_UNKNOWN;
	if (RCT2_GLOBAL(RCT2_ADDRESS_RESEARH_PROGRESS_STAGE, uint8) != 0) {
		uint16 expectedDay = RCT2_GLOBAL(RCT2_ADDRESS_NEXT_RESEARCH_EXPECTED_DAY, uint8);
		if (expectedDay != 255) {
			RCT2_GLOBAL(0x013CE952 + 2, uint16) = STR_DATE_DAY_1 + expectedDay;
			RCT2_GLOBAL(0x013CE952 + 4, uint16) = STR_MONTH_MARCH + RCT2_GLOBAL(RCT2_ADDRESS_NEXT_RESEARCH_EXPECTED_MONTH, uint8);
			RCT2_GLOBAL(0x013CE952, uint16) = 2289;
		}
	}
	gfx_draw_string_left(dpi, STR_RESEARCH_EXPECTED_LABEL, (void*)0x013CE952, 0, x, y);

	// Last development
	x = w->x + 10;
	y = w->y + window_research_development_widgets[WIDX_LAST_DEVELOPMENT_GROUP].top + 12;

	uint32 typeId = RCT2_GLOBAL(RCT2_ADDRESS_LAST_RESEARCHED_ITEM_SUBJECT, uint32);
	int lastDevelopmentFormat;
	if (typeId != 0xFFFFFFFF) {
		if (typeId >= 0x10000) {
			rct_ride_type *rideEntry = RCT2_GLOBAL(0x009ACFA4 + (typeId & 0xFF) * 4, rct_ride_type*);
			stringId = rideEntry->var_008 & 0x1000 ?
				rideEntry->name :
				((typeId >> 8) & 0xFF) + 2;

			lastDevelopmentFormat = STR_RESEARCH_RIDE_LABEL;
		} else {
			stringId = g_scenerySetEntries[typeId]->name;
			lastDevelopmentFormat = STR_RESEARCH_SCENERY_LABEL;
		}
		gfx_draw_string_left_wrapped(dpi, &stringId, x, y, 266, lastDevelopmentFormat, 0);
	}
}

#pragma endregion

#pragma region Funding page

/**
 * 
 *  rct2: 0x0069DB3F
 */
static void window_research_funding_mouseup()
{
	rct_window * w;
	short widgetIndex;
	int activeResearchTypes;

	window_widget_get_registers(w, widgetIndex);

	switch (widgetIndex) {
	case WIDX_CLOSE:
		window_close(w);
		break;
	case WIDX_TAB_1:
	case WIDX_TAB_2:
		window_research_set_page(w, widgetIndex - WIDX_TAB_1);
		break;
	case WIDX_TRANSPORT_RIDES:
	case WIDX_GENTLE_RIDES:
	case WIDX_ROLLER_COASTERS:
	case WIDX_THRILL_RIDES:
	case WIDX_WATER_RIDES:
	case WIDX_SHOPS_AND_STALLS:
	case WIDX_SCENERY_AND_THEMING:
		activeResearchTypes = RCT2_GLOBAL(RCT2_ADDRESS_ACTIVE_RESEARCH_TYPES, uint16);
		activeResearchTypes ^= 1 << (widgetIndex - WIDX_TRANSPORT_RIDES);
		research_set_priority(activeResearchTypes);
		break;
	}
}

/**
 * 
 *  rct2: 0x0069DB66
 */
static void window_research_funding_mousedown(int widgetIndex, rct_window *w, rct_widget* widget)
{
	rct_widget *dropdownWidget;
	int i;
	
	if (widgetIndex != WIDX_RESEARCH_FUNDING_DROPDOWN_BUTTON)
		return;

	dropdownWidget = widget - 1;

	for (i = 0; i < 4; i++) {
		gDropdownItemsFormat[i] = 1142;
		gDropdownItemsArgs[i] = STR_NO_FUNDING + i;
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

	int currentResearchLevel = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_RESEARCH_LEVEL, uint8);
	gDropdownItemsChecked = (1 << currentResearchLevel);
}

/**
 * 
 *  rct2: 0x0069DB6D
 */
static void window_research_funding_dropdown()
{
	rct_window *w;
	short widgetIndex;
	short dropdownIndex;

	window_dropdown_get_registers(w, widgetIndex, dropdownIndex);

	if (widgetIndex != WIDX_RESEARCH_FUNDING_DROPDOWN_BUTTON || dropdownIndex == -1)
		return;

	research_set_funding(dropdownIndex);
	window_invalidate(w);
}

/**
 * 
 *  rct2: 0x0069DC23
 */
static void window_research_funding_update(rct_window *w)
{
	// Tab animation
	if (++w->frame_no >= window_research_tab_animation_loops[w->page])
		w->frame_no = 0;
	widget_invalidate(w, WIDX_TAB_2);
}

/**
 * 
 *  rct2: 0x0069DA64
 */
static void window_research_funding_invalidate()
{
	rct_window *w;

	window_get_register(w);

	if (w->widgets != window_research_page_widgets[WINDOW_RESEARCH_PAGE_FUNDING]) {
		w->widgets = window_research_page_widgets[WINDOW_RESEARCH_PAGE_FUNDING];
		window_init_scroll_widgets(w);
	}

	window_research_set_pressed_tab(w);

	if ((RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & PARK_FLAGS_NO_MONEY)) {
		window_research_funding_widgets[WIDX_FUNDING_GROUP].type = WWT_EMPTY;
		window_research_funding_widgets[WIDX_RESEARCH_FUNDING].type = WWT_EMPTY;
		window_research_funding_widgets[WIDX_RESEARCH_FUNDING_DROPDOWN_BUTTON].type = WWT_EMPTY;
	} else {
		window_research_funding_widgets[WIDX_FUNDING_GROUP].type = WWT_GROUPBOX;
		window_research_funding_widgets[WIDX_RESEARCH_FUNDING].type = WWT_DROPDOWN;
		window_research_funding_widgets[WIDX_RESEARCH_FUNDING_DROPDOWN_BUTTON].type = WWT_DROPDOWN_BUTTON;

		// Current funding
		int currentResearchLevel = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_RESEARCH_LEVEL, uint8);
		window_research_funding_widgets[WIDX_RESEARCH_FUNDING].image = STR_NO_FUNDING + currentResearchLevel;
	}
	
	// Checkboxes
	int activeResearchTypes = RCT2_GLOBAL(RCT2_ADDRESS_ACTIVE_RESEARCH_TYPES, uint16);
	int uncompletedResearchTypes = gResearchUncompletedCategories;
	for (int i = 0; i < 7; i++) {
		int mask = 1 << i;
		int widgetMask = 1 << (i + WIDX_TRANSPORT_RIDES);

		// Set checkbox disabled if research type is complete
		if (gResearchUncompletedCategories & mask) {
			w->disabled_widgets &= ~widgetMask;

			// Set checkbox ticked if research type is active
			if (activeResearchTypes & mask)
				w->pressed_widgets |= widgetMask;
			else
				w->pressed_widgets &= ~widgetMask;
		} else {
			w->disabled_widgets |= widgetMask;
			w->pressed_widgets &= ~widgetMask;
		}
	}
}

/**
 * 
 *  rct2: 0x0069DAF0
 */
static void window_research_funding_paint()
{
	rct_window *w;
	rct_drawpixelinfo *dpi;

	window_paint_get_registers(w, dpi);

	window_draw_widgets(w, dpi);
	window_research_draw_tab_images(dpi, w);

	if (!(RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & PARK_FLAGS_NO_MONEY)) {
		int currentResearchLevel = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_RESEARCH_LEVEL, uint8);
		money32 currentResearchCostPerWeek = research_cost_table[currentResearchLevel];
		gfx_draw_string_left(dpi, STR_RESEARCH_COST_PER_MONTH, &currentResearchCostPerWeek, 0, w->x + 10, w->y + 77);
	}
}

#pragma endregion

#pragma region Common

/**
 *
 *  rct2: 0x0069CAC5
 */
static void window_research_set_page(rct_window *w, int page)
{
	w->page = page;
	w->frame_no = 0;
	if (w->viewport != NULL) {
		w->viewport->width = 0;
		w->viewport = NULL;
	}

	w->enabled_widgets = window_research_page_enabled_widgets[page];
	w->hold_down_widgets = 0;
	w->event_handlers = window_research_page_events[page];
	w->widgets = window_research_page_widgets[page];
	w->disabled_widgets = 0;
	w->pressed_widgets = 0;
	
	window_invalidate(w);
	if (w->page == WINDOW_RESEARCH_PAGE_DEVELOPMENT) {
		w->width = 300;
		w->height = 196;
	} else {
		w->width = 320;
		w->height = 207;
	}
	RCT2_CALLPROC_X(w->event_handlers[WE_RESIZE], 0, 0, 0, 0, (int)w, 0, 0);
	RCT2_CALLPROC_X(w->event_handlers[WE_INVALIDATE], 0, 0, 0, 0, (int)w, 0, 0);

	window_init_scroll_widgets(w);
	window_invalidate(w);
}

static void window_research_set_pressed_tab(rct_window *w)
{
	int i;
	for (i = 0; i < WINDOW_RESEARCH_PAGE_COUNT; i++)
		w->pressed_widgets &= ~(1 << (WIDX_TAB_1 + i));
	w->pressed_widgets |= 1LL << (WIDX_TAB_1 + w->page);
}

static void window_research_draw_tab_image(rct_drawpixelinfo *dpi, rct_window *w, int page, int spriteIndex)
{
	int widgetIndex = WIDX_TAB_1 + page;

	if (!(w->disabled_widgets & (1LL << widgetIndex))) {
		if (w->page == page) {
			int frame = w->frame_no / 2;
			if (page == WINDOW_RESEARCH_PAGE_DEVELOPMENT)
				frame %= 8;
			spriteIndex += frame;
		}

		gfx_draw_sprite(dpi, spriteIndex, w->x + w->widgets[widgetIndex].left, w->y + w->widgets[widgetIndex].top, 0);
	}
}

static void window_research_draw_tab_images(rct_drawpixelinfo *dpi, rct_window *w)
{
	window_research_draw_tab_image(dpi, w, WINDOW_RESEARCH_PAGE_DEVELOPMENT, SPR_TAB_FINANCES_RESEARCH_0);
	window_research_draw_tab_image(dpi, w, WINDOW_RESEARCH_PAGE_FUNDING, SPR_TAB_FINANCES_SUMMARY_0);
}

#pragma endregion