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

#include <windows.h>
#include <string.h>
#include <limits.h>
#include "../addresses.h"
#include "../game.h"
#include "../interface/widget.h"
#include "../interface/window.h"
#include "../localisation/localisation.h"
#include "../peep/peep.h"
#include "../ride/ride.h"
#include "../scenario.h"
#include "../sprites.h"
#include "../world/climate.h"
#include "../world/park.h"
#include "../world/sprite.h"

//#define WW 200
//#define WH 128
#define CHEATS_MONEY_INCREMENT 50000
#define CHEATS_TRAM_INCREMENT 1000
enum {
	WINDOW_CHEATS_PAGE_MONEY,
	WINDOW_CHEATS_PAGE_GUESTS,
	WINDOW_CHEATS_PAGE_MISC,
};

enum WINDOW_CHEATS_WIDGET_IDX {
	WIDX_BACKGROUND,
	WIDX_TITLE,
	WIDX_CLOSE,
	WIDX_PAGE_BACKGROUND,
	WIDX_TAB_1,
	WIDX_TAB_2,
	WIDX_TAB_3,
	WIDX_HIGH_MONEY,
	WIDX_PARK_ENTRANCE_FEE,
	WIDX_HAPPY_GUESTS = 7, //Same as HIGH_MONEY as it is also the 7th widget but on a different page
	WIDX_TRAM_GUESTS,
	WIDX_FREEZE_CLIMATE = 7,
	WIDX_OPEN_CLOSE_PARK,
	WIDX_DECREASE_GAME_SPEED,
	WIDX_INCREASE_GAME_SPEED,
	
};

#pragma region MEASUREMENTS
#define WW 240
#define WH 240
#define TAB_HEIGHT 43
#define XSPA 5			//X spacing
#define YSPA 5			//Y spacing
#define XOS 0			+ XSPA	//X offset from left
#define YOS TAB_HEIGHT	+ YSPA	//Y offset ofrom top (includes tabs height)
#define BTNW 110		//button width
#define BTNH 16			//button height
#define YPL(ROW) YOS + ((BTNH + YSPA) * ROW)
#define HPL(ROW) YPL(ROW) + BTNH
#define XPL(COL) XOS + ((BTNW + XSPA) * COL)
#define WPL(COL) XPL(COL) + BTNW

#define TXTO 3	//text horizontal offset from button left (for button text)
#pragma endregion

static rct_widget window_cheats_money_widgets[] = {
	{ WWT_FRAME,			0,	0,			WW - 1,	0,		WH - 1,		0x0FFFFFFFF,	65535},					// panel / background
	{ WWT_CAPTION,			0,	1,			WW - 2,	1,		14,			3165,			STR_WINDOW_TITLE_TIP},	// title bar
	{ WWT_CLOSEBOX,			0,	WW - 13,	WW - 3,	2,		13,			0x338,			STR_CLOSE_WINDOW_TIP},	// close x button
	{ WWT_IMGBTN,			1,	0,			WW - 1,	43,		WH - 1,		0x0FFFFFFFF,	65535},					// tab content panel
	{ WWT_TAB,				1,	3,			33,		17,		43,			0x2000144E,		2462},					// tab 1
	{ WWT_TAB,				1,	34,			64,		17,		43,			0x2000144E,		2462},					// tab 2
	{ WWT_TAB,				1,	65,			95,		17,		43,			0x2000144E,		2462},					// tab 3
	{ WWT_CLOSEBOX,			1,	XPL(0),		WPL(0),	YPL(1),	HPL(1),		2760,			STR_NONE},				// high money
	{ WWT_CLOSEBOX,			1,	XPL(0),		WPL(0),	YPL(3),	HPL(3),		2761,			STR_NONE},				//Park Entrance Fee Toggle	
	{ WIDGETS_END },
};

static rct_widget window_cheats_guests_widgets[] = {
	{ WWT_FRAME,			0, 0,			WW - 1, 0,	WH - 1,		0x0FFFFFFFF,	65535 },					// panel / background
	{ WWT_CAPTION,			0, 1,			WW - 2, 1,	14,			3165,			STR_WINDOW_TITLE_TIP },		// title bar
	{ WWT_CLOSEBOX,			0, WW - 13,		WW - 3, 2,	13,			0x338,			STR_CLOSE_WINDOW_TIP },		// close x button
	{ WWT_IMGBTN,			1, 0,			WW - 1, 43, WH - 1,		0x0FFFFFFFF,	65535 },					// tab content panel
	{ WWT_TAB,				1, 3,			33,		17, 43,			0x2000144E,		2462 },						// tab 1
	{ WWT_TAB,				1, 34,			64,		17, 43,			0x2000144E,		2462 },						// tab 2
	{ WWT_TAB,				1,	65,			95,		17,		43,		0x2000144E,		2462 },						// tab 3
	{ WWT_CLOSEBOX,			1, XPL(0),	WPL(0),	YPL(1), HPL(1),		2764,			STR_NONE},					// happy guests
	{ WWT_CLOSEBOX,			1, XPL(0),	WPL(0),	YPL(3), HPL(3),		2765,			STR_NONE},					// happy guests
	{ WIDGETS_END },
};

//Strings for following moved to window_cheats_paint()
static rct_widget window_cheats_misc_widgets[] = {
	{ WWT_FRAME,			0, 0,			WW - 1, 0,	WH - 1,		0x0FFFFFFFF,	65535 },					// panel / background
	{ WWT_CAPTION,			0, 1,			WW - 2, 1,	14,			3165,			STR_WINDOW_TITLE_TIP },		// title bar
	{ WWT_CLOSEBOX,			0, WW - 13,		WW - 3, 2,	13,			0x338,			STR_CLOSE_WINDOW_TIP },		// close x button
	{ WWT_IMGBTN,			1, 0,			WW - 1, 43, WH - 1,		0x0FFFFFFFF,	65535 },					// tab content panel
	{ WWT_TAB,				1, 3,			33,		17, 43,			0x2000144E,		2462 },						// tab 1
	{ WWT_TAB,				1, 34,			64,		17, 43,			0x2000144E,		2462 },						// tab 2
	{ WWT_TAB,				1,	65,			95,		17,		43,		0x2000144E,		2462},						// tab 3
	{ WWT_CLOSEBOX,			1, XPL(0),	WPL(0),	YPL(0), HPL(0),		2767,			STR_NONE},					// Freeze climate
	{ WWT_CLOSEBOX,			1, XPL(0),	WPL(0),	YPL(1), HPL(1),		2769,			STR_NONE},					// open / close park
	{ WWT_CLOSEBOX,			1, XPL(0),	WPL(0),	YPL(2), HPL(2),		2771,			STR_NONE},					// decrease game speed
	{ WWT_CLOSEBOX,			1, XPL(1),	WPL(1),	YPL(2), HPL(2),		2772,			STR_NONE},					// increase game speed
	{ WIDGETS_END },
};

static rct_widget *window_cheats_page_widgets[] = {
	window_cheats_money_widgets,
	window_cheats_guests_widgets,
	window_cheats_misc_widgets,
};

static void window_cheats_emptysub() { }
static void window_cheats_money_mouseup();
static void window_cheats_guests_mouseup();
static void window_cheats_misc_mouseup();
static void window_cheats_update(rct_window *w);
static void window_cheats_invalidate();
static void window_cheats_paint();
static void window_cheats_set_page(rct_window *w, int page);

static void* window_cheats_money_events[] = {
	window_cheats_emptysub,
	window_cheats_money_mouseup,
	window_cheats_emptysub,
	window_cheats_emptysub,
	window_cheats_emptysub,
	window_cheats_emptysub,
	window_cheats_update,
	window_cheats_emptysub,
	window_cheats_emptysub,
	window_cheats_emptysub,
	window_cheats_emptysub,
	window_cheats_emptysub,
	window_cheats_emptysub,
	window_cheats_emptysub,
	window_cheats_emptysub,
	window_cheats_emptysub,
	window_cheats_emptysub,
	window_cheats_emptysub,
	window_cheats_emptysub,
	window_cheats_emptysub,
	window_cheats_emptysub,
	window_cheats_emptysub,
	window_cheats_emptysub,
	window_cheats_emptysub,
	window_cheats_emptysub,
	window_cheats_invalidate,
	window_cheats_paint,
	window_cheats_emptysub
};

static void* window_cheats_guests_events[] = {
	window_cheats_emptysub,
	window_cheats_guests_mouseup,
	window_cheats_emptysub,
	window_cheats_emptysub,
	window_cheats_emptysub,
	window_cheats_emptysub,
	window_cheats_update,
	window_cheats_emptysub,
	window_cheats_emptysub,
	window_cheats_emptysub,
	window_cheats_emptysub,
	window_cheats_emptysub,
	window_cheats_emptysub,
	window_cheats_emptysub,
	window_cheats_emptysub,
	window_cheats_emptysub,
	window_cheats_emptysub,
	window_cheats_emptysub,
	window_cheats_emptysub,
	window_cheats_emptysub,
	window_cheats_emptysub,
	window_cheats_emptysub,
	window_cheats_emptysub,
	window_cheats_emptysub,
	window_cheats_emptysub,
	window_cheats_invalidate,
	window_cheats_paint,
	window_cheats_emptysub
};

static void* window_cheats_misc_events[] = {
	window_cheats_emptysub,
	window_cheats_misc_mouseup,
	window_cheats_emptysub,
	window_cheats_emptysub,
	window_cheats_emptysub,
	window_cheats_emptysub,
	window_cheats_update,
	window_cheats_emptysub,
	window_cheats_emptysub,
	window_cheats_emptysub,
	window_cheats_emptysub,
	window_cheats_emptysub,
	window_cheats_emptysub,
	window_cheats_emptysub,
	window_cheats_emptysub,
	window_cheats_emptysub,
	window_cheats_emptysub,
	window_cheats_emptysub,
	window_cheats_emptysub,
	window_cheats_emptysub,
	window_cheats_emptysub,
	window_cheats_emptysub,
	window_cheats_emptysub,
	window_cheats_emptysub,
	window_cheats_emptysub,
	window_cheats_invalidate,
	window_cheats_paint,
	window_cheats_emptysub
};

static void* window_cheats_page_events[] = {
	window_cheats_money_events,
	window_cheats_guests_events,
	window_cheats_misc_events,
};

static uint32 window_cheats_page_enabled_widgets[] = {
	(1 << WIDX_CLOSE) | (1 << WIDX_TAB_1) | (1 << WIDX_TAB_2) | (1 << WIDX_TAB_3) | (1 << WIDX_HIGH_MONEY) | (1 << WIDX_PARK_ENTRANCE_FEE),
	(1 << WIDX_CLOSE) | (1 << WIDX_TAB_1) | (1 << WIDX_TAB_2) | (1 << WIDX_TAB_3) | (1 << WIDX_HAPPY_GUESTS) | (1 << WIDX_TRAM_GUESTS),
	(1 << WIDX_CLOSE) | (1 << WIDX_TAB_1) | (1 << WIDX_TAB_2) | (1 << WIDX_TAB_3) | (1 << WIDX_FREEZE_CLIMATE) | (1 << WIDX_OPEN_CLOSE_PARK) | (1 << WIDX_DECREASE_GAME_SPEED) | (1 << WIDX_INCREASE_GAME_SPEED),
};

static void window_cheats_draw_tab_images(rct_drawpixelinfo *dpi, rct_window *w);

void window_cheats_open()
{
	rct_window* window;

	// Check if window is already open
	window = window_bring_to_front_by_class(WC_CHEATS);
	if (window != NULL)
		return;

	window = window_create(32, 32, WW, WH, (uint32*)window_cheats_money_events, WC_CHEATS, 0);
	window->widgets = window_cheats_money_widgets;
	window->enabled_widgets = window_cheats_page_enabled_widgets[0];
	window_init_scroll_widgets(window);
	window->page = WINDOW_CHEATS_PAGE_MONEY;
	window->colours[0] = 1;
	window->colours[1] = 19;
	window->colours[2] = 19;
}

static void window_cheats_money_mouseup()
{
	int i;
	short widgetIndex;
	rct_window *w;

	window_widget_get_registers(w, widgetIndex);

	switch (widgetIndex) {
	case WIDX_CLOSE:
		window_close(w);
		break;
	case WIDX_TAB_1:
	case WIDX_TAB_2:
	case WIDX_TAB_3:
		window_cheats_set_page(w, widgetIndex - WIDX_TAB_1);
		break;
	case WIDX_HIGH_MONEY:
		i = DECRYPT_MONEY(RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_MONEY_ENCRYPTED, sint32));

		if (i < INT_MAX - CHEATS_MONEY_INCREMENT) {
			i += CHEATS_MONEY_INCREMENT;
		}
		else {
			i = INT_MAX;
		}
		RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_MONEY_ENCRYPTED, sint32) = ENCRYPT_MONEY(i);
		window_invalidate_by_class(WC_BOTTOM_TOOLBAR);
		break;
	case WIDX_PARK_ENTRANCE_FEE:
		RCT2_GLOBAL(0x13573E5, uint32) ^= 0x020;
		if (!(RCT2_GLOBAL(0x13573E5, uint32) & 0x020) ) w->widgets[widgetIndex].image = 2762;
		else w->widgets[widgetIndex].image = 2761;
		window_invalidate_by_class(WC_PARK_INFORMATION);
		break;
	}
}

static void window_cheats_guests_mouseup()
{
	short widgetIndex;
	rct_window *w;
	int i;

	window_widget_get_registers(w, widgetIndex);

	rct_peep* peep;
	uint16 spriteIndex;

	switch (widgetIndex) {
	case WIDX_CLOSE:
		window_close(w);
		break;
	case WIDX_TAB_1:
	case WIDX_TAB_2:
	case WIDX_TAB_3:
		window_cheats_set_page(w, widgetIndex - WIDX_TAB_1);
		break;
	case WIDX_HAPPY_GUESTS:
		FOR_ALL_GUESTS(spriteIndex, peep)
			if (peep->var_2A == 0)
				peep->happiness = 255;
		window_invalidate_by_class(WC_BOTTOM_TOOLBAR);
		break;
	case WIDX_TRAM_GUESTS:
		for (i = 0; i < CHEATS_TRAM_INCREMENT; i++){
			generate_new_guest();
		}
		window_invalidate_by_class(WC_BOTTOM_TOOLBAR);
		break;
	}
}

static void window_cheats_misc_mouseup()
{
	short widgetIndex;
	rct_window *w;

	window_widget_get_registers(w, widgetIndex);

	switch (widgetIndex) {
	case WIDX_CLOSE:
		window_close(w);
		break;
	case WIDX_TAB_1:
	case WIDX_TAB_2:
	case WIDX_TAB_3:
		window_cheats_set_page(w, widgetIndex - WIDX_TAB_1);
		break;
	case WIDX_FREEZE_CLIMATE:
		toggle_climate_lock();
		w->widgets[widgetIndex].image = w->widgets[widgetIndex].image == 2767 ? 2768 : 2767;
		break;
	case WIDX_OPEN_CLOSE_PARK:
		game_do_command(0, 1, 0, park_is_open() ? 0 : 0x101, GAME_COMMAND_SET_PARK_OPEN, 0, 0);
		w->widgets[widgetIndex].image = w->widgets[widgetIndex].image == 2769 ? 2770 : 2769;
		window_invalidate_by_class(WC_PARK_INFORMATION);
		break;
	case WIDX_DECREASE_GAME_SPEED:
		game_reduce_game_speed();
		break;
	case WIDX_INCREASE_GAME_SPEED:
		game_increase_game_speed();
		break;

	}
}

static void window_cheats_update(rct_window *w)
{
	w->frame_no++;
	widget_invalidate(w, WIDX_TAB_1 + w->page);
}

static void window_cheats_invalidate()
{
	int i;
	rct_window *w;

	window_get_register(w);

	strcpy((char*)0x009BC677, "Cheats");

	rct_widget *widgets = window_cheats_page_widgets[w->page];
	if (w->widgets != widgets) {
		w->widgets = widgets;
		window_init_scroll_widgets(w);
	}

	// Set correct active tab
	for (i = 0; i < 7; i++)
		w->pressed_widgets &= ~(1 << (WIDX_TAB_1 + i));
	w->pressed_widgets |= 1LL << (WIDX_TAB_1 + w->page);
}

static void window_cheats_paint()
{
	rct_window *w;
	rct_drawpixelinfo *dpi;

	window_paint_get_registers(w, dpi);

	window_draw_widgets(w, dpi);
	window_cheats_draw_tab_images(dpi, w);

	if (w->page == WINDOW_CHEATS_PAGE_MONEY){
		char buffer[256];
		// Format text
		sprintf(buffer, "%c%c%s", FORMAT_MEDIUMFONT, FORMAT_BLACK, "Increases your money by 5,000.");
		// Draw shadow
		gfx_draw_string(dpi, buffer, 0, w->x + XPL(0) + TXTO, w->y + YPL(0) + TXTO);
		
		sprintf(buffer, "%c%c%s", FORMAT_MEDIUMFONT, FORMAT_BLACK, "Toggle between Free and Paid Entry");
		// Draw shadow
		gfx_draw_string(dpi, buffer, 0, w->x + XPL(0) + TXTO, w->y + YPL(2) + TXTO);
	}
	else if (w->page == WINDOW_CHEATS_PAGE_GUESTS){
		char buffer[256];
		// Format text
		sprintf(buffer, "%c%c%s", FORMAT_MEDIUMFONT, FORMAT_BLACK, "Increases every peeps happiness to max.");
		// Draw shadow
		gfx_draw_string(dpi, buffer, 0, w->x + XPL(0) + TXTO, w->y + YPL(0) + TXTO);

		sprintf(buffer, "%c%c%s", FORMAT_MEDIUMFONT, FORMAT_BLACK, "Large group of peeps arrive");
		gfx_draw_string(dpi, buffer, 0, w->x + XPL(0) + TXTO, w->y + YPL(2) + TXTO);
	}

}

static void window_cheats_draw_tab_images(rct_drawpixelinfo *dpi, rct_window *w)
{
	int sprite_idx;

	// Money tab
	if (!(w->disabled_widgets & (1 << WIDX_TAB_1))) {
		sprite_idx = 5261;
		if (w->page == WINDOW_CHEATS_PAGE_MONEY)
			sprite_idx += (w->frame_no / 2) % 8;
		gfx_draw_sprite(dpi, sprite_idx, w->x + w->widgets[WIDX_TAB_1].left, w->y + w->widgets[WIDX_TAB_1].top, 0);
	}
	
	// Guests tab
	if (!(w->disabled_widgets & (1 << WIDX_TAB_2))) {
		sprite_idx = 5568;
		if (w->page == WINDOW_CHEATS_PAGE_GUESTS)
			sprite_idx += (w->frame_no / 3) % 8;
		gfx_draw_sprite(dpi, sprite_idx, w->x + w->widgets[WIDX_TAB_2].left, w->y + w->widgets[WIDX_TAB_2].top, 0);
	}

	// Misc tab
	if (!(w->disabled_widgets & (1 << WIDX_TAB_3))) {
		sprite_idx = SPR_TAB_QUESTION;
		gfx_draw_sprite(dpi, sprite_idx, w->x + w->widgets[WIDX_TAB_3].left, w->y + w->widgets[WIDX_TAB_3].top, 0);
	}
}

static void window_cheats_set_page(rct_window *w, int page)
{
	w->page = page;
	
	w->enabled_widgets = window_cheats_page_enabled_widgets[page];
	
	w->event_handlers = window_cheats_page_events[page];
	w->widgets = window_cheats_page_widgets[page];

	window_invalidate(w);
}
