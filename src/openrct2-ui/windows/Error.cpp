/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <openrct2/Context.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/core/Math.hpp>
#include <openrct2-ui/windows/Window.h>

#include <openrct2/audio/audio.h>
#include <openrct2/drawing/Font.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/drawing/Drawing.h>

// clang-format off
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
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    window_error_unknown5,
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
    window_error_paint,
    nullptr
};
// clang-format on

static char _window_error_text[512];
static uint16_t _window_error_num_lines;

/**
 *
 *  rct2: 0x0066792F
 *
 * bx: title
 * dx: message
 */
rct_window * window_error_open(rct_string_id title, rct_string_id message)
{
    utf8 *dst;
    int32_t numLines, fontHeight, x, y, width, height, maxY;
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
        return nullptr;
    }

    // Check if there is any text to display
    if (dst == _window_error_text + 1)
        return nullptr;

    gCurrentFontSpriteBase = FONT_SPRITE_BASE_MEDIUM;
    width = gfx_get_string_width_new_lined(_window_error_text);
    width = std::min(196, width);

    gCurrentFontSpriteBase = FONT_SPRITE_BASE_MEDIUM;
    gfx_wrap_string(_window_error_text, width + 1, &numLines, &fontHeight);

    _window_error_num_lines = numLines;
    width = width + 3;
    height = (numLines + 1) * font_get_line_height(gCurrentFontSpriteBase) + 4;

    window_error_widgets[WIDX_BACKGROUND].right = width;
    window_error_widgets[WIDX_BACKGROUND].bottom = height;

    int32_t screenWidth = context_get_width();
    int32_t screenHeight = context_get_height();
    const CursorState * state = context_get_cursor_state();
    x = state->x - (width / 2);
    x = Math::Clamp(0, x, screenWidth);

    y = state->y + 26;
    y = std::max(22, y);
    maxY = screenHeight - height;
    if (y > maxY) {
        y = y - height - 40;
        y = std::min(y, maxY);
    }

    w = window_create(x, y, width, height, &window_error_events, WC_ERROR, WF_STICK_TO_FRONT | WF_TRANSPARENT | WF_RESIZABLE);
    w->widgets = window_error_widgets;
    w->error.var_480 = 0;
    if (!gDisableErrorWindowSound) {
        audio_play_sound(SOUND_ERROR, 0, w->x + (w->width / 2));
    }

    return w;
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
    int32_t t, l, r, b;

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
