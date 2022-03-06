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
#include <openrct2/config/Config.h>
#include <openrct2/localisation/StringIds.h>
#include <openrct2/world/Footpath.h>

// clang-format off
static rct_widget window_main_widgets[] = {
    MakeWidget({0, 0}, {0, 0}, WindowWidgetType::Viewport, WindowColour::Primary, STR_VIEWPORT),
    WIDGETS_END,
};

static void WindowMainSetViewportFlags(rct_viewport& viewport);
static void WindowMainPaint(rct_window *w, rct_drawpixelinfo *dpi);

static rct_window_event_list window_main_events([](auto& events)
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
    window_main_widgets[0].right = context_get_width();
    window_main_widgets[0].bottom = context_get_height();
    rct_window* window = WindowCreate(
        ScreenCoordsXY(0, 0), window_main_widgets[0].right, window_main_widgets[0].bottom, &window_main_events, WC_MAIN_WINDOW,
        WF_STICK_TO_BACK);
    window->widgets = window_main_widgets;

    viewport_create(window, window->windowPos, window->width, window->height, Focus(CoordsXYZ(0x0FFF, 0x0FFF, 0)));
    if (window->viewport != nullptr)
    {
        WindowMainSetViewportFlags(*window->viewport);
    }
    gCurrentRotation = 0;
    gShowGridLinesRefCount = 0;
    gShowLandRightsRefCount = 0;
    gShowConstuctionRightsRefCount = 0;
    WindowFootpathResetSelectedPath();

    return window;
}

static void WindowMainSetViewportFlags(rct_viewport& viewport)
{
    viewport.flags |= VIEWPORT_FLAG_SOUND_ON;
    if (gConfigGeneral.invisible_rides)
        viewport.flags |= VIEWPORT_FLAG_INVISIBLE_RIDES;
    if (gConfigGeneral.invisible_vehicles)
        viewport.flags |= VIEWPORT_FLAG_INVISIBLE_VEHICLES;
    if (gConfigGeneral.invisible_trees)
        viewport.flags |= VIEWPORT_FLAG_INVISIBLE_TREES;
    if (gConfigGeneral.invisible_scenery)
        viewport.flags |= VIEWPORT_FLAG_INVISIBLE_SCENERY;
    if (gConfigGeneral.invisible_paths)
        viewport.flags |= VIEWPORT_FLAG_INVISIBLE_PATHS;
    if (gConfigGeneral.invisible_supports)
        viewport.flags |= VIEWPORT_FLAG_INVISIBLE_SUPPORTS;
}

/**
 *
 *  rct2: 0x66CCAE
 * This function immediately jumps to 0x00685BE1 this is the second function
 * decompiled.
 */
static void WindowMainPaint(rct_window* w, rct_drawpixelinfo* dpi)
{
    viewport_render(dpi, w->viewport, { { dpi->x, dpi->y }, { dpi->x + dpi->width, dpi->y + dpi->height } });
}
