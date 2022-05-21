/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Window.h"

#include "../Context.h"
#include "../Editor.h"
#include "../Game.h"
#include "../Input.h"
#include "../OpenRCT2.h"
#include "../audio/audio.h"
#include "../config/Config.h"
#include "../core/Guard.hpp"
#include "../drawing/Drawing.h"
#include "../interface/Cursors.h"
#include "../localisation/Localisation.h"
#include "../localisation/StringIds.h"
#include "../platform/Platform.h"
#include "../ride/RideAudio.h"
#include "../scenario/Scenario.h"
#include "../sprites.h"
#include "../ui/UiContext.h"
#include "../ui/WindowManager.h"
#include "../world/Map.h"
#include "Viewport.h"
#include "Widget.h"
#include "Window_internal.h"

#include <algorithm>
#include <cmath>
#include <functional>
#include <iterator>
#include <list>

std::list<std::shared_ptr<rct_window>> g_window_list;
rct_window* gWindowAudioExclusive;

widget_identifier gCurrentTextBox = { { 255, 0 }, 0 };
char gTextBoxInput[TEXT_INPUT_SIZE] = { 0 };
int32_t gMaxTextBoxInputLength = 0;
int32_t gTextBoxFrameNo = 0;
bool gUsingWidgetTextBox = false;
TextInputSession* gTextInput;

uint16_t gWindowUpdateTicks;
uint16_t gWindowMapFlashingFlags;
colour_t gCurrentWindowColours[4];

// converted from uint16_t values at 0x009A41EC - 0x009A4230
// these are percentage coordinates of the viewport to centre to, if a window is obscuring a location, the next is tried
// clang-format off
static constexpr const float window_scroll_locations[][2] = {
    { 0.5f, 0.5f },
    { 0.75f, 0.5f },
    { 0.25f, 0.5f },
    { 0.5f, 0.75f },
    { 0.5f, 0.25f },
    { 0.75f, 0.75f },
    { 0.75f, 0.25f },
    { 0.25f, 0.75f },
    { 0.25f, 0.25f },
    { 0.125f, 0.5f },
    { 0.875f, 0.5f },
    { 0.5f, 0.125f },
    { 0.5f, 0.875f },
    { 0.875f, 0.125f },
    { 0.875f, 0.875f },
    { 0.125f, 0.875f },
    { 0.125f, 0.125f },
};
// clang-format on

namespace WindowCloseFlags
{
    static constexpr uint32_t None = 0;
    static constexpr uint32_t IterateReverse = (1 << 0);
    static constexpr uint32_t CloseSingle = (1 << 1);
} // namespace WindowCloseFlags

static void window_draw_core(rct_drawpixelinfo* dpi, rct_window* w, int32_t left, int32_t top, int32_t right, int32_t bottom);
static void window_draw_single(rct_drawpixelinfo* dpi, rct_window* w, int32_t left, int32_t top, int32_t right, int32_t bottom);

std::list<std::shared_ptr<rct_window>>::iterator window_get_iterator(const rct_window* w)
{
    return std::find_if(g_window_list.begin(), g_window_list.end(), [w](const std::shared_ptr<rct_window>& w2) -> bool {
        return w == w2.get();
    });
}

void window_visit_each(std::function<void(rct_window*)> func)
{
    auto windowList = g_window_list;
    for (auto& w : windowList)
    {
        func(w.get());
    }
}

/**
 *
 *  rct2: 0x006ED7B0
 */
void window_dispatch_update_all()
{
    window_visit_each([&](rct_window* w) { window_event_update_call(w); });
}

void window_update_all_viewports()
{
    window_visit_each([&](rct_window* w) {
        if (w->viewport != nullptr && window_is_visible(w))
        {
            viewport_update_position(w);
        }
    });
}

/**
 *
 *  rct2: 0x006E77A1
 */
void window_update_all()
{
    // window_update_all_viewports();

    // 1000 tick update
    gWindowUpdateTicks += gCurrentDeltaTime;
    if (gWindowUpdateTicks >= 1000)
    {
        gWindowUpdateTicks = 0;

        window_visit_each([](rct_window* w) { window_event_periodic_update_call(w); });
    }

    // Border flash invalidation
    window_visit_each([](rct_window* w) {
        if (w->flags & WF_WHITE_BORDER_MASK)
        {
            w->flags -= WF_WHITE_BORDER_ONE;
            if (!(w->flags & WF_WHITE_BORDER_MASK))
            {
                w->Invalidate();
            }
        }
    });

    auto windowManager = OpenRCT2::GetContext()->GetUiContext()->GetWindowManager();
    windowManager->UpdateMouseWheel();
}

static void window_close_surplus(int32_t cap, int8_t avoid_classification)
{
    // find the amount of windows that are currently open
    auto count = static_cast<int32_t>(g_window_list.size());
    // difference between amount open and cap = amount to close
    auto diff = count - WINDOW_LIMIT_RESERVED - cap;
    for (auto i = 0; i < diff; i++)
    {
        // iterates through the list until it finds the newest window, or a window that can be closed
        rct_window* foundW{};
        for (auto& w : g_window_list)
        {
            if (!(w->flags & (WF_STICK_TO_BACK | WF_STICK_TO_FRONT | WF_NO_AUTO_CLOSE)))
            {
                foundW = w.get();
                break;
            }
        }
        // skip window if window matches specified rct_windowclass (as user may be modifying via options)
        if (avoid_classification != -1 && foundW != nullptr && foundW->classification == avoid_classification)
        {
            continue;
        }
        window_close(foundW);
    }
}

/*
 * Changes the maximum amount of windows allowed
 */
void window_set_window_limit(int32_t value)
{
    int32_t prev = gConfigGeneral.window_limit;
    int32_t val = std::clamp(value, WINDOW_LIMIT_MIN, WINDOW_LIMIT_MAX);
    gConfigGeneral.window_limit = val;
    config_save_default();
    // Checks if value decreases and then closes surplus
    // windows if one sets a limit lower than the number of windows open
    if (val < prev)
    {
        window_close_surplus(val, WC_OPTIONS);
    }
}

/**
 * Closes the specified window.
 *  rct2: 0x006ECD4C
 *
 * @param window The window to close (esi).
 */
void window_close(rct_window* w)
{
    auto itWindow = window_get_iterator(w);
    if (itWindow == g_window_list.end())
        return;

    // Explicit copy of the shared ptr to keep the memory valid.
    std::shared_ptr<rct_window> window = *itWindow;

    window_event_close_call(window.get());

    // Remove viewport
    window->RemoveViewport();

    // Invalidate the window (area)
    window->Invalidate();

    // The window list may have been modified in the close event
    itWindow = window_get_iterator(w);
    if (itWindow != g_window_list.end())
        g_window_list.erase(itWindow);
}

template<typename TPred> static void window_close_by_condition(TPred pred, uint32_t flags = WindowCloseFlags::None)
{
    bool listUpdated;
    do
    {
        listUpdated = false;

        auto closeSingle = [&](std::shared_ptr<rct_window> window) -> bool {
            if (!pred(window.get()))
            {
                return false;
            }

            // Keep track of current amount, if a new window is created upon closing
            // we need to break this current iteration and restart.
            size_t previousCount = g_window_list.size();

            window_close(window.get());

            if ((flags & WindowCloseFlags::CloseSingle) != 0)
            {
                // Only close a single one.
                return true;
            }

            if (previousCount >= g_window_list.size())
            {
                // A new window was created during the close event.
                return true;
            }

            // Keep closing windows.
            return false;
        };

        // The closest to something like for_each_if is using find_if in order to avoid duplicate code
        // to change the loop direction.
        auto windowList = g_window_list;
        if ((flags & WindowCloseFlags::IterateReverse) != 0)
            listUpdated = std::find_if(windowList.rbegin(), windowList.rend(), closeSingle) != windowList.rend();
        else
            listUpdated = std::find_if(windowList.begin(), windowList.end(), closeSingle) != windowList.end();

        // If requested to close only a single window and a new window was created during close
        // we ignore it.
        if ((flags & WindowCloseFlags::CloseSingle) != 0)
            break;

    } while (listUpdated);
}

/**
 * Closes all windows with the specified window class.
 *  rct2: 0x006ECCF4
 * @param cls (cl) with bit 15 set
 */
void window_close_by_class(rct_windowclass cls)
{
    window_close_by_condition([&](rct_window* w) -> bool { return w->classification == cls; });
}

/**
 * Closes all windows with specified window class and number.
 *  rct2: 0x006ECCF4
 * @param cls (cl) without bit 15 set
 * @param number (dx)
 */
void window_close_by_number(rct_windowclass cls, rct_windownumber number)
{
    window_close_by_condition([cls, number](rct_window* w) -> bool { return w->classification == cls && w->number == number; });
}

// TODO: Refactor this to use variant once the new window class is done.
void window_close_by_number(rct_windowclass cls, EntityId number)
{
    window_close_by_number(cls, static_cast<rct_windownumber>(number.ToUnderlying()));
}

/**
 * Finds the first window with the specified window class.
 *  rct2: 0x006EA8A0
 * @param cls (cl) with bit 15 set
 * @returns the window or NULL if no window was found.
 */
rct_window* window_find_by_class(rct_windowclass cls)
{
    for (auto& w : g_window_list)
    {
        if (w->classification == cls)
        {
            return w.get();
        }
    }
    return nullptr;
}

/**
 * Finds the first window with the specified window class and number.
 *  rct2: 0x006EA8A0
 * @param cls (cl) without bit 15 set
 * @param number (dx)
 * @returns the window or NULL if no window was found.
 */
rct_window* window_find_by_number(rct_windowclass cls, rct_windownumber number)
{
    for (auto& w : g_window_list)
    {
        if (w->classification == cls && w->number == number)
        {
            return w.get();
        }
    }
    return nullptr;
}

// TODO: Use variant for this once the window framework is done.
rct_window* window_find_by_number(rct_windowclass cls, EntityId id)
{
    return window_find_by_number(cls, static_cast<rct_windownumber>(id.ToUnderlying()));
}

/**
 * Closes the top-most window
 *
 *  rct2: 0x006E403C
 */
void window_close_top()
{
    window_close_by_class(WC_DROPDOWN);

    if (gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR)
    {
        if (gEditorStep != EditorStep::LandscapeEditor)
            return;
    }

    auto pred = [](rct_window* w) -> bool { return !(w->flags & (WF_STICK_TO_BACK | WF_STICK_TO_FRONT)); };
    window_close_by_condition(pred, WindowCloseFlags::CloseSingle | WindowCloseFlags::IterateReverse);
}

/**
 * Closes all open windows
 *
 *  rct2: 0x006EE927
 */
void window_close_all()
{
    window_close_by_class(WC_DROPDOWN);
    window_close_by_condition([](rct_window* w) -> bool { return !(w->flags & (WF_STICK_TO_BACK | WF_STICK_TO_FRONT)); });
}

void window_close_all_except_class(rct_windowclass cls)
{
    window_close_by_class(WC_DROPDOWN);

    window_close_by_condition([cls](rct_window* w) -> bool {
        return w->classification != cls && !(w->flags & (WF_STICK_TO_BACK | WF_STICK_TO_FRONT));
    });
}

/**
 * Closes all windows, save for those having any of the passed flags.
 */
void window_close_all_except_flags(uint16_t flags)
{
    window_close_by_condition([flags](rct_window* w) -> bool { return !(w->flags & flags); });
}

/**
 *
 *  rct2: 0x006EA845
 */
rct_window* window_find_from_point(const ScreenCoordsXY& screenCoords)
{
    for (auto it = g_window_list.rbegin(); it != g_window_list.rend(); it++)
    {
        auto& w = *it;
        if (screenCoords.x < w->windowPos.x || screenCoords.x >= w->windowPos.x + w->width || screenCoords.y < w->windowPos.y
            || screenCoords.y >= w->windowPos.y + w->height)
            continue;

        if (w->flags & WF_NO_BACKGROUND)
        {
            auto widgetIndex = window_find_widget_from_point(w.get(), screenCoords);
            if (widgetIndex == -1)
                continue;
        }

        return w.get();
    }

    return nullptr;
}

/**
 *
 *  rct2: 0x006EA594
 * x (ax)
 * y (bx)
 * returns widget_index (edx)
 * EDI NEEDS TO BE SET TO w->widgets[widget_index] AFTER
 */
rct_widgetindex window_find_widget_from_point(rct_window* w, const ScreenCoordsXY& screenCoords)
{
    // Invalidate the window
    window_event_invalidate_call(w);

    // Find the widget at point x, y
    rct_widgetindex widget_index = -1;
    for (int32_t i = 0;; i++)
    {
        const auto& widget = w->widgets[i];
        if (widget.type == WindowWidgetType::Last)
        {
            break;
        }

        if (widget.type != WindowWidgetType::Empty && widget.IsVisible())
        {
            if (screenCoords.x >= w->windowPos.x + widget.left && screenCoords.x <= w->windowPos.x + widget.right
                && screenCoords.y >= w->windowPos.y + widget.top && screenCoords.y <= w->windowPos.y + widget.bottom)
            {
                widget_index = i;
            }
        }
    }

    // Return next widget if a dropdown
    if (widget_index != -1)
    {
        const auto& widget = w->widgets[widget_index];
        if (widget.type == WindowWidgetType::DropdownMenu)
            widget_index++;
    }

    // Return the widget index
    return widget_index;
}

/**
 * Invalidates the specified window.
 *  rct2: 0x006EB13A
 *
 * @param window The window to invalidate (esi).
 */
template<typename TPred> static void window_invalidate_by_condition(TPred pred)
{
    window_visit_each([pred](rct_window* w) {
        if (pred(w))
        {
            w->Invalidate();
        }
    });
}

/**
 * Invalidates all windows with the specified window class.
 *  rct2: 0x006EC3AC
 * @param cls (al) with bit 14 set
 */
void window_invalidate_by_class(rct_windowclass cls)
{
    window_invalidate_by_condition([cls](rct_window* w) -> bool { return w->classification == cls; });
}

/**
 * Invalidates all windows with the specified window class and number.
 *  rct2: 0x006EC3AC
 */
void window_invalidate_by_number(rct_windowclass cls, rct_windownumber number)
{
    window_invalidate_by_condition(
        [cls, number](rct_window* w) -> bool { return w->classification == cls && w->number == number; });
}

// TODO: Use variant for this once the window framework is done.
void window_invalidate_by_number(rct_windowclass cls, EntityId id)
{
    window_invalidate_by_number(cls, static_cast<rct_windownumber>(id.ToUnderlying()));
}

/**
 * Invalidates all windows.
 */
void window_invalidate_all()
{
    window_visit_each([](rct_window* w) { w->Invalidate(); });
}

/**
 * Invalidates the specified widget of a window.
 *  rct2: 0x006EC402
 */
void widget_invalidate(rct_window* w, rct_widgetindex widgetIndex)
{
    assert(w != nullptr);
#ifdef DEBUG
    for (int32_t i = 0; i <= widgetIndex; i++)
    {
        assert(w->widgets[i].type != WindowWidgetType::Last);
    }
#endif

    const auto& widget = w->widgets[widgetIndex];
    if (widget.left == -2)
        return;

    gfx_set_dirty_blocks({ { w->windowPos + ScreenCoordsXY{ widget.left, widget.top } },
                           { w->windowPos + ScreenCoordsXY{ widget.right + 1, widget.bottom + 1 } } });
}

template<typename TPred> static void widget_invalidate_by_condition(TPred pred)
{
    window_visit_each([pred](rct_window* w) {
        if (pred(w))
        {
            w->Invalidate();
        }
    });
}

/**
 * Invalidates the specified widget of all windows that match the specified window class.
 */
void widget_invalidate_by_class(rct_windowclass cls, rct_widgetindex widgetIndex)
{
    window_visit_each([cls, widgetIndex](rct_window* w) {
        if (w->classification == cls)
        {
            widget_invalidate(w, widgetIndex);
        }
    });
}

/**
 * Invalidates the specified widget of all windows that match the specified window class and number.
 *  rct2: 0x006EC3AC
 */
void widget_invalidate_by_number(rct_windowclass cls, rct_windownumber number, rct_widgetindex widgetIndex)
{
    window_visit_each([cls, number, widgetIndex](rct_window* w) {
        if (w->classification == cls && w->number == number)
        {
            widget_invalidate(w, widgetIndex);
        }
    });
}

/**
 *
 *  rct2: 0x006EAE4E
 *
 * @param w The window (esi).
 */
void window_update_scroll_widgets(rct_window* w)
{
    int32_t scrollIndex, width, height, scrollPositionChanged;
    rct_widgetindex widgetIndex;
    rct_widget* widget;

    widgetIndex = 0;
    scrollIndex = 0;
    assert(w != nullptr);
    for (widget = w->widgets; widget->type != WindowWidgetType::Last; widget++, widgetIndex++)
    {
        if (widget->type != WindowWidgetType::Scroll)
            continue;

        auto& scroll = w->scrolls[scrollIndex];
        width = 0;
        height = 0;
        window_get_scroll_size(w, scrollIndex, &width, &height);
        if (height == 0)
        {
            scroll.v_top = 0;
        }
        else if (width == 0)
        {
            scroll.h_left = 0;
        }
        width++;
        height++;

        scrollPositionChanged = 0;
        if ((widget->content & SCROLL_HORIZONTAL) && width != scroll.h_right)
        {
            scrollPositionChanged = 1;
            scroll.h_right = width;
        }

        if ((widget->content & SCROLL_VERTICAL) && height != scroll.v_bottom)
        {
            scrollPositionChanged = 1;
            scroll.v_bottom = height;
        }

        if (scrollPositionChanged)
        {
            WidgetScrollUpdateThumbs(w, widgetIndex);
            w->Invalidate();
        }
        scrollIndex++;
    }
}

int32_t window_get_scroll_data_index(rct_window* w, rct_widgetindex widget_index)
{
    int32_t i, result;

    result = 0;
    assert(w != nullptr);
    for (i = 0; i < widget_index; i++)
    {
        const auto& widget = w->widgets[i];
        if (widget.type == WindowWidgetType::Scroll)
            result++;
    }
    return result;
}

/**
 *
 *  rct2: 0x006ECDA4
 */
rct_window* window_bring_to_front(rct_window* w)
{
    if (!(w->flags & (WF_STICK_TO_BACK | WF_STICK_TO_FRONT)))
    {
        auto itSourcePos = window_get_iterator(w);
        if (itSourcePos != g_window_list.end())
        {
            // Insert in front of the first non-stick-to-front window
            auto itDestPos = g_window_list.begin();
            for (auto it = g_window_list.rbegin(); it != g_window_list.rend(); it++)
            {
                auto& w2 = *it;
                if (!(w2->flags & WF_STICK_TO_FRONT))
                {
                    itDestPos = it.base();
                    break;
                }
            }

            g_window_list.splice(itDestPos, g_window_list, itSourcePos);
            w->Invalidate();

            if (w->windowPos.x + w->width < 20)
            {
                int32_t i = 20 - w->windowPos.x;
                w->windowPos.x += i;
                if (w->viewport != nullptr)
                    w->viewport->pos.x += i;
                w->Invalidate();
            }
        }
    }
    return w;
}

rct_window* window_bring_to_front_by_class_with_flags(rct_windowclass cls, uint16_t flags)
{
    rct_window* w;

    w = window_find_by_class(cls);
    if (w != nullptr)
    {
        w->flags |= flags;
        w->Invalidate();
        w = window_bring_to_front(w);
    }

    return w;
}

rct_window* window_bring_to_front_by_class(rct_windowclass cls)
{
    return window_bring_to_front_by_class_with_flags(cls, WF_WHITE_BORDER_MASK);
}

/**
 *
 *  rct2: 0x006ED78A
 * cls (cl)
 * number (dx)
 */
rct_window* window_bring_to_front_by_number(rct_windowclass cls, rct_windownumber number)
{
    rct_window* w;

    w = window_find_by_number(cls, number);
    if (w != nullptr)
    {
        w->flags |= WF_WHITE_BORDER_MASK;
        w->Invalidate();
        w = window_bring_to_front(w);
    }

    return w;
}

/**
 *
 *  rct2: 0x006EE65A
 */
void window_push_others_right(rct_window* window)
{
    window_visit_each([window](rct_window* w) {
        if (w == window)
            return;
        if (w->flags & (WF_STICK_TO_BACK | WF_STICK_TO_FRONT))
            return;
        if (w->windowPos.x >= window->windowPos.x + window->width)
            return;
        if (w->windowPos.x + w->width <= window->windowPos.x)
            return;
        if (w->windowPos.y >= window->windowPos.y + window->height)
            return;
        if (w->windowPos.y + w->height <= window->windowPos.y)
            return;

        w->Invalidate();
        if (window->windowPos.x + window->width + 13 >= context_get_width())
            return;
        auto push_amount = window->windowPos.x + window->width - w->windowPos.x + 3;
        w->windowPos.x += push_amount;
        w->Invalidate();
        if (w->viewport != nullptr)
            w->viewport->pos.x += push_amount;
    });
}

/**
 *
 *  rct2: 0x006EE6EA
 */
void window_push_others_below(rct_window* w1)
{
    // Enumerate through all other windows
    window_visit_each([w1](rct_window* w2) {
        if (w1 == w2)
            return;
        // ?
        if (w2->flags & (WF_STICK_TO_BACK | WF_STICK_TO_FRONT))
            return;
        // Check if w2 intersects with w1
        if (w2->windowPos.x > (w1->windowPos.x + w1->width) || w2->windowPos.x + w2->width < w1->windowPos.x)
            return;
        if (w2->windowPos.y > (w1->windowPos.y + w1->height) || w2->windowPos.y + w2->height < w1->windowPos.y)
            return;

        // Check if there is room to push it down
        if (w1->windowPos.y + w1->height + 80 >= context_get_height())
            return;

        // Invalidate the window's current area
        w2->Invalidate();

        int32_t push_amount = w1->windowPos.y + w1->height - w2->windowPos.y + 3;
        w2->windowPos.y += push_amount;

        // Invalidate the window's new area
        w2->Invalidate();

        // Update viewport position if necessary
        if (w2->viewport != nullptr)
            w2->viewport->pos.y += push_amount;
    });
}

/**
 *
 *  rct2: 0x006EE2E4
 */
rct_window* window_get_main()
{
    for (auto& w : g_window_list)
    {
        if (w->classification == WC_MAIN_WINDOW)
        {
            return w.get();
        }
    }
    return nullptr;
}

/**
 *
 *  rct2: 0x006E7C9C
 * @param w (esi)
 * @param x (eax)
 * @param y (ecx)
 * @param z (edx)
 */
void window_scroll_to_location(rct_window* w, const CoordsXYZ& coords)
{
    assert(w != nullptr);
    window_unfollow_sprite(w);
    if (w->viewport != nullptr)
    {
        int16_t height = tile_element_height(coords);
        if (coords.z < height - 16)
        {
            if (!(w->viewport->flags & 1 << 0))
            {
                w->viewport->flags |= 1 << 0;
                w->Invalidate();
            }
        }
        else
        {
            if (w->viewport->flags & 1 << 0)
            {
                w->viewport->flags &= ~(1 << 0);
                w->Invalidate();
            }
        }

        auto screenCoords = translate_3d_to_2d_with_z(get_current_rotation(), coords);

        int32_t i = 0;
        if (!(gScreenFlags & SCREEN_FLAGS_TITLE_DEMO))
        {
            bool found = false;
            while (!found)
            {
                auto x2 = w->viewport->pos.x + static_cast<int32_t>(w->viewport->width * window_scroll_locations[i][0]);
                auto y2 = w->viewport->pos.y + static_cast<int32_t>(w->viewport->height * window_scroll_locations[i][1]);

                auto it = window_get_iterator(w);
                for (; it != g_window_list.end(); it++)
                {
                    auto w2 = (*it).get();
                    auto x1 = w2->windowPos.x - 10;
                    auto y1 = w2->windowPos.y - 10;
                    if (x2 >= x1 && x2 <= w2->width + x1 + 20)
                    {
                        if (y2 >= y1 && y2 <= w2->height + y1 + 20)
                        {
                            // window is covering this area, try the next one
                            i++;
                            found = false;
                            break;
                        }
                    }
                }
                if (it == g_window_list.end())
                {
                    found = true;
                }
                if (i >= static_cast<int32_t>(std::size(window_scroll_locations)))
                {
                    i = 0;
                    found = true;
                }
            }
        }
        // rct2: 0x006E7C76
        if (w->viewport_target_sprite.IsNull())
        {
            if (!(w->flags & WF_NO_SCROLLING))
            {
                w->savedViewPos = screenCoords
                    - ScreenCoordsXY{ static_cast<int32_t>(w->viewport->view_width * window_scroll_locations[i][0]),
                                      static_cast<int32_t>(w->viewport->view_height * window_scroll_locations[i][1]) };
                w->flags |= WF_SCROLLING_TO_LOCATION;
            }
        }
    }
}

/**
 *
 *  rct2: 0x00688956
 */
static void call_event_viewport_rotate_on_all_windows()
{
    window_visit_each([](rct_window* w) { window_event_viewport_rotate_call(w); });
}

/**
 *
 *  rct2: 0x0068881A
 * direction can be used to alter the camera rotation:
 *      1: clockwise
 *      -1: anti-clockwise
 */
void window_rotate_camera(rct_window* w, int32_t direction)
{
    rct_viewport* viewport = w->viewport;
    if (viewport == nullptr)
        return;

    auto windowPos = ScreenCoordsXY{ (viewport->width >> 1), (viewport->height >> 1) } + viewport->pos;

    // has something to do with checking if middle of the viewport is obstructed
    rct_viewport* other;
    auto mapXYCoords = screen_get_map_xy(windowPos, &other);
    CoordsXYZ coords{};

    // other != viewport probably triggers on viewports in ride or guest window?
    // naoXYCoords is nullopt if middle of viewport is obstructed by another window?
    if (!mapXYCoords.has_value() || other != viewport)
    {
        auto viewPos = ScreenCoordsXY{ (viewport->view_width >> 1), (viewport->view_height >> 1) } + viewport->viewPos;

        coords = viewport_adjust_for_map_height(viewPos);
    }
    else
    {
        coords.x = mapXYCoords->x;
        coords.y = mapXYCoords->y;
        coords.z = tile_element_height(coords);
    }

    gCurrentRotation = (get_current_rotation() + direction) & 3;

    auto centreLoc = centre_2d_coordinates(coords, viewport);

    if (centreLoc.has_value())
    {
        w->savedViewPos = centreLoc.value();
        viewport->viewPos = *centreLoc;
    }

    w->Invalidate();

    call_event_viewport_rotate_on_all_windows();
    reset_all_sprite_quadrant_placements();
}

void window_viewport_get_map_coords_by_cursor(
    rct_window* w, int32_t* map_x, int32_t* map_y, int32_t* offset_x, int32_t* offset_y)
{
    // Get mouse position to offset against.
    auto mouseCoords = context_get_cursor_position_scaled();

    // Compute map coordinate by mouse position.
    auto viewportPos = w->viewport->ScreenToViewportCoord(mouseCoords);
    auto coordsXYZ = viewport_adjust_for_map_height(viewportPos);
    auto mapCoords = viewport_coord_to_map_coord(viewportPos, coordsXYZ.z);
    *map_x = mapCoords.x;
    *map_y = mapCoords.y;

    // Get viewport coordinates centring around the tile.
    int32_t z = tile_element_height(mapCoords);

    auto centreLoc = centre_2d_coordinates({ mapCoords.x, mapCoords.y, z }, w->viewport);
    if (!centreLoc)
    {
        log_error("Invalid location.");
        return;
    }

    // Rebase mouse position onto centre of window, and compensate for zoom level.
    int32_t rebased_x = w->viewport->zoom.ApplyTo(w->width / 2 - mouseCoords.x);
    int32_t rebased_y = w->viewport->zoom.ApplyTo(w->height / 2 - mouseCoords.y);

    // Compute cursor offset relative to tile.
    *offset_x = w->viewport->zoom.ApplyTo(w->savedViewPos.x - (centreLoc->x + rebased_x));
    *offset_y = w->viewport->zoom.ApplyTo(w->savedViewPos.y - (centreLoc->y + rebased_y));
}

void window_viewport_centre_tile_around_cursor(rct_window* w, int32_t map_x, int32_t map_y, int32_t offset_x, int32_t offset_y)
{
    // Get viewport coordinates centring around the tile.
    int32_t z = tile_element_height({ map_x, map_y });
    auto centreLoc = centre_2d_coordinates({ map_x, map_y, z }, w->viewport);

    if (!centreLoc.has_value())
    {
        log_error("Invalid location.");
        return;
    }

    // Get mouse position to offset against.
    auto mouseCoords = context_get_cursor_position_scaled();

    // Rebase mouse position onto centre of window, and compensate for zoom level.
    int32_t rebased_x = w->viewport->zoom.ApplyTo((w->width >> 1) - mouseCoords.x);
    int32_t rebased_y = w->viewport->zoom.ApplyTo((w->height >> 1) - mouseCoords.y);

    // Apply offset to the viewport.
    w->savedViewPos = { centreLoc->x + rebased_x + w->viewport->zoom.ApplyInversedTo(offset_x),
                        centreLoc->y + rebased_y + w->viewport->zoom.ApplyInversedTo(offset_y) };
}

/**
 * For all windows with viewports, ensure they do not have a zoom level less than the minimum.
 */
void window_check_all_valid_zoom()
{
    window_visit_each([](rct_window* w) {
        if (w->viewport != nullptr && w->viewport->zoom < ZoomLevel::min())
        {
            window_zoom_set(w, ZoomLevel::min(), false);
        }
    });
}

void window_zoom_set(rct_window* w, ZoomLevel zoomLevel, bool atCursor)
{
    rct_viewport* v = w->viewport;
    if (v == nullptr)
        return;

    zoomLevel = std::clamp(zoomLevel, ZoomLevel::min(), ZoomLevel::max());
    if (v->zoom == zoomLevel)
        return;

    // Zooming to cursor? Remember where we're pointing at the moment.
    int32_t saved_map_x = 0;
    int32_t saved_map_y = 0;
    int32_t offset_x = 0;
    int32_t offset_y = 0;
    if (gConfigGeneral.zoom_to_cursor && atCursor)
    {
        window_viewport_get_map_coords_by_cursor(w, &saved_map_x, &saved_map_y, &offset_x, &offset_y);
    }

    // Zoom in
    while (v->zoom > zoomLevel)
    {
        v->zoom--;
        w->savedViewPos.x += v->view_width / 4;
        w->savedViewPos.y += v->view_height / 4;
        v->view_width /= 2;
        v->view_height /= 2;
    }

    // Zoom out
    while (v->zoom < zoomLevel)
    {
        v->zoom++;
        w->savedViewPos.x -= v->view_width / 2;
        w->savedViewPos.y -= v->view_height / 2;
        v->view_width *= 2;
        v->view_height *= 2;
    }

    // Zooming to cursor? Centre around the tile we were hovering over just now.
    if (gConfigGeneral.zoom_to_cursor && atCursor)
    {
        window_viewport_centre_tile_around_cursor(w, saved_map_x, saved_map_y, offset_x, offset_y);
    }

    // HACK: Prevents the redraw from failing when there is
    // a window on top of the viewport.
    window_bring_to_front(w);
    w->Invalidate();
}

/**
 *
 *  rct2: 0x006887A6
 */
void window_zoom_in(rct_window* w, bool atCursor)
{
    window_zoom_set(w, w->viewport->zoom - 1, atCursor);
}

/**
 *
 *  rct2: 0x006887E0
 */
void window_zoom_out(rct_window* w, bool atCursor)
{
    window_zoom_set(w, w->viewport->zoom + 1, atCursor);
}

void main_window_zoom(bool zoomIn, bool atCursor)
{
    auto* mainWindow = window_get_main();
    if (mainWindow == nullptr)
        return;

    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    if (gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR && gEditorStep != EditorStep::LandscapeEditor)
        return;

    if (gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER)
        return;

    if (zoomIn)
        window_zoom_in(mainWindow, atCursor);
    else
        window_zoom_out(mainWindow, atCursor);
}

/**
 * Splits a drawing of a window into regions that can be seen and are not hidden
 * by other opaque overlapping windows.
 */
void window_draw(rct_drawpixelinfo* dpi, rct_window* w, int32_t left, int32_t top, int32_t right, int32_t bottom)
{
    if (!window_is_visible(w))
        return;

    // Divide the draws up for only the visible regions of the window recursively
    auto itPos = window_get_iterator(w);
    for (auto it = std::next(itPos); it != g_window_list.end(); it++)
    {
        // Check if this window overlaps w
        auto topwindow = it->get();
        if (topwindow->windowPos.x >= right || topwindow->windowPos.y >= bottom)
            continue;
        if (topwindow->windowPos.x + topwindow->width <= left || topwindow->windowPos.y + topwindow->height <= top)
            continue;
        if (topwindow->flags & WF_TRANSPARENT)
            continue;

        // A window overlaps w, split up the draw into two regions where the window starts to overlap
        if (topwindow->windowPos.x > left)
        {
            // Split draw at topwindow.left
            window_draw_core(dpi, w, left, top, topwindow->windowPos.x, bottom);
            window_draw_core(dpi, w, topwindow->windowPos.x, top, right, bottom);
        }
        else if (topwindow->windowPos.x + topwindow->width < right)
        {
            // Split draw at topwindow.right
            window_draw_core(dpi, w, left, top, topwindow->windowPos.x + topwindow->width, bottom);
            window_draw_core(dpi, w, topwindow->windowPos.x + topwindow->width, top, right, bottom);
        }
        else if (topwindow->windowPos.y > top)
        {
            // Split draw at topwindow.top
            window_draw_core(dpi, w, left, top, right, topwindow->windowPos.y);
            window_draw_core(dpi, w, left, topwindow->windowPos.y, right, bottom);
        }
        else if (topwindow->windowPos.y + topwindow->height < bottom)
        {
            // Split draw at topwindow.bottom
            window_draw_core(dpi, w, left, top, right, topwindow->windowPos.y + topwindow->height);
            window_draw_core(dpi, w, left, topwindow->windowPos.y + topwindow->height, right, bottom);
        }

        // Drawing for this region should be done now, exit
        return;
    }

    // No windows overlap
    window_draw_core(dpi, w, left, top, right, bottom);
}

/**
 * Draws the given window and any other overlapping transparent windows.
 */
static void window_draw_core(rct_drawpixelinfo* dpi, rct_window* w, int32_t left, int32_t top, int32_t right, int32_t bottom)
{
    // Clamp region
    left = std::max<int32_t>(left, w->windowPos.x);
    top = std::max<int32_t>(top, w->windowPos.y);
    right = std::min<int32_t>(right, w->windowPos.x + w->width);
    bottom = std::min<int32_t>(bottom, w->windowPos.y + w->height);
    if (left >= right)
        return;
    if (top >= bottom)
        return;

    // Draw the window and any other overlapping transparent windows
    for (auto it = window_get_iterator(w); it != g_window_list.end(); it++)
    {
        auto v = (*it).get();
        if ((w == v || (v->flags & WF_TRANSPARENT)) && window_is_visible(v))
        {
            window_draw_single(dpi, v, left, top, right, bottom);
        }
    }
}

static void window_draw_single(rct_drawpixelinfo* dpi, rct_window* w, int32_t left, int32_t top, int32_t right, int32_t bottom)
{
    // Copy dpi so we can crop it
    rct_drawpixelinfo copy = *dpi;
    dpi = &copy;

    // Clamp left to 0
    int32_t overflow = left - dpi->x;
    if (overflow > 0)
    {
        dpi->x += overflow;
        dpi->width -= overflow;
        if (dpi->width <= 0)
            return;
        dpi->pitch += overflow;
        dpi->bits += overflow;
    }

    // Clamp width to right
    overflow = dpi->x + dpi->width - right;
    if (overflow > 0)
    {
        dpi->width -= overflow;
        if (dpi->width <= 0)
            return;
        dpi->pitch += overflow;
    }

    // Clamp top to 0
    overflow = top - dpi->y;
    if (overflow > 0)
    {
        dpi->y += overflow;
        dpi->height -= overflow;
        if (dpi->height <= 0)
            return;
        dpi->bits += (dpi->width + dpi->pitch) * overflow;
    }

    // Clamp height to bottom
    overflow = dpi->y + dpi->height - bottom;
    if (overflow > 0)
    {
        dpi->height -= overflow;
        if (dpi->height <= 0)
            return;
    }

    // Invalidate modifies the window colours so first get the correct
    // colour before setting the global variables for the string painting
    window_event_invalidate_call(w);

    // Text colouring
    gCurrentWindowColours[0] = NOT_TRANSLUCENT(w->colours[0]);
    gCurrentWindowColours[1] = NOT_TRANSLUCENT(w->colours[1]);
    gCurrentWindowColours[2] = NOT_TRANSLUCENT(w->colours[2]);
    gCurrentWindowColours[3] = NOT_TRANSLUCENT(w->colours[3]);

    window_event_paint_call(w, dpi);
}

/**
 *
 *  rct2: 0x00685BE1
 *
 * @param dpi (edi)
 * @param w (esi)
 */
void window_draw_viewport(rct_drawpixelinfo* dpi, rct_window* w)
{
    viewport_render(dpi, w->viewport, { { dpi->x, dpi->y }, { dpi->x + dpi->width, dpi->y + dpi->height } });
}

void window_set_position(rct_window* w, const ScreenCoordsXY& screenCoords)
{
    window_move_position(w, screenCoords - w->windowPos);
}

void window_move_position(rct_window* w, const ScreenCoordsXY& deltaCoords)
{
    if (deltaCoords.x == 0 && deltaCoords.y == 0)
        return;

    // Invalidate old region
    w->Invalidate();

    // Translate window and viewport
    w->windowPos += deltaCoords;
    if (w->viewport != nullptr)
    {
        w->viewport->pos += deltaCoords;
    }

    // Invalidate new region
    w->Invalidate();
}

void window_resize(rct_window* w, int32_t dw, int32_t dh)
{
    if (dw == 0 && dh == 0)
        return;

    // Invalidate old region
    w->Invalidate();

    // Clamp new size to minimum and maximum
    w->width = std::clamp<int32_t>(w->width + dw, w->min_width, w->max_width);
    w->height = std::clamp<int32_t>(w->height + dh, w->min_height, w->max_height);

    window_event_resize_call(w);
    window_event_invalidate_call(w);

    // Update scroll widgets
    for (int32_t i = 0; i < 3; i++)
    {
        auto& scroll = w->scrolls[i];
        scroll.h_right = WINDOW_SCROLL_UNDEFINED;
        scroll.v_bottom = WINDOW_SCROLL_UNDEFINED;
    }
    window_update_scroll_widgets(w);

    // Invalidate new region
    w->Invalidate();
}

void window_set_resize(rct_window* w, int32_t minWidth, int32_t minHeight, int32_t maxWidth, int32_t maxHeight)
{
    w->min_width = minWidth;
    w->min_height = minHeight;
    w->max_width = maxWidth;
    w->max_height = maxHeight;

    // Clamp width and height to minimum and maximum
    int32_t width = std::clamp<int32_t>(w->width, std::min(minWidth, maxWidth), std::max(minWidth, maxWidth));
    int32_t height = std::clamp<int32_t>(w->height, std::min(minHeight, maxHeight), std::max(minHeight, maxHeight));

    // Resize window if size has changed
    if (w->width != width || w->height != height)
    {
        w->Invalidate();
        w->width = width;
        w->height = height;
        w->Invalidate();
    }
}

/**
 *
 *  rct2: 0x006EE212
 *
 * @param tool (al)
 * @param widgetIndex (dx)
 * @param w (esi)
 */
bool tool_set(rct_window* w, rct_widgetindex widgetIndex, Tool tool)
{
    if (input_test_flag(INPUT_FLAG_TOOL_ACTIVE))
    {
        if (w->classification == gCurrentToolWidget.window_classification && w->number == gCurrentToolWidget.window_number
            && widgetIndex == gCurrentToolWidget.widget_index)
        {
            tool_cancel();
            return true;
        }

        tool_cancel();
    }

    input_set_flag(INPUT_FLAG_TOOL_ACTIVE, true);
    input_set_flag(INPUT_FLAG_6, false);
    gCurrentToolId = tool;
    gCurrentToolWidget.window_classification = w->classification;
    gCurrentToolWidget.window_number = w->number;
    gCurrentToolWidget.widget_index = widgetIndex;
    return false;
}

/**
 *
 *  rct2: 0x006EE281
 */
void tool_cancel()
{
    if (input_test_flag(INPUT_FLAG_TOOL_ACTIVE))
    {
        input_set_flag(INPUT_FLAG_TOOL_ACTIVE, false);

        map_invalidate_selection_rect();
        map_invalidate_map_selection_tiles();

        // Reset map selection
        gMapSelectFlags = 0;

        if (gCurrentToolWidget.widget_index != -1)
        {
            // Invalidate tool widget
            widget_invalidate_by_number(
                gCurrentToolWidget.window_classification, gCurrentToolWidget.window_number, gCurrentToolWidget.widget_index);

            // Abort tool event
            rct_window* w = window_find_by_number(gCurrentToolWidget.window_classification, gCurrentToolWidget.window_number);
            if (w != nullptr)
                window_event_tool_abort_call(w, gCurrentToolWidget.widget_index);
        }
    }
}

void window_event_close_call(rct_window* w)
{
    if (w->event_handlers == nullptr)
        w->OnClose();
    else if (w->event_handlers->close != nullptr)
        w->event_handlers->close(w);
}

void window_event_mouse_up_call(rct_window* w, rct_widgetindex widgetIndex)
{
    if (w->event_handlers == nullptr)
        w->OnMouseUp(widgetIndex);
    else if (w->event_handlers->mouse_up != nullptr)
        w->event_handlers->mouse_up(w, widgetIndex);
}

void window_event_resize_call(rct_window* w)
{
    if (w->event_handlers == nullptr)
        w->OnResize();
    else if (w->event_handlers->resize != nullptr)
        w->event_handlers->resize(w);
}

void window_event_mouse_down_call(rct_window* w, rct_widgetindex widgetIndex)
{
    if (w->event_handlers == nullptr)
        w->OnMouseDown(widgetIndex);
    else if (w->event_handlers->mouse_down != nullptr)
        w->event_handlers->mouse_down(w, widgetIndex, &w->widgets[widgetIndex]);
}

void window_event_dropdown_call(rct_window* w, rct_widgetindex widgetIndex, int32_t dropdownIndex)
{
    if (w->event_handlers == nullptr)
    {
        if (dropdownIndex != -1)
        {
            w->OnDropdown(widgetIndex, dropdownIndex);
        }
    }
    else if (w->event_handlers->dropdown != nullptr)
    {
        w->event_handlers->dropdown(w, widgetIndex, dropdownIndex);
    }
}

void window_event_unknown_05_call(rct_window* w)
{
    if (w->event_handlers != nullptr)
        if (w->event_handlers->unknown_05 != nullptr)
            w->event_handlers->unknown_05(w);
}

void window_event_update_call(rct_window* w)
{
    if (w->event_handlers == nullptr)
        w->OnUpdate();
    else if (w->event_handlers->update != nullptr)
        w->event_handlers->update(w);
}

void window_event_periodic_update_call(rct_window* w)
{
    if (w->event_handlers == nullptr)
        w->OnPeriodicUpdate();
    else if (w->event_handlers->periodic_update != nullptr)
        w->event_handlers->periodic_update(w);
}

void window_event_unknown_08_call(rct_window* w)
{
    if (w->event_handlers != nullptr)
        if (w->event_handlers->unknown_08 != nullptr)
            w->event_handlers->unknown_08(w);
}

void window_event_tool_update_call(rct_window* w, rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords)
{
    if (w->event_handlers == nullptr)
        w->OnToolUpdate(widgetIndex, screenCoords);
    else if (w->event_handlers->tool_update != nullptr)
        w->event_handlers->tool_update(w, widgetIndex, screenCoords);
}

void window_event_tool_down_call(rct_window* w, rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords)
{
    if (w->event_handlers == nullptr)
        w->OnToolDown(widgetIndex, screenCoords);
    else if (w->event_handlers->tool_down != nullptr)
        w->event_handlers->tool_down(w, widgetIndex, screenCoords);
}

void window_event_tool_drag_call(rct_window* w, rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords)
{
    if (w->event_handlers == nullptr)
        w->OnToolDrag(widgetIndex, screenCoords);
    else if (w->event_handlers->tool_drag != nullptr)
        w->event_handlers->tool_drag(w, widgetIndex, screenCoords);
}

void window_event_tool_up_call(rct_window* w, rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords)
{
    if (w->event_handlers == nullptr)
        w->OnToolUp(widgetIndex, screenCoords);
    else if (w->event_handlers->tool_up != nullptr)
        w->event_handlers->tool_up(w, widgetIndex, screenCoords);
}

void window_event_tool_abort_call(rct_window* w, rct_widgetindex widgetIndex)
{
    if (w->event_handlers == nullptr)
        w->OnToolAbort(widgetIndex);
    else if (w->event_handlers->tool_abort != nullptr)
        w->event_handlers->tool_abort(w, widgetIndex);
}

void window_event_unknown_0E_call(rct_window* w)
{
    if (w->event_handlers != nullptr)
        if (w->event_handlers->unknown_0E != nullptr)
            w->event_handlers->unknown_0E(w);
}

void window_get_scroll_size(rct_window* w, int32_t scrollIndex, int32_t* width, int32_t* height)
{
    if (w->event_handlers == nullptr)
    {
        auto size = w->OnScrollGetSize(scrollIndex);
        if (width != nullptr)
            *width = size.width;
        if (height != nullptr)
            *height = size.height;
    }
    else if (w->event_handlers->get_scroll_size != nullptr)
    {
        w->event_handlers->get_scroll_size(w, scrollIndex, width, height);
    }
}

void window_event_scroll_mousedown_call(rct_window* w, int32_t scrollIndex, const ScreenCoordsXY& screenCoords)
{
    if (w->event_handlers == nullptr)
        w->OnScrollMouseDown(scrollIndex, screenCoords);
    else if (w->event_handlers->scroll_mousedown != nullptr)
        w->event_handlers->scroll_mousedown(w, scrollIndex, screenCoords);
}

void window_event_scroll_mousedrag_call(rct_window* w, int32_t scrollIndex, const ScreenCoordsXY& screenCoords)
{
    if (w->event_handlers == nullptr)
        w->OnScrollMouseDrag(scrollIndex, screenCoords);
    else if (w->event_handlers->scroll_mousedrag != nullptr)
        w->event_handlers->scroll_mousedrag(w, scrollIndex, screenCoords);
}

void window_event_scroll_mouseover_call(rct_window* w, int32_t scrollIndex, const ScreenCoordsXY& screenCoords)
{
    if (w->event_handlers == nullptr)
        w->OnScrollMouseOver(scrollIndex, screenCoords);
    else if (w->event_handlers->scroll_mouseover != nullptr)
        w->event_handlers->scroll_mouseover(w, scrollIndex, screenCoords);
}

void window_event_textinput_call(rct_window* w, rct_widgetindex widgetIndex, char* text)
{
    if (w->event_handlers == nullptr)
    {
        if (text != nullptr)
        {
            w->OnTextInput(widgetIndex, text);
        }
    }
    else if (w->event_handlers->text_input != nullptr)
    {
        w->event_handlers->text_input(w, widgetIndex, text);
    }
}

void window_event_viewport_rotate_call(rct_window* w)
{
    if (w->event_handlers == nullptr)
        w->OnViewportRotate();
    else if (w->event_handlers != nullptr)
        if (w->event_handlers->viewport_rotate != nullptr)
            w->event_handlers->viewport_rotate(w);
}

void window_event_unknown_15_call(rct_window* w, int32_t scrollIndex, int32_t scrollAreaType)
{
    if (w->event_handlers != nullptr)
        if (w->event_handlers->unknown_15 != nullptr)
            w->event_handlers->unknown_15(w, scrollIndex, scrollAreaType);
}

OpenRCT2String window_event_tooltip_call(rct_window* w, const rct_widgetindex widgetIndex, const rct_string_id fallback)
{
    if (w->event_handlers == nullptr)
    {
        return w->OnTooltip(widgetIndex, fallback);
    }

    if (w->event_handlers->tooltip != nullptr)
    {
        return w->event_handlers->tooltip(w, widgetIndex, fallback);
    }

    return { fallback, {} };
}

CursorID window_event_cursor_call(rct_window* w, rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords)
{
    CursorID cursorId = CursorID::Arrow;
    if (w->event_handlers != nullptr)
        if (w->event_handlers->cursor != nullptr)
            w->event_handlers->cursor(w, widgetIndex, screenCoords, &cursorId);
    return cursorId;
}

void window_event_moved_call(rct_window* w, const ScreenCoordsXY& screenCoords)
{
    if (w->event_handlers != nullptr)
        if (w->event_handlers->moved != nullptr)
            w->event_handlers->moved(w, screenCoords);
}

void window_event_invalidate_call(rct_window* w)
{
    if (w->event_handlers == nullptr)
        w->OnPrepareDraw();
    else if (w->event_handlers->invalidate != nullptr)
        w->event_handlers->invalidate(w);
}

void window_event_paint_call(rct_window* w, rct_drawpixelinfo* dpi)
{
    if (w->event_handlers == nullptr)
        w->OnDraw(*dpi);
    else if (w->event_handlers->paint != nullptr)
        w->event_handlers->paint(w, dpi);
}

void window_event_scroll_paint_call(rct_window* w, rct_drawpixelinfo* dpi, int32_t scrollIndex)
{
    if (w->event_handlers == nullptr)
        w->OnScrollDraw(scrollIndex, *dpi);
    else if (w->event_handlers->scroll_paint != nullptr)
        w->event_handlers->scroll_paint(w, dpi, scrollIndex);
}

/**
 *
 *  rct2: 0x006ED710
 * Called after a window resize to move windows if they
 * are going to be out of sight.
 */
void window_relocate_windows(int32_t width, int32_t height)
{
    int32_t new_location = 8;
    window_visit_each([width, height, &new_location](rct_window* w) {
        // Work out if the window requires moving
        if (w->windowPos.x + 10 < width)
        {
            if (w->flags & (WF_STICK_TO_BACK | WF_STICK_TO_FRONT))
            {
                if (w->windowPos.y - 22 < height)
                {
                    return;
                }
            }
            if (w->windowPos.y + 10 < height)
            {
                return;
            }
        }

        // Calculate the new locations
        auto newWinPos = w->windowPos;
        w->windowPos = { new_location, new_location + TOP_TOOLBAR_HEIGHT + 1 };

        // Move the next new location so windows are not directly on top
        new_location += 8;

        // Adjust the viewport if required.
        if (w->viewport != nullptr)
        {
            w->viewport->pos -= newWinPos - w->windowPos;
        }
    });
}

/**
 * rct2: 0x0066B905
 */
void window_resize_gui(int32_t width, int32_t height)
{
    window_resize_gui_scenario_editor(width, height);
    if (gScreenFlags & SCREEN_FLAGS_EDITOR)
        return;

    rct_window* titleWind = window_find_by_class(WC_TITLE_MENU);
    if (titleWind != nullptr)
    {
        titleWind->windowPos.x = (width - titleWind->width) / 2;
        titleWind->windowPos.y = height - 182;
    }

    rct_window* exitWind = window_find_by_class(WC_TITLE_EXIT);
    if (exitWind != nullptr)
    {
        exitWind->windowPos.x = width - 40;
        exitWind->windowPos.y = height - 64;
    }

    rct_window* optionsWind = window_find_by_class(WC_TITLE_OPTIONS);
    if (optionsWind != nullptr)
    {
        optionsWind->windowPos.x = width - 80;
    }

    gfx_invalidate_screen();
}

/**
 * rct2: 0x0066F0DD
 */
void window_resize_gui_scenario_editor(int32_t width, int32_t height)
{
    auto* mainWind = window_get_main();
    if (mainWind != nullptr)
    {
        rct_viewport* viewport = mainWind->viewport;
        mainWind->width = width;
        mainWind->height = height;
        viewport->width = width;
        viewport->height = height;
        viewport->view_width = viewport->zoom.ApplyTo(width);
        viewport->view_height = viewport->zoom.ApplyTo(height);
        if (mainWind->widgets != nullptr && mainWind->widgets[WC_MAIN_WINDOW__0].type == WindowWidgetType::Viewport)
        {
            mainWind->widgets[WC_MAIN_WINDOW__0].right = width;
            mainWind->widgets[WC_MAIN_WINDOW__0].bottom = height;
        }
    }

    rct_window* topWind = window_find_by_class(WC_TOP_TOOLBAR);
    if (topWind != nullptr)
    {
        topWind->width = std::max(640, width);
    }

    rct_window* bottomWind = window_find_by_class(WC_BOTTOM_TOOLBAR);
    if (bottomWind != nullptr)
    {
        bottomWind->windowPos.y = height - 32;
        bottomWind->width = std::max(640, width);
    }
}

/**
 *
 *  rct2: 0x006CBCC3
 */
void window_close_construction_windows()
{
    window_close_by_class(WC_RIDE_CONSTRUCTION);
    window_close_by_class(WC_FOOTPATH);
    window_close_by_class(WC_TRACK_DESIGN_LIST);
    window_close_by_class(WC_TRACK_DESIGN_PLACE);
}

/**
 * Update zoom based volume attenuation for ride music and clear music list.
 *  rct2: 0x006BC348
 */
void window_update_viewport_ride_music()
{
    OpenRCT2::RideAudio::ClearAllViewportInstances();
    g_music_tracking_viewport = nullptr;

    for (auto it = g_window_list.rbegin(); it != g_window_list.rend(); it++)
    {
        auto w = it->get();
        auto viewport = w->viewport;
        if (viewport == nullptr || !(viewport->flags & VIEWPORT_FLAG_SOUND_ON))
            continue;

        g_music_tracking_viewport = viewport;
        gWindowAudioExclusive = w;

        if (viewport->zoom <= ZoomLevel{ 0 })
            OpenRCT2::Audio::gVolumeAdjustZoom = 0;
        else if (viewport->zoom == ZoomLevel{ 1 })
            OpenRCT2::Audio::gVolumeAdjustZoom = 30;
        else
            OpenRCT2::Audio::gVolumeAdjustZoom = 60;
        break;
    }
}

static void window_snap_left(rct_window* w, int32_t proximity)
{
    const auto* mainWindow = window_get_main();
    auto wBottom = w->windowPos.y + w->height;
    auto wLeftProximity = w->windowPos.x - (proximity * 2);
    auto wRightProximity = w->windowPos.x + (proximity * 2);
    auto rightMost = INT32_MIN;

    window_visit_each([&](rct_window* w2) {
        if (w2 == w || w2 == mainWindow)
            return;

        auto right = w2->windowPos.x + w2->width;

        if (wBottom < w2->windowPos.y || w->windowPos.y > w2->windowPos.y + w2->height)
            return;

        if (right < wLeftProximity || right > wRightProximity)
            return;

        rightMost = std::max(rightMost, right);
    });

    if (0 >= wLeftProximity && 0 <= wRightProximity)
        rightMost = std::max(rightMost, 0);

    if (rightMost != INT32_MIN)
        w->windowPos.x = rightMost;
}

static void window_snap_top(rct_window* w, int32_t proximity)
{
    const auto* mainWindow = window_get_main();
    auto wRight = w->windowPos.x + w->width;
    auto wTopProximity = w->windowPos.y - (proximity * 2);
    auto wBottomProximity = w->windowPos.y + (proximity * 2);
    auto bottomMost = INT32_MIN;

    window_visit_each([&](rct_window* w2) {
        if (w2 == w || w2 == mainWindow)
            return;

        auto bottom = w2->windowPos.y + w2->height;

        if (wRight < w2->windowPos.x || w->windowPos.x > w2->windowPos.x + w2->width)
            return;

        if (bottom < wTopProximity || bottom > wBottomProximity)
            return;

        bottomMost = std::max(bottomMost, bottom);
    });

    if (0 >= wTopProximity && 0 <= wBottomProximity)
        bottomMost = std::max(bottomMost, 0);

    if (bottomMost != INT32_MIN)
        w->windowPos.y = bottomMost;
}

static void window_snap_right(rct_window* w, int32_t proximity)
{
    const auto* mainWindow = window_get_main();
    auto wRight = w->windowPos.x + w->width;
    auto wBottom = w->windowPos.y + w->height;
    auto wLeftProximity = wRight - (proximity * 2);
    auto wRightProximity = wRight + (proximity * 2);
    auto leftMost = INT32_MAX;

    window_visit_each([&](rct_window* w2) {
        if (w2 == w || w2 == mainWindow)
            return;

        if (wBottom < w2->windowPos.y || w->windowPos.y > w2->windowPos.y + w2->height)
            return;

        if (w2->windowPos.x < wLeftProximity || w2->windowPos.x > wRightProximity)
            return;

        leftMost = std::min<int32_t>(leftMost, w2->windowPos.x);
    });

    auto screenWidth = context_get_width();
    if (screenWidth >= wLeftProximity && screenWidth <= wRightProximity)
        leftMost = std::min(leftMost, screenWidth);

    if (leftMost != INT32_MAX)
        w->windowPos.x = leftMost - w->width;
}

static void window_snap_bottom(rct_window* w, int32_t proximity)
{
    const auto* mainWindow = window_get_main();
    auto wRight = w->windowPos.x + w->width;
    auto wBottom = w->windowPos.y + w->height;
    auto wTopProximity = wBottom - (proximity * 2);
    auto wBottomProximity = wBottom + (proximity * 2);
    auto topMost = INT32_MAX;

    window_visit_each([&](rct_window* w2) {
        if (w2 == w || w2 == mainWindow)
            return;

        if (wRight < w2->windowPos.x || w->windowPos.x > w2->windowPos.x + w2->width)
            return;

        if (w2->windowPos.y < wTopProximity || w2->windowPos.y > wBottomProximity)
            return;

        topMost = std::min<int32_t>(topMost, w2->windowPos.y);
    });

    auto screenHeight = context_get_height();
    if (screenHeight >= wTopProximity && screenHeight <= wBottomProximity)
        topMost = std::min(topMost, screenHeight);

    if (topMost != INT32_MAX)
        w->windowPos.y = topMost - w->height;
}

void window_move_and_snap(rct_window* w, ScreenCoordsXY newWindowCoords, int32_t snapProximity)
{
    auto originalPos = w->windowPos;
    int32_t minY = (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO) ? 1 : TOP_TOOLBAR_HEIGHT + 2;

    newWindowCoords.y = std::clamp(newWindowCoords.y, minY, context_get_height() - 34);

    if (snapProximity > 0)
    {
        w->windowPos = newWindowCoords;

        window_snap_right(w, snapProximity);
        window_snap_bottom(w, snapProximity);
        window_snap_left(w, snapProximity);
        window_snap_top(w, snapProximity);

        if (w->windowPos == originalPos)
            return;

        newWindowCoords = w->windowPos;
        w->windowPos = originalPos;
    }

    window_set_position(w, newWindowCoords);
}

int32_t window_can_resize(rct_window* w)
{
    return (w->flags & WF_RESIZABLE) && (w->min_width != w->max_width || w->min_height != w->max_height);
}

/**
 *
 *  rct2: 0x006EE3C3
 */
void textinput_cancel()
{
    window_close_by_class(WC_TEXTINPUT);
}

void window_start_textbox(
    rct_window* call_w, rct_widgetindex call_widget, rct_string_id existing_text, char* existing_args, int32_t maxLength)
{
    if (gUsingWidgetTextBox)
        window_cancel_textbox();

    gUsingWidgetTextBox = true;
    gCurrentTextBox.window.classification = call_w->classification;
    gCurrentTextBox.window.number = call_w->number;
    gCurrentTextBox.widget_index = call_widget;
    gTextBoxFrameNo = 0;

    gMaxTextBoxInputLength = maxLength;

    window_close_by_class(WC_TEXTINPUT);

    // Clear the text input buffer
    std::fill_n(gTextBoxInput, maxLength, 0x00);

    // Enter in the text input buffer any existing
    // text.
    if (existing_text != STR_NONE)
        format_string(gTextBoxInput, TEXT_INPUT_SIZE, existing_text, &existing_args);

    // In order to prevent strings that exceed the maxLength
    // from crashing the game.
    gTextBoxInput[maxLength - 1] = '\0';

    gTextInput = context_start_text_input(gTextBoxInput, maxLength);
}

void window_cancel_textbox()
{
    if (gUsingWidgetTextBox)
    {
        rct_window* w = window_find_by_number(gCurrentTextBox.window.classification, gCurrentTextBox.window.number);
        if (w != nullptr)
        {
            window_event_textinput_call(w, gCurrentTextBox.widget_index, nullptr);
        }
        gCurrentTextBox.window.classification = WC_NULL;
        gCurrentTextBox.window.number = 0;
        context_stop_text_input();
        gUsingWidgetTextBox = false;
        if (w != nullptr)
        {
            widget_invalidate(w, gCurrentTextBox.widget_index);
        }
        gCurrentTextBox.widget_index = static_cast<uint16_t>(WindowWidgetType::Last);
    }
}

void window_update_textbox_caret()
{
    gTextBoxFrameNo++;
    if (gTextBoxFrameNo > 30)
        gTextBoxFrameNo = 0;
}

void window_update_textbox()
{
    if (gUsingWidgetTextBox)
    {
        gTextBoxFrameNo = 0;
        rct_window* w = window_find_by_number(gCurrentTextBox.window.classification, gCurrentTextBox.window.number);
        widget_invalidate(w, gCurrentTextBox.widget_index);
        window_event_textinput_call(w, gCurrentTextBox.widget_index, gTextBoxInput);
    }
}

bool window_is_visible(rct_window* w)
{
    // w->visibility is used to prevent repeat calculations within an iteration by caching the result
    if (w == nullptr)
        return false;

    if (w->visibility == VisibilityCache::Visible)
        return true;
    if (w->visibility == VisibilityCache::Covered)
        return false;

    // only consider viewports, consider the main window always visible
    if (w->viewport == nullptr || w->classification == WC_MAIN_WINDOW)
    {
        // default to previous behaviour
        w->visibility = VisibilityCache::Visible;
        return true;
    }

    // start from the window above the current
    auto itPos = window_get_iterator(w);
    for (auto it = std::next(itPos); it != g_window_list.end(); it++)
    {
        auto& w_other = *(*it);

        // if covered by a higher window, no rendering needed
        if (w_other.windowPos.x <= w->windowPos.x && w_other.windowPos.y <= w->windowPos.y
            && w_other.windowPos.x + w_other.width >= w->windowPos.x + w->width
            && w_other.windowPos.y + w_other.height >= w->windowPos.y + w->height)
        {
            w->visibility = VisibilityCache::Covered;
            w->viewport->visibility = VisibilityCache::Covered;
            return false;
        }
    }

    // default to previous behaviour
    w->visibility = VisibilityCache::Visible;
    w->viewport->visibility = VisibilityCache::Visible;
    return true;
}

/**
 *
 *  rct2: 0x006E7499
 * left (ax)
 * top (bx)
 * right (dx)
 * bottom (bp)
 */
void window_draw_all(rct_drawpixelinfo* dpi, int32_t left, int32_t top, int32_t right, int32_t bottom)
{
    auto windowDPI = dpi->Crop({ left, top }, { right - left, bottom - top });
    window_visit_each([&windowDPI, left, top, right, bottom](rct_window* w) {
        if (w->flags & WF_TRANSPARENT)
            return;
        if (right <= w->windowPos.x || bottom <= w->windowPos.y)
            return;
        if (left >= w->windowPos.x + w->width || top >= w->windowPos.y + w->height)
            return;
        window_draw(&windowDPI, w, left, top, right, bottom);
    });
}

rct_viewport* window_get_previous_viewport(rct_viewport* current)
{
    bool foundPrevious = (current == nullptr);
    for (auto it = g_window_list.rbegin(); it != g_window_list.rend(); it++)
    {
        auto& w = **it;
        if (w.viewport != nullptr)
        {
            if (foundPrevious)
            {
                return w.viewport;
            }
            if (w.viewport == current)
            {
                foundPrevious = true;
            }
        }
    }
    return nullptr;
}

void window_reset_visibilities()
{
    // reset window visibility status to unknown
    window_visit_each([](rct_window* w) {
        w->visibility = VisibilityCache::Unknown;
        if (w->viewport != nullptr)
        {
            w->viewport->visibility = VisibilityCache::Unknown;
        }
    });
}

void window_init_all()
{
    window_close_all_except_flags(0);
}

void window_follow_sprite(rct_window* w, EntityId spriteIndex)
{
    if (spriteIndex.ToUnderlying() < MAX_ENTITIES || spriteIndex.IsNull())
    {
        w->viewport_smart_follow_sprite = spriteIndex;
    }
}

void window_unfollow_sprite(rct_window* w)
{
    w->viewport_smart_follow_sprite = EntityId::GetNull();
    w->viewport_target_sprite = EntityId::GetNull();
}

rct_viewport* window_get_viewport(rct_window* w)
{
    if (w == nullptr)
    {
        return nullptr;
    }

    return w->viewport;
}

rct_window* window_get_listening()
{
    for (auto it = g_window_list.rbegin(); it != g_window_list.rend(); it++)
    {
        auto& w = **it;
        auto viewport = w.viewport;
        if (viewport != nullptr)
        {
            if (viewport->flags & VIEWPORT_FLAG_SOUND_ON)
            {
                return &w;
            }
        }
    }
    return nullptr;
}

rct_windowclass window_get_classification(rct_window* window)
{
    return window->classification;
}

/**
 *
 *  rct2: 0x006EAF26
 */
void WidgetScrollUpdateThumbs(rct_window* w, rct_widgetindex widget_index)
{
    const auto& widget = w->widgets[widget_index];
    auto& scroll = w->scrolls[window_get_scroll_data_index(w, widget_index)];

    if (scroll.flags & HSCROLLBAR_VISIBLE)
    {
        int32_t view_size = widget.width() - 21;
        if (scroll.flags & VSCROLLBAR_VISIBLE)
            view_size -= 11;
        int32_t x = scroll.h_left * view_size;
        if (scroll.h_right != 0)
            x /= scroll.h_right;
        scroll.h_thumb_left = x + 11;

        x = widget.width() - 2;
        if (scroll.flags & VSCROLLBAR_VISIBLE)
            x -= 11;
        x += scroll.h_left;
        if (scroll.h_right != 0)
            x = (x * view_size) / scroll.h_right;
        x += 11;
        view_size += 10;
        scroll.h_thumb_right = std::min(x, view_size);

        if (scroll.h_thumb_right - scroll.h_thumb_left < 20)
        {
            double barPosition = (scroll.h_thumb_right * 1.0) / view_size;

            scroll.h_thumb_left = static_cast<uint16_t>(std::lround(scroll.h_thumb_left - (20 * barPosition)));
            scroll.h_thumb_right = static_cast<uint16_t>(std::lround(scroll.h_thumb_right + (20 * (1 - barPosition))));
        }
    }

    if (scroll.flags & VSCROLLBAR_VISIBLE)
    {
        int32_t view_size = widget.height() - 21;
        if (scroll.flags & HSCROLLBAR_VISIBLE)
            view_size -= 11;
        int32_t y = scroll.v_top * view_size;
        if (scroll.v_bottom != 0)
            y /= scroll.v_bottom;
        scroll.v_thumb_top = y + 11;

        y = widget.height() - 2;
        if (scroll.flags & HSCROLLBAR_VISIBLE)
            y -= 11;
        y += scroll.v_top;
        if (scroll.v_bottom != 0)
            y = (y * view_size) / scroll.v_bottom;
        y += 11;
        view_size += 10;
        scroll.v_thumb_bottom = std::min(y, view_size);

        if (scroll.v_thumb_bottom - scroll.v_thumb_top < 20)
        {
            double barPosition = (scroll.v_thumb_bottom * 1.0) / view_size;

            scroll.v_thumb_top = static_cast<uint16_t>(std::lround(scroll.v_thumb_top - (20 * barPosition)));
            scroll.v_thumb_bottom = static_cast<uint16_t>(std::lround(scroll.v_thumb_bottom + (20 * (1 - barPosition))));
        }
    }
}
