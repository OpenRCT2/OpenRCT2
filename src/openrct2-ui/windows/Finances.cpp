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

static constexpr const int32_t WH_SUMMARY = 309;
static constexpr const int32_t WH_RESEARCH = 207;
static constexpr const int32_t WH_OTHER_TABS = 257;
static constexpr const int32_t WW_RESEARCH = 320;
static constexpr const int32_t WW_OTHER_TABS = 530;

// clang-format off
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

    WIDX_SUMMARY_SCROLL = 10,
    WIDX_LOAN,
    WIDX_LOAN_INCREASE,
    WIDX_LOAN_DECREASE,

    WIDX_ACTIVE_CAMPAIGNS_GROUP = 10,
    WIDX_CAMPAIGNS_AVAILABLE_GROUP,
    WIDX_CAMPAIGN_1,
    WIDX_CAMPAIGN_2,
    WIDX_CAMPAIGN_3,
    WIDX_CAMPAIGN_4,
    WIDX_CAMPAIGN_5,
    WIDX_CAMPAIGN_6,

    WIDX_RESEARCH_FUNDING = 11,
    WIDX_RESEARCH_FUNDING_DROPDOWN_BUTTON,
    WIDX_TRANSPORT_RIDES = 14,
    WIDX_GENTLE_RIDES,
    WIDX_ROLLER_COASTERS,
    WIDX_THRILL_RIDES,
    WIDX_WATER_RIDES,
    WIDX_SHOPS_AND_STALLS,
    WIDX_SCENERY_AND_THEMING,
};

#pragma region Widgets

#define TAB_WIDGETS \
    MakeTab({  3, 17}, STR_FINANCES_SHOW_SUMMARY_TAB_TIP      ), \
    MakeTab({ 34, 17}, STR_FINANCES_SHOW_CASH_TAB_TIP         ), \
    MakeTab({ 65, 17}, STR_FINANCES_SHOW_PARK_VALUE_TAB_TIP   ), \
    MakeTab({ 96, 17}, STR_FINANCES_SHOW_WEEKLY_PROFIT_TAB_TIP), \
    MakeTab({127, 17}, STR_FINANCES_SHOW_MARKETING_TAB_TIP    ), \
    MakeTab({158, 17}, STR_FINANCES_RESEARCH_TIP              )

static rct_widget _windowFinancesSummaryWidgets[] =
{
    WINDOW_SHIM(STR_FINANCIAL_SUMMARY, WW_OTHER_TABS, WH_SUMMARY),
    MakeWidget        ({  0,  43}, {530, 266}, WindowWidgetType::Resize,  WindowColour::Secondary),
    TAB_WIDGETS,
    MakeWidget        ({130,  50}, {391, 211}, WindowWidgetType::Scroll,  WindowColour::Secondary, SCROLL_HORIZONTAL              ),
    MakeSpinnerWidgets({ 64, 279}, { 97,  14}, WindowWidgetType::Spinner, WindowColour::Secondary, STR_FINANCES_SUMMARY_LOAN_VALUE), // NB: 3 widgets.
    WIDGETS_END,
};

static rct_widget _windowFinancesCashWidgets[] =
{
    WINDOW_SHIM(STR_FINANCIAL_GRAPH, WW_OTHER_TABS, WH_OTHER_TABS),
    MakeWidget({0, 43}, {WW_OTHER_TABS, 214}, WindowWidgetType::Resize, WindowColour::Secondary),
    TAB_WIDGETS,
    WIDGETS_END,
};

static rct_widget _windowFinancesParkValueWidgets[] =
{
    WINDOW_SHIM(STR_PARK_VALUE_GRAPH, WW_OTHER_TABS, WH_OTHER_TABS),
    MakeWidget({0, 43}, {WW_OTHER_TABS, 214}, WindowWidgetType::Resize, WindowColour::Secondary),
    TAB_WIDGETS,
    WIDGETS_END,
};

static rct_widget _windowFinancesProfitWidgets[] =
{
    WINDOW_SHIM(STR_PROFIT_GRAPH, WW_OTHER_TABS, WH_OTHER_TABS),
    MakeWidget({0, 43}, {WW_OTHER_TABS, 214}, WindowWidgetType::Resize, WindowColour::Secondary),
    TAB_WIDGETS,
    WIDGETS_END,
};

static rct_widget _windowFinancesMarketingWidgets[] =
{
    WINDOW_SHIM(STR_MARKETING, WW_OTHER_TABS, WH_OTHER_TABS),
    MakeWidget({0, 43}, {WW_OTHER_TABS, 214}, WindowWidgetType::Resize, WindowColour::Secondary),
    TAB_WIDGETS,
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
    WINDOW_SHIM(STR_RESEARCH_FUNDING, WW_RESEARCH, WH_RESEARCH),
    MakeWidget({0, 43}, {WW_RESEARCH, 164}, WindowWidgetType::Resize, WindowColour::Secondary),
    TAB_WIDGETS,
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

static rct_widget *_windowFinancesPageWidgets[] =
{
    _windowFinancesSummaryWidgets,
    _windowFinancesCashWidgets,
    _windowFinancesParkValueWidgets,
    _windowFinancesProfitWidgets,
    _windowFinancesMarketingWidgets,
    _windowFinancesResearchWidgets
};

#pragma endregion

#pragma region Events

static void WindowFinancesSummaryMouseup(rct_window *w, rct_widgetindex widgetIndex);
static void WindowFinancesSummaryMousedown(rct_window *w, rct_widgetindex widgetIndex, rct_widget* widget);
static void WindowFinancesSummaryScrollgetsize(rct_window *w, int32_t scrollIndex, int32_t *width, int32_t *height);
static void WindowFinancesSummaryInvertscroll(rct_window *w);
static void WindowFinancesSummaryUpdate(rct_window *w);
static void WindowFinancesSummaryInvalidate(rct_window *w);
static void WindowFinancesSummaryPaint(rct_window *w, rct_drawpixelinfo *dpi);
static void WindowFinancesSummaryScrollpaint(rct_window *w, rct_drawpixelinfo *dpi, int32_t scrollIndex);

static void WindowFinancesFinancialGraphMouseup(rct_window *w, rct_widgetindex widgetIndex);
static void WindowFinancesFinancialGraphUpdate(rct_window *w);
static void WindowFinancesFinancialGraphInvalidate(rct_window *w);
static void WindowFinancesFinancialGraphPaint(rct_window *w, rct_drawpixelinfo *dpi);

static void WindowFinancesParkValueGraphMouseup(rct_window *w, rct_widgetindex widgetIndex);
static void WindowFinancesParkValueGraphUpdate(rct_window *w);
static void WindowFinancesParkValueGraphInvalidate(rct_window *w);
static void WindowFinancesParkValueGraphPaint(rct_window *w, rct_drawpixelinfo *dpi);

static void WindowFinancesProfitGraphMouseup(rct_window *w, rct_widgetindex widgetIndex);
static void WindowFinancesProfitGraphUpdate(rct_window *w);
static void WindowFinancesProfitGraphInvalidate(rct_window *w);
static void WindowFinancesProfitGraphPaint(rct_window *w, rct_drawpixelinfo *dpi);

static void WindowFinancesMarketingMouseup(rct_window *w, rct_widgetindex widgetIndex);
static void WindowFinancesMarketingUpdate(rct_window *w);
static void WindowFinancesMarketingInvalidate(rct_window *w);
static void WindowFinancesMarketingPaint(rct_window *w, rct_drawpixelinfo *dpi);

static void WindowFinancesResearchMouseup(rct_window *w, rct_widgetindex widgetIndex);
static void WindowFinancesResearchMousedown(rct_window *w, rct_widgetindex widgetIndex, rct_widget* widget);
static void WindowFinancesResearchDropdown(rct_window *w, rct_widgetindex widgetIndex, int32_t dropdownIndex);
static void WindowFinancesResearchUpdate(rct_window *w);
static void WindowFinancesResearchInvalidate(rct_window *w);
static void WindowFinancesResearchPaint(rct_window *w, rct_drawpixelinfo *dpi);

// 0x00988EB8
static rct_window_event_list _windowFinancesSummaryEvents([](auto& events)
{
    events.mouse_up = &WindowFinancesSummaryMouseup;
    events.mouse_down = &WindowFinancesSummaryMousedown;
    events.update = &WindowFinancesSummaryUpdate;
    events.get_scroll_size = &WindowFinancesSummaryScrollgetsize;
    events.invalidate = &WindowFinancesSummaryInvalidate;
    events.paint = &WindowFinancesSummaryPaint;
    events.scroll_paint = &WindowFinancesSummaryScrollpaint;
});

// 0x00988F28
static rct_window_event_list _windowFinancesFinancialGraphEvents([](auto& events)
{
    events.mouse_up = &WindowFinancesFinancialGraphMouseup;
    events.update = &WindowFinancesFinancialGraphUpdate;
    events.invalidate = &WindowFinancesFinancialGraphInvalidate;
    events.paint = &WindowFinancesFinancialGraphPaint;
});

// 0x00988F98
static rct_window_event_list _windowFinancesValueGraphEvents([](auto& events)
{
    events.mouse_up = &WindowFinancesParkValueGraphMouseup;
    events.update = &WindowFinancesParkValueGraphUpdate;
    events.invalidate = &WindowFinancesParkValueGraphInvalidate;
    events.paint = &WindowFinancesParkValueGraphPaint;
});

// 0x00989008
static rct_window_event_list _windowFinancesProfitGraphEvents([](auto& events)
{
    events.mouse_up = &WindowFinancesProfitGraphMouseup;
    events.update = &WindowFinancesProfitGraphUpdate;
    events.invalidate = &WindowFinancesProfitGraphInvalidate;
    events.paint = &WindowFinancesProfitGraphPaint;
});

// 0x00989078
static rct_window_event_list _windowFinancesMarketingEvents([](auto& events)
{
    events.mouse_up = &WindowFinancesMarketingMouseup;
    events.update = &WindowFinancesMarketingUpdate;
    events.invalidate = &WindowFinancesMarketingInvalidate;
    events.paint = &WindowFinancesMarketingPaint;
});

// 0x009890E8
static rct_window_event_list _windowFinancesResearchEvents([](auto& events)
{
    events.mouse_up = &WindowFinancesResearchMouseup;
    events.mouse_down = &WindowFinancesResearchMousedown;
    events.dropdown = &WindowFinancesResearchDropdown;
    events.update = &WindowFinancesResearchUpdate;
    events.invalidate = &WindowFinancesResearchInvalidate;
    events.paint = &WindowFinancesResearchPaint;
});

static rct_window_event_list *const _windowFinancesPageEvents[] =
{
    &_windowFinancesSummaryEvents,
    &_windowFinancesFinancialGraphEvents,
    &_windowFinancesValueGraphEvents,
    &_windowFinancesProfitGraphEvents,
    &_windowFinancesMarketingEvents,
    &_windowFinancesResearchEvents
};

#pragma endregion

#pragma region Enabled widgets

#define ALWAYS_ENABLED_WIDGETS \
    ((1ULL << WIDX_CLOSE) | \
    (1ULL << WIDX_TAB_1) | \
    (1ULL << WIDX_TAB_2) | \
    (1ULL << WIDX_TAB_3) | \
    (1ULL << WIDX_TAB_4) | \
    (1ULL << WIDX_TAB_5) | \
    (1ULL << WIDX_TAB_6))

static constexpr const uint32_t WindowFinancesPageEnabledWidgets[] =
{
    ALWAYS_ENABLED_WIDGETS |
    (1ULL << WIDX_SUMMARY_SCROLL) |
    (1ULL << WIDX_LOAN_INCREASE) |
    (1ULL << WIDX_LOAN_DECREASE),

    ALWAYS_ENABLED_WIDGETS,

    ALWAYS_ENABLED_WIDGETS,

    ALWAYS_ENABLED_WIDGETS,

    ALWAYS_ENABLED_WIDGETS |
    (1ULL << WIDX_CAMPAIGN_1) |
    (1ULL << WIDX_CAMPAIGN_2) |
    (1ULL << WIDX_CAMPAIGN_3) |
    (1ULL << WIDX_CAMPAIGN_4) |
    (1ULL << WIDX_CAMPAIGN_5) |
    (1ULL << WIDX_CAMPAIGN_6),

    ALWAYS_ENABLED_WIDGETS |
    (1ULL << WIDX_RESEARCH_FUNDING) |
    (1ULL << WIDX_RESEARCH_FUNDING_DROPDOWN_BUTTON) |
    (1ULL << WIDX_TRANSPORT_RIDES) |
    (1ULL << WIDX_GENTLE_RIDES) |
    (1ULL << WIDX_ROLLER_COASTERS) |
    (1ULL << WIDX_THRILL_RIDES) |
    (1ULL << WIDX_WATER_RIDES) |
    (1ULL << WIDX_SHOPS_AND_STALLS) |
    (1ULL << WIDX_SCENERY_AND_THEMING)
};

static constexpr const uint32_t WindowFinancesPageHoldDownWidgets[] =
{
    (1ULL << WIDX_LOAN_INCREASE) |
    (1ULL << WIDX_LOAN_DECREASE),

    0,
    0,
    0,
    0,
    0
};

#pragma endregion

static constexpr const int32_t WindowFinancesTabAnimationLoops[] =
{
    16, 32, 32, 32, 38, 16
};

static constexpr const int32_t EXPENDITURE_COLUMN_WIDTH = 80;

static int32_t _lastPaintedMonth;

static constexpr const rct_string_id window_finances_summary_row_labels[static_cast<int32_t>(ExpenditureType::Count)] = {
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
// clang-format on

static void WindowFinancesSetPage(rct_window* w, int32_t page);
static void WindowFinancesSetPressedTab(rct_window* w);
static void WindowFinancesDrawTabImages(rct_drawpixelinfo* dpi, rct_window* w);

/**
 *
 *  rct2: 0x0069DDF1
 */
rct_window* WindowFinancesOpen()
{
    rct_window* w;

    w = window_bring_to_front_by_class(WC_FINANCES);
    if (w == nullptr)
    {
        w = WindowCreateAutoPos(WW_OTHER_TABS, WH_SUMMARY, _windowFinancesPageEvents[0], WC_FINANCES, WF_10);
        w->number = 0;
        w->frame_no = 0;

        ResearchUpdateUncompletedTypes();
    }

    w->page = WINDOW_FINANCES_PAGE_SUMMARY;
    w->Invalidate();
    w->width = WW_OTHER_TABS;
    w->height = WH_SUMMARY;
    w->Invalidate();

    w->widgets = _windowFinancesPageWidgets[WINDOW_FINANCES_PAGE_SUMMARY];
    w->enabled_widgets = WindowFinancesPageEnabledWidgets[WINDOW_FINANCES_PAGE_SUMMARY];
    w->hold_down_widgets = WindowFinancesPageHoldDownWidgets[WINDOW_FINANCES_PAGE_SUMMARY];
    w->event_handlers = _windowFinancesPageEvents[WINDOW_FINANCES_PAGE_SUMMARY];
    w->pressed_widgets = 0;
    w->disabled_widgets = 0;

    WindowInitScrollWidgets(w);

    // Scroll summary all the way to the right, initially.
    WindowFinancesSummaryInvertscroll(w);

    return w;
}

/**
 *
 *  rct2: 0x0069DDE1
 */
rct_window* WindowFinancesResearchOpen()
{
    rct_window* w;

    WindowFinancesOpen();
    w = window_find_by_class(WC_FINANCES);
    if (w != nullptr)
        WindowFinancesSetPage(w, WINDOW_FINANCES_PAGE_RESEARCH);

    return w;
}

#pragma region Summary page

/**
 *
 *  rct2: 0x0069CA99
 */
static void WindowFinancesSummaryMouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    if (widgetIndex == WIDX_CLOSE)
        window_close(w);
    else if (widgetIndex >= WIDX_TAB_1 && widgetIndex <= WIDX_TAB_6)
        WindowFinancesSetPage(w, widgetIndex - WIDX_TAB_1);
}

/**
 *
 *  rct2: 0x0069CAB0
 */
static void WindowFinancesSummaryMousedown(rct_window* w, rct_widgetindex widgetIndex, rct_widget* widget)
{
    switch (widgetIndex)
    {
        case WIDX_LOAN_INCREASE:
        {
            auto newLoan = gBankLoan + MONEY(1000, 00);
            auto gameAction = ParkSetLoanAction(newLoan);
            GameActions::Execute(&gameAction);
            break;
        }
        case WIDX_LOAN_DECREASE:
            if (gBankLoan > 0)
            {
                auto newLoan = gBankLoan - MONEY(1000, 00);
                auto gameAction = ParkSetLoanAction(newLoan);
                GameActions::Execute(&gameAction);
            }
            break;
    }
}

static uint16_t SummaryMaxAvailableMonth()
{
    return std::min<uint16_t>(gDateMonthsElapsed, EXPENDITURE_TABLE_MONTH_COUNT - 1);
}

static void WindowFinancesSummaryScrollgetsize(rct_window* w, int32_t scrollIndex, int32_t* width, int32_t* height)
{
    *width = EXPENDITURE_COLUMN_WIDTH * (SummaryMaxAvailableMonth() + 1);
}

static void WindowFinancesSummaryInvertscroll(rct_window* w)
{
    rct_widget summary = w->widgets[WIDX_SUMMARY_SCROLL];
    w->scrolls[0].h_left = std::max(0, w->scrolls[0].h_right - (summary.width() - 2));
    WidgetScrollUpdateThumbs(w, WIDX_SUMMARY_SCROLL);
}

/**
 *
 *  rct2: 0x0069CBA6
 */
static void WindowFinancesSummaryUpdate(rct_window* w)
{
    // Tab animation
    if (++w->frame_no >= WindowFinancesTabAnimationLoops[w->page])
        w->frame_no = 0;
    widget_invalidate(w, WIDX_TAB_1);
}

/**
 *
 *  rct2: 0x0069C732
 */
static void WindowFinancesSummaryInvalidate(rct_window* w)
{
    if (w->widgets != _windowFinancesPageWidgets[WINDOW_FINANCES_PAGE_SUMMARY])
    {
        w->widgets = _windowFinancesPageWidgets[WINDOW_FINANCES_PAGE_SUMMARY];
        WindowInitScrollWidgets(w);
    }

    // Keep up with new months being added in the first two years.
    if (gDateMonthsElapsed != _lastPaintedMonth)
        WindowFinancesSummaryInvertscroll(w);

    WindowFinancesSetPressedTab(w);
    auto ft = Formatter::Common();
    ft.Increment(6);
    ft.Add<money64>(gBankLoan);
}

/**
 *
 *  rct2: 0x0069C771
 */
static void WindowFinancesSummaryPaint(rct_window* w, rct_drawpixelinfo* dpi)
{
    WindowDrawWidgets(w, dpi);
    WindowFinancesDrawTabImages(dpi, w);

    auto screenCoords = w->windowPos + ScreenCoordsXY{ 8, 51 };

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
            gfx_fill_rect(
                dpi, { screenCoords - ScreenCoordsXY{ 0, 1 }, screenCoords + ScreenCoordsXY{ 121, (TABLE_CELL_HEIGHT - 2) } },
                ColourMapA[w->colours[1]].lighter | 0x1000000);

        DrawTextBasic(dpi, screenCoords - ScreenCoordsXY{ 0, 1 }, window_finances_summary_row_labels[i]);
        screenCoords.y += TABLE_CELL_HEIGHT;
    }

    // Horizontal rule below expenditure / income table
    gfx_fill_rect_inset(
        dpi, { w->windowPos + ScreenCoordsXY{ 8, 272 }, w->windowPos + ScreenCoordsXY{ 8 + 513, 272 + 1 } }, w->colours[1],
        INSET_RECT_FLAG_BORDER_INSET);

    // Loan and interest rate
    DrawTextBasic(dpi, w->windowPos + ScreenCoordsXY{ 8, 279 }, STR_FINANCES_SUMMARY_LOAN);
    auto ft = Formatter();
    ft.Add<uint16_t>(gBankLoanInterestRate);
    DrawTextBasic(dpi, w->windowPos + ScreenCoordsXY{ 167, 279 }, STR_FINANCES_SUMMARY_AT_X_PER_YEAR, ft);

    // Current cash
    ft = Formatter();
    ft.Add<money64>(gCash);
    rct_string_id stringId = gCash >= 0 ? STR_CASH_LABEL : STR_CASH_NEGATIVE_LABEL;
    DrawTextBasic(dpi, w->windowPos + ScreenCoordsXY{ 8, 294 }, stringId, ft);

    // Objective related financial information
    if (gScenarioObjective.Type == OBJECTIVE_MONTHLY_FOOD_INCOME)
    {
        auto lastMonthProfit = finance_get_last_month_shop_profit();
        ft = Formatter();
        ft.Add<money64>(lastMonthProfit);
        DrawTextBasic(
            dpi, w->windowPos + ScreenCoordsXY{ 280, 279 }, STR_LAST_MONTH_PROFIT_FROM_FOOD_DRINK_MERCHANDISE_SALES_LABEL, ft);
    }
    else
    {
        // Park value and company value
        ft = Formatter();
        ft.Add<money64>(gParkValue);
        DrawTextBasic(dpi, w->windowPos + ScreenCoordsXY{ 280, 279 }, STR_PARK_VALUE_LABEL, ft);
        ft = Formatter();
        ft.Add<money64>(gCompanyValue);
        DrawTextBasic(dpi, w->windowPos + ScreenCoordsXY{ 280, 294 }, STR_COMPANY_VALUE_LABEL, ft);
    }
}

static void WindowFinancesSummaryScrollpaint(rct_window* w, rct_drawpixelinfo* dpi, int32_t scrollIndex)
{
    auto screenCoords = ScreenCoordsXY{ 0, TABLE_CELL_HEIGHT + 2 };

    rct_widget self = w->widgets[WIDX_SUMMARY_SCROLL];
    int32_t row_width = std::max<uint16_t>(w->scrolls[0].h_right, self.width());

    // Expenditure / Income row labels
    for (int32_t i = 0; i < static_cast<int32_t>(ExpenditureType::Count); i++)
    {
        // Darken every even row
        if (i % 2 == 0)
            gfx_fill_rect(
                dpi,
                { screenCoords - ScreenCoordsXY{ 0, 1 }, screenCoords + ScreenCoordsXY{ row_width, (TABLE_CELL_HEIGHT - 2) } },
                ColourMapA[w->colours[1]].lighter | 0x1000000);

        screenCoords.y += TABLE_CELL_HEIGHT;
    }

    // Expenditure / Income values for each month
    int16_t currentMonthYear = static_cast<int16_t>(gDateMonthsElapsed);
    for (int32_t i = SummaryMaxAvailableMonth(); i >= 0; i--)
    {
        screenCoords.y = 0;

        int16_t monthyear = currentMonthYear - i;
        if (monthyear < 0)
            continue;

        // Month heading
        auto ft = Formatter();
        ft.Add<rct_string_id>(STR_FINANCES_SUMMARY_MONTH_HEADING);
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
            auto expenditure = gExpenditureTable[i][j];
            if (expenditure != 0)
            {
                profit += expenditure;
                const rct_string_id format = expenditure >= 0 ? STR_FINANCES_SUMMARY_INCOME_VALUE
                                                              : STR_FINANCES_SUMMARY_EXPENDITURE_VALUE;
                ft = Formatter();
                ft.Add<money64>(expenditure);
                DrawTextBasic(
                    dpi, screenCoords + ScreenCoordsXY{ EXPENDITURE_COLUMN_WIDTH, 0 }, format, ft, { TextAlignment::RIGHT });
            }
            screenCoords.y += TABLE_CELL_HEIGHT;
        }
        screenCoords.y += 4;

        // Month profit
        const rct_string_id format = profit >= 0 ? STR_FINANCES_SUMMARY_INCOME_VALUE : STR_FINANCES_SUMMARY_LOSS_VALUE;
        ft = Formatter();
        ft.Add<money64>(profit);
        DrawTextBasic(dpi, screenCoords + ScreenCoordsXY{ EXPENDITURE_COLUMN_WIDTH, 0 }, format, ft, { TextAlignment::RIGHT });

        gfx_fill_rect(
            dpi, { screenCoords + ScreenCoordsXY{ 10, -2 }, screenCoords + ScreenCoordsXY{ EXPENDITURE_COLUMN_WIDTH, -2 } },
            PALETTE_INDEX_10);

        screenCoords.x += EXPENDITURE_COLUMN_WIDTH;
    }

    _lastPaintedMonth = currentMonthYear;
}

#pragma endregion

#pragma region Financial graph page

/**
 *
 *  rct2: 0x0069CF70
 */
static void WindowFinancesFinancialGraphMouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    if (widgetIndex == WIDX_CLOSE)
        window_close(w);
    else if (widgetIndex >= WIDX_TAB_1 && widgetIndex <= WIDX_TAB_6)
        WindowFinancesSetPage(w, widgetIndex - WIDX_TAB_1);
}

/**
 *
 *  rct2: 0x0069CF8B
 */
static void WindowFinancesFinancialGraphUpdate(rct_window* w)
{
    // Tab animation
    if (++w->frame_no >= WindowFinancesTabAnimationLoops[w->page])
        w->frame_no = 0;

    w->Invalidate();
}

/**
 *
 *  rct2: 0x0069CBDB
 */
static void WindowFinancesFinancialGraphInvalidate(rct_window* w)
{
    if (w->widgets != _windowFinancesPageWidgets[WINDOW_FINANCES_PAGE_FINANCIAL_GRAPH])
    {
        w->widgets = _windowFinancesPageWidgets[WINDOW_FINANCES_PAGE_FINANCIAL_GRAPH];
        WindowInitScrollWidgets(w);
    }

    WindowFinancesSetPressedTab(w);
}

/**
 *
 *  rct2: 0x0069CC10
 */
static void WindowFinancesFinancialGraphPaint(rct_window* w, rct_drawpixelinfo* dpi)
{
    WindowDrawWidgets(w, dpi);
    WindowFinancesDrawTabImages(dpi, w);

    rct_widget* pageWidget = &_windowFinancesCashWidgets[WIDX_PAGE_BACKGROUND];
    auto graphTopLeft = w->windowPos + ScreenCoordsXY{ pageWidget->left + 4, pageWidget->top + 15 };
    auto graphBottomRight = w->windowPos + ScreenCoordsXY{ pageWidget->right - 4, pageWidget->bottom - 4 };

    // Cash (less loan)
    auto cashLessLoan = gCash - gBankLoan;
    auto ft = Formatter();
    ft.Add<money64>(cashLessLoan);

    DrawTextBasic(
        dpi, graphTopLeft - ScreenCoordsXY{ 0, 11 },
        cashLessLoan >= 0 ? STR_FINANCES_FINANCIAL_GRAPH_CASH_LESS_LOAN_POSITIVE
                          : STR_FINANCES_FINANCIAL_GRAPH_CASH_LESS_LOAN_NEGATIVE,
        ft);

    // Graph
    gfx_fill_rect_inset(dpi, { graphTopLeft, graphBottomRight }, w->colours[1], INSET_RECT_F_30);

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
    for (axisBase = MONEY(12, 00); axisBase >= MONEY(-12, 00); axisBase -= MONEY(6, 00))
    {
        auto axisValue = axisBase << yAxisScale;
        ft = Formatter();
        ft.Add<money64>(axisValue);
        DrawTextBasic(
            dpi, coords + ScreenCoordsXY{ 70, 0 }, STR_FINANCES_FINANCIAL_GRAPH_CASH_VALUE, ft,
            { FontSpriteBase::SMALL, TextAlignment::RIGHT });
        gfx_fill_rect_inset(
            dpi, { coords + ScreenCoordsXY{ 70, 5 }, { graphTopLeft.x + 482, coords.y + 5 } }, w->colours[2],
            INSET_RECT_FLAG_BORDER_INSET);
        coords.y += 39;
    }

    // X axis labels and values
    coords = graphTopLeft + ScreenCoordsXY{ 98, 17 };
    Graph::Draw(dpi, gCashHistory, 64, coords, yAxisScale, 128);
}

#pragma endregion

#pragma region Value graph page

/**
 *
 *  rct2: 0x0069D338
 */
static void WindowFinancesParkValueGraphMouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    if (widgetIndex == WIDX_CLOSE)
        window_close(w);
    else if (widgetIndex >= WIDX_TAB_1 && widgetIndex <= WIDX_TAB_6)
        WindowFinancesSetPage(w, widgetIndex - WIDX_TAB_1);
}

/**
 *
 *  rct2: 0x0069D353
 */
static void WindowFinancesParkValueGraphUpdate(rct_window* w)
{
    // Tab animation
    if (++w->frame_no >= WindowFinancesTabAnimationLoops[w->page])
        w->frame_no = 0;

    w->Invalidate();
}

/**
 *
 *  rct2: 0x0069CFC0
 */
static void WindowFinancesParkValueGraphInvalidate(rct_window* w)
{
    if (w->widgets != _windowFinancesPageWidgets[WINDOW_FINANCES_PAGE_VALUE_GRAPH])
    {
        w->widgets = _windowFinancesPageWidgets[WINDOW_FINANCES_PAGE_VALUE_GRAPH];
        WindowInitScrollWidgets(w);
    }

    WindowFinancesSetPressedTab(w);
}

/**
 *
 *  rct2: 0x0069CFF5
 */
static void WindowFinancesParkValueGraphPaint(rct_window* w, rct_drawpixelinfo* dpi)
{
    WindowDrawWidgets(w, dpi);
    WindowFinancesDrawTabImages(dpi, w);

    rct_widget* pageWidget = &_windowFinancesCashWidgets[WIDX_PAGE_BACKGROUND];
    auto graphTopLeft = w->windowPos + ScreenCoordsXY{ pageWidget->left + 4, pageWidget->top + 15 };
    auto graphBottomRight = w->windowPos + ScreenCoordsXY{ pageWidget->right - 4, pageWidget->bottom - 4 };

    // Park value
    auto ft = Formatter();
    ft.Add<money64>(gParkValue);
    DrawTextBasic(dpi, graphTopLeft - ScreenCoordsXY{ 0, 11 }, STR_FINANCES_PARK_VALUE, ft);

    // Graph
    gfx_fill_rect_inset(dpi, { graphTopLeft, graphBottomRight }, w->colours[1], INSET_RECT_F_30);

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
    for (axisBase = MONEY(24, 00); axisBase >= MONEY(0, 00); axisBase -= MONEY(6, 00))
    {
        auto axisValue = axisBase << yAxisScale;
        ft = Formatter();
        ft.Add<money64>(axisValue);
        DrawTextBasic(
            dpi, coords + ScreenCoordsXY{ 70, 0 }, STR_FINANCES_FINANCIAL_GRAPH_CASH_VALUE, ft,
            { FontSpriteBase::SMALL, TextAlignment::RIGHT });
        gfx_fill_rect_inset(
            dpi, { coords + ScreenCoordsXY{ 70, 5 }, { graphTopLeft.x + 482, coords.y + 5 } }, w->colours[2],
            INSET_RECT_FLAG_BORDER_INSET);
        coords.y += 39;
    }

    // X axis labels and values
    coords = graphTopLeft + ScreenCoordsXY{ 98, 17 };
    Graph::Draw(dpi, gParkValueHistory, 64, coords, yAxisScale, 0);
}

#pragma endregion

#pragma region Profit graph page

/**
 *
 *  rct2: 0x0069D715
 */
static void WindowFinancesProfitGraphMouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    if (widgetIndex == WIDX_CLOSE)
        window_close(w);
    else if (widgetIndex >= WIDX_TAB_1 && widgetIndex <= WIDX_TAB_6)
        WindowFinancesSetPage(w, widgetIndex - WIDX_TAB_1);
}

/**
 *
 *  rct2: 0x0069D730
 */
static void WindowFinancesProfitGraphUpdate(rct_window* w)
{
    // Tab animation
    if (++w->frame_no >= WindowFinancesTabAnimationLoops[w->page])
        w->frame_no = 0;
    widget_invalidate(w, WIDX_TAB_2);
}

/**
 *
 *  rct2: 0x0069D388
 */
static void WindowFinancesProfitGraphInvalidate(rct_window* w)
{
    if (w->widgets != _windowFinancesPageWidgets[WINDOW_FINANCES_PAGE_PROFIT_GRAPH])
    {
        w->widgets = _windowFinancesPageWidgets[WINDOW_FINANCES_PAGE_PROFIT_GRAPH];
        WindowInitScrollWidgets(w);
    }

    WindowFinancesSetPressedTab(w);
}

/**
 *
 *  rct2: 0x0069D3BD
 */
static void WindowFinancesProfitGraphPaint(rct_window* w, rct_drawpixelinfo* dpi)
{
    WindowDrawWidgets(w, dpi);
    WindowFinancesDrawTabImages(dpi, w);

    rct_widget* pageWidget = &_windowFinancesCashWidgets[WIDX_PAGE_BACKGROUND];
    auto graphTopLeft = w->windowPos + ScreenCoordsXY{ pageWidget->left + 4, pageWidget->top + 15 };
    auto graphBottomRight = w->windowPos + ScreenCoordsXY{ pageWidget->right - 4, pageWidget->bottom - 4 };

    // Weekly profit
    auto ft = Formatter();
    ft.Add<money64>(gCurrentProfit);
    DrawTextBasic(
        dpi, graphTopLeft - ScreenCoordsXY{ 0, 11 },
        gCurrentProfit >= 0 ? STR_FINANCES_WEEKLY_PROFIT_POSITIVE : STR_FINANCES_WEEKLY_PROFIT_LOSS, ft);

    // Graph
    gfx_fill_rect_inset(dpi, { graphTopLeft, graphBottomRight }, w->colours[1], INSET_RECT_F_30);

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
    for (axisBase = MONEY(12, 00); axisBase >= MONEY(-12, 00); axisBase -= MONEY(6, 00))
    {
        money64 axisValue = axisBase << yAxisScale;
        ft = Formatter();
        ft.Add<money64>(axisValue);
        DrawTextBasic(
            dpi, screenPos + ScreenCoordsXY{ 70, 0 }, STR_FINANCES_FINANCIAL_GRAPH_CASH_VALUE, ft,
            { FontSpriteBase::SMALL, TextAlignment::RIGHT });
        gfx_fill_rect_inset(
            dpi, { screenPos + ScreenCoordsXY{ 70, 5 }, { graphTopLeft.x + 482, screenPos.y + 5 } }, w->colours[2],
            INSET_RECT_FLAG_BORDER_INSET);
        screenPos.y += 39;
    }

    // X axis labels and values
    screenPos = graphTopLeft + ScreenCoordsXY{ 98, 17 };
    Graph::Draw(dpi, gWeeklyProfitHistory, 64, screenPos, yAxisScale, 128);
}

#pragma endregion

#pragma region Marketing page

/**
 *
 *  rct2: 0x0069D9F9
 */
static void WindowFinancesMarketingMouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    if (widgetIndex == WIDX_CLOSE)
        window_close(w);
    else if (widgetIndex >= WIDX_TAB_1 && widgetIndex <= WIDX_TAB_6)
        WindowFinancesSetPage(w, widgetIndex - WIDX_TAB_1);
    else if (widgetIndex >= WIDX_CAMPAIGN_1 && widgetIndex <= WIDX_CAMPAIGN_6)
        context_open_detail_window(WD_NEW_CAMPAIGN, widgetIndex - WIDX_CAMPAIGN_1);
}

/**
 *
 *  rct2: 0x0069DA2F
 */
static void WindowFinancesMarketingUpdate(rct_window* w)
{
    // Tab animation
    if (++w->frame_no >= WindowFinancesTabAnimationLoops[w->page])
        w->frame_no = 0;
    widget_invalidate(w, WIDX_TAB_5);
}

/**
 *
 *  rct2: 0x0069D765
 */
static void WindowFinancesMarketingInvalidate(rct_window* w)
{
    if (w->widgets != _windowFinancesPageWidgets[WINDOW_FINANCES_PAGE_MARKETING])
    {
        w->widgets = _windowFinancesPageWidgets[WINDOW_FINANCES_PAGE_MARKETING];
        WindowInitScrollWidgets(w);
    }

    WindowFinancesSetPressedTab(w);

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
        auto campaign = marketing_get_campaign(i);
        if (campaign == nullptr && marketing_is_campaign_type_applicable(i))
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

/**
 *
 *  rct2: 0x0069D834
 */
static void WindowFinancesMarketingPaint(rct_window* w, rct_drawpixelinfo* dpi)
{
    WindowDrawWidgets(w, dpi);
    WindowFinancesDrawTabImages(dpi, w);

    auto screenCoords = w->windowPos + ScreenCoordsXY{ 8, 62 };
    int32_t noCampaignsActive = 1;
    for (int32_t i = 0; i < ADVERTISING_CAMPAIGN_COUNT; i++)
    {
        auto campaign = marketing_get_campaign(i);
        if (campaign == nullptr)
            continue;

        noCampaignsActive = 0;
        auto ft = Formatter();

        // Set special parameters
        switch (i)
        {
            case ADVERTISING_CAMPAIGN_RIDE_FREE:
            case ADVERTISING_CAMPAIGN_RIDE:
            {
                auto ride = get_ride(campaign->RideId);
                if (ride != nullptr)
                {
                    ride->FormatNameTo(ft);
                }
                else
                {
                    ft.Add<rct_string_id>(STR_NONE);
                }
                break;
            }
            case ADVERTISING_CAMPAIGN_FOOD_OR_DRINK_FREE:
                ft.Add<rct_string_id>(GetShopItemDescriptor(campaign->ShopItemType).Naming.Plural);
                break;
            default:
            {
                auto& park = OpenRCT2::GetContext()->GetGameState()->GetPark();
                auto parkName = park.Name.c_str();
                ft.Add<rct_string_id>(STR_STRING);
                ft.Add<const char*>(parkName);
            }
        }

        // Advertisement
        DrawTextEllipsised(dpi, screenCoords + ScreenCoordsXY{ 4, 0 }, 296, MarketingCampaignNames[i][1], ft);

        // Duration
        uint16_t weeksRemaining = campaign->WeeksLeft;
        ft = Formatter();
        ft.Add<uint16_t>(weeksRemaining);
        DrawTextBasic(
            dpi, screenCoords + ScreenCoordsXY{ 304, 0 }, weeksRemaining == 1 ? STR_1_WEEK_REMAINING : STR_X_WEEKS_REMAINING,
            ft);

        screenCoords.y += LIST_ROW_HEIGHT;
    }

    if (noCampaignsActive)
    {
        DrawTextBasic(dpi, screenCoords + ScreenCoordsXY{ 4, 0 }, STR_MARKETING_CAMPAIGNS_NONE);
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
            DrawTextBasic(dpi, screenCoords + ScreenCoordsXY{ 4, 0 }, MarketingCampaignNames[i][0]);
            auto ft = Formatter();
            ft.Add<money64>(AdvertisingCampaignPricePerWeek[i]);
            DrawTextBasic(dpi, screenCoords + ScreenCoordsXY{ WH_SUMMARY, 0 }, STR_MARKETING_PER_WEEK, ft);

            screenCoords.y += BUTTON_FACE_HEIGHT + 2;
        }
    }
}

#pragma endregion

#pragma region Research page

/**
 *
 *  rct2: 0x0069DB3F
 */
static void WindowFinancesResearchMouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex)
    {
        case WIDX_CLOSE:
            window_close(w);
            break;
        case WIDX_TAB_1:
        case WIDX_TAB_2:
        case WIDX_TAB_3:
        case WIDX_TAB_4:
        case WIDX_TAB_5:
        case WIDX_TAB_6:
            WindowFinancesSetPage(w, widgetIndex - WIDX_TAB_1);
            break;
        case WIDX_TRANSPORT_RIDES:
        case WIDX_GENTLE_RIDES:
        case WIDX_ROLLER_COASTERS:
        case WIDX_THRILL_RIDES:
        case WIDX_WATER_RIDES:
        case WIDX_SHOPS_AND_STALLS:
        case WIDX_SCENERY_AND_THEMING:
        {
            auto activeResearchTypes = gResearchPriorities;
            activeResearchTypes ^= 1ULL << (widgetIndex - WIDX_TRANSPORT_RIDES);

            auto gameAction = ParkSetResearchFundingAction(activeResearchTypes, gResearchFundingLevel);
            GameActions::Execute(&gameAction);
            break;
        }
    }
}

/**
 *
 *  rct2: 0x0069DB66
 */
static void WindowFinancesResearchMousedown(rct_window* w, rct_widgetindex widgetIndex, rct_widget* widget)
{
    rct_widget* dropdownWidget;
    int32_t i;

    if (widgetIndex != WIDX_RESEARCH_FUNDING_DROPDOWN_BUTTON)
        return;

    dropdownWidget = widget - 1;

    for (i = 0; i < 4; i++)
    {
        gDropdownItems[i].Format = STR_DROPDOWN_MENU_LABEL;
        gDropdownItems[i].Args = ResearchFundingLevelNames[i];
    }
    WindowDropdownShowTextCustomWidth(
        { w->windowPos.x + dropdownWidget->left, w->windowPos.y + dropdownWidget->top }, dropdownWidget->height() + 1,
        w->colours[1], 0, Dropdown::Flag::StayOpen, 4, dropdownWidget->width() - 3);

    int32_t currentResearchLevel = gResearchFundingLevel;
    Dropdown::SetChecked(currentResearchLevel, true);
}

/**
 *
 *  rct2: 0x0069DB6D
 */
static void WindowFinancesResearchDropdown(rct_window* w, rct_widgetindex widgetIndex, int32_t dropdownIndex)
{
    if (widgetIndex != WIDX_RESEARCH_FUNDING_DROPDOWN_BUTTON || dropdownIndex == -1)
        return;

    auto gameAction = ParkSetResearchFundingAction(gResearchPriorities, dropdownIndex);
    GameActions::Execute(&gameAction);
}

/**
 *
 *  rct2: 0x0069DC23
 */
static void WindowFinancesResearchUpdate(rct_window* w)
{
    // Tab animation
    if (++w->frame_no >= WindowFinancesTabAnimationLoops[w->page])
        w->frame_no = 0;
    widget_invalidate(w, WIDX_TAB_6);
}

/**
 *
 *  rct2: 0x0069DA64
 */
static void WindowFinancesResearchInvalidate(rct_window* w)
{
    if (w->widgets != _windowFinancesPageWidgets[WINDOW_FINANCES_PAGE_RESEARCH])
    {
        w->widgets = _windowFinancesPageWidgets[WINDOW_FINANCES_PAGE_RESEARCH];
        WindowInitScrollWidgets(w);
    }

    WindowFinancesSetPressedTab(w);
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
            w->disabled_widgets &= ~widgetMask;

            // Set checkbox ticked if research type is active
            if (activeResearchTypes & mask)
                w->pressed_widgets |= widgetMask;
            else
                w->pressed_widgets &= ~widgetMask;
        }
        else
        {
            w->disabled_widgets |= widgetMask;
            w->pressed_widgets &= ~widgetMask;
        }
    }
}

/**
 *
 *  rct2: 0x0069DAF0
 */
static void WindowFinancesResearchPaint(rct_window* w, rct_drawpixelinfo* dpi)
{
    WindowDrawWidgets(w, dpi);
    WindowFinancesDrawTabImages(dpi, w);

    WindowResearchFundingPagePaint(w, dpi, WIDX_RESEARCH_FUNDING);
}

#pragma endregion

#pragma region Common

/**
 *
 *  rct2: 0x0069CAC5
 */
static void WindowFinancesSetPage(rct_window* w, int32_t page)
{
    w->page = page;
    w->frame_no = 0;
    w->RemoveViewport();

    w->enabled_widgets = WindowFinancesPageEnabledWidgets[page];
    w->hold_down_widgets = WindowFinancesPageHoldDownWidgets[page];
    w->event_handlers = _windowFinancesPageEvents[page];
    w->widgets = _windowFinancesPageWidgets[page];
    w->disabled_widgets = 0;
    w->pressed_widgets = 0;

    w->Invalidate();
    if (w->page == WINDOW_FINANCES_PAGE_RESEARCH)
    {
        w->width = WW_RESEARCH;
        w->height = WH_RESEARCH;
    }
    else if (w->page == WINDOW_FINANCES_PAGE_SUMMARY)
    {
        w->width = WW_OTHER_TABS;
        w->height = WH_SUMMARY;
    }
    else
    {
        w->width = WW_OTHER_TABS;
        w->height = WH_OTHER_TABS;
    }
    window_event_resize_call(w);
    window_event_invalidate_call(w);

    WindowInitScrollWidgets(w);
    w->Invalidate();

    // Scroll summary all the way to the right, initially.
    if (w->page == WINDOW_FINANCES_PAGE_SUMMARY)
        WindowFinancesSummaryInvertscroll(w);
}

static void WindowFinancesSetPressedTab(rct_window* w)
{
    int32_t i;
    for (i = 0; i < WINDOW_FINANCES_PAGE_COUNT; i++)
        w->pressed_widgets &= ~(1ULL << (WIDX_TAB_1 + i));
    w->pressed_widgets |= 1LL << (WIDX_TAB_1 + w->page);
}

static void WindowFinancesDrawTabImage(rct_drawpixelinfo* dpi, rct_window* w, int32_t page, int32_t spriteIndex)
{
    rct_widgetindex widgetIndex = WIDX_TAB_1 + page;

    if (!(w->disabled_widgets & (1LL << widgetIndex)))
    {
        if (w->page == page)
        {
            int32_t frame = w->frame_no / 2;
            if (page == WINDOW_FINANCES_PAGE_SUMMARY)
                frame %= 8;
            spriteIndex += frame;
        }

        gfx_draw_sprite(
            dpi, ImageId(spriteIndex),
            w->windowPos + ScreenCoordsXY{ w->widgets[widgetIndex].left, w->widgets[widgetIndex].top });
    }
}

static void WindowFinancesDrawTabImages(rct_drawpixelinfo* dpi, rct_window* w)
{
    WindowFinancesDrawTabImage(dpi, w, WINDOW_FINANCES_PAGE_SUMMARY, SPR_TAB_FINANCES_SUMMARY_0);
    WindowFinancesDrawTabImage(dpi, w, WINDOW_FINANCES_PAGE_FINANCIAL_GRAPH, SPR_TAB_FINANCES_FINANCIAL_GRAPH_0);
    WindowFinancesDrawTabImage(dpi, w, WINDOW_FINANCES_PAGE_VALUE_GRAPH, SPR_TAB_FINANCES_VALUE_GRAPH_0);
    WindowFinancesDrawTabImage(dpi, w, WINDOW_FINANCES_PAGE_PROFIT_GRAPH, SPR_TAB_FINANCES_PROFIT_GRAPH_0);
    WindowFinancesDrawTabImage(dpi, w, WINDOW_FINANCES_PAGE_MARKETING, SPR_TAB_FINANCES_MARKETING_0);
    WindowFinancesDrawTabImage(dpi, w, WINDOW_FINANCES_PAGE_RESEARCH, SPR_TAB_FINANCES_RESEARCH_0);
}

#pragma endregion
