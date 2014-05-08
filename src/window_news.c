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

#include <string.h>
#include "addresses.h"
#include "audio.h"
#include "news_item.h"
#include "strings.h"
#include "sprite.h"
#include "sprites.h"
#include "widget.h"
#include "window.h"

static enum WINDOW_NEWS_WIDGET_IDX {
	WIDX_BACKGROUND,
	WIDX_TITLE,
	WIDX_CLOSE,
	WIDX_SCROLL
};

static rct_widget window_news_widgets[] = {
	{ WWT_FRAME,			0,	0,			399,	0,		299,	0x0FFFFFFFF,			STR_NONE },				// panel / background
	{ WWT_CAPTION,			0,	1,			398,	1,		14,		STR_RECENT_MESSAGES,	STR_WINDOW_TITLE_TIP },	// title bar
	{ WWT_CLOSEBOX,			0,	387,		397,	2,		13,		STR_CLOSE_X,			STR_CLOSE_WINDOW_TIP },	// close x button
	{ WWT_SCROLL,			0,	4,			395,	18,		295,	2,						STR_NONE },				// scroll
	{ WIDGETS_END },
};

static void window_news_emptysub() { }
static void window_news_mouseup();
static void window_news_update();
static void window_news_scrollgetsize();
static void window_news_scrollmousedown();
static void window_news_tooltip();
static void window_news_paint();
static void window_news_scrollpaint();

static uint32 window_news_events[] = {
	window_news_emptysub,
	window_news_mouseup,
	window_news_emptysub,
	window_news_emptysub,
	window_news_emptysub,
	window_news_emptysub,
	window_news_update,
	window_news_emptysub,
	window_news_emptysub,
	window_news_emptysub,
	window_news_emptysub,
	window_news_emptysub,
	window_news_emptysub,
	window_news_emptysub,
	window_news_emptysub,
	window_news_scrollgetsize,
	window_news_scrollmousedown,
	window_news_emptysub,
	window_news_emptysub,
	window_news_emptysub,
	window_news_emptysub,
	window_news_emptysub,
	window_news_tooltip,
	window_news_emptysub,
	window_news_emptysub,
	window_news_emptysub,
	window_news_paint,
	window_news_scrollpaint
};

/**
 * 
 *  rct2: 0x0066E464
 */
void window_news_open()
{
	int x, y;
	rct_window* window;

	// Check if window is already open
	window = window_bring_to_front_by_id(WC_RECENT_NEWS, 0);
	if (window == NULL) {
		window = window_create_auto_pos(
			400,
			300,
			window_news_events,
			WC_RECENT_NEWS,
			0
		);
		window->widgets = window_news_widgets;
		window->enabled_widgets = (1 << WIDX_CLOSE);
		window_init_scroll_widgets(window);
		window->colours[0] = 1;
		window->colours[1] = 1;
		window->colours[2] = 0;
		window->var_480 = -1;
	}

// sub_66E4BA:
	int width, height;
	rct_widget *widget;

	window_get_scroll_size(window, 0, &width, &height);
	widget = &window_news_widgets[WIDX_SCROLL];
	window->scrolls[0].v_top = max(0, height - (widget->bottom - widget->top - 1));
	widget_scroll_update_thumbs(window, WIDX_SCROLL);
}

/**
 * 
 *  rct2: 0x0066D4D5
 */
static void window_news_mouseup()
{
	int i;
	short widgetIndex;
	rct_window *w;

	__asm mov widgetIndex, dx
	__asm mov w, esi

	if (widgetIndex == WIDX_CLOSE)
		window_close(w);
}

/**
 * 
 *  rct2: 0x0066EAB8
 */
static void window_news_update()
{
	int i, j, x, y, z;
	rct_window *w;
	rct_news_item *newsItems;

	__asm mov w, esi

	if (w->var_480 == -1)
		return;
	if (--w->var_484 != 0)
		return;

	window_invalidate(w);
	sound_play_panned(5, w->x + (w->width / 2));

	newsItems = RCT2_ADDRESS(RCT2_ADDRESS_NEWS_ITEM_LIST, rct_news_item);
	j = w->var_480;
	w->var_480 = -1;
	for (i = 11; i < 61; i++) {
		if (newsItems[i].type == NEWS_ITEM_NULL)
			return;

		if (j == 0) {
			if (newsItems[i].flags & 1)
				return;
			if (w->var_482 == 1) {
				news_item_open_subject(newsItems[i].type, newsItems[i].assoc);
				return;
			} else if (w->var_482 > 1) {
				news_item_get_subject_location(newsItems[i].type, newsItems[i].assoc, &x, &y, &z);
				if (x != SPRITE_LOCATION_NULL)
					if ((w = window_get_main()) != NULL)
						window_scroll_to_location(w, x, y, z);
				return;
			}
		}
		j--;
	}
}

/**
 * 
 *  rct2: 0x0066EA3C
 */
static void window_news_scrollgetsize()
{
	int i, height;
	rct_news_item *newsItems = RCT2_ADDRESS(RCT2_ADDRESS_NEWS_ITEM_LIST, rct_news_item);

	height = 0;
	for (i = 11; i < 61; i++) {
		if (newsItems[i].type == NEWS_ITEM_NULL)
			break;

		height += 42;
	}

	__asm mov edx, height
}

/**
 * 
 *  rct2: 0x0066EA5C
 */
static void window_news_scrollmousedown()
{
	int i, buttonIndex;
	short x, y;
	rct_window *w;
	rct_news_item *newsItems;

	__asm mov x, cx
	__asm mov y, dx
	__asm mov w, esi

	buttonIndex = 0;
	newsItems = RCT2_ADDRESS(RCT2_ADDRESS_NEWS_ITEM_LIST, rct_news_item);
	for (i = 11; i < 61; i++) {
		if (newsItems[i].type == NEWS_ITEM_NULL)
			break;

		if (y < 42) {
			if (newsItems[i].flags & 1) {
				buttonIndex = 0;
				break;
			} else if (y < 14) {
				buttonIndex = 0;
				break;
			} else if (y >= 38) {
				buttonIndex = 0;
				break;
			} else if (x < 328) {
				buttonIndex = 0;
				break;
			} else if (x < 351) {
				if (RCT2_ADDRESS(0x0097BE7C, uint8)[newsItems[i].type] & 2) {
					buttonIndex = 1;
					break;
				}
			} else if (x < 376) {
				if (RCT2_ADDRESS(0x0097BE7C, uint8)[newsItems[i].type] & 1) {
					buttonIndex = 2;
					break;
				}
			}
		}
		y -= 42;
	}

	if (buttonIndex != 0) {
		w->var_480 = i - 11;
		w->var_482 = buttonIndex;
		w->var_484 = 4;
		window_invalidate(w);
		sound_play_panned(4, w->x + (w->width / 2));
	}
}

/**
 * 
 *  rct2: 0x0066EAAE
 */
static void window_news_tooltip()
{
	RCT2_GLOBAL(0x013CE952, uint16) = 3159;
}

/**
 * 
 *  rct2: 0x0066E4E8
 */
static void window_news_paint()
{
	int x, y;
	rct_window *w;
	rct_drawpixelinfo *dpi;

	__asm mov w, esi
	__asm mov dpi, edi

	window_draw_widgets(w, dpi);
}

/**
 * 
 *  rct2: 0x0066E4EE
 */
static void window_news_scrollpaint()
{
	int i, x, y, yy, press;
	rct_window *w;
	rct_drawpixelinfo *dpi;
	rct_news_item *newsItems, *newsItem, *newsItem2;

	__asm mov w, esi
	__asm mov dpi, edi

	y = 0;
	newsItems = RCT2_ADDRESS(RCT2_ADDRESS_NEWS_ITEM_LIST, rct_news_item);
	for (i = 11; i < 61; i++) {
		newsItem = &newsItems[i];
		if (newsItem->type == NEWS_ITEM_NULL)
			break;
		if (y >= dpi->y + dpi->height)
			break;
		if (y + 42 < dpi->y) {
			y += 42;
			continue;
		}

		// Background
		gfx_fill_rect_inset(dpi, -1, y, 383, y + 41, w->colours[1], 0x24);

		// Date text
		RCT2_GLOBAL(0x013CE952, uint16) = STR_DATE_DAY_1 + newsItem->day - 1;
		RCT2_GLOBAL(0x013CE952 + 2, uint16) = STR_MONTH_MARCH + (newsItem->month % 8);
		gfx_draw_string_left(dpi, 2235, 0x013CE952, 2, 4, y);

		// Item text
		RCT2_GLOBAL(0x009B5F2C, uint8) = newsItem->colour;
		strcpy(0x009B5F2D, newsItem->text);
		gfx_draw_string_left_wrapped(dpi, 0, 2, y + 10, 325, 1926, 14);

		// Subject button
		if ((RCT2_ADDRESS(0x0097BE7C, uint8)[newsItem->type] & 2) && !(newsItem->flags & 1)) {
			x = 328;
			yy = y + 14;

			press = 0;
			if (w->var_480 != -1) {
				newsItem2 = &newsItems[11 + w->var_480];
				if (newsItem == newsItem2 && w->var_482 == 1)
					press = 0x20;
			}
			gfx_fill_rect_inset(dpi, x, yy, x + 23, yy + 23, w->colours[2], press);

			switch (newsItem->type) {
			case NEWS_ITEM_RIDE:
				gfx_draw_sprite(dpi, SPR_RIDE, x, yy);
				break;
			case NEWS_ITEM_PEEP_ON_RIDE:
				// TODO
				break;
			case NEWS_ITEM_PEEP:
				// TODO
				break;
			case NEWS_ITEM_MONEY:
				gfx_draw_sprite(dpi, SPR_FINANCE, x, yy);
				break;
			case NEWS_ITEM_RESEARCH:
				gfx_draw_sprite(dpi, newsItem->assoc < 0x10000 ? SPR_NEW_RIDE : SPR_SCENERY, x, yy);
				break;
			case NEWS_ITEM_PEEPS:
				gfx_draw_sprite(dpi, SPR_GUESTS, x, yy);
				break;
			case NEWS_ITEM_AWARD:
				gfx_draw_sprite(dpi, SPR_AWARD, x, yy);
				break;
			case NEWS_ITEM_GRAPH:
				gfx_draw_sprite(dpi, SPR_GRAPH, x, yy);
				break;
			}
		}

		// Location button
		if ((RCT2_ADDRESS(0x0097BE7C, uint8)[newsItem->type] & 1) && !(newsItem->flags & 1)) {
			x = 352;
			yy = y + 14;

			press = 0;
			if (w->var_480 != -1) {
				newsItem2 = &newsItems[11 + w->var_480];
				if (newsItem == newsItem2 && w->var_482 == 2)
					press = 0x20;
			}
			gfx_fill_rect_inset(dpi, x, yy, x + 23, yy + 23, w->colours[2], press);
			gfx_draw_sprite(dpi, SPR_LOCATE, x, yy);
		}

		y += 42;
	}
}