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
#include "rct2.h"
#include "game.h"
#include "news_item.h"
#include "osinterface.h"
#include "peep.h"
#include "widget.h"
#include "window.h"

void game_handle_input();

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
		if (RCT2_GLOBAL(0x009AAC73, uint8) == 255) {
			RCT2_CALLPROC_EBPSAFE(0x00675487);
		}
	}
}

void game_logic_update()
{
	short _bx, _dx;

	RCT2_GLOBAL(0x013628F4, sint32)++;
	RCT2_GLOBAL(0x00F663AC, sint32)++;
	RCT2_GLOBAL(0x009DEA66, sint16)++;
	if (RCT2_GLOBAL(0x009DEA66, sint16) == 0)
		RCT2_GLOBAL(0x009DEA66, sint16)--;

	RCT2_CALLPROC_EBPSAFE(0x0068B089);
	RCT2_CALLPROC_EBPSAFE(0x006C44B1);	// update_objective
	RCT2_CALLPROC_EBPSAFE(0x006C46B1);	// update_climate
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
	RCT2_CALLPROC_EBPSAFE(0x006BD0F8);
	news_item_update_current();
	RCT2_CALLPROC_EBPSAFE(0x0067009A);

	// Update windows
	window_dispatch_update_all();

	if (RCT2_GLOBAL(0x009AC31B, uint8) != 0) {
		_bx = 3010;
		_dx = RCT2_GLOBAL(0x009AC31C, uint16);
		if (RCT2_GLOBAL(0x009AC31B, uint8) != 254) {
			_bx = RCT2_GLOBAL(0x009AC31C, uint16);
			_dx = 0xFFFF;
		}
		RCT2_GLOBAL(0x009AC31B, uint8) = 0;

		RCT2_CALLPROC_X(0x0066792F, 0, _bx, 0, _dx, 0, 0, 0);
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

/**
 * 
 *  rct2: 0x006E8655
 */
static void game_handle_input_mouse(int x, int y, int state)
{
	rct_window *w;
	rct_widget *widget;
	int widgetIndex;

	// Get window and widget under cursor position
	w = window_find_from_point(x, y);
	widgetIndex = w == NULL ? -1 : window_find_widget_from_point(w, x, y);
	widget = widgetIndex == -1 ? 0 : &w->widgets[widgetIndex];

	switch (RCT2_GLOBAL(RCT2_ADDRESS_INPUT_STATE, uint8)) {
	case INPUT_STATE_RESET:
	case INPUT_STATE_NORMAL:
		if (RCT2_GLOBAL(RCT2_ADDRESS_INPUT_STATE, uint8) == 0) {
			RCT2_GLOBAL(RCT2_ADDRESS_TOOLTIP_CURSOR_X, uint8) = x;
			RCT2_GLOBAL(RCT2_ADDRESS_TOOLTIP_CURSOR_Y, uint8) = x;
			RCT2_GLOBAL(RCT2_ADDRESS_TOOLTIP_TIMEOUT, uint8) = 0;
			RCT2_GLOBAL(RCT2_ADDRESS_TOOLTIP_WINDOW_CLASS, uint8) = 255;
			RCT2_GLOBAL(RCT2_ADDRESS_INPUT_STATE, uint8) = 1;
			RCT2_GLOBAL(RCT2_ADDRESS_INPUT_STATE, uint32) = 1;
		}
		switch (state) {
		case 0:
			RCT2_CALLPROC_X(0x006E9253, x, y, state, widgetIndex, w, widget, 0);
			break;
		case 1:
			RCT2_CALLPROC_X(0x006E95F9, x, y, state, widgetIndex, w, widget, 0);
			break;
		case 3:
			// Close tooltip
			window_close_by_id(5, 0);
			
			if (w != NULL)
				window_bring_to_front(w);
			
			if (widgetIndex == -1)
				break;
			
			if (widget->type == WWT_VIEWPORT) {
				if (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & 9)
					break;
				w->flags &= ~(1 << 3);
				RCT2_GLOBAL(RCT2_ADDRESS_INPUT_STATE, uint8) = INPUT_STATE_VIEWPORT_LEFT;
				RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DRAG_LAST_X, sint16) = x;
				RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DRAG_LAST_Y, sint16) = y;
				RCT2_GLOBAL(0x009DE530, rct_windowclass) = w->classification;
				RCT2_GLOBAL(0x009DE52E, rct_windownumber) = w->number;
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
		RCT2_CALLPROC_X(0x006E8C5C, x, y, state, widgetIndex, w, widget, 0);
		break;
	case INPUT_STATE_VIEWPORT_LEFT:
	{
		int dx, dy;
		
		dx = x - RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DRAG_LAST_X, sint16);
		dy = y - RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DRAG_LAST_Y, sint16);
		w = window_find_by_id(RCT2_GLOBAL(0x009DE530, rct_windowclass), RCT2_GLOBAL(0x009DE52E, rct_windownumber));
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
							RCT2_CALLPROC_X(0x006B4857, 0, 0, 0, 0, 0, 0, 0);
						break;
					case 3:
						RCT2_CALLPROC_X(0x006CC056, 0, 0, 0, edx, 0, 0, 0);
						break;
					case 5:
						RCT2_CALLPROC_X(0x006E08D2, 0, 0, 0, edx, 0, 0, 0);
						break;
					case 6:
						RCT2_CALLPROC_X(0x006A614A, 0, 0, 0, edx, 0, 0, 0);
						break;
					case 7:
						RCT2_CALLPROC_X(0x006A61AB, 0, 0, 0, edx, 0, 0, 0);
						break;
					case 8:
						RCT2_CALLPROC_X(0x00666C0E, 0, 0, 0, edx, 0, 0, 0);
						break;
					case 9:
						RCT2_CALLPROC_X(0x006E57A9, 0, 0, 0, edx, 0, 0, 0);
						break;
					case 10:
						RCT2_CALLPROC_X(0x006B88DC, 0, 0, 0, edx, 0, 0, 0);
						break;
					case 12:
						RCT2_CALLPROC_X(0x006BA233, 0, 0, 0, edx, 0, 0, 0);
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
	case INPUT_STATE_VIEWPORT_RIGHT:
		RCT2_CALLPROC_X(0x006E87B4, x, y, state, widgetIndex, w, widget, 0);
		break;
	case INPUT_STATE_SCROLL_LEFT:
		RCT2_CALLPROC_X(0x006E8676, x, y, state, widgetIndex, w, widget, 0);
		break;
	case INPUT_STATE_RESIZING:
		RCT2_CALLPROC_X(0x006E8B46, x, y, state, widgetIndex, w, widget, 0);
		break;
	case 9:
		RCT2_CALLPROC_X(0x006E8ACB, x, y, state, widgetIndex, w, widget, 0);
		break;
	}
}