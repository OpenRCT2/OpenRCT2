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

#ifndef _WIDGET_H_
#define _WIDGET_H_

#include "window.h"

typedef enum {
    WWT_EMPTY = 0,
    WWT_FRAME = 1,
    WWT_RESIZE = 2,
    WWT_IMGBTN = 3,
    WWT_4 = 4, // Same as IMGBTN but uses .image + 1 while pressed/active
    WWT_5 = 5,
    WWT_COLOURBTN = 6,
    WWT_TRNBTN = 7,
    WWT_TAB = 8,
    WWT_FLATBTN = 9,
    WWT_DROPDOWN_BUTTON = 10,
    WWT_11, // Same as dropdown button but uses .text + 1 while pressed/active
    WWT_12, // looks like a normal label to me
    WWT_13,
    WWT_14,
    WWT_SPINNER = 15,
    WWT_DROPDOWN = 16,
    WWT_VIEWPORT = 17,
    WWT_18,
    WWT_GROUPBOX = 19,
    WWT_CAPTION = 20,
    WWT_CLOSEBOX = 21,
    WWT_SCROLL = 22,
    WWT_CHECKBOX = 23,
    WWT_24,
    WWT_25,
    WWT_TEXT_BOX = 27,
    WWT_LAST = 26,
} WINDOW_WIDGET_TYPES;

#define WIDGETS_END     WWT_LAST, 0, 0, 0, 0, 0, 0, 0
#define BAR_BLINK       (1u << 31)

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
void widget_scroll_get_part(rct_window *w, rct_widget* widget, sint32 x, sint32 y, sint32 *output_x, sint32 *output_y, sint32 *output_scroll_area, sint32 *scroll_id);

void widget_set_enabled(rct_window *w, rct_widgetindex widgetIndex, bool enabled);
void widget_set_checkbox_value(rct_window *w, rct_widgetindex widgetIndex, sint32 value);

#endif
