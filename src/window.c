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

#include "addresses.h"
#include "audio.h"
#include "gfx.h"
#include "rct2.h"
#include "widget.h"
#include "window.h"

#define RCT2_FIRST_WINDOW		(RCT2_ADDRESS(RCT2_ADDRESS_WINDOW_LIST, rct_window))
#define RCT2_LAST_WINDOW		(RCT2_GLOBAL(RCT2_ADDRESS_NEW_WINDOW_PTR, rct_window*) - 1)
#define RCT2_NEW_WINDOW			(RCT2_GLOBAL(RCT2_ADDRESS_NEW_WINDOW_PTR, rct_window*))

// rct2: 0x006ED7B0
void window_update_all()
{
	rct_window *w;

	RCT2_GLOBAL(0x01423604, sint32)++;
	RCT2_GLOBAL(RCT2_ADDRESS_TOOLTIP_NOT_SHOWN_TICKS, sint16)++;
	for (w = RCT2_LAST_WINDOW; w >= RCT2_FIRST_WINDOW; w--) {
		RCT2_CALLPROC_X(w->event_handlers[WE_UPDATE], 0, 0, 0, 0, w, 0, 0);
	}

	RCT2_CALLPROC_EBPSAFE(0x006EE411);
}

/**
 * Opens a new window.
 *  rct2: 0x006EACA4
 *
 * @param x (ax)
 * @param y (eax >> 16)
 * @param width (bx)
 * @param height (ebx >> 16)
 * @param events (edx)
 * @param flags (ch)
 * @param class (cl)
 */
rct_window *window_create(int x, int y, int width, int height, uint32 *event_handlers, rct_windowclass cls, uint16 flags)
{
	rct_window *w;

	// Check if there are any window slots left
	if (RCT2_NEW_WINDOW == &(RCT2_FIRST_WINDOW[12])) {
		// Close least recently used window
		for (w = RCT2_FIRST_WINDOW; w < RCT2_NEW_WINDOW; w++)
			if (!(w->flags & 0x203))
				break;

		window_close(w);
	}

	w = RCT2_NEW_WINDOW;

	// Flags
	if (flags & 0x01) {
		for (; w >= RCT2_FIRST_WINDOW + 1; w--)
			if (((w - 1)->flags & 0x01) && !((w - 1)->flags & 0x02))
				break;
	}
	else if (flags & 0x02) {
		for (; w >= RCT2_FIRST_WINDOW + 1; w--)
			if (!((w - 1)->flags & 0x02))
				break;
	}

	// Move w to new window slot
	if (w != RCT2_NEW_WINDOW)
		*RCT2_NEW_WINDOW = *w;

	// Setup window
	w->classification = cls;
	w->var_4B8 = -1;
	w->var_4B9 = -1;
	w->flags = 0;
	w->flags |= flags;

	// Play sound
	if (!(flags & 0x03))
		sound_play_panned(40, x + (width / 2));

	w->number = 0;
	w->x = x;
	w->y = y;
	w->width = width;
	w->height = height;
	w->viewport = NULL;
	w->event_handlers = event_handlers;
	w->enabled_widgets = 0;
	w->disabled_widgets = 0;
	w->pressed_widgets = 0;
	w->var_020 = 0;
	w->var_480 = 0;
	w->var_482 = 0;
	w->var_484 = 0;
	w->var_486 = 0;
	w->var_488 = 0;
	w->var_48A = 0;
	w->var_48C = 0;
	w->var_48E = 0;
	w->var_490 = 0;
	w->var_492 = 0;
	w->var_4AC = 0;
	w->var_4AE = 0;
	RCT2_NEW_WINDOW++;

	return w;
}

/**
 * Closes the specified window.
 *  rct2: 0x006ECD4C
 *
 * @param window The window to close (esi).
 */
void window_close(rct_window* window)
{
	int num_windows;

	if (window == NULL)
		return;

	// Call close event of window
	// window_call_basic_event(window, WE_CLOSE);

	window = window_find_by_id(window->classification, window->number);

	// Remove viewport
	if (window->viewport != NULL) {
		window->viewport->width = 0;
		window->viewport = NULL;
	}

	// Invalidate the window (area)
	window_invalidate(window);

	// Remove window from list and reshift all windows
	RCT2_NEW_WINDOW--;
	num_windows = (RCT2_NEW_WINDOW - window);
	if (num_windows > 0)
		memmove(window, window + 1, num_windows * sizeof(rct_window));

	// ?
	RCT2_CALLPROC(0x006EE510);
}

/**
 *
 *  rct2: 0x006EA8A0
 * @param cls (cl)
 * @param number (dx)
 */
rct_window *window_find_by_id(rct_windowclass cls, rct_windownumber number)
{
	rct_window *w;

	if (cls & 0x80) {
		for (w = RCT2_FIRST_WINDOW; w < RCT2_NEW_WINDOW; w++)
			if (w->classification == cls)
				return w;
	} else {
		for (w = RCT2_FIRST_WINDOW; w < RCT2_NEW_WINDOW; w++)
			if (w->classification == cls && w->number == number)
				return w;
	}

	return NULL;
}

/**
 * Invalidates the specified window.
 *  rct2: 0x006EB31A
 *
 * @param window The window to invalidate (esi).
 */
void window_invalidate(rct_window *window)
{
	if (window != NULL)
		gfx_set_dirty_blocks(window->x, window->y, window->x + window->width, window->y + window->height);
}

/**
 * 
 *  rct2: 0x006EC3AC
 *
 * @param cls (ax)
 * @param number (bx)
 */
void window_invalidate_by_id(uint16 cls, rct_windownumber number)
{
	rct_window* w;
	rct_widget* widget;
	uint8 widgetIndex;

	if (cls & 0x80) {
		widgetIndex = cls >> 8;
		cls &= 0x7F;
		for (w = RCT2_FIRST_WINDOW; w < RCT2_NEW_WINDOW; w++) {
			if (w->classification == cls && w->number == number) {
				widget = &w->widgets[widgetIndex];
				if (widget->left != -2) {
					gfx_set_dirty_blocks(w->x + widget->left, w->y + widget->top,
						w->x + widget->right + 1, w->y + widget->bottom + 1);
				}
			}
		}
	} else if (cls & 0x40) {
		cls &= 0xBF;
		for (w = RCT2_FIRST_WINDOW; w < RCT2_NEW_WINDOW; w++)
			if (w->classification == cls)
				window_invalidate(w);
	} else {
		for (w = RCT2_FIRST_WINDOW; w < RCT2_NEW_WINDOW; w++)
			if (w->classification == cls && w->number == number)
				window_invalidate(w);
	}
}

/**
 * Invalidates the specified window.
 *  rct2: 0x006EAEB8
 *
 * @param window The window (esi).
 */
void window_init_scroll_widgets(rct_window *w)
{
	rct_widget* widget;
	rct_scroll* scroll;
	int widget_index, scroll_index;

	widget_index = 0;
	scroll_index = 0;
	for (widget = w->widgets; widget->type != WWT_LAST; widget++) {
		if (widget->type != WWT_SCROLL) {
			widget_index++;
			continue;
		}

		scroll = &w->scrolls[scroll_index];

		{
			int _eax = 0, _ebx = scroll_index * sizeof(rct_scroll), _ecx = 0, _edx = 0, _esi = w, _edi = widget_index * sizeof(rct_widget), _ebp = 0;
			RCT2_CALLFUNC_X(w->event_handlers[WE_SCROLL_GETSIZE], & _eax, &_ebx, &_ecx, &_edx, &_esi, &_edi, &_ebp);

			scroll->h_left = 0;
			scroll->h_right = _ecx + 1;
			scroll->v_top = 0;
			scroll->v_bottom = _edx + 1;
		}

		if (widget->image & 0x01)
			scroll->flags |= HSCROLLBAR_VISIBLE;
		if (widget->image & 0x02)
			scroll->flags |= VSCROLLBAR_VISIBLE;

		widget_scroll_update_thumbs(w, widget_index);

		widget_index++;
		scroll_index++;
	}
}

int window_get_scroll_data_index(rct_window *w, int widget_index)
{
	int i, result;

	result = 0;
	for (i = 0; i < widget_index; i++) {
		if (w->widgets[i].type == WWT_SCROLL)
			result++;
	}
	return result;
}

/**
 * 
 *  rct2: 0x006ED78A
 * cls (cl)
 * number (dx)
 */
rct_window *window_bring_to_front_by_id(rct_windowclass cls, rct_windownumber number)
{
	rct_window* w;

	w = window_find_by_id(cls, number);
	if (w != NULL) {
		w->flags |= WF_WHITE_BORDER_MASK;
		window_invalidate(w);
		w = window_bring_to_front(w);
	}

	return w;
}

/**
 * 
 *  rct2: 0x006ECDA4
 */
rct_window *window_bring_to_front(rct_window *w)
{
	int i;
	rct_window* v, t;

	if (w->flags & 0x03)
		return w;

	for (v = RCT2_LAST_WINDOW; v >= RCT2_FIRST_WINDOW; v--)
		if (!(v->flags & 2))
			break;

	if (v >= RCT2_FIRST_WINDOW && w != v) {
		do {
			t = *w;
			*w = *(w + 1);
			*(w + 1) = t;
			w++;
		} while (w != v);

		window_invalidate(w);
	}

	if (w->x + w->width < 20) {
		i = 20 - w->x;
		w->x += i;
		if (w->viewport != NULL)
			w->viewport->x += i;
		window_invalidate(w);
	}

	return w;
}