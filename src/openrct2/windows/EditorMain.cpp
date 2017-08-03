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

#include "../Context.h"

extern "C"
{
#include "../interface/viewport.h"
#include "../interface/widget.h"
#include "../world/footpath.h"
}

static void window_editor_main_paint(rct_window *w, rct_drawpixelinfo *dpi);

static rct_window_event_list window_editor_main_events = {
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    window_editor_main_paint,// 0x0066FC97, // window_editor_main_paint,
    NULL,
};

static rct_widget window_editor_main_widgets[] = {
    { WWT_VIEWPORT, 0, 0, -1, 0, -1, 0xFFFFFFFE, 0xFFFF },
    { WIDGETS_END },
};


/**
* Creates the main editor window that holds the main viewport.
*  rct2: 0x0066EF38
*/
rct_window * window_editor_main_open()
{
    window_editor_main_widgets[0].right = context_get_width();
    window_editor_main_widgets[0].bottom = context_get_height();
    rct_window *window = window_create(0, 0, window_editor_main_widgets[0].right, window_editor_main_widgets[0].bottom,
        &window_editor_main_events, WC_MAIN_WINDOW, WF_STICK_TO_BACK);
    window->widgets = window_editor_main_widgets;

    viewport_create(window, window->x, window->y, window->width, window->height, 0, 0x0FFF, 0x0FFF, 0, 0x1, -1);
    window->viewport->flags |= 0x0400;

    gCurrentRotation = 0;
    gShowGridLinesRefCount = 0;
    gShowLandRightsRefCount = 0;
    gShowConstuctionRightsRefCount = 0;
    gFootpathSelectedType = 0;

    window_top_toolbar_open();
    window_editor_bottom_toolbar_open();

    return window_get_main();
}

/**
*
* rct2: 0x0066FC97
* This function immediately jumps to 0x00685BE1
*/
static void window_editor_main_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
    viewport_render(dpi, w->viewport, dpi->x, dpi->y, dpi->x + dpi->width, dpi->y + dpi->height);
}
