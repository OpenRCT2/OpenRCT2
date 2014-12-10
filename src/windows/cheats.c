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
#define CHEATS_MONEY_INCREMENT MONEY(5000,00)
#define CHEATS_TRAM_INCREMENT 250
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
	WIDX_ZERO_CLEARANCE,
	WIDX_WEATHER_SUN,
	WIDX_WEATHER_THUNDER,
	WIDX_CLEAR_GRASS,
	WIDX_MOWED_GRASS,
	WIDX_WATER_PLANTS,
	WIDX_FIX_VANDALISM,
	WIDX_REMOVE_LITTER
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
	{ WWT_CLOSEBOX,			1, XPL(1),	WPL(1),	YPL(3), HPL(3),		2759,			STR_NONE},					// Zero Clearance
	{ WWT_CLOSEBOX,			1, XPL(0),	WPL(0),	YPL(4), HPL(4),		2757,			STR_NONE},					// Sun
	{ WWT_CLOSEBOX,			1, XPL(1),	WPL(1),	YPL(4), HPL(4),		2758,			STR_NONE},					// Thunder

	{ WWT_CLOSEBOX,			1, XPL(0),	WPL(0),	YPL(5), HPL(5),		2752,			STR_NONE},					// Clear grass
	{ WWT_CLOSEBOX,			1, XPL(1),	WPL(1),	YPL(5), HPL(5),		2753,			STR_NONE},					// Mowed grass
	{ WWT_CLOSEBOX,			1, XPL(0),	WPL(0),	YPL(6), HPL(6),		2754,			STR_NONE},					// Water plants
	{ WWT_CLOSEBOX,			1, XPL(1),	WPL(1),	YPL(6), HPL(6),		2755,			STR_NONE},					// Fix vandalism
	{ WWT_CLOSEBOX,			1, XPL(1),	WPL(1),	YPL(7), HPL(7),		2756,			STR_NONE},					// Remove litter
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
void window_cheats_misc_tool_update();
void window_cheats_misc_tool_down();
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
	window_cheats_misc_tool_update,
	window_cheats_misc_tool_down,
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
	(1 << WIDX_CLOSE) | (1 << WIDX_TAB_1) | (1 << WIDX_TAB_2) | (1 << WIDX_TAB_3) | (1 << WIDX_FREEZE_CLIMATE) | (1 << WIDX_OPEN_CLOSE_PARK) | (1 << WIDX_DECREASE_GAME_SPEED) | (1 << WIDX_INCREASE_GAME_SPEED) | (1 << WIDX_ZERO_CLEARANCE) | (1 << WIDX_WEATHER_SUN) | (1 << WIDX_WEATHER_THUNDER) | (1 << WIDX_CLEAR_GRASS) | (1 << WIDX_MOWED_GRASS) | (1 << WIDX_WATER_PLANTS) | (1 << WIDX_FIX_VANDALISM) | (1 << WIDX_REMOVE_LITTER)
};

static void window_cheats_draw_tab_images(rct_drawpixelinfo *dpi, rct_window *w);

#pragma region Cheat functions

static void cheat_set_grass_length(int length)
{
	int x, y;
	rct_map_element *mapElement;

	for (y = 0; y < 256; y++) {
		for (x = 0; x < 256; x++) {
			mapElement = map_get_surface_element_at(x, y);
			if (!(mapElement->properties.surface.ownership & OWNERSHIP_OWNED))
				continue;

			if (map_element_get_terrain(mapElement) != TERRAIN_GRASS)
				continue;

			if ((mapElement->properties.surface.terrain & 0x1F) > 0)
				continue;

			mapElement->properties.surface.grass_length = length;
		}
	}

	gfx_invalidate_screen();
}

static void cheat_water_plants()
{
	int x, y;
	rct_map_element *mapElement;

	for (y = 0; y < 256; y++) {
		for (x = 0; x < 256; x++) {
			mapElement = TILE_MAP_ELEMENT_POINTER(y * 256 + x);
			do {
				if ((mapElement->type & MAP_ELEMENT_TYPE_MASK) == MAP_ELEMENT_TYPE_SCENERY) {
					mapElement->properties.scenery.age = 0;
				}
			} while (!((mapElement++)->flags & MAP_ELEMENT_FLAG_LAST_TILE));
		}
	}

	gfx_invalidate_screen();
}

static void cheat_fix_vandalism()
{
	int x, y;
	rct_map_element *mapElement;

	for (y = 0; y < 256; y++) {
		for (x = 0; x < 256; x++) {
			mapElement = TILE_MAP_ELEMENT_POINTER(y * 256 + x);
			do {
				if ((mapElement->type & MAP_ELEMENT_TYPE_MASK) != MAP_ELEMENT_TYPE_PATH)
					continue;

				if ((mapElement->properties.path.additions & 0x0F) == 0)
					continue;

				mapElement->flags &= ~MAP_ELEMENT_FLAG_BROKEN;
			} while (!((mapElement++)->flags & MAP_ELEMENT_FLAG_LAST_TILE));
		}
	}

	gfx_invalidate_screen();
}

static void cheat_remove_litter()
{
	rct_litter* litter;
	uint16 spriteIndex, nextSpriteIndex;

	for (spriteIndex = RCT2_GLOBAL(RCT2_ADDRESS_SPRITES_START_LITTER, uint16); spriteIndex != SPRITE_INDEX_NULL; spriteIndex = nextSpriteIndex) {
		litter = &(g_sprite_list[spriteIndex].litter);
		nextSpriteIndex = litter->next;
		sprite_remove((rct_sprite*)litter);
	}
}

static void cheat_fix_rides()
{
	int i;
	rct_ride *ride;

	// TODO doesn't quite work, probably need to call the correct fix ride function
	FOR_ALL_RIDES(i, ride) {
		if (ride->lifecycle_flags & (RIDE_LIFECYCLE_BREAKDOWN_PENDING | RIDE_LIFECYCLE_BROKEN_DOWN)) {
			ride->lifecycle_flags &= ~(RIDE_LIFECYCLE_BREAKDOWN_PENDING | RIDE_LIFECYCLE_BROKEN_DOWN);
		}
	}
}

static void cheat_clear_loan()
{
	// TODO, this sets the loan but stops loan borrowing from working, possible due to cheat detection stuff
	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_LOAN, money32) = MONEY(0,00);
	window_invalidate_by_class(WC_FINANCES);
}

static void cheat_increase_money(money32 amount)
{
	money32 currentMoney;

	currentMoney = DECRYPT_MONEY(RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_MONEY_ENCRYPTED, sint32));
	if (currentMoney < INT_MAX - amount)
		currentMoney += amount;
	else
		currentMoney = INT_MAX;
	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_MONEY_ENCRYPTED, sint32) = ENCRYPT_MONEY(currentMoney);

	window_invalidate_by_class(WC_FINANCES);
	window_invalidate_by_class(WC_BOTTOM_TOOLBAR);
}

static void cheat_generate_guests(int count)
{
	int i;

	for (i = 0; i < count; i++)
		generate_new_guest();

	window_invalidate_by_class(WC_BOTTOM_TOOLBAR);
}

static void cheat_make_guests_happy()
{
	int spriteIndex;
	rct_peep *peep;

	FOR_ALL_GUESTS(spriteIndex, peep)
		if (peep->var_2A == 0)
			peep->happiness = 255;
}

#pragma endregion

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
		cheat_increase_money(CHEATS_MONEY_INCREMENT);
		break;
	case WIDX_PARK_ENTRANCE_FEE:
		RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) ^= PARK_FLAGS_PARK_FREE_ENTRY;
		window_invalidate_by_class(WC_PARK_INFORMATION);
		break;
	}
}

static void window_cheats_guests_mouseup()
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
	case WIDX_HAPPY_GUESTS:
		cheat_make_guests_happy();
		break;
	case WIDX_TRAM_GUESTS:
		cheat_generate_guests(CHEATS_TRAM_INCREMENT);
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
		window_invalidate_by_class(WC_PARK_INFORMATION);
		break;
	case WIDX_DECREASE_GAME_SPEED:
		game_reduce_game_speed();
		break;
	case WIDX_INCREASE_GAME_SPEED:
		game_increase_game_speed();
		break;
	case WIDX_ZERO_CLEARANCE:
		if (tool_set(w, widgetIndex, 7)) {
			return;
		}
		break;
	case WIDX_WEATHER_SUN:
		climate_force_weather(WEATHER_SUNNY);
		break;
	case WIDX_WEATHER_THUNDER:
		climate_force_weather(WEATHER_THUNDER);
		break;
	case WIDX_CLEAR_GRASS:
		cheat_set_grass_length(GRASS_LENGTH_CLEAR_0);
		break;
	case WIDX_MOWED_GRASS:
		cheat_set_grass_length(GRASS_LENGTH_MOWED);
		break;
	case WIDX_WATER_PLANTS:
		cheat_water_plants();
		break;
	case WIDX_FIX_VANDALISM:
		cheat_fix_vandalism();
		break;
	case WIDX_REMOVE_LITTER:
		cheat_remove_litter();
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

	switch (w->page) {
	case WINDOW_CHEATS_PAGE_MONEY:
		w->widgets[WIDX_PARK_ENTRANCE_FEE].image = RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & PARK_FLAGS_PARK_FREE_ENTRY ?
			2761 : 2762;
		break;
	case WINDOW_CHEATS_PAGE_MISC:
		w->widgets[WIDX_OPEN_CLOSE_PARK].image = RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & PARK_FLAGS_PARK_OPEN ?
			2770 : 2769;
		break;
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

void window_cheats_misc_tool_update(){
	short widgetIndex;
	rct_window* w;
	short x, y;

	window_tool_get_registers(w, widgetIndex, x, y);

	if (widgetIndex != WIDX_ZERO_CLEARANCE) return;

	RCT2_CALLPROC_X(0x0068AAE1, x, y, 0, 0, (int)w, 0, 0);

	RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) &= ~(1 << 0);
	int temp_y = y + 16;

	int eax = x, ecx = 0, edx = widgetIndex, edi = 0, esi = (int)w, ebp = 0;
	RCT2_CALLFUNC_X(0x689726, &eax, &temp_y, &ecx, &edx, &esi, &edi, &ebp);
	if (eax != 0x8000){
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) |= 1;
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_TYPE, uint16) = 4;
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_X, uint16) = eax;
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_X, uint16) = eax;
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_Y, uint16) = temp_y;
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_Y, uint16) = temp_y;
		RCT2_CALLPROC_X(0x0068AAE1, eax, temp_y, 0, 0, (int)w, 0, 0);
	}

	RCT2_GLOBAL(RCT2_ADDRESS_PICKEDUP_PEEP_SPRITE, sint32) = -1;
}

void window_cheats_misc_tool_down(){
	short widgetIndex;
	rct_window* w;
	short x, y;

	window_tool_get_registers(w, widgetIndex, x, y);

	if (widgetIndex != WIDX_ZERO_CLEARANCE) return;

	int dest_x = x, dest_y = y, ecx = 0, edx = widgetIndex, edi = 0, esi = (int)w, ebp = 0;
	dest_y += 16;
	RCT2_CALLFUNC_X(0x689726, &dest_x, &dest_y, &ecx, &edx, &esi, &edi, &ebp);

	if (dest_x == 0x8000)return;

	// Set the coordinate of destination to be exactly 
	// in the middle of a tile.
	dest_x += 16;
	dest_y += 16;
	// Set the tile coordinate to top left of tile
	int tile_y = dest_y & 0xFFE0;
	int tile_x = dest_x & 0xFFE0;

	ebp = ((tile_y << 8) | tile_x) >> 5;

	rct_map_element* map_element = TILE_MAP_ELEMENT_POINTER(ebp);

	while (1){
		if ((map_element->type & MAP_ELEMENT_TYPE_MASK) != MAP_ELEMENT_TYPE_SURFACE){
			map_element->clearance_height = 0;
		}
		if (map_element->flags & MAP_ELEMENT_FLAG_LAST_TILE)
			break;
		map_element++;
	}
}