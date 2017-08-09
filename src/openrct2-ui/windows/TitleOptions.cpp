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
    #include <openrct2/intro.h>
    #include <openrct2/localisation/localisation.h>
    #include <openrct2/interface/widget.h>
}

enum WINDOW_TITLE_OPTIONS_WIDGET_IDX {
    WIDX_OPTIONS,
};

static rct_widget window_title_options_widgets[] = {
    { WWT_DROPDOWN_BUTTON, 2, 0, 79, 0, 11, STR_OPTIONS, STR_OPTIONS_TIP },
    { WIDGETS_END },
};

static void window_title_options_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_title_options_paint(rct_window *w, rct_drawpixelinfo *dpi);

static rct_window_event_list window_title_options_events = {
    NULL,
    window_title_options_mouseup,
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
    window_title_options_paint,
    NULL
};

/**
 * Creates the window containing the options button on the title screen.
 */
rct_window * window_title_options_open()
{
    rct_window * window = window_create(
        context_get_width() - 80, 0,
        80, 12,
        &window_title_options_events,
        WC_TITLE_OPTIONS,
        WF_STICK_TO_BACK | WF_TRANSPARENT
    );
    window->widgets = window_title_options_widgets;
    window->enabled_widgets |= (1ULL << WIDX_OPTIONS);
    window_init_scroll_widgets(window);

    return window;
}

static void window_title_options_mouseup(rct_window *w, rct_widgetindex widgetIndex)
{
    if (gIntroState != INTRO_STATE_NONE)
        return;

    switch (widgetIndex) {
    case WIDX_OPTIONS:
        context_open_window(WC_OPTIONS);
        break;
    }
}

static void window_title_options_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
    window_draw_widgets(w, dpi);
}

