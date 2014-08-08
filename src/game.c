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
 
#include "addresses.h"
#include "audio.h"
#include "climate.h"
#include "config.h"
#include "rct2.h"
#include "game.h"
#include "finance.h"
#include "news_item.h"
#include "object.h"
#include "osinterface.h"
#include "peep.h"
#include "sawyercoding.h"
#include "scenario.h"
#include "screenshot.h"
#include "sprite.h"
#include "string_ids.h"
#include "title.h"
#include "tutorial.h"
#include "vehicle.h"
#include "viewport.h"
#include "widget.h"
#include "window.h"
#include "window_error.h"
#include "window_tooltip.h"


int _gameSpeed = 1;

void game_handle_input();
void game_handle_keyboard_input();

void process_mouse_over(int x, int y);

void sub_0x684027(int left, int top, int width, int height, int ebp, uint32 edi, uint32 esi){
	uint8* pattern = RCT2_ADDRESS(0x9AC010, uint8*)[ebp];
	uint8 pattern_x_space = *pattern++;
	uint8 pattern_y_space = *pattern++;

	uint8 pattern_start_x_offset = edi % pattern_x_space;
	uint8 pattern_start_y_offset = esi % pattern_y_space;;

	rct_drawpixelinfo* dpi = RCT2_ADDRESS(RCT2_ADDRESS_SCREEN_DPI, rct_drawpixelinfo);
	uint32 pixel_offset = (dpi->pitch + dpi->width)*top + left;
	uint8 pattern_y_pos = pattern_start_y_offset;
	
	//Stores the colours of changed pixels
	uint32* pixel_store = RCT2_ADDRESS(0xEDF850, uint32);
	pixel_store += RCT2_GLOBAL(0x9AC00C, uint32);

	for (; height != 0; height--){
		uint8 pattern_x = pattern[pattern_y_pos * 2];
		if (pattern_x != 0xFF){
			if (RCT2_GLOBAL(0x9AC00C, uint32) <= 0x1F38){
				

				int final_pixel_offset = width + pixel_offset;

				int x_pixel_offset = pixel_offset;
				x_pixel_offset += (pattern_x - pattern_start_x_offset)%pattern_x_space;

				uint8 pattern_pixel = pattern[pattern_y_pos * 2 + 1];
				for( ; x_pixel_offset < final_pixel_offset; x_pixel_offset += pattern_x_space){
					uint8 current_pixel = dpi->bits[_pixel_offset];
					dpi->bits[_pixel_offset] = pattern_pixel;
					RCT2_GLOBAL(0x9AC00C, uint32)++;
					*pixel_store++ = (x_pixel_offset << 8) | current_pixel; //Store colour and position
				}
			}
		}

		pixel_offset += dpi->pitch + dpi->width;

		pattern_y_pos++;
		pattern_y_pos %= pattern_y_space;
	}
}

typedef void(*draw_rain_func)(int left, int top, int width, int height);

/**
*
*  rct2: 0x00684114
*/
void draw_light_rain(int left, int top, int width, int height){
	int edi = -RCT2_GLOBAL(0x00F663AC, int) + 8;
	int esi = (RCT2_GLOBAL(0x00F663AC, int) * 3) + 7;
	esi = -esi;

	edi += left;
	esi += top;

	sub_0x684027(left, top, width, height, 0, edi, esi);
	//RCT2_CALLPROC_X(0x00684027, left, top, width, height, esi, edi, 0);

	edi = -RCT2_GLOBAL(0x00F663AC, int) + 0x18;
	esi = (RCT2_GLOBAL(0x00F663AC, int) * 4) + 0x0D;
	esi = -esi;

	edi += left;
	esi += top;
	sub_0x684027(left, top, width, height, 0, edi, esi);
	//RCT2_CALLPROC_X(0x00684027, left, top, width, height, esi, edi, 0);
}

/**
*
*  rct2: 0x0068416D
*/
void draw_heavy_rain(int left, int top, int width, int height){
	int edi = -RCT2_GLOBAL(0x00F663AC, int);
	int esi = RCT2_GLOBAL(0x00F663AC, int) * 5;
	esi = -esi;

	edi += left;
	esi += top;

	sub_0x684027(left, top, width, height, 0, edi, esi);
	//RCT2_CALLPROC_X(0x00684027, left, top, width, height, esi, edi, 0);

	edi = -RCT2_GLOBAL(0x00F663AC, int) + 0x10;
	esi = (RCT2_GLOBAL(0x00F663AC, int) * 6) + 5;
	esi = -esi;

	edi += left;
	esi += top;

	sub_0x684027(left, top, width, height, 0, edi, esi);
	//RCT2_CALLPROC_X(0x00684027, left, top, width, height, esi, edi, 0);

	edi = -RCT2_GLOBAL(0x00F663AC, int) + 8;
	esi = (RCT2_GLOBAL(0x00F663AC, int) * 3) + 7;
	esi = -esi;

	edi += left;
	esi += top;

	sub_0x684027(left, top, width, height, 0, edi, esi);
	//RCT2_CALLPROC_X(0x00684027, left, top, width, height, esi, edi, 0);

	edi = -RCT2_GLOBAL(0x00F663AC, int) + 0x18;
	esi = (RCT2_GLOBAL(0x00F663AC, int) * 4) + 0x0D;
	esi = -esi;

	edi += left;
	esi += top;

	sub_0x684027(left, top, width, height, 0, edi, esi);
	//RCT2_CALLPROC_X(0x00684027, left, top, width, height, esi, edi, 0);
}

/**
*
*  rct2: 0x009AC058
*/
const draw_rain_func draw_rain_function[] = {
	NULL,
	&draw_light_rain,	// Light rain
	&draw_heavy_rain	// Heavy rain 
};




/**
*
*  rct2: 0x006ED801
*/
void sub_0x6ED801(int x, int y){
	if (RCT2_GLOBAL(0x9DE518, uint32) & (1 << 3)){
		rct_window* w = window_find_by_id(RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WINDOWCLASS, uint8), RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WINDOWNUMBER, uint16));
		if (w == NULL){
			tool_cancel();
		}
		else{
			RCT2_CALLPROC_X(w->event_handlers[WE_TOOL_UPDATE], x, y, 0, RCT2_GLOBAL(0x9DE546, uint16), (int)w, 0, 0);
		}
	}
}

/**
*
*  rct2: 0x006ED990
*/
void sub_0x6ED990(char cursor_id){
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
						sub_0x6ED990(CURSOR_HAND_POINT);
						return;
					}
					break;
				}
				cursorId = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_TOOL, uint8);
				subWindow = window_find_by_id(RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WINDOWCLASS, rct_windowclass), RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WINDOWNUMBER, rct_windownumber));
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
					sub_0x6ED990(cursorId);
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

	sub_0x6ED990(cursorId);
	return;
}

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

/**
 * 
 *  rct2: 0x006838BD
 */
void update_water_animation()
{
	RCT2_CALLPROC_EBPSAFE(0x006838BD);
}

/**
*
*  rct2: 0x00684383
*/
void call_draw_rain_func(rct_window* w, short left, short right, short top, short bottom, uint32 draw_rain_func)
{
	rct_viewport* vp = w->viewport;
	if (vp == NULL) {
		return;
	}

	left = max(left, vp->x);
	right = min(right, vp->width);

	top = max(top, vp->y);
	bottom = min(bottom, vp->height);

	if (left >= right || top >= bottom) {
		return;
	}

	int width = right - left;
	int height = bottom - top;

	draw_rain_function[draw_rain_func](left, top, width, height);
}

/**
*
*  rct2: 0x006842AF
*  From 0x00684383 on: split into call_draw_rain_func
*/
void draw_rain_window(rct_window* original_w, short left, short right, short top, short bottom, uint32 draw_rain_func)
{
	rct_window* newWindow = RCT2_GLOBAL(RCT2_ADDRESS_NEW_WINDOW_PTR, rct_window*);

	rct_window* w = original_w + 1; // Start from second window
	for (; ; w++) {
		if (w >= newWindow) {
			// Loop ended, draw rain for original_w
			call_draw_rain_func(original_w, left, right, top, bottom, draw_rain_func);
			return;
		}

		if (right <= w->x || bottom <= w->y) {
			continue;
		}

		if (RCT_WINDOW_RIGHT(w) <= left || RCT_WINDOW_BOTTOM(w) <= top) {
			continue;
		}

		if (left >= w->x) {
			break;
		}

		draw_rain_window(original_w, left, w->x, top, bottom, draw_rain_func);

		left = w->x;
		draw_rain_window(original_w, left, right, top, bottom, draw_rain_func);
		return;
	}

	sint16 w_right = RCT_WINDOW_RIGHT(w);
	if (right > w_right) {
		draw_rain_window(original_w, left, w_right, top, bottom, draw_rain_func);

		left = w_right;
		draw_rain_window(original_w, left, right, top, bottom, draw_rain_func); 
		return;
	}

	if (top < w->y) {
		draw_rain_window(original_w, left, right, top, w->y, draw_rain_func);

		top = w->y;
		draw_rain_window(original_w, left, right, top, bottom, draw_rain_func); 
		return;
	}

	sint16 w_bottom = RCT_WINDOW_BOTTOM(w);
	if (bottom > w_bottom) {
		draw_rain_window(original_w, left, right, top, w_bottom, draw_rain_func);

		top = w_bottom;
		draw_rain_window(original_w, left, right, top, bottom, draw_rain_func); 
		return;
	}
}

/**
*
*  rct2: 0x00684266
*/
void draw_rain_animation(uint32 draw_rain_func)
{
	rct_drawpixelinfo *screenDPI = RCT2_ADDRESS(RCT2_ADDRESS_SCREEN_DPI, rct_drawpixelinfo);
	short left = screenDPI->x;
	short right = left + screenDPI->width;
	short top = screenDPI->y;
	short bottom = top + screenDPI->height;

	rct_window* newWindow = (RCT2_GLOBAL(RCT2_ADDRESS_NEW_WINDOW_PTR, rct_window*));

	for (rct_window* w = g_window_list; w < newWindow; w++) {
		draw_rain_window(w, left, right, top, bottom, draw_rain_func);
	}
}

/**
 * 
 *  rct2: 0x00684218
 */
void update_rain_animation()
{
	if (RCT2_GLOBAL(0x009ABDF2, uint8) == 0)
		return;

	// Draw picked-up peep
	if (RCT2_GLOBAL(RCT2_ADDRESS_PICKEDUP_PEEP_SPRITE, uint32) != 0xFFFFFFFF) {
		gfx_draw_sprite(
			(rct_drawpixelinfo*)RCT2_ADDRESS_SCREEN_DPI,
			RCT2_GLOBAL(RCT2_ADDRESS_PICKEDUP_PEEP_SPRITE, uint32),
			RCT2_GLOBAL(RCT2_ADDRESS_PICKEDUP_PEEP_X, sint16),
			RCT2_GLOBAL(RCT2_ADDRESS_PICKEDUP_PEEP_Y, sint16)
		);
	}

	// Get rain draw function and draw rain
	uint32 draw_rain_func = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_RAIN_LEVEL, uint8);
	draw_rain_func = 2;
	if (draw_rain_func > 0 && !(RCT2_GLOBAL(0x009DEA6F, uint8) & 1))
		draw_rain_animation(draw_rain_func);
}

void game_update()
{
	int eax, tmp;

	// Handles picked-up peep and rain redraw
	RCT2_CALLPROC_EBPSAFE(0x006843DC);

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

	if (_gameSpeed > 1)
		eax = 1 << (_gameSpeed - 1);

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

	update_water_animation();
	update_rain_animation();

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

	sub_68B089();
	scenario_update();
	climate_update();
	RCT2_CALLPROC_EBPSAFE(0x006646E1);
	RCT2_CALLPROC_EBPSAFE(0x006A876D);
	peep_update_all();
	vehicle_update_all();
	RCT2_CALLPROC_EBPSAFE(0x00672AA4);	// update text effects
	RCT2_CALLPROC_EBPSAFE(0x006ABE4C);	// update rides
	park_update();
	RCT2_CALLPROC_EBPSAFE(0x00684C7A);
	RCT2_CALLPROC_EBPSAFE(0x006B5A2A);
	RCT2_CALLPROC_EBPSAFE(0x006B6456);	// update ride measurements
	RCT2_CALLPROC_EBPSAFE(0x0068AFAD);
	RCT2_CALLPROC_EBPSAFE(0x006BBC6B);	// vehicle and scream sounds
	peep_update_crowd_noise();
	climate_update_sound();
	news_item_update_current();
	RCT2_CALLPROC_EBPSAFE(0x0067009A);	// scenario editor opening of windows for a phase

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
		game_do_command(0, 1, 0, 0, GAME_COMMAND_LOAD_OR_QUIT, 2, 0); 
	}

	if (RCT2_GLOBAL(0x009ABDF2, uint8) != 0) {
		for (w = g_window_list; w < RCT2_GLOBAL(RCT2_ADDRESS_NEW_WINDOW_PTR, rct_window*); w++)
			RCT2_CALLPROC_X(w->event_handlers[WE_UNKNOWN_07], 0, 0, 0, 0, (int)w, 0, 0);

		RCT2_CALLPROC_EBPSAFE(0x006EA73F);
		RCT2_CALLPROC_EBPSAFE(0x006E8346); // update_cursor_position

		{
			// int eax, ebx, ecx, edx, esi, edi, ebp;
			int eax, ebx, ecx;

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
				process_mouse_over(eax, ebx);
				//RCT2_CALLPROC_X(0x006ED833, eax, ebx, 0, 0, 0, 0, 0);
				sub_0x6ED801(eax, ebx);
				//RCT2_CALLPROC_EBPSAFE(0x006ED801);
			}
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
 *  rct2: 0x006E876D
 *
 */
void invalidate_scroll(){
	int window_no = RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WINDOWNUMBER, uint16);
	int window_cls = RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WINDOWCLASS, uint8);

	rct_window* wind = window_find_by_id(window_cls, window_no);
	if (wind == NULL) return;

	int scroll_id = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_SCROLL_ID, uint32);
	//Reset to basic scroll
	wind->scrolls[scroll_id / sizeof(rct_scroll)].flags &= 0xFF11;

	window_invalidate_by_id(RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WINDOWCLASS, uint8), RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WINDOWNUMBER, uint16));
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
		RCT2_CALLPROC_X(0x006E8DA7, x, y, state, widgetIndex, (int)w, (int)widget, 0);
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
		RCT2_CALLPROC_X(0x006E8DA7, x, y, state, widgetIndex, (int)w, (int)widget, 0);
		break;
	case INPUT_STATE_VIEWPORT_LEFT:
		RCT2_CALLPROC_X(0x006E87B4, x, y, state, widgetIndex, (int)w, (int)widget, 0);
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
	rct_window *w = window_find_by_id(RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_OVER_WINDOWCLASS, rct_windowclass), RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_OVER_WINDOWNUMBER, rct_windownumber));
	if (w == NULL)
		return;

	RCT2_CALLPROC_X(w->event_handlers[WE_INVALIDATE], 0, 0, 0, 0, (int)w, 0, 0);
	if (w->widgets[RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_OVER_WIDGETINDEX, rct_windownumber)].type == WWT_FLATBTN)
		widget_invalidate(RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_OVER_WINDOWCLASS, rct_windowclass), RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_OVER_WINDOWNUMBER, rct_windownumber), RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_OVER_WIDGETINDEX, rct_windownumber));
}

static void RCT2_CALLPROC_WE_MOUSE_DOWN(int address,  int widgetIndex, rct_window*w, rct_widget* widget )
{
#ifdef _MSC_VER
	__asm {
		push address
		push widget
		push w
		push widgetIndex
		mov edi, widget
		mov edx, widgetIndex
		mov esi, w
		call[esp + 12]
		add esp, 16
	}
#else
	__asm__("\
			push %[address]\n\
			mov edi, %[widget] \n\
			mov eax, %[w]  \n\
			mov edx, %[widgetIndex] \n\
			push edi \n\
			push eax \n\
			push edx \n\
			mov esi, %[w]	\n\
			call [esp+12]	\n\
			add esp, 16	\n\
			" :[address] "+m" (address), [w] "+m" (w), [widget] "+m" (widget), [widgetIndex] "+m" (widgetIndex): : "eax", "esi", "edx", "edi"
		);
#endif
}

/**
 *  Horizontal scrollbar's "left" button held down, scroll it to the left
 *  rct2: 0x006E9A60
 */
static void input_hscrollbar_leftbutton(rct_window* w)
{
	rct_windowclass windowClass;
	rct_windownumber windowNumber;
	rct_window* w2;
	rct_widget* widget;
	rct_scroll* scroll;
	uint16 widgetIndex;
	sint16 left;

	windowClass = RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WINDOWCLASS, rct_windowclass);
	windowNumber = RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WINDOWNUMBER, rct_windownumber);
	w2 = window_find_by_id(windowClass, windowNumber);

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

	widgetIndex = RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WIDGETINDEX, uint8);
	windowClass = RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WINDOWCLASS, uint8);
	windowClass |= 0x80;

	window_invalidate_by_id(widgetIndex, windowClass);
}


/**
 *  Horizontal scrollbar's "right" button held down, scroll it to the right
 *  rct2: 0x006E9ABF
 */
static void input_hscrollbar_rightbutton(rct_window* w)
{
	rct_windowclass windowClass;
	rct_windownumber windowNumber;
	rct_window* w2;
	rct_widget* widget;
	rct_scroll* scroll;
	uint16 widgetIndex;
	sint16 left, widgetWidth;

	windowClass = RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WINDOWCLASS, rct_windowclass);
	windowNumber = RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WINDOWNUMBER, rct_windownumber);
	w2 = window_find_by_id(windowClass, windowNumber);

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

	widgetIndex = RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WIDGETINDEX, uint8);
	windowClass = RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WINDOWCLASS, uint8);
	windowClass |= 0x80;

	window_invalidate_by_id(widgetIndex, windowClass);
}

/**
 *  Horizontal scrollbar's left trough was clicked
 *  rct2: 0x006E9B47
 */
static void input_hscrollbar_left_trough(rct_window* w)
{
	rct_windowclass windowClass;
	rct_windownumber windowNumber;
	rct_window* w2;
	rct_widget* widget;
	rct_scroll* scroll;
	uint16 widgetIndex;
	sint16 left, widgetWidth;

	windowClass = RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WINDOWCLASS, rct_windowclass);
	windowNumber = RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WINDOWNUMBER, rct_windownumber);
	w2 = window_find_by_id(windowClass, windowNumber);

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

	widgetIndex = RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WIDGETINDEX, uint8);
	windowClass = RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WINDOWCLASS, uint8);
	windowClass |= 0x80;

	window_invalidate_by_id(widgetIndex, windowClass);
}

/**
 *  Horizontal scrollbar's right trough was clicked
 *  rct2: 0x006E9BB7
 */
static void input_hscrollbar_right_trough(rct_window* w)
{
	rct_windowclass windowClass;
	rct_windownumber windowNumber;
	rct_window* w2;
	rct_widget* widget;
	rct_scroll* scroll;
	uint16 widgetIndex;
	sint16 left, widgetWidth;

	windowClass = RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WINDOWCLASS, rct_windowclass);
	windowNumber = RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WINDOWNUMBER, rct_windownumber);
	w2 = window_find_by_id(windowClass, windowNumber);

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

	widgetIndex = RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WIDGETINDEX, uint8);
	windowClass = RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WINDOWCLASS, uint8);
	windowClass |= 0x80;

	window_invalidate_by_id(widgetIndex, windowClass);
}

/**
 *  Vertical scrollbar's "top" button held down, scroll it upwards
 *  rct2: 0x006E9C37
 */
static void input_vscrollbar_topbutton(rct_window* w)
{
	rct_windowclass windowClass;
	rct_windownumber windowNumber;
	rct_window* w2;
	rct_widget* widget;
	rct_scroll* scroll;
	uint16 widgetIndex;
	sint16 top;

	windowClass = RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WINDOWCLASS, rct_windowclass);
	windowNumber = RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WINDOWNUMBER, rct_windownumber);
	w2 = window_find_by_id(windowClass, windowNumber);

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

	widgetIndex = RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WIDGETINDEX, uint8);
	windowClass = RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WINDOWCLASS, uint8);
	windowClass |= 0x80;

	window_invalidate_by_id(widgetIndex, windowClass);
}

/**
*  Vertical scrollbar's "bottom" button held down, scroll it downwards
*  rct2: 0x006E9C96
*/
static void input_vscrollbar_bottombutton(rct_window* w)
{
	rct_windowclass windowClass;
	rct_windownumber windowNumber;
	rct_window* w2;
	rct_widget* widget;
	rct_scroll* scroll;
	uint16 widgetIndex;
	sint16 top, widgetHeight;

	windowClass = RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WINDOWCLASS, rct_windowclass);
	windowNumber = RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WINDOWNUMBER, rct_windownumber);
	w2 = window_find_by_id(windowClass, windowNumber);

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

	widgetIndex = RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WIDGETINDEX, uint8);
	windowClass = RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WINDOWCLASS, uint8);
	windowClass |= 0x80;

	window_invalidate_by_id(widgetIndex, windowClass);
}

/**
*  Vertical scrollbar's top trough was clicked
*  rct2: 0x006E9D1E
*/
static void input_vscrollbar_top_trough(rct_window* w)
{
	rct_windowclass windowClass;
	rct_windownumber windowNumber;
	rct_window* w2;
	rct_widget* widget;
	rct_scroll* scroll;
	uint16 widgetIndex;
	sint16 top, widgetHeight;

	windowClass = RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WINDOWCLASS, rct_windowclass);
	windowNumber = RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WINDOWNUMBER, rct_windownumber);
	w2 = window_find_by_id(windowClass, windowNumber);

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

	widgetIndex = RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WIDGETINDEX, uint8);
	windowClass = RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WINDOWCLASS, uint8);
	windowClass |= 0x80;

	window_invalidate_by_id(widgetIndex, windowClass);
}

/**
*  Vertical scrollbar's bottom trough was clicked
*  rct2: 0x006E9D8E
*/
static void input_vscrollbar_bottom_trough(rct_window* w)
{
	rct_windowclass windowClass;
	rct_windownumber windowNumber;
	rct_window* w2;
	rct_widget* widget;
	rct_scroll* scroll;
	uint16 widgetIndex;
	sint16 top, widgetHeight;

	windowClass = RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WINDOWCLASS, rct_windowclass);
	windowNumber = RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WINDOWNUMBER, rct_windownumber);
	w2 = window_find_by_id(windowClass, windowNumber);

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

	widgetIndex = RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WIDGETINDEX, uint8);
	windowClass = RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WINDOWCLASS, uint8);
	windowClass |= 0x80;

	window_invalidate_by_id(widgetIndex, windowClass);
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
	}

	window_close_by_id(WC_ERROR, 0);
	window_close_by_id(WC_TOOLTIP, 0);

	w = window_find_by_id(windowClass, windowNumber);
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
		if (!(RCT2_GLOBAL(0x009DE518, uint32) & (1 << 3)))
			break;

		w = window_find_by_id(RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WINDOWCLASS, rct_windowclass), RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WINDOWNUMBER, rct_windownumber));
		if (w == NULL)
			break;

		RCT2_CALLPROC_X(w->event_handlers[WE_TOOL_DOWN], x, y, 0, RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WIDGETINDEX, uint16), (int)w, 0, 0);
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
		// comment check as it disables the rotate station/building button in construction window
// 		if (!widget_is_enabled(w, widgetIndex))
// 			break;
		if (widget_is_disabled(w, widgetIndex))
			break;

		sound_play_panned(SOUND_CLICK_1, w->x + (widget->left + widget->right) / 2);
		
		// Set new cursor down widget
		RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WINDOWCLASS, rct_windowclass) = windowClass;
		RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WINDOWNUMBER, rct_windownumber) = windowNumber;
		RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WIDGETINDEX, uint16) = widgetIndex;
		RCT2_GLOBAL(0x009DE518, uint32) |= (1 << 0);
		RCT2_GLOBAL(RCT2_ADDRESS_INPUT_STATE, uint8) = INPUT_STATE_WIDGET_PRESSED;
		RCT2_GLOBAL(0x009DE528, uint16) = 1;

		widget_invalidate(windowClass, windowNumber, widgetIndex);
		RCT2_CALLPROC_WE_MOUSE_DOWN(w->event_handlers[WE_MOUSE_DOWN], widgetIndex, w, widget);
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
		mainWindow->saved_view_x += scrollX * (12 << mainWindow->viewport->zoom);
		RCT2_GLOBAL(0x009DE518, uint32) |= (1 << 7);
	}
	if (scrollY != 0) {
		mainWindow->saved_view_y += scrollY * (12 << mainWindow->viewport->zoom);
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
		mainWindow->saved_view_x += scrollX * (12 << mainWindow->viewport->zoom);
		RCT2_GLOBAL(0x009DE518, uint32) |= (1 << 7);
	}
	if (scrollY != 0) {
		mainWindow->saved_view_y += scrollY * (12 << mainWindow->viewport->zoom);
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
		window = window_find_by_id(WC_ERROR, 0);
		if (window != NULL)
			window_close(window);
		else if (RCT2_GLOBAL(0x009DE518, uint32) & (1 << 3))
			tool_cancel();
		break;
	case SHORTCUT_PAUSE_GAME:
		if (!(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & 10)) {
			window = window_find_by_id(WC_TOP_TOOLBAR, 0);
			if (window != NULL) {
				window_invalidate(window);
				window_event_helper(window, 0, WE_MOUSE_UP);
			}
		}
		break;
	case SHORTCUT_ZOOM_VIEW_OUT:
		if (!(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & 2) || RCT2_GLOBAL(0x0141F570, uint8) == 1) {
			if (!(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & 8)) {
				window = window_find_by_id(WC_TOP_TOOLBAR, 0);
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
				window = window_find_by_id(WC_TOP_TOOLBAR, 0);
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
				window = window_find_by_id(WC_TOP_TOOLBAR, 0);
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
		RCT2_CALLPROC_X(0x0066CF8A, 0, 0, 0, 0, 0, 0, 0);
		break;
	case SHORTCUT_REMOVE_BASE_LAND_TOGGLE:
		RCT2_CALLPROC_X(0x0066CF8A, 1, 0, 0, 0, 0, 0, 0);
		break;
	case SHORTCUT_REMOVE_VERTICAL_LAND_TOGGLE:
		RCT2_CALLPROC_X(0x0066CF8A, 2, 0, 0, 0, 0, 0, 0);
		break;
	case SHORTCUT_SEE_THROUGH_RIDES_TOGGLE:
		RCT2_CALLPROC_X(0x0066CF8A, 4, 0, 0, 0, 0, 0, 0);
		break;
	case SHORTCUT_SEE_THROUGH_SCENERY_TOGGLE:
		RCT2_CALLPROC_X(0x0066CF8A, 5, 0, 0, 0, 0, 0, 0);
		break;
	case SHORTCUT_INVISIBLE_SUPPORTS_TOGGLE:
		RCT2_CALLPROC_X(0x0066CF8A, 6, 0, 0, 0, 0, 0, 0);
		break;
	case SHORTCUT_INVISIBLE_PEOPLE_TOGGLE:
		RCT2_CALLPROC_X(0x0066CF8A, 7, 0, 0, 0, 0, 0, 0);
		break;
	case SHORTCUT_HEIGHT_MARKS_ON_LAND_TOGGLE:
		RCT2_CALLPROC_X(0x0066CF8A, 9, 0, 0, 0, 0, 0, 0);
		break;
	case SHORTCUT_HEIGHT_MARKS_ON_RIDE_TRACKS_TOGGLE:
		RCT2_CALLPROC_X(0x0066CF8A, 10, 0, 0, 0, 0, 0, 0);
		break;
	case SHORTCUT_HEIGHT_MARKS_ON_PATHS_TOGGLE:
		RCT2_CALLPROC_X(0x0066CF8A, 11, 0, 0, 0, 0, 0, 0);
		break;
	case SHORTCUT_ADJUST_LAND:
		if (!(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & 2) || RCT2_GLOBAL(0x0141F570, uint8) == 1) {
			if (!(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & 0x0C)) {
				window = window_find_by_id(WC_TOP_TOOLBAR, 0);
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
				window = window_find_by_id(WC_TOP_TOOLBAR, 0);
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
				window = window_find_by_id(WC_TOP_TOOLBAR, 0);
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
				window = window_find_by_id(WC_TOP_TOOLBAR, 0);
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
				window = window_find_by_id(WC_TOP_TOOLBAR, 0);
				if (window != NULL) {
					window_invalidate(window);
					window_event_helper(window, 11, WE_MOUSE_UP);
				}
			}
		}
		break;
	case SHORTCUT_SHOW_FINANCIAL_INFORMATION:
		if (!(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & 0x0C))
			if (!(RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & 0x800))
				window_finances_open();
		break;
	case SHORTCUT_SHOW_RESEARCH_INFORMATION:
		if (!(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & 0x0E)) {
			// Open new ride window
			RCT2_CALLPROC_EBPSAFE(0x006B3CFF);
			window = window_find_by_id(WC_CONSTRUCT_RIDE, 0);
			if (window != NULL)
				RCT2_CALLPROC_WE_MOUSE_DOWN(window->event_handlers[WE_MOUSE_DOWN], 10, window, NULL);
		}
		break;
	case SHORTCUT_SHOW_RIDES_LIST:
		if (!(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & 0x0E)) {
			window = window_find_by_id(WC_TOP_TOOLBAR, 0);
			if (window != NULL) {
				window_invalidate(window);
				window_event_helper(window, 12, WE_MOUSE_UP);
			}
		}
		break;
	case SHORTCUT_SHOW_PARK_INFORMATION:
		if (!(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & 0x0E)) {
			window = window_find_by_id(WC_TOP_TOOLBAR, 0);
			if (window != NULL) {
				window_invalidate(window);
				window_event_helper(window, 13, WE_MOUSE_UP);
			}
		}
		break;
	case SHORTCUT_SHOW_GUEST_LIST:
		if (!(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & 0x0E)) {
			window = window_find_by_id(WC_TOP_TOOLBAR, 0);
			if (window != NULL) {
				window_invalidate(window);
				window_event_helper(window, 15, WE_MOUSE_UP);
			}
		}
		break;
	case SHORTCUT_SHOW_STAFF_LIST:
		if (!(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & 0x0E)) {
		  window = window_find_by_id(WC_TOP_TOOLBAR, 0);
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
				window = window_find_by_id(WC_TOP_TOOLBAR, 0);
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
	_gameSpeed = min(8, _gameSpeed + 1);
}

void game_reduce_game_speed()
{
	_gameSpeed = max(1, _gameSpeed - 1);
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
 *  rct2: 0x0069C62C
 *
 * @param cost (ebp)
 */
static int game_check_affordability(int cost)
{
	if (cost <= 0)return cost;
	if (RCT2_GLOBAL(0x141F568, uint8) & 0xF0)return cost;
	
	if (!(RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32)&(1 << 8))){
		if (cost <= (sint32)(DECRYPT_MONEY(RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_MONEY_ENCRYPTED, sint32))))return cost;
	}
	RCT2_GLOBAL(0x13CE952, uint32) = cost;
	RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TEXT, uint16) = 827;
	return 0x80000000;
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
	game_do_command_p(esi, &eax, &ebx, &ecx, &edx, &esi, &edi, &ebp);
}

/**
*
*  rct2: 0x006677F2 with pointers as arguments
*
* @param flags (ebx)
* @param command (esi)
*/
int game_do_command_p(int command, int *eax, int *ebx, int *ecx, int *edx, int *esi, int *edi, int *ebp)
{
	int cost, flags, insufficientFunds;
	int original_ebx, original_edx, original_esi, original_edi, original_ebp;

	*esi = command;
	original_ebx = *ebx;
	original_edx = *edx;
	original_esi = *esi;
	original_edi = *edi;
	original_ebp = *ebp;

	flags = *ebx;
	RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TEXT, uint16) = 0xFFFF;

	// Increment nest count
	RCT2_GLOBAL(0x009A8C28, uint8)++;

	*ebx &= ~1;

	// Primary command
	RCT2_CALLFUNC_X(game_do_command_table[command], eax, ebx, ecx, edx, esi, edi, ebp);
	cost = *ebx;

	if (cost != 0x80000000) {
		// Check funds
		insufficientFunds = 0;
		if (RCT2_GLOBAL(0x009A8C28, uint8) == 1 && !(flags & 4) && !(flags & 0x20) && cost != 0)
			insufficientFunds = game_check_affordability(cost);

		if (insufficientFunds != 0x80000000) {
			*ebx = original_ebx;
			*edx = original_edx;
			*esi = original_esi;
			*edi = original_edi;
			*ebp = original_ebp;

			if (!(flags & 1)) {
				// Decrement nest count
				RCT2_GLOBAL(0x009A8C28, uint8)--;
				return cost;
			}

			// Secondary command
			RCT2_CALLFUNC_X(game_do_command_table[command], eax, ebx, ecx, edx, esi, edi, ebp);
			*edx = *ebx;

			if (*edx != 0x80000000 && *edx < cost)
				cost = *edx;

			// Decrement nest count
			RCT2_GLOBAL(0x009A8C28, uint8)--;
			if (RCT2_GLOBAL(0x009A8C28, uint8) != 0)
				return cost;

			// 
			if (!(flags & 0x20)) {
				// Update money balance
				finance_payment(cost, RCT2_GLOBAL(0x0141F56C, uint8));
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
void game_pause_toggle()
{
	char input_bl;

	#ifdef _MSC_VER
	__asm mov input_bl, bl
	#else
	__asm__ ( "mov %[input_bl], bl " : [input_bl] "+m" (input_bl) );
	#endif


	if (input_bl & 1) {
		RCT2_GLOBAL(0x009DEA6E, uint32) ^= 1;
		window_invalidate_by_id(WC_TOP_TOOLBAR, 0);
		if (RCT2_GLOBAL(0x009DEA6E, uint32) & 1)
			pause_sounds();
		else
			unpause_sounds();
	}

	#ifdef _MSC_VER
	__asm mov ebx, 0
	#else
	__asm__ ( "mov ebx, 0 "  );
	#endif

}

/**
 * 
 *  rct2: 0x0066DB5F
 */
static void game_load_or_quit()
{
	char input_bl, input_dl;
	short input_di;

	#ifdef _MSC_VER
	__asm mov input_bl, bl
	#else
	__asm__ ( "mov %[input_bl], bl " : [input_bl] "+m" (input_bl) );
	#endif

	#ifdef _MSC_VER
	__asm mov input_dl, dl
	#else
	__asm__ ( "mov %[input_dl], dl " : [input_dl] "+m" (input_dl) );
	#endif

	#ifdef _MSC_VER
	__asm mov input_di, di
	#else
	__asm__ ( "mov %[input_di], di " : [input_di] "+m" (input_di) );
	#endif

	if (!(input_bl & 1))
		return; // 0;

	switch (input_dl) {
	case 0:
		RCT2_GLOBAL(RCT2_ADDRESS_SAVE_PROMPT_MODE, uint16) = input_di;
		window_save_prompt_open();
		break;
	case 1:
		window_close_by_id(WC_SAVE_PROMPT, 0);
		break;
	default:
		game_load_or_quit_no_save_prompt();
		break;
	}

	#ifdef _MSC_VER
	__asm mov ebx, 0
	#else
	__asm__ ( "mov ebx, 0 "  );
	#endif

}

/**
*
*  rct2: 0x00669E55
*/
static void game_update_staff_colour()
{
	byte tabIndex, colour, _bl;
	int spriteIndex;
	rct_peep *peep;
	
	#ifdef _MSC_VER
	__asm mov _bl, bl
	#else
	__asm__("mov %[_bl], bl " : [_bl] "+m" (_bl));
	#endif

	#ifdef _MSC_VER
	__asm mov tabIndex, bh
	#else
	__asm__("mov %[tabIndex], bh " : [tabIndex] "+m" (tabIndex));
	#endif

	#ifdef _MSC_VER
	__asm mov colour, dh
	#else
	__asm__("mov %[colour], bh " : [colour] "+m" (colour));
	#endif

	if (_bl & 1) {
		RCT2_ADDRESS(RCT2_ADDRESS_HANDYMAN_COLOUR, uint8)[tabIndex] = colour;

		FOR_ALL_PEEPS(spriteIndex, peep) {
			if (peep->type == PEEP_TYPE_STAFF && peep->staff_type == tabIndex) {
				peep->tshirt_colour = colour;
				peep->trousers_colour = colour;
			}
		}
	}

	gfx_invalidate_screen();
	
	#ifdef _MSC_VER
	__asm mov ebx, 0
	#else
	__asm__("mov ebx, 0 ");
	#endif
}

/**
 * 
 *  rct2: 0x00674F40
 */
static int open_landscape_file_dialog()
{
	int result;
	format_string((char*)RCT2_ADDRESS_COMMON_STRING_FORMAT_BUFFER, STR_LOAD_LANDSCAPE_DIALOG_TITLE, 0);
	strcpy((char*)0x0141EF68, (char*)RCT2_ADDRESS_LANDSCAPES_PATH);
	format_string((char*)0x0141EE68, STR_RCT2_LANDSCAPE_FILE, 0);
	pause_sounds();
	result = osinterface_open_common_file_dialog(1, (char*)RCT2_ADDRESS_COMMON_STRING_FORMAT_BUFFER, (char*)0x0141EF68, "*.SV6;*.SV4;*.SC6", (char*)0x0141EE68);
	unpause_sounds();
	// window_proc
	return result;
}

/**
 * 
 *  rct2: 0x00674EB6
 */
static int open_load_game_dialog()
{
	int result;
	format_string((char*)RCT2_ADDRESS_COMMON_STRING_FORMAT_BUFFER, STR_LOAD_GAME_DIALOG_TITLE, 0);
	strcpy((char*)0x0141EF68, (char*)RCT2_ADDRESS_SAVED_GAMES_PATH);
	format_string((char*)0x0141EE68, STR_RCT2_SAVED_GAME, 0);
	pause_sounds();
	result = osinterface_open_common_file_dialog(1, (char*)RCT2_ADDRESS_COMMON_STRING_FORMAT_BUFFER, (char*)0x0141EF68, "*.SV6", (char*)0x0141EE68);
	unpause_sounds();
	// window_proc
	return result;
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
		char *esi = (char*)0x0141EF67;
		while (1) {
			esi++;
			if (*esi == '.')
				break;
			if (*esi != 0)
				continue;
			strcpy(esi, ".SC6");
			break;
		}
		strcpy((char*)RCT2_ADDRESS_SAVED_GAMES_PATH_2, (char*)0x0141EF68);

		RCT2_CALLPROC_EBPSAFE(0x006758C0); // landscape_load
		if (1) {
			gfx_invalidate_screen();
			rct2_endupdate();
		} else {
			RCT2_GLOBAL(0x009DEA66, uint16) = 0;
			rct2_endupdate();
		}
	}
}

/**
 * 
 *  rct2: 0x00675E1B
 */
int game_load_save()
{
	rct_window *mainWindow;
	FILE *file;
	char *path;
	int i, j;

	path = (char*)0x0141EF68;
	file = fopen(path, "rb");
	if (file == NULL) {
		RCT2_GLOBAL(0x009AC31B, uint8) = 255;
		RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_STRING_ID, uint16) = STR_FILE_CONTAINS_INVALID_DATA;
		return 0;
	}

	if (!sawyercoding_validate_checksum(file)) {
		fclose(file);
		RCT2_GLOBAL(0x009AC31B, uint8) = 255;
		RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_STRING_ID, uint16) = STR_FILE_CONTAINS_INVALID_DATA;
		return 0;
	}

	rct_s6_header *s6Header = (rct_s6_header*)0x009E34E4;
	rct_s6_info *s6Info = (rct_s6_info*)0x0141F570;

	// Read first chunk
	sawyercoding_read_chunk(file, (uint8*)s6Header);
	if (s6Header->type == S6_TYPE_SAVEDGAME) {
		// Read packed objects
		if (s6Header->num_packed_objects > 0) {
			j = 0;
			for (i = 0; i < s6Header->num_packed_objects; i++)
				j += object_load_packed();
			if (j > 0)
				object_list_load();
		}
	}

	object_read_and_load_entries(file);

	// Read flags (16 bytes)
	sawyercoding_read_chunk(file, (uint8*)RCT2_ADDRESS_CURRENT_MONTH_YEAR);

	// Read map elements
	memset((void*)RCT2_ADDRESS_MAP_ELEMENTS, 0, MAX_MAP_ELEMENTS * sizeof(rct_map_element));
	sawyercoding_read_chunk(file, (uint8*)RCT2_ADDRESS_MAP_ELEMENTS);

	// Read game data, including sprites
	sawyercoding_read_chunk(file, (uint8*)0x010E63B8);

	fclose(file);

	// Check expansion pack
	// RCT2_CALLPROC_EBPSAFE(0x006757E6);

	// The rest is the same as in scenario load and play
	RCT2_CALLPROC_EBPSAFE(0x006A9FC0);
	map_update_tile_pointers();
	reset_0x69EBE4();// RCT2_CALLPROC_EBPSAFE(0x0069EBE4);
	RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) = SCREEN_FLAGS_PLAYING;
	viewport_init_all();
	game_create_windows();
	mainWindow = window_get_main();

	mainWindow->viewport_target_sprite = -1;
	mainWindow->saved_view_x = RCT2_GLOBAL(RCT2_ADDRESS_SAVED_VIEW_X, sint16);
	mainWindow->saved_view_y = RCT2_GLOBAL(RCT2_ADDRESS_SAVED_VIEW_Y, sint16);
	uint8 _cl = (RCT2_GLOBAL(0x0138869E, sint16) & 0xFF) - mainWindow->viewport->zoom;
	mainWindow->viewport->zoom = RCT2_GLOBAL(0x0138869E, sint16) & 0xFF;
	*((char*)(&RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_ROTATION, sint32))) = RCT2_GLOBAL(0x0138869E, sint16) >> 8;
	if (_cl != 0) {
		if (_cl < 0) {
			_cl = -_cl;
			mainWindow->viewport->view_width >>= _cl;
			mainWindow->viewport->view_height >>= _cl;
		} else {
			mainWindow->viewport->view_width <<= _cl;
			mainWindow->viewport->view_height <<= _cl;
		}
	}
	mainWindow->saved_view_x -= mainWindow->viewport->view_width >> 1;
	mainWindow->saved_view_y -= mainWindow->viewport->view_height >> 1;
	window_invalidate(mainWindow);

	sub_0x0069E9A7(); 
	RCT2_CALLPROC_EBPSAFE(0x006DFEE4);
	window_new_ride_init_vars();
	RCT2_GLOBAL(0x009DEB7C, uint16) = 0;
	if (RCT2_GLOBAL(0x0013587C4, uint32) == 0)		// this check is not in scenario play
		sub_69E869();

	RCT2_CALLPROC_EBPSAFE(0x006837E3); // (palette related)
	gfx_invalidate_screen();
	return 1;
}

/*
 *
 * rct2: 0x0069E9A7
 */
void sub_0x0069E9A7(){
	//RCT2_CALLPROC_EBPSAFE(0x0069E9A7);
	//return;
	for (rct_sprite* spr = g_sprite_list; spr < (rct_sprite*)RCT2_ADDRESS_SPRITES_NEXT_INDEX; ++spr){
		if (spr->unknown.sprite_identifier != 0xFF){
			RCT2_CALLPROC_X(0x0069E9D3, spr->unknown.x, 0, spr->unknown.y, spr->unknown.z, (int)spr, 0, 0);
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
		char *esi = (char*)0x0141EF67;
		while (1) {
			esi++;
			if (*esi == '.')
				break;
			if (*esi != 0)
				continue;
			strcpy(esi, ".SV6");
			break;
		}
		strcpy((char*)RCT2_ADDRESS_SAVED_GAMES_PATH_2, (char*)0x0141EF68);

		if (game_load_save()) {
			gfx_invalidate_screen();
			rct2_endupdate();
		} else {
			RCT2_GLOBAL(0x009DEA66, uint16) = 0;
			rct2_endupdate();
		}
	}
}

char save_game()
{
	int eax, ebx, ecx, edx, esi, edi, ebp;
	RCT2_CALLFUNC_X(0x006750E9, &eax, &ebx, &ecx, &edx, &esi, &edi, &ebp);
	if (eax == 0) {
		// user pressed "cancel"
		gfx_invalidate_screen();
		return 0;
	}
	
	char *src = (char*)0x0141EF67;
	do {
		src++;
	} while (*src != '.' && *src != '\0');
	strcpy(src, ".SV6");
	strcpy((char*) RCT2_ADDRESS_SAVED_GAMES_PATH_2, (char*) 0x0141EF68);
	
	eax = 0;
	if (RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_FLAGS, uint8) & 8)
		eax |= 1;
	RCT2_CALLPROC_X(0x006754F5, eax, 0, 0, 0, 0, 0, 0);
	// check success?

	game_do_command(0, 1047, 0, -1, GAME_COMMAND_0, 0, 0);
	gfx_invalidate_screen();
	
	return 1;
}

/**
 * 
 *  rct2: 0x006E3879
 */
void rct2_exit()
{
	RCT2_CALLPROC_EBPSAFE(0x006E3879);
	//Post quit message does not work in 0x6e3879 as its windows only.
}

/**
 * 
 *  rct2: 0x0066DB79
 */
void game_load_or_quit_no_save_prompt()
{
	if (RCT2_GLOBAL(RCT2_ADDRESS_SAVE_PROMPT_MODE, uint16) < 1) {
		game_do_command(0, 1, 0, 1, GAME_COMMAND_LOAD_OR_QUIT, 0, 0);
		tool_cancel();
		if (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & 2)
			load_landscape();
		else
			load_game();
	} else if (RCT2_GLOBAL(RCT2_ADDRESS_SAVE_PROMPT_MODE, uint16) == 1) {
		game_do_command(0, 1, 0, 1, GAME_COMMAND_LOAD_OR_QUIT, 0, 0);
		if (RCT2_GLOBAL(0x009DE518, uint32) & (1 << 5)) {
			RCT2_CALLPROC_EBPSAFE(0x0040705E);
			RCT2_GLOBAL(0x009DE518, uint32) &= ~(1 << 5);
		}
		title_load();
		rct2_endupdate();
	} else {
		rct2_exit();
	}
}

#pragma region Game command function table

static uint32 game_do_command_table[58] = {
	0x006B2FC5,
	0x0066397F,
	(uint32)game_pause_toggle,
	0x006C511D,
	0x006C5B69,
	(uint32)game_load_or_quit,
	0x006B3F0F,
	0x006B49D9,
	0x006B4EA6,
	0x006B52D4,
	0x006B578B, // 10
	0x006B5559,
	0x006660A8,
	0x0066640B,
	0x006E0E01,
	0x006E08F4,
	0x006E650F,
	0x006A61DE,
	0x006A68AE,
	0x006A67C0,
	0x00663CCD, // 20
	0x006B53E9,
	0x00698D6C,
	0x0068C542,
	0x0068C6D1,
	0x0068BC01,
	0x006E66A0,
	0x006E6878,
	0x006C5AE9,
	0x006BEFA1,
	0x006C09D1, // 30
	0x006C0B83,
	0x006C0BB5,
	0x00669C6D,
	0x00669D4A,
	0x006649BD,
	0x006666E7,
	0x00666A63,
	0x006CD8CE,
	0x00669E30,
	(uint32)game_update_staff_colour, // 40
	0x006E519A,
	0x006E5597,
	0x006B893C,
	0x006B8E1B,
	0x0069DFB3,
	0x00684A7F,
	0x006D13FE,
	0x0069E73C,
	0x006CDEE4,
	0x006B9E6D, // 50
	0x006BA058,
	0x006E0F26,
	0x006E56B5,
	0x006B909A,
	0x006BA16A,
	0x006648E3,
	0x0068DF91
};

#pragma endregion
