/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <openrct2/Context.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/Game.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/windows/Intent.h>
#include <openrct2/world/Park.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Window.h>

#define WW 200
#define WH 100

static money32 _demolishRideCost;

// clang-format off
enum WINDOW_RIDE_DEMOLISH_WIDGET_IDX {
    WIDX_BACKGROUND,
    WIDX_TITLE,
    WIDX_CLOSE,
    WIDX_DEMOLISH = 3,
    WIDX_REFURBISH = 3,
    WIDX_CANCEL
};

// 0x009AEBA0
static rct_widget window_ride_demolish_widgets[] = {
    { WWT_FRAME,            0, 0,       WW - 1,     0,          WH - 1, STR_NONE,               STR_NONE },
    { WWT_CAPTION,          0, 1,       WW - 2,     1,          14,     STR_DEMOLISH_RIDE,      STR_WINDOW_TITLE_TIP },
    { WWT_CLOSEBOX,         0, WW - 13, WW - 3,     2,          13,     STR_CLOSE_X_WHITE,      STR_CLOSE_WINDOW_TIP },
    { WWT_BUTTON,           0, 10,      94,         WH - 20,    WH - 9, STR_DEMOLISH,           STR_NONE },
    { WWT_BUTTON,           0, WW - 95, WW - 11,    WH - 20,    WH - 9, STR_SAVE_PROMPT_CANCEL, STR_NONE },
    { WIDGETS_END }
};

static rct_widget window_ride_refurbish_widgets[] = {
    { WWT_FRAME,            0, 0,       WW - 1,     0,          WH - 1, STR_NONE,               STR_NONE },
    { WWT_CAPTION,          0, 1,       WW - 2,     1,          14,     STR_REFURBISH_RIDE,     STR_WINDOW_TITLE_TIP },
    { WWT_CLOSEBOX,         0, WW - 13, WW - 3,     2,          13,     STR_CLOSE_X_WHITE,      STR_CLOSE_WINDOW_TIP },
    { WWT_BUTTON,           0, 10,      94,         WH - 20,    WH - 9, STR_REFURBISH,          STR_NONE },
    { WWT_BUTTON,           0, WW - 95, WW - 11,    WH - 20,    WH - 9, STR_SAVE_PROMPT_CANCEL, STR_NONE },
    { WIDGETS_END }
};

static void window_ride_demolish_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_ride_demolish_paint(rct_window *w, rct_drawpixelinfo *dpi);
static void window_ride_refurbish_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_ride_refurbish_paint(rct_window *w, rct_drawpixelinfo *dpi);

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
// clang-format on

static rct_window_event_list window_ride_refurbish_events = {
    nullptr,
    window_ride_refurbish_mouseup,
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
    window_ride_refurbish_paint,
    nullptr
};

/** Based off of rct2: 0x006B486A */
rct_window * window_ride_demolish_prompt_open(int32_t rideIndex)
{
    rct_window *w;

    w = window_find_by_class(WC_DEMOLISH_RIDE_PROMPT);
    if (w != nullptr)
    {
        int x = w->x;
        int y = w->y;
        window_close(w);
        w = window_create(x, y, WW, WH, &window_ride_demolish_events, WC_DEMOLISH_RIDE_PROMPT, WF_TRANSPARENT);
    }
    else
    {
        w = window_create_centred(WW, WH, &window_ride_demolish_events, WC_DEMOLISH_RIDE_PROMPT, WF_TRANSPARENT);
    }

    w->widgets = window_ride_demolish_widgets;
    w->enabled_widgets = (1 << WIDX_CLOSE) | (1 << WIDX_CANCEL) | (1 << WIDX_DEMOLISH);
    window_init_scroll_widgets(w);
    w->number = rideIndex;
    _demolishRideCost = -ride_get_refund_price(rideIndex);

    return w;
}

rct_window * window_ride_refurbish_prompt_open(int32_t rideIndex)
{
    rct_window *w;

    w = window_find_by_class(WC_DEMOLISH_RIDE_PROMPT);
    if (w != nullptr)
    {
        int x = w->x;
        int y = w->y;
        window_close(w);
        w = window_create(x, y, WW, WH, &window_ride_refurbish_events, WC_DEMOLISH_RIDE_PROMPT, WF_TRANSPARENT);
    }
    else
    {
        w = window_create_centred(WW, WH, &window_ride_refurbish_events, WC_DEMOLISH_RIDE_PROMPT, WF_TRANSPARENT);
    }

    w->widgets = window_ride_refurbish_widgets;
    w->enabled_widgets = (1 << WIDX_CLOSE) | (1 << WIDX_CANCEL) | (1 << WIDX_REFURBISH);
    window_init_scroll_widgets(w);
    w->number = rideIndex;
    _demolishRideCost = -ride_get_refund_price(rideIndex);

    return w;
}

/**
*
*  rct2: 0x006B4933
*/
static void window_ride_demolish_mouseup(rct_window *w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex) {
    case WIDX_DEMOLISH:
    {
        ride_action_modify(w->number, RIDE_MODIFY_DEMOLISH, GAME_COMMAND_FLAG_APPLY);
        break;
    }
    case WIDX_CANCEL:
    case WIDX_CLOSE:
        window_close(w);
        break;
    }
}

static void window_ride_refurbish_mouseup(rct_window *w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex) {
    case WIDX_REFURBISH:
    {
        ride_action_modify(w->number, RIDE_MODIFY_RENEW, GAME_COMMAND_FLAG_APPLY);
        break;
    }
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
    set_format_arg(2, uint32_t, ride->name_arguments);
    set_format_arg(6, money32, _demolishRideCost);

    int32_t x = w->x + WW / 2;
    int32_t y = w->y + (WH / 2) - 3;

    if (gParkFlags & PARK_FLAGS_NO_MONEY)
    {
        gfx_draw_string_centred_wrapped(dpi, gCommonFormatArgs, x, y, WW - 4, STR_DEMOLISH_RIDE_ID, COLOUR_BLACK);
    }
    else
    {
        gfx_draw_string_centred_wrapped(dpi, gCommonFormatArgs, x, y, WW - 4, STR_DEMOLISH_RIDE_ID_MONEY, COLOUR_BLACK);
    }
}

static void window_ride_refurbish_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
    window_draw_widgets(w, dpi);

    Ride* ride = get_ride(w->number);

    set_format_arg(0, rct_string_id, ride->name);
    set_format_arg(2, uint32_t, ride->name_arguments);
    set_format_arg(6, money32, _demolishRideCost / 2);

    int32_t x = w->x + WW / 2;
    int32_t y = w->y + (WH / 2) - 3;

    if (gParkFlags & PARK_FLAGS_NO_MONEY)
    {
        gfx_draw_string_centred_wrapped(dpi, gCommonFormatArgs, x, y, WW - 4, STR_REFURBISH_RIDE_ID_NO_MONEY, COLOUR_BLACK);
    }
    else
    {
        gfx_draw_string_centred_wrapped(dpi, gCommonFormatArgs, x, y, WW - 4, STR_REFURBISH_RIDE_ID_MONEY, COLOUR_BLACK);
    }
}
