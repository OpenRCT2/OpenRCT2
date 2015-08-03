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
#include "../audio/audio.h"
#include "../management/news_item.h"
#include "../localisation/localisation.h"
#include "../world/sprite.h"
#include "../peep/staff.h"
#include "../sprites.h"
#include "../interface/widget.h"
#include "../interface/window.h"
#include "../interface/themes.h"

enum WINDOW_NEWS_WIDGET_IDX {
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

static void window_news_mouseup(rct_window *w, int widgetIndex);
static void window_news_update(rct_window *w);
static void window_news_scrollgetsize(rct_window *w, int scrollIndex, int *width, int *height);
static void window_news_scrollmousedown(rct_window *w, int scrollIndex, int x, int y);
static void window_news_tooltip(rct_window* w, int widgetIndex, rct_string_id *stringId);
static void window_news_invalidate(rct_window *w);
static void window_news_paint(rct_window *w, rct_drawpixelinfo *dpi);
static void window_news_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, int scrollIndex);

static rct_window_event_list window_news_events = {
	NULL,
	window_news_mouseup,
	NULL,
	NULL,
	NULL,
	NULL,
	window_news_update,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	window_news_scrollgetsize,
	window_news_scrollmousedown,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	window_news_tooltip,
	NULL,
	NULL,
	window_news_invalidate,
	window_news_paint,
	window_news_scrollpaint
};

/**
 * 
 *  rct2: 0x0066E464
 */
void window_news_open()
{
	rct_window* window;

	// Check if window is already open
	window = window_bring_to_front_by_class(WC_RECENT_NEWS);
	if (window == NULL) {
		window = window_create_auto_pos(
			400,
			300,
			&window_news_events,
			WC_RECENT_NEWS,
			0
		);
		window->widgets = window_news_widgets;
		window->enabled_widgets = (1 << WIDX_CLOSE);
		window_init_scroll_widgets(window);
		window->news.var_480 = -1;
	}

// sub_66E4BA:
	rct_widget *widget;

	int width = 0;
	int height = 0;
	window_get_scroll_size(window, 0, &width, &height);
	widget = &window_news_widgets[WIDX_SCROLL];
	window->scrolls[0].v_top = max(0, height - (widget->bottom - widget->top - 1));
	widget_scroll_update_thumbs(window, WIDX_SCROLL);

}

/**
 * 
 *  rct2: 0x0066D4D5
 */
static void window_news_mouseup(rct_window *w, int widgetIndex)
{
	switch (widgetIndex) {
	case WIDX_CLOSE:
		window_close(w);
		break;
	}
}

/**
 * 
 *  rct2: 0x0066EAB8
 */
static void window_news_update(rct_window *w)
{
	int i, j, x, y, z;

	if (w->news.var_480 == -1)
		return;
	if (--w->news.var_484 != 0)
		return;

	window_invalidate(w);
	sound_play_panned(SOUND_CLICK_2, w->x + (w->width / 2), 0, 0, 0);

	j = w->news.var_480;
	w->news.var_480 = -1;
	for (i = 11; i < 61; i++) {
		if (news_item_is_empty(i))
			return;

		if (j == 0) {
			rct_news_item * const newsItem = news_item_get(i);
			if (newsItem->flags & 1)
				return;
			if (w->news.var_482 == 1) {
				news_item_open_subject(newsItem->type, newsItem->assoc);
				return;
			}
			else if (w->news.var_482 > 1) {
				news_item_get_subject_location(newsItem->type, newsItem->assoc, &x, &y, &z);
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
static void window_news_scrollgetsize(rct_window *w, int scrollIndex, int *width, int *height)
{
	int i;

	*height = 0;
	for (i = 11; i < 61; i++) {
		if (news_item_is_empty(i))
			break;

		*height += 42;
	}
}

/**
 * 
 *  rct2: 0x0066EA5C
 */
static void window_news_scrollmousedown(rct_window *w, int scrollIndex, int x, int y)
{
	int i, buttonIndex;

	buttonIndex = 0;
	for (i = 11; i < 61; i++) {
		if (news_item_is_empty(i))
			break;

		if (y < 42) {
			rct_news_item * const newsItem = news_item_get(i);
			if (newsItem->flags & 1) {
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
				if (RCT2_ADDRESS(0x0097BE7C, uint8)[newsItem->type] & 2) {
					buttonIndex = 1;
					break;
				}
			} else if (x < 376) {
				if (RCT2_ADDRESS(0x0097BE7C, uint8)[newsItem->type] & 1) {
					buttonIndex = 2;
					break;
				}
			}
		}
		y -= 42;
	}

	if (buttonIndex != 0) {
		w->news.var_480 = i - 11;
		w->news.var_482 = buttonIndex;
		w->news.var_484 = 4;
		window_invalidate(w);
		sound_play_panned(SOUND_CLICK_1, w->x + (w->width / 2), 0, 0, 0);
	}
}

/**
 * 
 *  rct2: 0x0066EAAE
 */
static void window_news_tooltip(rct_window* w, int widgetIndex, rct_string_id *stringId)
{
	RCT2_GLOBAL(0x013CE952, uint16) = 3159;
}

/**
 * 
 *  rct2: 0x0066E4E8
 */
static void window_news_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
	window_draw_widgets(w, dpi);
}

static void window_news_invalidate(rct_window *w)
{
	colour_scheme_update(w);
}

/**
 * 
 *  rct2: 0x0066E4EE
 */
static void window_news_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, int scrollIndex)
{
	int i, x, y, yy, press;

	y = 0;
	for (i = 11; i < 61; i++) {
		rct_news_item * const newsItem = news_item_get(i);
		if (news_item_is_empty(i))
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
		RCT2_GLOBAL(0x013CE952 + 2, uint16) = STR_MONTH_MARCH + (newsItem->month_year % 8);
		gfx_draw_string_left(dpi, 2235, (void*)0x013CE952, 2, 4, y);

		// Item text
		utf8 buffer[400];
		utf8 *ch = buffer;
		ch = utf8_write_codepoint(ch, FORMAT_SMALLFONT);
		memcpy(ch, newsItem->text, 256);
		ch = buffer;
		gfx_draw_string_left_wrapped(dpi, &ch, 2, y + 10, 325, 1170, 14);

		// Subject button
		if ((RCT2_ADDRESS(0x0097BE7C, uint8)[newsItem->type] & 2) && !(newsItem->flags & 1)) {
			x = 328;
			yy = y + 14;

			press = 0;
			if (w->news.var_480 != -1) {
				const uint8 idx = 11 + w->news.var_480;
				news_item_is_valid_idx(idx);
				if (i == idx && w->news.var_482 == 1)
					press = 0x20;
			}
			gfx_fill_rect_inset(dpi, x, yy, x + 23, yy + 23, w->colours[2], press);

			switch (newsItem->type) {
			case NEWS_ITEM_RIDE:
				gfx_draw_sprite(dpi, SPR_RIDE, x, yy, 0);
				break;
			case NEWS_ITEM_PEEP:
			case NEWS_ITEM_PEEP_ON_RIDE:
			{
				rct_drawpixelinfo* cliped_dpi = clip_drawpixelinfo(dpi, x + 1, 22, yy + 1, 22);
				if (!cliped_dpi) break;

				rct_peep* peep = GET_PEEP(newsItem->assoc);
				int clip_x = 10, clip_y = 19;

				// If normal peep set sprite to normal (no food)
				// If staff set sprite to staff sprite
				int sprite_type = 0;
				if (peep->type == PEEP_TYPE_STAFF){
					sprite_type = peep->sprite_type;
					if (peep->staff_type == STAFF_TYPE_ENTERTAINER){
						clip_y += 3;
					}
				}

				uint32 image_id = *RCT2_ADDRESS(0x00982708, uint32*)[sprite_type * 2];
				image_id += 0xA0000001;
				image_id |= (peep->tshirt_colour << 19) | (peep->trousers_colour << 24);

				gfx_draw_sprite(cliped_dpi, image_id, clip_x, clip_y, 0);

				rct2_free(cliped_dpi);
				break; 
			}
			case NEWS_ITEM_MONEY:
				gfx_draw_sprite(dpi, SPR_FINANCE, x, yy, 0);
				break;
			case NEWS_ITEM_RESEARCH:
				gfx_draw_sprite(dpi, newsItem->assoc < 0x10000 ? SPR_NEW_RIDE : SPR_SCENERY, x, yy, 0);
				break;
			case NEWS_ITEM_PEEPS:
				gfx_draw_sprite(dpi, SPR_GUESTS, x, yy, 0);
				break;
			case NEWS_ITEM_AWARD:
				gfx_draw_sprite(dpi, SPR_AWARD, x, yy, 0);
				break;
			case NEWS_ITEM_GRAPH:
				gfx_draw_sprite(dpi, SPR_GRAPH, x, yy, 0);
				break;
			}
		}

		// Location button
		if ((RCT2_ADDRESS(0x0097BE7C, uint8)[newsItem->type] & 1) && !(newsItem->flags & 1)) {
			x = 352;
			yy = y + 14;

			press = 0;
			if (w->news.var_480 != -1) {
				const uint8 idx = 11 + w->news.var_480;
				news_item_is_valid_idx(idx);
				if (i == idx && w->news.var_482 == 2)
					press = 0x20;
			}
			gfx_fill_rect_inset(dpi, x, yy, x + 23, yy + 23, w->colours[2], press);
			gfx_draw_sprite(dpi, SPR_LOCATE, x, yy, 0);
		}

		y += 42;
	}
}
