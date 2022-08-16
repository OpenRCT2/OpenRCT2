/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <algorithm>
#include <openrct2-ui/interface/Dropdown.h>
#include <openrct2-ui/interface/Graph.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/Context.h>
#include <openrct2/Game.h>
#include <openrct2/GameState.h>
#include <openrct2/actions/ParkSetLoanAction.h>
#include <openrct2/actions/ParkSetResearchFundingAction.h>
#include <openrct2/config/Config.h>
#include <openrct2/localisation/Date.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/management/Finance.h>
#include <openrct2/ride/RideData.h>
#include <openrct2/ride/ShopItem.h>
#include <openrct2/scenario/Scenario.h>
#include <openrct2/sprites.h>
#include <openrct2/world/Park.h>

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

static constexpr const int32_t WH_SUMMARY = 309;
static constexpr const int32_t WH_RESEARCH = 207;
static constexpr const int32_t WH_OTHER_TABS = 257;
static constexpr const int32_t WW_RESEARCH = 320;
static constexpr const int32_t WW_OTHER_TABS = 530;
static constexpr const int32_t RSH_SUMMARY = 266;
static constexpr const int32_t RSH_RESEARCH = 164;
static constexpr const int32_t RSH_OTHER_TABS = 214;
static constexpr const int32_t RSW_RESEARCH = WW_RESEARCH;
static constexpr const int32_t RSW_OTHER_TABS = WW_OTHER_TABS;

#pragma endregion

#pragma region Widgets

// clang-format off
#define MAIN_FINANCES_WIDGETS(TITLE, RSW, RSH, WW, WH) \
    WINDOW_SHIM(TITLE, WW, WH), \
    MakeWidget({0, 43}, {RSW, RSH}, WindowWidgetType::Resize, WindowColour::Secondary), \
    MakeTab({  3, 17}, STR_FINANCES_SHOW_SUMMARY_TAB_TIP      ), \
    MakeTab({ 34, 17}, STR_FINANCES_SHOW_CASH_TAB_TIP         ), \
    MakeTab({ 65, 17}, STR_FINANCES_SHOW_PARK_VALUE_TAB_TIP   ), \
    MakeTab({ 96, 17}, STR_FINANCES_SHOW_WEEKLY_PROFIT_TAB_TIP), \
    MakeTab({127, 17}, STR_FINANCES_SHOW_MARKETING_TAB_TIP    ), \
    MakeTab({158, 17}, STR_FINANCES_RESEARCH_TIP              )

static rct_widget _windowFinancesSummaryWidgets[] =
{
    MAIN_FINANCES_WIDGETS(STR_FINANCIAL_SUMMARY, RSW_OTHER_TABS, RSH_SUMMARY, WW_OTHER_TABS, WH_SUMMARY),
    MakeWidget        ({130,  50}, {391, 211}, WindowWidgetType::Scroll,  WindowColour::Secondary, SCROLL_HORIZONTAL              ),
    MakeSpinnerWidgets({ 64, 279}, { 97,  14}, WindowWidgetType::Spinner, WindowColour::Secondary, STR_FINANCES_SUMMARY_LOAN_VALUE), // NB: 3 widgets.
    WIDGETS_END,
};

static rct_widget _windowFinancesCashWidgets[] =
{
    MAIN_FINANCES_WIDGETS(STR_FINANCIAL_GRAPH, RSW_OTHER_TABS, RSH_OTHER_TABS, WW_OTHER_TABS, WH_OTHER_TABS),
    WIDGETS_END,
};

static rct_widget _windowFinancesParkValueWidgets[] =
{
    MAIN_FINANCES_WIDGETS(STR_PARK_VALUE_GRAPH, RSW_OTHER_TABS, RSH_OTHER_TABS, WW_OTHER_TABS, WH_OTHER_TABS),
    WIDGETS_END,
};

static rct_widget _windowFinancesProfitWidgets[] =
{
    MAIN_FINANCES_WIDGETS(STR_PROFIT_GRAPH, RSW_OTHER_TABS, RSH_OTHER_TABS, WW_OTHER_TABS, WH_OTHER_TABS),
    WIDGETS_END,
};

static rct_widget _windowFinancesMarketingWidgets[] =
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
    WIDGETS_END,
};

static rct_widget _windowFinancesResearchWidgets[] =
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
    WIDGETS_END,
};
// clang-format on

static rct_widget* _windowFinancesPageWidgets[] = {
    _windowFinancesSummaryWidgets,   // WINDOW_FINANCES_PAGE_SUMMARY
    _windowFinancesCashWidgets,      // WINDOW_FINANCES_PAGE_FINANCIAL_GRAPH
    _windowFinancesParkValueWidgets, // WINDOW_FINANCES_PAGE_VALUE_GRAPH
    _windowFinancesProfitWidgets,    // WINDOW_FINANCES_PAGE_PROFIT_GRAPH
    _windowFinancesMarketingWidgets, // WINDOW_FINANCES_PAGE_MARKETING
    _windowFinancesResearchWidgets,  // WINDOW_FINANCES_PAGE_RESEARCH
};
static_assert(std::size(_windowFinancesPageWidgets) == WINDOW_FINANCES_PAGE_COUNT);

#pragma endregion

static constexpr const StringId _windowFinancesSummaryRowLabels[static_cast<int32_t>(ExpenditureType::Count)] = {
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

static constexpr const int32_t _windowFinancesTabAnimationFrames[] = {
    8,  // WINDOW_FINANCES_PAGE_SUMMARY
    16, // WINDOW_FINANCES_PAGE_FINANCIAL_GRAPH
    16, // WINDOW_FINANCES_PAGE_VALUE_GRAPH
    16, // WINDOW_FINANCES_PAGE_PROFIT_GRAPH
    19, // WINDOW_FINANCES_PAGE_MARKETING
    8,  // WINDOW_FINANCES_PAGE_RESEARCH
};
static_assert(std::size(_windowFinancesTabAnimationFrames) == WINDOW_FINANCES_PAGE_COUNT);

static constexpr const int32_t EXPENDITURE_COLUMN_WIDTH = 80;

static constexpr const uint32_t _windowFinancesPageHoldDownWidgets[] = {
    (1ULL << WIDX_LOAN_INCREASE) | (1ULL << WIDX_LOAN_DECREASE), // WINDOW_FINANCES_PAGE_SUMMARY

    0, // WINDOW_FINANCES_PAGE_FINANCIAL_GRAPH
    0, // WINDOW_FINANCES_PAGE_VALUE_GRAPH
    0, // WINDOW_FINANCES_PAGE_PROFIT_GRAPH
    0, // WINDOW_FINANCES_PAGE_MARKETING
    0, // WINDOW_FINANCES_PAGE_RESEARCH
};
static_assert(std::size(_windowFinancesPageHoldDownWidgets) == WINDOW_FINANCES_PAGE_COUNT);

#pragma endregion

class FinancesWindow final : public Window
{
private:
    int32_t _lastPaintedMonth;

public:
    void OnOpen() override
    {
        SetPage(WINDOW_FINANCES_PAGE_SUMMARY);
        _lastPaintedMonth = std::numeric_limits<int32_t>::max();
        ResearchUpdateUncompletedTypes();
    }

    void OnUpdate() override
    {
        frame_no++;
        InvalidateWidget(WIDX_TAB_1 + page);
    }

    void OnMouseDown(rct_widgetindex widgetIndex) override
    {
        switch (page)
        {
            case WINDOW_FINANCES_PAGE_SUMMARY:
                OnMouseDownSummary(widgetIndex);
                break;
            case WINDOW_FINANCES_PAGE_RESEARCH:
                OnMouseDownResearch(widgetIndex);
                break;
        }
    }

    void OnMouseUp(rct_widgetindex widgetIndex) override
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
                        OnMouseUpResearch(widgetIndex);
                }
                break;
        }
    }

    void OnDropdown(rct_widgetindex widgetIndex, int32_t selectedIndex) override
    {
        if (page == WINDOW_FINANCES_PAGE_RESEARCH)
        {
            OnDropdownResearch(widgetIndex, selectedIndex);
        }
    }

    void OnPrepareDraw() override
    {
        auto* targetWidgets = _windowFinancesPageWidgets[page];

        if (widgets != targetWidgets)
        {
            widgets = targetWidgets;
            WindowInitScrollWidgets(*this);
        }

        for (auto i = 0; i < WINDOW_FINANCES_PAGE_COUNT; i++)
            SetWidgetPressed(WIDX_TAB_1 + i, false);
        SetWidgetPressed(WIDX_TAB_1 + page, true);
        switch (page)
        {
            case WINDOW_FINANCES_PAGE_SUMMARY:
                OnPrepareDrawSummary();
                break;
            case WINDOW_FINANCES_PAGE_MARKETING:
                OnPrepareDrawMarketing();
                break;
            case WINDOW_FINANCES_PAGE_RESEARCH:
                OnPrepareDrawResearch();
                break;
        }
    }

    void OnDraw(rct_drawpixelinfo& dpi) override
    {
        DrawWidgets(dpi);
        DrawTabImages(dpi);

        switch (page)
        {
            case WINDOW_FINANCES_PAGE_SUMMARY:
                OnDrawSummary(dpi);
                break;
            case WINDOW_FINANCES_PAGE_FINANCIAL_GRAPH:
                OnDrawFinancialGraph(dpi);
                break;
            case WINDOW_FINANCES_PAGE_VALUE_GRAPH:
                OnDrawParkValueGraph(dpi);
                break;
            case WINDOW_FINANCES_PAGE_PROFIT_GRAPH:
                OnDrawProfitGraph(dpi);
                break;
            case WINDOW_FINANCES_PAGE_MARKETING:
                OnDrawMarketing(dpi);
                break;
            case WINDOW_FINANCES_PAGE_RESEARCH:
                OnDrawResearch(dpi);
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

    void OnScrollDraw(int32_t scrollIndex, rct_drawpixelinfo& dpi) override
    {
        if (page != WINDOW_FINANCES_PAGE_SUMMARY)
            return;

        auto screenCoords = ScreenCoordsXY{ 0, TABLE_CELL_HEIGHT + 2 };

        rct_widget self = widgets[WIDX_SUMMARY_SCROLL];
        int32_t row_width = std::max<uint16_t>(scrolls[0].h_right, self.width());

        // Expenditure / Income row labels
        for (int32_t i = 0; i < static_cast<int32_t>(ExpenditureType::Count); i++)
        {
            // Darken every even row
            if (i % 2 == 0)
                gfx_fill_rect(
                    &dpi,
                    { screenCoords - ScreenCoordsXY{ 0, 1 },
                      screenCoords + ScreenCoordsXY{ row_width, (TABLE_CELL_HEIGHT - 2) } },
                    ColourMapA[colours[1]].lighter | 0x1000000);

            screenCoords.y += TABLE_CELL_HEIGHT;
        }

        // Expenditure / Income values for each month
        uint16_t currentMonthYear = static_cast<uint16_t>(gDateMonthsElapsed);
        for (int32_t i = SummaryMaxAvailableMonth(); i >= 0; i--)
        {
            screenCoords.y = 0;

            uint16_t monthyear = currentMonthYear - i;

            // Month heading
            auto ft = Formatter();
            ft.Add<StringId>(STR_FINANCES_SUMMARY_MONTH_HEADING);
            ft.Add<uint16_t>(monthyear);
            DrawTextBasic(
                &dpi, screenCoords + ScreenCoordsXY{ EXPENDITURE_COLUMN_WIDTH, 0 },
                monthyear == currentMonthYear ? STR_WINDOW_COLOUR_2_STRINGID : STR_BLACK_STRING, ft,
                { TextUnderline::On, TextAlignment::RIGHT });
            screenCoords.y += 14;

            // Month expenditures
            money64 profit = 0;
            for (int32_t j = 0; j < static_cast<int32_t>(ExpenditureType::Count); j++)
            {
                auto expenditure = gExpenditureTable[i][j];
                if (expenditure != 0)
                {
                    profit += expenditure;
                    const StringId format = expenditure >= 0 ? STR_FINANCES_SUMMARY_INCOME_VALUE
                                                             : STR_FINANCES_SUMMARY_EXPENDITURE_VALUE;
                    ft = Formatter();
                    ft.Add<money64>(expenditure);
                    DrawTextBasic(
                        &dpi, screenCoords + ScreenCoordsXY{ EXPENDITURE_COLUMN_WIDTH, 0 }, format, ft,
                        { TextAlignment::RIGHT });
                }
                screenCoords.y += TABLE_CELL_HEIGHT;
            }
            screenCoords.y += 4;

            // Month profit
            const StringId format = profit >= 0 ? STR_FINANCES_SUMMARY_INCOME_VALUE : STR_FINANCES_SUMMARY_LOSS_VALUE;
            ft = Formatter();
            ft.Add<money64>(profit);
            DrawTextBasic(
                &dpi, screenCoords + ScreenCoordsXY{ EXPENDITURE_COLUMN_WIDTH, 0 }, format, ft, { TextAlignment::RIGHT });

            gfx_fill_rect(
                &dpi,
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
        widgets = _windowFinancesPageWidgets[p];

        Invalidate();
        if (p == WINDOW_FINANCES_PAGE_RESEARCH)
        {
            width = WW_RESEARCH;
            height = WH_RESEARCH;
        }
        else if (p == WINDOW_FINANCES_PAGE_SUMMARY)
        {
            width = WW_OTHER_TABS;
            height = WH_SUMMARY;
        }
        else
        {
            width = WW_OTHER_TABS;
            height = WH_OTHER_TABS;
        }
        window_event_resize_call(this);
        window_event_invalidate_call(this);

        WindowInitScrollWidgets(*this);

        // Scroll summary all the way to the right, initially.
        if (p == WINDOW_FINANCES_PAGE_SUMMARY)
            InitialiseScrollPosition(WIDX_SUMMARY_SCROLL, 0);

        Invalidate();
    }

#pragma region Summary Events

    void OnMouseDownSummary(rct_widgetindex widgetIndex)
    {
        switch (widgetIndex)
        {
            case WIDX_LOAN_INCREASE:
            {
                auto newLoan = gBankLoan + 1000.00_GBP;
                auto gameAction = ParkSetLoanAction(newLoan);
                GameActions::Execute(&gameAction);
                break;
            }
            case WIDX_LOAN_DECREASE:
            {
                if (gBankLoan > 0)
                {
                    auto newLoan = gBankLoan - 1000.00_GBP;
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
        ft.Add<money64>(gBankLoan);

        // Keep up with new months being added in the first two years.
        if (gDateMonthsElapsed != _lastPaintedMonth)
            InitialiseScrollPosition(WIDX_SUMMARY_SCROLL, 0);
    }

    void OnDrawSummary(rct_drawpixelinfo& dpi)
    {
        auto screenCoords = windowPos + ScreenCoordsXY{ 8, 51 };

        // Expenditure / Income heading
        DrawTextBasic(
            &dpi, screenCoords, STR_FINANCES_SUMMARY_EXPENDITURE_INCOME, {},
            { COLOUR_BLACK, TextUnderline::On, TextAlignment::LEFT });
        screenCoords.y += 14;

        // Expenditure / Income row labels
        for (int32_t i = 0; i < static_cast<int32_t>(ExpenditureType::Count); i++)
        {
            // Darken every even row
            if (i % 2 == 0)
                gfx_fill_rect(
                    &dpi,
                    { screenCoords - ScreenCoordsXY{ 0, 1 }, screenCoords + ScreenCoordsXY{ 121, (TABLE_CELL_HEIGHT - 2) } },
                    ColourMapA[colours[1]].lighter | 0x1000000);

            DrawTextBasic(&dpi, screenCoords - ScreenCoordsXY{ 0, 1 }, _windowFinancesSummaryRowLabels[i]);
            screenCoords.y += TABLE_CELL_HEIGHT;
        }

        // Horizontal rule below expenditure / income table
        gfx_fill_rect_inset(
            &dpi, { windowPos + ScreenCoordsXY{ 8, 272 }, windowPos + ScreenCoordsXY{ 8 + 513, 272 + 1 } }, colours[1],
            INSET_RECT_FLAG_BORDER_INSET);

        // Loan and interest rate
        DrawTextBasic(&dpi, windowPos + ScreenCoordsXY{ 8, 279 }, STR_FINANCES_SUMMARY_LOAN);
        auto ft = Formatter();
        ft.Add<uint16_t>(gBankLoanInterestRate);
        DrawTextBasic(&dpi, windowPos + ScreenCoordsXY{ 167, 279 }, STR_FINANCES_SUMMARY_AT_X_PER_YEAR, ft);

        // Current cash
        ft = Formatter();
        ft.Add<money64>(gCash);
        StringId stringId = gCash >= 0 ? STR_CASH_LABEL : STR_CASH_NEGATIVE_LABEL;
        DrawTextBasic(&dpi, windowPos + ScreenCoordsXY{ 8, 294 }, stringId, ft);

        // Objective related financial information
        if (gScenarioObjective.Type == OBJECTIVE_MONTHLY_FOOD_INCOME)
        {
            auto lastMonthProfit = finance_get_last_month_shop_profit();
            ft = Formatter();
            ft.Add<money64>(lastMonthProfit);
            DrawTextBasic(
                &dpi, windowPos + ScreenCoordsXY{ 280, 279 }, STR_LAST_MONTH_PROFIT_FROM_FOOD_DRINK_MERCHANDISE_SALES_LABEL,
                ft);
        }
        else
        {
            // Park value and company value
            ft = Formatter();
            ft.Add<money64>(gParkValue);
            DrawTextBasic(&dpi, windowPos + ScreenCoordsXY{ 280, 279 }, STR_PARK_VALUE_LABEL, ft);
            ft = Formatter();
            ft.Add<money64>(gCompanyValue);
            DrawTextBasic(&dpi, windowPos + ScreenCoordsXY{ 280, 294 }, STR_COMPANY_VALUE_LABEL, ft);
        }
    }

    uint16_t SummaryMaxAvailableMonth()
    {
        return std::min<uint16_t>(gDateMonthsElapsed, EXPENDITURE_TABLE_MONTH_COUNT - 1);
    }

#pragma endregion

#pragma region Financial Graph Events

    void OnDrawFinancialGraph(rct_drawpixelinfo& dpi)
    {
        rct_widget* pageWidget = &_windowFinancesCashWidgets[WIDX_PAGE_BACKGROUND];
        auto graphTopLeft = windowPos + ScreenCoordsXY{ pageWidget->left + 4, pageWidget->top + 15 };
        auto graphBottomRight = windowPos + ScreenCoordsXY{ pageWidget->right - 4, pageWidget->bottom - 4 };

        // Cash (less loan)
        auto cashLessLoan = gCash - gBankLoan;
        auto ft = Formatter();
        ft.Add<money64>(cashLessLoan);

        DrawTextBasic(
            &dpi, graphTopLeft - ScreenCoordsXY{ 0, 11 },
            cashLessLoan >= 0 ? STR_FINANCES_FINANCIAL_GRAPH_CASH_LESS_LOAN_POSITIVE
                              : STR_FINANCES_FINANCIAL_GRAPH_CASH_LESS_LOAN_NEGATIVE,
            ft);

        // Graph
        gfx_fill_rect_inset(&dpi, { graphTopLeft, graphBottomRight }, colours[1], INSET_RECT_F_30);

        // Calculate the Y axis scale (log2 of highest [+/-]balance)
        int32_t yAxisScale = 0;
        for (int32_t i = 0; i < 64; i++)
        {
            auto balance = gCashHistory[i];
            if (balance == MONEY64_UNDEFINED)
                continue;

            // Modifier balance then keep halving until less than 127 pixels
            balance = std::abs(balance) >> yAxisScale;
            while (balance > 127)
            {
                balance /= 2;
                yAxisScale++;
            }
        }

        // Y axis labels
        auto coords = graphTopLeft + ScreenCoordsXY{ 18, 14 };
        money64 axisBase;
        for (axisBase = 12.00_GBP; axisBase >= -12.00_GBP; axisBase -= 6.00_GBP)
        {
            auto axisValue = axisBase << yAxisScale;
            ft = Formatter();
            ft.Add<money64>(axisValue);
            DrawTextBasic(
                &dpi, coords + ScreenCoordsXY{ 70, 0 }, STR_FINANCES_FINANCIAL_GRAPH_CASH_VALUE, ft,
                { FontSpriteBase::SMALL, TextAlignment::RIGHT });
            gfx_fill_rect_inset(
                &dpi, { coords + ScreenCoordsXY{ 70, 5 }, { graphTopLeft.x + 482, coords.y + 5 } }, colours[2],
                INSET_RECT_FLAG_BORDER_INSET);
            coords.y += 39;
        }

        // X axis labels and values
        coords = graphTopLeft + ScreenCoordsXY{ 98, 17 };
        Graph::Draw(&dpi, gCashHistory, 64, coords, yAxisScale, 128);
    }

#pragma endregion

#pragma region Park Value Graph Events

    void OnDrawParkValueGraph(rct_drawpixelinfo& dpi)
    {
        rct_widget* pageWidget = &_windowFinancesCashWidgets[WIDX_PAGE_BACKGROUND];
        auto graphTopLeft = windowPos + ScreenCoordsXY{ pageWidget->left + 4, pageWidget->top + 15 };
        auto graphBottomRight = windowPos + ScreenCoordsXY{ pageWidget->right - 4, pageWidget->bottom - 4 };

        // Park value
        auto ft = Formatter();
        ft.Add<money64>(gParkValue);
        DrawTextBasic(&dpi, graphTopLeft - ScreenCoordsXY{ 0, 11 }, STR_FINANCES_PARK_VALUE, ft);

        // Graph
        gfx_fill_rect_inset(&dpi, { graphTopLeft, graphBottomRight }, colours[1], INSET_RECT_F_30);

        // Calculate the Y axis scale (log2 of highest [+/-]balance)
        int32_t yAxisScale = 0;
        for (int32_t i = 0; i < 64; i++)
        {
            auto balance = gParkValueHistory[i];
            if (balance == MONEY64_UNDEFINED)
                continue;

            // Modifier balance then keep halving until less than 255 pixels
            balance = std::abs(balance) >> yAxisScale;
            while (balance > 255)
            {
                balance /= 2;
                yAxisScale++;
            }
        }

        // Y axis labels
        auto coords = graphTopLeft + ScreenCoordsXY{ 18, 14 };
        money64 axisBase;
        for (axisBase = 24.00_GBP; axisBase >= 0.00_GBP; axisBase -= 6.00_GBP)
        {
            auto axisValue = axisBase << yAxisScale;
            ft = Formatter();
            ft.Add<money64>(axisValue);
            DrawTextBasic(
                &dpi, coords + ScreenCoordsXY{ 70, 0 }, STR_FINANCES_FINANCIAL_GRAPH_CASH_VALUE, ft,
                { FontSpriteBase::SMALL, TextAlignment::RIGHT });
            gfx_fill_rect_inset(
                &dpi, { coords + ScreenCoordsXY{ 70, 5 }, { graphTopLeft.x + 482, coords.y + 5 } }, colours[2],
                INSET_RECT_FLAG_BORDER_INSET);
            coords.y += 39;
        }

        // X axis labels and values
        coords = graphTopLeft + ScreenCoordsXY{ 98, 17 };
        Graph::Draw(&dpi, gParkValueHistory, 64, coords, yAxisScale, 0);
    }

#pragma endregion

#pragma region Profit Graph Events

    void OnDrawProfitGraph(rct_drawpixelinfo& dpi)
    {
        rct_widget* pageWidget = &_windowFinancesCashWidgets[WIDX_PAGE_BACKGROUND];
        auto graphTopLeft = windowPos + ScreenCoordsXY{ pageWidget->left + 4, pageWidget->top + 15 };
        auto graphBottomRight = windowPos + ScreenCoordsXY{ pageWidget->right - 4, pageWidget->bottom - 4 };

        // Weekly profit
        auto ft = Formatter();
        ft.Add<money64>(gCurrentProfit);
        DrawTextBasic(
            &dpi, graphTopLeft - ScreenCoordsXY{ 0, 11 },
            gCurrentProfit >= 0 ? STR_FINANCES_WEEKLY_PROFIT_POSITIVE : STR_FINANCES_WEEKLY_PROFIT_LOSS, ft);

        // Graph
        gfx_fill_rect_inset(&dpi, { graphTopLeft, graphBottomRight }, colours[1], INSET_RECT_F_30);

        // Calculate the Y axis scale (log2 of highest [+/-]balance)
        int32_t yAxisScale = 0;
        for (int32_t i = 0; i < 64; i++)
        {
            auto balance = gWeeklyProfitHistory[i];
            if (balance == MONEY64_UNDEFINED)
                continue;

            // Modifier balance then keep halving until less than 127 pixels
            balance = std::abs(balance) >> yAxisScale;
            while (balance > 127)
            {
                balance /= 2;
                yAxisScale++;
            }
        }

        // Y axis labels
        auto screenPos = graphTopLeft + ScreenCoordsXY{ 18, 14 };
        money64 axisBase;
        for (axisBase = 12.00_GBP; axisBase >= -12.00_GBP; axisBase -= 6.00_GBP)
        {
            money64 axisValue = axisBase << yAxisScale;
            ft = Formatter();
            ft.Add<money64>(axisValue);
            DrawTextBasic(
                &dpi, screenPos + ScreenCoordsXY{ 70, 0 }, STR_FINANCES_FINANCIAL_GRAPH_CASH_VALUE, ft,
                { FontSpriteBase::SMALL, TextAlignment::RIGHT });
            gfx_fill_rect_inset(
                &dpi, { screenPos + ScreenCoordsXY{ 70, 5 }, { graphTopLeft.x + 482, screenPos.y + 5 } }, colours[2],
                INSET_RECT_FLAG_BORDER_INSET);
            screenPos.y += 39;
        }

        // X axis labels and values
        screenPos = graphTopLeft + ScreenCoordsXY{ 98, 17 };
        Graph::Draw(&dpi, gWeeklyProfitHistory, 64, screenPos, yAxisScale, 128);
    }

#pragma endregion

#pragma region Marketing Events

    void OnMouseUpMarketing(rct_widgetindex widgetIndex)
    {
        if (widgetIndex >= WIDX_CAMPAIGN_1 && widgetIndex <= WIDX_CAMPAIGN_6)
        {
            context_open_detail_window(WD_NEW_CAMPAIGN, widgetIndex - WIDX_CAMPAIGN_1);
        }
    }

    void OnPrepareDrawMarketing()
    {
        // Count number of active campaigns
        int32_t numActiveCampaigns = static_cast<int32_t>(gMarketingCampaigns.size());
        int32_t y = std::max(1, numActiveCampaigns) * LIST_ROW_HEIGHT + 92;

        // Update group box positions
        _windowFinancesMarketingWidgets[WIDX_ACTIVE_CAMPAIGNS_GROUP].bottom = y - 22;
        _windowFinancesMarketingWidgets[WIDX_CAMPAIGNS_AVAILABLE_GROUP].top = y - 13;

        // Update new campaign button visibility
        y += 3;
        for (int32_t i = 0; i < ADVERTISING_CAMPAIGN_COUNT; i++)
        {
            auto campaignButton = &_windowFinancesMarketingWidgets[WIDX_CAMPAIGN_1 + i];
            auto marketingCampaign = marketing_get_campaign(i);
            if (marketingCampaign == nullptr && marketing_is_campaign_type_applicable(i))
            {
                campaignButton->type = WindowWidgetType::Button;
                campaignButton->top = y;
                campaignButton->bottom = y + BUTTON_FACE_HEIGHT + 1;
                y += BUTTON_FACE_HEIGHT + 2;
            }
            else
            {
                campaignButton->type = WindowWidgetType::Empty;
            }
        }
    }

    void OnDrawMarketing(rct_drawpixelinfo& dpi)
    {
        auto screenCoords = windowPos + ScreenCoordsXY{ 8, 62 };
        int32_t noCampaignsActive = 1;
        for (int32_t i = 0; i < ADVERTISING_CAMPAIGN_COUNT; i++)
        {
            auto marketingCampaign = marketing_get_campaign(i);
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
                    auto campaignRide = get_ride(marketingCampaign->RideId);
                    if (campaignRide != nullptr)
                    {
                        campaignRide->FormatNameTo(ft);
                    }
                    else
                    {
                        ft.Add<StringId>(STR_NONE);
                    }
                    break;
                }
                case ADVERTISING_CAMPAIGN_FOOD_OR_DRINK_FREE:
                    ft.Add<StringId>(GetShopItemDescriptor(marketingCampaign->ShopItemType).Naming.Plural);
                    break;
                default:
                {
                    auto& park = OpenRCT2::GetContext()->GetGameState()->GetPark();
                    auto parkName = park.Name.c_str();
                    ft.Add<StringId>(STR_STRING);
                    ft.Add<const char*>(parkName);
                }
            }
            // Advertisement
            DrawTextEllipsised(&dpi, screenCoords + ScreenCoordsXY{ 4, 0 }, 296, MarketingCampaignNames[i][1], ft);

            // Duration
            uint16_t weeksRemaining = marketingCampaign->WeeksLeft;
            ft = Formatter();
            ft.Add<uint16_t>(weeksRemaining);
            DrawTextBasic(
                &dpi, screenCoords + ScreenCoordsXY{ 304, 0 },
                weeksRemaining == 1 ? STR_1_WEEK_REMAINING : STR_X_WEEKS_REMAINING, ft);

            screenCoords.y += LIST_ROW_HEIGHT;
        }

        if (noCampaignsActive)
        {
            DrawTextBasic(&dpi, screenCoords + ScreenCoordsXY{ 4, 0 }, STR_MARKETING_CAMPAIGNS_NONE);
            screenCoords.y += LIST_ROW_HEIGHT;
        }
        screenCoords.y += 34;

        // Draw campaign button text
        for (int32_t i = 0; i < ADVERTISING_CAMPAIGN_COUNT; i++)
        {
            auto campaignButton = &_windowFinancesMarketingWidgets[WIDX_CAMPAIGN_1 + i];
            if (campaignButton->type != WindowWidgetType::Empty)
            {
                // Draw button text
                DrawTextBasic(&dpi, screenCoords + ScreenCoordsXY{ 4, 0 }, MarketingCampaignNames[i][0]);
                auto ft = Formatter();
                ft.Add<money64>(AdvertisingCampaignPricePerWeek[i]);
                DrawTextBasic(&dpi, screenCoords + ScreenCoordsXY{ WH_SUMMARY, 0 }, STR_MARKETING_PER_WEEK, ft);

                screenCoords.y += BUTTON_FACE_HEIGHT + 2;
            }
        }
    }

#pragma endregion

#pragma region Research Events

    void OnMouseUpResearch(rct_widgetindex widgetIndex)
    {
        if (widgetIndex >= WIDX_TRANSPORT_RIDES && widgetIndex <= WIDX_SCENERY_AND_THEMING)
        {
            auto activeResearchTypes = gResearchPriorities;
            activeResearchTypes ^= 1ULL << (widgetIndex - WIDX_TRANSPORT_RIDES);

            auto gameAction = ParkSetResearchFundingAction(activeResearchTypes, gResearchFundingLevel);
            GameActions::Execute(&gameAction);
        }
    }

    void OnMouseDownResearch(rct_widgetindex widgetIndex)
    {
        if (widgetIndex != WIDX_RESEARCH_FUNDING_DROPDOWN_BUTTON)
            return;

        rct_widget* dropdownWidget = &widgets[widgetIndex - 1];

        for (int32_t i = 0; i < 4; i++)
        {
            gDropdownItems[i].Format = STR_DROPDOWN_MENU_LABEL;
            gDropdownItems[i].Args = ResearchFundingLevelNames[i];
        }

        WindowDropdownShowTextCustomWidth(
            { windowPos.x + dropdownWidget->left, windowPos.y + dropdownWidget->top }, dropdownWidget->height() + 1, colours[1],
            0, Dropdown::Flag::StayOpen, 4, dropdownWidget->width() - 3);

        int32_t currentResearchLevel = gResearchFundingLevel;
        Dropdown::SetChecked(currentResearchLevel, true);
    }

    void OnDropdownResearch(rct_widgetindex widgetIndex, int32_t selectedIndex)
    {
        if (widgetIndex != WIDX_RESEARCH_FUNDING_DROPDOWN_BUTTON || selectedIndex == -1)
            return;

        auto gameAction = ParkSetResearchFundingAction(gResearchPriorities, selectedIndex);
        GameActions::Execute(&gameAction);
    }

    void OnPrepareDrawResearch()
    {
        if (gResearchProgressStage == RESEARCH_STAGE_FINISHED_ALL)
        {
            _windowFinancesResearchWidgets[WIDX_RESEARCH_FUNDING].type = WindowWidgetType::Empty;
            _windowFinancesResearchWidgets[WIDX_RESEARCH_FUNDING_DROPDOWN_BUTTON].type = WindowWidgetType::Empty;
        }
        else
        {
            _windowFinancesResearchWidgets[WIDX_RESEARCH_FUNDING].type = WindowWidgetType::DropdownMenu;
            _windowFinancesResearchWidgets[WIDX_RESEARCH_FUNDING_DROPDOWN_BUTTON].type = WindowWidgetType::Button;
        }
        int32_t currentResearchLevel = gResearchFundingLevel;

        // Current funding
        _windowFinancesResearchWidgets[WIDX_RESEARCH_FUNDING].text = ResearchFundingLevelNames[currentResearchLevel];

        // Checkboxes
        uint8_t activeResearchTypes = gResearchPriorities;
        int32_t uncompletedResearchTypes = gResearchUncompletedCategories;
        for (int32_t i = 0; i < 7; i++)
        {
            int32_t mask = 1 << i;
            int32_t widgetMask = 1ULL << (i + WIDX_TRANSPORT_RIDES);

            // Set checkbox disabled if research type is complete
            if (uncompletedResearchTypes & mask)
            {
                disabled_widgets &= ~widgetMask;

                // Set checkbox ticked if research type is active
                if (activeResearchTypes & mask)
                    pressed_widgets |= widgetMask;
                else
                    pressed_widgets &= ~widgetMask;
            }
            else
            {
                disabled_widgets |= widgetMask;
                pressed_widgets &= ~widgetMask;
            }
        }
    }

    void OnDrawResearch(rct_drawpixelinfo& dpi)
    {
        WindowResearchFundingPagePaint(this, &dpi, WIDX_RESEARCH_FUNDING);
    }

#pragma endregion

    void InitialiseScrollPosition(rct_widgetindex widgetIndex, int32_t scrollId)
    {
        const auto& widget = this->widgets[widgetIndex];
        scrolls[scrollId].h_left = std::max(0, scrolls[scrollId].h_right - (widget.width() - 2));

        WidgetScrollUpdateThumbs(*this, widgetIndex);
    }

    void DrawTabImage(rct_drawpixelinfo& dpi, int32_t tabPage, int32_t spriteIndex)
    {
        rct_widgetindex widgetIndex = WIDX_TAB_1 + tabPage;

        if (!IsWidgetDisabled(widgetIndex))
        {
            if (this->page == tabPage)
            {
                int32_t frame = frame_no / 2;
                spriteIndex += (frame % _windowFinancesTabAnimationFrames[this->page]);
            }

            gfx_draw_sprite(
                &dpi, ImageId(spriteIndex), windowPos + ScreenCoordsXY{ widgets[widgetIndex].left, widgets[widgetIndex].top });
        }
    }

    void DrawTabImages(rct_drawpixelinfo& dpi)
    {
        DrawTabImage(dpi, WINDOW_FINANCES_PAGE_SUMMARY, SPR_TAB_FINANCES_SUMMARY_0);
        DrawTabImage(dpi, WINDOW_FINANCES_PAGE_FINANCIAL_GRAPH, SPR_TAB_FINANCES_FINANCIAL_GRAPH_0);
        DrawTabImage(dpi, WINDOW_FINANCES_PAGE_VALUE_GRAPH, SPR_TAB_FINANCES_VALUE_GRAPH_0);
        DrawTabImage(dpi, WINDOW_FINANCES_PAGE_PROFIT_GRAPH, SPR_TAB_FINANCES_PROFIT_GRAPH_0);
        DrawTabImage(dpi, WINDOW_FINANCES_PAGE_MARKETING, SPR_TAB_FINANCES_MARKETING_0);
        DrawTabImage(dpi, WINDOW_FINANCES_PAGE_RESEARCH, SPR_TAB_FINANCES_RESEARCH_0);
    }
};

rct_window* WindowFinancesOpen()
{
    return WindowFocusOrCreate<FinancesWindow>(WindowClass::Finances, WW_OTHER_TABS, WH_SUMMARY, WF_10);
}

rct_window* WindowFinancesResearchOpen()
{
    auto* window = WindowFocusOrCreate<FinancesWindow>(WindowClass::Finances, WW_OTHER_TABS, WH_SUMMARY, WF_10);

    if (window != nullptr)
        window->SetPage(WINDOW_FINANCES_PAGE_RESEARCH);

    return window;
}
