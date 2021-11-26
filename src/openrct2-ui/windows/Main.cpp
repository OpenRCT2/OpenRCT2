/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <openrct2-ui/interface/Viewport.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/Context.h>
#include <openrct2/localisation/StringIds.h>
#include <openrct2/world/Footpath.h>

// clang-format off
static rct_widget _windowMainWidgets[] = {
    MakeWidget({0, 0}, {0, 0}, WindowWidgetType::Viewport, WindowColour::Primary, STR_VIEWPORT),
    WIDGETS_END,
};

void WindowMainPaint(rct_window *w, rct_drawpixelinfo *dpi);

static rct_window_event_list _windowMainEvents([](auto& events)
{
    events.paint = &WindowMainPaint;
});
// clang-format on

/**
 * Creates the main window that holds the main viewport.
 *  rct2: 0x0066B3E8
 */
rct_window* WindowMainOpen()
{
    _windowMainWidgets[0].right = context_get_width();
    _windowMainWidgets[0].bottom = context_get_height();
    rct_window* window = WindowCreate(
        ScreenCoordsXY(0, 0), _windowMainWidgets[0].right, _windowMainWidgets[0].bottom, &_windowMainEvents, WC_MAIN_WINDOW,
        WF_STICK_TO_BACK);
    window->widgets = _windowMainWidgets;

    viewport_create(window, window->windowPos, window->width, window->height, Focus(CoordsXYZ(0x0FFF, 0x0FFF, 0)));
    window->viewport->flags |= VIEWPORT_FLAG_SOUND_ON;
    gCurrentRotation = 0;
    gShowGridLinesRefCount = 0;
    gShowLandRightsRefCount = 0;
    gShowConstuctionRightsRefCount = 0;
    WindowFootpathResetSelectedPath();

    return window;
}

/**
 *
 *  rct2: 0x66CCAE
 * This function immediately jumps to 0x00685BE1 this is the second function
 * decompiled.
 */
void WindowMainPaint(rct_window* w, rct_drawpixelinfo* dpi)
{
    viewport_render(dpi, w->viewport, { { dpi->x, dpi->y }, { dpi->x + dpi->width, dpi->y + dpi->height } });
}
