/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Window.h"

#include "Theme.h"
#include "Widget.h"
#include "openrct2/world/Location.hpp"

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
        if (w->flags & WF_DEAD)
            continue;
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
    if (loc.y <= kTopToolbarHeight && !(gScreenFlags & SCREEN_FLAGS_TITLE_DEMO))
        return false;
    if (loc.x + width > ContextGetWidth())
        return false;
    if (loc.y + height > ContextGetHeight())
        return false;
    return WindowFitsBetweenOthers(loc, width, height);
}

static bool WindowFitsOnScreen(const ScreenCoordsXY& loc, int32_t width, int32_t height)
{
    uint16_t screenWidth = ContextGetWidth();
    uint16_t screenHeight = ContextGetHeight();
    int32_t unk;

    unk = -(width / 4);
    if (loc.x < unk)
        return false;
    unk = screenWidth + (unk * 2);
    if (loc.x > unk)
        return false;
    if (loc.y <= kTopToolbarHeight && !(gScreenFlags & SCREEN_FLAGS_TITLE_DEMO))
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

    auto toolbarAllowance = (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO) ? 0 : (kTopToolbarHeight + 1);
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
        if (w->flags & WF_DEAD)
            continue;
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
        if (w->flags & WF_DEAD)
            continue;
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
    return ScreenCoordsXY{ (screenWidth - width) / 2, std::max(kTopToolbarHeight + 1, (screenHeight - height) / 2) };
}

WindowBase* WindowCreate(
    std::unique_ptr<WindowBase>&& wp, WindowClass cls, ScreenCoordsXY pos, int32_t width, int32_t height, uint32_t flags)
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
    if (g_window_list.size() >= static_cast<size_t>(gConfigGeneral.WindowLimit + WINDOW_LIMIT_RESERVED))
    {
        // Close least recently used window
        for (auto& w : g_window_list)
        {
            if (w->flags & WF_DEAD)
                continue;
            if (!(w->flags & (WF_STICK_TO_BACK | WF_STICK_TO_FRONT | WF_NO_AUTO_CLOSE)))
            {
                WindowClose(*w.get());
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
            if ((*it)->flags & WF_DEAD)
                continue;
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
            if ((*it)->flags & WF_DEAD)
                continue;
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

    ColourSchemeUpdate(w);
    w->Invalidate();
    w->OnOpen();
    return w;
}

static int32_t WindowGetWidgetIndex(const WindowBase& w, Widget* widget)
{
    int32_t i = 0;
    for (Widget* widget2 = w.widgets; widget2->type != WindowWidgetType::Last; widget2++, i++)
        if (widget == widget2)
            return i;
    return -1;
}

static int32_t WindowGetScrollIndex(const WindowBase& w, int32_t targetWidgetIndex)
{
    if (w.widgets[targetWidgetIndex].type != WindowWidgetType::Scroll)
        return -1;

    int32_t scrollIndex = 0;
    WidgetIndex widgetIndex = 0;
    for (Widget* widget = w.widgets; widget->type != WindowWidgetType::Last; widget++, widgetIndex++)
    {
        if (widgetIndex == targetWidgetIndex)
            break;
        if (widget->type == WindowWidgetType::Scroll)
            scrollIndex++;
    }

    return scrollIndex;
}

static Widget* WindowGetScrollWidget(const WindowBase& w, int32_t scrollIndex)
{
    for (Widget* widget = w.widgets; widget->type != WindowWidgetType::Last; widget++)
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
static void WindowScrollWheelInput(WindowBase& w, int32_t scrollIndex, int32_t wheel)
{
    auto& scroll = w.scrolls[scrollIndex];
    Widget* widget = WindowGetScrollWidget(w, scrollIndex);
    WidgetIndex widgetIndex = WindowGetWidgetIndex(w, widget);

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

    WidgetScrollUpdateThumbs(w, widgetIndex);
    WidgetInvalidate(w, widgetIndex);
}

/**
 *
 *  rct2: 0x006E793B
 */
static int32_t WindowWheelInput(WindowBase& w, int32_t wheel)
{
    int32_t i = 0;
    for (Widget* widget = w.widgets; widget->type != WindowWidgetType::Last; widget++)
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
static void WindowViewportWheelInput(WindowBase& w, int32_t wheel)
{
    if (gScreenFlags & (SCREEN_FLAGS_TRACK_MANAGER | SCREEN_FLAGS_TITLE_DEMO))
        return;

    if (wheel < 0)
        WindowZoomIn(w, true);
    else if (wheel > 0)
        WindowZoomOut(w, true);
}

static bool WindowOtherWheelInput(WindowBase& w, WidgetIndex widgetIndex, int32_t wheel)
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

    WidgetIndex buttonWidgetIndex;
    WindowWidgetType expectedType;

    switch (widgetType)
    {
        case WindowWidgetType::ImgBtn:
        {
            auto expectedContent1 = ImageId(SPR_LAND_TOOL_DECREASE, FilterPaletteID::PaletteNull);
            auto expectedContent2 = ImageId(SPR_LAND_TOOL_INCREASE, FilterPaletteID::PaletteNull);

            auto button1Image = w.widgets[widgetIndex + 1].image;
            auto button2Image = w.widgets[widgetIndex + 2].image;
            if (button1Image != expectedContent1 || button2Image != expectedContent2)
            {
                return false;
            }

            buttonWidgetIndex = wheel < 0 ? widgetIndex + 2 : widgetIndex + 1;
            expectedType = WindowWidgetType::TrnBtn;
            break;
        }
        case WindowWidgetType::Spinner:
        {
            auto button1StringID = w.widgets[widgetIndex + 1].text;
            auto button2StringID = w.widgets[widgetIndex + 2].text;
            if (button1StringID != STR_NUMERIC_UP || button2StringID != STR_NUMERIC_DOWN)
            {
                return false;
            }

            buttonWidgetIndex = wheel < 0 ? widgetIndex + 1 : widgetIndex + 2;
            expectedType = WindowWidgetType::Button;
            break;
        }
        default:
            return false;
    }

    if (WidgetIsDisabled(w, buttonWidgetIndex))
    {
        return false;
    }

    auto button1Type = w.widgets[widgetIndex + 1].type;
    auto button2Type = w.widgets[widgetIndex + 2].type;
    if (button1Type != expectedType || button2Type != expectedType)
    {
        return false;
    }

    w.OnMouseDown(buttonWidgetIndex);
    return true;
}

/**
 *
 *  rct2: 0x006E7868
 */
void WindowAllWheelInput()
{
    // Get wheel value
    auto cursorState = ContextGetCursorState();
    int32_t absolute_wheel = cursorState->wheel;
    int32_t relative_wheel = absolute_wheel - _previousAbsoluteWheel;
    int32_t pixel_scroll = relative_wheel * WindowScrollPixels;
    _previousAbsoluteWheel = absolute_wheel;

    if (relative_wheel == 0)
        return;

    // Check window cursor is over
    if (!(InputTestFlag(INPUT_FLAG_5)))
    {
        WindowBase* w = WindowFindFromPoint(cursorState->position);
        if (w != nullptr)
        {
            // Check if main window
            if (w->classification == WindowClass::MainWindow || w->classification == WindowClass::Viewport)
            {
                WindowViewportWheelInput(*w, relative_wheel);
                return;
            }

            // Check scroll view, cursor is over
            WidgetIndex widgetIndex = WindowFindWidgetFromPoint(*w, cursorState->position);
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
    gConfigGeneral.DisableScreensaver ? SDL_DisableScreenSaver() : SDL_EnableScreenSaver();
}

/**
 * Initialises scroll widgets to their virtual size.
 *  rct2: 0x006EAEB8
 */
void WindowInitScrollWidgets(WindowBase& w)
{
    Widget* widget;
    int32_t widget_index, scroll_index;

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
        ScreenSize scrollSize = w.OnScrollGetSize(scroll_index);
        scroll.h_left = 0;
        scroll.h_right = scrollSize.width + 1;
        scroll.v_top = 0;
        scroll.v_bottom = scrollSize.height + 1;

        if (widget->content & SCROLL_HORIZONTAL)
            scroll.flags |= HSCROLLBAR_VISIBLE;
        if (widget->content & SCROLL_VERTICAL)
            scroll.flags |= VSCROLLBAR_VISIBLE;

        WidgetScrollUpdateThumbs(w, widget_index);

        widget_index++;
        scroll_index++;
    }
}

/**
 *
 *  rct2: 0x006EB15C
 */
void WindowDrawWidgets(WindowBase& w, DrawPixelInfo& dpi)
{
    Widget* widget;
    WidgetIndex widgetIndex;

    if ((w.flags & WF_TRANSPARENT) && !(w.flags & WF_NO_BACKGROUND))
        GfxFilterRect(
            dpi, { w.windowPos, w.windowPos + ScreenCoordsXY{ w.width - 1, w.height - 1 } }, FilterPaletteID::Palette51);

    // todo: some code missing here? Between 006EB18C and 006EB260

    widgetIndex = 0;
    for (widget = w.widgets; widget->type != WindowWidgetType::Last; widget++)
    {
        if (widget->IsVisible())
        {
            // Check if widget is outside the draw region
            if (w.windowPos.x + widget->left < dpi.x + dpi.width && w.windowPos.x + widget->right >= dpi.x)
            {
                if (w.windowPos.y + widget->top < dpi.y + dpi.height && w.windowPos.y + widget->bottom >= dpi.y)
                {
                    w.OnDrawWidget(widgetIndex, dpi);
                }
            }
        }
        widgetIndex++;
    }

    // todo: something missing here too? Between 006EC32B and 006EC369

    if (w.flags & WF_WHITE_BORDER_MASK)
    {
        GfxFillRectInset(
            dpi, { w.windowPos, w.windowPos + ScreenCoordsXY{ w.width - 1, w.height - 1 } }, COLOUR_WHITE,
            INSET_RECT_FLAG_FILL_NONE);
    }
}

/**
 *
 *  rct2: 0x006EA776
 */
static void WindowInvalidatePressedImageButton(const WindowBase& w)
{
    WidgetIndex widgetIndex;
    Widget* widget;

    widgetIndex = 0;
    for (widget = w.widgets; widget->type != WindowWidgetType::Last; widget++, widgetIndex++)
    {
        if (widget->type != WindowWidgetType::ImgBtn)
            continue;

        if (WidgetIsPressed(w, widgetIndex) || WidgetIsActiveTool(w, widgetIndex))
            GfxSetDirtyBlocks({ w.windowPos, w.windowPos + ScreenCoordsXY{ w.width, w.height } });
    }
}

/**
 *
 *  rct2: 0x006EA73F
 */
void InvalidateAllWindowsAfterInput()
{
    WindowVisitEach([](WindowBase* w) {
        WindowUpdateScrollWidgets(*w);
        WindowInvalidatePressedImageButton(*w);
        w->OnResize();
    });
}

void Window::OnDraw(DrawPixelInfo& dpi)
{
    WindowDrawWidgets(*this, dpi);
}

void Window::OnDrawWidget(WidgetIndex widgetIndex, DrawPixelInfo& dpi)
{
    WidgetDraw(dpi, *this, widgetIndex);
}

void Window::InitScrollWidgets()
{
    WindowInitScrollWidgets(*this);
}

void Window::InvalidateWidget(WidgetIndex widgetIndex)
{
    WidgetInvalidate(*this, widgetIndex);
}

bool Window::IsWidgetDisabled(WidgetIndex widgetIndex) const
{
    return WidgetIsDisabled(*this, widgetIndex);
}

bool Window::IsWidgetPressed(WidgetIndex widgetIndex) const
{
    return WidgetIsPressed(*this, widgetIndex);
}

void Window::SetWidgetEnabled(WidgetIndex widgetIndex, bool value)
{
    WidgetSetEnabled(*this, widgetIndex, value);
}

void Window::SetWidgetDisabled(WidgetIndex widgetIndex, bool value)
{
    WidgetSetDisabled(*this, widgetIndex, value);
}

void Window::SetWidgetDisabledAndInvalidate(WidgetIndex widgetIndex, bool value)
{
    bool oldState = IsWidgetDisabled(widgetIndex);
    if (oldState != value)
    {
        WidgetSetDisabled(*this, widgetIndex, value);
        InvalidateWidget(widgetIndex);
    }
}

void Window::SetWidgetPressed(WidgetIndex widgetIndex, bool value)
{
    WidgetSetPressed(*this, widgetIndex, value);
}

void Window::SetCheckboxValue(WidgetIndex widgetIndex, bool value)
{
    SetWidgetPressed(widgetIndex, value);
}

void Window::DrawWidgets(DrawPixelInfo& dpi)
{
    WindowDrawWidgets(*this, dpi);
}

void Window::Close()
{
    CloseWindowModifier modifier = GetCloseModifier();

    if (modifier == CloseWindowModifier::Shift)
    {
        CloseOthers();
    }
    else if (modifier == CloseWindowModifier::Control)
    {
        CloseOthersOfThisClass();
    }
    else
    {
        WindowClose(*this);
    }
}

void Window::CloseOthers()
{
    WindowCloseAllExceptNumberAndClass(number, classification);
}

void Window::CloseOthersOfThisClass()
{
    WindowCloseByClass(classification);
}

CloseWindowModifier Window::GetCloseModifier()
{
    CloseWindowModifier lastModifier = CloseWindowModifier::None;

    if (gLastCloseModifier.window.number == number && gLastCloseModifier.window.classification == classification)
    {
        lastModifier = gLastCloseModifier.modifier;
    }

    gLastCloseModifier.modifier = CloseWindowModifier::None;

    return lastModifier;
}

void Window::TextInputOpen(
    WidgetIndex callWidget, StringId title, StringId description, const Formatter& descriptionArgs, StringId existingText,
    uintptr_t existingArgs, int32_t maxLength)
{
    OpenRCT2::Ui::Windows::WindowTextInputOpen(
        this, callWidget, title, description, descriptionArgs, existingText, existingArgs, maxLength);
}

void WindowAlignTabs(WindowBase* w, WidgetIndex start_tab_id, WidgetIndex end_tab_id)
{
    int32_t i, x = w->widgets[start_tab_id].left;
    int32_t tab_width = w->widgets[start_tab_id].width();

    for (i = start_tab_id; i <= end_tab_id; i++)
    {
        auto& widget = w->widgets[i];
        if (!WidgetIsDisabled(*w, i))
        {
            widget.left = x;
            widget.right = x + tab_width;
            x += tab_width + 1;
        }
        else
        {
            // Workaround #20535: Avoid disabled widgets from sharing the same space as active ones, otherwise
            // WindowFindWidgetFromPoint could return the disabled one, causing issues.
            widget.left = 0;
            widget.right = 0;
        }
    }
}

ScreenCoordsXY WindowGetViewportSoundIconPos(WindowBase& w)
{
    const uint8_t buttonOffset = (gConfigInterface.WindowButtonsOnTheLeft) ? CloseButtonWidth + 2 : 0;
    return w.windowPos + ScreenCoordsXY{ 2 + buttonOffset, 2 };
}

namespace OpenRCT2::Ui::Windows
{
    u8string _textBoxInput;
    int32_t _textBoxFrameNo = 0;
    bool _usingWidgetTextBox = false;
    TextInputSession* _textInput;
    WidgetIdentifier _currentTextBox = { { WindowClass::Null, 0 }, 0 };

    WindowBase* WindowGetListening()
    {
        for (auto it = g_window_list.rbegin(); it != g_window_list.rend(); it++)
        {
            auto& w = **it;
            if (w.flags & WF_DEAD)
                continue;

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

    WindowClass WindowGetClassification(const WindowBase& window)
    {
        return window.classification;
    }

    void WindowStartTextbox(const WindowBase& callW, WidgetIndex callWidget, u8string existingText, int32_t maxLength)
    {
        if (_usingWidgetTextBox)
            WindowCancelTextbox();

        _usingWidgetTextBox = true;
        _currentTextBox.window.classification = callW.classification;
        _currentTextBox.window.number = callW.number;
        _currentTextBox.widget_index = callWidget;
        _textBoxFrameNo = 0;

        WindowCloseByClass(WindowClass::Textinput);

        // Enter in the text input buffer any existing
        // text.
        _textBoxInput = existingText;

        _textInput = ContextStartTextInput(_textBoxInput, maxLength);
    }

    void WindowCancelTextbox()
    {
        if (_usingWidgetTextBox)
        {
            WindowBase* w = WindowFindByNumber(_currentTextBox.window.classification, _currentTextBox.window.number);
            _currentTextBox.window.classification = WindowClass::Null;
            _currentTextBox.window.number = 0;
            ContextStopTextInput();
            _usingWidgetTextBox = false;
            if (w != nullptr)
            {
                WidgetInvalidate(*w, _currentTextBox.widget_index);
            }
            _currentTextBox.widget_index = static_cast<uint16_t>(WindowWidgetType::Last);
        }
    }

    void WindowUpdateTextboxCaret()
    {
        _textBoxFrameNo++;
        if (_textBoxFrameNo > 30)
            _textBoxFrameNo = 0;
    }

    void WindowUpdateTextbox()
    {
        if (_usingWidgetTextBox)
        {
            _textBoxFrameNo = 0;
            WindowBase* w = WindowFindByNumber(_currentTextBox.window.classification, _currentTextBox.window.number);
            WidgetInvalidate(*w, _currentTextBox.widget_index);
            w->OnTextInput(_currentTextBox.widget_index, _textBoxInput);
        }
    }
    const TextInputSession* GetTextboxSession()
    {
        return _textInput;
    }
    void SetTexboxSession(TextInputSession* session)
    {
        _textInput = session;
    }
    bool IsUsingWidgetTextBox()
    {
        return _usingWidgetTextBox;
    }
    bool TextBoxCaretIsFlashed()
    {
        return _textBoxFrameNo <= 15;
    }
    const WidgetIdentifier& GetCurrentTextBox()
    {
        return _currentTextBox;
    }
} // namespace OpenRCT2::Ui::Windows
