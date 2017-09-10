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

#define WW 200
#define WH 100

enum WINDOW_STAFF_FIRE_WIDGET_IDX {
    WIDX_BACKGROUND,
    WIDX_TITLE,
    WIDX_CLOSE,
    WIDX_YES,
    WIDX_CANCEL
};

// 0x9AFB4C
static rct_widget window_staff_fire_widgets[] = {
    { WWT_FRAME,            0,  0,          WW - 1,     0,          WH - 1,     STR_NONE,               STR_NONE },
    { WWT_CAPTION,          0,  1,          WW - 2,     1,          14,         STR_SACK_STAFF,         STR_WINDOW_TITLE_TIP },
    { WWT_CLOSEBOX,         0,  WW-13,      WW - 3,     2,          13,         STR_CLOSE_X,            STR_CLOSE_WINDOW_TIP },
    { WWT_DROPDOWN_BUTTON,  0,  10,         94,         WH - 20,    WH - 9,     STR_YES,                STR_NONE },
    { WWT_DROPDOWN_BUTTON,  0,  WW - 95,    WW - 11,    WH - 20,    WH - 9,     STR_SAVE_PROMPT_CANCEL, STR_NONE },
    { WIDGETS_END }
};

static void window_staff_fire_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_staff_fire_paint(rct_window *w, rct_drawpixelinfo *dpi);

//0x9A3F7C
static rct_window_event_list window_staff_fire_events = {
    nullptr,
    window_staff_fire_mouseup,
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
    window_staff_fire_paint,
    nullptr
};
/** Based off of rct2: 0x6C0A77 */
rct_window* window_staff_fire_prompt_open(rct_peep* peep)
{
    rct_window * w;

    // Check if the confirm window already exists.
    w = window_bring_to_front_by_number(WC_FIRE_PROMPT, peep->sprite_index);
    if (w != nullptr) {
        return w;
    }

    w = window_create_centred(WW, WH, &window_staff_fire_events, WC_FIRE_PROMPT, WF_TRANSPARENT);
    w->widgets = window_staff_fire_widgets;
    w->enabled_widgets |= (1 << WIDX_CLOSE) | (1 << WIDX_YES) | (1 << WIDX_CANCEL);

    window_init_scroll_widgets(w);

    w->number = peep->sprite_index;

    return w;
}


/**
*
*  rct2: 0x006C0B40
*/
static void window_staff_fire_mouseup(rct_window *w, rct_widgetindex widgetIndex)
{
    rct_peep* peep = &get_sprite(w->number)->peep;

    switch (widgetIndex){
    case WIDX_YES:
        game_do_command(peep->x, 1, peep->y, w->number, GAME_COMMAND_FIRE_STAFF_MEMBER, 0, 0);
        break;
    case WIDX_CANCEL:
    case WIDX_CLOSE:
        window_close(w);
    }
}

/**
*
*  rct2: 0x006C0AF2
*/
static void window_staff_fire_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
    window_draw_widgets(w, dpi);

    rct_peep* peep = &get_sprite(w->number)->peep;

    set_format_arg(0, rct_string_id, peep->name_string_idx);
    set_format_arg(2, uint32, peep->id);

    sint32 x = w->x + WW / 2;
    sint32 y = w->y + (WH / 2) - 3;

    gfx_draw_string_centred_wrapped(dpi, gCommonFormatArgs, x, y, WW - 4, STR_FIRE_STAFF_ID, COLOUR_BLACK);
}
