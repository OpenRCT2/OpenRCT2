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
#include <openrct2/drawing/Drawing.h>
#include <openrct2/interface/Colour.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/sprites.h>

static constexpr const int32_t WW = 232;
static constexpr const int32_t WH = 136;

enum
{
    WIDX_LOGO
};

static rct_widget window_title_logo_widgets[] = {
    MakeWidget({ 0, 0 }, { WW + 1, WH + 1 }, WindowWidgetType::ImgBtn, WindowColour::Primary),
    WIDGETS_END,
};

static void WindowTitleMenuMouseup(rct_window* w, rct_widgetindex widgetIndex);
static void WindowTitleLogoPaint(rct_window* w, rct_drawpixelinfo* dpi);

// clang-format off
static rct_window_event_list window_title_logo_events([](auto& events)
{
    events.mouse_up = &WindowTitleMenuMouseup;
    events.paint = &WindowTitleLogoPaint;
});
// clang-format on

/**
 * Creates the window containing the logo and the expansion packs on the title screen.
 *  rct2: 0x0066B679 (part of 0x0066B3E8)
 */
rct_window* WindowTitleLogoOpen()
{
    rct_window* window = WindowCreate(
        ScreenCoordsXY(0, 0), WW, WH, &window_title_logo_events, WC_TITLE_LOGO, WF_STICK_TO_BACK | WF_TRANSPARENT);
    window->widgets = window_title_logo_widgets;
    WindowInitScrollWidgets(window);
    window->colours[0] = TRANSLUCENT(COLOUR_GREY);
    window->colours[1] = TRANSLUCENT(COLOUR_GREY);
    window->colours[2] = TRANSLUCENT(COLOUR_GREY);

    return window;
}

static void WindowTitleMenuMouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex)
    {
        case WIDX_LOGO:
            WindowAboutOpen();
            break;
    }
}

/**
 *
 *  rct2: 0x0066B872
 */
static void WindowTitleLogoPaint(rct_window* w, rct_drawpixelinfo* dpi)
{
    auto screenCoords = w->windowPos + ScreenCoordsXY{ 2, 2 };
    gfx_draw_sprite(dpi, ImageId(SPR_G2_LOGO), screenCoords);
    gfx_draw_sprite(dpi, ImageId(SPR_G2_TITLE), screenCoords + ScreenCoordsXY{ 104, 18 });
}
