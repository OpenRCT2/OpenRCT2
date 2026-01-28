/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
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
#include <openrct2/SpriteIds.h>
#include <openrct2/actions/ParkSetLoanAction.h>
#include <openrct2/drawing/ColourMap.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/drawing/Rectangle.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/localisation/Formatting.h>
#include <openrct2/localisation/Localisation.Date.h>
#include <openrct2/management/Finance.h>
#include <openrct2/ride/RideData.h>
#include <openrct2/ride/ShopItem.h>
#include <openrct2/ui/WindowManager.h>
#include <openrct2/world/Park.h>

using namespace OpenRCT2::Drawing;

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

    static constexpr ScreenSize kWindowSizeResearch = { 320, 207 };
    static constexpr ScreenSize kTabContentSizeResearch = kWindowSizeResearch - ScreenSize(0, kTabBarHeight);

    static constexpr ScreenSize kWindowSizeSummary = { 530, 309 };
    static constexpr ScreenSize kTabContentSizeSummary = kWindowSizeSummary - ScreenSize(0, kTabBarHeight);

    static constexpr ScreenSize kWindowSizeGraphsMarketing = { 530, 257 };
    static constexpr ScreenSize kTabContentSizeGraphsMarketing = kWindowSizeGraphsMarketing - ScreenSize(0, kTabBarHeight);

    static constexpr int32_t kCostPerWeekOffset = 321;

#pragma endregion

    // clang-format off
#pragma region Widgets

    static constexpr auto makeFinancesWidgets = [](StringId title, ScreenSize resizeSize, ScreenSize frameSize) {
        return makeWidgets(
            makeWindowShim(title, frameSize),
            makeWidget({   0, 43 }, resizeSize, WidgetType::resize, WindowColour::secondary),
            makeTab   ({   3, 17 }, STR_FINANCES_SHOW_SUMMARY_TAB_TIP),
            makeTab   ({  34, 17 }, STR_FINANCES_SHOW_CASH_TAB_TIP),
            makeTab   ({  65, 17 }, STR_FINANCES_SHOW_PARK_VALUE_TAB_TIP),
            makeTab   ({  96, 17 }, STR_FINANCES_SHOW_WEEKLY_PROFIT_TAB_TIP),
            makeTab   ({ 127, 17 }, STR_FINANCES_SHOW_MARKETING_TAB_TIP),
            makeTab   ({ 158, 17 }, STR_FINANCES_RESEARCH_TIP)
        );
    };

    static constexpr auto _windowFinancesSummaryWidgets = makeWidgets(
        makeFinancesWidgets(STR_FINANCIAL_SUMMARY, kTabContentSizeSummary, kWindowSizeSummary),
        makeWidget        ({130,  50}, {391, 211}, WidgetType::scroll,  WindowColour::secondary, SCROLL_HORIZONTAL              ),
        makeSpinnerWidgets({ 64, 279}, { 97,  14}, WidgetType::spinner, WindowColour::secondary, STR_FINANCES_SUMMARY_LOAN_VALUE) // NB: 3 widgets
    );

    static constexpr auto _windowFinancesCashWidgets = makeWidgets(
        makeFinancesWidgets(STR_FINANCIAL_GRAPH, kTabContentSizeGraphsMarketing, kWindowSizeGraphsMarketing)
    );

    static constexpr auto _windowFinancesParkValueWidgets = makeWidgets(
        makeFinancesWidgets(STR_PARK_VALUE_GRAPH, kTabContentSizeGraphsMarketing, kWindowSizeGraphsMarketing)
    );

    static constexpr auto _windowFinancesProfitWidgets = makeWidgets(
        makeFinancesWidgets(STR_PROFIT_GRAPH, kTabContentSizeGraphsMarketing, kWindowSizeGraphsMarketing)
    );

    static constexpr auto _windowFinancesMarketingWidgets = makeWidgets(
        makeFinancesWidgets(STR_MARKETING, kTabContentSizeGraphsMarketing, kWindowSizeGraphsMarketing),
        makeWidget({3, 47}, { kWindowSizeGraphsMarketing.width - 6,  45}, WidgetType::groupbox, WindowColour::tertiary , STR_MARKETING_CAMPAIGNS_IN_OPERATION                                   ),
        makeWidget({3, 47}, { kWindowSizeGraphsMarketing.width - 6, 206}, WidgetType::groupbox, WindowColour::tertiary , STR_MARKETING_CAMPAIGNS_AVAILABLE                                      ),
        makeWidget({8,  0}, {kWindowSizeGraphsMarketing.width - 16,  14}, WidgetType::imgBtn,   WindowColour::secondary, 0xFFFFFFFF,                           STR_START_THIS_MARKETING_CAMPAIGN),
        makeWidget({8,  0}, {kWindowSizeGraphsMarketing.width - 16,  14}, WidgetType::imgBtn,   WindowColour::secondary, 0xFFFFFFFF,                           STR_START_THIS_MARKETING_CAMPAIGN),
        makeWidget({8,  0}, {kWindowSizeGraphsMarketing.width - 16,  14}, WidgetType::imgBtn,   WindowColour::secondary, 0xFFFFFFFF,                           STR_START_THIS_MARKETING_CAMPAIGN),
        makeWidget({8,  0}, {kWindowSizeGraphsMarketing.width - 16,  14}, WidgetType::imgBtn,   WindowColour::secondary, 0xFFFFFFFF,                           STR_START_THIS_MARKETING_CAMPAIGN),
        makeWidget({8,  0}, {kWindowSizeGraphsMarketing.width - 16,  14}, WidgetType::imgBtn,   WindowColour::secondary, 0xFFFFFFFF,                           STR_START_THIS_MARKETING_CAMPAIGN),
        makeWidget({8,  0}, {kWindowSizeGraphsMarketing.width - 16,  14}, WidgetType::imgBtn,   WindowColour::secondary, 0xFFFFFFFF,                           STR_START_THIS_MARKETING_CAMPAIGN)
    );

    static constexpr auto _windowFinancesResearchWidgets = makeWidgets(
        makeFinancesWidgets(STR_RESEARCH_FUNDING, kTabContentSizeResearch, kWindowSizeResearch),
        makeWidget({  3,  47}, { kWindowSizeResearch.width - 6,  45}, WidgetType::groupbox,     WindowColour::tertiary, STR_RESEARCH_FUNDING_                                                             ),
        makeWidget({  8,  59}, {                           160,  14}, WidgetType::dropdownMenu, WindowColour::tertiary, 0xFFFFFFFF,                           STR_SELECT_LEVEL_OF_RESEARCH_AND_DEVELOPMENT),
        makeWidget({156,  60}, {                            11,  12}, WidgetType::button,       WindowColour::tertiary, STR_DROPDOWN_GLYPH,                   STR_SELECT_LEVEL_OF_RESEARCH_AND_DEVELOPMENT),
        makeWidget({  3,  96}, {kWindowSizeResearch.width -  6, 107}, WidgetType::groupbox,     WindowColour::tertiary, STR_RESEARCH_PRIORITIES                                                           ),
        makeWidget({  8, 108}, {kWindowSizeResearch.width - 14,  12}, WidgetType::checkbox,     WindowColour::tertiary, STR_RESEARCH_NEW_TRANSPORT_RIDES,     STR_RESEARCH_NEW_TRANSPORT_RIDES_TIP        ),
        makeWidget({  8, 121}, {kWindowSizeResearch.width - 14,  12}, WidgetType::checkbox,     WindowColour::tertiary, STR_RESEARCH_NEW_GENTLE_RIDES,        STR_RESEARCH_NEW_GENTLE_RIDES_TIP           ),
        makeWidget({  8, 134}, {kWindowSizeResearch.width - 14,  12}, WidgetType::checkbox,     WindowColour::tertiary, STR_RESEARCH_NEW_ROLLER_COASTERS,     STR_RESEARCH_NEW_ROLLER_COASTERS_TIP        ),
        makeWidget({  8, 147}, {kWindowSizeResearch.width - 14,  12}, WidgetType::checkbox,     WindowColour::tertiary, STR_RESEARCH_NEW_THRILL_RIDES,        STR_RESEARCH_NEW_THRILL_RIDES_TIP           ),
        makeWidget({  8, 160}, {kWindowSizeResearch.width - 14,  12}, WidgetType::checkbox,     WindowColour::tertiary, STR_RESEARCH_NEW_WATER_RIDES,         STR_RESEARCH_NEW_WATER_RIDES_TIP            ),
        makeWidget({  8, 173}, {kWindowSizeResearch.width - 14,  12}, WidgetType::checkbox,     WindowColour::tertiary, STR_RESEARCH_NEW_SHOPS_AND_STALLS,    STR_RESEARCH_NEW_SHOPS_AND_STALLS_TIP       ),
        makeWidget({  8, 186}, {kWindowSizeResearch.width - 14,  12}, WidgetType::checkbox,     WindowColour::tertiary, STR_RESEARCH_NEW_SCENERY_AND_THEMING, STR_RESEARCH_NEW_SCENERY_AND_THEMING_TIP    )
    );
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

    static constexpr StringId _windowFinancesSummaryRowLabels[EnumValue(ExpenditureType::count)] = {
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

    static constexpr int32_t kExpenditureColumnWidth = 80;

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
            disabledWidgets = (getGameState().park.flags & PARK_FLAGS_FORBID_MARKETING_CAMPAIGN) ? (1uLL << WIDX_TAB_5) : 0;
        }

    public:
        void onOpen() override
        {
            setPage(WINDOW_FINANCES_PAGE_SUMMARY);
            _lastPaintedMonth = std::numeric_limits<uint32_t>::max();
            ResearchUpdateUncompletedTypes();
            _graphProps.hoverIdx = -1;
        }

        void onUpdate() override
        {
            currentFrame++;
            invalidateWidget(WIDX_TAB_1 + page);

            if (page == WINDOW_FINANCES_PAGE_VALUE_GRAPH || page == WINDOW_FINANCES_PAGE_PROFIT_GRAPH
                || page == WINDOW_FINANCES_PAGE_FINANCIAL_GRAPH)
            {
                if (_graphProps.UpdateHoverIndex())
                {
                    invalidateWidget(WIDX_BACKGROUND);
                }
            }
        }

        void onMouseDown(WidgetIndex widgetIndex) override
        {
            switch (page)
            {
                case WINDOW_FINANCES_PAGE_SUMMARY:
                    onMouseDownSummary(widgetIndex);
                    break;
                case WINDOW_FINANCES_PAGE_RESEARCH:
                    WindowResearchFundingMouseDown(this, widgetIndex, WIDX_RESEARCH_FUNDING);
                    break;
            }
        }

        void onMouseUp(WidgetIndex widgetIndex) override
        {
            switch (widgetIndex)
            {
                case WIDX_CLOSE:
                    close();
                    break;
                case WIDX_TAB_1:
                case WIDX_TAB_2:
                case WIDX_TAB_3:
                case WIDX_TAB_4:
                case WIDX_TAB_5:
                case WIDX_TAB_6:
                    setPage(widgetIndex - WIDX_TAB_1);
                    break;
                default:
                    switch (page)
                    {
                        case WINDOW_FINANCES_PAGE_MARKETING:
                            onMouseUpMarketing(widgetIndex);
                            break;
                        case WINDOW_FINANCES_PAGE_RESEARCH:
                            WindowResearchFundingMouseUp(widgetIndex, WIDX_RESEARCH_FUNDING);
                    }
                    break;
            }
        }

        void onDropdown(WidgetIndex widgetIndex, int32_t selectedIndex) override
        {
            if (page == WINDOW_FINANCES_PAGE_RESEARCH)
            {
                WindowResearchFundingDropdown(widgetIndex, selectedIndex, WIDX_RESEARCH_FUNDING);
            }
        }

        void onPrepareDraw() override
        {
            WindowAlignTabs(this, WIDX_TAB_1, WIDX_TAB_6);

            for (auto i = 0; i < WINDOW_FINANCES_PAGE_COUNT; i++)
                setWidgetPressed(WIDX_TAB_1 + i, false);
            setWidgetPressed(WIDX_TAB_1 + page, true);

            Widget* graphPageWidget;
            bool centredGraph;
            switch (page)
            {
                case WINDOW_FINANCES_PAGE_SUMMARY:
                    onPrepareDrawSummary();
                    return;
                case WINDOW_FINANCES_PAGE_MARKETING:
                    onPrepareDrawMarketing();
                    return;
                case WINDOW_FINANCES_PAGE_RESEARCH:
                    WindowResearchFundingPrepareDraw(this, WIDX_RESEARCH_FUNDING);
                    return;
                case WINDOW_FINANCES_PAGE_VALUE_GRAPH:
                    graphPageWidget = &widgets[WIDX_PAGE_BACKGROUND];
                    centredGraph = false;
                    _graphProps.series = getGameState().park.valueHistory;
                    break;
                case WINDOW_FINANCES_PAGE_PROFIT_GRAPH:
                    graphPageWidget = &widgets[WIDX_PAGE_BACKGROUND];
                    centredGraph = true;
                    _graphProps.series = getGameState().park.weeklyProfitHistory;
                    break;
                case WINDOW_FINANCES_PAGE_FINANCIAL_GRAPH:
                    graphPageWidget = &widgets[WIDX_PAGE_BACKGROUND];
                    centredGraph = true;
                    _graphProps.series = getGameState().park.cashHistory;
                    break;
                default:
                    return;
            }
            onPrepareDrawGraph(graphPageWidget, centredGraph);
        }

        void onDraw(RenderTarget& rt) override
        {
            drawWidgets(rt);
            DrawTabImages(rt);

            switch (page)
            {
                case WINDOW_FINANCES_PAGE_SUMMARY:
                    onDrawSummary(rt);
                    break;
                case WINDOW_FINANCES_PAGE_FINANCIAL_GRAPH:
                {
                    auto& gameState = getGameState();
                    const auto cashLessLoan = gameState.park.cash - gameState.park.bankLoan;
                    const auto fmt = cashLessLoan >= 0 ? STR_FINANCES_FINANCIAL_GRAPH_CASH_LESS_LOAN_POSITIVE
                                                       : STR_FINANCES_FINANCIAL_GRAPH_CASH_LESS_LOAN_NEGATIVE;
                    onDrawGraph(rt, cashLessLoan, fmt);
                    break;
                }
                case WINDOW_FINANCES_PAGE_VALUE_GRAPH:
                    onDrawGraph(rt, getGameState().park.value, STR_FINANCES_PARK_VALUE);
                    break;
                case WINDOW_FINANCES_PAGE_PROFIT_GRAPH:
                {
                    auto& gameState = getGameState();
                    const auto fmt = gameState.park.currentProfit >= 0 ? STR_FINANCES_WEEKLY_PROFIT_POSITIVE
                                                                       : STR_FINANCES_WEEKLY_PROFIT_LOSS;
                    onDrawGraph(rt, gameState.park.currentProfit, fmt);
                    break;
                }
                case WINDOW_FINANCES_PAGE_MARKETING:
                    onDrawMarketing(rt);
                    break;
                case WINDOW_FINANCES_PAGE_RESEARCH:
                    WindowResearchFundingDraw(this, rt);
                    break;
            }
        }

        ScreenSize onScrollGetSize(int32_t scrollIndex) override
        {
            if (page == WINDOW_FINANCES_PAGE_SUMMARY)
            {
                return { kExpenditureColumnWidth * (SummaryMaxAvailableMonth() + 1), 0 };
            }

            return {};
        }

        void onScrollDraw(int32_t scrollIndex, RenderTarget& rt) override
        {
            if (page != WINDOW_FINANCES_PAGE_SUMMARY)
                return;

            auto screenCoords = ScreenCoordsXY{ 0, kTableCellHeight + 2 };

            auto& self = widgets[WIDX_SUMMARY_SCROLL];
            int32_t row_width = std::max<uint16_t>(scrolls[0].contentWidth, self.width() - 1);

            // Expenditure / Income row labels
            for (int32_t i = 0; i < static_cast<int32_t>(ExpenditureType::count); i++)
            {
                // Darken every even row
                if (i % 2 == 0)
                    Rectangle::fill(
                        rt,
                        { screenCoords - ScreenCoordsXY{ 0, 1 },
                          screenCoords + ScreenCoordsXY{ row_width, (kTableCellHeight - 2) } },
                        getColourMap(colours[1].colour).lighter, true);

                screenCoords.y += kTableCellHeight;
            }

            auto& gameState = getGameState();
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
                    rt, screenCoords + ScreenCoordsXY{ kExpenditureColumnWidth, 0 },
                    monthyear == currentMonthYear ? STR_WINDOW_COLOUR_2_STRINGID : STR_BLACK_STRING, ft,
                    { TextUnderline::on, TextAlignment::right });
                screenCoords.y += 14;

                // Month expenditures
                money64 profit = 0;
                for (int32_t j = 0; j < static_cast<int32_t>(ExpenditureType::count); j++)
                {
                    auto expenditure = gameState.park.expenditureTable[i][j];
                    if (expenditure != 0)
                    {
                        profit += expenditure;
                        const StringId format = expenditure >= 0 ? STR_FINANCES_SUMMARY_INCOME_VALUE
                                                                 : STR_FINANCES_SUMMARY_EXPENDITURE_VALUE;
                        ft = Formatter();
                        ft.Add<money64>(expenditure);
                        DrawTextBasic(
                            rt, screenCoords + ScreenCoordsXY{ kExpenditureColumnWidth, 0 }, format, ft,
                            { TextAlignment::right });
                    }
                    screenCoords.y += kTableCellHeight;
                }
                screenCoords.y += 4;

                // Month profit
                const StringId format = profit >= 0 ? STR_FINANCES_SUMMARY_INCOME_VALUE : STR_FINANCES_SUMMARY_LOSS_VALUE;
                ft = Formatter();
                ft.Add<money64>(profit);
                DrawTextBasic(
                    rt, screenCoords + ScreenCoordsXY{ kExpenditureColumnWidth, 0 }, format, ft, { TextAlignment::right });

                Rectangle::fill(
                    rt,
                    { screenCoords + ScreenCoordsXY{ 10, -2 }, screenCoords + ScreenCoordsXY{ kExpenditureColumnWidth, -2 } },
                    PaletteIndex::pi10);

                screenCoords.x += kExpenditureColumnWidth;
            }

            _lastPaintedMonth = currentMonthYear;
        }

        void setPage(int32_t p)
        {
            // Skip setting page if we're already on this page, unless we're initialising the window
            if (page == p && !widgets.empty())
                return;

            page = p;
            currentFrame = 0;

            invalidate();
            if (p == WINDOW_FINANCES_PAGE_RESEARCH)
            {
                width = kWindowSizeResearch.width;
                height = kWindowSizeResearch.height;
                flags.unset(WindowFlag::resizable);
            }
            else if (p == WINDOW_FINANCES_PAGE_SUMMARY)
            {
                width = kWindowSizeSummary.width;
                height = kWindowSizeSummary.height;
                flags.unset(WindowFlag::resizable);
            }
            else if (
                p == WINDOW_FINANCES_PAGE_VALUE_GRAPH || p == WINDOW_FINANCES_PAGE_PROFIT_GRAPH
                || p == WINDOW_FINANCES_PAGE_FINANCIAL_GRAPH)
            {
                flags |= WindowFlag::resizable;

                // We need to compensate for the enlarged title bar for windows that do not
                // constrain the window height between tabs (e.g. chart tabs)
                height -= getTitleBarDiffNormal();

                WindowSetResize(*this, kWindowSizeGraphsMarketing, kMaxWindowSize);
            }
            else
            {
                width = kWindowSizeGraphsMarketing.width;
                height = kWindowSizeGraphsMarketing.height;
                flags.unset(WindowFlag::resizable);
            }

            setWidgets(_windowFinancesPageWidgets[p]);
            SetDisabledTabs();

            holdDownWidgets = _windowFinancesPageHoldDownWidgets[p];
            pressedWidgets = 0;

            resizeFrame();
            onPrepareDraw();
            initScrollWidgets();

            // Scroll summary all the way to the right, initially.
            if (p == WINDOW_FINANCES_PAGE_SUMMARY)
                initialiseScrollPosition(WIDX_SUMMARY_SCROLL, 0);

            invalidate();
        }

#pragma region Summary Events

        void onMouseDownSummary(WidgetIndex widgetIndex)
        {
            auto& gameState = getGameState();
            switch (widgetIndex)
            {
                case WIDX_LOAN_INCREASE:
                {
                    // If loan can be increased, do so.
                    // If not, action shows error message.
                    auto newLoan = gameState.park.bankLoan + 1000.00_GBP;
                    if (gameState.park.bankLoan < gameState.park.maxBankLoan)
                    {
                        newLoan = std::min(gameState.park.maxBankLoan, newLoan);
                    }
                    auto gameAction = GameActions::ParkSetLoanAction(newLoan);
                    GameActions::Execute(&gameAction, gameState);
                    break;
                }
                case WIDX_LOAN_DECREASE:
                {
                    // If loan is positive, decrease it.
                    // If loan is negative, action shows error message.
                    // If loan is exactly 0, prevent error message.
                    if (gameState.park.bankLoan != 0)
                    {
                        auto newLoan = gameState.park.bankLoan - 1000.00_GBP;
                        if (gameState.park.bankLoan > 0)
                        {
                            newLoan = std::max(0.00_GBP, newLoan);
                        }
                        auto gameAction = GameActions::ParkSetLoanAction(newLoan);
                        GameActions::Execute(&gameAction, gameState);
                    }
                    break;
                }
            }
        }

        void onPrepareDrawSummary()
        {
            // Setting loan widget's format arguments here.
            // Nothing else should use the global formatter until
            // drawing has completed.
            auto ft = Formatter::Common();
            ft.Increment(6);
            ft.Add<money64>(getGameState().park.bankLoan);

            // Keep up with new months being added in the first two years.
            if (GetDate().GetMonthsElapsed() != _lastPaintedMonth)
                initialiseScrollPosition(WIDX_SUMMARY_SCROLL, 0);
        }

        void onDrawSummary(RenderTarget& rt)
        {
            auto titleBarBottom = widgets[WIDX_TITLE].bottom;
            auto screenCoords = windowPos + ScreenCoordsXY{ 8, titleBarBottom + 37 };
            auto& gameState = getGameState();

            // Expenditure / Income heading
            DrawTextBasic(
                rt, screenCoords, STR_FINANCES_SUMMARY_EXPENDITURE_INCOME, {},
                { Drawing::Colour::black, TextUnderline::on, TextAlignment::left });
            screenCoords.y += 14;

            // Expenditure / Income row labels
            for (int32_t i = 0; i < static_cast<int32_t>(ExpenditureType::count); i++)
            {
                // Darken every even row
                if (i % 2 == 0)
                    Rectangle::fill(
                        rt,
                        { screenCoords - ScreenCoordsXY{ 0, 1 }, screenCoords + ScreenCoordsXY{ 121, (kTableCellHeight - 2) } },
                        getColourMap(colours[1].colour).lighter, true);

                DrawTextBasic(rt, screenCoords - ScreenCoordsXY{ 0, 1 }, _windowFinancesSummaryRowLabels[i]);
                screenCoords.y += kTableCellHeight;
            }

            // Horizontal rule below expenditure / income table
            Rectangle::fillInset(
                rt,
                { windowPos + ScreenCoordsXY{ 8, titleBarBottom + 258 },
                  windowPos + ScreenCoordsXY{ 8 + 513, titleBarBottom + 258 + 1 } },
                colours[1], Rectangle::BorderStyle::inset);

            // Loan and interest rate
            DrawTextBasic(rt, windowPos + ScreenCoordsXY{ 8, titleBarBottom + 265 }, STR_FINANCES_SUMMARY_LOAN);
            if (!(gameState.park.flags & PARK_FLAGS_RCT1_INTEREST))
            {
                auto ft = Formatter();
                ft.Add<uint16_t>(gameState.park.bankLoanInterestRate);
                DrawTextBasic(
                    rt, windowPos + ScreenCoordsXY{ 167, titleBarBottom + 265 }, STR_FINANCES_SUMMARY_AT_X_PER_YEAR, ft);
            }

            // Current cash
            auto ft = Formatter();
            ft.Add<money64>(gameState.park.cash);
            StringId stringId = gameState.park.cash >= 0 ? STR_CASH_LABEL : STR_CASH_NEGATIVE_LABEL;
            DrawTextBasic(rt, windowPos + ScreenCoordsXY{ 8, titleBarBottom + 280 }, stringId, ft);

            // Objective related financial information
            if (gameState.scenarioOptions.objective.Type == Scenario::ObjectiveType::monthlyFoodIncome)
            {
                auto lastMonthProfit = FinanceGetLastMonthShopProfit();
                ft = Formatter();
                ft.Add<money64>(lastMonthProfit);
                DrawTextBasic(
                    rt, windowPos + ScreenCoordsXY{ 280, titleBarBottom + 265 },
                    STR_LAST_MONTH_PROFIT_FROM_FOOD_DRINK_MERCHANDISE_SALES_LABEL, ft);
            }
            else
            {
                // Park value and company value
                ft = Formatter();
                ft.Add<money64>(gameState.park.value);
                DrawTextBasic(rt, windowPos + ScreenCoordsXY{ 280, titleBarBottom + 265 }, STR_PARK_VALUE_LABEL, ft);
                ft = Formatter();
                ft.Add<money64>(gameState.park.companyValue);
                DrawTextBasic(rt, windowPos + ScreenCoordsXY{ 280, titleBarBottom + 280 }, STR_COMPANY_VALUE_LABEL, ft);
            }
        }

        uint16_t SummaryMaxAvailableMonth()
        {
            return std::min<uint16_t>(GetDate().GetMonthsElapsed(), kExpenditureTableMonthCount - 1);
        }

#pragma endregion

#pragma region Marketing Events

        void onMouseUpMarketing(WidgetIndex widgetIndex)
        {
            if (widgetIndex >= WIDX_CAMPAIGN_1 && widgetIndex <= WIDX_CAMPAIGN_6)
            {
                ContextOpenDetailWindow(WindowDetail::newCampaign, widgetIndex - WIDX_CAMPAIGN_1);
            }
        }

        void onPrepareDrawMarketing()
        {
            // Count number of active campaigns
            int32_t numActiveCampaigns = static_cast<int32_t>(getGameState().park.marketingCampaigns.size());
            int32_t y = widgets[WIDX_TAB_1].top + std::max(1, numActiveCampaigns) * kListRowHeight + 75;

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
                    campaignButton->type = WidgetType::button;
                    campaignButton->top = y;
                    campaignButton->bottom = y + kButtonFaceHeight + 1;
                    y += kButtonFaceHeight + 2;
                }
                else
                {
                    campaignButton->type = WidgetType::empty;
                }
            }
        }

        void onDrawMarketing(RenderTarget& rt)
        {
            auto screenCoords = windowPos + ScreenCoordsXY{ 8, widgets[WIDX_TAB_1].top + 45 };
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
                            campaignRide->formatNameTo(ft);
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
                        auto parkName = getGameState().park.name.c_str();
                        ft.Add<StringId>(STR_STRING);
                        ft.Add<const char*>(parkName);
                    }
                }
                // Advertisement
                DrawTextEllipsised(rt, screenCoords + ScreenCoordsXY{ 4, 0 }, 296, kMarketingCampaignNames[i][1], ft);

                // Duration
                uint16_t weeksRemaining = marketingCampaign->WeeksLeft;
                ft = Formatter();
                ft.Add<uint16_t>(weeksRemaining);
                DrawTextBasic(
                    rt, screenCoords + ScreenCoordsXY{ 304, 0 },
                    weeksRemaining == 1 ? STR_1_WEEK_REMAINING : STR_X_WEEKS_REMAINING, ft);

                screenCoords.y += kListRowHeight;
            }

            if (noCampaignsActive)
            {
                DrawTextBasic(rt, screenCoords + ScreenCoordsXY{ 4, 0 }, STR_MARKETING_CAMPAIGNS_NONE);
            }

            // Draw campaign button text
            for (int32_t i = 0; i < ADVERTISING_CAMPAIGN_COUNT; i++)
            {
                auto campaignButton = &widgets[WIDX_CAMPAIGN_1 + i];
                if (campaignButton->type != WidgetType::empty)
                {
                    // Draw button text
                    screenCoords = windowPos + ScreenCoordsXY{ campaignButton->left, campaignButton->textTop() };
                    DrawTextBasic(rt, screenCoords + ScreenCoordsXY{ 4, 0 }, kMarketingCampaignNames[i][0]);
                    auto ft = Formatter();
                    ft.Add<money64>(AdvertisingCampaignPricePerWeek[i]);
                    DrawTextBasic(rt, screenCoords + ScreenCoordsXY{ kCostPerWeekOffset, 0 }, STR_MARKETING_PER_WEEK, ft);
                }
            }
        }

#pragma endregion

#pragma region Graph Events

        void onDrawGraph(RenderTarget& rt, const money64 currentValue, const StringId fmt) const
        {
            Formatter ft;
            ft.Add<money64>(currentValue);
            DrawTextBasic(rt, _graphBounds.Point1 - ScreenCoordsXY{ 0, 11 }, fmt, ft);

            // Graph
            Rectangle::fillInset(
                rt, _graphBounds, colours[1], Rectangle::BorderStyle::inset, Rectangle::FillBrightness::light,
                Rectangle::FillMode::none);
            // hide resize widget on graph area
            constexpr ScreenCoordsXY offset{ 1, 1 };
            constexpr ScreenCoordsXY bigOffset{ 5, 5 };
            Rectangle::fillInset(
                rt, { _graphBounds.Point2 - bigOffset, _graphBounds.Point2 - offset }, colours[1], Rectangle::BorderStyle::none,
                Rectangle::FillBrightness::light, Rectangle::FillMode::dontLightenWhenInset);

            Graph::DrawFinanceGraph(rt, _graphProps);
        }

        void onPrepareDrawGraph(const Widget* graphPageWidget, const bool centredGraph)
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
            int32_t maxGraphWidth = GfxGetStringWidth(buffer, FontStyle::small) + Graph::kYTickMarkPadding + 1;
            const ScreenCoordsXY dynamicPadding{ std::max(maxGraphWidth, kGraphTopLeftPadding.x), kGraphTopLeftPadding.y };

            _graphBounds = { windowPos + ScreenCoordsXY{ graphPageWidget->left + 4, graphPageWidget->top + 15 },
                             windowPos + ScreenCoordsXY{ graphPageWidget->right - 4, graphPageWidget->bottom - 4 } };
            _graphProps.RecalculateLayout(
                { _graphBounds.Point1 + dynamicPadding, _graphBounds.Point2 - kGraphBottomRightPadding }, kGraphNumYLabels,
                kGraphNumPoints);
            _graphProps.lineCol = colours[2];
        }

#pragma endregion

        void initialiseScrollPosition(WidgetIndex widgetIndex, int32_t scrollId)
        {
            const auto& widget = this->widgets[widgetIndex];
            scrolls[scrollId].contentOffsetX = std::max(0, scrolls[scrollId].contentWidth - (widget.width() - 3));

            widgetScrollUpdateThumbs(*this, widgetIndex);
        }

        void DrawTabImage(RenderTarget& rt, int32_t tabPage, int32_t spriteIndex)
        {
            WidgetIndex widgetIndex = WIDX_TAB_1 + tabPage;

            if (!isWidgetDisabled(widgetIndex))
            {
                if (this->page == tabPage)
                {
                    int32_t frame = currentFrame / 2;
                    spriteIndex += (frame % _windowFinancesTabAnimationFrames[this->page]);
                }

                GfxDrawSprite(
                    rt, ImageId(spriteIndex),
                    windowPos + ScreenCoordsXY{ widgets[widgetIndex].left, widgets[widgetIndex].top });
            }
        }

        void DrawTabImages(RenderTarget& rt)
        {
            DrawTabImage(rt, WINDOW_FINANCES_PAGE_SUMMARY, SPR_TAB_FINANCES_SUMMARY_0);
            DrawTabImage(rt, WINDOW_FINANCES_PAGE_FINANCIAL_GRAPH, SPR_TAB_FINANCES_FINANCIAL_GRAPH_0);
            DrawTabImage(rt, WINDOW_FINANCES_PAGE_VALUE_GRAPH, SPR_TAB_FINANCES_VALUE_GRAPH_0);
            DrawTabImage(rt, WINDOW_FINANCES_PAGE_PROFIT_GRAPH, SPR_TAB_FINANCES_PROFIT_GRAPH_0);
            DrawTabImage(rt, WINDOW_FINANCES_PAGE_MARKETING, SPR_TAB_FINANCES_MARKETING_0);
            DrawTabImage(rt, WINDOW_FINANCES_PAGE_RESEARCH, SPR_TAB_FINANCES_RESEARCH_0);
        }
    };

    static FinancesWindow* FinancesWindowOpen(uint8_t page)
    {
        auto* windowMgr = GetWindowManager();
        auto* window = windowMgr->FocusOrCreate<FinancesWindow>(
            WindowClass::finances, kWindowSizeSummary, WindowFlag::higherContrastOnPress);

        if (window != nullptr && page != WINDOW_FINANCES_PAGE_SUMMARY)
            window->setPage(page);

        return window;
    }

    WindowBase* FinancesOpen()
    {
        auto* windowMgr = GetWindowManager();
        return windowMgr->FocusOrCreate<FinancesWindow>(
            WindowClass::finances, kWindowSizeSummary, WindowFlag::higherContrastOnPress);
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
