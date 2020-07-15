/*****************************************************************************
 * Copyright (c) 2014-2019 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Window.h"

#include <openrct2-ui/input/KeyboardShortcuts.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2/config/Config.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/localisation/Localisation.h>

static constexpr const rct_string_id WINDOW_TITLE = STR_SHORTCUTS_TITLE;
static constexpr const int32_t WW = 420;
static constexpr const int32_t WH = 280;

static constexpr const int32_t WW_SC_MAX = 1200;
static constexpr const int32_t WH_SC_MAX = 800;

// clang-format off
enum WINDOW_SHORTCUT_WIDGET_IDX {
    WIDX_BACKGROUND,
    WIDX_TITLE,
    WIDX_CLOSE,
    WIDX_SCROLL,
    WIDX_RESET
};

// 0x9DE48C
static rct_widget window_shortcut_widgets[] = {
    WINDOW_SHIM(WINDOW_TITLE, WW, WH),
    { WWT_SCROLL,           0,  4,      WW - 5, 18,     WH - 18,    SCROLL_VERTICAL,            STR_SHORTCUT_LIST_TIP },
    { WWT_BUTTON,           0,  4,      153,    WH-15,  WH - 4,     STR_SHORTCUT_ACTION_RESET,  STR_SHORTCUT_ACTION_RESET_TIP },
    { WIDGETS_END }
};

static void window_shortcut_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_shortcut_resize(rct_window *w);
static void window_shortcut_invalidate(rct_window *w);
static void window_shortcut_paint(rct_window *w, rct_drawpixelinfo *dpi);
static void window_shortcut_scrollgetsize(rct_window *w, int32_t scrollIndex, int32_t *width, int32_t *height);
static void window_shortcut_scrollmousedown(rct_window *w, int32_t scrollIndex, const ScreenCoordsXY& screenCoords);
static void window_shortcut_scrollmouseover(rct_window *w, int32_t scrollIndex, const ScreenCoordsXY& screenCoords);
static void window_shortcut_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, int32_t scrollIndex);

static rct_window_event_list window_shortcut_events = {
    nullptr,
    window_shortcut_mouseup,
    window_shortcut_resize,
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
    window_shortcut_scrollgetsize,
    window_shortcut_scrollmousedown,
    nullptr,
    window_shortcut_scrollmouseover,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    window_shortcut_invalidate,
    window_shortcut_paint,
    window_shortcut_scrollpaint
};

struct ShortcutStringPair
{
    KeyboardShortcut ShortcutId;
    rct_string_id StringId;
};

static const ShortcutStringPair ShortcutList[] =
{
    { SHORTCUT_CLOSE_TOP_MOST_WINDOW,                 STR_SHORTCUT_CLOSE_TOP_MOST_WINDOW },
    { SHORTCUT_CLOSE_ALL_FLOATING_WINDOWS,            STR_SHORTCUT_CLOSE_ALL_FLOATING_WINDOWS },
    { SHORTCUT_CANCEL_CONSTRUCTION_MODE,              STR_SHORTCUT_CANCEL_CONSTRUCTION_MODE },
    { SHORTCUT_REMOVE_TOP_BOTTOM_TOOLBAR_TOGGLE,      STR_SHORTCUT_TOGGLE_VISIBILITY_OF_TOOLBARS },

    { SHORTCUT_UNDEFINED,                             STR_NONE },

    { SHORTCUT_PAUSE_GAME,                            STR_SHORTCUT_PAUSE_GAME },
    { SHORTCUT_REDUCE_GAME_SPEED,                     STR_SHORTCUT_REDUCE_GAME_SPEED },
    { SHORTCUT_INCREASE_GAME_SPEED,                   STR_SHORTCUT_INCREASE_GAME_SPEED },
    { SHORTCUT_LOAD_GAME,                             STR_LOAD_GAME },
    { SHORTCUT_QUICK_SAVE_GAME,                       STR_SHORTCUT_QUICK_SAVE_GAME },
    { SHORTCUT_SHOW_OPTIONS,                          STR_SHORTCUT_SHOW_OPTIONS },
    { SHORTCUT_SCREENSHOT,                            STR_SHORTCUT_SCREENSHOT },
    { SHORTCUT_MUTE_SOUND,                            STR_SHORTCUT_MUTE_SOUND },

    { SHORTCUT_UNDEFINED,                             STR_NONE },

    { SHORTCUT_OPEN_CHEAT_WINDOW,                     STR_SHORTCUT_OPEN_CHEATS_WINDOW },
    { SHORTCUT_TOGGLE_CLEARANCE_CHECKS,               STR_SHORTCUT_TOGGLE_CLEARANCE_CHECKS },

    { SHORTCUT_UNDEFINED,                             STR_NONE },

    { SHORTCUT_ZOOM_VIEW_OUT,                         STR_SHORTCUT_ZOOM_VIEW_OUT },
    { SHORTCUT_ZOOM_VIEW_IN,                          STR_SHORTCUT_ZOOM_VIEW_IN },
    { SHORTCUT_ROTATE_VIEW_CLOCKWISE,                 STR_SHORTCUT_ROTATE_VIEW_CLOCKWISE },
    { SHORTCUT_ROTATE_VIEW_ANTICLOCKWISE,             STR_SHORTCUT_ROTATE_VIEW_ANTICLOCKWISE },
    { SHORTCUT_SHOW_MAP,                              STR_SHORTCUT_SHOW_MAP },

    { SHORTCUT_UNDEFINED,                             STR_NONE },

    { SHORTCUT_CLEAR_SCENERY,                         STR_SHORTCUT_CLEAR_SCENERY },
    { SHORTCUT_ADJUST_LAND,                           STR_SHORTCUT_ADJUST_LAND },
    { SHORTCUT_ADJUST_WATER,                          STR_SHORTCUT_ADJUST_WATER },
    { SHORTCUT_BUILD_SCENERY,                         STR_SHORTCUT_BUILD_SCENERY },
    { SHORTCUT_BUILD_PATHS,                           STR_SHORTCUT_BUILD_PATHS },
    { SHORTCUT_BUILD_NEW_RIDE,                        STR_SHORTCUT_BUILD_NEW_RIDE },

    { SHORTCUT_UNDEFINED,                             STR_NONE },

    { SHORTCUT_SHOW_FINANCIAL_INFORMATION,            STR_SHORTCUT_SHOW_FINANCIAL_INFORMATION },
    { SHORTCUT_SHOW_RESEARCH_INFORMATION,             STR_SHORTCUT_SHOW_RESEARCH_INFORMATION },
    { SHORTCUT_SHOW_RIDES_LIST,                       STR_SHORTCUT_SHOW_RIDES_LIST },
    { SHORTCUT_SHOW_PARK_INFORMATION,                 STR_SHORTCUT_SHOW_PARK_INFORMATION },
    { SHORTCUT_SHOW_GUEST_LIST,                       STR_SHORTCUT_SHOW_GUEST_LIST },
    { SHORTCUT_SHOW_STAFF_LIST,                       STR_SHORTCUT_SHOW_STAFF_LIST },
    { SHORTCUT_SHOW_RECENT_MESSAGES,                  STR_SHORTCUT_SHOW_RECENT_MESSAGES },

    { SHORTCUT_UNDEFINED,                             STR_NONE },

    { SHORTCUT_SHOW_MULTIPLAYER,                      STR_SHORTCUT_SHOW_MULTIPLAYER },
    { SHORTCUT_OPEN_CHAT_WINDOW,                      STR_SEND_MESSAGE },

    { SHORTCUT_UNDEFINED,                             STR_NONE },

    { SHORTCUT_UNDERGROUND_VIEW_TOGGLE,               STR_SHORTCUT_UNDERGROUND_VIEW_TOGGLE },
    { SHORTCUT_REMOVE_BASE_LAND_TOGGLE,               STR_SHORTCUT_REMOVE_BASE_LAND_TOGGLE },
    { SHORTCUT_REMOVE_VERTICAL_LAND_TOGGLE,           STR_SHORTCUT_REMOVE_VERTICAL_LAND_TOGGLE },
    { SHORTCUT_SEE_THROUGH_RIDES_TOGGLE,              STR_SHORTCUT_SEE_THROUGH_RIDES_TOGGLE },
    { SHORTCUT_SEE_THROUGH_SCENERY_TOGGLE,            STR_SHORTCUT_SEE_THROUGH_SCENERY_TOGGLE },
    { SHORTCUT_SEE_THROUGH_PATHS_TOGGLE,              STR_SHORTCUT_SEE_THROUGH_PATHS_TOGGLE },
    { SHORTCUT_INVISIBLE_SUPPORTS_TOGGLE,             STR_SHORTCUT_INVISIBLE_SUPPORTS_TOGGLE },
    { SHORTCUT_INVISIBLE_PEOPLE_TOGGLE,               STR_SHORTCUT_INVISIBLE_PEOPLE_TOGGLE },
    { SHORTCUT_HEIGHT_MARKS_ON_LAND_TOGGLE,           STR_SHORTCUT_HEIGHT_MARKS_ON_LAND_TOGGLE },
    { SHORTCUT_HEIGHT_MARKS_ON_RIDE_TRACKS_TOGGLE,    STR_SHORTCUT_HEIGHT_MARKS_ON_RIDE_TRACKS_TOGGLE },
    { SHORTCUT_HEIGHT_MARKS_ON_PATHS_TOGGLE,          STR_SHORTCUT_HEIGHT_MARKS_ON_PATHS_TOGGLE },
    { SHORTCUT_VIEW_CLIPPING,                         STR_SHORTCUT_VIEW_CLIPPING },
    { SHORTCUT_HIGHLIGHT_PATH_ISSUES_TOGGLE,          STR_SHORTCUT_HIGHLIGHT_PATH_ISSUES_TOGGLE },
    { SHORTCUT_GRIDLINES_DISPLAY_TOGGLE,              STR_SHORTCUT_GRIDLINES_DISPLAY_TOGGLE },

    { SHORTCUT_UNDEFINED,                             STR_NONE },

    { SHORTCUT_SCENERY_PICKER,                        STR_SHORTCUT_OPEN_SCENERY_PICKER },
    { SHORTCUT_ROTATE_CONSTRUCTION_OBJECT,            STR_SHORTCUT_ROTATE_CONSTRUCTION_OBJECT },
    { SHORTCUT_RIDE_CONSTRUCTION_TURN_LEFT,           STR_SHORTCUT_RIDE_CONSTRUCTION_TURN_LEFT },
    { SHORTCUT_RIDE_CONSTRUCTION_TURN_RIGHT,          STR_SHORTCUT_RIDE_CONSTRUCTION_TURN_RIGHT },
    { SHORTCUT_RIDE_CONSTRUCTION_USE_TRACK_DEFAULT,   STR_SHORTCUT_RIDE_CONSTRUCTION_USE_TRACK_DEFAULT },
    { SHORTCUT_RIDE_CONSTRUCTION_SLOPE_DOWN,          STR_SHORTCUT_RIDE_CONSTRUCTION_SLOPE_DOWN },
    { SHORTCUT_RIDE_CONSTRUCTION_SLOPE_UP,            STR_SHORTCUT_RIDE_CONSTRUCTION_SLOPE_UP },
    { SHORTCUT_RIDE_CONSTRUCTION_CHAIN_LIFT_TOGGLE,   STR_SHORTCUT_RIDE_CONSTRUCTION_CHAIN_LIFT_TOGGLE },
    { SHORTCUT_RIDE_CONSTRUCTION_BANK_LEFT,           STR_SHORTCUT_RIDE_CONSTRUCTION_BANK_LEFT },
    { SHORTCUT_RIDE_CONSTRUCTION_BANK_RIGHT,          STR_SHORTCUT_RIDE_CONSTRUCTION_BANK_RIGHT },
    { SHORTCUT_RIDE_CONSTRUCTION_PREVIOUS_TRACK,      STR_SHORTCUT_RIDE_CONSTRUCTION_PREVIOUS_TRACK },
    { SHORTCUT_RIDE_CONSTRUCTION_NEXT_TRACK,          STR_SHORTCUT_RIDE_CONSTRUCTION_NEXT_TRACK },
    { SHORTCUT_RIDE_CONSTRUCTION_BUILD_CURRENT,       STR_SHORTCUT_RIDE_CONSTRUCTION_BUILD_CURRENT },
    { SHORTCUT_RIDE_CONSTRUCTION_DEMOLISH_CURRENT,    STR_SHORTCUT_RIDE_CONSTRUCTION_DEMOLISH_CURRENT },

    { SHORTCUT_UNDEFINED,                             STR_NONE },

    { SHORTCUT_SCROLL_MAP_UP,                         STR_SHORTCUT_SCROLL_MAP_UP },
    { SHORTCUT_SCROLL_MAP_LEFT,                       STR_SHORTCUT_SCROLL_MAP_LEFT },
    { SHORTCUT_SCROLL_MAP_DOWN,                       STR_SHORTCUT_SCROLL_MAP_DOWN },
    { SHORTCUT_SCROLL_MAP_RIGHT,                      STR_SHORTCUT_SCROLL_MAP_RIGHT },

    { SHORTCUT_UNDEFINED,                             STR_NONE },

    { SHORTCUT_WINDOWED_MODE_TOGGLE,                  STR_SHORTCUT_WINDOWED_MODE_TOGGLE },
    { SHORTCUT_SCALE_UP,                              STR_SHORTCUT_SCALE_UP },
    { SHORTCUT_SCALE_DOWN,                            STR_SHORTCUT_SCALE_DOWN },

    { SHORTCUT_UNDEFINED,                             STR_NONE },

    { SHORTCUT_TILE_INSPECTOR,                        STR_SHORTCUT_OPEN_TILE_INSPECTOR },
    { SHORTCUT_INSERT_CORRUPT_ELEMENT,                STR_SHORTCUT_INSERT_CORRPUT_ELEMENT },
    { SHORTCUT_COPY_ELEMENT,                          STR_SHORTCUT_COPY_ELEMENT },
    { SHORTCUT_PASTE_ELEMENT,                         STR_SHORTCUT_PASTE_ELEMENT },
    { SHORTCUT_REMOVE_ELEMENT,                        STR_SHORTCUT_REMOVE_ELEMENT },
    { SHORTCUT_MOVE_ELEMENT_UP,                       STR_SHORTCUT_MOVE_ELEMENT_UP },
    { SHORTCUT_MOVE_ELEMENT_DOWN,                     STR_SHORTCUT_MOVE_ELEMENT_DOWN },
    { SHORTCUT_INCREASE_X_COORD,                      STR_SHORTCUT_INCREASE_X_COORD },
    { SHORTCUT_DECREASE_X_COORD,                      STR_SHORTCUT_DECREASE_X_COORD },
    { SHORTCUT_INCREASE_Y_COORD,                      STR_SHORTCUT_INCREASE_Y_COORD },
    { SHORTCUT_DECREASE_Y_COORD,                      STR_SHORTCUT_DECREASE_Y_COORD },
    { SHORTCUT_INCREASE_ELEM_HEIGHT,                  STR_SHORTCUT_INCREASE_ELEM_HEIGHT },
    { SHORTCUT_DECREASE_ELEM_HEIGHT,                  STR_SHORTCUT_DECREASE_ELEM_HEIGHT },

    { SHORTCUT_UNDEFINED,                             STR_NONE },

    { SHORTCUT_ADVANCE_TO_NEXT_TICK,                  STR_ADVANCE_TO_NEXT_TICK },
    { SHORTCUT_PAINT_ORIGINAL_TOGGLE,                 STR_SHORTCUT_PAINT_ORIGINAL },
    { SHORTCUT_DEBUG_PAINT_TOGGLE,                    STR_SHORTCUT_DEBUG_PAINT_TOGGLE },
};
// clang-format on

/**
 *
 *  rct2: 0x006E3884
 */
rct_window* window_shortcut_keys_open()
{
    rct_window* w = window_bring_to_front_by_class(WC_KEYBOARD_SHORTCUT_LIST);
    if (w == nullptr)
    {
        w = window_create_auto_pos(WW, WH, &window_shortcut_events, WC_KEYBOARD_SHORTCUT_LIST, WF_RESIZABLE);

        w->widgets = window_shortcut_widgets;
        w->enabled_widgets = (1 << WIDX_CLOSE) | (1 << WIDX_RESET);
        window_init_scroll_widgets(w);

        w->no_list_items = static_cast<uint16_t>(std::size(ShortcutList));
        w->selected_list_item = -1;
        w->min_width = WW;
        w->min_height = WH;
        w->max_width = WW_SC_MAX;
        w->max_height = WH_SC_MAX;
    }
    return w;
}

/**
 *
 *  rct2: 0x006E39E4
 */
static void window_shortcut_mouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex)
    {
        case WIDX_CLOSE:
            window_close(w);
            break;
        case WIDX_RESET:
            keyboard_shortcuts_reset();
            keyboard_shortcuts_save();
            w->Invalidate();
            break;
    }
}

static void window_shortcut_resize(rct_window* w)
{
    window_set_resize(w, w->min_width, w->min_height, w->max_width, w->max_height);
}

static void window_shortcut_invalidate(rct_window* w)
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
static void window_shortcut_paint(rct_window* w, rct_drawpixelinfo* dpi)
{
    window_draw_widgets(w, dpi);
}

/**
 *
 *  rct2: 0x006E3A07
 */
static void window_shortcut_scrollgetsize(rct_window* w, int32_t scrollIndex, int32_t* width, int32_t* height)
{
    *height = w->no_list_items * SCROLLABLE_ROW_HEIGHT;
}

/**
 *
 *  rct2: 0x006E3A3E
 */
static void window_shortcut_scrollmousedown(rct_window* w, int32_t scrollIndex, const ScreenCoordsXY& screenCoords)
{
    int32_t selected_item = (screenCoords.y - 1) / SCROLLABLE_ROW_HEIGHT;
    if (selected_item >= w->no_list_items)
        return;

    // Is this a separator?
    if (ShortcutList[selected_item].ShortcutId == SHORTCUT_UNDEFINED)
        return;

    auto& shortcut = ShortcutList[selected_item];
    window_shortcut_change_open(shortcut.ShortcutId, shortcut.StringId);
}

/**
 *
 *  rct2: 0x006E3A16
 */
static void window_shortcut_scrollmouseover(rct_window* w, int32_t scrollIndex, const ScreenCoordsXY& screenCoords)
{
    int32_t selected_item = (screenCoords.y - 1) / SCROLLABLE_ROW_HEIGHT;
    if (selected_item >= w->no_list_items)
        return;

    w->selected_list_item = selected_item;

    w->Invalidate();
}

/**
 *
 *  rct2: 0x006E38E6
 */
static void window_shortcut_scrollpaint(rct_window* w, rct_drawpixelinfo* dpi, int32_t scrollIndex)
{
    auto dpiCoords = ScreenCoordsXY{ dpi->x, dpi->y };
    gfx_fill_rect(
        dpi, { dpiCoords, dpiCoords + ScreenCoordsXY{ dpi->width - 1, dpi->height - 1 } }, ColourMapA[w->colours[1]].mid_light);

    // TODO: the line below is a workaround for what is presumably a bug with dpi->width
    //       see https://github.com/OpenRCT2/OpenRCT2/issues/11238 for details
    const auto scrollWidth = w->width - SCROLLBAR_WIDTH - 10;

    for (int32_t i = 0; i < w->no_list_items; ++i)
    {
        int32_t y = 1 + i * SCROLLABLE_ROW_HEIGHT;
        if (y > dpi->y + dpi->height)
        {
            break;
        }

        if (y + SCROLLABLE_ROW_HEIGHT < dpi->y)
        {
            continue;
        }

        // Is this a separator?
        if (ShortcutList[i].ShortcutId == SHORTCUT_UNDEFINED)
        {
            const int32_t top = y + (SCROLLABLE_ROW_HEIGHT / 2) - 1;
            gfx_fill_rect(dpi, { { 0, top }, { scrollWidth, top } }, ColourMapA[w->colours[0]].mid_dark);
            gfx_fill_rect(dpi, { { 0, top + 1 }, { scrollWidth, top + 1 } }, ColourMapA[w->colours[0]].lightest);
            continue;
        }

        int32_t format = STR_BLACK_STRING;
        if (i == w->selected_list_item)
        {
            format = STR_WINDOW_COLOUR_2_STRINGID;
            gfx_filter_rect(dpi, 0, y - 1, scrollWidth, y + (SCROLLABLE_ROW_HEIGHT - 2), PALETTE_DARKEN_1);
        }

        const int32_t bindingOffset = scrollWidth - 150;
        auto ft = Formatter::Common();
        ft.Add<rct_string_id>(STR_SHORTCUT_ENTRY_FORMAT);
        ft.Add<rct_string_id>(ShortcutList[i].StringId);
        gfx_draw_string_left_clipped(dpi, format, gCommonFormatArgs, COLOUR_BLACK, { 0, y - 1 }, bindingOffset);

        char keybinding[128];
        keyboard_shortcuts_format_string(keybinding, 128, ShortcutList[i].ShortcutId);

        if (strlen(keybinding) > 0)
        {
            const int32_t maxWidth = 150;
            ft = Formatter::Common();
            ft.Add<rct_string_id>(STR_STRING);
            ft.Add<char*>(keybinding);
            gfx_draw_string_left_clipped(dpi, format, gCommonFormatArgs, COLOUR_BLACK, { bindingOffset, y - 1 }, maxWidth);
        }
    }
}
