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

#include <openrct2/game.h>
#include <openrct2/interface/widget.h>
#include <openrct2/localisation/localisation.h>
#include <openrct2-ui/windows/Window.h>

#define WW 200
#define WH 100

enum WINDOW_RIDE_DEMOLISH_WIDGET_IDX {
    WIDX_BACKGROUND,
    WIDX_TITLE,
    WIDX_CLOSE,
    WIDX_DEMOLISH,
    WIDX_CANCEL
};

// 0x009AEBA0
static rct_widget window_ride_demolish_widgets[] = {
    { WWT_FRAME,            0, 0,       WW - 1,     0,          WH - 1, STR_NONE,               STR_NONE },
    { WWT_CAPTION,          0, 1,       WW - 2,     1,          14,     STR_DEMOLISH_RIDE,      STR_WINDOW_TITLE_TIP },
    { WWT_CLOSEBOX,         0, WW - 13, WW - 3,     2,          13,     STR_CLOSE_X,            STR_CLOSE_WINDOW_TIP },
    { WWT_DROPDOWN_BUTTON,  0, 10,      94,         WH - 20,    WH - 9, STR_DEMOLISH,           STR_NONE },
    { WWT_DROPDOWN_BUTTON,  0, WW - 95, WW - 11,    WH - 20,    WH - 9, STR_SAVE_PROMPT_CANCEL, STR_NONE },
    { WIDGETS_END }
};

static void window_ride_demolish_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_ride_demolish_paint(rct_window *w, rct_drawpixelinfo *dpi);

//0x0098E2E4
static rct_window_event_list window_ride_demolish_events = {
    nullptr,
    window_ride_demolish_mouseup,
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
    window_ride_demolish_paint,
    nullptr
};

/** Based off of rct2: 0x006B486A */
rct_window * window_ride_demolish_prompt_open(sint32 rideIndex)
{
    rct_window *w;

    w = window_bring_to_front_by_number(WC_DEMOLISH_RIDE_PROMPT, rideIndex);
    if (w != nullptr)
        return w;

    w = window_create_centred(WW, WH, &window_ride_demolish_events, WC_DEMOLISH_RIDE_PROMPT, WF_TRANSPARENT);
    w->widgets = window_ride_demolish_widgets;
    w->enabled_widgets = (1 << WIDX_CLOSE) | (1 << WIDX_CANCEL) | (1 << WIDX_DEMOLISH);
    window_init_scroll_widgets(w);
    w->number = rideIndex;

    return w;
}


/**
*
*  rct2: 0x006B4933
*/
static void window_ride_demolish_mouseup(rct_window *w, rct_widgetindex widgetIndex)
{
    rct_window* window;
    switch (widgetIndex) {
    case WIDX_DEMOLISH:
        gGameCommandErrorTitle = STR_CANT_DEMOLISH_RIDE;
        ride_demolish(w->number, GAME_COMMAND_FLAG_APPLY);

        // Prevents demolished rides sticking around in the ride list window
        window = window_find_by_class(WC_RIDE_LIST);
        if (window != nullptr)
        {
            window_ride_list_refresh_list(window);
        }

        break;
    case WIDX_CANCEL:
    case WIDX_CLOSE:
        window_close(w);
        break;
    }
}

/**
*
*  rct2: 0x006B48E5
*/
static void window_ride_demolish_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
    window_draw_widgets(w, dpi);

    Ride* ride = get_ride(w->number);

    set_format_arg(0, rct_string_id, ride->name);
    set_format_arg(2, uint32, ride->name_arguments);

    sint32 x = w->x + WW / 2;
    sint32 y = w->y + (WH / 2) - 3;

    gfx_draw_string_centred_wrapped(dpi, gCommonFormatArgs, x, y, WW - 4, STR_DEMOLISH_RIDE_ID, COLOUR_BLACK);
}
