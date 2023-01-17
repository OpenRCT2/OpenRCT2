/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
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
#include "../localisation/Formatting.h"
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

widget_identifier gCurrentTextBox = { { WindowClass::Null, 0 }, 0 };
window_close_modifier gLastCloseModifier = { { WindowClass::Null, 0 }, CloseWindowModifier::None };
char gTextBoxInput[TEXT_INPUT_SIZE] = { 0 };
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

static void WindowDrawCore(rct_drawpixelinfo* dpi, rct_window& w, int32_t left, int32_t top, int32_t right, int32_t bottom);
static void WindowDrawSingle(rct_drawpixelinfo* dpi, rct_window& w, int32_t left, int32_t top, int32_t right, int32_t bottom);

std::list<std::shared_ptr<rct_window>>::iterator WindowGetIterator(const rct_window* w)
{
    return std::find_if(g_window_list.begin(), g_window_list.end(), [w](const std::shared_ptr<rct_window>& w2) -> bool {
        return w == w2.get();
    });
}

void WindowVisitEach(std::function<void(rct_window*)> func)
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
void WindowDispatchUpdateAll()
{
    // gTooltipNotShownTicks++;
    WindowVisitEach([&](rct_window* w) { WindowEventUpdateCall(w); });
}

void WindowUpdateAllViewports()
{
    WindowVisitEach([&](rct_window* w) {
        if (w->viewport != nullptr && WindowIsVisible(*w))
        {
            ViewportUpdatePosition(w);
        }
    });
}

/**
 *
 *  rct2: 0x006E77A1
 */
void WindowUpdateAll()
{
    // WindowUpdateAllViewports();

    // 1000 tick update
    gWindowUpdateTicks += gCurrentDeltaTime;
    if (gWindowUpdateTicks >= 1000)
    {
        gWindowUpdateTicks = 0;

        WindowVisitEach([](rct_window* w) { WindowEventPeriodicUpdateCall(w); });
    }

    // Border flash invalidation
    WindowVisitEach([](rct_window* w) {
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

static void WindowCloseSurplus(int32_t cap, WindowClass avoid_classification)
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
        // skip window if window matches specified WindowClass (as user may be modifying via options)
        if (avoid_classification != WindowClass::Null && foundW != nullptr && foundW->classification == avoid_classification)
        {
            continue;
        }
        WindowClose(*foundW);
    }
}

/*
 * Changes the maximum amount of windows allowed
 */
void WindowSetWindowLimit(int32_t value)
{
    int32_t prev = gConfigGeneral.WindowLimit;
    int32_t val = std::clamp(value, WINDOW_LIMIT_MIN, WINDOW_LIMIT_MAX);
    gConfigGeneral.WindowLimit = val;
    ConfigSaveDefault();
    // Checks if value decreases and then closes surplus
    // windows if one sets a limit lower than the number of windows open
    if (val < prev)
    {
        WindowCloseSurplus(val, WindowClass::Options);
    }
}

/**
 * Closes the specified window.
 *  rct2: 0x006ECD4C
 *
 * @param window The window to close (esi).
 */
void WindowClose(rct_window& w)
{
    auto itWindow = WindowGetIterator(&w);
    if (itWindow == g_window_list.end())
        return;

    // Explicit copy of the shared ptr to keep the memory valid.
    std::shared_ptr<rct_window> window = *itWindow;

    WindowEventCloseCall(window.get());

    // Remove viewport
    window->RemoveViewport();

    // Invalidate the window (area)
    window->Invalidate();

    // The window list may have been modified in the close event
    itWindow = WindowGetIterator(&w);
    if (itWindow != g_window_list.end())
        g_window_list.erase(itWindow);
}

template<typename TPred> static void WindowCloseByCondition(TPred pred, uint32_t flags = WindowCloseFlags::None)
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

            WindowClose(*window.get());

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
void WindowCloseByClass(WindowClass cls)
{
    WindowCloseByCondition([&](rct_window* w) -> bool { return w->classification == cls; });
}

/**
 * Closes all windows with specified window class and number.
 *  rct2: 0x006ECCF4
 * @param cls (cl) without bit 15 set
 * @param number (dx)
 */
void WindowCloseByNumber(WindowClass cls, rct_windownumber number)
{
    WindowCloseByCondition([cls, number](rct_window* w) -> bool { return w->classification == cls && w->number == number; });
}

// TODO: Refactor this to use variant once the new window class is done.
void WindowCloseByNumber(WindowClass cls, EntityId number)
{
    WindowCloseByNumber(cls, static_cast<rct_windownumber>(number.ToUnderlying()));
}

/**
 * Finds the first window with the specified window class.
 *  rct2: 0x006EA8A0
 * @param cls (cl) with bit 15 set
 * @returns the window or NULL if no window was found.
 */
rct_window* WindowFindByClass(WindowClass cls)
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
rct_window* WindowFindByNumber(WindowClass cls, rct_windownumber number)
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
rct_window* WindowFindByNumber(WindowClass cls, EntityId id)
{
    return WindowFindByNumber(cls, static_cast<rct_windownumber>(id.ToUnderlying()));
}

/**
 * Closes the top-most window
 *
 *  rct2: 0x006E403C
 */
void WindowCloseTop()
{
    WindowCloseByClass(WindowClass::Dropdown);

    if (gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR)
    {
        if (gEditorStep != EditorStep::LandscapeEditor)
            return;
    }

    auto pred = [](rct_window* w) -> bool { return !(w->flags & (WF_STICK_TO_BACK | WF_STICK_TO_FRONT)); };
    WindowCloseByCondition(pred, WindowCloseFlags::CloseSingle | WindowCloseFlags::IterateReverse);
}

/**
 * Closes all open windows
 *
 *  rct2: 0x006EE927
 */
void WindowCloseAll()
{
    WindowCloseByClass(WindowClass::Dropdown);
    WindowCloseByCondition([](rct_window* w) -> bool { return !(w->flags & (WF_STICK_TO_BACK | WF_STICK_TO_FRONT)); });
}

void WindowCloseAllExceptClass(WindowClass cls)
{
    WindowCloseByClass(WindowClass::Dropdown);

    WindowCloseByCondition([cls](rct_window* w) -> bool {
        return w->classification != cls && !(w->flags & (WF_STICK_TO_BACK | WF_STICK_TO_FRONT));
    });
}

/**
 * Closes all windows, save for those having any of the passed flags.
 */
void WindowCloseAllExceptFlags(uint16_t flags)
{
    WindowCloseByCondition([flags](rct_window* w) -> bool { return !(w->flags & flags); });
}

/**
 * Closes all windows except the specified window number and class.
 * @param number (dx)
 * @param cls (cl) without bit 15 set
 */
void WindowCloseAllExceptNumberAndClass(rct_windownumber number, WindowClass cls)
{
    WindowCloseByClass(WindowClass::Dropdown);
    WindowCloseByCondition([cls, number](rct_window* w) -> bool {
        return (!(w->number == number && w->classification == cls) && !(w->flags & (WF_STICK_TO_BACK | WF_STICK_TO_FRONT)));
    });
}

/**
 *
 *  rct2: 0x006EA845
 */
rct_window* WindowFindFromPoint(const ScreenCoordsXY& screenCoords)
{
    for (auto it = g_window_list.rbegin(); it != g_window_list.rend(); it++)
    {
        auto& w = *it;
        if (screenCoords.x < w->windowPos.x || screenCoords.x >= w->windowPos.x + w->width || screenCoords.y < w->windowPos.y
            || screenCoords.y >= w->windowPos.y + w->height)
            continue;

        if (w->flags & WF_NO_BACKGROUND)
        {
            auto widgetIndex = WindowFindWidgetFromPoint(*w.get(), screenCoords);
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
WidgetIndex WindowFindWidgetFromPoint(rct_window& w, const ScreenCoordsXY& screenCoords)
{
    // Invalidate the window
    WindowEventInvalidateCall(&w);

    // Find the widget at point x, y
    WidgetIndex widget_index = -1;
    for (int32_t i = 0;; i++)
    {
        const auto& widget = w.widgets[i];
        if (widget.type == WindowWidgetType::Last)
        {
            break;
        }

        if (widget.type != WindowWidgetType::Empty && widget.IsVisible())
        {
            if (screenCoords.x >= w.windowPos.x + widget.left && screenCoords.x <= w.windowPos.x + widget.right
                && screenCoords.y >= w.windowPos.y + widget.top && screenCoords.y <= w.windowPos.y + widget.bottom)
            {
                widget_index = i;
            }
        }
    }

    // Return next widget if a dropdown
    if (widget_index != -1)
    {
        const auto& widget = w.widgets[widget_index];
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
template<typename TPred> static void WindowInvalidateByCondition(TPred pred)
{
    WindowVisitEach([pred](rct_window* w) {
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
void WindowInvalidateByClass(WindowClass cls)
{
    WindowInvalidateByCondition([cls](rct_window* w) -> bool { return w->classification == cls; });
}

/**
 * Invalidates all windows with the specified window class and number.
 *  rct2: 0x006EC3AC
 */
void WindowInvalidateByNumber(WindowClass cls, rct_windownumber number)
{
    WindowInvalidateByCondition(
        [cls, number](rct_window* w) -> bool { return w->classification == cls && w->number == number; });
}

// TODO: Use variant for this once the window framework is done.
void WindowInvalidateByNumber(WindowClass cls, EntityId id)
{
    WindowInvalidateByNumber(cls, static_cast<rct_windownumber>(id.ToUnderlying()));
}

/**
 * Invalidates all windows.
 */
void WindowInvalidateAll()
{
    WindowVisitEach([](rct_window* w) { w->Invalidate(); });
}

/**
 * Invalidates the specified widget of a window.
 *  rct2: 0x006EC402
 */
void WidgetInvalidate(rct_window& w, WidgetIndex widgetIndex)
{
#ifdef DEBUG
    for (int32_t i = 0; i <= widgetIndex; i++)
    {
        assert(w.widgets[i].type != WindowWidgetType::Last);
    }
#endif

    const auto& widget = w.widgets[widgetIndex];
    if (widget.left == -2)
        return;

    GfxSetDirtyBlocks({ { w.windowPos + ScreenCoordsXY{ widget.left, widget.top } },
                        { w.windowPos + ScreenCoordsXY{ widget.right + 1, widget.bottom + 1 } } });
}

template<typename TPred> static void widget_invalidate_by_condition(TPred pred)
{
    WindowVisitEach([pred](rct_window* w) {
        if (pred(w))
        {
            w->Invalidate();
        }
    });
}

/**
 * Invalidates the specified widget of all windows that match the specified window class.
 */
void WidgetInvalidateByClass(WindowClass cls, WidgetIndex widgetIndex)
{
    WindowVisitEach([cls, widgetIndex](rct_window* w) {
        if (w->classification == cls)
        {
            WidgetInvalidate(*w, widgetIndex);
        }
    });
}

/**
 * Invalidates the specified widget of all windows that match the specified window class and number.
 *  rct2: 0x006EC3AC
 */
void WidgetInvalidateByNumber(WindowClass cls, rct_windownumber number, WidgetIndex widgetIndex)
{
    WindowVisitEach([cls, number, widgetIndex](rct_window* w) {
        if (w->classification == cls && w->number == number)
        {
            WidgetInvalidate(*w, widgetIndex);
        }
    });
}

/**
 *
 *  rct2: 0x006EAE4E
 *
 * @param w The window (esi).
 */
void WindowUpdateScrollWidgets(rct_window& w)
{
    int32_t scrollIndex, width, height, scrollPositionChanged;
    WidgetIndex widgetIndex;
    Widget* widget;

    widgetIndex = 0;
    scrollIndex = 0;
    for (widget = w.widgets; widget->type != WindowWidgetType::Last; widget++, widgetIndex++)
    {
        if (widget->type != WindowWidgetType::Scroll)
            continue;

        auto& scroll = w.scrolls[scrollIndex];
        width = 0;
        height = 0;
        WindowGetScrollSize(&w, scrollIndex, &width, &height);
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
            w.Invalidate();
        }
        scrollIndex++;
    }
}

int32_t WindowGetScrollDataIndex(const rct_window& w, WidgetIndex widget_index)
{
    int32_t i, result;

    result = 0;
    for (i = 0; i < widget_index; i++)
    {
        const auto& widget = w.widgets[i];
        if (widget.type == WindowWidgetType::Scroll)
            result++;
    }
    return result;
}

/**
 *
 *  rct2: 0x006ECDA4
 */
rct_window* WindowBringToFront(rct_window& w)
{
    if (!(w.flags & (WF_STICK_TO_BACK | WF_STICK_TO_FRONT)))
    {
        auto itSourcePos = WindowGetIterator(&w);
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
            w.Invalidate();

            if (w.windowPos.x + w.width < 20)
            {
                int32_t i = 20 - w.windowPos.x;
                w.windowPos.x += i;
                if (w.viewport != nullptr)
                    w.viewport->pos.x += i;
                w.Invalidate();
            }
        }
    }
    return &w;
}

rct_window* WindowBringToFrontByClassWithFlags(WindowClass cls, uint16_t flags)
{
    rct_window* w = WindowFindByClass(cls);
    if (w != nullptr)
    {
        w->flags |= flags;
        w->Invalidate();
        w = WindowBringToFront(*w);
    }

    return w;
}

rct_window* WindowBringToFrontByClass(WindowClass cls)
{
    return WindowBringToFrontByClassWithFlags(cls, WF_WHITE_BORDER_MASK);
}

/**
 *
 *  rct2: 0x006ED78A
 * cls (cl)
 * number (dx)
 */
rct_window* WindowBringToFrontByNumber(WindowClass cls, rct_windownumber number)
{
    rct_window* w;

    w = WindowFindByNumber(cls, number);
    if (w != nullptr)
    {
        w->flags |= WF_WHITE_BORDER_MASK;
        w->Invalidate();
        w = WindowBringToFront(*w);
    }

    return w;
}

/**
 *
 *  rct2: 0x006EE65A
 */
void WindowPushOthersRight(rct_window& window)
{
    WindowVisitEach([&window](rct_window* w) {
        if (w == &window)
            return;
        if (w->flags & (WF_STICK_TO_BACK | WF_STICK_TO_FRONT))
            return;
        if (w->windowPos.x >= window.windowPos.x + window.width)
            return;
        if (w->windowPos.x + w->width <= window.windowPos.x)
            return;
        if (w->windowPos.y >= window.windowPos.y + window.height)
            return;
        if (w->windowPos.y + w->height <= window.windowPos.y)
            return;

        w->Invalidate();
        if (window.windowPos.x + window.width + 13 >= ContextGetWidth())
            return;
        auto push_amount = window.windowPos.x + window.width - w->windowPos.x + 3;
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
void WindowPushOthersBelow(rct_window& w1)
{
    // Enumerate through all other windows
    WindowVisitEach([&w1](rct_window* w2) {
        if (&w1 == w2)
            return;
        // ?
        if (w2->flags & (WF_STICK_TO_BACK | WF_STICK_TO_FRONT))
            return;
        // Check if w2 intersects with w1
        if (w2->windowPos.x > (w1.windowPos.x + w1.width) || w2->windowPos.x + w2->width < w1.windowPos.x)
            return;
        if (w2->windowPos.y > (w1.windowPos.y + w1.height) || w2->windowPos.y + w2->height < w1.windowPos.y)
            return;

        // Check if there is room to push it down
        if (w1.windowPos.y + w1.height + 80 >= ContextGetHeight())
            return;

        // Invalidate the window's current area
        w2->Invalidate();

        int32_t push_amount = w1.windowPos.y + w1.height - w2->windowPos.y + 3;
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
rct_window* WindowGetMain()
{
    for (auto& w : g_window_list)
    {
        if (w->classification == WindowClass::MainWindow)
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
void WindowScrollToLocation(rct_window& w, const CoordsXYZ& coords)
{
    WindowUnfollowSprite(w);
    if (w.viewport != nullptr)
    {
        int16_t height = TileElementHeight(coords);
        if (coords.z < height - 16)
        {
            if (!(w.viewport->flags & VIEWPORT_FLAG_UNDERGROUND_INSIDE))
            {
                w.viewport->flags |= VIEWPORT_FLAG_UNDERGROUND_INSIDE;
                w.Invalidate();
            }
        }
        else
        {
            if (w.viewport->flags & VIEWPORT_FLAG_UNDERGROUND_INSIDE)
            {
                w.viewport->flags &= ~VIEWPORT_FLAG_UNDERGROUND_INSIDE;
                w.Invalidate();
            }
        }

        auto screenCoords = Translate3DTo2DWithZ(GetCurrentRotation(), coords);

        int32_t i = 0;
        if (!(gScreenFlags & SCREEN_FLAGS_TITLE_DEMO))
        {
            bool found = false;
            while (!found)
            {
                auto x2 = w.viewport->pos.x + static_cast<int32_t>(w.viewport->width * window_scroll_locations[i][0]);
                auto y2 = w.viewport->pos.y + static_cast<int32_t>(w.viewport->height * window_scroll_locations[i][1]);

                auto it = WindowGetIterator(&w);
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
        if (w.viewport_target_sprite.IsNull())
        {
            if (!(w.flags & WF_NO_SCROLLING))
            {
                w.savedViewPos = screenCoords
                    - ScreenCoordsXY{ static_cast<int32_t>(w.viewport->view_width * window_scroll_locations[i][0]),
                                      static_cast<int32_t>(w.viewport->view_height * window_scroll_locations[i][1]) };
                w.flags |= WF_SCROLLING_TO_LOCATION;
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
    WindowVisitEach([](rct_window* w) { WindowEventViewportRotateCall(w); });
}

/**
 *
 *  rct2: 0x0068881A
 * direction can be used to alter the camera rotation:
 *      1: clockwise
 *      -1: anti-clockwise
 */
void WindowRotateCamera(rct_window& w, int32_t direction)
{
    rct_viewport* viewport = w.viewport;
    if (viewport == nullptr)
        return;

    auto windowPos = ScreenCoordsXY{ (viewport->width >> 1), (viewport->height >> 1) } + viewport->pos;

    // has something to do with checking if middle of the viewport is obstructed
    rct_viewport* other;
    auto mapXYCoords = ScreenGetMapXY(windowPos, &other);
    CoordsXYZ coords{};

    // other != viewport probably triggers on viewports in ride or guest window?
    // naoXYCoords is nullopt if middle of viewport is obstructed by another window?
    if (!mapXYCoords.has_value() || other != viewport)
    {
        auto viewPos = ScreenCoordsXY{ (viewport->view_width >> 1), (viewport->view_height >> 1) } + viewport->viewPos;

        coords = ViewportAdjustForMapHeight(viewPos);
    }
    else
    {
        coords.x = mapXYCoords->x;
        coords.y = mapXYCoords->y;
        coords.z = TileElementHeight(coords);
    }

    gCurrentRotation = (GetCurrentRotation() + direction) & 3;

    auto centreLoc = centre_2d_coordinates(coords, viewport);

    if (centreLoc.has_value())
    {
        w.savedViewPos = centreLoc.value();
        viewport->viewPos = *centreLoc;
    }

    w.Invalidate();

    call_event_viewport_rotate_on_all_windows();
    ResetAllSpriteQuadrantPlacements();
}

void WindowViewportGetMapCoordsByCursor(
    const rct_window& w, int32_t* map_x, int32_t* map_y, int32_t* offset_x, int32_t* offset_y)
{
    // Get mouse position to offset against.
    auto mouseCoords = ContextGetCursorPositionScaled();

    // Compute map coordinate by mouse position.
    auto viewportPos = w.viewport->ScreenToViewportCoord(mouseCoords);
    auto coordsXYZ = ViewportAdjustForMapHeight(viewportPos);
    auto mapCoords = ViewportPosToMapPos(viewportPos, coordsXYZ.z);
    *map_x = mapCoords.x;
    *map_y = mapCoords.y;

    // Get viewport coordinates centring around the tile.
    int32_t z = TileElementHeight(mapCoords);

    auto centreLoc = centre_2d_coordinates({ mapCoords.x, mapCoords.y, z }, w.viewport);
    if (!centreLoc)
    {
        LOG_ERROR("Invalid location.");
        return;
    }

    // Rebase mouse position onto centre of window, and compensate for zoom level.
    int32_t rebased_x = w.viewport->zoom.ApplyTo(w.width / 2 - mouseCoords.x);
    int32_t rebased_y = w.viewport->zoom.ApplyTo(w.height / 2 - mouseCoords.y);

    // Compute cursor offset relative to tile.
    *offset_x = w.viewport->zoom.ApplyTo(w.savedViewPos.x - (centreLoc->x + rebased_x));
    *offset_y = w.viewport->zoom.ApplyTo(w.savedViewPos.y - (centreLoc->y + rebased_y));
}

void WindowViewportCentreTileAroundCursor(rct_window& w, int32_t map_x, int32_t map_y, int32_t offset_x, int32_t offset_y)
{
    // Get viewport coordinates centring around the tile.
    int32_t z = TileElementHeight({ map_x, map_y });
    auto centreLoc = centre_2d_coordinates({ map_x, map_y, z }, w.viewport);

    if (!centreLoc.has_value())
    {
        LOG_ERROR("Invalid location.");
        return;
    }

    // Get mouse position to offset against.
    auto mouseCoords = ContextGetCursorPositionScaled();

    // Rebase mouse position onto centre of window, and compensate for zoom level.
    int32_t rebased_x = w.viewport->zoom.ApplyTo((w.width >> 1) - mouseCoords.x);
    int32_t rebased_y = w.viewport->zoom.ApplyTo((w.height >> 1) - mouseCoords.y);

    // Apply offset to the viewport.
    w.savedViewPos = { centreLoc->x + rebased_x + w.viewport->zoom.ApplyInversedTo(offset_x),
                       centreLoc->y + rebased_y + w.viewport->zoom.ApplyInversedTo(offset_y) };
}

/**
 * For all windows with viewports, ensure they do not have a zoom level less than the minimum.
 */
void WindowCheckAllValidZoom()
{
    WindowVisitEach([](rct_window* w) {
        if (w->viewport != nullptr && w->viewport->zoom < ZoomLevel::min())
        {
            WindowZoomSet(*w, ZoomLevel::min(), false);
        }
    });
}

void WindowZoomSet(rct_window& w, ZoomLevel zoomLevel, bool atCursor)
{
    rct_viewport* v = w.viewport;
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
    if (gConfigGeneral.ZoomToCursor && atCursor)
    {
        WindowViewportGetMapCoordsByCursor(w, &saved_map_x, &saved_map_y, &offset_x, &offset_y);
    }

    // Zoom in
    while (v->zoom > zoomLevel)
    {
        v->zoom--;
        w.savedViewPos.x += v->view_width / 4;
        w.savedViewPos.y += v->view_height / 4;
        v->view_width /= 2;
        v->view_height /= 2;
    }

    // Zoom out
    while (v->zoom < zoomLevel)
    {
        v->zoom++;
        w.savedViewPos.x -= v->view_width / 2;
        w.savedViewPos.y -= v->view_height / 2;
        v->view_width *= 2;
        v->view_height *= 2;
    }

    // Zooming to cursor? Centre around the tile we were hovering over just now.
    if (gConfigGeneral.ZoomToCursor && atCursor)
    {
        WindowViewportCentreTileAroundCursor(w, saved_map_x, saved_map_y, offset_x, offset_y);
    }

    // HACK: Prevents the redraw from failing when there is
    // a window on top of the viewport.
    WindowBringToFront(w);
    w.Invalidate();
}

/**
 *
 *  rct2: 0x006887A6
 */
void WindowZoomIn(rct_window& w, bool atCursor)
{
    WindowZoomSet(w, w.viewport->zoom - 1, atCursor);
}

/**
 *
 *  rct2: 0x006887E0
 */
void WindowZoomOut(rct_window& w, bool atCursor)
{
    WindowZoomSet(w, w.viewport->zoom + 1, atCursor);
}

void MainWindowZoom(bool zoomIn, bool atCursor)
{
    auto* mainWindow = WindowGetMain();
    if (mainWindow == nullptr)
        return;

    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    if (gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR && gEditorStep != EditorStep::LandscapeEditor)
        return;

    if (gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER)
        return;

    if (zoomIn)
        WindowZoomIn(*mainWindow, atCursor);
    else
        WindowZoomOut(*mainWindow, atCursor);
}

/**
 * Splits a drawing of a window into regions that can be seen and are not hidden
 * by other opaque overlapping windows.
 */
void WindowDraw(rct_drawpixelinfo* dpi, rct_window& w, int32_t left, int32_t top, int32_t right, int32_t bottom)
{
    if (!WindowIsVisible(w))
        return;

    // Divide the draws up for only the visible regions of the window recursively
    auto itPos = WindowGetIterator(&w);
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
            WindowDrawCore(dpi, w, left, top, topwindow->windowPos.x, bottom);
            WindowDrawCore(dpi, w, topwindow->windowPos.x, top, right, bottom);
        }
        else if (topwindow->windowPos.x + topwindow->width < right)
        {
            // Split draw at topwindow.right
            WindowDrawCore(dpi, w, left, top, topwindow->windowPos.x + topwindow->width, bottom);
            WindowDrawCore(dpi, w, topwindow->windowPos.x + topwindow->width, top, right, bottom);
        }
        else if (topwindow->windowPos.y > top)
        {
            // Split draw at topwindow.top
            WindowDrawCore(dpi, w, left, top, right, topwindow->windowPos.y);
            WindowDrawCore(dpi, w, left, topwindow->windowPos.y, right, bottom);
        }
        else if (topwindow->windowPos.y + topwindow->height < bottom)
        {
            // Split draw at topwindow.bottom
            WindowDrawCore(dpi, w, left, top, right, topwindow->windowPos.y + topwindow->height);
            WindowDrawCore(dpi, w, left, topwindow->windowPos.y + topwindow->height, right, bottom);
        }

        // Drawing for this region should be done now, exit
        return;
    }

    // No windows overlap
    WindowDrawCore(dpi, w, left, top, right, bottom);
}

/**
 * Draws the given window and any other overlapping transparent windows.
 */
static void WindowDrawCore(rct_drawpixelinfo* dpi, rct_window& w, int32_t left, int32_t top, int32_t right, int32_t bottom)
{
    // Clamp region
    left = std::max<int32_t>(left, w.windowPos.x);
    top = std::max<int32_t>(top, w.windowPos.y);
    right = std::min<int32_t>(right, w.windowPos.x + w.width);
    bottom = std::min<int32_t>(bottom, w.windowPos.y + w.height);
    if (left >= right)
        return;
    if (top >= bottom)
        return;

    // Draw the window and any other overlapping transparent windows
    for (auto it = WindowGetIterator(&w); it != g_window_list.end(); it++)
    {
        auto* v = (*it).get();
        if ((&w == v || (v->flags & WF_TRANSPARENT)) && WindowIsVisible(*v))
        {
            WindowDrawSingle(dpi, *v, left, top, right, bottom);
        }
    }
}

static void WindowDrawSingle(rct_drawpixelinfo* dpi, rct_window& w, int32_t left, int32_t top, int32_t right, int32_t bottom)
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
    WindowEventInvalidateCall(&w);

    // Text colouring
    gCurrentWindowColours[0] = NOT_TRANSLUCENT(w.colours[0]);
    gCurrentWindowColours[1] = NOT_TRANSLUCENT(w.colours[1]);
    gCurrentWindowColours[2] = NOT_TRANSLUCENT(w.colours[2]);
    gCurrentWindowColours[3] = NOT_TRANSLUCENT(w.colours[3]);

    WindowEventPaintCall(&w, dpi);
}

/**
 *
 *  rct2: 0x00685BE1
 *
 * @param dpi (edi)
 * @param w (esi)
 */
void WindowDrawViewport(rct_drawpixelinfo* dpi, rct_window& w)
{
    ViewportRender(dpi, w.viewport, { { dpi->x, dpi->y }, { dpi->x + dpi->width, dpi->y + dpi->height } });
}

void WindowSetPosition(rct_window& w, const ScreenCoordsXY& screenCoords)
{
    WindowMovePosition(w, screenCoords - w.windowPos);
}

void WindowMovePosition(rct_window& w, const ScreenCoordsXY& deltaCoords)
{
    if (deltaCoords.x == 0 && deltaCoords.y == 0)
        return;

    // Invalidate old region
    w.Invalidate();

    // Translate window and viewport
    w.windowPos += deltaCoords;
    if (w.viewport != nullptr)
    {
        w.viewport->pos += deltaCoords;
    }

    // Invalidate new region
    w.Invalidate();
}

void WindowResize(rct_window& w, int32_t dw, int32_t dh)
{
    if (dw == 0 && dh == 0)
        return;

    // Invalidate old region
    w.Invalidate();

    // Clamp new size to minimum and maximum
    w.width = std::clamp<int32_t>(w.width + dw, w.min_width, w.max_width);
    w.height = std::clamp<int32_t>(w.height + dh, w.min_height, w.max_height);

    WindowEventResizeCall(&w);
    WindowEventInvalidateCall(&w);

    // Update scroll widgets
    for (auto& scroll : w.scrolls)
    {
        scroll.h_right = WINDOW_SCROLL_UNDEFINED;
        scroll.v_bottom = WINDOW_SCROLL_UNDEFINED;
    }
    WindowUpdateScrollWidgets(w);

    // Invalidate new region
    w.Invalidate();
}

void WindowSetResize(rct_window& w, int32_t minWidth, int32_t minHeight, int32_t maxWidth, int32_t maxHeight)
{
    w.min_width = minWidth;
    w.min_height = minHeight;
    w.max_width = maxWidth;
    w.max_height = maxHeight;

    // Clamp width and height to minimum and maximum
    int32_t width = std::clamp<int32_t>(w.width, std::min(minWidth, maxWidth), std::max(minWidth, maxWidth));
    int32_t height = std::clamp<int32_t>(w.height, std::min(minHeight, maxHeight), std::max(minHeight, maxHeight));

    // Resize window if size has changed
    if (w.width != width || w.height != height)
    {
        w.Invalidate();
        w.width = width;
        w.height = height;
        w.Invalidate();
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
bool ToolSet(const rct_window& w, WidgetIndex widgetIndex, Tool tool)
{
    if (InputTestFlag(INPUT_FLAG_TOOL_ACTIVE))
    {
        if (w.classification == gCurrentToolWidget.window_classification && w.number == gCurrentToolWidget.window_number
            && widgetIndex == gCurrentToolWidget.widget_index)
        {
            ToolCancel();
            return true;
        }

        ToolCancel();
    }

    InputSetFlag(INPUT_FLAG_TOOL_ACTIVE, true);
    InputSetFlag(INPUT_FLAG_4, false);
    InputSetFlag(INPUT_FLAG_6, false);
    gCurrentToolId = tool;
    gCurrentToolWidget.window_classification = w.classification;
    gCurrentToolWidget.window_number = w.number;
    gCurrentToolWidget.widget_index = widgetIndex;
    return false;
}

/**
 *
 *  rct2: 0x006EE281
 */
void ToolCancel()
{
    if (InputTestFlag(INPUT_FLAG_TOOL_ACTIVE))
    {
        InputSetFlag(INPUT_FLAG_TOOL_ACTIVE, false);

        MapInvalidateSelectionRect();
        MapInvalidateMapSelectionTiles();

        // Reset map selection
        gMapSelectFlags = 0;

        if (gCurrentToolWidget.widget_index != -1)
        {
            // Invalidate tool widget
            WidgetInvalidateByNumber(
                gCurrentToolWidget.window_classification, gCurrentToolWidget.window_number, gCurrentToolWidget.widget_index);

            // Abort tool event
            rct_window* w = WindowFindByNumber(gCurrentToolWidget.window_classification, gCurrentToolWidget.window_number);
            if (w != nullptr)
                WindowEventToolAbortCall(w, gCurrentToolWidget.widget_index);
        }
    }
}

void WindowEventCloseCall(rct_window* w)
{
    if (w->event_handlers == nullptr)
        w->OnClose();
    else if (w->event_handlers->close != nullptr)
        w->event_handlers->close(w);
}

void WindowEventMouseUpCall(rct_window* w, WidgetIndex widgetIndex)
{
    if (w->event_handlers == nullptr)
        w->OnMouseUp(widgetIndex);
    else if (w->event_handlers->mouse_up != nullptr)
        w->event_handlers->mouse_up(w, widgetIndex);
}

void WindowEventResizeCall(rct_window* w)
{
    if (w->event_handlers == nullptr)
        w->OnResize();
    else if (w->event_handlers->resize != nullptr)
        w->event_handlers->resize(w);
}

void WindowEventMouseDownCall(rct_window* w, WidgetIndex widgetIndex)
{
    if (w->event_handlers == nullptr)
        w->OnMouseDown(widgetIndex);
    else if (w->event_handlers->mouse_down != nullptr)
        w->event_handlers->mouse_down(w, widgetIndex, &w->widgets[widgetIndex]);
}

void WindowEventDropdownCall(rct_window* w, WidgetIndex widgetIndex, int32_t dropdownIndex)
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

void WindowEventUnknown05Call(rct_window* w)
{
    if (w->event_handlers == nullptr)
    {
        w->OnUnknown5();
    }
    else if (w->event_handlers->unknown_05 != nullptr)
    {
        w->event_handlers->unknown_05(w);
    }
}

void WindowEventUpdateCall(rct_window* w)
{
    if (w->event_handlers == nullptr)
        w->OnUpdate();
    else if (w->event_handlers->update != nullptr)
        w->event_handlers->update(w);
}

void WindowEventPeriodicUpdateCall(rct_window* w)
{
    if (w->event_handlers == nullptr)
        w->OnPeriodicUpdate();
    else if (w->event_handlers->periodic_update != nullptr)
        w->event_handlers->periodic_update(w);
}

void WindowEventToolUpdateCall(rct_window* w, WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords)
{
    if (w->event_handlers == nullptr)
        w->OnToolUpdate(widgetIndex, screenCoords);
    else if (w->event_handlers->tool_update != nullptr)
        w->event_handlers->tool_update(w, widgetIndex, screenCoords);
}

void WindowEventToolDownCall(rct_window* w, WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords)
{
    if (w->event_handlers == nullptr)
        w->OnToolDown(widgetIndex, screenCoords);
    else if (w->event_handlers->tool_down != nullptr)
        w->event_handlers->tool_down(w, widgetIndex, screenCoords);
}

void WindowEventToolDragCall(rct_window* w, WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords)
{
    if (w->event_handlers == nullptr)
        w->OnToolDrag(widgetIndex, screenCoords);
    else if (w->event_handlers->tool_drag != nullptr)
        w->event_handlers->tool_drag(w, widgetIndex, screenCoords);
}

void WindowEventToolUpCall(rct_window* w, WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords)
{
    if (w->event_handlers == nullptr)
        w->OnToolUp(widgetIndex, screenCoords);
    else if (w->event_handlers->tool_up != nullptr)
        w->event_handlers->tool_up(w, widgetIndex, screenCoords);
}

void WindowEventToolAbortCall(rct_window* w, WidgetIndex widgetIndex)
{
    if (w->event_handlers == nullptr)
        w->OnToolAbort(widgetIndex);
    else if (w->event_handlers->tool_abort != nullptr)
        w->event_handlers->tool_abort(w, widgetIndex);
}

void WindowGetScrollSize(rct_window* w, int32_t scrollIndex, int32_t* width, int32_t* height)
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

void WindowEventScrollMousedownCall(rct_window* w, int32_t scrollIndex, const ScreenCoordsXY& screenCoords)
{
    if (w->event_handlers == nullptr)
        w->OnScrollMouseDown(scrollIndex, screenCoords);
    else if (w->event_handlers->scroll_mousedown != nullptr)
        w->event_handlers->scroll_mousedown(w, scrollIndex, screenCoords);
}

void WindowEventScrollMousedragCall(rct_window* w, int32_t scrollIndex, const ScreenCoordsXY& screenCoords)
{
    if (w->event_handlers == nullptr)
        w->OnScrollMouseDrag(scrollIndex, screenCoords);
    else if (w->event_handlers->scroll_mousedrag != nullptr)
        w->event_handlers->scroll_mousedrag(w, scrollIndex, screenCoords);
}

void WindowEventScrollMouseoverCall(rct_window* w, int32_t scrollIndex, const ScreenCoordsXY& screenCoords)
{
    if (w->event_handlers == nullptr)
        w->OnScrollMouseOver(scrollIndex, screenCoords);
    else if (w->event_handlers->scroll_mouseover != nullptr)
        w->event_handlers->scroll_mouseover(w, scrollIndex, screenCoords);
}

void WindowEventTextinputCall(rct_window* w, WidgetIndex widgetIndex, char* text)
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

void WindowEventViewportRotateCall(rct_window* w)
{
    if (w->event_handlers == nullptr)
        w->OnViewportRotate();
    else if (w->event_handlers != nullptr)
        if (w->event_handlers->viewport_rotate != nullptr)
            w->event_handlers->viewport_rotate(w);
}

void WindowEventScrollSelectCall(rct_window* w, int32_t scrollIndex, int32_t scrollAreaType)
{
    if (w->event_handlers == nullptr)
        w->OnScrollSelect(scrollIndex, scrollAreaType);
    else if (w->event_handlers->scroll_select != nullptr)
        w->event_handlers->scroll_select(w, scrollIndex, scrollAreaType);
}

OpenRCT2String WindowEventTooltipCall(rct_window* w, const WidgetIndex widgetIndex, const StringId fallback)
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

CursorID WindowEventCursorCall(rct_window* w, WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords)
{
    CursorID cursorId = CursorID::Arrow;
    if (w->event_handlers == nullptr)
    {
        cursorId = w->OnCursor(widgetIndex, screenCoords, cursorId);
    }
    else if (w->event_handlers->cursor != nullptr)
        w->event_handlers->cursor(w, widgetIndex, screenCoords, &cursorId);
    return cursorId;
}

void WindowEventMovedCall(rct_window* w, const ScreenCoordsXY& screenCoords)
{
    if (w->event_handlers == nullptr)
    {
        w->OnMoved(screenCoords);
    }
    else if (w->event_handlers->moved != nullptr)
        w->event_handlers->moved(w, screenCoords);
}

void WindowEventInvalidateCall(rct_window* w)
{
    if (w->event_handlers == nullptr)
        w->OnPrepareDraw();
    else if (w->event_handlers->invalidate != nullptr)
        w->event_handlers->invalidate(w);
}

void WindowEventPaintCall(rct_window* w, rct_drawpixelinfo* dpi)
{
    if (w->event_handlers == nullptr)
        w->OnDraw(*dpi);
    else if (w->event_handlers->paint != nullptr)
        w->event_handlers->paint(w, dpi);
}

void WindowEventScrollPaintCall(rct_window* w, rct_drawpixelinfo* dpi, int32_t scrollIndex)
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
void WindowRelocateWindows(int32_t width, int32_t height)
{
    int32_t new_location = 8;
    WindowVisitEach([width, height, &new_location](rct_window* w) {
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
void WindowResizeGui(int32_t width, int32_t height)
{
    WindowResizeGuiScenarioEditor(width, height);
    if (gScreenFlags & SCREEN_FLAGS_EDITOR)
        return;

    rct_window* titleWind = WindowFindByClass(WindowClass::TitleMenu);
    if (titleWind != nullptr)
    {
        titleWind->windowPos.x = (width - titleWind->width) / 2;
        titleWind->windowPos.y = height - 182;
    }

    rct_window* exitWind = WindowFindByClass(WindowClass::TitleExit);
    if (exitWind != nullptr)
    {
        exitWind->windowPos.x = width - 40;
        exitWind->windowPos.y = height - 64;
    }

    rct_window* optionsWind = WindowFindByClass(WindowClass::TitleOptions);
    if (optionsWind != nullptr)
    {
        optionsWind->windowPos.x = width - 80;
    }

    GfxInvalidateScreen();
}

/**
 * rct2: 0x0066F0DD
 */
void WindowResizeGuiScenarioEditor(int32_t width, int32_t height)
{
    auto* mainWind = WindowGetMain();
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

    rct_window* topWind = WindowFindByClass(WindowClass::TopToolbar);
    if (topWind != nullptr)
    {
        topWind->width = std::max(640, width);
    }

    rct_window* bottomWind = WindowFindByClass(WindowClass::BottomToolbar);
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
void WindowCloseConstructionWindows()
{
    WindowCloseByClass(WindowClass::RideConstruction);
    WindowCloseByClass(WindowClass::Footpath);
    WindowCloseByClass(WindowClass::TrackDesignList);
    WindowCloseByClass(WindowClass::TrackDesignPlace);
}

/**
 * Update zoom based volume attenuation for ride music and clear music list.
 *  rct2: 0x006BC348
 */
void WindowUpdateViewportRideMusic()
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

static void window_snap_left(rct_window& w, int32_t proximity)
{
    const auto* mainWindow = WindowGetMain();
    auto wBottom = w.windowPos.y + w.height;
    auto wLeftProximity = w.windowPos.x - (proximity * 2);
    auto wRightProximity = w.windowPos.x + (proximity * 2);
    auto rightMost = INT32_MIN;

    WindowVisitEach([&](rct_window* w2) {
        if (w2 == &w || w2 == mainWindow)
            return;

        auto right = w2->windowPos.x + w2->width;

        if (wBottom < w2->windowPos.y || w.windowPos.y > w2->windowPos.y + w2->height)
            return;

        if (right < wLeftProximity || right > wRightProximity)
            return;

        rightMost = std::max(rightMost, right);
    });

    if (0 >= wLeftProximity && 0 <= wRightProximity)
        rightMost = std::max(rightMost, 0);

    if (rightMost != INT32_MIN)
        w.windowPos.x = rightMost;
}

static void window_snap_top(rct_window& w, int32_t proximity)
{
    const auto* mainWindow = WindowGetMain();
    auto wRight = w.windowPos.x + w.width;
    auto wTopProximity = w.windowPos.y - (proximity * 2);
    auto wBottomProximity = w.windowPos.y + (proximity * 2);
    auto bottomMost = INT32_MIN;

    WindowVisitEach([&](rct_window* w2) {
        if (w2 == &w || w2 == mainWindow)
            return;

        auto bottom = w2->windowPos.y + w2->height;

        if (wRight < w2->windowPos.x || w.windowPos.x > w2->windowPos.x + w2->width)
            return;

        if (bottom < wTopProximity || bottom > wBottomProximity)
            return;

        bottomMost = std::max(bottomMost, bottom);
    });

    if (0 >= wTopProximity && 0 <= wBottomProximity)
        bottomMost = std::max(bottomMost, 0);

    if (bottomMost != INT32_MIN)
        w.windowPos.y = bottomMost;
}

static void window_snap_right(rct_window& w, int32_t proximity)
{
    const auto* mainWindow = WindowGetMain();
    auto wRight = w.windowPos.x + w.width;
    auto wBottom = w.windowPos.y + w.height;
    auto wLeftProximity = wRight - (proximity * 2);
    auto wRightProximity = wRight + (proximity * 2);
    auto leftMost = INT32_MAX;

    WindowVisitEach([&](rct_window* w2) {
        if (w2 == &w || w2 == mainWindow)
            return;

        if (wBottom < w2->windowPos.y || w.windowPos.y > w2->windowPos.y + w2->height)
            return;

        if (w2->windowPos.x < wLeftProximity || w2->windowPos.x > wRightProximity)
            return;

        leftMost = std::min<int32_t>(leftMost, w2->windowPos.x);
    });

    auto screenWidth = ContextGetWidth();
    if (screenWidth >= wLeftProximity && screenWidth <= wRightProximity)
        leftMost = std::min(leftMost, screenWidth);

    if (leftMost != INT32_MAX)
        w.windowPos.x = leftMost - w.width;
}

static void window_snap_bottom(rct_window& w, int32_t proximity)
{
    const auto* mainWindow = WindowGetMain();
    auto wRight = w.windowPos.x + w.width;
    auto wBottom = w.windowPos.y + w.height;
    auto wTopProximity = wBottom - (proximity * 2);
    auto wBottomProximity = wBottom + (proximity * 2);
    auto topMost = INT32_MAX;

    WindowVisitEach([&](rct_window* w2) {
        if (w2 == &w || w2 == mainWindow)
            return;

        if (wRight < w2->windowPos.x || w.windowPos.x > w2->windowPos.x + w2->width)
            return;

        if (w2->windowPos.y < wTopProximity || w2->windowPos.y > wBottomProximity)
            return;

        topMost = std::min<int32_t>(topMost, w2->windowPos.y);
    });

    auto screenHeight = ContextGetHeight();
    if (screenHeight >= wTopProximity && screenHeight <= wBottomProximity)
        topMost = std::min(topMost, screenHeight);

    if (topMost != INT32_MAX)
        w.windowPos.y = topMost - w.height;
}

void WindowMoveAndSnap(rct_window& w, ScreenCoordsXY newWindowCoords, int32_t snapProximity)
{
    auto originalPos = w.windowPos;
    int32_t minY = (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO) ? 1 : TOP_TOOLBAR_HEIGHT + 2;

    newWindowCoords.y = std::clamp(newWindowCoords.y, minY, ContextGetHeight() - 34);

    if (snapProximity > 0)
    {
        w.windowPos = newWindowCoords;

        window_snap_right(w, snapProximity);
        window_snap_bottom(w, snapProximity);
        window_snap_left(w, snapProximity);
        window_snap_top(w, snapProximity);

        if (w.windowPos == originalPos)
            return;

        newWindowCoords = w.windowPos;
        w.windowPos = originalPos;
    }

    WindowSetPosition(w, newWindowCoords);
}

int32_t WindowCanResize(const rct_window& w)
{
    return (w.flags & WF_RESIZABLE) && (w.min_width != w.max_width || w.min_height != w.max_height);
}

/**
 *
 *  rct2: 0x006EE3C3
 */
void TextinputCancel()
{
    WindowCloseByClass(WindowClass::Textinput);
}

void WindowStartTextbox(
    rct_window& call_w, WidgetIndex call_widget, StringId existing_text, char* existing_args, int32_t maxLength)
{
    if (gUsingWidgetTextBox)
        WindowCancelTextbox();

    gUsingWidgetTextBox = true;
    gCurrentTextBox.window.classification = call_w.classification;
    gCurrentTextBox.window.number = call_w.number;
    gCurrentTextBox.widget_index = call_widget;
    gTextBoxFrameNo = 0;

    WindowCloseByClass(WindowClass::Textinput);

    // Clear the text input buffer
    std::fill_n(gTextBoxInput, maxLength, 0x00);

    // Enter in the text input buffer any existing
    // text.
    if (existing_text != STR_NONE)
        OpenRCT2::FormatStringLegacy(gTextBoxInput, TEXT_INPUT_SIZE, existing_text, &existing_args);

    // In order to prevent strings that exceed the maxLength
    // from crashing the game.
    gTextBoxInput[maxLength - 1] = '\0';

    gTextInput = ContextStartTextInput(gTextBoxInput, maxLength);
}

void WindowCancelTextbox()
{
    if (gUsingWidgetTextBox)
    {
        rct_window* w = WindowFindByNumber(gCurrentTextBox.window.classification, gCurrentTextBox.window.number);
        if (w != nullptr)
        {
            WindowEventTextinputCall(w, gCurrentTextBox.widget_index, nullptr);
        }
        gCurrentTextBox.window.classification = WindowClass::Null;
        gCurrentTextBox.window.number = 0;
        ContextStopTextInput();
        gUsingWidgetTextBox = false;
        if (w != nullptr)
        {
            WidgetInvalidate(*w, gCurrentTextBox.widget_index);
        }
        gCurrentTextBox.widget_index = static_cast<uint16_t>(WindowWidgetType::Last);
    }
}

void WindowUpdateTextboxCaret()
{
    gTextBoxFrameNo++;
    if (gTextBoxFrameNo > 30)
        gTextBoxFrameNo = 0;
}

void WindowUpdateTextbox()
{
    if (gUsingWidgetTextBox)
    {
        gTextBoxFrameNo = 0;
        rct_window* w = WindowFindByNumber(gCurrentTextBox.window.classification, gCurrentTextBox.window.number);
        WidgetInvalidate(*w, gCurrentTextBox.widget_index);
        WindowEventTextinputCall(w, gCurrentTextBox.widget_index, gTextBoxInput);
    }
}

bool WindowIsVisible(rct_window& w)
{
    // w->visibility is used to prevent repeat calculations within an iteration by caching the result

    if (w.visibility == VisibilityCache::Visible)
        return true;
    if (w.visibility == VisibilityCache::Covered)
        return false;

    // only consider viewports, consider the main window always visible
    if (w.viewport == nullptr || w.classification == WindowClass::MainWindow)
    {
        // default to previous behaviour
        w.visibility = VisibilityCache::Visible;
        return true;
    }

    // start from the window above the current
    auto itPos = WindowGetIterator(&w);
    for (auto it = std::next(itPos); it != g_window_list.end(); it++)
    {
        auto& w_other = *(*it);

        // if covered by a higher window, no rendering needed
        if (w_other.windowPos.x <= w.windowPos.x && w_other.windowPos.y <= w.windowPos.y
            && w_other.windowPos.x + w_other.width >= w.windowPos.x + w.width
            && w_other.windowPos.y + w_other.height >= w.windowPos.y + w.height)
        {
            w.visibility = VisibilityCache::Covered;
            w.viewport->visibility = VisibilityCache::Covered;
            return false;
        }
    }

    // default to previous behaviour
    w.visibility = VisibilityCache::Visible;
    w.viewport->visibility = VisibilityCache::Visible;
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
void WindowDrawAll(rct_drawpixelinfo* dpi, int32_t left, int32_t top, int32_t right, int32_t bottom)
{
    auto windowDPI = dpi->Crop({ left, top }, { right - left, bottom - top });
    WindowVisitEach([&windowDPI, left, top, right, bottom](rct_window* w) {
        if (w->flags & WF_TRANSPARENT)
            return;
        if (right <= w->windowPos.x || bottom <= w->windowPos.y)
            return;
        if (left >= w->windowPos.x + w->width || top >= w->windowPos.y + w->height)
            return;
        WindowDraw(&windowDPI, *w, left, top, right, bottom);
    });
}

rct_viewport* WindowGetPreviousViewport(rct_viewport* current)
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

void WindowResetVisibilities()
{
    // reset window visibility status to unknown
    WindowVisitEach([](rct_window* w) {
        w->visibility = VisibilityCache::Unknown;
        if (w->viewport != nullptr)
        {
            w->viewport->visibility = VisibilityCache::Unknown;
        }
    });
}

void WindowInitAll()
{
    WindowCloseAllExceptFlags(0);
}

void WindowFollowSprite(rct_window& w, EntityId spriteIndex)
{
    if (spriteIndex.ToUnderlying() < MAX_ENTITIES || spriteIndex.IsNull())
    {
        w.viewport_smart_follow_sprite = spriteIndex;
    }
}

void WindowUnfollowSprite(rct_window& w)
{
    w.viewport_smart_follow_sprite = EntityId::GetNull();
    w.viewport_target_sprite = EntityId::GetNull();
}

rct_viewport* WindowGetViewport(rct_window* w)
{
    if (w == nullptr)
    {
        return nullptr;
    }

    return w->viewport;
}

rct_window* WindowGetListening()
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

WindowClass WindowGetClassification(const rct_window& window)
{
    return window.classification;
}

/**
 *
 *  rct2: 0x006EAF26
 */
void WidgetScrollUpdateThumbs(rct_window& w, WidgetIndex widget_index)
{
    const auto& widget = w.widgets[widget_index];
    auto& scroll = w.scrolls[WindowGetScrollDataIndex(w, widget_index)];

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

void rct_window::ResizeFrame()
{
    // Frame
    widgets[0].right = width - 1;
    widgets[0].bottom = height - 1;
    // Title
    widgets[1].right = width - 2;
    // Close button
    widgets[2].left = width - 13;
    widgets[2].right = width - 3;
}

void rct_window::ResizeFrameWithPage()
{
    ResizeFrame();
    // Page background
    widgets[3].right = width - 1;
    widgets[3].bottom = height - 1;
}
