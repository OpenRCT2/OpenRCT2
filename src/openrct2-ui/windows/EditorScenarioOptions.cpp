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

#include <openrct2/Context.h>
#include <openrct2/Editor.h>
#include <openrct2/Game.h>
#include <openrct2/GameState.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/SpriteIds.h>
#include <openrct2/actions/ScenarioSetSettingAction.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/entity/Peep.h>
#include <openrct2/interface/Colour.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/management/Finance.h>
#include <openrct2/ui/WindowManager.h>
#include <openrct2/world/Climate.h>
#include <openrct2/world/Park.h>

namespace OpenRCT2::Ui::Windows
{
    static constexpr int32_t WW_FINANCIAL = 280;
    static constexpr int32_t WH_FINANCIAL = 149;

    static constexpr int32_t WW_GUESTS = 380;
    static constexpr int32_t WH_GUESTS = 137;

    static constexpr int32_t WW_PARK = 400;
    static constexpr int32_t WH_PARK = 200;

#pragma region Widgets

    enum
    {
        WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_FINANCIAL,
        WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_GUESTS,
        WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_PARK,
        WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_COUNT
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
        WIDX_RCT1_INTEREST,

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
        WIDX_GUESTS_INTENSITY_PREFERENCE_LABEL,
        WIDX_GUESTS_INTENSITY_PREFERENCE,
        WIDX_GUESTS_INTENSITY_PREFERENCE_DROPDOWN,

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
        WIDX_FORBID_TREE_REMOVAL,
        WIDX_FORBID_LANDSCAPE_CHANGES,
        WIDX_FORBID_HIGH_CONSTRUCTION,
        WIDX_HARD_PARK_RATING,
        WIDX_HARD_GUEST_GENERATION
    };

    // clang-format off
    static constexpr Widget window_editor_scenario_options_financial_widgets[] = {
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
        MakeWidget        ({  8, 116}, {WW_FINANCIAL - 16,  12}, WindowWidgetType::Checkbox, WindowColour::Secondary, STR_RCT1_INTEREST,      STR_RCT1_INTEREST_TIP             ),
    };

    static constexpr Widget window_editor_scenario_options_guests_widgets[] = {
        WINDOW_SHIM(STR_SCENARIO_OPTIONS_GUESTS, WW_GUESTS, WH_GUESTS),
        MakeWidget        ({  0,  43}, {WW_GUESTS, 106}, WindowWidgetType::Resize,       WindowColour::Secondary),
        MakeRemapWidget   ({  3,  17}, {       31,  27}, WindowWidgetType::Tab,          WindowColour::Secondary, SPR_TAB,                              STR_SCENARIO_OPTIONS_FINANCIAL_TIP),
        MakeRemapWidget   ({ 34,  17}, {       31,  30}, WindowWidgetType::Tab,          WindowColour::Secondary, SPR_TAB,                              STR_SCENARIO_OPTIONS_GUESTS_TIP   ),
        MakeRemapWidget   ({ 65,  17}, {       31,  27}, WindowWidgetType::Tab,          WindowColour::Secondary, SPR_TAB,                              STR_SCENARIO_OPTIONS_PARK_TIP     ),
        MakeSpinnerWidgets({298,  48}, {       70,  12}, WindowWidgetType::Spinner,      WindowColour::Secondary                                                                          ), // NB: 3 widgets
        MakeSpinnerWidgets({298,  65}, {       70,  12}, WindowWidgetType::Spinner,      WindowColour::Secondary                                                                          ), // NB: 3 widgets
        MakeSpinnerWidgets({298,  82}, {       70,  12}, WindowWidgetType::Spinner,      WindowColour::Secondary                                                                          ), // NB: 3 widgets
        MakeSpinnerWidgets({298,  99}, {       70,  12}, WindowWidgetType::Spinner,      WindowColour::Secondary                                                                          ), // NB: 3 widgets
        MakeWidget        ({  8, 116}, {      170,  12}, WindowWidgetType::Label,        WindowColour::Secondary, STR_GUESTS_PREFER_INTENSITY_LABEL                                       ),
        MakeWidget        ({198, 116}, {      170,  12}, WindowWidgetType::DropdownMenu, WindowColour::Secondary, kStringIdNone,                        STR_GUESTS_PREFER_INTENSITY_TIP   ),
        MakeWidget        ({357, 117}, {       11,  10}, WindowWidgetType::Button,       WindowColour::Secondary, STR_DROPDOWN_GLYPH,                   STR_GUESTS_PREFER_INTENSITY_TIP   ),
    };

    static constexpr Widget window_editor_scenario_options_park_widgets[] = {
        WINDOW_SHIM(STR_SCENARIO_OPTIONS_PARK, WW_PARK, WH_PARK),
        MakeWidget        ({  0,  43}, {     WW_PARK, 106}, WindowWidgetType::Resize,   WindowColour::Secondary                                                                  ),
        MakeRemapWidget   ({  3,  17}, {          31,  27}, WindowWidgetType::Tab,      WindowColour::Secondary, SPR_TAB,                      STR_SCENARIO_OPTIONS_FINANCIAL_TIP),
        MakeRemapWidget   ({ 34,  17}, {          31,  30}, WindowWidgetType::Tab,      WindowColour::Secondary, SPR_TAB,                      STR_SCENARIO_OPTIONS_GUESTS_TIP   ),
        MakeRemapWidget   ({ 65,  17}, {          31,  27}, WindowWidgetType::Tab,      WindowColour::Secondary, SPR_TAB,                      STR_SCENARIO_OPTIONS_PARK_TIP     ),
        MakeSpinnerWidgets({188,  48}, {          70,  12}, WindowWidgetType::Spinner,  WindowColour::Secondary                                                                  ), // NB: 3 widgets
        MakeSpinnerWidgets({188,  65}, {          70,  12}, WindowWidgetType::Spinner,  WindowColour::Secondary                                                                  ), // NB: 3 widgets
        MakeWidget        ({  8,  82}, {         210,  12}, WindowWidgetType::DropdownMenu, WindowColour::Secondary, kStringIdNone,                     STR_PAY_FOR_PARK_PAY_FOR_RIDES_TIP),
        MakeWidget        ({206,  83}, {          11,  10}, WindowWidgetType::Button,   WindowColour::Secondary, STR_DROPDOWN_GLYPH,           STR_PAY_FOR_PARK_PAY_FOR_RIDES_TIP),
        MakeSpinnerWidgets({328,  82}, {          67,  12}, WindowWidgetType::Spinner,  WindowColour::Secondary                                                                  ), // NB: 3 widgets
        MakeWidget        ({  8, 116}, {WW_PARK - 16,  12}, WindowWidgetType::Checkbox, WindowColour::Secondary, STR_FORBID_TREE_REMOVAL,      STR_FORBID_TREE_REMOVAL_TIP       ),
        MakeWidget        ({  8, 133}, {WW_PARK - 16,  12}, WindowWidgetType::Checkbox, WindowColour::Secondary, STR_FORBID_LANDSCAPE_CHANGES, STR_FORBID_LANDSCAPE_CHANGES_TIP  ),
        MakeWidget        ({  8, 150}, {WW_PARK - 16,  12}, WindowWidgetType::Checkbox, WindowColour::Secondary, STR_FORBID_HIGH_CONSTRUCTION, STR_FORBID_HIGH_CONSTRUCTION_TIP  ),
        MakeWidget        ({  8, 167}, {WW_PARK - 16,  12}, WindowWidgetType::Checkbox, WindowColour::Secondary, STR_HARD_PARK_RATING,         STR_HARD_PARK_RATING_TIP          ),
        MakeWidget        ({  8, 184}, {WW_PARK - 16,  12}, WindowWidgetType::Checkbox, WindowColour::Secondary, STR_HARD_GUEST_GENERATION,    STR_HARD_GUEST_GENERATION_TIP     ),
    };

    static constexpr std::span<const Widget> window_editor_scenario_options_widgets[] = {
        window_editor_scenario_options_financial_widgets,
        window_editor_scenario_options_guests_widgets,
        window_editor_scenario_options_park_widgets,
    };

#pragma endregion

#pragma region Enabled widgets

    static uint32_t window_editor_scenario_options_page_hold_down_widgets[] = {
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
    };
    // clang-format on

#pragma endregion

    class EditorScenarioOptionsWindow final : public Window
    {
    public:
        void OnOpen() override
        {
            SetPage(WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_FINANCIAL);
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

        void OnDraw(DrawPixelInfo& dpi) override
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
                case WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_GUESTS:
                    return GuestsDropdown(widgetIndex, selectedIndex);
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
            ResizeFrameWithPage();
        }

        void DrawTabImages(DrawPixelInfo& dpi)
        {
            Widget* widget;
            int32_t spriteIndex;

            // Tab 1
            widget = &widgets[WIDX_TAB_1];
            spriteIndex = SPR_TAB_FINANCES_SUMMARY_0;
            if (page == WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_FINANCIAL)
                spriteIndex += (frame_no / 2) % 8;

            GfxDrawSprite(dpi, ImageId(spriteIndex), windowPos + ScreenCoordsXY{ widget->left, widget->top });

            // Tab 2
            widget = &widgets[WIDX_TAB_2];
            spriteIndex = SPR_TAB_GUESTS_0;
            if (page == WINDOW_EDITOR_SCENARIO_OPTIONS_PAGE_GUESTS)
                spriteIndex += (frame_no / 4) % 8;

            GfxDrawSprite(dpi, ImageId(spriteIndex), windowPos + ScreenCoordsXY{ widget->left, widget->top });

            // Tab 3
            widget = &widgets[WIDX_TAB_3];
            spriteIndex = SPR_TAB_PARK;
            GfxDrawSprite(dpi, ImageId(spriteIndex), windowPos + ScreenCoordsXY{ widget->left, widget->top });
        }

        void SetPage(int32_t newPage)
        {
            // Skip setting page if we're already on this page, unless we're initialising the window
            if (page == newPage && !widgets.empty())
                return;

            page = newPage;
            frame_no = 0;
            hold_down_widgets = window_editor_scenario_options_page_hold_down_widgets[page];
            SetWidgets(window_editor_scenario_options_widgets[page]);
            Invalidate();
            OnResize();
            OnPrepareDraw();
            InitScrollWidgets();
            Invalidate();
        }

#pragma region Financial

        void FinancialMouseUp(WidgetIndex widgetIndex)
        {
            auto& gameState = getGameState();
            switch (widgetIndex)
            {
                case WIDX_CLOSE:
                    Close();
                    break;
                case WIDX_TAB_1:
                case WIDX_TAB_2:
                case WIDX_TAB_3:
                    SetPage(widgetIndex - WIDX_TAB_1);
                    break;
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
            WindowSetResize(*this, { 280, 149 }, { 280, 149 });
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
            }

            if (gLegacyScene == LegacyScene::playing)
            {
                auto* windowMgr = Ui::GetWindowManager();
                windowMgr->InvalidateByClass(WindowClass::Finances);
                windowMgr->InvalidateByClass(WindowClass::BottomToolbar);
            }
        }

        void FinancialUpdate()
        {
            frame_no++;
            FinancialPrepareDraw();
            InvalidateWidget(WIDX_TAB_1);
        }

        void FinancialPrepareDraw()
        {
            SetPressedTab();

            auto& gameState = getGameState();
            if (gameState.park.Flags & PARK_FLAGS_NO_MONEY)
            {
                SetWidgetPressed(WIDX_NO_MONEY, true);
                for (int32_t i = WIDX_INITIAL_CASH; i <= WIDX_RCT1_INTEREST; i++)
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
                widgets[WIDX_FORBID_MARKETING].type = WindowWidgetType::Checkbox;

                if (gameState.park.Flags & PARK_FLAGS_RCT1_INTEREST)
                {
                    widgets[WIDX_INTEREST_RATE].type = WindowWidgetType::Empty;
                    widgets[WIDX_INTEREST_RATE_INCREASE].type = WindowWidgetType::Empty;
                    widgets[WIDX_INTEREST_RATE_DECREASE].type = WindowWidgetType::Empty;
                    widgets[WIDX_RCT1_INTEREST].type = WindowWidgetType::Checkbox;
                    SetWidgetPressed(WIDX_RCT1_INTEREST, true);
                }
                else
                {
                    widgets[WIDX_INTEREST_RATE].type = WindowWidgetType::Spinner;
                    widgets[WIDX_INTEREST_RATE_INCREASE].type = WindowWidgetType::Button;
                    widgets[WIDX_INTEREST_RATE_DECREASE].type = WindowWidgetType::Button;
                    widgets[WIDX_RCT1_INTEREST].type = WindowWidgetType::Empty;
                }
            }

            SetWidgetPressed(WIDX_FORBID_MARKETING, gameState.park.Flags & PARK_FLAGS_FORBID_MARKETING_CAMPAIGN);

            widgets[WIDX_CLOSE].type = gLegacyScene == LegacyScene::scenarioEditor ? WindowWidgetType::Empty
                                                                                   : WindowWidgetType::CloseBox;

            AnchorBorderWidgets();
        }

        void FinancialDraw(DrawPixelInfo& dpi)
        {
            ScreenCoordsXY screenCoords{};

            WindowDrawWidgets(*this, dpi);
            DrawTabImages(dpi);

            auto& gameState = getGameState();

            const auto& initialCashWidget = widgets[WIDX_INITIAL_CASH];
            if (initialCashWidget.type != WindowWidgetType::Empty)
            {
                screenCoords = windowPos + ScreenCoordsXY{ 8, initialCashWidget.top };
                DrawTextBasic(dpi, screenCoords, STR_INIT_CASH_LABEL);

                screenCoords = windowPos + ScreenCoordsXY{ initialCashWidget.left + 1, initialCashWidget.top };
                auto ft = Formatter();
                ft.Add<money64>(getGameState().initialCash);
                DrawTextBasic(dpi, screenCoords, STR_CURRENCY_FORMAT_LABEL, ft);
            }

            const auto& initialLoanWidget = widgets[WIDX_INITIAL_LOAN];
            if (initialLoanWidget.type != WindowWidgetType::Empty)
            {
                screenCoords = windowPos + ScreenCoordsXY{ 8, initialLoanWidget.top };
                DrawTextBasic(dpi, screenCoords, STR_INIT_LOAN_LABEL);

                screenCoords = windowPos + ScreenCoordsXY{ initialLoanWidget.left + 1, initialLoanWidget.top };
                auto ft = Formatter();
                ft.Add<money64>(gameState.bankLoan);
                DrawTextBasic(dpi, screenCoords, STR_CURRENCY_FORMAT_LABEL, ft);
            }

            const auto& maximumLoanWidget = widgets[WIDX_MAXIMUM_LOAN];
            if (maximumLoanWidget.type != WindowWidgetType::Empty)
            {
                screenCoords = windowPos + ScreenCoordsXY{ 8, maximumLoanWidget.top };
                DrawTextBasic(dpi, screenCoords, STR_MAX_LOAN_LABEL);

                screenCoords = windowPos + ScreenCoordsXY{ maximumLoanWidget.left + 1, maximumLoanWidget.top };
                auto ft = Formatter();
                ft.Add<money64>(getGameState().maxBankLoan);
                DrawTextBasic(dpi, screenCoords, STR_CURRENCY_FORMAT_LABEL, ft);
            }

            const auto& interestRateWidget = widgets[WIDX_INTEREST_RATE];
            if (interestRateWidget.type != WindowWidgetType::Empty)
            {
                screenCoords = windowPos + ScreenCoordsXY{ 8, interestRateWidget.top };
                DrawTextBasic(dpi, screenCoords, STR_INTEREST_RATE_LABEL);

                screenCoords = windowPos + ScreenCoordsXY{ interestRateWidget.left + 1, interestRateWidget.top };

                auto ft = Formatter();
                ft.Add<int16_t>(
                    std::clamp<int16_t>(static_cast<int16_t>(gameState.bankLoanInterestRate), INT16_MIN, INT16_MAX));
                DrawTextBasic(dpi, screenCoords, STR_PERCENT_FORMAT_LABEL, ft);
            }
        }

#pragma endregion

#pragma region Guests

        void GuestsMouseUp(WidgetIndex widgetIndex)
        {
            switch (widgetIndex)
            {
                case WIDX_CLOSE:
                    Close();
                    break;
                case WIDX_TAB_1:
                case WIDX_TAB_2:
                case WIDX_TAB_3:
                    SetPage(widgetIndex - WIDX_TAB_1);
                    break;
            }
        }

        void GuestsResize()
        {
            WindowSetResize(*this, { WW_GUESTS, WH_GUESTS }, { WW_GUESTS, WH_GUESTS });
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
            GuestsPrepareDraw();
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

            AnchorBorderWidgets();
        }

        void GuestsDraw(DrawPixelInfo& dpi)
        {
            ScreenCoordsXY screenCoords{};

            WindowDrawWidgets(*this, dpi);
            DrawTabImages(dpi);
            auto& gameState = getGameState();

            const auto& cashPerGuestWidget = widgets[WIDX_CASH_PER_GUEST];
            if (cashPerGuestWidget.type != WindowWidgetType::Empty)
            {
                // Cash per guest label
                screenCoords = windowPos + ScreenCoordsXY{ 8, cashPerGuestWidget.top };
                DrawTextBasic(dpi, screenCoords, STR_CASH_PER_GUEST_LABEL);

                // Cash per guest value
                screenCoords = windowPos + ScreenCoordsXY{ cashPerGuestWidget.left + 1, cashPerGuestWidget.top };
                auto ft = Formatter();
                ft.Add<money64>(gameState.guestInitialCash);
                DrawTextBasic(dpi, screenCoords, STR_CURRENCY_FORMAT_LABEL, ft);
            }

            // Guest initial happiness label
            const auto& initialHappinessWidget = widgets[WIDX_GUEST_INITIAL_HAPPINESS];
            screenCoords = windowPos + ScreenCoordsXY{ 8, initialHappinessWidget.top };
            DrawTextBasic(dpi, screenCoords, STR_GUEST_INIT_HAPPINESS);

            // Guest initial happiness value
            screenCoords = windowPos + ScreenCoordsXY{ initialHappinessWidget.left + 1, initialHappinessWidget.top };
            auto ft = Formatter();
            ft.Add<uint16_t>((gameState.guestInitialHappiness * 100) / 255);
            DrawTextBasic(dpi, screenCoords, STR_PERCENT_FORMAT_LABEL, ft);

            // Guest initial hunger label
            const auto& initialHungerWidget = widgets[WIDX_GUEST_INITIAL_HUNGER];
            screenCoords = windowPos + ScreenCoordsXY{ 8, initialHungerWidget.top };
            DrawTextBasic(dpi, screenCoords, STR_GUEST_INIT_HUNGER);

            // Guest initial hunger value
            screenCoords = windowPos + ScreenCoordsXY{ initialHungerWidget.left + 1, initialHungerWidget.top };
            ft = Formatter();
            ft.Add<uint16_t>(((255 - gameState.guestInitialHunger) * 100) / 255);
            DrawTextBasic(dpi, screenCoords, STR_PERCENT_FORMAT_LABEL, ft);

            // Guest initial thirst label
            const auto& initialThirstWidget = widgets[WIDX_GUEST_INITIAL_THIRST];
            screenCoords = windowPos + ScreenCoordsXY{ 8, initialThirstWidget.top };
            DrawTextBasic(dpi, screenCoords, STR_GUEST_INIT_THIRST);

            // Guest initial thirst value
            screenCoords = windowPos + ScreenCoordsXY{ initialThirstWidget.left + 1, initialThirstWidget.top };
            ft = Formatter();
            ft.Add<uint16_t>(((255 - gameState.guestInitialThirst) * 100) / 255);
            DrawTextBasic(dpi, screenCoords, STR_PERCENT_FORMAT_LABEL, ft);

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
                DrawTextBasic(dpi, screenCoords, STR_WINDOW_COLOUR_2_STRINGID, ft);
            }
        }

#pragma endregion

#pragma region Park

        void ParkMouseUp(WidgetIndex widgetIndex)
        {
            auto& gameState = getGameState();
            switch (widgetIndex)
            {
                case WIDX_CLOSE:
                    Close();
                    break;
                case WIDX_TAB_1:
                case WIDX_TAB_2:
                case WIDX_TAB_3:
                    SetPage(widgetIndex - WIDX_TAB_1);
                    break;
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
                case WIDX_HARD_PARK_RATING:
                {
                    auto scenarioSetSetting = ScenarioSetSettingAction(
                        ScenarioSetSetting::ParkRatingHigherDifficultyLevel,
                        gameState.park.Flags & PARK_FLAGS_DIFFICULT_PARK_RATING ? 0 : 1);
                    GameActions::Execute(&scenarioSetSetting);
                    Invalidate();
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

        void ParkResize()
        {
            WindowSetResize(*this, { 400, 200 }, { 400, 200 });
        }

        void ParkMouseDown(WidgetIndex widgetIndex)
        {
            Widget* dropdownWidget;
            Widget* widget = &widgets[widgetIndex];

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

                    if (gameState.park.Flags & PARK_FLAGS_UNLOCK_ALL_PRICES)
                        Dropdown::SetChecked(2, true);
                    else if (gameState.park.Flags & PARK_FLAGS_PARK_FREE_ENTRY)
                        Dropdown::SetChecked(0, true);
                    else
                        Dropdown::SetChecked(1, true);

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
            }
        }

        void ParkUpdate()
        {
            frame_no++;
            ParkPrepareDraw();
            InvalidateWidget(WIDX_TAB_3);
        }

        void ParkPrepareDraw()
        {
            SetPressedTab();

            auto& gameState = getGameState();
            if (gameState.park.Flags & PARK_FLAGS_NO_MONEY)
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

                if (!Park::EntranceFeeUnlocked())
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

            SetWidgetPressed(WIDX_FORBID_TREE_REMOVAL, gameState.park.Flags & PARK_FLAGS_FORBID_TREE_REMOVAL);
            SetWidgetPressed(WIDX_FORBID_LANDSCAPE_CHANGES, gameState.park.Flags & PARK_FLAGS_FORBID_LANDSCAPE_CHANGES);
            SetWidgetPressed(WIDX_FORBID_HIGH_CONSTRUCTION, gameState.park.Flags & PARK_FLAGS_FORBID_HIGH_CONSTRUCTION);
            SetWidgetPressed(WIDX_HARD_PARK_RATING, gameState.park.Flags & PARK_FLAGS_DIFFICULT_PARK_RATING);
            SetWidgetPressed(WIDX_HARD_GUEST_GENERATION, gameState.park.Flags & PARK_FLAGS_DIFFICULT_GUEST_GENERATION);

            widgets[WIDX_CLOSE].type = gLegacyScene == LegacyScene::scenarioEditor ? WindowWidgetType::Empty
                                                                                   : WindowWidgetType::CloseBox;

            AnchorBorderWidgets();
        }

        void ParkDraw(DrawPixelInfo& dpi)
        {
            ScreenCoordsXY screenCoords{};

            WindowDrawWidgets(*this, dpi);
            DrawTabImages(dpi);

            const auto& gameState = getGameState();
            const auto& landCostWidget = widgets[WIDX_LAND_COST];
            if (landCostWidget.type != WindowWidgetType::Empty)
            {
                // Cost to buy land label
                screenCoords = windowPos + ScreenCoordsXY{ 8, landCostWidget.top };
                DrawTextBasic(dpi, screenCoords, STR_LAND_COST_LABEL);

                // Cost to buy land value
                screenCoords = windowPos + ScreenCoordsXY{ landCostWidget.left + 1, landCostWidget.top };
                auto ft = Formatter();
                ft.Add<money64>(gameState.landPrice);
                DrawTextBasic(dpi, screenCoords, STR_CURRENCY_FORMAT_LABEL, ft);
            }

            const auto& constructionRightsCostWidget = widgets[WIDX_CONSTRUCTION_RIGHTS_COST];
            if (constructionRightsCostWidget.type != WindowWidgetType::Empty)
            {
                // Cost to buy construction rights label
                screenCoords = windowPos + ScreenCoordsXY{ 8, constructionRightsCostWidget.top };
                DrawTextBasic(dpi, screenCoords, STR_RIGHTS_COST_LABEL);

                // Cost to buy construction rights value
                screenCoords = windowPos
                    + ScreenCoordsXY{ constructionRightsCostWidget.left + 1, constructionRightsCostWidget.top };
                auto ft = Formatter();
                ft.Add<money64>(gameState.constructionRightsPrice);
                DrawTextBasic(dpi, screenCoords, STR_CURRENCY_FORMAT_LABEL, ft);
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

                DrawTextBasic(dpi, screenCoords, STR_WINDOW_COLOUR_2_STRINGID, ft);
            }

            const auto& entryPriceWidget = widgets[WIDX_ENTRY_PRICE];
            if (entryPriceWidget.type != WindowWidgetType::Empty)
            {
                // Entry price label
                screenCoords = windowPos + ScreenCoordsXY{ payForParkOrRidesWidget.right + 8, entryPriceWidget.top };
                DrawTextBasic(dpi, screenCoords, STR_ENTRY_PRICE_LABEL);

                // Entry price value
                screenCoords = windowPos + ScreenCoordsXY{ entryPriceWidget.left + 1, entryPriceWidget.top };
                auto ft = Formatter();
                ft.Add<money64>(gameState.park.EntranceFee);
                DrawTextBasic(dpi, screenCoords, STR_CURRENCY_FORMAT_LABEL, ft);
            }
        }

#pragma endregion
    };

    WindowBase* EditorScenarioOptionsOpen()
    {
        auto* windowMgr = GetWindowManager();
        return windowMgr->FocusOrCreate<EditorScenarioOptionsWindow>(
            WindowClass::EditorScenarioOptions, 280, 148, WF_NO_SCROLLING);
    }
} // namespace OpenRCT2::Ui::Windows
