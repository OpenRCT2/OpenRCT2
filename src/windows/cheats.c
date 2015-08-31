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
#include "../world/scenery.h"
#include "../interface/themes.h"
#include "../cheats.h"

#define CHEATS_MONEY_INCREMENT MONEY(5000,00)
#define CHEATS_TRAM_INCREMENT 250

#define CHEATS_PARK_RATING_SPINNER_PAUSE 20

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
	WIDX_CLEAR_LOAN,
	WIDX_GUEST_PARAMETERS_GROUP = 8, //Same as HIGH_MONEY as it is also the 8th widget but on a different page
	WIDX_GUEST_HAPPINESS_MAX,
	WIDX_GUEST_HAPPINESS_MIN,
	WIDX_GUEST_ENERGY_MAX,
	WIDX_GUEST_ENERGY_MIN,
	WIDX_GUEST_HUNGER_MAX,
	WIDX_GUEST_HUNGER_MIN,
	WIDX_GUEST_THIRST_MAX,
	WIDX_GUEST_THIRST_MIN,
	WIDX_GUEST_NAUSEA_MAX,
	WIDX_GUEST_NAUSEA_MIN,
	WIDX_GUEST_NAUSEA_TOLERANCE_MAX,
	WIDX_GUEST_NAUSEA_TOLERANCE_MIN,
	WIDX_GUEST_BATHROOM_MAX,
	WIDX_GUEST_BATHROOM_MIN,
	WIDX_GUEST_RIDE_INTENSITY_MORE_THAN_1,
	WIDX_GUEST_RIDE_INTENSITY_LESS_THAN_15,
	WIDX_GUEST_IGNORE_RIDE_INTENSITY,
	WIDX_DISABLE_VANDALISM,
	WIDX_GIVE_ALL_GUESTS_GROUP,
	WIDX_GIVE_GUESTS_MONEY,
	WIDX_GIVE_GUESTS_PARK_MAPS,
	WIDX_GIVE_GUESTS_BALLOONS,
	WIDX_GIVE_GUESTS_UMBRELLAS,
	WIDX_TRAM_GUESTS,
	WIDX_REMOVE_ALL_GUESTS,
	WIDX_EXPLODE_GUESTS,
	WIDX_GENERAL_GROUP = 8,
	WIDX_OPEN_CLOSE_PARK,
	WIDX_PARK_PARAMETERS,
	WIDX_SANDBOX_MODE,
	WIDX_UNLOCK_ALL_PRICES,
	WIDX_FORCE_PARK_RATING,
	WIDX_PARK_RATING_SPINNER,
	WIDX_INCREASE_PARK_RATING,
	WIDX_DECREASE_PARK_RATING,
	WIDX_WIN_SCENARIO,
	WIDX_HAVE_FUN,
	WIDX_CLIMATE_GROUP,
	WIDX_FREEZE_CLIMATE,
	WIDX_WEATHER_SUN,
	WIDX_WEATHER_THUNDER,
	WIDX_STAFF_GROUP,
	WIDX_CLEAR_GRASS,
	WIDX_MOWED_GRASS,
	WIDX_WATER_PLANTS,
	WIDX_FIX_VANDALISM,
	WIDX_REMOVE_LITTER,
	WIDX_FAST_STAFF,
	WIDX_NORMAL_STAFF,
	WIDX_RENEW_RIDES = 8,
	WIDX_MAKE_DESTRUCTIBLE,
	WIDX_FIX_ALL,
	WIDX_FAST_LIFT_HILL,
	WIDX_DISABLE_BRAKES_FAILURE,
	WIDX_DISABLE_ALL_BREAKDOWNS,
	WIDX_BUILD_IN_PAUSE_MODE,
	WIDX_RESET_CRASH_STATUS
};

enum {
	GUEST_PARAMETER_HAPPINESS,
	GUEST_PARAMETER_ENERGY,
	GUEST_PARAMETER_HUNGER,
	GUEST_PARAMETER_THIRST,
	GUEST_PARAMETER_NAUSEA,
	GUEST_PARAMETER_NAUSEA_TOLERANCE,
	GUEST_PARAMETER_BATHROOM,
	GUEST_PARAMETER_PREFERRED_RIDE_INTENSITY
};

enum {
	OBJECT_MONEY,
	OBJECT_PARK_MAP,
	OBJECT_BALLOON,
	OBJECT_UMBRELLA
};

#pragma region MEASUREMENTS

#define WW				249
#define WH				300
#define TAB_HEIGHT		43
#define XSPA			5												// X spacing
#define YSPA			5												// Y spacing
#define XOS				6 + XSPA										// X offset from left
#define YOS				TAB_HEIGHT + YSPA								// Y offset from top (includes tabs height)
#define BTNW			110												// button width
#define BTNH			16												// button height
#define OPTW			220												// Option (checkbox) width (two colums)
#define OPTH			10												// Option (checkbox) height (two colums)
#define GROUP_SPACE		6

#define YPL(ROW)		((sint16)(YOS + ((BTNH + YSPA) * ROW)))
#define HPL(ROW)		((sint16)(YPL(ROW) + BTNH))
#define OHPL(ROW)		((sint16)(YPL(ROW) + OPTH))
#define XPL(COL)		((sint16)(XOS + ((BTNW + XSPA) * COL)))
#define WPL(COL)		((sint16)(XPL(COL) + BTNW))
#define OWPL			((sint16)(XPL(0) + OPTW))

#define MIN_BTN_LEFT	((sint16)(XPL(1)))
#define MIN_BTN_RIGHT	((sint16)(WPL(1) - (BTNW / 2)))
#define MAX_BTN_LEFT	((sint16)(XPL(1.5)))
#define MAX_BTN_RIGHT	((sint16)(WPL(1)))

#define TXTO 3	//text horizontal offset from button left (for button text)
#pragma endregion

#define MAIN_CHEATS_WIDGETS \
	{ WWT_FRAME,			0,	0,			WW - 1,	0,		WH - 1,		0x0FFFFFFFF,		STR_NONE },					/* panel / background	*/ \
	{ WWT_CAPTION,			0,	1,			WW - 2,	1,		14,			STR_CHEAT_TITLE,	STR_WINDOW_TITLE_TIP },		/* title bar			*/ \
	{ WWT_CLOSEBOX,			0,	WW - 13,	WW - 3,	2,		13,			STR_CLOSE_X,		STR_CLOSE_WINDOW_TIP },		/* close x button		*/ \
	{ WWT_IMGBTN,			1,	0,			WW - 1,	43,		WH - 1,		0x0FFFFFFFF,		STR_NONE },					/* tab content panel	*/ \
	{ WWT_TAB,				1,	3,			33,		17,		43,			0x2000144E,			STR_FINANCIAL_CHEATS_TIP },	/* tab 1				*/ \
	{ WWT_TAB,				1,	34,			64,		17,		43,			0x2000144E,			STR_GUEST_CHEATS_TIP },		/* tab 2				*/ \
	{ WWT_TAB,				1,	65,			95,		17,		43,			0x2000144E,			STR_PARK_CHEATS_TIP },		/* tab 3				*/ \
	{ WWT_TAB,				1,	96,			126,	17,		43,			0x2000144E,			STR_RIDE_CHEATS_TIP }		/* tab 4				*/

static rct_widget window_cheats_money_widgets[] = {
	MAIN_CHEATS_WIDGETS,
	{ WWT_CLOSEBOX,			1,		XPL(0),					WPL(0),					YPL(1),			HPL(1),			STR_CHEAT_5K_MONEY,					STR_NONE },								// high money
	{ WWT_CLOSEBOX,			1,		XPL(0),					WPL(0),					YPL(5),			HPL(5),			STR_CHEAT_CLEAR_LOAN,				STR_NONE },								// Clear loan
	{ WIDGETS_END },
};

static rct_widget window_cheats_guests_widgets[] = {
	MAIN_CHEATS_WIDGETS,
	{ WWT_GROUPBOX,			1,		XPL(0) - GROUP_SPACE,	WPL(1) + GROUP_SPACE,	YPL(0),			HPL(11.5),		STR_CHEAT_SET_GUESTS_PARAMETERS,	STR_NONE },								// Guests parameters group frame
	{ WWT_CLOSEBOX,			1,		MAX_BTN_LEFT,			MAX_BTN_RIGHT,			YPL(1),			HPL(1),			STR_MAX,							STR_NONE },								// happiness max
	{ WWT_CLOSEBOX,			1,		MIN_BTN_LEFT,			MIN_BTN_RIGHT,			YPL(1),			HPL(1),			STR_MIN,							STR_NONE },								// happiness min
	{ WWT_CLOSEBOX,			1,		MAX_BTN_LEFT,			MAX_BTN_RIGHT,			YPL(2),			HPL(2),			STR_MAX,							STR_NONE },								// energy max
	{ WWT_CLOSEBOX,			1,		MIN_BTN_LEFT,			MIN_BTN_RIGHT,			YPL(2),			HPL(2),			STR_MIN,							STR_NONE },								// energy min
	{ WWT_CLOSEBOX,			1,		MAX_BTN_LEFT,			MAX_BTN_RIGHT,			YPL(3),			HPL(3),			STR_MAX,							STR_NONE },								// hunger max
	{ WWT_CLOSEBOX,			1,		MIN_BTN_LEFT,			MIN_BTN_RIGHT,			YPL(3),			HPL(3),			STR_MIN,							STR_NONE },								// hunger min
	{ WWT_CLOSEBOX,			1,		MAX_BTN_LEFT,			MAX_BTN_RIGHT,			YPL(4),			HPL(4),			STR_MAX,							STR_NONE },								// thirst max
	{ WWT_CLOSEBOX,			1,		MIN_BTN_LEFT,			MIN_BTN_RIGHT,			YPL(4),			HPL(4),			STR_MIN,							STR_NONE },								// thirst min
	{ WWT_CLOSEBOX,			1,		MAX_BTN_LEFT,			MAX_BTN_RIGHT,			YPL(5),			HPL(5),			STR_MAX,							STR_NONE },								// nausea max
	{ WWT_CLOSEBOX,			1,		MIN_BTN_LEFT,			MIN_BTN_RIGHT,			YPL(5),			HPL(5),			STR_MIN,							STR_NONE },								// nausea min
	{ WWT_CLOSEBOX,			1,		MAX_BTN_LEFT,			MAX_BTN_RIGHT,			YPL(6),			HPL(6),			STR_MAX,							STR_NONE },								// nausea tolerance max
	{ WWT_CLOSEBOX,			1,		MIN_BTN_LEFT,			MIN_BTN_RIGHT,			YPL(6),			HPL(6),			STR_MIN,							STR_NONE },								// nausea tolerance min
	{ WWT_CLOSEBOX,			1,		MAX_BTN_LEFT,			MAX_BTN_RIGHT,			YPL(7),			HPL(7),			STR_MAX,							STR_NONE },								// bathroom max
	{ WWT_CLOSEBOX,			1,		MIN_BTN_LEFT,			MIN_BTN_RIGHT,			YPL(7),			HPL(7),			STR_MIN,							STR_NONE },								// bathroom min
	{ WWT_CLOSEBOX,			1,		XPL(1),					WPL(1),					YPL(9),			HPL(9),			STR_CHEAT_MORE_THAN_1,				STR_NONE },								// ride intensity > 1
	{ WWT_CLOSEBOX,			1,		XPL(0),					WPL(0),					YPL(9),			HPL(9),			STR_CHEAT_LESS_THAN_15,				STR_NONE },								// ride intensity < 15
	{ WWT_CHECKBOX,			2,		XPL(0),					OWPL, 					YPL(10),		OHPL(10),		STR_CHEAT_IGNORE_INTENSITY,			STR_NONE },								// guests ignore intensity
	{ WWT_CHECKBOX,			2,		XPL(0),					OWPL, 					YPL(11),		OHPL(11),		STR_CHEAT_DISABLE_VANDALISM,		STR_CHEAT_DISABLE_VANDALISM_TIP },		// disable vandalism
	{ WWT_GROUPBOX,			1,		XPL(0) - GROUP_SPACE,	WPL(1) + GROUP_SPACE,	YPL(13),		HPL(15.5),		STR_CHEAT_GIVE_ALL_GUESTS,			STR_NONE },								// Guests parameters group frame
	{ WWT_CLOSEBOX,			1,		XPL(0),					WPL(0),					YPL(14),		HPL(14),		STR_CHEAT_CURRENCY_FORMAT,			STR_NONE },								// give guests money
	{ WWT_CLOSEBOX,			1,		XPL(1),					WPL(1),					YPL(14),		HPL(14),		STR_SHOP_ITEM_PLURAL_PARK_MAP,		STR_NONE },								// give guests park maps
	{ WWT_CLOSEBOX,			1,		XPL(0),					WPL(0),					YPL(15),		HPL(15),		STR_SHOP_ITEM_PLURAL_BALLOON,		STR_NONE },								// give guests balloons
	{ WWT_CLOSEBOX,			1,		XPL(1),					WPL(1),					YPL(15),		HPL(15),		STR_SHOP_ITEM_PLURAL_UMBRELLA,		STR_NONE },								// give guests umbrellas
	{ WWT_CLOSEBOX,			1,		XPL(0),					WPL(0),					YPL(17),		HPL(17),		STR_CHEAT_LARGE_TRAM_GUESTS,		STR_CHEAT_TIP_LARGE_TRAM_GUESTS },		// large tram
	{ WWT_CLOSEBOX,			1,		XPL(1),					WPL(1),					YPL(17),		HPL(17),		STR_CHEAT_REMOVE_ALL_GUESTS,		STR_CHEAT_TIP_REMOVE_ALL_GUESTS },		// remove all guests
	{ WWT_CLOSEBOX,			1,		XPL(0),					WPL(0),					YPL(18),		HPL(18),		STR_CHEAT_EXPLODE,					STR_CHEAT_TIP_EXPLODE },				// explode guests
	{ WIDGETS_END },
};

//Strings for following moved to window_cheats_paint()
static rct_widget window_cheats_misc_widgets[] = {
	MAIN_CHEATS_WIDGETS,
	{ WWT_GROUPBOX,			1,		XPL(0) - GROUP_SPACE,	WPL(1) + GROUP_SPACE,	YPL(0),			HPL(5.5),		STR_CHEAT_GENERAL_GROUP,			STR_NONE },								// General group
	{ WWT_CLOSEBOX,			1,		XPL(0),					WPL(0),					YPL(1), 		HPL(1),			STR_CHEAT_OPEN_PARK,				STR_NONE},								// open / close park
	{ WWT_CLOSEBOX,			1,		XPL(1),					WPL(1), 				YPL(1), 		HPL(1),			STR_CHEAT_PARK_PARAMETERS,			STR_CHEAT_TIP_PARK_PARAMETERS},			// Park parameters
	{ WWT_CLOSEBOX,			1,		XPL(0),					WPL(0), 				YPL(2), 		HPL(2),			STR_CHEAT_SANDBOX_MODE,				STR_CHEAT_SANDBOX_MODE_TIP},			// Sandbox mode (edit land ownership in-game)
	{ WWT_CHECKBOX,			1,		XPL(0),					OWPL, 					YPL(3),			OHPL(3),		STR_CHEAT_UNLOCK_PRICES,			STR_NONE}, 								// Unlock all prices
	{ WWT_CHECKBOX,			1,		XPL(0),					WPL(0), 				YPL(4), 		HPL(4),			STR_FORCE_PARK_RATING,				STR_NONE},								// Force park rating
	{ WWT_SPINNER,			1,		XPL(1),					WPL(1) - 10,			YPL(4) + 2,		HPL(4) - 3,		STR_NONE,							STR_NONE },								// park rating
	{ WWT_DROPDOWN_BUTTON,	1,		WPL(1) - 10,			WPL(1),					YPL(4) + 3,		YPL(4) + 7,		STR_NUMERIC_UP,						STR_NONE },								// increase rating
	{ WWT_DROPDOWN_BUTTON,	1,		WPL(1) - 10,			WPL(1),					YPL(4) + 8,		YPL(4) + 12,	STR_NUMERIC_DOWN,					STR_NONE },								// decrease rating
	{ WWT_CLOSEBOX,			1,		XPL(0),					WPL(0),					YPL(5), 		HPL(5),			STR_CHEAT_WIN_SCENARIO,				STR_NONE},								// Win scenario
	{ WWT_CLOSEBOX,			1,		XPL(1),					WPL(1),					YPL(5), 		HPL(5),			STR_CHEAT_HAVE_FUN,					STR_NONE},								// Have fun!
	{ WWT_GROUPBOX,			1,		XPL(0) - GROUP_SPACE,	WPL(1) + GROUP_SPACE,	YPL(7),			HPL(9.5),		STR_CHEAT_CLIMATE_GROUP,			STR_NONE },								// Climate group
	{ WWT_CLOSEBOX,			1,		XPL(0),					WPL(0),					YPL(8), 		HPL(8),			STR_CHEAT_FREEZE_CLIMATE,			STR_NONE},								// Freeze climate
	{ WWT_CLOSEBOX,			1,		XPL(0),					WPL(0),					YPL(9), 		HPL(9),			STR_CHEAT_FORCE_SUN,				STR_NONE},								// Sun
	{ WWT_CLOSEBOX,			1,		XPL(1),					WPL(1),					YPL(9), 		HPL(9),			STR_CHEAT_FORCE_THUNDER,			STR_NONE},								// Thunder
	{ WWT_GROUPBOX,			1,		XPL(0) - GROUP_SPACE,	WPL(1) + GROUP_SPACE,	YPL(11),		HPL(15.5),		STR_CHEAT_STAFF_GROUP,				STR_NONE },								// Staff group
	{ WWT_CLOSEBOX,			1,		XPL(0),					WPL(0),					YPL(12), 		HPL(12),		STR_CHEAT_CLEAR_GRASS,				STR_NONE},								// Clear grass
	{ WWT_CLOSEBOX,			1,		XPL(1),					WPL(1),					YPL(12), 		HPL(12),		STR_CHEAT_MOWED_GRASS,				STR_NONE},								// Mowed grass
	{ WWT_CLOSEBOX,			1,		XPL(0),					WPL(0),					YPL(13), 		HPL(13),		STR_CHEAT_WATER_PLANTS,				STR_NONE},								// Water plants
	{ WWT_CLOSEBOX,			1,		XPL(1),					WPL(1),					YPL(13), 		HPL(13),		STR_CHEAT_FIX_VANDALISM,			STR_NONE},								// Fix vandalism
	{ WWT_CLOSEBOX,			1,		XPL(0),					WPL(0),					YPL(14), 		HPL(14),		STR_CHEAT_REMOVE_LITTER,			STR_NONE},								// Remove litter
	{ WWT_CLOSEBOX,			1,		MAX_BTN_LEFT,			MAX_BTN_RIGHT,			YPL(15),		HPL(15),		STR_FAST,							STR_NONE },								// Fast staff
	{ WWT_CLOSEBOX,			1,		MIN_BTN_LEFT,			MIN_BTN_RIGHT,			YPL(15),		HPL(15),		STR_NORMAL,							STR_NONE },								// Normal staff


	{ WIDGETS_END },
};
static rct_widget window_cheats_rides_widgets[] = {
	MAIN_CHEATS_WIDGETS,
	{ WWT_CLOSEBOX,			1,		XPL(0),					WPL(0),					YPL(0),			HPL(0),			STR_CHEAT_RENEW_RIDES,				STR_NONE},								// Renew rides
	{ WWT_CLOSEBOX,			1,		XPL(1),					WPL(1),					YPL(1),			HPL(1),			STR_CHEAT_MAKE_DESTRUCTABLE,		STR_NONE},								// Make destructable
	{ WWT_CLOSEBOX,			1,		XPL(0),					WPL(0),					YPL(1),			HPL(1),			STR_CHEAT_FIX_ALL_RIDES,			STR_NONE },								// Fix all rides
	{ WWT_CHECKBOX,			2,		XPL(0),					OWPL,					YPL(11),		OHPL(11) + 8,	STR_CHEAT_410_HILL_LIFT,			STR_NONE }, 							// 410 km/h lift hill
	{ WWT_CHECKBOX,			2,		XPL(0),					OWPL,					YPL(9),			OHPL(9),		STR_CHEAT_DISABLE_BRAKES_FAILURE,	STR_NONE }, 							// Disable brakes failure
	{ WWT_CHECKBOX,			2,		XPL(0),					OWPL,					YPL(10),		OHPL(10),		STR_CHEAT_DISABLE_BREAKDOWNS,		STR_NONE }, 							// Disable all breakdowns
	{ WWT_CHECKBOX,			2,		XPL(0),					OWPL,					YPL(8),			OHPL(8),		STR_CHEAT_BUILD_IN_PAUSE_MODE,		STR_NONE }, 							// Build in pause mode
	{ WWT_CLOSEBOX,			1,		XPL(0),					WPL(0),					YPL(2),			HPL(2),			STR_CHEAT_RESET_CRASH_STATUS,		STR_NONE },								// Reset crash status
	{ WIDGETS_END },
};

static rct_widget *window_cheats_page_widgets[] = {
	window_cheats_money_widgets,
	window_cheats_guests_widgets,
	window_cheats_misc_widgets,
	window_cheats_rides_widgets,
};

static void window_cheats_money_mouseup(rct_window *w, int widgetIndex);
static void window_cheats_guests_mouseup(rct_window *w, int widgetIndex);
static void window_cheats_misc_mouseup(rct_window *w, int widgetIndex);
static void window_cheats_rides_mouseup(rct_window *w, int widgetIndex);
static void window_cheats_update(rct_window *w);
static void window_cheats_invalidate(rct_window *w);
static void window_cheats_paint(rct_window *w, rct_drawpixelinfo *dpi);
static void window_cheats_set_page(rct_window *w, int page);

static rct_window_event_list window_cheats_money_events = {
	NULL,
	window_cheats_money_mouseup,
	NULL,
	NULL,
	NULL,
	NULL,
	window_cheats_update,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	window_cheats_invalidate,
	window_cheats_paint,
	NULL
};

static rct_window_event_list window_cheats_guests_events = {
	NULL,
	window_cheats_guests_mouseup,
	NULL,
	NULL,
	NULL,
	NULL,
	window_cheats_update,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	window_cheats_invalidate,
	window_cheats_paint,
	NULL
};

static rct_window_event_list window_cheats_misc_events = {
	NULL,
	window_cheats_misc_mouseup,
	NULL,
	NULL,
	NULL,
	NULL,
	window_cheats_update,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	window_cheats_invalidate,
	window_cheats_paint,
	NULL
};

static rct_window_event_list window_cheats_rides_events = {
	NULL,
	window_cheats_rides_mouseup,
	NULL,
	NULL,
	NULL,
	NULL,
	window_cheats_update,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	window_cheats_invalidate,
	window_cheats_paint,
	NULL
};


static rct_window_event_list *window_cheats_page_events[] = {
	&window_cheats_money_events,
	&window_cheats_guests_events,
	&window_cheats_misc_events,
	&window_cheats_rides_events,
};

static uint64 window_cheats_page_enabled_widgets[] = {
	(1ULL << WIDX_CLOSE) | (1ULL << WIDX_TAB_1) | (1ULL << WIDX_TAB_2) | (1ULL << WIDX_TAB_3) | (1ULL << WIDX_TAB_4) | (1ULL << WIDX_HIGH_MONEY) | (1ULL << WIDX_CLEAR_LOAN),
	(1ULL << WIDX_CLOSE) | (1ULL << WIDX_TAB_1) | (1ULL << WIDX_TAB_2) | (1ULL << WIDX_TAB_3) | (1ULL << WIDX_TAB_4) | (1ULL << WIDX_GUEST_PARAMETERS_GROUP) | (1ULL << WIDX_GUEST_HAPPINESS_MAX)  | (1ULL << WIDX_GUEST_HAPPINESS_MIN) | (1ULL << WIDX_GUEST_ENERGY_MAX) | (1ULL << WIDX_GUEST_ENERGY_MIN) | (1ULL << WIDX_GUEST_HUNGER_MAX) | (1ULL << WIDX_GUEST_HUNGER_MIN) | (1ULL << WIDX_GUEST_THIRST_MAX) | (1ULL << WIDX_GUEST_THIRST_MIN) | (1ULL << WIDX_GUEST_NAUSEA_MAX) | (1ULL << WIDX_GUEST_NAUSEA_MIN) | (1ULL << WIDX_GUEST_NAUSEA_TOLERANCE_MAX) | (1ULL << WIDX_GUEST_NAUSEA_TOLERANCE_MIN) | (1ULL << WIDX_GUEST_BATHROOM_MAX) | (1ULL << WIDX_GUEST_BATHROOM_MIN) | (1ULL << WIDX_GUEST_RIDE_INTENSITY_MORE_THAN_1) | (1ULL << WIDX_GUEST_RIDE_INTENSITY_LESS_THAN_15) | (1ULL << WIDX_GUEST_IGNORE_RIDE_INTENSITY) | (1ULL << WIDX_GIVE_ALL_GUESTS_GROUP) | (1ULL << WIDX_GIVE_GUESTS_MONEY) | (1ULL << WIDX_GIVE_GUESTS_PARK_MAPS) | (1ULL << WIDX_GIVE_GUESTS_BALLOONS) | (1ULL << WIDX_GIVE_GUESTS_UMBRELLAS) | (1ULL << WIDX_TRAM_GUESTS) | (1ULL << WIDX_REMOVE_ALL_GUESTS) | (1ULL << WIDX_EXPLODE_GUESTS) | (1ULL << WIDX_DISABLE_VANDALISM),
	(1ULL << WIDX_CLOSE) | (1ULL << WIDX_TAB_1) | (1ULL << WIDX_TAB_2) | (1ULL << WIDX_TAB_3) | (1ULL << WIDX_TAB_4) | (1ULL << WIDX_FREEZE_CLIMATE) | (1ULL << WIDX_OPEN_CLOSE_PARK) | (1ULL << WIDX_WEATHER_SUN) | (1ULL << WIDX_WEATHER_THUNDER) | (1ULL << WIDX_CLEAR_GRASS) | (1ULL << WIDX_MOWED_GRASS) | (1ULL << WIDX_WATER_PLANTS) | (1ULL << WIDX_FIX_VANDALISM) | (1ULL << WIDX_REMOVE_LITTER) | (1ULL << WIDX_WIN_SCENARIO) | (1ULL << WIDX_HAVE_FUN) | (1ULL << WIDX_UNLOCK_ALL_PRICES) | (1ULL << WIDX_SANDBOX_MODE) | (1ULL << WIDX_FAST_STAFF) | (1ULL << WIDX_NORMAL_STAFF) | (1ULL << WIDX_PARK_PARAMETERS) | (1ULL << WIDX_FORCE_PARK_RATING) | (1ULL << WIDX_INCREASE_PARK_RATING) | (1ULL << WIDX_DECREASE_PARK_RATING),
	(1ULL << WIDX_CLOSE) | (1ULL << WIDX_TAB_1) | (1ULL << WIDX_TAB_2) | (1ULL << WIDX_TAB_3) | (1ULL << WIDX_TAB_4) | (1ULL << WIDX_RENEW_RIDES) | (1ULL << WIDX_MAKE_DESTRUCTIBLE) | (1ULL << WIDX_FIX_ALL) | (1ULL << WIDX_FAST_LIFT_HILL) | (1ULL << WIDX_DISABLE_BRAKES_FAILURE) | (1ULL << WIDX_DISABLE_ALL_BREAKDOWNS) | (1ULL << WIDX_BUILD_IN_PAUSE_MODE) | (1ULL << WIDX_RESET_CRASH_STATUS)
};

static rct_string_id window_cheats_page_titles[] = {
	STR_CHEAT_TITLE_FINANCIAL,
	STR_CHEAT_TITLE_GUEST,
	STR_CHEAT_TITLE_RIDE,
	STR_CHEAT_TITLE_PARK,
};

static void window_cheats_draw_tab_images(rct_drawpixelinfo *dpi, rct_window *w);

int park_rating_spinner_value;
int park_rating_spinner_pressed_for = 0;

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

	map_element_iterator it;
	rct_scenery_entry *sceneryEntry;

	map_element_iterator_begin(&it);
	do {
		if (map_element_get_type(it.element) != MAP_ELEMENT_TYPE_PATH)
			continue;

		if ((it.element->properties.path.additions & 0x0F) == 0)
			continue;

		sceneryEntry = g_pathBitSceneryEntries[(it.element->properties.path.additions & 0xF) - 1];
		if(sceneryEntry->path_bit.var_06 & (1 << 0))
			it.element->properties.path.addition_status = 0xFF;

	} while (map_element_iterator_next(&it));

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

			ride_fix_breakdown(rideIndex, 0);
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

static void cheat_make_destructible()
{
	int i;
	rct_ride *ride;
	FOR_ALL_RIDES(i, ride)
	{
		if (ride->lifecycle_flags & RIDE_LIFECYCLE_INDESTRUCTIBLE)
			ride->lifecycle_flags&=~RIDE_LIFECYCLE_INDESTRUCTIBLE;
		if (ride->lifecycle_flags & RIDE_LIFECYCLE_INDESTRUCTIBLE_TRACK)
			ride->lifecycle_flags&=~RIDE_LIFECYCLE_INDESTRUCTIBLE_TRACK;
	}
	window_invalidate_by_class(WC_RIDE);
}

static void cheat_reset_crash_status()
{
	int i;
	rct_ride *ride;

	FOR_ALL_RIDES(i, ride){
		//reset crash status
		if (ride->lifecycle_flags & RIDE_LIFECYCLE_CRASHED)
			ride->lifecycle_flags&=~RIDE_LIFECYCLE_CRASHED;
		//reset crash history
		ride->last_crash_type=RIDE_CRASH_TYPE_NONE;
	}
	window_invalidate_by_class(WC_RIDE);
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

static void cheat_clear_loan()
{
	// First give money
	cheat_increase_money(RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_LOAN, money32));

	// Then pay the loan
	money32 newLoan;
	newLoan = MONEY(0, 00);
	game_do_command(0, GAME_COMMAND_FLAG_APPLY, 0, newLoan, GAME_COMMAND_SET_CURRENT_LOAN, 0, 0);
}

static void cheat_generate_guests(int count)
{
	int i;

	for (i = 0; i < count; i++)
		generate_new_guest();

	window_invalidate_by_class(WC_BOTTOM_TOOLBAR);
}

static void cheat_set_guest_parameter(int parameter, int value)
{
	int spriteIndex;
	rct_peep *peep;

	FOR_ALL_GUESTS(spriteIndex, peep) {
		switch(parameter) {
			case GUEST_PARAMETER_HAPPINESS:
				peep->happiness = value;
				break;
			case GUEST_PARAMETER_ENERGY:
				peep->energy = value;
				break;
			case GUEST_PARAMETER_HUNGER:
				peep->hunger = value;
				break;
			case GUEST_PARAMETER_THIRST:
				peep->thirst = value;
				break;
			case GUEST_PARAMETER_NAUSEA:
				peep->nausea = value;
				break;
			case GUEST_PARAMETER_NAUSEA_TOLERANCE:
				peep->nausea_tolerance = value;
				break;
			case GUEST_PARAMETER_BATHROOM:
				peep->bathroom = value;
				break;
			case GUEST_PARAMETER_PREFERRED_RIDE_INTENSITY:
				peep->intensity = (15 << 4) | value;
				break;
		}
		peep_update_sprite_type(peep);
	}

}

static void cheat_give_all_guests(int object)
{
	int spriteIndex;
	rct_peep *peep;

	FOR_ALL_GUESTS(spriteIndex, peep) {
		switch(object)
		{
			case OBJECT_MONEY:
				peep->cash_in_pocket = MONEY(1000,00);
				break;
			case OBJECT_PARK_MAP:
				peep->item_standard_flags |= PEEP_ITEM_MAP;
				break;
			case OBJECT_BALLOON:
				peep->item_standard_flags |= PEEP_ITEM_BALLOON;
				peep->balloon_colour=scenario_rand_max(31);
				peep_update_sprite_type(peep);
				break;
			case OBJECT_UMBRELLA:
				peep->item_standard_flags |= PEEP_ITEM_UMBRELLA;
				peep->umbrella_colour=scenario_rand_max(31);
				peep_update_sprite_type(peep);
				break;
		}
	}
	window_invalidate_by_class(WC_PEEP);
}

static void cheat_remove_all_guests()
{
	rct_peep *peep;
	uint16 spriteIndex, nextSpriteIndex;

	for (spriteIndex = RCT2_GLOBAL(RCT2_ADDRESS_SPRITES_START_PEEP, uint16); spriteIndex != SPRITE_INDEX_NULL; spriteIndex = nextSpriteIndex) {
		peep = &(g_sprite_list[spriteIndex].peep);
		nextSpriteIndex = peep->next;
		if (peep->type == PEEP_TYPE_GUEST) {
			peep_remove(peep);
		}
	}

	int i;
	rct_ride *ride;

	FOR_ALL_RIDES(i, ride)
	{
		ride_clear_for_construction(i);
		ride_set_status(i, RIDE_STATUS_CLOSED);

		for(int i=0;i<4;i++) {
			ride->first_peep_in_queue[i]=0xFFFF;
		}
	}
	window_invalidate_by_class(WC_RIDE);
	gfx_invalidate_screen();
}

static void cheat_explode_guests()
{
	int sprite_index;
	rct_peep *peep;

	FOR_ALL_GUESTS(sprite_index, peep) {
		if (scenario_rand_max(6) == 0) {
			peep->flags |= PEEP_FLAGS_EXPLODE;
		}
	}
}

static void cheat_set_staff_speed(uint8 value)
{
	uint16 spriteIndex;
	rct_peep *peep;

	FOR_ALL_STAFF(spriteIndex, peep) {
		peep->energy = value;
		peep->energy_growth_rate = value;
	}
}

#pragma endregion

void window_cheats_open()
{
	rct_window* window;

	// Check if window is already open
	window = window_bring_to_front_by_class(WC_CHEATS);
	if (window != NULL)
		return;

	window = window_create(32, 32, WW, WH, &window_cheats_money_events, WC_CHEATS, 0);
	window->widgets = window_cheats_money_widgets;
	window->enabled_widgets = window_cheats_page_enabled_widgets[0];
	window_init_scroll_widgets(window);
	window_cheats_set_page(window, WINDOW_CHEATS_PAGE_MONEY);
	park_rating_spinner_value = get_forced_park_rating() >= 0 ? get_forced_park_rating() : 999;
}

static void window_cheats_money_mouseup(rct_window *w, int widgetIndex)
{
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
	case WIDX_CLEAR_LOAN:
		cheat_clear_loan();
		break;
	}
}

static void window_cheats_guests_mouseup(rct_window *w, int widgetIndex)
{
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
	case WIDX_GUEST_HAPPINESS_MAX:
		cheat_set_guest_parameter(GUEST_PARAMETER_HAPPINESS,255);
		break;
	case WIDX_GUEST_HAPPINESS_MIN:
		cheat_set_guest_parameter(GUEST_PARAMETER_HAPPINESS,0);
		break;
	case WIDX_GUEST_ENERGY_MAX:
		cheat_set_guest_parameter(GUEST_PARAMETER_ENERGY,127);
		break;
	case WIDX_GUEST_ENERGY_MIN:
		cheat_set_guest_parameter(GUEST_PARAMETER_ENERGY,0);
		break;
	case WIDX_GUEST_HUNGER_MAX:
		cheat_set_guest_parameter(GUEST_PARAMETER_HUNGER,0);
		break;
	case WIDX_GUEST_HUNGER_MIN:
		cheat_set_guest_parameter(GUEST_PARAMETER_HUNGER,255);
		break;
	case WIDX_GUEST_THIRST_MAX:
		cheat_set_guest_parameter(GUEST_PARAMETER_THIRST,0);
		break;
	case WIDX_GUEST_THIRST_MIN:
		cheat_set_guest_parameter(GUEST_PARAMETER_THIRST,255);
		break;
	case WIDX_GUEST_NAUSEA_MAX:
		cheat_set_guest_parameter(GUEST_PARAMETER_NAUSEA,255);
		break;
	case WIDX_GUEST_NAUSEA_MIN:
		cheat_set_guest_parameter(GUEST_PARAMETER_NAUSEA,0);
		break;
	case WIDX_GUEST_NAUSEA_TOLERANCE_MAX:
		cheat_set_guest_parameter(GUEST_PARAMETER_NAUSEA_TOLERANCE,PEEP_NAUSEA_TOLERANCE_HIGH);
		break;
	case WIDX_GUEST_NAUSEA_TOLERANCE_MIN:
		cheat_set_guest_parameter(GUEST_PARAMETER_NAUSEA_TOLERANCE,PEEP_NAUSEA_TOLERANCE_NONE);
		break;
	case WIDX_GUEST_BATHROOM_MAX:
		cheat_set_guest_parameter(GUEST_PARAMETER_BATHROOM,255);
		break;
	case WIDX_GUEST_BATHROOM_MIN:
		cheat_set_guest_parameter(GUEST_PARAMETER_BATHROOM,0);
		break;
	case WIDX_GUEST_RIDE_INTENSITY_MORE_THAN_1:
		cheat_set_guest_parameter(GUEST_PARAMETER_PREFERRED_RIDE_INTENSITY,1);
		break;
	case WIDX_GUEST_RIDE_INTENSITY_LESS_THAN_15:
		cheat_set_guest_parameter(GUEST_PARAMETER_PREFERRED_RIDE_INTENSITY,0);
		break;
	case WIDX_TRAM_GUESTS:
		cheat_generate_guests(CHEATS_TRAM_INCREMENT);
		break;
	case WIDX_REMOVE_ALL_GUESTS:
		cheat_remove_all_guests();
		break;
	case WIDX_EXPLODE_GUESTS:
		cheat_explode_guests();
		break;
	case WIDX_GIVE_GUESTS_MONEY:
		cheat_give_all_guests(OBJECT_MONEY);
		break;
	case WIDX_GIVE_GUESTS_PARK_MAPS:
		cheat_give_all_guests(OBJECT_PARK_MAP);
		break;
	case WIDX_GIVE_GUESTS_BALLOONS:
		cheat_give_all_guests(OBJECT_BALLOON);
		break;
	case WIDX_GIVE_GUESTS_UMBRELLAS:
		cheat_give_all_guests(OBJECT_UMBRELLA);
		break;
	case WIDX_GUEST_IGNORE_RIDE_INTENSITY:
		gConfigCheat.ignore_ride_intensity ^= 1;
		config_save_default();
		window_invalidate(w);
		break;
	case WIDX_DISABLE_VANDALISM:
		gConfigCheat.disable_vandalism ^= 1;
		config_save_default();
		window_invalidate(w);
		break;
	}
}

static void window_cheats_misc_mouseup(rct_window *w, int widgetIndex)
{
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
	case WIDX_HAVE_FUN:
		RCT2_GLOBAL(RCT2_ADDRESS_OBJECTIVE_TYPE, uint8) = OBJECTIVE_HAVE_FUN;
		break;
	case WIDX_UNLOCK_ALL_PRICES:
		gConfigCheat.unlock_all_prices ^= 1;
		config_save_default();
		window_invalidate(w);
		window_invalidate_by_class(WC_RIDE);
		window_invalidate_by_class(WC_PARK_INFORMATION);
		break;
	case WIDX_SANDBOX_MODE:
		gCheatsSandboxMode = !gCheatsSandboxMode;
		w->widgets[widgetIndex].image = w->widgets[widgetIndex].image == STR_CHEAT_SANDBOX_MODE ? STR_CHEAT_SANDBOX_MODE_DISABLE : STR_CHEAT_SANDBOX_MODE;
		// To prevent tools from staying active after disabling cheat
		tool_cancel();
		window_invalidate_by_class(WC_MAP);
		window_invalidate_by_class(WC_FOOTPATH);
		break;
	case WIDX_FAST_STAFF:
		cheat_set_staff_speed(0xFF);
		break;
	case WIDX_NORMAL_STAFF:
		cheat_set_staff_speed(0x60);
		break;
	case WIDX_PARK_PARAMETERS:
		window_editor_scenario_options_open();
		break;
	case WIDX_FORCE_PARK_RATING:
		if (get_forced_park_rating() >= 0){
			set_forced_park_rating(-1);
		} else {
			set_forced_park_rating(park_rating_spinner_value);
		}
		break;
	case WIDX_INCREASE_PARK_RATING:
		park_rating_spinner_value = min(999, 10 * (park_rating_spinner_value / 10 + 1));
		widget_invalidate_by_class(WC_CHEATS, WIDX_PARK_RATING_SPINNER);
		if (get_forced_park_rating() >= 0)
			set_forced_park_rating(park_rating_spinner_value);
		break;
	case WIDX_DECREASE_PARK_RATING:
		park_rating_spinner_value = max(0, 10 * (park_rating_spinner_value / 10 - 1));
		widget_invalidate_by_class(WC_CHEATS, WIDX_PARK_RATING_SPINNER);
		if (get_forced_park_rating() >= 0)
			set_forced_park_rating(park_rating_spinner_value);
		break;
	}
}

static void window_cheats_rides_mouseup(rct_window *w, int widgetIndex)
{
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
	case WIDX_BUILD_IN_PAUSE_MODE:
		gConfigCheat.build_in_pause_mode ^= 1;
		config_save_default();
		window_invalidate(w);
		break;
	case WIDX_RESET_CRASH_STATUS:
		cheat_reset_crash_status();
		break;
	}
}

static void window_cheats_update(rct_window *w)
{
	w->frame_no++;
	widget_invalidate(w, WIDX_TAB_1 + w->page);

	if (widget_is_pressed(w, WIDX_INCREASE_PARK_RATING) || widget_is_pressed(w, WIDX_DECREASE_PARK_RATING))
		park_rating_spinner_pressed_for++;
	else
		park_rating_spinner_pressed_for = 0;
	if (park_rating_spinner_pressed_for >= CHEATS_PARK_RATING_SPINNER_PAUSE)
		if (!(w->frame_no % 3)){
			if (widget_is_pressed(w, WIDX_INCREASE_PARK_RATING)){
				park_rating_spinner_value = min(999, 10 * (park_rating_spinner_value / 10 + 1));
				widget_invalidate_by_class(WC_CHEATS, WIDX_PARK_RATING_SPINNER);
				if (get_forced_park_rating() >= 0)
					set_forced_park_rating(park_rating_spinner_value);
			} else if (widget_is_pressed(w, WIDX_DECREASE_PARK_RATING)){
				park_rating_spinner_value = max(0, 10 * (park_rating_spinner_value / 10 - 1));
				widget_invalidate_by_class(WC_CHEATS, WIDX_PARK_RATING_SPINNER);
				if (get_forced_park_rating() >= 0)
					set_forced_park_rating(park_rating_spinner_value);
			}
		}
}

static void window_cheats_invalidate(rct_window *w)
{
	int i;

	colour_scheme_update(w);

	rct_widget *widgets = window_cheats_page_widgets[w->page];
	if (w->widgets != widgets) {
		w->widgets = widgets;
		window_init_scroll_widgets(w);
	}

	w->pressed_widgets = 0;

	switch (w->page) {
	case WINDOW_CHEATS_PAGE_MONEY:
		RCT2_GLOBAL(0x013CE952, int) = 50000;
		break;
	case WINDOW_CHEATS_PAGE_GUESTS:
		RCT2_GLOBAL(0x013CE952, int) = 10000;
		widget_set_checkbox_value(w, WIDX_GUEST_IGNORE_RIDE_INTENSITY, gConfigCheat.ignore_ride_intensity);
		widget_set_checkbox_value(w, WIDX_DISABLE_VANDALISM, gConfigCheat.disable_vandalism);
		break;
	case WINDOW_CHEATS_PAGE_MISC:
		w->widgets[WIDX_OPEN_CLOSE_PARK].image = RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & PARK_FLAGS_PARK_OPEN ?
			STR_CHEAT_CLOSE_PARK : STR_CHEAT_OPEN_PARK;
		widget_set_checkbox_value(w, WIDX_UNLOCK_ALL_PRICES, gConfigCheat.unlock_all_prices);
		widget_set_checkbox_value(w, WIDX_FORCE_PARK_RATING, get_forced_park_rating() >= 0);
		break;
	case WINDOW_CHEATS_PAGE_RIDES:
		RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS + 0, uint16) = 255;
		widget_set_checkbox_value(w, WIDX_FAST_LIFT_HILL, gConfigCheat.fast_lift_hill);
		widget_set_checkbox_value(w, WIDX_DISABLE_BRAKES_FAILURE, gConfigCheat.disable_brakes_failure);
		widget_set_checkbox_value(w, WIDX_DISABLE_ALL_BREAKDOWNS, gConfigCheat.disable_all_breakdowns);
		widget_set_checkbox_value(w, WIDX_BUILD_IN_PAUSE_MODE, gConfigCheat.build_in_pause_mode);
		break;
	}

	// Set correct active tab
	for (i = 0; i < 7; i++)
		w->pressed_widgets &= ~(1 << (WIDX_TAB_1 + i));
	w->pressed_widgets |= 1LL << (WIDX_TAB_1 + w->page);

	// Set title
	w->widgets[WIDX_TITLE].image = window_cheats_page_titles[w->page];
}

static void window_cheats_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
	window_draw_widgets(w, dpi);
	window_cheats_draw_tab_images(dpi, w);

	if (w->page == WINDOW_CHEATS_PAGE_MONEY){
		RCT2_GLOBAL(0x0013CE952, money32) = CHEATS_MONEY_INCREMENT;
		gfx_draw_string_left(dpi, STR_CHEAT_TIP_5K_MONEY,	(void*)0x0013CE952,	0, w->x + XPL(0) + TXTO, w->y + YPL(0) + TXTO);
		gfx_draw_string_left(dpi, STR_CHEAT_TIP_CLEAR_LOAN,	NULL,				0, w->x + XPL(0) + TXTO, w->y + YPL(4) + TXTO);
	}
	else if(w->page == WINDOW_CHEATS_PAGE_MISC){
		gfx_draw_string_left(dpi, STR_CHEAT_STAFF_SPEED,			NULL,	0, w->x + XPL(0) + TXTO, w->y + YPL(15) + TXTO);
		gfx_draw_string_right(dpi, 5182,		&park_rating_spinner_value,	w->colours[2], w->x + WPL(1) - 10 - TXTO, w->y + YPL(4) + TXTO);
	}
	else if (w->page == WINDOW_CHEATS_PAGE_GUESTS){
		gfx_draw_string_left(dpi, STR_CHEAT_GUEST_HAPPINESS,		NULL,	0, w->x + XPL(0) + TXTO, w->y + YPL(1) + TXTO);
		gfx_draw_string_left(dpi, STR_CHEAT_GUEST_ENERGY,			NULL,	0, w->x + XPL(0) + TXTO, w->y + YPL(2) + TXTO);
		gfx_draw_string_left(dpi, STR_CHEAT_GUEST_HUNGER,			NULL,	0, w->x + XPL(0) + TXTO, w->y + YPL(3) + TXTO);
		gfx_draw_string_left(dpi, STR_CHEAT_GUEST_THIRST,			NULL,	0, w->x + XPL(0) + TXTO, w->y + YPL(4) + TXTO);
		gfx_draw_string_left(dpi, STR_CHEAT_GUEST_NAUSEA,			NULL,	0, w->x + XPL(0) + TXTO, w->y + YPL(5) + TXTO);
		gfx_draw_string_left(dpi, STR_CHEAT_GUEST_NAUSEA_TOLERANCE,	NULL,	0, w->x + XPL(0) + TXTO, w->y + YPL(6) + TXTO);
		gfx_draw_string_left(dpi, STR_CHEAT_GUEST_BATHROOM,			NULL,	0, w->x + XPL(0) + TXTO, w->y + YPL(7) + TXTO);
		gfx_draw_string_left(dpi, STR_CHEAT_GUEST_PREFERRED_INTENSITY,NULL,	0, w->x + XPL(0) + TXTO, w->y + YPL(8) + TXTO);
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

	int maxY = 0;
	rct_widget *widget = &w->widgets[8];
	while (widget->type != WWT_LAST) {
		maxY = max(maxY, widget->bottom);
		widget++;
	}
	maxY += 6;

	window_invalidate(w);
	w->height = maxY;
	w->widgets[WIDX_BACKGROUND].bottom = maxY - 1;
	w->widgets[WIDX_PAGE_BACKGROUND].bottom = maxY - 1;
	window_invalidate(w);
}
