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

#include <SDL_keycode.h>
#include <windows.h>
#include "addresses.h"
#include "audio/audio.h"
#include "config.h"
#include "game.h"
#include "input.h"
#include "interface/viewport.h"
#include "interface/widget.h"
#include "interface/window.h"
#include "platform/osinterface.h"
#include "tutorial.h"
#include "windows/tooltip.h"
#include "windows/dropdown.h"
#include "world/map.h"
#include "world/sprite.h"

POINT _dragPosition;

typedef struct {
	uint32 x, y;
	uint32 state; //1 = LeftDown 2 = LeftUp 3 = RightDown 4 = RightUp
} rct_mouse_data;

rct_mouse_data* mouse_buffer = RCT2_ADDRESS(RCT2_ADDRESS_INPUT_QUEUE, rct_mouse_data);

static void game_get_next_input(int *x, int *y, int *state);
static void input_mouseover(int x, int y, rct_window *w, int widgetIndex);
static void input_mouseover_widget_check(rct_windowclass windowClass, rct_windownumber windowNumber, int widgetIndex);
static void input_mouseover_widget_flatbutton_invalidate();
void process_mouse_over(int x, int y);
void process_mouse_tool(int x, int y);
void invalidate_scroll();
static rct_mouse_data* get_mouse_input();

#pragma region Scroll bar input

/**
 *  Horizontal scrollbar's "left" button held down, scroll it to the left
 *  rct2: 0x006E9A60
 */
static void input_hscrollbar_leftbutton(rct_window* w)
{
	rct_window* w2;
	rct_widget* widget;
	rct_scroll* scroll;
	uint16 widgetIndex;
	sint16 left;

	w2 = window_find_by_number(
		RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WINDOWCLASS, rct_windowclass),
		RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WINDOWNUMBER, rct_windownumber)
	);

	if (w2 == NULL)
		return;

	widgetIndex = RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WIDGETINDEX, uint16);

	widget = &w->widgets[widgetIndex];
	scroll = w->scrolls + RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_SCROLL_ID, uint32);

	left = scroll->h_left;
	left -= 3;
	if (left < 0)
		left = 0;
	scroll->h_left = left;

	widget_scroll_update_thumbs(w, widgetIndex);

	window_invalidate_by_number(
		RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WIDGETINDEX, uint8),
		RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WINDOWCLASS, uint8)
	);
}


/**
 *  Horizontal scrollbar's "right" button held down, scroll it to the right
 *  rct2: 0x006E9ABF
 */
static void input_hscrollbar_rightbutton(rct_window* w)
{
	rct_window* w2;
	rct_widget* widget;
	rct_scroll* scroll;
	uint16 widgetIndex;
	sint16 left, widgetWidth;

	w2 = window_find_by_number(
		RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WINDOWCLASS, rct_windowclass),
		RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WINDOWNUMBER, rct_windownumber)
	);

	if (w2 == NULL)
		return;

	widgetIndex = RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WIDGETINDEX, uint16);

	widget = &w->widgets[widgetIndex];
	scroll = w->scrolls + RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_SCROLL_ID, uint32);

	left = scroll->h_left;
	left += 3;

	widgetWidth = widget->right - widget->left - 1;
	if (scroll->flags & VSCROLLBAR_VISIBLE)
		widgetWidth -= 11;
	widgetWidth *= -1;
	widgetWidth += scroll->h_right;
	if (widgetWidth < 0)
		widgetWidth = 0;
	if (left > widgetWidth)
		left = widgetWidth;

	scroll->h_left = left;

	widget_scroll_update_thumbs(w, widgetIndex);

	window_invalidate_by_number(
		RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WIDGETINDEX, uint8),
		RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WINDOWCLASS, uint8)
	);
}

/**
 *  Horizontal scrollbar's left trough was clicked
 *  rct2: 0x006E9B47
 */
static void input_hscrollbar_left_trough(rct_window* w)
{
	rct_window* w2;
	rct_widget* widget;
	rct_scroll* scroll;
	uint16 widgetIndex;
	sint16 left, widgetWidth;

	w2 = window_find_by_number(
		RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WINDOWCLASS, rct_windowclass),
		RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WINDOWNUMBER, rct_windownumber)
	);

	if (w2 == NULL)
		return;

	widgetIndex = RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WIDGETINDEX, uint16);

	widget = &w->widgets[widgetIndex];
	scroll = w->scrolls + RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_SCROLL_ID, uint32);

	left = scroll->h_left;

	widgetWidth = widget->right - widget->left - 1;
	if (scroll->flags & VSCROLLBAR_VISIBLE)
		widgetWidth -= 11;
	left -= widgetWidth;
	if (left < 0)
		left = 0;
	scroll->h_left = left;

	widget_scroll_update_thumbs(w, widgetIndex);

	window_invalidate_by_number(
		RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WIDGETINDEX, uint8),
		RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WINDOWCLASS, uint8)
	);
}

/**
 *  Horizontal scrollbar's right trough was clicked
 *  rct2: 0x006E9BB7
 */
static void input_hscrollbar_right_trough(rct_window* w)
{
	rct_window* w2;
	rct_widget* widget;
	rct_scroll* scroll;
	uint16 widgetIndex;
	sint16 left, widgetWidth;

	w2 = window_find_by_number(
		RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WINDOWCLASS, rct_windowclass),
		RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WINDOWNUMBER, rct_windownumber)
	);

	if (w2 == NULL)
		return;

	widgetIndex = RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WIDGETINDEX, uint16);

	widget = &w->widgets[widgetIndex];
	scroll = w->scrolls + RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_SCROLL_ID, uint32);

	left = scroll->h_left;

	widgetWidth = widget->right - widget->left - 1;
	if (scroll->flags & VSCROLLBAR_VISIBLE)
		widgetWidth -= 11;
	left += widgetWidth;
	widgetWidth *= -1;
	widgetWidth += scroll->h_right;
	if (widgetWidth < 0)
		widgetWidth = 0;
	if (left > widgetWidth)
		left = widgetWidth;

	scroll->h_left = left;

	widget_scroll_update_thumbs(w, widgetIndex);

	window_invalidate_by_number(
		RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WIDGETINDEX, uint8),
		RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WINDOWCLASS, uint8)
	);
}

/**
 *  Vertical scrollbar's "top" button held down, scroll it upwards
 *  rct2: 0x006E9C37
 */
static void input_vscrollbar_topbutton(rct_window* w)
{
	rct_window* w2;
	rct_widget* widget;
	rct_scroll* scroll;
	uint16 widgetIndex;
	sint16 top;

	w2 = window_find_by_number(
		RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WINDOWCLASS, rct_windowclass),
		RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WINDOWNUMBER, rct_windownumber)
	);

	if (w2 == NULL)
		return;

	widgetIndex = RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WIDGETINDEX, uint16);

	widget = &w->widgets[widgetIndex];
	scroll = w->scrolls + RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_SCROLL_ID, uint32);

	top = scroll->v_top;
	top -= 3;
	if (top < 0)
		top = 0;
	scroll->v_top = top;

	widget_scroll_update_thumbs(w, widgetIndex);

	window_invalidate_by_number(
		RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WIDGETINDEX, uint8),
		RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WINDOWCLASS, uint8)
	);
}

/**
*  Vertical scrollbar's "bottom" button held down, scroll it downwards
*  rct2: 0x006E9C96
*/
static void input_vscrollbar_bottombutton(rct_window* w)
{
	rct_window* w2;
	rct_widget* widget;
	rct_scroll* scroll;
	uint16 widgetIndex;
	sint16 top, widgetHeight;

	w2 = window_find_by_number(
		RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WINDOWCLASS, rct_windowclass),
		RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WINDOWNUMBER, rct_windownumber)
	);

	if (w2 == NULL)
		return;

	widgetIndex = RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WIDGETINDEX, uint16);

	widget = &w->widgets[widgetIndex];
	scroll = w->scrolls + RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_SCROLL_ID, uint32);

	top = scroll->v_top;
	top += 3;

	widgetHeight = widget->bottom - widget->top - 1;
	if (scroll->flags & HSCROLLBAR_VISIBLE)
		widgetHeight -= 11;
	widgetHeight *= -1;
	widgetHeight += scroll->v_bottom;
	if (widgetHeight < 0)
		widgetHeight = 0;
	if (top > widgetHeight)
		top = widgetHeight;

	scroll->v_top = top;

	widget_scroll_update_thumbs(w, widgetIndex);

	window_invalidate_by_number(
		RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WIDGETINDEX, uint8),
		RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WINDOWCLASS, uint8)
	);
}

/**
*  Vertical scrollbar's top trough was clicked
*  rct2: 0x006E9D1E
*/
static void input_vscrollbar_top_trough(rct_window* w)
{
	rct_window* w2;
	rct_widget* widget;
	rct_scroll* scroll;
	uint16 widgetIndex;
	sint16 top, widgetHeight;

	w2 = window_find_by_number(
		RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WINDOWCLASS, rct_windowclass),
		RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WINDOWNUMBER, rct_windownumber)
	);

	if (w2 == NULL)
		return;

	widgetIndex = RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WIDGETINDEX, uint16);

	widget = &w->widgets[widgetIndex];
	scroll = w->scrolls + RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_SCROLL_ID, uint32);

	top = scroll->v_top;

	widgetHeight = widget->bottom - widget->top - 1;
	if (scroll->flags & HSCROLLBAR_VISIBLE)
		widgetHeight -= 11;
	top -= widgetHeight;
	if (top < 0)
		top = 0;
	scroll->v_top = top;

	widget_scroll_update_thumbs(w, widgetIndex);

	window_invalidate_by_number(
		RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WIDGETINDEX, uint8),
		RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WINDOWCLASS, uint8)
	);
}

/**
*  Vertical scrollbar's bottom trough was clicked
*  rct2: 0x006E9D8E
*/
static void input_vscrollbar_bottom_trough(rct_window* w)
{
	rct_window* w2;
	rct_widget* widget;
	rct_scroll* scroll;
	uint16 widgetIndex;
	sint16 top, widgetHeight;

	w2 = window_find_by_number(
		RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WINDOWCLASS, rct_windowclass),
		RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WINDOWNUMBER, rct_windownumber)
	);

	if (w2 == NULL)
		return;

	widgetIndex = RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WIDGETINDEX, uint16);

	widget = &w->widgets[widgetIndex];
	scroll = w->scrolls + RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_SCROLL_ID, uint32);

	top = scroll->v_top;

	widgetHeight = widget->bottom - widget->top - 1;
	if (scroll->flags & HSCROLLBAR_VISIBLE)
		widgetHeight -= 11;
	top += widgetHeight;
	widgetHeight *= -1;
	widgetHeight += scroll->v_bottom;
	if (widgetHeight < 0)
		widgetHeight = 0;
	if (top > widgetHeight)
		top = widgetHeight;

	scroll->v_top = top;

	widget_scroll_update_thumbs(w, widgetIndex);

	window_invalidate_by_number(
		RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WIDGETINDEX, uint8),
		RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WINDOWCLASS, uint8)
	);
}

#pragma endregion

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
	}

	window_close_by_class(WC_ERROR);
	window_close_by_class(WC_TOOLTIP);

	w = window_find_by_number(windowClass, windowNumber);
	if (w == NULL)
		return;

	w = window_bring_to_front(w);
	if (widgetIndex == -1)
		return;

	widget = &w->widgets[widgetIndex];

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
		if (!(RCT2_GLOBAL(RCT2_ADDRESS_INPUT_FLAGS, uint32) & INPUT_FLAG_TOOL_ACTIVE))
			break;

		w = window_find_by_number(
			RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WINDOWCLASS, rct_windowclass),
			RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WINDOWNUMBER, rct_windownumber)
		);
		if (w == NULL)
			break;

		RCT2_CALLPROC_X(w->event_handlers[WE_TOOL_DOWN], x, y, 0, RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WIDGETINDEX, uint16), (int)w, 0, 0);
		RCT2_GLOBAL(RCT2_ADDRESS_INPUT_FLAGS, uint32) |= INPUT_FLAG_4;
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

		int eax, ebx, ecx, edx;
		edx = 0; // safety
		widget_scroll_get_part(w, widget, x, y, &eax, &ebx, &ecx, &edx);

		RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_SCROLL_AREA, uint16) = ecx;
		RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_SCROLL_ID, uint32) = edx * 12;//We do this because scroll id is not all decompiled
		RCT2_CALLPROC_X(w->event_handlers[WE_UNKNOWN_15], RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_SCROLL_ID, uint32), ebx, ecx, edx, (int)w, (int)widget, 0);
		switch (ecx) {
		case SCROLL_PART_VIEW:
			RCT2_CALLPROC_X(w->event_handlers[WE_SCROLL_MOUSEDOWN], edx / sizeof(rct_scroll), ebx, eax, ebx, (int)w, (int)widget, 0);
			break;
		case SCROLL_PART_HSCROLLBAR_LEFT:           input_hscrollbar_leftbutton(w);    break;
		case SCROLL_PART_HSCROLLBAR_RIGHT:          input_hscrollbar_rightbutton(w);   break;
		case SCROLL_PART_HSCROLLBAR_LEFT_TROUGH:    input_hscrollbar_left_trough(w);   break;
		case SCROLL_PART_HSCROLLBAR_RIGHT_TROUGH:   input_hscrollbar_right_trough(w);  break;
		case SCROLL_PART_VSCROLLBAR_TOP:            input_vscrollbar_topbutton(w);     break;
		case SCROLL_PART_VSCROLLBAR_BOTTOM:         input_vscrollbar_bottombutton(w);  break;
		case SCROLL_PART_VSCROLLBAR_TOP_TROUGH:     input_vscrollbar_top_trough(w);    break;
		case SCROLL_PART_VSCROLLBAR_BOTTOM_TROUGH:  input_vscrollbar_bottom_trough(w); break;
		}
		break;
	default:
		if (!widget_is_enabled(w, widgetIndex))
			break;
		if (widget_is_disabled(w, widgetIndex))
			break;

		sound_play_panned(SOUND_CLICK_1, w->x + (widget->left + widget->right) / 2, 0, 0, 0);
		
		// Set new cursor down widget
		RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WINDOWCLASS, rct_windowclass) = windowClass;
		RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WINDOWNUMBER, rct_windownumber) = windowNumber;
		RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WIDGETINDEX, uint16) = widgetIndex;
		RCT2_GLOBAL(RCT2_ADDRESS_INPUT_FLAGS, uint32) |= INPUT_FLAG_WIDGET_PRESSED;
		RCT2_GLOBAL(RCT2_ADDRESS_INPUT_STATE, uint8) = INPUT_STATE_WIDGET_PRESSED;
		RCT2_GLOBAL(0x009DE528, uint16) = 1;

		widget_invalidate_by_number(windowClass, windowNumber, widgetIndex);
		RCT2_CALLPROC_WE_MOUSE_DOWN(w->event_handlers[WE_MOUSE_DOWN], widgetIndex, w, widget);
		break;
	}
}

/* rct2: 0x6E8DA7 */
void input_state_widget_pressed( int x, int y, int state, int widgetIndex, rct_window* w, rct_widget* widget ){
	//RCT2_CALLPROC_X(0x006E8DA7, x, y, state, widgetIndex, (int)w, (int)widget, 0);
	//return;
	RCT2_GLOBAL(0x1420054, uint16) = x;
	RCT2_GLOBAL(0x1420056, uint16) = y;

	rct_windowclass cursor_w_class;
	rct_windownumber cursor_w_number;
	cursor_w_class = RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WINDOWCLASS, rct_windowclass);
	cursor_w_number = RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WINDOWNUMBER, rct_windownumber);
	int cursor_widgetIndex = RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WIDGETINDEX, uint32);

	rct_window* cursor_w = window_find_by_number(cursor_w_class, cursor_w_number);
	if (cursor_w == NULL) {
		RCT2_GLOBAL(RCT2_ADDRESS_INPUT_STATE, uint8) = 0;
		return;
	}

	switch (state){
	case 0:
		if (!w || cursor_w_class != w->classification || cursor_w_number != w->number || widgetIndex != cursor_widgetIndex)
			break;

		if (w->disabled_widgets & (1ULL << widgetIndex))
			break;

		if (RCT2_GLOBAL(0x9DE528, uint16) != 0) RCT2_GLOBAL(0x9DE528, uint16)++;

		if (w->var_020 & (1ULL << widgetIndex) &&
			RCT2_GLOBAL(0x9DE528, uint16) >= 0x10 &&
			(!(RCT2_GLOBAL(0x9DE528, uint16) & 0x3))){
			RCT2_CALLPROC_WE_MOUSE_DOWN(w->event_handlers[WE_MOUSE_DOWN], widgetIndex, w, widget);
		}

		if (RCT2_GLOBAL(0x9DE518, uint32) & 1) return;

		RCT2_GLOBAL(0x9DE518, uint32) |= 1;
		widget_invalidate_by_number(cursor_w_class, cursor_w_number, widgetIndex);
		return;
	case 3:
	case 2:
		if (RCT2_GLOBAL(RCT2_ADDRESS_INPUT_STATE, uint8) == 5){
			if (w) {
				int dropdown_index = 0;

				if (w->classification == WC_DROPDOWN){
					dropdown_index = dropdown_index_from_point(x, y, w);
					if (dropdown_index == -1)goto dropdown_cleanup;

					// _dropdown_unknown?? highlighted?
					if (dropdown_index < 32 && RCT2_GLOBAL(0x009DED34, sint32) & (1 << dropdown_index))goto dropdown_cleanup;

					// gDropdownItemsFormat[dropdown_index] will not work until all windows that use dropdown decompiled
					if (RCT2_ADDRESS(0x9DEBA4, uint16)[dropdown_index] == 0)goto dropdown_cleanup;
				}
				else{
					if (cursor_w_class != w->classification || cursor_w_number != w->number || widgetIndex != cursor_widgetIndex)
						goto dropdown_cleanup;
					dropdown_index = -1;
					if (RCT2_GLOBAL(0x9DE518, uint32) & 2){
						if (!(RCT2_GLOBAL(0x9DE518, uint32) & 4)){
							RCT2_GLOBAL(0x9DE518, uint32) |= (1 << 2);
							return;
						}
					}
				}

				window_close_by_class(WC_DROPDOWN);
				cursor_w = window_find_by_number(cursor_w_class, cursor_w_number);
				if (RCT2_GLOBAL(0x9DE518, uint32) & 1){
					RCT2_GLOBAL(0x9DE518, uint32) &= 0xFFFE;
					widget_invalidate_by_number(cursor_w_class, cursor_w_number, cursor_widgetIndex);
				}

				RCT2_GLOBAL(RCT2_ADDRESS_INPUT_STATE, uint8) = 1;
				RCT2_GLOBAL(RCT2_ADDRESS_TOOLTIP_TIMEOUT, uint16) = 0;
				RCT2_GLOBAL(RCT2_ADDRESS_TOOLTIP_WIDGET_INDEX, uint16) = cursor_widgetIndex;
				RCT2_GLOBAL(RCT2_ADDRESS_TOOLTIP_WINDOW_CLASS, rct_windowclass) = cursor_w_class;
				RCT2_GLOBAL(RCT2_ADDRESS_TOOLTIP_WINDOW_NUMBER, rct_windownumber) = cursor_w_number;
				RCT2_CALLPROC_X(cursor_w->event_handlers[WE_DROPDOWN], dropdown_index, 0, 0, cursor_widgetIndex, (int)cursor_w, 0, 0);
			}
		dropdown_cleanup:
			window_close_by_class(WC_DROPDOWN);
		}
		if (state == 3) return;

		RCT2_GLOBAL(RCT2_ADDRESS_INPUT_STATE, uint8) = 1;
		RCT2_GLOBAL(RCT2_ADDRESS_TOOLTIP_TIMEOUT, uint16) = 0;
		RCT2_GLOBAL(RCT2_ADDRESS_TOOLTIP_WIDGET_INDEX, uint16) = cursor_widgetIndex;

		if (!w)
			break;

		int mid_point_x = (widget->left + widget->right) / 2 + w->x;
		sound_play_panned(5, mid_point_x, 0, 0, 0);
		if (cursor_w_class != w->classification || cursor_w_number != w->number || widgetIndex != cursor_widgetIndex)
			break;
		
		if (w->disabled_widgets & (1ULL << widgetIndex))
			break;

		widget_invalidate_by_number(cursor_w_class, cursor_w_number, widgetIndex);
		window_event_helper(w, widgetIndex, WE_MOUSE_UP);
	default:
		return;
	}

	RCT2_GLOBAL(0x9DE528, uint16) = 0;
	if (RCT2_GLOBAL(RCT2_ADDRESS_INPUT_STATE, uint8) != 5){
		// Hold down widget and drag outside of area??
		if (RCT2_GLOBAL(0x9DE518, uint32) & 1){
			RCT2_GLOBAL(0x9DE518, uint32) &= 0xFFFE;
			widget_invalidate_by_number(cursor_w_class, cursor_w_number, cursor_widgetIndex);
		}
		return;
	}

	if (!w) return;

	if (w->classification == WC_DROPDOWN){
		int dropdown_index = dropdown_index_from_point(x, y, w);

		if (dropdown_index == -1) return;

		// _dropdown_unknown?? highlighted?
		if (dropdown_index < 32 && RCT2_GLOBAL(0x009DED34, sint32) & (1 << dropdown_index))return;

		// gDropdownItemsFormat[dropdown_index] will not work until all windows that use dropdown decompiled
		if (RCT2_ADDRESS(0x9DEBA4, uint16)[dropdown_index] == 0)return;

		// _dropdown_highlighted_index
		RCT2_GLOBAL(0x009DEBA2, sint16) = dropdown_index;
		window_invalidate_by_class(WC_DROPDOWN);
	}
}

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
			window_close_by_class(WC_TOOLTIP);
			
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
				// RCT2_GLOBAL(RCT2_ADDRESS_INPUT_FLAGS, uint32) |= INPUT_FLAG_5;

				GetCursorPos(&_dragPosition);
				ShowCursor(FALSE);

			} else if (widget->type == WWT_SCROLL) {

			}

			break;
		}

		break;
	case INPUT_STATE_WIDGET_PRESSED:
		input_state_widget_pressed(x, y, state, widgetIndex, w, widget);
		//RCT2_CALLPROC_X(0x006E8DA7, x, y, state, widgetIndex, (int)w, (int)widget, 0);
		break;
	case INPUT_STATE_DRAGGING:
		// RCT2_CALLPROC_X(0x006E8C5C, x, y, state, widgetIndex, w, widget, 0);

		w = window_find_by_number(
			RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DRAG_WINDOWCLASS, rct_windowclass),
			RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DRAG_WINDOWNUMBER, rct_windownumber)
		);
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
			RCT2_GLOBAL(RCT2_ADDRESS_TOOLTIP_WIDGET_INDEX, uint16) = RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WIDGETINDEX, sint16);
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

			RCT2_CALLPROC_X(w->event_handlers[WE_UNKNOWN_18], 0, 0, x, y, (int)w, 0, 0);
		}
		break;
	case INPUT_STATE_VIEWPORT_DRAG:
	{
		int dx, dy;

		POINT newDragPosition;
		GetCursorPos(&newDragPosition);
		
		dx = newDragPosition.x - _dragPosition.x;
		dy = newDragPosition.y - _dragPosition.y;
		w = window_find_by_number(
			RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DRAG_WINDOWCLASS, rct_windowclass),
			RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DRAG_WINDOWNUMBER, rct_windownumber)
		);
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
					w->saved_view_x += dx;
					w->saved_view_y += dy;
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
							RCT2_CALLPROC_X(0x006B4857, eax, 0, ecx, edx, 0, 0, 0);
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
		input_state_widget_pressed(x, y, state, widgetIndex, w, widget);
		//RCT2_CALLPROC_X(0x006E8DA7, x, y, state, widgetIndex, (int)w, (int)widget, 0);
		break;
	case INPUT_STATE_VIEWPORT_LEFT:
		//RCT2_CALLPROC_X(0x006E87B4, x, y, state, widgetIndex, (int)w, (int)widget, 0);
		w = window_find_by_number(
			RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DRAG_WINDOWCLASS, rct_windowclass),
			RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DRAG_WINDOWNUMBER, rct_windownumber)
		);
		if (!w){
			RCT2_GLOBAL(RCT2_ADDRESS_INPUT_STATE, uint8) = 0;
			break;
		}

		if (state == 0){
			if (!w->viewport){
				RCT2_GLOBAL(RCT2_ADDRESS_INPUT_STATE, uint8) = 0;
				break;
			}

			if (w->classification != RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DRAG_WINDOWCLASS, rct_windowclass) ||
				w->number != RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DRAG_WINDOWNUMBER, rct_windownumber) ||
				!(RCT2_GLOBAL(0x9DE518, uint32)&(1 << 3)))break;
			
			w = window_find_by_number(
				RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WINDOWCLASS, rct_windowclass),
				RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WINDOWNUMBER, rct_windownumber)
			);
			if (w == NULL)
				break;

			RCT2_CALLPROC_X(w->event_handlers[WE_TOOL_DRAG], x, y, 0, (int)RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WIDGETINDEX, uint16), (int)w, 0, 0);
		}
		else if (state == 2){
			
			RCT2_GLOBAL(RCT2_ADDRESS_INPUT_STATE, uint8) = 0;
			if (RCT2_GLOBAL(0x9DE52E, rct_windownumber) != w->number)break;
			if ((RCT2_GLOBAL(0x9DE518, uint32)&(1 << 3))){
				w = window_find_by_number(
					RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WINDOWCLASS, rct_windowclass),
					RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WINDOWNUMBER, rct_windownumber)
				);
				if (w == NULL)
					break;
				RCT2_CALLPROC_X(w->event_handlers[WE_TOOL_UP], x, y, 0, (int)RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WIDGETINDEX, uint16), (int)w, 0, 0);
			}
			else{
				if ((RCT2_GLOBAL(0x9DE518, uint32)&(1 << 4)))break;
				rct_sprite* spr;
				int eax = x, ebx = y, ecx = state, esi = (int)w, edi = (int)widget, ebp = 0;
				RCT2_CALLFUNC_X(0X6ED9D0, &eax, &ebx, &ecx, (int*)&spr, &esi, &edi, &ebp);
				if ((ebx & 0xFF) == 2){
					
					if (spr->unknown.sprite_identifier == SPRITE_IDENTIFIER_VEHICLE){
						//Open ride window
						RCT2_CALLPROC_X(0x6ACAC2, eax, ebx, ecx, (int)spr, esi, edi, ebp);
					}
					else if (spr->unknown.sprite_identifier == SPRITE_IDENTIFIER_PEEP){
						window_guest_open(&spr->peep);
					}
					else if (spr->unknown.sprite_identifier == SPRITE_IDENTIFIER_FLOATING_TEXT){
						//Unknown for now
						RCT2_CALLPROC_X(0x6E88D7, eax, ebx, ecx, (int)spr, esi, edi, ebp);
					}
				}
				else if ((ebx & 0xFF) == 3){
					rct_map_element* map_element = (rct_map_element*)spr;
		
					if (!((map_element->type & MAP_ELEMENT_TYPE_MASK) == MAP_ELEMENT_TYPE_ENTRANCE)){
						eax = RCT2_ADDRESS(0x0099BA64, uint8)[16 * map_element->properties.track.type];
						if (!(eax & 0x10)){//If not station track
							//Open ride window in overview mode.
							window_ride_main_open(map_element->properties.track.ride_index);
							break;
						}
					}
					//Open ride window in station view
					RCT2_CALLPROC_X(0x6ACCCE, map_element->properties.track.ride_index, (map_element->properties.track.sequence & 0x70) >> 4, ecx, (int)map_element, esi, edi, ebp);
				}
				else if ((ebx & 0xFF) == 8){
					window_park_entrance_open();
				}
			}
		}
		break;
	case INPUT_STATE_SCROLL_LEFT://0x006E8676
		//RCT2_CALLPROC_X(0x006E8676, x, y, state, widgetIndex, (int)w, (int)widget, 0);
		if (state == 0){
			if (widgetIndex != RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WIDGETINDEX, uint32)){
				invalidate_scroll();
				return;
			}
			if (w->classification != RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WINDOWCLASS, uint8)){
				invalidate_scroll();
				return;
			}
			if (w->number != RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WINDOWNUMBER, uint16)){
				invalidate_scroll();
				return;
			}

			if (RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_SCROLL_AREA, uint16) == SCROLL_PART_HSCROLLBAR_THUMB){
				int temp_x = x;
				x -= RCT2_GLOBAL(RCT2_ADDRESS_TOOLTIP_CURSOR_X, uint16);
				RCT2_GLOBAL(RCT2_ADDRESS_TOOLTIP_CURSOR_X, uint16) = temp_x;
				RCT2_CALLPROC_X(0x006E98F2, x, temp_x, state, w->number, (int)w, (int)widget, x);
				return;
			}

			if (RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_SCROLL_AREA, uint16) == SCROLL_PART_VSCROLLBAR_THUMB){
				int temp_y = y;	
				y -= RCT2_GLOBAL(RCT2_ADDRESS_TOOLTIP_CURSOR_Y, uint16);
				RCT2_GLOBAL(RCT2_ADDRESS_TOOLTIP_CURSOR_Y, uint16) = temp_y;
				RCT2_CALLPROC_X(0x006E99A9, temp_y, y, state, w->number, (int)w, (int)widget, y);
				return;
			}
			int scroll_part, scroll_id;
			widget_scroll_get_part(w, widget, x, y, &x, &y, &scroll_part, &scroll_id);

			if (scroll_part != RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_SCROLL_AREA, uint16)){
				invalidate_scroll();
				return;
			}

			switch (scroll_part){
			case SCROLL_PART_VIEW:
				RCT2_CALLPROC_X(w->event_handlers[WE_TOOL_DRAG], w->number / 18, y, x, y, (int)w, (int)widget, (int)w->event_handlers);
				break;
			case SCROLL_PART_HSCROLLBAR_LEFT:
				RCT2_CALLPROC_X(0x006E9A60, x, y, scroll_part, w->number, (int)w, (int)widget, 0);
				break;
			case SCROLL_PART_HSCROLLBAR_RIGHT:
				RCT2_CALLPROC_X(0x006E9ABF, x, y, scroll_part, w->number, (int)w, (int)widget, 0);
				break;
			case SCROLL_PART_HSCROLLBAR_LEFT_TROUGH:
			case SCROLL_PART_HSCROLLBAR_RIGHT_TROUGH:
				return;
				break;
			case SCROLL_PART_HSCROLLBAR_THUMB:
			case SCROLL_PART_VSCROLLBAR_TOP:
				RCT2_CALLPROC_X(0x006E9C37, x, y, scroll_part, w->number, (int)w, (int)widget, 0);
				break;
			case SCROLL_PART_VSCROLLBAR_BOTTOM:
				RCT2_CALLPROC_X(0x006E9C96, x, y, scroll_part, w->number, (int)w, (int)widget, 0);
				break;
			case SCROLL_PART_VSCROLLBAR_TOP_TROUGH:
			case SCROLL_PART_VSCROLLBAR_BOTTOM_TROUGH:
				return;
				break;
			default:
				return;
			}
		}else if (state==2){
			RCT2_GLOBAL(RCT2_ADDRESS_INPUT_STATE, uint8) = INPUT_STATE_RESET;
			invalidate_scroll();
			return;
		}
		break;
	case INPUT_STATE_RESIZING:
		// RCT2_CALLPROC_X(0x006E8B46, x, y, state, widgetIndex, w, widget, 0);

		w = window_find_by_number(
			RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DRAG_WINDOWCLASS, rct_windowclass),
			RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DRAG_WINDOWNUMBER, rct_windownumber)
		);
		if (w == NULL) {
			RCT2_GLOBAL(RCT2_ADDRESS_INPUT_STATE, uint8) = INPUT_STATE_RESET;
			break;
		}

		if (state != 0 && state != 2)
			break;
		if (state == 2) {
			RCT2_GLOBAL(RCT2_ADDRESS_INPUT_STATE, uint8) = INPUT_STATE_NORMAL;
			RCT2_GLOBAL(RCT2_ADDRESS_TOOLTIP_TIMEOUT, uint8) = 0;
			RCT2_GLOBAL(RCT2_ADDRESS_TOOLTIP_WIDGET_INDEX, uint16) = RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WIDGETINDEX, sint16);
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
		RCT2_CALLPROC_X(0x006E8ACB, x, y, state, widgetIndex, (int)w, (int)widget, 0);
		break;
	}
}

/**
 * 
 *  rct2: 0x006E9253
 */
static void input_mouseover(int x, int y, rct_window *w, int widgetIndex)
{
	rct_windowclass windowClass = 255;
	rct_windownumber windowNumber = 0;
	rct_widget *widget = NULL;

	if (w != NULL) {
		windowClass = w->classification;
		windowNumber = w->number;
		widget = &w->widgets[widgetIndex];
	}

	input_mouseover_widget_check(windowClass, windowNumber, widgetIndex);

	if (w != NULL && widgetIndex != -1 && widget->type == WWT_SCROLL)
	{
		int eax, ebx, ecx, edx;
		widget_scroll_get_part(w, widget, x, y, &eax, &ebx, &ecx, &edx);
		
		if (ecx < 0)
			goto showTooltip;
		if (ecx == 0) {
			RCT2_CALLPROC_X(w->event_handlers[WE_SCROLL_MOUSEOVER], edx, 0, eax, ebx, (int)w, 0, 0);
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
			window_close_by_class(WC_TOOLTIP);
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
	if (widgetIndex == -1) return; //Prevents invalid widgets being clicked source of bug is elsewhere
	// Check if widget cursor was over has changed
	if (windowClass != RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_OVER_WINDOWCLASS, rct_windowclass) ||
		windowNumber != RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_OVER_WINDOWNUMBER, rct_windownumber) ||
		widgetIndex != RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_OVER_WIDGETINDEX, rct_windownumber)
	) {
		// Invalidate last widget cursor was on if widget is a flat button
		input_mouseover_widget_flatbutton_invalidate();

		// Set new cursor over widget
		RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_OVER_WINDOWCLASS, rct_windowclass) = windowClass;
		RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_OVER_WINDOWNUMBER, rct_windownumber) = windowNumber;
		RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_OVER_WIDGETINDEX, uint16) = widgetIndex;

		// Invalidate new widget cursor is on if widget is a flat button
		if (windowClass != 255)
			input_mouseover_widget_flatbutton_invalidate();
	}
}

static void input_mouseover_widget_flatbutton_invalidate()
{
	rct_window *w = window_find_by_number(
		RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_OVER_WINDOWCLASS, rct_windowclass),
		RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_OVER_WINDOWNUMBER, rct_windownumber)
	);
	if (w == NULL)
		return;

	RCT2_CALLPROC_X(w->event_handlers[WE_INVALIDATE], 0, 0, 0, 0, (int)w, 0, 0);
	if (w->widgets[RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_OVER_WIDGETINDEX, rct_windownumber)].type == WWT_FLATBTN) {
		widget_invalidate_by_number(
			RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_OVER_WINDOWCLASS, rct_windowclass),
			RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_OVER_WINDOWNUMBER, rct_windownumber),
			RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_OVER_WIDGETINDEX, rct_windownumber)
		);
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
		mainWindow->saved_view_x += scrollX * (12 << mainWindow->viewport->zoom);
		RCT2_GLOBAL(RCT2_ADDRESS_INPUT_FLAGS, uint32) |= INPUT_FLAG_VIEWPORT_SCROLLING;
	}
	if (scrollY != 0) {
		mainWindow->saved_view_y += scrollY * (12 << mainWindow->viewport->zoom);
		RCT2_GLOBAL(RCT2_ADDRESS_INPUT_FLAGS, uint32) |= INPUT_FLAG_VIEWPORT_SCROLLING;
	}
}

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

	rct_window *textWindow;

	textWindow = window_find_by_id(113, 0);
	if (textWindow) return;

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
		mainWindow->saved_view_x += scrollX * (12 << mainWindow->viewport->zoom);
		RCT2_GLOBAL(RCT2_ADDRESS_INPUT_FLAGS, uint32) |= INPUT_FLAG_VIEWPORT_SCROLLING;
	}
	if (scrollY != 0) {
		mainWindow->saved_view_y += scrollY * (12 << mainWindow->viewport->zoom);
		RCT2_GLOBAL(RCT2_ADDRESS_INPUT_FLAGS, uint32) |= INPUT_FLAG_VIEWPORT_SCROLLING;
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

void handle_shortcut_command(int shortcutIndex)
{
	rct_window *window;

	switch (shortcutIndex) {
	case SHORTCUT_CLOSE_TOP_MOST_WINDOW:
		window_close_top();
		break;
	case SHORTCUT_CLOSE_ALL_FLOATING_WINDOWS:
		if (!(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & 2))
			window_close_all();
		else if (RCT2_GLOBAL(0x0141F570, uint8) == 1)
			window_close_top();
		break;
	case SHORTCUT_CANCEL_CONSTRUCTION_MODE:
		window = window_find_by_class(WC_ERROR);
		if (window != NULL)
			window_close(window);
		else if (RCT2_GLOBAL(RCT2_ADDRESS_INPUT_FLAGS, uint32) & INPUT_FLAG_TOOL_ACTIVE)
			tool_cancel();
		break;
	case SHORTCUT_PAUSE_GAME:
		if (!(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & 10)) {
			window = window_find_by_class(WC_TOP_TOOLBAR);
			if (window != NULL) {
				window_invalidate(window);
				window_event_helper(window, 0, WE_MOUSE_UP);
			}
		}
		break;
	case SHORTCUT_ZOOM_VIEW_OUT:
		if (!(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & 2) || RCT2_GLOBAL(0x0141F570, uint8) == 1) {
			if (!(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & 8)) {
				window = window_find_by_class(WC_TOP_TOOLBAR);
				if (window != NULL) {
					window_invalidate(window);
					window_event_helper(window, 2, WE_MOUSE_UP);
				}
			}
		}
		break;
	case SHORTCUT_ZOOM_VIEW_IN:
		if (!(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & 2) || RCT2_GLOBAL(0x0141F570, uint8) == 1) {
			if (!(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & 8)) {
				window = window_find_by_class(WC_TOP_TOOLBAR);
				if (window != NULL) {
					window_invalidate(window);
					window_event_helper(window, 3, WE_MOUSE_UP);
				}
			}
		}
		break;
	case SHORTCUT_ROTATE_VIEW:
		if (!(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & 2) || RCT2_GLOBAL(0x0141F570, uint8) == 1) {
			if (!(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & 8)) {
				window = window_find_by_class(WC_TOP_TOOLBAR);
				if (window != NULL) {
					window_invalidate(window);
					window_event_helper(window, 4, WE_MOUSE_UP);
				}
			}
		}
		break;
	case SHORTCUT_ROTATE_CONSTRUCTION_OBJECT:
		RCT2_CALLPROC_EBPSAFE(0x006E4182);
		break;
	case SHORTCUT_UNDERGROUND_VIEW_TOGGLE:
		window = window_get_main();
		if (window != NULL) {
			window->viewport->flags ^= VIEWPORT_FLAG_UNDERGROUND_INSIDE;
			window_invalidate(window);
		}
		break;
	case SHORTCUT_REMOVE_BASE_LAND_TOGGLE:
		window = window_get_main();
		if (window != NULL) {
			window->viewport->flags ^= VIEWPORT_FLAG_HIDE_BASE;
			window_invalidate(window);
		}
		break;
	case SHORTCUT_REMOVE_VERTICAL_LAND_TOGGLE:
		window = window_get_main();
		if (window != NULL) {
			window->viewport->flags ^= VIEWPORT_FLAG_HIDE_VERTICAL;
			window_invalidate(window);
		}
		break;
	case SHORTCUT_SEE_THROUGH_RIDES_TOGGLE:
		window = window_get_main();
		if (window != NULL) {
			window->viewport->flags ^= VIEWPORT_FLAG_SEETHROUGH_RIDES;
			window_invalidate(window);
		}
		break;
	case SHORTCUT_SEE_THROUGH_SCENERY_TOGGLE:
		window = window_get_main();
		if (window != NULL) {
			window->viewport->flags ^= VIEWPORT_FLAG_SEETHROUGH_SCENERY;
			window_invalidate(window);
		}
		break;
	case SHORTCUT_INVISIBLE_SUPPORTS_TOGGLE:
		window = window_get_main();
		if (window != NULL) {
			window->viewport->flags ^= VIEWPORT_FLAG_INVISIBLE_SUPPORTS;
			window_invalidate(window);
		}
		break;
	case SHORTCUT_INVISIBLE_PEOPLE_TOGGLE:
		window = window_get_main();
		if (window != NULL) {
			window->viewport->flags ^= VIEWPORT_FLAG_INVISIBLE_PEEPS;
			window_invalidate(window);
		}
		break;
	case SHORTCUT_HEIGHT_MARKS_ON_LAND_TOGGLE:
		window = window_get_main();
		if (window != NULL) {
			window->viewport->flags ^= VIEWPORT_FLAG_LAND_HEIGHTS;
			window_invalidate(window);
		}
		break;
	case SHORTCUT_HEIGHT_MARKS_ON_RIDE_TRACKS_TOGGLE:
		window = window_get_main();
		if (window != NULL) {
			window->viewport->flags ^= VIEWPORT_FLAG_TRACK_HEIGHTS;
			window_invalidate(window);
		}
		break;
	case SHORTCUT_HEIGHT_MARKS_ON_PATHS_TOGGLE:
		window = window_get_main();
		if (window != NULL) {
			window->viewport->flags ^= VIEWPORT_FLAG_PATH_HEIGHTS;
			window_invalidate(window);
		}
		break;
	case SHORTCUT_ADJUST_LAND:
		if (!(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & 2) || RCT2_GLOBAL(0x0141F570, uint8) == 1) {
			if (!(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & 0x0C)) {
				window = window_find_by_class(WC_TOP_TOOLBAR);
				if (window != NULL) {
					window_invalidate(window);
					window_event_helper(window, 7, WE_MOUSE_UP);
				}
			}
		}
		break;
	case SHORTCUT_ADJUST_WATER:
		if (!(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & 2) || RCT2_GLOBAL(0x0141F570, uint8) == 1) {
			if (!(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & 0x0C)) {
				window = window_find_by_class(WC_TOP_TOOLBAR);
				if (window != NULL) {
					window_invalidate(window);
					window_event_helper(window, 8, WE_MOUSE_UP);
				}
			}
		}
		break;
	case SHORTCUT_BUILD_SCENERY:
		if (!(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & 2) || RCT2_GLOBAL(0x0141F570, uint8) == 1) {
			if (!(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & 0x0C)) {
				window = window_find_by_class(WC_TOP_TOOLBAR);
				if (window != NULL) {
					window_invalidate(window);
					window_event_helper(window, 9, WE_MOUSE_UP);
				}
			}
		}
		break;
	case SHORTCUT_BUILD_PATHS:
		if (!(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & 2) || RCT2_GLOBAL(0x0141F570, uint8) == 1) {
			if (!(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & 0x0C)) {
				window = window_find_by_class(WC_TOP_TOOLBAR);
				if (window != NULL) {
					window_invalidate(window);
					window_event_helper(window, 10, WE_MOUSE_UP);
				}
			}
		}
		break;
	case SHORTCUT_BUILD_NEW_RIDE:
		if (!(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & 2) || RCT2_GLOBAL(0x0141F570, uint8) == 1) {
			if (!(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & 0x0C)) {
				window = window_find_by_class(WC_TOP_TOOLBAR);
				if (window != NULL) {
					window_invalidate(window);
					window_event_helper(window, 11, WE_MOUSE_UP);
				}
			}
		}
		break;
	case SHORTCUT_SHOW_FINANCIAL_INFORMATION:
		if (!(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & 0x0C))
			if (!(RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & PARK_FLAGS_NO_MONEY))
				window_finances_open();
		break;
	case SHORTCUT_SHOW_RESEARCH_INFORMATION:
		if (!(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & 0x0E)) {
			// Open new ride window
			RCT2_CALLPROC_EBPSAFE(0x006B3CFF);
			window = window_find_by_class(WC_CONSTRUCT_RIDE);
			if (window != NULL)
				RCT2_CALLPROC_WE_MOUSE_DOWN(window->event_handlers[WE_MOUSE_DOWN], 10, window, NULL);
		}
		break;
	case SHORTCUT_SHOW_RIDES_LIST:
		if (!(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & 0x0E)) {
			window = window_find_by_class(WC_TOP_TOOLBAR);
			if (window != NULL) {
				window_invalidate(window);
				window_event_helper(window, 12, WE_MOUSE_UP);
			}
		}
		break;
	case SHORTCUT_SHOW_PARK_INFORMATION:
		if (!(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & 0x0E)) {
			window = window_find_by_class(WC_TOP_TOOLBAR);
			if (window != NULL) {
				window_invalidate(window);
				window_event_helper(window, 13, WE_MOUSE_UP);
			}
		}
		break;
	case SHORTCUT_SHOW_GUEST_LIST:
		if (!(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & 0x0E)) {
			window = window_find_by_class(WC_TOP_TOOLBAR);
			if (window != NULL) {
				window_invalidate(window);
				window_event_helper(window, 15, WE_MOUSE_UP);
			}
		}
		break;
	case SHORTCUT_SHOW_STAFF_LIST:
		if (!(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & 0x0E)) {
		  window = window_find_by_class(WC_TOP_TOOLBAR);
			if (window != NULL) {
				window_invalidate(window);
				window_event_helper(window, 14, WE_MOUSE_UP);
			}
		}
		break;
	case SHORTCUT_SHOW_RECENT_MESSAGES:
		if (!(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & 0x0E))
			window_news_open();
		break;
	case SHORTCUT_SHOW_MAP:
		if (!(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & 2) || RCT2_GLOBAL(0x0141F570, uint8) == 1) {
			if (!(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & 0x0C)) {
				window = window_find_by_class(WC_TOP_TOOLBAR);
				if (window != NULL) {
					window_invalidate(window);
					window_event_helper(window, 6, WE_MOUSE_UP);
				}
			}
		}
		break;
	case SHORTCUT_SCREENSHOT:
		RCT2_CALLPROC_EBPSAFE(0x006E4034); // set screenshot countdown to 2
		break;

	// New
	case SHORTCUT_REDUCE_GAME_SPEED:
		game_reduce_game_speed();
		break;
	case SHORTCUT_INCREASE_GAME_SPEED:
		game_increase_game_speed();
		break;
	}
}

void game_increase_game_speed()
{
	gGameSpeed = min(8, gGameSpeed + 1);
}

void game_reduce_game_speed()
{
	gGameSpeed = max(1, gGameSpeed - 1);
}

/**
 * 
 *  rct2: 0x006E3E68
 */
void handle_shortcut(int key)
{
	int i;
	for (i = 0; i < SHORTCUT_COUNT; i++) {
		if (key == gShortcutKeys[i]) {
			handle_shortcut_command(i);
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
	window_close_by_class(WC_CHANGE_KEYBOARD_SHORTCUT);
	window_invalidate_by_class(WC_KEYBOARD_SHORTCUT_LIST);
	config_save();
}

/**
 * 
 *  rct2: 0x006E3B43
 */
void game_handle_keyboard_input()
{
	rct_window *w;
	int key;

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
		if (key == 255)
			continue;

		key |= RCT2_GLOBAL(RCT2_ADDRESS_PLACE_OBJECT_MODIFIER, uint8) << 8;

		w = window_find_by_class(WC_CHANGE_KEYBOARD_SHORTCUT);
		if (w != NULL)
			set_shortcut(key);
		else if (RCT2_GLOBAL(RCT2_ADDRESS_ON_TUTORIAL, uint8) == 1)
			tutorial_stop();
		else{
			w = window_find_by_id(113, 0);
			if (w != NULL){
				void(*fp)(int, rct_window*) = (void*)(int)w->event_handlers[WE_TEXT_INPUT];
				(*fp)(key,w);
			}
			else{
				handle_shortcut(key);
			}
		}
			
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
				RCT2_CALLPROC_X(0x006EA2AA, 0, 0, 0, 0, (int)w, RCT2_GLOBAL(0x009DEA72, uint16), 0);
				RCT2_GLOBAL(0x009DEA72, uint16)++;
			}
		}
	} else {
		if (!(RCT2_GLOBAL(RCT2_ADDRESS_PLACE_OBJECT_MODIFIER, uint8) & 4)) {
			window_tooltip_close();
			if ((w = window_get_main()) != NULL) {
				RCT2_CALLPROC_X(0x006EA2AA, 0, 0, 0, 0, (int)w, RCT2_GLOBAL(0x009DEA72, uint16), 0);
				RCT2_GLOBAL(0x009DEA72, uint16)++;
			}
		}

		// Write tutorial input
		RCT2_CALLPROC_X(0x0066EEE1, RCT2_GLOBAL(RCT2_ADDRESS_PLACE_OBJECT_MODIFIER, uint8), 0, 0, 0, 0, 0, 0);
	}
}

/**
 * 
 *  rct2: 0x006EA627
 */
void game_handle_input()
{
	rct_window *w;
	int x, y, state;

	if (RCT2_GLOBAL(0x009DEA64, uint16) & 2) {
		RCT2_GLOBAL(0x009DEA64, uint16) &= ~2;
		game_do_command(0, 1, 0, 0, GAME_COMMAND_LOAD_OR_QUIT, 2, 0); 
	}

	if (RCT2_GLOBAL(0x009ABDF2, uint8) != 0) {
		for (w = g_window_list; w < RCT2_GLOBAL(RCT2_ADDRESS_NEW_WINDOW_PTR, rct_window*); w++)
			RCT2_CALLPROC_X(w->event_handlers[WE_UNKNOWN_07], 0, 0, 0, 0, (int)w, 0, 0);

		sub_6EA73F();
		RCT2_CALLPROC_EBPSAFE(0x006E8346); // update_cursor_position

		for (;;) {
			game_get_next_input(&x, &y, &state);
			if (state == 0)
				break;

			game_handle_input_mouse(x, y, state & 0xFF);
		}

		if (RCT2_GLOBAL(RCT2_ADDRESS_INPUT_FLAGS, uint32) & INPUT_FLAG_5) {
			game_handle_input_mouse(x, y, state);
		} else if (x != 0x80000000) {
			x = clamp(0, x, RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_WIDTH, sint16) - 1);
			y = clamp(0, y, RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_HEIGHT, sint16) - 1);

			game_handle_input_mouse(x, y, state);
			process_mouse_over(x, y);
			process_mouse_tool(x, y);
		}
	}

	for (w = g_window_list; w < RCT2_GLOBAL(RCT2_ADDRESS_NEW_WINDOW_PTR, rct_window*); w++)
		RCT2_CALLPROC_X(w->event_handlers[WE_UNKNOWN_08], 0, 0, 0, 0,(int) w, 0, 0);
}

/**
 * 
 *  rct2: 0x006E83C7
 */
static void game_get_next_input(int *x, int *y, int *state)
{
	rct_mouse_data* eax = get_mouse_input();
	if (eax == NULL) {
		*x = gCursorState.x;
		*y = gCursorState.y;
		*state = 0;
		return;
	}

	*x = eax->x;
	*y = eax->y;
	*state = eax->state;

	//int eax, ebx, ecx, edx, esi, edi, ebp;
	//RCT2_CALLFUNC_X(0x006E83C7, &eax, &ebx, &ecx, &edx, &esi, &edi, &ebp);
	//*x = eax & 0xFFFF;
	//*y = ebx & 0xFFFF;
	//*state = ecx & 0xFF;
	//return;

	//int on_tutorial = RCT2_GLOBAL(RCT2_ADDRESS_ON_TUTORIAL, uint8);
	//if (RCT2_GLOBAL(RCT2_ADDRESS_INPUT_FLAGS, uint32) & INPUT_FLAG_5) {
	//	if (on_tutorial == 1) {

	//	} else {
	//		RCT2_CALLPROC_EBPSAFE(0x00407074);
	//	}
	//	if (on_tutorial == 2) {

	//	}

	//} else {

	//}
}

/**
 *
 *  rct2: 0x006ED801
 */
void process_mouse_tool(int x, int y)
{
	if (RCT2_GLOBAL(0x9DE518, uint32) & (1 << 3))
	{
		rct_window* w = window_find_by_number(
			RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WINDOWCLASS, uint8),
			RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WINDOWNUMBER, uint16)
		);

		if (!w)
			tool_cancel();
		else
			RCT2_CALLPROC_X(w->event_handlers[WE_TOOL_UPDATE], x, y, 0, RCT2_GLOBAL(0x9DE546, uint16), (int)w, 0, 0);

	}
}

/**
*
*  rct2: 0x006ED990
*/
void sub_6ED990(char cursor_id){
	if (RCT2_GLOBAL(RCT2_ADDRESS_INPUT_STATE, uint8) == INPUT_STATE_RESIZING)
	{
		cursor_id = CURSOR_DIAGONAL_ARROWS;	//resize icon
	}

	if (cursor_id == RCT2_GLOBAL(RCT2_ADDRESS_CURENT_CURSOR, uint8))
	{
		return;
	}
	RCT2_GLOBAL(RCT2_ADDRESS_CURENT_CURSOR, uint8) = cursor_id;
	RCT2_GLOBAL(0x14241BC, uint32) = 2;
	osinterface_set_cursor(cursor_id);
	RCT2_GLOBAL(0x14241BC, uint32) = 0;
}

/**
*
*  rct2: 0x006ED833
*/
void process_mouse_over(int x, int y)
{
	rct_window* window;
	rct_window* subWindow;

	int widgetId;
	int cursorId;
	int eax, ebx, ecx, edx, esi, edi, ebp;

	cursorId = CURSOR_ARROW;
	RCT2_GLOBAL(0x9A9808, sint16) = -1;
	window = window_find_from_point(x, y);

	if (window != NULL)
	{
		widgetId = window_find_widget_from_point(window, x, y);
		RCT2_GLOBAL(0x1420046, sint16) = (widgetId & 0xFFFF);
		if (widgetId != -1)
		{

			switch (window->widgets[widgetId].type){
			case WWT_VIEWPORT:
				if ((RCT2_GLOBAL(0x9DE518, int) & 0x8) == 0)
				{
					edx = cursorId;
					eax = x;
					ebx = y;
					//Find out if there is a clickable item under pointer
					RCT2_CALLFUNC_X(0X6ED9D0, &eax, &ebx, &ecx, &edx, &esi, &edi, &ebp);

					if ((ebx & 0xFF) == 2 || (ebx & 0xFF) == 8 || (ebx & 0xFF) == 3)
					{
						sub_6ED990(CURSOR_HAND_POINT);
						return;
					}
					break;
				}
				cursorId = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_TOOL, uint8);
				subWindow = window_find_by_number(
					RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WINDOWCLASS, rct_windowclass),
					RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WINDOWNUMBER, rct_windownumber)
				);
				ebp = (int)subWindow;
				if (subWindow == NULL)
				{
					break;
				}
				ebx = ebx & 0xFFFFFF00;
				edi = cursorId;
				esi = (int) subWindow;
				RCT2_CALLFUNC_X(subWindow->event_handlers[WE_UNKNOWN_0E], &eax, &ebx, &ecx, &edx, &esi, &edi, &ebp);
				cursorId = edi;
				if ((ebx & 0xFF) != 0)
				{
					sub_6ED990(cursorId);
					return;
				}
				break;
			case WWT_FRAME:
			case WWT_RESIZE:
				if (!(window->flags & 0x100))
				{
					break;
				}
				if (window->min_width == window->max_width && window->min_height == window->max_height)
				{
					break;
				}
				if (x < window->x + window->width - 0x13)
				{
					break;
				}
				if (y < window->y + window->height - 0x13)
				{
					break;
				}
				cursorId = CURSOR_DIAGONAL_ARROWS;
				break;
			case WWT_SCROLL:
				RCT2_GLOBAL(0x9DE558, uint16) = x;
				RCT2_GLOBAL(0x9DE55A, uint16) = y;
				int output_x, output_y, output_scroll_area, scroll_id;
				widget_scroll_get_part(window, window->widgets, x, y, &output_x, &output_y, &output_scroll_area, &scroll_id);
				cursorId = scroll_id;
				if (output_scroll_area != SCROLL_PART_VIEW)
				{
					cursorId = CURSOR_ARROW;
					break;
				}
				//Fall through to default
			default:
				ecx = x;
				edx = y;
				eax = widgetId;
				ebx = 0xFFFFFFFF;
				edi = (int)&window->widgets[widgetId];

				RCT2_CALLFUNC_X(window->event_handlers[WE_UNKNOWN_17], &eax, &ebx, &ecx, &edx, &esi, &edi, &ebp);
				if (ebx == 0xFFFFFFFF)
				{
					cursorId = CURSOR_ARROW;
					break;
				}
				cursorId = ebx;
				break;
			}
		}
	}
	RCT2_CALLPROC_X(0x6EDE88, x, y, ecx, cursorId, esi, edi, ebp);
	//itemUnderCursor(&eax, &ebx, &ecx, &edx);

	sub_6ED990(cursorId);
	return;
}

/**
 * 
 *  rct2: 0x006E876D
 */
void invalidate_scroll()
{
	rct_window* wind = window_find_by_number(
		RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WINDOWCLASS, uint8),
		RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WINDOWNUMBER, uint16)
	);
	if (wind == NULL) return;

	int scroll_id = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_SCROLL_ID, uint32);
	//Reset to basic scroll
	wind->scrolls[scroll_id / sizeof(rct_scroll)].flags &= 0xFF11;

	window_invalidate_by_number(
		RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WINDOWCLASS, uint8),
		RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WINDOWNUMBER, uint16)
	);
}

/**
* rct2: 0x00406C96
*/
void store_mouse_input(int state)
{
	uint32 write_index = RCT2_GLOBAL(RCT2_ADDRESS_MOUSE_WRITE_INDEX, uint32);
	uint32 next_write_index = (write_index + 1) % 64;

	// check if the queue is full
	if (next_write_index == RCT2_GLOBAL(RCT2_ADDRESS_MOUSE_READ_INDEX, uint32))
		return;

	rct_mouse_data* item = &mouse_buffer[write_index];
	item->x = RCT2_GLOBAL(0x01424318, uint32);
	item->y = RCT2_GLOBAL(0x0142431C, uint32);
	item->state = state;

	RCT2_GLOBAL(RCT2_ADDRESS_MOUSE_WRITE_INDEX, uint32) = next_write_index;
}


/**
 * rct2: 0x00407074
 */
static rct_mouse_data* get_mouse_input()
{
	uint32 read_index = RCT2_GLOBAL(RCT2_ADDRESS_MOUSE_READ_INDEX, uint32);

	// check if that location has been written to yet
	if (read_index == RCT2_GLOBAL(RCT2_ADDRESS_MOUSE_WRITE_INDEX, uint32))
		return NULL;

	RCT2_GLOBAL(RCT2_ADDRESS_MOUSE_READ_INDEX, uint32) = (read_index + 1) % 64;
	return &mouse_buffer[read_index];
}
