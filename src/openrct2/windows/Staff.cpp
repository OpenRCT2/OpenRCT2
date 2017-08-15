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

extern "C"
{
    #include "../game.h"
    #include "../interface/viewport.h"
    #include "../interface/widget.h"
    #include "../localisation/localisation.h"
    #include "../peep/staff.h"
    #include "../sprites.h"
    #include "../world/footpath.h"
    #include "../input.h"
    #include "dropdown.h"
    #include "../interface/themes.h"
}

#define WW 190
#define WH 180

enum WINDOW_STAFF_PAGE {
    WINDOW_STAFF_OVERVIEW,
    WINDOW_STAFF_OPTIONS,
    WINDOW_STAFF_STATISTICS,
};

enum WINDOW_STAFF_WIDGET_IDX {
    WIDX_BACKGROUND,
    WIDX_TITLE,
    WIDX_CLOSE,
    WIDX_RESIZE,
    WIDX_TAB_1,
    WIDX_TAB_2,
    WIDX_TAB_3,
    WIDX_TAB_4,
    WIDX_VIEWPORT,
    WIDX_BTM_LABEL,
    WIDX_PICKUP,
    WIDX_PATROL,
    WIDX_RENAME,
    WIDX_LOCATE,
    WIDX_FIRE,

    WIDX_CHECKBOX_1 = 8,
    WIDX_CHECKBOX_2,
    WIDX_CHECKBOX_3,
    WIDX_CHECKBOX_4,
    WIDX_COSTUME_BOX,
    WIDX_COSTUME_BTN,
};

validate_global_widx(WC_PEEP, WIDX_PATROL);

static rct_widget window_staff_overview_widgets[] = {
    { WWT_FRAME,    0, 0,       WW - 1,     0,          WH - 1, 0xFFFFFFFF,             STR_NONE },             // Panel / Background
    { WWT_CAPTION,  0, 1,       WW - 2,     1,          14,     STR_STRINGID,           STR_WINDOW_TITLE_TIP }, // Title
    { WWT_CLOSEBOX, 0, WW - 13, WW - 3,     2,          13,     STR_CLOSE_X,            STR_CLOSE_WINDOW_TIP }, // Close x button
    { WWT_RESIZE,   1, 0,       WW - 1,     43,         WH - 1, 0xFFFFFFFF,             STR_NONE },             // Resize
    { WWT_TAB,      1, 3,       33,         17,         43,     IMAGE_TYPE_REMAP | SPR_TAB,   STR_STAFF_OVERVIEW_TIP },// Tab 1
    { WWT_TAB,      1, 34,      64,         17,         43,     IMAGE_TYPE_REMAP | SPR_TAB,   STR_STAFF_OPTIONS_TIP}, // Tab 2
    { WWT_TAB,      1, 65,      95,         17,         43,     IMAGE_TYPE_REMAP | SPR_TAB,   STR_STAFF_STATS_TIP},   // Tab 3
    { WWT_TAB,      1, 96,      126,        17,         43,     IMAGE_TYPE_REMAP | SPR_TAB,   STR_NONE},              // Tab 4
    { WWT_VIEWPORT, 1, 3,       WW - 26,    47,         WH - 14,0xFFFFFFFF,             STR_NONE},              // Viewport
    { WWT_12,       1, 3,       WW - 26,    WH - 13,    WH - 3, 0xFFFFFFFF,             STR_NONE },             // Label at bottom of viewport
    { WWT_FLATBTN,  1, WW - 25, WW - 2,     45,         68,     SPR_PICKUP_BTN,         STR_PICKUP_TIP},        // Pickup Button
    { WWT_FLATBTN,  1, WW - 25, WW - 2,     69,         92,     SPR_PATROL_BTN,         STR_SET_PATROL_TIP},    // Patrol Button
    { WWT_FLATBTN,  1, WW - 25, WW - 2,     93,         116,    SPR_RENAME,             STR_NAME_STAFF_TIP},    // Rename Button
    { WWT_FLATBTN,  1, WW - 25, WW - 2,     117,        140,    SPR_LOCATE,             STR_LOCATE_SUBJECT_TIP},// Locate Button
    { WWT_FLATBTN,  1, WW - 25, WW - 2,     141,        164,    SPR_DEMOLISH,           STR_FIRE_STAFF_TIP},    // Fire Button
    { WIDGETS_END },
};

//0x9AF910
static rct_widget window_staff_options_widgets[] = {
    { WWT_FRAME,            0, 0,       WW - 1, 0,      WH - 1, 0xFFFFFFFF,             STR_NONE },             // Panel / Background
    { WWT_CAPTION,          0, 1,       WW - 2, 1,      14,     STR_STRINGID,           STR_WINDOW_TITLE_TIP }, // Title
    { WWT_CLOSEBOX,         0, WW - 13, WW - 3, 2,      13,     STR_CLOSE_X,            STR_CLOSE_WINDOW_TIP }, // Close x button
    { WWT_RESIZE,           1, 0,       WW - 1, 43,     WH - 1, 0xFFFFFFFF,             STR_NONE },             // Resize
    { WWT_TAB,              1, 3,       33,     17,     43,     IMAGE_TYPE_REMAP | SPR_TAB,   STR_STAFF_OVERVIEW_TIP },// Tab 1
    { WWT_TAB,              1, 34,      64,     17,     43,     IMAGE_TYPE_REMAP | SPR_TAB,   STR_STAFF_OPTIONS_TIP}, // Tab 2
    { WWT_TAB,              1, 65,      95,     17,     43,     IMAGE_TYPE_REMAP | SPR_TAB,   STR_STAFF_STATS_TIP},   // Tab 3
    { WWT_TAB,              1, 96,      126,    17,     43,     IMAGE_TYPE_REMAP | SPR_TAB,   STR_NONE},              // Tab 4
    { WWT_CHECKBOX,         1, 5,       WW - 6, 50,     61,     0xFFFFFFFF,             STR_NONE},              // Checkbox 1
    { WWT_CHECKBOX,         1, 5,       WW - 6, 67,     78,     0xFFFFFFFF,             STR_NONE },             // Checkbox 2
    { WWT_CHECKBOX,         1, 5,       WW - 6, 84,     95,     0xFFFFFFFF,             STR_NONE},              // Checkbox 3
    { WWT_CHECKBOX,         1, 5,       WW - 6, 101,    112,    0xFFFFFFFF,             STR_NONE},              // Checkbox 4
    { WWT_DROPDOWN,         1, 5,       WW - 6, 50,     61,     0xFFFFFFFF,             STR_NONE},              // Costume Dropdown
    { WWT_DROPDOWN_BUTTON,  1, WW - 17, WW - 7, 51,     60,     STR_DROPDOWN_GLYPH, STR_SELECT_COSTUME_TIP},// Costume Dropdown Button
    { WIDGETS_END },
};

//0x9AF9F4
static rct_widget window_staff_stats_widgets[] = {
    { WWT_FRAME,    0, 0,       WW - 1, 0,  WH - 1, 0xFFFFFFFF,             STR_NONE },             // Panel / Background
    { WWT_CAPTION,  0, 1,       WW - 2, 1,  14,     STR_STRINGID,           STR_WINDOW_TITLE_TIP }, // Title
    { WWT_CLOSEBOX, 0, WW - 13, WW - 3, 2,  13,     STR_CLOSE_X,            STR_CLOSE_WINDOW_TIP }, // Close x button
    { WWT_RESIZE,   1, 0,       WW - 1, 43, WH - 1, 0xFFFFFFFF,             STR_NONE },             // Resize
    { WWT_TAB,      1, 3,       33,     17, 43,     IMAGE_TYPE_REMAP | SPR_TAB,   STR_STAFF_OVERVIEW_TIP },// Tab 1
    { WWT_TAB,      1, 34,      64,     17, 43,     IMAGE_TYPE_REMAP | SPR_TAB,   STR_STAFF_OPTIONS_TIP}, // Tab 2
    { WWT_TAB,      1, 65,      95,     17, 43,     IMAGE_TYPE_REMAP | SPR_TAB,   STR_STAFF_STATS_TIP},   // Tab 3
    { WWT_TAB,      1, 96,      126,    17, 43,     IMAGE_TYPE_REMAP | SPR_TAB,   STR_NONE},              // Tab 4
    { WIDGETS_END },
};

static rct_widget *window_staff_page_widgets[] = {
    window_staff_overview_widgets,
    window_staff_options_widgets,
    window_staff_stats_widgets
};

static void window_staff_set_page(rct_window* w, sint32 page);
static void window_staff_disable_widgets(rct_window* w);
static void window_staff_unknown_05(rct_window *w);
static void window_staff_viewport_init(rct_window* w);

static void window_staff_overview_close(rct_window *w);
static void window_staff_overview_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_staff_overview_resize(rct_window *w);
static void window_staff_overview_mousedown(rct_window *w, rct_widgetindex widgetIndex, rct_widget* widget);
static void window_staff_overview_dropdown(rct_window *w, rct_widgetindex widgetIndex, sint32 dropdownIndex);
static void window_staff_overview_update(rct_window* w);
static void window_staff_overview_invalidate(rct_window *w);
static void window_staff_overview_paint(rct_window *w, rct_drawpixelinfo *dpi);
static void window_staff_overview_tab_paint(rct_window* w, rct_drawpixelinfo* dpi);
static void window_staff_overview_tool_update(rct_window* w, rct_widgetindex widgetIndex, sint32 x, sint32 y);
static void window_staff_overview_tool_down(rct_window* w, rct_widgetindex widgetIndex, sint32 x, sint32 y);
static void window_staff_overview_tool_abort(rct_window *w, rct_widgetindex widgetIndex);
static void window_staff_overview_text_input(rct_window *w, rct_widgetindex widgetIndex, char *text);
static void window_staff_overview_viewport_rotate(rct_window *w);

static void window_staff_options_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_staff_options_update(rct_window* w);
static void window_staff_options_invalidate(rct_window *w);
static void window_staff_options_paint(rct_window *w, rct_drawpixelinfo *dpi);
static void window_staff_options_tab_paint(rct_window* w, rct_drawpixelinfo* dpi);
static void window_staff_options_mousedown(rct_window *w, rct_widgetindex widgetIndex, rct_widget* widget);
static void window_staff_options_dropdown(rct_window *w, rct_widgetindex widgetIndex, sint32 dropdownIndex);

static void window_staff_stats_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_staff_stats_resize(rct_window *w);
static void window_staff_stats_update(rct_window* w);
static void window_staff_stats_invalidate(rct_window *w);
static void window_staff_stats_paint(rct_window *w, rct_drawpixelinfo *dpi);
static void window_staff_stats_tab_paint(rct_window* w, rct_drawpixelinfo* dpi);

void window_staff_set_colours();

// 0x992AEC
static rct_window_event_list window_staff_overview_events = {
    window_staff_overview_close,
    window_staff_overview_mouseup,
    window_staff_overview_resize,
    window_staff_overview_mousedown,
    window_staff_overview_dropdown,
    nullptr,
    window_staff_overview_update,
    nullptr,
    nullptr,
    window_staff_overview_tool_update,
    window_staff_overview_tool_down,
    nullptr,
    nullptr,
    window_staff_overview_tool_abort,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    window_staff_overview_text_input,
    window_staff_overview_viewport_rotate,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    window_staff_overview_invalidate, //Invalidate
    window_staff_overview_paint, //Paint
    nullptr
};

// 0x992B5C
static rct_window_event_list window_staff_options_events = {
    nullptr,
    window_staff_options_mouseup,
    window_staff_stats_resize,
    window_staff_options_mousedown,
    window_staff_options_dropdown,
    window_staff_unknown_05,
    window_staff_options_update,
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
    window_staff_options_invalidate, //Invalidate
    window_staff_options_paint, //Paint
    nullptr
};

// 0x992BCC
static rct_window_event_list window_staff_stats_events = {
    nullptr,
    window_staff_stats_mouseup,
    window_staff_stats_resize,
    nullptr,
    nullptr,
    window_staff_unknown_05,
    window_staff_stats_update,
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
    window_staff_stats_invalidate, //Invalidate
    window_staff_stats_paint, //Paint
    nullptr
};

static rct_window_event_list *window_staff_page_events[] = {
    &window_staff_overview_events,
    &window_staff_options_events,
    &window_staff_stats_events
};

uint32 window_staff_page_enabled_widgets[] = {
    (1 << WIDX_CLOSE) |
    (1 << WIDX_TAB_1) |
    (1 << WIDX_TAB_2) |
    (1 << WIDX_TAB_3) |
    (1 << WIDX_PICKUP) |
    (1 << WIDX_PATROL) |
    (1 << WIDX_RENAME) |
    (1 << WIDX_LOCATE) |
    (1 << WIDX_FIRE),

    (1 << WIDX_CLOSE) |
    (1 << WIDX_TAB_1) |
    (1 << WIDX_TAB_2) |
    (1 << WIDX_TAB_3) |
    (1 << WIDX_CHECKBOX_1) |
    (1 << WIDX_CHECKBOX_2) |
    (1 << WIDX_CHECKBOX_3) |
    (1 << WIDX_CHECKBOX_4) |
    (1 << WIDX_COSTUME_BTN),

    (1 << WIDX_CLOSE) |
    (1 << WIDX_TAB_1) |
    (1 << WIDX_TAB_2) |
    (1 << WIDX_TAB_3)
};

static uint8 _availableCostumes[ENTERTAINER_COSTUME_COUNT];

/**
*
*  rct2: 0x006BEE98
*/
rct_window *window_staff_open(rct_peep* peep)
{
    rct_window* w = window_bring_to_front_by_number(WC_PEEP, peep->sprite_index);
    if (w == nullptr) {
        w = window_create_auto_pos(WW, WH, &window_staff_overview_events, WC_PEEP, WF_10 | WF_RESIZABLE);

        w->number = peep->sprite_index;
        w->page = 0;
        w->viewport_focus_coordinates.y = 0;
        w->frame_no = 0;
        w->highlighted_item = 0;

        window_staff_disable_widgets(w);

        w->min_width = WW;
        w->min_height = WH;
        w->max_width = 500;
        w->max_height = 450;
    }
    w->page = 0;
    window_invalidate(w);

    w->widgets = window_staff_overview_widgets;
    w->enabled_widgets = window_staff_page_enabled_widgets[0];
    w->hold_down_widgets = 0;
    w->event_handlers = window_staff_page_events[0];
    w->pressed_widgets = 0;
    window_staff_disable_widgets(w);
    window_init_scroll_widgets(w);
    window_staff_viewport_init(w);
    if (get_sprite(w->number)->peep.state == PEEP_STATE_PICKED)
        window_event_mouse_up_call(w, WIDX_CHECKBOX_3);

    return w;
}

/**
* rct2: 0x006BED21
* Disable the staff pickup if not in pickup state.
*/
void window_staff_disable_widgets(rct_window* w)
{
    rct_peep* peep = &get_sprite(w->number)->peep;
    uint64 disabled_widgets = (1 << WIDX_TAB_4);

    if (peep->staff_type == STAFF_TYPE_SECURITY){
        disabled_widgets |= (1 << WIDX_TAB_2);
    }

    if (w->page == WINDOW_STAFF_OVERVIEW){
        if (peep_can_be_picked_up(peep)){
            if (w->disabled_widgets & (1 << WIDX_PICKUP))
                window_invalidate(w);
        }
        else{
            disabled_widgets |= (1 << WIDX_PICKUP);
            if (!(w->disabled_widgets & (1 << WIDX_PICKUP)))
                window_invalidate(w);
        }
    }

    w->disabled_widgets = disabled_widgets;
}

/**
 * Same as window_peep_overview_close.
 *  rct2: 0x006BDFF8
 */
void window_staff_overview_close(rct_window *w)
{
    if (input_test_flag(INPUT_FLAG_TOOL_ACTIVE)) {
        if (w->classification == gCurrentToolWidget.window_classification &&
            w->number == gCurrentToolWidget.window_number)
            tool_cancel();
    }
}

/**
 * Mostly similar to window_peep_set_page.
 *  rct2: 0x006BE023
 */
void window_staff_set_page(rct_window* w, sint32 page)
{
    if (input_test_flag(INPUT_FLAG_TOOL_ACTIVE))
    {
        if(w->number == gCurrentToolWidget.window_number &&
           w->classification == gCurrentToolWidget.window_classification)
            tool_cancel();

    }

    sint32 listen = 0;
    if (page == WINDOW_STAFF_OVERVIEW && w->page == WINDOW_STAFF_OVERVIEW && w->viewport){
        if (!(w->viewport->flags & VIEWPORT_FLAG_SOUND_ON))
            listen = 1;
    }


    w->page = page;
    w->frame_no = 0;

    rct_viewport* viewport = w->viewport;
    w->viewport = 0;
    if (viewport){
        viewport->width = 0;
    }

    w->enabled_widgets = window_staff_page_enabled_widgets[page];
    w->hold_down_widgets = 0;
    w->event_handlers = window_staff_page_events[page];
    w->pressed_widgets = 0;
    w->widgets = window_staff_page_widgets[page];

    window_staff_disable_widgets(w);
    window_invalidate(w);

    window_event_resize_call(w);
    window_event_invalidate_call(w);

    window_init_scroll_widgets(w);
    window_invalidate(w);

    if (listen && w->viewport) w->viewport->flags |= VIEWPORT_FLAG_SOUND_ON;
}

void game_command_callback_pickup_staff(sint32 eax, sint32 ebx, sint32 ecx, sint32 edx, sint32 esi, sint32 edi, sint32 ebp)
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
 *  rct2: 0x006BDF55
 */
void window_staff_overview_mouseup(rct_window *w, rct_widgetindex widgetIndex)
{
    rct_peep* peep = GET_PEEP(w->number);

    switch (widgetIndex) {
    case WIDX_CLOSE:
        window_close(w);
        break;
    case WIDX_TAB_1:
    case WIDX_TAB_2:
    case WIDX_TAB_3:
        window_staff_set_page(w, widgetIndex - WIDX_TAB_1);
        break;
    case WIDX_LOCATE:
        window_scroll_to_viewport(w);
        break;
    case WIDX_PICKUP:
        {
        // this is called in callback when hiring staff, setting nestlevel to 0 so that command is sent separately
        sint32 oldNestLevel = gGameCommandNestLevel;
        gGameCommandNestLevel = 0;
        game_command_callback = game_command_callback_pickup_staff;
        w->picked_peep_old_x = peep->x;
        game_do_command(w->number, GAME_COMMAND_FLAG_APPLY, 0, 0, GAME_COMMAND_PICKUP_STAFF, 0, 0);
        gGameCommandNestLevel = oldNestLevel;
        }
        break;
    case WIDX_FIRE:
        window_staff_fire_prompt_open(peep);
        break;
    case WIDX_RENAME:
        window_text_input_open(w, widgetIndex, STR_STAFF_TITLE_STAFF_MEMBER_NAME, STR_STAFF_PROMPT_ENTER_NAME, peep->name_string_idx, peep->id, 32);
        break;
    }
}

/**
 *
 *  rct2: 0x006BE558
 */
void window_staff_overview_resize(rct_window *w)
{
    window_staff_disable_widgets(w);

    w->min_width = WW;
    w->max_width = 500;
    w->min_height = WH;
    w->max_height = 450;

    if (w->width < w->min_width) {
        w->width = w->min_width;
        window_invalidate(w);
    }

    if (w->width > w->max_width) {
        window_invalidate(w);
        w->width = w->max_width;
    }

    if (w->height < w->min_height) {
        w->height = w->min_height;
        window_invalidate(w);
    }

    if (w->height > w->max_height) {
        window_invalidate(w);
        w->height = w->max_height;
    }

    rct_viewport* viewport = w->viewport;

    if (viewport) {
        sint32 new_width = w->width - 30;
        sint32 new_height = w->height - 62;

        // Update the viewport size
        if (viewport->width != new_width || viewport->height != new_height) {
            viewport->width = new_width;
            viewport->height = new_height;
            viewport->view_width = new_width << viewport->zoom;
            viewport->view_height = new_height << viewport->zoom;
        }
    }

    window_staff_viewport_init(w);
}

/**
 * Handle the dropdown of patrol button.
 *  rct2: 0x006BDF98
 */
void window_staff_overview_mousedown(rct_window *w, rct_widgetindex widgetIndex, rct_widget* widget)
{
    if (widgetIndex != WIDX_PATROL) {
        return;
    }

    // Dropdown names
    gDropdownItemsFormat[0] = STR_SET_PATROL_AREA;
    gDropdownItemsFormat[1] = STR_CLEAR_PATROL_AREA;

    sint32 x = widget->left + w->x;
    sint32 y = widget->top + w->y;
    sint32 extray = widget->bottom - widget->top + 1;
    window_dropdown_show_text(x, y, extray, w->colours[1], 0, 2);
    gDropdownDefaultIndex = 0;

    rct_peep* peep = GET_PEEP(w->number);

    // Disable clear patrol area if no area is set.
    if (!(gStaffModes[peep->staff_id] & 2)) {
        dropdown_set_disabled(1, true);
    }
}

/**
 *
 *  rct2: 0x006BDFA3
 */
void window_staff_overview_dropdown(rct_window *w, rct_widgetindex widgetIndex, sint32 dropdownIndex)
{
    if (widgetIndex != WIDX_PATROL) {
        return;
    }

    // Clear patrol
    if (dropdownIndex == 1) {
        rct_peep* peep = GET_PEEP(w->number);
        uint32 *addr = (uint32*)((uintptr_t)gStaffPatrolAreas + (peep->staff_id * 512));
        for (sint32 i = 0; i < 128; i++) {
            addr[i] = 0;
        }
        gStaffModes[peep->staff_id] &= ~2;

        gfx_invalidate_screen();
        staff_update_greyed_patrol_areas();
    }
    else {
        if (!tool_set(w, widgetIndex, TOOL_WALK_DOWN)) {
            show_gridlines();
            gStaffDrawPatrolAreas = w->number;
            gfx_invalidate_screen();
        }
    }
}

/**
 * Update the animation frame of the tab icon.
 *  rct2: 0x6BE602
 */
void window_staff_overview_update(rct_window* w)
{
    sint32 var_496 = w->var_496;
    var_496++;
    if (var_496 >= 24) {
        var_496 = 0;
    }
    w->var_496 = var_496;
    widget_invalidate(w, WIDX_TAB_1);
}

/**
 *
 *  rct2: 0x006BE814
 */
static void window_staff_set_order(rct_window* w, sint32 order_id)
{
    rct_peep* peep = GET_PEEP(w->number);

    sint32 ax = peep->staff_orders ^ (1 << order_id);
    sint32 flags = (ax << 8) | 1;

    game_do_command(peep->x, flags, peep->y, w->number, GAME_COMMAND_SET_STAFF_ORDER, 0, 0);
}

/**
 *
 *  rct2: 0x006BE7DB
 */
void window_staff_options_mouseup(rct_window *w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex) {
    case WIDX_CLOSE:
        window_close(w);
        break;
    case WIDX_TAB_1:
    case WIDX_TAB_2:
    case WIDX_TAB_3:
        window_staff_set_page(w, widgetIndex - WIDX_TAB_1);
        break;
    case WIDX_CHECKBOX_1:
    case WIDX_CHECKBOX_2:
    case WIDX_CHECKBOX_3:
    case WIDX_CHECKBOX_4:
        window_staff_set_order(w, widgetIndex - WIDX_CHECKBOX_1);
        break;
    }
}

/**
 *
 *  rct2: 0x006BE960
 */
void window_staff_options_update(rct_window* w)
{
    w->frame_no++;
    widget_invalidate(w, WIDX_TAB_2);
}

/**
 *
 *  rct2: 0x006BEBCF
 */
void window_staff_stats_mouseup(rct_window *w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex) {
    case WIDX_CLOSE:
        window_close(w);
        break;
    case WIDX_TAB_1:
    case WIDX_TAB_2:
    case WIDX_TAB_3:
        window_staff_set_page(w, widgetIndex - WIDX_TAB_1);
        break;
    }
}

/**
 *
 *  rct2: 0x006BEC1B, 0x006BE975
 */
void window_staff_stats_resize(rct_window *w)
{
    w->min_width = 190;
    w->max_width = 190;
    w->min_height = 119;
    w->max_height = 119;

    if (w->width < w->min_width) {
        w->width = w->min_width;
        window_invalidate(w);
    }

    if (w->width > w->max_width) {
        window_invalidate(w);
        w->width = w->max_width;
    }

    if (w->height < w->min_height) {
        w->height = w->min_height;
        window_invalidate(w);
    }

    if (w->height > w->max_height) {
        window_invalidate(w);
        w->height = w->max_height;
    }
}

/**
 *
 *  rct2: 0x006BEBEA
 */
void window_staff_stats_update(rct_window* w)
{
    w->frame_no++;
    widget_invalidate(w, WIDX_TAB_3);

    rct_peep* peep = GET_PEEP(w->number);
    if (peep->window_invalidate_flags & PEEP_INVALIDATE_STAFF_STATS) {
        peep->window_invalidate_flags &= ~PEEP_INVALIDATE_STAFF_STATS;
        window_invalidate(w);
    }
}

/**
 *
 *  rct2: 0x6BEC80, 0x6BE9DA
 */
void window_staff_unknown_05(rct_window *w)
{
    widget_invalidate(w, WIDX_TAB_1);
}

/**
 *
 *  rct2: 0x006BE9E9
 */
void window_staff_stats_invalidate(rct_window *w)
{
    colour_scheme_update_by_class(w, (rct_windowclass)WC_STAFF);

    if (window_staff_page_widgets[w->page] != w->widgets){
        w->widgets = window_staff_page_widgets[w->page];
        window_init_scroll_widgets(w);
    }

    w->pressed_widgets |= 1ULL << (w->page + WIDX_TAB_1);

    rct_peep* peep = GET_PEEP(w->number);

    set_format_arg(0, rct_string_id, peep->name_string_idx);
    set_format_arg(2, uint32, peep->id);

    window_staff_stats_widgets[WIDX_BACKGROUND].right = w->width - 1;
    window_staff_stats_widgets[WIDX_BACKGROUND].bottom = w->height - 1;

    window_staff_stats_widgets[WIDX_RESIZE].right = w->width - 1;
    window_staff_stats_widgets[WIDX_RESIZE].bottom = w->height - 1;

    window_staff_stats_widgets[WIDX_TITLE].right = w->width - 2;

    window_staff_stats_widgets[WIDX_CLOSE].left = w->width - 13;
    window_staff_stats_widgets[WIDX_CLOSE].right = w->width - 3;

    window_align_tabs(w, WIDX_TAB_1, WIDX_TAB_3);
}


/**
 *
 *  rct2: 0x006BE62B
 */
void window_staff_options_invalidate(rct_window *w)
{
    colour_scheme_update_by_class(w, (rct_windowclass)WC_STAFF);

    if (window_staff_page_widgets[w->page] != w->widgets){
        w->widgets = window_staff_page_widgets[w->page];
        window_init_scroll_widgets(w);
    }

    w->pressed_widgets |= 1ULL << (w->page + WIDX_TAB_1);

    rct_peep* peep = GET_PEEP(w->number);

    set_format_arg(0, rct_string_id, peep->name_string_idx);
    set_format_arg(2, uint32, peep->id);

    switch (peep->staff_type){
    case STAFF_TYPE_ENTERTAINER:
        window_staff_options_widgets[WIDX_CHECKBOX_1].type = WWT_EMPTY;
        window_staff_options_widgets[WIDX_CHECKBOX_2].type = WWT_EMPTY;
        window_staff_options_widgets[WIDX_CHECKBOX_3].type = WWT_EMPTY;
        window_staff_options_widgets[WIDX_CHECKBOX_4].type = WWT_EMPTY;
        window_staff_options_widgets[WIDX_COSTUME_BOX].type = WWT_DROPDOWN;
        window_staff_options_widgets[WIDX_COSTUME_BTN].type = WWT_DROPDOWN_BUTTON;
        window_staff_options_widgets[WIDX_COSTUME_BOX].text = StaffCostumeNames[peep->sprite_type - 4];
        break;
    case STAFF_TYPE_HANDYMAN:
        window_staff_options_widgets[WIDX_CHECKBOX_1].type = WWT_CHECKBOX;
        window_staff_options_widgets[WIDX_CHECKBOX_1].text = STR_STAFF_OPTION_SWEEP_FOOTPATHS;
        window_staff_options_widgets[WIDX_CHECKBOX_2].type = WWT_CHECKBOX;
        window_staff_options_widgets[WIDX_CHECKBOX_2].text = STR_STAFF_OPTION_WATER_GARDENS;
        window_staff_options_widgets[WIDX_CHECKBOX_3].type = WWT_CHECKBOX;
        window_staff_options_widgets[WIDX_CHECKBOX_3].text = STR_STAFF_OPTION_EMPTY_LITTER;
        window_staff_options_widgets[WIDX_CHECKBOX_4].type = WWT_CHECKBOX;
        window_staff_options_widgets[WIDX_CHECKBOX_4].text = STR_STAFF_OPTION_MOW_GRASS;
        window_staff_options_widgets[WIDX_COSTUME_BOX].type = WWT_EMPTY;
        window_staff_options_widgets[WIDX_COSTUME_BTN].type = WWT_EMPTY;
        w->pressed_widgets &= ~((1 << WIDX_CHECKBOX_1) | (1 << WIDX_CHECKBOX_2) | (1 << WIDX_CHECKBOX_3) | (1 << WIDX_CHECKBOX_4));
        w->pressed_widgets |= peep->staff_orders << WIDX_CHECKBOX_1;
        break;
    case STAFF_TYPE_MECHANIC:
        window_staff_options_widgets[WIDX_CHECKBOX_1].type = WWT_CHECKBOX;
        window_staff_options_widgets[WIDX_CHECKBOX_1].text = STR_INSPECT_RIDES;
        window_staff_options_widgets[WIDX_CHECKBOX_2].type = WWT_CHECKBOX;
        window_staff_options_widgets[WIDX_CHECKBOX_2].text = STR_FIX_RIDES;
        window_staff_options_widgets[WIDX_CHECKBOX_3].type = WWT_EMPTY;
        window_staff_options_widgets[WIDX_CHECKBOX_4].type = WWT_EMPTY;
        window_staff_options_widgets[WIDX_COSTUME_BOX].type = WWT_EMPTY;
        window_staff_options_widgets[WIDX_COSTUME_BTN].type = WWT_EMPTY;
        w->pressed_widgets &= ~((1 << WIDX_CHECKBOX_1) | (1 << WIDX_CHECKBOX_2));
        w->pressed_widgets |= peep->staff_orders << WIDX_CHECKBOX_1;
        break;
    case STAFF_TYPE_SECURITY:
        // Security guards don't have an options screen.
        break;
    }

    window_staff_options_widgets[WIDX_BACKGROUND].right = w->width - 1;
    window_staff_options_widgets[WIDX_BACKGROUND].bottom = w->height - 1;

    window_staff_options_widgets[WIDX_RESIZE].right = w->width - 1;
    window_staff_options_widgets[WIDX_RESIZE].bottom = w->height - 1;

    window_staff_options_widgets[WIDX_TITLE].right = w->width - 2;

    window_staff_options_widgets[WIDX_CLOSE].left = w->width - 13;
    window_staff_options_widgets[WIDX_CLOSE].right = w->width - 3;

    window_align_tabs(w, WIDX_TAB_1, WIDX_TAB_3);
}

/**
 *
 *  rct2: 0x006BDD91
 */
void window_staff_overview_invalidate(rct_window *w)
{
    colour_scheme_update_by_class(w, (rct_windowclass)WC_STAFF);

    if (window_staff_page_widgets[w->page] != w->widgets){
        w->widgets = window_staff_page_widgets[w->page];
        window_init_scroll_widgets(w);
    }

    w->pressed_widgets |= 1ULL << (w->page + WIDX_TAB_1);

    rct_peep* peep = GET_PEEP(w->number);

    set_format_arg(0, rct_string_id, peep->name_string_idx);
    set_format_arg(2, uint32, peep->id);

    window_staff_overview_widgets[WIDX_BACKGROUND].right = w->width - 1;
    window_staff_overview_widgets[WIDX_BACKGROUND].bottom = w->height - 1;

    window_staff_overview_widgets[WIDX_RESIZE].right = w->width - 1;
    window_staff_overview_widgets[WIDX_RESIZE].bottom = w->height - 1;

    window_staff_overview_widgets[WIDX_TITLE].right = w->width - 2;

    window_staff_overview_widgets[WIDX_VIEWPORT].right = w->width - 26;
    window_staff_overview_widgets[WIDX_VIEWPORT].bottom = w->height - 14;

    window_staff_overview_widgets[WIDX_BTM_LABEL].right = w->width - 26;
    window_staff_overview_widgets[WIDX_BTM_LABEL].top = w->height - 13;
    window_staff_overview_widgets[WIDX_BTM_LABEL].bottom = w->height - 3;

    window_staff_overview_widgets[WIDX_CLOSE].left = w->width - 13;
    window_staff_overview_widgets[WIDX_CLOSE].right = w->width - 3;

    window_staff_overview_widgets[WIDX_PICKUP].left = w->width - 25;
    window_staff_overview_widgets[WIDX_PICKUP].right = w->width - 2;

    window_staff_overview_widgets[WIDX_PATROL].left = w->width - 25;
    window_staff_overview_widgets[WIDX_PATROL].right = w->width - 2;

    window_staff_overview_widgets[WIDX_RENAME].left = w->width - 25;
    window_staff_overview_widgets[WIDX_RENAME].right = w->width - 2;

    window_staff_overview_widgets[WIDX_LOCATE].left = w->width - 25;
    window_staff_overview_widgets[WIDX_LOCATE].right = w->width - 2;

    window_staff_overview_widgets[WIDX_FIRE].left = w->width - 25;
    window_staff_overview_widgets[WIDX_FIRE].right = w->width - 2;

    window_align_tabs(w, WIDX_TAB_1, WIDX_TAB_3);
}

/**
 *
 *  rct2: 0x6BDEAF
 */
void window_staff_overview_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
    window_draw_widgets(w, dpi);
    window_staff_overview_tab_paint(w, dpi);
    window_staff_options_tab_paint(w, dpi);
    window_staff_stats_tab_paint(w, dpi);


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
    rct_widget* widget = &w->widgets[WIDX_BTM_LABEL];
    sint32 x = (widget->left + widget->right) / 2 + w->x;
    sint32 y = w->y + widget->top;
    sint32 width = widget->right - widget->left;
    gfx_draw_string_centred_clipped(dpi, STR_BLACK_STRING, gCommonFormatArgs, COLOUR_BLACK, x, y, width);
}

/**
 *
 *  rct2: 0x6BEC8F
 */
void window_staff_options_tab_paint(rct_window* w, rct_drawpixelinfo* dpi)
{
    if (w->disabled_widgets & (1 << WIDX_TAB_2)) return;

    rct_widget* widget = &w->widgets[WIDX_TAB_2];
    sint32 x = widget->left + w->x;
    sint32 y = widget->top + w->y;

    sint32 image_id = SPR_TAB_STAFF_OPTIONS_0;

    if (w->page == WINDOW_STAFF_OPTIONS){
        image_id += (w->frame_no / 2) % 7;
    }

    gfx_draw_sprite(dpi, image_id, x, y, 0);
}

/**
 *
 *  rct2: 0x6BECD3
 */
void window_staff_stats_tab_paint(rct_window* w, rct_drawpixelinfo* dpi)
{
    if (w->disabled_widgets & (1 << WIDX_TAB_3)) return;

    rct_widget* widget = &w->widgets[WIDX_TAB_3];
    sint32 x = widget->left + w->x;
    sint32 y = widget->top + w->y;

    sint32 image_id = SPR_TAB_STATS_0;

    if (w->page == WINDOW_STAFF_STATISTICS){
        image_id += (w->frame_no / 4) % 7;
    }

    gfx_draw_sprite(dpi, image_id, x, y, 0);
}

/**
* Based on rct2: 0x6983dd in window_guest to be remerged into one when peep file added.
*/
void window_staff_overview_tab_paint(rct_window* w, rct_drawpixelinfo* dpi)
{
    if (w->disabled_widgets & (1 << WIDX_TAB_1))
        return;

    rct_widget* widget = &w->widgets[WIDX_TAB_1];
    sint32 width = widget->right - widget->left - 1;
    sint32 height = widget->bottom - widget->top - 1;
    sint32 x = widget->left + 1 + w->x;
    sint32 y = widget->top + 1 + w->y;
    if (w->page == WINDOW_STAFF_OVERVIEW) height++;

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

    if (w->page == WINDOW_STAFF_OVERVIEW){
        eax = w->highlighted_item >> 16;
        eax &= 0xFFFC;
    }
    ebx += eax;

    sint32 sprite_id = ebx | SPRITE_ID_PALETTE_COLOUR_2(peep->tshirt_colour , peep->trousers_colour);
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
 *  rct2: 0x6BE7C6
 */
void window_staff_options_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
    window_draw_widgets(w, dpi);
    window_staff_overview_tab_paint(w, dpi);
    window_staff_options_tab_paint(w, dpi);
    window_staff_stats_tab_paint(w, dpi);
}

/**
 *
 *  rct2: 0x6BEA86
 */
void window_staff_stats_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
    window_draw_widgets(w, dpi);
    window_staff_overview_tab_paint(w, dpi);
    window_staff_options_tab_paint(w, dpi);
    window_staff_stats_tab_paint(w, dpi);

    rct_peep* peep = GET_PEEP(w->number);

    sint32 x = w->x + window_staff_stats_widgets[WIDX_RESIZE].left + 4;
    sint32 y = w->y + window_staff_stats_widgets[WIDX_RESIZE].top + 4;

    if (!(gParkFlags & PARK_FLAGS_NO_MONEY)) {
        set_format_arg(0, money32, wage_table[peep->staff_type]);
        gfx_draw_string_left(dpi, STR_STAFF_STAT_WAGES, gCommonFormatArgs, COLOUR_BLACK, x, y);
        y += 10;
    }

    gfx_draw_string_left(dpi, STR_STAFF_STAT_EMPLOYED_FOR, (void*)&peep->time_in_park, COLOUR_BLACK, x, y);
    y += 10;

    switch (peep->staff_type){
    case STAFF_TYPE_HANDYMAN:
        gfx_draw_string_left(dpi, STR_STAFF_STAT_LAWNS_MOWN, (void*)&peep->staff_lawns_mown, COLOUR_BLACK, x, y);
        y += 10;
        gfx_draw_string_left(dpi, STR_STAFF_STAT_GARDENS_WATERED, (void*)&peep->staff_gardens_watered, COLOUR_BLACK, x, y);
        y += 10;
        gfx_draw_string_left(dpi, STR_STAFF_STAT_LITTER_SWEPT, (void*)&peep->staff_litter_swept, COLOUR_BLACK, x, y);
        y += 10;
        gfx_draw_string_left(dpi, STR_STAFF_STAT_BINS_EMPTIED, (void*)&peep->staff_bins_emptied, COLOUR_BLACK, x, y);
        break;
    case STAFF_TYPE_MECHANIC:
        gfx_draw_string_left(dpi, STR_STAFF_STAT_RIDES_INSPECTED, (void*)&peep->staff_rides_inspected, COLOUR_BLACK, x, y);
        y += 10;
        gfx_draw_string_left(dpi, STR_STAFF_STAT_RIDES_FIXED, (void*)&peep->staff_rides_fixed, COLOUR_BLACK, x, y);
        break;
    }
}

/**
 *
 *  rct2: 0x006BDFD8
 */
void window_staff_overview_tool_update(rct_window* w, rct_widgetindex widgetIndex, sint32 x, sint32 y)
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
 *  rct2: 0x006BDFC3
 */
void window_staff_overview_tool_down(rct_window* w, rct_widgetindex widgetIndex, sint32 x, sint32 y)
{
    if (widgetIndex == WIDX_PICKUP) {
        sint32 dest_x, dest_y;
        rct_map_element* mapElement;
        footpath_get_coordinates_from_pos(x, y + 16, &dest_x, &dest_y, nullptr, &mapElement);

        if (dest_x == MAP_LOCATION_NULL)
            return;

        game_command_callback = game_command_callback_pickup_staff;
        game_do_command(w->number, GAME_COMMAND_FLAG_APPLY, 2, mapElement->base_height, GAME_COMMAND_PICKUP_STAFF, dest_x, dest_y);
    }
    else if (widgetIndex == WIDX_PATROL){
        sint32 dest_x, dest_y;
        footpath_get_coordinates_from_pos(x, y, &dest_x, &dest_y, nullptr, nullptr);

        if (dest_x == MAP_LOCATION_NULL) return;

        game_do_command(dest_x, 1, dest_y, w->number, GAME_COMMAND_SET_STAFF_PATROL, 0, 0);
    }
}

/**
 *
 *  rct2: 0x6BDFAE
 */
void window_staff_overview_tool_abort(rct_window *w, rct_widgetindex widgetIndex)
{
    if (widgetIndex == WIDX_PICKUP) {
        game_do_command(w->number, GAME_COMMAND_FLAG_APPLY, 1, 0, GAME_COMMAND_PICKUP_STAFF, w->picked_peep_old_x, 0);
    }
    else if (widgetIndex == WIDX_PATROL){
        hide_gridlines();
        gStaffDrawPatrolAreas = 0xFFFF;
        gfx_invalidate_screen();
    }
}

/* rct2: 0x6BDFED */
void window_staff_overview_text_input(rct_window *w, rct_widgetindex widgetIndex, char *text)
{
    if (widgetIndex != WIDX_RENAME)
        return;

    if (text == nullptr)
        return;

    gGameCommandErrorTitle = STR_STAFF_ERROR_CANT_NAME_STAFF_MEMBER;
    game_do_command(1, GAME_COMMAND_FLAG_APPLY, w->number, *((sint32*)(text + 0)), GAME_COMMAND_SET_STAFF_NAME, *((sint32*)(text + 8)), *((sint32*)(text + 4)));
    game_do_command(2, GAME_COMMAND_FLAG_APPLY, w->number, *((sint32*)(text + 12)), GAME_COMMAND_SET_STAFF_NAME, *((sint32*)(text + 20)), *((sint32*)(text + 16)));
    game_do_command(0, GAME_COMMAND_FLAG_APPLY, w->number, *((sint32*)(text + 24)), GAME_COMMAND_SET_STAFF_NAME, *((sint32*)(text + 32)), *((sint32*)(text + 28)));
}

/**
 *
 *  rct2: 0x006BE5FC
 */
void window_staff_overview_viewport_rotate(rct_window *w)
{
    window_staff_viewport_init(w);
}

/**
 *
 *  rct2: 0x006BEDA3
 */
void window_staff_viewport_init(rct_window* w){
    if (w->page != WINDOW_STAFF_OVERVIEW) return;

    sprite_focus focus = { 0 };

    focus.sprite_id = w->number;

    rct_peep* peep = GET_PEEP(w->number);

    if (peep->state == PEEP_STATE_PICKED){
        focus.sprite_id = SPRITE_INDEX_NULL;
    }
    else{
        focus.type |= VIEWPORT_FOCUS_TYPE_SPRITE | VIEWPORT_FOCUS_TYPE_COORDINATE;
        focus.rotation = get_current_rotation();
    }

    uint16 viewport_flags;

    if (w->viewport){
        //Check all combos, for now skipping y and rot
        if (focus.sprite_id == w->viewport_focus_sprite.sprite_id &&
            focus.type == w->viewport_focus_sprite.type &&
            focus.rotation == w->viewport_focus_sprite.rotation)
            return;

        viewport_flags = w->viewport->flags;
        w->viewport->width = 0;
        w->viewport = 0;

        viewport_update_pointers();
    }
    else{
        viewport_flags = 0;
        if (gConfigGeneral.always_show_gridlines)
            viewport_flags |= VIEWPORT_FLAG_GRIDLINES;
    }

    window_event_invalidate_call(w);

    w->viewport_focus_sprite.sprite_id = focus.sprite_id;
    w->viewport_focus_sprite.type = focus.type;
    w->viewport_focus_sprite.rotation = focus.rotation;

    if (peep->state != PEEP_STATE_PICKED){
        if (!(w->viewport)){
            rct_widget* view_widget = &w->widgets[WIDX_VIEWPORT];

            sint32 x = view_widget->left + 1 + w->x;
            sint32 y = view_widget->top + 1 + w->y;
            sint32 width = view_widget->right - view_widget->left - 1;
            sint32 height = view_widget->bottom - view_widget->top - 1;

            viewport_create(w, x, y, width, height, 0, 0, 0, 0, focus.type & VIEWPORT_FOCUS_TYPE_MASK, focus.sprite_id);
            w->flags |= WF_NO_SCROLLING;
            window_invalidate(w);
        }
    }

    if (w->viewport)
        w->viewport->flags = viewport_flags;
    window_invalidate(w);
}

/**
* Handle the costume of staff member.
* rct2: 0x006BE802
*/
void window_staff_options_mousedown(rct_window *w, rct_widgetindex widgetIndex, rct_widget* widget)
{
    if (widgetIndex != WIDX_COSTUME_BTN) {
        return;
    }

    rct_peep* peep = GET_PEEP(w->number);
    sint32 checkedIndex = -1;
    //This will be moved below where Items Checked is when all
    //of dropdown related functions are finished. This prevents
    //the dropdown from not working on first click.
    sint32 numCostumes = staff_get_available_entertainer_costume_list(_availableCostumes);
    for (sint32 i = 0; i < numCostumes; i++) {
        uint8 costume = _availableCostumes[i];
        if (peep->sprite_type == PEEP_SPRITE_TYPE_ENTERTAINER_PANDA + costume) {
            checkedIndex = i;
        }
        gDropdownItemsArgs[i] = StaffCostumeNames[costume];
        gDropdownItemsFormat[i] = STR_DROPDOWN_MENU_LABEL;
    }

    //Get the dropdown box widget instead of button.
    widget--;

    sint32 x = widget->left + w->x;
    sint32 y = widget->top + w->y;
    sint32 extray = widget->bottom - widget->top + 1;
    sint32 width = widget->right - widget->left - 3;
    window_dropdown_show_text_custom_width(x, y, extray, w->colours[1], 0, DROPDOWN_FLAG_STAY_OPEN, numCostumes, width);

    // See above note.
    if (checkedIndex != -1)
    {
        dropdown_set_checked(checkedIndex, true);
    }
}

/**
 *
 *  rct2: 0x6BE809
 */
void window_staff_options_dropdown(rct_window *w, rct_widgetindex widgetIndex, sint32 dropdownIndex)
{
    if (widgetIndex != WIDX_COSTUME_BTN) {
        return;
    }

    if (dropdownIndex == -1)
        return;

    rct_peep* peep = GET_PEEP(w->number);
    sint32 costume = _availableCostumes[dropdownIndex] | 0x80;
    game_do_command(peep->x, (costume << 8) | 1, peep->y, w->number, GAME_COMMAND_SET_STAFF_ORDER, 0, 0);
}
