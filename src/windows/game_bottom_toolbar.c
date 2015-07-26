/*****************************************************************************
 * Copyright (c) 2014 Ted John, Peter Hill
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
#include "../localisation/date.h"
#include "../localisation/localisation.h"
#include "../interface/widget.h"
#include "../interface/window.h"
#include "../management/news_item.h"
#include "../peep/peep.h"
#include "../peep/staff.h"
#include "../sprites.h"
#include "../world/climate.h"
#include "../world/park.h"
#include "../world/sprite.h"
#include "../interface/themes.h"

enum WINDOW_GAME_BOTTOM_TOOLBAR_WIDGET_IDX {
	WIDX_LEFT_OUTSET,
	WIDX_LEFT_INSET,
	WIDX_MONEY,
	WIDX_GUESTS,
	WIDX_PARK_RATING,

	WIDX_MIDDLE_OUTSET,
	WIDX_MIDDLE_INSET,
	WIDX_NEWS_SUBJECT,
	WIDX_NEWS_LOCATE,

	WIDX_RIGHT_OUTSET,
	WIDX_RIGHT_INSET,
	WIDX_DATE
};


// Right panel needs to be a bit bigger than original so dates like "22nd September, Year 126" can fit.
// Left panel size was also increased for symmetry.
#define WIDTH_MOD 22

rct_widget window_game_bottom_toolbar_widgets[] = {
	{ WWT_IMGBTN,	0,	0x0000,	0x0077+WIDTH_MOD,	0,		33,		0xFFFFFFFF,	STR_NONE },	// Left outset panel
	{ WWT_IMGBTN,	0,	0x0002,	0x0075+WIDTH_MOD,	2,		31,		0xFFFFFFFF,	STR_NONE },	// Left inset panel
	{ WWT_FLATBTN,	0,	0x0002,	0x0075+WIDTH_MOD,	1,		12,		0xFFFFFFFF,	STR_PROFIT_PER_WEEK_AND_PARK_VALUE_TIP },	// Money window
	{ WWT_FLATBTN,	0,	0x0002,	0x0075+WIDTH_MOD,	11,		22,		0xFFFFFFFF,	STR_NONE },	// Guests window
	{ WWT_FLATBTN,	0,	0x0002,	0x0075+WIDTH_MOD,	21,		31,		0xFFFFFFFF,	STR_PARK_RATING_TIP },	// Park rating window

	{ WWT_IMGBTN,	2,	0x0078+WIDTH_MOD,	0x0207-WIDTH_MOD,	0,		33,		0xFFFFFFFF,	STR_NONE },	// Middle outset panel
	{ WWT_25,		2,	0x007A+WIDTH_MOD,	0x0205-WIDTH_MOD,	2,		31,		0xFFFFFFFF,	STR_NONE },	// Middle inset panel
	{ WWT_FLATBTN,	2,	0x007D+WIDTH_MOD,	0x0094+WIDTH_MOD,	5,		28,		0xFFFFFFFF,	STR_SHOW_SUBJECT_TIP },	// Associated news item window
	{ WWT_FLATBTN,	2,	0x01EB-WIDTH_MOD,	0x0202-WIDTH_MOD,	5,		28,		SPR_LOCATE,	STR_LOCATE_SUBJECT_TIP },	// Scroll to news item target

	{ WWT_IMGBTN,	0,	0x0208-WIDTH_MOD,	0x027F,	0,		33,		0xFFFFFFFF,	STR_NONE },	// Right outset panel
	{ WWT_IMGBTN,	0,	0x020A-WIDTH_MOD,	0x027D,	2,		31,		0xFFFFFFFF,	STR_NONE },	// Right inset panel
	{ WWT_FLATBTN,	0,	0x020A-WIDTH_MOD,	0x027D,	2,		13,		0xFFFFFFFF,	2290 },	// Date
	{ WIDGETS_END },
};

static void window_game_bottom_toolbar_mouseup(rct_window *w, int widgetIndex);
static void window_game_bottom_toolbar_tooltip(rct_window* w, int widgetIndex, rct_string_id *stringId);
static void window_game_bottom_toolbar_invalidate(rct_window *w);
static void window_game_bottom_toolbar_paint(rct_window *w, rct_drawpixelinfo *dpi);
static void window_game_bottom_toolbar_update(rct_window* w);
static void window_game_bottom_toolbar_cursor(rct_window *w, int widgetIndex, int x, int y, int *cursorId);
static void window_game_bottom_toolbar_unknown05(rct_window *w);

static void window_game_bottom_toolbar_draw_left_panel(rct_drawpixelinfo *dpi, rct_window *w);
static void window_game_bottom_toolbar_draw_park_rating(rct_drawpixelinfo *dpi, rct_window *w, int colour, int x, int y, uint8 factor);
static void window_game_bottom_toolbar_draw_right_panel(rct_drawpixelinfo *dpi, rct_window *w);
static void window_game_bottom_toolbar_draw_news_item(rct_drawpixelinfo *dpi, rct_window *w);
static void window_game_bottom_toolbar_draw_tutorial_text(rct_drawpixelinfo *dpi, rct_window *w);

/* rct2: 0x0097BFDC */
static rct_window_event_list window_game_bottom_toolbar_events = {
	NULL,
	window_game_bottom_toolbar_mouseup,
	NULL,
	NULL,
	NULL,
	window_game_bottom_toolbar_unknown05,
	window_game_bottom_toolbar_update,
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
	window_game_bottom_toolbar_tooltip,
	window_game_bottom_toolbar_cursor,
	NULL,
	window_game_bottom_toolbar_invalidate,
	window_game_bottom_toolbar_paint,
	NULL
};

static void window_game_bottom_toolbar_invalidate_dirty_widgets(rct_window *w);

/**
 * Creates the main game bottom toolbar window.
 *  rct2: 0x0066B52F (part of 0x0066B3E8)
 */
void window_game_bottom_toolbar_open()
{
	rct_window* window;

	window = window_create(
		0, RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_HEIGHT, uint16) - 32,
		RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_WIDTH, uint16), 32,
		&window_game_bottom_toolbar_events,
		WC_BOTTOM_TOOLBAR,
		WF_STICK_TO_FRONT | WF_TRANSPARENT | WF_5
	);
	window->widgets = window_game_bottom_toolbar_widgets;
	window->enabled_widgets |=
		(1 << WIDX_LEFT_OUTSET) |
		(1 << WIDX_MONEY) |
		(1 << WIDX_GUESTS) |
		(1 << WIDX_PARK_RATING) |
		(1 << WIDX_MIDDLE_OUTSET) |
		(1 << WIDX_MIDDLE_INSET) |
		(1 << WIDX_NEWS_SUBJECT) |
		(1 << WIDX_NEWS_LOCATE) |
		(1 << WIDX_RIGHT_OUTSET) |
		(1 << WIDX_DATE);

	window->frame_no = 0;
	window_init_scroll_widgets(window);

	// Reset the middle widget to not show by default.
	// If it is required to be shown news_update will reshow it.
	window_game_bottom_toolbar_widgets[WIDX_MIDDLE_OUTSET].type = WWT_EMPTY;
}

/**
 * 
 *  rct2: 0x0066C588
 */
static void window_game_bottom_toolbar_mouseup(rct_window *w, int widgetIndex)
{
	rct_window *mainWindow;
	rct_news_item *newsItem;

	switch (widgetIndex) {
	case WIDX_LEFT_OUTSET:
	case WIDX_MONEY:
		if (!(RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & PARK_FLAGS_NO_MONEY))
			window_finances_open();
		break;
	case WIDX_GUESTS:
		window_park_guests_open();
		break;
	case WIDX_PARK_RATING:
		window_park_rating_open();
		break;
	case WIDX_MIDDLE_INSET:
		news_item_close_current();
		break;
	case WIDX_NEWS_SUBJECT:
		newsItem = news_item_get(0);
		news_item_open_subject(newsItem->type, newsItem->assoc);
		break;
	case WIDX_NEWS_LOCATE:
		if (news_item_is_queue_empty())
			break;

		{
			newsItem = news_item_get(0);
			int x, y, z;
			int subject = newsItem->assoc;

			news_item_get_subject_location(newsItem->type, subject, &x, &y, &z);

			if (x == SPRITE_LOCATION_NULL)
				break;

			if ((mainWindow = window_get_main()) != NULL)
				window_scroll_to_location(mainWindow, x, y, z);
		}
		break;
	case WIDX_RIGHT_OUTSET:
	case WIDX_DATE:
		window_news_open();
		break;
	}
}

static void window_game_bottom_toolbar_tooltip(rct_window* w, int widgetIndex, rct_string_id *stringId)
{
	int month, day;

	switch (widgetIndex) {
	case WIDX_MONEY:
		RCT2_GLOBAL(0x013CE952, int) = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_PROFIT, sint32);
		RCT2_GLOBAL(0x013CE956, int) = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_PARK_VALUE, sint32);
		break;
	case WIDX_PARK_RATING:
		RCT2_GLOBAL(0x013CE952, short) = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_PARK_RATING, sint16);
		break;
	case WIDX_DATE:
		month = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_MONTH_YEAR, sint16) & 7;
		day = ((RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_MONTH_TICKS, uint16) * days_in_month[month]) >> 16) & 0xFF;
		
		RCT2_GLOBAL(0x013CE952, short) = STR_DATE_DAY_1 + day;
		RCT2_GLOBAL(0x013CE954, short) = STR_MONTH_MARCH + month;
		break;
	}
}

/**
 * 
 *  rct2: 0x0066BBA0
 */
static void window_game_bottom_toolbar_invalidate(rct_window *w)
{
	int x;
	rct_news_item *newsItem;

	colour_scheme_update(w);

	// Anchor the middle and right panel to the right
	x = RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_WIDTH, uint16);
	w->width = x;
	x--;
	window_game_bottom_toolbar_widgets[WIDX_RIGHT_OUTSET].right = x;
	x -= 2;
	window_game_bottom_toolbar_widgets[WIDX_RIGHT_INSET].right = x;
	x -= (115 + WIDTH_MOD);
	window_game_bottom_toolbar_widgets[WIDX_RIGHT_INSET].left = x;
	x -= 2;
	window_game_bottom_toolbar_widgets[WIDX_RIGHT_OUTSET].left = x;
	x--;
	window_game_bottom_toolbar_widgets[WIDX_MIDDLE_OUTSET].right = x;
	x -= 2;
	window_game_bottom_toolbar_widgets[WIDX_MIDDLE_INSET].right = x;
	x -= 3;
	window_game_bottom_toolbar_widgets[WIDX_NEWS_LOCATE].right = x;
	x -= 23;
	window_game_bottom_toolbar_widgets[WIDX_NEWS_LOCATE].left = x;
	window_game_bottom_toolbar_widgets[WIDX_DATE].left = window_game_bottom_toolbar_widgets[WIDX_RIGHT_OUTSET].left + 2;
	window_game_bottom_toolbar_widgets[WIDX_DATE].right = window_game_bottom_toolbar_widgets[WIDX_RIGHT_OUTSET].right - 2;

	window_game_bottom_toolbar_widgets[WIDX_LEFT_INSET].type = WWT_EMPTY;
	window_game_bottom_toolbar_widgets[WIDX_RIGHT_INSET].type = WWT_EMPTY;

	if (news_item_is_queue_empty()) {
		window_game_bottom_toolbar_widgets[WIDX_MIDDLE_INSET].type = WWT_EMPTY;
		window_game_bottom_toolbar_widgets[WIDX_NEWS_SUBJECT].type = WWT_EMPTY;
		window_game_bottom_toolbar_widgets[WIDX_NEWS_LOCATE].type = WWT_EMPTY;
	} else {
		newsItem = news_item_get(0);
		window_game_bottom_toolbar_widgets[WIDX_MIDDLE_INSET].type = WWT_25;
		window_game_bottom_toolbar_widgets[WIDX_NEWS_SUBJECT].type = WWT_FLATBTN;
		window_game_bottom_toolbar_widgets[WIDX_NEWS_LOCATE].type = WWT_FLATBTN;
		w->disabled_widgets &= ~(1 << WIDX_NEWS_SUBJECT);
		w->disabled_widgets &= ~(1 << WIDX_NEWS_LOCATE);

		// Find out if the news item is no longer valid
		int x, y, z;
		int subject = newsItem->assoc;
		news_item_get_subject_location(newsItem->type, subject, &x, &y, &z);

		if (x == SPRITE_LOCATION_NULL)
			w->disabled_widgets |= (1 << WIDX_NEWS_LOCATE);

		if (!(((char*)0x0097BE7C)[newsItem->type] & 2)) {
			w->disabled_widgets |= (1 << WIDX_NEWS_SUBJECT);
			window_game_bottom_toolbar_widgets[WIDX_NEWS_SUBJECT].type = WWT_EMPTY;
		}

		if (newsItem->flags & 1) {
			w->disabled_widgets |= (1 << WIDX_NEWS_SUBJECT);
			w->disabled_widgets |= (1 << WIDX_NEWS_LOCATE);
		}
	}

	// Hide money if there is no money
	if (RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & PARK_FLAGS_NO_MONEY) {
		window_game_bottom_toolbar_widgets[WIDX_MONEY].type = WWT_EMPTY;
		window_game_bottom_toolbar_widgets[WIDX_GUESTS].top = 1;
		window_game_bottom_toolbar_widgets[WIDX_GUESTS].bottom = 17;
		window_game_bottom_toolbar_widgets[WIDX_PARK_RATING].top = 17;
	} else {
		window_game_bottom_toolbar_widgets[WIDX_MONEY].type = WWT_FLATBTN;
		window_game_bottom_toolbar_widgets[WIDX_GUESTS].top = 11;
		window_game_bottom_toolbar_widgets[WIDX_GUESTS].bottom = 22;
		window_game_bottom_toolbar_widgets[WIDX_PARK_RATING].top = 21;
	}
}

/**
 * 
 *  rct2: 0x0066BB79
 */
void window_game_bottom_toolbar_invalidate_news_item()
{
	window_game_bottom_toolbar_widgets[WIDX_MIDDLE_OUTSET].type =
		news_item_is_queue_empty() ? WWT_EMPTY : WWT_IMGBTN;
	widget_invalidate_by_class(WC_BOTTOM_TOOLBAR, WIDX_MIDDLE_OUTSET);
}

/**
 * 
 *  rct2: 0x0066BC87
 */
static void window_game_bottom_toolbar_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
	// Draw panel grey backgrounds
	gfx_fill_rect(
		dpi,
		w->x + window_game_bottom_toolbar_widgets[WIDX_LEFT_OUTSET].left,
		w->y + window_game_bottom_toolbar_widgets[WIDX_LEFT_OUTSET].top,
		w->x + window_game_bottom_toolbar_widgets[WIDX_LEFT_OUTSET].right,
		w->y + window_game_bottom_toolbar_widgets[WIDX_LEFT_OUTSET].bottom,
		0x02000033
	);
	gfx_fill_rect(
		dpi,
		w->x + window_game_bottom_toolbar_widgets[WIDX_RIGHT_OUTSET].left,
		w->y + window_game_bottom_toolbar_widgets[WIDX_RIGHT_OUTSET].top,
		w->x + window_game_bottom_toolbar_widgets[WIDX_RIGHT_OUTSET].right,
		w->y + window_game_bottom_toolbar_widgets[WIDX_RIGHT_OUTSET].bottom,
		0x02000033
	);

	window_draw_widgets(w, dpi);

	window_game_bottom_toolbar_draw_left_panel(dpi, w);
	window_game_bottom_toolbar_draw_right_panel(dpi, w);

	if (!news_item_is_queue_empty())
		window_game_bottom_toolbar_draw_news_item(dpi, w);
	else if (RCT2_GLOBAL(RCT2_ADDRESS_ON_TUTORIAL, uint8))
		window_game_bottom_toolbar_draw_tutorial_text(dpi, w);
}

static void window_game_bottom_toolbar_draw_left_panel(rct_drawpixelinfo *dpi, rct_window *w)
{
	int x, y;

	// Draw green inset rectangle on panel
	gfx_fill_rect_inset(
		dpi,
		w->x + window_game_bottom_toolbar_widgets[WIDX_LEFT_OUTSET].left + 1,
		w->y + window_game_bottom_toolbar_widgets[WIDX_LEFT_OUTSET].top + 1,
		w->x + window_game_bottom_toolbar_widgets[WIDX_LEFT_OUTSET].right - 1,
		w->y + window_game_bottom_toolbar_widgets[WIDX_LEFT_OUTSET].bottom - 1,
		w->colours[1],
		48
	);

	x = (window_game_bottom_toolbar_widgets[WIDX_LEFT_OUTSET].left + window_game_bottom_toolbar_widgets[WIDX_LEFT_OUTSET].right) / 2 + w->x;
	y = window_game_bottom_toolbar_widgets[WIDX_LEFT_OUTSET].top + w->y + 4;

	// Draw money
	if (!(RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & PARK_FLAGS_NO_MONEY)) {
		RCT2_GLOBAL(0x013CE952, int) = DECRYPT_MONEY(RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_MONEY_ENCRYPTED, sint32));
		gfx_draw_string_centred(
			dpi,
			(RCT2_GLOBAL(0x013CE952, int) < 0 ? 1391 : 1390),
			x, y - 3,
			(RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_OVER_WINDOWCLASS, rct_windowclass) == 2 && RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_OVER_WIDGETINDEX, sint32) == WIDX_MONEY ? 2 : w->colours[0] & 0x7F),
			(void*)0x013CE952
			);
		y += 7;
	}

	// Draw guests
	gfx_draw_string_centred(
		dpi,
		STR_NUM_GUESTS + RCT2_GLOBAL(RCT2_ADDRESS_GUEST_CHANGE_MODIFIER, uint8),
		x, y,
		(RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_OVER_WINDOWCLASS, rct_windowclass) == 2 && RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_OVER_WIDGETINDEX, sint32) == WIDX_GUESTS ? 2 : w->colours[0] & 0x7F),
		(void*)RCT2_ADDRESS_GUESTS_IN_PARK
	);

	// Draw park rating
	window_game_bottom_toolbar_draw_park_rating(
		dpi,
		w,
		w->colours[3],
		w->x + window_game_bottom_toolbar_widgets[WIDX_PARK_RATING].left + 11,
		w->y + window_game_bottom_toolbar_widgets[WIDX_PARK_RATING].top,
		max(10, ((RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_PARK_RATING, sint16) / 4) * 263) / 256)
	);
}

/**
 * 
 *  rct2: 0x0066C76C
 */
static void window_game_bottom_toolbar_draw_park_rating(rct_drawpixelinfo *dpi, rct_window *w, int colour, int x, int y, uint8 factor)
{
	short bar_width;

	bar_width = (factor * (90 + WIDTH_MOD)) / 256;
	gfx_fill_rect_inset(dpi, x, y + 1, x + (93 + WIDTH_MOD), y + 9, w->colours[1], 48);
	if (!(colour & 0x80000000) || RCT2_GLOBAL(RCT2_ADDRESS_GAME_PAUSED, uint8) != 0 || (RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_TICKS, uint8) & 8)) {
		if (bar_width > 2)
			gfx_fill_rect_inset(dpi, x + 2, y + 2, x + bar_width - 1, y + 8, colour & 0x7FFFFFFF, 0);
	}

	// Draw thumbs on the sides
	gfx_draw_sprite(dpi, SPR_RATING_LOW, x - 14, y, 0);
	gfx_draw_sprite(dpi, SPR_RATING_HIGH, x + (92 + WIDTH_MOD), y, 0);
}

static void window_game_bottom_toolbar_draw_right_panel(rct_drawpixelinfo *dpi, rct_window *w)
{
	int x, y, temperature, format;

	// Draw green inset rectangle on panel
	gfx_fill_rect_inset(
		dpi,
		w->x + window_game_bottom_toolbar_widgets[WIDX_RIGHT_OUTSET].left + 1,
		w->y + window_game_bottom_toolbar_widgets[WIDX_RIGHT_OUTSET].top + 1,
		w->x + window_game_bottom_toolbar_widgets[WIDX_RIGHT_OUTSET].right - 1,
		w->y + window_game_bottom_toolbar_widgets[WIDX_RIGHT_OUTSET].bottom - 1,
		w->colours[1],
		48
	);

	x = (window_game_bottom_toolbar_widgets[WIDX_RIGHT_OUTSET].left + window_game_bottom_toolbar_widgets[WIDX_RIGHT_OUTSET].right) / 2 + w->x;
	y = window_game_bottom_toolbar_widgets[WIDX_RIGHT_OUTSET].top + w->y + 2;

	// Date
	int year = date_get_year(RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_MONTH_YEAR, sint16)) + 1;
	int month = date_get_month(RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_MONTH_YEAR, sint16) & 7);
	int day = ((RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_MONTH_TICKS, uint16) * days_in_month[month]) >> 16) & 0xFF;
	if (gConfigGeneral.date_format) {
		RCT2_GLOBAL(0x013CE952, short) = month;
		RCT2_GLOBAL(0x013CE954, short) = STR_DATE_DAY_1 + day;
	}
	else {
		RCT2_GLOBAL(0x013CE952, short) = STR_DATE_DAY_1 + day;
		RCT2_GLOBAL(0x013CE954, short) = month;
	}
	
	RCT2_GLOBAL(0x013CE956, short) = year;
	gfx_draw_string_centred(
		dpi,
		(gConfigGeneral.date_format ? 5160 : 2737),
		x,
		y,
		(RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_OVER_WINDOWCLASS, rct_windowclass) == 2 && RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_OVER_WIDGETINDEX, sint32) == WIDX_DATE ? 2 : w->colours[0] & 0x7F),
		(void*)0x013CE952
	);

	// Temperature
	x = w->x + window_game_bottom_toolbar_widgets[WIDX_RIGHT_OUTSET].left + 15;
	y += 11;

	temperature = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_TEMPERATURE, sint8);
	format = STR_CELSIUS_VALUE;
	if (RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_TEMPERATURE, uint8)) {
		temperature = climate_celsius_to_fahrenheit(temperature);
		format = STR_FAHRENHEIT_VALUE;
	}
	RCT2_GLOBAL(0x013CE952, short) = temperature;
	gfx_draw_string_left(dpi, format, (void*)0x013CE952, 0, x, y + 6);
	x += 30;

	// Current weather
	gfx_draw_sprite(dpi, climate_weather_data[RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_WEATHER, sint8)].sprite_id, x, y, 0);

	// Next weather
	if (climate_weather_data[RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_WEATHER, sint8)].sprite_id != climate_weather_data[gClimateNextWeather].sprite_id) {
		if (RCT2_GLOBAL(RCT2_ADDRESS_CLIMATE_UPDATE_TIMER, sint16) < 960) {
			gfx_draw_sprite(dpi, SPR_NEXT_WEATHER, x + 27, y + 5, 0);
			gfx_draw_sprite(dpi, climate_weather_data[gClimateNextWeather].sprite_id, x + 40, y, 0);
		}
	}
}

/**
 * 
 *  rct2: 0x0066BFA5
 */
static void window_game_bottom_toolbar_draw_news_item(rct_drawpixelinfo *dpi, rct_window *w)
{
	int x, y, width;
	rct_string_id stringId;
	rct_news_item *newsItem;
	rct_widget *middleOutsetWidget;

	middleOutsetWidget = &window_game_bottom_toolbar_widgets[WIDX_MIDDLE_OUTSET];
	newsItem = news_item_get(0);

	// Current news item
	gfx_fill_rect_inset(
		dpi,
		w->x + middleOutsetWidget->left + 1,
		w->y + middleOutsetWidget->top + 1,
		w->x + middleOutsetWidget->right - 1,
		w->y + middleOutsetWidget->bottom - 1,
		w->colours[2],
		48
	);

	// Text
	stringId = 1926;
	utf8 *buffer = (utf8*)0x009B5F2C;
	memcpy(buffer, &newsItem->text, 256);
	x = w->x + (middleOutsetWidget->left + middleOutsetWidget->right) / 2;
	y = w->y + middleOutsetWidget->top + 11;
	width = middleOutsetWidget->right - middleOutsetWidget->left - 62;
	sub_6C1F57(dpi, x, y, width, 14, stringId, NULL, newsItem->ticks);

	x = w->x + window_game_bottom_toolbar_widgets[WIDX_NEWS_SUBJECT].left;
	y = w->y + window_game_bottom_toolbar_widgets[WIDX_NEWS_SUBJECT].top;
	switch (newsItem->type) {
	case NEWS_ITEM_RIDE:
		gfx_draw_sprite(dpi, SPR_RIDE, x, y, 0);
		break;
	case NEWS_ITEM_PEEP_ON_RIDE:
	case NEWS_ITEM_PEEP:
		if (newsItem->flags & 1)
			break;

		rct_drawpixelinfo* cliped_dpi = clip_drawpixelinfo(dpi, x + 1, 22, y + 1, 22);
		if (!cliped_dpi) break;

		rct_peep* peep = GET_PEEP(newsItem->assoc);
		int clip_x = 10, clip_y = 19;

		if (peep->type == PEEP_TYPE_STAFF){
			if (peep->staff_type == STAFF_TYPE_ENTERTAINER){
				clip_y += 3;
			}
		}

		uint32 image_id_base = *RCT2_ADDRESS(0x00982708, uint32*)[peep->sprite_type * 2];
		image_id_base += w->frame_no & 0xFFFFFFFC;
		image_id_base++;

		uint32 image_id = image_id_base;
		image_id |= 0xA0000000 | (peep->tshirt_colour << 19) | (peep->trousers_colour << 24);

		gfx_draw_sprite(cliped_dpi, image_id, clip_x, clip_y, 0);

		if (image_id_base >= 0x2A1D && image_id_base < 0x2A3D){
			image_id_base += 32;
			image_id_base |= 0x20000000 | (peep->balloon_colour << 19);

			gfx_draw_sprite(cliped_dpi, image_id_base, clip_x, clip_y, 0);
		}
		else if (image_id_base >= 0x2BBD && image_id_base < 0x2BDD){
			image_id_base += 32;
			image_id_base |= 0x20000000 | (peep->umbrella_colour << 19);

			gfx_draw_sprite(cliped_dpi, image_id_base, clip_x, clip_y, 0);
		}
		else if (image_id_base >= 0x29DD && image_id_base < 0x29FD){
			image_id_base += 32;
			image_id_base |= 0x20000000 | (peep->hat_colour << 19);

			gfx_draw_sprite(cliped_dpi, image_id_base, clip_x, clip_y, 0);
		}

		rct2_free(cliped_dpi);
		break;
	case NEWS_ITEM_MONEY:
		gfx_draw_sprite(dpi, SPR_FINANCE, x, y, 0);
		break;
	case NEWS_ITEM_RESEARCH:
		gfx_draw_sprite(dpi, (newsItem->assoc < 0x10000 ? SPR_NEW_RIDE : SPR_SCENERY), x, y, 0);
		break;
	case NEWS_ITEM_PEEPS:
		gfx_draw_sprite(dpi, SPR_GUESTS, x, y, 0);
		break;
	case NEWS_ITEM_AWARD:
		gfx_draw_sprite(dpi, SPR_AWARD, x, y, 0);
		break;
	case NEWS_ITEM_GRAPH:
		gfx_draw_sprite(dpi, SPR_GRAPH, x, y, 0);
		break;
	}
}

static void window_game_bottom_toolbar_draw_tutorial_text(rct_drawpixelinfo *dpi, rct_window *w)
{
	int x, y;

	x = (window_game_bottom_toolbar_widgets[WIDX_MIDDLE_OUTSET].left + window_game_bottom_toolbar_widgets[WIDX_MIDDLE_OUTSET].right) / 2 + w->x;
	y = window_game_bottom_toolbar_widgets[WIDX_MIDDLE_OUTSET].top + w->y + 2;
	gfx_draw_string_centred(dpi, STR_TUTORIAL, x, y, 32, 0);
	gfx_draw_string_centred(dpi, STR_PRESS_KEY_OR_MOUSE_BUTTON_FOR_CONTROL, x, y + 10, 32, 0);
}

/* rct2: 0x0066C6D8 */
static void window_game_bottom_toolbar_update(rct_window* w){

	w->frame_no++;
	if (w->frame_no >= 24)
		w->frame_no = 0;

	window_game_bottom_toolbar_invalidate_dirty_widgets(w);
}

/* rct2: 0x006C644 */
static void window_game_bottom_toolbar_cursor(rct_window *w, int widgetIndex, int x, int y, int *cursorId)
{
	switch (widgetIndex) {
	case WIDX_MONEY:
	case WIDX_GUESTS:
	case WIDX_PARK_RATING:
	case WIDX_DATE:
		RCT2_GLOBAL(RCT2_ADDRESS_TOOLTIP_TIMEOUT, uint16) = 2000;
		break;
	}
}

/* rct2: 0x0066C6F2 */
static void window_game_bottom_toolbar_unknown05(rct_window *w)
{
	window_game_bottom_toolbar_invalidate_dirty_widgets(w);
}

/* rct2: 0x0066C6F2 */
static void window_game_bottom_toolbar_invalidate_dirty_widgets(rct_window *w)
{
	if (RCT2_GLOBAL(RCT2_ADDRESS_BTM_TOOLBAR_DIRTY_FLAGS, uint16) & BTM_TB_DIRTY_FLAG_MONEY){
		RCT2_GLOBAL(RCT2_ADDRESS_BTM_TOOLBAR_DIRTY_FLAGS, uint16) &= ~BTM_TB_DIRTY_FLAG_MONEY;
		widget_invalidate(w, WIDX_LEFT_INSET);
	}

	if (RCT2_GLOBAL(RCT2_ADDRESS_BTM_TOOLBAR_DIRTY_FLAGS, uint16) & BTM_TB_DIRTY_FLAG_DATE){
		RCT2_GLOBAL(RCT2_ADDRESS_BTM_TOOLBAR_DIRTY_FLAGS, uint16) &= ~BTM_TB_DIRTY_FLAG_DATE;
		widget_invalidate(w, WIDX_RIGHT_INSET);
	}

	if (RCT2_GLOBAL(RCT2_ADDRESS_BTM_TOOLBAR_DIRTY_FLAGS, uint16) & BTM_TB_DIRTY_FLAG_PEEP_COUNT){
		RCT2_GLOBAL(RCT2_ADDRESS_BTM_TOOLBAR_DIRTY_FLAGS, uint16) &= ~BTM_TB_DIRTY_FLAG_PEEP_COUNT;
		widget_invalidate(w, WIDX_LEFT_INSET);
	}

	if (RCT2_GLOBAL(RCT2_ADDRESS_BTM_TOOLBAR_DIRTY_FLAGS, uint16) & BTM_TB_DIRTY_FLAG_CLIMATE){
		RCT2_GLOBAL(RCT2_ADDRESS_BTM_TOOLBAR_DIRTY_FLAGS, uint16) &= ~BTM_TB_DIRTY_FLAG_CLIMATE;
		widget_invalidate(w, WIDX_RIGHT_INSET);
	}

	if (RCT2_GLOBAL(RCT2_ADDRESS_BTM_TOOLBAR_DIRTY_FLAGS, uint16) & BTM_TB_DIRTY_FLAG_PARK_RATING){
		RCT2_GLOBAL(RCT2_ADDRESS_BTM_TOOLBAR_DIRTY_FLAGS, uint16) &= ~BTM_TB_DIRTY_FLAG_PARK_RATING;
		widget_invalidate(w, WIDX_LEFT_INSET);
	}
}
