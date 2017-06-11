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

#include <openrct2/config/Config.h>
#include <openrct2/interface/window.h>
#include <openrct2/interface/widget.h>
#include <openrct2/localisation/localisation.h>
#include <openrct2/platform/platform.h>
#include "../input/KeyboardShortcuts.h"

#define WW 420
#define WH 280

#define WW_SC_MAX 1200
#define WH_SC_MAX 800

enum WINDOW_SHORTCUT_WIDGET_IDX {
    WIDX_BACKGROUND,
    WIDX_TITLE,
    WIDX_CLOSE,
    WIDX_SCROLL,
    WIDX_RESET
};

// 0x9DE48C
static rct_widget window_shortcut_widgets[] = {
    { WWT_FRAME,            0,  0,      WW - 1, 0,      WH - 1,     STR_NONE,                   STR_NONE },
    { WWT_CAPTION,          0,  1,      WW - 2, 1,      14,         STR_SHORTCUTS_TITLE,        STR_WINDOW_TITLE_TIP },
    { WWT_CLOSEBOX,         0,  WW-13,  WW - 3, 2,      13,         STR_CLOSE_X,                STR_CLOSE_WINDOW_TIP },
    { WWT_SCROLL,           0,  4,      WW - 5, 18,     WH - 18,    SCROLL_VERTICAL,            STR_SHORTCUT_LIST_TIP },
    { WWT_DROPDOWN_BUTTON,  0,  4,      153,    WH-15,  WH - 4,     STR_SHORTCUT_ACTION_RESET,  STR_SHORTCUT_ACTION_RESET_TIP },
    { WIDGETS_END }
};

static void window_shortcut_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_shortcut_resize(rct_window *w);
static void window_shortcut_invalidate(rct_window *w);
static void window_shortcut_paint(rct_window *w, rct_drawpixelinfo *dpi);
static void window_shortcut_tooltip(rct_window* w, rct_widgetindex widgetIndex, rct_string_id *stringId);
static void window_shortcut_scrollgetsize(rct_window *w, sint32 scrollIndex, sint32 *width, sint32 *height);
static void window_shortcut_scrollmousedown(rct_window *w, sint32 scrollIndex, sint32 x, sint32 y);
static void window_shortcut_scrollmouseover(rct_window *w, sint32 scrollIndex, sint32 x, sint32 y);
static void window_shortcut_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, sint32 scrollIndex);

static rct_window_event_list window_shortcut_events = {
    NULL,
    window_shortcut_mouseup,
    window_shortcut_resize,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    window_shortcut_scrollgetsize,
    window_shortcut_scrollmousedown,
    NULL,
    window_shortcut_scrollmouseover,
    NULL,
    NULL,
    NULL,
    window_shortcut_tooltip,
    NULL,
    NULL,
    window_shortcut_invalidate,
    window_shortcut_paint,
    window_shortcut_scrollpaint
};

const rct_string_id ShortcutStringIds[] = {
    STR_SHORTCUT_CLOSE_TOP_MOST_WINDOW,
    STR_SHORTCUT_CLOSE_ALL_FLOATING_WINDOWS,
    STR_SHORTCUT_CANCEL_CONSTRUCTION_MODE,
    STR_SHORTCUT_PAUSE_GAME,
    STR_SHORTCUT_ZOOM_VIEW_OUT,
    STR_SHORTCUT_ZOOM_VIEW_IN,
    STR_SHORTCUT_ROTATE_VIEW_CLOCKWISE,
    STR_SHORTCUT_ROTATE_VIEW_ANTICLOCKWISE,
    STR_SHORTCUT_ROTATE_CONSTRUCTION_OBJECT,
    STR_SHORTCUT_UNDERGROUND_VIEW_TOGGLE,
    STR_SHORTCUT_REMOVE_BASE_LAND_TOGGLE,
    STR_SHORTCUT_REMOVE_VERTICAL_LAND_TOGGLE,
    STR_SHORTCUT_SEE_THROUGH_RIDES_TOGGLE,
    STR_SHORTCUT_SEE_THROUGH_SCENERY_TOGGLE,
    STR_SHORTCUT_INVISIBLE_SUPPORTS_TOGGLE,
    STR_SHORTCUT_INVISIBLE_PEOPLE_TOGGLE,
    STR_SHORTCUT_HEIGHT_MARKS_ON_LAND_TOGGLE,
    STR_SHORTCUT_HEIGHT_MARKS_ON_RIDE_TRACKS_TOGGLE,
    STR_SHORTCUT_HEIGHT_MARKS_ON_PATHS_TOGGLE,
    STR_SHORTCUT_ADJUST_LAND,
    STR_SHORTCUT_ADJUST_WATER,
    STR_SHORTCUT_BUILD_SCENERY,
    STR_SHORTCUT_BUILD_PATHS,
    STR_SHORTCUT_BUILD_NEW_RIDE,
    STR_SHORTCUT_SHOW_FINANCIAL_INFORMATION,
    STR_SHORTCUT_SHOW_RESEARCH_INFORMATION,
    STR_SHORTCUT_SHOW_RIDES_LIST,
    STR_SHORTCUT_SHOW_PARK_INFORMATION,
    STR_SHORTCUT_SHOW_GUEST_LIST,
    STR_SHORTCUT_SHOW_STAFF_LIST,
    STR_SHORTCUT_SHOW_RECENT_MESSAGES,
    STR_SHORTCUT_SHOW_MAP,
    STR_SHORTCUT_SCREENSHOT,
    STR_SHORTCUT_REDUCE_GAME_SPEED,
    STR_SHORTCUT_INCREASE_GAME_SPEED,
    STR_SHORTCUT_OPEN_CHEATS_WINDOW,
    STR_SHORTCUT_TOGGLE_VISIBILITY_OF_TOOLBARS,
    STR_SHORTCUT_SCROLL_MAP_UP,
    STR_SHORTCUT_SCROLL_MAP_LEFT,
    STR_SHORTCUT_SCROLL_MAP_DOWN,
    STR_SHORTCUT_SCROLL_MAP_RIGHT,
    STR_SEND_MESSAGE,
    STR_SHORTCUT_QUICK_SAVE_GAME,
    STR_SHORTCUT_SHOW_OPTIONS,
    STR_SHORTCUT_MUTE_SOUND,
    STR_SHORTCUT_WINDOWED_MODE_TOGGLE,
    STR_SHORTCUT_SHOW_MULTIPLAYER,
    STR_SHORTCUT_PAINT_ORIGINAL,
    STR_SHORTCUT_DEBUG_PAINT_TOGGLE,
    STR_SHORTCUT_SEE_THROUGH_PATHS_TOGGLE,
    STR_SHORTCUT_RIDE_CONSTRUCTION_TURN_LEFT,
    STR_SHORTCUT_RIDE_CONSTRUCTION_TURN_RIGHT,
    STR_SHORTCUT_RIDE_CONSTRUCTION_USE_TRACK_DEFAULT,
    STR_SHORTCUT_RIDE_CONSTRUCTION_SLOPE_DOWN,
    STR_SHORTCUT_RIDE_CONSTRUCTION_SLOPE_UP,
    STR_SHORTCUT_RIDE_CONSTRUCTION_CHAIN_LIFT_TOGGLE,
    STR_SHORTCUT_RIDE_CONSTRUCTION_BANK_LEFT,
    STR_SHORTCUT_RIDE_CONSTRUCTION_BANK_RIGHT,
    STR_SHORTCUT_RIDE_CONSTRUCTION_PREVIOUS_TRACK,
    STR_SHORTCUT_RIDE_CONSTRUCTION_NEXT_TRACK,
    STR_SHORTCUT_RIDE_CONSTRUCTION_BUILD_CURRENT,
    STR_SHORTCUT_RIDE_CONSTRUCTION_DEMOLISH_CURRENT,
};

/**
 *
 *  rct2: 0x006E3884
 */
void window_shortcut_keys_open()
{
    rct_window* w;

    w = window_bring_to_front_by_class(WC_KEYBOARD_SHORTCUT_LIST);

    if (w) return;

    w = window_create_auto_pos(WW, WH, &window_shortcut_events, WC_KEYBOARD_SHORTCUT_LIST, WF_RESIZABLE);

    w->widgets = window_shortcut_widgets;
    w->enabled_widgets = (1 << WIDX_CLOSE) | (1 << WIDX_RESET);
    window_init_scroll_widgets(w);

    w->no_list_items = SHORTCUT_COUNT;
    w->selected_list_item = -1;
    w->min_width = WW;
    w->min_height = WH;
    w->max_width = WW_SC_MAX;
    w->max_height = WH_SC_MAX;
}

/**
*
*  rct2: 0x006E39E4
*/
static void window_shortcut_mouseup(rct_window *w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex){
    case WIDX_CLOSE:
        window_close(w);
        break;
    case WIDX_RESET:
        keyboard_shortcuts_reset();
        keyboard_shortcuts_save();
        window_invalidate(w);
        break;
    }
}

static void window_shortcut_resize(rct_window *w)
{
    window_set_resize(w, w->min_width, w->min_height, w->max_width, w->max_height);
}

static void window_shortcut_invalidate(rct_window *w)
{
    window_shortcut_widgets[WIDX_BACKGROUND].right = w->width - 1;
    window_shortcut_widgets[WIDX_BACKGROUND].bottom = w->height - 1;
    window_shortcut_widgets[WIDX_TITLE].right = w->width - 2;
    window_shortcut_widgets[WIDX_CLOSE].right = w->width - 3;
    window_shortcut_widgets[WIDX_CLOSE].left = w->width - 13;
    window_shortcut_widgets[WIDX_SCROLL].right = w->width - 5;
    window_shortcut_widgets[WIDX_SCROLL].bottom = w->height - 18;
    window_shortcut_widgets[WIDX_RESET].top = w->height - 15;
    window_shortcut_widgets[WIDX_RESET].bottom = w->height - 4;
}

/**
*
*  rct2: 0x006E38E0
*/
static void window_shortcut_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
    window_draw_widgets(w, dpi);
}

/**
*
*  rct2: 0x006E3A0C
*/
static void window_shortcut_tooltip(rct_window* w, rct_widgetindex widgetIndex, rct_string_id *stringId)
{
    set_format_arg(0, rct_string_id, STR_LIST);
}

/**
*
*  rct2: 0x006E3A07
*/
static void window_shortcut_scrollgetsize(rct_window *w, sint32 scrollIndex, sint32 *width, sint32 *height)
{
    *height = w->no_list_items * 10;
}

/**
*
*  rct2: 0x006E3A3E
*/
static void window_shortcut_scrollmousedown(rct_window *w, sint32 scrollIndex, sint32 x, sint32 y)
{
    sint32 selected_item = y / 10;
    if (selected_item >= w->no_list_items)
        return;

    window_shortcut_change_open(selected_item);
}

/**
*
*  rct2: 0x006E3A16
*/
static void window_shortcut_scrollmouseover(rct_window *w, sint32 scrollIndex, sint32 x, sint32 y)
{
    sint32 selected_item = y / 10;
    if (selected_item >= w->no_list_items)
        return;

    w->selected_list_item = selected_item;

    window_invalidate(w);
}

/**
 *
 *  rct2: 0x006E38E6
 */
static void window_shortcut_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, sint32 scrollIndex)
{
    gfx_fill_rect(dpi, dpi->x, dpi->y, dpi->x + dpi->width - 1, dpi->y + dpi->height - 1, ColourMapA[w->colours[1]].mid_light);

    for (sint32 i = 0; i < w->no_list_items; ++i) {
        sint32 y = i * 10;
        if (y > dpi->y + dpi->height)
            break;

        if (y + 10 < dpi->y)continue;
        sint32 format = STR_BLACK_STRING;
        if (i == w->selected_list_item) {
            format = STR_WINDOW_COLOUR_2_STRINGID;
            gfx_filter_rect(dpi, 0, y, 800, y + 9, PALETTE_DARKEN_1);
        }

        char templateString[128];
        keyboard_shortcuts_format_string(templateString, 128, i);

        set_format_arg(0, rct_string_id, STR_SHORTCUT_ENTRY_FORMAT);
        set_format_arg(2, rct_string_id, ShortcutStringIds[i]);
        set_format_arg(4, rct_string_id, STR_STRING);
        set_format_arg(6, char *, templateString);
        gfx_draw_string_left(dpi, format, gCommonFormatArgs, COLOUR_BLACK, 0, y - 1);
    }
}
