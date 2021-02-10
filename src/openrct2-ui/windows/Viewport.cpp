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
#include <openrct2/Game.h>
#include <openrct2/audio/audio.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/sprites.h>

static constexpr const rct_string_id WINDOW_TITLE = STR_VIEWPORT_NO;
static constexpr const int32_t WH = 500;
static constexpr const int32_t WW = 350;

// clang-format off
enum {
    WIDX_BACKGROUND,
    WIDX_TITLE,
    WIDX_CLOSE,
    WIDX_PAGE_BACKGROUND,
    WIDX_VIEWPORT,
    WIDX_ZOOM_IN,
    WIDX_ZOOM_OUT,
    WIDX_LOCATE
};

static rct_widget window_viewport_widgets[] = {
    WINDOW_SHIM(WINDOW_TITLE, WW, WH),
    MakeWidget({      0, 14}, { WW - 1, WH - 1}, WindowWidgetType::Resize,   WindowColour::Secondary                                         ), // resize
    MakeWidget({      3, 17}, {WW - 26, WH - 3}, WindowWidgetType::Viewport, WindowColour::Primary                                           ), // viewport
    MakeWidget({WW - 25, 17}, {     24,     24}, WindowWidgetType::FlatBtn,  WindowColour::Primary  , SPR_G2_ZOOM_IN,  STR_ZOOM_IN_TIP       ), // zoom in
    MakeWidget({WW - 25, 41}, {     24,     24}, WindowWidgetType::FlatBtn,  WindowColour::Primary  , SPR_G2_ZOOM_OUT, STR_ZOOM_OUT_TIP      ), // zoom out
    MakeWidget({WW - 25, 65}, {     24,     24}, WindowWidgetType::FlatBtn,  WindowColour::Primary  , SPR_LOCATE,      STR_LOCATE_SUBJECT_TIP), // locate
    { WIDGETS_END },
};

static void window_viewport_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_viewport_resize(rct_window *w);
static void window_viewport_update(rct_window *w);
static void window_viewport_invalidate(rct_window *w);
static void window_viewport_paint(rct_window *w, rct_drawpixelinfo *dpi);

static rct_window_event_list window_viewport_events([](auto& events)
{
    events.mouse_up = &window_viewport_mouseup;
    events.resize = &window_viewport_resize;
    events.update = &window_viewport_update;
    events.invalidate = &window_viewport_invalidate;
    events.paint = &window_viewport_paint;
});
// clang-format on

static int32_t _viewportNumber = 1;

/**
 * Creates a custom viewport window.
 */
rct_window* window_viewport_open()
{
    rct_window* w = WindowCreateAutoPos(WW, WH, &window_viewport_events, WC_VIEWPORT, WF_RESIZABLE);
    w->widgets = window_viewport_widgets;
    w->enabled_widgets = (1 << WIDX_CLOSE) | (1 << WIDX_ZOOM_IN) | (1 << WIDX_ZOOM_OUT) | (1 << WIDX_LOCATE);
    w->number = _viewportNumber++;

    // Create viewport
    viewport_create(w, w->windowPos, w->width, w->height, 0, TileCoordsXYZ(128, 128, 0).ToCoordsXYZ(), 1, SPRITE_INDEX_NULL);
    rct_window* mainWindow = window_get_main();
    if (mainWindow != nullptr)
    {
        rct_viewport* mainViewport = mainWindow->viewport;
        int32_t x = mainViewport->viewPos.x + (mainViewport->view_width / 2);
        int32_t y = mainViewport->viewPos.y + (mainViewport->view_height / 2);
        w->savedViewPos = { x - (w->viewport->view_width / 2), y - (w->viewport->view_height / 2) };
    }

    w->viewport->flags |= VIEWPORT_FLAG_SOUND_ON;

    return w;
}

static void window_viewport_anchor_border_widgets(rct_window* w)
{
    w->widgets[WIDX_BACKGROUND].right = w->width - 1;
    w->widgets[WIDX_BACKGROUND].bottom = w->height - 1;
    w->widgets[WIDX_PAGE_BACKGROUND].right = w->width - 1;
    w->widgets[WIDX_PAGE_BACKGROUND].bottom = w->height - 1;
    w->widgets[WIDX_TITLE].right = w->width - 2;
    w->widgets[WIDX_CLOSE].left = w->width - 13;
    w->widgets[WIDX_CLOSE].right = w->width - 3;
}

static void window_viewport_mouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    rct_window* mainWindow;

    switch (widgetIndex)
    {
        case WIDX_CLOSE:
            window_close(w);
            break;
        case WIDX_ZOOM_IN:
            if (w->viewport != nullptr && w->viewport->zoom > ZoomLevel::min())
            {
                w->viewport->zoom--;
                w->Invalidate();
            }
            break;
        case WIDX_ZOOM_OUT:
            if (w->viewport != nullptr && w->viewport->zoom < ZoomLevel::max())
            {
                w->viewport->zoom++;
                w->Invalidate();
            }
            break;
        case WIDX_LOCATE:
            mainWindow = window_get_main();
            if (mainWindow != nullptr)
            {
                auto info = get_map_coordinates_from_pos(
                    { w->windowPos.x + (w->width / 2), w->windowPos.y + (w->height / 2) }, ViewportInteractionItemAll);
                window_scroll_to_location(mainWindow, { info.Loc, tile_element_height(info.Loc) });
            }
            break;
    }
}

static void window_viewport_resize(rct_window* w)
{
    w->flags |= WF_RESIZABLE;
    window_set_resize(w, 200, 200, 2000, 2000);
}

static void window_viewport_update(rct_window* w)
{
    rct_window* mainWindow;

    mainWindow = window_get_main();
    if (mainWindow == nullptr)
        return;

    if (w->viewport->flags != mainWindow->viewport->flags)
    {
        w->viewport->flags = mainWindow->viewport->flags;
        w->Invalidate();
    }

    // Not sure how to invalidate part of the viewport that has changed, this will have to do for now
    // widget_invalidate(w, WIDX_VIEWPORT);
}

static void window_viewport_invalidate(rct_window* w)
{
    rct_widget* viewportWidget;
    rct_viewport* viewport;
    int32_t i;

    viewportWidget = &window_viewport_widgets[WIDX_VIEWPORT];
    viewport = w->viewport;

    // Anchor widgets
    window_viewport_anchor_border_widgets(w);
    viewportWidget->right = w->width - 26;
    viewportWidget->bottom = w->height - 3;
    for (i = WIDX_ZOOM_IN; i <= WIDX_LOCATE; i++)
    {
        window_viewport_widgets[i].left = w->width - 25;
        window_viewport_widgets[i].right = w->width - 2;
    }

    // Set title
    Formatter::Common().Add<uint32_t>(w->number);

    // Set disabled widgets
    w->disabled_widgets = 0;
    if (viewport->zoom == ZoomLevel::min())
        w->disabled_widgets |= 1 << WIDX_ZOOM_IN;
    if (viewport->zoom >= ZoomLevel::max())
        w->disabled_widgets |= 1 << WIDX_ZOOM_OUT;

    viewport->pos = w->windowPos + ScreenCoordsXY{ viewportWidget->left, viewportWidget->top };
    viewport->width = viewportWidget->width();
    viewport->height = viewportWidget->height();
    viewport->view_width = viewport->width * viewport->zoom;
    viewport->view_height = viewport->height * viewport->zoom;
}

static void window_viewport_paint(rct_window* w, rct_drawpixelinfo* dpi)
{
    WindowDrawWidgets(w, dpi);

    // Draw viewport
    if (w->viewport != nullptr)
        window_draw_viewport(dpi, w);
}
