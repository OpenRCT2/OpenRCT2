/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <algorithm>
#include <cmath>
#include <iterator>
#include <openrct2-ui/interface/Dropdown.h>
#include <openrct2-ui/interface/Viewport.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/interface/Window.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/Context.h>
#include <openrct2/Game.h>
#include <openrct2/Input.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/audio/audio.h>
#include <openrct2/config/Config.h>
#include <openrct2/interface/Chat.h>
#include <openrct2/interface/Colour.h>
#include <openrct2/interface/Cursors.h>
#include <openrct2/interface/InteractiveConsole.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/platform/Platform.h>
#include <openrct2/ride/RideData.h>
#include <openrct2/scenario/Scenario.h>
#include <openrct2/world/Banner.h>
#include <openrct2/world/Map.h>
#include <openrct2/world/Scenery.h>

struct RCTMouseData
{
    uint32_t x;
    uint32_t y;
    MouseState state;
};

static RCTMouseData _mouseInputQueue[64];
static uint8_t _mouseInputQueueReadIndex = 0;
static uint8_t _mouseInputQueueWriteIndex = 0;

static uint32_t _ticksSinceDragStart;
static WidgetRef _dragWidget;
static uint8_t _dragScrollIndex;
static int32_t _originalWindowWidth;
static int32_t _originalWindowHeight;

static uint8_t _currentScrollIndex;
static uint8_t _currentScrollArea;

ScreenCoordsXY gInputDragLast;

uint16_t gTooltipTimeout;
WidgetRef gTooltipWidget;
ScreenCoordsXY gTooltipCursor;

static int16_t _clickRepeatTicks;

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
    WindowVisitEach([](WindowBase* w) { WindowEventPeriodicUpdateCall(w); });

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
    _ticksSinceDragStart = 0;

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

    if (scroll.flags & HSCROLLBAR_VISIBLE)
    {
        int16_t size = widget.width() - 1;
        if (scroll.flags & VSCROLLBAR_VISIBLE)
            size -= 11;
        size = std::max(0, scroll.h_right - size);
        scroll.h_left = std::min<uint16_t>(std::max(0, scroll.h_left + differentialCoords.x), size);
    }

    if (scroll.flags & VSCROLLBAR_VISIBLE)
    {
        int16_t size = widget.height() - 1;
        if (scroll.flags & HSCROLLBAR_VISIBLE)
            size -= 11;
        size = std::max(0, scroll.v_bottom - size);
        scroll.v_top = std::min<uint16_t>(std::max(0, scroll.v_top + differentialCoords.y), size);
    }

    WidgetScrollUpdateThumbs(*w, widgetIndex);
    WindowInvalidateByNumber(w->classification, w->number);

    ScreenCoordsXY fixedCursorPosition = { static_cast<int32_t>(std::ceil(gInputDragLast.x * gConfigGeneral.WindowScale)),
                                           static_cast<int32_t>(std::ceil(gInputDragLast.y * gConfigGeneral.WindowScale)) };

    ContextSetCursorPosition(fixedCursorPosition);
}

/**
 *
 *  rct2: 0x006E8ACB
 */
static void InputScrollRight(const ScreenCoordsXY& screenCoords, MouseState state)
{
    WindowBase* w = WindowFindByNumber(_dragWidget.window_classification, _dragWidget.window_number);
    if (w == nullptr)
    {
        ContextShowCursor();
        _inputState = InputState::Reset;
        return;
    }

    switch (state)
    {
        case MouseState::Released:
            _ticksSinceDragStart += gCurrentDeltaTime;
            if (screenCoords.x != 0 || screenCoords.y != 0)
            {
                _ticksSinceDragStart = 1000;
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

    // Get window and widget under cursor position
    w = WindowFindFromPoint(screenCoords);
    widgetIndex = w == nullptr ? -1 : WindowFindWidgetFromPoint(*w, screenCoords);
    widget = widgetIndex == -1 ? nullptr : &w->widgets[widgetIndex];

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
                    WindowCloseByClass(WindowClass::Tooltip);

                    if (w != nullptr)
                    {
                        w = WindowBringToFront(*w);
                    }

                    if (widgetIndex != -1)
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
            w = WindowFindByNumber(_dragWidget.window_classification, _dragWidget.window_number);
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
                if (_ticksSinceDragStart < 500)
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
            w = WindowFindByNumber(_dragWidget.window_classification, _dragWidget.window_number);
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

                    w = WindowFindByNumber(gCurrentToolWidget.window_classification, gCurrentToolWidget.window_number);
                    if (w == nullptr)
                    {
                        break;
                    }

                    WindowEventToolDragCall(w, gCurrentToolWidget.widget_index, screenCoords);
                    break;
                case MouseState::LeftRelease:
                    _inputState = InputState::Reset;
                    if (_dragWidget.window_number == w->number)
                    {
                        if ((_inputFlags & INPUT_FLAG_TOOL_ACTIVE))
                        {
                            w = WindowFindByNumber(gCurrentToolWidget.window_classification, gCurrentToolWidget.window_number);
                            if (w != nullptr)
                            {
                                WindowEventToolUpCall(w, gCurrentToolWidget.widget_index, screenCoords);
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
            w = WindowFindByNumber(_dragWidget.window_classification, _dragWidget.window_number);
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

    snapProximity = (w.flags & WF_NO_SNAPPING) ? 0 : gConfigGeneral.WindowSnapProximity;
    WindowMoveAndSnap(w, newScreenCoords - lastScreenCoords, snapProximity);
}

static void InputWindowPositionEnd(WindowBase& w, const ScreenCoordsXY& screenCoords)
{
    _inputState = InputState::Normal;
    gTooltipTimeout = 0;
    gTooltipWidget = _dragWidget;
    WindowEventMovedCall(&w, screenCoords);
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
    gTooltipTimeout = 0;
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
    _ticksSinceDragStart = 0;
    auto cursorPosition = ContextGetCursorPosition();
    gInputDragLast = cursorPosition;
    if (!gConfigGeneral.InvertViewportDrag)
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
    const CursorState* cursorState = ContextGetCursorState();

    auto differentialCoords = newDragCoords - gInputDragLast;
    w = WindowFindByNumber(_dragWidget.window_classification, _dragWidget.window_number);

    // #3294: Window can be closed during a drag session, so just finish
    //        the session if the window no longer exists
    if (w == nullptr)
    {
        InputViewportDragEnd();
        return;
    }

    viewport = w->viewport;
    _ticksSinceDragStart += gCurrentDeltaTime;
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
            _ticksSinceDragStart = 1000;

            differentialCoords.x = (viewport->zoom + 1).ApplyTo(differentialCoords.x);
            differentialCoords.y = (viewport->zoom + 1).ApplyTo(differentialCoords.y);
            if (gConfigGeneral.InvertViewportDrag)
            {
                w->savedViewPos -= differentialCoords;
            }
            else
            {
                w->savedViewPos += differentialCoords;
            }
        }
    }

    if (cursorState->touch || gConfigGeneral.InvertViewportDrag)
    {
        gInputDragLast = newDragCoords;
    }
    else
    {
        ContextSetCursorPosition(gInputDragLast);
    }
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
    WindowEventScrollSelectCall(&w, scroll_id, scroll_area);
    if (scroll_area == SCROLL_PART_VIEW)
    {
        WindowEventScrollMousedownCall(&w, scroll_id, scrollCoords);
        return;
    }

    const auto& widg = w.widgets[widgetIndex];
    auto& scroll = w.scrolls[scroll_id];

    int32_t widget_width = widg.width() - 1;
    if (scroll.flags & VSCROLLBAR_VISIBLE)
        widget_width -= SCROLLBAR_WIDTH + 1;
    int32_t widget_content_width = std::max(scroll.h_right - widget_width, 0);

    int32_t widget_height = widg.bottom - widg.top - 1;
    if (scroll.flags & HSCROLLBAR_VISIBLE)
        widget_height -= SCROLLBAR_WIDTH + 1;
    int32_t widget_content_height = std::max(scroll.v_bottom - widget_height, 0);

    switch (scroll_area)
    {
        case SCROLL_PART_HSCROLLBAR_LEFT:
            scroll.h_left = std::max(scroll.h_left - 3, 0);
            break;
        case SCROLL_PART_HSCROLLBAR_RIGHT:
            scroll.h_left = std::min(scroll.h_left + 3, widget_content_width);
            break;
        case SCROLL_PART_HSCROLLBAR_LEFT_TROUGH:
            scroll.h_left = std::max(scroll.h_left - widget_width, 0);
            break;
        case SCROLL_PART_HSCROLLBAR_RIGHT_TROUGH:
            scroll.h_left = std::min(scroll.h_left + widget_width, widget_content_width);
            break;
        case SCROLL_PART_VSCROLLBAR_TOP:
            scroll.v_top = std::max(scroll.v_top - 3, 0);
            break;
        case SCROLL_PART_VSCROLLBAR_BOTTOM:
            scroll.v_top = std::min(scroll.v_top + 3, widget_content_height);
            break;
        case SCROLL_PART_VSCROLLBAR_TOP_TROUGH:
            scroll.v_top = std::max(scroll.v_top - widget_height, 0);
            break;
        case SCROLL_PART_VSCROLLBAR_BOTTOM_TROUGH:
            scroll.v_top = std::min(scroll.v_top + widget_height, widget_content_height);
            break;
        default:
            break;
    }
    WidgetScrollUpdateThumbs(w, widgetIndex);
    WindowInvalidateByNumber(w.classification, w.number);
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
            WindowEventScrollMousedragCall(&w, scroll_id, newScreenCoords);
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

    if (WindowFindByNumber(w.classification, w.number) != nullptr)
    {
        int32_t newLeft;
        newLeft = scroll.h_right;
        newLeft *= x;
        x = widget.width() - 21;
        if (scroll.flags & VSCROLLBAR_VISIBLE)
            x -= SCROLLBAR_WIDTH + 1;
        newLeft /= x;
        x = newLeft;
        scroll.flags |= HSCROLLBAR_THUMB_PRESSED;
        newLeft = scroll.h_left;
        newLeft += x;
        if (newLeft < 0)
            newLeft = 0;
        x = widget.width() - 1;
        if (scroll.flags & VSCROLLBAR_VISIBLE)
            x -= SCROLLBAR_WIDTH + 1;
        x *= -1;
        x += scroll.h_right;
        if (x < 0)
            x = 0;
        if (newLeft > x)
            newLeft = x;
        scroll.h_left = newLeft;
        WidgetScrollUpdateThumbs(w, widgetIndex);
        WidgetInvalidateByNumber(w.classification, w.number, widgetIndex);
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

    if (WindowFindByNumber(w.classification, w.number) != nullptr)
    {
        int32_t newTop;
        newTop = scroll.v_bottom;
        newTop *= y;
        y = widget.height() - 21;
        if (scroll.flags & HSCROLLBAR_VISIBLE)
            y -= SCROLLBAR_WIDTH + 1;
        newTop /= y;
        y = newTop;
        scroll.flags |= VSCROLLBAR_THUMB_PRESSED;
        newTop = scroll.v_top;
        newTop += y;
        if (newTop < 0)
            newTop = 0;
        y = widget.height() - 1;
        if (scroll.flags & HSCROLLBAR_VISIBLE)
            y -= SCROLLBAR_WIDTH + 1;
        y *= -1;
        y += scroll.v_bottom;
        if (y < 0)
            y = 0;
        if (newTop > y)
            newTop = y;
        scroll.v_top = newTop;
        WidgetScrollUpdateThumbs(w, widgetIndex);
        WidgetInvalidateByNumber(w.classification, w.number, widgetIndex);
    }
}

/**
 *
 *  rct2: 0x006E9A60
 */
static void InputScrollPartUpdateHLeft(WindowBase& w, WidgetIndex widgetIndex, int32_t scroll_id)
{
    if (WindowFindByNumber(w.classification, w.number) != nullptr)
    {
        auto& scroll = w.scrolls[scroll_id];
        scroll.flags |= HSCROLLBAR_LEFT_PRESSED;
        if (scroll.h_left >= 3)
            scroll.h_left -= 3;
        WidgetScrollUpdateThumbs(w, widgetIndex);
        WidgetInvalidateByNumber(w.classification, w.number, widgetIndex);
    }
}

/**
 *
 *  rct2: 0x006E9ABF
 */
static void InputScrollPartUpdateHRight(WindowBase& w, WidgetIndex widgetIndex, int32_t scroll_id)
{
    const auto& widget = w.widgets[widgetIndex];
    if (WindowFindByNumber(w.classification, w.number) != nullptr)
    {
        auto& scroll = w.scrolls[scroll_id];
        scroll.flags |= HSCROLLBAR_RIGHT_PRESSED;
        scroll.h_left += 3;
        int32_t newLeft = widget.width() - 1;
        if (scroll.flags & VSCROLLBAR_VISIBLE)
            newLeft -= SCROLLBAR_WIDTH + 1;
        newLeft *= -1;
        newLeft += scroll.h_right;
        if (newLeft < 0)
            newLeft = 0;
        if (scroll.h_left > newLeft)
            scroll.h_left = newLeft;
        WidgetScrollUpdateThumbs(w, widgetIndex);
        WidgetInvalidateByNumber(w.classification, w.number, widgetIndex);
    }
}

/**
 *
 *  rct2: 0x006E9C37
 */
static void InputScrollPartUpdateVTop(WindowBase& w, WidgetIndex widgetIndex, int32_t scroll_id)
{
    if (WindowFindByNumber(w.classification, w.number) != nullptr)
    {
        auto& scroll = w.scrolls[scroll_id];
        scroll.flags |= VSCROLLBAR_UP_PRESSED;
        if (scroll.v_top >= 3)
            scroll.v_top -= 3;
        WidgetScrollUpdateThumbs(w, widgetIndex);
        WidgetInvalidateByNumber(w.classification, w.number, widgetIndex);
    }
}

/**
 *
 *  rct2: 0x006E9C96
 */
static void InputScrollPartUpdateVBottom(WindowBase& w, WidgetIndex widgetIndex, int32_t scroll_id)
{
    const auto& widget = w.widgets[widgetIndex];
    if (WindowFindByNumber(w.classification, w.number) != nullptr)
    {
        auto& scroll = w.scrolls[scroll_id];
        scroll.flags |= VSCROLLBAR_DOWN_PRESSED;
        scroll.v_top += 3;
        int32_t newTop = widget.height() - 1;
        if (scroll.flags & HSCROLLBAR_VISIBLE)
            newTop -= SCROLLBAR_WIDTH + 1;
        newTop *= -1;
        newTop += scroll.v_bottom;
        if (newTop < 0)
            newTop = 0;
        if (scroll.v_top > newTop)
            scroll.v_top = newTop;
        WidgetScrollUpdateThumbs(w, widgetIndex);
        WidgetInvalidateByNumber(w.classification, w.number, widgetIndex);
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
        widget = &w->widgets[widgetIndex];
    }

    InputWidgetOverChangeCheck(windowClass, windowNumber, widgetIndex);

    if (w != nullptr && widgetIndex != -1 && widget->type == WindowWidgetType::Scroll)
    {
        int32_t scroll_part, scrollId;
        ScreenCoordsXY newScreenCoords;
        WidgetScrollGetPart(*w, widget, screenCoords, newScreenCoords, &scroll_part, &scrollId);

        if (scroll_part != SCROLL_PART_VIEW)
            WindowTooltipClose();
        else
        {
            WindowEventScrollMouseoverCall(w, scrollId, newScreenCoords);
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
    if (widgetIndex == -1)
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
 * Used to invalidate flat button widgets when the mouse leaves and enters them. This should be generalised so that all widgets
 * can use this in the future.
 */
static void InputWidgetOverFlatbuttonInvalidate()
{
    WindowBase* w = WindowFindByNumber(gHoverWidget.window_classification, gHoverWidget.window_number);
    if (w != nullptr)
    {
        WindowEventInvalidateCall(w);
        if (w->widgets[gHoverWidget.widget_index].type == WindowWidgetType::FlatBtn)
        {
            WidgetInvalidateByNumber(gHoverWidget.window_classification, gHoverWidget.window_number, gHoverWidget.widget_index);
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

    WindowCloseByClass(WindowClass::Error);
    WindowCloseByClass(WindowClass::Tooltip);

    // Window might have changed position in the list, therefore find it again
    w = WindowFindByNumber(windowClass, windowNumber);
    if (w == nullptr)
        return;

    w = WindowBringToFront(*w);
    if (widgetIndex == -1)
        return;

    if (windowClass != gCurrentTextBox.window.classification || windowNumber != gCurrentTextBox.window.number
        || widgetIndex != gCurrentTextBox.widget_index)
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
                w = WindowFindByNumber(gCurrentToolWidget.window_classification, gCurrentToolWidget.window_number);
                if (w != nullptr)
                {
                    InputSetFlag(INPUT_FLAG_4, true);
                    WindowEventToolDownCall(w, gCurrentToolWidget.widget_index, screenCoords);
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
        case WindowWidgetType::Placeholder:
        case WindowWidgetType::Last:
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
                _clickRepeatTicks = 1;

                WidgetInvalidateByNumber(windowClass, windowNumber, widgetIndex);
                WindowEventMouseDownCall(w, widgetIndex);
            }
            break;
    }
}

#pragma endregion

/**
 *
 *  rct2: 0x006ED833
 */
void ProcessMouseOver(const ScreenCoordsXY& screenCoords)
{
    WindowBase* window;

    CursorID cursorId = CursorID::Arrow;
    auto ft = Formatter();
    ft.Add<StringId>(STR_NONE);
    SetMapTooltip(ft);
    window = WindowFindFromPoint(screenCoords);

    if (window != nullptr)
    {
        WidgetIndex widgetId = WindowFindWidgetFromPoint(*window, screenCoords);
        if (widgetId != -1)
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
                    cursorId = WindowEventCursorCall(window, widgetId, scrollCoords);
                    if (cursorId == CursorID::Undefined)
                        cursorId = CursorID::Arrow;
                    break;
                }
                default:
                    cursorId = WindowEventCursorCall(window, widgetId, screenCoords);
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
        WindowBase* w = WindowFindByNumber(gCurrentToolWidget.window_classification, gCurrentToolWidget.window_number);

        if (w == nullptr)
            ToolCancel();
        else if (InputGetState() != InputState::ViewportRight)
            WindowEventToolUpdateCall(w, gCurrentToolWidget.widget_index, screenCoords);
    }
}

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

    WindowBase* cursor_w = WindowFindByNumber(cursor_w_class, cursor_w_number);
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
            if (gInputPlaceObjectModifier & PLACE_OBJECT_MODIFIER_SHIFT_Z)
            {
                gLastCloseModifier.window.number = w->number;
                gLastCloseModifier.window.classification = w->classification;
                gLastCloseModifier.modifier = CloseWindowModifier::Shift;
            }
            else if (gInputPlaceObjectModifier & PLACE_OBJECT_MODIFIER_COPY_Z)
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

            if (_clickRepeatTicks != 0)
            {
                _clickRepeatTicks++;

                // Handle click repeat
                if (_clickRepeatTicks >= 16 && (_clickRepeatTicks & 3) == 0)
                {
                    if (WidgetIsHoldable(*w, widgetIndex))
                    {
                        WindowEventMouseDownCall(w, widgetIndex);
                    }
                }
            }

            if (_inputFlags & INPUT_FLAG_WIDGET_PRESSED)
            {
                if (_inputState == InputState::DropdownActive)
                {
                    gDropdownHighlightedIndex = gDropdownDefaultIndex;
                    WindowInvalidateByClass(WindowClass::Dropdown);
                }
                return;
            }

            _inputFlags |= INPUT_FLAG_WIDGET_PRESSED;
            WidgetInvalidateByNumber(cursor_w_class, cursor_w_number, widgetIndex);
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
                            || (dropdown_index < Dropdown::ItemsMaxSize && Dropdown::IsDisabled(dropdown_index))
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

                    WindowCloseByClass(WindowClass::Dropdown);

                    if (dropdownCleanup)
                    {
                        // Update w as it will be invalid after closing the dropdown window
                        w = WindowFindByNumber(wClass, wNumber);
                    }
                    else
                    {
                        cursor_w = WindowFindByNumber(cursor_w_class, cursor_w_number);
                        if (_inputFlags & INPUT_FLAG_WIDGET_PRESSED)
                        {
                            _inputFlags &= ~INPUT_FLAG_WIDGET_PRESSED;
                            WidgetInvalidateByNumber(cursor_w_class, cursor_w_number, cursor_widgetIndex);
                        }

                        _inputState = InputState::Normal;
                        gTooltipTimeout = 0;
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
                        WindowEventDropdownCall(
                            cursor_w, cursor_widgetIndex,
                            (gDropdownIsColour) ? ColourToPaletteIndex(dropdown_index) : dropdown_index);
                    }
                }
            }

            _inputState = InputState::Normal;

            if (state == MouseState::RightPress)
            {
                return;
            }

            gTooltipTimeout = 0;
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

            WidgetInvalidateByNumber(cursor_w_class, cursor_w_number, widgetIndex);
            WindowEventMouseUpCall(w, widgetIndex);
            return;

        default:
            return;
    }

    _clickRepeatTicks = 0;
    if (_inputState != InputState::DropdownActive)
    {
        // Hold down widget and drag outside of area??
        if (_inputFlags & INPUT_FLAG_WIDGET_PRESSED)
        {
            _inputFlags &= ~INPUT_FLAG_WIDGET_PRESSED;
            WidgetInvalidateByNumber(cursor_w_class, cursor_w_number, cursor_widgetIndex);
        }
        return;
    }

    gDropdownHighlightedIndex = -1;
    WindowInvalidateByClass(WindowClass::Dropdown);
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

            static constexpr const StringId _colourTooltips[] = {
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
                STR_COLOUR_DARK_OLIVE_DARK_TIP,
                STR_COLOUR_DARK_OLIVE_LIGHT_TIP,
                STR_COLOUR_SATURATED_BROWN_LIGHT_TIP,
                STR_COLOUR_BORDEAUX_RED_DARK_TIP,
                STR_COLOUR_BORDEAUX_RED_LIGHT_TIP,
                STR_COLOUR_GRASS_GREEN_DARK_TIP,
                STR_COLOUR_GRASS_GREEN_LIGHT_TIP,
                STR_COLOUR_OLIVE_DARK_TIP,
                STR_COLOUR_OLIVE_LIGHT_TIP,
                STR_COLOUR_SATURATED_GREEN_LIGHT_TIP,
                STR_COLOUR_TAN_DARK_TIP,
                STR_COLOUR_TAN_LIGHT_TIP,
                STR_COLOUR_DULL_PURPLE_LIGHT_TIP,
                STR_COLOUR_DULL_GREEN_DARK_TIP,
                STR_COLOUR_DULL_GREEN_LIGHT_TIP,
                STR_COLOUR_SATURATED_PURPLE_DARK_TIP,
                STR_COLOUR_SATURATED_PURPLE_LIGHT_TIP,
                STR_COLOUR_ORANGE_LIGHT_TIP,
                STR_COLOUR_AQUA_DARK_TIP,
                STR_COLOUR_MAGENTA_LIGHT_TIP,
                STR_COLOUR_DULL_BROWN_DARK_TIP,
                STR_COLOUR_DULL_BROWN_LIGHT_TIP,
                STR_COLOUR_INVISIBLE_TIP,
                STR_COLOUR_VOID_TIP,
            };

            // Since there is discontinuity between old and new palette indices, need to do some math to get proper array index
            auto tooltipIndex = ColourToPaletteIndex(dropdown_index);
            if (tooltipIndex > COLOUR_NUM_ORIGINAL)
            {
                tooltipIndex -= COLOUR_ID_EXTENDED_OFFSET;
            }

            WindowTooltipShow(OpenRCT2String{ _colourTooltips[tooltipIndex], {} }, screenCoords);
        }

        if (dropdown_index < Dropdown::ItemsMaxSize && Dropdown::IsDisabled(dropdown_index))
        {
            return;
        }

        if (gDropdownItems[dropdown_index].IsSeparator())
        {
            return;
        }

        gDropdownHighlightedIndex = dropdown_index;
        WindowInvalidateByClass(WindowClass::Dropdown);
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
            _tooltipNotShownTicks++;
            if (_tooltipNotShownTicks > 50 && w != nullptr && WidgetIsVisible(*w, widgetIndex))
            {
                gTooltipTimeout = 0;
                WindowTooltipOpen(w, widgetIndex, screenCoords);
            }
        }

        gTooltipTimeout = 0;
        gTooltipCursor = screenCoords;
    }
    else
    {
        ResetTooltipNotShown();

        if (w == nullptr || gTooltipWidget.window_classification != w->classification
            || gTooltipWidget.window_number != w->number || gTooltipWidget.widget_index != widgetIndex
            || !WidgetIsVisible(*w, widgetIndex))
        {
            WindowTooltipClose();
        }

        gTooltipTimeout += gCurrentDeltaTime;
        if (gTooltipTimeout >= 8000)
        {
            WindowCloseByClass(WindowClass::Tooltip);
        }
    }
}

#pragma endregion

#pragma region Keyboard input

/**
 *
 *  rct2: 0x00406CD2
 */
int32_t GetNextKey()
{
    uint8_t* keysPressed = const_cast<uint8_t*>(ContextGetKeysPressed());
    for (int32_t i = 0; i < 221; i++)
    {
        if (keysPressed[i])
        {
            keysPressed[i] = 0;
            return i;
        }
    }

    return 0;
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
    WindowBase* w = WindowFindByNumber(gPressedWidget.window_classification, gPressedWidget.window_number);
    if (w != nullptr)
    {
        // Reset to basic scroll
        w->scrolls[_currentScrollIndex].flags &= 0xFF11;
        WindowInvalidateByNumber(gPressedWidget.window_classification, gPressedWidget.window_number);
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

bool InputTestPlaceObjectModifier(PLACE_OBJECT_MODIFIER modifier)
{
    return gInputPlaceObjectModifier & modifier;
}

void InputScrollViewport(const ScreenCoordsXY& scrollScreenCoords)
{
    WindowBase* mainWindow = WindowGetMain();
    Viewport* viewport = mainWindow->viewport;

    const int32_t speed = gConfigGeneral.EdgeScrollingSpeed;

    int32_t multiplier = viewport->zoom.ApplyTo(speed);
    int32_t dx = scrollScreenCoords.x * multiplier;
    int32_t dy = scrollScreenCoords.y * multiplier;

    if (scrollScreenCoords.x != 0)
    {
        // Speed up scrolling horizontally when at the edge of the map
        // so that the speed is consistent with vertical edge scrolling.
        int32_t x = mainWindow->savedViewPos.x + viewport->view_width / 2 + dx;
        int32_t y = mainWindow->savedViewPos.y + viewport->view_height / 2;
        int32_t y_dy = mainWindow->savedViewPos.y + viewport->view_height / 2 + dy;

        auto mapCoord = ViewportPosToMapPos({ x, y }, 0);
        auto mapCoord_dy = ViewportPosToMapPos({ x, y_dy }, 0);

        // Check if we're crossing the boundary
        // Clamp to the map minimum value
        int32_t at_map_edge = 0;
        int32_t at_map_edge_dy = 0;
        if (mapCoord.x < MAP_MINIMUM_X_Y || mapCoord.y < MAP_MINIMUM_X_Y)
        {
            at_map_edge = 1;
        }
        if (mapCoord_dy.x < MAP_MINIMUM_X_Y || mapCoord_dy.y < MAP_MINIMUM_X_Y)
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
