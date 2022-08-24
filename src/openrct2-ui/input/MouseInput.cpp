/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
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
static widget_ref _dragWidget;
static uint8_t _dragScrollIndex;
static int32_t _originalWindowWidth;
static int32_t _originalWindowHeight;

static uint8_t _currentScrollIndex;
static uint8_t _currentScrollArea;

ScreenCoordsXY gInputDragLast;

uint16_t gTooltipTimeout;
widget_ref gTooltipWidget;
ScreenCoordsXY gTooltipCursor;

static int16_t _clickRepeatTicks;

static MouseState GameGetNextInput(ScreenCoordsXY& screenCoords);
static void InputWidgetOver(const ScreenCoordsXY& screenCoords, rct_window* w, WidgetIndex widgetIndex);
static void InputWidgetOverChangeCheck(WindowClass windowClass, rct_windownumber windowNumber, WidgetIndex widgetIndex);
static void InputWidgetOverFlatbuttonInvalidate();
void ProcessMouseOver(const ScreenCoordsXY& screenCoords);
void ProcessMouseTool(const ScreenCoordsXY& screenCoords);
void InvalidateScroll();
static RCTMouseData* GetMouseInput();
void TileElementRightClick(int32_t type, TileElement* tileElement, const ScreenCoordsXY& screenCoords);
static void GameHandleInputMouse(const ScreenCoordsXY& screenCoords, MouseState state);
static void InputWidgetLeft(const ScreenCoordsXY& screenCoords, rct_window* w, WidgetIndex widgetIndex);
void InputStateWidgetPressed(
    const ScreenCoordsXY& screenCoords, MouseState state, WidgetIndex widgetIndex, rct_window* w, rct_widget* widget);
void SetCursor(CursorID cursor_id);
static void InputWindowPositionContinue(
    rct_window& w, const ScreenCoordsXY& lastScreenCoords, const ScreenCoordsXY& newScreenCoords);
static void InputWindowPositionEnd(rct_window& w, const ScreenCoordsXY& screenCoords);
static void InputWindowResizeBegin(rct_window& w, WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords);
static void InputWindowResizeContinue(rct_window& w, const ScreenCoordsXY& screenCoords);
static void InputWindowResizeEnd();
static void InputViewportDragBegin(rct_window& w);
static void InputViewportDragContinue();
static void InputViewportDragEnd();
static void InputScrollBegin(rct_window& w, WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords);
static void InputScrollContinue(rct_window& w, WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords);
static void InputScrollEnd();
static void InputScrollPartUpdateHThumb(rct_window& w, WidgetIndex widgetIndex, int32_t x, int32_t scroll_id);
static void InputScrollPartUpdateHLeft(rct_window& w, WidgetIndex widgetIndex, int32_t scroll_id);
static void InputScrollPartUpdateHRight(rct_window& w, WidgetIndex widgetIndex, int32_t scroll_id);
static void InputScrollPartUpdateVThumb(rct_window& w, WidgetIndex widgetIndex, int32_t y, int32_t scroll_id);
static void InputScrollPartUpdateVTop(rct_window& w, WidgetIndex widgetIndex, int32_t scroll_id);
static void InputScrollPartUpdateVBottom(rct_window& w, WidgetIndex widgetIndex, int32_t scroll_id);
static void InputUpdateTooltip(rct_window* w, WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords);

#pragma region Mouse input

/**
 *
 *  rct2: 0x006EA627
 */
void GameHandleInput()
{
    window_visit_each([](rct_window* w) { window_event_periodic_update_call(w); });

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
        int32_t screenWidth = context_get_width();
        int32_t screenHeight = context_get_height();
        screenCoords.x = std::clamp(screenCoords.x, 0, screenWidth - 1);
        screenCoords.y = std::clamp(screenCoords.y, 0, screenHeight - 1);

        GameHandleInputMouse(screenCoords, state);
        ProcessMouseOver(screenCoords);
        ProcessMouseTool(screenCoords);
    }

    window_visit_each([](rct_window* w) { window_event_unknown_08_call(w); });
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
        const CursorState* cursorState = context_get_cursor_state();
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
static void InputScrollDragBegin(const ScreenCoordsXY& screenCoords, rct_window* w, WidgetIndex widgetIndex)
{
    _inputState = InputState::ScrollRight;
    gInputDragLast = screenCoords;
    _dragWidget.window_classification = w->classification;
    _dragWidget.window_number = w->number;
    _dragWidget.widget_index = widgetIndex;
    _ticksSinceDragStart = 0;

    _dragScrollIndex = window_get_scroll_data_index(*w, widgetIndex);
    context_hide_cursor();
}

/**
 * Based on (heavily changed)
 *  rct2: 0x006E9E0E,  0x006E9ED0
 */
static void InputScrollDragContinue(const ScreenCoordsXY& screenCoords, rct_window* w)
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
    window_invalidate_by_number(w->classification, w->number);

    ScreenCoordsXY fixedCursorPosition = { static_cast<int32_t>(std::ceil(gInputDragLast.x * gConfigGeneral.window_scale)),
                                           static_cast<int32_t>(std::ceil(gInputDragLast.y * gConfigGeneral.window_scale)) };

    context_set_cursor_position(fixedCursorPosition);
}

/**
 *
 *  rct2: 0x006E8ACB
 */
static void InputScrollRight(const ScreenCoordsXY& screenCoords, MouseState state)
{
    rct_window* w = window_find_by_number(_dragWidget.window_classification, _dragWidget.window_number);
    if (w == nullptr)
    {
        context_show_cursor();
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
            context_show_cursor();
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
    rct_window* w;
    rct_widget* widget;
    WidgetIndex widgetIndex;

    // Get window and widget under cursor position
    w = window_find_from_point(screenCoords);
    widgetIndex = w == nullptr ? -1 : window_find_widget_from_point(*w, screenCoords);
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
                    window_close_by_class(WindowClass::Tooltip);

                    if (w != nullptr)
                    {
                        w = window_bring_to_front(*w);
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
            w = window_find_by_number(_dragWidget.window_classification, _dragWidget.window_number);
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
            w = window_find_by_number(_dragWidget.window_classification, _dragWidget.window_number);
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

                    if (w->classification != _dragWidget.window_classification || w->number != _dragWidget.window_number
                        || !(_inputFlags & INPUT_FLAG_TOOL_ACTIVE))
                    {
                        break;
                    }

                    w = window_find_by_number(gCurrentToolWidget.window_classification, gCurrentToolWidget.window_number);
                    if (w == nullptr)
                    {
                        break;
                    }

                    window_event_tool_drag_call(w, gCurrentToolWidget.widget_index, screenCoords);
                    break;
                case MouseState::LeftRelease:
                    _inputState = InputState::Reset;
                    if (_dragWidget.window_number == w->number)
                    {
                        if ((_inputFlags & INPUT_FLAG_TOOL_ACTIVE))
                        {
                            w = window_find_by_number(
                                gCurrentToolWidget.window_classification, gCurrentToolWidget.window_number);
                            if (w != nullptr)
                            {
                                window_event_tool_up_call(w, gCurrentToolWidget.widget_index, screenCoords);
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
            w = window_find_by_number(_dragWidget.window_classification, _dragWidget.window_number);
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

void InputWindowPositionBegin(rct_window& w, WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords)
{
    _inputState = InputState::PositioningWindow;
    gInputDragLast = screenCoords - w.windowPos;
    _dragWidget.window_classification = w.classification;
    _dragWidget.window_number = w.number;
    _dragWidget.widget_index = widgetIndex;
}

static void InputWindowPositionContinue(
    rct_window& w, const ScreenCoordsXY& lastScreenCoords, const ScreenCoordsXY& newScreenCoords)
{
    int32_t snapProximity;

    snapProximity = (w.flags & WF_NO_SNAPPING) ? 0 : gConfigGeneral.window_snap_proximity;
    window_move_and_snap(w, newScreenCoords - lastScreenCoords, snapProximity);
}

static void InputWindowPositionEnd(rct_window& w, const ScreenCoordsXY& screenCoords)
{
    _inputState = InputState::Normal;
    gTooltipTimeout = 0;
    gTooltipWidget = _dragWidget;
    window_event_moved_call(&w, screenCoords);
}

static void InputWindowResizeBegin(rct_window& w, WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords)
{
    _inputState = InputState::Resizing;
    gInputDragLast = screenCoords;
    _dragWidget.window_classification = w.classification;
    _dragWidget.window_number = w.number;
    _dragWidget.widget_index = widgetIndex;
    _originalWindowWidth = w.width;
    _originalWindowHeight = w.height;
}

static void InputWindowResizeContinue(rct_window& w, const ScreenCoordsXY& screenCoords)
{
    if (screenCoords.y < static_cast<int32_t>(context_get_height()) - 2)
    {
        auto differentialCoords = screenCoords - gInputDragLast;
        int32_t targetWidth = _originalWindowWidth + differentialCoords.x - w.width;
        int32_t targetHeight = _originalWindowHeight + differentialCoords.y - w.height;

        window_resize(w, targetWidth, targetHeight);
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

static void InputViewportDragBegin(rct_window& w)
{
    w.flags &= ~WF_SCROLLING_TO_LOCATION;
    _inputState = InputState::ViewportRight;
    _dragWidget.window_classification = w.classification;
    _dragWidget.window_number = w.number;
    _ticksSinceDragStart = 0;
    auto cursorPosition = context_get_cursor_position();
    gInputDragLast = cursorPosition;
    context_hide_cursor();

    window_unfollow_sprite(w);
    // gInputFlags |= INPUT_FLAG_5;
}

static void InputViewportDragContinue()
{
    rct_window* w;
    rct_viewport* viewport;

    auto newDragCoords = context_get_cursor_position();
    const CursorState* cursorState = context_get_cursor_state();

    auto differentialCoords = newDragCoords - gInputDragLast;
    w = window_find_by_number(_dragWidget.window_classification, _dragWidget.window_number);

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
        context_show_cursor();
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
            if (gConfigGeneral.invert_viewport_drag)
            {
                w->savedViewPos -= differentialCoords;
            }
            else
            {
                w->savedViewPos += differentialCoords;
            }
        }
    }

    if (cursorState->touch)
    {
        gInputDragLast = newDragCoords;
    }
    else
    {
        context_set_cursor_position(gInputDragLast);
    }
}

static void InputViewportDragEnd()
{
    _inputState = InputState::Reset;
    context_show_cursor();
}

#pragma endregion

#pragma region Scroll bars

static void InputScrollBegin(rct_window& w, WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords)
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
    window_event_unknown_15_call(&w, scroll_id, scroll_area);
    if (scroll_area == SCROLL_PART_VIEW)
    {
        window_event_scroll_mousedown_call(&w, scroll_id, scrollCoords);
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
    window_invalidate_by_number(w.classification, w.number);
}

static void InputScrollContinue(rct_window& w, WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords)
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
            window_event_scroll_mousedrag_call(&w, scroll_id, newScreenCoords);
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
static void InputScrollPartUpdateHThumb(rct_window& w, WidgetIndex widgetIndex, int32_t x, int32_t scroll_id)
{
    const auto& widget = w.widgets[widgetIndex];
    auto& scroll = w.scrolls[scroll_id];

    if (window_find_by_number(w.classification, w.number) != nullptr)
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
        widget_invalidate_by_number(w.classification, w.number, widgetIndex);
    }
}

/**
 *
 *  rct2: 0x006E99A9
 */
static void InputScrollPartUpdateVThumb(rct_window& w, WidgetIndex widgetIndex, int32_t y, int32_t scroll_id)
{
    const auto& widget = w.widgets[widgetIndex];
    auto& scroll = w.scrolls[scroll_id];

    if (window_find_by_number(w.classification, w.number) != nullptr)
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
        widget_invalidate_by_number(w.classification, w.number, widgetIndex);
    }
}

/**
 *
 *  rct2: 0x006E9A60
 */
static void InputScrollPartUpdateHLeft(rct_window& w, WidgetIndex widgetIndex, int32_t scroll_id)
{
    if (window_find_by_number(w.classification, w.number) != nullptr)
    {
        auto& scroll = w.scrolls[scroll_id];
        scroll.flags |= HSCROLLBAR_LEFT_PRESSED;
        if (scroll.h_left >= 3)
            scroll.h_left -= 3;
        WidgetScrollUpdateThumbs(w, widgetIndex);
        widget_invalidate_by_number(w.classification, w.number, widgetIndex);
    }
}

/**
 *
 *  rct2: 0x006E9ABF
 */
static void InputScrollPartUpdateHRight(rct_window& w, WidgetIndex widgetIndex, int32_t scroll_id)
{
    const auto& widget = w.widgets[widgetIndex];
    if (window_find_by_number(w.classification, w.number) != nullptr)
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
        widget_invalidate_by_number(w.classification, w.number, widgetIndex);
    }
}

/**
 *
 *  rct2: 0x006E9C37
 */
static void InputScrollPartUpdateVTop(rct_window& w, WidgetIndex widgetIndex, int32_t scroll_id)
{
    if (window_find_by_number(w.classification, w.number) != nullptr)
    {
        auto& scroll = w.scrolls[scroll_id];
        scroll.flags |= VSCROLLBAR_UP_PRESSED;
        if (scroll.v_top >= 3)
            scroll.v_top -= 3;
        WidgetScrollUpdateThumbs(w, widgetIndex);
        widget_invalidate_by_number(w.classification, w.number, widgetIndex);
    }
}

/**
 *
 *  rct2: 0x006E9C96
 */
static void InputScrollPartUpdateVBottom(rct_window& w, WidgetIndex widgetIndex, int32_t scroll_id)
{
    const auto& widget = w.widgets[widgetIndex];
    if (window_find_by_number(w.classification, w.number) != nullptr)
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
        widget_invalidate_by_number(w.classification, w.number, widgetIndex);
    }
}

#pragma endregion

#pragma region Widgets

/**
 *
 *  rct2: 0x006E9253
 */
static void InputWidgetOver(const ScreenCoordsXY& screenCoords, rct_window* w, WidgetIndex widgetIndex)
{
    WindowClass windowClass = WindowClass::Null;
    rct_windownumber windowNumber = 0;
    rct_widget* widget = nullptr;

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
            window_event_scroll_mouseover_call(w, scrollId, newScreenCoords);
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
    rct_window* w = window_find_by_number(gHoverWidget.window_classification, gHoverWidget.window_number);
    if (w != nullptr)
    {
        window_event_invalidate_call(w);
        if (w->widgets[gHoverWidget.widget_index].type == WindowWidgetType::FlatBtn)
        {
            widget_invalidate_by_number(
                gHoverWidget.window_classification, gHoverWidget.window_number, gHoverWidget.widget_index);
        }
    }
}

/**
 *
 *  rct2: 0x006E95F9
 */
static void InputWidgetLeft(const ScreenCoordsXY& screenCoords, rct_window* w, WidgetIndex widgetIndex)
{
    WindowClass windowClass = WindowClass::Null;
    rct_windownumber windowNumber = 0;

    if (w != nullptr)
    {
        windowClass = w->classification;
        windowNumber = w->number;
    }

    window_close_by_class(WindowClass::Error);
    window_close_by_class(WindowClass::Tooltip);

    // Window might have changed position in the list, therefore find it again
    w = window_find_by_number(windowClass, windowNumber);
    if (w == nullptr)
        return;

    w = window_bring_to_front(*w);
    if (widgetIndex == -1)
        return;

    if (windowClass != gCurrentTextBox.window.classification || windowNumber != gCurrentTextBox.window.number
        || widgetIndex != gCurrentTextBox.widget_index)
    {
        window_cancel_textbox();
    }

    const auto& widget = w->widgets[widgetIndex];

    switch (widget.type)
    {
        case WindowWidgetType::Frame:
        case WindowWidgetType::Resize:
            if (window_can_resize(*w)
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
                w = window_find_by_number(gCurrentToolWidget.window_classification, gCurrentToolWidget.window_number);
                if (w != nullptr)
                {
                    window_event_tool_down_call(w, gCurrentToolWidget.widget_index, screenCoords);
                    _inputFlags |= INPUT_FLAG_4;
                }
            }
            break;
        case WindowWidgetType::Caption:
            InputWindowPositionBegin(*w, widgetIndex, screenCoords);
            break;
        case WindowWidgetType::Scroll:
            InputScrollBegin(*w, widgetIndex, screenCoords);
            break;
        default:
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

                widget_invalidate_by_number(windowClass, windowNumber, widgetIndex);
                window_event_mouse_down_call(w, widgetIndex);
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
    rct_window* window;

    CursorID cursorId = CursorID::Arrow;
    auto ft = Formatter();
    ft.Add<StringId>(STR_NONE);
    SetMapTooltip(ft);
    window = window_find_from_point(screenCoords);

    if (window != nullptr)
    {
        WidgetIndex widgetId = window_find_widget_from_point(*window, screenCoords);
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
                    cursorId = window_event_cursor_call(window, widgetId, scrollCoords);
                    if (cursorId == CursorID::Undefined)
                        cursorId = CursorID::Arrow;
                    break;
                }
                default:
                    cursorId = window_event_cursor_call(window, widgetId, screenCoords);
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
        rct_window* w = window_find_by_number(gCurrentToolWidget.window_classification, gCurrentToolWidget.window_number);

        if (w == nullptr)
            tool_cancel();
        else if (input_get_state() != InputState::ViewportRight)
            window_event_tool_update_call(w, gCurrentToolWidget.widget_index, screenCoords);
    }
}

/**
 *
 *  rct2: 0x006E8DA7
 */
void InputStateWidgetPressed(
    const ScreenCoordsXY& screenCoords, MouseState state, WidgetIndex widgetIndex, rct_window* w, rct_widget* widget)
{
    WindowClass cursor_w_class;
    rct_windownumber cursor_w_number;
    cursor_w_class = gPressedWidget.window_classification;
    cursor_w_number = gPressedWidget.window_number;
    WidgetIndex cursor_widgetIndex = gPressedWidget.widget_index;

    rct_window* cursor_w = window_find_by_number(cursor_w_class, cursor_w_number);
    if (cursor_w == nullptr)
    {
        _inputState = InputState::Reset;
        return;
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
                        window_event_mouse_down_call(w, widgetIndex);
                    }
                }
            }

            if (_inputFlags & INPUT_FLAG_WIDGET_PRESSED)
            {
                if (_inputState == InputState::DropdownActive)
                {
                    gDropdownHighlightedIndex = gDropdownDefaultIndex;
                    window_invalidate_by_class(WindowClass::Dropdown);
                }
                return;
            }

            _inputFlags |= INPUT_FLAG_WIDGET_PRESSED;
            widget_invalidate_by_number(cursor_w_class, cursor_w_number, widgetIndex);
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

                    window_close_by_class(WindowClass::Dropdown);

                    if (dropdownCleanup)
                    {
                        // Update w as it will be invalid after closing the dropdown window
                        w = window_find_by_number(wClass, wNumber);
                    }
                    else
                    {
                        cursor_w = window_find_by_number(cursor_w_class, cursor_w_number);
                        if (_inputFlags & INPUT_FLAG_WIDGET_PRESSED)
                        {
                            _inputFlags &= ~INPUT_FLAG_WIDGET_PRESSED;
                            widget_invalidate_by_number(cursor_w_class, cursor_w_number, cursor_widgetIndex);
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
                        window_event_dropdown_call(cursor_w, cursor_widgetIndex, dropdown_index);
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

            widget_invalidate_by_number(cursor_w_class, cursor_w_number, widgetIndex);
            window_event_mouse_up_call(w, widgetIndex);
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
            widget_invalidate_by_number(cursor_w_class, cursor_w_number, cursor_widgetIndex);
        }
        return;
    }

    gDropdownHighlightedIndex = -1;
    window_invalidate_by_class(WindowClass::Dropdown);
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
            };
            WindowTooltipShow(OpenRCT2String{ _colourTooltips[dropdown_index], {} }, screenCoords);
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
        window_invalidate_by_class(WindowClass::Dropdown);
    }
    else
    {
        gDropdownLastColourHover = -1;
        WindowTooltipClose();
    }
}

static void InputUpdateTooltip(rct_window* w, WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords)
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
        reset_tooltip_not_shown();

        if (w == nullptr || gTooltipWidget.window_classification != w->classification
            || gTooltipWidget.window_number != w->number || gTooltipWidget.widget_index != widgetIndex
            || !WidgetIsVisible(*w, widgetIndex))
        {
            WindowTooltipClose();
        }

        gTooltipTimeout += gCurrentDeltaTime;
        if (gTooltipTimeout >= 8000)
        {
            window_close_by_class(WindowClass::Tooltip);
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
    uint8_t* keysPressed = const_cast<uint8_t*>(context_get_keys_pressed());
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
    context_setcurrentcursor(cursor_id);
}

/**
 *
 *  rct2: 0x006E876D
 */
void InvalidateScroll()
{
    rct_window* w = window_find_by_number(gPressedWidget.window_classification, gPressedWidget.window_number);
    if (w != nullptr)
    {
        // Reset to basic scroll
        w->scrolls[_currentScrollIndex].flags &= 0xFF11;
        window_invalidate_by_number(gPressedWidget.window_classification, gPressedWidget.window_number);
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
    rct_window* mainWindow;
    int32_t scrollX, scrollY;

    mainWindow = window_get_main();
    if (mainWindow == nullptr)
        return;
    if ((mainWindow->flags & WF_NO_SCROLLING) || (gScreenFlags & (SCREEN_FLAGS_TRACK_MANAGER | SCREEN_FLAGS_TITLE_DEMO)))
        return;
    if (mainWindow->viewport == nullptr)
        return;
    if (!context_has_focus())
        return;

    scrollX = 0;
    scrollY = 0;

    // Scroll left / right
    const CursorState* cursorState = context_get_cursor_state();
    if (cursorState->position.x == 0)
        scrollX = -1;
    else if (cursorState->position.x >= context_get_width() - 1)
        scrollX = 1;

    // Scroll up / down
    if (cursorState->position.y == 0)
        scrollY = -1;
    else if (cursorState->position.y >= context_get_height() - 1)
        scrollY = 1;

    InputScrollViewport(ScreenCoordsXY(scrollX, scrollY));
}

bool InputTestPlaceObjectModifier(PLACE_OBJECT_MODIFIER modifier)
{
    return gInputPlaceObjectModifier & modifier;
}

void InputScrollViewport(const ScreenCoordsXY& scrollScreenCoords)
{
    rct_window* mainWindow = window_get_main();
    rct_viewport* viewport = mainWindow->viewport;

    const int32_t speed = gConfigGeneral.edge_scrolling_speed;

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

        auto mapCoord = viewport_coord_to_map_coord({ x, y }, 0);
        auto mapCoord_dy = viewport_coord_to_map_coord({ x, y_dy }, 0);

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
