/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <openrct2/config/Config.h>
#include <openrct2/Context.h>
#include <openrct2-ui/windows/Window.h>

#include <openrct2/Intro.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2-ui/interface/Widget.h>

// clang-format off
enum WINDOW_TITLE_OPTIONS_WIDGET_IDX {
    WIDX_OPTIONS,
};

static rct_widget window_title_options_widgets[] = {
    { WWT_BUTTON, 2, 0, 79, 0, 14, STR_OPTIONS, STR_OPTIONS_TIP },
    { WIDGETS_END },
};

static void window_title_options_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_title_options_paint(rct_window *w, rct_drawpixelinfo *dpi);

static rct_window_event_list window_title_options_events = {
    nullptr,
    window_title_options_mouseup,
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
    window_title_options_paint,
    nullptr
};
// clang-format on

/**
 * Creates the window containing the options button on the title screen.
 */
rct_window * window_title_options_open()
{
    rct_window * window = window_create(
        context_get_width() - 80, 0,
        80, 15,
        &window_title_options_events,
        WC_TITLE_OPTIONS,
        WF_STICK_TO_BACK | WF_TRANSPARENT
    );
    window->widgets = window_title_options_widgets;
    window->enabled_widgets |= (1ULL << WIDX_OPTIONS);
    window_init_scroll_widgets(window);

    return window;
}

static void window_title_options_mouseup(rct_window *w, rct_widgetindex widgetIndex)
{
    if (gIntroState != INTRO_STATE_NONE)
        return;

    switch (widgetIndex) {
    case WIDX_OPTIONS:
        context_open_window(WC_OPTIONS);
        break;
    }
}

static void window_title_options_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
    window_draw_widgets(w, dpi);
}

