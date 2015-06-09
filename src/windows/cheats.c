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
#include "../config.h"
#include "../game.h"
#include "../interface/widget.h"
#include "../interface/window.h"
#include "../localisation/localisation.h"
#include "../world/park.h"
#include "../peep/peep.h"
#include "../ride/ride.h"
#include "../scenario.h"
#include "../sprites.h"
#include "../world/climate.h"
#include "../world/footpath.h"
#include "../world/park.h"
#include "../world/sprite.h"
#include "../interface/themes.h"

//#define WW 200
//#define WH 128
#define CHEATS_MONEY_INCREMENT MONEY(5000,00)
#define CHEATS_TRAM_INCREMENT 250
enum {
	WINDOW_CHEATS_PAGE_MONEY,
	WINDOW_CHEATS_PAGE_GUESTS,
	WINDOW_CHEATS_PAGE_MISC,
	WINDOW_CHEATS_PAGE_RIDES,
};

enum WINDOW_CHEATS_WIDGET_IDX {
	WIDX_BACKGROUND,
	WIDX_TITLE,
	WIDX_CLOSE,
	WIDX_PAGE_BACKGROUND,
	WIDX_TAB_1,
	WIDX_TAB_2,
	WIDX_TAB_3,
	WIDX_TAB_4,
	WIDX_HIGH_MONEY,
	WIDX_PARK_ENTRANCE_FEE,
	WIDX_HAPPY_GUESTS = 8, //Same as HIGH_MONEY as it is also the 8th widget but on a different page
	WIDX_TRAM_GUESTS,
	WIDX_NAUSEA_GUESTS,
	WIDX_FREEZE_CLIMATE = 8,
	WIDX_OPEN_CLOSE_PARK,
	WIDX_ZERO_CLEARANCE,
	WIDX_WEATHER_SUN,
	WIDX_WEATHER_THUNDER,
	WIDX_CLEAR_GRASS,
	WIDX_MOWED_GRASS,
	WIDX_WATER_PLANTS,
	WIDX_FIX_VANDALISM,
	WIDX_REMOVE_LITTER,
	WIDX_WIN_SCENARIO,
	WIDX_UNLOCK_ALL_PRICES,
	WIDX_INGAME_LAND_OWNERSHIP_EDITOR,
	WIDX_RENEW_RIDES = 8,
	WIDX_REMOVE_SIX_FLAGS,
	WIDX_MAKE_DESTRUCTIBLE,
	WIDX_FIX_ALL,
	WIDX_FAST_LIFT_HILL,
	WIDX_DISABLE_BRAKES_FAILURE,
	WIDX_DISABLE_ALL_BREAKDOWNS
};

#pragma region MEASUREMENTS
#define WW 240
#define WH 240
#define TAB_HEIGHT 43
#define XSPA 5			//X spacing
#define YSPA 5			//Y spacing
#define XOS 0			+ XSPA	//X offset from left
#define YOS TAB_HEIGHT	+ YSPA	//Y offset from top (includes tabs height)
#define BTNW 110		//button width
#define BTNH 16			//button height
#define OPTW 220		//Option (checkbox) width (two colums)
#define OPTH 10			//Option (checkbox) height (two colums)
#define YPL(ROW) YOS + ((BTNH + YSPA) * ROW)
#define HPL(ROW) YPL(ROW) + BTNH
#define OHPL(ROW) YPL(ROW) + OPTH
#define XPL(COL) XOS + ((BTNW + XSPA) * COL)
#define WPL(COL) XPL(COL) + BTNW
#define OWPL XPL(0) + OPTW

#define TXTO 3	//text horizontal offset from button left (for button text)
#pragma endregion

static rct_widget window_cheats_money_widgets[] = {
	{ WWT_FRAME,			0,	0,			WW - 1,	0,		WH - 1,		0x0FFFFFFFF,				65535},						// panel / background
	{ WWT_CAPTION,			0,	1,			WW - 2,	1,		14,			3165,						STR_WINDOW_TITLE_TIP},		// title bar
	{ WWT_CLOSEBOX,			0,	WW - 13,	WW - 3,	2,		13,			0x338,						STR_CLOSE_WINDOW_TIP},		// close x button
	{ WWT_IMGBTN,			1,	0,			WW - 1,	43,		WH - 1,		0x0FFFFFFFF,				65535},						// tab content panel
	{ WWT_TAB,				1,	3,			33,		17,		43,			0x2000144E,					STR_FINANCIAL_CHEATS_TIP },	// tab 1
	{ WWT_TAB,				1,	34,			64,		17,		43,			0x2000144E,					STR_GUEST_CHEATS_TIP },		// tab 2
	{ WWT_TAB,				1,	65,			95,		17,		43,			0x2000144E,					STR_PARK_CHEATS_TIP },		// tab 3
	{ WWT_TAB,				1,	96,			126,	17,		43,			0x2000144E,					STR_RIDE_CHEATS_TIP },		// tab 4
	{ WWT_CLOSEBOX,			1,	XPL(0),		WPL(0),	YPL(1),	HPL(1),		STR_CHEAT_5K_MONEY,			STR_NONE},					// high money
	{ WWT_CLOSEBOX,			1,	XPL(0),		WPL(0),	YPL(3),	HPL(3),		STR_CHEAT_PAY_ENTRANCE,		STR_NONE},					// Park Entrance Fee Toggle	
	{ WIDGETS_END },
};

static rct_widget window_cheats_guests_widgets[] = {
	{ WWT_FRAME,			0, 0,			WW - 1, 0,	WH - 1,		0x0FFFFFFFF,					65535 },					// panel / background
	{ WWT_CAPTION,			0, 1,			WW - 2, 1,	14,			3165,							STR_WINDOW_TITLE_TIP },		// title bar
	{ WWT_CLOSEBOX,			0, WW - 13,		WW - 3, 2,	13,			0x338,							STR_CLOSE_WINDOW_TIP },		// close x button
	{ WWT_IMGBTN,			1, 0,			WW - 1, 43, WH - 1,		0x0FFFFFFFF,					65535 },					// tab content panel
	{ WWT_TAB,				1, 3,			33,		17, 43,			0x2000144E,						STR_FINANCIAL_CHEATS_TIP },	// tab 1
	{ WWT_TAB,				1, 34,			64,		17, 43,			0x2000144E,						STR_GUEST_CHEATS_TIP },		// tab 2
	{ WWT_TAB,				1,	65,			95,		17,		43,		0x2000144E,						STR_PARK_CHEATS_TIP },		// tab 3
	{ WWT_TAB,				1,	96,			126,	17,		43,		0x2000144E,						STR_RIDE_CHEATS_TIP },		// tab 4
	{ WWT_CLOSEBOX,			1, XPL(0),	WPL(0),	YPL(1), HPL(1),		STR_CHEAT_HAPPY_GUESTS,			STR_NONE},					// happy guests
	{ WWT_CLOSEBOX,			1, XPL(0),	WPL(0),	YPL(3), HPL(3),		STR_CHEAT_LARGE_TRAM_GUESTS,	STR_NONE},					// large tram
	{ WWT_CLOSEBOX,			1, XPL(0),	WPL(0),	YPL(5), HPL(5),		STR_CHEAT_NAUSEA,				STR_NONE},					// nausea
	{ WIDGETS_END },
};

//Strings for following moved to window_cheats_paint()
static rct_widget window_cheats_misc_widgets[] = {
	{ WWT_FRAME,			0, 0,			WW - 1, 0,	WH - 1,		0x0FFFFFFFF,					65535 },					// panel / background
	{ WWT_CAPTION,			0, 1,			WW - 2, 1,	14,			3165,							STR_WINDOW_TITLE_TIP },		// title bar
	{ WWT_CLOSEBOX,			0, WW - 13,		WW - 3, 2,	13,			0x338,							STR_CLOSE_WINDOW_TIP },		// close x button
	{ WWT_IMGBTN,			1, 0,			WW - 1, 43, WH - 1,		0x0FFFFFFFF,					65535 },					// tab content panel
	{ WWT_TAB,				1, 3,			33,		17, 43,			0x2000144E,						STR_FINANCIAL_CHEATS_TIP },	// tab 1
	{ WWT_TAB,				1, 34,			64,		17, 43,			0x2000144E,						STR_GUEST_CHEATS_TIP },		// tab 2
	{ WWT_TAB,				1,	65,			95,		17,		43,		0x2000144E,						STR_PARK_CHEATS_TIP },		// tab 3
	{ WWT_TAB,				1,	96,			126,	17,		43,		0x2000144E,						STR_RIDE_CHEATS_TIP },		// tab 4
	{ WWT_CLOSEBOX,			1, XPL(1),	WPL(1),	YPL(1), HPL(1),		STR_CHEAT_FREEZE_CLIMATE,		STR_NONE},					// Freeze climate
	{ WWT_CLOSEBOX,			1, XPL(0),	WPL(0),	YPL(0), HPL(0),		STR_CHEAT_OPEN_PARK,			STR_NONE},					// open / close park
	{ WWT_CLOSEBOX,			1, XPL(0),	WPL(0),	YPL(1), HPL(1),		STR_CHEAT_ZERO_CLEARANCE,		STR_NONE},					// Zero Clearance
	{ WWT_CLOSEBOX,			1, XPL(0),	WPL(0),	YPL(2), HPL(2),		STR_CHEAT_FORCE_SUN,			STR_NONE},					// Sun
	{ WWT_CLOSEBOX,			1, XPL(1),	WPL(1),	YPL(2), HPL(2),		STR_CHEAT_FORCE_THUNDER,		STR_NONE},					// Thunder
	{ WWT_CLOSEBOX,			1, XPL(0),	WPL(0),	YPL(3), HPL(3),		STR_CHEAT_CLEAR_GRASS,			STR_NONE},					// Clear grass
	{ WWT_CLOSEBOX,			1, XPL(1),	WPL(1),	YPL(3), HPL(3),		STR_CHEAT_MOWED_GRASS,			STR_NONE},					// Mowed grass
	{ WWT_CLOSEBOX,			1, XPL(0),	WPL(0),	YPL(4), HPL(4),		STR_CHEAT_WATER_PLANTS,			STR_NONE},					// Water plants
	{ WWT_CLOSEBOX,			1, XPL(1),	WPL(1),	YPL(4), HPL(4),		STR_CHEAT_FIX_VANDALISM,		STR_NONE},					// Fix vandalism
	{ WWT_CLOSEBOX,			1, XPL(0),	WPL(0),	YPL(5), HPL(5),		STR_CHEAT_REMOVE_LITTER,		STR_NONE},					// Remove litter
	{ WWT_CLOSEBOX,			1, XPL(1),	WPL(1),	YPL(0), HPL(0),		STR_CHEAT_WIN_SCENARIO,			STR_NONE},					// Win scenario
	{ WWT_CHECKBOX,			1, XPL(0),    OWPL, YPL(8),OHPL(8),		STR_CHEAT_UNLOCK_PRICES,		STR_NONE}, 					// Unlock all prices
	{ WWT_CLOSEBOX,			1, XPL(1),  WPL(1), YPL(5), HPL(5),		STR_CHEAT_INGAME_LAND_OWNERSHIP_EDITOR,STR_CHEAT_INGAME_LAND_OWNERSHIP_EDITOR_TIP},// In-game land ownership editor
	{ WIDGETS_END },
};
static rct_widget window_cheats_rides_widgets[] = {
	{ WWT_FRAME,			0, 0,			WW - 1, 0,	WH - 1,		0x0FFFFFFFF,					65535 },					// panel / background
	{ WWT_CAPTION,			0, 1,			WW - 2, 1,	14,			3165,							STR_WINDOW_TITLE_TIP },		// title bar
	{ WWT_CLOSEBOX,			0, WW - 13,		WW - 3, 2,	13,			0x338,							STR_CLOSE_WINDOW_TIP },		// close x button
	{ WWT_IMGBTN,			1, 0,			WW - 1, 43, WH - 1,		0x0FFFFFFFF,					65535 },					// tab content panel
	{ WWT_TAB,				1, 3,			33,		17, 43,			0x2000144E,						STR_FINANCIAL_CHEATS_TIP },	// tab 1
	{ WWT_TAB,				1, 34,			64,		17, 43,			0x2000144E,						STR_GUEST_CHEATS_TIP },		// tab 2
	{ WWT_TAB,				1,	65,			95,		17,		43,		0x2000144E,						STR_PARK_CHEATS_TIP },		// tab 3
	{ WWT_TAB,				1,	96,			126,	17,		43,		0x2000144E,						STR_RIDE_CHEATS_TIP },		// tab 4
	{ WWT_CLOSEBOX,			1, XPL(0),	WPL(0),	YPL(0), HPL(0),		STR_CHEAT_RENEW_RIDES,			STR_NONE},					// Renew rides
	{ WWT_CLOSEBOX,			1, XPL(1),	WPL(1),	YPL(0), HPL(0),		STR_CHEAT_REMOVE_FLAGS,			STR_NONE},					// Remove flags
	{ WWT_CLOSEBOX,			1, XPL(1),	WPL(1),	YPL(1), HPL(1),		STR_CHEAT_MAKE_DESTRUCTABLE,	STR_NONE},					// Make destructable
	{ WWT_CLOSEBOX,			1, XPL(0),	WPL(0), YPL(1), HPL(1),		STR_CHEAT_FIX_ALL_RIDES,		STR_NONE },					// Fix all rides
	{ WWT_CHECKBOX,			2, XPL(0),    OWPL, YPL(8),OHPL(8),		STR_CHEAT_410_HILL_LIFT,		STR_NONE }, 				// 410 km/h lift hill
	{ WWT_CHECKBOX,			2, XPL(0),    OWPL, YPL(6),OHPL(6),		STR_CHEAT_DISABLE_BRAKES_FAILURE,STR_NONE }, 				// Disable brakes failure
	{ WWT_CHECKBOX,			2, XPL(0),    OWPL, YPL(7),OHPL(7),		STR_CHEAT_DISABLE_BREAKDOWNS,	STR_NONE }, 				// Disable all breakdowns
	{ WIDGETS_END },
};

static rct_widget *window_cheats_page_widgets[] = {
	window_cheats_money_widgets,
	window_cheats_guests_widgets,
	window_cheats_misc_widgets,
	window_cheats_rides_widgets,
};

static void window_cheats_emptysub() { }
static void window_cheats_money_mouseup();
static void window_cheats_guests_mouseup();
static void window_cheats_misc_mouseup();
static void window_cheats_rides_mouseup();
static void window_cheats_misc_tool_update();
static void window_cheats_misc_tool_down();
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

static void* window_cheats_rides_events[] = {
	window_cheats_emptysub,
	window_cheats_rides_mouseup,
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
	window_cheats_rides_events,
};

static uint32 window_cheats_page_enabled_widgets[] = {
	(1 << WIDX_CLOSE) | (1 << WIDX_TAB_1) | (1 << WIDX_TAB_2) | (1 << WIDX_TAB_3) | (1 << WIDX_TAB_4) | (1 << WIDX_HIGH_MONEY) | (1 << WIDX_PARK_ENTRANCE_FEE),
	(1 << WIDX_CLOSE) | (1 << WIDX_TAB_1) | (1 << WIDX_TAB_2) | (1 << WIDX_TAB_3) | (1 << WIDX_TAB_4) | (1 << WIDX_HAPPY_GUESTS) | (1 << WIDX_TRAM_GUESTS) | (1 << WIDX_NAUSEA_GUESTS),
	(1 << WIDX_CLOSE) | (1 << WIDX_TAB_1) | (1 << WIDX_TAB_2) | (1 << WIDX_TAB_3) | (1 << WIDX_TAB_4) | (1 << WIDX_FREEZE_CLIMATE) | (1 << WIDX_OPEN_CLOSE_PARK) | (1 << WIDX_ZERO_CLEARANCE) | (1 << WIDX_WEATHER_SUN) | (1 << WIDX_WEATHER_THUNDER) | (1 << WIDX_CLEAR_GRASS) | (1 << WIDX_MOWED_GRASS) | (1 << WIDX_WATER_PLANTS) | (1 << WIDX_FIX_VANDALISM) | (1 << WIDX_REMOVE_LITTER) | (1 << WIDX_WIN_SCENARIO) | (1 << WIDX_UNLOCK_ALL_PRICES) | (1 << WIDX_INGAME_LAND_OWNERSHIP_EDITOR),
	(1 << WIDX_CLOSE) | (1 << WIDX_TAB_1) | (1 << WIDX_TAB_2) | (1 << WIDX_TAB_3) | (1 << WIDX_TAB_4) | (1 << WIDX_RENEW_RIDES) | (1 << WIDX_REMOVE_SIX_FLAGS) | (1 << WIDX_MAKE_DESTRUCTIBLE) | (1 << WIDX_FIX_ALL) | (1 << WIDX_FAST_LIFT_HILL) | (1 << WIDX_DISABLE_BRAKES_FAILURE) | (1 << WIDX_DISABLE_ALL_BREAKDOWNS)
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
	map_element_iterator it;

	map_element_iterator_begin(&it);
	do {
		if (map_element_get_type(it.element) == MAP_ELEMENT_TYPE_SCENERY) {
			it.element->properties.scenery.age = 0;
		}
	} while (map_element_iterator_next(&it));

	gfx_invalidate_screen();
}

static void cheat_fix_vandalism()
{
	map_element_iterator it;

	map_element_iterator_begin(&it);
	do {
		if (map_element_get_type(it.element) != MAP_ELEMENT_TYPE_PATH)
			continue;

		if ((it.element->properties.path.additions & 0x0F) == 0)
			continue;

		it.element->flags &= ~MAP_ELEMENT_FLAG_BROKEN;
	} while (map_element_iterator_next(&it));

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

	gfx_invalidate_screen();
}

static void cheat_fix_rides()
{
	int rideIndex;
	rct_ride *ride;
	rct_peep *mechanic;

	FOR_ALL_RIDES(rideIndex, ride)
	{
		if ((ride->mechanic_status != RIDE_MECHANIC_STATUS_FIXING) && (ride->lifecycle_flags & (RIDE_LIFECYCLE_BREAKDOWN_PENDING | RIDE_LIFECYCLE_BROKEN_DOWN)))
		{
			mechanic = ride_get_assigned_mechanic(ride);

			if (mechanic != NULL){
				remove_peep_from_ride(mechanic);
			}

			RCT2_CALLPROC_X(0x006B7481, 0, 0, 0, rideIndex, 0, 0, 0);
			ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_MAIN | RIDE_INVALIDATE_RIDE_LIST;
		}
	}
}

static void cheat_renew_rides()
{
	int i;
	rct_ride *ride;

	FOR_ALL_RIDES(i, ride)
	{
		// Set build date to current date (so the ride is brand new)
		ride->build_date = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_MONTH_YEAR, uint16);
		// Set reliability to 100
		ride->reliability = (100 << 8);
	}
	window_invalidate_by_class(WC_RIDE);
}

static void cheat_remove_six_flags()
{
	int i;
	rct_ride *ride;
	FOR_ALL_RIDES(i, ride)
	{
		if (ride->lifecycle_flags & RIDE_LIFECYCLE_SIX_FLAGS)
			ride->lifecycle_flags&=~RIDE_LIFECYCLE_SIX_FLAGS;
	}
	window_invalidate_by_class(WC_RIDE);
}

static void cheat_make_destructible()
{
	int i;
	rct_ride *ride;
	FOR_ALL_RIDES(i, ride)
	{
		if (ride->lifecycle_flags & RIDE_LIFECYCLE_INDESTRUCTIBLE)
			ride->lifecycle_flags&=~RIDE_LIFECYCLE_INDESTRUCTIBLE;
	}
	window_invalidate_by_class(WC_RIDE);
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

static void cheat_make_guests_nauseous()
{
	int spriteIndex;
	rct_peep *peep;

	FOR_ALL_GUESTS(spriteIndex, peep)
		peep->flags |= PEEP_FLAGS_NAUSEA;
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
	case WIDX_TAB_4:
		window_cheats_set_page(w, widgetIndex - WIDX_TAB_1);
		break;
	case WIDX_HIGH_MONEY:
		cheat_increase_money(CHEATS_MONEY_INCREMENT);
		break;
	case WIDX_PARK_ENTRANCE_FEE:
		RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) ^= PARK_FLAGS_PARK_FREE_ENTRY;
		window_invalidate_by_class(WC_PARK_INFORMATION);
		window_invalidate_by_class(WC_RIDE);
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
	case WIDX_TAB_4:
		window_cheats_set_page(w, widgetIndex - WIDX_TAB_1);
		break;
	case WIDX_HAPPY_GUESTS:
		cheat_make_guests_happy();
		break;
	case WIDX_TRAM_GUESTS:
		cheat_generate_guests(CHEATS_TRAM_INCREMENT);
		break;
	case WIDX_NAUSEA_GUESTS:
		cheat_make_guests_nauseous();
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
	case WIDX_TAB_4:
		window_cheats_set_page(w, widgetIndex - WIDX_TAB_1);
		break;
	case WIDX_FREEZE_CLIMATE:
		toggle_climate_lock();
		w->widgets[widgetIndex].image = w->widgets[widgetIndex].image == STR_CHEAT_FREEZE_CLIMATE ? STR_CHEAT_UNFREEZE_CLIMATE : STR_CHEAT_FREEZE_CLIMATE;
		break;
	case WIDX_OPEN_CLOSE_PARK:
		park_set_open(park_is_open() ? 0 : 1);
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
	case WIDX_WIN_SCENARIO:
		scenario_success();
		break;
	case WIDX_UNLOCK_ALL_PRICES:
		gConfigCheat.unlock_all_prices ^= 1;
		config_save_default();
		window_invalidate(w);
		window_invalidate_by_class(WC_RIDE);
		window_invalidate_by_class(WC_PARK_INFORMATION);
		break;
	case WIDX_INGAME_LAND_OWNERSHIP_EDITOR:
		toggle_ingame_land_ownership_editor();
		w->widgets[widgetIndex].image = w->widgets[widgetIndex].image == STR_CHEAT_INGAME_LAND_OWNERSHIP_EDITOR ? STR_CHEAT_INGAME_LAND_OWNERSHIP_EDITOR_DISABLE : STR_CHEAT_INGAME_LAND_OWNERSHIP_EDITOR;
		// To prevent tools from staying active after disabling cheat
		window_close_by_class(WC_MAP);
		break;
	}
}

static void window_cheats_rides_mouseup()
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
	case WIDX_TAB_4:
		window_cheats_set_page(w, widgetIndex - WIDX_TAB_1);
		break;
	case WIDX_RENEW_RIDES:
		cheat_renew_rides();
		break;
	case WIDX_REMOVE_SIX_FLAGS:
		cheat_remove_six_flags();
		break;
	case WIDX_MAKE_DESTRUCTIBLE:
		cheat_make_destructible();
		break;
	case WIDX_FIX_ALL:
		cheat_fix_rides();
		break;
	case WIDX_FAST_LIFT_HILL:
		gConfigCheat.fast_lift_hill ^= 1;
		config_save_default();
		window_invalidate(w);
		break;
	case WIDX_DISABLE_BRAKES_FAILURE:
		gConfigCheat.disable_brakes_failure ^= 1;
		config_save_default();
		window_invalidate(w);
		break;
	case WIDX_DISABLE_ALL_BREAKDOWNS:
		gConfigCheat.disable_all_breakdowns ^= 1;
		config_save_default();
		window_invalidate(w);
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
	colour_scheme_update(w);

	strcpy((char*)0x009BC677, "Cheats");

	rct_widget *widgets = window_cheats_page_widgets[w->page];
	if (w->widgets != widgets) {
		w->widgets = widgets;
		window_init_scroll_widgets(w);
	}

	w->pressed_widgets = 0;

	switch (w->page) {
	case WINDOW_CHEATS_PAGE_MONEY:
		w->widgets[WIDX_PARK_ENTRANCE_FEE].image = RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & PARK_FLAGS_PARK_FREE_ENTRY ?
			STR_CHEAT_PAY_ENTRANCE : STR_CHEAT_PAY_RIDES;
		break;
	case WINDOW_CHEATS_PAGE_MISC:
		w->widgets[WIDX_OPEN_CLOSE_PARK].image = RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & PARK_FLAGS_PARK_OPEN ?
			STR_CHEAT_CLOSE_PARK : STR_CHEAT_OPEN_PARK;
		widget_set_checkbox_value(w, WIDX_UNLOCK_ALL_PRICES, gConfigCheat.unlock_all_prices);
		break;
	case WINDOW_CHEATS_PAGE_RIDES:
		RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS + 0, uint16) = 255;
		widget_set_checkbox_value(w, WIDX_FAST_LIFT_HILL, gConfigCheat.fast_lift_hill);
		widget_set_checkbox_value(w, WIDX_DISABLE_BRAKES_FAILURE, gConfigCheat.disable_brakes_failure);
		widget_set_checkbox_value(w, WIDX_DISABLE_ALL_BREAKDOWNS, gConfigCheat.disable_all_breakdowns);
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
		gfx_draw_string_left(dpi, STR_CHEAT_TIP_5K_MONEY,			NULL,	0, w->x + XPL(0) + TXTO, w->y + YPL(0) + TXTO);
		gfx_draw_string_left(dpi, STR_CHEAT_TIP_PAY_ENTRY,			NULL,	0, w->x + XPL(0) + TXTO, w->y + YPL(2) + TXTO);
	}
	else if (w->page == WINDOW_CHEATS_PAGE_GUESTS){
		gfx_draw_string_left(dpi, STR_CHEAT_TIP_HAPPY_GUESTS,		NULL,	0, w->x + XPL(0) + TXTO, w->y + YPL(0) + TXTO);
		gfx_draw_string_left(dpi, STR_CHEAT_TIP_LARGE_TRAM_GUESTS,	NULL,	0, w->x + XPL(0) + TXTO, w->y + YPL(2) + TXTO);
		gfx_draw_string_left(dpi, STR_CHEAT_TIP_NAUSEA,				NULL,	0, w->x + XPL(0) + TXTO, w->y + YPL(4) + TXTO);
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
		sprite_idx = STR_TAB_PARK;
		gfx_draw_sprite(dpi, sprite_idx, w->x + w->widgets[WIDX_TAB_3].left, w->y + w->widgets[WIDX_TAB_3].top, 0);
	}

	// Rides tab
	if (!(w->disabled_widgets & (1 << WIDX_TAB_4))) {
		sprite_idx = SPR_TAB_RIDE_0;
		if (w->page == WINDOW_CHEATS_PAGE_RIDES)
			sprite_idx += (w->frame_no / 4) % 16;
		gfx_draw_sprite(dpi, sprite_idx, w->x + w->widgets[WIDX_TAB_4].left, w->y + w->widgets[WIDX_TAB_4].top, 0);
	}
}

static void window_cheats_set_page(rct_window *w, int page)
{
	w->page = page;
	w->frame_no = 0;
	
	w->enabled_widgets = window_cheats_page_enabled_widgets[page];
	
	w->pressed_widgets = 0;

	w->event_handlers = window_cheats_page_events[page];
	w->widgets = window_cheats_page_widgets[page];

	window_invalidate(w);
}

static void window_cheats_misc_tool_update()
{
	short widgetIndex;
	rct_window* w;
	short x, y;

	window_tool_get_registers(w, widgetIndex, x, y);

	if (widgetIndex != WIDX_ZERO_CLEARANCE) return;

	map_invalidate_selection_rect();

	RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) &= ~(1 << 0);

	int map_x, map_y;
	footpath_get_coordinates_from_pos(x, y + 16, &map_x, &map_y, NULL, NULL);
	if (map_x != (sint16)0x8000){
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) |= 1;
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_TYPE, uint16) = 4;
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_X, sint16) = map_x;
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_X, sint16) = map_x;
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_Y, sint16) = map_y;
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_Y, sint16) = map_y;
		map_invalidate_selection_rect();
	}

	RCT2_GLOBAL(RCT2_ADDRESS_PICKEDUP_PEEP_SPRITE, sint32) = -1;
}

static void window_cheats_misc_tool_down()
{
	short widgetIndex;
	rct_window* w;
	short x, y;

	window_tool_get_registers(w, widgetIndex, x, y);

	if (widgetIndex != WIDX_ZERO_CLEARANCE) return;

	int dest_x, dest_y;
	footpath_get_coordinates_from_pos(x, y + 16, &dest_x, &dest_y, NULL, NULL);

	if (dest_x == (sint16)0x8000)return;

	// Set the coordinate of destination to be exactly 
	// in the middle of a tile.
	dest_x += 16;
	dest_y += 16;

	// Set the tile coordinate to top left of tile
	int tile_x = (dest_x & 0xFFE0) >> 5;
	int tile_y = (dest_y & 0xFFE0) >> 5;

	rct_map_element *mapElement = map_get_first_element_at(tile_x, tile_y);
	do {
		if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_SURFACE) {
			mapElement->clearance_height = 0;
		}
	} while (!map_element_is_last_for_tile(mapElement++));
}
