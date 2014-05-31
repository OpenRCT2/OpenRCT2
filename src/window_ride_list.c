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
#include "game.h"
#include "ride.h"
#include "string_ids.h"
#include "sprites.h"
#include "widget.h"
#include "window.h"
#include "window_dropdown.h"

enum {
	PAGE_RIDES,
	PAGE_SHOPS_AND_STALLS,
	PAGE_KIOSKS_AND_FACILITIES
};

enum WINDOW_RIDE_LIST_WIDGET_IDX {
	WIDX_BACKGROUND,
	WIDX_TITLE,
	WIDX_CLOSE,
	WIDX_PAGE_BACKGROUND,
	WIDX_OPEN_CLOSE_ALL,
	WIDX_CURRENT_INFORMATION_TYPE,
	WIDX_INFORMATION_TYPE_DROPDOWN,
	WIDX_SORT,
	WIDX_TAB_1,
	WIDX_TAB_2,
	WIDX_TAB_3,
	WIDX_LIST
};

static rct_widget window_ride_list_widgets[] = {
	{ WWT_FRAME,			0,	0,		339,	0,		239,	0x0FFFFFFFF,				65535 },									// panel / background
	{ WWT_CAPTION,			0,	1,		338,	1,		14,		0x0FFFFFFFF,				STR_WINDOW_TITLE_TIP },						// title bar
	{ WWT_CLOSEBOX,			0,	327,	337,	2,		13,		STR_CLOSE_X,				STR_CLOSE_WINDOW_TIP },						// close x button
	{ WWT_RESIZE,			1,	0,		339,	43,		239,	0x0FFFFFFFF,				65535 },									// tab page background
	{ WWT_FLATBTN,			1,	315,	338,	60,		83,		SPR_TOGGLE_OPEN_CLOSE,		STR_OPEN_OR_CLOSE_ALL_RIDES },				// open / close all toggle
	{ WWT_DROPDOWN,			1,	164,	273,	46,		57,		0x0FFFFFFFF,				65535 },									// current information type
	{ WWT_DROPDOWN_BUTTON,	1,	262,	272,	47,		56,		876,						STR_RIDE_LIST_INFORMATION_TYPE_TIP },		// information type dropdown button
	{ WWT_DROPDOWN_BUTTON,	1,	280,	333,	46,		57,		STR_SORT,					STR_RIDE_LIST_SORT_TIP },					// sort button
	{ WWT_TAB,				1,	3,		33,		17,		43,		0x2000144E,					STR_LIST_RIDES_TIP },						// tab 1
	{ WWT_TAB,				1,	34,		64,		17,		43,		0x2000144E,					STR_LIST_SHOPS_AND_STALLS_TIP },			// tab 2
	{ WWT_TAB,				1,	65,		95,		17,		43,		0x2000144E,					STR_LIST_KIOSKS_AND_FACILITIES_TIP },		// tab 3
	{ WWT_SCROLL,			1,	3,		336,	60,		236,	2,							65535 },									// list
	{ WIDGETS_END },
};

static void window_ride_list_emptysub() { }
static void window_ride_list_mouseup();
static void window_ride_list_resize();
static void window_ride_list_mousedown();
static void window_ride_list_dropdown();
static void window_ride_list_update(rct_window *w);
static void window_ride_list_scrollgetsize();
static void window_ride_list_scrollmousedown();
static void window_ride_list_scrollmouseover();
static void window_ride_list_tooltip();
static void window_ride_list_invalidate();
static void window_ride_list_paint();
static void window_ride_list_scrollpaint();

static void* window_ride_list_events[] = {
	window_ride_list_emptysub,
	window_ride_list_mouseup,
	window_ride_list_resize,
	window_ride_list_mousedown,
	window_ride_list_dropdown,
	window_ride_list_emptysub,
	window_ride_list_update,
	window_ride_list_emptysub,
	window_ride_list_emptysub,
	window_ride_list_emptysub,
	window_ride_list_emptysub,
	window_ride_list_emptysub,
	window_ride_list_emptysub,
	window_ride_list_emptysub,
	window_ride_list_emptysub,
	window_ride_list_scrollgetsize,
	window_ride_list_scrollmousedown,
	window_ride_list_emptysub,
	window_ride_list_scrollmouseover,
	window_ride_list_emptysub,
	window_ride_list_emptysub,
	window_ride_list_emptysub,
	window_ride_list_tooltip,
	window_ride_list_emptysub,
	window_ride_list_emptysub,
	window_ride_list_invalidate,
	window_ride_list_paint,
	window_ride_list_scrollpaint
};

enum {
	INFORMATION_TYPE_STATUS,
	INFORMATION_TYPE_POPULARITY,
	INFORMATION_TYPE_SATISFACTION,
	INFORMATION_TYPE_PROFIT,
	INFORMATION_TYPE_QUEUE_LENGTH,
	INFORMATION_TYPE_QUEUE_TIME,
	INFORMATION_TYPE_RELIABILITY,
	INFORMATION_TYPE_DOWN_TIME,
	INFORMATION_TYPE_GUESTS_FAVOURITE
};

static int _window_ride_list_information_type;

static void window_ride_list_draw_tab_images(rct_drawpixelinfo *dpi, rct_window *w);
static void window_ride_list_refresh_list(rct_window *w);
static void window_ride_list_close_all(rct_window *w);
static void window_ride_list_open_all(rct_window *w);

/**
 * 
 *  rct2: 0x006B30BC
 */
void window_ride_list_open()
{
	rct_window* window;

	// Check if window is already open
	window = window_bring_to_front_by_id(WC_RIDE_LIST, 0);
	if (window == NULL) {
		window = window_create_auto_pos(340, 240, (uint32*)window_ride_list_events, WC_RIDE_LIST, 0x0400);
		window->widgets = window_ride_list_widgets;
		window->enabled_widgets =
			(1 << WIDX_CLOSE) |
			(1 << WIDX_OPEN_CLOSE_ALL) |
			(1 << WIDX_CURRENT_INFORMATION_TYPE) |
			(1 << WIDX_INFORMATION_TYPE_DROPDOWN) |
			(1 << WIDX_SORT) |
			(1 << WIDX_TAB_1) |
			(1 << WIDX_TAB_2) |
			(1 << WIDX_TAB_3);
		window_init_scroll_widgets(window);
		window->page = PAGE_RIDES;
		window->var_476 = 0;
		window->var_47A = -1;
		window->var_48E = 0;
		window->min_width = 340;
		window->min_height = 240;
		window->max_width = 400;
		window->max_height = 450;
		window->flags = WF_RESIZABLE;
		window->colours[0] = 1;
		window->colours[1] = 26;
		window->colours[2] = 26;
	}
	_window_ride_list_information_type = INFORMATION_TYPE_STATUS;
	window->var_490 = 0;
}

/**
 * 
 *  rct2: 0x006B3511
 */
static void window_ride_list_mouseup()
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


	switch (widgetIndex) {
	case WIDX_CLOSE:
		window_close(w);
		break;
	case WIDX_SORT:
		w->var_490 = _window_ride_list_information_type;
		w->var_476 = 0;
		w->var_47A = -1;
		break;
	case WIDX_TAB_1:
	case WIDX_TAB_2:
	case WIDX_TAB_3:
		if (w->page != widgetIndex - WIDX_TAB_1) {
			w->page = widgetIndex - WIDX_TAB_1;
			w->var_476 = 0;
			w->var_47A = -1;
			if (w->page != PAGE_RIDES && _window_ride_list_information_type > INFORMATION_TYPE_PROFIT)
				_window_ride_list_information_type = INFORMATION_TYPE_PROFIT;
			window_invalidate(w);
		}
		break;
	}
}

/**
 * 
 *  rct2: 0x006B38A7
 */
static void window_ride_list_resize()
{
	rct_window *w;

	#ifdef _MSC_VER
	__asm mov w, esi
	#else
	__asm__ ( "mov %[w], esi " : [w] "+m" (w) );
	#endif


	w->min_width = 340;
	w->min_height = 124;
	if (w->width < w->min_width) {
		window_invalidate(w);
		w->width = w->min_width;
	}
	if (w->height < w->min_height) {
		window_invalidate(w);
		w->height = w->min_height;
	}

	window_ride_list_refresh_list(w);
}

/**
 * 
 *  rct2: 0x006B3532
 */
static void window_ride_list_mousedown()
{
	int numItems, i;
	short widgetIndex;
	rct_window *w;
	rct_widget *widget;

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

	#ifdef _MSC_VER
	__asm mov widget, edi
	#else
	__asm__ ( "mov %[widget], edi " : [widget] "+m" (widget) );
	#endif


	if (widgetIndex == WIDX_OPEN_CLOSE_ALL) {
		gDropdownItemsFormat[0] = STR_CLOSE_ALL;
		gDropdownItemsFormat[1] = STR_OPEN_ALL;
		window_dropdown_show_text(w->x + widget->left, w->y + widget->top, widget->bottom - widget->top, w->colours[1], 0, 2);
	} else if (widgetIndex == WIDX_INFORMATION_TYPE_DROPDOWN) {
		widget--;

		numItems = 9;
		if (w->page != PAGE_RIDES)
			numItems -= 5;
		if (RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & PARK_FLAGS_NO_MONEY)
			numItems--;

		for (i = 0; i < numItems; i++) {
			gDropdownItemsFormat[i] = 1142;
			gDropdownItemsArgs[i] = STR_STATUS + i;
		}		
		window_dropdown_show_text_custom_width(w->x + widget->left, w->y + widget->top, widget->bottom - widget->top, w->colours[1], 0x80, numItems, widget->right - widget->left - 3);
		gDropdownItemsChecked |= (1 << _window_ride_list_information_type);
	}
}

/**
 * 
 *  rct2: 0x006B3547
 */
static void window_ride_list_dropdown()
{
	short dropdownIndex, widgetIndex;
	rct_window *w;

	#ifdef _MSC_VER
	__asm mov dropdownIndex, ax
	#else
	__asm__ ( "mov %[dropdownIndex], ax " : [dropdownIndex] "+m" (dropdownIndex) );
	#endif

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


	if (widgetIndex == WIDX_OPEN_CLOSE_ALL) {
		if (dropdownIndex == 0)
			window_ride_list_close_all(w);
		else if (dropdownIndex == 1)
			window_ride_list_open_all(w);
	} else if (widgetIndex == WIDX_INFORMATION_TYPE_DROPDOWN) {
		if (dropdownIndex == -1)
			return;

		_window_ride_list_information_type = *((uint32*)&gDropdownItemsArgs[dropdownIndex]) - STR_STATUS;
		window_invalidate(w);
	}
}

/**
 * 
 *  rct2: 0x006B386B
 */
static void window_ride_list_update(rct_window *w)
{
	w->var_48E = (w->var_48E + 1) % 64;
	widget_invalidate(w->classification, w->number, WIDX_TAB_1 + w->page);
	if (_window_ride_list_information_type != INFORMATION_TYPE_STATUS)
		window_invalidate(w);
}

/**
 * 
 *  rct2: 0x006B35A1
 */
static void window_ride_list_scrollgetsize()
{
	int top, height;
	rct_window *w;

	#ifdef _MSC_VER
	__asm mov w, esi
	#else
	__asm__ ( "mov %[w], esi " : [w] "+m" (w) );
	#endif


	height = w->var_476 * 10;
	if (w->var_47A != -1) {
		w->var_47A = -1;
		window_invalidate(w);
	}

	top = height - window_ride_list_widgets[WIDX_LIST].bottom + window_ride_list_widgets[WIDX_LIST].top + 21;
	if (top < 0)
		top = 0;
	if (top < w->scrolls[0].v_top) {
		w->scrolls[0].v_top = top;
		window_invalidate(w);
	}

	#ifdef _MSC_VER
	__asm mov ecx, 0
	#else
	__asm__ ( "mov ecx, 0 "  );
	#endif

	#ifdef _MSC_VER
	__asm mov edx, height
	#else
	__asm__ ( "mov edx, %[height] " : [height] "+m" (height) );
	#endif

}

/**
 * 
 *  rct2: 0x006B361F
 */
static void window_ride_list_scrollmousedown()
{
	int index;
	short y;
	rct_window *w;

	#ifdef _MSC_VER
	__asm mov y, dx
	#else
	__asm__ ( "mov %[y], dx " : [y] "+m" (y) );
	#endif

	#ifdef _MSC_VER
	__asm mov w, esi
	#else
	__asm__ ( "mov %[w], esi " : [w] "+m" (w) );
	#endif


	index = y / 10;
	if (index >= w->var_476)
		return;

	// Open ride window
	RCT2_CALLPROC_X(0x006ACC28, w->var_076[index], 0, 0, 0, 0, 0, 0);
}

/**
 * 
 *  rct2: 0x006B35EF
 */
static void window_ride_list_scrollmouseover()
{
	int index;
	short y;
	rct_window *w;

	#ifdef _MSC_VER
	__asm mov y, dx
	#else
	__asm__ ( "mov %[y], dx " : [y] "+m" (y) );
	#endif

	#ifdef _MSC_VER
	__asm mov w, esi
	#else
	__asm__ ( "mov %[w], esi " : [w] "+m" (w) );
	#endif


	index = y / 10;
	if (index >= w->var_476)
		return;

	w->var_47A = index;
	window_invalidate(w);
}

/**
 * 
 *  rct2: 0x006B3861
 */
static void window_ride_list_tooltip()
{
	RCT2_GLOBAL(0x013CE952, uint16) = STR_LIST;
}

/**
 * 
 *  rct2: 0x006B3182
 */
static void window_ride_list_invalidate()
{
	int i;
	rct_window *w;

	#ifdef _MSC_VER
	__asm mov w, esi
	#else
	__asm__ ( "mov %[w], esi " : [w] "+m" (w) );
	#endif


	window_ride_list_widgets[WIDX_CURRENT_INFORMATION_TYPE].image = STR_STATUS + _window_ride_list_information_type;

	// Set correct active tab
	for (i = 0; i < 3; i++)
		w->pressed_widgets &= ~(1 << (WIDX_TAB_1 + i));
	w->pressed_widgets |= 1LL << (WIDX_TAB_1 + w->page);

	window_ride_list_widgets[WIDX_TITLE].image = STR_RIDES + w->page;

	w->widgets[WIDX_BACKGROUND].right = w->width - 1;
	w->widgets[WIDX_BACKGROUND].bottom = w->height - 1;
	w->widgets[WIDX_PAGE_BACKGROUND].right = w->width - 1;
	w->widgets[WIDX_PAGE_BACKGROUND].bottom = w->height - 1;
	w->widgets[WIDX_TITLE].right = w->width - 2;
	w->widgets[WIDX_CLOSE].left = w->width - 13;
	w->widgets[WIDX_CLOSE].right = w->width - 3;
	w->widgets[WIDX_LIST].right = w->width - 26;
	w->widgets[WIDX_LIST].bottom = w->height - 4;
	w->widgets[WIDX_OPEN_CLOSE_ALL].right = w->width - 2;
	w->widgets[WIDX_OPEN_CLOSE_ALL].left = w->width - 25;
}

/**
 * 
 *  rct2: 0x006B3235
 */
static void window_ride_list_paint()
{
	rct_window *w;
	rct_drawpixelinfo *dpi;

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
	window_ride_list_draw_tab_images(dpi, w);
}

/**
 * 
 *  rct2: 0x006AF561
 */
static void ride_get_status(int rideIndex, int *formatSecondary, int *argument)
{
	int eax, ebx, ecx, edx, esi, edi, ebp;
	edx = rideIndex;
	RCT2_CALLFUNC_X(0x006AF561, &eax, &ebx, &ecx, &edx, &esi, &edi, &ebp);
	*formatSecondary = ebx & 0xFFFF;
	*argument = eax;
}

/**
 * 
 *  rct2: 0x006B3240
 */
static void window_ride_list_scrollpaint()
{
	int i, y, format, formatSecondary, argument;
	rct_window *w;
	rct_drawpixelinfo *dpi;
	rct_ride *ride;

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


	gfx_fill_rect(dpi, dpi->x, dpi->y, dpi->x + dpi->width, dpi->y + dpi->height, RCT2_GLOBAL(0x0141FC48 + (w->colours[1] * 8), uint8));

	y = 0;
	for (i = 0; i < w->var_476; i++) {
		format = 1191;

		// Background highlight
		if (i == w->var_47A) {
			gfx_fill_rect(dpi, 0, y, 800, y + 9, 0x02000031);
			format = 1193;
		}
		
		// Get ride
		ride = &RCT2_ADDRESS(RCT2_ADDRESS_RIDE_LIST, rct_ride)[w->var_076[i]];

		// Ride name
		gfx_draw_string_left_clipped(dpi, format, &ride->var_04A, 0, 0, y - 1, 159);

		// Ride information
		formatSecondary = 0;
		switch (_window_ride_list_information_type) {
		case INFORMATION_TYPE_STATUS:
			ride_get_status(i, &formatSecondary, &argument);
			RCT2_GLOBAL(0x013CE952 + 2, sint32) = argument;
			break;
		case INFORMATION_TYPE_POPULARITY:
			formatSecondary = STR_POPULARITY_UNKNOWN_LABEL;
			if ((ride->popularity & 0xFF) != 255) {
				formatSecondary = STR_POPULARITY_LABEL;
				RCT2_GLOBAL(0x013CE952 + 2, uint16) = (ride->popularity & 0xFF) * 4;
			}
			break;
		case INFORMATION_TYPE_SATISFACTION:
			formatSecondary = STR_SATISFACTION_UNKNOWN_LABEL;
			if ((ride->satisfaction & 0xFF) != 255) {
				formatSecondary = STR_SATISFACTION_LABEL;
				RCT2_GLOBAL(0x013CE952 + 2, uint16) = (ride->satisfaction & 0xFF) * 5;
			}
			break;
		case INFORMATION_TYPE_PROFIT:
			formatSecondary = 0;
			if (ride->profit != MONEY32_UNDEFINED) {
				formatSecondary = STR_PROFIT_LABEL;
				RCT2_GLOBAL(0x013CE952 + 2, sint32) = ride->profit;
			}
			break;
		case INFORMATION_TYPE_QUEUE_LENGTH:
			RCT2_GLOBAL(0x013CE952 + 2, uint16) = ride_get_total_queue_length(ride);
			formatSecondary = STR_QUEUE_EMPTY;
			if (RCT2_GLOBAL(0x013CE952 + 2, uint16) == 1)
				formatSecondary = STR_QUEUE_ONE_PERSON;
			else if (RCT2_GLOBAL(0x013CE952 + 2, uint16) > 1)
				formatSecondary = STR_QUEUE_PEOPLE;
			break;
		case INFORMATION_TYPE_QUEUE_TIME:
			RCT2_GLOBAL(0x013CE952 + 2, uint16) = ride_get_max_queue_time(ride);
			formatSecondary = STR_QUEUE_TIME_LABEL;
			if (RCT2_GLOBAL(0x013CE952 + 2, uint16) > 1)
				formatSecondary = STR_QUEUE_TIME_PLURAL_LABEL;
			break;
		case INFORMATION_TYPE_RELIABILITY:
			// edx = RCT2_GLOBAL(0x009ACFA4 + (ride->var_001 * 4), uint32);

			RCT2_GLOBAL(0x013CE952 + 2, uint16) = ride->reliability;
			formatSecondary = STR_RELIABILITY_LABEL;
			break;
		case INFORMATION_TYPE_DOWN_TIME:
			// edx = RCT2_GLOBAL(0x009ACFA4 + (ride->var_001 * 4), uint32);

			RCT2_GLOBAL(0x013CE952 + 2, uint16) = ride->downtime;
			formatSecondary = STR_DOWN_TIME_LABEL;
			break;
		case INFORMATION_TYPE_GUESTS_FAVOURITE:
			formatSecondary = 0;
			if (RCT2_ADDRESS(0x0097C3AF, uint8)[ride->type] == PAGE_RIDES) {
				RCT2_GLOBAL(0x013CE952 + 2, uint16) = ride->guests_favourite;
				formatSecondary = ride->guests_favourite == 1 ? STR_GUESTS_FAVOURITE_LABEL : STR_GUESTS_FAVOURITE_PLURAL_LABEL;
			}
			break;
		}

		// Make test red and bold if broken down or crashed
		if (formatSecondary == STR_BROKEN_DOWN || formatSecondary == STR_CRASHED)
			format = 1192;

		RCT2_GLOBAL(0x013CE952, uint16) = formatSecondary;
		gfx_draw_string_left_clipped(dpi, format, (void*)0x013CE952, 0, 160, y - 1, 157);
		y += 10;
	}
}

/**
 * 
 *  rct2: 0x006B38EA
 */
static void window_ride_list_draw_tab_images(rct_drawpixelinfo *dpi, rct_window *w)
{
	int sprite_idx;

	// Rides tab
	sprite_idx = SPR_TAB_RIDE_0;
	if (w->page == PAGE_RIDES)
		sprite_idx += w->var_48E / 4;
	gfx_draw_sprite(dpi, sprite_idx, w->x + w->widgets[WIDX_TAB_1].left, w->y + w->widgets[WIDX_TAB_1].top);

	// Shops and stalls tab
	sprite_idx = SPR_TAB_SHOPS_AND_STALLS_0;
	if (w->page == PAGE_SHOPS_AND_STALLS)
		sprite_idx += w->var_48E / 4;
	gfx_draw_sprite(dpi, sprite_idx, w->x + w->widgets[WIDX_TAB_2].left, w->y + w->widgets[WIDX_TAB_2].top);

	// Information kiosks and facilities tab
	sprite_idx = SPR_TAB_KIOSKS_AND_FACILITIES_0;
	if (w->page == PAGE_KIOSKS_AND_FACILITIES)
		sprite_idx += (w->var_48E / 4) % 8;
	gfx_draw_sprite(dpi, sprite_idx, w->x + w->widgets[WIDX_TAB_3].left, w->y + w->widgets[WIDX_TAB_3].top);
}

/**
 * 
 *  rct2: 0x006B39A8
 */
static void window_ride_list_refresh_list(rct_window *w)
{
	int i, j, k, countA, countB;
	uint8 swapper;
	rct_ride *ride, *otherRide;

	countA = countB = 0;
	for (i = 0; i < MAX_RIDES; i++) {
		ride = &(RCT2_ADDRESS(RCT2_ADDRESS_RIDE_LIST, rct_ride)[i]);
		if (ride->type == RIDE_TYPE_NULL)
			continue;
		if (w->page != gRideClassifications[ride->type])
			continue;

		countA++;
		if (ride->var_14D & 8) {
			ride->var_14D &= ~8;
			countB++;
		}
	}
	
	if (countB != 0)
		window_invalidate(w);

	if (countA == w->var_476)
		return;

	w->var_476 = countA;
	j = 0;
	for (i = 0; i < MAX_RIDES; i++) {
		ride = &(RCT2_ADDRESS(RCT2_ADDRESS_RIDE_LIST, rct_ride)[i]);
		if (ride->type == RIDE_TYPE_NULL)
			continue;
		if (w->page != gRideClassifications[ride->type])
			continue;

		w->var_076[j] = i;
		k = j;
		switch (w->var_490) {
		case INFORMATION_TYPE_STATUS:
			RCT2_GLOBAL(0x013CE952, uint32) = ride->var_04C;
			RCT2_CALLPROC_X(0x006C2538, ride->var_04A, 0, 0x013CE952, 0, 0, 0x0141ED68, 0);
			while (--k >= 0) {
				otherRide = &(RCT2_ADDRESS(RCT2_ADDRESS_RIDE_LIST, rct_ride)[w->var_076[k]]);
				RCT2_GLOBAL(0x013CE952, uint32) = otherRide->var_04C;
				RCT2_CALLPROC_X(0x006C2538, otherRide->var_04A, 0, 0x013CE952, 0, 0, 0x0141EF68, 0);
				if (strcmp((char*)0x0141ED68, (char*)0x0141EF68) >= 0)
					break;

				swapper = w->var_076[k];
				w->var_076[k] = w->var_076[k + 1];
				w->var_076[k + 1] = swapper;
			}
			break;
		case INFORMATION_TYPE_POPULARITY:
			while (--k >= 0) {
				otherRide = &(RCT2_ADDRESS(RCT2_ADDRESS_RIDE_LIST, rct_ride)[w->var_076[k]]);
				if ((ride->popularity & 0xFF) * 4 <= (otherRide->popularity & 0xFF) * 4)
					break;

				swapper = w->var_076[k];
				w->var_076[k] = w->var_076[k + 1];
				w->var_076[k + 1] = swapper;
			}
			break;
		case INFORMATION_TYPE_SATISFACTION:
			while (--k >= 0) {
				otherRide = &(RCT2_ADDRESS(RCT2_ADDRESS_RIDE_LIST, rct_ride)[w->var_076[k]]);
				if ((ride->satisfaction & 0xFF) * 5 <= (otherRide->satisfaction & 0xFF) * 5)
					break;

				swapper = w->var_076[k];
				w->var_076[k] = w->var_076[k + 1];
				w->var_076[k + 1] = swapper;
			}
			break;
		case INFORMATION_TYPE_PROFIT:
			while (--k >= 0) {
				otherRide = &(RCT2_ADDRESS(RCT2_ADDRESS_RIDE_LIST, rct_ride)[w->var_076[k]]);
				if (ride->profit <= otherRide->profit)
					break;

				swapper = w->var_076[k];
				w->var_076[k] = w->var_076[k + 1];
				w->var_076[k + 1] = swapper;
			}
			break;
		case INFORMATION_TYPE_QUEUE_LENGTH:
			while (--k >= 0) {
				otherRide = &(RCT2_ADDRESS(RCT2_ADDRESS_RIDE_LIST, rct_ride)[w->var_076[k]]);
				if (ride_get_total_queue_length(ride) <= ride_get_total_queue_length(otherRide))
					break;

				swapper = w->var_076[k];
				w->var_076[k] = w->var_076[k + 1];
				w->var_076[k + 1] = swapper;
			}
			break;
		case INFORMATION_TYPE_QUEUE_TIME:
			while (--k >= 0) {
				otherRide = &(RCT2_ADDRESS(RCT2_ADDRESS_RIDE_LIST, rct_ride)[w->var_076[k]]);
				if (ride_get_max_queue_time(ride) <= ride_get_max_queue_time(otherRide))
					break;

				swapper = w->var_076[k];
				w->var_076[k] = w->var_076[k + 1];
				w->var_076[k + 1] = swapper;
			}
			break;
		case INFORMATION_TYPE_RELIABILITY:
			while (--k >= 0) {
				otherRide = &(RCT2_ADDRESS(RCT2_ADDRESS_RIDE_LIST, rct_ride)[w->var_076[k]]);
				if (ride->reliability <= otherRide->reliability)
					break;

				swapper = w->var_076[k];
				w->var_076[k] = w->var_076[k + 1];
				w->var_076[k + 1] = swapper;
			}
			break;
		case INFORMATION_TYPE_DOWN_TIME:
			while (--k >= 0) {
				otherRide = &(RCT2_ADDRESS(RCT2_ADDRESS_RIDE_LIST, rct_ride)[w->var_076[k]]);
				if (ride->downtime <= otherRide->downtime)
					break;

				swapper = w->var_076[k];
				w->var_076[k] = w->var_076[k + 1];
				w->var_076[k + 1] = swapper;
			}
			break;
		case INFORMATION_TYPE_GUESTS_FAVOURITE:
			while (--k >= 0) {
				otherRide = &(RCT2_ADDRESS(RCT2_ADDRESS_RIDE_LIST, rct_ride)[w->var_076[k]]);
				if (ride->guests_favourite <= otherRide->guests_favourite)
					break;

				swapper = w->var_076[k];
				w->var_076[k] = w->var_076[k + 1];
				w->var_076[k + 1] = swapper;
			}
			break;
		}

		j++;
	}

	w->var_47A = -1;
	window_invalidate(w);
}

static void window_ride_list_close_all(rct_window *w)
{
	int i;
	rct_ride *ride;

	for (i = 0; i < MAX_RIDES; i++) {
		ride = &RCT2_ADDRESS(RCT2_ADDRESS_RIDE_LIST, rct_ride)[i];
		if (ride->type == RIDE_TYPE_NULL)
			continue;
		if (w->page != gRideClassifications[ride->type])
			continue;
		if (ride->status == RIDE_STATUS_CLOSED)
			continue;
		RCT2_GLOBAL(0x013CE952 + 6, uint16) = w->scrolls[0].v_top;
		RCT2_GLOBAL(0x013CE952 + 8, uint32) = w->scrolls[0].v_bottom;

		game_do_command(0, 1, 0, i, 8, 0, 0);
	}
}

static void window_ride_list_open_all(rct_window *w)
{
	int i;
	rct_ride *ride;

	for (i = 0; i < MAX_RIDES; i++) {
		ride = &RCT2_ADDRESS(RCT2_ADDRESS_RIDE_LIST, rct_ride)[i];
		if (ride->type == RIDE_TYPE_NULL)
			continue;
		if (w->page != gRideClassifications[ride->type])
			continue;
		if (ride->status == RIDE_STATUS_OPEN)
			continue;
		RCT2_GLOBAL(0x013CE952 + 6, uint16) = w->scrolls[0].v_top;
		RCT2_GLOBAL(0x013CE952 + 8, uint32) = w->scrolls[0].v_bottom;

		game_do_command(0, 1, 0, (1 << 8) | i, 8, 0, 0);
	}
}
