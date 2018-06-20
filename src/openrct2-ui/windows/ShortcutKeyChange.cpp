/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <openrct2/config/Config.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2-ui/input/KeyboardShortcuts.h>

#include <openrct2-ui/interface/Window.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/drawing/Drawing.h>

#define WW 250
#define WH 60

// clang-format off
enum WINDOW_SHORTCUT_CHANGE_WIDGET_IDX {
    WIDX_BACKGROUND,
    WIDX_TITLE,
    WIDX_CLOSE,
};

// 0x9DE4E0
static rct_widget window_shortcut_change_widgets[] = {
    { WWT_FRAME,            0,  0,          WW - 1, 0,      WH - 1,     STR_NONE,                   STR_NONE },
    { WWT_CAPTION,          0,  1,          WW - 2, 1,      14,         STR_SHORTCUT_CHANGE_TITLE,  STR_WINDOW_TITLE_TIP },
    { WWT_CLOSEBOX,         0,  WW-13,      WW - 3, 2,      13,         STR_CLOSE_X,                STR_CLOSE_WINDOW_TIP },
    { WIDGETS_END }
};

static void window_shortcut_change_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_shortcut_change_paint(rct_window *w, rct_drawpixelinfo *dpi);

// 0x9A3F7C
static rct_window_event_list window_shortcut_change_events = {
    nullptr,
    window_shortcut_change_mouseup,
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
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    window_shortcut_change_paint,
    nullptr
};
// clang-format on

rct_window * window_shortcut_change_open(int32_t selected_key)
{
    // Move this to window_shortcut_change_open
    window_close_by_class(WC_CHANGE_KEYBOARD_SHORTCUT);
    // Save the item we are selecting for new window
    gKeyboardShortcutChangeId = selected_key;
    rct_window * w = window_create_centred(WW, WH, &window_shortcut_change_events, WC_CHANGE_KEYBOARD_SHORTCUT, 0);

    w->widgets = window_shortcut_change_widgets;
    w->enabled_widgets = (1ULL << WIDX_CLOSE);
    window_init_scroll_widgets(w);
    return w;
}

/**
*
*  rct2: 0x006E3AE0
*/
static void window_shortcut_change_mouseup(rct_window *w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex){
    case WIDX_CLOSE:
        window_close(w);
        break;
    }
}

/**
*
*  rct2: 0x006E3A9F
*/
static void window_shortcut_change_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
    window_draw_widgets(w, dpi);

    int32_t x = w->x + 125;
    int32_t y = w->y + 30;

    set_format_arg(0, rct_string_id, ShortcutStringIds[gKeyboardShortcutChangeId]);
    gfx_draw_string_centred_wrapped(dpi, gCommonFormatArgs, x, y, 242, STR_SHORTCUT_CHANGE_PROMPT, COLOUR_BLACK);
}
