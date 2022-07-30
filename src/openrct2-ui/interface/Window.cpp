/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Window.h"

#include "Theme.h"

#include <SDL.h>
#include <algorithm>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/Context.h>
#include <openrct2/Input.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/audio/audio.h>
#include <openrct2/config/Config.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/entity/EntityRegistry.h>
#include <openrct2/interface/Widget.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/localisation/StringIds.h>
#include <openrct2/sprites.h>
#include <openrct2/ui/UiContext.h>

using namespace OpenRCT2;

// The amount of pixels to scroll per wheel click
constexpr int32_t WindowScrollPixels = 17;

static int32_t _previousAbsoluteWheel = 0;

static bool WindowFitsBetweenOthers(const ScreenCoordsXY& loc, int32_t width, int32_t height)
{
    for (auto& w : g_window_list)
    {
        if (w->flags & WF_STICK_TO_BACK)
            continue;

        if (loc.x + width <= w->windowPos.x)
            continue;
        if (loc.x >= w->windowPos.x + w->width)
            continue;
        if (loc.y + height <= w->windowPos.y)
            continue;
        if (loc.y >= w->windowPos.y + w->height)
            continue;
        return false;
    }

    return true;
}

static bool WindowFitsWithinSpace(const ScreenCoordsXY& loc, int32_t width, int32_t height)
{
    if (loc.x < 0)
        return false;
    if (loc.y <= TOP_TOOLBAR_HEIGHT && !(gScreenFlags & SCREEN_FLAGS_TITLE_DEMO))
        return false;
    if (loc.x + width > context_get_width())
        return false;
    if (loc.y + height > context_get_height())
        return false;
    return WindowFitsBetweenOthers(loc, width, height);
}

static bool WindowFitsOnScreen(const ScreenCoordsXY& loc, int32_t width, int32_t height)
{
    uint16_t screenWidth = context_get_width();
    uint16_t screenHeight = context_get_height();
    int32_t unk;

    unk = -(width / 4);
    if (loc.x < unk)
        return false;
    unk = screenWidth + (unk * 2);
    if (loc.x > unk)
        return false;
    if (loc.y <= TOP_TOOLBAR_HEIGHT && !(gScreenFlags & SCREEN_FLAGS_TITLE_DEMO))
        return false;
    unk = screenHeight - (height / 4);
    if (loc.y > unk)
        return false;
    return WindowFitsBetweenOthers(loc, width, height);
}

static ScreenCoordsXY ClampWindowToScreen(
    const ScreenCoordsXY& pos, const int32_t screenWidth, const int32_t screenHeight, const int32_t width, const int32_t height)
{
    auto screenPos = pos;
    if (width > screenWidth || screenPos.x < 0)
        screenPos.x = 0;
    else if (screenPos.x + width > screenWidth)
        screenPos.x = screenWidth - width;

    auto toolbarAllowance = (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO) ? 0 : (TOP_TOOLBAR_HEIGHT + 1);
    if (height - toolbarAllowance > screenHeight || screenPos.y < toolbarAllowance)
        screenPos.y = toolbarAllowance;
    else if (screenPos.y + height - toolbarAllowance > screenHeight)
        screenPos.y = screenHeight + toolbarAllowance - height;

    return screenPos;
}

static ScreenCoordsXY GetAutoPositionForNewWindow(int32_t width, int32_t height)
{
    auto uiContext = GetContext()->GetUiContext();
    auto screenWidth = uiContext->GetWidth();
    auto screenHeight = uiContext->GetHeight();

    // Place window in an empty corner of the screen
    const ScreenCoordsXY cornerPositions[] = {
        { 0, 30 },                                           // topLeft
        { screenWidth - width, 30 },                         // topRight
        { 0, screenHeight - 34 - height },                   // bottomLeft
        { screenWidth - width, screenHeight - 34 - height }, // bottomRight
    };

    for (const auto& cornerPos : cornerPositions)
    {
        if (WindowFitsWithinSpace(cornerPos, width, height))
        {
            return ClampWindowToScreen(cornerPos, screenWidth, screenHeight, width, height);
        }
    }

    // Place window next to another
    for (auto& w : g_window_list)
    {
        if (w->flags & WF_STICK_TO_BACK)
            continue;

        const ScreenCoordsXY offsets[] = {
            { w->width + 2, 0 },
            { -w->width - 2, 0 },
            { 0, w->height + 2 },
            { 0, -w->height - 2 },
            { w->width + 2, -w->height - 2 },
            { -w->width - 2, -w->height - 2 },
            { w->width + 2, w->height + 2 },
            { -w->width - 2, w->height + 2 },
        };

        for (const auto& offset : offsets)
        {
            auto screenPos = w->windowPos + offset;
            if (WindowFitsWithinSpace(screenPos, width, height))
            {
                return ClampWindowToScreen(screenPos, screenWidth, screenHeight, width, height);
            }
        }
    }

    // Overlap
    for (auto& w : g_window_list)
    {
        if (w->flags & WF_STICK_TO_BACK)
            continue;

        const ScreenCoordsXY offsets[] = {
            { w->width + 2, 0 },
            { -w->width - 2, 0 },
            { 0, w->height + 2 },
            { 0, -w->height - 2 },
        };

        for (const auto& offset : offsets)
        {
            auto screenPos = w->windowPos + offset;
            if (WindowFitsOnScreen(screenPos, width, height))
            {
                return ClampWindowToScreen(screenPos, screenWidth, screenHeight, width, height);
            }
        }
    }

    // Cascade
    auto screenPos = ScreenCoordsXY{ 0, 30 };
    for (auto& w : g_window_list)
    {
        if (screenPos == w->windowPos)
        {
            screenPos.x += 5;
            screenPos.y += 5;
        }
    }

    return ClampWindowToScreen(screenPos, screenWidth, screenHeight, width, height);
}

static ScreenCoordsXY GetCentrePositionForNewWindow(int32_t width, int32_t height)
{
    auto uiContext = GetContext()->GetUiContext();
    auto screenWidth = uiContext->GetWidth();
    auto screenHeight = uiContext->GetHeight();
    return ScreenCoordsXY{ (screenWidth - width) / 2, std::max(TOP_TOOLBAR_HEIGHT + 1, (screenHeight - height) / 2) };
}

rct_window* WindowCreate(
    std::unique_ptr<rct_window>&& wp, rct_windowclass cls, ScreenCoordsXY pos, int32_t width, int32_t height, uint32_t flags)
{
    if (flags & WF_AUTO_POSITION)
    {
        if (flags & WF_CENTRE_SCREEN)
        {
            pos = GetCentrePositionForNewWindow(width, height);
        }
        else
        {
            pos = GetAutoPositionForNewWindow(width, height);
        }
    }

    // Check if there are any window slots left
    // include WINDOW_LIMIT_RESERVED for items such as the main viewport and toolbars to not appear to be counted.
    if (g_window_list.size() >= static_cast<size_t>(gConfigGeneral.window_limit + WINDOW_LIMIT_RESERVED))
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
    auto itDestPos = g_window_list.end();
    if (flags & WF_STICK_TO_BACK)
    {
        for (auto it = g_window_list.begin(); it != g_window_list.end(); it++)
        {
            if (!((*it)->flags & WF_STICK_TO_BACK))
            {
                itDestPos = it;
            }
        }
    }
    else if (!(flags & WF_STICK_TO_FRONT))
    {
        for (auto it = g_window_list.rbegin(); it != g_window_list.rend(); it++)
        {
            if (!((*it)->flags & WF_STICK_TO_FRONT))
            {
                itDestPos = it.base();
                break;
            }
        }
    }

    auto itNew = g_window_list.insert(itDestPos, std::move(wp));
    auto w = itNew->get();

    // Setup window
    w->classification = cls;
    w->flags = flags;

    // Play sounds and flash the window
    if (!(flags & (WF_STICK_TO_BACK | WF_STICK_TO_FRONT)))
    {
        w->flags |= WF_WHITE_BORDER_MASK;
        OpenRCT2::Audio::Play(OpenRCT2::Audio::SoundId::WindowOpen, 0, pos.x + (width / 2));
    }

    w->windowPos = pos;
    w->width = width;
    w->height = height;
    w->min_width = width;
    w->max_width = width;
    w->min_height = height;
    w->max_height = height;

    w->focus = std::nullopt;
    w->page = 0;
    w->var_48C = 0;
    w->var_492 = 0;

    ColourSchemeUpdate(w);
    w->Invalidate();
    w->OnOpen();
    return w;
}

rct_window* WindowCreate(
    const ScreenCoordsXY& pos, int32_t width, int32_t height, rct_window_event_list* event_handlers, rct_windowclass cls,
    uint32_t flags)
{
    auto w = std::make_unique<rct_window>();
    w->event_handlers = event_handlers;
    return WindowCreate(std::move(w), cls, pos, width, height, flags);
}

rct_window* WindowCreateAutoPos(
    int32_t width, int32_t height, rct_window_event_list* event_handlers, rct_windowclass cls, uint32_t flags)
{
    auto pos = GetAutoPositionForNewWindow(width, height);
    return WindowCreate(pos, width, height, event_handlers, cls, flags);
}

rct_window* WindowCreateCentred(
    int32_t width, int32_t height, rct_window_event_list* event_handlers, rct_windowclass cls, uint32_t flags)
{
    auto pos = GetCentrePositionForNewWindow(width, height);
    return WindowCreate(pos, width, height, event_handlers, cls, flags);
}

static int32_t WindowGetWidgetIndex(const rct_window& w, rct_widget* widget)
{
    int32_t i = 0;
    for (rct_widget* widget2 = w.widgets; widget2->type != WindowWidgetType::Last; widget2++, i++)
        if (widget == widget2)
            return i;
    return -1;
}

static int32_t WindowGetScrollIndex(const rct_window& w, int32_t targetWidgetIndex)
{
    if (w.widgets[targetWidgetIndex].type != WindowWidgetType::Scroll)
        return -1;

    int32_t scrollIndex = 0;
    rct_widgetindex widgetIndex = 0;
    for (rct_widget* widget = w.widgets; widget->type != WindowWidgetType::Last; widget++, widgetIndex++)
    {
        if (widgetIndex == targetWidgetIndex)
            break;
        if (widget->type == WindowWidgetType::Scroll)
            scrollIndex++;
    }

    return scrollIndex;
}

static rct_widget* WindowGetScrollWidget(const rct_window& w, int32_t scrollIndex)
{
    for (rct_widget* widget = w.widgets; widget->type != WindowWidgetType::Last; widget++)
    {
        if (widget->type != WindowWidgetType::Scroll)
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
static void WindowScrollWheelInput(rct_window& w, int32_t scrollIndex, int32_t wheel)
{
    auto& scroll = w.scrolls[scrollIndex];
    rct_widget* widget = WindowGetScrollWidget(w, scrollIndex);
    rct_widgetindex widgetIndex = WindowGetWidgetIndex(w, widget);

    if (scroll.flags & VSCROLLBAR_VISIBLE)
    {
        int32_t size = widget->height() - 1;
        if (scroll.flags & HSCROLLBAR_VISIBLE)
            size -= 11;
        size = std::max(0, scroll.v_bottom - size);
        scroll.v_top = std::min(std::max(0, scroll.v_top + wheel), size);
    }
    else
    {
        int32_t size = widget->width() - 1;
        if (scroll.flags & VSCROLLBAR_VISIBLE)
            size -= 11;
        size = std::max(0, scroll.h_right - size);
        scroll.h_left = std::min(std::max(0, scroll.h_left + wheel), size);
    }

    WidgetScrollUpdateThumbs(&w, widgetIndex);
    widget_invalidate(&w, widgetIndex);
}

/**
 *
 *  rct2: 0x006E793B
 */
static int32_t WindowWheelInput(rct_window& w, int32_t wheel)
{
    int32_t i = 0;
    for (rct_widget* widget = w.widgets; widget->type != WindowWidgetType::Last; widget++)
    {
        if (widget->type != WindowWidgetType::Scroll)
            continue;

        // Originally always checked first scroll view, bug maybe?
        const auto& scroll = w.scrolls[i];
        if (scroll.flags & (HSCROLLBAR_VISIBLE | VSCROLLBAR_VISIBLE))
        {
            WindowScrollWheelInput(w, i, wheel);
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
static void WindowViewportWheelInput(rct_window& w, int32_t wheel)
{
    if (gScreenFlags & (SCREEN_FLAGS_TRACK_MANAGER | SCREEN_FLAGS_TITLE_DEMO))
        return;

    if (wheel < 0)
        window_zoom_in(&w, true);
    else if (wheel > 0)
        window_zoom_out(&w, true);
}

static bool WindowOtherWheelInput(rct_window& w, rct_widgetindex widgetIndex, int32_t wheel)
{
    // HACK: Until we have a new window system that allows us to add new events like mouse wheel easily,
    //       this selective approach will have to do.

    // Allow mouse wheel scrolling to increment or decrement the land tool size for various windows
    auto widgetType = w.widgets[widgetIndex].type;

    // Lower widgetIndex once or twice we got a type that matches, to allow scrolling on the increase/decrease buttons too
    int32_t attempts = 0;
    while (widgetType != WindowWidgetType::ImgBtn && widgetType != WindowWidgetType::Spinner && widgetIndex > 0)
    {
        switch (widgetType)
        {
            case WindowWidgetType::TrnBtn: // + and - for preview widget
            case WindowWidgetType::Button: // + and - for spinner widget
            {
                if (attempts > 0)
                {
                    // Verify that the previous button was of the same type
                    auto previousType = w.widgets[widgetIndex + 1].type;
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
        widgetType = w.widgets[widgetIndex].type;
    }

    rct_widgetindex buttonWidgetIndex;
    WindowWidgetType expectedType;
    uint32_t expectedContent[2];
    switch (widgetType)
    {
        case WindowWidgetType::ImgBtn:
            buttonWidgetIndex = wheel < 0 ? widgetIndex + 2 : widgetIndex + 1;
            expectedType = WindowWidgetType::TrnBtn;
            expectedContent[0] = IMAGE_TYPE_REMAP | SPR_LAND_TOOL_DECREASE;
            expectedContent[1] = IMAGE_TYPE_REMAP | SPR_LAND_TOOL_INCREASE;
            break;
        case WindowWidgetType::Spinner:
            buttonWidgetIndex = wheel < 0 ? widgetIndex + 1 : widgetIndex + 2;
            expectedType = WindowWidgetType::Button;
            expectedContent[0] = STR_NUMERIC_UP;
            expectedContent[1] = STR_NUMERIC_DOWN;
            break;
        default:
            return false;
    }

    if (WidgetIsDisabled(&w, buttonWidgetIndex))
    {
        return false;
    }

    auto button1Type = w.widgets[widgetIndex + 1].type;
    auto button1Image = w.widgets[widgetIndex + 1].image;
    auto button2Type = w.widgets[widgetIndex + 2].type;
    auto button2Image = w.widgets[widgetIndex + 2].image;
    if (button1Type != expectedType || button2Type != expectedType || button1Image != expectedContent[0]
        || button2Image != expectedContent[1])
    {
        return false;
    }

    window_event_mouse_down_call(&w, buttonWidgetIndex);
    return true;
}

/**
 *
 *  rct2: 0x006E7868
 */
void WindowAllWheelInput()
{
    // Get wheel value
    auto cursorState = context_get_cursor_state();
    int32_t absolute_wheel = cursorState->wheel;
    int32_t relative_wheel = absolute_wheel - _previousAbsoluteWheel;
    int32_t pixel_scroll = relative_wheel * WindowScrollPixels;
    _previousAbsoluteWheel = absolute_wheel;

    if (relative_wheel == 0)
        return;

    // Check window cursor is over
    if (!(input_test_flag(INPUT_FLAG_5)))
    {
        rct_window* w = window_find_from_point(cursorState->position);
        if (w != nullptr)
        {
            // Check if main window
            if (w->classification == WC_MAIN_WINDOW || w->classification == WC_VIEWPORT)
            {
                WindowViewportWheelInput(*w, relative_wheel);
                return;
            }

            // Check scroll view, cursor is over
            rct_widgetindex widgetIndex = window_find_widget_from_point(w, cursorState->position);
            if (widgetIndex != -1)
            {
                const auto& widget = w->widgets[widgetIndex];
                if (widget.type == WindowWidgetType::Scroll)
                {
                    int32_t scrollIndex = WindowGetScrollIndex(*w, widgetIndex);
                    const auto& scroll = w->scrolls[scrollIndex];
                    if (scroll.flags & (HSCROLLBAR_VISIBLE | VSCROLLBAR_VISIBLE))
                    {
                        WindowScrollWheelInput(*w, WindowGetScrollIndex(*w, widgetIndex), pixel_scroll);
                        return;
                    }
                }
                else
                {
                    if (WindowOtherWheelInput(*w, widgetIndex, pixel_scroll))
                    {
                        return;
                    }
                }

                // Check other scroll views on window
                if (WindowWheelInput(*w, pixel_scroll))
                    return;
            }
        }
    }
}

void ApplyScreenSaverLockSetting()
{
    gConfigGeneral.disable_screensaver ? SDL_DisableScreenSaver() : SDL_EnableScreenSaver();
}

/**
 * Initialises scroll widgets to their virtual size.
 *  rct2: 0x006EAEB8
 */
void WindowInitScrollWidgets(rct_window& w)
{
    rct_widget* widget;
    int32_t widget_index, scroll_index;
    int32_t width, height;

    widget_index = 0;
    scroll_index = 0;
    for (widget = w.widgets; widget->type != WindowWidgetType::Last; widget++)
    {
        if (widget->type != WindowWidgetType::Scroll)
        {
            widget_index++;
            continue;
        }

        auto& scroll = w.scrolls[scroll_index];
        scroll.flags = 0;
        width = 0;
        height = 0;
        window_get_scroll_size(&w, scroll_index, &width, &height);
        scroll.h_left = 0;
        scroll.h_right = width + 1;
        scroll.v_top = 0;
        scroll.v_bottom = height + 1;

        if (widget->content & SCROLL_HORIZONTAL)
            scroll.flags |= HSCROLLBAR_VISIBLE;
        if (widget->content & SCROLL_VERTICAL)
            scroll.flags |= VSCROLLBAR_VISIBLE;

        WidgetScrollUpdateThumbs(&w, widget_index);

        widget_index++;
        scroll_index++;
    }
}

/**
 *
 *  rct2: 0x006EB15C
 */
void WindowDrawWidgets(rct_window& w, rct_drawpixelinfo* dpi)
{
    rct_widget* widget;
    rct_widgetindex widgetIndex;

    if ((w.flags & WF_TRANSPARENT) && !(w.flags & WF_NO_BACKGROUND))
        gfx_filter_rect(
            dpi, { w.windowPos, w.windowPos + ScreenCoordsXY{ w.width - 1, w.height - 1 } }, FilterPaletteID::Palette51);

    // todo: some code missing here? Between 006EB18C and 006EB260

    widgetIndex = 0;
    for (widget = w.widgets; widget->type != WindowWidgetType::Last; widget++)
    {
        if (widget->IsVisible())
        {
            // Check if widget is outside the draw region
            if (w.windowPos.x + widget->left < dpi->x + dpi->width && w.windowPos.x + widget->right >= dpi->x)
            {
                if (w.windowPos.y + widget->top < dpi->y + dpi->height && w.windowPos.y + widget->bottom >= dpi->y)
                {
                    if (w.IsLegacy())
                        WidgetDraw(dpi, &w, widgetIndex);
                    else
                        w.OnDrawWidget(widgetIndex, *dpi);
                }
            }
        }
        widgetIndex++;
    }

    // todo: something missing here too? Between 006EC32B and 006EC369

    if (w.flags & WF_WHITE_BORDER_MASK)
    {
        gfx_fill_rect_inset(
            dpi, { w.windowPos, w.windowPos + ScreenCoordsXY{ w.width - 1, w.height - 1 } }, COLOUR_WHITE,
            INSET_RECT_FLAG_FILL_NONE);
    }
}

/**
 *
 *  rct2: 0x006EA776
 */
static void WindowInvalidatePressedImageButton(const rct_window& w)
{
    rct_widgetindex widgetIndex;
    rct_widget* widget;

    widgetIndex = 0;
    for (widget = w.widgets; widget->type != WindowWidgetType::Last; widget++, widgetIndex++)
    {
        if (widget->type != WindowWidgetType::ImgBtn)
            continue;

        if (WidgetIsPressed(&w, widgetIndex) || WidgetIsActiveTool(&w, widgetIndex))
            gfx_set_dirty_blocks({ w.windowPos, w.windowPos + ScreenCoordsXY{ w.width, w.height } });
    }
}

/**
 *
 *  rct2: 0x006EA73F
 */
void InvalidateAllWindowsAfterInput()
{
    window_visit_each([](rct_window* w) {
        window_update_scroll_widgets(w);
        WindowInvalidatePressedImageButton(*w);
        window_event_resize_call(w);
    });
}

bool Window::IsLegacy()
{
    return false;
}

void Window::OnDraw(rct_drawpixelinfo& dpi)
{
    WindowDrawWidgets(*this, &dpi);
}

void Window::OnDrawWidget(rct_widgetindex widgetIndex, rct_drawpixelinfo& dpi)
{
    WidgetDraw(&dpi, this, widgetIndex);
}

void Window::InitScrollWidgets()
{
    WindowInitScrollWidgets(*this);
}

void Window::InvalidateWidget(rct_widgetindex widgetIndex)
{
    widget_invalidate(this, widgetIndex);
}

bool Window::IsWidgetDisabled(rct_widgetindex widgetIndex) const
{
    return WidgetIsDisabled(this, widgetIndex);
}

bool Window::IsWidgetPressed(rct_widgetindex widgetIndex) const
{
    return WidgetIsPressed(this, widgetIndex);
}

void Window::SetWidgetDisabled(rct_widgetindex widgetIndex, bool value)
{
    WidgetSetDisabled(this, widgetIndex, value);
}

void Window::SetWidgetPressed(rct_widgetindex widgetIndex, bool value)
{
    WidgetSetPressed(this, widgetIndex, value);
}

void Window::SetCheckboxValue(rct_widgetindex widgetIndex, bool value)
{
    SetWidgetPressed(widgetIndex, value);
}

void Window::DrawWidgets(rct_drawpixelinfo& dpi)
{
    WindowDrawWidgets(*this, &dpi);
}

void Window::Close()
{
    window_close(this);
}

void Window::TextInputOpen(
    rct_widgetindex callWidget, rct_string_id title, rct_string_id description, const Formatter& descriptionArgs,
    rct_string_id existingText, uintptr_t existingArgs, int32_t maxLength)
{
    WindowTextInputOpen(this, callWidget, title, description, descriptionArgs, existingText, existingArgs, maxLength);
}

void window_align_tabs(rct_window* w, rct_widgetindex start_tab_id, rct_widgetindex end_tab_id)
{
    int32_t i, x = w->widgets[start_tab_id].left;
    int32_t tab_width = w->widgets[start_tab_id].width();

    for (i = start_tab_id; i <= end_tab_id; i++)
    {
        if (!WidgetIsDisabled(w, i))
        {
            auto& widget = w->widgets[i];
            widget.left = x;
            widget.right = x + tab_width;
            x += tab_width + 1;
        }
    }
}
