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
#include "../ride/track.h"
#include "../drawing/drawing.h"
#include "../interface/themes.h"

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

enum {
	WIDX_BACKGROUND,
	WIDX_TITLE,
	WIDX_CLOSE,
	WIDX_PAGE_BACKGROUND,

	WIDX_DEMOLISH = 24
};

void window_construction_emptysub(){}
void window_construction_close();
void window_construction_mouseup();
void window_construction_paint();

void window_construction_maze_close();
void window_construction_maze_invalidate();
void window_construction_maze_paint();

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
	window_construction_maze_invalidate,
	window_construction_maze_paint,
	window_construction_emptysub
};

//0x993EEC
static void* window_construction_events[] = {
	window_construction_close,
	window_construction_mouseup,
	(void*)0x6C7934,
	(void*)0x6C6E6A,
	(void*)0x6C78CD,
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
	window_construction_paint,//(void*)0x6C6B86,
	window_construction_emptysub
};

/**
 *
 * rct2: 0x006CB481
 */
rct_window *window_construction_open()
{
	int ride_id = RCT2_GLOBAL(0xF440A7, uint8);
	sub_6b2fa9(ride_id);

	rct_window *w;
	rct_ride* ride = GET_RIDE(ride_id);
	if (ride->type == RIDE_TYPE_MAZE){
		w = window_create(0, 29, 166, 200, (uint32*)window_construction_maze_events, WC_RIDE_CONSTRUCTION, WF_9);

		w->widgets = (rct_widget*)0x9D7D04;
		w->enabled_widgets = 0x6F0001C4;

		window_init_scroll_widgets(w);

		colour_scheme_update(w);

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

	RCT2_GLOBAL(0xF44070, uint32) = MONEY32_UNDEFINED;
	RCT2_GLOBAL(0xF440CD, uint8) = 8;
	RCT2_GLOBAL(0xF440CE, uint8) = 18;
	RCT2_GLOBAL(0xF440CF, uint8) = 4;

	if (ride->type == RIDE_TYPE_REVERSE_FREEFALL_COASTER){
		RCT2_GLOBAL(0xF440CE, uint8) = 30;
	}

	if (ride->type == RIDE_TYPE_AIR_POWERED_VERTICAL_COASTER){
		RCT2_GLOBAL(0xF440CE, uint8) = 30;
	}

	RCT2_GLOBAL(0xF440A0, uint16) = RCT2_ADDRESS(0x0097CC68, uint8)[ride->type * 2] | 0x100;
	RCT2_GLOBAL(0x00F440B2, uint8) = 0;
	RCT2_GLOBAL(0x00F440B3, uint8) = 0;
	RCT2_GLOBAL(0x00F440B4, uint8) = 0;
	RCT2_GLOBAL(0x00F440B5, uint8) = 0;

	if (RCT2_ADDRESS(0x0097D4F2, uint16)[ride->type * 4] & 0x8000)
		RCT2_GLOBAL(0x00F440B5, uint8) |= 2;

	RCT2_GLOBAL(0x00F440B6, uint8) = 0;
	RCT2_GLOBAL(0x00F440B7, uint8) = 0;

	RCT2_GLOBAL(RCT2_ADDRESS_TRACK_PREVIEW_ROTATION, uint8) = 0;
	RCT2_GLOBAL(0x00F440A6, uint8) = 4;
	RCT2_GLOBAL(0x00F440B0, uint8) = 0;
	RCT2_GLOBAL(0x00F440B1, uint8) = 0;
	RCT2_GLOBAL(0x00F44159, uint8) = 0;
	RCT2_GLOBAL(0x00F4415C, uint8) = 0;
	colour_scheme_update(w);
	return w;
}

/* rct2: 0x006C845D */
void window_construction_close()
{
	rct_window *w;
	rct_xy_element mapElement;

	window_get_register(w);

	sub_6C9627();
	viewport_set_visibility(0);

	map_invalidate_map_selection_tiles();
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) &= ~(1 << 1);

	// In order to cancel the yellow arrow correctly the
	// selection tool should be cancelled.
	tool_cancel();

	hide_gridlines();

	uint8 rideIndex = RCT2_GLOBAL(0x00F440A7, uint8);
	if (!sub_6CAF80(rideIndex, &mapElement)) {
		int eax = RCT2_GLOBAL(RCT2_ADDRESS_GAME_PAUSED, uint8);

		RCT2_GLOBAL(RCT2_ADDRESS_GAME_PAUSED, uint8) = 0;
		game_do_command(0, 9, 0, rideIndex, GAME_COMMAND_7, 0, 0);

		RCT2_GLOBAL(RCT2_ADDRESS_GAME_PAUSED, uint8) = eax;
		return;
	}

	window_ride_main_open(rideIndex);
}


void window_construction_maze_close(){
	rct_window *w;

	window_get_register(w);

	sub_6C9627();
	viewport_set_visibility(0);

	map_invalidate_map_selection_tiles();
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) &= ~(1 << 1);

	// In order to cancel the yellow arrow correctly the
	// selection tool should be cancelled.
	tool_cancel();

	hide_gridlines();

	uint8 ride_id = RCT2_GLOBAL(0xF440A7, uint8);

	rct_ride* ride = GET_RIDE(ride_id);

	if (ride->overall_view == 0xFFFF){
		int eax = RCT2_GLOBAL(RCT2_ADDRESS_GAME_PAUSED, uint8);

		RCT2_GLOBAL(RCT2_ADDRESS_GAME_PAUSED, uint8) = 0;
		game_do_command(0, 9, 0, ride_id, GAME_COMMAND_7, 0, 0);

		RCT2_GLOBAL(RCT2_ADDRESS_GAME_PAUSED, uint8) = eax;
		return;
	}

	window_ride_main_open(ride_id);
}

void window_construction_mouseup_demolish(rct_window* w);

/* rct2: 0x006C6E14 */
void window_construction_mouseup(){
	short widgetIndex;
	rct_window *w;

	window_widget_get_registers(w, widgetIndex);

	RCT2_CALLPROC_X(0x6C6A77, 0, 0, 0, 0, 0, 0, 0);

	switch (widgetIndex){
	case WIDX_CLOSE:
		window_close(w);
		break;
	case 27:
		RCT2_CALLPROC_X(0x6C9296, 0, 0, 0, widgetIndex, (int)w, 0, 0);
		break;
	case 26:
		RCT2_CALLPROC_X(0x6C93B8, 0, 0, 0, widgetIndex, (int)w, 0, 0);
		break;
	case 23:
		RCT2_CALLPROC_X(0x6C9F72, 0, 0, 0, widgetIndex, (int)w, 0, 0);
		break;
	case WIDX_DEMOLISH:
		window_construction_mouseup_demolish(w);
		break;
	case 32:
		RCT2_CALLPROC_X(0x6C78AA, 0, 0, 0, widgetIndex, (int)w, 0, 0);
		break;
	case 29:
		RCT2_CALLPROC_X(0x6C7802, 0, 0, 0, widgetIndex, (int)w, 0, 0);
		break;
	case 30:
		RCT2_CALLPROC_X(0x6C7866, 0, 0, 0, widgetIndex, (int)w, 0, 0);
		break;
	}
}

/* rct2: 0x006C9BA5 */
void window_construction_mouseup_demolish(rct_window* w){
	RCT2_CALLPROC_X(0x6C9BA5, 0, 0, 0, 0, (int)w, 0, 0);
	return;

	RCT2_GLOBAL(0xF44070, uint32) = MONEY32_UNDEFINED;
	sub_6C9627();

	RCT2_GLOBAL(0xF440B8, uint8) = 3;
	if (RCT2_GLOBAL(0xF440A6, uint8) == 1){
		//6C9C4F
	}

	if (RCT2_GLOBAL(0xF440A6, uint8) != 2){
		//6c9cc4
		int eax = RCT2_GLOBAL(0xF440A8, uint16),
			ebx = RCT2_GLOBAL(0xF440AF, uint8) || (RCT2_GLOBAL(RCT2_ADDRESS_TRACK_PREVIEW_ROTATION, uint8) << 8),
			ecx = RCT2_GLOBAL(0xF440AA, uint16),
			edx = RCT2_GLOBAL(0xF440AC, uint16);

		sub_6C683D(&eax, &ecx, &edx, RCT2_GLOBAL(RCT2_ADDRESS_TRACK_PREVIEW_ROTATION, uint8), RCT2_GLOBAL(0xF440AF, uint8) & 0x3FF, 0, 0, 0);
	}

	int ride_id = RCT2_GLOBAL(0xF440A7, uint8);
	RCT2_GLOBAL(0xF441D2, uint8) = ride_id;
	//6c9BFE
}

void window_construction_maze_invalidate()
{
	int ride_idx = RCT2_GLOBAL(0x00F440A7, uint8);
	RCT2_GLOBAL(0x13CE958, uint32_t) = RCT2_GLOBAL(0x1362944 + ride_idx * 0x260, uint32_t);
	RCT2_GLOBAL(0x13CE956, uint16_t) = RCT2_GLOBAL(0x1362942 + ride_idx * 0x260, uint16_t);
}

//0x6C6B86
void window_construction_paint()
{
	rct_window *w;
	rct_drawpixelinfo *dpi;
	window_paint_get_registers(w, dpi);
	window_draw_widgets(w, dpi);
	if (RCT2_GLOBAL(0x9D7C00, uint8_t) == 0) return;
	uint32_t eax = 0, esi = (uint32_t)w, ebp = 0;//nothing
	uint32_t ebx = 0, ecx = 0, edx = 0, edi = (uint32_t)dpi;//returns
	if (RCT2_CALLFUNC_X(0x6CA2DF, &eax, &ebx, &ecx, &edx, &esi, &edi, &ebp) & 0x100) return;
	RCT2_GLOBAL(0xF44133, uint8_t) = edx & 0xFF;
	RCT2_GLOBAL(0xF44134, uint8_t) = (ebx >> 8) & 0xFF;
	RCT2_GLOBAL(0xF44135, uint8_t) = (edx >> 8) & 0xFF;
	edx >>= 16;
	RCT2_GLOBAL(0xF44136, int16_t) = edx;
	rct_ride* ride = GET_RIDE(RCT2_GLOBAL(0xF44133, uint8));
	RCT2_GLOBAL(0xF44064, uint32_t) = RCT2_GLOBAL(RCT2_ADDRESS_RIDE_FLAGS + ride->type * 8, uint32_t);
	// 0x009D7C04 is a widget address remember to change when widget implemented 
	short width = RCT2_GLOBAL(0x9D7C04, int16_t) - RCT2_GLOBAL(0x9D7C02, int16_t) - 1;
	short height = RCT2_GLOBAL(0x9D7C08, int16_t) - RCT2_GLOBAL(0x9D7C06, int16_t) - 1;
	rct_drawpixelinfo* clip_dpi = clip_drawpixelinfo(
		dpi,
		// 0x009D7C02 is a widget address remember to change when widget implemented
		w->x + RCT2_GLOBAL(0x9D7C02, int16_t) + 1,
		width,
		w->y + RCT2_GLOBAL(0x9D7C06, int16_t) + 1,
		height);
	if (clip_dpi != NULL)
	{
		rct_preview_track *trackBlock;
		ecx = RCT2_GLOBAL(0xF44135, uint8_t);
		if (RCT2_GLOBAL(0xF44064, uint32_t) & 0x80000) trackBlock = RCT2_ADDRESS(0x994A38, rct_preview_track*)[ecx];//RCT2_GLOBAL(0x994A38 + ecx * 4, rct_preview_track*);
		else trackBlock = RCT2_ADDRESS(0x994638, rct_preview_track*)[ecx];//RCT2_GLOBAL(0x994638 + ecx * 4, rct_preview_track*);
		while ((trackBlock + 1)->var_00 != 0xFF) trackBlock++;
		short x = trackBlock->x;
		short z = trackBlock->z;
		short y = trackBlock->y;
		if (trackBlock->var_09 & 2) x = y = 0;
		short tmp;
		switch (RCT2_GLOBAL(0xF44134, uint8_t) & 3)
		{
		case 1:
			tmp = x;
			x = y;
			y = -tmp;
			break;
		case 2:
			x = -x;
			y = -y;
			break;
		case 3:
			tmp = x;
			x = -y;
			y = tmp;
			break;
		case 0:
			break;
		}
		//this is actually case 0, but the other cases all jump to it
		x /= 2;
		y /= 2;
		x += 4112;
		y += 4112;
		z += 1024;
		ebx = RCT2_GLOBAL(0xF44135, uint8_t);
		short bx;
		if (RCT2_GLOBAL(0xF44064, uint32_t) & 0x80000) bx = RCT2_GLOBAL(0x9984A2 + ebx * 8, uint8_t);
		else bx = RCT2_GLOBAL(0x997CA2 + ebx * 8, uint8_t);
		z -= bx;
		int start_x = x;
		switch (RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_ROTATION, uint32_t))
		{
		case 0:
			x = y - x;
			y = (y + start_x) / 2 - z;
			break;
		case 1:
			x = -x - y;
			y = (y - start_x) / 2 - z;
			break;
		case 2:
			x -= y;
			y = (-y - start_x) / 2 - z;
			break;
		case 3:
			x += y;
			y = (-y + start_x) / 2 - z;
			break;
		}
		clip_dpi->x += x - width / 2;
		clip_dpi->y += y - height / 2 - 16;
		RCT2_GLOBAL(0x140E9A8, rct_drawpixelinfo*) = clip_dpi;
		uint32_t d = RCT2_GLOBAL(0xF44136, int16_t) << 16;
		d |= RCT2_GLOBAL(0xF44133, uint8_t);// Ride id
		d |= RCT2_GLOBAL(0xF44135, uint8_t) << 8;
		RCT2_CALLPROC_X(0x6CBCE2, 0x1000, (((uint16_t)bx) & 0xFF) | (RCT2_GLOBAL(0xF44134, uint8_t) << 8), 0x1000, d, width, 0x400, height);
		rct2_free(clip_dpi);
	}
	short string_x = (RCT2_GLOBAL(0x9D7C02, int16_t) + RCT2_GLOBAL(0x9D7C04, int16_t)) / 2 + w->x;
	short string_y = RCT2_GLOBAL(0x9D7C08, int16_t) + w->y - 23;
	if (RCT2_GLOBAL(0xF440A6, uint8_t) != 4) gfx_draw_string_centred(dpi, 1407, string_x, string_y, 0, w);
	string_y += 11;
	if (RCT2_GLOBAL(0xF44070, uint32_t) != MONEY32_UNDEFINED && !(RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32_t) & 0x800))
		gfx_draw_string_centred(dpi, 1408, string_x, string_y, 0, (void*)0xF44070);
}

//0x006CD45B
void window_construction_maze_paint()
{
	rct_window *w;
	rct_drawpixelinfo *dpi;
	window_paint_get_registers(w, dpi);
	window_draw_widgets(w, dpi);
}