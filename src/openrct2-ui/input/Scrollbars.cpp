/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Scrollbars.h"

#include <algorithm>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/Input.h>
#include <openrct2/interface/Widget.h>

/**
 *
 *  rct2: 0x006E98F2
 */
static void ScrollPartUpdateHThumb(rct_window* w, rct_widgetindex widgetIndex, int32_t x, int32_t scroll_id)
{
    if (!window_find_by_number(w->classification, w->number))
    {
        return;
    }

    int32_t newLeft = w->scrolls[scroll_id].h_right;
    newLeft *= x;

    rct_widget* widget = &w->widgets[widgetIndex];
    x = widget->right - widget->left - 21;

    if (w->scrolls[scroll_id].flags & VSCROLLBAR_VISIBLE)
    {
        x -= 11;
    }

    newLeft /= x;
    x = newLeft;
    w->scrolls[scroll_id].flags |= HSCROLLBAR_THUMB_PRESSED;

    newLeft = w->scrolls[scroll_id].h_left;
    newLeft += x;
    if (newLeft < 0)
    {
        newLeft = 0;
    }

    x = widget->right - widget->left - 1;
    if (w->scrolls[scroll_id].flags & VSCROLLBAR_VISIBLE)
    {
        x -= 11;
    }

    x *= -1;
    x += w->scrolls[scroll_id].h_right;
    if (x < 0)
    {
        x = 0;
    }

    if (newLeft > x)
    {
        newLeft = x;
    }

    w->scrolls[scroll_id].h_left = static_cast<uint16_t>(newLeft);
    widget_scroll_update_thumbs(w, widgetIndex);
    widget_invalidate_by_number(w->classification, w->number, widgetIndex);
}

/**
 *
 *  rct2: 0x006E99A9
 */
static void ScrollPartUpdateVThumb(rct_window* w, rct_widgetindex widgetIndex, int32_t y, int32_t scroll_id)
{
    rct_widget* widget = &w->widgets[widgetIndex];

    if (!window_find_by_number(w->classification, w->number))
    {
        return;
    }

    int32_t newTop = w->scrolls[scroll_id].v_bottom * y;

    y = widget->bottom - widget->top - 21;
    if (w->scrolls[scroll_id].flags & HSCROLLBAR_VISIBLE)
    {
        y -= 11;
    }

    y = newTop / y;

    w->scrolls[scroll_id].flags |= VSCROLLBAR_THUMB_PRESSED;

    newTop = w->scrolls[scroll_id].v_top + y;
    if (newTop < 0)
    {
        newTop = 0;
    }

    y = widget->bottom - widget->top - 1;
    if (w->scrolls[scroll_id].flags & HSCROLLBAR_VISIBLE)
    {
        y -= 11;
    }

    y *= -1;
    y += w->scrolls[scroll_id].v_bottom;
    if (y < 0)
    {
        y = 0;
    }

    if (newTop > y)
    {
        newTop = y;
    }

    w->scrolls[scroll_id].v_top = static_cast<uint16_t>(newTop);

    widget_scroll_update_thumbs(w, widgetIndex);
    widget_invalidate_by_number(w->classification, w->number, widgetIndex);
}

/**
 *
 *  rct2: 0x006E9A60
 */
static void ScrollPartUpdateHLeft(rct_window* w, rct_widgetindex widgetIndex, int32_t scroll_id)
{
    if (!window_find_by_number(w->classification, w->number))
    {
        return;
    }

    w->scrolls[scroll_id].flags |= HSCROLLBAR_LEFT_PRESSED;

    if (w->scrolls[scroll_id].h_left >= 3)
    {
        w->scrolls[scroll_id].h_left -= 3;
    }

    widget_scroll_update_thumbs(w, widgetIndex);
    widget_invalidate_by_number(w->classification, w->number, widgetIndex);
}

/**
 *
 *  rct2: 0x006E9ABF
 */
static void ScrollPartUpdateHRight(rct_window* w, rct_widgetindex widgetIndex, int32_t scroll_id)
{
    assert(w != nullptr);
    rct_widget* widget = &w->widgets[widgetIndex];
    if (window_find_by_number(w->classification, w->number))
    {
        w->scrolls[scroll_id].flags |= HSCROLLBAR_RIGHT_PRESSED;
        w->scrolls[scroll_id].h_left += 3;
        int32_t newLeft = widget->right - widget->left - 1;
        if (w->scrolls[scroll_id].flags & VSCROLLBAR_VISIBLE)
        {
            newLeft -= 11;
        }
        newLeft *= -1;
        newLeft += w->scrolls[scroll_id].h_right;
        if (newLeft < 0)
        {
            newLeft = 0;
        }
        if (w->scrolls[scroll_id].h_left > newLeft)
        {
            w->scrolls[scroll_id].h_left = static_cast<uint16_t>(newLeft);
        }
        widget_scroll_update_thumbs(w, widgetIndex);
        widget_invalidate_by_number(w->classification, w->number, widgetIndex);
    }
}

/**
 *
 *  rct2: 0x006E9C37
 */
static void ScrollPartUpdateVTop(rct_window* w, rct_widgetindex widgetIndex, int32_t scroll_id)
{
    assert(w != nullptr);
    if (window_find_by_number(w->classification, w->number))
    {
        w->scrolls[scroll_id].flags |= VSCROLLBAR_UP_PRESSED;
        if (w->scrolls[scroll_id].v_top >= 3)
        {
            w->scrolls[scroll_id].v_top -= 3;
        }
        widget_scroll_update_thumbs(w, widgetIndex);
        widget_invalidate_by_number(w->classification, w->number, widgetIndex);
    }
}

/**
 *
 *  rct2: 0x006E9C96
 */
static void ScrollPartUpdateVBottom(rct_window* w, rct_widgetindex widgetIndex, int32_t scroll_id)
{
    assert(w != nullptr);
    rct_widget* widget = &w->widgets[widgetIndex];
    if (window_find_by_number(w->classification, w->number))
    {
        w->scrolls[scroll_id].flags |= VSCROLLBAR_DOWN_PRESSED;
        w->scrolls[scroll_id].v_top += 3;
        int32_t newTop = widget->bottom - widget->top - 1;
        if (w->scrolls[scroll_id].flags & HSCROLLBAR_VISIBLE)
        {
            newTop -= 11;
        }
        newTop *= -1;
        newTop += w->scrolls[scroll_id].v_bottom;
        if (newTop < 0)
        {
            newTop = 0;
        }
        if (w->scrolls[scroll_id].v_top > newTop)
        {
            w->scrolls[scroll_id].v_top = static_cast<uint16_t>(newTop);
        }
        widget_scroll_update_thumbs(w, widgetIndex);
        widget_invalidate_by_number(w->classification, w->number, widgetIndex);
    }
}

/**
 *
 *  rct2: 0x006E876D
 */
static void InvalidateScroll()
{
    rct_window* w = window_find_by_number(gPressedWidget.window_classification, gPressedWidget.window_number);
    if (w != nullptr)
    {
        // Reset to basic scroll
        w->scrolls[gCurrentScrollIndex].flags &= 0xFF11;
        window_invalidate_by_number(gPressedWidget.window_classification, gPressedWidget.window_number);
    }
}

static void ScrollContinue(rct_window* w, rct_widgetindex widgetIndex, const input_mouse_data& mouseData)
{
    assert(w != nullptr);

    rct_widget* widget = &w->widgets[widgetIndex];
    if (w->classification != gPressedWidget.window_classification || w->number != gPressedWidget.window_number
        || widgetIndex != gPressedWidget.widget_index)
    {
        InvalidateScroll();
        return;
    }

    int32_t x, y, scrollPart, scrollId;
    widget_scroll_get_part(w, widget, mouseData.X, mouseData.Y, &x, &y, &scrollPart, &scrollId);

    if (gCurrentScrollArea == SCROLL_PART_HSCROLLBAR_THUMB)
    {
        int32_t originalTooltipCursorX = gTooltipCursorX;
        gTooltipCursorX = mouseData.X;
        ScrollPartUpdateHThumb(w, widgetIndex, mouseData.X - originalTooltipCursorX, scrollId);
        return;
    }

    if (gCurrentScrollArea == SCROLL_PART_VSCROLLBAR_THUMB)
    {
        int32_t originalTooltipCursorY = gTooltipCursorY;
        gTooltipCursorY = mouseData.Y;
        ScrollPartUpdateVThumb(w, widgetIndex, mouseData.Y - originalTooltipCursorY, scrollId);
        return;
    }

    if (scrollPart != gCurrentScrollArea)
    {
        InvalidateScroll();
        return;
    }

    switch (scrollPart)
    {
        case SCROLL_PART_VIEW:
            window_event_scroll_mousedrag_call(w, scrollId, x, y);
            break;

        case SCROLL_PART_HSCROLLBAR_LEFT:
            ScrollPartUpdateHLeft(w, widgetIndex, scrollId);
            break;

        case SCROLL_PART_HSCROLLBAR_RIGHT:
            ScrollPartUpdateHRight(w, widgetIndex, scrollId);
            break;

        case SCROLL_PART_VSCROLLBAR_TOP:
            ScrollPartUpdateVTop(w, widgetIndex, scrollId);
            break;

        case SCROLL_PART_VSCROLLBAR_BOTTOM:
            ScrollPartUpdateVBottom(w, widgetIndex, scrollId);
            break;

        default:
            break;
    }
}

void input_scroll_begin(rct_window* w, rct_widgetindex widgetIndex, const input_mouse_data& mouseData)
{
    rct_widget* widget = &w->widgets[widgetIndex];

    gInputState = INPUT_STATE_SCROLL_LEFT;
    gPressedWidget.window_classification = w->classification;
    gPressedWidget.window_number = w->number;
    gPressedWidget.widget_index = widgetIndex;
    gTooltipCursorX = mouseData.X;
    gTooltipCursorY = mouseData.Y;

    int32_t eax, ebx, scroll_area, scroll_id;
    scroll_id = 0; // safety
    widget_scroll_get_part(w, widget, mouseData.X, mouseData.Y, &eax, &ebx, &scroll_area, &scroll_id);

    gCurrentScrollArea = scroll_area;
    gCurrentScrollIndex = scroll_id;
    window_event_unknown_15_call(w, scroll_id, scroll_area);
    if (scroll_area == SCROLL_PART_VIEW)
    {
        window_event_scroll_mousedown_call(w, scroll_id, eax, ebx);
        return;
    }

    rct_widget* widg = &w->widgets[widgetIndex];
    rct_scroll* scroll = &w->scrolls[scroll_id];

    int32_t widget_width = widg->right - widg->left - 1;
    if (scroll->flags & VSCROLLBAR_VISIBLE)
    {
        widget_width -= 11;
    }
    int32_t widget_content_width = std::max(scroll->h_right - widget_width, 0);

    int32_t widget_height = widg->bottom - widg->top - 1;
    if (scroll->flags & HSCROLLBAR_VISIBLE)
    {
        widget_height -= 11;
    }
    int32_t widget_content_height = std::max(scroll->v_bottom - widget_height, 0);

    switch (scroll_area)
    {
        case SCROLL_PART_HSCROLLBAR_LEFT:
            scroll->h_left = (uint16_t)std::max(scroll->h_left - 3, 0);
            break;

        case SCROLL_PART_HSCROLLBAR_RIGHT:
            scroll->h_left = (uint16_t)std::min(scroll->h_left + 3, widget_content_width);
            break;

        case SCROLL_PART_HSCROLLBAR_LEFT_TROUGH:
            scroll->h_left = (uint16_t)std::max(scroll->h_left - widget_width, 0);
            break;

        case SCROLL_PART_HSCROLLBAR_RIGHT_TROUGH:
            scroll->h_left = (uint16_t)std::min(scroll->h_left + widget_width, widget_content_width);
            break;

        case SCROLL_PART_VSCROLLBAR_TOP:
            scroll->v_top = (uint16_t)std::max(scroll->v_top - 3, 0);
            break;

        case SCROLL_PART_VSCROLLBAR_BOTTOM:
            scroll->v_top = (uint16_t)std::min(scroll->v_top + 3, widget_content_height);
            break;

        case SCROLL_PART_VSCROLLBAR_TOP_TROUGH:
            scroll->v_top = (uint16_t)std::max(scroll->v_top - widget_height, 0);
            break;

        case SCROLL_PART_VSCROLLBAR_BOTTOM_TROUGH:
            scroll->v_top = (uint16_t)std::min(scroll->v_top + widget_height, widget_content_height);
            break;

        default:
            break;
    }

    widget_scroll_update_thumbs(w, widgetIndex);
    window_invalidate_by_number(w->classification, w->number);
}

void input_scroll_left(rct_window* w, const rct_widgetindex widgetIndex, const input_mouse_data& mouseData)
{
    if (mouseData.State == MOUSE_STATE_RELEASED)
    {
        ScrollContinue(w, widgetIndex, mouseData);
    }
    else
    {
        gInputState = INPUT_STATE_RESET;
        InvalidateScroll();
    }
}
