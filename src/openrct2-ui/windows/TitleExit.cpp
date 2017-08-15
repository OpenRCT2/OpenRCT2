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

#include <openrct2/config/Config.h>
#include <openrct2/Context.h>
#include <openrct2-ui/windows/Window.h>

extern "C"
{
    #include <openrct2/sprites.h>
    #include <openrct2/localisation/localisation.h>
    #include <openrct2/interface/widget.h>
    #include <openrct2/intro.h>
}

enum WINDOW_TITLE_EXIT_WIDGET_IDX {
    WIDX_EXIT,
};

static rct_widget window_title_exit_widgets[] = {
    { WWT_IMGBTN, 2, 0, 39, 0, 63, SPR_MENU_EXIT, STR_EXIT },
    { WIDGETS_END },
};

static void window_title_exit_paint(rct_window *w, rct_drawpixelinfo *dpi);
static void window_title_exit_mouseup(rct_window *w, rct_widgetindex widgetIndex);

static rct_window_event_list window_title_exit_events = {
    nullptr,
    window_title_exit_mouseup,
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
    window_title_exit_paint,
    nullptr
};

/**
 * Creates the window containing the exit button on the title screen.
 *  rct2: 0x0066B624 (part of 0x0066B3E8)
 */
rct_window * window_title_exit_open()
{
    rct_window* window;

    window = window_create(
        context_get_width() - 40, context_get_height() - 64,
        40, 64,
        &window_title_exit_events,
        WC_TITLE_EXIT,
        WF_STICK_TO_BACK | WF_TRANSPARENT
    );
    window->widgets = window_title_exit_widgets;
    window->enabled_widgets |= (1ULL << WIDX_EXIT);
    window_init_scroll_widgets(window);

    return window;
}

/**
*
*  rct2: 0x0066B83C
*/
static void window_title_exit_mouseup(rct_window *w, rct_widgetindex widgetIndex)
{
    if (gIntroState != INTRO_STATE_NONE)
        return;

    switch (widgetIndex) {
    case WIDX_EXIT:
        context_quit();
        //game_do_command(0, 1, 0, 0, 5, 3, 0);
        break;
    };
}

/**
*
*  rct2: 0x0066B836
*/
static void window_title_exit_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
    window_draw_widgets(w, dpi);
}
