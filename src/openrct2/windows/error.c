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

#include "../audio/audio.h"
#include "../Context.h"
#include "../interface/widget.h"
#include "../interface/window.h"
#include "../localisation/localisation.h"
#include "../platform/platform.h"
#include "../rct2.h"
#include "error.h"

bool gDisableErrorWindowSound = false;

enum {
    WIDX_BACKGROUND
};

static rct_widget window_error_widgets[] = {
    { WWT_IMGBTN, 0, 0, 199, 0, 41, 0xFFFFFFFF, STR_NONE },
    { WIDGETS_END }
};

static void window_error_unknown5(rct_window *w);
static void window_error_paint(rct_window *w, rct_drawpixelinfo *dpi);

static rct_window_event_list window_error_events = {
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    window_error_unknown5,
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
    window_error_paint,
    NULL
};

static char _window_error_text[512];
static uint16 _window_error_num_lines;

/**
 *
 *  rct2: 0x0066792F
 *
 * bx: title
 * dx: message
 */
void window_error_open(rct_string_id title, rct_string_id message)
{
    utf8 *dst;
    sint32 numLines, fontHeight, x, y, width, height, maxY;
    rct_window *w;

    window_close_by_class(WC_ERROR);
    dst = _window_error_text;

    // Format the title
    dst = utf8_write_codepoint(dst, FORMAT_BLACK);
    if (title != STR_NONE) {
        format_string(dst, 512 - (dst - _window_error_text), title, gCommonFormatArgs);
        dst = get_string_end(dst);
    }

    // Format the message
    if (message != STR_NONE) {
        dst = utf8_write_codepoint(dst, FORMAT_NEWLINE);
        format_string(dst, 512 - (dst - _window_error_text), message, gCommonFormatArgs);
        dst = get_string_end(dst);
    }

    log_verbose("show error, %s", _window_error_text + 1);

    // Don't do unnecessary work in headless. Also saves checking if cursor state is null.
    if (gOpenRCT2Headless) {
        return;
    }

    // Check if there is any text to display
    if (dst == _window_error_text + 1)
        return;

    gCurrentFontSpriteBase = FONT_SPRITE_BASE_MEDIUM;
    width = gfx_get_string_width_new_lined(_window_error_text);
    width = min(196, width);

    gCurrentFontSpriteBase = FONT_SPRITE_BASE_MEDIUM;
    gfx_wrap_string(_window_error_text, width + 1, &numLines, &fontHeight);

    _window_error_num_lines = numLines;
    width = width + 3;
    height = (numLines + 1) * 10 + 4;

    window_error_widgets[WIDX_BACKGROUND].right = width;
    window_error_widgets[WIDX_BACKGROUND].bottom = height;

    sint32 screenWidth = context_get_width();
    sint32 screenHeight = context_get_height();
    const CursorState * state = context_get_cursor_state();
    x = state->x - (width / 2);
    x = clamp(0, x, screenWidth);

    y = state->y + 26;
    y = max(22, y);
    maxY = screenHeight - height;
    if (y > maxY) {
        y = y - height - 40;
        y = min(y, maxY);
    }

    w = window_create(x, y, width, height, &window_error_events, WC_ERROR, WF_STICK_TO_FRONT | WF_TRANSPARENT | WF_RESIZABLE);
    w->widgets = window_error_widgets;
    w->error.var_480 = 0;
    if (!gDisableErrorWindowSound) {
        audio_play_sound(SOUND_ERROR, 0, w->x + (w->width / 2));
    }
}

/**
 *
 *  rct2: 0x00667BFE
 */
static void window_error_unknown5(rct_window *w)
{
    w->error.var_480++;
    if (w->error.var_480 >= 8)
        window_close(w);
}

/**
 *
 *  rct2: 0x00667AA3
 */
static void window_error_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
    sint32 t, l, r, b;

    l = w->x;
    t = w->y;
    r = w->x + w->width - 1;
    b = w->y + w->height - 1;

    gfx_filter_rect(dpi, l + 1, t + 1, r - 1, b - 1, PALETTE_45);
    gfx_filter_rect(dpi, l, t, r, b, PALETTE_GLASS_SATURATED_RED);

    gfx_filter_rect(dpi, l, t + 2, l, b - 2, PALETTE_DARKEN_3);
    gfx_filter_rect(dpi, r, t + 2, r, b - 2, PALETTE_DARKEN_3);
    gfx_filter_rect(dpi, l + 2, b, r - 2, b, PALETTE_DARKEN_3);
    gfx_filter_rect(dpi, l + 2, t, r - 2, t, PALETTE_DARKEN_3);

    gfx_filter_rect(dpi, r + 1, t + 1, r + 1, t + 1, PALETTE_DARKEN_3);
    gfx_filter_rect(dpi, r - 1, t + 1, r - 1, t + 1, PALETTE_DARKEN_3);
    gfx_filter_rect(dpi, l + 1, b - 1, l + 1, b - 1, PALETTE_DARKEN_3);
    gfx_filter_rect(dpi, r - 1, b - 1, r - 1, b - 1, PALETTE_DARKEN_3);

    l = w->x + (w->width + 1) / 2 - 1;
    t = w->y + 1;
    draw_string_centred_raw(dpi, l, t, _window_error_num_lines, _window_error_text);
}
