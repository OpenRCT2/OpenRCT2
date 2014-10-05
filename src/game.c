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
#include "finance.h"
#include "game.h"
#include "input.h"
#include "news_item.h"
#include "object.h"
#include "osinterface.h"
#include "park.h"
#include "peep.h"
#include "rct2.h"
#include "research.h"
#include "ride.h"
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
#include "staff.h"
#include "window_error.h"
#include "window_tooltip.h"


int gGameSpeed = 1;

typedef void(*draw_rain_func)(int left, int top, int width, int height);

/**
*
*  rct2: 0x00684114
*/
void draw_light_rain(int left, int top, int width, int height){
	int x_start = -RCT2_GLOBAL(RCT2_ADDRESS_SCENARIO_TICKS, int) + 8;
	int y_start = (RCT2_GLOBAL(RCT2_ADDRESS_SCENARIO_TICKS, int) * 3) + 7;
	y_start = -y_start;

	x_start += left;
	y_start += top;

	gfx_draw_rain(left, top, width, height, x_start, y_start);

	x_start = -RCT2_GLOBAL(RCT2_ADDRESS_SCENARIO_TICKS, int) + 0x18;
	y_start = (RCT2_GLOBAL(RCT2_ADDRESS_SCENARIO_TICKS, int) * 4) + 0x0D;
	y_start = -y_start;

	x_start += left;
	y_start += top;
	gfx_draw_rain(left, top, width, height, x_start, y_start);
}

/**
*
*  rct2: 0x0068416D
*/
void draw_heavy_rain(int left, int top, int width, int height){
	int x_start = -RCT2_GLOBAL(RCT2_ADDRESS_SCENARIO_TICKS, int);
	int y_start = RCT2_GLOBAL(RCT2_ADDRESS_SCENARIO_TICKS, int) * 5;
	y_start = -y_start;

	x_start += left;
	y_start += top;

	gfx_draw_rain(left, top, width, height, x_start, y_start);

	x_start = -RCT2_GLOBAL(RCT2_ADDRESS_SCENARIO_TICKS, int) + 0x10;
	y_start = (RCT2_GLOBAL(RCT2_ADDRESS_SCENARIO_TICKS, int) * 6) + 5;
	y_start = -y_start;

	x_start += left;
	y_start += top;

	gfx_draw_rain(left, top, width, height, x_start, y_start);

	x_start = -RCT2_GLOBAL(RCT2_ADDRESS_SCENARIO_TICKS, int) + 8;
	y_start = (RCT2_GLOBAL(RCT2_ADDRESS_SCENARIO_TICKS, int) * 3) + 7;
	y_start = -y_start;

	x_start += left;
	y_start += top;

	gfx_draw_rain(left, top, width, height, x_start, y_start);

	x_start = -RCT2_GLOBAL(RCT2_ADDRESS_SCENARIO_TICKS, int) + 0x18;
	y_start = (RCT2_GLOBAL(RCT2_ADDRESS_SCENARIO_TICKS, int) * 4) + 0x0D;
	y_start = -y_start;

	x_start += left;
	y_start += top;

	gfx_draw_rain(left, top, width, height, x_start, y_start);
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
void update_palette_effects()
{
	if (RCT2_GLOBAL(RCT2_ADDRESS_LIGHTNING_ACTIVE, uint8) == 1) {
		// change palette to lighter color during lightning
		int palette = 1532;
		if (RCT2_GLOBAL(0x009ADAE0, sint32) != -1) {
			palette = RCT2_GLOBAL(RCT2_GLOBAL(0x009ADAE0, int) + 2, int);
		}
		rct_g1_element g1_element = RCT2_ADDRESS(RCT2_ADDRESS_G1_ELEMENTS, rct_g1_element)[palette];
		int xoffset = g1_element.x_offset;
		xoffset = xoffset * 4;
		for (int i = 0; i < g1_element.width; i++) {
			RCT2_ADDRESS(0x01424680 + xoffset, uint8)[(i * 4) + 0] = -((0xFF - g1_element.offset[(i * 3) + 0]) / 2) - 1;
			RCT2_ADDRESS(0x01424680 + xoffset, uint8)[(i * 4) + 1] = -((0xFF - g1_element.offset[(i * 3) + 1]) / 2) - 1;
			RCT2_ADDRESS(0x01424680 + xoffset, uint8)[(i * 4) + 2] = -((0xFF - g1_element.offset[(i * 3) + 2]) / 2) - 1;
		}
		RCT2_GLOBAL(0x014241BC, uint32) = 2;
		osinterface_update_palette(RCT2_ADDRESS(0x01424680, uint8), 10, 236);
		RCT2_GLOBAL(0x014241BC, uint32) = 0;
		RCT2_GLOBAL(RCT2_ADDRESS_LIGHTNING_ACTIVE, uint8)++;
	} else {
		if (RCT2_GLOBAL(RCT2_ADDRESS_LIGHTNING_ACTIVE, uint8) == 2) {
			// change palette back to normal after lightning
			int palette = 1532;
			if (RCT2_GLOBAL(0x009ADAE0, sint32) != -1) {
				palette = RCT2_GLOBAL(RCT2_GLOBAL(0x009ADAE0, int) + 2, int);
			}
			rct_g1_element g1_element = RCT2_ADDRESS(RCT2_ADDRESS_G1_ELEMENTS, rct_g1_element)[palette];
			int xoffset = g1_element.x_offset;
			xoffset = xoffset * 4;
			for (int i = 0; i < g1_element.width; i++) {
				RCT2_ADDRESS(0x01424680 + xoffset, uint8)[(i * 4) + 0] = g1_element.offset[(i * 3) + 0];
				RCT2_ADDRESS(0x01424680 + xoffset, uint8)[(i * 4) + 1] = g1_element.offset[(i * 3) + 1];
				RCT2_ADDRESS(0x01424680 + xoffset, uint8)[(i * 4) + 2] = g1_element.offset[(i * 3) + 2];
			}
		}

		// animate the water/lava/chain movement palette
		int q = 0;
		int weather_colour = RCT2_ADDRESS(0x98195C, uint32)[RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_WEATHER_GLOOM, uint8)];
		if (weather_colour != -1) {
			q = 1;
			if (weather_colour != 0x2000031) {
				q = 2;
			}
		}
		uint32 j = RCT2_GLOBAL(0x009DE584, uint32);
		j = (((uint16)((~j / 2) * 128) * 15) >> 16);
		int p = 1533;
		if (RCT2_GLOBAL(0x009ADAE0, int) != -1) {
			p = RCT2_GLOBAL(RCT2_GLOBAL(0x009ADAE0, int) + 0x6, int);
		}
		rct_g1_element g1_element = RCT2_ADDRESS(RCT2_ADDRESS_G1_ELEMENTS, rct_g1_element)[q + p];
		uint8* vs = &g1_element.offset[j * 3];
		uint8* vd = RCT2_ADDRESS(0x01424A18, uint8);
		int n = 5;
		for (int i = 0; i < n; i++) {
			vd[0] = vs[0];
			vd[1] = vs[1];
			vd[2] = vs[2];
			vs += 9;
			if (vs >= &g1_element.offset[9 * n]) {
				vs -= 9 * n;
			}
			vd += 4;
		}

		p = 1536;
		if (RCT2_GLOBAL(0x009ADAE0, int) != -1) {
			p = RCT2_GLOBAL(RCT2_GLOBAL(0x009ADAE0, int) + 0xA, int);
		}
		g1_element = RCT2_ADDRESS(RCT2_ADDRESS_G1_ELEMENTS, rct_g1_element)[q + p];
		vs = &g1_element.offset[j * 3];
		n = 5;
		for (int i = 0; i < n; i++) {
			vd[0] = vs[0];
			vd[1] = vs[1];
			vd[2] = vs[2];
			vs += 9;
			if (vs >= &g1_element.offset[9 * n]) {
				vs -= 9 * n;
			}
			vd += 4;
		}

		j = ((uint16)(RCT2_GLOBAL(0x009DE584, uint32) * -960) * 3) >> 16;
		p = 1539;
		g1_element = RCT2_ADDRESS(RCT2_ADDRESS_G1_ELEMENTS, rct_g1_element)[q + p];
		vs = &g1_element.offset[j * 3];
		vd += 12;
		n = 3;
		for (int i = 0; i < n; i++) {
			vd[0] = vs[0];
			vd[1] = vs[1];
			vd[2] = vs[2];
			vs += 3;
			if (vs >= &g1_element.offset[3 * n]) {
				vs -= 3 * n;
			}
			vd += 4;
		}

		RCT2_GLOBAL(0x014241BC, uint32) = 2;
		osinterface_update_palette(RCT2_ADDRESS(0x01424680, uint8), 230, 16);
		RCT2_GLOBAL(0x014241BC, uint32) = 0;
		if (RCT2_GLOBAL(RCT2_ADDRESS_LIGHTNING_ACTIVE, uint8) == 2) {
			RCT2_GLOBAL(0x014241BC, uint32) = 2;
			osinterface_update_palette(RCT2_ADDRESS(0x01424680, uint8), 10, 236);
			RCT2_GLOBAL(0x014241BC, uint32) = 0;
			RCT2_GLOBAL(RCT2_ADDRESS_LIGHTNING_ACTIVE, uint8) = 0;
		}
	}
	if (RCT2_GLOBAL(0x009E2C4C, uint32) == 2 || RCT2_GLOBAL(0x009E2C4C, uint32) == 1) {
		if (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_CAP_BPP, uint32) != 8) {
			RCT2_GLOBAL(0x009E2C78, int) = 1;
		}
	}
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
			RCT2_GLOBAL(RCT2_ADDRESS_PICKEDUP_PEEP_Y, sint16), 0
		);
	}

	// Get rain draw function and draw rain
	uint32 draw_rain_func = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_RAIN_LEVEL, uint8);
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

	if (gGameSpeed > 1)
		eax = 1 << (gGameSpeed - 1);

	if (RCT2_GLOBAL(0x009DEA6E, uint8) == 0) {
		for (; eax > 0; eax--) {
			game_logic_update();
			start_title_music(); //RCT2_CALLPROC_EBPSAFE(0x006BD0F8); // play title screen music

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

	update_palette_effects();
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
	RCT2_GLOBAL(RCT2_ADDRESS_SCENARIO_TICKS, sint32)++;
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
	research_update();
	RCT2_CALLPROC_EBPSAFE(0x006B5A2A);	// update ride ratings
	ride_measurements_update();
	RCT2_CALLPROC_EBPSAFE(0x0068AFAD);
	vehicle_sounds_update();//RCT2_CALLPROC_EBPSAFE(0x006BBC6B);	// vehicle and scream sounds
	peep_update_crowd_noise();
	climate_update_sound();
	news_item_update_current();
	RCT2_CALLPROC_EBPSAFE(0x0067009A);	// scenario editor opening of windows for a phase

	stop_completed_sounds(); // removes other sounds that are no longer playing, this is normally called somewhere in rct2_init

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
static GAME_COMMAND_POINTER* new_game_command_table[58];

/**
 * 
 *  rct2: 0x006677F2
 *
 * @param flags (ebx)
 * @param command (esi)
 */
int game_do_command(int eax, int ebx, int ecx, int edx, int esi, int edi, int ebp)
{
	return game_do_command_p(esi, &eax, &ebx, &ecx, &edx, &esi, &edi, &ebp);
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
	if (game_do_command_table[command] == 0) {
		new_game_command_table[command](eax, ebx, ecx, edx, esi, edi, ebp);
	} else {
		RCT2_CALLFUNC_X(game_do_command_table[command], eax, ebx, ecx, edx, esi, edi, ebp);
	}
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
			if (game_do_command_table[command] == 0) {
				new_game_command_table[command](eax, ebx, ecx, edx, esi, edi, ebp);
			} else {
				RCT2_CALLFUNC_X(game_do_command_table[command], eax, ebx, ecx, edx, esi, edi, ebp);
			}
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
				j += object_load_packed(file);
			if (j > 0)
				object_list_load();
		}
	}

	if (!object_read_and_load_entries(file)){
		fclose(file);
		RCT2_GLOBAL(0x009AC31B, uint8) = 255;
		RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_STRING_ID, uint16) = STR_FILE_CONTAINS_INVALID_DATA;
		return 0;
	};

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
	0x00698D6C, // text input
	0x0068C542,
	0x0068C6D1,
	0x0068BC01,
	0x006E66A0,
	0x006E6878,
	0x006C5AE9,
	0, // use new_game_command_table, original: 0x006BEFA1, 29
	0x006C09D1, // 30
	0x006C0B83,
	0x006C0BB5,
	0x00669C6D,
	0x00669D4A,
	0x006649BD,
	0x006666E7,
	0x00666A63,
	0x006CD8CE,
	(uint32)game_command_set_park_entrance_fee,
	(uint32)game_command_update_staff_colour, // 40
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

void game_command_emptysub(int* eax, int* ebx, int* ecx, int* edx, int* esi, int* edi, int* ebp) {}

static GAME_COMMAND_POINTER* new_game_command_table[58] = {
	game_command_emptysub,
	game_command_emptysub,
	game_command_emptysub,
	game_command_emptysub,
	game_command_emptysub,
	game_command_emptysub,
	game_command_emptysub,
	game_command_emptysub,
	game_command_emptysub,
	game_command_emptysub,
	game_command_emptysub, // 10
	game_command_emptysub,
	game_command_emptysub,
	game_command_emptysub,
	game_command_emptysub,
	game_command_emptysub,
	game_command_emptysub,
	game_command_emptysub,
	game_command_emptysub,
	game_command_emptysub,
	game_command_emptysub, // 20
	game_command_emptysub,
	game_command_emptysub,
	game_command_emptysub,
	game_command_emptysub,
	game_command_emptysub,
	game_command_emptysub,
	game_command_emptysub,
	game_command_emptysub,
	game_command_hire_new_staff_member, //game_command_emptysub,
	game_command_emptysub, // 30
	game_command_emptysub,
	game_command_emptysub,
	game_command_emptysub,
	game_command_emptysub,
	game_command_emptysub,
	game_command_emptysub,
	game_command_emptysub,
	game_command_emptysub,
	game_command_emptysub,
	game_command_emptysub, // 40
	game_command_emptysub,
	game_command_emptysub,
	game_command_emptysub,
	game_command_emptysub,
	game_command_emptysub,
	game_command_emptysub,
	game_command_emptysub,
	game_command_emptysub,
	game_command_emptysub,
	game_command_emptysub, // 50
	game_command_emptysub,
	game_command_emptysub,
	game_command_emptysub,
	game_command_emptysub,
	game_command_emptysub,
	game_command_emptysub,
	game_command_emptysub
};

#pragma endregion
