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
#include <openrct2/sprites.h>

// clang-format off
enum WindowTitleExitWidgetIdx {
    WIDX_EXIT,
};

static rct_widget window_title_exit_widgets[] = {
    MakeWidget({0, 0}, {40, 64}, WindowWidgetType::ImgBtn, WindowColour::Tertiary, SPR_MENU_EXIT, STR_EXIT),
    WIDGETS_END,
};

static void WindowTitleExitPaint(rct_window *w, rct_drawpixelinfo *dpi);
static void WindowTitleExitMouseup(rct_window *w, rct_widgetindex widgetIndex);

static rct_window_event_list window_title_exit_events([](auto& events)
{
    events.mouse_up = &WindowTitleExitMouseup;
    events.paint = &WindowTitleExitPaint;
});
// clang-format on

/**
 * Creates the window containing the exit button on the title screen.
 *  rct2: 0x0066B624 (part of 0x0066B3E8)
 */
rct_window* WindowTitleExitOpen()
{
    rct_window* window;

    window = WindowCreate(
        ScreenCoordsXY(context_get_width() - 40, context_get_height() - 64), 40, 64, &window_title_exit_events, WC_TITLE_EXIT,
        WF_STICK_TO_BACK | WF_TRANSPARENT);
    window->widgets = window_title_exit_widgets;
    WindowInitScrollWidgets(*window);

    return window;
}

/**
 *
 *  rct2: 0x0066B83C
 */
static void WindowTitleExitMouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    if (gIntroState != IntroState::None)
        return;

    switch (widgetIndex)
    {
        case WIDX_EXIT:
            context_quit();
            // game_do_command(0, 1, 0, 0, 5, 3, 0);
            break;
    };
}

/**
 *
 *  rct2: 0x0066B836
 */
static void WindowTitleExitPaint(rct_window* w, rct_drawpixelinfo* dpi)
{
    WindowDrawWidgets(*w, dpi);
}
