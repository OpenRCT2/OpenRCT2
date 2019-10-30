/*****************************************************************************
 * Copyright (c) 2014-2019 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <algorithm>
#include <iterator>
#include <openrct2-ui/interface/Dropdown.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/Context.h>
#include <openrct2/Game.h>
#include <openrct2/actions/ParkSetDateAction.hpp>
#include <openrct2/actions/SetCheatAction.hpp>
#include <openrct2/config/Config.h>
#include <openrct2/localisation/Date.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/network/network.h>
#include <openrct2/sprites.h>
#include <openrct2/util/Util.h>
#include <openrct2/world/Climate.h>
#include <openrct2/world/Park.h>
#include <openrct2/world/Surface.h>

#define CHEATS_MONEY_DEFAULT MONEY(10000, 00)
#define CHEATS_MONEY_INCREMENT_DIV MONEY(5000, 00)
static utf8 _moneySpinnerText[MONEY_STRING_MAXLENGTH];
static money32 _moneySpinnerValue = CHEATS_MONEY_DEFAULT;
static int32_t _selectedStaffSpeed = 1;
static int32_t _parkRatingSpinnerValue;
static int32_t _yearSpinnerValue = 1;
static int32_t _monthSpinnerValue = 1;
static int32_t _daySpinnerValue = 1;

// clang-format off
enum
{
    WINDOW_CHEATS_PAGE_MONEY,
    WINDOW_CHEATS_PAGE_GUESTS,
    WINDOW_CHEATS_PAGE_MISC,
    WINDOW_CHEATS_PAGE_RIDES,
};

static rct_string_id _staffSpeedNames[] =
{
    STR_FROZEN,
    STR_NORMAL,
    STR_FAST,
};

static constexpr const rct_string_id WeatherTypes[] =
{
    STR_SUNNY,
    STR_PARTIALLY_CLOUDY,
    STR_CLOUDY,
    STR_RAIN,
    STR_HEAVY_RAIN,
    STR_THUNDERSTORM,
};

enum WINDOW_CHEATS_WIDGET_IDX
{
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
    WIDX_DATE_GROUP,
    WIDX_YEAR_BOX,
    WIDX_YEAR_UP,
    WIDX_YEAR_DOWN,
    WIDX_MONTH_BOX,
    WIDX_MONTH_UP,
    WIDX_MONTH_DOWN,
    WIDX_DAY_BOX,
    WIDX_DAY_UP,
    WIDX_DAY_DOWN,
    WIDX_DATE_SET,
    WIDX_DATE_RESET,

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
    WIDX_CREATE_DUCKS,
    WIDX_PARK_PARAMETERS,
    WIDX_REMOVE_DUCKS,
    WIDX_OWN_ALL_LAND,
    WIDX_FORCE_PARK_RATING,
    WIDX_PARK_RATING_SPINNER,
    WIDX_INCREASE_PARK_RATING,
    WIDX_DECREASE_PARK_RATING,
    WIDX_WIN_SCENARIO,
    WIDX_HAVE_FUN,
    WIDX_NEVERENDING_MARKETING,
    WIDX_WEATHER_GROUP,
    WIDX_FREEZE_WEATHER,
    WIDX_WEATHER,
    WIDX_WEATHER_DROPDOWN_BUTTON,
    WIDX_STAFF_GROUP,
    WIDX_CLEAR_GRASS,
    WIDX_MOWED_GRASS,
    WIDX_WATER_PLANTS,
    WIDX_FIX_VANDALISM,
    WIDX_REMOVE_LITTER,
    WIDX_DISABLE_PLANT_AGING,
    WIDX_STAFF_SPEED,
    WIDX_STAFF_SPEED_DROPDOWN_BUTTON,

    WIDX_FIX_ALL = WIDX_TAB_CONTENT,
    WIDX_RENEW_RIDES,
    WIDX_MAKE_DESTRUCTIBLE,
    WIDX_RESET_CRASH_STATUS,
    WIDX_10_MINUTE_INSPECTIONS,
    WIDX_CONSTRUCTION_GROUP,
    WIDX_BUILD_IN_PAUSE_MODE,
    WIDX_ENABLE_ALL_DRAWABLE_TRACK_PIECES,
    WIDX_ENABLE_CHAIN_LIFT_ON_ALL_TRACK,
    WIDX_OPERATION_MODES_GROUP,
    WIDX_SHOW_ALL_OPERATING_MODES,
    WIDX_FAST_LIFT_HILL,
    WIDX_DISABLE_BRAKES_FAILURE,
    WIDX_DISABLE_ALL_BREAKDOWNS,
    WIDX_DISABLE_RIDE_VALUE_AGING,
    WIDX_TRACK_PIECES_GROUP,
    WIDX_ENABLE_ARBITRARY_RIDE_TYPE_CHANGES,
    WIDX_SHOW_VEHICLES_FROM_OTHER_TRACK_TYPES,
    WIDX_DISABLE_TRAIN_LENGTH_LIMITS,
    WIDX_IGNORE_RESEARCH_STATUS,
};

#pragma region MEASUREMENTS

constexpr int32_t WW = 249;
constexpr int32_t WH = 300;
constexpr int32_t TAB_HEIGHT = 43;
constexpr int32_t XSPA = 5;                                              // X spacing
constexpr int32_t YSPA = 5;                                              // Y spacing
constexpr int32_t XOS = 6 + XSPA;                                        // X offset from left
constexpr int32_t YOS = TAB_HEIGHT + YSPA;                               // Y offset from top (includes tabs height)
constexpr int32_t BTNW = 110;                                            // button width
constexpr int32_t BTNH = 16;                                             // button height
constexpr int32_t OPTW = 220;                                            // Option (checkbox) width (two columns)
constexpr int32_t OPTH = 10;                                             // Option (checkbox) height (two columns)
constexpr int32_t GROUP_SPACE = 6;

#define YPL(ROW)        ((int16_t)(YOS + ((BTNH + YSPA) * ROW)))
#define HPL(ROW)        ((int16_t)(YPL(ROW) + BTNH))
#define OHPL(ROW)       ((int16_t)(YPL(ROW) + OPTH))
#define XPL(COL)        ((int16_t)(XOS + ((BTNW + XSPA) * COL)))
#define WPL(COL)        ((int16_t)(XPL(COL) + BTNW))
#define OWPL            ((int16_t)(XPL(0) + OPTW))

#define MIN_BTN_LEFT    ((int16_t)(XPL(1)))
#define MIN_BTN_RIGHT   ((int16_t)(WPL(1) - (BTNW / 2)))
#define MAX_BTN_LEFT    ((int16_t)(XPL(1.5)))
#define MAX_BTN_RIGHT   ((int16_t)(WPL(1)))

constexpr int32_t TXTO = 3;  // Text horizontal offset from button left (for button text)
#pragma endregion

#define MAIN_CHEATS_WIDGETS \
    { WWT_FRAME,            0,  0,          WW - 1, 0,      WH - 1,     0xFFFFFFFF,             STR_NONE },                 /* panel / background   */ \
    { WWT_CAPTION,          0,  1,          WW - 2, 1,      14,         STR_CHEAT_TITLE,        STR_WINDOW_TITLE_TIP },     /* title bar            */ \
    { WWT_CLOSEBOX,         0,  WW - 13,    WW - 3, 2,      13,         STR_CLOSE_X,            STR_CLOSE_WINDOW_TIP },     /* close x button       */ \
    { WWT_IMGBTN,           1,  0,          WW - 1, 43,     WH - 1,     0xFFFFFFFF,             STR_NONE },                 /* tab content panel    */ \
    { WWT_TAB,              1,  3,          33,     17,     43,         IMAGE_TYPE_REMAP | SPR_TAB,   STR_FINANCIAL_CHEATS_TIP }, /* tab 1                */ \
    { WWT_TAB,              1,  34,         64,     17,     43,         IMAGE_TYPE_REMAP | SPR_TAB,   STR_GUEST_CHEATS_TIP },     /* tab 2                */ \
    { WWT_TAB,              1,  65,         95,     17,     43,         IMAGE_TYPE_REMAP | SPR_TAB,   STR_PARK_CHEATS_TIP },      /* tab 3                */ \
    { WWT_TAB,              1,  96,         126,    17,     43,         IMAGE_TYPE_REMAP | SPR_TAB,   STR_RIDE_CHEATS_TIP }       /* tab 4                */

static rct_widget window_cheats_money_widgets[] =
{
    MAIN_CHEATS_WIDGETS,
    { WWT_CHECKBOX,         1,      XPL(0),                 WPL(0),                 YPL(0),         HPL(0),         STR_MAKE_PARK_NO_MONEY,             STR_NONE },                             // No money
    { WWT_GROUPBOX,         1,      XPL(0) - GROUP_SPACE,   WPL(1) + GROUP_SPACE,   YPL(1),         HPL(3.5),       STR_ADD_SET_MONEY,                  STR_NONE },                             // add / set money group frame
      SPINNER_WIDGETS      (1,      XPL(0),                 WPL(1) - 10,            YPL(2) + 2,     HPL(2) - 3,     STR_NONE,                           STR_NONE),                              // money value (3 widgets)
    { WWT_BUTTON,           1,      XPL(0),                 WPL(0),                 YPL(3),         HPL(3),         STR_ADD_MONEY,                      STR_NONE },                             // add money
    { WWT_BUTTON,           1,      XPL(1),                 WPL(1),                 YPL(3),         HPL(3),         STR_SET_MONEY,                      STR_NONE },                             // set money
    { WWT_BUTTON,           1,      XPL(0),                 WPL(0),                 YPL(5),         HPL(5),         STR_CHEAT_CLEAR_LOAN,               STR_NONE },                             // Clear loan
    { WWT_GROUPBOX,         1,      XPL(0) - GROUP_SPACE,   WPL(1) + GROUP_SPACE,   YPL(6.5),       HPL(10.5),      STR_DATE_SET,                       STR_NONE },                             // Date group
      SPINNER_WIDGETS      (1,      WPL(0),                 WPL(1) - 10,            YPL(7) + 2,     HPL(7) - 3,     STR_NONE,                           STR_NONE),                              // Year box (3 widgets)
      SPINNER_WIDGETS      (1,      WPL(0),                 WPL(1) - 10,            YPL(8) + 2,     HPL(8) - 3,     STR_NONE,                           STR_NONE),                              // Month box (3 widgets)
      SPINNER_WIDGETS      (1,      WPL(0),                 WPL(1) - 10,            YPL(9) + 2,     HPL(9) - 3,     STR_NONE,                           STR_NONE),                              // Day box (3 widgets)
    { WWT_BUTTON,           1,      XPL(0),                 WPL(0),                 YPL(10),        HPL(10),        STR_DATE_SET,                       STR_NONE },                             // Set Date
    { WWT_BUTTON,           1,      XPL(1),                 WPL(1),                 YPL(10),        HPL(10),        STR_DATE_RESET,                     STR_NONE },                             // Reset Date
    { WIDGETS_END },
};

static rct_widget window_cheats_guests_widgets[] =
{
    MAIN_CHEATS_WIDGETS,
    { WWT_GROUPBOX,         1,      XPL(0) - GROUP_SPACE,   WPL(1) + GROUP_SPACE,   YPL(0),         HPL(12.5),      STR_CHEAT_SET_GUESTS_PARAMETERS,    STR_NONE },                             // Guests parameters group frame
    { WWT_BUTTON,           1,      MAX_BTN_LEFT,           MAX_BTN_RIGHT,          YPL(1),         HPL(1),         STR_MAX,                            STR_NONE },                             // happiness max
    { WWT_BUTTON,           1,      MIN_BTN_LEFT,           MIN_BTN_RIGHT,          YPL(1),         HPL(1),         STR_MIN,                            STR_NONE },                             // happiness min
    { WWT_BUTTON,           1,      MAX_BTN_LEFT,           MAX_BTN_RIGHT,          YPL(2),         HPL(2),         STR_MAX,                            STR_NONE },                             // energy max
    { WWT_BUTTON,           1,      MIN_BTN_LEFT,           MIN_BTN_RIGHT,          YPL(2),         HPL(2),         STR_MIN,                            STR_NONE },                             // energy min
    { WWT_BUTTON,           1,      MAX_BTN_LEFT,           MAX_BTN_RIGHT,          YPL(3),         HPL(3),         STR_MAX,                            STR_NONE },                             // hunger max
    { WWT_BUTTON,           1,      MIN_BTN_LEFT,           MIN_BTN_RIGHT,          YPL(3),         HPL(3),         STR_MIN,                            STR_NONE },                             // hunger min
    { WWT_BUTTON,           1,      MAX_BTN_LEFT,           MAX_BTN_RIGHT,          YPL(4),         HPL(4),         STR_MAX,                            STR_NONE },                             // thirst max
    { WWT_BUTTON,           1,      MIN_BTN_LEFT,           MIN_BTN_RIGHT,          YPL(4),         HPL(4),         STR_MIN,                            STR_NONE },                             // thirst min
    { WWT_BUTTON,           1,      MAX_BTN_LEFT,           MAX_BTN_RIGHT,          YPL(5),         HPL(5),         STR_MAX,                            STR_NONE },                             // nausea max
    { WWT_BUTTON,           1,      MIN_BTN_LEFT,           MIN_BTN_RIGHT,          YPL(5),         HPL(5),         STR_MIN,                            STR_NONE },                             // nausea min
    { WWT_BUTTON,           1,      MAX_BTN_LEFT,           MAX_BTN_RIGHT,          YPL(6),         HPL(6),         STR_MAX,                            STR_NONE },                             // nausea tolerance max
    { WWT_BUTTON,           1,      MIN_BTN_LEFT,           MIN_BTN_RIGHT,          YPL(6),         HPL(6),         STR_MIN,                            STR_NONE },                             // nausea tolerance min
    { WWT_BUTTON,           1,      MAX_BTN_LEFT,           MAX_BTN_RIGHT,          YPL(7),         HPL(7),         STR_MAX,                            STR_NONE },                             // bathroom max
    { WWT_BUTTON,           1,      MIN_BTN_LEFT,           MIN_BTN_RIGHT,          YPL(7),         HPL(7),         STR_MIN,                            STR_NONE },                             // bathroom min
    { WWT_BUTTON,           1,      XPL(1),                 WPL(1),                 YPL(9),         HPL(9),         STR_CHEAT_MORE_THAN_1,              STR_NONE },                             // ride intensity > 1
    { WWT_BUTTON,           1,      XPL(0),                 WPL(0),                 YPL(9),         HPL(9),         STR_CHEAT_LESS_THAN_15,             STR_NONE },                             // ride intensity < 15
    { WWT_CHECKBOX,         1,      XPL(0),                 OWPL,                   YPL(10),        OHPL(10),       STR_CHEAT_IGNORE_INTENSITY,         STR_CHEAT_IGNORE_INTENSITY_TIP },       // guests ignore intensity
    { WWT_CHECKBOX,         1,      XPL(0),                 OWPL,                   YPL(11),        OHPL(11),       STR_CHEAT_DISABLE_VANDALISM,        STR_CHEAT_DISABLE_VANDALISM_TIP },      // disable vandalism
    { WWT_CHECKBOX,         1,      XPL(0),                 OWPL,                   YPL(12),        OHPL(12),       STR_CHEAT_DISABLE_LITTERING,        STR_CHEAT_DISABLE_LITTERING_TIP },      // disable littering
    { WWT_GROUPBOX,         1,      XPL(0) - GROUP_SPACE,   WPL(1) + GROUP_SPACE,   YPL(14),        HPL(16.5),      STR_CHEAT_GIVE_ALL_GUESTS,          STR_NONE },                             // Guests parameters group frame
    { WWT_BUTTON,           1,      XPL(0),                 WPL(0),                 YPL(15),        HPL(15),        STR_CHEAT_CURRENCY_FORMAT,          STR_NONE },                             // give guests money
    { WWT_BUTTON,           1,      XPL(1),                 WPL(1),                 YPL(15),        HPL(15),        STR_SHOP_ITEM_PLURAL_PARK_MAP,      STR_NONE },                             // give guests park maps
    { WWT_BUTTON,           1,      XPL(0),                 WPL(0),                 YPL(16),        HPL(16),        STR_SHOP_ITEM_PLURAL_BALLOON,       STR_NONE },                             // give guests balloons
    { WWT_BUTTON,           1,      XPL(1),                 WPL(1),                 YPL(16),        HPL(16),        STR_SHOP_ITEM_PLURAL_UMBRELLA,      STR_NONE },                             // give guests umbrellas
    { WWT_BUTTON,           1,      XPL(0),                 WPL(0),                 YPL(18),        HPL(18),        STR_CHEAT_LARGE_TRAM_GUESTS,        STR_CHEAT_LARGE_TRAM_GUESTS_TIP },      // large tram
    { WWT_BUTTON,           1,      XPL(1),                 WPL(1),                 YPL(18),        HPL(18),        STR_CHEAT_REMOVE_ALL_GUESTS,        STR_CHEAT_REMOVE_ALL_GUESTS_TIP },      // remove all guests
    { WWT_BUTTON,           1,      XPL(0),                 WPL(0),                 YPL(19),        HPL(19),        STR_CHEAT_EXPLODE,                  STR_CHEAT_EXPLODE_TIP },                // explode guests
    { WIDGETS_END },
};

//Strings for following moved to window_cheats_paint()
static rct_widget window_cheats_misc_widgets[] =
{
    MAIN_CHEATS_WIDGETS,
    { WWT_GROUPBOX,         1,      XPL(0) - GROUP_SPACE,   WPL(1) + GROUP_SPACE,   YPL(0),         HPL(7.25),      STR_CHEAT_GENERAL_GROUP,            STR_NONE },                             // General group
    { WWT_BUTTON,           1,      XPL(0),                 WPL(0),                 YPL(1),         HPL(1),         STR_CHEAT_OPEN_PARK,                STR_CHEAT_OPEN_PARK_TIP },              // open / close park
    { WWT_BUTTON,           1,      XPL(0),                 WPL(0),                 YPL(2),         HPL(2),         STR_CREATE_DUCKS,                   STR_CREATE_DUCKS },                     // Create ducks
    { WWT_BUTTON,           1,      XPL(1),                 WPL(1),                 YPL(1),         HPL(1),         STR_CHEAT_PARK_PARAMETERS,          STR_CHEAT_PARK_PARAMETERS_TIP },        // Park parameters
    { WWT_BUTTON,           1,      XPL(1),                 WPL(1),                 YPL(2),         HPL(2),         STR_REMOVE_DUCKS,                   STR_REMOVE_DUCKS },                     // Remove ducks
    { WWT_BUTTON,           1,      XPL(0),                 WPL(0),                 YPL(3),         HPL(3),         STR_CHEAT_OWN_ALL_LAND,             STR_CHEAT_OWN_ALL_LAND_TIP },           // Own all land
    { WWT_CHECKBOX,         1,      XPL(0),                 WPL(0),                 YPL(5),         HPL(5),         STR_FORCE_PARK_RATING,              STR_NONE },                             // Force park rating
      SPINNER_WIDGETS      (1,      XPL(1),                 WPL(1) - 10,            YPL(5) + 2,     HPL(5) - 3,     STR_NONE,                           STR_NONE),                              // park rating (3 widgets)
    { WWT_BUTTON,           1,      XPL(0),                 WPL(0),                 YPL(6),         HPL(6),         STR_CHEAT_WIN_SCENARIO,             STR_NONE },                             // Win scenario
    { WWT_BUTTON,           1,      XPL(1),                 WPL(1),                 YPL(6),         HPL(6),         STR_CHEAT_HAVE_FUN,                 STR_NONE },                             // Have fun!
    { WWT_CHECKBOX,         1,      XPL(0),                 OWPL,                   YPL(7),         HPL(7),         STR_CHEAT_NEVERENDING_MARKETING,    STR_CHEAT_NEVERENDING_MARKETING_TIP },  // never ending marketing campaigns
    { WWT_GROUPBOX,         1,      XPL(0) - GROUP_SPACE,   WPL(1) + GROUP_SPACE,   YPL(8.25),      HPL(10.5),      STR_CHEAT_WEATHER_GROUP,            STR_NONE },                             // Weather group
    { WWT_BUTTON,           1,      XPL(0),                 WPL(0),                 YPL(9),         HPL(9),         STR_CHEAT_FREEZE_WEATHER,           STR_CHEAT_FREEZE_WEATHER_TIP },         // Freeze weather
    { WWT_DROPDOWN,         1,      XPL(1),                 WPL(1),                 YPL(10) + 2,    YPL(10) + 13,   STR_NONE,                           STR_FORCE_WEATHER_TOOLTIP },            // Force weather
    { WWT_BUTTON,           1,      WPL(1) - 11,            WPL(1) - 1,             YPL(10) + 3,    YPL(10) + 12,   STR_DROPDOWN_GLYPH,                 STR_FORCE_WEATHER_TOOLTIP },            // Force weather
    { WWT_GROUPBOX,         1,      XPL(0) - GROUP_SPACE,   WPL(1) + GROUP_SPACE,   YPL(12),        HPL(17.5),      STR_CHEAT_STAFF_GROUP,              STR_NONE },                             // Staff group
    { WWT_BUTTON,           1,      XPL(0),                 WPL(0),                 YPL(13),        HPL(13),        STR_CHEAT_CLEAR_GRASS,              STR_NONE },                             // Clear grass
    { WWT_BUTTON,           1,      XPL(1),                 WPL(1),                 YPL(13),        HPL(13),        STR_CHEAT_MOWED_GRASS,              STR_NONE },                             // Mowed grass
    { WWT_BUTTON,           1,      XPL(0),                 WPL(0),                 YPL(14),        HPL(14),        STR_CHEAT_WATER_PLANTS,             STR_NONE },                             // Water plants
    { WWT_BUTTON,           1,      XPL(1),                 WPL(1),                 YPL(14),        HPL(14),        STR_CHEAT_FIX_VANDALISM,            STR_NONE },                             // Fix vandalism
    { WWT_BUTTON,           1,      XPL(0),                 WPL(0),                 YPL(15),        HPL(15),        STR_CHEAT_REMOVE_LITTER,            STR_NONE },                             // Remove litter
    { WWT_CHECKBOX,         1,      XPL(0),                 WPL(1),                 YPL(16),        HPL(16),        STR_CHEAT_DISABLE_PLANT_AGING,      STR_CHEAT_DISABLE_PLANT_AGING_TIP },    // Disable plant ageing
    { WWT_DROPDOWN,         1,      XPL(1),                 WPL(1),                 YPL(17) + 2,    YPL(17) + 13,   STR_NONE,                           STR_NONE },                             // Staff speed
    { WWT_BUTTON,           1,      WPL(1) - 11,            WPL(1) - 1,             YPL(17) + 3,    YPL(17) + 12,   STR_DROPDOWN_GLYPH,                 STR_NONE },                             // Staff speed
    { WIDGETS_END },
};
static rct_widget window_cheats_rides_widgets[] =
{
    MAIN_CHEATS_WIDGETS,
    { WWT_BUTTON,           1,      XPL(0),                 WPL(0),                 YPL(0),         HPL(0),         STR_CHEAT_FIX_ALL_RIDES,                        STR_CHEAT_FIX_ALL_RIDES_TIP },          // Fix all rides
    { WWT_BUTTON,           1,      XPL(1),                 WPL(1),                 YPL(0),         HPL(0),         STR_CHEAT_RENEW_RIDES,                          STR_CHEAT_RENEW_RIDES_TIP },            // Renew rides
    { WWT_BUTTON,           1,      XPL(0),                 WPL(0),                 YPL(1),         HPL(1),         STR_CHEAT_MAKE_DESTRUCTABLE,                    STR_CHEAT_MAKE_DESTRUCTABLE_TIP },      // All destructible
    { WWT_BUTTON,           1,      XPL(1),                 WPL(1),                 YPL(1),         HPL(1),         STR_CHEAT_RESET_CRASH_STATUS,                   STR_CHEAT_RESET_CRASH_STATUS_TIP },     // Reset crash status
    { WWT_BUTTON,           1,      XPL(0),                 WPL(0),                 YPL(2),         HPL(2),         STR_CHEAT_10_MINUTE_INSPECTIONS,                STR_CHEAT_10_MINUTE_INSPECTIONS_TIP },  // 10 minute inspections
    { WWT_GROUPBOX,         1,      XPL(0) - GROUP_SPACE,   WPL(1) + GROUP_SPACE,   YPL(3.25),      HPL(6.25),      STR_CHEAT_GROUP_CONSTRUCTION,                   STR_NONE },                             // Construction group
    { WWT_CHECKBOX,         1,      XPL(0),                 OWPL,                   YPL(4),         OHPL(4),        STR_CHEAT_BUILD_IN_PAUSE_MODE,                  STR_CHEAT_BUILD_IN_PAUSE_MODE_TIP },    // Build in pause mode
    { WWT_CHECKBOX,         1,      XPL(0),                 OWPL,                   YPL(5),         OHPL(5),        STR_CHEAT_ENABLE_ALL_DRAWABLE_TRACK_PIECES,     STR_CHEAT_ENABLE_ALL_DRAWABLE_TRACK_PIECES_TIP},   // Show all drawable track pieces
    { WWT_CHECKBOX,         1,      XPL(0),                 OWPL,                   YPL(6),         OHPL(6),        STR_CHEAT_ENABLE_CHAIN_LIFT_ON_ALL_TRACK,       STR_CHEAT_ENABLE_CHAIN_LIFT_ON_ALL_TRACK_TIP },    // Enable chain lift on all track
    { WWT_GROUPBOX,         1,      XPL(0) - GROUP_SPACE,   WPL(1) + GROUP_SPACE,   YPL(7.25),      HPL(12.25),     STR_CHEAT_GROUP_OPERATION,                      STR_NONE },                             // Construction group
    { WWT_CHECKBOX,         1,      XPL(0),                 OWPL,                   YPL(8),         OHPL(8),        STR_CHEAT_SHOW_ALL_OPERATING_MODES,             STR_NONE },                             // Show all operating modes
    { WWT_CHECKBOX,         1,      XPL(0),                 OWPL,                   YPL(9),         OHPL(9),        STR_CHEAT_UNLOCK_OPERATING_LIMITS,              STR_CHEAT_UNLOCK_OPERATING_LIMITS_TIP },// 410 km/h lift hill etc.
    { WWT_CHECKBOX,         1,      XPL(0),                 OWPL,                   YPL(10),        OHPL(10),       STR_CHEAT_DISABLE_BRAKES_FAILURE,               STR_CHEAT_DISABLE_BRAKES_FAILURE_TIP }, // Disable brakes failure
    { WWT_CHECKBOX,         1,      XPL(0),                 OWPL,                   YPL(11),        OHPL(11),       STR_CHEAT_DISABLE_BREAKDOWNS,                   STR_CHEAT_DISABLE_BREAKDOWNS_TIP },     // Disable all breakdowns
    { WWT_CHECKBOX,         1,      XPL(0),                 OWPL,                   YPL(12),        OHPL(12),       STR_CHEAT_DISABLE_RIDE_VALUE_AGING,             STR_CHEAT_DISABLE_RIDE_VALUE_AGING_TIP }, // Disable ride ageing
    { WWT_GROUPBOX,         1,      XPL(0) - GROUP_SPACE,   WPL(1) + GROUP_SPACE,   YPL(13.25),     HPL(17.25),     STR_CHEAT_GROUP_AVAILABILITY,                   STR_NONE },                             // Construction group
    { WWT_CHECKBOX,         1,      XPL(0),                 OWPL,                   YPL(14),        OHPL(14),       STR_CHEAT_ALLOW_ARBITRARY_RIDE_TYPE_CHANGES,    STR_CHEAT_ALLOW_ARBITRARY_RIDE_TYPE_CHANGES_TIP },  // Allow arbitrary ride type changes
    { WWT_CHECKBOX,         1,      XPL(0),                 OWPL,                   YPL(15),        OHPL(15),       STR_CHEAT_SHOW_VEHICLES_FROM_OTHER_TRACK_TYPES, STR_NONE },                 // Show vehicles from other track types
    { WWT_CHECKBOX,         1,      XPL(0),                 OWPL,                   YPL(16),        OHPL(16),       STR_CHEAT_DISABLE_TRAIN_LENGTH_LIMIT,           STR_CHEAT_DISABLE_TRAIN_LENGTH_LIMIT_TIP }, // Disable train length limits
    { WWT_CHECKBOX,         1,      XPL(0),                 OWPL,                   YPL(17),        OHPL(17),       STR_CHEAT_IGNORE_RESEARCH_STATUS,               STR_CHEAT_IGNORE_RESEARCH_STATUS_TIP},    // Ignore Research Status

    { WIDGETS_END },
};

static rct_widget *window_cheats_page_widgets[] =
{
    window_cheats_money_widgets,
    window_cheats_guests_widgets,
    window_cheats_misc_widgets,
    window_cheats_rides_widgets,
};

static void window_cheats_money_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_cheats_money_mousedown(rct_window *w, rct_widgetindex widgetIndex, rct_widget* widget);
static void window_cheats_misc_mousedown(rct_window *w, rct_widgetindex widgetIndex, rct_widget* widget);
static void window_cheats_misc_dropdown(rct_window *w, rct_widgetindex widgetIndex, int32_t dropdownIndex);
static void window_cheats_guests_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_cheats_misc_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_cheats_rides_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_cheats_update(rct_window *w);
static void window_cheats_invalidate(rct_window *w);
static void window_cheats_paint(rct_window *w, rct_drawpixelinfo *dpi);
static void window_cheats_set_page(rct_window *w, int32_t page);
static void window_cheats_text_input(rct_window *w, rct_widgetindex widgetIndex, char *text);

static rct_window_event_list window_cheats_money_events =
{
    nullptr,
    window_cheats_money_mouseup,
    nullptr,
    window_cheats_money_mousedown,
    nullptr,
    nullptr,
    window_cheats_update,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    window_cheats_text_input,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    window_cheats_invalidate,
    window_cheats_paint,
    nullptr
};

static rct_window_event_list window_cheats_guests_events =
{
    nullptr,
    window_cheats_guests_mouseup,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    window_cheats_update,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    window_cheats_invalidate,
    window_cheats_paint,
    nullptr
};

static rct_window_event_list window_cheats_misc_events =
{
    nullptr,
    window_cheats_misc_mouseup,
    nullptr,
    window_cheats_misc_mousedown,
    window_cheats_misc_dropdown,
    nullptr,
    window_cheats_update,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    window_cheats_invalidate,
    window_cheats_paint,
    nullptr
};

static rct_window_event_list window_cheats_rides_events =
{
    nullptr,
    window_cheats_rides_mouseup,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    window_cheats_update,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    window_cheats_invalidate,
    window_cheats_paint,
    nullptr
};


static rct_window_event_list *window_cheats_page_events[] =
{
    &window_cheats_money_events,
    &window_cheats_guests_events,
    &window_cheats_misc_events,
    &window_cheats_rides_events,
};

#define MAIN_CHEAT_ENABLED_WIDGETS (1ULL << WIDX_CLOSE) | (1ULL << WIDX_TAB_1) | (1ULL << WIDX_TAB_2) | (1ULL << WIDX_TAB_3) | (1ULL << WIDX_TAB_4)

static uint64_t window_cheats_page_enabled_widgets[] = {
    MAIN_CHEAT_ENABLED_WIDGETS |
    (1ULL << WIDX_NO_MONEY) |
    (1ULL << WIDX_ADD_SET_MONEY_GROUP) |
    (1ULL << WIDX_MONEY_SPINNER) |
    (1ULL << WIDX_MONEY_SPINNER_INCREMENT) |
    (1ULL << WIDX_MONEY_SPINNER_DECREMENT) |
    (1ULL << WIDX_ADD_MONEY) |
    (1ULL << WIDX_SET_MONEY) |
    (1ULL << WIDX_CLEAR_LOAN) |
    (1ULL << WIDX_DATE_SET) |
    (1ULL << WIDX_MONTH_BOX) |
    (1ULL << WIDX_MONTH_UP) |
    (1ULL << WIDX_MONTH_DOWN) |
    (1ULL << WIDX_YEAR_BOX) |
    (1ULL << WIDX_YEAR_UP) |
    (1ULL << WIDX_YEAR_DOWN) |
    (1ULL << WIDX_DAY_BOX) |
    (1ULL << WIDX_DAY_UP) |
    (1ULL << WIDX_DAY_DOWN) |
    (1ULL << WIDX_MONTH_BOX) |
    (1ULL << WIDX_DATE_GROUP) |
    (1ULL << WIDX_DATE_RESET),

    MAIN_CHEAT_ENABLED_WIDGETS |
    (1ULL << WIDX_GUEST_PARAMETERS_GROUP) |
    (1ULL << WIDX_GUEST_HAPPINESS_MAX) |
    (1ULL << WIDX_GUEST_HAPPINESS_MIN) |
    (1ULL << WIDX_GUEST_ENERGY_MAX) |
    (1ULL << WIDX_GUEST_ENERGY_MIN) |
    (1ULL << WIDX_GUEST_HUNGER_MAX) |
    (1ULL << WIDX_GUEST_HUNGER_MIN) |
    (1ULL << WIDX_GUEST_THIRST_MAX) |
    (1ULL << WIDX_GUEST_THIRST_MIN) |
    (1ULL << WIDX_GUEST_NAUSEA_MAX) |
    (1ULL << WIDX_GUEST_NAUSEA_MIN) |
    (1ULL << WIDX_GUEST_NAUSEA_TOLERANCE_MAX) |
    (1ULL << WIDX_GUEST_NAUSEA_TOLERANCE_MIN) |
    (1ULL << WIDX_GUEST_BATHROOM_MAX) |
    (1ULL << WIDX_GUEST_BATHROOM_MIN) |
    (1ULL << WIDX_GUEST_RIDE_INTENSITY_MORE_THAN_1) |
    (1ULL << WIDX_GUEST_RIDE_INTENSITY_LESS_THAN_15) |
    (1ULL << WIDX_GUEST_IGNORE_RIDE_INTENSITY) |
    (1ULL << WIDX_GIVE_ALL_GUESTS_GROUP) |
    (1ULL << WIDX_GIVE_GUESTS_MONEY) |
    (1ULL << WIDX_GIVE_GUESTS_PARK_MAPS) |
    (1ULL << WIDX_GIVE_GUESTS_BALLOONS) |
    (1ULL << WIDX_GIVE_GUESTS_UMBRELLAS) |
    (1ULL << WIDX_TRAM_GUESTS) |
    (1ULL << WIDX_REMOVE_ALL_GUESTS) |
    (1ULL << WIDX_EXPLODE_GUESTS) |
    (1ULL << WIDX_DISABLE_VANDALISM) |
    (1ULL << WIDX_DISABLE_LITTERING),

    MAIN_CHEAT_ENABLED_WIDGETS |
    (1ULL << WIDX_FREEZE_WEATHER) |
    (1ULL << WIDX_OPEN_CLOSE_PARK) |
    (1ULL << WIDX_CREATE_DUCKS) |
    (1ULL << WIDX_REMOVE_DUCKS) |
    (1ULL << WIDX_WEATHER) |
    (1ULL << WIDX_WEATHER_DROPDOWN_BUTTON) |
    (1ULL << WIDX_CLEAR_GRASS) |
    (1ULL << WIDX_MOWED_GRASS) |
    (1ULL << WIDX_WATER_PLANTS) |
    (1ULL << WIDX_DISABLE_PLANT_AGING) |
    (1ULL << WIDX_FIX_VANDALISM) |
    (1ULL << WIDX_REMOVE_LITTER) |
    (1ULL << WIDX_WIN_SCENARIO) |
    (1ULL << WIDX_HAVE_FUN) |
    (1ULL << WIDX_OWN_ALL_LAND) |
    (1ULL << WIDX_NEVERENDING_MARKETING) |
    (1ULL << WIDX_STAFF_SPEED) |
    (1ULL << WIDX_STAFF_SPEED_DROPDOWN_BUTTON) |
    (1ULL << WIDX_PARK_PARAMETERS) |
    (1ULL << WIDX_FORCE_PARK_RATING) |
    (1ULL << WIDX_INCREASE_PARK_RATING) |
    (1ULL << WIDX_DECREASE_PARK_RATING),

    MAIN_CHEAT_ENABLED_WIDGETS |
    (1ULL << WIDX_RENEW_RIDES) |
    (1ULL << WIDX_MAKE_DESTRUCTIBLE) |
    (1ULL << WIDX_FIX_ALL) |
    (1ULL << WIDX_FAST_LIFT_HILL) |
    (1ULL << WIDX_DISABLE_BRAKES_FAILURE) |
    (1ULL << WIDX_DISABLE_ALL_BREAKDOWNS) |
    (1ULL << WIDX_BUILD_IN_PAUSE_MODE) |
    (1ULL << WIDX_RESET_CRASH_STATUS) |
    (1ULL << WIDX_10_MINUTE_INSPECTIONS) |
    (1ULL << WIDX_SHOW_ALL_OPERATING_MODES) |
    (1ULL << WIDX_SHOW_VEHICLES_FROM_OTHER_TRACK_TYPES) |
    (1ULL << WIDX_DISABLE_TRAIN_LENGTH_LIMITS) |
    (1ULL << WIDX_ENABLE_CHAIN_LIFT_ON_ALL_TRACK) |
    (1ULL << WIDX_ENABLE_ARBITRARY_RIDE_TYPE_CHANGES) |
    (1ULL << WIDX_DISABLE_RIDE_VALUE_AGING) |
    (1ULL << WIDX_IGNORE_RESEARCH_STATUS) |
    (1ULL << WIDX_ENABLE_ALL_DRAWABLE_TRACK_PIECES)
};

static uint64_t window_cheats_page_hold_down_widgets[] = {
    (1ULL << WIDX_MONEY_SPINNER_INCREMENT) |
    (1ULL << WIDX_MONEY_SPINNER_DECREMENT) |
    (1ULL << WIDX_ADD_MONEY) |
    (1ULL << WIDX_YEAR_UP) |
    (1ULL << WIDX_YEAR_DOWN) |
    (1ULL << WIDX_MONTH_UP) |
    (1ULL << WIDX_MONTH_DOWN) |
    (1ULL << WIDX_DAY_UP) |
    (1ULL << WIDX_DAY_DOWN),

    0,

    (1ULL << WIDX_INCREASE_PARK_RATING) |
    (1ULL << WIDX_DECREASE_PARK_RATING),

    0
};

static rct_string_id window_cheats_page_titles[] = {
    STR_CHEAT_TITLE_FINANCIAL,
    STR_CHEAT_TITLE_GUEST,
    STR_CHEAT_TITLE_PARK,
    STR_CHEAT_TITLE_RIDE,
};
// clang-format on

static void window_cheats_draw_tab_images(rct_drawpixelinfo* dpi, rct_window* w);

rct_window* window_cheats_open()
{
    rct_window* window;

    // Check if window is already open
    window = window_bring_to_front_by_class(WC_CHEATS);
    if (window != nullptr)
        return window;

    window = window_create(ScreenCoordsXY(32, 32), WW, WH, &window_cheats_money_events, WC_CHEATS, 0);
    window->widgets = window_cheats_money_widgets;
    window->enabled_widgets = window_cheats_page_enabled_widgets[0];
    window->hold_down_widgets = window_cheats_page_hold_down_widgets[0];
    window_init_scroll_widgets(window);
    window_cheats_set_page(window, WINDOW_CHEATS_PAGE_MONEY);
    _parkRatingSpinnerValue = get_forced_park_rating() >= 0 ? get_forced_park_rating() : 999;

    return window;
}

static void window_cheats_money_mousedown(rct_window* w, rct_widgetindex widgetIndex, [[maybe_unused]] rct_widget* widget)
{
    switch (widgetIndex)
    {
        case WIDX_MONEY_SPINNER_INCREMENT:
            _moneySpinnerValue = add_clamp_money32(
                CHEATS_MONEY_INCREMENT_DIV * (_moneySpinnerValue / CHEATS_MONEY_INCREMENT_DIV), CHEATS_MONEY_INCREMENT_DIV);
            widget_invalidate_by_class(WC_CHEATS, WIDX_MONEY_SPINNER);
            break;
        case WIDX_MONEY_SPINNER_DECREMENT:
            _moneySpinnerValue = add_clamp_money32(
                CHEATS_MONEY_INCREMENT_DIV * (_moneySpinnerValue / CHEATS_MONEY_INCREMENT_DIV), -CHEATS_MONEY_INCREMENT_DIV);
            widget_invalidate_by_class(WC_CHEATS, WIDX_MONEY_SPINNER);
            break;
        case WIDX_ADD_MONEY:
            CheatsSet(CheatType::AddMoney, _moneySpinnerValue);
            break;
        case WIDX_YEAR_UP:
            _yearSpinnerValue++;
            _yearSpinnerValue = std::clamp(_yearSpinnerValue, 1, MAX_YEAR);
            widget_invalidate(w, WIDX_YEAR_BOX);
            break;
        case WIDX_YEAR_DOWN:
            _yearSpinnerValue--;
            _yearSpinnerValue = std::clamp(_yearSpinnerValue, 1, MAX_YEAR);
            widget_invalidate(w, WIDX_YEAR_BOX);
            break;
        case WIDX_MONTH_UP:
            _monthSpinnerValue++;
            _monthSpinnerValue = std::clamp(_monthSpinnerValue, 1, (int)MONTH_COUNT);
            _daySpinnerValue = std::clamp(_daySpinnerValue, 1, (int)days_in_month[_monthSpinnerValue - 1]);
            widget_invalidate(w, WIDX_MONTH_BOX);
            widget_invalidate(w, WIDX_DAY_BOX);
            break;
        case WIDX_MONTH_DOWN:
            _monthSpinnerValue--;
            _monthSpinnerValue = std::clamp(_monthSpinnerValue, 1, (int)MONTH_COUNT);
            _daySpinnerValue = std::clamp(_daySpinnerValue, 1, (int)days_in_month[_monthSpinnerValue - 1]);
            widget_invalidate(w, WIDX_MONTH_BOX);
            widget_invalidate(w, WIDX_DAY_BOX);
            break;
        case WIDX_DAY_UP:
            _daySpinnerValue++;
            _daySpinnerValue = std::clamp(_daySpinnerValue, 1, (int)days_in_month[_monthSpinnerValue - 1]);
            widget_invalidate(w, WIDX_DAY_BOX);
            break;
        case WIDX_DAY_DOWN:
            _daySpinnerValue--;
            _daySpinnerValue = std::clamp(_daySpinnerValue, 1, (int)days_in_month[_monthSpinnerValue - 1]);
            widget_invalidate(w, WIDX_DAY_BOX);
            break;
        case WIDX_DATE_SET:
        {
            auto setDateAction = ParkSetDateAction(_yearSpinnerValue, _monthSpinnerValue, _daySpinnerValue);
            GameActions::Execute(&setDateAction);
            window_invalidate_by_class(WC_BOTTOM_TOOLBAR);
            break;
        }
        case WIDX_DATE_RESET:
        {
            auto setDateAction = ParkSetDateAction(1, 1, 1);
            GameActions::Execute(&setDateAction);
            window_invalidate_by_class(WC_BOTTOM_TOOLBAR);
            widget_invalidate(w, WIDX_YEAR_BOX);
            widget_invalidate(w, WIDX_MONTH_BOX);
            widget_invalidate(w, WIDX_DAY_BOX);
            break;
        }
    }
}

static void window_cheats_misc_mousedown(rct_window* w, rct_widgetindex widgetIndex, rct_widget* widget)
{
    switch (widgetIndex)
    {
        case WIDX_INCREASE_PARK_RATING:
            _parkRatingSpinnerValue = std::min(999, 10 * (_parkRatingSpinnerValue / 10 + 1));
            widget_invalidate_by_class(WC_CHEATS, WIDX_PARK_RATING_SPINNER);
            if (get_forced_park_rating() >= 0)
            {
                auto setCheatAction = SetCheatAction(CheatType::SetForcedParkRating, _parkRatingSpinnerValue);
                GameActions::Execute(&setCheatAction);
            }
            break;
        case WIDX_DECREASE_PARK_RATING:
            _parkRatingSpinnerValue = std::max(0, 10 * (_parkRatingSpinnerValue / 10 - 1));
            widget_invalidate_by_class(WC_CHEATS, WIDX_PARK_RATING_SPINNER);
            if (get_forced_park_rating() >= 0)
            {
                CheatsSet(CheatType::SetForcedParkRating, _parkRatingSpinnerValue);
            }
            break;
        case WIDX_WEATHER_DROPDOWN_BUTTON:
        {
            rct_widget* dropdownWidget;
            int i, currentWeather;

            dropdownWidget = widget - 1;

            for (i = 0; i < 6; i++)
            {
                gDropdownItemsFormat[i] = STR_DROPDOWN_MENU_LABEL;
                gDropdownItemsArgs[i] = WeatherTypes[i];
            }
            window_dropdown_show_text_custom_width(
                w->x + dropdownWidget->left, w->y + dropdownWidget->top, dropdownWidget->bottom - dropdownWidget->top + 1,
                w->colours[1], 0, DROPDOWN_FLAG_STAY_OPEN, 6, dropdownWidget->right - dropdownWidget->left - 3);

            currentWeather = gClimateCurrent.Weather;
            dropdown_set_checked(currentWeather, true);
        }
        break;
        case WIDX_STAFF_SPEED_DROPDOWN_BUTTON:
        {
            rct_widget* dropdownWidget;

            dropdownWidget = widget - 1;

            for (size_t i = 0; i < std::size(_staffSpeedNames); i++)
            {
                gDropdownItemsArgs[i] = _staffSpeedNames[i];
                gDropdownItemsFormat[i] = STR_DROPDOWN_MENU_LABEL;
            }

            window_dropdown_show_text_custom_width(
                w->x + dropdownWidget->left, w->y + dropdownWidget->top, dropdownWidget->bottom - dropdownWidget->top + 1,
                w->colours[1], 0, DROPDOWN_FLAG_STAY_OPEN, 3, dropdownWidget->right - dropdownWidget->left - 3);
            dropdown_set_checked(_selectedStaffSpeed, true);
        }
    }
}

static void window_cheats_misc_dropdown([[maybe_unused]] rct_window* w, rct_widgetindex widgetIndex, int32_t dropdownIndex)
{
    if (dropdownIndex == -1)
    {
        return;
    }
    else if (widgetIndex == WIDX_WEATHER_DROPDOWN_BUTTON)
    {
        CheatsSet(CheatType::ForceWeather, dropdownIndex);
    }
    else if (widgetIndex == WIDX_STAFF_SPEED_DROPDOWN_BUTTON)
    {
        int32_t speed = CHEATS_STAFF_FAST_SPEED;
        switch (dropdownIndex)
        {
            case 0:
                speed = CHEATS_STAFF_FREEZE_SPEED;
                break;
            case 1:
                speed = CHEATS_STAFF_NORMAL_SPEED;
        }

        CheatsSet(CheatType::SetStaffSpeed, speed);
        _selectedStaffSpeed = dropdownIndex;
    }
}

static void window_cheats_money_mouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex)
    {
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
            CheatsSet(CheatType::NoMoney, gParkFlags & PARK_FLAGS_NO_MONEY ? 0 : 1);
            break;
        case WIDX_MONEY_SPINNER:
            money_to_string(_moneySpinnerValue, _moneySpinnerText, MONEY_STRING_MAXLENGTH, false);
            window_text_input_raw_open(
                w, WIDX_MONEY_SPINNER, STR_ENTER_NEW_VALUE, STR_ENTER_NEW_VALUE, _moneySpinnerText, MONEY_STRING_MAXLENGTH);
            break;
        case WIDX_SET_MONEY:
            CheatsSet(CheatType::SetMoney, _moneySpinnerValue);
            break;
        case WIDX_CLEAR_LOAN:
            CheatsSet(CheatType::ClearLoan, CHEATS_MONEY_DEFAULT);
            break;
    }
}

static void window_cheats_guests_mouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex)
    {
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
            CheatsSet(CheatType::SetGuestParameter, GUEST_PARAMETER_HAPPINESS, PEEP_MAX_HAPPINESS);
            break;
        case WIDX_GUEST_HAPPINESS_MIN:
            CheatsSet(CheatType::SetGuestParameter, GUEST_PARAMETER_HAPPINESS, 0);
            break;
        case WIDX_GUEST_ENERGY_MAX:
            CheatsSet(CheatType::SetGuestParameter, GUEST_PARAMETER_ENERGY, PEEP_MAX_ENERGY);
            break;
        case WIDX_GUEST_ENERGY_MIN:
            CheatsSet(CheatType::SetGuestParameter, GUEST_PARAMETER_ENERGY, PEEP_MIN_ENERGY);
            break;
        case WIDX_GUEST_HUNGER_MAX:
            CheatsSet(CheatType::SetGuestParameter, GUEST_PARAMETER_HUNGER, 0);
            break;
        case WIDX_GUEST_HUNGER_MIN:
            CheatsSet(CheatType::SetGuestParameter, GUEST_PARAMETER_HUNGER, PEEP_MAX_HUNGER);
            break;
        case WIDX_GUEST_THIRST_MAX:
            CheatsSet(CheatType::SetGuestParameter, GUEST_PARAMETER_THIRST, 0);
            break;
        case WIDX_GUEST_THIRST_MIN:
            CheatsSet(CheatType::SetGuestParameter, GUEST_PARAMETER_THIRST, PEEP_MAX_THIRST);
            break;
        case WIDX_GUEST_NAUSEA_MAX:
            CheatsSet(CheatType::SetGuestParameter, GUEST_PARAMETER_NAUSEA, PEEP_MAX_NAUSEA);
            break;
        case WIDX_GUEST_NAUSEA_MIN:
            CheatsSet(CheatType::SetGuestParameter, GUEST_PARAMETER_NAUSEA, 0);
            break;
        case WIDX_GUEST_NAUSEA_TOLERANCE_MAX:
            CheatsSet(CheatType::SetGuestParameter, GUEST_PARAMETER_NAUSEA_TOLERANCE, PEEP_NAUSEA_TOLERANCE_HIGH);
            break;
        case WIDX_GUEST_NAUSEA_TOLERANCE_MIN:
            CheatsSet(CheatType::SetGuestParameter, GUEST_PARAMETER_NAUSEA_TOLERANCE, PEEP_NAUSEA_TOLERANCE_NONE);
            break;
        case WIDX_GUEST_BATHROOM_MAX:
            CheatsSet(CheatType::SetGuestParameter, GUEST_PARAMETER_BATHROOM, PEEP_MAX_BATHROOM);
            break;
        case WIDX_GUEST_BATHROOM_MIN:
            CheatsSet(CheatType::SetGuestParameter, GUEST_PARAMETER_BATHROOM, 0);
            break;
        case WIDX_GUEST_RIDE_INTENSITY_MORE_THAN_1:
            CheatsSet(CheatType::SetGuestParameter, GUEST_PARAMETER_PREFERRED_RIDE_INTENSITY, 1);
            break;
        case WIDX_GUEST_RIDE_INTENSITY_LESS_THAN_15:
            CheatsSet(CheatType::SetGuestParameter, GUEST_PARAMETER_PREFERRED_RIDE_INTENSITY, 0);
            break;
        case WIDX_TRAM_GUESTS:
            CheatsSet(CheatType::GenerateGuests, CHEATS_TRAM_INCREMENT);
            break;
        case WIDX_REMOVE_ALL_GUESTS:
            CheatsSet(CheatType::RemoveAllGuests);
            break;
        case WIDX_EXPLODE_GUESTS:
            CheatsSet(CheatType::ExplodeGuests);
            break;
        case WIDX_GIVE_GUESTS_MONEY:
            CheatsSet(CheatType::GiveAllGuests, OBJECT_MONEY);
            break;
        case WIDX_GIVE_GUESTS_PARK_MAPS:
            CheatsSet(CheatType::GiveAllGuests, OBJECT_PARK_MAP);
            break;
        case WIDX_GIVE_GUESTS_BALLOONS:
            CheatsSet(CheatType::GiveAllGuests, OBJECT_BALLOON);
            break;
        case WIDX_GIVE_GUESTS_UMBRELLAS:
            CheatsSet(CheatType::GiveAllGuests, OBJECT_UMBRELLA);
            break;
        case WIDX_GUEST_IGNORE_RIDE_INTENSITY:
            CheatsSet(CheatType::IgnoreRideIntensity, !gCheatsIgnoreRideIntensity);
            break;
        case WIDX_DISABLE_VANDALISM:
            CheatsSet(CheatType::DisableVandalism, !gCheatsDisableVandalism);
            break;
        case WIDX_DISABLE_LITTERING:
            CheatsSet(CheatType::DisableLittering, !gCheatsDisableLittering);
            break;
    }
}

static void window_cheats_misc_mouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex)
    {
        case WIDX_CLOSE:
            window_close(w);
            break;
        case WIDX_TAB_1:
        case WIDX_TAB_2:
        case WIDX_TAB_3:
        case WIDX_TAB_4:
            window_cheats_set_page(w, widgetIndex - WIDX_TAB_1);
            break;
        case WIDX_FREEZE_WEATHER:
            CheatsSet(CheatType::FreezeWeather, !gCheatsFreezeWeather);
            break;
        case WIDX_OPEN_CLOSE_PARK:
            CheatsSet(CheatType::OpenClosePark);
            break;
        case WIDX_CREATE_DUCKS:
            CheatsSet(CheatType::CreateDucks, CHEATS_DUCK_INCREMENT);
            break;
        case WIDX_REMOVE_DUCKS:
            CheatsSet(CheatType::RemoveDucks);
            break;
        case WIDX_CLEAR_GRASS:
            CheatsSet(CheatType::SetGrassLength, GRASS_LENGTH_CLEAR_0);
            break;
        case WIDX_MOWED_GRASS:
            CheatsSet(CheatType::SetGrassLength, GRASS_LENGTH_MOWED);
            break;
        case WIDX_WATER_PLANTS:
            CheatsSet(CheatType::WaterPlants);
            break;
        case WIDX_FIX_VANDALISM:
            CheatsSet(CheatType::FixVandalism);
            break;
        case WIDX_REMOVE_LITTER:
            CheatsSet(CheatType::RemoveLitter);
            break;
        case WIDX_DISABLE_PLANT_AGING:
            CheatsSet(CheatType::DisablePlantAging, !gCheatsDisablePlantAging);
            break;
        case WIDX_WIN_SCENARIO:
            CheatsSet(CheatType::WinScenario);
            break;
        case WIDX_HAVE_FUN:
            CheatsSet(CheatType::HaveFun);
            break;
        case WIDX_OWN_ALL_LAND:
            CheatsSet(CheatType::OwnAllLand);
            break;
        case WIDX_NEVERENDING_MARKETING:
            CheatsSet(CheatType::NeverEndingMarketing, !gCheatsNeverendingMarketing);
            break;
        case WIDX_PARK_PARAMETERS:
            context_open_window(WC_EDITOR_SCENARIO_OPTIONS);
            break;
        case WIDX_FORCE_PARK_RATING:
            if (get_forced_park_rating() >= 0)
            {
                CheatsSet(CheatType::SetForcedParkRating, -1);
            }
            else
            {
                CheatsSet(CheatType::SetForcedParkRating, _parkRatingSpinnerValue);
            }
            break;
    }
}

static void window_cheats_rides_mouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex)
    {
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
            CheatsSet(CheatType::RenewRides);
            break;
        case WIDX_MAKE_DESTRUCTIBLE:
            CheatsSet(CheatType::MakeDestructible);
            break;
        case WIDX_FIX_ALL:
            CheatsSet(CheatType::FixRides);
            break;
        case WIDX_FAST_LIFT_HILL:
            CheatsSet(CheatType::FastLiftHill, !gCheatsFastLiftHill);

            break;
        case WIDX_DISABLE_BRAKES_FAILURE:
            CheatsSet(CheatType::DisableBrakesFailure, !gCheatsDisableBrakesFailure);
            break;
        case WIDX_DISABLE_ALL_BREAKDOWNS:
            CheatsSet(CheatType::DisableAllBreakdowns, !gCheatsDisableAllBreakdowns);
            break;
        case WIDX_BUILD_IN_PAUSE_MODE:
            CheatsSet(CheatType::BuildInPauseMode, !gCheatsBuildInPauseMode);
            break;
        case WIDX_RESET_CRASH_STATUS:
            CheatsSet(CheatType::ResetCrashStatus);
            break;
        case WIDX_10_MINUTE_INSPECTIONS:
            CheatsSet(CheatType::TenMinuteInspections);
            break;
        case WIDX_SHOW_ALL_OPERATING_MODES:
        {
            if (!gCheatsShowAllOperatingModes)
            {
                context_show_error(STR_WARNING_IN_CAPS, STR_THIS_FEATURE_IS_CURRENTLY_UNSTABLE);
            }
            CheatsSet(CheatType::ShowAllOperatingModes, !gCheatsShowAllOperatingModes);
        }
        break;
        case WIDX_SHOW_VEHICLES_FROM_OTHER_TRACK_TYPES:
        {
            if (!gCheatsShowVehiclesFromOtherTrackTypes)
            {
                context_show_error(STR_WARNING_IN_CAPS, STR_THIS_FEATURE_IS_CURRENTLY_UNSTABLE);
            }
            CheatsSet(CheatType::ShowVehiclesFromOtherTrackTypes, !gCheatsShowVehiclesFromOtherTrackTypes);
        }
        break;
        case WIDX_DISABLE_TRAIN_LENGTH_LIMITS:
        {
            if (!gCheatsDisableTrainLengthLimit)
            {
                context_show_error(STR_WARNING_IN_CAPS, STR_THIS_FEATURE_IS_CURRENTLY_UNSTABLE);
            }
            CheatsSet(CheatType::DisableTrainLengthLimit, !gCheatsDisableTrainLengthLimit);
        }
        break;
        case WIDX_ENABLE_CHAIN_LIFT_ON_ALL_TRACK:
            CheatsSet(CheatType::EnableChainLiftOnAllTrack, !gCheatsEnableChainLiftOnAllTrack);
            break;
        case WIDX_ENABLE_ARBITRARY_RIDE_TYPE_CHANGES:
        {
            if (!gCheatsAllowArbitraryRideTypeChanges)
            {
                context_show_error(STR_WARNING_IN_CAPS, STR_THIS_FEATURE_IS_CURRENTLY_UNSTABLE);
            }
            CheatsSet(CheatType::AllowArbitraryRideTypeChanges, !gCheatsAllowArbitraryRideTypeChanges);
        }
        break;
        case WIDX_DISABLE_RIDE_VALUE_AGING:
            CheatsSet(CheatType::DisableRideValueAging, !gCheatsDisableRideValueAging);
            break;
        case WIDX_IGNORE_RESEARCH_STATUS:
            CheatsSet(CheatType::IgnoreResearchStatus, !gCheatsIgnoreResearchStatus);
            break;
        case WIDX_ENABLE_ALL_DRAWABLE_TRACK_PIECES:
            CheatsSet(CheatType::EnableAllDrawableTrackPieces, !gCheatsEnableAllDrawableTrackPieces);
            break;
    }
}

static void window_cheats_text_input(rct_window* w, rct_widgetindex widgetIndex, char* text)
{
    if (text == nullptr)
        return;

    if (w->page == WINDOW_CHEATS_PAGE_MONEY && widgetIndex == WIDX_MONEY_SPINNER)
    {
        money32 val = string_to_money(text);
        if (val != MONEY32_UNDEFINED)
        {
            _moneySpinnerValue = val;
        }
        w->Invalidate();
    }
}

static void window_cheats_update(rct_window* w)
{
    w->frame_no++;
    widget_invalidate(w, WIDX_TAB_1 + w->page);
}

static void window_cheats_invalidate(rct_window* w)
{
    int32_t i;

    rct_widget* widgets = window_cheats_page_widgets[w->page];
    if (w->widgets != widgets)
    {
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

    switch (w->page)
    {
        case WINDOW_CHEATS_PAGE_MONEY:
        {
            widget_set_checkbox_value(w, WIDX_NO_MONEY, gParkFlags & PARK_FLAGS_NO_MONEY);

            uint64_t money_widgets = (1 << WIDX_ADD_SET_MONEY_GROUP) | (1 << WIDX_MONEY_SPINNER)
                | (1 << WIDX_MONEY_SPINNER_INCREMENT) | (1 << WIDX_MONEY_SPINNER_DECREMENT) | (1 << WIDX_ADD_MONEY)
                | (1 << WIDX_SET_MONEY) | (1 << WIDX_CLEAR_LOAN);
            if (gParkFlags & PARK_FLAGS_NO_MONEY)
            {
                w->disabled_widgets |= money_widgets;
            }
            else
            {
                w->disabled_widgets &= ~money_widgets;
            }
        }
        break;
        case WINDOW_CHEATS_PAGE_GUESTS:
            set_format_arg(0, int32_t, 10000);
            widget_set_checkbox_value(w, WIDX_GUEST_IGNORE_RIDE_INTENSITY, gCheatsIgnoreRideIntensity);
            widget_set_checkbox_value(w, WIDX_DISABLE_VANDALISM, gCheatsDisableVandalism);
            widget_set_checkbox_value(w, WIDX_DISABLE_LITTERING, gCheatsDisableLittering);
            break;
        case WINDOW_CHEATS_PAGE_MISC:
            w->widgets[WIDX_OPEN_CLOSE_PARK].text = (gParkFlags & PARK_FLAGS_PARK_OPEN) ? STR_CHEAT_CLOSE_PARK
                                                                                        : STR_CHEAT_OPEN_PARK;
            widget_set_checkbox_value(w, WIDX_FORCE_PARK_RATING, get_forced_park_rating() >= 0);
            w->widgets[WIDX_FREEZE_WEATHER].text = gCheatsFreezeWeather ? STR_CHEAT_UNFREEZE_WEATHER : STR_CHEAT_FREEZE_WEATHER;
            widget_set_checkbox_value(w, WIDX_NEVERENDING_MARKETING, gCheatsNeverendingMarketing);
            widget_set_checkbox_value(w, WIDX_DISABLE_PLANT_AGING, gCheatsDisablePlantAging);
            break;
        case WINDOW_CHEATS_PAGE_RIDES:
            set_format_arg(0, uint16_t, 255);
            widget_set_checkbox_value(w, WIDX_FAST_LIFT_HILL, gCheatsFastLiftHill);
            widget_set_checkbox_value(w, WIDX_DISABLE_BRAKES_FAILURE, gCheatsDisableBrakesFailure);
            widget_set_checkbox_value(w, WIDX_DISABLE_ALL_BREAKDOWNS, gCheatsDisableAllBreakdowns);
            widget_set_checkbox_value(w, WIDX_BUILD_IN_PAUSE_MODE, gCheatsBuildInPauseMode);
            widget_set_checkbox_value(w, WIDX_SHOW_ALL_OPERATING_MODES, gCheatsShowAllOperatingModes);
            widget_set_checkbox_value(w, WIDX_SHOW_VEHICLES_FROM_OTHER_TRACK_TYPES, gCheatsShowVehiclesFromOtherTrackTypes);
            widget_set_checkbox_value(w, WIDX_DISABLE_TRAIN_LENGTH_LIMITS, gCheatsDisableTrainLengthLimit);
            widget_set_checkbox_value(w, WIDX_ENABLE_CHAIN_LIFT_ON_ALL_TRACK, gCheatsEnableChainLiftOnAllTrack);
            widget_set_checkbox_value(w, WIDX_ENABLE_ARBITRARY_RIDE_TYPE_CHANGES, gCheatsAllowArbitraryRideTypeChanges);
            widget_set_checkbox_value(w, WIDX_DISABLE_RIDE_VALUE_AGING, gCheatsDisableRideValueAging);
            widget_set_checkbox_value(w, WIDX_IGNORE_RESEARCH_STATUS, gCheatsIgnoreResearchStatus);
            widget_set_checkbox_value(w, WIDX_ENABLE_ALL_DRAWABLE_TRACK_PIECES, gCheatsEnableAllDrawableTrackPieces);
            break;
    }

    // Current weather
    window_cheats_misc_widgets[WIDX_WEATHER].text = WeatherTypes[gClimateCurrent.Weather];
    // Staff speed
    window_cheats_misc_widgets[WIDX_STAFF_SPEED].text = _staffSpeedNames[_selectedStaffSpeed];
}

static void window_cheats_paint(rct_window* w, rct_drawpixelinfo* dpi)
{
    window_draw_widgets(w, dpi);
    window_cheats_draw_tab_images(dpi, w);

    if (w->page == WINDOW_CHEATS_PAGE_MONEY)
    {
        uint8_t colour = w->colours[1];
        set_format_arg(0, money32, _moneySpinnerValue);
        if (widget_is_disabled(w, WIDX_MONEY_SPINNER))
        {
            colour |= COLOUR_FLAG_INSET;
        }
        int32_t actual_month = _monthSpinnerValue - 1;
        gfx_draw_string_left(
            dpi, STR_BOTTOM_TOOLBAR_CASH, gCommonFormatArgs, colour, w->x + XPL(0) + TXTO, w->y + YPL(2) + TXTO);
        gfx_draw_string_left(dpi, STR_YEAR, nullptr, COLOUR_BLACK, w->x + XPL(0) + TXTO, w->y + YPL(7) + TXTO);
        gfx_draw_string_left(dpi, STR_MONTH, nullptr, COLOUR_BLACK, w->x + XPL(0) + TXTO, w->y + YPL(8) + TXTO);
        gfx_draw_string_left(dpi, STR_DAY, nullptr, COLOUR_BLACK, w->x + XPL(0) + TXTO, w->y + YPL(9) + TXTO);
        gfx_draw_string_right(
            dpi, STR_FORMAT_INTEGER, &_yearSpinnerValue, w->colours[1], w->x + WPL(1) - 34 - TXTO, w->y + YPL(7) + TXTO);
        gfx_draw_string_right(
            dpi, STR_FORMAT_MONTH, &actual_month, w->colours[1], w->x + WPL(1) - 34 - TXTO, w->y + YPL(8) + TXTO);
        gfx_draw_string_right(
            dpi, STR_FORMAT_INTEGER, &_daySpinnerValue, w->colours[1], w->x + WPL(1) - 34 - TXTO, w->y + YPL(9) + TXTO);
    }
    else if (w->page == WINDOW_CHEATS_PAGE_MISC)
    {
        gfx_draw_string_left(dpi, STR_CHEAT_STAFF_SPEED, nullptr, COLOUR_BLACK, w->x + XPL(0) + TXTO, w->y + YPL(17) + TXTO);
        gfx_draw_string_left(dpi, STR_FORCE_WEATHER, nullptr, COLOUR_BLACK, w->x + XPL(0) + TXTO, w->y + YPL(10) + TXTO);
        gfx_draw_string_right(
            dpi, STR_FORMAT_INTEGER, &_parkRatingSpinnerValue, w->colours[1], w->x + WPL(1) - 34 - TXTO, w->y + YPL(5) + TXTO);
    }
    else if (w->page == WINDOW_CHEATS_PAGE_GUESTS)
    {
        gfx_draw_string_left(dpi, STR_CHEAT_GUEST_HAPPINESS, nullptr, COLOUR_BLACK, w->x + XPL(0) + TXTO, w->y + YPL(1) + TXTO);
        gfx_draw_string_left(dpi, STR_CHEAT_GUEST_ENERGY, nullptr, COLOUR_BLACK, w->x + XPL(0) + TXTO, w->y + YPL(2) + TXTO);
        gfx_draw_string_left(dpi, STR_CHEAT_GUEST_HUNGER, nullptr, COLOUR_BLACK, w->x + XPL(0) + TXTO, w->y + YPL(3) + TXTO);
        gfx_draw_string_left(dpi, STR_CHEAT_GUEST_THIRST, nullptr, COLOUR_BLACK, w->x + XPL(0) + TXTO, w->y + YPL(4) + TXTO);
        gfx_draw_string_left(dpi, STR_CHEAT_GUEST_NAUSEA, nullptr, COLOUR_BLACK, w->x + XPL(0) + TXTO, w->y + YPL(5) + TXTO);
        gfx_draw_string_left(
            dpi, STR_CHEAT_GUEST_NAUSEA_TOLERANCE, nullptr, COLOUR_BLACK, w->x + XPL(0) + TXTO, w->y + YPL(6) + TXTO);
        gfx_draw_string_left(dpi, STR_CHEAT_GUEST_BATHROOM, nullptr, COLOUR_BLACK, w->x + XPL(0) + TXTO, w->y + YPL(7) + TXTO);
        gfx_draw_string_left(
            dpi, STR_CHEAT_GUEST_PREFERRED_INTENSITY, nullptr, COLOUR_BLACK, w->x + XPL(0) + TXTO, w->y + YPL(8) + TXTO);
    }
}

static void window_cheats_draw_tab_images(rct_drawpixelinfo* dpi, rct_window* w)
{
    int32_t sprite_idx;

    // Money tab
    if (!(w->disabled_widgets & (1 << WIDX_TAB_1)))
    {
        sprite_idx = SPR_TAB_FINANCES_SUMMARY_0;
        if (w->page == WINDOW_CHEATS_PAGE_MONEY)
            sprite_idx += (w->frame_no / 2) % 8;
        gfx_draw_sprite(dpi, sprite_idx, w->x + w->widgets[WIDX_TAB_1].left, w->y + w->widgets[WIDX_TAB_1].top, 0);
    }

    // Guests tab
    if (!(w->disabled_widgets & (1 << WIDX_TAB_2)))
    {
        sprite_idx = SPR_TAB_GUESTS_0;
        if (w->page == WINDOW_CHEATS_PAGE_GUESTS)
            sprite_idx += (w->frame_no / 3) % 8;
        gfx_draw_sprite(dpi, sprite_idx, w->x + w->widgets[WIDX_TAB_2].left, w->y + w->widgets[WIDX_TAB_2].top, 0);
    }

    // Misc tab
    if (!(w->disabled_widgets & (1 << WIDX_TAB_3)))
    {
        sprite_idx = SPR_TAB_PARK;
        gfx_draw_sprite(dpi, sprite_idx, w->x + w->widgets[WIDX_TAB_3].left, w->y + w->widgets[WIDX_TAB_3].top, 0);
    }

    // Rides tab
    if (!(w->disabled_widgets & (1 << WIDX_TAB_4)))
    {
        sprite_idx = SPR_TAB_RIDE_0;
        if (w->page == WINDOW_CHEATS_PAGE_RIDES)
            sprite_idx += (w->frame_no / 4) % 16;
        gfx_draw_sprite(dpi, sprite_idx, w->x + w->widgets[WIDX_TAB_4].left, w->y + w->widgets[WIDX_TAB_4].top, 0);
    }
}

static void window_cheats_set_page(rct_window* w, int32_t page)
{
    w->page = page;
    w->frame_no = 0;

    w->enabled_widgets = window_cheats_page_enabled_widgets[page];
    w->hold_down_widgets = window_cheats_page_hold_down_widgets[page];
    w->pressed_widgets = 0;

    w->event_handlers = window_cheats_page_events[page];
    w->widgets = window_cheats_page_widgets[page];

    int32_t maxY = 0;
    rct_widget* widget = &w->widgets[WIDX_TAB_CONTENT];
    while (widget->type != WWT_LAST)
    {
        maxY = std::max(maxY, (int32_t)widget->bottom);
        widget++;
    }
    maxY += 6;

    w->Invalidate();
    w->height = maxY;
    w->widgets[WIDX_BACKGROUND].bottom = maxY - 1;
    w->widgets[WIDX_PAGE_BACKGROUND].bottom = maxY - 1;
    w->Invalidate();
}
