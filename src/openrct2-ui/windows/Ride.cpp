/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../UiStringIds.h"
#include "../interface/Theme.h"

#include <cassert>
#include <cmath>
#include <iterator>
#include <limits>
#include <memory>
#include <openrct2-ui/interface/Dropdown.h>
#include <openrct2-ui/interface/Viewport.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Windows.h>
#include <openrct2/Cheats.h>
#include <openrct2/Context.h>
#include <openrct2/Diagnostic.h>
#include <openrct2/Game.h>
#include <openrct2/GameState.h>
#include <openrct2/Input.h>
#include <openrct2/Limits.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/SpriteIds.h>
#include <openrct2/actions/GameAction.h>
#include <openrct2/actions/ParkSetParameterAction.h>
#include <openrct2/actions/ResultWithMessage.h>
#include <openrct2/actions/RideSetAppearanceAction.h>
#include <openrct2/actions/RideSetColourSchemeAction.h>
#include <openrct2/actions/RideSetNameAction.h>
#include <openrct2/actions/RideSetPriceAction.h>
#include <openrct2/actions/RideSetSettingAction.h>
#include <openrct2/actions/RideSetStatusAction.h>
#include <openrct2/audio/Audio.h>
#include <openrct2/config/Config.h>
#include <openrct2/core/String.hpp>
#include <openrct2/core/UnitConversion.h>
#include <openrct2/drawing/ColourMap.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/drawing/Rectangle.h>
#include <openrct2/entity/EntityList.h>
#include <openrct2/entity/Staff.h>
#include <openrct2/localisation/Currency.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/localisation/Formatting.h>
#include <openrct2/localisation/Localisation.Date.h>
#include <openrct2/localisation/LocalisationService.h>
#include <openrct2/network/Network.h>
#include <openrct2/object/MusicObject.h>
#include <openrct2/object/ObjectLimits.h>
#include <openrct2/object/ObjectManager.h>
#include <openrct2/object/PeepAnimationsObject.h>
#include <openrct2/object/StationObject.h>
#include <openrct2/rct1/RCT1.h>
#include <openrct2/ride/RideConstruction.h>
#include <openrct2/ride/RideData.h>
#include <openrct2/ride/ShopItem.h>
#include <openrct2/ride/Track.h>
#include <openrct2/ride/TrackData.h>
#include <openrct2/ride/TrackDesign.h>
#include <openrct2/ride/TrackDesignRepository.h>
#include <openrct2/ride/Vehicle.h>
#include <openrct2/ui/WindowManager.h>
#include <openrct2/util/Util.h>
#include <openrct2/windows/Intent.h>
#include <openrct2/world/Map.h>
#include <openrct2/world/Park.h>
#include <openrct2/world/tile_element/EntranceElement.h>
#include <openrct2/world/tile_element/TrackElement.h>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

using namespace OpenRCT2::Drawing;
using namespace OpenRCT2::TrackMetaData;

namespace OpenRCT2::Ui::Windows
{
    static constexpr int32_t kMinimumWindowWidth = 316;

    static constexpr StringId kWindowTitle = STR_RIDE_WINDOW_TITLE;
    static constexpr ScreenSize kWindowSize = { kMinimumWindowWidth, 207 };

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

    enum
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
        WIDX_VEHICLE_REVERSED_TRAINS_CHECKBOX,
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
        WIDX_RELIABILITY_BAR,
        WIDX_DOWN_TIME_BAR,

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
        WIDX_VEHICLE_BODY_COLOUR,
        WIDX_VEHICLE_TRIM_COLOUR,
        WIDX_VEHICLE_TERTIARY_COLOUR,
        WIDX_SELL_ITEM_RANDOM_COLOUR_CHECKBOX,
        WIDX_RANDOMISE_VEHICLE_COLOURS,

        WIDX_PLAY_MUSIC = 14,
        WIDX_MUSIC,
        WIDX_MUSIC_DROPDOWN,
        WIDX_MUSIC_IMAGE,
        WIDX_MUSIC_DATA,

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

    // clang-format off
    constexpr int32_t kRCT1LightOffset = 4;

    static constexpr auto kMainRideWidgets = makeWidgets(
        makeWindowShim(kWindowTitle, kWindowSize),
        makeWidget({  0, 43}, {kMinimumWindowWidth, 137}, WidgetType::resize, WindowColour::secondary),
        makeTab({   3, 17 }, STR_VIEW_OF_RIDE_ATTRACTION_TIP),
        makeTab({  34, 17 }, STR_VEHICLE_DETAILS_AND_OPTIONS_TIP),
        makeTab({  65, 17 }, STR_OPERATING_OPTIONS_TIP),
        makeTab({  96, 17 }, STR_MAINTENANCE_OPTIONS_TIP),
        makeTab({ 127, 17 }, STR_COLOUR_SCHEME_OPTIONS_TIP),
        makeTab({ 158, 17 }, STR_SOUND_AND_MUSIC_OPTIONS_TIP),
        makeTab({ 189, 17 }, STR_MEASUREMENTS_AND_TEST_DATA_TIP),
        makeTab({ 220, 17 }, STR_GRAPHS_TIP),
        makeTab({ 251, 17 }, STR_INCOME_AND_COSTS_TIP),
        makeTab({ 282, 17 }, STR_CUSTOMER_INFORMATION_TIP)
    );

    // 0x009ADC34
    static constexpr auto _mainWidgets = makeWidgets(
        kMainRideWidgets,
        makeWidget({  3,  60}, {288, 107}, WidgetType::viewport,      WindowColour::secondary                                                                  ),
        makeWidget({ 35,  46}, {222,  12}, WidgetType::dropdownMenu,  WindowColour::secondary, kWidgetContentEmpty,                 STR_VIEW_SELECTION         ),
        makeWidget({245,  47}, { 11,  10}, WidgetType::button,        WindowColour::secondary, STR_DROPDOWN_GLYPH,                  STR_VIEW_SELECTION         ),
        makeWidget({  3, 167}, {288,  11}, WidgetType::labelCentred,  WindowColour::secondary                                                                  ),
        makeWidget({291,  46}, { 24,  24}, WidgetType::flatBtn,       WindowColour::secondary, kWidgetContentEmpty,                 STR_OPEN_CLOSE_OR_TEST_RIDE),
        makeWidget({291,  70}, { 24,  24}, WidgetType::flatBtn,       WindowColour::secondary, ImageId(SPR_CONSTRUCTION),           STR_CONSTRUCTION           ),
        makeWidget({291,  94}, { 24,  24}, WidgetType::flatBtn,       WindowColour::secondary, ImageId(SPR_RENAME),                 STR_NAME_RIDE_TIP          ),
        makeWidget({291, 118}, { 24,  24}, WidgetType::flatBtn,       WindowColour::secondary, ImageId(SPR_LOCATE),                 STR_LOCATE_SUBJECT_TIP     ),
        makeWidget({291, 142}, { 24,  24}, WidgetType::flatBtn,       WindowColour::secondary, ImageId(SPR_DEMOLISH),               STR_DEMOLISH_RIDE_TIP      ),
        makeWidget({296,  48}, { 14,  14}, WidgetType::imgBtn,        WindowColour::secondary, ImageId(SPR_G2_RCT1_CLOSE_BUTTON_0), STR_CLOSE_RIDE_TIP         ),
        makeWidget({296,  62}, { 14,  14}, WidgetType::imgBtn,        WindowColour::secondary, ImageId(SPR_G2_RCT1_TEST_BUTTON_0),  STR_SIMULATE_RIDE_TIP      ),
        makeWidget({296,  62}, { 14,  14}, WidgetType::imgBtn,        WindowColour::secondary, ImageId(SPR_G2_RCT1_TEST_BUTTON_0),  STR_TEST_RIDE_TIP          ),
        makeWidget({296,  76}, { 14,  14}, WidgetType::imgBtn,        WindowColour::secondary, ImageId(SPR_G2_RCT1_OPEN_BUTTON_0),  STR_OPEN_RIDE_TIP          ),
        makeWidget({  3, 180}, {305,  12}, WidgetType::dropdownMenu,  WindowColour::secondary, STR_ARG_6_STRINGID                                              ),
        makeWidget({297, 180}, { 11,  12}, WidgetType::button,        WindowColour::secondary, STR_DROPDOWN_GLYPH                                              )
    );

    // 0x009ADDA8
    static constexpr auto _vehicleWidgets = makeWidgets(
        kMainRideWidgets,
        makeWidget        ({  7,  50}, {302, 12}, WidgetType::dropdownMenu, WindowColour::secondary                                                          ),
        makeWidget        ({297,  51}, { 11, 10}, WidgetType::button,       WindowColour::secondary, STR_DROPDOWN_GLYPH                                      ),
        makeWidget        ({  7, 137}, {302, 12}, WidgetType::checkbox,     WindowColour::secondary, STR_OPTION_REVERSE_TRAINS, STR_OPTION_REVERSE_TRAINS_TIP),
        makeWidget        ({  7, 154}, {302, 43}, WidgetType::scroll,       WindowColour::secondary, kStringIdEmpty                                          ),
        makeSpinnerWidgets({  7, 203}, {145, 12}, WidgetType::spinner,      WindowColour::secondary, STR_RIDE_VEHICLE_COUNT, STR_MAX_VEHICLES_TIP            ),
        makeSpinnerWidgets({164, 203}, {145, 12}, WidgetType::spinner,      WindowColour::secondary, STR_1_CAR_PER_TRAIN,    STR_MAX_CARS_PER_TRAIN_TIP      )
    );

    // 0x009ADEFC
    static constexpr auto _operatingWidgets = makeWidgets(
        kMainRideWidgets,
        makeSpinnerWidgets({157,  61}, {152, 12}, WidgetType::spinner,      WindowColour::secondary, STR_ARG_18_STRINGID                                                                 ), // NB: 3 widgets
        makeSpinnerWidgets({157,  75}, {152, 12}, WidgetType::spinner,      WindowColour::secondary, STR_LIFT_HILL_CHAIN_SPEED_VALUE                                                     ), // NB: 3 widgets
        makeWidget        ({  7, 109}, { 80, 12}, WidgetType::checkbox,     WindowColour::secondary, STR_WAIT_FOR,                           STR_WAIT_FOR_PASSENGERS_BEFORE_DEPARTING_TIP),
        makeWidget        ({  7, 124}, {302, 12}, WidgetType::checkbox,     WindowColour::secondary                                                                                      ),
        makeWidget        ({  7, 139}, {150, 12}, WidgetType::checkbox,     WindowColour::secondary, STR_MINIMUM_WAITING_TIME,               STR_MINIMUM_LENGTH_BEFORE_DEPARTING_TIP     ),
        makeSpinnerWidgets({157, 139}, {152, 12}, WidgetType::spinner,      WindowColour::secondary, STR_ARG_10_STRINGID                                                                 ), // NB: 3 widgets
        makeWidget        ({  7, 154}, {150, 12}, WidgetType::checkbox,     WindowColour::secondary, STR_MAXIMUM_WAITING_TIME,               STR_MAXIMUM_LENGTH_BEFORE_DEPARTING_TIP     ),
        makeSpinnerWidgets({157, 154}, {152, 12}, WidgetType::spinner,      WindowColour::secondary, STR_ARG_14_STRINGID                                                                 ), // NB: 3 widgets
        makeWidget        ({  7, 169}, {302, 12}, WidgetType::checkbox,     WindowColour::secondary, STR_SYNCHRONISE_WITH_ADJACENT_STATIONS, STR_SYNCHRONISE_WITH_ADJACENT_STATIONS_TIP  ),
        makeWidget        ({ 21,  61}, {129, 12}, WidgetType::label,        WindowColour::secondary                                                                                      ),
        makeWidget        ({ 21,  75}, {129, 12}, WidgetType::label,        WindowColour::secondary, STR_LIFT_HILL_CHAIN_SPEED                                                           ),
        makeWidget        ({  7,  47}, {302, 12}, WidgetType::dropdownMenu, WindowColour::secondary, 0xFFFFFFFF,                             STR_SELECT_OPERATING_MODE                   ),
        makeWidget        ({297,  48}, { 11, 10}, WidgetType::button,       WindowColour::secondary, STR_DROPDOWN_GLYPH,                     STR_SELECT_OPERATING_MODE                   ),
        makeWidget        ({ 87, 109}, {222, 12}, WidgetType::dropdownMenu, WindowColour::secondary                                                                                      ),
        makeWidget        ({297, 110}, { 11, 10}, WidgetType::button,       WindowColour::secondary, STR_DROPDOWN_GLYPH                                                                  ),
        makeWidget        ({ 21,  89}, {129, 12}, WidgetType::label,        WindowColour::secondary, STR_NUMBER_OF_CIRCUITS,                 STR_NUMBER_OF_CIRCUITS_TIP                  ),
        makeSpinnerWidgets({157,  89}, {152, 12}, WidgetType::spinner,      WindowColour::secondary, STR_NUMBER_OF_CIRCUITS_VALUE                                                        )  // NB: 3 widgets
    );

    // 0x009AE190
    static constexpr auto _maintenanceWidgets = makeWidgets(
        kMainRideWidgets,
        makeWidget     ({107,  71}, {202,  12}, WidgetType::dropdownMenu, WindowColour::secondary, kStringIdEmpty,            STR_SELECT_HOW_OFTEN_A_MECHANIC_SHOULD_CHECK_THIS_RIDE),
        makeWidget     ({297,  72}, { 11,  10}, WidgetType::button,       WindowColour::secondary, STR_DROPDOWN_GLYPH,        STR_SELECT_HOW_OFTEN_A_MECHANIC_SHOULD_CHECK_THIS_RIDE),
        makeWidget     ({289, 108}, { 24,  24}, WidgetType::flatBtn,      WindowColour::secondary, 0xFFFFFFFF,                STR_LOCATE_NEAREST_AVAILABLE_MECHANIC_TIP             ),
        makeWidget     ({265, 108}, { 24,  24}, WidgetType::flatBtn,      WindowColour::secondary, ImageId(SPR_CONSTRUCTION), STR_REFURBISH_RIDE_TIP                                ),
        makeWidget     ({241, 108}, { 24,  24}, WidgetType::flatBtn,      WindowColour::secondary, ImageId(SPR_NO_ENTRY),     STR_DEBUG_FORCE_BREAKDOWN_TIP                         ),
        makeProgressBar({107,  47}, { 147, 10}, Drawing::Colour::brightGreen                                                                                                                 ),
        makeProgressBar({107,  58}, { 147, 10}, Drawing::Colour::brightRed                                                                                                                   )
    );

    // 0x009AE2A4
    static constexpr auto _colourWidgets = makeWidgets(
        kMainRideWidgets,
        makeWidget({  3,  47}, { 68, 47}, WidgetType::spinner,      WindowColour::secondary                                                                              ),
        makeWidget({ 74,  49}, {239, 12}, WidgetType::dropdownMenu, WindowColour::secondary, STR_ARG_14_STRINGID                                                         ),
        makeWidget({301,  50}, { 11, 10}, WidgetType::button,       WindowColour::secondary, STR_DROPDOWN_GLYPH,            STR_COLOUR_SCHEME_TO_CHANGE_TIP              ),
        makeWidget({ 79,  74}, { 12, 12}, WidgetType::colourBtn,    WindowColour::secondary, 0xFFFFFFFF,                    STR_SELECT_MAIN_COLOUR_TIP                   ),
        makeWidget({ 99,  74}, { 12, 12}, WidgetType::colourBtn,    WindowColour::secondary, 0xFFFFFFFF,                    STR_SELECT_ADDITIONAL_COLOUR_1_TIP           ),
        makeWidget({119,  74}, { 12, 12}, WidgetType::colourBtn,    WindowColour::secondary, 0xFFFFFFFF,                    STR_SELECT_SUPPORT_STRUCTURE_COLOUR_TIP      ),
        makeWidget({ 74,  49}, {239, 12}, WidgetType::dropdownMenu, WindowColour::secondary                                                                              ),
        makeWidget({301,  50}, { 11, 10}, WidgetType::button,       WindowColour::secondary, STR_DROPDOWN_GLYPH                                                          ),
        makeWidget({289,  68}, { 24, 24}, WidgetType::flatBtn,      WindowColour::secondary, ImageId(SPR_PAINTBRUSH),       STR_PAINT_INDIVIDUAL_AREA_TIP                ),
        makeWidget({245, 101}, { 68, 47}, WidgetType::spinner,      WindowColour::secondary                                                                              ),
        makeWidget({103, 103}, {139, 12}, WidgetType::dropdownMenu, WindowColour::secondary, kStringIdEmpty                                                              ),
        makeWidget({230, 104}, { 11, 10}, WidgetType::button,       WindowColour::secondary, STR_DROPDOWN_GLYPH,            STR_SELECT_STYLE_OF_ENTRANCE_EXIT_STATION_TIP),
        makeWidget({  3, 157}, { 68, 47}, WidgetType::scroll,       WindowColour::secondary, kStringIdEmpty                                                              ),
        makeWidget({ 74, 157}, {239, 12}, WidgetType::dropdownMenu, WindowColour::secondary, STR_ARG_6_STRINGID                                                          ),
        makeWidget({301, 158}, { 11, 10}, WidgetType::button,       WindowColour::secondary, STR_DROPDOWN_GLYPH,            STR_SELECT_VEHICLE_COLOUR_SCHEME_TIP         ),
        makeWidget({ 74, 173}, {239, 12}, WidgetType::dropdownMenu, WindowColour::secondary                                                                              ),
        makeWidget({301, 174}, { 11, 10}, WidgetType::button,       WindowColour::secondary, STR_DROPDOWN_GLYPH,            STR_SELECT_VEHICLE_TO_MODIFY_TIP             ),
        makeWidget({ 79, 190}, { 12, 12}, WidgetType::colourBtn,    WindowColour::secondary, 0xFFFFFFFF,                    STR_SELECT_MAIN_COLOUR_TIP                   ),
        makeWidget({ 99, 190}, { 12, 12}, WidgetType::colourBtn,    WindowColour::secondary, 0xFFFFFFFF,                    STR_SELECT_ADDITIONAL_COLOUR_1_TIP           ),
        makeWidget({119, 190}, { 12, 12}, WidgetType::colourBtn,    WindowColour::secondary, 0xFFFFFFFF,                    STR_SELECT_ADDITIONAL_COLOUR_2_TIP           ),
        makeWidget({100,  74}, {239, 12}, WidgetType::checkbox,     WindowColour::secondary, STR_RANDOM_COLOUR                                                           ),
        makeWidget({139, 190}, {110, 12}, WidgetType::button,       WindowColour::secondary, STR_RANDOMISE_VEHICLE_COLOURS, STR_RANDOMISE_VEHICLE_COLOURS_TIP            )
    );

    // 0x009AE4C8
    static constexpr auto _musicWidgets = makeWidgets(
        kMainRideWidgets,
        makeWidget({  7, 47}, {302,  12}, WidgetType::checkbox,     WindowColour::secondary, STR_PLAY_MUSIC,     STR_SELECT_MUSIC_TIP      ),
        makeWidget({  7, 62}, {302,  12}, WidgetType::dropdownMenu, WindowColour::secondary, kStringIdEmpty                                ),
        makeWidget({297, 63}, { 11,  10}, WidgetType::button,       WindowColour::secondary, STR_DROPDOWN_GLYPH, STR_SELECT_MUSIC_STYLE_TIP),
        makeWidget({154, 90}, {114, 114}, WidgetType::flatBtn,      WindowColour::secondary                                                ),
        makeWidget({  7, 90}, {500, 450}, WidgetType::scroll,       WindowColour::secondary, SCROLL_BOTH                                   )
    );

    // 0x009AE5DC
    static constexpr auto _measurementWidgets = makeWidgets(
        kMainRideWidgets,
        makeWidget({288, 194}, { 24, 24}, WidgetType::flatBtn, WindowColour::secondary, ImageId(SPR_FLOPPY),       STR_SAVE_TRACK_DESIGN),
        makeWidget({  4, 127}, {154, 14}, WidgetType::button,  WindowColour::secondary, STR_SELECT_NEARBY_SCENERY                       ),
        makeWidget({158, 127}, {154, 14}, WidgetType::button,  WindowColour::secondary, STR_RESET_SELECTION                             ),
        makeWidget({  4, 177}, {154, 14}, WidgetType::button,  WindowColour::secondary, STR_DESIGN_SAVE                                 ),
        makeWidget({158, 177}, {154, 14}, WidgetType::button,  WindowColour::secondary, STR_DESIGN_CANCEL                               )
    );

    // 0x009AE710
    static constexpr auto _graphsWidgets = makeWidgets(
        kMainRideWidgets,
        makeWidget({  3,  46}, {306, 112}, WidgetType::scroll, WindowColour::secondary, SCROLL_HORIZONTAL,       STR_LOGGING_DATA_FROM_TIP                               ),
        makeWidget({  3, 163}, { 73,  14}, WidgetType::button, WindowColour::secondary, STR_RIDE_STATS_VELOCITY, STR_SHOW_GRAPH_OF_VELOCITY_AGAINST_TIME_TIP             ),
        makeWidget({ 76, 163}, { 73,  14}, WidgetType::button, WindowColour::secondary, STR_RIDE_STATS_ALTITUDE, STR_SHOW_GRAPH_OF_ALTITUDE_AGAINST_TIME_TIP             ),
        makeWidget({149, 163}, { 73,  14}, WidgetType::button, WindowColour::secondary, STR_RIDE_STATS_VERT_G,   STR_SHOW_GRAPH_OF_VERTICAL_ACCELERATION_AGAINST_TIME_TIP),
        makeWidget({222, 163}, { 73,  14}, WidgetType::button, WindowColour::secondary, STR_RIDE_STATS_LAT_G,    STR_SHOW_GRAPH_OF_LATERAL_ACCELERATION_AGAINST_TIME_TIP )
    );

    // 0x009AE844
    static constexpr auto _incomeWidgets = makeWidgets(
        kMainRideWidgets,
        makeWidget        ({ 19,  50}, {126, 14}, WidgetType::label,    WindowColour::secondary                                                                    ),
        makeSpinnerWidgets({147,  50}, {162, 14}, WidgetType::spinner,  WindowColour::secondary, STR_ARG_6_CURRENCY2DP                                             ), // NB: 3 widgets
        makeWidget        ({  5,  62}, {306, 13}, WidgetType::checkbox, WindowColour::secondary, STR_SAME_PRICE_THROUGHOUT_PARK, STR_SAME_PRICE_THROUGHOUT_PARK_TIP),
        makeWidget        ({ 19,  94}, {126, 14}, WidgetType::label,    WindowColour::secondary                                                                    ),
        makeSpinnerWidgets({147,  94}, {162, 14}, WidgetType::spinner,  WindowColour::secondary, STR_RIDE_SECONDARY_PRICE_VALUE                                    ), // NB: 3 widgets
        makeWidget        ({  5, 106}, {306, 13}, WidgetType::checkbox, WindowColour::secondary, STR_SAME_PRICE_THROUGHOUT_PARK, STR_SAME_PRICE_THROUGHOUT_PARK_TIP)
    );

    // 0x009AE9C8
    static constexpr auto _customerWidgets = makeWidgets(
        kMainRideWidgets,
        makeWidget({289,  54}, {24, 24}, WidgetType::flatBtn, WindowColour::secondary, ImageId(SPR_SHOW_GUESTS_THOUGHTS_ABOUT_THIS_RIDE_ATTRACTION), STR_SHOW_GUESTS_THOUGHTS_ABOUT_THIS_RIDE_ATTRACTION_TIP),
        makeWidget({289,  78}, {24, 24}, WidgetType::flatBtn, WindowColour::secondary, ImageId(SPR_SHOW_GUESTS_ON_THIS_RIDE_ATTRACTION),             STR_SHOW_GUESTS_ON_THIS_RIDE_ATTRACTION_TIP            ),
        makeWidget({289, 102}, {24, 24}, WidgetType::flatBtn, WindowColour::secondary, ImageId(SPR_SHOW_GUESTS_QUEUING_FOR_THIS_RIDE_ATTRACTION),    STR_SHOW_GUESTS_QUEUING_FOR_THIS_RIDE_ATTRACTION_TIP   )
    );

    static constexpr std::span<const Widget> PageWidgets[] = {
        _mainWidgets,
        _vehicleWidgets,
        _operatingWidgets,
        _maintenanceWidgets,
        _colourWidgets,
        _musicWidgets,
        _measurementWidgets,
        _graphsWidgets,
        _incomeWidgets,
        _customerWidgets,
    };
    static_assert(std::size(PageWidgets) == WINDOW_RIDE_PAGE_COUNT);

    static constexpr std::array PageHoldDownWidgets = {
        0uLL,
        (1uLL << WIDX_VEHICLE_TRAINS_INCREASE) |
            (1uLL << WIDX_VEHICLE_TRAINS_DECREASE) |
            (1uLL << WIDX_VEHICLE_CARS_PER_TRAIN_INCREASE) |
            (1uLL << WIDX_VEHICLE_CARS_PER_TRAIN_DECREASE),
        (1uLL << WIDX_MODE_TWEAK_INCREASE) |
            (1uLL << WIDX_MODE_TWEAK_DECREASE) |
            (1uLL << WIDX_LIFT_HILL_SPEED_INCREASE) |
            (1uLL << WIDX_LIFT_HILL_SPEED_DECREASE) |
            (1uLL << WIDX_MINIMUM_LENGTH_INCREASE) |
            (1uLL << WIDX_MINIMUM_LENGTH_DECREASE) |
            (1uLL << WIDX_MAXIMUM_LENGTH_INCREASE) |
            (1uLL << WIDX_MAXIMUM_LENGTH_DECREASE) |
            (1uLL << WIDX_OPERATE_NUMBER_OF_CIRCUITS_INCREASE) |
            (1uLL << WIDX_OPERATE_NUMBER_OF_CIRCUITS_DECREASE),
        0uLL,
        0uLL,
        0uLL,
        0uLL,
        0uLL,
        (1uLL << WIDX_PRIMARY_PRICE_INCREASE) |
            (1uLL << WIDX_PRIMARY_PRICE_DECREASE) |
            (1uLL << WIDX_SECONDARY_PRICE_INCREASE) |
            (1uLL << WIDX_SECONDARY_PRICE_DECREASE),
        0uLL,
    };
    static_assert(std::size(PageHoldDownWidgets) == WINDOW_RIDE_PAGE_COUNT);
    // clang-format on

#pragma endregion

    static void CancelScenerySelection();

    static bool _collectTrackDesignScenery = false;
    static int32_t _lastSceneryX = 0;
    static int32_t _lastSceneryY = 0;
    static std::unique_ptr<TrackDesign> _trackDesign;

    // Cached overall view for each ride
    // (Re)calculated when the ride window is opened
    struct RideOverallView
    {
        CoordsXYZ loc;
        ZoomLevel zoom;
    };

    static std::vector<RideOverallView> _rideOverallViewsCache = {};

    static constexpr std::array PageTabAnimationDivisor = {
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
    static_assert(std::size(PageTabAnimationDivisor) == WINDOW_RIDE_PAGE_COUNT);

    static constexpr std::array PageTabAnimationNumFrames = {
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
    static_assert(std::size(PageTabAnimationNumFrames) == WINDOW_RIDE_PAGE_COUNT);

    // clang-format off
    static constexpr std::array RatingNames = {
        STR_RATING_LOW,
        STR_RATING_MEDIUM,
        STR_RATING_HIGH,
        STR_RATING_VERY_HIGH,
        STR_RATING_EXTREME,
        STR_RATING_ULTRA_EXTREME,
    };
    static_assert(std::size(RatingNames) == 6);
    // clang-format on

    static constexpr std::array RideBreakdownReasonNames = {
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

    // Used in other places as well
    const StringId ColourSchemeNames[4] = {
        STR_MAIN_COLOUR_SCHEME,          // RideColourScheme::main
        STR_ALTERNATIVE_COLOUR_SCHEME_1, // RideColourScheme::additional1
        STR_ALTERNATIVE_COLOUR_SCHEME_2, // RideColourScheme::additional2
        STR_ALTERNATIVE_COLOUR_SCHEME_3, // RideColourScheme::additional3
    };
    static_assert(std::size(ColourSchemeNames) == kNumRideColourSchemes);

    static constexpr std::array VehicleLoadNames = {
        STR_QUARTER_LOAD,       //  WAIT_FOR_LOAD_QUARTER
        STR_HALF_LOAD,          //  WAIT_FOR_LOAD_HALF
        STR_THREE_QUARTER_LOAD, //  WAIT_FOR_LOAD_THREE_QUARTER
        STR_FULL_LOAD,          //  WAIT_FOR_LOAD_FULL
        STR_ANY_LOAD,           //  WAIT_FOR_LOAD_ANY
    };
    static_assert(std::size(VehicleLoadNames) == WAIT_FOR_LOAD_COUNT);

    static constexpr std::array VehicleColourSchemeNames = {
        STR_ALL_VEHICLES_IN_SAME_COLOURS,  // VehicleColourSettings::same,
        STR_DIFFERENT_COLOURS_PER,         // VehicleColourSettings::perTrain,
        STR_DIFFERENT_COLOURS_PER_VEHICLE, // VehicleColourSettings::perCar,
    };
    static_assert(std::size(VehicleColourSchemeNames) == kNumVehicleColourSettings);

    static constexpr std::array VehicleStatusNames = {
        STR_MOVING_TO_END_OF,          // Vehicle::Status::movingToEndOfStation
        STR_WAITING_FOR_PASSENGERS_AT, // Vehicle::Status::waitingForPassengers
        STR_WAITING_TO_DEPART,         // Vehicle::Status::waitingToDepart
        STR_DEPARTING,                 // Vehicle::Status::departing
        STR_TRAVELLING_AT_0,           // Vehicle::Status::travelling
        STR_ARRIVING_AT,               // Vehicle::Status::arriving
        STR_UNLOADING_PASSENGERS_AT,   // Vehicle::Status::unloadingPassengers
        STR_TRAVELLING_AT_1,           // Vehicle::Status::travellingBoat
        STR_CRASHING,                  // Vehicle::Status::crashing
        STR_CRASHED_0,                 // Vehicle::Status::crashed
        STR_TRAVELLING_AT_2,           // Vehicle::Status::travellingDodgems
        STR_SWINGING,                  // Vehicle::Status::swinging
        STR_ROTATING_0,                // Vehicle::Status::rotating
        STR_ROTATING_1,                // Vehicle::Status::ferrisWheelRotating
        STR_OPERATING_0,               // Vehicle::Status::simulatorOperating
        STR_SHOWING_FILM,              // Vehicle::Status::showingFilm
        STR_ROTATING_2,                // Vehicle::Status::spaceRingsOperating
        STR_OPERATING_1,               // Vehicle::Status::topSpinOperating
        STR_OPERATING_2,               // Vehicle::Status::hauntedHouseOperating
        STR_DOING_CIRCUS_SHOW,         // Vehicle::Status::doingCircusShow
        STR_OPERATING_3,               // Vehicle::Status::crookedHouseOperating
        STR_WAITING_FOR_CABLE_LIFT,    // Vehicle::Status::waitingForCableLift
        STR_TRAVELLING_AT_3,           // Vehicle::Status::travellingCableLift
        STR_STOPPING_0,                // Vehicle::Status::stopping
        STR_WAITING_FOR_PASSENGERS,    // Vehicle::Status::waitingForPassengers17
        STR_WAITING_TO_START,          // Vehicle::Status::waitingToStart
        STR_STARTING,                  // Vehicle::Status::starting
        STR_OPERATING,                 // Vehicle::Status::operating1A
        STR_STOPPING_1,                // Vehicle::Status::stopping1B
        STR_UNLOADING_PASSENGERS,      // Vehicle::Status::unloadingPassengers1C
        STR_STOPPED_BY_BLOCK_BRAKES,   // Vehicle::Status::stoppedByBlockBrakes
    };
    static_assert(std::size(VehicleStatusNames) == 31);

    static constexpr std::array SingleSessionVehicleStatusNames = {
        STR_STOPPING_0,             // Vehicle::Status::movingToEndOfStation
        STR_WAITING_FOR_PASSENGERS, // Vehicle::Status::waitingForPassengers
        STR_WAITING_TO_START,       // Vehicle::Status::waitingToDepart
        STR_STARTING,               // Vehicle::Status::departing
        STR_OPERATING,              // Vehicle::Status::travelling
        STR_STOPPING_1,             // Vehicle::Status::arriving
        STR_UNLOADING_PASSENGERS,   // Vehicle::Status::unloadingPassengers
    };
    static_assert(std::size(SingleSessionVehicleStatusNames) == 7);

    struct WindowRideMazeDesignOption
    {
        StringId text;
        uint32_t sprite;
    };

    static constexpr std::array MazeOptions = {
        WindowRideMazeDesignOption{ STR_RIDE_DESIGN_MAZE_BRICK_WALLS, SPR_RIDE_DESIGN_PREVIEW_MAZE_BRICK_WALLS },
        WindowRideMazeDesignOption{ STR_RIDE_DESIGN_MAZE_HEDGES, SPR_RIDE_DESIGN_PREVIEW_MAZE_HEDGES },
        WindowRideMazeDesignOption{ STR_RIDE_DESIGN_MAZE_ICE_BLOCKS, SPR_RIDE_DESIGN_PREVIEW_MAZE_ICE_BLOCKS },
        WindowRideMazeDesignOption{ STR_RIDE_DESIGN_MAZE_WOODEN_FENCES, SPR_RIDE_DESIGN_PREVIEW_MAZE_WOODEN_FENCES },
    };
    static_assert(std::size(MazeOptions) == 4);

    struct GraphsYAxis
    {
        uint8_t interval;
        int8_t unit;
        int8_t unit_interval;
        StringId label;
    };

    static constexpr const std::array GraphsYAxisDetails = {
        GraphsYAxis{ 11, 0, 10, STR_RIDE_STATS_VELOCITY_FORMAT }, // GRAPH_VELOCITY
        GraphsYAxis{ 10, 0, 15, STR_RIDE_STATS_ALTITUDE_FORMAT }, // GRAPH_ALTITUDE
        GraphsYAxis{ 13, -3, 1, STR_RIDE_STATS_G_FORCE_FORMAT },  // GRAPH_VERTICAL
        GraphsYAxis{ 13, -4, 1, STR_RIDE_STATS_G_FORCE_FORMAT },  // GRAPH_LATERAL
    };
    static_assert(std::size(GraphsYAxisDetails) == 4);

    static constexpr auto kRideGForcesRedNegVertical = -MakeFixed16_2dp(2, 50);
    static constexpr auto kRideGForcesRedLateral = MakeFixed16_2dp(2, 80);

    // Used for sorting the ride type cheat dropdown.
    struct RideTypeLabel
    {
        ride_type_t RideTypeId;
        StringId LabelId;
        u8string LabelString;
    };

    // Used for sorting the vehicle type dropdown.
    struct VehicleTypeLabel
    {
        ObjectEntryIndex SubTypeId;
        StringId LabelId;
        u8string_view LabelString;
    };

    // Used for sorting the entrance type dropdown.
    struct EntranceTypeLabel
    {
        ObjectEntryIndex EntranceTypeId;
        StringId LabelId;
        u8string_view LabelString;
    };

    class RideWindow final : public Window
    {
        int16_t _viewIndex;
        std::vector<RideTypeLabel> _rideDropdownData;
        int32_t _rideDropdownDataLanguage = LANGUAGE_UNDEFINED;
        int32_t _vehicleDropdownDataLanguage = LANGUAGE_UNDEFINED;
        int32_t _entranceDropdownDataLanguage = LANGUAGE_UNDEFINED;
        const RideObjectEntry* _vehicleDropdownRideType = nullptr;
        bool _vehicleDropdownExpanded = false;
        std::vector<VehicleTypeLabel> _vehicleDropdownData;
        int16_t _vehicleIndex = 0;
        uint16_t _rideColour = 0;
        std::vector<EntranceTypeLabel> _entranceDropdownData;
        bool _autoScrollGraph = true;

        uint8_t getNumVisibleCars()
        {
            auto* ride = GetRide(rideId);
            if (ride == nullptr)
                return 0;

            auto* rideEntry = ride->getRideEntry();
            if (rideEntry == nullptr)
                return 0;

            uint8_t numItems = 0;
            for (auto i = 0; i < ride->numCarsPerTrain; i++)
            {
                const auto& carEntry = rideEntry->Cars[RideEntryGetVehicleAtPosition(ride->subtype, ride->numCarsPerTrain, i)];
                if (carEntry.isVisible())
                    numItems++;
            }

            return numItems;
        }

        uint8_t dropdownIndexToCarIndex(int32_t dropdownIndex) const
        {
            auto* ride = GetRide(rideId);
            if (ride == nullptr || ride->vehicleColourSettings != VehicleColourSettings::perCar)
                return dropdownIndex;

            auto* rideEntry = ride->getRideEntry();
            if (rideEntry == nullptr)
                return dropdownIndex;

            // `dropdownIndex` will contain a number picked from the visible cars.
            // Convert this to the actual index.
            auto carDropdownIndex = -1;
            for (auto carIndex = 0; carIndex < ride->numCarsPerTrain; carIndex++)
            {
                const auto& carEntry = rideEntry->Cars[RideEntryGetVehicleAtPosition(
                    ride->subtype, ride->numCarsPerTrain, carIndex)];
                if (!carEntry.isVisible())
                    continue;

                carDropdownIndex++;
                if (dropdownIndex == carDropdownIndex)
                {
                    return carIndex;
                }
            }

            // Should never happen
            return dropdownIndex;
        }

        int32_t carIndexToDropdownIndex(uint8_t selectedCarIndex) const
        {
            auto* ride = GetRide(rideId);
            if (ride == nullptr || ride->vehicleColourSettings != VehicleColourSettings::perCar)
                return selectedCarIndex;

            auto* rideEntry = ride->getRideEntry();
            if (rideEntry == nullptr)
                return selectedCarIndex;

            // `selectedCarIndex` will contain an offset that includes invisible cars.
            // Convert this to the corresponding dropdown index of actually visible cars.
            auto carDropdownIndex = -1;
            for (auto carIndex = 0; carIndex < ride->numCarsPerTrain; carIndex++)
            {
                const auto& carEntry = rideEntry->Cars[RideEntryGetVehicleAtPosition(
                    ride->subtype, ride->numCarsPerTrain, carIndex)];
                if (!carEntry.isVisible())
                    continue;

                carDropdownIndex++;
                if (carIndex == selectedCarIndex)
                {
                    return carDropdownIndex;
                }
            }

            // Should never happen
            return selectedCarIndex;
        }

    public:
        RideWindow(const Ride& ride)
        {
            rideId = ride.id;
        }

        void onOpen() override
        {
            setWidgets(PageWidgets[WINDOW_RIDE_PAGE_MAIN]);
            holdDownWidgets = PageHoldDownWidgets[WINDOW_RIDE_PAGE_MAIN];

            setPage(WINDOW_RIDE_PAGE_MAIN);
            listInformationType = 0;
            pickedPeepFrame = 0;
            DisableTabs();

            WindowSetResize(*this, { kMinimumWindowWidth, 180 }, { 500, 450 });

            auto ride = GetRide(rideId);
            if (ride == nullptr)
            {
                close();
                return;
            }
            UpdateOverallView(*ride);

            PopulateVehicleTypeDropdown(*ride, true);
        }

        void onClose() override
        {
            switch (page)
            {
                case WINDOW_RIDE_PAGE_COLOUR:
                    ColourClose();
                    break;
                case WINDOW_RIDE_PAGE_MEASUREMENTS:
                    MeasurementsClose();
                    break;
            }
        }
        void onResize() override
        {
            switch (page)
            {
                case WINDOW_RIDE_PAGE_MAIN:
                    MainResize();
                    break;
                case WINDOW_RIDE_PAGE_VEHICLE:
                    VehicleResize();
                    break;
                case WINDOW_RIDE_PAGE_OPERATING:
                    OperatingResize();
                    break;
                case WINDOW_RIDE_PAGE_MAINTENANCE:
                    MaintenanceResize();
                    break;
                case WINDOW_RIDE_PAGE_COLOUR:
                    ColourResize();
                    break;
                case WINDOW_RIDE_PAGE_MUSIC:
                    MusicResize();
                    break;
                case WINDOW_RIDE_PAGE_MEASUREMENTS:
                    MeasurementsResize();
                    break;
                case WINDOW_RIDE_PAGE_GRAPHS:
                    GraphsResize();
                    break;
                case WINDOW_RIDE_PAGE_INCOME:
                    IncomeResize();
                    break;
                case WINDOW_RIDE_PAGE_CUSTOMER:
                    CustomerResize();
                    break;
            }
        }

        void onUpdate() override
        {
            switch (page)
            {
                case WINDOW_RIDE_PAGE_MAIN:
                    MainUpdate();
                    break;
                case WINDOW_RIDE_PAGE_VEHICLE:
                    VehicleUpdate();
                    break;
                case WINDOW_RIDE_PAGE_OPERATING:
                    OperatingUpdate();
                    break;
                case WINDOW_RIDE_PAGE_MAINTENANCE:
                    MaintenanceUpdate();
                    break;
                case WINDOW_RIDE_PAGE_COLOUR:
                    ColourUpdate();
                    break;
                case WINDOW_RIDE_PAGE_MUSIC:
                    MusicUpdate();
                    break;
                case WINDOW_RIDE_PAGE_MEASUREMENTS:
                    MeasurementsUpdate();
                    break;
                case WINDOW_RIDE_PAGE_GRAPHS:
                    GraphsUpdate();
                    break;
                case WINDOW_RIDE_PAGE_INCOME:
                    IncomeUpdate();
                    break;
                case WINDOW_RIDE_PAGE_CUSTOMER:
                    CustomerUpdate();
                    break;
            }
        }

        void onPrepareDraw() override
        {
            switch (page)
            {
                case WINDOW_RIDE_PAGE_MAIN:
                    MainOnPrepareDraw();
                    break;
                case WINDOW_RIDE_PAGE_VEHICLE:
                    VehicleOnPrepareDraw();
                    break;
                case WINDOW_RIDE_PAGE_OPERATING:
                    OperatingOnPrepareDraw();
                    break;
                case WINDOW_RIDE_PAGE_MAINTENANCE:
                    MaintenanceOnPrepareDraw();
                    break;
                case WINDOW_RIDE_PAGE_COLOUR:
                    ColourOnPrepareDraw();
                    break;
                case WINDOW_RIDE_PAGE_MUSIC:
                    MusicOnPrepareDraw();
                    break;
                case WINDOW_RIDE_PAGE_MEASUREMENTS:
                    MeasurementsOnPrepareDraw();
                    break;
                case WINDOW_RIDE_PAGE_GRAPHS:
                    GraphsOnPrepareDraw();
                    break;
                case WINDOW_RIDE_PAGE_INCOME:
                    IncomeOnPrepareDraw();
                    break;
                case WINDOW_RIDE_PAGE_CUSTOMER:
                    CustomerOnPrepareDraw();
                    break;
            }
        }
        void onDraw(RenderTarget& rt) override
        {
            switch (page)
            {
                case WINDOW_RIDE_PAGE_MAIN:
                    MainOnDraw(rt);
                    break;
                case WINDOW_RIDE_PAGE_VEHICLE:
                    VehicleOnDraw(rt);
                    break;
                case WINDOW_RIDE_PAGE_OPERATING:
                    OperatingOnDraw(rt);
                    break;
                case WINDOW_RIDE_PAGE_MAINTENANCE:
                    MaintenanceOnDraw(rt);
                    break;
                case WINDOW_RIDE_PAGE_COLOUR:
                    ColourOnDraw(rt);
                    break;
                case WINDOW_RIDE_PAGE_MUSIC:
                    MusicOnDraw(rt);
                    break;
                case WINDOW_RIDE_PAGE_MEASUREMENTS:
                    MeasurementsOnDraw(rt);
                    break;
                case WINDOW_RIDE_PAGE_GRAPHS:
                    GraphsOnDraw(rt);
                    break;
                case WINDOW_RIDE_PAGE_INCOME:
                    IncomeOnDraw(rt);
                    break;
                case WINDOW_RIDE_PAGE_CUSTOMER:
                    CustomerOnDraw(rt);
                    break;
            }
        }

        OpenRCT2String onTooltip(WidgetIndex widgetIndex, StringId fallback) override
        {
            switch (page)
            {
                case WINDOW_RIDE_PAGE_VEHICLE:
                    return VehicleTooltip(widgetIndex, fallback);
                case WINDOW_RIDE_PAGE_GRAPHS:
                    return GraphsTooltip(widgetIndex, fallback);
            }
            return { fallback, {} };
        }
        void onMouseDown(WidgetIndex widgetIndex) override
        {
            switch (page)
            {
                case WINDOW_RIDE_PAGE_MAIN:
                    MainOnMouseDown(widgetIndex);
                    break;
                case WINDOW_RIDE_PAGE_VEHICLE:
                    VehicleOnMouseDown(widgetIndex);
                    break;
                case WINDOW_RIDE_PAGE_OPERATING:
                    OperatingOnMouseDown(widgetIndex);
                    break;
                case WINDOW_RIDE_PAGE_MAINTENANCE:
                    MaintenanceOnMouseDown(widgetIndex);
                    break;
                case WINDOW_RIDE_PAGE_COLOUR:
                    ColourOnMouseDown(widgetIndex);
                    break;
                case WINDOW_RIDE_PAGE_MUSIC:
                    MusicOnMouseDown(widgetIndex);
                    break;
                case WINDOW_RIDE_PAGE_MEASUREMENTS:
                    MeasurementsOnMouseDown(widgetIndex);
                    break;
                case WINDOW_RIDE_PAGE_GRAPHS:
                    GraphsOnMouseDown(widgetIndex);
                    break;
                case WINDOW_RIDE_PAGE_INCOME:
                    IncomeOnMouseDown(widgetIndex);
                    break;
            }
        }
        void onMouseUp(WidgetIndex widgetIndex) override
        {
            switch (page)
            {
                case WINDOW_RIDE_PAGE_MAIN:
                    MainOnMouseUp(widgetIndex);
                    break;
                case WINDOW_RIDE_PAGE_VEHICLE:
                    VehicleOnMouseUp(widgetIndex);
                    break;
                case WINDOW_RIDE_PAGE_OPERATING:
                    OperatingOnMouseUp(widgetIndex);
                    break;
                case WINDOW_RIDE_PAGE_MAINTENANCE:
                    MaintenanceOnMouseUp(widgetIndex);
                    break;
                case WINDOW_RIDE_PAGE_COLOUR:
                    ColourOnMouseUp(widgetIndex);
                    break;
                case WINDOW_RIDE_PAGE_MUSIC:
                    MusicOnMouseUp(widgetIndex);
                    break;
                case WINDOW_RIDE_PAGE_MEASUREMENTS:
                    MeasurementsOnMouseUp(widgetIndex);
                    break;
                case WINDOW_RIDE_PAGE_GRAPHS:
                    GraphsOnMouseUp(widgetIndex);
                    break;
                case WINDOW_RIDE_PAGE_INCOME:
                    IncomeOnMouseUp(widgetIndex);
                    break;
                case WINDOW_RIDE_PAGE_CUSTOMER:
                    CustomerOnMouseUp(widgetIndex);
                    break;
            }
        }
        void onDropdown(WidgetIndex widgetIndex, int32_t selectedIndex) override
        {
            switch (page)
            {
                case WINDOW_RIDE_PAGE_MAIN:
                    MainOnDropdown(widgetIndex, selectedIndex);
                    break;
                case WINDOW_RIDE_PAGE_VEHICLE:
                    VehicleOnDropdown(widgetIndex, selectedIndex);
                    break;
                case WINDOW_RIDE_PAGE_OPERATING:
                    OperatingOnDropdown(widgetIndex, selectedIndex);
                    break;
                case WINDOW_RIDE_PAGE_MAINTENANCE:
                    MaintenanceOnDropdown(widgetIndex, selectedIndex);
                    break;
                case WINDOW_RIDE_PAGE_COLOUR:
                    ColourOnDropdown(widgetIndex, selectedIndex);
                    break;
                case WINDOW_RIDE_PAGE_MUSIC:
                    MusicOnDropdown(widgetIndex, selectedIndex);
                    break;
                case WINDOW_RIDE_PAGE_MEASUREMENTS:
                    MeasurementsOnDropdown(widgetIndex, selectedIndex);
                    break;
            }
        }
        void onTextInput(WidgetIndex widgetIndex, std::string_view text) override
        {
            switch (page)
            {
                case WINDOW_RIDE_PAGE_MAIN:
                    MainOnTextInput(widgetIndex, text);
                    break;
                case WINDOW_RIDE_PAGE_OPERATING:
                    OperatingOnTextInput(widgetIndex, text);
                    break;
                case WINDOW_RIDE_PAGE_INCOME:
                    IncomeOnTextInput(widgetIndex, text);
                    break;
            }
        }
        ScreenSize onScrollGetSize(int32_t scrollIndex) override
        {
            switch (page)
            {
                case WINDOW_RIDE_PAGE_GRAPHS:
                    return GraphsScrollGetSize(scrollIndex);
                case WINDOW_RIDE_PAGE_MUSIC:
                    return MusicScrollGetSize(scrollIndex);
            }
            return {};
        }
        void onScrollSelect(int32_t scrollIndex, int32_t scrollAreaType) override
        {
            switch (page)
            {
                case WINDOW_RIDE_PAGE_GRAPHS:
                    GraphsOnScrollSelect(scrollIndex, scrollAreaType);
                    break;
            }
        }
        void onScrollDraw(int32_t scrollIndex, RenderTarget& rt) override
        {
            switch (page)
            {
                case WINDOW_RIDE_PAGE_VEHICLE:
                    VehicleOnScrollDraw(rt, scrollIndex);
                    break;
                case WINDOW_RIDE_PAGE_COLOUR:
                    ColourOnScrollDraw(rt, scrollIndex);
                    break;
                case WINDOW_RIDE_PAGE_GRAPHS:
                    GraphsOnScrollDraw(rt, scrollIndex);
                    break;
                case WINDOW_RIDE_PAGE_MUSIC:
                    MusicOnScrollDraw(rt, scrollIndex);
                    break;
            }
        }
        void onToolDown(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords) override
        {
            switch (page)
            {
                case WINDOW_RIDE_PAGE_COLOUR:
                    ColourOnToolDown(widgetIndex, screenCoords);
                    break;
                case WINDOW_RIDE_PAGE_MEASUREMENTS:
                    MeasurementsOnToolDown(widgetIndex, screenCoords);
                    break;
            }
        }
        void onToolDrag(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords) override
        {
            switch (page)
            {
                case WINDOW_RIDE_PAGE_COLOUR:
                    ColourOnToolDrag(widgetIndex, screenCoords);
                    break;
                case WINDOW_RIDE_PAGE_MEASUREMENTS:
                    MeasurementsOnToolDrag(widgetIndex, screenCoords);
                    break;
            }
        }
        void onToolAbort(WidgetIndex widgetIndex) override
        {
            switch (page)
            {
                case WINDOW_RIDE_PAGE_MEASUREMENTS:
                    MeasurementsOnToolAbort(widgetIndex);
                    break;
            }
        }
        void onViewportRotate() override
        {
            switch (page)
            {
                case WINDOW_RIDE_PAGE_MAIN:
                    MainViewportRotate();
                    break;
            }
        }

        void setPage(int32_t newPage)
        {
            if (isToolActive(classification, number))
                ToolCancel();

            if (newPage == WINDOW_RIDE_PAGE_VEHICLE)
            {
                auto* windowMgr = GetWindowManager();
                auto constructionWindow = windowMgr->FindByClass(WindowClass::rideConstruction);
                if (constructionWindow != nullptr && constructionWindow->number == number)
                {
                    windowMgr->CloseByClass(WindowClass::rideConstruction);
                    // Closing the construction window sets the tab to the first page, which we don't want here,
                    // as user just clicked the Vehicle page
                    setPage(WINDOW_RIDE_PAGE_VEHICLE);
                }
            }

            // Set listen only to viewport
            bool listen = false;
            if (newPage == WINDOW_RIDE_PAGE_MAIN && page == WINDOW_RIDE_PAGE_MAIN && viewport != nullptr)
            {
                viewport->flags ^= VIEWPORT_FLAG_SOUND_ON;
                listen = (viewport->flags & VIEWPORT_FLAG_SOUND_ON) != 0;
            }

            // Skip setting page if we're already on this page, unless we're initialising the window
            if (page == newPage && !widgets.empty())
                return;

            page = newPage;
            currentFrame = 0;
            pickedPeepFrame = 0;

            // There doesn't seem to be any need for this call, and it can sometimes modify the reported number of cars per
            // train, so I've removed it if (newPage == WINDOW_RIDE_PAGE_VEHICLE) { ride_update_max_vehicles(ride);
            //}

            removeViewport();

            holdDownWidgets = PageHoldDownWidgets[page];
            pressedWidgets = 0;
            setWidgets(PageWidgets[page]);
            DisableTabs();
            invalidate();

            onResize();
            onPrepareDraw();
            initScrollWidgets();
            invalidate();

            if (listen && viewport != nullptr)
                viewport->flags |= VIEWPORT_FLAG_SOUND_ON;
        }

        void setViewIndex(int16_t newIndex)
        {
            _viewIndex = newIndex;
            onViewportRotate();
        }
        int16_t getViewIndex() const
        {
            return _viewIndex;
        }

        void resetVehicleIndex()
        {
            _vehicleIndex = dropdownIndexToCarIndex(0);
        }

    private:
        void drawTabImage(RenderTarget& rt, int32_t tab, int32_t spriteIndex)
        {
            WidgetIndex widgetIndex = WIDX_TAB_1 + tab;

            if (!widgetIsDisabled(*this, widgetIndex))
            {
                if (page == tab)
                {
                    int32_t frame = currentFrame / PageTabAnimationDivisor[page];
                    spriteIndex += (frame % PageTabAnimationNumFrames[page]);
                }

                const auto& widget = widgets[widgetIndex];
                GfxDrawSprite(rt, ImageId(spriteIndex), windowPos + ScreenCoordsXY{ widget.left, widget.top });
            }
        }

        void DrawTabMain(RenderTarget& rt)
        {
            WidgetIndex widgetIndex = WIDX_TAB_1 + static_cast<int32_t>(WINDOW_RIDE_PAGE_MAIN);
            if (!widgetIsDisabled(*this, widgetIndex))
            {
                auto ride = GetRide(rideId);
                if (ride != nullptr)
                {
                    int32_t spriteIndex = 0;
                    switch (ride->getClassification())
                    {
                        case RideClassification::ride:
                            spriteIndex = SPR_TAB_RIDE_0;
                            if (page == WINDOW_RIDE_PAGE_MAIN)
                                spriteIndex += (currentFrame / 4) % 16;
                            break;
                        case RideClassification::shopOrStall:
                            spriteIndex = SPR_TAB_SHOPS_AND_STALLS_0;
                            if (page == WINDOW_RIDE_PAGE_MAIN)
                                spriteIndex += (currentFrame / 4) % 16;
                            break;
                        case RideClassification::kioskOrFacility:
                            spriteIndex = SPR_TAB_KIOSKS_AND_FACILITIES_0;
                            if (page == WINDOW_RIDE_PAGE_MAIN)
                                spriteIndex += (currentFrame / 4) % 8;
                            break;
                    }

                    const auto& widget = widgets[widgetIndex];
                    GfxDrawSprite(rt, ImageId(spriteIndex), windowPos + ScreenCoordsXY{ widget.left, widget.top });
                }
            }
        }

        void DrawTabVehicle(RenderTarget& rt)
        {
            WidgetIndex widgetIndex = WIDX_TAB_1 + static_cast<int32_t>(WINDOW_RIDE_PAGE_VEHICLE);
            const auto& widget = widgets[widgetIndex];

            if (!widgetIsDisabled(*this, widgetIndex))
            {
                const Ride* const ride = GetRide(rideId);
                if (ride == nullptr)
                    return;

                const auto* const rideEntry = ride->getRideEntry();
                if (rideEntry == nullptr)
                    return;

                const bool isHalfScale = rideEntry->flags & RIDE_ENTRY_FLAG_VEHICLE_TAB_SCALE_HALF;

                auto screenCoords = ScreenCoordsXY{ widget.left + 1, widget.top + 1 };
                int32_t clipWidth = widget.right - screenCoords.x;
                int32_t clipHeight = widget.bottom - screenCoords.y;
                if (page == WINDOW_RIDE_PAGE_VEHICLE && !isHalfScale)
                {
                    clipHeight += 4;
                }

                screenCoords += windowPos;

                RenderTarget clipRT;
                if (!ClipRenderTarget(clipRT, rt, screenCoords, clipWidth, clipHeight))
                {
                    return;
                }

                auto spriteCoords = ScreenCoordsXY{ widget.width() / 2, 17 };

                if (isHalfScale)
                {
                    clipRT.zoom_level = ZoomLevel{ 1 };
                    spriteCoords.x *= 2;
                    spriteCoords.y *= 2;
                }

                const auto vehicle = RideEntryGetVehicleAtPosition(ride->subtype, ride->numCarsPerTrain, rideEntry->TabCar);
                const auto& carEntry = rideEntry->Cars[vehicle];

                spriteCoords.y += carEntry.tab_height;

                auto vehicleId = (ride->vehicleColourSettings == VehicleColourSettings::perCar) ? rideEntry->TabCar : 0;
                VehicleColour vehicleColour = RideGetVehicleColour(*ride, vehicleId);

                // imageIndex represents a precision of 64
                auto imageIndex = Entity::Yaw::YawFrom4(2) * 2;
                if (page == WINDOW_RIDE_PAGE_VEHICLE)
                    imageIndex += currentFrame;
                imageIndex = carEntry.SpriteByYaw(imageIndex / 2, SpriteGroupType::SlopeFlat);
                imageIndex &= carEntry.TabRotationMask;
                imageIndex *= carEntry.base_num_frames;
                imageIndex += carEntry.base_image_id;
                auto imageId = ImageId(imageIndex, vehicleColour.Body, vehicleColour.Trim, vehicleColour.Tertiary);
                GfxDrawSprite(clipRT, imageId, spriteCoords);
            }
        }

        void DrawTabCustomer(RenderTarget& rt)
        {
            WidgetIndex widgetIndex = WIDX_TAB_1 + static_cast<int32_t>(WINDOW_RIDE_PAGE_CUSTOMER);

            if (!widgetIsDisabled(*this, widgetIndex))
            {
                const auto& widget = widgets[widgetIndex];
                int32_t spriteIndex = 0;
                if (page == WINDOW_RIDE_PAGE_CUSTOMER)
                    spriteIndex = pickedPeepFrame & ~3;

                auto* animObj = findPeepAnimationsObjectForType(AnimationPeepType::guest);
                spriteIndex += animObj->GetPeepAnimation(PeepAnimationGroup::normal).baseImage + 1;

                GfxDrawSprite(
                    rt, ImageId(spriteIndex, Drawing::Colour::brightRed, Drawing::Colour::darkWater),
                    windowPos + ScreenCoordsXY{ widget.midX(), widget.bottom - 6 });
            }
        }

        void drawTabImages(RenderTarget& rt)
        {
            DrawTabVehicle(rt);
            drawTabImage(rt, WINDOW_RIDE_PAGE_OPERATING, SPR_TAB_GEARS_0);
            drawTabImage(rt, WINDOW_RIDE_PAGE_MAINTENANCE, SPR_TAB_WRENCH_0);
            drawTabImage(rt, WINDOW_RIDE_PAGE_INCOME, SPR_TAB_ADMISSION_0);
            DrawTabMain(rt);
            drawTabImage(rt, WINDOW_RIDE_PAGE_MEASUREMENTS, SPR_TAB_TIMER_0);
            drawTabImage(rt, WINDOW_RIDE_PAGE_COLOUR, SPR_TAB_PAINT_0);
            drawTabImage(rt, WINDOW_RIDE_PAGE_GRAPHS, SPR_TAB_GRAPH_A_0);
            DrawTabCustomer(rt);
            drawTabImage(rt, WINDOW_RIDE_PAGE_MUSIC, SPR_TAB_MUSIC_0);
        }

        void DisableTabs()
        {
            uint32_t disabledTabs = 0;
            auto ride = GetRide(rideId);
            if (ride == nullptr)
                return;

            const auto& rtd = ride->getRideTypeDescriptor();

            if (!rtd.HasFlag(RtdFlag::hasDataLogging))
                disabledTabs |= (1uLL << WIDX_TAB_8); // 0x800

            if (rtd.specialType == RtdSpecialType::miniGolf)
                disabledTabs |= (1uLL << WIDX_TAB_2 | 1uLL << WIDX_TAB_3 | 1uLL << WIDX_TAB_4); // 0xE0

            if (rtd.HasFlag(RtdFlag::noVehicles))
                disabledTabs |= (1uLL << WIDX_TAB_2); // 0x20

            if (!rtd.HasFlag(RtdFlag::hasTrackColourMain) && !rtd.HasFlag(RtdFlag::hasTrackColourAdditional)
                && !rtd.HasFlag(RtdFlag::hasTrackColourSupports) && !rtd.HasFlag(RtdFlag::hasVehicleColours)
                && !rtd.HasFlag(RtdFlag::hasEntranceAndExit))
            {
                disabledTabs |= (1uLL << WIDX_TAB_5); // 0x100
            }

            if (rtd.HasFlag(RtdFlag::isShopOrFacility))
                disabledTabs |= (1uLL << WIDX_TAB_3 | 1uLL << WIDX_TAB_4 | 1uLL << WIDX_TAB_7); // 0x4C0

            if (!rtd.HasFlag(RtdFlag::allowMusic))
            {
                disabledTabs |= (1uLL << WIDX_TAB_6); // 0x200
            }

            if (rtd.specialType == RtdSpecialType::cashMachine || rtd.specialType == RtdSpecialType::firstAid
                || (getGameState().park.flags & PARK_FLAGS_NO_MONEY) != 0)
                disabledTabs |= (1uLL << WIDX_TAB_9); // 0x1000

            if (gLegacyScene == LegacyScene::trackDesigner)
                disabledTabs |= (1uLL << WIDX_TAB_4 | 1uLL << WIDX_TAB_6 | 1uLL << WIDX_TAB_9 | 1uLL << WIDX_TAB_10); // 0x3280

            const auto* rideEntry = GetRideEntryByIndex(ride->subtype);

            if (rideEntry == nullptr)
            {
                disabledTabs |= 1uLL << WIDX_TAB_2 | 1uLL << WIDX_TAB_3 | 1uLL << WIDX_TAB_4 | 1uLL << WIDX_TAB_5
                    | 1uLL << WIDX_TAB_6 | 1uLL << WIDX_TAB_7 | 1uLL << WIDX_TAB_8 | 1uLL << WIDX_TAB_9 | 1uLL << WIDX_TAB_10;
            }
            else if ((rideEntry->flags & RIDE_ENTRY_FLAG_DISABLE_COLOUR_TAB) != 0)
            {
                disabledTabs |= (1uLL << WIDX_TAB_5);
            }

            disabledWidgets = disabledTabs;
        }

        void UpdateOverallView(const Ride& ride) const
        {
            // Calculate x, y, z bounds of the entire ride using its track elements
            TileElementIterator it;

            TileElementIteratorBegin(&it);

            CoordsXYZ min = { std::numeric_limits<int32_t>::max(), std::numeric_limits<int32_t>::max(),
                              std::numeric_limits<int32_t>::max() };
            CoordsXYZ max = { std::numeric_limits<int32_t>::min(), std::numeric_limits<int32_t>::min(),
                              std::numeric_limits<int32_t>::min() };

            while (TileElementIteratorNext(&it))
            {
                if (it.element->GetType() != TileElementType::Track)
                    continue;

                if (it.element->AsTrack()->GetRideIndex() != ride.id)
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

            const auto rideIndex = ride.id.ToUnderlying();
            if (rideIndex >= _rideOverallViewsCache.size())
            {
                _rideOverallViewsCache.resize(rideIndex + 1);
            }

            auto& view = _rideOverallViewsCache[rideIndex];
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

        void SetPressedTab()
        {
            for (int32_t i = 0; i < WINDOW_RIDE_PAGE_COUNT; i++)
                pressedWidgets &= ~(1 << (WIDX_TAB_1 + i));
            pressedWidgets |= 1LL << (WIDX_TAB_1 + page);
        }

#pragma region Main

        std::optional<StationIndex> GetStationIndexFromViewSelection() const
        {
            const auto* ride = GetRide(RideId::FromUnderlying(number));
            if (ride == nullptr)
                return std::nullopt;

            int32_t viewSelectionIndex = _viewIndex - 1 - ride->numTrains;
            if (viewSelectionIndex < 0)
            {
                return std::nullopt;
            }

            for (const auto& station : ride->getStations())
            {
                if (!station.Start.IsNull() && viewSelectionIndex-- == 0)
                {
                    const auto stationIndex = ride->getStationIndex(&station);
                    return std::make_optional(stationIndex);
                }
            }
            return std::nullopt;
        }

        void initViewport()
        {
            if (page != WINDOW_RIDE_PAGE_MAIN)
                return;

            auto ride = GetRide(rideId);
            if (ride == nullptr)
                return;

            int32_t viewSelectionIndex = _viewIndex - 1;

            std::optional<Focus> newFocus;

            if (viewSelectionIndex >= 0 && viewSelectionIndex < ride->numTrains
                && ride->lifecycleFlags & RIDE_LIFECYCLE_ON_TRACK)
            {
                auto vehId = ride->vehicles[viewSelectionIndex];
                const auto* rideEntry = ride->getRideEntry();
                if (rideEntry != nullptr && rideEntry->TabCar != 0)
                {
                    Vehicle* vehicle = getGameState().entities.GetEntity<Vehicle>(vehId);
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
            else if (viewSelectionIndex >= ride->numTrains && viewSelectionIndex < (ride->numTrains + ride->numStations))
            {
                auto stationIndex = GetStationIndexFromViewSelection();
                if (stationIndex)
                {
                    const auto location = ride->getStation(*stationIndex).GetStart();
                    newFocus = Focus(location);
                }
            }
            else
            {
                if (viewSelectionIndex > 0)
                {
                    _viewIndex = 0;
                }
                if (static_cast<uint16_t>(number) < _rideOverallViewsCache.size())
                {
                    const auto& view = _rideOverallViewsCache[number];
                    newFocus = Focus(view.loc, view.zoom);
                }
            }

            uint16_t newViewportFlags = 0;
            if (viewport != nullptr)
            {
                if (focus == newFocus)
                {
                    return;
                }
                newViewportFlags = viewport->flags;
                removeViewport();
            }
            else if (Config::Get().general.alwaysShowGridlines)
            {
                newViewportFlags |= VIEWPORT_FLAG_GRIDLINES;
            }

            onPrepareDraw();

            focus = newFocus;

            // rct2: 0x006aec9c only used here so brought it into the function
            if (viewport == nullptr && !ride->overallView.IsNull() && focus.has_value())
            {
                const auto& viewWidget = widgets[WIDX_VIEWPORT];

                auto screenPos = windowPos + ScreenCoordsXY{ viewWidget.left + 1, viewWidget.top + 1 };
                int32_t viewWidth = viewWidget.width() - 2;
                int32_t viewHeight = viewWidget.height() - 2;

                ViewportCreate(*this, screenPos, viewWidth, viewHeight, focus.value());

                flags |= WindowFlag::noScrolling;
                invalidate();
            }
            if (viewport != nullptr)
            {
                viewport->flags = newViewportFlags;
                invalidate();
            }
        }

        void Rename()
        {
            auto ride = GetRide(rideId);
            if (ride != nullptr)
            {
                auto rideName = ride->getName();
                WindowTextInputRawOpen(
                    this, WIDX_RENAME, STR_RIDE_ATTRACTION_NAME, STR_ENTER_NEW_NAME_FOR_THIS_RIDE_ATTRACTION, {},
                    rideName.c_str(), 32);
            }
        }

        void MainOnMouseUp(WidgetIndex widgetIndex)
        {
            switch (widgetIndex)
            {
                case WIDX_CLOSE:
                    close();
                    return;
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
                    setPage(widgetIndex - WIDX_TAB_1);
                    break;
                case WIDX_CONSTRUCTION:
                {
                    auto ride = GetRide(rideId);
                    if (ride != nullptr)
                    {
                        RideConstructionStart(*ride);
                        auto* windowMgr = GetWindowManager();
                        if (windowMgr->FindByNumber(WindowClass::rideConstruction, ride->id.ToUnderlying()) != nullptr)
                        {
                            close();
                            return;
                        }
                    }
                    break;
                }
                case WIDX_RENAME:
                    Rename();
                    break;
                case WIDX_DEMOLISH:
                    ContextOpenDetailWindow(WindowDetail::demolishRide, number);
                    break;
                case WIDX_CLOSE_LIGHT:
                case WIDX_SIMULATE_LIGHT:
                case WIDX_TEST_LIGHT:
                case WIDX_OPEN_LIGHT:
                {
                    auto ride = GetRide(rideId);
                    if (ride != nullptr)
                    {
                        RideStatus status;
                        switch (widgetIndex)
                        {
                            default:
                            case WIDX_CLOSE_LIGHT:
                                status = RideStatus::closed;
                                break;
                            case WIDX_SIMULATE_LIGHT:
                                status = RideStatus::simulating;
                                break;
                            case WIDX_TEST_LIGHT:
                                status = RideStatus::testing;
                                break;
                            case WIDX_OPEN_LIGHT:
                                status = RideStatus::open;
                                break;
                        }
                        auto gameAction = GameActions::RideSetStatusAction(ride->id, status);
                        GameActions::Execute(&gameAction, getGameState());
                    }
                    break;
                }
            }
        }

        void MainResize()
        {
            int32_t newMinHeight = 180;
            if (ThemeGetFlags() & UITHEME_FLAG_USE_LIGHTS_RIDE)
            {
                newMinHeight += 20 + kRCT1LightOffset;

                auto ride = GetRide(rideId);
                if (ride != nullptr)
                {
#ifdef __SIMULATE_IN_RIDE_WINDOW__
                    if (ride->supportsStatus(RideStatus::simulating))
                    {
                        newMinHeight += 14;
                    }
#endif
                    if (ride->supportsStatus(RideStatus::testing))
                    {
                        newMinHeight += 14;
                    }
                }
            }
            if (getGameState().cheats.allowArbitraryRideTypeChanges)
            {
                newMinHeight += 15;
            }

            flags |= WindowFlag::resizable;
            WindowSetResize(*this, { kMinimumWindowWidth, newMinHeight }, { 500, 450 });
            // Unlike with other windows, the focus needs to be recentred so it’s best to just reset it.
            focus = std::nullopt;
            initViewport();
        }

        size_t GetNumPeepsInTrain(const Ride& ride, int32_t trainIndex) const
        {
            auto numPeepsInTrain = 0;
            const auto* vehicle = TryGetVehicle(ride.vehicles[trainIndex]);
            while (vehicle != nullptr)
            {
                numPeepsInTrain += vehicle->num_peeps;
                vehicle = TryGetVehicle(vehicle->next_vehicle_on_train);
            }
            return numPeepsInTrain;
        }

        bool TrainMustBeHidden(const Ride& ride, int32_t trainIndex) const
        {
            if (!(ride.lifecycleFlags & RIDE_LIFECYCLE_ON_TRACK))
                return true;

            const auto* rideEntry = ride.getRideEntry();
            if (rideEntry == nullptr)
                return false;

            if (!(rideEntry->flags & RIDE_ENTRY_FLAG_HIDE_EMPTY_TRAINS))
                return false;

            return GetNumPeepsInTrain(ride, trainIndex) == 0;
        }

        void ShowViewDropdown(Widget* widget)
        {
            Widget* dropdownWidget = widget - 1;
            auto ride = GetRide(rideId);
            if (ride == nullptr)
                return;

            const auto& rtd = ride->getRideTypeDescriptor();

            int32_t numItems = 1;
            if (!rtd.HasFlag(RtdFlag::noVehicles))
            {
                numItems += ride->numStations;
                numItems += ride->numTrains;
            }

            WindowDropdownShowTextCustomWidth(
                { windowPos.x + dropdownWidget->left, windowPos.y + dropdownWidget->top }, dropdownWidget->height(), colours[1],
                0, 0, numItems, widget->right - dropdownWidget->left);

            // First item
            gDropdown.items[0] = Dropdown::MenuLabel(STR_OVERALL_VIEW);
            int32_t currentItem = 1;

            // Vehicles
            int32_t name = GetRideComponentName(rtd.NameConvention.vehicle).number;
            for (int32_t i = 0; i < ride->numTrains; i++)
            {
                Formatter ft;
                ft.Add<uint16_t>(i + 1);
                gDropdown.items[currentItem] = Dropdown::MenuLabel(name, ft);

                if (TrainMustBeHidden(*ride, i))
                {
                    gDropdown.items[currentItem].setDisabled(true);
                }
                currentItem++;
            }

            // Stations
            name = GetRideComponentName(rtd.NameConvention.station).number;
            for (int32_t i = 0; i < ride->numStations; i++)
            {
                Formatter ft;
                ft.Add<uint16_t>(i + 1);
                gDropdown.items[currentItem] = Dropdown::MenuLabel(name, ft);
                currentItem++;
            }

            // Set checked item
            gDropdown.items[_viewIndex].setChecked(true);
        }

        RideStatus GetNextDefaultStatus(const Ride& ride) const
        {
            switch (ride.status)
            {
                default:
                case RideStatus::closed:
                    if ((ride.lifecycleFlags & RIDE_LIFECYCLE_CRASHED)
                        || (ride.lifecycleFlags & RIDE_LIFECYCLE_HAS_STALLED_VEHICLE))
                    {
                        return RideStatus::closed;
                    }
                    if (ride.supportsStatus(RideStatus::testing) && !(ride.lifecycleFlags & RIDE_LIFECYCLE_TESTED))
                    {
                        return RideStatus::testing;
                    }
                    return RideStatus::open;
                case RideStatus::simulating:
                    return RideStatus::testing;
                case RideStatus::testing:
                    return (ride.lifecycleFlags & RIDE_LIFECYCLE_TESTED) ? RideStatus::open : RideStatus::closed;
                case RideStatus::open:
                    return RideStatus::closed;
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

        void SetDropdown(RideStatusDropdownInfo& info, RideStatus status, StringId text) const
        {
            if (info.Ride->supportsStatus(status))
            {
                auto index = info.NumItems;
                gDropdown.items[index] = Dropdown::MenuLabel(text);
                gDropdown.items[index].value = EnumValue(status);
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

        void ShowOpenDropdown(Widget* widget)
        {
            RideStatusDropdownInfo info;
            info.Ride = GetRide(rideId);
            if (info.Ride == nullptr)
                return;

            info.CurrentStatus = info.Ride->status;
            info.DefaultStatus = GetNextDefaultStatus(*info.Ride);
            SetDropdown(info, RideStatus::closed, STR_CLOSE_RIDE);
#ifdef __SIMULATE_IN_RIDE_WINDOW__
            SetDropdown(info, RideStatus::simulating, STR_SIMULATE_RIDE);
#endif
            SetDropdown(info, RideStatus::testing, STR_TEST_RIDE);
            SetDropdown(info, RideStatus::open, STR_OPEN_RIDE);
            WindowDropdownShowText(
                { windowPos.x + widget->left, windowPos.y + widget->top }, widget->height(), colours[1], 0, info.NumItems);
            if (info.CheckedIndex != -1)
            {
                gDropdown.items[info.CheckedIndex].setChecked(true);
            }
            gDropdown.defaultIndex = info.DefaultIndex;
        }

        static constexpr StringId GetRideTypeNameForDropdown(ride_type_t rideType)
        {
            auto stringId = GetRideTypeDescriptor(rideType).Naming.Name;
            if (stringId == STR_UNKNOWN_RIDE)
            {
                stringId = STR_RIDE_NAME_UNKNOWN_INT32;
            }
            return stringId;
        }

        void PopulateRideTypeDropdown()
        {
            auto& ls = GetContext()->GetLocalisationService();
            if (_rideDropdownDataLanguage == ls.GetCurrentLanguage())
                return;

            _rideDropdownData.clear();

            for (uint8_t i = 0; i < RIDE_TYPE_COUNT; i++)
            {
                // Will return the actual name for most rides, but a special string "Unknown Ride ({INT32})" for unknown ones.
                // The placeholder will then be filled with the ID.
                auto name = GetRideTypeNameForDropdown(i);
                auto ft = Formatter();
                ft.Add<int32_t>(i);
                auto label = FormatStringIDLegacy(name, ft.Data());
                _rideDropdownData.push_back({ i, name, label });
            }

            std::sort(_rideDropdownData.begin(), _rideDropdownData.end(), [](auto& a, auto& b) {
                return a.LabelString.compare(b.LabelString) < 0;
            });

            _rideDropdownDataLanguage = ls.GetCurrentLanguage();
        }

        void ShowRideTypeDropdown(Widget* widget)
        {
            auto ride = GetRide(rideId);
            if (ride == nullptr)
                return;

            PopulateRideTypeDropdown();

            for (size_t i = 0; i < _rideDropdownData.size(); i++)
            {
                gDropdown.items[i] = Dropdown::MenuLabel(_rideDropdownData[i].LabelString);
            }

            Widget* dropdownWidget = widget - 1;
            WindowDropdownShowText(
                { windowPos.x + dropdownWidget->left, windowPos.y + dropdownWidget->top }, dropdownWidget->height(), colours[1],
                Dropdown::Flag::StayOpen, RIDE_TYPE_COUNT);

            // Find the current ride type in the ordered list.
            int32_t pos = 0;
            for (int32_t i = 0; i < RIDE_TYPE_COUNT; i++)
            {
                if (_rideDropdownData[i].RideTypeId == ride->type)
                {
                    pos = i;
                    break;
                }
            }

            gDropdown.highlightedIndex = pos;
            gDropdown.defaultIndex = pos;
            gDropdown.items[pos].setChecked(true);
        }

        void ShowLocateDropdown(Widget* widget)
        {
            auto ride = GetRide(rideId);
            if (ride == nullptr)
                return;

            gDropdown.items[0] = Dropdown::PlainMenuLabel(STR_LOCATE_SUBJECT_TIP);
            gDropdown.items[1] = Dropdown::PlainMenuLabel(STR_FOLLOW_SUBJECT_TIP);

            WindowDropdownShowText(
                { windowPos.x + widget->left, windowPos.y + widget->top }, widget->height(), colours[1], 0, 2);
            gDropdown.defaultIndex = 0;
            if (!ride->getRideTypeDescriptor().HasFlag(RtdFlag::hasTrack) || _viewIndex == 0 || _viewIndex > ride->numTrains)
            {
                // Disable if we're a flat ride, 'overall view' is selected or a station is selected
                gDropdown.items[1].setDisabled(true);
            }
        }

        void MainFollowRide()
        {
            auto* ride = GetRide(rideId);
            if (ride != nullptr)
            {
                if (!ride->windowInvalidateFlags.has(RideInvalidateFlag::main))
                {
                    if (_viewIndex > 0)
                    {
                        if (_viewIndex <= ride->numTrains)
                        {
                            Vehicle* vehicle = getGameState().entities.GetEntity<Vehicle>(ride->vehicles[_viewIndex - 1]);
                            if (vehicle != nullptr)
                            {
                                auto headVehicleSpriteIndex = vehicle->Id;
                                WindowBase* w_main = WindowGetMain();
                                WindowFollowSprite(*w_main, headVehicleSpriteIndex);
                            }
                        }
                    }
                }
            }
        }

        void PopulateVehicleTypeDropdown(const Ride& ride, bool forceRefresh = false)
        {
            auto& objManager = GetContext()->GetObjectManager();
            const auto* rideEntry = ride.getRideEntry();

            bool selectionShouldBeExpanded;
            ride_type_t rideTypeIterator, rideTypeIteratorMax;

            const auto& gameState = getGameState();
            const auto& rtd = ride.getRideTypeDescriptor();
            if (gameState.cheats.showVehiclesFromOtherTrackTypes
                && !(
                    rtd.HasFlag(RtdFlag::isFlatRide) || rtd.specialType == RtdSpecialType::maze
                    || rtd.specialType == RtdSpecialType::miniGolf))
            {
                selectionShouldBeExpanded = true;
                rideTypeIterator = 0;
                rideTypeIteratorMax = RIDE_TYPE_COUNT - 1;
            }
            else
            {
                selectionShouldBeExpanded = false;
                rideTypeIterator = ride.type;
                rideTypeIteratorMax = ride.type;
            }

            // Don't repopulate the list if we just did.
            auto& ls = GetContext()->GetLocalisationService();
            if (!forceRefresh && _vehicleDropdownExpanded == selectionShouldBeExpanded && _vehicleDropdownRideType == rideEntry
                && _vehicleDropdownDataLanguage == ls.GetCurrentLanguage())
                return;

            _vehicleDropdownData.clear();

            for (; rideTypeIterator <= rideTypeIteratorMax; rideTypeIterator++)
            {
                const auto& rtdIterator = GetRideTypeDescriptor(rideTypeIterator);
                if (selectionShouldBeExpanded && rtdIterator.HasFlag(RtdFlag::isFlatRide))
                    continue;
                if (selectionShouldBeExpanded
                    && (rtdIterator.specialType == RtdSpecialType::maze || rtd.specialType == RtdSpecialType::miniGolf))
                    continue;

                auto& rideEntries = objManager.GetAllRideEntries(rideTypeIterator);
                for (auto rideEntryIndex : rideEntries)
                {
                    const auto* currentRideEntry = GetRideEntryByIndex(rideEntryIndex);
                    if (currentRideEntry == nullptr)
                        continue;

                    // Skip if vehicle type has not been invented yet
                    if (!RideEntryIsInvented(rideEntryIndex) && !gameState.cheats.ignoreResearchStatus)
                        continue;

                    auto name = currentRideEntry->naming.Name;
                    _vehicleDropdownData.push_back({ rideEntryIndex, name, u8string_view{ ls.GetString(name) } });
                }
            }

            std::sort(_vehicleDropdownData.begin(), _vehicleDropdownData.end(), [](auto& a, auto& b) {
                return a.LabelString.compare(b.LabelString) < 0;
            });

            _vehicleDropdownExpanded = selectionShouldBeExpanded;
            _vehicleDropdownRideType = rideEntry;
            _vehicleDropdownDataLanguage = ls.GetCurrentLanguage();
        }

        void ShowVehicleTypeDropdown(Widget* widget)
        {
            auto ride = GetRide(rideId);
            if (ride == nullptr)
                return;

            PopulateVehicleTypeDropdown(*ride);

            size_t numItems = std::min<size_t>(_vehicleDropdownData.size(), Dropdown::kItemsMaxSize);

            for (size_t i = 0; i < numItems; i++)
            {
                gDropdown.items[i] = Dropdown::MenuLabel(_vehicleDropdownData[i].LabelId);
            }

            Widget* dropdownWidget = widget - 1;
            WindowDropdownShowTextCustomWidth(
                { windowPos.x + dropdownWidget->left, windowPos.y + dropdownWidget->top }, dropdownWidget->height(), colours[1],
                0, Dropdown::Flag::StayOpen, numItems, dropdownWidget->width() - 1);

            // Find the current vehicle type in the ordered list.
            int32_t pos = 0;
            for (int32_t i = 0; i < static_cast<int32_t>(_vehicleDropdownData.size()); i++)
            {
                if (_vehicleDropdownData[i].SubTypeId == ride->subtype)
                {
                    pos = i;
                    break;
                }
            }

            gDropdown.highlightedIndex = pos;
            gDropdown.defaultIndex = pos;
            gDropdown.items[pos].setChecked(true);
        }

        void PopulateEntranceStyleDropdown()
        {
            auto& ls = GetContext()->GetLocalisationService();
            if (_entranceDropdownDataLanguage == ls.GetCurrentLanguage())
                return;

            _entranceDropdownData.clear();

            auto& objManager = GetContext()->GetObjectManager();

            for (ObjectEntryIndex i = 0; i < kMaxStationObjects; i++)
            {
                auto stationObj = objManager.GetLoadedObject<StationObject>(i);
                if (stationObj != nullptr)
                {
                    auto name = stationObj->NameStringId;
                    _entranceDropdownData.push_back({ i, name, u8string_view{ ls.GetString(name) } });
                }
            }

            std::sort(_entranceDropdownData.begin(), _entranceDropdownData.end(), [](auto& a, auto& b) {
                return a.LabelString.compare(b.LabelString) < 0;
            });

            _entranceDropdownDataLanguage = ls.GetCurrentLanguage();
        }

        void ShowEntranceStyleDropdown()
        {
            auto dropdownWidget = &widgets[WIDX_ENTRANCE_STYLE_DROPDOWN] - 1;
            auto ride = GetRide(rideId);

            PopulateEntranceStyleDropdown();

            for (size_t i = 0; i < _entranceDropdownData.size(); i++)
            {
                gDropdown.items[i] = Dropdown::MenuLabel(_entranceDropdownData[i].LabelId);
                if (_entranceDropdownData[i].EntranceTypeId == ride->entranceStyle)
                    gDropdown.items[i].setChecked(true);
            }

            WindowDropdownShowTextCustomWidth(
                { windowPos.x + dropdownWidget->left, windowPos.y + dropdownWidget->top }, dropdownWidget->height(), colours[1],
                0, Dropdown::Flag::StayOpen, _entranceDropdownData.size(),
                widgets[WIDX_ENTRANCE_STYLE_DROPDOWN].right - dropdownWidget->left);
        }

        void MainOnMouseDown(WidgetIndex widgetIndex)
        {
            switch (widgetIndex)
            {
                case WIDX_VIEW_DROPDOWN:
                    ShowViewDropdown(&widgets[widgetIndex]);
                    break;
                case WIDX_OPEN:
                    ShowOpenDropdown(&widgets[widgetIndex]);
                    break;
                case WIDX_RIDE_TYPE_DROPDOWN:
                    ShowRideTypeDropdown(&widgets[widgetIndex]);
                    break;
                case WIDX_LOCATE:
                    ShowLocateDropdown(&widgets[widgetIndex]);
                    break;
            }
        }

        void MainOnDropdown(WidgetIndex widgetIndex, int32_t dropdownIndex)
        {
            auto& gameState = getGameState();
            switch (widgetIndex)
            {
                case WIDX_VIEW_DROPDOWN:
                    if (dropdownIndex == -1)
                    {
                        dropdownIndex = _viewIndex + 1;
                        auto ride = GetRide(rideId);
                        if (ride != nullptr)
                        {
                            if (dropdownIndex != 0 && dropdownIndex <= ride->numTrains
                                && !(ride->lifecycleFlags & RIDE_LIFECYCLE_ON_TRACK))
                            {
                                dropdownIndex = ride->numTrains + 1;
                            }
                            if (dropdownIndex >= gDropdown.numItems)
                            {
                                dropdownIndex = 0;
                            }
                        }
                    }

                    _viewIndex = dropdownIndex;
                    initViewport();
                    invalidate();
                    break;
                case WIDX_OPEN:
                {
                    auto ride = GetRide(rideId);
                    if (ride != nullptr)
                    {
                        auto status = RideStatus::closed;
                        if (dropdownIndex < 0)
                        {
                            dropdownIndex = gDropdown.highlightedIndex;
                        }
                        if (dropdownIndex < static_cast<int32_t>(std::size(gDropdown.items)))
                        {
                            status = static_cast<RideStatus>(gDropdown.items[dropdownIndex].value);
                        }
                        auto gameAction = GameActions::RideSetStatusAction(ride->id, status);
                        GameActions::Execute(&gameAction, gameState);
                    }
                    break;
                }
                case WIDX_RIDE_TYPE_DROPDOWN:
                    if (dropdownIndex != -1 && dropdownIndex < RIDE_TYPE_COUNT)
                    {
                        auto rideLabelId = std::clamp(dropdownIndex, 0, RIDE_TYPE_COUNT - 1);
                        auto rideType = _rideDropdownData[rideLabelId].RideTypeId;
                        if (rideType < RIDE_TYPE_COUNT)
                        {
                            auto rideSetSetting = GameActions::RideSetSettingAction(
                                rideId, GameActions::RideSetSetting::RideType, rideType);
                            rideSetSetting.SetCallback(
                                [](const GameActions::GameAction* ga, const GameActions::Result* result) {
                                    // Reset ghost track if ride construction window is open, prevents a crash
                                    // Will get set to the correct Alternative variable during set_default_next_piece.
                                    // TODO: Rework construction window to prevent the need for this.
                                    _currentTrackAlternative.clearAll();
                                    RideConstructionSetDefaultNextPiece();
                                });
                            GameActions::Execute(&rideSetSetting, gameState);
                        }
                    }
                    break;
                case WIDX_LOCATE:
                {
                    if (dropdownIndex == 0)
                    {
                        scrollToViewport();
                    }
                    else if (dropdownIndex == 1)
                    {
                        MainFollowRide();
                    }
                    break;
                }
            }
        }

        void MainUpdate()
        {
            // Update tab animation
            currentFrame++;
            onPrepareDraw();
            invalidateWidget(WIDX_TAB_1);

            // Update status
            auto ride = GetRide(rideId);
            if (ride != nullptr)
            {
                if (!ride->windowInvalidateFlags.has(RideInvalidateFlag::main))
                {
                    if (_viewIndex == 0)
                        return;

                    if (_viewIndex <= ride->numTrains)
                    {
                        Vehicle* vehicle = getGameState().entities.GetEntity<Vehicle>(ride->vehicles[_viewIndex - 1]);
                        if (vehicle == nullptr
                            || (vehicle->status != Vehicle::Status::travelling
                                && vehicle->status != Vehicle::Status::travellingCableLift
                                && vehicle->status != Vehicle::Status::travellingDodgems
                                && vehicle->status != Vehicle::Status::travellingBoat))
                        {
                            return;
                        }
                    }
                }
                ride->windowInvalidateFlags.unset(RideInvalidateFlag::main);
            }
            invalidateWidget(WIDX_STATUS);
        }

        void MainOnTextInput(WidgetIndex widgetIndex, std::string_view text)
        {
            if (widgetIndex != WIDX_RENAME || text.empty())
                return;

            auto ride = GetRide(rideId);
            if (ride != nullptr)
            {
                auto strText = std::string(text);
                auto gameAction = GameActions::RideSetNameAction(ride->id, strText);
                GameActions::Execute(&gameAction, getGameState());
            }
        }

        void MainViewportRotate()
        {
            focus = std::nullopt;
            initViewport();
        }

        void MainOnPrepareDraw()
        {
            int32_t i, widgetHeight;

            SetPressedTab();

            auto ride = GetRide(rideId);
            if (ride == nullptr)
                return;

            const auto& gameState = getGameState();
            disabledWidgets &= ~((1uLL << WIDX_DEMOLISH) | (1uLL << WIDX_CONSTRUCTION));
            if (ride->lifecycleFlags & (RIDE_LIFECYCLE_INDESTRUCTIBLE | RIDE_LIFECYCLE_INDESTRUCTIBLE_TRACK)
                && !gameState.cheats.makeAllDestructible)
                disabledWidgets |= (1uLL << WIDX_DEMOLISH);

            auto ft = Formatter::Common();
            ride->formatNameTo(ft);

            uint32_t spriteIds[] = {
                SPR_CLOSED,
                SPR_OPEN,
                SPR_TESTING,
                SPR_G2_SIMULATE,
            };
            widgets[WIDX_OPEN].image = ImageId(spriteIds[EnumValue(ride->status)]);

#ifdef __SIMULATE_IN_RIDE_WINDOW__
            widgets[WIDX_CLOSE_LIGHT].image = SPR_G2_RCT1_CLOSE_BUTTON_0 + (ride->status == RideStatus::closed) * 2
                + widgetIsPressed(*this, WIDX_CLOSE_LIGHT);
            widgets[WIDX_SIMULATE_LIGHT].image = SPR_G2_RCT1_SIMULATE_BUTTON_0 + (ride->status == RideStatus::simulating) * 2
                + widgetIsPressed(*w, WIDX_SIMULATE_LIGHT);
            widgets[WIDX_TEST_LIGHT].image = SPR_G2_RCT1_TEST_BUTTON_0 + (ride->status == RideStatus::testing) * 2
                + widgetIsPressed(*this, WIDX_TEST_LIGHT);
#else
            const auto closeLightImage = SPR_G2_RCT1_CLOSE_BUTTON_0 + (ride->status == RideStatus::closed) * 2
                + widgetIsPressed(*this, WIDX_CLOSE_LIGHT);
            widgets[WIDX_CLOSE_LIGHT].image = ImageId(closeLightImage);

            auto baseSprite = ride->status == RideStatus::simulating ? SPR_G2_RCT1_SIMULATE_BUTTON_0
                                                                     : SPR_G2_RCT1_TEST_BUTTON_0;
            const auto testLightImage = baseSprite
                + (ride->status == RideStatus::testing || ride->status == RideStatus::simulating) * 2
                + widgetIsPressed(*this, WIDX_TEST_LIGHT);
            widgets[WIDX_TEST_LIGHT].image = ImageId(testLightImage);
#endif
            const auto openLightImage = SPR_G2_RCT1_OPEN_BUTTON_0 + (ride->status == RideStatus::open) * 2
                + widgetIsPressed(*this, WIDX_OPEN_LIGHT);
            widgets[WIDX_OPEN_LIGHT].image = ImageId(openLightImage);

            const int32_t offset = gameState.cheats.allowArbitraryRideTypeChanges ? 15 : 0;
            // Anchor main page specific widgets
            widgets[WIDX_VIEWPORT].right = width - 26;
            widgets[WIDX_VIEWPORT].bottom = height - (14 + offset);
            widgets[WIDX_STATUS].right = width - 26;
            widgets[WIDX_STATUS].top = height - (13 + offset);
            widgets[WIDX_STATUS].bottom = height - (3 + offset);
            widgets[WIDX_VIEW].right = width - 60;
            widgets[WIDX_VIEW_DROPDOWN].right = width - 61;
            widgets[WIDX_VIEW_DROPDOWN].left = width - 71;
            widgets[WIDX_RIDE_TYPE].right = width - 26;
            widgets[WIDX_RIDE_TYPE].top = height - 17;
            widgets[WIDX_RIDE_TYPE].bottom = height - 4;
            widgets[WIDX_RIDE_TYPE_DROPDOWN].left = width - 37;
            widgets[WIDX_RIDE_TYPE_DROPDOWN].right = width - 27;
            widgets[WIDX_RIDE_TYPE_DROPDOWN].top = height - 16;
            widgets[WIDX_RIDE_TYPE_DROPDOWN].bottom = height - 5;

            if (!gameState.cheats.allowArbitraryRideTypeChanges)
            {
                widgets[WIDX_RIDE_TYPE].type = WidgetType::empty;
                widgets[WIDX_RIDE_TYPE_DROPDOWN].type = WidgetType::empty;
            }
            else
            {
                widgets[WIDX_RIDE_TYPE].type = WidgetType::dropdownMenu;
                widgets[WIDX_RIDE_TYPE].text = ride->getRideTypeDescriptor().Naming.Name;
                widgets[WIDX_RIDE_TYPE_DROPDOWN].type = WidgetType::button;
            }

            WindowAlignTabs(this, WIDX_TAB_1, WIDX_TAB_10);

            if (ThemeGetFlags() & UITHEME_FLAG_USE_LIGHTS_RIDE)
            {
                widgets[WIDX_OPEN].type = WidgetType::empty;
                widgets[WIDX_CLOSE_LIGHT].type = WidgetType::imgBtn;
                widgets[WIDX_SIMULATE_LIGHT].type = WidgetType::empty;
#ifdef __SIMULATE_IN_RIDE_WINDOW__
                if (ride->supportsStatus(RideStatus::simulating))
                    widgets[WIDX_SIMULATE_LIGHT].type = WidgetType::imgBtn;
#endif
                widgets[WIDX_TEST_LIGHT].type = ride->supportsStatus(RideStatus::testing) ? WidgetType::imgBtn
                                                                                          : WidgetType::empty;
                widgets[WIDX_OPEN_LIGHT].type = WidgetType::imgBtn;

                widgetHeight = widgets[WIDX_PAGE_BACKGROUND].top + 19;
                if (widgets[WIDX_SIMULATE_LIGHT].type != WidgetType::empty)
                {
                    widgets[WIDX_SIMULATE_LIGHT].top = widgetHeight;
                    widgets[WIDX_SIMULATE_LIGHT].bottom = widgetHeight + 13;
                    widgetHeight += 14;
                }
                if (widgets[WIDX_TEST_LIGHT].type != WidgetType::empty)
                {
                    widgets[WIDX_TEST_LIGHT].top = widgetHeight;
                    widgets[WIDX_TEST_LIGHT].bottom = widgetHeight + 13;
                    widgetHeight += 14;
                }
                widgets[WIDX_OPEN_LIGHT].top = widgetHeight;
                widgets[WIDX_OPEN_LIGHT].bottom = widgetHeight + 13;
                widgetHeight += 14 - 24 + kRCT1LightOffset;
            }
            else
            {
                widgets[WIDX_OPEN].type = WidgetType::flatBtn;
                widgets[WIDX_CLOSE_LIGHT].type = WidgetType::empty;
                widgets[WIDX_SIMULATE_LIGHT].type = WidgetType::empty;
                widgets[WIDX_TEST_LIGHT].type = WidgetType::empty;
                widgets[WIDX_OPEN_LIGHT].type = WidgetType::empty;
                widgetHeight = widgets[WIDX_PAGE_BACKGROUND].top + 3;
            }

            for (i = WIDX_CLOSE_LIGHT; i <= WIDX_OPEN_LIGHT; i++)
            {
                widgets[i].left = width - 20;
                widgets[i].right = width - 7;
            }
            for (i = WIDX_OPEN; i <= WIDX_DEMOLISH; i++, widgetHeight += 24)
            {
                widgets[i].left = width - 25;
                widgets[i].right = width - 2;
                widgets[i].top = widgetHeight;
                widgets[i].bottom = widgetHeight + 23;
            }
        }

        StringId GetStatusOverallView(Formatter& ft) const
        {
            auto stringId = kStringIdNone;
            auto ride = GetRide(rideId);
            if (ride != nullptr)
            {
                ride->formatStatusTo(ft);
                stringId = STR_BLACK_STRING;
                if ((ride->lifecycleFlags & RIDE_LIFECYCLE_BROKEN_DOWN) || (ride->lifecycleFlags & RIDE_LIFECYCLE_CRASHED))
                {
                    stringId = STR_RED_OUTLINED_STRING;
                }
            }
            return stringId;
        }

        StringId GetStatusVehicle(Formatter& ft) const
        {
            auto ride = GetRide(rideId);
            if (ride == nullptr)
                return kStringIdEmpty;

            auto vehicle = getGameState().entities.GetEntity<Vehicle>(ride->vehicles[_viewIndex - 1]);
            if (vehicle == nullptr)
                return kStringIdEmpty;

            auto& rtd = ride->getRideTypeDescriptor();
            if (vehicle->status != Vehicle::Status::crashing && vehicle->status != Vehicle::Status::crashed)
            {
                auto trackType = vehicle->GetTrackType();
                if (trackType == TrackElemType::blockBrakes || trackType == TrackElemType::cableLiftHill
                    || trackType == TrackElemType::up25ToFlat || trackType == TrackElemType::up60ToFlat
                    || trackType == TrackElemType::diagUp25ToFlat || trackType == TrackElemType::diagUp60ToFlat
                    || trackType == TrackElemType::diagBlockBrakes)
                {
                    if (rtd.SupportsTrackGroup(TrackGroup::blockBrakes) && vehicle->velocity == 0)
                    {
                        ft.Add<StringId>(STR_STOPPED_BY_BLOCK_BRAKES);
                        return STR_BLACK_STRING;
                    }
                }
            }

            if (rtd.specialType == RtdSpecialType::miniGolf)
                return kStringIdEmpty;

            auto stringId = VehicleStatusNames[EnumValue(vehicle->status)];
            if (ride->getRideTypeDescriptor().HasFlag(RtdFlag::singleSession)
                && vehicle->status <= Vehicle::Status::unloadingPassengers)
            {
                stringId = SingleSessionVehicleStatusNames[EnumValue(vehicle->status)];
            }

            ft.Add<StringId>(stringId);
            uint16_t speedInMph = ToHumanReadableSpeed(abs(vehicle->velocity));
            ft.Add<uint16_t>(speedInMph);
            const RideComponentName stationName = GetRideComponentName(ride->getRideTypeDescriptor().NameConvention.station);
            ft.Add<StringId>(ride->numStations > 1 ? stationName.number : stationName.singular);
            ft.Add<uint16_t>(vehicle->current_station.ToUnderlying() + 1);

            if (stringId != STR_CRASHING && stringId != STR_CRASHED_0)
                return STR_BLACK_STRING;
            else
                return STR_RED_OUTLINED_STRING;
        }

        StringId GetStatusStation(Formatter& ft) const
        {
            auto ride = GetRide(rideId);
            if (ride == nullptr)
                return kStringIdNone;

            const auto stationIndex = GetStationIndexFromViewSelection();
            if (!stationIndex)
            {
                return kStringIdNone;
            }

            const auto& station = ride->getStation(*stationIndex);
            StringId stringId = kStringIdEmpty;
            // Entrance / exit
            if (ride->status == RideStatus::closed)
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
            if (stringId == kStringIdEmpty)
            {
                stringId = STR_QUEUE_EMPTY;
                uint16_t queueLength = ride->getStation(*stationIndex).QueueLength;
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

        StringId GetStatus(Formatter& ft) const
        {
            auto ride = GetRide(rideId);
            if (_viewIndex == 0)
                return GetStatusOverallView(ft);
            if (ride != nullptr && _viewIndex <= ride->numTrains)
                return GetStatusVehicle(ft);
            if (ride != nullptr && ride->lifecycleFlags & RIDE_LIFECYCLE_BROKEN_DOWN)
                return GetStatusOverallView(ft);
            return GetStatusStation(ft);
        }

        void MainOnDraw(RenderTarget& rt)
        {
            WindowDrawWidgets(*this, rt);
            drawTabImages(rt);

            // Viewport and ear icon
            if (viewport != nullptr)
            {
                WindowDrawViewport(rt, *this);
                if (viewport->flags & VIEWPORT_FLAG_SOUND_ON)
                    GfxDrawSprite(rt, ImageId(SPR_HEARING_VIEWPORT), WindowGetViewportSoundIconPos(*this));
            }

            // View dropdown
            auto ride = GetRide(rideId);
            if (ride == nullptr)
                return;

            auto ft = Formatter();
            if (_viewIndex != 0)
            {
                if (_viewIndex > ride->numTrains)
                {
                    ft.Add<StringId>(GetRideComponentName(ride->getRideTypeDescriptor().NameConvention.station).number);
                    ft.Add<uint16_t>(_viewIndex - ride->numTrains);
                }
                else
                {
                    ft.Add<StringId>(GetRideComponentName(ride->getRideTypeDescriptor().NameConvention.vehicle).number);
                    ft.Add<uint16_t>(_viewIndex);
                }
            }
            else
            {
                ft.Add<StringId>(STR_OVERALL_VIEW);
            }

            auto* widget = &widgets[WIDX_VIEW];
            DrawTextBasic(
                rt, { windowPos.x + (widget->left + widget->right - 11) / 2, windowPos.y + widget->top },
                STR_WINDOW_COLOUR_2_STRINGID, ft, { TextAlignment::centre });

            // Status
            ft = Formatter();
            widget = &widgets[WIDX_STATUS];
            StringId rideStatus = GetStatus(ft);
            DrawTextEllipsised(
                rt, windowPos + ScreenCoordsXY{ (widget->left + widget->right) / 2, widget->top }, widget->width() - 1,
                rideStatus, ft, { TextAlignment::centre });
        }

#pragma endregion

#pragma region Vehicle

        void VehicleOnMouseUp(WidgetIndex widgetIndex)
        {
            switch (widgetIndex)
            {
                case WIDX_CLOSE:
                    close();
                    return;
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
                    setPage(widgetIndex - WIDX_TAB_1);
                    break;
            }
        }

        void VehicleResize()
        {
            auto bottom = widgets[WIDX_VEHICLE_TRAINS].bottom + 6 - getTitleBarDiffNormal();
            WindowSetResize(*this, { kMinimumWindowWidth, bottom }, { kMinimumWindowWidth, bottom });
        }

        void VehicleOnMouseDown(WidgetIndex widgetIndex)
        {
            auto ride = GetRide(rideId);
            if (ride == nullptr)
                return;

            switch (widgetIndex)
            {
                case WIDX_VEHICLE_TYPE_DROPDOWN:
                    ShowVehicleTypeDropdown(&widgets[widgetIndex]);
                    break;
                case WIDX_VEHICLE_REVERSED_TRAINS_CHECKBOX:
                    ride->setReversedTrains(!ride->hasLifecycleFlag(RIDE_LIFECYCLE_REVERSED_TRAINS));
                    break;
                case WIDX_VEHICLE_TRAINS_INCREASE:
                    if (ride->numTrains < Limits::kMaxTrainsPerRide)
                        ride->setNumTrains(ride->numTrains + 1);
                    break;
                case WIDX_VEHICLE_TRAINS_DECREASE:
                    if (ride->numTrains > 1)
                        ride->setNumTrains(ride->numTrains - 1);
                    break;
                case WIDX_VEHICLE_CARS_PER_TRAIN_INCREASE:
                    if (ride->numCarsPerTrain < Limits::kMaxCarsPerTrain)
                        ride->setNumCarsPerTrain(ride->numCarsPerTrain + 1);
                    break;
                case WIDX_VEHICLE_CARS_PER_TRAIN_DECREASE:
                    if (ride->numCarsPerTrain > 1)
                        ride->setNumCarsPerTrain(ride->numCarsPerTrain - 1);
                    break;
            }
        }

        void VehicleOnDropdown(WidgetIndex widgetIndex, int32_t dropdownIndex)
        {
            if (dropdownIndex == -1)
                return;

            switch (widgetIndex)
            {
                case WIDX_VEHICLE_TYPE_DROPDOWN:
                    if (dropdownIndex >= 0 && static_cast<std::size_t>(dropdownIndex) < _vehicleDropdownData.size())
                    {
                        auto ride = GetRide(rideId);
                        if (ride != nullptr)
                        {
                            auto newRideType = _vehicleDropdownData[dropdownIndex].SubTypeId;
                            ride->setRideEntry(newRideType);
                        }
                    }
                    break;
            }
        }

        void VehicleUpdate()
        {
            currentFrame++;
            onPrepareDraw();
            invalidateWidget(WIDX_TAB_2);
        }

        OpenRCT2String VehicleTooltip(const WidgetIndex widgetIndex, StringId fallback)
        {
            auto ride = GetRide(rideId);
            if (ride == nullptr)
                return { kStringIdNone, {} };

            switch (widgetIndex)
            {
                case WIDX_VEHICLE_TRAINS:
                case WIDX_VEHICLE_TRAINS_DECREASE:
                case WIDX_VEHICLE_TRAINS_INCREASE:
                {
                    auto ft = Formatter();
                    ft.Increment(12);

                    RideComponentType vehicleType = ride->getRideTypeDescriptor().NameConvention.vehicle;
                    StringId stringId = GetRideComponentName(vehicleType).count;
                    if (ride->maxTrains > 1)
                    {
                        stringId = GetRideComponentName(vehicleType).count_plural;
                    }
                    ft.Add<StringId>(stringId);
                    ft.Add<uint16_t>(ride->maxTrains);
                    return { fallback, ft };
                }
                case WIDX_VEHICLE_CARS_PER_TRAIN:
                case WIDX_VEHICLE_CARS_PER_TRAIN_DECREASE:
                case WIDX_VEHICLE_CARS_PER_TRAIN_INCREASE:
                {
                    auto rideEntry = ride->getRideEntry();
                    if (rideEntry == nullptr)
                        return { kStringIdNone, {} };

                    auto ft = Formatter();
                    ft.Increment(16);
                    ft.Add<uint16_t>(std::max(uint8_t(1), ride->maxCarsPerTrain) - rideEntry->zero_cars);

                    StringId stringId = GetRideComponentName(RideComponentType::Car).singular;
                    if (ride->maxCarsPerTrain - rideEntry->zero_cars > 1)
                    {
                        stringId = GetRideComponentName(RideComponentType::Car).plural;
                    }
                    ft.Add<StringId>(stringId);
                    return { fallback, ft };
                }
            }
            return { fallback, {} };
        }

        void VehicleOnPrepareDraw()
        {
            StringId stringId;
            int32_t carsPerTrain;

            SetPressedTab();

            auto ride = GetRide(rideId);
            if (ride == nullptr)
                return;

            const auto* rideEntry = ride->getRideEntry();

            widgets[WIDX_TITLE].text = STR_ARG_20_STRINGID;

            // Widget setup
            carsPerTrain = ride->numCarsPerTrain - rideEntry->zero_cars;

            // Vehicle type
            widgets[WIDX_VEHICLE_TYPE].text = rideEntry->naming.Name;

            const auto& gameState = getGameState();
            // Trains
            if (rideEntry->cars_per_flat_ride > 1 || gameState.cheats.disableTrainLengthLimit)
            {
                widgets[WIDX_VEHICLE_TRAINS].type = WidgetType::spinner;
                widgets[WIDX_VEHICLE_TRAINS_INCREASE].type = WidgetType::button;
                widgets[WIDX_VEHICLE_TRAINS_DECREASE].type = WidgetType::button;
            }
            else
            {
                widgets[WIDX_VEHICLE_TRAINS].type = WidgetType::empty;
                widgets[WIDX_VEHICLE_TRAINS_INCREASE].type = WidgetType::empty;
                widgets[WIDX_VEHICLE_TRAINS_DECREASE].type = WidgetType::empty;
            }

            // Cars per train
            if (rideEntry->zero_cars + 1 < rideEntry->max_cars_in_train || gameState.cheats.disableTrainLengthLimit)
            {
                widgets[WIDX_VEHICLE_CARS_PER_TRAIN].type = WidgetType::spinner;
                widgets[WIDX_VEHICLE_CARS_PER_TRAIN_INCREASE].type = WidgetType::button;
                widgets[WIDX_VEHICLE_CARS_PER_TRAIN_DECREASE].type = WidgetType::button;
            }
            else
            {
                widgets[WIDX_VEHICLE_CARS_PER_TRAIN].type = WidgetType::empty;
                widgets[WIDX_VEHICLE_CARS_PER_TRAIN_INCREASE].type = WidgetType::empty;
                widgets[WIDX_VEHICLE_CARS_PER_TRAIN_DECREASE].type = WidgetType::empty;
            }

            if (ride->getRideTypeDescriptor().HasFlag(RtdFlag::allowReversedTrains)
                || (gameState.cheats.disableTrainLengthLimit && !ride->getRideTypeDescriptor().HasFlag(RtdFlag::isFlatRide)))
            {
                widgets[WIDX_VEHICLE_REVERSED_TRAINS_CHECKBOX].type = WidgetType::checkbox;
                if (ride->hasLifecycleFlag(RIDE_LIFECYCLE_REVERSED_TRAINS))
                {
                    pressedWidgets |= (1uLL << WIDX_VEHICLE_REVERSED_TRAINS_CHECKBOX);
                }
                else
                {
                    pressedWidgets &= ~(1uLL << WIDX_VEHICLE_REVERSED_TRAINS_CHECKBOX);
                }
            }
            else
            {
                widgets[WIDX_VEHICLE_REVERSED_TRAINS_CHECKBOX].type = WidgetType::empty;
            }

            auto ft = Formatter::Common();
            ft.Increment(6);
            ft.Add<uint16_t>(carsPerTrain);
            RideComponentType vehicleType = ride->getRideTypeDescriptor().NameConvention.vehicle;
            stringId = GetRideComponentName(vehicleType).count;
            if (ride->numTrains > 1)
            {
                stringId = GetRideComponentName(vehicleType).count_plural;
            }
            ft.Add<StringId>(stringId);
            ft.Add<uint16_t>(ride->numTrains);

            ft.Increment(8);

            ride->formatNameTo(ft);

            WindowAlignTabs(this, WIDX_TAB_1, WIDX_TAB_10);

            if (abs(ride->numCarsPerTrain - rideEntry->zero_cars) == 1)
            {
                widgets[WIDX_VEHICLE_CARS_PER_TRAIN].text = STR_1_CAR_PER_TRAIN;
            }
            else
            {
                widgets[WIDX_VEHICLE_CARS_PER_TRAIN].text = STR_X_CARS_PER_TRAIN;
            }
        }

        void VehicleOnDraw(RenderTarget& rt)
        {
            WindowDrawWidgets(*this, rt);
            drawTabImages(rt);

            auto ride = GetRide(rideId);
            if (ride == nullptr)
                return;

            auto rideEntry = ride->getRideEntry();
            if (rideEntry == nullptr)
                return;

            auto screenCoords = windowPos + ScreenCoordsXY{ 8, widgets[WIDX_VEHICLE_TYPE_DROPDOWN].bottom + 5 };
            // Description
            auto ft = Formatter();
            ft.Add<StringId>(rideEntry->naming.Description);
            screenCoords.y += DrawTextWrapped(rt, screenCoords, 300, STR_BLACK_STRING, ft, { TextAlignment::left });
            screenCoords.y += 2;

            // Capacity
            ft = Formatter();
            ft.Add<StringId>(rideEntry->capacity);
            DrawTextBasic(rt, screenCoords, STR_CAPACITY, ft);

            // Excitement Factor
            if (rideEntry->excitement_multiplier != 0)
            {
                screenCoords.y += kListRowHeight;

                ft = Formatter();
                ft.Add<int16_t>(abs(rideEntry->excitement_multiplier));
                StringId stringId = rideEntry->excitement_multiplier > 0 ? STR_EXCITEMENT_FACTOR
                                                                         : STR_EXCITEMENT_FACTOR_NEGATIVE;
                DrawTextBasic(rt, screenCoords, stringId, ft);
            }

            // Intensity Factor
            if (rideEntry->intensity_multiplier != 0)
            {
                int32_t lineHeight = FontGetLineHeight(FontStyle::medium);
                if (lineHeight != 10)
                    screenCoords.x += 150;
                else
                    screenCoords.y += kListRowHeight;

                ft = Formatter();
                ft.Add<int16_t>(abs(rideEntry->intensity_multiplier));
                StringId stringId = rideEntry->intensity_multiplier > 0 ? STR_INTENSITY_FACTOR : STR_INTENSITY_FACTOR_NEGATIVE;
                DrawTextBasic(rt, screenCoords, stringId, ft);

                if (lineHeight != 10)
                    screenCoords.x -= 150;
            }

            // Nausea Factor
            if (rideEntry->nausea_multiplier != 0)
            {
                screenCoords.y += kListRowHeight;

                ft = Formatter();
                ft.Add<int16_t>(abs(rideEntry->nausea_multiplier));
                StringId stringId = rideEntry->nausea_multiplier > 0 ? STR_NAUSEA_FACTOR : STR_NAUSEA_FACTOR_NEGATIVE;
                DrawTextBasic(rt, screenCoords, stringId, ft);
            }

            const auto minimumPreviewStart = screenCoords.y - windowPos.y + kListRowHeight + 5;
            if (minimumPreviewStart > widgets[WIDX_VEHICLE_TRAINS_PREVIEW].top)
            {
                auto heightIncrease = minimumPreviewStart - widgets[WIDX_VEHICLE_TRAINS_PREVIEW].top;
                height += heightIncrease;
                resizeFrame();

                for (auto i = EnumValue(WIDX_VEHICLE_TRAINS_PREVIEW); i <= WIDX_VEHICLE_CARS_PER_TRAIN_DECREASE; i++)
                {
                    widgets[i].moveDown(heightIncrease);
                }
            }
        }

        struct VehicleDrawInfo
        {
            int16_t x;
            int16_t y;
            ImageId imageId;
        };

        void VehicleOnScrollDraw(RenderTarget& rt, int32_t scrollIndex)
        {
            auto ride = GetRide(rideId);
            if (ride == nullptr)
                return;

            const auto* rideEntry = ride->getRideEntry();

            // Background
            Rectangle::fill(rt, { { rt.x, rt.y }, { rt.x + rt.width, rt.y + rt.height } }, PaletteIndex::pi12);

            Widget* widget = &widgets[WIDX_VEHICLE_TRAINS_PREVIEW];
            int32_t startX = std::max(2, (widget->width() - 1 - ((ride->numTrains - 1) * 36)) / 2 - 25);
            int32_t startY = widget->height() - 5;

            bool isReversed = ride->hasLifecycleFlag(RIDE_LIFECYCLE_REVERSED_TRAINS);
            int32_t carIndex = (isReversed) ? ride->numCarsPerTrain - 1 : 0;

            const auto& firstCarEntry = rideEntry->Cars[RideEntryGetVehicleAtPosition(
                ride->subtype, ride->numCarsPerTrain, carIndex)];
            startY += firstCarEntry.tab_height;

            // For each train
            for (int32_t i = 0; i < ride->numTrains; i++)
            {
                VehicleDrawInfo trainCarImages[Limits::kMaxCarsPerTrain];
                VehicleDrawInfo* nextSpriteToDraw = trainCarImages;
                int32_t x = startX;
                int32_t y = startY;

                // For each car in train
                static_assert(std::numeric_limits<decltype(ride->numCarsPerTrain)>::max() <= std::size(trainCarImages));
                for (int32_t j = 0; j < ride->numCarsPerTrain; j++)
                {
                    carIndex = (isReversed) ? (ride->numCarsPerTrain - 1) - j : j;

                    const auto& carEntry = rideEntry->Cars[RideEntryGetVehicleAtPosition(
                        ride->subtype, ride->numCarsPerTrain, carIndex)];
                    x += carEntry.spacing / 17432;
                    y -= (carEntry.spacing / 2) / 17432;

                    // Get colour of vehicle
                    int32_t vehicleColourIndex = 0;
                    switch (ride->vehicleColourSettings)
                    {
                        case VehicleColourSettings::same:
                            vehicleColourIndex = 0;
                            break;
                        case VehicleColourSettings::perTrain:
                            vehicleColourIndex = i;
                            break;
                        case VehicleColourSettings::perCar:
                            vehicleColourIndex = carIndex;
                            break;
                    }
                    VehicleColour vehicleColour = RideGetVehicleColour(*ride, vehicleColourIndex);

                    ImageIndex imageIndex = carEntry.SpriteByYaw(Entity::Yaw::kBaseRotation / 2, SpriteGroupType::SlopeFlat);
                    if (isReversed)
                    {
                        auto baseRotation = carEntry.NumRotationSprites(SpriteGroupType::SlopeFlat);
                        imageIndex = carEntry.SpriteByYaw(
                            (imageIndex + (baseRotation / 2)) & (baseRotation - 1), SpriteGroupType::SlopeFlat);
                    }

                    imageIndex &= carEntry.TabRotationMask;
                    imageIndex *= carEntry.base_num_frames;
                    imageIndex += carEntry.base_image_id;

                    auto imageId = ImageId(imageIndex, vehicleColour.Body, vehicleColour.Trim, vehicleColour.Tertiary);

                    nextSpriteToDraw->x = x;
                    nextSpriteToDraw->y = y;
                    nextSpriteToDraw->imageId = imageId;
                    nextSpriteToDraw++;

                    x += carEntry.spacing / 17432;
                    y -= (carEntry.spacing / 2) / 17432;
                }

                if (ride->getRideTypeDescriptor().HasFlag(RtdFlag::layeredVehiclePreview))
                {
                    VehicleDrawInfo tmp = *(nextSpriteToDraw - 1);
                    *(nextSpriteToDraw - 1) = *(nextSpriteToDraw - 2);
                    *(nextSpriteToDraw - 2) = tmp;
                }

                VehicleDrawInfo* current = nextSpriteToDraw;
                while (--current >= trainCarImages)
                    GfxDrawSprite(rt, current->imageId, { current->x, current->y });

                startX += 36;
            }
        }

#pragma endregion

#pragma region Operating

        void ModeTweakIncrease()
        {
            auto ride = GetRide(rideId);
            if (ride == nullptr)
                return;

            const auto& operatingSettings = ride->getRideTypeDescriptor().OperatingSettings;
            const auto& gameState = getGameState();
            uint8_t maxValue = operatingSettings.MaxValue;
            uint8_t minValue = gameState.cheats.unlockOperatingLimits ? 0 : operatingSettings.MinValue;

            if (gameState.cheats.unlockOperatingLimits)
            {
                maxValue = Limits::kCheatsMaxOperatingLimit;
            }

            uint8_t increment = ride->mode == RideMode::dodgems ? 10 : 1;

            SetOperatingSetting(
                rideId, GameActions::RideSetSetting::Operation,
                std::clamp<int16_t>(ride->operationOption + increment, minValue, maxValue));
        }

        void ModeTweakDecrease()
        {
            auto ride = GetRide(rideId);
            if (ride == nullptr)
                return;

            const auto& operatingSettings = ride->getRideTypeDescriptor().OperatingSettings;
            const auto& gameState = getGameState();
            uint8_t maxValue = operatingSettings.MaxValue;
            uint8_t minValue = gameState.cheats.unlockOperatingLimits ? 0 : operatingSettings.MinValue;
            if (gameState.cheats.unlockOperatingLimits)
            {
                maxValue = Limits::kCheatsMaxOperatingLimit;
            }

            uint8_t decrement = ride->mode == RideMode::dodgems ? 10 : 1;

            SetOperatingSetting(
                rideId, GameActions::RideSetSetting::Operation,
                std::clamp<int16_t>(ride->operationOption - decrement, minValue, maxValue));
        }

        void ModeDropdown(Widget* widget)
        {
            Widget* dropdownWidget;

            dropdownWidget = widget - 1;
            auto ride = GetRide(rideId);
            if (ride == nullptr)
                return;

            auto availableModes = ride->getAvailableModes();

            // Create dropdown list
            auto numAvailableModes = 0;
            auto checkedIndex = -1;
            for (auto i = 0; i < static_cast<uint8_t>(RideMode::count); i++)
            {
                if (availableModes & (1uLL << i))
                {
                    gDropdown.items[numAvailableModes] = Dropdown::MenuLabel(kRideModeNames[i]);

                    if (ride->mode == static_cast<RideMode>(i))
                        checkedIndex = numAvailableModes;

                    numAvailableModes++;
                }
            }

            WindowDropdownShowTextCustomWidth(
                { windowPos.x + dropdownWidget->left, windowPos.y + dropdownWidget->top }, dropdownWidget->height(), colours[1],
                0, Dropdown::Flag::StayOpen, numAvailableModes, widget->right - dropdownWidget->left);

            if (checkedIndex != -1)
            {
                gDropdown.items[checkedIndex].setChecked(true);
            }
        }

        void LoadDropdown(Widget* widget)
        {
            auto ride = GetRide(rideId);
            if (ride == nullptr)
                return;

            auto dropdownWidget = widget - 1;
            for (auto i = 0; i < 5; i++)
            {
                gDropdown.items[i] = Dropdown::MenuLabel(VehicleLoadNames[i]);
            }
            WindowDropdownShowTextCustomWidth(
                { windowPos.x + dropdownWidget->left, windowPos.y + dropdownWidget->top }, dropdownWidget->height(), colours[1],
                0, Dropdown::Flag::StayOpen, 5, widget->right - dropdownWidget->left);

            gDropdown.items[ride->departFlags & RIDE_DEPART_WAIT_FOR_LOAD_MASK].setChecked(true);
        }

        void OperatingOnMouseUp(WidgetIndex widgetIndex)
        {
            auto ride = GetRide(rideId);
            if (ride == nullptr)
                return;

            switch (widgetIndex)
            {
                case WIDX_CLOSE:
                    close();
                    return;
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
                    setPage(widgetIndex - WIDX_TAB_1);
                    break;
                case WIDX_LOAD_CHECKBOX:
                    SetOperatingSetting(
                        rideId, GameActions::RideSetSetting::Departure, ride->departFlags ^ RIDE_DEPART_WAIT_FOR_LOAD);
                    break;
                case WIDX_LEAVE_WHEN_ANOTHER_ARRIVES_CHECKBOX:
                    SetOperatingSetting(
                        rideId, GameActions::RideSetSetting::Departure,
                        ride->departFlags ^ RIDE_DEPART_LEAVE_WHEN_ANOTHER_ARRIVES);
                    break;
                case WIDX_MINIMUM_LENGTH_CHECKBOX:
                    SetOperatingSetting(
                        rideId, GameActions::RideSetSetting::Departure,
                        ride->departFlags ^ RIDE_DEPART_WAIT_FOR_MINIMUM_LENGTH);
                    break;
                case WIDX_MAXIMUM_LENGTH_CHECKBOX:
                    SetOperatingSetting(
                        rideId, GameActions::RideSetSetting::Departure,
                        ride->departFlags ^ RIDE_DEPART_WAIT_FOR_MAXIMUM_LENGTH);
                    break;
                case WIDX_SYNCHRONISE_WITH_ADJACENT_STATIONS_CHECKBOX:
                    SetOperatingSetting(
                        rideId, GameActions::RideSetSetting::Departure,
                        ride->departFlags ^ RIDE_DEPART_SYNCHRONISE_WITH_ADJACENT_STATIONS);
                    break;
            }
        }

        void OperatingResize()
        {
            auto bottom = widgets[WIDX_SYNCHRONISE_WITH_ADJACENT_STATIONS_CHECKBOX].bottom + 6 - getTitleBarDiffNormal();
            WindowSetResize(*this, { kMinimumWindowWidth, bottom }, { kMinimumWindowWidth, bottom });
        }

        void OperatingOnMouseDown(WidgetIndex widgetIndex)
        {
            auto ride = GetRide(rideId);
            if (ride == nullptr)
                return;

            uint8_t upperBound, lowerBound;
            switch (widgetIndex)
            {
                case WIDX_MODE_TWEAK:
                    OperatingTweakTextInput(*ride);
                    break;
                case WIDX_MODE_TWEAK_INCREASE:
                    ModeTweakIncrease();
                    break;
                case WIDX_MODE_TWEAK_DECREASE:
                    ModeTweakDecrease();
                    break;
                case WIDX_LIFT_HILL_SPEED_INCREASE:
                    upperBound = getGameState().cheats.unlockOperatingLimits
                        ? Limits::kCheatsMaxOperatingLimit
                        : ride->getRideTypeDescriptor().LiftData.maximum_speed;
                    lowerBound = getGameState().cheats.unlockOperatingLimits
                        ? 0
                        : ride->getRideTypeDescriptor().LiftData.minimum_speed;
                    SetOperatingSetting(
                        rideId, GameActions::RideSetSetting::LiftHillSpeed,
                        std::clamp<int16_t>(ride->liftHillSpeed + 1, lowerBound, upperBound));
                    break;
                case WIDX_LIFT_HILL_SPEED_DECREASE:
                    upperBound = getGameState().cheats.unlockOperatingLimits
                        ? Limits::kCheatsMaxOperatingLimit
                        : ride->getRideTypeDescriptor().LiftData.maximum_speed;
                    lowerBound = getGameState().cheats.unlockOperatingLimits
                        ? 0
                        : ride->getRideTypeDescriptor().LiftData.minimum_speed;
                    SetOperatingSetting(
                        rideId, GameActions::RideSetSetting::LiftHillSpeed,
                        std::clamp<int16_t>(ride->liftHillSpeed - 1, lowerBound, upperBound));
                    break;
                case WIDX_MINIMUM_LENGTH:
                    OperatingLengthWindow(WIDX_MINIMUM_LENGTH);
                    break;
                case WIDX_MAXIMUM_LENGTH:
                    OperatingLengthWindow(WIDX_MAXIMUM_LENGTH);
                    break;
                case WIDX_MINIMUM_LENGTH_INCREASE:
                    upperBound = Limits::kMaxWaitingTime;
                    lowerBound = 0;
                    SetOperatingSetting(
                        rideId, GameActions::RideSetSetting::MinWaitingTime,
                        std::clamp<int16_t>(ride->minWaitingTime + 1, lowerBound, upperBound));
                    break;
                case WIDX_MINIMUM_LENGTH_DECREASE:
                    upperBound = Limits::kMaxWaitingTime;
                    lowerBound = 0;
                    SetOperatingSetting(
                        rideId, GameActions::RideSetSetting::MinWaitingTime,
                        std::clamp<int16_t>(ride->minWaitingTime - 1, lowerBound, upperBound));
                    break;
                case WIDX_MAXIMUM_LENGTH_INCREASE:
                    upperBound = Limits::kMaxWaitingTime;
                    lowerBound = 0;
                    SetOperatingSetting(
                        rideId, GameActions::RideSetSetting::MaxWaitingTime,
                        std::clamp<int16_t>(ride->maxWaitingTime + 1, lowerBound, upperBound));
                    break;
                case WIDX_MAXIMUM_LENGTH_DECREASE:
                    upperBound = Limits::kMaxWaitingTime;
                    lowerBound = 0;
                    SetOperatingSetting(
                        rideId, GameActions::RideSetSetting::MaxWaitingTime,
                        std::clamp<int16_t>(ride->maxWaitingTime - 1, lowerBound, upperBound));
                    break;
                case WIDX_MODE_DROPDOWN:
                    ModeDropdown(&widgets[widgetIndex]);
                    break;
                case WIDX_LOAD_DROPDOWN:
                    LoadDropdown(&widgets[widgetIndex]);
                    break;
                case WIDX_OPERATE_NUMBER_OF_CIRCUITS_INCREASE:
                    upperBound = getGameState().cheats.unlockOperatingLimits ? Limits::kCheatsMaxOperatingLimit
                                                                             : Limits::kMaxCircuitsPerRide;
                    lowerBound = 1;
                    SetOperatingSetting(
                        rideId, GameActions::RideSetSetting::NumCircuits,
                        std::clamp<int16_t>(ride->numCircuits + 1, lowerBound, upperBound));
                    break;
                case WIDX_OPERATE_NUMBER_OF_CIRCUITS_DECREASE:
                    upperBound = getGameState().cheats.unlockOperatingLimits ? Limits::kCheatsMaxOperatingLimit
                                                                             : Limits::kMaxCircuitsPerRide;
                    lowerBound = 1;
                    SetOperatingSetting(
                        rideId, GameActions::RideSetSetting::NumCircuits,
                        std::clamp<int16_t>(ride->numCircuits - 1, lowerBound, upperBound));
                    break;
            }
        }

        void OperatingLengthWindow(WidgetIndex widgetIndex)
        {
            auto ride = GetRide(rideId);
            if (ride == nullptr)
                return;

            uint8_t upperBound = Limits::kMaxWaitingTime;
            uint8_t lowerBound = 0;
            Formatter ft;
            ft.Add<int16_t>(lowerBound);
            ft.Add<int16_t>(upperBound);
            auto title = (widgetIndex == WIDX_MINIMUM_LENGTH) ? STR_MINIMUM_WAITING_TIME : STR_MAXIMUM_WAITING_TIME;
            auto currentValue = (widgetIndex == WIDX_MINIMUM_LENGTH) ? ride->minWaitingTime : ride->maxWaitingTime;
            char buffer[5]{};
            snprintf(buffer, std::size(buffer), "%u", currentValue);
            WindowTextInputRawOpen(this, widgetIndex, title, STR_ENTER_VALUE, ft, buffer, 4);
        }

        void OperatingTweakTextInput(const Ride& ride)
        {
            switch (ride.mode)
            {
                case RideMode::poweredLaunchPasstrough:
                case RideMode::poweredLaunch:
                case RideMode::upwardLaunch:
                case RideMode::poweredLaunchBlockSectioned:
                case RideMode::stationToStation:
                case RideMode::dodgems:
                    return;
                default:
                    break;
            }

            const auto& operatingSettings = ride.getRideTypeDescriptor().OperatingSettings;
            const auto& gameState = getGameState();
            int16_t maxValue = gameState.cheats.unlockOperatingLimits ? Limits::kCheatsMaxOperatingLimit
                                                                      : operatingSettings.MaxValue;
            int16_t minValue = gameState.cheats.unlockOperatingLimits ? 0 : operatingSettings.MinValue;

            const auto& title = widgets[WIDX_MODE_TWEAK_LABEL].text;
            Formatter ft;
            ft.Add<int16_t>(minValue * operatingSettings.OperatingSettingMultiplier);
            ft.Add<int16_t>(maxValue * operatingSettings.OperatingSettingMultiplier);

            uint16_t currentValue = static_cast<uint16_t>(ride.operationOption) * operatingSettings.OperatingSettingMultiplier;
            char buffer[6]{};
            snprintf(buffer, std::size(buffer), "%u", currentValue);

            WindowTextInputRawOpen(this, WIDX_MODE_TWEAK, title, STR_ENTER_VALUE, ft, buffer, 4);
        }

        void OperatingOnDropdown(WidgetIndex widgetIndex, int32_t dropdownIndex)
        {
            if (dropdownIndex == -1)
                return;

            auto ride = GetRide(rideId);
            if (ride == nullptr)
                return;

            switch (widgetIndex)
            {
                case WIDX_MODE_DROPDOWN:
                {
                    RideMode rideMode = RideMode::nullMode;
                    auto availableModes = ride->getAvailableModes();
                    auto modeInDropdownIndex = -1;
                    for (RideMode rideModeIndex = RideMode::normal; rideModeIndex < RideMode::count; rideModeIndex++)
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
                    if (rideMode != RideMode::nullMode)
                        SetOperatingSetting(rideId, GameActions::RideSetSetting::Mode, static_cast<uint8_t>(rideMode));
                    break;
                }
                case WIDX_LOAD_DROPDOWN:
                    SetOperatingSetting(
                        rideId, GameActions::RideSetSetting::Departure,
                        (ride->departFlags & ~RIDE_DEPART_WAIT_FOR_LOAD_MASK) | dropdownIndex);
                    break;
            }
        }

        void OperatingUpdate()
        {
            currentFrame++;
            onPrepareDraw();
            invalidateWidget(WIDX_TAB_3);

            auto ride = GetRide(rideId);
            if (ride != nullptr && ride->windowInvalidateFlags.has(RideInvalidateFlag::operatingSettings))
            {
                ride->windowInvalidateFlags.unset(RideInvalidateFlag::operatingSettings);
                invalidate();
            }
        }

        void OperatingOnTextInput(WidgetIndex widgetIndex, std::string_view text)
        {
            if (text.empty())
                return;

            auto ride = GetRide(rideId);
            if (ride == nullptr)
                return;

            if (widgetIndex == WIDX_MODE_TWEAK)
            {
                const auto& operatingSettings = ride->getRideTypeDescriptor().OperatingSettings;
                const auto& gameState = getGameState();
                uint32_t maxValue = gameState.cheats.unlockOperatingLimits ? Limits::kCheatsMaxOperatingLimit
                                                                           : operatingSettings.MaxValue;
                uint32_t minValue = gameState.cheats.unlockOperatingLimits ? 0 : operatingSettings.MinValue;
                auto multiplier = ride->getRideTypeDescriptor().OperatingSettings.OperatingSettingMultiplier;

                try
                {
                    uint32_t origSize = std::stol(std::string(text)) / multiplier;
                    uint8_t size = static_cast<uint8_t>(std::clamp(origSize, minValue, maxValue));
                    SetOperatingSetting(ride->id, GameActions::RideSetSetting::Operation, size);
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
                    auto rideSetSetting = widgetIndex == WIDX_MINIMUM_LENGTH ? GameActions::RideSetSetting::MinWaitingTime
                                                                             : GameActions::RideSetSetting::MaxWaitingTime;

                    uint16_t upperBound = Limits::kMaxWaitingTime;
                    uint16_t lowerBound = 0;
                    uint16_t size = std::stol(std::string(text));
                    size = std::clamp(size, lowerBound, upperBound);
                    SetOperatingSetting(ride->id, rideSetSetting, size);
                }
                catch (const std::logic_error&)
                {
                    // std::stol can throw std::out_of_range or std::invalid_argument
                }
            }
        }

        void OperatingOnPrepareDraw()
        {
            StringId format, caption, tooltip;

            SetPressedTab();

            auto ride = GetRide(rideId);
            if (ride == nullptr)
                return;

            auto ft = Formatter::Common();
            ride->formatNameTo(ft);

            // Widget setup
            pressedWidgets &= ~(
                (1uLL << WIDX_LOAD_CHECKBOX) | (1uLL << WIDX_LEAVE_WHEN_ANOTHER_ARRIVES_CHECKBOX)
                | (1uLL << WIDX_MINIMUM_LENGTH_CHECKBOX) | (1uLL << WIDX_MAXIMUM_LENGTH_CHECKBOX)
                | (1uLL << WIDX_SYNCHRONISE_WITH_ADJACENT_STATIONS_CHECKBOX));

            // Sometimes, only one of the alternatives support lift hill pieces. Make sure to check both.
            const auto& rtd = ride->getRideTypeDescriptor();
            bool hasAlternativeType = rtd.HasFlag(RtdFlag::hasInvertedVariant);
            if (rtd.TrackPaintFunctions.Regular.SupportsTrackGroup(TrackGroup::liftHill)
                || (hasAlternativeType && rtd.InvertedTrackPaintFunctions.Regular.SupportsTrackGroup(TrackGroup::liftHill)))
            {
                widgets[WIDX_LIFT_HILL_SPEED_LABEL].type = WidgetType::label;
                widgets[WIDX_LIFT_HILL_SPEED].type = WidgetType::spinner;
                widgets[WIDX_LIFT_HILL_SPEED_INCREASE].type = WidgetType::button;
                widgets[WIDX_LIFT_HILL_SPEED_DECREASE].type = WidgetType::button;
                ft.Rewind();
                ft.Increment(20);
                ft.Add<uint16_t>(ride->liftHillSpeed);
            }
            else
            {
                widgets[WIDX_LIFT_HILL_SPEED_LABEL].type = WidgetType::empty;
                widgets[WIDX_LIFT_HILL_SPEED].type = WidgetType::empty;
                widgets[WIDX_LIFT_HILL_SPEED_INCREASE].type = WidgetType::empty;
                widgets[WIDX_LIFT_HILL_SPEED_DECREASE].type = WidgetType::empty;
            }

            // Number of circuits
            if (ride->canHaveMultipleCircuits())
            {
                widgets[WIDX_OPERATE_NUMBER_OF_CIRCUITS_LABEL].type = WidgetType::label;
                widgets[WIDX_OPERATE_NUMBER_OF_CIRCUITS].type = WidgetType::spinner;
                widgets[WIDX_OPERATE_NUMBER_OF_CIRCUITS_INCREASE].type = WidgetType::button;
                widgets[WIDX_OPERATE_NUMBER_OF_CIRCUITS_DECREASE].type = WidgetType::button;
                ft.Rewind();
                ft.Increment(22);
                ft.Add<uint16_t>(ride->numCircuits);
            }
            else
            {
                widgets[WIDX_OPERATE_NUMBER_OF_CIRCUITS_LABEL].type = WidgetType::empty;
                widgets[WIDX_OPERATE_NUMBER_OF_CIRCUITS].type = WidgetType::empty;
                widgets[WIDX_OPERATE_NUMBER_OF_CIRCUITS_INCREASE].type = WidgetType::empty;
                widgets[WIDX_OPERATE_NUMBER_OF_CIRCUITS_DECREASE].type = WidgetType::empty;
            }

            // Leave if another vehicle arrives at station
            if (ride->getRideTypeDescriptor().HasFlag(RtdFlag::hasLeaveWhenAnotherVehicleArrivesAtStation)
                && ride->numTrains > 1 && !ride->isBlockSectioned())
            {
                widgets[WIDX_LEAVE_WHEN_ANOTHER_ARRIVES_CHECKBOX].type = WidgetType::checkbox;
                widgets[WIDX_LEAVE_WHEN_ANOTHER_ARRIVES_CHECKBOX].tooltip = STR_LEAVE_IF_ANOTHER_VEHICLE_ARRIVES_TIP;
                widgets[WIDX_LEAVE_WHEN_ANOTHER_ARRIVES_CHECKBOX].text = ride->getRideTypeDescriptor().NameConvention.vehicle
                        == RideComponentType::Boat
                    ? STR_LEAVE_IF_ANOTHER_BOAT_ARRIVES
                    : STR_LEAVE_IF_ANOTHER_TRAIN_ARRIVES;
            }
            else
            {
                widgets[WIDX_LEAVE_WHEN_ANOTHER_ARRIVES_CHECKBOX].type = WidgetType::empty;
            }

            // Synchronise with adjacent stations
            if (ride->getRideTypeDescriptor().HasFlag(RtdFlag::canSynchroniseWithAdjacentStations))
            {
                widgets[WIDX_SYNCHRONISE_WITH_ADJACENT_STATIONS_CHECKBOX].type = WidgetType::checkbox;
                widgets[WIDX_SYNCHRONISE_WITH_ADJACENT_STATIONS_CHECKBOX].text = STR_SYNCHRONISE_WITH_ADJACENT_STATIONS;
                widgets[WIDX_SYNCHRONISE_WITH_ADJACENT_STATIONS_CHECKBOX].tooltip = STR_SYNCHRONISE_WITH_ADJACENT_STATIONS_TIP;
            }
            else
            {
                widgets[WIDX_SYNCHRONISE_WITH_ADJACENT_STATIONS_CHECKBOX].type = WidgetType::empty;
            }

            // Mode
            widgets[WIDX_MODE].text = kRideModeNames[EnumValue(ride->mode)];

            // Waiting
            widgets[WIDX_LOAD].text = VehicleLoadNames[(ride->departFlags & RIDE_DEPART_WAIT_FOR_LOAD_MASK)];
            if (ride->getRideTypeDescriptor().HasFlag(RtdFlag::hasLoadOptions))
            {
                widgets[WIDX_LOAD_CHECKBOX].type = WidgetType::checkbox;
                widgets[WIDX_LOAD].type = WidgetType::dropdownMenu;
                widgets[WIDX_LOAD_DROPDOWN].type = WidgetType::button;

                widgets[WIDX_MINIMUM_LENGTH_CHECKBOX].type = WidgetType::checkbox;
                widgets[WIDX_MINIMUM_LENGTH].type = WidgetType::spinner;
                widgets[WIDX_MINIMUM_LENGTH_INCREASE].type = WidgetType::button;
                widgets[WIDX_MINIMUM_LENGTH_DECREASE].type = WidgetType::button;

                widgets[WIDX_MAXIMUM_LENGTH_CHECKBOX].type = WidgetType::checkbox;
                widgets[WIDX_MAXIMUM_LENGTH].type = WidgetType::spinner;
                widgets[WIDX_MAXIMUM_LENGTH_INCREASE].type = WidgetType::button;
                widgets[WIDX_MAXIMUM_LENGTH_DECREASE].type = WidgetType::button;

                ft.Rewind();
                ft.Increment(10);
                ft.Add<StringId>(STR_FORMAT_SECONDS);
                ft.Add<uint16_t>(ride->minWaitingTime);
                ft.Add<StringId>(STR_FORMAT_SECONDS);
                ft.Add<uint16_t>(ride->maxWaitingTime);

                if (ride->departFlags & RIDE_DEPART_WAIT_FOR_LOAD)
                    pressedWidgets |= (1uLL << WIDX_LOAD_CHECKBOX);
            }
            else
            {
                widgets[WIDX_LOAD_CHECKBOX].type = WidgetType::empty;
                widgets[WIDX_LOAD].type = WidgetType::empty;
                widgets[WIDX_LOAD_DROPDOWN].type = WidgetType::empty;

                widgets[WIDX_MINIMUM_LENGTH_CHECKBOX].type = WidgetType::empty;
                widgets[WIDX_MINIMUM_LENGTH].type = WidgetType::empty;
                widgets[WIDX_MINIMUM_LENGTH_INCREASE].type = WidgetType::empty;
                widgets[WIDX_MINIMUM_LENGTH_DECREASE].type = WidgetType::empty;

                widgets[WIDX_MAXIMUM_LENGTH_CHECKBOX].type = WidgetType::empty;
                widgets[WIDX_MAXIMUM_LENGTH].type = WidgetType::empty;
                widgets[WIDX_MAXIMUM_LENGTH_INCREASE].type = WidgetType::empty;
                widgets[WIDX_MAXIMUM_LENGTH_DECREASE].type = WidgetType::empty;
            }

            if (ride->departFlags & RIDE_DEPART_LEAVE_WHEN_ANOTHER_ARRIVES)
                pressedWidgets |= (1uLL << WIDX_LEAVE_WHEN_ANOTHER_ARRIVES_CHECKBOX);
            if (ride->departFlags & RIDE_DEPART_SYNCHRONISE_WITH_ADJACENT_STATIONS)
                pressedWidgets |= (1uLL << WIDX_SYNCHRONISE_WITH_ADJACENT_STATIONS_CHECKBOX);
            if (ride->departFlags & RIDE_DEPART_WAIT_FOR_MINIMUM_LENGTH)
                pressedWidgets |= (1uLL << WIDX_MINIMUM_LENGTH_CHECKBOX);
            if (ride->departFlags & RIDE_DEPART_WAIT_FOR_MAXIMUM_LENGTH)
                pressedWidgets |= (1uLL << WIDX_MAXIMUM_LENGTH_CHECKBOX);

            // Mode specific functionality
            auto multiplier = ride->getRideTypeDescriptor().OperatingSettings.OperatingSettingMultiplier;
            ft.Rewind();
            ft.Increment(18);
            ft.Add<uint16_t>(static_cast<uint16_t>(ride->operationOption) * multiplier);
            switch (ride->mode)
            {
                case RideMode::poweredLaunchPasstrough:
                case RideMode::poweredLaunch:
                case RideMode::upwardLaunch:
                case RideMode::poweredLaunchBlockSectioned:
                    ft.Rewind();
                    ft.Increment(18);
                    ft.Add<uint16_t>((ride->launchSpeed * 9) / 4);
                    format = STR_RIDE_MODE_SPEED_VALUE;
                    caption = STR_LAUNCH_SPEED;
                    tooltip = STR_LAUNCH_SPEED_TIP;
                    break;
                case RideMode::stationToStation:
                    ft.Rewind();
                    ft.Increment(18);
                    ft.Add<uint16_t>((ride->speed * 9) / 4);
                    format = STR_RIDE_MODE_SPEED_VALUE;
                    caption = STR_SPEED;
                    tooltip = STR_SPEED_TIP;
                    break;
                case RideMode::race:
                    ft.Rewind();
                    ft.Increment(18);
                    ft.Add<uint16_t>(ride->numLaps);
                    format = STR_NUMBER_OF_LAPS_VALUE;
                    caption = STR_NUMBER_OF_LAPS;
                    tooltip = STR_NUMBER_OF_LAPS_TIP;
                    break;
                case RideMode::dodgems:
                    format = STR_RIDE_MODE_TIME_LIMIT_VALUE;
                    caption = STR_TIME_LIMIT;
                    tooltip = STR_TIME_LIMIT_TIP;
                    break;
                case RideMode::swing:
                    format = STR_RIDE_MODE_NUMBER_OF_SWINGS_VALUE;
                    caption = STR_NUMBER_OF_SWINGS;
                    tooltip = STR_NUMBER_OF_SWINGS_TIP;
                    break;
                case RideMode::rotation:
                case RideMode::forwardRotation:
                case RideMode::backwardRotation:
                    format = STR_NUMBER_OF_ROTATIONS_VALUE;
                    caption = STR_NUMBER_OF_ROTATIONS;
                    tooltip = STR_NUMBER_OF_ROTATIONS_TIP;
                    break;
                default:
                    format = STR_MAX_PEOPLE_ON_RIDE_VALUE;
                    caption = STR_MAX_PEOPLE_ON_RIDE;
                    tooltip = STR_MAX_PEOPLE_ON_RIDE_TIP;
                    if (!ride->getRideTypeDescriptor().HasFlag(RtdFlag::noVehicles))
                        format = 0;
                    break;
            }

            if (format != 0)
            {
                widgets[WIDX_MODE_TWEAK_LABEL].type = WidgetType::label;
                widgets[WIDX_MODE_TWEAK_LABEL].text = caption;
                widgets[WIDX_MODE_TWEAK_LABEL].tooltip = tooltip;
                widgets[WIDX_MODE_TWEAK].type = WidgetType::spinner;
                widgets[WIDX_MODE_TWEAK].text = format;
                widgets[WIDX_MODE_TWEAK_INCREASE].type = WidgetType::button;
                widgets[WIDX_MODE_TWEAK_DECREASE].type = WidgetType::button;
                pressedWidgets &= ~(1uLL << WIDX_LEAVE_WHEN_ANOTHER_ARRIVES_CHECKBOX);
            }
            else
            {
                widgets[WIDX_MODE_TWEAK_LABEL].type = WidgetType::empty;
                widgets[WIDX_MODE_TWEAK].type = WidgetType::empty;
                widgets[WIDX_MODE_TWEAK_INCREASE].type = WidgetType::empty;
                widgets[WIDX_MODE_TWEAK_DECREASE].type = WidgetType::empty;
            }

            WindowAlignTabs(this, WIDX_TAB_1, WIDX_TAB_10);
        }

        void OperatingOnDraw(RenderTarget& rt)
        {
            drawWidgets(rt);
            drawTabImages(rt);

            auto ride = GetRide(rideId);
            if (ride == nullptr)
                return;

            // Horizontal rule between mode settings and depart settings
            auto ruleStart = widgets[WIDX_LOAD_DROPDOWN].top - 8;
            Rectangle::fillInset(
                rt,
                { windowPos + ScreenCoordsXY{ widgets[WIDX_PAGE_BACKGROUND].left + 4, ruleStart },
                  windowPos + ScreenCoordsXY{ widgets[WIDX_PAGE_BACKGROUND].right - 5, ruleStart + 1 } },
                colours[1], Rectangle::BorderStyle::inset);

            // Number of block sections
            if (ride->isBlockSectioned())
            {
                auto ft = Formatter();
                ft.Add<uint16_t>(ride->numBlockBrakes + ride->numStations);
                auto underWidget = ride->mode == RideMode::poweredLaunchBlockSectioned ? WIDX_MODE_TWEAK : WIDX_MODE;
                DrawTextBasic(
                    rt, windowPos + ScreenCoordsXY{ 21, widgets[underWidget].bottom + 3 }, STR_BLOCK_SECTIONS, ft,
                    { Drawing::Colour::black });
            }
        }

#pragma endregion

#pragma region Maintenance

        void LocateMechanic()
        {
            auto ride = GetRide(rideId);
            if (ride == nullptr)
                return;

            // First check if there is a mechanic assigned
            Peep* mechanic = RideGetAssignedMechanic(*ride);

            // Otherwise find the closest mechanic
            if (mechanic == nullptr)
                mechanic = RideFindClosestMechanic(*ride, 1);

            if (mechanic == nullptr)
                ContextShowError(STR_UNABLE_TO_LOCATE_MECHANIC, kStringIdNone, {});
            else
            {
                auto intent = Intent(WindowClass::peep);
                intent.PutExtra(INTENT_EXTRA_PEEP, mechanic);
                ContextOpenIntent(&intent);
            }
        }

        void MaintenanceOnMouseUp(WidgetIndex widgetIndex)
        {
            switch (widgetIndex)
            {
                case WIDX_CLOSE:
                    close();
                    return;
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
                    setPage(widgetIndex - WIDX_TAB_1);
                    break;
                case WIDX_LOCATE_MECHANIC:
                    LocateMechanic();
                    break;
                case WIDX_REFURBISH_RIDE:
                    ContextOpenDetailWindow(WindowDetail::refurbishRide, number);
                    break;
            }
        }

        void MaintenanceResize()
        {
            auto bottom = widgets[WIDX_LOCATE_MECHANIC].bottom + 6 - getTitleBarDiffNormal();
            WindowSetResize(*this, { kMinimumWindowWidth, bottom }, { kMinimumWindowWidth, bottom });
        }

        void MaintenanceOnMouseDown(WidgetIndex widgetIndex)
        {
            auto ride = GetRide(rideId);
            if (ride == nullptr)
                return;

            auto rideEntry = ride->getRideEntry();
            if (rideEntry == nullptr)
                return;

            Widget* dropdownWidget = &widgets[widgetIndex];
            int32_t j, numItems;

            switch (widgetIndex)
            {
                case WIDX_INSPECTION_INTERVAL_DROPDOWN:
                    dropdownWidget--;
                    for (int32_t i = 0; i < 7; i++)
                    {
                        gDropdown.items[i] = Dropdown::MenuLabel(kRideInspectionIntervalNames[i]);
                    }
                    WindowDropdownShowTextCustomWidth(
                        { windowPos.x + dropdownWidget->left, windowPos.y + dropdownWidget->top }, dropdownWidget->height(),
                        colours[1], 0, Dropdown::Flag::StayOpen, 7, widgets[widgetIndex].right - dropdownWidget->left);

                    gDropdown.items[EnumValue(ride->inspectionInterval)].setChecked(true);
                    break;

                case WIDX_FORCE_BREAKDOWN:
                    numItems = 1;
                    for (j = 0; j < RCT2::ObjectLimits::kMaxRideTypesPerRideEntry; j++)
                    {
                        if (rideEntry->ride_type[j] != kRideTypeNull)
                            break;
                    }
                    gDropdown.items[0] = Dropdown::MenuLabel(STR_DEBUG_FIX_RIDE);
                    for (int32_t i = 0; i < 8; i++)
                    {
                        assert(j < static_cast<int32_t>(std::size(rideEntry->ride_type)));
                        if (GetRideTypeDescriptor(rideEntry->ride_type[j]).AvailableBreakdowns & static_cast<uint8_t>(1 << i))
                        {
                            if (i == BREAKDOWN_BRAKES_FAILURE && ride->isBlockSectioned())
                            {
                                if (ride->numTrains != 1)
                                    continue;
                            }
                            gDropdown.items[numItems] = Dropdown::MenuLabel(RideBreakdownReasonNames[i]);
                            numItems++;
                        }
                    }
                    if (numItems == 1)
                    {
                        ContextShowError(STR_DEBUG_NO_BREAKDOWNS_AVAILABLE, kStringIdNone, {});
                    }
                    else
                    {
                        WindowDropdownShowText(
                            { windowPos.x + dropdownWidget->left, windowPos.y + dropdownWidget->top }, dropdownWidget->height(),
                            colours[1], Dropdown::Flag::StayOpen, numItems);

                        numItems = 1;
                        int32_t breakdownReason = ride->breakdownReasonPending;
                        if (breakdownReason != BREAKDOWN_NONE && (ride->lifecycleFlags & RIDE_LIFECYCLE_BREAKDOWN_PENDING))
                        {
                            for (int32_t i = 0; i < 8; i++)
                            {
                                if (GetRideTypeDescriptor(rideEntry->ride_type[j]).AvailableBreakdowns
                                    & static_cast<uint8_t>(1 << i))
                                {
                                    if (i == BREAKDOWN_BRAKES_FAILURE && ride->isBlockSectioned())
                                    {
                                        if (ride->numTrains != 1)
                                            continue;
                                    }
                                    if (i == breakdownReason)
                                    {
                                        gDropdown.items[numItems].setChecked(true);
                                        break;
                                    }
                                    gDropdown.items[numItems] = Dropdown::MenuLabel(RideBreakdownReasonNames[i]);
                                    numItems++;
                                }
                            }
                        }

                        if ((ride->lifecycleFlags & RIDE_LIFECYCLE_BREAKDOWN_PENDING) == 0)
                        {
                            gDropdown.items[0].setDisabled(true);
                        }
                    }
                    break;
            }
        }

        void MaintenanceOnDropdown(WidgetIndex widgetIndex, int32_t dropdownIndex)
        {
            if (dropdownIndex == -1)
                return;

            auto ride = GetRide(rideId);
            if (ride == nullptr)
                return;

            auto rideEntry = ride->getRideEntry();
            if (rideEntry == nullptr)
                return;

            switch (widgetIndex)
            {
                case WIDX_INSPECTION_INTERVAL_DROPDOWN:
                    SetOperatingSetting(rideId, GameActions::RideSetSetting::InspectionInterval, dropdownIndex);
                    break;

                case WIDX_FORCE_BREAKDOWN:
                    if (dropdownIndex == 0)
                    {
                        Vehicle* vehicle;
                        switch (ride->breakdownReasonPending)
                        {
                            case BREAKDOWN_SAFETY_CUT_OUT:
                                if (!(ride->lifecycleFlags & RIDE_LIFECYCLE_ON_TRACK))
                                    break;
                                for (int32_t i = 0; i < ride->numTrains; ++i)
                                {
                                    for (vehicle = getGameState().entities.GetEntity<Vehicle>(ride->vehicles[i]);
                                         vehicle != nullptr;
                                         vehicle = getGameState().entities.GetEntity<Vehicle>(vehicle->next_vehicle_on_train))
                                    {
                                        vehicle->ClearFlag(
                                            VehicleFlags::CarIsBroken | VehicleFlags::StoppedBySafetyCutOut
                                            | VehicleFlags::TrainIsBroken);
                                    }
                                }
                                break;
                            case BREAKDOWN_RESTRAINTS_STUCK_CLOSED:
                            case BREAKDOWN_RESTRAINTS_STUCK_OPEN:
                            case BREAKDOWN_DOORS_STUCK_CLOSED:
                            case BREAKDOWN_DOORS_STUCK_OPEN:
                                vehicle = getGameState().entities.GetEntity<Vehicle>(ride->vehicles[ride->brokenTrain]);
                                if (vehicle != nullptr)
                                {
                                    vehicle->ClearFlag(VehicleFlags::CarIsBroken);
                                }
                                break;
                            case BREAKDOWN_VEHICLE_MALFUNCTION:
                                vehicle = getGameState().entities.GetEntity<Vehicle>(ride->vehicles[ride->brokenTrain]);
                                if (vehicle != nullptr)
                                {
                                    vehicle->ClearFlag(VehicleFlags::TrainIsBroken);
                                }
                                break;
                        }
                        ride->lifecycleFlags &= ~(RIDE_LIFECYCLE_BREAKDOWN_PENDING | RIDE_LIFECYCLE_BROKEN_DOWN);

                        auto* windowMgr = GetWindowManager();
                        windowMgr->InvalidateByNumber(WindowClass::ride, number);
                        break;
                    }
                    if (ride->lifecycleFlags
                        & (RIDE_LIFECYCLE_BREAKDOWN_PENDING | RIDE_LIFECYCLE_BROKEN_DOWN | RIDE_LIFECYCLE_CRASHED))
                    {
                        ContextShowError(STR_DEBUG_CANT_FORCE_BREAKDOWN, STR_DEBUG_RIDE_ALREADY_BROKEN, {});
                    }
                    else if (ride->status == RideStatus::closed)
                    {
                        ContextShowError(STR_DEBUG_CANT_FORCE_BREAKDOWN, STR_DEBUG_RIDE_IS_CLOSED, {});
                    }
                    else
                    {
                        int32_t j;
                        for (j = 0; j < RCT2::ObjectLimits::kMaxRideTypesPerRideEntry; j++)
                        {
                            if (rideEntry->ride_type[j] != kRideTypeNull)
                                break;
                        }
                        int32_t i;
                        int32_t numItems = 1;
                        for (i = 0; i < BREAKDOWN_COUNT; i++)
                        {
                            assert(j < static_cast<int32_t>(std::size(rideEntry->ride_type)));
                            if (GetRideTypeDescriptor(rideEntry->ride_type[j]).AvailableBreakdowns
                                & static_cast<uint8_t>(1 << i))
                            {
                                if (i == BREAKDOWN_BRAKES_FAILURE && ride->isBlockSectioned())
                                {
                                    if (ride->numTrains != 1)
                                        continue;
                                }
                                if (numItems == dropdownIndex)
                                    break;
                                numItems++;
                            }
                        }
                        RidePrepareBreakdown(*ride, i);
                    }
                    break;
            }
        }

        void MaintenanceUpdate()
        {
            currentFrame++;
            onPrepareDraw();
            invalidateWidget(WIDX_TAB_4);

            auto ride = GetRide(rideId);
            if (ride != nullptr && ride->windowInvalidateFlags.has(RideInvalidateFlag::maintenance))
            {
                ride->windowInvalidateFlags.unset(RideInvalidateFlag::maintenance);
                invalidate();
            }
        }

        void MaintenanceOnPrepareDraw()
        {
            SetPressedTab();

            auto ride = GetRide(rideId);
            if (ride == nullptr)
                return;

            auto ft = Formatter::Common();
            ride->formatNameTo(ft);

            widgets[WIDX_INSPECTION_INTERVAL].text = kRideInspectionIntervalNames[EnumValue(ride->inspectionInterval)];

            WindowAlignTabs(this, WIDX_TAB_1, WIDX_TAB_10);

            if (Config::Get().general.debuggingTools && Network::GetMode() == Network::Mode::none)
            {
                widgets[WIDX_FORCE_BREAKDOWN].type = WidgetType::flatBtn;
            }
            else
            {
                widgets[WIDX_FORCE_BREAKDOWN].type = WidgetType::empty;
            }

            if (ride->getRideTypeDescriptor().AvailableBreakdowns == 0
                || !(ride->lifecycleFlags & RIDE_LIFECYCLE_EVER_BEEN_OPENED))
            {
                disabledWidgets |= (1uLL << WIDX_REFURBISH_RIDE);
                widgets[WIDX_REFURBISH_RIDE].tooltip = STR_CANT_REFURBISH_NOT_NEEDED;
            }
            else
            {
                disabledWidgets &= ~(1uLL << WIDX_REFURBISH_RIDE);
                widgets[WIDX_REFURBISH_RIDE].tooltip = STR_REFURBISH_RIDE_TIP;
            }
        }

        void MaintenanceOnDraw(RenderTarget& rt)
        {
            auto ride = GetRide(rideId);
            if (ride == nullptr)
                return;

            uint16_t reliability = ride->reliabilityPercentage;
            widgetProgressBarSetNewPercentage(widgets[WIDX_RELIABILITY_BAR], reliability);

            uint16_t downTime = ride->downtime;
            widgetProgressBarSetNewPercentage(widgets[WIDX_DOWN_TIME_BAR], downTime);

            drawWidgets(rt);
            drawTabImages(rt);

            // Locate mechanic button image
            Widget* widget = &widgets[WIDX_LOCATE_MECHANIC];
            auto screenCoords = windowPos + ScreenCoordsXY{ widget->left, widget->top };
            auto image = ImageId(SPR_MECHANIC, Drawing::Colour::black, getGameState().park.staffMechanicColour);
            GfxDrawSprite(rt, image, screenCoords);

            // Inspection label
            widget = &widgets[WIDX_INSPECTION_INTERVAL];
            screenCoords = windowPos + ScreenCoordsXY{ 4, widget->top + 1 };
            DrawTextBasic(rt, screenCoords, STR_INSPECTION);

            // Reliability
            widget = &widgets[WIDX_PAGE_BACKGROUND];
            screenCoords = windowPos + ScreenCoordsXY{ widget->left + 4, widget->top + 4 };

            auto ft = Formatter();
            ft.Add<uint16_t>(reliability);
            DrawTextBasic(rt, screenCoords, STR_RELIABILITY_LABEL_1757, ft);
            screenCoords.y += 11;

            ft = Formatter();
            ft.Add<uint16_t>(downTime);
            DrawTextBasic(rt, screenCoords, STR_DOWN_TIME_LABEL_1889, ft);
            screenCoords.y += 26;

            // Last inspection
            StringId stringId;
            if (ride->lastInspection <= 1)
                stringId = STR_TIME_SINCE_LAST_INSPECTION_MINUTE;
            else if (ride->lastInspection <= 240)
                stringId = STR_TIME_SINCE_LAST_INSPECTION_MINUTES;
            else
                stringId = STR_TIME_SINCE_LAST_INSPECTION_MORE_THAN_4_HOURS;

            ft = Formatter();
            ft.Add<uint16_t>(ride->lastInspection);
            DrawTextBasic(rt, screenCoords, stringId, ft);
            screenCoords.y += 12;

            // Last / current breakdown
            if (ride->breakdownReason == BREAKDOWN_NONE)
                return;

            stringId = (ride->lifecycleFlags & RIDE_LIFECYCLE_BROKEN_DOWN) ? STR_CURRENT_BREAKDOWN : STR_LAST_BREAKDOWN;
            ft = Formatter();
            ft.Add<StringId>(RideBreakdownReasonNames[ride->breakdownReason]);
            DrawTextBasic(rt, screenCoords, stringId, ft);
            screenCoords.y += 12;

            // Mechanic status
            if (ride->lifecycleFlags & RIDE_LIFECYCLE_BROKEN_DOWN)
            {
                switch (ride->mechanicStatus)
                {
                    case MechanicStatus::calling:
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
                    case MechanicStatus::heading:
                        stringId = STR_MECHANIC_IS_HEADING_FOR_THE_RIDE;
                        break;
                    case MechanicStatus::fixing:
                    case MechanicStatus::hasFixedStationBrakes:
                        stringId = STR_MECHANIC_IS_FIXING_THE_RIDE;
                        break;
                    default:
                        stringId = kStringIdEmpty;
                        break;
                }

                if (stringId != kStringIdEmpty)
                {
                    if (stringId == STR_CALLING_MECHANIC || stringId == STR_NO_MECHANICS_ARE_HIRED_MESSAGE)
                    {
                        DrawTextWrapped(rt, screenCoords, 280, stringId, {}, { TextAlignment::left });
                    }
                    else
                    {
                        auto staff = getGameState().entities.GetEntity<Staff>(ride->mechanic);
                        if (staff != nullptr && staff->IsMechanic())
                        {
                            ft = Formatter();
                            staff->FormatNameTo(ft);
                            DrawTextWrapped(rt, screenCoords, 280, stringId, ft, { TextAlignment::left });
                        }
                    }
                }
            }
        }

#pragma endregion

#pragma region Colour

        int32_t HasTrackColour(const Ride& ride, int32_t trackColour)
        {
            // Get station flags (shops don't have them)
            auto stationObjFlags = 0;
            if (!ride.getRideTypeDescriptor().HasFlag(RtdFlag::isShopOrFacility))
            {
                auto stationObj = ride.getStationObject();
                if (stationObj != nullptr)
                {
                    stationObjFlags = stationObj->Flags;
                }
            }

            switch (trackColour)
            {
                case 0:
                    return (stationObjFlags & StationObjectFlags::hasPrimaryColour)
                        || ride.getRideTypeDescriptor().HasFlag(RtdFlag::hasTrackColourMain);
                case 1:
                    return (stationObjFlags & StationObjectFlags::hasSecondaryColour)
                        || ride.getRideTypeDescriptor().HasFlag(RtdFlag::hasTrackColourAdditional);
                case 2:
                    return ride.getRideTypeDescriptor().HasFlag(RtdFlag::hasTrackColourSupports);
                default:
                    return 0;
            }
        }

        void SetTrackColourScheme(const ScreenCoordsXY& screenPos)
        {
            auto newColourScheme = static_cast<uint8_t>(_rideColour);
            auto info = GetMapCoordinatesFromPos(screenPos, EnumsToFlags(ViewportInteractionItem::ride));

            if (info.interactionType != ViewportInteractionItem::ride)
                return;
            if (info.Element->GetType() != TileElementType::Track)
                return;
            if (info.Element->AsTrack()->GetRideIndex() != rideId)
                return;
            if (info.Element->AsTrack()->GetColourScheme() == newColourScheme)
                return;

            auto z = info.Element->GetBaseZ();
            auto direction = info.Element->GetDirection();
            auto gameAction = GameActions::RideSetColourSchemeAction(
                CoordsXYZD{ info.Loc, z, direction }, info.Element->AsTrack()->GetTrackType(), newColourScheme);
            GameActions::Execute(&gameAction, getGameState());
        }

        void ColourClose()
        {
            if (isToolActive(classification, number))
                ToolCancel();
        }

        void ColourOnMouseUp(WidgetIndex widgetIndex)
        {
            auto& gameState = getGameState();
            switch (widgetIndex)
            {
                case WIDX_CLOSE:
                    close();
                    return;
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
                    setPage(widgetIndex - WIDX_TAB_1);
                    break;
                case WIDX_PAINT_INDIVIDUAL_AREA:
                    ToolSet(*this, WIDX_PAINT_INDIVIDUAL_AREA, Tool::paintDown);
                    break;
                case WIDX_SELL_ITEM_RANDOM_COLOUR_CHECKBOX:
                {
                    auto ride = GetRide(rideId);
                    if (ride != nullptr)
                    {
                        const bool currentlyEnabled = ride->hasLifecycleFlag(RIDE_LIFECYCLE_RANDOM_SHOP_COLOURS);
                        auto rideSetAppearanceAction = GameActions::RideSetAppearanceAction(
                            rideId, GameActions::RideSetAppearanceType::SellingItemColourIsRandom, currentlyEnabled ? 0 : 1, 0);
                        GameActions::Execute(&rideSetAppearanceAction, gameState);
                    }
                    break;
                }
                case WIDX_RANDOMISE_VEHICLE_COLOURS:
                {
                    auto ride = GetRide(rideId);
                    if (ride == nullptr)
                        return;

                    auto rideEntry = ride->getRideEntry();
                    if (rideEntry == nullptr)
                        return;

                    bool allowChangingBodyColour = false;
                    bool allowChangingTrimColour = false;
                    bool allowChangingTertiaryColour = false;
                    for (int32_t i = 0; i < ride->numCarsPerTrain; i++)
                    {
                        uint8_t vehicleTypeIndex = RideEntryGetVehicleAtPosition(ride->subtype, ride->numCarsPerTrain, i);

                        if (rideEntry->Cars[vehicleTypeIndex].flags.has(CarEntryFlag::enableBodyColour))
                            allowChangingBodyColour = true;

                        if (rideEntry->Cars[vehicleTypeIndex].flags.has(CarEntryFlag::enableTrimColour))
                            allowChangingTrimColour = true;

                        if (rideEntry->Cars[vehicleTypeIndex].flags.has(CarEntryFlag::enableTertiaryColour))
                            allowChangingTertiaryColour = true;
                    }

                    int32_t numItems = ride->numTrains;
                    if (ride->vehicleColourSettings != VehicleColourSettings::perTrain)
                        numItems = ride->numCarsPerTrain;

                    for (auto i = 0; i < numItems; i++)
                    {
                        if (allowChangingBodyColour)
                        {
                            auto colour = static_cast<Colour>(UtilRand() % kColourNumNormal);
                            auto vehicleSetBodyColourAction = GameActions::RideSetAppearanceAction(
                                rideId, GameActions::RideSetAppearanceType::VehicleColourBody, EnumValue(colour), i);
                            GameActions::Execute(&vehicleSetBodyColourAction, gameState);
                        }

                        if (allowChangingTrimColour)
                        {
                            auto colour = static_cast<Colour>(UtilRand() % kColourNumNormal);
                            auto vehicleSetTrimColourAction = GameActions::RideSetAppearanceAction(
                                rideId, GameActions::RideSetAppearanceType::VehicleColourTrim, EnumValue(colour), i);
                            GameActions::Execute(&vehicleSetTrimColourAction, gameState);
                        }

                        if (allowChangingTertiaryColour)
                        {
                            auto colour = static_cast<Colour>(UtilRand() % kColourNumNormal);
                            auto vehicleSetTertiaryColourAction = GameActions::RideSetAppearanceAction(
                                rideId, GameActions::RideSetAppearanceType::VehicleColourTertiary, EnumValue(colour), i);
                            GameActions::Execute(&vehicleSetTertiaryColourAction, gameState);
                        }
                    }
                    break;
                }
            }
        }

        void ColourResize()
        {
            auto bottom = widgets[WIDX_VEHICLE_PREVIEW].bottom + 6 - getTitleBarDiffNormal();
            WindowSetResize(*this, { kMinimumWindowWidth, bottom }, { kMinimumWindowWidth, bottom });
        }

        void ColourOnMouseDown(WidgetIndex widgetIndex)
        {
            VehicleColour vehicleColour;
            int32_t numItems;
            StringId stringId;

            auto ride = GetRide(rideId);
            if (ride == nullptr)
                return;

            auto rideEntry = ride->getRideEntry();
            if (rideEntry == nullptr)
                return;

            auto colourSchemeIndex = _rideColour;
            auto dropdownWidget = &widgets[widgetIndex] - 1;

            switch (widgetIndex)
            {
                case WIDX_TRACK_COLOUR_SCHEME_DROPDOWN:
                {
                    for (size_t i = 0; i < std::size(ColourSchemeNames); i++)
                    {
                        gDropdown.items[i] = Dropdown::MenuLabel(ColourSchemeNames[i]);
                    }

                    WindowDropdownShowTextCustomWidth(
                        { windowPos.x + dropdownWidget->left, windowPos.y + dropdownWidget->top }, dropdownWidget->height(),
                        colours[1], 0, Dropdown::Flag::StayOpen, 4, widgets[widgetIndex].right - dropdownWidget->left);

                    gDropdown.items[colourSchemeIndex].setChecked(true);
                    break;
                }
                case WIDX_TRACK_MAIN_COLOUR:
                    WindowDropdownShowColour(
                        this, &widgets[widgetIndex], colours[1], ride->trackColours[colourSchemeIndex].main);
                    break;
                case WIDX_TRACK_ADDITIONAL_COLOUR:
                    WindowDropdownShowColour(
                        this, &widgets[widgetIndex], colours[1], ride->trackColours[colourSchemeIndex].additional);
                    break;
                case WIDX_TRACK_SUPPORT_COLOUR:
                    WindowDropdownShowColour(
                        this, &widgets[widgetIndex], colours[1], ride->trackColours[colourSchemeIndex].supports);
                    break;
                case WIDX_MAZE_STYLE_DROPDOWN:
                {
                    for (auto i = 0; i < 4; i++)
                    {
                        gDropdown.items[i] = Dropdown::MenuLabel(MazeOptions[i].text);
                    }

                    WindowDropdownShowTextCustomWidth(
                        { windowPos.x + dropdownWidget->left, windowPos.y + dropdownWidget->top }, dropdownWidget->height(),
                        colours[1], 0, Dropdown::Flag::StayOpen, 4, widgets[widgetIndex].right - dropdownWidget->left);

                    gDropdown.items[EnumValue(ride->trackColours[colourSchemeIndex].supports)].setChecked(true);
                    break;
                }
                case WIDX_ENTRANCE_STYLE_DROPDOWN:
                    ShowEntranceStyleDropdown();
                    break;
                case WIDX_VEHICLE_COLOUR_SCHEME_DROPDOWN:
                {
                    // Train, boat, ...
                    auto vehicleTypeName = GetRideComponentName(ride->getRideTypeDescriptor().NameConvention.vehicle).singular;

                    auto numDropdownItems = 2;
                    gDropdown.items[0] = Dropdown::MenuLabel(STR_ALL_VEHICLES_IN_SAME_COLOURS);
                    Formatter ft;
                    ft.Add<StringId>(vehicleTypeName);
                    gDropdown.items[1] = Dropdown::MenuLabel(STR_DIFFERENT_COLOURS_PER, ft);

                    if (getNumVisibleCars() > 1)
                    {
                        numDropdownItems++;
                        gDropdown.items[2] = Dropdown::MenuLabel(STR_DIFFERENT_COLOURS_PER_VEHICLE);
                    }

                    WindowDropdownShowTextCustomWidth(
                        { windowPos.x + dropdownWidget->left, windowPos.y + dropdownWidget->top }, dropdownWidget->height(),
                        colours[1], 0, Dropdown::Flag::StayOpen, numDropdownItems,
                        widgets[widgetIndex].right - dropdownWidget->left);

                    gDropdown.items[EnumValue(ride->vehicleColourSettings)].setChecked(true);
                    break;
                }
                case WIDX_VEHICLE_COLOUR_INDEX_DROPDOWN:
                {
                    numItems = ride->numTrains;
                    if (ride->vehicleColourSettings != VehicleColourSettings::perTrain)
                        numItems = ride->numCarsPerTrain;

                    stringId = ride->vehicleColourSettings == VehicleColourSettings::perTrain ? STR_RIDE_COLOUR_TRAIN_OPTION
                                                                                              : STR_RIDE_COLOUR_VEHICLE_OPTION;
                    auto dropdownIndex = 0;
                    for (auto i = 0; i < std::min(numItems, Dropdown::kItemsMaxSize); i++)
                    {
                        if (ride->vehicleColourSettings == VehicleColourSettings::perCar)
                        {
                            const auto& carEntry = rideEntry->Cars[RideEntryGetVehicleAtPosition(
                                ride->subtype, ride->numCarsPerTrain, i)];
                            if (!carEntry.isVisible())
                            {
                                continue;
                            }
                        }

                        uint16_t vehicleIndex = dropdownIndex + 1;
                        Formatter ft;
                        ft.Add<StringId>(
                            GetRideComponentName(ride->getRideTypeDescriptor().NameConvention.vehicle).capitalised);
                        ft.Add<uint16_t>(vehicleIndex);

                        gDropdown.items[dropdownIndex] = Dropdown::MenuLabel(stringId, ft);
                        dropdownIndex++;
                    }

                    WindowDropdownShowTextCustomWidth(
                        { windowPos.x + dropdownWidget->left, windowPos.y + dropdownWidget->top }, dropdownWidget->height(),
                        colours[1], 0, Dropdown::Flag::StayOpen, dropdownIndex,
                        widgets[widgetIndex].right - dropdownWidget->left);

                    gDropdown.items[carIndexToDropdownIndex(_vehicleIndex)].setChecked(true);
                    break;
                }
                case WIDX_VEHICLE_BODY_COLOUR:
                    vehicleColour = RideGetVehicleColour(*ride, _vehicleIndex);
                    WindowDropdownShowColour(this, &widgets[widgetIndex], colours[1], vehicleColour.Body);
                    break;
                case WIDX_VEHICLE_TRIM_COLOUR:
                    vehicleColour = RideGetVehicleColour(*ride, _vehicleIndex);
                    WindowDropdownShowColour(this, &widgets[widgetIndex], colours[1], vehicleColour.Trim);
                    break;
                case WIDX_VEHICLE_TERTIARY_COLOUR:
                    vehicleColour = RideGetVehicleColour(*ride, _vehicleIndex);
                    WindowDropdownShowColour(this, &widgets[widgetIndex], colours[1], vehicleColour.Tertiary);
                    break;
            }
        }

        void ColourOnDropdown(WidgetIndex widgetIndex, int32_t dropdownIndex)
        {
            if (dropdownIndex == -1)
                return;

            auto& gameState = getGameState();
            switch (widgetIndex)
            {
                case WIDX_TRACK_COLOUR_SCHEME_DROPDOWN:
                    _rideColour = static_cast<uint16_t>(dropdownIndex);
                    invalidate();
                    break;
                case WIDX_TRACK_MAIN_COLOUR:
                {
                    auto rideSetAppearanceAction = GameActions::RideSetAppearanceAction(
                        rideId, GameActions::RideSetAppearanceType::TrackColourMain,
                        EnumValue(ColourDropDownIndexToColour(dropdownIndex)), _rideColour);
                    GameActions::Execute(&rideSetAppearanceAction, gameState);
                }
                break;
                case WIDX_TRACK_ADDITIONAL_COLOUR:
                {
                    auto rideSetAppearanceAction = GameActions::RideSetAppearanceAction(
                        rideId, GameActions::RideSetAppearanceType::TrackColourAdditional,
                        EnumValue(ColourDropDownIndexToColour(dropdownIndex)), _rideColour);
                    GameActions::Execute(&rideSetAppearanceAction, gameState);
                }
                break;
                case WIDX_TRACK_SUPPORT_COLOUR:
                {
                    auto rideSetAppearanceAction = GameActions::RideSetAppearanceAction(
                        rideId, GameActions::RideSetAppearanceType::TrackColourSupports,
                        EnumValue(ColourDropDownIndexToColour(dropdownIndex)), _rideColour);
                    GameActions::Execute(&rideSetAppearanceAction, gameState);
                }
                break;
                case WIDX_MAZE_STYLE_DROPDOWN:
                {
                    auto rideSetAppearanceAction = GameActions::RideSetAppearanceAction(
                        rideId, GameActions::RideSetAppearanceType::MazeStyle, dropdownIndex, _rideColour);
                    GameActions::Execute(&rideSetAppearanceAction, gameState);
                }
                break;
                case WIDX_ENTRANCE_STYLE_DROPDOWN:
                {
                    if (static_cast<size_t>(dropdownIndex) >= _entranceDropdownData.size())
                    {
                        break;
                    }
                    auto objIndex = _entranceDropdownData[dropdownIndex].EntranceTypeId;
                    auto rideSetAppearanceAction = GameActions::RideSetAppearanceAction(
                        rideId, GameActions::RideSetAppearanceType::EntranceStyle, objIndex, 0);
                    rideSetAppearanceAction.SetCallback(
                        [objIndex](const GameActions::GameAction*, const GameActions::Result* res) {
                            if (res->error != GameActions::Status::ok)
                                return;
                            getGameState().lastEntranceStyle = objIndex;
                        });
                    GameActions::Execute(&rideSetAppearanceAction, gameState);
                    break;
                }
                case WIDX_VEHICLE_COLOUR_SCHEME_DROPDOWN:
                {
                    auto rideSetAppearanceAction = GameActions::RideSetAppearanceAction(
                        rideId, GameActions::RideSetAppearanceType::VehicleColourScheme, dropdownIndex, 0);
                    rideSetAppearanceAction.SetCallback(
                        [this](const GameActions::GameAction* ga, const GameActions::Result* result) {
                            if (result->error == GameActions::Status::ok)
                            {
                                resetVehicleIndex();
                            }
                        });
                    GameActions::Execute(&rideSetAppearanceAction, gameState);
                    break;
                }
                case WIDX_VEHICLE_COLOUR_INDEX_DROPDOWN:
                    _vehicleIndex = dropdownIndexToCarIndex(dropdownIndex);
                    invalidate();
                    break;
                case WIDX_VEHICLE_BODY_COLOUR:
                {
                    auto rideSetAppearanceAction = GameActions::RideSetAppearanceAction(
                        rideId, GameActions::RideSetAppearanceType::VehicleColourBody,
                        EnumValue(ColourDropDownIndexToColour(dropdownIndex)), _vehicleIndex);
                    GameActions::Execute(&rideSetAppearanceAction, gameState);
                }
                break;
                case WIDX_VEHICLE_TRIM_COLOUR:
                {
                    auto rideSetAppearanceAction = GameActions::RideSetAppearanceAction(
                        rideId, GameActions::RideSetAppearanceType::VehicleColourTrim,
                        EnumValue(ColourDropDownIndexToColour(dropdownIndex)), _vehicleIndex);
                    GameActions::Execute(&rideSetAppearanceAction, gameState);
                }
                break;
                case WIDX_VEHICLE_TERTIARY_COLOUR:
                {
                    auto rideSetAppearanceAction = GameActions::RideSetAppearanceAction(
                        rideId, GameActions::RideSetAppearanceType::VehicleColourTertiary,
                        EnumValue(ColourDropDownIndexToColour(dropdownIndex)), _vehicleIndex);
                    GameActions::Execute(&rideSetAppearanceAction, gameState);
                }
                break;
            }
        }

        void ColourUpdate()
        {
            currentFrame++;
            onPrepareDraw();
            invalidateWidget(WIDX_TAB_5);
            invalidateWidget(WIDX_VEHICLE_PREVIEW);
        }

        void ColourOnToolDown(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords)
        {
            if (widgetIndex == WIDX_PAINT_INDIVIDUAL_AREA)
                SetTrackColourScheme(screenCoords);
        }

        void ColourOnToolDrag(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords)
        {
            if (widgetIndex == WIDX_PAINT_INDIVIDUAL_AREA)
                SetTrackColourScheme(screenCoords);
        }

        void ColourOnPrepareDraw()
        {
            TrackColour trackColour;
            VehicleColour vehicleColour;

            SetPressedTab();

            auto ride = GetRide(rideId);
            if (ride == nullptr)
                return;

            auto rideEntry = ride->getRideEntry();
            if (rideEntry == nullptr)
                return;

            widgets[WIDX_TITLE].text = STR_ARG_16_STRINGID;
            auto ft = Formatter::Common();
            ft.Increment(16);
            ride->formatNameTo(ft);

            // Track colours
            int32_t colourScheme = _rideColour;
            trackColour = ride->trackColours[colourScheme];

            // Maze style
            const auto& rtd = ride->getRideTypeDescriptor();
            if (rtd.specialType == RtdSpecialType::maze)
            {
                widgets[WIDX_MAZE_STYLE].type = WidgetType::dropdownMenu;
                widgets[WIDX_MAZE_STYLE_DROPDOWN].type = WidgetType::button;
                widgets[WIDX_MAZE_STYLE].text = MazeOptions[EnumValue(trackColour.supports)].text;
            }
            else
            {
                widgets[WIDX_MAZE_STYLE].type = WidgetType::empty;
                widgets[WIDX_MAZE_STYLE_DROPDOWN].type = WidgetType::empty;
            }

            // Track, multiple colour schemes
            if (ride->getRideTypeDescriptor().HasFlag(RtdFlag::supportsMultipleColourSchemes))
            {
                widgets[WIDX_TRACK_COLOUR_SCHEME].type = WidgetType::dropdownMenu;
                widgets[WIDX_TRACK_COLOUR_SCHEME_DROPDOWN].type = WidgetType::button;
                widgets[WIDX_PAINT_INDIVIDUAL_AREA].type = WidgetType::flatBtn;
            }
            else
            {
                widgets[WIDX_TRACK_COLOUR_SCHEME].type = WidgetType::empty;
                widgets[WIDX_TRACK_COLOUR_SCHEME_DROPDOWN].type = WidgetType::empty;
                widgets[WIDX_PAINT_INDIVIDUAL_AREA].type = WidgetType::empty;
            }

            // Track main colour
            if (HasTrackColour(*ride, 0))
            {
                widgets[WIDX_TRACK_MAIN_COLOUR].type = WidgetType::colourBtn;
                widgets[WIDX_TRACK_MAIN_COLOUR].image = getColourButtonImage(trackColour.main);
            }
            else
            {
                widgets[WIDX_TRACK_MAIN_COLOUR].type = WidgetType::empty;
            }

            // Track additional colour
            if (HasTrackColour(*ride, 1))
            {
                widgets[WIDX_TRACK_ADDITIONAL_COLOUR].type = WidgetType::colourBtn;
                widgets[WIDX_TRACK_ADDITIONAL_COLOUR].image = getColourButtonImage(trackColour.additional);
            }
            else
            {
                widgets[WIDX_TRACK_ADDITIONAL_COLOUR].type = WidgetType::empty;
            }

            // Selling item random colour checkbox
            if (ride->hasRecolourableShopItems())
            {
                widgets[WIDX_SELL_ITEM_RANDOM_COLOUR_CHECKBOX].type = WidgetType::checkbox;
                if (ride->hasLifecycleFlag(RIDE_LIFECYCLE_RANDOM_SHOP_COLOURS))
                {
                    pressedWidgets |= (1uLL << WIDX_SELL_ITEM_RANDOM_COLOUR_CHECKBOX);
                }
                else
                {
                    pressedWidgets &= ~(1uLL << WIDX_SELL_ITEM_RANDOM_COLOUR_CHECKBOX);
                }
            }
            else
            {
                widgets[WIDX_SELL_ITEM_RANDOM_COLOUR_CHECKBOX].type = WidgetType::empty;
            }

            // Track supports colour
            if (HasTrackColour(*ride, 2) && rtd.specialType != RtdSpecialType::maze)
            {
                widgets[WIDX_TRACK_SUPPORT_COLOUR].type = WidgetType::colourBtn;
                widgets[WIDX_TRACK_SUPPORT_COLOUR].image = getColourButtonImage(trackColour.supports);
            }
            else
            {
                widgets[WIDX_TRACK_SUPPORT_COLOUR].type = WidgetType::empty;
            }

            // Track preview
            if (rtd.HasFlag(RtdFlag::hasTrackColourMain) || rtd.HasFlag(RtdFlag::hasTrackColourAdditional)
                || rtd.HasFlag(RtdFlag::hasTrackColourSupports))
                widgets[WIDX_TRACK_PREVIEW].type = WidgetType::spinner;
            else
                widgets[WIDX_TRACK_PREVIEW].type = WidgetType::empty;

            // Entrance style
            if (ride->getRideTypeDescriptor().HasFlag(RtdFlag::hasEntranceAndExit))
            {
                widgets[WIDX_ENTRANCE_PREVIEW].type = WidgetType::spinner;
                widgets[WIDX_ENTRANCE_STYLE].type = WidgetType::dropdownMenu;
                widgets[WIDX_ENTRANCE_STYLE_DROPDOWN].type = WidgetType::button;

                auto stringId = kStringIdNone;
                auto stationObj = ride->getStationObject();
                if (stationObj != nullptr)
                {
                    stringId = stationObj->NameStringId;
                }
                widgets[WIDX_ENTRANCE_STYLE].text = stringId;
            }
            else
            {
                widgets[WIDX_ENTRANCE_PREVIEW].type = WidgetType::empty;
                widgets[WIDX_ENTRANCE_STYLE].type = WidgetType::empty;
                widgets[WIDX_ENTRANCE_STYLE_DROPDOWN].type = WidgetType::empty;
            }

            // Vehicle colours
            if (!ride->getRideTypeDescriptor().HasFlag(RtdFlag::noVehicles)
                && ride->getRideTypeDescriptor().HasFlag(RtdFlag::hasVehicleColours))
            {
                if (ride->vehicleColourSettings == VehicleColourSettings::same)
                {
                    _vehicleIndex = 0;
                    widgets[WIDX_RANDOMISE_VEHICLE_COLOURS].type = WidgetType::empty;
                }
                else
                {
                    widgets[WIDX_RANDOMISE_VEHICLE_COLOURS].type = WidgetType::button;
                }

                vehicleColour = RideGetVehicleColour(*ride, _vehicleIndex);

                widgets[WIDX_VEHICLE_PREVIEW].type = WidgetType::scroll;
                widgets[WIDX_VEHICLE_BODY_COLOUR].type = WidgetType::colourBtn;
                widgets[WIDX_VEHICLE_BODY_COLOUR].image = getColourButtonImage(vehicleColour.Body);

                bool allowChangingBodyColour = false;
                bool allowChangingTrimColour = false;
                bool allowChangingTertiaryColour = false;

                for (int32_t i = 0; i < ride->numCarsPerTrain; i++)
                {
                    uint8_t vehicleTypeIndex = RideEntryGetVehicleAtPosition(ride->subtype, ride->numCarsPerTrain, i);

                    if (rideEntry->Cars[vehicleTypeIndex].flags.has(CarEntryFlag::enableBodyColour))
                        allowChangingBodyColour = true;

                    if (rideEntry->Cars[vehicleTypeIndex].flags.has(CarEntryFlag::enableTrimColour))
                        allowChangingTrimColour = true;

                    if (rideEntry->Cars[vehicleTypeIndex].flags.has(CarEntryFlag::enableTertiaryColour))
                        allowChangingTertiaryColour = true;
                }

                widgets[WIDX_VEHICLE_BODY_COLOUR].type = WidgetType::empty;
                widgets[WIDX_VEHICLE_TRIM_COLOUR].type = WidgetType::empty;
                widgets[WIDX_VEHICLE_TERTIARY_COLOUR].type = WidgetType::empty;

                if (allowChangingBodyColour)
                {
                    widgets[WIDX_VEHICLE_BODY_COLOUR].type = WidgetType::colourBtn;
                    widgets[WIDX_VEHICLE_BODY_COLOUR].image = getColourButtonImage(vehicleColour.Body);
                }
                if (allowChangingTrimColour)
                {
                    widgets[WIDX_VEHICLE_TRIM_COLOUR].type = WidgetType::colourBtn;
                    widgets[WIDX_VEHICLE_TRIM_COLOUR].image = getColourButtonImage(vehicleColour.Trim);
                }
                if (allowChangingTertiaryColour)
                {
                    widgets[WIDX_VEHICLE_TERTIARY_COLOUR].type = WidgetType::colourBtn;
                    widgets[WIDX_VEHICLE_TERTIARY_COLOUR].image = getColourButtonImage(vehicleColour.Tertiary);
                }

                // Vehicle colour scheme type
                if (!ride->getRideTypeDescriptor().HasFlag(RtdFlag::vehicleIsIntegral)
                    && (ride->numCarsPerTrain | ride->numTrains) > 1)
                {
                    widgets[WIDX_VEHICLE_COLOUR_SCHEME].type = WidgetType::dropdownMenu;
                    widgets[WIDX_VEHICLE_COLOUR_SCHEME_DROPDOWN].type = WidgetType::button;
                }
                else
                {
                    widgets[WIDX_VEHICLE_COLOUR_SCHEME].type = WidgetType::empty;
                    widgets[WIDX_VEHICLE_COLOUR_SCHEME_DROPDOWN].type = WidgetType::empty;
                }
                ft.Rewind();
                ft.Increment(6);
                ft.Add<StringId>(VehicleColourSchemeNames[EnumValue(ride->vehicleColourSettings)]);
                ft.Add<StringId>(GetRideComponentName(ride->getRideTypeDescriptor().NameConvention.vehicle).singular);
                ft.Add<StringId>(GetRideComponentName(ride->getRideTypeDescriptor().NameConvention.vehicle).capitalised);
                ft.Add<uint16_t>(carIndexToDropdownIndex(_vehicleIndex) + 1);

                // Vehicle index
                if (ride->vehicleColourSettings != VehicleColourSettings::same)
                {
                    widgets[WIDX_VEHICLE_COLOUR_INDEX].type = WidgetType::dropdownMenu;
                    widgets[WIDX_VEHICLE_COLOUR_INDEX_DROPDOWN].type = WidgetType::button;
                    widgets[WIDX_VEHICLE_COLOUR_INDEX].text = ride->vehicleColourSettings == VehicleColourSettings::perTrain
                        ? STR_RIDE_COLOUR_TRAIN_VALUE
                        : STR_RIDE_COLOUR_VEHICLE_VALUE;
                }
                else
                {
                    widgets[WIDX_VEHICLE_COLOUR_INDEX].type = WidgetType::empty;
                    widgets[WIDX_VEHICLE_COLOUR_INDEX_DROPDOWN].type = WidgetType::empty;
                }
            }
            else
            {
                widgets[WIDX_VEHICLE_PREVIEW].type = WidgetType::empty;
                widgets[WIDX_VEHICLE_COLOUR_SCHEME].type = WidgetType::empty;
                widgets[WIDX_VEHICLE_COLOUR_SCHEME_DROPDOWN].type = WidgetType::empty;
                widgets[WIDX_VEHICLE_COLOUR_INDEX].type = WidgetType::empty;
                widgets[WIDX_VEHICLE_COLOUR_INDEX_DROPDOWN].type = WidgetType::empty;
                widgets[WIDX_VEHICLE_BODY_COLOUR].type = WidgetType::empty;
                widgets[WIDX_VEHICLE_TRIM_COLOUR].type = WidgetType::empty;
                widgets[WIDX_VEHICLE_TERTIARY_COLOUR].type = WidgetType::empty;
                widgets[WIDX_RANDOMISE_VEHICLE_COLOURS].type = WidgetType::empty;
            }

            ft.Rewind();
            ft.Increment(14);
            ft.Add<StringId>(ColourSchemeNames[colourScheme]);

            WindowAlignTabs(this, WIDX_TAB_1, WIDX_TAB_10);
        }

        void ColourOnDraw(RenderTarget& rt)
        {
            // TODO: This should use lists and identified sprites
            RenderTarget clippedRT;

            auto ride = GetRide(rideId);
            if (ride == nullptr)
                return;

            drawWidgets(rt);
            drawTabImages(rt);

            // Track / shop item preview
            const auto& trackPreviewWidget = widgets[WIDX_TRACK_PREVIEW];
            if (trackPreviewWidget.type != WidgetType::empty)
                Rectangle::fill(
                    rt,
                    { { windowPos + ScreenCoordsXY{ trackPreviewWidget.left + 1, trackPreviewWidget.top + 1 } },
                      { windowPos + ScreenCoordsXY{ trackPreviewWidget.right - 1, trackPreviewWidget.bottom - 1 } } },
                    PaletteIndex::pi12);

            auto trackColour = ride->trackColours[_rideColour];

            //
            auto rideEntry = ride->getRideEntry();
            if (rideEntry == nullptr || rideEntry->shop_item[0] == ShopItem::none)
            {
                auto screenCoords = windowPos + ScreenCoordsXY{ trackPreviewWidget.left, trackPreviewWidget.top };

                // Track
                const auto& rtd = ride->getRideTypeDescriptor();
                if (rtd.specialType == RtdSpecialType::maze)
                {
                    GfxDrawSprite(rt, ImageId(MazeOptions[EnumValue(trackColour.supports)].sprite), screenCoords);
                }
                else
                {
                    const auto& typeDescriptor = ride->getRideTypeDescriptor();
                    int32_t spriteIndex = typeDescriptor.ColourPreview.Track;
                    if (spriteIndex != 0)
                    {
                        GfxDrawSprite(rt, ImageId(spriteIndex, trackColour.main, trackColour.additional), screenCoords);
                    }

                    // Supports
                    spriteIndex = typeDescriptor.ColourPreview.Supports;
                    if (spriteIndex != 0)
                    {
                        GfxDrawSprite(rt, ImageId(spriteIndex, trackColour.supports), screenCoords);
                    }
                }
            }
            else
            {
                auto screenCoords = windowPos
                    + ScreenCoordsXY{ (trackPreviewWidget.left + trackPreviewWidget.right) / 2 - 8,
                                      (trackPreviewWidget.bottom + trackPreviewWidget.top) / 2 - 6 };

                ShopItem shopItem = rideEntry->shop_item[1] == ShopItem::none ? rideEntry->shop_item[0]
                                                                              : rideEntry->shop_item[1];
                if (ride->hasLifecycleFlag(RIDE_LIFECYCLE_RANDOM_SHOP_COLOURS))
                {
                    auto spriteColour = static_cast<Colour>((getGameState().currentTicks / 32) % kColourNumTotal);

                    GfxDrawSprite(rt, ImageId(GetShopItemDescriptor(shopItem).Image, spriteColour), screenCoords);
                }
                else
                {
                    GfxDrawSprite(rt, ImageId(GetShopItemDescriptor(shopItem).Image, ride->trackColours[0].main), screenCoords);
                }
            }

            // Entrance preview
            trackColour = ride->trackColours[0];
            const auto& entrancePreviewWidget = widgets[WIDX_ENTRANCE_PREVIEW];
            if (entrancePreviewWidget.type != WidgetType::empty)
            {
                if (ClipRenderTarget(
                        clippedRT, rt,
                        windowPos + ScreenCoordsXY{ entrancePreviewWidget.left + 1, entrancePreviewWidget.top + 1 },
                        entrancePreviewWidget.width() - 1, entrancePreviewWidget.height() - 1))
                {
                    GfxClear(clippedRT, PaletteIndex::pi12);

                    auto stationObj = ride->getStationObject();
                    if (stationObj != nullptr && stationObj->BaseImageId != kImageIndexUndefined)
                    {
                        auto imageId = ImageId(stationObj->BaseImageId, trackColour.main, trackColour.additional);

                        // Back
                        GfxDrawSprite(clippedRT, imageId, { 34, 20 });

                        // Front
                        GfxDrawSprite(clippedRT, imageId.WithIndexOffset(4), { 34, 20 });

                        // Glass
                        if (stationObj->Flags & StationObjectFlags::isTransparent)
                        {
                            auto glassImageId = ImageId(stationObj->BaseImageId + 20).WithTransparency(trackColour.main);
                            GfxDrawSprite(clippedRT, glassImageId, { 34, 20 });
                        }
                    }
                }

                auto labelPos = windowPos + ScreenCoordsXY{ 3, widgets[WIDX_ENTRANCE_STYLE].top };
                DrawTextEllipsised(rt, labelPos, 97, STR_STATION_STYLE, {});
            }
        }

        void ColourOnScrollDraw(RenderTarget& rt, int32_t scrollIndex) const
        {
            auto ride = GetRide(rideId);
            if (ride == nullptr)
                return;

            auto rideEntry = ride->getRideEntry();
            if (rideEntry == nullptr)
                return;

            auto vehiclePreviewWidget = &widgets[WIDX_VEHICLE_PREVIEW];
            auto vehicleColour = RideGetVehicleColour(*ride, _vehicleIndex);

            // Background colour
            Rectangle::fill(rt, { { rt.x, rt.y }, { rt.x + rt.width - 1, rt.y + rt.height - 1 } }, PaletteIndex::pi12);

            // ?
            auto screenCoords = ScreenCoordsXY{ vehiclePreviewWidget->width() / 2, vehiclePreviewWidget->height() - 16 };

            // ?
            auto trainCarIndex = ride->vehicleColourSettings == VehicleColourSettings::perCar ? _vehicleIndex
                                                                                              : rideEntry->TabCar;

            const auto& carEntry = rideEntry->Cars[RideEntryGetVehicleAtPosition(
                ride->subtype, ride->numCarsPerTrain, trainCarIndex)];

            screenCoords.y += carEntry.tab_height;

            // Draw the coloured spinning vehicle
            // currentFrame represents a SpritePrecision of 64
            ImageIndex imageIndex = carEntry.SpriteByYaw(currentFrame / 2, SpriteGroupType::SlopeFlat);
            imageIndex &= carEntry.TabRotationMask;
            imageIndex *= carEntry.base_num_frames;
            imageIndex += carEntry.base_image_id;
            auto imageId = ImageId(imageIndex, vehicleColour.Body, vehicleColour.Trim, vehicleColour.Tertiary);
            GfxDrawSprite(rt, imageId, screenCoords);
        }

#pragma endregion

#pragma region Music
        std::vector<ObjectEntryIndex> window_ride_current_music_style_order;

        void ToggleMusic()
        {
            auto ride = GetRide(rideId);
            if (ride != nullptr)
            {
                int32_t activateMusic = (ride->lifecycleFlags & RIDE_LIFECYCLE_MUSIC) ? 0 : 1;
                SetOperatingSetting(rideId, GameActions::RideSetSetting::Music, activateMusic);
                ride->windowInvalidateFlags.set(RideInvalidateFlag::music);
            }
        }

        void MusicOnMouseUp(WidgetIndex widgetIndex)
        {
            switch (widgetIndex)
            {
                case WIDX_CLOSE:
                    close();
                    return;
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
                    setPage(widgetIndex - WIDX_TAB_1);
                    break;
                case WIDX_PLAY_MUSIC:
                    ToggleMusic();
                    break;
            }
        }

        void MusicResize()
        {
            flags |= WindowFlag::resizable;

            auto ride = GetRide(rideId);
            if (ride == nullptr)
                return;

            // Expand the window when music is playing
            auto isMusicActivated = (ride->lifecycleFlags & RIDE_LIFECYCLE_MUSIC) != 0;
            auto standardHeight = widgets[WIDX_MUSIC_DROPDOWN].bottom + 6;
            auto newMinHeight = isMusicActivated ? standardHeight + 133 : standardHeight;
            auto newMaxHeight = isMusicActivated ? standardHeight + 369 : standardHeight;
            WindowSetResize(*this, { kMinimumWindowWidth, newMinHeight }, { 500, newMaxHeight });
        }

        static std::string GetMusicString(ObjectEntryIndex musicObjectIndex)
        {
            auto& objManager = GetContext()->GetObjectManager();
            auto musicObj = objManager.GetLoadedObject<MusicObject>(musicObjectIndex);

            return LanguageGetString(musicObj->NameStringId);
        }

        /**
         *
         *  rct2: 0x006B1EFC
         */
        void MusicOnMouseDown(WidgetIndex widgetIndex)
        {
            if (widgetIndex != WIDX_MUSIC_DROPDOWN)
                return;

            auto dropdownWidget = &widgets[widgetIndex] - 1;
            auto ride = GetRide(rideId);
            if (ride == nullptr)
                return;

            const auto& rtd = ride->getRideTypeDescriptor();
            // Construct list of available music
            auto& musicOrder = window_ride_current_music_style_order;
            musicOrder.clear();
            auto& objManager = GetContext()->GetObjectManager();
            for (ObjectEntryIndex i = 0; i < kMaxMusicObjects; i++)
            {
                auto musicObj = objManager.GetLoadedObject<MusicObject>(i);
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

                    if (getGameState().cheats.unlockOperatingLimits
                        || musicObj->SupportsRideType(ride->type, rtd.HasFlag(RtdFlag::requireExplicitListingInMusicObjects)))
                    {
                        musicOrder.push_back(i);
                    }
                }
            }

            // Sort available music by the alphabetical order
            std::stable_sort(musicOrder.begin(), musicOrder.end(), [](const ObjectEntryIndex& a, const ObjectEntryIndex& b) {
                return String::compare(GetMusicString(b), GetMusicString(a), false) > 0;
            });

            // Setup dropdown list
            auto numItems = musicOrder.size();
            for (size_t i = 0; i < numItems; i++)
            {
                auto musicObj = objManager.GetLoadedObject<MusicObject>(musicOrder[i]);
                gDropdown.items[i] = Dropdown::MenuLabel(musicObj->NameStringId);
            }

            WindowDropdownShowTextCustomWidth(
                { windowPos.x + dropdownWidget->left, windowPos.y + dropdownWidget->top }, dropdownWidget->height(), colours[1],
                0, Dropdown::Flag::StayOpen, numItems, widgets[widgetIndex].right - dropdownWidget->left);

            // Set currently checked item
            for (size_t i = 0; i < numItems; i++)
            {
                if (musicOrder[i] == ride->music)
                {
                    gDropdown.items[static_cast<int32_t>(i)].setChecked(true);
                }
            }
        }

        void MusicOnDropdown(WidgetIndex widgetIndex, int32_t dropdownIndex)
        {
            if (widgetIndex == WIDX_MUSIC_DROPDOWN && dropdownIndex >= 0
                && static_cast<size_t>(dropdownIndex) < window_ride_current_music_style_order.size())
            {
                auto musicStyle = window_ride_current_music_style_order[dropdownIndex];
                SetOperatingSetting(rideId, GameActions::RideSetSetting::MusicType, musicStyle);
            }
        }

        void MusicUpdate()
        {
            currentFrame++;
            onPrepareDraw();
            invalidateWidget(WIDX_TAB_6);

            if (auto ride = GetRide(rideId); ride != nullptr && ride->windowInvalidateFlags.has(RideInvalidateFlag::music))
            {
                ride->windowInvalidateFlags.unset(RideInvalidateFlag::music);
                invalidate();
                onResize();
                onPrepareDraw();
                invalidate();
            }

            widgetScrollUpdateThumbs(*this, WIDX_MUSIC_DATA);
        }

        ScreenSize MusicScrollGetSize(int32_t scrollIndex)
        {
            // Hack: can be removed when widgets are no longer shared globally
            onPrepareDraw();

            // Figure out minimum size
            ScreenSize size{};
            size.height = widgets[WIDX_MUSIC_DATA].height() - 3;
            size.width = widgets[WIDX_MUSIC_DATA].width() - 3;

            auto ride = GetRide(rideId);
            if (ride == nullptr)
                return size;

            auto musicObj = ride->getMusicObject();
            if (musicObj == nullptr)
                return size;

            // Compute scroll width based on track name and composer
            int32_t newWidth = 0;
            for (size_t i = 0; i < musicObj->GetTrackCount(); i++)
            {
                const auto* track = musicObj->GetTrack(i);
                if (track->Name.empty())
                    continue;

                auto nameWidth = GfxGetStringWidth(track->Name.c_str(), FontStyle::small);
                auto composerWidth = GfxGetStringWidth(track->Composer.c_str(), FontStyle::small);
                newWidth = std::max(newWidth, nameWidth + composerWidth + 24);
            }

            // Compute scroll height based on number of tracks
            const int32_t newHeight = static_cast<int32_t>(musicObj->GetTrackCount()) * kScrollableRowHeight;

            // Return the computed size
            return { newWidth, newHeight };
        }

        void MusicOnPrepareDraw()
        {
            SetPressedTab();

            auto ride = GetRide(rideId);
            if (ride == nullptr)
                return;

            auto ft = Formatter::Common();
            ride->formatNameTo(ft);

            // Align music dropdown
            widgets[WIDX_MUSIC].right = width - 8;
            widgets[WIDX_MUSIC_DROPDOWN].right = width - 9;
            widgets[WIDX_MUSIC_DROPDOWN].left = width - 19;

            // Set selected music
            StringId musicName = kStringIdNone;
            auto musicObj = ride->getMusicObject();
            if (musicObj != nullptr)
            {
                musicName = musicObj->NameStringId;
            }
            widgets[WIDX_MUSIC].text = musicName;

            auto isMusicActivated = (ride->lifecycleFlags & RIDE_LIFECYCLE_MUSIC) != 0;
            bool hasPreviewImage = musicObj != nullptr && musicObj->HasPreview();

            widgetSetVisible(*this, WIDX_MUSIC_DATA, isMusicActivated);
            widgetSetVisible(*this, WIDX_MUSIC_IMAGE, isMusicActivated && hasPreviewImage);

            if (isMusicActivated)
            {
                widgets[WIDX_MUSIC_DATA].bottom = height - 11;

                if (hasPreviewImage)
                {
                    widgets[WIDX_MUSIC_DATA].right = width - 129;
                    widgets[WIDX_MUSIC_IMAGE].left = width - 121;
                    widgets[WIDX_MUSIC_IMAGE].right = width - 8;
                }
                else
                {
                    widgets[WIDX_MUSIC_DATA].right = width - 8;
                }

                pressedWidgets |= (1uLL << WIDX_PLAY_MUSIC) | (1uLL << WIDX_MUSIC_IMAGE);
                disabledWidgets &= ~((1uLL << WIDX_MUSIC) | (1uLL << WIDX_MUSIC_DROPDOWN) | (1uLL << WIDX_MUSIC_DATA));
            }
            else
            {
                pressedWidgets &= ~(1uLL << WIDX_PLAY_MUSIC);
                pressedWidgets |= (1uLL << WIDX_MUSIC_IMAGE);
                disabledWidgets |= (1uLL << WIDX_MUSIC) | (1uLL << WIDX_MUSIC_DROPDOWN) | (1uLL << WIDX_MUSIC_DATA);
            }

            WindowAlignTabs(this, WIDX_TAB_1, WIDX_TAB_10);
        }

        void MusicOnDraw(RenderTarget& rt)
        {
            drawWidgets(rt);
            drawTabImages(rt);
            auto ride = GetRide(rideId);
            if (ride == nullptr)
                return;

            // Draw music data only when music is activated
            auto isMusicActivated = (ride->lifecycleFlags & RIDE_LIFECYCLE_MUSIC) != 0;
            if (!isMusicActivated)
                return;

            // 'Tracks' caption
            auto trackLabelPos = windowPos + ScreenCoordsXY{ widgets[WIDX_MUSIC_DATA].left, widgets[WIDX_MUSIC_DATA].top - 13 };
            DrawTextWrapped(rt, trackLabelPos, width, STR_MUSIC_OBJECT_TRACK_HEADER, {}, { TextAlignment::left });

            // Do we have a preview image to draw?
            auto musicObj = ride->getMusicObject();
            bool hasPreview = musicObj != nullptr && musicObj->HasPreview();
            if (!hasPreview)
                return;

            // Figure out where the image should go
            const auto& previewWidget = widgets[WIDX_MUSIC_IMAGE];
            int32_t clipWidth = previewWidget.width() - 2;
            int32_t clipHeight = previewWidget.height() - 2;

            // Draw the preview image
            RenderTarget clipRT;
            auto screenPos = windowPos + ScreenCoordsXY{ previewWidget.left + 1, previewWidget.top + 1 };
            if (ClipRenderTarget(clipRT, rt, screenPos, clipWidth, clipHeight))
            {
                musicObj->DrawPreview(clipRT, clipWidth, clipHeight);
            }
        }

        void MusicOnScrollDraw(RenderTarget& rt, int32_t scrollIndex)
        {
            auto ride = GetRide(rideId);
            if (ride == nullptr)
                return;

            // only draw track listing when music is activated
            auto isMusicActivated = (ride->lifecycleFlags & RIDE_LIFECYCLE_MUSIC) != 0;
            if (!isMusicActivated)
                return;

            auto paletteIndex = getColourMap(colours[1].colour).midLight;
            GfxClear(rt, paletteIndex);

            auto* musicObj = ride->getMusicObject();
            if (musicObj == nullptr)
                return;

            auto y = 0;

            for (size_t i = 0; i < musicObj->GetTrackCount(); i++)
            {
                // Skip invisible items
                if (y + kScrollableRowHeight < rt.y || y > rt.y + rt.height)
                {
                    y += kScrollableRowHeight;
                    continue;
                }

                // Skip empty items
                const auto* track = musicObj->GetTrack(i);
                if (track->Name.empty())
                    continue;

                // Prepare items for display
                auto ft = Formatter();
                ft.Add<const char*>(track->Name.c_str());
                ft.Add<const char*>(track->Composer.c_str());

                // Do we have composer info to show?
                auto stringId = track->Composer.empty() ? STR_MUSIC_OBJECT_TRACK_LIST_ITEM
                                                        : STR_MUSIC_OBJECT_TRACK_LIST_ITEM_WITH_COMPOSER;

                // Draw the track
                DrawTextBasic(rt, { 0, y }, stringId, ft, { FontStyle::small });
                y += kScrollableRowHeight;
            }
        }

#pragma endregion

#pragma region Measurements

        static constexpr StringId GetRatingName(RideRating_t rating)
        {
            int32_t index = std::clamp<int32_t>(rating >> 8, 0, static_cast<int32_t>(std::size(RatingNames)) - 1);
            return RatingNames[index];
        }

        void SetupScenerySelection()
        {
            if (gTrackDesignSaveMode)
            {
                CancelScenerySelection();
            }

            while (ToolSet(*this, WIDX_BACKGROUND, Tool::crosshair))
                ;

            gTrackDesignSaveRideIndex = rideId;

            TrackDesignSaveInit();
            gGamePaused |= GAME_PAUSED_SAVING_TRACK;
            gTrackDesignSaveMode = true;

            Audio::StopAll();

            WindowBase* w_main = WindowGetMain();
            if (w_main != nullptr)
            {
                w_main->viewport->flags |= (VIEWPORT_FLAG_HIDE_VERTICAL | VIEWPORT_FLAG_HIDE_BASE);
            }

            GfxInvalidateScreen();
        }

        void MeasurementsDesignReset()
        {
            TrackDesignSaveResetScenery();
        }

        void MeasurementsDesignSelectNearbyScenery()
        {
            TrackDesignSaveSelectNearbyScenery(gTrackDesignSaveRideIndex);
        }

        void MeasurementsDesignCancel()
        {
            if (gTrackDesignSaveMode)
            {
                CancelScenerySelection();
            }
        }

        static void TrackDesignCallback(ModalResult result, [[maybe_unused]] const utf8* path)
        {
            if (result == ModalResult::ok)
            {
                TrackRepositoryScan();
            }
            GfxInvalidateScreen();
        };

        void MeasurementsDesignSave()
        {
            TrackDesignState tds{};

            Ride* ride = GetRide(rideId);
            _trackDesign = ride->saveToTrackDesign(tds);
            if (!_trackDesign)
            {
                return;
            }

            if (gTrackDesignSaveMode)
            {
                auto errMessage = _trackDesign->CreateTrackDesignScenery(tds);
                if (!errMessage.Successful)
                {
                    ContextShowError(STR_CANT_SAVE_TRACK_DESIGN, errMessage.Message, {});
                    return;
                }
                if (errMessage.HasMessage())
                {
                    ContextShowError(errMessage.Message, kStringIdEmpty, {});
                }
            }

            auto trackName = ride->getName();
            auto intent = Intent(WindowClass::loadsave);
            intent.PutEnumExtra<LoadSaveAction>(INTENT_EXTRA_LOADSAVE_ACTION, LoadSaveAction::save);
            intent.PutEnumExtra<LoadSaveType>(INTENT_EXTRA_LOADSAVE_TYPE, LoadSaveType::track);
            intent.PutExtra(INTENT_EXTRA_TRACK_DESIGN, _trackDesign.get());
            intent.PutExtra(INTENT_EXTRA_PATH, trackName);
            intent.PutExtra(INTENT_EXTRA_CALLBACK, reinterpret_cast<CloseCallback>(&TrackDesignCallback));

            ContextOpenIntent(&intent);
        }

        void MeasurementsClose()
        {
            MeasurementsDesignCancel();
        }

        void MeasurementsOnMouseUp(WidgetIndex widgetIndex)
        {
            switch (widgetIndex)
            {
                case WIDX_CLOSE:
                    close();
                    return;
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
                    setPage(widgetIndex - WIDX_TAB_1);
                    break;
                case WIDX_SELECT_NEARBY_SCENERY:
                    MeasurementsDesignSelectNearbyScenery();
                    break;
                case WIDX_RESET_SELECTION:
                    MeasurementsDesignReset();
                    break;
                case WIDX_SAVE_DESIGN:
                    MeasurementsDesignSave();
                    break;
                case WIDX_CANCEL_DESIGN:
                    MeasurementsDesignCancel();
                    break;
            }
        }

        void MeasurementsResize()
        {
            WindowSetResize(*this, { kMinimumWindowWidth, 234 }, { kMinimumWindowWidth, 234 });
        }

        void MeasurementsOnMouseDown(WidgetIndex widgetIndex)
        {
            if (widgetIndex != WIDX_SAVE_TRACK_DESIGN)
                return;

            auto ride = GetRide(rideId);
            if (ride == nullptr)
                return;

            gDropdown.items[0] = Dropdown::PlainMenuLabel(STR_SAVE_TRACK_DESIGN_ITEM);
            gDropdown.items[1] = Dropdown::PlainMenuLabel(STR_SAVE_TRACK_DESIGN_WITH_SCENERY_ITEM);

            WindowDropdownShowText(
                { windowPos.x + widgets[widgetIndex].left, windowPos.y + widgets[widgetIndex].top },
                widgets[widgetIndex].height(), colours[1], Dropdown::Flag::StayOpen, 2);
            gDropdown.defaultIndex = 0;
            if (!ride->getRideTypeDescriptor().HasFlag(RtdFlag::hasTrack))
            {
                // Disable saving without scenery if we're a flat ride
                gDropdown.items[0].setDisabled(true);
                gDropdown.defaultIndex = 1;
            }
        }

        void MeasurementsOnDropdown(WidgetIndex widgetIndex, int32_t dropdownIndex)
        {
            if (widgetIndex != WIDX_SAVE_TRACK_DESIGN)
                return;

            if (dropdownIndex == -1)
                dropdownIndex = gDropdown.highlightedIndex;

            if (dropdownIndex == 0)
            {
                MeasurementsDesignSave();
            }
            else
                SetupScenerySelection();
        }

        void MeasurementsUpdate()
        {
            currentFrame++;
            onPrepareDraw();
            invalidateWidget(WIDX_TAB_7);

            Ride* const ride = GetRide(rideId);
            if (ride && ride->windowInvalidateFlags.has(RideInvalidateFlag::ratings))
            {
                invalidate();
                ride->windowInvalidateFlags.unset(RideInvalidateFlag::ratings);
            }
        }

        void MeasurementsOnToolDown(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords)
        {
            _lastSceneryX = screenCoords.x;
            _lastSceneryY = screenCoords.y;
            _collectTrackDesignScenery = true; // Default to true in case user does not select anything valid

            constexpr auto interactionFlags = EnumsToFlags(
                ViewportInteractionItem::scenery, ViewportInteractionItem::footpath, ViewportInteractionItem::wall,
                ViewportInteractionItem::largeScenery);
            auto info = GetMapCoordinatesFromPos(screenCoords, interactionFlags);
            switch (info.interactionType)
            {
                case ViewportInteractionItem::scenery:
                case ViewportInteractionItem::largeScenery:
                case ViewportInteractionItem::wall:
                case ViewportInteractionItem::footpath:
                    _collectTrackDesignScenery = !TrackDesignSaveContainsTileElement(info.Element);
                    TrackDesignSaveSelectTileElement(info.interactionType, info.Loc, info.Element, _collectTrackDesignScenery);
                    break;
                default:
                    break;
            }
        }

        void MeasurementsOnToolDrag(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords)
        {
            if (screenCoords.x == _lastSceneryX && screenCoords.y == _lastSceneryY)
                return;
            _lastSceneryX = screenCoords.x;
            _lastSceneryY = screenCoords.y;

            constexpr auto interactionFlags = EnumsToFlags(
                ViewportInteractionItem::scenery, ViewportInteractionItem::footpath, ViewportInteractionItem::wall,
                ViewportInteractionItem::largeScenery);
            auto info = GetMapCoordinatesFromPos(screenCoords, interactionFlags);
            switch (info.interactionType)
            {
                case ViewportInteractionItem::scenery:
                case ViewportInteractionItem::largeScenery:
                case ViewportInteractionItem::wall:
                case ViewportInteractionItem::footpath:
                    TrackDesignSaveSelectTileElement(info.interactionType, info.Loc, info.Element, _collectTrackDesignScenery);
                    break;
                default:
                    break;
            }
        }

        void MeasurementsOnToolAbort(WidgetIndex widgetIndex)
        {
            MeasurementsDesignCancel();
        }

        void MeasurementsOnPrepareDraw()
        {
            SetPressedTab();

            auto ride = GetRide(rideId);
            if (ride == nullptr)
                return;

            auto ft = Formatter::Common();
            ride->formatNameTo(ft);

            widgets[WIDX_SAVE_TRACK_DESIGN].tooltip = STR_SAVE_TRACK_DESIGN_NOT_POSSIBLE;
            widgets[WIDX_SAVE_TRACK_DESIGN].type = WidgetType::empty;
            if (gTrackDesignSaveMode && gTrackDesignSaveRideIndex == rideId)
            {
                widgets[WIDX_SELECT_NEARBY_SCENERY].type = WidgetType::button;
                widgets[WIDX_RESET_SELECTION].type = WidgetType::button;
                widgets[WIDX_SAVE_DESIGN].type = WidgetType::button;
                widgets[WIDX_CANCEL_DESIGN].type = WidgetType::button;
            }
            else
            {
                widgets[WIDX_SELECT_NEARBY_SCENERY].type = WidgetType::empty;
                widgets[WIDX_RESET_SELECTION].type = WidgetType::empty;
                widgets[WIDX_SAVE_DESIGN].type = WidgetType::empty;
                widgets[WIDX_CANCEL_DESIGN].type = WidgetType::empty;

                widgets[WIDX_SAVE_TRACK_DESIGN].type = WidgetType::flatBtn;
                disabledWidgets |= (1uLL << WIDX_SAVE_TRACK_DESIGN);
                if (ride->lifecycleFlags & RIDE_LIFECYCLE_TESTED)
                {
                    if (!ride->ratings.isNull())
                    {
                        disabledWidgets &= ~(1uLL << WIDX_SAVE_TRACK_DESIGN);
                        widgets[WIDX_SAVE_TRACK_DESIGN].tooltip = STR_SAVE_TRACK_DESIGN;
                    }
                }
            }

            WindowAlignTabs(this, WIDX_TAB_1, WIDX_TAB_10);
        }

        void MeasurementsOnDraw(RenderTarget& rt)
        {
            drawWidgets(rt);
            drawTabImages(rt);

            if (widgets[WIDX_SAVE_DESIGN].type == WidgetType::button)
            {
                Widget* widget = &widgets[WIDX_PAGE_BACKGROUND];

                ScreenCoordsXY widgetCoords(windowPos.x + widget->midX(), windowPos.y + widget->top + 40);
                DrawTextWrapped(
                    rt, widgetCoords, width - 8, STR_CLICK_ITEMS_OF_SCENERY_TO_SELECT, {}, { TextAlignment::centre });

                widgetCoords.x = windowPos.x + 4;
                widgetCoords.y = windowPos.y + widgets[WIDX_SELECT_NEARBY_SCENERY].bottom + 17;
                Rectangle::fillInset(
                    rt, { widgetCoords, { windowPos.x + 312, widgetCoords.y + 1 } }, colours[1], Rectangle::BorderStyle::inset);
            }
            else
            {
                auto ride = GetRide(rideId);
                if (ride == nullptr)
                    return;

                auto screenCoords = windowPos
                    + ScreenCoordsXY{ widgets[WIDX_PAGE_BACKGROUND].left + 4, widgets[WIDX_PAGE_BACKGROUND].top + 4 };

                if (ride->lifecycleFlags & RIDE_LIFECYCLE_TESTED)
                {
                    // Excitement
                    StringId ratingName = GetRatingName(ride->ratings.excitement);
                    auto ft = Formatter();
                    ft.Add<uint32_t>(ride->ratings.excitement);
                    ft.Add<StringId>(ratingName);
                    StringId stringId = !RideHasRatings(*ride) ? STR_EXCITEMENT_RATING_NOT_YET_AVAILABLE
                                                               : STR_EXCITEMENT_RATING;
                    DrawTextBasic(rt, screenCoords, stringId, ft);
                    screenCoords.y += kListRowHeight;

                    // Intensity
                    ratingName = GetRatingName(ride->ratings.intensity);
                    ft = Formatter();
                    ft.Add<uint32_t>(ride->ratings.intensity);
                    ft.Add<StringId>(ratingName);

                    stringId = STR_INTENSITY_RATING;
                    if (!RideHasRatings(*ride))
                        stringId = STR_INTENSITY_RATING_NOT_YET_AVAILABLE;
                    else if (ride->ratings.intensity >= RideRating::make(10, 00))
                        stringId = STR_INTENSITY_RATING_RED;

                    DrawTextBasic(rt, screenCoords, stringId, ft);
                    screenCoords.y += kListRowHeight;

                    // Nausea
                    ratingName = GetRatingName(ride->ratings.nausea);
                    ft = Formatter();
                    ft.Add<uint32_t>(ride->ratings.nausea);
                    ft.Add<StringId>(ratingName);
                    stringId = !RideHasRatings(*ride) ? STR_NAUSEA_RATING_NOT_YET_AVAILABLE : STR_NAUSEA_RATING;
                    DrawTextBasic(rt, screenCoords, stringId, ft);
                    screenCoords.y += 2 * kListRowHeight;

                    // Horizontal rule
                    Rectangle::fillInset(
                        rt, { screenCoords - ScreenCoordsXY{ 0, 6 }, screenCoords + ScreenCoordsXY{ 303, -5 } }, colours[1],
                        Rectangle::BorderStyle::inset);

                    if (!(ride->lifecycleFlags & RIDE_LIFECYCLE_NO_RAW_STATS))
                    {
                        if (ride->getRideTypeDescriptor().specialType == RtdSpecialType::miniGolf)
                        {
                            // Holes
                            ft = Formatter();
                            ft.Add<uint16_t>(ride->numHoles);
                            DrawTextBasic(rt, screenCoords, STR_HOLES, ft);
                            screenCoords.y += kListRowHeight;
                        }
                        else
                        {
                            // Max speed
                            ft = Formatter();
                            ft.Add<int32_t>(ToHumanReadableSpeed(ride->maxSpeed));
                            DrawTextBasic(rt, screenCoords, STR_MAX_SPEED, ft);
                            screenCoords.y += kListRowHeight;

                            // Average speed
                            ft = Formatter();
                            ft.Add<int32_t>(ToHumanReadableSpeed(ride->averageSpeed));
                            DrawTextBasic(rt, screenCoords, STR_AVERAGE_SPEED, ft);
                            screenCoords.y += kListRowHeight;

                            // Ride time
                            ft = Formatter();
                            int32_t numTimes = 0;
                            // TODO: STR_RIDE_TIME only takes up to 4 stations modify to take more
                            // also if modified may need to be split into multiple format strings
                            // as formatter cannot take more than 256 bytes
                            for (int32_t i = 0; i < std::min<int32_t>(ride->numStations, 4); i++)
                            {
                                StationIndex stationIndex = StationIndex::FromUnderlying(numTimes);
                                auto time = ride->getStation(stationIndex).SegmentTime;
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
                            DrawTextEllipsised(rt, screenCoords, 308, STR_RIDE_TIME, ft);
                            screenCoords.y += kListRowHeight;
                        }

                        // Ride length
                        ft = Formatter();
                        int32_t numLengths = 0;
                        // TODO: see above STR_RIDE_LENGTH is also only able to display max 4
                        for (int32_t i = 0; i < std::min<int32_t>(ride->numStations, 4); i++)
                        {
                            StationIndex stationIndex = StationIndex::FromUnderlying(i);
                            auto length = ride->getStation(stationIndex).SegmentLength;
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
                        DrawTextEllipsised(rt, screenCoords, 308, STR_RIDE_LENGTH, ft);

                        screenCoords.y += kListRowHeight;

                        if (ride->getRideTypeDescriptor().HasFlag(RtdFlag::hasGForces))
                        {
                            // Max. positive vertical G's
                            stringId = STR_MAX_POSITIVE_VERTICAL_G;

                            ft = Formatter();
                            ft.Add<fixed16_2dp>(ride->maxPositiveVerticalG);
                            DrawTextBasic(rt, screenCoords, stringId, ft);
                            screenCoords.y += kListRowHeight;

                            // Max. negative vertical G's
                            stringId = ride->maxNegativeVerticalG <= kRideGForcesRedNegVertical
                                ? STR_MAX_NEGATIVE_VERTICAL_G_RED
                                : STR_MAX_NEGATIVE_VERTICAL_G;
                            ft = Formatter();
                            ft.Add<int32_t>(ride->maxNegativeVerticalG);
                            DrawTextBasic(rt, screenCoords, stringId, ft);
                            screenCoords.y += kListRowHeight;

                            // Max lateral G's
                            stringId = ride->maxLateralG > kRideGForcesRedLateral ? STR_MAX_LATERAL_G_RED : STR_MAX_LATERAL_G;
                            ft = Formatter();
                            ft.Add<fixed16_2dp>(ride->maxLateralG);
                            DrawTextBasic(rt, screenCoords, stringId, ft);
                            screenCoords.y += kListRowHeight;

                            // Total 'air' time
                            ft = Formatter();
                            ft.Add<fixed32_2dp>(ToHumanReadableAirTime(ride->totalAirTime));
                            DrawTextBasic(rt, screenCoords, STR_TOTAL_AIR_TIME, ft);
                            screenCoords.y += kListRowHeight;
                        }

                        if (ride->getRideTypeDescriptor().HasFlag(RtdFlag::hasDrops))
                        {
                            ft = Formatter();
                            ft.Add<uint16_t>(ride->numDrops);
                            DrawTextBasic(rt, screenCoords, STR_DROPS, ft);
                            screenCoords.y += kListRowHeight;

                            // Highest drop height
                            auto highestDropHeight = (ride->highestDropHeight * 3) / 4;
                            ft = Formatter();
                            ft.Add<int32_t>(highestDropHeight);
                            DrawTextBasic(rt, screenCoords, STR_HIGHEST_DROP_HEIGHT, ft);
                            screenCoords.y += kListRowHeight;
                        }

                        if (ride->getRideTypeDescriptor().specialType != RtdSpecialType::miniGolf)
                        {
                            // Inversions
                            if (ride->numInversions != 0)
                            {
                                ft = Formatter();
                                ft.Add<uint16_t>(ride->numInversions);
                                DrawTextBasic(rt, screenCoords, STR_INVERSIONS, ft);
                                screenCoords.y += kListRowHeight;
                            }
                        }
                    }
                }
                else
                {
                    DrawTextBasic(rt, screenCoords, STR_NO_TEST_RESULTS_YET);
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

        void SetGraph(int32_t type)
        {
            if (listInformationType == type)
            {
                _autoScrollGraph = !_autoScrollGraph;
            }
            else
            {
                listInformationType = type;
            }
            invalidate();
        }

        void GraphsOnMouseUp(WidgetIndex widgetIndex)
        {
            switch (widgetIndex)
            {
                case WIDX_CLOSE:
                    close();
                    return;
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
                    setPage(widgetIndex - WIDX_TAB_1);
                    break;
            }
        }

        void GraphsResize()
        {
            WindowSetResize(*this, { kMinimumWindowWidth, 182 }, { kMaxWindowSize.width, 450 });
        }

        void GraphsOnMouseDown(WidgetIndex widgetIndex)
        {
            switch (widgetIndex)
            {
                case WIDX_GRAPH_VELOCITY:
                    SetGraph(GRAPH_VELOCITY);
                    break;
                case WIDX_GRAPH_ALTITUDE:
                    SetGraph(GRAPH_ALTITUDE);
                    break;
                case WIDX_GRAPH_VERTICAL:
                    SetGraph(GRAPH_VERTICAL);
                    break;
                case WIDX_GRAPH_LATERAL:
                    SetGraph(GRAPH_LATERAL);
                    break;
            }
        }

        void GraphsUpdate()
        {
            Widget* widget;
            int32_t x;

            currentFrame++;
            onPrepareDraw();
            invalidateWidget(WIDX_TAB_8);
            onPrepareDraw();
            invalidateWidget(WIDX_GRAPH);

            widget = &widgets[WIDX_GRAPH];
            x = scrolls[0].contentOffsetX;
            if (_autoScrollGraph)
            {
                auto ride = GetRide(rideId);
                if (ride != nullptr)
                {
                    RideMeasurement* measurement{};
                    std::tie(measurement, std::ignore) = ride->getMeasurement();
                    x = measurement == nullptr ? 0 : measurement->current_item - (((widget->width() - 1) / 4) * 3);
                }
            }

            scrolls[0].contentOffsetX = std::clamp(x, 0, scrolls[0].contentWidth - (widget->width() - 3));
            widgetScrollUpdateThumbs(*this, WIDX_GRAPH);
        }

        ScreenSize GraphsScrollGetSize(int32_t scrollIndex)
        {
            onPrepareDraw();

            ScreenSize size{};
            // Set minimum size
            size.width = widgets[WIDX_GRAPH].width() - 3;

            // Get measurement size
            auto ride = GetRide(rideId);
            if (ride != nullptr)
            {
                RideMeasurement* measurement{};
                std::tie(measurement, std::ignore) = ride->getMeasurement();
                if (measurement != nullptr)
                {
                    size.width = std::max<int32_t>(size.width, measurement->num_items);
                }
            }
            return size;
        }

        void GraphsOnScrollSelect(int32_t scrollIndex, int32_t scrollAreaType)
        {
            _autoScrollGraph = false;
        }

        OpenRCT2String GraphsTooltip(const WidgetIndex widgetIndex, const StringId fallback)
        {
            if (widgetIndex == WIDX_GRAPH)
            {
                auto ride = GetRide(rideId);
                if (ride != nullptr)
                {
                    auto [measurement, message] = ride->getMeasurement();
                    if (measurement != nullptr && measurement->flags.has(RideMeasurementFlag::running))
                    {
                        auto ft = Formatter();
                        ft.Increment(2);
                        ft.Add<StringId>(GetRideComponentName(ride->getRideTypeDescriptor().NameConvention.vehicle).number);
                        ft.Add<uint16_t>(measurement->vehicle_index + 1);
                        return { fallback, ft };
                    }

                    return message;
                }
            }
            else
            {
                return { kStringIdNone, {} };
            }
            return { fallback, {} };
        }

        void GraphsOnPrepareDraw()
        {
            SetPressedTab();

            auto ride = GetRide(rideId);
            if (ride == nullptr)
                return;

            auto ft = Formatter::Common();
            ride->formatNameTo(ft);

            // Set pressed graph button type
            pressedWidgets &= ~(1uLL << WIDX_GRAPH_VELOCITY);
            pressedWidgets &= ~(1uLL << WIDX_GRAPH_ALTITUDE);
            pressedWidgets &= ~(1uLL << WIDX_GRAPH_VERTICAL);
            pressedWidgets &= ~(1uLL << WIDX_GRAPH_LATERAL);
            pressedWidgets |= (1LL << (WIDX_GRAPH_VELOCITY + listInformationType));

            // Hide graph buttons that are not applicable
            if (ride->getRideTypeDescriptor().HasFlag(RtdFlag::hasGForces))
            {
                widgets[WIDX_GRAPH_VERTICAL].type = WidgetType::button;
                widgets[WIDX_GRAPH_LATERAL].type = WidgetType::button;
            }
            else
            {
                widgets[WIDX_GRAPH_VERTICAL].type = WidgetType::empty;
                widgets[WIDX_GRAPH_LATERAL].type = WidgetType::empty;
            }

            // Anchor graph widget
            auto x = width - 4;
            auto y = height - kButtonFaceHeight - 8;

            widgets[WIDX_GRAPH].right = x;
            widgets[WIDX_GRAPH].bottom = y;
            y += 3;
            widgets[WIDX_GRAPH_VELOCITY].top = y;
            widgets[WIDX_GRAPH_ALTITUDE].top = y;
            widgets[WIDX_GRAPH_VERTICAL].top = y;
            widgets[WIDX_GRAPH_LATERAL].top = y;
            y += kButtonFaceHeight + 1;
            widgets[WIDX_GRAPH_VELOCITY].bottom = y;
            widgets[WIDX_GRAPH_ALTITUDE].bottom = y;
            widgets[WIDX_GRAPH_VERTICAL].bottom = y;
            widgets[WIDX_GRAPH_LATERAL].bottom = y;

            WindowAlignTabs(this, WIDX_TAB_1, WIDX_TAB_10);
        }

        void GraphsOnDraw(RenderTarget& rt)
        {
            drawWidgets(rt);
            drawTabImages(rt);
        }

        void GraphsOnScrollDraw(RenderTarget& rt, int32_t scrollIndex)
        {
            GfxClear(rt, getColourMap(Drawing::Colour::saturatedGreen).darker);

            auto widget = &widgets[WIDX_GRAPH];
            auto ride = GetRide(rideId);
            if (ride == nullptr)
            {
                return;
            }

            auto [measurement, message] = ride->getMeasurement();

            if (measurement == nullptr)
            {
                // No measurement message
                ScreenCoordsXY stringCoords(widget->width() / 2, widget->height() - 1 / 2 - 5);
                int32_t txtWidth = widget->width() - 3;
                DrawTextWrapped(rt, stringCoords, txtWidth, message.str, message.args, { TextAlignment::centre });
                return;
            }

            // Vertical grid lines
            const auto lightColour = getColourMap(Drawing::Colour::saturatedGreen).midLight;
            const auto darkColour = getColourMap(Drawing::Colour::saturatedGreen).midDark;

            int32_t time = 0;
            for (int32_t x = 0; x < rt.x + rt.width; x += 80)
            {
                if (x + 80 >= rt.x)
                {
                    auto coord1 = ScreenCoordsXY{ x, rt.y };
                    auto coord2 = ScreenCoordsXY{ x, rt.y + rt.height - 1 };
                    Rectangle::fill(rt, { coord1, coord2 }, lightColour);
                    Rectangle::fill(rt, { coord1 + ScreenCoordsXY{ 16, 0 }, coord2 + ScreenCoordsXY{ 16, 0 } }, darkColour);
                    Rectangle::fill(rt, { coord1 + ScreenCoordsXY{ 32, 0 }, coord2 + ScreenCoordsXY{ 32, 0 } }, darkColour);
                    Rectangle::fill(rt, { coord1 + ScreenCoordsXY{ 48, 0 }, coord2 + ScreenCoordsXY{ 48, 0 } }, darkColour);
                    Rectangle::fill(rt, { coord1 + ScreenCoordsXY{ 64, 0 }, coord2 + ScreenCoordsXY{ 64, 0 } }, darkColour);
                }
                time += 5;
            }

            // Horizontal grid lines
            int32_t listType = listInformationType;
            int16_t yUnit = GraphsYAxisDetails[listType].unit;
            StringId stringID = GraphsYAxisDetails[listType].label;
            int16_t yUnitInterval = GraphsYAxisDetails[listType].unit_interval;
            int16_t yInterval = GraphsYAxisDetails[listType].interval;

            // Scale modifier
            if (listType == GRAPH_ALTITUDE)
            {
                yUnit -= kMapBaseZ * 3;
            }

            for (int32_t y = widget->height() - 14; y >= 8; y -= yInterval, yUnit += yUnitInterval)
            {
                // Minor / major line
                auto colour = yUnit == 0 ? lightColour : darkColour;
                Rectangle::fill(rt, { { rt.x, y }, { rt.x + rt.width - 1, y } }, colour);

                int16_t scaled_yUnit = yUnit;
                // Scale modifier
                if (listType == GRAPH_ALTITUDE)
                    scaled_yUnit /= 2;

                auto ft = Formatter();
                ft.Add<int16_t>(scaled_yUnit);

                DrawTextBasic(rt, { scrolls[0].contentOffsetX + 1, y - 4 }, stringID, ft, { FontStyle::small });
            }

            // Time marks
            time = 0;
            for (int32_t x = 0; x < rt.x + rt.width; x += 80)
            {
                auto ft = Formatter();
                ft.Add<int32_t>(time);
                if (x + 80 >= rt.x)
                    DrawTextBasic(rt, { x + 2, 1 }, STR_RIDE_STATS_TIME, ft, { FontStyle::small });
                time += 5;
            }

            // Plot
            int32_t x = rt.x;
            int32_t firstPoint, secondPoint;
            // Uses the force limits (used to draw extreme G's in red on measurement tab) to determine if line should be drawn
            // red.
            int32_t intensityThresholdPositive = 0;
            int32_t intensityThresholdNegative = 0;
            for (int32_t graphWidth = 0; graphWidth < rt.width; graphWidth++, x++)
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
                        intensityThresholdNegative = (kRideGForcesRedNegVertical / 8) + VerticalGraphHeightOffset;
                        break;
                    case GRAPH_LATERAL:
                        firstPoint = measurement->lateral[x] + LateralGraphHeightOffset;
                        secondPoint = measurement->lateral[x + 1] + LateralGraphHeightOffset;
                        intensityThresholdPositive = (kRideGForcesRedLateral / 8) + LateralGraphHeightOffset;
                        intensityThresholdNegative = -(kRideGForcesRedLateral / 8) + LateralGraphHeightOffset;
                        break;
                    default:
                        LOG_ERROR("Wrong graph type %d", listType);
                        firstPoint = secondPoint = 0;
                        break;
                }

                // Adjust line to match graph widget position.
                firstPoint = widget->height() - firstPoint - 14;
                secondPoint = widget->height() - secondPoint - 14;
                if (firstPoint > secondPoint)
                {
                    std::swap(firstPoint, secondPoint);
                }

                // Adjust threshold line position as well
                if (listType == GRAPH_VERTICAL || listType == GRAPH_LATERAL)
                {
                    intensityThresholdPositive = widget->height() - intensityThresholdPositive - 14;
                    intensityThresholdNegative = widget->height() - intensityThresholdNegative - 14;
                }

                const bool previousMeasurement = x > measurement->current_item;

                // Draw the current line in grey.
                Rectangle::fill(
                    rt, { { x, firstPoint }, { x, secondPoint } },
                    previousMeasurement ? PaletteIndex::pi17 : PaletteIndex::pi21);

                // Draw red over extreme values (if supported by graph type).
                if (listType == GRAPH_VERTICAL || listType == GRAPH_LATERAL)
                {
                    const auto redLineColour = previousMeasurement ? PaletteIndex::pi171 : PaletteIndex::pi173;

                    // Line exceeds negative threshold (at bottom of graph).
                    if (secondPoint >= intensityThresholdNegative)
                    {
                        const auto redLineTop = ScreenCoordsXY{ x, std::max(firstPoint, intensityThresholdNegative) };
                        const auto redLineBottom = ScreenCoordsXY{ x, std::max(secondPoint, intensityThresholdNegative) };
                        Rectangle::fill(rt, { redLineTop, redLineBottom }, redLineColour);
                    }

                    // Line exceeds positive threshold (at top of graph).
                    if (listType == GRAPH_LATERAL && firstPoint < intensityThresholdPositive)
                    {
                        const auto redLineTop = ScreenCoordsXY{ x, std::min(firstPoint, intensityThresholdPositive) };
                        const auto redLineBottom = ScreenCoordsXY{ x, std::min(secondPoint, intensityThresholdPositive) };
                        Rectangle::fill(rt, { redLineTop, redLineBottom }, redLineColour);
                    }
                }
            }
        }

#pragma endregion

#pragma region Income

        static void UpdateSamePriceThroughoutFlags(ShopItem shop_item)
        {
            const auto& gameState = getGameState();
            const auto existingFlags = gameState.park.samePriceThroughoutPark;

            auto newFlags = existingFlags;
            if (GetShopItemDescriptor(shop_item).IsPhoto())
            {
                if (existingFlags & EnumToFlag(shop_item))
                    newFlags &= ~EnumsToFlags(ShopItem::photo, ShopItem::photo2, ShopItem::photo3, ShopItem::photo4);
                else
                    newFlags |= EnumsToFlags(ShopItem::photo, ShopItem::photo2, ShopItem::photo3, ShopItem::photo4);
            }
            else
            {
                newFlags ^= EnumToFlag(shop_item);
            }

            auto parkSetParameter = GameActions::ParkSetParameterAction(GameActions::ParkParameter::SamePriceInPark, newFlags);
            GameActions::Execute(&parkSetParameter, getGameState());
        }

        void IncomeTogglePrimaryPrice()
        {
            auto ride = GetRide(rideId);
            if (ride == nullptr)
                return;

            ShopItem shopItem;
            const auto& rtd = ride->getRideTypeDescriptor();
            if (rtd.specialType == RtdSpecialType::toilet)
            {
                shopItem = ShopItem::admission;
            }
            else
            {
                auto rideEntry = GetRideEntryByIndex(ride->subtype);
                if (rideEntry != nullptr)
                {
                    shopItem = rideEntry->shop_item[0];
                    if (shopItem == ShopItem::none)
                        return;
                }
                else
                {
                    return;
                }
            }

            UpdateSamePriceThroughoutFlags(shopItem);

            auto rideSetPriceAction = GameActions::RideSetPriceAction(rideId, ride->price[0], true);
            GameActions::Execute(&rideSetPriceAction, getGameState());
        }

        void IncomeToggleSecondaryPrice()
        {
            auto ride = GetRide(rideId);
            if (ride == nullptr)
                return;

            auto rideEntry = GetRideEntryByIndex(ride->subtype);
            if (rideEntry == nullptr)
                return;

            auto shop_item = rideEntry->shop_item[1];
            if (shop_item == ShopItem::none)
                shop_item = ride->getRideTypeDescriptor().PhotoItem;

            UpdateSamePriceThroughoutFlags(shop_item);

            auto rideSetPriceAction = GameActions::RideSetPriceAction(rideId, ride->price[1], false);
            GameActions::Execute(&rideSetPriceAction, getGameState());
        }

        void IncomeSetPrimaryPrice(money64 price)
        {
            auto rideSetPriceAction = GameActions::RideSetPriceAction(rideId, price, true);
            GameActions::Execute(&rideSetPriceAction, getGameState());
        }

        void IncomeIncreasePrimaryPrice()
        {
            if (!IncomeCanModifyPrimaryPrice())
                return;

            auto ride = GetRide(rideId);
            if (ride == nullptr)
                return;

            auto price = ride->price[0];
            if (price < kRideMaxPrice)
                price++;

            IncomeSetPrimaryPrice(price);
        }

        void IncomeDecreasePrimaryPrice()
        {
            if (!IncomeCanModifyPrimaryPrice())
                return;

            auto ride = GetRide(rideId);
            if (ride == nullptr)
                return;

            auto price = ride->price[0];
            if (price > kRideMinPrice)
                price--;

            IncomeSetPrimaryPrice(price);
        }

        money64 IncomeGetSecondaryPrice()
        {
            auto ride = GetRide(rideId);
            if (ride == nullptr)
                return 0;

            return ride->price[1];
        }

        void IncomeSetSecondaryPrice(money64 price)
        {
            auto rideSetPriceAction = GameActions::RideSetPriceAction(rideId, price, false);
            GameActions::Execute(&rideSetPriceAction, getGameState());
        }

        bool IncomeCanModifyPrimaryPrice()
        {
            auto ride = GetRide(rideId);
            if (ride == nullptr)
                return false;

            auto rideEntry = ride->getRideEntry();
            const auto& rtd = ride->getRideTypeDescriptor();
            return Park::RidePricesUnlocked() || rtd.specialType == RtdSpecialType::toilet
                || (rideEntry != nullptr && rideEntry->shop_item[0] != ShopItem::none);
        }

        void IncomeIncreaseSecondaryPrice()
        {
            auto price = IncomeGetSecondaryPrice();

            if (price < kRideMaxPrice)
                price++;

            IncomeSetSecondaryPrice(price);
        }

        void IncomeDecreaseSecondaryPrice()
        {
            auto price = IncomeGetSecondaryPrice();

            if (price > 0.00_GBP)
                price--;

            IncomeSetSecondaryPrice(price);
        }

        void IncomeOnMouseUp(WidgetIndex widgetIndex)
        {
            utf8 _moneyInputText[kMoneyStringMaxlength] = {};

            switch (widgetIndex)
            {
                case WIDX_CLOSE:
                    close();
                    return;
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
                    setPage(widgetIndex - WIDX_TAB_1);
                    break;
                case WIDX_PRIMARY_PRICE:
                {
                    if (!IncomeCanModifyPrimaryPrice())
                        return;

                    auto ride = GetRide(rideId);
                    if (ride != nullptr)
                    {
                        MoneyToString(ride->price[0], _moneyInputText, kMoneyStringMaxlength, true);
                        WindowTextInputRawOpen(
                            this, WIDX_PRIMARY_PRICE, STR_ENTER_NEW_VALUE, STR_ENTER_NEW_VALUE, {}, _moneyInputText,
                            kMoneyStringMaxlength);
                    }
                    break;
                }
                case WIDX_PRIMARY_PRICE_SAME_THROUGHOUT_PARK:
                    IncomeTogglePrimaryPrice();
                    break;
                case WIDX_SECONDARY_PRICE:
                {
                    auto price64 = IncomeGetSecondaryPrice();

                    MoneyToString(price64, _moneyInputText, kMoneyStringMaxlength, true);
                    WindowTextInputRawOpen(
                        this, WIDX_SECONDARY_PRICE, STR_ENTER_NEW_VALUE, STR_ENTER_NEW_VALUE, {}, _moneyInputText,
                        kMoneyStringMaxlength);
                }
                break;
                case WIDX_SECONDARY_PRICE_SAME_THROUGHOUT_PARK:
                    IncomeToggleSecondaryPrice();
                    break;
            }
        }

        void IncomeResize()
        {
            auto newHeight = 194;
            WindowSetResize(*this, { kMinimumWindowWidth, newHeight }, { kMinimumWindowWidth, newHeight });
        }

        void IncomeOnMouseDown(WidgetIndex widgetIndex)
        {
            switch (widgetIndex)
            {
                case WIDX_PRIMARY_PRICE_INCREASE:
                    IncomeIncreasePrimaryPrice();
                    break;
                case WIDX_PRIMARY_PRICE_DECREASE:
                    IncomeDecreasePrimaryPrice();
                    break;
                case WIDX_SECONDARY_PRICE_INCREASE:
                    IncomeIncreaseSecondaryPrice();
                    break;
                case WIDX_SECONDARY_PRICE_DECREASE:
                    IncomeDecreaseSecondaryPrice();
                    break;
            }
        }

        void IncomeUpdate()
        {
            currentFrame++;
            onPrepareDraw();
            invalidateWidget(WIDX_TAB_9);

            auto ride = GetRide(rideId);
            if (ride != nullptr && ride->windowInvalidateFlags.has(RideInvalidateFlag::income))
            {
                ride->windowInvalidateFlags.unset(RideInvalidateFlag::income);
                invalidate();
            }
        }

        void IncomeOnTextInput(WidgetIndex widgetIndex, std::string_view text)
        {
            if ((widgetIndex != WIDX_PRIMARY_PRICE && widgetIndex != WIDX_SECONDARY_PRICE) || text.empty())
                return;

            std::string strText{ text };
            money64 price = StringToMoney(strText.c_str());
            if (price == kMoney64Undefined)
            {
                return;
            }

            price = std::clamp(price, kRideMinPrice, kRideMaxPrice);

            if (widgetIndex == WIDX_PRIMARY_PRICE)
            {
                IncomeSetPrimaryPrice(price);
            }
            else
            {
                IncomeSetSecondaryPrice(price);
            }
        }

        void IncomeOnPrepareDraw()
        {
            SetPressedTab();

            auto ride = GetRide(rideId);
            if (ride == nullptr)
                return;

            widgets[WIDX_TITLE].text = STR_ARG_18_STRINGID;

            auto ft = Formatter::Common();
            ft.Increment(18);
            ride->formatNameTo(ft);

            auto rideEntry = ride->getRideEntry();
            if (rideEntry == nullptr)
                return;

            // Primary item
            pressedWidgets &= ~(1uLL << WIDX_PRIMARY_PRICE_SAME_THROUGHOUT_PARK);
            disabledWidgets &= ~(1uLL << WIDX_PRIMARY_PRICE);

            widgets[WIDX_PRIMARY_PRICE_LABEL].tooltip = kStringIdNone;
            widgets[WIDX_PRIMARY_PRICE].tooltip = kStringIdNone;

            // If ride prices are locked, do not allow setting the price, unless we're dealing with a shop or toilet.
            const auto& rtd = ride->getRideTypeDescriptor();
            if (!Park::RidePricesUnlocked() && rideEntry->shop_item[0] == ShopItem::none
                && rtd.specialType != RtdSpecialType::toilet)
            {
                disabledWidgets |= (1uLL << WIDX_PRIMARY_PRICE);
                widgets[WIDX_PRIMARY_PRICE_LABEL].tooltip = STR_RIDE_INCOME_ADMISSION_PAY_FOR_ENTRY_TIP;
                widgets[WIDX_PRIMARY_PRICE].tooltip = STR_RIDE_INCOME_ADMISSION_PAY_FOR_ENTRY_TIP;
            }

            widgets[WIDX_PRIMARY_PRICE_LABEL].text = STR_RIDE_INCOME_ADMISSION_PRICE;
            widgets[WIDX_SECONDARY_PRICE_LABEL].text = STR_SHOP_ITEM_PRICE_LABEL_ON_RIDE_PHOTO;
            widgets[WIDX_PRIMARY_PRICE_SAME_THROUGHOUT_PARK].type = WidgetType::empty;

            widgets[WIDX_PRIMARY_PRICE].text = STR_BOTTOM_TOOLBAR_CASH;
            auto ridePrimaryPrice = RideGetPrice(*ride);
            ft.Rewind();
            ft.Add<money64>(ridePrimaryPrice);
            if (ridePrimaryPrice == 0)
                widgets[WIDX_PRIMARY_PRICE].text = STR_FREE;

            ShopItem primaryItem = ShopItem::admission;
            if (rtd.specialType == RtdSpecialType::toilet || ((primaryItem = rideEntry->shop_item[0]) != ShopItem::none))
            {
                widgets[WIDX_PRIMARY_PRICE_SAME_THROUGHOUT_PARK].type = WidgetType::checkbox;

                if (ShopItemHasCommonPrice(primaryItem))
                    pressedWidgets |= (1uLL << WIDX_PRIMARY_PRICE_SAME_THROUGHOUT_PARK);

                widgets[WIDX_PRIMARY_PRICE_LABEL].text = GetShopItemDescriptor(primaryItem).Naming.PriceLabel;
            }

            // Get secondary item
            auto secondaryItem = ride->getRideTypeDescriptor().PhotoItem;
            if (!(ride->lifecycleFlags & RIDE_LIFECYCLE_ON_RIDE_PHOTO))
            {
                if ((secondaryItem = rideEntry->shop_item[1]) != ShopItem::none)
                {
                    widgets[WIDX_SECONDARY_PRICE_LABEL].text = GetShopItemDescriptor(secondaryItem).Naming.PriceLabel;
                }
            }

            if (secondaryItem == ShopItem::none)
            {
                // Hide secondary item widgets
                widgets[WIDX_SECONDARY_PRICE_LABEL].type = WidgetType::empty;
                widgets[WIDX_SECONDARY_PRICE].type = WidgetType::empty;
                widgets[WIDX_SECONDARY_PRICE_INCREASE].type = WidgetType::empty;
                widgets[WIDX_SECONDARY_PRICE_DECREASE].type = WidgetType::empty;
                widgets[WIDX_SECONDARY_PRICE_SAME_THROUGHOUT_PARK].type = WidgetType::empty;
            }
            else
            {
                // Set same price throughout park checkbox
                pressedWidgets &= ~(1uLL << WIDX_SECONDARY_PRICE_SAME_THROUGHOUT_PARK);
                if (ShopItemHasCommonPrice(secondaryItem))
                    pressedWidgets |= (1uLL << WIDX_SECONDARY_PRICE_SAME_THROUGHOUT_PARK);

                // Show widgets
                widgets[WIDX_SECONDARY_PRICE_LABEL].type = WidgetType::label;
                widgets[WIDX_SECONDARY_PRICE].type = WidgetType::spinner;
                widgets[WIDX_SECONDARY_PRICE_INCREASE].type = WidgetType::button;
                widgets[WIDX_SECONDARY_PRICE_DECREASE].type = WidgetType::button;
                widgets[WIDX_SECONDARY_PRICE_SAME_THROUGHOUT_PARK].type = WidgetType::checkbox;

                // Set secondary item price
                widgets[WIDX_SECONDARY_PRICE].text = STR_RIDE_SECONDARY_PRICE_VALUE;
                ft.Rewind();
                ft.Increment(10);
                ft.Add<money64>(ride->price[1]);
                if (ride->price[1] == 0)
                    widgets[WIDX_SECONDARY_PRICE].text = STR_FREE;
            }

            WindowAlignTabs(this, WIDX_TAB_1, WIDX_TAB_10);
        }

        void IncomeOnDraw(RenderTarget& rt)
        {
            StringId stringId;
            money64 profit;
            ShopItem primaryItem, secondaryItem;

            drawWidgets(rt);
            drawTabImages(rt);

            auto ride = GetRide(rideId);
            if (ride == nullptr)
                return;

            auto rideEntry = ride->getRideEntry();
            if (rideEntry == nullptr)
                return;

            auto screenCoords = windowPos
                + ScreenCoordsXY{ widgets[WIDX_PAGE_BACKGROUND].left + 4, widgets[WIDX_PAGE_BACKGROUND].top + 33 };

            // Primary item profit / loss per item sold
            primaryItem = rideEntry->shop_item[0];
            if (primaryItem != ShopItem::none)
            {
                profit = ride->price[0];

                stringId = STR_PROFIT_PER_ITEM_SOLD;
                profit -= GetShopItemDescriptor(primaryItem).Cost;
                if (profit < 0)
                {
                    profit *= -1;
                    stringId = STR_LOSS_PER_ITEM_SOLD;
                }

                auto ft = Formatter();
                ft.Add<money64>(profit);

                DrawTextBasic(rt, screenCoords, stringId, ft);
            }
            screenCoords.y += 44;

            // Secondary item profit / loss per item sold
            secondaryItem = ride->getRideTypeDescriptor().PhotoItem;
            if (!(ride->lifecycleFlags & RIDE_LIFECYCLE_ON_RIDE_PHOTO))
                secondaryItem = rideEntry->shop_item[1];

            if (secondaryItem != ShopItem::none)
            {
                profit = ride->price[1];

                stringId = STR_PROFIT_PER_ITEM_SOLD;
                profit -= GetShopItemDescriptor(secondaryItem).Cost;
                if (profit < 0)
                {
                    profit *= -1;
                    stringId = STR_LOSS_PER_ITEM_SOLD;
                }

                auto ft = Formatter();
                ft.Add<money64>(profit);

                DrawTextBasic(rt, screenCoords, stringId, ft);
            }
            screenCoords.y += 18;

            // Income per hour
            if (ride->incomePerHour != kMoney64Undefined)
            {
                auto ft = Formatter();
                ft.Add<money64>(ride->incomePerHour);

                DrawTextBasic(rt, screenCoords, STR_INCOME_PER_HOUR, ft);
                screenCoords.y += kListRowHeight;
            }

            // Running cost per hour
            money64 costPerHour = ride->upkeepCost * 16;
            stringId = ride->upkeepCost == kMoney64Undefined ? STR_RUNNING_COST_UNKNOWN : STR_RUNNING_COST_PER_HOUR;
            auto ft = Formatter();
            ft.Add<money64>(costPerHour);
            DrawTextBasic(rt, screenCoords, stringId, ft);
            screenCoords.y += kListRowHeight;

            // Profit per hour
            if (ride->profit != kMoney64Undefined)
            {
                ft = Formatter();
                ft.Add<money64>(ride->profit);
                DrawTextBasic(rt, screenCoords, STR_PROFIT_PER_HOUR, ft);
                screenCoords.y += kListRowHeight;
            }
            screenCoords.y += 5;

            // Total profit
            ft = Formatter();
            ft.Add<money64>(ride->totalProfit);
            DrawTextBasic(rt, screenCoords, STR_TOTAL_PROFIT, ft);
        }

#pragma endregion

#pragma region Customer

        void CustomerOnMouseUp(WidgetIndex widgetIndex)
        {
            switch (widgetIndex)
            {
                case WIDX_CLOSE:
                    close();
                    return;
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
                    setPage(widgetIndex - WIDX_TAB_1);
                    break;
                case WIDX_SHOW_GUESTS_THOUGHTS:
                {
                    auto intent = Intent(WindowClass::guestList);
                    intent.PutExtra(
                        INTENT_EXTRA_GUEST_LIST_FILTER, static_cast<int32_t>(GuestListFilterType::guestsThinkingAboutRide));
                    intent.PutExtra(INTENT_EXTRA_RIDE_ID, number);
                    ContextOpenIntent(&intent);
                    break;
                }
                case WIDX_SHOW_GUESTS_ON_RIDE:
                {
                    auto intent = Intent(WindowClass::guestList);
                    intent.PutExtra(INTENT_EXTRA_GUEST_LIST_FILTER, static_cast<int32_t>(GuestListFilterType::guestsOnRide));
                    intent.PutExtra(INTENT_EXTRA_RIDE_ID, number);
                    ContextOpenIntent(&intent);
                    break;
                }
                case WIDX_SHOW_GUESTS_QUEUING:
                {
                    auto intent = Intent(WindowClass::guestList);
                    intent.PutExtra(INTENT_EXTRA_GUEST_LIST_FILTER, static_cast<int32_t>(GuestListFilterType::guestsInQueue));
                    intent.PutExtra(INTENT_EXTRA_RIDE_ID, number);
                    ContextOpenIntent(&intent);
                    break;
                }
            }
        }

        void CustomerResize()
        {
            flags |= WindowFlag::resizable;
            WindowSetResize(*this, { kMinimumWindowWidth, 163 }, { kMinimumWindowWidth, 163 });
        }

        void CustomerUpdate()
        {
            pickedPeepFrame++;
            if (pickedPeepFrame >= 24)
                pickedPeepFrame = 0;

            onPrepareDraw();
            invalidateWidget(WIDX_TAB_10);

            auto ride = GetRide(rideId);
            if (ride != nullptr && ride->windowInvalidateFlags.has(RideInvalidateFlag::customers))
            {
                ride->windowInvalidateFlags.unset(RideInvalidateFlag::customers);
                invalidate();
            }
        }

        void CustomerOnPrepareDraw()
        {
            SetPressedTab();

            auto ride = GetRide(rideId);
            if (ride != nullptr)
            {
                auto ft = Formatter::Common();
                ride->formatNameTo(ft);

                widgets[WIDX_SHOW_GUESTS_THOUGHTS].type = WidgetType::flatBtn;
                if (ride->getRideTypeDescriptor().HasFlag(RtdFlag::isShopOrFacility))
                {
                    widgets[WIDX_SHOW_GUESTS_ON_RIDE].type = WidgetType::empty;
                    widgets[WIDX_SHOW_GUESTS_QUEUING].type = WidgetType::empty;
                }
                else
                {
                    widgets[WIDX_SHOW_GUESTS_ON_RIDE].type = WidgetType::flatBtn;
                    widgets[WIDX_SHOW_GUESTS_QUEUING].type = WidgetType::flatBtn;
                }

                WindowAlignTabs(this, WIDX_TAB_1, WIDX_TAB_10);
            }
        }

        void CustomerOnDraw(RenderTarget& rt)
        {
            ShopItem shopItem;
            int16_t popularity, satisfaction, queueTime;
            StringId stringId;

            drawWidgets(rt);
            drawTabImages(rt);

            auto ride = GetRide(rideId);
            if (ride == nullptr)
                return;

            auto screenCoords = windowPos
                + ScreenCoordsXY{ widgets[WIDX_PAGE_BACKGROUND].left + 4, widgets[WIDX_PAGE_BACKGROUND].top + 4 };

            // Customers currently on ride
            if (ride->isRide())
            {
                auto ft = Formatter();
                ft.Add<int16_t>(ride->numRiders);
                DrawTextBasic(rt, screenCoords, STR_CUSTOMERS_ON_RIDE, ft);
                screenCoords.y += kListRowHeight;
            }

            // Customers per hour
            auto ft = Formatter();
            ft.Add<int32_t>(RideCustomersPerHour(*ride));
            DrawTextBasic(rt, screenCoords, STR_CUSTOMERS_PER_HOUR, ft);
            screenCoords.y += kListRowHeight;

            // Popularity
            popularity = ride->popularity;
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
            DrawTextBasic(rt, screenCoords, stringId, ft);
            screenCoords.y += kListRowHeight;

            // Satisfaction
            satisfaction = ride->satisfaction;
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
            DrawTextBasic(rt, screenCoords, stringId, ft);
            screenCoords.y += kListRowHeight;

            // Queue time
            if (ride->isRide())
            {
                queueTime = ride->getMaxQueueTime();
                stringId = queueTime == 1 ? STR_QUEUE_TIME_MINUTE : STR_QUEUE_TIME_MINUTES;
                ft = Formatter();
                ft.Add<int32_t>(queueTime);
                screenCoords.y += DrawTextWrapped(rt, screenCoords, 308, stringId, ft, { TextAlignment::left });
                screenCoords.y += 5;
            }

            // Primary shop items sold
            shopItem = ride->getRideEntry()->shop_item[0];
            if (shopItem != ShopItem::none)
            {
                ft = Formatter();
                ft.Add<StringId>(GetShopItemDescriptor(shopItem).Naming.Plural);
                ft.Add<uint32_t>(ride->numPrimaryItemsSold);
                DrawTextBasic(rt, screenCoords, STR_ITEMS_SOLD, ft);
                screenCoords.y += kListRowHeight;
            }

            // Secondary shop items sold / on-ride photos sold
            shopItem = (ride->lifecycleFlags & RIDE_LIFECYCLE_ON_RIDE_PHOTO) ? ride->getRideTypeDescriptor().PhotoItem
                                                                             : ride->getRideEntry()->shop_item[1];
            if (shopItem != ShopItem::none)
            {
                ft = Formatter();
                ft.Add<StringId>(GetShopItemDescriptor(shopItem).Naming.Plural);
                ft.Add<uint32_t>(ride->numSecondaryItemsSold);
                DrawTextBasic(rt, screenCoords, STR_ITEMS_SOLD, ft);
                screenCoords.y += kListRowHeight;
            }

            // Total customers
            ft = Formatter();
            ft.Add<uint32_t>(ride->totalCustomers);
            DrawTextBasic(rt, screenCoords, STR_TOTAL_CUSTOMERS, ft);
            screenCoords.y += kListRowHeight;

            // Guests favourite
            if (ride->isRide())
            {
                ft = Formatter();
                ft.Add<uint32_t>(ride->guestsFavourite);
                stringId = ride->guestsFavourite == 1 ? STR_FAVOURITE_RIDE_OF_GUEST : STR_FAVOURITE_RIDE_OF_GUESTS;
                DrawTextBasic(rt, screenCoords, stringId, ft);
                screenCoords.y += kListRowHeight;
            }
            screenCoords.y += 2;

            // Age
            // If the ride has a build date that is in the future, show it as built this year.
            int16_t age = std::max(DateGetYear(ride->getAge()), 0);
            stringId = age == 0 ? STR_BUILT_THIS_YEAR : age == 1 ? STR_BUILT_LAST_YEAR : STR_BUILT_YEARS_AGO;
            ft = Formatter();
            ft.Add<int16_t>(age);
            DrawTextBasic(rt, screenCoords, stringId, ft);
        }

#pragma endregion
    };

    /**
     *
     *  rct2: 0x006AEAB4
     */
    static RideWindow* WindowRideOpen(const Ride& ride)
    {
        auto* windowMgr = GetWindowManager();
        return windowMgr->Create<RideWindow>(
            WindowClass::ride, kWindowSize, { WindowFlag::higherContrastOnPress, WindowFlag::resizable }, ride);
    }

    /**
     *
     *  rct2: 0x006ACC28
     */
    WindowBase* RideMainOpen(const Ride& ride)
    {
        if (ride.type >= RIDE_TYPE_COUNT)
        {
            return nullptr;
        }

        auto* windowMgr = GetWindowManager();
        RideWindow* w = static_cast<RideWindow*>(windowMgr->BringToFrontByNumber(WindowClass::ride, ride.id.ToUnderlying()));
        if (w == nullptr)
        {
            w = WindowRideOpen(ride);
            w->setViewIndex(0);
        }
        else if (w->getViewIndex() >= (1 + ride.numTrains + ride.numStations))
        {
            w->setViewIndex(0);
        }

        if (isToolActive(w->classification, w->number))
        {
            ToolCancel();
        }

        if (w->page != WINDOW_RIDE_PAGE_MAIN)
        {
            w->setPage(WINDOW_RIDE_PAGE_MAIN);
        }

        w->onViewportRotate();
        return w;
    }

    /**
     *
     *  rct2: 0x006ACCCE
     */
    static WindowBase* WindowRideOpenStation(const Ride& ride, StationIndex stationIndex)
    {
        if (ride.type >= RIDE_TYPE_COUNT)
            return nullptr;

        if (ride.getRideTypeDescriptor().HasFlag(RtdFlag::noVehicles))
            return RideMainOpen(ride);

        auto* windowMgr = GetWindowManager();
        auto* w = static_cast<RideWindow*>(windowMgr->BringToFrontByNumber(WindowClass::ride, ride.id.ToUnderlying()));
        if (w == nullptr)
        {
            w = WindowRideOpen(ride);
        }

        if (isToolActive(w->classification, w->number))
        {
            ToolCancel();
        }

        // View
        for (int32_t i = stationIndex.ToUnderlying(); i >= 0; i--)
        {
            if (ride.getStations()[i].Start.IsNull())
            {
                stationIndex = StationIndex::FromUnderlying(stationIndex.ToUnderlying() - 1);
            }
        }

        w->setViewIndex(1 + ride.numTrains + stationIndex.ToUnderlying());

        return w;
    }

    WindowBase* RideOpenTrack(TileElement* tileElement)
    {
        assert(tileElement != nullptr);
        auto rideIndex = tileElement->GetRideIndex();
        if (!rideIndex.IsNull())
        {
            auto ride = GetRide(rideIndex);
            if (ride != nullptr)
            {
                const auto type = tileElement->GetType();
                if (type == TileElementType::Entrance)
                {
                    // Open ride window in station view
                    auto entranceElement = tileElement->AsEntrance();
                    auto stationIndex = entranceElement->GetStationIndex();
                    return WindowRideOpenStation(*ride, stationIndex);
                }
                else if (type == TileElementType::Track)
                {
                    // Open ride window in station view
                    auto trackElement = tileElement->AsTrack();
                    auto trackType = trackElement->GetTrackType();
                    const auto& ted = GetTrackElementDescriptor(trackType);
                    if (ted.sequences[0].flags.has(SequenceFlag::trackOrigin))
                    {
                        auto stationIndex = trackElement->GetStationIndex();
                        return WindowRideOpenStation(*ride, stationIndex);
                    }
                }

                // Open ride window in overview mode
                return RideMainOpen(*ride);
            }
        }
        return nullptr;
    }

    /**
     *
     *  rct2: 0x006ACAC2
     */
    WindowBase* RideOpenVehicle(Vehicle* vehicle)
    {
        if (vehicle == nullptr)
            return nullptr;

        Vehicle* headVehicle = vehicle->TrainHead();
        if (headVehicle == nullptr)
            return nullptr;

        EntityId headVehicleSpriteIndex = headVehicle->Id;
        auto ride = headVehicle->GetRide();
        if (ride == nullptr)
            return nullptr;

        // Get view index
        int16_t view = 1;
        for (int32_t i = 0; i <= Limits::kMaxTrainsPerRide; i++)
        {
            if (ride->vehicles[i] == headVehicleSpriteIndex)
                break;

            view++;
        }

        auto* windowMgr = GetWindowManager();
        auto* w = static_cast<RideWindow*>(windowMgr->FindByNumber(WindowClass::ride, ride->id.ToUnderlying()));
        if (w != nullptr)
        {
            w->invalidate();

            if (isToolActive(w->classification, w->number))
            {
                ToolCancel();
            }

            int32_t openedPeepWindow = 0;
            if (w->getViewIndex() == view)
            {
                int32_t numPeepsLeft = vehicle->num_peeps;
                for (int32_t i = 0; i < 32 && numPeepsLeft > 0; i++)
                {
                    Peep* peep = getGameState().entities.GetEntity<Guest>(vehicle->peep[i]);
                    if (peep == nullptr)
                        continue;

                    numPeepsLeft--;

                    WindowBase* w2 = windowMgr->FindByNumber(WindowClass::peep, vehicle->peep[i]);
                    if (w2 == nullptr)
                    {
                        auto intent = Intent(WindowClass::peep);
                        intent.PutExtra(INTENT_EXTRA_PEEP, peep);
                        ContextOpenIntent(&intent);
                        openedPeepWindow = 1;

                        break;
                    }
                }
            }

            w = static_cast<RideWindow*>(
                openedPeepWindow ? windowMgr->FindByNumber(WindowClass::ride, ride->id.ToUnderlying())
                                 : windowMgr->BringToFrontByNumber(WindowClass::ride, ride->id.ToUnderlying()));
        }

        if (w == nullptr)
        {
            w = WindowRideOpen(*ride);
        }

        w->setViewIndex(view);
        w->invalidate();

        return w;
    }

    void WindowRideInvalidateVehicle(const Vehicle& vehicle)
    {
        auto* windowMgr = GetWindowManager();
        auto* w = static_cast<RideWindow*>(windowMgr->FindByNumber(WindowClass::ride, vehicle.ride.ToUnderlying()));
        if (w == nullptr)
            return;

        auto ride = vehicle.GetRide();
        auto viewVehicleIndex = w->getViewIndex() - 1;
        if (ride == nullptr || viewVehicleIndex < 0 || viewVehicleIndex >= ride->numTrains)
            return;

        if (vehicle.Id != ride->vehicles[viewVehicleIndex])
            return;

        w->invalidate();
    }

    void WindowRidePaintResetVehicle(RideId rideIndex)
    {
        auto* windowMgr = GetWindowManager();
        auto w = static_cast<RideWindow*>(windowMgr->FindByNumber(WindowClass::ride, rideIndex.ToUnderlying()));
        if (w != nullptr)
        {
            if (w->page == 4) // WINDOW_RIDE_PAGE_COLOUR
            {
                w->resetVehicleIndex();
            }
            w->invalidate();
        }
    }

    static void CancelScenerySelection()
    {
        gGamePaused &= ~GAME_PAUSED_SAVING_TRACK;
        gTrackDesignSaveMode = false;
        Audio::Resume();

        WindowBase* main_w = WindowGetMain();
        if (main_w != nullptr)
        {
            main_w->viewport->flags &= ~(VIEWPORT_FLAG_HIDE_VERTICAL | VIEWPORT_FLAG_HIDE_BASE);
        }

        GfxInvalidateScreen();
        ToolCancel();
    }

    void WindowRideMeasurementsDesignCancel()
    {
        if (gTrackDesignSaveMode)
        {
            CancelScenerySelection();
        }
    }
} // namespace OpenRCT2::Ui::Windows
