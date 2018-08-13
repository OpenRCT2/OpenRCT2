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

static void PositionEnd(rct_window* w, const input_mouse_data& mouseData)
{
    gInputState = INPUT_STATE_NORMAL;
    gTooltipTimeout = 0;
    gTooltipWidget = gDragWidget;
    window_event_moved_call(w, mouseData.X, mouseData.Y);
}

static void ResizeContinue(rct_window* w, const input_mouse_data& mouseData)
{
    if (mouseData.Y < (int32_t)context_get_height() - 2)
    {
        int32_t targetWidth = OriginalWindowWidth + mouseData.X - gInputDragLastX;
        int32_t targetHeight = OriginalWindowHeight + mouseData.Y - gInputDragLastY;

        window_resize(w, targetWidth - w->width, targetHeight - w->height);
    }
}

static void ResizeEnd()
{
    gInputState = INPUT_STATE_NORMAL;
    gTooltipTimeout = 0;
    gTooltipWidget = gDragWidget;
}

void input_window_position_begin(rct_window* w, rct_widgetindex widgetIndex, int32_t x, int32_t y)
{
    gInputState = INPUT_STATE_POSITIONING_WINDOW;
    gInputDragLastX = x - w->x;
    gInputDragLastY = y - w->y;
    gDragWidget.window_classification = w->classification;
    gDragWidget.window_number = w->number;
    gDragWidget.widget_index = widgetIndex;
}

void input_window_positioning(const input_mouse_data& mouseData)
{
    rct_window* w = window_find_by_number(gDragWidget.window_classification, gDragWidget.window_number);
    if (w == nullptr)
    {
        gInputState = INPUT_STATE_RESET;
        return;
    }

    int32_t snapProximity = (w->flags & WF_NO_SNAPPING) ? 0 : gConfigGeneral.window_snap_proximity;
    window_move_and_snap(w, mouseData.X - gInputDragLastX, mouseData.Y - gInputDragLastY, snapProximity);

    if (mouseData.State == MOUSE_STATE_LEFT_RELEASE)
    {
        PositionEnd(w, mouseData);
    }
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

void input_window_resizing(const input_mouse_data& mouseData)
{
    rct_window* w = window_find_by_number(gDragWidget.window_classification, gDragWidget.window_number);
    if (w == nullptr)
    {
        gInputState = INPUT_STATE_RESET;
        return;
    }

    if (mouseData.State == MOUSE_STATE_RELEASED)
    {
        ResizeContinue(w, mouseData);
    }
    else
    {
        ResizeEnd();
    }
}
