/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <openrct2-ui/windows/Window.h>

#include <openrct2/localisation/Localisation.h>
#include <openrct2/sprites.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2/interface/Colour.h>
#include <openrct2/drawing/Drawing.h>

// clang-format off
static rct_widget window_title_logo_widgets[] = {
    { WIDGETS_END },
};

static void window_title_logo_paint(rct_window *w, rct_drawpixelinfo *dpi);

static rct_window_event_list window_title_logo_events = {
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
rct_window * window_title_logo_open()
{
    rct_window *window = window_create(0, 0, 232, 136, &window_title_logo_events, WC_TITLE_LOGO, WF_STICK_TO_BACK | WF_TRANSPARENT);
    window->widgets = window_title_logo_widgets;
    window_init_scroll_widgets(window);
    window->colours[0] = TRANSLUCENT(COLOUR_GREY);
    window->colours[1] = TRANSLUCENT(COLOUR_GREY);
    window->colours[2] = TRANSLUCENT(COLOUR_GREY);

    return window;
}

/**
*
*  rct2: 0x0066B872
*/
static void window_title_logo_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
    int32_t x = 2;
    int32_t y = 2;
    gfx_draw_sprite(dpi, SPR_G2_LOGO, w->x + x, w->y + y, 0);
    gfx_draw_sprite(dpi, SPR_G2_TITLE, w->x + x + 104, w->y + y + 18, 0);
}
