/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../UiStringIds.h"

#include <iterator>
#include <openrct2-ui/interface/Dropdown.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Windows.h>
#include <openrct2/Context.h>
#include <openrct2/Game.h>
#include <openrct2/GameState.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/actions/CheatSetAction.h>
#include <openrct2/actions/ParkSetDateAction.h>
#include <openrct2/core/EnumUtils.hpp>
#include <openrct2/localisation/Currency.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/localisation/Localisation.Date.h>
#include <openrct2/network/network.h>
#include <openrct2/sprites.h>
#include <openrct2/ui/WindowManager.h>
#include <openrct2/util/Util.h>
#include <openrct2/world/Park.h>
#include <openrct2/world/tile_element/SurfaceElement.h>

namespace OpenRCT2::Ui::Windows
{
    constexpr auto CHEATS_MONEY_DEFAULT = 10000.00_GBP;
    constexpr auto CHEATS_MONEY_INCREMENT_DIV = 5000.00_GBP;

    // clang-format off
enum
{
    WINDOW_CHEATS_PAGE_MONEY,
    WINDOW_CHEATS_PAGE_DATE,
    WINDOW_CHEATS_PAGE_GUESTS,
    WINDOW_CHEATS_PAGE_STAFF,
    WINDOW_CHEATS_PAGE_PARK,
    WINDOW_CHEATS_PAGE_RIDES,
    WINDOW_CHEATS_PAGE_WEATHER,
    WINDOW_CHEATS_PAGE_COUNT,
};

static StringId _staffSpeedNames[] =
{
    STR_NORMAL,
    STR_FROZEN,
    STR_FAST,
};

static constexpr StringId WeatherTypes[] =
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
    WIDX_TAB_5,
    WIDX_TAB_6,
    WIDX_TAB_7,
    WIDX_TAB_CONTENT,

    WIDX_NO_MONEY = WIDX_TAB_CONTENT,
    WIDX_ADD_SET_MONEY_GROUP,
    WIDX_MONEY_SPINNER,
    WIDX_MONEY_SPINNER_INCREMENT,
    WIDX_MONEY_SPINNER_DECREMENT,
    WIDX_ADD_MONEY,
    WIDX_SET_MONEY,
    WIDX_CLEAR_LOAN,

    WIDX_DATE_GROUP = WIDX_TAB_CONTENT,
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

    WIDX_TRAM_GUESTS = WIDX_TAB_CONTENT,
    WIDX_REMOVE_ALL_GUESTS,
    WIDX_GUEST_PARAMETERS_GROUP,
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
    WIDX_GIVE_ALL_GUESTS_GROUP,
    WIDX_GIVE_GUESTS_MONEY,
    WIDX_GIVE_GUESTS_PARK_MAPS,
    WIDX_GIVE_GUESTS_BALLOONS,
    WIDX_GIVE_GUESTS_UMBRELLAS,
    WIDX_GUEST_BEHAVIOUR_GROUP,
    WIDX_GUEST_IGNORE_RIDE_INTENSITY,
    WIDX_GUEST_IGNORE_PRICE,
    WIDX_DISABLE_VANDALISM,
    WIDX_DISABLE_LITTERING,

    WIDX_STAFF_GROUP = WIDX_TAB_CONTENT,
    WIDX_STAFF_SPEED,
    WIDX_STAFF_SPEED_DROPDOWN_BUTTON,
    WIDX_MAINTENANCE_GROUP,
    WIDX_REMOVE_LITTER,
    WIDX_FIX_VANDALISM,
    WIDX_CLEAR_GRASS,
    WIDX_MOWED_GRASS,
    WIDX_WATER_PLANTS,
    WIDX_DISABLE_PLANT_AGING,

    WIDX_GENERAL_GROUP = WIDX_TAB_CONTENT,
    WIDX_OWN_ALL_LAND,
    WIDX_REMOVE_PARK_FENCES,
    WIDX_OPEN_CLOSE_PARK,
    WIDX_OBJECTIVE_GROUP,
    WIDX_NEVERENDING_MARKETING,
    WIDX_FORCE_PARK_RATING,
    WIDX_PARK_RATING_SPINNER,
    WIDX_INCREASE_PARK_RATING,
    WIDX_DECREASE_PARK_RATING,
    WIDX_WIN_SCENARIO,
    WIDX_HAVE_FUN,
    WIDX_PARK_CONSTRUCTION_GROUP,
    WIDX_ALLOW_BUILD_IN_PAUSE_MODE,
    WIDX_ALLOW_REGULAR_PATH_AS_QUEUE,
    WIDX_ALLOW_SPECIAL_COLOUR_SCHEMES,

    WIDX_FIX_ALL = WIDX_TAB_CONTENT,
    WIDX_RENEW_RIDES,
    WIDX_RESET_CRASH_STATUS,
    WIDX_10_MINUTE_INSPECTIONS,
    WIDX_CONSTRUCTION_GROUP,
    WIDX_ENABLE_ALL_DRAWABLE_TRACK_PIECES,
    WIDX_ENABLE_CHAIN_LIFT_ON_ALL_TRACK,
    WIDX_ALLOW_TRACK_PLACE_INVALID_HEIGHTS,
    WIDX_MAKE_DESTRUCTIBLE,
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

    WIDX_WEATHER_GROUP = WIDX_TAB_CONTENT,
    WIDX_WEATHER,
    WIDX_WEATHER_DROPDOWN_BUTTON,
    WIDX_FREEZE_WEATHER,
    WIDX_FAUNA_GROUP,
    WIDX_CREATE_DUCKS,
    WIDX_REMOVE_DUCKS,
};

#pragma region MEASUREMENTS

static constexpr StringId WINDOW_TITLE = STR_CHEAT_TITLE;
static constexpr int32_t WW = 249;
static constexpr int32_t WH = 300;

static constexpr ScreenSize CHEAT_BUTTON = {110, 17};
static constexpr ScreenSize CHEAT_CHECK = {221, 12};
static constexpr ScreenSize CHEAT_SPINNER = {117, 14};
static constexpr ScreenSize MINMAX_BUTTON = {55, 17};

static constexpr int32_t TAB_WIDTH = 31;
static constexpr int32_t TAB_START = 3;

#pragma endregion

#define MAIN_CHEATS_WIDGETS \
    WINDOW_SHIM(WINDOW_TITLE, WW, WH), \
    MakeWidget({  0, 43}, {WW, 257}, WindowWidgetType::Resize, WindowColour::Secondary), /* tab content panel */ \
    MakeTab   ({  3, 17}, STR_FINANCIAL_CHEATS_TIP                      ), /* tab 1 */ \
    MakeTab   ({ 34, 17}, STR_DATE_CHEATS_TIP                           ), /* tab 2 */ \
    MakeTab   ({ 65, 17}, STR_GUEST_CHEATS_TIP                          ), /* tab 3 */ \
    MakeTab   ({ 96, 17}, STR_STAFF_CHEATS_TIP                          ), /* tab 5 */ \
    MakeTab   ({127, 17}, STR_PARK_CHEATS_TIP                           ), /* tab 6 */ \
    MakeTab   ({158, 17}, STR_RIDE_CHEATS_TIP                           ), /* tab 4 */ \
    MakeTab   ({189, 17}, STR_WEATHER_NATURE_CHEATS_TIP                 )  /* tab 7 */

static constexpr Widget window_cheats_money_widgets[] =
{
    MAIN_CHEATS_WIDGETS,
    MakeWidget        ({ 11,  48}, CHEAT_BUTTON,  WindowWidgetType::Checkbox, WindowColour::Secondary, STR_MAKE_PARK_NO_MONEY), // No money
    MakeWidget        ({  5,  69}, {238, 69},     WindowWidgetType::Groupbox, WindowColour::Secondary, STR_ADD_SET_MONEY     ), // add / set money group frame
    MakeSpinnerWidgets({ 11,  92}, CHEAT_SPINNER, WindowWidgetType::Spinner,  WindowColour::Secondary                        ), // money value
    MakeWidget        ({ 11, 111}, CHEAT_BUTTON,  WindowWidgetType::Button,   WindowColour::Secondary, STR_ADD_MONEY         ), // add money
    MakeWidget        ({127, 111}, CHEAT_BUTTON,  WindowWidgetType::Button,   WindowColour::Secondary, STR_SET_MONEY         ), // set money
    MakeWidget        ({ 11, 145}, CHEAT_BUTTON,  WindowWidgetType::Button,   WindowColour::Secondary, STR_CHEAT_CLEAR_LOAN  ), // Clear loan
};

static constexpr Widget window_cheats_date_widgets[] =
{
    MAIN_CHEATS_WIDGETS,
    MakeWidget        ({  5,  48}, {238, 99} ,    WindowWidgetType::Groupbox, WindowColour::Secondary, STR_DATE_SET  ), // Date group
    MakeSpinnerWidgets({120,  61}, CHEAT_SPINNER, WindowWidgetType::Spinner,  WindowColour::Secondary                ), // Year box
    MakeSpinnerWidgets({120,  82}, CHEAT_SPINNER, WindowWidgetType::Spinner,  WindowColour::Secondary                ), // Month box
    MakeSpinnerWidgets({120, 103}, CHEAT_SPINNER, WindowWidgetType::Spinner,  WindowColour::Secondary                ), // Day box
    MakeWidget        ({ 11, 122}, CHEAT_BUTTON,  WindowWidgetType::Button,   WindowColour::Secondary, STR_DATE_SET  ), // Set Date
    MakeWidget        ({127, 122}, CHEAT_BUTTON,  WindowWidgetType::Button,   WindowColour::Secondary, STR_DATE_RESET), // Reset Date
};

static constexpr Widget window_cheats_guests_widgets[] =
{
    MAIN_CHEATS_WIDGETS,
    MakeWidget({ 11,  48}, CHEAT_BUTTON,  WindowWidgetType::Button,   WindowColour::Secondary, STR_CHEAT_LARGE_TRAM_GUESTS,     STR_CHEAT_LARGE_TRAM_GUESTS_TIP), // large tram
    MakeWidget({127,  48}, CHEAT_BUTTON,  WindowWidgetType::Button,   WindowColour::Secondary, STR_CHEAT_REMOVE_ALL_GUESTS,     STR_CHEAT_REMOVE_ALL_GUESTS_TIP), // remove all guests

    MakeWidget({  5,  70}, {238, 206},    WindowWidgetType::Groupbox, WindowColour::Secondary, STR_CHEAT_SET_GUESTS_PARAMETERS                                 ), // Guests parameters group frame
    MakeWidget({183,  84}, MINMAX_BUTTON, WindowWidgetType::Button,   WindowColour::Secondary, STR_MAX                                                         ), // happiness max
    MakeWidget({127,  84}, MINMAX_BUTTON, WindowWidgetType::Button,   WindowColour::Secondary, STR_MIN                                                         ), // happiness min
    MakeWidget({183, 105}, MINMAX_BUTTON, WindowWidgetType::Button,   WindowColour::Secondary, STR_MAX                                                         ), // energy max
    MakeWidget({127, 105}, MINMAX_BUTTON, WindowWidgetType::Button,   WindowColour::Secondary, STR_MIN                                                         ), // energy min
    MakeWidget({183, 126}, MINMAX_BUTTON, WindowWidgetType::Button,   WindowColour::Secondary, STR_MAX                                                         ), // hunger max
    MakeWidget({127, 126}, MINMAX_BUTTON, WindowWidgetType::Button,   WindowColour::Secondary, STR_MIN                                                         ), // hunger min
    MakeWidget({183, 147}, MINMAX_BUTTON, WindowWidgetType::Button,   WindowColour::Secondary, STR_MAX                                                         ), // thirst max
    MakeWidget({127, 147}, MINMAX_BUTTON, WindowWidgetType::Button,   WindowColour::Secondary, STR_MIN                                                         ), // thirst min
    MakeWidget({183, 168}, MINMAX_BUTTON, WindowWidgetType::Button,   WindowColour::Secondary, STR_MAX                                                         ), // nausea max
    MakeWidget({127, 168}, MINMAX_BUTTON, WindowWidgetType::Button,   WindowColour::Secondary, STR_MIN                                                         ), // nausea min
    MakeWidget({183, 189}, MINMAX_BUTTON, WindowWidgetType::Button,   WindowColour::Secondary, STR_MAX                                                         ), // nausea tolerance max
    MakeWidget({127, 189}, MINMAX_BUTTON, WindowWidgetType::Button,   WindowColour::Secondary, STR_MIN                                                         ), // nausea tolerance min
    MakeWidget({183, 210}, MINMAX_BUTTON, WindowWidgetType::Button,   WindowColour::Secondary, STR_MAX                                                         ), // toilet max
    MakeWidget({127, 210}, MINMAX_BUTTON, WindowWidgetType::Button,   WindowColour::Secondary, STR_MIN                                                         ), // toilet min
    MakeWidget({127, 251}, CHEAT_BUTTON,  WindowWidgetType::Button,   WindowColour::Secondary, STR_CHEAT_MORE_THAN_1                                           ), // ride intensity > 1
    MakeWidget({ 11, 251}, CHEAT_BUTTON,  WindowWidgetType::Button,   WindowColour::Secondary, STR_CHEAT_LESS_THAN_15                                          ), // ride intensity < 15

    MakeWidget({  5, 258+15+6+2}, {238, 62},    WindowWidgetType::Groupbox, WindowColour::Secondary, STR_CHEAT_GIVE_ALL_GUESTS                                       ), // Guests inventory group frame
    MakeWidget({ 11, 279+15+6-3}, CHEAT_BUTTON, WindowWidgetType::Button,   WindowColour::Secondary, STR_CURRENCY_FORMAT                                             ), // give guests money
    MakeWidget({127, 279+15+6-3}, CHEAT_BUTTON, WindowWidgetType::Button,   WindowColour::Secondary, STR_SHOP_ITEM_PLURAL_PARK_MAP                                   ), // give guests park maps
    MakeWidget({ 11, 300+15+6-3}, CHEAT_BUTTON, WindowWidgetType::Button,   WindowColour::Secondary, STR_SHOP_ITEM_PLURAL_BALLOON                                    ), // give guests balloons
    MakeWidget({127, 300+15+6-3}, CHEAT_BUTTON, WindowWidgetType::Button,   WindowColour::Secondary, STR_SHOP_ITEM_PLURAL_UMBRELLA                                   ), // give guests umbrellas

    MakeWidget({  5, 342+6}, {238,  85},    WindowWidgetType::Groupbox, WindowColour::Secondary, STR_GUEST_BEHAVIOUR                                             ), // Guests behaviour group frame
    MakeWidget({ 11, 363+1}, CHEAT_CHECK,   WindowWidgetType::Checkbox, WindowColour::Secondary, STR_CHEAT_IGNORE_INTENSITY,      STR_CHEAT_IGNORE_INTENSITY_TIP ), // guests ignore intensity
    MakeWidget({ 11, 380+1}, CHEAT_CHECK,   WindowWidgetType::Checkbox, WindowColour::Secondary, STR_CHEAT_IGNORE_PRICE,          STR_CHEAT_IGNORE_PRICE_TIP     ), // guests ignore price
    MakeWidget({ 11, 397+1}, CHEAT_CHECK,   WindowWidgetType::Checkbox, WindowColour::Secondary, STR_CHEAT_DISABLE_VANDALISM,     STR_CHEAT_DISABLE_VANDALISM_TIP), // disable vandalism
    MakeWidget({ 11, 414+1}, CHEAT_CHECK,   WindowWidgetType::Checkbox, WindowColour::Secondary, STR_CHEAT_DISABLE_LITTERING,     STR_CHEAT_DISABLE_LITTERING_TIP), // disable littering
};

static constexpr Widget window_cheats_staff_widgets[] =
{
    MAIN_CHEATS_WIDGETS,
    MakeWidget        ({  5, 357-309}, {238,  35},   WindowWidgetType::Groupbox,     WindowColour::Secondary, STR_CHEAT_STAFF_GROUP                                           ), // Staff group
    MakeWidget        ({126, 371-309}, {111,  14},   WindowWidgetType::DropdownMenu, WindowColour::Secondary                                                                  ), // Staff speed
    MakeWidget        ({225, 372-309}, { 11,  12},   WindowWidgetType::Button,       WindowColour::Secondary, STR_DROPDOWN_GLYPH                                              ), // Staff speed

    MakeWidget        ({  5, 257-168}, {238,  99},   WindowWidgetType::Groupbox, WindowColour::Secondary, STR_CHEAT_MAINTENANCE_GROUP                                         ), // Maintenance group
    MakeWidget        ({ 11, 271-168}, CHEAT_BUTTON, WindowWidgetType::Button,   WindowColour::Secondary, STR_CHEAT_REMOVE_LITTER                                             ), // Remove litter
    MakeWidget        ({127, 271-168}, CHEAT_BUTTON, WindowWidgetType::Button,   WindowColour::Secondary, STR_CHEAT_FIX_VANDALISM                                             ), // Fix vandalism
    MakeWidget        ({ 11, 292-168}, CHEAT_BUTTON, WindowWidgetType::Button,   WindowColour::Secondary, STR_CHEAT_CLEAR_GRASS                                               ), // Clear grass
    MakeWidget        ({127, 292-168}, CHEAT_BUTTON, WindowWidgetType::Button,   WindowColour::Secondary, STR_CHEAT_MOWED_GRASS                                               ), // Mowed grass
    MakeWidget        ({ 11, 313-168}, CHEAT_BUTTON, WindowWidgetType::Button,   WindowColour::Secondary, STR_CHEAT_WATER_PLANTS                                              ), // Water plants
    MakeWidget        ({ 11, 334-164}, CHEAT_CHECK,  WindowWidgetType::Checkbox, WindowColour::Secondary, STR_CHEAT_DISABLE_PLANT_AGING,   STR_CHEAT_DISABLE_PLANT_AGING_TIP  ), // Disable plant ageing
};

static constexpr Widget window_cheats_park_widgets[] =
{
    MAIN_CHEATS_WIDGETS,
    MakeWidget        ({  5,  48}, {238,  60},   WindowWidgetType::Groupbox, WindowColour::Secondary, STR_CHEAT_GENERAL_GROUP                                             ), // General group
    MakeWidget        ({ 11,  62}, CHEAT_BUTTON, WindowWidgetType::Button,   WindowColour::Secondary, STR_CHEAT_OWN_ALL_LAND,          STR_CHEAT_OWN_ALL_LAND_TIP         ), // Own all land
    MakeWidget        ({127,  62}, CHEAT_BUTTON, WindowWidgetType::Button,   WindowColour::Secondary, STR_CHEAT_REMOVE_PARK_FENCES,    STR_CHEAT_REMOVE_PARK_FENCES_TIP   ), // Remove park fences
    MakeWidget        ({ 11,  83}, CHEAT_BUTTON, WindowWidgetType::Button,   WindowColour::Secondary, STR_CHEAT_OPEN_PARK,             STR_CHEAT_OPEN_PARK_TIP            ), // open / close park

    MakeWidget        ({  5, 113}, {238,  75},   WindowWidgetType::Groupbox, WindowColour::Secondary, STR_CHEAT_OBJECTIVE_GROUP                                           ), // Objective group
    MakeWidget        ({ 11, 128}, CHEAT_CHECK,  WindowWidgetType::Checkbox, WindowColour::Secondary, STR_CHEAT_NEVERENDING_MARKETING, STR_CHEAT_NEVERENDING_MARKETING_TIP), // never ending marketing campaigns
    MakeWidget        ({ 11, 145}, {281,  12},   WindowWidgetType::Checkbox, WindowColour::Secondary, STR_FORCE_PARK_RATING                                               ), // Force park rating
    MakeSpinnerWidgets({156, 143}, { 81,  14},   WindowWidgetType::Spinner,  WindowColour::Secondary                                                                      ), // park rating (3 widgets)
    MakeWidget        ({ 11, 163}, CHEAT_BUTTON, WindowWidgetType::Button,   WindowColour::Secondary, STR_CHEAT_WIN_SCENARIO                                              ), // Win scenario
    MakeWidget        ({127, 163}, CHEAT_BUTTON, WindowWidgetType::Button,   WindowColour::Secondary, STR_CHEAT_HAVE_FUN                                                  ), // Have fun!

    MakeWidget        ({  5, 192}, {238,  68},   WindowWidgetType::Groupbox, WindowColour::Secondary, STR_CHEAT_GROUP_CONSTRUCTION                                                      ), // Construction group
    MakeWidget        ({ 11, 207}, CHEAT_CHECK,  WindowWidgetType::Checkbox, WindowColour::Secondary, STR_CHEAT_BUILD_IN_PAUSE_MODE,          STR_CHEAT_BUILD_IN_PAUSE_MODE_TIP         ), // Build in pause mode
    MakeWidget        ({ 11, 224}, CHEAT_CHECK,  WindowWidgetType::Checkbox, WindowColour::Secondary, STR_CHEAT_ALLOW_PATH_AS_QUEUE,          STR_CHEAT_ALLOW_PATH_AS_QUEUE_TIP         ), // Allow regular footpaths as queue path
    MakeWidget        ({ 11, 241}, CHEAT_CHECK,  WindowWidgetType::Checkbox, WindowColour::Secondary, STR_CHEAT_ALLOW_SPECIAL_COLOUR_SCHEMES, STR_CHEAT_ALLOW_SPECIAL_COLOUR_SCHEMES_TIP), // Allow special colours in dropdown
};

static constexpr Widget window_cheats_rides_widgets[] =
{
    MAIN_CHEATS_WIDGETS,
    MakeWidget({ 11,  48}, CHEAT_BUTTON, WindowWidgetType::Button,   WindowColour::Secondary, STR_CHEAT_FIX_ALL_RIDES,                        STR_CHEAT_FIX_ALL_RIDES_TIP                    ), // Fix all rides
    MakeWidget({127,  48}, CHEAT_BUTTON, WindowWidgetType::Button,   WindowColour::Secondary, STR_CHEAT_RENEW_RIDES,                          STR_CHEAT_RENEW_RIDES_TIP                      ), // Renew rides
    MakeWidget({127,  69}, CHEAT_BUTTON, WindowWidgetType::Button,   WindowColour::Secondary, STR_CHEAT_RESET_CRASH_STATUS,                   STR_CHEAT_RESET_CRASH_STATUS_TIP               ), // Reset crash status
    MakeWidget({ 11,  69}, CHEAT_BUTTON, WindowWidgetType::Button,   WindowColour::Secondary, STR_CHEAT_10_MINUTE_INSPECTIONS,                STR_CHEAT_10_MINUTE_INSPECTIONS_TIP            ), // 10 minute inspections

    MakeWidget({  5, 95},  {238, 87},    WindowWidgetType::Groupbox, WindowColour::Secondary, STR_CHEAT_GROUP_CONSTRUCTION                                                                   ), // Construction group
    MakeWidget({ 11, 111}, CHEAT_CHECK,  WindowWidgetType::Checkbox, WindowColour::Secondary, STR_CHEAT_ENABLE_ALL_DRAWABLE_TRACK_PIECES,     STR_CHEAT_ENABLE_ALL_DRAWABLE_TRACK_PIECES_TIP ), // Show all drawable track pieces
    MakeWidget({ 11, 128}, CHEAT_CHECK,  WindowWidgetType::Checkbox, WindowColour::Secondary, STR_CHEAT_ENABLE_CHAIN_LIFT_ON_ALL_TRACK,       STR_CHEAT_ENABLE_CHAIN_LIFT_ON_ALL_TRACK_TIP   ), // Enable chain lift on all track
    MakeWidget({ 11, 145}, CHEAT_CHECK,  WindowWidgetType::Checkbox, WindowColour::Secondary, STR_CHEAT_ALLOW_TRACK_PLACE_INVALID_HEIGHTS,    STR_CHEAT_ALLOW_TRACK_PLACE_INVALID_HEIGHTS_TIP), // Allow track place at invalid heights
    MakeWidget({ 11, 162}, CHEAT_CHECK,  WindowWidgetType::Checkbox, WindowColour::Secondary, STR_CHEAT_MAKE_DESTRUCTABLE,                    STR_CHEAT_MAKE_DESTRUCTABLE_TIP                ), // All destructible

    MakeWidget({  5, 186}, {238, 102},   WindowWidgetType::Groupbox, WindowColour::Secondary, STR_CHEAT_GROUP_OPERATION                                                                      ), // Operation group
    MakeWidget({ 11, 201}, CHEAT_CHECK,  WindowWidgetType::Checkbox, WindowColour::Secondary, STR_CHEAT_SHOW_ALL_OPERATING_MODES                                                             ), // Show all operating modes
    MakeWidget({ 11, 218}, CHEAT_CHECK,  WindowWidgetType::Checkbox, WindowColour::Secondary, STR_CHEAT_UNLOCK_OPERATING_LIMITS,              STR_CHEAT_UNLOCK_OPERATING_LIMITS_TIP          ), // 410 km/h lift hill etc.
    MakeWidget({ 11, 235}, CHEAT_CHECK,  WindowWidgetType::Checkbox, WindowColour::Secondary, STR_CHEAT_DISABLE_BRAKES_FAILURE,               STR_CHEAT_DISABLE_BRAKES_FAILURE_TIP           ), // Disable brakes failure
    MakeWidget({ 11, 252}, CHEAT_CHECK,  WindowWidgetType::Checkbox, WindowColour::Secondary, STR_CHEAT_DISABLE_BREAKDOWNS,                   STR_CHEAT_DISABLE_BREAKDOWNS_TIP               ), // Disable all breakdowns
    MakeWidget({ 11, 269}, CHEAT_CHECK,  WindowWidgetType::Checkbox, WindowColour::Secondary, STR_CHEAT_DISABLE_RIDE_VALUE_AGING,             STR_CHEAT_DISABLE_RIDE_VALUE_AGING_TIP         ), // Disable ride ageing

    MakeWidget({  5, 292}, {238, 86},    WindowWidgetType::Groupbox, WindowColour::Secondary, STR_CHEAT_GROUP_AVAILABILITY                                                                   ), // Availability group
    MakeWidget({ 11, 308}, CHEAT_CHECK,  WindowWidgetType::Checkbox, WindowColour::Secondary, STR_CHEAT_ALLOW_ARBITRARY_RIDE_TYPE_CHANGES,    STR_CHEAT_ALLOW_ARBITRARY_RIDE_TYPE_CHANGES_TIP), // Allow arbitrary ride type changes
    MakeWidget({ 11, 325}, CHEAT_CHECK,  WindowWidgetType::Checkbox, WindowColour::Secondary, STR_CHEAT_SHOW_VEHICLES_FROM_OTHER_TRACK_TYPES                                                 ), // Show vehicles from other track types
    MakeWidget({ 11, 342}, CHEAT_CHECK,  WindowWidgetType::Checkbox, WindowColour::Secondary, STR_CHEAT_DISABLE_TRAIN_LENGTH_LIMIT,           STR_CHEAT_DISABLE_TRAIN_LENGTH_LIMIT_TIP       ), // Disable train length limits
    MakeWidget({ 11, 359}, CHEAT_CHECK,  WindowWidgetType::Checkbox, WindowColour::Secondary, STR_CHEAT_IGNORE_RESEARCH_STATUS,               STR_CHEAT_IGNORE_RESEARCH_STATUS_TIP           ), // Ignore Research Status
};

static constexpr Widget window_cheats_weather_widgets[] =
{
    MAIN_CHEATS_WIDGETS,
    MakeWidget        ({  5,  48}, {238,  50},   WindowWidgetType::Groupbox,     WindowColour::Secondary, STR_CHEAT_WEATHER_GROUP                                      ), // Weather group
    MakeWidget        ({126,  62}, {111,  14},   WindowWidgetType::DropdownMenu, WindowColour::Secondary, kStringIdNone,                        STR_CHANGE_WEATHER_TOOLTIP  ), // Force weather
    MakeWidget        ({225,  63}, { 11,  12},   WindowWidgetType::Button,       WindowColour::Secondary, STR_DROPDOWN_GLYPH,              STR_CHANGE_WEATHER_TOOLTIP  ), // Force weather
    MakeWidget        ({ 11,  80}, CHEAT_CHECK,  WindowWidgetType::Checkbox,     WindowColour::Secondary, STR_CHEAT_FREEZE_WEATHER,        STR_CHEAT_FREEZE_WEATHER_TIP), // Freeze weather
    MakeWidget        ({  5, 102}, {238,  37},   WindowWidgetType::Groupbox,     WindowColour::Secondary, STR_FAUNA                                                    ), // Fauna group
    MakeWidget        ({ 11, 115}, CHEAT_BUTTON, WindowWidgetType::Button,       WindowColour::Secondary, STR_CREATE_DUCKS,                STR_CREATE_DUCKS_TIP        ), // Create ducks
    MakeWidget        ({127, 115}, CHEAT_BUTTON, WindowWidgetType::Button,       WindowColour::Secondary, STR_REMOVE_DUCKS,                STR_REMOVE_DUCKS_TIP        ), // Remove ducks
};

static constexpr std::span<const Widget> window_cheats_page_widgets[] =
{
    window_cheats_money_widgets,
    window_cheats_date_widgets,
    window_cheats_guests_widgets,
    window_cheats_staff_widgets,
    window_cheats_park_widgets,
    window_cheats_rides_widgets,
    window_cheats_weather_widgets,
};

static uint64_t window_cheats_page_hold_down_widgets[] = {
    (1uLL << WIDX_MONEY_SPINNER_INCREMENT) |
    (1uLL << WIDX_MONEY_SPINNER_DECREMENT) |
    (1uLL << WIDX_ADD_MONEY),

    (1uLL << WIDX_YEAR_UP) |
    (1uLL << WIDX_YEAR_DOWN) |
    (1uLL << WIDX_MONTH_UP) |
    (1uLL << WIDX_MONTH_DOWN) |
    (1uLL << WIDX_DAY_UP) |
    (1uLL << WIDX_DAY_DOWN),

    0,

    0,

    (1uLL << WIDX_INCREASE_PARK_RATING) |
    (1uLL << WIDX_DECREASE_PARK_RATING),

    0,

    0,
};

static StringId window_cheats_page_titles[] = {
    STR_CHEAT_TITLE_FINANCIAL,
    STR_CHEAT_TITLE_DATE,
    STR_CHEAT_TITLE_GUEST,
    STR_CHEAT_TITLE_STAFF,
    STR_CHEAT_TITLE_PARK,
    STR_CHEAT_TITLE_RIDE,
    STR_CHEAT_TITLE_WEATHER,
};
    // clang-format on

    class CheatsWindow final : public Window
    {
    private:
        char _moneySpinnerText[kMoneyStringMaxlength]{};
        money64 _moneySpinnerValue = CHEATS_MONEY_DEFAULT;
        int32_t _parkRatingSpinnerValue{};
        int32_t _yearSpinnerValue = 1;
        int32_t _monthSpinnerValue = 1;
        int32_t _daySpinnerValue = 1;

    public:
        void OnOpen() override
        {
            SetPage(WINDOW_CHEATS_PAGE_MONEY);
            _parkRatingSpinnerValue = Park::GetForcedRating() >= 0 ? Park::GetForcedRating() : 999;
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
                case WINDOW_CHEATS_PAGE_DATE:
                    OnMouseDownDate(widgetIndex);
                    break;
                case WINDOW_CHEATS_PAGE_PARK:
                    OnMouseDownPark(widgetIndex);
                    break;
                case WINDOW_CHEATS_PAGE_STAFF:
                    OnMouseDownStaff(widgetIndex);
                    break;
                case WINDOW_CHEATS_PAGE_WEATHER:
                    OnMouseDownWeather(widgetIndex);
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
                case WIDX_TAB_5:
                case WIDX_TAB_6:
                case WIDX_TAB_7:
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
                        case WINDOW_CHEATS_PAGE_PARK:
                            OnMouseUpPark(widgetIndex);
                            break;
                        case WINDOW_CHEATS_PAGE_RIDES:
                            OnMouseUpRides(widgetIndex);
                            break;
                        case WINDOW_CHEATS_PAGE_STAFF:
                            OnMouseUpStaff(widgetIndex);
                            break;
                        case WINDOW_CHEATS_PAGE_WEATHER:
                            OnMouseUpWeather(widgetIndex);
                            break;
                    }
                    break;
            }
        }

        void OnDropdown(WidgetIndex widgetIndex, int32_t selectedIndex) override
        {
            if (page == WINDOW_CHEATS_PAGE_STAFF)
            {
                OnDropdownStaff(widgetIndex, selectedIndex);
            }
            else if (page == WINDOW_CHEATS_PAGE_WEATHER)
            {
                OnDropdownWeather(widgetIndex, selectedIndex);
            }
        }

        void OnPrepareDraw() override
        {
            pressed_widgets = 0;
            disabled_widgets = 0;

            // Set correct active tab
            for (auto i = 0; i < WINDOW_CHEATS_PAGE_COUNT; i++)
                SetWidgetPressed(WIDX_TAB_1 + i, false);
            SetWidgetPressed(WIDX_TAB_1 + page, true);

            // Set title
            widgets[WIDX_TITLE].text = window_cheats_page_titles[page];

            auto& gameState = GetGameState();
            switch (page)
            {
                case WINDOW_CHEATS_PAGE_MONEY:
                {
                    auto moneyDisabled = (gameState.Park.Flags & PARK_FLAGS_NO_MONEY) != 0;
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
                    SetCheckboxValue(WIDX_GUEST_IGNORE_RIDE_INTENSITY, gameState.Cheats.ignoreRideIntensity);
                    SetCheckboxValue(WIDX_GUEST_IGNORE_PRICE, gameState.Cheats.ignorePrice);
                    SetCheckboxValue(WIDX_DISABLE_VANDALISM, gameState.Cheats.disableVandalism);
                    SetCheckboxValue(WIDX_DISABLE_LITTERING, gameState.Cheats.disableLittering);
                    break;
                }
                case WINDOW_CHEATS_PAGE_PARK:
                    widgets[WIDX_OPEN_CLOSE_PARK].text = STR_CHEAT_OPEN_PARK;
                    if (gameState.Park.Flags & PARK_FLAGS_PARK_OPEN)
                        widgets[WIDX_OPEN_CLOSE_PARK].text = STR_CHEAT_CLOSE_PARK;

                    SetCheckboxValue(WIDX_FORCE_PARK_RATING, Park::GetForcedRating() >= 0);
                    SetCheckboxValue(WIDX_NEVERENDING_MARKETING, gameState.Cheats.neverendingMarketing);
                    SetCheckboxValue(WIDX_ALLOW_BUILD_IN_PAUSE_MODE, gameState.Cheats.buildInPauseMode);
                    SetCheckboxValue(WIDX_ALLOW_REGULAR_PATH_AS_QUEUE, gameState.Cheats.allowRegularPathAsQueue);
                    SetCheckboxValue(WIDX_ALLOW_SPECIAL_COLOUR_SCHEMES, gameState.Cheats.allowSpecialColourSchemes);
                    break;
                case WINDOW_CHEATS_PAGE_RIDES:
                    SetCheckboxValue(WIDX_UNLOCK_OPERATING_LIMITS, gameState.Cheats.unlockOperatingLimits);
                    SetCheckboxValue(WIDX_DISABLE_BRAKES_FAILURE, gameState.Cheats.disableBrakesFailure);
                    SetCheckboxValue(WIDX_DISABLE_ALL_BREAKDOWNS, gameState.Cheats.disableAllBreakdowns);
                    SetCheckboxValue(WIDX_SHOW_ALL_OPERATING_MODES, gameState.Cheats.showAllOperatingModes);
                    SetCheckboxValue(
                        WIDX_SHOW_VEHICLES_FROM_OTHER_TRACK_TYPES, gameState.Cheats.showVehiclesFromOtherTrackTypes);
                    SetCheckboxValue(WIDX_DISABLE_TRAIN_LENGTH_LIMITS, gameState.Cheats.disableTrainLengthLimit);
                    SetCheckboxValue(WIDX_ENABLE_CHAIN_LIFT_ON_ALL_TRACK, gameState.Cheats.enableChainLiftOnAllTrack);
                    SetCheckboxValue(WIDX_ENABLE_ARBITRARY_RIDE_TYPE_CHANGES, gameState.Cheats.allowArbitraryRideTypeChanges);
                    SetCheckboxValue(WIDX_DISABLE_RIDE_VALUE_AGING, gameState.Cheats.disableRideValueAging);
                    SetCheckboxValue(WIDX_IGNORE_RESEARCH_STATUS, gameState.Cheats.ignoreResearchStatus);
                    SetCheckboxValue(WIDX_ENABLE_ALL_DRAWABLE_TRACK_PIECES, gameState.Cheats.enableAllDrawableTrackPieces);
                    SetCheckboxValue(WIDX_ALLOW_TRACK_PLACE_INVALID_HEIGHTS, gameState.Cheats.allowTrackPlaceInvalidHeights);
                    SetCheckboxValue(WIDX_MAKE_DESTRUCTIBLE, gameState.Cheats.makeAllDestructible);
                    break;
                case WINDOW_CHEATS_PAGE_STAFF:
                    SetCheckboxValue(WIDX_DISABLE_PLANT_AGING, gameState.Cheats.disablePlantAging);
                    break;
                case WINDOW_CHEATS_PAGE_WEATHER:
                    SetCheckboxValue(WIDX_FREEZE_WEATHER, gameState.Cheats.freezeWeather);
                    break;
            }

            // Current weather
            if (page == WINDOW_CHEATS_PAGE_WEATHER)
            {
                widgets[WIDX_WEATHER].text = WeatherTypes[EnumValue(gameState.ClimateCurrent.Weather)];
            }

            // Staff speed
            if (page == WINDOW_CHEATS_PAGE_STAFF)
            {
                widgets[WIDX_STAFF_SPEED].text = _staffSpeedNames[EnumValue(gameState.Cheats.selectedStaffSpeed)];
            }

            if (gScreenFlags & SCREEN_FLAGS_EDITOR)
            {
                SetWidgetDisabled(WIDX_TAB_2, true);
                SetWidgetDisabled(WIDX_TAB_3, true);
                SetWidgetDisabled(WIDX_NO_MONEY, true);
            }
        }

        void OnDraw(DrawPixelInfo& dpi) override
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
                    colour.setFlag(ColourFlag::inset, true);
                }
                DrawTextBasic(dpi, windowPos + ScreenCoordsXY{ _xLcol, 93 }, STR_BOTTOM_TOOLBAR_CASH, ft, { colour });
            }
            else if (page == WINDOW_CHEATS_PAGE_DATE)
            {
                auto& yearBox = widgets[WIDX_YEAR_BOX];
                DrawTextBasic(dpi, windowPos + ScreenCoordsXY{ _xLcol, yearBox.top + 2 }, STR_YEAR);

                auto& monthBox = widgets[WIDX_MONTH_BOX];
                DrawTextBasic(dpi, windowPos + ScreenCoordsXY{ _xLcol, monthBox.top + 2 }, STR_MONTH);

                auto& dayBox = widgets[WIDX_DAY_BOX];
                DrawTextBasic(dpi, windowPos + ScreenCoordsXY{ _xLcol, dayBox.top + 2 }, STR_DAY);

                auto ft = Formatter();
                ft.Add<int32_t>(_yearSpinnerValue);
                DrawTextBasic(
                    dpi, windowPos + ScreenCoordsXY{ _xRcol, yearBox.top + 2 }, STR_FORMAT_INTEGER, ft,
                    { colours[1], TextAlignment::RIGHT });

                ft = Formatter();
                int32_t actual_month = _monthSpinnerValue - 1;
                ft.Add<int32_t>(actual_month);
                DrawTextBasic(
                    dpi, windowPos + ScreenCoordsXY{ _xRcol, monthBox.top + 2 }, STR_FORMAT_MONTH, ft,
                    { colours[1], TextAlignment::RIGHT });

                ft = Formatter();
                ft.Add<int32_t>(_daySpinnerValue);
                DrawTextBasic(
                    dpi, windowPos + ScreenCoordsXY{ _xRcol, dayBox.top + 2 }, STR_FORMAT_INTEGER, ft,
                    { colours[1], TextAlignment::RIGHT });
            }
            else if (page == WINDOW_CHEATS_PAGE_PARK)
            {
                {
                    auto ft = Formatter();
                    ft.Add<int32_t>(_parkRatingSpinnerValue);

                    auto& widget = widgets[WIDX_PARK_RATING_SPINNER];
                    DrawTextBasic(
                        dpi, windowPos + ScreenCoordsXY{ widget.left + 1, widget.top + 2 }, STR_FORMAT_INTEGER, ft,
                        { colours[1] });
                }
            }
            else if (page == WINDOW_CHEATS_PAGE_STAFF)
            {
                {
                    auto& widget = widgets[WIDX_STAFF_SPEED];
                    DrawTextBasic(dpi, windowPos + ScreenCoordsXY{ _xLcol - 3, widget.top + 1 }, STR_CHEAT_STAFF_SPEED);
                }
            }
            else if (page == WINDOW_CHEATS_PAGE_GUESTS)
            {
                {
                    auto& widget = widgets[WIDX_GUEST_HAPPINESS_MIN];
                    DrawTextBasic(dpi, windowPos + ScreenCoordsXY{ _xLcol - 3, widget.top + 2 }, STR_CHEAT_GUEST_HAPPINESS);
                }
                {
                    auto& widget = widgets[WIDX_GUEST_ENERGY_MIN];
                    DrawTextBasic(dpi, windowPos + ScreenCoordsXY{ _xLcol - 3, widget.top + 2 }, STR_CHEAT_GUEST_ENERGY);
                }
                {
                    auto& widget = widgets[WIDX_GUEST_HUNGER_MIN];
                    DrawTextBasic(dpi, windowPos + ScreenCoordsXY{ _xLcol - 3, widget.top + 2 }, STR_CHEAT_GUEST_HUNGER);
                }
                {
                    auto& widget = widgets[WIDX_GUEST_THIRST_MIN];
                    DrawTextBasic(dpi, windowPos + ScreenCoordsXY{ _xLcol - 3, widget.top + 2 }, STR_CHEAT_GUEST_THIRST);
                }
                {
                    auto& widget = widgets[WIDX_GUEST_NAUSEA_MIN];
                    DrawTextBasic(dpi, windowPos + ScreenCoordsXY{ _xLcol - 3, widget.top + 2 }, STR_CHEAT_GUEST_NAUSEA);
                }
                {
                    auto& widget = widgets[WIDX_GUEST_NAUSEA_TOLERANCE_MIN];
                    DrawTextBasic(
                        dpi, windowPos + ScreenCoordsXY{ _xLcol - 3, widget.top + 2 }, STR_CHEAT_GUEST_NAUSEA_TOLERANCE);
                }
                {
                    auto& widget = widgets[WIDX_GUEST_TOILET_MIN];
                    DrawTextBasic(dpi, windowPos + ScreenCoordsXY{ _xLcol - 3, widget.top + 2 }, STR_CHEAT_GUEST_TOILET);
                }
                {
                    auto& widget = widgets[WIDX_GUEST_RIDE_INTENSITY_LESS_THAN_15];
                    DrawTextBasic(
                        dpi, windowPos + ScreenCoordsXY{ _xLcol - 3, widget.top - 17 }, STR_CHEAT_GUEST_PREFERRED_INTENSITY);
                }
            }
            else if (page == WINDOW_CHEATS_PAGE_WEATHER)
            {
                {
                    auto& widget = widgets[WIDX_WEATHER];
                    DrawTextBasic(dpi, windowPos + ScreenCoordsXY{ _xLcol - 3, widget.top + 1 }, STR_CHANGE_WEATHER);
                }
            }
        }

        void OnTextInput(WidgetIndex widgetIndex, std::string_view text) override
        {
            if (page == WINDOW_CHEATS_PAGE_MONEY && widgetIndex == WIDX_MONEY_SPINNER)
            {
                auto val = StringToMoney(std::string(text).c_str());
                if (val != kMoney64Undefined)
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
            SetWidgets(window_cheats_page_widgets[p]);

            auto maxY = 0;
            for (WidgetIndex widgetIdx = WIDX_TAB_CONTENT; widgetIdx < widgets.size(); widgetIdx++)
            {
                auto& widget = widgets[widgetIdx];
                maxY = std::max<int32_t>(maxY, widget.bottom);
            }
            maxY += 6;

            Invalidate();
            WindowInitScrollWidgets(*this);
            height = maxY;
            widgets[WIDX_BACKGROUND].bottom = maxY - 1;
            widgets[WIDX_PAGE_BACKGROUND].bottom = maxY - 1;
            Invalidate();
        }

        void UpdateTabPositions()
        {
            constexpr uint16_t tabs[] = {
                WIDX_TAB_1, WIDX_TAB_2, WIDX_TAB_3, WIDX_TAB_4, WIDX_TAB_5, WIDX_TAB_6,
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

        void DrawTabImages(DrawPixelInfo& dpi)
        {
            // Money tab
            if (!IsWidgetDisabled(WIDX_TAB_1))
            {
                uint32_t sprite_idx = SPR_TAB_FINANCES_SUMMARY_0;
                if (page == WINDOW_CHEATS_PAGE_MONEY)
                    sprite_idx += (frame_no / 2) % 8;
                GfxDrawSprite(
                    dpi, ImageId(sprite_idx), windowPos + ScreenCoordsXY{ widgets[WIDX_TAB_1].left, widgets[WIDX_TAB_1].top });
            }

            // Date tab
            if (!IsWidgetDisabled(WIDX_TAB_2))
            {
                uint32_t sprite_idx = SPR_TAB_TIMER_0;
                if (page == WINDOW_CHEATS_PAGE_DATE)
                    sprite_idx += (frame_no / 8) % 8;
                GfxDrawSprite(
                    dpi, ImageId(sprite_idx), windowPos + ScreenCoordsXY{ widgets[WIDX_TAB_2].left, widgets[WIDX_TAB_2].top });
            }

            // Guests tab
            if (!IsWidgetDisabled(WIDX_TAB_3))
            {
                uint32_t sprite_idx = SPR_TAB_GUESTS_0;
                if (page == WINDOW_CHEATS_PAGE_GUESTS)
                    sprite_idx += (frame_no / 3) % 8;
                GfxDrawSprite(
                    dpi, ImageId(sprite_idx), windowPos + ScreenCoordsXY{ widgets[WIDX_TAB_3].left, widgets[WIDX_TAB_3].top });
            }

            // Staff tab
            if (!IsWidgetDisabled(WIDX_TAB_4))
            {
                uint32_t sprite_idx = SPR_MECHANIC;
                GfxDrawSprite(
                    dpi, ImageId(sprite_idx),
                    windowPos + ScreenCoordsXY{ widgets[WIDX_TAB_4].left + 2, widgets[WIDX_TAB_4].top + 1 });
            }

            // Park tab
            if (!IsWidgetDisabled(WIDX_TAB_5))
            {
                GfxDrawSprite(
                    dpi, ImageId(SPR_TAB_PARK),
                    windowPos + ScreenCoordsXY{ widgets[WIDX_TAB_5].left, widgets[WIDX_TAB_5].top });
            }

            // Rides tab
            if (!IsWidgetDisabled(WIDX_TAB_6))
            {
                uint32_t sprite_idx = SPR_TAB_RIDE_0;
                if (page == WINDOW_CHEATS_PAGE_RIDES)
                    sprite_idx += (frame_no / 4) % 16;
                GfxDrawSprite(
                    dpi, ImageId(sprite_idx), windowPos + ScreenCoordsXY{ widgets[WIDX_TAB_6].left, widgets[WIDX_TAB_6].top });
            }

            // Nature/weather tab
            if (!IsWidgetDisabled(WIDX_TAB_7))
            {
                uint32_t sprite_idx = SPR_WEATHER_SUN_CLOUD;
                GfxDrawSprite(
                    dpi, ImageId(sprite_idx),
                    windowPos + ScreenCoordsXY{ widgets[WIDX_TAB_7].left + 2, widgets[WIDX_TAB_7].top + 4 });
            }
        }

        void OnMouseDownMoney(WidgetIndex widgetIndex)
        {
            switch (widgetIndex)
            {
                case WIDX_MONEY_SPINNER_INCREMENT:
                    _moneySpinnerValue = AddClamp<money64>(
                        CHEATS_MONEY_INCREMENT_DIV * (_moneySpinnerValue / CHEATS_MONEY_INCREMENT_DIV),
                        CHEATS_MONEY_INCREMENT_DIV);
                    InvalidateWidget(WIDX_MONEY_SPINNER);
                    break;
                case WIDX_MONEY_SPINNER_DECREMENT:
                    _moneySpinnerValue = AddClamp<money64>(
                        CHEATS_MONEY_INCREMENT_DIV * (_moneySpinnerValue / CHEATS_MONEY_INCREMENT_DIV),
                        -CHEATS_MONEY_INCREMENT_DIV);
                    InvalidateWidget(WIDX_MONEY_SPINNER);
                    break;
                case WIDX_ADD_MONEY:
                    CheatsSet(CheatType::AddMoney, _moneySpinnerValue);
                    break;
            }
        }

        void OnMouseDownDate(WidgetIndex widgetIndex)
        {
            auto* windowMgr = Ui::GetWindowManager();

            switch (widgetIndex)
            {
                case WIDX_YEAR_UP:
                    _yearSpinnerValue++;
                    _yearSpinnerValue = std::clamp(_yearSpinnerValue, 1, kMaxYear);
                    InvalidateWidget(WIDX_YEAR_BOX);
                    break;
                case WIDX_YEAR_DOWN:
                    _yearSpinnerValue--;
                    _yearSpinnerValue = std::clamp(_yearSpinnerValue, 1, kMaxYear);
                    InvalidateWidget(WIDX_YEAR_BOX);
                    break;
                case WIDX_MONTH_UP:
                    _monthSpinnerValue++;
                    _monthSpinnerValue = std::clamp(_monthSpinnerValue, 1, static_cast<int32_t>(MONTH_COUNT));
                    _daySpinnerValue = std::clamp(
                        _daySpinnerValue, 1, static_cast<int32_t>(Date::GetDaysInMonth(_monthSpinnerValue - 1)));
                    InvalidateWidget(WIDX_MONTH_BOX);
                    InvalidateWidget(WIDX_DAY_BOX);
                    break;
                case WIDX_MONTH_DOWN:
                    _monthSpinnerValue--;
                    _monthSpinnerValue = std::clamp(_monthSpinnerValue, 1, static_cast<int32_t>(MONTH_COUNT));
                    _daySpinnerValue = std::clamp(
                        _daySpinnerValue, 1, static_cast<int32_t>(Date::GetDaysInMonth(_monthSpinnerValue - 1)));
                    InvalidateWidget(WIDX_MONTH_BOX);
                    InvalidateWidget(WIDX_DAY_BOX);
                    break;
                case WIDX_DAY_UP:
                    _daySpinnerValue++;
                    _daySpinnerValue = std::clamp(
                        _daySpinnerValue, 1, static_cast<int32_t>(Date::GetDaysInMonth(_monthSpinnerValue - 1)));
                    InvalidateWidget(WIDX_DAY_BOX);
                    break;
                case WIDX_DAY_DOWN:
                    _daySpinnerValue--;
                    _daySpinnerValue = std::clamp(
                        _daySpinnerValue, 1, static_cast<int32_t>(Date::GetDaysInMonth(_monthSpinnerValue - 1)));
                    InvalidateWidget(WIDX_DAY_BOX);
                    break;
                case WIDX_DATE_SET:
                {
                    auto setDateAction = ParkSetDateAction(_yearSpinnerValue - 1, _monthSpinnerValue - 1, _daySpinnerValue - 1);
                    GameActions::Execute(&setDateAction);
                    windowMgr->InvalidateByClass(WindowClass::BottomToolbar);
                    break;
                }
                case WIDX_DATE_RESET:
                {
                    auto setDateAction = ParkSetDateAction(0, 0, 0);
                    GameActions::Execute(&setDateAction);
                    windowMgr->InvalidateByClass(WindowClass::BottomToolbar);
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
                    CheatsSet(CheatType::NoMoney, GetGameState().Park.Flags & PARK_FLAGS_NO_MONEY ? 0 : 1);
                    break;
                case WIDX_MONEY_SPINNER:
                    MoneyToString(_moneySpinnerValue, _moneySpinnerText, kMoneyStringMaxlength, false);
                    WindowTextInputRawOpen(
                        this, WIDX_MONEY_SPINNER, STR_ENTER_NEW_VALUE, STR_ENTER_NEW_VALUE, {}, _moneySpinnerText,
                        kMoneyStringMaxlength);
                    break;
                case WIDX_SET_MONEY:
                    CheatsSet(CheatType::SetMoney, _moneySpinnerValue);
                    break;
                case WIDX_CLEAR_LOAN:
                    CheatsSet(CheatType::ClearLoan);
                    break;
            }
        }

        void OnMouseDownPark(WidgetIndex widgetIndex)
        {
            switch (widgetIndex)
            {
                case WIDX_INCREASE_PARK_RATING:
                    _parkRatingSpinnerValue = std::min(999, 10 * (_parkRatingSpinnerValue / 10 + 1));
                    InvalidateWidget(WIDX_PARK_RATING_SPINNER);
                    if (Park::GetForcedRating() >= 0)
                    {
                        auto cheatSetAction = CheatSetAction(CheatType::SetForcedParkRating, _parkRatingSpinnerValue);
                        GameActions::Execute(&cheatSetAction);
                    }
                    break;
                case WIDX_DECREASE_PARK_RATING:
                    _parkRatingSpinnerValue = std::max(0, 10 * (_parkRatingSpinnerValue / 10 - 1));
                    InvalidateWidget(WIDX_PARK_RATING_SPINNER);
                    if (Park::GetForcedRating() >= 0)
                    {
                        CheatsSet(CheatType::SetForcedParkRating, _parkRatingSpinnerValue);
                    }
                    break;
            }
        }

        void OnMouseDownStaff(WidgetIndex widgetIndex)
        {
            auto* widget = &widgets[widgetIndex];
            auto& gameState = GetGameState();
            switch (widgetIndex)
            {
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
                    Dropdown::SetChecked(EnumValue(gameState.Cheats.selectedStaffSpeed), true);
                }
            }
        }

        void OnMouseDownWeather(WidgetIndex widgetIndex)
        {
            auto* widget = &widgets[widgetIndex];
            auto& gameState = GetGameState();
            switch (widgetIndex)
            {
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

                    auto currentWeather = gameState.ClimateCurrent.Weather;
                    Dropdown::SetChecked(EnumValue(currentWeather), true);

                    break;
                }
            }
        }

        void OnMouseUpPark(WidgetIndex widgetIndex)
        {
            auto& gameState = GetGameState();
            switch (widgetIndex)
            {
                case WIDX_OWN_ALL_LAND:
                    CheatsSet(CheatType::OwnAllLand);
                    break;
                case WIDX_REMOVE_PARK_FENCES:
                    CheatsSet(CheatType::RemoveParkFences);
                    break;
                case WIDX_OPEN_CLOSE_PARK:
                    CheatsSet(CheatType::OpenClosePark);
                    break;
                case WIDX_WIN_SCENARIO:
                    CheatsSet(CheatType::WinScenario);
                    break;
                case WIDX_HAVE_FUN:
                    CheatsSet(CheatType::HaveFun);
                    break;
                case WIDX_NEVERENDING_MARKETING:
                    CheatsSet(CheatType::NeverendingMarketing, !gameState.Cheats.neverendingMarketing);
                    break;
                case WIDX_FORCE_PARK_RATING:
                    if (Park::GetForcedRating() >= 0)
                    {
                        CheatsSet(CheatType::SetForcedParkRating, -1);
                    }
                    else
                    {
                        CheatsSet(CheatType::SetForcedParkRating, _parkRatingSpinnerValue);
                    }
                    break;
                case WIDX_ALLOW_BUILD_IN_PAUSE_MODE:
                    CheatsSet(CheatType::BuildInPauseMode, !gameState.Cheats.buildInPauseMode);
                    break;
                case WIDX_ALLOW_REGULAR_PATH_AS_QUEUE:
                    CheatsSet(CheatType::AllowRegularPathAsQueue, !gameState.Cheats.allowRegularPathAsQueue);
                    break;
                case WIDX_ALLOW_SPECIAL_COLOUR_SCHEMES:
                    CheatsSet(CheatType::AllowSpecialColourSchemes, !gameState.Cheats.allowSpecialColourSchemes);
                    break;
            }
        }

        void OnMouseUpStaff(WidgetIndex widgetIndex)
        {
            auto& gameState = GetGameState();
            switch (widgetIndex)
            {
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
                    CheatsSet(CheatType::DisablePlantAging, !gameState.Cheats.disablePlantAging);
                    break;
            }
        }

        void OnMouseUpWeather(WidgetIndex widgetIndex)
        {
            auto& gameState = GetGameState();
            switch (widgetIndex)
            {
                case WIDX_FREEZE_WEATHER:
                    CheatsSet(CheatType::FreezeWeather, !gameState.Cheats.freezeWeather);
                    break;
                case WIDX_CREATE_DUCKS:
                    CheatsSet(CheatType::CreateDucks, kCheatsDuckIncrement);
                    break;
                case WIDX_REMOVE_DUCKS:
                    CheatsSet(CheatType::RemoveDucks);
                    break;
            }
        }

        void OnDropdownStaff(WidgetIndex widgetIndex, int32_t dropdownIndex)
        {
            if (dropdownIndex == -1)
            {
                return;
            }

            if (widgetIndex == WIDX_STAFF_SPEED_DROPDOWN_BUTTON)
            {
                int32_t speed = kCheatsStaffNormalSpeed;
                auto& gameState = GetGameState();
                switch (dropdownIndex)
                {
                    case 0:
                        gameState.Cheats.selectedStaffSpeed = StaffSpeedCheat::None;
                        speed = kCheatsStaffNormalSpeed;
                        break;

                    case 1:
                        gameState.Cheats.selectedStaffSpeed = StaffSpeedCheat::Frozen;
                        speed = kCheatsStaffFreezeSpeed;
                        break;

                    case 2:
                        gameState.Cheats.selectedStaffSpeed = StaffSpeedCheat::Fast;
                        speed = kCheatsStaffFastSpeed;
                }
                CheatsSet(CheatType::SetStaffSpeed, speed);
            }
        }

        void OnDropdownWeather(WidgetIndex widgetIndex, int32_t dropdownIndex)
        {
            if (dropdownIndex == -1)
            {
                return;
            }

            if (widgetIndex == WIDX_WEATHER_DROPDOWN_BUTTON)
            {
                CheatsSet(CheatType::ForceWeather, dropdownIndex);
            }
        }

        void OnMouseUpGuests(WidgetIndex widgetIndex)
        {
            auto& gameState = GetGameState();
            switch (widgetIndex)
            {
                case WIDX_GUEST_HAPPINESS_MAX:
                    CheatsSet(CheatType::SetGuestParameter, GUEST_PARAMETER_HAPPINESS, kPeepMaxHappiness);
                    break;
                case WIDX_GUEST_HAPPINESS_MIN:
                    CheatsSet(CheatType::SetGuestParameter, GUEST_PARAMETER_HAPPINESS, 0);
                    break;
                case WIDX_GUEST_ENERGY_MAX:
                    CheatsSet(CheatType::SetGuestParameter, GUEST_PARAMETER_ENERGY, kPeepMaxEnergy);
                    break;
                case WIDX_GUEST_ENERGY_MIN:
                    CheatsSet(CheatType::SetGuestParameter, GUEST_PARAMETER_ENERGY, kPeepMinEnergy);
                    break;
                case WIDX_GUEST_HUNGER_MAX:
                    CheatsSet(CheatType::SetGuestParameter, GUEST_PARAMETER_HUNGER, 0);
                    break;
                case WIDX_GUEST_HUNGER_MIN:
                    CheatsSet(CheatType::SetGuestParameter, GUEST_PARAMETER_HUNGER, kPeepMaxHunger);
                    break;
                case WIDX_GUEST_THIRST_MAX:
                    CheatsSet(CheatType::SetGuestParameter, GUEST_PARAMETER_THIRST, 0);
                    break;
                case WIDX_GUEST_THIRST_MIN:
                    CheatsSet(CheatType::SetGuestParameter, GUEST_PARAMETER_THIRST, kPeepMaxThirst);
                    break;
                case WIDX_GUEST_NAUSEA_MAX:
                    CheatsSet(CheatType::SetGuestParameter, GUEST_PARAMETER_NAUSEA, kPeepMaxNausea);
                    break;
                case WIDX_GUEST_NAUSEA_MIN:
                    CheatsSet(CheatType::SetGuestParameter, GUEST_PARAMETER_NAUSEA, 0);
                    break;
                case WIDX_GUEST_NAUSEA_TOLERANCE_MAX:
                    CheatsSet(
                        CheatType::SetGuestParameter, GUEST_PARAMETER_NAUSEA_TOLERANCE, EnumValue(PeepNauseaTolerance::High));
                    break;
                case WIDX_GUEST_NAUSEA_TOLERANCE_MIN:
                    CheatsSet(
                        CheatType::SetGuestParameter, GUEST_PARAMETER_NAUSEA_TOLERANCE, EnumValue(PeepNauseaTolerance::None));
                    break;
                case WIDX_GUEST_TOILET_MAX:
                    CheatsSet(CheatType::SetGuestParameter, GUEST_PARAMETER_TOILET, kPeepMaxToilet);
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
                    CheatsSet(CheatType::GenerateGuests, kCheatsTramIncrement);
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
                    CheatsSet(CheatType::IgnoreRideIntensity, !gameState.Cheats.ignoreRideIntensity);
                    break;
                case WIDX_GUEST_IGNORE_PRICE:
                    CheatsSet(CheatType::IgnorePrice, !gameState.Cheats.ignorePrice);
                    break;
                case WIDX_DISABLE_VANDALISM:
                    CheatsSet(CheatType::DisableVandalism, !gameState.Cheats.disableVandalism);
                    break;
                case WIDX_DISABLE_LITTERING:
                    CheatsSet(CheatType::DisableLittering, !gameState.Cheats.disableLittering);
                    break;
            }
        }

        void OnMouseUpRides(WidgetIndex widgetIndex)
        {
            auto& gameState = GetGameState();
            switch (widgetIndex)
            {
                case WIDX_RENEW_RIDES:
                    CheatsSet(CheatType::RenewRides);
                    break;
                case WIDX_MAKE_DESTRUCTIBLE:
                    CheatsSet(CheatType::MakeDestructible, !gameState.Cheats.makeAllDestructible);
                    break;
                case WIDX_FIX_ALL:
                    CheatsSet(CheatType::FixRides);
                    break;
                case WIDX_UNLOCK_OPERATING_LIMITS:
                    CheatsSet(CheatType::FastLiftHill, !gameState.Cheats.unlockOperatingLimits);
                    break;
                case WIDX_DISABLE_BRAKES_FAILURE:
                    CheatsSet(CheatType::DisableBrakesFailure, !gameState.Cheats.disableBrakesFailure);
                    break;
                case WIDX_DISABLE_ALL_BREAKDOWNS:
                    CheatsSet(CheatType::DisableAllBreakdowns, !gameState.Cheats.disableAllBreakdowns);
                    break;
                case WIDX_RESET_CRASH_STATUS:
                    CheatsSet(CheatType::ResetCrashStatus);
                    break;
                case WIDX_10_MINUTE_INSPECTIONS:
                    CheatsSet(CheatType::TenMinuteInspections);
                    break;
                case WIDX_SHOW_ALL_OPERATING_MODES:
                {
                    if (!gameState.Cheats.showAllOperatingModes)
                    {
                        ContextShowError(STR_WARNING_IN_CAPS, STR_THIS_FEATURE_IS_CURRENTLY_UNSTABLE, {});
                    }
                    CheatsSet(CheatType::ShowAllOperatingModes, !gameState.Cheats.showAllOperatingModes);
                }
                break;
                case WIDX_SHOW_VEHICLES_FROM_OTHER_TRACK_TYPES:
                {
                    if (!gameState.Cheats.showVehiclesFromOtherTrackTypes)
                    {
                        ContextShowError(STR_WARNING_IN_CAPS, STR_THIS_FEATURE_IS_CURRENTLY_UNSTABLE, {});
                    }
                    CheatsSet(CheatType::ShowVehiclesFromOtherTrackTypes, !gameState.Cheats.showVehiclesFromOtherTrackTypes);
                }
                break;
                case WIDX_DISABLE_TRAIN_LENGTH_LIMITS:
                {
                    if (!gameState.Cheats.disableTrainLengthLimit)
                    {
                        ContextShowError(STR_WARNING_IN_CAPS, STR_THIS_FEATURE_IS_CURRENTLY_UNSTABLE, {});
                    }
                    CheatsSet(CheatType::DisableTrainLengthLimit, !gameState.Cheats.disableTrainLengthLimit);
                }
                break;
                case WIDX_ENABLE_CHAIN_LIFT_ON_ALL_TRACK:
                    CheatsSet(CheatType::EnableChainLiftOnAllTrack, !gameState.Cheats.enableChainLiftOnAllTrack);
                    break;
                case WIDX_ENABLE_ARBITRARY_RIDE_TYPE_CHANGES:
                {
                    if (!gameState.Cheats.allowArbitraryRideTypeChanges)
                    {
                        ContextShowError(STR_WARNING_IN_CAPS, STR_THIS_FEATURE_IS_CURRENTLY_UNSTABLE, {});
                    }
                    CheatsSet(CheatType::AllowArbitraryRideTypeChanges, !gameState.Cheats.allowArbitraryRideTypeChanges);
                }
                break;
                case WIDX_DISABLE_RIDE_VALUE_AGING:
                    CheatsSet(CheatType::DisableRideValueAging, !gameState.Cheats.disableRideValueAging);
                    break;
                case WIDX_IGNORE_RESEARCH_STATUS:
                    CheatsSet(CheatType::IgnoreResearchStatus, !gameState.Cheats.ignoreResearchStatus);
                    break;
                case WIDX_ENABLE_ALL_DRAWABLE_TRACK_PIECES:
                    CheatsSet(CheatType::EnableAllDrawableTrackPieces, !gameState.Cheats.enableAllDrawableTrackPieces);
                    break;
                case WIDX_ALLOW_TRACK_PLACE_INVALID_HEIGHTS:
                {
                    if (!gameState.Cheats.allowTrackPlaceInvalidHeights)
                    {
                        ContextShowError(STR_WARNING_IN_CAPS, STR_THIS_FEATURE_IS_CURRENTLY_UNSTABLE, {});
                    }
                    CheatsSet(CheatType::AllowTrackPlaceInvalidHeights, !gameState.Cheats.allowTrackPlaceInvalidHeights);
                }
                break;
            }
        }

        void OnResize() override
        {
            ResizeFrameWithPage();
        }
    };

    WindowBase* CheatsOpen()
    {
        auto* windowMgr = GetWindowManager();
        auto* window = windowMgr->BringToFrontByClass(WindowClass::Cheats);
        if (window == nullptr)
        {
            window = windowMgr->Create<CheatsWindow>(WindowClass::Cheats, ScreenCoordsXY(32, 32), WW, WH);
        }
        return window;
    }
} // namespace OpenRCT2::Ui::Windows
