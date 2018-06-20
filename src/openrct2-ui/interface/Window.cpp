/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <openrct2/audio/audio.h>
#include <openrct2/config/Config.h>
#include <openrct2/Context.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/Input.h>
#include <openrct2/interface/Widget.h>
#include <openrct2/localisation/StringIds.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/sprites.h>
#include <openrct2/world/Sprite.h>
#include <openrct2/ui/UiContext.h>
#include "Theme.h"
#include "Window.h"

using namespace OpenRCT2;

// The amount of pixels to scroll per wheel click
constexpr int32_t WINDOW_SCROLL_PIXELS = 17;

static int32_t _previousAbsoluteWheel = 0;

static bool window_fits_between_others(int32_t x, int32_t y, int32_t width, int32_t height)
{
    for (auto& w : g_window_list)
    {
        if (w->flags & WF_STICK_TO_BACK)
            continue;

        if (x + width <= w->x) continue;
        if (x >= w->x + w->width) continue;
        if (y + height <= w->y) continue;
        if (y >= w->y + w->height) continue;
        return false;
    }

    return true;
}

static bool window_fits_within_space(int32_t x, int32_t y, int32_t width, int32_t height)
{
    if (x < 0) return false;
    if (y <= TOP_TOOLBAR_HEIGHT && !(gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)) return false;
    if (x + width > context_get_width()) return false;
    if (y + height > context_get_height()) return false;
    return window_fits_between_others(x, y, width, height);
}

static bool window_fits_on_screen(int32_t x, int32_t y, int32_t width, int32_t height)
{
    uint16_t screenWidth = context_get_width();
    uint16_t screenHeight = context_get_height();
    int32_t unk;

    unk = -(width / 4);
    if (x < unk) return false;
    unk = screenWidth + (unk * 2);
    if (x > unk) return false;
    if (y <= TOP_TOOLBAR_HEIGHT && !(gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)) return false;
    unk = screenHeight - (height / 4);
    if (y > unk) return false;
    return window_fits_between_others(x, y, width, height);
}

rct_window *window_create(int32_t x, int32_t y, int32_t width, int32_t height, rct_window_event_list *event_handlers, rct_windowclass cls, uint16_t flags)
{
    // Check if there are any window slots left
    // include WINDOW_LIMIT_RESERVED for items such as the main viewport and toolbars to not appear to be counted.
    if (g_window_list.size() >= (size_t)(gConfigGeneral.window_limit + WINDOW_LIMIT_RESERVED))
    {
        // Close least recently used window
        for (auto& w : g_window_list)
        {
            if (!(w->flags & (WF_STICK_TO_BACK | WF_STICK_TO_FRONT | WF_NO_AUTO_CLOSE)))
            {
                window_close(w.get());
                break;
            }
        }
    }

    // Find right position to insert new window
    auto dstIndex = g_window_list.size();
    if (flags & WF_STICK_TO_BACK)
    {
        for (size_t i = 0; i < g_window_list.size(); i++)
        {
            if (!(g_window_list[i]->flags & WF_STICK_TO_BACK))
            {
                dstIndex = i;
            }
        }
    }
    else if (!(flags & WF_STICK_TO_FRONT))
    {
        for (size_t i = g_window_list.size(); i > 0; i--)
        {
            if (!(g_window_list[i - 1]->flags & WF_STICK_TO_FRONT))
            {
                dstIndex = i;
                break;
            }
        }
    }

    g_window_list.insert(g_window_list.begin() + dstIndex, std::make_unique<rct_window>());
    auto w = g_window_list[dstIndex].get();

    // Setup window
    w->classification = cls;
    w->var_4B8 = -1;
    w->var_4B9 = -1;
    w->flags = flags;

    // Play sounds and flash the window
    if (!(flags & (WF_STICK_TO_BACK | WF_STICK_TO_FRONT))) {
        w->flags |= WF_WHITE_BORDER_MASK;
        audio_play_sound(SOUND_WINDOW_OPEN, 0, x + (width / 2));
    }

    w->number = 0;
    w->x = x;
    w->y = y;
    w->width = width;
    w->height = height;
    w->viewport = nullptr;
    w->event_handlers = event_handlers;
    w->enabled_widgets = 0;
    w->disabled_widgets = 0;
    w->pressed_widgets = 0;
    w->hold_down_widgets = 0;
    w->viewport_focus_coordinates.var_480 = 0;
    w->viewport_focus_coordinates.x = 0;
    w->viewport_focus_coordinates.y = 0;
    w->viewport_focus_coordinates.z = 0;
    w->viewport_focus_coordinates.rotation = 0;
    w->page = 0;
    w->var_48C = 0;
    w->frame_no = 0;
    w->list_information_type = 0;
    w->var_492 = 0;
    w->selected_tab = 0;
    w->var_4AE = 0;
    w->viewport_smart_follow_sprite = SPRITE_INDEX_NULL;

    colour_scheme_update(w);
    window_invalidate(w);
    return w;
}

rct_window *window_create_auto_pos(int32_t width, int32_t height, rct_window_event_list *event_handlers, rct_windowclass cls, uint16_t flags)
{
    auto uiContext = GetContext()->GetUiContext();
    auto screenWidth = uiContext->GetWidth();
    auto screenHeight = uiContext->GetHeight();

    // TODO dead code, looks like it is cascading the new window offset from an existing window
    // we will have to re-implement this in our own way.
    //
    // if (cls & 0x80) {
    //  cls &= ~0x80;
    //  rct_window *w = window_find_by_number(0, 0);
    //  if (w != nullptr) {
    //      if (w->x > -60 && w->x < screenWidth - 20) {
    //          if (w->y < screenHeight - 20) {
    //              int32_t x = w->x;
    //              if (w->x + width > screenWidth)
    //                  x = screenWidth - 20 - width;
    //              int32_t y = w->y;
    //              return window_create(x + 10, y + 10, width, height, event_handlers, cls, flags);
    //          }
    //      }
    //  }
    // }

    // Place window in an empty corner of the screen
    int32_t x = 0;
    int32_t y = 30;
    if (window_fits_within_space(x, y, width, height)) goto foundSpace;

    x = screenWidth - width;
    y = 30;
    if (window_fits_within_space(x, y, width, height)) goto foundSpace;

    x = 0;
    y = screenHeight - 34 - height;
    if (window_fits_within_space(x, y, width, height)) goto foundSpace;

    x = screenWidth - width;
    y = screenHeight - 34 - height;
    if (window_fits_within_space(x, y, width, height)) goto foundSpace;

    // Place window next to another
    for (auto& w : g_window_list)
    {
        if (w->flags & WF_STICK_TO_BACK)
            continue;

        x = w->x + w->width + 2;
        y = w->y;
        if (window_fits_within_space(x, y, width, height)) goto foundSpace;

        x = w->x - w->width - 2;
        y = w->y;
        if (window_fits_within_space(x, y, width, height)) goto foundSpace;

        x = w->x;
        y = w->y + w->height + 2;
        if (window_fits_within_space(x, y, width, height)) goto foundSpace;

        x = w->x;
        y = w->y - w->height - 2;
        if (window_fits_within_space(x, y, width, height)) goto foundSpace;

        x = w->x + w->width + 2;
        y = w->y - w->height - 2;
        if (window_fits_within_space(x, y, width, height)) goto foundSpace;

        x = w->x - w->width - 2;
        y = w->y - w->height - 2;
        if (window_fits_within_space(x, y, width, height)) goto foundSpace;

        x = w->x + w->width + 2;
        y = w->y + w->height + 2;
        if (window_fits_within_space(x, y, width, height)) goto foundSpace;

        x = w->x - w->width - 2;
        y = w->y + w->height + 2;
        if (window_fits_within_space(x, y, width, height)) goto foundSpace;
    }

    // Overlap
    for (auto& w : g_window_list)
    {
        if (w->flags & WF_STICK_TO_BACK)
            continue;

        x = w->x + w->width + 2;
        y = w->y;
        if (window_fits_on_screen(x, y, width, height)) goto foundSpace;

        x = w->x - w->width - 2;
        y = w->y;
        if (window_fits_on_screen(x, y, width, height)) goto foundSpace;

        x = w->x;
        y = w->y + w->height + 2;
        if (window_fits_on_screen(x, y, width, height)) goto foundSpace;

        x = w->x;
        y = w->y - w->height - 2;
        if (window_fits_on_screen(x, y, width, height)) goto foundSpace;
    }

    // Cascade
    x = 0;
    y = 30;
    for (auto& w : g_window_list)
    {
        if (x == w->x && y == w->y)
        {
            x += 5;
            y += 5;
        }
    }

    // Clamp to inside the screen
foundSpace:
    if (x < 0)
        x = 0;
    if (x + width > screenWidth)
        x = screenWidth - width;

    return window_create(x, y, width, height, event_handlers, cls, flags);
}

rct_window * window_create_centred(int32_t width, int32_t height, rct_window_event_list *event_handlers, rct_windowclass cls, uint16_t flags)
{
    auto uiContext = GetContext()->GetUiContext();
    auto screenWidth = uiContext->GetWidth();
    auto screenHeight = uiContext->GetHeight();

    int32_t x = (screenWidth - width) / 2;
    int32_t y = std::max(TOP_TOOLBAR_HEIGHT + 1, (screenHeight - height) / 2);
    return window_create(x, y, width, height, event_handlers, cls, flags);
}

static int32_t window_get_widget_index(rct_window *w, rct_widget *widget)
{
    int32_t i = 0;
    for (rct_widget *widget2 = w->widgets; widget2->type != WWT_LAST; widget2++, i++)
        if (widget == widget2)
            return i;
    return -1;
}

static int32_t window_get_scroll_index(rct_window *w, int32_t targetWidgetIndex)
{
    if (w->widgets[targetWidgetIndex].type != WWT_SCROLL)
        return -1;

    int32_t scrollIndex = 0;
    rct_widgetindex widgetIndex = 0;
    for (rct_widget *widget = w->widgets; widget->type != WWT_LAST; widget++, widgetIndex++) {
        if (widgetIndex == targetWidgetIndex)
            break;
        if (widget->type == WWT_SCROLL)
            scrollIndex++;
    }

    return scrollIndex;
}

static rct_widget *window_get_scroll_widget(rct_window *w, int32_t scrollIndex)
{
    for (rct_widget *widget = w->widgets; widget->type != WWT_LAST; widget++) {
        if (widget->type != WWT_SCROLL)
            continue;

        if (scrollIndex == 0)
            return widget;
        scrollIndex--;
    }

    return nullptr;
}

/**
 *
 *  rct2: 0x006E78E3
 */
static void window_scroll_wheel_input(rct_window *w, int32_t scrollIndex, int32_t wheel)
{
    rct_scroll *scroll = &w->scrolls[scrollIndex];
    rct_widget *widget = window_get_scroll_widget(w, scrollIndex);
    rct_widgetindex widgetIndex = window_get_widget_index(w, widget);

    if (scroll->flags & VSCROLLBAR_VISIBLE) {
        int32_t size = widget->bottom - widget->top - 1;
        if (scroll->flags & HSCROLLBAR_VISIBLE)
            size -= 11;
        size = std::max(0, scroll->v_bottom - size);
        scroll->v_top = std::min(std::max(0, scroll->v_top + wheel), size);
    } else {
        int32_t size = widget->right - widget->left - 1;
        if (scroll->flags & VSCROLLBAR_VISIBLE)
            size -= 11;
        size = std::max(0, scroll->h_right - size);
        scroll->h_left = std::min(std::max(0, scroll->h_left + wheel), size);
    }

    widget_scroll_update_thumbs(w, widgetIndex);
    widget_invalidate(w, widgetIndex);
}

/**
 *
 *  rct2: 0x006E793B
 */
static int32_t window_wheel_input(rct_window *w, int32_t wheel)
{
    int32_t i = 0;
    for (rct_widget *widget = w->widgets; widget->type != WWT_LAST; widget++) {
        if (widget->type != WWT_SCROLL)
            continue;

        // Originally always checked first scroll view, bug maybe?
        rct_scroll *scroll = &w->scrolls[i];
        if (scroll->flags & (HSCROLLBAR_VISIBLE | VSCROLLBAR_VISIBLE)) {
            window_scroll_wheel_input(w, i, wheel);
            return 1;
        }
        i++;
    }

    return 0;
}

/**
 *
 *  rct2: 0x006E79FB
 */
static void window_viewport_wheel_input(rct_window *w, int32_t wheel)
{
    if (gScreenFlags & (SCREEN_FLAGS_TRACK_MANAGER | SCREEN_FLAGS_TITLE_DEMO))
        return;

    if (wheel < 0)
        window_zoom_in(w, true);
    else if (wheel > 0)
        window_zoom_out(w, true);
}

static bool window_other_wheel_input(rct_window* w, rct_widgetindex widgetIndex, int32_t wheel)
{
    // HACK: Until we have a new window system that allows us to add new events like mouse wheel easily,
    //       this selective approach will have to do.

    // Allow mouse wheel scrolling to increment or decrement the land tool size for various windows
    auto widgetType = w->widgets[widgetIndex].type;

    // Lower widgetIndex once or twice we got a type that matches, to allow scrolling on the increase/decrease buttons too
    int32_t attempts = 0;
    while (widgetType != WWT_IMGBTN && widgetType != WWT_SPINNER && widgetIndex > 0)
    {
        switch (widgetType)
        {
            case WWT_TRNBTN: // + and - for preview widget
            case WWT_BUTTON: // + and - for spinner widget
            {
                if (attempts > 0)
                {
                    // Verify that the previous button was of the same type
                    auto previousType = w->widgets[widgetIndex + 1].type;
                    if (previousType != widgetType)
                    {
                        return false;
                    }
                }
                break;
            }
            default:
                // The widget type is not an increment or decrement button
                return false;
        }

        attempts++;
        if (attempts > 2)
        {
            // We're 2 buttons up, and no preview or spinner widget was found
            return false;
        }

        widgetIndex--;
        widgetType = w->widgets[widgetIndex].type;
    }

    rct_widgetindex buttonWidgetIndex;
    uint16_t expectedType;
    uint32_t expectedContent[2];
    switch (widgetType)
    {
        case WWT_IMGBTN:
            buttonWidgetIndex = wheel < 0 ? widgetIndex + 2 : widgetIndex + 1;
            expectedType = WWT_TRNBTN;
            expectedContent[0] = IMAGE_TYPE_REMAP | SPR_LAND_TOOL_DECREASE;
            expectedContent[1] = IMAGE_TYPE_REMAP | SPR_LAND_TOOL_INCREASE;
            break;
        case WWT_SPINNER:
            buttonWidgetIndex = wheel < 0 ? widgetIndex + 1 : widgetIndex + 2;
            expectedType = WWT_BUTTON;
            expectedContent[0] = STR_NUMERIC_UP;
            expectedContent[1] = STR_NUMERIC_DOWN;
            break;
        default: return false;
    }

    if (widget_is_disabled(w, buttonWidgetIndex))
    {
        return false;
    }

    auto button1Type = w->widgets[widgetIndex + 1].type;
    auto button1Image = w->widgets[widgetIndex + 1].image;
    auto button2Type = w->widgets[widgetIndex + 2].type;
    auto button2Image = w->widgets[widgetIndex + 2].image;
    if (button1Type != expectedType || button2Type != expectedType || button1Image != expectedContent[0]
        || button2Image != expectedContent[1])
    {
        return false;
    }

    window_event_mouse_down_call(w, buttonWidgetIndex);
    return true;
}

/**
 *
 *  rct2: 0x006E7868
 */
void window_all_wheel_input()
{
    // Get wheel value
    CursorState * cursorState = (CursorState *)context_get_cursor_state();
    int32_t absolute_wheel = cursorState->wheel;
    int32_t relative_wheel = absolute_wheel - _previousAbsoluteWheel;
    int32_t pixel_scroll = relative_wheel * WINDOW_SCROLL_PIXELS;
    _previousAbsoluteWheel = absolute_wheel;

    if (relative_wheel == 0)
        return;

    // Check window cursor is over
    if (!(input_test_flag(INPUT_FLAG_5))) {
        rct_window *w = window_find_from_point(cursorState->x, cursorState->y);
        if (w != nullptr) {
            // Check if main window
            if (w->classification == WC_MAIN_WINDOW || w->classification == WC_VIEWPORT) {
                window_viewport_wheel_input(w, relative_wheel);
                return;
            }

            // Check scroll view, cursor is over
            rct_widgetindex widgetIndex = window_find_widget_from_point(w, cursorState->x, cursorState->y);
            if (widgetIndex != -1) {
                rct_widget *widget = &w->widgets[widgetIndex];
                if (widget->type == WWT_SCROLL) {
                    int32_t scrollIndex = window_get_scroll_index(w, widgetIndex);
                    rct_scroll *scroll =  &w->scrolls[scrollIndex];
                    if (scroll->flags & (HSCROLLBAR_VISIBLE | VSCROLLBAR_VISIBLE)) {
                        window_scroll_wheel_input(w, window_get_scroll_index(w, widgetIndex), pixel_scroll);
                        return;
                    }
                } else {
                    if (window_other_wheel_input(w, widgetIndex, pixel_scroll)) {
                        return;
                    }
                }

                // Check other scroll views on window
                if (window_wheel_input(w, pixel_scroll))
                    return;
            }
        }
    }
}

/**
 * Initialises scroll widgets to their virtual size.
 *  rct2: 0x006EAEB8
 */
void window_init_scroll_widgets(rct_window *w)
{
    rct_widget* widget;
    rct_scroll* scroll;
    int32_t widget_index, scroll_index;
    int32_t width, height;

    widget_index = 0;
    scroll_index = 0;
    for (widget = w->widgets; widget->type != WWT_LAST; widget++) {
        if (widget->type != WWT_SCROLL) {
            widget_index++;
            continue;
        }

        scroll = &w->scrolls[scroll_index];
        scroll->flags = 0;
        width = 0;
        height = 0;
        window_get_scroll_size(w, scroll_index, &width, &height);
        scroll->h_left = 0;
        scroll->h_right = width + 1;
        scroll->v_top = 0;
        scroll->v_bottom = height + 1;

        if (widget->content & SCROLL_HORIZONTAL)
            scroll->flags |= HSCROLLBAR_VISIBLE;
        if (widget->content & SCROLL_VERTICAL)
            scroll->flags |= VSCROLLBAR_VISIBLE;

        widget_scroll_update_thumbs(w, widget_index);

        widget_index++;
        scroll_index++;
    }
}

/**
 *
 *  rct2: 0x006EB15C
 */
void window_draw_widgets(rct_window *w, rct_drawpixelinfo *dpi)
{
    rct_widget *widget;
    rct_widgetindex widgetIndex;

    if ((w->flags & WF_TRANSPARENT) && !(w->flags & WF_NO_BACKGROUND))
        gfx_filter_rect(dpi, w->x, w->y, w->x + w->width - 1, w->y + w->height - 1, PALETTE_51);

    //todo: some code missing here? Between 006EB18C and 006EB260

    widgetIndex = 0;
    for (widget = w->widgets; widget->type != WWT_LAST; widget++) {
        // Check if widget is outside the draw region
        if (w->x + widget->left < dpi->x + dpi->width && w->x + widget->right >= dpi->x)
            if (w->y + widget->top < dpi->y + dpi->height && w->y + widget->bottom >= dpi->y)
                widget_draw(dpi, w, widgetIndex);

        widgetIndex++;
    }

    //todo: something missing here too? Between 006EC32B and 006EC369

    if (w->flags & WF_WHITE_BORDER_MASK) {
        gfx_fill_rect_inset(dpi, w->x, w->y, w->x + w->width - 1, w->y + w->height - 1, COLOUR_WHITE, INSET_RECT_FLAG_FILL_NONE);
    }
}

/**
 *
 *  rct2: 0x006EA776
 */
static void window_invalidate_pressed_image_buttons(rct_window *w)
{
    rct_widgetindex widgetIndex;
    rct_widget *widget;

    widgetIndex = 0;
    for (widget = w->widgets; widget->type != WWT_LAST; widget++, widgetIndex++) {
        if (widget->type != WWT_IMGBTN)
            continue;

        if (widget_is_pressed(w, widgetIndex) || widget_is_active_tool(w, widgetIndex))
            gfx_set_dirty_blocks(w->x, w->y, w->x + w->width, w->y + w->height);
    }
}

/**
 *
 *  rct2: 0x006EA73F
 */
void invalidate_all_windows_after_input()
{
    for (auto& w : g_window_list)
    {
        window_update_scroll_widgets(w.get());
        window_invalidate_pressed_image_buttons(w.get());
        window_event_resize_call(w.get());
    }
}
