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
#include "../network/network.h"

extern "C"
{
    #include "../game.h"
    #include "../input.h"
    #include "../management/marketing.h"
    #include "../peep/staff.h"
    #include "../ride/ride_data.h"
    #include "../localisation/localisation.h"
    #include "../sprites.h"
    #include "../interface/viewport.h"
    #include "../interface/widget.h"
    #include "../util/util.h"
    #include "../world/footpath.h"
}

enum WINDOW_GUEST_PAGE {
    WINDOW_GUEST_OVERVIEW,
    WINDOW_GUEST_STATS,
    WINDOW_GUEST_RIDES,
    WINDOW_GUEST_FINANCE,
    WINDOW_GUEST_THOUGHTS,
    WINDOW_GUEST_INVENTORY
};

enum WINDOW_GUEST_WIDGET_IDX {
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

    WIDX_MARQUEE = 10,
    WIDX_VIEWPORT,
    WIDX_ACTION_LBL,
    WIDX_PICKUP,
    WIDX_RENAME,
    WIDX_LOCATE,
    WIDX_TRACK,

    WIDX_RIDE_SCROLL = 10
};

validate_global_widx(WC_PEEP, WIDX_ACTION_LBL);

static rct_widget window_guest_overview_widgets[] = {
    {WWT_FRAME,    0, 0,   191, 0,   156, 0xFFFFFFFF, STR_NONE},                            // Panel / Background
    {WWT_CAPTION,  0, 1,   190, 1,   14,  STR_STRINGID,         STR_WINDOW_TITLE_TIP},      // Title
    {WWT_CLOSEBOX, 0, 179, 189, 2,   13,  STR_CLOSE_X,          STR_CLOSE_WINDOW_TIP},      // Close x button
    {WWT_RESIZE,   1, 0,   191, 43,  156, 0xFFFFFFFF, STR_NONE},                            // Resize
    {WWT_TAB,      1, 3,   33,  17,  43,  IMAGE_TYPE_REMAP | SPR_TAB, STR_SHOW_GUEST_VIEW_TIP},           // Tab 1
    {WWT_TAB,      1, 73,  64,  17,  43,  IMAGE_TYPE_REMAP | SPR_TAB, STR_SHOW_GUEST_NEEDS_TIP},          // Tab 2
    {WWT_TAB,      1, 65,  95,  17,  43,  IMAGE_TYPE_REMAP | SPR_TAB, STR_SHOW_GUEST_VISITED_RIDES_TIP},  // Tab 3
    {WWT_TAB,      1, 96,  126, 17,  43,  IMAGE_TYPE_REMAP | SPR_TAB, STR_SHOW_GUEST_FINANCE_TIP},        // Tab 4
    {WWT_TAB,      1, 127, 157, 17,  43,  IMAGE_TYPE_REMAP | SPR_TAB, STR_SHOW_GUEST_THOUGHTS_TIP},       // Tab 5
    {WWT_TAB,      1, 158, 188, 17,  43,  IMAGE_TYPE_REMAP | SPR_TAB, STR_SHOW_GUEST_ITEMS_TIP},          // Tab 6
    {WWT_12,       1, 3,   166, 45,  56,  0xFFFFFFFF, STR_NONE},                // Label Thought marquee
    {WWT_VIEWPORT, 1, 3,   166, 57,  143, 0xFFFFFFFF, STR_NONE},                // Viewport
    {WWT_12,       1, 3,   166, 144, 154, 0xFFFFFFFF, STR_NONE},                // Label Action
    {WWT_FLATBTN,  1, 167, 190, 45,  68,  SPR_PICKUP_BTN,       STR_PICKUP_TIP},                    // Pickup Button
    {WWT_FLATBTN,  1, 167, 190, 69,  92,  SPR_RENAME,           STR_NAME_GUEST_TIP},                // Rename Button
    {WWT_FLATBTN,  1, 167, 190, 93,  116, SPR_LOCATE,           STR_LOCATE_SUBJECT_TIP},            // Locate Button
    {WWT_FLATBTN,  1, 167, 190, 117, 140, SPR_TRACK_PEEP,       STR_TOGGLE_GUEST_TRACKING_TIP},     // Track Button
    { WIDGETS_END },
};

static rct_widget window_guest_stats_widgets[] = {
    {WWT_FRAME,    0, 0,   191, 0,  156, STR_NONE, STR_NONE},
    {WWT_CAPTION,  0, 1,   190, 1,  14, STR_STRINGID,         STR_WINDOW_TITLE_TIP},
    {WWT_CLOSEBOX, 0, 179, 189, 2,  13, STR_CLOSE_X,          STR_CLOSE_WINDOW_TIP},
    {WWT_RESIZE,   1, 0,   191, 43, 156, STR_NONE, STR_NONE},
    {WWT_TAB,      1, 3,   33,  17, 43, IMAGE_TYPE_REMAP | SPR_TAB, STR_SHOW_GUEST_VIEW_TIP},
    {WWT_TAB,      1, 34,  64,  17, 43, IMAGE_TYPE_REMAP | SPR_TAB, STR_SHOW_GUEST_NEEDS_TIP},
    {WWT_TAB,      1, 65,  95,  17, 43, IMAGE_TYPE_REMAP | SPR_TAB, STR_SHOW_GUEST_VISITED_RIDES_TIP},
    {WWT_TAB,      1, 96,  126, 17, 43, IMAGE_TYPE_REMAP | SPR_TAB, STR_SHOW_GUEST_FINANCE_TIP},
    {WWT_TAB,      1, 127, 157, 17, 43, IMAGE_TYPE_REMAP | SPR_TAB, STR_SHOW_GUEST_THOUGHTS_TIP},
    {WWT_TAB,      1, 158, 188, 17, 43, IMAGE_TYPE_REMAP | SPR_TAB, STR_SHOW_GUEST_ITEMS_TIP},
    {WIDGETS_END},
};

static rct_widget window_guest_rides_widgets[] = {
    {WWT_FRAME,    0, 0,   191, 0,  156, STR_NONE, STR_NONE},
    {WWT_CAPTION,  0, 1,   190, 1,  14,  STR_STRINGID,         STR_WINDOW_TITLE_TIP},
    {WWT_CLOSEBOX, 0, 179, 189, 2,  13,  STR_CLOSE_X,          STR_CLOSE_WINDOW_TIP},
    {WWT_RESIZE,   1, 0,   191, 43, 156, STR_NONE, STR_NONE},
    {WWT_TAB,      1, 3,   33,  17, 43,  IMAGE_TYPE_REMAP | SPR_TAB, STR_SHOW_GUEST_VIEW_TIP},
    {WWT_TAB,      1, 34,  64,  17, 43,  IMAGE_TYPE_REMAP | SPR_TAB, STR_SHOW_GUEST_NEEDS_TIP},
    {WWT_TAB,      1, 65,  95,  17, 43,  IMAGE_TYPE_REMAP | SPR_TAB, STR_SHOW_GUEST_VISITED_RIDES_TIP},
    {WWT_TAB,      1, 96,  126, 17, 43,  IMAGE_TYPE_REMAP | SPR_TAB, STR_SHOW_GUEST_FINANCE_TIP},
    {WWT_TAB,      1, 127, 157, 17, 43,  IMAGE_TYPE_REMAP | SPR_TAB, STR_SHOW_GUEST_THOUGHTS_TIP},
    {WWT_TAB,      1, 158, 188, 17, 43,  IMAGE_TYPE_REMAP | SPR_TAB, STR_SHOW_GUEST_ITEMS_TIP},
    {WWT_SCROLL,   1, 3,   188, 57, 143, SCROLL_VERTICAL,  STR_NONE},
    {WIDGETS_END},
};

static rct_widget window_guest_finance_widgets[] = {
    {WWT_FRAME,    0, 0,   191, 0,  156, STR_NONE, STR_NONE},
    {WWT_CAPTION,  0, 1,   190, 1,  14, STR_STRINGID,         STR_WINDOW_TITLE_TIP},
    {WWT_CLOSEBOX, 0, 179, 189, 2,  13, STR_CLOSE_X,          STR_CLOSE_WINDOW_TIP},
    {WWT_RESIZE,   1, 0,   191, 43, 156, STR_NONE, STR_NONE},
    {WWT_TAB,      1, 3,   33,  17, 43, IMAGE_TYPE_REMAP | SPR_TAB, STR_SHOW_GUEST_VIEW_TIP},
    {WWT_TAB,      1, 34,  64,  17, 43, IMAGE_TYPE_REMAP | SPR_TAB, STR_SHOW_GUEST_NEEDS_TIP},
    {WWT_TAB,      1, 65,  95,  17, 43, IMAGE_TYPE_REMAP | SPR_TAB, STR_SHOW_GUEST_VISITED_RIDES_TIP},
    {WWT_TAB,      1, 96,  126, 17, 43, IMAGE_TYPE_REMAP | SPR_TAB, STR_SHOW_GUEST_FINANCE_TIP},
    {WWT_TAB,      1, 127, 157, 17, 43, IMAGE_TYPE_REMAP | SPR_TAB, STR_SHOW_GUEST_THOUGHTS_TIP},
    {WWT_TAB,      1, 158, 188, 17, 43, IMAGE_TYPE_REMAP | SPR_TAB, STR_SHOW_GUEST_ITEMS_TIP},
    {WIDGETS_END},
};

static rct_widget window_guest_thoughts_widgets[] = {
    {WWT_FRAME,    0, 0,   191, 0,  156, STR_NONE, STR_NONE},
    {WWT_CAPTION,  0, 1,   190, 1,  14, STR_STRINGID,         STR_WINDOW_TITLE_TIP},
    {WWT_CLOSEBOX, 0, 179, 189, 2,  13, STR_CLOSE_X,          STR_CLOSE_WINDOW_TIP},
    {WWT_RESIZE,   1, 0,   191, 43, 156, STR_NONE, STR_NONE},
    {WWT_TAB,      1, 3,   33,  17, 43, IMAGE_TYPE_REMAP | SPR_TAB, STR_SHOW_GUEST_VIEW_TIP},
    {WWT_TAB,      1, 34,  64,  17, 43, IMAGE_TYPE_REMAP | SPR_TAB, STR_SHOW_GUEST_NEEDS_TIP},
    {WWT_TAB,      1, 65,  95,  17, 43, IMAGE_TYPE_REMAP | SPR_TAB, STR_SHOW_GUEST_VISITED_RIDES_TIP},
    {WWT_TAB,      1, 96,  126, 17, 43, IMAGE_TYPE_REMAP | SPR_TAB, STR_SHOW_GUEST_FINANCE_TIP},
    {WWT_TAB,      1, 127, 157, 17, 43, IMAGE_TYPE_REMAP | SPR_TAB, STR_SHOW_GUEST_THOUGHTS_TIP},
    {WWT_TAB,      1, 158, 188, 17, 43, IMAGE_TYPE_REMAP | SPR_TAB, STR_SHOW_GUEST_ITEMS_TIP},
    {WIDGETS_END},
};

static rct_widget window_guest_inventory_widgets[] = {
    {WWT_FRAME,    0, 0,   191, 0,  156, STR_NONE, STR_NONE},
    {WWT_CAPTION,  0, 1,   190, 1,  14, STR_STRINGID,         STR_WINDOW_TITLE_TIP},
    {WWT_CLOSEBOX, 0, 179, 189, 2,  13, STR_CLOSE_X,          STR_CLOSE_WINDOW_TIP},
    {WWT_RESIZE,   1, 0,   191, 43, 156, STR_NONE, STR_NONE},
    {WWT_TAB,      1, 3,   33,  17, 43, IMAGE_TYPE_REMAP | SPR_TAB, STR_SHOW_GUEST_VIEW_TIP},
    {WWT_TAB,      1, 34,  64,  17, 43, IMAGE_TYPE_REMAP | SPR_TAB, STR_SHOW_GUEST_NEEDS_TIP},
    {WWT_TAB,      1, 65,  95,  17, 43, IMAGE_TYPE_REMAP | SPR_TAB, STR_SHOW_GUEST_VISITED_RIDES_TIP},
    {WWT_TAB,      1, 96,  126, 17, 43, IMAGE_TYPE_REMAP | SPR_TAB, STR_SHOW_GUEST_FINANCE_TIP},
    {WWT_TAB,      1, 127, 157, 17, 43, IMAGE_TYPE_REMAP | SPR_TAB, STR_SHOW_GUEST_THOUGHTS_TIP},
    {WWT_TAB,      1, 158, 188, 17, 43, IMAGE_TYPE_REMAP | SPR_TAB, STR_SHOW_GUEST_ITEMS_TIP},
    {WIDGETS_END},
};

// 0x981D0C
static rct_widget *window_guest_page_widgets[] = {
    window_guest_overview_widgets,
    window_guest_stats_widgets,
    window_guest_rides_widgets,
    window_guest_finance_widgets,
    window_guest_thoughts_widgets,
    window_guest_inventory_widgets
};

static void window_guest_set_page(rct_window* w, sint32 page);
static void window_guest_disable_widgets(rct_window* w);
static void window_guest_viewport_init(rct_window* w);

static void window_guest_overview_close(rct_window *w);
static void window_guest_overview_resize(rct_window *w);
static void window_guest_overview_mouse_up(rct_window *w, rct_widgetindex widgetIndex);
static void window_guest_overview_paint(rct_window *w, rct_drawpixelinfo *dpi);
static void window_guest_overview_invalidate(rct_window *w);
static void window_guest_overview_viewport_rotate(rct_window *w);
static void window_guest_overview_update(rct_window* w);
static void window_guest_overview_text_input(rct_window *w, rct_widgetindex widgetIndex, char *text);
static void window_guest_overview_tool_update(rct_window* w, rct_widgetindex widgetIndex, sint32 x, sint32 y);
static void window_guest_overview_tool_down(rct_window* w, rct_widgetindex widgetIndex, sint32 x, sint32 y);
static void window_guest_overview_tool_abort(rct_window *w, rct_widgetindex widgetIndex);

static void window_guest_mouse_up(rct_window *w, rct_widgetindex widgetIndex);
static void window_guest_unknown_05(rct_window *w);

static void window_guest_stats_resize(rct_window *w);
static void window_guest_stats_update(rct_window *w);
static void window_guest_stats_invalidate(rct_window *w);
static void window_guest_stats_paint(rct_window *w, rct_drawpixelinfo *dpi);

static void window_guest_rides_resize(rct_window *w);
static void window_guest_rides_update(rct_window *w);
static void window_guest_rides_tooltip(rct_window* w, rct_widgetindex widgetIndex, rct_string_id *stringId);
static void window_guest_rides_scroll_get_size(rct_window *w, sint32 scrollIndex, sint32 *width, sint32 *height);
static void window_guest_rides_scroll_mouse_down(rct_window *w, sint32 scrollIndex, sint32 x, sint32 y);
static void window_guest_rides_scroll_mouse_over(rct_window *w, sint32 scrollIndex, sint32 x, sint32 y);
static void window_guest_rides_invalidate(rct_window *w);
static void window_guest_rides_paint(rct_window *w, rct_drawpixelinfo *dpi);
static void window_guest_rides_scroll_paint(rct_window *w, rct_drawpixelinfo *dpi, sint32 scrollIndex);

static void window_guest_finance_resize(rct_window *w);
static void window_guest_finance_update(rct_window *w);
static void window_guest_finance_invalidate(rct_window *w);
static void window_guest_finance_paint(rct_window *w, rct_drawpixelinfo *dpi);

static void window_guest_thoughts_resize(rct_window *w);
static void window_guest_thoughts_update(rct_window *w);
static void window_guest_thoughts_invalidate(rct_window *w);
static void window_guest_thoughts_paint(rct_window *w, rct_drawpixelinfo *dpi);

static void window_guest_inventory_resize(rct_window *w);
static void window_guest_inventory_update(rct_window *w);
static void window_guest_inventory_invalidate(rct_window *w);
static void window_guest_inventory_paint(rct_window *w, rct_drawpixelinfo *dpi);

static rct_window_event_list window_guest_overview_events = {
    window_guest_overview_close,
    window_guest_overview_mouse_up,
    window_guest_overview_resize,
    nullptr,
    nullptr,
    nullptr,
    window_guest_overview_update,
    nullptr,
    nullptr,
    window_guest_overview_tool_update,
    window_guest_overview_tool_down,
    nullptr,
    nullptr,
    window_guest_overview_tool_abort,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    window_guest_overview_text_input,
    window_guest_overview_viewport_rotate,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    window_guest_overview_invalidate,
    window_guest_overview_paint,
    nullptr
};

static rct_window_event_list window_guest_stats_events = {
    nullptr,
    window_guest_mouse_up,
    window_guest_stats_resize,
    nullptr,
    nullptr,
    window_guest_unknown_05,
    window_guest_stats_update,
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
    window_guest_stats_invalidate,
    window_guest_stats_paint,
    nullptr
};

static rct_window_event_list window_guest_rides_events = {
    nullptr,
    window_guest_mouse_up,
    window_guest_rides_resize,
    nullptr,
    nullptr,
    window_guest_unknown_05,
    window_guest_rides_update,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    window_guest_rides_scroll_get_size,
    window_guest_rides_scroll_mouse_down,
    nullptr,
    window_guest_rides_scroll_mouse_over,
    nullptr,
    nullptr,
    nullptr,
    window_guest_rides_tooltip,
    nullptr,
    nullptr,
    window_guest_rides_invalidate,
    window_guest_rides_paint,
    window_guest_rides_scroll_paint
};

static rct_window_event_list window_guest_finance_events = {
    nullptr,
    window_guest_mouse_up,
    window_guest_finance_resize,
    nullptr,
    nullptr,
    window_guest_unknown_05,
    window_guest_finance_update,
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
    window_guest_finance_invalidate,
    window_guest_finance_paint,
    nullptr
};

static rct_window_event_list window_guest_thoughts_events = {
    nullptr,
    window_guest_mouse_up,
    window_guest_thoughts_resize,
    nullptr,
    nullptr,
    window_guest_unknown_05,
    window_guest_thoughts_update,
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
    window_guest_thoughts_invalidate,
    window_guest_thoughts_paint,
    nullptr
};

static rct_window_event_list window_guest_inventory_events = {
    nullptr,
    window_guest_mouse_up,
    window_guest_inventory_resize,
    nullptr,
    nullptr,
    window_guest_unknown_05,
    window_guest_inventory_update,
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
    window_guest_inventory_invalidate,
    window_guest_inventory_paint,
    nullptr
};

// 0x981D24
static rct_window_event_list *window_guest_page_events[] = {
    &window_guest_overview_events,
    &window_guest_stats_events,
    &window_guest_rides_events,
    &window_guest_finance_events,
    &window_guest_thoughts_events,
    &window_guest_inventory_events
};

void window_guest_set_colours();

// 0x981D3C
uint32 window_guest_page_enabled_widgets[] = {
    (1 << WIDX_CLOSE) |
    (1 << WIDX_TAB_1) |
    (1 << WIDX_TAB_2) |
    (1 << WIDX_TAB_3) |
    (1 << WIDX_TAB_4) |
    (1 << WIDX_TAB_5) |
    (1 << WIDX_TAB_6) |
    (1 << WIDX_RENAME)|
    (1 << WIDX_PICKUP)|
    (1 << WIDX_LOCATE)|
    (1 << WIDX_TRACK),

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
    (1 << WIDX_RIDE_SCROLL),

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
    (1 << WIDX_TAB_6)
};

/**
 *
 *  rct2: 0x006989E9
 *
 */
void window_guest_open(rct_peep* peep){

    if (peep->type == PEEP_TYPE_STAFF){
        window_staff_open(peep);
        return;
    }

    rct_window* window;

    window = window_bring_to_front_by_number(WC_PEEP, peep->sprite_index);
    if (window == nullptr){
        window = window_create_auto_pos(192, 157, &window_guest_overview_events, WC_PEEP, WF_RESIZABLE);
        window->widgets = window_guest_overview_widgets;
        window->enabled_widgets = window_guest_page_enabled_widgets[0];
        window->number = peep->sprite_index;
        window->page = 0;
        window->viewport_focus_coordinates.y = 0;
        window->frame_no = 0;
        window->list_information_type = 0;
        window->picked_peep_frame = 0;
        window->highlighted_item = 0;
        window_guest_disable_widgets(window);
        window->min_width = 192;
        window->min_height = 157;
        window->max_width = 500;
        window->max_height = 450;
        window->no_list_items = 0;
        window->selected_list_item = -1;

        window->viewport_focus_coordinates.y = -1;
    }

    window->page = 0;
    window_invalidate(window);

    window->widgets = window_guest_page_widgets[WINDOW_GUEST_OVERVIEW];
    window->enabled_widgets = window_guest_page_enabled_widgets[WINDOW_GUEST_OVERVIEW];
    window->hold_down_widgets = 0;
    window->event_handlers = window_guest_page_events[WINDOW_GUEST_OVERVIEW];
    window->pressed_widgets = 0;

    window_guest_disable_widgets(window);
    window_init_scroll_widgets(window);
    window_guest_viewport_init(window);
}

/**
 * Disables the finance tab when no money.
 * Disables peep pickup when in certain no pickup states.
 *  rct2: 0x006987A6
 */
void window_guest_disable_widgets(rct_window* w){
    rct_peep* peep = &get_sprite(w->number)->peep;
    uint64 disabled_widgets = 0;

    if (peep_can_be_picked_up(peep)){
        if (w->disabled_widgets & (1 << WIDX_PICKUP))
            window_invalidate(w);
    }
    else{
        disabled_widgets = (1 << WIDX_PICKUP);
        if (!(w->disabled_widgets & (1 << WIDX_PICKUP)))
            window_invalidate(w);
    }
    if (gParkFlags & PARK_FLAGS_NO_MONEY){
        disabled_widgets |= (1 << WIDX_TAB_4); // Disable finance tab if no money
    }
    w->disabled_widgets = disabled_widgets;
}

/**
 *
 *  rct2: 0x00696A75
 */
void window_guest_overview_close(rct_window *w)
{
    if (input_test_flag(INPUT_FLAG_TOOL_ACTIVE)) {
        if (w->classification == gCurrentToolWidget.window_classification &&
            w->number == gCurrentToolWidget.window_number)
            tool_cancel();
    }
}

/**
 *
 *  rct2: 0x00696FBE
 */
void window_guest_overview_resize(rct_window *w){
    window_guest_disable_widgets(w);
    window_event_invalidate_call(w);

    widget_invalidate(w, WIDX_MARQUEE);

    window_set_resize(w, 192, 159, 500, 450);

    rct_viewport* view = w->viewport;

    if (view){
        if ((w->width - 30) == view->width){
            if ((w->height - 72) == view->height){
                window_guest_viewport_init(w);
                return;
            }
        }
        uint8 zoom_amount = 1 << view->zoom;
        view->width = w->width - 30;
        view->height = w->height - 72;
        view->view_width = view->width / zoom_amount;
        view->view_height = view->height / zoom_amount;
    }
    window_guest_viewport_init(w);
}

void game_command_callback_pickup_guest(sint32 eax, sint32 ebx, sint32 ecx, sint32 edx, sint32 esi, sint32 edi, sint32 ebp)
{
    switch(ecx){
    case 0:{
        sint32 peepnum = eax;
        rct_window* w = window_find_by_number(WC_PEEP, peepnum);
        if (w) {
            tool_set(w, WIDX_PICKUP, TOOL_PICKER);
        }
        }break;
    case 2:
        if (ebx == 0) {
            tool_cancel();
            gPickupPeepImage = UINT32_MAX;
        }
        break;
    }
}

/**
 *
 *  rct2: 0x00696A06
 */
void window_guest_overview_mouse_up(rct_window *w, rct_widgetindex widgetIndex)
{
    rct_peep* peep = GET_PEEP(w->number);

    switch(widgetIndex){
    case WIDX_CLOSE:
        window_close(w);
        break;
    case WIDX_TAB_1:
    case WIDX_TAB_2:
    case WIDX_TAB_3:
    case WIDX_TAB_4:
    case WIDX_TAB_5:
    case WIDX_TAB_6:
        window_guest_set_page(w, widgetIndex - WIDX_TAB_1);
        break;
    case WIDX_PICKUP:
        if (!peep_can_be_picked_up(peep)) {
            return;
        }
        w->picked_peep_old_x = peep->x;
        game_command_callback = game_command_callback_pickup_guest;
        game_do_command(w->number, GAME_COMMAND_FLAG_APPLY, 0, 0, GAME_COMMAND_PICKUP_GUEST, 0, 0);
        break;
    case WIDX_RENAME:
        window_text_input_open(w, widgetIndex, STR_GUEST_RENAME_TITLE, STR_GUEST_RENAME_PROMPT, peep->name_string_idx, peep->id, 32);
        break;
    case WIDX_LOCATE:
        window_scroll_to_viewport(w);
        break;
    case WIDX_TRACK:
        get_sprite(w->number)->peep.peep_flags ^= PEEP_FLAGS_TRACKING;
        break;
    }
}

/**
 *
 *  rct2: 0x696AA0
 */
void window_guest_set_page(rct_window* w, sint32 page){
    if (input_test_flag(INPUT_FLAG_TOOL_ACTIVE))
    {
        if(w->number == gCurrentToolWidget.window_number &&
           w->classification == gCurrentToolWidget.window_classification)
            tool_cancel();

    }
    sint32 listen = 0;
    if ( page == WINDOW_GUEST_OVERVIEW && w->page==WINDOW_GUEST_OVERVIEW && w->viewport){
        if(!(w->viewport->flags & VIEWPORT_FLAG_SOUND_ON))
            listen = 1;
    }


    w->page = page;
    w->frame_no = 0;
    w->no_list_items = 0;
    w->selected_list_item = -1;

    rct_viewport* viewport = w->viewport;
    w->viewport = 0;
    if (viewport){
        viewport->width = 0;
    }

    w->enabled_widgets = window_guest_page_enabled_widgets[page];
    w->hold_down_widgets = 0;
    w->event_handlers = window_guest_page_events[page];
    w->pressed_widgets = 0;
    w->widgets = window_guest_page_widgets[page];
    window_guest_disable_widgets(w);
    window_invalidate(w);
    window_event_resize_call(w);
    window_event_invalidate_call(w);
    window_init_scroll_widgets(w);
    window_invalidate(w);

    if (listen && w->viewport) w->viewport->flags |= VIEWPORT_FLAG_SOUND_ON;
}

void window_guest_overview_viewport_rotate(rct_window *w)
{
    window_guest_viewport_init(w);
}

/**
 *
 *  rct2: 0x0069883C
 */
void window_guest_viewport_init(rct_window* w){
    if (w->page != WINDOW_GUEST_OVERVIEW) return;

    union{
        sprite_focus sprite;
        coordinate_focus coordinate;
    } focus = { 0 }; // The focus will be either a sprite or a coordinate.

    focus.sprite.sprite_id = w->number;

    rct_peep* peep = GET_PEEP(w->number);

    if (peep->state == PEEP_STATE_PICKED){
        focus.sprite.sprite_id = SPRITE_INDEX_NULL;
    }
    else{
        uint8 final_check = 1;
        if (peep->state == PEEP_STATE_ON_RIDE
            || peep->state == PEEP_STATE_ENTERING_RIDE
            || (peep->state == PEEP_STATE_LEAVING_RIDE && peep->x == SPRITE_LOCATION_NULL)){

            rct_ride *ride = get_ride(peep->current_ride);
            if (ride->lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK){
                rct_vehicle* train = GET_VEHICLE(ride->vehicles[peep->current_train]);
                sint32 car = peep->current_car;

                for (; car != 0; car--){
                    train = GET_VEHICLE(train->next_vehicle_on_train);
                }

                focus.sprite.sprite_id = train->sprite_index;
                final_check = 0;
            }
        }
        if (peep->x == SPRITE_LOCATION_NULL && final_check){
            rct_ride *ride = get_ride(peep->current_ride);
            sint32 x = ride->overall_view.x * 32 + 16;
            sint32 y = ride->overall_view.y * 32 + 16;
            sint32 height = map_element_height(x, y);
            height += 32;
            focus.coordinate.x = x;
            focus.coordinate.y = y;
            focus.coordinate.z = height;
            focus.sprite.type |= VIEWPORT_FOCUS_TYPE_COORDINATE;
        }
        else{
            focus.sprite.type |= VIEWPORT_FOCUS_TYPE_SPRITE | VIEWPORT_FOCUS_TYPE_COORDINATE;
            focus.sprite.pad_486 &= 0xFFFF;
        }
        focus.coordinate.rotation = get_current_rotation();
    }

    uint16 viewport_flags;

    if (w->viewport){
        // Check all combos, for now skipping y and rot
        if (focus.coordinate.x == w->viewport_focus_coordinates.x &&
            focus.coordinate.y == w->viewport_focus_coordinates.y &&
            focus.coordinate.z == w->viewport_focus_coordinates.z &&
            focus.coordinate.rotation == w->viewport_focus_coordinates.rotation)
            return;

        viewport_flags = w->viewport->flags;
        w->viewport->width = 0;
        w->viewport = 0;

        viewport_update_pointers();
    }
    else{
        viewport_flags = 0;
        if (gConfigGeneral.always_show_gridlines) {
            viewport_flags |= VIEWPORT_FLAG_GRIDLINES;
        }
    }

    window_event_invalidate_call(w);

    w->viewport_focus_coordinates.x = focus.coordinate.x;
    w->viewport_focus_coordinates.y = focus.coordinate.y;
    w->viewport_focus_coordinates.z = focus.coordinate.z;
    w->viewport_focus_coordinates.rotation = focus.coordinate.rotation;

    if (peep->state != PEEP_STATE_PICKED){
        if (!(w->viewport)){
            rct_widget* view_widget = &w->widgets[WIDX_VIEWPORT];

            sint32 x = view_widget->left + 1 + w->x;
            sint32 y = view_widget->top + 1 + w->y;
            sint32 width = view_widget->right - view_widget->left - 1;
            sint32 height = view_widget->bottom - view_widget->top - 1;

            viewport_create(w, x, y, width, height, 0, focus.coordinate.x, focus.coordinate.y, focus.coordinate.z, focus.sprite.type & VIEWPORT_FOCUS_TYPE_MASK, focus.sprite.sprite_id);
            w->flags |= WF_NO_SCROLLING;
            window_invalidate(w);
        }
    }

    if (w->viewport)
        w->viewport->flags = viewport_flags;
    window_invalidate(w);
}

/**
 *
 *  rct2: 0x6983dd
 * used by window_staff as well
 */
static void window_guest_overview_tab_paint(rct_window* w, rct_drawpixelinfo* dpi){
    if (w->disabled_widgets & (1<<WIDX_TAB_1))
        return;

    rct_widget* widget = &w->widgets[WIDX_TAB_1];
    sint32 width = widget->right - widget->left - 1;
    sint32 height = widget->bottom - widget->top - 1;
    sint32 x = widget->left + 1 + w->x;
    sint32 y = widget->top + 1 + w->y;
    if (w->page == WINDOW_GUEST_OVERVIEW) height++;

    rct_drawpixelinfo clip_dpi;
    if (!clip_drawpixelinfo(&clip_dpi, dpi, x, y, width, height)) {
        return;
    }

    x = 14;
    y = 20;

    rct_peep* peep = GET_PEEP(w->number);

    if (peep->type == PEEP_TYPE_STAFF && peep->staff_type == STAFF_TYPE_ENTERTAINER)
        y++;

    sint32 ebx = g_peep_animation_entries[peep->sprite_type].sprite_animation->base_image + 1;

    sint32 eax = 0;

    if (w->page == WINDOW_GUEST_OVERVIEW){
        eax = w->var_496;
        eax &= 0xFFFC;
    }
    ebx += eax;

    sint32 sprite_id = ebx | SPRITE_ID_PALETTE_COLOUR_2(peep->tshirt_colour, peep->trousers_colour);
    gfx_draw_sprite(&clip_dpi, sprite_id, x, y, 0);

    // If holding a balloon
    if (ebx >= 0x2A1D && ebx < 0x2A3D){
        ebx += 32;
        ebx |= SPRITE_ID_PALETTE_COLOUR_1(peep->balloon_colour);
        gfx_draw_sprite(&clip_dpi, ebx, x, y, 0);
    }

    // If holding umbrella
    if (ebx >= 0x2BBD && ebx < 0x2BDD){
        ebx += 32;
        ebx |= SPRITE_ID_PALETTE_COLOUR_1(peep->umbrella_colour);
        gfx_draw_sprite(&clip_dpi, ebx, x, y, 0);
    }

    // If wearing hat
    if (ebx >= 0x29DD && ebx < 0x29FD){
        ebx += 32;
        ebx |= SPRITE_ID_PALETTE_COLOUR_1(peep->hat_colour);
        gfx_draw_sprite(&clip_dpi, ebx, x, y, 0);
    }
}

/**
 *
 *  rct2: 0x69869b
 */
static void window_guest_stats_tab_paint(rct_window* w, rct_drawpixelinfo* dpi){
    if (w->disabled_widgets & (1 << WIDX_TAB_2))
        return;

    rct_widget* widget = &w->widgets[WIDX_TAB_2];
    sint32 x = widget->left + w->x;
    sint32 y = widget->top + w->y;

    rct_peep* peep = GET_PEEP(w->number);
    sint32 image_id = get_peep_face_sprite_large(peep);
    if (w->page == WINDOW_GUEST_STATS){
        // If currently viewing this tab animate tab
        // if it is very sick or angry.
        switch (image_id){
        case SPR_PEEP_LARGE_FACE_VERY_VERY_SICK_0:
            image_id += (w->frame_no / 4) & 0xF;
            break;
        case SPR_PEEP_LARGE_FACE_VERY_SICK_0:
            image_id += (w->frame_no / 8) & 0x3;
            break;
        case SPR_PEEP_LARGE_FACE_ANGRY_0:
            image_id += (w->frame_no / 8) & 0x3;
            break;
        }
    }
    gfx_draw_sprite(dpi, image_id, x, y, 0);
}

/**
 *
 *  rct2: 0x69861F
 */
static void window_guest_rides_tab_paint(rct_window* w, rct_drawpixelinfo* dpi){
    if (w->disabled_widgets & (1 << WIDX_TAB_3)) return;

    rct_widget* widget = &w->widgets[WIDX_TAB_3];
    sint32 x = widget->left + w->x;
    sint32 y = widget->top + w->y;

    sint32 image_id = SPR_TAB_RIDE_0;

    if ( w->page == WINDOW_GUEST_RIDES ){
        image_id += (w->frame_no / 4) & 0xF;
    }

    gfx_draw_sprite(dpi, image_id, x, y, 0);
}

/**
 *
 *  rct2: 0x698597
 */
static void window_guest_finance_tab_paint(rct_window* w, rct_drawpixelinfo* dpi){
    if (w->disabled_widgets & (1 << WIDX_TAB_4)) return;

    rct_widget* widget = &w->widgets[WIDX_TAB_4];
    sint32 x = widget->left + w->x;
    sint32 y = widget->top + w->y;

    sint32 image_id = SPR_TAB_FINANCES_SUMMARY_0;

    if ( w->page == WINDOW_GUEST_FINANCE ){
        image_id += (w->frame_no / 2) & 0x7;
    }

    gfx_draw_sprite(dpi, image_id, x, y, 0);
}

/**
 *
 *  rct2: 0x6985D8
 */
static void window_guest_thoughts_tab_paint(rct_window* w, rct_drawpixelinfo* dpi){
    if (w->disabled_widgets & (1 << WIDX_TAB_5)) return;

    rct_widget* widget = &w->widgets[WIDX_TAB_5];
    sint32 x = widget->left + w->x;
    sint32 y = widget->top + w->y;

    sint32 image_id = SPR_TAB_THOUGHTS_0;

    if ( w->page == WINDOW_GUEST_THOUGHTS ){
        image_id += (w->frame_no / 2) & 0x7;
    }

    gfx_draw_sprite(dpi, image_id, x, y, 0);
}

/**
 *
 *  rct2: 0x698661
 */
static void window_guest_inventory_tab_paint(rct_window* w, rct_drawpixelinfo* dpi){
    if (w->disabled_widgets & (1 << WIDX_TAB_6)) return;

    rct_widget* widget = &w->widgets[WIDX_TAB_6];
    sint32 x = widget->left + w->x;
    sint32 y = widget->top + w->y;

    sint32 image_id = SPR_TAB_GUEST_INVENTORY;

    gfx_draw_sprite(dpi, image_id, x, y, 0);
}

/**
 *
 *  rct2: 0x696887
 */
void window_guest_overview_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
    window_draw_widgets(w, dpi);
    window_guest_overview_tab_paint(w, dpi);
    window_guest_stats_tab_paint(w, dpi);
    window_guest_rides_tab_paint(w, dpi);
    window_guest_finance_tab_paint(w, dpi);
    window_guest_thoughts_tab_paint(w, dpi);
    window_guest_inventory_tab_paint(w, dpi);

    // Draw the viewport no sound sprite
    if (w->viewport){
        window_draw_viewport(dpi, w);
        rct_viewport* viewport = w->viewport;
        if (viewport->flags & VIEWPORT_FLAG_SOUND_ON){
            gfx_draw_sprite(dpi, SPR_HEARING_VIEWPORT, w->x + 2, w->y + 2, 0);
        }
    }

    // Draw the centred label
    uint32 argument1, argument2;
    rct_peep* peep = GET_PEEP(w->number);
    get_arguments_from_action(peep, &argument1, &argument2);
    set_format_arg(0, uint32, argument1);
    set_format_arg(4, uint32, argument2);
    rct_widget* widget = &w->widgets[WIDX_ACTION_LBL];
    sint32 x = (widget->left + widget->right) / 2 + w->x;
    sint32 y = w->y + widget->top - 1;
    sint32 width = widget->right - widget->left;
    gfx_draw_string_centred_clipped(dpi, STR_BLACK_STRING, gCommonFormatArgs, COLOUR_BLACK, x, y, width);

    // Draw the marquee thought
    widget = &w->widgets[WIDX_MARQUEE];
    width = widget->right - widget->left - 3;
    sint32 left = widget->left + 2 + w->x;
    sint32 top = widget->top + w->y;
    sint32 height = widget->bottom - widget->top;
    rct_drawpixelinfo dpi_marquee;
    if (!clip_drawpixelinfo(&dpi_marquee, dpi, left, top, width, height)) {
        return;
    }

    sint32 i = 0;
    for (; i < PEEP_MAX_THOUGHTS; ++i){
        if (peep->thoughts[i].type == PEEP_THOUGHT_TYPE_NONE){
            w->list_information_type = 0;
            return;
        }
        if (peep->thoughts[i].var_2 == 1){ // If a fresh thought
            break;
        }
    }
    if (i == PEEP_MAX_THOUGHTS){
        w->list_information_type = 0;
        return;
    }

    x = widget->right - widget->left - w->list_information_type;
    peep_thought_set_format_args(&peep->thoughts[i]);
    gfx_draw_string_left(&dpi_marquee, STR_WINDOW_COLOUR_2_STRINGID, gCommonFormatArgs, COLOUR_BLACK, x, 0);
}

/**
 *
 *  rct2: 0x696749
 */
void window_guest_overview_invalidate(rct_window *w)
{
    if (window_guest_page_widgets[w->page] != w->widgets){
        w->widgets = window_guest_page_widgets[w->page];
        window_init_scroll_widgets(w);
    }

    w->pressed_widgets &= ~(
        (1ULL << WIDX_TAB_1) |
        (1ULL << WIDX_TAB_2) |
        (1ULL << WIDX_TAB_3) |
        (1ULL << WIDX_TAB_4) |
        (1ULL << WIDX_TAB_5) |
        (1ULL << WIDX_TAB_6)
        );
    w->pressed_widgets |= 1ULL << (w->page + WIDX_TAB_1);

    rct_peep* peep = GET_PEEP(w->number);
    set_format_arg(0, rct_string_id, peep->name_string_idx);
    set_format_arg(2, uint32, peep->id);

    w->pressed_widgets &= ~(1<<WIDX_TRACK);
    if (peep->peep_flags & PEEP_FLAGS_TRACKING){
        w->pressed_widgets |= (1<<WIDX_TRACK);
    }

    window_guest_overview_widgets[WIDX_BACKGROUND].right = w->width - 1;
    window_guest_overview_widgets[WIDX_BACKGROUND].bottom = w->height - 1;

    window_guest_overview_widgets[WIDX_PAGE_BACKGROUND].right =w->width - 1;
    window_guest_overview_widgets[WIDX_PAGE_BACKGROUND].bottom = w->height - 1;

    window_guest_overview_widgets[WIDX_TITLE].right = w->width - 2;

    window_guest_overview_widgets[WIDX_CLOSE].left = w->width - 13;
    window_guest_overview_widgets[WIDX_CLOSE].right = w->width - 3;

    window_guest_overview_widgets[WIDX_VIEWPORT].right = w->width - 26;
    window_guest_overview_widgets[WIDX_VIEWPORT].bottom = w->height - 14;

    window_guest_overview_widgets[WIDX_ACTION_LBL].top = w->height - 12;
    window_guest_overview_widgets[WIDX_ACTION_LBL].bottom = w->height - 3;
    window_guest_overview_widgets[WIDX_ACTION_LBL].right = w->width - 24;

    window_guest_overview_widgets[WIDX_MARQUEE].right = w->width - 24;

    window_guest_overview_widgets[WIDX_PICKUP].right = w->width - 2;
    window_guest_overview_widgets[WIDX_RENAME].right = w->width - 2;
    window_guest_overview_widgets[WIDX_LOCATE].right = w->width - 2;
    window_guest_overview_widgets[WIDX_TRACK].right = w->width - 2;

    window_guest_overview_widgets[WIDX_PICKUP].left = w->width - 25;
    window_guest_overview_widgets[WIDX_RENAME].left = w->width - 25;
    window_guest_overview_widgets[WIDX_LOCATE].left = w->width - 25;
    window_guest_overview_widgets[WIDX_TRACK].left = w->width - 25;

    window_align_tabs(w, WIDX_TAB_1, WIDX_TAB_6);
}

/**
 *
 *  rct2: 0x696F45
 */
void window_guest_overview_update(rct_window* w){
    sint32 var_496 = w->var_496;
    var_496++;
    var_496 %= 24;
    w->var_496 = var_496;

    widget_invalidate(w, WIDX_TAB_1);
    widget_invalidate(w, WIDX_TAB_2);

    w->list_information_type += 2;

    if ((w->highlighted_item & 0xFFFF) == 0xFFFF)
        w->highlighted_item &= 0xFFFF0000;
    else
        w->highlighted_item++;

    // Disable peep watching thought for multiplayer as it's client specific
    if (network_get_mode() == NETWORK_MODE_NONE) {
        // Create the "I have the strangest feeling I am being watched thought"
        if ((w->highlighted_item & 0xFFFF) >= 3840) {
            if (!(w->highlighted_item & 0x3FF)) {
                sint32 random = util_rand() & 0xFFFF;
                if (random <= 0x2AAA) {
                    rct_peep* peep = GET_PEEP(w->number);
                    peep_insert_new_thought(peep, PEEP_THOUGHT_TYPE_WATCHED, 0xFF);
                }
            }
        }
    }
}

/* rct2: 0x696A6A */
void window_guest_overview_text_input(rct_window *w, rct_widgetindex widgetIndex, char *text)
{
    if (widgetIndex != WIDX_RENAME)
        return;

    if (text == nullptr)
        return;

    gGameCommandErrorTitle = STR_CANT_NAME_GUEST;
    game_do_command(1, GAME_COMMAND_FLAG_APPLY, w->number, *((sint32*)(text + 0)), GAME_COMMAND_SET_GUEST_NAME, *((sint32*)(text + 8)), *((sint32*)(text + 4)));
    game_do_command(2, GAME_COMMAND_FLAG_APPLY, w->number, *((sint32*)(text + 12)), GAME_COMMAND_SET_GUEST_NAME, *((sint32*)(text + 20)), *((sint32*)(text + 16)));
    game_do_command(0, GAME_COMMAND_FLAG_APPLY, w->number, *((sint32*)(text + 24)), GAME_COMMAND_SET_GUEST_NAME, *((sint32*)(text + 32)), *((sint32*)(text + 28)));
}

/**
 *
 *  rct2: 0x696A5F
 */
void window_guest_overview_tool_update(rct_window* w, rct_widgetindex widgetIndex, sint32 x, sint32 y)
{
    if (widgetIndex != WIDX_PICKUP)
        return;

    map_invalidate_selection_rect();

    gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE;

    sint32 map_x, map_y;
    footpath_get_coordinates_from_pos(x, y + 16, &map_x, &map_y, nullptr, nullptr);
    if (map_x != MAP_LOCATION_NULL) {
        gMapSelectFlags |= MAP_SELECT_FLAG_ENABLE;
        gMapSelectType = MAP_SELECT_TYPE_FULL;
        gMapSelectPositionA.x = map_x;
        gMapSelectPositionB.x = map_x;
        gMapSelectPositionA.y = map_y;
        gMapSelectPositionB.y = map_y;
        map_invalidate_selection_rect();
    }

    gPickupPeepImage = UINT32_MAX;

    sint32 interactionType;
    get_map_coordinates_from_pos(x, y, VIEWPORT_INTERACTION_MASK_NONE, nullptr, nullptr, &interactionType, nullptr, nullptr);
    if (interactionType == VIEWPORT_INTERACTION_ITEM_NONE)
        return;

    x--;
    y += 16;
    gPickupPeepX = x;
    gPickupPeepY = y;
    w->picked_peep_frame++;
    if (w->picked_peep_frame >= 48) {
        w->picked_peep_frame = 0;
    }

    rct_peep* peep;
    peep = GET_PEEP(w->number);

    uint32 imageId = g_peep_animation_entries[peep->sprite_type].sprite_animation[PEEP_ACTION_SPRITE_TYPE_UI].base_image;
    imageId += w->picked_peep_frame >> 2;

    imageId |= (peep->tshirt_colour << 19) | (peep->trousers_colour << 24) | IMAGE_TYPE_REMAP | IMAGE_TYPE_REMAP_2_PLUS;
    gPickupPeepImage = imageId;
}

/**
 *
 *  rct2: 0x696A54
 */
void window_guest_overview_tool_down(rct_window* w, rct_widgetindex widgetIndex, sint32 x, sint32 y)
{
    if (widgetIndex != WIDX_PICKUP)
        return;

    sint32 dest_x, dest_y;
    rct_map_element* mapElement;
    footpath_get_coordinates_from_pos(x, y + 16, &dest_x, &dest_y, nullptr, &mapElement);

    if (dest_x == MAP_LOCATION_NULL)
        return;

    game_command_callback = game_command_callback_pickup_guest;
    game_do_command(w->number, GAME_COMMAND_FLAG_APPLY, 2, mapElement->base_height, GAME_COMMAND_PICKUP_GUEST, dest_x, dest_y);
}

/**
 *
 *  rct2: 0x696A49
 */
void window_guest_overview_tool_abort(rct_window *w, rct_widgetindex widgetIndex)
{
    if (widgetIndex != WIDX_PICKUP)
        return;

    game_do_command(w->number, GAME_COMMAND_FLAG_APPLY, 1, 0, GAME_COMMAND_PICKUP_GUEST, w->picked_peep_old_x, 0);
}

/**
 * This is a combination of 5 functions that were identical
 *  rct2: 0x69744F, 0x697795, 0x697BDD, 0x697E18, 0x698279
 */
void window_guest_mouse_up(rct_window *w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex){
    case WIDX_CLOSE:
        window_close(w);
        break;
    case WIDX_TAB_1:
    case WIDX_TAB_2:
    case WIDX_TAB_3:
    case WIDX_TAB_4:
    case WIDX_TAB_5:
    case WIDX_TAB_6:
        window_guest_set_page(w, widgetIndex - WIDX_TAB_1);
        break;
    }
}

/**
 *
 *  rct2: 0x697488
 */
void window_guest_stats_resize(rct_window *w)
{
    window_set_resize(w, 192, 162, 192, 162);
}

/**
 * This is a combination of 5 functions that were identical
 *  rct2: 0x6974ED, 0x00697959, 0x00697C7B, 0x00697ED2, 0x00698333
 */
void window_guest_unknown_05(rct_window *w)
{
    widget_invalidate(w, WIDX_TAB_1);
}

/**
 *
 *  rct2: 0x69746A
 */
void window_guest_stats_update(rct_window *w)
{
    w->frame_no++;
    rct_peep* peep = GET_PEEP(w->number);
    peep->window_invalidate_flags &= ~PEEP_INVALIDATE_PEEP_STATS;

    window_invalidate(w);
}

/**
 *
 *  rct2: 0x69707D
 */
void window_guest_stats_invalidate(rct_window *w)
{
    if (w->widgets != window_guest_page_widgets[w->page]) {
        w->widgets = window_guest_page_widgets[w->page];
        window_init_scroll_widgets(w);
    }

    w->pressed_widgets |= 1ULL << (w->page + WIDX_TAB_1);

    rct_peep* peep = GET_PEEP(w->number);
    set_format_arg(0, rct_string_id, peep->name_string_idx);
    set_format_arg(2, uint32, peep->id);

    window_guest_stats_widgets[WIDX_BACKGROUND].right = w->width - 1;
    window_guest_stats_widgets[WIDX_BACKGROUND].bottom = w->height - 1;

    window_guest_stats_widgets[WIDX_PAGE_BACKGROUND].right = w->width - 1;
    window_guest_stats_widgets[WIDX_PAGE_BACKGROUND].bottom = w->height - 1;

    window_guest_stats_widgets[WIDX_TITLE].right = w->width - 2;

    window_guest_stats_widgets[WIDX_CLOSE].left = w->width - 13;
    window_guest_stats_widgets[WIDX_CLOSE].right = w->width - 3;

    window_align_tabs(w, WIDX_TAB_1, WIDX_TAB_6);
}

/**
*
*  rct2: 0x0066ECC1
*
*  ebp: colour, contains flag BAR_BLINK for blinking
*/
static void window_guest_stats_bars_paint(sint32 value, sint32 x, sint32 y, rct_window *w, rct_drawpixelinfo *dpi, sint32 colour){
    value *= 0x76;
    value >>= 8;

    gfx_fill_rect_inset(dpi, x + 0x3A, y + 1, x + 0x3A + 0x79, y + 9, w->colours[1], INSET_RECT_F_30);

    sint32 blink_flag = colour & BAR_BLINK;
    colour &= ~BAR_BLINK;

    if (!blink_flag ||
        game_is_paused() ||
        (gCurrentTicks & 8) == 0)
    {
        if (value <= 2)
            return;
        gfx_fill_rect_inset(dpi, x + 0x3C, y + 2, x + 0x3C + value - 1, y + 8, colour, 0);
    }

}

/**
 *
 *  rct2: 0x0069711D
 */
void window_guest_stats_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
    window_draw_widgets(w, dpi);
    window_guest_overview_tab_paint(w, dpi);
    window_guest_stats_tab_paint(w, dpi);
    window_guest_rides_tab_paint(w, dpi);
    window_guest_finance_tab_paint(w, dpi);
    window_guest_thoughts_tab_paint(w, dpi);
    window_guest_inventory_tab_paint(w, dpi);

    // ebx
    rct_peep* peep = GET_PEEP(w->number);

    // Not sure why this is not stats widgets
    // cx
    sint32 x = w->x + window_guest_rides_widgets[WIDX_PAGE_BACKGROUND].left + 4;
    // dx
    sint32 y = w->y + window_guest_rides_widgets[WIDX_PAGE_BACKGROUND].top + 4;

    // Happiness
    gfx_draw_string_left(dpi, STR_GUEST_STAT_HAPPINESS_LABEL, gCommonFormatArgs, COLOUR_BLACK, x, y);

    sint32 happiness = peep->happiness;
    if (happiness < 10)happiness = 10;
    sint32 ebp = COLOUR_BRIGHT_GREEN;
    if (happiness < 50){
        ebp |= BAR_BLINK;
    }
    window_guest_stats_bars_paint(happiness, x, y, w, dpi, ebp);

    // Energy
    y += 10;
    gfx_draw_string_left(dpi, STR_GUEST_STAT_ENERGY_LABEL, gCommonFormatArgs, COLOUR_BLACK, x, y);

    sint32 energy = ((peep->energy - 32) * 85) / 32;
    ebp = COLOUR_BRIGHT_GREEN;
    if (energy < 50){
        ebp |= BAR_BLINK;
    }
    if (energy < 10)energy = 10;
    window_guest_stats_bars_paint(energy, x, y, w, dpi, ebp);

    // Hunger
    y += 10;
    gfx_draw_string_left(dpi, STR_GUEST_STAT_HUNGER_LABEL, gCommonFormatArgs, COLOUR_BLACK, x, y);

    sint32 hunger = peep->hunger;
    if (hunger > 190) hunger = 190;

    hunger -= 32;
    if (hunger < 0) hunger = 0;
    hunger *= 51;
    hunger /= 32;
    hunger = 0xFF & ~hunger;

    ebp = COLOUR_BRIGHT_RED;
    if (hunger > 170){
        ebp |= BAR_BLINK;
    }
    window_guest_stats_bars_paint(hunger, x, y, w, dpi, ebp);

    // Thirst
    y += 10;
    gfx_draw_string_left(dpi, STR_GUEST_STAT_THIRST_LABEL, gCommonFormatArgs, COLOUR_BLACK, x, y);

    sint32 thirst = peep->thirst;
    if (thirst > 190) thirst = 190;

    thirst -= 32;
    if (thirst < 0) thirst = 0;
    thirst *= 51;
    thirst /= 32;
    thirst = 0xFF & ~thirst;

    ebp = COLOUR_BRIGHT_RED;
    if (thirst > 170){
        ebp |= BAR_BLINK;
    }
    window_guest_stats_bars_paint(thirst, x, y, w, dpi, ebp);

    // Nausea
    y += 10;
    gfx_draw_string_left(dpi, STR_GUEST_STAT_NAUSEA_LABEL, gCommonFormatArgs, COLOUR_BLACK, x, y);

    sint32 nausea = peep->nausea - 32;

    if (nausea  < 0) nausea = 0;
    nausea *= 36;
    nausea /= 32;

    ebp = COLOUR_BRIGHT_RED;
    if (nausea > 120){
        ebp |= BAR_BLINK;
    }
    window_guest_stats_bars_paint(nausea, x, y, w, dpi, ebp);

    // Bathroom
    y += 10;
    gfx_draw_string_left(dpi, STR_GUEST_STAT_TOILET_LABEL, gCommonFormatArgs, COLOUR_BLACK, x, y);

    sint32 bathroom = peep->bathroom - 32;
    if (bathroom > 210) bathroom = 210;

    bathroom -= 32;
    if (bathroom < 0) bathroom = 0;
    bathroom *= 45;
    bathroom /= 32;

    ebp = COLOUR_BRIGHT_RED;
    if (bathroom > 160){
        ebp |= BAR_BLINK;
    }
    window_guest_stats_bars_paint(bathroom, x, y, w, dpi, ebp);

    // Time in park
    y += 11;
    if (peep->time_in_park != -1){
        sint32 eax = gScenarioTicks;
        eax -= peep->time_in_park;
        eax >>= 11;
        set_format_arg(0, uint16, eax & 0xFFFF);
        gfx_draw_string_left(dpi, STR_GUEST_STAT_TIME_IN_PARK, gCommonFormatArgs, COLOUR_BLACK, x, y);
    }

    y += 19;
    gfx_fill_rect_inset(dpi, x, y - 6, x + 179, y - 5, w->colours[1], INSET_RECT_FLAG_BORDER_INSET);

    // Preferred Ride
    gfx_draw_string_left(dpi, STR_GUEST_STAT_PREFERRED_RIDE, nullptr, COLOUR_BLACK, x, y);
    y += 10;

    // Intensity
    sint32 intensity = peep->intensity / 16;
    set_format_arg(0, uint16, intensity);
    sint32 string_id = STR_GUEST_STAT_PREFERRED_INTESITY_BELOW;
    if (peep->intensity & 0xF){
        set_format_arg(0, uint16, peep->intensity & 0xF);
        set_format_arg(2, uint16, intensity);
        string_id = STR_GUEST_STAT_PREFERRED_INTESITY_BETWEEN;
        if (intensity == 15) string_id = STR_GUEST_STAT_PREFERRED_INTESITY_ABOVE;
    }

    gfx_draw_string_left(dpi, string_id, gCommonFormatArgs, COLOUR_BLACK, x + 4, y);

    // Nausea tolerance
    static const rct_string_id nauseaTolerances[] = {
        STR_PEEP_STAT_NAUSEA_TOLERANCE_NONE,
        STR_PEEP_STAT_NAUSEA_TOLERANCE_LOW,
        STR_PEEP_STAT_NAUSEA_TOLERANCE_AVERAGE,
        STR_PEEP_STAT_NAUSEA_TOLERANCE_HIGH,
    };
    y += 10;
    sint32 nausea_tolerance = peep->nausea_tolerance & 0x3;
    set_format_arg(0, rct_string_id, nauseaTolerances[nausea_tolerance]);
    gfx_draw_string_left(dpi, STR_GUEST_STAT_NAUSEA_TOLERANCE, gCommonFormatArgs, COLOUR_BLACK, x, y);
}

/**
 *
 *  rct2: 0x006978F4
 */
void window_guest_rides_resize(rct_window *w)
{
    window_set_resize(w, 192, 128, 500, 400);
}

/**
 *
 *  rct2: 0x6977B0
 */
void window_guest_rides_update(rct_window *w)
{
    w->frame_no++;

    widget_invalidate(w, WIDX_TAB_2);
    widget_invalidate(w, WIDX_TAB_3);

    rct_peep* peep = GET_PEEP(w->number);

    // Every 2048 ticks do a full window_invalidate
    sint32 number_of_ticks = gScenarioTicks - peep->time_in_park;
    if (!(number_of_ticks & 0x7FF)) window_invalidate(w);

    uint8 curr_list_position = 0;
    for (uint8 ride_id = 0; ride_id < 255; ++ride_id){
        // Offset to the ride_id bit in peep_rides_been_on
        uint8 ride_id_bit = ride_id % 8;
        uint8 ride_id_offset = ride_id / 8;
        if (peep->rides_been_on[ride_id_offset] & (1 << ride_id_bit)){
            rct_ride* ride = get_ride(ride_id);
            if (gRideClassifications[ride->type] == RIDE_CLASS_RIDE){
                w->list_item_positions[curr_list_position] = ride_id;
                curr_list_position++;
            }
        }
    }
    // If there are new items
    if (w->no_list_items != curr_list_position){
        w->no_list_items = curr_list_position;
        window_invalidate(w);
    }
}

/**
 *
 *  rct2: 0x697844
 */
void window_guest_rides_tooltip(rct_window* w, rct_widgetindex widgetIndex, rct_string_id *stringId)
{
    set_format_arg(0, rct_string_id, STR_LIST);
}

/**
 *
 *  rct2: 0x69784E
 */
void window_guest_rides_scroll_get_size(rct_window *w, sint32 scrollIndex, sint32 *width, sint32 *height)
{
    *height = w->no_list_items * 10;

    if (w->selected_list_item != -1){
        w->selected_list_item = -1;
        window_invalidate(w);
    }

    sint32 visable_height = *height
        - window_guest_rides_widgets[WIDX_RIDE_SCROLL].bottom
        + window_guest_rides_widgets[WIDX_RIDE_SCROLL].top
        + 21;

    if (visable_height < 0) visable_height = 0;

    if (visable_height < w->scrolls[0].v_top){
        w->scrolls[0].v_top = visable_height;
        window_invalidate(w);
    }
}

/**
 *
 *  rct2: 0x006978CC
 */
void window_guest_rides_scroll_mouse_down(rct_window *w, sint32 scrollIndex, sint32 x, sint32 y)
{
    sint32 index;

    index = y / 10;
    if (index >= w->no_list_items) return;

    window_ride_main_open(w->list_item_positions[index]);
}

/**
 *
 *  rct2: 0x0069789C
 */
void window_guest_rides_scroll_mouse_over(rct_window *w, sint32 scrollIndex, sint32 x, sint32 y)
{
    sint32 index;

    index = y / 10;
    if (index >= w->no_list_items)return;

    if (index == w->selected_list_item)return;
    w->selected_list_item = index;

    window_invalidate(w);
}

/**
 *
 *  rct2: 0x0069757A
 */
void window_guest_rides_invalidate(rct_window *w)
{
    if (window_guest_page_widgets[w->page] != w->widgets){
        w->widgets = window_guest_page_widgets[w->page];
        window_init_scroll_widgets(w);
    }

    w->pressed_widgets |= 1ULL << (w->page + WIDX_TAB_1);

    rct_peep* peep = GET_PEEP(w->number);
    set_format_arg(0, uint16, peep->name_string_idx);
    set_format_arg(2, uint32, peep->id);

    window_guest_rides_widgets[WIDX_BACKGROUND].right = w->width - 1;
    window_guest_rides_widgets[WIDX_BACKGROUND].bottom = w->height - 1;

    window_guest_rides_widgets[WIDX_PAGE_BACKGROUND].right = w->width - 1;
    window_guest_rides_widgets[WIDX_PAGE_BACKGROUND].bottom = w->height - 1;

    window_guest_rides_widgets[WIDX_TITLE].right = w->width - 2;

    window_guest_rides_widgets[WIDX_CLOSE].left = w->width - 13;
    window_guest_rides_widgets[WIDX_CLOSE].right = w->width - 3;

    window_guest_rides_widgets[WIDX_RIDE_SCROLL].right = w->width - 4;
    window_guest_rides_widgets[WIDX_RIDE_SCROLL].bottom = w->height - 15;

    window_align_tabs(w, WIDX_TAB_1, WIDX_TAB_6);
}

/**
 *
 *  rct2: 0x00697637
 */
void window_guest_rides_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
    window_draw_widgets(w, dpi);
    window_guest_overview_tab_paint(w, dpi);
    window_guest_stats_tab_paint(w, dpi);
    window_guest_rides_tab_paint(w, dpi);
    window_guest_finance_tab_paint(w, dpi);
    window_guest_thoughts_tab_paint(w, dpi);
    window_guest_inventory_tab_paint(w, dpi);

    rct_peep* peep = GET_PEEP(w->number);

    // cx
    sint32 x = w->x + window_guest_rides_widgets[WIDX_PAGE_BACKGROUND].left + 2;
    // dx
    sint32 y = w->y + window_guest_rides_widgets[WIDX_PAGE_BACKGROUND].top + 2;

    gfx_draw_string_left(dpi, STR_GUEST_LABEL_RIDES_BEEN_ON, nullptr, COLOUR_BLACK, x, y);

    y = w->y + window_guest_rides_widgets[WIDX_PAGE_BACKGROUND].bottom - 12;

    sint32 ride_string_id = STR_PEEP_FAVOURITE_RIDE_NOT_AVAILABLE;
    sint32 ride_string_arguments = 0;
    if (peep->favourite_ride != 0xFF){
        rct_ride* ride = get_ride(peep->favourite_ride);
        ride_string_arguments = ride->name_arguments;
        ride_string_id = ride->name;
    }
    set_format_arg(0, rct_string_id, ride_string_id);
    set_format_arg(2, uint32, ride_string_arguments);

    gfx_draw_string_left_clipped(dpi, STR_FAVOURITE_RIDE, gCommonFormatArgs, COLOUR_BLACK, x, y, w->width - 14);
}

/**
 *
 *  rct2: 0x006976FC
 */
void window_guest_rides_scroll_paint(rct_window *w, rct_drawpixelinfo *dpi, sint32 scrollIndex)
{
    // ax
    sint32 left = dpi->x;
    // bx
    sint32 right = dpi->x + dpi->width - 1;
    // cx
    sint32 top = dpi->y;
    // dx
    sint32 bottom = dpi->y + dpi->height - 1;

    sint32 colour = ColourMapA[w->colours[1]].mid_light;
    gfx_fill_rect(dpi, left, top, right, bottom, colour);

    for (sint32 list_index = 0; list_index < w->no_list_items; list_index++){
        sint32 y = list_index * 10;
        sint32 string_format = STR_BLACK_STRING;
        if (list_index == w->selected_list_item){
            gfx_filter_rect(dpi, 0, y, 800, y + 9, PALETTE_DARKEN_1);
            string_format = STR_WINDOW_COLOUR_2_STRINGID;
        }
        rct_ride* ride = get_ride(w->list_item_positions[list_index]);

        gfx_draw_string_left(dpi, string_format, (void*)&ride->name, COLOUR_BLACK, 0, y - 1);
    }
}

/**
 *
 *  rct2: 0x00697C16
 */
void window_guest_finance_resize(rct_window *w)
{
    window_set_resize(w, 210, 134, 210, 134);
}

/**
 *
 *  rct2: 0x00697BF8
 */
void window_guest_finance_update(rct_window *w)
{
    w->frame_no++;

    widget_invalidate(w, WIDX_TAB_2);
    widget_invalidate(w, WIDX_TAB_4);
}

/**
 *
 *  rct2: 0x00697968
 */
void window_guest_finance_invalidate(rct_window *w)
{
    if (window_guest_page_widgets[w->page] != w->widgets){
        w->widgets = window_guest_page_widgets[w->page];
        window_init_scroll_widgets(w);
    }

    w->pressed_widgets |= 1ULL << (w->page + WIDX_TAB_1);

    rct_peep* peep = GET_PEEP(w->number);

    set_format_arg(0, rct_string_id, peep->name_string_idx);
    set_format_arg(2, uint32, peep->id);

    window_guest_finance_widgets[WIDX_BACKGROUND].right = w->width - 1;
    window_guest_finance_widgets[WIDX_BACKGROUND].bottom = w->height - 1;

    window_guest_finance_widgets[WIDX_PAGE_BACKGROUND].right = w->width - 1;
    window_guest_finance_widgets[WIDX_PAGE_BACKGROUND].bottom = w->height - 1;

    window_guest_finance_widgets[WIDX_TITLE].right = w->width - 2;

    window_guest_finance_widgets[WIDX_CLOSE].left = w->width - 13;
    window_guest_finance_widgets[WIDX_CLOSE].right = w->width - 3;

    window_align_tabs(w, WIDX_TAB_1, WIDX_TAB_6);
}

/**
 *
 *  rct2: 0x00697A08
 */
void window_guest_finance_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
    window_draw_widgets(w, dpi);
    window_guest_overview_tab_paint(w, dpi);
    window_guest_stats_tab_paint(w, dpi);
    window_guest_rides_tab_paint(w, dpi);
    window_guest_finance_tab_paint(w, dpi);
    window_guest_thoughts_tab_paint(w, dpi);
    window_guest_inventory_tab_paint(w, dpi);

    rct_peep* peep = GET_PEEP(w->number);

    // cx
    sint32 x = w->x + window_guest_finance_widgets[WIDX_PAGE_BACKGROUND].left + 4;
    // dx
    sint32 y = w->y + window_guest_finance_widgets[WIDX_PAGE_BACKGROUND].top + 4;

    // Cash in pocket
    set_format_arg(0, money32, peep->cash_in_pocket);
    gfx_draw_string_left(dpi, STR_GUEST_STAT_CASH_IN_POCKET, gCommonFormatArgs, COLOUR_BLACK, x, y);

    // Cash spent
    y += 10;
    set_format_arg(0, money32, peep->cash_spent);
    gfx_draw_string_left(dpi, STR_GUEST_STAT_CASH_SPENT, gCommonFormatArgs, COLOUR_BLACK, x, y);

    y += 20;
    gfx_fill_rect_inset(dpi, x, y - 6, x + 179, y - 5, w->colours[1], INSET_RECT_FLAG_BORDER_INSET);

    // Paid to enter
    set_format_arg(0, money32, peep->paid_to_enter);
    gfx_draw_string_left(dpi, STR_GUEST_EXPENSES_ENTRANCE_FEE, gCommonFormatArgs, COLOUR_BLACK, x, y);

    // Paid on rides
    y += 10;
    set_format_arg(0, money32, peep->paid_on_rides);
    set_format_arg(4, uint16, peep->no_of_rides);
    if (peep->no_of_rides != 1){
        gfx_draw_string_left(dpi, STR_GUEST_EXPENSES_RIDE_PLURAL, gCommonFormatArgs, COLOUR_BLACK, x, y);
    }
    else{
        gfx_draw_string_left(dpi, STR_GUEST_EXPENSES_RIDE, gCommonFormatArgs, COLOUR_BLACK, x, y);
    }

    // Paid on food
    y += 10;
    set_format_arg(0, money32, peep->paid_on_food);
    set_format_arg(4, uint16, peep->no_of_food);
    if (peep->no_of_food != 1){
        gfx_draw_string_left(dpi, STR_GUEST_EXPENSES_FOOD_PLURAL, gCommonFormatArgs, COLOUR_BLACK, x, y);
    }
    else{
        gfx_draw_string_left(dpi, STR_GUEST_EXPENSES_FOOD, gCommonFormatArgs, COLOUR_BLACK, x, y);
    }

    // Paid on drinks
    y += 10;
    set_format_arg(0, money32, peep->paid_on_drink);
    set_format_arg(4, uint16, peep->no_of_drinks);
    if (peep->no_of_drinks != 1){
        gfx_draw_string_left(dpi, STR_GUEST_EXPENSES_DRINK_PLURAL, gCommonFormatArgs, COLOUR_BLACK, x, y);
    }
    else{
        gfx_draw_string_left(dpi, STR_GUEST_EXPENSES_DRINK, gCommonFormatArgs, COLOUR_BLACK, x, y);
    }

    // Paid on souvenirs
    y += 10;
    set_format_arg(0, money32, peep->paid_on_souvenirs);
    set_format_arg(4, uint16, peep->no_of_souvenirs);
    if (peep->no_of_souvenirs != 1){
        gfx_draw_string_left(dpi, STR_GUEST_EXPENSES_SOUVENIR_PLURAL, gCommonFormatArgs, COLOUR_BLACK, x, y);
    }
    else{
        gfx_draw_string_left(dpi, STR_GUEST_EXPENSES_SOUVENIR, gCommonFormatArgs, COLOUR_BLACK, x, y);
    }
}

/**
 *
 *  rct2: 0x00697E33
 */
void window_guest_thoughts_resize(rct_window *w)
{
    rct_peep* peep = GET_PEEP(w->number);
    if (peep->window_invalidate_flags & PEEP_INVALIDATE_PEEP_THOUGHTS){
        peep->window_invalidate_flags &= ~PEEP_INVALIDATE_PEEP_THOUGHTS;
        window_invalidate(w);
    }

    window_set_resize(w, 192, 159, 500, 450);
}

/**
 *
 *  rct2: 0x00697EB4
 */
void window_guest_thoughts_update(rct_window *w)
{
    w->frame_no++;

    widget_invalidate(w, WIDX_TAB_2);
    widget_invalidate(w, WIDX_TAB_5);
}

/**
 *
 *  rct2: 0x00697C8A
 */
void window_guest_thoughts_invalidate(rct_window *w)
{
    if (window_guest_page_widgets[w->page] != w->widgets){
        w->widgets = window_guest_page_widgets[w->page];
        window_init_scroll_widgets(w);
    }

    w->pressed_widgets |= 1ULL << (w->page + WIDX_TAB_1);

    rct_peep* peep = GET_PEEP(w->number);

    set_format_arg(0, rct_string_id, peep->name_string_idx);
    set_format_arg(2, uint32, peep->id);

    window_guest_thoughts_widgets[WIDX_BACKGROUND].right = w->width - 1;
    window_guest_thoughts_widgets[WIDX_BACKGROUND].bottom = w->height - 1;

    window_guest_thoughts_widgets[WIDX_PAGE_BACKGROUND].right = w->width - 1;
    window_guest_thoughts_widgets[WIDX_PAGE_BACKGROUND].bottom = w->height - 1;

    window_guest_thoughts_widgets[WIDX_TITLE].right = w->width - 2;

    window_guest_thoughts_widgets[WIDX_CLOSE].left = w->width - 13;
    window_guest_thoughts_widgets[WIDX_CLOSE].right = w->width - 3;

    window_align_tabs(w, WIDX_TAB_1, WIDX_TAB_6);
}

/**
 *
 *  rct2: 0x00697D2A
 */
void window_guest_thoughts_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
    window_draw_widgets(w, dpi);
    window_guest_overview_tab_paint(w, dpi);
    window_guest_stats_tab_paint(w, dpi);
    window_guest_rides_tab_paint(w, dpi);
    window_guest_finance_tab_paint(w, dpi);
    window_guest_thoughts_tab_paint(w, dpi);
    window_guest_inventory_tab_paint(w, dpi);

    rct_peep* peep = GET_PEEP(w->number);

    // cx
    sint32 x = w->x + window_guest_thoughts_widgets[WIDX_PAGE_BACKGROUND].left + 4;
    // dx
    sint32 y = w->y + window_guest_thoughts_widgets[WIDX_PAGE_BACKGROUND].top + 4;

    gfx_draw_string_left(dpi, STR_GUEST_RECENT_THOUGHTS_LABEL, nullptr, COLOUR_BLACK, x, y);

    y += 10;
    for (rct_peep_thought* thought = peep->thoughts; thought < &peep->thoughts[PEEP_MAX_THOUGHTS]; ++thought){
        if (thought->type == PEEP_THOUGHT_TYPE_NONE) return;
        if (thought->var_2 == 0) continue;

        sint32 width = window_guest_thoughts_widgets[WIDX_PAGE_BACKGROUND].right
            - window_guest_thoughts_widgets[WIDX_PAGE_BACKGROUND].left
            - 8;

        peep_thought_set_format_args(thought);
        y += gfx_draw_string_left_wrapped(dpi, gCommonFormatArgs, x, y, width, STR_BLACK_STRING, COLOUR_BLACK);

        // If this is the last visible line end drawing.
        if (y > w->y + window_guest_thoughts_widgets[WIDX_PAGE_BACKGROUND].bottom - 32) return;
    }
}


/**
 *
 *  rct2: 0x00698294
 */
void window_guest_inventory_resize(rct_window *w)
{
    rct_peep* peep = GET_PEEP(w->number);
    if (peep->window_invalidate_flags & PEEP_INVALIDATE_PEEP_INVENTORY){
        peep->window_invalidate_flags &= ~PEEP_INVALIDATE_PEEP_INVENTORY;
        window_invalidate(w);
    }

    window_set_resize(w, 192, 159, 500, 450);
}

/**
 *
 *  rct2: 0x00698315
 */
void window_guest_inventory_update(rct_window *w)
{
    w->frame_no++;

    widget_invalidate(w, WIDX_TAB_2);
    widget_invalidate(w, WIDX_TAB_6);
}

/**
 *
 *  rct2: 0x00697EE1
 */
void window_guest_inventory_invalidate(rct_window *w)
{
    if (window_guest_page_widgets[w->page] != w->widgets){
        w->widgets = window_guest_page_widgets[w->page];
        window_init_scroll_widgets(w);
    }

    w->pressed_widgets |= 1ULL << (w->page + WIDX_TAB_1);

    rct_peep* peep = GET_PEEP(w->number);

    set_format_arg(0, rct_string_id, peep->name_string_idx);
    set_format_arg(2, uint32, peep->id);

    window_guest_inventory_widgets[WIDX_BACKGROUND].right = w->width - 1;
    window_guest_inventory_widgets[WIDX_BACKGROUND].bottom = w->height - 1;

    window_guest_inventory_widgets[WIDX_PAGE_BACKGROUND].right = w->width - 1;
    window_guest_inventory_widgets[WIDX_PAGE_BACKGROUND].bottom = w->height - 1;

    window_guest_inventory_widgets[WIDX_TITLE].right = w->width - 2;

    window_guest_inventory_widgets[WIDX_CLOSE].left = w->width - 13;
    window_guest_inventory_widgets[WIDX_CLOSE].right = w->width - 3;

    window_align_tabs(w, WIDX_TAB_1, WIDX_TAB_6);
}

static rct_string_id window_guest_inventory_format_item(rct_peep *peep, sint32 item)
{
    rct_ride *ride;

    // Default arguments
    set_format_arg(0, uint32, ShopItemImage[item]);
    set_format_arg(4, rct_string_id, ShopItemStringIds[item].display);
    set_format_arg(6, rct_string_id, gParkName);
    set_format_arg(8, uint32, gParkNameArgs);

    // Special overrides
    switch (item) {
    case SHOP_ITEM_BALLOON:
        set_format_arg(0, uint32, SPRITE_ID_PALETTE_COLOUR_1(peep->balloon_colour) | ShopItemImage[item]);
        break;
    case SHOP_ITEM_PHOTO:
        ride = get_ride(peep->photo1_ride_ref);
        set_format_arg(6, rct_string_id, ride->name);
        set_format_arg(8, uint32, ride->name_arguments);
        break;
    case SHOP_ITEM_UMBRELLA:
        set_format_arg(0, uint32, SPRITE_ID_PALETTE_COLOUR_1(peep->umbrella_colour) | ShopItemImage[item]);
        break;
    case SHOP_ITEM_VOUCHER:
        switch (peep->voucher_type) {
        case VOUCHER_TYPE_PARK_ENTRY_FREE:
            set_format_arg(6, rct_string_id, STR_PEEP_INVENTORY_VOUCHER_PARK_ENTRY_FREE);
            set_format_arg(8, rct_string_id, gParkName);
            set_format_arg(10, uint32, gParkNameArgs);
            break;
        case VOUCHER_TYPE_RIDE_FREE:
            ride = get_ride(peep->voucher_arguments);
            set_format_arg(6, rct_string_id, STR_PEEP_INVENTORY_VOUCHER_RIDE_FREE);
            set_format_arg(8, rct_string_id, ride->name);
            set_format_arg(10, uint32, ride->name_arguments);
            break;
        case VOUCHER_TYPE_PARK_ENTRY_HALF_PRICE:
            set_format_arg(6, rct_string_id, STR_PEEP_INVENTORY_VOUCHER_PARK_ENTRY_HALF_PRICE);
            set_format_arg(8, rct_string_id, gParkName);
            set_format_arg(10, uint32, gParkNameArgs);
            break;
        case VOUCHER_TYPE_FOOD_OR_DRINK_FREE:
            set_format_arg(6, rct_string_id, STR_PEEP_INVENTORY_VOUCHER_FOOD_OR_DRINK_FREE);
            set_format_arg(8, rct_string_id, ShopItemStringIds[peep->voucher_arguments].singular);
            break;
        }
        break;
    case SHOP_ITEM_HAT:
        set_format_arg(0, uint32, SPRITE_ID_PALETTE_COLOUR_1(peep->hat_colour) | ShopItemImage[item]);
        break;
    case SHOP_ITEM_TSHIRT:
        set_format_arg(0, uint32, SPRITE_ID_PALETTE_COLOUR_1(peep->tshirt_colour) | ShopItemImage[item]);
        break;
    case SHOP_ITEM_PHOTO2:
        ride = get_ride(peep->photo2_ride_ref);
        set_format_arg(6, rct_string_id, ride->name);
        set_format_arg(8, uint32, ride->name_arguments);
        break;
    case SHOP_ITEM_PHOTO3:
        ride = get_ride(peep->photo3_ride_ref);
        set_format_arg(6, rct_string_id, ride->name);
        set_format_arg(8, uint32, ride->name_arguments);
        break;
    case SHOP_ITEM_PHOTO4:
        ride = get_ride(peep->photo4_ride_ref);
        set_format_arg(6, rct_string_id, ride->name);
        set_format_arg(8, uint32, ride->name_arguments);
        break;
    }

    return STR_GUEST_ITEM_FORMAT;
}

/**
 *
 *  rct2: 0x00697F81
 */
void window_guest_inventory_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
    window_draw_widgets(w, dpi);
    window_guest_overview_tab_paint(w, dpi);
    window_guest_stats_tab_paint(w, dpi);
    window_guest_rides_tab_paint(w, dpi);
    window_guest_finance_tab_paint(w, dpi);
    window_guest_thoughts_tab_paint(w, dpi);
    window_guest_inventory_tab_paint(w, dpi);

    rct_peep *peep = GET_PEEP(w->number);

    rct_widget *pageBackgroundWidget = &window_guest_inventory_widgets[WIDX_PAGE_BACKGROUND];
    sint32 x = w->x + pageBackgroundWidget->left + 4;
    sint32 y = w->y + pageBackgroundWidget->top + 2;
    sint32 itemNameWidth = pageBackgroundWidget->right - pageBackgroundWidget->left - 8;

    sint32 maxY = w->y + w->height - 22;
    sint32 numItems = 0;

    gfx_draw_string_left(dpi, STR_CARRYING, nullptr, COLOUR_BLACK, x, y);
    y += 10;

    for (sint32 item = 0; item < SHOP_ITEM_COUNT; item++) {
        if (y >= maxY) break;
        if (!peep_has_item(peep, item)) continue;

        rct_string_id stringId = window_guest_inventory_format_item(peep, item);
        y += gfx_draw_string_left_wrapped(dpi, gCommonFormatArgs, x, y, itemNameWidth, stringId, COLOUR_BLACK);
        numItems++;
    }

    if (numItems == 0) {
        gfx_draw_string_left(dpi, STR_NOTHING, nullptr, COLOUR_BLACK, x, y);
    }
}
