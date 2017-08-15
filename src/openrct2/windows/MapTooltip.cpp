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
    #include "../input.h"
    #include "../interface/widget.h"
    #include "../localisation/localisation.h"
}

static rct_widget window_map_tooltip_widgets[] = {
    { WWT_IMGBTN, 0, 0, 199, 0, 29, 0xFFFFFFFF, STR_NONE },
    { WIDGETS_END }
};

static void window_map_tooltip_update(rct_window *w);
static void window_map_tooltip_paint(rct_window *w, rct_drawpixelinfo *dpi);

static rct_window_event_list window_map_tooltip_events = {
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    window_map_tooltip_update,
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
    window_map_tooltip_paint,
    nullptr
};

#define MAP_TOOLTIP_ARGS

static sint32 _lastCursorX;
static sint32 _lastCursorY;
static sint32 _cursorHoldDuration;

static void window_map_tooltip_open();

/**
 *
 *  rct2: 0x006EE77A
 */
void window_map_tooltip_update_visibility()
{
    sint32 cursorX, cursorY;

    const CursorState * state = context_get_cursor_state();
    cursorX = state->x;
    cursorY = state->y;

    // Check for cursor movement
    _cursorHoldDuration++;
    if (abs(cursorX - _lastCursorX) > 5 || abs(cursorY - _lastCursorY) > 5 || (input_test_flag(INPUT_FLAG_5)))
        _cursorHoldDuration = 0;

    _lastCursorX = cursorX;
    _lastCursorY = cursorY;

    // Show or hide tooltip
    rct_string_id stringId;
    memcpy(&stringId, gMapTooltipFormatArgs, sizeof(rct_string_id));

    if (_cursorHoldDuration < 25 ||
        stringId == STR_NONE ||
        input_test_place_object_modifier((PLACE_OBJECT_MODIFIER) (PLACE_OBJECT_MODIFIER_COPY_Z | PLACE_OBJECT_MODIFIER_SHIFT_Z)) ||
        window_find_by_class(WC_ERROR) != nullptr
    ) {
        window_close_by_class(WC_MAP_TOOLTIP);
    } else {
        window_map_tooltip_open();
    }
}

/**
 *
 *  rct2: 0x006A7C43
 */
static void window_map_tooltip_open()
{
    rct_window* w;
    sint32 x, y, width, height;

    width = 200;
    height = 44;
    const CursorState * state = context_get_cursor_state();
    x = state->x - (width / 2);
    y = state->y + 15;

    w = window_find_by_class(WC_MAP_TOOLTIP);
    if (w == nullptr) {
        w = window_create(
            x, y, width, height, &window_map_tooltip_events, WC_MAP_TOOLTIP, WF_STICK_TO_FRONT | WF_TRANSPARENT | WF_NO_BACKGROUND
        );
        w->widgets = window_map_tooltip_widgets;
    } else {
        window_invalidate(w);
        w->x = x;
        w->y = y;
        w->width = width;
        w->height = height;
    }
}

/**
 *
 *  rct2: 0x006EE8CE
 */
static void window_map_tooltip_update(rct_window *w)
{
    window_invalidate(w);
}

/**
 *
 *  rct2: 0x006EE894
 */
static void window_map_tooltip_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
    rct_string_id stringId;
    memcpy(&stringId, gMapTooltipFormatArgs, sizeof(rct_string_id));
    if (stringId == STR_NONE) {
        return;
    }

    gfx_draw_string_centred_wrapped(dpi, gMapTooltipFormatArgs, w->x + (w->width / 2), w->y + (w->height / 2), w->width, STR_MAP_TOOLTIP_STRINGID, COLOUR_BLACK);
}
