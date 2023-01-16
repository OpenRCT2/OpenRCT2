/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
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
#include <openrct2/OpenRCT2.h>
#include <openrct2/actions/CheatSetAction.h>
#include <openrct2/actions/ParkSetDateAction.h>
#include <openrct2/config/Config.h>
#include <openrct2/localisation/Date.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/network/network.h>
#include <openrct2/sprites.h>
#include <openrct2/util/Util.h>
#include <openrct2/world/Climate.h>
#include <openrct2/world/Park.h>
#include <openrct2/world/Surface.h>

constexpr auto CHEATS_MONEY_DEFAULT = 10000.00_GBP;
constexpr auto CHEATS_MONEY_INCREMENT_DIV = 5000.00_GBP;

// clang-format off
enum
{
    WINDOW_CHEATS_PAGE_MONEY,
    WINDOW_CHEATS_PAGE_GUESTS,
    WINDOW_CHEATS_PAGE_MISC,
    WINDOW_CHEATS_PAGE_RIDES,
    WINDOW_CHEATS_PAGE_COUNT,
};

static StringId _staffSpeedNames[] =
{
    STR_FROZEN,
    STR_NORMAL,
    STR_FAST,
};

static constexpr const StringId WeatherTypes[] =
{
    STR_SUNNY,
    STR_PARTIALLY_CLOUDY,
    STR_CLOUDY,
    STR_RAIN,
    STR_HEAVY_RAIN,
    STR_THUNDERSTORM,
    STR_SNOW,
    STR_HEAVY_SNOW,
    STR_BLIZZARD,
};

enum WindowCheatsWidgetIdx
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
    WIDX_GUEST_TOILET_MAX,
    WIDX_GUEST_TOILET_MIN,
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

    WIDX_GENERAL_GROUP = WIDX_TAB_CONTENT,
    WIDX_OPEN_CLOSE_PARK,
    WIDX_CREATE_DUCKS,
    WIDX_OWN_ALL_LAND,
    WIDX_REMOVE_DUCKS,
    WIDX_OBJECTIVE_GROUP,
    WIDX_NEVERENDING_MARKETING,
    WIDX_FORCE_PARK_RATING,
    WIDX_PARK_RATING_SPINNER,
    WIDX_INCREASE_PARK_RATING,
    WIDX_DECREASE_PARK_RATING,
    WIDX_WIN_SCENARIO,
    WIDX_HAVE_FUN,
    WIDX_WEATHER_GROUP,
    WIDX_WEATHER,
    WIDX_WEATHER_DROPDOWN_BUTTON,
    WIDX_FREEZE_WEATHER,
    WIDX_MAINTENANCE_GROUP,
    WIDX_REMOVE_LITTER,
    WIDX_FIX_VANDALISM,
    WIDX_CLEAR_GRASS,
    WIDX_MOWED_GRASS,
    WIDX_WATER_PLANTS,
    WIDX_DISABLE_PLANT_AGING,
    WIDX_STAFF_GROUP,
    WIDX_STAFF_SPEED,
    WIDX_STAFF_SPEED_DROPDOWN_BUTTON,
    WIDX_PARK_CONSTRUCTION_GROUP,
    WIDX_ALLOW_REGULAR_PATH_AS_QUEUE,

    WIDX_FIX_ALL = WIDX_TAB_CONTENT,
    WIDX_RENEW_RIDES,
    WIDX_MAKE_DESTRUCTIBLE,
    WIDX_RESET_CRASH_STATUS,
    WIDX_10_MINUTE_INSPECTIONS,
    WIDX_CONSTRUCTION_GROUP,
    WIDX_BUILD_IN_PAUSE_MODE,
    WIDX_ENABLE_ALL_DRAWABLE_TRACK_PIECES,
    WIDX_ENABLE_CHAIN_LIFT_ON_ALL_TRACK,
    WIDX_ALLOW_TRACK_PLACE_INVALID_HEIGHTS,
    WIDX_OPERATION_MODES_GROUP,
    WIDX_SHOW_ALL_OPERATING_MODES,
    WIDX_UNLOCK_OPERATING_LIMITS,
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

static constexpr const StringId WINDOW_TITLE = STR_CHEAT_TITLE;
static constexpr const int32_t WW = 249;
static constexpr const int32_t WH = 300;

static constexpr ScreenSize CHEAT_BUTTON = {110, 17};
static constexpr ScreenSize CHEAT_CHECK = {221, 12};
static constexpr ScreenSize CHEAT_SPINNER = {117, 14};
static constexpr ScreenSize MINMAX_BUTTON = {55, 17};

static constexpr const int32_t TAB_WIDTH = 31;
static constexpr const int32_t TAB_START = 3;


#pragma endregion

#define MAIN_CHEATS_WIDGETS \
    WINDOW_SHIM(WINDOW_TITLE, WW, WH), \
    MakeWidget({ 0, 43}, {WW, 257}, WindowWidgetType::Resize, WindowColour::Secondary), /* tab content panel */ \
    MakeTab   ({ 3, 17}, STR_FINANCIAL_CHEATS_TIP                      ), /* tab 1 */ \
    MakeTab   ({34, 17}, STR_GUEST_CHEATS_TIP                          ), /* tab 2 */ \
    MakeTab   ({65, 17}, STR_PARK_CHEATS_TIP                           ), /* tab 3 */ \
    MakeTab   ({96, 17}, STR_RIDE_CHEATS_TIP                           )  /* tab 4 */

static Widget window_cheats_money_widgets[] =
{
    MAIN_CHEATS_WIDGETS,
    MakeWidget        ({ 11,  48}, CHEAT_BUTTON,  WindowWidgetType::Checkbox, WindowColour::Secondary, STR_MAKE_PARK_NO_MONEY), // No money
    MakeWidget        ({  5,  69}, {238,  69},    WindowWidgetType::Groupbox, WindowColour::Secondary, STR_ADD_SET_MONEY     ), // add / set money group frame
    MakeSpinnerWidgets({ 11,  92}, CHEAT_SPINNER, WindowWidgetType::Spinner,  WindowColour::Secondary                        ), // money value
    MakeWidget        ({ 11, 111}, CHEAT_BUTTON,  WindowWidgetType::Button,   WindowColour::Secondary, STR_ADD_MONEY         ), // add money
    MakeWidget        ({127, 111}, CHEAT_BUTTON,  WindowWidgetType::Button,   WindowColour::Secondary, STR_SET_MONEY         ), // set money
    MakeWidget        ({ 11, 153}, CHEAT_BUTTON,  WindowWidgetType::Button,   WindowColour::Secondary, STR_CHEAT_CLEAR_LOAN  ), // Clear loan
    MakeWidget        ({  5, 184}, {238, 101},    WindowWidgetType::Groupbox, WindowColour::Secondary, STR_DATE_SET          ), // Date group
    MakeSpinnerWidgets({120, 197}, CHEAT_SPINNER, WindowWidgetType::Spinner,  WindowColour::Secondary                        ), // Year box
    MakeSpinnerWidgets({120, 218}, CHEAT_SPINNER, WindowWidgetType::Spinner,  WindowColour::Secondary                        ), // Month box
    MakeSpinnerWidgets({120, 239}, CHEAT_SPINNER, WindowWidgetType::Spinner,  WindowColour::Secondary                        ), // Day box
    MakeWidget        ({ 11, 258}, CHEAT_BUTTON,  WindowWidgetType::Button,   WindowColour::Secondary, STR_DATE_SET          ), // Set Date
    MakeWidget        ({127, 258}, CHEAT_BUTTON,  WindowWidgetType::Button,   WindowColour::Secondary, STR_DATE_RESET        ), // Reset Date
    WIDGETS_END,
};

static Widget window_cheats_guests_widgets[] =
{
    MAIN_CHEATS_WIDGETS,
    MakeWidget({  5,  48}, {238, 279},    WindowWidgetType::Groupbox, WindowColour::Secondary, STR_CHEAT_SET_GUESTS_PARAMETERS                                 ), // Guests parameters group frame
    MakeWidget({183,  69}, MINMAX_BUTTON, WindowWidgetType::Button,   WindowColour::Secondary, STR_MAX                                                         ), // happiness max
    MakeWidget({127,  69}, MINMAX_BUTTON, WindowWidgetType::Button,   WindowColour::Secondary, STR_MIN                                                         ), // happiness min
    MakeWidget({183,  90}, MINMAX_BUTTON, WindowWidgetType::Button,   WindowColour::Secondary, STR_MAX                                                         ), // energy max
    MakeWidget({127,  90}, MINMAX_BUTTON, WindowWidgetType::Button,   WindowColour::Secondary, STR_MIN                                                         ), // energy min
    MakeWidget({183, 111}, MINMAX_BUTTON, WindowWidgetType::Button,   WindowColour::Secondary, STR_MAX                                                         ), // hunger max
    MakeWidget({127, 111}, MINMAX_BUTTON, WindowWidgetType::Button,   WindowColour::Secondary, STR_MIN                                                         ), // hunger min
    MakeWidget({183, 132}, MINMAX_BUTTON, WindowWidgetType::Button,   WindowColour::Secondary, STR_MAX                                                         ), // thirst max
    MakeWidget({127, 132}, MINMAX_BUTTON, WindowWidgetType::Button,   WindowColour::Secondary, STR_MIN                                                         ), // thirst min
    MakeWidget({183, 153}, MINMAX_BUTTON, WindowWidgetType::Button,   WindowColour::Secondary, STR_MAX                                                         ), // nausea max
    MakeWidget({127, 153}, MINMAX_BUTTON, WindowWidgetType::Button,   WindowColour::Secondary, STR_MIN                                                         ), // nausea min
    MakeWidget({183, 174}, MINMAX_BUTTON, WindowWidgetType::Button,   WindowColour::Secondary, STR_MAX                                                         ), // nausea tolerance max
    MakeWidget({127, 174}, MINMAX_BUTTON, WindowWidgetType::Button,   WindowColour::Secondary, STR_MIN                                                         ), // nausea tolerance min
    MakeWidget({183, 195}, MINMAX_BUTTON, WindowWidgetType::Button,   WindowColour::Secondary, STR_MAX                                                         ), // toilet max
    MakeWidget({127, 195}, MINMAX_BUTTON, WindowWidgetType::Button,   WindowColour::Secondary, STR_MIN                                                         ), // toilet min
    MakeWidget({127, 237}, CHEAT_BUTTON,  WindowWidgetType::Button,   WindowColour::Secondary, STR_CHEAT_MORE_THAN_1                                           ), // ride intensity > 1
    MakeWidget({ 11, 237}, CHEAT_BUTTON,  WindowWidgetType::Button,   WindowColour::Secondary, STR_CHEAT_LESS_THAN_15                                          ), // ride intensity < 15
    MakeWidget({ 11, 258}, CHEAT_CHECK,   WindowWidgetType::Checkbox, WindowColour::Secondary, STR_CHEAT_IGNORE_INTENSITY,      STR_CHEAT_IGNORE_INTENSITY_TIP ), // guests ignore intensity
    MakeWidget({ 11, 279}, CHEAT_CHECK,   WindowWidgetType::Checkbox, WindowColour::Secondary, STR_CHEAT_DISABLE_VANDALISM,     STR_CHEAT_DISABLE_VANDALISM_TIP), // disable vandalism
    MakeWidget({ 11, 300}, CHEAT_CHECK,   WindowWidgetType::Checkbox, WindowColour::Secondary, STR_CHEAT_DISABLE_LITTERING,     STR_CHEAT_DISABLE_LITTERING_TIP), // disable littering
    MakeWidget({  5, 342}, {238,  69},    WindowWidgetType::Groupbox, WindowColour::Secondary, STR_CHEAT_GIVE_ALL_GUESTS                                       ), // Guests parameters group frame
    MakeWidget({ 11, 363}, CHEAT_BUTTON,  WindowWidgetType::Button,   WindowColour::Secondary, STR_CURRENCY_FORMAT                                             ), // give guests money
    MakeWidget({127, 363}, CHEAT_BUTTON,  WindowWidgetType::Button,   WindowColour::Secondary, STR_SHOP_ITEM_PLURAL_PARK_MAP                                   ), // give guests park maps
    MakeWidget({ 11, 384}, CHEAT_BUTTON,  WindowWidgetType::Button,   WindowColour::Secondary, STR_SHOP_ITEM_PLURAL_BALLOON                                    ), // give guests balloons
    MakeWidget({127, 384}, CHEAT_BUTTON,  WindowWidgetType::Button,   WindowColour::Secondary, STR_SHOP_ITEM_PLURAL_UMBRELLA                                   ), // give guests umbrellas
    MakeWidget({ 11, 426}, CHEAT_BUTTON,  WindowWidgetType::Button,   WindowColour::Secondary, STR_CHEAT_LARGE_TRAM_GUESTS,     STR_CHEAT_LARGE_TRAM_GUESTS_TIP), // large tram
    MakeWidget({127, 426}, CHEAT_BUTTON,  WindowWidgetType::Button,   WindowColour::Secondary, STR_CHEAT_REMOVE_ALL_GUESTS,     STR_CHEAT_REMOVE_ALL_GUESTS_TIP), // remove all guests
    WIDGETS_END,
};

//Strings for following moved to window_cheats_paint()
static Widget window_cheats_misc_widgets[] =
{
    MAIN_CHEATS_WIDGETS,
    MakeWidget        ({  5,  48}, {238,  60},   WindowWidgetType::Groupbox, WindowColour::Secondary, STR_CHEAT_GENERAL_GROUP                                             ), // General group
    MakeWidget        ({ 11,  62}, CHEAT_BUTTON, WindowWidgetType::Button,   WindowColour::Secondary, STR_CHEAT_OPEN_PARK,             STR_CHEAT_OPEN_PARK_TIP            ), // open / close park
    MakeWidget        ({ 11,  83}, CHEAT_BUTTON, WindowWidgetType::Button,   WindowColour::Secondary, STR_CREATE_DUCKS,                STR_CREATE_DUCKS_TIP               ), // Create ducks
    MakeWidget        ({127,  62}, CHEAT_BUTTON, WindowWidgetType::Button,   WindowColour::Secondary, STR_CHEAT_OWN_ALL_LAND,          STR_CHEAT_OWN_ALL_LAND_TIP         ), // Own all land
    MakeWidget        ({127,  83}, CHEAT_BUTTON, WindowWidgetType::Button,   WindowColour::Secondary, STR_REMOVE_DUCKS,                STR_REMOVE_DUCKS_TIP               ), // Remove ducks

    MakeWidget        ({  5, 112}, {238,  75},   WindowWidgetType::Groupbox, WindowColour::Secondary, STR_CHEAT_OBJECTIVE_GROUP                                           ), // Objective group
    MakeWidget        ({ 11, 127}, CHEAT_CHECK,  WindowWidgetType::Checkbox, WindowColour::Secondary, STR_CHEAT_NEVERENDING_MARKETING, STR_CHEAT_NEVERENDING_MARKETING_TIP), // never ending marketing campaigns
    MakeWidget        ({ 11, 144}, {281,  12},   WindowWidgetType::Checkbox, WindowColour::Secondary, STR_FORCE_PARK_RATING                                               ), // Force park rating
    MakeSpinnerWidgets({156, 142}, { 81,  14},   WindowWidgetType::Spinner,  WindowColour::Secondary                                                                      ), // park rating (3 widgets)
    MakeWidget        ({ 11, 162}, CHEAT_BUTTON, WindowWidgetType::Button,   WindowColour::Secondary, STR_CHEAT_WIN_SCENARIO                                              ), // Win scenario
    MakeWidget        ({127, 162}, CHEAT_BUTTON, WindowWidgetType::Button,   WindowColour::Secondary, STR_CHEAT_HAVE_FUN                                                  ), // Have fun!

    MakeWidget        ({  5, 190}, {238,  50},   WindowWidgetType::Groupbox, WindowColour::Secondary, STR_CHEAT_WEATHER_GROUP                                             ), // Weather group
    MakeWidget        ({126, 204}, {111,  14},   WindowWidgetType::DropdownMenu, WindowColour::Secondary, STR_NONE,                        STR_CHANGE_WEATHER_TOOLTIP         ), // Force weather
    MakeWidget        ({225, 205}, { 11,  12},   WindowWidgetType::Button,   WindowColour::Secondary, STR_DROPDOWN_GLYPH,              STR_CHANGE_WEATHER_TOOLTIP         ), // Force weather
    MakeWidget        ({ 11, 222}, CHEAT_CHECK,  WindowWidgetType::Checkbox, WindowColour::Secondary, STR_CHEAT_FREEZE_WEATHER,        STR_CHEAT_FREEZE_WEATHER_TIP       ), // Freeze weather

    MakeWidget        ({  5, 244}, {238,  99},   WindowWidgetType::Groupbox, WindowColour::Secondary, STR_CHEAT_MAINTENANCE_GROUP                                         ), // Maintenance group
    MakeWidget        ({ 11, 259}, CHEAT_BUTTON, WindowWidgetType::Button,   WindowColour::Secondary, STR_CHEAT_REMOVE_LITTER                                             ), // Remove litter
    MakeWidget        ({127, 259}, CHEAT_BUTTON, WindowWidgetType::Button,   WindowColour::Secondary, STR_CHEAT_FIX_VANDALISM                                             ), // Fix vandalism
    MakeWidget        ({ 11, 280}, CHEAT_BUTTON, WindowWidgetType::Button,   WindowColour::Secondary, STR_CHEAT_CLEAR_GRASS                                               ), // Clear grass
    MakeWidget        ({127, 280}, CHEAT_BUTTON, WindowWidgetType::Button,   WindowColour::Secondary, STR_CHEAT_MOWED_GRASS                                               ), // Mowed grass
    MakeWidget        ({ 11, 301}, CHEAT_BUTTON, WindowWidgetType::Button,   WindowColour::Secondary, STR_CHEAT_WATER_PLANTS                                              ), // Water plants
    MakeWidget        ({ 11, 322}, CHEAT_CHECK,  WindowWidgetType::Checkbox, WindowColour::Secondary, STR_CHEAT_DISABLE_PLANT_AGING,   STR_CHEAT_DISABLE_PLANT_AGING_TIP  ), // Disable plant ageing

    MakeWidget        ({  5, 347}, {238,  35},   WindowWidgetType::Groupbox, WindowColour::Secondary, STR_CHEAT_STAFF_GROUP                                               ), // Staff group
    MakeWidget        ({126, 361}, {111,  14},   WindowWidgetType::DropdownMenu, WindowColour::Secondary                                                                      ), // Staff speed
    MakeWidget        ({225, 362}, { 11,  12},   WindowWidgetType::Button,   WindowColour::Secondary, STR_DROPDOWN_GLYPH                                                  ), // Staff speed

    MakeWidget        ({  5, 392}, {238,  35},   WindowWidgetType::Groupbox, WindowColour::Secondary, STR_CHEAT_GROUP_CONSTRUCTION                                        ), // Construction group
    MakeWidget        ({ 11, 407}, CHEAT_CHECK,  WindowWidgetType::Checkbox, WindowColour::Secondary, STR_CHEAT_ALLOW_PATH_AS_QUEUE,   STR_CHEAT_ALLOW_PATH_AS_QUEUE_TIP  ), // Allow regular footpaths as queue path

    WIDGETS_END,
};
static Widget window_cheats_rides_widgets[] =
{
    MAIN_CHEATS_WIDGETS,
    MakeWidget({ 11,  48}, CHEAT_BUTTON, WindowWidgetType::Button,   WindowColour::Secondary, STR_CHEAT_FIX_ALL_RIDES,                        STR_CHEAT_FIX_ALL_RIDES_TIP                    ), // Fix all rides
    MakeWidget({127,  48}, CHEAT_BUTTON, WindowWidgetType::Button,   WindowColour::Secondary, STR_CHEAT_RENEW_RIDES,                          STR_CHEAT_RENEW_RIDES_TIP                      ), // Renew rides
    MakeWidget({ 11,  69}, CHEAT_BUTTON, WindowWidgetType::Button,   WindowColour::Secondary, STR_CHEAT_MAKE_DESTRUCTABLE,                    STR_CHEAT_MAKE_DESTRUCTABLE_TIP                ), // All destructible
    MakeWidget({127,  69}, CHEAT_BUTTON, WindowWidgetType::Button,   WindowColour::Secondary, STR_CHEAT_RESET_CRASH_STATUS,                   STR_CHEAT_RESET_CRASH_STATUS_TIP               ), // Reset crash status
    MakeWidget({ 11,  90}, CHEAT_BUTTON, WindowWidgetType::Button,   WindowColour::Secondary, STR_CHEAT_10_MINUTE_INSPECTIONS,                STR_CHEAT_10_MINUTE_INSPECTIONS_TIP            ), // 10 minute inspections
    MakeWidget({  5, 116}, {238, 101},   WindowWidgetType::Groupbox, WindowColour::Secondary, STR_CHEAT_GROUP_CONSTRUCTION                                                                   ), // Construction group
    MakeWidget({ 11, 132}, CHEAT_CHECK,  WindowWidgetType::Checkbox, WindowColour::Secondary, STR_CHEAT_BUILD_IN_PAUSE_MODE,                  STR_CHEAT_BUILD_IN_PAUSE_MODE_TIP              ), // Build in pause mode
    MakeWidget({ 11, 153}, CHEAT_CHECK,  WindowWidgetType::Checkbox, WindowColour::Secondary, STR_CHEAT_ENABLE_ALL_DRAWABLE_TRACK_PIECES,     STR_CHEAT_ENABLE_ALL_DRAWABLE_TRACK_PIECES_TIP ), // Show all drawable track pieces
    MakeWidget({ 11, 174}, CHEAT_CHECK,  WindowWidgetType::Checkbox, WindowColour::Secondary, STR_CHEAT_ENABLE_CHAIN_LIFT_ON_ALL_TRACK,       STR_CHEAT_ENABLE_CHAIN_LIFT_ON_ALL_TRACK_TIP   ), // Enable chain lift on all track
    MakeWidget({ 11, 195}, CHEAT_CHECK,  WindowWidgetType::Checkbox, WindowColour::Secondary, STR_CHEAT_ALLOW_TRACK_PLACE_INVALID_HEIGHTS,    STR_CHEAT_ALLOW_TRACK_PLACE_INVALID_HEIGHTS_TIP), // Allow track place at invalid heights
    MakeWidget({  5, 221}, {238, 122},   WindowWidgetType::Groupbox, WindowColour::Secondary, STR_CHEAT_GROUP_OPERATION                                                                      ), // Operation group
    MakeWidget({ 11, 237}, CHEAT_CHECK,  WindowWidgetType::Checkbox, WindowColour::Secondary, STR_CHEAT_SHOW_ALL_OPERATING_MODES                                                             ), // Show all operating modes
    MakeWidget({ 11, 258}, CHEAT_CHECK,  WindowWidgetType::Checkbox, WindowColour::Secondary, STR_CHEAT_UNLOCK_OPERATING_LIMITS,              STR_CHEAT_UNLOCK_OPERATING_LIMITS_TIP          ), // 410 km/h lift hill etc.
    MakeWidget({ 11, 279}, CHEAT_CHECK,  WindowWidgetType::Checkbox, WindowColour::Secondary, STR_CHEAT_DISABLE_BRAKES_FAILURE,               STR_CHEAT_DISABLE_BRAKES_FAILURE_TIP           ), // Disable brakes failure
    MakeWidget({ 11, 300}, CHEAT_CHECK,  WindowWidgetType::Checkbox, WindowColour::Secondary, STR_CHEAT_DISABLE_BREAKDOWNS,                   STR_CHEAT_DISABLE_BREAKDOWNS_TIP               ), // Disable all breakdowns
    MakeWidget({ 11, 321}, CHEAT_CHECK,  WindowWidgetType::Checkbox, WindowColour::Secondary, STR_CHEAT_DISABLE_RIDE_VALUE_AGING,             STR_CHEAT_DISABLE_RIDE_VALUE_AGING_TIP         ), // Disable ride ageing
    MakeWidget({  5, 347}, {238, 101},   WindowWidgetType::Groupbox, WindowColour::Secondary, STR_CHEAT_GROUP_AVAILABILITY                                                                   ), // Availability group
    MakeWidget({ 11, 363}, CHEAT_CHECK,  WindowWidgetType::Checkbox, WindowColour::Secondary, STR_CHEAT_ALLOW_ARBITRARY_RIDE_TYPE_CHANGES,    STR_CHEAT_ALLOW_ARBITRARY_RIDE_TYPE_CHANGES_TIP), // Allow arbitrary ride type changes
    MakeWidget({ 11, 384}, CHEAT_CHECK,  WindowWidgetType::Checkbox, WindowColour::Secondary, STR_CHEAT_SHOW_VEHICLES_FROM_OTHER_TRACK_TYPES                                                 ), // Show vehicles from other track types
    MakeWidget({ 11, 405}, CHEAT_CHECK,  WindowWidgetType::Checkbox, WindowColour::Secondary, STR_CHEAT_DISABLE_TRAIN_LENGTH_LIMIT,           STR_CHEAT_DISABLE_TRAIN_LENGTH_LIMIT_TIP       ), // Disable train length limits
    MakeWidget({ 11, 426}, CHEAT_CHECK,  WindowWidgetType::Checkbox, WindowColour::Secondary, STR_CHEAT_IGNORE_RESEARCH_STATUS,               STR_CHEAT_IGNORE_RESEARCH_STATUS_TIP           ), // Ignore Research Status
    WIDGETS_END,
};

static Widget *window_cheats_page_widgets[] =
{
    window_cheats_money_widgets,
    window_cheats_guests_widgets,
    window_cheats_misc_widgets,
    window_cheats_rides_widgets,
};

static uint64_t window_cheats_page_hold_down_widgets[] = {
    (1uLL << WIDX_MONEY_SPINNER_INCREMENT) |
    (1uLL << WIDX_MONEY_SPINNER_DECREMENT) |
    (1uLL << WIDX_ADD_MONEY) |
    (1uLL << WIDX_YEAR_UP) |
    (1uLL << WIDX_YEAR_DOWN) |
    (1uLL << WIDX_MONTH_UP) |
    (1uLL << WIDX_MONTH_DOWN) |
    (1uLL << WIDX_DAY_UP) |
    (1uLL << WIDX_DAY_DOWN),

    0,

    (1uLL << WIDX_INCREASE_PARK_RATING) |
    (1uLL << WIDX_DECREASE_PARK_RATING),

    0,
};

static StringId window_cheats_page_titles[] = {
    STR_CHEAT_TITLE_FINANCIAL,
    STR_CHEAT_TITLE_GUEST,
    STR_CHEAT_TITLE_PARK,
    STR_CHEAT_TITLE_RIDE,
};
// clang-format on

class CheatsWindow final : public Window
{
private:
    char _moneySpinnerText[MONEY_STRING_MAXLENGTH]{};
    money64 _moneySpinnerValue = CHEATS_MONEY_DEFAULT;
    int32_t _selectedStaffSpeed = 1;
    int32_t _parkRatingSpinnerValue{};
    int32_t _yearSpinnerValue = 1;
    int32_t _monthSpinnerValue = 1;
    int32_t _daySpinnerValue = 1;

public:
    void OnOpen() override
    {
        SetPage(WINDOW_CHEATS_PAGE_MONEY);
        _parkRatingSpinnerValue = ParkGetForcedRating() >= 0 ? ParkGetForcedRating() : 999;
    }

    void OnUpdate() override
    {
        frame_no++;
        InvalidateWidget(WIDX_TAB_1 + page);
    }

    void OnMouseDown(WidgetIndex widgetIndex) override
    {
        switch (page)
        {
            case WINDOW_CHEATS_PAGE_MONEY:
                OnMouseDownMoney(widgetIndex);
                break;
            case WINDOW_CHEATS_PAGE_MISC:
                OnMouseDownMisc(widgetIndex);
                break;
        }
    }

    void OnMouseUp(WidgetIndex widgetIndex) override
    {
        switch (widgetIndex)
        {
            case WIDX_CLOSE:
                Close();
                break;
            case WIDX_TAB_1:
            case WIDX_TAB_2:
            case WIDX_TAB_3:
            case WIDX_TAB_4:
                SetPage(widgetIndex - WIDX_TAB_1);
                break;
            default:
                switch (page)
                {
                    case WINDOW_CHEATS_PAGE_MONEY:
                        OnMouseUpMoney(widgetIndex);
                        break;
                    case WINDOW_CHEATS_PAGE_GUESTS:
                        OnMouseUpGuests(widgetIndex);
                        break;
                    case WINDOW_CHEATS_PAGE_MISC:
                        OnMouseUpMisc(widgetIndex);
                        break;
                    case WINDOW_CHEATS_PAGE_RIDES:
                        OnMouseUpRides(widgetIndex);
                        break;
                }
                break;
        }
    }

    void OnDropdown(WidgetIndex widgetIndex, int32_t selectedIndex) override
    {
        if (page == WINDOW_CHEATS_PAGE_MISC)
        {
            OnDropdownMisc(widgetIndex, selectedIndex);
        }
    }

    void OnPrepareDraw() override
    {
        auto* targetWidgets = window_cheats_page_widgets[page];
        if (widgets != targetWidgets)
        {
            widgets = targetWidgets;
            WindowInitScrollWidgets(*this);
        }

        pressed_widgets = 0;
        disabled_widgets = 0;

        // Set correct active tab
        for (auto i = 0; i < WINDOW_CHEATS_PAGE_COUNT; i++)
            SetWidgetPressed(WIDX_TAB_1 + i, false);
        SetWidgetPressed(WIDX_TAB_1 + page, true);

        // Set title
        widgets[WIDX_TITLE].text = window_cheats_page_titles[page];

        switch (page)
        {
            case WINDOW_CHEATS_PAGE_MONEY:
            {
                auto moneyDisabled = (gParkFlags & PARK_FLAGS_NO_MONEY) != 0;
                SetCheckboxValue(WIDX_NO_MONEY, moneyDisabled);
                SetWidgetDisabled(WIDX_ADD_SET_MONEY_GROUP, moneyDisabled);
                SetWidgetDisabled(WIDX_MONEY_SPINNER, moneyDisabled);
                SetWidgetDisabled(WIDX_MONEY_SPINNER_INCREMENT, moneyDisabled);
                SetWidgetDisabled(WIDX_MONEY_SPINNER_DECREMENT, moneyDisabled);
                SetWidgetDisabled(WIDX_ADD_MONEY, moneyDisabled);
                SetWidgetDisabled(WIDX_SET_MONEY, moneyDisabled);
                SetWidgetDisabled(WIDX_CLEAR_LOAN, moneyDisabled);
                break;
            }
            case WINDOW_CHEATS_PAGE_GUESTS:
            {
                auto ft = Formatter::Common();
                ft.Add<money64>(1000.00_GBP);
                SetCheckboxValue(WIDX_GUEST_IGNORE_RIDE_INTENSITY, gCheatsIgnoreRideIntensity);
                SetCheckboxValue(WIDX_DISABLE_VANDALISM, gCheatsDisableVandalism);
                SetCheckboxValue(WIDX_DISABLE_LITTERING, gCheatsDisableLittering);
                break;
            }
            case WINDOW_CHEATS_PAGE_MISC:
                widgets[WIDX_OPEN_CLOSE_PARK].text = (gParkFlags & PARK_FLAGS_PARK_OPEN) ? STR_CHEAT_CLOSE_PARK
                                                                                         : STR_CHEAT_OPEN_PARK;
                SetCheckboxValue(WIDX_FORCE_PARK_RATING, ParkGetForcedRating() >= 0);
                SetCheckboxValue(WIDX_FREEZE_WEATHER, gCheatsFreezeWeather);
                SetCheckboxValue(WIDX_NEVERENDING_MARKETING, gCheatsNeverendingMarketing);
                SetCheckboxValue(WIDX_DISABLE_PLANT_AGING, gCheatsDisablePlantAging);
                SetCheckboxValue(WIDX_ALLOW_REGULAR_PATH_AS_QUEUE, gCheatsAllowRegularPathAsQueue);
                break;
            case WINDOW_CHEATS_PAGE_RIDES:
                SetCheckboxValue(WIDX_UNLOCK_OPERATING_LIMITS, gCheatsUnlockOperatingLimits);
                SetCheckboxValue(WIDX_DISABLE_BRAKES_FAILURE, gCheatsDisableBrakesFailure);
                SetCheckboxValue(WIDX_DISABLE_ALL_BREAKDOWNS, gCheatsDisableAllBreakdowns);
                SetCheckboxValue(WIDX_BUILD_IN_PAUSE_MODE, gCheatsBuildInPauseMode);
                SetCheckboxValue(WIDX_SHOW_ALL_OPERATING_MODES, gCheatsShowAllOperatingModes);
                SetCheckboxValue(WIDX_SHOW_VEHICLES_FROM_OTHER_TRACK_TYPES, gCheatsShowVehiclesFromOtherTrackTypes);
                SetCheckboxValue(WIDX_DISABLE_TRAIN_LENGTH_LIMITS, gCheatsDisableTrainLengthLimit);
                SetCheckboxValue(WIDX_ENABLE_CHAIN_LIFT_ON_ALL_TRACK, gCheatsEnableChainLiftOnAllTrack);
                SetCheckboxValue(WIDX_ENABLE_ARBITRARY_RIDE_TYPE_CHANGES, gCheatsAllowArbitraryRideTypeChanges);
                SetCheckboxValue(WIDX_DISABLE_RIDE_VALUE_AGING, gCheatsDisableRideValueAging);
                SetCheckboxValue(WIDX_IGNORE_RESEARCH_STATUS, gCheatsIgnoreResearchStatus);
                SetCheckboxValue(WIDX_ENABLE_ALL_DRAWABLE_TRACK_PIECES, gCheatsEnableAllDrawableTrackPieces);
                SetCheckboxValue(WIDX_ALLOW_TRACK_PLACE_INVALID_HEIGHTS, gCheatsAllowTrackPlaceInvalidHeights);
                break;
        }

        // Current weather
        window_cheats_misc_widgets[WIDX_WEATHER].text = WeatherTypes[EnumValue(gClimateCurrent.Weather)];
        // Staff speed
        window_cheats_misc_widgets[WIDX_STAFF_SPEED].text = _staffSpeedNames[_selectedStaffSpeed];

        if (gScreenFlags & SCREEN_FLAGS_EDITOR)
        {
            SetWidgetDisabled(WIDX_TAB_2, true);
            SetWidgetDisabled(WIDX_TAB_3, true);
            SetWidgetDisabled(WIDX_NO_MONEY, true);
        }
    }

    void OnDraw(rct_drawpixelinfo& dpi) override
    {
        UpdateTabPositions();
        DrawWidgets(dpi);
        DrawTabImages(dpi);

        static constexpr int16_t _xLcol = 14;
        static constexpr int16_t _xRcol = 208;

        if (page == WINDOW_CHEATS_PAGE_MONEY)
        {
            auto colour = colours[1];
            auto ft = Formatter();
            ft.Add<money64>(_moneySpinnerValue);
            if (IsWidgetDisabled(WIDX_MONEY_SPINNER))
            {
                colour |= COLOUR_FLAG_INSET;
            }
            int32_t actual_month = _monthSpinnerValue - 1;
            DrawTextBasic(&dpi, windowPos + ScreenCoordsXY{ _xLcol, 93 }, STR_BOTTOM_TOOLBAR_CASH, ft, { colour });
            DrawTextBasic(&dpi, windowPos + ScreenCoordsXY{ _xLcol, 198 }, STR_YEAR);
            DrawTextBasic(&dpi, windowPos + ScreenCoordsXY{ _xLcol, 219 }, STR_MONTH);
            DrawTextBasic(&dpi, windowPos + ScreenCoordsXY{ _xLcol, 240 }, STR_DAY);
            ft = Formatter();
            ft.Add<int32_t>(_yearSpinnerValue);
            DrawTextBasic(
                &dpi, windowPos + ScreenCoordsXY{ _xRcol, 198 }, STR_FORMAT_INTEGER, ft, { colours[1], TextAlignment::RIGHT });
            ft = Formatter();
            ft.Add<int32_t>(actual_month);
            DrawTextBasic(
                &dpi, windowPos + ScreenCoordsXY{ _xRcol, 219 }, STR_FORMAT_MONTH, ft, { colours[1], TextAlignment::RIGHT });
            ft = Formatter();
            ft.Add<int32_t>(_daySpinnerValue);
            DrawTextBasic(
                &dpi, windowPos + ScreenCoordsXY{ _xRcol, 240 }, STR_FORMAT_INTEGER, ft, { colours[1], TextAlignment::RIGHT });
        }
        else if (page == WINDOW_CHEATS_PAGE_MISC)
        {
            {
                auto& widget = widgets[WIDX_WEATHER];
                DrawTextBasic(&dpi, windowPos + ScreenCoordsXY{ _xLcol - 3, widget.top + 1 }, STR_CHANGE_WEATHER);
            }

            {
                auto ft = Formatter();
                ft.Add<int32_t>(_parkRatingSpinnerValue);

                auto& widget = widgets[WIDX_PARK_RATING_SPINNER];
                DrawTextBasic(
                    &dpi, windowPos + ScreenCoordsXY{ widget.left + 1, widget.top + 2 }, STR_FORMAT_INTEGER, ft,
                    { colours[1] });
            }

            {
                auto& widget = widgets[WIDX_STAFF_SPEED];
                DrawTextBasic(&dpi, windowPos + ScreenCoordsXY{ _xLcol - 3, widget.top + 1 }, STR_CHEAT_STAFF_SPEED);
            }
        }
        else if (page == WINDOW_CHEATS_PAGE_GUESTS)
        {
            DrawTextBasic(&dpi, windowPos + ScreenCoordsXY{ _xLcol, 72 }, STR_CHEAT_GUEST_HAPPINESS);
            DrawTextBasic(&dpi, windowPos + ScreenCoordsXY{ _xLcol, 93 }, STR_CHEAT_GUEST_ENERGY);
            DrawTextBasic(&dpi, windowPos + ScreenCoordsXY{ _xLcol, 114 }, STR_CHEAT_GUEST_HUNGER);
            DrawTextBasic(&dpi, windowPos + ScreenCoordsXY{ _xLcol, 135 }, STR_CHEAT_GUEST_THIRST);
            DrawTextBasic(&dpi, windowPos + ScreenCoordsXY{ _xLcol, 156 }, STR_CHEAT_GUEST_NAUSEA);
            DrawTextBasic(&dpi, windowPos + ScreenCoordsXY{ _xLcol, 177 }, STR_CHEAT_GUEST_NAUSEA_TOLERANCE);
            DrawTextBasic(&dpi, windowPos + ScreenCoordsXY{ _xLcol, 198 }, STR_CHEAT_GUEST_TOILET);
            DrawTextBasic(&dpi, windowPos + ScreenCoordsXY{ _xLcol, 219 }, STR_CHEAT_GUEST_PREFERRED_INTENSITY);
        }
    }

    void OnTextInput(WidgetIndex widgetIndex, std::string_view text) override
    {
        if (page == WINDOW_CHEATS_PAGE_MONEY && widgetIndex == WIDX_MONEY_SPINNER)
        {
            auto val = string_to_money(std::string(text).c_str());
            if (val != MONEY64_UNDEFINED)
            {
                _moneySpinnerValue = val;
            }
            Invalidate();
        }
    }

    OpenRCT2String OnTooltip(WidgetIndex widgetIndex, StringId fallback) override
    {
        if (page == WINDOW_CHEATS_PAGE_RIDES && widgetIndex == WIDX_UNLOCK_OPERATING_LIMITS)
        {
            auto ft = Formatter{};
            ft.Add<uint16_t>(255);
            return { fallback, ft };
        }
        return { fallback, {} };
    }

private:
    void SetPage(int32_t p)
    {
        page = p;
        frame_no = 0;

        hold_down_widgets = window_cheats_page_hold_down_widgets[p];
        pressed_widgets = 0;
        widgets = window_cheats_page_widgets[p];

        auto maxY = 0;
        auto* widget = &widgets[WIDX_TAB_CONTENT];
        while (widget->type != WindowWidgetType::Last)
        {
            maxY = std::max<int32_t>(maxY, widget->bottom);
            widget++;
        }
        maxY += 6;

        Invalidate();
        height = maxY;
        widgets[WIDX_BACKGROUND].bottom = maxY - 1;
        widgets[WIDX_PAGE_BACKGROUND].bottom = maxY - 1;
        Invalidate();
    }

    void UpdateTabPositions()
    {
        constexpr const uint16_t tabs[] = {
            WIDX_TAB_1,
            WIDX_TAB_2,
            WIDX_TAB_3,
            WIDX_TAB_4,
        };

        auto left = TAB_START;
        for (auto tab : tabs)
        {
            widgets[tab].left = left;
            if (!IsWidgetDisabled(tab))
            {
                left += TAB_WIDTH;
            }
        }
    }

    void DrawTabImages(rct_drawpixelinfo& dpi)
    {
        // Money tab
        if (!IsWidgetDisabled(WIDX_TAB_1))
        {
            uint32_t sprite_idx = SPR_TAB_FINANCES_SUMMARY_0;
            if (page == WINDOW_CHEATS_PAGE_MONEY)
                sprite_idx += (frame_no / 2) % 8;
            gfx_draw_sprite(
                &dpi, ImageId(sprite_idx), windowPos + ScreenCoordsXY{ widgets[WIDX_TAB_1].left, widgets[WIDX_TAB_1].top });
        }

        // Guests tab
        if (!IsWidgetDisabled(WIDX_TAB_2))
        {
            uint32_t sprite_idx = SPR_TAB_GUESTS_0;
            if (page == WINDOW_CHEATS_PAGE_GUESTS)
                sprite_idx += (frame_no / 3) % 8;
            gfx_draw_sprite(
                &dpi, ImageId(sprite_idx), windowPos + ScreenCoordsXY{ widgets[WIDX_TAB_2].left, widgets[WIDX_TAB_2].top });
        }

        // Misc tab
        if (!IsWidgetDisabled(WIDX_TAB_3))
        {
            gfx_draw_sprite(
                &dpi, ImageId(SPR_TAB_PARK), windowPos + ScreenCoordsXY{ widgets[WIDX_TAB_3].left, widgets[WIDX_TAB_3].top });
        }

        // Rides tab
        if (!IsWidgetDisabled(WIDX_TAB_4))
        {
            uint32_t sprite_idx = SPR_TAB_RIDE_0;
            if (page == WINDOW_CHEATS_PAGE_RIDES)
                sprite_idx += (frame_no / 4) % 16;
            gfx_draw_sprite(
                &dpi, ImageId(sprite_idx), windowPos + ScreenCoordsXY{ widgets[WIDX_TAB_4].left, widgets[WIDX_TAB_4].top });
        }
    }

    void OnMouseDownMoney(WidgetIndex widgetIndex)
    {
        switch (widgetIndex)
        {
            case WIDX_MONEY_SPINNER_INCREMENT:
                _moneySpinnerValue = add_clamp_money32(
                    CHEATS_MONEY_INCREMENT_DIV * (_moneySpinnerValue / CHEATS_MONEY_INCREMENT_DIV), CHEATS_MONEY_INCREMENT_DIV);
                InvalidateWidget(WIDX_MONEY_SPINNER);
                break;
            case WIDX_MONEY_SPINNER_DECREMENT:
                _moneySpinnerValue = add_clamp_money32(
                    CHEATS_MONEY_INCREMENT_DIV * (_moneySpinnerValue / CHEATS_MONEY_INCREMENT_DIV),
                    -CHEATS_MONEY_INCREMENT_DIV);
                InvalidateWidget(WIDX_MONEY_SPINNER);
                break;
            case WIDX_ADD_MONEY:
                CheatsSet(CheatType::AddMoney, _moneySpinnerValue);
                break;
            case WIDX_YEAR_UP:
                _yearSpinnerValue++;
                _yearSpinnerValue = std::clamp(_yearSpinnerValue, 1, MAX_YEAR);
                InvalidateWidget(WIDX_YEAR_BOX);
                break;
            case WIDX_YEAR_DOWN:
                _yearSpinnerValue--;
                _yearSpinnerValue = std::clamp(_yearSpinnerValue, 1, MAX_YEAR);
                InvalidateWidget(WIDX_YEAR_BOX);
                break;
            case WIDX_MONTH_UP:
                _monthSpinnerValue++;
                _monthSpinnerValue = std::clamp(_monthSpinnerValue, 1, static_cast<int32_t>(MONTH_COUNT));
                _daySpinnerValue = std::clamp(_daySpinnerValue, 1, static_cast<int32_t>(days_in_month[_monthSpinnerValue - 1]));
                InvalidateWidget(WIDX_MONTH_BOX);
                InvalidateWidget(WIDX_DAY_BOX);
                break;
            case WIDX_MONTH_DOWN:
                _monthSpinnerValue--;
                _monthSpinnerValue = std::clamp(_monthSpinnerValue, 1, static_cast<int32_t>(MONTH_COUNT));
                _daySpinnerValue = std::clamp(_daySpinnerValue, 1, static_cast<int32_t>(days_in_month[_monthSpinnerValue - 1]));
                InvalidateWidget(WIDX_MONTH_BOX);
                InvalidateWidget(WIDX_DAY_BOX);
                break;
            case WIDX_DAY_UP:
                _daySpinnerValue++;
                _daySpinnerValue = std::clamp(_daySpinnerValue, 1, static_cast<int32_t>(days_in_month[_monthSpinnerValue - 1]));
                InvalidateWidget(WIDX_DAY_BOX);
                break;
            case WIDX_DAY_DOWN:
                _daySpinnerValue--;
                _daySpinnerValue = std::clamp(_daySpinnerValue, 1, static_cast<int32_t>(days_in_month[_monthSpinnerValue - 1]));
                InvalidateWidget(WIDX_DAY_BOX);
                break;
            case WIDX_DATE_SET:
            {
                auto setDateAction = ParkSetDateAction(_yearSpinnerValue, _monthSpinnerValue, _daySpinnerValue);
                GameActions::Execute(&setDateAction);
                WindowInvalidateByClass(WindowClass::BottomToolbar);
                break;
            }
            case WIDX_DATE_RESET:
            {
                auto setDateAction = ParkSetDateAction(1, 1, 1);
                GameActions::Execute(&setDateAction);
                WindowInvalidateByClass(WindowClass::BottomToolbar);
                InvalidateWidget(WIDX_YEAR_BOX);
                InvalidateWidget(WIDX_MONTH_BOX);
                InvalidateWidget(WIDX_DAY_BOX);
                break;
            }
        }
    }

    void OnMouseUpMoney(WidgetIndex widgetIndex)
    {
        switch (widgetIndex)
        {
            case WIDX_NO_MONEY:
                CheatsSet(CheatType::NoMoney, gParkFlags & PARK_FLAGS_NO_MONEY ? 0 : 1);
                break;
            case WIDX_MONEY_SPINNER:
                money_to_string(_moneySpinnerValue, _moneySpinnerText, MONEY_STRING_MAXLENGTH, false);
                WindowTextInputRawOpen(
                    this, WIDX_MONEY_SPINNER, STR_ENTER_NEW_VALUE, STR_ENTER_NEW_VALUE, {}, _moneySpinnerText,
                    MONEY_STRING_MAXLENGTH);
                break;
            case WIDX_SET_MONEY:
                CheatsSet(CheatType::SetMoney, _moneySpinnerValue);
                break;
            case WIDX_CLEAR_LOAN:
                CheatsSet(CheatType::ClearLoan, CHEATS_MONEY_DEFAULT);
                break;
        }
    }

    void OnMouseDownMisc(WidgetIndex widgetIndex)
    {
        auto* widget = &widgets[widgetIndex];
        switch (widgetIndex)
        {
            case WIDX_INCREASE_PARK_RATING:
                _parkRatingSpinnerValue = std::min(999, 10 * (_parkRatingSpinnerValue / 10 + 1));
                InvalidateWidget(WIDX_PARK_RATING_SPINNER);
                if (ParkGetForcedRating() >= 0)
                {
                    auto cheatSetAction = CheatSetAction(CheatType::SetForcedParkRating, _parkRatingSpinnerValue);
                    GameActions::Execute(&cheatSetAction);
                }
                break;
            case WIDX_DECREASE_PARK_RATING:
                _parkRatingSpinnerValue = std::max(0, 10 * (_parkRatingSpinnerValue / 10 - 1));
                InvalidateWidget(WIDX_PARK_RATING_SPINNER);
                if (ParkGetForcedRating() >= 0)
                {
                    CheatsSet(CheatType::SetForcedParkRating, _parkRatingSpinnerValue);
                }
                break;
            case WIDX_WEATHER_DROPDOWN_BUTTON:
            {
                Widget* dropdownWidget = widget - 1;

                for (size_t i = 0; i < std::size(WeatherTypes); i++)
                {
                    gDropdownItems[i].Format = STR_DROPDOWN_MENU_LABEL;
                    gDropdownItems[i].Args = WeatherTypes[i];
                }
                WindowDropdownShowTextCustomWidth(
                    { windowPos.x + dropdownWidget->left, windowPos.y + dropdownWidget->top }, dropdownWidget->height() + 1,
                    colours[1], 0, Dropdown::Flag::StayOpen, std::size(WeatherTypes), dropdownWidget->width() - 3);

                auto currentWeather = gClimateCurrent.Weather;
                Dropdown::SetChecked(EnumValue(currentWeather), true);
            }
            break;
            case WIDX_STAFF_SPEED_DROPDOWN_BUTTON:
            {
                Widget* dropdownWidget;

                dropdownWidget = widget - 1;

                for (size_t i = 0; i < std::size(_staffSpeedNames); i++)
                {
                    gDropdownItems[i].Args = _staffSpeedNames[i];
                    gDropdownItems[i].Format = STR_DROPDOWN_MENU_LABEL;
                }

                WindowDropdownShowTextCustomWidth(
                    { windowPos.x + dropdownWidget->left, windowPos.y + dropdownWidget->top }, dropdownWidget->height() + 1,
                    colours[1], 0, Dropdown::Flag::StayOpen, 3, dropdownWidget->width() - 3);
                Dropdown::SetChecked(_selectedStaffSpeed, true);
            }
        }
    }

    void OnMouseUpMisc(WidgetIndex widgetIndex)
    {
        switch (widgetIndex)
        {
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
            case WIDX_FORCE_PARK_RATING:
                if (ParkGetForcedRating() >= 0)
                {
                    CheatsSet(CheatType::SetForcedParkRating, -1);
                }
                else
                {
                    CheatsSet(CheatType::SetForcedParkRating, _parkRatingSpinnerValue);
                }
                break;
            case WIDX_ALLOW_REGULAR_PATH_AS_QUEUE:
                CheatsSet(CheatType::AllowRegularPathAsQueue, !gCheatsAllowRegularPathAsQueue);
                break;
        }
    }

    void OnDropdownMisc(WidgetIndex widgetIndex, int32_t dropdownIndex)
    {
        if (dropdownIndex == -1)
        {
            return;
        }

        if (widgetIndex == WIDX_WEATHER_DROPDOWN_BUTTON)
        {
            CheatsSet(CheatType::ForceWeather, dropdownIndex);
        }
        if (widgetIndex == WIDX_STAFF_SPEED_DROPDOWN_BUTTON)
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

    void OnMouseUpGuests(WidgetIndex widgetIndex)
    {
        switch (widgetIndex)
        {
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
                CheatsSet(CheatType::SetGuestParameter, GUEST_PARAMETER_NAUSEA_TOLERANCE, EnumValue(PeepNauseaTolerance::High));
                break;
            case WIDX_GUEST_NAUSEA_TOLERANCE_MIN:
                CheatsSet(CheatType::SetGuestParameter, GUEST_PARAMETER_NAUSEA_TOLERANCE, EnumValue(PeepNauseaTolerance::None));
                break;
            case WIDX_GUEST_TOILET_MAX:
                CheatsSet(CheatType::SetGuestParameter, GUEST_PARAMETER_TOILET, PEEP_MAX_TOILET);
                break;
            case WIDX_GUEST_TOILET_MIN:
                CheatsSet(CheatType::SetGuestParameter, GUEST_PARAMETER_TOILET, 0);
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

    void OnMouseUpRides(WidgetIndex widgetIndex)
    {
        switch (widgetIndex)
        {
            case WIDX_RENEW_RIDES:
                CheatsSet(CheatType::RenewRides);
                break;
            case WIDX_MAKE_DESTRUCTIBLE:
                CheatsSet(CheatType::MakeDestructible);
                break;
            case WIDX_FIX_ALL:
                CheatsSet(CheatType::FixRides);
                break;
            case WIDX_UNLOCK_OPERATING_LIMITS:
                CheatsSet(CheatType::FastLiftHill, !gCheatsUnlockOperatingLimits);
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
                    ContextShowError(STR_WARNING_IN_CAPS, STR_THIS_FEATURE_IS_CURRENTLY_UNSTABLE, {});
                }
                CheatsSet(CheatType::ShowAllOperatingModes, !gCheatsShowAllOperatingModes);
            }
            break;
            case WIDX_SHOW_VEHICLES_FROM_OTHER_TRACK_TYPES:
            {
                if (!gCheatsShowVehiclesFromOtherTrackTypes)
                {
                    ContextShowError(STR_WARNING_IN_CAPS, STR_THIS_FEATURE_IS_CURRENTLY_UNSTABLE, {});
                }
                CheatsSet(CheatType::ShowVehiclesFromOtherTrackTypes, !gCheatsShowVehiclesFromOtherTrackTypes);
            }
            break;
            case WIDX_DISABLE_TRAIN_LENGTH_LIMITS:
            {
                if (!gCheatsDisableTrainLengthLimit)
                {
                    ContextShowError(STR_WARNING_IN_CAPS, STR_THIS_FEATURE_IS_CURRENTLY_UNSTABLE, {});
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
                    ContextShowError(STR_WARNING_IN_CAPS, STR_THIS_FEATURE_IS_CURRENTLY_UNSTABLE, {});
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
            case WIDX_ALLOW_TRACK_PLACE_INVALID_HEIGHTS:
            {
                if (!gCheatsAllowTrackPlaceInvalidHeights)
                {
                    ContextShowError(STR_WARNING_IN_CAPS, STR_THIS_FEATURE_IS_CURRENTLY_UNSTABLE, {});
                }
                CheatsSet(CheatType::AllowTrackPlaceInvalidHeights, !gCheatsAllowTrackPlaceInvalidHeights);
            }
            break;
        }
    }
};

rct_window* WindowCheatsOpen()
{
    auto* window = WindowBringToFrontByClass(WindowClass::Cheats);
    if (window == nullptr)
    {
        window = WindowCreate<CheatsWindow>(WindowClass::Cheats, ScreenCoordsXY(32, 32), WW, WH);
    }
    return window;
}
