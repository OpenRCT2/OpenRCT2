/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <openrct2-ui/input/KeyboardShortcuts.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/interface/Window.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/config/Config.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/localisation/Localisation.h>

static constexpr const rct_string_id WINDOW_TITLE = STR_SHORTCUT_CHANGE_TITLE;
static constexpr const int32_t WW = 250;
static constexpr const int32_t WH = 60;

// clang-format off
enum WINDOW_SHORTCUT_CHANGE_WIDGET_IDX {
    WIDX_BACKGROUND,
    WIDX_TITLE,
    WIDX_CLOSE,
};

// 0x9DE4E0
static rct_widget window_shortcut_change_widgets[] = {
    WINDOW_SHIM(WINDOW_TITLE, WW, WH),
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

static rct_string_id CurrentShortcutKeyStringId{};

rct_window* window_shortcut_change_open(OpenRCT2::Input::Shortcut shortcut, rct_string_id key_string_id)
{
    // Move this to window_shortcut_change_open
    window_close_by_class(WC_CHANGE_KEYBOARD_SHORTCUT);

    // Save the item we are selecting for new window
    gKeyboardShortcutChangeId = shortcut;
    CurrentShortcutKeyStringId = key_string_id;

    rct_window* w = window_create_centred(WW, WH, &window_shortcut_change_events, WC_CHANGE_KEYBOARD_SHORTCUT, 0);

    w->widgets = window_shortcut_change_widgets;
    w->enabled_widgets = (1ULL << WIDX_CLOSE);
    window_init_scroll_widgets(w);
    return w;
}

/**
 *
 *  rct2: 0x006E3AE0
 */
static void window_shortcut_change_mouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex)
    {
        case WIDX_CLOSE:
            window_close(w);
            break;
    }
}

/**
 *
 *  rct2: 0x006E3A9F
 */
static void window_shortcut_change_paint(rct_window* w, rct_drawpixelinfo* dpi)
{
    window_draw_widgets(w, dpi);

    ScreenCoordsXY stringCoords(w->windowPos.x + 125, w->windowPos.y + 30);

    auto ft = Formatter::Common();
    ft.Add<rct_string_id>(CurrentShortcutKeyStringId);
    gfx_draw_string_centred_wrapped(dpi, gCommonFormatArgs, stringCoords, 242, STR_SHORTCUT_CHANGE_PROMPT, COLOUR_BLACK);
}
