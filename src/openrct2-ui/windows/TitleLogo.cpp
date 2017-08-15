#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#include <openrct2-ui/windows/Window.h>

extern "C"
{
    #include <openrct2/localisation/localisation.h>
    #include <openrct2/sprites.h>
    #include <openrct2/interface/widget.h>
}

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

/**
 * Creates the window containing the logo and the expansion packs on the title screen.
 *  rct2: 0x0066B679 (part of 0x0066B3E8)
 */
rct_window * window_title_logo_open()
{
    rct_window *window = window_create(0, 0, 200, 106, &window_title_logo_events, WC_TITLE_LOGO, WF_STICK_TO_BACK | WF_TRANSPARENT);
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
    sint32 x = 2;
    sint32 y = 2;
    gfx_draw_sprite(dpi, SPR_G2_LOGO, w->x + x, w->y + y, 0);
    gfx_draw_sprite(dpi, SPR_G2_TITLE, w->x + x + 104, w->y + y + 18, 0);
}
