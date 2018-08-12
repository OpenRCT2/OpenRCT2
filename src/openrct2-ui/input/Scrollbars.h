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

void input_scroll_begin(rct_window* w, rct_widgetindex widgetIndex, const input_mouse_data& mouseData);
void input_scroll_left(rct_window *w, const rct_widgetindex widgetIndex, const input_mouse_data &mouseData);
