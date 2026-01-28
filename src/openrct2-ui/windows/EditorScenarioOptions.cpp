/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../UiStringIds.h"
#include "../interface/Dropdown.h"
#include "../interface/Widget.h"
#include "Windows.h"

#include <algorithm>
#include <openrct2/Context.h>
#include <openrct2/Editor.h>
#include <openrct2/Game.h>
#include <openrct2/GameState.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/SpriteIds.h>
#include <openrct2/actions/ParkSetNameAction.h>
#include <openrct2/actions/ScenarioSetSettingAction.h>
#include <openrct2/core/String.hpp>
#include <openrct2/drawing/ColourMap.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/drawing/Font.h>
#include <openrct2/drawing/Rectangle.h>
#include <openrct2/drawing/Text.h>
#include <openrct2/entity/Peep.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/localisation/Language.h>
#include <openrct2/localisation/Localisation.Date.h>
#include <openrct2/management/Finance.h>
#include <openrct2/ride/Ride.h>
#include <openrct2/ride/RideManager.hpp>
#include <openrct2/ui/WindowManager.h>
#include <openrct2/world/Park.h>

using namespace OpenRCT2::Drawing;

namespace OpenRCT2::Ui::Windows
{
    static constexpr ScreenSize kSizeObjective = { 450, 122 };
    static constexpr ScreenSize kSizeScenarioDetails = { 450, 169 };
    static constexpr ScreenSize kSizeFinancial = { 380, 226 };
    static constexpr ScreenSize kSizeGuests = { 380, 154 };
    static constexpr ScreenSize kSizeLand = { 268, 134 };
    static constexpr ScreenSize kSizeRides = { 380, 224 };

    static constexpr money64 kObjectiveCurrencyLoanAndValueMax = 2000000.00_GBP;
    static constexpr money64 kObjectiveCurrencyLoanAndValueMin = 1000.00_GBP;
    static constexpr money64 kObjectiveCurrencyLoanAndValueAdjustment = 1000.00_GBP;

    static constexpr money64 kObjectiveCurrencyFoodMax = 2000000.00_GBP;
    static constexpr money64 kObjectiveCurrencyFoodMin = 1000.00_GBP;
    static constexpr money64 kObjectiveCurrencyFoodAdjustment = 100.00_GBP;

    static constexpr uint16_t kObjectiveLengthMax = 5000;
    static constexpr uint16_t kObjectiveLengthMin = 1000;
    static constexpr uint16_t kObjectiveLengthAdjustment = 100;

    static constexpr RideRating_t kObjectiveExcitementMax = RideRating::make(9, 90);
    static constexpr RideRating_t kObjectiveExcitementMin = RideRating::make(4, 00);
    static constexpr RideRating_t kObjectiveExcitementAdjustment = RideRating::make(0, 10);

    // The number has to leave a bit of room for other entities like vehicles, litter and balloons.
    static constexpr uint16_t kObjectiveGuestsMax = 50000;
    static constexpr uint16_t kObjectiveGuestsMin = 250;
    static constexpr uint16_t kObjectiveGuestsAdjustment = 50;

    static constexpr uint8_t kObjectiveYearMax = 25;
    static constexpr uint8_t kObjectiveYearMin = 1;
    static constexpr uint8_t kObjectiveYearAdjustment = 1;

#pragma region Widgets

    enum
    {
        WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_OBJECTIVE,
        WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_SCENARIO_DETAILS,
        WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_FINANCIAL,
        WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_GUESTS,
        WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_PARK,
        WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_RIDES,
        WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_COUNT
    };

    static constexpr StringId ObjectiveDropdownOptionNames[] = {
        STR_OBJECTIVE_DROPDOWN_NONE,
        STR_OBJECTIVE_DROPDOWN_NUMBER_OF_GUESTS_AT_A_GIVEN_DATE,
        STR_OBJECTIVE_DROPDOWN_PARK_VALUE_AT_A_GIVEN_DATE,
        STR_OBJECTIVE_DROPDOWN_HAVE_FUN,
        STR_OBJECTIVE_DROPDOWN_BUILD_THE_BEST_RIDE_YOU_CAN,
        STR_OBJECTIVE_DROPDOWN_BUILD_10_ROLLER_COASTERS,
        STR_OBJECTIVE_DROPDOWN_NUMBER_OF_GUESTS_IN_PARK,
        STR_OBJECTIVE_DROPDOWN_MONTHLY_INCOME_FROM_RIDE_TICKETS,
        STR_OBJECTIVE_DROPDOWN_BUILD_10_ROLLER_COASTERS_OF_A_GIVEN_LENGTH,
        STR_OBJECTIVE_DROPDOWN_FINISH_BUILDING_5_ROLLER_COASTERS,
        STR_OBJECTIVE_DROPDOWN_REPAY_LOAN_AND_ACHIEVE_A_GIVEN_PARK_VALUE,
        STR_OBJECTIVE_DROPDOWN_MONTHLY_PROFIT_FROM_FOOD_MERCHANDISE,
    };

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
        WIDX_PAGE_START,

        // Objective tab
        WIDX_OBJECTIVE_LABEL = WIDX_PAGE_START,
        WIDX_OBJECTIVE,
        WIDX_OBJECTIVE_DROPDOWN,
        WIDX_OBJECTIVE_ARG_1_LABEL,
        WIDX_OBJECTIVE_ARG_1,
        WIDX_OBJECTIVE_ARG_1_INCREASE,
        WIDX_OBJECTIVE_ARG_1_DECREASE,
        WIDX_OBJECTIVE_ARG_2_LABEL,
        WIDX_OBJECTIVE_ARG_2,
        WIDX_OBJECTIVE_ARG_2_INCREASE,
        WIDX_OBJECTIVE_ARG_2_DECREASE,
        WIDX_HARD_PARK_RATING,

        // Scenario details tab
        WIDX_PARK_NAME = WIDX_PAGE_START,
        WIDX_SCENARIO_NAME,
        WIDX_CATEGORY,
        WIDX_CATEGORY_DROPDOWN,
        WIDX_DETAILS,

        // Financial tab
        WIDX_NO_MONEY = WIDX_PAGE_START,
        WIDX_GROUP_LOAN,
        WIDX_INITIAL_LOAN_LABEL,
        WIDX_INITIAL_LOAN,
        WIDX_INITIAL_LOAN_INCREASE,
        WIDX_INITIAL_LOAN_DECREASE,
        WIDX_MAXIMUM_LOAN_LABEL,
        WIDX_MAXIMUM_LOAN,
        WIDX_MAXIMUM_LOAN_INCREASE,
        WIDX_MAXIMUM_LOAN_DECREASE,
        WIDX_INTEREST_RATE_LABEL,
        WIDX_INTEREST_RATE,
        WIDX_INTEREST_RATE_INCREASE,
        WIDX_INTEREST_RATE_DECREASE,
        WIDX_RCT1_INTEREST,
        WIDX_GROUP_BUSINESS_MODEL,
        WIDX_INITIAL_CASH_LABEL,
        WIDX_INITIAL_CASH,
        WIDX_INITIAL_CASH_INCREASE,
        WIDX_INITIAL_CASH_DECREASE,
        WIDX_PAY_FOR_PARK_OR_RIDES_LABEL,
        WIDX_PAY_FOR_PARK_OR_RIDES,
        WIDX_PAY_FOR_PARK_OR_RIDES_DROPDOWN,
        WIDX_ENTRY_PRICE_LABEL,
        WIDX_ENTRY_PRICE,
        WIDX_ENTRY_PRICE_INCREASE,
        WIDX_ENTRY_PRICE_DECREASE,
        WIDX_FORBID_MARKETING,

        // Guests tab
        WIDX_CASH_PER_GUEST_LABEL = WIDX_PAGE_START,
        WIDX_CASH_PER_GUEST,
        WIDX_CASH_PER_GUEST_INCREASE,
        WIDX_CASH_PER_GUEST_DECREASE,
        WIDX_GUEST_INITIAL_HAPPINESS_LABEL,
        WIDX_GUEST_INITIAL_HAPPINESS,
        WIDX_GUEST_INITIAL_HAPPINESS_INCREASE,
        WIDX_GUEST_INITIAL_HAPPINESS_DECREASE,
        WIDX_GUEST_INITIAL_HUNGER_LABEL,
        WIDX_GUEST_INITIAL_HUNGER,
        WIDX_GUEST_INITIAL_HUNGER_INCREASE,
        WIDX_GUEST_INITIAL_HUNGER_DECREASE,
        WIDX_GUEST_INITIAL_THIRST_LABEL,
        WIDX_GUEST_INITIAL_THIRST,
        WIDX_GUEST_INITIAL_THIRST_INCREASE,
        WIDX_GUEST_INITIAL_THIRST_DECREASE,
        WIDX_GUESTS_INTENSITY_PREFERENCE_LABEL,
        WIDX_GUESTS_INTENSITY_PREFERENCE,
        WIDX_GUESTS_INTENSITY_PREFERENCE_DROPDOWN,
        WIDX_HARD_GUEST_GENERATION,

        // Land tab
        WIDX_LAND_COST_LABEL = WIDX_PAGE_START,
        WIDX_LAND_COST,
        WIDX_LAND_COST_INCREASE,
        WIDX_LAND_COST_DECREASE,
        WIDX_CONSTRUCTION_RIGHTS_COST_LABEL,
        WIDX_CONSTRUCTION_RIGHTS_COST,
        WIDX_CONSTRUCTION_RIGHTS_COST_INCREASE,
        WIDX_CONSTRUCTION_RIGHTS_COST_DECREASE,
        WIDX_FORBID_TREE_REMOVAL,
        WIDX_FORBID_LANDSCAPE_CHANGES,
        WIDX_FORBID_HIGH_CONSTRUCTION
    };

    // clang-format off
    static constexpr auto makeOptionsWidgets = [](StringId title, ScreenSize size) {
        return makeWidgets(
            makeWindowShim(title, size),
            makeWidget({  0, 43}, { size.width, 106 }, WidgetType::resize, WindowColour::secondary),
            makeTab   ({  3, 17}, STR_SCENARIO_OPTIONS_OBJECTIVE_TIP             ),
            makeTab   ({ 34, 17}, STR_SCENARIO_OPTIONS_SCENARIO_DETAILS_TIP      ),
            makeTab   ({ 65, 17}, STR_SCENARIO_OPTIONS_FINANCIAL_TIP             ),
            makeTab   ({ 96, 17}, STR_SCENARIO_OPTIONS_GUESTS_TIP                ),
            makeTab   ({127, 17}, STR_SCENARIO_OPTIONS_LAND_RESTRICTIONS_TIP     ),
            makeTab   ({158, 17}, STR_SCENARIO_OPTIONS_PRESERVED_RIDES_TIP       )
        );
    };

    static constexpr auto window_editor_scenario_options_objective_widgets = makeWidgets(
        makeOptionsWidgets(STR_SCENARIO_OPTIONS_OBJECTIVE, kSizeObjective),
        makeWidget        ({  8,  48}, {344,  12}, WidgetType::label,        WindowColour::secondary, STR_OBJECTIVE_DROPDOWN_LABEL                                            ),
        makeWidget        ({ 98,  48}, {344,  12}, WidgetType::dropdownMenu, WindowColour::secondary, kStringIdNone,                STR_SELECT_OBJECTIVE_FOR_THIS_SCENARIO_TIP),
        makeWidget        ({430,  49}, { 11,  10}, WidgetType::button,       WindowColour::secondary, STR_DROPDOWN_GLYPH,           STR_SELECT_OBJECTIVE_FOR_THIS_SCENARIO_TIP),
        makeWidget        ({ 28,  65}, {140,  12}, WidgetType::label,        WindowColour::secondary, kStringIdEmpty                                                          ),
        makeSpinnerWidgets({158,  65}, {120,  12}, WidgetType::button,       WindowColour::secondary                                                                          ), // NB: 3 widgets
        makeWidget        ({ 28,  82}, {140,  12}, WidgetType::label,        WindowColour::secondary, STR_WINDOW_OBJECTIVE_DATE                                               ),
        makeSpinnerWidgets({158,  82}, {120,  12}, WidgetType::button,       WindowColour::secondary                                                                          ), // NB: 3 widgets
        makeWidget        ({ 14,  99}, {340,  12}, WidgetType::checkbox,     WindowColour::secondary, STR_HARD_PARK_RATING,         STR_HARD_PARK_RATING_TIP                  )
    );

    static constexpr auto window_editor_scenario_options_scenario_details_widgets = makeWidgets(
        makeOptionsWidgets(STR_SCENARIO_OPTIONS_SCENARIO_DETAILS, kSizeScenarioDetails),
        makeWidget        ({370, 48}, { 75,  12}, WidgetType::button,       WindowColour::secondary, STR_CHANGE,         STR_CHANGE_NAME_OF_PARK_TIP                    ),
        makeWidget        ({370, 65}, { 75,  12}, WidgetType::button,       WindowColour::secondary, STR_CHANGE,         STR_CHANGE_NAME_OF_SCENARIO_TIP                ),
        makeWidget        ({ 98, 82}, {180,  12}, WidgetType::dropdownMenu, WindowColour::secondary, kStringIdNone,      STR_SELECT_WHICH_GROUP_THIS_SCENARIO_APPEARS_IN),
        makeWidget        ({266, 83}, { 11,  10}, WidgetType::button,       WindowColour::secondary, STR_DROPDOWN_GLYPH, STR_SELECT_WHICH_GROUP_THIS_SCENARIO_APPEARS_IN),
        makeWidget        ({370, 99}, { 75,  12}, WidgetType::button,       WindowColour::secondary, STR_CHANGE,         STR_CHANGE_DETAIL_NOTES_ABOUT_PARK_SCENARIO_TIP)
    );

    static constexpr auto window_editor_scenario_options_financial_widgets = makeWidgets(
        makeOptionsWidgets(STR_SCENARIO_OPTIONS_FINANCIAL, kSizeFinancial),
        makeWidget        ({  8,  48}, {kSizeFinancial.width - 16, 12}, WidgetType::checkbox,     WindowColour::secondary, STR_MAKE_PARK_NO_MONEY,   STR_MAKE_PARK_NO_MONEY_TIP        ),
        makeWidget        ({  5,  63}, {kSizeFinancial.width - 10, 67}, WidgetType::groupbox,     WindowColour::secondary, STR_GROUP_LOAN_OPTIONS                                      ),
        makeWidget        ({  9,  77}, {                      250, 12}, WidgetType::label,        WindowColour::secondary, STR_INIT_LOAN_LABEL                                         ),
        makeSpinnerWidgets({268,  77}, {                      100, 12}, WidgetType::spinner,      WindowColour::secondary                                                              ), // NB: 3 widgets
        makeWidget        ({  9,  94}, {                      250, 12}, WidgetType::label,        WindowColour::secondary, STR_MAX_LOAN_LABEL                                          ),
        makeSpinnerWidgets({268,  94}, {                      100, 12}, WidgetType::spinner,      WindowColour::secondary                                                              ), // NB: 3 widgets
        makeWidget        ({  9, 111}, {                      280, 12}, WidgetType::label,        WindowColour::secondary, STR_INTEREST_RATE_LABEL                                     ),
        makeSpinnerWidgets({298, 111}, {                       70, 12}, WidgetType::spinner,      WindowColour::secondary                                                              ), // NB: 3 widgets
        makeWidget        ({ 10, 111}, {kSizeFinancial.width - 16, 12}, WidgetType::checkbox,     WindowColour::secondary, STR_RCT1_INTEREST,        STR_RCT1_INTEREST_TIP             ),
        makeWidget        ({  5, 133}, {kSizeFinancial.width - 10, 87}, WidgetType::groupbox,     WindowColour::secondary, STR_GROUP_BUSINESS_MODEL                                    ),
        makeWidget        ({  9, 150}, {                      250, 12}, WidgetType::label,        WindowColour::secondary, STR_INIT_CASH_LABEL                                         ),
        makeSpinnerWidgets({268, 150}, {                      100, 12}, WidgetType::spinner,      WindowColour::secondary                                                              ), // NB: 3 widgets
        makeWidget        ({  9, 167}, {                      150, 12}, WidgetType::label,        WindowColour::secondary, STR_EARNINGS_LABEL                                          ),
        makeWidget        ({158, 167}, {                      210, 12}, WidgetType::dropdownMenu, WindowColour::secondary, kStringIdNone,            STR_PAY_FOR_PARK_PAY_FOR_RIDES_TIP),
        makeWidget        ({356, 168}, {                       11, 10}, WidgetType::button,       WindowColour::secondary, STR_DROPDOWN_GLYPH,       STR_PAY_FOR_PARK_PAY_FOR_RIDES_TIP),
        makeWidget        ({  9, 184}, {                      280, 12}, WidgetType::label,        WindowColour::secondary, STR_ENTRY_PRICE_LABEL                                       ),
        makeSpinnerWidgets({298, 184}, {                       70, 12}, WidgetType::spinner,      WindowColour::secondary                                                              ), // NB: 3 widgets
        makeWidget        ({ 10, 201}, {kSizeFinancial.width - 16, 12}, WidgetType::checkbox,     WindowColour::secondary, STR_FORBID_MARKETING,     STR_FORBID_MARKETING_TIP          )
    );

    static constexpr auto window_editor_scenario_options_guests_widgets = makeWidgets(
        makeOptionsWidgets(STR_SCENARIO_OPTIONS_GUESTS, kSizeGuests),
        makeWidget        ({  8,  48}, {      280,  12}, WidgetType::label,        WindowColour::secondary, STR_CASH_PER_GUEST_LABEL                                                ),
        makeSpinnerWidgets({298,  48}, {       70,  12}, WidgetType::spinner,      WindowColour::secondary                                                                          ), // NB: 3 widgets
        makeWidget        ({  8,  65}, {      280,  12}, WidgetType::label,        WindowColour::secondary, STR_GUEST_INIT_HAPPINESS                                                ),
        makeSpinnerWidgets({298,  65}, {       70,  12}, WidgetType::spinner,      WindowColour::secondary                                                                          ), // NB: 3 widgets
        makeWidget        ({  8,  82}, {      280,  12}, WidgetType::label,        WindowColour::secondary, STR_GUEST_INIT_HUNGER                                                   ),
        makeSpinnerWidgets({298,  82}, {       70,  12}, WidgetType::spinner,      WindowColour::secondary                                                                          ), // NB: 3 widgets
        makeWidget        ({  8,  99}, {      280,  12}, WidgetType::label,        WindowColour::secondary, STR_GUEST_INIT_THIRST                                                   ),
        makeSpinnerWidgets({298,  99}, {       70,  12}, WidgetType::spinner,      WindowColour::secondary                                                                          ), // NB: 3 widgets
        makeWidget        ({  8, 116}, {      180,  12}, WidgetType::label,        WindowColour::secondary, STR_GUESTS_PREFER_INTENSITY_LABEL                                       ),
        makeWidget        ({198, 116}, {      170,  12}, WidgetType::dropdownMenu, WindowColour::secondary, kStringIdNone,                        STR_GUESTS_PREFER_INTENSITY_TIP   ),
        makeWidget        ({357, 117}, {       11,  10}, WidgetType::button,       WindowColour::secondary, STR_DROPDOWN_GLYPH,                   STR_GUESTS_PREFER_INTENSITY_TIP   ),
        makeWidget        ({  8, 133}, {      350,  12}, WidgetType::checkbox,     WindowColour::secondary, STR_HARD_GUEST_GENERATION,            STR_HARD_GUEST_GENERATION_TIP     )
    );

    static constexpr auto window_editor_scenario_options_land_widgets = makeWidgets(
        makeOptionsWidgets(STR_SCENARIO_OPTIONS_LAND_RESTRICTIONS, kSizeLand),
        makeWidget        ({  8,  48}, {                 170,  12}, WidgetType::label,    WindowColour::secondary, STR_LAND_COST_LABEL                                             ),
        makeSpinnerWidgets({188,  48}, {                  70,  12}, WidgetType::spinner,  WindowColour::secondary                                                                  ), // NB: 3 widgets
        makeWidget        ({  8,  65}, {                 170,  12}, WidgetType::label,    WindowColour::secondary, STR_RIGHTS_COST_LABEL                                           ),
        makeSpinnerWidgets({188,  65}, {                  70,  12}, WidgetType::spinner,  WindowColour::secondary                                                                  ), // NB: 3 widgets
        makeWidget        ({  8,  82}, {kSizeLand.width - 16,  12}, WidgetType::checkbox, WindowColour::secondary, STR_FORBID_TREE_REMOVAL,      STR_FORBID_TREE_REMOVAL_TIP       ),
        makeWidget        ({  8,  99}, {kSizeLand.width - 16,  12}, WidgetType::checkbox, WindowColour::secondary, STR_FORBID_LANDSCAPE_CHANGES, STR_FORBID_LANDSCAPE_CHANGES_TIP  ),
        makeWidget        ({  8, 116}, {kSizeLand.width - 16,  12}, WidgetType::checkbox, WindowColour::secondary, STR_FORBID_HIGH_CONSTRUCTION, STR_FORBID_HIGH_CONSTRUCTION_TIP  )
    );

    static constexpr auto window_editor_scenario_options_rides_widgets = makeWidgets(
        makeOptionsWidgets(STR_SCENARIO_OPTIONS_PRESERVED_RIDES, kSizeRides),
        makeWidget({  6,  46}, {kSizeRides.width - 12,  12}, WidgetType::label,  WindowColour::secondary, STR_WINDOW_PRESERVATION_ORDER),
        makeWidget({  3,  60}, {                  374, 161}, WidgetType::scroll, WindowColour::secondary, SCROLL_VERTICAL              )
    );

    static constexpr std::span<const Widget> window_editor_scenario_options_widgets[] = {
        window_editor_scenario_options_objective_widgets,
        window_editor_scenario_options_scenario_details_widgets,
        window_editor_scenario_options_financial_widgets,
        window_editor_scenario_options_guests_widgets,
        window_editor_scenario_options_land_widgets,
        window_editor_scenario_options_rides_widgets,
    };

#pragma endregion

#pragma region Enabled widgets

    static uint64_t window_editor_scenario_options_page_holdDownWidgets[] = {
        (1uLL << WIDX_OBJECTIVE_ARG_1_INCREASE) |
            (1uLL << WIDX_OBJECTIVE_ARG_1_DECREASE) |
            (1uLL << WIDX_OBJECTIVE_ARG_2_INCREASE) |
            (1uLL << WIDX_OBJECTIVE_ARG_2_DECREASE),
        0,
        (1uLL << WIDX_INITIAL_CASH_INCREASE) |
            (1uLL << WIDX_INITIAL_CASH_DECREASE) |
            (1uLL << WIDX_INITIAL_LOAN_INCREASE) |
            (1uLL << WIDX_INITIAL_LOAN_DECREASE) |
            (1uLL << WIDX_MAXIMUM_LOAN_INCREASE) |
            (1uLL << WIDX_MAXIMUM_LOAN_DECREASE) |
            (1uLL << WIDX_INTEREST_RATE_INCREASE) |
            (1uLL << WIDX_INTEREST_RATE_DECREASE),
        (1uLL << WIDX_CASH_PER_GUEST_INCREASE) |
            (1uLL << WIDX_CASH_PER_GUEST_DECREASE) |
            (1uLL << WIDX_GUEST_INITIAL_HAPPINESS_INCREASE) |
            (1uLL << WIDX_GUEST_INITIAL_HAPPINESS_DECREASE) |
            (1uLL << WIDX_GUEST_INITIAL_HUNGER_INCREASE) |
            (1uLL << WIDX_GUEST_INITIAL_HUNGER_DECREASE) |
            (1uLL << WIDX_GUEST_INITIAL_THIRST_INCREASE) |
            (1uLL << WIDX_GUEST_INITIAL_THIRST_DECREASE),
        (1uLL << WIDX_LAND_COST_INCREASE) |
            (1uLL << WIDX_LAND_COST_DECREASE) |
            (1uLL << WIDX_CONSTRUCTION_RIGHTS_COST_INCREASE) |
            (1uLL << WIDX_CONSTRUCTION_RIGHTS_COST_DECREASE) |
            (1uLL << WIDX_ENTRY_PRICE_INCREASE) |
            (1uLL << WIDX_ENTRY_PRICE_DECREASE),
        0
    };
    // clang-format on

#pragma endregion

    class EditorScenarioOptionsWindow final : public Window
    {
    private:
        // Not shops or facilities
        std::vector<RideId> _rideableRides;

    public:
        void onOpen() override
        {
            setPage(WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_OBJECTIVE);
        }

        void onMouseUp(WidgetIndex widgetIndex) override
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
                    setPage(widgetIndex - WIDX_TAB_1);
                    return;
            }

            switch (page)
            {
                case WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_OBJECTIVE:
                    return ObjectiveOnMouseUp(widgetIndex);
                case WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_SCENARIO_DETAILS:
                    return ScenarioDetailsOnMouseUp(widgetIndex);
                case WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_FINANCIAL:
                    return FinancialMouseUp(widgetIndex);
                case WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_PARK:
                    return LandMouseUp(widgetIndex);
            }
        }

        void onResize() override
        {
            switch (page)
            {
                case WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_OBJECTIVE:
                    return ObjectiveOnResize();
                case WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_SCENARIO_DETAILS:
                    return ScenarioDetailsOnResize();
                case WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_FINANCIAL:
                    return FinancialResize();
                case WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_GUESTS:
                    return GuestsResize();
                case WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_PARK:
                    return LandResize();
                case WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_RIDES:
                    return RidesOnResize();
            }
        }

        void onMouseDown(WidgetIndex widgetIndex) override
        {
            switch (page)
            {
                case WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_OBJECTIVE:
                    return ObjectiveOnMouseDown(widgetIndex);
                case WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_SCENARIO_DETAILS:
                    return ScenarioDetailsOnMouseDown(widgetIndex);
                case WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_FINANCIAL:
                    return FinancialMouseDown(widgetIndex);
                case WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_GUESTS:
                    return GuestsMouseDown(widgetIndex);
                case WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_PARK:
                    return LandMouseDown(widgetIndex);
            }
        }

        void onUpdate() override
        {
            switch (page)
            {
                case WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_OBJECTIVE:
                    return ObjectiveOnUpdate();
                case WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_SCENARIO_DETAILS:
                    return ScenarioDetailsOnUpdate();
                case WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_FINANCIAL:
                    return FinancialUpdate();
                case WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_GUESTS:
                    return GuestsUpdate();
                case WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_PARK:
                    return LandUpdate();
                case WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_RIDES:
                    return RidesOnUpdate();
            }
        }

        void onPrepareDraw() override
        {
            HideUnavailableTabs();

            switch (page)
            {
                case WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_OBJECTIVE:
                    return ObjectiveOnPrepareDraw();
                case WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_SCENARIO_DETAILS:
                    return ScenarioDetailsOnPrepareDraw();
                case WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_FINANCIAL:
                    return FinancialPrepareDraw();
                case WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_GUESTS:
                    return GuestsPrepareDraw();
                case WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_PARK:
                    return LandPrepareDraw();
                case WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_RIDES:
                    return RidesOnPrepareDraw();
            }
        }

        void onDraw(RenderTarget& rt) override
        {
            switch (page)
            {
                case WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_OBJECTIVE:
                    return ObjectiveOnDraw(rt);
                case WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_SCENARIO_DETAILS:
                    return ScenarioDetailsOnDraw(rt);
                case WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_FINANCIAL:
                    return FinancialDraw(rt);
                case WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_GUESTS:
                    return GuestsDraw(rt);
                case WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_PARK:
                    return LandDraw(rt);
                case WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_RIDES:
                    return RidesOnDraw(rt);
            }
        }

        void onDropdown(WidgetIndex widgetIndex, int32_t selectedIndex) override
        {
            switch (page)
            {
                case WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_OBJECTIVE:
                    return ObjectiveOnDropdown(widgetIndex, selectedIndex);
                case WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_SCENARIO_DETAILS:
                    return ScenarioDetailsOnDropdown(widgetIndex, selectedIndex);
                case WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_FINANCIAL:
                    return FinancialDropdown(widgetIndex, selectedIndex);
                case WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_GUESTS:
                    return GuestsDropdown(widgetIndex, selectedIndex);
            }
        }

        void onTextInput(WidgetIndex widgetIndex, std::string_view text) override
        {
            if (page == WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_SCENARIO_DETAILS)
            {
                ScenarioDetailsOnTextInput(widgetIndex, text);
            }
        }

        void onScrollMouseDown(int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
        {
            if (page == WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_RIDES)
            {
                RidesOnScrollMouseDown(scrollIndex, screenCoords);
            }
        }

        void onScrollMouseOver(int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
        {
            if (page == WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_RIDES)
            {
                RidesOnScrollMouseOver(scrollIndex, screenCoords);
            }
        }

        void onScrollDraw(int32_t scrollIndex, RenderTarget& rt) override
        {
            if (page == WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_RIDES)
            {
                RidesOnScrollDraw(rt, scrollIndex);
            }
        }

        ScreenSize onScrollGetSize(int32_t scrollIndex) override
        {
            if (page == WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_RIDES)
            {
                return RidesOnScrollGetSize(scrollIndex);
            }

            return {};
        }

    private:
        /**
         *
         *  rct2: 0x00672609
         */
        bool AnyRidesExist()
        {
            // Check if there are any rides (not shops or facilities)
            const auto& gameState = getGameState();
            const auto& rideManager = RideManager(gameState);
            return std::any_of(
                rideManager.begin(), rideManager.end(), [](const Ride& rideToCheck) { return rideToCheck.isRide(); });
        }

        void HideUnavailableTabs()
        {
            if (gLegacyScene != LegacyScene::scenarioEditor)
                return;

            auto step = getGameState().editorStep;
            bool isObjectiveSelection = step == EditorStep::ObjectiveSelection;
            bool isScenarioDetails = step == EditorStep::ScenarioDetails;
            bool isOtherTab = !isObjectiveSelection && !isScenarioDetails;

            // Disable tabs based on current editor step
            setWidgetDisabled(WIDX_TAB_1, !isObjectiveSelection);
            setWidgetDisabled(WIDX_TAB_2, !isScenarioDetails);
            for (auto i = 2; i < WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_COUNT; i++)
                setWidgetDisabled(WIDX_TAB_1 + i, !isOtherTab);

            setWidgetDisabled(WIDX_TAB_6, !(isOtherTab && AnyRidesExist()));

            // Reposition tabs based on availability so there are no gaps
            widgets[WIDX_TAB_1].left = 3;
            widgets[WIDX_TAB_1].right = widgets[WIDX_TAB_1].left + 30;
            WindowAlignTabs(this, WIDX_TAB_1, WIDX_TAB_6);

            // Switch tabs if our current tab has become unavailable
            if (isWidgetDisabled(WIDX_TAB_1 + page))
            {
                if (isObjectiveSelection)
                    setPage(WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_OBJECTIVE);
                else if (isScenarioDetails)
                    setPage(WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_SCENARIO_DETAILS);
                else
                    setPage(WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_FINANCIAL);
            }
        }

        void SetPressedTab()
        {
            for (int32_t i = 0; i < WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_COUNT; i++)
                setWidgetPressed(WIDX_TAB_1 + i, false);
            setWidgetPressed(WIDX_TAB_1 + page, true);
        }

        void DrawTabImages(RenderTarget& rt)
        {
            Widget* widget;
            int32_t spriteIndex;

            // Tab 1
            if (!isWidgetDisabled(WIDX_TAB_1))
            {
                widget = &widgets[WIDX_TAB_1];
                spriteIndex = SPR_TAB_OBJECTIVE_0;
                if (page == WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_OBJECTIVE)
                    spriteIndex += (currentFrame / 4) % 16;

                GfxDrawSprite(rt, ImageId(spriteIndex), windowPos + ScreenCoordsXY{ widget->left, widget->top });
            }

            // Tab 2
            if (!isWidgetDisabled(WIDX_TAB_2))
            {
                widget = &widgets[WIDX_TAB_2];
                spriteIndex = SPR_TAB_KIOSKS_AND_FACILITIES_0;
                if (page == WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_SCENARIO_DETAILS)
                    spriteIndex += (currentFrame / 4) % 8;

                GfxDrawSprite(rt, ImageId(spriteIndex), windowPos + ScreenCoordsXY{ widget->left, widget->top });
            }

            // Tab 3
            if (!isWidgetDisabled(WIDX_TAB_3))
            {
                widget = &widgets[WIDX_TAB_3];
                spriteIndex = SPR_TAB_FINANCES_SUMMARY_0;
                if (page == WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_FINANCIAL)
                    spriteIndex += (currentFrame / 2) % 8;

                GfxDrawSprite(rt, ImageId(spriteIndex), windowPos + ScreenCoordsXY{ widget->left, widget->top });
            }

            // Tab 4
            if (!isWidgetDisabled(WIDX_TAB_4))
            {
                widget = &widgets[WIDX_TAB_4];
                spriteIndex = SPR_TAB_GUESTS_0;
                if (page == WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_GUESTS)
                    spriteIndex += (currentFrame / 4) % 8;

                GfxDrawSprite(rt, ImageId(spriteIndex), windowPos + ScreenCoordsXY{ widget->left, widget->top });
            }

            // Tab 5
            if (!isWidgetDisabled(WIDX_TAB_5))
            {
                widget = &widgets[WIDX_TAB_5];
                spriteIndex = SPR_G2_MAP_GEN_TERRAIN_TAB;
                GfxDrawSprite(rt, ImageId(spriteIndex), windowPos + ScreenCoordsXY{ widget->left, widget->top });
            }

            // Tab 6
            if (!isWidgetDisabled(WIDX_TAB_6))
            {
                widget = &widgets[WIDX_TAB_6];
                spriteIndex = SPR_TAB_RIDE_0;
                if (page == WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_RIDES)
                    spriteIndex += (currentFrame / 4) % 16;

                GfxDrawSprite(rt, ImageId(spriteIndex), windowPos + ScreenCoordsXY{ widget->left, widget->top });
            }
        }

        void setPage(int32_t newPage)
        {
            // Skip setting page if we're already on this page, unless we're initialising the window
            if (page == newPage && !widgets.empty())
                return;

            page = newPage;
            currentFrame = 0;
            disabledWidgets = 0;
            holdDownWidgets = window_editor_scenario_options_page_holdDownWidgets[page];
            pressedWidgets = 0;

            setWidgets(window_editor_scenario_options_widgets[page]);
            invalidate();
            onResize();
            onPrepareDraw();
            initScrollWidgets();
            invalidate();
        }

#pragma region Objective

        /**
         *
         *  rct2: 0x0067201D
         */
        void SetObjective(Scenario::ObjectiveType objective)
        {
            auto& gameState = getGameState();
            auto& scenarioOptions = gameState.scenarioOptions;

            scenarioOptions.objective.Type = objective;
            invalidate();

            // Set default objective arguments
            switch (objective)
            {
                case Scenario::ObjectiveType::none:
                case Scenario::ObjectiveType::haveFun:
                case Scenario::ObjectiveType::buildTheBest:
                case Scenario::ObjectiveType::tenRollercoasters:
                    break;
                case Scenario::ObjectiveType::guestsBy:
                    scenarioOptions.objective.Year = 3;
                    scenarioOptions.objective.NumGuests = 1500;
                    break;
                case Scenario::ObjectiveType::parkValueBy:
                    scenarioOptions.objective.Year = 3;
                    scenarioOptions.objective.Currency = 50000.00_GBP;
                    break;
                case Scenario::ObjectiveType::guestsAndRating:
                    scenarioOptions.objective.NumGuests = 2000;
                    break;
                case Scenario::ObjectiveType::monthlyRideIncome:
                    scenarioOptions.objective.Currency = 10000.00_GBP;
                    break;
                case Scenario::ObjectiveType::tenRollercoastersLength:
                    scenarioOptions.objective.MinimumLength = 1200;
                    break;
                case Scenario::ObjectiveType::finishFiveRollercoasters:
                    scenarioOptions.objective.MinimumExcitement = RideRating::make(6, 70);
                    break;
                case Scenario::ObjectiveType::repayLoanAndParkValue:
                    scenarioOptions.objective.Currency = 50000.00_GBP;
                    break;
                case Scenario::ObjectiveType::monthlyFoodIncome:
                    scenarioOptions.objective.Currency = 1000.00_GBP;
                    break;
                default:
                    break;
            }
        }

        void ShowObjectiveDropdown()
        {
            const auto& gameState = getGameState();
            const auto& scenarioOptions = gameState.scenarioOptions;
            auto objectiveType = EnumValue(scenarioOptions.objective.Type);

            int32_t numItems = 0;
            for (auto i = 0; i < EnumValue(Scenario::ObjectiveType::count); i++)
            {
                auto obj = Scenario::ObjectiveType(i);
                if (obj == Scenario::ObjectiveType::none || obj == Scenario::ObjectiveType::buildTheBest)
                    continue;

                const bool objectiveAllowedByMoneyUsage = !(gameState.park.flags & PARK_FLAGS_NO_MONEY)
                    || !Scenario::ObjectiveNeedsMoney(obj);

                // This objective can only work if the player can ask money for rides.
                const bool objectiveAllowedByPaymentSettings = (obj != Scenario::ObjectiveType::monthlyRideIncome)
                    || Park::RidePricesUnlocked();

                if (objectiveAllowedByMoneyUsage && objectiveAllowedByPaymentSettings)
                {
                    gDropdown.items[numItems] = Dropdown::MenuLabel(ObjectiveDropdownOptionNames[i]);
                    gDropdown.items[numItems].value = i;
                    if (i == objectiveType)
                    {
                        gDropdown.items[numItems].setChecked(true);
                    }
                    numItems++;
                }
            }

            Widget* dropdownWidget = &widgets[WIDX_OBJECTIVE];
            WindowDropdownShowTextCustomWidth(
                { windowPos.x + dropdownWidget->left, windowPos.y + dropdownWidget->top }, dropdownWidget->height(), colours[1],
                0, Dropdown::Flag::StayOpen, numItems, dropdownWidget->width() - 4);
        }

        void ShowCategoryDropdown()
        {
            for (int32_t i = EnumValue(Scenario::Category::beginner); i <= EnumValue(Scenario::Category::other); i++)
            {
                gDropdown.items[i] = Dropdown::MenuLabel(Scenario::kScenarioCategoryStringIds[i]);
            }

            Widget* dropdownWidget = &widgets[WIDX_CATEGORY];
            WindowDropdownShowTextCustomWidth(
                { windowPos.x + dropdownWidget->left, windowPos.y + dropdownWidget->top }, dropdownWidget->height(), colours[1],
                0, Dropdown::Flag::StayOpen, 5, dropdownWidget->width() - 4);

            gDropdown.items[EnumValue(getGameState().scenarioOptions.category)].setChecked(true);
        }

        void Arg1Increase()
        {
            auto& gameState = getGameState();
            auto& scenarioOptions = gameState.scenarioOptions;

            switch (scenarioOptions.objective.Type)
            {
                case Scenario::ObjectiveType::parkValueBy:
                case Scenario::ObjectiveType::monthlyRideIncome:
                case Scenario::ObjectiveType::repayLoanAndParkValue:
                    if (scenarioOptions.objective.Currency >= kObjectiveCurrencyLoanAndValueMax)
                    {
                        ContextShowError(STR_CANT_INCREASE_FURTHER, kStringIdNone, {});
                    }
                    else
                    {
                        scenarioOptions.objective.Currency += kObjectiveCurrencyLoanAndValueAdjustment;
                        invalidate();
                    }
                    break;
                case Scenario::ObjectiveType::monthlyFoodIncome:
                    if (scenarioOptions.objective.Currency >= kObjectiveCurrencyFoodMax)
                    {
                        ContextShowError(STR_CANT_INCREASE_FURTHER, kStringIdNone, {});
                    }
                    else
                    {
                        scenarioOptions.objective.Currency += kObjectiveCurrencyFoodAdjustment;
                        invalidate();
                    }
                    break;
                case Scenario::ObjectiveType::tenRollercoastersLength:
                    if (scenarioOptions.objective.MinimumLength >= kObjectiveLengthMax)
                    {
                        ContextShowError(STR_CANT_INCREASE_FURTHER, kStringIdNone, {});
                    }
                    else
                    {
                        scenarioOptions.objective.MinimumLength += kObjectiveLengthAdjustment;
                        invalidate();
                    }
                    break;
                case Scenario::ObjectiveType::finishFiveRollercoasters:
                    if (scenarioOptions.objective.MinimumExcitement >= kObjectiveExcitementMax)
                    {
                        ContextShowError(STR_CANT_INCREASE_FURTHER, kStringIdNone, {});
                    }
                    else
                    {
                        scenarioOptions.objective.MinimumExcitement += kObjectiveExcitementAdjustment;
                        invalidate();
                    }
                    break;
                default:
                    if (scenarioOptions.objective.NumGuests >= kObjectiveGuestsMax)
                    {
                        ContextShowError(STR_CANT_INCREASE_FURTHER, kStringIdNone, {});
                    }
                    else
                    {
                        scenarioOptions.objective.NumGuests += kObjectiveGuestsAdjustment;
                        invalidate();
                    }
                    break;
            }
        }

        void Arg1Decrease()
        {
            auto& gameState = getGameState();
            auto& scenarioOptions = gameState.scenarioOptions;

            switch (scenarioOptions.objective.Type)
            {
                case Scenario::ObjectiveType::parkValueBy:
                case Scenario::ObjectiveType::monthlyRideIncome:
                case Scenario::ObjectiveType::repayLoanAndParkValue:
                    if (scenarioOptions.objective.Currency <= kObjectiveCurrencyLoanAndValueMin)
                    {
                        ContextShowError(STR_CANT_REDUCE_FURTHER, kStringIdNone, {});
                    }
                    else
                    {
                        scenarioOptions.objective.Currency -= kObjectiveCurrencyLoanAndValueAdjustment;
                        invalidate();
                    }
                    break;
                case Scenario::ObjectiveType::monthlyFoodIncome:
                    if (scenarioOptions.objective.Currency <= kObjectiveCurrencyFoodMin)
                    {
                        ContextShowError(STR_CANT_REDUCE_FURTHER, kStringIdNone, {});
                    }
                    else
                    {
                        scenarioOptions.objective.Currency -= kObjectiveCurrencyFoodAdjustment;
                        invalidate();
                    }
                    break;
                case Scenario::ObjectiveType::tenRollercoastersLength:
                    if (scenarioOptions.objective.MinimumLength <= kObjectiveLengthMin)
                    {
                        ContextShowError(STR_CANT_REDUCE_FURTHER, kStringIdNone, {});
                    }
                    else
                    {
                        scenarioOptions.objective.MinimumLength -= kObjectiveLengthAdjustment;
                        invalidate();
                    }
                    break;
                case Scenario::ObjectiveType::finishFiveRollercoasters:
                    if (scenarioOptions.objective.MinimumExcitement <= kObjectiveExcitementMin)
                    {
                        ContextShowError(STR_CANT_REDUCE_FURTHER, kStringIdNone, {});
                    }
                    else
                    {
                        scenarioOptions.objective.MinimumExcitement -= kObjectiveExcitementAdjustment;
                        invalidate();
                    }
                    break;
                default:
                    if (scenarioOptions.objective.NumGuests <= kObjectiveGuestsMin)
                    {
                        ContextShowError(STR_CANT_REDUCE_FURTHER, kStringIdNone, {});
                    }
                    else
                    {
                        scenarioOptions.objective.NumGuests -= kObjectiveGuestsAdjustment;
                        invalidate();
                    }
                    break;
            }
        }

        void Arg2Increase()
        {
            auto& gameState = getGameState();
            auto& scenarioOptions = gameState.scenarioOptions;

            if (scenarioOptions.objective.Year >= kObjectiveYearMax)
            {
                ContextShowError(STR_CANT_INCREASE_FURTHER, kStringIdNone, {});
            }
            else
            {
                scenarioOptions.objective.Year += kObjectiveYearAdjustment;
                invalidate();
            }
        }

        void Arg2Decrease()
        {
            auto& gameState = getGameState();
            auto& scenarioOptions = gameState.scenarioOptions;

            if (scenarioOptions.objective.Year <= kObjectiveYearMin)
            {
                ContextShowError(STR_CANT_REDUCE_FURTHER, kStringIdNone, {});
            }
            else
            {
                scenarioOptions.objective.Year -= kObjectiveYearAdjustment;
                invalidate();
            }
        }

        /**
         *
         *  rct2: 0x00672254
         */
        void ObjectiveOnResize()
        {
            WindowSetResize(*this, kSizeObjective, kSizeObjective);
        }

        void ObjectiveOnMouseUp(WidgetIndex widgetIndex)
        {
            switch (widgetIndex)
            {
                case WIDX_HARD_PARK_RATING:
                {
                    auto& gameState = getGameState();
                    auto scenarioSetSetting = GameActions::ScenarioSetSettingAction(
                        GameActions::ScenarioSetSetting::ParkRatingHigherDifficultyLevel,
                        gameState.park.flags & PARK_FLAGS_DIFFICULT_PARK_RATING ? 0 : 1);
                    GameActions::Execute(&scenarioSetSetting, gameState);
                    invalidate();
                    break;
                }
            }
        }

        /**
         *
         *  rct2: 0x00671A0D
         */
        void ObjectiveOnMouseDown(WidgetIndex widgetIndex)
        {
            switch (widgetIndex)
            {
                case WIDX_OBJECTIVE_DROPDOWN:
                    ShowObjectiveDropdown();
                    break;
                case WIDX_OBJECTIVE_ARG_1_INCREASE:
                    Arg1Increase();
                    break;
                case WIDX_OBJECTIVE_ARG_1_DECREASE:
                    Arg1Decrease();
                    break;
                case WIDX_OBJECTIVE_ARG_2_INCREASE:
                    Arg2Increase();
                    break;
                case WIDX_OBJECTIVE_ARG_2_DECREASE:
                    Arg2Decrease();
                    break;
            }
        }

        /**
         *
         *  rct2: 0x00671A54
         */
        void ObjectiveOnDropdown(WidgetIndex widgetIndex, int32_t dropdownIndex)
        {
            auto& gameState = getGameState();

            if (dropdownIndex == -1)
                return;

            switch (widgetIndex)
            {
                case WIDX_OBJECTIVE_DROPDOWN:
                    auto newObjectiveType = static_cast<Scenario::ObjectiveType>(gDropdown.items[dropdownIndex].value);
                    if (gameState.scenarioOptions.objective.Type != newObjectiveType)
                        SetObjective(newObjectiveType);
                    break;
            }
        }

        /**
         *
         *  rct2: 0x006721E7
         */
        void ObjectiveOnUpdate()
        {
            currentFrame++;
            onPrepareDraw();
            invalidateWidget(WIDX_TAB_1);

            auto objectiveType = getGameState().scenarioOptions.objective.Type;

            // Check if objective is allowed by money and pay-per-ride settings.
            const bool objectiveAllowedByMoneyUsage = !(getGameState().park.flags & PARK_FLAGS_NO_MONEY)
                || !ObjectiveNeedsMoney(objectiveType);

            // This objective can only work if the player can ask money for rides.
            const bool objectiveAllowedByPaymentSettings = (objectiveType != Scenario::ObjectiveType::monthlyRideIncome)
                || Park::RidePricesUnlocked();

            if (!objectiveAllowedByMoneyUsage || !objectiveAllowedByPaymentSettings)
            {
                // Reset objective
                SetObjective(Scenario::ObjectiveType::guestsAndRating);
            }
        }

        /**
         *
         *  rct2: 0x0067161C
         */
        void ObjectiveOnPrepareDraw()
        {
            auto& gameState = getGameState();

            SetPressedTab();

            switch (gameState.scenarioOptions.objective.Type)
            {
                case Scenario::ObjectiveType::guestsBy:
                case Scenario::ObjectiveType::parkValueBy:
                    widgets[WIDX_OBJECTIVE_ARG_1_LABEL].type = WidgetType::label;
                    widgets[WIDX_OBJECTIVE_ARG_1].type = WidgetType::spinner;
                    widgets[WIDX_OBJECTIVE_ARG_1_INCREASE].type = WidgetType::button;
                    widgets[WIDX_OBJECTIVE_ARG_1_DECREASE].type = WidgetType::button;
                    widgets[WIDX_OBJECTIVE_ARG_2_LABEL].type = WidgetType::label;
                    widgets[WIDX_OBJECTIVE_ARG_2].type = WidgetType::spinner;
                    widgets[WIDX_OBJECTIVE_ARG_2_INCREASE].type = WidgetType::button;
                    widgets[WIDX_OBJECTIVE_ARG_2_DECREASE].type = WidgetType::button;
                    break;
                case Scenario::ObjectiveType::guestsAndRating:
                case Scenario::ObjectiveType::monthlyRideIncome:
                case Scenario::ObjectiveType::tenRollercoastersLength:
                case Scenario::ObjectiveType::finishFiveRollercoasters:
                case Scenario::ObjectiveType::repayLoanAndParkValue:
                case Scenario::ObjectiveType::monthlyFoodIncome:
                    widgets[WIDX_OBJECTIVE_ARG_1_LABEL].type = WidgetType::label;
                    widgets[WIDX_OBJECTIVE_ARG_1].type = WidgetType::spinner;
                    widgets[WIDX_OBJECTIVE_ARG_1_INCREASE].type = WidgetType::button;
                    widgets[WIDX_OBJECTIVE_ARG_1_DECREASE].type = WidgetType::button;
                    widgets[WIDX_OBJECTIVE_ARG_2_LABEL].type = WidgetType::empty;
                    widgets[WIDX_OBJECTIVE_ARG_2].type = WidgetType::empty;
                    widgets[WIDX_OBJECTIVE_ARG_2_INCREASE].type = WidgetType::empty;
                    widgets[WIDX_OBJECTIVE_ARG_2_DECREASE].type = WidgetType::empty;
                    break;
                default:
                    widgets[WIDX_OBJECTIVE_ARG_1_LABEL].type = WidgetType::empty;
                    widgets[WIDX_OBJECTIVE_ARG_1].type = WidgetType::empty;
                    widgets[WIDX_OBJECTIVE_ARG_1_INCREASE].type = WidgetType::empty;
                    widgets[WIDX_OBJECTIVE_ARG_1_DECREASE].type = WidgetType::empty;
                    widgets[WIDX_OBJECTIVE_ARG_2_LABEL].type = WidgetType::empty;
                    widgets[WIDX_OBJECTIVE_ARG_2].type = WidgetType::empty;
                    widgets[WIDX_OBJECTIVE_ARG_2_INCREASE].type = WidgetType::empty;
                    widgets[WIDX_OBJECTIVE_ARG_2_DECREASE].type = WidgetType::empty;
                    break;
            }

            auto arg1StringId = kStringIdEmpty;
            if (widgets[WIDX_OBJECTIVE_ARG_1_LABEL].type != WidgetType::empty)
            {
                // Objective argument 1 label
                switch (gameState.scenarioOptions.objective.Type)
                {
                    case Scenario::ObjectiveType::guestsBy:
                    case Scenario::ObjectiveType::guestsAndRating:
                        arg1StringId = STR_WINDOW_OBJECTIVE_GUEST_COUNT;
                        break;
                    case Scenario::ObjectiveType::parkValueBy:
                    case Scenario::ObjectiveType::repayLoanAndParkValue:
                        arg1StringId = STR_WINDOW_OBJECTIVE_PARK_VALUE;
                        break;
                    case Scenario::ObjectiveType::monthlyRideIncome:
                        arg1StringId = STR_WINDOW_OBJECTIVE_MONTHLY_INCOME;
                        break;
                    case Scenario::ObjectiveType::monthlyFoodIncome:
                        arg1StringId = STR_WINDOW_OBJECTIVE_MONTHLY_PROFIT;
                        break;
                    case Scenario::ObjectiveType::tenRollercoastersLength:
                        arg1StringId = STR_WINDOW_OBJECTIVE_MINIMUM_LENGTH;
                        break;
                    default:
                        arg1StringId = STR_WINDOW_OBJECTIVE_EXCITEMENT_RATING;
                        break;
                }
            }

            widgets[WIDX_OBJECTIVE_ARG_1_LABEL].text = arg1StringId;

            widgets[WIDX_CLOSE].type = gLegacyScene == LegacyScene::scenarioEditor ? WidgetType::empty : WidgetType::closeBox;

            setWidgetPressed(WIDX_HARD_PARK_RATING, gameState.park.flags & PARK_FLAGS_DIFFICULT_PARK_RATING);
        }

        /**
         *
         *  rct2: 0x0067161C
         */
        void ObjectiveOnDraw(RenderTarget& rt)
        {
            const auto& gameState = getGameState();
            const auto& scenarioOptions = gameState.scenarioOptions;

            drawWidgets(rt);
            DrawTabImages(rt);

            // Objective value
            auto screenCoords = windowPos + ScreenCoordsXY{ widgets[WIDX_OBJECTIVE].left + 1, widgets[WIDX_OBJECTIVE].top };
            auto ft = Formatter();
            ft.Add<StringId>(ObjectiveDropdownOptionNames[EnumValue(scenarioOptions.objective.Type)]);
            DrawTextBasic(rt, screenCoords, STR_WINDOW_COLOUR_2_STRINGID, ft);

            if (widgets[WIDX_OBJECTIVE_ARG_1].type != WidgetType::empty)
            {
                const auto wColour2 = colours[1];
                StringId stringId = kStringIdEmpty;

                // Objective argument 1 value
                screenCoords = windowPos
                    + ScreenCoordsXY{ widgets[WIDX_OBJECTIVE_ARG_1].left + 1, widgets[WIDX_OBJECTIVE_ARG_1].top };
                ft = Formatter();
                switch (scenarioOptions.objective.Type)
                {
                    case Scenario::ObjectiveType::guestsBy:
                    case Scenario::ObjectiveType::guestsAndRating:
                        stringId = STR_WINDOW_COLOUR_2_COMMA32;
                        ft.Add<int32_t>(scenarioOptions.objective.NumGuests);
                        break;
                    case Scenario::ObjectiveType::parkValueBy:
                    case Scenario::ObjectiveType::repayLoanAndParkValue:
                    case Scenario::ObjectiveType::monthlyRideIncome:
                    case Scenario::ObjectiveType::monthlyFoodIncome:
                        stringId = STR_CURRENCY_FORMAT_LABEL;
                        ft.Add<money64>(scenarioOptions.objective.Currency);
                        break;
                    case Scenario::ObjectiveType::tenRollercoastersLength:
                        stringId = STR_WINDOW_COLOUR_2_LENGTH;
                        ft.Add<uint16_t>(scenarioOptions.objective.MinimumLength);
                        break;
                    case Scenario::ObjectiveType::finishFiveRollercoasters:
                        stringId = STR_WINDOW_COLOUR_2_COMMA2DP32;
                        ft.Add<uint16_t>(scenarioOptions.objective.MinimumExcitement);
                        break;
                    default:
                        stringId = STR_WINDOW_COLOUR_2_COMMA2DP32;
                        ft.Add<money64>(scenarioOptions.objective.Currency);
                        break;
                }
                DrawTextBasic(rt, screenCoords, stringId, ft, wColour2);
            }

            if (widgets[WIDX_OBJECTIVE_ARG_2].type != WidgetType::empty)
            {
                // Objective argument 2 value
                screenCoords = windowPos
                    + ScreenCoordsXY{ widgets[WIDX_OBJECTIVE_ARG_2].left + 1, widgets[WIDX_OBJECTIVE_ARG_2].top };
                ft = Formatter();
                ft.Add<uint16_t>((scenarioOptions.objective.Year * MONTH_COUNT) - 1);
                DrawTextBasic(rt, screenCoords, STR_WINDOW_OBJECTIVE_VALUE_DATE, ft);
            }
        }

#pragma endregion

#pragma region ScenarioDetails

        /**
         *
         *  rct2: 0x006719CA
         */
        void ScenarioDetailsOnMouseUp(WidgetIndex widgetIndex)
        {
            const auto& gameState = getGameState();
            switch (widgetIndex)
            {
                case WIDX_PARK_NAME:
                {
                    WindowTextInputRawOpen(
                        this, WIDX_PARK_NAME, STR_PARK_NAME, STR_ENTER_PARK_NAME, {}, gameState.park.name.c_str(),
                        kParkNameMaxLength);
                    break;
                }
                case WIDX_SCENARIO_NAME:
                    WindowTextInputRawOpen(
                        this, WIDX_SCENARIO_NAME, STR_SCENARIO_NAME, STR_ENTER_SCENARIO_NAME, {},
                        gameState.scenarioOptions.name.c_str(), kScenarioNameMaxLength);
                    break;
                case WIDX_DETAILS:
                    WindowTextInputRawOpen(
                        this, WIDX_DETAILS, STR_PARK_SCENARIO_DETAILS, STR_ENTER_SCENARIO_DESCRIPTION, {},
                        gameState.scenarioOptions.details.c_str(), kScenarioDetailsNameMaxLength);
                    break;
            }
        }

        void ScenarioDetailsOnMouseDown(WidgetIndex widgetIndex)
        {
            switch (widgetIndex)
            {
                case WIDX_CATEGORY_DROPDOWN:
                    ShowCategoryDropdown();
                    break;
            }
        }

        void ScenarioDetailsOnDropdown(WidgetIndex widgetIndex, int32_t dropdownIndex)
        {
            if (dropdownIndex == -1)
                return;

            switch (widgetIndex)
            {
                case WIDX_CATEGORY_DROPDOWN:
                {
                    auto& gameState = getGameState();
                    if (gameState.scenarioOptions.category != static_cast<Scenario::Category>(dropdownIndex))
                    {
                        gameState.scenarioOptions.category = static_cast<Scenario::Category>(dropdownIndex);
                        invalidate();
                    }
                    break;
                }
            }
        }
        void ScenarioDetailsOnPrepareDraw()
        {
            SetPressedTab();
        }

        void ScenarioDetailsOnDraw(RenderTarget& rt)
        {
            drawWidgets(rt);
            DrawTabImages(rt);

            const auto& gameState = getGameState();

            // Land name
            auto screenCoords = windowPos + ScreenCoordsXY{ 8, widgets[WIDX_PARK_NAME].top };
            int32_t widthToSet = widgets[WIDX_PARK_NAME].left - 16;

            {
                auto parkName = gameState.park.name.c_str();

                auto ft = Formatter();
                ft.Add<StringId>(STR_STRING);
                ft.Add<const char*>(parkName);
                DrawTextEllipsised(rt, screenCoords, widthToSet, STR_WINDOW_PARK_NAME, ft);
            }

            // Scenario name
            screenCoords = windowPos + ScreenCoordsXY{ 8, widgets[WIDX_SCENARIO_NAME].top };
            widthToSet = widgets[WIDX_SCENARIO_NAME].left - 16;

            auto ft = Formatter();
            ft.Add<StringId>(STR_STRING);
            ft.Add<const char*>(gameState.scenarioOptions.name.c_str());
            DrawTextEllipsised(rt, screenCoords, widthToSet, STR_WINDOW_SCENARIO_NAME, ft);

            // Scenario details label
            screenCoords = windowPos + ScreenCoordsXY{ 8, widgets[WIDX_DETAILS].top };
            DrawTextBasic(rt, screenCoords, STR_WINDOW_PARK_DETAILS);

            // Scenario details value
            screenCoords = windowPos + ScreenCoordsXY{ 16, widgets[WIDX_DETAILS].top + 10 };
            widthToSet = widgets[WIDX_DETAILS].left - 4;

            ft = Formatter();
            ft.Add<StringId>(STR_STRING);
            ft.Add<const char*>(gameState.scenarioOptions.details.c_str());
            DrawTextWrapped(rt, screenCoords, widthToSet, STR_BLACK_STRING, ft);

            // Scenario category label
            screenCoords = windowPos + ScreenCoordsXY{ 8, widgets[WIDX_CATEGORY].top };
            DrawTextBasic(rt, screenCoords, STR_WINDOW_SCENARIO_GROUP);

            // Scenario category value
            screenCoords = windowPos + ScreenCoordsXY{ widgets[WIDX_CATEGORY].left + 1, widgets[WIDX_CATEGORY].top };
            ft = Formatter();
            ft.Add<StringId>(Scenario::kScenarioCategoryStringIds[EnumValue(gameState.scenarioOptions.category)]);
            DrawTextBasic(rt, screenCoords, STR_WINDOW_COLOUR_2_STRINGID, ft);
        }

        /**
         *
         *  rct2: 0x00671A73
         */
        void ScenarioDetailsOnTextInput(WidgetIndex widgetIndex, std::string_view text)
        {
            if (text.empty())
                return;

            auto& gameState = getGameState();
            auto& scenarioOptions = gameState.scenarioOptions;

            switch (widgetIndex)
            {
                case WIDX_PARK_NAME:
                {
                    auto action = GameActions::ParkSetNameAction(std::string(text));
                    GameActions::Execute(&action, gameState);

                    if (scenarioOptions.name.empty())
                    {
                        scenarioOptions.name = gameState.park.name;
                    }
                    break;
                }
                case WIDX_SCENARIO_NAME:
                    scenarioOptions.name = text;
                    invalidate();
                    break;
                case WIDX_DETAILS:
                    scenarioOptions.details = text;
                    invalidate();
                    break;
            }
        }

        void ScenarioDetailsOnUpdate()
        {
            currentFrame++;
            invalidateWidget(WIDX_TAB_1);
        }

        void ScenarioDetailsOnResize()
        {
            WindowSetResize(*this, kSizeScenarioDetails, kSizeScenarioDetails);
        }

#pragma endregion

#pragma region Financial

        void FinancialMouseUp(WidgetIndex widgetIndex)
        {
            auto& gameState = getGameState();
            switch (widgetIndex)
            {
                case WIDX_NO_MONEY:
                {
                    auto newMoneySetting = (gameState.park.flags & PARK_FLAGS_NO_MONEY) ? 0 : 1;
                    auto scenarioSetSetting = GameActions::ScenarioSetSettingAction(
                        GameActions::ScenarioSetSetting::NoMoney, newMoneySetting);
                    GameActions::Execute(&scenarioSetSetting, gameState);
                    invalidate();
                    break;
                }
                case WIDX_FORBID_MARKETING:
                {
                    auto scenarioSetSetting = GameActions::ScenarioSetSettingAction(
                        GameActions::ScenarioSetSetting::ForbidMarketingCampaigns,
                        gameState.park.flags & PARK_FLAGS_FORBID_MARKETING_CAMPAIGN ? 0 : 1);
                    GameActions::Execute(&scenarioSetSetting, gameState);
                    invalidate();
                    break;
                }
                case WIDX_RCT1_INTEREST:
                {
                    auto scenarioSetSetting = GameActions::ScenarioSetSettingAction(
                        GameActions::ScenarioSetSetting::UseRCT1Interest,
                        gameState.park.flags & PARK_FLAGS_RCT1_INTEREST ? 0 : 1);
                    GameActions::Execute(&scenarioSetSetting, gameState);
                    invalidate();
                    break;
                }
            }
        }

        void FinancialResize()
        {
            WindowSetResize(*this, kSizeFinancial, kSizeFinancial);
        }

        void FinancialMouseDown(WidgetIndex widgetIndex)
        {
            auto& gameState = getGameState();
            switch (widgetIndex)
            {
                case WIDX_INITIAL_CASH_INCREASE:
                    if (gameState.scenarioOptions.initialCash < 1000000.00_GBP)
                    {
                        auto scenarioSetSetting = GameActions::ScenarioSetSettingAction(
                            GameActions::ScenarioSetSetting::InitialCash, gameState.scenarioOptions.initialCash + 500.00_GBP);
                        GameActions::Execute(&scenarioSetSetting, gameState);
                    }
                    else
                    {
                        ContextShowError(STR_CANT_INCREASE_CASH, kStringIdNone, {});
                    }
                    invalidate();
                    break;
                case WIDX_INITIAL_CASH_DECREASE:
                    if (gameState.scenarioOptions.initialCash > 0.00_GBP)
                    {
                        auto scenarioSetSetting = GameActions::ScenarioSetSettingAction(
                            GameActions::ScenarioSetSetting::InitialCash, gameState.scenarioOptions.initialCash - 500.00_GBP);
                        GameActions::Execute(&scenarioSetSetting, gameState);
                    }
                    else
                    {
                        ContextShowError(STR_CANT_REDUCE_CASH, kStringIdNone, {});
                    }
                    invalidate();
                    break;
                case WIDX_INITIAL_LOAN_INCREASE:
                    if (gameState.park.bankLoan < 5000000.00_GBP)
                    {
                        auto scenarioSetSetting = GameActions::ScenarioSetSettingAction(
                            GameActions::ScenarioSetSetting::InitialLoan, gameState.park.bankLoan + 1000.00_GBP);
                        GameActions::Execute(&scenarioSetSetting, gameState);
                    }
                    else
                    {
                        ContextShowError(STR_CANT_INCREASE_INIT_LOAN, kStringIdNone, {});
                    }
                    invalidate();
                    break;
                case WIDX_INITIAL_LOAN_DECREASE:
                    if (gameState.park.bankLoan > 0.00_GBP)
                    {
                        auto scenarioSetSetting = GameActions::ScenarioSetSettingAction(
                            GameActions::ScenarioSetSetting::InitialLoan, gameState.park.bankLoan - 1000.00_GBP);
                        GameActions::Execute(&scenarioSetSetting, gameState);
                    }
                    else
                    {
                        ContextShowError(STR_CANT_REDUCE_INIT_LOAN, kStringIdNone, {});
                    }
                    invalidate();
                    break;
                case WIDX_MAXIMUM_LOAN_INCREASE:
                    if (gameState.park.maxBankLoan < 5000000.00_GBP)
                    {
                        auto scenarioSetSetting = GameActions::ScenarioSetSettingAction(
                            GameActions::ScenarioSetSetting::MaximumLoanSize, gameState.park.maxBankLoan + 1000.00_GBP);
                        GameActions::Execute(&scenarioSetSetting, gameState);
                    }
                    else
                    {
                        ContextShowError(STR_CANT_INCREASE_MAX_LOAN, kStringIdNone, {});
                    }
                    invalidate();
                    break;
                case WIDX_MAXIMUM_LOAN_DECREASE:
                    if (gameState.park.maxBankLoan > 0.00_GBP)
                    {
                        auto scenarioSetSetting = GameActions::ScenarioSetSettingAction(
                            GameActions::ScenarioSetSetting::MaximumLoanSize, gameState.park.maxBankLoan - 1000.00_GBP);
                        GameActions::Execute(&scenarioSetSetting, gameState);
                    }
                    else
                    {
                        ContextShowError(STR_CANT_REDUCE_MAX_LOAN, kStringIdNone, {});
                    }
                    invalidate();
                    break;
                case WIDX_INTEREST_RATE_INCREASE:
                    if (gameState.park.bankLoanInterestRate < kMaxBankLoanInterestRate)
                    {
                        auto scenarioSetSetting = GameActions::ScenarioSetSettingAction(
                            GameActions::ScenarioSetSetting::AnnualInterestRate, gameState.park.bankLoanInterestRate + 1);
                        GameActions::Execute(&scenarioSetSetting, gameState);
                    }
                    else
                    {
                        ContextShowError(STR_CANT_INCREASE_INTEREST_RATE, kStringIdNone, {});
                    }
                    invalidate();
                    break;
                case WIDX_INTEREST_RATE_DECREASE:
                    if (gameState.park.bankLoanInterestRate > 0)
                    {
                        auto interest = std::min<uint8_t>(kMaxBankLoanInterestRate, gameState.park.bankLoanInterestRate - 1);
                        auto scenarioSetSetting = GameActions::ScenarioSetSettingAction(
                            GameActions::ScenarioSetSetting::AnnualInterestRate, interest);
                        GameActions::Execute(&scenarioSetSetting, gameState);
                    }
                    else
                    {
                        ContextShowError(STR_CANT_REDUCE_INTEREST_RATE, kStringIdNone, {});
                    }
                    invalidate();
                    break;
                case WIDX_ENTRY_PRICE_INCREASE:
                    if (gameState.park.entranceFee < kMaxEntranceFee)
                    {
                        auto scenarioSetSetting = GameActions::ScenarioSetSettingAction(
                            GameActions::ScenarioSetSetting::ParkChargeEntryFee, gameState.park.entranceFee + 1.00_GBP);
                        GameActions::Execute(&scenarioSetSetting, gameState);
                    }
                    else
                    {
                        ContextShowError(STR_CANT_INCREASE_FURTHER, kStringIdNone, {});
                    }
                    invalidate();
                    break;
                case WIDX_ENTRY_PRICE_DECREASE:
                    if (gameState.park.entranceFee > 0.00_GBP)
                    {
                        auto scenarioSetSetting = GameActions::ScenarioSetSettingAction(
                            GameActions::ScenarioSetSetting::ParkChargeEntryFee, gameState.park.entranceFee - 1.00_GBP);
                        GameActions::Execute(&scenarioSetSetting, gameState);
                    }
                    else
                    {
                        ContextShowError(STR_CANT_REDUCE_FURTHER, kStringIdNone, {});
                    }
                    invalidate();
                    break;
                case WIDX_PAY_FOR_PARK_OR_RIDES_DROPDOWN:
                {
                    Widget* dropdownWidget = &widgets[widgetIndex - 1];

                    gDropdown.items[0] = Dropdown::MenuLabel(STR_FREE_PARK_ENTER);
                    gDropdown.items[1] = Dropdown::MenuLabel(STR_PAY_PARK_ENTER);
                    gDropdown.items[2] = Dropdown::MenuLabel(STR_PAID_ENTRY_PAID_RIDES);

                    WindowDropdownShowTextCustomWidth(
                        { windowPos.x + dropdownWidget->left, windowPos.y + dropdownWidget->top }, dropdownWidget->height() - 2,
                        colours[1], 0, Dropdown::Flag::StayOpen, 3, dropdownWidget->width() - 4);

                    if (gameState.park.flags & PARK_FLAGS_UNLOCK_ALL_PRICES)
                        gDropdown.items[2].setChecked(true);
                    else if (gameState.park.flags & PARK_FLAGS_PARK_FREE_ENTRY)
                        gDropdown.items[0].setChecked(true);
                    else
                        gDropdown.items[1].setChecked(true);

                    break;
                }
            }

            if (gLegacyScene == LegacyScene::playing)
            {
                auto* windowMgr = GetWindowManager();
                windowMgr->InvalidateByClass(WindowClass::finances);
                windowMgr->InvalidateByClass(WindowClass::bottomToolbar);
            }
        }

        void FinancialDropdown(WidgetIndex widgetIndex, int32_t dropdownIndex)
        {
            if (dropdownIndex == -1)
            {
                return;
            }

            switch (widgetIndex)
            {
                case WIDX_PAY_FOR_PARK_OR_RIDES_DROPDOWN:
                {
                    auto scenarioSetSetting = GameActions::ScenarioSetSettingAction(
                        GameActions::ScenarioSetSetting::ParkChargeMethod, dropdownIndex);
                    GameActions::Execute(&scenarioSetSetting, getGameState());
                    invalidate();
                    break;
                }
            }
        }

        void FinancialUpdate()
        {
            currentFrame++;
            invalidateWidget(WIDX_TAB_1);
        }

        void FinancialPrepareDraw()
        {
            SetPressedTab();

            auto& gameState = getGameState();
            bool noMoney = gameState.park.flags & PARK_FLAGS_NO_MONEY;
            setWidgetPressed(WIDX_NO_MONEY, noMoney);

            setWidgetDisabled(WIDX_GROUP_LOAN, noMoney);
            setWidgetDisabled(WIDX_INITIAL_LOAN_LABEL, noMoney);
            setWidgetDisabled(WIDX_INITIAL_LOAN, noMoney);
            setWidgetDisabled(WIDX_INITIAL_LOAN_INCREASE, noMoney);
            setWidgetDisabled(WIDX_INITIAL_LOAN_DECREASE, noMoney);
            setWidgetDisabled(WIDX_MAXIMUM_LOAN_LABEL, noMoney);
            setWidgetDisabled(WIDX_MAXIMUM_LOAN, noMoney);
            setWidgetDisabled(WIDX_MAXIMUM_LOAN_INCREASE, noMoney);
            setWidgetDisabled(WIDX_MAXIMUM_LOAN_DECREASE, noMoney);

            if (gameState.park.flags & PARK_FLAGS_RCT1_INTEREST)
            {
                widgets[WIDX_INTEREST_RATE_LABEL].type = WidgetType::empty;
                widgets[WIDX_INTEREST_RATE].type = WidgetType::empty;
                widgets[WIDX_INTEREST_RATE_INCREASE].type = WidgetType::empty;
                widgets[WIDX_INTEREST_RATE_DECREASE].type = WidgetType::empty;
                widgets[WIDX_RCT1_INTEREST].type = WidgetType::checkbox;
                setWidgetPressed(WIDX_RCT1_INTEREST, true);
                setWidgetDisabled(WIDX_RCT1_INTEREST, noMoney);
            }
            else
            {
                widgets[WIDX_INTEREST_RATE_LABEL].type = WidgetType::label;
                widgets[WIDX_INTEREST_RATE].type = WidgetType::spinner;
                widgets[WIDX_INTEREST_RATE_INCREASE].type = WidgetType::button;
                widgets[WIDX_INTEREST_RATE_DECREASE].type = WidgetType::button;
                widgets[WIDX_RCT1_INTEREST].type = WidgetType::empty;
                setWidgetDisabled(WIDX_INTEREST_RATE_LABEL, noMoney);
                setWidgetDisabled(WIDX_INTEREST_RATE, noMoney);
                setWidgetDisabled(WIDX_INTEREST_RATE_INCREASE, noMoney);
                setWidgetDisabled(WIDX_INTEREST_RATE_DECREASE, noMoney);
            }

            setWidgetDisabled(WIDX_GROUP_BUSINESS_MODEL, noMoney);
            setWidgetDisabled(WIDX_INITIAL_CASH_LABEL, noMoney);
            setWidgetDisabled(WIDX_INITIAL_CASH, noMoney);
            setWidgetDisabled(WIDX_INITIAL_CASH_INCREASE, noMoney);
            setWidgetDisabled(WIDX_INITIAL_CASH_DECREASE, noMoney);
            setWidgetDisabled(WIDX_PAY_FOR_PARK_OR_RIDES_LABEL, noMoney);
            setWidgetDisabled(WIDX_PAY_FOR_PARK_OR_RIDES, noMoney);
            setWidgetDisabled(WIDX_PAY_FOR_PARK_OR_RIDES_DROPDOWN, noMoney);
            setWidgetDisabled(WIDX_FORBID_MARKETING, noMoney);

            if (!Park::EntranceFeeUnlocked())
            {
                widgets[WIDX_ENTRY_PRICE_LABEL].type = WidgetType::empty;
                widgets[WIDX_ENTRY_PRICE].type = WidgetType::empty;
                widgets[WIDX_ENTRY_PRICE_INCREASE].type = WidgetType::empty;
                widgets[WIDX_ENTRY_PRICE_DECREASE].type = WidgetType::empty;
            }
            else
            {
                widgets[WIDX_ENTRY_PRICE_LABEL].type = WidgetType::label;
                widgets[WIDX_ENTRY_PRICE].type = WidgetType::spinner;
                widgets[WIDX_ENTRY_PRICE_INCREASE].type = WidgetType::button;
                widgets[WIDX_ENTRY_PRICE_DECREASE].type = WidgetType::button;
                setWidgetDisabled(WIDX_ENTRY_PRICE_LABEL, noMoney);
                setWidgetDisabled(WIDX_ENTRY_PRICE, noMoney);
                setWidgetDisabled(WIDX_ENTRY_PRICE_INCREASE, noMoney);
                setWidgetDisabled(WIDX_ENTRY_PRICE_DECREASE, noMoney);
            }

            setWidgetPressed(WIDX_FORBID_MARKETING, gameState.park.flags & PARK_FLAGS_FORBID_MARKETING_CAMPAIGN);

            widgets[WIDX_CLOSE].type = gLegacyScene == LegacyScene::scenarioEditor ? WidgetType::empty : WidgetType::closeBox;
        }

        void FinancialDraw(RenderTarget& rt)
        {
            ScreenCoordsXY screenCoords{};

            WindowDrawWidgets(*this, rt);
            DrawTabImages(rt);

            const auto& gameState = getGameState();
            const auto wColour2 = colours[1];

            const auto& initialCashWidget = widgets[WIDX_INITIAL_CASH];
            if (initialCashWidget.type != WidgetType::empty)
            {
                screenCoords = windowPos + ScreenCoordsXY{ initialCashWidget.left + 1, initialCashWidget.top };
                auto ft = Formatter();
                ft.Add<money64>(getGameState().scenarioOptions.initialCash);
                auto colour = !isWidgetDisabled(WIDX_INITIAL_CASH) ? wColour2 : wColour2.withFlag(ColourFlag::inset, true);
                DrawTextBasic(rt, screenCoords, STR_CURRENCY_FORMAT_LABEL, ft, colour);
            }

            const auto& initialLoanWidget = widgets[WIDX_INITIAL_LOAN];
            if (initialLoanWidget.type != WidgetType::empty)
            {
                screenCoords = windowPos + ScreenCoordsXY{ initialLoanWidget.left + 1, initialLoanWidget.top };
                auto ft = Formatter();
                ft.Add<money64>(gameState.park.bankLoan);
                auto colour = !isWidgetDisabled(WIDX_INITIAL_LOAN) ? wColour2 : wColour2.withFlag(ColourFlag::inset, true);
                DrawTextBasic(rt, screenCoords, STR_CURRENCY_FORMAT_LABEL, ft, colour);
            }

            const auto& maximumLoanWidget = widgets[WIDX_MAXIMUM_LOAN];
            if (maximumLoanWidget.type != WidgetType::empty)
            {
                screenCoords = windowPos + ScreenCoordsXY{ maximumLoanWidget.left + 1, maximumLoanWidget.top };
                auto ft = Formatter();
                ft.Add<money64>(getGameState().park.maxBankLoan);
                auto colour = !isWidgetDisabled(WIDX_MAXIMUM_LOAN) ? wColour2 : wColour2.withFlag(ColourFlag::inset, true);
                DrawTextBasic(rt, screenCoords, STR_CURRENCY_FORMAT_LABEL, ft, colour);
            }

            const auto& interestRateWidget = widgets[WIDX_INTEREST_RATE];
            if (interestRateWidget.type != WidgetType::empty)
            {
                screenCoords = windowPos + ScreenCoordsXY{ interestRateWidget.left + 1, interestRateWidget.top };

                auto ft = Formatter();
                ft.Add<int16_t>(
                    std::clamp<int16_t>(static_cast<int16_t>(gameState.park.bankLoanInterestRate), INT16_MIN, INT16_MAX));
                auto colour = !isWidgetDisabled(WIDX_INTEREST_RATE) ? wColour2 : wColour2.withFlag(ColourFlag::inset, true);
                DrawTextBasic(rt, screenCoords, STR_PERCENT_FORMAT_LABEL, ft, colour);
            }

            const auto& payForParkOrRidesWidget = widgets[WIDX_PAY_FOR_PARK_OR_RIDES];
            if (payForParkOrRidesWidget.type != WidgetType::empty)
            {
                // Pay for park or rides label
                screenCoords = windowPos + ScreenCoordsXY{ payForParkOrRidesWidget.left + 1, payForParkOrRidesWidget.top };

                auto ft = Formatter();
                // Pay for park and/or rides value
                if (gameState.park.flags & PARK_FLAGS_UNLOCK_ALL_PRICES)
                    ft.Add<StringId>(STR_PAID_ENTRY_PAID_RIDES);
                else if (gameState.park.flags & PARK_FLAGS_PARK_FREE_ENTRY)
                    ft.Add<StringId>(STR_FREE_PARK_ENTER);
                else
                    ft.Add<StringId>(STR_PAY_PARK_ENTER);

                auto colour = !isWidgetDisabled(WIDX_PAY_FOR_PARK_OR_RIDES) ? wColour2
                                                                            : wColour2.withFlag(ColourFlag::inset, true);
                DrawTextBasic(rt, screenCoords, STR_STRINGID, ft, colour);
            }

            const auto& entryPriceWidget = widgets[WIDX_ENTRY_PRICE];
            if (entryPriceWidget.type != WidgetType::empty)
            {
                // Entry price value
                screenCoords = windowPos + ScreenCoordsXY{ entryPriceWidget.left + 1, entryPriceWidget.top };
                auto ft = Formatter();
                ft.Add<money64>(gameState.park.entranceFee);
                auto colour = !isWidgetDisabled(WIDX_INITIAL_CASH) ? wColour2 : wColour2.withFlag(ColourFlag::inset, true);
                DrawTextBasic(rt, screenCoords, STR_CURRENCY_FORMAT_LABEL, ft, colour);
            }
        }

#pragma endregion

#pragma region Guests

        void GuestsResize()
        {
            WindowSetResize(*this, kSizeGuests, kSizeGuests);
        }

        void GuestsMouseDown(WidgetIndex widgetIndex)
        {
            auto& gameState = getGameState();
            auto& scenarioOptions = gameState.scenarioOptions;

            switch (widgetIndex)
            {
                case WIDX_CASH_PER_GUEST_INCREASE:
                    if (scenarioOptions.guestInitialCash < 1000.00_GBP)
                    {
                        auto scenarioSetSetting = GameActions::ScenarioSetSettingAction(
                            GameActions::ScenarioSetSetting::AverageCashPerGuest, scenarioOptions.guestInitialCash + 1.00_GBP);
                        GameActions::Execute(&scenarioSetSetting, gameState);
                    }
                    else
                    {
                        ContextShowError(STR_CANT_INCREASE_FURTHER, kStringIdNone, {});
                    }
                    invalidate();
                    break;
                case WIDX_CASH_PER_GUEST_DECREASE:
                    if (scenarioOptions.guestInitialCash > 0.00_GBP)
                    {
                        auto scenarioSetSetting = GameActions::ScenarioSetSettingAction(
                            GameActions::ScenarioSetSetting::AverageCashPerGuest, scenarioOptions.guestInitialCash - 1.00_GBP);
                        GameActions::Execute(&scenarioSetSetting, gameState);
                    }
                    else
                    {
                        ContextShowError(STR_CANT_REDUCE_FURTHER, kStringIdNone, {});
                    }
                    invalidate();
                    break;
                case WIDX_GUEST_INITIAL_HAPPINESS_INCREASE:
                    if (scenarioOptions.guestInitialHappiness < 250)
                    {
                        auto scenarioSetSetting = GameActions::ScenarioSetSettingAction(
                            GameActions::ScenarioSetSetting::GuestInitialHappiness, scenarioOptions.guestInitialHappiness + 4);
                        GameActions::Execute(&scenarioSetSetting, gameState);
                    }
                    else
                    {
                        ContextShowError(STR_CANT_INCREASE_FURTHER, kStringIdNone, {});
                    }
                    invalidate();
                    break;
                case WIDX_GUEST_INITIAL_HAPPINESS_DECREASE:
                    if (scenarioOptions.guestInitialHappiness > 40)
                    {
                        auto scenarioSetSetting = GameActions::ScenarioSetSettingAction(
                            GameActions::ScenarioSetSetting::GuestInitialHappiness, scenarioOptions.guestInitialHappiness - 4);
                        GameActions::Execute(&scenarioSetSetting, gameState);
                    }
                    else
                    {
                        ContextShowError(STR_CANT_REDUCE_FURTHER, kStringIdNone, {});
                    }
                    invalidate();
                    break;
                case WIDX_GUEST_INITIAL_HUNGER_INCREASE:
                    if (scenarioOptions.guestInitialHunger > 40)
                    {
                        auto scenarioSetSetting = GameActions::ScenarioSetSettingAction(
                            GameActions::ScenarioSetSetting::GuestInitialHunger, scenarioOptions.guestInitialHunger - 4);
                        GameActions::Execute(&scenarioSetSetting, gameState);
                    }
                    else
                    {
                        ContextShowError(STR_CANT_INCREASE_FURTHER, kStringIdNone, {});
                    }
                    invalidate();
                    break;
                case WIDX_GUEST_INITIAL_HUNGER_DECREASE:
                    if (scenarioOptions.guestInitialHunger < 250)
                    {
                        auto scenarioSetSetting = GameActions::ScenarioSetSettingAction(
                            GameActions::ScenarioSetSetting::GuestInitialHunger, scenarioOptions.guestInitialHunger + 4);
                        GameActions::Execute(&scenarioSetSetting, gameState);
                    }
                    else
                    {
                        ContextShowError(STR_CANT_REDUCE_FURTHER, kStringIdNone, {});
                    }
                    invalidate();
                    break;
                case WIDX_GUEST_INITIAL_THIRST_INCREASE:
                    if (scenarioOptions.guestInitialThirst > 40)
                    {
                        auto scenarioSetSetting = GameActions::ScenarioSetSettingAction(
                            GameActions::ScenarioSetSetting::GuestInitialThirst, scenarioOptions.guestInitialThirst - 4);
                        GameActions::Execute(&scenarioSetSetting, gameState);
                    }
                    else
                    {
                        ContextShowError(STR_CANT_INCREASE_FURTHER, kStringIdNone, {});
                    }
                    invalidate();
                    break;
                case WIDX_GUEST_INITIAL_THIRST_DECREASE:
                    if (scenarioOptions.guestInitialThirst < 250)
                    {
                        auto scenarioSetSetting = GameActions::ScenarioSetSettingAction(
                            GameActions::ScenarioSetSetting::GuestInitialThirst, scenarioOptions.guestInitialThirst + 4);
                        GameActions::Execute(&scenarioSetSetting, gameState);
                    }
                    else
                    {
                        ContextShowError(STR_CANT_REDUCE_FURTHER, kStringIdNone, {});
                    }
                    invalidate();
                    break;

                case WIDX_GUESTS_INTENSITY_PREFERENCE_DROPDOWN:
                {
                    auto& dropdownWidget = widgets[widgetIndex - 1];

                    gDropdown.items[0] = Dropdown::MenuLabel(STR_GUESTS_PREFER_INTENSITY_NONE);
                    gDropdown.items[1] = Dropdown::MenuLabel(STR_GUESTS_PREFER_INTENSITY_BALANCED);
                    gDropdown.items[2] = Dropdown::MenuLabel(STR_GUESTS_PREFER_INTENSITY_LESS_INTENSE_RIDES);
                    gDropdown.items[3] = Dropdown::MenuLabel(STR_GUESTS_PREFER_INTENSITY_MORE_INTENSE_RIDES);

                    WindowDropdownShowTextCustomWidth(
                        { windowPos.x + dropdownWidget.left, windowPos.y + dropdownWidget.top }, dropdownWidget.height() - 2,
                        colours[1], 0, Dropdown::Flag::StayOpen, 4, dropdownWidget.width() - 4);

                    const auto preferLess = gameState.park.flags & PARK_FLAGS_PREF_LESS_INTENSE_RIDES;
                    const auto preferMore = gameState.park.flags & PARK_FLAGS_PREF_MORE_INTENSE_RIDES;

                    auto prefItem = 1;
                    if (preferLess && preferMore)
                        prefItem = 0;
                    else if (preferLess && !preferMore)
                        prefItem = 2;
                    else if (!preferLess && preferMore)
                        prefItem = 3;

                    gDropdown.items[prefItem].setChecked(true);
                    break;
                }
                case WIDX_HARD_GUEST_GENERATION:
                {
                    auto scenarioSetSetting = GameActions::ScenarioSetSettingAction(
                        GameActions::ScenarioSetSetting::GuestGenerationHigherDifficultyLevel,
                        gameState.park.flags & PARK_FLAGS_DIFFICULT_GUEST_GENERATION ? 0 : 1);
                    GameActions::Execute(&scenarioSetSetting, gameState);
                    invalidate();
                    break;
                }
            }
        }

        void GuestsDropdown(WidgetIndex widgetIndex, int32_t dropdownIndex)
        {
            if (dropdownIndex < 0 || dropdownIndex > 3)
                return;

            bool preferLess = dropdownIndex == 0 || dropdownIndex == 2;
            bool preferMore = dropdownIndex == 0 || dropdownIndex == 3;

            auto& gameState = getGameState();

            auto scenarioSetLessSetting = GameActions::ScenarioSetSettingAction(
                GameActions::ScenarioSetSetting::GuestsPreferLessIntenseRides, preferLess);
            GameActions::Execute(&scenarioSetLessSetting, gameState);

            auto scenarioSetMoreSetting = GameActions::ScenarioSetSettingAction(
                GameActions::ScenarioSetSetting::GuestsPreferMoreIntenseRides, preferMore);
            GameActions::Execute(&scenarioSetMoreSetting, gameState);
        }

        void GuestsUpdate()
        {
            currentFrame++;
            invalidateWidget(WIDX_TAB_2);
        }

        void GuestsPrepareDraw()
        {
            SetPressedTab();

            auto& gameState = getGameState();
            bool noMoney = gameState.park.flags & PARK_FLAGS_NO_MONEY;

            setWidgetDisabled(WIDX_CASH_PER_GUEST_LABEL, noMoney);
            setWidgetDisabled(WIDX_CASH_PER_GUEST, noMoney);
            setWidgetDisabled(WIDX_CASH_PER_GUEST_INCREASE, noMoney);
            setWidgetDisabled(WIDX_CASH_PER_GUEST_DECREASE, noMoney);

            widgets[WIDX_CLOSE].type = gLegacyScene == LegacyScene::scenarioEditor ? WidgetType::empty : WidgetType::closeBox;

            setWidgetPressed(WIDX_HARD_GUEST_GENERATION, gameState.park.flags & PARK_FLAGS_DIFFICULT_GUEST_GENERATION);
        }

        void GuestsDraw(RenderTarget& rt)
        {
            ScreenCoordsXY screenCoords{};

            WindowDrawWidgets(*this, rt);
            DrawTabImages(rt);

            const auto& gameState = getGameState();
            const auto wColour2 = colours[1];

            const auto& cashPerGuestWidget = widgets[WIDX_CASH_PER_GUEST];
            if (cashPerGuestWidget.type != WidgetType::empty)
            {
                // Cash per guest value
                screenCoords = windowPos + ScreenCoordsXY{ cashPerGuestWidget.left + 1, cashPerGuestWidget.top };
                auto ft = Formatter();
                ft.Add<money64>(gameState.scenarioOptions.guestInitialCash);
                auto colour = !isWidgetDisabled(WIDX_CASH_PER_GUEST) ? wColour2 : wColour2.withFlag(ColourFlag::inset, true);
                DrawTextBasic(rt, screenCoords, STR_CURRENCY_FORMAT_LABEL, ft, colour);
            }

            auto colour = wColour2;

            // Guest initial happiness value
            const auto& initialHappinessWidget = widgets[WIDX_GUEST_INITIAL_HAPPINESS];
            screenCoords = windowPos + ScreenCoordsXY{ initialHappinessWidget.left + 1, initialHappinessWidget.top };
            auto ft = Formatter();
            ft.Add<uint16_t>((gameState.scenarioOptions.guestInitialHappiness * 100) / 255);
            DrawTextBasic(rt, screenCoords, STR_PERCENT_FORMAT_LABEL, ft, colour);

            // Guest initial hunger value
            const auto& initialHungerWidget = widgets[WIDX_GUEST_INITIAL_HUNGER];
            screenCoords = windowPos + ScreenCoordsXY{ initialHungerWidget.left + 1, initialHungerWidget.top };
            ft = Formatter();
            ft.Add<uint16_t>(((255 - gameState.scenarioOptions.guestInitialHunger) * 100) / 255);
            DrawTextBasic(rt, screenCoords, STR_PERCENT_FORMAT_LABEL, ft, colour);

            // Guest initial thirst value
            const auto& initialThirstWidget = widgets[WIDX_GUEST_INITIAL_THIRST];
            screenCoords = windowPos + ScreenCoordsXY{ initialThirstWidget.left + 1, initialThirstWidget.top };
            ft = Formatter();
            ft.Add<uint16_t>(((255 - gameState.scenarioOptions.guestInitialThirst) * 100) / 255);
            DrawTextBasic(rt, screenCoords, STR_PERCENT_FORMAT_LABEL, ft, colour);

            // Guests' intensity value
            {
                const auto& guestsIntensity = widgets[WIDX_GUESTS_INTENSITY_PREFERENCE];
                screenCoords = windowPos + ScreenCoordsXY{ guestsIntensity.left + 1, guestsIntensity.top };

                const auto preferLess = gameState.park.flags & PARK_FLAGS_PREF_LESS_INTENSE_RIDES;
                const auto preferMore = gameState.park.flags & PARK_FLAGS_PREF_MORE_INTENSE_RIDES;

                StringId prefString = STR_GUESTS_PREFER_INTENSITY_BALANCED;
                if (preferLess && preferMore)
                    prefString = STR_GUESTS_PREFER_INTENSITY_NONE;
                else if (preferLess && !preferMore)
                    prefString = STR_GUESTS_PREFER_INTENSITY_LESS_INTENSE_RIDES;
                else if (!preferLess && preferMore)
                    prefString = STR_GUESTS_PREFER_INTENSITY_MORE_INTENSE_RIDES;

                ft = Formatter();
                ft.Add<StringId>(prefString);
                DrawTextBasic(rt, screenCoords, STR_WINDOW_COLOUR_2_STRINGID, ft);
            }
        }

#pragma endregion

#pragma region Land

        void LandMouseUp(WidgetIndex widgetIndex)
        {
            auto& gameState = getGameState();
            switch (widgetIndex)
            {
                case WIDX_FORBID_TREE_REMOVAL:
                {
                    auto scenarioSetSetting = GameActions::ScenarioSetSettingAction(
                        GameActions::ScenarioSetSetting::ForbidTreeRemoval,
                        gameState.park.flags & PARK_FLAGS_FORBID_TREE_REMOVAL ? 0 : 1);
                    GameActions::Execute(&scenarioSetSetting, gameState);
                    invalidate();
                    break;
                }
                case WIDX_FORBID_LANDSCAPE_CHANGES:
                {
                    auto scenarioSetSetting = GameActions::ScenarioSetSettingAction(
                        GameActions::ScenarioSetSetting::ForbidLandscapeChanges,
                        gameState.park.flags & PARK_FLAGS_FORBID_LANDSCAPE_CHANGES ? 0 : 1);
                    GameActions::Execute(&scenarioSetSetting, gameState);
                    invalidate();
                    break;
                }
                case WIDX_FORBID_HIGH_CONSTRUCTION:
                {
                    auto scenarioSetSetting = GameActions::ScenarioSetSettingAction(
                        GameActions::ScenarioSetSetting::ForbidHighConstruction,
                        gameState.park.flags & PARK_FLAGS_FORBID_HIGH_CONSTRUCTION ? 0 : 1);
                    GameActions::Execute(&scenarioSetSetting, gameState);
                    invalidate();
                    break;
                }
            }
        }

        void LandResize()
        {
            WindowSetResize(*this, kSizeLand, kSizeLand);
        }

        void LandMouseDown(WidgetIndex widgetIndex)
        {
            auto& gameState = getGameState();
            auto& scenarioOptions = gameState.scenarioOptions;

            switch (widgetIndex)
            {
                case WIDX_LAND_COST_INCREASE:
                    if (scenarioOptions.landPrice < 200.00_GBP)
                    {
                        auto scenarioSetSetting = GameActions::ScenarioSetSettingAction(
                            GameActions::ScenarioSetSetting::CostToBuyLand, scenarioOptions.landPrice + 1.00_GBP);
                        GameActions::Execute(&scenarioSetSetting, gameState);
                    }
                    else
                    {
                        ContextShowError(STR_CANT_INCREASE_FURTHER, kStringIdNone, {});
                    }
                    invalidate();
                    break;
                case WIDX_LAND_COST_DECREASE:
                    if (scenarioOptions.landPrice > 5.00_GBP)
                    {
                        auto scenarioSetSetting = GameActions::ScenarioSetSettingAction(
                            GameActions::ScenarioSetSetting::CostToBuyLand, scenarioOptions.landPrice - 1.00_GBP);
                        GameActions::Execute(&scenarioSetSetting, gameState);
                    }
                    else
                    {
                        ContextShowError(STR_CANT_REDUCE_FURTHER, kStringIdNone, {});
                    }
                    invalidate();
                    break;
                case WIDX_CONSTRUCTION_RIGHTS_COST_INCREASE:
                    if (scenarioOptions.constructionRightsPrice < 200.00_GBP)
                    {
                        auto scenarioSetSetting = GameActions::ScenarioSetSettingAction(
                            GameActions::ScenarioSetSetting::CostToBuyConstructionRights,
                            scenarioOptions.constructionRightsPrice + 1.00_GBP);
                        GameActions::Execute(&scenarioSetSetting, gameState);
                    }
                    else
                    {
                        ContextShowError(STR_CANT_INCREASE_FURTHER, kStringIdNone, {});
                    }
                    invalidate();
                    break;
                case WIDX_CONSTRUCTION_RIGHTS_COST_DECREASE:
                    if (scenarioOptions.constructionRightsPrice > 5.00_GBP)
                    {
                        auto scenarioSetSetting = GameActions::ScenarioSetSettingAction(
                            GameActions::ScenarioSetSetting::CostToBuyConstructionRights,
                            scenarioOptions.constructionRightsPrice - 1.00_GBP);
                        GameActions::Execute(&scenarioSetSetting, gameState);
                    }
                    else
                    {
                        ContextShowError(STR_CANT_REDUCE_FURTHER, kStringIdNone, {});
                    }
                    invalidate();
                    break;
            }
        }

        void LandUpdate()
        {
            currentFrame++;
            invalidateWidget(WIDX_TAB_3);
        }

        void LandPrepareDraw()
        {
            SetPressedTab();

            auto& gameState = getGameState();
            bool noMoney = gameState.park.flags & PARK_FLAGS_NO_MONEY;

            setWidgetDisabled(WIDX_LAND_COST_LABEL, noMoney);
            setWidgetDisabled(WIDX_LAND_COST, noMoney);
            setWidgetDisabled(WIDX_LAND_COST_INCREASE, noMoney);
            setWidgetDisabled(WIDX_LAND_COST_DECREASE, noMoney);
            setWidgetDisabled(WIDX_CONSTRUCTION_RIGHTS_COST_LABEL, noMoney);
            setWidgetDisabled(WIDX_CONSTRUCTION_RIGHTS_COST, noMoney);
            setWidgetDisabled(WIDX_CONSTRUCTION_RIGHTS_COST_INCREASE, noMoney);
            setWidgetDisabled(WIDX_CONSTRUCTION_RIGHTS_COST_DECREASE, noMoney);

            setWidgetPressed(WIDX_FORBID_TREE_REMOVAL, gameState.park.flags & PARK_FLAGS_FORBID_TREE_REMOVAL);
            setWidgetPressed(WIDX_FORBID_LANDSCAPE_CHANGES, gameState.park.flags & PARK_FLAGS_FORBID_LANDSCAPE_CHANGES);
            setWidgetPressed(WIDX_FORBID_HIGH_CONSTRUCTION, gameState.park.flags & PARK_FLAGS_FORBID_HIGH_CONSTRUCTION);

            widgets[WIDX_CLOSE].type = gLegacyScene == LegacyScene::scenarioEditor ? WidgetType::empty : WidgetType::closeBox;
        }

        void LandDraw(RenderTarget& rt)
        {
            ScreenCoordsXY screenCoords{};

            WindowDrawWidgets(*this, rt);
            DrawTabImages(rt);

            const auto& gameState = getGameState();
            const auto wColour2 = colours[1];

            const auto& landCostWidget = widgets[WIDX_LAND_COST];
            if (landCostWidget.type != WidgetType::empty)
            {
                // Cost to buy land value
                screenCoords = windowPos + ScreenCoordsXY{ landCostWidget.left + 1, landCostWidget.top };
                auto ft = Formatter();
                ft.Add<money64>(gameState.scenarioOptions.landPrice);
                auto colour = !isWidgetDisabled(WIDX_LAND_COST) ? wColour2 : wColour2.withFlag(ColourFlag::inset, true);
                DrawTextBasic(rt, screenCoords, STR_CURRENCY_FORMAT_LABEL, ft, colour);
            }

            const auto& constructionRightsCostWidget = widgets[WIDX_CONSTRUCTION_RIGHTS_COST];
            if (constructionRightsCostWidget.type != WidgetType::empty)
            {
                // Cost to buy construction rights value
                screenCoords = windowPos
                    + ScreenCoordsXY{ constructionRightsCostWidget.left + 1, constructionRightsCostWidget.top };
                auto ft = Formatter();
                ft.Add<money64>(gameState.scenarioOptions.constructionRightsPrice);
                auto colour = !isWidgetDisabled(WIDX_CONSTRUCTION_RIGHTS_COST) ? wColour2
                                                                               : wColour2.withFlag(ColourFlag::inset, true);
                DrawTextBasic(rt, screenCoords, STR_CURRENCY_FORMAT_LABEL, ft, colour);
            }
        }

#pragma endregion

#pragma region Rides

        /**
         *
         *  rct2: 0x006725A8
         */
        void RidesOnResize()
        {
            WindowSetResize(*this, kSizeRides, kSizeRides);
        }

        static std::string GetRideString(RideId rideId)
        {
            const auto* ride = GetRide(rideId);
            if (ride == nullptr)
                return {};
            return ride->getName();
        }

        /**
         *
         *  rct2: 0x00672544
         */
        void RidesOnUpdate()
        {
            currentFrame++;
            onPrepareDraw();
            onResize();
            invalidateWidget(WIDX_TAB_2);

            const auto oldSize = _rideableRides.size();
            _rideableRides.clear();

            const auto& gameState = getGameState();
            for (auto& currentRide : RideManager(gameState))
            {
                if (currentRide.isRide())
                {
                    _rideableRides.push_back(currentRide.id);
                }
            }

            // Sort the rides alphabetically
            std::sort(_rideableRides.begin(), _rideableRides.end(), [](const RideId& a, const RideId& b) {
                return String::compare(GetRideString(a), GetRideString(b), false) < 0;
            });

            if (oldSize != _rideableRides.size())
            {
                invalidate();
            }
        }

        /**
         *
         *  rct2: 0x006724BF
         */
        ScreenSize RidesOnScrollGetSize(int32_t scrollIndex)
        {
            ScreenSize newSize;
            newSize.height = static_cast<int32_t>(_rideableRides.size()) * 10;

            return newSize;
        }

        /**
         *
         *  rct2: 0x006724FC
         */
        void RidesOnScrollMouseDown(int32_t scrollIndex, const ScreenCoordsXY& screenCoords)
        {
            auto i = screenCoords.y / 12;
            if (i < 0 || i >= static_cast<int32_t>(_rideableRides.size()))
                return;

            auto* currentRide = GetRide(_rideableRides[i]);
            if (currentRide != nullptr)
            {
                currentRide->lifecycleFlags ^= RIDE_LIFECYCLE_INDESTRUCTIBLE;
            }
            invalidate();
        }

        /**
         *
         *  rct2: 0x006724CC
         */
        void RidesOnScrollMouseOver(int32_t scrollIndex, const ScreenCoordsXY& screenCoords)
        {
            int32_t i;

            i = screenCoords.y / 12;
            if (i < 0 || i >= static_cast<int32_t>(_rideableRides.size()))
                return;

            if (selectedListItem != i)
            {
                selectedListItem = i;
                invalidate();
            }
        }

        /**
         *
         *  rct2: 0x006722B5
         */
        void RidesOnPrepareDraw()
        {
            SetPressedTab();

            widgets[WIDX_CLOSE].type = gLegacyScene == LegacyScene::scenarioEditor ? WidgetType::empty : WidgetType::closeBox;
        }

        /**
         *
         *  rct2: 0x00672340
         */
        void RidesOnDraw(RenderTarget& rt)
        {
            drawWidgets(rt);
            DrawTabImages(rt);
        }

        /**
         *
         *  rct2: 0x0067236F
         */
        void RidesOnScrollDraw(RenderTarget& rt, int32_t scrollIndex)
        {
            auto colour = getColourMap(colours[1].colour).midLight;
            Rectangle::fill(rt, { { rt.x, rt.y }, { rt.x + rt.width - 1, rt.y + rt.height - 1 } }, colour);

            for (int32_t i = 0; i < static_cast<int32_t>(_rideableRides.size()); i++)
            {
                int32_t y = i * 12;

                if (y + 12 < rt.y || y >= rt.y + rt.height)
                    continue;

                // Checkbox
                Rectangle::fillInset(
                    rt, { { 2, y }, { 11, y + 10 } }, colours[1], Rectangle::BorderStyle::inset,
                    Rectangle::FillBrightness::dark, Rectangle::FillMode::dontLightenWhenInset);

                // Highlighted
                auto stringId = STR_BLACK_STRING;
                if (i == selectedListItem)
                {
                    stringId = STR_WINDOW_COLOUR_2_STRINGID;
                    Rectangle::filter(rt, { 0, y, width, y + 11 }, FilterPaletteID::paletteDarken1);
                }

                // Checkbox mark
                auto* currentRide = GetRide(_rideableRides[i]);
                if (currentRide != nullptr)
                {
                    if (currentRide->lifecycleFlags & RIDE_LIFECYCLE_INDESTRUCTIBLE)
                    {
                        auto darkness = stringId == STR_WINDOW_COLOUR_2_STRINGID ? TextDarkness::extraDark : TextDarkness::dark;
                        DrawText(
                            rt, { 2, y }, { colours[1].withFlag(ColourFlag::translucent, false), FontStyle::medium, darkness },
                            kCheckMarkString);
                    }

                    // Ride name

                    Formatter ft;
                    currentRide->formatNameTo(ft);
                    DrawTextBasic(rt, { 15, y }, stringId, ft);
                }
            }
        }

#pragma endregion
    };

    WindowBase* EditorScenarioOptionsOpen()
    {
        auto* windowMgr = GetWindowManager();
        return windowMgr->FocusOrCreate<EditorScenarioOptionsWindow>(
            WindowClass::editorScenarioOptions, { 280, 148 }, { WindowFlag::noScrolling, WindowFlag::centreScreen });
    }
} // namespace OpenRCT2::Ui::Windows
