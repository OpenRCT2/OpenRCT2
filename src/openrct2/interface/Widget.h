/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifndef _WIDGET_H_
#define _WIDGET_H_

#include "Window.h"

enum WINDOW_WIDGET_TYPES {
    WWT_EMPTY = 0,
    WWT_FRAME = 1,
    WWT_RESIZE = 2,
    WWT_IMGBTN = 3,
    WWT_COLOURBTN = 6,
    WWT_TRNBTN = 7,
    WWT_TAB = 8,
    WWT_FLATBTN = 9,
    WWT_BUTTON = 10,
    WWT_LABEL_CENTRED = 12,    // Centred text
    WWT_TABLE_HEADER = 13,     // Left-aligned textual button
    WWT_LABEL = 14,            // Left-aligned text
    WWT_SPINNER = 15,
    WWT_DROPDOWN = 16,
    WWT_VIEWPORT = 17,
    WWT_GROUPBOX = 19,
    WWT_CAPTION = 20,
    WWT_CLOSEBOX = 21,
    WWT_SCROLL = 22,
    WWT_CHECKBOX = 23,
    WWT_PLACEHOLDER = 25,
    WWT_TEXT_BOX = 27,
    WWT_LAST = 26,
};

#define WIDGETS_END     WWT_LAST, 0, 0, 0, 0, 0, 0, 0
#define BAR_BLINK       (1u << 31)

#define SPINNER_INCREASE(l, r, t, b)   (r) - 12, (r) - 1,  (t) + 1, (b) - 1
#define SPINNER_DECREASE(l, r, t, b)   (r) - 25, (r) - 13, (t) + 1, (b) - 1
#define SPINNER_WIDGETS(colour, left, right, top, bottom, text, tooltip) \
    { WWT_SPINNER,      colour, left, right, top, bottom,                   text,             tooltip  }, \
    { WWT_BUTTON,       colour, SPINNER_INCREASE(left, right, top, bottom), STR_NUMERIC_UP,   STR_NONE }, \
    { WWT_BUTTON,       colour, SPINNER_DECREASE(left, right, top, bottom), STR_NUMERIC_DOWN, STR_NONE }  \

enum {
    SCROLL_HORIZONTAL = (1 << 0),
    SCROLL_VERTICAL = (1 << 1),
    SCROLL_BOTH = SCROLL_HORIZONTAL | SCROLL_VERTICAL
};

void widget_scroll_update_thumbs(rct_window *w, rct_widgetindex widget_index);
void widget_draw(rct_drawpixelinfo *dpi, rct_window *w, rct_widgetindex widgetIndex);

bool widget_is_enabled(rct_window *w, rct_widgetindex widgetIndex);
bool widget_is_disabled(rct_window *w, rct_widgetindex widgetIndex);
bool widget_is_pressed(rct_window *w, rct_widgetindex widgetIndex);
bool widget_is_highlighted(rct_window *w, rct_widgetindex widgetIndex);
bool widget_is_active_tool(rct_window *w, rct_widgetindex widgetIndex);
void widget_scroll_get_part(rct_window *w, rct_widget* widget, int32_t x, int32_t y, int32_t *output_x, int32_t *output_y, int32_t *output_scroll_area, int32_t *scroll_id);

void widget_set_enabled(rct_window *w, rct_widgetindex widgetIndex, bool enabled);
void widget_set_checkbox_value(rct_window *w, rct_widgetindex widgetIndex, int32_t value);

#endif
