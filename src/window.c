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
#include "osinterface.h"
#include "rct2.h"
#include "widget.h"
#include "window.h"
#include "viewport.h"

#define RCT2_FIRST_WINDOW		(RCT2_ADDRESS(RCT2_ADDRESS_WINDOW_LIST, rct_window))
#define RCT2_LAST_WINDOW		(RCT2_GLOBAL(RCT2_ADDRESS_NEW_WINDOW_PTR, rct_window*) - 1)
#define RCT2_NEW_WINDOW			(RCT2_GLOBAL(RCT2_ADDRESS_NEW_WINDOW_PTR, rct_window*))

static void window_all_wheel_input();
static int window_draw_split(rct_window *w, int left, int top, int right, int bottom);

int window_get_widget_index(rct_window *w, rct_widget *widget)
{
	rct_widget *widget2;

	int i = 0;
	for (widget2 = w->widgets; widget2->type != WWT_LAST; widget2++, i++)
		if (widget == widget2)
			return i;
	return -1;
}

int window_get_scroll_index(rct_window *w, int targetWidgetIndex)
{
	int widgetIndex, scrollIndex;
	rct_widget *widget;

	if (w->widgets[targetWidgetIndex].type != WWT_SCROLL)
		return -1;

	scrollIndex = 0;
	widgetIndex = 0;
	for (widget = w->widgets; widget->type != WWT_LAST; widget++, widgetIndex++) {
		if (widgetIndex == targetWidgetIndex)
			break;
		if (widget->type == WWT_SCROLL)
			scrollIndex++;
	}

	return scrollIndex;
}

int window_get_scroll_index_from_widget(rct_window *w, rct_widget *widget)
{
	int scrollIndex;
	rct_widget *widget2;

	if (widget->type != WWT_SCROLL)
		return -1;

	scrollIndex = 0;
	for (widget2 = w->widgets; widget2->type != WWT_LAST; widget2++) {
		if (widget2 == widget)
			break;
		if (widget2->type == WWT_SCROLL)
			scrollIndex++;
	}

	return scrollIndex;
}

rct_widget *window_get_scroll_widget(rct_window *w, int scrollIndex)
{
	rct_widget *widget;

	for (widget = w->widgets; widget->type != WWT_LAST; widget++) {
		if (widget->type != WWT_SCROLL)
			continue;

		if (scrollIndex == 0)
			return widget;
		scrollIndex--;
	}

	return NULL;
}

/**
 * 
 *  rct2: 0x006ED7B0
 */
void window_dispatch_update_all()
{
	rct_window *w;

	RCT2_GLOBAL(0x01423604, sint32)++;
	RCT2_GLOBAL(RCT2_ADDRESS_TOOLTIP_NOT_SHOWN_TICKS, sint16)++;
	for (w = RCT2_LAST_WINDOW; w >= RCT2_FIRST_WINDOW; w--)
		RCT2_CALLPROC_X(w->event_handlers[WE_UPDATE], 0, 0, 0, 0, w, 0, 0);

	RCT2_CALLPROC_EBPSAFE(0x006EE411);	// handle_text_input
}

/**
 * 
 *  rct2: 0x006E77A1
 */
void window_update_all()
{
	rct_window* w;

	RCT2_GLOBAL(0x009E3CD8, sint32)++;
	// if (RCT2_GLOBAL(0x009E3CD8, sint32) == 224 && RCT2_GLOBAL(0x009ABDF2, sint8) != 0)
	//	RCT2_CALLPROC(0x004067E3); // ddwindow_move_to_top_corner

	if (RCT2_GLOBAL(0x009ABDF2, sint8) == 0)
		return;

	gfx_draw_all_dirty_blocks();

	for (w = RCT2_FIRST_WINDOW; w < RCT2_NEW_WINDOW; w++)
		if (w->viewport != NULL)
			viewport_update_position(w);

	// 1000 tick update
	RCT2_GLOBAL(0x009DEB7C, sint16) += RCT2_GLOBAL(0x009DE588, sint16);
	if (RCT2_GLOBAL(0x009DEB7C, sint16) >= 1000) {
		RCT2_GLOBAL(0x009DEB7C, sint16) = 0;
		for (w = RCT2_LAST_WINDOW; w >= RCT2_FIRST_WINDOW; w--)
			RCT2_CALLPROC_X(w->event_handlers[WE_UNKNOWN_07], 0, 0, 0, 0, w, 0, 0);
	}

	// Border flash invalidation
	for (w = RCT2_LAST_WINDOW; w >= RCT2_FIRST_WINDOW; w--) {
		if (w->flags & WF_WHITE_BORDER_MASK) {
			w->flags -= WF_WHITE_BORDER_ONE;
			if (!(w->flags & WF_WHITE_BORDER_MASK))
				window_invalidate(w);
		}
	}

	// RCT2_CALLPROC_X(0x006E7868, 0, 0, 0, 0, 0, RCT2_ADDRESS(RCT2_ADDRESS_SCREEN_DPI, rct_drawpixelinfo), 0); // process_mouse_wheel_input();
	window_all_wheel_input();
}

/**
 * 
 *  rct2: 0x006E78E3
 */
static void window_scroll_wheel_input(rct_window *w, int scrollIndex, int wheel)
{
	int widgetIndex, newValue, size;
	rct_scroll *scroll;
	rct_widget *widget;
	
	scroll = &w->scrolls[scrollIndex];
	widget = window_get_scroll_widget(w, scrollIndex);
	widgetIndex = window_get_widget_index(w, widget);

	if (scroll->flags & VSCROLLBAR_VISIBLE) {
		size = widget->bottom - widget->top - 1;
		if (scroll->flags & HSCROLLBAR_VISIBLE)
			size -= 11;
		size = max(0, scroll->v_bottom - size);
		scroll->v_top = min(max(0, scroll->v_top + wheel), size);
	} else {
		size = widget->right - widget->left - 1;
		if (scroll->flags & VSCROLLBAR_VISIBLE)
			size -= 11;
		size = max(0, scroll->h_right - size);
		scroll->h_left = min(max(0, scroll->h_left + wheel), size);
	}

	widget_scroll_update_thumbs(w, widgetIndex);
	widget_invalidate(w->classification, w->number, widgetIndex);
}

/**
 * 
 *  rct2: 0x006E793B
 */
static int window_wheel_input(rct_window *w, int wheel)
{
	int i;
	rct_widget *widget;
	rct_scroll *scroll;

	i = 0;
	for (widget = w->widgets; widget->type != WWT_LAST; widget++) {
		if (widget->type != WWT_SCROLL)
			continue;

		// Originally always checked first scroll view, bug maybe?
		scroll = &w->scrolls[i * sizeof(rct_scroll)];
		if (scroll->flags & (HSCROLLBAR_VISIBLE | VSCROLLBAR_VISIBLE)) {
			window_scroll_wheel_input(w, i, wheel);
			return 1;
		}
		i++;
	}

	return 0;
}

/**
 * 
 *  rct2: 0x006E79FB
 */
static void window_viewport_wheel_input(rct_window *w, int wheel)
{
	if (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & 9)
		return;

	if (wheel < 0)
		window_zoom_in(w);
	else if (wheel > 0)
		window_zoom_out(w);
}

/**
 * 
 *  rct2: 0x006E7868
 */
static void window_all_wheel_input()
{
	int i, raw, wheel, widgetIndex;
	rct_window *w;
	rct_widget *widget;
	rct_scroll *scroll;

	// Get wheel value
	raw = gCursorState.wheel;
	wheel = 0;
	while (1) {
		raw -= 120;
		if (raw < 0)
			break;
		wheel -= 17;
	}
	raw += 120;
	while (1) {
		raw += 120;
		if (raw > 0)
			break;
		wheel += 17;
	}
	raw -= 120;
	gCursorState.wheel = raw;

	if (wheel == 0)
		return;

	// Check window cursor is over
	if (!(RCT2_GLOBAL(0x009DE518, uint32) & (1 << 5))) {
		w = window_find_from_point(gCursorState.x, gCursorState.y);
		if (w != NULL) {
			// Check if main window
			if (w->classification == WC_MAIN_WINDOW) {
				window_viewport_wheel_input(w, wheel);
				return;
			}

			// Check scroll view, cursor is over
			widgetIndex = window_find_widget_from_point(w, gCursorState.x, gCursorState.y);
			if (widgetIndex != -1) {
				widget = &w->widgets[widgetIndex];
				if (widget->type == WWT_SCROLL) {
					scroll = &w->scrolls[RCT2_GLOBAL(0x01420075, uint8) * sizeof(rct_scroll)];
					if (scroll->flags & (HSCROLLBAR_VISIBLE | VSCROLLBAR_VISIBLE)) {
						window_scroll_wheel_input(w, window_get_scroll_index(w, widgetIndex), wheel);
						return;
					}
				}
				
				// Check other scroll views on window
				if (window_wheel_input(w, wheel))
					return;
			}
		}
	}

	// Check windows, front to back
	for (w = RCT2_LAST_WINDOW; w >= RCT2_FIRST_WINDOW; w--)
		if (window_wheel_input(w, wheel))
			return;
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
			if (!(w->flags & (WF_STICK_TO_BACK | WF_STICK_TO_FRONT | WF_9)))
				break;

		window_close(w);
	}

	w = RCT2_NEW_WINDOW;

	// Flags
	if (flags & WF_STICK_TO_BACK) {
		for (; w >= RCT2_FIRST_WINDOW + 1; w--) {
			if ((w - 1)->flags & WF_STICK_TO_FRONT)
				continue;
			if ((w - 1)->flags & WF_STICK_TO_BACK)
				break;
		}
	} else if (!(flags & WF_STICK_TO_FRONT)) {
		for (; w >= RCT2_FIRST_WINDOW + 1; w--) {
			if (!((w - 1)->flags & WF_STICK_TO_FRONT))
				break;
		}
	}

	// Move w to new window slot
	if (w != RCT2_NEW_WINDOW)
		*RCT2_NEW_WINDOW = *w;

	// Setup window
	w->classification = cls;
	w->var_4B8 = -1;
	w->var_4B9 = -1;
	w->flags = flags;

	// Play sound
	if (!(flags & (WF_STICK_TO_BACK | WF_STICK_TO_FRONT)))
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
	w->page = 0;
	w->var_48C = 0;
	w->var_48E = 0;
	w->var_490 = 0;
	w->var_492 = 0;
	w->var_4AC = 0;
	w->var_4AE = 0;
	RCT2_NEW_WINDOW++;

	window_invalidate(w);
	return w;
}

/**
 * Opens a new window, supposedly automatically positioned
 *  rct2: 0x006EA9B1
 *
 * @param width (bx)
 * @param height (ebx >> 16)
 * @param events (edx)
 * @param flags (ch)
 * @param class (cl)
 */
rct_window *window_create_auto_pos(int width, int height, uint32 *event_handlers, rct_windowclass cls, uint16 flags)
{
	int eax, ebx, ecx, edx, esi, edi, ebp;

	ebx = (height << 16) | width;
	ecx = (flags << 8) | cls;
	edx = event_handlers;
	RCT2_CALLFUNC_X(0x006EA9B1, &eax, &ebx, &ecx, &edx, &esi, &edi, &ebp);
	return esi;
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
	RCT2_CALLPROC_X(window->event_handlers[WE_CLOSE], 0, 0, 0, 0, window, 0, 0);

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

	viewport_update_pointers();
}

/**
 * 
 *  rct2: 0x006ECCF4
 * @param cls (cl)
 * @param number (dx)
 */
void window_close_by_id(rct_windowclass cls, rct_windownumber number)
{
	rct_window* w;
	
	if (cls & 0x80) {
		for (w = RCT2_FIRST_WINDOW; w < RCT2_NEW_WINDOW; w++) {
			if (w->classification == cls) {
				window_close(w);
				w = RCT2_FIRST_WINDOW - 1;
			}
		}
	} else {
		for (w = RCT2_FIRST_WINDOW; w < RCT2_NEW_WINDOW; w++) {
			if (w->classification == cls && w->number == number) {
				window_close(w);
				w = RCT2_FIRST_WINDOW - 1;
			}
		}
	}
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
 * 
 *  rct2: 0x006EA845
 */
rct_window *window_find_from_point(int x, int y)
{
	rct_window *w, *w2;
	rct_widget *widget;
	int widget_index;

	for (w = RCT2_LAST_WINDOW; w >= RCT2_FIRST_WINDOW; w--) {
		if (x < w->x || x >= w->x + w->width || y < w->y || y >= w->y + w->height)
			continue;

		if (w->flags & WF_5) {
			widget_index = window_find_widget_from_point(w, x, y);
			if (widget_index == -1)
				continue;

			widget = &w->widgets[widget_index];
		}
		
		return w;
	}

	return NULL;
}

/**
 * 
 *  rct2: 0x006EA594
 * x (ax)
 * y (bx)
 * returns widget_index (edx)
 * EDI NEEDS TO BE SET TO w->widgets[widget_index] AFTER
 */
int window_find_widget_from_point(rct_window *w, int x, int y)
{
	rct_widget *widget;
	int i, widget_index;

	// Invalidate the window
	RCT2_CALLPROC_X(w->event_handlers[WE_INVALIDATE], 0, 0, 0, 0, w, 0, 0);

	// Find the widget at point x, y
	widget_index = -1;
	RCT2_GLOBAL(0x01420074, uint8) = -1;
	for (i = 0;; i++) {
		widget = &w->widgets[i];
		if (widget->type == WWT_LAST) {
			break;
		} else if (widget->type != WWT_EMPTY) {
			if (widget->type == WWT_SCROLL)
				RCT2_GLOBAL(0x01420074, uint8)++;

			if (x >= w->x + widget->left && x <= w->x + widget->right &&
				y >= w->y + widget->top && y <= w->y + widget->bottom
			) {
				widget_index = i;
				RCT2_GLOBAL(0x01420075, uint8) = RCT2_GLOBAL(0x01420074, uint8);
			}
		}
	}

	// Return next widget if a dropdown
	if (widget_index != -1)
		if (w->widgets[widget_index].type == WWT_DROPDOWN)
			widget_index++;

	// Return the widget index
	return widget_index;
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

void widget_invalidate(rct_windowclass cls, rct_windownumber number, int widgetIndex)
{
	window_invalidate_by_id((widgetIndex << 8) | 0x80 | cls, number);
}

/**
 * Initialises scroll widgets to their virtual size.
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

/**
 * 
 *  rct2: 0x006EAE4E
 *
 * @param w The window (esi).
 */
void window_update_scroll_widgets(rct_window *w)
{
	RCT2_CALLPROC_X(0x006EAE4E, 0, 0, 0, 0, w, 0, 0);
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

/**
 * 
 *  rct2: 0x006EE6EA
 */
void window_push_others_below(rct_window *w1)
{
	int push_amount;
	rct_window* w2;

	// Enumerate through all other windows
	for (w2 = RCT2_FIRST_WINDOW; w2 < RCT2_NEW_WINDOW; w2++) {
		if (w1 == w2)
			continue;

		// ?
		if (w2->flags & 3)
			continue;

		// Check if w2 intersects with w1
		if (w2->x > (w1->x + w1->width) || w2->x + w2->width < w1->x)
			continue;
		if (w2->y > (w1->y + w1->height) || w2->y + w2->height < w1->y)
			continue;

		// Check if there is room to push it down
		if (w1->y + w1->height + 80 >= RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_HEIGHT, sint16))
			continue;

		// Invalidate the window's current area
		window_invalidate(w2);

		push_amount = w1->y + w1->height - w2->y + 3;
		w2->y += push_amount;

		// Invalidate the window's new area
		window_invalidate(w2);

		// Update viewport position if necessary
		if (w2->viewport != NULL)
			w2->viewport->y += push_amount;
	}
}

/**
 * 
 *  rct2: 0x006EE2E4
 */
rct_window *window_get_main()
{
	rct_window* w;
	
	for (w = RCT2_FIRST_WINDOW; w < RCT2_NEW_WINDOW; w++)
		if (w->classification == WC_MAIN_WINDOW)
			return w;

	return NULL;
}

/**
 * 
 *  rct2: 0x006E7C9C
 */
void window_scroll_to_location(rct_window *w, int x, int y, int z)
{
	RCT2_CALLPROC_X(0x006E7C9C, x, 0, y, z, w, 0, 0);
}

/**
 * 
 *  rct2: 0x0068881A
 */
void window_rotate_camera(rct_window *w)
{
	RCT2_CALLPROC_X(0x0068881A, 0, 0, 0, 0, w, 0, 0);
}

/**
 * 
 *  rct2: 0x006887A6
 */
void window_zoom_in(rct_window *w)
{
	RCT2_CALLPROC_X(0x006887A6, 0, 0, 0, 0, w, 0, 0);
}

/**
 * 
 *  rct2: 0x006887E0
 */
void window_zoom_out(rct_window *w)
{
	RCT2_CALLPROC_X(0x006887E0, 0, 0, 0, 0, w, 0, 0);
}

/**
 * 
 *  rct2: 0x006EE308
 */
void window_show_textinput(rct_window *w, int widgetIndex, uint16 title, uint16 text, int value)
{
	RCT2_CALLPROC_X(0x006EE308, title, text, value, widgetIndex, w, 0, 0);
}

/**
 * Draws a window that is in the specified region.
 *  rct2: 0x006E756C
 * left (ax)
 * top (bx)
 * right (dx)
 * bottom (bp)
 */
void window_draw(rct_window *w, int left, int top, int right, int bottom)
{
	rct_window* v;
	rct_drawpixelinfo *dpi, copy;
	int overflow;

	char *copyBits;
	int copyLeft;
	int copyWidth;
	int copyPitch;

	// RCT2_CALLPROC_X(0x006E756C, left, top, 0, right, w, 0, bottom);
	// return;

	// Split window into only the regions that require drawing
	if (window_draw_split(w, left, top, right, bottom))
		return;

	// Clamp region
	left = max(left, w->x);
	top = max(top, w->y);
	right = min(right, w->x + w->width);
	bottom = min(bottom, w->y + w->height);
	if (left >= right)
		return;
	if (top >= bottom)
		return;

	// Draw the window in this region
	for (v = w; v < RCT2_NEW_WINDOW; v++) {
		// Don't draw overlapping opaque windows, they won't have changed
		if (w != v && !(v->flags & WF_TRANSPARENT))
			continue;

		copy = *RCT2_ADDRESS(RCT2_ADDRESS_WINDOW_DPI, rct_drawpixelinfo);
		dpi = &copy;

		// Clamp left to 0
		overflow = left - dpi->x;
		if (overflow > 0) {
			dpi->x += overflow;
			dpi->width -= overflow;
			if (dpi->width <= 0)
				continue;
			dpi->pitch += overflow;
			dpi->bits += overflow;
		}

		// Clamp width to right
		overflow = dpi->x + dpi->width - right;
		if (overflow > 0) {
			dpi->width -= overflow;
			if (dpi->width <= 0)
				continue;
			dpi->pitch += overflow;
		}

		// Clamp top to 0
		overflow = top - dpi->y;
		if (overflow > 0) {
			dpi->y += overflow;
			dpi->height -= overflow;
			if (dpi->height <= 0)
				continue;
			dpi->bits += (dpi->width + dpi->pitch) * overflow;
		}

		// Clamp height to bottom
		overflow = dpi->y + dpi->height - bottom;
		if (overflow > 0) {
			dpi->height -= overflow;
			if (dpi->height <= 0)
				continue;
		}

		RCT2_GLOBAL(0x01420070, sint32) = v->x;

		// Text colouring
		RCT2_GLOBAL(0x0141F740, uint8) = v->colours[0] & 0x7F;
		RCT2_GLOBAL(0x0141F741, uint8) = v->colours[1] & 0x7F;
		RCT2_GLOBAL(0x0141F742, uint8) = v->colours[2] & 0x7F;
		RCT2_GLOBAL(0x0141F743, uint8) = v->colours[3] & 0x7F;

		// Invalidate the window
		RCT2_CALLPROC_X(v->event_handlers[WE_INVALIDATE], 0, 0, 0, 0, v, 0, 0);

		// Paint the window
		RCT2_CALLPROC_X(v->event_handlers[WE_PAINT], 0, 0, 0, 0, v, dpi, 0);
	}
}

/**
 * Splits a drawing of a window into regions that can be seen and are not hidden
 * by other opaque overlapping windows.
 */
static int window_draw_split(rct_window *w, int left, int top, int right, int bottom)
{
	rct_window* topwindow;

	// Divide the draws up for only the visible regions of the window recursively
	for (topwindow = w + 1; topwindow < RCT2_NEW_WINDOW; topwindow++) {
		// Check if this window overlaps w
		if (topwindow->x >= right || topwindow->y >= bottom)
			continue;
		if (topwindow->x + topwindow->width <= left || topwindow->y + topwindow->height <= top)
			continue;
		if (topwindow->flags & WF_TRANSPARENT)
			continue;

		// A window overlaps w, split up the draw into two regions where the window starts to overlap
		if (topwindow->x > left) {
			// Split draw at topwindow.left
			window_draw(w, left, top, topwindow->x, bottom);
			window_draw(w, topwindow->x, top, right, bottom);
		} else if (topwindow->x + topwindow->width < right) {
			// Split draw at topwindow.right
			window_draw(w, left, top, topwindow->x + topwindow->width, bottom);
			window_draw(w, topwindow->x + topwindow->width, top, right, bottom);
		} else if (topwindow->y > top) {
			// Split draw at topwindow.top
			window_draw(w, left, top, right, topwindow->y);
			window_draw(w, left, topwindow->y, right, bottom);
		} else if (topwindow->y + topwindow->height < bottom) {
			// Split draw at topwindow.bottom
			window_draw(w, left, top, right, topwindow->y + topwindow->height);
			window_draw(w, left, topwindow->y + topwindow->height, right, bottom);
		}

		// Drawing for this region should be done now, exit
		return 1;
	}

	// No windows overlap
	return 0;
}

/**
 * 
 *  rct2: 0x006EB15C
 *
 * @param window (esi)
 * @param dpi (edi)
 */
void window_draw_widgets(rct_window *w, rct_drawpixelinfo *dpi)
{
	rct_widget *widget;
	int widgetIndex;

	// RCT2_CALLPROC_X(0x006EB15C, 0, 0, 0, 0, w, dpi, 0);
	// return;

	if ((w->flags & WF_TRANSPARENT) && !(w->flags & 0x20))
		gfx_fill_rect(dpi, w->x, w->y, w->x + w->width - 1, w->y + w->height - 1, 0x2000000 | 51);

	widgetIndex = 0;
	for (widget = w->widgets; widget->type != WWT_LAST; widget++) {
		// Check if widget is outside the draw region
		if (w->x + widget->left < dpi->x + dpi->width && w->x + widget->right > dpi->x)
			if (w->y + widget->top < dpi->y + dpi->height && w->y + widget->bottom > dpi->y)
				widget_draw(dpi, w, widgetIndex);

		widgetIndex++;
	}
}

/**
 * 
 *  rct2: 0x00685BE1
 *
 * @param dpi (edi)
 * @param w (esi)
 */
void window_draw_viewport(rct_drawpixelinfo *dpi, rct_window *w)
{
	viewport_render(dpi, w->viewport, dpi->x, dpi->y, dpi->x + dpi->width, dpi->y + dpi->height);
}

void window_move_position(rct_window *w, int dx, int dy)
{
	if (dx == 0 && dy == 0)
		return;

	// Invalidate old region
	window_invalidate(w);

	// Translate window and viewport
	w->x += dx;
	w->y += dy;
	if (w->viewport != NULL) {
		w->viewport->x += dx;
		w->viewport->y += dy;
	}

	// Invalidate new region
	window_invalidate(w);
}

void window_resize(rct_window *w, int dw, int dh)
{
	int i;
	if (dw == 0 && dh == 0)
		return;

	// Invalidate old region
	window_invalidate(w);

	// Clamp new size to minimum and maximum
	w->width = clamp(w->min_width, w->width + dw, w->max_width);
	w->height = clamp(w->min_height, w->height + dh, w->max_height);

	RCT2_CALLPROC_X(w->event_handlers[WE_RESIZE], w->width, w->height, 0, 0, w, 0, 0);
	RCT2_CALLPROC_X(w->event_handlers[WE_INVALIDATE], 0, 0, 0, 0, w, 0, 0);

	// Update scroll widgets
	for (i = 0; i < 3; i++) {
		w->scrolls[i].h_right = -1;
		w->scrolls[i].v_bottom = -1;
	}
	window_update_scroll_widgets(w);

	// Invalidate new region
	window_invalidate(w);
}

void window_set_resize(rct_window *w, int minWidth, int minHeight, int maxWidth, int maxHeight)
{
	w->min_width = minWidth;
	w->min_height = minHeight;
	w->max_width = maxWidth;
	w->max_height = maxHeight;

	// Clamp width and height to minimum and maximum
	if (w->width < minWidth) {
		w->width = minWidth;
		window_invalidate(w);
	}
	if (w->height < minHeight) {
		w->height = minHeight;
		window_invalidate(w);
	}
	if (w->width > maxWidth) {
		w->width = maxWidth;
		window_invalidate(w);
	}
	if (w->height > maxHeight) {
		w->height = maxHeight;
		window_invalidate(w);
	}
}

/**
 * 
 *  rct2: 0x006EE212
 *
 * @param tool (al)
 * @param widgetIndex (dx)
 * @param w (esi)
 */
int tool_set(rct_window *w, int widgetIndex, int tool)
{
	if (RCT2_GLOBAL(0x009DE518, uint32) & (1 << 3)) {
		if (
			w->classification == RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WINDOWCLASS, rct_windowclass) &&
			w->number == RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WINDOWNUMBER, rct_windownumber) &&
			widgetIndex == RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WIDGETINDEX, uint16)
		) {
			tool_cancel();
			return 1;
		}
	}

	RCT2_GLOBAL(0x009DE518, uint32) |= (1 << 3);
	RCT2_GLOBAL(0x009DE518, uint32) &= ~(1 << 6);
	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_TOOL, uint8) = tool;
	RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WINDOWCLASS, rct_windowclass) = w->classification;
	RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WINDOWNUMBER, rct_windownumber) = w->number;
	RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WIDGETINDEX, uint16) = widgetIndex;
	return 0;
}

/**
 * 
 *  rct2: 0x006EE281
 */
void tool_cancel()
{
	rct_window *w;

	if (RCT2_GLOBAL(0x009DE518, uint32) & (1 << 3)) {
		RCT2_GLOBAL(0x009DE518, uint32) &= ~(1 << 3);

		// 
		RCT2_CALLPROC_EBPSAFE(0x0068AAE1);
		RCT2_CALLPROC_EBPSAFE(0x0068AB1B);

		// Reset map selection
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) = 0;

		if (RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WIDGETINDEX, sint16) >= 0) {
			// Invalidate tool widget
			widget_invalidate(
				RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WINDOWCLASS, rct_windowclass),
				RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WINDOWNUMBER, rct_windownumber),
				RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WIDGETINDEX, uint16)
			);

			// Abort tool event
			w = window_find_by_id(
				RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WINDOWCLASS, rct_windowclass),
				RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WINDOWNUMBER, rct_windownumber)
			);
			if (w != NULL)
				RCT2_CALLPROC_X(w->event_handlers[WE_TOOL_ABORT], 0, 0, 0, RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WIDGETINDEX, uint16), w, 0, 0);
		}
	}
}