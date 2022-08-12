/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../interface/Theme.h"

#include <algorithm>
#include <cmath>
#include <iterator>
#include <limits>
#include <memory>
#include <openrct2-ui/interface/Dropdown.h>
#include <openrct2-ui/interface/Viewport.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/Cheats.h>
#include <openrct2/Context.h>
#include <openrct2/Game.h>
#include <openrct2/Input.h>
#include <openrct2/Limits.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/actions/GameAction.h>
#include <openrct2/actions/ParkSetParameterAction.h>
#include <openrct2/actions/RideSetAppearanceAction.h>
#include <openrct2/actions/RideSetColourSchemeAction.h>
#include <openrct2/actions/RideSetNameAction.h>
#include <openrct2/actions/RideSetPriceAction.h>
#include <openrct2/actions/RideSetSettingAction.h>
#include <openrct2/actions/RideSetStatusAction.h>
#include <openrct2/audio/audio.h>
#include <openrct2/config/Config.h>
#include <openrct2/core/String.hpp>
#include <openrct2/entity/EntityList.h>
#include <openrct2/entity/Staff.h>
#include <openrct2/localisation/Date.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/localisation/LocalisationService.h>
#include <openrct2/localisation/StringIds.h>
#include <openrct2/network/network.h>
#include <openrct2/object/MusicObject.h>
#include <openrct2/object/ObjectManager.h>
#include <openrct2/object/ObjectRepository.h>
#include <openrct2/object/StationObject.h>
#include <openrct2/rct1/RCT1.h>
#include <openrct2/rct2/T6Exporter.h>
#include <openrct2/ride/RideConstruction.h>
#include <openrct2/ride/RideData.h>
#include <openrct2/ride/ShopItem.h>
#include <openrct2/ride/Station.h>
#include <openrct2/ride/Track.h>
#include <openrct2/ride/TrackData.h>
#include <openrct2/ride/TrackDesign.h>
#include <openrct2/ride/TrackDesignRepository.h>
#include <openrct2/ride/Vehicle.h>
#include <openrct2/sprites.h>
#include <openrct2/windows/Intent.h>
#include <openrct2/world/Park.h>
#include <optional>
#include <string>
#include <vector>

using namespace OpenRCT2;
using namespace OpenRCT2::TrackMetaData;

enum
{
    WINDOW_RIDE_PAGE_MAIN,
    WINDOW_RIDE_PAGE_VEHICLE,
    WINDOW_RIDE_PAGE_OPERATING,
    WINDOW_RIDE_PAGE_MAINTENANCE,
    WINDOW_RIDE_PAGE_COLOUR,
    WINDOW_RIDE_PAGE_MUSIC,
    WINDOW_RIDE_PAGE_MEASUREMENTS,
    WINDOW_RIDE_PAGE_GRAPHS,
    WINDOW_RIDE_PAGE_INCOME,
    WINDOW_RIDE_PAGE_CUSTOMER,
    WINDOW_RIDE_PAGE_COUNT
};

#pragma region Widgets

// clang-format off
enum {
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
    WIDX_TAB_8,
    WIDX_TAB_9,
    WIDX_TAB_10,

    WIDX_VIEWPORT = 14,
    WIDX_VIEW,
    WIDX_VIEW_DROPDOWN,
    WIDX_STATUS,
    WIDX_OPEN,
    WIDX_CONSTRUCTION,
    WIDX_RENAME,
    WIDX_LOCATE,
    WIDX_DEMOLISH,
    WIDX_CLOSE_LIGHT,
    WIDX_SIMULATE_LIGHT,
    WIDX_TEST_LIGHT,
    WIDX_OPEN_LIGHT,
    WIDX_RIDE_TYPE,
    WIDX_RIDE_TYPE_DROPDOWN,

    WIDX_VEHICLE_TYPE = 14,
    WIDX_VEHICLE_TYPE_DROPDOWN,
    WIDX_VEHICLE_TRAINS_PREVIEW,
    WIDX_VEHICLE_TRAINS,
    WIDX_VEHICLE_TRAINS_INCREASE,
    WIDX_VEHICLE_TRAINS_DECREASE,
    WIDX_VEHICLE_CARS_PER_TRAIN,
    WIDX_VEHICLE_CARS_PER_TRAIN_INCREASE,
    WIDX_VEHICLE_CARS_PER_TRAIN_DECREASE,

    WIDX_MODE_TWEAK = 14,
    WIDX_MODE_TWEAK_INCREASE,
    WIDX_MODE_TWEAK_DECREASE,
    WIDX_LIFT_HILL_SPEED,
    WIDX_LIFT_HILL_SPEED_INCREASE,
    WIDX_LIFT_HILL_SPEED_DECREASE,
    WIDX_LOAD_CHECKBOX,
    WIDX_LEAVE_WHEN_ANOTHER_ARRIVES_CHECKBOX,
    WIDX_MINIMUM_LENGTH_CHECKBOX,
    WIDX_MINIMUM_LENGTH,
    WIDX_MINIMUM_LENGTH_INCREASE,
    WIDX_MINIMUM_LENGTH_DECREASE,
    WIDX_MAXIMUM_LENGTH_CHECKBOX,
    WIDX_MAXIMUM_LENGTH,
    WIDX_MAXIMUM_LENGTH_INCREASE,
    WIDX_MAXIMUM_LENGTH_DECREASE,
    WIDX_SYNCHRONISE_WITH_ADJACENT_STATIONS_CHECKBOX,
    WIDX_MODE_TWEAK_LABEL,
    WIDX_LIFT_HILL_SPEED_LABEL,
    WIDX_MODE,
    WIDX_MODE_DROPDOWN,
    WIDX_LOAD,
    WIDX_LOAD_DROPDOWN,
    WIDX_OPERATE_NUMBER_OF_CIRCUITS_LABEL,
    WIDX_OPERATE_NUMBER_OF_CIRCUITS,
    WIDX_OPERATE_NUMBER_OF_CIRCUITS_INCREASE,
    WIDX_OPERATE_NUMBER_OF_CIRCUITS_DECREASE,

    WIDX_INSPECTION_INTERVAL = 14,
    WIDX_INSPECTION_INTERVAL_DROPDOWN,
    WIDX_LOCATE_MECHANIC,
    WIDX_REFURBISH_RIDE,
    WIDX_FORCE_BREAKDOWN,

    WIDX_TRACK_PREVIEW = 14,
    WIDX_TRACK_COLOUR_SCHEME,
    WIDX_TRACK_COLOUR_SCHEME_DROPDOWN,
    WIDX_TRACK_MAIN_COLOUR,
    WIDX_TRACK_ADDITIONAL_COLOUR,
    WIDX_TRACK_SUPPORT_COLOUR,
    WIDX_MAZE_STYLE,
    WIDX_MAZE_STYLE_DROPDOWN,
    WIDX_PAINT_INDIVIDUAL_AREA,
    WIDX_ENTRANCE_PREVIEW,
    WIDX_ENTRANCE_STYLE,
    WIDX_ENTRANCE_STYLE_DROPDOWN,
    WIDX_VEHICLE_PREVIEW,
    WIDX_VEHICLE_COLOUR_SCHEME,
    WIDX_VEHICLE_COLOUR_SCHEME_DROPDOWN,
    WIDX_VEHICLE_COLOUR_INDEX,
    WIDX_VEHICLE_COLOUR_INDEX_DROPDOWN,
    WIDX_VEHICLE_BODY_COLOR,
    WIDX_VEHICLE_TRIM_COLOUR,
    WIDX_VEHICLE_TERNARY_COLOUR,
    WIDX_SELL_ITEM_RANDOM_COLOUR_CHECKBOX,

    WIDX_PLAY_MUSIC = 14,
    WIDX_MUSIC,
    WIDX_MUSIC_DROPDOWN,

    WIDX_SAVE_TRACK_DESIGN = 14,
    WIDX_SELECT_NEARBY_SCENERY,
    WIDX_RESET_SELECTION,
    WIDX_SAVE_DESIGN,
    WIDX_CANCEL_DESIGN,

    WIDX_GRAPH = 14,
    WIDX_GRAPH_VELOCITY,
    WIDX_GRAPH_ALTITUDE,
    WIDX_GRAPH_VERTICAL,
    WIDX_GRAPH_LATERAL,

    WIDX_PRIMARY_PRICE_LABEL = 14,
    WIDX_PRIMARY_PRICE,
    WIDX_PRIMARY_PRICE_INCREASE,
    WIDX_PRIMARY_PRICE_DECREASE,
    WIDX_PRIMARY_PRICE_SAME_THROUGHOUT_PARK,
    WIDX_SECONDARY_PRICE_LABEL,
    WIDX_SECONDARY_PRICE,
    WIDX_SECONDARY_PRICE_INCREASE,
    WIDX_SECONDARY_PRICE_DECREASE,
    WIDX_SECONDARY_PRICE_SAME_THROUGHOUT_PARK,

    WIDX_SHOW_GUESTS_THOUGHTS = 14,
    WIDX_SHOW_GUESTS_ON_RIDE,
    WIDX_SHOW_GUESTS_QUEUING
};

constexpr int32_t RCT1_LIGHT_OFFSET = 4;

static constexpr const StringId WINDOW_TITLE = STR_RIDE_WINDOW_TITLE;
static constexpr const int32_t WH = 207;
static constexpr const int32_t WW = 316;

#define MAIN_RIDE_WIDGETS \
    WINDOW_SHIM(WINDOW_TITLE, WW, WH), \
    MakeWidget({  0, 43}, {316, 137}, WindowWidgetType::Resize, WindowColour::Secondary), \
    MakeTab   ({  3, 17}, STR_VIEW_OF_RIDE_ATTRACTION_TIP                ), \
    MakeTab   ({ 34, 17}, STR_VEHICLE_DETAILS_AND_OPTIONS_TIP            ), \
    MakeTab   ({ 65, 17}, STR_OPERATING_OPTIONS_TIP                      ), \
    MakeTab   ({ 96, 17}, STR_MAINTENANCE_OPTIONS_TIP                    ), \
    MakeTab   ({127, 17}, STR_COLOUR_SCHEME_OPTIONS_TIP                  ), \
    MakeTab   ({158, 17}, STR_SOUND_AND_MUSIC_OPTIONS_TIP                ), \
    MakeTab   ({189, 17}, STR_MEASUREMENTS_AND_TEST_DATA_TIP             ), \
    MakeTab   ({220, 17}, STR_GRAPHS_TIP                                 ), \
    MakeTab   ({251, 17}, STR_INCOME_AND_COSTS_TIP                       ), \
    MakeTab   ({282, 17}, STR_CUSTOMER_INFORMATION_TIP                   )

// 0x009ADC34
static rct_widget window_ride_main_widgets[] = {
    MAIN_RIDE_WIDGETS,
    MakeWidget({  3,  60}, {288, 107}, WindowWidgetType::Viewport,      WindowColour::Secondary, STR_VIEWPORT                                           ),
    MakeWidget({ 35,  46}, {222,  12}, WindowWidgetType::DropdownMenu,      WindowColour::Secondary, 0xFFFFFFFF,                 STR_VIEW_SELECTION         ),
    MakeWidget({245,  47}, { 11,  10}, WindowWidgetType::Button,        WindowColour::Secondary, STR_DROPDOWN_GLYPH,         STR_VIEW_SELECTION         ),
    MakeWidget({  3, 167}, {288,  11}, WindowWidgetType::LabelCentred, WindowColour::Secondary                                                         ),
    MakeWidget({291,  46}, { 24,  24}, WindowWidgetType::FlatBtn,       WindowColour::Secondary, 0xFFFFFFFF,                 STR_OPEN_CLOSE_OR_TEST_RIDE),
    MakeWidget({291,  70}, { 24,  24}, WindowWidgetType::FlatBtn,       WindowColour::Secondary, SPR_CONSTRUCTION,           STR_CONSTRUCTION           ),
    MakeWidget({291,  94}, { 24,  24}, WindowWidgetType::FlatBtn,       WindowColour::Secondary, SPR_RENAME,                 STR_NAME_RIDE_TIP          ),
    MakeWidget({291, 118}, { 24,  24}, WindowWidgetType::FlatBtn,       WindowColour::Secondary, SPR_LOCATE,                 STR_LOCATE_SUBJECT_TIP     ),
    MakeWidget({291, 142}, { 24,  24}, WindowWidgetType::FlatBtn,       WindowColour::Secondary, SPR_DEMOLISH,               STR_DEMOLISH_RIDE_TIP      ),
    MakeWidget({296,  48}, { 14,  14}, WindowWidgetType::ImgBtn,        WindowColour::Secondary, SPR_G2_RCT1_CLOSE_BUTTON_0, STR_CLOSE_RIDE_TIP         ),
    MakeWidget({296,  62}, { 14,  14}, WindowWidgetType::ImgBtn,        WindowColour::Secondary, SPR_G2_RCT1_TEST_BUTTON_0,  STR_SIMULATE_RIDE_TIP      ),
    MakeWidget({296,  62}, { 14,  14}, WindowWidgetType::ImgBtn,        WindowColour::Secondary, SPR_G2_RCT1_TEST_BUTTON_0,  STR_TEST_RIDE_TIP          ),
    MakeWidget({296,  76}, { 14,  14}, WindowWidgetType::ImgBtn,        WindowColour::Secondary, SPR_G2_RCT1_OPEN_BUTTON_0,  STR_OPEN_RIDE_TIP          ),
    MakeWidget({  3, 180}, {305,  12}, WindowWidgetType::DropdownMenu,      WindowColour::Secondary, STR_ARG_6_STRINGID                                     ),
    MakeWidget({297, 180}, { 11,  12}, WindowWidgetType::Button,        WindowColour::Secondary, STR_DROPDOWN_GLYPH                                     ),
    WIDGETS_END,
};

// 0x009ADDA8
static rct_widget window_ride_vehicle_widgets[] = {
    MAIN_RIDE_WIDGETS,
    MakeWidget        ({  7,  50}, {302, 12}, WindowWidgetType::DropdownMenu, WindowColour::Secondary                                                    ),
    MakeWidget        ({297,  51}, { 11, 10}, WindowWidgetType::Button,   WindowColour::Secondary, STR_DROPDOWN_GLYPH                                ),
    MakeWidget        ({  7, 147}, {302, 43}, WindowWidgetType::Scroll,   WindowColour::Secondary, STR_EMPTY                                         ),
    MakeSpinnerWidgets({  7, 196}, {145, 12}, WindowWidgetType::Spinner,  WindowColour::Secondary, STR_RIDE_VEHICLE_COUNT, STR_MAX_VEHICLES_TIP      ),
    MakeSpinnerWidgets({164, 196}, {145, 12}, WindowWidgetType::Spinner,  WindowColour::Secondary, STR_1_CAR_PER_TRAIN,    STR_MAX_CARS_PER_TRAIN_TIP),
    WIDGETS_END,
};

// 0x009ADEFC
static rct_widget window_ride_operating_widgets[] = {
    MAIN_RIDE_WIDGETS,
    MakeSpinnerWidgets({157,  61}, {152, 12}, WindowWidgetType::Spinner,  WindowColour::Secondary, STR_ARG_18_STRINGID                                                                 ), // NB: 3 widgets
    MakeSpinnerWidgets({157,  75}, {152, 12}, WindowWidgetType::Spinner,  WindowColour::Secondary, STR_LIFT_HILL_CHAIN_SPEED_VALUE                                                     ), // NB: 3 widgets
    MakeWidget        ({  7, 109}, { 80, 12}, WindowWidgetType::Checkbox, WindowColour::Secondary, STR_WAIT_FOR,                           STR_WAIT_FOR_PASSENGERS_BEFORE_DEPARTING_TIP),
    MakeWidget        ({  7, 124}, {302, 12}, WindowWidgetType::Checkbox, WindowColour::Secondary                                                                                      ),
    MakeWidget        ({  7, 139}, {150, 12}, WindowWidgetType::Checkbox, WindowColour::Secondary, STR_MINIMUM_WAITING_TIME,               STR_MINIMUM_LENGTH_BEFORE_DEPARTING_TIP     ),
    MakeSpinnerWidgets({157, 139}, {152, 12}, WindowWidgetType::Spinner,  WindowColour::Secondary, STR_ARG_10_STRINGID                                                                 ), // NB: 3 widgets
    MakeWidget        ({  7, 154}, {150, 12}, WindowWidgetType::Checkbox, WindowColour::Secondary, STR_MAXIMUM_WAITING_TIME,               STR_MAXIMUM_LENGTH_BEFORE_DEPARTING_TIP     ),
    MakeSpinnerWidgets({157, 154}, {152, 12}, WindowWidgetType::Spinner,  WindowColour::Secondary, STR_ARG_14_STRINGID                                                                 ), // NB: 3 widgets
    MakeWidget        ({  7, 169}, {302, 12}, WindowWidgetType::Checkbox, WindowColour::Secondary, STR_SYNCHRONISE_WITH_ADJACENT_STATIONS, STR_SYNCHRONISE_WITH_ADJACENT_STATIONS_TIP  ),
    MakeWidget        ({ 21,  61}, {129, 12}, WindowWidgetType::Label,    WindowColour::Secondary                                                                                      ),
    MakeWidget        ({ 21,  75}, {129, 12}, WindowWidgetType::Label,    WindowColour::Secondary, STR_LIFT_HILL_CHAIN_SPEED                                                           ),
    MakeWidget        ({  7,  47}, {302, 12}, WindowWidgetType::DropdownMenu, WindowColour::Secondary, 0xFFFFFFFF,                             STR_SELECT_OPERATING_MODE                   ),
    MakeWidget        ({297,  48}, { 11, 10}, WindowWidgetType::Button,   WindowColour::Secondary, STR_DROPDOWN_GLYPH,                     STR_SELECT_OPERATING_MODE                   ),
    MakeWidget        ({ 87, 109}, {222, 12}, WindowWidgetType::DropdownMenu, WindowColour::Secondary                                                                                      ),
    MakeWidget        ({297, 110}, { 11, 10}, WindowWidgetType::Button,   WindowColour::Secondary, STR_DROPDOWN_GLYPH                                                                  ),
    MakeWidget        ({ 21,  89}, {129, 12}, WindowWidgetType::Label,    WindowColour::Secondary, STR_NUMBER_OF_CIRCUITS,                 STR_NUMBER_OF_CIRCUITS_TIP                  ),
    MakeSpinnerWidgets({157,  89}, {152, 12}, WindowWidgetType::Spinner,  WindowColour::Secondary, STR_NUMBER_OF_CIRCUITS_VALUE                                                        ), // NB: 3 widgets
    WIDGETS_END,
};

// 0x009AE190
static rct_widget window_ride_maintenance_widgets[] = {
    MAIN_RIDE_WIDGETS,
    MakeWidget({107,  71}, {202, 12}, WindowWidgetType::DropdownMenu, WindowColour::Secondary, STR_EMPTY,          STR_SELECT_HOW_OFTEN_A_MECHANIC_SHOULD_CHECK_THIS_RIDE),
    MakeWidget({297,  72}, { 11, 10}, WindowWidgetType::Button,   WindowColour::Secondary, STR_DROPDOWN_GLYPH, STR_SELECT_HOW_OFTEN_A_MECHANIC_SHOULD_CHECK_THIS_RIDE),
    MakeWidget({289, 108}, { 24, 24}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, 0xFFFFFFFF,         STR_LOCATE_NEAREST_AVAILABLE_MECHANIC_TIP             ),
    MakeWidget({265, 108}, { 24, 24}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, SPR_CONSTRUCTION,   STR_REFURBISH_RIDE_TIP                                ),
    MakeWidget({241, 108}, { 24, 24}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, SPR_NO_ENTRY,       STR_DEBUG_FORCE_BREAKDOWN_TIP                         ),
    WIDGETS_END,
};

// 0x009AE2A4
static rct_widget window_ride_colour_widgets[] = {
    MAIN_RIDE_WIDGETS,
    MakeWidget({  3,  47}, { 68, 47}, WindowWidgetType::Spinner,   WindowColour::Secondary                                                                    ),
    MakeWidget({ 74,  49}, {239, 12}, WindowWidgetType::DropdownMenu,  WindowColour::Secondary, STR_ARG_14_STRINGID                                               ),
    MakeWidget({301,  50}, { 11, 10}, WindowWidgetType::Button,    WindowColour::Secondary, STR_DROPDOWN_GLYPH,  STR_COLOUR_SCHEME_TO_CHANGE_TIP              ),
    MakeWidget({ 79,  74}, { 12, 12}, WindowWidgetType::ColourBtn, WindowColour::Secondary, 0xFFFFFFFF,          STR_SELECT_MAIN_COLOUR_TIP                   ),
    MakeWidget({ 99,  74}, { 12, 12}, WindowWidgetType::ColourBtn, WindowColour::Secondary, 0xFFFFFFFF,          STR_SELECT_ADDITIONAL_COLOUR_1_TIP           ),
    MakeWidget({119,  74}, { 12, 12}, WindowWidgetType::ColourBtn, WindowColour::Secondary, 0xFFFFFFFF,          STR_SELECT_SUPPORT_STRUCTURE_COLOUR_TIP      ),
    MakeWidget({ 74,  49}, {239, 12}, WindowWidgetType::DropdownMenu,  WindowColour::Secondary                                                                    ),
    MakeWidget({301,  50}, { 11, 10}, WindowWidgetType::Button,    WindowColour::Secondary, STR_DROPDOWN_GLYPH                                                ),
    MakeWidget({289,  68}, { 24, 24}, WindowWidgetType::FlatBtn,   WindowColour::Secondary, SPR_PAINTBRUSH,      STR_PAINT_INDIVIDUAL_AREA_TIP                ),
    MakeWidget({245, 101}, { 68, 47}, WindowWidgetType::Spinner,   WindowColour::Secondary                                                                    ),
    MakeWidget({103, 103}, {139, 12}, WindowWidgetType::DropdownMenu,  WindowColour::Secondary, STR_EMPTY                                                         ),
    MakeWidget({230, 104}, { 11, 10}, WindowWidgetType::Button,    WindowColour::Secondary, STR_DROPDOWN_GLYPH,  STR_SELECT_STYLE_OF_ENTRANCE_EXIT_STATION_TIP),
    MakeWidget({  3, 157}, { 68, 47}, WindowWidgetType::Scroll,    WindowColour::Secondary, STR_EMPTY                                                         ),
    MakeWidget({ 74, 157}, {239, 12}, WindowWidgetType::DropdownMenu,  WindowColour::Secondary, STR_ARG_6_STRINGID                                                ),
    MakeWidget({301, 158}, { 11, 10}, WindowWidgetType::Button,    WindowColour::Secondary, STR_DROPDOWN_GLYPH,  STR_SELECT_VEHICLE_COLOUR_SCHEME_TIP         ),
    MakeWidget({ 74, 173}, {239, 12}, WindowWidgetType::DropdownMenu,  WindowColour::Secondary                                                                    ),
    MakeWidget({301, 174}, { 11, 10}, WindowWidgetType::Button,    WindowColour::Secondary, STR_DROPDOWN_GLYPH,  STR_SELECT_VEHICLE_TO_MODIFY_TIP             ),
    MakeWidget({ 79, 190}, { 12, 12}, WindowWidgetType::ColourBtn, WindowColour::Secondary, 0xFFFFFFFF,          STR_SELECT_MAIN_COLOUR_TIP                   ),
    MakeWidget({ 99, 190}, { 12, 12}, WindowWidgetType::ColourBtn, WindowColour::Secondary, 0xFFFFFFFF,          STR_SELECT_ADDITIONAL_COLOUR_1_TIP           ),
    MakeWidget({119, 190}, { 12, 12}, WindowWidgetType::ColourBtn, WindowColour::Secondary, 0xFFFFFFFF,          STR_SELECT_ADDITIONAL_COLOUR_2_TIP           ),
    MakeWidget({100,  74}, {239, 12}, WindowWidgetType::Checkbox,  WindowColour::Secondary, STR_RANDOM_COLOUR                                                 ),
    WIDGETS_END,
};

// 0x009AE4C8
static rct_widget window_ride_music_widgets[] = {
    MAIN_RIDE_WIDGETS,
    MakeWidget({  7, 47}, {302, 12}, WindowWidgetType::Checkbox, WindowColour::Secondary, STR_PLAY_MUSIC,     STR_SELECT_MUSIC_TIP      ),
    MakeWidget({  7, 62}, {302, 12}, WindowWidgetType::DropdownMenu, WindowColour::Secondary, STR_EMPTY                                     ),
    MakeWidget({297, 63}, { 11, 10}, WindowWidgetType::Button,   WindowColour::Secondary, STR_DROPDOWN_GLYPH, STR_SELECT_MUSIC_STYLE_TIP),
    WIDGETS_END,
};

// 0x009AE5DC
static rct_widget window_ride_measurements_widgets[] = {
    MAIN_RIDE_WIDGETS,
    MakeWidget({288, 194}, { 24, 24}, WindowWidgetType::FlatBtn, WindowColour::Secondary, SPR_FLOPPY,                STR_SAVE_TRACK_DESIGN),
    MakeWidget({  4, 127}, {154, 14}, WindowWidgetType::Button,  WindowColour::Secondary, STR_SELECT_NEARBY_SCENERY                       ),
    MakeWidget({158, 127}, {154, 14}, WindowWidgetType::Button,  WindowColour::Secondary, STR_RESET_SELECTION                             ),
    MakeWidget({  4, 177}, {154, 14}, WindowWidgetType::Button,  WindowColour::Secondary, STR_DESIGN_SAVE                                 ),
    MakeWidget({158, 177}, {154, 14}, WindowWidgetType::Button,  WindowColour::Secondary, STR_DESIGN_CANCEL                               ),
    WIDGETS_END,
};

// 0x009AE710
static rct_widget window_ride_graphs_widgets[] = {
    MAIN_RIDE_WIDGETS,
    MakeWidget({  3,  46}, {306, 112}, WindowWidgetType::Scroll, WindowColour::Secondary, SCROLL_HORIZONTAL,       STR_LOGGING_DATA_FROM_TIP                               ),
    MakeWidget({  3, 163}, { 73,  14}, WindowWidgetType::Button, WindowColour::Secondary, STR_RIDE_STATS_VELOCITY, STR_SHOW_GRAPH_OF_VELOCITY_AGAINST_TIME_TIP             ),
    MakeWidget({ 76, 163}, { 73,  14}, WindowWidgetType::Button, WindowColour::Secondary, STR_RIDE_STATS_ALTITUDE, STR_SHOW_GRAPH_OF_ALTITUDE_AGAINST_TIME_TIP             ),
    MakeWidget({149, 163}, { 73,  14}, WindowWidgetType::Button, WindowColour::Secondary, STR_RIDE_STATS_VERT_G,   STR_SHOW_GRAPH_OF_VERTICAL_ACCELERATION_AGAINST_TIME_TIP),
    MakeWidget({222, 163}, { 73,  14}, WindowWidgetType::Button, WindowColour::Secondary, STR_RIDE_STATS_LAT_G,    STR_SHOW_GRAPH_OF_LATERAL_ACCELERATION_AGAINST_TIME_TIP ),
    WIDGETS_END,
};

// 0x009AE844
static rct_widget window_ride_income_widgets[] = {
    MAIN_RIDE_WIDGETS,
    MakeWidget        ({ 19,  50}, {126, 14}, WindowWidgetType::Label,    WindowColour::Secondary                                                                    ),
    MakeSpinnerWidgets({147,  50}, {162, 14}, WindowWidgetType::Spinner,  WindowColour::Secondary, STR_ARG_6_CURRENCY2DP                                             ), // NB: 3 widgets
    MakeWidget        ({  5,  62}, {306, 13}, WindowWidgetType::Checkbox, WindowColour::Secondary, STR_SAME_PRICE_THROUGHOUT_PARK, STR_SAME_PRICE_THROUGHOUT_PARK_TIP),
    MakeWidget        ({ 19,  94}, {126, 14}, WindowWidgetType::Label,    WindowColour::Secondary                                                                    ),
    MakeSpinnerWidgets({147,  94}, {162, 14}, WindowWidgetType::Spinner,  WindowColour::Secondary, STR_RIDE_SECONDARY_PRICE_VALUE                                    ), // NB: 3 widgets
    MakeWidget        ({  5, 106}, {306, 13}, WindowWidgetType::Checkbox, WindowColour::Secondary, STR_SAME_PRICE_THROUGHOUT_PARK, STR_SAME_PRICE_THROUGHOUT_PARK_TIP),
    WIDGETS_END,
};

// 0x009AE9C8
static rct_widget window_ride_customer_widgets[] = {
    MAIN_RIDE_WIDGETS,
    MakeWidget({289,  54}, {24, 24}, WindowWidgetType::FlatBtn, WindowColour::Secondary, SPR_SHOW_GUESTS_THOUGHTS_ABOUT_THIS_RIDE_ATTRACTION, STR_SHOW_GUESTS_THOUGHTS_ABOUT_THIS_RIDE_ATTRACTION_TIP),
    MakeWidget({289,  78}, {24, 24}, WindowWidgetType::FlatBtn, WindowColour::Secondary, SPR_SHOW_GUESTS_ON_THIS_RIDE_ATTRACTION,             STR_SHOW_GUESTS_ON_THIS_RIDE_ATTRACTION_TIP            ),
    MakeWidget({289, 102}, {24, 24}, WindowWidgetType::FlatBtn, WindowColour::Secondary, SPR_SHOW_GUESTS_QUEUING_FOR_THIS_RIDE_ATTRACTION,    STR_SHOW_GUESTS_QUEUING_FOR_THIS_RIDE_ATTRACTION_TIP   ),
    WIDGETS_END,
};

static rct_widget *window_ride_page_widgets[] = {
    window_ride_main_widgets,
    window_ride_vehicle_widgets,
    window_ride_operating_widgets,
    window_ride_maintenance_widgets,
    window_ride_colour_widgets,
    window_ride_music_widgets,
    window_ride_measurements_widgets,
    window_ride_graphs_widgets,
    window_ride_income_widgets,
    window_ride_customer_widgets,
};

static constexpr const uint64_t window_ride_page_hold_down_widgets[] = {
    0,
    (1ULL << WIDX_VEHICLE_TRAINS_INCREASE) |
        (1ULL << WIDX_VEHICLE_TRAINS_DECREASE) |
        (1ULL << WIDX_VEHICLE_CARS_PER_TRAIN_INCREASE) |
        (1ULL << WIDX_VEHICLE_CARS_PER_TRAIN_DECREASE),
    (1ULL << WIDX_MODE_TWEAK_INCREASE) |
        (1ULL << WIDX_MODE_TWEAK_DECREASE) |
        (1ULL << WIDX_LIFT_HILL_SPEED_INCREASE) |
        (1ULL << WIDX_LIFT_HILL_SPEED_DECREASE) |
        (1ULL << WIDX_MINIMUM_LENGTH_INCREASE) |
        (1ULL << WIDX_MINIMUM_LENGTH_DECREASE) |
        (1ULL << WIDX_MAXIMUM_LENGTH_INCREASE) |
        (1ULL << WIDX_MAXIMUM_LENGTH_DECREASE) |
        (1ULL << WIDX_OPERATE_NUMBER_OF_CIRCUITS_INCREASE) |
        (1ULL << WIDX_OPERATE_NUMBER_OF_CIRCUITS_DECREASE),
    0,
    0,
    0,
    0,
    0,
    (1ULL << WIDX_PRIMARY_PRICE_INCREASE) |
        (1ULL << WIDX_PRIMARY_PRICE_DECREASE) |
        (1ULL << WIDX_SECONDARY_PRICE_INCREASE) |
        (1ULL << WIDX_SECONDARY_PRICE_DECREASE),
    0,
};
// clang-format on

#pragma endregion

// Cached overall view for each ride
// (Re)calculated when the ride window is opened
struct RideOverallView
{
    CoordsXYZ loc;
    ZoomLevel zoom;
};

static std::vector<RideOverallView> ride_overall_views = {};

static constexpr const int32_t window_ride_tab_animation_divisor[] = {
    0, // WINDOW_RIDE_PAGE_MAIN
    0, // WINDOW_RIDE_PAGE_VEHICLE
    2, // WINDOW_RIDE_PAGE_OPERATING
    2, // WINDOW_RIDE_PAGE_MAINTENANCE
    4, // WINDOW_RIDE_PAGE_COLOUR
    2, // WINDOW_RIDE_PAGE_MUSIC
    8, // WINDOW_RIDE_PAGE_MEASUREMENTS
    8, // WINDOW_RIDE_PAGE_GRAPHS
    2, // WINDOW_RIDE_PAGE_INCOME
    0, // WINDOW_RIDE_PAGE_CUSTOMER
};
static_assert(std::size(window_ride_tab_animation_divisor) == WINDOW_RIDE_PAGE_COUNT);

static constexpr const int32_t window_ride_tab_animation_frames[] = {
    0,  // WINDOW_RIDE_PAGE_MAIN
    0,  // WINDOW_RIDE_PAGE_VEHICLE
    4,  // WINDOW_RIDE_PAGE_OPERATING
    16, // WINDOW_RIDE_PAGE_MAINTENANCE
    8,  // WINDOW_RIDE_PAGE_COLOUR
    16, // WINDOW_RIDE_PAGE_MUSIC
    8,  // WINDOW_RIDE_PAGE_MEASUREMENTS
    8,  // WINDOW_RIDE_PAGE_GRAPHS
    8,  // WINDOW_RIDE_PAGE_INCOME
    0,  // WINDOW_RIDE_PAGE_CUSTOMER
};
static_assert(std::size(window_ride_tab_animation_frames) == WINDOW_RIDE_PAGE_COUNT);

// clang-format off
static constexpr const StringId RatingNames[] = {
    STR_RATING_LOW,
    STR_RATING_MEDIUM,
    STR_RATING_HIGH,
    STR_RATING_VERY_HIGH,
    STR_RATING_EXTREME,
    STR_RATING_ULTRA_EXTREME,
};
// clang-format on

static constexpr const StringId RideBreakdownReasonNames[] = {
    STR_RIDE_BREAKDOWN_SAFETY_CUT_OUT,          // BREAKDOWN_SAFETY_CUT_OUT
    STR_RIDE_BREAKDOWN_RESTRAINTS_STUCK_CLOSED, // BREAKDOWN_RESTRAINTS_STUCK_CLOSED
    STR_RIDE_BREAKDOWN_RESTRAINTS_STUCK_OPEN,   // BREAKDOWN_RESTRAINTS_STUCK_OPEN
    STR_RIDE_BREAKDOWN_DOORS_STUCK_CLOSED,      // BREAKDOWN_DOORS_STUCK_CLOSED
    STR_RIDE_BREAKDOWN_DOORS_STUCK_OPEN,        // BREAKDOWN_DOORS_STUCK_OPEN
    STR_RIDE_BREAKDOWN_VEHICLE_MALFUNCTION,     // BREAKDOWN_VEHICLE_MALFUNCTION
    STR_RIDE_BREAKDOWN_BRAKES_FAILURE,          // BREAKDOWN_BRAKES_FAILURE
    STR_RIDE_BREAKDOWN_CONTROL_FAILURE,         // BREAKDOWN_CONTROL_FAILURE
};
static_assert(std::size(RideBreakdownReasonNames) == BREAKDOWN_COUNT);

const StringId ColourSchemeNames[] = {
    STR_MAIN_COLOUR_SCHEME,          // RIDE_COLOUR_SCHEME_MAIN
    STR_ALTERNATIVE_COLOUR_SCHEME_1, // RIDE_COLOUR_SCHEME_ADDITIONAL_1
    STR_ALTERNATIVE_COLOUR_SCHEME_2, // RIDE_COLOUR_SCHEME_ADDITIONAL_2
    STR_ALTERNATIVE_COLOUR_SCHEME_3, // RIDE_COLOUR_SCHEME_ADDITIONAL_3
};
static_assert(std::size(ColourSchemeNames) == RIDE_COLOUR_SCHEME_COUNT);

static constexpr const StringId VehicleLoadNames[] = {
    STR_QUARTER_LOAD,       //  WAIT_FOR_LOAD_QUARTER
    STR_HALF_LOAD,          //  WAIT_FOR_LOAD_HALF
    STR_THREE_QUARTER_LOAD, //  WAIT_FOR_LOAD_THREE_QUARTER
    STR_FULL_LOAD,          //  WAIT_FOR_LOAD_FULL
    STR_ANY_LOAD,           //  WAIT_FOR_LOAD_ANY
};
static_assert(std::size(VehicleLoadNames) == WAIT_FOR_LOAD_COUNT);

static constexpr const StringId VehicleColourSchemeNames[] = {
    STR_ALL_VEHICLES_IN_SAME_COLOURS,  // RIDE_COLOUR_SCHEME_MODE_ALL_SAME,
    STR_DIFFERENT_COLOURS_PER,         // RIDE_COLOUR_SCHEME_MODE_DIFFERENT_PER_TRAIN,
    STR_DIFFERENT_COLOURS_PER_VEHICLE, // RIDE_COLOUR_SCHEME_MODE_DIFFERENT_PER_CAR,
};
static_assert(std::size(VehicleColourSchemeNames) == RIDE_COLOUR_SCHEME_MODE_COUNT);

static constexpr const StringId VehicleStatusNames[] = {
    STR_MOVING_TO_END_OF,          // Vehicle::Status::MovingToEndOfStation
    STR_WAITING_FOR_PASSENGERS_AT, // Vehicle::Status::WaitingForPassengers
    STR_WAITING_TO_DEPART,         // Vehicle::Status::WaitingToDepart
    STR_DEPARTING,                 // Vehicle::Status::Departing
    STR_TRAVELLING_AT_0,           // Vehicle::Status::Travelling
    STR_ARRIVING_AT,               // Vehicle::Status::Arriving
    STR_UNLOADING_PASSENGERS_AT,   // Vehicle::Status::UnloadingPassengers
    STR_TRAVELLING_AT_1,           // Vehicle::Status::TravellingBoat
    STR_CRASHING,                  // Vehicle::Status::Crashing
    STR_CRASHED_0,                 // Vehicle::Status::Crashed
    STR_TRAVELLING_AT_2,           // Vehicle::Status::TravellingDodgems
    STR_SWINGING,                  // Vehicle::Status::Swinging
    STR_ROTATING_0,                // Vehicle::Status::Rotating
    STR_ROTATING_1,                // Vehicle::Status::FerrisWheelRotating
    STR_OPERATING_0,               // Vehicle::Status::SimulatorOperating
    STR_SHOWING_FILM,              // Vehicle::Status::ShowingFilm
    STR_ROTATING_2,                // Vehicle::Status::SpaceRingsOperating
    STR_OPERATING_1,               // Vehicle::Status::TopSpinOperating
    STR_OPERATING_2,               // Vehicle::Status::HauntedHouseOperating
    STR_DOING_CIRCUS_SHOW,         // Vehicle::Status::DoingCircusShow
    STR_OPERATING_3,               // Vehicle::Status::CrookedHouseOperating
    STR_WAITING_FOR_CABLE_LIFT,    // Vehicle::Status::WaitingForCableLift
    STR_TRAVELLING_AT_3,           // Vehicle::Status::TravellingCableLift
    STR_STOPPING_0,                // Vehicle::Status::Stopping
    STR_WAITING_FOR_PASSENGERS,    // Vehicle::Status::WaitingForPassengers17
    STR_WAITING_TO_START,          // Vehicle::Status::WaitingToStart
    STR_STARTING,                  // Vehicle::Status::Starting
    STR_OPERATING,                 // Vehicle::Status::Operating1A
    STR_STOPPING_1,                // Vehicle::Status::Stopping1B
    STR_UNLOADING_PASSENGERS,      // Vehicle::Status::UnloadingPassengers1C
    STR_STOPPED_BY_BLOCK_BRAKES,   // Vehicle::Status::StoppedByBlockBrakes
};

static constexpr const StringId SingleSessionVehicleStatusNames[] = {
    STR_STOPPING_0,             // Vehicle::Status::MovingToEndOfStation
    STR_WAITING_FOR_PASSENGERS, // Vehicle::Status::WaitingForPassengers
    STR_WAITING_TO_START,       // Vehicle::Status::WaitingToDepart
    STR_STARTING,               // Vehicle::Status::Departing
    STR_OPERATING,              // Vehicle::Status::Travelling
    STR_STOPPING_1,             // Vehicle::Status::Arriving
    STR_UNLOADING_PASSENGERS,   // Vehicle::Status::UnloadingPassengers
};

struct WindowRideMazeDesignOption
{
    StringId text;
    uint32_t sprite;
};

static constexpr const WindowRideMazeDesignOption MazeOptions[] = {
    { STR_RIDE_DESIGN_MAZE_BRICK_WALLS, SPR_RIDE_DESIGN_PREVIEW_MAZE_BRICK_WALLS },
    { STR_RIDE_DESIGN_MAZE_HEDGES, SPR_RIDE_DESIGN_PREVIEW_MAZE_HEDGES },
    { STR_RIDE_DESIGN_MAZE_ICE_BLOCKS, SPR_RIDE_DESIGN_PREVIEW_MAZE_ICE_BLOCKS },
    { STR_RIDE_DESIGN_MAZE_WOODEN_FENCES, SPR_RIDE_DESIGN_PREVIEW_MAZE_WOODEN_FENCES },
};

struct GraphsYAxis
{
    uint8_t interval;
    int8_t unit;
    int8_t unit_interval;
    StringId label;
};

/** rct2: 0x0098DD98 */
static constexpr const GraphsYAxis window_graphs_y_axi[] = {
    { 11, 0, 10, STR_RIDE_STATS_VELOCITY_FORMAT }, // GRAPH_VELOCITY
    { 10, 0, 15, STR_RIDE_STATS_ALTITUDE_FORMAT }, // GRAPH_ALTITUDE
    { 13, -3, 1, STR_RIDE_STATS_G_FORCE_FORMAT },  // GRAPH_VERTICAL
    { 13, -4, 1, STR_RIDE_STATS_G_FORCE_FORMAT },  // GRAPH_LATERAL
};

static constexpr auto RIDE_G_FORCES_RED_NEG_VERTICAL = -FIXED_2DP(2, 50);
static constexpr auto RIDE_G_FORCES_RED_LATERAL = FIXED_2DP(2, 80);

// Used for sorting the ride type cheat dropdown.
struct RideTypeLabel
{
    ride_type_t ride_type_id;
    StringId label_id;
    const char* label_string;
};

// Used for sorting the vehicle type dropdown.
struct VehicleTypeLabel
{
    ObjectEntryIndex subtype_id;
    StringId label_id;
    const char* label_string;
};

static void TrackDesignCallback(int32_t result, [[maybe_unused]] const utf8* path)
{
    if (result == MODAL_RESULT_OK)
    {
        track_repository_scan();
    }
    gfx_invalidate_screen();
};

/**
 *
 *  rct2: 0x006D2804
  when al == 0*/
static void CancelScenerySelection()
{
    gGamePaused &= ~GAME_PAUSED_SAVING_TRACK;
    gTrackDesignSaveMode = false;
    OpenRCT2::Audio::Resume();

    rct_window* main_w = window_get_main();
    if (main_w != nullptr)
    {
        main_w->viewport->flags &= ~(VIEWPORT_FLAG_HIDE_VERTICAL | VIEWPORT_FLAG_HIDE_BASE);
    }

    gfx_invalidate_screen();
    tool_cancel();
}

/**
 *
 *  rct2: 0x006AD4DA
 */
extern void WindowRideMeasurementsDesignCancel()
{
    if (gTrackDesignSaveMode)
    {
        CancelScenerySelection();
    }
}

class RideWindow final : public Window
{
private:
    utf8 _moneyInputText[MONEY_STRING_MAXLENGTH];

    bool _collectTrackDesignScenery = false;
    int32_t _lastSceneryX = 0;
    int32_t _lastSceneryY = 0;
    std::unique_ptr<TrackDesign> _trackDesign;

    int32_t RideDropdownDataLanguage = LANGUAGE_UNDEFINED;
    std::vector<RideTypeLabel> RideDropdownData;

    int32_t VehicleDropdownDataLanguage = LANGUAGE_UNDEFINED;
    rct_ride_entry* VehicleDropdownRideType = nullptr;
    bool VehicleDropdownExpanded = false;
    std::vector<VehicleTypeLabel> VehicleDropdownData;

public:
#pragma region Window Override Events
    void OnOpen() override
    {
        // We can't use this event because the window needs to know what ride is currently open before initializing
    }

    void OnClose() override
    {
        switch (page)
        {
            case WINDOW_RIDE_PAGE_COLOUR:
                WindowRideColourClose();
                break;
            case WINDOW_RIDE_PAGE_MEASUREMENTS:
                WindowRideMeasurementsClose();
                break;
        };
    }

    void OnResize() override
    {
        switch (page)
        {
            case WINDOW_RIDE_PAGE_MAIN:
                WindowRideMainResize();
                break;
            case WINDOW_RIDE_PAGE_VEHICLE:
                WindowRideVehicleResize();
                break;
            case WINDOW_RIDE_PAGE_OPERATING:
                WindowRideOperatingResize();
                break;
            case WINDOW_RIDE_PAGE_MAINTENANCE:
                WindowRideMaintenanceResize();
                break;
            case WINDOW_RIDE_PAGE_COLOUR:
                WindowRideColourResize();
                break;
            case WINDOW_RIDE_PAGE_MUSIC:
                WindowRideMusicResize();
                break;
            case WINDOW_RIDE_PAGE_MEASUREMENTS:
                WindowRideMeasurementsResize();
                break;
            case WINDOW_RIDE_PAGE_GRAPHS:
                WindowRideGraphsResize();
                break;
            case WINDOW_RIDE_PAGE_INCOME:
                WindowRideIncomeResize();
                break;
            case WINDOW_RIDE_PAGE_CUSTOMER:
                WindowRideCustomerResize();
                break;
        };
    }

    void OnUpdate() override
    {
        switch (page)
        {
            case WINDOW_RIDE_PAGE_MAIN:
                WindowRideMainUpdate();
                break;
            case WINDOW_RIDE_PAGE_VEHICLE:
                WindowRideVehicleUpdate();
                break;
            case WINDOW_RIDE_PAGE_OPERATING:
                WindowRideOperatingUpdate();
                break;
            case WINDOW_RIDE_PAGE_MAINTENANCE:
                WindowRideMaintenanceUpdate();
                break;
            case WINDOW_RIDE_PAGE_COLOUR:
                WindowRideColourUpdate();
                break;
            case WINDOW_RIDE_PAGE_MUSIC:
                WindowRideMusicUpdate();
                break;
            case WINDOW_RIDE_PAGE_MEASUREMENTS:
                WindowRideMeasurementsUpdate();
                break;
            case WINDOW_RIDE_PAGE_GRAPHS:
                WindowRideGraphsUpdate();
                break;
            case WINDOW_RIDE_PAGE_INCOME:
                WindowRideIncomeUpdate();
                break;
            case WINDOW_RIDE_PAGE_CUSTOMER:
                WindowRideCustomerUpdate();
                break;
        };
    }

    void OnPrepareDraw() override
    {
        switch (page)
        {
            case WINDOW_RIDE_PAGE_MAIN:
                WindowRideMainInvalidate();
                break;
            case WINDOW_RIDE_PAGE_VEHICLE:
                WindowRideVehicleInvalidate();
                break;
            case WINDOW_RIDE_PAGE_OPERATING:
                WindowRideOperatingInvalidate();
                break;
            case WINDOW_RIDE_PAGE_MAINTENANCE:
                WindowRideMaintenanceInvalidate();
                break;
            case WINDOW_RIDE_PAGE_COLOUR:
                WindowRideColourInvalidate();
                break;
            case WINDOW_RIDE_PAGE_MUSIC:
                WindowRideMusicInvalidate();
                break;
            case WINDOW_RIDE_PAGE_MEASUREMENTS:
                WindowRideMeasurementsInvalidate();
                break;
            case WINDOW_RIDE_PAGE_GRAPHS:
                WindowRideGraphsInvalidate();
                break;
            case WINDOW_RIDE_PAGE_INCOME:
                WindowRideIncomeInvalidate();
                break;
            case WINDOW_RIDE_PAGE_CUSTOMER:
                WindowRideCustomerInvalidate();
                break;
        };
    }

    void OnDraw(rct_drawpixelinfo& dpi) override
    {
        switch (page)
        {
            case WINDOW_RIDE_PAGE_MAIN:
                WindowRideMainPaint(&dpi);
                break;
            case WINDOW_RIDE_PAGE_VEHICLE:
                WindowRideVehiclePaint(&dpi);
                break;
            case WINDOW_RIDE_PAGE_OPERATING:
                WindowRideOperatingPaint(&dpi);
                break;
            case WINDOW_RIDE_PAGE_MAINTENANCE:
                WindowRideMaintenancePaint(&dpi);
                break;
            case WINDOW_RIDE_PAGE_COLOUR:
                WindowRideColourPaint(&dpi);
                break;
            case WINDOW_RIDE_PAGE_MUSIC:
                WindowRideMusicPaint(&dpi);
                break;
            case WINDOW_RIDE_PAGE_MEASUREMENTS:
                WindowRideMeasurementsPaint(&dpi);
                break;
            case WINDOW_RIDE_PAGE_GRAPHS:
                WindowRideGraphsPaint(&dpi);
                break;
            case WINDOW_RIDE_PAGE_INCOME:
                WindowRideIncomePaint(&dpi);
                break;
            case WINDOW_RIDE_PAGE_CUSTOMER:
                WindowRideCustomerPaint(&dpi);
                break;
        };
    }

    OpenRCT2String OnTooltip(WidgetIndex widgetIndex, StringId fallback) override
    {
        switch (page)
        {
            case WINDOW_RIDE_PAGE_VEHICLE:
                return WindowRideVehicleTooltip(widgetIndex, fallback);
                break;
            case WINDOW_RIDE_PAGE_GRAPHS:
                return WindowRideVehicleTooltip(widgetIndex, fallback);
                break;
            default:
                return { fallback, {} };
        };
    }

    void OnMouseDown(WidgetIndex widgetIndex) override
    {
        switch (page)
        {
            case WINDOW_RIDE_PAGE_MAIN:
                WindowRideMainMousedown(widgetIndex, &widgets[widgetIndex]);
                break;
            case WINDOW_RIDE_PAGE_VEHICLE:
                WindowRideVehicleMousedown(widgetIndex, &widgets[widgetIndex]);
                break;
            case WINDOW_RIDE_PAGE_OPERATING:
                WindowRideOperatingMousedown(widgetIndex, &widgets[widgetIndex]);
                break;
            case WINDOW_RIDE_PAGE_MAINTENANCE:
                WindowRideMaintenanceMousedown(widgetIndex, &widgets[widgetIndex]);
                break;
            case WINDOW_RIDE_PAGE_COLOUR:
                WindowRideColourMousedown(widgetIndex, &widgets[widgetIndex]);
                break;
            case WINDOW_RIDE_PAGE_MUSIC:
                WindowRideMusicMousedown(widgetIndex, &widgets[widgetIndex]);
                break;
            case WINDOW_RIDE_PAGE_MEASUREMENTS:
                WindowRideMeasurementsMousedown(widgetIndex, &widgets[widgetIndex]);
                break;
            case WINDOW_RIDE_PAGE_GRAPHS:
                WindowRideGraphsMousedown(widgetIndex, &widgets[widgetIndex]);
                break;
            case WINDOW_RIDE_PAGE_INCOME:
                WindowRideIncomeMousedown(widgetIndex, &widgets[widgetIndex]);
                break;
        };
    }

    void OnMouseUp(WidgetIndex widgetIndex) override
    {
        switch (page)
        {
            case WINDOW_RIDE_PAGE_MAIN:
                WindowRideMainMouseup(widgetIndex);
                break;
            case WINDOW_RIDE_PAGE_VEHICLE:
                WindowRideVehicleMouseup(widgetIndex);
                break;
            case WINDOW_RIDE_PAGE_OPERATING:
                WindowRideOperatingMouseup(widgetIndex);
                break;
            case WINDOW_RIDE_PAGE_MAINTENANCE:
                WindowRideMaintenanceMouseup(widgetIndex);
                break;
            case WINDOW_RIDE_PAGE_COLOUR:
                WindowRideColourMouseup(widgetIndex);
                break;
            case WINDOW_RIDE_PAGE_MUSIC:
                WindowRideMusicMouseup(widgetIndex);
                break;
            case WINDOW_RIDE_PAGE_MEASUREMENTS:
                WindowRideMeasurementsMouseup(widgetIndex);
                break;
            case WINDOW_RIDE_PAGE_GRAPHS:
                WindowRideGraphsMouseup(widgetIndex);
                break;
            case WINDOW_RIDE_PAGE_INCOME:
                WindowRideIncomeMouseup(widgetIndex);
                break;
            case WINDOW_RIDE_PAGE_CUSTOMER:
                WindowRideCustomerMouseup(widgetIndex);
                break;
        };
    }

    void OnDropdown(WidgetIndex widgetIndex, int32_t selectedIndex) override
    {
        switch (page)
        {
            case WINDOW_RIDE_PAGE_MAIN:
                WindowRideMainDropdown(widgetIndex, selectedIndex);
                break;
            case WINDOW_RIDE_PAGE_VEHICLE:
                WindowRideVehicleDropdown(widgetIndex, selectedIndex);
                break;
            case WINDOW_RIDE_PAGE_OPERATING:
                WindowRideOperatingDropdown(widgetIndex, selectedIndex);
                break;
            case WINDOW_RIDE_PAGE_MAINTENANCE:
                WindowRideMaintenanceDropdown(widgetIndex, selectedIndex);
                break;
            case WINDOW_RIDE_PAGE_COLOUR:
                WindowRideColourDropdown(widgetIndex, selectedIndex);
                break;
            case WINDOW_RIDE_PAGE_MUSIC:
                WindowRideMusicDropdown(widgetIndex, selectedIndex);
                break;
            case WINDOW_RIDE_PAGE_MEASUREMENTS:
                WindowRideMeasurementsDropdown(widgetIndex, selectedIndex);
                break;
        };
    }

    void OnTextInput(WidgetIndex widgetIndex, std::string_view text) override
    {
        switch (page)
        {
            case WINDOW_RIDE_PAGE_MAIN:
                WindowRideMainTextinput(widgetIndex, text);
                break;
            case WINDOW_RIDE_PAGE_OPERATING:
                WindowRideOperatingTextinput(widgetIndex, text);
                break;
            case WINDOW_RIDE_PAGE_INCOME:
                WindowRideIncomeTextinput(widgetIndex, text);
                break;
        };
    }

    ScreenSize OnScrollGetSize(int32_t scrollIndex) override
    {
        switch (page)
        {
            case WINDOW_RIDE_PAGE_GRAPHS:
                return WindowRideGraphsScrollgetheight(scrollIndex);
                break;
            default:
                return {};
        };
    }

    void OnScrollSelect(int32_t scrollIndex, int32_t scrollType)
    {
        switch (page)
        {
            case WINDOW_RIDE_PAGE_GRAPHS:
                WindowRideGraphs15(scrollIndex, scrollType);
                break;
        };
    }

    void OnScrollDraw(int32_t scrollIndex, rct_drawpixelinfo& dpi) override
    {
        switch (page)
        {
            case WINDOW_RIDE_PAGE_VEHICLE:
                WindowRideVehicleScrollpaint(&dpi, scrollIndex);
                break;
            case WINDOW_RIDE_PAGE_COLOUR:
                WindowRideColourScrollpaint(&dpi, scrollIndex);
                break;
            case WINDOW_RIDE_PAGE_GRAPHS:
                WindowRideGraphsScrollpaint(&dpi, scrollIndex);
                break;
        };
    }

    void OnToolDown(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords) override
    {
        switch (page)
        {
            case WINDOW_RIDE_PAGE_COLOUR:
                WindowRideColourTooldown(widgetIndex, screenCoords);
                break;
            case WINDOW_RIDE_PAGE_MEASUREMENTS:
                WindowRideMeasurementsTooldown(widgetIndex, screenCoords);
                break;
        };
    }

    void OnToolDrag(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords) override
    {
        switch (page)
        {
            case WINDOW_RIDE_PAGE_COLOUR:
                WindowRideColourTooldrag(widgetIndex, screenCoords);
                break;
            case WINDOW_RIDE_PAGE_MEASUREMENTS:
                WindowRideMeasurementsTooldrag(widgetIndex, screenCoords);
                break;
        };
    }

    void OnToolAbort(WidgetIndex widgetIndex) override
    {
        switch (page)
        {
            case WINDOW_RIDE_PAGE_MEASUREMENTS:
                WindowRideMeasurementsToolabort(widgetIndex);
                break;
        };
    }

    void OnViewportRotate() override
    {
        switch (page)
        {
            case WINDOW_RIDE_PAGE_MAIN:
                WindowRideInitViewport();
                break;
        };
    }
#pragma endregion

    void OnWindowRideOpen(Ride* windowRide)
    {
        assert(windowRide != nullptr);

        widgets = window_ride_page_widgets[WINDOW_RIDE_PAGE_MAIN];
        hold_down_widgets = window_ride_page_hold_down_widgets[WINDOW_RIDE_PAGE_MAIN];
        rideId = windowRide->id;

        page = WINDOW_RIDE_PAGE_MAIN;
        vehicleIndex = 0;
        frame_no = 0;
        list_information_type = 0;
        picked_peep_frame = 0;
        ride_colour = 0;
        WindowRideDisableTabs();
        min_width = 316;
        min_height = 180;
        max_width = 500;
        max_height = 450;

        WindowRideUpdateOverallView(windowRide);

        PopulateVehicleTypeDropdown(windowRide, true);

        windowRide = nullptr;
    }

    void OnWindowRideMainOpen(Ride* windowRide)
    {
        if (input_test_flag(INPUT_FLAG_TOOL_ACTIVE))
        {
            if (classification == gCurrentToolWidget.window_classification && number == gCurrentToolWidget.window_number)
            {
                tool_cancel();
            }
        }

        if (page != WINDOW_RIDE_PAGE_MAIN)
        {
            WindowRideSetPage(WINDOW_RIDE_PAGE_MAIN);
        }

        WindowRideInitViewport();
    }

    void OnWindowRideOpenStation(Ride* windowRide, StationIndex stationIndex)
    {
        if (input_test_flag(INPUT_FLAG_TOOL_ACTIVE) && gCurrentToolWidget.window_classification == classification
            && gCurrentToolWidget.window_number == number)
        {
            tool_cancel();
        }

        page = WINDOW_RIDE_PAGE_MAIN;
        width = 316;
        height = 180;
        Invalidate();

        widgets = window_ride_page_widgets[page];
        hold_down_widgets = window_ride_page_hold_down_widgets[page];
        pressed_widgets = 0;
        WindowRideDisableTabs();
        WindowInitScrollWidgets(*this);

        // View
        for (int32_t i = stationIndex.ToUnderlying(); i >= 0; i--)
        {
            if (windowRide->GetStations()[i].Start.IsNull())
            {
                stationIndex = StationIndex::FromUnderlying(stationIndex.ToUnderlying() - 1);
            }
        }

        ride.view = 1 + windowRide->num_vehicles + stationIndex.ToUnderlying();
        WindowRideInitViewport();
    }

    void OnWindowRideOpenVehicle(int32_t viewIndex)
    {
        page = WINDOW_RIDE_PAGE_MAIN;
        width = 316;
        height = 180;
        Invalidate();

        widgets = window_ride_page_widgets[page];
        hold_down_widgets = window_ride_page_hold_down_widgets[page];
        pressed_widgets = 0;
        WindowRideDisableTabs();
        WindowInitScrollWidgets(*this);

        ride.view = viewIndex;

        WindowRideInitViewport();
        Invalidate();
    }

    /**
     *
     *  rct2: 0x006AF1D2
     */
    void WindowRideSetPage(int32_t newPage)
    {
        int32_t listen;

        if (input_test_flag(INPUT_FLAG_TOOL_ACTIVE))
            if (classification == gCurrentToolWidget.window_classification && number == gCurrentToolWidget.window_number)
                tool_cancel();

        if (newPage == WINDOW_RIDE_PAGE_VEHICLE)
        {
            auto constructionWindow = window_find_by_class(WindowClass::RideConstruction);
            if (constructionWindow != nullptr && constructionWindow->number == number)
            {
                window_close_by_class(WindowClass::RideConstruction);
                // Closing the construction window sets the tab to the first page, which we don't want here,
                // as user just clicked the Vehicle page
                WindowRideSetPage(WINDOW_RIDE_PAGE_VEHICLE);
            }
        }

        // Set listen only to viewport
        listen = 0;
        if (newPage == WINDOW_RIDE_PAGE_MAIN && page == WINDOW_RIDE_PAGE_MAIN && viewport != nullptr
            && !(viewport->flags & VIEWPORT_FLAG_SOUND_ON))
            listen++;

        page = newPage;
        frame_no = 0;
        picked_peep_frame = 0;

        // There doesn't seem to be any need for this call, and it can sometimes modify the reported number of cars per train,
        // so I've removed it if (page == WINDOW_RIDE_PAGE_VEHICLE) { ride_update_max_vehicles(windowRide);
        //}

        RemoveViewport();

        hold_down_widgets = window_ride_page_hold_down_widgets[newPage];
        pressed_widgets = 0;
        widgets = window_ride_page_widgets[newPage];
        WindowRideDisableTabs();
        Invalidate();

        window_event_resize_call(this);
        window_event_invalidate_call(this);
        WindowInitScrollWidgets(*this);
        Invalidate();

        if (listen != 0 && viewport != nullptr)
            viewport->flags |= VIEWPORT_FLAG_SOUND_ON;
    }

    /**
     *
     *  rct2: 0x006AF994
     */
    void WindowRideInitViewport()
    {
        if (page != WINDOW_RIDE_PAGE_MAIN)
            return;

        auto windowRide = get_ride(rideId);
        if (windowRide == nullptr)
            return;

        int32_t viewSelectionIndex = ride.view - 1;

        std::optional<Focus> newFocus;

        if (viewSelectionIndex >= 0 && viewSelectionIndex < windowRide->num_vehicles
            && windowRide->lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK)
        {
            auto vehId = windowRide->vehicles[viewSelectionIndex];
            rct_ride_entry* ride_entry = windowRide->GetRideEntry();
            if (ride_entry != nullptr && ride_entry->TabCar != 0)
            {
                Vehicle* vehicle = GetEntity<Vehicle>(vehId);
                if (vehicle == nullptr)
                {
                    vehId = EntityId::GetNull();
                }
                else if (!vehicle->next_vehicle_on_train.IsNull())
                {
                    vehId = vehicle->next_vehicle_on_train;
                }
            }
            if (!vehId.IsNull())
            {
                newFocus = Focus(vehId);
            }
        }
        else if (
            viewSelectionIndex >= windowRide->num_vehicles
            && viewSelectionIndex < (windowRide->num_vehicles + windowRide->num_stations))
        {
            auto stationIndex = GetStationIndexFromViewSelection();
            if (stationIndex)
            {
                const auto location = windowRide->GetStation(*stationIndex).GetStart();
                newFocus = Focus(location);
            }
        }
        else
        {
            if (viewSelectionIndex > 0)
            {
                ride.view = 0;
            }
            if (number < ride_overall_views.size())
            {
                const auto& view = ride_overall_views[number];
                newFocus = Focus(view.loc, view.zoom);
            }
        }

        uint16_t viewport_flags = 0;
        if (viewport != nullptr)
        {
            if (newFocus == focus)
            {
                return;
            }
            viewport_flags = viewport->flags;
            RemoveViewport();
        }
        else if (gConfigGeneral.always_show_gridlines)
        {
            viewport_flags |= VIEWPORT_FLAG_GRIDLINES;
        }

        window_event_invalidate_call(this);

        focus = newFocus;

        // rct2: 0x006aec9c only used here so brought it into the function
        if (viewport == nullptr && !windowRide->overall_view.IsNull() && focus.has_value())
        {
            const auto& view_widget = widgets[WIDX_VIEWPORT];

            auto screenPos = windowPos + ScreenCoordsXY{ view_widget.left + 1, view_widget.top + 1 };
            int32_t newWidth = view_widget.width() - 1;
            int32_t newHeight = view_widget.height() - 1;

            viewport_create(this, screenPos, newWidth, newHeight, focus.value());

            flags |= WF_NO_SCROLLING;
            Invalidate();
        }
        if (viewport != nullptr)
        {
            viewport->flags = viewport_flags;
            Invalidate();
        }
    }

    /**
     *
     * rct2: 0x006AEB9F
     */
    void WindowRideDisableTabs()
    {
        uint32_t disabled_tabs = 0;
        auto windowRide = get_ride(rideId);
        if (windowRide == nullptr)
            return;

        const auto& rtd = windowRide->GetRideTypeDescriptor();

        if (!rtd.HasFlag(RIDE_TYPE_FLAG_HAS_DATA_LOGGING))
            disabled_tabs |= (1ULL << WIDX_TAB_8); // 0x800

        if (windowRide->type == RIDE_TYPE_MINI_GOLF)
            disabled_tabs |= (1ULL << WIDX_TAB_2 | 1ULL << WIDX_TAB_3 | 1ULL << WIDX_TAB_4); // 0xE0

        if (rtd.HasFlag(RIDE_TYPE_FLAG_NO_VEHICLES))
            disabled_tabs |= (1ULL << WIDX_TAB_2); // 0x20

        if (!rtd.HasFlag(RIDE_TYPE_FLAG_HAS_TRACK_COLOUR_MAIN) && !rtd.HasFlag(RIDE_TYPE_FLAG_HAS_TRACK_COLOUR_ADDITIONAL)
            && !rtd.HasFlag(RIDE_TYPE_FLAG_HAS_TRACK_COLOUR_SUPPORTS) && !rtd.HasFlag(RIDE_TYPE_FLAG_HAS_VEHICLE_COLOURS)
            && !rtd.HasFlag(RIDE_TYPE_FLAG_HAS_ENTRANCE_EXIT))
        {
            disabled_tabs |= (1ULL << WIDX_TAB_5); // 0x100
        }

        if (rtd.HasFlag(RIDE_TYPE_FLAG_IS_SHOP_OR_FACILITY))
            disabled_tabs |= (1ULL << WIDX_TAB_3 | 1ULL << WIDX_TAB_4 | 1ULL << WIDX_TAB_7); // 0x4C0

        if (!rtd.HasFlag(RIDE_TYPE_FLAG_ALLOW_MUSIC))
        {
            disabled_tabs |= (1ULL << WIDX_TAB_6); // 0x200
        }

        if (rtd.HasFlag(RIDE_TYPE_FLAG_IS_CASH_MACHINE) || rtd.HasFlag(RIDE_TYPE_FLAG_IS_FIRST_AID)
            || (gParkFlags & PARK_FLAGS_NO_MONEY) != 0)
            disabled_tabs |= (1ULL << WIDX_TAB_9); // 0x1000

        if ((gScreenFlags & SCREEN_FLAGS_TRACK_DESIGNER) != 0)
            disabled_tabs |= (1ULL << WIDX_TAB_4 | 1ULL << WIDX_TAB_6 | 1ULL << WIDX_TAB_9 | 1ULL << WIDX_TAB_10); // 0x3280

        rct_ride_entry* rideEntry = get_ride_entry(windowRide->subtype);

        if (rideEntry == nullptr)
        {
            disabled_tabs |= 1ULL << WIDX_TAB_2 | 1ULL << WIDX_TAB_3 | 1ULL << WIDX_TAB_4 | 1ULL << WIDX_TAB_5
                | 1ULL << WIDX_TAB_6 | 1ULL << WIDX_TAB_7 | 1ULL << WIDX_TAB_8 | 1ULL << WIDX_TAB_9 | 1ULL << WIDX_TAB_10;
        }
        else if ((rideEntry->flags & RIDE_ENTRY_FLAG_DISABLE_COLOUR_TAB) != 0)
        {
            disabled_tabs |= (1ULL << WIDX_TAB_5);
        }

        disabled_widgets = disabled_tabs;
    }

private:
#pragma region Page Methods

#pragma region Main

    std::optional<StationIndex> GetStationIndexFromViewSelection()
    {
        const auto* windowRide = get_ride(RideId::FromUnderlying(number));
        if (windowRide == nullptr)
            return std::nullopt;

        int32_t viewSelectionIndex = ride.view - 1 - windowRide->num_vehicles;
        if (viewSelectionIndex < 0)
        {
            return std::nullopt;
        }

        for (const auto& station : windowRide->GetStations())
        {
            if (!station.Start.IsNull() && viewSelectionIndex-- == 0)
            {
                const auto stationIndex = windowRide->GetStationIndex(&station);
                return std::make_optional(stationIndex);
            }
        }
        return std::nullopt;
    }

    /**
     *
     *  rct2: 0x006AF315
     */
    void WindowRideRename()
    {
        auto windowRide = get_ride(rideId);
        if (windowRide != nullptr)
        {
            auto rideName = windowRide->GetName();
            WindowTextInputRawOpen(
                this, WIDX_RENAME, STR_RIDE_ATTRACTION_NAME, STR_ENTER_NEW_NAME_FOR_THIS_RIDE_ATTRACTION, {}, rideName.c_str(),
                32);
        }
    }

    /**
     *
     *  rct2: 0x006AF17E
     */
    void WindowRideMainMouseup(WidgetIndex widgetIndex)
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
            case WIDX_TAB_8:
            case WIDX_TAB_9:
            case WIDX_TAB_10:
                WindowRideSetPage(widgetIndex - WIDX_TAB_1);
                break;
            case WIDX_CONSTRUCTION:
            {
                auto windowRide = get_ride(rideId);
                if (windowRide != nullptr)
                {
                    ride_construct(windowRide);
                    if (window_find_by_number(WindowClass::RideConstruction, windowRide->id.ToUnderlying()) != nullptr)
                    {
                        Close();
                    }
                }
                break;
            }
            case WIDX_RENAME:
                WindowRideRename();
                break;
            case WIDX_DEMOLISH:
                context_open_detail_window(WD_DEMOLISH_RIDE, number);
                break;
            case WIDX_CLOSE_LIGHT:
            case WIDX_SIMULATE_LIGHT:
            case WIDX_TEST_LIGHT:
            case WIDX_OPEN_LIGHT:
            {
                auto windowRide = get_ride(rideId);
                if (windowRide != nullptr)
                {
                    RideStatus status;
                    switch (widgetIndex)
                    {
                        default:
                        case WIDX_CLOSE_LIGHT:
                            status = RideStatus::Closed;
                            break;
                        case WIDX_SIMULATE_LIGHT:
                            status = RideStatus::Simulating;
                            break;
                        case WIDX_TEST_LIGHT:
                            status = RideStatus::Testing;
                            break;
                        case WIDX_OPEN_LIGHT:
                            status = RideStatus::Open;
                            break;
                    }
                    auto gameAction = RideSetStatusAction(windowRide->id, status);
                    GameActions::Execute(&gameAction);
                }
                break;
            }
        }
    }

    /**
     *
     *  rct2: 0x006AF4A2
     */
    void WindowRideMainResize()
    {
        int32_t minHeight = 180;
        if (ThemeGetFlags() & UITHEME_FLAG_USE_LIGHTS_RIDE)
        {
            minHeight += 20 + RCT1_LIGHT_OFFSET;

            auto windowRide = get_ride(rideId);
            if (windowRide != nullptr)
            {
#ifdef __SIMULATE_IN_RIDE_WINDOW__
                if (windowRide->SupportsStatus(RideStatus::Simulating))
                {
                    minHeight += 14;
                }
#endif
                if (windowRide->SupportsStatus(RideStatus::Testing))
                {
                    minHeight += 14;
                }
            }
        }
        if (gCheatsAllowArbitraryRideTypeChanges)
        {
            minHeight += 15;
        }

        flags |= WF_RESIZABLE;
        window_set_resize(*this, 316, minHeight, 500, 450);
        // Unlike with other windows, the focus needs to be recentred so it’s best to just reset it.
        focus = std::nullopt;
        WindowRideInitViewport();
    }

    /**
     *
     *  rct2: 0x006AF825
     */
    void WindowRideShowViewDropdown(rct_widget* widget)
    {
        rct_widget* dropdownWidget = widget - 1;
        auto windowRide = get_ride(rideId);
        if (windowRide == nullptr)
            return;

        int32_t numItems = 1;
        if (!windowRide->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_NO_VEHICLES))
        {
            numItems += windowRide->num_stations;
            numItems += windowRide->num_vehicles;
        }

        WindowDropdownShowTextCustomWidth(
            { windowPos.x + dropdownWidget->left, windowPos.y + dropdownWidget->top }, dropdownWidget->height() + 1, colours[1],
            0, 0, numItems, widget->right - dropdownWidget->left);

        // First item
        gDropdownItems[0].Format = STR_DROPDOWN_MENU_LABEL;
        gDropdownItems[0].Args = STR_OVERALL_VIEW;
        int32_t currentItem = 1;

        const auto& rtd = windowRide->GetRideTypeDescriptor();

        // Vehicles
        int32_t name = GetRideComponentName(rtd.NameConvention.vehicle).number;
        for (int32_t i = 1; i <= windowRide->num_vehicles; i++)
        {
            gDropdownItems[currentItem].Format = STR_DROPDOWN_MENU_LABEL;
            gDropdownItems[currentItem].Args = name | (currentItem << 16);
            currentItem++;
        }

        // Stations
        name = GetRideComponentName(rtd.NameConvention.station).number;
        for (int32_t i = 1; i <= windowRide->num_stations; i++)
        {
            gDropdownItems[currentItem].Format = STR_DROPDOWN_MENU_LABEL;
            gDropdownItems[currentItem].Args = name | (i << 16);
            currentItem++;
        }

        // Set highlighted item
        if (!(windowRide->lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK))
        {
            for (int32_t i = 0; i < windowRide->num_vehicles; i++)
            {
                // The +1 is to skip 'Overall view'
                Dropdown::SetDisabled(i + 1, true);
            }
        }

        // Set checked item
        Dropdown::SetChecked(ride.view, true);
    }

    RideStatus WindowRideGetNextDefaultStatus(const Ride* windowRide)
    {
        switch (windowRide->status)
        {
            default:
            case RideStatus::Closed:
                if ((windowRide->lifecycle_flags & RIDE_LIFECYCLE_CRASHED)
                    || (windowRide->lifecycle_flags & RIDE_LIFECYCLE_HAS_STALLED_VEHICLE))
                {
                    return RideStatus::Closed;
                }
                if (windowRide->SupportsStatus(RideStatus::Testing) && !(windowRide->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
                {
                    return RideStatus::Testing;
                }
                return RideStatus::Open;
            case RideStatus::Simulating:
                return RideStatus::Testing;
            case RideStatus::Testing:
                return (windowRide->lifecycle_flags & RIDE_LIFECYCLE_TESTED) ? RideStatus::Open : RideStatus::Closed;
            case RideStatus::Open:
                return RideStatus::Closed;
        }
    }

    struct RideStatusDropdownInfo
    {
        struct Ride* Ride{};
        RideStatus CurrentStatus{};
        RideStatus DefaultStatus{};

        int32_t NumItems{};
        int32_t CheckedIndex = -1;
        int32_t DefaultIndex = -1;
    };

    void WindowRideSetDropdown(RideStatusDropdownInfo& info, RideStatus status, StringId text)
    {
        if (info.Ride->SupportsStatus(status))
        {
            auto index = info.NumItems;
            gDropdownItems[index].Format = STR_DROPDOWN_MENU_LABEL;
            gDropdownItems[index].Args = text;
            if (info.CurrentStatus == status)
            {
                info.CheckedIndex = index;
            }
            if (info.DefaultStatus == status)
            {
                info.DefaultIndex = index;
            }
            info.NumItems++;
        }
    }

    void WindowRideShowOpenDropdown(rct_widget* widget)
    {
        RideStatusDropdownInfo info;
        info.Ride = get_ride(rideId);
        if (info.Ride == nullptr)
            return;

        info.CurrentStatus = info.Ride->status;
        info.DefaultStatus = WindowRideGetNextDefaultStatus(info.Ride);
        WindowRideSetDropdown(info, RideStatus::Closed, STR_CLOSE_RIDE);
#ifdef __SIMULATE_IN_RIDE_WINDOW__
        window_ride_set_dropdown(info, RideStatus::Simulating, STR_SIMULATE_RIDE);
#endif
        WindowRideSetDropdown(info, RideStatus::Testing, STR_TEST_RIDE);
        WindowRideSetDropdown(info, RideStatus::Open, STR_OPEN_RIDE);
        WindowDropdownShowText(
            { windowPos.x + widget->left, windowPos.y + widget->top }, widget->height() + 1, colours[1], 0, info.NumItems);
        Dropdown::SetChecked(info.CheckedIndex, true);
        gDropdownDefaultIndex = info.DefaultIndex;
    }

    StringId GetRideTypeNameForDropdown(uint8_t rideType)
    {
        switch (rideType)
        {
            case RIDE_TYPE_1D:
                return STR_RIDE_NAME_1D;
            case RIDE_TYPE_1F:
                return STR_RIDE_NAME_1F;
            case RIDE_TYPE_22:
                return STR_RIDE_NAME_22;
            case RIDE_TYPE_50:
                return STR_RIDE_NAME_50;
            case RIDE_TYPE_52:
                return STR_RIDE_NAME_52;
            case RIDE_TYPE_53:
                return STR_RIDE_NAME_53;
            case RIDE_TYPE_54:
                return STR_RIDE_NAME_54;
            case RIDE_TYPE_55:
                return STR_RIDE_NAME_55;
            case RIDE_TYPE_59:
                return STR_RIDE_NAME_59;
            default:
                return GetRideTypeDescriptor(rideType).Naming.Name;
        }
    }

    void PopulateRideTypeDropdown()
    {
        auto& ls = OpenRCT2::GetContext()->GetLocalisationService();
        if (RideDropdownDataLanguage == ls.GetCurrentLanguage())
            return;

        RideDropdownData.clear();

        for (uint8_t i = 0; i < RIDE_TYPE_COUNT; i++)
        {
            auto name = GetRideTypeNameForDropdown(i);
            RideDropdownData.push_back({ i, name, ls.GetString(name) });
        }

        std::sort(RideDropdownData.begin(), RideDropdownData.end(), [](auto& a, auto& b) {
            return String::Compare(a.label_string, b.label_string, true) < 0;
        });

        RideDropdownDataLanguage = ls.GetCurrentLanguage();
    }

    void WindowRideShowRideTypeDropdown(rct_widget* widget)
    {
        auto windowRide = get_ride(rideId);
        if (windowRide == nullptr)
            return;

        PopulateRideTypeDropdown();

        for (size_t i = 0; i < RideDropdownData.size(); i++)
        {
            gDropdownItems[i].Format = STR_DROPDOWN_MENU_LABEL;
            gDropdownItems[i].Args = RideDropdownData[i].label_id;
        }

        rct_widget* dropdownWidget = widget - 1;
        WindowDropdownShowText(
            { windowPos.x + dropdownWidget->left, windowPos.y + dropdownWidget->top }, dropdownWidget->height() + 1, colours[1],
            Dropdown::Flag::StayOpen, RIDE_TYPE_COUNT);

        // Find the current ride type in the ordered list.
        int32_t pos = 0;
        for (int32_t i = 0; i < RIDE_TYPE_COUNT; i++)
        {
            if (RideDropdownData[i].ride_type_id == windowRide->type)
            {
                pos = i;
                break;
            }
        }

        gDropdownHighlightedIndex = pos;
        gDropdownDefaultIndex = pos;
        Dropdown::SetChecked(pos, true);
    }

    void WindowRideShowLocateDropdown(rct_widget* widget)
    {
        auto windowRide = get_ride(rideId);
        if (windowRide == nullptr)
            return;

        gDropdownItems[0].Format = STR_LOCATE_SUBJECT_TIP;
        gDropdownItems[1].Format = STR_FOLLOW_SUBJECT_TIP;

        WindowDropdownShowText(
            { windowPos.x + widget->left, windowPos.y + widget->top }, widget->height() + 1, colours[1], 0, 2);
        gDropdownDefaultIndex = 0;
        if (!windowRide->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_HAS_TRACK) || ride.view == 0
            || ride.view > windowRide->num_vehicles)
        {
            // Disable if we're a flat ride, 'overall view' is selected or a station is selected
            Dropdown::SetDisabled(1, true);
        }
    }

    void WindowRideMainFollowRide()
    {
        auto* windowRide = get_ride(rideId);
        if (windowRide != nullptr)
        {
            if (!(windowRide->window_invalidate_flags & RIDE_INVALIDATE_RIDE_MAIN))
            {
                if (ride.view > 0)
                {
                    if (ride.view <= windowRide->num_vehicles)
                    {
                        Vehicle* vehicle = GetEntity<Vehicle>(windowRide->vehicles[ride.view - 1]);
                        if (vehicle != nullptr)
                        {
                            auto headVehicleSpriteIndex = vehicle->sprite_index;
                            rct_window* w_main = window_get_main();
                            window_follow_sprite(*w_main, headVehicleSpriteIndex);
                        }
                    }
                }
            }
        }
    }

    void PopulateVehicleTypeDropdown(Ride* windowRide, bool forceRefresh = false)
    {
        auto& objManager = GetContext()->GetObjectManager();
        rct_ride_entry* rideEntry = windowRide->GetRideEntry();

        bool selectionShouldBeExpanded;
        int32_t rideTypeIterator, rideTypeIteratorMax;
        if (gCheatsShowVehiclesFromOtherTrackTypes
            && !(
                windowRide->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_FLAT_RIDE) || windowRide->type == RIDE_TYPE_MAZE
                || windowRide->type == RIDE_TYPE_MINI_GOLF))
        {
            selectionShouldBeExpanded = true;
            rideTypeIterator = 0;
            rideTypeIteratorMax = RIDE_TYPE_COUNT - 1;
        }
        else
        {
            selectionShouldBeExpanded = false;
            rideTypeIterator = windowRide->type;
            rideTypeIteratorMax = windowRide->type;
        }

        // Don't repopulate the list if we just did.
        auto& ls = OpenRCT2::GetContext()->GetLocalisationService();
        if (!forceRefresh && VehicleDropdownExpanded == selectionShouldBeExpanded && VehicleDropdownRideType == rideEntry
            && VehicleDropdownDataLanguage == ls.GetCurrentLanguage())
            return;

        VehicleDropdownData.clear();

        for (; rideTypeIterator <= rideTypeIteratorMax; rideTypeIterator++)
        {
            if (selectionShouldBeExpanded && GetRideTypeDescriptor(rideTypeIterator).HasFlag(RIDE_TYPE_FLAG_FLAT_RIDE))
                continue;
            if (selectionShouldBeExpanded && (rideTypeIterator == RIDE_TYPE_MAZE || rideTypeIterator == RIDE_TYPE_MINI_GOLF))
                continue;

            auto& rideEntries = objManager.GetAllRideEntries(rideTypeIterator);
            for (auto rideEntryIndex : rideEntries)
            {
                const auto* currentRideEntry = get_ride_entry(rideEntryIndex);
                if (currentRideEntry == nullptr)
                    continue;

                // Skip if vehicle type has not been invented yet
                if (!ride_entry_is_invented(rideEntryIndex) && !gCheatsIgnoreResearchStatus)
                    continue;

                VehicleDropdownData.push_back(
                    { rideEntryIndex, currentRideEntry->naming.Name, ls.GetString(currentRideEntry->naming.Name) });
            }
        }

        std::sort(VehicleDropdownData.begin(), VehicleDropdownData.end(), [](auto& a, auto& b) {
            return String::Compare(a.label_string, b.label_string, true) < 0;
        });

        VehicleDropdownExpanded = selectionShouldBeExpanded;
        VehicleDropdownRideType = rideEntry;
        VehicleDropdownDataLanguage = ls.GetCurrentLanguage();
    }

    void WindowRideShowVehicleTypeDropdown(rct_widget* widget)
    {
        auto windowRide = get_ride(rideId);
        if (windowRide == nullptr)
            return;

        PopulateVehicleTypeDropdown(windowRide);

        size_t numItems = std::min<size_t>(VehicleDropdownData.size(), Dropdown::ItemsMaxSize);

        for (size_t i = 0; i < numItems; i++)
        {
            gDropdownItems[i].Format = STR_DROPDOWN_MENU_LABEL;
            gDropdownItems[i].Args = VehicleDropdownData[i].label_id;
        }

        rct_widget* dropdownWidget = widget - 1;
        WindowDropdownShowTextCustomWidth(
            { windowPos.x + dropdownWidget->left, windowPos.y + dropdownWidget->top }, dropdownWidget->height() + 1, colours[1],
            0, Dropdown::Flag::StayOpen, numItems, widget->right - dropdownWidget->left);

        // Find the current vehicle type in the ordered list.
        int32_t pos = 0;
        for (int32_t i = 0; i < static_cast<int32_t>(VehicleDropdownData.size()); i++)
        {
            if (VehicleDropdownData[i].subtype_id == windowRide->subtype)
            {
                pos = i;
                break;
            }
        }

        gDropdownHighlightedIndex = pos;
        gDropdownDefaultIndex = pos;
        Dropdown::SetChecked(pos, true);
    }

    /**
     *
     *  rct2: 0x006AF1BD
     */
    void WindowRideMainMousedown(WidgetIndex widgetIndex, rct_widget* widget)
    {
        switch (widgetIndex)
        {
            case WIDX_VIEW_DROPDOWN:
                WindowRideShowViewDropdown(widget);
                break;
            case WIDX_OPEN:
                WindowRideShowOpenDropdown(widget);
                break;
            case WIDX_RIDE_TYPE_DROPDOWN:
                WindowRideShowRideTypeDropdown(widget);
                break;
            case WIDX_LOCATE:
                WindowRideShowLocateDropdown(widget);
                break;
        }
    }

    /**
     *
     *  rct2: 0x006AF300
     */
    void WindowRideMainDropdown(WidgetIndex widgetIndex, int32_t dropdownIndex)
    {
        switch (widgetIndex)
        {
            case WIDX_VIEW_DROPDOWN:
                if (dropdownIndex == -1)
                {
                    dropdownIndex = ride.view + 1;
                    auto windowRide = get_ride(rideId);
                    if (windowRide != nullptr)
                    {
                        if (dropdownIndex != 0 && dropdownIndex <= windowRide->num_vehicles
                            && !(windowRide->lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK))
                        {
                            dropdownIndex = windowRide->num_vehicles + 1;
                        }
                        if (dropdownIndex >= gDropdownNumItems)
                        {
                            dropdownIndex = 0;
                        }
                    }
                }

                ride.view = dropdownIndex;
                WindowRideInitViewport();
                Invalidate();
                break;
            case WIDX_OPEN:
            {
                auto windowRide = get_ride(rideId);
                if (windowRide != nullptr)
                {
                    auto status = RideStatus::Closed;
                    if (dropdownIndex < 0)
                    {
                        dropdownIndex = gDropdownHighlightedIndex;
                    }
                    if (dropdownIndex < static_cast<int32_t>(std::size(gDropdownItems)))
                    {
                        switch (gDropdownItems[dropdownIndex].Args)
                        {
                            case STR_CLOSE_RIDE:
                                status = RideStatus::Closed;
                                break;
                            case STR_SIMULATE_RIDE:
                                status = RideStatus::Simulating;
                                break;
                            case STR_TEST_RIDE:
                                status = RideStatus::Testing;
                                break;
                            case STR_OPEN_RIDE:
                                status = RideStatus::Open;
                                break;
                        }
                    }
                    auto gameAction = RideSetStatusAction(windowRide->id, status);
                    GameActions::Execute(&gameAction);
                }
                break;
            }
            case WIDX_RIDE_TYPE_DROPDOWN:
                if (dropdownIndex != -1 && dropdownIndex < RIDE_TYPE_COUNT)
                {
                    auto rideLabelId = std::clamp(dropdownIndex, 0, RIDE_TYPE_COUNT - 1);
                    auto rideType = RideDropdownData[rideLabelId].ride_type_id;
                    if (rideType < RIDE_TYPE_COUNT)
                    {
                        auto rideSetSetting = RideSetSettingAction(rideId, RideSetSetting::RideType, rideType);
                        rideSetSetting.SetCallback([](const GameAction* ga, const GameActions::Result* result) {
                            // Reset ghost track if ride construction window is open, prevents a crash
                            // Will get set to the correct Alternative variable during set_default_next_piece.
                            // TODO: Rework construction window to prevent the need for this.
                            _currentTrackAlternative = RIDE_TYPE_NO_ALTERNATIVES;
                            ride_construction_set_default_next_piece();
                        });
                        GameActions::Execute(&rideSetSetting);
                    }
                }
                break;
            case WIDX_LOCATE:
            {
                if (dropdownIndex == 0)
                {
                    ScrollToViewport();
                }
                else if (dropdownIndex == 1)
                {
                    WindowRideMainFollowRide();
                }
                break;
            }
        }
    }

    /**
     *
     *  rct2: 0x006AF40F
     */
    void WindowRideMainUpdate()
    {
        // Update tab animation
        frame_no++;
        window_event_invalidate_call(this);
        widget_invalidate(*this, WIDX_TAB_1);

        // Update status
        auto windowRide = get_ride(rideId);
        if (windowRide != nullptr)
        {
            if (!(windowRide->window_invalidate_flags & RIDE_INVALIDATE_RIDE_MAIN))
            {
                if (ride.view == 0)
                    return;

                if (ride.view <= windowRide->num_vehicles)
                {
                    Vehicle* vehicle = GetEntity<Vehicle>(windowRide->vehicles[ride.view - 1]);
                    if (vehicle == nullptr
                        || (vehicle->status != Vehicle::Status::Travelling
                            && vehicle->status != Vehicle::Status::TravellingCableLift
                            && vehicle->status != Vehicle::Status::TravellingDodgems
                            && vehicle->status != Vehicle::Status::TravellingBoat))
                    {
                        return;
                    }
                }
            }
            windowRide->window_invalidate_flags &= ~RIDE_INVALIDATE_RIDE_MAIN;
        }
        widget_invalidate(*this, WIDX_STATUS);
    }

    /**
     *
     *  rct2: 0x006AF2F9
     */
    void WindowRideMainTextinput(WidgetIndex widgetIndex, std::string_view text)
    {
        if (widgetIndex != WIDX_RENAME || text.empty())
            return;

        auto windowRide = get_ride(rideId);
        if (windowRide != nullptr)
        {
            auto gameAction = RideSetNameAction(windowRide->id, std::string(text));
            GameActions::Execute(&gameAction);
        }
    }

    /**
     *
     *  rct2: 0x006AECF6
     */
    void WindowRideMainInvalidate()
    {
        rct_widget* newWidgets;
        int32_t i, newHeight;

        newWidgets = window_ride_page_widgets[page];
        if (widgets != newWidgets)
        {
            widgets = newWidgets;
            WindowInitScrollWidgets(*this);
        }

        WindowRideSetPressedTab();

        auto windowRide = get_ride(rideId);
        if (windowRide == nullptr)
            return;

        disabled_widgets &= ~((1ULL << WIDX_DEMOLISH) | (1ULL << WIDX_CONSTRUCTION));
        if (windowRide->lifecycle_flags & (RIDE_LIFECYCLE_INDESTRUCTIBLE | RIDE_LIFECYCLE_INDESTRUCTIBLE_TRACK))
            disabled_widgets |= (1ULL << WIDX_DEMOLISH);

        auto ft = Formatter::Common();
        windowRide->FormatNameTo(ft);

        uint32_t spriteIds[] = {
            SPR_CLOSED,
            SPR_OPEN,
            SPR_TESTING,
            SPR_G2_SIMULATE,
        };
        window_ride_main_widgets[WIDX_OPEN].image = spriteIds[EnumValue(windowRide->status)];

#ifdef __SIMULATE_IN_RIDE_WINDOW__
        window_ride_main_widgets[WIDX_CLOSE_LIGHT].image = SPR_G2_RCT1_CLOSE_BUTTON_0
            + (windowRide->status == RideStatus::Closed) * 2 + WidgetIsPressed(*this, WIDX_CLOSE_LIGHT);
        window_ride_main_widgets[WIDX_SIMULATE_LIGHT].image = SPR_G2_RCT1_SIMULATE_BUTTON_0
            + (windowRide->status == RideStatus::Simulating) * 2 + WidgetIsPressed(*this, WIDX_SIMULATE_LIGHT);
        window_ride_main_widgets[WIDX_TEST_LIGHT].image = SPR_G2_RCT1_TEST_BUTTON_0
            + (windowRide->status == RideStatus::Testing) * 2 + WidgetIsPressed(*this, WIDX_TEST_LIGHT);
#else
        window_ride_main_widgets[WIDX_CLOSE_LIGHT].image = SPR_G2_RCT1_CLOSE_BUTTON_0
            + (windowRide->status == RideStatus::Closed) * 2 + WidgetIsPressed(*this, WIDX_CLOSE_LIGHT);

        auto baseSprite = windowRide->status == RideStatus::Simulating ? SPR_G2_RCT1_SIMULATE_BUTTON_0
                                                                       : SPR_G2_RCT1_TEST_BUTTON_0;
        window_ride_main_widgets[WIDX_TEST_LIGHT].image = baseSprite
            + (windowRide->status == RideStatus::Testing || windowRide->status == RideStatus::Simulating) * 2
            + WidgetIsPressed(*this, WIDX_TEST_LIGHT);
#endif
        window_ride_main_widgets[WIDX_OPEN_LIGHT].image = SPR_G2_RCT1_OPEN_BUTTON_0
            + (windowRide->status == RideStatus::Open) * 2 + WidgetIsPressed(*this, WIDX_OPEN_LIGHT);

        WindowRideAnchorBorderWidgets();

        const int32_t offset = gCheatsAllowArbitraryRideTypeChanges ? 15 : 0;
        // Anchor main page specific widgets
        window_ride_main_widgets[WIDX_VIEWPORT].right = width - 26;
        window_ride_main_widgets[WIDX_VIEWPORT].bottom = height - (14 + offset);
        window_ride_main_widgets[WIDX_STATUS].right = width - 26;
        window_ride_main_widgets[WIDX_STATUS].top = height - (13 + offset);
        window_ride_main_widgets[WIDX_STATUS].bottom = height - (3 + offset);
        window_ride_main_widgets[WIDX_VIEW].right = width - 60;
        window_ride_main_widgets[WIDX_VIEW_DROPDOWN].right = width - 61;
        window_ride_main_widgets[WIDX_VIEW_DROPDOWN].left = width - 71;
        window_ride_main_widgets[WIDX_RIDE_TYPE].right = width - 26;
        window_ride_main_widgets[WIDX_RIDE_TYPE].top = height - 17;
        window_ride_main_widgets[WIDX_RIDE_TYPE].bottom = height - 4;
        window_ride_main_widgets[WIDX_RIDE_TYPE_DROPDOWN].left = width - 37;
        window_ride_main_widgets[WIDX_RIDE_TYPE_DROPDOWN].right = width - 27;
        window_ride_main_widgets[WIDX_RIDE_TYPE_DROPDOWN].top = height - 16;
        window_ride_main_widgets[WIDX_RIDE_TYPE_DROPDOWN].bottom = height - 5;

        if (!gCheatsAllowArbitraryRideTypeChanges)
        {
            window_ride_main_widgets[WIDX_RIDE_TYPE].type = WindowWidgetType::Empty;
            window_ride_main_widgets[WIDX_RIDE_TYPE_DROPDOWN].type = WindowWidgetType::Empty;
        }
        else
        {
            window_ride_main_widgets[WIDX_RIDE_TYPE].type = WindowWidgetType::DropdownMenu;
            window_ride_main_widgets[WIDX_RIDE_TYPE].text = windowRide->GetRideTypeDescriptor().Naming.Name;
            window_ride_main_widgets[WIDX_RIDE_TYPE_DROPDOWN].type = WindowWidgetType::Button;
        }

        window_align_tabs(this, WIDX_TAB_1, WIDX_TAB_10);

        if (ThemeGetFlags() & UITHEME_FLAG_USE_LIGHTS_RIDE)
        {
            window_ride_main_widgets[WIDX_OPEN].type = WindowWidgetType::Empty;
            window_ride_main_widgets[WIDX_CLOSE_LIGHT].type = WindowWidgetType::ImgBtn;
            window_ride_main_widgets[WIDX_SIMULATE_LIGHT].type = WindowWidgetType::Empty;
#ifdef __SIMULATE_IN_RIDE_WINDOW__
            if (windowRide->SupportsStatus(RideStatus::Simulating))
                window_ride_main_widgets[WIDX_SIMULATE_LIGHT].type = WindowWidgetType::ImgBtn;
#endif
            window_ride_main_widgets[WIDX_TEST_LIGHT].type = windowRide->SupportsStatus(RideStatus::Testing)
                ? WindowWidgetType::ImgBtn
                : WindowWidgetType::Empty;
            window_ride_main_widgets[WIDX_OPEN_LIGHT].type = WindowWidgetType::ImgBtn;

            newHeight = 62;
            if (window_ride_main_widgets[WIDX_SIMULATE_LIGHT].type != WindowWidgetType::Empty)
            {
                window_ride_main_widgets[WIDX_SIMULATE_LIGHT].top = newHeight;
                window_ride_main_widgets[WIDX_SIMULATE_LIGHT].bottom = newHeight + 13;
                newHeight += 14;
            }
            if (window_ride_main_widgets[WIDX_TEST_LIGHT].type != WindowWidgetType::Empty)
            {
                window_ride_main_widgets[WIDX_TEST_LIGHT].top = newHeight;
                window_ride_main_widgets[WIDX_TEST_LIGHT].bottom = newHeight + 13;
                newHeight += 14;
            }
            window_ride_main_widgets[WIDX_OPEN_LIGHT].top = newHeight;
            window_ride_main_widgets[WIDX_OPEN_LIGHT].bottom = newHeight + 13;
            newHeight += 14 - 24 + RCT1_LIGHT_OFFSET;
        }
        else
        {
            window_ride_main_widgets[WIDX_OPEN].type = WindowWidgetType::FlatBtn;
            window_ride_main_widgets[WIDX_CLOSE_LIGHT].type = WindowWidgetType::Empty;
            window_ride_main_widgets[WIDX_SIMULATE_LIGHT].type = WindowWidgetType::Empty;
            window_ride_main_widgets[WIDX_TEST_LIGHT].type = WindowWidgetType::Empty;
            window_ride_main_widgets[WIDX_OPEN_LIGHT].type = WindowWidgetType::Empty;
            newHeight = 46;
        }
        for (i = WIDX_CLOSE_LIGHT; i <= WIDX_OPEN_LIGHT; i++)
        {
            window_ride_main_widgets[i].left = width - 20;
            window_ride_main_widgets[i].right = width - 7;
        }
        for (i = WIDX_OPEN; i <= WIDX_DEMOLISH; i++, newHeight += 24)
        {
            window_ride_main_widgets[i].left = width - 25;
            window_ride_main_widgets[i].right = width - 2;
            window_ride_main_widgets[i].top = newHeight;
            window_ride_main_widgets[i].bottom = newHeight + 23;
        }
    }

    /**
     *
     *  rct2: 0x006AF10A
     */
    StringId WindowRideGetStatusOverallView(Formatter& ft)
    {
        auto stringId = STR_NONE;
        auto windowRide = get_ride(rideId);
        if (windowRide != nullptr)
        {
            windowRide->FormatStatusTo(ft);
            stringId = STR_BLACK_STRING;
            if ((windowRide->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN)
                || (windowRide->lifecycle_flags & RIDE_LIFECYCLE_CRASHED))
            {
                stringId = STR_RED_OUTLINED_STRING;
            }
        }
        return stringId;
    }

    /**
     *
     *  rct2: 0x006AEFEF
     */
    StringId WindowRideGetStatusVehicle(Formatter& ft)
    {
        auto windowRide = get_ride(rideId);
        if (windowRide == nullptr)
            return STR_EMPTY;

        auto vehicle = GetEntity<Vehicle>(windowRide->vehicles[ride.view - 1]);
        if (vehicle == nullptr)
            return STR_EMPTY;

        if (vehicle->status != Vehicle::Status::Crashing && vehicle->status != Vehicle::Status::Crashed)
        {
            auto trackType = vehicle->GetTrackType();
            if (trackType == TrackElemType::BlockBrakes || trackType == TrackElemType::CableLiftHill
                || trackType == TrackElemType::Up25ToFlat || trackType == TrackElemType::Up60ToFlat
                || trackType == TrackElemType::DiagUp25ToFlat || trackType == TrackElemType::DiagUp60ToFlat)
            {
                if (windowRide->GetRideTypeDescriptor().SupportsTrackPiece(TRACK_BLOCK_BRAKES) && vehicle->velocity == 0)
                {
                    ft.Add<StringId>(STR_STOPPED_BY_BLOCK_BRAKES);
                    return STR_BLACK_STRING;
                }
            }
        }

        if (windowRide->type == RIDE_TYPE_MINI_GOLF)
            return STR_EMPTY;

        auto stringId = VehicleStatusNames[static_cast<size_t>(vehicle->status)];
        if (windowRide->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_SINGLE_SESSION)
            && vehicle->status <= Vehicle::Status::UnloadingPassengers)
        {
            stringId = SingleSessionVehicleStatusNames[static_cast<size_t>(vehicle->status)];
        }

        ft.Add<StringId>(stringId);
        uint16_t speedInMph = (abs(vehicle->velocity) * 9) >> 18;
        ft.Add<uint16_t>(speedInMph);
        const RideComponentName stationName = GetRideComponentName(windowRide->GetRideTypeDescriptor().NameConvention.station);
        ft.Add<StringId>(windowRide->num_stations > 1 ? stationName.number : stationName.singular);
        ft.Add<uint16_t>(vehicle->current_station.ToUnderlying() + 1);
        return stringId != STR_CRASHING && stringId != STR_CRASHED_0 ? STR_BLACK_STRING : STR_RED_OUTLINED_STRING;
    }

    /**
     *
     *  rct2: 0x006AEF65
     */
    StringId WindowRideGetStatusStation(Formatter& ft)
    {
        auto windowRide = get_ride(rideId);
        if (windowRide == nullptr)
            return STR_NONE;

        const auto stationIndex = GetStationIndexFromViewSelection();
        if (!stationIndex)
        {
            return STR_NONE;
        }

        const auto& station = windowRide->GetStation(*stationIndex);
        StringId stringId = STR_EMPTY;
        // Entrance / exit
        if (windowRide->status == RideStatus::Closed)
        {
            if (station.Entrance.IsNull())
                stringId = STR_NO_ENTRANCE;
            else if (station.Exit.IsNull())
                stringId = STR_NO_EXIT;
        }
        else
        {
            if (station.Entrance.IsNull())
                stringId = STR_EXIT_ONLY;
        }
        // Queue length
        if (stringId == STR_EMPTY)
        {
            stringId = STR_QUEUE_EMPTY;
            uint16_t queueLength = windowRide->GetStation(*stationIndex).QueueLength;
            if (queueLength == 1)
                stringId = STR_QUEUE_ONE_PERSON;
            else if (queueLength > 1)
                stringId = STR_QUEUE_PEOPLE;

            ft.Add<StringId>(stringId);
            ft.Add<uint16_t>(queueLength);
        }
        else
        {
            ft.Add<StringId>(stringId);
        }

        return STR_BLACK_STRING;
    }

    /**
     *
     *  rct2: 0x006AEE73
     */
    StringId WindowRideGetStatus(Formatter& ft)
    {
        auto windowRide = get_ride(rideId);
        if (ride.view == 0)
            return WindowRideGetStatusOverallView(ft);
        if (windowRide != nullptr && ride.view <= windowRide->num_vehicles)
            return WindowRideGetStatusVehicle(ft);
        if (windowRide != nullptr && windowRide->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN)
            return WindowRideGetStatusOverallView(ft);
        return WindowRideGetStatusStation(ft);
    }

    /**
     *
     *  rct2: 0x006AEE73
     */
    void WindowRideMainPaint(rct_drawpixelinfo* dpi)
    {
        rct_widget* widget;

        WindowDrawWidgets(*this, dpi);
        WindowRideDrawTabImages(dpi);

        // Viewport and ear icon
        if (viewport != nullptr)
        {
            window_draw_viewport(dpi, *this);
            if (viewport->flags & VIEWPORT_FLAG_SOUND_ON)
                gfx_draw_sprite(dpi, ImageId(SPR_HEARING_VIEWPORT), windowPos + ScreenCoordsXY{ 2, 2 });
        }

        // View dropdown
        auto windowRide = get_ride(rideId);
        if (windowRide == nullptr)
            return;

        auto ft = Formatter();
        if (ride.view != 0)
        {
            if (ride.view > windowRide->num_vehicles)
            {
                ft.Add<StringId>(GetRideComponentName(windowRide->GetRideTypeDescriptor().NameConvention.station).number);
                ft.Add<uint16_t>(ride.view - windowRide->num_vehicles);
            }
            else
            {
                ft.Add<StringId>(GetRideComponentName(windowRide->GetRideTypeDescriptor().NameConvention.vehicle).number);
                ft.Add<uint16_t>(ride.view);
            }
        }
        else
        {
            ft.Add<StringId>(STR_OVERALL_VIEW);
        }

        widget = &window_ride_main_widgets[WIDX_VIEW];
        DrawTextBasic(
            dpi, { windowPos.x + (widget->left + widget->right - 11) / 2, windowPos.y + widget->top },
            STR_WINDOW_COLOUR_2_STRINGID, ft, { TextAlignment::CENTRE });

        // Status
        ft = Formatter();
        widget = &window_ride_main_widgets[WIDX_STATUS];
        StringId rideStatus = WindowRideGetStatus(ft);
        DrawTextEllipsised(
            dpi, windowPos + ScreenCoordsXY{ (widget->left + widget->right) / 2, widget->top }, widget->width(), rideStatus, ft,
            { TextAlignment::CENTRE });
    }

    void WindowRideUpdateOverallView(Ride* windowRide)
    {
        // Calculate x, y, z bounds of the entire ride using its track elements
        tile_element_iterator it;

        tile_element_iterator_begin(&it);

        CoordsXYZ min = { std::numeric_limits<int32_t>::max(), std::numeric_limits<int32_t>::max(),
                          std::numeric_limits<int32_t>::max() };
        CoordsXYZ max = { std::numeric_limits<int32_t>::min(), std::numeric_limits<int32_t>::min(),
                          std::numeric_limits<int32_t>::min() };

        while (tile_element_iterator_next(&it))
        {
            if (it.element->GetType() != TileElementType::Track)
                continue;

            if (it.element->AsTrack()->GetRideIndex() != windowRide->id)
                continue;

            auto location = TileCoordsXY(it.x, it.y).ToCoordsXY();
            int32_t baseZ = it.element->GetBaseZ();
            int32_t clearZ = it.element->GetClearanceZ();

            min.x = std::min(min.x, location.x);
            min.y = std::min(min.y, location.y);
            min.z = std::min(min.z, baseZ);

            max.x = std::max(max.x, location.x);
            max.y = std::max(max.y, location.y);
            max.z = std::max(max.z, clearZ);
        }

        const auto rideIndex = windowRide->id.ToUnderlying();
        if (rideIndex >= ride_overall_views.size())
        {
            ride_overall_views.resize(rideIndex + 1);
        }

        auto& view = ride_overall_views[rideIndex];
        view.loc = CoordsXYZ{ (min.x + max.x) / 2, (min.y + max.y) / 2, (min.z + max.z) / 2 } + CoordsXYZ{ 16, 16, -8 };

        // Calculate size to determine from how far away to view the ride
        const auto diff = max - min;

        const int32_t size = static_cast<int32_t>(std::sqrt(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z));

        if (size >= 80)
        {
            // Each farther zoom level shows twice as many tiles (log)
            // Appropriate zoom is lowered by one to fill the entire view with the ride
            const auto zoomValue = static_cast<int8_t>(std::ceil(std::log(size / 80)) - 1);
            view.zoom = std::clamp(ZoomLevel{ zoomValue }, ZoomLevel{ 0 }, ZoomLevel::max());
        }
        else
        {
            // Small rides or stalls are zoomed in all the way.
            view.zoom = ZoomLevel{ 0 };
        }
    }

#pragma endregion

#pragma region Vehicle

    /**
     *
     *  rct2: 0x006B272D
     */
    void WindowRideVehicleMouseup(WidgetIndex widgetIndex)
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
            case WIDX_TAB_8:
            case WIDX_TAB_9:
            case WIDX_TAB_10:
                WindowRideSetPage(widgetIndex - WIDX_TAB_1);
                break;
        }
    }

    /**
     *
     *  rct2: 0x006B2ABB
     */
    void WindowRideVehicleResize()
    {
        window_set_resize(*this, 316, 214, 316, 214);
    }

    /**
     *
     *  rct2: 0x006B2748
     */
    void WindowRideVehicleMousedown(WidgetIndex widgetIndex, rct_widget* widget)
    {
        auto windowRide = get_ride(rideId);
        if (windowRide == nullptr)
            return;

        switch (widgetIndex)
        {
            case WIDX_VEHICLE_TYPE_DROPDOWN:
                WindowRideShowVehicleTypeDropdown(&widgets[widgetIndex]);
                break;
            case WIDX_VEHICLE_TRAINS_INCREASE:
                if (windowRide->num_vehicles < OpenRCT2::Limits::MaxTrainsPerRide)
                    windowRide->SetNumVehicles(windowRide->num_vehicles + 1);
                break;
            case WIDX_VEHICLE_TRAINS_DECREASE:
                if (windowRide->num_vehicles > 1)
                    windowRide->SetNumVehicles(windowRide->num_vehicles - 1);
                break;
            case WIDX_VEHICLE_CARS_PER_TRAIN_INCREASE:
                if (windowRide->num_cars_per_train < OpenRCT2::Limits::MaxCarsPerTrain)
                    windowRide->SetNumCarsPerVehicle(windowRide->num_cars_per_train + 1);
                break;
            case WIDX_VEHICLE_CARS_PER_TRAIN_DECREASE:
                rct_ride_entry* rideEntry = windowRide->GetRideEntry();
                if (windowRide->num_cars_per_train > rideEntry->zero_cars + 1)
                    windowRide->SetNumCarsPerVehicle(windowRide->num_cars_per_train - 1);
                break;
        }
    }

    /**
     *
     *  rct2: 0x006B2767
     */
    void WindowRideVehicleDropdown(WidgetIndex widgetIndex, int32_t dropdownIndex)
    {
        if (dropdownIndex == -1)
            return;

        switch (widgetIndex)
        {
            case WIDX_VEHICLE_TYPE_DROPDOWN:
                if (dropdownIndex >= 0 && static_cast<std::size_t>(dropdownIndex) < VehicleDropdownData.size())
                {
                    auto windowRide = get_ride(rideId);
                    if (windowRide != nullptr)
                    {
                        auto newRideType = VehicleDropdownData[dropdownIndex].subtype_id;
                        windowRide->SetRideEntry(newRideType);
                    }
                }
                break;
        }
    }

    /**
     *
     *  rct2: 0x006B2AA1
     */
    void WindowRideVehicleUpdate()
    {
        frame_no++;
        window_event_invalidate_call(this);
        widget_invalidate(*this, WIDX_TAB_2);
    }

    OpenRCT2String WindowRideVehicleTooltip(const WidgetIndex widgetIndex, StringId fallback)
    {
        auto windowRide = get_ride(rideId);
        if (windowRide == nullptr)
            return { STR_NONE, {} };

        switch (widgetIndex)
        {
            case WIDX_VEHICLE_TRAINS:
            case WIDX_VEHICLE_TRAINS_DECREASE:
            case WIDX_VEHICLE_TRAINS_INCREASE:
            {
                auto ft = Formatter();
                ft.Increment(12);

                RideComponentType vehicleType = windowRide->GetRideTypeDescriptor().NameConvention.vehicle;
                StringId stringId = GetRideComponentName(vehicleType).count;
                if (windowRide->max_trains > 1)
                {
                    stringId = GetRideComponentName(vehicleType).count_plural;
                }
                ft.Add<StringId>(stringId);
                ft.Add<uint16_t>(windowRide->max_trains);
                return { fallback, ft };
            }
            case WIDX_VEHICLE_CARS_PER_TRAIN:
            case WIDX_VEHICLE_CARS_PER_TRAIN_DECREASE:
            case WIDX_VEHICLE_CARS_PER_TRAIN_INCREASE:
            {
                auto rideEntry = windowRide->GetRideEntry();
                if (rideEntry == nullptr)
                    return { STR_NONE, {} };

                auto ft = Formatter();
                ft.Increment(16);
                ft.Add<uint16_t>(std::max(uint8_t(1), windowRide->MaxCarsPerTrain) - rideEntry->zero_cars);

                StringId stringId = GetRideComponentName(RideComponentType::Car).singular;
                if (windowRide->MaxCarsPerTrain - rideEntry->zero_cars > 1)
                {
                    stringId = GetRideComponentName(RideComponentType::Car).plural;
                }
                ft.Add<StringId>(stringId);
                return { fallback, ft };
            }
        }
        return { fallback, {} };
    }

    /**
     *
     *  rct2: 0x006B222C
     */
    void WindowRideVehicleInvalidate()
    {
        rct_widget* newWidgets;
        rct_ride_entry* rideEntry;
        StringId stringId;
        int32_t carsPerTrain;

        newWidgets = window_ride_page_widgets[page];
        if (widgets != newWidgets)
        {
            widgets = newWidgets;
            WindowInitScrollWidgets(*this);
        }

        WindowRideSetPressedTab();

        auto windowRide = get_ride(rideId);
        if (windowRide == nullptr)
            return;

        rideEntry = windowRide->GetRideEntry();

        widgets[WIDX_TITLE].text = STR_ARG_20_STRINGID;

        // Widget setup
        carsPerTrain = windowRide->num_cars_per_train - rideEntry->zero_cars;

        // Vehicle type
        window_ride_vehicle_widgets[WIDX_VEHICLE_TYPE].text = rideEntry->naming.Name;

        // Trains
        if (rideEntry->cars_per_flat_ride > 1 || gCheatsDisableTrainLengthLimit)
        {
            window_ride_vehicle_widgets[WIDX_VEHICLE_TRAINS].type = WindowWidgetType::Spinner;
            window_ride_vehicle_widgets[WIDX_VEHICLE_TRAINS_INCREASE].type = WindowWidgetType::Button;
            window_ride_vehicle_widgets[WIDX_VEHICLE_TRAINS_DECREASE].type = WindowWidgetType::Button;
        }
        else
        {
            window_ride_vehicle_widgets[WIDX_VEHICLE_TRAINS].type = WindowWidgetType::Empty;
            window_ride_vehicle_widgets[WIDX_VEHICLE_TRAINS_INCREASE].type = WindowWidgetType::Empty;
            window_ride_vehicle_widgets[WIDX_VEHICLE_TRAINS_DECREASE].type = WindowWidgetType::Empty;
        }

        // Cars per train
        if (rideEntry->zero_cars + 1 < rideEntry->max_cars_in_train || gCheatsDisableTrainLengthLimit)
        {
            window_ride_vehicle_widgets[WIDX_VEHICLE_CARS_PER_TRAIN].type = WindowWidgetType::Spinner;
            window_ride_vehicle_widgets[WIDX_VEHICLE_CARS_PER_TRAIN_INCREASE].type = WindowWidgetType::Button;
            window_ride_vehicle_widgets[WIDX_VEHICLE_CARS_PER_TRAIN_DECREASE].type = WindowWidgetType::Button;
        }
        else
        {
            window_ride_vehicle_widgets[WIDX_VEHICLE_CARS_PER_TRAIN].type = WindowWidgetType::Empty;
            window_ride_vehicle_widgets[WIDX_VEHICLE_CARS_PER_TRAIN_INCREASE].type = WindowWidgetType::Empty;
            window_ride_vehicle_widgets[WIDX_VEHICLE_CARS_PER_TRAIN_DECREASE].type = WindowWidgetType::Empty;
        }

        auto ft = Formatter::Common();
        ft.Increment(6);
        ft.Add<uint16_t>(carsPerTrain);
        RideComponentType vehicleType = windowRide->GetRideTypeDescriptor().NameConvention.vehicle;
        stringId = GetRideComponentName(vehicleType).count;
        if (windowRide->num_vehicles > 1)
        {
            stringId = GetRideComponentName(vehicleType).count_plural;
        }
        ft.Add<StringId>(stringId);
        ft.Add<uint16_t>(windowRide->num_vehicles);

        ft.Increment(8);

        windowRide->FormatNameTo(ft);

        WindowRideAnchorBorderWidgets();
        window_align_tabs(this, WIDX_TAB_1, WIDX_TAB_10);

        if (windowRide->num_cars_per_train > (rideEntry->zero_cars + 1))
        {
            window_ride_vehicle_widgets[WIDX_VEHICLE_CARS_PER_TRAIN].text = STR_X_CARS_PER_TRAIN;
        }
        else
        {
            window_ride_vehicle_widgets[WIDX_VEHICLE_CARS_PER_TRAIN].text = STR_1_CAR_PER_TRAIN;
        }
    }

    /**
     *
     *  rct2: 0x006B23DC
     */
    void WindowRideVehiclePaint(rct_drawpixelinfo* dpi)
    {
        WindowDrawWidgets(*this, dpi);
        WindowRideDrawTabImages(dpi);

        auto windowRide = get_ride(rideId);
        if (windowRide == nullptr)
            return;

        auto rideEntry = windowRide->GetRideEntry();
        if (rideEntry == nullptr)
            return;

        auto screenCoords = windowPos + ScreenCoordsXY{ 8, 64 };

        // Description
        auto ft = Formatter();
        ft.Add<StringId>(rideEntry->naming.Description);
        screenCoords.y += DrawTextWrapped(dpi, screenCoords, 300, STR_BLACK_STRING, ft, { TextAlignment::LEFT });
        screenCoords.y += 2;

        // Capacity
        ft = Formatter();
        ft.Add<StringId>(rideEntry->capacity);
        DrawTextBasic(dpi, screenCoords, STR_CAPACITY, ft);

        // Excitement Factor
        if (rideEntry->excitement_multiplier > 0)
        {
            screenCoords.y += LIST_ROW_HEIGHT;

            ft = Formatter();
            ft.Add<int16_t>(rideEntry->excitement_multiplier);
            DrawTextBasic(dpi, screenCoords, STR_EXCITEMENT_FACTOR, ft);
        }

        // Intensity Factor
        if (rideEntry->intensity_multiplier > 0)
        {
            int32_t lineHeight = font_get_line_height(FontSpriteBase::MEDIUM);
            if (lineHeight != 10)
                screenCoords.x += 150;
            else
                screenCoords.y += LIST_ROW_HEIGHT;

            ft = Formatter();
            ft.Add<int16_t>(rideEntry->intensity_multiplier);
            DrawTextBasic(dpi, screenCoords, STR_INTENSITY_FACTOR, ft);

            if (lineHeight != 10)
                screenCoords.x -= 150;
        }

        // Nausea Factor
        if (rideEntry->nausea_multiplier > 0)
        {
            screenCoords.y += LIST_ROW_HEIGHT;

            ft = Formatter();
            ft.Add<int16_t>(rideEntry->nausea_multiplier);
            DrawTextBasic(dpi, screenCoords, STR_NAUSEA_FACTOR, ft);
        }
    }

    struct VehicleDrawInfo
    {
        int16_t x;
        int16_t y;
        ImageId imageId;
    };

    /**
     *
     *  rct2: 0x006B2502
     */
    void WindowRideVehicleScrollpaint(rct_drawpixelinfo* dpi, int32_t scrollIndex)
    {
        auto windowRide = get_ride(rideId);
        if (windowRide == nullptr)
            return;

        rct_ride_entry* rideEntry = windowRide->GetRideEntry();

        // Background
        gfx_fill_rect(dpi, { { dpi->x, dpi->y }, { dpi->x + dpi->width, dpi->y + dpi->height } }, PALETTE_INDEX_12);

        rct_widget* widget = &window_ride_vehicle_widgets[WIDX_VEHICLE_TRAINS_PREVIEW];
        int32_t startX = std::max(2, (widget->width() - ((windowRide->num_vehicles - 1) * 36)) / 2 - 25);
        int32_t startY = widget->height() - 4;

        CarEntry* carEntry = &rideEntry->Cars[ride_entry_get_vehicle_at_position(
            windowRide->subtype, windowRide->num_cars_per_train, 0)];
        startY += carEntry->tab_height;

        // For each train
        for (int32_t i = 0; i < windowRide->num_vehicles; i++)
        {
            VehicleDrawInfo trainCarImages[OpenRCT2::Limits::MaxCarsPerTrain];
            VehicleDrawInfo* nextSpriteToDraw = trainCarImages;
            int32_t x = startX;
            int32_t y = startY;

            // For each car in train
            static_assert(std::numeric_limits<decltype(windowRide->num_cars_per_train)>::max() <= std::size(trainCarImages));
            for (int32_t j = 0; j < windowRide->num_cars_per_train; j++)
            {
                carEntry = &rideEntry->Cars[ride_entry_get_vehicle_at_position(
                    windowRide->subtype, windowRide->num_cars_per_train, j)];
                x += carEntry->spacing / 17432;
                y -= (carEntry->spacing / 2) / 17432;

                // Get colour of vehicle
                int32_t vehicleColourIndex = 0;
                switch (windowRide->colour_scheme_type & 3)
                {
                    case VEHICLE_COLOUR_SCHEME_SAME:
                        vehicleColourIndex = 0;
                        break;
                    case VEHICLE_COLOUR_SCHEME_PER_TRAIN:
                        vehicleColourIndex = i;
                        break;
                    case VEHICLE_COLOUR_SCHEME_PER_VEHICLE:
                        vehicleColourIndex = j;
                        break;
                }
                VehicleColour vehicleColour = ride_get_vehicle_colour(windowRide, vehicleColourIndex);

                ImageIndex imageIndex = carEntry->SpriteByYaw(
                    OpenRCT2::Entity::Yaw::BaseRotation / 2, SpriteGroupType::SlopeFlat);
                imageIndex &= carEntry->TabRotationMask;
                imageIndex *= carEntry->base_num_frames;
                imageIndex += carEntry->base_image_id;

                auto imageId = ImageId(imageIndex, vehicleColour.Body, vehicleColour.Trim, vehicleColour.Tertiary);

                nextSpriteToDraw->x = x;
                nextSpriteToDraw->y = y;
                nextSpriteToDraw->imageId = imageId;
                nextSpriteToDraw++;

                x += carEntry->spacing / 17432;
                y -= (carEntry->spacing / 2) / 17432;
            }

            if (windowRide->type == RIDE_TYPE_REVERSER_ROLLER_COASTER)
            {
                VehicleDrawInfo tmp = *(nextSpriteToDraw - 1);
                *(nextSpriteToDraw - 1) = *(nextSpriteToDraw - 2);
                *(nextSpriteToDraw - 2) = tmp;
            }

            VehicleDrawInfo* current = nextSpriteToDraw;
            while (--current >= trainCarImages)
                gfx_draw_sprite(dpi, current->imageId, { current->x, current->y });

            startX += 36;
        }
    }

#pragma endregion

#pragma region Operating

    /**
     *
     *  rct2: 0x006B11D5
     */
    void WindowRideModeTweakIncrease()
    {
        auto windowRide = get_ride(rideId);
        if (windowRide == nullptr)
            return;

        const auto& operatingSettings = windowRide->GetRideTypeDescriptor().OperatingSettings;
        uint8_t maxValue = operatingSettings.MaxValue;
        uint8_t minValue = gCheatsUnlockOperatingLimits ? 0 : operatingSettings.MinValue;

        if (gCheatsUnlockOperatingLimits)
        {
            maxValue = OpenRCT2::Limits::CheatsMaxOperatingLimit;
        }

        uint8_t increment = windowRide->mode == RideMode::Dodgems ? 10 : 1;

        set_operating_setting(
            rideId, RideSetSetting::Operation,
            std::clamp<int16_t>(windowRide->operation_option + increment, minValue, maxValue));
    }

    /**
     *
     *  rct2: 0x006B120A
     */
    void WindowRideModeTweakDecrease()
    {
        auto windowRide = get_ride(rideId);
        if (windowRide == nullptr)
            return;

        const auto& operatingSettings = windowRide->GetRideTypeDescriptor().OperatingSettings;
        uint8_t maxValue = operatingSettings.MaxValue;
        uint8_t minValue = gCheatsUnlockOperatingLimits ? 0 : operatingSettings.MinValue;
        if (gCheatsUnlockOperatingLimits)
        {
            maxValue = OpenRCT2::Limits::CheatsMaxOperatingLimit;
        }

        uint8_t decrement = windowRide->mode == RideMode::Dodgems ? 10 : 1;

        set_operating_setting(
            rideId, RideSetSetting::Operation,
            std::clamp<int16_t>(windowRide->operation_option - decrement, minValue, maxValue));
    }

    /**
     *
     *  rct2: 0x006B1631
     */
    void WindowRideModeDropdown(rct_widget* widget)
    {
        rct_widget* dropdownWidget;

        dropdownWidget = widget - 1;
        auto windowRide = get_ride(rideId);
        if (windowRide == nullptr)
            return;

        auto availableModes = windowRide->GetAvailableModes();

        // Create dropdown list
        auto numAvailableModes = 0;
        auto checkedIndex = -1;
        for (auto i = 0; i < static_cast<uint8_t>(RideMode::Count); i++)
        {
            if (availableModes & (1ULL << i))
            {
                gDropdownItems[numAvailableModes].Format = STR_DROPDOWN_MENU_LABEL;
                gDropdownItems[numAvailableModes].Args = RideModeNames[i];

                if (windowRide->mode == static_cast<RideMode>(i))
                    checkedIndex = numAvailableModes;

                numAvailableModes++;
            }
        }

        WindowDropdownShowTextCustomWidth(
            { windowPos.x + dropdownWidget->left, windowPos.y + dropdownWidget->top }, dropdownWidget->height() + 1, colours[1],
            0, Dropdown::Flag::StayOpen, numAvailableModes, widget->right - dropdownWidget->left);

        if (checkedIndex != -1)
        {
            Dropdown::SetChecked(checkedIndex, true);
        }
    }

    /**
     *
     *  rct2: 0x006B15C0
     */
    void WindowRideLoadDropdown(rct_widget* widget)
    {
        auto windowRide = get_ride(rideId);
        if (windowRide == nullptr)
            return;

        auto dropdownWidget = widget - 1;
        for (auto i = 0; i < 5; i++)
        {
            gDropdownItems[i].Format = STR_DROPDOWN_MENU_LABEL;
            gDropdownItems[i].Args = VehicleLoadNames[i];
        }
        WindowDropdownShowTextCustomWidth(
            { windowPos.x + dropdownWidget->left, windowPos.y + dropdownWidget->top }, dropdownWidget->height() + 1, colours[1],
            0, Dropdown::Flag::StayOpen, 5, widget->right - dropdownWidget->left);

        Dropdown::SetChecked(windowRide->depart_flags & RIDE_DEPART_WAIT_FOR_LOAD_MASK, true);
    }

    /**
     *
     *  rct2: 0x006B10A7
     */
    void WindowRideOperatingMouseup(WidgetIndex widgetIndex)
    {
        const auto currentRideId = rideId;
        auto windowRide = get_ride(currentRideId);
        if (windowRide == nullptr)
            return;

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
            case WIDX_TAB_8:
            case WIDX_TAB_9:
            case WIDX_TAB_10:
                WindowRideSetPage(widgetIndex - WIDX_TAB_1);
                break;
            case WIDX_LOAD_CHECKBOX:
                set_operating_setting(
                    currentRideId, RideSetSetting::Departure, windowRide->depart_flags ^ RIDE_DEPART_WAIT_FOR_LOAD);
                break;
            case WIDX_LEAVE_WHEN_ANOTHER_ARRIVES_CHECKBOX:
                set_operating_setting(
                    currentRideId, RideSetSetting::Departure,
                    windowRide->depart_flags ^ RIDE_DEPART_LEAVE_WHEN_ANOTHER_ARRIVES);
                break;
            case WIDX_MINIMUM_LENGTH_CHECKBOX:
                set_operating_setting(
                    currentRideId, RideSetSetting::Departure, windowRide->depart_flags ^ RIDE_DEPART_WAIT_FOR_MINIMUM_LENGTH);
                break;
            case WIDX_MAXIMUM_LENGTH_CHECKBOX:
                set_operating_setting(
                    currentRideId, RideSetSetting::Departure, windowRide->depart_flags ^ RIDE_DEPART_WAIT_FOR_MAXIMUM_LENGTH);
                break;
            case WIDX_SYNCHRONISE_WITH_ADJACENT_STATIONS_CHECKBOX:
                set_operating_setting(
                    currentRideId, RideSetSetting::Departure,
                    windowRide->depart_flags ^ RIDE_DEPART_SYNCHRONISE_WITH_ADJACENT_STATIONS);
                break;
        }
    }

    /**
     *
     *  rct2: 0x006B1715
     */
    void WindowRideOperatingResize()
    {
        window_set_resize(*this, 316, 186, 316, 186);
    }

    /**
     *
     *  rct2: 0x006B10F4
     */
    void WindowRideOperatingMousedown(WidgetIndex widgetIndex, rct_widget* widget)
    {
        const auto currentRideId = rideId;
        auto windowRide = get_ride(currentRideId);
        if (windowRide == nullptr)
            return;

        uint8_t upper_bound, lower_bound;
        switch (widgetIndex)
        {
            case WIDX_MODE_TWEAK:
                WindowRideOperatingTweakTextInput(*windowRide);
                break;
            case WIDX_MODE_TWEAK_INCREASE:
                WindowRideModeTweakIncrease();
                break;
            case WIDX_MODE_TWEAK_DECREASE:
                WindowRideModeTweakDecrease();
                break;
            case WIDX_LIFT_HILL_SPEED_INCREASE:
                upper_bound = gCheatsUnlockOperatingLimits ? OpenRCT2::Limits::CheatsMaxOperatingLimit
                                                           : windowRide->GetRideTypeDescriptor().LiftData.maximum_speed;
                lower_bound = gCheatsUnlockOperatingLimits ? 0 : windowRide->GetRideTypeDescriptor().LiftData.minimum_speed;
                set_operating_setting(
                    currentRideId, RideSetSetting::LiftHillSpeed,
                    std::clamp<int16_t>(windowRide->lift_hill_speed + 1, lower_bound, upper_bound));
                break;
            case WIDX_LIFT_HILL_SPEED_DECREASE:
                upper_bound = gCheatsUnlockOperatingLimits ? OpenRCT2::Limits::CheatsMaxOperatingLimit
                                                           : windowRide->GetRideTypeDescriptor().LiftData.maximum_speed;
                lower_bound = gCheatsUnlockOperatingLimits ? 0 : windowRide->GetRideTypeDescriptor().LiftData.minimum_speed;
                set_operating_setting(
                    currentRideId, RideSetSetting::LiftHillSpeed,
                    std::clamp<int16_t>(windowRide->lift_hill_speed - 1, lower_bound, upper_bound));
                break;
            case WIDX_MINIMUM_LENGTH:
                WindowRideOperatingLengthWindow(WIDX_MINIMUM_LENGTH);
                break;
            case WIDX_MAXIMUM_LENGTH:
                WindowRideOperatingLengthWindow(WIDX_MAXIMUM_LENGTH);
                break;
            case WIDX_MINIMUM_LENGTH_INCREASE:
                upper_bound = OpenRCT2::Limits::MaxWaitingTime;
                lower_bound = 0;
                set_operating_setting(
                    currentRideId, RideSetSetting::MinWaitingTime,
                    std::clamp<int16_t>(windowRide->min_waiting_time + 1, lower_bound, upper_bound));
                break;
            case WIDX_MINIMUM_LENGTH_DECREASE:
                upper_bound = OpenRCT2::Limits::MaxWaitingTime;
                lower_bound = 0;
                set_operating_setting(
                    currentRideId, RideSetSetting::MinWaitingTime,
                    std::clamp<int16_t>(windowRide->min_waiting_time - 1, lower_bound, upper_bound));
                break;
            case WIDX_MAXIMUM_LENGTH_INCREASE:
                upper_bound = OpenRCT2::Limits::MaxWaitingTime;
                lower_bound = 0;
                set_operating_setting(
                    currentRideId, RideSetSetting::MaxWaitingTime,
                    std::clamp<int16_t>(windowRide->max_waiting_time + 1, lower_bound, upper_bound));
                break;
            case WIDX_MAXIMUM_LENGTH_DECREASE:
                upper_bound = OpenRCT2::Limits::MaxWaitingTime;
                lower_bound = 0;
                set_operating_setting(
                    currentRideId, RideSetSetting::MaxWaitingTime,
                    std::clamp<int16_t>(windowRide->max_waiting_time - 1, lower_bound, upper_bound));
                break;
            case WIDX_MODE_DROPDOWN:
                WindowRideModeDropdown(widget);
                break;
            case WIDX_LOAD_DROPDOWN:
                WindowRideLoadDropdown(widget);
                break;
            case WIDX_OPERATE_NUMBER_OF_CIRCUITS_INCREASE:
                upper_bound = gCheatsUnlockOperatingLimits ? OpenRCT2::Limits::CheatsMaxOperatingLimit
                                                           : OpenRCT2::Limits::MaxCircuitsPerRide;
                lower_bound = 1;
                set_operating_setting(
                    currentRideId, RideSetSetting::NumCircuits,
                    std::clamp<int16_t>(windowRide->num_circuits + 1, lower_bound, upper_bound));
                break;
            case WIDX_OPERATE_NUMBER_OF_CIRCUITS_DECREASE:
                upper_bound = gCheatsUnlockOperatingLimits ? OpenRCT2::Limits::CheatsMaxOperatingLimit
                                                           : OpenRCT2::Limits::MaxCircuitsPerRide;
                lower_bound = 1;
                set_operating_setting(
                    currentRideId, RideSetSetting::NumCircuits,
                    std::clamp<int16_t>(windowRide->num_circuits - 1, lower_bound, upper_bound));
                break;
        }
    }

    void WindowRideOperatingLengthWindow(WidgetIndex widgetIndex)
    {
        auto windowRide = get_ride(rideId);
        if (windowRide == nullptr)
            return;

        uint8_t upperBound = OpenRCT2::Limits::MaxWaitingTime;
        uint8_t lowerBound = 0;
        Formatter ft;
        ft.Add<int16_t>(lowerBound);
        ft.Add<int16_t>(upperBound);
        auto title = (widgetIndex == WIDX_MINIMUM_LENGTH) ? STR_MINIMUM_WAITING_TIME : STR_MAXIMUM_WAITING_TIME;
        auto currentValue = (widgetIndex == WIDX_MINIMUM_LENGTH) ? windowRide->min_waiting_time : windowRide->max_waiting_time;
        char buffer[5]{};
        snprintf(buffer, std::size(buffer), "%u", currentValue);
        WindowTextInputRawOpen(this, widgetIndex, title, STR_ENTER_VALUE, ft, buffer, 4);
    }

    void WindowRideOperatingTweakTextInput(const Ride& windowRide)
    {
        switch (windowRide.mode)
        {
            case RideMode::PoweredLaunchPasstrough:
            case RideMode::PoweredLaunch:
            case RideMode::UpwardLaunch:
            case RideMode::PoweredLaunchBlockSectioned:
            case RideMode::StationToStation:
            case RideMode::Dodgems:
                return;
            default:
                break;
        }

        const auto& operatingSettings = windowRide.GetRideTypeDescriptor().OperatingSettings;
        int16_t maxValue = gCheatsUnlockOperatingLimits ? OpenRCT2::Limits::CheatsMaxOperatingLimit
                                                        : operatingSettings.MaxValue;
        int16_t minValue = gCheatsUnlockOperatingLimits ? 0 : operatingSettings.MinValue;

        const auto& title = window_ride_operating_widgets[WIDX_MODE_TWEAK_LABEL].text;
        Formatter ft;
        ft.Add<int16_t>(minValue * operatingSettings.OperatingSettingMultiplier);
        ft.Add<int16_t>(maxValue * operatingSettings.OperatingSettingMultiplier);

        uint16_t currentValue = static_cast<uint16_t>(windowRide.operation_option)
            * operatingSettings.OperatingSettingMultiplier;
        char buffer[6]{};
        snprintf(buffer, std::size(buffer), "%u", currentValue);

        WindowTextInputRawOpen(this, WIDX_MODE_TWEAK, title, STR_ENTER_VALUE, ft, buffer, 4);
    }

    /**
     *
     *  rct2: 0x006B1165
     */
    void WindowRideOperatingDropdown(WidgetIndex widgetIndex, int32_t dropdownIndex)
    {
        if (dropdownIndex == -1)
            return;

        const auto currentRideId = rideId;
        auto windowRide = get_ride(currentRideId);
        if (windowRide == nullptr)
            return;

        switch (widgetIndex)
        {
            case WIDX_MODE_DROPDOWN:
            {
                RideMode rideMode = RideMode::NullMode;
                auto availableModes = windowRide->GetAvailableModes();
                auto modeInDropdownIndex = -1;
                for (RideMode rideModeIndex = RideMode::Normal; rideModeIndex < RideMode::Count; rideModeIndex++)
                {
                    if (availableModes & EnumToFlag(rideModeIndex))
                    {
                        modeInDropdownIndex++;
                        if (modeInDropdownIndex == dropdownIndex)
                        {
                            rideMode = rideModeIndex;
                            break;
                        }
                    }
                }
                if (rideMode != RideMode::NullMode)
                    set_operating_setting(currentRideId, RideSetSetting::Mode, static_cast<uint8_t>(rideMode));
                break;
            }
            case WIDX_LOAD_DROPDOWN:
                set_operating_setting(
                    currentRideId, RideSetSetting::Departure,
                    (windowRide->depart_flags & ~RIDE_DEPART_WAIT_FOR_LOAD_MASK) | dropdownIndex);
                break;
        }
    }

    /**
     *
     *  rct2: 0x006B178E
     */
    void WindowRideOperatingUpdate()
    {
        frame_no++;
        window_event_invalidate_call(this);
        widget_invalidate(*this, WIDX_TAB_3);

        auto windowRide = get_ride(rideId);
        if (windowRide != nullptr && windowRide->window_invalidate_flags & RIDE_INVALIDATE_RIDE_OPERATING)
        {
            windowRide->window_invalidate_flags &= ~RIDE_INVALIDATE_RIDE_OPERATING;
            Invalidate();
        }
    }

    void WindowRideOperatingTextinput(WidgetIndex widgetIndex, std::string_view text)
    {
        if (text.empty())
            return;

        auto windowRide = get_ride(rideId);
        if (windowRide == nullptr)
            return;

        if (widgetIndex == WIDX_MODE_TWEAK)
        {
            const auto& operatingSettings = windowRide->GetRideTypeDescriptor().OperatingSettings;
            uint32_t maxValue = gCheatsUnlockOperatingLimits ? OpenRCT2::Limits::CheatsMaxOperatingLimit
                                                             : operatingSettings.MaxValue;
            uint32_t minValue = gCheatsUnlockOperatingLimits ? 0 : operatingSettings.MinValue;
            auto multiplier = windowRide->GetRideTypeDescriptor().OperatingSettings.OperatingSettingMultiplier;

            try
            {
                uint32_t origSize = std::stol(std::string(text)) / multiplier;
                uint8_t size = static_cast<uint8_t>(std::clamp(origSize, minValue, maxValue));
                set_operating_setting(windowRide->id, RideSetSetting::Operation, size);
            }
            catch (const std::logic_error&)
            {
                // std::stol can throw std::out_of_range or std::invalid_argument
            }
        }
        else if (widgetIndex == WIDX_MINIMUM_LENGTH || widgetIndex == WIDX_MAXIMUM_LENGTH)
        {
            try
            {
                auto rideSetSetting = widgetIndex == WIDX_MINIMUM_LENGTH ? RideSetSetting::MinWaitingTime
                                                                         : RideSetSetting::MaxWaitingTime;

                uint16_t upperBound = OpenRCT2::Limits::MaxWaitingTime;
                uint16_t lowerBound = 0;
                uint16_t size = std::stol(std::string(text));
                size = std::clamp(size, lowerBound, upperBound);
                set_operating_setting(windowRide->id, rideSetSetting, size);
            }
            catch (const std::logic_error&)
            {
                // std::stol can throw std::out_of_range or std::invalid_argument
            }
        }
    }

    /**
     *
     *  rct2: 0x006B0B30
     */
    void WindowRideOperatingInvalidate()
    {
        rct_widget* newWidgets;
        StringId format, caption, tooltip;

        newWidgets = window_ride_page_widgets[page];
        if (widgets != newWidgets)
        {
            widgets = newWidgets;
            WindowInitScrollWidgets(*this);
        }

        WindowRideSetPressedTab();

        auto windowRide = get_ride(rideId);
        if (windowRide == nullptr)
            return;

        auto ft = Formatter::Common();
        windowRide->FormatNameTo(ft);

        // Widget setup
        pressed_widgets &= ~(
            (1ULL << WIDX_LOAD_CHECKBOX) | (1ULL << WIDX_LEAVE_WHEN_ANOTHER_ARRIVES_CHECKBOX)
            | (1ULL << WIDX_MINIMUM_LENGTH_CHECKBOX) | (1ULL << WIDX_MAXIMUM_LENGTH_CHECKBOX)
            | (1ULL << WIDX_SYNCHRONISE_WITH_ADJACENT_STATIONS_CHECKBOX));

        // Sometimes, only one of the alternatives support lift hill pieces. Make sure to check both.
        bool hasAlternativeType = windowRide->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_HAS_ALTERNATIVE_TRACK_TYPE);
        if (windowRide->GetRideTypeDescriptor().SupportsTrackPiece(TRACK_LIFT_HILL)
            || (hasAlternativeType
                && GetRideTypeDescriptor(windowRide->GetRideTypeDescriptor().AlternateType)
                       .SupportsTrackPiece(TRACK_LIFT_HILL)))
        {
            window_ride_operating_widgets[WIDX_LIFT_HILL_SPEED_LABEL].type = WindowWidgetType::Label;
            window_ride_operating_widgets[WIDX_LIFT_HILL_SPEED].type = WindowWidgetType::Spinner;
            window_ride_operating_widgets[WIDX_LIFT_HILL_SPEED_INCREASE].type = WindowWidgetType::Button;
            window_ride_operating_widgets[WIDX_LIFT_HILL_SPEED_DECREASE].type = WindowWidgetType::Button;
            ft.Rewind();
            ft.Increment(20);
            ft.Add<uint16_t>(windowRide->lift_hill_speed);
        }
        else
        {
            window_ride_operating_widgets[WIDX_LIFT_HILL_SPEED_LABEL].type = WindowWidgetType::Empty;
            window_ride_operating_widgets[WIDX_LIFT_HILL_SPEED].type = WindowWidgetType::Empty;
            window_ride_operating_widgets[WIDX_LIFT_HILL_SPEED_INCREASE].type = WindowWidgetType::Empty;
            window_ride_operating_widgets[WIDX_LIFT_HILL_SPEED_DECREASE].type = WindowWidgetType::Empty;
        }

        // Number of circuits
        if (windowRide->CanHaveMultipleCircuits())
        {
            window_ride_operating_widgets[WIDX_OPERATE_NUMBER_OF_CIRCUITS_LABEL].type = WindowWidgetType::Label;
            window_ride_operating_widgets[WIDX_OPERATE_NUMBER_OF_CIRCUITS].type = WindowWidgetType::Spinner;
            window_ride_operating_widgets[WIDX_OPERATE_NUMBER_OF_CIRCUITS_INCREASE].type = WindowWidgetType::Button;
            window_ride_operating_widgets[WIDX_OPERATE_NUMBER_OF_CIRCUITS_DECREASE].type = WindowWidgetType::Button;
            ft.Rewind();
            ft.Increment(22);
            ft.Add<uint16_t>(windowRide->num_circuits);
        }
        else
        {
            window_ride_operating_widgets[WIDX_OPERATE_NUMBER_OF_CIRCUITS_LABEL].type = WindowWidgetType::Empty;
            window_ride_operating_widgets[WIDX_OPERATE_NUMBER_OF_CIRCUITS].type = WindowWidgetType::Empty;
            window_ride_operating_widgets[WIDX_OPERATE_NUMBER_OF_CIRCUITS_INCREASE].type = WindowWidgetType::Empty;
            window_ride_operating_widgets[WIDX_OPERATE_NUMBER_OF_CIRCUITS_DECREASE].type = WindowWidgetType::Empty;
        }

        // Leave if another vehicle arrives at station
        if (windowRide->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_HAS_LEAVE_WHEN_ANOTHER_VEHICLE_ARRIVES_AT_STATION)
            && windowRide->num_vehicles > 1 && !windowRide->IsBlockSectioned())
        {
            window_ride_operating_widgets[WIDX_LEAVE_WHEN_ANOTHER_ARRIVES_CHECKBOX].type = WindowWidgetType::Checkbox;
            window_ride_operating_widgets[WIDX_LEAVE_WHEN_ANOTHER_ARRIVES_CHECKBOX].tooltip
                = STR_LEAVE_IF_ANOTHER_VEHICLE_ARRIVES_TIP;
            window_ride_operating_widgets[WIDX_LEAVE_WHEN_ANOTHER_ARRIVES_CHECKBOX].text = windowRide->GetRideTypeDescriptor()
                                                                                               .NameConvention.vehicle
                    == RideComponentType::Boat
                ? STR_LEAVE_IF_ANOTHER_BOAT_ARRIVES
                : STR_LEAVE_IF_ANOTHER_TRAIN_ARRIVES;
        }
        else
        {
            window_ride_operating_widgets[WIDX_LEAVE_WHEN_ANOTHER_ARRIVES_CHECKBOX].type = WindowWidgetType::Empty;
        }

        // Synchronise with adjacent stations
        if (windowRide->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_CAN_SYNCHRONISE_ADJACENT_STATIONS))
        {
            window_ride_operating_widgets[WIDX_SYNCHRONISE_WITH_ADJACENT_STATIONS_CHECKBOX].type = WindowWidgetType::Checkbox;
            window_ride_operating_widgets[WIDX_SYNCHRONISE_WITH_ADJACENT_STATIONS_CHECKBOX].image
                = STR_SYNCHRONISE_WITH_ADJACENT_STATIONS;
            window_ride_operating_widgets[WIDX_SYNCHRONISE_WITH_ADJACENT_STATIONS_CHECKBOX].tooltip
                = STR_SYNCHRONISE_WITH_ADJACENT_STATIONS_TIP;
        }
        else
        {
            window_ride_operating_widgets[WIDX_SYNCHRONISE_WITH_ADJACENT_STATIONS_CHECKBOX].type = WindowWidgetType::Empty;
        }

        // Mode
        window_ride_operating_widgets[WIDX_MODE].text = RideModeNames[static_cast<int>(windowRide->mode)];

        // Waiting
        window_ride_operating_widgets[WIDX_LOAD].text = VehicleLoadNames[(
            windowRide->depart_flags & RIDE_DEPART_WAIT_FOR_LOAD_MASK)];
        if (windowRide->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_HAS_LOAD_OPTIONS))
        {
            window_ride_operating_widgets[WIDX_LOAD_CHECKBOX].type = WindowWidgetType::Checkbox;
            window_ride_operating_widgets[WIDX_LOAD].type = WindowWidgetType::DropdownMenu;
            window_ride_operating_widgets[WIDX_LOAD_DROPDOWN].type = WindowWidgetType::Button;

            window_ride_operating_widgets[WIDX_MINIMUM_LENGTH_CHECKBOX].type = WindowWidgetType::Checkbox;
            window_ride_operating_widgets[WIDX_MINIMUM_LENGTH].type = WindowWidgetType::Spinner;
            window_ride_operating_widgets[WIDX_MINIMUM_LENGTH_INCREASE].type = WindowWidgetType::Button;
            window_ride_operating_widgets[WIDX_MINIMUM_LENGTH_DECREASE].type = WindowWidgetType::Button;

            window_ride_operating_widgets[WIDX_MAXIMUM_LENGTH_CHECKBOX].type = WindowWidgetType::Checkbox;
            window_ride_operating_widgets[WIDX_MAXIMUM_LENGTH].type = WindowWidgetType::Spinner;
            window_ride_operating_widgets[WIDX_MAXIMUM_LENGTH_INCREASE].type = WindowWidgetType::Button;
            window_ride_operating_widgets[WIDX_MAXIMUM_LENGTH_DECREASE].type = WindowWidgetType::Button;

            ft.Rewind();
            ft.Increment(10);
            ft.Add<StringId>(STR_FORMAT_SECONDS);
            ft.Add<uint16_t>(windowRide->min_waiting_time);
            ft.Add<StringId>(STR_FORMAT_SECONDS);
            ft.Add<uint16_t>(windowRide->max_waiting_time);

            if (windowRide->depart_flags & RIDE_DEPART_WAIT_FOR_LOAD)
                pressed_widgets |= (1ULL << WIDX_LOAD_CHECKBOX);
        }
        else
        {
            window_ride_operating_widgets[WIDX_LOAD_CHECKBOX].type = WindowWidgetType::Empty;
            window_ride_operating_widgets[WIDX_LOAD].type = WindowWidgetType::Empty;
            window_ride_operating_widgets[WIDX_LOAD_DROPDOWN].type = WindowWidgetType::Empty;

            window_ride_operating_widgets[WIDX_MINIMUM_LENGTH_CHECKBOX].type = WindowWidgetType::Empty;
            window_ride_operating_widgets[WIDX_MINIMUM_LENGTH].type = WindowWidgetType::Empty;
            window_ride_operating_widgets[WIDX_MINIMUM_LENGTH_INCREASE].type = WindowWidgetType::Empty;
            window_ride_operating_widgets[WIDX_MINIMUM_LENGTH_DECREASE].type = WindowWidgetType::Empty;

            window_ride_operating_widgets[WIDX_MAXIMUM_LENGTH_CHECKBOX].type = WindowWidgetType::Empty;
            window_ride_operating_widgets[WIDX_MAXIMUM_LENGTH].type = WindowWidgetType::Empty;
            window_ride_operating_widgets[WIDX_MAXIMUM_LENGTH_INCREASE].type = WindowWidgetType::Empty;
            window_ride_operating_widgets[WIDX_MAXIMUM_LENGTH_DECREASE].type = WindowWidgetType::Empty;
        }

        if (windowRide->depart_flags & RIDE_DEPART_LEAVE_WHEN_ANOTHER_ARRIVES)
            pressed_widgets |= (1ULL << WIDX_LEAVE_WHEN_ANOTHER_ARRIVES_CHECKBOX);
        if (windowRide->depart_flags & RIDE_DEPART_SYNCHRONISE_WITH_ADJACENT_STATIONS)
            pressed_widgets |= (1ULL << WIDX_SYNCHRONISE_WITH_ADJACENT_STATIONS_CHECKBOX);
        if (windowRide->depart_flags & RIDE_DEPART_WAIT_FOR_MINIMUM_LENGTH)
            pressed_widgets |= (1ULL << WIDX_MINIMUM_LENGTH_CHECKBOX);
        if (windowRide->depart_flags & RIDE_DEPART_WAIT_FOR_MAXIMUM_LENGTH)
            pressed_widgets |= (1ULL << WIDX_MAXIMUM_LENGTH_CHECKBOX);

        // Mode specific functionality
        auto multiplier = windowRide->GetRideTypeDescriptor().OperatingSettings.OperatingSettingMultiplier;
        ft.Rewind();
        ft.Increment(18);
        ft.Add<uint16_t>(static_cast<uint16_t>(windowRide->operation_option) * multiplier);
        switch (windowRide->mode)
        {
            case RideMode::PoweredLaunchPasstrough:
            case RideMode::PoweredLaunch:
            case RideMode::UpwardLaunch:
            case RideMode::PoweredLaunchBlockSectioned:
                ft.Rewind();
                ft.Increment(18);
                ft.Add<uint16_t>((windowRide->launch_speed * 9) / 4);
                format = STR_RIDE_MODE_SPEED_VALUE;
                caption = STR_LAUNCH_SPEED;
                tooltip = STR_LAUNCH_SPEED_TIP;
                break;
            case RideMode::StationToStation:
                ft.Rewind();
                ft.Increment(18);
                ft.Add<uint16_t>((windowRide->speed * 9) / 4);
                format = STR_RIDE_MODE_SPEED_VALUE;
                caption = STR_SPEED;
                tooltip = STR_SPEED_TIP;
                break;
            case RideMode::Race:
                ft.Rewind();
                ft.Increment(18);
                ft.Add<uint16_t>(windowRide->NumLaps);
                format = STR_NUMBER_OF_LAPS_VALUE;
                caption = STR_NUMBER_OF_LAPS;
                tooltip = STR_NUMBER_OF_LAPS_TIP;
                break;
            case RideMode::Dodgems:
                format = STR_RIDE_MODE_TIME_LIMIT_VALUE;
                caption = STR_TIME_LIMIT;
                tooltip = STR_TIME_LIMIT_TIP;
                break;
            case RideMode::Swing:
                format = STR_RIDE_MODE_NUMBER_OF_SWINGS_VALUE;
                caption = STR_NUMBER_OF_SWINGS;
                tooltip = STR_NUMBER_OF_SWINGS_TIP;
                break;
            case RideMode::Rotation:
            case RideMode::ForwardRotation:
            case RideMode::BackwardRotation:
                format = STR_NUMBER_OF_ROTATIONS_VALUE;
                caption = STR_NUMBER_OF_ROTATIONS;
                tooltip = STR_NUMBER_OF_ROTATIONS_TIP;
                break;
            default:
                format = STR_MAX_PEOPLE_ON_RIDE_VALUE;
                caption = STR_MAX_PEOPLE_ON_RIDE;
                tooltip = STR_MAX_PEOPLE_ON_RIDE_TIP;
                if (!windowRide->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_NO_VEHICLES))
                    format = 0;
                break;
        }

        if (format != 0)
        {
            window_ride_operating_widgets[WIDX_MODE_TWEAK_LABEL].type = WindowWidgetType::Label;
            window_ride_operating_widgets[WIDX_MODE_TWEAK_LABEL].text = caption;
            window_ride_operating_widgets[WIDX_MODE_TWEAK_LABEL].tooltip = tooltip;
            window_ride_operating_widgets[WIDX_MODE_TWEAK].type = WindowWidgetType::Spinner;
            window_ride_operating_widgets[WIDX_MODE_TWEAK].text = format;
            window_ride_operating_widgets[WIDX_MODE_TWEAK_INCREASE].type = WindowWidgetType::Button;
            window_ride_operating_widgets[WIDX_MODE_TWEAK_DECREASE].type = WindowWidgetType::Button;
            pressed_widgets &= ~(1ULL << WIDX_LEAVE_WHEN_ANOTHER_ARRIVES_CHECKBOX);
        }
        else
        {
            window_ride_operating_widgets[WIDX_MODE_TWEAK_LABEL].type = WindowWidgetType::Empty;
            window_ride_operating_widgets[WIDX_MODE_TWEAK].type = WindowWidgetType::Empty;
            window_ride_operating_widgets[WIDX_MODE_TWEAK_INCREASE].type = WindowWidgetType::Empty;
            window_ride_operating_widgets[WIDX_MODE_TWEAK_DECREASE].type = WindowWidgetType::Empty;
        }

        WindowRideAnchorBorderWidgets();
        window_align_tabs(this, WIDX_TAB_1, WIDX_TAB_10);
    }

    /**
     *
     *  rct2: 0x006B1001
     */
    void WindowRideOperatingPaint(rct_drawpixelinfo* dpi)
    {
        WindowDrawWidgets(*this, dpi);
        WindowRideDrawTabImages(dpi);

        auto windowRide = get_ride(rideId);
        if (windowRide == nullptr)
            return;

        // Horizontal rule between mode settings and depart settings
        gfx_fill_rect_inset(
            dpi,
            { windowPos + ScreenCoordsXY{ window_ride_operating_widgets[WIDX_PAGE_BACKGROUND].left + 4, 103 },
              windowPos + ScreenCoordsXY{ window_ride_operating_widgets[WIDX_PAGE_BACKGROUND].right - 5, 104 } },
            colours[1], INSET_RECT_FLAG_BORDER_INSET);

        // Number of block sections
        if (windowRide->IsBlockSectioned())
        {
            auto ft = Formatter();
            ft.Add<uint16_t>(windowRide->num_block_brakes + windowRide->num_stations);
            DrawTextBasic(
                dpi, windowPos + ScreenCoordsXY{ 21, windowRide->mode == RideMode::PoweredLaunchBlockSectioned ? 89 : 61 },
                STR_BLOCK_SECTIONS, ft, COLOUR_BLACK);
        }
    }

#pragma endregion

#pragma region Maintenance

    /**
     *
     *  rct2: 0x006B1AE4
     */
    void WindowRideLocateMechanic()
    {
        auto windowRide = get_ride(rideId);
        if (windowRide == nullptr)
            return;

        // First check if there is a mechanic assigned
        Peep* mechanic = ride_get_assigned_mechanic(windowRide);

        // Otherwise find the closest mechanic
        if (mechanic == nullptr)
            mechanic = ride_find_closest_mechanic(windowRide, 1);

        if (mechanic == nullptr)
            context_show_error(STR_UNABLE_TO_LOCATE_MECHANIC, STR_NONE, {});
        else
        {
            auto intent = Intent(WindowClass::Peep);
            intent.putExtra(INTENT_EXTRA_PEEP, mechanic);
            context_open_intent(&intent);
        }
    }

    /**
     *
     *  rct2: 0x006B7D08
     */
    void WindowRideMaintenanceDrawBar(rct_drawpixelinfo* dpi, const ScreenCoordsXY& coords, int32_t value, int32_t colour)
    {
        gfx_fill_rect_inset(dpi, { coords, coords + ScreenCoordsXY{ 149, 8 } }, colours[1], INSET_RECT_F_30);
        if (colour & BAR_BLINK)
        {
            colour &= ~BAR_BLINK;
            if (game_is_not_paused() && (gCurrentRealTimeTicks & 8))
                return;
        }

        value = ((186 * ((value * 2) & 0xFF)) >> 8) & 0xFF;
        if (value > 2)
        {
            gfx_fill_rect_inset(dpi, { coords + ScreenCoordsXY{ 2, 1 }, coords + ScreenCoordsXY{ value + 1, 7 } }, colour, 0);
        }
    }

    /**
     *
     *  rct2: 0x006B1AAD
     */
    void WindowRideMaintenanceMouseup(WidgetIndex widgetIndex)
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
            case WIDX_TAB_8:
            case WIDX_TAB_9:
            case WIDX_TAB_10:
                WindowRideSetPage(widgetIndex - WIDX_TAB_1);
                break;
            case WIDX_LOCATE_MECHANIC:
                WindowRideLocateMechanic();
                break;
            case WIDX_REFURBISH_RIDE:
                context_open_detail_window(WD_REFURBISH_RIDE, number);
                break;
        }
    }

    /**
     *
     *  rct2: 0x006B1D70
     */
    void WindowRideMaintenanceResize()
    {
        window_set_resize(*this, 316, 135, 316, 135);
    }

    /**
     *
     *  rct2: 0x006B1ACE
     */
    void WindowRideMaintenanceMousedown(WidgetIndex widgetIndex, rct_widget* widget)
    {
        auto windowRide = get_ride(rideId);
        if (windowRide == nullptr)
            return;

        auto rideEntry = windowRide->GetRideEntry();
        if (rideEntry == nullptr)
            return;

        rct_widget* dropdownWidget = widget;
        int32_t j, num_items;

        switch (widgetIndex)
        {
            case WIDX_INSPECTION_INTERVAL_DROPDOWN:
                dropdownWidget--;
                for (int32_t i = 0; i < 7; i++)
                {
                    gDropdownItems[i].Format = STR_DROPDOWN_MENU_LABEL;
                    gDropdownItems[i].Args = RideInspectionIntervalNames[i];
                }
                WindowDropdownShowTextCustomWidth(
                    { windowPos.x + dropdownWidget->left, windowPos.y + dropdownWidget->top }, dropdownWidget->height() + 1,
                    colours[1], 0, Dropdown::Flag::StayOpen, 7, widget->right - dropdownWidget->left);

                Dropdown::SetChecked(windowRide->inspection_interval, true);
                break;

            case WIDX_FORCE_BREAKDOWN:
                num_items = 1;
                for (j = 0; j < RCT2::ObjectLimits::MaxRideTypesPerRideEntry; j++)
                {
                    if (rideEntry->ride_type[j] != RIDE_TYPE_NULL)
                        break;
                }
                gDropdownItems[0].Format = STR_DROPDOWN_MENU_LABEL;
                gDropdownItems[0].Args = STR_DEBUG_FIX_RIDE;
                for (int32_t i = 0; i < 8; i++)
                {
                    assert(j < static_cast<int32_t>(std::size(rideEntry->ride_type)));
                    if (GetRideTypeDescriptor(rideEntry->ride_type[j]).AvailableBreakdowns & static_cast<uint8_t>(1 << i))
                    {
                        if (i == BREAKDOWN_BRAKES_FAILURE && windowRide->IsBlockSectioned())
                        {
                            if (windowRide->num_vehicles != 1)
                                continue;
                        }
                        gDropdownItems[num_items].Format = STR_DROPDOWN_MENU_LABEL;
                        gDropdownItems[num_items].Args = RideBreakdownReasonNames[i];
                        num_items++;
                    }
                }
                if (num_items == 1)
                {
                    context_show_error(STR_DEBUG_NO_BREAKDOWNS_AVAILABLE, STR_NONE, {});
                }
                else
                {
                    WindowDropdownShowText(
                        { windowPos.x + dropdownWidget->left, windowPos.y + dropdownWidget->top }, dropdownWidget->height() + 1,
                        colours[1], Dropdown::Flag::StayOpen, num_items);

                    num_items = 1;
                    int32_t breakdownReason = windowRide->breakdown_reason_pending;
                    if (breakdownReason != BREAKDOWN_NONE && (windowRide->lifecycle_flags & RIDE_LIFECYCLE_BREAKDOWN_PENDING))
                    {
                        for (int32_t i = 0; i < 8; i++)
                        {
                            if (GetRideTypeDescriptor(rideEntry->ride_type[j]).AvailableBreakdowns
                                & static_cast<uint8_t>(1 << i))
                            {
                                if (i == BREAKDOWN_BRAKES_FAILURE && windowRide->IsBlockSectioned())
                                {
                                    if (windowRide->num_vehicles != 1)
                                        continue;
                                }
                                if (i == breakdownReason)
                                {
                                    Dropdown::SetChecked(num_items, true);
                                    break;
                                }
                                gDropdownItems[num_items].Format = STR_DROPDOWN_MENU_LABEL;
                                gDropdownItems[num_items].Args = RideBreakdownReasonNames[i];
                                num_items++;
                            }
                        }
                    }

                    if ((windowRide->lifecycle_flags & RIDE_LIFECYCLE_BREAKDOWN_PENDING) == 0)
                    {
                        Dropdown::SetDisabled(0, true);
                    }
                }
                break;
        }
    }

    /**
     *
     *  rct2: 0x006B1AD9
     */
    void WindowRideMaintenanceDropdown(WidgetIndex widgetIndex, int32_t dropdownIndex)
    {
        if (dropdownIndex == -1)
            return;

        auto windowRide = get_ride(rideId);
        if (windowRide == nullptr)
            return;

        auto rideEntry = windowRide->GetRideEntry();
        if (rideEntry == nullptr)
            return;

        switch (widgetIndex)
        {
            case WIDX_INSPECTION_INTERVAL_DROPDOWN:
                set_operating_setting(rideId, RideSetSetting::InspectionInterval, dropdownIndex);
                break;

            case WIDX_FORCE_BREAKDOWN:
                if (dropdownIndex == 0)
                {
                    Vehicle* vehicle;
                    switch (windowRide->breakdown_reason_pending)
                    {
                        case BREAKDOWN_SAFETY_CUT_OUT:
                            if (!(windowRide->lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK))
                                break;
                            for (int32_t i = 0; i < windowRide->num_vehicles; ++i)
                            {
                                for (vehicle = GetEntity<Vehicle>(windowRide->vehicles[i]); vehicle != nullptr;
                                     vehicle = GetEntity<Vehicle>(vehicle->next_vehicle_on_train))
                                {
                                    vehicle->ClearUpdateFlag(
                                        VEHICLE_UPDATE_FLAG_BROKEN_CAR | VEHICLE_UPDATE_FLAG_ZERO_VELOCITY
                                        | VEHICLE_UPDATE_FLAG_BROKEN_TRAIN);
                                }
                            }
                            break;
                        case BREAKDOWN_RESTRAINTS_STUCK_CLOSED:
                        case BREAKDOWN_RESTRAINTS_STUCK_OPEN:
                        case BREAKDOWN_DOORS_STUCK_CLOSED:
                        case BREAKDOWN_DOORS_STUCK_OPEN:
                            vehicle = GetEntity<Vehicle>(windowRide->vehicles[windowRide->broken_vehicle]);
                            if (vehicle != nullptr)
                            {
                                vehicle->ClearUpdateFlag(VEHICLE_UPDATE_FLAG_BROKEN_CAR);
                            }
                            break;
                        case BREAKDOWN_VEHICLE_MALFUNCTION:
                            vehicle = GetEntity<Vehicle>(windowRide->vehicles[windowRide->broken_vehicle]);
                            if (vehicle != nullptr)
                            {
                                vehicle->ClearUpdateFlag(VEHICLE_UPDATE_FLAG_BROKEN_TRAIN);
                            }
                            break;
                    }
                    windowRide->lifecycle_flags &= ~(RIDE_LIFECYCLE_BREAKDOWN_PENDING | RIDE_LIFECYCLE_BROKEN_DOWN);
                    window_invalidate_by_number(WindowClass::Ride, number);
                    break;
                }
                if (windowRide->lifecycle_flags
                    & (RIDE_LIFECYCLE_BREAKDOWN_PENDING | RIDE_LIFECYCLE_BROKEN_DOWN | RIDE_LIFECYCLE_CRASHED))
                {
                    context_show_error(STR_DEBUG_CANT_FORCE_BREAKDOWN, STR_DEBUG_RIDE_ALREADY_BROKEN, {});
                }
                else if (windowRide->status == RideStatus::Closed)
                {
                    context_show_error(STR_DEBUG_CANT_FORCE_BREAKDOWN, STR_DEBUG_RIDE_IS_CLOSED, {});
                }
                else
                {
                    int32_t j;
                    for (j = 0; j < RCT2::ObjectLimits::MaxRideTypesPerRideEntry; j++)
                    {
                        if (rideEntry->ride_type[j] != RIDE_TYPE_NULL)
                            break;
                    }
                    int32_t i;
                    int32_t num_items = 1;
                    for (i = 0; i < BREAKDOWN_COUNT; i++)
                    {
                        assert(j < static_cast<int32_t>(std::size(rideEntry->ride_type)));
                        if (GetRideTypeDescriptor(rideEntry->ride_type[j]).AvailableBreakdowns & static_cast<uint8_t>(1 << i))
                        {
                            if (i == BREAKDOWN_BRAKES_FAILURE && windowRide->IsBlockSectioned())
                            {
                                if (windowRide->num_vehicles != 1)
                                    continue;
                            }
                            if (num_items == dropdownIndex)
                                break;
                            num_items++;
                        }
                    }
                    ride_prepare_breakdown(windowRide, i);
                }
                break;
        }
    }

    /**
     *
     *  rct2: 0x006B1D37
     */
    void WindowRideMaintenanceUpdate()
    {
        frame_no++;
        window_event_invalidate_call(this);
        widget_invalidate(*this, WIDX_TAB_4);

        auto windowRide = get_ride(rideId);
        if (windowRide != nullptr && windowRide->window_invalidate_flags & RIDE_INVALIDATE_RIDE_MAINTENANCE)
        {
            windowRide->window_invalidate_flags &= ~RIDE_INVALIDATE_RIDE_MAINTENANCE;
            Invalidate();
        }
    }

    /**
     *
     *  rct2: 0x006B17C8
     */
    void WindowRideMaintenanceInvalidate()
    {
        auto newWidgets = window_ride_page_widgets[page];
        if (widgets != newWidgets)
        {
            widgets = newWidgets;
            WindowInitScrollWidgets(*this);
        }

        WindowRideSetPressedTab();

        auto windowRide = get_ride(rideId);
        if (windowRide == nullptr)
            return;

        auto ft = Formatter::Common();
        windowRide->FormatNameTo(ft);

        window_ride_maintenance_widgets[WIDX_INSPECTION_INTERVAL].text = RideInspectionIntervalNames[windowRide
                                                                                                         ->inspection_interval];

        WindowRideAnchorBorderWidgets();
        window_align_tabs(this, WIDX_TAB_1, WIDX_TAB_10);

        if (gConfigGeneral.debugging_tools && network_get_mode() == NETWORK_MODE_NONE)
        {
            window_ride_maintenance_widgets[WIDX_FORCE_BREAKDOWN].type = WindowWidgetType::FlatBtn;
        }
        else
        {
            window_ride_maintenance_widgets[WIDX_FORCE_BREAKDOWN].type = WindowWidgetType::Empty;
        }

        if (windowRide->GetRideTypeDescriptor().AvailableBreakdowns == 0
            || !(windowRide->lifecycle_flags & RIDE_LIFECYCLE_EVER_BEEN_OPENED))
        {
            disabled_widgets |= (1ULL << WIDX_REFURBISH_RIDE);
            window_ride_maintenance_widgets[WIDX_REFURBISH_RIDE].tooltip = STR_CANT_REFURBISH_NOT_NEEDED;
        }
        else
        {
            disabled_widgets &= ~(1ULL << WIDX_REFURBISH_RIDE);
            window_ride_maintenance_widgets[WIDX_REFURBISH_RIDE].tooltip = STR_REFURBISH_RIDE_TIP;
        }
    }

    /**
     *
     *  rct2: 0x006B1877
     */
    void WindowRideMaintenancePaint(rct_drawpixelinfo* dpi)
    {
        WindowDrawWidgets(*this, dpi);
        WindowRideDrawTabImages(dpi);

        auto windowRide = get_ride(rideId);
        if (windowRide == nullptr)
            return;

        // Locate mechanic button image
        rct_widget* widget = &window_ride_maintenance_widgets[WIDX_LOCATE_MECHANIC];
        auto screenCoords = windowPos + ScreenCoordsXY{ widget->left, widget->top };
        gfx_draw_sprite(
            dpi, (gStaffMechanicColour << 24) | IMAGE_TYPE_REMAP | IMAGE_TYPE_REMAP_2_PLUS | SPR_MECHANIC, screenCoords, 0);

        // Inspection label
        widget = &window_ride_maintenance_widgets[WIDX_INSPECTION_INTERVAL];
        screenCoords = windowPos + ScreenCoordsXY{ 4, widget->top + 1 };
        DrawTextBasic(dpi, screenCoords, STR_INSPECTION);

        // Reliability
        widget = &window_ride_maintenance_widgets[WIDX_PAGE_BACKGROUND];
        screenCoords = windowPos + ScreenCoordsXY{ widget->left + 4, widget->top + 4 };

        uint16_t reliability = windowRide->reliability_percentage;
        auto ft = Formatter();
        ft.Add<uint16_t>(reliability);
        DrawTextBasic(dpi, screenCoords, STR_RELIABILITY_LABEL_1757, ft);
        WindowRideMaintenanceDrawBar(
            dpi, screenCoords + ScreenCoordsXY{ 103, 0 }, std::max<int32_t>(10, reliability), COLOUR_BRIGHT_GREEN);
        screenCoords.y += 11;

        uint16_t downTime = windowRide->downtime;
        ft = Formatter();
        ft.Add<uint16_t>(downTime);
        DrawTextBasic(dpi, screenCoords, STR_DOWN_TIME_LABEL_1889, ft);
        WindowRideMaintenanceDrawBar(dpi, screenCoords + ScreenCoordsXY{ 103, 0 }, downTime, COLOUR_BRIGHT_RED);
        screenCoords.y += 26;

        // Last inspection
        StringId stringId;
        if (windowRide->last_inspection <= 1)
            stringId = STR_TIME_SINCE_LAST_INSPECTION_MINUTE;
        else if (windowRide->last_inspection <= 240)
            stringId = STR_TIME_SINCE_LAST_INSPECTION_MINUTES;
        else
            stringId = STR_TIME_SINCE_LAST_INSPECTION_MORE_THAN_4_HOURS;

        ft = Formatter();
        ft.Add<uint16_t>(windowRide->last_inspection);
        DrawTextBasic(dpi, screenCoords, stringId, ft);
        screenCoords.y += 12;

        // Last / current breakdown
        if (windowRide->breakdown_reason == BREAKDOWN_NONE)
            return;

        stringId = (windowRide->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN) ? STR_CURRENT_BREAKDOWN : STR_LAST_BREAKDOWN;
        ft = Formatter();
        ft.Add<StringId>(RideBreakdownReasonNames[windowRide->breakdown_reason]);
        DrawTextBasic(dpi, screenCoords, stringId, ft);
        screenCoords.y += 12;

        // Mechanic status
        if (windowRide->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN)
        {
            switch (windowRide->mechanic_status)
            {
                case RIDE_MECHANIC_STATUS_CALLING:
                {
                    stringId = STR_NO_MECHANICS_ARE_HIRED_MESSAGE;

                    for (auto peep : EntityList<Staff>())
                    {
                        if (peep->IsMechanic())
                        {
                            stringId = STR_CALLING_MECHANIC;
                            break;
                        }
                    }
                    break;
                }
                case RIDE_MECHANIC_STATUS_HEADING:
                    stringId = STR_MEHCANIC_IS_HEADING_FOR_THE_RIDE;
                    break;
                case RIDE_MECHANIC_STATUS_FIXING:
                case RIDE_MECHANIC_STATUS_HAS_FIXED_STATION_BRAKES:
                    stringId = STR_MEHCANIC_IS_FIXING_THE_RIDE;
                    break;
                default:
                    stringId = STR_EMPTY;
                    break;
            }

            if (stringId != STR_EMPTY)
            {
                if (stringId == STR_CALLING_MECHANIC || stringId == STR_NO_MECHANICS_ARE_HIRED_MESSAGE)
                {
                    DrawTextWrapped(dpi, screenCoords, 280, stringId, {}, { TextAlignment::LEFT });
                }
                else
                {
                    auto staff = GetEntity<Staff>(windowRide->mechanic);
                    if (staff != nullptr && staff->IsMechanic())
                    {
                        ft = Formatter();
                        staff->FormatNameTo(ft);
                        DrawTextWrapped(dpi, screenCoords, 280, stringId, ft, { TextAlignment::LEFT });
                    }
                }
            }
        }
    }

#pragma endregion

#pragma region Colour

    uint32_t WindowRideGetColourButtonImage(int32_t colour)
    {
        return IMAGE_TYPE_TRANSPARENT | SPRITE_ID_PALETTE_COLOUR_1(colour) | SPR_PALETTE_BTN;
    }

    int32_t WindowRideHasTrackColour(Ride* windowRide, int32_t trackColour)
    {
        // Get station flags (shops don't have them)
        auto stationObjFlags = 0;
        if (!windowRide->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_IS_SHOP_OR_FACILITY))
        {
            auto stationObj = windowRide->GetStationObject();
            if (stationObj != nullptr)
            {
                stationObjFlags = stationObj->Flags;
            }
        }

        switch (trackColour)
        {
            case 0:
                return (stationObjFlags & STATION_OBJECT_FLAGS::HAS_PRIMARY_COLOUR)
                    || windowRide->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_HAS_TRACK_COLOUR_MAIN);
            case 1:
                return (stationObjFlags & STATION_OBJECT_FLAGS::HAS_SECONDARY_COLOUR)
                    || windowRide->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_HAS_TRACK_COLOUR_ADDITIONAL);
            case 2:
                return windowRide->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_HAS_TRACK_COLOUR_SUPPORTS);
            default:
                return 0;
        }
    }

    void WindowRideSetTrackColourScheme(const ScreenCoordsXY& screenPos)
    {
        auto newColourScheme = static_cast<uint8_t>(ride_colour);
        auto info = get_map_coordinates_from_pos(screenPos, EnumsToFlags(ViewportInteractionItem::Ride));

        if (info.SpriteType != ViewportInteractionItem::Ride)
            return;
        if (info.Element->GetType() != TileElementType::Track)
            return;
        if (info.Element->AsTrack()->GetRideIndex() != rideId)
            return;
        if (info.Element->AsTrack()->GetColourScheme() == newColourScheme)
            return;

        auto z = info.Element->GetBaseZ();
        auto direction = info.Element->GetDirection();
        auto gameAction = RideSetColourSchemeAction(
            CoordsXYZD{ info.Loc, z, static_cast<Direction>(direction) }, info.Element->AsTrack()->GetTrackType(),
            newColourScheme);
        GameActions::Execute(&gameAction);
    }

    /**
     *
     *  rct2: 0x006B04FA
     */
    void WindowRideColourClose()
    {
        if (!(input_test_flag(INPUT_FLAG_TOOL_ACTIVE)))
            return;

        if (gCurrentToolWidget.window_classification != classification)
            return;

        if (gCurrentToolWidget.window_number != number)
            return;

        tool_cancel();
    }

    /**
     *
     *  rct2: 0x006B02A1
     */
    void WindowRideColourMouseup(WidgetIndex widgetIndex)
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
            case WIDX_TAB_8:
            case WIDX_TAB_9:
            case WIDX_TAB_10:
                WindowRideSetPage(widgetIndex - WIDX_TAB_1);
                break;
            case WIDX_PAINT_INDIVIDUAL_AREA:
                tool_set(*this, WIDX_PAINT_INDIVIDUAL_AREA, Tool::PaintDown);
                break;
            case WIDX_SELL_ITEM_RANDOM_COLOUR_CHECKBOX:
                auto windowRide = get_ride(rideId);
                if (windowRide != nullptr)
                {
                    const bool currentlyEnabled = windowRide->HasLifecycleFlag(RIDE_LIFECYCLE_RANDOM_SHOP_COLOURS);
                    auto rideSetAppearanceAction = RideSetAppearanceAction(
                        rideId, RideSetAppearanceType::SellingItemColourIsRandom, currentlyEnabled ? 0 : 1, 0);
                    GameActions::Execute(&rideSetAppearanceAction);
                }
                break;
        }
    }

    /**
     *
     *  rct2: 0x006B0AB6
     */
    void WindowRideColourResize()
    {
        window_set_resize(*this, 316, 207, 316, 207);
    }

    /**
     *
     *  rct2: 0x006B02C6
     */
    void WindowRideColourMousedown(WidgetIndex widgetIndex, rct_widget* widget)
    {
        VehicleColour vehicleColour;
        int32_t i, numItems;
        StringId stringId;

        auto windowRide = get_ride(rideId);
        if (windowRide == nullptr)
            return;

        auto rideEntry = windowRide->GetRideEntry();
        if (rideEntry == nullptr)
            return;

        auto colourSchemeIndex = ride_colour;
        auto dropdownWidget = widget - 1;

        switch (widgetIndex)
        {
            case WIDX_TRACK_COLOUR_SCHEME_DROPDOWN:
                for (i = 0; i < OpenRCT2::Limits::NumColourSchemes; i++)
                {
                    gDropdownItems[i].Format = STR_DROPDOWN_MENU_LABEL;
                    gDropdownItems[i].Args = ColourSchemeNames[i];
                }

                WindowDropdownShowTextCustomWidth(
                    { windowPos.x + dropdownWidget->left, windowPos.y + dropdownWidget->top }, dropdownWidget->height() + 1,
                    colours[1], 0, Dropdown::Flag::StayOpen, 4, widget->right - dropdownWidget->left);

                Dropdown::SetChecked(colourSchemeIndex, true);
                break;
            case WIDX_TRACK_MAIN_COLOUR:
                WindowDropdownShowColour(this, widget, colours[1], windowRide->track_colour[colourSchemeIndex].main);
                break;
            case WIDX_TRACK_ADDITIONAL_COLOUR:
                WindowDropdownShowColour(this, widget, colours[1], windowRide->track_colour[colourSchemeIndex].additional);
                break;
            case WIDX_TRACK_SUPPORT_COLOUR:
                WindowDropdownShowColour(this, widget, colours[1], windowRide->track_colour[colourSchemeIndex].supports);
                break;
            case WIDX_MAZE_STYLE_DROPDOWN:
                for (i = 0; i < 4; i++)
                {
                    gDropdownItems[i].Format = STR_DROPDOWN_MENU_LABEL;
                    gDropdownItems[i].Args = MazeOptions[i].text;
                }

                WindowDropdownShowTextCustomWidth(
                    { windowPos.x + dropdownWidget->left, windowPos.y + dropdownWidget->top }, dropdownWidget->height() + 1,
                    colours[1], 0, Dropdown::Flag::StayOpen, 4, widget->right - dropdownWidget->left);

                Dropdown::SetChecked(windowRide->track_colour[colourSchemeIndex].supports, true);
                break;
            case WIDX_ENTRANCE_STYLE_DROPDOWN:
            {
                auto ddIndex = 0;
                auto& objManager = GetContext()->GetObjectManager();
                for (i = 0; i < MAX_STATION_OBJECTS; i++)
                {
                    auto stationObj = static_cast<StationObject*>(objManager.GetLoadedObject(ObjectType::Station, i));
                    if (stationObj != nullptr)
                    {
                        gDropdownItems[ddIndex].Format = STR_DROPDOWN_MENU_LABEL;
                        gDropdownItems[ddIndex].Args = stationObj->NameStringId;
                        if (windowRide->entrance_style == i)
                        {
                            gDropdownItems[ddIndex].Format = STR_DROPDOWN_MENU_LABEL_SELECTED;
                        }
                        ddIndex++;
                    }
                }

                WindowDropdownShowTextCustomWidth(
                    { windowPos.x + dropdownWidget->left, windowPos.y + dropdownWidget->top }, dropdownWidget->height() + 1,
                    colours[1], 0, Dropdown::Flag::StayOpen, ddIndex, widget->right - dropdownWidget->left);
                break;
            }
            case WIDX_VEHICLE_COLOUR_SCHEME_DROPDOWN:
                for (i = 0; i < 3; i++)
                {
                    gDropdownItems[i].Format = STR_DROPDOWN_MENU_LABEL;
                    gDropdownItems[i].Args = (GetRideComponentName(windowRide->GetRideTypeDescriptor().NameConvention.vehicle)
                                                  .singular
                                              << 16)
                        | VehicleColourSchemeNames[i];
                }

                WindowDropdownShowTextCustomWidth(
                    { windowPos.x + dropdownWidget->left, windowPos.y + dropdownWidget->top }, dropdownWidget->height() + 1,
                    colours[1], 0, Dropdown::Flag::StayOpen, rideEntry->max_cars_in_train > 1 ? 3 : 2,
                    widget->right - dropdownWidget->left);

                Dropdown::SetChecked(windowRide->colour_scheme_type & 3, true);
                break;
            case WIDX_VEHICLE_COLOUR_INDEX_DROPDOWN:
                numItems = windowRide->num_vehicles;
                if ((windowRide->colour_scheme_type & 3) != VEHICLE_COLOUR_SCHEME_PER_TRAIN)
                    numItems = windowRide->num_cars_per_train;

                stringId = (windowRide->colour_scheme_type & 3) == VEHICLE_COLOUR_SCHEME_PER_TRAIN
                    ? STR_RIDE_COLOUR_TRAIN_OPTION
                    : STR_RIDE_COLOUR_VEHICLE_OPTION;
                for (i = 0; i < std::min(numItems, Dropdown::ItemsMaxSize); i++)
                {
                    gDropdownItems[i].Format = STR_DROPDOWN_MENU_LABEL;
                    gDropdownItems[i].Args = (static_cast<int64_t>(i + 1) << 32)
                        | ((GetRideComponentName(windowRide->GetRideTypeDescriptor().NameConvention.vehicle).capitalised) << 16)
                        | stringId;
                }

                WindowDropdownShowTextCustomWidth(
                    { windowPos.x + dropdownWidget->left, windowPos.y + dropdownWidget->top }, dropdownWidget->height() + 1,
                    colours[1], 0, Dropdown::Flag::StayOpen, numItems, widget->right - dropdownWidget->left);

                Dropdown::SetChecked(vehicleIndex, true);
                break;
            case WIDX_VEHICLE_BODY_COLOR:
                vehicleColour = ride_get_vehicle_colour(windowRide, vehicleIndex);
                WindowDropdownShowColour(this, widget, colours[1], vehicleColour.Body);
                break;
            case WIDX_VEHICLE_TRIM_COLOUR:
                vehicleColour = ride_get_vehicle_colour(windowRide, vehicleIndex);
                WindowDropdownShowColour(this, widget, colours[1], vehicleColour.Trim);
                break;
            case WIDX_VEHICLE_TERNARY_COLOUR:
                vehicleColour = ride_get_vehicle_colour(windowRide, vehicleIndex);
                WindowDropdownShowColour(this, widget, colours[1], vehicleColour.Tertiary);
                break;
        }
    }

    /**
     *
     *  rct2: 0x006B0331
     */
    void WindowRideColourDropdown(WidgetIndex widgetIndex, int32_t dropdownIndex)
    {
        if (dropdownIndex == -1)
            return;

        const auto currentRideId = rideId;
        switch (widgetIndex)
        {
            case WIDX_TRACK_COLOUR_SCHEME_DROPDOWN:
                ride_colour = static_cast<uint16_t>(dropdownIndex);
                Invalidate();
                break;
            case WIDX_TRACK_MAIN_COLOUR:
            {
                auto rideSetAppearanceAction = RideSetAppearanceAction(
                    currentRideId, RideSetAppearanceType::TrackColourMain, dropdownIndex, ride_colour);
                GameActions::Execute(&rideSetAppearanceAction);
            }
            break;
            case WIDX_TRACK_ADDITIONAL_COLOUR:
            {
                auto rideSetAppearanceAction = RideSetAppearanceAction(
                    currentRideId, RideSetAppearanceType::TrackColourAdditional, dropdownIndex, ride_colour);
                GameActions::Execute(&rideSetAppearanceAction);
            }
            break;
            case WIDX_TRACK_SUPPORT_COLOUR:
            {
                auto rideSetAppearanceAction = RideSetAppearanceAction(
                    currentRideId, RideSetAppearanceType::TrackColourSupports, dropdownIndex, ride_colour);
                GameActions::Execute(&rideSetAppearanceAction);
            }
            break;
            case WIDX_MAZE_STYLE_DROPDOWN:
            {
                auto rideSetAppearanceAction = RideSetAppearanceAction(
                    currentRideId, RideSetAppearanceType::MazeStyle, dropdownIndex, ride_colour);
                GameActions::Execute(&rideSetAppearanceAction);
            }
            break;
            case WIDX_ENTRANCE_STYLE_DROPDOWN:
            {
                auto ddIndex = 0;
                auto& objManager = GetContext()->GetObjectManager();
                for (auto i = 0; i < MAX_STATION_OBJECTS; i++)
                {
                    auto stationObj = static_cast<StationObject*>(objManager.GetLoadedObject(ObjectType::Station, i));
                    if (stationObj != nullptr)
                    {
                        if (ddIndex == dropdownIndex)
                        {
                            auto rideSetAppearanceAction = RideSetAppearanceAction(
                                currentRideId, RideSetAppearanceType::EntranceStyle, ddIndex, 0);
                            rideSetAppearanceAction.SetCallback([ddIndex](const GameAction*, const GameActions::Result* res) {
                                if (res->Error != GameActions::Status::Ok)
                                    return;
                                gLastEntranceStyle = ddIndex;
                            });
                            GameActions::Execute(&rideSetAppearanceAction);
                            break;
                        }
                        ddIndex++;
                    }
                }
                break;
            }
            case WIDX_VEHICLE_COLOUR_SCHEME_DROPDOWN:
            {
                auto rideSetAppearanceAction = RideSetAppearanceAction(
                    currentRideId, RideSetAppearanceType::VehicleColourScheme, dropdownIndex, 0);
                GameActions::Execute(&rideSetAppearanceAction);
                vehicleIndex = 0;
            }
            break;
            case WIDX_VEHICLE_COLOUR_INDEX_DROPDOWN:
                vehicleIndex = dropdownIndex;
                Invalidate();
                break;
            case WIDX_VEHICLE_BODY_COLOR:
            {
                auto rideSetAppearanceAction = RideSetAppearanceAction(
                    currentRideId, RideSetAppearanceType::VehicleColourBody, dropdownIndex, vehicleIndex);
                GameActions::Execute(&rideSetAppearanceAction);
            }
            break;
            case WIDX_VEHICLE_TRIM_COLOUR:
            {
                auto rideSetAppearanceAction = RideSetAppearanceAction(
                    currentRideId, RideSetAppearanceType::VehicleColourTrim, dropdownIndex, vehicleIndex);
                GameActions::Execute(&rideSetAppearanceAction);
            }
            break;
            case WIDX_VEHICLE_TERNARY_COLOUR:
            {
                auto rideSetAppearanceAction = RideSetAppearanceAction(
                    currentRideId, RideSetAppearanceType::VehicleColourTernary, dropdownIndex, vehicleIndex);
                GameActions::Execute(&rideSetAppearanceAction);
            }
            break;
        }
    }

    /**
     *
     *  rct2: 0x006B0A8F
     */
    void WindowRideColourUpdate()
    {
        frame_no++;
        window_event_invalidate_call(this);
        widget_invalidate(*this, WIDX_TAB_5);
        widget_invalidate(*this, WIDX_VEHICLE_PREVIEW);
    }

    /**
     *
     *  rct2: 0x006B04EC
     */
    void WindowRideColourTooldown(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords)
    {
        if (widgetIndex == WIDX_PAINT_INDIVIDUAL_AREA)
            WindowRideSetTrackColourScheme(screenCoords);
    }

    /**
     *
     *  rct2: 0x006B04F3
     */
    void WindowRideColourTooldrag(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords)
    {
        if (widgetIndex == WIDX_PAINT_INDIVIDUAL_AREA)
            WindowRideSetTrackColourScheme(screenCoords);
    }

    /**
     *
     *  rct2: 0x006AFB36
     */
    void WindowRideColourInvalidate()
    {
        TrackColour trackColour;
        VehicleColour vehicleColour;

        auto newWidgets = window_ride_page_widgets[page];
        if (widgets != newWidgets)
        {
            widgets = newWidgets;
            WindowInitScrollWidgets(*this);
        }

        WindowRideSetPressedTab();

        auto windowRide = get_ride(rideId);
        if (windowRide == nullptr)
            return;

        auto rideEntry = windowRide->GetRideEntry();
        if (rideEntry == nullptr)
            return;

        widgets[WIDX_TITLE].text = STR_ARG_16_STRINGID;
        auto ft = Formatter::Common();
        ft.Increment(16);
        windowRide->FormatNameTo(ft);

        // Track colours
        int32_t colourScheme = ride_colour;
        trackColour = ride_get_track_colour(windowRide, colourScheme);

        // Maze style
        if (windowRide->type == RIDE_TYPE_MAZE)
        {
            window_ride_colour_widgets[WIDX_MAZE_STYLE].type = WindowWidgetType::DropdownMenu;
            window_ride_colour_widgets[WIDX_MAZE_STYLE_DROPDOWN].type = WindowWidgetType::Button;
            window_ride_colour_widgets[WIDX_MAZE_STYLE].text = MazeOptions[trackColour.supports].text;
        }
        else
        {
            window_ride_colour_widgets[WIDX_MAZE_STYLE].type = WindowWidgetType::Empty;
            window_ride_colour_widgets[WIDX_MAZE_STYLE_DROPDOWN].type = WindowWidgetType::Empty;
        }

        // Track, multiple colour schemes
        if (windowRide->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_SUPPORTS_MULTIPLE_TRACK_COLOUR))
        {
            window_ride_colour_widgets[WIDX_TRACK_COLOUR_SCHEME].type = WindowWidgetType::DropdownMenu;
            window_ride_colour_widgets[WIDX_TRACK_COLOUR_SCHEME_DROPDOWN].type = WindowWidgetType::Button;
            window_ride_colour_widgets[WIDX_PAINT_INDIVIDUAL_AREA].type = WindowWidgetType::FlatBtn;
        }
        else
        {
            window_ride_colour_widgets[WIDX_TRACK_COLOUR_SCHEME].type = WindowWidgetType::Empty;
            window_ride_colour_widgets[WIDX_TRACK_COLOUR_SCHEME_DROPDOWN].type = WindowWidgetType::Empty;
            window_ride_colour_widgets[WIDX_PAINT_INDIVIDUAL_AREA].type = WindowWidgetType::Empty;
        }

        // Track main colour
        if (WindowRideHasTrackColour(windowRide, 0))
        {
            window_ride_colour_widgets[WIDX_TRACK_MAIN_COLOUR].type = WindowWidgetType::ColourBtn;
            window_ride_colour_widgets[WIDX_TRACK_MAIN_COLOUR].image = WindowRideGetColourButtonImage(trackColour.main);
        }
        else
        {
            window_ride_colour_widgets[WIDX_TRACK_MAIN_COLOUR].type = WindowWidgetType::Empty;
        }

        // Track additional colour
        if (WindowRideHasTrackColour(windowRide, 1))
        {
            window_ride_colour_widgets[WIDX_TRACK_ADDITIONAL_COLOUR].type = WindowWidgetType::ColourBtn;
            window_ride_colour_widgets[WIDX_TRACK_ADDITIONAL_COLOUR].image = WindowRideGetColourButtonImage(
                trackColour.additional);
        }
        else
        {
            window_ride_colour_widgets[WIDX_TRACK_ADDITIONAL_COLOUR].type = WindowWidgetType::Empty;
        }

        // Selling item random colour checkbox
        if (windowRide->HasRecolourableShopItems())
        {
            window_ride_colour_widgets[WIDX_SELL_ITEM_RANDOM_COLOUR_CHECKBOX].type = WindowWidgetType::Checkbox;
            if (windowRide->HasLifecycleFlag(RIDE_LIFECYCLE_RANDOM_SHOP_COLOURS))
            {
                pressed_widgets |= (1ULL << WIDX_SELL_ITEM_RANDOM_COLOUR_CHECKBOX);
            }
            else
            {
                pressed_widgets &= ~(1ULL << WIDX_SELL_ITEM_RANDOM_COLOUR_CHECKBOX);
            }
        }
        else
        {
            window_ride_colour_widgets[WIDX_SELL_ITEM_RANDOM_COLOUR_CHECKBOX].type = WindowWidgetType::Empty;
        }

        // Track supports colour
        if (WindowRideHasTrackColour(windowRide, 2) && windowRide->type != RIDE_TYPE_MAZE)
        {
            window_ride_colour_widgets[WIDX_TRACK_SUPPORT_COLOUR].type = WindowWidgetType::ColourBtn;
            window_ride_colour_widgets[WIDX_TRACK_SUPPORT_COLOUR].image = WindowRideGetColourButtonImage(trackColour.supports);
        }
        else
        {
            window_ride_colour_widgets[WIDX_TRACK_SUPPORT_COLOUR].type = WindowWidgetType::Empty;
        }

        // Track preview
        if (windowRide->GetRideTypeDescriptor().HasFlag(
                RIDE_TYPE_FLAG_HAS_TRACK_COLOUR_MAIN | RIDE_TYPE_FLAG_HAS_TRACK_COLOUR_ADDITIONAL
                | RIDE_TYPE_FLAG_HAS_TRACK_COLOUR_SUPPORTS))
            window_ride_colour_widgets[WIDX_TRACK_PREVIEW].type = WindowWidgetType::Spinner;
        else
            window_ride_colour_widgets[WIDX_TRACK_PREVIEW].type = WindowWidgetType::Empty;

        // Entrance style
        if (windowRide->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_HAS_ENTRANCE_EXIT))
        {
            window_ride_colour_widgets[WIDX_ENTRANCE_PREVIEW].type = WindowWidgetType::Spinner;
            window_ride_colour_widgets[WIDX_ENTRANCE_STYLE].type = WindowWidgetType::DropdownMenu;
            window_ride_colour_widgets[WIDX_ENTRANCE_STYLE_DROPDOWN].type = WindowWidgetType::Button;

            auto stringId = STR_NONE;
            auto stationObj = windowRide->GetStationObject();
            if (stationObj != nullptr)
            {
                stringId = stationObj->NameStringId;
            }
            window_ride_colour_widgets[WIDX_ENTRANCE_STYLE].text = stringId;
        }
        else
        {
            window_ride_colour_widgets[WIDX_ENTRANCE_PREVIEW].type = WindowWidgetType::Empty;
            window_ride_colour_widgets[WIDX_ENTRANCE_STYLE].type = WindowWidgetType::Empty;
            window_ride_colour_widgets[WIDX_ENTRANCE_STYLE_DROPDOWN].type = WindowWidgetType::Empty;
        }

        // Vehicle colours
        if (!windowRide->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_NO_VEHICLES)
            && windowRide->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_HAS_VEHICLE_COLOURS))
        {
            int32_t vehicleColourSchemeType = windowRide->colour_scheme_type & 3;
            if (vehicleColourSchemeType == 0)
                vehicleIndex = 0;

            vehicleColour = ride_get_vehicle_colour(windowRide, vehicleIndex);

            window_ride_colour_widgets[WIDX_VEHICLE_PREVIEW].type = WindowWidgetType::Scroll;
            window_ride_colour_widgets[WIDX_VEHICLE_BODY_COLOR].type = WindowWidgetType::ColourBtn;
            window_ride_colour_widgets[WIDX_VEHICLE_BODY_COLOR].image = WindowRideGetColourButtonImage(vehicleColour.Body);

            bool allowChangingTrimColour = false;
            bool allowChangingTernaryColour = false;

            for (int32_t i = 0; i < windowRide->num_cars_per_train; i++)
            {
                uint8_t vehicleTypeIndex = ride_entry_get_vehicle_at_position(
                    windowRide->subtype, windowRide->num_cars_per_train, i);

                if (rideEntry->Cars[vehicleTypeIndex].flags & CAR_ENTRY_FLAG_ENABLE_TRIM_COLOUR)
                {
                    allowChangingTrimColour = true;
                }
                if (rideEntry->Cars[vehicleTypeIndex].flags & CAR_ENTRY_FLAG_ENABLE_TERNARY_COLOUR)
                {
                    allowChangingTernaryColour = true;
                }
            }

            // Additional colours
            if (allowChangingTrimColour)
            {
                window_ride_colour_widgets[WIDX_VEHICLE_TRIM_COLOUR].type = WindowWidgetType::ColourBtn;
                window_ride_colour_widgets[WIDX_VEHICLE_TRIM_COLOUR].image = WindowRideGetColourButtonImage(vehicleColour.Trim);
                if (allowChangingTernaryColour)
                {
                    window_ride_colour_widgets[WIDX_VEHICLE_TERNARY_COLOUR].type = WindowWidgetType::ColourBtn;
                    window_ride_colour_widgets[WIDX_VEHICLE_TERNARY_COLOUR].image = WindowRideGetColourButtonImage(
                        vehicleColour.Tertiary);
                }
                else
                {
                    window_ride_colour_widgets[WIDX_VEHICLE_TERNARY_COLOUR].type = WindowWidgetType::Empty;
                }
            }
            else
            {
                window_ride_colour_widgets[WIDX_VEHICLE_TRIM_COLOUR].type = WindowWidgetType::Empty;
                window_ride_colour_widgets[WIDX_VEHICLE_TERNARY_COLOUR].type = WindowWidgetType::Empty;
            }

            // Vehicle colour scheme type
            if (!windowRide->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_VEHICLE_IS_INTEGRAL)
                && (windowRide->num_cars_per_train | windowRide->num_vehicles) > 1)
            {
                window_ride_colour_widgets[WIDX_VEHICLE_COLOUR_SCHEME].type = WindowWidgetType::DropdownMenu;
                window_ride_colour_widgets[WIDX_VEHICLE_COLOUR_SCHEME_DROPDOWN].type = WindowWidgetType::Button;
            }
            else
            {
                window_ride_colour_widgets[WIDX_VEHICLE_COLOUR_SCHEME].type = WindowWidgetType::Empty;
                window_ride_colour_widgets[WIDX_VEHICLE_COLOUR_SCHEME_DROPDOWN].type = WindowWidgetType::Empty;
            }
            ft.Rewind();
            ft.Increment(6);
            ft.Add<StringId>(VehicleColourSchemeNames[vehicleColourSchemeType]);
            ft.Add<StringId>(GetRideComponentName(windowRide->GetRideTypeDescriptor().NameConvention.vehicle).singular);
            ft.Add<StringId>(GetRideComponentName(windowRide->GetRideTypeDescriptor().NameConvention.vehicle).capitalised);
            ft.Add<uint16_t>(vehicleIndex + 1);

            // Vehicle index
            if (vehicleColourSchemeType != 0)
            {
                window_ride_colour_widgets[WIDX_VEHICLE_COLOUR_INDEX].type = WindowWidgetType::DropdownMenu;
                window_ride_colour_widgets[WIDX_VEHICLE_COLOUR_INDEX_DROPDOWN].type = WindowWidgetType::Button;
                window_ride_colour_widgets[WIDX_VEHICLE_COLOUR_INDEX].text = vehicleColourSchemeType == 1
                    ? STR_RIDE_COLOUR_TRAIN_VALUE
                    : STR_RIDE_COLOUR_VEHICLE_VALUE;
            }
            else
            {
                window_ride_colour_widgets[WIDX_VEHICLE_COLOUR_INDEX].type = WindowWidgetType::Empty;
                window_ride_colour_widgets[WIDX_VEHICLE_COLOUR_INDEX_DROPDOWN].type = WindowWidgetType::Empty;
            }
        }
        else
        {
            window_ride_colour_widgets[WIDX_VEHICLE_PREVIEW].type = WindowWidgetType::Empty;
            window_ride_colour_widgets[WIDX_VEHICLE_COLOUR_SCHEME].type = WindowWidgetType::Empty;
            window_ride_colour_widgets[WIDX_VEHICLE_COLOUR_SCHEME_DROPDOWN].type = WindowWidgetType::Empty;
            window_ride_colour_widgets[WIDX_VEHICLE_COLOUR_INDEX].type = WindowWidgetType::Empty;
            window_ride_colour_widgets[WIDX_VEHICLE_COLOUR_INDEX_DROPDOWN].type = WindowWidgetType::Empty;
            window_ride_colour_widgets[WIDX_VEHICLE_BODY_COLOR].type = WindowWidgetType::Empty;
            window_ride_colour_widgets[WIDX_VEHICLE_TRIM_COLOUR].type = WindowWidgetType::Empty;
            window_ride_colour_widgets[WIDX_VEHICLE_TERNARY_COLOUR].type = WindowWidgetType::Empty;
        }

        ft.Rewind();
        ft.Increment(14);
        ft.Add<StringId>(ColourSchemeNames[colourScheme]);

        WindowRideAnchorBorderWidgets();
        window_align_tabs(this, WIDX_TAB_1, WIDX_TAB_10);
    }

    /**
     *
     *  rct2: 0x006AFF3E
     */
    void WindowRideColourPaint(rct_drawpixelinfo* dpi)
    {
        // TODO: This should use lists and identified sprites
        rct_drawpixelinfo clippedDpi;

        auto windowRide = get_ride(rideId);
        if (windowRide == nullptr)
            return;

        WindowDrawWidgets(*this, dpi);
        WindowRideDrawTabImages(dpi);

        // Track / shop item preview
        const auto& trackPreviewWidget = window_ride_colour_widgets[WIDX_TRACK_PREVIEW];
        if (trackPreviewWidget.type != WindowWidgetType::Empty)
            gfx_fill_rect(
                dpi,
                { { windowPos + ScreenCoordsXY{ trackPreviewWidget.left + 1, trackPreviewWidget.top + 1 } },
                  { windowPos + ScreenCoordsXY{ trackPreviewWidget.right - 1, trackPreviewWidget.bottom - 1 } } },
                PALETTE_INDEX_12);

        auto trackColour = ride_get_track_colour(windowRide, ride_colour);

        //
        auto rideEntry = windowRide->GetRideEntry();
        if (rideEntry == nullptr || rideEntry->shop_item[0] == ShopItem::None)
        {
            auto screenCoords = windowPos + ScreenCoordsXY{ trackPreviewWidget.left, trackPreviewWidget.top };

            // Track
            if (windowRide->type == RIDE_TYPE_MAZE)
            {
                gfx_draw_sprite(dpi, ImageId(MazeOptions[trackColour.supports].sprite), screenCoords);
            }
            else
            {
                auto typeDescriptor = windowRide->GetRideTypeDescriptor();
                int32_t spriteIndex = typeDescriptor.ColourPreview.Track;
                if (spriteIndex != 0)
                {
                    gfx_draw_sprite(dpi, ImageId(spriteIndex, trackColour.main, trackColour.additional), screenCoords);
                }

                // Supports
                spriteIndex = typeDescriptor.ColourPreview.Supports;
                if (spriteIndex != 0)
                {
                    gfx_draw_sprite(dpi, ImageId(spriteIndex, trackColour.supports), screenCoords);
                }
            }
        }
        else
        {
            auto screenCoords = windowPos
                + ScreenCoordsXY{ (trackPreviewWidget.left + trackPreviewWidget.right) / 2 - 8,
                                  (trackPreviewWidget.bottom + trackPreviewWidget.top) / 2 - 6 };

            ShopItem shopItem = rideEntry->shop_item[1] == ShopItem::None ? rideEntry->shop_item[0] : rideEntry->shop_item[1];
            if (windowRide->HasLifecycleFlag(RIDE_LIFECYCLE_RANDOM_SHOP_COLOURS))
            {
                colour_t spriteColour = COLOUR_BLACK;
                // Limit update rate of preview to avoid making people dizzy.
                if ((gCurrentTicks % 64) == 0)
                {
                    spriteColour++;
                    if (spriteColour >= COLOUR_COUNT)
                    {
                        spriteColour = COLOUR_BLACK;
                    }
                }

                gfx_draw_sprite(dpi, ImageId(GetShopItemDescriptor(shopItem).Image, spriteColour), screenCoords);
            }
            else
            {
                gfx_draw_sprite(
                    dpi, ImageId(GetShopItemDescriptor(shopItem).Image, windowRide->track_colour[0].main), screenCoords);
            }
        }

        // Entrance preview
        trackColour = ride_get_track_colour(windowRide, 0);
        const auto& entrancePreviewWidget = widgets[WIDX_ENTRANCE_PREVIEW];
        if (entrancePreviewWidget.type != WindowWidgetType::Empty)
        {
            if (clip_drawpixelinfo(
                    &clippedDpi, dpi,
                    windowPos + ScreenCoordsXY{ entrancePreviewWidget.left + 1, entrancePreviewWidget.top + 1 },
                    entrancePreviewWidget.width(), entrancePreviewWidget.height()))
            {
                gfx_clear(&clippedDpi, PALETTE_INDEX_12);

                auto stationObj = windowRide->GetStationObject();
                if (stationObj != nullptr && stationObj->BaseImageId != ImageIndexUndefined)
                {
                    auto imageId = ImageId(stationObj->BaseImageId, trackColour.main, trackColour.additional);

                    // Back
                    gfx_draw_sprite(&clippedDpi, imageId, { 34, 20 });

                    // Front
                    gfx_draw_sprite(&clippedDpi, imageId.WithIndexOffset(4), { 34, 20 });

                    // Glass
                    if (stationObj->Flags & STATION_OBJECT_FLAGS::IS_TRANSPARENT)
                    {
                        auto glassImageId = ImageId(stationObj->BaseImageId + 20).WithTransparancy(trackColour.main);
                        gfx_draw_sprite(&clippedDpi, glassImageId, { 34, 20 });
                    }
                }
            }

            DrawTextEllipsised(dpi, { windowPos.x + 3, windowPos.y + 103 }, 97, STR_STATION_STYLE, {});
        }
    }

    /**
     *
     *  rct2: 0x006B0192
     */
    void WindowRideColourScrollpaint(rct_drawpixelinfo* dpi, int32_t scrollIndex)
    {
        auto windowRide = get_ride(rideId);
        if (windowRide == nullptr)
            return;

        auto rideEntry = windowRide->GetRideEntry();
        if (rideEntry == nullptr)
            return;

        auto vehiclePreviewWidget = &window_ride_colour_widgets[WIDX_VEHICLE_PREVIEW];
        auto vehicleColour = ride_get_vehicle_colour(windowRide, vehicleIndex);

        // Background colour
        gfx_fill_rect(dpi, { { dpi->x, dpi->y }, { dpi->x + dpi->width - 1, dpi->y + dpi->height - 1 } }, PALETTE_INDEX_12);

        // ?
        auto screenCoords = ScreenCoordsXY{ vehiclePreviewWidget->width() / 2, vehiclePreviewWidget->height() - 15 };

        // ?
        auto trainCarIndex = (windowRide->colour_scheme_type & 3) == RIDE_COLOUR_SCHEME_MODE_DIFFERENT_PER_CAR
            ? vehicleIndex
            : rideEntry->TabCar;

        CarEntry* carEntry = &rideEntry->Cars[ride_entry_get_vehicle_at_position(
            windowRide->subtype, windowRide->num_cars_per_train, trainCarIndex)];

        screenCoords.y += carEntry->tab_height;

        // Draw the coloured spinning vehicle
        // frame_no represents a SpritePrecision of 64
        ImageIndex imageIndex = carEntry->SpriteByYaw(frame_no / 2, SpriteGroupType::SlopeFlat);
        imageIndex &= carEntry->TabRotationMask;
        imageIndex *= carEntry->base_num_frames;
        imageIndex += carEntry->base_image_id;
        auto imageId = ImageId(imageIndex, vehicleColour.Body, vehicleColour.Trim, vehicleColour.Tertiary);
        gfx_draw_sprite(dpi, imageId, screenCoords);
    }

#pragma endregion

#pragma region Music
    std::vector<ObjectEntryIndex> window_ride_current_music_style_order;

    /**
     *
     *  rct2: 0x006B215D
     */
    void WindowRideToggleMusic()
    {
        auto windowRide = get_ride(rideId);
        if (windowRide != nullptr)
        {
            int32_t activateMusic = (windowRide->lifecycle_flags & RIDE_LIFECYCLE_MUSIC) ? 0 : 1;
            set_operating_setting(rideId, RideSetSetting::Music, activateMusic);
        }
    }

    /**
     *
     *  rct2: 0x006B1ED7
     */
    void WindowRideMusicMouseup(WidgetIndex widgetIndex)
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
            case WIDX_TAB_8:
            case WIDX_TAB_9:
            case WIDX_TAB_10:
                WindowRideSetPage(widgetIndex - WIDX_TAB_1);
                break;
            case WIDX_PLAY_MUSIC:
                WindowRideToggleMusic();
                break;
        }
    }

    /**
     *
     *  rct2: 0x006AF4A2
     */
    void WindowRideMusicResize()
    {
        flags |= WF_RESIZABLE;
        window_set_resize(*this, 316, 81, 316, 81);
    }

    std::string GetMusicString(ObjectEntryIndex musicObjectIndex)
    {
        auto& objManager = GetContext()->GetObjectManager();
        auto musicObj = static_cast<MusicObject*>(objManager.GetLoadedObject(ObjectType::Music, musicObjectIndex));

        auto name = musicObj->NameStringId;
        return format_string(name, {});
    }

    /**
     *
     *  rct2: 0x006B1EFC
     */
    void WindowRideMusicMousedown(WidgetIndex widgetIndex, rct_widget* widget)
    {
        if (widgetIndex != WIDX_MUSIC_DROPDOWN)
            return;

        auto dropdownWidget = widget - 1;
        auto windowRide = get_ride(rideId);
        if (windowRide == nullptr)
            return;

        // Construct list of available music
        auto& musicOrder = window_ride_current_music_style_order;
        musicOrder.clear();
        auto& objManager = GetContext()->GetObjectManager();
        for (ObjectEntryIndex i = 0; i < MAX_MUSIC_OBJECTS; i++)
        {
            auto musicObj = static_cast<MusicObject*>(objManager.GetLoadedObject(ObjectType::Music, i));
            if (musicObj != nullptr)
            {
                // Hide custom music if the WAV file does not exist
                auto originalStyleId = musicObj->GetOriginalStyleId();
                if (originalStyleId.has_value()
                    && (originalStyleId == MUSIC_STYLE_CUSTOM_MUSIC_1 || originalStyleId == MUSIC_STYLE_CUSTOM_MUSIC_2))
                {
                    auto numTracks = musicObj->GetTrackCount();
                    if (numTracks != 0)
                    {
                        auto track0 = musicObj->GetTrack(0);
                        if (!track0->Asset.IsAvailable())
                        {
                            continue;
                        }
                    }
                    else
                    {
                        continue;
                    }
                }

                if (gCheatsUnlockOperatingLimits || musicObj->SupportsRideType(windowRide->type))
                {
                    musicOrder.push_back(i);
                }
            }
        }

        // Sort available music by the alphabetical order
        std::stable_sort(musicOrder.begin(), musicOrder.end(), [this](const ObjectEntryIndex& a, const ObjectEntryIndex& b) {
            return String::Compare(GetMusicString(b), GetMusicString(a), false) > 0;
        });

        // Setup dropdown list
        auto numItems = musicOrder.size();
        for (size_t i = 0; i < numItems; i++)
        {
            auto musicObj = static_cast<MusicObject*>(objManager.GetLoadedObject(ObjectType::Music, musicOrder[i]));
            gDropdownItems[i].Format = STR_DROPDOWN_MENU_LABEL;
            gDropdownItems[i].Args = musicObj->NameStringId;
        }

        WindowDropdownShowTextCustomWidth(
            { windowPos.x + dropdownWidget->left, windowPos.y + dropdownWidget->top }, dropdownWidget->height() + 1, colours[1],
            0, Dropdown::Flag::StayOpen, numItems, widget->right - dropdownWidget->left);

        // Set currently checked item
        for (size_t i = 0; i < numItems; i++)
        {
            if (musicOrder[i] == windowRide->music)
            {
                Dropdown::SetChecked(static_cast<int32_t>(i), true);
            }
        }
    }

    /**
     *
     *  rct2: 0x006B1F03
     */
    void WindowRideMusicDropdown(WidgetIndex widgetIndex, int32_t dropdownIndex)
    {
        if (widgetIndex == WIDX_MUSIC_DROPDOWN && dropdownIndex >= 0
            && static_cast<size_t>(dropdownIndex) < window_ride_current_music_style_order.size())
        {
            auto musicStyle = window_ride_current_music_style_order[dropdownIndex];
            set_operating_setting(rideId, RideSetSetting::MusicType, musicStyle);
        }
    }

    /**
     *
     *  rct2: 0x006B2198
     */
    void WindowRideMusicUpdate()
    {
        frame_no++;
        window_event_invalidate_call(this);
        widget_invalidate(*this, WIDX_TAB_6);
    }

    /**
     *
     *  rct2: 0x006B1DEA
     */
    void WindowRideMusicInvalidate()
    {
        auto newWidgets = window_ride_page_widgets[page];
        if (widgets != newWidgets)
        {
            widgets = newWidgets;
            WindowInitScrollWidgets(*this);
        }

        WindowRideSetPressedTab();

        auto windowRide = get_ride(rideId);
        if (windowRide == nullptr)
            return;

        auto ft = Formatter::Common();
        windowRide->FormatNameTo(ft);

        // Set selected music
        StringId musicName = STR_NONE;
        auto& objManager = GetContext()->GetObjectManager();
        auto musicObj = static_cast<MusicObject*>(objManager.GetLoadedObject(ObjectType::Music, windowRide->music));
        if (musicObj != nullptr)
        {
            musicName = musicObj->NameStringId;
        }
        window_ride_music_widgets[WIDX_MUSIC].text = musicName;

        // Set music activated
        auto isMusicActivated = (windowRide->lifecycle_flags & RIDE_LIFECYCLE_MUSIC) != 0;
        if (isMusicActivated)
        {
            pressed_widgets |= (1ULL << WIDX_PLAY_MUSIC);
            disabled_widgets &= ~(1ULL << WIDX_MUSIC);
            disabled_widgets &= ~(1ULL << WIDX_MUSIC_DROPDOWN);
        }
        else
        {
            pressed_widgets &= ~(1ULL << WIDX_PLAY_MUSIC);
            disabled_widgets |= (1ULL << WIDX_MUSIC);
            disabled_widgets |= (1ULL << WIDX_MUSIC_DROPDOWN);
        }

        WindowRideAnchorBorderWidgets();
        window_align_tabs(this, WIDX_TAB_1, WIDX_TAB_10);
    }

    /**
     *
     *  rct2: 0x006B1ECC
     */
    void WindowRideMusicPaint(rct_drawpixelinfo* dpi)
    {
        WindowDrawWidgets(*this, dpi);
        WindowRideDrawTabImages(dpi);
    }

#pragma endregion

#pragma region Measurements

    StringId GetRatingName(ride_rating rating)
    {
        int32_t index = std::clamp<int32_t>(rating >> 8, 0, static_cast<int32_t>(std::size(RatingNames)) - 1);
        return RatingNames[index];
    }

    /**
     *
     *  rct2: 0x006D27A3
     */
    void SetupScenerySelection()
    {
        if (gTrackDesignSaveMode)
        {
            CancelScenerySelection();
        }

        while (tool_set(*this, WIDX_BACKGROUND, Tool::Crosshair))
            ;

        gTrackDesignSaveRideIndex = rideId;

        track_design_save_init();
        gGamePaused |= GAME_PAUSED_SAVING_TRACK;
        gTrackDesignSaveMode = true;

        OpenRCT2::Audio::StopAll();

        rct_window* w_main = window_get_main();
        if (w_main != nullptr)
        {
            w_main->viewport->flags |= (VIEWPORT_FLAG_HIDE_VERTICAL | VIEWPORT_FLAG_HIDE_BASE);
        }

        gfx_invalidate_screen();
    }

    /**
     *
     *  rct2: 0x006AD4DA
     */
    void WindowRideMeasurementsClose()
    {
        WindowRideMeasurementsDesignCancel();
    }

    /**
     *
     *  rct2: 0x006D3026
     */
    void WindowRideMeasurementsDesignReset()
    {
        track_design_save_reset_scenery();
    }

    /**
     *
     *  rct2: 0x006D303D
     */
    void WindowRideMeasurementsDesignSelectNearbyScenery()
    {
        track_design_save_select_nearby_scenery(gTrackDesignSaveRideIndex);
    }

    /**
     *
     *  rct2: 0x006AD4CD
     */
    void WindowRideMeasurementsDesignSave()
    {
        TrackDesignState tds{};

        Ride* windowRide = get_ride(rideId);
        _trackDesign = windowRide->SaveToTrackDesign(tds);
        if (!_trackDesign)
        {
            return;
        }

        if (gTrackDesignSaveMode)
        {
            auto errMessage = _trackDesign->CreateTrackDesignScenery(tds);
            if (errMessage != STR_NONE)
            {
                context_show_error(STR_CANT_SAVE_TRACK_DESIGN, errMessage, {});
                return;
            }
        }

        auto trackName = windowRide->GetName();
        auto intent = Intent(WindowClass::Loadsave);
        intent.putExtra(INTENT_EXTRA_LOADSAVE_TYPE, LOADSAVETYPE_SAVE | LOADSAVETYPE_TRACK);
        intent.putExtra(INTENT_EXTRA_TRACK_DESIGN, _trackDesign.get());
        intent.putExtra(INTENT_EXTRA_PATH, trackName);
        intent.putExtra(INTENT_EXTRA_CALLBACK, reinterpret_cast<void*>(&TrackDesignCallback));

        context_open_intent(&intent);
    }

    /**
     *
     *  rct2: 0x006AD478
     */
    void WindowRideMeasurementsMouseup(WidgetIndex widgetIndex)
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
            case WIDX_TAB_8:
            case WIDX_TAB_9:
            case WIDX_TAB_10:
                WindowRideSetPage(widgetIndex - WIDX_TAB_1);
                break;
            case WIDX_SELECT_NEARBY_SCENERY:
                WindowRideMeasurementsDesignSelectNearbyScenery();
                break;
            case WIDX_RESET_SELECTION:
                WindowRideMeasurementsDesignReset();
                break;
            case WIDX_SAVE_DESIGN:
                WindowRideMeasurementsDesignSave();
                break;
            case WIDX_CANCEL_DESIGN:
                WindowRideMeasurementsDesignCancel();
                break;
        }
    }

    /**
     *
     *  rct2: 0x006AD564
     */
    void WindowRideMeasurementsResize()
    {
        window_set_resize(*this, 316, 234, 316, 234);
    }

    /**
     *
     *  rct2: 0x006AD4AB
     */
    void WindowRideMeasurementsMousedown(WidgetIndex widgetIndex, rct_widget* widget)
    {
        if (widgetIndex != WIDX_SAVE_TRACK_DESIGN)
            return;

        auto windowRide = get_ride(rideId);
        if (windowRide == nullptr)
            return;

        gDropdownItems[0].Format = STR_SAVE_TRACK_DESIGN_ITEM;
        gDropdownItems[1].Format = STR_SAVE_TRACK_DESIGN_WITH_SCENERY_ITEM;

        WindowDropdownShowText(
            { windowPos.x + widget->left, windowPos.y + widget->top }, widget->height() + 1, colours[1],
            Dropdown::Flag::StayOpen, 2);
        gDropdownDefaultIndex = 0;
        if (!windowRide->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_HAS_TRACK))
        {
            // Disable saving without scenery if we're a flat ride
            Dropdown::SetDisabled(0, true);
            gDropdownDefaultIndex = 1;
        }
    }

    /**
     *
     *  rct2: 0x006AD4B2
     */
    void WindowRideMeasurementsDropdown(WidgetIndex widgetIndex, int32_t dropdownIndex)
    {
        if (widgetIndex != WIDX_SAVE_TRACK_DESIGN)
            return;

        if (dropdownIndex == -1)
            dropdownIndex = gDropdownHighlightedIndex;

        if (dropdownIndex == 0)
        {
            WindowRideMeasurementsDesignSave();
        }
        else
            SetupScenerySelection();
    }

    /**
     *
     *  rct2: 0x006AD5DD
     */
    void WindowRideMeasurementsUpdate()
    {
        frame_no++;
        window_event_invalidate_call(this);
        widget_invalidate(*this, WIDX_TAB_7);
    }

    /**
     *
     *  rct2: 0x006D2AE7
     */
    void WindowRideMeasurementsTooldown(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords)
    {
        _lastSceneryX = screenCoords.x;
        _lastSceneryY = screenCoords.y;
        _collectTrackDesignScenery = true; // Default to true in case user does not select anything valid

        constexpr auto windowFlags = EnumsToFlags(
            ViewportInteractionItem::Scenery, ViewportInteractionItem::Footpath, ViewportInteractionItem::Wall,
            ViewportInteractionItem::LargeScenery);
        auto info = get_map_coordinates_from_pos(screenCoords, windowFlags);
        switch (info.SpriteType)
        {
            case ViewportInteractionItem::Scenery:
            case ViewportInteractionItem::LargeScenery:
            case ViewportInteractionItem::Wall:
            case ViewportInteractionItem::Footpath:
                _collectTrackDesignScenery = !track_design_save_contains_tile_element(info.Element);
                track_design_save_select_tile_element(info.SpriteType, info.Loc, info.Element, _collectTrackDesignScenery);
                break;
            default:
                break;
        }
    }

    void WindowRideMeasurementsTooldrag(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords)
    {
        if (screenCoords.x == _lastSceneryX && screenCoords.y == _lastSceneryY)
            return;
        _lastSceneryX = screenCoords.x;
        _lastSceneryY = screenCoords.y;

        auto windowFlags = EnumsToFlags(
            ViewportInteractionItem::Scenery, ViewportInteractionItem::Footpath, ViewportInteractionItem::Wall,
            ViewportInteractionItem::LargeScenery);
        auto info = get_map_coordinates_from_pos(screenCoords, windowFlags);
        switch (info.SpriteType)
        {
            case ViewportInteractionItem::Scenery:
            case ViewportInteractionItem::LargeScenery:
            case ViewportInteractionItem::Wall:
            case ViewportInteractionItem::Footpath:
                track_design_save_select_tile_element(info.SpriteType, info.Loc, info.Element, _collectTrackDesignScenery);
                break;
            default:
                break;
        }
    }

    /**
     *
     *  rct2: 0x006AD4DA
     */
    void WindowRideMeasurementsToolabort(WidgetIndex widgetIndex)
    {
        WindowRideMeasurementsDesignCancel();
    }

    /**
     *
     *  rct2: 0x006ACDBC
     */
    void WindowRideMeasurementsInvalidate()
    {
        auto newWidgets = window_ride_page_widgets[page];
        if (widgets != newWidgets)
        {
            widgets = newWidgets;
            WindowInitScrollWidgets(*this);
        }

        WindowRideSetPressedTab();

        const auto currentRideId = rideId;
        auto windowRide = get_ride(currentRideId);
        if (windowRide == nullptr)
            return;

        auto ft = Formatter::Common();
        windowRide->FormatNameTo(ft);

        window_ride_measurements_widgets[WIDX_SAVE_TRACK_DESIGN].tooltip = STR_SAVE_TRACK_DESIGN_NOT_POSSIBLE;
        window_ride_measurements_widgets[WIDX_SAVE_TRACK_DESIGN].type = WindowWidgetType::Empty;
        if (gTrackDesignSaveMode && gTrackDesignSaveRideIndex == currentRideId)
        {
            window_ride_measurements_widgets[WIDX_SELECT_NEARBY_SCENERY].type = WindowWidgetType::Button;
            window_ride_measurements_widgets[WIDX_RESET_SELECTION].type = WindowWidgetType::Button;
            window_ride_measurements_widgets[WIDX_SAVE_DESIGN].type = WindowWidgetType::Button;
            window_ride_measurements_widgets[WIDX_CANCEL_DESIGN].type = WindowWidgetType::Button;
        }
        else
        {
            window_ride_measurements_widgets[WIDX_SELECT_NEARBY_SCENERY].type = WindowWidgetType::Empty;
            window_ride_measurements_widgets[WIDX_RESET_SELECTION].type = WindowWidgetType::Empty;
            window_ride_measurements_widgets[WIDX_SAVE_DESIGN].type = WindowWidgetType::Empty;
            window_ride_measurements_widgets[WIDX_CANCEL_DESIGN].type = WindowWidgetType::Empty;

            window_ride_measurements_widgets[WIDX_SAVE_TRACK_DESIGN].type = WindowWidgetType::FlatBtn;
            disabled_widgets |= (1ULL << WIDX_SAVE_TRACK_DESIGN);
            if (windowRide->lifecycle_flags & RIDE_LIFECYCLE_TESTED)
            {
                if (windowRide->excitement != RIDE_RATING_UNDEFINED)
                {
                    disabled_widgets &= ~(1ULL << WIDX_SAVE_TRACK_DESIGN);
                    window_ride_measurements_widgets[WIDX_SAVE_TRACK_DESIGN].tooltip = STR_SAVE_TRACK_DESIGN;
                }
            }
        }

        WindowRideAnchorBorderWidgets();
        window_align_tabs(this, WIDX_TAB_1, WIDX_TAB_10);
    }

    /**
     *
     *  rct2: 0x006ACF07
     */
    void WindowRideMeasurementsPaint(rct_drawpixelinfo* dpi)
    {
        WindowDrawWidgets(*this, dpi);
        WindowRideDrawTabImages(dpi);

        if (window_ride_measurements_widgets[WIDX_SAVE_DESIGN].type == WindowWidgetType::Button)
        {
            rct_widget* widget = &window_ride_measurements_widgets[WIDX_PAGE_BACKGROUND];

            ScreenCoordsXY widgetCoords(windowPos.x + widget->width() / 2, windowPos.y + widget->top + 40);
            DrawTextWrapped(dpi, widgetCoords, width - 8, STR_CLICK_ITEMS_OF_SCENERY_TO_SELECT, {}, { TextAlignment::CENTRE });

            widgetCoords.x = windowPos.x + 4;
            widgetCoords.y = windowPos.y + window_ride_measurements_widgets[WIDX_SELECT_NEARBY_SCENERY].bottom + 17;
            gfx_fill_rect_inset(
                dpi, { widgetCoords, { windowPos.x + 312, widgetCoords.y + 1 } }, colours[1], INSET_RECT_FLAG_BORDER_INSET);
        }
        else
        {
            auto windowRide = get_ride(rideId);
            if (windowRide == nullptr)
                return;

            auto screenCoords = windowPos
                + ScreenCoordsXY{ window_ride_measurements_widgets[WIDX_PAGE_BACKGROUND].left + 4,
                                  window_ride_measurements_widgets[WIDX_PAGE_BACKGROUND].top + 4 };

            if (windowRide->lifecycle_flags & RIDE_LIFECYCLE_TESTED)
            {
                // Excitement
                StringId ratingName = GetRatingName(windowRide->excitement);
                auto ft = Formatter();
                ft.Add<uint32_t>(windowRide->excitement);
                ft.Add<StringId>(ratingName);
                StringId stringId = !ride_has_ratings(windowRide) ? STR_EXCITEMENT_RATING_NOT_YET_AVAILABLE
                                                                  : STR_EXCITEMENT_RATING;
                DrawTextBasic(dpi, screenCoords, stringId, ft);
                screenCoords.y += LIST_ROW_HEIGHT;

                // Intensity
                ratingName = GetRatingName(windowRide->intensity);
                ft = Formatter();
                ft.Add<uint32_t>(windowRide->intensity);
                ft.Add<StringId>(ratingName);

                stringId = STR_INTENSITY_RATING;
                if (!ride_has_ratings(windowRide))
                    stringId = STR_INTENSITY_RATING_NOT_YET_AVAILABLE;
                else if (windowRide->intensity >= RIDE_RATING(10, 00))
                    stringId = STR_INTENSITY_RATING_RED;

                DrawTextBasic(dpi, screenCoords, stringId, ft);
                screenCoords.y += LIST_ROW_HEIGHT;

                // Nausea
                ratingName = GetRatingName(windowRide->nausea);
                ft = Formatter();
                ft.Add<uint32_t>(windowRide->nausea);
                ft.Add<StringId>(ratingName);
                stringId = !ride_has_ratings(windowRide) ? STR_NAUSEA_RATING_NOT_YET_AVAILABLE : STR_NAUSEA_RATING;
                DrawTextBasic(dpi, screenCoords, stringId, ft);
                screenCoords.y += 2 * LIST_ROW_HEIGHT;

                // Horizontal rule
                gfx_fill_rect_inset(
                    dpi, { screenCoords - ScreenCoordsXY{ 0, 6 }, screenCoords + ScreenCoordsXY{ 303, -5 } }, colours[1],
                    INSET_RECT_FLAG_BORDER_INSET);

                if (!(windowRide->lifecycle_flags & RIDE_LIFECYCLE_NO_RAW_STATS))
                {
                    if (windowRide->type == RIDE_TYPE_MINI_GOLF)
                    {
                        // Holes
                        ft = Formatter();
                        ft.Add<uint16_t>(windowRide->holes);
                        DrawTextBasic(dpi, screenCoords, STR_HOLES, ft);
                        screenCoords.y += LIST_ROW_HEIGHT;
                    }
                    else
                    {
                        // Max speed
                        ft = Formatter();
                        ft.Add<int32_t>((windowRide->max_speed * 9) >> 18);
                        DrawTextBasic(dpi, screenCoords, STR_MAX_SPEED, ft);
                        screenCoords.y += LIST_ROW_HEIGHT;

                        // Average speed
                        ft = Formatter();
                        ft.Add<int32_t>((windowRide->average_speed * 9) >> 18);
                        DrawTextBasic(dpi, screenCoords, STR_AVERAGE_SPEED, ft);
                        screenCoords.y += LIST_ROW_HEIGHT;

                        // Ride time
                        ft = Formatter();
                        int32_t numTimes = 0;
                        for (int32_t i = 0; i < windowRide->num_stations; i++)
                        {
                            StationIndex stationIndex = StationIndex::FromUnderlying(numTimes);
                            auto time = windowRide->GetStation(stationIndex).SegmentTime;
                            if (time != 0)
                            {
                                ft.Add<uint16_t>(STR_RIDE_TIME_ENTRY_WITH_SEPARATOR);
                                ft.Add<uint16_t>(time);
                                numTimes++;
                            }
                        }
                        if (numTimes == 0)
                        {
                            ft.Add<uint16_t>(STR_RIDE_TIME_ENTRY);
                            ft.Add<uint16_t>(0);
                            numTimes++;
                        }
                        else
                        {
                            // sadly, STR_RIDE_TIME_ENTRY_WITH_SEPARATOR are defined with the separator AFTER an entry
                            // therefore we set the last entry to use the no-separator format now, post-format
                            ft.Rewind();
                            ft.Increment((numTimes - 1) * 4);
                            ft.Add<uint16_t>(STR_RIDE_TIME_ENTRY);
                        }
                        ft.Rewind();
                        ft.Increment(numTimes * 4);
                        ft.Add<uint16_t>(0);
                        ft.Add<uint16_t>(0);
                        ft.Add<uint16_t>(0);
                        ft.Add<uint16_t>(0);
                        DrawTextEllipsised(dpi, screenCoords, 308, STR_RIDE_TIME, ft);
                        screenCoords.y += LIST_ROW_HEIGHT;
                    }

                    // Ride length
                    ft = Formatter();
                    int32_t numLengths = 0;
                    for (int32_t i = 0; i < windowRide->num_stations; i++)
                    {
                        StationIndex stationIndex = StationIndex::FromUnderlying(i);
                        auto length = windowRide->GetStation(stationIndex).SegmentLength;
                        if (length != 0)
                        {
                            length >>= 16;
                            ft.Add<StringId>(STR_RIDE_LENGTH_ENTRY_WITH_SEPARATOR);
                            ft.Add<uint16_t>(length & 0xFFFF);
                            numLengths++;
                        }
                    }
                    if (numLengths == 0)
                    {
                        ft.Add<StringId>(STR_RIDE_LENGTH_ENTRY);
                        ft.Add<uint16_t>(0);
                        numLengths++;
                    }
                    else
                    {
                        // sadly, STR_RIDE_LENGTH_ENTRY_WITH_SEPARATOR are defined with the separator AFTER an entry
                        // therefore we set the last entry to use the no-separator format now, post-format
                        ft.Rewind();
                        ft.Increment((numLengths - 1) * 4);
                        ft.Add<StringId>(STR_RIDE_LENGTH_ENTRY);
                    }
                    ft.Rewind();
                    ft.Increment(numLengths * 4);
                    ft.Add<uint16_t>(0);
                    ft.Add<uint16_t>(0);
                    ft.Add<uint16_t>(0);
                    ft.Add<uint16_t>(0);
                    DrawTextEllipsised(dpi, screenCoords, 308, STR_RIDE_LENGTH, ft);

                    screenCoords.y += LIST_ROW_HEIGHT;

                    if (windowRide->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_HAS_G_FORCES))
                    {
                        // Max. positive vertical G's
                        stringId = STR_MAX_POSITIVE_VERTICAL_G;

                        ft = Formatter();
                        ft.Add<fixed16_2dp>(windowRide->max_positive_vertical_g);
                        DrawTextBasic(dpi, screenCoords, stringId, ft);
                        screenCoords.y += LIST_ROW_HEIGHT;

                        // Max. negative vertical G's
                        stringId = windowRide->max_negative_vertical_g <= RIDE_G_FORCES_RED_NEG_VERTICAL
                            ? STR_MAX_NEGATIVE_VERTICAL_G_RED
                            : STR_MAX_NEGATIVE_VERTICAL_G;
                        ft = Formatter();
                        ft.Add<int32_t>(windowRide->max_negative_vertical_g);
                        DrawTextBasic(dpi, screenCoords, stringId, ft);
                        screenCoords.y += LIST_ROW_HEIGHT;

                        // Max lateral G's
                        stringId = windowRide->max_lateral_g > RIDE_G_FORCES_RED_LATERAL ? STR_MAX_LATERAL_G_RED
                                                                                         : STR_MAX_LATERAL_G;
                        ft = Formatter();
                        ft.Add<fixed16_2dp>(windowRide->max_lateral_g);
                        DrawTextBasic(dpi, screenCoords, stringId, ft);
                        screenCoords.y += LIST_ROW_HEIGHT;

                        // Total 'air' time
                        ft = Formatter();
                        ft.Add<fixed32_2dp>(windowRide->total_air_time * 3);
                        DrawTextBasic(dpi, screenCoords, STR_TOTAL_AIR_TIME, ft);
                        screenCoords.y += LIST_ROW_HEIGHT;
                    }

                    if (windowRide->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_HAS_DROPS))
                    {
                        // Drops
                        auto drops = windowRide->drops & 0x3F;
                        ft = Formatter();
                        ft.Add<uint16_t>(drops);
                        DrawTextBasic(dpi, screenCoords, STR_DROPS, ft);
                        screenCoords.y += LIST_ROW_HEIGHT;

                        // Highest drop height
                        auto highestDropHeight = (windowRide->highest_drop_height * 3) / 4;
                        ft = Formatter();
                        ft.Add<int32_t>(highestDropHeight);
                        DrawTextBasic(dpi, screenCoords, STR_HIGHEST_DROP_HEIGHT, ft);
                        screenCoords.y += LIST_ROW_HEIGHT;
                    }

                    if (windowRide->type != RIDE_TYPE_MINI_GOLF)
                    {
                        // Inversions
                        if (windowRide->inversions != 0)
                        {
                            ft = Formatter();
                            ft.Add<uint16_t>(windowRide->inversions);
                            DrawTextBasic(dpi, screenCoords, STR_INVERSIONS, ft);
                            screenCoords.y += LIST_ROW_HEIGHT;
                        }
                    }
                }
            }
            else
            {
                DrawTextBasic(dpi, screenCoords, STR_NO_TEST_RESULTS_YET);
            }
        }
    }

#pragma endregion

#pragma region Graphs

    enum
    {
        GRAPH_VELOCITY,
        GRAPH_ALTITUDE,
        GRAPH_VERTICAL,
        GRAPH_LATERAL
    };

    /**
     *
     *  rct2: 0x006AE8A6
     */
    void WindowRideSetGraph(int32_t type)
    {
        if ((list_information_type & 0xFF) == type)
        {
            list_information_type ^= 0x8000;
        }
        else
        {
            list_information_type &= 0xFF00;
            list_information_type |= type;
        }
        Invalidate();
    }

    /**
     *
     *  rct2: 0x006AE85D
     */
    void WindowRideGraphsMouseup(WidgetIndex widgetIndex)
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
            case WIDX_TAB_8:
            case WIDX_TAB_9:
            case WIDX_TAB_10:
                WindowRideSetPage(widgetIndex - WIDX_TAB_1);
                break;
        }
    }

    /**
     *
     *  rct2: 0x006AE8DA
     */
    void WindowRideGraphsResize()
    {
        window_set_resize(*this, 316, 182, 500, 450);
    }

    /**
     *
     *  rct2: 0x006AE878
     */
    void WindowRideGraphsMousedown(WidgetIndex widgetIndex, rct_widget* widget)
    {
        switch (widgetIndex)
        {
            case WIDX_GRAPH_VELOCITY:
                WindowRideSetGraph(GRAPH_VELOCITY);
                break;
            case WIDX_GRAPH_ALTITUDE:
                WindowRideSetGraph(GRAPH_ALTITUDE);
                break;
            case WIDX_GRAPH_VERTICAL:
                WindowRideSetGraph(GRAPH_VERTICAL);
                break;
            case WIDX_GRAPH_LATERAL:
                WindowRideSetGraph(GRAPH_LATERAL);
                break;
        }
    }

    /**
     *
     *  rct2: 0x006AE95D
     */
    void WindowRideGraphsUpdate()
    {
        rct_widget* widget;
        int32_t x;

        frame_no++;
        window_event_invalidate_call(this);
        widget_invalidate(*this, WIDX_TAB_8);
        window_event_invalidate_call(this);
        widget_invalidate(*this, WIDX_GRAPH);

        widget = &window_ride_graphs_widgets[WIDX_GRAPH];
        x = scrolls[0].h_left;
        if (!(list_information_type & 0x8000))
        {
            auto windowRide = get_ride(rideId);
            if (windowRide != nullptr)
            {
                RideMeasurement* measurement{};
                std::tie(measurement, std::ignore) = windowRide->GetMeasurement();
                x = measurement == nullptr ? 0 : measurement->current_item - ((widget->width() / 4) * 3);
            }
        }

        scrolls[0].h_left = std::clamp(x, 0, scrolls[0].h_right - (widget->width() - 2));
        WidgetScrollUpdateThumbs(*this, WIDX_GRAPH);
    }

    /**
     *
     *  rct2: 0x006AEA75
     */
    ScreenSize WindowRideGraphsScrollgetheight(int32_t scrollIndex)
    {
        window_event_invalidate_call(this);

        // Set minimum size
        auto newWidth = window_ride_graphs_widgets[WIDX_GRAPH].width() - 2;

        // Get measurement size
        auto windowRide = get_ride(rideId);
        if (windowRide != nullptr)
        {
            RideMeasurement* measurement{};
            std::tie(measurement, std::ignore) = windowRide->GetMeasurement();
            if (measurement != nullptr)
            {
                newWidth = std::max<int32_t>(newWidth, measurement->num_items);
            }
        }

        return { newWidth, 0 };
    }

    /**
     *
     *  rct2: 0x006AE953
     */
    void WindowRideGraphs15(int32_t scrollIndex, int32_t scrollAreaType)
    {
        list_information_type |= 0x8000;
    }

    /**
     *
     *  rct2: 0x006AEA05
     */
    OpenRCT2String WindowRideGraphsTooltip(const WidgetIndex widgetIndex, const StringId fallback)
    {
        if (widgetIndex == WIDX_GRAPH)
        {
            auto windowRide = get_ride(rideId);
            if (windowRide != nullptr)
            {
                auto [measurement, message] = windowRide->GetMeasurement();
                if (measurement != nullptr && (measurement->flags & RIDE_MEASUREMENT_FLAG_RUNNING))
                {
                    auto ft = Formatter();
                    ft.Increment(2);
                    ft.Add<StringId>(GetRideComponentName(windowRide->GetRideTypeDescriptor().NameConvention.vehicle).number);
                    ft.Add<uint16_t>(measurement->vehicle_index + 1);
                    return { fallback, ft };
                }

                return message;
            }
        }
        else
        {
            return { STR_NONE, {} };
        }
        return { fallback, {} };
    }

    /**
     *
     *  rct2: 0x006AE372
     */
    void WindowRideGraphsInvalidate()
    {
        auto newWidgets = window_ride_page_widgets[page];
        if (widgets != newWidgets)
        {
            widgets = newWidgets;
            WindowInitScrollWidgets(*this);
        }

        WindowRideSetPressedTab();

        auto windowRide = get_ride(rideId);
        if (windowRide == nullptr)
            return;

        auto ft = Formatter::Common();
        windowRide->FormatNameTo(ft);

        // Set pressed graph button type
        pressed_widgets &= ~(1ULL << WIDX_GRAPH_VELOCITY);
        pressed_widgets &= ~(1ULL << WIDX_GRAPH_ALTITUDE);
        pressed_widgets &= ~(1ULL << WIDX_GRAPH_VERTICAL);
        pressed_widgets &= ~(1ULL << WIDX_GRAPH_LATERAL);
        pressed_widgets |= (1LL << (WIDX_GRAPH_VELOCITY + (list_information_type & 0xFF)));

        // Hide graph buttons that are not applicable
        if (windowRide->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_HAS_G_FORCES))
        {
            window_ride_graphs_widgets[WIDX_GRAPH_VERTICAL].type = WindowWidgetType::Button;
            window_ride_graphs_widgets[WIDX_GRAPH_LATERAL].type = WindowWidgetType::Button;
        }
        else
        {
            window_ride_graphs_widgets[WIDX_GRAPH_VERTICAL].type = WindowWidgetType::Empty;
            window_ride_graphs_widgets[WIDX_GRAPH_LATERAL].type = WindowWidgetType::Empty;
        }

        // Anchor graph widget
        auto x = width - 4;
        auto y = height - BUTTON_FACE_HEIGHT - 8;

        window_ride_graphs_widgets[WIDX_GRAPH].right = x;
        window_ride_graphs_widgets[WIDX_GRAPH].bottom = y;
        y += 3;
        window_ride_graphs_widgets[WIDX_GRAPH_VELOCITY].top = y;
        window_ride_graphs_widgets[WIDX_GRAPH_ALTITUDE].top = y;
        window_ride_graphs_widgets[WIDX_GRAPH_VERTICAL].top = y;
        window_ride_graphs_widgets[WIDX_GRAPH_LATERAL].top = y;
        y += BUTTON_FACE_HEIGHT + 1;
        window_ride_graphs_widgets[WIDX_GRAPH_VELOCITY].bottom = y;
        window_ride_graphs_widgets[WIDX_GRAPH_ALTITUDE].bottom = y;
        window_ride_graphs_widgets[WIDX_GRAPH_VERTICAL].bottom = y;
        window_ride_graphs_widgets[WIDX_GRAPH_LATERAL].bottom = y;

        WindowRideAnchorBorderWidgets();
        window_align_tabs(this, WIDX_TAB_1, WIDX_TAB_10);
    }

    /**
     *
     *  rct2: 0x006AE4BC
     */
    void WindowRideGraphsPaint(rct_drawpixelinfo* dpi)
    {
        WindowDrawWidgets(*this, dpi);
        WindowRideDrawTabImages(dpi);
    }

    /**
     *
     *  rct2: 0x006AE4C7
     */
    void WindowRideGraphsScrollpaint(rct_drawpixelinfo* dpi, int32_t scrollIndex)
    {
        gfx_clear(dpi, ColourMapA[COLOUR_SATURATED_GREEN].darker);

        auto widget = &window_ride_graphs_widgets[WIDX_GRAPH];
        auto windowRide = get_ride(rideId);
        if (windowRide == nullptr)
        {
            return;
        }

        auto [measurement, message] = windowRide->GetMeasurement();

        if (measurement == nullptr)
        {
            // No measurement message
            ScreenCoordsXY stringCoords(widget->width() / 2, widget->height() / 2 - 5);
            int32_t newWidth = widget->width() - 2;
            DrawTextWrapped(dpi, stringCoords, newWidth, message.str, message.args, { TextAlignment::CENTRE });
            return;
        }

        // Vertical grid lines
        const uint8_t lightColour = ColourMapA[COLOUR_SATURATED_GREEN].mid_light;
        const uint8_t darkColour = ColourMapA[COLOUR_SATURATED_GREEN].mid_dark;

        int32_t time = 0;
        for (int32_t x = 0; x < dpi->x + dpi->width; x += 80)
        {
            if (x + 80 >= dpi->x)
            {
                auto coord1 = ScreenCoordsXY{ x, dpi->y };
                auto coord2 = ScreenCoordsXY{ x, dpi->y + dpi->height - 1 };
                gfx_fill_rect(dpi, { coord1, coord2 }, lightColour);
                gfx_fill_rect(dpi, { coord1 + ScreenCoordsXY{ 16, 0 }, coord2 + ScreenCoordsXY{ 16, 0 } }, darkColour);
                gfx_fill_rect(dpi, { coord1 + ScreenCoordsXY{ 32, 0 }, coord2 + ScreenCoordsXY{ 32, 0 } }, darkColour);
                gfx_fill_rect(dpi, { coord1 + ScreenCoordsXY{ 48, 0 }, coord2 + ScreenCoordsXY{ 48, 0 } }, darkColour);
                gfx_fill_rect(dpi, { coord1 + ScreenCoordsXY{ 64, 0 }, coord2 + ScreenCoordsXY{ 64, 0 } }, darkColour);
            }
            time += 5;
        }

        // Horizontal grid lines
        int32_t listType = list_information_type & 0xFF;
        int16_t yUnit = window_graphs_y_axi[listType].unit;
        StringId stringID = window_graphs_y_axi[listType].label;
        int16_t yUnitInterval = window_graphs_y_axi[listType].unit_interval;
        int16_t yInterval = window_graphs_y_axi[listType].interval;

        // Scale modifier
        if (listType == GRAPH_ALTITUDE)
        {
            yUnit -= gMapBaseZ * 3;
        }

        for (int32_t y = widget->height() - 13; y >= 8; y -= yInterval, yUnit += yUnitInterval)
        {
            // Minor / major line
            int32_t colour = yUnit == 0 ? lightColour : darkColour;
            gfx_fill_rect(dpi, { { dpi->x, y }, { dpi->x + dpi->width - 1, y } }, colour);

            int16_t scaled_yUnit = yUnit;
            // Scale modifier
            if (listType == GRAPH_ALTITUDE)
                scaled_yUnit /= 2;

            auto ft = Formatter();
            ft.Add<int16_t>(scaled_yUnit);

            DrawTextBasic(dpi, { scrolls[0].h_left + 1, y - 4 }, stringID, ft, { FontSpriteBase::SMALL });
        }

        // Time marks
        time = 0;
        for (int32_t x = 0; x < dpi->x + dpi->width; x += 80)
        {
            auto ft = Formatter();
            ft.Add<int32_t>(time);
            if (x + 80 >= dpi->x)
                DrawTextBasic(dpi, { x + 2, 1 }, STR_RIDE_STATS_TIME, ft, { FontSpriteBase::SMALL });
            time += 5;
        }

        // Plot
        int32_t x = dpi->x;
        int32_t firstPoint, secondPoint;
        // Uses the force limits (used to draw extreme G's in red on measurement tab) to determine if line should be drawn red.
        int32_t intensityThresholdPositive = 0;
        int32_t intensityThresholdNegative = 0;
        for (int32_t newWidth = 0; newWidth < dpi->width; newWidth++, x++)
        {
            if (x < 0 || x >= measurement->num_items - 1)
                continue;

            constexpr int32_t VerticalGraphHeightOffset = 39;
            constexpr int32_t LateralGraphHeightOffset = 52;

            switch (listType)
            {
                case GRAPH_VELOCITY:
                    firstPoint = measurement->velocity[x] / 2;
                    secondPoint = measurement->velocity[x + 1] / 2;
                    break;
                case GRAPH_ALTITUDE:
                    firstPoint = measurement->altitude[x];
                    secondPoint = measurement->altitude[x + 1];
                    break;
                case GRAPH_VERTICAL:
                    firstPoint = measurement->vertical[x] + VerticalGraphHeightOffset;
                    secondPoint = measurement->vertical[x + 1] + VerticalGraphHeightOffset;
                    intensityThresholdNegative = (RIDE_G_FORCES_RED_NEG_VERTICAL / 8) + VerticalGraphHeightOffset;
                    break;
                case GRAPH_LATERAL:
                    firstPoint = measurement->lateral[x] + LateralGraphHeightOffset;
                    secondPoint = measurement->lateral[x + 1] + LateralGraphHeightOffset;
                    intensityThresholdPositive = (RIDE_G_FORCES_RED_LATERAL / 8) + LateralGraphHeightOffset;
                    intensityThresholdNegative = -(RIDE_G_FORCES_RED_LATERAL / 8) + LateralGraphHeightOffset;
                    break;
                default:
                    log_error("Wrong graph type %d", listType);
                    firstPoint = secondPoint = 0;
                    break;
            }

            // Adjust line to match graph widget position.
            firstPoint = widget->height() - firstPoint - 13;
            secondPoint = widget->height() - secondPoint - 13;
            if (firstPoint > secondPoint)
            {
                std::swap(firstPoint, secondPoint);
            }

            // Adjust threshold line position as well
            if (listType == GRAPH_VERTICAL || listType == GRAPH_LATERAL)
            {
                intensityThresholdPositive = widget->height() - intensityThresholdPositive - 13;
                intensityThresholdNegative = widget->height() - intensityThresholdNegative - 13;
            }

            const bool previousMeasurement = x > measurement->current_item;

            // Draw the current line in grey.
            gfx_fill_rect(
                dpi, { { x, firstPoint }, { x, secondPoint } }, previousMeasurement ? PALETTE_INDEX_17 : PALETTE_INDEX_21);

            // Draw red over extreme values (if supported by graph type).
            if (listType == GRAPH_VERTICAL || listType == GRAPH_LATERAL)
            {
                const auto redLineColour = previousMeasurement ? PALETTE_INDEX_171 : PALETTE_INDEX_173;

                // Line exceeds negative threshold (at bottom of graph).
                if (secondPoint >= intensityThresholdNegative)
                {
                    const auto redLineTop = ScreenCoordsXY{ x, std::max(firstPoint, intensityThresholdNegative) };
                    const auto redLineBottom = ScreenCoordsXY{ x, std::max(secondPoint, intensityThresholdNegative) };
                    gfx_fill_rect(dpi, { redLineTop, redLineBottom }, redLineColour);
                }

                // Line exceeds positive threshold (at top of graph).
                if (listType == GRAPH_LATERAL && firstPoint < intensityThresholdPositive)
                {
                    const auto redLineTop = ScreenCoordsXY{ x, std::min(firstPoint, intensityThresholdPositive) };
                    const auto redLineBottom = ScreenCoordsXY{ x, std::min(secondPoint, intensityThresholdPositive) };
                    gfx_fill_rect(dpi, { redLineTop, redLineBottom }, redLineColour);
                }
            }
        }
    }

#pragma endregion

#pragma region Income

    void UpdateSamePriceThroughoutFlags(ShopItem shop_item)
    {
        uint64_t newFlags;

        if (GetShopItemDescriptor(shop_item).IsPhoto())
        {
            newFlags = gSamePriceThroughoutPark;
            newFlags ^= EnumsToFlags(ShopItem::Photo, ShopItem::Photo2, ShopItem::Photo3, ShopItem::Photo4);
            auto parkSetParameter = ParkSetParameterAction(ParkParameter::SamePriceInPark, newFlags);
            GameActions::Execute(&parkSetParameter);
        }
        else
        {
            newFlags = gSamePriceThroughoutPark;
            newFlags ^= EnumToFlag(shop_item);
            auto parkSetParameter = ParkSetParameterAction(ParkParameter::SamePriceInPark, newFlags);
            GameActions::Execute(&parkSetParameter);
        }
    }

    /**
     *
     *  rct2: 0x006ADEFD
     */
    void WindowRideIncomeTogglePrimaryPrice()
    {
        const auto currentRideId = rideId;
        auto windowRide = get_ride(currentRideId);
        if (windowRide == nullptr)
            return;

        ShopItem shop_item;
        const auto& rtd = windowRide->GetRideTypeDescriptor();
        if (rtd.HasFlag(RIDE_TYPE_FLAG_IS_TOILET))
        {
            shop_item = ShopItem::Admission;
        }
        else
        {
            auto rideEntry = get_ride_entry(windowRide->subtype);
            if (rideEntry != nullptr)
            {
                shop_item = rideEntry->shop_item[0];
                if (shop_item == ShopItem::None)
                    return;
            }
            else
            {
                return;
            }
        }

        UpdateSamePriceThroughoutFlags(shop_item);

        auto rideSetPriceAction = RideSetPriceAction(currentRideId, windowRide->price[0], true);
        GameActions::Execute(&rideSetPriceAction);
    }

    /**
     *
     *  rct2: 0x006AE06E
     */
    void WindowRideIncomeToggleSecondaryPrice()
    {
        const auto currentRideId = rideId;
        auto windowRide = get_ride(currentRideId);
        if (windowRide == nullptr)
            return;

        auto rideEntry = get_ride_entry(windowRide->subtype);
        if (rideEntry == nullptr)
            return;

        auto shop_item = rideEntry->shop_item[1];
        if (shop_item == ShopItem::None)
            shop_item = windowRide->GetRideTypeDescriptor().PhotoItem;

        UpdateSamePriceThroughoutFlags(shop_item);

        auto rideSetPriceAction = RideSetPriceAction(currentRideId, windowRide->price[1], false);
        GameActions::Execute(&rideSetPriceAction);
    }

    void WindowRideIncomeSetPrimaryPrice(money16 price)
    {
        auto rideSetPriceAction = RideSetPriceAction(rideId, price, true);
        GameActions::Execute(&rideSetPriceAction);
    }

    /**
     *
     *  rct2: 0x006AE1E4
     */
    void WindowRideIncomeIncreasePrimaryPrice()
    {
        if (!WindowRideIncomeCanModifyPrimaryPrice())
            return;

        auto windowRide = get_ride(rideId);
        if (windowRide == nullptr)
            return;

        money16 price = windowRide->price[0];
        if (price < 20.00_GBP)
            price++;

        WindowRideIncomeSetPrimaryPrice(price);
    }

    /**
     *
     *  rct2: 0x006AE237
     */
    void WindowRideIncomeDecreasePrimaryPrice()
    {
        if (!WindowRideIncomeCanModifyPrimaryPrice())
            return;

        auto windowRide = get_ride(rideId);
        if (windowRide == nullptr)
            return;

        money16 price = windowRide->price[0];
        if (price > 0.00_GBP)
            price--;

        WindowRideIncomeSetPrimaryPrice(price);
    }

    money16 WindowRideIncomeGetSecondaryPrice()
    {
        auto windowRide = get_ride(rideId);
        if (windowRide == nullptr)
            return 0;

        money16 price = windowRide->price[1];
        return price;
    }

    void WindowRideIncomeSetSecondaryPrice(money16 price)
    {
        auto rideSetPriceAction = RideSetPriceAction(rideId, price, false);
        GameActions::Execute(&rideSetPriceAction);
    }

    bool WindowRideIncomeCanModifyPrimaryPrice()
    {
        auto windowRide = get_ride(rideId);
        if (windowRide == nullptr)
            return false;

        auto rideEntry = windowRide->GetRideEntry();
        const auto& rtd = windowRide->GetRideTypeDescriptor();
        return ParkRidePricesUnlocked() || rtd.HasFlag(RIDE_TYPE_FLAG_IS_TOILET)
            || (rideEntry != nullptr && rideEntry->shop_item[0] != ShopItem::None);
    }

    /**
     *
     *  rct2: 0x006AE269
     */
    void WindowRideIncomeIncreaseSecondaryPrice()
    {
        money16 price = WindowRideIncomeGetSecondaryPrice();

        if (price < 20.00_GBP)
            price++;

        WindowRideIncomeSetSecondaryPrice(price);
    }

    /**
     *
     *  rct2: 0x006AE28D
     */
    void WindowRideIncomeDecreaseSecondaryPrice()
    {
        money16 price = WindowRideIncomeGetSecondaryPrice();

        if (price > 0.00_GBP)
            price--;

        WindowRideIncomeSetSecondaryPrice(price);
    }

    /**
     *
     *  rct2: 0x006ADEA9
     */
    void WindowRideIncomeMouseup(WidgetIndex widgetIndex)
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
            case WIDX_TAB_8:
            case WIDX_TAB_9:
            case WIDX_TAB_10:
                WindowRideSetPage(widgetIndex - WIDX_TAB_1);
                break;
            case WIDX_PRIMARY_PRICE:
            {
                if (!WindowRideIncomeCanModifyPrimaryPrice())
                    return;

                auto windowRide = get_ride(rideId);
                if (windowRide != nullptr)
                {
                    money_to_string(static_cast<money32>(windowRide->price[0]), _moneyInputText, MONEY_STRING_MAXLENGTH, true);
                    WindowTextInputRawOpen(
                        this, WIDX_PRIMARY_PRICE, STR_ENTER_NEW_VALUE, STR_ENTER_NEW_VALUE, {}, _moneyInputText,
                        MONEY_STRING_MAXLENGTH);
                }
                break;
            }
            case WIDX_PRIMARY_PRICE_SAME_THROUGHOUT_PARK:
                WindowRideIncomeTogglePrimaryPrice();
                break;
            case WIDX_SECONDARY_PRICE:
            {
                money32 price32 = static_cast<money32>(WindowRideIncomeGetSecondaryPrice());

                money_to_string(price32, _moneyInputText, MONEY_STRING_MAXLENGTH, true);
                WindowTextInputRawOpen(
                    this, WIDX_SECONDARY_PRICE, STR_ENTER_NEW_VALUE, STR_ENTER_NEW_VALUE, {}, _moneyInputText,
                    MONEY_STRING_MAXLENGTH);
            }
            break;
            case WIDX_SECONDARY_PRICE_SAME_THROUGHOUT_PARK:
                WindowRideIncomeToggleSecondaryPrice();
                break;
        }
    }

    /**
     *
     *  rct2: 0x006AE2F8
     */
    void WindowRideIncomeResize()
    {
        window_set_resize(*this, 316, 194, 316, 194);
    }

    /**
     *
     *  rct2: 0x006ADED4
     */
    void WindowRideIncomeMousedown(WidgetIndex widgetIndex, rct_widget* widget)
    {
        switch (widgetIndex)
        {
            case WIDX_PRIMARY_PRICE_INCREASE:
                WindowRideIncomeIncreasePrimaryPrice();
                break;
            case WIDX_PRIMARY_PRICE_DECREASE:
                WindowRideIncomeDecreasePrimaryPrice();
                break;
            case WIDX_SECONDARY_PRICE_INCREASE:
                WindowRideIncomeIncreaseSecondaryPrice();
                break;
            case WIDX_SECONDARY_PRICE_DECREASE:
                WindowRideIncomeDecreaseSecondaryPrice();
                break;
        }
    }

    /**
     *
     *  rct2: 0x006AE2BF
     */
    void WindowRideIncomeUpdate()
    {
        frame_no++;
        window_event_invalidate_call(this);
        widget_invalidate(*this, WIDX_TAB_9);

        auto windowRide = get_ride(rideId);
        if (windowRide != nullptr && windowRide->window_invalidate_flags & RIDE_INVALIDATE_RIDE_INCOME)
        {
            windowRide->window_invalidate_flags &= ~RIDE_INVALIDATE_RIDE_INCOME;
            Invalidate();
        }
    }

    void WindowRideIncomeTextinput(WidgetIndex widgetIndex, std::string_view text)
    {
        if ((widgetIndex != WIDX_PRIMARY_PRICE && widgetIndex != WIDX_SECONDARY_PRICE) || text.empty())
            return;

        money32 price = string_to_money(std::string(text).c_str());
        if (price == MONEY32_UNDEFINED)
        {
            return;
        }

        price = std::clamp(price, 0.00_GBP, 20.00_GBP);
        money16 price16 = static_cast<money16>(price);

        if (widgetIndex == WIDX_PRIMARY_PRICE)
        {
            WindowRideIncomeSetPrimaryPrice(price16);
        }
        else
        {
            WindowRideIncomeSetSecondaryPrice(price16);
        }
    }

    /**
     *
     *  rct2: 0x006ADAA3
     */
    void WindowRideIncomeInvalidate()
    {
        auto newWidgets = window_ride_page_widgets[page];
        if (widgets != newWidgets)
        {
            widgets = newWidgets;
            WindowInitScrollWidgets(*this);
        }

        WindowRideSetPressedTab();

        auto windowRide = get_ride(rideId);
        if (windowRide == nullptr)
            return;

        widgets[WIDX_TITLE].text = STR_ARG_18_STRINGID;

        auto ft = Formatter::Common();
        ft.Increment(18);
        windowRide->FormatNameTo(ft);

        auto rideEntry = windowRide->GetRideEntry();
        if (rideEntry == nullptr)
            return;

        // Primary item
        pressed_widgets &= ~(1ULL << WIDX_PRIMARY_PRICE_SAME_THROUGHOUT_PARK);
        disabled_widgets &= ~(1ULL << WIDX_PRIMARY_PRICE);

        window_ride_income_widgets[WIDX_PRIMARY_PRICE_LABEL].tooltip = STR_NONE;
        window_ride_income_widgets[WIDX_PRIMARY_PRICE].tooltip = STR_NONE;

        // If ride prices are locked, do not allow setting the price, unless we're dealing with a shop or toilet.
        const auto& rtd = windowRide->GetRideTypeDescriptor();
        if (!ParkRidePricesUnlocked() && rideEntry->shop_item[0] == ShopItem::None && !rtd.HasFlag(RIDE_TYPE_FLAG_IS_TOILET))
        {
            disabled_widgets |= (1ULL << WIDX_PRIMARY_PRICE);
            window_ride_income_widgets[WIDX_PRIMARY_PRICE_LABEL].tooltip = STR_RIDE_INCOME_ADMISSION_PAY_FOR_ENTRY_TIP;
            window_ride_income_widgets[WIDX_PRIMARY_PRICE].tooltip = STR_RIDE_INCOME_ADMISSION_PAY_FOR_ENTRY_TIP;
        }

        window_ride_income_widgets[WIDX_PRIMARY_PRICE_LABEL].text = STR_RIDE_INCOME_ADMISSION_PRICE;
        window_ride_income_widgets[WIDX_SECONDARY_PRICE_LABEL].text = STR_SHOP_ITEM_PRICE_LABEL_ON_RIDE_PHOTO;
        window_ride_income_widgets[WIDX_PRIMARY_PRICE_SAME_THROUGHOUT_PARK].type = WindowWidgetType::Empty;

        window_ride_income_widgets[WIDX_PRIMARY_PRICE].text = STR_BOTTOM_TOOLBAR_CASH;
        money16 ridePrimaryPrice = ride_get_price(windowRide);
        ft.Rewind();
        ft.Add<money64>(ridePrimaryPrice);
        if (ridePrimaryPrice == 0)
            window_ride_income_widgets[WIDX_PRIMARY_PRICE].text = STR_FREE;

        ShopItem primaryItem = ShopItem::Admission;
        if (rtd.HasFlag(RIDE_TYPE_FLAG_IS_TOILET) || ((primaryItem = rideEntry->shop_item[0]) != ShopItem::None))
        {
            window_ride_income_widgets[WIDX_PRIMARY_PRICE_SAME_THROUGHOUT_PARK].type = WindowWidgetType::Checkbox;

            if (shop_item_has_common_price(primaryItem))
                pressed_widgets |= (1ULL << WIDX_PRIMARY_PRICE_SAME_THROUGHOUT_PARK);

            window_ride_income_widgets[WIDX_PRIMARY_PRICE_LABEL].text = GetShopItemDescriptor(primaryItem).Naming.PriceLabel;
        }

        // Get secondary item
        auto secondaryItem = windowRide->GetRideTypeDescriptor().PhotoItem;
        if (!(windowRide->lifecycle_flags & RIDE_LIFECYCLE_ON_RIDE_PHOTO))
        {
            if ((secondaryItem = rideEntry->shop_item[1]) != ShopItem::None)
            {
                window_ride_income_widgets[WIDX_SECONDARY_PRICE_LABEL].text = GetShopItemDescriptor(secondaryItem)
                                                                                  .Naming.PriceLabel;
            }
        }

        if (secondaryItem == ShopItem::None)
        {
            // Hide secondary item widgets
            window_ride_income_widgets[WIDX_SECONDARY_PRICE_LABEL].type = WindowWidgetType::Empty;
            window_ride_income_widgets[WIDX_SECONDARY_PRICE].type = WindowWidgetType::Empty;
            window_ride_income_widgets[WIDX_SECONDARY_PRICE_INCREASE].type = WindowWidgetType::Empty;
            window_ride_income_widgets[WIDX_SECONDARY_PRICE_DECREASE].type = WindowWidgetType::Empty;
            window_ride_income_widgets[WIDX_SECONDARY_PRICE_SAME_THROUGHOUT_PARK].type = WindowWidgetType::Empty;
        }
        else
        {
            // Set same price throughout park checkbox
            pressed_widgets &= ~(1ULL << WIDX_SECONDARY_PRICE_SAME_THROUGHOUT_PARK);
            if (shop_item_has_common_price(secondaryItem))
                pressed_widgets |= (1ULL << WIDX_SECONDARY_PRICE_SAME_THROUGHOUT_PARK);

            // Show widgets
            window_ride_income_widgets[WIDX_SECONDARY_PRICE_LABEL].type = WindowWidgetType::Label;
            window_ride_income_widgets[WIDX_SECONDARY_PRICE].type = WindowWidgetType::Spinner;
            window_ride_income_widgets[WIDX_SECONDARY_PRICE_INCREASE].type = WindowWidgetType::Button;
            window_ride_income_widgets[WIDX_SECONDARY_PRICE_DECREASE].type = WindowWidgetType::Button;
            window_ride_income_widgets[WIDX_SECONDARY_PRICE_SAME_THROUGHOUT_PARK].type = WindowWidgetType::Checkbox;

            // Set secondary item price
            window_ride_income_widgets[WIDX_SECONDARY_PRICE].text = STR_RIDE_SECONDARY_PRICE_VALUE;
            ft.Rewind();
            ft.Increment(10);
            ft.Add<money64>(windowRide->price[1]);
            if (windowRide->price[1] == 0)
                window_ride_income_widgets[WIDX_SECONDARY_PRICE].text = STR_FREE;
        }

        WindowRideAnchorBorderWidgets();
        window_align_tabs(this, WIDX_TAB_1, WIDX_TAB_10);
    }

    /**
     *
     *  rct2: 0x006ADCE5
     */
    void WindowRideIncomePaint(rct_drawpixelinfo* dpi)
    {
        StringId stringId;
        money64 profit;
        ShopItem primaryItem, secondaryItem;

        WindowDrawWidgets(*this, dpi);
        WindowRideDrawTabImages(dpi);

        auto windowRide = get_ride(rideId);
        if (windowRide == nullptr)
            return;

        auto rideEntry = windowRide->GetRideEntry();
        if (rideEntry == nullptr)
            return;

        auto screenCoords = windowPos
            + ScreenCoordsXY{ window_ride_income_widgets[WIDX_PAGE_BACKGROUND].left + 4,
                              window_ride_income_widgets[WIDX_PAGE_BACKGROUND].top + 33 };

        // Primary item profit / loss per item sold
        primaryItem = rideEntry->shop_item[0];
        if (primaryItem != ShopItem::None)
        {
            profit = windowRide->price[0];

            stringId = STR_PROFIT_PER_ITEM_SOLD;
            profit -= GetShopItemDescriptor(primaryItem).Cost;
            if (profit < 0)
            {
                profit *= -1;
                stringId = STR_LOSS_PER_ITEM_SOLD;
            }

            auto ft = Formatter();
            ft.Add<money64>(profit);

            DrawTextBasic(dpi, screenCoords, stringId, ft);
        }
        screenCoords.y += 44;

        // Secondary item profit / loss per item sold
        secondaryItem = windowRide->GetRideTypeDescriptor().PhotoItem;
        if (!(windowRide->lifecycle_flags & RIDE_LIFECYCLE_ON_RIDE_PHOTO))
            secondaryItem = rideEntry->shop_item[1];

        if (secondaryItem != ShopItem::None)
        {
            profit = windowRide->price[1];

            stringId = STR_PROFIT_PER_ITEM_SOLD;
            profit -= GetShopItemDescriptor(secondaryItem).Cost;
            if (profit < 0)
            {
                profit *= -1;
                stringId = STR_LOSS_PER_ITEM_SOLD;
            }

            auto ft = Formatter();
            ft.Add<money64>(profit);

            DrawTextBasic(dpi, screenCoords, stringId, ft);
        }
        screenCoords.y += 18;

        // Income per hour
        if (windowRide->income_per_hour != MONEY64_UNDEFINED)
        {
            auto ft = Formatter();
            ft.Add<money64>(windowRide->income_per_hour);

            DrawTextBasic(dpi, screenCoords, STR_INCOME_PER_HOUR, ft);
            screenCoords.y += LIST_ROW_HEIGHT;
        }

        // Running cost per hour
        money64 costPerHour = windowRide->upkeep_cost * 16;
        stringId = windowRide->upkeep_cost == MONEY16_UNDEFINED ? STR_RUNNING_COST_UNKNOWN : STR_RUNNING_COST_PER_HOUR;
        auto ft = Formatter();
        ft.Add<money64>(costPerHour);
        DrawTextBasic(dpi, screenCoords, stringId, ft);
        screenCoords.y += LIST_ROW_HEIGHT;

        // Profit per hour
        if (windowRide->profit != MONEY64_UNDEFINED)
        {
            ft = Formatter();
            ft.Add<money64>(windowRide->profit);
            DrawTextBasic(dpi, screenCoords, STR_PROFIT_PER_HOUR, ft);
            screenCoords.y += LIST_ROW_HEIGHT;
        }
        screenCoords.y += 5;

        // Total profit
        ft = Formatter();
        ft.Add<money64>(windowRide->total_profit);
        DrawTextBasic(dpi, screenCoords, STR_TOTAL_PROFIT, ft);
    }

#pragma endregion

#pragma region Customer

    /**
     *
     *  rct2: 0x006AD986
     */
    void WindowRideCustomerMouseup(WidgetIndex widgetIndex)
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
            case WIDX_TAB_8:
            case WIDX_TAB_9:
            case WIDX_TAB_10:
                WindowRideSetPage(widgetIndex - WIDX_TAB_1);
                break;
            case WIDX_SHOW_GUESTS_THOUGHTS:
            {
                auto intent = Intent(WindowClass::GuestList);
                intent.putExtra(
                    INTENT_EXTRA_GUEST_LIST_FILTER, static_cast<int32_t>(GuestListFilterType::GuestsThinkingAboutRide));
                intent.putExtra(INTENT_EXTRA_RIDE_ID, number);
                context_open_intent(&intent);
                break;
            }
            case WIDX_SHOW_GUESTS_ON_RIDE:
            {
                auto intent = Intent(WindowClass::GuestList);
                intent.putExtra(INTENT_EXTRA_GUEST_LIST_FILTER, static_cast<int32_t>(GuestListFilterType::GuestsOnRide));
                intent.putExtra(INTENT_EXTRA_RIDE_ID, number);
                context_open_intent(&intent);
                break;
            }
            case WIDX_SHOW_GUESTS_QUEUING:
            {
                auto intent = Intent(WindowClass::GuestList);
                intent.putExtra(INTENT_EXTRA_GUEST_LIST_FILTER, static_cast<int32_t>(GuestListFilterType::GuestsInQueue));
                intent.putExtra(INTENT_EXTRA_RIDE_ID, number);
                context_open_intent(&intent);
                break;
            }
        }
    }

    /**
     *
     *  rct2: 0x006ADA29
     */
    void WindowRideCustomerResize()
    {
        flags |= WF_RESIZABLE;
        window_set_resize(*this, 316, 163, 316, 163);
    }

    /**
     *
     *  rct2: 0x006AD9DD
     */
    void WindowRideCustomerUpdate()
    {
        picked_peep_frame++;
        if (picked_peep_frame >= 24)
            picked_peep_frame = 0;

        window_event_invalidate_call(this);
        widget_invalidate(*this, WIDX_TAB_10);

        auto windowRide = get_ride(rideId);
        if (windowRide != nullptr && windowRide->window_invalidate_flags & RIDE_INVALIDATE_RIDE_CUSTOMER)
        {
            windowRide->window_invalidate_flags &= ~RIDE_INVALIDATE_RIDE_CUSTOMER;
            Invalidate();
        }
    }

    /**
     *
     *  rct2: 0x006AD5F8
     */
    void WindowRideCustomerInvalidate()
    {
        auto newWidgets = window_ride_page_widgets[page];
        if (widgets != newWidgets)
        {
            widgets = newWidgets;
            WindowInitScrollWidgets(*this);
        }

        WindowRideSetPressedTab();

        auto windowRide = get_ride(rideId);
        if (windowRide != nullptr)
        {
            auto ft = Formatter::Common();
            windowRide->FormatNameTo(ft);

            window_ride_customer_widgets[WIDX_SHOW_GUESTS_THOUGHTS].type = WindowWidgetType::FlatBtn;
            if (windowRide->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_IS_SHOP_OR_FACILITY))
            {
                window_ride_customer_widgets[WIDX_SHOW_GUESTS_ON_RIDE].type = WindowWidgetType::Empty;
                window_ride_customer_widgets[WIDX_SHOW_GUESTS_QUEUING].type = WindowWidgetType::Empty;
            }
            else
            {
                window_ride_customer_widgets[WIDX_SHOW_GUESTS_ON_RIDE].type = WindowWidgetType::FlatBtn;
                window_ride_customer_widgets[WIDX_SHOW_GUESTS_QUEUING].type = WindowWidgetType::FlatBtn;
            }

            WindowRideAnchorBorderWidgets();
            window_align_tabs(this, WIDX_TAB_1, WIDX_TAB_10);
        }
    }

    /**
     *
     *  rct2: 0x006AD6CD
     */
    void WindowRideCustomerPaint(rct_drawpixelinfo* dpi)
    {
        ShopItem shopItem;
        int16_t popularity, satisfaction, queueTime;
        StringId stringId;

        WindowDrawWidgets(*this, dpi);
        WindowRideDrawTabImages(dpi);

        auto windowRide = get_ride(rideId);
        if (windowRide == nullptr)
            return;

        auto screenCoords = windowPos
            + ScreenCoordsXY{ window_ride_customer_widgets[WIDX_PAGE_BACKGROUND].left + 4,
                              window_ride_customer_widgets[WIDX_PAGE_BACKGROUND].top + 4 };

        // Customers currently on ride
        if (windowRide->IsRide())
        {
            auto ft = Formatter();
            ft.Add<int16_t>(windowRide->num_riders);
            DrawTextBasic(dpi, screenCoords, STR_CUSTOMERS_ON_RIDE, ft);
            screenCoords.y += LIST_ROW_HEIGHT;
        }

        // Customers per hour
        auto ft = Formatter();
        ft.Add<int32_t>(ride_customers_per_hour(windowRide));
        DrawTextBasic(dpi, screenCoords, STR_CUSTOMERS_PER_HOUR, ft);
        screenCoords.y += LIST_ROW_HEIGHT;

        // Popularity
        popularity = windowRide->popularity;
        if (popularity == 255)
        {
            stringId = STR_POPULARITY_UNKNOWN;
        }
        else
        {
            stringId = STR_POPULARITY_PERCENT;
            popularity *= 4;
        }
        ft = Formatter();
        ft.Add<int16_t>(popularity);
        DrawTextBasic(dpi, screenCoords, stringId, ft);
        screenCoords.y += LIST_ROW_HEIGHT;

        // Satisfaction
        satisfaction = windowRide->satisfaction;
        if (satisfaction == 255)
        {
            stringId = STR_SATISFACTION_UNKNOWN;
        }
        else
        {
            stringId = STR_SATISFACTION_PERCENT;
            satisfaction *= 5;
        }
        ft = Formatter();
        ft.Add<int16_t>(satisfaction);
        DrawTextBasic(dpi, screenCoords, stringId, ft);
        screenCoords.y += LIST_ROW_HEIGHT;

        // Queue time
        if (windowRide->IsRide())
        {
            queueTime = windowRide->GetMaxQueueTime();
            stringId = queueTime == 1 ? STR_QUEUE_TIME_MINUTE : STR_QUEUE_TIME_MINUTES;
            ft = Formatter();
            ft.Add<int32_t>(queueTime);
            screenCoords.y += DrawTextWrapped(dpi, screenCoords, 308, stringId, ft, { TextAlignment::LEFT });
            screenCoords.y += 5;
        }

        // Primary shop items sold
        shopItem = windowRide->GetRideEntry()->shop_item[0];
        if (shopItem != ShopItem::None)
        {
            ft = Formatter();
            ft.Add<StringId>(GetShopItemDescriptor(shopItem).Naming.Plural);
            ft.Add<uint32_t>(windowRide->no_primary_items_sold);
            DrawTextBasic(dpi, screenCoords, STR_ITEMS_SOLD, ft);
            screenCoords.y += LIST_ROW_HEIGHT;
        }

        // Secondary shop items sold / on-ride photos sold
        shopItem = (windowRide->lifecycle_flags & RIDE_LIFECYCLE_ON_RIDE_PHOTO) ? windowRide->GetRideTypeDescriptor().PhotoItem
                                                                                : windowRide->GetRideEntry()->shop_item[1];
        if (shopItem != ShopItem::None)
        {
            ft = Formatter();
            ft.Add<StringId>(GetShopItemDescriptor(shopItem).Naming.Plural);
            ft.Add<uint32_t>(windowRide->no_secondary_items_sold);
            DrawTextBasic(dpi, screenCoords, STR_ITEMS_SOLD, ft);
            screenCoords.y += LIST_ROW_HEIGHT;
        }

        // Total customers
        ft = Formatter();
        ft.Add<uint32_t>(windowRide->total_customers);
        DrawTextBasic(dpi, screenCoords, STR_TOTAL_CUSTOMERS, ft);
        screenCoords.y += LIST_ROW_HEIGHT;

        // Guests favourite
        if (windowRide->IsRide())
        {
            ft = Formatter();
            ft.Add<uint16_t>(windowRide->guests_favourite);
            stringId = windowRide->guests_favourite == 1 ? STR_FAVOURITE_RIDE_OF_GUEST : STR_FAVOURITE_RIDE_OF_GUESTS;
            DrawTextBasic(dpi, screenCoords, stringId, ft);
            screenCoords.y += LIST_ROW_HEIGHT;
        }
        screenCoords.y += 2;

        // Age
        // If the ride has a build date that is in the future, show it as built this year.
        int16_t age = std::max(date_get_year(windowRide->GetAge()), 0);
        stringId = age == 0 ? STR_BUILT_THIS_YEAR : age == 1 ? STR_BUILT_LAST_YEAR : STR_BUILT_YEARS_AGO;
        ft = Formatter();
        ft.Add<int16_t>(age);
        DrawTextBasic(dpi, screenCoords, stringId, ft);
    }

#pragma endregion

#pragma endregion

#pragma region Tabs

    void WindowRideDrawTabImage(rct_drawpixelinfo* dpi, int32_t tabPage, int32_t spriteIndex)
    {
        WidgetIndex widgetIndex = WIDX_TAB_1 + tabPage;

        if (!WidgetIsDisabled(*this, widgetIndex))
        {
            if (page == tabPage)
            {
                int32_t frame = frame_no / window_ride_tab_animation_divisor[page];
                spriteIndex += (frame % window_ride_tab_animation_frames[page]);
            }

            const auto& widget = widgets[widgetIndex];
            gfx_draw_sprite(dpi, ImageId(spriteIndex), windowPos + ScreenCoordsXY{ widget.left, widget.top });
        }
    }

    /**
     *
     *  rct2: 0x006B2E88
     */
    void WindowRideDrawTabMain(rct_drawpixelinfo* dpi)
    {
        WidgetIndex widgetIndex = WIDX_TAB_1 + WINDOW_RIDE_PAGE_MAIN;
        if (!WidgetIsDisabled(*this, widgetIndex))
        {
            auto windowRide = get_ride(rideId);
            if (windowRide != nullptr)
            {
                int32_t spriteIndex = 0;
                switch (windowRide->GetClassification())
                {
                    case RideClassification::Ride:
                        spriteIndex = SPR_TAB_RIDE_0;
                        if (page == WINDOW_RIDE_PAGE_MAIN)
                            spriteIndex += (frame_no / 4) % 16;
                        break;
                    case RideClassification::ShopOrStall:
                        spriteIndex = SPR_TAB_SHOPS_AND_STALLS_0;
                        if (page == WINDOW_RIDE_PAGE_MAIN)
                            spriteIndex += (frame_no / 4) % 16;
                        break;
                    case RideClassification::KioskOrFacility:
                        spriteIndex = SPR_TAB_KIOSKS_AND_FACILITIES_0;
                        if (page == WINDOW_RIDE_PAGE_MAIN)
                            spriteIndex += (frame_no / 4) % 8;
                        break;
                }

                const auto& widget = widgets[widgetIndex];
                gfx_draw_sprite(dpi, ImageId(spriteIndex), windowPos + ScreenCoordsXY{ widget.left, widget.top });
            }
        }
    }

    /**
     *
     *  rct2: 0x006B2B68
     */
    void WindowRideDrawTabVehicle(rct_drawpixelinfo* dpi)
    {
        WidgetIndex widgetIndex = WIDX_TAB_1 + WINDOW_RIDE_PAGE_VEHICLE;
        const auto& widget = widgets[widgetIndex];

        if (!WidgetIsDisabled(*this, widgetIndex))
        {
            auto screenCoords = ScreenCoordsXY{ widget.left + 1, widget.top + 1 };
            int32_t newWidth = widget.right - screenCoords.x;
            int32_t newHeight = widget.bottom - 3 - screenCoords.y;
            if (page == WINDOW_RIDE_PAGE_VEHICLE)
                newHeight += 4;

            screenCoords += windowPos;

            rct_drawpixelinfo clipDPI;
            if (!clip_drawpixelinfo(&clipDPI, dpi, screenCoords, newWidth, newHeight))
            {
                return;
            }

            screenCoords = ScreenCoordsXY{ widget.width() / 2, widget.height() - 12 };

            auto windowRide = get_ride(rideId);
            if (windowRide == nullptr)
                return;

            auto rideEntry = windowRide->GetRideEntry();
            if (rideEntry == nullptr)
                return;

            if (rideEntry->flags & RIDE_ENTRY_FLAG_VEHICLE_TAB_SCALE_HALF)
            {
                clipDPI.zoom_level = ZoomLevel{ 1 };
                clipDPI.width *= 2;
                clipDPI.height *= 2;
                screenCoords.x *= 2;
                screenCoords.y *= 2;
                clipDPI.x *= 2;
                clipDPI.y *= 2;
            }

            // For any suspended rides, move image higher in the vehicle tab on the rides window
            if (windowRide->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_IS_SUSPENDED))
            {
                screenCoords.y /= 4;
            }

            const auto vehicle = ride_entry_get_vehicle_at_position(
                windowRide->subtype, windowRide->num_cars_per_train, rideEntry->TabCar);
            CarEntry* carEntry = &rideEntry->Cars[vehicle];

            auto vehicleId = ((windowRide->colour_scheme_type & 3) == VEHICLE_COLOUR_SCHEME_PER_VEHICLE) ? rideEntry->TabCar
                                                                                                         : 0;
            VehicleColour vehicleColour = ride_get_vehicle_colour(windowRide, vehicleId);

            // imageIndex represents a precision of 64
            auto imageIndex = OpenRCT2::Entity::Yaw::YawFrom4(2) * 2;
            if (page == WINDOW_RIDE_PAGE_VEHICLE)
                imageIndex += frame_no;
            imageIndex = carEntry->SpriteByYaw(imageIndex / 2, SpriteGroupType::SlopeFlat);
            imageIndex &= carEntry->TabRotationMask;
            imageIndex *= carEntry->base_num_frames;
            imageIndex += carEntry->base_image_id;
            auto imageId = ImageId(imageIndex, vehicleColour.Body, vehicleColour.Trim, vehicleColour.Tertiary);
            gfx_draw_sprite(&clipDPI, imageId, screenCoords);
        }
    }

    /**
     *
     *  rct2: 0x006B2F42
     */
    void WindowRideDrawTabCustomer(rct_drawpixelinfo* dpi)
    {
        WidgetIndex widgetIndex = WIDX_TAB_1 + WINDOW_RIDE_PAGE_CUSTOMER;

        if (!WidgetIsDisabled(*this, widgetIndex))
        {
            const auto& widget = widgets[widgetIndex];
            int32_t spriteIndex = 0;
            if (page == WINDOW_RIDE_PAGE_CUSTOMER)
                spriteIndex = picked_peep_frame & ~3;

            spriteIndex += GetPeepAnimation(PeepSpriteType::Normal).base_image;
            spriteIndex += 1;
            spriteIndex |= 0xA9E00000;

            gfx_draw_sprite(
                dpi, ImageId::FromUInt32(spriteIndex), windowPos + ScreenCoordsXY{ widget.midX(), widget.bottom - 6 });
        }
    }

    /**
     *
     *  rct2: 0x006B2B35
     */
    void WindowRideDrawTabImages(rct_drawpixelinfo* dpi)
    {
        WindowRideDrawTabVehicle(dpi);
        WindowRideDrawTabImage(dpi, WINDOW_RIDE_PAGE_OPERATING, SPR_TAB_GEARS_0);
        WindowRideDrawTabImage(dpi, WINDOW_RIDE_PAGE_MAINTENANCE, SPR_TAB_WRENCH_0);
        WindowRideDrawTabImage(dpi, WINDOW_RIDE_PAGE_INCOME, SPR_TAB_ADMISSION_0);
        WindowRideDrawTabMain(dpi);
        WindowRideDrawTabImage(dpi, WINDOW_RIDE_PAGE_MEASUREMENTS, SPR_TAB_TIMER_0);
        WindowRideDrawTabImage(dpi, WINDOW_RIDE_PAGE_COLOUR, SPR_TAB_PAINT_0);
        WindowRideDrawTabImage(dpi, WINDOW_RIDE_PAGE_GRAPHS, SPR_TAB_GRAPH_A_0);
        WindowRideDrawTabCustomer(dpi);
        WindowRideDrawTabImage(dpi, WINDOW_RIDE_PAGE_MUSIC, SPR_TAB_MUSIC_0);
    }

#pragma endregion

    void WindowRideSetPressedTab()
    {
        int32_t i;
        for (i = 0; i < WINDOW_RIDE_PAGE_COUNT; i++)
            pressed_widgets &= ~(1 << (WIDX_TAB_1 + i));
        pressed_widgets |= 1LL << (WIDX_TAB_1 + page);
    }

    void WindowRideAnchorBorderWidgets()
    {
        widgets[WIDX_BACKGROUND].right = width - 1;
        widgets[WIDX_BACKGROUND].bottom = height - 1;
        widgets[WIDX_PAGE_BACKGROUND].right = width - 1;
        widgets[WIDX_PAGE_BACKGROUND].bottom = height - 1;
        widgets[WIDX_TITLE].right = width - 2;
        widgets[WIDX_CLOSE].left = width - 13;
        widgets[WIDX_CLOSE].right = width - 3;
    }
};

/**
 *
 *  rct2: 0x006AEAB4
 */
rct_window* WindowRideOpen(Ride* windowRide)
{
    auto* window = WindowCreate<RideWindow>(WindowClass::Ride, 316, 207, WF_10 | WF_RESIZABLE);

    return window;
}

/**
 *
 *  rct2: 0x006ACC28
 */
rct_window* WindowRideMainOpen(Ride* windowRide)
{
    if (windowRide->type >= RIDE_TYPE_COUNT)
    {
        return nullptr;
    }

    rct_window* w = window_bring_to_front_by_number(WindowClass::Ride, windowRide->id.ToUnderlying());
    if (w == nullptr)
    {
        w = WindowRideOpen(windowRide);
        w->ride.var_482 = -1;
        w->ride.view = 0;
    }
    else if (w->ride.view >= (1 + windowRide->num_vehicles + windowRide->num_stations))
    {
        w->ride.view = 0;
    }

    auto rideWindow = static_cast<RideWindow*>(w);
    assert(rideWindow != nullptr);

    rideWindow->OnWindowRideMainOpen(windowRide);

    return w;
}

/**
 *
 *  rct2: 0x006ACCCE
 */
rct_window* WindowRideOpenStation(Ride* windowRide, StationIndex stationIndex)
{
    if (windowRide->type >= RIDE_TYPE_COUNT)
        return nullptr;

    if (windowRide->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_NO_VEHICLES))
        return WindowRideMainOpen(windowRide);

    auto w = window_bring_to_front_by_number(WindowClass::Ride, windowRide->id.ToUnderlying());
    if (w == nullptr)
    {
        w = WindowRideOpen(windowRide);
        w->ride.var_482 = -1;
    }

    auto rideWindow = static_cast<RideWindow*>(w);
    assert(rideWindow != nullptr);

    rideWindow->OnWindowRideOpenStation(windowRide, stationIndex);

    return w;
}

rct_window* WindowRideOpenTrack(TileElement* tileElement)
{
    assert(tileElement != nullptr);
    auto rideIndex = tileElement->GetRideIndex();
    if (!rideIndex.IsNull())
    {
        auto windowRide = get_ride(rideIndex);
        if (windowRide != nullptr)
        {
            const auto type = tileElement->GetType();
            if (type == TileElementType::Entrance)
            {
                // Open ride window in station view
                auto entranceElement = tileElement->AsEntrance();
                auto stationIndex = entranceElement->GetStationIndex();
                return WindowRideOpenStation(windowRide, stationIndex);
            }
            else if (type == TileElementType::Track)
            {
                // Open ride window in station view
                auto trackElement = tileElement->AsTrack();
                auto trackType = trackElement->GetTrackType();
                const auto& ted = GetTrackElementDescriptor(trackType);
                if (ted.SequenceProperties[0] & TRACK_SEQUENCE_FLAG_ORIGIN)
                {
                    auto stationIndex = trackElement->GetStationIndex();
                    return WindowRideOpenStation(windowRide, stationIndex);
                }
            }

            // Open ride window in overview mode
            return WindowRideMainOpen(windowRide);
        }
    }
    return nullptr;
}

/**
 *
 *  rct2: 0x006ACAC2
 */
rct_window* WindowRideOpenVehicle(Vehicle* vehicle)
{
    if (vehicle == nullptr)
        return nullptr;

    Vehicle* headVehicle = vehicle->TrainHead();
    if (headVehicle == nullptr)
        return nullptr;

    EntityId headVehicleSpriteIndex = headVehicle->sprite_index;
    auto windowRide = headVehicle->GetRide();
    if (windowRide == nullptr)
        return nullptr;

    // Get view index
    int32_t view = 1;
    for (int32_t i = 0; i <= OpenRCT2::Limits::MaxTrainsPerRide; i++)
    {
        if (windowRide->vehicles[i] == headVehicleSpriteIndex)
            break;

        view++;
    }

    rct_window* w = window_find_by_number(WindowClass::Ride, windowRide->id.ToUnderlying());
    if (w != nullptr)
    {
        w->Invalidate();

        if (input_test_flag(INPUT_FLAG_TOOL_ACTIVE) && gCurrentToolWidget.window_classification == w->classification
            && gCurrentToolWidget.window_number == w->number)
        {
            tool_cancel();
        }

        int32_t openedPeepWindow = 0;
        if (w->ride.view == view)
        {
            int32_t numPeepsLeft = vehicle->num_peeps;
            for (int32_t i = 0; i < 32 && numPeepsLeft > 0; i++)
            {
                Peep* peep = GetEntity<Guest>(vehicle->peep[i]);
                if (peep == nullptr)
                    continue;

                numPeepsLeft--;
                rct_window* w2 = window_find_by_number(WindowClass::Peep, vehicle->peep[i]);
                if (w2 == nullptr)
                {
                    auto intent = Intent(WindowClass::Peep);
                    intent.putExtra(INTENT_EXTRA_PEEP, peep);
                    context_open_intent(&intent);
                    openedPeepWindow = 1;

                    break;
                }
            }
        }

        w = openedPeepWindow ? window_find_by_number(WindowClass::Ride, windowRide->id.ToUnderlying())
                             : window_bring_to_front_by_number(WindowClass::Ride, windowRide->id.ToUnderlying());
    }

    if (w == nullptr)
    {
        w = WindowRideOpen(windowRide);
        w->ride.var_482 = -1;
    }

    auto rideWindow = static_cast<RideWindow*>(w);
    assert(rideWindow != nullptr);

    rideWindow->OnWindowRideOpenVehicle(view);

    return w;
}
