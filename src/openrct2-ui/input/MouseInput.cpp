/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "MouseInput.h"

#include "../UiStringIds.h"

#include <cassert>
#include <cmath>
#include <iterator>
#include <openrct2-ui/UiContext.h>
#include <openrct2-ui/input/InputManager.h>
#include <openrct2-ui/interface/Dropdown.h>
#include <openrct2-ui/interface/Viewport.h>
#include <openrct2-ui/interface/ViewportInteraction.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/interface/Window.h>
#include <openrct2-ui/windows/Windows.h>
#include <openrct2/Context.h>
#include <openrct2/Game.h>
#include <openrct2/Input.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/audio/audio.h>
#include <openrct2/config/Config.h>
#include <openrct2/interface/Chat.h>
#include <openrct2/interface/Cursors.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/platform/Platform.h>
#include <openrct2/ride/RideData.h>
#include <openrct2/scenario/Scenario.h>
#include <openrct2/ui/UiContext.h>
#include <openrct2/ui/WindowManager.h>
#include <openrct2/world/Banner.h>
#include <openrct2/world/Map.h>
#include <openrct2/world/Scenery.h>
#include <optional>

namespace OpenRCT2
{
    using namespace OpenRCT2::Ui;
    using namespace OpenRCT2::Ui::Windows;

    struct RCTMouseData
    {
        uint32_t x;
        uint32_t y;
        MouseState state;
    };

    static RCTMouseData _mouseInputQueue[64];
    static uint8_t _mouseInputQueueReadIndex = 0;
    static uint8_t _mouseInputQueueWriteIndex = 0;

    static std::optional<uint32_t> _ticksSinceDragStart;
    static WidgetRef _dragWidget;
    static uint8_t _dragScrollIndex;
    static int32_t _originalWindowWidth;
    static int32_t _originalWindowHeight;

    static uint8_t _currentScrollIndex;
    static uint8_t _currentScrollArea;

    ScreenCoordsXY gInputDragLast;

    uint32_t gTooltipCloseTimeout;
    WidgetRef gTooltipWidget;
    ScreenCoordsXY gTooltipCursor;

    static std::optional<uint32_t> _clickRepeatTicks;

    static MouseState GameGetNextInput(ScreenCoordsXY& screenCoords);
    static void InputWidgetOver(const ScreenCoordsXY& screenCoords, WindowBase* w, WidgetIndex widgetIndex);
    static void InputWidgetOverChangeCheck(WindowClass windowClass, rct_windownumber windowNumber, WidgetIndex widgetIndex);
    static void InputWidgetOverFlatbuttonInvalidate();
    void ProcessMouseOver(const ScreenCoordsXY& screenCoords);
    void ProcessMouseTool(const ScreenCoordsXY& screenCoords);
    void InvalidateScroll();
    static RCTMouseData* GetMouseInput();
    void TileElementRightClick(int32_t type, TileElement* tileElement, const ScreenCoordsXY& screenCoords);
    static void GameHandleInputMouse(const ScreenCoordsXY& screenCoords, MouseState state);
    static void InputWidgetLeft(const ScreenCoordsXY& screenCoords, WindowBase* w, WidgetIndex widgetIndex);
    void InputStateWidgetPressed(
        const ScreenCoordsXY& screenCoords, MouseState state, WidgetIndex widgetIndex, WindowBase* w, Widget* widget);
    void SetCursor(CursorID cursor_id);
    static void InputWindowPositionContinue(
        WindowBase& w, const ScreenCoordsXY& lastScreenCoords, const ScreenCoordsXY& newScreenCoords);
    static void InputWindowPositionEnd(WindowBase& w, const ScreenCoordsXY& screenCoords);
    static void InputWindowResizeBegin(WindowBase& w, WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords);
    static void InputWindowResizeContinue(WindowBase& w, const ScreenCoordsXY& screenCoords);
    static void InputWindowResizeEnd();
    static void InputViewportDragBegin(WindowBase& w);
    static void InputViewportDragContinue();
    static void InputViewportDragEnd();
    static void InputScrollBegin(WindowBase& w, WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords);
    static void InputScrollContinue(WindowBase& w, WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords);
    static void InputScrollEnd();
    static void InputScrollPartUpdateHThumb(WindowBase& w, WidgetIndex widgetIndex, int32_t x, int32_t scroll_id);
    static void InputScrollPartUpdateHLeft(WindowBase& w, WidgetIndex widgetIndex, int32_t scroll_id);
    static void InputScrollPartUpdateHRight(WindowBase& w, WidgetIndex widgetIndex, int32_t scroll_id);
    static void InputScrollPartUpdateVThumb(WindowBase& w, WidgetIndex widgetIndex, int32_t y, int32_t scroll_id);
    static void InputScrollPartUpdateVTop(WindowBase& w, WidgetIndex widgetIndex, int32_t scroll_id);
    static void InputScrollPartUpdateVBottom(WindowBase& w, WidgetIndex widgetIndex, int32_t scroll_id);
    static void InputUpdateTooltip(WindowBase* w, WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords);

#pragma region Mouse input

    /**
     *
     *  rct2: 0x006EA627
     */
    void GameHandleInput()
    {
        InvalidateAllWindowsAfterInput();

        MouseState state;
        ScreenCoordsXY screenCoords;
        while ((state = GameGetNextInput(screenCoords)) != MouseState::Released)
        {
            GameHandleInputMouse(screenCoords, state);
        }

        if (_inputFlags & INPUT_FLAG_5)
        {
            GameHandleInputMouse(screenCoords, state);
        }
        else
        {
            int32_t screenWidth = ContextGetWidth();
            int32_t screenHeight = ContextGetHeight();
            screenCoords.x = std::clamp(screenCoords.x, 0, screenWidth - 1);
            screenCoords.y = std::clamp(screenCoords.y, 0, screenHeight - 1);

            GameHandleInputMouse(screenCoords, state);
            ProcessMouseOver(screenCoords);
            ProcessMouseTool(screenCoords);
        }
    }

    /**
     *
     *  rct2: 0x006E83C7
     */
    static MouseState GameGetNextInput(ScreenCoordsXY& screenCoords)
    {
        RCTMouseData* input = GetMouseInput();
        if (input == nullptr)
        {
            const CursorState* cursorState = ContextGetCursorState();
            screenCoords = cursorState->position;
            return MouseState::Released;
        }

        screenCoords.x = input->x;
        screenCoords.y = input->y;
        return input->state;
    }

    /**
     *
     *  rct2: 0x00407074
     */
    static RCTMouseData* GetMouseInput()
    {
        // Check if that location has been written to yet
        if (_mouseInputQueueReadIndex == _mouseInputQueueWriteIndex)
        {
            return nullptr;
        }

        RCTMouseData* result = &_mouseInputQueue[_mouseInputQueueReadIndex];
        _mouseInputQueueReadIndex = (_mouseInputQueueReadIndex + 1) % std::size(_mouseInputQueue);
        return result;
    }

    /**
     *
     *  rct2: 0x006E957F
     */
    static void InputScrollDragBegin(const ScreenCoordsXY& screenCoords, WindowBase* w, WidgetIndex widgetIndex)
    {
        _inputState = InputState::ScrollRight;
        gInputDragLast = screenCoords;
        _dragWidget.window_classification = w->classification;
        _dragWidget.window_number = w->number;
        _dragWidget.widget_index = widgetIndex;
        _ticksSinceDragStart = gCurrentRealTimeTicks;

        _dragScrollIndex = WindowGetScrollDataIndex(*w, widgetIndex);
        ContextHideCursor();
    }

    /**
     * Based on (heavily changed)
     *  rct2: 0x006E9E0E,  0x006E9ED0
     */
    static void InputScrollDragContinue(const ScreenCoordsXY& screenCoords, WindowBase* w)
    {
        WidgetIndex widgetIndex = _dragWidget.widget_index;
        uint8_t scrollIndex = _dragScrollIndex;

        const auto& widget = w->widgets[widgetIndex];
        auto& scroll = w->scrolls[scrollIndex];

        ScreenCoordsXY differentialCoords = screenCoords - gInputDragLast;
        if (differentialCoords.x == 0 && differentialCoords.y == 0)
            return;

        if (scroll.flags & HSCROLLBAR_VISIBLE)
        {
            int16_t size = widget.width() - 1;
            if (scroll.flags & VSCROLLBAR_VISIBLE)
                size -= 11;
            size = std::max(0, scroll.contentWidth - size);
            scroll.contentOffsetX = std::min<uint16_t>(std::max(0, scroll.contentOffsetX + differentialCoords.x), size);
        }

        if (scroll.flags & VSCROLLBAR_VISIBLE)
        {
            int16_t size = widget.height() - 1;
            if (scroll.flags & HSCROLLBAR_VISIBLE)
                size -= 11;
            size = std::max(0, scroll.contentHeight - size);
            scroll.contentOffsetY = std::min<uint16_t>(std::max(0, scroll.contentOffsetY + differentialCoords.y), size);
        }

        WidgetScrollUpdateThumbs(*w, widgetIndex);

        auto* windowMgr = Ui::GetWindowManager();
        windowMgr->InvalidateByNumber(w->classification, w->number);

        ScreenCoordsXY fixedCursorPosition = {
            static_cast<int32_t>(std::ceil(gInputDragLast.x * Config::Get().general.WindowScale)),
            static_cast<int32_t>(std::ceil(gInputDragLast.y * Config::Get().general.WindowScale))
        };

        ContextSetCursorPosition(fixedCursorPosition);
    }

    /**
     *
     *  rct2: 0x006E8ACB
     */
    static void InputScrollRight(const ScreenCoordsXY& screenCoords, MouseState state)
    {
        auto* windowMgr = GetWindowManager();
        WindowBase* w = windowMgr->FindByNumber(_dragWidget.window_classification, _dragWidget.window_number);
        if (w == nullptr)
        {
            ContextShowCursor();
            _inputState = InputState::Reset;
            return;
        }

        switch (state)
        {
            case MouseState::Released:
                if (screenCoords.x != 0 || screenCoords.y != 0)
                {
                    _ticksSinceDragStart = std::nullopt;
                    InputScrollDragContinue(screenCoords, w);
                }
                break;
            case MouseState::RightRelease:
                _inputState = InputState::Reset;
                ContextShowCursor();
                break;
            case MouseState::LeftPress:
            case MouseState::LeftRelease:
            case MouseState::RightPress:
                // Function only handles right button, so it's the only one relevant
                break;
        }
    }

    /**
     *
     *  rct2: 0x006E8655
     */
    static void GameHandleInputMouse(const ScreenCoordsXY& screenCoords, MouseState state)
    {
        WindowBase* w;
        Widget* widget;
        WidgetIndex widgetIndex;

        auto* windowMgr = GetWindowManager();

        // Get window and widget under cursor position
        w = windowMgr->FindFromPoint(screenCoords);
        widgetIndex = w == nullptr ? kWidgetIndexNull : windowMgr->FindWidgetFromPoint(*w, screenCoords);
        widget = widgetIndex == kWidgetIndexNull ? nullptr : &w->widgets[widgetIndex];

        switch (_inputState)
        {
            case InputState::Reset:
                WindowTooltipReset(screenCoords);
                // fall-through
            case InputState::Normal:
                switch (state)
                {
                    case MouseState::Released:
                        InputWidgetOver(screenCoords, w, widgetIndex);
                        break;
                    case MouseState::LeftPress:
                        InputWidgetLeft(screenCoords, w, widgetIndex);
                        break;
                    case MouseState::RightPress:
                        windowMgr->CloseByClass(WindowClass::Tooltip);

                        if (w != nullptr)
                        {
                            w = windowMgr->BringToFront(*w);
                        }

                        if (widgetIndex != kWidgetIndexNull)
                        {
                            switch (widget->type)
                            {
                                case WindowWidgetType::Viewport:
                                    if (!(gScreenFlags & (SCREEN_FLAGS_TRACK_MANAGER | SCREEN_FLAGS_TITLE_DEMO)))
                                    {
                                        InputViewportDragBegin(*w);
                                    }
                                    break;
                                case WindowWidgetType::Scroll:
                                    InputScrollDragBegin(screenCoords, w, widgetIndex);
                                    break;
                                default:
                                    break;
                            }
                        }
                        break;
                    case MouseState::LeftRelease:
                    case MouseState::RightRelease:
                        // In this switch only button presses are relevant
                        break;
                }
                break;
            case InputState::WidgetPressed:
                InputStateWidgetPressed(screenCoords, state, widgetIndex, w, widget);
                break;
            case InputState::PositioningWindow:
                w = windowMgr->FindByNumber(_dragWidget.window_classification, _dragWidget.window_number);
                if (w == nullptr)
                {
                    _inputState = InputState::Reset;
                }
                else
                {
                    InputWindowPositionContinue(*w, gInputDragLast, screenCoords);
                    if (state == MouseState::LeftRelease)
                    {
                        InputWindowPositionEnd(*w, screenCoords);
                    }
                }
                break;
            case InputState::ViewportRight:
                if (state == MouseState::Released)
                {
                    InputViewportDragContinue();
                }
                else if (state == MouseState::RightRelease)
                {
                    InputViewportDragEnd();
                    if (_ticksSinceDragStart.has_value() && gCurrentRealTimeTicks - _ticksSinceDragStart.value() < 500)
                    {
                        // If the user pressed the right mouse button for less than 500 ticks, interpret as right click
                        ViewportInteractionRightClick(screenCoords);
                    }
                }
                break;
            case InputState::DropdownActive:
                InputStateWidgetPressed(screenCoords, state, widgetIndex, w, widget);
                break;
            case InputState::ViewportLeft:
                w = windowMgr->FindByNumber(_dragWidget.window_classification, _dragWidget.window_number);
                if (w == nullptr)
                {
                    _inputState = InputState::Reset;
                    break;
                }

                switch (state)
                {
                    case MouseState::Released:
                        if (w->viewport == nullptr)
                        {
                            _inputState = InputState::Reset;
                            break;
                        }

                        if (!InputTestFlag(INPUT_FLAG_4))
                            break;

                        if (w->classification != _dragWidget.window_classification || w->number != _dragWidget.window_number
                            || !(_inputFlags & INPUT_FLAG_TOOL_ACTIVE))
                        {
                            break;
                        }

                        w = windowMgr->FindByNumber(gCurrentToolWidget.window_classification, gCurrentToolWidget.window_number);
                        if (w == nullptr)
                        {
                            break;
                        }

                        w->OnToolDrag(gCurrentToolWidget.widget_index, screenCoords);
                        break;
                    case MouseState::LeftRelease:
                        _inputState = InputState::Reset;
                        if (_dragWidget.window_number == w->number)
                        {
                            if ((_inputFlags & INPUT_FLAG_TOOL_ACTIVE))
                            {
                                w = windowMgr->FindByNumber(
                                    gCurrentToolWidget.window_classification, gCurrentToolWidget.window_number);
                                if (w != nullptr)
                                {
                                    w->OnToolUp(gCurrentToolWidget.widget_index, screenCoords);
                                }
                            }
                            else if (!(_inputFlags & INPUT_FLAG_4))
                            {
                                ViewportInteractionLeftClick(screenCoords);
                            }
                        }
                        break;
                    case MouseState::LeftPress:
                    case MouseState::RightPress:
                    case MouseState::RightRelease:
                        // In this switch only left button release is relevant
                        break;
                }
                break;
            case InputState::ScrollLeft:
                switch (state)
                {
                    case MouseState::Released:
                        InputScrollContinue(*w, widgetIndex, screenCoords);
                        break;
                    case MouseState::LeftRelease:
                        InputScrollEnd();
                        break;
                    case MouseState::LeftPress:
                    case MouseState::RightPress:
                    case MouseState::RightRelease:
                        // In this switch only left button release is relevant
                        break;
                }
                break;
            case InputState::Resizing:
                w = windowMgr->FindByNumber(_dragWidget.window_classification, _dragWidget.window_number);
                if (w == nullptr)
                {
                    _inputState = InputState::Reset;
                }
                else
                {
                    if (state == MouseState::LeftRelease)
                    {
                        InputWindowResizeEnd();
                    }
                    if (state == MouseState::Released || state == MouseState::LeftRelease)
                    {
                        InputWindowResizeContinue(*w, screenCoords);
                    }
                }
                break;
            case InputState::ScrollRight:
                InputScrollRight(screenCoords, state);
                break;
        }
    }

#pragma region Window positioning / resizing

    void InputWindowPositionBegin(WindowBase& w, WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords)
    {
        _inputState = InputState::PositioningWindow;
        gInputDragLast = screenCoords - w.windowPos;
        _dragWidget.window_classification = w.classification;
        _dragWidget.window_number = w.number;
        _dragWidget.widget_index = widgetIndex;
    }

    static void InputWindowPositionContinue(
        WindowBase& w, const ScreenCoordsXY& lastScreenCoords, const ScreenCoordsXY& newScreenCoords)
    {
        int32_t snapProximity;

        snapProximity = (w.flags & WF_NO_SNAPPING) ? 0 : Config::Get().general.WindowSnapProximity;
        WindowMoveAndSnap(w, newScreenCoords - lastScreenCoords, snapProximity);
    }

    static void InputWindowPositionEnd(WindowBase& w, const ScreenCoordsXY& screenCoords)
    {
        _inputState = InputState::Normal;
        gTooltipCloseTimeout = 0;
        gTooltipWidget = _dragWidget;
        w.OnMoved(screenCoords);
    }

    static void InputWindowResizeBegin(WindowBase& w, WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords)
    {
        _inputState = InputState::Resizing;
        gInputDragLast = screenCoords;
        _dragWidget.window_classification = w.classification;
        _dragWidget.window_number = w.number;
        _dragWidget.widget_index = widgetIndex;
        _originalWindowWidth = w.width;
        _originalWindowHeight = w.height;
    }

    static void InputWindowResizeContinue(WindowBase& w, const ScreenCoordsXY& screenCoords)
    {
        if (screenCoords.y < static_cast<int32_t>(ContextGetHeight()) - 2)
        {
            auto differentialCoords = screenCoords - gInputDragLast;
            int32_t targetWidth = _originalWindowWidth + differentialCoords.x - w.width;
            int32_t targetHeight = _originalWindowHeight + differentialCoords.y - w.height;

            WindowResize(w, targetWidth, targetHeight);
        }
    }

    static void InputWindowResizeEnd()
    {
        _inputState = InputState::Normal;
        gTooltipCloseTimeout = 0;
        gTooltipWidget = _dragWidget;
    }

#pragma endregion

#pragma region Viewport dragging

    static void InputViewportDragBegin(WindowBase& w)
    {
        w.flags &= ~WF_SCROLLING_TO_LOCATION;
        _inputState = InputState::ViewportRight;
        _dragWidget.window_classification = w.classification;
        _dragWidget.window_number = w.number;
        _ticksSinceDragStart = gCurrentRealTimeTicks;
        auto cursorPosition = ContextGetCursorPosition();
        gInputDragLast = cursorPosition;
        if (!Config::Get().general.InvertViewportDrag)
        {
            ContextHideCursor();
        }

        WindowUnfollowSprite(w);
        // gInputFlags |= INPUT_FLAG_5;
    }

    static void InputViewportDragContinue()
    {
        WindowBase* w;
        Viewport* viewport;

        auto newDragCoords = ContextGetCursorPosition();

        auto differentialCoords = newDragCoords - gInputDragLast;
        if (differentialCoords.x == 0 && differentialCoords.y == 0)
            return;

        auto* windowMgr = GetWindowManager();
        w = windowMgr->FindByNumber(_dragWidget.window_classification, _dragWidget.window_number);

        // #3294: Window can be closed during a drag session, so just finish
        //        the session if the window no longer exists
        if (w == nullptr)
        {
            InputViewportDragEnd();
            return;
        }

        viewport = w->viewport;
        if (viewport == nullptr)
        {
            ContextShowCursor();
            _inputState = InputState::Reset;
        }
        else if (differentialCoords.x != 0 || differentialCoords.y != 0)
        {
            if (!(w->flags & WF_NO_SCROLLING))
            {
                // User dragged a scrollable viewport

                // If the drag time is less than 500 the "drag" is usually interpreted as a right click.
                // As the user moved the mouse, don't interpret it as right click in any case.
                _ticksSinceDragStart = std::nullopt;

                // applying the zoom only with negative values avoids a "deadzone" effect where small positive value round to
                // zero.
                const bool posX = differentialCoords.x > 0;
                const bool posY = differentialCoords.y > 0;
                differentialCoords.x = (viewport->zoom + 1).ApplyTo(-std::abs(differentialCoords.x));
                differentialCoords.y = (viewport->zoom + 1).ApplyTo(-std::abs(differentialCoords.y));
                differentialCoords.x = posX ? -differentialCoords.x : differentialCoords.x;
                differentialCoords.y = posY ? -differentialCoords.y : differentialCoords.y;

                if (Config::Get().general.InvertViewportDrag)
                {
                    w->savedViewPos -= differentialCoords;
                }
                else
                {
                    w->savedViewPos += differentialCoords;
                }
            }
        }

#ifndef __EMSCRIPTEN__
        const CursorState* cursorState = ContextGetCursorState();
        if (cursorState->touch || Config::Get().general.InvertViewportDrag)
        {
            gInputDragLast = newDragCoords;
        }
        else
        {
            ContextSetCursorPosition(gInputDragLast);
        }
#else
        gInputDragLast = newDragCoords;
#endif
    }

    static void InputViewportDragEnd()
    {
        _inputState = InputState::Reset;
        ContextShowCursor();
    }

#pragma endregion

#pragma region Scroll bars

    static void InputScrollBegin(WindowBase& w, WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords)
    {
        const auto& widget = w.widgets[widgetIndex];

        _inputState = InputState::ScrollLeft;
        gPressedWidget.window_classification = w.classification;
        gPressedWidget.window_number = w.number;
        gPressedWidget.widget_index = widgetIndex;
        gTooltipCursor = screenCoords;

        int32_t scroll_area, scroll_id;
        ScreenCoordsXY scrollCoords;
        scroll_id = 0; // safety
        WidgetScrollGetPart(w, &widget, screenCoords, scrollCoords, &scroll_area, &scroll_id);

        _currentScrollArea = scroll_area;
        _currentScrollIndex = scroll_id;
        w.OnScrollSelect(scroll_id, scroll_area);
        if (scroll_area == SCROLL_PART_VIEW)
        {
            w.OnScrollMouseDown(scroll_id, scrollCoords);
            return;
        }

        const auto& widg = w.widgets[widgetIndex];
        auto& scroll = w.scrolls[scroll_id];

        int32_t widget_width = widg.width() - 1;
        if (scroll.flags & VSCROLLBAR_VISIBLE)
            widget_width -= kScrollBarWidth + 1;
        int32_t widget_content_width = std::max(scroll.contentWidth - widget_width, 0);

        int32_t widget_height = widg.bottom - widg.top - 1;
        if (scroll.flags & HSCROLLBAR_VISIBLE)
            widget_height -= kScrollBarWidth + 1;
        int32_t widget_content_height = std::max(scroll.contentHeight - widget_height, 0);

        switch (scroll_area)
        {
            case SCROLL_PART_HSCROLLBAR_LEFT:
                scroll.contentOffsetX = std::max(scroll.contentOffsetX - 3, 0);
                break;
            case SCROLL_PART_HSCROLLBAR_RIGHT:
                scroll.contentOffsetX = std::min(scroll.contentOffsetX + 3, widget_content_width);
                break;
            case SCROLL_PART_HSCROLLBAR_LEFT_TROUGH:
                scroll.contentOffsetX = std::max(scroll.contentOffsetX - widget_width, 0);
                break;
            case SCROLL_PART_HSCROLLBAR_RIGHT_TROUGH:
                scroll.contentOffsetX = std::min(scroll.contentOffsetX + widget_width, widget_content_width);
                break;
            case SCROLL_PART_VSCROLLBAR_TOP:
                scroll.contentOffsetY = std::max(scroll.contentOffsetY - 3, 0);
                break;
            case SCROLL_PART_VSCROLLBAR_BOTTOM:
                scroll.contentOffsetY = std::min(scroll.contentOffsetY + 3, widget_content_height);
                break;
            case SCROLL_PART_VSCROLLBAR_TOP_TROUGH:
                scroll.contentOffsetY = std::max(scroll.contentOffsetY - widget_height, 0);
                break;
            case SCROLL_PART_VSCROLLBAR_BOTTOM_TROUGH:
                scroll.contentOffsetY = std::min(scroll.contentOffsetY + widget_height, widget_content_height);
                break;
            default:
                break;
        }
        WidgetScrollUpdateThumbs(w, widgetIndex);

        auto* windowMgr = Ui::GetWindowManager();
        windowMgr->InvalidateByNumber(w.classification, w.number);
    }

    static void InputScrollContinue(WindowBase& w, WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords)
    {
        int32_t scroll_part, scroll_id;

        const auto& widget = w.widgets[widgetIndex];
        if (w.classification != gPressedWidget.window_classification || w.number != gPressedWidget.window_number
            || widgetIndex != gPressedWidget.widget_index)
        {
            InvalidateScroll();
            return;
        }

        ScreenCoordsXY newScreenCoords;
        WidgetScrollGetPart(w, &widget, screenCoords, newScreenCoords, &scroll_part, &scroll_id);

        if (_currentScrollArea == SCROLL_PART_HSCROLLBAR_THUMB)
        {
            int32_t originalTooltipCursorX = gTooltipCursor.x;
            gTooltipCursor.x = screenCoords.x;
            InputScrollPartUpdateHThumb(w, widgetIndex, screenCoords.x - originalTooltipCursorX, scroll_id);
            return;
        }

        if (_currentScrollArea == SCROLL_PART_VSCROLLBAR_THUMB)
        {
            int32_t originalTooltipCursorY = gTooltipCursor.y;
            gTooltipCursor.y = screenCoords.y;
            InputScrollPartUpdateVThumb(w, widgetIndex, screenCoords.y - originalTooltipCursorY, scroll_id);
            return;
        }

        if (scroll_part != _currentScrollArea)
        {
            InvalidateScroll();
            return;
        }

        switch (scroll_part)
        {
            case SCROLL_PART_VIEW:
                w.OnScrollMouseDrag(scroll_id, newScreenCoords);
                break;
            case SCROLL_PART_HSCROLLBAR_LEFT:
                InputScrollPartUpdateHLeft(w, widgetIndex, scroll_id);
                break;
            case SCROLL_PART_HSCROLLBAR_RIGHT:
                InputScrollPartUpdateHRight(w, widgetIndex, scroll_id);
                break;
            case SCROLL_PART_VSCROLLBAR_TOP:
                InputScrollPartUpdateVTop(w, widgetIndex, scroll_id);
                break;
            case SCROLL_PART_VSCROLLBAR_BOTTOM:
                InputScrollPartUpdateVBottom(w, widgetIndex, scroll_id);
                break;
        }
    }

    static void InputScrollEnd()
    {
        _inputState = InputState::Reset;
        InvalidateScroll();
    }

    /**
     *
     *  rct2: 0x006E98F2
     */
    static void InputScrollPartUpdateHThumb(WindowBase& w, WidgetIndex widgetIndex, int32_t x, int32_t scroll_id)
    {
        const auto& widget = w.widgets[widgetIndex];
        auto& scroll = w.scrolls[scroll_id];

        auto* windowMgr = GetWindowManager();
        if (windowMgr->FindByNumber(w.classification, w.number) != nullptr)
        {
            int32_t newLeft;
            newLeft = scroll.contentWidth;
            newLeft *= x;
            x = widget.width() - 21;
            if (scroll.flags & VSCROLLBAR_VISIBLE)
                x -= kScrollBarWidth + 1;
            newLeft /= x;
            x = newLeft;
            scroll.flags |= HSCROLLBAR_THUMB_PRESSED;
            newLeft = scroll.contentOffsetX;
            newLeft += x;
            if (newLeft < 0)
                newLeft = 0;
            x = widget.width() - 1;
            if (scroll.flags & VSCROLLBAR_VISIBLE)
                x -= kScrollBarWidth + 1;
            x *= -1;
            x += scroll.contentWidth;
            if (x < 0)
                x = 0;
            if (newLeft > x)
                newLeft = x;
            scroll.contentOffsetX = newLeft;
            WidgetScrollUpdateThumbs(w, widgetIndex);
            windowMgr->InvalidateWidgetByNumber(w.classification, w.number, widgetIndex);
        }
    }

    /**
     *
     *  rct2: 0x006E99A9
     */
    static void InputScrollPartUpdateVThumb(WindowBase& w, WidgetIndex widgetIndex, int32_t y, int32_t scroll_id)
    {
        const auto& widget = w.widgets[widgetIndex];
        auto& scroll = w.scrolls[scroll_id];

        auto* windowMgr = GetWindowManager();
        if (windowMgr->FindByNumber(w.classification, w.number) != nullptr)
        {
            int32_t newTop;
            newTop = scroll.contentHeight;
            newTop *= y;
            y = widget.height() - 21;
            if (scroll.flags & HSCROLLBAR_VISIBLE)
                y -= kScrollBarWidth + 1;
            newTop /= y;
            y = newTop;
            scroll.flags |= VSCROLLBAR_THUMB_PRESSED;
            newTop = scroll.contentOffsetY;
            newTop += y;
            if (newTop < 0)
                newTop = 0;
            y = widget.height() - 1;
            if (scroll.flags & HSCROLLBAR_VISIBLE)
                y -= kScrollBarWidth + 1;
            y *= -1;
            y += scroll.contentHeight;
            if (y < 0)
                y = 0;
            if (newTop > y)
                newTop = y;
            scroll.contentOffsetY = newTop;
            WidgetScrollUpdateThumbs(w, widgetIndex);
            windowMgr->InvalidateWidgetByNumber(w.classification, w.number, widgetIndex);
        }
    }

    /**
     *
     *  rct2: 0x006E9A60
     */
    static void InputScrollPartUpdateHLeft(WindowBase& w, WidgetIndex widgetIndex, int32_t scroll_id)
    {
        auto* windowMgr = GetWindowManager();
        if (windowMgr->FindByNumber(w.classification, w.number) != nullptr)
        {
            auto& scroll = w.scrolls[scroll_id];
            scroll.flags |= HSCROLLBAR_LEFT_PRESSED;
            if (scroll.contentOffsetX >= 3)
                scroll.contentOffsetX -= 3;
            WidgetScrollUpdateThumbs(w, widgetIndex);
            windowMgr->InvalidateWidgetByNumber(w.classification, w.number, widgetIndex);
        }
    }

    /**
     *
     *  rct2: 0x006E9ABF
     */
    static void InputScrollPartUpdateHRight(WindowBase& w, WidgetIndex widgetIndex, int32_t scroll_id)
    {
        const auto& widget = w.widgets[widgetIndex];

        auto* windowMgr = GetWindowManager();
        if (windowMgr->FindByNumber(w.classification, w.number) != nullptr)
        {
            auto& scroll = w.scrolls[scroll_id];
            scroll.flags |= HSCROLLBAR_RIGHT_PRESSED;
            scroll.contentOffsetX += 3;
            int32_t newLeft = widget.width() - 1;
            if (scroll.flags & VSCROLLBAR_VISIBLE)
                newLeft -= kScrollBarWidth + 1;
            newLeft *= -1;
            newLeft += scroll.contentWidth;
            if (newLeft < 0)
                newLeft = 0;
            if (scroll.contentOffsetX > newLeft)
                scroll.contentOffsetX = newLeft;
            WidgetScrollUpdateThumbs(w, widgetIndex);
            windowMgr->InvalidateWidgetByNumber(w.classification, w.number, widgetIndex);
        }
    }

    /**
     *
     *  rct2: 0x006E9C37
     */
    static void InputScrollPartUpdateVTop(WindowBase& w, WidgetIndex widgetIndex, int32_t scroll_id)
    {
        auto* windowMgr = GetWindowManager();
        if (windowMgr->FindByNumber(w.classification, w.number) != nullptr)
        {
            auto& scroll = w.scrolls[scroll_id];
            scroll.flags |= VSCROLLBAR_UP_PRESSED;
            if (scroll.contentOffsetY >= 3)
                scroll.contentOffsetY -= 3;
            WidgetScrollUpdateThumbs(w, widgetIndex);
            windowMgr->InvalidateWidgetByNumber(w.classification, w.number, widgetIndex);
        }
    }

    /**
     *
     *  rct2: 0x006E9C96
     */
    static void InputScrollPartUpdateVBottom(WindowBase& w, WidgetIndex widgetIndex, int32_t scroll_id)
    {
        const auto& widget = w.widgets[widgetIndex];

        auto* windowMgr = GetWindowManager();
        if (windowMgr->FindByNumber(w.classification, w.number) != nullptr)
        {
            auto& scroll = w.scrolls[scroll_id];
            scroll.flags |= VSCROLLBAR_DOWN_PRESSED;
            scroll.contentOffsetY += 3;
            int32_t newTop = widget.height() - 1;
            if (scroll.flags & HSCROLLBAR_VISIBLE)
                newTop -= kScrollBarWidth + 1;
            newTop *= -1;
            newTop += scroll.contentHeight;
            if (newTop < 0)
                newTop = 0;
            if (scroll.contentOffsetY > newTop)
                scroll.contentOffsetY = newTop;
            WidgetScrollUpdateThumbs(w, widgetIndex);
            windowMgr->InvalidateWidgetByNumber(w.classification, w.number, widgetIndex);
        }
    }

#pragma endregion

#pragma region Widgets

    /**
     *
     *  rct2: 0x006E9253
     */
    static void InputWidgetOver(const ScreenCoordsXY& screenCoords, WindowBase* w, WidgetIndex widgetIndex)
    {
        WindowClass windowClass = WindowClass::Null;
        rct_windownumber windowNumber = 0;
        Widget* widget = nullptr;

        if (w != nullptr)
        {
            windowClass = w->classification;
            windowNumber = w->number;
            widget = widgetIndex == kWidgetIndexNull ? nullptr : &w->widgets[widgetIndex];
        }

        InputWidgetOverChangeCheck(windowClass, windowNumber, widgetIndex);

        if (w != nullptr && widgetIndex != kWidgetIndexNull && widget->type == WindowWidgetType::Scroll)
        {
            int32_t scroll_part, scrollId;
            ScreenCoordsXY newScreenCoords;
            WidgetScrollGetPart(*w, widget, screenCoords, newScreenCoords, &scroll_part, &scrollId);

            if (scroll_part != SCROLL_PART_VIEW)
                WindowTooltipClose();
            else
            {
                w->OnScrollMouseOver(scrollId, newScreenCoords);
                InputUpdateTooltip(w, widgetIndex, screenCoords);
            }
        }
        else
        {
            InputUpdateTooltip(w, widgetIndex, screenCoords);
        }
    }

    /**
     *
     *  rct2: 0x006E9269
     */
    static void InputWidgetOverChangeCheck(WindowClass windowClass, rct_windownumber windowNumber, WidgetIndex widgetIndex)
    {
        // Prevents invalid widgets being clicked source of bug is elsewhere
        if (widgetIndex == kWidgetIndexNull)
            return;

        // Check if the widget that the cursor was over, has changed
        if (windowClass != gHoverWidget.window_classification || windowNumber != gHoverWidget.window_number
            || widgetIndex != gHoverWidget.widget_index)
        {
            // Invalidate last widget cursor was on if widget is a flat button
            InputWidgetOverFlatbuttonInvalidate();

            // Set new cursor over widget
            gHoverWidget.window_classification = windowClass;
            gHoverWidget.window_number = windowNumber;
            gHoverWidget.widget_index = widgetIndex;

            // Invalidate new widget cursor is on if widget is a flat button
            if (windowClass != WindowClass::Null)
                InputWidgetOverFlatbuttonInvalidate();
        }
    }

    /**
     * Used to invalidate flat button widgets when the mouse leaves and enters them. This should be generalised so that all
     * widgets can use this in the future.
     */
    static void InputWidgetOverFlatbuttonInvalidate()
    {
        auto* windowMgr = GetWindowManager();
        WindowBase* w = windowMgr->FindByNumber(gHoverWidget.window_classification, gHoverWidget.window_number);
        if (w != nullptr)
        {
            w->OnPrepareDraw();
            if (w->widgets[gHoverWidget.widget_index].type == WindowWidgetType::FlatBtn)
            {
                windowMgr->InvalidateWidgetByNumber(
                    gHoverWidget.window_classification, gHoverWidget.window_number, gHoverWidget.widget_index);
            }
        }
    }

    /**
     *
     *  rct2: 0x006E95F9
     */
    static void InputWidgetLeft(const ScreenCoordsXY& screenCoords, WindowBase* w, WidgetIndex widgetIndex)
    {
        WindowClass windowClass = WindowClass::Null;
        rct_windownumber windowNumber = 0;

        if (w != nullptr)
        {
            windowClass = w->classification;
            windowNumber = w->number;
        }

        auto* windowMgr = GetWindowManager();
        windowMgr->CloseByClass(WindowClass::Error);
        windowMgr->CloseByClass(WindowClass::Tooltip);

        // Window might have changed position in the list, therefore find it again
        w = windowMgr->FindByNumber(windowClass, windowNumber);
        if (w == nullptr)
            return;

        w = windowMgr->BringToFront(*w);
        if (widgetIndex == kWidgetIndexNull)
            return;

        if (windowClass != GetCurrentTextBox().window.classification || windowNumber != GetCurrentTextBox().window.number
            || widgetIndex != GetCurrentTextBox().widget_index)
        {
            WindowCancelTextbox();
        }

        const auto& widget = w->widgets[widgetIndex];

        switch (widget.type)
        {
            case WindowWidgetType::Frame:
            case WindowWidgetType::Resize:
                if (WindowCanResize(*w)
                    && (screenCoords.x >= w->windowPos.x + w->width - 19 && screenCoords.y >= w->windowPos.y + w->height - 19))
                    InputWindowResizeBegin(*w, widgetIndex, screenCoords);
                break;
            case WindowWidgetType::Viewport:
                _inputState = InputState::ViewportLeft;
                gInputDragLast = screenCoords;
                _dragWidget.window_classification = windowClass;
                _dragWidget.window_number = windowNumber;
                if (_inputFlags & INPUT_FLAG_TOOL_ACTIVE)
                {
                    w = windowMgr->FindByNumber(gCurrentToolWidget.window_classification, gCurrentToolWidget.window_number);
                    if (w != nullptr)
                    {
                        InputSetFlag(INPUT_FLAG_4, true);
                        w->OnToolDown(gCurrentToolWidget.widget_index, screenCoords);
                    }
                }
                break;
            case WindowWidgetType::Caption:
                InputWindowPositionBegin(*w, widgetIndex, screenCoords);
                break;
            case WindowWidgetType::Scroll:
                InputScrollBegin(*w, widgetIndex, screenCoords);
                break;
            case WindowWidgetType::Empty:
            case WindowWidgetType::LabelCentred:
            case WindowWidgetType::Label:
            case WindowWidgetType::Groupbox:
            case WindowWidgetType::ProgressBar:
            case WindowWidgetType::Placeholder:
                // Non-interactive widget type
                break;
            case WindowWidgetType::ImgBtn:
            case WindowWidgetType::ColourBtn:
            case WindowWidgetType::TrnBtn:
            case WindowWidgetType::Tab:
            case WindowWidgetType::FlatBtn:
            case WindowWidgetType::Button:
            case WindowWidgetType::TableHeader:
            case WindowWidgetType::Spinner:
            case WindowWidgetType::DropdownMenu:
            case WindowWidgetType::CloseBox:
            case WindowWidgetType::Checkbox:
            case WindowWidgetType::TextBox:
            case WindowWidgetType::Custom:
                if (!WidgetIsDisabled(*w, widgetIndex))
                {
                    OpenRCT2::Audio::Play(OpenRCT2::Audio::SoundId::Click1, 0, w->windowPos.x + widget.midX());

                    // Set new cursor down widget
                    gPressedWidget.window_classification = windowClass;
                    gPressedWidget.window_number = windowNumber;
                    gPressedWidget.widget_index = widgetIndex;
                    _inputFlags |= INPUT_FLAG_WIDGET_PRESSED;
                    _inputState = InputState::WidgetPressed;
                    _clickRepeatTicks = gCurrentRealTimeTicks;

                    windowMgr->InvalidateWidgetByNumber(windowClass, windowNumber, widgetIndex);
                    w->OnMouseDown(widgetIndex);
                }
                break;
        }
    }

    /**
     *
     *  rct2: 0x006ED833
     */
    void ProcessMouseOver(const ScreenCoordsXY& screenCoords)
    {
        CursorID cursorId = CursorID::Arrow;
        auto ft = Formatter();
        ft.Add<StringId>(kStringIdNone);
        SetMapTooltip(ft);

        auto* windowMgr = GetWindowManager();
        WindowBase* window = windowMgr->FindFromPoint(screenCoords);

        if (window != nullptr)
        {
            WidgetIndex widgetId = windowMgr->FindWidgetFromPoint(*window, screenCoords);
            if (widgetId != kWidgetIndexNull)
            {
                switch (window->widgets[widgetId].type)
                {
                    case WindowWidgetType::Viewport:
                        if (!(_inputFlags & INPUT_FLAG_TOOL_ACTIVE))
                        {
                            if (ViewportInteractionLeftOver(screenCoords))
                            {
                                SetCursor(CursorID::HandPoint);
                                return;
                            }
                            break;
                        }
                        cursorId = static_cast<CursorID>(gCurrentToolId);
                        break;

                    case WindowWidgetType::Frame:
                    case WindowWidgetType::Resize:
                        if (!(window->flags & WF_RESIZABLE))
                            break;

                        if (window->min_width == window->max_width && window->min_height == window->max_height)
                            break;

                        if (screenCoords.x < window->windowPos.x + window->width - 0x13)
                            break;

                        if (screenCoords.y < window->windowPos.y + window->height - 0x13)
                            break;

                        cursorId = CursorID::DiagonalArrows;
                        break;

                    case WindowWidgetType::Scroll:
                    {
                        int32_t output_scroll_area, scroll_id;
                        ScreenCoordsXY scrollCoords;
                        WidgetScrollGetPart(
                            *window, &window->widgets[widgetId], screenCoords, scrollCoords, &output_scroll_area, &scroll_id);
                        if (output_scroll_area != SCROLL_PART_VIEW)
                        {
                            cursorId = CursorID::Arrow;
                            break;
                        }
                        // Same as default but with scroll_x/y
                        cursorId = window->OnCursor(widgetId, scrollCoords, CursorID::Arrow);
                        if (cursorId == CursorID::Undefined)
                            cursorId = CursorID::Arrow;
                        break;
                    }
                    default:
                        cursorId = window->OnCursor(widgetId, screenCoords, CursorID::Arrow);
                        if (cursorId == CursorID::Undefined)
                            cursorId = CursorID::Arrow;
                        break;
                }
            }
        }

        ViewportInteractionRightOver(screenCoords);
        SetCursor(cursorId);
    }

    /**
     *
     *  rct2: 0x006ED801
     */
    void ProcessMouseTool(const ScreenCoordsXY& screenCoords)
    {
        if (_inputFlags & INPUT_FLAG_TOOL_ACTIVE)
        {
            auto* windowMgr = GetWindowManager();
            WindowBase* w = windowMgr->FindByNumber(gCurrentToolWidget.window_classification, gCurrentToolWidget.window_number);

            if (w == nullptr)
                ToolCancel();
            else if (InputGetState() != InputState::ViewportRight)
                w->OnToolUpdate(gCurrentToolWidget.widget_index, screenCoords);
        }
    }

    const std::map<colour_t, StringId> kColourToTip = {
        { COLOUR_BLACK, STR_COLOUR_BLACK_TIP },
        { COLOUR_GREY, STR_COLOUR_GREY_TIP },
        { COLOUR_WHITE, STR_COLOUR_WHITE_TIP },
        { COLOUR_DARK_PURPLE, STR_COLOUR_DARK_PURPLE_TIP },
        { COLOUR_LIGHT_PURPLE, STR_COLOUR_LIGHT_PURPLE_TIP },
        { COLOUR_BRIGHT_PURPLE, STR_COLOUR_BRIGHT_PURPLE_TIP },
        { COLOUR_DARK_BLUE, STR_COLOUR_DARK_BLUE_TIP },
        { COLOUR_LIGHT_BLUE, STR_COLOUR_LIGHT_BLUE_TIP },
        { COLOUR_ICY_BLUE, STR_COLOUR_ICY_BLUE_TIP },
        { COLOUR_TEAL, STR_COLOUR_TEAL_TIP },
        { COLOUR_AQUAMARINE, STR_COLOUR_AQUAMARINE_TIP },
        { COLOUR_SATURATED_GREEN, STR_COLOUR_SATURATED_GREEN_TIP },
        { COLOUR_DARK_GREEN, STR_COLOUR_DARK_GREEN_TIP },
        { COLOUR_MOSS_GREEN, STR_COLOUR_MOSS_GREEN_TIP },
        { COLOUR_BRIGHT_GREEN, STR_COLOUR_BRIGHT_GREEN_TIP },
        { COLOUR_OLIVE_GREEN, STR_COLOUR_OLIVE_GREEN_TIP },
        { COLOUR_DARK_OLIVE_GREEN, STR_COLOUR_DARK_OLIVE_GREEN_TIP },
        { COLOUR_BRIGHT_YELLOW, STR_COLOUR_BRIGHT_YELLOW_TIP },
        { COLOUR_YELLOW, STR_COLOUR_YELLOW_TIP },
        { COLOUR_DARK_YELLOW, STR_COLOUR_DARK_YELLOW_TIP },
        { COLOUR_LIGHT_ORANGE, STR_COLOUR_LIGHT_ORANGE_TIP },
        { COLOUR_DARK_ORANGE, STR_COLOUR_DARK_ORANGE_TIP },
        { COLOUR_LIGHT_BROWN, STR_COLOUR_LIGHT_BROWN_TIP },
        { COLOUR_SATURATED_BROWN, STR_COLOUR_SATURATED_BROWN_TIP },
        { COLOUR_DARK_BROWN, STR_COLOUR_DARK_BROWN_TIP },
        { COLOUR_SALMON_PINK, STR_COLOUR_SALMON_PINK_TIP },
        { COLOUR_BORDEAUX_RED, STR_COLOUR_BORDEAUX_RED_TIP },
        { COLOUR_SATURATED_RED, STR_COLOUR_SATURATED_RED_TIP },
        { COLOUR_BRIGHT_RED, STR_COLOUR_BRIGHT_RED_TIP },
        { COLOUR_DARK_PINK, STR_COLOUR_DARK_PINK_TIP },
        { COLOUR_BRIGHT_PINK, STR_COLOUR_BRIGHT_PINK_TIP },
        { COLOUR_LIGHT_PINK, STR_COLOUR_LIGHT_PINK_TIP },
        { COLOUR_DARK_OLIVE_DARK, STR_COLOUR_DARK_OLIVE_DARK_TIP },
        { COLOUR_DARK_OLIVE_LIGHT, STR_COLOUR_DARK_OLIVE_LIGHT_TIP },
        { COLOUR_SATURATED_BROWN_LIGHT, STR_COLOUR_SATURATED_BROWN_LIGHT_TIP },
        { COLOUR_BORDEAUX_RED_DARK, STR_COLOUR_BORDEAUX_RED_DARK_TIP },
        { COLOUR_BORDEAUX_RED_LIGHT, STR_COLOUR_BORDEAUX_RED_LIGHT_TIP },
        { COLOUR_GRASS_GREEN_DARK, STR_COLOUR_GRASS_GREEN_DARK_TIP },
        { COLOUR_GRASS_GREEN_LIGHT, STR_COLOUR_GRASS_GREEN_LIGHT_TIP },
        { COLOUR_OLIVE_DARK, STR_COLOUR_OLIVE_DARK_TIP },
        { COLOUR_OLIVE_LIGHT, STR_COLOUR_OLIVE_LIGHT_TIP },
        { COLOUR_SATURATED_GREEN_LIGHT, STR_COLOUR_SATURATED_GREEN_LIGHT_TIP },
        { COLOUR_TAN_DARK, STR_COLOUR_TAN_DARK_TIP },
        { COLOUR_TAN_LIGHT, STR_COLOUR_TAN_LIGHT_TIP },
        { COLOUR_DULL_PURPLE_LIGHT, STR_COLOUR_DULL_PURPLE_LIGHT_TIP },
        { COLOUR_DULL_GREEN_DARK, STR_COLOUR_DULL_GREEN_DARK_TIP },
        { COLOUR_DULL_GREEN_LIGHT, STR_COLOUR_DULL_GREEN_LIGHT_TIP },
        { COLOUR_SATURATED_PURPLE_DARK, STR_COLOUR_SATURATED_PURPLE_DARK_TIP },
        { COLOUR_SATURATED_PURPLE_LIGHT, STR_COLOUR_SATURATED_PURPLE_LIGHT_TIP },
        { COLOUR_ORANGE_LIGHT, STR_COLOUR_ORANGE_LIGHT_TIP },
        { COLOUR_AQUA_DARK, STR_COLOUR_AQUA_DARK_TIP },
        { COLOUR_MAGENTA_LIGHT, STR_COLOUR_MAGENTA_LIGHT_TIP },
        { COLOUR_DULL_BROWN_DARK, STR_COLOUR_DULL_BROWN_DARK_TIP },
        { COLOUR_DULL_BROWN_LIGHT, STR_COLOUR_DULL_BROWN_LIGHT_TIP },
        { COLOUR_INVISIBLE, STR_COLOUR_INVISIBLE_TIP },
        { COLOUR_VOID, STR_COLOUR_VOID_TIP },
    };

    /**
     *
     *  rct2: 0x006E8DA7
     */
    void InputStateWidgetPressed(
        const ScreenCoordsXY& screenCoords, MouseState state, WidgetIndex widgetIndex, WindowBase* w, Widget* widget)
    {
        WindowClass cursor_w_class;
        rct_windownumber cursor_w_number;
        cursor_w_class = gPressedWidget.window_classification;
        cursor_w_number = gPressedWidget.window_number;
        WidgetIndex cursor_widgetIndex = gPressedWidget.widget_index;

        auto* windowMgr = GetWindowManager();
        WindowBase* cursor_w = windowMgr->FindByNumber(cursor_w_class, cursor_w_number);
        if (cursor_w == nullptr)
        {
            _inputState = InputState::Reset;
            return;
        }

        if (w != nullptr && state == MouseState::LeftRelease)
        {
            if (w->widgets[widgetIndex].type == WindowWidgetType::CloseBox && cursor_w_class == w->classification
                && cursor_w_number == w->number && widgetIndex == cursor_widgetIndex)
            {
                auto& im = GetInputManager();
                if (im.IsModifierKeyPressed(ModifierKey::shift))
                {
                    gLastCloseModifier.window.number = w->number;
                    gLastCloseModifier.window.classification = w->classification;
                    gLastCloseModifier.modifier = CloseWindowModifier::Shift;
                }
                else if (im.IsModifierKeyPressed(ModifierKey::ctrl))
                {
                    gLastCloseModifier.window.number = w->number;
                    gLastCloseModifier.window.classification = w->classification;
                    gLastCloseModifier.modifier = CloseWindowModifier::Control;
                }
            }
        }

        switch (state)
        {
            case MouseState::Released:
                if (w == nullptr || cursor_w_class != w->classification || cursor_w_number != w->number
                    || widgetIndex != cursor_widgetIndex)
                    break;

                if (WidgetIsDisabled(*w, widgetIndex))
                    break;

                // If this variable is non-zero then its the last tick the mouse down event was fired.
                if (_clickRepeatTicks.has_value())
                {
                    // The initial amount of time in ticks to wait until the first click repeat.
                    constexpr auto kTicksUntilRepeats = 16u;

                    // The amount of ticks between each click repeat.
                    constexpr auto kEventDelayInTicks = 3u;

                    // The amount of ticks since the last click repeat.
                    const auto clickRepeatsDelta = gCurrentRealTimeTicks - _clickRepeatTicks.value();

                    // Handle click repeat, only start this when at least 16 ticks elapsed.
                    if (clickRepeatsDelta >= kTicksUntilRepeats && (clickRepeatsDelta & kEventDelayInTicks) == 0)
                    {
                        if (WidgetIsHoldable(*w, widgetIndex))
                        {
                            w->OnMouseDown(widgetIndex);
                        }

                        // Subtract initial delay from here on we want the event each third tick.
                        _clickRepeatTicks = gCurrentRealTimeTicks - kTicksUntilRepeats;
                    }
                }

                if (_inputFlags & INPUT_FLAG_WIDGET_PRESSED)
                {
                    if (_inputState == InputState::DropdownActive)
                    {
                        gDropdownHighlightedIndex = gDropdownDefaultIndex;
                        windowMgr->InvalidateByClass(WindowClass::Dropdown);
                    }
                    return;
                }

                _inputFlags |= INPUT_FLAG_WIDGET_PRESSED;
                windowMgr->InvalidateWidgetByNumber(cursor_w_class, cursor_w_number, widgetIndex);
                return;
            case MouseState::LeftRelease:
            case MouseState::RightPress:
                if (_inputState == InputState::DropdownActive)
                {
                    if (w != nullptr)
                    {
                        auto wClass = w->classification;
                        auto wNumber = w->number;
                        int32_t dropdown_index = 0;
                        bool dropdownCleanup = false;

                        if (w->classification == WindowClass::Dropdown)
                        {
                            dropdown_index = DropdownIndexFromPoint(screenCoords, w);
                            dropdownCleanup = dropdown_index == -1
                                || (dropdown_index < Dropdown::kItemsMaxSize && Dropdown::IsDisabled(dropdown_index))
                                || gDropdownItems[dropdown_index].IsSeparator();
                            w = nullptr; // To be closed right next
                        }
                        else
                        {
                            if (cursor_w_class != w->classification || cursor_w_number != w->number
                                || widgetIndex != cursor_widgetIndex)
                            {
                                dropdownCleanup = true;
                            }
                            else
                            {
                                dropdown_index = -1;
                                if (_inputFlags & INPUT_FLAG_DROPDOWN_STAY_OPEN)
                                {
                                    if (!(_inputFlags & INPUT_FLAG_DROPDOWN_MOUSE_UP))
                                    {
                                        _inputFlags |= INPUT_FLAG_DROPDOWN_MOUSE_UP;
                                        return;
                                    }
                                }
                            }
                        }

                        windowMgr->CloseByClass(WindowClass::Dropdown);

                        if (dropdownCleanup)
                        {
                            // Update w as it will be invalid after closing the dropdown window
                            w = windowMgr->FindByNumber(wClass, wNumber);
                        }
                        else
                        {
                            cursor_w = windowMgr->FindByNumber(cursor_w_class, cursor_w_number);
                            if (_inputFlags & INPUT_FLAG_WIDGET_PRESSED)
                            {
                                _inputFlags &= ~INPUT_FLAG_WIDGET_PRESSED;
                                windowMgr->InvalidateWidgetByNumber(cursor_w_class, cursor_w_number, cursor_widgetIndex);
                            }

                            _inputState = InputState::Normal;
                            gTooltipCloseTimeout = 0;
                            gTooltipWidget.widget_index = cursor_widgetIndex;
                            gTooltipWidget.window_classification = cursor_w_class;
                            gTooltipWidget.window_number = cursor_w_number;

                            if (dropdown_index == -1)
                            {
                                if (!Dropdown::IsDisabled(gDropdownDefaultIndex))
                                {
                                    dropdown_index = gDropdownDefaultIndex;
                                }
                            }
                            cursor_w->OnDropdown(cursor_widgetIndex, dropdown_index);
                        }
                    }
                }

                _inputState = InputState::Normal;

                if (state == MouseState::RightPress)
                {
                    return;
                }

                gTooltipCloseTimeout = 0;
                gTooltipWidget.widget_index = cursor_widgetIndex;

                if (w == nullptr)
                    break;

                if (widget == nullptr)
                    break;

                {
                    int32_t mid_point_x = widget->midX() + w->windowPos.x;
                    OpenRCT2::Audio::Play(OpenRCT2::Audio::SoundId::Click2, 0, mid_point_x);
                }
                if (cursor_w_class != w->classification || cursor_w_number != w->number || widgetIndex != cursor_widgetIndex)
                    break;

                if (WidgetIsDisabled(*w, widgetIndex))
                    break;

                windowMgr->InvalidateWidgetByNumber(cursor_w_class, cursor_w_number, widgetIndex);
                w->OnMouseUp(widgetIndex);
                return;

            default:
                return;
        }

        _clickRepeatTicks = std::nullopt;
        if (_inputState != InputState::DropdownActive)
        {
            // Hold down widget and drag outside of area??
            if (_inputFlags & INPUT_FLAG_WIDGET_PRESSED)
            {
                _inputFlags &= ~INPUT_FLAG_WIDGET_PRESSED;
                windowMgr->InvalidateWidgetByNumber(cursor_w_class, cursor_w_number, cursor_widgetIndex);
            }
            return;
        }
        else if (gDropdownIsColour)
        {
            // This is ordinarily covered in InputWidgetOver but the dropdown with colours is a special case.
            InputUpdateTooltip(w, widgetIndex, screenCoords);
        }

        gDropdownHighlightedIndex = -1;
        windowMgr->InvalidateByClass(WindowClass::Dropdown);
        if (w == nullptr)
        {
            return;
        }

        if (w->classification == WindowClass::Dropdown)
        {
            int32_t dropdown_index = DropdownIndexFromPoint(screenCoords, w);
            if (dropdown_index == -1)
            {
                return;
            }

            if (gDropdownIsColour && gDropdownLastColourHover != dropdown_index)
            {
                gDropdownLastColourHover = dropdown_index;
                WindowTooltipClose();

                WindowTooltipShow(
                    OpenRCT2String{ kColourToTip.at(ColourDropDownIndexToColour(dropdown_index)), {} }, screenCoords);
            }

            if (dropdown_index < Dropdown::kItemsMaxSize && Dropdown::IsDisabled(dropdown_index))
            {
                return;
            }

            if (gDropdownItems[dropdown_index].IsSeparator())
            {
                return;
            }

            gDropdownHighlightedIndex = dropdown_index;
            windowMgr->InvalidateByClass(WindowClass::Dropdown);
        }
        else
        {
            gDropdownLastColourHover = -1;
            WindowTooltipClose();
        }
    }

    static void InputUpdateTooltip(WindowBase* w, WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords)
    {
        if (gTooltipWidget.window_classification == WindowClass::Null)
        {
            if (gTooltipCursor == screenCoords)
            {
                if (gCurrentRealTimeTicks >= _tooltipNotShownTimeout && w != nullptr && widgetIndex != kWidgetIndexNull
                    && WidgetIsVisible(*w, widgetIndex))
                {
                    gTooltipCloseTimeout = gCurrentRealTimeTicks + 8000;
                    WindowTooltipOpen(w, widgetIndex, screenCoords);
                }
            }
            else
            {
                ResetTooltipNotShown();
            }

            gTooltipCloseTimeout = gCurrentRealTimeTicks + 8000;
            gTooltipCursor = screenCoords;
        }
        else
        {
            gTooltipCursor = screenCoords;
            ResetTooltipNotShown();

            if (w == nullptr || gTooltipWidget.window_classification != w->classification
                || gTooltipWidget.window_number != w->number || gTooltipWidget.widget_index != widgetIndex
                || !WidgetIsVisible(*w, widgetIndex))
            {
                WindowTooltipClose();
            }

            if (gCurrentRealTimeTicks >= gTooltipCloseTimeout)
            {
                auto* windowMgr = GetWindowManager();
                windowMgr->CloseByClass(WindowClass::Tooltip);
            }
        }
    }

#pragma endregion

    /**
     *
     *  rct2: 0x006ED990
     */
    void SetCursor(CursorID cursor_id)
    {
        assert(cursor_id != CursorID::Undefined);
        if (_inputState == InputState::Resizing)
        {
            cursor_id = CursorID::DiagonalArrows;
        }
        ContextSetCurrentCursor(cursor_id);
    }

    /**
     *
     *  rct2: 0x006E876D
     */
    void InvalidateScroll()
    {
        auto* windowMgr = GetWindowManager();
        WindowBase* w = windowMgr->FindByNumber(gPressedWidget.window_classification, gPressedWidget.window_number);
        if (w != nullptr)
        {
            // Reset to basic scroll
            w->scrolls[_currentScrollIndex].flags &= 0xFF11;
            windowMgr->InvalidateByNumber(gPressedWidget.window_classification, gPressedWidget.window_number);
        }
    }

    /**
     * rct2: 0x00406C96
     */
    void StoreMouseInput(MouseState state, const ScreenCoordsXY& screenCoords)
    {
        uint32_t writeIndex = _mouseInputQueueWriteIndex;
        uint32_t nextWriteIndex = (writeIndex + 1) % std::size(_mouseInputQueue);

        // Check if the queue is full
        if (nextWriteIndex != _mouseInputQueueReadIndex)
        {
            RCTMouseData* item = &_mouseInputQueue[writeIndex];
            item->x = screenCoords.x;
            item->y = screenCoords.y;
            item->state = state;

            _mouseInputQueueWriteIndex = nextWriteIndex;
        }
    }

    void GameHandleEdgeScroll()
    {
        WindowBase* mainWindow;
        int32_t scrollX, scrollY;

        mainWindow = WindowGetMain();
        if (mainWindow == nullptr)
            return;
        if ((mainWindow->flags & WF_NO_SCROLLING) || (gScreenFlags & (SCREEN_FLAGS_TRACK_MANAGER | SCREEN_FLAGS_TITLE_DEMO)))
            return;
        if (mainWindow->viewport == nullptr)
            return;
        if (!ContextHasFocus())
            return;

        scrollX = 0;
        scrollY = 0;

        // Scroll left / right
        const CursorState* cursorState = ContextGetCursorState();
        if (cursorState->position.x == 0)
            scrollX = -1;
        else if (cursorState->position.x >= ContextGetWidth() - 1)
            scrollX = 1;

        // Scroll up / down
        if (cursorState->position.y == 0)
            scrollY = -1;
        else if (cursorState->position.y >= ContextGetHeight() - 1)
            scrollY = 1;

        InputScrollViewport(ScreenCoordsXY(scrollX, scrollY));
    }

    void InputScrollViewport(const ScreenCoordsXY& scrollScreenCoords)
    {
        WindowBase* mainWindow = WindowGetMain();
        if (mainWindow == nullptr)
            return;

        Viewport* viewport = mainWindow->viewport;
        if (viewport == nullptr)
            return;

        const int32_t speed = Config::Get().general.EdgeScrollingSpeed;

        int32_t multiplier = viewport->zoom.ApplyTo(speed);
        int32_t dx = scrollScreenCoords.x * multiplier;
        int32_t dy = scrollScreenCoords.y * multiplier;

        if (scrollScreenCoords.x != 0)
        {
            // Speed up scrolling horizontally when at the edge of the map
            // so that the speed is consistent with vertical edge scrolling.
            int32_t x = mainWindow->savedViewPos.x + viewport->ViewWidth() / 2 + dx;
            int32_t y = mainWindow->savedViewPos.y + viewport->ViewHeight() / 2;
            int32_t y_dy = mainWindow->savedViewPos.y + viewport->ViewHeight() / 2 + dy;

            auto mapCoord = ViewportPosToMapPos({ x, y }, 0, viewport->rotation);
            auto mapCoord_dy = ViewportPosToMapPos({ x, y_dy }, 0, viewport->rotation);

            // Check if we're crossing the boundary
            // Clamp to the map minimum value
            int32_t at_map_edge = 0;
            int32_t at_map_edge_dy = 0;
            if (mapCoord.x < kMapMinimumXY || mapCoord.y < kMapMinimumXY)
            {
                at_map_edge = 1;
            }
            if (mapCoord_dy.x < kMapMinimumXY || mapCoord_dy.y < kMapMinimumXY)
            {
                at_map_edge_dy = 1;
            }

            // Clamp to the map maximum value (scenario specific)
            auto mapSizeMinus2 = GetMapSizeMinus2();
            if (mapCoord.x > mapSizeMinus2.x || mapCoord.y > mapSizeMinus2.y)
            {
                at_map_edge = 1;
            }
            if (mapCoord_dy.x > mapSizeMinus2.x || mapCoord_dy.y > mapSizeMinus2.y)
            {
                at_map_edge_dy = 1;
            }

            // If we crossed the boundary, multiply the distance by 2
            if (at_map_edge && at_map_edge_dy)
            {
                dx *= 2;
            }

            mainWindow->savedViewPos.x += dx;
            _inputFlags |= INPUT_FLAG_VIEWPORT_SCROLLING;
        }
        if (scrollScreenCoords.y != 0)
        {
            mainWindow->savedViewPos.y += dy;
            _inputFlags |= INPUT_FLAG_VIEWPORT_SCROLLING;
        }
    }
} // namespace OpenRCT2
