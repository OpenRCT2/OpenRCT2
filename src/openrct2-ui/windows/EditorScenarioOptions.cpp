/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
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
#include <openrct2/drawing/Drawing.h>
#include <openrct2/drawing/Font.h>
#include <openrct2/drawing/Text.h>
#include <openrct2/entity/Peep.h>
#include <openrct2/interface/Colour.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/localisation/Language.h>
#include <openrct2/localisation/Localisation.Date.h>
#include <openrct2/management/Finance.h>
#include <openrct2/ride/Ride.h>
#include <openrct2/ride/RideManager.hpp>
#include <openrct2/ui/WindowManager.h>
#include <openrct2/world/Park.h>

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

    static constexpr ride_rating kObjectiveExcitementMax = MakeRideRating(9, 90);
    static constexpr ride_rating kObjectiveExcitementMin = MakeRideRating(4, 00);
    static constexpr ride_rating kObjectiveExcitementAdjustment = MakeRideRating(0, 10);

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
        WIDX_OBJECTIVE = WIDX_PAGE_START,
        WIDX_OBJECTIVE_DROPDOWN,
        WIDX_OBJECTIVE_ARG_1,
        WIDX_OBJECTIVE_ARG_1_INCREASE,
        WIDX_OBJECTIVE_ARG_1_DECREASE,
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
            makeWindowShim(title, size.width, size.height),
            makeWidget({  0, 43}, { size.width, 106 }, WindowWidgetType::Resize, WindowColour::Secondary),
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
        makeWidget        ({ 98,  48}, {344,  12}, WindowWidgetType::DropdownMenu, WindowColour::Secondary, kStringIdNone,           STR_SELECT_OBJECTIVE_FOR_THIS_SCENARIO_TIP     ),
        makeWidget        ({430,  49}, { 11,  10}, WindowWidgetType::Button,   WindowColour::Secondary, STR_DROPDOWN_GLYPH, STR_SELECT_OBJECTIVE_FOR_THIS_SCENARIO_TIP     ),
        makeSpinnerWidgets({158,  65}, {120,  12}, WindowWidgetType::Button,   WindowColour::Secondary                                                                     ), // NB: 3 widgets
        makeSpinnerWidgets({158,  82}, {120,  12}, WindowWidgetType::Button,   WindowColour::Secondary                                                                     ), // NB: 3 widgets
        makeWidget        ({ 14,  99}, {340,  12}, WindowWidgetType::Checkbox, WindowColour::Secondary, STR_HARD_PARK_RATING,   STR_HARD_PARK_RATING_TIP                   )
    );

    static constexpr auto window_editor_scenario_options_scenario_details_widgets = makeWidgets(
        makeOptionsWidgets(STR_SCENARIO_OPTIONS_SCENARIO_DETAILS, kSizeScenarioDetails),
        makeWidget        ({370, 48}, { 75,  12}, WindowWidgetType::Button,   WindowColour::Secondary, STR_CHANGE,         STR_CHANGE_NAME_OF_PARK_TIP                    ),
        makeWidget        ({370, 65}, { 75,  12}, WindowWidgetType::Button,   WindowColour::Secondary, STR_CHANGE,         STR_CHANGE_NAME_OF_SCENARIO_TIP                ),
        makeWidget        ({ 98, 82}, {180,  12}, WindowWidgetType::DropdownMenu, WindowColour::Secondary, kStringIdNone,           STR_SELECT_WHICH_GROUP_THIS_SCENARIO_APPEARS_IN),
        makeWidget        ({266, 83}, { 11,  10}, WindowWidgetType::Button,   WindowColour::Secondary, STR_DROPDOWN_GLYPH, STR_SELECT_WHICH_GROUP_THIS_SCENARIO_APPEARS_IN),
        makeWidget        ({370, 99}, { 75,  12}, WindowWidgetType::Button,   WindowColour::Secondary, STR_CHANGE,         STR_CHANGE_DETAIL_NOTES_ABOUT_PARK_SCENARIO_TIP)
    );

    static constexpr auto window_editor_scenario_options_financial_widgets = makeWidgets(
        makeOptionsWidgets(STR_SCENARIO_OPTIONS_FINANCIAL, kSizeFinancial),
        makeWidget        ({  8,  48}, {kSizeFinancial.width - 16, 12}, WindowWidgetType::Checkbox, WindowColour::Secondary, STR_MAKE_PARK_NO_MONEY, STR_MAKE_PARK_NO_MONEY_TIP        ),
        makeWidget        ({  5,  63}, {kSizeFinancial.width - 10, 67}, WindowWidgetType::Groupbox, WindowColour::Secondary, STR_GROUP_LOAN_OPTIONS                                    ),
        makeWidget        ({  9,  77}, {                      250, 12}, WindowWidgetType::Label,    WindowColour::Secondary, STR_INIT_LOAN_LABEL                                        ),
        makeSpinnerWidgets({268,  77}, {                      100, 12}, WindowWidgetType::Spinner,  WindowColour::Secondary                                                            ), // NB: 3 widgets
        makeWidget        ({  9,  94}, {                      250, 12}, WindowWidgetType::Label,    WindowColour::Secondary, STR_MAX_LOAN_LABEL                                        ),
        makeSpinnerWidgets({268,  94}, {                      100, 12}, WindowWidgetType::Spinner,  WindowColour::Secondary                                                            ), // NB: 3 widgets
        makeWidget        ({  9, 111}, {                      280, 12}, WindowWidgetType::Label,    WindowColour::Secondary, STR_INTEREST_RATE_LABEL                                        ),
        makeSpinnerWidgets({298, 111}, {                       70, 12}, WindowWidgetType::Spinner,  WindowColour::Secondary                                                            ), // NB: 3 widgets
        makeWidget        ({ 10, 111}, {kSizeFinancial.width - 16, 12}, WindowWidgetType::Checkbox, WindowColour::Secondary, STR_RCT1_INTEREST,      STR_RCT1_INTEREST_TIP             ),
        makeWidget        ({  5, 133}, {kSizeFinancial.width - 10, 87}, WindowWidgetType::Groupbox, WindowColour::Secondary, STR_GROUP_BUSINESS_MODEL                                  ),
        makeWidget        ({  9, 150}, {                      250, 12}, WindowWidgetType::Label,    WindowColour::Secondary, STR_INIT_CASH_LABEL                                        ),
        makeSpinnerWidgets({268, 150}, {                      100, 12}, WindowWidgetType::Spinner,  WindowColour::Secondary                                                            ), // NB: 3 widgets
        makeWidget        ({  9, 167}, {                      150, 12}, WindowWidgetType::Label,    WindowColour::Secondary, STR_EARNINGS_LABEL                                        ),
        makeWidget        ({158, 167}, {                      210, 12}, WindowWidgetType::DropdownMenu, WindowColour::Secondary, kStringIdNone,      STR_PAY_FOR_PARK_PAY_FOR_RIDES_TIP),
        makeWidget        ({356, 168}, {                       11, 10}, WindowWidgetType::Button,   WindowColour::Secondary, STR_DROPDOWN_GLYPH,     STR_PAY_FOR_PARK_PAY_FOR_RIDES_TIP),
        makeWidget        ({  9, 184}, {                      280, 12}, WindowWidgetType::Label,    WindowColour::Secondary, STR_ENTRY_PRICE_LABEL                                        ),
        makeSpinnerWidgets({298, 184}, {                       70, 12}, WindowWidgetType::Spinner,  WindowColour::Secondary                                                            ), // NB: 3 widgets
        makeWidget        ({ 10, 201}, {kSizeFinancial.width - 16, 12}, WindowWidgetType::Checkbox, WindowColour::Secondary, STR_FORBID_MARKETING,   STR_FORBID_MARKETING_TIP          )
    );

    static constexpr auto window_editor_scenario_options_guests_widgets = makeWidgets(
        makeOptionsWidgets(STR_SCENARIO_OPTIONS_GUESTS, kSizeGuests),
        makeWidget        ({  8,  48}, {      280,  12}, WindowWidgetType::Label,        WindowColour::Secondary, STR_CASH_PER_GUEST_LABEL                                                ),
        makeSpinnerWidgets({298,  48}, {       70,  12}, WindowWidgetType::Spinner,      WindowColour::Secondary                                                                          ), // NB: 3 widgets
        makeWidget        ({  8,  65}, {      280,  12}, WindowWidgetType::Label,        WindowColour::Secondary, STR_GUEST_INIT_HAPPINESS                                                ),
        makeSpinnerWidgets({298,  65}, {       70,  12}, WindowWidgetType::Spinner,      WindowColour::Secondary                                                                          ), // NB: 3 widgets
        makeWidget        ({  8,  82}, {      280,  12}, WindowWidgetType::Label,        WindowColour::Secondary, STR_GUEST_INIT_HUNGER                                                   ),
        makeSpinnerWidgets({298,  82}, {       70,  12}, WindowWidgetType::Spinner,      WindowColour::Secondary                                                                          ), // NB: 3 widgets
        makeWidget        ({  8,  99}, {      280,  12}, WindowWidgetType::Label,        WindowColour::Secondary, STR_GUEST_INIT_THIRST                                                   ),
        makeSpinnerWidgets({298,  99}, {       70,  12}, WindowWidgetType::Spinner,      WindowColour::Secondary                                                                          ), // NB: 3 widgets
        makeWidget        ({  8, 116}, {      180,  12}, WindowWidgetType::Label,        WindowColour::Secondary, STR_GUESTS_PREFER_INTENSITY_LABEL                                       ),
        makeWidget        ({198, 116}, {      170,  12}, WindowWidgetType::DropdownMenu, WindowColour::Secondary, kStringIdNone,                        STR_GUESTS_PREFER_INTENSITY_TIP   ),
        makeWidget        ({357, 117}, {       11,  10}, WindowWidgetType::Button,       WindowColour::Secondary, STR_DROPDOWN_GLYPH,                   STR_GUESTS_PREFER_INTENSITY_TIP   ),
        makeWidget        ({  8, 133}, {      350,  12}, WindowWidgetType::Checkbox,     WindowColour::Secondary, STR_HARD_GUEST_GENERATION,            STR_HARD_GUEST_GENERATION_TIP     )
    );

    static constexpr auto window_editor_scenario_options_land_widgets = makeWidgets(
        makeOptionsWidgets(STR_SCENARIO_OPTIONS_LAND_RESTRICTIONS, kSizeLand),
        makeWidget        ({  8,  48}, {                 170,  12}, WindowWidgetType::Label,    WindowColour::Secondary, STR_LAND_COST_LABEL                                             ),
        makeSpinnerWidgets({188,  48}, {                  70,  12}, WindowWidgetType::Spinner,  WindowColour::Secondary                                                                  ), // NB: 3 widgets
        makeWidget        ({  8,  65}, {                 170,  12}, WindowWidgetType::Label,    WindowColour::Secondary, STR_RIGHTS_COST_LABEL                                           ),
        makeSpinnerWidgets({188,  65}, {                  70,  12}, WindowWidgetType::Spinner,  WindowColour::Secondary                                                                  ), // NB: 3 widgets
        makeWidget        ({  8,  82}, {kSizeLand.width - 16,  12}, WindowWidgetType::Checkbox, WindowColour::Secondary, STR_FORBID_TREE_REMOVAL,      STR_FORBID_TREE_REMOVAL_TIP       ),
        makeWidget        ({  8,  99}, {kSizeLand.width - 16,  12}, WindowWidgetType::Checkbox, WindowColour::Secondary, STR_FORBID_LANDSCAPE_CHANGES, STR_FORBID_LANDSCAPE_CHANGES_TIP  ),
        makeWidget        ({  8, 116}, {kSizeLand.width - 16,  12}, WindowWidgetType::Checkbox, WindowColour::Secondary, STR_FORBID_HIGH_CONSTRUCTION, STR_FORBID_HIGH_CONSTRUCTION_TIP  )
    );

    static constexpr auto window_editor_scenario_options_rides_widgets = makeWidgets(
        makeOptionsWidgets(STR_SCENARIO_OPTIONS_PRESERVED_RIDES, kSizeRides),
        makeWidget({  6,  46}, {kSizeRides.width - 12,  12}, WindowWidgetType::Label,  WindowColour::Secondary, STR_WINDOW_PRESERVATION_ORDER),
        makeWidget({  3,  60}, {                  374, 161}, WindowWidgetType::Scroll, WindowColour::Secondary, SCROLL_VERTICAL              )
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

    static uint64_t window_editor_scenario_options_page_hold_down_widgets[] = {
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
        void OnOpen() override
        {
            SetPage(WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_OBJECTIVE);
        }

        void OnMouseUp(WidgetIndex widgetIndex) override
        {
            switch (widgetIndex)
            {
                case WIDX_CLOSE:
                    Close();
                    return;
                case WIDX_TAB_1:
                case WIDX_TAB_2:
                case WIDX_TAB_3:
                case WIDX_TAB_4:
                case WIDX_TAB_5:
                case WIDX_TAB_6:
                    SetPage(widgetIndex - WIDX_TAB_1);
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

        void OnResize() override
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

        void OnMouseDown(WidgetIndex widgetIndex) override
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

        void OnUpdate() override
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

        void OnPrepareDraw() override
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

        void OnDraw(RenderTarget& rt) override
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

        void OnDropdown(WidgetIndex widgetIndex, int32_t selectedIndex) override
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

        void OnTextInput(WidgetIndex widgetIndex, std::string_view text) override
        {
            if (page == WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_SCENARIO_DETAILS)
            {
                ScenarioDetailsOnTextInput(widgetIndex, text);
            }
        }

        void OnScrollMouseDown(int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
        {
            if (page == WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_RIDES)
            {
                RidesOnScrollMouseDown(scrollIndex, screenCoords);
            }
        }

        void OnScrollMouseOver(int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
        {
            if (page == WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_RIDES)
            {
                RidesOnScrollMouseOver(scrollIndex, screenCoords);
            }
        }

        void OnScrollDraw(int32_t scrollIndex, RenderTarget& rt) override
        {
            if (page == WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_RIDES)
            {
                RidesOnScrollDraw(rt, scrollIndex);
            }
        }

        ScreenSize OnScrollGetSize(int32_t scrollIndex) override
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
            const auto& rideManager = GetRideManager();
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
            SetWidgetDisabled(WIDX_TAB_1, !isObjectiveSelection);
            SetWidgetDisabled(WIDX_TAB_2, !isScenarioDetails);
            for (auto i = 2; i < WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_COUNT; i++)
                SetWidgetDisabled(WIDX_TAB_1 + i, !isOtherTab);

            SetWidgetDisabled(WIDX_TAB_6, !(isOtherTab && AnyRidesExist()));

            // Reposition tabs based on availability so there are no gaps
            widgets[WIDX_TAB_1].left = 3;
            widgets[WIDX_TAB_1].right = widgets[WIDX_TAB_1].left + 30;
            WindowAlignTabs(this, WIDX_TAB_1, WIDX_TAB_6);

            // Switch tabs if our current tab has become unavailable
            if (IsWidgetDisabled(WIDX_TAB_1 + page))
            {
                if (isObjectiveSelection)
                    SetPage(WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_OBJECTIVE);
                else if (isScenarioDetails)
                    SetPage(WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_SCENARIO_DETAILS);
                else
                    SetPage(WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_FINANCIAL);
            }
        }

        void SetPressedTab()
        {
            int32_t i;
            for (i = 0; i < WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_COUNT; i++)
                SetWidgetPressed(WIDX_TAB_1 + i, false);
            SetWidgetPressed(WIDX_TAB_1 + page, true);
        }

        void DrawTabImages(RenderTarget& rt)
        {
            Widget* widget;
            int32_t spriteIndex;

            // Tab 1
            if (!IsWidgetDisabled(WIDX_TAB_1))
            {
                widget = &widgets[WIDX_TAB_1];
                spriteIndex = SPR_TAB_OBJECTIVE_0;
                if (page == WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_OBJECTIVE)
                    spriteIndex += (frame_no / 4) % 16;

                GfxDrawSprite(rt, ImageId(spriteIndex), windowPos + ScreenCoordsXY{ widget->left, widget->top });
            }

            // Tab 2
            if (!IsWidgetDisabled(WIDX_TAB_2))
            {
                widget = &widgets[WIDX_TAB_2];
                spriteIndex = SPR_TAB_KIOSKS_AND_FACILITIES_0;
                if (page == WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_SCENARIO_DETAILS)
                    spriteIndex += (frame_no / 4) % 8;

                GfxDrawSprite(rt, ImageId(spriteIndex), windowPos + ScreenCoordsXY{ widget->left, widget->top });
            }

            // Tab 3
            if (!IsWidgetDisabled(WIDX_TAB_3))
            {
                widget = &widgets[WIDX_TAB_3];
                spriteIndex = SPR_TAB_FINANCES_SUMMARY_0;
                if (page == WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_FINANCIAL)
                    spriteIndex += (frame_no / 2) % 8;

                GfxDrawSprite(rt, ImageId(spriteIndex), windowPos + ScreenCoordsXY{ widget->left, widget->top });
            }

            // Tab 4
            if (!IsWidgetDisabled(WIDX_TAB_4))
            {
                widget = &widgets[WIDX_TAB_4];
                spriteIndex = SPR_TAB_GUESTS_0;
                if (page == WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_GUESTS)
                    spriteIndex += (frame_no / 4) % 8;

                GfxDrawSprite(rt, ImageId(spriteIndex), windowPos + ScreenCoordsXY{ widget->left, widget->top });
            }

            // Tab 5
            if (!IsWidgetDisabled(WIDX_TAB_5))
            {
                widget = &widgets[WIDX_TAB_5];
                spriteIndex = SPR_G2_MAP_GEN_TERRAIN_TAB;
                GfxDrawSprite(rt, ImageId(spriteIndex), windowPos + ScreenCoordsXY{ widget->left, widget->top });
            }

            // Tab 6
            if (!IsWidgetDisabled(WIDX_TAB_6))
            {
                widget = &widgets[WIDX_TAB_6];
                spriteIndex = SPR_TAB_RIDE_0;
                if (page == WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_RIDES)
                    spriteIndex += (frame_no / 4) % 16;

                GfxDrawSprite(rt, ImageId(spriteIndex), windowPos + ScreenCoordsXY{ widget->left, widget->top });
            }
        }

        void SetPage(int32_t newPage)
        {
            // Skip setting page if we're already on this page, unless we're initialising the window
            if (page == newPage && !widgets.empty())
                return;

            page = newPage;
            frame_no = 0;
            disabled_widgets = 0;
            hold_down_widgets = window_editor_scenario_options_page_hold_down_widgets[page];
            pressed_widgets = 0;

            SetWidgets(window_editor_scenario_options_widgets[page]);
            Invalidate();
            OnResize();
            OnPrepareDraw();
            InitScrollWidgets();
            Invalidate();
        }

#pragma region Objective

        /**
         *
         *  rct2: 0x0067201D
         */
        void SetObjective(int32_t objective)
        {
            auto& gameState = getGameState();
            gameState.scenarioObjective.Type = objective;
            Invalidate();

            // Set default objective arguments
            switch (objective)
            {
                case OBJECTIVE_NONE:
                case OBJECTIVE_HAVE_FUN:
                case OBJECTIVE_BUILD_THE_BEST:
                case OBJECTIVE_10_ROLLERCOASTERS:
                    break;
                case OBJECTIVE_GUESTS_BY:
                    gameState.scenarioObjective.Year = 3;
                    gameState.scenarioObjective.NumGuests = 1500;
                    break;
                case OBJECTIVE_PARK_VALUE_BY:
                    gameState.scenarioObjective.Year = 3;
                    gameState.scenarioObjective.Currency = 50000.00_GBP;
                    break;
                case OBJECTIVE_GUESTS_AND_RATING:
                    gameState.scenarioObjective.NumGuests = 2000;
                    break;
                case OBJECTIVE_MONTHLY_RIDE_INCOME:
                    gameState.scenarioObjective.Currency = 10000.00_GBP;
                    break;
                case OBJECTIVE_10_ROLLERCOASTERS_LENGTH:
                    gameState.scenarioObjective.MinimumLength = 1200;
                    break;
                case OBJECTIVE_FINISH_5_ROLLERCOASTERS:
                    gameState.scenarioObjective.MinimumExcitement = MakeRideRating(6, 70);
                    break;
                case OBJECTIVE_REPAY_LOAN_AND_PARK_VALUE:
                    gameState.scenarioObjective.Currency = 50000.00_GBP;
                    break;
                case OBJECTIVE_MONTHLY_FOOD_INCOME:
                    gameState.scenarioObjective.Currency = 1000.00_GBP;
                    break;
            }
        }

        void ShowObjectiveDropdown()
        {
            const auto& gameState = getGameState();
            int32_t numItems = 0, objectiveType;
            Widget* dropdownWidget;

            dropdownWidget = &widgets[WIDX_OBJECTIVE];

            for (auto i = 0; i < OBJECTIVE_COUNT; i++)
            {
                if (i == OBJECTIVE_NONE || i == OBJECTIVE_BUILD_THE_BEST)
                    continue;

                const bool objectiveAllowedByMoneyUsage = !(gameState.park.Flags & PARK_FLAGS_NO_MONEY)
                    || !ObjectiveNeedsMoney(i);
                // This objective can only work if the player can ask money for rides.
                const bool objectiveAllowedByPaymentSettings = (i != OBJECTIVE_MONTHLY_RIDE_INCOME)
                    || Park::RidePricesUnlocked();
                if (objectiveAllowedByMoneyUsage && objectiveAllowedByPaymentSettings)
                {
                    gDropdownItems[numItems].Format = STR_DROPDOWN_MENU_LABEL;
                    gDropdownItems[numItems].Args = ObjectiveDropdownOptionNames[i];
                    numItems++;
                }
            }

            WindowDropdownShowTextCustomWidth(
                { windowPos.x + dropdownWidget->left, windowPos.y + dropdownWidget->top }, dropdownWidget->height() + 1,
                colours[1], 0, Dropdown::Flag::StayOpen, numItems, dropdownWidget->width() - 3);

            objectiveType = gameState.scenarioObjective.Type;
            for (int32_t j = 0; j < numItems; j++)
            {
                if (gDropdownItems[j].Args - STR_OBJECTIVE_DROPDOWN_NONE == objectiveType)
                {
                    Dropdown::SetChecked(j, true);
                    break;
                }
            }
        }

        void ShowCategoryDropdown()
        {
            int32_t i;
            Widget* dropdownWidget;

            dropdownWidget = &widgets[WIDX_CATEGORY];

            for (i = EnumValue(ScenarioCategory::beginner); i <= EnumValue(ScenarioCategory::other); i++)
            {
                gDropdownItems[i].Format = STR_DROPDOWN_MENU_LABEL;
                gDropdownItems[i].Args = kScenarioCategoryStringIds[i];
            }
            WindowDropdownShowTextCustomWidth(
                { windowPos.x + dropdownWidget->left, windowPos.y + dropdownWidget->top }, dropdownWidget->height() + 1,
                colours[1], 0, Dropdown::Flag::StayOpen, 5, dropdownWidget->width() - 3);
            Dropdown::SetChecked(EnumValue(getGameState().scenarioCategory), true);
        }

        void Arg1Increase()
        {
            auto& gameState = getGameState();
            switch (gameState.scenarioObjective.Type)
            {
                case OBJECTIVE_PARK_VALUE_BY:
                case OBJECTIVE_MONTHLY_RIDE_INCOME:
                case OBJECTIVE_REPAY_LOAN_AND_PARK_VALUE:
                    if (gameState.scenarioObjective.Currency >= kObjectiveCurrencyLoanAndValueMax)
                    {
                        ContextShowError(STR_CANT_INCREASE_FURTHER, kStringIdNone, {});
                    }
                    else
                    {
                        gameState.scenarioObjective.Currency += kObjectiveCurrencyLoanAndValueAdjustment;
                        Invalidate();
                    }
                    break;
                case OBJECTIVE_MONTHLY_FOOD_INCOME:
                    if (gameState.scenarioObjective.Currency >= kObjectiveCurrencyFoodMax)
                    {
                        ContextShowError(STR_CANT_INCREASE_FURTHER, kStringIdNone, {});
                    }
                    else
                    {
                        gameState.scenarioObjective.Currency += kObjectiveCurrencyFoodAdjustment;
                        Invalidate();
                    }
                    break;
                case OBJECTIVE_10_ROLLERCOASTERS_LENGTH:
                    if (gameState.scenarioObjective.MinimumLength >= kObjectiveLengthMax)
                    {
                        ContextShowError(STR_CANT_INCREASE_FURTHER, kStringIdNone, {});
                    }
                    else
                    {
                        gameState.scenarioObjective.MinimumLength += kObjectiveLengthAdjustment;
                        Invalidate();
                    }
                    break;
                case OBJECTIVE_FINISH_5_ROLLERCOASTERS:
                    if (gameState.scenarioObjective.MinimumExcitement >= kObjectiveExcitementMax)
                    {
                        ContextShowError(STR_CANT_INCREASE_FURTHER, kStringIdNone, {});
                    }
                    else
                    {
                        gameState.scenarioObjective.MinimumExcitement += kObjectiveExcitementAdjustment;
                        Invalidate();
                    }
                    break;
                default:
                    if (gameState.scenarioObjective.NumGuests >= kObjectiveGuestsMax)
                    {
                        ContextShowError(STR_CANT_INCREASE_FURTHER, kStringIdNone, {});
                    }
                    else
                    {
                        gameState.scenarioObjective.NumGuests += kObjectiveGuestsAdjustment;
                        Invalidate();
                    }
                    break;
            }
        }

        void Arg1Decrease()
        {
            auto& gameState = getGameState();
            switch (gameState.scenarioObjective.Type)
            {
                case OBJECTIVE_PARK_VALUE_BY:
                case OBJECTIVE_MONTHLY_RIDE_INCOME:
                case OBJECTIVE_REPAY_LOAN_AND_PARK_VALUE:
                    if (gameState.scenarioObjective.Currency <= kObjectiveCurrencyLoanAndValueMin)
                    {
                        ContextShowError(STR_CANT_REDUCE_FURTHER, kStringIdNone, {});
                    }
                    else
                    {
                        gameState.scenarioObjective.Currency -= kObjectiveCurrencyLoanAndValueAdjustment;
                        Invalidate();
                    }
                    break;
                case OBJECTIVE_MONTHLY_FOOD_INCOME:
                    if (gameState.scenarioObjective.Currency <= kObjectiveCurrencyFoodMin)
                    {
                        ContextShowError(STR_CANT_REDUCE_FURTHER, kStringIdNone, {});
                    }
                    else
                    {
                        gameState.scenarioObjective.Currency -= kObjectiveCurrencyFoodAdjustment;
                        Invalidate();
                    }
                    break;
                case OBJECTIVE_10_ROLLERCOASTERS_LENGTH:
                    if (gameState.scenarioObjective.MinimumLength <= kObjectiveLengthMin)
                    {
                        ContextShowError(STR_CANT_REDUCE_FURTHER, kStringIdNone, {});
                    }
                    else
                    {
                        gameState.scenarioObjective.MinimumLength -= kObjectiveLengthAdjustment;
                        Invalidate();
                    }
                    break;
                case OBJECTIVE_FINISH_5_ROLLERCOASTERS:
                    if (gameState.scenarioObjective.MinimumExcitement <= kObjectiveExcitementMin)
                    {
                        ContextShowError(STR_CANT_REDUCE_FURTHER, kStringIdNone, {});
                    }
                    else
                    {
                        gameState.scenarioObjective.MinimumExcitement -= kObjectiveExcitementAdjustment;
                        Invalidate();
                    }
                    break;
                default:
                    if (gameState.scenarioObjective.NumGuests <= kObjectiveGuestsMin)
                    {
                        ContextShowError(STR_CANT_REDUCE_FURTHER, kStringIdNone, {});
                    }
                    else
                    {
                        gameState.scenarioObjective.NumGuests -= kObjectiveGuestsAdjustment;
                        Invalidate();
                    }
                    break;
            }
        }

        void Arg2Increase()
        {
            auto& gameState = getGameState();
            if (gameState.scenarioObjective.Year >= kObjectiveYearMax)
            {
                ContextShowError(STR_CANT_INCREASE_FURTHER, kStringIdNone, {});
            }
            else
            {
                gameState.scenarioObjective.Year += kObjectiveYearAdjustment;
                Invalidate();
            }
        }

        void Arg2Decrease()
        {
            auto& gameState = getGameState();
            if (gameState.scenarioObjective.Year <= kObjectiveYearMin)
            {
                ContextShowError(STR_CANT_REDUCE_FURTHER, kStringIdNone, {});
            }
            else
            {
                gameState.scenarioObjective.Year -= kObjectiveYearAdjustment;
                Invalidate();
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
                    auto scenarioSetSetting = ScenarioSetSettingAction(
                        ScenarioSetSetting::ParkRatingHigherDifficultyLevel,
                        gameState.park.Flags & PARK_FLAGS_DIFFICULT_PARK_RATING ? 0 : 1);
                    GameActions::Execute(&scenarioSetSetting);
                    Invalidate();
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
            uint8_t newObjectiveType;

            if (dropdownIndex == -1)
                return;

            switch (widgetIndex)
            {
                case WIDX_OBJECTIVE_DROPDOWN:
                    // TODO: Don't rely on string ID order
                    newObjectiveType = static_cast<uint8_t>(gDropdownItems[dropdownIndex].Args - STR_OBJECTIVE_DROPDOWN_NONE);
                    if (gameState.scenarioObjective.Type != newObjectiveType)
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
            uint8_t objectiveType;

            frame_no++;
            OnPrepareDraw();
            InvalidateWidget(WIDX_TAB_1);

            objectiveType = getGameState().scenarioObjective.Type;

            // Check if objective is allowed by money and pay-per-ride settings.
            const bool objectiveAllowedByMoneyUsage = !(getGameState().park.Flags & PARK_FLAGS_NO_MONEY)
                || !ObjectiveNeedsMoney(objectiveType);
            // This objective can only work if the player can ask money for rides.
            const bool objectiveAllowedByPaymentSettings = (objectiveType != OBJECTIVE_MONTHLY_RIDE_INCOME)
                || Park::RidePricesUnlocked();
            if (!objectiveAllowedByMoneyUsage || !objectiveAllowedByPaymentSettings)
            {
                // Reset objective
                SetObjective(OBJECTIVE_GUESTS_AND_RATING);
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

            switch (gameState.scenarioObjective.Type)
            {
                case OBJECTIVE_GUESTS_BY:
                case OBJECTIVE_PARK_VALUE_BY:
                    widgets[WIDX_OBJECTIVE_ARG_1].type = WindowWidgetType::Spinner;
                    widgets[WIDX_OBJECTIVE_ARG_1_INCREASE].type = WindowWidgetType::Button;
                    widgets[WIDX_OBJECTIVE_ARG_1_DECREASE].type = WindowWidgetType::Button;
                    widgets[WIDX_OBJECTIVE_ARG_2].type = WindowWidgetType::Spinner;
                    widgets[WIDX_OBJECTIVE_ARG_2_INCREASE].type = WindowWidgetType::Button;
                    widgets[WIDX_OBJECTIVE_ARG_2_DECREASE].type = WindowWidgetType::Button;
                    break;
                case OBJECTIVE_GUESTS_AND_RATING:
                case OBJECTIVE_MONTHLY_RIDE_INCOME:
                case OBJECTIVE_10_ROLLERCOASTERS_LENGTH:
                case OBJECTIVE_FINISH_5_ROLLERCOASTERS:
                case OBJECTIVE_REPAY_LOAN_AND_PARK_VALUE:
                case OBJECTIVE_MONTHLY_FOOD_INCOME:
                    widgets[WIDX_OBJECTIVE_ARG_1].type = WindowWidgetType::Spinner;
                    widgets[WIDX_OBJECTIVE_ARG_1_INCREASE].type = WindowWidgetType::Button;
                    widgets[WIDX_OBJECTIVE_ARG_1_DECREASE].type = WindowWidgetType::Button;
                    widgets[WIDX_OBJECTIVE_ARG_2].type = WindowWidgetType::Empty;
                    widgets[WIDX_OBJECTIVE_ARG_2_INCREASE].type = WindowWidgetType::Empty;
                    widgets[WIDX_OBJECTIVE_ARG_2_DECREASE].type = WindowWidgetType::Empty;
                    break;
                default:
                    widgets[WIDX_OBJECTIVE_ARG_1].type = WindowWidgetType::Empty;
                    widgets[WIDX_OBJECTIVE_ARG_1_INCREASE].type = WindowWidgetType::Empty;
                    widgets[WIDX_OBJECTIVE_ARG_1_DECREASE].type = WindowWidgetType::Empty;
                    widgets[WIDX_OBJECTIVE_ARG_2].type = WindowWidgetType::Empty;
                    widgets[WIDX_OBJECTIVE_ARG_2_INCREASE].type = WindowWidgetType::Empty;
                    widgets[WIDX_OBJECTIVE_ARG_2_DECREASE].type = WindowWidgetType::Empty;
                    break;
            }

            widgets[WIDX_CLOSE].type = gLegacyScene == LegacyScene::scenarioEditor ? WindowWidgetType::Empty
                                                                                   : WindowWidgetType::CloseBox;

            SetWidgetPressed(WIDX_HARD_PARK_RATING, gameState.park.Flags & PARK_FLAGS_DIFFICULT_PARK_RATING);
        }

        /**
         *
         *  rct2: 0x0067161C
         */
        void ObjectiveOnDraw(RenderTarget& rt)
        {
            const auto& gameState = getGameState();
            StringId stringId;

            DrawWidgets(rt);
            DrawTabImages(rt);

            // Objective label
            auto screenCoords = windowPos + ScreenCoordsXY{ 8, widgets[WIDX_OBJECTIVE].top };
            DrawTextBasic(rt, screenCoords, STR_OBJECTIVE_DROPDOWN_LABEL);

            // Objective value
            screenCoords = windowPos + ScreenCoordsXY{ widgets[WIDX_OBJECTIVE].left + 1, widgets[WIDX_OBJECTIVE].top };
            auto ft = Formatter();
            ft.Add<StringId>(ObjectiveDropdownOptionNames[gameState.scenarioObjective.Type]);
            DrawTextBasic(rt, screenCoords, STR_WINDOW_COLOUR_2_STRINGID, ft);

            if (widgets[WIDX_OBJECTIVE_ARG_1].type != WindowWidgetType::Empty)
            {
                // Objective argument 1 label
                screenCoords = windowPos + ScreenCoordsXY{ 28, widgets[WIDX_OBJECTIVE_ARG_1].top };
                switch (gameState.scenarioObjective.Type)
                {
                    case OBJECTIVE_GUESTS_BY:
                    case OBJECTIVE_GUESTS_AND_RATING:
                        stringId = STR_WINDOW_OBJECTIVE_GUEST_COUNT;
                        break;
                    case OBJECTIVE_PARK_VALUE_BY:
                    case OBJECTIVE_REPAY_LOAN_AND_PARK_VALUE:
                        stringId = STR_WINDOW_OBJECTIVE_PARK_VALUE;
                        break;
                    case OBJECTIVE_MONTHLY_RIDE_INCOME:
                        stringId = STR_WINDOW_OBJECTIVE_MONTHLY_INCOME;
                        break;
                    case OBJECTIVE_MONTHLY_FOOD_INCOME:
                        stringId = STR_WINDOW_OBJECTIVE_MONTHLY_PROFIT;
                        break;
                    case OBJECTIVE_10_ROLLERCOASTERS_LENGTH:
                        stringId = STR_WINDOW_OBJECTIVE_MINIMUM_LENGTH;
                        break;
                    default:
                        stringId = STR_WINDOW_OBJECTIVE_EXCITEMENT_RATING;
                        break;
                }
                DrawTextBasic(rt, screenCoords, stringId);

                // Objective argument 1 value
                screenCoords = windowPos
                    + ScreenCoordsXY{ widgets[WIDX_OBJECTIVE_ARG_1].left + 1, widgets[WIDX_OBJECTIVE_ARG_1].top };
                ft = Formatter();
                switch (gameState.scenarioObjective.Type)
                {
                    case OBJECTIVE_GUESTS_BY:
                    case OBJECTIVE_GUESTS_AND_RATING:
                        stringId = STR_WINDOW_COLOUR_2_COMMA32;
                        ft.Add<int32_t>(gameState.scenarioObjective.NumGuests);
                        break;
                    case OBJECTIVE_PARK_VALUE_BY:
                    case OBJECTIVE_REPAY_LOAN_AND_PARK_VALUE:
                    case OBJECTIVE_MONTHLY_RIDE_INCOME:
                    case OBJECTIVE_MONTHLY_FOOD_INCOME:
                        stringId = STR_CURRENCY_FORMAT_LABEL;
                        ft.Add<money64>(gameState.scenarioObjective.Currency);
                        break;
                    case OBJECTIVE_10_ROLLERCOASTERS_LENGTH:
                        stringId = STR_WINDOW_COLOUR_2_LENGTH;
                        ft.Add<uint16_t>(gameState.scenarioObjective.MinimumLength);
                        break;
                    case OBJECTIVE_FINISH_5_ROLLERCOASTERS:
                        stringId = STR_WINDOW_COLOUR_2_COMMA2DP32;
                        ft.Add<uint16_t>(gameState.scenarioObjective.MinimumExcitement);
                        break;
                    default:
                        stringId = STR_WINDOW_COLOUR_2_COMMA2DP32;
                        ft.Add<money64>(gameState.scenarioObjective.Currency);
                        break;
                }
                DrawTextBasic(rt, screenCoords, stringId, ft, { COLOUR_BLACK });
            }

            if (widgets[WIDX_OBJECTIVE_ARG_2].type != WindowWidgetType::Empty)
            {
                // Objective argument 2 label
                screenCoords = windowPos + ScreenCoordsXY{ 28, widgets[WIDX_OBJECTIVE_ARG_2].top };
                DrawTextBasic(rt, screenCoords, STR_WINDOW_OBJECTIVE_DATE);

                // Objective argument 2 value
                screenCoords = windowPos
                    + ScreenCoordsXY{ widgets[WIDX_OBJECTIVE_ARG_2].left + 1, widgets[WIDX_OBJECTIVE_ARG_2].top };
                ft = Formatter();
                ft.Add<uint16_t>((gameState.scenarioObjective.Year * MONTH_COUNT) - 1);
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
                        this, WIDX_PARK_NAME, STR_PARK_NAME, STR_ENTER_PARK_NAME, {}, gameState.park.Name.c_str(),
                        kParkNameMaxLength);
                    break;
                }
                case WIDX_SCENARIO_NAME:
                    WindowTextInputRawOpen(
                        this, WIDX_SCENARIO_NAME, STR_SCENARIO_NAME, STR_ENTER_SCENARIO_NAME, {},
                        gameState.scenarioName.c_str(), kScenarioNameMaxLength);
                    break;
                case WIDX_DETAILS:
                    WindowTextInputRawOpen(
                        this, WIDX_DETAILS, STR_PARK_SCENARIO_DETAILS, STR_ENTER_SCENARIO_DESCRIPTION, {},
                        gameState.scenarioDetails.c_str(), kScenarioDetailsNameMaxLength);
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
                    if (gameState.scenarioCategory != static_cast<ScenarioCategory>(dropdownIndex))
                    {
                        gameState.scenarioCategory = static_cast<ScenarioCategory>(dropdownIndex);
                        Invalidate();
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
            DrawWidgets(rt);
            DrawTabImages(rt);

            const auto& gameState = getGameState();

            // Land name
            auto screenCoords = windowPos + ScreenCoordsXY{ 8, widgets[WIDX_PARK_NAME].top };
            int32_t widthToSet = widgets[WIDX_PARK_NAME].left - 16;

            {
                auto parkName = gameState.park.Name.c_str();

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
            ft.Add<const char*>(gameState.scenarioName.c_str());
            DrawTextEllipsised(rt, screenCoords, widthToSet, STR_WINDOW_SCENARIO_NAME, ft);

            // Scenario details label
            screenCoords = windowPos + ScreenCoordsXY{ 8, widgets[WIDX_DETAILS].top };
            DrawTextBasic(rt, screenCoords, STR_WINDOW_PARK_DETAILS);

            // Scenario details value
            screenCoords = windowPos + ScreenCoordsXY{ 16, widgets[WIDX_DETAILS].top + 10 };
            widthToSet = widgets[WIDX_DETAILS].left - 4;

            ft = Formatter();
            ft.Add<StringId>(STR_STRING);
            ft.Add<const char*>(gameState.scenarioDetails.c_str());
            DrawTextWrapped(rt, screenCoords, widthToSet, STR_BLACK_STRING, ft);

            // Scenario category label
            screenCoords = windowPos + ScreenCoordsXY{ 8, widgets[WIDX_CATEGORY].top };
            DrawTextBasic(rt, screenCoords, STR_WINDOW_SCENARIO_GROUP);

            // Scenario category value
            screenCoords = windowPos + ScreenCoordsXY{ widgets[WIDX_CATEGORY].left + 1, widgets[WIDX_CATEGORY].top };
            ft = Formatter();
            ft.Add<StringId>(kScenarioCategoryStringIds[EnumValue(gameState.scenarioCategory)]);
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
            switch (widgetIndex)
            {
                case WIDX_PARK_NAME:
                {
                    auto action = ParkSetNameAction(std::string(text));
                    GameActions::Execute(&action);

                    if (gameState.scenarioName.empty())
                    {
                        gameState.scenarioName = gameState.park.Name;
                    }
                    break;
                }
                case WIDX_SCENARIO_NAME:
                    gameState.scenarioName = text;
                    Invalidate();
                    break;
                case WIDX_DETAILS:
                    gameState.scenarioDetails = text;
                    Invalidate();
                    break;
            }
        }

        void ScenarioDetailsOnUpdate()
        {
            frame_no++;
            InvalidateWidget(WIDX_TAB_1);
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
                    auto newMoneySetting = (gameState.park.Flags & PARK_FLAGS_NO_MONEY) ? 0 : 1;
                    auto scenarioSetSetting = ScenarioSetSettingAction(ScenarioSetSetting::NoMoney, newMoneySetting);
                    GameActions::Execute(&scenarioSetSetting);
                    Invalidate();
                    break;
                }
                case WIDX_FORBID_MARKETING:
                {
                    auto scenarioSetSetting = ScenarioSetSettingAction(
                        ScenarioSetSetting::ForbidMarketingCampaigns,
                        gameState.park.Flags & PARK_FLAGS_FORBID_MARKETING_CAMPAIGN ? 0 : 1);
                    GameActions::Execute(&scenarioSetSetting);
                    Invalidate();
                    break;
                }
                case WIDX_RCT1_INTEREST:
                {
                    auto scenarioSetSetting = ScenarioSetSettingAction(
                        ScenarioSetSetting::UseRCT1Interest, gameState.park.Flags & PARK_FLAGS_RCT1_INTEREST ? 0 : 1);
                    GameActions::Execute(&scenarioSetSetting);
                    Invalidate();
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
                    if (gameState.initialCash < 1000000.00_GBP)
                    {
                        auto scenarioSetSetting = ScenarioSetSettingAction(
                            ScenarioSetSetting::InitialCash, gameState.initialCash + 500.00_GBP);
                        GameActions::Execute(&scenarioSetSetting);
                    }
                    else
                    {
                        ContextShowError(STR_CANT_INCREASE_CASH, kStringIdNone, {});
                    }
                    Invalidate();
                    break;
                case WIDX_INITIAL_CASH_DECREASE:
                    if (gameState.initialCash > 0.00_GBP)
                    {
                        auto scenarioSetSetting = ScenarioSetSettingAction(
                            ScenarioSetSetting::InitialCash, gameState.initialCash - 500.00_GBP);
                        GameActions::Execute(&scenarioSetSetting);
                    }
                    else
                    {
                        ContextShowError(STR_CANT_REDUCE_CASH, kStringIdNone, {});
                    }
                    Invalidate();
                    break;
                case WIDX_INITIAL_LOAN_INCREASE:
                    if (gameState.bankLoan < 5000000.00_GBP)
                    {
                        auto scenarioSetSetting = ScenarioSetSettingAction(
                            ScenarioSetSetting::InitialLoan, gameState.bankLoan + 1000.00_GBP);
                        GameActions::Execute(&scenarioSetSetting);
                    }
                    else
                    {
                        ContextShowError(STR_CANT_INCREASE_INIT_LOAN, kStringIdNone, {});
                    }
                    Invalidate();
                    break;
                case WIDX_INITIAL_LOAN_DECREASE:
                    if (gameState.bankLoan > 0.00_GBP)
                    {
                        auto scenarioSetSetting = ScenarioSetSettingAction(
                            ScenarioSetSetting::InitialLoan, gameState.bankLoan - 1000.00_GBP);
                        GameActions::Execute(&scenarioSetSetting);
                    }
                    else
                    {
                        ContextShowError(STR_CANT_REDUCE_INIT_LOAN, kStringIdNone, {});
                    }
                    Invalidate();
                    break;
                case WIDX_MAXIMUM_LOAN_INCREASE:
                    if (gameState.maxBankLoan < 5000000.00_GBP)
                    {
                        auto scenarioSetSetting = ScenarioSetSettingAction(
                            ScenarioSetSetting::MaximumLoanSize, gameState.maxBankLoan + 1000.00_GBP);
                        GameActions::Execute(&scenarioSetSetting);
                    }
                    else
                    {
                        ContextShowError(STR_CANT_INCREASE_MAX_LOAN, kStringIdNone, {});
                    }
                    Invalidate();
                    break;
                case WIDX_MAXIMUM_LOAN_DECREASE:
                    if (gameState.maxBankLoan > 0.00_GBP)
                    {
                        auto scenarioSetSetting = ScenarioSetSettingAction(
                            ScenarioSetSetting::MaximumLoanSize, gameState.maxBankLoan - 1000.00_GBP);
                        GameActions::Execute(&scenarioSetSetting);
                    }
                    else
                    {
                        ContextShowError(STR_CANT_REDUCE_MAX_LOAN, kStringIdNone, {});
                    }
                    Invalidate();
                    break;
                case WIDX_INTEREST_RATE_INCREASE:
                    if (gameState.bankLoanInterestRate < MaxBankLoanInterestRate)
                    {
                        auto scenarioSetSetting = ScenarioSetSettingAction(
                            ScenarioSetSetting::AnnualInterestRate, gameState.bankLoanInterestRate + 1);
                        GameActions::Execute(&scenarioSetSetting);
                    }
                    else
                    {
                        ContextShowError(STR_CANT_INCREASE_INTEREST_RATE, kStringIdNone, {});
                    }
                    Invalidate();
                    break;
                case WIDX_INTEREST_RATE_DECREASE:
                    if (gameState.bankLoanInterestRate > 0)
                    {
                        auto interest = std::min<uint8_t>(MaxBankLoanInterestRate, gameState.bankLoanInterestRate - 1);
                        auto scenarioSetSetting = ScenarioSetSettingAction(ScenarioSetSetting::AnnualInterestRate, interest);
                        GameActions::Execute(&scenarioSetSetting);
                    }
                    else
                    {
                        ContextShowError(STR_CANT_REDUCE_INTEREST_RATE, kStringIdNone, {});
                    }
                    Invalidate();
                    break;
                case WIDX_ENTRY_PRICE_INCREASE:
                    if (gameState.park.EntranceFee < kMaxEntranceFee)
                    {
                        auto scenarioSetSetting = ScenarioSetSettingAction(
                            ScenarioSetSetting::ParkChargeEntryFee, gameState.park.EntranceFee + 1.00_GBP);
                        GameActions::Execute(&scenarioSetSetting);
                    }
                    else
                    {
                        ContextShowError(STR_CANT_INCREASE_FURTHER, kStringIdNone, {});
                    }
                    Invalidate();
                    break;
                case WIDX_ENTRY_PRICE_DECREASE:
                    if (gameState.park.EntranceFee > 0.00_GBP)
                    {
                        auto scenarioSetSetting = ScenarioSetSettingAction(
                            ScenarioSetSetting::ParkChargeEntryFee, gameState.park.EntranceFee - 1.00_GBP);
                        GameActions::Execute(&scenarioSetSetting);
                    }
                    else
                    {
                        ContextShowError(STR_CANT_REDUCE_FURTHER, kStringIdNone, {});
                    }
                    Invalidate();
                    break;
                case WIDX_PAY_FOR_PARK_OR_RIDES_DROPDOWN:
                {
                    Widget* dropdownWidget = &widgets[widgetIndex - 1];

                    gDropdownItems[0].Format = STR_DROPDOWN_MENU_LABEL;
                    gDropdownItems[0].Args = STR_FREE_PARK_ENTER;
                    gDropdownItems[1].Format = STR_DROPDOWN_MENU_LABEL;
                    gDropdownItems[1].Args = STR_PAY_PARK_ENTER;
                    gDropdownItems[2].Format = STR_DROPDOWN_MENU_LABEL;
                    gDropdownItems[2].Args = STR_PAID_ENTRY_PAID_RIDES;

                    WindowDropdownShowTextCustomWidth(
                        { windowPos.x + dropdownWidget->left, windowPos.y + dropdownWidget->top }, dropdownWidget->height() - 1,
                        colours[1], 0, Dropdown::Flag::StayOpen, 3, dropdownWidget->width() - 3);

                    if (gameState.park.Flags & PARK_FLAGS_UNLOCK_ALL_PRICES)
                        Dropdown::SetChecked(2, true);
                    else if (gameState.park.Flags & PARK_FLAGS_PARK_FREE_ENTRY)
                        Dropdown::SetChecked(0, true);
                    else
                        Dropdown::SetChecked(1, true);

                    break;
                }
            }

            if (gLegacyScene == LegacyScene::playing)
            {
                auto* windowMgr = Ui::GetWindowManager();
                windowMgr->InvalidateByClass(WindowClass::Finances);
                windowMgr->InvalidateByClass(WindowClass::BottomToolbar);
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
                    auto scenarioSetSetting = ScenarioSetSettingAction(ScenarioSetSetting::ParkChargeMethod, dropdownIndex);
                    GameActions::Execute(&scenarioSetSetting);
                    Invalidate();
                    break;
                }
            }
        }

        void FinancialUpdate()
        {
            frame_no++;
            InvalidateWidget(WIDX_TAB_1);
        }

        void FinancialPrepareDraw()
        {
            SetPressedTab();

            auto& gameState = getGameState();
            if (gameState.park.Flags & PARK_FLAGS_NO_MONEY)
            {
                SetWidgetPressed(WIDX_NO_MONEY, true);
                for (int32_t i = WIDX_GROUP_LOAN; i <= WIDX_FORBID_MARKETING; i++)
                    widgets[i].type = WindowWidgetType::Empty;
            }
            else
            {
                SetWidgetPressed(WIDX_NO_MONEY, false);

                widgets[WIDX_GROUP_LOAN].type = WindowWidgetType::Groupbox;
                widgets[WIDX_INITIAL_LOAN_LABEL].type = WindowWidgetType::Label;
                widgets[WIDX_INITIAL_LOAN].type = WindowWidgetType::Spinner;
                widgets[WIDX_INITIAL_LOAN_INCREASE].type = WindowWidgetType::Button;
                widgets[WIDX_INITIAL_LOAN_DECREASE].type = WindowWidgetType::Button;
                widgets[WIDX_MAXIMUM_LOAN_LABEL].type = WindowWidgetType::Label;
                widgets[WIDX_MAXIMUM_LOAN].type = WindowWidgetType::Spinner;
                widgets[WIDX_MAXIMUM_LOAN_INCREASE].type = WindowWidgetType::Button;
                widgets[WIDX_MAXIMUM_LOAN_DECREASE].type = WindowWidgetType::Button;

                if (gameState.park.Flags & PARK_FLAGS_RCT1_INTEREST)
                {
                    widgets[WIDX_INTEREST_RATE_LABEL].type = WindowWidgetType::Empty;
                    widgets[WIDX_INTEREST_RATE].type = WindowWidgetType::Empty;
                    widgets[WIDX_INTEREST_RATE_INCREASE].type = WindowWidgetType::Empty;
                    widgets[WIDX_INTEREST_RATE_DECREASE].type = WindowWidgetType::Empty;
                    widgets[WIDX_RCT1_INTEREST].type = WindowWidgetType::Checkbox;
                    SetWidgetPressed(WIDX_RCT1_INTEREST, true);
                }
                else
                {
                    widgets[WIDX_INTEREST_RATE_LABEL].type = WindowWidgetType::Label;
                    widgets[WIDX_INTEREST_RATE].type = WindowWidgetType::Spinner;
                    widgets[WIDX_INTEREST_RATE_INCREASE].type = WindowWidgetType::Button;
                    widgets[WIDX_INTEREST_RATE_DECREASE].type = WindowWidgetType::Button;
                    widgets[WIDX_RCT1_INTEREST].type = WindowWidgetType::Empty;
                }

                widgets[WIDX_GROUP_BUSINESS_MODEL].type = WindowWidgetType::Groupbox;
                widgets[WIDX_INITIAL_CASH_LABEL].type = WindowWidgetType::Label;
                widgets[WIDX_INITIAL_CASH].type = WindowWidgetType::Spinner;
                widgets[WIDX_INITIAL_CASH_INCREASE].type = WindowWidgetType::Button;
                widgets[WIDX_INITIAL_CASH_DECREASE].type = WindowWidgetType::Button;
                widgets[WIDX_PAY_FOR_PARK_OR_RIDES_LABEL].type = WindowWidgetType::Label;
                widgets[WIDX_PAY_FOR_PARK_OR_RIDES].type = WindowWidgetType::DropdownMenu;
                widgets[WIDX_PAY_FOR_PARK_OR_RIDES_DROPDOWN].type = WindowWidgetType::Button;
                widgets[WIDX_ENTRY_PRICE_LABEL].type = WindowWidgetType::Label;
                widgets[WIDX_ENTRY_PRICE].type = WindowWidgetType::Spinner;
                widgets[WIDX_ENTRY_PRICE_INCREASE].type = WindowWidgetType::Button;
                widgets[WIDX_ENTRY_PRICE_DECREASE].type = WindowWidgetType::Button;
                widgets[WIDX_FORBID_MARKETING].type = WindowWidgetType::Checkbox;

                if (!Park::EntranceFeeUnlocked())
                {
                    widgets[WIDX_ENTRY_PRICE_LABEL].type = WindowWidgetType::Empty;
                    widgets[WIDX_ENTRY_PRICE].type = WindowWidgetType::Empty;
                    widgets[WIDX_ENTRY_PRICE_INCREASE].type = WindowWidgetType::Empty;
                    widgets[WIDX_ENTRY_PRICE_DECREASE].type = WindowWidgetType::Empty;
                }
            }

            SetWidgetPressed(WIDX_FORBID_MARKETING, gameState.park.Flags & PARK_FLAGS_FORBID_MARKETING_CAMPAIGN);

            widgets[WIDX_CLOSE].type = gLegacyScene == LegacyScene::scenarioEditor ? WindowWidgetType::Empty
                                                                                   : WindowWidgetType::CloseBox;
        }

        void FinancialDraw(RenderTarget& rt)
        {
            ScreenCoordsXY screenCoords{};

            WindowDrawWidgets(*this, rt);
            DrawTabImages(rt);

            auto& gameState = getGameState();

            const auto& initialCashWidget = widgets[WIDX_INITIAL_CASH];
            if (initialCashWidget.type != WindowWidgetType::Empty)
            {
                screenCoords = windowPos + ScreenCoordsXY{ initialCashWidget.left + 1, initialCashWidget.top };
                auto ft = Formatter();
                ft.Add<money64>(getGameState().initialCash);
                DrawTextBasic(rt, screenCoords, STR_CURRENCY_FORMAT_LABEL, ft);
            }

            const auto& initialLoanWidget = widgets[WIDX_INITIAL_LOAN];
            if (initialLoanWidget.type != WindowWidgetType::Empty)
            {
                screenCoords = windowPos + ScreenCoordsXY{ initialLoanWidget.left + 1, initialLoanWidget.top };
                auto ft = Formatter();
                ft.Add<money64>(gameState.bankLoan);
                DrawTextBasic(rt, screenCoords, STR_CURRENCY_FORMAT_LABEL, ft);
            }

            const auto& maximumLoanWidget = widgets[WIDX_MAXIMUM_LOAN];
            if (maximumLoanWidget.type != WindowWidgetType::Empty)
            {
                screenCoords = windowPos + ScreenCoordsXY{ maximumLoanWidget.left + 1, maximumLoanWidget.top };
                auto ft = Formatter();
                ft.Add<money64>(getGameState().maxBankLoan);
                DrawTextBasic(rt, screenCoords, STR_CURRENCY_FORMAT_LABEL, ft);
            }

            const auto& interestRateWidget = widgets[WIDX_INTEREST_RATE];
            if (interestRateWidget.type != WindowWidgetType::Empty)
            {
                screenCoords = windowPos + ScreenCoordsXY{ interestRateWidget.left + 1, interestRateWidget.top };

                auto ft = Formatter();
                ft.Add<int16_t>(
                    std::clamp<int16_t>(static_cast<int16_t>(gameState.bankLoanInterestRate), INT16_MIN, INT16_MAX));
                DrawTextBasic(rt, screenCoords, STR_PERCENT_FORMAT_LABEL, ft);
            }

            const auto& payForParkOrRidesWidget = widgets[WIDX_PAY_FOR_PARK_OR_RIDES];
            if (payForParkOrRidesWidget.type != WindowWidgetType::Empty)
            {
                // Pay for park or rides label
                screenCoords = windowPos + ScreenCoordsXY{ payForParkOrRidesWidget.left + 1, payForParkOrRidesWidget.top };

                auto ft = Formatter();
                // Pay for park and/or rides value
                if (gameState.park.Flags & PARK_FLAGS_UNLOCK_ALL_PRICES)
                    ft.Add<StringId>(STR_PAID_ENTRY_PAID_RIDES);
                else if (gameState.park.Flags & PARK_FLAGS_PARK_FREE_ENTRY)
                    ft.Add<StringId>(STR_FREE_PARK_ENTER);
                else
                    ft.Add<StringId>(STR_PAY_PARK_ENTER);

                DrawTextBasic(rt, screenCoords, STR_WINDOW_COLOUR_2_STRINGID, ft);
            }

            const auto& entryPriceWidget = widgets[WIDX_ENTRY_PRICE];
            if (entryPriceWidget.type != WindowWidgetType::Empty)
            {
                // Entry price value
                screenCoords = windowPos + ScreenCoordsXY{ entryPriceWidget.left + 1, entryPriceWidget.top };
                auto ft = Formatter();
                ft.Add<money64>(gameState.park.EntranceFee);
                DrawTextBasic(rt, screenCoords, STR_CURRENCY_FORMAT_LABEL, ft);
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

            switch (widgetIndex)
            {
                case WIDX_CASH_PER_GUEST_INCREASE:
                    if (gameState.guestInitialCash < 1000.00_GBP)
                    {
                        auto scenarioSetSetting = ScenarioSetSettingAction(
                            ScenarioSetSetting::AverageCashPerGuest, gameState.guestInitialCash + 1.00_GBP);
                        GameActions::Execute(&scenarioSetSetting);
                    }
                    else
                    {
                        ContextShowError(STR_CANT_INCREASE_FURTHER, kStringIdNone, {});
                    }
                    Invalidate();
                    break;
                case WIDX_CASH_PER_GUEST_DECREASE:
                    if (gameState.guestInitialCash > 0.00_GBP)
                    {
                        auto scenarioSetSetting = ScenarioSetSettingAction(
                            ScenarioSetSetting::AverageCashPerGuest, gameState.guestInitialCash - 1.00_GBP);
                        GameActions::Execute(&scenarioSetSetting);
                    }
                    else
                    {
                        ContextShowError(STR_CANT_REDUCE_FURTHER, kStringIdNone, {});
                    }
                    Invalidate();
                    break;
                case WIDX_GUEST_INITIAL_HAPPINESS_INCREASE:
                    if (gameState.guestInitialHappiness < 250)
                    {
                        auto scenarioSetSetting = ScenarioSetSettingAction(
                            ScenarioSetSetting::GuestInitialHappiness, gameState.guestInitialHappiness + 4);
                        GameActions::Execute(&scenarioSetSetting);
                    }
                    else
                    {
                        ContextShowError(STR_CANT_INCREASE_FURTHER, kStringIdNone, {});
                    }
                    Invalidate();
                    break;
                case WIDX_GUEST_INITIAL_HAPPINESS_DECREASE:
                    if (gameState.guestInitialHappiness > 40)
                    {
                        auto scenarioSetSetting = ScenarioSetSettingAction(
                            ScenarioSetSetting::GuestInitialHappiness, gameState.guestInitialHappiness - 4);
                        GameActions::Execute(&scenarioSetSetting);
                    }
                    else
                    {
                        ContextShowError(STR_CANT_REDUCE_FURTHER, kStringIdNone, {});
                    }
                    Invalidate();
                    break;
                case WIDX_GUEST_INITIAL_HUNGER_INCREASE:
                    if (gameState.guestInitialHunger > 40)
                    {
                        auto scenarioSetSetting = ScenarioSetSettingAction(
                            ScenarioSetSetting::GuestInitialHunger, gameState.guestInitialHunger - 4);
                        GameActions::Execute(&scenarioSetSetting);
                    }
                    else
                    {
                        ContextShowError(STR_CANT_INCREASE_FURTHER, kStringIdNone, {});
                    }
                    Invalidate();
                    break;
                case WIDX_GUEST_INITIAL_HUNGER_DECREASE:
                    if (gameState.guestInitialHunger < 250)
                    {
                        auto scenarioSetSetting = ScenarioSetSettingAction(
                            ScenarioSetSetting::GuestInitialHunger, gameState.guestInitialHunger + 4);
                        GameActions::Execute(&scenarioSetSetting);
                    }
                    else
                    {
                        ContextShowError(STR_CANT_REDUCE_FURTHER, kStringIdNone, {});
                    }
                    Invalidate();
                    break;
                case WIDX_GUEST_INITIAL_THIRST_INCREASE:
                    if (gameState.guestInitialThirst > 40)
                    {
                        auto scenarioSetSetting = ScenarioSetSettingAction(
                            ScenarioSetSetting::GuestInitialThirst, gameState.guestInitialThirst - 4);
                        GameActions::Execute(&scenarioSetSetting);
                    }
                    else
                    {
                        ContextShowError(STR_CANT_INCREASE_FURTHER, kStringIdNone, {});
                    }
                    Invalidate();
                    break;
                case WIDX_GUEST_INITIAL_THIRST_DECREASE:
                    if (gameState.guestInitialThirst < 250)
                    {
                        auto scenarioSetSetting = ScenarioSetSettingAction(
                            ScenarioSetSetting::GuestInitialThirst, gameState.guestInitialThirst + 4);
                        GameActions::Execute(&scenarioSetSetting);
                    }
                    else
                    {
                        ContextShowError(STR_CANT_REDUCE_FURTHER, kStringIdNone, {});
                    }
                    Invalidate();
                    break;

                case WIDX_GUESTS_INTENSITY_PREFERENCE_DROPDOWN:
                {
                    auto& dropdownWidget = widgets[widgetIndex - 1];

                    gDropdownItems[0].Format = STR_DROPDOWN_MENU_LABEL;
                    gDropdownItems[1].Format = STR_DROPDOWN_MENU_LABEL;
                    gDropdownItems[2].Format = STR_DROPDOWN_MENU_LABEL;
                    gDropdownItems[3].Format = STR_DROPDOWN_MENU_LABEL;

                    gDropdownItems[0].Args = STR_GUESTS_PREFER_INTENSITY_NONE;
                    gDropdownItems[1].Args = STR_GUESTS_PREFER_INTENSITY_BALANCED;
                    gDropdownItems[2].Args = STR_GUESTS_PREFER_INTENSITY_LESS_INTENSE_RIDES;
                    gDropdownItems[3].Args = STR_GUESTS_PREFER_INTENSITY_MORE_INTENSE_RIDES;

                    WindowDropdownShowTextCustomWidth(
                        { windowPos.x + dropdownWidget.left, windowPos.y + dropdownWidget.top }, dropdownWidget.height() - 1,
                        colours[1], 0, Dropdown::Flag::StayOpen, 4, dropdownWidget.width() - 3);

                    const auto preferLess = gameState.park.Flags & PARK_FLAGS_PREF_LESS_INTENSE_RIDES;
                    const auto preferMore = gameState.park.Flags & PARK_FLAGS_PREF_MORE_INTENSE_RIDES;

                    auto prefItem = 1;
                    if (preferLess && preferMore)
                        prefItem = 0;
                    else if (preferLess && !preferMore)
                        prefItem = 2;
                    else if (!preferLess && preferMore)
                        prefItem = 3;

                    Dropdown::SetChecked(prefItem, true);
                    break;
                }
                case WIDX_HARD_GUEST_GENERATION:
                {
                    auto scenarioSetSetting = ScenarioSetSettingAction(
                        ScenarioSetSetting::GuestGenerationHigherDifficultyLevel,
                        gameState.park.Flags & PARK_FLAGS_DIFFICULT_GUEST_GENERATION ? 0 : 1);
                    GameActions::Execute(&scenarioSetSetting);
                    Invalidate();
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

            auto scenarioSetLessSetting = ScenarioSetSettingAction(
                ScenarioSetSetting::GuestsPreferLessIntenseRides, preferLess);
            GameActions::Execute(&scenarioSetLessSetting);

            auto scenarioSetMoreSetting = ScenarioSetSettingAction(
                ScenarioSetSetting::GuestsPreferMoreIntenseRides, preferMore);
            GameActions::Execute(&scenarioSetMoreSetting);
        }

        void GuestsUpdate()
        {
            frame_no++;
            InvalidateWidget(WIDX_TAB_2);
        }

        void GuestsPrepareDraw()
        {
            SetPressedTab();

            auto& gameState = getGameState();
            if (gameState.park.Flags & PARK_FLAGS_NO_MONEY)
            {
                widgets[WIDX_CASH_PER_GUEST].type = WindowWidgetType::Empty;
                widgets[WIDX_CASH_PER_GUEST_INCREASE].type = WindowWidgetType::Empty;
                widgets[WIDX_CASH_PER_GUEST_DECREASE].type = WindowWidgetType::Empty;
            }
            else
            {
                widgets[WIDX_CASH_PER_GUEST].type = WindowWidgetType::Spinner;
                widgets[WIDX_CASH_PER_GUEST_INCREASE].type = WindowWidgetType::Button;
                widgets[WIDX_CASH_PER_GUEST_DECREASE].type = WindowWidgetType::Button;
            }

            widgets[WIDX_CLOSE].type = gLegacyScene == LegacyScene::scenarioEditor ? WindowWidgetType::Empty
                                                                                   : WindowWidgetType::CloseBox;

            SetWidgetPressed(WIDX_HARD_GUEST_GENERATION, gameState.park.Flags & PARK_FLAGS_DIFFICULT_GUEST_GENERATION);
        }

        void GuestsDraw(RenderTarget& rt)
        {
            ScreenCoordsXY screenCoords{};

            WindowDrawWidgets(*this, rt);
            DrawTabImages(rt);
            auto& gameState = getGameState();

            const auto& cashPerGuestWidget = widgets[WIDX_CASH_PER_GUEST];
            if (cashPerGuestWidget.type != WindowWidgetType::Empty)
            {
                // Cash per guest value
                screenCoords = windowPos + ScreenCoordsXY{ cashPerGuestWidget.left + 1, cashPerGuestWidget.top };
                auto ft = Formatter();
                ft.Add<money64>(gameState.guestInitialCash);
                DrawTextBasic(rt, screenCoords, STR_CURRENCY_FORMAT_LABEL, ft);
            }

            // Guest initial happiness value
            const auto& initialHappinessWidget = widgets[WIDX_GUEST_INITIAL_HAPPINESS];
            screenCoords = windowPos + ScreenCoordsXY{ initialHappinessWidget.left + 1, initialHappinessWidget.top };
            auto ft = Formatter();
            ft.Add<uint16_t>((gameState.guestInitialHappiness * 100) / 255);
            DrawTextBasic(rt, screenCoords, STR_PERCENT_FORMAT_LABEL, ft);

            // Guest initial hunger value
            const auto& initialHungerWidget = widgets[WIDX_GUEST_INITIAL_HUNGER];
            screenCoords = windowPos + ScreenCoordsXY{ initialHungerWidget.left + 1, initialHungerWidget.top };
            ft = Formatter();
            ft.Add<uint16_t>(((255 - gameState.guestInitialHunger) * 100) / 255);
            DrawTextBasic(rt, screenCoords, STR_PERCENT_FORMAT_LABEL, ft);

            // Guest initial thirst value
            const auto& initialThirstWidget = widgets[WIDX_GUEST_INITIAL_THIRST];
            screenCoords = windowPos + ScreenCoordsXY{ initialThirstWidget.left + 1, initialThirstWidget.top };
            ft = Formatter();
            ft.Add<uint16_t>(((255 - gameState.guestInitialThirst) * 100) / 255);
            DrawTextBasic(rt, screenCoords, STR_PERCENT_FORMAT_LABEL, ft);

            // Guests' intensity value
            {
                const auto& guestsIntensity = widgets[WIDX_GUESTS_INTENSITY_PREFERENCE];
                screenCoords = windowPos + ScreenCoordsXY{ guestsIntensity.left + 1, guestsIntensity.top };

                const auto preferLess = gameState.park.Flags & PARK_FLAGS_PREF_LESS_INTENSE_RIDES;
                const auto preferMore = gameState.park.Flags & PARK_FLAGS_PREF_MORE_INTENSE_RIDES;

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
                    auto scenarioSetSetting = ScenarioSetSettingAction(
                        ScenarioSetSetting::ForbidTreeRemoval, gameState.park.Flags & PARK_FLAGS_FORBID_TREE_REMOVAL ? 0 : 1);
                    GameActions::Execute(&scenarioSetSetting);
                    Invalidate();
                    break;
                }
                case WIDX_FORBID_LANDSCAPE_CHANGES:
                {
                    auto scenarioSetSetting = ScenarioSetSettingAction(
                        ScenarioSetSetting::ForbidLandscapeChanges,
                        gameState.park.Flags & PARK_FLAGS_FORBID_LANDSCAPE_CHANGES ? 0 : 1);
                    GameActions::Execute(&scenarioSetSetting);
                    Invalidate();
                    break;
                }
                case WIDX_FORBID_HIGH_CONSTRUCTION:
                {
                    auto scenarioSetSetting = ScenarioSetSettingAction(
                        ScenarioSetSetting::ForbidHighConstruction,
                        gameState.park.Flags & PARK_FLAGS_FORBID_HIGH_CONSTRUCTION ? 0 : 1);
                    GameActions::Execute(&scenarioSetSetting);
                    Invalidate();
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
            switch (widgetIndex)
            {
                case WIDX_LAND_COST_INCREASE:
                    if (gameState.landPrice < 200.00_GBP)
                    {
                        auto scenarioSetSetting = ScenarioSetSettingAction(
                            ScenarioSetSetting::CostToBuyLand, gameState.landPrice + 1.00_GBP);
                        GameActions::Execute(&scenarioSetSetting);
                    }
                    else
                    {
                        ContextShowError(STR_CANT_INCREASE_FURTHER, kStringIdNone, {});
                    }
                    Invalidate();
                    break;
                case WIDX_LAND_COST_DECREASE:
                    if (gameState.landPrice > 5.00_GBP)
                    {
                        auto scenarioSetSetting = ScenarioSetSettingAction(
                            ScenarioSetSetting::CostToBuyLand, gameState.landPrice - 1.00_GBP);
                        GameActions::Execute(&scenarioSetSetting);
                    }
                    else
                    {
                        ContextShowError(STR_CANT_REDUCE_FURTHER, kStringIdNone, {});
                    }
                    Invalidate();
                    break;
                case WIDX_CONSTRUCTION_RIGHTS_COST_INCREASE:
                    if (gameState.constructionRightsPrice < 200.00_GBP)
                    {
                        auto scenarioSetSetting = ScenarioSetSettingAction(
                            ScenarioSetSetting::CostToBuyConstructionRights, gameState.constructionRightsPrice + 1.00_GBP);
                        GameActions::Execute(&scenarioSetSetting);
                    }
                    else
                    {
                        ContextShowError(STR_CANT_INCREASE_FURTHER, kStringIdNone, {});
                    }
                    Invalidate();
                    break;
                case WIDX_CONSTRUCTION_RIGHTS_COST_DECREASE:
                    if (gameState.constructionRightsPrice > 5.00_GBP)
                    {
                        auto scenarioSetSetting = ScenarioSetSettingAction(
                            ScenarioSetSetting::CostToBuyConstructionRights, gameState.constructionRightsPrice - 1.00_GBP);
                        GameActions::Execute(&scenarioSetSetting);
                    }
                    else
                    {
                        ContextShowError(STR_CANT_REDUCE_FURTHER, kStringIdNone, {});
                    }
                    Invalidate();
                    break;
            }
        }

        void LandUpdate()
        {
            frame_no++;
            InvalidateWidget(WIDX_TAB_3);
        }

        void LandPrepareDraw()
        {
            SetPressedTab();

            auto& gameState = getGameState();
            if (gameState.park.Flags & PARK_FLAGS_NO_MONEY)
            {
                for (int32_t i = WIDX_LAND_COST_LABEL; i <= WIDX_CONSTRUCTION_RIGHTS_COST_DECREASE; i++)
                    widgets[i].type = WindowWidgetType::Empty;
            }
            else
            {
                widgets[WIDX_LAND_COST_LABEL].type = WindowWidgetType::Label;
                widgets[WIDX_LAND_COST].type = WindowWidgetType::Spinner;
                widgets[WIDX_LAND_COST_INCREASE].type = WindowWidgetType::Button;
                widgets[WIDX_LAND_COST_DECREASE].type = WindowWidgetType::Button;
                widgets[WIDX_CONSTRUCTION_RIGHTS_COST].type = WindowWidgetType::Spinner;
                widgets[WIDX_CONSTRUCTION_RIGHTS_COST_INCREASE].type = WindowWidgetType::Button;
                widgets[WIDX_CONSTRUCTION_RIGHTS_COST_DECREASE].type = WindowWidgetType::Button;
            }

            SetWidgetPressed(WIDX_FORBID_TREE_REMOVAL, gameState.park.Flags & PARK_FLAGS_FORBID_TREE_REMOVAL);
            SetWidgetPressed(WIDX_FORBID_LANDSCAPE_CHANGES, gameState.park.Flags & PARK_FLAGS_FORBID_LANDSCAPE_CHANGES);
            SetWidgetPressed(WIDX_FORBID_HIGH_CONSTRUCTION, gameState.park.Flags & PARK_FLAGS_FORBID_HIGH_CONSTRUCTION);

            widgets[WIDX_CLOSE].type = gLegacyScene == LegacyScene::scenarioEditor ? WindowWidgetType::Empty
                                                                                   : WindowWidgetType::CloseBox;
        }

        void LandDraw(RenderTarget& rt)
        {
            ScreenCoordsXY screenCoords{};

            WindowDrawWidgets(*this, rt);
            DrawTabImages(rt);

            const auto& gameState = getGameState();
            const auto& landCostWidget = widgets[WIDX_LAND_COST];
            if (landCostWidget.type != WindowWidgetType::Empty)
            {
                // Cost to buy land value
                screenCoords = windowPos + ScreenCoordsXY{ landCostWidget.left + 1, landCostWidget.top };
                auto ft = Formatter();
                ft.Add<money64>(gameState.landPrice);
                DrawTextBasic(rt, screenCoords, STR_CURRENCY_FORMAT_LABEL, ft);
            }

            const auto& constructionRightsCostWidget = widgets[WIDX_CONSTRUCTION_RIGHTS_COST];
            if (constructionRightsCostWidget.type != WindowWidgetType::Empty)
            {
                // Cost to buy construction rights value
                screenCoords = windowPos
                    + ScreenCoordsXY{ constructionRightsCostWidget.left + 1, constructionRightsCostWidget.top };
                auto ft = Formatter();
                ft.Add<money64>(gameState.constructionRightsPrice);
                DrawTextBasic(rt, screenCoords, STR_CURRENCY_FORMAT_LABEL, ft);
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
                return "";
            return ride->getName();
        }

        /**
         *
         *  rct2: 0x00672544
         */
        void RidesOnUpdate()
        {
            frame_no++;
            OnPrepareDraw();
            OnResize();
            InvalidateWidget(WIDX_TAB_2);

            const auto oldSize = _rideableRides.size();
            _rideableRides.clear();
            for (auto& currentRide : GetRideManager())
            {
                if (currentRide.isRide())
                {
                    _rideableRides.push_back(currentRide.id);
                }
            }

            // Sort the rides alphabetically
            std::stable_sort(_rideableRides.begin(), _rideableRides.end(), [](const RideId& a, const RideId& b) {
                return String::compare(GetRideString(a), GetRideString(b), false) < 0;
            });

            if (oldSize != _rideableRides.size())
            {
                Invalidate();
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
            Invalidate();
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

            if (selected_list_item != i)
            {
                selected_list_item = i;
                Invalidate();
            }
        }

        /**
         *
         *  rct2: 0x006722B5
         */
        void RidesOnPrepareDraw()
        {
            SetPressedTab();

            widgets[WIDX_CLOSE].type = gLegacyScene == LegacyScene::scenarioEditor ? WindowWidgetType::Empty
                                                                                   : WindowWidgetType::CloseBox;
        }

        /**
         *
         *  rct2: 0x00672340
         */
        void RidesOnDraw(RenderTarget& rt)
        {
            DrawWidgets(rt);
            DrawTabImages(rt);
        }

        /**
         *
         *  rct2: 0x0067236F
         */
        void RidesOnScrollDraw(RenderTarget& rt, int32_t scrollIndex)
        {
            int32_t colour = ColourMapA[colours[1].colour].mid_light;
            GfxFillRect(rt, { { rt.x, rt.y }, { rt.x + rt.width - 1, rt.y + rt.height - 1 } }, colour);

            for (int32_t i = 0; i < static_cast<int32_t>(_rideableRides.size()); i++)
            {
                int32_t y = i * 12;

                if (y + 12 < rt.y || y >= rt.y + rt.height)
                    continue;

                // Checkbox
                GfxFillRectInset(rt, { { 2, y }, { 11, y + 10 } }, colours[1], INSET_RECT_F_E0);

                // Highlighted
                auto stringId = STR_BLACK_STRING;
                if (i == selected_list_item)
                {
                    stringId = STR_WINDOW_COLOUR_2_STRINGID;
                    GfxFilterRect(rt, { 0, y, width, y + 11 }, FilterPaletteID::PaletteDarken1);
                }

                // Checkbox mark
                auto* currentRide = GetRide(_rideableRides[i]);
                if (currentRide != nullptr)
                {
                    if (currentRide->lifecycleFlags & RIDE_LIFECYCLE_INDESTRUCTIBLE)
                    {
                        auto darkness = stringId == STR_WINDOW_COLOUR_2_STRINGID ? TextDarkness::ExtraDark : TextDarkness::Dark;
                        DrawText(
                            rt, { 2, y }, { colours[1].withFlag(ColourFlag::translucent, false), FontStyle::Medium, darkness },
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
            WindowClass::EditorScenarioOptions, 280, 148, WF_NO_SCROLLING | WF_CENTRE_SCREEN);
    }
} // namespace OpenRCT2::Ui::Windows
