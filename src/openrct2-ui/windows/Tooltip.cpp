/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <openrct2/Context.h>
#include <openrct2/core/Math.hpp>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/Input.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/drawing/Drawing.h>

// clang-format off
enum {
    WIDX_BACKGROUND
};

static rct_widget window_tooltip_widgets[] = {
    { WWT_IMGBTN, 0, 0, 199, 0, 31, 0xFFFFFFFF, STR_NONE },
    { WIDGETS_END },
};

static void window_tooltip_update(rct_window *w);
static void window_tooltip_paint(rct_window *w, rct_drawpixelinfo *dpi);

static rct_window_event_list window_tooltip_events = {
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    window_tooltip_update,
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
    window_tooltip_paint,
    nullptr
};
// clang-format on

static utf8 _tooltipText[sizeof(gCommonStringFormatBuffer)];
static int16_t _tooltipNumLines;

void window_tooltip_reset(int32_t x, int32_t y)
{
    gTooltipCursorX = x;
    gTooltipCursorY = y;
    gTooltipTimeout = 0;
    gTooltipWidget.window_classification = 255;
    input_set_state(INPUT_STATE_NORMAL);
    input_set_flag(INPUT_FLAG_4, false);
}

void window_tooltip_show(rct_string_id id, int32_t x, int32_t y)
{
    rct_window *w;
    int32_t width, height;

    w = window_find_by_class(WC_ERROR);
    if (w != nullptr)
        return;

    char* buffer = gCommonStringFormatBuffer;

    format_string(buffer, 256, id, gCommonFormatArgs);
    gCurrentFontSpriteBase = FONT_SPRITE_BASE_MEDIUM;

    int32_t tooltip_text_width;
    tooltip_text_width = gfx_get_string_width_new_lined(buffer);
    buffer = gCommonStringFormatBuffer;
    tooltip_text_width = std::min(tooltip_text_width, 196);

    gCurrentFontSpriteBase = FONT_SPRITE_BASE_MEDIUM;

    int32_t numLines, fontSpriteBase;
    tooltip_text_width = gfx_wrap_string(buffer, tooltip_text_width + 1, &numLines, &fontSpriteBase);

    _tooltipNumLines = numLines;
    width = tooltip_text_width + 3;
    height = ((numLines + 1) * font_get_line_height(gCurrentFontSpriteBase)) + 4;
    window_tooltip_widgets[WIDX_BACKGROUND].right = width;
    window_tooltip_widgets[WIDX_BACKGROUND].bottom = height;

    memcpy(_tooltipText, buffer, sizeof(_tooltipText));

    int32_t screenWidth = context_get_width();
    int32_t screenHeight = context_get_height();
    x = Math::Clamp(0, x - (width / 2), screenWidth - width);

    // TODO The cursor size will be relative to the window DPI.
    //      The amount to offset the y should be adjusted.

    int32_t max_y = screenHeight - height;
    y += 26; // Normally, we'd display the tooltip 26 lower
    if (y > max_y)
        // If y is too large, the tooltip could be forced below the cursor if we'd just clamped y,
        // so we'll subtract a bit more
        y -= height + 40;
    y = Math::Clamp(22, y, max_y);

    w = window_create(
        x,
        y,
        width,
        height,
        &window_tooltip_events,
        WC_TOOLTIP,
        WF_TRANSPARENT | WF_STICK_TO_FRONT
        );
    w->widgets = window_tooltip_widgets;

    reset_tooltip_not_shown();
}

/**
 *
 *  rct2: 0x006EA10D
 */
void window_tooltip_open(rct_window *widgetWindow, rct_widgetindex widgetIndex, int32_t x, int32_t y)
{
    rct_widget *widget;

    if (widgetWindow == nullptr || widgetIndex == -1)
        return;

    widget = &widgetWindow->widgets[widgetIndex];
    window_event_invalidate_call(widgetWindow);
    if (widget->tooltip == 0xFFFF)
        return;

    gTooltipWidget.window_classification = widgetWindow->classification;
    gTooltipWidget.window_number = widgetWindow->number;
    gTooltipWidget.widget_index = widgetIndex;

    if (window_event_tooltip_call(widgetWindow, widgetIndex) == STR_NONE)
        return;

    window_tooltip_show(widget->tooltip, x, y);
}

/**
 *
 *  rct2: 0x006E98C6
 */
void window_tooltip_close()
{
    window_close_by_class(WC_TOOLTIP);
    gTooltipTimeout = 0;
    gTooltipWidget.window_classification = 255;
}

/**
 *
 *  rct2: 0x006EA580
 */
static void window_tooltip_update(rct_window *w)
{
    reset_tooltip_not_shown();
}

/**
 *
 *  rct2: 0x006EA41D
 */
static void window_tooltip_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
    int32_t left = w->x;
    int32_t top = w->y;
    int32_t right = w->x + w->width - 1;
    int32_t bottom = w->y + w->height - 1;

    // Background
    gfx_filter_rect(dpi, left + 1, top + 1, right - 1, bottom - 1, PALETTE_45);
    gfx_filter_rect(dpi, left + 1, top + 1, right - 1, bottom - 1, PALETTE_GLASS_LIGHT_ORANGE);

    // Sides
    gfx_filter_rect(dpi, left  + 0, top    + 2, left  + 0, bottom - 2, PALETTE_DARKEN_3);
    gfx_filter_rect(dpi, right + 0, top    + 2, right + 0, bottom - 2, PALETTE_DARKEN_3);
    gfx_filter_rect(dpi, left  + 2, bottom + 0, right - 2, bottom + 0, PALETTE_DARKEN_3);
    gfx_filter_rect(dpi, left  + 2, top    + 0, right - 2, top    + 0, PALETTE_DARKEN_3);

    // Corners
    gfx_filter_pixel(dpi, left  + 1, top    + 1, PALETTE_DARKEN_3);
    gfx_filter_pixel(dpi, right - 1, top    + 1, PALETTE_DARKEN_3);
    gfx_filter_pixel(dpi, left  + 1, bottom - 1, PALETTE_DARKEN_3);
    gfx_filter_pixel(dpi, right - 1, bottom - 1, PALETTE_DARKEN_3);

    // Text
    left = w->x + ((w->width + 1) / 2) - 1;
    top = w->y + 1;
    draw_string_centred_raw(dpi, left, top, _tooltipNumLines, _tooltipText);
}
