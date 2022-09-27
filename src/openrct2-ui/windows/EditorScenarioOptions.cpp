/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../interface/Dropdown.h"
#include "../interface/Widget.h"
#include "../interface/Window.h"
#include "Window.h"

#include <algorithm>
#include <openrct2/Context.h>
#include <openrct2/Editor.h>
#include <openrct2/Game.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/actions/ClimateSetAction.h>
#include <openrct2/actions/ScenarioSetSettingAction.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/entity/Peep.h>
#include <openrct2/interface/Colour.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/localisation/StringIds.h>
#include <openrct2/management/Finance.h>
#include <openrct2/sprites.h>
#include <openrct2/world/Climate.h>
#include <openrct2/world/Park.h>

static constexpr const int32_t WW_FINANCIAL = 280;
static constexpr const int32_t WH_FINANCIAL = 149;

static constexpr const int32_t WW_GUESTS = 380;
static constexpr const int32_t WH_GUESTS = 149;

static constexpr const int32_t WW_PARK = 400;
static constexpr const int32_t WH_PARK = 200;

#pragma region Widgets

// clang-format off
enum {
    WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_FINANCIAL,
    WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_GUESTS,
    WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_PARK,
    WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_COUNT
};

static constexpr const StringId ClimateNames[] = {
    STR_CLIMATE_COOL_AND_WET,
    STR_CLIMATE_WARM,
    STR_CLIMATE_HOT_AND_DRY,
    STR_CLIMATE_COLD,
};

enum {
    WIDX_BACKGROUND,
    WIDX_TITLE,
    WIDX_CLOSE,
    WIDX_PAGE_BACKGROUND,
    WIDX_TAB_1,
    WIDX_TAB_2,
    WIDX_TAB_3,
    WIDX_PAGE_START,

    // Financial tab
    WIDX_NO_MONEY = WIDX_PAGE_START,
    WIDX_INITIAL_CASH,
    WIDX_INITIAL_CASH_INCREASE,
    WIDX_INITIAL_CASH_DECREASE,
    WIDX_INITIAL_LOAN,
    WIDX_INITIAL_LOAN_INCREASE,
    WIDX_INITIAL_LOAN_DECREASE,
    WIDX_MAXIMUM_LOAN,
    WIDX_MAXIMUM_LOAN_INCREASE,
    WIDX_MAXIMUM_LOAN_DECREASE,
    WIDX_INTEREST_RATE,
    WIDX_INTEREST_RATE_INCREASE,
    WIDX_INTEREST_RATE_DECREASE,
    WIDX_FORBID_MARKETING,

    // Guests tab
    WIDX_CASH_PER_GUEST = WIDX_PAGE_START,
    WIDX_CASH_PER_GUEST_INCREASE,
    WIDX_CASH_PER_GUEST_DECREASE,
    WIDX_GUEST_INITIAL_HAPPINESS,
    WIDX_GUEST_INITIAL_HAPPINESS_INCREASE,
    WIDX_GUEST_INITIAL_HAPPINESS_DECREASE,
    WIDX_GUEST_INITIAL_HUNGER,
    WIDX_GUEST_INITIAL_HUNGER_INCREASE,
    WIDX_GUEST_INITIAL_HUNGER_DECREASE,
    WIDX_GUEST_INITIAL_THIRST,
    WIDX_GUEST_INITIAL_THIRST_INCREASE,
    WIDX_GUEST_INITIAL_THIRST_DECREASE,
    WIDX_GUEST_PREFER_LESS_INTENSE_RIDES,
    WIDX_GUEST_PREFER_MORE_INTENSE_RIDES,

    // Park tab
    WIDX_LAND_COST = WIDX_PAGE_START,
    WIDX_LAND_COST_INCREASE,
    WIDX_LAND_COST_DECREASE,
    WIDX_CONSTRUCTION_RIGHTS_COST,
    WIDX_CONSTRUCTION_RIGHTS_COST_INCREASE,
    WIDX_CONSTRUCTION_RIGHTS_COST_DECREASE,
    WIDX_PAY_FOR_PARK_OR_RIDES,
    WIDX_PAY_FOR_PARK_OR_RIDES_DROPDOWN,
    WIDX_ENTRY_PRICE,
    WIDX_ENTRY_PRICE_INCREASE,
    WIDX_ENTRY_PRICE_DECREASE,
    WIDX_CLIMATE,
    WIDX_CLIMATE_DROPDOWN,
    WIDX_FORBID_TREE_REMOVAL,
    WIDX_FORBID_LANDSCAPE_CHANGES,
    WIDX_FORBID_HIGH_CONSTRUCTION,
    WIDX_HARD_PARK_RATING,
    WIDX_HARD_GUEST_GENERATION
};

static rct_widget window_editor_scenario_options_financial_widgets[] = {
    WINDOW_SHIM(STR_SCENARIO_OPTIONS_FINANCIAL, WW_FINANCIAL, WH_FINANCIAL),
    MakeWidget        ({  0,  43}, {     WW_FINANCIAL, 106}, WindowWidgetType::Resize,   WindowColour::Secondary                                                            ),
    MakeTab           ({  3,  17},                                                                                          STR_SCENARIO_OPTIONS_FINANCIAL_TIP),
    MakeTab           ({ 34,  17},                                                                                          STR_SCENARIO_OPTIONS_GUESTS_TIP   ),
    MakeTab           ({ 65,  17},                                                                                          STR_SCENARIO_OPTIONS_PARK_TIP     ),
    MakeWidget        ({  8,  48}, {WW_FINANCIAL - 16,  12}, WindowWidgetType::Checkbox, WindowColour::Secondary, STR_MAKE_PARK_NO_MONEY, STR_MAKE_PARK_NO_MONEY_TIP        ),
    MakeSpinnerWidgets({168,  65}, {              100,  12}, WindowWidgetType::Spinner,  WindowColour::Secondary                                                            ), // NB: 3 widgets
    MakeSpinnerWidgets({168,  82}, {              100,  12}, WindowWidgetType::Spinner,  WindowColour::Secondary                                                            ), // NB: 3 widgets
    MakeSpinnerWidgets({168,  99}, {              100,  12}, WindowWidgetType::Spinner,  WindowColour::Secondary                                                            ), // NB: 3 widgets
    MakeSpinnerWidgets({168, 116}, {               70,  12}, WindowWidgetType::Spinner,  WindowColour::Secondary                                                            ), // NB: 3 widgets
    MakeWidget        ({  8, 133}, {WW_FINANCIAL - 16,  12}, WindowWidgetType::Checkbox, WindowColour::Secondary, STR_FORBID_MARKETING,   STR_FORBID_MARKETING_TIP          ),
    WIDGETS_END,
};

static rct_widget window_editor_scenario_options_guests_widgets[] = {
    WINDOW_SHIM(STR_SCENARIO_OPTIONS_GUESTS, WW_GUESTS, WH_GUESTS),
    MakeWidget        ({  0,  43}, {     WW_GUESTS, 106}, WindowWidgetType::Resize,   WindowColour::Secondary),
    MakeRemapWidget   ({  3,  17}, {            31,  27}, WindowWidgetType::Tab,      WindowColour::Secondary, SPR_TAB,                              STR_SCENARIO_OPTIONS_FINANCIAL_TIP      ),
    MakeRemapWidget   ({ 34,  17}, {            31,  30}, WindowWidgetType::Tab,      WindowColour::Secondary, SPR_TAB,                              STR_SCENARIO_OPTIONS_GUESTS_TIP         ),
    MakeRemapWidget   ({ 65,  17}, {            31,  27}, WindowWidgetType::Tab,      WindowColour::Secondary, SPR_TAB,                              STR_SCENARIO_OPTIONS_PARK_TIP           ),
    MakeSpinnerWidgets({268,  48}, {            70,  12}, WindowWidgetType::Spinner,  WindowColour::Secondary                                                                                ), // NB: 3 widgets
    MakeSpinnerWidgets({268,  65}, {            70,  12}, WindowWidgetType::Spinner,  WindowColour::Secondary                                                                                ), // NB: 3 widgets
    MakeSpinnerWidgets({268,  82}, {            70,  12}, WindowWidgetType::Spinner,  WindowColour::Secondary                                                                                ), // NB: 3 widgets
    MakeSpinnerWidgets({268,  99}, {            70,  12}, WindowWidgetType::Spinner,  WindowColour::Secondary                                                                                ), // NB: 3 widgets
    MakeWidget        ({  8, 116}, {WW_GUESTS - 16,  12}, WindowWidgetType::Checkbox, WindowColour::Secondary, STR_GUESTS_PREFER_LESS_INTENSE_RIDES, STR_GUESTS_PREFER_LESS_INTENSE_RIDES_TIP),
    MakeWidget        ({  8, 133}, {WW_GUESTS - 16,  12}, WindowWidgetType::Checkbox, WindowColour::Secondary, STR_GUESTS_PREFER_MORE_INTENSE_RIDES, STR_GUESTS_PREFER_MORE_INTENSE_RIDES_TIP),
    WIDGETS_END,
};

static rct_widget window_editor_scenario_options_park_widgets[] = {
    WINDOW_SHIM(STR_SCENARIO_OPTIONS_PARK, WW_PARK, WH_PARK),
    MakeWidget        ({  0,  43}, {     WW_PARK, 106}, WindowWidgetType::Resize,   WindowColour::Secondary                                                                  ),
    MakeRemapWidget   ({  3,  17}, {          31,  27}, WindowWidgetType::Tab,      WindowColour::Secondary, SPR_TAB,                      STR_SCENARIO_OPTIONS_FINANCIAL_TIP),
    MakeRemapWidget   ({ 34,  17}, {          31,  30}, WindowWidgetType::Tab,      WindowColour::Secondary, SPR_TAB,                      STR_SCENARIO_OPTIONS_GUESTS_TIP   ),
    MakeRemapWidget   ({ 65,  17}, {          31,  27}, WindowWidgetType::Tab,      WindowColour::Secondary, SPR_TAB,                      STR_SCENARIO_OPTIONS_PARK_TIP     ),
    MakeSpinnerWidgets({188,  48}, {          70,  12}, WindowWidgetType::Spinner,  WindowColour::Secondary                                                                  ), // NB: 3 widgets
    MakeSpinnerWidgets({188,  65}, {          70,  12}, WindowWidgetType::Spinner,  WindowColour::Secondary                                                                  ), // NB: 3 widgets
    MakeWidget        ({  8,  82}, {         210,  12}, WindowWidgetType::DropdownMenu, WindowColour::Secondary, STR_NONE,                     STR_PAY_FOR_PARK_PAY_FOR_RIDES_TIP),
    MakeWidget        ({206,  83}, {          11,  10}, WindowWidgetType::Button,   WindowColour::Secondary, STR_DROPDOWN_GLYPH,           STR_PAY_FOR_PARK_PAY_FOR_RIDES_TIP),
    MakeSpinnerWidgets({328,  82}, {          67,  12}, WindowWidgetType::Spinner,  WindowColour::Secondary                                                                  ), // NB: 3 widgets
    MakeWidget        ({188,  99}, {         207,  12}, WindowWidgetType::DropdownMenu, WindowColour::Secondary, STR_NONE,                     STR_SELECT_CLIMATE_TIP            ),
    MakeWidget        ({383, 100}, {          11,  10}, WindowWidgetType::Button,   WindowColour::Secondary, STR_DROPDOWN_GLYPH,           STR_SELECT_CLIMATE_TIP            ),
    MakeWidget        ({  8, 116}, {WW_PARK - 16,  12}, WindowWidgetType::Checkbox, WindowColour::Secondary, STR_FORBID_TREE_REMOVAL,      STR_FORBID_TREE_REMOVAL_TIP       ),
    MakeWidget        ({  8, 133}, {WW_PARK - 16,  12}, WindowWidgetType::Checkbox, WindowColour::Secondary, STR_FORBID_LANDSCAPE_CHANGES, STR_FORBID_LANDSCAPE_CHANGES_TIP  ),
    MakeWidget        ({  8, 150}, {WW_PARK - 16,  12}, WindowWidgetType::Checkbox, WindowColour::Secondary, STR_FORBID_HIGH_CONSTRUCTION, STR_FORBID_HIGH_CONSTRUCTION_TIP  ),
    MakeWidget        ({  8, 167}, {WW_PARK - 16,  12}, WindowWidgetType::Checkbox, WindowColour::Secondary, STR_HARD_PARK_RATING,         STR_HARD_PARK_RATING_TIP          ),
    MakeWidget        ({  8, 184}, {WW_PARK - 16,  12}, WindowWidgetType::Checkbox, WindowColour::Secondary, STR_HARD_GUEST_GENERATION,    STR_HARD_GUEST_GENERATION_TIP     ),
    WIDGETS_END,
};

static rct_widget *window_editor_scenario_options_widgets[] = {
    window_editor_scenario_options_financial_widgets,
    window_editor_scenario_options_guests_widgets,
    window_editor_scenario_options_park_widgets,
};

#pragma endregion

#pragma region Enabled widgets

static uint32_t window_editor_scenario_options_page_hold_down_widgets[] = {
    (1ULL << WIDX_INITIAL_CASH_INCREASE) |
        (1ULL << WIDX_INITIAL_CASH_DECREASE) |
        (1ULL << WIDX_INITIAL_LOAN_INCREASE) |
        (1ULL << WIDX_INITIAL_LOAN_DECREASE) |
        (1ULL << WIDX_MAXIMUM_LOAN_INCREASE) |
        (1ULL << WIDX_MAXIMUM_LOAN_DECREASE) |
        (1ULL << WIDX_INTEREST_RATE_INCREASE) |
        (1ULL << WIDX_INTEREST_RATE_DECREASE),
    (1ULL << WIDX_CASH_PER_GUEST_INCREASE) |
        (1ULL << WIDX_CASH_PER_GUEST_DECREASE) |
        (1ULL << WIDX_GUEST_INITIAL_HAPPINESS_INCREASE) |
        (1ULL << WIDX_GUEST_INITIAL_HAPPINESS_DECREASE) |
        (1ULL << WIDX_GUEST_INITIAL_HUNGER_INCREASE) |
        (1ULL << WIDX_GUEST_INITIAL_HUNGER_DECREASE) |
        (1ULL << WIDX_GUEST_INITIAL_THIRST_INCREASE) |
        (1ULL << WIDX_GUEST_INITIAL_THIRST_DECREASE),
    (1ULL << WIDX_LAND_COST_INCREASE) |
        (1ULL << WIDX_LAND_COST_DECREASE) |
        (1ULL << WIDX_CONSTRUCTION_RIGHTS_COST_INCREASE) |
        (1ULL << WIDX_CONSTRUCTION_RIGHTS_COST_DECREASE) |
        (1ULL << WIDX_ENTRY_PRICE_INCREASE) |
        (1ULL << WIDX_ENTRY_PRICE_DECREASE),
};
// clang-format on

#pragma endregion

class EditorScenarioOptionsWindow final : public Window
{
public:
    void OnOpen() override
    {
        widgets = window_editor_scenario_options_widgets[WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_FINANCIAL];
        hold_down_widgets = window_editor_scenario_options_page_hold_down_widgets
            [WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_FINANCIAL];
        WindowInitScrollWidgets(*this);
        page = 0;
    }

    void OnMouseUp(WidgetIndex widgetIndex) override
    {
        switch (page)
        {
            case WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_FINANCIAL:
                return FinancialMouseUp(widgetIndex);
            case WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_GUESTS:
                return GuestsMouseUp(widgetIndex);
            case WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_PARK:
                return ParkMouseUp(widgetIndex);
        }
    }

    void OnResize() override
    {
        switch (page)
        {
            case WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_FINANCIAL:
                return FinancialResize();
            case WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_GUESTS:
                return GuestsResize();
            case WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_PARK:
                return ParkResize();
        }
    }

    void OnMouseDown(WidgetIndex widgetIndex) override
    {
        switch (page)
        {
            case WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_FINANCIAL:
                return FinancialMouseDown(widgetIndex);
            case WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_GUESTS:
                return GuestsMouseDown(widgetIndex);
            case WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_PARK:
                return ParkMouseDown(widgetIndex);
        }
    }

    void OnUpdate() override
    {
        switch (page)
        {
            case WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_FINANCIAL:
                return FinancialUpdate();
            case WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_GUESTS:
                return GuestsUpdate();
            case WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_PARK:
                return ParkUpdate();
        }
    }

    void OnPrepareDraw() override
    {
        switch (page)
        {
            case WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_FINANCIAL:
                return FinancialPrepareDraw();
            case WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_GUESTS:
                return GuestsPrepareDraw();
            case WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_PARK:
                return ParkPrepareDraw();
        }
    }

    void OnDraw(rct_drawpixelinfo& dpi) override
    {
        switch (page)
        {
            case WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_FINANCIAL:
                return FinancialDraw(dpi);
            case WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_GUESTS:
                return GuestsDraw(dpi);
            case WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_PARK:
                return ParkDraw(dpi);
        }
    }

    void OnDropdown(WidgetIndex widgetIndex, int32_t selectedIndex) override
    {
        switch (page)
        {
            case WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_PARK:
                return ParkDropdown(widgetIndex, selectedIndex);
        }
    }

private:
    void SetPressedTab()
    {
        int32_t i;
        for (i = 0; i < WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_COUNT; i++)
            SetWidgetPressed(WIDX_TAB_1 + i, false);
        SetWidgetPressed(WIDX_TAB_1 + page, true);
    }

    void AnchorBorderWidgets()
    {
        widgets[WIDX_BACKGROUND].right = width - 1;
        widgets[WIDX_BACKGROUND].bottom = height - 1;
        widgets[WIDX_PAGE_BACKGROUND].right = width - 1;
        widgets[WIDX_PAGE_BACKGROUND].bottom = height - 1;
        widgets[WIDX_TITLE].right = width - 2;
        widgets[WIDX_CLOSE].left = width - 13;
        widgets[WIDX_CLOSE].right = width - 3;
    }

    void DrawTabImages(rct_drawpixelinfo& dpi)
    {
        rct_widget* widget;
        int32_t spriteIndex;

        // Tab 1
        widget = &widgets[WIDX_TAB_1];
        spriteIndex = SPR_TAB_FINANCES_SUMMARY_0;
        if (page == WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_FINANCIAL)
            spriteIndex += (frame_no / 2) % 8;

        gfx_draw_sprite(&dpi, ImageId(spriteIndex), windowPos + ScreenCoordsXY{ widget->left, widget->top });

        // Tab 2
        widget = &widgets[WIDX_TAB_2];
        spriteIndex = SPR_TAB_GUESTS_0;
        if (page == WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_GUESTS)
            spriteIndex += (frame_no / 4) % 8;

        gfx_draw_sprite(&dpi, ImageId(spriteIndex), windowPos + ScreenCoordsXY{ widget->left, widget->top });

        // Tab 3
        widget = &widgets[WIDX_TAB_3];
        spriteIndex = SPR_TAB_PARK;
        gfx_draw_sprite(&dpi, ImageId(spriteIndex), windowPos + ScreenCoordsXY{ widget->left, widget->top });
    }

    void SetPage(int32_t newPage)
    {
        if (page == newPage)
            return;

        page = newPage;
        frame_no = 0;
        var_492 = 0;
        hold_down_widgets = window_editor_scenario_options_page_hold_down_widgets[page];
        widgets = window_editor_scenario_options_widgets[page];
        Invalidate();
        OnResize();
        OnPrepareDraw();
        WindowInitScrollWidgets(*this);
        Invalidate();
    }

#pragma region Financial

    void FinancialMouseUp(WidgetIndex widgetIndex)
    {
        switch (widgetIndex)
        {
            case WIDX_CLOSE:
                window_close(*this);
                break;
            case WIDX_TAB_1:
            case WIDX_TAB_2:
            case WIDX_TAB_3:
                SetPage(widgetIndex - WIDX_TAB_1);
                break;
            case WIDX_NO_MONEY:
            {
                auto newMoneySetting = (gParkFlags & PARK_FLAGS_NO_MONEY) ? 0 : 1;
                auto scenarioSetSetting = ScenarioSetSettingAction(ScenarioSetSetting::NoMoney, newMoneySetting);
                GameActions::Execute(&scenarioSetSetting);
                Invalidate();
                break;
            }
            case WIDX_FORBID_MARKETING:
            {
                auto scenarioSetSetting = ScenarioSetSettingAction(
                    ScenarioSetSetting::ForbidMarketingCampaigns, gParkFlags & PARK_FLAGS_FORBID_MARKETING_CAMPAIGN ? 0 : 1);
                GameActions::Execute(&scenarioSetSetting);
                Invalidate();
                break;
            }
        }
    }

    void FinancialResize()
    {
        window_set_resize(*this, 280, 149, 280, 149);
    }

    void ShowClimateDropdown()
    {
        int32_t i;

        const auto& dropdownWidget = widgets[WIDX_CLIMATE];

        for (i = 0; i < static_cast<uint8_t>(ClimateType::Count); i++)
        {
            gDropdownItems[i].Format = STR_DROPDOWN_MENU_LABEL;
            gDropdownItems[i].Args = ClimateNames[i];
        }
        WindowDropdownShowTextCustomWidth(
            { windowPos.x + dropdownWidget.left, windowPos.y + dropdownWidget.top }, dropdownWidget.height() + 1, colours[1], 0,
            Dropdown::Flag::StayOpen, static_cast<uint8_t>(ClimateType::Count), dropdownWidget.width() - 3);
        Dropdown::SetChecked(static_cast<uint8_t>(gClimate), true);
    }

    void FinancialMouseDown(WidgetIndex widgetIndex)
    {
        switch (widgetIndex)
        {
            case WIDX_INITIAL_CASH_INCREASE:
                if (gInitialCash < 1000000.00_GBP)
                {
                    auto scenarioSetSetting = ScenarioSetSettingAction(
                        ScenarioSetSetting::InitialCash, gInitialCash + 500.00_GBP);
                    GameActions::Execute(&scenarioSetSetting);
                }
                else
                {
                    context_show_error(STR_CANT_INCREASE_CASH, STR_NONE, {});
                }
                Invalidate();
                break;
            case WIDX_INITIAL_CASH_DECREASE:
                if (gInitialCash > 0.00_GBP)
                {
                    auto scenarioSetSetting = ScenarioSetSettingAction(
                        ScenarioSetSetting::InitialCash, gInitialCash - 500.00_GBP);
                    GameActions::Execute(&scenarioSetSetting);
                }
                else
                {
                    context_show_error(STR_CANT_REDUCE_CASH, STR_NONE, {});
                }
                Invalidate();
                break;
            case WIDX_INITIAL_LOAN_INCREASE:
                if (gBankLoan < 5000000.00_GBP)
                {
                    auto scenarioSetSetting = ScenarioSetSettingAction(
                        ScenarioSetSetting::InitialLoan, gBankLoan + 1000.00_GBP);
                    GameActions::Execute(&scenarioSetSetting);
                }
                else
                {
                    context_show_error(STR_CANT_INCREASE_INIT_LOAN, STR_NONE, {});
                }
                Invalidate();
                break;
            case WIDX_INITIAL_LOAN_DECREASE:
                if (gBankLoan > 0.00_GBP)
                {
                    auto scenarioSetSetting = ScenarioSetSettingAction(
                        ScenarioSetSetting::InitialLoan, gBankLoan - 1000.00_GBP);
                    GameActions::Execute(&scenarioSetSetting);
                }
                else
                {
                    context_show_error(STR_CANT_REDUCE_INIT_LOAN, STR_NONE, {});
                }
                Invalidate();
                break;
            case WIDX_MAXIMUM_LOAN_INCREASE:
                if (gMaxBankLoan < 5000000.00_GBP)
                {
                    auto scenarioSetSetting = ScenarioSetSettingAction(
                        ScenarioSetSetting::MaximumLoanSize, gMaxBankLoan + 1000.00_GBP);
                    GameActions::Execute(&scenarioSetSetting);
                }
                else
                {
                    context_show_error(STR_CANT_INCREASE_MAX_LOAN, STR_NONE, {});
                }
                Invalidate();
                break;
            case WIDX_MAXIMUM_LOAN_DECREASE:
                if (gMaxBankLoan > 0.00_GBP)
                {
                    auto scenarioSetSetting = ScenarioSetSettingAction(
                        ScenarioSetSetting::MaximumLoanSize, gMaxBankLoan - 1000.00_GBP);
                    GameActions::Execute(&scenarioSetSetting);
                }
                else
                {
                    context_show_error(STR_CANT_REDUCE_MAX_LOAN, STR_NONE, {});
                }
                Invalidate();
                break;
            case WIDX_INTEREST_RATE_INCREASE:
                if (gBankLoanInterestRate < MaxBankLoanInterestRate)
                {
                    auto scenarioSetSetting = ScenarioSetSettingAction(
                        ScenarioSetSetting::AnnualInterestRate, gBankLoanInterestRate + 1);
                    GameActions::Execute(&scenarioSetSetting);
                }
                else
                {
                    context_show_error(STR_CANT_INCREASE_INTEREST_RATE, STR_NONE, {});
                }
                Invalidate();
                break;
            case WIDX_INTEREST_RATE_DECREASE:
                if (gBankLoanInterestRate > 0)
                {
                    auto interest = std::min<uint8_t>(MaxBankLoanInterestRate, gBankLoanInterestRate - 1);
                    auto scenarioSetSetting = ScenarioSetSettingAction(ScenarioSetSetting::AnnualInterestRate, interest);
                    GameActions::Execute(&scenarioSetSetting);
                }
                else
                {
                    context_show_error(STR_CANT_REDUCE_INTEREST_RATE, STR_NONE, {});
                }
                Invalidate();
                break;
        }

        if (gScreenFlags == SCREEN_FLAGS_PLAYING)
        {
            window_invalidate_by_class(WindowClass::Finances);
            window_invalidate_by_class(WindowClass::BottomToolbar);
        }
    }

    void FinancialUpdate()
    {
        frame_no++;
        FinancialPrepareDraw();
        widget_invalidate(*this, WIDX_TAB_1);
    }

    void FinancialPrepareDraw()
    {
        rct_widget* newWidgets = window_editor_scenario_options_widgets[page];
        if (widgets != newWidgets)
        {
            widgets = newWidgets;
            WindowInitScrollWidgets(*this);
        }

        SetPressedTab();

        if (gParkFlags & PARK_FLAGS_NO_MONEY)
        {
            SetWidgetPressed(WIDX_NO_MONEY, true);
            for (int32_t i = WIDX_INITIAL_CASH; i <= WIDX_FORBID_MARKETING; i++)
                widgets[i].type = WindowWidgetType::Empty;
        }
        else
        {
            SetWidgetPressed(WIDX_NO_MONEY, false);
            widgets[WIDX_INITIAL_CASH].type = WindowWidgetType::Spinner;
            widgets[WIDX_INITIAL_CASH_INCREASE].type = WindowWidgetType::Button;
            widgets[WIDX_INITIAL_CASH_DECREASE].type = WindowWidgetType::Button;
            widgets[WIDX_INITIAL_LOAN].type = WindowWidgetType::Spinner;
            widgets[WIDX_INITIAL_LOAN_INCREASE].type = WindowWidgetType::Button;
            widgets[WIDX_INITIAL_LOAN_DECREASE].type = WindowWidgetType::Button;
            widgets[WIDX_MAXIMUM_LOAN].type = WindowWidgetType::Spinner;
            widgets[WIDX_MAXIMUM_LOAN_INCREASE].type = WindowWidgetType::Button;
            widgets[WIDX_MAXIMUM_LOAN_DECREASE].type = WindowWidgetType::Button;
            widgets[WIDX_INTEREST_RATE].type = WindowWidgetType::Spinner;
            widgets[WIDX_INTEREST_RATE_INCREASE].type = WindowWidgetType::Button;
            widgets[WIDX_INTEREST_RATE_DECREASE].type = WindowWidgetType::Button;
            widgets[WIDX_FORBID_MARKETING].type = WindowWidgetType::Checkbox;
        }

        SetWidgetPressed(WIDX_FORBID_MARKETING, gParkFlags & PARK_FLAGS_FORBID_MARKETING_CAMPAIGN);

        widgets[WIDX_CLOSE].type = (gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) ? WindowWidgetType::Empty
                                                                                 : WindowWidgetType::CloseBox;

        AnchorBorderWidgets();
    }

    void FinancialDraw(rct_drawpixelinfo& dpi)
    {
        ScreenCoordsXY screenCoords{};

        WindowDrawWidgets(*this, &dpi);
        DrawTabImages(dpi);

        const auto& initialCashWidget = widgets[WIDX_INITIAL_CASH];
        if (initialCashWidget.type != WindowWidgetType::Empty)
        {
            screenCoords = windowPos + ScreenCoordsXY{ 8, initialCashWidget.top };
            DrawTextBasic(&dpi, screenCoords, STR_INIT_CASH_LABEL);

            screenCoords = windowPos + ScreenCoordsXY{ initialCashWidget.left + 1, initialCashWidget.top };
            auto ft = Formatter();
            ft.Add<money64>(gInitialCash);
            DrawTextBasic(&dpi, screenCoords, STR_CURRENCY_FORMAT_LABEL, ft);
        }

        const auto& initialLoanWidget = widgets[WIDX_INITIAL_LOAN];
        if (initialLoanWidget.type != WindowWidgetType::Empty)
        {
            screenCoords = windowPos + ScreenCoordsXY{ 8, initialLoanWidget.top };
            DrawTextBasic(&dpi, screenCoords, STR_INIT_LOAN_LABEL);

            screenCoords = windowPos + ScreenCoordsXY{ initialLoanWidget.left + 1, initialLoanWidget.top };
            auto ft = Formatter();
            ft.Add<money64>(gBankLoan);
            DrawTextBasic(&dpi, screenCoords, STR_CURRENCY_FORMAT_LABEL, ft);
        }

        const auto& maximumLoanWidget = widgets[WIDX_MAXIMUM_LOAN];
        if (maximumLoanWidget.type != WindowWidgetType::Empty)
        {
            screenCoords = windowPos + ScreenCoordsXY{ 8, maximumLoanWidget.top };
            DrawTextBasic(&dpi, screenCoords, STR_MAX_LOAN_LABEL);

            screenCoords = windowPos + ScreenCoordsXY{ maximumLoanWidget.left + 1, maximumLoanWidget.top };
            auto ft = Formatter();
            ft.Add<money64>(gMaxBankLoan);
            DrawTextBasic(&dpi, screenCoords, STR_CURRENCY_FORMAT_LABEL, ft);
        }

        const auto& interestRateWidget = widgets[WIDX_INTEREST_RATE];
        if (interestRateWidget.type != WindowWidgetType::Empty)
        {
            screenCoords = windowPos + ScreenCoordsXY{ 8, interestRateWidget.top };
            DrawTextBasic(&dpi, screenCoords, STR_INTEREST_RATE_LABEL);

            screenCoords = windowPos + ScreenCoordsXY{ interestRateWidget.left + 1, interestRateWidget.top };

            auto ft = Formatter();
            ft.Add<int16_t>(std::clamp<int16_t>(static_cast<int16_t>(gBankLoanInterestRate), INT16_MIN, INT16_MAX));
            DrawTextBasic(&dpi, screenCoords, STR_PERCENT_FORMAT_LABEL, ft);
        }
    }

#pragma endregion

#pragma region Guests

    void GuestsMouseUp(WidgetIndex widgetIndex)
    {
        switch (widgetIndex)
        {
            case WIDX_CLOSE:
                window_close(*this);
                break;
            case WIDX_TAB_1:
            case WIDX_TAB_2:
            case WIDX_TAB_3:
                SetPage(widgetIndex - WIDX_TAB_1);
                break;
            case WIDX_GUEST_PREFER_LESS_INTENSE_RIDES:
            {
                auto scenarioSetSetting = ScenarioSetSettingAction(
                    ScenarioSetSetting::GuestsPreferLessIntenseRides, gParkFlags & PARK_FLAGS_PREF_LESS_INTENSE_RIDES ? 0 : 1);
                GameActions::Execute(&scenarioSetSetting);
                Invalidate();
                break;
            }
            case WIDX_GUEST_PREFER_MORE_INTENSE_RIDES:
            {
                auto scenarioSetSetting = ScenarioSetSettingAction(
                    ScenarioSetSetting::GuestsPreferMoreIntenseRides, gParkFlags & PARK_FLAGS_PREF_MORE_INTENSE_RIDES ? 0 : 1);
                GameActions::Execute(&scenarioSetSetting);
                Invalidate();
                break;
            }
        }
    }

    void GuestsResize()
    {
        window_set_resize(*this, 380, 149, 380, 149);
    }

    void GuestsMouseDown(WidgetIndex widgetIndex)
    {
        switch (widgetIndex)
        {
            case WIDX_CASH_PER_GUEST_INCREASE:
                if (gGuestInitialCash < 1000.00_GBP)
                {
                    auto scenarioSetSetting = ScenarioSetSettingAction(
                        ScenarioSetSetting::AverageCashPerGuest, gGuestInitialCash + 1.00_GBP);
                    GameActions::Execute(&scenarioSetSetting);
                }
                else
                {
                    context_show_error(STR_CANT_INCREASE_FURTHER, STR_NONE, {});
                }
                Invalidate();
                break;
            case WIDX_CASH_PER_GUEST_DECREASE:
                if (gGuestInitialCash > 0.00_GBP)
                {
                    auto scenarioSetSetting = ScenarioSetSettingAction(
                        ScenarioSetSetting::AverageCashPerGuest, gGuestInitialCash - 1.00_GBP);
                    GameActions::Execute(&scenarioSetSetting);
                }
                else
                {
                    context_show_error(STR_CANT_REDUCE_FURTHER, STR_NONE, {});
                }
                Invalidate();
                break;
            case WIDX_GUEST_INITIAL_HAPPINESS_INCREASE:
                if (gGuestInitialHappiness < 250)
                {
                    auto scenarioSetSetting = ScenarioSetSettingAction(
                        ScenarioSetSetting::GuestInitialHappiness, gGuestInitialHappiness + 4);
                    GameActions::Execute(&scenarioSetSetting);
                }
                else
                {
                    context_show_error(STR_CANT_INCREASE_FURTHER, STR_NONE, {});
                }
                Invalidate();
                break;
            case WIDX_GUEST_INITIAL_HAPPINESS_DECREASE:
                if (gGuestInitialHappiness > 40)
                {
                    auto scenarioSetSetting = ScenarioSetSettingAction(
                        ScenarioSetSetting::GuestInitialHappiness, gGuestInitialHappiness - 4);
                    GameActions::Execute(&scenarioSetSetting);
                }
                else
                {
                    context_show_error(STR_CANT_REDUCE_FURTHER, STR_NONE, {});
                }
                Invalidate();
                break;
            case WIDX_GUEST_INITIAL_HUNGER_INCREASE:
                if (gGuestInitialHunger > 40)
                {
                    auto scenarioSetSetting = ScenarioSetSettingAction(
                        ScenarioSetSetting::GuestInitialHunger, gGuestInitialHunger - 4);
                    GameActions::Execute(&scenarioSetSetting);
                }
                else
                {
                    context_show_error(STR_CANT_INCREASE_FURTHER, STR_NONE, {});
                }
                Invalidate();
                break;
            case WIDX_GUEST_INITIAL_HUNGER_DECREASE:
                if (gGuestInitialHunger < 250)
                {
                    auto scenarioSetSetting = ScenarioSetSettingAction(
                        ScenarioSetSetting::GuestInitialHunger, gGuestInitialHunger + 4);
                    GameActions::Execute(&scenarioSetSetting);
                }
                else
                {
                    context_show_error(STR_CANT_REDUCE_FURTHER, STR_NONE, {});
                }
                Invalidate();
                break;
            case WIDX_GUEST_INITIAL_THIRST_INCREASE:
                if (gGuestInitialThirst > 40)
                {
                    auto scenarioSetSetting = ScenarioSetSettingAction(
                        ScenarioSetSetting::GuestInitialThirst, gGuestInitialThirst - 4);
                    GameActions::Execute(&scenarioSetSetting);
                }
                else
                {
                    context_show_error(STR_CANT_INCREASE_FURTHER, STR_NONE, {});
                }
                Invalidate();
                break;
            case WIDX_GUEST_INITIAL_THIRST_DECREASE:
                if (gGuestInitialThirst < 250)
                {
                    auto scenarioSetSetting = ScenarioSetSettingAction(
                        ScenarioSetSetting::GuestInitialThirst, gGuestInitialThirst + 4);
                    GameActions::Execute(&scenarioSetSetting);
                }
                else
                {
                    context_show_error(STR_CANT_REDUCE_FURTHER, STR_NONE, {});
                }
                Invalidate();
                break;
        }
    }

    void GuestsUpdate()
    {
        frame_no++;
        GuestsPrepareDraw();
        widget_invalidate(*this, WIDX_TAB_2);
    }

    void GuestsPrepareDraw()
    {
        rct_widget* newWidgets = window_editor_scenario_options_widgets[page];
        if (widgets != newWidgets)
        {
            widgets = newWidgets;
            WindowInitScrollWidgets(*this);
        }

        SetPressedTab();

        if (gParkFlags & PARK_FLAGS_NO_MONEY)
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

        SetWidgetPressed(WIDX_GUEST_PREFER_LESS_INTENSE_RIDES, gParkFlags & PARK_FLAGS_PREF_LESS_INTENSE_RIDES);
        SetWidgetPressed(WIDX_GUEST_PREFER_MORE_INTENSE_RIDES, gParkFlags & PARK_FLAGS_PREF_MORE_INTENSE_RIDES);

        widgets[WIDX_CLOSE].type = (gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) ? WindowWidgetType::Empty
                                                                                 : WindowWidgetType::CloseBox;

        AnchorBorderWidgets();
    }

    void GuestsDraw(rct_drawpixelinfo& dpi)
    {
        ScreenCoordsXY screenCoords{};

        WindowDrawWidgets(*this, &dpi);
        DrawTabImages(dpi);

        const auto& cashPerGuestWidget = widgets[WIDX_CASH_PER_GUEST];
        if (cashPerGuestWidget.type != WindowWidgetType::Empty)
        {
            // Cash per guest label
            screenCoords = windowPos + ScreenCoordsXY{ 8, cashPerGuestWidget.top };
            DrawTextBasic(&dpi, screenCoords, STR_CASH_PER_GUEST_LABEL);

            // Cash per guest value
            screenCoords = windowPos + ScreenCoordsXY{ cashPerGuestWidget.left + 1, cashPerGuestWidget.top };
            auto ft = Formatter();
            ft.Add<money64>(gGuestInitialCash);
            DrawTextBasic(&dpi, screenCoords, STR_CURRENCY_FORMAT_LABEL, ft);
        }

        // Guest initial happiness label
        const auto& initialHappinessWidget = widgets[WIDX_GUEST_INITIAL_HAPPINESS];
        screenCoords = windowPos + ScreenCoordsXY{ 8, initialHappinessWidget.top };
        DrawTextBasic(&dpi, screenCoords, STR_GUEST_INIT_HAPPINESS);

        // Guest initial happiness value
        screenCoords = windowPos + ScreenCoordsXY{ initialHappinessWidget.left + 1, initialHappinessWidget.top };
        auto ft = Formatter();
        ft.Add<uint16_t>((gGuestInitialHappiness * 100) / 255);
        DrawTextBasic(&dpi, screenCoords, STR_PERCENT_FORMAT_LABEL, ft);

        // Guest initial hunger label
        const auto& initialHungerWidget = widgets[WIDX_GUEST_INITIAL_HUNGER];
        screenCoords = windowPos + ScreenCoordsXY{ 8, initialHungerWidget.top };
        DrawTextBasic(&dpi, screenCoords, STR_GUEST_INIT_HUNGER);

        // Guest initial hunger value
        screenCoords = windowPos + ScreenCoordsXY{ initialHungerWidget.left + 1, initialHungerWidget.top };
        ft = Formatter();
        ft.Add<uint16_t>(((255 - gGuestInitialHunger) * 100) / 255);
        DrawTextBasic(&dpi, screenCoords, STR_PERCENT_FORMAT_LABEL, ft);

        // Guest initial thirst label
        const auto& initialThirstWidget = widgets[WIDX_GUEST_INITIAL_THIRST];
        screenCoords = windowPos + ScreenCoordsXY{ 8, initialThirstWidget.top };
        DrawTextBasic(&dpi, screenCoords, STR_GUEST_INIT_THIRST);

        // Guest initial thirst value
        screenCoords = windowPos + ScreenCoordsXY{ initialThirstWidget.left + 1, initialThirstWidget.top };
        ft = Formatter();
        ft.Add<uint16_t>(((255 - gGuestInitialThirst) * 100) / 255);
        DrawTextBasic(&dpi, screenCoords, STR_PERCENT_FORMAT_LABEL, ft);
    }

#pragma endregion

#pragma region Park

    void ParkMouseUp(WidgetIndex widgetIndex)
    {
        switch (widgetIndex)
        {
            case WIDX_CLOSE:
                window_close(*this);
                break;
            case WIDX_TAB_1:
            case WIDX_TAB_2:
            case WIDX_TAB_3:
                SetPage(widgetIndex - WIDX_TAB_1);
                break;
            case WIDX_FORBID_TREE_REMOVAL:
            {
                auto scenarioSetSetting = ScenarioSetSettingAction(
                    ScenarioSetSetting::ForbidTreeRemoval, gParkFlags & PARK_FLAGS_FORBID_TREE_REMOVAL ? 0 : 1);
                GameActions::Execute(&scenarioSetSetting);
                Invalidate();
                break;
            }
            case WIDX_FORBID_LANDSCAPE_CHANGES:
            {
                auto scenarioSetSetting = ScenarioSetSettingAction(
                    ScenarioSetSetting::ForbidLandscapeChanges, gParkFlags & PARK_FLAGS_FORBID_LANDSCAPE_CHANGES ? 0 : 1);
                GameActions::Execute(&scenarioSetSetting);
                Invalidate();
                break;
            }
            case WIDX_FORBID_HIGH_CONSTRUCTION:
            {
                auto scenarioSetSetting = ScenarioSetSettingAction(
                    ScenarioSetSetting::ForbidHighConstruction, gParkFlags & PARK_FLAGS_FORBID_HIGH_CONSTRUCTION ? 0 : 1);
                GameActions::Execute(&scenarioSetSetting);
                Invalidate();
                break;
            }
            case WIDX_HARD_PARK_RATING:
            {
                auto scenarioSetSetting = ScenarioSetSettingAction(
                    ScenarioSetSetting::ParkRatingHigherDifficultyLevel, gParkFlags & PARK_FLAGS_DIFFICULT_PARK_RATING ? 0 : 1);
                GameActions::Execute(&scenarioSetSetting);
                Invalidate();
                break;
            }
            case WIDX_HARD_GUEST_GENERATION:
            {
                auto scenarioSetSetting = ScenarioSetSettingAction(
                    ScenarioSetSetting::GuestGenerationHigherDifficultyLevel,
                    gParkFlags & PARK_FLAGS_DIFFICULT_GUEST_GENERATION ? 0 : 1);
                GameActions::Execute(&scenarioSetSetting);
                Invalidate();
                break;
            }
        }
    }

    void ParkResize()
    {
        window_set_resize(*this, 400, 200, 400, 200);
    }

    void ParkMouseDown(WidgetIndex widgetIndex)
    {
        rct_widget* dropdownWidget;
        rct_widget* widget = &widgets[widgetIndex];

        switch (widgetIndex)
        {
            case WIDX_LAND_COST_INCREASE:
                if (gLandPrice < 200.00_GBP)
                {
                    auto scenarioSetSetting = ScenarioSetSettingAction(
                        ScenarioSetSetting::CostToBuyLand, gLandPrice + 1.00_GBP);
                    GameActions::Execute(&scenarioSetSetting);
                }
                else
                {
                    context_show_error(STR_CANT_INCREASE_FURTHER, STR_NONE, {});
                }
                Invalidate();
                break;
            case WIDX_LAND_COST_DECREASE:
                if (gLandPrice > 5.00_GBP)
                {
                    auto scenarioSetSetting = ScenarioSetSettingAction(
                        ScenarioSetSetting::CostToBuyLand, gLandPrice - 1.00_GBP);
                    GameActions::Execute(&scenarioSetSetting);
                }
                else
                {
                    context_show_error(STR_CANT_REDUCE_FURTHER, STR_NONE, {});
                }
                Invalidate();
                break;
            case WIDX_CONSTRUCTION_RIGHTS_COST_INCREASE:
                if (gConstructionRightsPrice < 200.00_GBP)
                {
                    auto scenarioSetSetting = ScenarioSetSettingAction(
                        ScenarioSetSetting::CostToBuyConstructionRights, gConstructionRightsPrice + 1.00_GBP);
                    GameActions::Execute(&scenarioSetSetting);
                }
                else
                {
                    context_show_error(STR_CANT_INCREASE_FURTHER, STR_NONE, {});
                }
                Invalidate();
                break;
            case WIDX_CONSTRUCTION_RIGHTS_COST_DECREASE:
                if (gConstructionRightsPrice > 5.00_GBP)
                {
                    auto scenarioSetSetting = ScenarioSetSettingAction(
                        ScenarioSetSetting::CostToBuyConstructionRights, gConstructionRightsPrice - 1.00_GBP);
                    GameActions::Execute(&scenarioSetSetting);
                }
                else
                {
                    context_show_error(STR_CANT_REDUCE_FURTHER, STR_NONE, {});
                }
                Invalidate();
                break;
            case WIDX_ENTRY_PRICE_INCREASE:
                if (gParkEntranceFee < MAX_ENTRANCE_FEE)
                {
                    auto scenarioSetSetting = ScenarioSetSettingAction(
                        ScenarioSetSetting::ParkChargeEntryFee, gParkEntranceFee + 1.00_GBP);
                    GameActions::Execute(&scenarioSetSetting);
                }
                else
                {
                    context_show_error(STR_CANT_INCREASE_FURTHER, STR_NONE, {});
                }
                Invalidate();
                break;
            case WIDX_ENTRY_PRICE_DECREASE:
                if (gParkEntranceFee > 0.00_GBP)
                {
                    auto scenarioSetSetting = ScenarioSetSettingAction(
                        ScenarioSetSetting::ParkChargeEntryFee, gParkEntranceFee - 1.00_GBP);
                    GameActions::Execute(&scenarioSetSetting);
                }
                else
                {
                    context_show_error(STR_CANT_REDUCE_FURTHER, STR_NONE, {});
                }
                Invalidate();
                break;
            case WIDX_PAY_FOR_PARK_OR_RIDES_DROPDOWN:
                dropdownWidget = widget - 1;

                gDropdownItems[0].Format = STR_DROPDOWN_MENU_LABEL;
                gDropdownItems[0].Args = STR_FREE_PARK_ENTER;
                gDropdownItems[1].Format = STR_DROPDOWN_MENU_LABEL;
                gDropdownItems[1].Args = STR_PAY_PARK_ENTER;
                gDropdownItems[2].Format = STR_DROPDOWN_MENU_LABEL;
                gDropdownItems[2].Args = STR_PAID_ENTRY_PAID_RIDES;

                WindowDropdownShowTextCustomWidth(
                    { windowPos.x + dropdownWidget->left, windowPos.y + dropdownWidget->top }, dropdownWidget->height() - 1,
                    colours[1], 0, Dropdown::Flag::StayOpen, 3, dropdownWidget->width() - 3);

                if (gParkFlags & PARK_FLAGS_UNLOCK_ALL_PRICES)
                    Dropdown::SetChecked(2, true);
                else if (gParkFlags & PARK_FLAGS_PARK_FREE_ENTRY)
                    Dropdown::SetChecked(0, true);
                else
                    Dropdown::SetChecked(1, true);

                break;
            case WIDX_CLIMATE_DROPDOWN:
                ShowClimateDropdown();
                break;
        }
    }

    void ParkDropdown(WidgetIndex widgetIndex, int32_t dropdownIndex)
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
            case WIDX_CLIMATE_DROPDOWN:
                if (static_cast<uint8_t>(gClimate) != static_cast<uint8_t>(dropdownIndex))
                {
                    auto gameAction = ClimateSetAction(ClimateType{ static_cast<uint8_t>(dropdownIndex) });
                    GameActions::Execute(&gameAction);
                }
                break;
        }
    }

    void ParkUpdate()
    {
        frame_no++;
        ParkPrepareDraw();
        widget_invalidate(*this, WIDX_TAB_3);
    }

    void ParkPrepareDraw()
    {
        rct_widget* newWidgets = window_editor_scenario_options_widgets[page];
        if (widgets != newWidgets)
        {
            widgets = newWidgets;
            WindowInitScrollWidgets(*this);
        }

        SetPressedTab();

        if (gParkFlags & PARK_FLAGS_NO_MONEY)
        {
            for (int32_t i = WIDX_LAND_COST; i <= WIDX_ENTRY_PRICE_DECREASE; i++)
                widgets[i].type = WindowWidgetType::Empty;
        }
        else
        {
            widgets[WIDX_LAND_COST].type = WindowWidgetType::Spinner;
            widgets[WIDX_LAND_COST_INCREASE].type = WindowWidgetType::Button;
            widgets[WIDX_LAND_COST_DECREASE].type = WindowWidgetType::Button;
            widgets[WIDX_CONSTRUCTION_RIGHTS_COST].type = WindowWidgetType::Spinner;
            widgets[WIDX_CONSTRUCTION_RIGHTS_COST_INCREASE].type = WindowWidgetType::Button;
            widgets[WIDX_CONSTRUCTION_RIGHTS_COST_DECREASE].type = WindowWidgetType::Button;
            widgets[WIDX_PAY_FOR_PARK_OR_RIDES].type = WindowWidgetType::DropdownMenu;
            widgets[WIDX_PAY_FOR_PARK_OR_RIDES_DROPDOWN].type = WindowWidgetType::Button;

            if (!park_entry_price_unlocked())
            {
                widgets[WIDX_ENTRY_PRICE].type = WindowWidgetType::Empty;
                widgets[WIDX_ENTRY_PRICE_INCREASE].type = WindowWidgetType::Empty;
                widgets[WIDX_ENTRY_PRICE_DECREASE].type = WindowWidgetType::Empty;
            }
            else
            {
                widgets[WIDX_ENTRY_PRICE].type = WindowWidgetType::Spinner;
                widgets[WIDX_ENTRY_PRICE_INCREASE].type = WindowWidgetType::Button;
                widgets[WIDX_ENTRY_PRICE_DECREASE].type = WindowWidgetType::Button;
            }
        }

        SetWidgetPressed(WIDX_FORBID_TREE_REMOVAL, gParkFlags & PARK_FLAGS_FORBID_TREE_REMOVAL);
        SetWidgetPressed(WIDX_FORBID_LANDSCAPE_CHANGES, gParkFlags & PARK_FLAGS_FORBID_LANDSCAPE_CHANGES);
        SetWidgetPressed(WIDX_FORBID_HIGH_CONSTRUCTION, gParkFlags & PARK_FLAGS_FORBID_HIGH_CONSTRUCTION);
        SetWidgetPressed(WIDX_HARD_PARK_RATING, gParkFlags & PARK_FLAGS_DIFFICULT_PARK_RATING);
        SetWidgetPressed(WIDX_HARD_GUEST_GENERATION, gParkFlags & PARK_FLAGS_DIFFICULT_GUEST_GENERATION);

        widgets[WIDX_CLOSE].type = (gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) ? WindowWidgetType::Empty
                                                                                 : WindowWidgetType::CloseBox;

        AnchorBorderWidgets();
    }

    void ParkDraw(rct_drawpixelinfo& dpi)
    {
        ScreenCoordsXY screenCoords{};

        WindowDrawWidgets(*this, &dpi);
        DrawTabImages(dpi);

        const auto& landCostWidget = widgets[WIDX_LAND_COST];
        if (landCostWidget.type != WindowWidgetType::Empty)
        {
            // Cost to buy land label
            screenCoords = windowPos + ScreenCoordsXY{ 8, landCostWidget.top };
            DrawTextBasic(&dpi, screenCoords, STR_LAND_COST_LABEL);

            // Cost to buy land value
            screenCoords = windowPos + ScreenCoordsXY{ landCostWidget.left + 1, landCostWidget.top };
            auto ft = Formatter();
            ft.Add<money64>(gLandPrice);
            DrawTextBasic(&dpi, screenCoords, STR_CURRENCY_FORMAT_LABEL, ft);
        }

        const auto& constructionRightsCostWidget = widgets[WIDX_CONSTRUCTION_RIGHTS_COST];
        if (constructionRightsCostWidget.type != WindowWidgetType::Empty)
        {
            // Cost to buy construction rights label
            screenCoords = windowPos + ScreenCoordsXY{ 8, constructionRightsCostWidget.top };
            DrawTextBasic(&dpi, screenCoords, STR_RIGHTS_COST_LABEL);

            // Cost to buy construction rights value
            screenCoords = windowPos
                + ScreenCoordsXY{ constructionRightsCostWidget.left + 1, constructionRightsCostWidget.top };
            auto ft = Formatter();
            ft.Add<money64>(gConstructionRightsPrice);
            DrawTextBasic(&dpi, screenCoords, STR_CURRENCY_FORMAT_LABEL, ft);
        }

        const auto& payForParkOrRidesWidget = widgets[WIDX_PAY_FOR_PARK_OR_RIDES];
        if (payForParkOrRidesWidget.type != WindowWidgetType::Empty)
        {
            // Pay for park or rides label
            screenCoords = windowPos + ScreenCoordsXY{ payForParkOrRidesWidget.left + 1, payForParkOrRidesWidget.top };

            auto ft = Formatter();
            // Pay for park and/or rides value
            if (gParkFlags & PARK_FLAGS_UNLOCK_ALL_PRICES)
                ft.Add<StringId>(STR_PAID_ENTRY_PAID_RIDES);
            else if (gParkFlags & PARK_FLAGS_PARK_FREE_ENTRY)
                ft.Add<StringId>(STR_FREE_PARK_ENTER);
            else
                ft.Add<StringId>(STR_PAY_PARK_ENTER);

            DrawTextBasic(&dpi, screenCoords, STR_WINDOW_COLOUR_2_STRINGID, ft);
        }

        const auto& entryPriceWidget = widgets[WIDX_ENTRY_PRICE];
        if (entryPriceWidget.type != WindowWidgetType::Empty)
        {
            // Entry price label
            screenCoords = windowPos + ScreenCoordsXY{ payForParkOrRidesWidget.right + 8, entryPriceWidget.top };
            DrawTextBasic(&dpi, screenCoords, STR_ENTRY_PRICE_LABEL);

            // Entry price value
            screenCoords = windowPos + ScreenCoordsXY{ entryPriceWidget.left + 1, entryPriceWidget.top };
            auto ft = Formatter();
            ft.Add<money64>(gParkEntranceFee);
            DrawTextBasic(&dpi, screenCoords, STR_CURRENCY_FORMAT_LABEL, ft);
        }

        // Climate label
        const auto& climateWidget = widgets[WIDX_CLIMATE];
        screenCoords = windowPos + ScreenCoordsXY{ 8, climateWidget.top };
        DrawTextBasic(&dpi, screenCoords, STR_CLIMATE_LABEL);

        // Climate value
        screenCoords = windowPos + ScreenCoordsXY{ climateWidget.left + 1, climateWidget.top };
        auto ft = Formatter();
        ft.Add<StringId>(ClimateNames[static_cast<uint8_t>(gClimate)]);
        DrawTextBasic(&dpi, screenCoords, STR_WINDOW_COLOUR_2_STRINGID, ft);
    }

#pragma endregion
};

rct_window* WindowEditorScenarioOptionsOpen()
{
    return WindowFocusOrCreate<EditorScenarioOptionsWindow>(WindowClass::EditorScenarioOptions, 280, 148, WF_NO_SCROLLING);
}
