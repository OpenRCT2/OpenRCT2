#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#include "../config/Config.h"
#include "../game.h"
#include "../interface/widget.h"
#include "../interface/window.h"
#include "../localisation/localisation.h"
#include "../world/park.h"
#include "../peep/peep.h"
#include "../ride/ride.h"
#include "../scenario/scenario.h"
#include "../sprites.h"
#include "../world/Climate.h"
#include "../world/park.h"
#include "../world/sprite.h"
#include "../interface/themes.h"
#include "../cheats.h"
#include "../network/network.h"
#include "../management/marketing.h"
#include "../util/util.h"
#include "error.h"
#include "dropdown.h"

#define MONEY_DEFAULT MONEY(5000,00)
#define MONEY_INCREMENT_DIV MONEY(1000,00)
#define MONEY_DIGITS 14
static utf8 _moneySpinnerText[MONEY_DIGITS];
static money32 _moneySpinnerValue = MONEY_DEFAULT;

enum {
	WINDOW_CHEATS_PAGE_MONEY,
	WINDOW_CHEATS_PAGE_GUESTS,
	WINDOW_CHEATS_PAGE_MISC,
	WINDOW_CHEATS_PAGE_RIDES,
};

static const rct_string_id WeatherTypes[] = {
	STR_SUNNY,
	STR_PARTIALLY_CLOUDY,
	STR_CLOUDY,
	STR_RAIN,
	STR_HEAVY_RAIN,
	STR_THUNDERSTORM,
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
	WIDX_TAB_CONTENT,

	WIDX_NO_MONEY = WIDX_TAB_CONTENT,
	WIDX_ADD_SET_MONEY_GROUP,
	WIDX_MONEY_SPINNER,
	WIDX_MONEY_SPINNER_INCREMENT,
	WIDX_MONEY_SPINNER_DECREMENT,
	WIDX_ADD_MONEY,
	WIDX_SET_MONEY,
	WIDX_CLEAR_LOAN,

	WIDX_GUEST_PARAMETERS_GROUP = WIDX_TAB_CONTENT,
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
	WIDX_DISABLE_LITTERING,
	WIDX_GIVE_ALL_GUESTS_GROUP,
	WIDX_GIVE_GUESTS_MONEY,
	WIDX_GIVE_GUESTS_PARK_MAPS,
	WIDX_GIVE_GUESTS_BALLOONS,
	WIDX_GIVE_GUESTS_UMBRELLAS,
	WIDX_TRAM_GUESTS,
	WIDX_REMOVE_ALL_GUESTS,
	WIDX_EXPLODE_GUESTS,

	WIDX_GENERAL_GROUP = WIDX_TAB_CONTENT,
	WIDX_OPEN_CLOSE_PARK,
	WIDX_PARK_PARAMETERS,
	WIDX_SANDBOX_MODE,
	WIDX_RESET_DATE,
	WIDX_UNLOCK_ALL_PRICES,
	WIDX_FORCE_PARK_RATING,
	WIDX_PARK_RATING_SPINNER,
	WIDX_INCREASE_PARK_RATING,
	WIDX_DECREASE_PARK_RATING,
	WIDX_WIN_SCENARIO,
	WIDX_HAVE_FUN,
	WIDX_NEVERENDING_MARKETING,
	WIDX_CLIMATE_GROUP,
	WIDX_FREEZE_CLIMATE,
	WIDX_WEATHER,
	WIDX_WEATHER_DROPDOWN_BUTTON,
	WIDX_STAFF_GROUP,
	WIDX_CLEAR_GRASS,
	WIDX_MOWED_GRASS,
	WIDX_WATER_PLANTS,
	WIDX_FIX_VANDALISM,
	WIDX_REMOVE_LITTER,
	WIDX_DISABLE_PLANT_AGING,
	WIDX_FAST_STAFF,
	WIDX_NORMAL_STAFF,

	WIDX_RENEW_RIDES = WIDX_TAB_CONTENT,
	WIDX_MAKE_DESTRUCTIBLE,
	WIDX_FIX_ALL,
	WIDX_FAST_LIFT_HILL,
	WIDX_DISABLE_BRAKES_FAILURE,
	WIDX_DISABLE_ALL_BREAKDOWNS,
	WIDX_BUILD_IN_PAUSE_MODE,
	WIDX_RESET_CRASH_STATUS,
	WIDX_10_MINUTE_INSPECTIONS,
	WIDX_SHOW_ALL_OPERATING_MODES,
	WIDX_SHOW_VEHICLES_FROM_OTHER_TRACK_TYPES,
	WIDX_DISABLE_TRAIN_LENGTH_LIMITS,
	WIDX_ENABLE_CHAIN_LIFT_ON_ALL_TRACK,
	WIDX_ENABLE_ARBITRARY_RIDE_TYPE_CHANGES,
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
#define OPTW			220												// Option (checkbox) width (two columns)
#define OPTH			10												// Option (checkbox) height (two columns)
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
	{ WWT_FRAME,			0,	0,			WW - 1,	0,		WH - 1,		0xFFFFFFFF,				STR_NONE },					/* panel / background	*/ \
	{ WWT_CAPTION,			0,	1,			WW - 2,	1,		14,			STR_CHEAT_TITLE,		STR_WINDOW_TITLE_TIP },		/* title bar			*/ \
	{ WWT_CLOSEBOX,			0,	WW - 13,	WW - 3,	2,		13,			STR_CLOSE_X,			STR_CLOSE_WINDOW_TIP },		/* close x button		*/ \
	{ WWT_IMGBTN,			1,	0,			WW - 1,	43,		WH - 1,		0xFFFFFFFF,				STR_NONE },					/* tab content panel	*/ \
	{ WWT_TAB,				1,	3,			33,		17,		43,			0x20000000 | SPR_TAB,	STR_FINANCIAL_CHEATS_TIP },	/* tab 1				*/ \
	{ WWT_TAB,				1,	34,			64,		17,		43,			0x20000000 | SPR_TAB,	STR_GUEST_CHEATS_TIP },		/* tab 2				*/ \
	{ WWT_TAB,				1,	65,			95,		17,		43,			0x20000000 | SPR_TAB,	STR_PARK_CHEATS_TIP },		/* tab 3				*/ \
	{ WWT_TAB,				1,	96,			126,	17,		43,			0x20000000 | SPR_TAB,	STR_RIDE_CHEATS_TIP }		/* tab 4				*/

static rct_widget window_cheats_money_widgets[] = {
	MAIN_CHEATS_WIDGETS,
	{ WWT_CHECKBOX,			1,		XPL(0),					WPL(0),					YPL(0),			HPL(0),			STR_MAKE_PARK_NO_MONEY,				STR_NONE },								// No money
	{ WWT_GROUPBOX,			1,		XPL(0) - GROUP_SPACE,	WPL(1) + GROUP_SPACE,	YPL(1),			HPL(3.5),		STR_ADD_SET_MONEY,					STR_NONE },								// add / set money group frame
	{ WWT_SPINNER,			1,		XPL(0),					WPL(1) - 10,			YPL(2) + 2,		HPL(2) - 3,		STR_NONE,							STR_NONE },								// money value
	{ WWT_DROPDOWN_BUTTON,	1,		WPL(1) - 10,			WPL(1),					YPL(2) + 3,		YPL(2) + 7,		STR_NUMERIC_UP,						STR_NONE },								// increase money
	{ WWT_DROPDOWN_BUTTON,	1,		WPL(1) - 10,			WPL(1),					YPL(2) + 8,		YPL(2) + 12,	STR_NUMERIC_DOWN,					STR_NONE },								// decrease money
	{ WWT_CLOSEBOX,			1,		XPL(0),					WPL(0),					YPL(3),			HPL(3),			STR_ADD_MONEY,						STR_NONE },								// add money
	{ WWT_CLOSEBOX,			1,		XPL(1),					WPL(1),					YPL(3),			HPL(3),			STR_SET_MONEY,						STR_NONE },								// set money
	{ WWT_CLOSEBOX,			1,		XPL(0),					WPL(0),					YPL(5),			HPL(5),			STR_CHEAT_CLEAR_LOAN,				STR_NONE },								// Clear loan
  	{ WIDGETS_END },
};

static rct_widget window_cheats_guests_widgets[] = {
	MAIN_CHEATS_WIDGETS,
	{ WWT_GROUPBOX,			1,		XPL(0) - GROUP_SPACE,	WPL(1) + GROUP_SPACE,	YPL(0),			HPL(12.5),		STR_CHEAT_SET_GUESTS_PARAMETERS,	STR_NONE },								// Guests parameters group frame
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
	{ WWT_CHECKBOX,			1,		XPL(0),					OWPL, 					YPL(10),		OHPL(10),		STR_CHEAT_IGNORE_INTENSITY,			STR_CHEAT_IGNORE_INTENSITY_TIP },		// guests ignore intensity
	{ WWT_CHECKBOX,			1,		XPL(0),					OWPL, 					YPL(11),		OHPL(11),		STR_CHEAT_DISABLE_VANDALISM,		STR_CHEAT_DISABLE_VANDALISM_TIP },		// disable vandalism
	{ WWT_CHECKBOX,			1,		XPL(0),					OWPL,					YPL(12),		OHPL(12),		STR_CHEAT_DISABLE_LITTERING,		STR_CHEAT_DISABLE_LITTERING_TIP },		// disable littering
	{ WWT_GROUPBOX,			1,		XPL(0) - GROUP_SPACE,	WPL(1) + GROUP_SPACE,	YPL(14),		HPL(16.5),		STR_CHEAT_GIVE_ALL_GUESTS,			STR_NONE },								// Guests parameters group frame
	{ WWT_CLOSEBOX,			1,		XPL(0),					WPL(0),					YPL(15),		HPL(15),		STR_CHEAT_CURRENCY_FORMAT,			STR_NONE },								// give guests money
	{ WWT_CLOSEBOX,			1,		XPL(1),					WPL(1),					YPL(15),		HPL(15),		STR_SHOP_ITEM_PLURAL_PARK_MAP,		STR_NONE },								// give guests park maps
	{ WWT_CLOSEBOX,			1,		XPL(0),					WPL(0),					YPL(16),		HPL(16),		STR_SHOP_ITEM_PLURAL_BALLOON,		STR_NONE },								// give guests balloons
	{ WWT_CLOSEBOX,			1,		XPL(1),					WPL(1),					YPL(16),		HPL(16),		STR_SHOP_ITEM_PLURAL_UMBRELLA,		STR_NONE },								// give guests umbrellas
	{ WWT_CLOSEBOX,			1,		XPL(0),					WPL(0),					YPL(18),		HPL(18),		STR_CHEAT_LARGE_TRAM_GUESTS,		STR_CHEAT_LARGE_TRAM_GUESTS_TIP },		// large tram
	{ WWT_CLOSEBOX,			1,		XPL(1),					WPL(1),					YPL(18),		HPL(18),		STR_CHEAT_REMOVE_ALL_GUESTS,		STR_CHEAT_REMOVE_ALL_GUESTS_TIP },		// remove all guests
	{ WWT_CLOSEBOX,			1,		XPL(0),					WPL(0),					YPL(19),		HPL(19),		STR_CHEAT_EXPLODE,					STR_CHEAT_EXPLODE_TIP },				// explode guests
	{ WIDGETS_END },
};

//Strings for following moved to window_cheats_paint()
static rct_widget window_cheats_misc_widgets[] = {
	MAIN_CHEATS_WIDGETS,
	{ WWT_GROUPBOX,			1,		XPL(0) - GROUP_SPACE,	WPL(1) + GROUP_SPACE,	YPL(0),			HPL(6.25),		STR_CHEAT_GENERAL_GROUP,			STR_NONE },								// General group
	{ WWT_CLOSEBOX,			1,		XPL(0),					WPL(0),					YPL(1),			HPL(1),			STR_CHEAT_OPEN_PARK,				STR_CHEAT_OPEN_PARK_TIP },				// open / close park
	{ WWT_CLOSEBOX,			1,		XPL(1),					WPL(1),					YPL(1),			HPL(1),			STR_CHEAT_PARK_PARAMETERS,			STR_CHEAT_PARK_PARAMETERS_TIP },		// Park parameters
	{ WWT_CLOSEBOX,			1,		XPL(0),					WPL(0),					YPL(2),			HPL(2),			STR_CHEAT_SANDBOX_MODE,				STR_CHEAT_SANDBOX_MODE_TIP },			// Sandbox mode (edit land ownership in-game)
	{ WWT_CLOSEBOX,			1,		XPL(1),					WPL(1),					YPL(2),			HPL(2),			STR_CHEAT_RESET_DATE,				STR_NONE },								// Reset date
	{ WWT_CHECKBOX,			1,		XPL(0),					OWPL,					YPL(3),			OHPL(3),		STR_CHEAT_UNLOCK_PRICES,			STR_CHEAT_UNLOCK_PRICES_TIP },			// Unlock all prices
	{ WWT_CHECKBOX,			1,		XPL(0),					WPL(0),					YPL(4),			HPL(4),			STR_FORCE_PARK_RATING,				STR_NONE },								// Force park rating
	{ WWT_SPINNER,			1,		XPL(1),					WPL(1) - 10,			YPL(4) + 2,		HPL(4) - 3,		STR_NONE,							STR_NONE },								// park rating
	{ WWT_DROPDOWN_BUTTON,	1,		WPL(1) - 10,			WPL(1),					YPL(4) + 3,		YPL(4) + 7,		STR_NUMERIC_UP,						STR_NONE },								// increase rating
	{ WWT_DROPDOWN_BUTTON,	1,		WPL(1) - 10,			WPL(1),					YPL(4) + 8,		YPL(4) + 12,	STR_NUMERIC_DOWN,					STR_NONE },								// decrease rating
	{ WWT_CLOSEBOX,			1,		XPL(0),					WPL(0),					YPL(5),			HPL(5),			STR_CHEAT_WIN_SCENARIO,				STR_NONE },								// Win scenario
	{ WWT_CLOSEBOX,			1,		XPL(1),					WPL(1),					YPL(5),			HPL(5),			STR_CHEAT_HAVE_FUN,					STR_NONE },								// Have fun!
	{ WWT_CHECKBOX,			1,		XPL(0),					OWPL,					YPL(6),			HPL(6),			STR_CHEAT_NEVERENDING_MARKETING,	STR_CHEAT_NEVERENDING_MARKETING_TIP },	// never ending marketing campaigns
	{ WWT_GROUPBOX,			1,		XPL(0) - GROUP_SPACE,	WPL(1) + GROUP_SPACE,	YPL(7.25),		HPL(9.5),		STR_CHEAT_CLIMATE_GROUP,			STR_NONE },								// Climate group
	{ WWT_CLOSEBOX,			1,		XPL(0),					WPL(0),					YPL(8),			HPL(8),			STR_CHEAT_FREEZE_CLIMATE,			STR_CHEAT_FREEZE_CLIMATE_TIP },			// Freeze climate
	{ WWT_DROPDOWN,			1,		XPL(1),					WPL(1),					YPL(9) + 2,		YPL(9) + 13,	STR_NONE,							STR_FORCE_WEATHER_TOOLTIP },			// Force weather
	{ WWT_DROPDOWN_BUTTON,	1,		WPL(1) - 11,			WPL(1) - 1,				YPL(9) + 3,		YPL(9) + 12,	STR_DROPDOWN_GLYPH,					STR_FORCE_WEATHER_TOOLTIP },			// Force weather
	{ WWT_GROUPBOX,			1,		XPL(0) - GROUP_SPACE,	WPL(1) + GROUP_SPACE,	YPL(11),		HPL(16.5),		STR_CHEAT_STAFF_GROUP,				STR_NONE },								// Staff group
	{ WWT_CLOSEBOX,			1,		XPL(0),					WPL(0),					YPL(12),		HPL(12),		STR_CHEAT_CLEAR_GRASS,				STR_NONE },								// Clear grass
	{ WWT_CLOSEBOX,			1,		XPL(1),					WPL(1),					YPL(12),		HPL(12),		STR_CHEAT_MOWED_GRASS,				STR_NONE },								// Mowed grass
	{ WWT_CLOSEBOX,			1,		XPL(0),					WPL(0),					YPL(13),		HPL(13),		STR_CHEAT_WATER_PLANTS,				STR_NONE },								// Water plants
	{ WWT_CLOSEBOX,			1,		XPL(1),					WPL(1),					YPL(13),		HPL(13),		STR_CHEAT_FIX_VANDALISM,			STR_NONE },								// Fix vandalism
	{ WWT_CLOSEBOX,			1,		XPL(0),					WPL(0),					YPL(14),		HPL(14),		STR_CHEAT_REMOVE_LITTER,			STR_NONE },								// Remove litter
	{ WWT_CHECKBOX,			1,		XPL(0),					WPL(0),					YPL(15),		HPL(15),		STR_CHEAT_DISABLE_PLANT_AGING,		STR_CHEAT_DISABLE_PLANT_AGING_TIP },	// Disable plant aging
	{ WWT_CLOSEBOX,			1,		MAX_BTN_LEFT,			MAX_BTN_RIGHT,			YPL(16),		HPL(16),		STR_FAST,							STR_NONE },								// Fast staff
	{ WWT_CLOSEBOX,			1,		MIN_BTN_LEFT,			MIN_BTN_RIGHT,			YPL(16),		HPL(16),		STR_NORMAL,							STR_NONE },								// Normal staff
	{ WIDGETS_END },
};
static rct_widget window_cheats_rides_widgets[] = {
	MAIN_CHEATS_WIDGETS,
	{ WWT_CLOSEBOX,			1,		XPL(0),					WPL(0),					YPL(0),			HPL(0),			STR_CHEAT_RENEW_RIDES,				STR_CHEAT_RENEW_RIDES_TIP },			// Renew rides
	{ WWT_CLOSEBOX,			1,		XPL(1),					WPL(1),					YPL(1),			HPL(1),			STR_CHEAT_MAKE_DESTRUCTABLE,		STR_CHEAT_MAKE_DESTRUCTABLE_TIP },		// All destructible
	{ WWT_CLOSEBOX,			1,		XPL(0),					WPL(0),					YPL(1),			HPL(1),			STR_CHEAT_FIX_ALL_RIDES,			STR_CHEAT_FIX_ALL_RIDES_TIP },			// Fix all rides
	{ WWT_CHECKBOX,			1,		XPL(0),					OWPL,					YPL(11),		OHPL(11),		STR_CHEAT_UNLOCK_OPERATING_LIMITS,	STR_CHEAT_UNLOCK_OPERATING_LIMITS_TIP },// 410 km/h lift hill etc.
	{ WWT_CHECKBOX,			1,		XPL(0),					OWPL,					YPL(9),			OHPL(9),		STR_CHEAT_DISABLE_BRAKES_FAILURE,	STR_CHEAT_DISABLE_BRAKES_FAILURE_TIP }, // Disable brakes failure
	{ WWT_CHECKBOX,			1,		XPL(0),					OWPL,					YPL(10),		OHPL(10),		STR_CHEAT_DISABLE_BREAKDOWNS,		STR_CHEAT_DISABLE_BREAKDOWNS_TIP }, 	// Disable all breakdowns
	{ WWT_CHECKBOX,			1,		XPL(0),					OWPL,					YPL(8),			OHPL(8),		STR_CHEAT_BUILD_IN_PAUSE_MODE,		STR_CHEAT_BUILD_IN_PAUSE_MODE_TIP }, 	// Build in pause mode
	{ WWT_CLOSEBOX,			1,		XPL(0),					WPL(0),					YPL(2),			HPL(2),			STR_CHEAT_RESET_CRASH_STATUS,		STR_CHEAT_RESET_CRASH_STATUS_TIP },		// Reset crash status
	{ WWT_CLOSEBOX,			1,		XPL(1),					WPL(1),					YPL(2),			HPL(2),			STR_CHEAT_10_MINUTE_INSPECTIONS,	STR_CHEAT_10_MINUTE_INSPECTIONS_TIP },	// 10 minute inspections
	{ WWT_CHECKBOX,			1,		XPL(0),					OWPL,					YPL(7),			OHPL(7),		STR_CHEAT_SHOW_ALL_OPERATING_MODES,	STR_NONE }, 							// Show all operating modes
	{ WWT_CHECKBOX,			1,		XPL(0),					OWPL,					YPL(6),			OHPL(6),		STR_CHEAT_SHOW_VEHICLES_FROM_OTHER_TRACK_TYPES,	STR_NONE }, 				// Show vehicles from other track types
	{ WWT_CHECKBOX,			1,		XPL(0),					OWPL,					YPL(12),		OHPL(12),		STR_CHEAT_DISABLE_TRAIN_LENGTH_LIMIT,	STR_CHEAT_DISABLE_TRAIN_LENGTH_LIMIT_TIP },	// Disable train length limits
	{ WWT_CHECKBOX,			1,		XPL(0),					OWPL,					YPL(13),		OHPL(13),		STR_CHEAT_ENABLE_CHAIN_LIFT_ON_ALL_TRACK,	STR_CHEAT_ENABLE_CHAIN_LIFT_ON_ALL_TRACK_TIP },	// Enable chain lift on all track
	{ WWT_CHECKBOX,			1,		XPL(0),					OWPL,					YPL(14),		OHPL(14),		STR_CHEAT_ALLOW_ARBITRARY_RIDE_TYPE_CHANGES,	STR_CHEAT_ALLOW_ARBITRARY_RIDE_TYPE_CHANGES_TIP },	// Allow arbitrary ride type changes
	{ WIDGETS_END },
};

static rct_widget *window_cheats_page_widgets[] = {
	window_cheats_money_widgets,
	window_cheats_guests_widgets,
	window_cheats_misc_widgets,
	window_cheats_rides_widgets,
};

static void window_cheats_money_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_cheats_money_mousedown(rct_widgetindex widgetIndex, rct_window *w, rct_widget* widget);
static void window_cheats_misc_mousedown(rct_widgetindex widgetIndex, rct_window *w, rct_widget* widget);
static void window_cheats_misc_dropdown(rct_window *w, rct_widgetindex widgetIndex, sint32 dropdownIndex);
static void window_cheats_guests_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_cheats_misc_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_cheats_rides_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_cheats_update(rct_window *w);
static void window_cheats_invalidate(rct_window *w);
static void window_cheats_paint(rct_window *w, rct_drawpixelinfo *dpi);
static void window_cheats_set_page(rct_window *w, sint32 page);
static void window_cheats_text_input(rct_window *w, rct_widgetindex widgetIndex, char *text);

static rct_window_event_list window_cheats_money_events = {
	NULL,
	window_cheats_money_mouseup,
	NULL,
	window_cheats_money_mousedown,
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
	window_cheats_text_input,
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
	window_cheats_misc_mousedown,
	window_cheats_misc_dropdown,
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

#define MAIN_CHEAT_ENABLED_WIDGETS (1ULL << WIDX_CLOSE) | (1ULL << WIDX_TAB_1) | (1ULL << WIDX_TAB_2) | (1ULL << WIDX_TAB_3) | (1ULL << WIDX_TAB_4)

static uint64 window_cheats_page_enabled_widgets[] = {
	MAIN_CHEAT_ENABLED_WIDGETS | (1ULL << WIDX_NO_MONEY) | (1ULL << WIDX_ADD_SET_MONEY_GROUP) | (1ULL << WIDX_MONEY_SPINNER) | (1ULL << WIDX_MONEY_SPINNER_INCREMENT) |
	(1ULL << WIDX_MONEY_SPINNER_DECREMENT) | (1ULL << WIDX_ADD_MONEY) | (1ULL << WIDX_SET_MONEY) | (1ULL << WIDX_CLEAR_LOAN),
	MAIN_CHEAT_ENABLED_WIDGETS | (1ULL << WIDX_GUEST_PARAMETERS_GROUP) |
		(1ULL << WIDX_GUEST_HAPPINESS_MAX) | (1ULL << WIDX_GUEST_HAPPINESS_MIN) | (1ULL << WIDX_GUEST_ENERGY_MAX) | (1ULL << WIDX_GUEST_ENERGY_MIN) |
		(1ULL << WIDX_GUEST_HUNGER_MAX) | (1ULL << WIDX_GUEST_HUNGER_MIN) | (1ULL << WIDX_GUEST_THIRST_MAX) | (1ULL << WIDX_GUEST_THIRST_MIN) |
		(1ULL << WIDX_GUEST_NAUSEA_MAX) | (1ULL << WIDX_GUEST_NAUSEA_MIN) | (1ULL << WIDX_GUEST_NAUSEA_TOLERANCE_MAX) | (1ULL << WIDX_GUEST_NAUSEA_TOLERANCE_MIN) |
		(1ULL << WIDX_GUEST_BATHROOM_MAX) | (1ULL << WIDX_GUEST_BATHROOM_MIN) | (1ULL << WIDX_GUEST_RIDE_INTENSITY_MORE_THAN_1) | (1ULL << WIDX_GUEST_RIDE_INTENSITY_LESS_THAN_15) |
		(1ULL << WIDX_GUEST_IGNORE_RIDE_INTENSITY) | (1ULL << WIDX_GIVE_ALL_GUESTS_GROUP) | (1ULL << WIDX_GIVE_GUESTS_MONEY) | (1ULL << WIDX_GIVE_GUESTS_PARK_MAPS) |
		(1ULL << WIDX_GIVE_GUESTS_BALLOONS) | (1ULL << WIDX_GIVE_GUESTS_UMBRELLAS) | (1ULL << WIDX_TRAM_GUESTS) | (1ULL << WIDX_REMOVE_ALL_GUESTS) | (1ULL << WIDX_EXPLODE_GUESTS) |
		(1ULL << WIDX_DISABLE_VANDALISM) | (1ULL << WIDX_DISABLE_LITTERING),
	MAIN_CHEAT_ENABLED_WIDGETS | (1ULL << WIDX_FREEZE_CLIMATE) |
		(1ULL << WIDX_OPEN_CLOSE_PARK) | (1ULL << WIDX_WEATHER) | (1ULL << WIDX_WEATHER_DROPDOWN_BUTTON) | (1ULL << WIDX_CLEAR_GRASS) | (1ULL << WIDX_MOWED_GRASS) |
		(1ULL << WIDX_WATER_PLANTS) | (1ULL << WIDX_DISABLE_PLANT_AGING) | (1ULL << WIDX_FIX_VANDALISM) | (1ULL << WIDX_REMOVE_LITTER) | (1ULL << WIDX_WIN_SCENARIO) | (1ULL << WIDX_HAVE_FUN) |
		(1ULL << WIDX_NEVERENDING_MARKETING) | (1ULL << WIDX_UNLOCK_ALL_PRICES) | (1ULL << WIDX_SANDBOX_MODE) | (1ULL << WIDX_RESET_DATE) | (1ULL << WIDX_FAST_STAFF) | (1ULL << WIDX_NORMAL_STAFF) |
		(1ULL << WIDX_PARK_PARAMETERS) | (1ULL << WIDX_FORCE_PARK_RATING) | (1ULL << WIDX_INCREASE_PARK_RATING) | (1ULL << WIDX_DECREASE_PARK_RATING),
	MAIN_CHEAT_ENABLED_WIDGETS | (1ULL << WIDX_RENEW_RIDES) |
		(1ULL << WIDX_MAKE_DESTRUCTIBLE) | (1ULL << WIDX_FIX_ALL) | (1ULL << WIDX_FAST_LIFT_HILL) | (1ULL << WIDX_DISABLE_BRAKES_FAILURE) |
		(1ULL << WIDX_DISABLE_ALL_BREAKDOWNS) | (1ULL << WIDX_BUILD_IN_PAUSE_MODE) | (1ULL << WIDX_RESET_CRASH_STATUS) | (1ULL << WIDX_10_MINUTE_INSPECTIONS) |
		(1ULL << WIDX_SHOW_ALL_OPERATING_MODES) | (1ULL << WIDX_SHOW_VEHICLES_FROM_OTHER_TRACK_TYPES) | (1ULL << WIDX_DISABLE_TRAIN_LENGTH_LIMITS) |
		(1ULL << WIDX_ENABLE_CHAIN_LIFT_ON_ALL_TRACK) | (1ULL << WIDX_ENABLE_ARBITRARY_RIDE_TYPE_CHANGES)
};

static uint64 window_cheats_page_hold_down_widgets[] = {
	(1ULL << WIDX_MONEY_SPINNER_INCREMENT) | (1ULL << WIDX_MONEY_SPINNER_DECREMENT) | (1ULL << WIDX_ADD_MONEY),
	0,
	(1ULL << WIDX_INCREASE_PARK_RATING) | (1ULL << WIDX_DECREASE_PARK_RATING),
	0
};

static rct_string_id window_cheats_page_titles[] = {
	STR_CHEAT_TITLE_FINANCIAL,
	STR_CHEAT_TITLE_GUEST,
	STR_CHEAT_TITLE_PARK,
	STR_CHEAT_TITLE_RIDE,
};

static void window_cheats_draw_tab_images(rct_drawpixelinfo *dpi, rct_window *w);

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
	window->hold_down_widgets = window_cheats_page_hold_down_widgets[0];
	window_init_scroll_widgets(window);
	window_cheats_set_page(window, WINDOW_CHEATS_PAGE_MONEY);
	park_rating_spinner_value = get_forced_park_rating() >= 0 ? get_forced_park_rating() : 999;
}

static void window_cheats_money_mousedown(rct_widgetindex widgetIndex, rct_window *w, rct_widget* widget)
{
	switch (widgetIndex) {
	case WIDX_MONEY_SPINNER_INCREMENT:
		_moneySpinnerValue = add_clamp_money32(MONEY_INCREMENT_DIV * (_moneySpinnerValue / MONEY_INCREMENT_DIV), MONEY_INCREMENT_DIV);
		widget_invalidate_by_class(WC_CHEATS, WIDX_MONEY_SPINNER);
		break;
	case WIDX_MONEY_SPINNER_DECREMENT:
		_moneySpinnerValue = add_clamp_money32(MONEY_INCREMENT_DIV * (_moneySpinnerValue / MONEY_INCREMENT_DIV), -MONEY_INCREMENT_DIV);
		widget_invalidate_by_class(WC_CHEATS, WIDX_MONEY_SPINNER);
		break;
	case WIDX_ADD_MONEY:
		game_do_command(0, GAME_COMMAND_FLAG_APPLY, CHEAT_ADDMONEY, _moneySpinnerValue, GAME_COMMAND_CHEAT, 0, 0);
		break;
	}
}

static void window_cheats_misc_mousedown(rct_widgetindex widgetIndex, rct_window *w, rct_widget* widget)
{
	switch (widgetIndex) {
	case WIDX_INCREASE_PARK_RATING:
		park_rating_spinner_value = min(999, 10 * (park_rating_spinner_value / 10 + 1));
		widget_invalidate_by_class(WC_CHEATS, WIDX_PARK_RATING_SPINNER);
		if (get_forced_park_rating() >= 0)
			game_do_command(0, GAME_COMMAND_FLAG_APPLY, CHEAT_SETFORCEDPARKRATING, park_rating_spinner_value, GAME_COMMAND_CHEAT, 0, 0);
		break;
	case WIDX_DECREASE_PARK_RATING:
		park_rating_spinner_value = max(0, 10 * (park_rating_spinner_value / 10 - 1));
		widget_invalidate_by_class(WC_CHEATS, WIDX_PARK_RATING_SPINNER);
		if (get_forced_park_rating() >= 0)
			game_do_command(0, GAME_COMMAND_FLAG_APPLY, CHEAT_SETFORCEDPARKRATING, park_rating_spinner_value, GAME_COMMAND_CHEAT, 0, 0);
		break;
	case WIDX_WEATHER_DROPDOWN_BUTTON:{
		rct_widget *dropdownWidget;
		int i, currentWeather;

		dropdownWidget = widget - 1;

		for (i = 0; i < 6; i++) {
			gDropdownItemsFormat[i] = STR_DROPDOWN_MENU_LABEL;
			gDropdownItemsArgs[i] = WeatherTypes[i];
		}
		window_dropdown_show_text_custom_width(
			w->x + dropdownWidget->left,
			w->y + dropdownWidget->top,
			dropdownWidget->bottom - dropdownWidget->top + 1,
			w->colours[1],
			0,
			DROPDOWN_FLAG_STAY_OPEN,
			6,
			dropdownWidget->right - dropdownWidget->left - 3
		);

		currentWeather = gClimateCurrentWeather;
		dropdown_set_checked(currentWeather, true);
	}
	break;
	}
}

static void window_cheats_misc_dropdown(rct_window *w, rct_widgetindex widgetIndex, sint32 dropdownIndex)
{
	if (widgetIndex != WIDX_WEATHER_DROPDOWN_BUTTON || dropdownIndex == -1)
		return;

	game_do_command(0, GAME_COMMAND_FLAG_APPLY, CHEAT_FORCEWEATHER, dropdownIndex, GAME_COMMAND_CHEAT, 0, 0);
}

static void window_cheats_money_mouseup(rct_window *w, rct_widgetindex widgetIndex)
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
	case WIDX_NO_MONEY:
		game_do_command(0, GAME_COMMAND_FLAG_APPLY, CHEAT_NOMONEY, gParkFlags & PARK_FLAGS_NO_MONEY ? 0 : 1, GAME_COMMAND_CHEAT, 0, 0);
		break;
	case WIDX_MONEY_SPINNER:
		money_to_string(_moneySpinnerValue, _moneySpinnerText, MONEY_DIGITS);
		window_text_input_raw_open(w, WIDX_MONEY_SPINNER, STR_ENTER_NEW_VALUE, STR_ENTER_NEW_VALUE, _moneySpinnerText, MONEY_DIGITS);
		break;
	case WIDX_SET_MONEY:
		game_do_command(0, GAME_COMMAND_FLAG_APPLY, CHEAT_SETMONEY, _moneySpinnerValue, GAME_COMMAND_CHEAT, 0, 0);
		break;
	case WIDX_CLEAR_LOAN:
		game_do_command(0, GAME_COMMAND_FLAG_APPLY, CHEAT_CLEARLOAN, CHEATS_MONEY_INCREMENT, GAME_COMMAND_CHEAT, 0, 0);
		break;
	}
}

static void window_cheats_guests_mouseup(rct_window *w, rct_widgetindex widgetIndex)
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
		game_do_command(0, GAME_COMMAND_FLAG_APPLY, CHEAT_SETGUESTPARAMETER, GUEST_PARAMETER_HAPPINESS, GAME_COMMAND_CHEAT, 255, 0);
		break;
	case WIDX_GUEST_HAPPINESS_MIN:
		game_do_command(0, GAME_COMMAND_FLAG_APPLY, CHEAT_SETGUESTPARAMETER, GUEST_PARAMETER_HAPPINESS, GAME_COMMAND_CHEAT, 0, 0);
		break;
	case WIDX_GUEST_ENERGY_MAX:
		game_do_command(0, GAME_COMMAND_FLAG_APPLY, CHEAT_SETGUESTPARAMETER, GUEST_PARAMETER_ENERGY, GAME_COMMAND_CHEAT, 127, 0);
		break;
	case WIDX_GUEST_ENERGY_MIN:
		game_do_command(0, GAME_COMMAND_FLAG_APPLY, CHEAT_SETGUESTPARAMETER, GUEST_PARAMETER_ENERGY, GAME_COMMAND_CHEAT, 0, 0);
		break;
	case WIDX_GUEST_HUNGER_MAX:
		game_do_command(0, GAME_COMMAND_FLAG_APPLY, CHEAT_SETGUESTPARAMETER, GUEST_PARAMETER_HUNGER, GAME_COMMAND_CHEAT, 0, 0);
		break;
	case WIDX_GUEST_HUNGER_MIN:
		game_do_command(0, GAME_COMMAND_FLAG_APPLY, CHEAT_SETGUESTPARAMETER, GUEST_PARAMETER_HUNGER, GAME_COMMAND_CHEAT, 255, 0);
		break;
	case WIDX_GUEST_THIRST_MAX:
		game_do_command(0, GAME_COMMAND_FLAG_APPLY, CHEAT_SETGUESTPARAMETER, GUEST_PARAMETER_THIRST, GAME_COMMAND_CHEAT, 0, 0);
		break;
	case WIDX_GUEST_THIRST_MIN:
		game_do_command(0, GAME_COMMAND_FLAG_APPLY, CHEAT_SETGUESTPARAMETER, GUEST_PARAMETER_THIRST, GAME_COMMAND_CHEAT, 255, 0);
		break;
	case WIDX_GUEST_NAUSEA_MAX:
		game_do_command(0, GAME_COMMAND_FLAG_APPLY, CHEAT_SETGUESTPARAMETER, GUEST_PARAMETER_NAUSEA, GAME_COMMAND_CHEAT, 255, 0);
		break;
	case WIDX_GUEST_NAUSEA_MIN:
		game_do_command(0, GAME_COMMAND_FLAG_APPLY, CHEAT_SETGUESTPARAMETER, GUEST_PARAMETER_NAUSEA, GAME_COMMAND_CHEAT, 0, 0);
		break;
	case WIDX_GUEST_NAUSEA_TOLERANCE_MAX:
		game_do_command(0, GAME_COMMAND_FLAG_APPLY, CHEAT_SETGUESTPARAMETER, GUEST_PARAMETER_NAUSEA_TOLERANCE, GAME_COMMAND_CHEAT, PEEP_NAUSEA_TOLERANCE_HIGH, 0);
		break;
	case WIDX_GUEST_NAUSEA_TOLERANCE_MIN:
		game_do_command(0, GAME_COMMAND_FLAG_APPLY, CHEAT_SETGUESTPARAMETER, GUEST_PARAMETER_NAUSEA_TOLERANCE, GAME_COMMAND_CHEAT, PEEP_NAUSEA_TOLERANCE_NONE, 0);
		break;
	case WIDX_GUEST_BATHROOM_MAX:
		game_do_command(0, GAME_COMMAND_FLAG_APPLY, CHEAT_SETGUESTPARAMETER, GUEST_PARAMETER_BATHROOM, GAME_COMMAND_CHEAT, 255, 0);
		break;
	case WIDX_GUEST_BATHROOM_MIN:
		game_do_command(0, GAME_COMMAND_FLAG_APPLY, CHEAT_SETGUESTPARAMETER, GUEST_PARAMETER_BATHROOM, GAME_COMMAND_CHEAT, 0, 0);
		break;
	case WIDX_GUEST_RIDE_INTENSITY_MORE_THAN_1:
		game_do_command(0, GAME_COMMAND_FLAG_APPLY, CHEAT_SETGUESTPARAMETER, GUEST_PARAMETER_PREFERRED_RIDE_INTENSITY, GAME_COMMAND_CHEAT, 1, 0);
		break;
	case WIDX_GUEST_RIDE_INTENSITY_LESS_THAN_15:
		game_do_command(0, GAME_COMMAND_FLAG_APPLY, CHEAT_SETGUESTPARAMETER, GUEST_PARAMETER_PREFERRED_RIDE_INTENSITY, GAME_COMMAND_CHEAT, 0, 0);
		break;
	case WIDX_TRAM_GUESTS:
		game_do_command(0, GAME_COMMAND_FLAG_APPLY, CHEAT_GENERATEGUESTS, CHEATS_TRAM_INCREMENT, GAME_COMMAND_CHEAT, 0, 0);
		break;
	case WIDX_REMOVE_ALL_GUESTS:
		game_do_command(0, GAME_COMMAND_FLAG_APPLY, CHEAT_REMOVEALLGUESTS, 0, GAME_COMMAND_CHEAT, 0, 0);
		break;
	case WIDX_EXPLODE_GUESTS:
		game_do_command(0, GAME_COMMAND_FLAG_APPLY, CHEAT_EXPLODEGUESTS, 0, GAME_COMMAND_CHEAT, 0, 0);
		break;
	case WIDX_GIVE_GUESTS_MONEY:
		game_do_command(0, GAME_COMMAND_FLAG_APPLY, CHEAT_GIVEALLGUESTS, OBJECT_MONEY, GAME_COMMAND_CHEAT, 0, 0);
		break;
	case WIDX_GIVE_GUESTS_PARK_MAPS:
		game_do_command(0, GAME_COMMAND_FLAG_APPLY, CHEAT_GIVEALLGUESTS, OBJECT_PARK_MAP, GAME_COMMAND_CHEAT, 0, 0);
		break;
	case WIDX_GIVE_GUESTS_BALLOONS:
		game_do_command(0, GAME_COMMAND_FLAG_APPLY, CHEAT_GIVEALLGUESTS, OBJECT_BALLOON, GAME_COMMAND_CHEAT, 0, 0);
		break;
	case WIDX_GIVE_GUESTS_UMBRELLAS:
		game_do_command(0, GAME_COMMAND_FLAG_APPLY, CHEAT_GIVEALLGUESTS, OBJECT_UMBRELLA, GAME_COMMAND_CHEAT, 0, 0);
		break;
	case WIDX_GUEST_IGNORE_RIDE_INTENSITY:
		game_do_command(0, GAME_COMMAND_FLAG_APPLY, CHEAT_IGNORERIDEINTENSITY, !gCheatsIgnoreRideIntensity, GAME_COMMAND_CHEAT, 0, 0);
		break;
	case WIDX_DISABLE_VANDALISM:
		game_do_command(0, GAME_COMMAND_FLAG_APPLY, CHEAT_DISABLEVANDALISM, !gCheatsDisableVandalism, GAME_COMMAND_CHEAT, 0, 0);
		break;
	case WIDX_DISABLE_LITTERING:
		game_do_command(0, GAME_COMMAND_FLAG_APPLY, CHEAT_DISABLELITTERING, !gCheatsDisableLittering, GAME_COMMAND_CHEAT, 0, 0);
		break;
	}
}

static void window_cheats_misc_mouseup(rct_window *w, rct_widgetindex widgetIndex)
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
		game_do_command(0, GAME_COMMAND_FLAG_APPLY, CHEAT_FREEZECLIMATE, !gCheatsFreezeClimate, GAME_COMMAND_CHEAT, 0, 0);
		break;
	case WIDX_OPEN_CLOSE_PARK:
		game_do_command(0, GAME_COMMAND_FLAG_APPLY, CHEAT_OPENCLOSEPARK, 0, GAME_COMMAND_CHEAT, 0, 0);
		break;
	case WIDX_CLEAR_GRASS:
		game_do_command(0, GAME_COMMAND_FLAG_APPLY, CHEAT_SETGRASSLENGTH, GRASS_LENGTH_CLEAR_0, GAME_COMMAND_CHEAT, 0, 0);
		break;
	case WIDX_MOWED_GRASS:
		game_do_command(0, GAME_COMMAND_FLAG_APPLY, CHEAT_SETGRASSLENGTH, GRASS_LENGTH_MOWED, GAME_COMMAND_CHEAT, 0, 0);
		break;
	case WIDX_WATER_PLANTS:
		game_do_command(0, GAME_COMMAND_FLAG_APPLY, CHEAT_WATERPLANTS, 0, GAME_COMMAND_CHEAT, 0, 0);
		break;
	case WIDX_FIX_VANDALISM:
		game_do_command(0, GAME_COMMAND_FLAG_APPLY, CHEAT_FIXVANDALISM, 0, GAME_COMMAND_CHEAT, 0, 0);
		break;
	case WIDX_REMOVE_LITTER:
		game_do_command(0, GAME_COMMAND_FLAG_APPLY, CHEAT_REMOVELITTER, 0, GAME_COMMAND_CHEAT, 0, 0);
		break;
	case WIDX_DISABLE_PLANT_AGING:
		game_do_command(0, GAME_COMMAND_FLAG_APPLY, CHEAT_DISABLEPLANTAGING, !gCheatsDisablePlantAging, GAME_COMMAND_CHEAT, 0, 0);
		break;
	case WIDX_WIN_SCENARIO:
		game_do_command(0, GAME_COMMAND_FLAG_APPLY, CHEAT_WINSCENARIO, 0, GAME_COMMAND_CHEAT, 0, 0);
		break;
	case WIDX_HAVE_FUN:
		game_do_command(0, GAME_COMMAND_FLAG_APPLY, CHEAT_HAVEFUN, 0, GAME_COMMAND_CHEAT, 0, 0);
		break;
	case WIDX_NEVERENDING_MARKETING:
		game_do_command(0, GAME_COMMAND_FLAG_APPLY, CHEAT_NEVERENDINGMARKETING, !gCheatsNeverendingMarketing, GAME_COMMAND_CHEAT, 0, 0);
		break;
	case WIDX_UNLOCK_ALL_PRICES:
		game_do_command(0, GAME_COMMAND_FLAG_APPLY, CHEAT_UNLOCKALLPRICES, !gCheatsUnlockAllPrices, GAME_COMMAND_CHEAT, 0, 0);
		break;
	case WIDX_SANDBOX_MODE:
		game_do_command(0, GAME_COMMAND_FLAG_APPLY, CHEAT_SANDBOXMODE, !gCheatsSandboxMode, GAME_COMMAND_CHEAT, 0, 0);
		// To prevent tools from staying active after disabling cheat
		//tool_cancel();
		break;
	case WIDX_RESET_DATE:
		game_do_command(0, GAME_COMMAND_FLAG_APPLY, CHEAT_RESETDATE, 0, GAME_COMMAND_CHEAT, 0, 0);
		break;
	case WIDX_FAST_STAFF:
		game_do_command(0, GAME_COMMAND_FLAG_APPLY, CHEAT_SETSTAFFSPEED, 0xFF, GAME_COMMAND_CHEAT, 0, 0);
		break;
	case WIDX_NORMAL_STAFF:
		game_do_command(0, GAME_COMMAND_FLAG_APPLY, CHEAT_SETSTAFFSPEED, 0x60, GAME_COMMAND_CHEAT, 0, 0);
		break;
	case WIDX_PARK_PARAMETERS:
		window_editor_scenario_options_open();
		break;
	case WIDX_FORCE_PARK_RATING:
		if (get_forced_park_rating() >= 0){
			game_do_command(0, GAME_COMMAND_FLAG_APPLY, CHEAT_SETFORCEDPARKRATING, -1, GAME_COMMAND_CHEAT, 0, 0);
		} else {
			game_do_command(0, GAME_COMMAND_FLAG_APPLY, CHEAT_SETFORCEDPARKRATING, park_rating_spinner_value, GAME_COMMAND_CHEAT, 0, 0);
		}
		break;
	}
}

static void window_cheats_rides_mouseup(rct_window *w, rct_widgetindex widgetIndex)
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
		game_do_command(0, GAME_COMMAND_FLAG_APPLY, CHEAT_RENEWRIDES, 0, GAME_COMMAND_CHEAT, 0, 0);
		break;
	case WIDX_MAKE_DESTRUCTIBLE:
		game_do_command(0, GAME_COMMAND_FLAG_APPLY, CHEAT_MAKEDESTRUCTIBLE, 0, GAME_COMMAND_CHEAT, 0, 0);
		break;
	case WIDX_FIX_ALL:
		game_do_command(0, GAME_COMMAND_FLAG_APPLY, CHEAT_FIXRIDES, 0, GAME_COMMAND_CHEAT, 0, 0);
		break;
	case WIDX_FAST_LIFT_HILL:
		game_do_command(0, GAME_COMMAND_FLAG_APPLY, CHEAT_FASTLIFTHILL, !gCheatsFastLiftHill, GAME_COMMAND_CHEAT, 0, 0);
		break;
	case WIDX_DISABLE_BRAKES_FAILURE:
		game_do_command(0, GAME_COMMAND_FLAG_APPLY, CHEAT_DISABLEBRAKESFAILURE, !gCheatsDisableBrakesFailure, GAME_COMMAND_CHEAT, 0, 0);
		break;
	case WIDX_DISABLE_ALL_BREAKDOWNS:
		game_do_command(0, GAME_COMMAND_FLAG_APPLY, CHEAT_DISABLEALLBREAKDOWNS, !gCheatsDisableAllBreakdowns, GAME_COMMAND_CHEAT, 0, 0);
		break;
	case WIDX_BUILD_IN_PAUSE_MODE:
		game_do_command(0, GAME_COMMAND_FLAG_APPLY, CHEAT_BUILDINPAUSEMODE, !gCheatsBuildInPauseMode, GAME_COMMAND_CHEAT, 0, 0);
		break;
	case WIDX_RESET_CRASH_STATUS:
		game_do_command(0, GAME_COMMAND_FLAG_APPLY, CHEAT_RESETCRASHSTATUS, 0, GAME_COMMAND_CHEAT, 0, 0);
		break;
	case WIDX_10_MINUTE_INSPECTIONS:
		game_do_command(0, GAME_COMMAND_FLAG_APPLY, CHEAT_10MINUTEINSPECTIONS, 0, GAME_COMMAND_CHEAT, 0, 0);
		break;
	case WIDX_SHOW_ALL_OPERATING_MODES:
		game_do_command(0, GAME_COMMAND_FLAG_APPLY, CHEAT_SHOWALLOPERATINGMODES, !gCheatsShowAllOperatingModes, GAME_COMMAND_CHEAT, 0, 0);
		if (gCheatsShowAllOperatingModes) {
			window_error_open(STR_WARNING_IN_CAPS, STR_THIS_FEATURE_IS_CURRENTLY_UNSTABLE);
		}
		break;
	case WIDX_SHOW_VEHICLES_FROM_OTHER_TRACK_TYPES:
		game_do_command(0, GAME_COMMAND_FLAG_APPLY, CHEAT_SHOWVEHICLESFROMOTHERTRACKTYPES, !gCheatsShowVehiclesFromOtherTrackTypes, GAME_COMMAND_CHEAT, 0, 0);
		if (gCheatsShowVehiclesFromOtherTrackTypes) {
			window_error_open(STR_WARNING_IN_CAPS, STR_THIS_FEATURE_IS_CURRENTLY_UNSTABLE);
		}
		break;
	case WIDX_DISABLE_TRAIN_LENGTH_LIMITS:
		game_do_command(0, GAME_COMMAND_FLAG_APPLY, CHEAT_DISABLETRAINLENGTHLIMIT, !gCheatsDisableTrainLengthLimit, GAME_COMMAND_CHEAT, 0, 0);
		if (gCheatsDisableTrainLengthLimit) {
			window_error_open(STR_WARNING_IN_CAPS, STR_THIS_FEATURE_IS_CURRENTLY_UNSTABLE);
		}
		break;
	case WIDX_ENABLE_CHAIN_LIFT_ON_ALL_TRACK:
		game_do_command(0, GAME_COMMAND_FLAG_APPLY, CHEAT_ENABLECHAINLIFTONALLTRACK, !gCheatsEnableChainLiftOnAllTrack, GAME_COMMAND_CHEAT, 0, 0);
		break;
	case WIDX_ENABLE_ARBITRARY_RIDE_TYPE_CHANGES:
		game_do_command(0, GAME_COMMAND_FLAG_APPLY, CHEAT_ALLOW_ARBITRARY_RIDE_TYPE_CHANGES, !gCheatsAllowArbitraryRideTypeChanges, GAME_COMMAND_CHEAT, 0, 0);
		if (gCheatsAllowArbitraryRideTypeChanges) {
			window_error_open(STR_WARNING_IN_CAPS, STR_THIS_FEATURE_IS_CURRENTLY_UNSTABLE);
		}
		break;
	}
}

static void window_cheats_text_input(rct_window *w, rct_widgetindex widgetIndex, char *text) {
	if (text == NULL)
		return;

	if (w->page == WINDOW_CHEATS_PAGE_MONEY && widgetIndex == WIDX_MONEY_SPINNER) {
		money32 val = string_to_money(text);
		if (val != MONEY32_UNDEFINED) {
			_moneySpinnerValue = val;
		}
		window_invalidate(w);
	}
}

static void window_cheats_update(rct_window *w)
{
	w->frame_no++;
	widget_invalidate(w, WIDX_TAB_1 + w->page);
}

static void window_cheats_invalidate(rct_window *w)
{
	sint32 i;

	colour_scheme_update(w);

	rct_widget *widgets = window_cheats_page_widgets[w->page];
	if (w->widgets != widgets) {
		w->widgets = widgets;
		window_init_scroll_widgets(w);
	}

	w->pressed_widgets = 0;
	w->disabled_widgets = 0;

	// Set correct active tab
	for (i = 0; i < 7; i++)
		w->pressed_widgets &= ~(1 << (WIDX_TAB_1 + i));
	w->pressed_widgets |= 1LL << (WIDX_TAB_1 + w->page);

	// Set title
	w->widgets[WIDX_TITLE].text = window_cheats_page_titles[w->page];

	switch (w->page) {
	case WINDOW_CHEATS_PAGE_MONEY:{
		widget_set_checkbox_value(w, WIDX_NO_MONEY, gParkFlags & PARK_FLAGS_NO_MONEY);

		uint64 money_widgets = (1 << WIDX_ADD_SET_MONEY_GROUP) | (1 << WIDX_MONEY_SPINNER) | (1 << WIDX_MONEY_SPINNER_INCREMENT) |
			(1 << WIDX_MONEY_SPINNER_DECREMENT) | (1 << WIDX_ADD_MONEY) | (1 << WIDX_SET_MONEY) | (1 << WIDX_CLEAR_LOAN);
		if (gParkFlags & PARK_FLAGS_NO_MONEY) {
			w->disabled_widgets |= money_widgets;
		}
		else {
			w->disabled_widgets &= ~money_widgets;
		}
	}break;
	case WINDOW_CHEATS_PAGE_GUESTS:
		set_format_arg(0, sint32, 10000);
		widget_set_checkbox_value(w, WIDX_GUEST_IGNORE_RIDE_INTENSITY, gCheatsIgnoreRideIntensity);
		widget_set_checkbox_value(w, WIDX_DISABLE_VANDALISM, gCheatsDisableVandalism);
		widget_set_checkbox_value(w, WIDX_DISABLE_LITTERING, gCheatsDisableLittering);
		break;
	case WINDOW_CHEATS_PAGE_MISC:
		w->widgets[WIDX_OPEN_CLOSE_PARK].text = (gParkFlags & PARK_FLAGS_PARK_OPEN) ? STR_CHEAT_CLOSE_PARK : STR_CHEAT_OPEN_PARK;
		widget_set_checkbox_value(w, WIDX_UNLOCK_ALL_PRICES, gCheatsUnlockAllPrices);
		widget_set_checkbox_value(w, WIDX_FORCE_PARK_RATING, get_forced_park_rating() >= 0);
		w->widgets[WIDX_SANDBOX_MODE].text = gCheatsSandboxMode ? STR_CHEAT_SANDBOX_MODE_DISABLE : STR_CHEAT_SANDBOX_MODE;
		w->widgets[WIDX_FREEZE_CLIMATE].text = gCheatsFreezeClimate ? STR_CHEAT_UNFREEZE_CLIMATE : STR_CHEAT_FREEZE_CLIMATE;
		widget_set_checkbox_value(w, WIDX_NEVERENDING_MARKETING, gCheatsNeverendingMarketing);
		widget_set_checkbox_value(w, WIDX_DISABLE_PLANT_AGING, gCheatsDisablePlantAging);
		break;
	case WINDOW_CHEATS_PAGE_RIDES:
		set_format_arg(0, uint16, 255);
		widget_set_checkbox_value(w, WIDX_FAST_LIFT_HILL, gCheatsFastLiftHill);
		widget_set_checkbox_value(w, WIDX_DISABLE_BRAKES_FAILURE, gCheatsDisableBrakesFailure);
		widget_set_checkbox_value(w, WIDX_DISABLE_ALL_BREAKDOWNS, gCheatsDisableAllBreakdowns);
		widget_set_checkbox_value(w, WIDX_BUILD_IN_PAUSE_MODE, gCheatsBuildInPauseMode);
		widget_set_checkbox_value(w, WIDX_SHOW_ALL_OPERATING_MODES, gCheatsShowAllOperatingModes);
		widget_set_checkbox_value(w, WIDX_SHOW_VEHICLES_FROM_OTHER_TRACK_TYPES, gCheatsShowVehiclesFromOtherTrackTypes);
		widget_set_checkbox_value(w, WIDX_DISABLE_TRAIN_LENGTH_LIMITS, gCheatsDisableTrainLengthLimit);
		widget_set_checkbox_value(w, WIDX_ENABLE_CHAIN_LIFT_ON_ALL_TRACK, gCheatsEnableChainLiftOnAllTrack);
		widget_set_checkbox_value(w, WIDX_ENABLE_ARBITRARY_RIDE_TYPE_CHANGES, gCheatsAllowArbitraryRideTypeChanges);
		break;
	}

	// Current weather
	window_cheats_misc_widgets[WIDX_WEATHER].text = WeatherTypes[gClimateCurrentWeather];
}

static void window_cheats_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
	window_draw_widgets(w, dpi);
	window_cheats_draw_tab_images(dpi, w);

	if (w->page == WINDOW_CHEATS_PAGE_MONEY){
		uint8 colour = w->colours[1];
		set_format_arg(0, money32, _moneySpinnerValue);
		if (widget_is_disabled(w, WIDX_MONEY_SPINNER)) {
			colour |= COLOUR_FLAG_INSET;
		}
		gfx_draw_string_left(dpi, STR_BOTTOM_TOOLBAR_CASH, gCommonFormatArgs, colour, w->x + XPL(0) + TXTO, w->y + YPL(2) + TXTO);
	}
	else if(w->page == WINDOW_CHEATS_PAGE_MISC){
		gfx_draw_string_left(dpi, STR_CHEAT_STAFF_SPEED,			NULL,	COLOUR_BLACK, w->x + XPL(0) + TXTO, w->y + YPL(16) + TXTO);
		gfx_draw_string_left(dpi, STR_FORCE_WEATHER,				NULL,	COLOUR_BLACK, w->x + XPL(0) + TXTO, w->y + YPL(9) + TXTO);
		gfx_draw_string_right(dpi, STR_FORMAT_INTEGER,		&park_rating_spinner_value,	w->colours[1], w->x + WPL(1) - 10 - TXTO, w->y + YPL(4) + TXTO);
	}
	else if (w->page == WINDOW_CHEATS_PAGE_GUESTS){
		gfx_draw_string_left(dpi, STR_CHEAT_GUEST_HAPPINESS,		NULL,	COLOUR_BLACK, w->x + XPL(0) + TXTO, w->y + YPL(1) + TXTO);
		gfx_draw_string_left(dpi, STR_CHEAT_GUEST_ENERGY,			NULL,	COLOUR_BLACK, w->x + XPL(0) + TXTO, w->y + YPL(2) + TXTO);
		gfx_draw_string_left(dpi, STR_CHEAT_GUEST_HUNGER,			NULL,	COLOUR_BLACK, w->x + XPL(0) + TXTO, w->y + YPL(3) + TXTO);
		gfx_draw_string_left(dpi, STR_CHEAT_GUEST_THIRST,			NULL,	COLOUR_BLACK, w->x + XPL(0) + TXTO, w->y + YPL(4) + TXTO);
		gfx_draw_string_left(dpi, STR_CHEAT_GUEST_NAUSEA,			NULL,	COLOUR_BLACK, w->x + XPL(0) + TXTO, w->y + YPL(5) + TXTO);
		gfx_draw_string_left(dpi, STR_CHEAT_GUEST_NAUSEA_TOLERANCE,	NULL,	COLOUR_BLACK, w->x + XPL(0) + TXTO, w->y + YPL(6) + TXTO);
		gfx_draw_string_left(dpi, STR_CHEAT_GUEST_BATHROOM,			NULL,	COLOUR_BLACK, w->x + XPL(0) + TXTO, w->y + YPL(7) + TXTO);
		gfx_draw_string_left(dpi, STR_CHEAT_GUEST_PREFERRED_INTENSITY,NULL,	COLOUR_BLACK, w->x + XPL(0) + TXTO, w->y + YPL(8) + TXTO);
	}
}

static void window_cheats_draw_tab_images(rct_drawpixelinfo *dpi, rct_window *w)
{
	sint32 sprite_idx;

	// Money tab
	if (!(w->disabled_widgets & (1 << WIDX_TAB_1))) {
		sprite_idx = SPR_TAB_FINANCES_SUMMARY_0;
		if (w->page == WINDOW_CHEATS_PAGE_MONEY)
			sprite_idx += (w->frame_no / 2) % 8;
		gfx_draw_sprite(dpi, sprite_idx, w->x + w->widgets[WIDX_TAB_1].left, w->y + w->widgets[WIDX_TAB_1].top, 0);
	}

	// Guests tab
	if (!(w->disabled_widgets & (1 << WIDX_TAB_2))) {
		sprite_idx = SPR_TAB_GUESTS_0;
		if (w->page == WINDOW_CHEATS_PAGE_GUESTS)
			sprite_idx += (w->frame_no / 3) % 8;
		gfx_draw_sprite(dpi, sprite_idx, w->x + w->widgets[WIDX_TAB_2].left, w->y + w->widgets[WIDX_TAB_2].top, 0);
	}

	// Misc tab
	if (!(w->disabled_widgets & (1 << WIDX_TAB_3))) {
		sprite_idx = SPR_TAB_PARK;
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

static void window_cheats_set_page(rct_window *w, sint32 page)
{
	w->page = page;
	w->frame_no = 0;

	w->enabled_widgets = window_cheats_page_enabled_widgets[page];
	w->hold_down_widgets = window_cheats_page_hold_down_widgets[page];
	w->pressed_widgets = 0;

	w->event_handlers = window_cheats_page_events[page];
	w->widgets = window_cheats_page_widgets[page];

	sint32 maxY = 0;
	rct_widget *widget = &w->widgets[WIDX_TAB_CONTENT];
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
