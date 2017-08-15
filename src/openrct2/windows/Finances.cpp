#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#include "../config/Config.h"
#include "../core/Math.hpp"

extern "C"
{
    #include "../game.h"
    #include "../interface/graph.h"
    #include "../interface/widget.h"
    #include "../localisation/date.h"
    #include "../localisation/localisation.h"
    #include "../ride/ride_data.h"
    #include "../sprites.h"
    #include "dropdown.h"
}

enum {
    WINDOW_FINANCES_PAGE_SUMMARY,
    WINDOW_FINANCES_PAGE_FINANCIAL_GRAPH,
    WINDOW_FINANCES_PAGE_VALUE_GRAPH,
    WINDOW_FINANCES_PAGE_PROFIT_GRAPH,
    WINDOW_FINANCES_PAGE_MARKETING,
    WINDOW_FINANCES_PAGE_RESEARCH,
    WINDOW_FINANCES_PAGE_COUNT
};

enum {
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

    WIDX_LOAN = 10,
    WIDX_LOAN_INCREASE,
    WIDX_LOAN_DECREASE,

    WIDX_ACITVE_CAMPAGINS_GROUP = 10,
    WIDX_CAMPAGINS_AVAILABLE_GROUP,
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

static rct_widget window_finances_summary_widgets[] = {
    { WWT_FRAME,            0,  0,      529,    0,      256,    0xFFFFFFFF,                         STR_NONE },
    { WWT_CAPTION,          0,  1,      528,    1,      14,     STR_FINANCIAL_SUMMARY,              STR_WINDOW_TITLE_TIP },
    { WWT_CLOSEBOX,         0,  517,    527,    2,      13,     STR_CLOSE_X,                        STR_CLOSE_WINDOW_TIP },
    { WWT_RESIZE,           1,  0,      529,    43,     256,    0xFFFFFFFF,                         STR_NONE },
    { WWT_TAB,              1,  3,      33,     17,     43,     IMAGE_TYPE_REMAP | SPR_TAB,               STR_FINANCES_SHOW_SUMMARY_TAB_TIP },
    { WWT_TAB,              1,  34,     64,     17,     43,     IMAGE_TYPE_REMAP | SPR_TAB,               STR_FINANCES_SHOW_CASH_TAB_TIP },
    { WWT_TAB,              1,  65,     95,     17,     43,     IMAGE_TYPE_REMAP | SPR_TAB,               STR_FINANCES_SHOW_PARK_VALUE_TAB_TIP },
    { WWT_TAB,              1,  96,     126,    17,     43,     IMAGE_TYPE_REMAP | SPR_TAB,               STR_FINANCES_SHOW_WEEKLY_PROFIT_TAB_TIP },
    { WWT_TAB,              1,  127,    157,    17,     43,     IMAGE_TYPE_REMAP | SPR_TAB,               STR_FINANCES_SHOW_MARKETING_TAB_TIP },
    { WWT_TAB,              1,  158,    188,    17,     43,     IMAGE_TYPE_REMAP | SPR_TAB,               STR_FINANCES_RESEARCH_TIP },
    { WWT_SPINNER,          1,  64,     153,    229,    240,    STR_FINANCES_SUMMARY_LOAN_VALUE,    STR_NONE },
    { WWT_DROPDOWN_BUTTON,  1,  142,    152,    230,    234,    STR_NUMERIC_UP,                     STR_NONE },
    { WWT_DROPDOWN_BUTTON,  1,  142,    152,    235,    239,    STR_NUMERIC_DOWN,                   STR_NONE },
    { WIDGETS_END },
};

static rct_widget window_finances_cash_widgets[] = {
    { WWT_FRAME,            0,  0,      529,    0,      256,    0xFFFFFFFF,             STR_NONE },
    { WWT_CAPTION,          0,  1,      528,    1,      14,     STR_FINANCIAL_GRAPH,    STR_WINDOW_TITLE_TIP },
    { WWT_CLOSEBOX,         0,  517,    527,    2,      13,     STR_CLOSE_X,            STR_CLOSE_WINDOW_TIP },
    { WWT_RESIZE,           1,  0,      529,    43,     256,    0xFFFFFFFF,             STR_NONE },
    { WWT_TAB,              1,  3,      33,     17,     43,     IMAGE_TYPE_REMAP | SPR_TAB,   STR_FINANCES_SHOW_SUMMARY_TAB_TIP },
    { WWT_TAB,              1,  34,     64,     17,     43,     IMAGE_TYPE_REMAP | SPR_TAB,   STR_FINANCES_SHOW_CASH_TAB_TIP },
    { WWT_TAB,              1,  65,     95,     17,     43,     IMAGE_TYPE_REMAP | SPR_TAB,   STR_FINANCES_SHOW_PARK_VALUE_TAB_TIP },
    { WWT_TAB,              1,  96,     126,    17,     43,     IMAGE_TYPE_REMAP | SPR_TAB,   STR_FINANCES_SHOW_WEEKLY_PROFIT_TAB_TIP },
    { WWT_TAB,              1,  127,    157,    17,     43,     IMAGE_TYPE_REMAP | SPR_TAB,   STR_FINANCES_SHOW_MARKETING_TAB_TIP },
    { WWT_TAB,              1,  158,    188,    17,     43,     IMAGE_TYPE_REMAP | SPR_TAB,   STR_FINANCES_RESEARCH_TIP },
    { WIDGETS_END },
};

static rct_widget window_finances_park_value_widgets[] = {
    { WWT_FRAME,            0,  0,      529,    0,      256,    0xFFFFFFFF,             STR_NONE },
    { WWT_CAPTION,          0,  1,      528,    1,      14,     STR_PARK_VALUE_GRAPH,   STR_WINDOW_TITLE_TIP },
    { WWT_CLOSEBOX,         0,  517,    527,    2,      13,     STR_CLOSE_X,            STR_CLOSE_WINDOW_TIP },
    { WWT_RESIZE,           1,  0,      529,    43,     256,    0xFFFFFFFF,             STR_NONE },
    { WWT_TAB,              1,  3,      33,     17,     43,     IMAGE_TYPE_REMAP | SPR_TAB,   STR_FINANCES_SHOW_SUMMARY_TAB_TIP },
    { WWT_TAB,              1,  34,     64,     17,     43,     IMAGE_TYPE_REMAP | SPR_TAB,   STR_FINANCES_SHOW_CASH_TAB_TIP },
    { WWT_TAB,              1,  65,     95,     17,     43,     IMAGE_TYPE_REMAP | SPR_TAB,   STR_FINANCES_SHOW_PARK_VALUE_TAB_TIP },
    { WWT_TAB,              1,  96,     126,    17,     43,     IMAGE_TYPE_REMAP | SPR_TAB,   STR_FINANCES_SHOW_WEEKLY_PROFIT_TAB_TIP },
    { WWT_TAB,              1,  127,    157,    17,     43,     IMAGE_TYPE_REMAP | SPR_TAB,   STR_FINANCES_SHOW_MARKETING_TAB_TIP },
    { WWT_TAB,              1,  158,    188,    17,     43,     IMAGE_TYPE_REMAP | SPR_TAB,   STR_FINANCES_RESEARCH_TIP },
    { WIDGETS_END },
};

static rct_widget window_finances_profit_widgets[] = {
    { WWT_FRAME,            0,  0,      529,    0,      256,    0xFFFFFFFF,             STR_NONE },
    { WWT_CAPTION,          0,  1,      528,    1,      14,     STR_PROFIT_GRAPH,       STR_WINDOW_TITLE_TIP },
    { WWT_CLOSEBOX,         0,  517,    527,    2,      13,     STR_CLOSE_X,            STR_CLOSE_WINDOW_TIP },
    { WWT_RESIZE,           1,  0,      529,    43,     256,    0xFFFFFFFF,             STR_NONE },
    { WWT_TAB,              1,  3,      33,     17,     43,     IMAGE_TYPE_REMAP | SPR_TAB,   STR_FINANCES_SHOW_SUMMARY_TAB_TIP },
    { WWT_TAB,              1,  34,     64,     17,     43,     IMAGE_TYPE_REMAP | SPR_TAB,   STR_FINANCES_SHOW_CASH_TAB_TIP },
    { WWT_TAB,              1,  65,     95,     17,     43,     IMAGE_TYPE_REMAP | SPR_TAB,   STR_FINANCES_SHOW_PARK_VALUE_TAB_TIP },
    { WWT_TAB,              1,  96,     126,    17,     43,     IMAGE_TYPE_REMAP | SPR_TAB,   STR_FINANCES_SHOW_WEEKLY_PROFIT_TAB_TIP },
    { WWT_TAB,              1,  127,    157,    17,     43,     IMAGE_TYPE_REMAP | SPR_TAB,   STR_FINANCES_SHOW_MARKETING_TAB_TIP },
    { WWT_TAB,              1,  158,    188,    17,     43,     IMAGE_TYPE_REMAP | SPR_TAB,   STR_FINANCES_RESEARCH_TIP },
    { WIDGETS_END },
};

static rct_widget window_finances_marketing_widgets[] = {
    { WWT_FRAME,            0,  0,      529,    0,      256,    0xFFFFFFFF,                             STR_NONE },
    { WWT_CAPTION,          0,  1,      528,    1,      14,     STR_MARKETING,                          STR_WINDOW_TITLE_TIP },
    { WWT_CLOSEBOX,         0,  517,    527,    2,      13,     STR_CLOSE_X,                            STR_CLOSE_WINDOW_TIP },
    { WWT_RESIZE,           1,  0,      529,    43,     256,    0xFFFFFFFF,                             STR_NONE },
    { WWT_TAB,              1,  3,      33,     17,     43,     IMAGE_TYPE_REMAP | SPR_TAB,                   STR_FINANCES_SHOW_SUMMARY_TAB_TIP },
    { WWT_TAB,              1,  34,     64,     17,     43,     IMAGE_TYPE_REMAP | SPR_TAB,                   STR_FINANCES_SHOW_CASH_TAB_TIP },
    { WWT_TAB,              1,  65,     95,     17,     43,     IMAGE_TYPE_REMAP | SPR_TAB,                   STR_FINANCES_SHOW_PARK_VALUE_TAB_TIP },
    { WWT_TAB,              1,  96,     126,    17,     43,     IMAGE_TYPE_REMAP | SPR_TAB,                   STR_FINANCES_SHOW_WEEKLY_PROFIT_TAB_TIP },
    { WWT_TAB,              1,  127,    157,    17,     43,     IMAGE_TYPE_REMAP | SPR_TAB,                   STR_FINANCES_SHOW_MARKETING_TAB_TIP },
    { WWT_TAB,              1,  158,    188,    17,     43,     IMAGE_TYPE_REMAP | SPR_TAB,                   STR_FINANCES_RESEARCH_TIP },
    { WWT_GROUPBOX,         2,  3,      526,    47,     91,     STR_MARKETING_CAMPAIGNS_IN_OPERATION,   STR_NONE },
    { WWT_GROUPBOX,         2,  3,      526,    47,     252,    STR_MARKETING_CAMPAIGNS_AVAILABLE,      STR_NONE },
    { WWT_IMGBTN,           1,  8,      521,    0,      11,     0xFFFFFFFF,                             STR_START_THIS_MARKETING_CAMPAIGN },
    { WWT_IMGBTN,           1,  8,      521,    0,      11,     0xFFFFFFFF,                             STR_START_THIS_MARKETING_CAMPAIGN },
    { WWT_IMGBTN,           1,  8,      521,    0,      11,     0xFFFFFFFF,                             STR_START_THIS_MARKETING_CAMPAIGN },
    { WWT_IMGBTN,           1,  8,      521,    0,      11,     0xFFFFFFFF,                             STR_START_THIS_MARKETING_CAMPAIGN },
    { WWT_IMGBTN,           1,  8,      521,    0,      11,     0xFFFFFFFF,                             STR_START_THIS_MARKETING_CAMPAIGN },
    { WWT_IMGBTN,           1,  8,      521,    0,      11,     0xFFFFFFFF,                             STR_START_THIS_MARKETING_CAMPAIGN },
    { WIDGETS_END },
};

static rct_widget window_finances_research_widgets[] = {
    { WWT_FRAME,            0,  0,      319,    0,      206,    0xFFFFFFFF,                             STR_NONE },
    { WWT_CAPTION,          0,  1,      318,    1,      14,     STR_RESEARCH_FUNDING,                   STR_WINDOW_TITLE_TIP },
    { WWT_CLOSEBOX,         0,  307,    317,    2,      13,     STR_CLOSE_X,                            STR_CLOSE_WINDOW_TIP },
    { WWT_RESIZE,           1,  0,      319,    43,     206,    0xFFFFFFFF,                             STR_NONE },
    { WWT_TAB,              1,  3,      33,     17,     43,     IMAGE_TYPE_REMAP | SPR_TAB,                   STR_FINANCES_SHOW_SUMMARY_TAB_TIP },
    { WWT_TAB,              1,  34,     64,     17,     43,     IMAGE_TYPE_REMAP | SPR_TAB,                   STR_FINANCES_SHOW_CASH_TAB_TIP },
    { WWT_TAB,              1,  65,     95,     17,     43,     IMAGE_TYPE_REMAP | SPR_TAB,                   STR_FINANCES_SHOW_PARK_VALUE_TAB_TIP },
    { WWT_TAB,              1,  96,     126,    17,     43,     IMAGE_TYPE_REMAP | SPR_TAB,                   STR_FINANCES_SHOW_WEEKLY_PROFIT_TAB_TIP },
    { WWT_TAB,              1,  127,    157,    17,     43,     IMAGE_TYPE_REMAP | SPR_TAB,                   STR_FINANCES_SHOW_MARKETING_TAB_TIP },
    { WWT_TAB,              1,  158,    188,    17,     43,     IMAGE_TYPE_REMAP | SPR_TAB,                   STR_FINANCES_RESEARCH_TIP },
    { WWT_GROUPBOX,         2,  3,      316,    47,     91,     STR_RESEARCH_FUNDING_,                  STR_NONE },
    { WWT_DROPDOWN,         2,  8,      167,    59,     70,     0xFFFFFFFF,                             STR_SELECT_LEVEL_OF_RESEARCH_AND_DEVELOPMENT },
    { WWT_DROPDOWN_BUTTON,  2,  156,    166,    60,     69,     STR_DROPDOWN_GLYPH,                     STR_SELECT_LEVEL_OF_RESEARCH_AND_DEVELOPMENT },
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

static rct_widget *window_finances_page_widgets[] = {
    window_finances_summary_widgets,
    window_finances_cash_widgets,
    window_finances_park_value_widgets,
    window_finances_profit_widgets,
    window_finances_marketing_widgets,
    window_finances_research_widgets
};

#pragma endregion

#pragma region Events

static void window_finances_summary_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_finances_summary_mousedown(rct_window *w, rct_widgetindex widgetIndex, rct_widget* widget);
static void window_finances_summary_update(rct_window *w);
static void window_finances_summary_invalidate(rct_window *w);
static void window_finances_summary_paint(rct_window *w, rct_drawpixelinfo *dpi);

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
static void window_finances_research_dropdown(rct_window *w, rct_widgetindex widgetIndex, sint32 dropdownIndex);
static void window_finances_research_update(rct_window *w);
static void window_finances_research_invalidate(rct_window *w);
static void window_finances_research_paint(rct_window *w, rct_drawpixelinfo *dpi);

// 0x00988EB8
static rct_window_event_list window_finances_summary_events = {
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
    window_finances_summary_invalidate,
    window_finances_summary_paint,
    nullptr
};

// 0x00988F28
static rct_window_event_list window_finances_financial_graph_events = {
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
static rct_window_event_list window_finances_value_graph_events = {
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
static rct_window_event_list window_finances_profit_graph_events = {
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
static rct_window_event_list window_finances_marketing_events = {
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
static rct_window_event_list window_finances_research_events = {
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

static rct_window_event_list *window_finances_page_events[] = {
    &window_finances_summary_events,
    &window_finances_financial_graph_events,
    &window_finances_value_graph_events,
    &window_finances_profit_graph_events,
    &window_finances_marketing_events,
    &window_finances_research_events
};

static void window_finances_set_colours();

#pragma endregion

#pragma region Enabled widgets

static uint32 window_finances_page_enabled_widgets[] = {
    (1 << WIDX_CLOSE) |
    (1 << WIDX_TAB_1) |
    (1 << WIDX_TAB_2) |
    (1 << WIDX_TAB_3) |
    (1 << WIDX_TAB_4) |
    (1 << WIDX_TAB_5) |
    (1 << WIDX_TAB_6) |
    (1 << WIDX_LOAN_INCREASE) |
    (1 << WIDX_LOAN_DECREASE),

    (1 << WIDX_CLOSE) |
    (1 << WIDX_TAB_1) |
    (1 << WIDX_TAB_2) |
    (1 << WIDX_TAB_3) |
    (1 << WIDX_TAB_4) |
    (1 << WIDX_TAB_5) |
    (1 << WIDX_TAB_6),

    (1 << WIDX_CLOSE) |
    (1 << WIDX_TAB_1) |
    (1 << WIDX_TAB_2) |
    (1 << WIDX_TAB_3) |
    (1 << WIDX_TAB_4) |
    (1 << WIDX_TAB_5) |
    (1 << WIDX_TAB_6),

    (1 << WIDX_CLOSE) |
    (1 << WIDX_TAB_1) |
    (1 << WIDX_TAB_2) |
    (1 << WIDX_TAB_3) |
    (1 << WIDX_TAB_4) |
    (1 << WIDX_TAB_5) |
    (1 << WIDX_TAB_6),

    (1 << WIDX_CLOSE) |
    (1 << WIDX_TAB_1) |
    (1 << WIDX_TAB_2) |
    (1 << WIDX_TAB_3) |
    (1 << WIDX_TAB_4) |
    (1 << WIDX_TAB_5) |
    (1 << WIDX_TAB_6) |
    (1 << WIDX_CAMPAIGN_1) |
    (1 << WIDX_CAMPAIGN_2) |
    (1 << WIDX_CAMPAIGN_3) |
    (1 << WIDX_CAMPAIGN_4) |
    (1 << WIDX_CAMPAIGN_5) |
    (1 << WIDX_CAMPAIGN_6),

    (1 << WIDX_CLOSE) |
    (1 << WIDX_TAB_1) |
    (1 << WIDX_TAB_2) |
    (1 << WIDX_TAB_3) |
    (1 << WIDX_TAB_4) |
    (1 << WIDX_TAB_5) |
    (1 << WIDX_TAB_6) |

    (1 << WIDX_RESEARCH_FUNDING) |
    (1 << WIDX_RESEARCH_FUNDING_DROPDOWN_BUTTON) |
    (1 << WIDX_TRANSPORT_RIDES) |
    (1 << WIDX_GENTLE_RIDES) |
    (1 << WIDX_ROLLER_COASTERS) |
    (1 << WIDX_THRILL_RIDES) |
    (1 << WIDX_WATER_RIDES) |
    (1 << WIDX_SHOPS_AND_STALLS) |
    (1 << WIDX_SCENERY_AND_THEMING)
};

static uint32 window_finances_page_hold_down_widgets[] = {
    (1 << WIDX_LOAN_INCREASE) |
    (1 << WIDX_LOAN_DECREASE),

    0,
    0,
    0,
    0,
    0
};

#pragma endregion

const sint32 window_finances_tab_animation_loops[] = { 16, 32, 32, 32, 38, 16 };

static const rct_string_id window_finances_summary_row_labels[RCT_EXPENDITURE_TYPE_COUNT] = {
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

static void window_finances_set_page(rct_window *w, sint32 page);
static void window_finances_set_pressed_tab(rct_window *w);
static void window_finances_draw_tab_images(rct_drawpixelinfo *dpi, rct_window *w);

/**
 *
 *  rct2: 0x0069DDF1
 */
void window_finances_open()
{
    rct_window *w;

    w = window_bring_to_front_by_class(WC_FINANCES);
    if (w == nullptr) {
        w = window_create_auto_pos(530, 257, window_finances_page_events[0], WC_FINANCES, WF_10);
        w->number = 0;
        w->frame_no = 0;

        research_update_uncompleted_types();
    }

    w->page = WINDOW_FINANCES_PAGE_SUMMARY;
    window_invalidate(w);
    w->width = 530;
    w->height = 257;
    window_invalidate(w);

    w->widgets = window_finances_page_widgets[WINDOW_FINANCES_PAGE_SUMMARY];
    w->enabled_widgets = window_finances_page_enabled_widgets[WINDOW_FINANCES_PAGE_SUMMARY];
    w->hold_down_widgets = window_finances_page_hold_down_widgets[WINDOW_FINANCES_PAGE_SUMMARY];
    w->event_handlers = window_finances_page_events[WINDOW_FINANCES_PAGE_SUMMARY];
    w->pressed_widgets = 0;
    w->disabled_widgets = 0;
    window_init_scroll_widgets(w);
}

/**
 *
 *  rct2: 0x0069DDE1
 */
void window_finances_research_open()
{
    rct_window *w;

    window_finances_open();
    w = window_find_by_class(WC_FINANCES);
    if (w != nullptr)
        window_finances_set_page(w, WINDOW_FINANCES_PAGE_RESEARCH);
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
    money32 newLoan;

    switch (widgetIndex) {
    case WIDX_LOAN_INCREASE:
        newLoan = gBankLoan + MONEY(1000, 00);
        gGameCommandErrorTitle = STR_CANT_BORROW_ANY_MORE_MONEY;
        finance_set_loan(newLoan);
        break;
    case WIDX_LOAN_DECREASE:
        if (gBankLoan > 0) {
            newLoan = gBankLoan - MONEY(1000, 00);
            gGameCommandErrorTitle = STR_CANT_PAY_BACK_LOAN;
            finance_set_loan(newLoan);
        }
        break;
    }
}

/**
 *
 *  rct2: 0x0069CBA6
 */
static void window_finances_summary_update(rct_window *w)
{
    // Tab animation
    if (++w->frame_no >= window_finances_tab_animation_loops[w->page])
        w->frame_no = 0;
    widget_invalidate(w, WIDX_TAB_1);
}

/**
 *
 *  rct2: 0x0069C732
 */
static void window_finances_summary_invalidate(rct_window *w)
{
    if (w->widgets != window_finances_page_widgets[WINDOW_FINANCES_PAGE_SUMMARY]) {
        w->widgets = window_finances_page_widgets[WINDOW_FINANCES_PAGE_SUMMARY];
        window_init_scroll_widgets(w);
    }

    window_finances_set_pressed_tab(w);
    set_format_arg(6, money32, gBankLoan);
}

/**
 *
 *  rct2: 0x0069C771
 */
static void window_finances_summary_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
    sint32 i, j, x, y;

    window_draw_widgets(w, dpi);
    window_finances_draw_tab_images(dpi, w);

    x = w->x + 8;
    y = w->y + 47;

    // Expenditure / Income heading
    draw_string_left_underline(dpi, STR_FINANCES_SUMMARY_EXPENDITURE_INCOME, nullptr, COLOUR_BLACK, x, y - 1);
    y += 14;

    // Expenditure / Income row labels
    for (i = 0; i < RCT_EXPENDITURE_TYPE_COUNT; i++) {
        // Darken every even row
        if (i % 2 == 0)
            gfx_fill_rect(dpi, x, y, x + 513 - 2, y + 9, ColourMapA[w->colours[1]].lighter | 0x1000000);

        gfx_draw_string_left(dpi, window_finances_summary_row_labels[i], nullptr, COLOUR_BLACK, x, y - 1);
        y += 10;
    }

    // Expenditure / Income values for each month
    x = w->x + 118;
    sint16 currentMonthYear = gDateMonthsElapsed;
    for (i = 4; i >= 0; i--) {
        y = w->y + 47;

        sint16 monthyear = currentMonthYear - i;
        if (monthyear < 0)
            continue;

        // Month heading
        set_format_arg(0, rct_string_id, STR_FINANCES_SUMMARY_MONTH_HEADING);
        set_format_arg(2, uint16, monthyear);
        draw_string_right_underline(
            dpi,
            monthyear == currentMonthYear ? STR_WINDOW_COLOUR_2_STRINGID : STR_BLACK_STRING,
            gCommonFormatArgs,
            COLOUR_BLACK,
            x + 80,
            y - 1
        );
        y += 14;

        // Month expenditures
        money32 profit = 0;
        money32 *expenditures = &gExpenditureTable[i * RCT_EXPENDITURE_TYPE_COUNT];
        for (j = 0; j < RCT_EXPENDITURE_TYPE_COUNT; j++) {
            money32 expenditure = expenditures[j];
            if (expenditure != 0) {
                profit += expenditure;
                gfx_draw_string_right(
                    dpi,
                    expenditure >= 0 ? STR_FINANCES_SUMMARY_INCOME_VALUE : STR_FINANCES_SUMMARY_EXPENDITURE_VALUE,
                    &expenditure,
                    COLOUR_BLACK,
                    x + 80,
                    y - 1
                );
            }
            y += 10;
        }
        y += 4;

        // Month profit
        gfx_draw_string_right(
            dpi,
            profit >= 0 ? STR_FINANCES_SUMMARY_INCOME_VALUE : STR_FINANCES_SUMMARY_LOSS_VALUE,
            &profit,
            COLOUR_BLACK,
            x + 80,
            y - 1
        );
        gfx_fill_rect(dpi, x + 10, y - 2, x + 10 + 70, y - 2, PALETTE_INDEX_10);

        x += 80;
    }


    // Horizontal rule below expenditure / income table
    gfx_fill_rect_inset(dpi, w->x + 8, w->y + 223, w->x + 8 + 513, w->y + 223 + 1, w->colours[1], INSET_RECT_FLAG_BORDER_INSET);

    // Loan and interest rate
    gfx_draw_string_left(dpi, STR_FINANCES_SUMMARY_LOAN, nullptr, COLOUR_BLACK, w->x + 4, w->y + 229);
    set_format_arg(0, uint16, gBankLoanInterestRate);
    gfx_draw_string_left(dpi, STR_FINANCES_SUMMARY_AT_X_PER_YEAR, gCommonFormatArgs, COLOUR_BLACK, w->x + 156, w->y + 229);

    // Current cash
    money32 currentCash = DECRYPT_MONEY(gCashEncrypted);
    rct_string_id stringId = currentCash >= 0 ? STR_CASH_LABEL : STR_CASH_NEGATIVE_LABEL;
    gfx_draw_string_left(dpi, stringId, &currentCash, COLOUR_BLACK, w->x + 4, w->y + 244);

    // Objective related financial information
    if (gScenarioObjectiveType == OBJECTIVE_MONTHLY_FOOD_INCOME) {
        money32 lastMonthProfit = finance_get_last_month_shop_profit();
        set_format_arg(0, money32, lastMonthProfit);
        gfx_draw_string_left(dpi, STR_LAST_MONTH_PROFIT_FROM_FOOD_DRINK_MERCHANDISE_SALES_LABEL, gCommonFormatArgs, COLOUR_BLACK, w->x + 280, w->y + 229);
    } else {
        // Park value and company value
        gfx_draw_string_left(dpi, STR_PARK_VALUE_LABEL, &gParkValue, COLOUR_BLACK, w->x + 280, w->y + 229);
        gfx_draw_string_left(dpi, STR_COMPANY_VALUE_LABEL, &gCompanyValue, COLOUR_BLACK, w->x + 280, w->y + 244);
    }
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
    if (++w->frame_no >= window_finances_tab_animation_loops[w->page])
        w->frame_no = 0;
    widget_invalidate(w, WIDX_TAB_2);
}

/**
 *
 *  rct2: 0x0069CBDB
 */
static void window_finances_financial_graph_invalidate(rct_window *w)
{
    if (w->widgets != window_finances_page_widgets[WINDOW_FINANCES_PAGE_FINANCIAL_GRAPH]) {
        w->widgets = window_finances_page_widgets[WINDOW_FINANCES_PAGE_FINANCIAL_GRAPH];
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
    sint32 i, x, y, graphLeft, graphTop, graphRight, graphBottom;

    window_draw_widgets(w, dpi);
    window_finances_draw_tab_images(dpi, w);

    rct_widget *pageWidget = &window_finances_cash_widgets[WIDX_PAGE_BACKGROUND];
    graphLeft = w->x + pageWidget->left + 4;
    graphTop = w->y + pageWidget->top + 15;
    graphRight = w->x + pageWidget->right - 4;
    graphBottom = w->y + pageWidget->bottom - 4;

    // Cash (less loan)
    money32 cashLessLoan =
        DECRYPT_MONEY(gCashEncrypted) -
        gBankLoan;

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
    sint32 yAxisScale = 0;
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
    if (++w->frame_no >= window_finances_tab_animation_loops[w->page])
        w->frame_no = 0;
    widget_invalidate(w, WIDX_TAB_2);
}

/**
 *
 *  rct2: 0x0069CFC0
 */
static void window_finances_park_value_graph_invalidate(rct_window *w)
{
    if (w->widgets != window_finances_page_widgets[WINDOW_FINANCES_PAGE_VALUE_GRAPH]) {
        w->widgets = window_finances_page_widgets[WINDOW_FINANCES_PAGE_VALUE_GRAPH];
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
    sint32 i, x, y, graphLeft, graphTop, graphRight, graphBottom;

    window_draw_widgets(w, dpi);
    window_finances_draw_tab_images(dpi, w);

    rct_widget *pageWidget = &window_finances_cash_widgets[WIDX_PAGE_BACKGROUND];
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
    sint32 yAxisScale = 0;
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
    if (++w->frame_no >= window_finances_tab_animation_loops[w->page])
        w->frame_no = 0;
    widget_invalidate(w, WIDX_TAB_2);
}

/**
 *
 *  rct2: 0x0069D388
 */
static void window_finances_profit_graph_invalidate(rct_window *w)
{
    if (w->widgets != window_finances_page_widgets[WINDOW_FINANCES_PAGE_PROFIT_GRAPH]) {
        w->widgets = window_finances_page_widgets[WINDOW_FINANCES_PAGE_PROFIT_GRAPH];
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
    sint32 i, x, y, graphLeft, graphTop, graphRight, graphBottom;

    window_draw_widgets(w, dpi);
    window_finances_draw_tab_images(dpi, w);

    rct_widget *pageWidget = &window_finances_cash_widgets[WIDX_PAGE_BACKGROUND];
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
    sint32 yAxisScale = 0;
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
        window_new_campaign_open(widgetIndex - WIDX_CAMPAIGN_1);

}

/**
 *
 *  rct2: 0x0069DA2F
 */
static void window_finances_marketing_update(rct_window *w)
{
    // Tab animation
    if (++w->frame_no >= window_finances_tab_animation_loops[w->page])
        w->frame_no = 0;
    widget_invalidate(w, WIDX_TAB_5);
}

/**
 *
 *  rct2: 0x0069D765
 */
static void window_finances_marketing_invalidate(rct_window *w)
{
    sint32 i;

    if (w->widgets != window_finances_page_widgets[WINDOW_FINANCES_PAGE_MARKETING]) {
        w->widgets = window_finances_page_widgets[WINDOW_FINANCES_PAGE_MARKETING];
        window_init_scroll_widgets(w);
    }

    window_finances_set_pressed_tab(w);

    // Count number of active campaigns
    sint32 numActiveCampaigns = 0;
    for (i = 0; i < ADVERTISING_CAMPAIGN_COUNT; i++)
        if (gMarketingCampaignDaysLeft[i] != 0)
            numActiveCampaigns++;

    sint32 y = Math::Max(1, numActiveCampaigns) * 10 + 92;

    // Update group box positions
    window_finances_marketing_widgets[WIDX_ACITVE_CAMPAGINS_GROUP].bottom = y - 20;
    window_finances_marketing_widgets[WIDX_CAMPAGINS_AVAILABLE_GROUP].top = y - 13;

    // Update new campaign button visibility
    for (i = 0; i < ADVERTISING_CAMPAIGN_COUNT; i++) {
        rct_widget *campaginButton = &window_finances_marketing_widgets[WIDX_CAMPAIGN_1 + i];

        campaginButton->type = WWT_EMPTY;

        if (gMarketingCampaignDaysLeft[i] != 0)
            continue;

        if (!marketing_is_campaign_type_applicable(i))
            continue;

        campaginButton->type = WWT_DROPDOWN_BUTTON;
        campaginButton->top = y;
        campaginButton->bottom = y + 11;
        y += 12;
    }
}

/**
 *
 *  rct2: 0x0069D834
 */
static void window_finances_marketing_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
    sint32 i, x, y, weeksRemaining;
    rct_ride *ride;

    window_draw_widgets(w, dpi);
    window_finances_draw_tab_images(dpi, w);

    x = w->x + 8;
    y = w->y + 62;

    sint32 noCampaignsActive = 1;
    for (i = 0; i < ADVERTISING_CAMPAIGN_COUNT; i++) {
        if (gMarketingCampaignDaysLeft[i] == 0)
            continue;

        noCampaignsActive = 0;
        set_format_arg(0, rct_string_id, gParkName);
        set_format_arg(2, uint32, gParkNameArgs);

        // Set special parameters
        switch (i) {
        case ADVERTISING_CAMPAIGN_RIDE_FREE:
        case ADVERTISING_CAMPAIGN_RIDE:
            ride = get_ride(gMarketingCampaignRideIndex[i]);
            set_format_arg(0, rct_string_id, ride->name);
            set_format_arg(2, uint32, ride->name_arguments);
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

        y += 10;
    }

    if (noCampaignsActive) {
        gfx_draw_string_left(dpi, STR_MARKETING_CAMPAGINS_NONE, nullptr, COLOUR_BLACK, x + 4, y);
        y += 10;
    }
    y += 31;

    // Draw campaign button text
    for (i = 0; i < ADVERTISING_CAMPAIGN_COUNT; i++) {
        rct_widget *campaginButton = &window_finances_marketing_widgets[WIDX_CAMPAIGN_1 + i];

        if (campaginButton->type == WWT_EMPTY)
            continue;

        money32 pricePerWeek = AdvertisingCampaignPricePerWeek[i];

        // Draw button text
        gfx_draw_string_left(dpi, MarketingCampaignNames[i][0], nullptr, COLOUR_BLACK, x + 4, y - 1);
        gfx_draw_string_left(dpi, STR_MARKETING_PER_WEEK, &pricePerWeek, COLOUR_BLACK, x + 310, y - 1);

        y += 12;
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
    sint32 activeResearchTypes;

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
        activeResearchTypes = gResearchPriorities;
        activeResearchTypes ^= 1 << (widgetIndex - WIDX_TRANSPORT_RIDES);
        research_set_priority(activeResearchTypes);
        break;
    }
}

/**
 *
 *  rct2: 0x0069DB66
 */
static void window_finances_research_mousedown(rct_window *w, rct_widgetindex widgetIndex, rct_widget* widget)
{
    rct_widget *dropdownWidget;
    sint32 i;

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

    sint32 currentResearchLevel = gResearchFundingLevel;
    dropdown_set_checked(currentResearchLevel, true);
}

/**
 *
 *  rct2: 0x0069DB6D
 */
static void window_finances_research_dropdown(rct_window *w, rct_widgetindex widgetIndex, sint32 dropdownIndex)
{
    if (widgetIndex != WIDX_RESEARCH_FUNDING_DROPDOWN_BUTTON || dropdownIndex == -1)
        return;

    research_set_funding(dropdownIndex);
}

/**
 *
 *  rct2: 0x0069DC23
 */
static void window_finances_research_update(rct_window *w)
{
    // Tab animation
    if (++w->frame_no >= window_finances_tab_animation_loops[w->page])
        w->frame_no = 0;
    widget_invalidate(w, WIDX_TAB_6);
}

/**
 *
 *  rct2: 0x0069DA64
 */
static void window_finances_research_invalidate(rct_window *w)
{
    if (w->widgets != window_finances_page_widgets[WINDOW_FINANCES_PAGE_RESEARCH]) {
        w->widgets = window_finances_page_widgets[WINDOW_FINANCES_PAGE_RESEARCH];
        window_init_scroll_widgets(w);
    }

    window_finances_set_pressed_tab(w);
    if (gResearchProgressStage == RESEARCH_STAGE_FINISHED_ALL) {
        window_finances_research_widgets[WIDX_RESEARCH_FUNDING].type = WWT_EMPTY;
        window_finances_research_widgets[WIDX_RESEARCH_FUNDING_DROPDOWN_BUTTON].type = WWT_EMPTY;
    }
    sint32 currentResearchLevel = gResearchFundingLevel;

    // Current funding
    window_finances_research_widgets[WIDX_RESEARCH_FUNDING].text = ResearchFundingLevelNames[currentResearchLevel];

    // Checkboxes
    uint8 activeResearchTypes = gResearchPriorities;
    sint32 uncompletedResearchTypes = gResearchUncompletedCategories;
    for (sint32 i = 0; i < 7; i++) {
        sint32 mask = 1 << i;
        sint32 widgetMask = 1 << (i + WIDX_TRANSPORT_RIDES);

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
static void window_finances_set_page(rct_window *w, sint32 page)
{
    w->page = page;
    w->frame_no = 0;
    if (w->viewport != nullptr) {
        w->viewport->width = 0;
        w->viewport = nullptr;
    }

    w->enabled_widgets = window_finances_page_enabled_widgets[page];
    w->hold_down_widgets = window_finances_page_hold_down_widgets[page];
    w->event_handlers = window_finances_page_events[page];
    w->widgets = window_finances_page_widgets[page];
    w->disabled_widgets = 0;
    w->pressed_widgets = 0;

    window_invalidate(w);
    if (w->page == WINDOW_FINANCES_PAGE_RESEARCH) {
        w->width = 320;
        w->height = 207;
    } else {
        w->width = 530;
        w->height = 257;
    }
    window_event_resize_call(w);
    window_event_invalidate_call(w);

    window_init_scroll_widgets(w);
    window_invalidate(w);
}

static void window_finances_set_pressed_tab(rct_window *w)
{
    sint32 i;
    for (i = 0; i < WINDOW_FINANCES_PAGE_COUNT; i++)
        w->pressed_widgets &= ~(1 << (WIDX_TAB_1 + i));
    w->pressed_widgets |= 1LL << (WIDX_TAB_1 + w->page);
}

static void window_finances_draw_tab_image(rct_drawpixelinfo *dpi, rct_window *w, sint32 page, sint32 spriteIndex)
{
    rct_widgetindex widgetIndex = WIDX_TAB_1 + page;

    if (!(w->disabled_widgets & (1LL << widgetIndex))) {
        if (w->page == page) {
            sint32 frame = w->frame_no / 2;
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
