/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <openrct2-ui/input/Input.h>
#include <openrct2-ui/windows/Window.h>
#include <optional>

void input_widget_over(rct_window* w, const input_mouse_data& mouseData, const rct_widgetindex widgetIndex);
void input_widget_left(rct_window* w, const input_mouse_data& mouseData, const rct_widgetindex widgetIndex);
void input_widget_right(
    rct_window* w, const input_mouse_data& mouseData, const rct_widget* widget, const rct_widgetindex widgetIndex);
void input_widget_pressed(rct_widgetindex widgetIndex, rct_window* w, rct_widget* widget, const input_mouse_data& mouseData);

std::optional<int32_t> input_get_widget_cursor_id(
    rct_window* window, rct_widgetindex widgetId, const input_mouse_data& mouseData);

void input_scroll_drag_continue(rct_window* w, const input_mouse_data& mouseData);
void input_scroll_right(const input_mouse_data& mouseData);

void input_viewport_drag_end();
void input_viewport_drag_continue();
void input_viewport_right(const input_mouse_data& mouseData);
void input_viewport_left(const input_mouse_data& mouseData);