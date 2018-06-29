/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <openrct2/actions/ParkSetLoanAction.hpp>
#include <openrct2/actions/ParkSetResearchFundingAction.hpp>
#include <openrct2/config/Config.h>
#include <openrct2/core/Math.hpp>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/Context.h>
#include <openrct2/Game.h>
#include <openrct2-ui/interface/Graph.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2/localisation/Date.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/ride/RideData.h>
#include <openrct2/ride/ShopItem.h>
#include <openrct2/sprites.h>
#include <openrct2-ui/interface/Dropdown.h>
#include <openrct2/management/Finance.h>
#include <openrct2/scenario/Scenario.h>
#include <openrct2/world/Park.h>

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
    { WWT_TAB,      1,  3,      33,     17, 43,     IMAGE_TYPE_REMAP | SPR_TAB, STR_FINANCES_SHOW_SUMMARY_TAB_TIP       }, \
    { WWT_TAB,      1,  34,     64,     17, 43,     IMAGE_TYPE_REMAP | SPR_TAB, STR_FINANCES_SHOW_CASH_TAB_TIP          }, \
    { WWT_TAB,      1,  65,     95,     17, 43,     IMAGE_TYPE_REMAP | SPR_TAB, STR_FINANCES_SHOW_PARK_VALUE_TAB_TIP    }, \
    { WWT_TAB,      1,  96,     126,    17, 43,     IMAGE_TYPE_REMAP | SPR_TAB, STR_FINANCES_SHOW_WEEKLY_PROFIT_TAB_TIP }, \
    { WWT_TAB,      1,  127,    157,    17, 43,     IMAGE_TYPE_REMAP | SPR_TAB, STR_FINANCES_SHOW_MARKETING_TAB_TIP     }, \
    { WWT_TAB,      1,  158,    188,    17, 43,     IMAGE_TYPE_REMAP | SPR_TAB, STR_FINANCES_RESEARCH_TIP               }

static rct_widget _windowFinancesSummaryWidgets[] =
{
    { WWT_FRAME,            0,  0,      529,    0,  309,    0xFFFFFFFF,                 STR_NONE                                }, \
    { WWT_CAPTION,          0,  1,      528,    1,  14,     STR_FINANCIAL_SUMMARY,      STR_WINDOW_TITLE_TIP                    }, \
    { WWT_CLOSEBOX,         0,  517,    527,    2,  13,     STR_CLOSE_X,                STR_CLOSE_WINDOW_TIP                    }, \
    { WWT_RESIZE,           1,  0,      529,    43, 309,    0xFFFFFFFF,                 STR_NONE                                }, \
    TAB_WIDGETS,
    { WWT_SCROLL,           1,  130,    520,     50,    260,    SCROLL_HORIZONTAL,                  STR_NONE },
      SPINNER_WIDGETS      (1,  64,     160,    279,    290,    STR_FINANCES_SUMMARY_LOAN_VALUE,    STR_NONE), // NB: 3 widgets.
    { WIDGETS_END },
};

static rct_widget _windowFinancesCashWidgets[] =
{
    { WWT_FRAME,            0,  0,      529,    0,      256,    0xFFFFFFFF,             STR_NONE },
    { WWT_CAPTION,          0,  1,      528,    1,      14,     STR_FINANCIAL_GRAPH,    STR_WINDOW_TITLE_TIP },
    { WWT_CLOSEBOX,         0,  517,    527,    2,      13,     STR_CLOSE_X,            STR_CLOSE_WINDOW_TIP },
    { WWT_RESIZE,           1,  0,      529,    43,     256,    0xFFFFFFFF,             STR_NONE },
    TAB_WIDGETS,
    { WIDGETS_END },
};

static rct_widget _windowFinancesParkValueWidgets[] =
{
    { WWT_FRAME,            0,  0,      529,    0,      256,    0xFFFFFFFF,             STR_NONE },
    { WWT_CAPTION,          0,  1,      528,    1,      14,     STR_PARK_VALUE_GRAPH,   STR_WINDOW_TITLE_TIP },
    { WWT_CLOSEBOX,         0,  517,    527,    2,      13,     STR_CLOSE_X,            STR_CLOSE_WINDOW_TIP },
    { WWT_RESIZE,           1,  0,      529,    43,     256,    0xFFFFFFFF,             STR_NONE },
    TAB_WIDGETS,
    { WIDGETS_END },
};

static rct_widget _windowFinancesProfitWidgets[] =
{
    { WWT_FRAME,            0,  0,      529,    0,      256,    0xFFFFFFFF,             STR_NONE },
    { WWT_CAPTION,          0,  1,      528,    1,      14,     STR_PROFIT_GRAPH,       STR_WINDOW_TITLE_TIP },
    { WWT_CLOSEBOX,         0,  517,    527,    2,      13,     STR_CLOSE_X,            STR_CLOSE_WINDOW_TIP },
    { WWT_RESIZE,           1,  0,      529,    43,     256,    0xFFFFFFFF,             STR_NONE },
    TAB_WIDGETS,
    { WIDGETS_END },
};

static rct_widget _windowFinancesMarketingWidgets[] =
{
    { WWT_FRAME,            0,  0,      529,    0,      256,    0xFFFFFFFF,                             STR_NONE },
    { WWT_CAPTION,          0,  1,      528,    1,      14,     STR_MARKETING,                          STR_WINDOW_TITLE_TIP },
    { WWT_CLOSEBOX,         0,  517,    527,    2,      13,     STR_CLOSE_X,                            STR_CLOSE_WINDOW_TIP },
    { WWT_RESIZE,           1,  0,      529,    43,     256,    0xFFFFFFFF,                             STR_NONE },
    TAB_WIDGETS,
    { WWT_GROUPBOX,         2,  3,      526,    47,     91,     STR_MARKETING_CAMPAIGNS_IN_OPERATION,   STR_NONE },
    { WWT_GROUPBOX,         2,  3,      526,    47,     252,    STR_MARKETING_CAMPAIGNS_AVAILABLE,      STR_NONE },
    { WWT_IMGBTN,           1,  8,      521,    0,      13,     0xFFFFFFFF,                             STR_START_THIS_MARKETING_CAMPAIGN },
    { WWT_IMGBTN,           1,  8,      521,    0,      13,     0xFFFFFFFF,                             STR_START_THIS_MARKETING_CAMPAIGN },
    { WWT_IMGBTN,           1,  8,      521,    0,      13,     0xFFFFFFFF,                             STR_START_THIS_MARKETING_CAMPAIGN },
    { WWT_IMGBTN,           1,  8,      521,    0,      13,     0xFFFFFFFF,                             STR_START_THIS_MARKETING_CAMPAIGN },
    { WWT_IMGBTN,           1,  8,      521,    0,      13,     0xFFFFFFFF,                             STR_START_THIS_MARKETING_CAMPAIGN },
    { WWT_IMGBTN,           1,  8,      521,    0,      13,     0xFFFFFFFF,                             STR_START_THIS_MARKETING_CAMPAIGN },
    { WIDGETS_END },
};

static rct_widget _windowFinancesResearchWidgets[] =
{
    { WWT_FRAME,            0,  0,      319,    0,      206,    0xFFFFFFFF,                             STR_NONE },
    { WWT_CAPTION,          0,  1,      318,    1,      14,     STR_RESEARCH_FUNDING,                   STR_WINDOW_TITLE_TIP },
    { WWT_CLOSEBOX,         0,  307,    317,    2,      13,     STR_CLOSE_X,                            STR_CLOSE_WINDOW_TIP },
    { WWT_RESIZE,           1,  0,      319,    43,     206,    0xFFFFFFFF,                             STR_NONE },
    TAB_WIDGETS,
    { WWT_GROUPBOX,         2,  3,      316,    47,     91,     STR_RESEARCH_FUNDING_,                  STR_NONE },
    { WWT_DROPDOWN,         2,  8,      167,    59,     70,     0xFFFFFFFF,                             STR_SELECT_LEVEL_OF_RESEARCH_AND_DEVELOPMENT },
    { WWT_BUTTON,           2,  156,    166,    60,     69,     STR_DROPDOWN_GLYPH,                     STR_SELECT_LEVEL_OF_RESEARCH_AND_DEVELOPMENT },
    { WWT_GROUPBOX,         2,  3,      316,    96,     202,    STR_RESEARCH_PRIORITIES,                STR_NONE },
    { WWT_CHECKBOX,         2,  8,      311,    108,    119,    STR_RESEARCH_NEW_TRANSPORT_RIDES,       STR_RESEARCH_NEW_TRANSPORT_RIDES_TIP },
    { WWT_CHECKBOX,         2,  8,      311,    121,    132,    STR_RESEARCH_NEW_GENTLE_RIDES,          STR_RESEARCH_NEW_GENTLE_RIDES_TIP },
    { WWT_CHECKBOX,         2,  8,      311,    134,    145,    STR_RESEARCH_NEW_ROLLER_COASTERS,       STR_RESEARCH_NEW_ROLLER_COASTERS_TIP },
    { WWT_CHECKBOX,         2,  8,      311,    147,    158,    STR_RESEARCH_NEW_THRILL_RIDES,          STR_RESEARCH_NEW_THRILL_RIDES_TIP },
    { WWT_CHECKBOX,         2,  8,      311,    160,    171,    STR_RESEARCH_NEW_WATER_RIDES,           STR_RESEARCH_NEW_WATER_RIDES_TIP },
    { WWT_CHECKBOX,         2,  8,      311,    173,    184,    STR_RESEARCH_NEW_SHOPS_AND_STALLS,      STR_RESEARCH_NEW_SHOPS_AND_STALLS_TIP },
    { WWT_CHECKBOX,         2,  8,      311,    186,    197,    STR_RESEARCH_NEW_SCENERY_AND_THEMING,   STR_RESEARCH_NEW_SCENERY_AND_THEMING_TIP },
    { WIDGETS_END },
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

static void window_finances_summary_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_finances_summary_mousedown(rct_window *w, rct_widgetindex widgetIndex, rct_widget* widget);
static void window_finances_summary_scrollgetsize(rct_window *w, int32_t scrollIndex, int32_t *width, int32_t *height);
static void window_finances_summary_invertscroll(rct_window *w);
static void window_finances_summary_update(rct_window *w);
static void window_finances_summary_invalidate(rct_window *w);
static void window_finances_summary_paint(rct_window *w, rct_drawpixelinfo *dpi);
static void window_finances_summary_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, int32_t scrollIndex);

static void window_finances_financial_graph_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_finances_financial_graph_update(rct_window *w);
static void window_finances_financial_graph_invalidate(rct_window *w);
static void window_finances_financial_graph_paint(rct_window *w, rct_drawpixelinfo *dpi);

static void window_finances_park_value_graph_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_finances_park_value_graph_update(rct_window *w);
static void window_finances_park_value_graph_invalidate(rct_window *w);
static void window_finances_park_value_graph_paint(rct_window *w, rct_drawpixelinfo *dpi);

static void window_finances_profit_graph_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_finances_profit_graph_update(rct_window *w);
static void window_finances_profit_graph_invalidate(rct_window *w);
static void window_finances_profit_graph_paint(rct_window *w, rct_drawpixelinfo *dpi);

static void window_finances_marketing_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_finances_marketing_update(rct_window *w);
static void window_finances_marketing_invalidate(rct_window *w);
static void window_finances_marketing_paint(rct_window *w, rct_drawpixelinfo *dpi);

static void window_finances_research_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_finances_research_mousedown(rct_window *w, rct_widgetindex widgetIndex, rct_widget* widget);
static void window_finances_research_dropdown(rct_window *w, rct_widgetindex widgetIndex, int32_t dropdownIndex);
static void window_finances_research_update(rct_window *w);
static void window_finances_research_invalidate(rct_window *w);
static void window_finances_research_paint(rct_window *w, rct_drawpixelinfo *dpi);

// 0x00988EB8
static rct_window_event_list _windowFinancesSummaryEvents =
{
    nullptr,
    window_finances_summary_mouseup,
    nullptr,
    window_finances_summary_mousedown,
    nullptr,
    nullptr,
    window_finances_summary_update,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    window_finances_summary_scrollgetsize,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    window_finances_summary_invalidate,
    window_finances_summary_paint,
    window_finances_summary_scrollpaint
};

// 0x00988F28
static rct_window_event_list _windowFinancesFinancialGraphEvents =
{
    nullptr,
    window_finances_financial_graph_mouseup,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    window_finances_financial_graph_update,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    window_finances_financial_graph_invalidate,
    window_finances_financial_graph_paint,
    nullptr
};

// 0x00988F98
static rct_window_event_list _windowFinancesValueGraphEvents =
{
    nullptr,
    window_finances_park_value_graph_mouseup,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    window_finances_park_value_graph_update,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    window_finances_park_value_graph_invalidate,
    window_finances_park_value_graph_paint,
    nullptr
};

// 0x00989008
static rct_window_event_list _windowFinancesProfitGraphEvents =
{
    nullptr,
    window_finances_profit_graph_mouseup,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    window_finances_profit_graph_update,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    window_finances_profit_graph_invalidate,
    window_finances_profit_graph_paint,
    nullptr
};

// 0x00989078
static rct_window_event_list _windowFinancesMarketingEvents =
{
    nullptr,
    window_finances_marketing_mouseup,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    window_finances_marketing_update,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    window_finances_marketing_invalidate,
    window_finances_marketing_paint,
    nullptr
};

// 0x009890E8
static rct_window_event_list _windowFinancesResearchEvents =
{
    nullptr,
    window_finances_research_mouseup,
    nullptr,
    window_finances_research_mousedown,
    window_finances_research_dropdown,
    nullptr,
    window_finances_research_update,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    window_finances_research_invalidate,
    window_finances_research_paint,
    nullptr
};

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

static constexpr const rct_string_id window_finances_summary_row_labels[RCT_EXPENDITURE_TYPE_COUNT] = {
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

static void window_finances_set_page(rct_window *w, int32_t page);
static void window_finances_set_pressed_tab(rct_window *w);
static void window_finances_draw_tab_images(rct_drawpixelinfo *dpi, rct_window *w);

/**
 *
 *  rct2: 0x0069DDF1
 */
rct_window * window_finances_open()
{
    rct_window *w;

    w = window_bring_to_front_by_class(WC_FINANCES);
    if (w == nullptr) {
        w = window_create_auto_pos(530, 310, _windowFinancesPageEvents[0], WC_FINANCES, WF_10);
        w->number = 0;
        w->frame_no = 0;

        research_update_uncompleted_types();
    }

    w->page = WINDOW_FINANCES_PAGE_SUMMARY;
    window_invalidate(w);
    w->width = 530;
    w->height = 310;
    window_invalidate(w);

    w->widgets = _windowFinancesPageWidgets[WINDOW_FINANCES_PAGE_SUMMARY];
    w->enabled_widgets = WindowFinancesPageEnabledWidgets[WINDOW_FINANCES_PAGE_SUMMARY];
    w->hold_down_widgets = WindowFinancesPageHoldDownWidgets[WINDOW_FINANCES_PAGE_SUMMARY];
    w->event_handlers = _windowFinancesPageEvents[WINDOW_FINANCES_PAGE_SUMMARY];
    w->pressed_widgets = 0;
    w->disabled_widgets = 0;

    window_init_scroll_widgets(w);

    // Scroll summary all the way to the right, initially.
    window_finances_summary_invertscroll(w);

    return w;
}

/**
 *
 *  rct2: 0x0069DDE1
 */
rct_window * window_finances_research_open()
{
    rct_window *w;

    window_finances_open();
    w = window_find_by_class(WC_FINANCES);
    if (w != nullptr)
        window_finances_set_page(w, WINDOW_FINANCES_PAGE_RESEARCH);

    return w;
}

#pragma region Summary page

/**
 *
 *  rct2: 0x0069CA99
 */
static void window_finances_summary_mouseup(rct_window *w, rct_widgetindex widgetIndex)
{
    if (widgetIndex == WIDX_CLOSE)
        window_close(w);
    else if (widgetIndex >= WIDX_TAB_1 && widgetIndex <= WIDX_TAB_6)
        window_finances_set_page(w, widgetIndex - WIDX_TAB_1);
}

/**
 *
 *  rct2: 0x0069CAB0
 */
static void window_finances_summary_mousedown(rct_window *w, rct_widgetindex widgetIndex, rct_widget* widget)
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

static uint16_t summary_num_months_available()
{
    return std::min<uint16_t>(gDateMonthsElapsed, EXPENDITURE_TABLE_MONTH_COUNT);
}

static void window_finances_summary_scrollgetsize(rct_window *w, int32_t scrollIndex, int32_t *width, int32_t *height)
{
    *width = EXPENDITURE_COLUMN_WIDTH * (summary_num_months_available() + 1);
}

static void window_finances_summary_invertscroll(rct_window *w)
{
    rct_widget summary = w->widgets[WIDX_SUMMARY_SCROLL];
    w->scrolls[0].h_left = std::max(0, w->scrolls[0].h_right - ((summary.right - summary.left) - 2));
    widget_scroll_update_thumbs(w, WIDX_SUMMARY_SCROLL);
}

/**
 *
 *  rct2: 0x0069CBA6
 */
static void window_finances_summary_update(rct_window *w)
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
static void window_finances_summary_invalidate(rct_window *w)
{
    if (w->widgets != _windowFinancesPageWidgets[WINDOW_FINANCES_PAGE_SUMMARY]) {
        w->widgets = _windowFinancesPageWidgets[WINDOW_FINANCES_PAGE_SUMMARY];
        window_init_scroll_widgets(w);
    }

    // Keep up with new months being added in the first two years.
    if (gDateMonthsElapsed != _lastPaintedMonth)
        window_finances_summary_invertscroll(w);

    window_finances_set_pressed_tab(w);
    set_format_arg(6, money32, gBankLoan);
}

/**
 *
 *  rct2: 0x0069C771
 */
static void window_finances_summary_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
    window_draw_widgets(w, dpi);
    window_finances_draw_tab_images(dpi, w);

    int32_t x = w->x + 8;
    int32_t y = w->y + 51;

    // Expenditure / Income heading
    draw_string_left_underline(dpi, STR_FINANCES_SUMMARY_EXPENDITURE_INCOME, nullptr, COLOUR_BLACK, x, y);
    y += 14;

    // Expenditure / Income row labels
    for (int32_t i = 0; i < RCT_EXPENDITURE_TYPE_COUNT; i++)
    {
        // Darken every even row
        if (i % 2 == 0)
            gfx_fill_rect(dpi, x, y - 1, x + 121, y + (TABLE_CELL_HEIGHT - 2), ColourMapA[w->colours[1]].lighter | 0x1000000);

        gfx_draw_string_left(dpi, window_finances_summary_row_labels[i], nullptr, COLOUR_BLACK, x, y - 1);
        y += TABLE_CELL_HEIGHT;
    }

    // Horizontal rule below expenditure / income table
    gfx_fill_rect_inset(dpi, w->x + 8, w->y + 272, w->x + 8 + 513, w->y + 272 + 1, w->colours[1], INSET_RECT_FLAG_BORDER_INSET);

    // Loan and interest rate
    gfx_draw_string_left(dpi, STR_FINANCES_SUMMARY_LOAN, nullptr, COLOUR_BLACK, w->x + 8, w->y + 279);
    set_format_arg(0, uint16_t, gBankLoanInterestRate);
    gfx_draw_string_left(dpi, STR_FINANCES_SUMMARY_AT_X_PER_YEAR, gCommonFormatArgs, COLOUR_BLACK, w->x + 167, w->y + 279);

    // Current cash
    rct_string_id stringId = gCash >= 0 ? STR_CASH_LABEL : STR_CASH_NEGATIVE_LABEL;
    gfx_draw_string_left(dpi, stringId, &gCash, COLOUR_BLACK, w->x + 8, w->y + 294);

    // Objective related financial information
    if (gScenarioObjectiveType == OBJECTIVE_MONTHLY_FOOD_INCOME) {
        money32 lastMonthProfit = finance_get_last_month_shop_profit();
        set_format_arg(0, money32, lastMonthProfit);
        gfx_draw_string_left(dpi, STR_LAST_MONTH_PROFIT_FROM_FOOD_DRINK_MERCHANDISE_SALES_LABEL, gCommonFormatArgs, COLOUR_BLACK, w->x + 280, w->y + 279);
    } else {
        // Park value and company value
        gfx_draw_string_left(dpi, STR_PARK_VALUE_LABEL, &gParkValue, COLOUR_BLACK, w->x + 280, w->y + 279);
        gfx_draw_string_left(dpi, STR_COMPANY_VALUE_LABEL, &gCompanyValue, COLOUR_BLACK, w->x + 280, w->y + 294);
    }
}

static void window_finances_summary_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, int32_t scrollIndex)
{
    int32_t x = 0;
    int32_t y = TABLE_CELL_HEIGHT + 2;

    rct_widget self = w->widgets[WIDX_SUMMARY_SCROLL];
    int32_t row_width = std::max<uint16_t>(w->scrolls[0].h_right, self.right - self.left);

    // Expenditure / Income row labels
    for (int32_t i = 0; i < RCT_EXPENDITURE_TYPE_COUNT; i++)
    {
        // Darken every even row
        if (i % 2 == 0)
            gfx_fill_rect(dpi, x, y - 1, x + row_width, y + (TABLE_CELL_HEIGHT - 2), ColourMapA[w->colours[1]].lighter | 0x1000000);

        y += TABLE_CELL_HEIGHT;
    }

    // Expenditure / Income values for each month
    int16_t currentMonthYear = gDateMonthsElapsed;
    for (int32_t i = summary_num_months_available(); i >= 0; i--)
    {
        y = 0;

        int16_t monthyear = currentMonthYear - i;
        if (monthyear < 0)
            continue;

        // Month heading
        set_format_arg(0, rct_string_id, STR_FINANCES_SUMMARY_MONTH_HEADING);
        set_format_arg(2, uint16_t, monthyear);
        draw_string_right_underline(
            dpi,
            monthyear == currentMonthYear ? STR_WINDOW_COLOUR_2_STRINGID : STR_BLACK_STRING,
            gCommonFormatArgs,
            COLOUR_BLACK,
            x + EXPENDITURE_COLUMN_WIDTH,
            y
        );
        y += 14;

        // Month expenditures
        money32 profit = 0;
        for (int32_t j = 0; j < RCT_EXPENDITURE_TYPE_COUNT; j++)
        {
            money32 expenditure = gExpenditureTable[i][j];
            if (expenditure != 0)
            {
                profit += expenditure;
                gfx_draw_string_right(
                    dpi,
                    expenditure >= 0 ? STR_FINANCES_SUMMARY_INCOME_VALUE : STR_FINANCES_SUMMARY_EXPENDITURE_VALUE,
                    &expenditure,
                    COLOUR_BLACK,
                    x + EXPENDITURE_COLUMN_WIDTH,
                    y
                );
            }
            y += TABLE_CELL_HEIGHT;
        }
        y += 4;

        // Month profit
        gfx_draw_string_right(
            dpi,
            profit >= 0 ? STR_FINANCES_SUMMARY_INCOME_VALUE : STR_FINANCES_SUMMARY_LOSS_VALUE,
            &profit,
            COLOUR_BLACK,
            x + EXPENDITURE_COLUMN_WIDTH,
            y
        );
        gfx_fill_rect(dpi, x + 10, y - 2, x + EXPENDITURE_COLUMN_WIDTH, y - 2, PALETTE_INDEX_10);

        x += EXPENDITURE_COLUMN_WIDTH;
    }

    _lastPaintedMonth = currentMonthYear;
}

#pragma endregion

#pragma region Financial graph page

/**
 *
 *  rct2: 0x0069CF70
 */
static void window_finances_financial_graph_mouseup(rct_window *w, rct_widgetindex widgetIndex)
{
    if (widgetIndex == WIDX_CLOSE)
        window_close(w);
    else if (widgetIndex >= WIDX_TAB_1 && widgetIndex <= WIDX_TAB_6)
        window_finances_set_page(w, widgetIndex - WIDX_TAB_1);
}

/**
 *
 *  rct2: 0x0069CF8B
 */
static void window_finances_financial_graph_update(rct_window *w)
{
    // Tab animation
    if (++w->frame_no >= WindowFinancesTabAnimationLoops[w->page])
        w->frame_no = 0;
    widget_invalidate(w, WIDX_TAB_2);
}

/**
 *
 *  rct2: 0x0069CBDB
 */
static void window_finances_financial_graph_invalidate(rct_window *w)
{
    if (w->widgets != _windowFinancesPageWidgets[WINDOW_FINANCES_PAGE_FINANCIAL_GRAPH]) {
        w->widgets = _windowFinancesPageWidgets[WINDOW_FINANCES_PAGE_FINANCIAL_GRAPH];
        window_init_scroll_widgets(w);
    }

    window_finances_set_pressed_tab(w);
}

/**
 *
 *  rct2: 0x0069CC10
 */
static void window_finances_financial_graph_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
    int32_t i, x, y, graphLeft, graphTop, graphRight, graphBottom;

    window_draw_widgets(w, dpi);
    window_finances_draw_tab_images(dpi, w);

    rct_widget *pageWidget = &_windowFinancesCashWidgets[WIDX_PAGE_BACKGROUND];
    graphLeft = w->x + pageWidget->left + 4;
    graphTop = w->y + pageWidget->top + 15;
    graphRight = w->x + pageWidget->right - 4;
    graphBottom = w->y + pageWidget->bottom - 4;

    // Cash (less loan)
    money32 cashLessLoan = gCash - gBankLoan;

    gfx_draw_string_left(
        dpi,
        cashLessLoan >= 0 ?
            STR_FINANCES_FINANCIAL_GRAPH_CASH_LESS_LOAN_POSITIVE : STR_FINANCES_FINANCIAL_GRAPH_CASH_LESS_LOAN_NEGATIVE,
        &cashLessLoan,
        COLOUR_BLACK,
        graphLeft,
        graphTop - 11
    );

    // Graph
    gfx_fill_rect_inset(dpi, graphLeft, graphTop, graphRight, graphBottom, w->colours[1], INSET_RECT_F_30);

    // Calculate the Y axis scale (log2 of highest [+/-]balance)
    int32_t yAxisScale = 0;
    for (i = 0; i < 64; i++) {
        money32 balance = gCashHistory[i];
        if (balance == MONEY32_UNDEFINED)
            continue;

        // Modifier balance then keep halving until less than 127 pixels
        balance = abs(balance) >> yAxisScale;
        while (balance > 127) {
            balance /= 2;
            yAxisScale++;
        }
    }

    // Y axis labels
    x = graphLeft + 18;
    y = graphTop + 14;
    money32 axisBase;
    for (axisBase = MONEY(12,00); axisBase >= MONEY(-12,00); axisBase -= MONEY(6,00)) {
        money32 axisValue = axisBase << yAxisScale;
        gfx_draw_string_right(dpi, STR_FINANCES_FINANCIAL_GRAPH_CASH_VALUE, &axisValue, COLOUR_BLACK, x + 70, y);
        y += 39;
    }

    // X axis labels and values
    x = graphLeft + 98;
    y = graphTop + 17;
    graph_draw_money32(dpi, gCashHistory, 64, x, y, yAxisScale, 128);
}

#pragma endregion

#pragma region Value graph page

/**
 *
 *  rct2: 0x0069D338
 */
static void window_finances_park_value_graph_mouseup(rct_window *w, rct_widgetindex widgetIndex)
{
    if (widgetIndex == WIDX_CLOSE)
        window_close(w);
    else if (widgetIndex >= WIDX_TAB_1 && widgetIndex <= WIDX_TAB_6)
        window_finances_set_page(w, widgetIndex - WIDX_TAB_1);
}

/**
 *
 *  rct2: 0x0069D353
 */
static void window_finances_park_value_graph_update(rct_window *w)
{
    // Tab animation
    if (++w->frame_no >= WindowFinancesTabAnimationLoops[w->page])
        w->frame_no = 0;
    widget_invalidate(w, WIDX_TAB_2);
}

/**
 *
 *  rct2: 0x0069CFC0
 */
static void window_finances_park_value_graph_invalidate(rct_window *w)
{
    if (w->widgets != _windowFinancesPageWidgets[WINDOW_FINANCES_PAGE_VALUE_GRAPH]) {
        w->widgets = _windowFinancesPageWidgets[WINDOW_FINANCES_PAGE_VALUE_GRAPH];
        window_init_scroll_widgets(w);
    }

    window_finances_set_pressed_tab(w);
}

/**
 *
 *  rct2: 0x0069CFF5
 */
static void window_finances_park_value_graph_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
    int32_t i, x, y, graphLeft, graphTop, graphRight, graphBottom;

    window_draw_widgets(w, dpi);
    window_finances_draw_tab_images(dpi, w);

    rct_widget *pageWidget = &_windowFinancesCashWidgets[WIDX_PAGE_BACKGROUND];
    graphLeft = w->x + pageWidget->left + 4;
    graphTop = w->y + pageWidget->top + 15;
    graphRight = w->x + pageWidget->right - 4;
    graphBottom = w->y + pageWidget->bottom - 4;

    // Park value
    money32 parkValue = gParkValue;
    gfx_draw_string_left(
        dpi,
        STR_FINANCES_PARK_VALUE,
        &parkValue,
        COLOUR_BLACK,
        graphLeft,
        graphTop - 11
    );

    // Graph
    gfx_fill_rect_inset(dpi, graphLeft, graphTop, graphRight, graphBottom, w->colours[1], INSET_RECT_F_30);

    // Calculate the Y axis scale (log2 of highest [+/-]balance)
    int32_t yAxisScale = 0;
    for (i = 0; i < 64; i++) {
        money32 balance = gParkValueHistory[i];
        if (balance == MONEY32_UNDEFINED)
            continue;

        // Modifier balance then keep halving until less than 255 pixels
        balance = abs(balance) >> yAxisScale;
        while (balance > 255) {
            balance /= 2;
            yAxisScale++;
        }
    }

    // Y axis labels
    x = graphLeft + 18;
    y = graphTop + 14;
    money32 axisBase;
    for (axisBase = MONEY(24,00); axisBase >= MONEY(0,00); axisBase -= MONEY(6,00)) {
        money32 axisValue = axisBase << yAxisScale;
        gfx_draw_string_right(dpi, STR_FINANCES_FINANCIAL_GRAPH_CASH_VALUE, &axisValue, COLOUR_BLACK, x + 70, y);
        y += 39;
    }

    // X axis labels and values
    x = graphLeft + 98;
    y = graphTop + 17;
    graph_draw_money32(dpi, gParkValueHistory, 64, x, y, yAxisScale, 0);
}

#pragma endregion

#pragma region Profit graph page

/**
 *
 *  rct2: 0x0069D715
 */
static void window_finances_profit_graph_mouseup(rct_window *w, rct_widgetindex widgetIndex)
{
    if (widgetIndex == WIDX_CLOSE)
        window_close(w);
    else if (widgetIndex >= WIDX_TAB_1 && widgetIndex <= WIDX_TAB_6)
        window_finances_set_page(w, widgetIndex - WIDX_TAB_1);
}

/**
 *
 *  rct2: 0x0069D730
 */
static void window_finances_profit_graph_update(rct_window *w)
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
static void window_finances_profit_graph_invalidate(rct_window *w)
{
    if (w->widgets != _windowFinancesPageWidgets[WINDOW_FINANCES_PAGE_PROFIT_GRAPH]) {
        w->widgets = _windowFinancesPageWidgets[WINDOW_FINANCES_PAGE_PROFIT_GRAPH];
        window_init_scroll_widgets(w);
    }

    window_finances_set_pressed_tab(w);
}

/**
 *
 *  rct2: 0x0069D3BD
 */
static void window_finances_profit_graph_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
    int32_t i, x, y, graphLeft, graphTop, graphRight, graphBottom;

    window_draw_widgets(w, dpi);
    window_finances_draw_tab_images(dpi, w);

    rct_widget *pageWidget = &_windowFinancesCashWidgets[WIDX_PAGE_BACKGROUND];
    graphLeft = w->x + pageWidget->left + 4;
    graphTop = w->y + pageWidget->top + 15;
    graphRight = w->x + pageWidget->right - 4;
    graphBottom = w->y + pageWidget->bottom - 4;

    // Weekly profit
    money32 weeklyPofit = gCurrentProfit;
    gfx_draw_string_left(
        dpi,
        weeklyPofit >= 0 ? STR_FINANCES_WEEKLY_PROFIT_POSITIVE : STR_FINANCES_WEEKLY_PROFIT_LOSS,
        &weeklyPofit,
        COLOUR_BLACK,
        graphLeft,
        graphTop - 11
    );

    // Graph
    gfx_fill_rect_inset(dpi, graphLeft, graphTop, graphRight, graphBottom, w->colours[1], INSET_RECT_F_30);

    // Calculate the Y axis scale (log2 of highest [+/-]balance)
    int32_t yAxisScale = 0;
    for (i = 0; i < 64; i++) {
        money32 balance = gWeeklyProfitHistory[i];
        if (balance == MONEY32_UNDEFINED)
            continue;

        // Modifier balance then keep halving until less than 127 pixels
        balance = abs(balance) >> yAxisScale;
        while (balance > 127) {
            balance /= 2;
            yAxisScale++;
        }
    }

    // Y axis labels
    x = graphLeft + 18;
    y = graphTop + 14;
    money32 axisBase;
    for (axisBase = MONEY(12,00); axisBase >= MONEY(-12,00); axisBase -= MONEY(6,00)) {
        money32 axisValue = axisBase << yAxisScale;
        gfx_draw_string_right(dpi, STR_FINANCES_FINANCIAL_GRAPH_CASH_VALUE, &axisValue, COLOUR_BLACK, x + 70, y);
        y += 39;
    }

    // X axis labels and values
    x = graphLeft + 98;
    y = graphTop + 17;
    graph_draw_money32(dpi, gWeeklyProfitHistory, 64, x, y, yAxisScale, 128);
}

#pragma endregion

#pragma region Marketing page

/**
 *
 *  rct2: 0x0069D9F9
 */
static void window_finances_marketing_mouseup(rct_window *w, rct_widgetindex widgetIndex)
{
    if (widgetIndex == WIDX_CLOSE)
        window_close(w);
    else if (widgetIndex >= WIDX_TAB_1 && widgetIndex <= WIDX_TAB_6)
        window_finances_set_page(w, widgetIndex - WIDX_TAB_1);
    else if (widgetIndex >= WIDX_CAMPAIGN_1 && widgetIndex <= WIDX_CAMPAIGN_6)
        context_open_detail_window(WD_NEW_CAMPAIGN, widgetIndex - WIDX_CAMPAIGN_1);

}

/**
 *
 *  rct2: 0x0069DA2F
 */
static void window_finances_marketing_update(rct_window *w)
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
static void window_finances_marketing_invalidate(rct_window *w)
{
    int32_t i;

    if (w->widgets != _windowFinancesPageWidgets[WINDOW_FINANCES_PAGE_MARKETING]) {
        w->widgets = _windowFinancesPageWidgets[WINDOW_FINANCES_PAGE_MARKETING];
        window_init_scroll_widgets(w);
    }

    window_finances_set_pressed_tab(w);

    // Count number of active campaigns
    int32_t numActiveCampaigns = 0;
    for (i = 0; i < ADVERTISING_CAMPAIGN_COUNT; i++)
        if (gMarketingCampaignDaysLeft[i] != 0)
            numActiveCampaigns++;

    int32_t y = std::max(1, numActiveCampaigns) * LIST_ROW_HEIGHT + 92;

    // Update group box positions
    _windowFinancesMarketingWidgets[WIDX_ACTIVE_CAMPAIGNS_GROUP].bottom = y - 22;
    _windowFinancesMarketingWidgets[WIDX_CAMPAIGNS_AVAILABLE_GROUP].top = y - 13;

    // Update new campaign button visibility
    y += 3;
    for (i = 0; i < ADVERTISING_CAMPAIGN_COUNT; i++) {
        rct_widget *campaignButton = &_windowFinancesMarketingWidgets[WIDX_CAMPAIGN_1 + i];

        campaignButton->type = WWT_EMPTY;

        if (gMarketingCampaignDaysLeft[i] != 0)
            continue;

        if (!marketing_is_campaign_type_applicable(i))
            continue;

        campaignButton->type = WWT_BUTTON;
        campaignButton->top = y;
        campaignButton->bottom = y + BUTTON_FACE_HEIGHT + 1;
        y += BUTTON_FACE_HEIGHT + 2;
    }
}

/**
 *
 *  rct2: 0x0069D834
 */
static void window_finances_marketing_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
    int32_t i, x, y, weeksRemaining;
    Ride *ride;

    window_draw_widgets(w, dpi);
    window_finances_draw_tab_images(dpi, w);

    x = w->x + 8;
    y = w->y + 62;

    int32_t noCampaignsActive = 1;
    for (i = 0; i < ADVERTISING_CAMPAIGN_COUNT; i++) {
        if (gMarketingCampaignDaysLeft[i] == 0)
            continue;

        noCampaignsActive = 0;
        set_format_arg(0, rct_string_id, gParkName);
        set_format_arg(2, uint32_t, gParkNameArgs);

        // Set special parameters
        switch (i) {
        case ADVERTISING_CAMPAIGN_RIDE_FREE:
        case ADVERTISING_CAMPAIGN_RIDE:
            ride = get_ride(gMarketingCampaignRideIndex[i]);
            set_format_arg(0, rct_string_id, ride->name);
            set_format_arg(2, uint32_t, ride->name_arguments);
            break;
        case ADVERTISING_CAMPAIGN_FOOD_OR_DRINK_FREE:
            set_format_arg(0, rct_string_id, ShopItemStringIds[gMarketingCampaignRideIndex[i]].plural);
            break;
        }

        // Advertisement
        gfx_draw_string_left_clipped(dpi, MarketingCampaignNames[i][1], gCommonFormatArgs, COLOUR_BLACK, x + 4, y, 296);

        // Duration
        weeksRemaining = (gMarketingCampaignDaysLeft[i] % 128);
        gfx_draw_string_left(dpi, weeksRemaining == 1 ? STR_1_WEEK_REMAINING : STR_X_WEEKS_REMAINING, &weeksRemaining, COLOUR_BLACK, x + 304, y);

        y += LIST_ROW_HEIGHT;
    }

    if (noCampaignsActive) {
        gfx_draw_string_left(dpi, STR_MARKETING_CAMPAIGNS_NONE, nullptr, COLOUR_BLACK, x + 4, y);
        y += LIST_ROW_HEIGHT;
    }
    y += 34;

    // Draw campaign button text
    for (i = 0; i < ADVERTISING_CAMPAIGN_COUNT; i++) {
        rct_widget *campaginButton = &_windowFinancesMarketingWidgets[WIDX_CAMPAIGN_1 + i];

        if (campaginButton->type == WWT_EMPTY)
            continue;

        money32 pricePerWeek = AdvertisingCampaignPricePerWeek[i];

        // Draw button text
        gfx_draw_string_left(dpi, MarketingCampaignNames[i][0], nullptr, COLOUR_BLACK, x + 4, y);
        gfx_draw_string_left(dpi, STR_MARKETING_PER_WEEK, &pricePerWeek, COLOUR_BLACK, x + 310, y);

        y += BUTTON_FACE_HEIGHT + 2;
    }
}

#pragma endregion

#pragma region Research page

/**
 *
 *  rct2: 0x0069DB3F
 */
static void window_finances_research_mouseup(rct_window *w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex) {
    case WIDX_CLOSE:
        window_close(w);
        break;
    case WIDX_TAB_1:
    case WIDX_TAB_2:
    case WIDX_TAB_3:
    case WIDX_TAB_4:
    case WIDX_TAB_5:
    case WIDX_TAB_6:
        window_finances_set_page(w, widgetIndex - WIDX_TAB_1);
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
static void window_finances_research_mousedown(rct_window *w, rct_widgetindex widgetIndex, rct_widget* widget)
{
    rct_widget *dropdownWidget;
    int32_t i;

    if (widgetIndex != WIDX_RESEARCH_FUNDING_DROPDOWN_BUTTON)
        return;

    dropdownWidget = widget - 1;

    for (i = 0; i < 4; i++) {
        gDropdownItemsFormat[i] = STR_DROPDOWN_MENU_LABEL;
        gDropdownItemsArgs[i] = ResearchFundingLevelNames[i];
    }
    window_dropdown_show_text_custom_width(
        w->x + dropdownWidget->left,
        w->y + dropdownWidget->top,
        dropdownWidget->bottom - dropdownWidget->top + 1,
        w->colours[1],
        0,
        DROPDOWN_FLAG_STAY_OPEN,
        4,
        dropdownWidget->right - dropdownWidget->left - 3
    );

    int32_t currentResearchLevel = gResearchFundingLevel;
    dropdown_set_checked(currentResearchLevel, true);
}

/**
 *
 *  rct2: 0x0069DB6D
 */
static void window_finances_research_dropdown(rct_window *w, rct_widgetindex widgetIndex, int32_t dropdownIndex)
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
static void window_finances_research_update(rct_window *w)
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
static void window_finances_research_invalidate(rct_window *w)
{
    if (w->widgets != _windowFinancesPageWidgets[WINDOW_FINANCES_PAGE_RESEARCH]) {
        w->widgets = _windowFinancesPageWidgets[WINDOW_FINANCES_PAGE_RESEARCH];
        window_init_scroll_widgets(w);
    }

    window_finances_set_pressed_tab(w);
    if (gResearchProgressStage == RESEARCH_STAGE_FINISHED_ALL) {
        _windowFinancesResearchWidgets[WIDX_RESEARCH_FUNDING].type = WWT_EMPTY;
        _windowFinancesResearchWidgets[WIDX_RESEARCH_FUNDING_DROPDOWN_BUTTON].type = WWT_EMPTY;
    }
    int32_t currentResearchLevel = gResearchFundingLevel;

    // Current funding
    _windowFinancesResearchWidgets[WIDX_RESEARCH_FUNDING].text = ResearchFundingLevelNames[currentResearchLevel];

    // Checkboxes
    uint8_t activeResearchTypes = gResearchPriorities;
    int32_t uncompletedResearchTypes = gResearchUncompletedCategories;
    for (int32_t i = 0; i < 7; i++) {
        int32_t mask = 1 << i;
        int32_t widgetMask = 1ULL << (i + WIDX_TRANSPORT_RIDES);

        // Set checkbox disabled if research type is complete
        if (uncompletedResearchTypes & mask) {
            w->disabled_widgets &= ~widgetMask;

            // Set checkbox ticked if research type is active
            if (activeResearchTypes & mask)
                w->pressed_widgets |= widgetMask;
            else
                w->pressed_widgets &= ~widgetMask;
        } else {
            w->disabled_widgets |= widgetMask;
            w->pressed_widgets &= ~widgetMask;
        }
    }
}

/**
 *
 *  rct2: 0x0069DAF0
 */
static void window_finances_research_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
    window_draw_widgets(w, dpi);
    window_finances_draw_tab_images(dpi, w);

    window_research_funding_page_paint(w, dpi, WIDX_RESEARCH_FUNDING);
}

#pragma endregion

#pragma region Common

/**
 *
 *  rct2: 0x0069CAC5
 */
static void window_finances_set_page(rct_window *w, int32_t page)
{
    w->page = page;
    w->frame_no = 0;
    if (w->viewport != nullptr) {
        w->viewport->width = 0;
        w->viewport = nullptr;
    }

    w->enabled_widgets = WindowFinancesPageEnabledWidgets[page];
    w->hold_down_widgets = WindowFinancesPageHoldDownWidgets[page];
    w->event_handlers = _windowFinancesPageEvents[page];
    w->widgets = _windowFinancesPageWidgets[page];
    w->disabled_widgets = 0;
    w->pressed_widgets = 0;

    window_invalidate(w);
    if (w->page == WINDOW_FINANCES_PAGE_RESEARCH) {
        w->width = 320;
        w->height = 207;
    } else if (w->page == WINDOW_FINANCES_PAGE_SUMMARY) {
        w->width = 530;
        w->height = 310;
    } else {
        w->width = 530;
        w->height = 257;
    }
    window_event_resize_call(w);
    window_event_invalidate_call(w);

    window_init_scroll_widgets(w);
    window_invalidate(w);

    // Scroll summary all the way to the right, initially.
    if (w->page == WINDOW_FINANCES_PAGE_SUMMARY)
        window_finances_summary_invertscroll(w);
}

static void window_finances_set_pressed_tab(rct_window *w)
{
    int32_t i;
    for (i = 0; i < WINDOW_FINANCES_PAGE_COUNT; i++)
        w->pressed_widgets &= ~(1ULL << (WIDX_TAB_1 + i));
    w->pressed_widgets |= 1LL << (WIDX_TAB_1 + w->page);
}

static void window_finances_draw_tab_image(rct_drawpixelinfo *dpi, rct_window *w, int32_t page, int32_t spriteIndex)
{
    rct_widgetindex widgetIndex = WIDX_TAB_1 + page;

    if (!(w->disabled_widgets & (1LL << widgetIndex))) {
        if (w->page == page) {
            int32_t frame = w->frame_no / 2;
            if (page == WINDOW_FINANCES_PAGE_SUMMARY)
                frame %= 8;
            spriteIndex += frame;
        }

        gfx_draw_sprite(dpi, spriteIndex, w->x + w->widgets[widgetIndex].left, w->y + w->widgets[widgetIndex].top, 0);
    }
}

static void window_finances_draw_tab_images(rct_drawpixelinfo *dpi, rct_window *w)
{
    window_finances_draw_tab_image(dpi, w, WINDOW_FINANCES_PAGE_SUMMARY, SPR_TAB_FINANCES_SUMMARY_0);
    window_finances_draw_tab_image(dpi, w, WINDOW_FINANCES_PAGE_FINANCIAL_GRAPH, SPR_TAB_FINANCES_FINANCIAL_GRAPH_0);
    window_finances_draw_tab_image(dpi, w, WINDOW_FINANCES_PAGE_VALUE_GRAPH, SPR_TAB_FINANCES_VALUE_GRAPH_0);
    window_finances_draw_tab_image(dpi, w, WINDOW_FINANCES_PAGE_PROFIT_GRAPH, SPR_TAB_FINANCES_PROFIT_GRAPH_0);
    window_finances_draw_tab_image(dpi, w, WINDOW_FINANCES_PAGE_MARKETING, SPR_TAB_FINANCES_MARKETING_0);
    window_finances_draw_tab_image(dpi, w, WINDOW_FINANCES_PAGE_RESEARCH, SPR_TAB_FINANCES_RESEARCH_0);
}

#pragma endregion
