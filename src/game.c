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
#include "climate.h"
#include "config.h"
#include "rct2.h"
#include "game.h"
#include "news_item.h"
#include "osinterface.h"
#include "peep.h"
#include "screenshot.h"
#include "strings.h"
#include "title.h"
#include "tutorial.h"
#include "widget.h"
#include "window.h"
#include "window_error.h"
#include "window_tooltip.h"

void game_handle_input();
void game_handle_keyboard_input();

/**
 * 
 *  rct2: 0x0066B5C0 (part of 0x0066B3E8)
 */
void game_create_windows()
{
	window_main_open();
	window_game_top_toolbar_open();
	window_game_bottom_toolbar_open();
	RCT2_CALLPROC_EBPSAFE(0x0066B905);
}

void game_update()
{
	int eax, tmp;

	// 0x006E3AEC // screen_game_process_mouse_input();
	// RCT2_CALLPROC_EBPSAFE(0x006E3AEC); // screen_game_process_keyboard_input();
	screenshot_check();
	game_handle_keyboard_input();

	// do game logic
	eax = RCT2_GLOBAL(0x009DE588, uint16) / 31;
	if (eax == 0)
		eax = 1;
	if (eax > 4)
		eax = 4;

	// if (ted_fastforwarding)
	//	eax += 8 - 1;

	if (RCT2_GLOBAL(0x009DEA6E, uint8) == 0) {
		for (; eax > 0; eax--) {
			game_logic_update();
			RCT2_CALLPROC_EBPSAFE(0x006BD0F8); // play title screen music

			/*
			if (rctmem->dword_009E2D74 == 1) {
			rctmem->dword_009E2D74 = 0;
			break;
			} else {
			if (rctmem->input_state != INPUT_STATE_WIDGET_RESET && rctmem->input_state != INPUT_STATE_WIDGET_NORMAL)
			break;

			tmp = rctmem->dword_009DE518 & 0x80;
			rctmem->dword_009DE518 &= ~0x80;
			if (tmp)
			break;
			}
			*/
		}
	}


	RCT2_GLOBAL(0x009DE518, uint32) &= ~0x80;
	RCT2_GLOBAL(0x009AC861, uint16) &= ~0x8000;
	RCT2_GLOBAL(0x009AC861, uint16) &= ~0x02;
	tmp = RCT2_GLOBAL(0x009AC861, uint16) & 0x01;
	RCT2_GLOBAL(0x009AC861, uint16) &= ~0x01;
	if (!tmp)
		RCT2_GLOBAL(0x009AC861, uint16) |= 0x02;
	RCT2_GLOBAL(0x009AC861, uint16) &= ~0x08;
	tmp = RCT2_GLOBAL(0x009AC861, uint16) & 0x04;
	RCT2_GLOBAL(0x009AC861, uint16) &= ~0x04;
	if (!tmp)
		RCT2_GLOBAL(0x009AC861, uint16) |= 0x04;

	RCT2_CALLPROC_EBPSAFE(0x006EE77A);

	window_update_all();

	RCT2_GLOBAL(0x01388698, uint16)++;

	// Input
	RCT2_GLOBAL(0x0141F568, uint8) = RCT2_GLOBAL(0x0013CA740, uint8);
	game_handle_input();

	RCT2_CALLPROC_EBPSAFE(0x006838BD);
	RCT2_CALLPROC_EBPSAFE(0x00684218);

	if (RCT2_GLOBAL(0x009AAC73, uint8) != 255) {
		RCT2_GLOBAL(0x009AAC73, uint8)++;
		if (RCT2_GLOBAL(0x009AAC73, uint8) == 255)
			config_save();
	}
}

void game_logic_update()
{
	short stringId, _dx;

	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_TICKS, sint32)++;
	RCT2_GLOBAL(0x00F663AC, sint32)++;
	RCT2_GLOBAL(0x009DEA66, sint16)++;
	if (RCT2_GLOBAL(0x009DEA66, sint16) == 0)
		RCT2_GLOBAL(0x009DEA66, sint16)--;

	RCT2_CALLPROC_EBPSAFE(0x0068B089);
	RCT2_CALLPROC_EBPSAFE(0x006C44B1);	// update_objective
	climate_update();
	RCT2_CALLPROC_EBPSAFE(0x006646E1);
	RCT2_CALLPROC_EBPSAFE(0x006A876D);
	peep_update_all();
	RCT2_CALLPROC_EBPSAFE(0x006D4204);	// update vehicles
	RCT2_CALLPROC_EBPSAFE(0x00672AA4);	// update text effects
	RCT2_CALLPROC_EBPSAFE(0x006ABE4C);	// update rides
	RCT2_CALLPROC_EBPSAFE(0x006674F7);	// update park
	RCT2_CALLPROC_EBPSAFE(0x00684C7A);
	RCT2_CALLPROC_EBPSAFE(0x006B5A2A);
	RCT2_CALLPROC_EBPSAFE(0x006B6456);	// update ride measurements
	RCT2_CALLPROC_EBPSAFE(0x0068AFAD);
	RCT2_CALLPROC_EBPSAFE(0x006BBC6B);
	RCT2_CALLPROC_EBPSAFE(0x006BD18A);
	RCT2_CALLPROC_EBPSAFE(0x006BCB91);
	news_item_update_current();
	RCT2_CALLPROC_EBPSAFE(0x0067009A);

	// Update windows
	window_dispatch_update_all();

	if (RCT2_GLOBAL(0x009AC31B, uint8) != 0) {
		stringId = STR_UNABLE_TO_LOAD_FILE;
		_dx = RCT2_GLOBAL(0x009AC31C, uint16);
		if (RCT2_GLOBAL(0x009AC31B, uint8) != 254) {
			stringId = RCT2_GLOBAL(0x009AC31C, uint16);
			_dx = 0xFFFF;
		}
		RCT2_GLOBAL(0x009AC31B, uint8) = 0;

		window_error_open(stringId, _dx);
	}
}

static void game_handle_input_mouse();
static void game_get_next_input(int *x, int *y, int *state);

/**
 * 
 *  rct2: 0x006EA627
 */
void game_handle_input()
{
	rct_window *w;

	if (RCT2_GLOBAL(0x009DEA64, uint16) & 2) {
		RCT2_GLOBAL(0x009DEA64, uint16) &= ~2;
		RCT2_CALLPROC_X(0x006677F2, 0, 1, 0, 0, 5, 2, 0);
	}

	if (RCT2_GLOBAL(0x009ABDF2, uint8) != 0) {
		for (w = RCT2_ADDRESS(RCT2_ADDRESS_WINDOW_LIST, rct_window); w < RCT2_GLOBAL(RCT2_ADDRESS_NEW_WINDOW_PTR, rct_window*); w++)
			RCT2_CALLPROC_X(w->event_handlers[WE_UNKNOWN_07], 0, 0, 0, 0, w, 0, 0);

		RCT2_CALLPROC_EBPSAFE(0x006EA73F);
		RCT2_CALLPROC_EBPSAFE(0x006E8346); // update_cursor_position

		{
			int eax, ebx, ecx, edx, esi, edi, ebp;

			for (;;) {
				game_get_next_input(&eax, &ebx, &ecx);
				if (ecx == 0)
					break;

				game_handle_input_mouse(eax, ebx, ecx & 0xFF);
				// RCT2_CALLPROC_X(0x006E8655, eax, ebx, ecx, 0, 0, 0, 0); // window_process_mouse_input
			}

			if (RCT2_GLOBAL(0x009DE518, uint32) & (1 << 5)) {
				game_handle_input_mouse(eax, ebx, ecx);
				// RCT2_CALLPROC_X(0x006E8655, eax, ebx, 0, 0, 0, 0, 0); // window_process_mouse_input
			} else if (eax != 0x80000000) {
				eax = clamp(0, eax, RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_WIDTH, sint16) - 1);
				ebx = clamp(0, ebx, RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_HEIGHT, sint16) - 1);

				game_handle_input_mouse(eax, ebx, ecx);
				// RCT2_CALLPROC_X(0x006E8655, eax, ebx, 0, 0, 0, 0, 0); // window_process_mouse_input
				RCT2_CALLPROC_X(0x006ED833, eax, ebx, 0, 0, 0, 0, 0);
				RCT2_CALLPROC_EBPSAFE(0x006ED801);
			}
		}
	}

	for (w = RCT2_ADDRESS(RCT2_ADDRESS_WINDOW_LIST, rct_window); w < RCT2_GLOBAL(RCT2_ADDRESS_NEW_WINDOW_PTR, rct_window*); w++)
		RCT2_CALLPROC_X(w->event_handlers[WE_UNKNOWN_08], 0, 0, 0, 0, w, 0, 0);
}

/**
 * 
 *  rct2: 0x006E83C7
 */
static void game_get_next_input(int *x, int *y, int *state)
{
	int eax, ebx, ecx, edx, esi, edi, ebp;
	RCT2_CALLFUNC_X(0x00407074, &eax, &ebx, &ecx, &edx, &esi, &edi, &ebp);
	if (eax == 0) {
		*x = gCursorState.x;
		*y = gCursorState.y;
		*state = 0;
		return;
	}

	*x = RCT2_GLOBAL(eax + 0, sint32);
	*y = RCT2_GLOBAL(eax + 4, sint32);
	*state = RCT2_GLOBAL(eax + 8, sint32);

	//int eax, ebx, ecx, edx, esi, edi, ebp;
	//RCT2_CALLFUNC_X(0x006E83C7, &eax, &ebx, &ecx, &edx, &esi, &edi, &ebp);
	//*x = eax & 0xFFFF;
	//*y = ebx & 0xFFFF;
	//*state = ecx & 0xFF;
	//return;

	//int on_tutorial = RCT2_GLOBAL(RCT2_ADDRESS_ON_TUTORIAL, uint8);
	//if (RCT2_GLOBAL(0x009DE518, uint32) & (1 << 5)) {
	//	if (on_tutorial == 1) {

	//	} else {
	//		RCT2_CALLPROC_EBPSAFE(0x00407074);
	//	}
	//	if (on_tutorial == 2) {

	//	}

	//} else {

	//}
}

#include <windows.h>
POINT _dragPosition;

static void input_mouseover(int x, int y, rct_window *w, int widgetIndex);
static void input_mouseover_widget_check(rct_windowclass windowClass, rct_windownumber windowNumber, int widgetIndex);
static void input_mouseover_widget_flatbutton_invalidate();
static void input_leftmousedown(int x, int y, rct_window *w, int widgetIndex);


/**
 * 
 *  rct2: 0x006E8655
 */
static void game_handle_input_mouse(int x, int y, int state)
{
	rct_window *w, *w2;
	rct_widget *widget;
	int widgetIndex;
	rct_windowclass windowClass;
	rct_windownumber windowNumber;

	// Get window and widget under cursor position
	w = window_find_from_point(x, y);
	widgetIndex = w == NULL ? -1 : window_find_widget_from_point(w, x, y);
	widget = widgetIndex == -1 ? 0 : &w->widgets[widgetIndex];

	switch (RCT2_GLOBAL(RCT2_ADDRESS_INPUT_STATE, uint8)) {
	case INPUT_STATE_RESET:
		window_tooltip_reset(x, y);
		// fall-through
	case INPUT_STATE_NORMAL:
		switch (state) {
		case 0:
			input_mouseover(x, y, w, widgetIndex);
			break;
		case 1:
			input_leftmousedown(x, y, w, widgetIndex);
			break;
		case 3:
			// Close tooltip
			window_close_by_id(5, 0);
			
			if (w != NULL)
				w = window_bring_to_front(w);
			
			if (widgetIndex == -1)
				break;
			
			if (widget->type == WWT_VIEWPORT) {
				if (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & 9)
					break;
				w->flags &= ~(1 << 3);
				RCT2_GLOBAL(RCT2_ADDRESS_INPUT_STATE, uint8) = INPUT_STATE_VIEWPORT_DRAG;
				RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DRAG_LAST_X, sint16) = x;
				RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DRAG_LAST_Y, sint16) = y;
				RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DRAG_WINDOWCLASS, rct_windowclass) = w->classification;
				RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DRAG_WINDOWNUMBER, rct_windownumber) = w->number;
				RCT2_GLOBAL(0x009DE540, sint16) = 0;
				// hide cursor
				// RCT2_CALLPROC_X(0x00407045, 0, 0, 0, 0, 0, 0, 0);
				// RCT2_GLOBAL(0x009DE518, uint32) |= (1 << 5);

				GetCursorPos(&_dragPosition);
				ShowCursor(FALSE);

			} else if (widget->type == WWT_SCROLL) {

			}

			break;
		}

		break;
	case INPUT_STATE_WIDGET_PRESSED:
		RCT2_CALLPROC_X(0x006E8DA7, x, y, state, widgetIndex, w, widget, 0);
		break;
	case INPUT_STATE_DRAGGING:
		// RCT2_CALLPROC_X(0x006E8C5C, x, y, state, widgetIndex, w, widget, 0);

		w = window_find_by_id(RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DRAG_WINDOWCLASS, rct_windowclass), RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DRAG_WINDOWNUMBER, rct_windownumber));
		if (w == NULL) {
			RCT2_GLOBAL(RCT2_ADDRESS_INPUT_STATE, uint8) = INPUT_STATE_RESET;
			break;
		}

		if (state == 0) {
			y = clamp(29, y, RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_HEIGHT, uint16) - 34);
			window_move_position(
				w,
				x - RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DRAG_LAST_X, sint16),
				y - RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DRAG_LAST_Y, sint16)
			);
			RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DRAG_LAST_X, sint16) = x;
			RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DRAG_LAST_Y, sint16) = y;
		} else if (state == 2) {
			RCT2_GLOBAL(RCT2_ADDRESS_INPUT_STATE, uint8) = INPUT_STATE_NORMAL;
			RCT2_GLOBAL(RCT2_ADDRESS_TOOLTIP_TIMEOUT, uint8) = 0;
			RCT2_GLOBAL(RCT2_ADDRESS_TOOLTIP_WIDGET_INDEX, uint8) = RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WIDGETINDEX, sint16);
			RCT2_GLOBAL(RCT2_ADDRESS_TOOLTIP_WINDOW_CLASS, rct_windowclass) = RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WINDOWCLASS, rct_windowclass);
			RCT2_GLOBAL(RCT2_ADDRESS_TOOLTIP_WINDOW_NUMBER, rct_windownumber) = RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WINDOWNUMBER, rct_windownumber);
			y = clamp(29, y, RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_HEIGHT, uint16) - 34);
			window_move_position(
				w,
				x - RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DRAG_LAST_X, sint16),
				y - RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DRAG_LAST_Y, sint16)
			);
			RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DRAG_LAST_X, sint16) = x;
			RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DRAG_LAST_Y, sint16) = y;

			RCT2_CALLPROC_X(w->event_handlers[WE_UNKNOWN_18], 0, 0, x, y, w, 0, 0);
		}
		break;
	case INPUT_STATE_VIEWPORT_DRAG:
	{
		int dx, dy;
		
		dx = x - RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DRAG_LAST_X, sint16);
		dy = y - RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DRAG_LAST_Y, sint16);
		w = window_find_by_id(RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DRAG_WINDOWCLASS, rct_windowclass), RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DRAG_WINDOWNUMBER, rct_windownumber));
		if (state == 0) {
			rct_viewport *viewport = w->viewport;
			RCT2_GLOBAL(0x009DE540, sint16) += RCT2_GLOBAL(0x009DE588, sint16);
			if (viewport == NULL) {
				ShowCursor(TRUE);
				RCT2_GLOBAL(RCT2_ADDRESS_INPUT_STATE, uint8) = INPUT_STATE_RESET;
			} else if (dx != 0 || dy != 0) {
				if (!(w->flags & (1 << 2))) {
					RCT2_GLOBAL(0x009DE540, sint16) = 1000;
					dx <<= viewport->zoom + 1;
					dy <<= viewport->zoom + 1;
					w->var_4B2 += dx;
					w->var_4B4 += dy;
				}
			}
		} else if (state == 4) {
			ShowCursor(TRUE);
			RCT2_GLOBAL(RCT2_ADDRESS_INPUT_STATE, uint8) = INPUT_STATE_RESET;
			if (RCT2_GLOBAL(0x009DE540, sint16) < 500) {
				// Right click
				{
					int eax, ebx, ecx, edx, esi, edi, ebp;
					eax = RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DRAG_LAST_X, sint16);
					ebx = RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DRAG_LAST_Y, sint16);
					RCT2_CALLFUNC_X(0x006EDE88, &eax, &ebx, &ecx, &edx, &esi, &edi, &ebp);
					switch (ebx & 0xFF) {
					case 2:
						if (*((uint8*)edx) == 0)
							RCT2_CALLPROC_X(0x006B4857, eax, 0, ecx, 0, 0, 0, 0);
						break;
					case 3:
						RCT2_CALLPROC_X(0x006CC056, eax, 0, ecx, edx, 0, 0, 0);
						break;
					case 5:
						RCT2_CALLPROC_X(0x006E08D2, eax, 0, ecx, edx, 0, 0, 0);
						break;
					case 6:
						RCT2_CALLPROC_X(0x006A614A, eax, 0, ecx, edx, 0, 0, 0);
						break;
					case 7:
						RCT2_CALLPROC_X(0x006A61AB, eax, 0, ecx, edx, 0, 0, 0);
						break;
					case 8:
						RCT2_CALLPROC_X(0x00666C0E, eax, 0, ecx, edx, 0, 0, 0);
						break;
					case 9:
						RCT2_CALLPROC_X(0x006E57A9, eax, 0, ecx, edx, 0, 0, 0);
						break;
					case 10:
						RCT2_CALLPROC_X(0x006B88DC, eax, 0, ecx, edx, 0, 0, 0);
						break;
					case 12:
						RCT2_CALLPROC_X(0x006BA233, eax, 0, ecx, edx, 0, 0, 0);
						break;
					default:
						break;
					}
				}
			}
		}

		//
		//
		SetCursorPos(_dragPosition.x, _dragPosition.y);
		// RCT2_CALLPROC_X(0x006E89C6, x - RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DRAG_LAST_X, sint16), y - RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DRAG_LAST_Y, sint16), state, widgetIndex, w, widget, 0);
		break;
	}
	case INPUT_STATE_DROPDOWN_ACTIVE:
		RCT2_CALLPROC_X(0x006E8DA7, x, y, state, widgetIndex, w, widget, 0);
		break;
	case INPUT_STATE_VIEWPORT_LEFT:
		RCT2_CALLPROC_X(0x006E87B4, x, y, state, widgetIndex, w, widget, 0);
		break;
	case INPUT_STATE_SCROLL_LEFT:
		RCT2_CALLPROC_X(0x006E8676, x, y, state, widgetIndex, w, widget, 0);
		break;
	case INPUT_STATE_RESIZING:
		// RCT2_CALLPROC_X(0x006E8B46, x, y, state, widgetIndex, w, widget, 0);

		w = window_find_by_id(RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DRAG_WINDOWCLASS, rct_windowclass), RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DRAG_WINDOWNUMBER, rct_windownumber));
		if (w == NULL) {
			RCT2_GLOBAL(RCT2_ADDRESS_INPUT_STATE, uint8) = INPUT_STATE_RESET;
			break;
		}

		if (state != 0 && state != 2)
			break;
		if (state == 2) {
			RCT2_GLOBAL(RCT2_ADDRESS_INPUT_STATE, uint8) = INPUT_STATE_NORMAL;
			RCT2_GLOBAL(RCT2_ADDRESS_TOOLTIP_TIMEOUT, uint8) = 0;
			RCT2_GLOBAL(RCT2_ADDRESS_TOOLTIP_WIDGET_INDEX, uint8) = RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WIDGETINDEX, sint16);
			RCT2_GLOBAL(RCT2_ADDRESS_TOOLTIP_WINDOW_CLASS, rct_windowclass) = RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WINDOWCLASS, rct_windowclass);
			RCT2_GLOBAL(RCT2_ADDRESS_TOOLTIP_WINDOW_NUMBER, rct_windownumber) = RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WINDOWNUMBER, rct_windownumber);
		}

		if (y < RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_HEIGHT, uint16) - 2) {
			window_resize(
				w,
				x - RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DRAG_LAST_X, sint16),
				y - RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DRAG_LAST_Y, sint16)
			);
		}
		RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DRAG_LAST_X, sint16) = x;
		RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DRAG_LAST_Y, sint16) = y;
		break;
	case 9:
		RCT2_CALLPROC_X(0x006E8ACB, x, y, state, widgetIndex, w, widget, 0);
		break;
	}
}

/**
 * 
 *  rct2: 0x006E9253
 */
static void input_mouseover(int x, int y, rct_window *w, int widgetIndex)
{
	// RCT2_CALLPROC_X(0x006E9253, x, y, state, widgetIndex, w, widget, 0);

	rct_windowclass windowClass = 255;
	rct_windownumber windowNumber = 0;
	rct_widget *widget;

	if (w != NULL) {
		windowClass = w->classification;
		windowNumber = w->number;
		widget = &w->widgets[widgetIndex];
	}

	input_mouseover_widget_check(windowClass, windowNumber, widgetIndex);

	if (w != NULL && widgetIndex != -1 && widget->type == WWT_SCROLL) {
		int eax, ebx, ecx, edx, esi, edi, ebp;
		eax = x;
		ebx = y;
		esi = w;
		edi = widget;
		RCT2_CALLFUNC_X(0x006E9F92, &eax, &ebx, &ecx, &edx, &esi, &edi, &ebp); // widget_scoll_get_part
		eax &= 0xFFFF;
		ebx &= 0xFFFF;
		ecx &= 0xFFFF;
		edx &= 0xFFFF;
		if (ecx < 0)
			goto showTooltip;
		if (ecx == 0) {
			RCT2_CALLPROC_X(w->event_handlers[WE_SCROLL_MOUSEOVER], edx, 0, eax, ebx, w, 0, 0);
			goto showTooltip;
		} else {

		}
	} else {
		showTooltip:
		if (RCT2_GLOBAL(RCT2_ADDRESS_TOOLTIP_WINDOW_CLASS, rct_windowclass) == 255) {
			if (RCT2_GLOBAL(RCT2_ADDRESS_TOOLTIP_NOT_SHOWN_TICKS, uint16) < 500 ||
				(RCT2_GLOBAL(RCT2_ADDRESS_TOOLTIP_CURSOR_X, sint16) == x &&
				RCT2_GLOBAL(RCT2_ADDRESS_TOOLTIP_CURSOR_Y, sint16) == y)
			) {
				RCT2_GLOBAL(RCT2_ADDRESS_TOOLTIP_TIMEOUT, uint16) = RCT2_GLOBAL(0x009DE588, uint16);

				int bp = 2000;
				if (RCT2_GLOBAL(RCT2_ADDRESS_TOOLTIP_NOT_SHOWN_TICKS, uint16) <= 1000)
					bp = 0;
				if (bp > RCT2_GLOBAL(RCT2_ADDRESS_TOOLTIP_TIMEOUT, uint16))
					return;

				window_tooltip_open(w, widgetIndex, x, y);
				// RCT2_CALLPROC_X(0x006EA10D, x, y, 0, widgetIndex, w, widget, 0); // window_tooltip_open();
			}
		} else {
			if (RCT2_GLOBAL(RCT2_ADDRESS_TOOLTIP_WINDOW_CLASS, rct_windowclass) != w->classification ||
				RCT2_GLOBAL(RCT2_ADDRESS_TOOLTIP_WINDOW_NUMBER, rct_windownumber) != w->number ||
				RCT2_GLOBAL(RCT2_ADDRESS_TOOLTIP_WIDGET_INDEX, uint16) != widgetIndex
			) {
				window_tooltip_close();
			}
			RCT2_GLOBAL(RCT2_ADDRESS_TOOLTIP_TIMEOUT, uint16) += RCT2_GLOBAL(0x009DE588, uint16);
			if (RCT2_GLOBAL(RCT2_ADDRESS_TOOLTIP_TIMEOUT, uint16) < 8000)
				return;
			window_close_by_id(WC_TOOLTIP, 0);
		}
	}

	RCT2_GLOBAL(RCT2_ADDRESS_TOOLTIP_TIMEOUT, uint16) = 0;
	RCT2_GLOBAL(RCT2_ADDRESS_TOOLTIP_CURSOR_X, sint16) = x;
	RCT2_GLOBAL(RCT2_ADDRESS_TOOLTIP_CURSOR_Y, sint16) = y;
}

/**
 * 
 *  rct2: 0x006E9269
 */
static void input_mouseover_widget_check(rct_windowclass windowClass, rct_windownumber windowNumber, int widgetIndex)
{
	rct_window *w;

	// Check if widget cursor was over has changed
	if (windowClass != RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_OVER_WINDOWCLASS, rct_windowclass) ||
		windowNumber != RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_OVER_WINDOWNUMBER, rct_windownumber) ||
		widgetIndex != RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_OVER_WIDGETINDEX, rct_windownumber)
	) {
		// Invalidate last widget cursor was on if widget is a flat button
		input_mouseover_widget_flatbutton_invalidate();

		// Set new cursor over widget
		RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_OVER_WINDOWCLASS, rct_windowclass) = windowClass;
		RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_OVER_WINDOWNUMBER, rct_windowclass) = windowNumber;
		RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_OVER_WIDGETINDEX, rct_windowclass) = widgetIndex;

		// Invalidate new widget cursor is on if widget is a flat button
		if (windowClass != 255)
			input_mouseover_widget_flatbutton_invalidate();
	}
}

static void input_mouseover_widget_flatbutton_invalidate()
{
	rct_window *w = window_find_by_id(RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_OVER_WINDOWCLASS, rct_windowclass), RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_OVER_WINDOWNUMBER, rct_windownumber));
	if (w == NULL)
		return;

	RCT2_CALLPROC_X(w->event_handlers[WE_INVALIDATE], 0, 0, 0, 0, w, 0, 0);
	if (w->widgets[RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_OVER_WIDGETINDEX, rct_windownumber)].type == WWT_FLATBTN)
		widget_invalidate(RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_OVER_WINDOWCLASS, rct_windowclass), RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_OVER_WINDOWNUMBER, rct_windownumber), RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_OVER_WIDGETINDEX, rct_windownumber));
}

/**
 * 
 *  rct2: 0x006E95F9
 */
static void input_leftmousedown(int x, int y, rct_window *w, int widgetIndex)
{
	// RCT2_CALLPROC_X(0x006E95F9, x, y, state, widgetIndex, w, widget, 0);

	rct_windowclass windowClass = 255;
	rct_windownumber windowNumber = 0;
	rct_widget *widget;

	if (w != NULL) {
		windowClass = w->classification;
		windowNumber = w->number;
		widget = &w->widgets[widgetIndex];
	}

	window_close_by_id(WC_ERROR, 0);
	window_close_by_id(WC_TOOLTIP, 0);

	w = window_find_by_id(windowClass, windowNumber);
	if (w == NULL)
		return;

	w = window_bring_to_front(w);
	if (widgetIndex == -1)
		return;

	switch (widget->type) {
	case WWT_FRAME:
	case WWT_RESIZE:
		if (!(w->flags & WF_RESIZABLE))
			break;
		if (w->min_width == w->max_width && w->min_height == w->max_height)
			break;
		if (x < w->x + w->width - 19 || y < w->y + w->height - 19)
			break;

		RCT2_GLOBAL(RCT2_ADDRESS_INPUT_STATE, uint8) = INPUT_STATE_RESIZING;
		RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WIDGETINDEX, uint16) = widgetIndex;
		RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DRAG_LAST_X, uint16) = x;
		RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DRAG_LAST_Y, uint16) = y;
		RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DRAG_WINDOWCLASS, rct_windowclass) = windowClass;
		RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DRAG_WINDOWNUMBER, rct_windownumber) = windowNumber;
		break;
	case WWT_VIEWPORT:
		RCT2_GLOBAL(RCT2_ADDRESS_INPUT_STATE, uint8) = INPUT_STATE_VIEWPORT_LEFT;
		RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DRAG_LAST_X, uint16) = x;
		RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DRAG_LAST_Y, uint16) = y;
		RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DRAG_WINDOWCLASS, rct_windowclass) = windowClass;
		RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DRAG_WINDOWNUMBER, rct_windownumber) = windowNumber;
		if (!(RCT2_GLOBAL(0x009DE518, uint32) & (1 << 3)))
			break;

		w = window_find_by_id(RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WINDOWCLASS, rct_windowclass), RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WINDOWNUMBER, rct_windownumber));
		if (w == NULL)
			break;

		RCT2_CALLPROC_X(w->event_handlers[WE_TOOL_DOWN], x, y, 0, RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WIDGETINDEX, uint16), w, 0, 0);
		RCT2_GLOBAL(0x009DE518, uint32) |= (1 << 4);
		break;
	case WWT_CAPTION:
		RCT2_GLOBAL(RCT2_ADDRESS_INPUT_STATE, uint8) = INPUT_STATE_DRAGGING;
		RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WIDGETINDEX, uint16) = widgetIndex;
		RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DRAG_LAST_X, uint16) = x;
		RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DRAG_LAST_Y, uint16) = y;
		RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DRAG_WINDOWCLASS, rct_windowclass) = windowClass;
		RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DRAG_WINDOWNUMBER, rct_windownumber) = windowNumber;
		break;
	case WWT_SCROLL:
		RCT2_GLOBAL(RCT2_ADDRESS_INPUT_STATE, uint8) = INPUT_STATE_SCROLL_LEFT;
		RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WIDGETINDEX, uint16) = widgetIndex;
		RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WINDOWCLASS, rct_windowclass) = windowClass;
		RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WINDOWNUMBER, rct_windownumber) = windowNumber;
		RCT2_GLOBAL(RCT2_ADDRESS_TOOLTIP_CURSOR_X, uint16) = x;
		RCT2_GLOBAL(RCT2_ADDRESS_TOOLTIP_CURSOR_Y, uint16) = y;

		int eax, ebx, ecx, edx, esi, edi, ebp;
		eax = x;
		ebx = y;
		esi = w;
		edi = widget;
		RCT2_CALLFUNC_X(0x006E9F92, &eax, &ebx, &ecx, &edx, &esi, &edi, &ebp); // widget_scoll_get_part
		eax &= 0xFFFF;
		ebx &= 0xFFFF;
		ecx &= 0xFFFF;
		edx &= 0xFFFF;

		RCT2_GLOBAL(0x009DE548, uint16) = ecx;
		RCT2_GLOBAL(0x009DE54C, uint32) = edx;
		RCT2_CALLPROC_X(w->event_handlers[WE_UNKNOWN_15], RCT2_GLOBAL(0x009DE54C, uint32), ebx, ecx, edx, w, widget, 0);
		switch (ecx) {
		case SCROLL_PART_VIEW:
			RCT2_CALLPROC_X(w->event_handlers[WE_SCROLL_MOUSEDOWN], edx / sizeof(rct_scroll), ebx, eax, ebx, w, widget, 0);
			break;
		case SCROLL_PART_HSCROLLBAR_LEFT:
			// 0x006E9A60
			RCT2_CALLPROC_X(0x006E9A60, 0, 0, 0, 0, w, 0, 0);
			break;
		case SCROLL_PART_HSCROLLBAR_RIGHT:
			// 0x006E9ABF
			RCT2_CALLPROC_X(0x006E9ABF, 0, 0, 0, 0, w, 0, 0);
			break;
		case SCROLL_PART_HSCROLLBAR_LEFT_TROUGH:
			// 0x006E9B47
			RCT2_CALLPROC_X(0x006E9B47, 0, 0, 0, 0, w, 0, 0);
			break;
		case SCROLL_PART_HSCROLLBAR_RIGHT_TROUGH:
			// 0x006E9BB7
			RCT2_CALLPROC_X(0x006E9BB7, 0, 0, 0, 0, w, 0, 0);
			break;
		case SCROLL_PART_VSCROLLBAR_TOP:
			// 0x006E9C37
			RCT2_CALLPROC_X(0x006E9C37, 0, 0, 0, 0, w, 0, 0);
			break;
		case SCROLL_PART_VSCROLLBAR_BOTTOM:
			// 0x006E9C96
			RCT2_CALLPROC_X(0x006E9C96, 0, 0, 0, 0, w, 0, 0);
			break;
		case SCROLL_PART_VSCROLLBAR_TOP_TROUGH:
			// 0x006E9D1E
			RCT2_CALLPROC_X(0x006E9D1E, 0, 0, 0, 0, w, 0, 0);
			break;
		case SCROLL_PART_VSCROLLBAR_BOTTOM_TROUGH:
			// 0x006E9D8E
			RCT2_CALLPROC_X(0x006E9D8E, 0, 0, 0, 0, w, 0, 0);
			break;
		}
		break;
	default:
		if (!widget_is_enabled(w, widgetIndex))
			break;
		if (widget_is_disabled(w, widgetIndex))
			break;

		sound_play_panned(4, w->x + (widget->left + widget->right) / 2);
		
		// Set new cursor down widget
		RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WINDOWCLASS, rct_windowclass) = windowClass;
		RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WINDOWNUMBER, rct_windowclass) = windowNumber;
		RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WIDGETINDEX, rct_windowclass) = widgetIndex;
		RCT2_GLOBAL(0x009DE518, uint32) |= (1 << 0);
		RCT2_GLOBAL(RCT2_ADDRESS_INPUT_STATE, uint8) = INPUT_STATE_WIDGET_PRESSED;
		RCT2_GLOBAL(0x009DE528, uint16) = 1;

		widget_invalidate(windowClass, windowNumber, widgetIndex);
		RCT2_CALLPROC_X(w->event_handlers[WE_MOUSE_DOWN], 0, 0, 0, widgetIndex, w, widget, 0);
		break;
	}
}

void game_handle_edge_scroll()
{
	rct_window *mainWindow;
	int scrollX, scrollY;

	mainWindow = window_get_main();
	if (mainWindow == NULL)
		return;
	if ((mainWindow->flags & WF_2) || (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & 9))
		return;
	if (mainWindow->viewport == NULL)
		return;

	scrollX = 0;
	scrollY = 0;

	// Scroll left / right
	if (gCursorState.x == 0)
		scrollX = -1;
	else if (gCursorState.x == RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_WIDTH, uint16) - 1)
		scrollX = 1;

	// Scroll up / down
	if (gCursorState.y == 0)
		scrollY = -1;
	else if (gCursorState.y == RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_HEIGHT, uint16) - 1)
		scrollY = 1;

	// Scroll viewport
	if (scrollX != 0) {
		mainWindow->var_4B2 += scrollX * (12 << mainWindow->viewport->zoom);
		RCT2_GLOBAL(0x009DE518, uint32) |= (1 << 7);
	}
	if (scrollY != 0) {
		mainWindow->var_4B4 += scrollY * (12 << mainWindow->viewport->zoom);
		RCT2_GLOBAL(0x009DE518, uint32) |= (1 << 7);
	}
}

#include <SDL_keycode.h>

void game_handle_key_scroll()
{
	rct_window *mainWindow;
	int scrollX, scrollY;

	mainWindow = window_get_main();
	if (mainWindow == NULL)
		return;
	if ((mainWindow->flags & WF_2) || (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & 9))
		return;
	if (mainWindow->viewport == NULL)
		return;

	scrollX = 0;
	scrollY = 0;

	// Scroll left / right
	if (gKeysState[SDL_SCANCODE_LEFT])
		scrollX = -1;
	else if (gKeysState[SDL_SCANCODE_RIGHT])
		scrollX = 1;

	// Scroll up / down
	if (gKeysState[SDL_SCANCODE_UP])
		scrollY = -1;
	else if (gKeysState[SDL_SCANCODE_DOWN])
		scrollY = 1;

	// Scroll viewport
	if (scrollX != 0) {
		mainWindow->var_4B2 += scrollX * (12 << mainWindow->viewport->zoom);
		RCT2_GLOBAL(0x009DE518, uint32) |= (1 << 7);
	}
	if (scrollY != 0) {
		mainWindow->var_4B4 += scrollY * (12 << mainWindow->viewport->zoom);
		RCT2_GLOBAL(0x009DE518, uint32) |= (1 << 7);
	}
}

/**
 * 
 *  rct2: 0x00406CD2
 */
int get_next_key()
{
	int i;
	for (i = 0; i < 221; i++) {
		if (gKeysPressed[i]) {
			gKeysPressed[i] = 0;
			return i;
		}
	}

	return 0;
}

/**
 * 
 *  rct2: 0x006E3E68
 */
void handle_shortcut(int key)
{
	int i;
	for (i = 0; i < 32; i++) {
		if (key == gShortcutKeys[i]) {
			RCT2_CALLPROC_EBPSAFE(RCT2_ADDRESS(0x006E3FB4, uint32)[i]);
			break;
		}
	}
}

/**
 * 
 *  rct2: 0x006E3E91
 */
void set_shortcut(int key)
{
	int i;

	// Unmap shortcut that already uses this key
	for (i = 0; i < 32; i++) {
		if (key == gShortcutKeys[i]) {
			gShortcutKeys[i] = 0xFFFF;
			break;
		}
	}

	// Map shortcut to this key
	gShortcutKeys[RCT2_GLOBAL(0x009DE511, uint8)] = key;
	window_close_by_id(WC_CHANGE_KEYBOARD_SHORTCUT, 0);
	window_invalidate_by_id(WC_KEYBOARD_SHORTCUT_LIST, 0);
	config_save();
}

/**
 * 
 *  rct2: 0x006E3B43
 */
void game_handle_keyboard_input()
{
	rct_window *w;
	int key, i;

	// Handle mouse scrolling
	if (RCT2_GLOBAL(RCT2_ADDRESS_ON_TUTORIAL, uint8) == 0)
		if (RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_EDGE_SCROLLING, uint8) != 0)
			if (RCT2_GLOBAL(RCT2_ADDRESS_INPUT_STATE, uint8) == 1)
				if (!(RCT2_GLOBAL(RCT2_ADDRESS_PLACE_OBJECT_MODIFIER, uint8) & 3))
					game_handle_edge_scroll();

	// Handle modifier keys and key scrolling
	RCT2_GLOBAL(RCT2_ADDRESS_PLACE_OBJECT_MODIFIER, uint8) = 0;
	if (RCT2_GLOBAL(0x009E2B64, uint32) != 1) {
		if (gKeysState[SDL_SCANCODE_LSHIFT] || gKeysState[SDL_SCANCODE_RSHIFT])
			RCT2_GLOBAL(RCT2_ADDRESS_PLACE_OBJECT_MODIFIER, uint8) |= 1;
		if (gKeysState[SDL_SCANCODE_LCTRL] || gKeysState[SDL_SCANCODE_RCTRL])
			RCT2_GLOBAL(RCT2_ADDRESS_PLACE_OBJECT_MODIFIER, uint8) |= 2;
		if (RCT2_GLOBAL(RCT2_ADDRESS_ON_TUTORIAL, uint8) == 0)
			game_handle_key_scroll();
	}


	// Handle key input
	while ((key = get_next_key()) != 0) {
		if (key == 255 || key == 0x10 || key == 0x11)
			continue;

		key |= RCT2_GLOBAL(RCT2_ADDRESS_PLACE_OBJECT_MODIFIER, uint8) << 8;

		w = window_find_by_id(WC_CHANGE_KEYBOARD_SHORTCUT, 0);
		if (w != NULL)
			set_shortcut(key);
		else if (RCT2_GLOBAL(RCT2_ADDRESS_ON_TUTORIAL, uint8) == 1)
			tutorial_stop();
		else
			handle_shortcut(key);
	}

	if (RCT2_GLOBAL(RCT2_ADDRESS_ON_TUTORIAL, uint8) == 0)
		return;

	// Tutorial and the modifier key
	if (RCT2_GLOBAL(RCT2_ADDRESS_ON_TUTORIAL, uint8) == 1) {
		int eax, ebx, ecx, edx, esi, edi, ebp;
		RCT2_CALLFUNC_X(0x0066EEB4, &eax, &ebx, &ecx, &edx, &esi, &edi, &ebp);
		eax &= 0xFF;
		RCT2_GLOBAL(RCT2_ADDRESS_PLACE_OBJECT_MODIFIER, uint8) = eax;
		if (RCT2_GLOBAL(RCT2_ADDRESS_PLACE_OBJECT_MODIFIER, uint8) & 4) {
			window_tooltip_close();
			if ((w = window_get_main()) != NULL) {
				RCT2_CALLPROC_X(0x006EA2AA, 0, 0, 0, 0, w, RCT2_GLOBAL(0x009DEA72, uint16), 0);
				RCT2_GLOBAL(0x009DEA72, uint16)++;
			}
		}
	} else {
		if (!(RCT2_GLOBAL(RCT2_ADDRESS_PLACE_OBJECT_MODIFIER, uint8) & 4)) {
			window_tooltip_close();
			if ((w = window_get_main()) != NULL) {
				RCT2_CALLPROC_X(0x006EA2AA, 0, 0, 0, 0, w, RCT2_GLOBAL(0x009DEA72, uint16), 0);
				RCT2_GLOBAL(0x009DEA72, uint16)++;
			}
		}

		// Write tutorial input
		RCT2_CALLPROC_X(0x0066EEE1, RCT2_GLOBAL(RCT2_ADDRESS_PLACE_OBJECT_MODIFIER, uint8), 0, 0, 0, 0, 0, 0);
	}
}

/**
 * 
 *  rct2: 0x006677F2
 *
 * @param cost (ebp)
 */
static int game_check_affordability(int cost)
{
	int eax, ebx, ecx, edx, esi, edi, ebp;
	ebp = cost;
	RCT2_CALLFUNC_X(0x0069C62C, &eax, &ebx, &ecx, &edx, &esi, &edi, &ebp);
	return ebp;
}

static uint32 game_do_command_table[58];

/**
 * 
 *  rct2: 0x006677F2
 *
 * @param flags (ebx)
 * @param command (esi)
 */
int game_do_command(int eax, int ebx, int ecx, int edx, int esi, int edi, int ebp)
{
	int cost, flags, insufficientFunds;
	int original_ebx, original_edx, original_esi, original_edi, original_ebp;

	original_ebx = ebx;
	original_edx = edx;
	original_esi = esi;
	original_edi = edi;
	original_ebp = ebp;

	flags = ebx;
	RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TEXT, uint16) = 0xFFFF;

	// Increment nest count
	RCT2_GLOBAL(0x009A8C28, uint8)++;

	ebx &= ~1;

	// Primary command
	RCT2_CALLFUNC_X(game_do_command_table[esi], &eax, &ebx, &ecx, &edx, &esi, &edi, &ebp);
	cost = ebx;

	if (cost != 0x80000000) {
		// Check funds
		insufficientFunds = 0;
		if (RCT2_GLOBAL(0x009A8C28, uint8) == 1 && !(flags & 4) && !(flags & 0x20) && cost != 0)
			insufficientFunds = game_check_affordability(cost);

		if (insufficientFunds != 0x80000000) {
			ebx = original_ebx;
			edx = original_edx;
			esi = original_esi;
			edi = original_edi;
			ebp = original_ebp;

			if (!(flags & 1)) {
				// Decrement nest count
				RCT2_GLOBAL(0x009A8C28, uint8)--;
				return cost;
			}

			// Secondary command
			RCT2_CALLFUNC_X(game_do_command_table[esi], &eax, &ebx, &ecx, &edx, &esi, &edi, &ebp);
			edx = ebx;

			if (edx != 0x80000000 && edx < cost)
				cost = edx;

			// Decrement nest count
			RCT2_GLOBAL(0x009A8C28, uint8)--;
			if (RCT2_GLOBAL(0x009A8C28, uint8) != 0)
				return cost;

			// 
			if (!(flags & 0x20)) {
				// Update money balance
				RCT2_CALLPROC_X(0x0069C674, 0, cost, 0, 0, 0, 0, 0);
				if (RCT2_GLOBAL(0x0141F568, uint8) == RCT2_GLOBAL(0x013CA740, uint8)) {
					// Create a +/- money text effect
					if (cost != 0)
						RCT2_CALLPROC_X(0x0069C5D0, 0, cost, 0, 0, 0, 0, 0);
				}
			}

			return cost;
		}
	}

	// Error occured

	// Decrement nest count
	RCT2_GLOBAL(0x009A8C28, uint8)--;

	// Show error window
	if (RCT2_GLOBAL(0x009A8C28, uint8) == 0 && (flags & 1) && RCT2_GLOBAL(0x0141F568, uint8) == RCT2_GLOBAL(0x013CA740, uint8) && !(flags & 8))
		window_error_open(RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TITLE, uint16), RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TEXT, uint16));

	return 0x80000000;
}

/**
 * 
 *  rct2: 0x00667C15
 */
static void game_pause_toggle()
{
	rct_window *w;
	char input_bl, input_dl;
	short input_di;

	__asm mov input_bl, bl

	if (input_bl & 1) {
		RCT2_GLOBAL(0x009DEA6E, uint32) ^= 1;
		window_invalidate_by_id(WC_TOP_TOOLBAR, 0);
		if (RCT2_GLOBAL(0x009DEA6E, uint32) & 1)
			RCT2_CALLPROC_EBPSAFE(0x006BABB4); // pause_sounds
		else
			RCT2_CALLPROC_EBPSAFE(0x006BABD8); // unpause_sounds
	}

	__asm mov ebx, 0
}

/**
 * 
 *  rct2: 0x0066DB5F
 */
static void game_load_or_quit()
{
	rct_window *w;
	char input_bl, input_dl;
	short input_di;

	__asm mov input_bl, bl
	__asm mov input_dl, dl
	__asm mov input_di, di

	if (!(input_bl & 1))
		return 0;
	
	switch (input_dl) {
	case 0:
		RCT2_GLOBAL(0x009A9802, uint16) = input_di;
		window_save_prompt_open();
		break;
	case 1:
		window_close_by_id(WC_SAVE_PROMPT, 0);
		break;
	default:
		game_load_or_quit_no_save_prompt();
		break;
	}

	__asm mov ebx, 0
}

/**
 * 
 *  rct2: 0x00674F40
 */
static int open_landscape_file_dialog()
{
	int eax, ebx, ecx, edx, esi, edi, ebp;
	RCT2_CALLFUNC_X(0x00674F40, &eax, &ebx, &ecx, &edx, &esi, &edi, &ebp);
	return eax;
}

/**
 * 
 *  rct2: 0x00674EB6
 */
static int open_load_game_dialog()
{
	int eax, ebx, ecx, edx, esi, edi, ebp;
	RCT2_CALLFUNC_X(0x00674EB6, &eax, &ebx, &ecx, &edx, &esi, &edi, &ebp);
	return eax;
}

/**
 * 
 *  rct2: 0x0066DC0F
 */
static void load_landscape()
{
	if (open_landscape_file_dialog() == 0) {
		gfx_invalidate_screen();
	} else {
		// Set default filename
		char *esi = 0x0141EF67;
		while (1) {
			esi++;
			if (*esi == '.')
				break;
			if (*esi != 0)
				continue;
			strcpy(esi, ".SC6");
			break;
		}
		strcpy(0x009ABB37, 0x0141EF68);

		RCT2_CALLPROC_EBPSAFE(0x006758C0); // landscape_load
		if (1) {
			gfx_invalidate_screen();
			// game_loop_iteration
		} else {
			RCT2_GLOBAL(0x009DEA66, uint16) = 0;
			// game_loop_iteration
		}
	}
}

/**
 * 
 *  rct2: 0x0066DBB7
 */
static void load_game()
{
	if (open_load_game_dialog() == 0) {
		gfx_invalidate_screen();
	} else {
		// Set default filename
		char *esi = 0x0141EF67;
		while (1) {
			esi++;
			if (*esi == '.')
				break;
			if (*esi != 0)
				continue;
			strcpy(esi, ".SV6");
			break;
		}
		strcpy(0x009ABB37, 0x0141EF68);

		RCT2_CALLPROC_EBPSAFE(0x00675E1B); // game_load
		if (1) {
			gfx_invalidate_screen();
			// game_loop_iteration
		} else {
			RCT2_GLOBAL(0x009DEA66, uint16) = 0;
			// game_loop_iteration
		}
	}
}

/**
 * 
 *  rct2: 0x006E3879
 */
static void rct2_exit()
{
	RCT2_CALLPROC_EBPSAFE(0x006E3879);
}

/**
 * 
 *  rct2: 0x0066DB79
 */
void game_load_or_quit_no_save_prompt()
{
	if (RCT2_GLOBAL(0x009A9802, uint16) < 1) {
		game_do_command(0, 1, 0, 1, 5, 0, 0);
		tool_cancel();
		if (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & 2)
			load_landscape();
		else
			load_game();
	} else if (RCT2_GLOBAL(0x009A9802, uint16) == 1) {
		game_do_command(0, 1, 0, 1, 5, 0, 0);
		if (RCT2_GLOBAL(0x009DE518, uint32) & (1 << 5)) {
			RCT2_CALLPROC_EBPSAFE(0x0040705E);
			RCT2_GLOBAL(0x009DE518, uint32) &= ~(1 << 5);
		}
		title_load();
		// game_loop_iteration
	} else {
		rct2_exit();
	}
}

#pragma region Game command function table

static uint32 game_do_command_table[58] = {
	0x006B2FC5,
	0x0066397F,
	game_pause_toggle,
	0x006C511D,
	0x006C5B69,
	game_load_or_quit,
	0x006B3F0F,
	0x006B49D9,
	0x006B4EA6,
	0x006B52D4,
	0x006B578B,
	0x006B5559,
	0x006660A8,
	0x0066640B,
	0x006E0E01,
	0x006E08F4,
	0x006E650F,
	0x006A61DE,
	0x006A68AE,
	0x006A67C0,
	0x00663CCD,
	0x006B53E9,
	0x00698D6C,
	0x0068C542,
	0x0068C6D1,
	0x0068BC01,
	0x006E66A0,
	0x006E6878,
	0x006C5AE9,
	0x006BEFA1,
	0x006C09D1,
	0x006C0B83,
	0x006C0BB5,
	0x00669C6D,
	0x00669D4A,
	0x006649BD,
	0x006666E7,
	0x00666A63,
	0x006CD8CE,
	0x00669E30,
	0x00669E55,
	0x006E519A,
	0x006E5597,
	0x006B893C,
	0x006B8E1B,
	0x0069DFB3,
	0x00684A7F,
	0x006D13FE,
	0x0069E73C,
	0x006CDEE4,
	0x006B9E6D,
	0x006BA058,
	0x006E0F26,
	0x006E56B5,
	0x006B909A,
	0x006BA16A,
	0x006648E3,
	0x0068DF91
};

#pragma endregion