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

#include "../localisation/localisation.h"
#include "../interface/widget.h"
#include "../interface/window.h"
#include "../title/TitleScreen.h"
#include "../Version.h"
#include "../OpenRCT2.h"
#include "../Context.h"

// Width is determined during create and does not need assigned
#define WH 26

enum WINDOW_TITLE_VERSION_WIDGET_IDX
{
    WIDX_VERSION_CHANGELOG,
};

static rct_widget window_title_version_widgets[] = {
    { WWT_FLATBTN, 0, 0, 0, 0, WH, STR_NONE, STR_NONE },
    { WIDGETS_END },
};

static void window_title_version_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_title_version_resize(rct_window *w);
static void window_title_version_paint(rct_window *w, rct_drawpixelinfo *dpi);

static rct_window_event_list window_title_version_events = {
    NULL,
    window_title_version_mouseup,
    window_title_version_resize,
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
    window_title_version_paint,
    NULL
};

void window_title_version_open()
{
    // Resize window to fit text
    sint32 win_width, text_width;
    utf8 buffer[256];
    utf8 *ch = buffer;

    // Write format codes
    ch = utf8_write_codepoint(ch, FORMAT_MEDIUMFONT);
    ch = utf8_write_codepoint(ch, FORMAT_OUTLINE);
    ch = utf8_write_codepoint(ch, FORMAT_WHITE);

    // Set width to name and version information
    openrct2_write_full_version_info(ch, sizeof(buffer) - (ch - buffer));
    win_width = gfx_get_string_width(buffer) + 5;
    
    // Resize to fit platform information
    snprintf(ch, sizeof(buffer) - (ch - buffer), "%s (%s)", OPENRCT2_PLATFORM, OPENRCT2_ARCHITECTURE);
    text_width = gfx_get_string_width(buffer) + 5;
    win_width = max(win_width, text_width);

    // Create window
    rct_window *window = window_create(
        5,
        context_get_height() - WH - 5,
        win_width, WH, 
        &window_title_version_events, 
        WC_TITLE_VERSION, 
        WF_STICK_TO_BACK | WF_TRANSPARENT
    );
    
    // Resize button
    window_title_version_widgets[WIDX_VERSION_CHANGELOG].right = win_width;

    window->widgets = window_title_version_widgets;
    window->enabled_widgets = (1ULL << WIDX_VERSION_CHANGELOG);

    window_init_scroll_widgets(window);
    window->colours[0] = TRANSLUCENT(COLOUR_GREY);
    window->colours[1] = TRANSLUCENT(COLOUR_GREY);
    window->colours[2] = TRANSLUCENT(COLOUR_GREY);
}

static void window_title_version_mouseup(rct_window *w, rct_widgetindex widgetIndex)
{
    if (!(gScreenFlags & SCREEN_FLAGS_TITLE_DEMO) || title_should_hide_version_info()) {
        return;
    }

    switch (widgetIndex) {
    case WIDX_VERSION_CHANGELOG:
        window_changelog_open();
        break;
    }
}

static void window_title_version_resize(rct_window *w)
{
    w->x = 5;
    w->y = context_get_height() - WH - 5;

    window_invalidate(w);
}

static void window_title_version_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
    if (!(gScreenFlags & SCREEN_FLAGS_TITLE_DEMO) || title_should_hide_version_info()) {
        return;
    }

    utf8 buffer[256];

    // Write format codes
    utf8 *ch = buffer;
    ch = utf8_write_codepoint(ch, FORMAT_MEDIUMFONT);
    ch = utf8_write_codepoint(ch, FORMAT_OUTLINE);
    ch = utf8_write_codepoint(ch, FORMAT_WHITE);

    // Write name and version information
    openrct2_write_full_version_info(ch, sizeof(buffer) - (ch - buffer));
    gfx_draw_string(dpi, buffer, COLOUR_BLACK, w->x, w->y);

    // Write platform information
    snprintf(ch, sizeof(buffer) - (ch - buffer), "%s (%s)", OPENRCT2_PLATFORM, OPENRCT2_ARCHITECTURE);
    gfx_draw_string(dpi, buffer, COLOUR_BLACK, w->x, w->y + 13);
}
