/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <openrct2-ui/interface/Dropdown.h>
#include <openrct2-ui/interface/Graph.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Windows.h>
#include <openrct2/GameState.h>
#include <openrct2/actions/ParkSetLoanAction.h>
#include <openrct2/actions/ParkSetResearchFundingAction.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/localisation/Formatting.h>
#include <openrct2/localisation/Localisation.Date.h>
#include <openrct2/management/Finance.h>
#include <openrct2/ride/RideData.h>
#include <openrct2/ride/ShopItem.h>
#include <openrct2/scenario/Scenario.h>
#include <openrct2/sprites.h>
#include <openrct2/ui/WindowManager.h>
#include <openrct2/world/Park.h>

namespace OpenRCT2::Ui::Windows
{
    enum
    {
        WINDOW_FINANCES_PAGE_SUMMARY,
        WINDOW_FINANCES_PAGE_FINANCIAL_GRAPH,
        WINDOW_FINANCES_PAGE_VALUE_GRAPH,
        WINDOW_FINANCES_PAGE_PROFIT_GRAPH,
        WINDOW_FINANCES_PAGE_MARKETING,
        WINDOW_FINANCES_PAGE_RESEARCH,
        WINDOW_FINANCES_PAGE_COUNT
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

        WIDX_TAB_CONTENT,

        WIDX_SUMMARY_SCROLL = WIDX_TAB_CONTENT,
        WIDX_LOAN,
        WIDX_LOAN_INCREASE,
        WIDX_LOAN_DECREASE,

        WIDX_ACTIVE_CAMPAIGNS_GROUP = WIDX_TAB_CONTENT,
        WIDX_CAMPAIGNS_AVAILABLE_GROUP,
        WIDX_CAMPAIGN_1,
        WIDX_CAMPAIGN_2,
        WIDX_CAMPAIGN_3,
        WIDX_CAMPAIGN_4,
        WIDX_CAMPAIGN_5,
        WIDX_CAMPAIGN_6,

        WIDX_RESEARCH_FUNDING_GROUP = WIDX_TAB_CONTENT,
        WIDX_RESEARCH_FUNDING,
        WIDX_RESEARCH_FUNDING_DROPDOWN_BUTTON,
        WIDX_RESEARCH_PRIORITIES_GROUP,
        WIDX_TRANSPORT_RIDES,
        WIDX_GENTLE_RIDES,
        WIDX_ROLLER_COASTERS,
        WIDX_THRILL_RIDES,
        WIDX_WATER_RIDES,
        WIDX_SHOPS_AND_STALLS,
        WIDX_SCENERY_AND_THEMING,
    };

#pragma region Measurements

    static constexpr int32_t WH_SUMMARY = 309;
    static constexpr int32_t WH_RESEARCH = 207;
    static constexpr int32_t WH_OTHER_TABS = 257;
    static constexpr int32_t WW_RESEARCH = 320;
    static constexpr int32_t WW_OTHER_TABS = 530;
    static constexpr int32_t RSH_SUMMARY = 266;
    static constexpr int32_t RSH_RESEARCH = 164;
    static constexpr int32_t RSH_OTHER_TABS = 214;
    static constexpr int32_t RSW_RESEARCH = WW_RESEARCH;
    static constexpr int32_t RSW_OTHER_TABS = WW_OTHER_TABS;

#pragma endregion

    // clang-format off
#pragma region Widgets

#define MAIN_FINANCES_WIDGETS(TITLE, RSW, RSH, WW, WH)                                          \
    WINDOW_SHIM(TITLE, WW, WH),                                                                 \
        MakeWidget({ 0, 43 }, { RSW, RSH }, WindowWidgetType::Resize, WindowColour::Secondary), \
        MakeTab({ 3, 17 }, STR_FINANCES_SHOW_SUMMARY_TAB_TIP),                                  \
        MakeTab({ 34, 17 }, STR_FINANCES_SHOW_CASH_TAB_TIP),                                    \
        MakeTab({ 65, 17 }, STR_FINANCES_SHOW_PARK_VALUE_TAB_TIP),                              \
        MakeTab({ 96, 17 }, STR_FINANCES_SHOW_WEEKLY_PROFIT_TAB_TIP),                           \
        MakeTab({ 127, 17 }, STR_FINANCES_SHOW_MARKETING_TAB_TIP),                              \
        MakeTab({ 158, 17 }, STR_FINANCES_RESEARCH_TIP)

    static constexpr Widget _windowFinancesSummaryWidgets[] =
    {
        MAIN_FINANCES_WIDGETS(STR_FINANCIAL_SUMMARY, RSW_OTHER_TABS, RSH_SUMMARY, WW_OTHER_TABS, WH_SUMMARY),
        MakeWidget        ({130,  50}, {391, 211}, WindowWidgetType::Scroll,  WindowColour::Secondary, SCROLL_HORIZONTAL              ),
        MakeSpinnerWidgets({ 64, 279}, { 97,  14}, WindowWidgetType::Spinner, WindowColour::Secondary, STR_FINANCES_SUMMARY_LOAN_VALUE), // NB: 3 widgets.
    };

    static constexpr Widget _windowFinancesCashWidgets[] =
    {
        MAIN_FINANCES_WIDGETS(STR_FINANCIAL_GRAPH, RSW_OTHER_TABS, RSH_OTHER_TABS, WW_OTHER_TABS, WH_OTHER_TABS),
    };

    static constexpr Widget _windowFinancesParkValueWidgets[] =
    {
        MAIN_FINANCES_WIDGETS(STR_PARK_VALUE_GRAPH, RSW_OTHER_TABS, RSH_OTHER_TABS, WW_OTHER_TABS, WH_OTHER_TABS),
    };

    static constexpr Widget _windowFinancesProfitWidgets[] =
    {
        MAIN_FINANCES_WIDGETS(STR_PROFIT_GRAPH, RSW_OTHER_TABS, RSH_OTHER_TABS, WW_OTHER_TABS, WH_OTHER_TABS),
    };

    static constexpr Widget _windowFinancesMarketingWidgets[] =
    {
        MAIN_FINANCES_WIDGETS(STR_MARKETING, RSW_OTHER_TABS, RSH_OTHER_TABS, WW_OTHER_TABS, WH_OTHER_TABS),
        MakeWidget({3, 47}, { WW_OTHER_TABS - 6,  45}, WindowWidgetType::Groupbox, WindowColour::Tertiary , STR_MARKETING_CAMPAIGNS_IN_OPERATION                                   ),
        MakeWidget({3, 47}, { WW_OTHER_TABS - 6, 206}, WindowWidgetType::Groupbox, WindowColour::Tertiary , STR_MARKETING_CAMPAIGNS_AVAILABLE                                      ),
        MakeWidget({8,  0}, {WW_OTHER_TABS - 16,  14}, WindowWidgetType::ImgBtn,   WindowColour::Secondary, 0xFFFFFFFF,                           STR_START_THIS_MARKETING_CAMPAIGN),
        MakeWidget({8,  0}, {WW_OTHER_TABS - 16,  14}, WindowWidgetType::ImgBtn,   WindowColour::Secondary, 0xFFFFFFFF,                           STR_START_THIS_MARKETING_CAMPAIGN),
        MakeWidget({8,  0}, {WW_OTHER_TABS - 16,  14}, WindowWidgetType::ImgBtn,   WindowColour::Secondary, 0xFFFFFFFF,                           STR_START_THIS_MARKETING_CAMPAIGN),
        MakeWidget({8,  0}, {WW_OTHER_TABS - 16,  14}, WindowWidgetType::ImgBtn,   WindowColour::Secondary, 0xFFFFFFFF,                           STR_START_THIS_MARKETING_CAMPAIGN),
        MakeWidget({8,  0}, {WW_OTHER_TABS - 16,  14}, WindowWidgetType::ImgBtn,   WindowColour::Secondary, 0xFFFFFFFF,                           STR_START_THIS_MARKETING_CAMPAIGN),
        MakeWidget({8,  0}, {WW_OTHER_TABS - 16,  14}, WindowWidgetType::ImgBtn,   WindowColour::Secondary, 0xFFFFFFFF,                           STR_START_THIS_MARKETING_CAMPAIGN),
    };

    static constexpr Widget _windowFinancesResearchWidgets[] =
    {
        MAIN_FINANCES_WIDGETS(STR_RESEARCH_FUNDING, RSW_RESEARCH, RSH_RESEARCH, WW_RESEARCH, WH_RESEARCH),
        MakeWidget({  3,  47}, { WW_RESEARCH - 6,  45}, WindowWidgetType::Groupbox, WindowColour::Tertiary, STR_RESEARCH_FUNDING_                                                             ),
        MakeWidget({  8,  59}, {             160,  14}, WindowWidgetType::DropdownMenu, WindowColour::Tertiary, 0xFFFFFFFF,                           STR_SELECT_LEVEL_OF_RESEARCH_AND_DEVELOPMENT),
        MakeWidget({156,  60}, {              11,  12}, WindowWidgetType::Button,   WindowColour::Tertiary, STR_DROPDOWN_GLYPH,                   STR_SELECT_LEVEL_OF_RESEARCH_AND_DEVELOPMENT),
        MakeWidget({  3,  96}, { WW_RESEARCH - 6, 107}, WindowWidgetType::Groupbox, WindowColour::Tertiary, STR_RESEARCH_PRIORITIES                                                           ),
        MakeWidget({  8, 108}, {WW_RESEARCH - 14,  12}, WindowWidgetType::Checkbox, WindowColour::Tertiary, STR_RESEARCH_NEW_TRANSPORT_RIDES,     STR_RESEARCH_NEW_TRANSPORT_RIDES_TIP        ),
        MakeWidget({  8, 121}, {WW_RESEARCH - 14,  12}, WindowWidgetType::Checkbox, WindowColour::Tertiary, STR_RESEARCH_NEW_GENTLE_RIDES,        STR_RESEARCH_NEW_GENTLE_RIDES_TIP           ),
        MakeWidget({  8, 134}, {WW_RESEARCH - 14,  12}, WindowWidgetType::Checkbox, WindowColour::Tertiary, STR_RESEARCH_NEW_ROLLER_COASTERS,     STR_RESEARCH_NEW_ROLLER_COASTERS_TIP        ),
        MakeWidget({  8, 147}, {WW_RESEARCH - 14,  12}, WindowWidgetType::Checkbox, WindowColour::Tertiary, STR_RESEARCH_NEW_THRILL_RIDES,        STR_RESEARCH_NEW_THRILL_RIDES_TIP           ),
        MakeWidget({  8, 160}, {WW_RESEARCH - 14,  12}, WindowWidgetType::Checkbox, WindowColour::Tertiary, STR_RESEARCH_NEW_WATER_RIDES,         STR_RESEARCH_NEW_WATER_RIDES_TIP            ),
        MakeWidget({  8, 173}, {WW_RESEARCH - 14,  12}, WindowWidgetType::Checkbox, WindowColour::Tertiary, STR_RESEARCH_NEW_SHOPS_AND_STALLS,    STR_RESEARCH_NEW_SHOPS_AND_STALLS_TIP       ),
        MakeWidget({  8, 186}, {WW_RESEARCH - 14,  12}, WindowWidgetType::Checkbox, WindowColour::Tertiary, STR_RESEARCH_NEW_SCENERY_AND_THEMING, STR_RESEARCH_NEW_SCENERY_AND_THEMING_TIP    ),
    };
    // clang-format on

    static constexpr std::span<const Widget> _windowFinancesPageWidgets[] = {
        _windowFinancesSummaryWidgets,   // WINDOW_FINANCES_PAGE_SUMMARY
        _windowFinancesCashWidgets,      // WINDOW_FINANCES_PAGE_FINANCIAL_GRAPH
        _windowFinancesParkValueWidgets, // WINDOW_FINANCES_PAGE_VALUE_GRAPH
        _windowFinancesProfitWidgets,    // WINDOW_FINANCES_PAGE_PROFIT_GRAPH
        _windowFinancesMarketingWidgets, // WINDOW_FINANCES_PAGE_MARKETING
        _windowFinancesResearchWidgets,  // WINDOW_FINANCES_PAGE_RESEARCH
    };
    static_assert(std::size(_windowFinancesPageWidgets) == WINDOW_FINANCES_PAGE_COUNT);

#pragma endregion

#pragma region Constants

    static constexpr StringId _windowFinancesSummaryRowLabels[EnumValue(ExpenditureType::Count)] = {
        STR_FINANCES_SUMMARY_RIDE_CONSTRUCTION,
        STR_FINANCES_SUMMARY_RIDE_RUNNING_COSTS,
        STR_FINANCES_SUMMARY_LAND_PURCHASE,
        STR_FINANCES_SUMMARY_LANDSCAPING,
        STR_FINANCES_SUMMARY_PARK_ENTRANCE_TICKETS,
        STR_FINANCES_SUMMARY_RIDE_TICKETS,
        STR_FINANCES_SUMMARY_SHOP_SALES,
        STR_FINANCES_SUMMARY_SHOP_STOCK,
        STR_FINANCES_SUMMARY_FOOD_DRINK_SALES,
        STR_FINANCES_SUMMARY_FOOD_DRINK_STOCK,
        STR_FINANCES_SUMMARY_STAFF_WAGES,
        STR_FINANCES_SUMMARY_MARKETING,
        STR_FINANCES_SUMMARY_RESEARCH,
        STR_FINANCES_SUMMARY_LOAN_INTEREST,
    };

    static constexpr int32_t _windowFinancesTabAnimationFrames[] = {
        8,  // WINDOW_FINANCES_PAGE_SUMMARY
        16, // WINDOW_FINANCES_PAGE_FINANCIAL_GRAPH
        16, // WINDOW_FINANCES_PAGE_VALUE_GRAPH
        16, // WINDOW_FINANCES_PAGE_PROFIT_GRAPH
        19, // WINDOW_FINANCES_PAGE_MARKETING
        8,  // WINDOW_FINANCES_PAGE_RESEARCH
    };
    static_assert(std::size(_windowFinancesTabAnimationFrames) == WINDOW_FINANCES_PAGE_COUNT);

    static constexpr int32_t EXPENDITURE_COLUMN_WIDTH = 80;

    static constexpr uint32_t _windowFinancesPageHoldDownWidgets[] = {
        (1uLL << WIDX_LOAN_INCREASE) | (1uLL << WIDX_LOAN_DECREASE), // WINDOW_FINANCES_PAGE_SUMMARY

        0, // WINDOW_FINANCES_PAGE_FINANCIAL_GRAPH
        0, // WINDOW_FINANCES_PAGE_VALUE_GRAPH
        0, // WINDOW_FINANCES_PAGE_PROFIT_GRAPH
        0, // WINDOW_FINANCES_PAGE_MARKETING
        0, // WINDOW_FINANCES_PAGE_RESEARCH
    };
    static_assert(std::size(_windowFinancesPageHoldDownWidgets) == WINDOW_FINANCES_PAGE_COUNT);

    static constexpr ScreenCoordsXY kGraphTopLeftPadding{ 88, 20 };
    static constexpr ScreenCoordsXY kGraphBottomRightPadding{ 15, 18 };
    static constexpr uint8_t kGraphNumYLabels = 5;
    static constexpr int32_t kGraphNumPoints = 64;

#pragma endregion

    class FinancesWindow final : public Window
    {
    private:
        uint32_t _lastPaintedMonth = std::numeric_limits<uint32_t>::max();
        ScreenRect _graphBounds;
        Graph::GraphProperties<money64> _graphProps{};

        void SetDisabledTabs()
        {
            disabled_widgets = (GetGameState().Park.Flags & PARK_FLAGS_FORBID_MARKETING_CAMPAIGN) ? (1uLL << WIDX_TAB_5) : 0;
        }

    public:
        void OnOpen() override
        {
            SetPage(WINDOW_FINANCES_PAGE_SUMMARY);
            _lastPaintedMonth = std::numeric_limits<uint32_t>::max();
            ResearchUpdateUncompletedTypes();
            _graphProps.hoverIdx = -1;
        }

        void OnUpdate() override
        {
            frame_no++;
            InvalidateWidget(WIDX_TAB_1 + page);

            if (page == WINDOW_FINANCES_PAGE_VALUE_GRAPH || page == WINDOW_FINANCES_PAGE_PROFIT_GRAPH
                || page == WINDOW_FINANCES_PAGE_FINANCIAL_GRAPH)
            {
                if (_graphProps.UpdateHoverIndex())
                {
                    InvalidateWidget(WIDX_BACKGROUND);
                }
            }
        }

        void OnMouseDown(WidgetIndex widgetIndex) override
        {
            switch (page)
            {
                case WINDOW_FINANCES_PAGE_SUMMARY:
                    OnMouseDownSummary(widgetIndex);
                    break;
                case WINDOW_FINANCES_PAGE_RESEARCH:
                    WindowResearchFundingMouseDown(this, widgetIndex, WIDX_RESEARCH_FUNDING);
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
                    SetPage(widgetIndex - WIDX_TAB_1);
                    break;
                default:
                    switch (page)
                    {
                        case WINDOW_FINANCES_PAGE_MARKETING:
                            OnMouseUpMarketing(widgetIndex);
                            break;
                        case WINDOW_FINANCES_PAGE_RESEARCH:
                            WindowResearchFundingMouseUp(widgetIndex, WIDX_RESEARCH_FUNDING);
                    }
                    break;
            }
        }

        void OnDropdown(WidgetIndex widgetIndex, int32_t selectedIndex) override
        {
            if (page == WINDOW_FINANCES_PAGE_RESEARCH)
            {
                WindowResearchFundingDropdown(widgetIndex, selectedIndex, WIDX_RESEARCH_FUNDING);
            }
        }

        void OnPrepareDraw() override
        {
            WindowAlignTabs(this, WIDX_TAB_1, WIDX_TAB_6);

            for (auto i = 0; i < WINDOW_FINANCES_PAGE_COUNT; i++)
                SetWidgetPressed(WIDX_TAB_1 + i, false);
            SetWidgetPressed(WIDX_TAB_1 + page, true);

            Widget* graphPageWidget;
            bool centredGraph;
            switch (page)
            {
                case WINDOW_FINANCES_PAGE_SUMMARY:
                    OnPrepareDrawSummary();
                    return;
                case WINDOW_FINANCES_PAGE_MARKETING:
                    OnPrepareDrawMarketing();
                    return;
                case WINDOW_FINANCES_PAGE_RESEARCH:
                    WindowResearchFundingPrepareDraw(this, WIDX_RESEARCH_FUNDING);
                    return;
                default:
                    return;

                case WINDOW_FINANCES_PAGE_VALUE_GRAPH:
                    graphPageWidget = &widgets[WIDX_PAGE_BACKGROUND];
                    centredGraph = false;
                    _graphProps.series = GetGameState().Park.ValueHistory;
                    break;
                case WINDOW_FINANCES_PAGE_PROFIT_GRAPH:
                    graphPageWidget = &widgets[WIDX_PAGE_BACKGROUND];
                    centredGraph = true;
                    _graphProps.series = GetGameState().WeeklyProfitHistory;
                    break;
                case WINDOW_FINANCES_PAGE_FINANCIAL_GRAPH:
                    graphPageWidget = &widgets[WIDX_PAGE_BACKGROUND];
                    centredGraph = true;
                    _graphProps.series = GetGameState().CashHistory;
                    break;
            }
            OnPrepareDrawGraph(graphPageWidget, centredGraph);
        }

        void OnDraw(DrawPixelInfo& dpi) override
        {
            DrawWidgets(dpi);
            DrawTabImages(dpi);

            switch (page)
            {
                case WINDOW_FINANCES_PAGE_SUMMARY:
                    OnDrawSummary(dpi);
                    break;
                case WINDOW_FINANCES_PAGE_FINANCIAL_GRAPH:
                {
                    auto& gameState = GetGameState();
                    const auto cashLessLoan = gameState.Cash - gameState.BankLoan;
                    const auto fmt = cashLessLoan >= 0 ? STR_FINANCES_FINANCIAL_GRAPH_CASH_LESS_LOAN_POSITIVE
                                                       : STR_FINANCES_FINANCIAL_GRAPH_CASH_LESS_LOAN_NEGATIVE;
                    OnDrawGraph(dpi, cashLessLoan, fmt);
                    break;
                }
                case WINDOW_FINANCES_PAGE_VALUE_GRAPH:
                    OnDrawGraph(dpi, GetGameState().Park.Value, STR_FINANCES_PARK_VALUE);
                    break;
                case WINDOW_FINANCES_PAGE_PROFIT_GRAPH:
                {
                    auto& gameState = GetGameState();
                    const auto fmt = gameState.CurrentProfit >= 0 ? STR_FINANCES_WEEKLY_PROFIT_POSITIVE
                                                                  : STR_FINANCES_WEEKLY_PROFIT_LOSS;
                    OnDrawGraph(dpi, gameState.CurrentProfit, fmt);
                    break;
                }
                case WINDOW_FINANCES_PAGE_MARKETING:
                    OnDrawMarketing(dpi);
                    break;
                case WINDOW_FINANCES_PAGE_RESEARCH:
                    WindowResearchFundingDraw(this, dpi);
                    break;
            }
        }

        ScreenSize OnScrollGetSize(int32_t scrollIndex) override
        {
            if (page == WINDOW_FINANCES_PAGE_SUMMARY)
            {
                return { EXPENDITURE_COLUMN_WIDTH * (SummaryMaxAvailableMonth() + 1), 0 };
            }

            return {};
        }

        void OnScrollDraw(int32_t scrollIndex, DrawPixelInfo& dpi) override
        {
            if (page != WINDOW_FINANCES_PAGE_SUMMARY)
                return;

            auto screenCoords = ScreenCoordsXY{ 0, kTableCellHeight + 2 };

            Widget self = widgets[WIDX_SUMMARY_SCROLL];
            int32_t row_width = std::max<uint16_t>(scrolls[0].contentWidth, self.width());

            // Expenditure / Income row labels
            for (int32_t i = 0; i < static_cast<int32_t>(ExpenditureType::Count); i++)
            {
                // Darken every even row
                if (i % 2 == 0)
                    GfxFillRect(
                        dpi,
                        { screenCoords - ScreenCoordsXY{ 0, 1 },
                          screenCoords + ScreenCoordsXY{ row_width, (kTableCellHeight - 2) } },
                        ColourMapA[colours[1].colour].lighter | 0x1000000);

                screenCoords.y += kTableCellHeight;
            }

            auto& gameState = GetGameState();
            // Expenditure / Income values for each month
            auto currentMonthYear = GetDate().GetMonthsElapsed();
            for (int32_t i = SummaryMaxAvailableMonth(); i >= 0; i--)
            {
                screenCoords.y = 0;

                uint16_t monthyear = currentMonthYear - i;

                // Month heading
                auto ft = Formatter();
                ft.Add<StringId>(STR_FINANCES_SUMMARY_MONTH_HEADING);
                ft.Add<uint16_t>(monthyear);
                DrawTextBasic(
                    dpi, screenCoords + ScreenCoordsXY{ EXPENDITURE_COLUMN_WIDTH, 0 },
                    monthyear == currentMonthYear ? STR_WINDOW_COLOUR_2_STRINGID : STR_BLACK_STRING, ft,
                    { TextUnderline::On, TextAlignment::RIGHT });
                screenCoords.y += 14;

                // Month expenditures
                money64 profit = 0;
                for (int32_t j = 0; j < static_cast<int32_t>(ExpenditureType::Count); j++)
                {
                    auto expenditure = gameState.ExpenditureTable[i][j];
                    if (expenditure != 0)
                    {
                        profit += expenditure;
                        const StringId format = expenditure >= 0 ? STR_FINANCES_SUMMARY_INCOME_VALUE
                                                                 : STR_FINANCES_SUMMARY_EXPENDITURE_VALUE;
                        ft = Formatter();
                        ft.Add<money64>(expenditure);
                        DrawTextBasic(
                            dpi, screenCoords + ScreenCoordsXY{ EXPENDITURE_COLUMN_WIDTH, 0 }, format, ft,
                            { TextAlignment::RIGHT });
                    }
                    screenCoords.y += kTableCellHeight;
                }
                screenCoords.y += 4;

                // Month profit
                const StringId format = profit >= 0 ? STR_FINANCES_SUMMARY_INCOME_VALUE : STR_FINANCES_SUMMARY_LOSS_VALUE;
                ft = Formatter();
                ft.Add<money64>(profit);
                DrawTextBasic(
                    dpi, screenCoords + ScreenCoordsXY{ EXPENDITURE_COLUMN_WIDTH, 0 }, format, ft, { TextAlignment::RIGHT });

                GfxFillRect(
                    dpi,
                    { screenCoords + ScreenCoordsXY{ 10, -2 }, screenCoords + ScreenCoordsXY{ EXPENDITURE_COLUMN_WIDTH, -2 } },
                    PALETTE_INDEX_10);

                screenCoords.x += EXPENDITURE_COLUMN_WIDTH;
            }

            _lastPaintedMonth = currentMonthYear;
        }

        void SetPage(int32_t p)
        {
            page = p;
            frame_no = 0;

            hold_down_widgets = _windowFinancesPageHoldDownWidgets[p];
            pressed_widgets = 0;
            SetWidgets(_windowFinancesPageWidgets[p]);
            SetDisabledTabs();
            Invalidate();

            if (p == WINDOW_FINANCES_PAGE_RESEARCH)
            {
                width = WW_RESEARCH;
                height = WH_RESEARCH;
                flags &= ~WF_RESIZABLE;
            }
            else if (p == WINDOW_FINANCES_PAGE_SUMMARY)
            {
                width = WW_OTHER_TABS;
                height = WH_SUMMARY;
                flags &= ~WF_RESIZABLE;
            }
            else if (
                p == WINDOW_FINANCES_PAGE_VALUE_GRAPH || p == WINDOW_FINANCES_PAGE_PROFIT_GRAPH
                || p == WINDOW_FINANCES_PAGE_FINANCIAL_GRAPH)
            {
                flags |= WF_RESIZABLE;
                WindowSetResize(
                    *this, WW_OTHER_TABS, WH_OTHER_TABS, std::numeric_limits<int16_t>::max(),
                    std::numeric_limits<int16_t>::max());
            }
            else
            {
                width = WW_OTHER_TABS;
                height = WH_OTHER_TABS;
                flags &= ~WF_RESIZABLE;
            }
            OnResize();
            OnPrepareDraw();
            InitScrollWidgets();

            // Scroll summary all the way to the right, initially.
            if (p == WINDOW_FINANCES_PAGE_SUMMARY)
                InitialiseScrollPosition(WIDX_SUMMARY_SCROLL, 0);

            Invalidate();
        }

#pragma region Summary Events

        void OnMouseDownSummary(WidgetIndex widgetIndex)
        {
            auto& gameState = GetGameState();
            switch (widgetIndex)
            {
                case WIDX_LOAN_INCREASE:
                {
                    // If loan can be increased, do so.
                    // If not, action shows error message.
                    auto newLoan = gameState.BankLoan + 1000.00_GBP;
                    if (gameState.BankLoan < gameState.MaxBankLoan)
                    {
                        newLoan = std::min(gameState.MaxBankLoan, newLoan);
                    }
                    auto gameAction = ParkSetLoanAction(newLoan);
                    GameActions::Execute(&gameAction);
                    break;
                }
                case WIDX_LOAN_DECREASE:
                {
                    // If loan is positive, decrease it.
                    // If loan is negative, action shows error message.
                    // If loan is exactly 0, prevent error message.
                    if (gameState.BankLoan != 0)
                    {
                        auto newLoan = gameState.BankLoan - 1000.00_GBP;
                        if (gameState.BankLoan > 0)
                        {
                            newLoan = std::max(static_cast<money64>(0LL), newLoan);
                        }
                        auto gameAction = ParkSetLoanAction(newLoan);
                        GameActions::Execute(&gameAction);
                    }
                    break;
                }
            }
        }

        void OnPrepareDrawSummary()
        {
            // Setting loan widget's format arguments here.
            // Nothing else should use the global formatter until
            // drawing has completed.
            auto ft = Formatter::Common();
            ft.Increment(6);
            ft.Add<money64>(GetGameState().BankLoan);

            // Keep up with new months being added in the first two years.
            if (GetDate().GetMonthsElapsed() != _lastPaintedMonth)
                InitialiseScrollPosition(WIDX_SUMMARY_SCROLL, 0);
        }

        void OnDrawSummary(DrawPixelInfo& dpi)
        {
            auto screenCoords = windowPos + ScreenCoordsXY{ 8, 51 };
            auto& gameState = GetGameState();

            // Expenditure / Income heading
            DrawTextBasic(
                dpi, screenCoords, STR_FINANCES_SUMMARY_EXPENDITURE_INCOME, {},
                { COLOUR_BLACK, TextUnderline::On, TextAlignment::LEFT });
            screenCoords.y += 14;

            // Expenditure / Income row labels
            for (int32_t i = 0; i < static_cast<int32_t>(ExpenditureType::Count); i++)
            {
                // Darken every even row
                if (i % 2 == 0)
                    GfxFillRect(
                        dpi,
                        { screenCoords - ScreenCoordsXY{ 0, 1 }, screenCoords + ScreenCoordsXY{ 121, (kTableCellHeight - 2) } },
                        ColourMapA[colours[1].colour].lighter | 0x1000000);

                DrawTextBasic(dpi, screenCoords - ScreenCoordsXY{ 0, 1 }, _windowFinancesSummaryRowLabels[i]);
                screenCoords.y += kTableCellHeight;
            }

            // Horizontal rule below expenditure / income table
            GfxFillRectInset(
                dpi, { windowPos + ScreenCoordsXY{ 8, 272 }, windowPos + ScreenCoordsXY{ 8 + 513, 272 + 1 } }, colours[1],
                INSET_RECT_FLAG_BORDER_INSET);

            // Loan and interest rate
            DrawTextBasic(dpi, windowPos + ScreenCoordsXY{ 8, 279 }, STR_FINANCES_SUMMARY_LOAN);
            if (!(gameState.Park.Flags & PARK_FLAGS_RCT1_INTEREST))
            {
                auto ft = Formatter();
                ft.Add<uint16_t>(gameState.BankLoanInterestRate);
                DrawTextBasic(dpi, windowPos + ScreenCoordsXY{ 167, 279 }, STR_FINANCES_SUMMARY_AT_X_PER_YEAR, ft);
            }

            // Current cash
            auto ft = Formatter();
            ft.Add<money64>(gameState.Cash);
            StringId stringId = gameState.Cash >= 0 ? STR_CASH_LABEL : STR_CASH_NEGATIVE_LABEL;
            DrawTextBasic(dpi, windowPos + ScreenCoordsXY{ 8, 294 }, stringId, ft);

            // Objective related financial information
            if (gameState.ScenarioObjective.Type == OBJECTIVE_MONTHLY_FOOD_INCOME)
            {
                auto lastMonthProfit = FinanceGetLastMonthShopProfit();
                ft = Formatter();
                ft.Add<money64>(lastMonthProfit);
                DrawTextBasic(
                    dpi, windowPos + ScreenCoordsXY{ 280, 279 }, STR_LAST_MONTH_PROFIT_FROM_FOOD_DRINK_MERCHANDISE_SALES_LABEL,
                    ft);
            }
            else
            {
                // Park value and company value
                ft = Formatter();
                ft.Add<money64>(gameState.Park.Value);
                DrawTextBasic(dpi, windowPos + ScreenCoordsXY{ 280, 279 }, STR_PARK_VALUE_LABEL, ft);
                ft = Formatter();
                ft.Add<money64>(gameState.CompanyValue);
                DrawTextBasic(dpi, windowPos + ScreenCoordsXY{ 280, 294 }, STR_COMPANY_VALUE_LABEL, ft);
            }
        }

        uint16_t SummaryMaxAvailableMonth()
        {
            return std::min<uint16_t>(GetDate().GetMonthsElapsed(), kExpenditureTableMonthCount - 1);
        }

#pragma endregion

#pragma region Marketing Events

        void OnMouseUpMarketing(WidgetIndex widgetIndex)
        {
            if (widgetIndex >= WIDX_CAMPAIGN_1 && widgetIndex <= WIDX_CAMPAIGN_6)
            {
                ContextOpenDetailWindow(WD_NEW_CAMPAIGN, widgetIndex - WIDX_CAMPAIGN_1);
            }
        }

        void OnPrepareDrawMarketing()
        {
            // Count number of active campaigns
            int32_t numActiveCampaigns = static_cast<int32_t>(GetGameState().MarketingCampaigns.size());
            int32_t y = std::max(1, numActiveCampaigns) * kListRowHeight + 92;

            // Update group box positions
            widgets[WIDX_ACTIVE_CAMPAIGNS_GROUP].bottom = y - 22;
            widgets[WIDX_CAMPAIGNS_AVAILABLE_GROUP].top = y - 13;

            // Update new campaign button visibility
            y += 3;
            for (int32_t i = 0; i < ADVERTISING_CAMPAIGN_COUNT; i++)
            {
                auto campaignButton = &widgets[WIDX_CAMPAIGN_1 + i];
                auto marketingCampaign = MarketingGetCampaign(i);
                if (marketingCampaign == nullptr && MarketingIsCampaignTypeApplicable(i))
                {
                    campaignButton->type = WindowWidgetType::Button;
                    campaignButton->top = y;
                    campaignButton->bottom = y + kButtonFaceHeight + 1;
                    y += kButtonFaceHeight + 2;
                }
                else
                {
                    campaignButton->type = WindowWidgetType::Empty;
                }
            }
        }

        void OnDrawMarketing(DrawPixelInfo& dpi)
        {
            auto screenCoords = windowPos + ScreenCoordsXY{ 8, 62 };
            int32_t noCampaignsActive = 1;
            for (int32_t i = 0; i < ADVERTISING_CAMPAIGN_COUNT; i++)
            {
                auto marketingCampaign = MarketingGetCampaign(i);
                if (marketingCampaign == nullptr)
                    continue;

                noCampaignsActive = 0;
                auto ft = Formatter();

                // Set special parameters
                switch (i)
                {
                    case ADVERTISING_CAMPAIGN_RIDE_FREE:
                    case ADVERTISING_CAMPAIGN_RIDE:
                    {
                        auto campaignRide = GetRide(marketingCampaign->RideId);
                        if (campaignRide != nullptr)
                        {
                            campaignRide->FormatNameTo(ft);
                        }
                        else
                        {
                            ft.Add<StringId>(kStringIdNone);
                        }
                        break;
                    }
                    case ADVERTISING_CAMPAIGN_FOOD_OR_DRINK_FREE:
                        ft.Add<StringId>(GetShopItemDescriptor(marketingCampaign->ShopItemType).Naming.Plural);
                        break;
                    default:
                    {
                        auto parkName = GetGameState().Park.Name.c_str();
                        ft.Add<StringId>(STR_STRING);
                        ft.Add<const char*>(parkName);
                    }
                }
                // Advertisement
                DrawTextEllipsised(dpi, screenCoords + ScreenCoordsXY{ 4, 0 }, 296, kMarketingCampaignNames[i][1], ft);

                // Duration
                uint16_t weeksRemaining = marketingCampaign->WeeksLeft;
                ft = Formatter();
                ft.Add<uint16_t>(weeksRemaining);
                DrawTextBasic(
                    dpi, screenCoords + ScreenCoordsXY{ 304, 0 },
                    weeksRemaining == 1 ? STR_1_WEEK_REMAINING : STR_X_WEEKS_REMAINING, ft);

                screenCoords.y += kListRowHeight;
            }

            if (noCampaignsActive)
            {
                DrawTextBasic(dpi, screenCoords + ScreenCoordsXY{ 4, 0 }, STR_MARKETING_CAMPAIGNS_NONE);
                screenCoords.y += kListRowHeight;
            }
            screenCoords.y += 34;

            // Draw campaign button text
            for (int32_t i = 0; i < ADVERTISING_CAMPAIGN_COUNT; i++)
            {
                auto campaignButton = &widgets[WIDX_CAMPAIGN_1 + i];
                if (campaignButton->type != WindowWidgetType::Empty)
                {
                    // Draw button text
                    DrawTextBasic(dpi, screenCoords + ScreenCoordsXY{ 4, 0 }, kMarketingCampaignNames[i][0]);
                    auto ft = Formatter();
                    ft.Add<money64>(AdvertisingCampaignPricePerWeek[i]);
                    DrawTextBasic(dpi, screenCoords + ScreenCoordsXY{ WH_SUMMARY, 0 }, STR_MARKETING_PER_WEEK, ft);

                    screenCoords.y += kButtonFaceHeight + 2;
                }
            }
        }

#pragma endregion

#pragma region Graph Events

        void OnDrawGraph(DrawPixelInfo& dpi, const money64 currentValue, const StringId fmt) const
        {
            Formatter ft;
            ft.Add<money64>(currentValue);
            DrawTextBasic(dpi, _graphBounds.Point1 - ScreenCoordsXY{ 0, 11 }, fmt, ft);

            // Graph
            GfxFillRectInset(dpi, _graphBounds, colours[1], INSET_RECT_F_30);
            // hide resize widget on graph area
            constexpr ScreenCoordsXY offset{ 1, 1 };
            constexpr ScreenCoordsXY bigOffset{ 5, 5 };
            GfxFillRectInset(
                dpi, { _graphBounds.Point2 - bigOffset, _graphBounds.Point2 - offset }, colours[1],
                INSET_RECT_FLAG_FILL_DONT_LIGHTEN | INSET_RECT_FLAG_BORDER_NONE);

            Graph::DrawFinanceGraph(dpi, _graphProps);
        }

        void OnPrepareDrawGraph(const Widget* graphPageWidget, const bool centredGraph)
        {
            // Calculate Y axis max and min.
            money64 maxVal = 0;
            const auto series = _graphProps.series;
            for (int32_t i = 0; i < kGraphNumPoints; i++)
            {
                auto val = std::abs(series[i]);
                if (val == kMoney64Undefined)
                    continue;
                if (val > maxVal)
                    maxVal = val;
            }
            // This algorithm increments the leading digit of the max and sets all other digits to zero.
            // e.g. 681 => 700.
            money64 oom = 10;
            while (maxVal / oom >= 10)
                oom *= 10;
            const money64 max = std::max(10.00_GBP, ((maxVal + oom - 1) / oom) * oom);

            _graphProps.min = centredGraph ? -max : 0.00_GBP;
            _graphProps.max = max;

            // dynamic padding for long axis labels:
            char buffer[64]{};
            FormatStringToBuffer(buffer, sizeof(buffer), "{BLACK}{CURRENCY2DP}", centredGraph ? -max : max);
            int32_t maxWidth = GfxGetStringWidth(buffer, FontStyle::Small) + Graph::kYTickMarkPadding + 1;
            const ScreenCoordsXY dynamicPadding{ std::max(maxWidth, kGraphTopLeftPadding.x), kGraphTopLeftPadding.y };

            _graphBounds = { windowPos + ScreenCoordsXY{ graphPageWidget->left + 4, graphPageWidget->top + 15 },
                             windowPos + ScreenCoordsXY{ graphPageWidget->right - 4, graphPageWidget->bottom - 4 } };
            _graphProps.RecalculateLayout(
                { _graphBounds.Point1 + dynamicPadding, _graphBounds.Point2 - kGraphBottomRightPadding }, kGraphNumYLabels,
                kGraphNumPoints);
            _graphProps.lineCol = colours[2];
        }

#pragma endregion

        void InitialiseScrollPosition(WidgetIndex widgetIndex, int32_t scrollId)
        {
            const auto& widget = this->widgets[widgetIndex];
            scrolls[scrollId].contentOffsetX = std::max(0, scrolls[scrollId].contentWidth - (widget.width() - 2));

            WidgetScrollUpdateThumbs(*this, widgetIndex);
        }

        void DrawTabImage(DrawPixelInfo& dpi, int32_t tabPage, int32_t spriteIndex)
        {
            WidgetIndex widgetIndex = WIDX_TAB_1 + tabPage;

            if (!IsWidgetDisabled(widgetIndex))
            {
                if (this->page == tabPage)
                {
                    int32_t frame = frame_no / 2;
                    spriteIndex += (frame % _windowFinancesTabAnimationFrames[this->page]);
                }

                GfxDrawSprite(
                    dpi, ImageId(spriteIndex),
                    windowPos + ScreenCoordsXY{ widgets[widgetIndex].left, widgets[widgetIndex].top });
            }
        }

        void DrawTabImages(DrawPixelInfo& dpi)
        {
            DrawTabImage(dpi, WINDOW_FINANCES_PAGE_SUMMARY, SPR_TAB_FINANCES_SUMMARY_0);
            DrawTabImage(dpi, WINDOW_FINANCES_PAGE_FINANCIAL_GRAPH, SPR_TAB_FINANCES_FINANCIAL_GRAPH_0);
            DrawTabImage(dpi, WINDOW_FINANCES_PAGE_VALUE_GRAPH, SPR_TAB_FINANCES_VALUE_GRAPH_0);
            DrawTabImage(dpi, WINDOW_FINANCES_PAGE_PROFIT_GRAPH, SPR_TAB_FINANCES_PROFIT_GRAPH_0);
            DrawTabImage(dpi, WINDOW_FINANCES_PAGE_MARKETING, SPR_TAB_FINANCES_MARKETING_0);
            DrawTabImage(dpi, WINDOW_FINANCES_PAGE_RESEARCH, SPR_TAB_FINANCES_RESEARCH_0);
        }

        void OnResize() override
        {
            ResizeFrameWithPage();
        }
    };

    static FinancesWindow* FinancesWindowOpen(uint8_t page)
    {
        auto* windowMgr = Ui::GetWindowManager();
        auto* window = windowMgr->FocusOrCreate<FinancesWindow>(WindowClass::Finances, WW_OTHER_TABS, WH_SUMMARY, WF_10);

        if (window != nullptr && page != WINDOW_FINANCES_PAGE_SUMMARY)
            window->SetPage(page);

        return window;
    }

    WindowBase* FinancesOpen()
    {
        auto* windowMgr = Ui::GetWindowManager();
        return windowMgr->FocusOrCreate<FinancesWindow>(WindowClass::Finances, WW_OTHER_TABS, WH_SUMMARY, WF_10);
    }

    WindowBase* FinancesResearchOpen()
    {
        return FinancesWindowOpen(WINDOW_FINANCES_PAGE_RESEARCH);
    }

    WindowBase* FinancesMarketingOpen()
    {
        return FinancesWindowOpen(WINDOW_FINANCES_PAGE_MARKETING);
    }
} // namespace OpenRCT2::Ui::Windows
