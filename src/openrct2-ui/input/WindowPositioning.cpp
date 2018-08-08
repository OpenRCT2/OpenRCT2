/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "WindowPositioning.h"

#include "Input.h"

#include <openrct2/Context.h>
#include <openrct2/Input.h>
#include <openrct2/config/Config.h>

static int32_t OriginalWindowWidth;
static int32_t OriginalWindowHeight;

void input_window_position_begin(rct_window* w, rct_widgetindex widgetIndex, int32_t x, int32_t y)
{
    gInputState = INPUT_STATE_POSITIONING_WINDOW;
    gInputDragLastX = x - w->x;
    gInputDragLastY = y - w->y;
    gDragWidget.window_classification = w->classification;
    gDragWidget.window_number = w->number;
    gDragWidget.widget_index = widgetIndex;
}

void input_window_position_end(rct_window* w, const input_mouse_data& mouseData)
{
    gInputState = INPUT_STATE_NORMAL;
    gTooltipTimeout = 0;
    gTooltipWidget = gDragWidget;
    window_event_moved_call(w, mouseData.X, mouseData.Y);
}

void input_window_resize_begin(rct_window* w, rct_widgetindex widgetIndex, const input_mouse_data& mouseData)
{
    gInputState = INPUT_STATE_RESIZING;
    gInputDragLastX = mouseData.X;
    gInputDragLastY = mouseData.Y;

    gDragWidget.window_classification = w->classification;
    gDragWidget.window_number = w->number;
    gDragWidget.widget_index = widgetIndex;

    OriginalWindowWidth = w->width;
    OriginalWindowHeight = w->height;
}

void input_window_resize_continue(rct_window* w, const input_mouse_data& mouseData)
{
    if (mouseData.Y < (int32_t)context_get_height() - 2)
    {
        int32_t targetWidth = OriginalWindowWidth + mouseData.X - gInputDragLastX;
        int32_t targetHeight = OriginalWindowHeight + mouseData.Y - gInputDragLastY;

        window_resize(w, targetWidth - w->width, targetHeight - w->height);
    }
}

void input_window_resize_end()
{
    gInputState = INPUT_STATE_NORMAL;
    gTooltipTimeout = 0;
    gTooltipWidget = gDragWidget;
}