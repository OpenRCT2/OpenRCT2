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

static void WindowEditorMainPaint(rct_window* w, rct_drawpixelinfo* dpi);

// clang-format off
static rct_window_event_list window_editor_main_events([](auto& events) {
    events.paint = &WindowEditorMainPaint;
});
// clang-format on

static rct_widget window_editor_main_widgets[] = {
    MakeWidget({ 0, 0 }, { 0, 0 }, WindowWidgetType::Viewport, WindowColour::Primary, STR_VIEWPORT),
    WIDGETS_END,
};

/**
 * Creates the main editor window that holds the main viewport.
 *  rct2: 0x0066EF38
 */
rct_window* WindowEditorMainOpen()
{
    window_editor_main_widgets[0].right = context_get_width();
    window_editor_main_widgets[0].bottom = context_get_height();
    rct_window* window = WindowCreate(
        ScreenCoordsXY(0, 0), window_editor_main_widgets[0].right, window_editor_main_widgets[0].bottom,
        &window_editor_main_events, WC_MAIN_WINDOW, WF_STICK_TO_BACK);
    window->widgets = window_editor_main_widgets;

    viewport_create(window, window->windowPos, window->width, window->height, Focus(CoordsXYZ(0x0FFF, 0x0FFF, 0)));
    window->viewport->flags |= 0x0400;

    gCurrentRotation = 0;
    gShowGridLinesRefCount = 0;
    gShowLandRightsRefCount = 0;
    gShowConstuctionRightsRefCount = 0;
    WindowFootpathResetSelectedPath();

    context_open_window(WC_TOP_TOOLBAR);
    context_open_window_view(WV_EDITOR_BOTTOM_TOOLBAR);

    return window_get_main();
}

/**
 *
 * rct2: 0x0066FC97
 * This function immediately jumps to 0x00685BE1
 */
static void WindowEditorMainPaint(rct_window* w, rct_drawpixelinfo* dpi)
{
    viewport_render(dpi, w->viewport, { { dpi->x, dpi->y }, { dpi->x + dpi->width, dpi->y + dpi->height } });
}
