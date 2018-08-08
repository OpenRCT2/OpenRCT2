/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Scrollbars.h"
#include "WindowPositioning.h"

#include <algorithm>
#include <cmath>
#include <openrct2-ui/input/Input.h>
#include <openrct2-ui/input/Widget.h>
#include <openrct2-ui/interface/Dropdown.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/Context.h>
#include <openrct2/Game.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/audio/audio.h>
#include <openrct2/config/Config.h>
#include <openrct2/interface/Cursors.h>
#include <openrct2/interface/Viewport.h>
#include <openrct2/interface/Widget.h>
#include <openrct2/localisation/StringIds.h>
#include <optional>

static int16_t ClickRepeatTicks;
static uint32_t TicksSinceDragStart;
static size_t DragScrollIndex;

static constexpr const rct_string_id ColourTooltips[] = {
    STR_COLOUR_BLACK_TIP,
    STR_COLOUR_GREY_TIP,
    STR_COLOUR_WHITE_TIP,
    STR_COLOUR_DARK_PURPLE_TIP,
    STR_COLOUR_LIGHT_PURPLE_TIP,
    STR_COLOUR_BRIGHT_PURPLE_TIP,
    STR_COLOUR_DARK_BLUE_TIP,
    STR_COLOUR_LIGHT_BLUE_TIP,
    STR_COLOUR_ICY_BLUE_TIP,
    STR_COLOUR_TEAL_TIP,
    STR_COLOUR_AQUAMARINE_TIP,
    STR_COLOUR_SATURATED_GREEN_TIP,
    STR_COLOUR_DARK_GREEN_TIP,
    STR_COLOUR_MOSS_GREEN_TIP,
    STR_COLOUR_BRIGHT_GREEN_TIP,
    STR_COLOUR_OLIVE_GREEN_TIP,
    STR_COLOUR_DARK_OLIVE_GREEN_TIP,
    STR_COLOUR_BRIGHT_YELLOW_TIP,
    STR_COLOUR_YELLOW_TIP,
    STR_COLOUR_DARK_YELLOW_TIP,
    STR_COLOUR_LIGHT_ORANGE_TIP,
    STR_COLOUR_DARK_ORANGE_TIP,
    STR_COLOUR_LIGHT_BROWN_TIP,
    STR_COLOUR_SATURATED_BROWN_TIP,
    STR_COLOUR_DARK_BROWN_TIP,
    STR_COLOUR_SALMON_PINK_TIP,
    STR_COLOUR_BORDEAUX_RED_TIP,
    STR_COLOUR_SATURATED_RED_TIP,
    STR_COLOUR_BRIGHT_RED_TIP,
    STR_COLOUR_DARK_PINK_TIP,
    STR_COLOUR_BRIGHT_PINK_TIP,
    STR_COLOUR_LIGHT_PINK_TIP,
};

static void UpdateTooltip(rct_window* w, rct_widgetindex widgetIndex, const input_mouse_data& mouseData)
{
    if (gTooltipWidget.window_classification == 255)
    {
        if (gTooltipCursorX == mouseData.X && gTooltipCursorY == mouseData.Y)
        {
            gTooltipsNotShownTicks++;
            if (gTooltipsNotShownTicks > 50)
            {
                gTooltipTimeout = 0;
                window_tooltip_open(w, widgetIndex, mouseData.X, mouseData.Y);
            }
        }
    }
    else
    {
        reset_tooltip_not_shown();

        if (w == nullptr || gTooltipWidget.window_classification != w->classification
            || gTooltipWidget.window_number != w->number || gTooltipWidget.widget_index != widgetIndex)
        {
            window_tooltip_close();
        }

        gTooltipTimeout += gTicksSinceLastUpdate;
        if (gTooltipTimeout >= 8000)
        {
            window_close_by_class(WC_TOOLTIP);
        }
    }
}

static bool HandleDropdown(rct_widgetindex widgetIndex, rct_window* w, rct_window* cursor_w, const input_mouse_data& mouseData)
{
    if (w == nullptr)
    {
        return true;
    }

    int32_t dropdown_index = 0;
    if (w->classification == WC_DROPDOWN)
    {
        dropdown_index = dropdown_index_from_point(mouseData.X, mouseData.Y, w);
        if (dropdown_index == -1)
        {
            return true;
        }

        if (dropdown_index < DROPDOWN_ITEMS_MAX_SIZE
            && (dropdown_is_disabled(dropdown_index) || gDropdownItemsFormat[dropdown_index] == DROPDOWN_SEPARATOR))
        {
            return true;
        }
    }
    else
    {
        if (gPressedWidget.window_classification != w->classification || gPressedWidget.window_number != w->number
            || gPressedWidget.widget_index != widgetIndex)
        {
            return true;
        }

        dropdown_index = -1;
        if ((gInputFlags & INPUT_FLAG_DROPDOWN_STAY_OPEN) && !(gInputFlags & INPUT_FLAG_DROPDOWN_MOUSE_UP))
        {
            gInputFlags |= INPUT_FLAG_DROPDOWN_MOUSE_UP;
            return false;
        }
    }

    window_close_by_class(WC_DROPDOWN);
    cursor_w = window_find_by_number(gPressedWidget.window_classification, gPressedWidget.window_number);
    if (gInputFlags & INPUT_FLAG_WIDGET_PRESSED)
    {
        gInputFlags &= ~INPUT_FLAG_WIDGET_PRESSED;
        widget_invalidate_by_number(
            gPressedWidget.window_classification, gPressedWidget.window_number, gPressedWidget.widget_index);
    }

    gInputState = INPUT_STATE_NORMAL;
    gTooltipTimeout = 0;
    gTooltipWidget.widget_index = gPressedWidget.widget_index;
    gTooltipWidget.window_classification = gPressedWidget.window_classification;
    gTooltipWidget.window_number = gPressedWidget.window_number;

    if (dropdown_index == -1 && !dropdown_is_disabled(gDropdownDefaultIndex))
    {
        dropdown_index = gDropdownDefaultIndex;
    }

    window_event_dropdown_call(cursor_w, gPressedWidget.widget_index, dropdown_index);
    return true;
}

static void ViewportDragBegin(rct_window* w)
{
    if (w == nullptr)
    {
        return;
    }

    w->flags &= ~WF_SCROLLING_TO_LOCATION;
    gInputState = INPUT_STATE_VIEWPORT_RIGHT;
    gDragWidget.window_classification = w->classification;
    gDragWidget.window_number = w->number;
    TicksSinceDragStart = 0;
    context_get_cursor_position(&gInputDragLastX, &gInputDragLastY);
    context_hide_cursor();

    window_unfollow_sprite(w);
    // gInputFlags |= INPUT_FLAG_5;
}

/**
 *
 *  rct2: 0x006E957F
 */
static void ScrollDragBegin(rct_window* w, rct_widgetindex widgetIndex, const input_mouse_data& mouseData)
{
    if (w == nullptr)
    {
        return;
    }

    gInputState = INPUT_STATE_SCROLL_RIGHT;
    gInputDragLastX = mouseData.X;
    gInputDragLastY = mouseData.Y;

    gDragWidget.window_classification = w->classification;
    gDragWidget.window_number = w->number;
    gDragWidget.widget_index = widgetIndex;

    TicksSinceDragStart = 0;

    DragScrollIndex = static_cast<size_t>(window_get_scroll_data_index(w, widgetIndex));
    context_hide_cursor();
}

/**
 * Used to invalidate flat button widgets when the mouse leaves and enters them. This should be generalised so that all widgets
 * can use this in the future.
 */
static void WidgetOverFlatbuttonInvalidate()
{
    rct_window* w = window_find_by_number(gHoverWidget.window_classification, gHoverWidget.window_number);
    if (w != nullptr)
    {
        window_event_invalidate_call(w);
        if (w->widgets[gHoverWidget.widget_index].type == WWT_FLATBTN)
        {
            widget_invalidate_by_number(
                gHoverWidget.window_classification, gHoverWidget.window_number, gHoverWidget.widget_index);
        }
    }
}

/**
 *
 *  rct2: 0x006E9269
 */
static void WidgetOverChangeCheck(rct_windowclass windowClass, rct_windownumber windowNumber, rct_widgetindex widgetIndex)
{
    // Prevents invalid widgets being clicked source of bug is elsewhere
    if (widgetIndex == -1)
    {
        return;
    }

    // Check if the widget that the cursor was over, has changed
    if (windowClass == gHoverWidget.window_classification && windowNumber == gHoverWidget.window_number
        && widgetIndex == gHoverWidget.widget_index)
    {
        return;
    }

    // Invalidate last widget cursor was on if widget is a flat button
    WidgetOverFlatbuttonInvalidate();

    // Set new cursor over widget
    gHoverWidget.window_classification = windowClass;
    gHoverWidget.window_number = windowNumber;
    gHoverWidget.widget_index = widgetIndex;

    // Invalidate new widget cursor is on if widget is a flat button
    if (windowClass != 255)
    {
        WidgetOverFlatbuttonInvalidate();
    }
}

static void ViewportLeftReleased(rct_window* w, const input_mouse_data& mouseData)
{
    if (w->viewport == nullptr)
    {
        gInputState = INPUT_STATE_RESET;
        return;
    }

    if (w->classification != gDragWidget.window_classification || w->number != gDragWidget.window_number
        || !(gInputFlags & INPUT_FLAG_TOOL_ACTIVE))
    {
        return;
    }

    rct_window* toolWindow = window_find_by_number(gCurrentToolWidget.window_classification, gCurrentToolWidget.window_number);
    if (toolWindow != nullptr)
    {
        window_event_tool_drag_call(toolWindow, gCurrentToolWidget.widget_index, mouseData.X, mouseData.Y);
    }
}

static void ViewportLeftLeftRelease(rct_window* w, const input_mouse_data& mouseData)
{
    if (gDragWidget.window_number != w->number)
    {
        return;
    }

    gInputState = INPUT_STATE_RESET;
    if ((gInputFlags & INPUT_FLAG_TOOL_ACTIVE))
    {
        w = window_find_by_number(gCurrentToolWidget.window_classification, gCurrentToolWidget.window_number);
        if (w != nullptr)
        {
            window_event_tool_up_call(w, gCurrentToolWidget.widget_index, mouseData.X, mouseData.Y);
        }
    }
    else if (!(gInputFlags & INPUT_FLAG_4))
    {
        viewport_interaction_left_click(mouseData.X, mouseData.Y);
    }
}

/**
 *
 *  rct2: 0x006E8ACB
 */
void input_scroll_right(const input_mouse_data& mouseData)
{
    rct_window* w = window_find_by_number(gDragWidget.window_classification, gDragWidget.window_number);
    if (w == nullptr)
    {
        context_show_cursor();
        gInputState = INPUT_STATE_RESET;
        return;
    }

    switch (mouseData.State)
    {
        case MOUSE_STATE_RELEASED:
            TicksSinceDragStart += gTicksSinceLastUpdate;
            if (mouseData.X != 0 || mouseData.Y != 0)
            {
                TicksSinceDragStart = 1000;
                input_scroll_drag_continue(w, mouseData);
            }
            break;

        case MOUSE_STATE_RIGHT_RELEASE:
            gInputState = INPUT_STATE_RESET;
            context_show_cursor();
            break;

        default:
            break;
    }
}

/**
 * Based on (heavily changed)
 *  rct2: 0x006E9E0E,  0x006E9ED0
 */
void input_scroll_drag_continue(rct_window* w, const input_mouse_data& mouseData)
{
    rct_widgetindex widgetIndex = gDragWidget.widget_index;
    size_t scrollIndex = DragScrollIndex;

    rct_widget* widget = &w->widgets[widgetIndex];
    rct_scroll* scroll = &w->scrolls[scrollIndex];

    int32_t dx, dy;
    dx = mouseData.X - gInputDragLastX;
    dy = mouseData.Y - gInputDragLastY;

    if (scroll->flags & HSCROLLBAR_VISIBLE)
    {
        int32_t size = widget->right - widget->left - 1;
        if (scroll->flags & VSCROLLBAR_VISIBLE)
        {
            size -= 11;
        }
        size = std::max(0, scroll->h_right - size);
        scroll->h_left = (uint16_t)std::clamp(scroll->h_left + dx, 0, size);
    }

    if (scroll->flags & VSCROLLBAR_VISIBLE)
    {
        int32_t size = widget->bottom - widget->top - 1;
        if (scroll->flags & HSCROLLBAR_VISIBLE)
        {
            size -= 11;
        }
        size = std::max(0, scroll->v_bottom - size);

        scroll->v_top = (uint16_t)std::clamp(scroll->v_top + dy, 0, size);
    }

    widget_scroll_update_thumbs(w, widgetIndex);
    window_invalidate_by_number(w->classification, w->number);

    auto fixedCursorPositionX = (int32_t)std::ceil(gInputDragLastX * gConfigGeneral.window_scale);
    auto fixedCursorPositionY = (int32_t)std::ceil(gInputDragLastY * gConfigGeneral.window_scale);

    context_set_cursor_position(fixedCursorPositionX, fixedCursorPositionY);
}

/**
 *
 *  rct2: 0x006E9253
 */
void input_widget_over(rct_window* w, const input_mouse_data& mouseData, const rct_widgetindex widgetIndex)
{
    rct_windowclass windowClass = WC_NULL;
    rct_windownumber windowNumber = 0;
    rct_widget* widget = nullptr;

    if (w != nullptr)
    {
        windowClass = w->classification;
        windowNumber = w->number;
        widget = &w->widgets[widgetIndex];
    }

    WidgetOverChangeCheck(windowClass, windowNumber, widgetIndex);

    if (w != nullptr && widgetIndex != -1 && widget->type == WWT_SCROLL)
    {
        int32_t eax, ebx, scroll_part, edx;
        widget_scroll_get_part(w, widget, mouseData.X, mouseData.Y, &eax, &ebx, &scroll_part, &edx);

        if (scroll_part != SCROLL_PART_VIEW)
        {
            window_tooltip_close();
        }
        else
        {
            window_event_scroll_mouseover_call(w, edx, eax, ebx);
            UpdateTooltip(w, widgetIndex, mouseData);
        }
    }
    else
    {
        UpdateTooltip(w, widgetIndex, mouseData);
    }

    gTooltipTimeout = 0;
    gTooltipCursorX = mouseData.X;
    gTooltipCursorY = mouseData.Y;
}

/**
 *
 *  rct2: 0x006E95F9
 */
void input_widget_left(rct_window* w, const input_mouse_data& mouseData, const rct_widgetindex widgetIndex)
{
    rct_windowclass windowClass = WC_NULL;
    rct_windownumber windowNumber = 0;

    if (w != nullptr)
    {
        windowClass = w->classification;
        windowNumber = w->number;
    }

    window_close_by_class(WC_ERROR);
    window_close_by_class(WC_TOOLTIP);

    // Window might have changed position in the list, therefore find it again
    w = window_find_by_number(windowClass, windowNumber);
    if (w == nullptr)
    {
        return;
    }

    w = window_bring_to_front(w);
    if (widgetIndex == -1)
    {
        return;
    }

    if (windowClass != gCurrentTextBox.window.classification || windowNumber != gCurrentTextBox.window.number
        || widgetIndex != gCurrentTextBox.widget_index)
    {
        window_cancel_textbox();
    }

    switch (rct_widget* widget = &w->widgets[widgetIndex]; widget->type)
    {
        case WWT_FRAME:
        case WWT_RESIZE:
            if (window_can_resize(w) && mouseData.X >= w->x + w->width - 19 && mouseData.Y >= w->y + w->height - 19)
            {
                input_window_resize_begin(w, widgetIndex, mouseData);
            }
            break;

        case WWT_VIEWPORT:
            gInputState = INPUT_STATE_VIEWPORT_LEFT;
            gInputDragLastX = mouseData.X;
            gInputDragLastY = mouseData.Y;
            gDragWidget.window_classification = windowClass;
            gDragWidget.window_number = windowNumber;

            if (!(gInputFlags & INPUT_FLAG_TOOL_ACTIVE))
            {
                break;
            }

            w = window_find_by_number(gCurrentToolWidget.window_classification, gCurrentToolWidget.window_number);
            if (w != nullptr)
            {
                window_event_tool_down_call(w, gCurrentToolWidget.widget_index, mouseData.X, mouseData.Y);
                gInputFlags |= INPUT_FLAG_4;
            }
            break;

        case WWT_CAPTION:
            input_window_position_begin(w, widgetIndex, mouseData.X, mouseData.Y);
            break;

        case WWT_SCROLL:
            input_scroll_begin(w, widgetIndex, mouseData);
            break;

        default:
            if (!widget_is_enabled(w, widgetIndex) || widget_is_disabled(w, widgetIndex))
            {
                break;
            }

            audio_play_sound(SOUND_CLICK_1, 0, w->x + ((widget->left + widget->right) / 2));

            // Set new cursor down widget
            gPressedWidget.window_classification = windowClass;
            gPressedWidget.window_number = windowNumber;
            gPressedWidget.widget_index = widgetIndex;
            gInputFlags |= INPUT_FLAG_WIDGET_PRESSED;
            gInputState = INPUT_STATE_WIDGET_PRESSED;
            ClickRepeatTicks = 1;

            widget_invalidate_by_number(windowClass, windowNumber, widgetIndex);
            window_event_mouse_down_call(w, widgetIndex);

            break;
    }
}

void input_widget_right(
    rct_window* w, const input_mouse_data& mouseData, const rct_widget* widget, const rct_widgetindex widgetIndex)
{
    window_close_by_class(WC_TOOLTIP);

    if (w != nullptr)
    {
        w = window_bring_to_front(w);
    }

    if (widget != nullptr)
    {
        if (widget->type == WWT_VIEWPORT && !(gScreenFlags & (SCREEN_FLAGS_TRACK_MANAGER | SCREEN_FLAGS_TITLE_DEMO)))
        {
            ViewportDragBegin(w);
        }
        else if (widget->type == WWT_SCROLL)
        {
            ScrollDragBegin(w, widgetIndex, mouseData);
        }
    }
}

/**
 *
 *  rct2: 0x006E8DA7
 */
void input_widget_pressed(rct_widgetindex widgetIndex, rct_window* w, rct_widget* widget, const input_mouse_data& mouseData)
{
    rct_window* cursor_w = window_find_by_number(gPressedWidget.window_classification, gPressedWidget.window_number);
    if (cursor_w == nullptr)
    {
        gInputState = INPUT_STATE_RESET;
        return;
    }

    if (widget == nullptr)
    {
        return;
    }

    switch (mouseData.State)
    {
        case MOUSE_STATE_RELEASED:
            if (gPressedWidget.window_classification != w->classification || gPressedWidget.window_number != w->number
                || gPressedWidget.widget_index != widgetIndex || w->disabled_widgets & (1 << widgetIndex))
            {
                break;
            }

            if (ClickRepeatTicks != 0)
            {
                ClickRepeatTicks++;

                // Handle click repeat
                auto is_hold_down_widget = static_cast<bool>(w->hold_down_widgets & (1 << widgetIndex));
                if (ClickRepeatTicks >= 16 && !(ClickRepeatTicks & 3) && is_hold_down_widget)
                {
                    window_event_mouse_down_call(w, widgetIndex);
                }
            }

            if (gInputFlags & INPUT_FLAG_WIDGET_PRESSED)
            {
                if (gInputState == INPUT_STATE_DROPDOWN_ACTIVE)
                {
                    gDropdownHighlightedIndex = gDropdownDefaultIndex;
                    window_invalidate_by_class(WC_DROPDOWN);
                }
                return;
            }

            gInputFlags |= INPUT_FLAG_WIDGET_PRESSED;
            widget_invalidate_by_number(gPressedWidget.window_classification, gPressedWidget.window_number, widgetIndex);

            return;

        case MOUSE_STATE_LEFT_RELEASE:
        case MOUSE_STATE_RIGHT_PRESS:
            if (gInputState == INPUT_STATE_DROPDOWN_ACTIVE)
            {
                if (!HandleDropdown(widgetIndex, w, cursor_w, mouseData))
                {
                    return;
                }

                window_close_by_class(WC_DROPDOWN);

                // Update w as it will be invalid after closing the dropdown window
                w = window_find_by_number(w->classification, w->number);
            }

            gInputState = INPUT_STATE_NORMAL;

            if (mouseData.State == MOUSE_STATE_LEFT_RELEASE)
            {
                gTooltipTimeout = 0;
                gTooltipWidget.widget_index = gPressedWidget.widget_index;

                if (w == nullptr)
                {
                    break;
                }

                int32_t mid_point_x = (widget->left + widget->right) / 2 + w->x;
                audio_play_sound(SOUND_CLICK_2, 0, mid_point_x);

                if (gPressedWidget.window_classification != w->classification || gPressedWidget.window_number != w->number
                    || gPressedWidget.widget_index != widgetIndex || (w->disabled_widgets & (1 << widgetIndex)))
                {
                    break;
                }

                widget_invalidate_by_number(gPressedWidget.window_classification, gPressedWidget.window_number, widgetIndex);
                window_event_mouse_up_call(w, widgetIndex);
            }

            return;

        default:
            return;
    }

    ClickRepeatTicks = 0;
    if (gInputState != INPUT_STATE_DROPDOWN_ACTIVE)
    {
        // Hold down widget and drag outside of area??
        if (gInputFlags & INPUT_FLAG_WIDGET_PRESSED)
        {
            gInputFlags &= ~INPUT_FLAG_WIDGET_PRESSED;
            widget_invalidate_by_number(
                gPressedWidget.window_classification, gPressedWidget.window_number, gPressedWidget.widget_index);
        }
        return;
    }

    gDropdownHighlightedIndex = -1;
    window_invalidate_by_class(WC_DROPDOWN);

    if (w->classification != WC_DROPDOWN)
    {
        gDropdownLastColourHover = -1;
        window_tooltip_close();
        return;
    }

    if (int32_t dropdownIndex = dropdown_index_from_point(mouseData.X, mouseData.Y, w); dropdownIndex != -1)
    {
        if (gDropdownIsColour && gDropdownLastColourHover != dropdownIndex)
        {
            gDropdownLastColourHover = dropdownIndex;

            window_tooltip_close();
            window_tooltip_show(STR_COLOUR_BLACK_TIP + dropdownIndex, mouseData.X, mouseData.Y);
        }

        if (dropdownIndex < DROPDOWN_ITEMS_MAX_SIZE
            && (dropdown_is_disabled(dropdownIndex) || gDropdownItemsFormat[dropdownIndex] == DROPDOWN_SEPARATOR))
        {
            return;
        }

        gDropdownHighlightedIndex = dropdownIndex;
        window_invalidate_by_class(WC_DROPDOWN);
    }
}

std::optional<int32_t> input_get_widget_cursor_id(
    rct_window* window, rct_widgetindex widgetId, const input_mouse_data& mouseData)
{
    switch (window->widgets[widgetId].type)
    {
        case WWT_VIEWPORT:
            if (gInputFlags & INPUT_FLAG_TOOL_ACTIVE)
            {
                return gCurrentToolId;
            }

            if (viewport_interaction_left_over(mouseData.X, mouseData.Y))
            {
                input_set_mouse_cursor(CURSOR_HAND_POINT);
                return {};
            }

            // Window event WE_UNKNOWN_0E was called here, but no windows actually implemented a handler and
            // it's not known what it was for
            return CURSOR_ARROW;

        case WWT_FRAME:
        case WWT_RESIZE:
            if (!(window->flags & WF_RESIZABLE)
                || (window->min_width == window->max_width && window->min_height == window->max_height)
                || mouseData.X < window->x + window->width - 19 || mouseData.Y < window->y + window->height - 19)
            {
                return CURSOR_ARROW;
            }

            return CURSOR_DIAGONAL_ARROWS;

        case WWT_SCROLL:
        {
            int32_t output_scroll_area, scroll_id;
            int32_t scroll_x, scroll_y;
            widget_scroll_get_part(
                window, &window->widgets[widgetId], mouseData.X, mouseData.Y, &scroll_x, &scroll_y, &output_scroll_area,
                &scroll_id);

            if (output_scroll_area != SCROLL_PART_VIEW)
            {
                return CURSOR_ARROW;
            }

            // Same as default but with scroll_x/y
            int32_t cursorId = window_event_cursor_call(window, widgetId, scroll_x, scroll_y);
            return cursorId == -1 ? CURSOR_ARROW : cursorId;
        }

        default:
        {
            int32_t cursorId = window_event_cursor_call(window, widgetId, mouseData.X, mouseData.Y);
            return cursorId == -1 ? CURSOR_ARROW : cursorId;
        }
    }
}

void input_viewport_drag_end()
{
    gInputState = INPUT_STATE_RESET;
    context_show_cursor();
}

void input_viewport_drag_continue()
{
    // The window can be closed during a drag session, so just finish the session if the window no longer exists
    rct_window* w = window_find_by_number(gDragWidget.window_classification, gDragWidget.window_number);
    if (w == nullptr)
    {
        input_viewport_drag_end();
        return;
    }

    int32_t newDragX, newDragY;
    context_get_cursor_position(&newDragX, &newDragY);
    int32_t dx = newDragX - gInputDragLastX;
    int32_t dy = newDragY - gInputDragLastY;

    TicksSinceDragStart += gTicksSinceLastUpdate;
    if (w->viewport == nullptr)
    {
        context_show_cursor();
        gInputState = INPUT_STATE_RESET;
    }
    else if ((dx != 0 || dy != 0) && !(w->flags & WF_NO_SCROLLING))
    {
        // User dragged a scrollable viewport

        // If the drag time is less than 500 the "drag" is usually interpreted as a right click.
        // As the user moved the mouse, don't interpret it as right click in any case.
        TicksSinceDragStart = 1000;

        int32_t zoomFactor = 1 << (w->viewport->zoom + 1);

        dx *= zoomFactor;
        dy *= zoomFactor;

        if (gConfigGeneral.invert_viewport_drag)
        {
            w->saved_view_x -= dx;
            w->saved_view_y -= dy;
        }
        else
        {
            w->saved_view_x += dx;
            w->saved_view_y += dy;
        }
    }

    context_set_cursor_position(gInputDragLastX, gInputDragLastY);
}

void input_viewport_right(const input_mouse_data& mouseData)
{
    if (mouseData.State == MOUSE_STATE_RELEASED)
    {
        input_viewport_drag_continue();
        return;
    }

    input_viewport_drag_end();
    if (TicksSinceDragStart < 500)
    {
        // If the user pressed the right mouse button for less than 500 ticks, interpret as right click
        viewport_interaction_right_click(mouseData.X, mouseData.Y);
    }
}

void input_viewport_left(const input_mouse_data& mouseData)
{
    rct_window* w = window_find_by_number(gDragWidget.window_classification, gDragWidget.window_number);
    if (w == nullptr)
    {
        gInputState = INPUT_STATE_RESET;
        return;
    }

    switch (mouseData.State)
    {
        case MOUSE_STATE_RELEASED:
            ViewportLeftReleased(w, mouseData);
            break;

        case MOUSE_STATE_LEFT_RELEASE:
            ViewportLeftLeftRelease(w, mouseData);
            break;

        default:
            break;
    }
}