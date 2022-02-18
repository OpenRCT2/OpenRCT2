/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/Context.h>
#include <openrct2/Intro.h>
#include <openrct2/config/Config.h>
#include <openrct2/localisation/Localisation.h>

// clang-format off
enum WindowTitleOptionsWidgetIdx {
    WIDX_OPTIONS,
};

static rct_widget window_title_options_widgets[] = {
    MakeWidget({0, 0}, {80, 15}, WindowWidgetType::Button, WindowColour::Tertiary, STR_OPTIONS, STR_OPTIONS_TIP),
    WIDGETS_END,
};

static void WindowTitleOptionsMouseup(rct_window *w, rct_widgetindex widgetIndex);
static void WindowTitleOptionsPaint(rct_window *w, rct_drawpixelinfo *dpi);

static rct_window_event_list window_title_options_events([](auto& events)
{
    events.mouse_up = &WindowTitleOptionsMouseup;
    events.paint = &WindowTitleOptionsPaint;
});
// clang-format on

/**
 * Creates the window containing the options button on the title screen.
 */
rct_window* WindowTitleOptionsOpen()
{
    rct_window* window = WindowCreate(
        ScreenCoordsXY(context_get_width() - 80, 0), 80, 15, &window_title_options_events, WC_TITLE_OPTIONS,
        WF_STICK_TO_BACK | WF_TRANSPARENT);
    window->widgets = window_title_options_widgets;
    WindowInitScrollWidgets(window);

    return window;
}

static void WindowTitleOptionsMouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    if (gIntroState != IntroState::None)
        return;

    switch (widgetIndex)
    {
        case WIDX_OPTIONS:
            context_open_window(WC_OPTIONS);
            break;
    }
}

static void WindowTitleOptionsPaint(rct_window* w, rct_drawpixelinfo* dpi)
{
    WindowDrawWidgets(w, dpi);
}
