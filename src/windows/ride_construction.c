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
#include "../interface/window.h"
#include "../interface/viewport.h"
#include "../game.h"

/* move to ride.c */
void sub_6b2fa9(rct_windownumber number){
	rct_window* w;

	w = window_find_by_number(WC_RIDE, number);
	if (w){
		if (w->page == 1){
			window_close(w);
		}
	}
}

void window_construction_emptysub(){}
void window_construction_close();

void window_construction_maze_close();

// 0x993F6C
static void* window_construction_maze_events[] = {
	window_construction_maze_close,
	(void*)0x6CD461,
	(void*)0x6CD623,
	(void*)0x6CD48C,
	window_construction_emptysub,
	window_construction_emptysub,
	(void*)0x6CD767,
	window_construction_emptysub,
	window_construction_emptysub,
	(void*)0x6CD63E,
	(void*)0x6CD65D,
	window_construction_emptysub,
	window_construction_emptysub,
	window_construction_emptysub,
	window_construction_emptysub,
	window_construction_emptysub,
	window_construction_emptysub,
	window_construction_emptysub,
	window_construction_emptysub,
	window_construction_emptysub,
	window_construction_emptysub,
	window_construction_emptysub,
	window_construction_emptysub,
	window_construction_emptysub,
	window_construction_emptysub,
	(void*)0x6CD435,
	(void*)0x6CD45B,
	window_construction_emptysub
};

//0x993EEC
static void* window_construction_events[] = {
	window_construction_close,
	(void*)0x6C6E14,
	(void*)0x6C7934,
	(void*)0x6C6E6A,
	window_construction_emptysub,
	window_construction_emptysub,
	(void*)0x6C8374,
	window_construction_emptysub,
	window_construction_emptysub,
	(void*)0x6C8229,
	(void*)0x6C8248,
	window_construction_emptysub,
	window_construction_emptysub,
	window_construction_emptysub,
	window_construction_emptysub,
	window_construction_emptysub,
	window_construction_emptysub,
	window_construction_emptysub,
	window_construction_emptysub,
	window_construction_emptysub,
	window_construction_emptysub,
	window_construction_emptysub,
	window_construction_emptysub,
	window_construction_emptysub,
	window_construction_emptysub,
	(void*)0x6C6AD5,
	(void*)0x6C6B86,
	window_construction_emptysub
};

/**
 * 
 * rct2: 0x006CB481
 */
rct_window *window_construction_open()
{
	int eax, ebx, ecx, edx, esi, edi, ebp;
	
	int ride_id = RCT2_GLOBAL(0xF440A7, uint8);
	sub_6b2fa9(ride_id);

	rct_window *w;
	rct_ride* ride = GET_RIDE(ride_id);
	if (ride->type == RIDE_TYPE_MAZE){
		w = window_create(0, 29, 166, 200, (uint32*)window_construction_maze_events, WC_RIDE_CONSTRUCTION, WF_9);

		w->widgets = (rct_widget*)0x9D7D04;
		w->enabled_widgets = 0x6F0001C4;

		window_init_scroll_widgets(w);

		w->colours[0] = 24;
		w->colours[1] = 24;
		w->colours[2] = 24;

		w->number = ride_id;

		window_push_others_right(w);
		show_gridlines();
		return w;
	}

	w = window_create(0, 29, 166, 394, (uint32*)window_construction_events, WC_RIDE_CONSTRUCTION, WF_9);

	w->widgets = (rct_widget*)0x9D7A90;
	w->enabled_widgets = 0x67EFFFFFC4;

	window_init_scroll_widgets(w);

	w->colours[0] = 24;
	w->colours[1] = 24;
	w->colours[2] = 24;

	w->number = ride_id;

	window_push_others_right(w);
	show_gridlines();

	RCT2_GLOBAL(0xF44070, uint32) = 0x80000000;
	RCT2_GLOBAL(0xF440CD, uint8) = 8;
	RCT2_GLOBAL(0xF440CE, uint8) = 18;
	RCT2_GLOBAL(0xF440CF, uint8) = 4;

	if (ride->type == RIDE_TYPE_REVERSE_FREEFALL_COASTER){
		RCT2_GLOBAL(0xF440CE, uint8) = 30;
	}

	if (ride->type == RIDE_TYPE_AIR_POWERED_VERTICAL_COASTER){
		RCT2_GLOBAL(0xF440CE, uint8) = 30;
	}

	RCT2_GLOBAL(0xF440A0,uint16) = RCT2_ADDRESS(0x0097CC68, uint8)[ride->type * 2] | 0x100;
	RCT2_GLOBAL(0x00F440B2, uint8) = 0;
	RCT2_GLOBAL(0x00F440B3, uint8) = 0;
	RCT2_GLOBAL(0x00F440B4, uint8) = 0;
	RCT2_GLOBAL(0x00F440B5, uint8) = 0;

	if (RCT2_ADDRESS(0x0097D4F2, uint16)[ride->type * 4] & 0x8000)
		RCT2_GLOBAL(0x00F440B5, uint8) |= 2;

	RCT2_GLOBAL(0x00F440B6, uint8) = 0;
	RCT2_GLOBAL(0x00F440B7, uint8) = 0;

	RCT2_GLOBAL(0x00F440AE, uint8) = 0;
	RCT2_GLOBAL(0x00F440A6, uint8) = 4;
	RCT2_GLOBAL(0x00F440B0, uint8) = 0;
	RCT2_GLOBAL(0x00F440B1, uint8) = 0;
	RCT2_GLOBAL(0x00F44159, uint8) = 0;
	RCT2_GLOBAL(0x00F4415C, uint8) = 0;
	return w;

	RCT2_CALLFUNC_X(0x006CB481, &eax, &ebx, &ecx, &edx, &esi, &edi, &ebp);
	return (rct_window*)esi;
}

void window_construction_close(){
	rct_window *w;

	window_get_register(w);

	RCT2_CALLPROC_X(0x006C9627, 0, 0, 0, 0, 0, 0, 0);
	viewport_set_visibility(0);

	map_invalidate_map_selection_tiles();
	RCT2_GLOBAL(0x9DE58A, uint16) &= 0xFFFD;

	hide_gridlines();

	int x, y;
	uint8 ride_id = RCT2_GLOBAL(0xF440A7, uint8);
	rct_map_element* map_element = sub_6CAF80(ride_id, &x, &y);

	if ((int)map_element == -1){
		int eax = RCT2_GLOBAL(0x009DEA6E, uint8);

		RCT2_GLOBAL(0x009DEA6E, uint8) = 0;
		game_do_command(0, 9, 0, ride_id, GAME_COMMAND_7, 0, 0);

		RCT2_GLOBAL(0x009DEA6E, uint8) = eax;
		return;
	}

	window_ride_main_open(ride_id);
}


void window_construction_maze_close(){
	rct_window *w;

	window_get_register(w);

	RCT2_CALLPROC_X(0x006C9627, 0, 0, 0, 0, 0, 0, 0);
	viewport_set_visibility(0);

	map_invalidate_map_selection_tiles();
	RCT2_GLOBAL(0x9DE58A, uint16) &= 0xFFFD;

	hide_gridlines();
	
	uint8 ride_id = RCT2_GLOBAL(0xF440A7, uint8);

	rct_ride* ride = GET_RIDE(ride_id);

	if (ride->overall_view == 0xFFFF){
		int eax = RCT2_GLOBAL(0x009DEA6E, uint8);

		RCT2_GLOBAL(0x009DEA6E, uint8) = 0;
		game_do_command(0, 9, 0, ride_id, GAME_COMMAND_7, 0, 0);

		RCT2_GLOBAL(0x009DEA6E, uint8) = eax;
		return;
	}

	window_ride_main_open(ride_id);
}