/*****************************************************************************
 * Copyright (c) 2014-2019 OpenRCT2 developers
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
    { WWT_IMGBTN, 0, 0, WW, 0, WH, STR_NONE, STR_NONE },
    { WIDGETS_END },
};

static void window_title_menu_mouseup(rct_window* w, rct_widgetindex widgetIndex);
static void window_title_logo_paint(rct_window* w, rct_drawpixelinfo* dpi);

// clang-format off
static rct_window_event_list window_title_logo_events = {
    nullptr,
    window_title_menu_mouseup,
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
    window_title_logo_paint,
    nullptr
};
// clang-format on

/**
 * Creates the window containing the logo and the expansion packs on the title screen.
 *  rct2: 0x0066B679 (part of 0x0066B3E8)
 */
rct_window* window_title_logo_open()
{
    rct_window* window = window_create(
        ScreenCoordsXY(0, 0), WW, WH, &window_title_logo_events, WC_TITLE_LOGO, WF_STICK_TO_BACK | WF_TRANSPARENT);
    window->widgets = window_title_logo_widgets;
    window_init_scroll_widgets(window);
    window->colours[0] = TRANSLUCENT(COLOUR_GREY);
    window->colours[1] = TRANSLUCENT(COLOUR_GREY);
    window->colours[2] = TRANSLUCENT(COLOUR_GREY);
    window->enabled_widgets = (1 << WIDX_LOGO);

    return window;
}

static void window_title_menu_mouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex)
    {
        case WIDX_LOGO:
            window_about_open();
            break;
    }
}

/**
 *
 *  rct2: 0x0066B872
 */
static void window_title_logo_paint(rct_window* w, rct_drawpixelinfo* dpi)
{
    int32_t x = 2;
    int32_t y = 2;
    gfx_draw_sprite(dpi, SPR_G2_LOGO, w->windowPos.x + x, w->windowPos.y + y, 0);
    gfx_draw_sprite(dpi, SPR_G2_TITLE, w->windowPos.x + x + 104, w->windowPos.y + y + 18, 0);
}
