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
#include <openrct2/OpenRCT2.h>
#include <openrct2/core/Util.hpp>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/windows/Intent.h>
#include <openrct2/Context.h>

#include <openrct2/Game.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2-ui/interface/Widget.h>

enum WINDOW_SPEEDRUN_OPTION_WIDGET_IDX {
    WIDX_BACKGROUND,
    WIDX_TITLE,
    WIDX_CLOSE,
    WIDX_LABEL,
    WIDX_RUN_GROUP,
    WIDX_RUN_SCENARIO
};

static rct_widget window_speedrun_option_widgets[] = {
    { WWT_FRAME,            0,  0,      176,    0,  33, STR_NONE,                           STR_NONE },                 // panel / background
    { WWT_CAPTION,          0,  1,      175,    1,  14, STR_SPEEDRUNNING_OPTION_PROMPT,     STR_NONE },                 // title bar
    { WWT_CLOSEBOX,         0,  164,    174,    2,  13, STR_CLOSE_X_WHITE,                  STR_CLOSE_WINDOW_TIP },     // close x button
    { WWT_LABEL_CENTRED,    0,  2,      257,    19, 30, STR_SPEEDRUNNING_GROUP_OR_SCENARIO, STR_NONE },                 // question/label
    { WWT_BUTTON,           0,  8,      85,     19, 30, STR_SPEEDRUNNING_RUN_GROUP,         STR_NONE },                 // run group
    { WWT_BUTTON,           0,  91,     168,    19, 30, STR_SPEEDRUNNING_RUN_SCENARIO,      STR_NONE },                 // run scenario
    { WIDGETS_END },
};

static void window_speedrun_option_close(rct_window *w);
static void window_speedrun_option_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_speedrun_option_paint(rct_window *w, rct_drawpixelinfo *dpi);
static void window_speedrun_option_callback(sint32 result, const utf8 * path);

static rct_window_event_list window_speedrun_option_events = {
    window_speedrun_option_close,
    window_speedrun_option_mouseup,
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
    window_speedrun_option_paint,
    nullptr
};

/**
*
*  rct2: 0x0066DCBE
*/
rct_window * window_speedrun_option_open()
{
    sint32 width, height;
    rct_string_id stringId;
    rct_window* window;
    uint8 prompt_mode;
    rct_widget *widgets;
    uint64 enabled_widgets;

    // Check if window is already open
    window = window_bring_to_front_by_class(WC_SPEEDRUN_OPTION);
    if (window) {
        window_close(window);
    }

    window = window_create_centred(
        width,
        height,
        &window_speedrun_option_events,
        WC_SAVE_PROMPT,
        WF_TRANSPARENT | WF_STICK_TO_FRONT
    );

    window->widgets = widgets;
    window->enabled_widgets = enabled_widgets;
    window_init_scroll_widgets(window);

    return window;
}

/**
*
*  rct2: 0x0066DF17
*/
static void window_speedrun_option_close(rct_window *w)
{
    window_close(w);
}

/**
*
*  rct2: 0x0066DDF2
*/
static void window_speedrun_option_mouseup(rct_window *w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex) {
        case WIDX_RUN_GROUP:
            // TODO
            break;
        case WIDX_RUN_SCENARIO:
            // TODO
            break;
        case WIDX_CLOSE:
            window_close(w);
            break;
    }
}

static void window_speedrun_option_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
    window_draw_widgets(w, dpi);
}

static void window_speedrun_option_callback(sint32 result, const utf8 * path)
{
    if (result == MODAL_RESULT_OK) {
        game_load_or_quit_no_save_prompt();
    }
}
