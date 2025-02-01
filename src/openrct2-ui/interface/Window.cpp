/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Window.h"

#include "../UiStringIds.h"
#include "Theme.h"
#include "Widget.h"

#include <SDL.h>
#include <algorithm>
#include <openrct2-ui/windows/Windows.h>
#include <openrct2/Context.h>
#include <openrct2/GameState.h>
#include <openrct2/Input.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/audio/audio.h>
#include <openrct2/config/Config.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/entity/EntityRegistry.h>
#include <openrct2/interface/Viewport.h>
#include <openrct2/interface/Widget.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/sprites.h>
#include <openrct2/ui/WindowManager.h>
#include <openrct2/world/Location.hpp>

namespace OpenRCT2
{
    using namespace OpenRCT2::Ui;

    // The amount of pixels to scroll per wheel click
    constexpr int32_t kWindowScrollPixels = 17;

    static int32_t _previousAbsoluteWheel = 0;

    static int32_t WindowGetWidgetIndex(const WindowBase& w, Widget* widget)
    {
        const auto it = std::find_if(
            w.widgets.begin(), w.widgets.end(), [&](auto& otherWidget) { return &otherWidget == widget; });
        if (it == w.widgets.end())
            return -1;
        return std::distance(w.widgets.begin(), it);
    }

    static int32_t WindowGetScrollIndex(const WindowBase& w, int32_t targetWidgetIndex)
    {
        if (w.widgets[targetWidgetIndex].type != WindowWidgetType::Scroll)
            return -1;

        int32_t scrollIndex = 0;
        for (WidgetIndex widgetIndex = 0; widgetIndex < w.widgets.size(); widgetIndex++)
        {
            if (widgetIndex == targetWidgetIndex)
                break;
            auto& widget = w.widgets[widgetIndex];
            if (widget.type == WindowWidgetType::Scroll)
                scrollIndex++;
        }

        return scrollIndex;
    }

    static Widget* WindowGetScrollWidget(const WindowBase& w, int32_t scrollIndex)
    {
        for (WidgetIndex widgetIndex = 0; widgetIndex < w.widgets.size(); widgetIndex++)
        {
            auto& widget = w.widgets[widgetIndex];
            if (widget.type != WindowWidgetType::Scroll)
                continue;

            if (scrollIndex == 0)
                return const_cast<Widget*>(&widget);
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
            size = std::max(0, scroll.contentHeight - size);
            scroll.contentOffsetY = std::min(std::max(0, scroll.contentOffsetY + wheel), size);
        }
        else
        {
            int32_t size = widget->width() - 1;
            if (scroll.flags & VSCROLLBAR_VISIBLE)
                size -= 11;
            size = std::max(0, scroll.contentWidth - size);
            scroll.contentOffsetX = std::min(std::max(0, scroll.contentOffsetX + wheel), size);
        }

        WidgetScrollUpdateThumbs(w, widgetIndex);

        auto* windowMgr = Ui::GetWindowManager();
        windowMgr->InvalidateWidget(w, widgetIndex);
    }

    /**
     *
     *  rct2: 0x006E793B
     */
    static int32_t WindowWheelInput(WindowBase& w, int32_t wheel)
    {
        int32_t scrollIndex = 0;
        for (WidgetIndex widgetIndex = 0; widgetIndex < w.widgets.size(); widgetIndex++)
        {
            const auto& widget = w.widgets[widgetIndex];
            if (widget.type != WindowWidgetType::Scroll)
                continue;

            // Originally always checked first scroll view, bug maybe?
            const auto& scroll = w.scrolls[scrollIndex];
            if (scroll.flags & (HSCROLLBAR_VISIBLE | VSCROLLBAR_VISIBLE))
            {
                WindowScrollWheelInput(w, scrollIndex, wheel);
                return 1;
            }
            scrollIndex++;
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
            Windows::WindowZoomIn(w, true);
        else if (wheel > 0)
            Windows::WindowZoomOut(w, true);
    }

    static bool isSpinnerGroup(WindowBase& w, WidgetIndex index, WindowWidgetType buttonType)
    {
        const auto& widgets = w.widgets;

        if (widgets[index].type != WindowWidgetType::Spinner && widgets[index].type != WindowWidgetType::ImgBtn)
            return false;

        if (static_cast<size_t>(index + 2) >= widgets.size())
            return false;

        if (widgets[index + 1].type != buttonType)
            return false;

        if (widgets[index + 2].type != buttonType)
            return false;

        return true;
    }

    static std::optional<WidgetIndex> getSpinnerGroupWidgetIndex(WindowBase& w, WidgetIndex startIndex)
    {
        // We only iterate 3 times as we might be at the spinner or one of its buttons.
        for (WidgetIndex index = 0; index < 3; index++)
        {
            const auto reverseIndex = startIndex - index;
            if (reverseIndex < 0)
            {
                break;
            }

            if (isSpinnerGroup(w, reverseIndex, WindowWidgetType::TrnBtn))
            {
                return reverseIndex;
            }

            if (isSpinnerGroup(w, reverseIndex, WindowWidgetType::Button))
            {
                return reverseIndex;
            }
        }

        return std::nullopt;
    }

    // Allow mouse wheel scrolling to manipulate spinner widgets and tool sizes
    static bool WindowOtherWheelInput(WindowBase& w, WidgetIndex widgetIndex, int32_t wheel)
    {
        // HACK: Until we have a new window system that allows us to add new events like mouse wheel easily,
        //       this selective approach will have to do.

        const auto spinnerGroupIndex = getSpinnerGroupWidgetIndex(w, widgetIndex);
        if (!spinnerGroupIndex.has_value())
        {
            return false;
        }

        const auto entryWidgetType = w.widgets[*spinnerGroupIndex].type;
        auto targetWidgetIndex = *spinnerGroupIndex;

        if (entryWidgetType == WindowWidgetType::ImgBtn)
        {
            auto expectedContent1 = ImageId(SPR_LAND_TOOL_DECREASE, FilterPaletteID::PaletteNull);
            auto expectedContent2 = ImageId(SPR_LAND_TOOL_INCREASE, FilterPaletteID::PaletteNull);

            auto button1Image = w.widgets[*spinnerGroupIndex + 1].image;
            auto button2Image = w.widgets[*spinnerGroupIndex + 2].image;
            if (button1Image != expectedContent1 || button2Image != expectedContent2)
            {
                return false;
            }

            // Expected widget order: decrease, increase
            targetWidgetIndex += wheel < 0 ? 2 : 1;
        }
        else if (entryWidgetType == WindowWidgetType::Spinner)
        {
            auto button1StringId = w.widgets[*spinnerGroupIndex + 1].text;
            auto button2StringId = w.widgets[*spinnerGroupIndex + 2].text;
            if (button1StringId != STR_NUMERIC_UP || button2StringId != STR_NUMERIC_DOWN)
            {
                return false;
            }

            // Expected widget order: increase, decrease
            targetWidgetIndex += wheel < 0 ? 1 : 2;
        }

        assert(targetWidgetIndex >= 0 && targetWidgetIndex < w.widgets.size());

        if (WidgetIsDisabled(w, targetWidgetIndex))
        {
            return false;
        }

        w.OnMouseDown(targetWidgetIndex);
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
        int32_t pixel_scroll = relative_wheel * kWindowScrollPixels;
        _previousAbsoluteWheel = absolute_wheel;

        if (relative_wheel == 0)
            return;

        // Check window cursor is over
        if (!(InputTestFlag(INPUT_FLAG_5)))
        {
            auto* windowMgr = GetWindowManager();
            WindowBase* w = windowMgr->FindFromPoint(cursorState->position);
            if (w != nullptr)
            {
                // Check if main window
                if (w->classification == WindowClass::MainWindow || w->classification == WindowClass::Viewport)
                {
                    WindowViewportWheelInput(*w, relative_wheel);
                    return;
                }

                // Check scroll view, cursor is over
                WidgetIndex widgetIndex = windowMgr->FindWidgetFromPoint(*w, cursorState->position);
                if (widgetIndex != kWidgetIndexNull)
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
        Config::Get().general.DisableScreensaver ? SDL_DisableScreenSaver() : SDL_EnableScreenSaver();
    }

    /**
     *
     *  rct2: 0x006EA776
     */
    static void WindowInvalidatePressedImageButton(const WindowBase& w)
    {
        for (WidgetIndex widgetIndex = 0; widgetIndex < w.widgets.size(); widgetIndex++)
        {
            auto& widget = w.widgets[widgetIndex];
            if (widget.type != WindowWidgetType::ImgBtn)
                continue;

            if (WidgetIsPressed(w, widgetIndex) || isToolActive(w, widgetIndex))
                GfxSetDirtyBlocks({ w.windowPos, w.windowPos + ScreenCoordsXY{ w.width, w.height } });
        }
    }

    void Window::ScrollToViewport()
    {
        if (viewport == nullptr || !focus.has_value())
            return;

        CoordsXYZ newCoords = focus.value().GetPos();

        auto mainWindow = WindowGetMain();
        if (mainWindow != nullptr)
            WindowScrollToLocation(*mainWindow, newCoords);
    }

    void Window::OnDraw(DrawPixelInfo& dpi)
    {
        Windows::WindowDrawWidgets(*this, dpi);
    }

    void Window::OnDrawWidget(WidgetIndex widgetIndex, DrawPixelInfo& dpi)
    {
        WidgetDraw(dpi, *this, widgetIndex);
    }

    void Window::InitScrollWidgets()
    {
        Windows::WindowInitScrollWidgets(*this);
    }

    void Window::InvalidateWidget(WidgetIndex widgetIndex)
    {
        auto* windowMgr = Ui::GetWindowManager();
        windowMgr->InvalidateWidget(*this, widgetIndex);
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
        Windows::WindowDrawWidgets(*this, dpi);
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
            auto* windowMgr = Ui::GetWindowManager();
            windowMgr->Close(*this);
        }
    }

    void Window::CloseOthers()
    {
        auto* windowMgr = Ui::GetWindowManager();
        windowMgr->CloseAllExceptNumberAndClass(number, classification);
    }

    void Window::CloseOthersOfThisClass()
    {
        auto* windowMgr = Ui::GetWindowManager();
        windowMgr->CloseByClass(classification);
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

    void Window::ResizeFrame()
    {
        // Frame
        widgets[0].right = width - 1;
        widgets[0].bottom = height - 1;
        // Title
        widgets[1].right = width - 2;
        // Close button
        if (Config::Get().interface.WindowButtonsOnTheLeft)
        {
            widgets[2].left = 2;
            widgets[2].right = 2 + kCloseButtonWidth;
        }
        else
        {
            widgets[2].left = width - 3 - kCloseButtonWidth;
            widgets[2].right = width - 3;
        }
    }

    void Window::ResizeFrameWithPage()
    {
        ResizeFrame();
        // Page background
        widgets[3].right = width - 1;
        widgets[3].bottom = height - 1;
    }

    void Window::ResizeSpinner(WidgetIndex widgetIndex, const ScreenCoordsXY& origin, const ScreenSize& size)
    {
        auto right = origin.x + size.width - 1;
        auto bottom = origin.y + size.height - 1;
        widgets[widgetIndex].left = origin.x;
        widgets[widgetIndex].top = origin.y;
        widgets[widgetIndex].right = right;
        widgets[widgetIndex].bottom = bottom;

        widgets[widgetIndex + 1].left = right - size.height; // subtract height to maintain aspect ratio
        widgets[widgetIndex + 1].top = origin.y + 1;
        widgets[widgetIndex + 1].right = right - 1;
        widgets[widgetIndex + 1].bottom = bottom - 1;

        widgets[widgetIndex + 2].left = right - size.height * 2;
        widgets[widgetIndex + 2].top = origin.y + 1;
        widgets[widgetIndex + 2].right = right - size.height - 1;
        widgets[widgetIndex + 2].bottom = bottom - 1;
    }

    void Window::ResizeDropdown(WidgetIndex widgetIndex, const ScreenCoordsXY& origin, const ScreenSize& size)
    {
        auto right = origin.x + size.width - 1;
        auto bottom = origin.y + size.height - 1;
        widgets[widgetIndex].left = origin.x;
        widgets[widgetIndex].top = origin.y;
        widgets[widgetIndex].right = right;
        widgets[widgetIndex].bottom = bottom;

        widgets[widgetIndex + 1].left = right - size.height + 1; // subtract height to maintain aspect ratio
        widgets[widgetIndex + 1].top = origin.y + 1;
        widgets[widgetIndex + 1].right = right - 1;
        widgets[widgetIndex + 1].bottom = bottom - 1;
    }

    void WindowAlignTabs(WindowBase* w, WidgetIndex start_tab_id, WidgetIndex end_tab_id)
    {
        assert(start_tab_id < w->widgets.size());
        assert(end_tab_id < w->widgets.size());

        int32_t i, x = w->widgets[start_tab_id].left;
        int32_t tab_width = w->widgets[start_tab_id].width();

        for (i = start_tab_id; i <= end_tab_id; i++)
        {
            auto& widget = w->widgets[i];
            assert(widget.type == WindowWidgetType::Tab);

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
        const uint8_t buttonOffset = (Config::Get().interface.WindowButtonsOnTheLeft) ? kCloseButtonWidth + 2 : 0;
        return w.windowPos + ScreenCoordsXY{ 2 + buttonOffset, 2 };
    }
} // namespace OpenRCT2

namespace OpenRCT2::Ui::Windows
{
    static u8string _textBoxInput;
    static int32_t _textBoxFrameNo = 0;
    static bool _usingWidgetTextBox = false;
    static TextInputSession* _textInput;
    static WidgetIdentifier _currentTextBox = { { WindowClass::Null, 0 }, 0 };

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

        auto* windowMgr = Ui::GetWindowManager();
        windowMgr->CloseByClass(WindowClass::Textinput);

        _textBoxInput = existingText;

        _textInput = ContextStartTextInput(_textBoxInput, maxLength);
    }

    void WindowCancelTextbox()
    {
        if (_usingWidgetTextBox)
        {
            auto* windowMgr = GetWindowManager();
            WindowBase* w = windowMgr->FindByNumber(_currentTextBox.window.classification, _currentTextBox.window.number);
            _currentTextBox.window.classification = WindowClass::Null;
            _currentTextBox.window.number = 0;
            ContextStopTextInput();
            _usingWidgetTextBox = false;
            if (w != nullptr)
            {
                windowMgr->InvalidateWidget(*w, _currentTextBox.widget_index);
            }
            _currentTextBox.widget_index = kWidgetIndexNull;
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
            auto* windowMgr = GetWindowManager();
            WindowBase* w = windowMgr->FindByNumber(_currentTextBox.window.classification, _currentTextBox.window.number);
            windowMgr->InvalidateWidget(*w, _currentTextBox.widget_index);
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

    void WindowResize(WindowBase& w, int16_t dw, int16_t dh)
    {
        if (dw == 0 && dh == 0)
            return;

        // Invalidate old region
        w.Invalidate();

        // Clamp new size to minimum and maximum
        w.width = std::clamp<int16_t>(w.width + dw, w.min_width, w.max_width);
        w.height = std::clamp<int16_t>(w.height + dh, w.min_height, w.max_height);

        w.OnResize();
        w.OnPrepareDraw();

        // Update scroll widgets
        for (auto& scroll : w.scrolls)
        {
            scroll.contentWidth = -1;
            scroll.contentHeight = -1;
        }
        WindowUpdateScrollWidgets(w);

        // Invalidate new region
        w.Invalidate();
    }

    /**
     *
     *  rct2: 0x006EAE4E
     *
     * @param w The window (esi).
     */
    void WindowUpdateScrollWidgets(WindowBase& w)
    {
        int32_t scrollIndex, width, height, scrollPositionChanged;
        scrollIndex = 0;
        for (WidgetIndex widgetIndex = 0; widgetIndex < w.widgets.size(); widgetIndex++)
        {
            auto& widget = w.widgets[widgetIndex];
            if (widget.type != WindowWidgetType::Scroll)
                continue;

            auto& scroll = w.scrolls[scrollIndex];
            ScreenSize scrollSize = w.OnScrollGetSize(scrollIndex);
            width = scrollSize.width;
            height = scrollSize.height;

            if (height == 0)
            {
                scroll.contentOffsetY = 0;
            }
            else if (width == 0)
            {
                scroll.contentOffsetX = 0;
            }
            width++;
            height++;

            scrollPositionChanged = 0;
            if ((widget.content & SCROLL_HORIZONTAL) && width != scroll.contentWidth)
            {
                scrollPositionChanged = 1;
                scroll.contentWidth = width;
            }

            if ((widget.content & SCROLL_VERTICAL) && height != scroll.contentHeight)
            {
                scrollPositionChanged = 1;
                scroll.contentHeight = height;
            }

            if (scrollPositionChanged)
            {
                WidgetScrollUpdateThumbs(w, widgetIndex);
                w.Invalidate();
            }
            scrollIndex++;
        }
    }
    /**
     * Initialises scroll widgets to their virtual size.
     *  rct2: 0x006EAEB8
     */
    void WindowInitScrollWidgets(WindowBase& w)
    {
        int32_t scroll_index{};
        for (WidgetIndex widgetIndex = 0; widgetIndex < w.widgets.size(); widgetIndex++)
        {
            auto& widget = w.widgets[widgetIndex];
            if (widget.type != WindowWidgetType::Scroll)
            {
                continue;
            }

            auto& scroll = w.scrolls[scroll_index];
            scroll.flags = 0;
            ScreenSize scrollSize = w.OnScrollGetSize(scroll_index);
            scroll.contentOffsetX = 0;
            scroll.contentWidth = scrollSize.width + 1;
            scroll.contentOffsetY = 0;
            scroll.contentHeight = scrollSize.height + 1;

            if (widget.content & SCROLL_HORIZONTAL)
                scroll.flags |= HSCROLLBAR_VISIBLE;
            if (widget.content & SCROLL_VERTICAL)
                scroll.flags |= VSCROLLBAR_VISIBLE;

            WidgetScrollUpdateThumbs(w, widgetIndex);
            scroll_index++;
        }
    }

    static void SnapLeft(WindowBase& w, int32_t proximity)
    {
        const auto* mainWindow = WindowGetMain();
        auto wBottom = w.windowPos.y + w.height;
        auto wLeftProximity = w.windowPos.x - (proximity * 2);
        auto wRightProximity = w.windowPos.x + (proximity * 2);
        auto rightMost = INT32_MIN;

        WindowVisitEach([&](WindowBase* w2) {
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

    static void SnapTop(WindowBase& w, int32_t proximity)
    {
        const auto* mainWindow = WindowGetMain();
        auto wRight = w.windowPos.x + w.width;
        auto wTopProximity = w.windowPos.y - (proximity * 2);
        auto wBottomProximity = w.windowPos.y + (proximity * 2);
        auto bottomMost = INT32_MIN;

        WindowVisitEach([&](WindowBase* w2) {
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

    static void SnapRight(WindowBase& w, int32_t proximity)
    {
        const auto* mainWindow = WindowGetMain();
        auto wRight = w.windowPos.x + w.width;
        auto wBottom = w.windowPos.y + w.height;
        auto wLeftProximity = wRight - (proximity * 2);
        auto wRightProximity = wRight + (proximity * 2);
        auto leftMost = INT32_MAX;

        WindowVisitEach([&](WindowBase* w2) {
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

    static void SnapBottom(WindowBase& w, int32_t proximity)
    {
        const auto* mainWindow = WindowGetMain();
        auto wRight = w.windowPos.x + w.width;
        auto wBottom = w.windowPos.y + w.height;
        auto wTopProximity = wBottom - (proximity * 2);
        auto wBottomProximity = wBottom + (proximity * 2);
        auto topMost = INT32_MAX;

        WindowVisitEach([&](WindowBase* w2) {
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

    void WindowMoveAndSnap(WindowBase& w, ScreenCoordsXY newWindowCoords, int32_t snapProximity)
    {
        auto originalPos = w.windowPos;
        int32_t minY = (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO) ? 1 : kTopToolbarHeight + 2;

        newWindowCoords.y = std::clamp(newWindowCoords.y, minY, ContextGetHeight() - 34);

        if (snapProximity > 0)
        {
            w.windowPos = newWindowCoords;

            SnapRight(w, snapProximity);
            SnapBottom(w, snapProximity);
            SnapLeft(w, snapProximity);
            SnapTop(w, snapProximity);

            if (w.windowPos == originalPos)
                return;

            newWindowCoords = w.windowPos;
            w.windowPos = originalPos;
        }

        WindowSetPosition(w, newWindowCoords);
    }

    void WindowMovePosition(WindowBase& w, const ScreenCoordsXY& deltaCoords)
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

    void WindowSetPosition(WindowBase& w, const ScreenCoordsXY& screenCoords)
    {
        WindowMovePosition(w, screenCoords - w.windowPos);
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
        WindowVisitEach([width, height, &new_location](WindowBase* w) {
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
            w->windowPos = { new_location, new_location + kTopToolbarHeight + 1 };

            // Move the next new location so windows are not directly on top
            new_location += 8;

            // Adjust the viewport if required.
            if (w->viewport != nullptr)
            {
                w->viewport->pos -= newWinPos - w->windowPos;
            }
        });
    }

    void WindowSetResize(WindowBase& w, int16_t minWidth, int16_t minHeight, int16_t maxWidth, int16_t maxHeight)
    {
        w.min_width = minWidth;
        w.min_height = minHeight;
        w.max_width = maxWidth;
        w.max_height = maxHeight;

        // Clamp width and height to minimum and maximum
        int16_t width = std::clamp<int16_t>(w.width, std::min(minWidth, maxWidth), std::max(minWidth, maxWidth));
        int16_t height = std::clamp<int16_t>(w.height, std::min(minHeight, maxHeight), std::max(minHeight, maxHeight));

        // Resize window if size has changed
        if (w.width != width || w.height != height)
        {
            w.Invalidate();
            w.width = width;
            w.height = height;
            w.Invalidate();
        }
    }

    bool WindowCanResize(const WindowBase& w)
    {
        return (w.flags & WF_RESIZABLE) && (w.min_width != w.max_width || w.min_height != w.max_height);
    }

    /**
     *
     *  rct2: 0x006EA73F
     */
    void InvalidateAllWindowsAfterInput()
    {
        WindowVisitEach([](WindowBase* w) {
            Windows::WindowUpdateScrollWidgets(*w);
            WindowInvalidatePressedImageButton(*w);
            w->OnResize();
        });
    }

    /**
     *
     *  rct2: 0x00685BE1
     *
     * @param dpi (edi)
     * @param w (esi)
     */
    void WindowDrawViewport(DrawPixelInfo& dpi, WindowBase& w)
    {
        ViewportRender(dpi, w.viewport);
    }

    /**
     *
     *  rct2: 0x006EB15C
     */
    void WindowDrawWidgets(WindowBase& w, DrawPixelInfo& dpi)
    {
        if ((w.flags & WF_TRANSPARENT) && !(w.flags & WF_NO_BACKGROUND))
            GfxFilterRect(
                dpi, { w.windowPos, w.windowPos + ScreenCoordsXY{ w.width - 1, w.height - 1 } }, FilterPaletteID::Palette51);

        // todo: some code missing here? Between 006EB18C and 006EB260
        for (WidgetIndex widgetIndex = 0; widgetIndex < w.widgets.size(); widgetIndex++)
        {
            auto& widget = w.widgets[widgetIndex];
            if (!widget.IsVisible())
            {
                continue;
            }

            // Check if widget is outside the draw region
            if (w.windowPos.x + widget.left < dpi.x + dpi.width && w.windowPos.x + widget.right >= dpi.x)
            {
                if (w.windowPos.y + widget.top < dpi.y + dpi.height && w.windowPos.y + widget.bottom >= dpi.y)
                {
                    w.OnDrawWidget(widgetIndex, dpi);
                }
            }
        }

        // todo: something missing here too? Between 006EC32B and 006EC369

        if (w.flags & WF_WHITE_BORDER_MASK)
        {
            GfxFillRectInset(
                dpi, { w.windowPos, w.windowPos + ScreenCoordsXY{ w.width - 1, w.height - 1 } }, { COLOUR_WHITE },
                INSET_RECT_FLAG_FILL_NONE);
        }
    }

    /**
     *
     *  rct2: 0x006887A6
     */
    void WindowZoomIn(WindowBase& w, bool atCursor)
    {
        WindowZoomSet(w, w.viewport->zoom - 1, atCursor);
    }

    /**
     *
     *  rct2: 0x006887E0
     */
    void WindowZoomOut(WindowBase& w, bool atCursor)
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

        if (gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR && GetGameState().EditorStep != EditorStep::LandscapeEditor)
            return;

        if (gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER)
            return;

        if (zoomIn)
            WindowZoomIn(*mainWindow, atCursor);
        else
            WindowZoomOut(*mainWindow, atCursor);
    }

} // namespace OpenRCT2::Ui::Windows
